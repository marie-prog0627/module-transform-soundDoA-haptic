#! /bin/bash
# for VoiceCAP
# (C)Copyright 2017 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

# to delete old
KERNEL_BASE=/lib/modules/$(uname -r)/kernel
CODECS=$KERNEL_BASE/sound/soc/codecs/pcm1808-adc.ko
BCM=$KERNEL_BASE/sound/soc/bcm/snd_soc_pcm1808_adc.ko
if [ -e $CODECS ]; then sudo rm $CODECS; fi
if [ -e $BCM ]; then sudo rm $BCM; fi

# to make PCM1808 driver

sudo make stop
sudo make remove_dts
sudo make remove_modules

sudo make dts
sudo make

sudo make install_dts
sudo make install_modules
sudo make start

sudo make clean


