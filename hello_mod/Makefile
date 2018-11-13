obj-m := hello_mod.o

KERNEL_DIR := headers/linux-headers-4.15.0-29-generic
PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.o .*.cmd *.order .tmp_versions Module.symvers .cache.mk
.PHONY:clean

