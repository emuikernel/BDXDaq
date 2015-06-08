#ifdef VXWORKS

/******************************************************************************
*
*  v851Lib.c  -  Driver library for Highland V851 VME Digital delay
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          March 2002
*
* Sergey Boyarinov: minor changes (3-sep-2008)
*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <logLib.h>
#include <taskLib.h>


/* Include definitions */

#define V851_BASE_ADDR   0xfbffd000
#define V851_MODULE_ID   0xfeee
#define V851_MODULE_TYPE 0x5943

#define V851_ACTION_MASK        0x8085
#define V851_ACTION_FEOD        0x0001
#define V851_ACTION_CALIB       0x0004
#define V851_ACTION_XFR         0x0080
#define V851_ACTION_FIRE        0x8000

#define V851_CONTROL_MASK       0xff00
#define V851_CONTROL_SLOPE      0x0100
#define V851_CONTROL_VTRIG      0x0200
#define V851_CONTROL_ITRIG      0x0400
#define V851_CONTROL_PRESCALE   0x0800
#define V851_CONTROL_LOAD       0x1000
#define V851_CONTROL_CCAL       0x2000
#define V851_CONTROL_EXTCLOCK   0x4000
#define V851_CONTROL_DISARM     0x8000

#define V851_WF1_ONESHOT        0x08
#define V851_WF1_MODE0          0x00
#define V851_WF1_MODE1          0x01
#define V851_WF1_MODE2          0x02
#define V851_WF1_MODE3          0x03
#define V851_WF1_MODE4          0x04
#define V851_WF1_MODE5          0x05
#define V851_WF1_MODE6          0x06
#define V851_WF1_MODE7          0x07
#define V851_WF2_ONESHOT        0x80
#define V851_WF2_MODE0          0x00
#define V851_WF2_MODE1          0x10
#define V851_WF2_MODE2          0x20
#define V851_WF2_MODE3          0x30
#define V851_WF2_MODE4          0x40
#define V851_WF2_MODE5          0x50
#define V851_WF2_MODE6          0x60
#define V851_WF2_MODE7          0x70
#define V851_WF3_ONESHOT        0x08
#define V851_WF3_MODE0          0x00
#define V851_WF3_MODE1          0x01
#define V851_WF3_MODE2          0x02
#define V851_WF3_MODE3          0x03
#define V851_WF3_MODE4          0x04
#define V851_WF3_MODE5          0x05
#define V851_WF3_MODE6          0x06
#define V851_WF3_MODE7          0x07
#define V851_WF4_ONESHOT        0x80
#define V851_WF4_MODE0          0x00
#define V851_WF4_MODE1          0x10
#define V851_WF4_MODE2          0x20
#define V851_WF4_MODE3          0x30
#define V851_WF4_MODE4          0x40
#define V851_WF4_MODE5          0x50
#define V851_WF4_MODE6          0x60
#define V851_WF4_MODE7          0x70
#define V851_WF5_ONESHOT        0x08
#define V851_WF5_MODE0          0x00
#define V851_WF5_MODE1          0x01
#define V851_WF5_MODE2          0x02
#define V851_WF5_MODE3          0x03
#define V851_WF5_MODE4          0x04
#define V851_WF5_MODE5          0x05
#define V851_WF5_MODE6          0x06
#define V851_WF5_MODE7          0x07
#define V851_WF6_ONESHOT        0x80
#define V851_WF6_MODE0          0x00
#define V851_WF6_MODE1          0x10
#define V851_WF6_MODE2          0x20
#define V851_WF6_MODE3          0x30
#define V851_WF6_MODE4          0x40
#define V851_WF6_MODE5          0x50
#define V851_WF6_MODE6          0x60
#define V851_WF6_MODE7          0x70

/* Vlow default = -0.8V */
#define V851_VLOW_DEFAULT       0x6600
/* Vhigh default = 0.0V */
#define V851_VHIGH_DEFAULT      0x0000


/* Global variables */
UINT16 v851ControlReg = 0;
int    v851VmeTrigEnable = 0;

/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);


struct v851_struct {
/*00*/ volatile unsigned short id;          /* manufacturer ID, always 0xFEEE=65262 */
/*02*/ volatile unsigned short type;        /* module type, always 0x5943=22851 */
/*04*/ volatile unsigned short status;      /* status register; bits 8 and 9 are used */
/*06*/ volatile unsigned short blank0;      /* unused */
/*08*/ volatile unsigned short voutLow;     /* channel and T0 low voltage level */
/*0A*/ volatile unsigned short voutHigh;    /* channel and T0 high voltage level */
/*0C*/ volatile unsigned short actions;     /* module action command bits */
/*0E*/ volatile unsigned short trigLevel;   /* external trigger level set */
/*10*/ volatile unsigned short control;     /* module control bits */
/*12*/ volatile unsigned short intRate;     /* internal trigger reprate divisor */
/*14*/ volatile unsigned short blank1;      /* unused */
/*16*/ volatile unsigned short blank2;      /* unused */
/*18*/ volatile unsigned short blank3;      /* unused */
/*1A*/ volatile unsigned short blank4;      /* unused */
/*1C*/ volatile unsigned short blank5;      /* unused */
/*1E*/ volatile unsigned short blank6;      /* unused */
/*20*/ volatile unsigned short delay1high;  /* delays for channels 1 */
/*22*/ volatile unsigned short delay1low;   /* delays for channels 1 */
/*24*/ volatile unsigned short delay2high;  /* delays for channels 2 */
/*26*/ volatile unsigned short delay2low;   /* delays for channels 2 */
/*28*/ volatile unsigned short delay3high;  /* delays for channels 3 */
/*2A*/ volatile unsigned short delay3low;   /* delays for channels 3 */
/*2C*/ volatile unsigned short delay4high;  /* delays for channels 4 */
/*2E*/ volatile unsigned short delay4low;   /* delays for channels 4 */
/*30*/ volatile unsigned short delay5high;  /* delays for channels 5 */
/*32*/ volatile unsigned short delay5low;   /* delays for channels 5 */
/*34*/ volatile unsigned short delay6high;  /* delays for channels 6 */
/*36*/ volatile unsigned short delay6low;   /* delays for channels 6 */
/*38*/ volatile unsigned short waveForm1_2; /* waveform programming, channels 1/2 */
/*3A*/ volatile unsigned short waveForm3_4; /* waveform programming, channels 3/4 */
/*3C*/ volatile unsigned short waveForm5_6; /* waveform programming, channels 5/6 */
};

volatile struct v851_struct *v851p[2];

/* Help Utility */
void
v851Help()
{
  printf(" Highland V851 6 channel Digital Delay Library\n");
  printf(" ---------------------------------------------\n");
  printf("  v851Init(address, id)               : Initialize V851 Access\n");
  printf("  v851SetMode(mode,option,id)         : Set operating mode\n");
  printf("  v851SetDelay(channel,delay,xfr,id)  : Set delays (nsec), xfr=1 (update)\n");  
  printf("  v851SetAll(d1,d2,d3,d4,d5,d6,id)    : Set all 6 delay values and update\n"); 
  printf("  v851SetVout(Vlow,Vhigh,id)          : Set output voltages (milliVolts)\n");
  printf("  v851SetTrigLevel(Vtrig,id)          : Set input trigger level (milliVolts)\n");
  printf("  v851Enable(id)                      : Enable digital delay\n");
  printf("  v851Disable(id)                     : Disable digital delay\n");
  printf("  v851UpdateDelay(id)                 : Implement new delay programming\n");
  printf("  v851Trig(id)                        : Trigger delay cycle from VME\n");

}



STATUS
v851Init(unsigned long addr, int id)
{
  int res;

  if (addr == 0)
  {
    /* Use default */
    v851p[id] = (struct v851_struct *)V851_BASE_ADDR;
  }
  else if ((addr>0)&&(addr<0xffff))
  {
    /* A16 Local address - find translation */
    res = sysBusToLocalAdrs(0x29,(char *)addr,(char **)&v851p[id]);
    if (res != 0) {
      printf("v851Init: ERROR in sysBusToLocalAdrs(0x29,0x%x,&laddr) \n",(int)addr);
      return(ERROR);
    }    
  }
  else
  {
    /* NO TRANSLATION */
    v851p[id] = (struct v851_struct *)addr;
  }

  if(v851p[id]->id != V851_MODULE_ID) {
    printf("v851Init: ERROR: Invalid Module ID (0x%x) at address 0x%x\n",
	   v851p[id]->id,(int)v851p[id]);
    v851p[id]=0;
    return(ERROR);
  }

  /* Initialize Control Register and readback variable */
  v851ControlReg  = V851_CONTROL_DISARM;
  v851p[id]->control = v851ControlReg;

  return(OK);
}


/**************************************/
/* module immediate actions functions */

void
v851UpdateDelay(int id)
{
  v851p[id]->actions = (V851_ACTION_XFR | V851_ACTION_FEOD);
}

void
v851Trig(int id)
{
  if(v851VmeTrigEnable)
  {
    v851p[id]->actions = V851_ACTION_FIRE;
  }
  else
  {
    logMsg("v851Trig: ERROR: VME Triggers not Enabled\n",0,0,0,0,0,0);
  }
}



UINT16
v851Disable(int id)
{

  v851ControlReg = V851_CONTROL_DISARM | v851ControlReg;
  v851p[id]->control = v851ControlReg;

  return(v851ControlReg);

}

UINT16
v851Enable(int id)
{

  v851ControlReg = ~V851_CONTROL_DISARM&v851ControlReg;
  v851p[id]->control = v851ControlReg ;

  return(v851ControlReg);

}


UINT16
v851SetMode(int mode, int option, int id)
{
  UINT16 rate=50000;

  if(v851p == NULL)
  {
    printf("v851SetMode: ERROR, v851 not initialized\n");
    return(ERROR);
  }
  
  if((mode==0)||(mode>3))
  {
    printf("Usage: v851SetMode  <mode>, [option], [id]\n");
    printf("    mode = 1  External Triggers\n");
    printf("      option = 0/1 negative/positive edge input\n");
    printf("\n");
    printf("    mode = 2  Internal Triggers (Pulser)\n");
    printf("      option = rate(Hz) (1Hz - 50000Hz)\n");
    printf("\n");
    printf("    mode = 3  VME Triggers\n");
    printf("      option flag ignored (use v851Trig(id) to trigger)\n");
    return(0);
  }

  switch(mode)
  {
  case 1:
    if(option!=0)
    {
      v851ControlReg = V851_CONTROL_DISARM;
    }
    else
    {
      v851ControlReg = V851_CONTROL_DISARM | V851_CONTROL_SLOPE;
    }
    v851p[id]->control = v851ControlReg;
    v851VmeTrigEnable = 0;
    break;
  case 2:
    if((option<=0)||(option>50000))
    {
      rate = 50000; /* Set to 1 Hz */
    }
    else
    {
      rate = (UINT16)(50000/option);
    }
    v851ControlReg = V851_CONTROL_DISARM | V851_CONTROL_ITRIG | 
                     V851_CONTROL_PRESCALE | V851_CONTROL_LOAD;
    v851p[id]->intRate = rate;
    v851p[id]->control = v851ControlReg;
    v851VmeTrigEnable = 0;
    break;
  case 3:
    v851ControlReg = V851_CONTROL_DISARM | V851_CONTROL_VTRIG;
    v851p[id]->control = v851ControlReg;
    v851VmeTrigEnable = 1;
  }

  return(v851ControlReg);
}


/**************************************************************
*
*  v851SetDelay()
*
*    Inputs:   chan  -  channel ID  1-6
*              delay -  delay in Nanoseconds
*
*    The DDG is calibrated for 39.0625 picoseconds/count
*    or 5/128 nsec/count
*    so to get the programming register value
*
*         rdelay = (delay*128)/5
*
*    (an rdelay value of 0.0 will disable that channel - ???)
*/

int
v851SetDelay(int chan, unsigned int delay, int xfr, int id)
{
  UINT32 rdelay=0;

  if(v851p == NULL)
  {
    printf("v851SetDelay: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  if(delay == 0)
  {
    printf("v851SetDelay: Error: delay must be > 0\n");
    return(ERROR);
  }
  else
  {
    rdelay = (UINT32)(((float)delay*128.0)/5.0); /* delay/39.0625ps */
  }

  switch(chan)
  {
  case 1:
    v851p[id]->delay1high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay1low = rdelay&0xFFFF;
	break;
  case 2:
    v851p[id]->delay2high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay2low = rdelay&0xFFFF;
	break;
  case 3:
    v851p[id]->delay3high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay3low = rdelay&0xFFFF;
	break;
  case 4:
    v851p[id]->delay4high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay4low = rdelay&0xFFFF;
	break;
  case 5:
    v851p[id]->delay5high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay5low = rdelay&0xFFFF;
	break;
  case 6:
    v851p[id]->delay6high = (rdelay>>16)&0xFFFF;
    v851p[id]->delay6low = rdelay&0xFFFF;
	break;
  default:
    printf("v851SetDelay ERROR: bad channelnumber=%d\n",chan);
  }

  printf("v851SetDelay: delay=%d, rdelay=%d(0x%08x), registers: high=0x%04x low=0x%04x\n",
		 delay,rdelay,rdelay,(rdelay>>16)&0xFFFF,rdelay&0xFFFF);

  /* if transfer flag set then update the delay */
  if(xfr) v851UpdateDelay(id);  

  return(0);
}

void
v851SetAll(UINT32 d1,UINT32 d2, UINT32 d3, UINT32 d4, UINT32 d5, UINT32 d6, int id)
{
  if(d1)
    v851SetDelay(1,d1,0,id);
  if(d2)
    v851SetDelay(2,d2,0,id);
  if(d3)
    v851SetDelay(3,d3,0,id);
  if(d4)
    v851SetDelay(4,d4,0,id);
  if(d5)
    v851SetDelay(5,d5,0,id);
  if(d6)
    v851SetDelay(6,d6,0,id);

  v851UpdateDelay(id);   
}



STATUS
v851SetRate(int rate, int id)
{
  UINT16 rval;

  if(v851p == NULL)
  {
    printf("v851SetRate: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  if((rate<1)||(rate>50000))
  {
    printf("v851SetRate: ERROR: 1 <= rate <= 50000 Hz\n");
    return(ERROR);
  }
  else
  {
    rval = (UINT16)(50000/rate);
  }

  v851p[id]->intRate = rval;

  return(OK);
}



STATUS
v851SetVout(short lo, short hi, int id)
{
  UINT16 rlo=0, rhi=0;

  if(v851p == NULL)
  {
    printf("v851SetVout: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  /* Voltage levels should be passed in millivolts

     so  Vlow  is negitive    -2000<Vlow <0
    and  Vhigh is positive     0<Vhigh<5000
   */

  if((lo<-2000)||(lo>0))
  {
    printf("Vlow (%d millivolts) out of range (-2000<Vlow<0)\n",lo);
    return(ERROR);
  }

  if((hi<0)||(hi>5000))
  {
    printf("Vhigh (%d millivolts) out of range (0>Vhigh>5000\n",hi);
    return(ERROR);
  }

  rlo = ((UINT16)((float)(-lo)*32.64)) & 0xFF00;
  rhi = ((UINT16)((float)(hi)*13.056)) & 0xFF00;
  printf("v851SetVout: Vmin/Vmax=%d/%d -> rlo=%d(0x%04x) rhi=%d(0x%04x)\n",
    lo,hi,rlo,rlo,rhi,rhi);

  v851p[id]->voutHigh = rhi;
  v851p[id]->voutLow  = rlo;

  return(OK);
}


/* external trigger level */

STATUS
v851SetTrigLevel(short level, int id)
{
  UINT16 rlevel;

  if(v851p == NULL)
  {
    printf("v851SetTrigLevel: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  /* Voltage levels should be passed in millivolts

     so  level    -2500< level <2500
   */

  if((level<-2500)||(level>2500))
  {
    printf("Trigger level (%d millivolts) out of range (-2500<level<2500)\n",level);
    return(ERROR);
  }

  if(level == 0)
  {
    rlevel = 0x8000;
  }
  else
  {
    rlevel = ((UINT16)((float)(2500-level)*13.107)) & 0xFF00;
  }

  v851p[id]->trigLevel = rlevel;

  return(OK);
}

STATUS
v851SetOneShot(int id)
{
  if(v851p == NULL)
  {
    printf("v851SetOneShot: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  /* set all outputs to oneshot (pulsed) mode */

  v851p[id]->waveForm1_2 = V851_WF1_ONESHOT | V851_WF1_MODE0 |
                           V851_WF2_ONESHOT | V851_WF2_MODE0;
  v851p[id]->waveForm3_4 = V851_WF3_ONESHOT | V851_WF3_MODE0 |
                           V851_WF4_ONESHOT | V851_WF4_MODE0;
  v851p[id]->waveForm5_6 = V851_WF5_ONESHOT | V851_WF5_MODE0 |
                           V851_WF6_ONESHOT | V851_WF6_MODE0;

  return(OK);
}


/* always one shot mode !!! */

STATUS
v851SetWaveformLogic(int mode, int id)
{
  if(v851p == NULL)
  {
    printf("v851SetWaveformLogic: ERROR, v851 not initialized\n");
    return(ERROR);
  }

  if(mode<0||mode>7)
  {
    printf("v851SetWaveformLogic: ERROR, mode=%d, must be 0<=mode<=7\n",mode);
    return(ERROR);
  }

  /* set all outputs to oneshot (pulsed) mode */

  v851p[id]->waveForm1_2 = V851_WF1_ONESHOT |  mode |
                           V851_WF2_ONESHOT | (mode<<4);
  v851p[id]->waveForm3_4 = V851_WF3_ONESHOT |  mode |
                           V851_WF4_ONESHOT | (mode<<4);
  v851p[id]->waveForm5_6 = V851_WF5_ONESHOT |  mode |
                           V851_WF6_ONESHOT | (mode<<4);

  return(OK);
}


void
v851ProgExt(int id)
{

/* Setup for external Triggers
             negative Slope
	     disable Delay
*/
  v851SetMode(1,0,id);

/* Set voltage output levels to NIM Complement */

  v851SetVout(0,800,id);

/* Set Trigger Input level to trigger on NIM */

  v851SetTrigLevel(-300,id);

/* Set Waveforms to One shot mode */

  v851SetOneShot(id);

/* Program Delays for each channel */

  v851SetDelay(1,200,0,id);
  v851SetDelay(2,300,0,id);
  v851SetDelay(3,500,0,id);
  v851SetDelay(4,700,0,id);
  v851SetDelay(5,900,0,id);
  v851SetDelay(6,1000,0,id);
  

/* Intiate module programming */

  v851UpdateDelay(id);

/* Rearm */

  v851Enable(id);


}


/*
v851Init(0xfbffd000,0)
v851ProgPulser(1000,0)
*/

void
v851ProgPulser(int rate, int id)
{

  if(rate<=0)
  {
    printf("ERROR: Specify rate (1 - 50000Hz)\n");
    return;
  }


/* Setup for internal Triggers
             negative Slope
	     disable Delay
*/
  v851SetMode(2,rate,id); /*2 means internal trigger, not 'mode' from manual*/

/* Set voltage output levels to NIM Complement */

  v851SetVout(-1500,0,id); /*Vmin,Vmax -m millivolts*/


/* Set Trigger Input level to trigger on NIM */

  v851SetTrigLevel(-300,id);

/* Set Waveforms to One shot mode */

  /*v851SetOneShot(id);*/

  /* set waveform mode to 2: width=delay_ch2-delay_ch1 */
  v851SetWaveformLogic(2,0); /* first arg - waveform mode (0-7) */

/* Program Delays for each channel */

  v851SetDelay(1,79,0,id);
  v851SetDelay(2,129,0,id);
  v851SetDelay(3,79,0,id);
  v851SetDelay(4,129,0,id);
  v851SetDelay(5,79,0,id);
  v851SetDelay(6,129,0,id);
  

/* Intiate module programming */

  v851UpdateDelay(id);

/* Rearm */

  v851Enable(id);

}





/*
extern int  c775SetFSR(int id, UINT16 fsr);
extern void c775Clear(int id);
*/

void
v851ProgExe(int chan, int count, int inc)
{

  int ii, jj, fsr=100;
  

  v851Disable(0);
  v851SetMode(3,0,0);

  /*  
  fsr = c775SetFSR(0,0);
  c775Clear(0);
  */

  if(inc<=0) 
  inc = fsr/10;

  for(ii=1;ii<=10;ii++) {
    v851SetDelay(chan,(ii*inc),1,0);
    v851Enable(0);
    for(jj=0;jj<count;jj++) {
      taskDelay(1);
      v851Trig(0);
    }
    v851Disable(0);
  }

  printf("v851ProgExe: Done\n");

}

#else

int
v851_dummy()
{
  exit(0);
}

#endif











