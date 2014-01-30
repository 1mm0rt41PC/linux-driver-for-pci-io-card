#include "proc.h"
#include "extra.h"
#include <linux/proc_fs.h>

// Voir: http://www.linuxtopia.org/online_books/Linux_Kernel_Module_Programming_Guide/x714.html

#define PROC_FS_NAME "pci-7250-0"

// This structure hold information about the /proc file
struct proc_dir_entry* G_procfs;
/*
struct proc_dir_entry {
	unsigned int low_ino;
	umode_t mode;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	const struct inode_operations *proc_iops;
	*
	 * NULL ->proc_fops means "PDE is going away RSN" or
	 * "PDE is just created". In either case, e.g. ->read_proc won't be
	 * called because it's too late or too early, respectively.
	 *
	 * If you're allocating ->proc_fops dynamically, save a pointer
	 * somewhere.
	 *
	const struct file_operations *proc_fops;
	struct proc_dir_entry *next, *parent, *subdir;
	void *data;
	read_proc_t *read_proc;
	write_proc_t *write_proc;
	atomic_t count;// use count
	int pde_users;// number of callers into module in progress
	struct completion *pde_unload_completion;
	struct list_head pde_openers;// who did ->open, but not ->release
	spinlock_t pde_unload_lock;// proc_fops checks and pde_users bumps
	u8 namelen;
	char name[];
};
*/






char init_procfs(void)
{
	if( (G_procfs = create_proc_entry(PROC_FS_NAME, 0644, NULL)) == NULL ){
		remove_proc_entry(PROC_FS_NAME, NULL);
		stdError(KERN_ALERT, "Error: Could not initialize /proc/%s", PROC_FS_NAME);
		return -1;
	}

	G_procfs->read_proc		= procfile_read;
	G_procfs->write_proc	= procfile_write;
	G_procfs->mode			= S_IFREG | S_IRUGO;
	G_procfs->uid			= 0;
	G_procfs->gid			= 0;
	G_procfs->size			= 37;
	G_procfs->data			= (void*)0x1;// Carte 1 (on évite le 0 pour le non initialisé)

	stdError(KERN_INFO, "/proc/%s created", PROC_FS_NAME);
	return 0;// everything is ok.
}

void cleanup_procfs(void)
{
	remove_proc_entry(PROC_FS_NAME, NULL);
	stdError(KERN_INFO, "/proc/%s removed", PROC_FS_NAME);
}





/* Put data into the proc fs file.
 *
 * Arguments
 * =========
 * 1. The buffer where the data is to be inserted, if
 *    you decide to use it.
 * 2. A pointer to a pointer to characters. This is
 *    useful if you don't want to use the buffer
 *    allocated by the kernel.
 * 3. The current position in the file
 * 4. The size of the buffer in the first argument.
 * 5. Write a "1" here to indicate EOF.
 * 6. A pointer to data (useful in case one common
 *    read for multiple /proc/... entries)
 *
 * Usage and Return Value
 * ======================
 * A return value of zero means you have no further
 * information at this time (end of file). A negative
 * return value is an error condition.
 *
 * For More Information
 * ====================
 * The way I discovered what to do with this function
 * wasn't by reading documentation, but by reading the
 * code which used it. I just looked to see what uses
 * the get_info field of proc_dir_entry struct (I used a
 * combination of find and grep, if you're interested),
 * and I saw that  it is used in <kernel source
 * directory>/fs/proc/array.c.
 *
 * If something is unknown about the kernel, this is
 * usually the way to go. In Linux we have the great
 * advantage of having the kernel source code for
 * free - use it.
 */
int procfile_read( char* buffer, char** buffer_location, off_t offset, int buffer_length, int* eof, void* data )
{
	int ret=0;
	stdError(KERN_INFO, "procfile_read (/proc/%s) called", PROC_FS_NAME);
	if( data ){
		stdError(KERN_INFO, "Read the card number %d", (int)data);
	}


	/*
	 * We give all of our information in one go, so if the
	 * user asks us if we have more information the
	 * answer should always be no.
	 *
	 * This is important because the standard read
	 * function from the library would continue to issue
	 * the read system call until the kernel replies
	 * that it has no more information, or until its
	 * buffer is filled.
	 */
	if( offset > 0 ){
		/* we have finished to read, return 0 */
		ret  = 0;
		*eof = 1;// Fin de fichier
	}else{
		/* fill the buffer, return the buffer size */
		ret = sprintf(buffer, "HelloWorld!\n");
		*eof = 1;// Fin de fichier
	}

	return ret;
}


int procfile_write( struct file* file, const char* __user buf, unsigned long count, void* data )
{
	stdError(KERN_INFO, "write into PCI %s", buf);
	return count;
}
