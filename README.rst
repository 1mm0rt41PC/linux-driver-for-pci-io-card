======
README
======

:Info: See <https://bitbucket.org/ImmortalPC/linux-driver-for-pci-io-card> for repo.
:Author: NUEL Guillaume (ImmortalPC)
:Date: $Date: 2014-01-19 $
:Revision: $Revision: 0.1 $
:Description: Linux driver for the io card: PCI-7250

|

.. csv-table::
   :header: "Warning"

   "This code is not finished and is currently unstable."

|

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

**Code** **Format** **and** **Standards**
-----------------------------------------
- The code is in UTF-8 
- The code is indented with real tabs (\\t) 
- The code is intended to be displayed with a size of 4 for the tab (\\t) 
- The line endings type LF (\\n) 
- IDE used: QtCreator 
- Comments are in doxygen format.

|

**Tools** **and** **libs** **required**
---------------------------------------
- gcc
- Kernel 3.x.x

|

**Usage**
---------
Build::

	$ make

Start the module::
	
	$ make in

After a new device will be aviable at ``/dev/PCI_IO_GOD``


|

**Documentation**
-----------------
For more information see the documentation in the directory **/doc/**

|

**IDE** **RST**
---------------
RST created through viewer: https://github.com/anru/rsted