#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)\ 
do{\
	long timeout = (nMilliSec)*HZ/1000;\
	while(timeout > 0)\
	{\
	timeout = schedule_timeout(timeout);\
	}\
}while(0);
#endif

typedef struct{
	void *priv;
	int dev_flag;
	struct semaphore sem;
	struct cdev cdev;
	int dev_num;
	unsigned char *mmap_buf;
}SIMPLE_DEV;

SIMPLE_DEV simple_dev;

struct class *simple_class;
struct device *simple_device;
static struct task_struct *my_thread;


void tasklet_func(void *ptr)
{
	printk("<0>""enter tasklet\n");
}

DECLARE_TASKLET(my_task,tasklet_func,&simple_dev);

void my_timerfunc()
{
	printk("<0>""my timer\n");
//	tasklet_schedule(&my_task);
}

struct timer_list my_timer = TIMER_INITIALIZER(my_timerfunc,0,0);

static ssize_t simple_read(struct file *file_ptr,char *buf,size_t len,loff_t *off)
{
	int read_len;
	printk("<0>""%s %d\n",__FUNCTION__,__LINE__);
}

static ssize_t simple_write(struct file *file_ptr,char *buf,size_t len,loff_t *off)
{

	printk("<0>""%s %d\n",__FUNCTION__,__LINE__);
}

static int simple_open(struct inode *inode_ptr,struct file *file_ptr)
{
	
//	mod_timer(&my_timer,jiffies+1000*HZ);

//	add_timer(&my_timer);


	printk("<0>""%s %d\n",__FUNCTION__,__LINE__);
	return 0;
}

static int simple_close(struct inode *inode_ptr,struct file *file_ptr)
{
	printk("<0>""%s %d\n",__FUNCTION__,__LINE__);
}

static int simple_mmap(struct file *file_ptr,struct vm_area_struct *vma)
{
	
	unsigned char start = vma->vm_start;
	int len = vma->vm_end -vma->vm_start;
	int phy_map_addr = virt_to_phys(simple_dev.mmap_buf);

printk("<0>""phy size %x vir %x mmap buf %x\n",phy_map_addr,vma->vm_start,simple_dev.mmap_buf);
	//vma->vm_flags |= VM_IO;
	//vma->vm_flags |= VM_RESERVED;
	
	if(remap_pfn_range(vma,vma->vm_start,phy_map_addr>>PAGE_SIZE,len,vma->vm_page_prot))
		return -1;//_EAGAIN;
	printk("<0>""phy size %x vir %x\n",phy_map_addr,vma->vm_start);
	
	memset(simple_dev.mmap_buf,0xab,PAGE_SIZE);

	
	return 0;
}

struct file_operations simple_dev_ops={
	.owner = THIS_MODULE,
	.open = simple_open,
	.release = simple_close,
	.read = simple_read,

//	.unlocked_ioctl =simple_ioctl,
	.mmap = simple_mmap,
};

static int test_thread(void *data)
{
	while(!kthread_should_stop())
	{
		printk("<0>""thread\n");
		SLEEP_MILLI_SEC(2000);
		tasklet_schedule(&my_task);
	}
}


static __init int simple_dev_init(void)
{
	int result;
	dev_t dev_num;

	result = alloc_chrdev_region(&dev_num,0,1,"simple_dev");
	if(result < 0)
	{
		printk("<0>""fail\n");
	}	
	
	printk("<0>""dev num %d\n",dev_num);	
	
	cdev_init(&simple_dev.cdev,&simple_dev_ops);
	simple_dev.cdev.owner = THIS_MODULE;
	simple_dev.dev_num = dev_num;
	result = cdev_add(&simple_dev.cdev,dev_num,1);
	
	
	simple_class = 	class_create(THIS_MODULE,"simple_dev");

	device_create(simple_class,NULL,dev_num,NULL,"simple_dev");	

	simple_dev.mmap_buf = kmalloc(PAGE_SIZE,GFP_KERNEL);
	printk("<0>""hello world, result = %d\n",result);

	my_thread = kthread_create(test_thread,NULL,"test_thread");
	if(IS_ERR(my_thread)){
		printk("create kernel thread fail?\n");
		my_thread = NULL;
		return -1;
	}
	wake_up_process(my_thread);
	return 0;
}

static __exit void simple_dev_exit(void)
{
	device_destroy(simple_class,simple_dev.dev_num);
	class_destroy(simple_class);
	unregister_chrdev_region(simple_dev.dev_num,1);
	kfree(simple_dev.mmap_buf);
	del_timer(&my_timer);
	printk("<0>""good bye\n");
}

module_init(simple_dev_init);
module_exit(simple_dev_exit);

MODULE_LICENSE("GPL");
