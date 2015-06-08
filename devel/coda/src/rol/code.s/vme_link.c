#ifdef VXWORKS

/*  "vme_link.c"  -  Test VME Trigger Interface with NEW Trigger Supervisor --- computer 
			 in TS crate should be running "ts2_link" (TS is in different crate).
		         TS program generates random trigger data --- this program configures 
		         the VME Trigger Interface to be in TS mode and generate VMEbus 
		         interrupts upon receipt of trigger data, and checks the integrity of 
		         this data.							     */

/*  E.J.  9/19/2002  */  /* changes for Power PC processor */ 
    
/*  9/19/2006  -  modify for use together with either version SFI card */

#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>
#include <intLib.h>
#include <cacheLib.h>
#include <iv.h>

#define ACTIVE 0xFFF		/*  all trigger inputs to TS are active  */
#define TI_ADDR 0x0ED0		/*  base address of Trigger Interface  */
#define INTERRUPT_LEVEL 5	/*  Trigger Interface interrupt level  */

struct vme_struct {
		short csr;
		short vector;
		short trig_dat;
		short o_port;
		short i_port;
		};

struct roc_stats {
		long int_count;	
		long class_0;	
		long class_1;	
		long class_2;	
		long class_3;	
		long sync;	
		long sched_sync;	
		long force_sync;
		long error_count;	
		};

volatile struct vme_struct *vme_int; 
struct roc_stats *stats; 

void acknowledge_vme(void);
int ts_vme_events(void);
int ran_vme(void);
void statistics_vme( struct roc_stats *stats );

volatile long roc_data;
volatile long roc_code;
volatile long trig_data;

int error = 0;
unsigned long ti_addr;
int int_level_vme = INTERRUPT_LEVEL;

long memory[4096];

int control;			/*  global process control flag  */
unsigned long int nxt;		/*  for random number generator  */

void vme_link()
{
	char next[18];
	char *gets();
	int id_1;
	long address = 0;

	control = 1;
	nxt = 1;

/*  interface address  */
	printf("\nEnter (16-bit hex) interface address ('0' to quit)  -  ");
	scanf("%lx",&address);
	if( address == 0 )
		exit;
	ti_addr = (unsigned long)address;	

	id_1 = sp(ts_vme_events,0,0,0,0,0,0,0,0,0);		/*  spawn ts event process  */

	printf("\n***  triggers with TS link  ***\n");
	printf("     (type 'q <CR>' to exit, 's <CR>' for statistics)\n\n");

	while( control > 0 )
	{
		gets(next);
		if( *next == 'q' ) 			/*  quit run  */
		{
			control = 0;
			printf("run stopped\n");
		}
		if( *next == 's' )			/*  do statistics  */ 
			control = 2;

	}

	if( error )
		printf("******* error:	intr count = %ld    data: actual = %x  expected = %x\n",
				stats->int_count,roc_code,trig_data);
}

int ts_vme_events(void)
{
	STATUS status;

	long *vme2_6C;
	long temp;
	long temp_1;
	long temp_2;
	long temp_3;
	long old_count;
	int addr;
	long x;
	unsigned long laddr;
	int lock_key;

	error = 0;
	
	stats->int_count = 0;
	stats->class_0 = 0;
	stats->class_1 = 0;
	stats->class_2 = 0;
	stats->class_3 = 0;
	stats->sync = 0;
	stats->sched_sync = 0;
	stats->force_sync = 0;
	stats->error_count = 0;
	
/*  Motorola PPC board VME address of for Trigger Interface (A16/D16 transfers)  */
	sysBusToLocalAdrs(0x29,ti_addr,&laddr);
	printf("local address = %x\n",laddr);  
        vme_int =  (struct vme_struct *)laddr; 

	printf("\n reset interface \n");
	vme_int->csr = 0x80;

/*  read all register locations  */
/*	printf("\n register values \n\n");
	printf("     Control/Status Register = %x\n",vme_int->csr);
	printf("     Interrupt Register      = %x\n",vme_int->vector);
	printf("     Trigger Data Register   = %x\n",vme_int->trig_dat);
	printf("     Output Port Register    = %x\n",vme_int->o_port);
	printf("     Input Port Register     = %x\n",vme_int->i_port);		*/

/*  assign board interrupt vector 0xEE  */
	vme_int->vector = 0xEE;

/*  connect routine to interrupt  */
	status = intDisconnect(INUM_TO_IVEC(0xEE));
	printf("\n connect interrupt service routine \n");
	status = intConnect(INUM_TO_IVEC(0xEE), (FUNCPTR)acknowledge_vme, 0);
	printf("\n intConnect status = %d\n",status);
	
/*  enable VME interrupt  */
	printf("\n enable VME interrupt level %d\n",int_level_vme);
	lock_key = intLock();
	sysIntEnable(int_level_vme);
	printf("\n enable external interrupts into PPC chip \n");
	intEnable(11);		/* enable external interrupts into PowerPC chip */
        intUnlock(lock_key);

/*  reconstruct TS memory data  ---   all trigger patterns that form the memory address were 
     assigned to trigger class 3.  The remaining memory bits were assigned randomly.  */
	memory[0] = 0;
	for( addr = 1; addr <= 4095;  addr++ )		/*  assign data to all memory addresses  */
	{
		do
		{	
			x = ran_vme() & 0x3FFF;		/*  choose random data for memory bits  */
		} while( (x & 0x0F00) == 0 );		/*  require that low 4 bits of roc code are non-zero */
							/*  this makes compatible with SFI versions 1 & 2  */	
		memory[addr] = (((long)(x)) << 8) + 9;
	}

/*  set up board for external trigger mode and enable interrupt generation  */
/*	printf("\n setup TS mode and enable interrupts & triggers \n");		*/
	vme_int->csr = 6;

	old_count = 0;
	
	while( 1 )
	{
		temp = stats->int_count;
		temp_1 = roc_code;
		temp_2 = trig_data;
		temp_3 = stats->error_count;
		if( ( !(temp % 100000) ) && (temp != old_count ) )	   /* print interrupt total every 100000 counts */
		{
			printf("intr count = %ld   data: actual = %x  expected = %x   errors = %ld\n",
				temp,temp_1,temp_2,temp_3);
			old_count = temp;
		}

	/*  check process control flag  */
		if( control == 0 )			/*  stop run and give statistics  */
		{
			temp = stats->int_count;
			temp_1 = roc_code;
			temp_2 = trig_data;
			temp_3 = stats->error_count;
			printf("intr count = %ld   data: actual = %x  expected = %x   errors = %ld\n",
				temp,temp_1,temp_2,temp_3);
			statistics_vme(stats);
			return(error);
		}
		if( control == 2 ) 			/*  give statistics and continue */
		{
			temp = stats->int_count;
			temp_1 = roc_code;
			temp_2 = trig_data;
			temp_3 = stats->error_count;
			printf("intr count = %ld   data: actual = %x  expected = %x   errors = %ld\n",
				temp,temp_1,temp_2,temp_3);
			statistics_vme(stats);
			control = 1;			/*  return to normal running state  */
		}
	}

}

void acknowledge_vme(void)
/* on interrupt, read trigger data and compare with expected data.  Write acknowledge  
   and increment interrupt count */
{
	int trig_hit;
	int value;
	int class;

	sysBusIntAck(int_level_vme); 			/* acknowledge VME interface chip */

	stats->int_count++;
	roc_data = (long)((0x3F) & vme_int->trig_dat);	/* read roc data and mask unused bits including 2 highest bits if event type, need 4 bits */
	roc_code = roc_data >> 2;
	do						/*  regenerate trigger pattern  */
	{						
		trig_hit = ran_vme() & 0xFFF & ACTIVE;	/*  use only active trigger inputs  */
	}  while( trig_hit == 0 );			/*  require non-zero pattern  */

	switch( memory[trig_hit] & 0xF )	/* get class of trigger */
	{
		case  3:			/* class 1 trigger */
		    class = 1;
		    stats->class_1++;
	            break;
		case  5:			/* class 2 trigger */
		    class = 2;
		    stats->class_2++;
		    break;
		case  9:			/* class 3 trigger */
		    class = 3;
		    stats->class_3++;
		    break;
		default:			/* illegal */ 
		    class = 0;
		    stats->class_0++;
		    error = -3;
	}
	
	if( roc_data & 0x1 )			/* test for synchronization flag */
	{			
		stats->sync++;	
		if( roc_code == 0 )	
			stats->force_sync++;
		else
			stats->sched_sync++;	 
	}
	
	trig_data = (( memory[trig_hit] & 0x3F0000 ) >> 16) & 0xf; /* 4 bits of event type only */
	if( trig_data != roc_code )
	{
		stats->error_count++;
	/*	error = 1;
		control = 0;  */    
	}
	
	vme_int->trig_dat = 0x8000;		/* acknowledge trigger */
}

int ran_vme(void)
{
/*  generate random integer in the range  0 - 32k  */
	nxt = nxt * 1103515245 + 12345;
	return (unsigned int)(nxt/65536) % 32768;
}

void statistics_vme( struct roc_stats *stats )
{
	printf("\n.........statistics........\n\n");

	printf("events    = %ld\n",stats->int_count);
	printf("class 0 = %ld     class 1 = %ld     class 2 = %ld     class 3 = %ld\n",
		stats->class_0,stats->class_1,stats->class_2,stats->class_3);
	printf("sync = %ld     sched sync = %ld     forced sync = %ld\n\n",
		stats->sync, stats->sched_sync, stats->force_sync);
	printf("roc data errors = %ld\n\n",stats->error_count); 	

}

	
#else

void
vme_link_dummy()
{
  return;
}

#endif
