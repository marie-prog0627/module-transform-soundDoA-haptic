#! /bin/bash
# Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

echo LEDoff
port=5

if [ ! -e /sys/class/gpio/gpio5 ]; then
        echo $port > /sys/class/gpio/export
fi
sudo sh -c 'echo low > /sys/class/gpio/gpio5/direction'


