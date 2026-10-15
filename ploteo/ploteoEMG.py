import serial, numpy as np, matplotlib.pyplot as plt
import time
from scipy.signal import butter, sosfiltfilt, iirnotch, filtfilt

PORT, BAUD, FS, DUR = "COM4", 921600, 1000, 10   # 10 s de registro

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()
data = []
while len(data) < FS * DUR:
    line = ser.readline().decode(errors="ignore").strip()
    if line.isdigit():
        data.append(int(line))
ser.close()

raw = np.array(data, float)
volts = raw / 4095 * 3.3
x = volts - volts.mean()                          # quita offset DC

sos = butter(4, [20, 450], btype="band", fs=FS, output="sos")
filt = sosfiltfilt(sos, x)                        # pasa-banda 20-450 Hz
b, a = iirnotch(50, 30, fs=FS)                    # red eléctrica de 50 Hz (Chile)
filt = filtfilt(b, a, filt)

rect = np.abs(filt)                               # rectificación onda completa
env = sosfiltfilt(butter(2, 5, fs=FS, output="sos"), rect)   # envolvente

t = np.arange(len(raw)) / FS
fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
ax[0].plot(t, volts);  ax[0].set_title("Señal cruda (V)")
ax[1].plot(t, filt);   ax[1].set_title("Filtrada 20-450 Hz + notch 50 Hz")
ax[2].plot(t, env);    ax[2].set_title("Rectificada + envolvente (5 Hz)")
ax[2].set_xlabel("Tiempo (s)")
plt.tight_layout(); plt.show()