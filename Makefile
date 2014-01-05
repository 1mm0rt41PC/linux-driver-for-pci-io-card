NAME_internal = PCI_IO_GOD
# Voir /proc/devices
MAJOR_internal = 2544

obj-m += PCI_IO_GOD.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

in:
	insmod ./PCI_IO_GOD.ko
#	b      create a block (buffered) special file
#	c, u   create a character (unbuffered) special file
#	p      create a FIFO
	mknod /dev/$(NAME_internal) c $(MAJOR_internal) 0


out: rm
rm:
	rmmod PCI_IO_GOD.ko
	rm /dev/PCI_IO_GOD

sh: show
show:
	dmesg | tail

test:
	python -c 'fp=open("/dev/PCI_IO_GOD", "w"); fp.write("123 456"); fp.close();'
	dmesg | tail
