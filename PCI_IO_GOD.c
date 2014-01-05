#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <asm/io.h>// outb, ... (Voir cat /proc/ioports pour les adresses)
// Voi aussi cat /proc/bus/pci/devices > 1.txt

MODULE_AUTHOR("Immortal-PC");
MODULE_DESCRIPTION("PCI-IO-By-God");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");
#define DEV_NAME "PCI_IO_GOD"

static int major = 2544;// numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique
module_param(major, int, 0);
MODULE_PARM_DESC(major, "major number");

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

	// major:		Numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique.
	// DEV_NAME:	Nom du périphérique qui apparaîtra dans /proc/devices
	// fops:		Pointeur vers une structure qui contient des pointeurs de fonction.
	// 				Ils définissent les fonctions appelées lors des appels systèmes (open, read...) du côté utilisateur.
	if( (ret=register_chrdev(major, DEV_NAME, &fops)) < 0 ){
		if( ret == -EINVAL ){
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d => Le nombre specifie n'est pas valide (> MAX_CHRDEV)", ret);
		}else if( ret == -EBUSY ){
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d => Le nombre majeur est occupe.", ret);
		}else{
			stdError(KERN_WARNING, "Erreur avec register_chrdev=%d", ret);
		}
		return ret;
	}

	stdError(KERN_DEBUG, "Ready " DEV_NAME " Major=%d", major);
	return 0;
}


/***************************************************************************//*!
* @brief Point de sortie
* @return 0 Si tout est OK. >0 sinon.
*/
static void __exit exitPoint(void)
{
	stdError(KERN_DEBUG, "Trying to quit " DEV_NAME);

	// major:		Numéro majeur du driver, 0 indique que l'on souhaite une affectation dynamique.
	// DEV_NAME:	Nom du périphérique qui apparaîtra dans /proc/devices
	unregister_chrdev(major, DEV_NAME);

	stdError(KERN_DEBUG, "Quit " DEV_NAME);
}

module_init(entryPoint);
module_exit(exitPoint);
