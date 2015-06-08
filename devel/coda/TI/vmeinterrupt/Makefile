
KERNEL_SOURCE = /home/davidl/DAG/PPCLinux/kernel/linux-2.6.10-1.ydl.1
INCLUDE = $(KERNEL_SOURCE)/include
MODPOST = $(KERNEL_SOURCE)/scripts/mod/modpost

CFLAGS = -fno-common -DKBUILD_MODNAME=vmeinterrupt
CFLAGS += -D__KERNEL__ -DMODULE

vmeinterrupt.ko: vmeinterrupt.o vmeinterrupt.mod.o
	ld -r vmeinterrupt.o vmeinterrupt.mod.o -o vmeinterrupt.ko

vmeinterrupt.o:vmeinterrupt.c
	gcc -c  -I$(INCLUDE) $(CFLAGS) vmeinterrupt.c

vmeinterrupt.mod.o:vmeinterrupt.o
	$(MODPOST) vmeinterrupt.o
	gcc -c  -I$(INCLUDE) $(CFLAGS) vmeinterrupt.mod.c

clean:
	rm -f vmeinterrupt.o vmeinterrupt.mod.* vmeinterrupt.ko
