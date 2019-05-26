import pyaudio
import wave
import numpy as np
import serial

chunk = 48000
length = 48000
fs = 48000
Format = pyaudio.paInt16
Channels = 2
soundspeed = 340
micdistance = 0.1



ser = serial.Serial('/dev/ttyACM0',115200)


hanning = np.hanning(length)

p = pyaudio.PyAudio()

stream = p.open(format=Format,
                channels=Channels,
                rate=fs,
                input=True,
                frames_per_buffer=chunk)


def rec():

    frames = []
    num_frames = []

    data = stream.read(chunk)
    frames.append(data)

    num_frames.append(np.frombuffer(data, dtype='int16').reshape(
        (chunk, Channels)) / float(chunk))
    num_data = np.frombuffer(data, dtype='int16').reshape(
        (chunk, Channels)) / float(chunk)

    ch1 = num_data[:, 0]
    ch2 = num_data[:, 1]
    
    zarray = np.zeros(length - chunk)
    ch1 = np.concatenate([ch1, zarray])
    ch2 = np.concatenate([ch2, zarray])

    fft1 = np.fft.fft(ch1 * hanning)
    fft2 = np.fft.fft(ch2 * hanning)

    xcor = fft1 * np.conj(fft2) / (abs(fft1) * abs(fft2))

    for i in range(chunk):
        if i > 2000 or i < 200:
            xcor[i] = 0

    csp = np.fft.ifft(xcor)

    if np.argmax(csp) > chunk / 2:
        delay = float(np.argmax(csp) - chunk) / fs
    else:
        delay = float(np.argmax(csp)) / fs

    print(np.argmax(csp))

    theta = np.arcsin(delay * soundspeed / micdistance) / np.pi

    print(theta)
    
    Abs1 = abs(fft1[200:2000])
    Abs2 = abs(fft2[200:2000])
    
    if Abs1[np.argmax(Abs1)] > Abs2[np.argmax(Abs2)]:
        maxi = Abs1[np.argmax(Abs1)]
    else:
        maxi = Abs2[np.argmax(Abs2)]

    print(maxi)
    throw = 0

    if maxi < 50:
        throw = 12
    elif theta >= -0.5 and theta < -0.27:
        throw = 6
        ser.write(b"6")
    elif theta >= -0.27 and theta < -0.125:
        throw = 5
        ser.write(b"5")
    elif theta >= -0.125 and theta < 0.125:
        throw = 4
        ser.write(b"4")
    elif theta >= 0.125 and theta < 0.27:
        throw = 3
        ser.write(b"3")
    elif theta >= 0.27 and theta <= 0.5:
        throw = 2
        ser.write(b"2")

    print(throw)



if __name__ == "__main__":
    while True:
        rec()

    stream.stop_stream()
    stream.close()
    p.terminate()

