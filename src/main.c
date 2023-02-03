#include <linux/module.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <linux/cdev.h>
#include "dev.h"// Tout pour gérer le lien /dev/PCI_IO
#include "pci.h"// Tout pour gérer la liaison PCI

MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO interact with PCI card");
MODULE_LICENSE("GPL");

int my_dev_uevent( struct device *dev, struct kobj_uevent_env *env );


static dev_t			first; // Global variable for the first device number
static struct cdev		c_dev; // Global variable for the character device structure
static struct class     my_class = {// Global variable for the device class
    .name           = DEV_NAME,
    .owner          = THIS_MODULE,
    //.dev_uevent   = my_dev_uevent,// Permet de changer le chmod
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
		return __LINE__;
	}

	stdError(KERN_DEBUG, "Init - class_register");
	if( (ret = class_register(&my_class)) ){
		stdError(KERN_WARNING, "Erreur avec class_register=%d", ret);
		unregister_chrdev_region(first, 1);
		return __LINE__;
	}

	/*
	* http://lists.kernelnewbies.org/pipermail/kernelnewbies/2013-January/007111.html
	* After searching a lot figured out that to create a folder under /dev
	* is to use a '!' as separator instead of '/' in the "fmt" argument of
	* the device_create() call. This works with kernel 2.6.18 onwards.
    *
	* "test!power" => /dev/test/power
	* DEV_NAME"!sous-fichier"
	*/
	stdError(KERN_DEBUG, "Init - device_create");
	if( device_create(&my_class, NULL, first, NULL, DEV_NAME) == NULL ){
		stdError(KERN_WARNING, "Erreur avec device_create=NULL");
		class_unregister(&my_class);
		unregister_chrdev_region(first, 1);
		return __LINE__;
	}

	stdError(KERN_DEBUG, "Init - cdev_init");
	cdev_init(&c_dev, &fops);

	stdError(KERN_DEBUG, "Init - cdev_add");
	if( cdev_add(&c_dev, first, 1) == -1 ){
		stdError(KERN_WARNING, "Erreur avec cdev_add=-1");
		device_destroy(&my_class, first);
		class_unregister(&my_class);
		unregister_chrdev_region(first, 1);
		return __LINE__;
	}


	//__pci_register_driver(pci_dr_strct, THIS_MODULE, DEV_NAME);
	stdError(KERN_DEBUG, "Init - pci_register_driver");
	if( (ret=pci_register_driver(&pci_dr_strct)) != 0 ){//Returns a negative value on error, otherwise 0
		stdError(KERN_DEBUG, "pci_register_driver ERROR %d", ret);
		return ret;
	}

	stdError(KERN_DEBUG, "Init - OK");
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
	device_destroy(&my_class, first);
	class_unregister(&my_class);
	unregister_chrdev_region(first, 1);

	stdError(KERN_DEBUG, "Quit " DEV_NAME);
}


/***************************************************************************//*!
* @brief Permet de changer des paramtères comme le chmod
* @return 0 Si tout est OK. >0 sinon.
*/
int my_dev_uevent( struct device* dev, struct kobj_uevent_env* env )
{
	add_uevent_var(env, "DEVMODE=%#o", 0440);
	return 0;
}


module_init(entryPoint);
module_exit(exitPoint);
