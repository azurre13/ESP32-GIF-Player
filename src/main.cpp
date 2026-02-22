#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <AnimatedGIF.h>

// Panggil file wadah GIF tadi
#include "gif_data.h"

// --- PINOUT S3 N16R8 KE ST7789 (7-PIN) ---
#define TFT_DC   2
#define TFT_RST  4
#define TFT_CS  -1  // CS gak dipake di 7-pin
#define TFT_SDA 16  // Pin Data (MOSI)
#define TFT_SCL 17  // Pin Clock (SCK)

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCL, TFT_SDA);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 0, true, 240, 240);

AnimatedGIF gif;

// --- FUNGSI 'MAGIC' PENGGAMBAR GIF DARI TUTORIAL ---
void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > 240) iWidth = 240; 

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; 

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) { 
    for (x = 0; x < iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  if (pDraw->ucHasTransparency) {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int iCount;
    pEnd = s + iWidth;
    x = 0;
    while (s < pEnd) {
      c = *s++;
      if (c == ucTransparent) {
        s--; s++; iCount = 1;
        while (s < pEnd && *s == ucTransparent) { s++; iCount++; }
        x += iCount;
      } else {
        s--; iCount = 1;
        while (s < pEnd && *s != ucTransparent) { s++; iCount++; }
        for (int i=0; i<iCount; i++) usTemp[i] = usPalette[*(s - iCount + i)];
        gfx->draw16bitRGBBitmap(pDraw->iX + x, y, usTemp, iCount, 1);
        x += iCount;
      }
    }
  } else {
    s = pDraw->pPixels;
    for (x = 0; x < iWidth; x++) usTemp[x] = usPalette[*s++];
    gfx->draw16bitRGBBitmap(pDraw->iX, y, usTemp, iWidth, 1);
  }
}

void setup() {
  Serial.begin(115200);

  // Nyalakan Layar
  gfx->begin();
  gfx->fillScreen(BLACK);
  
  // Tulis pesan tes
  gfx->setTextColor(GREEN);
  gfx->setTextSize(2);
  gfx->setCursor(20, 110);
  gfx->println("SYSTEM READY");
  gfx->setCursor(20, 140);
  gfx->setTextSize(1);
  gfx->setTextColor(WHITE);
  gfx->println("(Menunggu GIF Asli...)");

  // Init Library GIF
  gif.begin(LITTLE_ENDIAN_PIXELS);
}

void loop() {
  // Mainkan GIF dari Memory
  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw)) {
    while (gif.playFrame(true, NULL)) {
      // Looping frame
    }
    gif.close();
  } else {
    // Kalau gagal (karena masih file dummy), diam aja
    delay(1000);
  }
}