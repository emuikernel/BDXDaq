h20112
s 00001/00001/00902
d D 1.4 07/10/12 10:14:46 boiarino 5 4
c MIN was defined as MAX !!! fixed
c 
e
s 00001/00001/00902
d D 1.3 02/03/07 16:27:09 boiarino 4 3
c return (result ? result : 0x80);
c 
e
s 00057/00053/00846
d D 1.2 02/03/05 21:57:49 boiarino 3 1
c new version
e
s 00000/00000/00000
d R 1.2 01/11/19 18:54:36 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/l3.s/electron_check.c
e
s 00899/00000/00000
d D 1.1 01/11/19 18:54:35 boiarino 1 0
c date and time created 01/11/19 18:54:35 by boiarino
e
u
U
f e 0
t
T
I 1

/* electron_check.c - level3 from Franz Klein */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "bosio.h"
I 3
#include "sdakeys.h"
SDAkeys keys;
E 3

#ifndef ABS
#define ABS(a)   ((a) < 0 ? -(a):(a))
#define MAX(a,b) ((a) > (b) ? (a):(b))
D 5
#define MIN(a,b) ((a) > (b) ? (b):(a))
E 5
I 5
#define MIN(a,b) ((a) < (b) ? (b):(a))
E 5
#endif
#ifndef MAX_LINELEN
#define MAX_LINELEN 128
#endif
#ifndef MAX_FILELEN 
#define MAX_FILELEN 256
#endif

/*  BOS bank structures  */

#ifdef Linux
typedef struct {
  unsigned char wire;
  unsigned char layer;
  unsigned short tdc;
} DC0_t;

typedef struct {
  unsigned char id;
  unsigned char layer;
  unsigned short tdc;
  unsigned short adc;
} PMT1_t;

#else

typedef struct {
  unsigned char layer;
  unsigned char wire;
  unsigned short tdc;
} DC0_t;

typedef struct {
  unsigned char layer;
  unsigned char id;
  unsigned short tdc;
  unsigned short adc;
} PMT1_t;

#endif

typedef struct {
  unsigned short id;
  unsigned short tdcl;
  unsigned short adcl;
  unsigned short tdcr;
  unsigned short adcr;
} PMT2_t;

typedef struct {
  int id;
  int mean;
  float sigma;
} PED1_t;

typedef struct {
  int id;
  int mean_left;
  float sigma_left;
  int mean_right;
  float sigma_right;
} PED2_t;


/* prototype for Map routine */
extern int map_get_float();

/* output: bit mask for sectors where electron candidates were found */
int l3_init1(int runnumb);  
int l3_event1(int *iw);  
int electron_check(int *jw);
D 3
void electron_check_init(char *pedfile, int ped_flag, int runno, 
			 int EL_outbending, int EG1_TG_position,
			 int SCdead_pattern[6], int CC_matching, 
E 3
I 3
void electron_check_init(char *pedfile, int flg_pedfile, int runno, 
			 int EL_outbending, int TGBI_latch1, int flg_EG1_TG_pos,
			 int SCdead_pattern[6], int flg_CC_matching, 
E 3
			 int CC_threshold, int ECi_threshold, 
			 int ECtot_threshold, int ECview_threshold);

/*  all ids-1 !         1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 */
const int ecuminp[24]={-1,-1,-1,-1,-1, 7, 8,10,11,13,14,16,17,19,20,22,23,25,26,28,29,30,31,100};
const int ecumaxp[24]={ 5, 8, 9,11,12,14,15,17,18,20,22,23,25,26,28,29,31,32,34,36,36,36,36,-100};
const int ecvwminp[24]={30,29,28,27,25,23,22,20,19,18,16,15,13,12,11, 9, 8, 6, 5, 4, 2, 1,-1,37};

/*                      1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 */
const int dc2min1p[24]={0, 0, 0, 0, 0, 0, 0, 0, 1, 7,10,13,17,20,24,27,31,35,36,40,44,46, 0, 0};
const int dc2max1p[24]={5, 5, 9,12,16,19,23,27,31,34,38,41,44,48,51,54,57,60,62,65,69,71,-4,-4};
const int dc3min1p[24]={0, 0, 0, 0, 2, 5, 7,10,13,17,20,23,25,28,31,34,37,40,42,45,48,50, 0, 0};
const int dc3max1p[24]={5, 6, 9,10,15,19,22,25,28,32,35,38,41,44,47,50,53,56,59,61,64,67,-3,-3};
const int dc2min2p[24]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 0, 0};
const int dc2max2p[24]={-3,-3,-3,-3,-3,6, 7,10,13,20,24,28,31,35,39,43,47,51,52,56,60,61,-4,-4};
const int dc3min2p[24]={0, 0, 0, 0, 0, 0, 7, 8,10,12,14,16,17,19,21,22,24,26,27,29,30,31, 0, 0}; 
const int dc3max2p[24]={-3,-3,-3,-3,-3,9,14,16,20,24,28,31,34,38,41,44,47,50,52,56,59,61,-3,-3};


/* all ids-1 !           1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 */
const int  ecumine[24]={-1, 0, 2, 4, 5, 7, 8,10,12,13,15,16,18,19,21,22,24,25,27,29,30,32,33,100};
const int  ecumaxe[24]={ 6, 7, 9,10,12,13,14,16,18,19,21,22,24,25,27,28,30,31,33,35,36,36,36,-100};
const int ecvwmine[24]={29,28,27,25,24,22,21,20,18,17,15,14,13,11,10, 8, 7, 6, 4, 3, 1, 0,-1,37};

/*                       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 */
const int dc2min1e[24]={ 2, 3, 6, 8,11,13,16,19,21,24,27,29,32,35,37,40,43,45,48,50,53,55,59,0};
const int dc2max1e[24]={37,35,35,38,41,44,44,47,50,53,55,58,59,61,65,67,68,71,72,75,76,80,86,-4};
const int dc3min1e[24]={ 0, 0, 1, 3, 6, 8,11,14,17,20,23,25,28,31,34,36,39,42,44,47,49,52,54,0}; 
const int dc3max1e[24]={16,16,18,21,24,27,28,31,34,37,40,43,45,48,51,54,56,59,61,64,66,69,73,-3};
const int dc2min2e[24]={18,18,18,21,24,26,27,29,32,35,38,41,41,44,47,50,51,54,55,59,60,63,71,0};
const int dc2max2e[24]={72,73,74,76,77,78,79,81,82,84,87,88,90,91,93,95,95,95,95,95,95,95,95,-3};
const int dc3min2e[24]={ 2, 3, 5, 8,10,13,16,18,21,24,27,30,32,35,38,41,43,46,48,51,53,56,59,0};
const int dc3max2e[24]={33,36,37,40,42,45,47,50,52,55,57,61,63,65,68,71,73,76,78,81,83,86,87,-3};

/* pol. target position: */
/*                       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 */
const int dc2tmin1e[24]={ 6, 7,10,12,15,18,21,24,27,30,33,37,40,43,46,49,52,55,58,61,64,67,70,0};
const int dc2tmax1e[24]={37,39,42,43,46,50,53,56,59,62,65,67,71,73,75,76,78,81,84,88,88,91,96,-4};
const int dc3tmin1e[24]={ 0, 0, 3, 5, 8,11,14,17,20,23,26,29,32,35,38,40,43,46,49,52,54,57,60,0}; 
const int dc3tmax1e[24]={16,17,20,22,25,28,31,34,37,40,43,46,49,52,55,57,60,62,66,69,70,73,77,-3};
const int dc2tmin2e[24]={20,23,26,26,29,32,34,37,40,43,45,48,51,54,56,57,60,64,67,70,71,74,80,0};
const int dc2tmax2e[24]={77,79,80,82,83,86,88,90,94,95,97,101,103,106,108,111,114,116,119,122,123,127,127,-3};
const int dc3tmin2e[24]={ 4, 5, 8,10,13,16,19,22,25,27,30,33,36,39,42,44,47,50,53,56,58,61,63,0};
const int dc3tmax2e[24]={33,36,38,41,43,45,48,51,54,56,60,62,65,68,71,74,77,80,82,85,88,90,91,-3};

/*  visible energy threshold ~10 MeV, ADC conversion 10 cnts/MeV */ 
/*  threshold 100mV ~280-300 MeV -> conversion: 10*2.8 */
#define EC_CONVERSION 28
#define MIN_ECISUM 40 
#define MIN_ECTOTSUM 60 
#define MIN_ECVIEWSUM 15 
#define MIN_CCADC  50

/* sub ( add ) value to ecumin[id] ( ecumax[id] ) for CC cut  */
#define CC_OFFMINE 2
#define CC_OFFMAXE 7
#define CC_OFFMINP 10
#define CC_OFFMAXP 2

/* parameters */
static char *clonparms;
static char *ec_tped_file = "/pedman/Tfiles/ec.tped";
D 3
static int ped_flag = 0;
E 3
static char ec_pedfile[128];
I 3
static int ped_flag    = 1;
static int use_TGBI_latch1 = 1;
static int flg_outbending  = 0;       /* 0=inbending */
static int EG1_TG_position = 0;
E 3
static int SCdead_pattern[6]={0,0,0,0,0,0};
D 3
static int flg_outbending = 0; /* inbending */
E 3
static int CC_matching = 1;
D 3
static int CC_thresh = -1;
E 3
I 3
static int CC_thresh  = -1;
E 3
static int ECi_thresh = -1;
static int ECt_thresh = -1;
static int ECv_thresh = -1;
D 3
static int EG1_TG_position = 1;
E 3

/* DC patterns: for R2 & R3 per SCid */
static unsigned int dc2patt1[24];
static unsigned int dc2patt2[24];
static unsigned int dc3patt1[24];
static unsigned int dc3patt2[24];
static unsigned int ecupatt[24];
static unsigned int ecvwpatt[24];
static unsigned int ccpatt[24];
static unsigned int scdead_patt[6];
static int dc_maxwire[24];

#define MIN_ECID 257
#define MAX_ECID 1573
static short ecped[6][216];
static int ECi_minsum, ECtot_minsum, ECview_minsum;
static short CC_minadc;
D 3
static int CC_match_flag;
E 3
static int secbit[6];
I 3
static int flg_TGBI_latch1=0x003F;
static int flg_CC_matching;
E 3

/******************************************************************/
/******************************************************************/
/***********           electron_check_init               **********/
/******************************************************************/

#define NWBOS 30000

void
D 3
electron_check_init(char *pedfile, int ped_flag, int runno, 
			 int EL_outbending, int EG1_TG_position,
			 int SCdead_pattern[6], int CC_matching, 
E 3
I 3
electron_check_init(char *pedfile, int flg_pedfile, int runno, 
			 int EL_outbending, int TGBI_latch1, int flg_EG1_TG_pos,
			 int SCdead_pattern[6], int flg_CC_matching, 
E 3
			 int CC_threshold, int ECi_threshold, 
			 int ECtot_threshold, int ECview_threshold)
{
  int *jw;
  int isec, il, id, i, n, id0, iped;
  int first, endofbank, ind;
  float sig, val[216];
  int nlines=0;
  int boslun=0;
  int ierr=0;
  PED1_t *ecpe;
  FILE *fp = NULL;
  char *ec_ped_txtfile="ec_ped_default.txt";
  char map_item[15];
  char *map_layer[3]={"u","v","w"};
  char line[MAX_LINELEN];

D 3
  CC_match_flag=CC_matching;
E 3
I 3
  flg_CC_matching=CC_matching;
  if ( !TGBI_latch1 ) flg_TGBI_latch1=0;
E 3

D 3
 /*  read database (runno>0) or "parms_clasprod_nnnnnn.dat" (runno<0) 
     tped-file (runno=0)  for EC-pedestqals  */
E 3
I 3
 /*  read EC pedestals from:  database   (for flg_pedfile>0 and runno>0) 
       or  "parms_clasprod_nnnnnn.dat"   (for flg_pedfile<0 and runno>0) 
       or  tped-file                     (for flg_pedfile=0)           */

E 3
  for (isec=0; isec<6; isec++) {
    secbit[isec]=1<<isec;        
    /* if Sector data not initialized: set secbit[isec]=0 */
    for (il=0; il<6; il++)
      for (i=0; i<36; i++) 
	ecped[isec][il*36+i]=400;
  }

D 3
  if (!pedfile) ped_flag=0;
E 3
I 3
  if (runno<0) { runno=-runno; flg_pedfile=-1; }
  if (!pedfile || !runno)  flg_pedfile=0;
E 3

D 3
  if (ped_flag>0) {
E 3
I 3
  if (flg_pedfile>0) {
E 3
    /* Map: /group/clas/parms/Maps/EC_PEDESTALS.map
       Subsystem: inner,       nitems: 3
       Item: u,        length: 216,    type: float,
       Item: v,        length: 216,    type: float,
       Item: w,        length: 216,    type: float,
       Subsystem: outer,       nitems: 3
       Item: u,        length: 216,    type: float,
       Item: v,        length: 216,    type: float,
       Item: w,        length: 216,    type: float,
    */
    sprintf(map_item,"inner");
    for (il=0; ierr==0 && il<3; il++) {
      ierr=map_get_float(pedfile,map_item,map_layer[il],216,val,runno,&first);
      if (!ierr) {
	nlines++;
	for (i=0; i<216; i++) 
	  ecped[i/36][i%36+il*36]=val[i];
      }
    }
    sprintf(map_item,"outer");
    for (il=0; ierr==0 && il<3; il++) {
      map_get_float(pedfile,map_item,map_layer[il],216,val,runno,&first);
      nlines++;
      for (i=0; i<216; i++) 
	ecped[i/36][i%36+(il+3)*36]=val[i];
    }
D 3
    if (ierr) nlines=ped_flag=0;
E 3
I 3
    if (ierr) nlines=flg_pedfile=0;
E 3
  }

D 3
  else if (ped_flag<0) {
E 3
I 3
  else if (flg_pedfile<0) {
E 3
    jw=(int *)malloc(NWBOS*sizeof(int));
    if (jw) bosInit(jw,NWBOS);
    if (!jw || bosOpen(pedfile,"r",&boslun)!=0 || bosRead(boslun,jw,"E")!=0) {
      fprintf(stderr,"%s:  bosOpen or bos Read Error  %s\n",__FILE__,pedfile);
D 3
      ped_flag=0;
E 3
I 3
      flg_pedfile=0;
E 3
    }
    else {
      for (isec=0; isec<6; isec++) {
	if ( (ind=etNlink(jw,"ECPE",isec+1)) > 0) {
	  endofbank=(int)(&jw[ind]+etNdata(jw,ind));
	  ecpe=(PED1_t *)&jw[ind];
	  while ((int)ecpe < endofbank) {
	    id=ecpe->id -MIN_ECID;
	    if (id>=0 && id<MAX_ECID-MIN_ECID) {
	      nlines++;
	      il=id/256; i=id%256;
	      if (i<36) 
		ecped[isec][il*36+i]=ecpe->mean;
	    }
	    ecpe++;
	  }
	}
	else {
	  secbit[isec]=0;
	}
      }
      bosClose(boslun);
    }
    if (jw) free(jw);
  }

D 3
  if (!ped_flag) {
E 3
I 3
  if (!flg_pedfile) {
E 3
    if (pedfile && access(pedfile,R_OK)!=EOF) {
      fp=fopen(pedfile,"r");
      fgets(line,MAX_LINELEN,fp);
      while (fgets(line,MAX_LINELEN,fp)!=NULL) {
	nlines++;
	if (*line == '/' || *line == '#') continue;
	if (*line == '*' ) {
	  if (strncmp(line,"*SECTORS*",9)) continue;
	  fgets(line,MAX_LINELEN,fp);
	  nlines++;
	  continue;
	}
	isec=-1;
	n=sscanf(line,"%d %d %d %f",&id,&isec,&iped,&sig);
	if (n!=4) 
	  fprintf(stderr,"%s:  ERROR reading %s : line %d\n",__FILE__,pedfile,nlines); 
	if (isec<0 || isec>5 || id<MIN_ECID || id>MAX_ECID) continue;
	il=id/256 -1;
	i=id%256 -1;
	if (i<0 || i>35) continue;
	ecped[isec][i+il*36]=iped;
      }
      fclose(fp);
    }

    else if (access(ec_ped_txtfile,R_OK)!=EOF) {
      pedfile=ec_ped_txtfile;
      fp=fopen(ec_ped_txtfile,"r");
      id=0;
      while ( fgets(line,MAX_LINELEN,fp)!=NULL && *line!='E') {
	nlines++;
	if( *line!='/' && *line!='*' ) {
	  for (i=0; i<12; i++) {
	    n=sscanf(line,"%d",&id0);
	    if (n) {
	      il=id/216;
	      isec=(id-il*216)/36;
	      ecped[isec][il*36+id%36]=id0;
	    }
	    id++;
	  }
	}
      }
      fclose(fp);
    }
  }
D 3
  /*  for (isec=0; isec<6; isec++)
  for (il=0; il<6; il++)
  printf("is %d il %d: %d %d %d %d %d %d %d %d %d %d\n",isec,il,ecped[isec][il*36],ecped[isec][il*36+1],ecped[isec][il*36+2],ecped[isec][il*36+3],ecped[isec][il*36+4],ecped[isec][il*36+5],ecped[isec][il*36+6],ecped[isec][il*36+7],ecped[isec][il*36+8],ecped[isec][il*36+9]); */
E 3
  if (nlines) 
    fprintf(stderr,"%s:  Read EC pedestals from %s : %d lines\n",__FILE__,pedfile,nlines);

D 3
  CC_minadc=MIN_CCADC;
  if (CC_threshold > 0 ) 
    CC_minadc=(short)CC_threshold;
E 3
I 3
  CC_minadc=    (CC_threshold > 0) ? (short)CC_threshold : MIN_CCADC; 
  ECi_minsum=   EC_CONVERSION * (ECi_threshold > 0) ? ECi_threshold : MIN_ECISUM;
  ECtot_minsum= EC_CONVERSION * (ECtot_threshold>0) ? ECtot_threshold : MIN_ECTOTSUM;
  ECview_minsum=EC_CONVERSION * (ECview_threshold>=0) ? ECview_threshold : MIN_ECVIEWSUM;
E 3

D 3
  ECi_minsum=MIN_ECISUM*EC_CONVERSION;
  if (ECi_threshold > 0 ) 
    ECi_minsum=ECi_threshold*EC_CONVERSION;

  ECtot_minsum=MIN_ECTOTSUM*EC_CONVERSION;
  if (ECtot_threshold > 0 ) 
    ECtot_minsum=ECtot_threshold*EC_CONVERSION;

  ECview_minsum=MIN_ECVIEWSUM*EC_CONVERSION;
  if (ECview_threshold >= 0 ) 
    ECview_minsum=ECview_threshold*EC_CONVERSION;

E 3
  for (id=0; id<24; id++) {
    dc2patt1[id]=0;
    dc2patt2[id]=0;
    dc3patt1[id]=0;
    dc3patt2[id]=0;
    ecupatt[id] =0;
    ecvwpatt[id]=0;
    ccpatt[id]  =0;
  }
  for (i=0; i<6; i++) 
    scdead_patt[i]= (SCdead_pattern[i]>0)? SCdead_pattern[i] : 0;

  dc_maxwire[0]=dc_maxwire[12]=0; 

  if (EL_outbending) {

    for (id=0; id<24; id++) {
      for (i=ecuminp[id]/2; i<=ecumaxp[id]/2; i++)
	ecupatt[id]|=1<<i;
      for (i=ecvwminp[id]/2; i<=18; i++)
	ecvwpatt[id]|=1<<i;
      for (i=MAX(0,(ecuminp[id]-CC_OFFMINP)/2); i<=MIN(31,(ecumaxp[id]+CC_OFFMAXP)/2); i++)
	ccpatt[id]|=1<<i;
    }

D 3
    if (EG1_TG_position) {
E 3
I 3
    if (flg_EG1_TG_pos) {
E 3

      for (id=0; id<24; id++) {
	for (i=dc2min1p[id]/4; i<=(dc2max1p[id]+3)/4; i++)
	  dc2patt1[id]|=1<<i;
	for (i=dc2min2p[id]/4; i<=(dc2max2p[id]+3)/4; i++) 
	  dc2patt2[id]|=1<<i;
	for (i=dc3min1p[id]/3; i<=(dc3max1p[id]+2)/3; i++) 
	  dc3patt1[id]|=1<<i;
	for (i=dc3min2p[id]/3; i<=(dc3max2p[id]+2)/3; i++) 
	  dc3patt2[id]|=1<<i;
      }

      for (id=15; id<24; id++) {
	if (dc2max1p[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max1p[id];
	if (dc2max2p[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max2p[id];
	
	if (dc3max1p[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max1p[id];
	if (dc3max2p[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max2p[id];
      }

    }
    else {    /* default target position :  */

      for (id=0; id<24; id++) {
	for (i=dc2min1p[id]/4; i<=(dc2max1p[id]+3)/4; i++)
	  dc2patt1[id]|=1<<i;
	for (i=dc2min2p[id]/4; i<=(dc2max2p[id]+3)/4; i++) 
	  dc2patt2[id]|=1<<i;
	for (i=dc3min1p[id]/3; i<=(dc3max1p[id]+2)/3; i++) 
	  dc3patt1[id]|=1<<i;
	for (i=dc3min2p[id]/3; i<=(dc3max2p[id]+2)/3; i++) 
	  dc3patt2[id]|=1<<i;
      }

      for (id=15; id<24; id++) {
	if (dc2max1p[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max1p[id];
	if (dc2max2p[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max2p[id];
	
	if (dc3max1p[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max1p[id];
	if (dc3max2p[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max2p[id];
      }
D 3
    }                  /*  if (EG1_TG_position) {  */
E 3
I 3
    }                  /*  if (flg_EG1_TG_pos) {  */
E 3

  }
  else {               /*  for inbending electrons:  */

    for (id=0; id<24; id++) {
      for (i=ecumine[id]/2; i<=ecumaxe[id]/2; i++)
	ecupatt[id]|=1<<i;
      for (i=ecvwmine[id]/2; i<=18; i++)
	ecvwpatt[id]|=1<<i;
      for (i=MAX(0,(ecumine[id]-CC_OFFMINE)/2); i<=MIN(31,(ecumaxe[id]+CC_OFFMAXE)/2); i++)
	ccpatt[id]|=1<<i;
    }

D 3
    if (EG1_TG_position) {
E 3
I 3
    if (flg_EG1_TG_pos) {
E 3

      for (id=0; id<24; id++) {
	for (i=dc2tmin1e[id]/4; i<=(dc2tmax1e[id]+3)/4; i++) 
	  dc2patt1[id]|=1<<i;
	for (i=dc2tmin2e[id]/4; i<=(dc2tmax2e[id]+3)/4; i++) 
	  dc2patt2[id]|=1<<i;
	for (i=dc3tmin1e[id]/3; i<=(dc3tmax1e[id]+2)/3; i++) 
	  dc3patt1[id]|=1<<i;
	for (i=dc3tmin2e[id]/3; i<=(dc3tmax2e[id]+2)/3; i++) 
	  dc3patt2[id]|=1<<i;
      }

      for (id=15; id<24; id++) {
	if (dc2max1e[id]>0 && dc2tmax1e[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2tmax1e[id];
	if (dc2max2e[id]>0 && dc2tmax2e[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2tmax2e[id];
	
	if (dc3max1e[id]>0 && dc3tmax1e[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3tmax1e[id];
	if (dc3max2e[id]>0 && dc3tmax2e[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3tmax2e[id];
      }

    }
    else {                   /*  default target position  */

      for (id=0; id<24; id++) {
	for (i=dc2min1e[id]/4; i<=(dc2max1e[id]+3)/4; i++) 
	  dc2patt1[id]|=1<<i;
	for (i=dc2min2e[id]/4; i<=(dc2max2e[id]+3)/4; i++) 
	  dc2patt2[id]|=1<<i;
	for (i=dc3min1e[id]/3; i<=(dc3max1e[id]+2)/3; i++) 
	  dc3patt1[id]|=1<<i;
	for (i=dc3min2e[id]/3; i<=(dc3max2e[id]+2)/3; i++) 
	  dc3patt2[id]|=1<<i;
      }

      for (id=15; id<24; id++) {
	if (dc2max1e[id]>0 && dc2max1e[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max1e[id];
	if (dc2max2e[id]>0 && dc2max2e[id]>dc_maxwire[0]) 
	  dc_maxwire[0]=dc2max2e[id];
	
	if (dc3max1e[id]>0 && dc3max1e[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max1e[id];
	if (dc3max2e[id]>0 && dc3max2e[id]>dc_maxwire[12]) 
	  dc_maxwire[12]=dc3max2e[id];
      }
    }  /*  if (EG_TG_position) {...} else {  */

  }    /*  if (EL_outbending) {...} else {  */ 

  for (i=1; i<12; i++) 
    dc_maxwire[i]=dc_maxwire[0];
  for (i=13; i<24; i++) 
    dc_maxwire[i]=dc_maxwire[12];

  fprintf(stderr,"%s:  run %d: Outbending %d, EG1_Target_pos %d, CC_match %d, CC_thr %d, ECi_thr %d, ECtot_thr %d, ECv_thr %d\n",
D 3
	  __FILE__,ABS(runno),EL_outbending,EG1_TG_position,CC_match_flag,CC_minadc,ECi_minsum,ECtot_minsum,ECview_minsum);
E 3
I 3
	  __FILE__,runno,EL_outbending,flg_EG1_TG_pos,flg_CC_matching,CC_minadc,ECi_minsum,ECtot_minsum,ECview_minsum);
E 3
  return;
}

/******************************************************************/
/***********             electron_check                  **********/
/******************************************************************/

int
electron_check(int *jw)
{
  int result=0, ccdat_res=0;
  unsigned int vbits=0;
  unsigned int ccbits=0;
  /*  unsigned int ecbits=0;
      unsigned int scbits=0;
      unsigned int dcbits=0; */
  
  DC0_t *dc;
  PMT1_t *ec, *cc;
  PMT2_t *sc;
  int indSC[6], indDC[6], indCC[6], indEC[6];
  int ndatSC[6], ndatDC[6], ndatCC[6], ndatEC[6];
D 3
  int indTGBI, latch2;
E 3
I 3
  int indTGBI, latch1;
E 3
  int endofbank, ind;

  int nsc, nsccc, nscec, nscdc;
  int vscid[24], vsccc[24], vscec[24]; 
  int ndc1[4], ndc2[4];

#define MAX_ECCLUST 6
  int ncl,n_ecl[3],emax,idmax;
  int echit[3][36];
  int iadc,sum,visum[3],ecisum,ectotsum;
  int ecl_sum[3][MAX_ECCLUST],ecl_max[3][MAX_ECCLUST];
  int ecl_id0[3][MAX_ECCLUST],ecl_id1[3][MAX_ECCLUST];

  unsigned int ecl_patt[3][MAX_ECCLUST];
  unsigned int wpatt[24];
  unsigned int patt, scbit[24];

  int id,id0,id1,nseg1,nseg2,k0;
  int i,j,isc,is,isl,il,il0,iwir;


  if (!*jw) return 0;
D 3
/*  if ( (indTGBI=etNlink(jw,"TGBI",0)) == 0 ) return 0;
	latch2=(unsigned int)&jw[indTGBI+3]; */
E 3
I 3
  latch1=0;
  if ( (indTGBI=etNlink(jw,"TGBI",0)) >0 ) 
    latch1=flg_TGBI_latch1 & (unsigned int)&jw[indTGBI+3];
  if(!latch1) latch1 = flg_TGBI_latch1;

E 3
  /*
  if ((ind=etNamind(jw,"EC  "))==0) return 0;
  printf("\nEC %d",ind);
  while ((ind=etNnext(jw,ind)) >0) {
    is=etNnum(jw,ind)-1;
    ecbits|=secbit[is];
    indEC[is] =ind;
    ndatEC[is]=etNdata(jw,ind);
    printf("sec%d %d ndat=%d; ",is,ind,ndatEC[is]);
  }
  if ((ind=etNamind(jw,"SC  "))==0) return 0;
  printf("\nSC %d",ind);
  while ((ind=etNnext(jw,ind)) >0) {
    is=etNnum(jw,ind) -1;
    scbits|=secbit[is];
    indSC[is] =ind;
    ndatSC[is]=etNdata(jw,ind);
    printf("sec%d %d ndat=%d; ",is,ind,ndatSC[is]);
  }
  if ((ind=etNamind(jw,"DC0 "))==0) return 0;
  printf("\nDC0 %d",ind);
  while ((ind=etNnext(jw,ind)) >0) {
    is=etNnum(jw,ind) -1;
    dcbits|=secbit[is];
    indDC[is] =ind;
    ndatDC[is]=etNdata(jw,ind);
    printf("sec%d %d ndat=%d; ",is,ind,ndatDC[is]);
  }
  if ((ind=etNamind(jw,"CC  "))>0) {
  printf("\nCC %d",ind);
    while ((ind=etNnext(jw,ind)) >0) {
      is=etNnum(jw,ind) -1;
      ccbits|=secbit[is];
      indCC[is] =ind;
      ndatCC[is]=etNdata(jw,ind);
    printf("sec%d %d ndat=%d; ",is,ind,ndatCC[is]);
    }
  }
  if( (vbits=ecbits&scbits&dcbits) ==0) return 0;
D 3
  if (CC_match_flag && (vbits&=ccbits)==0) return 0;
E 3
I 3
  if (flg_CC_matching && (vbits&=ccbits)==0) return 0;
E 3
  */

  for (is=0; is<6; is++) {
D 3
/*    if (!latch2&secbit[is]) continue; */
E 3
I 3
    if ( latch1&secbit[is] == 0 ) continue; 
E 3
    if ( (indEC[is]=etNlink(jw,"EC  ",is+1)) == 0 ) continue;
    if ( (ndatEC[is]=etNdata(jw,indEC[is])) == 0 )  continue;
    if ( (indSC[is]=etNlink(jw,"SC  ",is+1)) == 0 ) continue;
    if ( (ndatSC[is]=etNdata(jw,indSC[is])) == 0 )  continue;
    if ( (indDC[is]=etNlink(jw,"DC0 ",is+1)) == 0 ) continue;
    if ( (ndatDC[is]=etNdata(jw,indDC[is])) == 0 )  continue;
    vbits|=secbit[is];
    if ( (indCC[is]=etNlink(jw,"CC  ",is+1)) && (ndatCC[is]=etNdata(jw,indCC[is])) )
	ccbits|=secbit[is];
  }
  /*  printf("\nbits: %x %x %x %x - %x\n",ecbits,scbits,dcbits,ccbits,ecbits&scbits&dcbits);*/
  if (!vbits) return 0;
D 3
  if (CC_match_flag && (vbits&=ccbits)==0) return 0;
E 3
I 3
  if (flg_CC_matching && (vbits&=ccbits)==0) return 0;
E 3

  /********  loop over sectors with all required banks  ******/

  for (is=0; is<6; is++) {
    nscdc=0;
    if (!(vbits&secbit[is])) goto nextsector;

    patt=scdead_patt[is];
    endofbank=(int)(&jw[indSC[is]] + ndatSC[is]);
    sc=(PMT2_t *)&jw[indSC[is]];

    while ((int)sc < endofbank) {
      id=(int)sc->id -1;
      if (id>=0 && id<23 && (sc->tdcl>10 || sc->tdcr>10)) {
	patt|=1<<id;
	/*	printf("%d ",id);*/
      }
      sc++;
    }
    nsc=id=0;
    do {
      if ( patt&1 ) {
	scbit[nsc]=1<<id;
	vscid[nsc++]=id;
      }
      id++;
    } while ( patt=patt>>1 );
    /*    printf(" - nSC %d\n",nsc);*/
    if (!nsc) goto nextsector;            /* no forward SC hit */

    nsccc=0;
    if (ccbits&secbit[is]) {
      patt=0;
      k0=0;
      endofbank=(int)(&jw[indCC[is]] + ndatCC[is]);
      cc=(PMT1_t *)&jw[indCC[is]];

      while ((int)cc < endofbank) {
	if ( cc->id >0 && cc->adc > CC_minadc ) {
	  k0++;
	  id0=1<<(cc->id/2);
	  for (i=0; i<nsc; i++) {
	    id=vscid[i];
	    if (id0&ccpatt[id] && !(patt&scbit[i]) ) {
	      vsccc[nsccc++]=id;
	      patt |=scbit[i];
	      /*	printf("%d ",id);*/
	    }
	  }
	}
	cc++;
      }

      /*    printf(" - nSC-CC %d\n",nsccc);*/
D 3
      if (CC_match_flag) {
E 3
I 3
      if (flg_CC_matching) {
E 3
	if (!nsccc)  goto nextsector;            /* no SC-CC matching */
	nsc=nsccc;
	for (i=0; i<nsc; i++) vscid[i]=vsccc[i];
      }
      if (k0) ccdat_res|=secbit[is];

    }           /*  if 'cc_data'  */


    /*  EC matching  */
    nscec=0;
    endofbank=(int)(&jw[indEC[is]] + ndatEC[is]);
    ec=(PMT1_t *)&jw[indEC[is]];
    
    ecisum=ectotsum=0; 
    visum[0]=visum[1]=visum[2]=0;
    bzero((char *)echit, 108*sizeof(int));

    while ((int)ec < endofbank) {
      il=(int)ec->layer -1; 
      id=(int)ec->id -1;
      if (il>=0 && il<6 && id>=0 && id<36) {
	iadc=(int)(ec->adc - ecped[is][il*36+id]); 
	if (iadc>0) {
	  k0=il%3;
	  echit[k0][id]+=iadc;
	  visum[k0]    +=iadc;
	  ectotsum     +=iadc;
	  if (il<3) ecisum +=iadc;
	  /*	  	  printf(" %d-%d %d,",il,id,iadc);  */
	}
      }
      ec++;
    }
    /* enough energy in forward calorimeter?  */
    /*  printf("\n EC: sum %d %d %d: %d %d in Sec %d\n",visum[0],visum[1],visum[2],ecisum,ectotsum,is); */

    if (ecisum<ECi_minsum || ectotsum<ECtot_minsum)  goto nextsector;
    k0=0;
    for (il=0; il<3; il++) {
      if (visum[il]<ECview_minsum) {
	if(++k0>1) goto nextsector;
      }
    }

    /* find EC clusters with up to 2 maxima per cluster */

    k0=0;
    for (il=0; il<3; il++) {
      n_ecl[il]=ncl=0;
      sum=0;
      for (id=0; id<36; id++) {
	if (echit[il][id]) {
	  if (!sum) { 
	    id0 =idmax =id;
	    id1 =id-1;
	    emax=echit[il][id];
	  }
	  else if (echit[il][id]>emax) {
	    idmax=id;
	    emax=echit[il][id];
	  }
	  id1++;
	  sum+=echit[il][id];
	  if (id==35 || echit[il][id+1]==0) {
	    if (sum>ECview_minsum && ncl<MAX_ECCLUST) {
	      ecl_id0[il][ncl]=id0;
	      ecl_id1[il][ncl]=id1;
	      ecl_max[il][ncl]=idmax;
	      ecl_patt[il][ncl]=1<<(idmax/2);
	      ecl_sum[il][ncl++]=sum;
	    }
	    sum=0;
	    id0=-10;
	  }
	}
      }
      /*      if (!ncl) goto nextsector;   */ /* EC cluster missing in one orientation */
      if (ncl)  k0++;
      n_ecl[il]=ncl;
      for (i=0; i<ncl; i++) {
	id0=-1;
	idmax=ecl_max[il][i];
	emax=echit[il][idmax]/5;
	for ( id=ecl_id0[il][i]; id<ecl_id1[il][i]; id++) {
          if ((id<idmax-1 || id>idmax+1) && echit[il][id]>emax) {
	    emax=echit[il][id];
	    id0=id;
	  }
	}
	if (id0>0 && echit[il][id0-1]<emax && echit[il][id0+1]<emax) 
	  ecl_patt[il][i]|=1<<(id0/2);
      }
    }
    /*        printf(" nclust: %d %d %d k=%d\n",n_ecl[0],n_ecl[1],n_ecl[2],k0); */
    if (k0<2)  goto nextsector;    /* EC cluster missing in more than one orientation */

    /* matching between EC clusters and SC hits */
    for (isc=0; isc<nsc; isc++) {
      id=vscid[isc];
      k0=0;
      for (i=0; i<n_ecl[0]; i++) {
	if (ecl_patt[0][i]&ecupatt[id]) {
	  k0++;
	  break;
	}
      }
      for (il=1; il<3; il++) {
	for (i=0; i<n_ecl[il]; i++) {
	  if (ecl_patt[il][i]&ecvwpatt[id]) {
	    k0++;
	    break;
	  }
	}
      }
      if(k0>1)  vscec[nscec++]=id;
    }

    /*    printf(" - nSC-EC %d\n",nscec); */
    if (!nscec)  goto nextsector;     /* no EC-SC matching */

    /*  DC region 2 and 3 data  */

    endofbank=(int)(&jw[indDC[is]] + ndatDC[is]);
    dc=(DC0_t *)&jw[indDC[is]];
    bzero((char *)wpatt, 72*sizeof(int));
    
    while ((int)dc < endofbank) {
      il=(int)dc->layer -13;
      iwir=(int)dc->wire-1;
      if (il>=0 && il<24 && iwir>=0 && iwir<dc_maxwire[il]) 
	wpatt[il] |= (il<12) ? 1<<(iwir/4) : 1<<(iwir/3) ;
      dc++;
    }

    /*  matching between SC hit (with EC,CC info) and DC region 3 patterns */

    for (isc=0; isc<nscec; isc++) {
      id=vscec[isc];

      for (isl=2; isl<4; isl++) {
	ndc1[isl]=ndc2[isl]=0;
	for (il=isl*6; il<(isl+1)*6; il++) {
	  if (wpatt[il]&dc3patt1[id]) ndc1[isl]++;
	  if (wpatt[il]&dc3patt2[id]) ndc2[isl]++;
	}
      }
      /*      printf("ndc1 R3 %d %d     ndc2 R3 %d %d\n",ndc1[2],ndc1[3],ndc2[2],ndc2[3]); */
      if (ndc1[2]>2 && ndc1[3]>2) {
	/*	vscdc[nscdc++]=id;  */
	nscdc++;
	goto nextsector;                /* success: pattern in DC region 3 */
      }

      nseg1=(ndc1[2]>2 || ndc1[3]>2) ? 1 : 0;
      nseg2=(ndc2[2]>2) ? 1 : 0;
      if (ndc2[3]>2) nseg2++;
      /*      printf("nseg1 %d, nseg2 %d\n",nseg1,nseg2); */
      if (nseg1+nseg2==0) continue;

      /* for low mom. tracks (group 2 patterns) 
	 and missing segment in one Reg.3 superlayer
	 check DC Region 2 patterns                */
      /* - case1: one Region 3 segment in group 1  */

      ndc1[0]=ndc1[1]=0;
      if (nseg1) {
	for (isl=0; isl<2; isl++) {
	  for (il=isl*6; il<(isl+1)*6; il++) {
	    if (wpatt[il]&dc2patt1[id]) ndc1[isl]++;
	  }
	  if (ndc1[isl]<3) break;     /* no segment in one Reg.2 superlayer */
	}
      }
      /*      printf("ndc1 R2 %d %d\n",ndc1[0],ndc1[1]); */
      if (ndc1[1]>2) {
	/*	vscdc[nscdc++]=id; */
	nscdc++;
	goto nextsector;           /* success: segments in one Reg.3 and 
				      both Reg.2 superlayers             */
      }

      /* - case2: both Reg.3 segments in group 2 -> one more in Reg.2 */
      /* - case3: one Reg.3 segment in group 2  -> two more in Reg.2  */
      ndc2[0]=ndc2[1]=0;
      isl=-1;
      while (nseg2 && ++isl<2) {
	for (il=isl*6; il<(isl+1)*6; il++) {
	  if (wpatt[il]&dc2patt2[id]) ndc2[isl]++;
	}
	if (nseg2==1 && ndc2[isl]<3) 
	  nseg2=0;                 /* no segment in one Reg.2 superlayer */
      }
      /*      printf("ndc2 R2 %d %d\n",ndc2[0],ndc2[1]); */
      if (nseg2 && (ndc2[0]>2 || ndc2[1]>2)) {
	/*	vscdc[nscdc++]=id; */
	nscdc++;
        goto nextsector;           /* success: segments in Reg.3 and 
				      Reg.2 superlayers               */
      }
    }
nextsector:
    if (nscdc) result|=secbit[is];
  }
  /*  printf("** result: %x , sccc %x, ccdat %x\n",result,sccc_res,ccdat_res); */
  if (result&ccdat_res)
    result&=ccdat_res;
D 4
  return result;
E 4
I 4
  return (result ? result : 0x80);
E 4
}


/* standard interface - do not change names or parameters !!! */

int
D 3
l3_init1(int runno)
E 3
I 3
l3_init1(int run_number)
E 3
{
  int i;

D 3
  for (i=0; i<6; i++) secbit[i]=1<<i;

E 3
  clonparms = getenv("CLON_PARMS");
  if(clonparms)
  {
    strcpy(ec_pedfile,clonparms);
    strcat(ec_pedfile,ec_tped_file);
    ped_flag=0;
  }
D 3
  electron_check_init(ec_pedfile,ped_flag,runno,flg_outbending,EG1_TG_position,
E 3
I 3
  if (keys.zmagn[1] < 0)   flg_outbending=1;
  if (keys.zmagn[3] !=0.0) EG1_TG_position=1;
  if (keys.l3p1[0] > 0.0) CC_thresh =(int)keys.l3p1[0];
  if (keys.l3p1[1] > 0.0) ECi_thresh=(int)keys.l3p1[1];
  if (keys.l3p1[2] > 0.0) ECt_thresh=(int)keys.l3p1[2];
  if (keys.l3p1[3] > 0.0) ECv_thresh=(int)keys.l3p1[3];
  for (i=0; i<6; i++) 
    if (keys.l3p1[i+4] !=0.0) SCdead_pattern[i]=(int)keys.l3p1[i+4];

  electron_check_init(ec_pedfile,ped_flag,run_number,flg_outbending,use_TGBI_latch1,EG1_TG_position,
E 3
                          SCdead_pattern,CC_matching,CC_thresh,ECi_thresh,ECt_thresh,ECv_thresh);

  return(0);
}

int
l3_event1(int *iw)
{
  return(electron_check(iw));
}  
E 1
