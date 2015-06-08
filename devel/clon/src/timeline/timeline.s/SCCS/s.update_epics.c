h54773
s 00000/00000/00000
d R 1.2 03/12/22 17:19:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/update_epics.c
e
s 00083/00000/00000
d D 1.1 03/12/22 17:19:43 boiarino 1 0
c date and time created 03/12/22 17:19:43 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 * caput the timeline values to EPICS database records....
 */

#include "tsDefs.h"
#include "cadef.h"
#include "ezca.h"

int update_epics(char name[100], char group[100], float value) {

  double dvalue;
  int l_name, l_group, l_rec;
  int ezca_error;
  char epics_rec[29];
  char s_name[28];
  char s_group[28];
  char colon[2];

  l_name = strlen(name);
  l_group = strlen(group);
  if (l_name + l_group < 26) {
    strcpy(epics_rec, group);
    strcat(epics_rec,":");
    strcat(epics_rec,name);
  } else {
    if (l_group > 18) {
      strncpy(s_group, group, 18);
      s_group[18] = '\0';
    } else {
      strcpy(s_group, group);
    }

    l_group = strlen(s_group);
    if (l_name > 27-l_group) {
      strncpy(s_name, name, 27-l_group);
      l_name = 27-l_group;
    } else {
      strcpy(s_name, name);
    }
    l_rec = l_name + l_group + 1;

    /*    
    printf("l_group:%i: l_name:%i: l_rec:%i:\n",l_group, l_name, l_rec);
    printf("s_group:%s: s_name:%s:\n",s_group, s_name);
    */
    strcpy(epics_rec, s_group);
    strcat(epics_rec,":");
    strcat(epics_rec,s_name);
    epics_rec[l_rec] = '\0';
  }

  ezca_error = ezcaPut(epics_rec, ezcaFloat, 1, &value);

  switch(ezca_error) {
  case(EZCA_INVALIDARG):
    printf("update_epics: INVALIDARG for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_FAILEDMALLOC):
    printf("update_epics: FAILED_MALLOC for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_CAFAILURE):
    printf("update_epics: CAFAILURE for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_UDFREQ):
    printf("update_epics: UDFREQ for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_NOTCONNECTED):
    printf("update_epics: NOTCONNECTED for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_NOTIMELYRESPONSE):
    printf("update_epics: NOTIMELYRESPONSE for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_INGROUP):
    printf("update_epics: INGROUP for epics rec: %s\n",epics_rec);
    break;
  case(EZCA_NOTINGROUP):
    printf("update_epics: NOTINGROUP for epics rec: %s\n",epics_rec);
    break;
  default:
    break;
  }
  return(1);
}
E 1
