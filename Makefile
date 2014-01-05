obj-m += PCI_IO_GOD.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

in:
	insmod ./PCI_IO_GOD.ko

out: rm
rm:
	rmmod PCI_IO_GOD.ko

sh: show
show:
	dmesg
