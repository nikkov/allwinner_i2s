obj-m := dac_clock.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo cp dac_clock.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/codecs
	sudo depmod -a
