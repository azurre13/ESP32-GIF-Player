import os
import io
from PIL import Image, ImageSequence

# --- CUMAN GANTI 1 PATH INI AJA ---
GIF_PATH = r"C:\Users\daffa\Documents\GIF_Project_S3\miyabi.gif"

# Otomatis langsung masuk ke project PlatformIO kamu
OUT_PATH = r"C:\Users\daffa\Documents\GIF_Project_S3\src\gif_data.h"

print(f"Membaca dan otomatis membesarkan {GIF_PATH} ke 240x240...")

try:
    img = Image.open(GIF_PATH)
    frames = []
    duration = img.info.get('duration', 100)
    
    # 1. Paksa Resize ke 240x240 (Full Screen)
    for frame in ImageSequence.Iterator(img):
        frames.append(frame.resize((240, 240), Image.Resampling.LANCZOS))
        
    # 2. Simpan GIF baru di "Memori Sementara" (biar gak menuhin laptop)
    temp_gif = io.BytesIO()
    frames[0].save(temp_gif, format="GIF", save_all=True, append_images=frames[1:], loop=0, duration=duration)
    gif_data = temp_gif.getvalue()
    
    print(f"Ukuran file: {len(gif_data) / 1024:.2f} KB. Sedang menulis ke gif_data.h...")
    
    # 3. Langsung jadikan file Hex .h
    with open(OUT_PATH, "w") as f:
        f.write("#ifndef GIF_DATA_H\n#define GIF_DATA_H\n\n")
        f.write("#include <pgmspace.h>\n\n")
        f.write("// File ini di-generate otomatis (Sudah Full Screen 240x240)\n")
        f.write("const uint8_t GIF_IMAGE[] PROGMEM = {\n")
        
        for i, byte in enumerate(gif_data):
            f.write(f"0x{byte:02X}, ")
            if (i + 1) % 16 == 0:
                f.write("\n")
                
        f.write("\n};\n\n#endif\n")

    print(f"BERHASIL! File gif_data.h sudah diperbarui.")
    print("Silakan buka VS Code dan langsung klik UPLOAD!")

except Exception as e:
    print(f"Error: {e}")