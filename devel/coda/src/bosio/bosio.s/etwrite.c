
/* etWrite.c - writing FPACK file from ET buffer

       input parameters:

   descriptor - file descriptor
   ev - pointer to ET buffer

*/
#include <stdio.h>
#include "fpack.h"
#include "ioincl.h"


#define min(a,b) (a<b)?a:b
#define max(a,b) (a>b)?a:b


/******************************************************/
/*     ET -> FPACK file                               */
/******************************************************/

int
etWrite(BOSIO *descriptor, int *ev)
{
  BOSIO *BIOstream;
  int *bufout;
  int ifree,error,lenh,lenb,ncop,nfopen,lrlen,ind;
  int ip,jp,kp; /* Ukazatel` na record segment header i data header v ET bufere */
  int dathead[70]; /* intermediate buffers */
  char *fmt, *ch;
  int i;

  /* following call will swap input buffer if necessary (???) */
/* !!!!!!!!!!! ET should always provide LOCAL format, so we should NOT swap here !!!!!!!!!!!!!!
  etfswap((unsigned int *)ev);
*/

  BIOstream = descriptor; /* set pointers */
  bufout    = BIOS_BUFP;
  error     = nfopen = 0;
  lrlen     = ev[irNWRD]+RECHEADLEN;

  ALLOCATE_OUTPUT_BUFFER;

  /*********************
   * check unit status *
   *********************/

  CHECK_OUTPUT_UNIT(descriptor);

  /********************************************************
   * do we have enough space in buffer for whole record ? *
   ********************************************************/

  /*printf("rest %d words, need %d words.\n",bufout[0]-bufout[1],lrlen);*/

  if((bufout[0]-bufout[1]) > lrlen)
  {
    bcopy((char *)ev, (char *)&bufout[IP], lrlen*sizeof(int));

    BIOS_RECHEAD[irPTRN ] = SWAP;
    BIOS_RECHEAD[irFORG ] = LOCAL*16;
    BIOS_RECHEAD[irNRSG ] = 100*BIOS_NPHYSREC+(BIOS_NSEGM-1);
    BIOS_RECHEAD[irNPHYS] = 0;
    BIOS_RECHEAD[irCODE ] = 0;

    /* go through banks seting dathead[idCODE] to zero */

    ind = IP + RECHEADLEN;
    /*printf("IP+lrlen=%d\n",IP+lrlen);*/
    while(ind < IP+lrlen)
    {
      /*printf("ind=%d headlen=%d name>%4.4s< code=%d size=%d\n",
      ind,bufout[ind],&bufout[ind+1],bufout[ind+idCODE],bufout[ind+idDATA]);*/
      ind = ind+bufout[ind+idNHEAD]+bufout[ind+idDATA];
    }

    IP = bufout[1] + lrlen;
    bufout[1] = IP;

    if (nfopen) return(EBIO_NEWFILEOPEN);
    return(0);
  }

  /********************************************
   * space not enough - write splitted record *
   ********************************************/

  /*********************
   * add record header *
   *********************/

  if(bufout[1]+81 > bufout[0])	/* we don't know data header length, let 70 */
  {
    WRITE_BUF_TO_DISK(descriptor);
    START_NEW_PHYSREC;
    BIOS_NSEGM--;
  }
  BIOS_NSEGM++;
  BIOS_NLOGREC++;

  bcopy((char *)ev, (char *)BIOS_RECHEAD, RECHEADLEN*sizeof(int));

  BIOS_RECHEAD[irPTRN ] = SWAP;	   /* the pattern to recognize byte swapping     */
  BIOS_RECHEAD[irFORG ] = LOCAL*16; /* format code: 1-IEEE,2-IBM,3-VAX,4-DEC      */
  BIOS_RECHEAD[irNRSG ] = 100*BIOS_NPHYSREC+(BIOS_NSEGM-1); /* #logrec(starting 1)*100 + #segment(starting 0) */
  BIOS_RECHEAD[irNPHYS] = 0;        /* # of several phys. records in one logical  */
  BIOS_RECHEAD[irNWRD ] = 0;        /* the number of words in this record segment */

  bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], RECHEADLEN*sizeof(int));

  bufout[1] = bufout[1] + RECHEADLEN;	/* update physrec length info */
  JP = IP + RECHEADLEN;    /* set data header pointer in I/O buffer */

  jp = 0 + RECHEADLEN;     /* set data header pointer in ET buffer */
  ifree = JP;              /* Ukazatel` na pervoe svobodnoe slovo v I/O bufere */

  /*****************
   * loop on banks *
   *****************/

  do
  {
    /*******************
     * add data header *
     *******************/

    lenh = ev[jp];          /* Dlina headera dannyh */
    lenb = ev[jp+idDATA];   /* Dlina dannyh         */
    JP = ifree;             /* Nachalo headera      */
    KP = JP + lenh;         /* Nachalo dannyh       */
    if( (ifree+lenh+1) > bufout[0] ) /* there is no space for data header */
    {
      /* Update record segment header and write to disk this buffer */
      if(bufout[IP+irCODE] <= 1) bufout[IP+irCODE]++; /* update record segment code */
      bufout[IP+irNWRD] = ifree-(IP+RECHEADLEN); /* update # of words in the record segment */
      BIOS_RECHEAD[irCODE] = bufout[IP+irCODE];       /* save   record segment code */
      WRITE_BUF_TO_DISK(descriptor);
      START_NEW_PHYSREC;
      BIOS_NSEGM--;
      /* Update record segment header */
      BIOS_RECHEAD[irNRSG] = 100*BIOS_NPHYSREC+(BIOS_NSEGM-1);
      BIOS_RECHEAD[irNPHYS]++;      /* # of several phys. records in one logical */
      BIOS_RECHEAD[irNWRD] = 0;     /* the number of words in this record segment */
      /* copy record segment header to I/O buffer */
      bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], RECHEADLEN*sizeof(int)); 
      bufout[1] = bufout[1] + RECHEADLEN;   /* update length info */
      JP = IP + RECHEADLEN;           /* set data header pointer in I/O buffer */
      ifree  = JP;            /* Ukazatel` na pervoe svobodnoe slovo v I/O bufere */ 
    }

    /* Coping data header to the I/O buffer to bufout[] */
    ncop = ev[jp];
    bcopy ( (char *)&ev[jp], (char *)&bufout[JP], ncop*sizeof(int) ); 
    bufout[1]         += ncop; /* update physical record length info */
    bufout[IP+irNWRD] += ncop; /* update number of words in this record segment */
    kp = jp + ncop;         /* set data pointer v  ET bufere */
    KP = JP + ncop;         /* set data pointer v I/O bufere */
    ifree  = KP;            /* Ukazatel` na pervoe svobodnoe slovo v I/O bufere */
 

    /************
     * add data *
     ************/

    do
    {
      /* copy as many data as possible */
      ncop = min(lenb,(bufout[0]-ifree)); 
      bcopy((char *)&ev[kp], (char *)&bufout[KP], ncop<<2); /* copy data */
      bufout[1]         += ncop; /* update physical record length info               */
      bufout[IP+irNWRD] += ncop; /* update number of words in this record segment    */
      kp             += ncop; /* set data pointer v  ET bufere                    */
      KP             += ncop; /* set data pointer v I/O bufere                    */
      ifree          += ncop; /* Ukazatel` na pervoe svobodnoe slovo v I/O bufere */
	  lenb           -= ncop; /* Ostalos` skopirovat` dannyh iz etogo banka       */
      /* bufout[JP+idNPREV] = 0; *//* words in previous datasegments                   */
      bufout[JP+idDATA]   = ncop; /* words in this datasegment                        */


      if(lenb<0)
      {
        printf("etWrite : impossible things happens : lenb<0\n");
        return(EBIO_INTERNAL);
      }
      if(lenb==0) /* All data copied -> goto next bank */
      {
        if (bufout[JP+idCODE]!=0) bufout[JP+idCODE]=3; /* kol` ne polny to posledniy */
        JP = ifree;                        /* ukazatel` na sleduyuschiy bank */
        jp = nextjp(jp);                   /* ukazatel` na sleduyuschiy bank */
        break;                             /* all data are written           */
      }
        

      /* if data are left, write both headers again and next piece of data     */
      if (bufout[JP+idCODE] <= 1) bufout[JP+idCODE]++; /* update data segment code   */
      dathead[idCODE]=bufout[JP+idCODE];
      if (bufout[IP+irCODE] <= 1) bufout[IP+irCODE]++; /* update record segment code */
      BIOS_RECHEAD[irCODE]=bufout[IP+irCODE];

      /* save dathead and correct it */
      bcopy ( (char *)&bufout[JP], (char *)dathead, lenh*sizeof(int));
      dathead[idNPREV] = kp - (jp+ev[jp]);      /* words in previous datasegments  */
      dathead[idDATA]  = 0;                     /* zapisano dannyh v etom segmente */
      WRITE_BUF_TO_DISK(descriptor);
      START_NEW_PHYSREC;
      BIOS_NSEGM--;

      /* Update record segment header */
      BIOS_RECHEAD[irNRSG] = 100*BIOS_NPHYSREC+(BIOS_NSEGM-1);
      BIOS_RECHEAD[irNPHYS]++;         /* # of several phys. records in one logical */
      BIOS_RECHEAD[irNWRD] = 0;        /* the number of words in this record segment */

      /* copy record segment header to I/O buffer */
      bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], RECHEADLEN*sizeof(int)); 
      bufout[1] = bufout[1] + RECHEADLEN;   /* update length info */
      JP = IP + RECHEADLEN;           /* set data header pointer in I/O buffer */
      ifree  = JP;            /* Ukazatel` na pervoe svobodnoe slovo v I/O bufere */

      /* copy data segment header to I/O buffer                                   */
      bcopy((char *)dathead, (char *)&bufout[ifree], lenh*sizeof(int));
      bufout[1]         += lenh; /* update length info in physrecord                 */
      bufout[IP+irNWRD] += lenh; /* update number of words in this record segment    */
      ifree          += lenh; /* ukazatel` na svobodnoe mesto                     */
      KP      = ifree;        /* set data pointer v I/O bufere                    */
    } 
    while(1);       /* end of add data loop */
  }
  while (jp < lrlen); /* end of loop for banks in ET buffer */

  if (bufout[IP+irCODE] != 0) bufout[IP+irCODE]=3; /* kol` ne polny to posledniy */
  bufout[1] = IP = ifree; /* Ustanovim ukazateli na konec zapisi */

  if (nfopen) return(EBIO_NEWFILEOPEN);
  return(error);
}

 
/******************************************************/
/*                                                    */
/******************************************************/
 
int
etFlush(BOSIO *descriptor)
{ 
  int error;
  int *bufout;
  BOSIO *BIOstream; 

  BIOstream = descriptor;
  bufout    = BIOS_BUFP;

  error = 0;
  if(IP != 0 && BIOS_CURSTA == 3) /* output buffer exist and we are in writing stage */
  {
    printf("etFlush: flush output buffer\n"); fflush(stdout);
    WRITE_BUF_TO_DISK(descriptor);
    START_NEW_PHYSREC; /* ??? */
    BIOS_NSEGM--;      /* ??? */
  }

  return(error);
}
