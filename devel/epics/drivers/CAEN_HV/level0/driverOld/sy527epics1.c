
/* sy527epics1.c - EPICS driver support for CAEN SY527 HV mainframe */
/*                  (a la Lecroy)                                     */

#ifdef vxWorks

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include "sy527.h"
#include "sy527epics1.h"

#undef DEBUG
 
/* static structure to keep track of active channels 
static double properties[MAX_HVPS][NUM_CAEN_PROP];
*/

/* open communication with mainframe under logical number 'id' */
int
CAEN_HVstart(unsigned id, char *ip_address)
{
  return(sy527Start(id,ip_address));
}

/* close communication with mainframe under logical number 'id' */
int
CAEN_HVstop(unsigned id)
{
  return(sy527Stop(id));
}

/* returns 1 if mainframe is ON, otherwise returns 0 */
int
CAEN_GetHv(unsigned id)
{
  int active, onoff, alarm;

  sy527GetMainframeStatus(id, &active, &onoff, &alarm);

  return(onoff);
}

/* returns 1 if any channel is alarming, otherwise returns 0 */
int
CAEN_GetAlarm(unsigned id)
{
  int active, onoff, alarm;

  sy527GetMainframeStatus(id, &active, &onoff, &alarm);

  return(alarm);
}

/* */
int
CAEN_GetValidity(unsigned id)
{
  int active, onoff, alarm;

  sy527GetMainframeStatus(id, &active, &onoff, &alarm);

  return(active);
}

/* set HV on/off for whole mainframe */
int
CAEN_SetHV(unsigned id, unsigned char on_off)
{
  unsigned int u = on_off;

  sy527SetMainframeOnOff(id, u);

  return(0);
}

/* */
STATUS
CAEN_HVload(unsigned id, unsigned slot, unsigned channel,
            char *property, float value)
{
  if(!strncmp("MC",property,2))
  {
    printf("CAEN_HVload: request for MC\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("MV",property,2))
  {
    printf("CAEN_HVload: request for MV\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("DV",property,2))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for DV\n");
#endif
    sy527SetChannelDemandVoltage(id, slot, channel, value);
  }
  else if(!strncmp("RUP",property,3))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for RUP\n");
#endif
    sy527SetChannelRampUp(id, slot, channel, value);
  }
  else if(!strncmp("RDN",property,3))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for RDN\n");
#endif
    sy527SetChannelRampDown(id, slot, channel, value);
  }
  else if(!strncmp("TC",property,2))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for TC\n");
#endif
    sy527SetChannelMaxCurrent(id, slot, channel, value);
  }
  else if(!strncmp("CE",property,2))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for CE: id=%d sl=%d ch=%d val=%f\n",
      id,slot,channel,value);
#endif
    if(value > 0.5)
    {
      /* enable the channel */
      sy527SetChannelEnableDisable(id, slot, channel, 1);
#ifdef vxWorks
      ttsleep(200);
#else
      sleep(2);
#endif
      /* if mainframe is ON, turn channel ON */
	  {
        int active, onoff, alarm;
        sy527GetMainframeStatus(id, &active, &onoff, &alarm);
        if(onoff)
        {
#ifdef vxWorks
          ttsleep(200);
#else
          sleep(2);
#endif
          sy527SetChannelOnOff(id, slot, channel, 1);
        }
	  }
	}
    else
    {
      /* disable the channel */
      sy527SetChannelOnOff(id, slot, channel, 0);
#ifdef vxWorks
      ttsleep(200);
#else
      sleep(2);
#endif
      sy527SetChannelEnableDisable(id, slot, channel, 0);
#ifdef vxWorks
      ttsleep(200);
#else
      sleep(2);
#endif
	}
  }
  else if(!strncmp("ST",property,2))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for ST\n");
#endif
    ;
  }
  else if(!strncmp("MVDZ",property,4))
  {
    printf("CAEN_HVload: request for MVDZ\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("MCDZ",property,4))
  {
    printf("CAEN_HVload: request for MCDZ\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("HVL",property,3))
  {
#ifdef DEBUG
    printf("CAEN_HVload: request for HVL\n");
#endif
    sy527SetChannelMaxVoltage(id, slot, channel, value);
  }

  return(0);
}

/* */
STATUS
CAEN_GetProperty(unsigned id, unsigned slot, unsigned channel,
                 char *property, float *value)
{
  if(!strncmp("MC",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for MC\n");
#endif
    *value = sy527GetChannelMeasuredCurrent(id, slot, channel);
  }
  else if(!strncmp("MV",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for MV\n");
#endif
    *value = sy527GetChannelMeasuredVoltage(id, slot, channel);
  }
  else if(!strncmp("DV",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for DV\n");
#endif
    *value = sy527GetChannelDemandVoltage(id, slot, channel);
  }
  else if(!strncmp("RUP",property,3))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for RUP\n");
#endif
    *value = sy527GetChannelRampUp(id, slot, channel);
  }
  else if(!strncmp("RDN",property,3))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for RDN\n");
#endif
    *value = sy527GetChannelRampDown(id, slot, channel);
  }
  else if(!strncmp("TC",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for TC\n");
#endif
    *value = sy527GetChannelMaxCurrent(id, slot, channel);
  }
  else if(!strncmp("CE",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for CE\n");
#endif
    *value = (float) sy527GetChannelEnableDisable(id, slot, channel);
  }
  else if(!strncmp("ST",property,2))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for ST\n");
#endif
    *value = (float) sy527GetChannelStatus(id, slot, channel);
  }
  else if(!strncmp("MVDZ",property,4))
  {
    printf("CAEN_GetProperty: request for MVDZ\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("MCDZ",property,4))
  {
    printf("CAEN_GetProperty: request for MCDZ\n");
    printf("!!! NEVER CALL IT AGAIN !!!\n");
  }
  else if(!strncmp("HVL",property,3))
  {
#ifdef DEBUG
    printf("CAEN_GetProperty: request for HVL\n");
#endif
    *value = sy527GetChannelMaxVoltage(id, slot, channel);
  }

  return(0);
}

/* returns all values for one channel */
STATUS
CAEN_GetChannel(unsigned id, unsigned slot, unsigned channel,
                double *property, double *delta)
{
  /*  printf("CAEN_GetChannel reached\n"); */
  property[PROP_MC] = sy527GetChannelMeasuredCurrent(id, slot, channel);
  property[PROP_MV] = sy527GetChannelMeasuredVoltage(id, slot, channel);
  property[PROP_DV] = sy527GetChannelDemandVoltage(id, slot, channel);
  property[PROP_RUP] = sy527GetChannelRampUp(id, slot, channel);
  property[PROP_RDN] = sy527GetChannelRampDown(id, slot, channel);
  property[PROP_TC] = sy527GetChannelMaxCurrent(id, slot, channel);
  property[PROP_CE] = (float) sy527GetChannelEnableDisable(id, slot, channel);
  property[PROP_ST] = (float) sy527GetChannelStatus(id, slot, channel);
  property[PROP_MVDZ] = 0.0;
  property[PROP_MCDZ] = 0.0;
  property[PROP_HVL] = sy527GetChannelMaxVoltage(id, slot, channel);
  *delta =  fabs(property[PROP_MV] - property[PROP_DV]) ;
  return(0);
}

#else

void
sy527epics1_dummy()
{
  return;
}

#endif
