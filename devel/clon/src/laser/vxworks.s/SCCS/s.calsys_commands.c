h56099
s 00001/00001/01548
d D 1.10 05/02/25 14:42:48 boiarino 11 10
c *** empty log message ***
e
s 00033/00001/01516
d D 1.9 03/04/25 15:49:09 giovanet 10 9
c still a prob 
c 
e
s 00044/00017/01473
d D 1.8 03/04/25 12:26:53 giovanet 9 8
c still not reading ?
c 
e
s 00053/00023/01437
d D 1.7 03/04/25 09:17:44 giovanet 8 7
c updated for serial port still testing
e
s 00001/00001/01459
d D 1.6 03/04/23 12:03:35 giovanet 7 6
c changed the serial driver software
c not completed yet
c 
e
s 00185/00006/01275
d D 1.5 03/04/23 10:01:08 boiarino 6 5
c under development by Kevin Giovanetti, check it in to make release
e
s 00023/00002/01258
d D 1.4 03/03/03 14:01:27 boiarino 5 4
c first version for 2306 CPU
e
s 00003/00002/01257
d D 1.3 03/02/14 13:02:39 boiarino 4 3
c #include "laser.h"
e
s 00893/00878/00366
d D 1.2 03/02/14 11:21:39 boiarino 3 1
c some cleanup
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/calsys_commands.c
e
s 01244/00000/00000
d D 1.1 00/08/03 10:53:13 gurjyan 1 0
c date and time created 00/08/03 10:53:13 by gurjyan
e
u
U
f e 0
t
T
I 1
D 3
/*   
E 3
I 3
/*
I 9

E 9
  calsys_commands.c - code for the CLAS Calorimeter Calibration System
D 11
                      [to be run on CAMAC3]
E 11
I 11
                      to be run on camac1 (br=0, if branch is essential)
E 11
E 3

I 8
   changed spring 2003 to incorporate vmivme 6016
        VMEbus Intelligent 16-Channel Asynchronous Serial Controller

E 8
D 3
	move to new directory giovanet/EC
     calsys_commands.c
 for problems with the crate you may need to delete the watrchdog
 
    i   ;shows the tasks 
    td t2  ; nsme of the watch dog is then determined to be t2
    
         C-code for the CLAS Calorimeter Calibration System
     [to be run on CAMAC3]

     newest working version [17Sept1998]
     [older versions kept in ~clasrun/jmu/old]

 *   camac register
     each command sets and/or resets bits in the output register 
E 3
I 3
 * camac register
      each command sets and/or resets bits in the output register 
E 3
 	gason              one bit on/off
 	gasoff
 	hp_pulseron           NA if I use the  CAMAC pulser
 	hp_pulseroff          NA
 	laser_enable            one bit for enable pulse    
 	laser_disable           one bit for disable pulse  
 	lpoweron           (power)
 	lpoweroff          (power)
 	stepperon          (power)
 	stepperoff         (power)
 	
D 3
 *  rs232 port to the stepper motors
    sends and receives ascii strings through the rs232 port to link master
E 3
I 3
 * rs232 port to the stepper motors
      sends and receives ascii strings through the rs232 port to link master
I 8
      to distinguish port we have 1=laser filter motor, 0=fiber mask motor

I 9
        0/1  1 is cal motors    0 is the laser filter motor 
E 9
E 8
E 3
 	laser_motor_init
	cal_motor_init
D 8
 	send_mess
 	read_mess2
E 8
I 8
 	send_mess 0/1
 	read_mess 0/1
E 8
 	laser_motor
	cal_motor
 	semix_test
 	lpos_open()  move the motor
 	lpos_A()
 	lpos_B()
I 6
D 8
	close_port()
E 8
I 8
	close_ECport()   0/1
I 9

E 9
E 8
E 6
 * camac pulser
D 3
   downloads a pulse sequence and starts pulser   debug_pulser=1 for debuging
E 3
I 3
      downloads a pulse sequence and starts pulser debug_pulser=1 for debuging
E 3
   	make_pulser_data   makes the data on the ROC
   	test_pulser_data   prints the data buffer
       	get_data           loads data from clon's
 	load_pulser        loads the on ROC to the pulser
 	dataway_start
  	ext_start_on
 	ext_start_off
 	get_pulser_status
 	pulser_output_enable    ; should be written for completeness
 	pulser_output_disable    ; """"""""""  not yet done
 	(get_data, load_pulser, ext_start_on, dataway_start== get a pulse train)
 	go_lpulser  combo
 	stop_lpulser
 	go_pulse    use the task delay and run cont. 
 	
  all camac uses the coda callabler routines 
  	cdreg(int *a, int b, int c, int n, int aa)
  	      establish an address that points to the camac device
	cfsa(int f, int e, int *d, int *q) single function, 32 bit data
	ctci(int aa, int *l) test inhibit
	ccci(int aaa, int ll) control crate inhibit
	ctsta(int *s) test status
	
 * VME input register
 	getstatus
 
 * other
 	send_test  just sends ascii strings for master -slave com testing	
D 3
		
E 3
I 3
	
E 3
 */

D 3


/*
 *  Includes
 */
E 3
  
D 3
    #include <vxWorks.h>
    #include <ioLib.h>       /* probably the communications function  lib */
    #include <tyLib.h>
    #include <ctype.h>
    #include <selectLib.h>
    #include <stdio.h>
    #include <string.h>
    #include <taskLib.h>
/*  #include <semLib.h>  */
E 3
I 3
#include <vxWorks.h>
#include <ioLib.h>
#include <tyLib.h>
#include <ctype.h>
#include <selectLib.h>
#include <stdio.h>
#include <string.h>
#include <taskLib.h>
I 4
#include "laser.h"
E 4
E 3

I 6



#include "drv6016.h"
#include "params.h"

E 6
D 3
/*
 *  Defines 
 */
E 3
D 4

E 4
D 3
    #define CR	        13
    #define DELIM       '!'
    #define GTsym       '>'
    #define BADstring   -2
    #define LONGread     2
    #define OK_GTonly    0
    #define OKstring     1
    #define ETX	         3
    #define EOS	       '\0'
    #define MAXLEN      1023
E 3
I 3
#define CR	        13
#define DELIM       '!'
#define GTsym       '>'
#define BADstring   -2
#define LONGread     2
#define OK_GTonly    0
#define OKstring     1
#define ETX	         3
#define EOS	       '\0'
#define MAXLEN      1023
E 3

I 3
#define INIT_ERROR   1 /* semix Initialization Error */
#define SUCCESS      0
#define WRITE_ERROR  1
#define READ_ERROR 	 1
E 3

I 9


E 9
D 3
    /* Subroutine Return Defines */

       #define INIT_ERROR        1      /* semix Initialization Error */
       #define SUCCESS           0
       #define WRITE_ERROR 	 1
       #define READ_ERROR 	 1

E 3
/*
 *  Global Debug Flags 
 */

I 9
int maxrd= MAXLEN;
E 9
D 3
    int DEBUG1 = 0;                /* Debug flag for dynaPowerLib.c */
    int ReadbackTimeoutCount = 16; /* 1 second */
    int readDelay = 6;		   /* 1/12 of a second */
    int move_delay = 2000;		   /* 1/12 of a second */
    int pulser_interim = 3000;
    char tbuf[1024];		   /* Test Buffer */
    int pcount=1000;
    int go_pulse_on=1;		/*set to 1 for recycle 0 for stop */
E 3
I 3
int DEBUG1 = 0;                /* Debug flag for dynaPowerLib.c */
int ReadbackTimeoutCount = 16; /* 1 second */
int readDelay = 6;		   /* 1/12 of a second */
int move_delay = 2000;		   /* 1/12 of a second */
int pulser_interim = 3000;
char tbuf[1024];		   /* Test Buffer */
int pcount=1000;
int go_pulse_on=1;		/*set to 1 for recycle 0 for stop */
E 3


I 6
static char enable_OnLine_mode = 'F';   /* E-with echo ON, F-with echo OFF */

E 6
/*
 *  Init Flags 
 */

D 3
    char CalPortName[] = {"/tyIP/9"};   
    char LaserPortName[] = {"/tyIP/11"};    /* Serial Port Device Names */
    int CalPort = 0;
    int LaserPort = 0;                    /* Data Acquistion File Descriptors */
    int calInitialized = 0;
    int laserInitialized = 0;              /* Global Flag Used for Epics call */
    char semixbuf[MAXLEN]={"this is the semix buffer"}	;	
E 3
I 3
D 5
char CalPortName[] = {"/tyIP/9"};   
char LaserPortName[] = {"/tyIP/11"}; /* Serial Port Device Names */
E 5
D 6
int CalPort = 0;
int LaserPort = 0;                   /* Data Acquistion File Descriptors */
E 6
I 6
int CalPort = -1;
int LaserPort = -1;                   /* Data Acquistion File Descriptors */
E 6
int calInitialized = 0;
int laserInitialized = 0;            /* Global Flag Used for Epics call */
char semixbuf[MAXLEN]={"this is the semix buffer"}	;	
E 3
    
/*
 *  Used for laser and pulser commands
 */
 
D 3
    extern void cdreg(int *a, int b, int c, int n, int aa);
    extern void cfsa(int f, int e, int *d, int *q);
    extern void ctci(int aa, int *l);
    extern void ccci(int aaa, int ll);
    extern void ctsta(int *s);
E 3
I 3
extern void cdreg(int *a, int b, int c, int n, int aa);
extern void cfsa(int f, int e, int *d, int *q);
extern void ctci(int aa, int *l);
extern void ccci(int aaa, int ll);
extern void ctsta(int *s);
E 3

D 3
    int sgas=0;
    int spul=0;
    int slpower=0;
E 3
I 3
int sgas=0;
int spul=0;
int slpower=0;
E 3
    
D 3
    int soft_status_lpower=0;  /* power on =1 power off =0 */
    int soft_status_laser=0;   /* laser enabled =1 laser disabled =0 */
    int soft_status_gas=0;     /* gas on =1 gass off =0 */
    int soft_status_hp_pul=0;  /* hp pulser status 1=on (register state only)*/
    int soft_status_stepper=0; /* stepper on =1 off =0 */
    int soft_status_pul_get=0; /* get data  sets it to 1 */
    int soft_status_pul_make=0; /* make pulser data  sets it to 1 */
    int soft_status_pul_ld=0;  /* load sets to 1 */
    int soft_status_pul_ext=0; /* on for external start */
    int soft_status_vme_reg=0; /* last value of vme status register */
    int soft_status_l_motor=0; /* 0 not initialized  1 init */
    int soft_status_c_motor=0; /*  0 cal motor nit init */
E 3
I 3
int soft_status_lpower=0;  /* power on =1 power off =0 */
int soft_status_laser=0;   /* laser enabled =1 laser disabled =0 */
int soft_status_gas=0;     /* gas on =1 gass off =0 */
int soft_status_hp_pul=0;  /* hp pulser status 1=on (register state only)*/
int soft_status_stepper=0; /* stepper on =1 off =0 */
int soft_status_pul_get=0; /* get data  sets it to 1 */
int soft_status_pul_make=0; /* make pulser data  sets it to 1 */
int soft_status_pul_ld=0;  /* load sets to 1 */
int soft_status_pul_ext=0; /* on for external start */
int soft_status_vme_reg=0; /* last value of vme status register */
int soft_status_l_motor=0; /* 0 not initialized  1 init */
int soft_status_c_motor=0; /*  0 cal motor nit init */
E 3

D 3
    int stepper_off_bit  = 0x200 ;  /* bit 10 pulse using delay */
    int stepper_on_bit   = 0x100 ;  /* bit  9 pulse using delay */
    int lpower_onoff_bit   = 0x80  ;  /* bit  8 level  */
    int laser_off_bit    = 0x20  ;  /* bit  6 pulse using delay */
    int laser_on_bit     = 0x10  ;  /* bit  5 pulse using delay */
    int pulser_on_bit    = 0x8   ;  /* bit  4 level 1 line only */
    int gas_onoff_bit       = 0x4   ;  /* bit  3 level 1 line only */
E 3
I 3
int stepper_off_bit  = 0x200 ;  /* bit 10 pulse using delay */
int stepper_on_bit   = 0x100 ;  /* bit  9 pulse using delay */
int lpower_onoff_bit   = 0x80  ;  /* bit  8 level  */
int laser_off_bit    = 0x20  ;  /* bit  6 pulse using delay */
int laser_on_bit     = 0x10  ;  /* bit  5 pulse using delay */
int pulser_on_bit    = 0x8   ;  /* bit  4 level 1 line only */
int gas_onoff_bit       = 0x4   ;  /* bit  3 level 1 line only */
E 3

D 3
    int p_branch = 0;
    int p_crate = 0;
    int p_slot = 12;
    int p_address0 = 0;
    int p_address1 = 1;
    int p_address2 = 2;
E 3
I 3
int p_branch = 0;
int p_crate = 0;
int p_slot = 12;
int p_address0 = 0;
int p_address1 = 1;
int p_address2 = 2;
E 3

D 3
    int r_branch = 0;
    int r_crate = 0;
    int r_slot = 10;
    int r_address = 0;
    int debug_pulser = 0;
    int debug_register =0;  
    int debug_vme = 0; 
    
    long numberOfPulses = 300;
    long output_data[1024], set_data[1024];
    long no_pulser_data=0;
    long period_10usec=10000;
E 3
I 3
int r_branch = 0;
int r_crate = 0;
int r_slot = 10;
int r_address = 0;
int debug_pulser = 0;
int debug_register =0;  
int debug_vme = 0; 
E 3

I 3
long numberOfPulses = 300;
long output_data[1024], set_data[1024];
long no_pulser_data=0;
long period_10usec=10000;

E 3
/* status register for system */
D 3
    int calsys_status = 0;
E 3
I 3
int calsys_status = 0;
E 3
    	
D 3

E 3
/* 36 provides 380ms pulse, works for laser enable-disable */
D 3
    int pulseDelay = 36;	
E 3
I 3
int pulseDelay = 36;	
E 3
    			    
I 5
/* Green Spring stuff */
#ifdef GREEN_SPRING

char CalPortName[] = {"/tyIP/9"};   
char LaserPortName[] = {"/tyIP/11"}; /* Serial Port Device Names */

E 5
D 3
/*----------------------------------------------------------------------------*/
    int cal_motor_init()

E 3
I 3
/*---------------------------------------------------------------------------*/
E 3
/*  opens serial port to SEMIX LinkMaster RC-002, sets boud rate, flushes
 *  I/O pot
 */
D 3
  
E 3
I 3
int
cal_motor_init()
E 3
{	
D 3
	
	/*
	 *  Open Serial Port 
         */
E 3
I 3
  /* Open Serial Port */
  if((CalPort = open(CalPortName, O_RDWR, 0777)) == ERROR)
  {
    printf("cal_motor_init(): Could not open %s\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	if ((CalPort = open(CalPortName,O_RDWR,0777)) == ERROR) {
	    printf("cal_motor_init(): Could not open %s\n",CalPortName);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}
E 3
I 3
  /* Set Baud Rate */
  if(ioctl(CalPort, FIOBAUDRATE, 9600) == ERROR)
  {
    printf("cal_motor_init(): Could not set baud rate\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	/*
	 *  Set Baud Rate 
         */
E 3
I 3
  /* Set Raw Mode */
  if(ioctl(CalPort, FIOSETOPTIONS, OPT_RAW) == ERROR)
  {
    printf("cal_motor_init(): Could not set raw mode\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	if (ioctl(CalPort, FIOBAUDRATE, 9600) == ERROR ) {
	    printf("cal_motor_init(): Could not set baud rate\n");
	    close(CalPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}
E 3
I 3
  /* Flush I/O Port */
  if(ioctl(CalPort, FIOFLUSH, 0) == ERROR)
  {
    printf("cal_motor_init(): Could not Flush I/O Buffer\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	/*
	 *  Set Raw Mode 
	 */
E 3
I 3
  calInitialized = 1; /* Set semix Initialization Flag */
  soft_status_c_motor = 1;
  printf("!!I!0!!\n");
E 3

D 3
	if (ioctl(CalPort, FIOSETOPTIONS, OPT_RAW) == ERROR ) {
	    printf("cal_motor_init(): Could not set raw mode\n");
	    close(CalPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}

	/*
	 *  Flush I/O Port 
	 */

	if (ioctl(CalPort, FIOFLUSH, 0) == ERROR ) {
	    printf("cal_motor_init(): Could not Flush I/O Buffer\n");
	    close(CalPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}

	calInitialized = 1;	/* Set semix Initialization Flag */
	soft_status_c_motor=1;
	printf("!!I!0!!\n");
	return(SUCCESS);
E 3
I 3
  return(SUCCESS);
E 3
}
D 3
/*----------------------------------------------------------------------------*/
    int laser_motor_init()
E 3

I 3
/*---------------------------------------------------------------------------*/
E 3
/*  opens serial port to SEMIX LinkMaster RC-002, sets boud rate, flushes
 *  I/O pot
 */
D 3
  
{	
	
	/*
	 *  Open Serial Port 
         */
E 3
I 3
int
laser_motor_init()
{
  /* Open Serial Port */
  if((LaserPort = open(LaserPortName, O_RDWR, 0777)) == ERROR)
  {
D 6
    printf("laser_motor_init(): Could not open %s \n\n",LaserPortName);
E 6
I 6
    printf("Laser_motor_init(): Could not open %s \n\n",LaserPortName);
E 6
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	if ((LaserPort = open(LaserPortName,O_RDWR,0777)) == ERROR) {
	    printf("laser_motor_init(): Could not open %s \n\n ",LaserPortName);
            printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}
E 3
I 3
  /* Set Baud Rate */
  if(ioctl(LaserPort, FIOBAUDRATE, 9600) == ERROR)
  {
    printf("laser_motor_init(): Could not set baud rate\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	/*
	 *  Set Baud Rate 
         */
E 3
I 3
  /* Set Raw Mode */
  if(ioctl(LaserPort, FIOSETOPTIONS, OPT_RAW) == ERROR)
  {
    printf("laser_motor_init(): Could not set raw mode\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	if (ioctl(LaserPort, FIOBAUDRATE, 9600) == ERROR ) {
	    printf("laser_motor_init(): Could not set baud rate\n");
	    close(LaserPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}
E 3
I 3
  /* Flush I/O Port */
  if(ioctl(LaserPort, FIOFLUSH, 0) == ERROR)
  {
    printf("laser_motor_init(): Could not Flush I/O Buffer\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }
E 3

D 3
	/*
	 *  Set Raw Mode 
	 */
E 3
I 3
  laserInitialized = 1; /* Set semix Initialization Flag */
  soft_status_l_motor = 1;
  printf("!!I!0!!\n");
E 3

D 3
	if (ioctl(LaserPort, FIOSETOPTIONS, OPT_RAW) == ERROR ) {
	    printf("laser_motor_init(): Could not set raw mode\n");
	    close(LaserPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}

	/*
	 *  Flush I/O Port 
	 */

	if (ioctl(LaserPort, FIOFLUSH, 0) == ERROR ) {
	    printf("laser_motor_init(): Could not Flush I/O Buffer\n");
	    close(LaserPort);
	    printf("!!I!1!!\n");
	    return(INIT_ERROR);
	}

	laserInitialized = 1;	/* Set semix Initialization Flag */
	soft_status_l_motor=1;
	printf("!!I!0!!\n");
	return(SUCCESS);
E 3
I 3
  return(SUCCESS);
E 3
}

I 5
#else

char CalPortName[] = {"/tty6016/5"};   
char LaserPortName[] = {"/tty6016/6"}; /* Serial Port Device Names */
I 6


E 6
int
cal_motor_init()
{
D 6
 printf("TO DO !!!\n");	
E 6
I 6
  CONFIG_BUFFER       bufferConfigInfo;  /* Holds buffer config */
  CONFIG_CHANNEL      channelConfigInfo; /* Channel config info */
  SET_CHANNEL_ENABLE  channelEnableInfo; /* Channel enable info */


 /* check if it opened already */
  if(CalPort != ERROR)
  {
    printf("cal_motor_init(): Already open %s \n\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }


  /* open channel */
D 7
  CalPort = open(CalPortName, O_RDWR, 0777);
E 7
I 7
  CalPort = open(CalPortName, O_RDWR, 0644);
E 7
  if(CalPort == ERROR)
  {
    printf("cal_motor_init(): Open error %s \n\n",CalPortName);
D 8
    /*close_port(CalPort);*/
E 8
I 8
D 9
    close_ECport(0);
E 9
I 9
    close_ECport(1);
E 9
E 8
    printf("cal_motor_init(): Close port value now=  %d \n\n",CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Initialize all channels */

  /* Configure the buffer parameters */
  bufferConfigInfo.BufferAddress   = SLAVE_ADDR;
  bufferConfigInfo.ReleaseMode     = RELROR;
  bufferConfigInfo.FairnessTimeout = 0xf;
  bufferConfigInfo.UseA32          = FALSE;
  bufferConfigInfo.UseSupervisor   = FALSE;
  if(ioctl(CalPort, CONFIG_6016_BUFFER,
           (int)&bufferConfigInfo) == ERROR)
  {
    printf("cal_motor_init(): init error %s \n\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

 /* Configure the channel */
  channelConfigInfo.FlowControl  = NO_FLOW;
  channelConfigInfo.BaudRate     = BAUDS_9600;
  channelConfigInfo.InputTimeout = THREE_CHAR_TIMEOUT;
  channelConfigInfo.StopBits     = TWO_STOP_BITS;
  channelConfigInfo.ParityType   = NO_PARITY;
  channelConfigInfo.ParitySense  = EVEN_PARITY;
  channelConfigInfo.NumBits      = SEVEN_BITS;
  if(ioctl(CalPort, CONFIG_6016_CHANNEL,
           (int)&channelConfigInfo) == ERROR )
  {
    printf("cal_motor_init(): config error %s \n\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Enable Channel */
  channelEnableInfo.Enable = TRUE;
  if(ioctl(CalPort, SET_6016_CHANNEL_ENABLE,
           (int)&channelEnableInfo) == ERROR)
  {
    printf("cal_motor_init(): enable error %s \n\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* enable On-Line mode */
  write(CalPort, &enable_OnLine_mode, 1);

  calInitialized = 1; /* Set semix Initialization Flag */
  soft_status_c_motor = 1;
  printf("!!I!0!!\n");
  return(SUCCESS);

E 6
}
I 6


E 6
int
laser_motor_init()
{
D 6
 printf("TO DO !!!\n");
E 6
I 6
  CONFIG_BUFFER       bufferConfigInfo;  /* Holds buffer config */
  CONFIG_CHANNEL      channelConfigInfo; /* Channel config info */
  SET_CHANNEL_ENABLE  channelEnableInfo; /* Channel enable info */


 /* check if it opened already */
  if(LaserPort != ERROR)
  {
    printf("laser_motor_init(): Already open %s \n\n",LaserPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }


  /* open channel */
D 9
  LaserPort = open(LaserPortName, O_RDWR, 0777);
E 9
I 9
  LaserPort = open(LaserPortName, O_RDWR, 0644);
E 9
  if(LaserPort == ERROR)
  {
    printf("laser_motor_init(): Open error %s \n\n",LaserPortName);
D 8
    /* close_port(LaserPort);*/
E 8
I 8
D 9
    close_ECport(1);
E 9
I 9
    close_ECport(0);
E 9
E 8
    printf("laser_motor_init(): Close port value now=  %d \n\n",LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Initialize all channels */

  /* Configure the buffer parameters */
  bufferConfigInfo.BufferAddress   = SLAVE_ADDR;
  bufferConfigInfo.ReleaseMode     = RELROR;
  bufferConfigInfo.FairnessTimeout = 0xf;
  bufferConfigInfo.UseA32          = FALSE;
  bufferConfigInfo.UseSupervisor   = FALSE;
  if(ioctl(LaserPort, CONFIG_6016_BUFFER,
           (int)&bufferConfigInfo) == ERROR)
  {
    printf("laser_motor_init(): init error %s \n\n",LaserPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

 /* Configure the channel */
  channelConfigInfo.FlowControl  = NO_FLOW;
  channelConfigInfo.BaudRate     = BAUDS_9600;
  channelConfigInfo.InputTimeout = THREE_CHAR_TIMEOUT;
  channelConfigInfo.StopBits     = TWO_STOP_BITS;
  channelConfigInfo.ParityType   = NO_PARITY;
  channelConfigInfo.ParitySense  = EVEN_PARITY;
  channelConfigInfo.NumBits      = SEVEN_BITS;
  if(ioctl(LaserPort, CONFIG_6016_CHANNEL,
           (int)&channelConfigInfo) == ERROR )
  {
    printf("laser_motor_init(): config error %s \n\n",LaserPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Enable Channel */
  channelEnableInfo.Enable = TRUE;
  if(ioctl(LaserPort, SET_6016_CHANNEL_ENABLE,
           (int)&channelEnableInfo) == ERROR)
  {
    printf("laser_motor_init(): enable error %s \n\n",LaserPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* enable On-Line mode */
  write(LaserPort, &enable_OnLine_mode, 1);

  calInitialized = 1; /* Set semix Initialization Flag */
  soft_status_c_motor = 1;
  printf("!!I!0!!\n");
  return(SUCCESS);

E 6
}
I 6
D 9
/*********************
E 9
I 9

E 9
D 8
int
close_port(int whichPort)
E 8
I 8
int close_ECport(int whichPort)
E 8
{
D 8
  if(whichPort == CalPort)
    {
      close(CalPort);
      CalPort=-1;
      return(SUCCESS);
    }
  if(whichPort == LaserPort)
    {
      close(LaserPort);
      LaserPort=-1;
      return(SUCCESS);
    }
E 8
I 8
  int SerialPort;          
D 9
  if(whichPort) SerialPort = CalPort;
  else          SerialPort = LaserPort; 
 
E 9
I 9
  if(whichPort)
    { 
   SerialPort = CalPort;
    printf("close cal port = %d \n", CalPort);
    } 
  else
    {          
   SerialPort = LaserPort; 
   printf("close laser port  port = %d \n", LaserPort);
    }

E 9
  close(SerialPort);

  if(whichPort) CalPort=-1;
  else          LaserPort=-1; 

  return(SUCCESS);

E 8
}
D 9
	   ************/
E 9
I 9

E 9
E 6
#endif


E 5
D 3
/*----------------------------------------------------------------------------*/
    int send_mess(char *raw, int whichPort)

E 3
I 3
/*---------------------------------------------------------------------------*/
E 3
/*  sends argument to the RS232 Port
 *  returns error status (defined in SUCCESS and WRITE_ERROR)
 */
D 3

E 3
I 3
int
send_mess(char *raw, int whichPort)
E 3
{
D 3
           
	char output[140];
	int msgSize = 0;		/* Number of bytes to write */
	int bytesSent = 0;		/* Number of bytes written */
        int SerialPort;          
        int i=0;
        
        if (whichPort) SerialPort = CalPort;
        else SerialPort = LaserPort; 
    /*
     *  Write Message 
     */
   
   
       
        do {output[i]= raw[i]; ++i;} while ( raw[i]!=EOS );
        output[i]= '\r'; output[++i]= EOS;   
   
 	msgSize = strlen(output);
  
        printf(" send= %s \n", output);
            
	bytesSent = write(SerialPort, output, msgSize); 
E 3
I 3
  char output[140];
  int msgSize = 0;   /* Number of bytes to write */
  int bytesSent = 0; /* Number of bytes written */
  int SerialPort;          
  int i=0;
E 3

D 3
	if (DEBUG1)
		printf("\nsend_mess(): Message Size: <%d> Actual Sent <%d>\n",
			msgSize,bytesSent);
E 3
I 3
  if(whichPort) SerialPort = CalPort;
  else          SerialPort = LaserPort; 
E 3

I 9
  printf("send message to port %d laser port = %d \n",SerialPort,LaserPort);

E 9
D 3
	if (bytesSent != msgSize) {
		if (DEBUG1) {
			printf("\nsend_mess(): Message Size: <%d> != Actual Sent
			<%d>\n", msgSize,bytesSent);
		}
		return(WRITE_ERROR);
	}
	return(SUCCESS);
E 3
I 3
  /* Write Message */
  do
  {
    output[i] = raw[i];
    ++i;
  } while (raw[i]!=EOS);
  output[i]= '\r'; output[++i]= EOS;   
  msgSize = strlen(output);
  printf(" send= %s \n", output);
  bytesSent = write(SerialPort, output, msgSize); 
D 9
#ifdef DEBUG
E 9
I 9
    /** #ifdef  DEBUG   ***/
E 9
  printf("\nsend_mess(): Message Size: <%d> Actual Sent <%d>\n",
         msgSize,bytesSent);
D 9
#endif
E 9
I 9
  /*** #endif  ***/
E 9
  if(bytesSent != msgSize)
  {
D 9
#ifdef DEBUG
E 9
I 9
    /** #ifdef  DEBUG   ***/
E 9
    printf("\nsend_mess(): Message Size: <%d> != Actual Sent <%d>\n",
           msgSize,bytesSent);
D 9
#endif
E 9
I 9
    /*** #endif  ***/

E 9
    return(WRITE_ERROR);
  }

  return(SUCCESS);
E 3
}


D 3
/*----------------------------------------------------------------------------*/
    int read_mess(int whichPort)

E 3
I 3
/*---------------------------------------------------------------------------*/
E 3
/*  modification of read_mess by A. Coleman, K. Beard, and J. Voshell 6/5/97 
 * 
 *  reads output from Semix LinkMaster RC-002 into an RS232 serial port
 *  into input[MAXLEN].
 * 
 *  transfers this string to clean[MAXLEN], looking for a > and a CR
 *
 *  outputs the following to standard output:
 *
D 3
 *      !!I!n!S!clean!!CR  ...  where n is a status word depending on the contents 
E 3
I 3
 *  !!I!n!S!clean!!CR  ...  where n is a status word depending on the contents 
E 3
 *                        of input[MAXLEN]:
 *                        
 *                        n = ... |  when input[MAXLEN] contains ...
 *                             0  |  ">" w/ or w/o a CR
 *                             1  |  ">..." w/  a CR
 *                             2  |  ">..." w/o a CR
 *                            -2  |  no leading ">"
 *
 *                        n is to aid in pasing by TCL later
 *  returns n 
 */
D 3
 
E 3
I 3
int
read_mess(int whichPort)
E 3
{
D 3
	char clean[MAXLEN];
        char input[MAXLEN];
	int status = -9;
        int i = 0;
	int k;
	int fndCR=0;
	int j = 0;
	int NumUnRead = -1; 
   	int Count = 0;
        int SerialPort;
        
        if (whichPort) SerialPort = CalPort;
        else SerialPort = LaserPort;   
  
    /*  
     *  Waits, then finds number of bytes waiting in buffer (these are the 
     *  unread counted in NumUnRead ). Puts ith character into input[i],
     *  increments i and continues in this fashion until the maximum length
     *  is reached, a carriage return as defined by CR is reached, or te
     *  count exceeds the amount defined in ReadbackTimeoutCount
     */
E 3
I 3
  char clean[MAXLEN];
  char input[MAXLEN];
  int status = -9;
  int i = 0;
  int k;
  int fndCR=0;
  int j = 0;
  int NumUnRead = -1; 
  int Count = 0;
  int SerialPort;
I 8
  int noRD=0;
E 8
E 3

D 3
	do {
      		taskDelay(readDelay);                        
		ioctl(SerialPort, FIONREAD, (int)&NumUnRead);   
		
		if ((NumUnRead + i) > MAXLEN) 
		{ 
		        break;
		}
		          
		if (NumUnRead != 0)  
		{
			read(SerialPort, &input[i], NumUnRead);
			i = i + NumUnRead; 
		}
E 3
I 3
  if(whichPort) SerialPort = CalPort;
  else SerialPort = LaserPort;   
E 3

I 9

E 9
D 3
		Count++;
	
		j = (i > 0) ? i : 0; 
E 3
I 3
  /*  
   *  Waits, then finds number of bytes waiting in buffer (these are the 
   *  unread counted in NumUnRead ). Puts ith character into input[i],
   *  increments i and continues in this fashion until the maximum length
   *  is reached, a carriage return as defined by CR is reached, or te
   *  count exceeds the amount defined in ReadbackTimeoutCount
   */
I 8

  /********************** do while loop ****************/
I 9
  printf(" openeing port %d  filter port= %d \n",SerialPort,LaserPort);
E 9
E 8
  do
  {
    taskDelay(readDelay);
D 8
    ioctl(SerialPort, FIONREAD, (int)&NumUnRead);
E 8
I 8


#ifdef GREEN_SPRING
    /* gets the number of bytes in the buffer. Not sure if it works for VMIVEM 6016 */
           NumUnRead = -1;   
           ioctl(SerialPort, FIONREAD, (int)&NumUnRead);
#else
D 9
           NumUnRead = MAXLEN;
E 9
I 9
           NumUnRead = 10;
	   printf(" no to read =%d \n", NumUnRead);
E 9
#endif

E 8
D 9
    if((NumUnRead + i) > MAXLEN) break;
E 9
I 9
    if((NumUnRead + i) > maxrd) break;
E 9
I 8
 
E 8
    if(NumUnRead != 0)
D 8
    {
      read(SerialPort, &input[i], NumUnRead);
E 8
I 8
     {

#ifdef GREEN_SPRING 
                read(SerialPort, &input[i], NumUnRead);
#else 
          /* maintain compatablity with old code***********/
          /* request a large read find out the actual amount and set the NumUnRead vlaue **/
D 9
           noRD=read(SerialPort, &input[i], NumUnRead);
E 9
I 9
	   printf(" start read requ=  %d port= %d \n", NumUnRead, SerialPort);

D 10
           noRD=read(SerialPort, input, NumUnRead);
E 10
I 10
           noRD=read(SerialPort, &input[i], NumUnRead);
E 10
E 9
           NumUnRead=noRD;
I 9
	   printf(" finiahed read requ=  %d rec= %d \n", NumUnRead, noRD);
	   printf(" data = %s \n", input);


E 9
#endif 

E 8
      i = i + NumUnRead; 
D 8
    }
E 8
I 8
     }
  
E 8
    Count++;
    j = (i > 0) ? i : 0; 
I 8

E 8
  } while( (input[j] != CR) && (Count < ReadbackTimeoutCount) );
E 3

I 8

/************* end of the do while  ****************/

E 8
D 3
	}  
	
	while( (input[j] != CR) && (Count < ReadbackTimeoutCount) );
E 3
I 3
  /* Pastes a "null" on the end of the input as an End-Of-String */
  input[j] = EOS;
E 3

D 3
    /* 
     *  Pastes a "null" on the end of the input as an End-Of-String
     */
E 3
I 3
  /*
   *  Did we get a <CR>? If so, set fndCR and replace it with EOS
   *  If not, press on.
   */
  k=0;
  do
  {
    clean[k] = input[k]; 
    if(clean[k]==CR)
    {
      fndCR=1;
      clean[k]= EOS;
    }
    k++;
  } while(k <= j);	
E 3

D 3
	input[j] = EOS; 
E 3
I 3
  /* What kind of response did the RC-002 send us ? */
  if(clean[0]==GTsym && clean[1]==EOS)  /* only a ">"                */
    status= OK_GTonly;
  else if (!fndCR &&  clean[0]==GTsym)  /* ">" with no <CR> anywhere */
    status= LONGread;
  else if ( clean[0]==GTsym )           /* ">" with a <CR> somewhere */
    status= OKstring;
  else 
    status= BADstring;                  /* doesn't start with a ">"  */
E 3

D 3
    /*
     *  Did we get a <CR>? If so, set fndCR and replace it with EOS
     *  If not, press on.
     */ 
E 3
I 3
  /* Print contents of clean */
  printf("!!I!%d!S!%s!!\n",status,clean);
E 3

D 3
        k=0;  

	do {
		clean[k]=input[k]; 
	        if(clean[k]==CR) {fndCR=1; clean[k]= EOS;};
                k++;

	}  while(k <= j);	

    /* 
     *  What kind of response did the RC-002 send us ?
     */

	if ( clean[0]==GTsym && clean[1]==EOS )  /* only a ">"                */
	     status= OK_GTonly;

        else if (!fndCR &&  clean[0]==GTsym)     /* ">" with no <CR> anywhere */
	     status= LONGread;

        else if ( clean[0]==GTsym )              /* ">" with a <CR> somewhere */
             status= OKstring;

        else 
	     status= BADstring;                  /* doesn't start with a ">"  */

    /*
     *  Print contents of clean
     */

        printf("!!I!%d!S!%s!!\n",status,clean);

	return status;                           /* why not?, not really used */

E 3
I 3
  return(status);                       /* why not?, not really used */
E 3
}

D 3
void cal_motor(char *mss)
E 3
I 3
void
cal_motor(char *mss)
E 3
{
D 3
	send_mess(mss,1);
	read_mess(1);
E 3
I 3
D 9
  send_mess(mss,1);
E 9
I 9
  send_mess(mss,1);   
E 9
  read_mess(1);
E 3
}

D 3
void laser_motor(char *mss)
E 3
I 3
void
laser_motor(char *mss)
E 3
{
D 3
	send_mess(mss,0);
	read_mess(0);
E 3
I 3
D 9
  send_mess(mss,0);
E 9
I 9
   printf(" laser motor command \n");
  send_mess(mss,0); 
E 9
  read_mess(0);
E 3
}

D 3
void lpos_open()
E 3
I 3
void
lpos_open()
E 3
{
D 3
	laser_motor("$1");
        printf("\n");
	laser_motor("$18");
        printf("\n");
        taskDelay(move_delay);
	laser_motor("$19");
        printf("\n");	
	laser_motor("$1CL");	
E 3
I 3
  laser_motor("$1");
  printf("\n");
  laser_motor("$18");
  printf("\n");
  taskDelay(move_delay);
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
E 3
}

D 3
void lpos_A()
E 3
I 3
void
lpos_A()
E 3
{
D 3
	laser_motor("$1");
        printf("\n");
        laser_motor("$18");
        printf("\n");
        taskDelay(move_delay);
	laser_motor("$19");
        printf("\n");	
	laser_motor("$1CL");	
	laser_motor("$121600");
	laser_motor("$14");	
        printf("\n");
        taskDelay(move_delay);
	laser_motor("$19");
        printf("\n");	
	laser_motor("$1CL");	
E 3
I 3
  laser_motor("$1");
  printf("\n");
  laser_motor("$18");
  printf("\n");
  taskDelay(move_delay);
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
  laser_motor("$121600");
  laser_motor("$14");	
  printf("\n");
  taskDelay(move_delay);
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
E 3
}
D 3
void lpos_B()
E 3
I 3

void
lpos_B()
E 3
{
D 3
	laser_motor("$1");
        printf("\n");
        laser_motor("$18");
        printf("\n");
        taskDelay(move_delay);       
	laser_motor("$19");
        printf("\n");	
	laser_motor("$1CL");	
	laser_motor("$122400");
	laser_motor("$14");	
        printf("\n");
        taskDelay(move_delay);
	laser_motor("$19");
        printf("\n");	
	laser_motor("$1CL");	
E 3
I 3
  laser_motor("$1");
  printf("\n");
  laser_motor("$18");
  printf("\n");
  taskDelay(move_delay);       
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
  laser_motor("$122400");
  laser_motor("$14");	
  printf("\n");
  taskDelay(move_delay);
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
E 3
}

D 3
	
void semix_test()
{                   /* mindlessly cycle forever */
	int  j=1;
	do {
	laser_motor("$1");
        printf("\n");
	laser_motor("$1");
        printf("\n#%d\n",j);
	}
	while (j++>0);
E 3
I 3
void
semix_test()
{
  int j = 1;

  do /* mindlessly cycle forever */
  {
    laser_motor("$1");
    printf("\n");
    laser_motor("$1");
    printf("\n#%d\n",j);
  } while (j++>0);
E 3
}

/*
 program to set bits in camac reg

        12 11 10 9   |   8 7 6 5 |   4 3 2 1
        1   0  0 0          0 0 0 0       0 0 0 0   == bit 12 0n == 800 in hex
        0   0  1 0          0 0 0 0       0 0 0 0   == bit 10 on == 200 in hex
        etc... 

D 3
 camac reg changed so that all are levels    ( here we call the first bit 1 , not 0)
E 3
I 3
 camac reg changed so that all are levels
 ( here we call the first bit 1 , not 0)
E 3
             bit 12 == level == Not used ==  			0x800
             bit 11 == level == Not used == 			0x400
             bit 10 == pulse == step pwr on  == 			0x200
             bit 09 == pulse == step pwr off == 			0x100             
             bit 08 == level == las power onoff == ***		0x80             
             bit 07 == level == Not used==			0x40             
D 3
             bit 06 == pulse == laser disable (off)= ***		0x20             
             bit 05 == pulse == laser enable (on)== ****		0x10             
             bit 04 == level == pulser on (NA)  == ***			0x8             
E 3
I 3
             bit 06 == pulse == laser disable (off)= ***		0x20
             bit 05 == pulse == laser enable (on)== ****		0x10
             bit 04 == level == pulser on (NA)  == ***			0x8
E 3
             bit 03 == level == nitrogen gas on == ***		0x4      
             bit 02 == level == Not used == 			0x2
             bit 01 == level == Not used  == 			0x1 (broken)
I 3
*/
int
inhibitclear()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int semixval = 100;
E 3

D 3

*/   

int inhibitclear() {

int *ext_a = (int *)0;
int inhib = 0;
int semixval=100;

E 3
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  inhib = 0;
  ccci((int)ext_a,!!inhib);
D 3
  if (debug_register) printf("inhibit clear for reg \n");
E 3
I 3
  if(debug_register) printf("inhibit clear for reg \n");
E 3
 
D 3
  return semixval;
E 3
I 3
  return(semixval);
E 3
}

D 3
int laser_enable ()  {
E 3
I 3
int
laser_enable()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;
E 3

D 3
int *ext_a = (int *)0;

int inhib = 0;
int data = 0x800;
int q = 0;
int semixval=100;


 
E 3
  data=laser_on_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
  if (debug_register) printf("  inhibit = %d \n",inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

E 3
  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
  if (debug_register) printf(" laser_enable write 1 q = %d \n",q);
E 3
I 3
  if(debug_register) printf(" laser_enable write 1 q = %d \n",q);
E 3
 
  taskDelay(pulseDelay);   
D 3
   data=spul+sgas+slpower; 
E 3
I 3
  data=spul+sgas+slpower; 
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
    if (debug_register) printf(" laser_enable write 2 q = %d \n",q);
    soft_status_laser=1;
  return semixval;
E 3
I 3
  if(debug_register) printf(" laser_enable write 2 q = %d \n",q);

  soft_status_laser = 1;

  return(semixval);
E 3
}

D 3
int laser_disable () 
E 3
I 3
int
laser_disable() 
E 3
{
I 3
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x200;
  int q = 0;
  int semixval = 100;
E 3

D 3
int *ext_a = (int *)0;

int inhib = 0;
int data = 0x200;
int q = 0;
int semixval=100;


E 3
  data=laser_off_bit+spul+sgas+slpower; 
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
   if (debug_register) printf("  inhibit = %d \n",inhib);
   inhib = 0;
   ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
  if (debug_register) printf(" laser_disable write 1 q = %d \n",q);
    taskDelay(pulseDelay);   
   data=spul+sgas+slpower; 
E 3
I 3
  if(debug_register) printf(" laser_disable write 1 q = %d \n",q);

  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
   if (debug_register) printf(" laser_disable write 2 q = %d \n",q);
   soft_status_laser=0;
E 3
I 3
  if(debug_register) printf(" laser_disable write 2 q = %d \n",q);
E 3

D 3
 return semixval ; 
E 3
I 3
  soft_status_laser=0;

  return(semixval);
E 3
}
D 3
int lpoweron ()  {
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
int
lpoweron()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0;
  int q = 0;
  int semixval = 100;
E 3

D 3
int inhib = 0;
int data = 0;
int q = 0;
int semixval=100;
 slpower=lpower_onoff_bit;
E 3
I 3
  slpower=lpower_onoff_bit;
E 3
  data=spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
      ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
  if (debug_register) printf(" poweron write 1 q = %d \n",q);
   soft_status_lpower=1;
    return semixval;
E 3
I 3
  if(debug_register) printf(" poweron write 1 q = %d \n",q);

  soft_status_lpower=1;

  return(semixval);
E 3
}
D 3
int lpoweroff ()  {
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
int
lpoweroff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;
E 3

D 3
int inhib = 0;
int data = 0x800;
int q = 0;
int semixval=100;
E 3
  slpower=0;
  data=spul+sgas+slpower;   
D 3
   cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
E 3
I 3
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
E 3
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a,!!inhib);
   cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
    if (debug_register) printf(" poweroff write 1 q = %d \n",q);
    soft_status_lpower=0;
    return semixval;
}
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);
E 3

D 3
int stepperon ()  {
E 3
I 3
  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" poweroff write 1 q = %d \n",q);
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
  soft_status_lpower=0;
E 3

D 3
int inhib = 0;
int data = 0x800;
int q = 0;
int semixval=100;
E 3
I 3
  return(semixval);
}
E 3

I 3
int
stepperon()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

E 3
  data=stepper_on_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

E 3
  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
    if (debug_register) printf(" poweron write 1 q = %d \n",q);
E 3
I 3
  if(debug_register) printf(" poweron write 1 q = %d \n",q);

E 3
  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q); 
D 3
    if (debug_register) printf(" poweron write 2 q = %d \n",q);
    soft_status_stepper=1;  
  return semixval;
}
E 3
I 3
  if(debug_register) printf(" poweron write 2 q = %d \n",q);
E 3

D 3
int stepperoff ()  {
E 3
I 3
  soft_status_stepper=1;  
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
  return(semixval);
}
E 3

D 3
int inhib = 0;
int data = 0x800;
int q = 0;
int semixval=100;
 
E 3
I 3
int
stepperoff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

E 3
  data=stepper_off_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

E 3
  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
    if (debug_register) printf(" poweroff write 1 q = %d \n",q);
E 3
I 3
  if(debug_register) printf(" poweroff write 1 q = %d \n",q);

E 3
  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q); 
D 3
    if (debug_register) printf(" poweroff write 2 q = %d \n",q);
    soft_status_stepper=0; 
  return semixval;
}
E 3
I 3
  if(debug_register) printf(" poweroff write 2 q = %d \n",q);
E 3

D 3
char gason () {
E 3
I 3
  soft_status_stepper=0; 
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
  return(semixval);
}
E 3

D 3
int inhib = 0;
int data;
int q = 0;
char semixinfo = 63;
E 3
I 3
char
gason()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;
  char semixinfo = 63;
E 3

  sgas=gas_onoff_bit;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
   if (debug_register) printf(" gason write 1 q = %d \n",q); 
   soft_status_gas=1; 
  return semixinfo;
E 3
I 3
  if(debug_register) printf(" gason write 1 q = %d \n",q); 
E 3

I 3
  soft_status_gas=1; 

  return(semixinfo);
E 3
}

D 3
void hp_pulseron () { 
E 3
I 3
void
hp_pulseron()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;
E 3

D 3
int *ext_a = (int *)0;

int inhib = 0;
int data;
int q = 0;

E 3
  spul=pulser_on_bit;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
      if (debug_register) printf(" pulser on write 1 q = %d \n",q);
      soft_status_hp_pul=1;
      return;
}
E 3
I 3
  if(debug_register) printf(" pulser on write 1 q = %d \n",q);
E 3

D 3
void hp_pulseroff () { 
E 3
I 3
  soft_status_hp_pul=1;
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
  return;
}
E 3

D 3
int inhib = 0;
int data;
int q = 0;
E 3
I 3
D 4
void hp_pulseroff()
E 4
I 4
void
hp_pulseroff()
E 4
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;
E 3

  spul=0;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
      if (debug_register) printf(" pulserroff write 1 q = %d \n",q);
      soft_status_hp_pul=0;
      return;
}
E 3
I 3
  if(debug_register) printf(" pulserroff write 1 q = %d \n",q);
E 3

D 3
void gasoff () {
E 3
I 3
  soft_status_hp_pul=0;
E 3

D 3
int *ext_a = (int *)0;
E 3
I 3
  return;
}
E 3

D 3
int inhib = 0;
int data = 0x4;
int q = 0;
E 3
I 3
void
gasoff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x4;
  int q = 0;
E 3

  sgas=0;
  data=sgas+spul+slpower; 
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
D 3
    if (debug_register) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a,!!inhib);
E 3
I 3
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
E 3
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
D 3
      if (debug_register) printf(" gasoff write 1 q = %d \n",q);
      soft_status_gas=0;
      return;
E 3
I 3
  if(debug_register) printf(" gasoff write 1 q = %d \n",q);
E 3

I 3
  soft_status_gas=0;

  return;
E 3
}

I 3



E 3
/********pulser c code*****************************************/

D 3
int get_data()
E 3
I 3
int
get_data()
E 3
{
D 3
	 int counter = 0, foundstart=0;	
E 3
I 3
  int counter = 0, foundstart = 0;
  FILE *file_handle;
  char *input_filename, *word;
  input_filename = "/home/clasrun/jmu/vme-camac/pulserload";
E 3

I 3
  /* returns !Null if can't open file */
  if( !(file_handle = fopen(input_filename,"r")))
  {
    if(debug_pulser) printf("ERROR: Could not open %s",input_filename);
    return(1);
  }
  if(debug_pulser)printf("searching for DATA_BEGIN\n");
  while(!counter) 
  {
    fscanf(file_handle,"%s",word); /* Sergey: word is an empty pointer !!! */
    /*if(debug_pulser)printf("%s\n",word);*/
    if(!strcmp(word,"DATA_BEGIN")) 
    {
      if(debug_pulser)printf("found DATA_BEGIN\n");
      counter=1;
      foundstart=1;
    }
  }	 	
E 3

D 3
	 FILE *file_handle ;
	 char *input_filename, *word;
	 
	 input_filename = "/home/clasrun/jmu/vme-camac/pulserload";
E 3
I 3
  counter = 0;
  if(foundstart)
  {
    while(counter < 1024) 
    { 
      fscanf(file_handle,"%ld",&output_data[counter]);   	
      fscanf(file_handle,"%ld",&set_data[counter]);
      if(debug_pulser) printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
        counter,set_data[counter],counter,output_data[counter]);	
      if(set_data[counter] == 16777215) 
      {
        counter = counter + 1 ;
        if(debug_pulser)
          printf("found -1: counter+1=no of data (2* pulses+2)= %d\n",counter);
        break;
      }
      counter = counter + 1 ;
    }	
  }
  no_pulser_data=counter;
  fclose(file_handle);
  soft_status_pul_get=1; 	 
E 3

D 3
	 if ( !(file_handle = fopen(input_filename,"r"))) /* returns !Null if
	                                                    can't open file */	  
	 {
	 	if (debug_pulser) printf("ERROR: Could not open %s",input_filename);
	 	return(1);
	 }
	 if (debug_pulser)printf("searching for DATA_BEGIN\n");
	 while (!counter) 
	 {

	 	fscanf(file_handle,"%s",word);
/*	 	if (debug_pulser)printf("%s\n",word); */
	 	if (!strcmp(word,"DATA_BEGIN")) 
	 	{
	 	        if (debug_pulser)printf("found DATA_BEGIN\n");
	 		counter=1;
	 		foundstart=1;
	 	}	 	
	 }	
	 	
	 counter = 0;
	 
	 if (foundstart) {
	   	while (counter < 1024) 
	   	{ 
	 		fscanf(file_handle,"%ld",&output_data[counter]);   	
	  		fscanf(file_handle,"%ld",&set_data[counter]);
if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
			
			if (set_data[counter] == 16777215) 
                        {	counter =  counter + 1 ;
	 			if (debug_pulser) printf("found -1: counter+1=no of data (2* pulses+2)= %d \n",counter);
				break ;
                        }
			counter = counter + 1 ;
		}	
	 }
	 no_pulser_data=counter;
	 fclose(file_handle);
	 soft_status_pul_get=1; 	 
	 return(0);
	 
E 3
I 3
  return(0);
E 3
}

D 3
int load_pulser()
E 3
I 3
int
load_pulser()
E 3
{
D 3
int *ext_a0 = (int *)0;
int *ext_a1 = (int *)0;
int data;
int inhib = 0;
int q = 0;
int counter=0;
E 3
I 3
  int *ext_a0 = (int *)0;
  int *ext_a1 = (int *)0;
  int data;
  int inhib = 0;
  int q = 0;
  int counter=0;
E 3

D 3
/* after you execdute a get_data (see above) you want to load
   the data into the two buffers: 
   	settings buffer=  24 bit times for transition
   	output reg buffer= 12 bit value for output at each transistion
 */
E 3
I 3
  /* after you execdute a get_data (see above) you want to load
  the data into the two buffers: 
  settings buffer=  24 bit times for transition
  output reg buffer= 12 bit value for output at each transistion
  */
E 3
   
D 3
 /* set up the pointer to the pulse module using standard commands
   cdreg, checka nd clear inhibit on crate if set */
E 3
I 3
  /* set up the pointer to the pulse module using standard commands
  cdreg, checka nd clear inhibit on crate if set */
E 3
     	
D 3
  if (debug_pulser) printf(" no of pairs of word to load= %ld\n", no_pulser_data);
  cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);  /* point to sub adress 0 */
  cdreg((int *)&ext_a1,p_branch,p_crate,p_slot,p_address1);  /* point to sub adress 1 */
E 3
I 3
  if(debug_pulser)
    printf(" no of pairs of word to load= %ld\n", no_pulser_data);
  /* point to sub address 0 */
  cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);
  /* point to sub address 1 */
  cdreg((int *)&ext_a1,p_branch,p_crate,p_slot,p_address1);
E 3
  ctci((int)ext_a0,&inhib);
D 3
    if (debug_pulser) printf("  inhibit = %d \n",inhib);
    inhib = 0;
   ccci((int)ext_a0,!!inhib);
E 3
I 3
  if(debug_pulser) printf("  inhibit = %d\n",inhib);
  inhib = 0;
  ccci((int)ext_a0,!!inhib);
E 3

D 3
 /*  set starting addrss of buffer to 0 F9A0. prepare for download register */
 cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
 if (debug_pulser) printf(" clear output memory  q = %d \n",q);
 
E 3
I 3
  /* set starting addrss of buffer to 0 F9A0. prepare for download register */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
  if(debug_pulser) printf(" clear output memory  q = %d\n",q);
E 3

I 3
  while(counter < no_pulser_data) 
  {   
    data=output_data[counter];
    /*if (debug_pulser) printf(" data word for output mem %d \n",data);*/
    cfsa(16,(int)ext_a0,(int *)&data,(int *)&q);
    /*if (debug_pulser) printf(" load word  q = %d \n",q);*/
    counter = counter + 1;
  }
E 3

D 3
	   	while (counter < no_pulser_data) 
	   	{   
                        data=output_data[counter];
                     /*   if (debug_pulser) printf(" data word for output mem %d \n",data); */
                        cfsa(16,(int)ext_a0,(int *)&data,(int *)&q); 
                      /*  if (debug_pulser) printf(" load word  q = %d \n",q); */
                        counter=  counter+1;
		}
		
/*  set starting addrss of buffer to 0 F9A0. prepare for download transitions */		
 cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /*  reset  address? */
 if (debug_pulser) printf(" clear set memory  q = %d \n",q);		
                
                counter = 0;
		
		while (counter < no_pulser_data)
		{
                        data=set_data[counter];
                       /* if (debug_pulser) printf(" data word for set mem %d \n",data); */
                        cfsa(16,(int)ext_a1,(int *)&data,(int *)&q);
if(debug_pulser)printf("set_data[%d]= %ld, output_data[%d] =%ld, q= %d\n ",counter, set_data[counter],counter,output_data[counter],q);                     
			counter= counter+1;
E 3
I 3
  /* set starting addrss of buffer to 0 F9A0. prepare for download transitions */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /*  reset  address? */
  if(debug_pulser) printf(" clear set memory  q = %d\n",q);		
E 3

I 3
  counter = 0;	
  while(counter < no_pulser_data)
  {
    data = set_data[counter];
    /*if(debug_pulser) printf(" data word for set mem %d \n",data);*/
    cfsa(16,(int)ext_a1,(int *)&data,(int *)&q);
    if(debug_pulser)
      printf("set_data[%d]= %ld, output_data[%d] =%ld, q= %d\n ",
             counter, set_data[counter],counter,output_data[counter],q);
    counter = counter + 1;
  }
E 3

D 3
		}
/* need to reset the address to point to start of buffers  before a go */		
 cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
 if (debug_pulser) printf(" clear set memory  q = %d \n",q);				
E 3
I 3
  /* need to reset the address to point to start of buffers  before a go */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
  if(debug_pulser) printf(" clear set memory  q = %d\n",q);
E 3

D 3
/* need to enable output word */
 cfsa(26,(int)ext_a1,(int *)&data,(int *)&q);
 soft_status_pul_ld=1;
 return(0);
}	 	
E 3
I 3
  /* need to enable output word */
  cfsa(26,(int)ext_a1,(int *)&data,(int *)&q);
E 3

D 3
void ext_start_on()
{
E 3
I 3
  soft_status_pul_ld = 1;
E 3

D 3
int *ext_a2 = (int *)0;
int data;
int q=0 ;
E 3
I 3
  return(0);
}	 	
E 3

D 3
cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);  /* point to sub adress 2 */
cfsa(26,(int)ext_a2,(int *)&data,(int *)&q); /* enable external start */
soft_status_pul_ext=1;
return;
}
void get_pulser_status()
E 3
I 3
void
ext_start_on()
E 3
{
I 3
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;
E 3

D 3
int *ext_a2 = (int *)0;
int data;
int q=0 ;
E 3
I 3
  /* point to sub adress 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);
  /* enable external start */
  cfsa(26,(int)ext_a2,(int *)&data,(int *)&q);
E 3

D 3
cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address0);  /* point to sub adress 2 */
cfsa(1,(int)ext_a2,(int *)&data,(int *)&q); /* enable external start */
printf("!!I!%d!s!q=%d!! \n",data,q);
E 3
I 3
  soft_status_pul_ext=1;
E 3

D 3
return;
E 3
I 3
  return;
E 3
}
D 3
void ext_start_off()
E 3
I 3

void
get_pulser_status()
E 3
{
I 3
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;
E 3

D 3
int *ext_a2 = (int *)0;
int data;
int q=0 ;
E 3
I 3
  /* point to sub address 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address0);
  /* enable external start */
  cfsa(1,(int)ext_a2,(int *)&data,(int *)&q);
E 3

D 3
cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);  /* point to sub adress 2 */
cfsa(24,(int)ext_a2,(int *)&data,(int *)&q); /* disable external start */
soft_status_pul_ext=0;
return;
E 3
I 3
  printf("!!I!%d!s!q=%d!! \n",data,q);

  return;
E 3
}

D 3
void dataway_start()
E 3
I 3
void
ext_start_off()
E 3
{
I 3
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;
E 3

D 3
int *ext_a0 = (int *)0;
int data;
int q=0 ;
E 3
I 3
  /* point to sub address 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);
  /* disable external start */
  cfsa(24,(int)ext_a2,(int *)&data,(int *)&q);
E 3

D 3
cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);  /* point to sub adress 0 */
cfsa(25,(int)ext_a0,(int *)&data,(int *)&q); /*  start */
E 3
I 3
  soft_status_pul_ext=0;
E 3

D 3
return;
E 3
I 3
  return;
E 3
}

D 3
void go_lpulser()
E 3
I 3
void
dataway_start()
E 3
{
D 3
get_data();
if (debug_pulser) printf("get data done \n ");
load_pulser();
if (debug_pulser) printf("load pulser done \n ");
ext_start_on();
if (debug_pulser) printf("start on \n ");
dataway_start();
if (debug_pulser) printf("pulser going (dataway start) \n ");
return;
E 3
I 3
  int *ext_a0 = (int *)0;
  int data;
  int q = 0;

  /* point to sub address 0 */
  cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);
  /* start */
  cfsa(25,(int)ext_a0,(int *)&data,(int *)&q);

  return;
E 3
}

D 3
void go_pulse()
E 3
I 3
void
go_lpulser()
E 3
{
D 3
get_data();
if (debug_pulser) printf("get data done \n ");
load_pulser();
if (debug_pulser) printf("load pulser done \n ");
dataway_start();
if (debug_pulser) printf("pulser going (dataway start) \n ");
E 3
I 3
  get_data();
  if(debug_pulser) printf("get data done \n");
  load_pulser();
  if(debug_pulser) printf("load pulser done \n");
  ext_start_on();
  if(debug_pulser) printf("start on \n");
  dataway_start();
  if(debug_pulser) printf("pulser going (dataway start) \n");
E 3

D 3
while(pcount&&go_pulse_on)
	{
	if (debug_pulser) printf("pulser while loop pcount= %d  go_pulse_on= %d \n ", pcount, go_pulse_on);
	taskDelay(pulser_interim);
	dataway_start();
	if (debug_pulser) printf("pulser going (dataway start) \n ");
	pcount--;
	}
return;
E 3
I 3
  return;
E 3
}

D 3
void stop_lpulser()
E 3
I 3
void
go_pulse()
E 3
{
I 3
  get_data();
  if(debug_pulser) printf("get data done \n");
  load_pulser();
  if(debug_pulser) printf("load pulser done \n");
  dataway_start();
  if(debug_pulser) printf("pulser going (dataway start) \n");
E 3

D 3
ext_start_off();
if (debug_pulser) printf("start on \n ");
return;
E 3
I 3
  while(pcount&&go_pulse_on)
  {
    if(debug_pulser) printf("pulser while loop pcount= %d  go_pulse_on= %d \n",
      pcount, go_pulse_on);
    taskDelay(pulser_interim);
    dataway_start();
    if(debug_pulser) printf("pulser going (dataway start) \n");
    pcount--;
  }
E 3

I 3
  return;
E 3
}

I 3
void
stop_lpulser()
{
  ext_start_off();
  if(debug_pulser) printf("start on \n");
E 3

D 3
int  getstatus() {
E 3
I 3
  return;
}
E 3

D 3
int bit00=0 ;  /* 1st bit laser power on */
int bit01=0 ;  /* 2nd bit laser ready    */
int bit02=0 ;  /*          laser enabled */
int bit03=0 ;
int bit04=0 ;
int bit05=0 ;
int bit06=0 ;
int bit07=0 ;
int bit08=0 ;
int bit09=0 ;
int bit10=0 ;
int bit11=0 ;
E 3

D 3
struct vme_struct {
		short calsys_csr ;
		short calsys_vector ;
		short calsys_trig ;
		short calsys_out ;
		short calsys_in ;
	             } ;
	             
 struct vme_struct   *vme_io_ptr ;	             	
 
 /*  the address of the board is set by switches and is geven below */
          vme_io_ptr = (struct vme_struct *)0xFFFF0ED0 ;
E 3

D 3
 if (debug_vme) printf(" VME control status = %x \n",vme_io_ptr->calsys_csr );	
 if (debug_vme) printf(" VME interrupt vector = %x \n",vme_io_ptr->calsys_vector );	
 if (debug_vme) printf(" VMEtrigger data= %x \n",vme_io_ptr->calsys_trig );
 if (debug_vme) printf(" VME output port = %x \n",vme_io_ptr->calsys_out ); 
 if (debug_vme) printf(" VMEinput port  = %x \n",vme_io_ptr->calsys_in );
	
	vme_io_ptr->calsys_csr = 0 ;  /* set the csr for No interrrupt generation */   	 
   	 calsys_status =  (0xFFF) &  vme_io_ptr->calsys_in ;  /* mask off the upper bits  */
   	printf(" VME input port  = %d\n", calsys_status );
   	if( ((0x1) & calsys_status))   bit00=1 ;
   	if( ((0x2) & calsys_status))   bit01=1 ;
	if( ((0x4) & calsys_status))   bit02=1 ;
	if( ((0x8) & calsys_status))   bit03=1 ;
	if( ((0x10) & calsys_status))  bit04=1 ;
	if( ((0x20) & calsys_status))  bit05=1 ;
	if( ((0x40) & calsys_status))  bit06=1 ;
	if( ((0x80) & calsys_status))  bit07=1 ;
	if( ((0x100) & calsys_status)) bit08=1 ;
	if( ((0x200) & calsys_status)) bit09=1 ;
	if( ((0x400) & calsys_status)) bit10=1 ;
	if( ((0x800) & calsys_status)) bit11=1 ;
	
   	
   	 
	printf(" 12 bit port = %d%d%d%d%d%d%d%d%d%d%d%d \n\n ",
	bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
	
	soft_status_vme_reg=calsys_status;
	printf(" !!i!%d!s!%d%d%d%d%d%d%d%d%d%d%d%d!! ",calsys_status,
	bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);
  return  calsys_status ;
}
 
 
int send_test()
E 3
I 3
/*****************************************************************************/

int
getstatus()
E 3
{
I 3
  int bit00 = 0;  /* 1st bit laser power on */
  int bit01 = 0;  /* 2nd bit laser ready    */
  int bit02 = 0;  /*          laser enabled */
  int bit03 = 0;
  int bit04 = 0;
  int bit05 = 0;
  int bit06 = 0;
  int bit07 = 0;
  int bit08 = 0;
  int bit09 = 0;
  int bit10 = 0;
  int bit11 = 0;
E 3

I 3
  struct vme_struct
  {
    short calsys_csr;
    short calsys_vector;
    short calsys_trig;
    short calsys_out;
    short calsys_in;
  };
  struct vme_struct *vme_io_ptr;	             	
E 3

D 3
printf("!!I!3!s!kevin giovanetti!R!23.2!r!100!I!1234!S!  hello world!!");	
	
return 100;
E 3
I 3
  /* the address of the board is set by switches and is geven below */
D 6
  vme_io_ptr = (struct vme_struct *)0xFFFF0ED0 ;
E 6
I 6
  vme_io_ptr = (struct vme_struct *)0xFBFF0ED0 ;
E 6
  if(debug_vme)
  {
    printf(" VME control status = %x \n",vme_io_ptr->calsys_csr);
    printf(" VME interrupt vector = %x \n",vme_io_ptr->calsys_vector);
    printf(" VMEtrigger data= %x \n",vme_io_ptr->calsys_trig);
    printf(" VME output port = %x \n",vme_io_ptr->calsys_out);
    printf(" VMEinput port  = %x \n",vme_io_ptr->calsys_in);
  }

  vme_io_ptr->calsys_csr = 0; /* set the csr for No interrrupt generation */
  calsys_status = (0xFFF) & vme_io_ptr->calsys_in; /* mask off the upper bits*/
  printf(" VME input port  = %d\n", calsys_status);
  if( ((0x1) & calsys_status))   bit00 = 1;
  if( ((0x2) & calsys_status))   bit01 = 1;
  if( ((0x4) & calsys_status))   bit02 = 1;
  if( ((0x8) & calsys_status))   bit03 = 1;
  if( ((0x10) & calsys_status))  bit04 = 1;
  if( ((0x20) & calsys_status))  bit05 = 1;
  if( ((0x40) & calsys_status))  bit06 = 1;
  if( ((0x80) & calsys_status))  bit07 = 1;
  if( ((0x100) & calsys_status)) bit08 = 1;
  if( ((0x200) & calsys_status)) bit09 = 1;
  if( ((0x400) & calsys_status)) bit10 = 1;
  if( ((0x800) & calsys_status)) bit11 = 1;   	

  printf(" 12 bit port = %d%d%d%d%d%d%d%d%d%d%d%d \n\n ",
    bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);

  soft_status_vme_reg=calsys_status;
  printf(" !!i!%d!s!%d%d%d%d%d%d%d%d%d%d%d%d!! ",calsys_status,
    bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);

  return(calsys_status);
E 3
}

D 3
int test_pulser_data()
E 3
I 3
int
send_test()
E 3
{
D 3
int i=0;
   do 
   {
   printf(" pulser data %d %ld %ld \n",i, set_data[i],output_data[i]) ;
E 3
I 3
  printf("!!I!3!s!kevin giovanetti!R!23.2!r!100!I!1234!S!  hello world!!");
  return(100);
}

int
test_pulser_data()
{
  int i=0;

  do 
  {
    printf(" pulser data %d %ld %ld \n",i, set_data[i],output_data[i]) ;
E 3
    ++i;
D 3
   } while ( i< 1024 && set_data[i] < 16777215   );
   
return(0);
E 3
I 3
  } while(i< 1024 && set_data[i] < 16777215);

  return(0);
E 3
}

D 3
int make_pulser_data()
E 3
I 3
int
make_pulser_data()
E 3
{
D 3
/* mod 9/2/99
   the no_pulser data represents teh number of pieces fo data
E 3
I 3
  /* mod 9/2/99
   the no_pulser data represents the number of pieces fo data
E 3
   in either data stream (output or set).
   to generate a pulse stream you need an  "off" then "on". This re4quires
   two times and two register values.
   No of data = 2 * number of pulse + ?
   In addition the last pulse is off, on, on, stop.
   therefore you need two extra pieces of data
   No of data = 2 * number of pulse + 2.
D 3
	add end of seq and be of seq marks using 16 and 32 instead of 0
*/
   
 int counter1 = 0;
 int counter=0;
E 3
I 3
   add end of seq and be of seq marks using 16 and 32 instead of 0
  */
  int counter1 = 0;
  int counter=0;
E 3

D 3
 /* set pulse width to 35 ms */
 long pulsewidth = 3500;  
  
 int output1 = 0;
 int output2 = 7;
 int loop_number=0;
 long long1=0, long2=0, long3=0, long4=0;
 
 
 if(debug_pulser) printf(" making pulser data ");
 if(debug_pulser)printf(" period in 10usec units (10000 correspods to 10Hz)= %ld no of pusle= %ld \n",
                 period_10usec,numberOfPulses);       
E 3
I 3
  /* set pulse width to 35 ms */
  long pulsewidth = 3500;  
E 3

D 3
/* 1 Hz -> 100000, .01Hz 10000000, 25Hz 4000  */
 if ((period_10usec > 10000000) || (period_10usec < 4000))
      {
       printf("Frequency not between .01Hz and 25Hz\n");
       return(1);
      }
E 3
I 3
  int output1 = 0;
  int output2 = 7;
  int loop_number = 0;
  long long1 = 0, long2 = 0, long3 = 0, long4 = 0;
E 3

D 3
 if ((numberOfPulses < 1) || (numberOfPulses > 500))
      {
       printf("Number of pulses not from 1 to 500\n");
       return(1);
      }
E 3
I 3
  if(debug_pulser)
  {
    printf(" making pulser data ");
    printf(" period in 10usec units (10000 correspods to 10Hz)= %ld",
           period_10usec);
    printf(" no of pusle= %ld \n",numberOfPulses);       
  }
E 3

D 3
if (  (numberOfPulses*period_10usec) > 16700000)
      {
       printf("(number of pulses) / (frequency) must be less than 167\n");
       return(1);
      }
 
E 3
I 3
  /* 1 Hz -> 100000, .01Hz 10000000, 25Hz 4000  */
  if((period_10usec > 10000000) || (period_10usec < 4000))
  {
    printf("Frequency not between .01Hz and 25Hz\n");
    return(1);
  }
E 3

D 3
loop_number = 2*(numberOfPulses - 2) ; /* dont count oth and last */
if(debug_pulser)printf(" loop max=%d\n",loop_number);
while( counter <= loop_number)
      {
      long1 =     counter1    * period_10usec   ; 
      long2 = ((counter1 + 1) * period_10usec) - pulsewidth;
      set_data[counter]=long1;	             
      output_data[counter]=output1;
      counter++;
      set_data[counter]=long2;	             
      output_data[counter]=output2;
      counter++;
      counter1++;
   /*  
      if(debug_pulser)printf(" counter0,1= %d %d  n", counter, counter1);
      if(debug_pulser)printf("data= %d %ld %d %ld \n ", output1, long1, output2, long2);
   */
       }
        
 if(debug_pulser)printf( "out of loop counter1= %d counter= %d\n",counter1,counter);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter-1, set_data[counter-1],counter-1,output_data[counter-1]);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
E 3
I 3
  if((numberOfPulses < 1) || (numberOfPulses > 500))
  {
    printf("Number of pulses not from 1 to 500\n");
    return(1);
  }
E 3

D 3
 long1 =     counter1    * period_10usec   ; 
 long2 = ((counter1 + 1) * period_10usec) - pulsewidth;
 long3 = ((counter1 + 1) * period_10usec) - 1;
E 3
I 3
  if((numberOfPulses*period_10usec) > 16700000)
  {
    printf("(number of pulses) / (frequency) must be less than 167\n");
    return(1);
  }
E 3

D 3
 long4 = 16777215;
 if(debug_pulser) printf(" values= %ld  %ld  %ld  %ld  \n",long1,long2,long3,long4);
E 3
I 3
  loop_number = 2*(numberOfPulses - 2) ; /* dont count oth and last */
  if(debug_pulser) printf(" loop max=%d\n",loop_number);
  while( counter <= loop_number)
  {
    long1 =     counter1    * period_10usec   ; 
    long2 = ((counter1 + 1) * period_10usec) - pulsewidth;
    set_data[counter]=long1;	             
    output_data[counter]=output1;
    counter++;
    set_data[counter]=long2;	             
    output_data[counter]=output2;
    counter++;
    counter1++;
    /*  
    if(debug_pulser)printf(" counter0,1= %d %d  n", counter, counter1);
    if(debug_pulser)printf("data= %d %ld %d %ld \n ", output1, long1, output2, long2);
    */
  }
E 3

D 3
 set_data[counter]=long1;	             
 output_data[counter]=32; /* mark end of seqeunce */
 if(debug_pulser)printf(" counter0,1= %d %d  n", counter, counter1);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
E 3
I 3
  if(debug_pulser)
  {
    printf("out of loop counter1= %d counter= %d\n",counter1,counter);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter-1, set_data[counter-1],counter-1,output_data[counter-1]);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
E 3

D 3
 counter++;
 
 
 set_data[counter]=long2;	             
 output_data[counter]=output2;
E 3
I 3
  long1 =     counter1    * period_10usec;
  long2 = ((counter1 + 1) * period_10usec) - pulsewidth;
  long3 = ((counter1 + 1) * period_10usec) - 1;
E 3

D 3
 if(debug_pulser)printf(" counter0,1= %d %d  \n", counter, counter1);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
 counter++;
 
 set_data[counter]=long3;	             
 output_data[counter]=output2;
E 3
I 3
  long4 = 16777215;
  if(debug_pulser)
    printf(" values= %ld  %ld  %ld  %ld  \n",long1,long2,long3,long4);
E 3

D 3
 if(debug_pulser)printf(" counter0,1= %d %d  \n", counter, counter1);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
 counter++;
E 3
I 3
  set_data[counter]=long1;	             
  output_data[counter]=32; /* mark end of seqeunce */
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n",counter,counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;
E 3

D 3
 set_data[counter]=long4;	             
 output_data[counter]=output2;
 if(debug_pulser)printf(" counter0,1= %d %d  \n", counter, counter1);
 if(debug_pulser)printf("set_data[%d]= %ld , output_data[%d] =%ld \n ",counter, set_data[counter],counter,output_data[counter]);
E 3
I 3
  set_data[counter]=long2;	             
  output_data[counter]=output2;
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n", counter, counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;
E 3

D 3
 output_data[0]=16;  /* one pulse for start */
 	
 no_pulser_data=counter+1;
 
 
E 3
I 3
  set_data[counter]=long3;	             
  output_data[counter]=output2;
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n", counter, counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;
E 3

D 3
	 soft_status_pul_make=1; 
 printf(" generated %ld pieces (%ld pulses) of data with %ld * 10usec for period and %ld *10usec for width ",no_pulser_data,numberOfPulses,period_10usec, pulsewidth);	 
	 	 
	 return(0);
	 
E 3
I 3
  set_data[counter]=long4;	             
  output_data[counter]=output2;
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n", counter, counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  output_data[0]=16; /* one pulse for start */
  no_pulser_data=counter+1;

  soft_status_pul_make=1; 
  printf(" generated %ld pieces (%ld pulses) of data with %ld * 10usec",
         no_pulser_data,numberOfPulses,period_10usec);
  printf(" for period and %ld *10usec for width ",pulsewidth);	  	 

  return(0);	 
E 3
}
I 10

void kevin()
{

  int write_ret;
  char test_RDbuffer[201];

  printf(" intialize the laser port for filter******* \n");

  laser_motor_init();

  printf(" write to the port $1 \n");

  write_ret=write(LaserPort,"$1",2);
  taskDelay(100);

  printf(" write %d  bytes \n", write_ret);

   write_ret=read(LaserPort,test_RDbuffer,100);

 printf(" read %d  bytes \n", write_ret);
 printf(" data \n  %s   \n", test_RDbuffer);




  printf(" close the laser port for filter******* \n");
  close_ECport(0);

  return;
}

E 10
E 1
