obj-m := sun4i-i2s.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo cp sun4i-i2s.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/sunxi
	sudo depmod -a
