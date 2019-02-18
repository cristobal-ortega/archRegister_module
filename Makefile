obj-m = proc_module.o
KVERSION = $(shell uname -r)
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

