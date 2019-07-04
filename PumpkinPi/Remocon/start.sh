#! /bin/bash
# for Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

export ALSADEV=plughw:0,0
cd /home/pi/julius-kit/grammar*/command
./server.sh &
sleep 3
cd /home/pi/PumpkinPi/Remocon
./remocon.py &


