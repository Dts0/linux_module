#ifndef MEM_MOD_H
#define MEM_MOD_H

struct mem_dev{
	char* data;
	unsigned long size;
};

//#define MEMDEV_MAJOR 251 //默认主设备号，已弃用，当前为动态获取主设备号
#define MEMDEV_NR_DEVS 2 //设备数
#define MEMDEV_SIZE 1024 //大小

#endif
