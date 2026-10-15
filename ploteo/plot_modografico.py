import serial, time
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import welch

PORT, BAUD, FS, DUR = "COM4", 921600, 1000, 15   # 15 s de registro

# ---------- Captura ----------
# ---------- Captura ----------
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)                    # el ESP32 se reinicia al abrir el puerto
ser.reset_input_buffer()

print("Esperando datos del ESP32...")
crudo, filt = [], []
mostradas, descartadas = 0, 0
t0 = time.time()
while len(crudo) < FS * DUR:
    linea = ser.readline().decode(errors="ignore").strip()
    if mostradas < 5 and linea:
        print("  recibido:", repr(linea))
        mostradas += 1
    try:
        a, b = linea.split(",")
        crudo.append(float(a))
        filt.append(float(b))
        if len(crudo) % FS == 0:
            print(f"  {len(crudo)//FS} s / {DUR} s")
    except ValueError:
        descartadas += 1
    if not crudo and time.time() - t0 > 5:
        ser.close()
        raise SystemExit("No llegan datos validos en 5 s. Revisa: sketch con "
                         "MODO_GRAFICO 1 subido, baudios y puerto.")
ser.close()
print(f"Listo ({descartadas} lineas descartadas)")

crudo, filt = np.array(crudo), np.array(filt)
crudo, filt = np.array(crudo), np.array(filt)
np.savetxt("emg_datos.csv", np.column_stack([crudo, filt]),
           delimiter=",", header="crudo_adc,filtrado_adc", comments="")

# ---------- Preparación ----------
ini = FS                                   # descarta el 1er segundo (transitorio de los filtros)
crudo, filt = crudo[ini:], filt[ini:]
t = np.arange(len(crudo)) / FS
k = 3.3 / 4095                             # cuentas -> voltios en el pin del ADC

ventana = int(0.1 * FS)                    # RMS en ventana de 100 ms
env = np.sqrt(np.convolve(filt**2, np.ones(ventana) / ventana, mode="same"))

# ---------- Figura 1: señal en el tiempo ----------
fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
ax[0].plot(t, crudo * k, lw=0.5)
ax[0].set_ylabel("V"); ax[0].set_title("Señal cruda (pin ADC)")
ax[1].plot(t, filt * k * 1000, lw=0.5)
ax[1].set_ylabel("mV"); ax[1].set_title("Señal filtrada (20-400 Hz + notch 50 Hz)")
ax[2].plot(t, env * k * 1000)
ax[2].set_ylabel("mV"); ax[2].set_title("Envolvente RMS (ventana 100 ms)")
ax[2].set_xlabel("Tiempo (s)")
plt.tight_layout()
plt.savefig("emg_signal.pdf"); plt.savefig("emg_signal.png", dpi=300)

# ---------- Figura 2 (opcional): espectro, muestra el efecto del filtro ----------
f1, p1 = welch(crudo - crudo.mean(), FS, nperseg=1024)
f2, p2 = welch(filt, FS, nperseg=1024)
plt.figure(figsize=(8, 4))
plt.semilogy(f1, p1, label="Cruda")
plt.semilogy(f2, p2, label="Filtrada")
plt.xlabel("Frecuencia (Hz)"); plt.ylabel("Densidad espectral")
plt.legend(); plt.tight_layout()
plt.savefig("emg_espectro.pdf")

plt.show()