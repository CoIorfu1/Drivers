KERNEL_DIR= /home/book/100ask_imx6ull-sdk/Linux-4.9.88

obj-m :=ap3216c.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o ap3216c_app ap3216c_test.c

.PHONE:clean 

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -rf modules.order
	rm -f ap3216c_app