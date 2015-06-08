/*  "sfi_link.c"  ------  Test SFI Trigger Interface with Trigger Supervisor --- computer in TS crate
                          should be running "ts_link" (TS is in different crate).
		          TS program generates random trigger data --- this program configures the SFI
		          Trigger Interface to be in TS mode and generate interrupts upon receipt
		          of trigger data, and checks the integrity of this data.		*/
		          
/*    NOTE:  This version for PowerPC board as SFI controller     */
/*	     ------------------------------------------------     */

/*  E.J.  11/27/96, 9/22/97 */
/*  E.J.   3/98  changes to work with Rev 6 Radstone CPU */
/*  E.J.   7/99  changes to work with Motorola PowerPC CPU (needs EVEN interrupt ID) */
/*  E.J.   3/03  print error on data mismatch (not just stop) */

/*  9/19/2006  -  for TS2 - modify for use together with either version SFI card */
/*  9/24/2007  -  auto-detect SFI interface version */


#include <stdio.h>
#include <intLib.h>
#include <cacheLib.h>
#include <iv.h>

#define RETRY 10		/* # attempts to read valid AUX data per interrupt.
				   A value of at least 1 is required.  A value greater 
				   than 1 protects against the SFI occasionally returning 
				   the invalid data pattern 0xFFFFFFFF.  This appears to
				   be a problem only with PowerPC hosts. */

#define ACTIVE 0xFFF		/*  all trigger inputs to TS are active  */
#define SFI_ADDR 0xE00000	/*  base address of SFI  */
#define INTERRUPT_LEVEL 3	/*  SFI interrupt level  */

struct b_field {
		unsigned b_31: 1;       unsigned b_30: 1;
		unsigned b_29: 1;       unsigned b_28: 1;
		unsigned b_27: 1;       unsigned b_26: 1;
		unsigned b_25: 1;       unsigned b_24: 1;
		unsigned b_23: 1;       unsigned b_22: 1;
		unsigned b_21: 1;       unsigned b_20: 1;
		unsigned b_19: 1;       unsigned b_18: 1;
		unsigned b_17: 1;       unsigned b_16: 1;
		unsigned b_15: 1;       unsigned b_14: 1;
		unsigned b_13: 1;       unsigned b_12: 1;
		unsigned b_11: 1;       unsigned b_10: 1;
		unsigned b_9: 1;        unsigned b_8: 1;
		unsigned b_7: 1;        unsigned b_6: 1;
		unsigned b_5: 1;        unsigned b_4: 1;
		unsigned b_3: 1;        unsigned b_2: 1;
		unsigned b_1: 1;        unsigned b_0: 1;
		};

volatile long *seq_disable;
volatile long *clr_vme_out;
volatile long *int_aux;
volatile long *aux_wrt;
volatile long *vme_out;
volatile long *vme_in;
volatile long *ff_input;
volatile long *int_id_w;
volatile long *int_id_r;
volatile long *int_mask_w;
volatile long *int_mask_r;
volatile long *reset_lca2_w;

volatile long int_count = 0;
volatile long trig_data = 0;
volatile long roc_data = 0;
volatile long roc_code = 0;
volatile long old_trig_data = -1;
volatile long next_roc_data = -1;
volatile long error_count = 0;
volatile long temp_0 = 0;
volatile long sfi_version;
volatile long sync = 0;
volatile long force_sync = 0;
volatile long sched_sync = 0;

int ts_events(void);
void write_aux(long data);
volatile unsigned long read_aux(void);
void acknowledge(void);
int ran(void);
void print_bit(struct b_field *bit);

int int_level = INTERRUPT_LEVEL;

long memory[4096];

int error = 0;
int control;			/*  global process control flag  */
unsigned long int nxt;		/*  for random number generator  */

void sfi_link()
{
	volatile long key_value;
	char next[18];
	char *gets();
	int id_1;

	control = 1;
	nxt = 1;

	printf("\nAuto-detect SFI interface card version (1 or 2)\n");
		
	id_1 = sp(ts_events,0,0,0,0,0,0,0,0,0);		/*  spawn ts event process  */

	printf("\n***  triggers with TS link (interrupt mode)  ***\n");
	printf("     (type 'q <CR>' to exit, 's <CR>' for statistics)\n\n");

	while( control > 0 )
	{
		gets(next);
		if( *next == 'q' ) 			/*  quit run  */
		{
			control = 0;
			printf("run stopped - error count = %ld\n",error_count);
		}
		if( *next == 's' )			/*  do statistics  */ 
			control = 2;

	}
	
}

int ts_events(void)
{
	STATUS status;

	volatile long temp_1;
	volatile long temp_2;
	volatile long temp_3;
	volatile long temp_4;
	volatile long temp_5;
	volatile long data = 0;
	volatile long value;
	volatile long version_id;
	volatile long temp;
	volatile long old_count;
	int addr;
	long x;
	unsigned long laddr;

	struct b_field *bit;
	bit = (struct b_field *)&value;
	
	sysBusToLocalAdrs(0x39,SFI_ADDR,&laddr);
	printf("local address = %x\n",laddr);  
	
	seq_disable =  (long *)(laddr + 0x2024);
	clr_vme_out =  (long *)(laddr + 0x1104);
	int_aux =      (long *)(laddr + 0x1110);
	aux_wrt =      (long *)(laddr + 0x1114);
	vme_out =      (long *)(laddr + 0x1100);
	vme_in =       (long *)(laddr + 0x1000);
	ff_input =     (long *)(laddr + 0x200C);
	int_id_w =     (long *)(laddr + 0x2110);
	int_id_r =     (long *)(laddr + 0x2010);
	int_mask_w =   (long *)(laddr + 0x2114);
	int_mask_r =   (long *)(laddr + 0x2014);
	reset_lca2_w = (long *)(laddr + 0x211C);
	
	printf("seq_disable = %x\n",seq_disable);
	printf("clr_vme_out = %x\n",clr_vme_out);
	printf("int_aux = %x\n",int_aux);
	printf("aux_wrt = %x\n",aux_wrt);
	printf("vme_out = %x\n",vme_out);
	printf("vme_in = %x\n",vme_in);
	printf("ff_input = %x\n",ff_input);
	printf("int_id_w = %x\n",int_id_w);
	printf("int_id_r = %x\n",int_id_r);
	printf("int_mask_w = %x\n",int_mask_w);
	printf("int_mask_r = %x\n",int_mask_r);
	printf("reset_lca2_w = %x\n",reset_lca2_w);
	
	
/*  reset AUX interface  */
	printf("\n reset AUX interface \n");
	write_aux(0x8000);
	printf("\n reset register group LCA2 \n");
	*reset_lca2_w = 1;

/*  reconstruct TS memory data  ---   all trigger patterns that form the memory address were 
     assigned to trigger class 3.  The remaining memory bits were assigned randomly.  */
	memory[0] = 0;
	for( addr = 1; addr <= 4095;  addr++ )		/*  assign data to all memory addresses  */
	{
		do
		{	
			x = ran() & 0x3FFF;		/*  choose random data for memory bits  */
		} while( (x & 0x0F00) == 0 );		/*  require that low 4 bits of roc code are non-zero */
							/*  this makes compatible with SFI versions 1 & 2  */	
		memory[addr] = (((long)(x)) << 8) + 9;
	}
	
/*	for( addr = 0; addr <= 30;  addr++ )
		printf("memory[%x] = %x\n",addr,memory[addr]);	*/	

/*  enable VME interrupt  */
	printf("\n enable VME interrupt level %d\n",int_level);
	sysIntEnable(int_level);
	
	intEnable(11);		/* enable external interrupts into PowerPC chip */
	
/*  enable AUX generated interrupts */
	printf("\n enable AUX generated interrupts \n");
	*int_mask_w = 0x10;
	cacheInvalidate(DATA_CACHE,int_mask_r,4);		/* for PowerPC */ 
	data = *int_mask_r;
	printf(" int_mask = %lx\n",data);

/*  set interrupt level, & assign interrupt ID = 0xEE  */
	printf("\n set interrupt level, & assign interrupt ID = 0xEE \n");
	*int_id_w = 0x8EE + (int_level << 8);
	cacheInvalidate(DATA_CACHE,int_id_r,4);		/* for PowerPC */ 
	temp_1 = *int_id_r;
	printf(" int_id = %lx\n",temp_1);

/*  connect routine to interrupt  */
	status = intDisconnect(INUM_TO_IVEC(0xEE));
	status = intConnect(INUM_TO_IVEC(0xEE), (FUNCPTR)acknowledge, 0);

/*  setup AUX board for TS triggers  */
	printf("\n setup for TS triggers\n");
	write_aux(0x0);
	data = read_aux();
	printf("*** AUX port = %lx\n",data);
	value = data;
	print_bit(bit);
	version_id = (data >> 13) & 3;
	if( version_id == 3 )
	    sfi_version = 1;
	else if( version_id == 2 )
	    sfi_version = 2;
	else
	    sfi_version = 0;    
	printf("\n***** Using version %d of SFI interface card *****\n\n", sfi_version);

	        
	old_count = 0;

	while( 1 )
	{
		temp = int_count;		/* print trigger total every 100000 counts */
		temp_1 = roc_code;
		temp_2 = error_count;
		temp_3 = trig_data;
		if( ( !(temp % 100000) ) && (temp != old_count) )
		{	
			printf("interrupts = %ld   data:  actual = %lx   expected = %lx   errors = %ld\n",
				temp,temp_1,temp_3,temp_2);
			old_count = temp;
		}

		if( error )
		{
	 		printf("**** error: count = %ld trig data = %x roc data = %x\n",
				int_count,trig_data,roc_data);
	 		printf("     last data = %x next data = %x\n",
				old_trig_data,next_roc_data);
			control = 0;
			return(error);
		}
			
	/*  check process control flag  */
		if( control == 0 )		/*  stop run and give statistics */
		{
			temp = int_count;
			temp_1 = roc_code;
			temp_2 = error_count;
			temp_3 = trig_data;
			printf("interrupts = %ld   data:  actual = %lx   expected = %lx   errors = %ld\n",
				temp,temp_1,temp_3,temp_2);
			printf("sync = %ld   scheduled sync = %ld   forced sync = %ld\n",
				sync,sched_sync,force_sync);
			return(error);	
		}
		if( control == 2 )		/* give statistics and continue */
		{
			temp = int_count;
			temp_1 = roc_code;
			temp_2 = error_count;
			temp_3 = trig_data;
			printf("interrupts = %ld   data:  actual = %lx   expected = %lx   errors = %ld\n",
				temp,temp_1,temp_3,temp_2);
			printf("sync = %ld   scheduled sync = %ld   forced sync = %ld\n",
				sync,sched_sync,force_sync);
			control = 1;		/* return to normal running state */
		}

	}

}

void write_aux(long data)		/* write to SFI AUX port */
{
	int lock_key;
	int lock_key_2;

	*seq_disable = 1;		/* disable FB sequencer */
	lock_key = intLock();			/* lock out interrupts  --------------------  */
	*int_aux = data;		/* write data to internal AUX port register */
	*vme_out = 0x4000;		/* enable data path to AUX port (A45) */
	*aux_wrt = 1;			/* write pulse to AUX port (B40) */
	*vme_out = 0x40000000;		/* disable data path to AUX port */
	lock_key_2 = intUnlock(lock_key);	/* unlock interrupts  --------------------  */
	return;
}

volatile unsigned long read_aux(void)			/* read from SFI AUX port */
{
/* up to RETRY data reads are attempted until valid data (i.e. != 0xFFFFFFFF) is obtained */
	volatile unsigned long data;
	int jj;
	
	jj = 1;				/* retry count */ 
	data = 0xFFFFFFFF;		/* initialize to unacceptable pattern */
	*seq_disable = 1;		/* disable FB sequencer */
	*vme_out = 0x2000;		/* enable data path from AUX port (A28) */
	while( (data == 0xFFFFFFFF) && (jj <= RETRY) )
	{   
		data = *vme_in;		/* read data from AUX port */
		jj++;
	}
	*vme_out = 0x20000000;		/* disable data path from AUX port */
	return data;
}

void acknowledge(void)
/*  on interrupt, read & test data, write acknowledge and increment count  */
{
	int trig_hit;
	volatile int loop_count;
	volatile int test;

	int_count++;
	error = 0;
	
	*int_mask_w = 0x1000;			/* clear interrupt flag */
	sysBusIntAck(int_level); 		/* acknowledge VME interface chip */

	if( sfi_version == 1)
	    roc_data = (0x3F) & read_aux();	/* read trigger data and mask unused bits */
	else    
	    roc_data = (0xFF) & read_aux();	/* read trigger data and mask unused bits */
	    
	roc_code = roc_data >> 2;

	if( roc_data & 0x1 )			/* test for synchronization flag */
	{			
		sync++;	
		if( roc_code == 0 )	
		    force_sync++;
		else
		    sched_sync++;	 
	}
	
	do						/*  regenerate trigger pattern  */
	{						
		trig_hit = ran() & 0xFFF & ACTIVE;	/*  use only active trigger inputs  */
	}  while( trig_hit == 0 );			/*  require non-zero pattern  */

	if( sfi_version == 1)
	    trig_data = ( memory[trig_hit] & 0x0F0000 ) >> 16;
	else    
	    trig_data = ( memory[trig_hit] & 0x3F0000 ) >> 16;

	if( trig_data != roc_code )
	{
	/*	error = 1;  	*/
		error_count++;
	}	
	/*	sysBusIntAck(int_level);  */		/* acknowledge VME interface chip */
	*vme_out = 0x1000;				/* acknowledge trigger */
	*int_mask_w = 0x10;				/* re-enable interrupt source */
	*vme_out = 0x10000000;				/* clear acknowledge */
}

int ran(void)
{
/*  generate random integer in the range  0 - 32k  */
	nxt = nxt * 1103515245 + 12345;
	return (unsigned int)(nxt/65536) % 32768;
}

void print_bit( struct b_field *bit )
{
	printf("%d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d  %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d\n",
		bit->b_31,bit->b_30,bit->b_29,bit->b_28,bit->b_27,bit->b_26,bit->b_25,bit->b_24,
		bit->b_23,bit->b_22,bit->b_21,bit->b_20,bit->b_19,bit->b_18,bit->b_17,bit->b_16,
		bit->b_15,bit->b_14,bit->b_13,bit->b_12,bit->b_11,bit->b_10,bit->b_9,bit->b_8,
		bit->b_7,bit->b_6,bit->b_5,bit->b_4,bit->b_3,bit->b_2,bit->b_1,bit->b_0 );
}




