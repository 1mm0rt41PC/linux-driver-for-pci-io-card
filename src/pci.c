#include "pci.h"
#include "extra.h"
#include <linux/module.h>


/***************************************************************************//*!
* @brief [SLOT] Démarrage du périférique PCI. (Appelé lors du branchage du périf
* ou lors du démarage du driver).
* @param[in,out] dev		Le périférique a démarrer.
* @param[in] id				Info sur le périf (cf la table au dessus)
* @return[NONE]
*/
int pci_init( struct pci_dev* dev, const struct pci_device_id* id )
{
	int ret=0;
	//u16 input=0;
	unsigned long io_base=0, io_end=0, io_flags=0, io_len=0;
	int i=0;

	stdError(KERN_DEBUG, "pci_detection - FOUND");
	//pci_bus_read_config_word();
	// int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
	/*if( pci_bus_read_config_word(dev->bus, dev->devfn, 4, &input) != 0 ){// Returns 0 on success.
		stdError(KERN_WARNING, "ERROR pci_bus_read_config_word=%d", ret);
	}// PCI_BRIDGE_RESOURCE_NUM
	stdError(KERN_DEBUG, "u16=%hd", input);*/

	/*
	// 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252
	if( (ret=pci_bus_write_config_dword(dev->bus, dev->devfn, 252, -1)) != 0 ){
		stdError(KERN_WARNING, "ERROR pci_bus_write_config_dword=%d", ret);
	}
	*/

	if( (ret = pci_enable_device(dev)) != 0 ){
		stdError(KERN_WARNING, "ERROR pci_enable_device=%d", ret);
		return ret;
	}

	for(i=0;i<=6;++i)
	{
		io_base = pci_resource_start(dev, i);// (doc) i=PCI_BASE_ADDRESS_SPACE_IO
		io_end = pci_resource_end(dev, i);
		io_flags = pci_resource_flags(dev, i);
		io_len = pci_resource_len(dev, i);
		stdError(KERN_DEBUG, "pci_resource_start(%d)=%lu, end=%lu, flags=%lu, len=%lu", i, io_base, io_end, io_flags, io_len);
	}
	
	stdError(KERN_DEBUG, DEV_NAME " CARD @: %lu", (long unsigned int)pci_resource_start(dev, 2));
	

	return 0;
}


/***************************************************************************//*!
* @brief [SLOT] Arrêt du périférique PCI. (Appelé lors du débranchage du périf
* ou lors de l'arrêt du driver).
* @param[in,out] dev		Le périférique a arrêter.
* @return[NONE]
*/
void pci_remove( struct pci_dev* dev )
{
	pci_disable_device(dev);
	stdError(KERN_DEBUG, "pci_remove - FOUND");
}




/***************************************************************************//*!
* @brief Liste des périfériques accèpté par le driver et qui seront passé aux fonctions:
* - pci_init
* - pci_remove
*/
struct pci_device_id pci_tbl[] =
{
      {PCI7250_VENDOR, PCI7250_DEVICE, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
      { }
};
MODULE_DEVICE_TABLE (pci, pci_tbl);


/***************************************************************************//*!
* @brief Permet de faire la liaison entre le driver et les action vis à vis des
* périfériques qui seront détecté et qui seront élligible selon {pci_tbl}
*/
struct pci_driver pci_dr_strct =
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
