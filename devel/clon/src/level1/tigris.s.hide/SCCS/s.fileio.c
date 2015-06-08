h28128
s 00001/00000/01519
d D 1.11 08/04/01 16:37:01 boiarino 12 11
c return on first error
c 
e
s 00034/00041/01485
d D 1.10 08/04/01 14:49:33 boiarino 11 10
c *** empty log message ***
e
s 00011/00000/01515
d D 1.9 06/09/01 00:35:31 boiarino 10 9
c dummy VXWORKS
c 
e
s 00003/00069/01512
d D 1.8 06/07/02 01:18:11 boiarino 9 8
c remove OLDLOAD
e
s 00010/00002/01571
d D 1.7 05/02/25 15:24:21 boiarino 8 7
c *** empty log message ***
e
s 00001/00001/01572
d D 1.6 02/09/09 01:00:16 boiarino 7 6
c fix bug: close -> fclose
e
s 00464/00404/01109
d D 1.5 02/08/25 19:43:07 boiarino 6 5
c big cleanup (globals, locals, prototypes)
c fix some place(s) to make async stuff works
e
s 00005/00003/01508
d D 1.4 02/08/22 20:52:25 boiarino 5 4
c fix header files warnings
c 
e
s 00019/00005/01492
d D 1.3 02/07/14 14:50:27 boiarino 4 3
c write files to CLON_PARMS/ts/ area
c 
e
s 00397/00283/01100
d D 1.2 02/07/06 23:42:54 boiarino 3 1
c new and old methods - use #ifdef OLDLOAD
c 
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/fileio.c
e
s 01383/00000/00000
d D 1.1 02/03/29 12:13:01 boiarino 1 0
c date and time created 02/03/29 12:13:01 by boiarino
e
u
U
f e 0
t
T
I 10

#ifndef VXWORKS

I 11

E 11
E 10
I 1
D 6

     
#ifndef	lint
static char RCSID[120] = "$ID$";
#endif
E 6
/*
|    file name - fileio.c
|====================================================================
     By Stephan G. Lemon - with major mods to fit into tigris by JAM
|====================================================================
*/
I 6
D 8
     
E 8
I 8

D 11
#define TEST_SETUP

E 11
E 8
#ifndef	lint
static char RCSID[120] = "$ID$";
#endif
E 6

D 6
#include "stdio.h"
E 6
I 6
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
E 6
#include "Hv.h"
#include "ced.h"
D 6
#include "scintlook.h"
E 6
I 6
#include "tigris.h"
E 6
I 3
#include "level1.h"
I 5
D 6
#include "sgl.h"
E 5
E 3

E 6
I 5
#include "vme_tcpInfo.h"
D 6
#include "vme_download.h"
E 6
I 6
#include "vme_download.h" 
E 6

I 6
struct SLsecdef Sector[7]; /* global, holds sector data of 6 sectors 1-6 */
struct phase3_def phase3; /* global, 3D array of trigger definitions in event processor */
struct phase3_def JAMphase3; /* global, 3D array of trigger definitions in event processor */
E 6

E 5
D 6
#define LOCAL static
#define VOID  void
#define INT   int
E 6
I 6
char InFileName[512]; /* global, holds name of input file to read data from */
char OutFileName[512]; /* global, holds name of output file to write data to */
char dlhn[1024]; /* global, dlhn = Down Load Host Name */
E 6

D 3
VOID FillSDArray() ;
VOID FillSPArray() ;
VOID FillEPArray() ;
VOID FillEDArray() ;
VOID FillSEArray() ;
VOID compare_phase1_addresses () ;
VOID make_php_address () ;
VOID compare_addresses () ;
E 3
I 3
D 6
VOID FillSDArray();
VOID FillSPArray();
VOID FillEPArray();
VOID FillEDArray();
VOID FillSEArray();
VOID make_php_address();
VOID compare_phase1_addresses(unsigned short int memory_data[65536],
                         unsigned short int check_address, int which_bit);
VOID compare_addresses(unsigned short int memory_data[65536],
                  unsigned short int check_address,
                  unsigned short int X_mask, int which_bit, int max_X);
E 6
E 3

I 3
D 5
#include "sgl.h"
#include "vme_download.h"
E 3

E 5
D 3
#include   "sgl.h"
#include   "vme_download.h"
/*
#include "/usr/user3/dputnam/lib/vme_download/vme_download.h"
*/
extern int FileOnlyMode ;
E 3
I 3
D 6
extern int FileOnlyMode;
extern VisSectorDef secdef[];
extern BitsLookup bitslookup[];
E 6
E 3

D 3
extern VisSectorDef  secdef [] ;
extern BitsLookup bitslookup [] ;
E 3
I 3
D 6
extern char *dlhn;
E 6
I 6
/* local variables */
E 6
E 3

D 3
extern char *dlhn ;

E 3
D 6
typedef int BOOLEAN; 
E 6
I 6
static int TOF[65]; /* holds 64 TOF vars in Sec.v to control thresholds */
static int phase1A_active = 1;
E 6
D 3
           
E 3

I 3
D 6

E 3
/* structure variables for use */
D 3
 struct SLsecdef Sector[7];           /* holds sector data of 6 sectors 1-6                 */
LOCAL int TOF[65];                         /* holds 64 TOF vars in Sec.v to control thresholds   */
struct phase3_def phase3;            /* 3D array of trigger definitions in event processor */
struct phase3_def JAMphase3;            /* 3D array of trigger definitions in event processor */
E 3
I 3
struct SLsecdef Sector[7]; /* holds sector data of 6 sectors 1-6 */
LOCAL int TOF[65]; /* holds 64 TOF vars in Sec.v to control thresholds */
struct phase3_def phase3; /* 3D array of trigger definitions in event processor */
struct phase3_def JAMphase3; /* 3D array of trigger definitions in event processor */
E 3

LOCAL int phase1A_active = 1;

E 6
/* the SD's and ED's hold value for which SD or ED is currently bound to TOF variables */
/* since overlapping bit definitions must only highlight selected SD and ED's          */
D 6
LOCAL int sd0_2 = 0; 
LOCAL int sd3_5 = 3; 
/* -- JAM - (-Wall gnu) ---
 *  LOCAL int sp0_1 = 6;
 *  LOCAL int sp2_3 = 8;
 *  LOCAL int ed0_2 = 0;
 * --------------------- */
LOCAL int ed3_5 = 3;
LOCAL enable_spare1a[6];
LOCAL enable_spare1b[6];
E 6
I 6
static int sd0_2 = 0;
static int sd3_5 = 3;
static int ed3_5 = 3;
D 11
static enable_spare1a[6];
static enable_spare1b[6];
E 11
I 11
static int enable_spare1a[6];
static int enable_spare1b[6];
E 11
E 6

/* Miscellaneous or main procedure variables */
D 6
LOCAL int WhichSector;          /* sector which is currently active if in phase1 or phase2 views */
char InFileName[512];            /* holds name of input file to read data from  */
E 6
I 6
static int WhichSector;          /* sector which is currently active if in phase1 or phase2 views */
E 6

D 6
char OutFileName[512];           /* holds name of output file to write data to  */
char OutFileTitle[512];          /* holds title line for output file */
E 6
I 6
static char OutFileTitle[512]; /* holds title line for output file */
static char OutFileComment[512]; /* holds comment line for output file */
E 6

D 6
char OutFileComment[512];        /* holds comment line for output file */

E 6
I 3
D 9
#ifndef OLDLOAD
E 9
static unsigned short bigdata[BIGDATA]; /* download trigger data */
static unsigned short bigref[BIGDATA]; /* upload trigger data */
D 9
#endif
E 9
E 3

I 9

E 9
D 3


typedef struct Node {
E 3
I 3
typedef struct Node
{
E 3
  struct Node *next;
  char z;
} Node;
D 3

E 3
D 6
Node *head, *tail;
E 6
I 6
static Node *head, *tail;
E 6


I 6
/* local prototypes */
E 6

I 6
static void SetTOFvars();
static void DuplicateSector(int from_sec, int to_sec);
static void addS(char c);
static char *terminateS(int myCnt);
static void FillSDArray(FILE *fd, int sectornum, int phase, int i, int jstart,
                        int jend);
static void FillSPArray(FILE *fd, int sectornum, int phase, int i, int jstart,
                        int jend);
static void FillEDArray(FILE *fd, int sectornum, int phase, int i, int jstart,
                        int jend);
static void FillSEArray(FILE *fd, int sectornum, int se_num, int prod_num);
static void FillEPArray(FILE *fd, int trigger_num, int prod_num);
static void bigdata_write(unsigned short *data, char *filename);
static void compare_phase1_addresses(unsigned short int memory_data[65536],
                         unsigned short int check_address, int which_bit);
static void compare_addresses(unsigned short int memory_data[65536],
                  unsigned short int check_address,
                  unsigned short int X_mask, int which_bit, int max_X);
static void make_php_address(unsigned short int *check_address,
                 unsigned short int *X_mask,
                 int *max_X, int trig_num, int product, int which_address);


E 6
/* ----------------------- Function SetTOFvars --------------------------- */
/*
 *  Loops thru each TOF var and sets value to ( 1 + SD or ED number ) if the active 
 *  scintillator or extra data bit is 1.  By adding by the SD or ED number, it allows
 *  unique thresholds to be implemented in sec.v for the TOF vars.  This enables the TOF
 *  vars to be displayed as any one of the SD or ED vars in their respective colors.
 *  If a bit is 0, no change is made, the TOF var remains at 0, and no threshold is
 *  reached.
D 6
*/
E 6
I 6
 */
E 6
D 3
VOID SetTOFvars()
E 3
I 3

D 6
VOID
E 6
I 6
static void
E 6
SetTOFvars()
E 3
{
   int i, phase;

   if (phase1A_active)
     phase = 0;
   else
     phase = 1;

   for (i=1;i<17;i++)
   {
      TOF[i] = Sector[WhichSector].SD[phase][sd0_2][i];
      if (TOF[i] == 1)
         TOF[i] += sd0_2;
   }
   for (i=18;i<49;i=i+2)
   {
      TOF[i] = Sector[WhichSector].SD[phase][sd3_5][i];
      if (TOF[i] == 1)
         TOF[i] += sd3_5;
   }
     for (i=49;i<65;i++)
     {
        TOF[i] = Sector[WhichSector].ED[phase][ed3_5][i];
        if (TOF[i] == 1)
           TOF[i] += ed3_5;
     }
}

D 3

E 3
/* -------------------- Function DuplicateSector  ------------------------ */
/*
 * Called by ManageDupSector being passed from_sec, the sector from which data will be copied
 * and to_sec, the sector to receive the copied data. The SD,ED, and SE data from the from_sec
 * sector is then copied to the to_sec sector data memory locations.
*/
D 3
VOID DuplicateSector(from_sec, to_sec)
int from_sec;
int to_sec;
E 3
I 3

D 6
VOID
E 6
I 6
static void
E 6
DuplicateSector(int from_sec, int to_sec)
E 3
{
D 6
  INT i,j,k;  /* looping control variables */
E 6
I 6
  int i,j,k;  /* looping control variables */
E 6

D 3
  for (i=0;i<2;i++)
E 3
I 3
  for(i=0;i<2;i++)
E 3
  {
D 3
    for (j=0;j<6;j++)
E 3
I 3
    for(j=0;j<6;j++)
E 3
    {
D 3
      for (k=1;k<33;k++)
E 3
I 3
      for(k=1;k<33;k++)
E 3
        Sector[to_sec].SD[i][j][k] = Sector[from_sec].SD[i][j][k];
    }
  }
  for (i=0;i<2;i++)
  {
    for (j=0;j<4;j++)
    {
      for (k=1;k<17;k++)
        Sector[to_sec].SP[i][j][k] = Sector[from_sec].SP[i][j][k];
    }
  }
  for (i=0;i<2;i++)
  {
    for (j=0;j<6;j++)
      {
	for (k=1;k<33;k++)
	  Sector[to_sec].ED[i][j][k] = Sector[from_sec].ED[i][j][k];
      }
  }

  for (i=0;i<MaxSE;i++)
  {
    for (j=0;j<MaxProducts;j++)
    {
      for (k=0;k<16;k++)
	{
	  Sector[to_sec].SE[i][j][k] = Sector[from_sec].SE[i][j][k];
	}
    }
  }
}
I 6

E 6
/*
 * JAM - use this to build up a string. Call it with a character as an argument
 *
 */
D 6
void addS (c)
char c;
E 6
I 6

static void
addS(char c)
E 6
{
  Node *n = calloc (1, sizeof (Node));

  if (n == 0) {
    printf ("panic: no memory: addS: in ReadValues: in fileio.c\n");
    exit (-6);
  }
  n->z = c;
  if ((head == 0) && (tail == 0)) {
    head = n;
    tail = n;
    n->next = 0;
  } else {
    tail->next = n;
    tail = n;
  }
}
I 6

E 6
/*
 * JAM - return all the chars add from addS as 1 string.
 */
D 6
char * terminateS (myCnt)
int myCnt ;
E 6
I 6

static char *
terminateS(int myCnt)
E 6
{
  Node *p, *t;
  int j, i;
  char *s ;

  j=0;
  if (head == 0) 
    s = calloc (1, 4096) ;
  else {
    p = head;
    j=0;
    while (p) {
      p = p->next;
      j += 1;
    }
    if (myCnt != j) 
      printf ("terminateS: count differ: mc[%d] j[%d]\n", myCnt, j);

    s = calloc (1, (j+1)) ;
    p = head;
    j = 0;
    while (p) {
      s[j++] = p->z;
      t = p;
      p = p->next;
      free (t);
    }
    for (i=0; i<10; i++) {
      /*      printf (" ----====>%s< --- %c \n", s, s[j-i]); */
      if (s[j-i] == '\012')
	s[j-i] = 0;  /* remove the last line feed */
    }
  }
  
  head = 0;
  tail = 0;
  s[j] = 0;
D 6
  return s;
E 6
I 6

  return(s);
E 6
}
I 6

E 6
/* ---------------------- Function ReadValues -------------------------- */
/*
 * Using filename entered using text entry bar in a view, this procedure
 * reads in all data for phase1, phase2, and the event processor.  The
 * format of the output file must match the style used by the TSF data
 * routine.  Data represented as an X in the input/output files are 
 * converted to the number 2 or 0, depending upon section,  which is used
 *  in computation.
*/ 
D 6
int ReadValues()
E 6
I 6

int
ReadValues()
E 6
{
  int i, value;
  int sector_num, sector_event_num, product_number, trigger_num;  /* values of read in data */
  char *sz, s[80], strnum[5], c; /* Hold string values read in from input file */
  int eof = 0;              /* Boolean TRUE if EOF reached */
  int event_processor = 0;  /* Boolean to differentiate Product lines of phase 2 and php */
  int phase;                /* 0 if phase 1A and 1 if phase 1B */
D 6
  FILE* fdIn;
E 6
I 6
  FILE *fdIn;
E 6

D 6
  if((fdIn=fopen(InFileName,"r")) == NULL)
  {
    return(1);
  }
E 6
I 6
D 8
  if((fdIn=fopen(InFileName,"r")) == NULL) return(1);
E 8
I 8
  if((fdIn=fopen(InFileName,"r")) == NULL)
  {
    printf("ReadValues: cannot open trigger file >%s<\n",InFileName);
    exit(0);/*return(1);*/
  }
E 8
E 6

  phase = -1;
D 6
  
E 6
  while(!eof)
I 6
  {
    /* read first token on line into string s */
    i=0;
    while( ( (c=getc(fdIn)) != ' ') && (c != '\n') && (c != EOF) )
E 6
    {
D 6
      /* read first token on line into string s */
      i=0;
      while( ( (c=getc(fdIn)) != ' ') && (c != '\n') && (c != EOF) )
	    {
	      s[i] = c;
	      i++;
	    }
      if (c==EOF) eof = 1;
      s[i] = '\0';
      /* if string s is comment marker move to next line and begin loop again */
      if (strcmp(s,"/*")==0)
E 6
I 6
      s[i] = c;
      i++;
    }
    if(c==EOF) eof = 1;
    s[i] = '\0';
    /* if string s is comment marker move to next line and begin loop again */
    if (strcmp(s,"/*")==0) /* */
    {
      do
E 6
      {
D 6
	do
	{
	  c=getc(fdIn);
	} while (c != '\n');
E 6
I 6
        c=getc(fdIn);
      } while(c != '\n');
    }
    else
    {
      if(strcmp(s,"SECTOR")==0)
      {
        /* read in sector number */
        strnum[0] = getc(fdIn);
        strnum[1] = '.';
        strnum[2] = '\0';
        sector_num = atoi(strnum);
        event_processor = 0;
        if(sector_num == 1) phase++;
E 6
      }
D 6
      else
	{
	    if (strcmp(s,"SECTOR")==0)
	    {
	      /* read in sector number */
	      strnum[0] = getc(fdIn);
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      sector_num = atoi(strnum);
	      event_processor = 0;
	      if (sector_num == 1)
	        phase++;
	    }
	    if (strcmp(s,"SD") == 0)
	      {
                /* Read in SD# and then read data line */
E 6
I 6
      if(strcmp(s,"SD") == 0)
      {
        /* Read in SD# and then read data line */
E 6
		strnum[0] = getc(fdIn);
		strnum[1] = '.';
		strnum[2] = '\0';
		c = getc(fdIn);  /* read in : following SD# before reading data */
		value = atoi(strnum);
D 6
		if ((value >= 0) && (value <=2))
E 6
I 6
		if((value >= 0) && (value <=2))
E 6
		  FillSDArray(fdIn,sector_num,phase,value,1,16);
D 6
		else if ((value >= 3) && (value <=5))
E 6
I 6
		else if((value >= 3) && (value <=5))
E 6
		  FillSDArray(fdIn,sector_num,phase,value,17,32);
D 6
	      }
	    if (strcmp(s,"SP")==0)
	    {
E 6
I 6
      }
      if(strcmp(s,"SP")==0)
      {
E 6
	      /* Read in SP# and then read data line */
	      strnum[0] = getc(fdIn);
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      c = getc(fdIn); /* read in : following SP# before reading data */
	      value = atoi(strnum);
	      FillSPArray(fdIn,sector_num,phase,value,1,16);
D 6
	    }
	    if (strcmp(s,"ED")==0)
	    {
E 6
I 6
      }
      if(strcmp(s,"ED")==0)
      {
E 6
	      /* Read in ED# and then read data line */
	      strnum[0] = getc(fdIn);
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      c = getc(fdIn); /* read in : following ED# before reading data */
	      value = atoi(strnum);
	      if ((value >=0) && (value <=2))
	        FillEDArray(fdIn,sector_num,phase,value,1,16);
	      else if ((value >= 3) && (value <=5))
	        FillEDArray(fdIn,sector_num,phase,value,17,32);
D 6
	    }
	    if (strcmp(s,"EXTERNAL_SP_ENABLE")==0)
	    {
	      /* Read in enable for whichever phase is active */
	      strnum[0] = getc(fdIn);
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      if (phase == 0)
		enable_spare1a[sector_num] = atoi(strnum);
	      else
		enable_spare1b[sector_num] = atoi(strnum);
	    }
	    if (strcmp(s,"SECTOR_EVENT")==0)
	    {
	      /* Read in Sector Event # */
	      strnum[0] = getc(fdIn);
	      strnum[1] = getc(fdIn);
	      if ((strnum[1] == ' ') || (strnum[1] == '\n'))
	      {
		strnum[1] = '.';
		strnum[2] = '\0';
	      }
	      else
	      {
		strnum[2] = '.';
		strnum[3] = '\0';
	      }
	      sector_event_num = atoi(strnum);
	    }
	    if (strcmp(s,"Comment:")==0)
	    {
	      /* Read in phase 2 comment */
	      i=0;
	      do
	      {
		c=getc(fdIn);
		addS(c); 
	        /* s[i] = c; */
	        i++;
	      } while (c != '\n');
	      sz = terminateS(i) ;
	      /* JAM - 10/23/96 Comments are now VERY long !!!! */
	      Sector[sector_num].comment[sector_event_num] = strdup (sz);
              /*   JAM --- strcpy(Sector[sector_num].comment[sector_event_num], s);   */
	    }
	    if ((strcmp(s,"Product")==0) && (!event_processor))
	    {
	      /* Product line for phase 2. Read in product # and get data line */
	      i=0;
	      c = getc(fdIn);
	      while (c != ':')
	      {
		strnum[i]=c;
		i++;
		c = getc(fdIn);
	      }
	      strnum[i] = '.';
	      strnum[i+1] = '\0';
	      product_number = atoi(strnum);
	      FillSEArray(fdIn,sector_num,sector_event_num,product_number);
	    }
	    if (strcmp(s,"TRIGGER")==0)
	    {
	      /* read in trigger number */
	      event_processor = 1;  /* Boolean to differentiate Product lines as php not phase 2 */
	      strnum[0] = getc(fdIn);
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      trigger_num = atoi(strnum);
	    }
	    if (strcmp(s,"Trigger_Pulse_Width:")==0)
	    {
	      /* Read in trigger pulse width value */
	      strnum[0] = getc(fdIn);                   
	      strnum[1] = '.';
	      strnum[2] = '\0';
	      phase3.Trig_Delay[trigger_num] = atoi(strnum);
	    }
	    if (strcmp(s,"Enable_Async:")==0)
	    {
	      /* Read in enable async value */
	      strnum[0] = getc(fdIn);                   
	      strnum[1] = '.';
	      strnum[2] = '\0';
fprintf(stderr, "trigger_num: %d\n", trigger_num);
fprintf(stderr, "strnum: %s\n", strnum);
	      phase3.Enable_Async[trigger_num] = atoi(strnum);
fprintf(stderr, "phase3: %d\n", phase3.Enable_Async[trigger_num]);
	    }
	    if ((strcmp(s,"Product")==0) && (event_processor))
	    {
	      /* Product line for php. Read in product # and get data line */
	      i=0;
	      c = getc(fdIn);
	      while (c != ':')
	      {
		strnum[i]=c;
		i++;
		c = getc(fdIn);
	      }
	      strnum[i] = '.';
	      strnum[i+1] = '\0';
	      product_number = atoi(strnum);	
              FillEPArray(fdIn,trigger_num,product_number);
            }
	  }
E 6
I 6
      }
      if(strcmp(s,"EXTERNAL_SP_ENABLE")==0)
      {
        /* Read in enable for whichever phase is active */
        strnum[0] = getc(fdIn);
        strnum[1] = '.';
        strnum[2] = '\0';
        if(phase == 0)
          enable_spare1a[sector_num] = atoi(strnum);
        else
          enable_spare1b[sector_num] = atoi(strnum);
      }

      if(strcmp(s,"SECTOR_EVENT")==0)
      {
        /* Read in Sector Event # */
        strnum[0] = getc(fdIn);
        strnum[1] = getc(fdIn);
        if((strnum[1] == ' ') || (strnum[1] == '\n'))
        {
          strnum[1] = '.';
          strnum[2] = '\0';
        }
        else
        {
          strnum[2] = '.';
          strnum[3] = '\0';
        }
        sector_event_num = atoi(strnum);
      }

      if(strcmp(s,"Comment:")==0)
      {
        /* Read in phase 2 comment */
        i=0;
        do
        {
          c=getc(fdIn);
          addS(c); 
          /* s[i] = c; */
          i++;
        } while (c != '\n');
        sz = terminateS(i);
printf("sz >%d<\n",sz);fflush(stdout);
        /* JAM - 10/23/96 Comments are now VERY long !!!! */
        Sector[sector_num].comment[sector_event_num] = strdup(sz);
        /* JAM --- strcpy(Sector[sector_num].comment[sector_event_num], s); */
      }

      if((strcmp(s,"Product")==0) && (!event_processor))
      {
        /* Product line for phase 2. Read in product # and get data line */
        i=0;
        c = getc(fdIn);
        while (c != ':')
        {
          strnum[i] = c;
          i++;
          c = getc(fdIn);
        }
        strnum[i] = '.';
        strnum[i+1] = '\0';
        product_number = atoi(strnum);
        FillSEArray(fdIn,sector_num,sector_event_num,product_number);
      }

      if(strcmp(s,"TRIGGER")==0)
      {
        /* read in trigger number */
        event_processor = 1; /* Boolean to differentiate Product lines as php not phase 2 */
        strnum[0] = getc(fdIn);
        strnum[1] = '.';
        strnum[2] = '\0';
        trigger_num = atoi(strnum);
      }

      if(strcmp(s,"Trigger_Pulse_Width:")==0)
      {
        /* Read in trigger pulse width value */
        strnum[0] = getc(fdIn);                   
        strnum[1] = '.';
        strnum[2] = '\0';
        phase3.Trig_Delay[trigger_num] = atoi(strnum);
      }

      if(strcmp(s,"Enable_Async:")==0)
      {
        /* Read in enable async value */
        strnum[0] = getc(fdIn);
        strnum[1] = '.';
        strnum[2] = '\0';
    fprintf(stderr, "trigger_num: %d\n", trigger_num);
    fprintf(stderr, "strnum: %s\n", strnum);
        phase3.Enable_Async[trigger_num] = atoi(strnum);
    fprintf(stderr, "phase3: %d\n", phase3.Enable_Async[trigger_num]);
      }

      if((strcmp(s,"Product")==0) && (event_processor))
      {
        /* Product line for php. Read in product # and get data line */
        i=0;
        c = getc(fdIn);
        while(c != ':')
        {
          strnum[i]=c;
          i++;
          c = getc(fdIn);
        }
        strnum[i] = '.';
        strnum[i+1] = '\0';
        product_number = atoi(strnum);	
        FillEPArray(fdIn, trigger_num, product_number);
      }
E 6
    }
D 6
    fclose(fdIn);
    return(0);
E 6
I 6
  }

  fclose(fdIn);
  return(0);
E 6
}

/* -------------------- Function SaveFinalValues ----------------------- */
/*
 * Loads up a view with three text entry areas, one for the output file name, one for a short title
 * for the file, and one for a comment line for the file. All data values are
 * stored to the file along with comments which are preceeded by 
*/
D 6
int SaveFinalValues()
E 6
I 6

int
SaveFinalValues()
E 6
{
D 6
  int i,j,k;
  int WhichSector, unused_product, product_num, phase, counter;
E 6
I 6
  int i, j, k;
  int WhichSector, unused_product, product_num, phase, counter, se_yes;
E 6
  int beginloop, endloop;
D 6
  FILE* fdOut;
E 6
I 6
  FILE *fdOut;
E 6

D 6
  if ((fdOut=fopen(OutFileName,"w")) == NULL) {
    return(1);
  }
E 6
I 6
  if((fdOut=fopen(OutFileName,"w")) == NULL) return(1);
E 6

D 6
  fprintf(fdOut,"/* Title   :  %s\n\n",OutFileTitle);
  fprintf(fdOut,"/* Comment :  %s\n\n",OutFileComment);
  fprintf(fdOut,"/* Router Definitions:\n");
E 6
I 6
  fprintf(fdOut,"/* Title   :  %s\n\n",OutFileTitle);   /* */
  fprintf(fdOut,"/* Comment :  %s\n\n",OutFileComment); /* */
  fprintf(fdOut,"/* Router Definitions:\n");            /* */
E 6

D 6
  for (phase=0;phase<2;phase++)
E 6
I 6
  for(phase=0; phase<2; phase++)
E 6
  {
D 6
    if (phase == 0)
      fprintf(fdOut,"/* Triggers 1-4\n");
E 6
I 6
    if(phase == 0)
      fprintf(fdOut,"/* Triggers 1-4\n"); /* */
E 6
    else
D 6
      fprintf(fdOut,"/* Triggers 5-8\n");
E 6
I 6
      fprintf(fdOut,"/* Triggers 5-8\n"); /* */
E 6

D 6
    for (WhichSector=1;WhichSector<=6;WhichSector++)
E 6
I 6
    for(WhichSector=1; WhichSector<=6; WhichSector++)
E 6
    {
D 6
      fprintf(fdOut,"/* Sector %d - Phase 1\n",WhichSector);
E 6
I 6
      fprintf(fdOut,"/* Sector %d - Phase 1\n",WhichSector); /* */
E 6
      fprintf(fdOut,"SECTOR %i\n",WhichSector);
D 6
      fprintf(fdOut,"/* 1st 3 bits use 1st 16 scintillators (Bit order is 1-16)\n");
      for (i=0; i<3; i++) {
E 6
I 6

      fprintf(fdOut,
         "/* 1st 3 bits use 1st 16 scintillators (Bit order is 1-16)\n"); /* */
      for(i=0; i<3; i++)
      {
E 6
        fprintf(fdOut,"SD %d:",i);
D 6
        for (j=1; j<16; j++) {
          if (Sector[WhichSector].SD[phase][i][j] == 0)
	    fprintf(fdOut, " X");
E 6
I 6
        for(j=1; j<16; j++)
        {
          if(Sector[WhichSector].SD[phase][i][j] == 0)
            fprintf(fdOut, " X");
E 6
          else
D 6
	    fprintf(fdOut, " 1");
E 6
I 6
            fprintf(fdOut, " 1");
E 6
        }
D 6
        if (Sector[WhichSector].SD[phase][i][j] == 0)
E 6
I 6
        if(Sector[WhichSector].SD[phase][i][j] == 0)
E 6
          fprintf(fdOut," X\n");
        else
          fprintf(fdOut," 1\n");
      }
D 6
      fprintf(fdOut,"/* 2nd 3 bits use 2nd 16 scintillators (Bit order is 17-32)\n");
      for (i=3; i<6; i++) {
E 6
I 6

      fprintf(fdOut,
        "/* 2nd 3 bits use 2nd 16 scintillators (Bit order is 17-32)\n"); /* */
      for(i=3; i<6; i++)
      {
E 6
        fprintf(fdOut,"SD %d:",i);
D 6
        for (j=17; j<32; j++) {
          if (Sector[WhichSector].SD[phase][i][j] == 0)
	    fprintf(fdOut, " X");
E 6
I 6
        for(j=17; j<32; j++)
        {
          if(Sector[WhichSector].SD[phase][i][j] == 0)
            fprintf(fdOut, " X");
E 6
          else
D 6
	    fprintf(fdOut, " 1");
E 6
I 6
            fprintf(fdOut, " 1");
E 6
        }
D 6
        if (Sector[WhichSector].SD[phase][i][j] == 0)
E 6
I 6
        if(Sector[WhichSector].SD[phase][i][j] == 0)
E 6
          fprintf(fdOut," X\n");
        else
          fprintf(fdOut," 1\n");
      } 
D 6
      fprintf(fdOut,"/* Next 4 are spare bits defined by user\n");
      for (i=0; i<4; i++) {
        if (i==2)
	{
	  if (phase == 0)
	    fprintf(fdOut,"EXTERNAL_SP_ENABLE %d\n",enable_spare1a[WhichSector]);
	  else
	    fprintf(fdOut,"EXTERNAL_SP_ENABLE %d\n",enable_spare1b[WhichSector]);
	}
	fprintf(fdOut,"SP %d:",i);
        for (j=1; j<16; j++) {
          if (Sector[WhichSector].SP[phase][i][j] == 0)
	    fprintf(fdOut, " X");
E 6
I 6

      fprintf(fdOut,"/* Next 4 are spare bits defined by user\n"); /* */
      for(i=0; i<4; i++)
      {
        if(i==2)
        {
          if(phase == 0)
            fprintf(fdOut,
                    "EXTERNAL_SP_ENABLE %d\n",enable_spare1a[WhichSector]);
E 6
          else
D 6
	    fprintf(fdOut, " 1");
E 6
I 6
            fprintf(fdOut,
                    "EXTERNAL_SP_ENABLE %d\n",enable_spare1b[WhichSector]);
E 6
        }
D 6
        if (Sector[WhichSector].SP[phase][i][j] == 0)
E 6
I 6

        fprintf(fdOut,"SP %d:",i);
        for(j=1; j<16; j++)
        {
          if(Sector[WhichSector].SP[phase][i][j] == 0)
            fprintf(fdOut, " X");
          else
            fprintf(fdOut, " 1");
        }
        if(Sector[WhichSector].SP[phase][i][j] == 0)
E 6
          fprintf(fdOut," X\n");
        else
          fprintf(fdOut," 1\n");
      }
D 6
      fprintf(fdOut,"/* Next 3 bits use first 16 extra data inputs (Bit order is 1-16)\n");
      for (i=0; i<3; i++) {
E 6
I 6

      fprintf(fdOut,
   "/* Next 3 bits use first 16 extra data inputs (Bit order is 1-16)\n");/* */
      for(i=0; i<3; i++)
      {
E 6
        fprintf(fdOut,"ED %d:",i);
D 6
        for (j=1; j<16; j++) {
          if (Sector[WhichSector].ED[phase][i][j] == 0)
	    fprintf(fdOut, " X");
E 6
I 6
        for(j=1; j<16; j++)
        {
          if(Sector[WhichSector].ED[phase][i][j] == 0)
            fprintf(fdOut, " X");
E 6
          else
D 6
	    fprintf(fdOut, " 1");
E 6
I 6
            fprintf(fdOut, " 1");
E 6
        }
D 6
        if (Sector[WhichSector].ED[phase][i][j] == 0)
E 6
I 6
        if(Sector[WhichSector].ED[phase][i][j] == 0)
E 6
          fprintf(fdOut," X\n");
        else
          fprintf(fdOut," 1\n");
      }
D 6
      fprintf(fdOut,"/* Last 3 bits use second 16 extra data inputs (Bit order is 17-32)\n");
      for (i=3; i<6; i++) {
E 6
I 6
      fprintf(fdOut,
"/* Last 3 bits use second 16 extra data inputs (Bit order is 17-32)\n"); /* */
      for(i=3; i<6; i++)
      {
E 6
        fprintf(fdOut,"ED %d:",i);
D 6
        for (j=17; j<32; j++) {
          if (Sector[WhichSector].ED[phase][i][j] == 0)
	    fprintf(fdOut, " X");
E 6
I 6
        for(j=17; j<32; j++)
        {
          if(Sector[WhichSector].ED[phase][i][j] == 0)
            fprintf(fdOut, " X");
E 6
          else
D 6
	    fprintf(fdOut, " 1");
E 6
I 6
            fprintf(fdOut, " 1");
E 6
        }
D 6
        if (Sector[WhichSector].ED[phase][i][j] == 0)
E 6
I 6
        if(Sector[WhichSector].ED[phase][i][j] == 0)
E 6
          fprintf(fdOut," X\n");
        else
          fprintf(fdOut," 1\n");
      }

D 6
      fprintf(fdOut,"\n/* Sector %i - Phase 2:\n",WhichSector);
      fprintf(fdOut,"/* Define up to 12 Sector Events in terms of phase 1 bits.\n");
      fprintf(fdOut,"/* SE0-5 for Triggers 1-4, SE6-11 for Triggers 5-8.\n");
      if (phase == 0)
E 6
I 6
      fprintf(fdOut,"\n/* Sector %i - Phase 2:\n",WhichSector); /* */
      fprintf(fdOut,
         "/* Define up to 12 Sector Events in terms of phase 1 bits.\n"); /* */
      fprintf(fdOut,
              "/* SE0-5 for Triggers 1-4, SE6-11 for Triggers 5-8.\n"); /* */
      if(phase == 0)
E 6
      {
D 6
	beginloop = 0;
	endloop = 6;
E 6
I 6
        beginloop = 0;
        endloop = 6;
E 6
      }
      else
      {
D 6
	beginloop = 6;
	endloop = 12;
E 6
I 6
        beginloop = 6;
        endloop = 12;
E 6
      }
D 6
      for (i=beginloop;i<endloop;i++)
E 6
I 6
      for(i=beginloop; i<endloop; i++)
E 6
      {
        unused_product = 1;
        product_num = 0;
D 6
        for (k=0;k<MaxProducts;k++)
E 6
I 6
        for(k=0; k<MaxProducts; k++)
E 6
        {
D 6
	  /* check each product bit to determine if product was used */
	  for (j=0;j<16;j++)
	    if (Sector[WhichSector].SE[i][k][j] != 2)
	    {
	      /* if product was used then print it */
	      /* if first used product of SE then print SE header info before data */
	      if (product_num == 0)
	      {
		/* JAM - 10/23/96 - provision for VERY long SE comments */
	        fprintf(fdOut,"\nSECTOR_EVENT %i\n",i);
		fprintf(fdOut,"Comment: ");
		if (Sector[WhichSector].comment[i]) 
		  fprintf (fdOut, "%s", Sector[WhichSector].comment[i]);
		fprintf(fdOut,"\n");
	      }  
E 6
I 6
          /* check each product bit to determine if product was used */
          for(j=0; j<16; j++)
          if(Sector[WhichSector].SE[i][k][j] != 2)
          {
            /* if product was used then print it */
            /* if first used product of SE then print SE header
            info before data */
            if(product_num == 0)
            {
              /* JAM - 10/23/96 - provision for VERY long SE comments */
              fprintf(fdOut,"\nSECTOR_EVENT %i\n",i);
              fprintf(fdOut,"Comment: ");
              if(Sector[WhichSector].comment[i]) 
                fprintf(fdOut,"%s",Sector[WhichSector].comment[i]);
              fprintf(fdOut,"\n");
            }
E 6

D 6
	      /* line up product numbers in print out */
	      if (product_num < 10)
	        fprintf(fdOut,"Product  %i:",product_num);
	      else
	        fprintf(fdOut,"Product %i:",product_num);
E 6
I 6
            /* line up product numbers in print out */
            if(product_num < 10)
              fprintf(fdOut,"Product  %i:",product_num);
            else
              fprintf(fdOut,"Product %i:",product_num);
E 6

D 6
	      for (j=0;j<16;j++)
	      {
	        if (Sector[WhichSector].SE[i][k][j] == 2)
	          fprintf(fdOut," X");
	        else
	          fprintf(fdOut," %i",Sector[WhichSector].SE[i][k][j]);
	      }
	      fprintf(fdOut,"\n");
	      product_num++;
	      break; /* break for loop checking product bits */
	    }
E 6
I 6
            for(j=0; j<16; j++)
            {
              if(Sector[WhichSector].SE[i][k][j] == 2)
                fprintf(fdOut," X");
              else
                fprintf(fdOut," %i",Sector[WhichSector].SE[i][k][j]);
            }
            fprintf(fdOut,"\n");
            product_num ++;
            break; /* break for loop checking product bits */
          }
E 6
        }
      }
      fprintf(fdOut,"\n");
    }

D 3
    if (phase == 0)
E 3
I 3
    if(phase == 0)
E 3
    {
D 6
      fprintf(fdOut,"/* Event Processor Triggers 1-4 Definitions:\n");
E 6
I 6
      fprintf(fdOut,"/* Event Processor Triggers 1-4 Definitions:\n"); /* */
E 6
      beginloop = 1;
      endloop = 5;
    }
    else
    {
D 6
      fprintf(fdOut,"/* Event Processor Triggers 5-8 Definitions:\n");
E 6
I 6
      fprintf(fdOut,"/* Event Processor Triggers 5-8 Definitions:\n"); /* */
E 6
      beginloop = 5;
      endloop = 9;
    }

I 6

printf("2=============================== %d\n",phase3.Trig[1][0][18]);


E 6
    fprintf(fdOut,"/* Define up to %d products for each Trigger */\n",MaxTrigProducts);
D 6
    for (i=beginloop;i<endloop;i++)
E 6
I 6
    for(i=beginloop; i<endloop; i++)
E 6
    {
      fprintf(fdOut,"\nTRIGGER %i\n",i);
      fprintf(fdOut,"Trigger_Pulse_Width: %i\n",phase3.Trig_Delay[i]);
      fprintf(fdOut,"Enable_Async: %i\n",phase3.Enable_Async[i]);
      unused_product = 1;
      product_num = 0;
D 6
      for (j=0;j<MaxTrigProducts;j++) 
E 6
I 6
      for(j=0; j<MaxTrigProducts; j++) 
E 6
      {
D 6
        /* check each product bit to determine if product was used */
        for (k=0;k<19;k++)
	  if (phase3.Trig[i][j][k] != 2)
  	  {
	    /* if product was used then print it */
  	    /* if first product used then print header info */
	    if (product_num == 0)
	    {
	      fprintf(fdOut,"/* Sector:  SCTR1  SCTR2  SCTR3  SCTR4  SCTR5  SCTR6  SYNC\n");
	      if ((i==1) || (i==2))
	        fprintf(fdOut,"/* SE Num:  0 1 2  0 1 2  0 1 2  0 1 2  0 1 2  0 1 2  EXT0\n");
	      if ((i==3) || (i==4))
	        fprintf(fdOut,"/* SE Num:  3 4 5  3 4 5  3 4 5  3 4 5  3 4 5  3 4 5  EXT1\n");
	      if ((i==5) || (i==6))
	        fprintf(fdOut,"/* SE Num:  6 7 8  6 7 8  6 7 8  6 7 8  6 7 8  6 7 8  EXT2\n");
	      if ((i==7) || (i==8))
	        fprintf(fdOut,"/* SE Num:  9 A B  9 A B  9 A B  9 A B  9 A B  9 A B  EXT3\n");
	    }
E 6
I 6
        /* check each product bit to determine if product was used: */
        /* check if we have at least one active SE in product; if no one SE
        is active or if EXT only active then we will print nothing */
        se_yes = 0;
        for(k=0; k<18; k++)
        {
          if(phase3.Trig[i][j][k] != 2)
          {
            se_yes = 1;
            break;
          }
        }

        if(se_yes == 1)
  	    {
  	      /* if first product used then print header info */
	      if(product_num == 0)
	      {
	        fprintf(fdOut,
		 "/* Sector:  SCTR1  SCTR2  SCTR3  SCTR4  SCTR5  SCTR6  SYNC\n"); /* */
	        if((i==1) || (i==2))
            {
	          fprintf(fdOut,
              "/* SE Num:  0 1 2  0 1 2  0 1 2  0 1 2  0 1 2  0 1 2  EXT0\n");
              /* */
            }
	        if((i==3) || (i==4))
            {
	          fprintf(fdOut,
              "/* SE Num:  3 4 5  3 4 5  3 4 5  3 4 5  3 4 5  3 4 5  EXT1\n");
              /* */
            }
	        if((i==5) || (i==6))
            {
	          fprintf(fdOut,
              "/* SE Num:  6 7 8  6 7 8  6 7 8  6 7 8  6 7 8  6 7 8  EXT2\n");
              /* */
            }
	        if((i==7) || (i==8))
            {
	          fprintf(fdOut,
              "/* SE Num:  9 A B  9 A B  9 A B  9 A B  9 A B  9 A B  EXT3\n");
              /* */
            }
	      }
E 6
	    
D 6
	    /* line up product numbers in print out */
	    if (product_num < 10)
	      fprintf(fdOut,"Product  %i:",product_num);
	    else
	      fprintf(fdOut,"Product %i:",product_num);
E 6
I 6
	      /* line up product numbers in print out */
	      if(product_num < 10)
	        fprintf(fdOut,"Product  %i:",product_num);
	      else
	        fprintf(fdOut,"Product %i:",product_num);
E 6

D 6
	    /* print out data bits aligned with header info */
	    counter = 1;
	    for (k=0;k<18;k++)
	    {
	      /* Add extra space to line up with header if new sector group (every 3 data bits) */
	      if (counter==4)
E 6
I 6
	      /* print out data bits aligned with header info */
	      counter = 1;
	      for(k=0; k<18; k++)
E 6
	      {
D 6
	        fprintf(fdOut," ");
	        counter = 1;
E 6
I 6
	        /* Add extra space to line up with header if new sector group (every 3 data bits) */
	        if(counter==4)
	        {
	          fprintf(fdOut," ");
	          counter = 1;
	        }

	        if (phase3.Trig[i][j][k] == 2)
	          fprintf(fdOut," X");
	        else
	          fprintf(fdOut," %i",phase3.Trig[i][j][k]);

	        counter ++;
E 6
	      }

D 6
	      if (phase3.Trig[i][j][k] == 2)
	        fprintf(fdOut," X");
E 6
I 6
	      /* External Sync Bit last in array, lined up with header */
	      if(phase3.Trig[i][j][18] == 2)
	        fprintf(fdOut,"   X\n");
E 6
	      else
D 6
	        fprintf(fdOut," %i",phase3.Trig[i][j][k]);
E 6
I 6
	        fprintf(fdOut,"   %i\n",phase3.Trig[i][j][18]);
E 6

D 6
	      counter++;
E 6
I 6
	      product_num++;
E 6
	    }
D 6
	    /* External Sync Bit last in array, lined up with header */
	    if (phase3.Trig[i][j][18] == 2)
	      fprintf(fdOut,"   X\n");
	    else
	      fprintf(fdOut,"   %i\n",phase3.Trig[i][j][18]);
E 6

D 6
	    product_num++;
	    break;
	  }
E 6
      }
    }
D 6
  fprintf(fdOut,"\n");
E 6
I 6
    fprintf(fdOut,"\n");
E 6
  }
  fclose(fdOut);
  return(0);
}

/* ---------------------- Function FillSDArray ------------------------- */
/* 
 *  This function reads in data from file and fills SD array correctly.
 */
D 6
VOID FillSDArray(fd, sectornum, phase, i, jstart, jend)
int sectornum, phase, i, jstart, jend;
FILE* fd;
E 6
I 6

static void
FillSDArray(FILE *fd, int sectornum, int phase, int i, int jstart, int jend)
E 6
{
  int j;
  char s[2];
/*
  printf ("sectnum[%d] phase[%d] i[%d] jstart[%d] jend[%d]\n", 
	  sectornum, phase, i, jstart, jend);
*/
  for (j=jstart; j<=jend; j++)
    {
      s[0] = getc(fd);
      s[1] = getc(fd);
      if (s[1] == 'X')
	Sector[sectornum].SD[phase][i][j] = 0;
      else if (s[1] == '1')
	Sector[sectornum].SD[phase][i][j] = 1;
    }
}

/* ---------------------- Function FillSPArray ------------------------- */
/* 
 *  This function reads in data from file and fills SP array correctly.
 */
D 6
VOID FillSPArray(fd, sectornum, phase, i, jstart, jend)
int sectornum, phase, i, jstart, jend;
FILE* fd;
E 6
I 6

static void
FillSPArray(FILE *fd, int sectornum, int phase, int i, int jstart, int jend)
E 6
{
  int j;
  char s[2];
  for (j=jstart; j<=jend; j++)
    {
      s[0] = getc(fd);
      s[1] = getc(fd);
      if (s[1] == 'X')
	Sector[sectornum].SP[phase][i][j] = 0;
      else if (s[1] == '1')
	Sector[sectornum].SP[phase][i][j] = 1;
    }
}

/* ---------------------- Function FillEDArray ------------------------- */
/* 
 *  This function reads in data from file and fills ED array correctly.
 */
D 6
VOID FillEDArray(fd, sectornum, phase, i, jstart, jend)
int sectornum, phase, i, jstart, jend;
FILE* fd;
E 6
I 6
static void
FillEDArray(FILE *fd, int sectornum, int phase, int i, int jstart, int jend)
E 6
{
  int j;
  char s[2];
  for (j=jstart; j<=jend; j++)
    {
      s[0] = getc(fd);
      s[1] = getc(fd);
      if (s[1] == 'X')
	Sector[sectornum].ED[phase][i][j] = 0;
      else if (s[1] == '1')
	Sector[sectornum].ED[phase][i][j] = 1;
    }
}

/* ---------------------- Function FillSEArray ------------------------- */
/* 
 *  This function reads in data from file and fills SE array correctly.
 */
D 6
VOID FillSEArray(fd, sectornum, se_num, prod_num)
int sectornum,se_num,prod_num;
FILE *fd;
E 6
I 6

static void
FillSEArray(FILE *fd, int sectornum, int se_num, int prod_num)
E 6
{
  int j;
  char s[2];
  for (j=0; j<16; j++)
    {
      s[0] = getc(fd);
      s[1] = getc(fd);
      if (s[1] == 'X')
	Sector[sectornum].SE[se_num][prod_num][j] = 2;
      else if (s[1] == '0')
	Sector[sectornum].SE[se_num][prod_num][j] = 0;
      else if (s[1] == '1')
	Sector[sectornum].SE[se_num][prod_num][j] = 1;
    }
}

/* ---------------------- Function FillEPArray ------------------------- */
/* 
 * This function reads in data from file and fills the event processor
 * array correctly. 
 */

D 6
VOID FillEPArray(fd, trigger_num, prod_num)
int trigger_num, prod_num;
FILE *fd;
E 6
I 6
static void
FillEPArray(FILE *fd, int trigger_num, int prod_num)
E 6
{
D 6
  int i,j,offset;
E 6
I 6
  int i, j, offset;
E 6
  char s[2];

  offset = 0;
D 6
  for (i=0;i<6;i++)
  {  
    for (j=0;j<3;j++)
E 6
I 6
  for(i=0; i<6; i++)
  {
    for(j=0; j<3; j++)
E 6
    {
      s[0] = getc(fd);
      s[1] = getc(fd);
D 6
      if (s[1] == 'X')
	phase3.Trig[trigger_num][prod_num][j+offset] = 2;
      else if (s[1] == '0')
	phase3.Trig[trigger_num][prod_num][j+offset] = 0;
      else if (s[1] == '1')
	phase3.Trig[trigger_num][prod_num][j+offset] = 1;
E 6
I 6
      if(s[1] == 'X')
        phase3.Trig[trigger_num][prod_num][j+offset] = 2;
      else if(s[1] == '0')
        phase3.Trig[trigger_num][prod_num][j+offset] = 0;
      else if(s[1] == '1')
        phase3.Trig[trigger_num][prod_num][j+offset] = 1;
E 6
    }
    offset = offset+3;
    s[0] = getc(fd);  /* read extra blank dividing SE columns */
  }
  /* read in EXT SYNC bit last */
  s[0] = getc(fd);  /* read in two remaining blanks before EXT bit */
  s[0] = getc(fd);
  s[1] = getc(fd);
D 6
  if (s[1] == 'X')
    phase3.Trig[trigger_num][prod_num][18] = 2;
  if (s[1] == '0')
    phase3.Trig[trigger_num][prod_num][18] = 0;
  if (s[1] == '1')
    phase3.Trig[trigger_num][prod_num][18] = 2;
E 6
I 6
  if(s[1] == 'X')      phase3.Trig[trigger_num][prod_num][18] = 2;
  else if(s[1] == '0') phase3.Trig[trigger_num][prod_num][18] = 0;
  else if(s[1] == '1') phase3.Trig[trigger_num][prod_num][18] = 1; /* Sergey: was 2,
                         must be 1, but program during 'Save' trying to interprete '1'
                         in SYNC/EXT' column as sector 7 - BUG */
E 6
}

I 3
D 9
#ifndef OLDLOAD
E 9
I 9

E 9
void
bigdata_cleanup()
{
  int i;

  for(i=0; i<BIGDATA/2; i++) bigdata[i] = 0;

  return;
}

D 6
void
E 6
I 6
static void
E 6
bigdata_write(unsigned short *data, char *filename)
{
  FILE *fd;
  int nn;

D 4
  printf("bigdata_write reached\n");
E 4
I 4
  printf("bigdata_write reached, writing file >%s<\n",filename);
E 4
  if((fd = fopen(filename, "w")) != NULL)
  {
    nn = fwrite(data, 1, BIGDATA, fd);
    printf("bigdata_write: nbytes=%d\n",nn);
D 7
    close(fd);
E 7
I 7
    fclose(fd);
E 7
  }
  else
  {
    printf("bigdata_write: ERROR opening file >%s< - exit.\n",filename);
    exit(0);
  }
  printf("bigdata_write done\n");

  return;
}
D 9
#endif
E 9

I 9

E 9
E 3
/* ---------------------- Procedure download_phase1 -----------------------*/
/*
 * Procedure which fills a phase1 memory array for the sector given by the
 * input parameter and downloads the data to the correct router board.
I 3
 *   1 <= sector_num <= 6
 *   0 <= phase <= 1
E 3
 */
D 3
void download_phase1(sector_num, phase)
int sector_num, phase;
E 3
I 3

void
download_phase1(int sector_num, int phase)
E 3
{
  int address_array[16], SD_num, ED_num, SP_num, i, j;
  unsigned short router_phase1_data[65536], check_address;
  FILE *outfp;
  char buf[1024], it='A', fb[80];
D 3
  unsigned jamaddr, down_addr;
E 3
I 3
D 11
  unsigned jamaddr, down_addr, big_addr;
E 11
I 11
  /*unsigned jamaddr, down_addr;*/
  unsigned int big_addr;
E 11
E 3

  /* initialize data */
D 3
  for(i=0;i<65536;i++)
    router_phase1_data[i] = 0x0000;
E 3
I 3
  for(i=0; i<65536; i++) router_phase1_data[i] = 0x0000;
E 3

  /* for each SD and ED fill appropriate data locations */
D 3
  for (SD_num=0;SD_num<16;SD_num++)
E 3
I 3
  for(SD_num=0; SD_num<16; SD_num++)
E 3
  {
    /* Copy SD, SP, or ED data to address_array */
D 3
    if (SD_num<3)
      {
	for (i=0;i<16;i++)
	  address_array[i] = Sector[sector_num].SD[phase][SD_num][i+1];
      }
    else if ((SD_num>=3) && (SD_num<6))
      {
	for (i=0;i<16;i++)
	  address_array[i] = Sector[sector_num].SD[phase][SD_num][i+17];
      }
    else if ((SD_num==6) || (SD_num==7))
      {
	SP_num = SD_num - 6;
	for (i=0;i<16;i++)
	  address_array[i] = Sector[sector_num].SP[phase][SP_num][i+1];
      }
E 3
I 3
    if(SD_num<3)
    {
	  for(i=0; i<16; i++)
	    address_array[i] = Sector[sector_num].SD[phase][SD_num][i+1];
    }
    else if((SD_num>=3) && (SD_num<6))
    {
      for(i=0; i<16; i++)
        address_array[i] = Sector[sector_num].SD[phase][SD_num][i+17];
    }
    else if((SD_num==6) || (SD_num==7))
    {
      SP_num = SD_num - 6;
      for(i=0; i<16; i++)
        address_array[i] = Sector[sector_num].SP[phase][SP_num][i+1];
    }
E 3
    else if ((SD_num==8) || (SD_num==9))
      {
	SP_num = SD_num - 6;
	  for (i=0;i<16;i++)
	    address_array[i] = Sector[sector_num].SP[phase][SP_num][i+1];
      }
    else if ((SD_num>= 10) && (SD_num <= 12))
      {
	ED_num = SD_num - 10;
	for(i=0;i<16;i++)
	  address_array[i] = Sector[sector_num].ED[phase][ED_num][i+1];
      }
    else if ((SD_num>= 13) && (SD_num <= 15))
      {
	ED_num = SD_num - 10;
	for(i=0;i<16;i++)
	  address_array[i] = Sector[sector_num].ED[phase][ED_num][i+17];
      }

    /* Create check_address where X's are 0's */
    check_address = 0x0000;
D 3
    for (i=0;i<16;i++)
E 3
I 3
    for(i=0; i<16; i++)
E 3
    {
D 3
      if (address_array[i] == 1)
	check_address = check_address | (1<<i);
E 3
I 3
      if(address_array[i] == 1)
        check_address = check_address | (1<<i);
E 3
    }

    /* call procedure to load valid addresse with 1's */
    compare_phase1_addresses(router_phase1_data, check_address, SD_num);
  }


  printf("\nrouter_phase1_data = ");
D 3
  for(i=0;i<128;i++) {
E 3
I 3
  for(i=0; i<128; i++)
  {
E 3
    if ((i % 16) == 0) printf ("\n");
    printf(" %x ",router_phase1_data[i]);
  }



D 3
  if (phase == 0) {
E 3
I 3
  if(phase == 0)
  {
I 11
	/*
E 11
E 3
    sprintf (fb, "j.phase1a%d", sector_num);
D 3
    if ((outfp = fopen (fb, "w")) == 0)
E 3
I 3
    if((outfp = fopen(fb, "w")) == 0)
E 3
      printf ("file: %s could not open!\n", fb);
    it = 'A';
D 3
    jamaddr = (0xfa000000 + ((sector_num - 1) * 0x80000));
    down_addr = (0xfa000000 + ((sector_num - 1) * 0x80000)) ;
  } else {
E 3
I 3
D 11
    jamaddr =   (0xfa000000 + ((sector_num - 1) * 0x80000));
    down_addr = (0xfa000000 + ((sector_num - 1) * 0x80000));
E 11
I 11
    jamaddr =   (LEVEL1BASE+0x000000 + ((sector_num - 1) * 0x80000));
    down_addr = (LEVEL1BASE+0x000000 + ((sector_num - 1) * 0x80000));
	*/
    big_addr =             (0x000000 + ((sector_num - 1) * 0x80000))/2; /* index for 'short' array */
E 11
  }
  else
  {
I 11
	/*
E 11
E 3
    sprintf (fb, "j.phase1b%d", sector_num);
D 3
    if ((outfp = fopen (fb, "w")) == 0)
E 3
I 3
    if((outfp = fopen(fb, "w")) == 0)
E 3
      printf ("file: %s could not open!\n", fb);
    it = 'B';
D 3
    jamaddr = (0xfa020000 + ((sector_num - 1) * 0x80000));
    down_addr = (0xfa020000 + ((sector_num - 1) * 0x80000)) ;
E 3
I 3
D 11
    jamaddr =   (0xfa020000 + ((sector_num - 1) * 0x80000));
    down_addr = (0xfa020000 + ((sector_num - 1) * 0x80000));
E 11
I 11
    jamaddr =   (LEVEL1BASE+0x020000 + ((sector_num - 1) * 0x80000));
    down_addr = (LEVEL1BASE+0x020000 + ((sector_num - 1) * 0x80000));
	*/
    big_addr = (            0x020000 + ((sector_num - 1) * 0x80000))/2; /* index for 'short' array */
E 11
E 3
  }
I 3
D 11
  big_addr = (down_addr & 0xFFFFFF)/2; /* index for 'short' array */
E 11
E 3
  printf("\nrouter_phase1_data");
D 11
  
E 11
I 11
  /*
E 11
  sprintf (buf, "ROUTER %d PHASE 1%c BASE %u\n", sector_num, it, jamaddr ) ;
D 11
  /*  j = fwrite (buf, 1, strlen (buf), outfp) ;
  j = fwrite (router_phase1_data, sizeof (*router_phase1_data), 0x20000, outfp) ;  */
  printf ("return code = %d\n", j);
E 11
  fclose (outfp);
D 11
  
  
  printf("down loading phase 1/2 : %s \n", dlhn);
E 11
I 11
  */
E 11

D 3
  switch (sector_num)
    {
E 3
I 3
D 11
  /* vme_download(down_addr, router_phase1_data, dlhn); */

E 11
D 9
#ifndef OLDLOAD
E 9
  for(i=0; i<65536; i++) bigdata[big_addr+i] = router_phase1_data[i];
D 9
#else
  switch(sector_num)
  {
E 9

D 9
    case 1: vme_download(down_addr, router_phase1_data, dlhn);
            break;
E 3
      
D 3
    case 1:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
E 3
I 3
    case 2: vme_download(down_addr, router_phase1_data, dlhn);
            break;
E 3
      
D 3
    case 2:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
      
    case 3:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
E 3
I 3
    case 3: vme_download(down_addr, router_phase1_data, dlhn);
            break;
E 3

D 3
    case 4:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
E 3
I 3
    case 4: vme_download(down_addr, router_phase1_data, dlhn);
            break;
E 3
      
D 3
    case 5:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
E 3
I 3
    case 5: vme_download(down_addr, router_phase1_data, dlhn);
            break;
E 3
      
D 3
    case 6:  vme_download(down_addr, router_phase1_data,dlhn);
      break;
    }
/*  router_init (sector_num, dlhn);     */
E 3
I 3
    case 6: vme_download(down_addr, router_phase1_data, dlhn);
            break;
  }
#endif
E 3

E 9
I 3
  return;
E 3
}



/*******************************************************************************
  This procedure takes an address with 1's and X's and finds and sets to 1 all
  locations which correspond to any 1's in address.  The memroy array is loaded
  with 1's in the bit position specified if valid, or left with 0 if not.
*******************************************************************************/
D 3
void compare_phase1_addresses( memory_data, check_address, which_bit)
unsigned short int memory_data[65536], check_address;
int which_bit;
E 3
I 3

D 6
void
E 6
I 6
static void
E 6
compare_phase1_addresses(unsigned short int memory_data[65536],
                         unsigned short int check_address, int which_bit)
E 3
{
  int loop_address;
  unsigned short int address, and_address;

  address=0;
  /* loop over all 64K addresses */
D 3
  for (loop_address=0;loop_address<65536;loop_address++)
E 3
I 3
  for(loop_address=0; loop_address<65536; loop_address++)
E 3
  {
    and_address = address & check_address;
    /* if and_address has any 1 values then is valid address */
D 3
    if (and_address > 0)
E 3
I 3
    if(and_address > 0)
    {
E 3
      memory_data[address] = memory_data[address] | (1<<which_bit);
I 3
    }
E 3
    address++;
  }
}


/* ---------------------- Procedure download_phase2 ------------------------*/
/*
 * Procedure which fills a phase2 memory array for the sector given by the
 * input parameter and downloads data to correct router board.
I 3
 *   1 <= sector_num <= 6
E 3
 */
D 3
void download_phase2(sector_num)
int sector_num;
E 3
I 3

void
download_phase2(int sector_num)
E 3
{
  int address_array[16], max_X, SE_num, product, i, z;
  unsigned short int router_phase2_data[65536];
  unsigned short check_address, X_mask, j;
D 3
  unsigned int jamaddr;
E 3
I 3
D 11
  unsigned int jamaddr, big_addr;
E 11
I 11
  /*unsigned int jamaddr;*/
  unsigned int big_addr;
E 11
E 3
  char buf[1024], fb[80];
  FILE *outfp;

  int correct_SE_num;	/* mod by dcd 6/21/99 to fix upper 4 bits*/

  /* initialize the data */
D 3
  for(i=0;i<65536;i++)
    router_phase2_data[i] = 0x0000;
E 3
I 3
  for(i=0; i<65536; i++) router_phase2_data[i] = 0x0000;
E 3
  printf ("------------- download phase2  ------------ *\n");
I 3

E 3
  /* for each SE loop over all products and load locations in memory */
D 3
  for (SE_num=0;SE_num<MaxSE;SE_num++)
E 3
I 3
  for(SE_num=0; SE_num<MaxSE; SE_num++)
E 3
  {
D 3
    for (product=0; product<MaxProducts; product++)
E 3
I 3
    for(product=0; product<MaxProducts; product++)
E 3
    {
      check_address = 0x0000;
      X_mask = 0x0000;
      max_X = 16;
D 3
      for(i=0;i<16;i++)
E 3
I 3
      for(i=0; i<16; i++)
E 3
      {
        /* Copy current product of current SE into address_array */
        address_array[i] = Sector[sector_num].SE[SE_num][product][i]; 

D 3
	/* Find number of X's in address and create X_mask and address where X's are 0's */
	if (address_array[i] != 2)
	{
	  X_mask = X_mask | (1<<i);
	  max_X--;
	}
E 3
I 3
        /* Find number of X's in address and create X_mask
           and address where X's are 0's */
        if(address_array[i] != 2)
        {
          X_mask = X_mask | (1<<i);
          max_X--;
        }
E 3

D 3
	if (address_array[i] == 1)
	  check_address = check_address | (1<<i);
E 3
I 3
        if(address_array[i] == 1) check_address = check_address | (1<<i);
E 3
      }

      /* call fill_memory_array to load data appropriately */
D 3
      if (max_X < 16) {

/* Mod 6/21/99 DCD
   If we are on sector events 6-11, we need to adjust the bit in memory
   cause these actually come out of bit 8-13
   So do the conversion - then pass in correct value to compare_addresses
*/
	if(SE_num >5)
	{
	   correct_SE_num = SE_num + 2;
	}
	else
	{
	   correct_SE_num = SE_num;
	}

	compare_addresses(router_phase2_data, check_address, X_mask, correct_SE_num, max_X);

E 3
I 3
      if(max_X < 16)
      {
        /* Mod 6/21/99 DCD
         If we are on sector events 6-11, we need to adjust the bit in memory
         cause these actually come out of bit 8-13
         So do the conversion - then pass in correct value to compare_addresses
        */
        if(SE_num >5)
        {
          correct_SE_num = SE_num + 2;
        }
        else
        {
          correct_SE_num = SE_num;
        }
        compare_addresses(router_phase2_data, check_address,
                          X_mask, correct_SE_num, max_X);
E 3
      }
    }
  }

  printf("\nrouter_phase2_data = ");
D 3
  for(i=0;i<256;i++) {
    if ((i%16)==0) printf ("\n");
E 3
I 3
  for(i=0; i<256; i++)
  {
    if((i%16)==0) printf("\n");
E 3
    printf(" %x ",router_phase2_data[i]);
  }
  printf("\n");
D 11

E 11
I 11
  /*
E 11
D 3

  sprintf (fb, "j.phase2%d", sector_num);
  if ((outfp = fopen (fb, "w")) == 0) {
E 3
I 3
  sprintf(fb, "j.phase2%d", sector_num);
  if((outfp = fopen (fb, "w")) == 0)
  {
E 3
    printf ("file: %s could not open!\n", fb);
D 3
    exit (00);
E 3
I 3
    exit(0);
E 3
  }
D 11

D 3

  jamaddr = 0xfa040000 + (( sector_num-1) * 0x80000) ; 
E 3
I 3
  jamaddr = 0xfa040000 + (( sector_num-1) * 0x80000);
  big_addr = (jamaddr & 0xFFFFFF)/2; /* index in 'short' array */
E 11
I 11
  jamaddr = LEVEL1BASE+0x040000 + (( sector_num-1) * 0x80000);
  */
  big_addr = (         0x040000 + (( sector_num-1) * 0x80000))/2; /* index in 'short' array */
E 11
E 3
  printf ("sector num = %d\n", (sector_num) );
  printf("\nrouter_phase2_data file output");
D 11

D 3
  sprintf (buf, "ROUTER %d PHASE 2 BASE %u\n", sector_num, jamaddr ) ;
E 3
I 3
  sprintf(buf, "ROUTER %d PHASE 2 BASE %u\n", sector_num, jamaddr);
E 3
  /*  j = fwrite (buf, 1, strlen (buf), outfp) ;
  j = fwrite (router_phase2_data, sizeof (*router_phase2_data), 0x20000, outfp) ; */
  
E 11
I 11
  /*
  sprintf(buf, "ROUTER %d PHASE 2 BASE %u\n", sector_num, jamaddr);  
E 11
  sprintf (buf, "ROUTER %d CREG 0 BASE %u\n", sector_num, (jamaddr+0x20000));
  z = fwrite (buf, 1, strlen(buf), outfp);
  printf ("\nreturn code = %d: %d\n", j, z);
  fclose (outfp);
D 11

E 11
I 11
  */
E 11
I 3
D 9
#ifndef OLDLOAD
E 9
  for(i=0; i<65536; i++) bigdata[big_addr+i] = router_phase2_data[i];
D 9
#else
  switch(sector_num)
  {
    case 1: vme_download(0xfa040000, router_phase2_data, dlhn);
            break;
E 9
E 3

D 3
  switch (sector_num)
    {
    case 1:  vme_download(0xfa040000, router_phase2_data,dlhn);
             break;
E 3
I 3
D 9
    case 2: vme_download(0xfa0C0000, router_phase2_data, dlhn);
            break;
E 3

D 3
    case 2:  vme_download(0xfa0C0000, router_phase2_data,dlhn);
             break;
E 3
I 3
    case 3: vme_download(0xfa140000, router_phase2_data, dlhn);
            break;
E 3

D 3
    case 3:  vme_download(0xfa140000, router_phase2_data,dlhn);
             break;
E 3
I 3
    case 4: vme_download(0xfa1C0000, router_phase2_data, dlhn);
            break;
E 3

D 3
    case 4:  vme_download(0xfa1C0000, router_phase2_data,dlhn);
             break;
E 3
I 3
    case 5: vme_download(0xfa240000, router_phase2_data, dlhn);
            break;
E 3

D 3
    case 5:  vme_download(0xfa240000, router_phase2_data,dlhn);
             break;
E 3
I 3
    case 6: vme_download(0xfa2C0000, router_phase2_data, dlhn);
            break;
  }
  router_init(sector_num, dlhn);
#endif
E 3

E 9
D 3
    case 6:  vme_download(0xfa2C0000, router_phase2_data,dlhn);
             break;
    }
  router_init (sector_num, dlhn);

E 3
I 3
  return;
E 3
}

/* ---------------------- Procedure download_phase3 ------------------------*/
/*
 * Procedure which fills a php memory array for the sector given by the
 * input parameter and downloads the data to event processor board.
 */
D 3
void download_phase3()
E 3
I 3

void
download_phase3()
E 3
{
  int max_X, Trigger_num, bit_shift, i;
  int test_value1, test_value2, trig_shift, product;
  unsigned short int check_address, X_mask;
  unsigned short int php_data[65536], creg1, creg2;
D 3
  unsigned int jamaddr, j;
E 3
I 3
D 11
  unsigned int jamaddr, big_addr, j;
E 11
I 11
  /*unsigned int jamaddr;*/
  unsigned int big_addr, j;
E 11
E 3
  char buf[1024];
I 4
  char *parm, fname1[1000], fname2[1000];
E 4
  FILE *outfp;

  /* initialize the data */
D 3
  for(i=0;i<65536;i++)
    php_data[i] = 0x0000;
E 3
I 3
  for(i=0; i<65536; i++) php_data[i] = 0x0000;
E 3
  creg1 = 0x0000;
  creg2 = 0x0000;

  /* for each Trigger loop over all products and load locations in memory */
D 3
  for (Trigger_num=1;Trigger_num<MaxTrig;Trigger_num++)
E 3
I 3
  for(Trigger_num=1; Trigger_num<MaxTrig; Trigger_num++)
E 3
  {
D 3
    for (product=0; product<MaxTrigProducts; product++)
E 3
I 3
    for(product=0; product<MaxTrigProducts; product++)
E 3
    {
      check_address = 0x0000;
      X_mask = 0x0000;
      max_X = 16;

      /* determine two memory pairs should be loaded, 1, 2, or both */
D 3
      if ((phase3.Trig[Trigger_num][product][2]==1) || (phase3.Trig[Trigger_num][product][5]==1) || (phase3.Trig[Trigger_num][product][8]==1))
	test_value1 = 1;
      else test_value1 = 0;
      if (( phase3.Trig[Trigger_num][product][11]==1) || (phase3.Trig[Trigger_num][product][14]==1) || (phase3.Trig[Trigger_num][product][17]==1))
	test_value2 = 1;
      else test_value2 = 0;
E 3
I 3
      if((phase3.Trig[Trigger_num][product][2]==1) ||
         (phase3.Trig[Trigger_num][product][5]==1) ||
         (phase3.Trig[Trigger_num][product][8]==1))
        test_value1 = 1;
      else
        test_value1 = 0;
      if((phase3.Trig[Trigger_num][product][11]==1) ||
         (phase3.Trig[Trigger_num][product][14]==1) ||
         (phase3.Trig[Trigger_num][product][17]==1))
        test_value2 = 1;
      else
        test_value2 = 0;
E 3

      /* each trigger will have to have data bits shifted correctly */
      trig_shift = 0;
D 3
      if ((Trigger_num == 3) || (Trigger_num == 4))
	  trig_shift = 4;
      else if ((Trigger_num == 5) || (Trigger_num == 6))
	  trig_shift = 8;
      else if ((Trigger_num == 7) || (Trigger_num == 8))
	  trig_shift = 12;
E 3
I 3
      if((Trigger_num == 3) || (Trigger_num == 4))
	    trig_shift = 4;
      else if((Trigger_num == 5) || (Trigger_num == 6))
	    trig_shift = 8;
      else if((Trigger_num == 7) || (Trigger_num == 8))
	    trig_shift = 12;
E 3

      /* determine proper memory data bit and create correct address */
      if ((Trigger_num%2) != 0)
      {
D 3
	max_X = 0;
	if (test_value1)
	{
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 0);
	  if (max_X < 16)
	  {
	    bit_shift = 0 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	}
	else if (test_value2)
	{
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 1);
	  if (max_X < 16)
	  {
	    bit_shift = 2 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	}
	else
	{
	  /* load both bits of a pair */
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 0);
	  if (max_X < 16)
	  {
	    bit_shift = 0 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 1);
	  if (max_X < 16)
	  {
	    bit_shift = 2 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
E 3
I 3
        max_X = 0;
        if(test_value1)
        {
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 0);
          if(max_X < 16)
          {
            bit_shift = 0 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
E 3
        }
I 3
        else if(test_value2)
        {
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 1);
          if(max_X < 16)
          {
            bit_shift = 2 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
        }
        else
        {
          /* load both bits of a pair */
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 0);
          if(max_X < 16)
          {
            bit_shift = 0 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 1);
          if(max_X < 16)
          {
            bit_shift = 2 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
        }
E 3
      }
      else
      {
D 3
	if (test_value1)
	{
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 0);
	  if (max_X < 16)
	  {
	    bit_shift = 1 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	}
	else if (test_value2)
	{
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 1);
	  if (max_X < 16)
	  {
	    bit_shift = 3 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	}
	else
	{
	  /* load both bits of a pair */
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 0);
	  if (max_X < 16)
	  {
	    bit_shift = 1 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
	  make_php_address(&check_address, &X_mask, &max_X, Trigger_num, product, 1);
	  if (max_X < 16)
	  {
	    bit_shift = 3 + trig_shift;
	    compare_addresses(php_data, check_address, X_mask, bit_shift, max_X);
	  }
E 3
I 3
        if(test_value1)
        {
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 0);
          if(max_X < 16)
          {
            bit_shift = 1 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
E 3
        }
I 3
        else if(test_value2)
        {
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 1);
          if(max_X < 16)
          {
            bit_shift = 3 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
        }
        else
        {
          /* load both bits of a pair */
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 0);
          if(max_X < 16)
          {
            bit_shift = 1 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
          make_php_address(&check_address, &X_mask, &max_X,
                           Trigger_num, product, 1);
          if(max_X < 16)
          {
            bit_shift = 3 + trig_shift;
            compare_addresses(php_data,check_address,X_mask,bit_shift,max_X);
          }
        }
E 3
      }
    }
    /* create creg2 while looping over Trigger_num values */

D 3
fprintf(stderr, "*** <  B4 > TrigNum: %d  phase3: %d  creg2: %d\n",
	Trigger_num, phase3.Enable_Async[Trigger_num], creg2);
E 3
I 3
    fprintf(stderr, "*** <  B4 > TrigNum: %d  phase3: %d  creg2: %d\n",
	  Trigger_num, phase3.Enable_Async[Trigger_num], creg2);
E 3

    creg2 = creg2 | (phase3.Enable_Async[Trigger_num] << (Trigger_num-1));

D 3
fprintf(stderr, "*** <After> TrigNum: %d  phase3: %d  creg2: %d\n",
	Trigger_num, phase3.Enable_Async[Trigger_num], creg2);
E 3
I 3
    fprintf(stderr, "*** <After> TrigNum: %d  phase3: %d  creg2: %d\n",
	  Trigger_num, phase3.Enable_Async[Trigger_num], creg2);
E 3

D 3

E 3
  }		

  printf("\nphp_data0-20 = \n");
D 3
  for(i=0;i<8000;i++) {
    if ((i % 16) == 0)
      printf ("\n");
E 3
I 3
  for(i=0; i<8000; i++)
  {
    if((i % 16) == 0) printf ("\n");
E 3
    printf(" %x ",php_data[i]);
  }
  printf("\n");

D 11

E 11
I 11
  /*
E 11
D 3

E 3
  if ((outfp = fopen ("j.phase3", "w")) == 0) 
  printf ("file: j.phase3 could not open!\n");
D 11


D 3
  jamaddr = 0xfa300000; 
E 3
I 3
  jamaddr = 0xfa300000;
  big_addr = (jamaddr & 0xFFFFFF)/2; /* index in 'short' array */
E 11
I 11
  jamaddr = LEVEL1BASE+0x300000;
  */
  big_addr = (         0x300000)/2; /* index in 'short' array */
E 11
E 3
  printf("\nrouter_phase3_data");
D 11

E 11
I 11
  /*
E 11
D 3
  sprintf (buf, "EVT_PROC BASE %u\n", jamaddr ) ;
  /*  j = fwrite (buf, 1, strlen (buf), outfp) ;
  j = fwrite (php_data, sizeof (*php_data), 0x20000, outfp) ;   
E 3
I 3
  sprintf (buf, "EVT_PROC BASE %u\n", jamaddr );
D 11
  /*  j = fwrite (buf, 1, strlen (buf), outfp);
  j = fwrite (php_data, sizeof (*php_data), 0x20000, outfp);   
E 3
  */
E 11
D 3
  sprintf (buf, "EVT_PROC CREG1 0 BASE %u\n", (jamaddr+0x20000) ) ;
E 3
I 3
  sprintf (buf, "EVT_PROC CREG1 0 BASE %u\n", (jamaddr+0x20000) );
E 3
  i = fwrite (buf, 1, strlen (buf), outfp) ;  

D 3
  sprintf (buf, "EVT_PROC CREG2 0 BASE %u\n", (jamaddr+0x40000) ) ;
E 3
I 3
  sprintf (buf, "EVT_PROC CREG2 0 BASE %u\n", (jamaddr+0x40000) );
E 3
  i = fwrite (buf, 1, strlen (buf), outfp) ;
  printf ("return code = %d: %d\n", j, i);
D 3
  fclose (outfp);
E 3
I 3
  fclose(outfp);
E 3
D 11

E 11
I 11
  */
E 11
I 3
D 9
#ifndef OLDLOAD
E 9
  for(i=0; i<65536; i++) bigdata[big_addr+i] = php_data[i];
D 9
#else
E 3
  vme_download(0xfa300000, php_data, dlhn); 
I 3
#endif
E 9
E 3

  /* create creg1 values from pulse stretch value */
  creg1 = 0x0000;
D 3
  for (i=0;i<MaxTrig-1;i++)
E 3
I 3
  for(i=0; i<MaxTrig-1; i++)
E 3
  {
    switch (phase3.Trig_Delay[i+1])
    {
D 3
    case 1: break;
    case 2: creg1 = creg1 | (1 << (2*i));
            break;
    case 4: creg1 = creg1 | (1 << (2*i+1));
            break;
    case 8: creg1 = creg1 | (1 << (2*i));
            creg1 = creg1 | (1 << (2*i+1));
            break;
E 3
I 3
      case 1: break;

      case 2: creg1 = creg1 | (1 << (2*i));
              break;

      case 4: creg1 = creg1 | (1 << (2*i+1));
              break;

      case 8: creg1 = creg1 | (1 << (2*i));
              creg1 = creg1 | (1 << (2*i+1));
              break;
E 3
    }
  }

  fprintf(stderr, "creg1 = %d\n", creg1);
  fprintf(stderr, "creg2 = %d\n", creg2);

I 3
D 9
#ifndef OLDLOAD
E 9
  bigdata[0x320000/2] = creg1;
D 9
#else
E 3
  ep_creg1_download(creg1, dlhn);
I 3
#endif

#ifndef OLDLOAD
E 9
  bigdata[0x340000/2] = creg2;
D 9
#else
E 3
  ep_creg2_download(creg2, dlhn);
D 3
/*
  evt_proc_init (dlhn);
*/
E 3
I 3
#endif
E 9

D 9
#ifndef OLDLOAD
D 4
  bigdata_write(bigdata,"./test1.dat");
E 4
I 4

E 9
  /* download , upload and compare */
E 4
  bigdata_download(bigdata, dlhn);
I 4
  for(i=0; i<BIGDATA/sizeof(short); i++) bigref[i] = 0; /* to make sure */
E 4
  bigdata_upload(bigref, dlhn);
I 8
#ifndef TEST_SETUP
E 8
D 4
  bigdata_write(bigref,"./test2.dat");

  /* compare arrays */
E 4
  for(i=0; i<BIGDATA/sizeof(short); i++)
  {
    if(bigdata[i] != bigref[i])
    {
      printf("fileio: ERROR: at addr=0x%08x in=%d out=%d\n",
                i*2,bigdata[i],bigref[i]);
I 12
      return;
E 12
    }
  }
I 8
#endif
E 8
I 4

  /* create reference files */
  if((parm = getenv("CLON_PARMS")) == NULL)
  {
    printf("fileio: ERROR: CLON_PARMS is not set - cannot create level1 file\n");
  }
  else
  {
    sprintf(fname1,"%s/ts/level1.dat",parm);
    sprintf(fname2,"%s/ts/level1_out.dat",parm);
    bigdata_write(bigdata,fname1);
    bigdata_write(bigref,fname2);
  }

E 4
D 9
#endif

E 9
  /* evt_proc_init(dlhn); */

  return;
E 3
}

/**************************************************************************
 Procedure compare_addresses
**************************************************************************/
D 3
void compare_addresses( memory_data, check_address, X_mask, which_bit, max_X)
unsigned short int memory_data[65536], check_address, X_mask;
int which_bit, max_X;
E 3
I 3
D 6
void
E 6
I 6
static void
E 6
compare_addresses(unsigned short int memory_data[65536],
                  unsigned short int check_address,
                  unsigned short int X_mask, int which_bit, int max_X)
E 3
{
  unsigned short int and_address, xor_address, shifted_data;
  int loop, address;

  shifted_data = 0x0000;
  shifted_data = shifted_data | (1<<which_bit);

  /* loop over all 64K locations loading shifted_data if address valid */
  address = 0;
D 3
  for (loop=0; loop<65536;loop++)
E 3
I 3
  for(loop=0; loop<65536;loop++)
E 3
  {
    and_address = address & X_mask;

    /* xor and_address against check_address and if valid address will get 0 */
    xor_address = and_address ^ check_address;

D 3
    if (xor_address == 0)
E 3
I 3
    if(xor_address == 0)
E 3
      memory_data[address] = memory_data[address] | shifted_data;

    address++;
  }
I 3

  return;
E 3
}

D 6


/* -------------------------- Function make_php_address ------------------------*/
E 6
I 6
/* -------------------------- Function make_php_address ---------------------*/
E 6
/*
 * This function creates one of two possible 16 bit addresses from an 18 bit
 * address specified by trigger_num and product parameters.  The two addresses
 * possible correspond to the shuffle network scramble of bits on the php
 * board.
 */
D 3
void make_php_address(check_address, X_mask, max_X, trig_num, product, which_address)
unsigned short int *check_address, *X_mask;
int trig_num, product, *max_X, which_address;
E 3
I 3

D 6
void
E 6
I 6
static void
E 6
make_php_address(unsigned short int *check_address,
                 unsigned short int *X_mask,
                 int *max_X, int trig_num, int product, int which_address)
E 3
{
  int address_array[16], i;
  
  if (!which_address)
  {
    /* create 16 bit address using high SE's from sectors 1-3 */
D 3
    for (i=0;i<11;i++)
E 3
I 3
    for(i=0;i<11;i++)
E 3
      address_array[i] = phase3.Trig[trig_num][product][i];
    address_array[11] = phase3.Trig[trig_num][product][12];
    address_array[12] = phase3.Trig[trig_num][product][13];
    address_array[13] = phase3.Trig[trig_num][product][15];
    address_array[14] = phase3.Trig[trig_num][product][16];
    address_array[15] = phase3.Trig[trig_num][product][18];
  }
  else
  {
    /* create 16 bit address using high SE's from sectors 4-6 */
    address_array[0] = phase3.Trig[trig_num][product][0];
    address_array[1] = phase3.Trig[trig_num][product][1];
    address_array[2] = phase3.Trig[trig_num][product][3];
    address_array[3] = phase3.Trig[trig_num][product][4];
    address_array[4] = phase3.Trig[trig_num][product][6];
    address_array[5] = phase3.Trig[trig_num][product][7];
D 3
    for (i=9;i<19;i++)
E 3
I 3
    for(i=9;i<19;i++)
E 3
      address_array[i-3] = phase3.Trig[trig_num][product][i];
  }

  /* find number and locations of all X values in address array */
  *max_X = 16;
D 3
  for (i=0;i<16;i++)
E 3
I 3
  for(i=0;i<16;i++)
  {
    if (address_array[i] != 2)
E 3
    {
D 3
      if (address_array[i] != 2)
	{
	  *X_mask = *X_mask | (1<<i);
	  (*max_X)--;
	}
      if (address_array[i] == 1)
	*check_address = *check_address | (1<<i);
E 3
I 3
      *X_mask = *X_mask | (1<<i);
      (*max_X)--;
E 3
    }
I 3
    if(address_array[i] == 1) *check_address = *check_address | (1<<i);
  }

  return;
E 3
}

I 10
#else /* UNIX only */

void
fileio_dummy()
{
}

#endif
E 10

E 1
