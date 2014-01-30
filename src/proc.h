#ifndef PROC_h
#define PROC_h
#include <linux/module.h>	/* Specifically, a module */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */

char init_procfs(void);
void cleanup_procfs(void);
int procfile_read( char* buffer, char** buffer_location, off_t offset, int buffer_length, int* eof, void* data );
int procfile_write( struct file*file, const char* __user buf, unsigned long count, void* data );

#endif// PROC_h
