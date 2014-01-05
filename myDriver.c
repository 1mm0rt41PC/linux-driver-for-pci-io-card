#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...

MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO-By-God");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");
#define DEV_NAME "PCI_IO_GOD"

static int major = 254;// numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique
module_param(major, int, 0);
MODULE_PARM_DESC(major, "major numbe");

#define stdError( lvl, msg, ... ) printk(lvl "[" __FILE__ ":%d]: " msg "\n", __LINE__, ##__VA_ARGS__)





static ssize_t readDev( struct file* file, char* buf, size_t count, loff_t* ppos )
{
	printk(KERN_DEBUG "read()\n");
	return 0;
}

static ssize_t writeDev( struct file* file, const char* buf, size_t count, loff_t* ppos )
{
	printk(KERN_DEBUG "write()\n");
	return 0;
}

static int openDev( struct inode* inode, struct file* file )
{
	printk(KERN_DEBUG "open()\n");
	return 0;
}

static int closeDev( struct inode* inode, struct file* file )
{
	printk(KERN_DEBUG "close()\n");
	return 0;
}




/***************************************************************************//*!
* @brief Permet d'affecter des fonctions à: read, write, ...
*/
struct file_operations fops =
{
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
	stdError(KERN_DEBUG, "Init " DEV_NAME);

	// major:		Numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique.
	// DEV_NAME:	Nom du périphérique qui apparaîtra dans /proc/devices
	// fops:		Pointeur vers une structure qui contient des pointeurs de fonction.
	// 				Ils définissent les fonctions appelées lors des appels systèmes (open, read...) du côté utilisateur.
    if( (ret=register_chrdev(major, DEV_NAME, &fops)) < 0 ){
		stdError(KERN_WARNING, " Erreur avec register_chrdev=%d ", ret);
	}

	stdError(KERN_DEBUG, "Ready " DEV_NAME);
	return 0;
}


/***************************************************************************//*!
* @brief Point de sortie
* @return 0 Si tout est OK. >0 sinon.
*/
static void __exit exitPoint(void)
{
	int ret =0;
	stdError(KERN_DEBUG, "Trying to quit " DEV_NAME);

	// major:		Numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique.
	// DEV_NAME:	Nom du périphérique qui apparaîtra dans /proc/devices
	unregister_chrdev(major, DEV_NAME);
	if( (ret = unregister_chrdev(major, DEV_NAME)) < 0 ){
		stdError(KERN_WARNING, " Erreur avec unregister_chrdev=%d ", ret);
	}

	stdError(KERN_DEBUG, "Quit " DEV_NAME);
}

module_init(entryPoint);
module_exit(exitPoint);
