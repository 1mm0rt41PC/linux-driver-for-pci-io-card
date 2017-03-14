#ifndef EXTRA_h
#define EXTRA_h

#define stdError( lvl, msg, ... ) printk(lvl "[" __FILE__ ":%d]: " msg "\n", __LINE__, ##__VA_ARGS__)

#define DEV_NAME "PCI_IO"
#define PCI7250_VENDOR 5194
#define PCI7250_DEVICE 29264

struct file;
const char* getFullPath( const struct file* pfile );
char getID( const struct file* pfile, int* card_w, int* card_r );
int str2int( const char str[] );

#endif// EXTRA_h
