
#include <stdio.h>
#include "bosio.h"


/* #define COND_DEBUG */
/* #define DEBUG  */
#include "dbgpr.h"


int
bosin(BOSIO *descriptor, int *ntot)
{
  BOSIO *st;
  int i, error, ircn, nskip, nbytes, *buf;

  st = descriptor;
  error = 0;

  buf = st->bufp;

  if(st->access == 1)		/* sequential write */
  {
    ircn = 0;
    nskip = 0;
  }
  else if(st->access == 2)	/* direct write - determine record number */
  {
    ircn = st->innum + 1;
    nskip = ircn - st->recnum - 1;
  }

  if(st->client != 0)
  {
    nskip = nskip*(*ntot)*sizeof(int);
    nbytes = (*ntot)*sizeof(int);
 
    if(st->client < 0)	/* network channel is dead, try to reconnect */
    {
      error = bosnres(descriptor, st->client, st->stream, 1);
      if(error != 0) return(EBIO_IOERROR);	/* reconnection failed */
    }
a:
    readc(&st->client,&st->stream,&nskip,&nbytes,buf,&error);
    if(error == EBIO_EOF)	/* network channel is dead, try to reconnect */
    {
      error = bosnres(descriptor, st->client, st->stream, 1);
      if(error == 0) goto a;	/* reconnection successful */
      error = EBIO_EOF;
    }

    if(error < nbytes)	/* check end_of_file (?) */
    {
      error = EBIO_EOF;
    }
    else
    {
      error = 0;
    }
  }
  else
  {

a10:

    error = cget(&st->stream,&ircn,&st->medium,ntot,buf);

    /*if(error != 0) printf("bosin : cget returns error = %i\n",error);*/
    if(error == EBIO_EOF)
    {
      if(st->splitmb == 0 && st->splitev == 0) /* splited files not allowed */
      {
        /*printf("bosin: end-of-file-1 >%s<\n",st->finame);*/
        return(EBIO_EOF);
      }
      else /* trying to open next portion of the splited file */
      {
        printf("bosin: Trying to ReOpen file ...\n");
        st->reopen = 1;
        if ( (error=BOSREOPEN(descriptor)) != 0)
        {
          if(error == -1) printf("bosin: end of file in BOSREOPEN\n");
          else printf("bosin: error ReOpen file >%s< : %i\n",st->finame,error);
          return(error); /* in case of opening error return end-of-file */
        }
        else
        {

          /* reallocate buffer - could be different length ! */
          BOSIOptr BIOstream;
          int ii = 12;
          int jbuf[12], error;

          BIOstream = st;
          error = cget(&st->stream,&ircn,&st->medium,&ii,jbuf);
          fswap_(jbuf,&ii);
          if(error < 0)
          {
            return(EBIO_EOF);
          }
          bosRewind(descriptor);
          BIOS_CURSTA = 1;
          *ntot = jbuf[0];
          BIOS_RECL = (*ntot) * sizeof(int);
	  /*printf("bosin: BIOS_RECL=%d\n",BIOS_RECL); fflush(stdout);*/
          FREE(BIOS_BUFP);
          BIOS_BUFP = buf = (int *) MALLOC(BIOS_RECL); /* seting buf here not enough to be visible in calling program !!! */
          BIOS_NPHYSREC = 0;
          BIOS_NLOGREC = 0;
          BIOS_NDATREC = 0;
          IP = 0;

          printf("bosin: INFO: ReOpened file >%s<\n",st->finame); fflush(stdout);
          goto a10;
        }
      }
    }
    if(error != 0) return(EBIO_IOERROR);
  }
 
  if(error != 0) return(error);
 
/* successful operation */
 
  st->nphysrec++;
  st->recnum = ircn;
  st->innum = ircn;
  fswap_(buf,ntot);              /* swap buf */
  st->recfmtr = buf[3];          /* actual format code / origin code */
  st->recl = buf[0]*sizeof(int); /* record length (bytes) */
  /*printf("bosin: st->recl=%d buf[0]=%d buf[1]=%d\n",st->recl,buf[0],buf[1]); fflush(stdout);*/

  return(error);
}
 
