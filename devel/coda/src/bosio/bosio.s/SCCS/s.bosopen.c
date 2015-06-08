h28882
s 00001/00001/00922
d D 1.5 10/03/09 12:25:05 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00922
d D 1.4 08/01/26 13:55:07 boiarino 5 4
c fix calculation of 'avail'
c 
e
s 00025/00006/00898
d D 1.3 08/01/18 17:19:16 boiarino 4 3
c (buf.f_bavail*buf.f_frsize)/1024
c 
e
s 00047/00055/00857
d D 1.2 03/04/17 16:45:50 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosopen.c
e
s 00912/00000/00000
d D 1.1 00/08/10 11:10:06 boiarino 1 0
c date and time created 00/08/10 11:10:06 by boiarino
e
u
U
f e 0
t
T
I 1
D 3
/*DECK ID>, BOSOPEN. */
E 3

D 3
#include <string.h>
E 3
I 3
/* bosopen.c */

#include <stdio.h>
E 3
#include <stdlib.h>
I 3
#include <string.h>
E 3
#include <sys/types.h>
#ifdef SunOS
#include <sys/statvfs.h>
#endif

#include "bosio.h"
#include "fpack.h"

/* #define COND_DEBUG */
/* #define DEBUG   */
#include "dbgpr.h"
 
/* declare a global variable */
 
D 3
static	int		lund;		/* how many files are opened */
static	int		ncon;		/* how many connections are opened */
static	Nptr		net[NSERV];
static	BOSIOptr	infu[NUNITS];
E 3
I 3
static	int      lund;       /* how many files are opened */
static	int      ncon;       /* how many connections are opened */
static	Nptr     net[NSERV];
static	BOSIOptr infu[NUNITS];
E 3
 
int
D 3
bosopen_(char *file, char *flag, int *descriptor, int filen, int fllen)
E 3
I 3
bosopen_(char *file, char *flag, BOSIO **descriptor, int filen, int fllen)
E 3
{
  char *fi, *fl;
  int status;
 
  fi = (char *) MALLOC(filen+1);
  strncpy(fi,file,filen);
  fi[filen] = '\0';
  fl = (char *) MALLOC(fllen+1);
  strncpy(fl,flag,fllen);
  fl[fllen] = '\0';
  DPR3("bosopen_ : Call bosOpen(file='%s', flag='%s', desc=%X)\n",fi, fl, descriptor);
  status = bosOpen(fi, fl, descriptor);
  FREE(fl);
  FREE(fi);
  return(status);
}
D 3
 
BOSIOptr
getDEFBOSIO()
{
  return (fillDEFBOSIO(getBOSIO()));
}
E 3

D 3
BOSIOptr
fillDEFBOSIO (BOSIOptr lun) 
E 3
I 3
BOSIO *
fillDEFBOSIO(BOSIO *lun) 
E 3
{
  lun->unit    = 0;
  lun->recl    = 32760;
  lun->action  = 2;
  lun->status  = 2;
  lun->access  = 1;
  lun->format  = 0;
  lun->medium  = 2;
  lun->nrec    = 100;
  lun->splitmb = 0;
  lun->splitev = 0; /* not in used */
  lun->reopen  = 0;
  lun->stream  = 0;
  lun->client  = 0;
  lun->recnum  = 0;
  lun->outnum  = 0;
  lun->cursta  = 0;
  lun->convert = 0;
  lun->recfmtw = 0;
  lun->recfmtr = 0;
  lun->innum   = 0;
  lun->iobptr  = 0;
  lun->ip      = 0;
  lun->jp      = 0;
  lun->kp      = 0;
  lun->bufp    = NULL;
  lun->finame  = " ";
  lun->honame  = " ";

  lun->recname[0] = 'R';
  lun->recname[1] = 'U';
  lun->recname[2] = 'N';
  lun->recname[3] = 'E';
  lun->recname[4] = 'V';
  lun->recname[5] = 'E';
  lun->recname[6] = 'N';
  lun->recname[7] = 'T';
  lun->runnum = 0;
  lun->evntnum = 0;
  lun->trig = 0;
D 3
	
  return lun;
E 3
I 3

  return(lun);
E 3
}

I 3
BOSIO *
getDEFBOSIO()
{
  return(fillDEFBOSIO(  (BOSIO *) MALLOC(sizeof(BOSIO))  ));
}
E 3




D 3


E 3
char *
D 3
cleanFN (char *file)
E 3
I 3
cleanFN(char *file)
E 3
{
  int  len;
  char *f, *cp;

  f=file;
  while (*f==' ') f++; /* Skip spaces */
  for ( cp=f; *cp!=' ' && isprint(*cp) && *cp!=0; cp++) {};
  len = (int)(cp-f);
  len = strlen(f);
  cp = memcpy((char *) MALLOC(len+1),f,len);
  cp[len]=0;
  return(cp);
}

int
D 3
parseListFN (char *FList, bosioFNList *flist)
E 3
I 3
parseListFN(char *FList, bosioFNList *flist)
E 3
{
  int i, flen, n;
  char *f, *beg, *end, *ind, *ext, *e1, *e2, *tmp, *veryend;

  DPR1("parseListFN : FN spec '%s'\n",FList);
  beg = f = cleanFN(FList);
  veryend = f + strlen(f);
  flist->curFN = 0;
  flist->nFN = 0;

  /* Loop on comma separated list */
  do
  {
    if ( (end=strchr(beg,',')) == NULL ) end=veryend;
    if (flist->nFN == MAXFNLIST) {
      printf("parseListFN : Error : Max number of files in list %i is exeeded => stop parsing\n",MAXFNLIST);
      FREE(f);
      return(-1);
    }
    *end=0;
    /* Search for first '.' from the end  */
    for (ind=end-1; ind>=beg && *ind!='.' ; ind--) ;
    ext=ind+1;
    DPR1("parseListFN : Process List element '%s' ............. \n",beg);
    if (ext!=beg && end-ext==7 && 
    	isalpha(ext[0])&&isdigit(ext[1])&&isdigit(ext[2]) &&
    	ext[3] == '-' &&
    	isalpha(ext[4])&&isdigit(ext[5])&&isdigit(ext[6]) )
    {
      /* Extension represents range */
      *(ext-1)=ext[3]=0;
      e1=ext; e2=ext+4;
      flen=strlen(beg);
      DPR5("parseListFN : got range '%s' of extensions  '%s' & '%s' for file '%s' flen=%i\n"
    	   ,ext,e1,e2,beg,flen);
      /* Loop over range of extensions */
      while (strcmp(e1,e2) <= 0)
      {
    	if (flist->nFN == MAXFNLIST)
        {
    	  printf("parseListFN : Error : Max number of files in list %i is exeeded => stop parsing\n",MAXFNLIST);
    	  FREE(f);
    	  return -1;
        }
        tmp = flist->FN[flist->nFN] = (char *)MALLOC(flen+5);
        strcpy(tmp,beg);
        tmp[flen]='.';
        strcpy(tmp+flen+1,e1);
        flist->nFN++;
        /* Now increment extension */
        n = (e1[2]-'0') + (e1[1]-'0')*10;
        DPR2("parseListFN : incrementing extension %s (n=%i) -> ",e1,n);
        if(n == 99)
        {
          e1[0]++;
          e1[1]='0';
          e1[2]='0';
        }
        else
        {
          n++;
          e1[1]=n/10+'0';
          e1[2]=n%10+'0';
        }
        DPR2("%s (n=%i)\n",e1,n);
      }
    }
    else
    {
      /* Store this file as is */
      tmp = flist->FN[flist->nFN] = (char *)MALLOC(strlen(beg)+1);
      strcpy(tmp,beg); /* copy from 'beg' to 'tmp' */
      flist->nFN++;
    }
    beg=end+1;
    DPR1("parseListFN : left to process '%s'\n",beg);
  } while ( beg < veryend ) ;
  DPR4("parseListFN : beg=0x%8.8X='%s' veryend=0x%8.8X='%s\n",beg,beg,veryend,veryend);

  FREE(f);
  flist->FN[flist->nFN]=NULL;
  return(flist->nFN);
}

char *
D 3
getNextFN(BOSIOptr stream)
E 3
I 3
getNextFN(BOSIO *stream)
E 3
{
  return stream->flist.FN[stream->flist.curFN+1];
}







char *
getSplitFN(char *file)
{
  int len, n;
  char *f, *nf, *cp;
  f = file;
  DPR1("getSplitFN : got filename '%s'\n",file);
  while (*f==' ') f++;
  for (cp=f; *cp != NULL; cp++) if((*cp==' ') || !(isprint(*cp))) *cp='\0';
  len = strlen(f);
  DPR1("getSplitFN : filename with stripped blanks '%s'\n",f);
  if(f[len-4]=='.'&&isalpha(f[len-3])&&isdigit(f[len-2])&&isdigit(f[len-1]))
  {
    DPR("getSplitFN : incrementing version\n");
    nf = f;
    n = (f[len-1]-'0') + (f[len-2]-'0')*10;
    if(n == 99)
    {
      f[len-3]++;
      f[len-2]='0';
      f[len-1]='0';
    }
    else
    {
      n++;
      f[len-2]=n/10 + '0';
      f[len-1]=n%10 + '0';
    }
  }
  else
  {
    nf = (char *) MALLOC(len+5);
    strcpy(nf,f);
    nf[len+0]='.';
    nf[len+1]='A';
    nf[len+2]='0';
    nf[len+3]='0';
    nf[len+4]=0;	  /* \0 ??? */
  }
  DPR1("getSplitFN : made filename '%s'\n",nf);
  return nf;
}

 
int
D 3
bosOpen(char *file, char *flag, int *descriptor)
E 3
I 3
bosOpen(char *file, char *flag, BOSIO **descriptor)
E 3
{
D 3
  *descriptor = (int) getDEFBOSIO();
E 3
I 3
  *descriptor = getDEFBOSIO();
E 3

D 3
  return( bosOPEN(file, flag, descriptor) );
E 3
I 3
  return( bosOPEN(file, flag, *descriptor) );
E 3
}

int
D 3
bosOPEN(char *file, char *flag, int *descriptor)
E 3
I 3
bosOPEN(char *file, char *flag, BOSIO *descriptor)
E 3
{
  BOSIOptr BIOstream;
  FILE *f;
  char *action, *cp, ch[40], *bositoa();
  char *wtmp = "wb,recfm=f,lrecl=";
  char *atmp = "a+b,recfm=f,lrecl=";
  int i, lun, error, iop3, iop5, iagain, reopen, *buf;
D 3

E 3
  /* structure BOSIO have to be allocated outside */
  /* (*descriptor) contains pointer to it */
  BOSIOptr Blun;
D 3
  Blun = (BOSIOptr) *descriptor;
  BIOstream = (BOSIOptr) *descriptor;
E 3

I 3
  Blun = descriptor;
  BIOstream = descriptor;

E 3
  /* set local "reopen" and clean global one */
  reopen = Blun->reopen;
  Blun->reopen = 0;

D 3

E 3
  DPR5("bosOPEN : file='%s' flag='%s' des.=%X Blun=%X reopen=%i\n",
        file,flag,*descriptor,Blun,reopen); 
 
  Blun->flag = flag;

D 3

E 3
  if(!reopen)    /* Process New File      */
  {
    /* looking for free slot */
    for(lun=0; (lun < NUNITS) && (infu[lun] != 0); lun++) {;}
    if(lun == NUNITS) /* too many units are used */
    {
      error = EBIO_NOFREEUNITS;
      goto exit;
    }
    lund++;		/* new slot */
    infu[lun] = Blun;
    infu[lun]->unit = lun + 1;

    if(parseListFN(file, &(Blun->flist)) <= 0)  /* get list of file names */
    {
      error = EBIO_WRONGPARAM; 
      goto exit;
    }
    else
    {
      int i;
      for(i=0; i<Blun->flist.nFN; i++) printf("file[%2d]->%s<-\n",i,Blun->flist.FN[i]);
    }
  }
  else
  {
    DPR("bosOPEN : close file, but remember lun\n");
    printf("bosopen.c: close file, but remember lun\n");
    lun = Blun->unit-1;
    Blun->reopen = 1;
D 3
    bosClose(*descriptor);
E 3
I 3
    bosClose(descriptor);
E 3
    printf("bosopen.c: old file closed\n");
  }

D 3

E 3
 /*
  * check action
  */
 
  switch (*flag)
  {
    /* READ */
    case ' ':
    case 'r':
    case 'R': infu[lun]->action = 1;
              infu[lun]->status = 1;
              action = "rb";
              break;
    /* WRITE */
    case 'w':
    case 'W': infu[lun]->action = 2;
              infu[lun]->status = 2;
              action = "wb";
              break;
    /* READWRITE */
    case 'a':
    case 'A': infu[lun]->action = 3;
              infu[lun]->status = 1;
              action = "a+b";
              break;
    /* UPDATE */
    case 'u':
    case 'U': infu[lun]->action = 4;
              infu[lun]->status = 1;
              action = "r+b";
              break;
    default:  printf("bosopen: wrong parameters\n");
              FREE(infu[lun]);
              infu[lun] = 0;
              error = EBIO_WRONGPARAM;
              goto exit;
  }
 
  /* check consistence */
 
  iop3 = infu[lun]->action;
  iop5 = infu[lun]->access;
  iagain = 0;
  if(infu[lun]->access == 2)	/* some corrections for direct access */
  {
    if(infu[lun]->action != 2 && iagain == 0)
      iop5 = 1;	/* open as sequential to obtain record length */
    else
      iop5 = 2;	/* open as direct */
    if(infu[lun]->action != 1 && infu[lun]->action != 2)  /* append -> update */
    {
      iop3 = 4;
      action = "r+b";
    }
  }

D 3

E 3
  /* get file name */

  if(reopen && infu[lun]->action ==2 && (infu[lun]->splitmb!=0 || infu[lun]->splitev!=0) ) 
  {
    infu[lun]->finame = getSplitFN(infu[lun]->flist.FN[0]);
  }
  else if(reopen && infu[lun]->action == 1)
  {
    /* Reopen Stream taking next file from the list */
    if (++infu[lun]->flist.curFN >= infu[lun]->flist.nFN)
    {
      printf("No more continuation files, infu[%d]=%08x\n",lun,infu[lun]);
      error = EBIO_EOF;
      goto exit;
    }
    infu[lun]->finame = infu[lun]->flist.FN[infu[lun]->flist.curFN];
  }
  else if(reopen && infu[lun]->action != 1)
  {
    /* Reopening file in APPEND and UPDATE mode not possible */
    error = EBIO_WRONGPARAM; 
    goto exit;	
  }
  else
  {
    infu[lun]->finame = infu[lun]->flist.FN[0];
    if(infu[lun]->action == 1 && Blun->flist.nFN > 1) Blun->splitmb = 1; /* force reopen */
  }

  /* if writing and have splitted files, then check disk partition - CLAS specific thing !!! */

#ifdef SunOS
  if(infu[lun]->action == 2 && Blun->splitmb != 0)
  {
    int status;
D 4
    long avail;
E 4
I 4
    int avail;
E 4
    char str[100];
    struct statvfs buf;

I 4
    printf("bosopen.c: splitted files handling ..\n");fflush(stdout);
    printf("bosopen.c: >%s<\n",Blun->finame);fflush(stdout);

E 4
    i = strlen(Blun->finame);
I 4
    printf("bosopen.c: len1=%d\n",i);fflush(stdout);

E 4
    if(i >= 100) i = 99;
D 4
    while(Blun->finame[i] != '/') i--;
    strncpy(str,Blun->finame,i);
    str[i] = '\0';
    printf("checking how much disk space left in directory %s\n",str);
E 4
I 4
    printf("bosopen.c: len2=%d\n",i);fflush(stdout);

    while((Blun->finame[i] != '/') && (i>0)) i--;
printf("bosopen.c: len3=%d\n",i);fflush(stdout);

    if(i>0)
    {
      strncpy(str,Blun->finame,i);
      str[i] = '\0';
	}
    else
    {
      strcpy(str,".");
    }
printf("bosopen.c: >%s< len=%d\n",Blun->finame,i);fflush(stdout);

    printf("bosopen.c: checking how much disk space left in directory %s\n",
      str);
E 4
    if(status=statvfs(str,&buf))
    {
      printf("bosopen.c: ERROR=%d calling statvfs(), so can not check how much disk space left ...\n",status);
    }
    else
    {
      printf("bosopen.c: total %d blocks, free %d blocks, available %d blocks (1 block = %d bytes)\n",
              buf.f_blocks,buf.f_bfree,buf.f_bavail,buf.f_frsize);
D 4
      avail = buf.f_bavail*(buf.f_frsize/1024);
E 4
I 4
D 5
      avail = (buf.f_bavail*buf.f_frsize)/1024;
E 5
I 5
      avail = (buf.f_bavail/1024)*buf.f_frsize;
E 5
E 4
      printf("bosopen.c: so we have %d kbytes available space\n",avail);
      if(avail > Blun->splitmb*1024)
      {
        printf("bosopen.c: we can continue to write file.\n");
      }
      else
      {
        printf("bosopen.c: ERROR: there is no enough space for writing.\n");
        printf("bosopen.c: ERROR: disk could be overflow in any moment !!!\n");
        printf("bosopen.c: ERROR: !!!!! STOP WRITING IMMIDEATELY !!!\n");
      }
    }
  }
#endif

  /* opening */
 
  if(infu[lun]->honame[0] != ' ')	/* remote file */
  {
    if(*action == 'w')
    {
      bcopy(wtmp,ch,strlen(wtmp));
      action = strcat(ch,bositoa(infu[lun]->recl));
    }
    else if(*action == 'a')
    {
      bcopy(atmp,ch,strlen(wtmp));
      action = strcat(ch,bositoa(infu[lun]->recl));
    }
D 3
    if((error = fpopen((int)infu[lun],action)) != 0)
E 3
I 3
    if((error = fpopen(infu[lun],action)) != 0)
E 3
    {
      /* error = -6; */
      goto exit;
    }
  }
  else				/* local file */
  {
    DPR1("bosOPEN : before copen call infu[lun]->medium = %i\n",infu[lun]->medium); 
    if((error = copen(&infu[lun]->stream, infu[lun]->finame, action,
        infu[lun]->medium, infu[lun]->recl, &infu[lun]->iobptr, reopen)) != 0)
    {
      printf("bosopen: error in copen = %d\n",error); fflush(stdout);
      /* error = -7; */
      goto exit;
    }
  }

  if(infu[lun]->action == 2 && Blun->splitmb != 0)
  {
    if(!strncmp(Blun->finame, "/ssa/active/stage_in/", 21) || !strncmp(Blun->finame, "/raid/stage_in/", 15))
    {
D 6
      char str[100], *strval = "                    ";
E 6
I 6
      char str[100], strval[21];
E 6
      printf("bosopen.c: check for partition swaping: file->%s<-\n",Blun->finame);
      strcpy(str,"checkdisk ");
      sprintf(strval," %4d",Blun->splitmb * 2);
      strcat(str,strval);
      printf("bosopen.c: system call: >%s<\n",str);
      system(str);
      printf("bosopen.c: system call completed\n");
    }
  }


D 3



E 3
  /* sequential reading from direct file to get record length */

  if(infu[lun]->access == 2 && iop5 == 1)
  {
    /* SDELAT' ETOT KUSOK VIZOVAMI NIZSHEGO UROVNYA !!! */

    printf("NEVER SHOULD BE HERE !!!\n");
    /* call frseq(lun,1,12,jbuf,error) */
    infu[lun]->recnum = 0;
    if(error == 0)
    {
      /*  CALL FSWAP(jbuf,12)
      infu[lun]->recl = jbuf(1)*4 */
      ;
    }
    else if(infu[lun]->recl == 0)
    {
      infu[lun]->recl = 32760;
    }
    iagain = 1;
D 3
    bosClose((int)infu[lun]);	
E 3
I 3
    bosClose(infu[lun]);	
E 3
    /* CAN'T DO THAT, BECAUSE OF bosClose ARE MAKING free(..) */
    exit(-1);
  }







exit:

  if(error)
  {
    if(error == EBIO_EOF) return(error); /* if end of file, do nothing, leave for bosClose() */
    FREE(infu[lun]);
    infu[lun] = 0;
D 3
    *descriptor = 0;
E 3
I 3
    descriptor = 0;
E 3
    return(error);
  }
 
  if(infu[lun]->unit)
  {
D 3
    *descriptor = (int) infu[lun];
E 3
I 3
    descriptor = infu[lun];
E 3
    return(0);
  }
  else
  {
    FREE(infu[lun]);
    infu[lun] = 0;
D 3
    *descriptor = 0;
E 3
I 3
    descriptor = 0;
E 3
    return(EBIO_INTERNAL);
  }
}
 
 
int
D 3
bosclose_(int *descriptor)
E 3
I 3
bosclose_(BOSIO *descriptor)
E 3
{
D 3
  return(bosClose(*descriptor));
E 3
I 3
  return(bosClose(descriptor));
E 3
}
 
int
D 3
bosClose(int descriptor)
E 3
I 3
bosClose(BOSIO *descriptor)
E 3
{
  BOSIOptr st;
  int i, j, error, client, nc, lun, reopen;

D 3
  st = (BOSIOptr)descriptor;	/* set pointers */
E 3
I 3
  st = descriptor;	/* set pointers */
E 3

  error = 0;

  /* set local "reopen" and clean global one */
  reopen = st->reopen;
  st->reopen = 0;

  if(st->client != 0)	/* close remote file, stop server if necessary */
  {
    client = st->client;
    nc = 1;
    for(i=0; i < ncon; i++)
    {
      if(client == net[i]->client)
      {
        nc=net[i]->nfiles;
        break;
      }
    }

    if(nc == 1)
    {
      error = 1;
      if(i <= ncon)
      {
        for(j=i; j < ncon; j++)
        {
          net[j]->honame = net[j+1]->honame;
          net[j]->client = net[j+1]->client;
          net[j]->nfiles = net[j+1]->nfiles;
        }
        ncon--;
      }
    }
    else
    {
      net[i]->nfiles = net[i]->nfiles - 1;
    }
    if(client < 0)	/* network channel is dead */
      error = EBIO_EOF;
    else
      closec(&client,&st->stream,&error);
    st->stream = 0;
    st->client = 0;
  }
  else	/* close local file */
  {
    DPR("bosClose : close local file\n");
    printf("bosClose : close local file\n");
    if(st->stream < 0) return(error);
    DPR3("bosClose : Call cclose(stream=%i,iobptr=%X,medium=%i)\n",st->stream,st->iobptr,st->medium);
    printf("bosClose : Call cclose(stream=%i,iobptr=%X,medium=%i)\n",st->stream,st->iobptr,st->medium);
    cclose(st->stream,st->iobptr,st->medium);
    if(!reopen)
    {
      printf("-100\n");
      st->stream = -100;
    }
  }
 
/* release slot */

  if (!reopen)
  {
    /*if(st->splitmb!=0 || st->splitev!=0 ) FREE(st->finame); - will do it anyway ... */
    if(st->bufp != NULL) FREE(st->bufp);
    if(st->finame != NULL) FREE(st->finame);
    lun = st->unit-1;
    FREE(infu[lun]);
    infu[lun] = 0;
  }
  else
  {
    printf("bosClose : Not releasing slot\n");
  }


  return(error);
}


int
D 3
fpopen(int descriptor, char *action)
E 3
I 3
fpopen(BOSIO *descriptor, char *action)
E 3
{
D 3
  BOSIOptr st;
E 3
I 3
  BOSIO *st;
E 3
  int i, j, lun, error, stream, client, ii;
 
D 3
  st = (BOSIOptr)descriptor;	/* set pointers */
E 3
I 3
  st = descriptor; /* set pointers */
E 3
 
  error = 0;
 
a:
  stream = 0;
  if(ncon > 0)
  {
    for(i=0; i<ncon; i++)
    {
      if(strcmp(st->honame,net[i]->honame) == 0)
      {
        client = net[i]->client;
        ii = (1 > st->nrec/5) ? 1 : st->nrec/5;
        openc(&client,&stream,st->finame,st->honame,action,&st->recl,&st->nrec,
               &ii);
        if(stream == 0)
        {
          if(client == 0)	/* old connection is dead, try to recover */
          {
            if(i < ncon)
            {
              for(j=i; j<ncon; j++)
              {
                net[j]->honame = net[j+1]->honame;
                net[j]->client = net[j+1]->client;
                net[j]->nfiles = net[j+1]->nfiles;
              }
            }
            ncon--;
            for(j=0; j<NUNITS; j++)	/* mark all units opened through this */
            {				/* channel as unusable */
              if((int)infu[j] != 0)
              {
                if(infu[j]->client==client) infu[j]->client= -1*infu[j]->client;
              }
            }
            goto a;
          }
          else	/* open failed */
          {
            return(EBIO_CONNOTRECOV);
          }
        }
        else
        {
          net[i]->nfiles = net[i]->nfiles + 1;
          st->stream = stream;
          st->client = client;
          return(0);
        }
      }
    }
  }
 
  client = 0;
  ii = (1 > st->nrec/5) ? 1 : st->nrec/5;
  openc(&client,&stream,st->finame,st->honame,action,&st->recl,&st->nrec,&ii);
 
  if(client == 0)	/* no connection to server */
  {
    return(EBIO_NOCONNECTION);
  }
  else
  {
    if(stream <=0 && stream >-100000)  /* open failed - stops a remote server */
    {
      if(client < 0) return(EBIO_EOF);	/* network channel is dead */
      stopsc(&client,&error);
      return(EBIO_DEADCHANNEL);
    }
    else	/* success */
    {
      st->stream = stream;
      st->client = client;
      if(ncon < NSERV)
      {
        if((net[ncon] = getNET()) == 0) return(-9);
        net[ncon]->honame = st->honame;
        net[ncon]->client = client;
        net[ncon]->nfiles = 1;
        ncon++;
      }
      return(0);
    }
  }
 
}
 
int
D 3
bosnres(int descriptor, int client, int stream, int mode)
E 3
I 3
bosnres(BOSIO *descriptor, int client, int stream, int mode)
E 3
{
D 3
  BOSIOptr st;
E 3
I 3
  BOSIO *st;
E 3
  int i, j, lun, error, i1, i2, nbytes, nskip, buf[12];
  char *action, *cp, ch[40], *bositoa();
  char *atmp = "a+b,recfm=f,lrecl=";
 
  error = EBIO_EOF;
 
  for(i=0; i<NUNITS; i++)	/* find corresponding unit */
  {
    if((int)infu[i] == 0) goto a10;		/* ??? */
    st = infu[i];
    if(st->client == client && st->stream == stream)
    {
      lun = i;
      goto a20;
    }
  }
a10:
  return(error);	/* unit not found */
 
a20:	/* unit found, try to reopen */
 
  if(client > 0)	/* this is the first time network problem is found */
  {			/* for the channel; remove the entry for host from */
			/* the control table first ... */
    for(i=0; i < ncon; i++)
    {
      if(client == net[i]->client) goto a40;
    }
a40:
    if(i <= ncon)
    {
      for(j=i; j < ncon; j++)
      {
        net[j]->honame = net[j+1]->honame;
        net[j]->client = net[j+1]->client;
        net[j]->nfiles = net[j+1]->nfiles;
      }
    }
    ncon--;
    for(i=0; i<NUNITS; i++)	/* ... and mark all units opened through */
    {				/* this channel as unusable */
      if((int)infu[i] != 0)
      {
        st = infu[i];
        if(st->client == client) st->client = -1*st->client;
      }
    }
  }
 
  if(st->action == 1)
    action = "rb";
  else if(st->action == 2)	/* if was 'w', reopen as 'u' to save written */
    action = "r+b";
  else if(st->action == 3)
  {
    bcopy(atmp,ch,strlen(atmp));
    action = strcat(ch,bositoa(st->recl));
  }
  else if(st->action == 4)
    action = "r+b";
 
  error = fpopen(descriptor,action);
  if(error != 0)
  {
    return(EBIO_EOF);	/* reconnection failed */
  }
  else		/* reconnection successful */
  {
    stream = st->stream;
    client = st->client;
  }
 
  if(mode == 0) return(0);
  if(st->access == 2) return(0);
 
  if(st->access == 1)	/* reset status for sequential file */
  {
    if(st->cursta == 1 && st->cursta == 3)	/* reading or writing */
    {
      if(st->cursta == 1)
        nskip = st->recl * st->innum;
      else
        nskip = st->recl * st->outnum;
      nbytes = 0;
      readc(&client,&stream,&nskip,&nbytes,buf,&error);
      if(error == nbytes) return(0);
      return(error);
    }
    else
    {
      return(0);	/* no reset for other status (realy ?) */
    }
  }
 
  return(0);
}
 
int
D 3
bosIoctl(int descriptor, char *option, void *arg)
E 3
I 3
bosIoctl(BOSIO *descriptor, char *option, void *arg)
E 3
{
  BOSIOptr BIOstream;
D 3
  BIOstream = (BOSIOptr)descriptor;
E 3
I 3
  BIOstream = descriptor;
E 3

  if(strcmp(option,"host") == 0)	/* set host name */
  {
    BIOstream->honame  = (char *)arg;
  }
  else
  {
    return(EBIO_UNKNOWNOPTION);				/* unknown option */
  }

  return(0);
}
 
 
D 3
char
*bositoa(int number)	/* integer -> char string */
E 3
I 3
char *
bositoa(int number)	/* integer -> char string */
E 3
{
  int i, j, k;
  char *ch;
 
  /* the number of digits */
 
  i = 0;
  j = number;
  do
  {
    i++;
    j = j/10;
  }
  while(j != 0);
 
  ch = (char *) MALLOC(i+1); /* allocate space for string */
  for(j=0; j<i; j++)
  {
    k = number % 10;
    switch(k)
    {
      case 0: ch[i-j-1] = '0'; break;
      case 1: ch[i-j-1] = '1'; break;
      case 2: ch[i-j-1] = '2'; break;
      case 3: ch[i-j-1] = '3'; break;
      case 4: ch[i-j-1] = '4'; break;
      case 5: ch[i-j-1] = '5'; break;
      case 6: ch[i-j-1] = '6'; break;
      case 7: ch[i-j-1] = '7'; break;
      case 8: ch[i-j-1] = '8'; break;
      case 9: ch[i-j-1] = '9'; break;
    }
    number = number/10;
  }
  ch[i] = '\0';
 
  return(ch);
}
 
E 1
