h61828
s 00001/00001/00171
d D 1.3 07/03/02 10:52:12 boiarino 4 3
c expand error 11 message
c 
e
s 00005/00004/00167
d D 1.2 03/04/17 16:46:37 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/boswrite.c
e
s 00171/00000/00000
d D 1.1 00/08/10 11:10:08 boiarino 1 0
c date and time created 00/08/10 11:10:08 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSWRITE. */
 
I 3
#include <stdio.h>
E 3
#include "bos.h"
#include "bosio.h"
#include "fpack.h"

/* #define COND_DEBUG  */
/* #define DEBUG */
#include "dbgpr.h"

static int writerecord = 0;

int
D 3
boswrite_(int *descriptor, int *jw, char *list, int lilen)
E 3
I 3
boswrite_(BOSIO *descriptor, int *jw, char *list, int lilen)
E 3
{
  char *li;
  int status;
  li = (char *) MALLOC(lilen+1);
  strncpy(li,list,lilen);
  li[lilen] = '\0';
D 3
  status = bosWrite(*descriptor, jw, li);
E 3
I 3
  status = bosWrite(descriptor, jw, li);
E 3
  FREE(li);
  return(status);
}

int
D 3
bosWrite(int descriptor, int *jw, char *list)
E 3
I 3
bosWrite(BOSIO *descriptor, int *jw, char *list)
E 3
{
  BOSptr b;
  BOSIOptr BIOstream;
  char *recname = "RUNEVENT";
  int *bufout;
  int *w, *ptr, *ev;
  int i,ii,ind,id,icount,jls,nls,ils,error,nch,nami;
  int dathead[70]; /* intermediate buffers */
  int nfopen; /* to make CHECK_OUTPUT_UNIT happy */

D 3
  BIOstream = (BOSIOptr)descriptor;	/* set pointers */
E 3
I 3
  BIOstream = descriptor; /* set pointers */
E 3
  bufout = BIOS_BUFP;
  b = (BOSptr)jw;
  w = jw - 1;

  error = 0;

  ALLOCATE_OUTPUT_BUFFER;

  if(*list == '0')	/* output last buffer */
  {
    DPR("bosWrite : Output last buffer\n");
    WRITE_BUF_TO_DISK(descriptor);
    START_NEW_PHYSREC;
    BIOS_NSEGM--;
    goto exit;
  }

  CHECK_OUTPUT_UNIT(descriptor);

/*********************
 * add record header *
 *********************/

  if(writerecord) /* take record header fro file descriptor */
  {
    PUT_RECORD_HEADER(descriptor,BIOstream->recname,BIOstream->runnum,
                          BIOstream->evntnum,BIOstream->trig);
  }
  else /* take record header from HEAD bank, name = "RUNEVENT" */
  {
    ind  = bosNlink(jw,"HEAD",0);
    PUT_RECORD_HEADER(descriptor,recname,w[ind+2],w[ind+3],w[ind+5]);
  }

/*****************
 * loop on banks *
 *****************/

  icount = 0;
  listn1(jw,list,&jls,&nls);
  ils = 0;

/* loop for names in list */
  while((nami=listn2(jw,list,&ils,&jls,&nls)) != 0)
  {
    if(w[nami] == 0) continue;
    ind = nami + 1;
    while((ind = w[ind-INXT]) != 0)	/* loop for banks with present name */
    {
      char fmt[61];

      if((id = w[b->idfmt + nami - b->s->nsyst]) == 0)
      {
        nch = 4; for(i=0; i<4; i++) fmt[i] = ' ';
      }
      else
      {
        if(w[id+3] != 0)
        {
          nch = 4;
          if(w[id+3] == 1) {fmt[0]= 'B'; fmt[1]= '3'; fmt[2]= '2'; fmt[3]= ' ';}
          if(w[id+3] == 2) {fmt[0]= 'B'; fmt[1]= '1'; fmt[2]= '6'; fmt[3]= ' ';}
          if(w[id+3] == 3) {fmt[0]= 'B'; fmt[1]= '0'; fmt[2]= '8'; fmt[3]= ' ';}
        }
        else
        {
          char *ch;
          ch = (char *)&w[id+4];
          nch = strlen(ch);
          bcopy((char *)ch, (char *)fmt, nch);
          fmt[nch++] = ' ';
          fmt[nch++] = ' ';
          fmt[nch] = ' ';
          nch = ((nch + 1)/4) * 4;
        }
      }

      icount = icount + NHW + w[ind];
 
      /* add data header */

      PUT_DATA_HEADER(descriptor,w[ind-INAM],w[ind-INR],w[ind-ICOL],w[ind-IROW],fmt,nch);

      /* add data */

      PUT_DATA(descriptor,w[ind],w,ind);

    }
  }

  if(bufout[IP+irCODE] != 0) bufout[IP+irCODE] = 3; /* if last record was "continued" mark it as the last */

  if(icount == 0)
  {
D 4
    printf("bosWrite: error 11\n");
E 4
I 4
    printf("bosWrite: error 11: there is no any banks to be written\n");
E 4
    error = -11;
    goto exit;
  }

/*   BIOS_OUTNUM++; */
  bufout[1] = KP;
  IP = KP;

exit:

  writerecord = 0;

  return(error);
}
 
/*
   call next if want to use ET record header; otherwise
   record name will be "RUNEVENT" and other info from HEAD bank
*/

void 
bosWriteRecord(int descriptor, char *recname, int runnum, int evntnum, int trig)
{
  BOSIOptr BIOstream;
  int i;

  BIOstream = (BOSIOptr)descriptor;

  for(i=0; i<8; i++) BIOstream->recname[i] = recname[i];
  BIOstream->runnum = runnum;
  BIOstream->evntnum = evntnum;
  BIOstream->trig = trig;

  writerecord = 1;

  return;
}

E 1
