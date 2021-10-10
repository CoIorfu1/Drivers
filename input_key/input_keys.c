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
#include <linux/slab.h>
#include <linux/input.h>

//设备结构体
struct keys_device {
    struct gpio_desc *gpiod;
    int irq;
};

static struct input_dev *keys_input_dev;
static struct keys_device *keys_device;

static irqreturn_t key_irq(int irq,void *dev_id) {
    struct keys_device *key_device = dev_id; 
    printk("irq_handler");
    if(key_device->irq == keys_device[0].irq){
    printk("key1");
    input_event(keys_input_dev,EV_KEY,KEY_1,1);
    input_sync(keys_input_dev);
    return IRQ_HANDLED;
    }
    else if(key_device->irq == keys_device[1].irq){
    printk("key2");
    input_event(keys_input_dev,EV_KEY,KEY_4,1);
    input_sync(keys_input_dev);
    return IRQ_HANDLED;
    }
    return IRQ_HANDLED;
}

static int keys_device_probe(struct platform_device *pdev){
    int err;
    int i;
    int count=0;
    printk("into probe");

    keys_device = kzalloc(sizeof(struct keys_device)*2,GFP_KERNEL);
    printk("kzalloc ok");

    //input_dev结构体
    keys_input_dev = devm_input_allocate_device(&pdev->dev);
    
    keys_input_dev->name = "input-keys";
    keys_input_dev->phys = "input-keys";

    __set_bit(EV_KEY,keys_input_dev->evbit);
    __set_bit(KEY_1,keys_input_dev->keybit);
    __set_bit(KEY_4,keys_input_dev->keybit);

    err = input_register_device(keys_input_dev);
    if(err){
        printk("input_dev fail");
        return -1;
    }

    //获取资源
    count = of_gpio_named_count(pdev->dev.of_node,"keys-gpios");
    printk("count =%d",count);
    for(i=0;i<count;++i){
        keys_device[i].gpiod = devm_gpiod_get_index(&pdev->dev,"keys",i,0);
        keys_device[i].irq = gpiod_to_irq(keys_device[i].gpiod);
    }
    for(i=0;i<count;++i){
        err = request_irq(keys_device[i].irq,key_irq,IRQF_TRIGGER_RISING,"input-keys",&keys_device[i]);
        if(err != 0){
            free_irq(keys_device[i].irq,NULL);
            printk("irq fail");
            return -1;
        }
    printk("irq ok");
    }

    return 0;
}

static int keys_device_remove(struct platform_device *pdev){
    free_irq(keys_device[0].irq,NULL);
    free_irq(keys_device[1].irq,NULL);
    input_unregister_device(keys_input_dev);
    kfree(keys_device);
    return 0;
}

static const struct of_device_id keys_device_match_table[] = {
    {.compatible = "input-keys"},
    {},
};

static struct platform_driver keys_driver = {
    .probe = keys_device_probe,
    .remove = keys_device_remove,
    .driver = {
        .name = "input-keys",
        .of_match_table = keys_device_match_table,
    },
};

static int __init keys_device_init(void) {
    int err;
    err = platform_driver_register(&keys_driver);
    return 0;
}

static void __exit keys_device_exit(void) {
    platform_driver_unregister(&keys_driver);
}

module_init(keys_device_init);
module_exit(keys_device_exit);
MODULE_LICENSE("GPL");