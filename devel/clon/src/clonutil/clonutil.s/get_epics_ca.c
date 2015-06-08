/*

  get_epics_ca

  c callable function gets epics value via channel access

  ejw, 24-oct-00

*/


#include <tsDefs.h>
#include <cadef.h>
#include <ezca.h>


/*--------------------------------------------------------------------------*/


double get_epics_ca(const char *channel, const char *attrib) {

  double d;
  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
  ezcaGet(temp,ezcaDouble,1,&d);
  
  return d;

}


/*--------------------------------------------------------------------------*/
