#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>


typedef struct{
	void *priv;
	int dev_flag;
	struct semaphore sem;
	struct cdev cdev;
}SIMPLE_DEV;

SIMPLE_DEV simple_dev;

struct class *sample_class;
struct device *sample_device;


struct file_operations simple_dev_ops={
	.owner = THIS_MODULE,
	.open = NULL,//simple_open,
	.release = NULL,//simple_close,
	.read = NULL,//simple_read,
	.write = NULL,//simple_write,
//	.ioctl = NULL,//simple_ioctl,
};



static __init int simple_dev_init(void)
{
	int result;
	dev_t dev_num;

	result = alloc_chrdev_region(&dev_num,0,1,"sample_dev");
	if(result < 0)
	{
		printk("<0>""fail\n");
	}	
	
	printk("<0>""dev num %d\n",dev_num);	
	
	cdev_init(&simple_dev.cdev,&simple_dev_ops);
	simple_dev.cdev.owner = THIS_MODULE;
	result = cdev_add(&simple_dev.cdev,dev_num,1);
	
	
	sample_class = 	class_create(THIS_MODULE,"sample_dev");

	device_create(sample_class,NULL,dev_num,NULL,"sample_new_dev");	

	printk("<0>""hello world, result = %d\n",result);
	return 0;
}

static __exit void simple_dev_exit(void)
{
	printk("<0>""good bye\n");
}

module_init(simple_dev_init);
module_exit(simple_dev_exit);

MODULE_LICENSE("GPL");
