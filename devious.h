#ifndef DEVIOUS_H_
#define DEVIOUS_H_
#include <linux/compiler.h>

__must_check int register_device(void);
void unregister_device(void);

#endif

