import os
import io
from PIL import Image, ImageSequence

# --- PASTIKAN 2 FILE INI ADA DI LAPTOP KAMU ---
GIF_1 = r"C:\Users\daffa\Documents\GIF_Project_S3\ever.gif"
GIF_2 = r"C:\Users\daffa\Documents\GIF_Project_S3\miyabi.gif"

OUT_PATH = r"C:\Users\daffa\Documents\GIF_Project_S3\src\gif_data.h"

def convert_gif_to_hex(path, var_name):
    print(f"Memproses {path} menjadi {var_name}...")
    try:
        img = Image.open(path)
    except FileNotFoundError:
        print(f"GAGAL: File {path} tidak ditemukan!")
        exit()
        
    frames = []
    duration = img.info.get('duration', 100)
    
    # Paksa Full Screen 240x240
    for frame in ImageSequence.Iterator(img):
        frames.append(frame.resize((240, 240), Image.Resampling.LANCZOS))
        
    temp_gif = io.BytesIO()
    frames[0].save(temp_gif, format="GIF", save_all=True, append_images=frames[1:], loop=0, duration=duration)
    data = temp_gif.getvalue()
    
    out = f"const uint8_t {var_name}[] PROGMEM = {{\n"
    for i, byte in enumerate(data):
        out += f"0x{byte:02X}, "
        if (i + 1) % 16 == 0:
            out += "\n"
    out += "\n};\n"
    out += f"const int {var_name}_size = {len(data)};\n\n"
    return out

print("Memulai penggabungan 2 GIF...")

with open(OUT_PATH, "w") as f:
    f.write("#ifndef GIF_DATA_H\n#define GIF_DATA_H\n\n")
    f.write("#include <pgmspace.h>\n\n")
    
    f.write(convert_gif_to_hex(GIF_1, "GIF_EVER"))
    f.write(convert_gif_to_hex(GIF_2, "GIF_MIYABI"))
    
    f.write("#endif\n")

print(f"SUKSES DEWA! 2 GIF berhasil ditanam ke dalam gif_data.h!")