#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/nvmem-provider.h>
#include <linux/regmap.h>
#include <linux/pm_runtime.h>
#include <linux/uaccess.h>

static struct i2c_client *ap3216c_client;
static dev_t ap3216c_dnum;
static struct cdev ap3216c_cdev;
static struct class *ap3216c_class;
static struct device *ap3216c_device;

static ssize_t ap3216c_read(struct file *file, char __user *buf, size_t size, loff_t *offset){
    int err;
	char kernel_buf[6];
	int val;
	
	if (size != 6)
		return -EINVAL;

	val = i2c_smbus_read_word_data(ap3216c_client, 0xA); /* read IR */
	kernel_buf[0] = val & 0xff;
	kernel_buf[1] = (val>>8) & 0xff;
	
	val = i2c_smbus_read_word_data(ap3216c_client, 0xC); /* read 光强 */
	kernel_buf[2] = val & 0xff;
	kernel_buf[3] = (val>>8) & 0xff;

	val = i2c_smbus_read_word_data(ap3216c_client, 0xE); /* read 距离 */
	kernel_buf[4] = val & 0xff;
	kernel_buf[5] = (val>>8) & 0xff;
	
	err = copy_to_user(buf, kernel_buf, size);
	return size;
}

static int ap3216c_open(struct inode *node, struct file *file){
    //重置设备
    i2c_smbus_write_byte_data(ap3216c_client,0,0x4);
    mdelay(20);
    //使能设备
    i2c_smbus_write_byte_data(ap3216c_client,0,0x3);
    mdelay(250);
    return 0;
}

static struct file_operations ap3216c_opr = {
    .open = ap3216c_open,
    .read = ap3216c_read,
};

static int ap3216c_probe(struct i2c_client * client, const struct i2c_device_id *id){
    int err;
    //获得i2c_client结构体
    ap3216c_client = client;

    //注册设备
    err = alloc_chrdev_region(&ap3216c_dnum,0,1,"ap3216c");
    if(err != 0)
        return -1;
    ap3216c_cdev.owner = THIS_MODULE;
    cdev_init(&ap3216c_cdev,&ap3216c_opr);
    err = cdev_add(&ap3216c_cdev,ap3216c_dnum,1);
    if(err != 0){
        unregister_chrdev_region(ap3216c_dnum,1);
        return -1;
    }
    ap3216c_class = class_create(THIS_MODULE,"ap3216c");
    ap3216c_device = device_create(ap3216c_class,NULL,ap3216c_dnum,NULL,"ap3216c");

    return 0;
}

static int ap3216c_remove(struct i2c_client *client){
    //注销设备
    device_destroy(ap3216c_class,ap3216c_dnum);
    class_destroy(ap3216c_class);
    cdev_del(&ap3216c_cdev);
    unregister_chrdev_region(ap3216c_dnum,1);

    return 0;
}

static const struct i2c_device_id ap3216c_id_table[] = {
    {"ap3216c" , (kernel_ulong_t)NULL},
    {},
};

static const struct of_device_id ap3216c_match_table[] = {
    {.compatible = "ap3216c", .data = NULL},
    {},
};

static struct i2c_driver ap3216c_driver = {
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .id_table = ap3216c_id_table,
    .driver = {
        .name = "ap3216c",
        .of_match_table = ap3216c_match_table,
    }
};

static int __init ap3216c_init(void){
     //注册i2c_driver结构体      
    return i2c_add_driver(&ap3216c_driver);
}

static void __exit ap3216c_exit(void){
    //注销i2c_driver结构体
    i2c_del_driver(&ap3216c_driver);
}

module_init(ap3216c_init);
module_exit(ap3216c_exit);
MODULE_LICENSE("GPL");