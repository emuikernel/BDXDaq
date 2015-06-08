/*  "bang_a16_w.c" -- write location repeatedly, alternating with zero */
/*  E.J.  8/31/10  */

/*  for Motorola PPC processor  */ 

#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>

#define NCYCLES 1000000000
#define PRINT_INTERVAL 1000000

int events(void);

int control = 0;
unsigned long addr_local = 0;
short pattern = 0;

void bang_a16_w()
{
	char next[18];
	char *gets();
	int id_1;
	
	printf("\nEnter address (hex A16) to access - ");
	scanf("%lx",&addr_local);
	printf("\nEnter data pattern (hex 16 bit) to write - ");
	scanf("%x",&pattern);
	
	control = 0;

	id_1 = sp(events,0,0,0,0,0,0,0,0,0);   /*  spawn access process  */
	while( control >= 0 )
	{
		printf("\n");
		printf("     'q <CR>'= exit\n");
		gets(next);
		if( *next == 'q' ) 			/*  quit run  */
		{
			control = -1;
			printf("\n    stopped\n\n");
		}
	}
}

int events(void)
{
	volatile unsigned long *address;
	volatile unsigned long laddr;
	unsigned long count = 0;
	
	sysBusToLocalAdrs(0x29,addr_local,&laddr);
	printf("TDC local address = %x\n",laddr);
	  
        address = (volatile unsigned long *)laddr; 
	
	while( count < NCYCLES )
	{
	    *address = 0x0;		/* write */
	    count++;
	      	
	    /*  check process control flag  */
	    if( control == -1 ) 	/*  stop */
		exit(0);		
	}
			
}

