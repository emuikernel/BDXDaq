h49330
s 00002/00002/00044
d D 1.5 00/11/20 16:30:17 wolin 6 5
c put needs address, not value
e
s 00016/00001/00030
d D 1.4 00/11/20 14:07:21 wolin 5 4
c Need different versions for double,long
e
s 00001/00001/00030
d D 1.3 00/11/20 13:19:32 wolin 4 3
c const
e
s 00001/00001/00030
d D 1.2 00/11/20 13:15:39 wolin 3 1
c Minor
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/s/put_epics_ca.c
e
s 00031/00000/00000
d D 1.1 00/11/20 13:14:07 wolin 1 0
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

  put_epics_ca

  c callable function puts epics value via channel access

D 3
  ejw, 24-oct-00
E 3
I 3
  ejw, 20-nov-00
E 3

*/


#include <tsDefs.h>
#include <cadef.h>
#include <ezca.h>


/*--------------------------------------------------------------------------*/


D 4
void put_epics_ca(char *channel, char *attrib, double d) {
E 4
I 4
D 5
void put_epics_ca(const char *channel, const char *attrib, double d) {
E 5
I 5
void put_epics_ca_d(const char *channel, const char *attrib, double d) {
E 5
E 4

  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
D 6
  ezcaPut(temp,ezcaDouble,1,(void*)d);
E 6
I 6
  ezcaPut(temp,ezcaDouble,1,(void*)&d);
E 6
  
  return;
}


/*--------------------------------------------------------------------------*/
I 5


void put_epics_ca_l(const char *channel, const char *attrib, long l) {

  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
D 6
  ezcaPut(temp,ezcaLong,1,(void*)l);
E 6
I 6
  ezcaPut(temp,ezcaLong,1,(void*)&l);
E 6
  
  return;
}


/*--------------------------------------------------------------------------*/

E 5
E 1
