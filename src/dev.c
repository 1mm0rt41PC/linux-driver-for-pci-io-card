#include <linux/module.h>
#include <linux/fs.h>// Pour créer un accès à /dev/...
#include <linux/uaccess.h>
#include <linux/delay.h>
#include "dev.h"
#include "extra.h"

struct task {
	const struct file* file;
	char isEOF;
	int card_r;
	int card_w;
};
struct task taskList[40] = {{0,0,0}};


ssize_t readDev( struct file* file, char* buf, size_t count, loff_t* ppos )
{
	unsigned char val = 0;
	struct task* t = getTask(file);
	
	if( t->isEOF ){
		t->isEOF = 0;
		return 0;
	}

	val = inb(t->card_r);// return unsigned char
	stdError(KERN_DEBUG, "read(%u)=%u", t->card_r, val);
	copy_to_user(buf, &val, 1);
	t->isEOF = 1;
	return 1;
}


ssize_t writeDev( struct file* file, const char* buf, size_t count, loff_t* ppos )
{
	int val = 0;
	struct task* t = getTask(file);

	if( !count ){
		stdError(KERN_WARNING, "write(): Empty write => Correct usage: echo '<id Card> <value>' > /dev/" DEV_NAME);
		return -1;
	}
	
	val = str2int( buf );

	stdError(KERN_DEBUG, "Write %u at %u", val, t->card_w);
	outb( val, t->card_w );

	//unsigned inb(unsigned port);
	//void outb(unsigned char byte, unsigned port);
	//unsigned inw(unsigned port);
	//void outw(unsigned short word, unsigned port);
	//unsigned inl(unsigned port);
	//void outl(unsigned longword, unsigned port);
	return count;
}


int openDev( struct inode* inode, struct file* file )
{
	struct task* t = 0;
	
	//stdError(KERN_DEBUG, "open()");
	t = getTask(file);
	t->file = file;
	getID(file, &(t->card_w), &(t->card_r));
	
	if( !getID(file, &(t->card_w), &(t->card_r)) ){
		stdError(KERN_WARNING, "openDev(): Bad usage cat >/dev/PIO/[0-9] or cat >/dev/PIO/card_w4242_r4242");
		return -1;
	}
	return 0;
}


int closeDev( struct inode* inode, struct file* file )
{
	struct task* t = 0;
	//stdError(KERN_DEBUG, "close()");
	t = getTask(file);
	t->file = 0;
	return 0;
}
/*
int readdirDev( struct file* file, void* dirent, filldir_t filldir )
{
	return 0;
}
*/

/***************************************************************************//*!
* @brief Permet d'affecter des fonctions à: read, write, ...
*/
struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= readDev,
	.write		= writeDev,
	.open		= openDev,
	.release	= closeDev,// correspond a close
	//.readdir	= readdirDev
};



struct task* getTask( const struct file* file )
{
	int i = 0;
	struct task* ret = 0;
	for( i=0; i<40; ++i )
	{
		if( taskList[i].file == file )
			return &taskList[i];
		if( taskList[i].file == 0 )
			ret = &taskList[i];
	}
	return  ret;
}
