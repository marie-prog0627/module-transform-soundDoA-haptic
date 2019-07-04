#! /bin/bash


# wget http://einstlab.web.fc2.com/RaspberryPi/PumpkinPi.tar
# tar xvf PumpkinPi.tar
# cd PumpkinPi
# ./setup.sh

echo "Driver"
cd
cd PumpkinPi/Driver
./setup.sh
cd
cd PumpkinPi/Remocon
./compile.sh


echo "Step 0"
SLOTS=slots=snd_soc_pcm1808_adc,snd_bcm2835
cd
sudo sed -i -e 's/i2c-dev//' /etc/modules
sudo sh -c "echo i2c-dev >> /etc/modules"
sudo sed -i -e 's/#dtparam=i2s=on/dtparam=i2s=on/' /boot/config.txt
sudo sed -i -e 's/#dtparam=i2c_arm=on/dtparam=i2c_arm=on/' /boot/config.txt
sudo sh -c "echo options snd $SLOTS > /etc/modprobe.d/alsa-base.conf"
sudo sed -i -e 's/snd-pcm-oss//' /etc/modules
# sudo sh -c "echo snd-pcm-oss >> /etc/modules"
sudo sed -i -e 's/dtoverlay=pcm1808-adc//' /boot/config.txt
sudo sh -c "echo dtoverlay=pcm1808-adc >> /boot/config.txt"
sudo apt-get install i2c-tools
cd PumpkinPi/ADC
./compile.sh


echo "Step 1"
cd
# for OSS
# sudo apt-get -y install osspd-alsa
# for ALSA
sudo apt-get -y install libasound2-dev
#
sudo sh -c "rm -r julius-master"
wget https://github.com/julius-speech/julius/archive/master.zip
unzip master.zip
rm master.zip
cd julius-master
#./configure
./configure --with-mictype=alsa --enable-words-int
make
sudo make install


echo "Step 2"
cd
sudo sh -c "rm -r julius-kit"
mkdir julius-kit
cd julius-kit
wget https://github.com/julius-speech/dictation-kit/archive/master.zip
unzip master.zip
rm master.zip
wget https://github.com/julius-speech/grammar-kit/archive/master.zip
unzip master.zip
rm master.zip


echo "Step 3"
cd ~/julius-kit/grammar*
cp ~/PumpkinPi/command.tar .
tar xvf command.tar
rm command.tar
cd command
./setup.sh


echo "done"
# rc script
BASE=/home/pi/PumpkinPi
# sudo sed -i -e "s|sudo -u pi $BASE/Remocon/start.sh \&||" /etc/rc.local
# sudo sed -i -e "s|exit 0||" /etc/rc.local
# sudo sh -c "echo sudo -u pi $BASE/Remocon/start.sh \& >> /etc/rc.local"
# sudo sh -c "echo exit 0 >> /etc/rc.local"

