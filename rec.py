import pyaudio
import wave
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 48000
RECORD_SECONDS = 1
WAVE_OUTPUT_FILENAME = "output.wav"

p = pyaudio.PyAudio()

stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

print("* recording")

frames = []
num_frames = []

for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
    data = stream.read(CHUNK)
    frames.append(data)
    num_data = np.frombuffer(data, dtype='int16').reshape((CHUNK, CHANNELS)) / float(2 ** 15)
    num_frames.append(np.frombuffer(data, dtype='int16').reshape((CHUNK, CHANNELS)) / float(2 ** 15))
    ch1 = num_data[:,0]
    ch2 = num_data[:,1]

print("* done recording")
stream.stop_stream()
stream.close()
p.terminate()

x = np.concatenate(num_frames)
print(x.shape)

fill = np.zeros(3000)

for i in range(1500):
    fill[500 + i] = 1


for i in range(CHANNELS):
    #plt.subplot(CHANNELS, 1, i + 1)
    wave = x[:,i]
    wave = wave[:3000:]
    fft = np.fft.fft(wave) * fill
    plt.plot(wave)

plt.savefig('figure.png')

print("write data")
