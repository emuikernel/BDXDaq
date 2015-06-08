h03471
s 00001/00001/01280
d D 1.2 05/02/25 14:43:00 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 04/08/09 16:30:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/calsys_commands_march2003.c
e
s 01281/00000/00000
d D 1.1 04/08/09 16:30:37 sergpozd 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
/*
  calsys_commands.c - code for the CLAS Calorimeter Calibration System
D 3
                      [to be run on CAMAC3]
E 3
I 3
                      to be run on camac1 (br=0, if branch is essential)
E 3

 * camac register
      each command sets and/or resets bits in the output register 
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
 	
 * rs232 port to the stepper motors
      sends and receives ascii strings through the rs232 port to link master
 	laser_motor_init
	cal_motor_init
 	send_mess
 	read_mess2
 	laser_motor
	cal_motor
 	semix_test
 	lpos_open()  move the motor
 	lpos_A()
 	lpos_B()
 * camac pulser
      downloads a pulse sequence and starts pulser debug_pulser=1 for debuging
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
	
 */

  
#include <vxWorks.h>
#include <ioLib.h>
#include <tyLib.h>
#include <ctype.h>
#include <selectLib.h>
#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include "laser.h"

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

#define INIT_ERROR   1 /* semix Initialization Error */
#define SUCCESS      0
#define WRITE_ERROR  1
#define READ_ERROR 	 1

/*
 *  Global Debug Flags 
 */

int DEBUG1 = 0;                /* Debug flag for dynaPowerLib.c */
int ReadbackTimeoutCount = 16; /* 1 second */
int readDelay = 6;		   /* 1/12 of a second */
int move_delay = 2000;		   /* 1/12 of a second */
int pulser_interim = 3000;
char tbuf[1024];		   /* Test Buffer */
int pcount=1000;
int go_pulse_on=1;		/*set to 1 for recycle 0 for stop */


/*
 *  Init Flags 
 */

int CalPort = 0;
int LaserPort = 0;                   /* Data Acquistion File Descriptors */
int calInitialized = 0;
int laserInitialized = 0;            /* Global Flag Used for Epics call */
char semixbuf[MAXLEN]={"this is the semix buffer"}	;	
    
/*
 *  Used for laser and pulser commands
 */
 
extern void cdreg(int *a, int b, int c, int n, int aa);
extern void cfsa(int f, int e, int *d, int *q);
extern void ctci(int aa, int *l);
extern void ccci(int aaa, int ll);
extern void ctsta(int *s);

int sgas=0;
int spul=0;
int slpower=0;
    
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

int stepper_off_bit  = 0x200 ;  /* bit 10 pulse using delay */
int stepper_on_bit   = 0x100 ;  /* bit  9 pulse using delay */
int lpower_onoff_bit   = 0x80  ;  /* bit  8 level  */
int laser_off_bit    = 0x20  ;  /* bit  6 pulse using delay */
int laser_on_bit     = 0x10  ;  /* bit  5 pulse using delay */
int pulser_on_bit    = 0x8   ;  /* bit  4 level 1 line only */
int gas_onoff_bit       = 0x4   ;  /* bit  3 level 1 line only */

int p_branch = 0;
int p_crate = 0;
int p_slot = 12;
int p_address0 = 0;
int p_address1 = 1;
int p_address2 = 2;

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

/* status register for system */
int calsys_status = 0;
    	
/* 36 provides 380ms pulse, works for laser enable-disable */
int pulseDelay = 36;	
    			    
/* Green Spring stuff */
#ifdef GREEN_SPRING

char CalPortName[] = {"/tyIP/9"};   
char LaserPortName[] = {"/tyIP/11"}; /* Serial Port Device Names */

/*---------------------------------------------------------------------------*/
/*  opens serial port to SEMIX LinkMaster RC-002, sets boud rate, flushes
 *  I/O pot
 */
int
cal_motor_init()
{	
  /* Open Serial Port */
  if((CalPort = open(CalPortName, O_RDWR, 0777)) == ERROR)
  {
    printf("cal_motor_init(): Could not open %s\n",CalPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Set Baud Rate */
  if(ioctl(CalPort, FIOBAUDRATE, 9600) == ERROR)
  {
    printf("cal_motor_init(): Could not set baud rate\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Set Raw Mode */
  if(ioctl(CalPort, FIOSETOPTIONS, OPT_RAW) == ERROR)
  {
    printf("cal_motor_init(): Could not set raw mode\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Flush I/O Port */
  if(ioctl(CalPort, FIOFLUSH, 0) == ERROR)
  {
    printf("cal_motor_init(): Could not Flush I/O Buffer\n");
    close(CalPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  calInitialized = 1; /* Set semix Initialization Flag */
  soft_status_c_motor = 1;
  printf("!!I!0!!\n");

  return(SUCCESS);
}

/*---------------------------------------------------------------------------*/
/*  opens serial port to SEMIX LinkMaster RC-002, sets boud rate, flushes
 *  I/O pot
 */
int
laser_motor_init()
{
  /* Open Serial Port */
  if((LaserPort = open(LaserPortName, O_RDWR, 0777)) == ERROR)
  {
    printf("laser_motor_init(): Could not open %s \n\n",LaserPortName);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Set Baud Rate */
  if(ioctl(LaserPort, FIOBAUDRATE, 9600) == ERROR)
  {
    printf("laser_motor_init(): Could not set baud rate\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Set Raw Mode */
  if(ioctl(LaserPort, FIOSETOPTIONS, OPT_RAW) == ERROR)
  {
    printf("laser_motor_init(): Could not set raw mode\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  /* Flush I/O Port */
  if(ioctl(LaserPort, FIOFLUSH, 0) == ERROR)
  {
    printf("laser_motor_init(): Could not Flush I/O Buffer\n");
    close(LaserPort);
    printf("!!I!1!!\n");
    return(INIT_ERROR);
  }

  laserInitialized = 1; /* Set semix Initialization Flag */
  soft_status_l_motor = 1;
  printf("!!I!0!!\n");

  return(SUCCESS);
}

#else

char CalPortName[] = {"/tty6016/5"};   
char LaserPortName[] = {"/tty6016/6"}; /* Serial Port Device Names */
int
cal_motor_init()
{
 printf("TO DO !!!\n");	
}
int
laser_motor_init()
{
 printf("TO DO !!!\n");
}
#endif


/*---------------------------------------------------------------------------*/
/*  sends argument to the RS232 Port
 *  returns error status (defined in SUCCESS and WRITE_ERROR)
 */
int
send_mess(char *raw, int whichPort)
{
  char output[140];
  int msgSize = 0;   /* Number of bytes to write */
  int bytesSent = 0; /* Number of bytes written */
  int SerialPort;          
  int i=0;

  if(whichPort) SerialPort = CalPort;
  else          SerialPort = LaserPort; 

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
#ifdef DEBUG
  printf("\nsend_mess(): Message Size: <%d> Actual Sent <%d>\n",
         msgSize,bytesSent);
#endif
  if(bytesSent != msgSize)
  {
#ifdef DEBUG
    printf("\nsend_mess(): Message Size: <%d> != Actual Sent <%d>\n",
           msgSize,bytesSent);
#endif
    return(WRITE_ERROR);
  }

  return(SUCCESS);
}


/*---------------------------------------------------------------------------*/
/*  modification of read_mess by A. Coleman, K. Beard, and J. Voshell 6/5/97 
 * 
 *  reads output from Semix LinkMaster RC-002 into an RS232 serial port
 *  into input[MAXLEN].
 * 
 *  transfers this string to clean[MAXLEN], looking for a > and a CR
 *
 *  outputs the following to standard output:
 *
 *  !!I!n!S!clean!!CR  ...  where n is a status word depending on the contents 
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
int
read_mess(int whichPort)
{
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

  if(whichPort) SerialPort = CalPort;
  else SerialPort = LaserPort;   

  /*  
   *  Waits, then finds number of bytes waiting in buffer (these are the 
   *  unread counted in NumUnRead ). Puts ith character into input[i],
   *  increments i and continues in this fashion until the maximum length
   *  is reached, a carriage return as defined by CR is reached, or te
   *  count exceeds the amount defined in ReadbackTimeoutCount
   */
  do
  {
    taskDelay(readDelay);
    ioctl(SerialPort, FIONREAD, (int)&NumUnRead);
    if((NumUnRead + i) > MAXLEN) break;
    if(NumUnRead != 0)
    {
      read(SerialPort, &input[i], NumUnRead);
      i = i + NumUnRead; 
    }
    Count++;
    j = (i > 0) ? i : 0; 
  } while( (input[j] != CR) && (Count < ReadbackTimeoutCount) );

  /* Pastes a "null" on the end of the input as an End-Of-String */
  input[j] = EOS;

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

  /* What kind of response did the RC-002 send us ? */
  if(clean[0]==GTsym && clean[1]==EOS)  /* only a ">"                */
    status= OK_GTonly;
  else if (!fndCR &&  clean[0]==GTsym)  /* ">" with no <CR> anywhere */
    status= LONGread;
  else if ( clean[0]==GTsym )           /* ">" with a <CR> somewhere */
    status= OKstring;
  else 
    status= BADstring;                  /* doesn't start with a ">"  */

  /* Print contents of clean */
  printf("!!I!%d!S!%s!!\n",status,clean);

  return(status);                       /* why not?, not really used */
}

void
cal_motor(char *mss)
{
  send_mess(mss,1);
  read_mess(1);
}

void
laser_motor(char *mss)
{
  send_mess(mss,0);
  read_mess(0);
}

void
lpos_open()
{
  laser_motor("$1");
  printf("\n");
  laser_motor("$18");
  printf("\n");
  taskDelay(move_delay);
  laser_motor("$19");
  printf("\n");	
  laser_motor("$1CL");	
}

void
lpos_A()
{
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
}

void
lpos_B()
{
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
}

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
}

/*
 program to set bits in camac reg

        12 11 10 9   |   8 7 6 5 |   4 3 2 1
        1   0  0 0          0 0 0 0       0 0 0 0   == bit 12 0n == 800 in hex
        0   0  1 0          0 0 0 0       0 0 0 0   == bit 10 on == 200 in hex
        etc... 

 camac reg changed so that all are levels
 ( here we call the first bit 1 , not 0)
             bit 12 == level == Not used ==  			0x800
             bit 11 == level == Not used == 			0x400
             bit 10 == pulse == step pwr on  == 			0x200
             bit 09 == pulse == step pwr off == 			0x100             
             bit 08 == level == las power onoff == ***		0x80             
             bit 07 == level == Not used==			0x40             
             bit 06 == pulse == laser disable (off)= ***		0x20
             bit 05 == pulse == laser enable (on)== ****		0x10
             bit 04 == level == pulser on (NA)  == ***			0x8
             bit 03 == level == nitrogen gas on == ***		0x4      
             bit 02 == level == Not used == 			0x2
             bit 01 == level == Not used  == 			0x1 (broken)
*/
int
inhibitclear()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int semixval = 100;

  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  inhib = 0;
  ccci((int)ext_a,!!inhib);
  if(debug_register) printf("inhibit clear for reg \n");
 
  return(semixval);
}

int
laser_enable()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

  data=laser_on_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" laser_enable write 1 q = %d \n",q);
 
  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" laser_enable write 2 q = %d \n",q);

  soft_status_laser = 1;

  return(semixval);
}

int
laser_disable() 
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x200;
  int q = 0;
  int semixval = 100;

  data=laser_off_bit+spul+sgas+slpower; 
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" laser_disable write 1 q = %d \n",q);

  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" laser_disable write 2 q = %d \n",q);

  soft_status_laser=0;

  return(semixval);
}

int
lpoweron()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0;
  int q = 0;
  int semixval = 100;

  slpower=lpower_onoff_bit;
  data=spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" poweron write 1 q = %d \n",q);

  soft_status_lpower=1;

  return(semixval);
}

int
lpoweroff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

  slpower=0;
  data=spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" poweroff write 1 q = %d \n",q);

  soft_status_lpower=0;

  return(semixval);
}

int
stepperon()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

  data=stepper_on_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" poweron write 1 q = %d \n",q);

  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q); 
  if(debug_register) printf(" poweron write 2 q = %d \n",q);

  soft_status_stepper=1;  

  return(semixval);
}

int
stepperoff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x800;
  int q = 0;
  int semixval = 100;

  data=stepper_off_bit+spul+sgas+slpower;   
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" poweroff write 1 q = %d \n",q);

  taskDelay(pulseDelay);   
  data=spul+sgas+slpower; 
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q); 
  if(debug_register) printf(" poweroff write 2 q = %d \n",q);

  soft_status_stepper=0; 

  return(semixval);
}

char
gason()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;
  char semixinfo = 63;

  sgas=gas_onoff_bit;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" gason write 1 q = %d \n",q); 

  soft_status_gas=1; 

  return(semixinfo);
}

void
hp_pulseron()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;

  spul=pulser_on_bit;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" pulser on write 1 q = %d \n",q);

  soft_status_hp_pul=1;

  return;
}

void
hp_pulseroff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data;
  int q = 0;

  spul=0;
  data=sgas+spul+slpower;
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" pulserroff write 1 q = %d \n",q);

  soft_status_hp_pul=0;

  return;
}

void
gasoff()
{
  int *ext_a = (int *)0;
  int inhib = 0;
  int data = 0x4;
  int q = 0;

  sgas=0;
  data=sgas+spul+slpower; 
  cdreg((int *)&ext_a,r_branch,r_crate,r_slot,r_address);
  ctci((int)ext_a,&inhib);
  if(debug_register) printf("  inhibit = %d \n",inhib);

  inhib = 0;
  ccci((int)ext_a,!!inhib);
  cfsa(16,(int)ext_a,(int *)&data,(int *)&q);
  if(debug_register) printf(" gasoff write 1 q = %d \n",q);

  soft_status_gas=0;

  return;
}




/********pulser c code*****************************************/

int
get_data()
{
  int counter = 0, foundstart = 0;
  FILE *file_handle;
  char *input_filename, *word;
  input_filename = "/home/clasrun/jmu/vme-camac/pulserload";

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

  return(0);
}

int
load_pulser()
{
  int *ext_a0 = (int *)0;
  int *ext_a1 = (int *)0;
  int data;
  int inhib = 0;
  int q = 0;
  int counter=0;

  /* after you execdute a get_data (see above) you want to load
  the data into the two buffers: 
  settings buffer=  24 bit times for transition
  output reg buffer= 12 bit value for output at each transistion
  */
   
  /* set up the pointer to the pulse module using standard commands
  cdreg, checka nd clear inhibit on crate if set */
     	
  if(debug_pulser)
    printf(" no of pairs of word to load= %ld\n", no_pulser_data);
  /* point to sub address 0 */
  cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);
  /* point to sub address 1 */
  cdreg((int *)&ext_a1,p_branch,p_crate,p_slot,p_address1);
  ctci((int)ext_a0,&inhib);
  if(debug_pulser) printf("  inhibit = %d\n",inhib);
  inhib = 0;
  ccci((int)ext_a0,!!inhib);

  /* set starting addrss of buffer to 0 F9A0. prepare for download register */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
  if(debug_pulser) printf(" clear output memory  q = %d\n",q);

  while(counter < no_pulser_data) 
  {   
    data=output_data[counter];
    /*if (debug_pulser) printf(" data word for output mem %d \n",data);*/
    cfsa(16,(int)ext_a0,(int *)&data,(int *)&q);
    /*if (debug_pulser) printf(" load word  q = %d \n",q);*/
    counter = counter + 1;
  }

  /* set starting addrss of buffer to 0 F9A0. prepare for download transitions */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /*  reset  address? */
  if(debug_pulser) printf(" clear set memory  q = %d\n",q);		

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

  /* need to reset the address to point to start of buffers  before a go */
  cfsa(9,(int)ext_a0,(int *)&data,(int *)&q); /* reset  address? */
  if(debug_pulser) printf(" clear set memory  q = %d\n",q);

  /* need to enable output word */
  cfsa(26,(int)ext_a1,(int *)&data,(int *)&q);

  soft_status_pul_ld = 1;

  return(0);
}	 	

void
ext_start_on()
{
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;

  /* point to sub adress 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);
  /* enable external start */
  cfsa(26,(int)ext_a2,(int *)&data,(int *)&q);

  soft_status_pul_ext=1;

  return;
}

void
get_pulser_status()
{
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;

  /* point to sub address 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address0);
  /* enable external start */
  cfsa(1,(int)ext_a2,(int *)&data,(int *)&q);

  printf("!!I!%d!s!q=%d!! \n",data,q);

  return;
}

void
ext_start_off()
{
  int *ext_a2 = (int *)0;
  int data;
  int q = 0;

  /* point to sub address 2 */
  cdreg((int *)&ext_a2,p_branch,p_crate,p_slot,p_address2);
  /* disable external start */
  cfsa(24,(int)ext_a2,(int *)&data,(int *)&q);

  soft_status_pul_ext=0;

  return;
}

void
dataway_start()
{
  int *ext_a0 = (int *)0;
  int data;
  int q = 0;

  /* point to sub address 0 */
  cdreg((int *)&ext_a0,p_branch,p_crate,p_slot,p_address0);
  /* start */
  cfsa(25,(int)ext_a0,(int *)&data,(int *)&q);

  return;
}

void
go_lpulser()
{
  get_data();
  if(debug_pulser) printf("get data done \n");
  load_pulser();
  if(debug_pulser) printf("load pulser done \n");
  ext_start_on();
  if(debug_pulser) printf("start on \n");
  dataway_start();
  if(debug_pulser) printf("pulser going (dataway start) \n");

  return;
}

void
go_pulse()
{
  get_data();
  if(debug_pulser) printf("get data done \n");
  load_pulser();
  if(debug_pulser) printf("load pulser done \n");
  dataway_start();
  if(debug_pulser) printf("pulser going (dataway start) \n");

  while(pcount&&go_pulse_on)
  {
    if(debug_pulser) printf("pulser while loop pcount= %d  go_pulse_on= %d \n",
      pcount, go_pulse_on);
    taskDelay(pulser_interim);
    dataway_start();
    if(debug_pulser) printf("pulser going (dataway start) \n");
    pcount--;
  }

  return;
}

void
stop_lpulser()
{
  ext_start_off();
  if(debug_pulser) printf("start on \n");

  return;
}



/*****************************************************************************/

int
getstatus()
{
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

  struct vme_struct
  {
    short calsys_csr;
    short calsys_vector;
    short calsys_trig;
    short calsys_out;
    short calsys_in;
  };
  struct vme_struct *vme_io_ptr;	             	

  /* the address of the board is set by switches and is geven below */
  vme_io_ptr = (struct vme_struct *)0xFBFF0ED0 ;
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
}

int
send_test()
{
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
    ++i;
  } while(i< 1024 && set_data[i] < 16777215);

  return(0);
}

int
make_pulser_data()
{
  /* mod 9/2/99
   the no_pulser data represents the number of pieces fo data
   in either data stream (output or set).
   to generate a pulse stream you need an  "off" then "on". This re4quires
   two times and two register values.
   No of data = 2 * number of pulse + ?
   In addition the last pulse is off, on, on, stop.
   therefore you need two extra pieces of data
   No of data = 2 * number of pulse + 2.
   add end of seq and be of seq marks using 16 and 32 instead of 0
  */
  int counter1 = 0;
  int counter=0;

  /* set pulse width to 35 ms */
  long pulsewidth = 3500;  

  int output1 = 0;
  int output2 = 7;
  int loop_number = 0;
  long long1 = 0, long2 = 0, long3 = 0, long4 = 0;

  if(debug_pulser)
  {
    printf(" making pulser data ");
    printf(" period in 10usec units (10000 correspods to 10Hz)= %ld",
           period_10usec);
    printf(" no of pusle= %ld \n",numberOfPulses);       
  }

  /* 1 Hz -> 100000, .01Hz 10000000, 25Hz 4000  */
  if((period_10usec > 10000000) || (period_10usec < 4000))
  {
    printf("Frequency not between .01Hz and 25Hz\n");
    return(1);
  }

  if((numberOfPulses < 1) || (numberOfPulses > 500))
  {
    printf("Number of pulses not from 1 to 500\n");
    return(1);
  }

  if((numberOfPulses*period_10usec) > 16700000)
  {
    printf("(number of pulses) / (frequency) must be less than 167\n");
    return(1);
  }

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

  if(debug_pulser)
  {
    printf("out of loop counter1= %d counter= %d\n",counter1,counter);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter-1, set_data[counter-1],counter-1,output_data[counter-1]);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }

  long1 =     counter1    * period_10usec;
  long2 = ((counter1 + 1) * period_10usec) - pulsewidth;
  long3 = ((counter1 + 1) * period_10usec) - 1;

  long4 = 16777215;
  if(debug_pulser)
    printf(" values= %ld  %ld  %ld  %ld  \n",long1,long2,long3,long4);

  set_data[counter]=long1;	             
  output_data[counter]=32; /* mark end of seqeunce */
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n",counter,counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;

  set_data[counter]=long2;	             
  output_data[counter]=output2;
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n", counter, counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;

  set_data[counter]=long3;	             
  output_data[counter]=output2;
  if(debug_pulser)
  {
    printf(" counter0,1= %d %d  \n", counter, counter1);
    printf("set_data[%d]= %ld , output_data[%d] =%ld \n",
           counter,set_data[counter],counter,output_data[counter]);
  }
  counter++;

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
}
E 1
