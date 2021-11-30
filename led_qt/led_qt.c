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
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>

struct gpio_desc *led_gpiod;
static dev_t led_qt_devt;
static struct cdev led_qt_cdev;
static struct class *led_qt_class;
static struct device *led_qt_dev;

static int led_qt_open(struct inode *node, struct file *file){
    printk("opened");
    gpiod_direction_output(led_gpiod,0);
    return 0;
}

static int led_qt_write(struct file *file, const char __user *buf, size_t size, loff_t *offset){
    char value=0;
    int ret=0;
    ret = copy_from_user(&value,buf,size);
    if(value==0)
        gpiod_set_value(led_gpiod,0);
    else
        gpiod_set_value(led_gpiod,1);
    return 0;
}


//file_operations结构体
static struct file_operations led_qt_fops = {
    .owner = THIS_MODULE,
    .open = led_qt_open,
    .write = led_qt_write,
};


//probe函数 获取资源 注册file_operations
static int led_qt_probe(struct platform_device *pdev){
    int err;

    //获取属性
    //gpiod
    led_gpiod = devm_gpiod_get(&pdev->dev,"led",0);
    //注册file_operations结构体
    //动态分配获得设备号
    err = alloc_chrdev_region(&led_qt_devt,0,1,"led_qt");
    if(err < 0 ){
        return -1;
    }
    //关联cdev和file_operations结构体
    led_qt_cdev.owner = THIS_MODULE;
    cdev_init(&led_qt_cdev,&led_qt_fops);
    //添加cdev至散列表
    err = cdev_add(&led_qt_cdev,led_qt_devt,1);
    if( err < 0 ){
        //注销设备号
        unregister_chrdev_region(led_qt_devt, 1);
    }
    //创建类和设备
    led_qt_class = class_create(THIS_MODULE,"led_qt");
    led_qt_dev = device_create(led_qt_class,NULL,led_qt_devt,NULL,"led_qt");
    printk("device created\n");
    return 0;
}

static int led_qt_remove(struct platform_device *pdev){
    device_destroy(led_qt_class,led_qt_devt);
    class_destroy(led_qt_class);
    cdev_del(&led_qt_cdev);
    unregister_chrdev_region(led_qt_devt,1);
    return 0;
}

//定义platform_driver结构体
static const struct of_device_id led_qt_match_table[] = {
    {.compatible = "led-qt"}, //和设备树中信息一致
    {},
};

static struct platform_driver led_qt_driver = {
    .probe = led_qt_probe,
    .remove = led_qt_remove,
    .driver = {
        .name = "led-qt",
        .of_match_table = led_qt_match_table,
    },
};

//入口函数 注册platform_driver结构体
static int __init led_qt_platform_driver_init(void){
    int err;
    err = platform_driver_register(&led_qt_driver);
    return 0;
}

static void __exit led_qt_platform_driver_exit(void){
    platform_driver_unregister(&led_qt_driver);
}

module_init(led_qt_platform_driver_init);
module_exit(led_qt_platform_driver_exit);

MODULE_LICENSE("GPL");