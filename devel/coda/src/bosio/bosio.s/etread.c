
/* etRead.c - reading FPACK file records and filling ET buffer

     some terminology & info:

  block - physical record
  record - logical record
  IP - pointer to record header
  JP - pointer to data header
  KP - pointer to data
  BIOS_RECHEAD[irCODE] = 0 - complete segment
  BIOS_RECHEAD[irCODE] = 1 - first segment
  BIOS_RECHEAD[irCODE] = 2 - middle segment
  BIOS_RECHEAD[irCODE] = 3 - last segment

     input parameters:

  descriptor - file descriptor
  ev - pointer to ET buffer
  et_size - size of ET buffer (bytes)

     output parameters:

  size of event in ET (bytes)
  control[0] - event type
  control[1] - 
  control[2] - 
  control[3] - trigger pattern

*/

#include <stdio.h>
#include "bosio.h"
#include "fpack.h"

int
etRead(BOSIO *descriptor, int *ev, int et_size, int *size, int control[4])
{
  BOSIO *BIOstream;
  int *bufin;
  int i,ii,ind,error,nami;
  int start,ihd,nread,ndim,nf,ird,ntot,nama,nw;
  int dathead[70]; /* intermediate buffers */
  char *fmt;
 
  BIOstream = descriptor; /* set pointers */

  error = 0;

  /* check unit status (for sequential IO only) */

  CHECK_INPUT_UNIT(descriptor);

  /* allocate input buffer */

  ALLOCATE_INPUT_BUFFER(descriptor);

  /* scroll records until will get complete or first one */
  do
  {

    if(IP != 0 && NEXTIP(IP) < bufin[1])   /* we are in the middle of block */
    {
      IP = NEXTIP(IP);
    }
    else                                 /* block are finished - let's get another one */
    {
      ii = BIOS_RECL/sizeof(int);
      error = bosin(descriptor,&ii);
      bufin = BIOS_BUFP; /* could be changed in bosin() ! */

      if(error < 0)	/* end of file */
      {
        return(error);
      }
      if(error != 0)    /* some error */
      {
        return(error);
      }
      ii = LOCAL; frconv_(bufin,&ii,&error);  /* convert buffer */
      IP = 2;	/* successful read - reset pointer */
    }

    bcopy((char *)&bufin[IP],(char *)BIOS_RECHEAD, RECHEADLEN*sizeof(int));
    BIOS_NLOGREC++;
    if(BIOS_RECHEAD[irCODE] < 0 || BIOS_RECHEAD[irCODE] > 3) return(EBIO_WRONGSEGMCODE);
 
  } while(BIOS_RECHEAD[irCODE] != 0 && BIOS_RECHEAD[irCODE] != 1);


  ndim = et_size / sizeof(int);

  /* do we have space enough in ET buffer ? - does not make sence for splitted records ! */
  if((BIOS_RECHEAD[irNWRD]+RECHEADLEN) > ndim) /* not enough space in ET */
  {
    printf("etread: ERROR - not enough space in ET ( %d > %d )\n",
                        (BIOS_RECHEAD[irNWRD]+RECHEADLEN),ndim);
    start = 0;
    error = EBIO_NOETSPACE;
    goto error_exit;
  }


  /* we are in the begining of complete (=0) or first (=1) segment */
  JP = 0;
  error = start = 0;

  bcopy((char *)BIOS_RECHEAD, (char *)&ev[start], RECHEADLEN*sizeof(int));
  start += RECHEADLEN;

  control[0] = BIOS_RECHEAD[irNRUN];
  control[1] = 0;
  control[2] = 0;
  control[3] = BIOS_RECHEAD[irTRIG];

  if(BIOS_RECHEAD[irCODE] == 1) /* first segment means splitted record */
  {
/*printf("rechead0: len=%d\n",BIOS_RECHEAD[irNWRD]);*/
    do
    {
      do
      {
        GET_DATA_HEADER(descriptor,done,error,error_exit);
/*printf("rechead1: len=%d\n",BIOS_RECHEAD[irNWRD]);*/
      } while(dathead[idCODE] != 0 && dathead[idCODE] != 1);
      KP = 0;
      bcopy((char *)dathead, (char *)&ev[start], dathead[idNHEAD]*sizeof(int));
      ev[start+idCODE] = 0; /* no splited banks in ET buffer ! */

      nread = 0;
      while(1)
      {
        GET_DATA(&ev[start+dathead[idNHEAD]+nread],ndim,error);
        if(nread == ndim || dathead[idCODE] == 0 || dathead[idCODE] == 3)
        {
          ev[start+idDATA] = nread; /* update bank length - important for splited data */
/*printf("start=%d name >%4.4s< nr=%d\n",start,&ev[start+idNAME1],ev[start+idNSGM]);*/
          start += (dathead[idNHEAD] + nread);
          break; /* we got all data for present bank - exit from loop */
        }

        /* this is "second" header (dathead[idCODE] == 2 or 3) - do not copy it, just skip !!! */
        GET_DATA_HEADER(descriptor,done,error,error_exit);
/*printf("rechead2: len=%d\n",BIOS_RECHEAD[irNWRD]);*/
        KP = 0;
        if(nread && (dathead[idCODE] == 0 || dathead[idCODE] == 1)) {printf("!!!\n"); return(EBIO_NOTCONTCODE);}
      }
/*printf("rechead3: start=%d nread=%d ndim=%d\n",start,nread,ndim);*/
    } while(start < ndim);
    /*} while(nread < ndim);*/
  }
  else if(BIOS_RECHEAD[irCODE] == 0) /* complete record */
  {
    bcopy((char *)&bufin[IP+RECHEADLEN], (char *)&ev[start], BIOS_RECHEAD[irNWRD]*sizeof(int));
    start += BIOS_RECHEAD[irNWRD];
    goto done;
  }
  else
  {
    printf("etread: wrong BIOS_RECHEAD[irCODE]=%d\n",BIOS_RECHEAD[irCODE]);
    return(EBIO_NOTCONTCODE);
  }


  /* if we are here - something wrong */

  error = EBIO_NOETSPACE;
  printf("etread: if start=%d > ndim=%d, increase ET buffer size.\n",start,ndim); fflush(stdout);

error_exit: /* error exit from GET_DATA_HEADER() - could be end-of-file !!! */
/*
  if(error == -1) {printf("etread: end_of_file.\n"); fflush(stdout);}
  else            {printf("etread: error %d\n",error); fflush(stdout);}
*/
  return(error);

done:

/*printf("start1(out)=%d  ev[irNWRD]+RECHEADLEN=%d\n",start,ev[irNWRD]+RECHEADLEN);*/
  ev[irNWRD] = start - RECHEADLEN; /* update record length (need if BIOS_RECHEAD[irCODE] was 1) */
/*printf("start2(out)=%d  ev[irNWRD]+RECHEADLEN=%d\n",start,ev[irNWRD]+RECHEADLEN);*/

  ev[irNPHYS] = 0; /* set that value to zero: there are no splited records in ET */
  ev[irCODE] = 0;  /* set that value to zero: there are no splited records in ET */
  ev[irNRSG] = (ev[irNRSG] / 100) * 100;

  if(error == -1) error = 0;
  *size = start * sizeof(int);

  if(*size > et_size)
  {
    printf("etread: error: *size > et_size (%d > %d) - ET buffer not enough.\n",*size,et_size);
    error = EBIO_NOETSPACE;
  }

/*if((*size/4) == 14074) printf("etread: %d\n",BIOS_RECHEAD[irCODE]);*/

  return(error);
}
 
 








