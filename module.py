from __future__ import print_function
import numpy as np
import pyaudio
import wave
import serial
import os
import sys
import socket
from contextlib import closing


#detect const about record
CHUNK = 1024
FORMAT = pyaudio.paFloat32
CHANNELS = 2
RATE = 48000
INDEX = 0

#detect const about processing
LENGTH = 48000
SOUND_SPEED = 340
DISTANCE = 0.1

#detect variable about processing
dt = 1 / RATE
space = np.zeros(LENGTH - CHUNK)
hanning = np.concatenate([np.hanning(CHUNK), space])

#prepareing arduino
ser = serial.Serial('/dev/ttyACM0',115200)

#preparing record using pyaudio 
p = pyaudio.PyAudio()

stream = p.open(format=FORMAT,
                channels=CHANNELS,
                input_device_index=INDEX,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

def rec():
    #record sound with 2 channel
    frames = []
    data = stream.read(CHUNK)
    frames.append(data)
    data = b''.join(frames)
    
    #extract sound values
    rec = np.frombuffer(data, dtype="float32")
    rec0 = rec[0::2]
    rec1 = rec[1::2]

    return rec0, rec1


def csp(spectrum0, spectrum1):

    xcor = np.conj(np.array(spectrum0)) * np.array(spectrum1) / (np.abs(np.array(spectrum0)) * np.abs(np.array(spectrum1)))
    
    #band pass filter
    for i in range(xcor.shape[0]):
        if i > 2000 or i < 200:
            xcor[i] = 0

    csp = np.fft.ifft(xcor)

    return csp

def select_angle(angle):
    if angle >= -0.5 and angle < -0.27:
        num = 6
    elif angle >= -0.27 and angle < -0.125:
        num = 5
    elif angle >= -0.125 and angle < 0.125:
        num = 4
    elif angle >= 0.125 and angle < 0.27:
        num = 3
    elif angle >= 0.27 and angle <= 0.5:
        num = 2

    return num

def select_vibration(num, keyword):
    if keyword == "name":
        num = num + 8
    elif keyword == "care":
        num = num + 16
    elif keyword == "pupu":
        num = num + 24
    elif keyword == "excuse":
        num = num + 32
    elif keyword == "hi":
        num = num + 40
    else:
        num = 1

    return num




def main():

    #record
    rec0, rec1 = rec()

    #zero padding
    wave1 = np.concatenate([rec0, space])
    wave2 = np.concatenate([rec1, space])

    #fft wave
    fft0 = np.fft.fft(wave1 * hanning)
    fft1 = np.fft.fft(wave2 * hanning)

    #csp method
    angle = csp(fft0, fft1)

    #estimate direction of arrival
    if np.argmax(angle.real) > LENGTH/2:
        delay = (np.argmax(angle.real) - LENGTH) * dt
    else:
        delay = np.argmax(angle.real) * dt

    theta = np.arcsin(delay * SOUND_SPEED / DISTANCE) / np.pi

    #print theta
    print(theta)

    if delay > 50:
        throw = 1
    else:
        throw = select_angle(theta)
        throw = select_vibration(throw)

        ser.write(str(throw).encode())  

    print(throw)

    

if __name__ == '__main__':

    host = '127.0.0.1'
    port = 10500
    bufsize = 4096
    b = ""
    index = -1

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    with closing(sock):
        sock.connect((host, port))
    
    while True:
        main()

    stream.stop_stream()
    stream.close()
    p.terminate()

