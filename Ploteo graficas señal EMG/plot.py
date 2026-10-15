import serial, time
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, sosfiltfilt, iirnotch, filtfilt, welch

PORT, BAUD, FS, DUR = "COM4", 921600, 1000, 15   # 15 s de registro

# ---------- Captura ----------
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)                    # el ESP32 se reinicia al abrir el puerto
ser.reset_input_buffer()

print("Grabando: reposo -> contrae -> relaja (3 ciclos)")
crudo = []
descartadas = 0
t0 = time.time()
t_ini = None
while len(crudo) < FS * DUR:
    linea = ser.readline().decode(errors="ignore").strip()
    partes = linea.split(",")
    try:
        if len(partes) not in (1, 2):
            raise ValueError
        crudo.append(float(partes[0]))      # la 1ª columna es siempre el valor crudo
        if t_ini is None:
            t_ini = time.time()
        if len(crudo) % FS == 0:
            print(f"  {len(crudo)//FS} s / {DUR} s")
    except ValueError:
        descartadas += 1
    if not crudo and time.time() - t0 > 5:
        ser.close()
        raise SystemExit("No llegan datos validos en 5 s. Revisa puerto y baudios.")
ser.close()

fs_real = len(crudo) / (time.time() - t_ini)
print(f"Listo ({descartadas} lineas descartadas). Frecuencia efectiva: {fs_real:.0f} Hz")
if abs(fs_real - FS) > 0.05 * FS:
    print("AVISO: la frecuencia efectiva difiere de 1 kHz; el eje de tiempo y los filtros serian inexactos.")

crudo = np.array(crudo)
np.savetxt("emg_datos.csv", crudo, header="crudo_adc", comments="")

# ---------- Procesamiento ----------
k = 3.3 / 4095                       # cuentas -> voltios en el pin del ADC
v = crudo * k
x = v - v.mean()                     # quita el offset DC

sos = butter(4, [20, 400], btype="band", fs=FS, output="sos")
filt = sosfiltfilt(sos, x)           # pasa-banda 20-400 Hz (fase cero)
b, a = iirnotch(50, 30, fs=FS)       # red de 50 Hz
filt = filtfilt(b, a, filt)

ventana = int(0.1 * FS)              # RMS en ventana de 100 ms
env = np.sqrt(np.convolve(filt**2, np.ones(ventana) / ventana, mode="same"))

ini = FS                             # descarta el 1er segundo (arranque)
t = np.arange(len(v) - ini) / FS
v, filt, env = v[ini:], filt[ini:], env[ini:]

# ---------- Figura 1: señal en el tiempo ----------
fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
ax[0].plot(t, v, lw=0.5)
ax[0].set_ylabel("V"); ax[0].set_title("Señal cruda (pin ADC)")
ax[1].plot(t, filt * 1000, lw=0.5)
ax[1].set_ylabel("mV"); ax[1].set_title("Señal filtrada (20-400 Hz + notch 50 Hz)")
ax[2].plot(t, env * 1000)
ax[2].set_ylabel("mV"); ax[2].set_title("Envolvente RMS (ventana 100 ms)")
ax[2].set_xlabel("Tiempo (s)")
plt.tight_layout()
plt.savefig("emg_signal.pdf"); plt.savefig("emg_signal.png", dpi=300)

# ---------- Figura 2: espectro ----------
f1, p1 = welch(v - v.mean(), FS, nperseg=1024)
f2, p2 = welch(filt, FS, nperseg=1024)
plt.figure(figsize=(8, 4))
plt.semilogy(f1, p1, label="Cruda")
plt.semilogy(f2, p2, label="Filtrada")
plt.xlabel("Frecuencia (Hz)"); plt.ylabel("Densidad espectral")
plt.legend(); plt.tight_layout()
plt.savefig("emg_espectro.pdf")

plt.show()