obj-m += vulnwrite.o

KDIR ?= $(HOME)/kernels/linux-5.15.176/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
