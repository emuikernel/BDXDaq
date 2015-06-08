#ifndef VXWORKS

/* bigbufcheck.c - test for bigbufcheck function */

#include <stdio.h>
#include <stdlib.h>

#include "da.h"
#include "circbuf.h"
#include "etbosio.h"

int deflt; /* 1 for CODA format, 0 for BOS format (see deb_component.c) */

int nddl; /* see deb_component.c and BOS_format.c */
DDL ddl[NDDL]; /* see deb_component.c and BOS_format.c */

int
bufferCheck(unsigned int *cbuf, int nlongs)
{
  unsigned int lwd, t1, t2;
  int ii, jj, ix;
  int tlen, blen, dtype, typ;
  short shd;
  char cd;
  char *cp;
  short *sp;
  unsigned int *lp, *lpdebug;


  ii = 0;

  /* swap buffer header: BBHEAD words */
  lp = (unsigned int *)&cbuf[ii];
  for(jj=0; jj<BBHEAD; jj++)
  {
	lwd = LSWAP(*lp);
	*lp++ = lwd;
  }
  ii += BBHEAD;

  /* swap CODA fragments and BOS banks inside every fragment */
  while(ii<nlongs)
  {
    lp = (unsigned int *)&cbuf[ii];

    lwd = LSWAP(*lp);    /* Swap the CODA fragment length */
    *lp++ = lwd;
    blen = lwd - 1;
	t1=lwd;
	printf("\nCODA fragment length = %d (0x%04x)\n",blen,blen);

    lwd = LSWAP(*lp);    /* Swap the CODA fragment header */
    *lp++ = lwd;
    dtype = (lwd&0xff00)>>8;
    typ = (lwd>>16)&0xff;
	t2=lwd;
	printf("CODA fragment typ=%d, dtype = %d\n",typ,dtype);

    ii += 2;

    if(blen == 0) continue; /* nothing to do with empty fragment */

    if(dtype != DT_BANK)
    {
	  switch(dtswap[dtype])
      {
        case 0:
		  printf("case 0: no swap\n");
					/*
		  printf("ii=%d nlongs=%d 0x%08x 0x%08x)\n",
            ii,nlongs,t1,t2);
		  {
            FILE *fd;
            int iii;
            fd = fopen("/home/boiarino/abc.txt","w");
            for(iii=0; iii<ii; iii++) fprintf(fd,"[%6d] 0x%08x\n",iii,cbuf[iii]);
            fclose(fd);
		  }
		  exit(0);
					*/
	      /* No swap */
	      ii += blen;
	    break;

        case 1:
		  printf("case 1: short swap\n");
	      /* short swap */
	      sp = (short *)&cbuf[ii];
	      for(jj=0; jj<(blen<<1); jj++)
          {
	        shd = SSWAP(*sp);
	        *sp++ = shd;
	      }
	      ii += blen;
	      break;

        case 2:
		  /*printf("case 2: long swap\n");*/
          /* long swap */
          if(deflt || ((typ > 15) && (typ < 32)) || blen < 2) /* CODA fragment contains CODA data */
          {
            lp = (unsigned int *)&cbuf[ii];
            for(jj=0; jj<blen; jj++)
            {
              lwd = LSWAP(*lp);
              *lp++ = lwd;
            }
          }
          else /* CODA fragment contains BOS banks */
          {
            unsigned int bankid, banknum, ncol, nrow, lfmt, fmtlen, nwords, bb;

            lpdebug = lp = (unsigned int *)&cbuf[ii];

            bb = 0;
			
            printf("=== fragment contains BOS banks, blen=%d\n",blen);
            for(jj=0; jj<blen; jj++) printf("=== [%3d] 0x%08x\n",jj,LSWAP(lp[jj]));
			
            while(bb < blen)
            {
              /* swap BOS bank header */
              for(jj=0; jj<2; jj++)
              {
                lwd = LSWAP(*lp);
                *lp++ = lwd;
              }
              bb += 2;
			  if(bb >= blen)
              {
                printf("warn: early break: bb=%d, blen=%d\n",bb,blen);
                break;
              }
              bankid  = ((*(lp-2))>>16)&0xFFFF;
              banknum = (*(lp-2))&0xFFFF;
              nrow    = *(lp-1);
              ncol = ddl[bankid].ncol;
              lfmt = ddl[bankid].lfmt;
              printf("bankid=%d banknum=%d nrow=%d ncol=%d lfmt=%d\n",bankid,banknum,nrow,ncol,lfmt);


if(lfmt <= 0)
{
  printf("ERROR: lfmt=%d bankid=%d\n",lfmt,bankid);fflush(stdout);
  printf("Beginning of fragment: ii=%d, cbuf[ii]=0x%08x, bb=%d\n",ii,cbuf[ii],bb);
  

  /*
=== [107] 0x80710000 - must be 0x00710000

		  {
            FILE *fd;
            int iii;
            fd = fopen("/home/clasrun/coda_eb/abc.txt","w");
            fprintf(fd,"Beginning of fragment: ii=%d, cbuf[ii]=0x%08x, bb=%d\n",ii,cbuf[ii],bb);
            fprintf(fd,"Whole buffer (nlongs=%d):\n",nlongs);
            for(iii=0; iii<nlongs; iii++) fprintf(fd,"[%6d] 0x%08x\n",iii,cbuf[iii]);
            fclose(fd);
		  }
  */
  exit(0);
}

              fmtlen = 4 / lfmt;
              nwords = (ncol * nrow * fmtlen + 3) / 4;

              /* if BOS bank length inconsistent with CODA fragment length,
              print error message and try to recover */
              if((bb+nwords) > blen)
              {
                printf("ERROR: bb=%d nwords=%d -> (bb+nwords)=%d > blen=%d\n",
                  bb,nwords,(bb+nwords),blen);
                printf("ERROR: bankid=0x%04x (%d)\n",bankid,bankid);
                printf("ERROR: banknum=%d\n",banknum);
                printf("ERROR: nrow=%d ncol=%d lfmt=%d\n",nrow,ncol,lfmt);
                nwords = blen - bb;
                printf("Trying to recover: set nwords=%d, nrow=%d (%d %d)\n",
					   nwords,(nwords*lfmt)/ncol,lfmt,ncol);
                nrow = (nwords*lfmt)/ncol;
                *(lp-1) = nrow;
              }

              /*printf("=== bb=%d, bankid=%d, banknum=%d, nwords=%d, nrow=%d, ncol=%d, fmtlen=%d\n",
                bb, bankid, banknum, nwords, nrow, ncol, lfmt);*/
			  
              /* convert BOS data */
              if(lfmt == 1) /* long */
              {
                /*printf("=== swap %6d long words\n",nwords);*/
                for(jj=0; jj<nwords; jj++)
                {
                  lwd = LSWAP(*lp);
                  *lp++ = lwd;
                }
              }
              else if(lfmt == 2) /* short */
              {
                /*printf("=== swap %6d short words\n",nwords*2);*/
                sp = (short *)lp;
                for(jj=0; jj<(nwords*2); jj++)
                {
                  shd = SSWAP(*sp);
                  *sp++ = shd;
                }
                lp += nwords;
              }
              /* do nothing for char */

              bb += nwords;
              /*printf("=== bb = %d\n",bb);*/
            }

          }

	      ii += blen;
	      break;

        case 3:
		  printf("case 3: double swap\n");
	      /* double swap */
	      lp = (unsigned int *)&cbuf[ii];
	      for(jj=0; jj<blen; jj++)
          {
	        lwd = LSWAP(*lp);
	        *lp++ = lwd;
	      }
	      ii += blen;
	      break;

        default:
		  printf("default: no swap\n");
	      /* No swap */
	      ii += blen;
      }
    }
    else
    {
      printf("dtype=0x%08x\n",dtype);
    }
  }

  return(0);
}


int
main()
{
  FILE *fd;
  char str[128], *ptr;
  unsigned int data, nlongs;
  unsigned int *cbuf;

  deflt = 0; /* 1 for CODA format, 0 for BOS format (see deb_component.c) */

  cbuf = (unsigned int *) malloc(10000000);

  nddl = NDDL;
  etDDLInit(nddl, ddl);
  printf("ddl=0x%08x size=%d\n",ddl,sizeof(DDL));

  fd = fopen("abc6.txt","r");
  fgets(str,128,fd);
  fgets(str,128,fd);
  nlongs = 0;
  while(fgets(str,128,fd)>0)
  {
    ptr = (char *) &str[9];
    ptr[10] = '\0';
    sscanf(ptr,"%x",&data);
    /*printf(">%s< -> 0x%08x\n",ptr,data);*/
    cbuf[nlongs++] = LSWAP(data);
  }
  fclose(fd);

  bufferCheck(cbuf, nlongs);

  exit(0);
}

#else

int
bigbufcheck_dummy()
{
  return;
}

#endif
