MOD_NAME=jprobe_mod
obj-m := jprobe_mod.o

KDIR := /home/keerthi/rpmbuild/BUILD/kernel-3.10.0-514.10.2.el7/linux-3.10.0-514.10.2.el7.x86_64

PWD := $(shell pwd)

all: jprobe_mod.c
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
