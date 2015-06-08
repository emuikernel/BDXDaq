h29795
s 00004/00004/00118
d D 1.3 03/12/16 22:19:44 boiarino 4 3
c .
e
s 00008/00002/00114
d D 1.2 03/04/17 16:56:02 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:12:19 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/uthinit.c
e
s 00116/00000/00000
d D 1.1 01/11/19 19:12:18 boiarino 1 0
c date and time created 01/11/19 19:12:18 by boiarino
e
u
U
f e 0
t
T
I 1

/* PAW/HBOOK initialization */

#include <stdio.h>
I 3
#include <stdlib.h>

E 3
#include "dclib.h"
I 3
#include "uthbook.h"
E 3
#include "sdakeys.h"
SDAkeys sdakeys_;

D 4
#define NWPAW  2000000
E 4
I 4
#define NWPAW  3000000
E 4

typedef struct pawcommon
{
  float paw[NWPAW];
} PAWcommon;

PAWcommon pawc_; /* COMMON/PAWC/PAW(NWPAW) */

static int initialized = 0;

void
uthinit_()
{
  uthinit();
}

void
uthinit()
{
  int itmp;

  /* HBOOK Initialization */

  itmp = NWPAW;
  hlimit_(&itmp);

  return;
}



void
dch2hbook_()
{
  dch2hbook();
}

void
dch2hbook()
{
  int i, j, k, l, itmp, ids[1000], nid, nbinx, nbiny;
  float x, y, dx, dy, xmin, xmax, ymin, ymax, content[10000];
  int titlelen, tmp, tmp1, tmp2, istat, icycle, RunNo;
  char title[1000];
  char NameHis[9];
  UThistf *histf;

printf("11111\n");fflush(stdout);
  uthgetlocal(&histf);
printf("22222\n");fflush(stdout);
printf("&histf[0].nbinx=0x%08x\n",&histf[0].nbinx);
printf("33333\n");fflush(stdout);

  if(!initialized) uthinit();

  uthidall(histf, ids, &nid);
  for(j=0; j<nid; j++)
  {
    titlelen = 999;
    uthgive(histf, ids[j], title, &nbinx, &xmin, &xmax,
                           &nbiny, &ymin, &ymax, &titlelen);
    if(nbiny == 0) /* 1-dim */
    {
      uthunpak(histf, ids[j], content, " ", 0);
      dx = (xmax-xmin)/((float)nbinx);
      HBOOK1(ids[j], title, nbinx, xmin, xmax, 0.);
      for(i=0; i<nbinx; i++)
      {
        x = xmin + (i+0.5)*dx;
        for(k=0; k<(int)content[i]; k++) HFILL(ids[j],x,0.,1.);
      }
    }
    else
    {
I 3
D 4
	  /* following piece failed on uthij() for 64bit *//*
E 4
I 4
	  /* following piece failed on uthij() for 64bit */
E 4
E 3
      dx = (xmax-xmin)/((float)nbinx);
      dy = (ymax-ymin)/((float)nbiny);
      HBOOK2(ids[j], title, nbinx, xmin, xmax, nbiny, ymin, ymax, 0.);
      for(i=0; i<nbinx; i++)
      {
        x = xmin + (i+0.5)*dx;
        for(k=0; k<nbiny; k++)
        {
          y = ymin + (k+0.5)*dy;
I 3
D 4
		  printf("-> 0x%08x %d[%d] %d %d\n",histf, ids[j], j, i, k);
E 4
I 4
          /*printf("-> 0x%08x %d[%d] %d %d\n",histf, ids[j], j, i, k);*/
E 4
E 3
          itmp = (int)uthij(histf, ids[j], i, k);
          for(l=0; l<itmp; l++)
          {
            HFILL(ids[j],x,y,1.);
          }
        }
D 3
      }
E 3
I 3
D 4
      }*/
E 4
I 4
      }
E 4
	  /* end of 64bit trouble */
E 3
    }
  }

D 3
  /* Open, Write and Close the HBOOK histogram file */
E 3
I 3
  printf("uthinit: Open, Write and Close the HBOOK histogram file\n");
E 3
  RunNo = sdakeys_.ltrig[0];
  sprintf(NameHis,"r%04d.his",RunNo);
  tmp = 0;
  tmp1 = 53;
  tmp2 = 1024;
  HROPEN(&tmp1,"RHIS",NameHis,"N",&tmp2,&istat,4,strlen(NameHis),1);
  HROUT(&tmp,&icycle,"T",1);
  HREND("RHIS",4);

  return;
}


E 1
