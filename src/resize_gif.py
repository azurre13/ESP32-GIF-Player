from PIL import Image, ImageSequence

# Path GIF asli yang kecil
INPUT_GIF = r"C:\Users\daffa\Documents\GIF_Project_S3\ever.gif"
# Nama GIF baru yang udah digedein
OUTPUT_GIF = r"C:\Users\daffa\Documents\GIF_Project_S3\ever_240.gif"

try:
    img = Image.open(INPUT_GIF)
    frames = []
    
    print("Sedang memperbesar GIF ke 240x240...")
    
    # Ambil durasi asli biar speed-nya gak berubah
    duration = img.info.get('duration', 100)
    
    # Loop tiap frame dan perbesar
    for frame in ImageSequence.Iterator(img):
        # Resize paksa ke 240x240 (Abaikan rasio biar full screen)
        resized = frame.resize((240, 240), Image.Resampling.LANCZOS)
        frames.append(resized)
        
    # Simpan jadi GIF baru
    frames[0].save(
        OUTPUT_GIF,
        save_all=True,
        append_images=frames[1:],
        loop=0,
        duration=duration
    )
    
    print(f"MANTAP! File baru berhasil dibuat: {OUTPUT_GIF}")

except Exception as e:
    print(f"Error: {e}")