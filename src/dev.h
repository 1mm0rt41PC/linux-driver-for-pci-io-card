#ifndef DEV_h
#define DEV_h
#include "extra.h"

extern struct file_operations fops;


ssize_t readDev( struct file* file, char* buf, size_t count, loff_t* ppos );
ssize_t writeDev( struct file* file, const char* buf, size_t count, loff_t* ppos );
int openDev( struct inode* inode, struct file* file );
int closeDev( struct inode* inode, struct file* file );
struct task;
struct task* getTask( const struct file* file );

#endif// DEV_h
