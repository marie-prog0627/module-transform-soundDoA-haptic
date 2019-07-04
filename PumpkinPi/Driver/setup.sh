#! /bin/bash
# for PumpkinPi
# (C)Copyright 2017 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com

# to make Kernel Driver

ver=$(uname -r)
ver=${ver%.*}
ver=${ver/./}
echo $ver
if [ $ver -le 414 ]
then
  cp snd-soc-pcm1808.414.c snd-soc-pcm1808.c
else
  cp snd-soc-pcm1808.418.c snd-soc-pcm1808.c
fi


# step 1
# to get tools
sudo apt-get update
#sudo apt-get -y install build-essential
#sudo apt-get -y install git
sudo apt-get -y install ncurses-dev
#sudo apt-get -y install device-tree-compiler
sudo apt-get -y install bc
# --- above 4.18.x ---
sudo apt-get -y install bison
sudo apt-get -y install flex
sudo apt-get -y install libssl-dev

# step 2
# to get Raspbian source
RPISOURCE=https://raw.githubusercontent.com/notro/rpi-source/master
sudo wget $RPISOURCE/rpi-source -O /usr/bin/rpi-source
sudo chmod +x /usr/bin/rpi-source
sudo /usr/bin/rpi-source -q --tag-update
sudo rpi-source --skip-gcc -g --delete -v

# step 3
./compile.sh
sudo sh -c "rm /root/linux"
sudo sh -c "rm -r /root/linux-*"


