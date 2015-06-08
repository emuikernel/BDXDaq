
/* ttvme.c - service routines for the VME crates */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"

/*#define DEBUG 1*/

#define kUPDATE 1000


static char* Version = " VERSION: ttvme.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;

static int ncolumns[2];
static int counter[4][32];

/* will be called in 'Go' transition */

void
TT_VmeGo()
{
  int i,j;

  printf("cleanup counters ...\n");

  for(i=0; i<2; i++)
  {
    ncolumns[i] = 0;
  }

  for(i=0; i<4; i++)
  {
    for(j=0; j<32; j++)
    {
      counter[i][j] = 0;
    }
  }

  return;
}


/* process event */

int
TT_MakeVmeBank(long *bufin, long *bufout, TTSPtr ttp)
{
  unsigned long *fb, *in;           /* input data pointer */
  unsigned long *hit, *iw;          /* output data pointers */
  unsigned short out[2][16], *ptr16;
  int bit1, bit2, len, id;

  /* set input data pointer */

  fb = (unsigned long *)&bufin[NHC];

  /* set output data pointer */

  iw = (unsigned long *)&bufout[2];
  len = 0;

  /* scan input buffer assuming it contains banks created by bosMopen() */

  while((long *)fb < (&bufin[1] + bufin[0]))
  {
    unsigned long nama, nr;
    char name[9];
    int count, i, ind, len1, len2, chan, j, nch, nchold, ncol, nrow, lfmt;

    /* get info from bank header (mini-format, 2 header words) */
    id = ((*fb) >> 16) & 0xffff;
    nr = (*fb++) & 0xffff;
    nrow = *fb++;
	/*
	printf("ttvme: id=%d nr=%d nrow=%d\n",id,nr,nrow);
	*/
    if(nrow == 0) continue;
    if(bosMgetname(id, name)<0)
    {
      printf("tttrans: ERROR in bosMgetname()\n");
      return(len);
	}

    ncol = bosMgetncol(name);
    lfmt = bosMgetlfmt(name);
    count = (ncol * nrow * (4/lfmt) + 3) / 4;

    nama = *((unsigned long *)name);

/* hook for 'HLS '; all this wrong !!! */
if( (nama&0xFF) == '\0' )
{
  /*printf("befor nama = 0x%08x\n",nama);*/
  nama = nama & 0xFFFFFF00;
  nama = nama + 0x20; 
  /*printf("after nama = 0x%08x\n",nama);*/
}

    /*printf("\nid=%d nr=%d count=%x hex (%d decimal) ->name(nama)>%s<(0x%08x)\n",
                id,nr,count,count,name,nama); fflush(stdout);*/

    /*
    if(nama == *((unsigned long *)"RC00") )
	{
      fb += count;
	}
    else*/ if(nama == *((unsigned long *)"PTRN") )
	{
      fb += count; /* ignore; will be processed in the TT_Main() function */
	}
    else if(nama == *((unsigned long *)"HLS "))
    {
      /*printf("!! nama=>%4.4s< nr=%d count=%d lfmt=%d\n",&nama,nr,count,lfmt);*/
      if(count > 0)
      {
        ncolumns[nr] = 1;
        if(count > 1)
        {
          in = fb;
          nchold = (*in++) & 0x1f000000;
          nch = (*in++) & 0x1f000000;
          while(nchold < nch && (long *)in <= (&bufin[1] + bufin[0]))
          {
            ncolumns[nr] ++;
            nchold = nch;
            nch = (*in++) & 0x1f000000;
          }
        }
        /*printf("ncolumns=%d\n",ncolumns[nr]);*/
        if( (ind = bosNopen(iw,nama,nr,ncolumns[nr],count/ncolumns[nr])) <= 0 )
        {
          printf("TT_MakeVmeBank: ERROR4: bosNopen returned %d !!!\n",ind);
          return(len);
        }
        hit = &iw[ind+1];

        j = 0;
        for(i=0; i<count; i++)
        {
          *hit++ = *fb++;
          bit1 = (int)((*(hit-1)&0x40000000)>>30);
          bit2 = (int)((*(hit-1))>>31);
          if(bit1)
          {
            if(i<(count-2)) counter[nr*2][j++] += *(hit-1) & 0xffffff;
            else            counter[nr*2][j++] = *(hit-1) & 0xffffff;
          }
          else
          {
            if(i<(count-2)) counter[nr*2+1][j++] += *(hit-1) & 0xffffff;
            else            counter[nr*2+1][j++] = *(hit-1) & 0xffffff;
          }
          if(j == ncolumns[nr]) j=0;
	  /*
          if(!(TT_nev%kUPDATE))
             printf("scaler : bit2=%1d bit1=%1d ch=%2d value=%6x\n",bit2,bit1,
                    (int)((*(hit-1)&0x1f000000)>>24),(int)(*(hit-1)&0xffffff));
		    */
        }
        len += bosNclose(iw,ind,ncolumns[nr],count/ncolumns[nr]);
        /*if(!(TT_nev%kUPDATE)) printf("\n");*/

      }
	}
    else if(nama == *((unsigned long *)"HLS+"))
    {
      fb += count;
	  /*printf("ttvme(HLS): scaler event ...\n");*/
      if(ncolumns[nr] > 1) /* write summing scalers for HLS #0 */
      {
        if( (ind = bosNopen(iw, *((unsigned long *)"HLS+") ,nr,ncolumns[nr]*2,1)) <= 0 )
        {
          printf("TT_MakeVmeBank: ERROR2: bosNopen returned %d !!!\n",ind);
          return(len);
        }
        hit = &iw[ind+1];

        /*printf("\nHLS bank bit1 set:\n");*/
        for(j=0; j<ncolumns[nr]; j++)
        {
          *hit++ = counter[nr*2][j];
          /*printf("  counter[%d][%d] = %09d\n",nr*2,j,counter[nr*2][j]);*/
        }

        /*printf("\nHLS bank bit1 reset:\n");*/
        for(j=0; j<ncolumns[nr]; j++)
        {
          *hit++ = counter[nr*2+1][j];
          /*printf("  counter[%d][%d] = %09d\n",nr*2+1,j,counter[nr*2+1][j]);*/
        }

        len += bosNclose(iw,ind,ncolumns[nr]*2,1);
        /*printf("\n");*/
      }
    }
    else if(nama == *((unsigned long *)"MPTA") )
    {
      bzero((char *)out, 64);
      len2 = *fb++; /* tdc */
      for(i=0; i<len2; i++)
      {
        chan = (*fb & 0x1e0000)>>17;
        out[0][chan] = (short)*fb++;
      }
      len2 = *fb++; /* adc */
      for(i=0; i<len2; i++)
      {
        chan = (*fb & 0x1e0000)>>17;
        out[1][chan] = (short)*fb++;
      }
      len1 = len2; /* to be used in next piece of code */
      len2 = *fb++; /* adc1 */
      for(i=0; i<len2; i++)
      {
        chan = (*fb & 0x1e0000)>>17;
        out[1][chan+len1] = (short)*fb++;
      }
      len2 = 0;
      for(i=0; i<16; i++)
      {
        if(out[0][i] != 0 || out[1][i] != 0) len2++;
      }
      if( (ind = bosNopen(iw,nama,nr,3,len2)) <= 0 )
      {
        printf("TT_MakeVmeBank: ERROR5: bosNopen returned %d !!!\n",ind);
        return(len);
      }

	  /*
      ptr16 = (unsigned short *)&iw[ind+1];
      if(TT_nev%kUPDATE == 0) printf("nama=>%4.4s< nw=%d\n",&nama,count);
      for(i=0; i<16; i++)
      {
        if(out[0][i] != 0 || out[1][i] != 0)
        {
          *ptr16++ = i + 1;
          *ptr16++ = out[0][i];
          *ptr16++ = out[1][i];
          if(TT_nev%kUPDATE == 0)
            printf(" ch=%2d tdc=%4x adc=%4x\n",i,out[0][i],out[1][i]);
        }
      }
	  */

      len += bosNclose(iw,ind,3,len2);
    }
    else /* no special actions for that bank - just pass it (TEST IT FOR B16 and B08 !!!) */
    {
#ifdef DEBUG
      printf("passing bank: nama=>%4.4s< nw=%d ncol=%d nrow=%d\n",
		&nama,count,ncol,nrow);
#endif
      if( (ind = bosNopen(iw,nama,nr,ncol,nrow)) <= 0 )
      {
        printf("TT_MakeVmeBank: ERROR7: bosNopen returned %d >%4.4s< nw=%d\n",
                        ind,(char *)&nama,count);
        return(len);
      }
      hit = &iw[ind+1];
      for(i=0; i<count; i++) *hit++ = *fb++;
      len += bosNclose(iw,ind,ncol,nrow);
    }

  }

  return( len );  /* returns data length (long words) */
}




