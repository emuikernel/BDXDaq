
/* ttreadraw.c - simulation interface for CODA (read raw file) */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "bosio.h"
#include "coda.h"
#include "tt.h"

extern int  *iw;                    /* BOS Array */
extern BOSIO *BOSIOdescriptor;		/* BOS file descriptor */
extern char *TT_DataFile;
extern int  TT_nev;
static int  ntst=5;

static char* Version = " VERSION: ttreadraw.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;

/******************************************************************************

  Routine             : TT_ReadRaw

  Input parameters    : none

  Output parameters   : none

  Discription         : This routine fill data from next event to rol->dabufp
                        Algoritm:
                           1. get next event from BOS file (if end - rewind)
                           2. extract RAW BOS bank for this ROC
                           4. fill output buffer rol->dabufp by raw data

******************************************************************************/

#define IW bcs_.iw

#define PRINT_BUFFER \
  { \
    unsigned int *aa = (PTDC1Ptr)&bufin[1]; \
    printf("ttvme first data: %08x %08x %08x %08x %08x %08x\n", \
      aa[0],aa[1],aa[2],aa[3],aa[4],aa[5]); \
    i = 1; \
    printf("buffer start at 0x%08x, buffer end at 0x%08x (len=%d)\n", \
      aa,((&bufin[1])+bufin[0]),bufin[0]); \
    while((long *)aa < (((&bufin[1])+bufin[0]))) \
    { \
printf("[%3d]->0x%08x(0x%08x)->slot %2d, chan %3d, edge=%1d, data %7d (count=%d sl=%d)\n", \
        i,aa,*aa,((*aa)>>27)&0x1F,((*aa)>>19)&0x7F,((*aa)>>26)&0x1,(*aa)&0x3FFFF, \
        ((*aa)>>5)&0x3FFFFF,(*aa)&0x1F); \
      aa++; \
      i++; \
    } \
  }

int 
TT_ReadRaw(long *bufout, int roc)
{
  INDPtr fb;
  int  i, ind, ind1, slot, len, len1, lll, id, status=1, count;
  char bbn[16];
  int skip;
  long *bufout1, *bufin, *buflen, *boardlen;
  long buftmp[32000];

  /* drop all BOS banks from list E */

  bdrop_(iw,"E",1); /* drop all BOS banks from list E */
  bgarb_(iw); /* Garbage collection */

  if(BOSIOdescriptor==NULL)
  {
    printf("TT_ReadRaw: ERROR: BOS file are not opened !!!\n");
    return(0);
  }

  /* read events skiping type > 15 */

  skip = 1;
  while(skip)
  {
    if(status = bosRead(BOSIOdescriptor,iw,"E"))
    {
      if(status = -1) /* end of file */
      {
        status = bosClose(BOSIOdescriptor); /* close BOS file */

        status=bosOpen(TT_DataFile,"r",&BOSIOdescriptor); /* open it again; status=0 - O'k */
        if(status) 
        {
          printf("TT_ReadRaw: ERROR: Can't open BOS file %s\n",TT_DataFile);
          return status;
        }
        else
        {
          printf("TT_ReadRaw: INFO: BOS file %s reopened.\n",TT_DataFile);
        }
        status=1;
      }
      if(status = bosRead(BOSIOdescriptor,iw,"E"))
      {
        printf("TT_ReadRaw: ERROR: after bosRead status = %d\n",status);
        return(status);
      }
    }
    /* get HEAD bank and check event type */
    ind = bosNlink(iw,"HEAD",0);
    if(ind != 0 && iw[ind+6] < 16) skip = 0;
  }

  /*printf("CLASSIM: Read event\n");*/
  len = len1 = 0;
  /* ===  Go with Raw Data === */
  sprintf (bbn,"RC%2.2i",roc);
  /*printf("CLASSIM: Looking for BOS Bank '%s'\n",bbn);*/
  ind = bosNlink(iw,bbn,0);
  if(ind)
  {
    if(roc <= 11) /* DC - add extra word for odd groups - a la 64 bit mode */
	{
      DBG("CLASSIM: Got BOS Bank '%s' len = %i; ind = %i\n",bbn,iw[ind-1],ind);
      len=iw[ind-1];
      DBG("len=%d\n",len);
bufout1 = bufout;
      ind1 = ind;
      fb = (INDPtr) &iw[ind1];
      slot = fb->slot;
      for(i=0;i<=len;i++)
      {
        fb = (INDPtr) &iw[ind+i];
        if(slot - fb->slot > 1 || i == len)
        {
          slot = fb->slot;
          if( (ind+i-ind1)%2 )
          {
/* emulates 64 bits
            ((INDPtr)bufout)->slot = SLOT64;
            bufout ++;
            len1 ++;
*/
          }
          DBG("copy %d words\n",ind+i-ind1);
          bcopy((char *)&iw[ind1], (char *)bufout, (ind+i-ind1)*4);
          bufout += ind+i-ind1;
          ind1 = ind+i;
        }
        else
        {
          slot = fb->slot;
        }
      }
      bufout --;

      fb = (INDPtr) bufout1;
      DBG("%08x %08x\n",(int)fb,(int)bufout);
      DBG("slot     channel     data\n");
      if(TT_nev < ntst) while((int)fb <= (int)bufout)
      {
        printf("%3i ( %3i - %3i ) %4X\n",fb->slot,fb->channel,fb->channel>>1,fb->data);
        fb ++;
      }
    }
    else if(roc == 24) /* lac2 */
    {
      len=iw[ind-1];
      /*printf("CLASSIM: Got BOS Bank '%s' len = %i; ind = %i\n",bbn,len,ind);*/
	  
      bufout1 = bufout;
	  

      /* make compact BOS header */
      id = bosMgetid(bbn);
      if(id < 0) {printf("bosMlink: ERROR: bosMgetid returns %d\n",id); exit(0);}
      /*else printf("id=%d\n",id);*/

      bufout1[0] = (id << 16)/* + nr*/;
      bufout1[1] = 0; /* bank len will be here */
      buflen = &bufout1[1]; 
/*printf("! 0x%08x %d\n",buflen,*buflen);*/
      bufout1 += 2;

      memcpy((char *)buftmp, (char *)&iw[ind], len<<2);

for(i=0; i<len; i++)
{
  if( (buftmp[i]>>23)&0x1 ) /* data word */
  {
/*
printf(" [%3d] 0x%08x -> slot=%2d (chip=%2d chan=%2d) ch=%2d tdc=%5d (HOV=%1d OOV=%1d LOCK=%1d)\n",
      i,buftmp[i],(buftmp[i]>>27)&0x1F,(buftmp[i]>>19)&0x7,(buftmp[i]>>16)&0x7,
      (buftmp[i]>>16)&0x3F,
      buftmp[i]&0xFFFF,(buftmp[i]>>24)&0x1,(buftmp[i]>>25)&0x1,(buftmp[i]>>26)&0x1);
*/

      /* put data word */
      *bufout1 = ( (((buftmp[i]>>27)&0x1F)<<27) |
                   (((buftmp[i]>>16)&0x3F)<<19) |
                   (buftmp[i]&0xFFFF) );

      /* update board header */
      len1 = ((*boardlen)>>5)&0x3FFFFF;
      slot = (*boardlen)&0x1F;
      len1 ++;
      *boardlen = slot | (len1<<5);
      /*printf("?? 0x%08x %d\n",boardlen,*boardlen);*/

      /* update bank length */
      *buflen = (*buflen) + 1;
      /*printf("!! 0x%08x %d\n",buflen,*buflen);*/

      bufout1 ++;
  }
  else /* header/trailer */
  {
/*
printf("== [%3d] 0x%08x => slot=%2d ",i,buftmp[i],(buftmp[i]>>27)&0x1F);
*/
    if(((buftmp[i]>>27)&0x1F)==0) /*printf("-> filler word\n")*/;
    else if(((buftmp[i]>>27)&0x1F)==30) /*printf("-> data not valid\n")*/;
    else
    {
/*
printf("-> TOV=%1d evnum=%2d trigtime=%4d Xor=%1d chip=%2d chan=%2d\n",
        (buftmp[i]>>22)&0x1,(buftmp[i]>>16)&0x3F,(buftmp[i]>>7)&0x1FF,(buftmp[i]>>6)&0x1,
        (buftmp[i]>>3)&0x7,(buftmp[i])&0x7);
*/

      if( ((buftmp[i])&0x7) == 0) /* create board header */
      {
        boardlen = bufout1; /* remember pointer */
        *boardlen = ((buftmp[i]>>27)&0x1F) | (1<<5); /*put slot number and 1 as word count*/
        /*printf("??? 0x%08x %d\n",boardlen,*boardlen);*/

        /* update bank length */
        *buflen = (*buflen) + 1;
        /*printf("!!! 0x%08x %d\n",buflen,*buflen);*/

        bufout1 ++;
      }
	}
  }
}

bufin = &bufout[1]; /* pointer to the bank length */
/*
PRINT_BUFFER;
*/


  /* EC1 */
{
  int j, id1, id2, ind;
  unsigned short *ptr, ncol, nrow;
  /*printf("0\n");*/
  count=0;
  ind = bosNlink(iw,"EC1 ",1);
  if(ind)
  {
    printf("sector 1 =========\n");
    nrow = iw[ind-5];
    ncol = iw[ind-6];
    lll = iw[ind-1];
    ptr = (unsigned short *)&iw[ind];
    printf("   id    ->   tdc   adc   tdc   adc\n");
    for(i=0; i<nrow; i++)
	{
      id1=((*ptr)>>8)&0xFF;
      id2=(*ptr)&0xFF;
      if(id1==2&&id2==25||id1==4&&id2==25);
      else count ++;
      printf("%3d %3d -> ",id1,id2);
      ptr++;
      for(j=1; j<ncol; j++) printf("%6d",*ptr++);
      printf("\n");
	}
  }
  ind = bosNlink(iw,"EC1 ",2);
  if(ind)
  {
    printf("sector 2 =========\n");
    nrow = iw[ind-5];
    ncol = iw[ind-6];
    lll = iw[ind-1];
    ptr = (unsigned short *)&iw[ind];
    printf("   id    ->   tdc   adc   tdc   adc\n");
    for(i=0; i<nrow; i++)
	{
      id1=((*ptr)>>8)&0xFF;
      id2=(*ptr)&0xFF;
      if(id1==2&&id2==25||id1==4&&id2==25);
      else count ++;
      printf("%3d %3d -> ",id1,id2);
      ptr++;
      for(j=1; j<ncol; j++) printf("%6d",*ptr++);
      printf("\n");
	}
  }
  printf("count=%d\n",count);
}

    }
    else if(roc == 25) /* polar */
    {
      len=iw[ind-1];
      DBG("CLASSIM: Got BOS Bank '%s' len = %i; ind = %i\n",bbn,len,ind);
/*bufout1 = bufout;*/

      bcopy((char *)&iw[ind], (char *)bufout, len*4);
      bufout += len;

      bufout--;
    }
    else /* nonDC */
    {
      DBG("CLASSIM: Got BOS Bank '%s' len = %i; ind = %i\n",bbn,iw[ind-1],ind);
      len=iw[ind-1];
/*bufout1 = bufout;*/


/*
      DBG("slot     channel     data\n");
      if(TT_nev < ntst) for(i=0;i<len;i++)
      {
        fb = (INDPtr) &iw[ind+i];
        printf("%3i ( %3i - %3i ) %4X\n",fb->slot,fb->channel,fb->channel>>1,fb->data);
      }
*/


/* emulates 64 bits
((INDPtr)bufout)->slot = SLOT64;
bufout ++;
len1 ++;
*/

      DBG("CLASSIM: copying to bufout ...\n");

/*
      bcopy((char *)&iw[ind], (char *)bufout, len*4);
*/
      lll = len / 2;
      bcopy((char *)&iw[ind], (char *)bufout, lll*4);
      bufout += lll;

/* emulates 64 bits
((INDPtr)bufout)->slot = SLOT64;
bufout ++;
len1 ++;
*/

      bcopy((char *)&iw[ind+lll], (char *)bufout, (len-lll)*4);
      bufout += (len-lll);

      DBG("CLASSIM: %i words copied to bufout\n",len);


      bufout--;
      fb = (INDPtr) bufout1;
      DBG("%08x %08x\n",(int)fb,(int)bufout);
      DBG("slot     channel     data\n");
      if(TT_nev < ntst) while((int)fb <= (int)bufout)
      {
        printf("%3i ( %3i - %3i ) %4X\n",fb->slot,fb->channel,fb->channel>>1,fb->data);
        fb ++;
      }

    }
  }
  else
  {
    /*printf("CLASSIM: BOS Bank '%s' not found\n",bbn)*/;
  }

  if(ind)
  {
    /*printf("==> count=%d\n",count);*/
    if(count) return(bufout[1]);
    else      return(0);
  }

  return(0);
}





