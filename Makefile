obj-m += myDriver.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

in:
	insmod ./myDriver.ko

out: rm
rm:
	rmmod myDriver.ko

sh: show
show:
	dmesg
