#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <asm/io.h>// outb, ... (Voir cat /proc/ioports pour les adresses)
// Voi aussi cat /proc/bus/pci/devices > 1.txt
#include <linux/device.h>// Pour auto mount en /dev/
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>


MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO-By-God");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");
#define DEV_NAME "PCI_IO_GOD"

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class


#define stdError( lvl, msg, ... ) printk(lvl "[" __FILE__ ":%d]: " msg "\n", __LINE__, ##__VA_ARGS__)



static ssize_t readDev( struct file* file, char* buf, size_t count, loff_t* ppos )
{
	stdError(KERN_DEBUG, "read()");
	return 0;
}

static ssize_t writeDev( struct file* file, const char* buf, size_t count, loff_t* ppos )
{
	int i=0;
	int spacePos = -1;
	u8 val1 = 1;
	u16 val2 = 1;
	u32 val3 = 1;

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

	for( i=0x1880; i<=0x18ff; i++ )
	{
		outb(val1, i);
		msleep(1000);
		outw(val2, i);
		msleep(1000);
		outl(val3, i);
		msleep(1000);
	}

	stdError(KERN_DEBUG, "write() %s", buf);
	return count;
}

static int openDev( struct inode* inode, struct file* file )
{
	stdError(KERN_DEBUG, "open()");
	return 0;
}

static int closeDev( struct inode* inode, struct file* file )
{
	stdError(KERN_DEBUG, "close()");
	return 0;
}






/***************************************************************************//*!
* @brief Permet d'affecter des fonctions à: read, write, ...
*/
struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = readDev,
	.write = writeDev,
	.open = openDev,
	.release = closeDev// correspond a close
};






/***************************************************************************//*!
* @brief Point d'entrée.
* @return 0 Si tout est OK. >0 sinon.
*
* @note Pour passer un paramètre:
* $insmod ./module.ko param=2
* Avec dans le code:
* @code
* static int param;
* module_param(param, int, 0);
* MODULE_PARM_DESC(param, "Un paramètre de ce module");
* @endcode
*/
static int __init entryPoint(void)
{
	int ret=0;
	stdError(KERN_DEBUG, "---------------------------------------------------");
	stdError(KERN_DEBUG, "Init " DEV_NAME);

	if( (ret=alloc_chrdev_region(&first, 0, 1, DEV_NAME)) < 0 ){
		if( ret == -EINVAL ){
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d => Le nombre specifie n'est pas valide (> MAX_CHRDEV)", ret);
		}else if( ret == -EBUSY ){
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d => Le nombre majeur est occupe.", ret);
		}else{
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d", ret);
		}
		return -1;
	}

	if( (cl = class_create(THIS_MODULE, DEV_NAME)) == NULL ){
		stdError(KERN_WARNING, "Erreur avec class_create=NULL");
		unregister_chrdev_region(first, 1);
		return -1;
	}

	if( device_create(cl, NULL, first, NULL, DEV_NAME) == NULL ){
		stdError(KERN_WARNING, "Erreur avec device_create=NULL");
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}

	cdev_init(&c_dev, &fops);

	if( cdev_add(&c_dev, first, 1) == -1 ){
		stdError(KERN_WARNING, "Erreur avec cdev_add=-1");
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	return 0;
}


/***************************************************************************//*!
* @brief Point de sortie
* @return 0 Si tout est OK. >0 sinon.
*/
static void __exit exitPoint(void)
{
	stdError(KERN_DEBUG, "Trying to quit " DEV_NAME);

	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);

	stdError(KERN_DEBUG, "Quit " DEV_NAME);
}

module_init(entryPoint);
module_exit(exitPoint);
