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

struct led_key{
    /* data */
    int key_irq;
    struct gpio_desc *led_gpiod;
    struct gpio_desc *key_gpiod;
};
//设备结构体指针
static struct led_key *pled_key;
static dev_t led_key_devt;
static struct cdev led_key_cdev;
static struct class *led_key_class;
static struct device *led_key_dev;
static bool open = 0;

static int led_key_open(struct inode *node, struct file *file){
    printk("opened");
    gpiod_direction_input(pled_key->key_gpiod);
    gpiod_direction_output(pled_key->led_gpiod,0);
    return 0;
}
//file_operations结构体
static struct file_operations led_key_opr = {
    .owner = THIS_MODULE,
    .open = led_key_open,
};

//中断服务函数
static irqreturn_t led_key_irq(int irq,void *dev_id) {
    //struct led_key *led_key_dev = dev_id;
    int value = 0;
    if ( open ) {
        value = 1;
    }
    else {
        value = 0;
    }
    open = !open;
    printk("value %d \n", value);
    gpiod_set_value(pled_key->led_gpiod,value);
    return IRQ_HANDLED;
}

//probe函数 获取资源 注册file_operations
static int led_key_probe(struct platform_device *pdev){
    int err;
    pled_key = kzalloc(sizeof(struct led_key),GFP_KERNEL);

    //找到节点
    //struct device_node *np = pdev->dev.of_node;

    //获取属性
    //gpio
    pled_key->led_gpiod = devm_gpiod_get(&pdev->dev,"led",0);
    pled_key->key_gpiod = devm_gpiod_get(&pdev->dev,"key",0);

    //irq
    pled_key->key_irq = gpiod_to_irq(pled_key->key_gpiod);
    err = request_irq(pled_key->key_irq,led_key_irq,IRQF_TRIGGER_RISING,"led_key",&pled_key);

    //注册file_operations结构体
    //动态分配获得设备号
    err = alloc_chrdev_region(&led_key_devt,0,1,"led_key");
    if(err < 0 ){
        return -1;
    }
    //关联cdev和file_operations结构体
    led_key_cdev.owner = THIS_MODULE;
    cdev_init(&led_key_cdev,&led_key_opr);
    //添加cdev至散列表
    err = cdev_add(&led_key_cdev,led_key_devt,1);
    if( err < 0 ){
        //注销设备号
        unregister_chrdev_region(led_key_devt, 1);
    }
    //创建类和设备
    led_key_class = class_create(THIS_MODULE,"led_key");
    led_key_dev = device_create(led_key_class,NULL,led_key_devt,NULL,"led_key");
    return 0;
}

static int led_key_remove(struct platform_device *pdev){
    device_destroy(led_key_class,led_key_devt);
    class_destroy(led_key_class);
    cdev_del(&led_key_cdev);
    unregister_chrdev_region(led_key_devt,1);
    kfree(pled_key);
    return 0;
}

//定义platform_driver结构体
static const struct of_device_id led_key_match_table[] = {
    {.compatible = "led-key"}, //和设备树中信息一致
    {},
};

static struct platform_driver led_key_driver = {
    .probe = led_key_probe,
    .remove = led_key_remove,
    .driver = {
        .name = "led_key",
        .of_match_table = led_key_match_table,
    },
};

//入口函数 注册platform_driver结构体
static int __init led_key_platform_driver_init(void){
    int err;
    err = platform_driver_register(&led_key_driver);
    return 0;
}

static void __exit led_key_platform_driver_exit(void){
    platform_driver_unregister(&led_key_driver);
}

module_init(led_key_platform_driver_init);
module_exit(led_key_platform_driver_exit);

MODULE_LICENSE("GPL");


