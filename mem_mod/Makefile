obj-m := mem_mod.o

KERNEL_DIR := ../headers/linux-headers-4.15.0-38-generic
PWD := $(shell pwd)
MEMDEV_NO :=  $(shell cat /proc/devices | grep mem_dev | awk '{print ($$1)}')

all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.o .*.cmd *.order .tmp_versions Module.symvers .cache.mk *.out *.mod.c
install:
	sudo insmod mem_mod.ko
unstall:
	sudo rmmod mem_mod
cat_devs:
	cat /proc/devices | grep mem_dev
test:
	#查找mem_dev的设备号 并为其构造结点
	sudo mknod /dev/memdev0 c $(MEMDEV_NO) 0
	#允许读写
	sudo chmod 666 /dev/memdev0
	#查看/dev中的memdev结点
	ls /dev/ | grep memdev
write:
	#法 1，通过c标准库fwrite写入
	#gcc write.c -o write.out 
	#sudo ./write.out 'This is a mem_char_dev'

	#法 2，直接用shell写入
	sudo echo "This is a char dev!">/dev/memdev0
read:
	#读设备文件内容
	 cat /dev/memdev0
.PHONY:clean

