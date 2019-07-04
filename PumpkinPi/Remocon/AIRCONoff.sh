#! /bin/bash
# for Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

base=/home/pi/PumpkinPi/Remocon

echo AIRCONoff
sudo $base/setIR2 AEHA 48 20 99 0 16 16 2
$base/LEDon.sh
sleep 1
$base/LEDoff.sh


