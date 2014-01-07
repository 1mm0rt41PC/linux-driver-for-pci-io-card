#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <asm/io.h>// outb, ... (Voir cat /proc/ioports pour les adresses)
// Voi aussi cat /proc/bus/pci/devices > 1.txt
#include <linux/device.h>// Pour auto mount en /dev/
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
//#include <linux/delay.h>// Pour msleep
#include <linux/pci.h>


MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO-By-God");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");
#define DEV_NAME "PCI_IO_GOD"
#define PCI7250_VENDOR 5194
#define PCI7250_DEVICE 29264

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
* @brief [SLOT] Démarrage du périférique PCI. (Appelé lors du branchage du périf
* ou lors du démarage du driver).
* @param[in,out] dev		Le périférique a démarrer.
* @param[in] id				Info sur le périf (cf la table au dessus)
* @return[NONE]
*/
static int pci_init( struct pci_dev* dev, const struct pci_device_id* id )
{
	int ret=0;
	u16 input=0;
	stdError(KERN_DEBUG, "pci_detection - FOUND");
	//pci_bus_read_config_word();
	// int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
	if( pci_bus_read_config_word(dev->bus, dev->devfn, 4, &input) != 0 ){// Returns 0 on success.
		stdError(KERN_WARNING, "ERROR pci_bus_read_config_word=%d", ret);
	}// PCI_BRIDGE_RESOURCE_NUM
	stdError(KERN_DEBUG, "u16=%hd", input);

	/*
	// 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252
	if( (ret=pci_bus_write_config_dword(dev->bus, dev->devfn, 252, -1)) != 0 ){
		stdError(KERN_WARNING, "ERROR pci_bus_write_config_dword=%d", ret);
	}
	*/

	pci_enable_device(dev);
	return 0;
}


/***************************************************************************//*!
* @brief [SLOT] Arrêt du périférique PCI. (Appelé lors du débranchage du périf
* ou lors de l'arrêt du driver).
* @param[in,out] dev		Le périférique a arrêter.
* @return[NONE]
*/
static void pci_remove( struct pci_dev* dev )
{
	pci_disable_device(dev);
	stdError(KERN_DEBUG, "pci_remove - FOUND");
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


/***************************************************************************//*!
* @brief Liste des périfériques accèpté par le driver et qui seront passé aux fonctions:
* - pci_init
* - pci_remove
*/
static struct pci_device_id pci_tbl[] =
{
      {PCI7250_VENDOR, PCI7250_DEVICE, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
      { }
};
MODULE_DEVICE_TABLE (pci, pci_tbl);


/***************************************************************************//*!
* @brief Permet de faire la liaison entre le driver et les action vis à vis des
* périfériques qui seront détecté et qui seront élligible selon {pci_tbl}
*/
static struct pci_driver pci_dr_strct =
{
      .name     = DEV_NAME,//!< Nom du driver
      .id_table = pci_tbl,//!< Liste des devices supportés.
      .probe    = pci_init,//!< Détection device
      .remove   = pci_remove//!< Libération device

/*
 * 	Champs possible
	struct list_head node;

	const char *name;
	const struct pci_device_id *id_table;	// must be non-NULL for probe to be called
	int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);	// New device inserted
	void (*remove) (struct pci_dev *dev);	// Device removed (NULL if not a hot-plug capable driver)

	int  (*suspend) (struct pci_dev *dev, pm_message_t state);	// Device suspended
	int  (*suspend_late) (struct pci_dev *dev, pm_message_t state);
	int  (*resume_early) (struct pci_dev *dev);
	int  (*resume) (struct pci_dev *dev);	                // Device woken up
	void (*shutdown) (struct pci_dev *dev);
	struct pci_error_handlers *err_handler;
	struct device_driver	driver;
	struct pci_dynids dynids;
*/
};


/*
static void init_device()
{
	struct pci_dev *dev = NULL;

	// Remplacer pci_find_device par pci_get_device
	// On cherche tout les PCI avec pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev)
	while( (dev=pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL )
	{
		stdError(KERN_DEBUG, "PCI-7250 Found");
	}
}
*/



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

	//__pci_register_driver(pci_dr_strct, THIS_MODULE, DEV_NAME);
	if( (ret=pci_register_driver(&pci_dr_strct)) != 0 ){//Returns a negative value on error, otherwise 0
		stdError(KERN_DEBUG, "pci_register_driver ERROR %d", ret);
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

	pci_unregister_driver(&pci_dr_strct);
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);

	stdError(KERN_DEBUG, "Quit " DEV_NAME);
}

module_init(entryPoint);
module_exit(exitPoint);
