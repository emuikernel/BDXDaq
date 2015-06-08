

#include <linux/module.h>  
#include <linux/tty.h>      /* console_print() interface */

#include "../ca91c042/ca91c042.h"
void my_irq_handler(int vmeirq, int vector, void *dev_id, struct pt_regs *regs);

#define MY_IRQ 5

extern wait_queue_head_t vme_int_queue;
#define RING_BUFF_SIZE 10
extern int vme_int_irq_count;
extern int vme_int_vector[RING_BUFF_SIZE];
extern int vme_int_irq[RING_BUFF_SIZE];

//-----------------
// init_module
//-----------------
int init_module()
{
	int err;

	console_print("Loading VME ISR module ...\n");

	// Register our ISR
	err = request_vmeirq(MY_IRQ, my_irq_handler);
	if(err){
		printk("Error inserting ISR (%d)\n", err);
		return -1;
	}
	
	// Enable the interrupt
	enable_vmeirq(MY_IRQ);

	printk("VME ISR installed\n");

	return 0;
}

//-----------------
// cleanup_module
//-----------------
void cleanup_module()
{
	free_vmeirq(MY_IRQ);  
	console_print("VME ISR unloaded.\n");
}

//-----------------
// my_irq_handler
//-----------------
void my_irq_handler(int vmeirq, int vector, void *dev_id, struct pt_regs *regs)
{
	// Here, we just copy some stuff to the vme_int part of the ca91c042
	// driver and wake up any sleeping proceses trying to read from the
	// /dev/vme_int device.
	
	// Do we need a spinlock here?
	vme_int_irq_count++;
	vme_int_vector[vme_int_irq_count%RING_BUFF_SIZE] = vector;
	vme_int_irq[vme_int_irq_count%RING_BUFF_SIZE] = vmeirq;
	
	wake_up_interruptible(&vme_int_queue);
}
