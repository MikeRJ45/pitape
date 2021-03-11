/* Babby's first kernel module
 * scaffolding for a stupid ass
 * pi "faucet" char device
**/
#include "devious.h"
#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Char Device, write pi index, read pi digit at index+1");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("YoMomma, Inc");

static int mod_init(void)
{
	int result = 0;
	printk(KERN_NOTICE "PiCharDrv: Initilization...\n");
	result = register_device();
	return result;
}

static void mod_exit(void)
{
	printk(KERN_NOTICE "PiCharDrv: Exit\n");
	unregister_device();
}

module_init(mod_init);
module_exit(mod_exit);
