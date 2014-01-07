#ifndef EXTRA_h
#define EXTRA_h

#define stdError( lvl, msg, ... ) printk(lvl "[" __FILE__ ":%d]: " msg "\n", __LINE__, ##__VA_ARGS__)

#define DEV_NAME "PCI_IO_GOD"
#define PCI7250_VENDOR 5194
#define PCI7250_DEVICE 29264

#endif// EXTRA_h
