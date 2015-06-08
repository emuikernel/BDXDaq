
#ifdef VXWORKS

/*  "ts2_link.c"  ---- Test new Trigger Supervisor with ROC  --- SFI_V2 type ROC should be running 
		       "sfi_link" (c/sfi_v2), while a VME type ROC should be running "vme_link_ts5"
		       (c/vme-int).  This program sets up the TS in test mode to generate random 
		       level 1 trigger patterns of CLASS 3.  The ROC receives the trigger data, 
		       tests that it is correct, and acknowledges the receipt of the data to the TS.
												     */
/*  E.J.  10/11/2002, 4/12/2001  */
/*  1/28/2003  -  for PPC cpu */

/*  9/19/2006  -  modify for use with either version SFI card */

#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>

/* --------------------------------------------------------------- */
#define ACTIVE 0xFFF		/*  trigger inputs 1-12 are active  */
/* --------------------------------------------------------------- */
#define TOTAL 300000000		/*  number of events  */
#define TS_ADDR  0xED0000	/*  base address of Trigger Supervisor registers */
#define MEM_ADDR 0xED4000	/*  base address of Trigger Supervisor memory  */
	
#define TIME_1     3000		/*  permit clear time (ns) for class 2 & 3 triggers  */
#define TIME_2      500		/*  level 2 accept time (ns) for class 1 triggers  */
#define TIME_3     1000		/*  level 3 accept time (ns) for class 1 & class 2 triggers  */
#define TIME_4     4000		/*  front end busy time (ns)  */
#define TIME_5      700		/*  clear signal assertion time (ns) for triggers that fail  */

#define SYNC  1			/*  enable synchronizations  */
#define SYNC_COUNT  999 	/*  synchronization interval (= count - 1)  */

#define MASK_01 0x0000FFFF      /*  mask for bytes 0,1  */

struct TS_struc {
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
		long scaler[18];	/* 0(80)..4(90)..8(A0)..12(B0)..16(C0)..17(C4) */
		long scaler_event;	/* C8 */
		long scaler_live1;	/* CC */
		long scaler_live2;	/* D0 */
		};

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

struct TS_stats {
		unsigned long triggers;	
		unsigned long trig[12];	
		unsigned long events;	
		unsigned long roc_events;	
		unsigned long class_0;	
		unsigned long class_1;	
		unsigned long class_2;	
		unsigned long class_3;	
		unsigned long pass1_1;	
		unsigned long pass2_2;	
		unsigned long fail2_2;	
		unsigned long pass3_3;	
		unsigned long fail3_3;	
		unsigned long fail2_3;	
		};

	int control;			/*  global process control flag  */
	unsigned long int nxt;		/*  for random number generator  */
	long roc_list;			/*  for ROC enable list  */
	int buffer_mode;		/*  buffering mode  */
	int fast_mode;			/*  for L1 mode  */ 

int ts_events(void);
int setup( long *memory, volatile long *mem, volatile struct TS_struc *ts );
int trig_p3( int trig_hit, volatile struct TS_struc *ts );
void statistics( struct TS_stats *stats, volatile struct TS_struc *ts );
int ran(void);
void print_bit( struct b_field *bit );
void ts_status(volatile struct TS_struc *ts );

void ts2_link()
{
	char next[18];
	char *gets();
	int id_1;

/*  L1 Accept mode (fast = drive all L1 Accepts, normal = drive pattern)  */
	fast_mode = 0;
	printf("\nEnable FAST L1 mode? (1 = yes, 0 = no) -  ");
	scanf("%d",&fast_mode);
	
/*  ROC buffer mode  */
	buffer_mode = 0;
	printf("\nEnter ROC buffer mode (1 = buffer, 0 = lock)  - ");
	scanf("%d",&buffer_mode);

/*  list of enabled ROCs  */
	roc_list = 0;
	printf("\nEnter (hex) ROC enable pattern ('0' to quit)  -  ");
	scanf("%lx",&roc_list);
	if( roc_list == 0 )
		exit;

	control = 0;
	nxt = 1;

	id_1 = sp(ts_events,0,0,0,0,0,0,0,0,0);   /*  spawn event generate process  */

	printf("\n***  continuous event  ---  TS-ROC link  ***\n\n");

	while( control >= 0 )
	{
		gets(next);
		printf("     'g <CR>' = go,            'p <CR>' = pause,           'q <CR>' = exit\n");
		printf("     'c <CR>' = pause on sync, 's <CR>' = stats,           'r <CR>' = register values\n");
		if( *next == 'g' ) 			/*  start run  */
		{
			control = 1;
			printf("\n    run started\n\n");
		}
		if( *next == 'p' ) 			/*  pause run  */
		{
			control = 2;
			printf("\n    run paused\n\n");
		}
		if( *next == 'q' ) 			/*  quit run  */
		{
			control = -1;
			printf("\n    run stopped\n\n");
		}
		if( *next == 's' )			/*  do statistics  */ 
			control = 3;
		if( *next == 'r' )			/*  read registers  */ 
			control = 4;
		if( *next == 'c' )			/*  pause on next sync  */ 
		{
			control = 7;
			printf("\n    pause on next sync\n\n");
		}

	}
}

int ts_events(void)
{
	volatile struct TS_struc *ts;
	struct TS_stats *stats;

	volatile long *mem;
	long memory[4096];
	
	unsigned long laddr;

	int trig_hit = 0;
	int hits;

	int error = 0;
	long value;
	int test = 0;
	int ii;
	struct b_field *bit;
	bit = (struct b_field *)&value;

/*  Motorola PPC board VME address of for TS (A24/D32 transfers)  */
	sysBusToLocalAdrs(0x39,TS_ADDR,&laddr);
	printf("TS register local address = %x\n",laddr);  
        ts =  (struct TS_struc *)laddr;
        
	sysBusToLocalAdrs(0x39,MEM_ADDR,&laddr);
	printf("TS memory local address = %x\n",laddr);  
	mem = (volatile long *)laddr;

	stats = (struct TS_stats *)malloc( sizeof(struct TS_stats) );

	stats->triggers = 0;		/*  initialize statistics  */	
	for(ii = 0; ii < 12; ii++)
		stats->trig[ii] = 0;	
	stats->events = 0;	
	stats->roc_events = 0;	
	stats->class_0 = 0;	
	stats->class_1 = 0;	
	stats->class_2 = 0;	
	stats->class_3 = 0;	
	stats->pass1_1 = 0;	
	stats->pass2_2 = 0;	
	stats->fail2_2 = 0;	
	stats->pass3_3 = 0;	
	stats->fail3_3 = 0;	
	stats->fail2_3 = 0;	
	 
	error = setup(memory,mem,ts);
	if( error ) exit(error);

	while( 1 )
	{
		value = ts->state;
		if( bit->b_15 != 0 ) 		/*  check if TS is ready  */
		{
			do						/*  generate trigger pattern (12-bit random number)  */
			{	
				trig_hit = ran() & 0xFFF & ACTIVE;	/*  use only active trigger inputs  */
			}  while( trig_hit == 0 );			/*  require non-zero pattern  */

			stats->triggers++;

			hits = trig_hit;
			for( ii=0; ii< 12; ii++ )			/*  count triggers on each input  */
			{
				if( hits & 0x0001 )
					stats->trig[ii]++;
				hits >>= 1;
			}

			error = trig_p3(trig_hit,ts);
			stats->class_3++;
			stats->pass3_3++;
			stats->events++;
			if( (!(stats->events % 100000)) && stats->events )     /* print every 100000 counts */
				printf("     events = %ld\n",stats->events);

			if( error ) 
			{
				printf("error found (%d) ---  \n",error);
				ts_status(ts);
				statistics(stats,ts);
				control = -1;
				exit(error);
			}
		}

	   /*  check process control flag  */
		if( control == -1 ) 		/*  quit run  */
		{
			ts->csr1 = 0x10000;	/*  STOP   (reset csr1 bit 0)  */
			statistics(stats,ts);	/*  give statistics  */
			exit(error);
		}
		if( control == 1 )		/*  start run  */
		{
			ts->csr1 = 1;		/*  GO   (set csr1 bit 0)  */
			control = 0;
		}
		if( control == 2 ) 		/*  pause run  */
		{
			ts->csr1 = 0x10000;	/*  STOP   (reset csr1 bit 0)  */
			statistics(stats,ts);	/*  give statistics  */
			control = 0;
		}
		if( control == 3 ) 		/*  give statistics */
		{
			statistics(stats,ts);	
		/*	value = ts->state;	*/	/*  read & print state register  */
		/*	printf("\n    state = ");
			print_bit(bit);		*/
			control = 0;		
		}
		if( control == 4 )		/*  read registers  */
		{
			ts_status(ts);
			control = 0;
		}	
		if( control == 7 )		/*  pause on next sync  */
		{
			ts->csr1 = 2;		/*     set csr1 bit 1  */
			control = 0;
		}
	}

}

int setup( long *memory, volatile long *mem, volatile struct TS_struc *ts )
{
	long value;
	long mem_value;
	long x;
	long ii;
	int addr;
	int error;
	long t_max;
	long t1, t2, t3, t4, t5;
	struct b_field *bit;
	bit = (struct b_field *)&value;

	error = 0;

/*  setup board  */
	ts->csr1 = 0x8000;		/*  initialize board (csr bit 15)  */
	printf("\n\n*****  INITIALIZE BOARD  *****\n\n");

	if( SYNC == 1 )
		ts->csr2 = 1;		/*  enable scheduled synchronizations (csr2 bit 0)  */
	if( buffer_mode == 0 )
	{
	    value = 0xFFFF & ts->csr2;
	    ts->csr2 = value | 0x03E0;	/*  ROC lock all branches (csr2 bits 5-9)  */
	}
	if( fast_mode )
	{
	    value = 0xFFFF & ts->csr2;
	    ts->csr2 = value | 0x8000;	/*  enable fast L1 mode  */
	}
	ts->trig = 0x1FFF;		/*  enable all triggers in non-strobe mode  */
	ts->roc = roc_list;		/*  enable ROCs  */
	ts->sync = SYNC_COUNT;		/*  synchronization count  */
	ts->test = 0;
	ts->csr1 = 0x100;		/*  enable test mode (csr1 bit 8)  */
	ts->scaler_assign = 0xC43210;	/*  assign 6 scaler channels  */
	ts->scaler_control = 0xFFFFF;	/*  reset all scalers  */

/*  setup level 2 accept and level 3 accept timers  */
	t_max = TIME_3;
	if( TIME_2 > TIME_3 )
	{
		t_max = TIME_2;
		printf("*****  NOTE - level 2 accept time (%d) > level 3 accept time (%d)\n",TIME_2,TIME_3);
	}
	t2 = (long)(TIME_2/40);		/*  level 2 & 3 accept times in counts  */
	t3 = (long)(TIME_3/40);
	if( (t2 > 65535) || (t3 > 65535) )	/*  restrict times to 16 bits  */
	{
		error = -1;
		printf("*****  timer overflow error  *****  t2 = %d   t3 = %d\n",t2,t3);
		return error;
	}
	ts->timer[1] = t2;		/*  set level 2 and level 3 timers  */
	ts->timer[2] = t3;

/*  setup front busy timer  */
	t4 = (long)(TIME_4/40);		/*  front busy time in counts  */
	if( t4 > 65535 )		/*  restrict times to 16 bits  */
	{
		error = -1;
		printf("*****  front busy timer overflow error  *****  t4 = %d\n",t4);
		return error;
	}
	ts->timer[3] = t4;		/*  set front busy timer  */
	value = 0xFFFF & ts->csr2;
	ts->csr2 = value | 0x4;		/*  enable front busy timer (csr2 bit 2)  */
	value = 0xFFFF & ts->csr2;
	ts->csr2 = value | 0x10;	/*  enable front busy input (csr2 bit 4)  */
	ts->csr1 = 0x080;		/*  override inhibit input (csr1 bit 7)  */

/*  setup clear hold timer  */
	t5 = (long)(TIME_5/20);		/*  clear time in counts  */
	if( t5 > 255 )			/*  restrict times to 8 bits  */
	{
		error = -1;
		printf("*****  clear timer overflow error  *****  t5 = %d\n",t5);
		return error;
	}
	ts->timer[4] = t5;		/*  set clear hold timer  */
	value = 0xFFFF & ts->csr2;
	ts->csr2 = value | 0x8;		/*  enable clear hold timer (csr2 bit 3)  */

/*  setup permit clear timer  */
	t1 = (long)(TIME_1/40);		/*  permit clear time in counts  */
	if( t1 > 65535 )		/*  restrict times to 16 bits  */
	{
		error = -1;
		printf("*****  permit clear timer overflow error  *****  t1 = %d\n",t1);
		return error;
	}
	ts->timer[0] = t1;		/*  set permit clear timer  */
	value = 0xFFFF & ts->csr2;
/*	ts->csr2 = value | 0x2;	  */	/*  enable permit clear timer (csr2 bit 1)  */

/*  construct memory data  ---  in the following model, all trigger patterns that form the memory 
     address are assigned to trigger class 3.  The remaining memory bits are assigned randomly.  */
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

/*  load and test memory  */
	for( addr = 0; addr <= 4095;  addr++ )		/* load entire memory */
		mem[addr] = memory[addr];
	for( addr = 0; addr <= 4095;  addr++ )		/* read & test memory */
	{
		mem_value = mem[addr];
		if( memory[addr] != ( 0xFFFFFF & mem_value ) )
		{
			error = -1;  
			printf("***  memory error  ***  addr = %x   data = %x   value = %x\n",
					addr,memory[addr],mem_value);
			return error;   
		}
	}

/*	value = ts->state;	*/	/*  read & print state register  */
/*	printf("\nsetup complete:    state = ");
	print_bit(bit);		*/

	return error;
}

int trig_p3( int trig_hit,  volatile struct TS_struc *ts )
{
	volatile long value;
	int error;
	struct b_field *bit;
	bit = (struct b_field *)&value;

	error = 0;

	value = ts->test;
	value = ts->state;
	if( bit->b_15 == 0 ) 	/*  check if TS is ready  */
	{
		error = 1;
		printf("error %d   state = %x\n",error,value);
		return error;
	} 
 	
	ts->trig = (long)(trig_hit * 2);	/*  set trigger pattern  */
 	ts->test = 0x0001;		/*  pulse trigger  */
	value = ts->test;
	value = ts->state;
/*	printf("lev 1 trigger:     state = ");
	print_bit(bit);				*/
	if( (value & MASK_01) != 0x7003 )	
	{	
		error = 2;
		printf("error %d   state = %x   trig = %x\n",error,value,trig_hit);
		return error;
	}
	ts->test = 0x0100;		/*  set front end busy  */
	value = ts->test;
	value = ts->state;
/*	printf("FE busy:           state = ");
	print_bit(bit);			*/
	if( (value & MASK_01) != 0x7403 )
	{	
		error = 3;
		printf("error %d   state = %x\n",error,value);
		return error;
	}
	ts->test = 0x0102;		/*  set level 2 pass  */
	value = ts->test;
	value = ts->state;
/*	printf("lev 2 pass:        state = ");
	print_bit(bit);			*/
	if( (value & MASK_01) != 0x7437 )
	{	
		error = 4;
		printf("error %d   state = %x\n",error,value);
		return error;
	}
	ts->test = 0x0104;		/*  set level 3 pass  */
	value = ts->test;
	value = ts->state;
/*	printf("lev 3 pass:        state = ");
	print_bit(bit);			*/
	if( (value & MASK_01) != 0x7577 )
	{	
		error = 5;
		printf("error %d   state = %x\n",error,value);
		return error;
	}
	ts->test = 0x0000;		/*  clear front end busy  */
	value = ts->test;
	value = ts->state;
/*	printf("FE not busy:       state = ");
	print_bit(bit);			*/
/*	if( (value & MASK_01) != 0x8000 )
	{	
		error = 6;
		printf("error %d   state = %x\n",error,value);
		return error;
	}	*/
	return error;
}

void statistics( struct TS_stats *stats, volatile struct TS_struc *ts )
{
	long value, value_1, value_2;
	long fail, fail_2;
	int buff;
	unsigned long trig;
	long p;
	long f;
	long f2;
	long f3;
	int ii;
	struct b_field *bit;
	bit = (struct b_field *)&value;

	printf("\n.........statistics........\n\n");

	buff = stats->events - stats->roc_events;
	fail_2 = stats->fail2_2 + stats->fail2_3;
	fail = fail_2 + stats->fail3_3;

	trig = (stats->triggers);
	p  = (100 * stats->events)/trig;		/*  % fractions  */
	f  = (100 * fail)/trig;
	f2 = (100 * fail_2)/trig;
	f3 = (100 * stats->fail3_3)/trig;

	printf("triggers    = %ld\n",stats->triggers);
	printf("events      = %ld     roc events = %ld     buffer = %d\n",
		stats->events,stats->roc_events,buff);
	printf("fail        = %ld     (fail 2 = %ld     fail 3 = %ld)\n",fail,fail_2,stats->fail3_3);
	printf("fast resets = %ld \n",stats->class_0);
	printf("pass = %ld     fail = %ld     (fail 2 = %ld   fail 3 = %ld)\n",p,f,f2,f3);
	printf("class 1 = %ld     class 2 = %ld     class 3 = %ld\n\n",
		stats->class_1,stats->class_2,stats->class_3);
	for( ii=0; ii < 12; ii++)
		printf("trigger input %2d = %ld\n",ii+1,stats->trig[ii]);

	for(ii = 1; ii <= 18; ii++) 	
		printf("scaler %2d  = %ld\n",ii,ts->scaler[ii-1]);
	printf("\nscaler_event = %ld\n",ts->scaler_event);
	printf("scaler_live1 = %ld\n",ts->scaler_live1);
	printf("scaler_live2 = %ld\n",ts->scaler_live2);
	value = ts->state;		/*  read & print state register  */
	printf("\n  state (%lx) = ",value);
	print_bit(bit);		
	value_1 = ts->roc_buff_status;
	value_2 = ts->local_roc_buff_status;
	printf("\n  roc status = %lx   local roc status = %lx\n",value_1,value_2);			

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

void ts_status(volatile struct TS_struc *ts )
{
/* print register values */
	int ii; 
		
	printf("\ncsr1 = %lx\n",ts->csr1);
	printf("csr2 = %lx\n",ts->csr2);
	printf("trig = %lx\n",ts->trig);
	printf("roc = %lx\n",ts->roc);
	printf("sync = %ld\n",0xFFFF & ts->sync);
	printf("trig_count = %ld\n",0xFFFF & ts->trig_count);
	printf("trig_data = %lx\n",ts->trig_data);
	printf("local_roc_data = %lx\n\n",ts->local_roc_data);
	printf("prescale[0] = %ld\t\t",0xFFFFFF & ts->prescale[0]);
	printf("prescale[1] = %ld\n",0xFFFFFF & ts->prescale[1]);
	printf("prescale[2] = %ld\t\t",0xFFFFFF & ts->prescale[2]);
	printf("prescale[3] = %ld\n",0xFFFFFF & ts->prescale[3]);
	printf("prescale[4] = %ld\t\t",0xFFFF & ts->prescale[4]);
	printf("prescale[5] = %ld\n",0xFFFF & ts->prescale[5]);
	printf("prescale[6] = %ld\t\t",0xFFFF & ts->prescale[6]);
	printf("prescale[7] = %ld\n\n",0xFFFF & ts->prescale[7]);
	printf("timer[0] = %ld\t\t",0xFFFF & ts->timer[0]);
	printf("timer[1] = %ld\n",0xFFFF & ts->timer[1]);
	printf("timer[2] = %ld\t\t",0xFFFF & ts->timer[2]);
	printf("timer[3] = %ld\n",0xFFFF & ts->timer[3]);
	printf("timer[4] = %ld\n\n",0xFF & ts->timer[4]);
	printf("vector = %lx\n",ts->vector);
	printf("roc_buff_status = %lx\n",ts->roc_buff_status);
	printf("local_roc_buff_status = %lx\n",ts->local_roc_buff_status);
	printf("roc_ack_status = %lx\n",ts->roc_ack_status);
	printf("user1_data = %lx\n",ts->user1_data);
	printf("user2_data = %lx\n",ts->user2_data);
	printf("state = %lx\n",ts->state);
	printf("test = %lx\n\n",ts->test);
	printf("scaler_assign = %lx\n",ts->scaler_assign);
	printf("scaler_control = %lx\n",ts->scaler_control);
/*	for(ii = 1; ii < 19; ii++)
		printf("scaler[%d] = %ld\n",ii,ts->scaler[ii-1]);
	printf("scaler_event = %ld\n",ts->scaler_event);
	printf("scaler_live1 = %ld\n",ts->scaler_live1);
	printf("scaler_live2 = %ld\n",ts->scaler_live2); */

}

#else

void
ts2_link_dummy()
{
  return;
}

#endif
