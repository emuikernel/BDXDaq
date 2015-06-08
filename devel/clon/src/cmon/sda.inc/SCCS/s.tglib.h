h05119
s 00004/00003/00097
d D 1.5 07/11/03 13:32:23 boiarino 6 5
c *** empty log message ***
e
s 00024/00168/00076
d D 1.4 07/11/01 14:50:00 boiarino 5 4
c new STN readout from Franz
c 
e
s 00001/00001/00243
d D 1.3 07/10/26 14:19:56 boiarino 4 3
c *** empty log message ***
e
s 00129/00020/00115
d D 1.2 07/10/26 14:18:10 boiarino 3 1
c bla
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/tglib.h
e
s 00135/00000/00000
d D 1.1 01/11/19 19:00:39 boiarino 1 0
c date and time created 01/11/19 19:00:39 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _TGLIB_

#ifdef	__cplusplus
extern "C" {
#endif


/* macros */

D 3
/* get TGT raw data; input: jw; output: tgtndig, tgtdigi */
E 3
I 3
D 4
/* get T-counters TDC data */
E 4
I 4
D 5
/* get T-counters TDC data and converts to 'ns' */
E 4
E 3
#define GETTGTDATA \
E 5
I 5
/* get Start Counter raw data; input: jw; output: stndig, stdigi */
/* curently TDCs only !!! */
/* fills a la old ST */
#define GETSTDATA \
E 5
I 3
{ \
D 5
  int i, j, nrow; \
E 5
I 5
  int i, itdc, iadc, nrow; \
E 5
  unsigned short *i16; \
E 3
D 5
  tgtndig = 0; \
D 3
  if((ind=etNlink(jw,"TAGT",0)) > 0 || (ind=etNlink(jw,"TAGT",1)) > 0) \
E 3
I 3
  for(i=0;i<61;i++) for(j=0;j<3;j++) tgtdigi[i][j]=0; \
  if((ind=etNlink(jw,"TGTL",0)) > 0) \
E 5
I 5
  for(ih=0; ih<24; ih++) \
E 5
E 3
  { \
D 3
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
E 3
I 3
D 5
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
E 3
    { \
D 3
      for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1]; \
      nd2 = 2*nd - 1; \
      for(i=0; i<nd2; i+=3) \
E 3
I 3
      id = i16[0]; \
      if(id < 1 || id > 61) continue; \
      /* take only first hit: need something better .. */ \
      j = 0; \
      for(i=0; i<tgtndig; i++) \
E 3
      { \
D 3
        id = i16[i]; \
        if(id < 1 || id > 61) continue; \
        if(tgtndig >= 62) continue; \
E 3
I 3
        if(tgtdigi[i][0]==id) \
        { \
          j = 1; \
          break; \
        } \
      } \
      if(j==0) \
      { \
        tgtdigi[tgtndig][0] = id; \
        tgtdigi[tgtndig][1] = i16[1]; \
E 3
        tgtndig ++; \
D 3
        ih = tgtndig; \
        tgtdigi[ih-1][0] = i16[i]; \
        tgtdigi[ih-1][1] = i16[i+1]; \
        tgtdigi[ih-1][2] = i16[i+2]; \
E 3
I 3
        if(tgtndig > 61) break; \
E 3
      } \
I 3
      i16 += 2; \
E 3
    } \
E 5
I 5
    stdigi[ih][0] = 0; \
    stdigi[ih][1] = 0; \
E 5
D 3
  }
E 3
I 3
  } \
D 5
  if((ind=etNlink(jw,"TGTR",0)) > 0) \
E 5
I 5
  for(i=0; i<=6; i++) \
    stndig[i]=0; \
  if((ind=etNlink(jw,"STN0",0)) > 0) \
E 5
  { \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
    { \
D 5
      id = i16[0]; \
      if(id < 1 || id > 61) continue; \
      /* take only first hit, and look for the left partner; */ \
      /* if left partner does not exist, skip the right one */ \
      for(i=0; i<tgtndig; i++) \
      { \
        if((tgtdigi[i][0]==id)&&((tgtdigi[i][2]==0))) \
        { \
          tgtdigi[i][0] = id; \
          tgtdigi[i][2] = i16[1]; \
          break; \
        } \
      } \
      i16 += 2; \
    } \
  } \
  /*printf("T-counters data ---\n"); \
  for(i=0; i<tgtndig; i++) \
    printf("[%2d] %5d %5d\n",tgtdigi[i][0],tgtdigi[i][1],tgtdigi[i][2]);*/ \
}
E 3

D 3
/* get TGE raw data; input: jw; output: tgendig, tgedigi */
E 3
I 3


/* get TAGE raw data; input: jw; output: tgendig, tgedigi */
E 3
#define GETTGEDATA \
  tgendig = 0; \
  if((ind=etNlink(jw,"TAGE",0)) > 0 || (ind=etNlink(jw,"TAGE",1)) > 0) \
  { \
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
    { \
      for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1]; \
      nd2 = 2*nd - 1; \
      for(i=1; i<=nd2; i+=2) \
      { \
        id = i16[i-1]; \
        if(id < 1 || id > 384) continue; \
        if(tgendig >= 384) continue; \
        tgendig ++; \
        ih = tgendig; \
        tgedigi[ih-1][0] = i16[i-1]; \
        tgedigi[ih-1][1] = i16[i+1-1]; \
      } \
    } \
  }

I 3


E 3
/* get BM raw data; input: jw; output: bmndig, bmdigi */
I 3
/* we have several hits, howto choose right one ??? */
E 3
#define GETBMDATA \
I 3
{ \
  int i, nrow; \
  unsigned short *i16; \
E 3
  bmndig = 0; \
  for(ih=0; ih<7; ih++) \
  { \
    bmdigi[ih][0] = 0; \
    bmdigi[ih][1] = 0; \
    bmdigi[ih][2] = 0; \
  } \
I 3
  if((ind=etNlink(jw,"RFT ",0)) > 0) \
  { \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNdata(jw,ind); \
    bmndig = 1; \
    for(i=0; i<nrow; i++) \
    { \
E 5
      ih = i16[0]; \
D 5
      if(ih==6) /* we have 6 and 7, lets use 6 */ \
E 5
I 5
      itdc = i16[1]; \
      if(ih > 0 && ih < 25 && itdc > 0) \
E 5
      { \
D 5
        bmdigi[0][0] = i16[0]; \
        bmdigi[0][1] = i16[1]; \
        /* no ADCs bmdigi[ih-1][2] = i16[];*/ \
E 5
I 5
        stdigi[ih-1][0] = itdc; \
        stndig[0]++; \
        stndig[(ih+3)/4]++; \
E 5
      } \
      i16 += 2; \
    } \
  } \
D 5
}


#define GETBMDATA_OLD \
  bmndig = 0; \
  for(ih=0; ih<7; ih++) \
E 5
I 5
  if((ind=etNlink(jw,"STN1",0)) > 0) \
E 5
  { \
D 5
    bmdigi[ih][0] = 0; \
    bmdigi[ih][1] = 0; \
    bmdigi[ih][2] = 0; \
  } \
E 3
  if((ind=etNlink(jw,"CALL",0)) > 0) \
  { \
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
    { \
      for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1]; \
      bmndig = 7; \
      nd2 = 2*nd - 1; \
      for(i=1; i<=nd2; i+=3) \
      { \
        ih = i16[i-1]; \
        if(ih > 0 && ih <= 7) \
        { \
          bmdigi[ih-1][0] = i16[i-1]; \
          bmdigi[ih-1][1] = i16[i+1-1]; \
          bmdigi[ih-1][2] = i16[i+2-1]; \
        } \
      } \
    } \
  }

D 3
/* get ST raw data; input: jw; output: stndig, stdigi */
E 3
I 3


/* get Start Counter raw data; input: jw; output: stndig, stdigi */
/* curently TDCs only !!! */
/* fills a la old ST */
E 3
#define GETSTDATA \
I 3
{ \
  int i, idold, nrow; \
  unsigned short *i16; \
E 3
  stndig  = 0; \
  for(ih=1; ih<=6; ih++) \
I 3
  { \
    stdigi[ih-1][0] = 0; \
    stdigi[ih-1][1] = 0; \
    stdigi[ih-1][2] = 0; \
  } \
  if((ind=etNlink(jw,"STN0",0)) > 0) \
  { \
E 5
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
D 5
    stndig = 6; \
E 5
    for(i=0; i<nrow; i++) \
    { \
      ih = i16[0]; \
D 5
      if(ih > 0) \
E 5
I 5
      iadc = i16[1]; \
      if(ih > 0 && ih < 25 && iadc > 0) \
E 5
      { \
D 5
        idold = (ih-1) / 4; \
printf("%d -> %d\n",ih,idold); \
        stdigi[idold][0] = ih; \
        stdigi[idold][1] = i16[1]; \
E 5
I 5
        stdigi[ih-1][1] = iadc; \
E 5
      } \
D 5
	  i16 += 2; \
E 5
I 5
      i16 += 2; \
E 5
    } \
  } \
D 6
  printf("Start Counter data ---\n"); \
D 5
  for(i=0; i<stndig; i++) \
    printf("[%2d] %5d %5d\n",stdigi[i][0],stdigi[i][1],stdigi[i][2]); \
E 5
I 5
  for(i=0; i<stndig[0]; i++) { \
E 6
I 6
  /*printf("Start Counter data ---\n"); \
  for(i=0; i<stndig[0]; i++) \
  { \
E 6
    if(stdigi[i][0]) printf("[%2d] %5d %5d\n",i,stdigi[i][0],stdigi[i][1]); \
D 6
  } \
E 6
I 6
  }*/ \
E 6
E 5
}

#define GETSTDATA_OLD \
  stndig  = 0; \
  for(ih=1; ih<=6; ih++) \
E 3
  { \
    stdigi[ih-1][0] = 0; \
    stdigi[ih-1][1] = 0; \
    stdigi[ih-1][2] = 0; \
  } \
  if((ind=etNlink(jw,"ST  ",0)) > 0) \
  { \
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
    { \
      for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1]; \
      stndig = 6; \
      nd2 = 2*nd - 1; \
      for(i=1; i<=nd2; i+=3) \
      { \
        ih = i16[i-1]; \
        if(ih > 0) \
        { \
          stdigi[ih-1][0] = i16[i-1]; \
          stdigi[ih-1][1] = i16[i+1-1]; \
          stdigi[ih-1][2] = i16[i+2-1]; \
        } \
      } \
    } \
  }

#ifdef	__cplusplus
}
#endif


#define _TGLIB_
#endif
E 1
