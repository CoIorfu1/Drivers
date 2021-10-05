KERNEL_DIR= /home/book/100ask_imx6ull-sdk/Linux-4.9.88

obj-m :=led_key_driver.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o led_key_app led_key_app.c

.PHONE:clean 

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -rf modules.order
	rm -f led_key_app
