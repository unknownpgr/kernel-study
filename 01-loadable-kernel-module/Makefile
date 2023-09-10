# https://www.kernel.org/doc/Documentation/kbuild/modules.txt

obj-m := custom-lkm.o
ccflags-y := -std=gnu11
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo rmmod custom-lkm || true
	sudo insmod custom-lkm.ko