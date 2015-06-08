
/* ttload_.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tcl.h>
#include "coda.h"
#include "tt.h"


#define MYSQL_NOT_IMPLEMENTED \
 printf("%s at %i : Loading translation table from MYSQL DB not implemented ==> FATAL !!!\n"\
		  ,__FILE__,__LINE__);

#define NUMTTFIELDS sizeof(TTRow)/4
#define MAXSTRLNG 128

static int UseMYSQL = 0;

static char* Version = " VERSION: ttload_nomysql.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;
static int   nline;
static TTSPtr ttpwork=NULL;


/* local prototypes */
static void   *TT_OpenTable_(int);
static TTRPtr  TT_GetRow_(void *);
static int     TT_CloseTable_(void *);
static TTSPtr  TT_Alloc_(void);
static void    TT_Clean_(TTSPtr);
static FILE   *TT_OpenTabFile_(int);
static int     TT_ProcessRow_(TTRPtr);
static void    TT_PrintSlot_(int);
static void    TT_PrintSlots_(void);
static int     TT_GetTTStatistics_(void);




/******************************************************************************

  Routine             : TT_load

  Parameters          : none

  Discription         : This routine loads translation tables in structure look

******************************************************************************/

TTSPtr 
TT_LoadROCTT_(int roc, TTSPtr TTP)
{
  void *tth;
  int    i;
  TTRPtr r;
  char *env;

  ttpwork=TTP;

  /* MSG1("TT_Load : \"%s\"\n",Version);*/
  /* MSG1("TT_Load : compiled at \"%s\"\n",CompileTime);*/

  if(ttpwork == NULL) /* TT Struct not yet allocated */
  {
    printf("ttload: allocate translation table memory\n");
	if( (ttpwork=TT_Alloc_()) == NULL ) /* Can't alloc */
    {
	  MSG("Can`t Allocate memory for translation table ==> FATAL !!!\n");
	  exit(1);
	}
  }
  else /* TT Struct exists - needs cleanup */
  {
    printf("ttload: cleanup translation table memory\n");
	TT_Clean_(ttpwork);
  }

  /* fill ROC id (do that before Translation Table opening: if open fails
     we will have roc # anyway) */
  
  ttpwork->roc = roc;

  if ( (tth=TT_OpenTable_(roc)) == NULL) /* Fails open Translation table */
  {
    MSG1("Can`t Open translation table for ROC #%i ==> FATAL !!!\n",roc);
    return NULL;
  }
  
  /* fill the number of statistic words (must be less then MAXSTATWD !!!) */
  
  if(ttpwork->roc <= 11) /* we have 34 stat words for DC */
  {
	ttpwork->statname = *((unsigned long *)"DCST");
    ttpwork->nstatwd = 34; 
  }
  else                   /* and none for the rest        */
  {
    ttpwork->statname = *((unsigned long *)"    ");
    ttpwork->nstatwd = 0;
  }
  for (i=0; i<26; i++) ttpwork->type[i] = -1;       /* not filled */

  /**************** Now process TT line by line *******************/
  /* MSG1("Start to process rows ; handle = 0x%8.8X\n",tth); */
  
  while ( (r = TT_GetRow_(tth)) != NULL ) {
	TT_ProcessRow_(r); 
  }

  TT_CloseTable_(tth);

  TT_GetTTStatistics_();

  return ttpwork;
}

/*********************************************************************
 * Get one entry from TT
 *********************************************************************/

static TTRPtr 
TT_GetRow_(void *tth)
{
  int  nread=0,badline=0,comment,i;
  char str[MAXSTRLNG], *s, nm[10];
  static TTRow  r;
  unsigned long tag; /* for backward compartibility */

  if (UseMYSQL) {
	MYSQL_NOT_IMPLEMENTED;
	return NULL;
  }
  else
  {
    do 
    {
      s=fgets(str,MAXSTRLNG,(FILE*)tth);
      nline++;
      if (s == NULL)
      {
        if (ferror((FILE*)tth)) /* check for errors */
        {
          MSG1("Error reading TT file at line # %i\n",nline);
          perror("TT_GetRow_: Error in fgets()");
        }
		if (feof ((FILE*)tth)) /* check for End-Of-File */
        {
          ; /* MSG1("Found end of TT file at line # %i\n",nline); */
        }
        return NULL;              /* Stop reading */
      }
	  str[strlen(str)-1] = 0;
	  comment = (*str==0 || *str=='#' || *str=='!' || *str==';') ;
	  badline = 0;
	  /* printf("String %i: <%s>",nline,str); */
	  if (!comment)
      {
	    nread = sscanf (str,"%4c %i %i %i %i %i %i %i %i %i %i",
	  				  &r.nm,&r.nr,&r.sd1,&r.sd2,&r.pl,&tag,
		   			  &r.st,&r.roc,&r.sl,&r.typ,&r.chn);
        /*printf("=+=> %x(%d) %x(%d)\n",r.sd1,r.sd1,r.sd2,r.sd2);*/
		/* printf(" : <%s>\n",&r.nm); */
	    badline = (nread != NUMTTFIELDS + 1); /* one extra field in line (tag) */
	    if (badline)
        {
	  	  MSG4("Error pasing line # %i of TT file ; nItems=%i Nfields=%i ; \n"
				 "Line was:\"%s\"\n",nline,nread,NUMTTFIELDS-1,str);
		  /* return NULL; */
		}		
	  }
      else
      {
		; /* MSG1("Found comment at line # %i\n",nline); */
	  }		  
    } 
	while(comment || badline);

	return(&r);
  }
}


/*********************************************************************
 *
 *********************************************************************/
#define slot    r->sl
#define channel r->chn

static int 
TT_ProcessRow_(TTRPtr r) 
{
  int type,i,bosid,idnr;
  char *chara, name[5] = {' ',' ',' ',' ','\0'};
  /* 
  printf("Row %4d: %4.4s#%d id %2.2d:%3.3d:%d  Addr %2.2d:%2.2d\n"
		 ,nline,&(r->nm),r->nr,r->sd1,r->sd2,r->pl,r->sl,r->chn);
  */   
  /* We want process only data corresponding to that ROC */
  if (r->roc != ttpwork->roc) {
	MSG3("Changed ROC # %i to %i at line # %i\n",ttpwork->roc,r->roc,nline);
	return 0; 
  }
  if (r->sl > 26 || r->sl < 0) {
    MSG2("Wrong slot number %i in line %i\n",r->sl,nline);
	return 0;
  }
  if (r->chn > 256 || r->chn < 0) {
    MSG2("Wrong channel number %i in line %i\n",r->chn,nline);
	return 0;
  }
  
  if     (r->typ == 1872)  type = 0; /* even - tdc */
  else if(r->typ == 1881)  type = 1; /* odd  - adc */
  else if(r->typ == 1877)  type = 2; /* even - tdc */
  else if(r->typ == 1190)  type = 4; /* even - tdc */
  else                               /* unknown */
  {
	MSG2("Unknown board type # %i at line # %i\n",r->typ,nline);
	return 0; /* skip this junk */
  }

  if (ttpwork->type[slot] == -1 ) ttpwork->type[slot] = type;
  else if (ttpwork->type[slot] != type)
  {
	MSG4("Slot #%i : type # %i changed to %i at line # %i\n",
		 slot,ttpwork->type[slot],type,nline);
	return(0); /* skip this junk */
  }

  /* calculate 'bosid' */
  bosid = r->sd1*256 + r->sd2;

  /* calculate 'idnr' from 'name' and 'nr' */
  chara = (char *)&r->nm;
  for(i=0; i<4; i++) name[i] = chara[i];
  idnr = (bosMgetid_(name) << 16) + r->nr;
  /*printf("name=>%4.4s< nr=%d -> idnr=0x%08x\n",&r->nm,r->nr,idnr);*/

  /* fill the table */

  if(type == 0)      /* channel# --> 0..7 bits (Lecroy 1872A) */
  {
    ttpwork->idnr[slot][channel]   = idnr;
    ttpwork->name[slot][channel]   = r->nm;
    ttpwork->nr[slot][channel]     = r->nr;
    ttpwork->place[slot][channel]  = r->pl;
    ttpwork->id[slot][channel]     = bosid;

/* repeat for setting bit 23 */

    ttpwork->idnr[slot][128+channel]   = idnr;
    ttpwork->name[slot][128+channel]   = r->nm;          /* bank name */
    ttpwork->nr[slot][128+channel]     = r->nr;			 /* bank#     */
    ttpwork->place[slot][128+channel]  = r->pl;			 /* place#    */
    ttpwork->id[slot][128+channel]     = bosid;          /* id        */
  }

  if(type == 1)			/* channel# --> 1..7 bits (Lecroy 1881M) */
  {
    int chan = r->chn << 1;

    ttpwork->idnr[slot][chan]   = idnr;
    ttpwork->name[slot][chan]   = r->nm;                  /* bank name */
    ttpwork->nr[slot][chan]     = r->nr;                  /* bank#     */
    ttpwork->place[slot][chan]  = r->pl;                  /* place#    */
    ttpwork->id[slot][chan]     = bosid;                  /* id        */

/* repeat for setting bit 0 */

    ttpwork->idnr[slot][chan+1]   = idnr;
    ttpwork->name[slot][chan+1]   = r->nm;                /* bank name */
    ttpwork->nr[slot][chan+1]     = r->nr;                /* bank#     */
    ttpwork->place[slot][chan+1]  = r->pl;                /* place#    */
    ttpwork->id[slot][chan+1]     = bosid;                /* id        */
	/*
printf("slot=%d chan=%d -> name %4.4s\n",slot,chan,&(ttpwork->name[slot][chan]));
	*/
  }

  if(type == 2)	/* channel# --> 1..8 bits (Lecroy 1877 with multiplexing) */
  {
    ttpwork->idnr[slot][channel]       = idnr;
    ttpwork->name[slot][channel]       = r->nm;
    ttpwork->nr[slot][channel]         = r->nr;
    ttpwork->id2[slot][r->pl][channel] = bosid;
  }

  if(type == 4)	/* channel# --> 1..8 bits (CAEN 1190 pipeline) */
  {
    ttpwork->idnr[slot][channel]       = idnr;
    ttpwork->name[slot][channel]       = r->nm;
    ttpwork->nr[slot][channel]         = r->nr;
    ttpwork->id2[slot][r->pl][channel] = bosid;
  }
  
  return(1);
}

/*********************************************************************
 *
 * TT_GetTTStatistics fills "ttpwork->nplace[slot][chan]"  values, which
 * defines how many short's should be allocated for a BOS Bank row. It
 * implies that different detectors are alined on slot boundary.
 *
 *********************************************************************/
int 
TT_GetTTStatistics_( void ) 
{
  int  sl, ch, i;
  char *chara, name[5] = {' ',' ',' ',' ','\0'};

  for(sl=FIRST_SLOT; sl<=LAST_SLOT; sl++)
  {
	if(ttpwork->type[sl] == -1) continue;
	for(ch=0; ch<MAXCHANNEL; ch++)
    {
      if(ttpwork->id[sl][ch] != 0 || ttpwork->id2[sl][0][ch] != 0 || ttpwork->id2[sl][1][ch] != 0)
	  {
        chara = (char *)&ttpwork->name[sl][ch];
        for(i=0; i<4; i++) name[i] = chara[i];
        ttpwork->nplace[sl][ch] = bosMgetncol_(name);

/* extra test - keep it for a while */
if((ttpwork->name[sl][ch] ==  (*((int*)"DC  ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"DC0 ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"SCS ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"ECS ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TAGE")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TLV1")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TL1 ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TL2 ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TRGT")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"CC  ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"CC1 ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"EC  ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"EC2 ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"IC  ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"ELSR")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"FBPM")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"TAGT")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"ST  ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"CL  ")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"CALL")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"SC  ")) && ttpwork->nplace[sl][ch] != 5) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"EC1 ")) && ttpwork->nplace[sl][ch] != 5) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"STMP")) && ttpwork->nplace[sl][ch] != 5) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"DSPS")) && ttpwork->nplace[sl][ch] != 3) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"DSPC")) && ttpwork->nplace[sl][ch] != 8) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"DSTC")) && ttpwork->nplace[sl][ch] != 9) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"L2H ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"L2S ")) && ttpwork->nplace[sl][ch] != 2) ||
   (ttpwork->name[sl][ch] ==  (*((int*)"PRIM")) && ttpwork->nplace[sl][ch] != 2))
  {
    printf("ttload: ERROR: name >%4.4s< ncol=%d\n",&ttpwork->name[sl][ch],ttpwork->nplace[sl][ch]);
    exit(0);
  }


	  }
	}
  }

  return(0);
}
/*********************************************************************
 *
 *********************************************************************/
static void *
TT_OpenTable_(int id)
{
  char cmd[256];
  /* MSG1("Open table for id # %i\n",id);*/

  if (UseMYSQL) {
	MYSQL_NOT_IMPLEMENTED;
	return NULL;
  }
  else {
	return (void *)TT_OpenTabFile_(id);
  }
}

/*********************************************************************
 *
 *********************************************************************/
static FILE * 
TT_OpenTabFile_(int id)
{
  char *fn,*dir;
  FILE* f;
  int   l;

  if ( (dir=getenv("TT_DIR")) == NULL ) dir = TTDir;
  l  = strlen( dir ) + 12 ;
  /* MSG2("length of TTDir = \"%s\" is %i bytes\n",TTDir,strlen( TTDir ));*/
  fn = malloc( l );
  /* MSG2("Malloc %i bytes for Filename returns 0x%8.8X\n",l,fn);*/
  sprintf(fn,"%s/ROC%2.2i.tab",dir,id);
  nline = 0;
  MSG2("Open TT file \"%s\" for id # %i\n",fn,id);

  if( (f=fopen(fn,"r")) == NULL )
  {
	MSG2("Can't open file \"%s\" for TT # %i ==> FATAL !!!\n",fn,id);
	free(fn);
	return (void *)NULL;	
  }

  free(fn);
  return (void *)f;
}

/*********************************************************************
 *
 *********************************************************************/
static int 
TT_CloseTable_(void *tth)
{
  if (UseMYSQL)
  {
    MYSQL_NOT_IMPLEMENTED;
    return 0;	
  }
  else
  {
    printf("ttload.c: close translation table file.\n");
    fclose ((FILE*)tth);
    return 1;
  }
}

/*********************************************************************
 *
 *********************************************************************/
static TTSPtr 
TT_Alloc_(void)
{
  static void *ptr;
  printf("ttload: Allocating %i bytes for TTS\n",sizeof(TTS));
  /* MSG1("Allocating %i bytes for TTS\n",sizeof(TTS));*/
  ptr = calloc(1,sizeof(TTS));
  printf("ttload: ttpwork at 0x%8.8X\n",ptr);
  /* MSG1("ttpwork at 0x%8.8X\n",ptr);*/
  return (TTSPtr)ptr;
}

/*********************************************************************
 *
 *********************************************************************/
static void 
TT_Clean_(TTSPtr ttp)
{
  /* MSG2("Zeroing %i bytes in TTS; ptr=0x%8.8X\n",sizeof(TTS),ttp);*/
  bzero(ttp, sizeof(TTS));
}

/******************************************************************************

  Routine             : printlook

  Parameters          : none

  Discription         : This routine prints look structure

******************************************************************************/

static void 
TT_PrintSlot_( int sl )
{
  int chan,maxchn=64,ch;

  printf("====================  S L O T  # %2i type %i  ===================== \n"
		 ,sl,ttpwork->type[sl]);
  if (ttpwork->type[sl]==-1) return;

  if (ttpwork->type[sl]==0) maxchn=64;
  if (ttpwork->type[sl]==1) maxchn=64;
  if (ttpwork->type[sl]==2) maxchn=96;

  for(chan=1; chan<maxchn; chan++)
	{
	  ch=chan;
	  if (ttpwork->type[sl]==1) {
		ch=chan<<1;
		printf("chan=%2d : %4.4s#%1i  id 0x%4.4X:%i"
			   " -- %4.4s#%1i  id 0x%4.4X:%i\n",chan
			   ,&ttpwork->name[sl][ch],   (int)ttpwork->nr[sl][ch]
			   ,(int)ttpwork->id[sl][ch], (int)ttpwork->place[sl][ch]
			   ,&ttpwork->name[sl][ch+1],   (int)ttpwork->nr[sl][ch+1]
			   ,(int)ttpwork->id[sl][ch+1], (int)ttpwork->place[sl][ch+1]);
	  }
	  else if (ttpwork->type[sl]==0) {
		printf("chan=%2d : %4.4s#%1i  id 0x%4.4X:%i"
			   " -- %4.4s#%1i  id 0x%4.4X:%i\n",chan
			   ,&ttpwork->name[sl][ch],   (int)ttpwork->nr[sl][ch]
			   ,(int)ttpwork->id[sl][ch], (int)ttpwork->place[sl][ch]
			   ,&ttpwork->name[sl][ch+128],   (int)ttpwork->nr[sl][ch+128]
			   ,(int)ttpwork->id[sl][ch+128], (int)ttpwork->place[sl][ch+128]);
	  }
	  else if (ttpwork->type[sl]==2) {
		printf("chan=%3d %4.4s#%1i  id 0x%4.4X:0x%4.4X\n",chan
			   ,&ttpwork->name[sl][ch],(int)ttpwork->nr[sl][ch]
			   ,(int)ttpwork->id2[sl][0][chan],(int)ttpwork->id2[sl][1][chan]);
	  }
	} 
}

/******************************************************************************
 *
 ******************************************************************************/

static void 
TT_PrintSlots_( )
{
  int sl;
  
  for(sl=1; sl<26; sl++) {
    printf("look->type[%2d]=%d name=%4.4s nr=%i\n",
		   sl,ttpwork->type[sl],&(ttpwork->name[sl][0]),(int)ttpwork->nr[sl][0]);
  }
}
  
