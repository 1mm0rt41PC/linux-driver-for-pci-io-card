#!/bin/bash
cd `dirname "$0"`

function pci_stop()
{
	echo 'Stop...'
	sudo rmmod PCI_IO.ko
	rm -rf /dev/PIO/ 2>/dev/null
	rm /etc/udev/rules.d/80-PCI_IO.rules 2>/dev/null
}


function pci_start()
{
	echo 'Start...'
	[ -z "$USERNAME" ] && export USERNAME="$USER"
	# Pour les rules udev, voir http://www.reactivated.net/writing_udev_rules.html#basic
	echo 'KERNEL=="PCI_IO", MODE="0660", OWNER="root", GROUP="'$USERNAME'"' | sudo tee /etc/udev/rules.d/80-PCI_IO.rules
	sudo rm -rf /dev/PIO/ 2>/dev/null
	sudo dmesg -C
	sudo insmod ./PCI_IO.ko
	sudo mkdir /dev/PIO/
	sudo dmesg | grep -F 'PCI_IO CARD @:' | awk -F '@: ' '{print $2}' | while read -r line; do
		export line=$line
		sudo ln /dev/PCI_IO /dev/PIO/$line
		sudo ln /dev/PCI_IO /dev/PIO/`expr $line + 1`
		sudo ln /dev/PCI_IO "/dev/PIO/card_w${line}_r`expr $line + 1`"
		sudo ln -s "/dev/PIO/card_w${line}_r`expr $line + 1`" "/dev/PIO/card_`ls -l /dev/PIO/card_w* |wc -l`"
#	b      create a block (buffered) special file
#	c, u   create a character (unbuffered) special file
#	p      create a FIFO
#	mknod /dev/$(NAME_internal) c $(MAJOR_internal) 0
	done
	sudo chown root:$USERNAME -R /dev/PIO/
	echo "`ls -l /dev/PIO/card_w* |wc -l` card found"
	ls -alFpih --color=auto /dev/PIO/
}


function pci_test()
{
	echo 'Test...'
	echo 'cat > /dev/PIO/card_1   <==== Example'
	echo -e '--------\n';
	for pci in `ls /dev/PIO/card_w*`; do
		echo "$pci"
		/bin/echo 255 > $pci
		sleep 1
		/bin/echo 0 > $pci
		cat $pci | hexdump -C
	done
	
	echo 'Test2...'
	sleep 1
	cat <<'EOD' | python3
from random import random;
from time import sleep;
from requests import get
from glob import glob
handle = [ open(p,'w') for p in glob('/dev/PIO/card_*') ]
while 1:
    for h in handle:
        h.write(str(int(random()*1000)%255));
        h.flush();
#    get(f'http://192.168.2.100/current_state.xml?pw=admin&Relay1={int(random()*1000)%2}&Relay2={int(random()*1000)%2}&Relay3={int(random()*1000)%2}')
#    get(f'http://192.168.2.100/current_state.xml?pw=admin&Relay4={int(random()*1000)%2}&Relay5={int(random()*1000)%2}&Relay6={int(random()*1000)%2}')
#    get(f'http://192.168.2.100/current_state.xml?pw=admin&Relay7={int(random()*1000)%2}&Relay8={int(random()*1000)%2}')
    sleep(0.25);
EOD

}


function pci_restart()
{
	pci_stop;
	pci_start;
}


[ "$1" = "start" ] && pci_start;
[ "$1" = "stop" ] && pci_stop;
[ "$1" = "restart" ] && pci_restart;
[ "$1" = "test" ] && pci_test;
[ "$1" = "install" ] && make clean && make && sudo make install
