#include <linux/module.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include "dev.h"

ssize_t readDev( struct file* file, char* buf, size_t count, loff_t* ppos )
{
	stdError(KERN_DEBUG, "read()");
	return 0;
}

ssize_t writeDev( struct file* file, const char* buf, size_t count, loff_t* ppos )
{
	int i=0;
	int spacePos = -1;
	u8 val1 = 1;
	//u16 val2 = 1;
	//u32 val3 = 1;

	if( !count ){
		stdError(KERN_DEBUG, "write(): Empty write => Correct usage: echo '<id Card> <value>' > /dev/PCI_IO_GOD");
		return -1;
	}
	for( i=0; buf[i]; ++i )
	{
		if( buf[i] == ' ' ){
			spacePos = i;
		}else if( !('0' <= buf[i] && buf[i] <= '9') ){
			stdError(KERN_DEBUG, "write(): Bad write (%c) => Correct usage: echo '<id Card> <value>' > /dev/PCI_IO_GOD", buf[i]);
			return -1;
		}
	}

	for( i=0x1800; i<=0x18ff; i++ )
	{
		outb(val1, i);
		//msleep(1000);
		//outw(val2, i);
		//msleep(1000);
		//outl(val3, i);
		//msleep(1000);
	}

	stdError(KERN_DEBUG, "write() %s", buf);
	return count;
}

int openDev( struct inode* inode, struct file* file )
{
	stdError(KERN_DEBUG, "open()");
	return 0;
}

int closeDev( struct inode* inode, struct file* file )
{
	stdError(KERN_DEBUG, "close()");
	return 0;
}


/***************************************************************************//*!
* @brief Permet d'affecter des fonctions à: read, write, ...
*/
struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= readDev,
	.write		= writeDev,
	.open		= openDev,
	.release	= closeDev// correspond a close
};
