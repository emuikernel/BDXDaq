h23062
s 00001/00001/00032
d D 1.3 00/11/20 13:19:27 wolin 4 3
c const
e
s 00000/00012/00033
d D 1.2 00/10/24 14:16:50 wolin 3 1
c Minor
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/s/get_epics_ca.c
e
s 00045/00000/00000
d D 1.1 00/10/24 14:13:56 wolin 1 0
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

  get_epics_ca

  c callable function gets epics value via channel access

  ejw, 24-oct-00

*/

D 3
/*
To link:

cc -g -o eztest eztest.c -I/home/epics/R3.13.0.beta11/base/include/ 
-I/home/epics/R3.13.0.beta11/extensions/include/ 
-L/home/epics/R3.13.0.beta11/extensions/lib/solaris/ -lezca 
-L/home/epics/R3.13.0.beta11/base/lib/solaris/ -lca -lcas -lCom 
-lnsl -ldl -lsocket -lm


 */

E 3

#include <tsDefs.h>
#include <cadef.h>
#include <ezca.h>


/*--------------------------------------------------------------------------*/


D 4
double get_epics_ca(char *channel, char *attrib) {
E 4
I 4
double get_epics_ca(const char *channel, const char *attrib) {
E 4

  double d;
  char temp[128]; 
  
  sprintf(temp,"%s.%s",channel,attrib);
  ezcaGet(temp,ezcaDouble,1,&d);
  
  return d;

}


/*--------------------------------------------------------------------------*/
E 1
