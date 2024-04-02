# Read the file and plot the sound
from matplotlib import pyplot as plt
import numpy as np
from glob import glob
from numba import njit


@njit
def moving_average(signal, window=100):
    out = np.ones_like(signal) * np.nan
    for i in range(window, len(signal)):
        out[i] = np.mean(signal[i - window // 2 : i + window // 2])
    return out


filename = "./recordings/hz_gen_1200_at_20000.wav"  # <-- This was recorded at 20000 Hz, the others were recorded at 44100 Hz
# filename = sorted(glob("./recordings/output_*.wav"))[-1]
# print(f"Reading file: {filename}")

SAMPLE_RATE = 20000

sound = np.frombuffer(open(filename, "rb").read(), dtype=np.uint8)
sound = sound - 128
sound = sound / 128

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

fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(moving_average(sound, 100), label="Moving average (window=100)")
ax.plot(moving_average(sound, 500), label="Moving average (window=500)")
ax.plot(moving_average(sound, 1000), label="Moving average (window=1000)")
ax.legend()
ax.grid()
# ax.set_xlabel("Time (s)")
plt.tight_layout()


# -------------
# FFT
# -------------

# Original
fft_sound = np.fft.fft(sound - np.nanmean(sound))
fft_freq = np.fft.fftfreq(len(sound), d=1 / SAMPLE_RATE)

fft_sound = 2 * np.abs(fft_sound[: len(sound) // 2]) / len(sound)
fft_freq = fft_freq[: len(sound) // 2]


fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(fft_freq, fft_sound)
ax.grid()
ax.set_xlabel("Frequency (Hz)")
plt.tight_layout()

# Moving average
sound_ma = moving_average(sound - np.nanmean(sound), 10)
sound_ma = sound_ma[~np.isnan(sound_ma)]
fft_sound = np.fft.fft(sound_ma)
fft_freq = np.fft.fftfreq(len(sound_ma), d=1 / SAMPLE_RATE)

fft_sound = 2 * np.abs(fft_sound[: len(sound_ma) // 2]) / len(sound_ma)
fft_freq = fft_freq[: len(sound_ma) // 2]


fig = plt.figure(figsize=(12, 4))
ax = fig.add_subplot(111)
ax.plot(fft_freq, fft_sound)
ax.grid()
ax.set_xlabel("Frequency (Hz)")
plt.tight_layout()
plt.show()
