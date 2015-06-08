h07623
s 00007/00005/00424
d D 1.2 03/04/17 16:46:49 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/ddgetlogrec.c
e
s 00429/00000/00000
d D 1.1 00/08/10 11:10:11 boiarino 1 0
c date and time created 00/08/10 11:10:11 by boiarino
e
u
U
f e 0
t
T
I 1


I 3
#include <stdio.h>
E 3
#include "bosio.h"
#include "ioincl.h"

/*#define COND_DEBUG */ 
/*#define DEBUG   */
#include "dbgpr.h"


#define TRUE 1
#define FALSE 0

  
/**********************************************
 *                                            *
 **********************************************/
 
int 
D 3
ddGetLogRec(int descriptor, int **bufp, int *evlen, int *freeflag)
E 3
I 3
ddGetLogRec(BOSIO *descriptor, int **bufp, int *evlen, int *freeflag)
E 3
{
  BOSIOptr BIOstream;
  int ip, jp, kp, conv, segm=0;
  int *w, *bufin, *tmpbp;
  int i,ii,error,nskrk;
  int ifree, ildh, len;
  int dathead[70], jbuf[12]; /* intermediate buffers */
  char *fmt;

  
  DPR("\nSTART OF ddGetLogRec >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
 
D 3
  BIOstream = (BOSIOptr)descriptor;	/* set pointers */
E 3
I 3
  BIOstream = descriptor; /* set pointers */
E 3
 
  error = 0;
  DPR1(">>> At the beginning IP=%i\n",IP);
/**********************************************
 * check unit status (for sequential IO only) *
 **********************************************/
 
  if(BIOS_ACCESS == 1)
    {
      if(BIOS_CURSTA == 0 || BIOS_CURSTA == 5) /* undef or rewind - go to read */
        {
          BIOS_INNUM = 0;
          IP = JP = KP = 0;
          BIOS_CURSTA = 1;
        }
      else if(BIOS_CURSTA == 2)
        {
          return(EBIO_EOFREAD);
        }
      else if(BIOS_CURSTA == 4)	/* rewind and goto read */
        {
          BIOS_INNUM = 0;
          IP = JP = KP = 0;
          bosRewind(descriptor);
          BIOS_CURSTA = 1;
        }
      else if(BIOS_CURSTA != 1)
        {
          return(EBIO_WRONGSTATUS);	/* error in calling order */
        }
    }
  
  /***************
   * key reading *
   ***************/
  
  if(IP == 0)	/* allocate input buffer */
    {
      i = 0;
      ii = 12;
      do
        {
          DPR(">>> First time reading -> determine RECL \n ");
          error = bosinn(descriptor,&ii,jbuf);
          if(error < 0) {
            DPR1("ddGetLogRec : bosinn returns error = %i when determine RECL\n",error);
            return(EBIO_EOF);   /* end of file */
          }
          bosRewind(descriptor);
          /*    call fswap(jbuf,12) */
          i++;
        } while(error != 0 && i < 10);
      BIOS_CURSTA = 1;
      BIOS_RECL = jbuf[0]*sizeof(int);
      BIOS_BUFP = bufin = (int *)malloc(BIOS_RECL);
      DPR1(">>> RECL = %i\n",BIOS_RECL);
      IP = 0;
    }
  bufin = BIOS_BUFP;
  DPR1(">>> IP=%i\n",IP);

  do
    {
      if(IP != 0 && IP < bufin[1] )
        {   /* Ne nachalo fiz. zapisi i pervy  segment vnutri 
               uzhe schitannogo kuska. Perehodim na novy segment */
            DPR2(">>> O.K. new Logical record inside old physical one : bufin[1]=%i > IP=%i\n"
                ,bufin[1],IP);
			PRRH(&bufin[IP]);
            DPR1("\n========================= RECORD SEGMENT in bufin[%i] ==========================\n",IP);
            PRLR(&bufin[IP]);
            DPR1("========================= RECORD SEGMENT in bufin%i] ==========================\n\n",IP);
			
        }
      else
        {   
            
            /* Fizich. zapis` konchilas`, chitaem next */
            IP = 0;                     /* Ukazatel` na nachalo */
            ii = BIOS_RECL/sizeof(int);  /* dlina Fizich. zapisi */
            DPR(">>> Search for Begining of Logical record \n ");
            error = bosinn(descriptor,&ii,bufin);
            PRPR(bufin);
            if(error ==EBIO_EOF) { 
              IP=bufin[1];                   /* Force end of phisical record*/
              DPR1("ddGetLogRec : bosinn returns error = %i\n",error);
              return(error); 
            }
            if(error != 0)
              { DPR1("ddGetLogRec : bosinn returns error = %i\n",error);return(error); }
            /*    call fswap(bufin,0) */
            
            PRPR(bufin);
            DPR("\n========================= RECORD SEGMENT in bufin[2] ==========================\n");
            PRLR(&bufin[2]);
            DPR("========================= RECORD SEGMENT in bufin[2] ==========================\n\n");
            
            conv = -1;	                /* record not yet converted */
            IP = 2;	                    /* successful read - reset pointer 
                                           na nachalo Log. zapisi */
        }
      /* kopiruem record segment header iz bufin v rechead */
      bcopy((char *)&bufin[IP], (char *)BIOS_RECHEAD, 44);
      /* Segment code = 0..3 */
      if(BIOS_RECHEAD[9] < 0 || BIOS_RECHEAD[9] > 3) return(EBIO_WRONGSEGMCODE);
      
    }     /* Poka Segment code = 2 ili 3 (t.e. poka ne naydyom pervy segment) */
  while(BIOS_RECHEAD[9] != 0 && BIOS_RECHEAD[9] != 1);
  
  /* My v nachale pervogo segmenta Log. zapisi (vozmozhno on i posledniy ) */
  JP = error = 0;

  tmpbp = &bufin[IP];
  
  if (BIOS_RECHEAD[9] == 1)  /* NE Polny segment */
    {                   /* Combine record from segments */
  
      DPR(">>> Begin combining record from segments\n");
      /* Allocate buffer to keep the maximum record */
      DPR1(">>> Allocating st->recl*100=%i bytes for tmpbp\n",BIOS_RECL*100);
      if (segm==0) tmpbp = (int *)malloc(BIOS_RECL*(segm+2));
	  else         tmpbp = (int *)realloc(tmpbp,BIOS_RECL*(segm+2));
	  segm++;
		  
      DPR1(">>> Out of malloc tmpbp=%X\n",tmpbp);
      if (tmpbp==NULL) 
        {
            printf("ddGetLogRec : fail to allocate %i bytes --> ABORT !\n"
                   ,BIOS_RECL*(segm+2));
            exit(1);
        }
  
      /* kopiruem segment v bufer */
      len = bufin[IP+(11-1)]+11; /* length of record segment */
      DPR4(">>> Coping from &bufin[IP]=%X to tmpbp=%X %i bytes (IP=%i)\n",
             &bufin[IP],tmpbp,len*sizeof(int),IP);
      bcopy ( (char *)(&bufin[IP]), (char *)tmpbp, len*sizeof(int) ) ; 
      /* search last data header */
      ildh = ii = 11;       /* First data header in tmpbp */
      ifree = bufin[1] - IP;  /* Ukazatel` na free space v tmpbp */
      DPR(">>> Search last data header\n");
      while (ii < ifree)
        {
          ildh = ii; 
          DPR2(">>> Last data header index = %i , free space index = %i\n"
               ,ildh,ifree);
          PRDH(&tmpbp[ildh]);
          ii += tmpbp[ildh]+tmpbp[ildh+8];
        }
      DPR1(">>> Last data header at index %i\n",ildh);
      /* Join parts of splitted logical record */
      while (BIOS_RECHEAD[9] != 3) /* Ne posledniy segment */
        {
          /* Chitaem sleduyuschiy segment */
          ii = BIOS_RECL/sizeof(int);      /* dlina Fizich. zapisi */
          
          DPR(">>> Read next segment\n");
          error = bosinn(descriptor,&ii,bufin);
		  if (error!=0) {
              DPR1("ddGetLogRec : bosinn returns error = %i\n",error);
              DBGOFF;
              if (error==EBIO_EOF) IP=bufin[1]; /* Forse end of phisical record*/
			  return error;
		  }
          tmpbp = (int *)realloc(tmpbp,BIOS_RECL*(segm+2));
          if (tmpbp==NULL) {
             printf("ddGetLogRec : fail to allocate %i bytes --> ABORT !\n"
                    ,BIOS_RECL*(segm+2));
             exit(1);
          }
          segm++;
          

          DPR("\n========================= RECORD SEGMENT in bufin[2] ==========================\n");
          PRLR(&bufin[2]);
          DPR("========================= RECORD SEGMENT in bufin[2] ==========================\n\n");
          IP = 2;
   	      PRPR(bufin);
          if (error  < 0) 
            {DPR1("ddGetLogRec : bosinn returns error = %i\n",error);return(error);}   /* end of file */ 
          if (error != 0) 
            {DPR1("ddGetLogRec : bosinn returns error = %i\n",error);return(error); }
          /* kopiruem record segment header */
          bcopy ( (char *)&bufin[2], (char *)BIOS_RECHEAD, 44 );
          /* Check segment code */
          if (BIOS_RECHEAD[9] < 0 || BIOS_RECHEAD[9] > 3) return(EBIO_WRONGSEGMCODE);
          if (BIOS_RECHEAD[9] < 2 ) return (EBIO_NOTCONTCODE);
          /* yavlyaetsya li pervy data segment prodolzheniem predyduschego */
          ii = 2 + 11 + (7-1); /* eto index data segment coda pervogo banka*/
          if ( bufin[ii] < 0 || bufin[ii] > 3 )     return(EBIO_WRONGSEGMCODE);
          DPR(">>> Pervy Data block v novoy fizzapisi :\n");
          PRDH(&bufin[2 + 11]);
          JP = IP + 11;           /* eto nachalo 1'go headera dannyh       */
          KP = JP + bufin[JP];      /* eto nachalo 1'go banka dannyh         */
          if ( bufin[ii] > 1 ) 
            { /* Da, yavlyaetsya */
              DPR(">>> O.K. - eto prodolzhenie predyduschego\n");
              /* kopiruem segment v bufer bez 2-x pervyh headerov */
              len = bufin[IP+(11-1)]-bufin[JP]; /* eto dlina segmenta zapisi v slovah */
       	      DPR3(">>> Kopiruem segment v bufer bez 2-x pervyh headerov from bufin[%i] to tmpbp[%i] %i words\n"
                  ,KP,ifree,len);
              bcopy ( (char *)&bufin[KP], (char *)&tmpbp[ifree], len*sizeof(int) );
              /* podstraivaem last data header v predyduschem segmente zapisi */
              ii = ildh+(7-1); /* chech data segment code, it should be incomplete */
              if ( tmpbp[ii] != 1 && tmpbp[ii] != 2 )     return(EBIO_NOTCONTCODE);
              /* Number of words should be the same */
              if ( tmpbp[ildh+(9-1)] != bufin[2+11+(8-1)] ) return(EBIO_BANKHEADINTEGR);
              tmpbp[ildh+(9-1)] += bufin[2+11+(9-1)];
              /* Podstraevaem ukazateli */
              ifree += len;
              /* Podstraevaem segment code */
              tmpbp[ildh+(7-1)] = 0;
              /* search last data header */
              ii = ildh;
              DPR(">>> Again Search last data header\n");
              while (ii < ifree)
                {
                  ildh = ii; 
                  DPR2(">>> Last data header index = %i , free space index = %i\n"
                       ,ildh,ifree);
                  PRDH(&tmpbp[ildh]);
                  ii += tmpbp[ildh]+tmpbp[ildh+8];
                }
              DPR1(">>> Last data header at index %i\n",ildh);
            }
          else
            { /* Net, eto pervy ili polny segment dannyh */
              DPR(">>> Pervy bank novoy zapisi - pervy ili polny bank dannyh\n");
              /* kopiruem segment v bufer bez pervogo headera */
              len = bufin[IP+(11-1)] ; /*  dlina segmenta zapisi v slovah */
              bcopy ( (char *)&bufin[JP], (char *)&tmpbp[ifree], len*sizeof(int) );
              /* Podstraevaem ukazateli */
              ifree += len;
              /* search last data header */
              ii = ildh;
              while (ii < ifree)
                {
                  ildh = ii;
                  ii += tmpbp[ildh]+tmpbp[ildh+8];
                }			  
        	}
          /* podstraivaem record header */
          tmpbp[11-1] = ifree - 11;
      } /* end while not last segment*/
      /* Zapomnim ukazateli na nachalo sleduyuschey logicheskoy zapisi */
      DPR2(">>> Zapomnim ukazatel` na nachalo novoy zapisi ; Old IP=%i, New IP=%i\n",IP,NEXTIP(IP));
      IP = NEXTIP(IP); 
    } 
  else
    {
        DPR(">>> Not splitted LOgical record -> working in bufin\n");
    }/* end if NEPOLNY SEGMENT */
  
  /* tmpbp ukazyvaet libo na bufin[2] esli logicheskaya zapis` polnost`yu
   umestilas` v fizicheskoy; libo na allocirovanny bufer v kotorom eta 
   zapis` byla sobrana iz neskol`kih kuskov */
  /* Opredelim dlinu logical recorda v bytah*/
  DPR("\n========================= RESULTING RECORD in tmpbp ==========================\n");
  PRLR(tmpbp);
  DPR("========================= RESULTING RECORD in tmpbp ==========================\n\n");
  
  len = 11 + tmpbp[11-1] ;
  /* Chistim record header dlya CODY */
  tmpbp[2-1] = 24;
  tmpbp[3-1] = tmpbp[3-1]/100*100;
  tmpbp[8-1] = tmpbp[10-1] = 0;

  if (tmpbp != &bufin[IP]) 
    {  /* Esli allocirovali vremenny bufer dlya sborki logical recorda 
          osvobodim ego */
	  DPR(">>> Freeing temporary buffer tmpbp\n");
      *freeflag = TRUE;
    }
  else
    {
      /* ip na nachalo cleduyuschego segmenta */
      IP = NEXTIP(IP); 
      *freeflag = FALSE;
    }	  
  
  DPR1(">>> At end IP=%i\n",IP);
  DPR("END OF ddGetLogRec <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
  DBGOFF;

  *bufp  = tmpbp;
  *evlen = len;

  return(error);
}


#include <stdio.h>

#include "bosio.h"


/* #define COND_DEBUG */
/* #define DEBUG  */
#include "dbgpr.h"

D 3
int bosinn(int descriptor, int *ntot, int *buf)
E 3
I 3
int
bosinn(BOSIO *descriptor, int *ntot, int *buf)
E 3
{
D 3
  BOSIOptr st;
E 3
I 3
  BOSIO *st;
E 3
  int i, error, ircn, nskip, nbytes;

D 3
  st = (BOSIOptr)descriptor;
E 3
I 3
  st = descriptor;
E 3
  error = 0;

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
      error = bosnres(descriptor,st->client,st->stream,1);
      if(error != 0) return(EBIO_IOERROR);	/* reconnection failed */
    }
a:
    readc(&st->client,&st->stream,&nskip,&nbytes,buf,&error);
    if(error == EBIO_EOF)	/* network channel is dead, try to reconnect */
    {
      error = bosnres(descriptor,st->client,st->stream,1);
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

    if(error != 0) DPR1("bosinn : cget returns error = %i\n",error);
    if(error == EBIO_EOF)
    {
      if(st->splitmb == 0 && st->splitev == 0) /* splited files not allowed */
      {
        /*printf("bosinn: end-of-file-1 >%s<\n",st->finame);*/
        return(EBIO_EOF);
      }
      else /* trying to open next portion of the splited file */
      {
        printf("bosinn: Trying to ReOpen file ...\n");
        st->reopen = 1;
        if ( (error=BOSREOPEN(descriptor)) != 0)
        {
          if(error == -1) printf("bosinn: end of file in BOSREOPEN\n");
          printf("bosinn: error ReOpen file >%s< : %i\n",st->finame,error);
          return(error); /* in case of opening error return end-of-file */
        }
        else
        {
          printf("bosinn: INFO: ReOpened file >%s<\n",st->finame); fflush(stdout);
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
 
  return(error);
}
 
E 1
