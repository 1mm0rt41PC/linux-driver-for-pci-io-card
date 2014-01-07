#ifndef PCI_h
#define PCI_h
#include <linux/pci.h>

extern struct pci_device_id pci_tbl[];
extern struct pci_driver pci_dr_strct;

int pci_init( struct pci_dev* dev, const struct pci_device_id* id );
void pci_remove( struct pci_dev* dev );

#endif// PCI_h
