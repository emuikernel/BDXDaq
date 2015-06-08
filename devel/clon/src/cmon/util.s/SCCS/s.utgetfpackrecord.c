h36085
s 00004/00001/00098
d D 1.3 10/03/14 17:48:13 boiarino 4 3
c *** empty log message ***
e
s 00004/00000/00095
d D 1.2 03/04/17 16:56:13 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:12:19 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/utgetfpackrecord.c
e
s 00095/00000/00000
d D 1.1 01/11/19 19:12:18 boiarino 1 0
c date and time created 01/11/19 19:12:18 by boiarino
e
u
U
f e 0
t
T
I 1

/* get record 'recname' from FPACK file 'fname' */

#include <stdio.h>
#include <string.h>
I 3
#include <time.h>
E 3

I 3
#include "uttime.h"
#include "bosio.h"

E 3
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define MAXDATA 100000

int
utGetFpackRecord(char *fname, char *recname, int nwmax, float *rw)
{
  int i, ind, itmp, numra, numrb, icl, ierr, ncol, nrow, nch, nwd, nread, run,
      numdb, ikey, eof, status, eor, max_data, version, utime, *idata, iret;
  static float data[MAXDATA];
  char str[1000], utimes[25];
D 4
  char *kname="        ", *hname="        ", *format="               ";
E 4
I 4
  char kname[9], hname[9], format[16];
  kname[8] = '\0';
  hname[8] = '\0';
  format[15] = '\0';
E 4

  idata = (int *) data;
  max_data = MAXDATA;

  sprintf(str,"OPEN INPUT1 UNIT=55 FILE='%s' READ OLD BINARY",fname);
  status = fparm_(str,strlen(str));
  frname_("INPUT1",6);

  ikey=0;
  eof=0;
  iret=0;
  while(!eof)  /* loop over all input records */
  {
    for(i=0; i<strlen(kname); i++) kname[i] = ' ';
    frkey_(kname,&numra,&numrb,&icl,&ierr,8);
    if(ierr == -1) eof = 1;

    if(!eof)  /* dump each record key */
    {
      ikey++;
      if(ierr != 0) printf("sclib: error on frkey: %d\n",ierr);
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               ikey,kname,numra,numrb,icl);
      /* dump all headers in this record */
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
      eor = 0;
      while(!eor)
      {
        for(i=0; i<strlen(hname); i++) hname[i] = ' ';
        for(i=0; i<strlen(format); i++) format[i] = ' ';
        frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,8,15);

        if(ierr == -1) eor = 1;
        if(!eor)
        {
          if(ierr != 0)
          {
            printf("sclib: error on frhdr: %d\n",ierr);
          }
          else
          {
            frdat_(&nwd,data,&max_data);
            printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,nwd,format);

            if(!strncmp("CHDR",hname,4)) /* header */
            {
              version = idata[0];
              utime   = idata[1];
              getasciitime_(&utime,utimes,strlen(utimes));
            }
            else if(!strncmp(recname,hname,4))
            {
              iret=1;
              nch = MIN(nwd,nwmax) * 4;
              printf("utGetFpackRecord: copy %d bytes (%d %d)\n",nch,nwd,nwmax);
              memcpy((char *)rw, (char *)data, nch);
            }

          }
        }
      }
      printf("\n");
      printf(" Read 'sdacal.bos' file for %8.8s v.%02d created on %24.24s\n",
                                kname,version,utimes);
      printf("\n");
    }
  }

  /* close file */
  status = fparm_("CLOSE UNIT=55",13);

  return(iret);
}

E 1
