#! /bin/bash
# for Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

base=/home/pi/PumpkinPi/Remocon

echo TVon
sudo $base/setIR2 AEHA 48 2 32 176 0 61 141
$base/LEDon.sh
sleep 1
$base/LEDoff.sh


