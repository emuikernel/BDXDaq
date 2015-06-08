/*

  put_epics_ca

  c callable function puts epics value via channel access

  ejw, 20-nov-00

*/


#include <tsDefs.h>
#include <cadef.h>
#include <ezca.h>


/*--------------------------------------------------------------------------*/


void put_epics_ca_d(const char *channel, const char *attrib, double d) {

  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
  ezcaPut(temp,ezcaDouble,1,(void*)&d);
  
  return;
}


/*--------------------------------------------------------------------------*/


void put_epics_ca_l(const char *channel, const char *attrib, long l) {

  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
  ezcaPut(temp,ezcaLong,1,(void*)&l);
  
  return;
}


/*--------------------------------------------------------------------------*/

