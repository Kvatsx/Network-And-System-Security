obj-m := netfilter.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	insmod ./netfilter.ko

clean:
	rmmod netfilter
	rm netfilter.ko
	