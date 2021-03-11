TARGET_MODULE:=PiCharDrv

ifneq ($(KERNELRELEASE),)
	hostprogs-y := pihelper
	always := $(hostprogs-y)
	HOSTCFLAGS_pihelper.o += -I$(objtree)/usr/include -lm
	$(TARGET_MODULE)-objs := main.o devious.o
	obj-m := $(TARGET_MODULE).o
else
	BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)

all:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

load:
	insmod ./$(TARGET_MODULE).ko

unload:
	rmmod ./$(TARGET_MODULE).ko

endif
