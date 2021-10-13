#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

static struct spi_device *icm20608_spi_device;
static int icm20608_gpio;
static dev_t icm20608_dnum;
static struct cdev icm20608_cdev;
static struct class *icm20608_class;
static struct device *icm20608_device;

signed int gyro_x_adc;		/* 陀螺仪X轴原始值 	 */
signed int gyro_y_adc;		/* 陀螺仪Y轴原始值		*/
signed int gyro_z_adc;		/* 陀螺仪Z轴原始值 		*/
signed int accel_x_adc;		/* 加速度计X轴原始值 	*/
signed int accel_y_adc;		/* 加速度计Y轴原始值	*/
signed int accel_z_adc;		/* 加速度计Z轴原始值 	*/
signed int temp_adc;		/* 温度原始值 			*/

static void icm20608_write_reg(struct spi_device *spi,u8 reg,u8 value){
    u8 *txdata;
    txdata = kzalloc(2*sizeof(u8),GFP_KERNEL);
    txdata[0] = reg &~0x80;
    memcpy(&txdata[1],&value,sizeof(u8));
    spi_write(spi,txdata,2*sizeof(u8));
    kfree(txdata);
}

static int icm20608_open(struct inode *node, struct file *file){
    u8 value = 0;
    u8 reg = 0;
    
    //片选拉低选中
    gpio_direction_output(icm20608_gpio,1);

    //初始化icm20608
    icm20608_write_reg(icm20608_spi_device,0x6B,0x80);
    mdelay(50);
    icm20608_write_reg(icm20608_spi_device,0x6B,0x01);
    mdelay(50);

    reg = 0x75 | 0x80;
    spi_write_then_read(icm20608_spi_device,&reg,1,&value,1);
    printk("icm20608 ID = %#x\r\n",value);

    icm20608_write_reg(icm20608_spi_device,0x19, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_reg(icm20608_spi_device, 0x1b, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_reg(icm20608_spi_device, 0x1c, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write_reg(icm20608_spi_device, 0x1a, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_reg(icm20608_spi_device, 0x1d, 0x04); /* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_reg(icm20608_spi_device, 0x6c, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_reg(icm20608_spi_device, 0x1e, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_reg(icm20608_spi_device, 0x23, 0x00);		/* 关闭FIFO						*/
    return 0;
}

static int icm20608_read(struct file *file, char __user *buf, size_t size, loff_t *offset){
    unsigned char data[14];
    signed int ret[7];
    int err;
    u8 reg = 0x3b | 0x80; 

    spi_write_then_read(icm20608_spi_device,&reg,1,&data,14); 
    accel_x_adc = (signed short)((data[0] << 8) | data[1]); 
	accel_y_adc = (signed short)((data[2] << 8) | data[3]); 
	accel_z_adc = (signed short)((data[4] << 8) | data[5]); 
	temp_adc    = (signed short)((data[6] << 8) | data[7]); 
	gyro_x_adc  = (signed short)((data[8] << 8) | data[9]); 
	gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
	gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);
    
    ret[0] = gyro_x_adc;
	ret[1] = gyro_y_adc;
	ret[2] = gyro_z_adc;
	ret[3] = accel_x_adc;
	ret[4] = accel_y_adc;
	ret[5] = accel_z_adc;
	ret[6] = temp_adc;
	err = copy_to_user(buf, ret, sizeof(ret));
	return 0;
}

static struct file_operations icm20608_fops = {
    .open = icm20608_open,
    .read = icm20608_read,
};

static int icm20608_probe(struct spi_device *spi){
    int err;
    //获取片选引脚
    struct device_node *node;
    node = of_find_node_by_path("/soc/aips-bus@02000000/spba-bus@02000000/ecspi@02010000");
    icm20608_gpio = of_get_named_gpio(node,"cs-gpios",0);

    //获取spi_device结构体
    icm20608_spi_device = spi;
    //设置spi_device结构体
    icm20608_spi_device->mode = 0;
    spi_setup(icm20608_spi_device);

    //注册file_operations
    err = alloc_chrdev_region(&icm20608_dnum,0,1,"icm20608");
    if(err<0) return -1;
    icm20608_cdev.owner = THIS_MODULE;
    cdev_init(&icm20608_cdev,&icm20608_fops);
    err = cdev_add(&icm20608_cdev,icm20608_dnum,1);
    if(err<0){
        unregister_chrdev_region(icm20608_dnum,1);
        return -1;
    }
    icm20608_class = class_create(THIS_MODULE,"icm20608");
    icm20608_device = device_create(icm20608_class,NULL,icm20608_dnum,NULL,"icm20608");

    return 0;
}

static int icm20608_remove(struct spi_device *spi){
    device_destroy(icm20608_class,icm20608_dnum);
    class_destroy(icm20608_class);
    cdev_del(&icm20608_cdev);
    unregister_chrdev_region(icm20608_dnum,1);
    return 0;
}

static const struct of_device_id icm20608_match_table[] = {
    {.compatible = "icm20608"},
    {}
};

static const struct spi_device_id icm20608_id_table[] = {
    {"icm20608",0},
    {}
};

static struct spi_driver icm20608_driver = {
    .probe = icm20608_probe,
    .remove = icm20608_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "icm20608",
        .of_match_table = icm20608_match_table,
    },
    .id_table = icm20608_id_table,
};

static int __init icm20608_init(void){
    int err;
    err = spi_register_driver(&icm20608_driver);
    if (err)
        return -1;
    return 0;
}

static void __exit icm20608_exit(void){
    spi_unregister_driver(&icm20608_driver);
}

module_init(icm20608_init);
module_exit(icm20608_exit);
MODULE_LICENSE("GPL");