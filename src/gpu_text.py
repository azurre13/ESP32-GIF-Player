import serial
import time
import GPUtil

# --- GANTI SESUAI COM PORT ESP32 KAMU ---
# (Cek di pojok kanan bawah VS Code, misalnya COM3, COM5, dll)
SERIAL_PORT = 'COM5' 
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    # Ini buat nge-reset jalur serial biar ESP32 siap nerima data
    ser.setDTR(False)
    ser.setRTS(False)
    time.sleep(1)
    print(f"Berhasil nyambung ke {SERIAL_PORT}!")
except Exception as e:
    print(f"Gagal buka port {SERIAL_PORT}! Pastikan Serial Monitor di VS Code ditutup. Error: {e}")
    exit()

print("Mulai mengirim data RTX 3060 ke layar... (Tekan Ctrl+C di terminal untuk stop)")

while True:
    try:
        gpus = GPUtil.getGPUs()
        if gpus:
            gpu = gpus[0]
            # Ambil Load dan Temp, jadikan angka bulat
            load = int(gpu.load * 100)
            temp = int(gpu.temperature)
            
            # Format paket data: "LOAD,TEMP\n" (contoh: "45,60\n")
            data_string = f"{load},{temp}\n"
            ser.write(data_string.encode('utf-8'))
            ser.flush()
            
            print(f"Terkirim -> Load GPU: {load}% | Temp GPU: {temp}C")
        else:
            print("Waduh, GPU tidak terdeteksi!")
            
        time.sleep(1) # Update angka tiap 1 detik (Biar hemat daya & gak bikin layar semut)
        
    except KeyboardInterrupt:
        print("\nProgram dimatikan.")
        ser.close()
        break
    except Exception as e:
        print(f"Error: {e}")
        time.sleep(1)