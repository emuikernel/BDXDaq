
/* BOS_format.c - CLAS-specific part of Event Builder */
/*     complaines: boiarino@jlab.org              */

/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, *
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@jlab.org   Tel: (757) 269-7030                                  *
 * CLAS collaboration 
 *      boiarino@jlab.org Tel: (757) 269-5795
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      
 *      $Log: BOS_format.c,v $
 *      Revision 1.5  2000/08/17 18:48:34  boiarino
 *      remove piece for crate #25
 *
 *      Revision 1.4  2000/06/29 15:41:04  boiarino
 *      add comment
 *
 *      Revision 1.3  2000/06/29 14:24:23  boiarino
 *      somebody's changes are implemented when I was in Moscow ...
 *
 *      Revision 1.2  1999/05/12 13:57:57  boiarino
 *      big revision - fixing bugs
 *
 *      Revision 1.1.1.1  1998/09/23 18:29:30  boiarino
 *      First release - Serguei Boiarinov
 *
 *      Revision 2.3  1996/10/29 19:03:09  heyes
 *      new rcServer
 *
 *      Revision 2.2  1996/09/19 17:16:06  heyes
 *      Support for BOS file format
 *
 *      Revision 2.1  1996/09/19 14:14:08  heyes
 *      Initial release
 *
 *      Revision 1.2  1996/08/29 17:03:58  heyes
 *----------------------------------------------------------------------------*/

#ifndef FMT
#define FMT NBOS
#endif
#define ERROR -1
#define OK 0
#define NOT_TYPE 1

#define IFDBG if(0)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "bosio.h"
#include "etbosio.h"
#include "CODA_format.h"


/* DDL stuff (read-only here) */
extern int nddl; /* see LINK_support.c and deb_component.c */
extern DDL ddl[NDDL]; /* see LINK_support.c and deb_component.c */



/*****************************************************************************
 *                      Record Header service                                *
 *****************************************************************************/

/******************************************************************************

 Routine     : BOS_decode_head - NOT CALLING ANY MORE !!!

 Parameters  : datap - input buffer

 Discription : Extracts total event length and event type from CODA header

******************************************************************************/
int
BOS_decode_head(unsigned long **datap, evDesc desc)
{
  unsigned long marker;
  
  desc->type   = ((*datap)[1] >> 16 ) & 0xff;
  desc->length = ((*datap)[0] - 1) << 2;
  marker = (*datap)[1] & 0xffff;

  *datap += 2;          /* CODA headers are two long words */

  if(marker == 0x10CC) return OK;  /* CODA event header must have 0x10CC as low 16-bits */ 
  else                 return ERROR;
}

/******************************************************************************

 Routine     : BOS_reserv_head

 Parameters  : datap - output buffer

 Discription : Reserve space for FPACK record header - 11 32-bits words

******************************************************************************/
int 
BOS_reserv_head(unsigned long **datap, evDesc desc)
{
  desc->head_start = *datap;
  *datap +=11;
}

/******************************************************************************

 Routine     : BOS_encode_head (for event type < 16 only)

 Parameters  : datap - output buffer
               decs  - local event decsriptor

 Discription : Creates FPACK record header

******************************************************************************/
int 
BOS_encode_head(unsigned long **datap, evDesc desc)
{
  int i;

  unsigned long *rechead = desc->head_start;
  /* ======= WRITING FPACK LOGICAL RECORD HEADER : 11 words ======= */

  rechead[irPTRN]  = SWAP/*67305985*/; /* 0x04030201 - Byte swap pattern */
  rechead[irFORG]  = LOCAL*16 + 8/*24*/;       /* Coda signature               */
  rechead[irNRSG]  = 0;           /* nrec*100+nseg - what we realy want here ? */
  if(desc->type == 0)
  {
    rechead[irNAME1]  = *((unsigned long *)"SCAL");  /*        Scalers          */
    rechead[irNAME2]  = *((unsigned long *)"ERS ");  /*         Event           */
    rechead[irNEVNT]  = desc->nSCALERS; /*desc->evnb;*/    /* Event number            */
  }
  else
  {
    rechead[irNAME1]  = *((unsigned long *)"RUNE");  /*        Physics          */
    rechead[irNAME2]  = *((unsigned long *)"VENT");  /*         Event           */
    rechead[irNEVNT]  = desc->nRUNEVENT; /*desc->evnb;*/   /* Event number            */
  }
  rechead[irNRUN]  = desc->runnb;                   /* Run number              */
  rechead[irNPHYS] = 0;                             /* Physical record inside logical = 0  */
/*printf("_encode_head: 0x%08x 0x%08x\n",desc->user[1],desc->trigbits);*/
  rechead[irTRIG]  = desc->user[1] = desc->trigbits;      /* trigger bits            */
  rechead[irCODE]  = 0;                             /* Segment code = 0        */
  rechead[irNWRD]  = *datap - rechead - 11;         /* Event length without record header  */






  /* set static variables to original values - this function calls last !!! */
/* ERROR: they are NOT STATIC any more !!!!! */

  desc->trigbits =   0; /* cleanup trig bits */
  desc->eventtype  = 1; /* set event type 1 - regular physics event */
  desc->latefail = 0;
  desc->rocpattern = 0; /* cleanup roc pattern bits */
  for(i=0; i<32; i++)
  {
    desc->rocstatus[i] = 0;
    desc->rocexist[i] = 0;
  }
}


/*****************************************************************************
                          HEAD bank service
 *****************************************************************************/

/******************************************************************************

 Routine     : BOS_decode_desc - NOT CALLING ANY MORE !!!

 Parameters  : datap - input buffer

 Discription : Extracts info from CODA descriptor

******************************************************************************/
int 
BOS_decode_desc(unsigned long **datap, evDesc desc)
{
  unsigned long marker;

  marker = (*datap)[1];
  desc->evnb = (*datap)[2];
  desc->type = (*datap)[3];
  desc->err[1] = (*datap)[4];
  /*
  printf("BOS_decode_desc:(*datap)[0] %d, marker %d, evnb %d, type %d, err %d\n",
		 (*datap)[0],marker,desc->evnb,desc->type,desc->err[1]);
  */
  *datap += 5;
  /*trigbits = desc->user[1] = 0;*/ /* cleanup trigger bits */

  if( marker == 0xC0000100 ) return OK;
  else return ERROR; 
}

/******************************************************************************

 Routine     : BOS_reserv_desc

 Parameters  : datap - output buffer

 Discription : Reserve space for HEAD bank - 18 32-bits words

 ******************************************************************************/
int 
BOS_reserv_desc(unsigned long **datap, evDesc desc)
{
  desc->desc_start = *datap;
  *datap +=18;
}

/******************************************************************************

 Routine     : BOS_encode_desc (for event type < 16 only); NOTE: this function
                            will be called BEFORE BOS_encode_head !!!

 Parameters  : datap - output buffer
               decs  - local event decsriptor

 Discription : Creates HEAD bank

******************************************************************************/
int 
BOS_encode_desc(unsigned long **datap, evDesc desc)
{
  int i;
  unsigned long *desc_start = desc->desc_start;
  
IFDBG printf("BOS_encode_desc reached: 0x%08x 0x%08x 0x%08x\n",
             desc,desc->desc_start,desc_start);fflush(stdout);
  
  /* check for syncronization on the roc level */
  if(desc->eventtype == 2)
  {
    for(i=0; i<32; i++)
    {
      if(desc->rocexist[i] && !desc->rocstatus[i])
      {
        desc->eventtype = 3;
      }
    }
  }
  else if(desc->latefail) /* check late fail */
  {
    /*printf("EB: late failed\n");*/
    desc->eventtype = 4;
  }

  /* check CODA flags */

  if(desc->syncev == 1)
  {
    if(desc->err[1] != 0) printf("EB: ERROR - roc mask from CODA is 0x%08x\n",
                                                        desc->err[1]);
  }

  /* update event counters */
  if(desc->type == 0)
  {
    desc->nSCALERS ++;
  }
  else
  {
    desc->nRUNEVENT ++;
  }

  /* FPACK DATA BLOCK HEADER for HEAD bank */

  desc_start[0]  = NFPHEAD;
  desc_start[1]  = *((unsigned long *)"HEAD");     /* bank name */
  desc_start[2]  = *((unsigned long *)"    ");
  desc_start[3]  = 0;           /* bank number */
  desc_start[4]  = 8;           /* the number of columns */
  desc_start[5]  = 1;           /* the number of rows */
  desc_start[6]  = 0;           /* data segment code */
  desc_start[7]  = 0;           /* #words in previous bank (0 here) */
  desc_start[8]  = 8;           /* #words in this bank */
  desc_start[9]  = *((unsigned long *)"I   ");     /* bank format */

  /* data for HEAD bank */

  desc_start[10] = 0;           /* version number */
  desc_start[11] = desc->runnb; /* run number */
  desc_start[13] = time(NULL);  /* event time */
  if(desc->type == 0)
  {
    desc_start[12] = desc->nSCALERS; /*desc->evnb;*/ /* event number */
    desc_start[14] = 10; /* scalers record */
  }
  else
  {
    desc_start[12] = desc->nRUNEVENT; /*desc->evnb;*/ /* event number */
    desc_start[14] = desc->eventtype; /* physics record */
  }
  desc_start[15] = desc->rocpattern;  /* ROC status */
  desc_start[16] = desc->type;  /* event classification (type from CODA 0-15,16-20) */
/*printf("_encode_desc: 0x%08x 0x%08x\n",desc->user[1],desc->trigbits);*/
  desc_start[17] = desc->user[1] = desc->trigbits;    /* event weight - trigger bits */

}

/*****************************************************************************
                         Bank header service
 *****************************************************************************/

/******************************************************************************

 Routine     : BOS_decode_frag
 
 *************** NOTE FOR OLD VERSION ***************
   for all types of events; NOTE: it will be
   called 2 times for first event in each buffer
 ****************************************************

 Parameters  : datap - input buffer

 Discription : Extracts info from CODA fragment header, fills 2-dim pointer
               array for all bank fragments are included in present CODA
               fragment

******************************************************************************/

int 
BOS_decode_frag(unsigned long **datap, evDesc desc)
{
  BossssheadPtr bos, endOfFrag;
  DDL *ddlptr;
  int count, i, tmp, id;
  unsigned int idnr, nrow, ndata, name;
  unsigned long *fb;
  unsigned long tgb  = *((unsigned long *)"+TRG");
  unsigned long syn  = *((unsigned long *)"+SYN");
  unsigned long sync = *((unsigned long *)"SYNC");
  char *ch;

  FILE *fd;
  unsigned long *start, *stop;


  desc->soe = *datap;                           /* save start fragment pointer */
  desc->length = ((*datap)[0] - 1) << 2;        /* extracts all useful      */
  desc->evnb   =  (*datap)[1] & 0xff;           /* info from CODA header -  */
  desc->type   = ((*datap)[1] >> 16) & 0xff;    /*  - first two long words  */
  desc->latefail = ((*datap)[1] >> 25) & 0x1;

  tmp=desc->type;

  
  IFDBG
  {
    if(desc->type == 0) printf("start BOS_decode_frag for SCALERS %d\n",desc->type); 
    printf("\nStart of BOS_decode_frag >>>>>>>>>>>>>>>>>>>>>>>\n"
             " Fragment length ((*datap)[0]) = %d words\n"
             " Event %d(0x%04x) from ROC#%d type = %d of desc->length %d\n",
             (*datap)[0],desc->evnb,desc->evnb,desc->rocid,desc->type,desc->length);
  }
  

  desc->rocexist[desc->rocid] = 1; /* set exist flag for this roc */
  if(desc->type >= 17 && desc->type <= 20) /* control fragment - never come here ?! */
  {
printf("BOS_format: NEVER COME HERE !!!\n");fflush(stdout);
    /* printf("BOS_decode_frag: got control fragment of type %i\n",desc->type); */
    desc->length = ((*datap)[0] - 1) << 2;
    desc->fragments[0] = &(*datap)[2];
    desc->bankTag[0] = desc->rocid;
    desc->fragLen[0] = desc->length;
    desc->bankCount = 1;
  }
  else if(desc->length == 0)               /* empty fragment */
  {
    IFDBG printf("BOS_format: EMPTY FRAGMENT\n");fflush(stdout);
    desc->fragments[0] = &(*datap)[2];
    desc->bankTag[0] = 0;
    desc->fragLen[0] = 0;
    desc->bankCount = 1;
  }
  else                                     /* physics event */
  {
    bos = (BossssheadPtr)&(*datap)[2];
    IFDBG
    {
      /* TT_PrintROCBuffer((*datap)[0]+1,*datap); */
    }
    desc->length = 8; /* First 2 4-bytes CODA words already decoded and SHOULD BE COUNTED !!! */
    desc->bankCount = 0;
    count = 0;
    endOfFrag = (BossssheadPtr) (&(*datap)[1] + (*datap)[0]);
    IFDBG printf("PHYSICS EVENT: type=%d bos=%d endOfFrag=%d\n",desc->type,(int)bos,(int)endOfFrag);

    /* That's very bad - we process several ROC's and trigbits gets overwritten by zero */
    /* desc->user[1] = 0; *//* cleanup trigger bits */

    do
    {
      /* get info from 2-word bank header and check if it is make sense */
      idnr = bos->idnr;
      id = (idnr>>16)&0xFFFF;
      nrow = bos->nrow;
      if(id>=NDDL || id<=0)
      {
        printf("BOS_decode_frag ERR: idnr=0x%08x id=%d\n",idnr,id);
        printf("BOS_decode_frag : bos=0x%08x, idnr=0x%08x id=%d nrow=%d\n",
                    bos,idnr,id,nrow);
        fflush(stdout);
        return(ERROR);
	  }

      /* pointer to the bank description in 'ddl' area */
      ddlptr = (DDL *)&ddl[id];
      IFDBG printf("===> id=%d ddlptr=0x%08x 0x%08x\n",id,ddlptr,ddl);

      /* calculate bank length in words */

if(id > NDDL)
{
  printf("ERROR: id=%d\n",id);
  fflush(stdout);
}

if(ddl[id].lfmt <= 0)
{
  printf("ERROR: id=%d lfmt=%d\n",id,ddl[id].lfmt);
  fflush(stdout);
}

      ndata = (nrow * ddlptr->ncol * (4 / ddl[id].lfmt) + 3) / 4;
      IFDBG printf("111: idnr=0x%08x id=%d nrow=%d ndata=%d\n",
              idnr,id,nrow,ndata);

      desc->bankTag[count] = idnr;
      desc->fragments[count]  = (unsigned long *)bos;
      desc->fragLen[count]    = ((ndata + NHEAD)<<2);
      desc->user[0] = 0;

	  /* MAKE NEW ONE FOR 2-WORD HEADER
      if(!TT_CheckROCBOSDatHead((int *)bos))
      {
        printf("\nBOS_decode_frag: Found ERROR in BOS Bank !!!\n");
        printf("===> desc->rocid=%d desc->evnb=%d desc->type=%d desc->length=%d\n",
                                          desc->rocid,desc->evnb,desc->type,desc->length);
        TT_PrintROCBOSBankHead(bos); 
      }
	  */

      if(ndata == 0) /* discarding bank with zero length */
      {

        if((fd = fopen("/tmp/BOS_format.log","a")) != NULL)
        {
          start = (unsigned long *) &(*datap)[0];
          stop = (unsigned long *) (&(*datap)[1] + (*datap)[0]);
          fprintf(fd,"BOS_decode_frag: zero BOS bank length !!!\n");fflush(fd);
          fprintf(fd,"BOS_decode_frag: something wrong, so print entire buf\n"
                   " start=0x%08x, stop=0x%08x, nwords=%d, desc->length=%d\n",
                   start,stop,stop-start,desc->length/4);
          do
          {
            fprintf(fd,"0x%08x %10d >%4.4s<\n",*start,*start,start);
            start++;
          } while (start < stop);
          fclose(fd);
        }

        printf("BOS_decode_frag: zero BOS bank length !!!\n");
		/* MAKE NEW ONE FOR 2-WORD HEADER
        TT_PrintROCBOSBankHead(bos);
		*/
        puts ("BOS_decode_frag: Discarding BOS Bank with zero length ...");
        desc->length           += (NHEAD<<2);
        desc->fragLen[count]    = 0;
        desc->bankTag[count]    = 1;
        bos++;
        count++;
        continue;
      }

      /* get bank name */
      ch = (char *)&name;
      for(i=0; i<ddlptr->nname; i++) ch[i] = ddlptr->name[i];
      for(i=ddlptr->nname; i<4; i++) ch[i] = ' ';

      /*********************************/
      /*********************************/
      /* special cases - temporary !!! */


      desc->eventtype = 1;      /* physics event (default value) */
      desc->trigbits = 0;       /* set it to 0; used as 'OR' in EB */
      if(name == syn || name == sync) /* if +SYN or SYNC bank - set event type 2 in HEAD bank */
      {
        desc->eventtype = 2;              /* set event type 2 - sync physics event */
        desc->rocstatus[desc->rocid] = 1; /* set sync flag for this roc */
      }

      if(name == sync) /* if SYNC bank - set corresponding bit in roc pattern */
      {
        printf("EB: WARNING - resyncronization in crate controller number %2d ... fixed.\n",desc->rocid);
        desc->rocpattern = 1; /* TEMPORARY set lowest bit !!! */
      }

      if(name == tgb) /* get trigger bits from +TRG bank and discard it */
      {
        desc->trigbits = *(((long *)&bos->nrow)+1); /* first word after bos->ndata -> record header */
        desc->user[1] = desc->trigbits; /* pass to Graham trigger bits to be put in fev.ctlb2 */
/*printf("_decode_frag: 0x%08x 0x%08x\n",desc->user[1],desc->trigbits);*/
      }

      /*********************************/
      /*********************************/
      /*********************************/



      /* pass to Graham the length of Header in longwords, 
         he need it for splitted banks only, but we'll pass it for any */
      desc->user[0] = NHEAD;

      desc->length += desc->fragLen[count];
      bos = (BossssheadPtr)((int)(bos+1)+(ndata<<2)); /* next bank frag */
      count++;
      if(count > 31)
      {
        printf("EB: ERROR !!! count = %d in roc = %d\n",count,desc->rocid);
        break;
      }
	
    } while (bos < endOfFrag);

    desc->bankCount = count; /* the number of bank fragments */

  }

  /* That is workaround of problem with garbage */
  if(desc->length > 0)
  {
    IFDBG printf ("Start at 0x%8.8X End at 0x%8.8X bos=0x%8.8X -> len=%d desc_length=%d bytes\n",
		  desc->soe,endOfFrag,bos,(int)endOfFrag-(int)(desc->soe),desc->length);
    desc->length = (int)endOfFrag-(int)(&(*datap)[2]); 
  }

  IFDBG {
	printf("End of BOS_decode_frag <<<<<<<<<<<<<<<<<<<<<<<\n");
	if(desc->type == 0) printf("stop BOS_decode_frag for SCALERS %d\n",desc->type);
  }
  
   /* fragment headers have a variable format and */
   /* are hard to check, we assume here that if   */
   /* the length is resonable then so is the rest */
   /* of the event. NOTE: limit 0.5MBy.           */


  if(desc->length < 800000)  return OK;                
  else
  {
    printf("ERROR: desc->length=%d bytes, limit is 800000 bytes\n",desc->length);
    return ERROR;
  } 
 
}

/******************************************************************************

 Routine     : BOS_reserv_frag

 Parameters  : datap - output buffer

 Discription : Reserve space for bank header - 10 long words

******************************************************************************/
int 
BOS_reserv_frag(unsigned long **datap, evDesc desc)
{
/* save fragment address to be used in BOS_encode_frag */
  desc->frag_start = *datap;
  desc->head_length = NFPHEAD; /* send FPACK header length so appropriate
                                  space will be reserved */
}

/******************************************************************************

 Routine     : BOS_encode_frag (for event type < 16 only)

 Parameters  : datap - output buffer
               decs  - local event decsriptor

 Discription : Creates bank header

******************************************************************************/

int 
BOS_encode_frag(unsigned long **datap, evDesc desc)
{
  unsigned long tgb  = *((unsigned long *)"+TRG");
  FpackheadPtr fpack;
  BossssheadPtr bos;
  DDL *ddlptr;
  int i, nbytes, id;
  unsigned int idnr, nrow, ndata, name;
  char *ch;

  /*if(desc->type == 0) printf("start BOS_encode_frag for SCALERS\n");*/

  /* skip banks with zero data length - discarded banks !!! */
  if(desc->data_length == 0)
  {
    if(desc->type == 0) printf("exit BOS_encode_frag for SCALERS\n");
    return(0);
  }

  /* set 'fpack' and 'bos' pointers to the begining of fragment,
  although may not need both of them */
  fpack = (FpackheadPtr)desc->frag_start; 
  bos = (BossssheadPtr)desc->frag_start; 

  /* if bank appears first time, get info from bos header;
  if not the first time (secondary parts of splitted banks),
  skip it and goto fpack->ndata correction */

  /* to recognize bos header from fpack one, use first word; for
  bos header it contains bank id in 2 highest bytes; bank id
  always > 0 so first word always > 0xFFFF; in case of fpack
  header first word contains header length, which typicaly
  equal to 10; it can be bigger if bank has long format
  description, but it is unlikely; anyway, if format description
  will be bigger then (0xFFFF-10) words, this program will fail */
  idnr = bos->idnr;
  IFDBG printf("BOS_encode_frag: idnr = 0x%08x\n",idnr);
  if(idnr > 0xFFFF) /* bos header, so bank appears for the first time */
  {
    id = (idnr>>16)&0xFFFF;
    nrow = bos->nrow;
    ddlptr = (DDL *)&ddl[id];

    if(ddl[id].lfmt<=0)
    {
      printf("ERROR: ddl[id].lfmt=%d, id=%d, idnr=0x%08x, nrow=0x%08x\n",
        ddl[id].lfmt,id,idnr,nrow); fflush(stdout);
    }
    else
    {
      ndata = (nrow * ddlptr->ncol * (4 / ddl[id].lfmt) + 3) / 4;
    }

    IFDBG printf("222: 0x%08x %d %d %d\n",idnr,id,nrow,ndata);

    /* get bank name and correct it if TGBI */
    ch = (char *)&name;
    for(i=0; i<ddlptr->nname; i++) ch[i] = ddlptr->name[i];
    for(i=ddlptr->nname; i<4; i++) ch[i] = ' ';
    /* '+TRG' -> 'TGBI' */
    if(name == tgb) name = *((int *)"TGBI");

    /* fill fpack header */
    fpack->nhead  = NFPHEAD;
    fpack->namext = *((unsigned long *)"    ");
    fpack->ncol   = ddlptr->ncol;
    fpack->nrow   = nrow;
    fpack->ndata  = ndata;
    fpack->name   = name;
    fpack->nr     = idnr&0xFFFF;
    fpack->code   = 0;
    fpack->nprev  = 0;
    ch = (char *)&fpack->frmt;
    for(i=0; i<ddlptr->nfmt; i++) ch[i] = ddlptr->fmt[i];
    for(i=ddlptr->nfmt; i<4; i++) ch[i] = ' ';

    IFDBG
    {
      printf("===nhead :  %d\n",fpack->nhead);
      printf("===namext: >%4.4s<\n",&fpack->namext);
      printf("===ncol  :  %d\n",fpack->ncol);
      printf("===nrow  :  %d\n",fpack->nrow);
      printf("===ndata :  %d\n",fpack->ndata);
      printf("===nr    :  %d\n",fpack->nr);
      printf("===frmt  : >%4.4s<\n",&fpack->frmt);
      printf("===name  : >%4.4s<\n",&fpack->name);
    }
  }
  else /* fpack header already exist, so we need to correct fpack->ndata */
  {

    /*printf("!!!!!!!!!!!!!!! %d %d\n",fpack->ndata,desc->data_length);*/
    /* data length correction for splitted bank */

    if(fpack->ndata != desc->data_length) /* always true for splitted ??? */
    {
      IFDBG printf("splitted banks !! %d %d\n",fpack->ndata,desc->data_length);
      if(fpack->ndata != 0)
      {
        nbytes = (fpack->nrow * fpack->ncol) / fpack->ndata;
        IFDBG printf("BOS_encode_frag : nbytes=%i\n",nbytes);
      }
      else
      {
        printf("BOS_encode_frag :: ERROR :: fpack->ndata = 0\n");
        nbytes = 0;
      }
      fpack->ndata = desc->data_length;
      if (fpack->ncol != 0)
      {
        fpack->nrow = (nbytes * fpack->ndata) / fpack->ncol;
      }
      else
      {
        printf("BOS_encode_frag :: ERROR :: fpack->ncol = 0 ; set to 1\n");
        fpack->ncol = 1;
        fpack->nrow = (nbytes * fpack->ndata) / fpack->ncol;
      }
    }
  }

  /* if(desc->type == 0) printf("stop BOS_encode_frag for SCALERS\n"); */

}

/*****************************************************************************
                        SPECIAL EVENTS SERVICE
 *****************************************************************************/

/******************************************************************************

 Routine     : BOS_decode_spec (for event type 16-20 only)

 Parameters  : 

 Discription : 

******************************************************************************/
int 
BOS_decode_spec(unsigned long **datap, evDesc desc)
{
  unsigned long marker;

/*  desc->length = ((*datap)[0] - 1) << 2;*/ /* data length in spec fragment */
/*  desc->length = 0;*/ /* destroy spec fragment because FPACK logical header will
                           be created during BOS_encode_spec */ /* This doesn't work !!!!*/

  desc->length = ((*datap)[0] - 1) << 2;
  desc->type   = ((*datap)[1] >> 16) & 0xff;

  marker = (*datap)[1] & 0xffff;

  desc->time = (*datap)[2];

  /* printf ("BOS_decode_spec: event of type %i\n",desc->type); */
  switch(desc->type)
  {
    case 16:
    {
      desc->syncev = (*datap)[3];
      desc->evnb = (*datap)[4];
      desc->err[1] = (*datap)[5];
    } 
    /* break; ??? */
    case 17: 
    {
      printf("BOS_format: clean up event counters.\n");
      /*desc->nRUNEVENT = 0;*/ /*  does not matter, will be      */
      /*desc->nSCALERS  = 0;*/ /*  rewhitten in deb_component.c  */

      desc->runnb = (*datap)[3];
      desc->runty = (*datap)[4];
    }
    /* break; ??? */
    default: 
    {
      desc->evnb = (*datap)[4];
    }
  }
  *datap += ((*datap)[0] + 1); /* skip spec fragment */

  if(marker == 0x01CC)
  {
    return OK;
  }
  else
  {
    return ERROR;
  }  
}

/******************************************************************************

 Routine     : BOS_encode_spec (for event type 16-20 only)

 Parameters  : 

 Discription : 

******************************************************************************/
int 
BOS_encode_spec(unsigned long **datap, evDesc desc)
{
  unsigned long *rechead=(*datap);

printf("BOS_encode_spec reached\n");fflush(stdout);

  /* update event counter */
  desc->nRUNEVENT ++;

  /* record header */
  rechead[irPTRN]  = SWAP/*67305985*/;    /* 0x04030201 - Byte swap pattern            */
  rechead[irFORG]  = LOCAL*16 + 8/*24*/;          /* Coda signature                            */
  rechead[irNRSG]  = 0;           /* nrec*100+nseg - what we realy want here ? */
  rechead[irNAME1] = *((unsigned long *)"RUNE");   /*        Physics          */
  rechead[irNAME2] = *((unsigned long *)"VENT");   /*         Event           */
  rechead[irNRUN]  = desc->runnb;                  /* Run number              */
  rechead[irNEVNT] = desc->nRUNEVENT; /*desc->evnb;*/    /* Event number            */
  rechead[irNPHYS] = 0;                            /* Physical record inside logical = 0  */
  rechead[irTRIG]  = desc->type;                   /* trigger bits = 0 for special events */
  rechead[irCODE]  = 0;                            /* Segment code = 0        */
  rechead[irNWRD]  = 18;                           /* Event length without record header  */


  /* HEAD bank */

  /* header for HEAD bank */
  (*datap)[11]  = NFPHEAD;
  (*datap)[12]  = *((unsigned long *)"HEAD");     /* bank name */
  (*datap)[13]  = *((unsigned long *)"    ");
  (*datap)[14]  = 0;          /* bank number */
  (*datap)[15]  = 8;          /* the number of columns */
  (*datap)[16]  = 1;          /* the number of rows */
  (*datap)[17]  = 0;          /* data segment code */
  (*datap)[18]  = 0;          /* #words in previous bank (0 here) */
  (*datap)[19]  = 8;          /* #words in this bank */
  (*datap)[20]  = *((unsigned long *)"I   ");     /* bank format */

  /* data for HEAD bank */
  (*datap)[21] = 0;            /* version number */
  (*datap)[22] = desc->runnb;  /* run number */
  (*datap)[23] = desc->nRUNEVENT; /*desc->evnb;*/ /* event number */
  (*datap)[24] = time(NULL);   /* event time */
  (*datap)[25] = 0;            /* event type */
  (*datap)[26] = desc->rocid;  /* ROC status */
  (*datap)[27] = desc->type;   /* event classification */
  (*datap)[28] = desc->type;   /* event weight - trigger bits */

  /* write length and shift pointer */

  desc->length = 116; /* 29 << 2 */
  *datap += 29; /* 11 + 18 */

printf("BOS_encode_spec done: len=%d\n",desc->length);fflush(stdout);

}

