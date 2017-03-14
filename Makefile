# Doc: https://www.kernel.org/doc/Documentation/kbuild/modules.txt

NAME_internal = PCI_IO
# Voir /proc/devices
MAJOR_internal = 2544


obj-m += PCI_IO.o
PCI_IO-objs := ./src/main.o ./src/dev.o ./src/pci.o ./src/extra.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean


out: rm
rm:
	rmmod PCI_IO.ko
	rm -rf /dev/PIO/ 2>/dev/null
	rm /etc/udev/rules.d/80-PCI_IO.rules
#	rm /dev/PCI_IO


sh: show
show:
	dmesg | tail

test:
	VALUE=255 CARD_1_OUT=7232 python -c 'import os; fp=open("/dev/PIO/"+os.environ["CARD_1_OUT"], "w"); fp.write(os.environ["VALUE"]); fp.close();'
	CARD_1_OUT=7232 python -c 'import os; print(ord(list(open("/dev/PIO/"+str(int(os.environ["CARD_1_OUT"])+2), "r").read(2))[0]));'
	#python -c 'import sys,os; [open("/dev/PIO/"+str(i), "w").write("255") for i in xrange(5300,5400)]'


	
dr:
	cd /home/maison/Bureau/PCI-7250-GPIO/pci-dask_427/drivers/ && ./dask_inst.pl


drrm:
	rmmod p7250


install:
	make -C /lib/modules/$$(uname -r)/build M=$(shell pwd) modules_install
