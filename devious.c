/* Pi Char Device driver
 * Pointless exercise in kernel char dev
 * Michael Johnson 11-5-2020
 */

#include "devious.h"
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kmod.h>
#include <linux/limits.h>

/* Neat fact, gcc silently tries to use
 * __divmodd4i from stl when attempting 'val mod base'
 * where base is 10, stick to 16, everythings hex anyway
 *
 * Simple loff_t(long long int) to hex char array
 * */

char* lofftoa(loff_t val)
{
	static char buf[64] = {0};
	int i = 62;
	if(val == 0) return "0";
	for(; val && i ; --i, val /= 16) {
		buf[i] = "0123456789abcdef"[val % 16];
	}

	return &buf[i+1];
}

/* Obviously, we don't have the math libraries
 * to implement the BBP algo in kernel
 * so we have a userspace helper do that
 * using the user mode API the kernel provides
 */
char* call_pihelper(loff_t index)
{
	char* idx[] = {lofftoa(index)};
	char* argv[] = {"/home/mike/pifs/mkultra/pihelper", *idx, NULL};
	static char* envp[] = {
		"HOME=/"
		, "TERM=linux"
		, "PATH=/sbin:/bin:/usr/sbin:/usr/bin"
		, NULL
	};
	printk(KERN_NOTICE "PiCharDrv User space called with value: %s\n Original Index: %lld\n", *idx, index);
	/* *
	 * wrapper function that does call_usermode_setup and call_usermode_exec.
	 * UMH_WAIT_PROC - wait for process to return.
	 * */
	return (char*)call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

static ssize_t device_file_read(
		struct file *file_ptr
		, char __user *user_buffer
		, size_t count
		, loff_t *possition)
{
	char* junk = {call_pihelper(*possition)};

	if(*possition >= INT_MAX) //still pretty big
	{
		printk(KERN_NOTICE "PiCharDrv Read: Possition end!");
		return 0;
	}
	if(count > INT_MAX)
	{
		printk(KERN_NOTICE "PiCharDrv Read: Count too big!");
		return 0;
	}

	if(*possition + count > INT_MAX) {
		printk(KERN_NOTICE "Over INT_MAX");
		count = INT_MAX - *possition;
	}
		
	if(copy_to_user(user_buffer, &junk, count) !=0)
	{
		return -EFAULT;
	}
	/* - deadlock from printk! neat! */
	//printk(KERN_NOTICE "PiCharDrv user buffer %s", junk);

	*possition += count;
	return count;
}

static struct file_operations pidrv_fops =
{
	.owner = THIS_MODULE,
	.read = device_file_read,
};
 
static int dev_maj_num = 0;
static const char device_name[] = "PiCharDrv";

int register_device(void)
{
	int result = 0;

	printk(KERN_NOTICE "PiCharDrv: Reggo Called. \n");

	result = register_chrdev(0,device_name, &pidrv_fops);
	if(result < 0 )
	{
		printk(KERN_WARNING "PiCharDrv: No Reggo, Boom Boom: %i\n"
				, result);
		return result;
	}

	dev_maj_num = result;
	printk(KERN_NOTICE "PiCharDrv: Reggo'ed! Major Num: %i\n", dev_maj_num);
	return 0;
}

void unregister_device(void)
{
	printk(KERN_NOTICE "PiCharDrv: Unreg Called.\n");
	if(dev_maj_num != 0)
	{
		unregister_chrdev(dev_maj_num, device_name);
	}
}

