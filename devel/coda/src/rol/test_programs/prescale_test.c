/*  -------- extensive test of TS prescalers  --------  */

/*  E.J.  4/19/02  */
/* 8/01/06 - modify for ppc processor */

#include <stdio.h>
#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>

#define TS_ADDR  0xED0000	/*  base address of Trigger Supervisor registers */

long trig_in,ps;
int control;


void prescale_test(void)
{	
	int channel,ii;
	unsigned long laddr;
	unsigned long count;
	long trig_in,ps,factor,ps_fact;
	volatile struct TS_struc {
			long csr1;		/* 00 */
			long csr2;		/* 04 */
			long trig;		/* 08 */
			long roc;		/* 0C */
			long sync;		/* 10 */
			long trig_count;	/* 14 */
			long trig_data;		/* 18 */
			long local_roc_data;	/* 1C */
			long prescale[8];	/* 20-3C */
			long timer[5];		/* 40-50 */
			long vector;		/* 54 */
			long roc_buff_status;	/* 58 */ 
			long local_roc_buff_status;	/* 5C */
			long roc_ack_status;	/* 60 */ 
			long user1_data;	/* 64 */
			long user2_data;	/* 68 */
			long state;		/* 6C */
			long test;		/* 70 */
			long blank_1;           /* 74 */
			long scaler_assign;	/* 78 */
			long scaler_control;	/* 7C */
			long scaler[18];	/* 80-C4 */
			long scaler_event;	/* C8 */
			long scaler_live1;	/* CC */
			long scaler_live2;	/* D0 */
			} *ts;

/*  Motorola PPC board VME address of for TS (A24/D32 transfers)  */
	sysBusToLocalAdrs(0x39,TS_ADDR,&laddr);
	printf("TS register local address = %x\n",laddr);  
        ts =  (struct TS_struc *)laddr;
        
/*  initialize board (csr bit 15)  */
	ts->csr1 = 0x8000;

/*  set up test mode (csr bit 8)  */
	ts->csr1 = 0x100;

/*  enable all inputs and open prescales (trig bits 0-12, 15)  */
	ts->trig = 0x9FFF;

	ps = 1;
	trig_in = 64;
	
	for( ii = 0; ii <= 24; ii++)
	{
	    /*  set all prescale factors  */
	    	factor = ps - 1;	
		for( channel = 1; channel <= 8; channel++ )
	    		ts->prescale[channel - 1] = factor;
		ps_fact = ps;

	    /*  test all channels simultaneously  */
		ts->scaler_control = 0xFFFFF;       /* zero all scalers */
		for( count = 1; count <= trig_in; count++ )
		    	ts->test = 1;   		/* pulse inputs */

		printf("\n\n -----  test %ld     triggers = %ld  -----\n", ii,trig_in);
		for( channel = 1; channel <= 12; channel++ )
		{
		    if( channel > 8 ) ps_fact = 1;
		        printf("channel %2d counts = %ld\t (triggers = %ld, prescale factor = %d)\n",
			    channel,ts->scaler[channel-1],count-1,ps_fact);    /* read and print scalers */
		}
		ps = ps * 2;
		trig_in = trig_in * 2;
	}	
	printf("\n----- Prescale Test Done -----\n");

}	
	