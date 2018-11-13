#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/types.h>	//dev_t
#include<linux/fs.h>
#include<linux/moduleparam.h>	//moudle_param
#include<linux/cdev.h>	//struct cdev
#include <linux/uaccess.h>	//copy_to_user
#include"mem_mod.h"

static unsigned int mem_dev_major=MEMDEV_MAJOR;
//moudle_param(mem_dev_major, uint ,S_IRUGO);
struct mem_dev* pmem_dev;//设备结构体
struct cdev cdev;

int mem_open(struct inode* pinode,struct file* pfile){
	struct mem_dev* dev;

	int minor=MINOR(pinode->i_rdev);

	if(minor>=MEMDEV_NR_DEVS)
		return -ENODEV;
	dev=&pmem_dev[minor];

	pfile->private_data=dev;

	return 0;
}

int mem_release(struct inode* pinode,struct file* pfile){
	return 0;
}
static ssize_t mem_read(struct file* pfile,char __user * buf,size_t size,loff_t* ppos){
	loff_t p=*ppos;//记录开始的偏移
	size_t count=size;//需要读取的字节数
	int ret=0;
	struct mem_dev* dev=pfile->private_data;

	if(p>=MEMDEV_SIZE){//要读取的偏移大于MEMDEV_SIZE
		ret=0;
		goto read_end;
	}
	if(count>MEMDEV_SIZE-p)//最多读取MEMDEV_SIZE-p字节
		count=MEMDEV_SIZE-p;
	
	//读数据到用户空间
	if(copy_to_user(buf,(void*)(dev->data+p),count)){
		ret=-EFAULT;
		goto read_end;
	} else {
		*ppos+=count;
		ret=count;
	}

	printk(KERN_INFO "read %ld byte(s) from %lld\n",count,p);

	read_end:
	return ret;
}

static ssize_t mem_write(struct file* pfile,const char __user *buf,size_t size,loff_t* ppos){
	loff_t p=*ppos;//记录开始的偏移
	size_t count=size;//需要写入的字节数
	int ret=0;
	struct mem_dev* dev=pfile->private_data;

	if(p>=MEMDEV_SIZE){//要写入的偏移大于MEMDEV_SIZE
		ret=0;
		goto read_end;
	}
	if(count>MEMDEV_SIZE-p)//最多写入MEMDEV_SIZE-p字节
		count=MEMDEV_SIZE-p;
	
	//读数据到用户空间
	if(copy_from_user((void*)(dev->data+p),buf,count)){
		ret=-EFAULT;
		goto read_end;
	} else {
		*ppos+=count;
		ret=count;
	}

	printk(KERN_INFO "write %ld byte(s) to %lld\n",count,p);

	read_end:
	return ret;
}

//文件定位函数
static loff_t mem_llseek(struct file* pfile,loff_t offset,int whence){
	loff_t newpos;
	switch(whence){
		case 0://SEEK_SET，相对开始位置
			newpos=offset;
			break;
		case 1://SEEK_CUR
			newpos=pfile->f_pos+offset;
			break;
		case 2://SEEK_END
			newpos=MEMDEV_SIZE-1+offset;
			break;
		default:
			return -EINVAL;
	}
	if(newpos<0 || newpos>MEMDEV_SIZE)
		return -EINVAL;
	pfile->f_pos=newpos;
	return newpos;
}

static const struct file_operations mem_fops={
	.owner=THIS_MODULE,
	.llseek=mem_llseek,
	.read=mem_read,
	.write=mem_write,
	.open=mem_open,
	.release=mem_release
};

static int mem_dev_init(void){
	int result;
	int i;
	dev_t devno=MKDEV(mem_dev_major,0);

	/*
	if(mem_dev_major){
		result=register_chrdev_region(devno, 2, "mem_dev");
	} else {
	*/	
		result = alloc_chrdev_region(&devno, 0, 2, "mem_dev");
    	mem_dev_major = MAJOR(devno);

	//}

	if(result<0){
		goto fail_register;
	}
	
	printk("mem_dev_major : %d\n",mem_dev_major);

	cdev_init(&cdev,&mem_fops);
	cdev.owner=THIS_MODULE;
	cdev.ops=&mem_fops;

	//注册字符设备
	cdev_add(&cdev, MKDEV(mem_dev_major, 0), MEMDEV_NR_DEVS);

	printk("char dev added\n");

	pmem_dev=kmalloc(MEMDEV_NR_DEVS*sizeof(struct mem_dev),GFP_KERNEL);
	if(!pmem_dev){
		result=-ENOMEM;
		printk("kmalloc fail\n");
		goto fail_malloc;
	}
	memset(pmem_dev,0,MEMDEV_NR_DEVS*sizeof(struct mem_dev));

	for(i=0;i<MEMDEV_NR_DEVS;++i){
		pmem_dev[i].size=MEMDEV_SIZE;
		pmem_dev[i].data=kmalloc(MEMDEV_SIZE,GFP_KERNEL);
		memset(pmem_dev[i].data,0,MEMDEV_SIZE);
	}

	printk("mem_mod inited\n");
	return 0;
	fail_register:
		return result;
	fail_malloc:
		unregister_chrdev_region(devno, 1);
		return result;
}

static void mem_dev_exit(void){
	cdev_del(&cdev);
	printk("char dev deleted\n");
	int i;
	for(i=0;i<MEMDEV_NR_DEVS;++i){
		kfree(pmem_dev[i].data);
	}
	kfree(pmem_dev);
	unregister_chrdev_region(MKDEV(mem_dev_major,0), 2);
}

MODULE_AUTHOR("dts");
MODULE_DESCRIPTION("内存字符设备mem_dev");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

module_init(mem_dev_init);
module_exit(mem_dev_exit);

