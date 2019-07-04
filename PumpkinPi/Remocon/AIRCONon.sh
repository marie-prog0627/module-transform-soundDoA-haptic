#! /bin/bash
# for Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

base=/home/pi/PumpkinPi/Remocon

echo AIRCONon
sudo $base/setIR2 AEHA 120 20 99 0 16 16 252 8 48 129 20 0 60 8 0 247
$base/LEDon.sh
sleep 1
$base/LEDoff.sh


