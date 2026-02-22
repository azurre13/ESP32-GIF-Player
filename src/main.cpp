#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <AnimatedGIF.h>
#include "gif_data.h"

#define TFT_DC   2
#define TFT_RST  4
#define TFT_CS  -1
#define TFT_SDA 16
#define TFT_SCL 17

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCL, TFT_SDA);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 0, true, 240, 240);
AnimatedGIF gif;

int currentLoad = 0;
int currentTemp = 0;
int lastLoad = -1;  // Buat ngecek kapan angka berubah
int lastTemp = -1;
int activeGif = -1;

// --- TRIK DEWA: POTONG GIF BUAT TEMPAT TEKS ---
void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth > 240) iWidth = 240;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y;
    
    // JANGAN gambar pixel GIF di area Y 0 sampai 40 (Biar teks gak tertimpa!)
    if (y <= 40) return; 

    s = pDraw->pPixels;
    for (x = 0; x < iWidth; x++) usTemp[x] = usPalette[*s++];
    gfx->draw16bitRGBBitmap(pDraw->iX, y, usTemp, iWidth, 1);
}

void setup() {
    Serial.begin(115200);
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, LOW); delay(200);
    digitalWrite(TFT_RST, HIGH); delay(200);

    gfx->begin();
    gfx->fillScreen(BLACK);
    gif.begin(LITTLE_ENDIAN_PIXELS);
}

void loop() {
    // 1. TERIMA DATA LEBIH AMAN DARI PC
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        data.trim(); // Bersihkan sisa karakter aneh
        int commaIndex = data.indexOf(',');
        
        if (commaIndex > 0) {
            String loadStr = data.substring(0, commaIndex);
            String tempStr = data.substring(commaIndex + 1);
            
            // Validasi: Pastikan data benar-benar angka, bukan kosong
            if (loadStr.length() > 0 && tempStr.length() > 0) {
                currentLoad = loadStr.toInt();
                currentTemp = tempStr.toInt();
            }
        }
    }

    // 2. GANTI GIF (>= 50% Ever, < 50% Miyabi)
    int targetGif = (currentLoad >= 50) ? 1 : 0;
    if (targetGif != activeGif) {
        if (activeGif != -1) gif.close();
        activeGif = targetGif;
        
        if (activeGif == 1) {
            gif.open((uint8_t *)GIF_EVER, GIF_EVER_size, GIFDraw);
        } else {
            gif.open((uint8_t *)GIF_MIYABI, GIF_MIYABI_size, GIFDraw);
        }
    }

    // 3. PUTAR GIF
    int result = gif.playFrame(true, NULL);
    if (result <= 0) gif.reset();

    // 4. UPDATE TEKS HANYA KALAU ANGKA BERUBAH
    if (currentLoad != lastLoad || currentTemp != lastTemp) {
        // Bersihkan kotak atas pakai warna hitam
        gfx->fillRect(0, 0, 240, 40, BLACK);
        
        gfx->setTextSize(2);
        
        // Teks Load
        gfx->setTextColor(GREEN);
        gfx->setCursor(5, 5);
        gfx->print("GPU:"); gfx->print(currentLoad); gfx->print("% "); 
        
        // Teks Temp
        gfx->setTextColor(CYAN);
        gfx->setCursor(5, 25);
        gfx->print("TMP:"); gfx->print(currentTemp); gfx->print("C ");
        
        lastLoad = currentLoad;
        lastTemp = currentTemp;
    }
}