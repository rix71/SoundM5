# Read the file and plot the sound
from matplotlib import pyplot as plt
import numpy as np

WAVE_OUTPUT_FILENAME = "output.wav"

sound = np.frombuffer(open(WAVE_OUTPUT_FILENAME, "rb").read(), dtype=np.uint8)
sound = sound - 128
sound = sound / 128
sound = sound.astype(np.float32)

print(len(sound))

fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(sound)
ax.grid()
# ax.set_xlabel("Time (s)")
plt.tight_layout()

fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(sound - np.nanmean(sound))
ax.grid()
# ax.set_xlabel("Time (s)")
plt.tight_layout()

# -------------
# FFT
# -------------
fft_sound = np.fft.fft(sound - np.nanmean(sound))
fft_freq = np.fft.fftfreq(len(sound), d=1 / 44100)

fft_sound = np.abs(fft_sound) / len(sound)

fft_sound = fft_sound[: len(fft_sound) // 2]
fft_freq = fft_freq[: len(fft_freq) // 2]

fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(fft_freq, fft_sound)
ax.grid()
ax.set_xlabel("Frequency (Hz)")
plt.tight_layout()

plt.show()
