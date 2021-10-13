KERNEL_DIR= /home/book/100ask_imx6ull-sdk/Linux-4.9.88

obj-m :=icm20608.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o icm20608_app icm20608_app.c

.PHONE:clean 

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -rf modules.order
	rm -f icm20608_app