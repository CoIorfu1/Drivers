KERNEL_DIR= /home/book/100ask_imx6ull-sdk/Linux-4.9.88

obj-m :=led_qt.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules	

.PHONE:clean 

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -rf modules.order

