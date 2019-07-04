KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
DTC_DIR=/lib/modules/$(shell uname -r)/build/scripts/dtc
KERNEL_BASE=/lib/modules/$(shell uname -r)/kernel
CODECS=$(KERNEL_BASE)/sound/soc/codecs/snd-soc-pcm1808.ko
BCM=$(KERNEL_BASE)/sound/soc/bcm/snd-soc-pcm1808-adc.ko
DTS=/boot/overlays/pcm1808-adc.dtbo

obj-m := snd-soc-pcm1808.o snd-soc-pcm1808-adc.o

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) modules
	cp snd-soc-pcm1808.ko Backup/snd-soc-pcm1808.bak
	cp snd-soc-pcm1808-adc.ko Backup/snd-soc-pcm1808-adc.bak
	echo $(shell uname -r) > Backup/memo.txt

clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) clean 
#	rm -f pcm1808-adc.dtbo

dts:
	$(DTC_DIR)/dtc -@ -H epapr -I dts -O dtb -o pcm1808-adc.dtbo pcm1808-adc.dts

install_modules:
	cp snd-soc-pcm1808.ko $(KERNEL_BASE)/sound/soc/codecs/
	cp snd-soc-pcm1808-adc.ko $(KERNEL_BASE)/sound/soc/bcm/
	depmod -a

remove_modules:
	if [ -e $(CODECS) ]; then rm $(CODECS); fi
	if [ -e $(BCM) ]; then rm $(BCM); fi
	depmod -a

start:
	modprobe snd-soc-pcm1808
	modprobe snd-soc-pcm1808-adc

stop:
	modprobe -r snd-soc-pcm1808-adc
	modprobe -r snd-soc-pcm1808

install_dts:
	cp pcm1808-adc.dtbo /boot/overlays/

remove_dts:
	if [ -e $(DTS) ]; then rm $(DTS); fi

