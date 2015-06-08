
/* fpack.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bosio.h"

/* #define COND_DEBUG  */
/* #define DEBUG */
#include "dbgpr.h"

#define	MUNITS	100

typedef struct bosoldstruct *OLDptr;
typedef struct bosoldstruct
{
  chptr	daname;
  BOSIO *descriptor;
} OLD;

static	OLDptr	bosold[MUNITS];

int
fparm_(char *string, int len)
{
  BOSIO *descriptor;
  char *s, *ss;
  int res;
  static char *stmp;
  static cont = 0;

  while(string[len-1]==' ') len--; /* remove traling spaces */
  s = (char *) MALLOC(len+1);
  bcopy((char *)string, (char *)s, len);
  s[len] = '\0';

  if(cont) /* it is not a first piece ! */
  {
    ss = strdup(stmp);
    FREE(stmp);
    stmp = (char *) MALLOC(strlen(ss) + len + 1);
    bcopy((char *)ss, (char *)stmp, strlen(ss));
    stmp[strlen(ss)] = '\0';
    strcat(stmp,s);
    FREE(ss);
    FREE(s);
    len = strlen(stmp);
    /*printf("fparm_: second stmp=%s\n",stmp);*/
  }
  else /* first piece */
  {
    stmp = s;
    /*printf("fparm_:  first stmp=%s\n",stmp);*/
  }

  if(stmp[len-1] == '&') /* is it last piece ? */
  {
    stmp[len-1] = '\0';
    cont = 1;
    return(0);
  }
printf("fparm_: ->%s<-\n",stmp);
  cont = 0;
  res = FParm(stmp, &descriptor);
  FREE(stmp);
  if (res != 0) exit(1);
  return(res);
}


int
FParm(char *string, BOSIO **descriptor)
{
  char *s=NULL, *cp=NULL, *fi=NULL, *fl=NULL;
  int i, nch, first, unit, recl, status, access, medium, splitmb=0, len, error=0;
  char *ch=NULL, *file=NULL, *flag="r", *daname=NULL, *fpinput="FPINPUT";
  BOSIO *LUN;

  s = strdup(string); /* save input string because will change it */ 
  len = strlen(s);
  while(s[len-1]==' ') len--; /* remove trailing spaces */
                              /* ERROR: NEED CHECK FOR len<0 !!!!!!!!! */

  i = 0;
  while(s[i]==' ') i++; /* skip spaces in the begining of string */
  { int j; j=i; while(isalpha(s[j])) {s[j] = toupper(s[j]); j++; } } /* to upper case */
  if(s[i]=='O'&&s[i+1]=='P'&&s[i+2]=='E'&&s[i+3]=='N')	/* open */
  {
    unit = 0;
    first = 1;
    i = i + 4;
    LUN = getDEFBOSIO();
    while(i < len)
    {
      while(s[i]==' ') i++; /* skip spaces */
      if(s[i]=='U'&&s[i+1]=='N'&&s[i+2]=='I'&&s[i+3]=='T')
      {
        first = 0;
        i = i + 4;
        while(s[i] == ' ' || s[i] == '=') i++;
        cp = &s[i];
        while(isdigit(s[i])) i++;
        s[i] = '\0';
        LUN->unit = unit = atoi(cp);
        i++;
      }
      else if(s[i]=='F'&&s[i+1]=='I'&&s[i+2]=='L'&&s[i+3]=='E')
      {
        first = 0;
        i = i + 4;
        while(s[i] == ' ' || s[i] == '=' || s[i] == '"' || s[i] == '\'') i++;



	/* comment it out - LUN->finame will be filled in bosOPEN !!!
        LUN->finame = file = &s[i];
        while(s[i] != '"' && s[i] != ' ' && s[i] != '\'') i++;
        s[i] = '\0';
        LUN->finame = strdup(file);
        file = LUN->finame;*/

	/* DO WE NEED THAT ? */
        ch = &s[i]; /* remember the begining of file name */
        while(s[i] != '"' && s[i] != ' ' && s[i] != '\'') i++;
        s[i] = '\0';
        file = strdup(ch); /* save filename, string "s" will be removed ! */ 

        i++;
        while(s[i] == ' ' || s[i] == '"') i++;
      }
      else if(s[i]=='R'&&s[i+1]=='E'&&s[i+2]=='C'&&s[i+3]=='L')
      {
        first = 0;
        i = i + 4;
        while(s[i] == ' ' || s[i] == '=') i++;
        cp = &s[i];
        while(isdigit(s[i])) i++;
        s[i] = '\0';
        LUN->recl = recl = atoi(cp);
        i++;
      }
      else if(s[i]=='S'&&s[i+1]=='P'&&s[i+2]=='L'&&s[i+3]=='I'
              &&s[i+4]=='T'&&s[i+5]=='M'&&s[i+6]=='B')
      {
        first = 0;
        i = i + 7;
        while(s[i] == ' ' || s[i] == '=') i++;
        cp = &s[i];
        while(isdigit(s[i])) i++;
        s[i] = '\0';
        LUN->splitmb = splitmb = atoi(cp);
        i++;
      }
      else if(s[i]=='R'&&s[i+1]=='E'&&s[i+2]=='A'&&s[i+3]=='D')
      {
        if(s[i+4]=='W'&&s[i+5]=='R'&&s[i+6]=='I'&&s[i+7]=='T'&&s[i+8]=='E')
        {
          first = 0;
          i = i + 9;
          flag = "a";
          i++;
        }
        else
        {
          first = 0;
          i = i + 4;
          flag = "r";
          i++;
        }
      }
      else if(s[i]=='W'&&s[i+1]=='R'&&s[i+2]=='I'&&s[i+3]=='T'&&s[i+4]=='E')
      {
        first = 0;
        i = i + 5;
        flag = "w";
        i++;
      }
      else if(s[i]=='U'&&s[i+1]=='P'&&s[i+2]=='D'&&s[i+3]=='A'&&s[i+4]=='T'&&s[i+5]=='E')
      {
        first = 0;
        i = i + 6;
        flag = "u";
        i++;
      }
      else if(s[i]=='O'&&s[i+1]=='L'&&s[i+2]=='D')
      {
        first = 0;
        i = i + 3;
        LUN->status = status = 1;
        i++;
      }
      else if(s[i]=='N'&&s[i+1]=='E'&&s[i+2]=='W')
      {
        first = 0;
        i = i + 3;
        LUN->status = status = 2;
        i++;
      }
      else if(s[i]=='S'&&s[i+1]=='E'&&s[i+2]=='Q')
      {
        first = 0;
        i = i + 3;
        LUN->access = access = 1;
        i++;
      }
      else if(s[i]=='D'&&s[i+1]=='I'&&s[i+2]=='R')
      {
        first = 0;
        i = i + 3;
        LUN->access = access = 2;
        i++;
      }
      else if(s[i]=='R'&&s[i+1]=='A'&&s[i+2]=='W')
      {
        first = 0;
        i = i + 3;
        LUN->medium = medium = 2;
        i++;
      }
      else if(s[i]=='T'&&s[i+1]=='E'&&s[i+2]=='X'&&s[i+3]=='T')
      {
        first = 0;
        i = i + 4;
        /* not implemented - always binary */
        i++;
      }
      else if(s[i]=='B'&&s[i+1]=='I'&&s[i+2]=='N'&&s[i+3]=='A'&&
                                      s[i+4]=='R'&&s[i+5]=='Y')
      {
        first = 0;
        i = i + 6;
        /* not implemented - always binary */
        i++;
      }

      /* skipping default statements like [ACTION =] */

      else if(s[i]=='A'&&s[i+1]=='C'&&s[i+2]=='T'&&s[i+3]=='I'&&
                                      s[i+4]=='O'&&s[i+5]=='N')
      {
        first = 0;
        i = i + 6;
        while(s[i] == ' ' || s[i] == '=') i++;
      }
      else if(s[i]=='A'&&s[i+1]=='C'&&s[i+2]=='C'&&s[i+3]=='E'&&
                                      s[i+4]=='S'&&s[i+5]=='S')
      {
        first = 0;
        i = i + 6;
        while(s[i] == ' ' || s[i] == '=') i++;
      }
      else if(s[i]=='S'&&s[i+1]=='T'&&s[i+2]=='A'&&s[i+3]=='T'&&
                                      s[i+4]=='U'&&s[i+5]=='S')
      {
        first = 0;
        i = i + 6;
        while(s[i] == ' ' || s[i] == '=') i++;
      }
      else if(s[i]=='F'&&s[i+1]=='O'&&s[i+2]=='R'&&s[i+3]=='M')
      {
        first = 0;
        i = i + 4;
        while(s[i] == ' ' || s[i] == '=') i++;
      }
      else if(first == 1)	/* data name specified */
      {
        first = 0;
        nch = 0;
        while(s[i] != ' ') { i++; nch++; }
        s[i] = '\0';
        daname = (char *) MALLOC(nch+1);
        bcopy((char *)&s[i-nch], (char *)daname, nch+1);
        i++;
        if(unit == 0)	/* chouse unit number, can be redefined if UNIT=.. */
        {
          while((int)bosold[unit] != 0)
          {
            unit++;
            if(unit>=MUNITS)
            {
              printf("fparm: there is no free slots\n");
              *descriptor = 0;
              FREE(s);
              return EBIO_NOFREEUNITS;
            }
          }
        }
      }
      else
      {
        printf("fparm: unknown key >%c%c%c%c< etc\n",
          s[i],s[i+1],s[i+2],s[i+3]);
        *descriptor = 0;
        FREE(s);
        return EBIO_UNKNOWNOPTION;
      }
    } /* i < len */

    /* opening */
    if(unit == 0)
    {
      printf("fparm: unit undefined !!!\n");
      FREE(LUN);
      *descriptor = 0;
      FREE(s);
      return EBIO_WRONGPARAM;
    }
    if((int)bosold[unit] != 0)
    {
      printf("fparm: unit %d opened already !!!\n",unit);
      FREE(LUN);
      *descriptor = 0;
      FREE(s);
      return EBIO_UNITINUSE;
    }

    bosold[unit] = (OLDptr) MALLOC(sizeof(OLD));
    if(daname != NULL)
    {
      bosold[unit]->daname = daname;
    }
    else
    {
      daname = (char *) MALLOC(8);
      bcopy((char *)fpinput, (char *)daname, strlen(fpinput));
      daname[7] = '\0';
      bosold[unit]->daname = daname;
    }

    if(strlen(file)==0)
    {
      printf("fparm: opening error : zero length filename !!!\n");
      FREE(s);
      return EBIO_WRONGPARAM;
    }
    error = bosOPEN(file, flag, LUN);
    if( error != 0 )
    {
      printf("fparm: opening error !!!\n");
      FREE(file); 
      FREE(LUN); 
      *descriptor = 0;
      FREE(s);
      return(error);
    }
    bosold[unit]->descriptor = LUN;
    *descriptor = LUN;
    FREE(file);
    FREE(s);
    return(0);
  }
  else if(s[i]=='C'&&s[i+1]=='L'&&s[i+2]=='O'&&s[i+3]=='S'&&s[i+4]=='E')
  {
    i = i + 5;
    while(s[i]==' ') i++;
    if(s[i]=='U'&&s[i+1]=='N'&&s[i+2]=='I'&&s[i+3]=='T')
    {
      i = i + 4;
      while(s[i] == ' ' || s[i] == '=') i++;
      cp = &s[i];
      while(isdigit(s[i])) i++;
      s[i] = '\0';
      unit = atoi(cp);
      if((int)bosold[unit] != 0)
      {
        bosClose((BOSIOptr)bosold[unit]->descriptor);
        daname = bosold[unit]->daname; /* will be free() after bosold[unit] */
        FREE(bosold[unit]);
        if(daname) FREE(daname);
        bosold[unit] = 0;
        *descriptor = 0;
        FREE(s);
        return(0);
      }
      *descriptor = 0;
      return EBIO_WRONGPARAM;
    }
    else if(len > 6)	/* data name specified */
    {
      nch = 0;
      while(s[i] != ' ' && i < len) { i++; nch++; }
      s[i] = '\0';
      daname = (char *) MALLOC(nch+1);
      bcopy((char *)&s[i-nch], (char *)daname, nch+1);
      for(i=0; i<MUNITS; i++)
      {
        if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,daname) == 0)
        {
          FREE(daname);
          bosClose((BOSIOptr)bosold[i]->descriptor);
          daname = bosold[i]->daname;
          FREE(bosold[i]);
          if(daname) FREE(daname);
          bosold[i] = 0;
          *descriptor = 0;
          FREE(s);
          return 0;
        }
      }
      return 0;
    }
    else	/* data name not specified, close all files */
    {
      for(i=0; i<MUNITS; i++)
      {
        if((int)bosold[i] != 0)
        {
          bosClose((BOSIOptr)bosold[i]->descriptor);
          daname = bosold[i]->daname;
          FREE(bosold[i]);
          if(daname) FREE(daname);
          bosold[i] = 0;
        }
      }
      *descriptor = 0;
      FREE(s);
      return 0;
    }
  }
  else if(s[i]=='R'&&s[i+1]=='E'&&s[i+2]=='W'&&s[i+3]=='I'&&s[i+4]=='N'&&
          s[i+5]=='D')
  {
    i = i + 6;
    while(s[i]==' ') i++;
 
    nch = 0;
    while(s[i] != ' ' && i < len) { i++; nch++; }
    s[i] = '\0';
    daname = (char *) MALLOC(nch+1);
    bcopy((char *)&s[i-nch], (char *)daname, nch+1);
    for(i=0; i<MUNITS; i++)
    {
      if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,daname) == 0)
      {
        bosRewind((BOSIOptr)bosold[i]->descriptor);
        *descriptor = bosold[i]->descriptor;
        FREE(s);
        return 0;
      }
    }
    FREE(s);
    return EBIO_WRONGPARAM;
  }
  else if(s[i]=='T'&&s[i+1]=='I'&&s[i+2]=='M'&&s[i+3]=='E'&&s[i+4]=='S'&&
          s[i+5]=='T'&&s[i+6]=='O'&&s[i+7]=='P')
  { /* ignore it !!! */
    i = i + 8;
    while(s[i]==' ') i++;
    printf("fpack.c: ignore TIMESTOP, return 0\n"); fflush(stdout);
    return 0;
  }
  else
  {
    printf("fparm: unknown statement: %c%c%c ..\n",s[i],s[i+1],s[i+2]);
    FREE(s);
    return EBIO_UNKNOWNOPTION;
  }
}
 
void
frbos_(int *jw, int *unit, char *list, int *error, int len)
{
  char *li;
 
  li = (char *) MALLOC(len+1);
  strncpy(li,list,len);
  li[len] = '\0';

  *error = bosRead(bosold[*unit]->descriptor, jw, li);

  FREE(li);
 
  return ;
}
 
void
fwbos_(int *jw, int *unit, char *list, int *error, int len)
{
  char *li;
 
  li = (char *) MALLOC(len+1);
  strncpy(li,list,len);
  li[len] = '\0';

  *error = bosWrite((BOSIOptr)bosold[*unit]->descriptor, jw, li);
 
  FREE(li);
 
  return;
}
 
void
fpstat_()
{
  int unit;
  BOSIOptr LUN;

  printf("\n\n\n"); 
  printf(" ***********************************************************************\n");
  printf(" *                                                                     *\n");
  printf(" *         I N P U T / O U T P U T     S T A T I S T I C S             *\n");
  printf(" *                                                                     *\n");
  printf(" ***********************************************************************\n");
  printf("\n");

  for(unit=0; unit<MUNITS; unit++)
  {
    if((int)bosold[unit] == 0) continue;
    LUN = (BOSIOptr)bosold[unit]->descriptor;
  /*fqfile();*/

    printf(" Data name .......... %s\n",bosold[unit]->daname);
    printf(" File name .......... %s\n",LUN->finame);
    printf(" Host name .......... %s\n",LUN->honame);
    printf("\n");
    printf(" Unit number ............................................... %11d\n",LUN->unit);

    if(LUN->access == 1)
      printf(" Access ....................................................  sequential\n");
    else if(LUN->access == 2)
      printf(" Access ....................................................      direct\n");
    printf(" Form ......................................................      binary\n");
    if(!strcmp(LUN->flag,"r"))
      printf(" Action ....................................................        read\n");
	else if(!strcmp(LUN->flag,"w"))
      printf(" Action ....................................................       write\n");
    printf(" Physical record length in bytes ........................... %11d\n",LUN->recl);
    printf("\n");

    if(!strcmp(LUN->flag,"r"))
    {
      printf(" Number of physical records read ........................... %11d\n",LUN->nphysrec);
      printf(" Number of logical records read ............................ %11d\n",LUN->nlogrec);
      printf("\n");
      printf(" Number of Kbytes read ..................................... %11d\n",LUN->recl*LUN->nphysrec/1000);
      printf("\n");
      printf(" Average length of logical record on input ................. %11d\n",LUN->recl*LUN->nphysrec/LUN->nlogrec);
      printf(" Number of data blocks read ................................ %11d\n",LUN->ndatrec);
    }
    else if(!strcmp(LUN->flag,"w"))
    {
      printf(" Number of physical records written ........................ %11d\n",LUN->nphysrec);
      printf(" Number of logical records written ......................... %11d\n",LUN->nlogrec);
      printf("\n");
      printf(" Number of Kbytes written .................................. %11d\n",LUN->recl*LUN->nphysrec/1000);
      printf("\n");
      printf(" Average length of logical record written .................. %11d\n",LUN->recl*LUN->nphysrec/LUN->nlogrec);
      printf(" Number of data blocks written ............................. %11d\n",LUN->ndatrec);
    }

    printf(" -----------------------------------------------------------------------\n");
  }

  printf("\n\n");

  return;
}

/* not ready yet !!! */
 
void
fqfile_(char *dnam, char *fnam, char *hnam, int par[30], int *ier, int l1, int l2, int l3)
{
  BOSIOptr BIOstream;
  int i, len;
  char *da;

return; /* until it is finished */

  /* clean output array */
  for(i=0; i<30; i++) par[i] = 0;

  da = (char *) MALLOC(l1+1);
  while(dnam[l1-1]==' ') l1--; /* remove traling spaces */
  strncpy(da,dnam,l1);
  da[l1] = '\0';
  /*printf("fqfile reached: dnam=>%s<\n",da);*/
  for(i=0; i<MUNITS; i++)
  {
    /*if((int)bosold[i] != 0)
    {
      printf("fqfile: daname=>%s< -> LUN=%x unit=%d finame=>%s<\n",
        bosold[i]->daname,(BOSIOptr)bosold[i]->descriptor,((BOSIOptr)bosold[i]->descriptor)->unit,
        ((BOSIOptr)bosold[i]->descriptor)->finame);
    }*/
  
    if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,da) == 0)
    {
      BIOstream = (BOSIOptr)bosold[i]->descriptor;

      /*printf("fqfile: fnam=%s\n",BIOS_FINAME);*/
      len = (l2 < strlen(BIOS_FINAME)) ? l2 : strlen(BIOS_FINAME);
      bcopy((char *)BIOS_FINAME, (char *)fnam, len);

      len = (l3 < strlen(BIOS_HONAME)) ? l3 : strlen(BIOS_HONAME);
      bcopy((char *)BIOS_HONAME, (char *)hnam, len);

      par[0] = BIOS_UNIT; /* unit number */
      par[1] = BIOS_ACCESS; /* access */
      par[2] = BIOS_FORMAT;
      par[3] = BIOS_ACTION;
      par[4] = BIOS_RECL;
      
/*     number of physical records read
      LUNPAR(6) = INFU(IRSTA+2,LUNFP)*/
      par[5] = /*BIOS_INNUM*/BIOS_NPHYSREC;

/*     number of physical records written
      LUNPAR(7) = INFU(IWSTA+2,LUNFP)*/
      par[6] = /*BIOS_OUTNUM*/BIOS_NPHYSREC;

/*     number of logical records read
      LUNPAR(8) = INFU(IRSTA+3,LUNFP)*/
      if(BIOS_ACTION==1) par[7] = BIOS_NLOGREC;

/*     number of logical records written
      LUNPAR(9) = INFU(IWSTA+3,LUNFP)*/
      if(BIOS_ACTION!=1) par[8] = BIOS_NLOGREC;

/*     number of Kbytes read
      LUNPAR(10) = INFU(IRSTA+4,LUNFP)*/
      par[9] = (/*BIOS_INNUM*/BIOS_NPHYSREC * BIOS_RECL) / 1024;

/*     number of Kbytes written
      LUNPAR(11) = INFU(IWSTA+4,LUNFP)*/
      par[10] = (/*BIOS_OUTNUM*/BIOS_NPHYSREC * BIOS_RECL) / 1024;

/*     average length of logical record on input
      IF(INFU(IRSTA+6,LUNFP).NE.0) LUNPAR(14) =
     +  LUNPAR(10)*1000.0 / INFU(IRSTA+6,LUNFP)*/
      par[13] = (BIOS_NPHYSREC*BIOS_RECL)/BIOS_NLOGREC;

/*     average length of logical records written
      IF(LUNPAR(9).NE.0) LUNPAR(15) = LUNPAR(11)*1000.0 / LUNPAR(9)*/
      par[14] = (BIOS_NPHYSREC*BIOS_RECL)/BIOS_NLOGREC;

/*     number of data blocks read
      LUNPAR(16) = INFU(IRSTA+5,LUNFP)*/
      par[15] = BIOS_NDATREC;

/*     number of data blocks written
      LUNPAR(17) = INFU(IWSTA+5,LUNFP)*/
      par[16] = BIOS_NDATREC;

/*     number of data blocks written without format ( B32 added )
      LUNPAR(18) = INFU(IWSTA+6,LUNFP)*/
    }
  }
  /*printf("fqfile done.\n");*/

  *ier = 0;
  return;
}
 
#define NBCS 700000 
#include "bcs.h"
 
static int sequnit[MUNITS];	/* current unit for sequential writing */
static int ustop = 0;       /* from cseqr.inc */
static int eod = 0;


/*
     Arguments:

 daname = name of data set
  error = returned flag with values ...
  error =  1   record read
        =  0   end-of-data condition - DO NOT IMPLEMENTED YET !!!
        = -1   after end-of data condition (skip modules)
*/

void
fseqr_(char *daname, int *error, int len)
{
  int i;
  char *da;
  static int first = 1;

  if(first)
  {
    first = 0;
    ustop = 0;
  }
  else
  {
    if(eod)
	{
      eod = 0;
      *error = -1;
      return;
	}
  }

  for(i=0; i<MUNITS; i++)	/* output specified units */
  {
    if(sequnit[i] == 1) bosWrite((BOSIOptr)bosold[i]->descriptor, bcs_.iw, "E");
  }
  bosLdrop(bcs_.iw,"E");
  bosNgarbage(bcs_.iw);

  while(daname[len-1]==' ') len--; /* remove traling spaces */
  da = (char *)malloc(len+1);
  strncpy(da,daname,len);
  da[len] = '\0';

  for(i=0; i<MUNITS; i++)
  {
    if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,da) == 0)
    {
      *error = bosRead((BOSIOptr)bosold[i]->descriptor,bcs_.iw,"E");
 
      if(*error == -1 || ustop == 1) /* finish output file */
      {
        for(i=0; i<MUNITS; i++)	/* finish specified units */
        {
          if(sequnit[i] == 1) bosWrite((BOSIOptr)bosold[i]->descriptor, bcs_.iw, "0");
        }
        bosLdrop(bcs_.iw,"E");
        bosNgarbage(bcs_.iw);
        if(ustop)
        {
          eod = 1;
          ustop = 0;
        }
        return;
      }
      if(*error == 0) *error = 1;
      return;
    }
  }
  /*printf("fseqr: file not found !!!\n");*/
  return;
}

void
fseqw_(char *daname, int len)
{
  int i;
  char *da;
 
  while(daname[len-1]==' ') len--; /* remove traling spaces */
  da = (char *)MALLOC(len+1);
  strncpy(da,daname,len);
  da[len] = '\0';
  /*printf("fseqw_ >%s<\n",da);*/

  for(i=0; i<MUNITS; i++)
  {
    if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,da) == 0)
    {
      sequnit[i] = 1;
      return;
    }
  }
  /*printf("fseqw: file not found !!!\n");*/
  return;
}


/* HOW TO RETURN IER ??? - not done yet */

#include "fpack.h"

static BOSIO *input_descriptor;  /* current unit for reading */
static BOSIO *output_descriptor; /* current unit for writing */
static int dathead[70];
static int nread;

/* select unit for reading */
 
void
frname_(char *daname, int len)
{
  int i;
  char *da;
 
  while(daname[len-1]==' ') len--; /* remove traling spaces */
  da = (char *) MALLOC(len+1);
  strncpy(da,daname,len);
  da[len] = '\0';
 
  for(i=0; i<MUNITS; i++)
  {
    if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,da) == 0)
    {
      input_descriptor = (BOSIOptr)bosold[i]->descriptor;
      FREE(da);
      return;
    }
  }
  printf("frname: file >%s<not found.\n",da);
  FREE(da);
  return;
}

/* get record header */

int
frkey_(char *recname, int *numra, int *numrb, int *icl, int *err, int len)
{
  BOSIOptr BIOstream;
  int *bufin;

  BIOstream = input_descriptor;
  bufin = BIOS_BUFP;

  CHECK_INPUT_UNIT(input_descriptor);
  ALLOCATE_INPUT_BUFFER(input_descriptor);

  /* scroll records until will get completed or first one */
  
  do
  {
    if(IP != 0 && NEXTIP(IP) < bufin[1])   /* we are in the middle of block */
    {
      IP = NEXTIP(IP);
    }
    else                                 /* block are finished - let's get another one */
    {
      int ii, error;

      ii = BIOS_RECL/sizeof(int);
      error = bosin(input_descriptor,&ii);
      bufin = BIOS_BUFP; /* could be changed in bosin() ! */
      if(error < 0)	/* end of file */
      {
        *err = error;
        return(error);
      }
      if(error != 0)    /* some error */
      {
        *err = error;
        return(error);
      }
      ii = LOCAL; frconv_(bufin,&ii,&error); /* convert buffer */
      IP = 2;                                /* successful read - reset pointer */
    }

    bcopy((char *)&bufin[IP],(char *)BIOS_RECHEAD, RECHEADLEN*sizeof(int));
    BIOS_NLOGREC++;
    if(BIOS_RECHEAD[irCODE] < 0 || BIOS_RECHEAD[irCODE] > 3) return(EBIO_WRONGSEGMCODE);
 
  } while(BIOS_RECHEAD[irCODE] != 0 && BIOS_RECHEAD[irCODE] != 1);

  /* reset JP - frhdr() need it !!! */

  JP = 0;

  /* fill output parameters and return */

  len = (len < 8) ? len : 8;
  bcopy((char *)&BIOS_RECHEAD[irNAME1], recname, len);
  *numra = BIOS_RECHEAD[irNRUN];
  *numrb = BIOS_RECHEAD[irNEVNT];
  *icl = BIOS_RECHEAD[irTRIG];

  *err = 0;
  return(*err);
}

/* get data header */

int
frhdr_(char *datname,int *nr,int *ncol,int *nrow,int *fm,int *nfm,int *err,int len1,int len2)
{
  BOSIOptr BIOstream;
  char *fmt;
  int i, lfmt, *bufin;

  BIOstream = input_descriptor;
  bufin = BIOS_BUFP;

  GET_DATA_HEADER(input_descriptor,done,*err,error_exit);

done: /* go here if end of record */

  len1 = (len1 < 8) ? len1 : 8;
  bcopy((char *)&dathead[idNAME1], datname, len1);
  /*printf("=>%c%c%c%c<=\n",datname[0],datname[1],datname[2],datname[3]);*/
  *nr = dathead[idNSGM];
  *ncol = dathead[idNCOL];
  *nrow = dathead[idNROW];

  fmt = (char *)&dathead[idFRMT];
  *nfm = 0;
  while(fmt[*nfm] != ' ' && fmt[*nfm] != '\0' && *nfm < (dathead[idNHEAD]-idFRMT)*4) (*nfm)++;

  lfmt = (len2 < *nfm) ? len2 : *nfm;
  bcopy((char *)&dathead[idFRMT], fm, lfmt);
  fmt = (char *)fm;
  fmt[lfmt] = '\0';
  nread = 0; /* clean it up for following frdat_() calls */
  KP = 0;

error_exit:

  return(*err);
}

/* get data; if *ndata == 0 - no more data, otherwise call this function again */

int
frdat_(int *ndata, int *array, int *ndim)
{
  BOSIOptr BIOstream;
  int *bufin;
  int error;

  BIOstream = input_descriptor;
  bufin = BIOS_BUFP;

  while(1 == 1)
  {
    GET_DATA(&array[nread],(*ndim),error);
    if(nread == *ndim || dathead[idCODE] == 0 || dathead[idCODE] == 3)
    {
      *ndata = nread; /* in manual - 0, but in fpack - nread */

      if(error)
      {
        printf("fpack.c: SPACE NOT ENOUGH - LOST THE REST OF DATA !!!\n");
        printf("NDIM=%d\n",*ndim);
        exit(1);
      }

      return(0); /* got all data */
    }

    GET_DATA_HEADER(input_descriptor,done,error,error_exit);
    KP = 0;
    if(dathead[idCODE] == 0 || dathead[idCODE] == 1) return(EBIO_NOTCONTCODE);
  }

done:
  printf("frdat_(): ERROR at done.\n");
  return(1);

error_exit:
  printf("frdat_(): ERROR at error_exit.\n");
  return(1);
}

/* select unit for writing */
 
void
fwname_(char *daname, int len)
{
  int i;
  char *da;
 
  while(daname[len-1]==' ') len--; /* remove traling spaces */
  da = (char *) MALLOC(len+1);
  strncpy(da,daname,len);
  da[len] = '\0';
 
  for(i=0; i<MUNITS; i++)
  {
    if((int)bosold[i] != 0 && strcmp(bosold[i]->daname,da) == 0)
    {
      output_descriptor = (BOSIOptr)bosold[i]->descriptor;
      return;
    }
  }
  printf("fwname: file not found !!!\n");
  return;
}

/* put record header */

int
fwkey_(char *recname, int *numra, int *numrb, int *icl, int len)
{
  BOSIOptr BIOstream;
  int i, *bufout;
  int nfopen; /* to make CHECK_OUTPUT_UNIT happy */
  char tmp[9];

  BIOstream = output_descriptor;
  bufout    = BIOS_BUFP;

  if(len > 8) len = 8;
  strncpy(tmp,recname,len);
  for(i=len; i<8; i++) tmp[i] = ' ';
  tmp[9] = '\0';

  ALLOCATE_OUTPUT_BUFFER;
  CHECK_OUTPUT_UNIT(output_descriptor);

  PUT_RECORD_HEADER(output_descriptor,tmp,*numra,*numrb,*icl);

  return (0);
}

/* put data header */

int
fwhdr_(char *datname, int *nr, int *ncol, int *nrow, char *fm, int len1, int len2)
{
  BOSIOptr BIOstream;
  int *bufout;
  char *fmt;
  int nch, i, *name;

  BIOstream = output_descriptor;
  bufout    = BIOS_BUFP;

  nch = ((len2 + 3)/4) * 4;
  fmt = (char *) MALLOC(nch+1);
  strncpy(fmt,fm,len2);
  for(i=len2; i<nch; i++) fmt[i] = ' ';
  fmt[nch] = '\0';
  name = (int *)datname;
/*
  printf("=>%4.4s<=\n",name);
  printf("->%s<-\n",fmt);
*/
  PUT_DATA_HEADER(output_descriptor,(*name),(*nr),(*ncol),(*nrow),fmt,nch);

  return(0);
}

/* put data */

int
fwdat_(int *ndata, int *array)
{
  BOSIOptr BIOstream;
  int *bufout;

  BIOstream = output_descriptor;
  bufout    = BIOS_BUFP;

  PUT_DATA(output_descriptor,(*ndata),array,(-1));

  return(0);
}


/* have to be called in the end of logical record, namely
  after fwhdr()-fwdat() loop and before next fwkey() */

int
fwend_(int *error)
{
  BOSIOptr BIOstream;
  int *bufout;

  BIOstream = output_descriptor;
  bufout    = BIOS_BUFP;

  if(bufout[IP+irCODE] != 0) bufout[IP+irCODE] = 3; /* if last record was "continued" mark it as the last */

  bufout[1] = KP;
  IP = KP;

  return(0);
}


/* put last buffer into the file; have to be called before file closing */

int
fweod_()
{
  BOSIOptr BIOstream;
  int *bufout;

  BIOstream = output_descriptor;
  bufout    = BIOS_BUFP;

  if(bufout[IP+irCODE] != 0) bufout[IP+irCODE] = 3; /* if last record was "continued" mark it as the last */

  WRITE_BUF_TO_DISK(output_descriptor);
  START_NEW_PHYSREC;
  BIOS_NSEGM--;

  return(0);
}


/*                       */
/* some stuff for RECSIS */
/*                       */

/* read statements from the unit "*lun" and execute them */

void
fparmr_(int *lun)
{
  FILE *desc;
  char string[256];
  int unit;

  /*printf("fparmr_ reached\n");*/
  unit = *lun;
  if((int)bosold[unit] != 0)
  {
    desc = (FILE *)bosold[unit]->descriptor;
    while( fgets(string, 256, desc) != NULL) printf("fparmr_: %s\n",string);
  }
}

void
fuclos_(int *lun)
{
  char string[15];
  int unit;

  unit = *lun;
  sprintf(string,"CLOSE UNIT=%02d",unit);
  /*printf("fuclos_: string >%s<\n",string);*/
  if((int)bosold[unit] != 0) fparm_(string,strlen(string));
}

void
fclos_()
{
  int lun;
  /*printf("fclos_ reached\n");*/
  for(lun=0; lun<MUNITS; lun++) fuclos_(&lun);
  /*printf("fclos_ done\n");*/
}


/*
  suppose to return message from text; dummy now
  if no message, set all text to ' ' !!!
 */
void
fermes_(char *text, int *ne, int len)
{
  int i;

  for(i=0; i<len; i++) text[i] = ' ';
  /*printf("fermes_ reached\n");*/
}


void
fseqh_(char *name, int *nr, int *irun, int *ievt, int len)
{
  /*printf("fsegh_ reached\n");*/
  return;
}

void
fseqe_()
{
  /*printf("fseqe_ reached\n");*/
  ustop = 1;
  return;
}

void
fseqp_(int *iarg1, int *iarg2)
{
  /*printf("fseqp_ reached\n");*/
  return;
}

