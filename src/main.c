#include <linux/module.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <linux/cdev.h>
#include "dev.h"// Tout pour gérer le lien /dev/PCI_IO_GOD
#include "pci.h"// Tout pour gérer la liaison PCI


MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO-By-God");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");


static dev_t			first; // Global variable for the first device number
static struct cdev		c_dev; // Global variable for the character device structure
static struct class*	cl; // Global variable for the device class





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
