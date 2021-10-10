KERNEL_DIR= /home/book/100ask_imx6ull-sdk/Linux-4.9.88

obj-m :=input_keys.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o input_keys_app input_keys_app.c

.PHONE:clean 

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -rf modules.order
	rm -f led_key_app
