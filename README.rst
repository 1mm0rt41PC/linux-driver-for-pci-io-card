======
README
======

:Info: See <https://bitbucket.org/ImmortalPC/linux-driver-for-pci-io-card> for repo.
:Author: NUEL Guillaume (ImmortalPC)
:Date: $Date: 2017-03-14 $
:Revision: $Revision: 1.0 $
:Description: Linux driver for the io card: PCI-7250


::

	======================================================
	= Warning =
	===========
	This code is finished and is currently stable.
	======================================================



**Screenshot**
--------------
.. image:: https://bitbucket.org/ImmortalPC/linux-driver-for-pci-io-card/raw/master/doc/PCI-7250.jpg
	:alt: Screenshot
	:width: 400px
	:align: center
	:target: https://bitbucket.org/ImmortalPC/linux-driver-for-pci-io-card/raw/master/doc/PCI-7250.jpg

|

**Description**
---------------
This is a linux diver for the io card: PCI-7250. It work's with all kernels >= 3.x.x

|

**Usage**
---------------
Build::
	
	$ chmod +x ./pci.sh
	$ ./pci.sh install
	$ ./pci.sh start
	$ ./pci.sh test

After a new device will be aviable at ``/dev/PCI_IO`` and at ``/dev/PIO/...``::
	
Basic usage::

	root@home:~$ cat > /dev/PIO/card_1
	255
	^D
	root@home:~$ cat /dev/PIO/card_1
	255
	# Or
	root@home:~$ cat /dev/PIO/1234 <<<<< Card address
	255
	
Basic usage with python::
	
	with open('/dev/PIO/card_1', 'rb+') as fp:
		fp.write('255');
		print(str(fp.read(2)));

|

**Tools** **and** **libs** **required**
---------------------------------------
- gcc
- Kernel 3.x.x

|

**Documentation**
-----------------
For more information see the documentation in the directory **/doc/**

|

**Code** **Format** **and** **Standards**
-----------------------------------------
- The code is in UTF-8 
- The code is indented with real tabs (\\t) 
- The code is intended to be displayed with a size of 4 for the tab (\\t) 
- The line endings type LF (\\n) 
- IDE used: QtCreator 
- Comments are in doxygen format.

|

**IDE** **RST**
---------------
RST created through viewer: https://github.com/anru/rsted