#ifndef _TGLIB_

#ifdef	__cplusplus
extern "C" {
#endif


/* macros */

/* get Start Counter raw data; input: jw; output: stndig, stdigi */
/* curently TDCs only !!! */
/* fills a la old ST */
#define GETSTDATA \
{ \
  int i, itdc, iadc, nrow; \
  unsigned short *i16; \
  for(ih=0; ih<24; ih++) \
  { \
    stdigi[ih][0] = 0; \
    stdigi[ih][1] = 0; \
  } \
  for(i=0; i<=6; i++) \
    stndig[i]=0; \
  if((ind=etNlink(jw,"STN0",0)) > 0) \
  { \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
    { \
      ih = i16[0]; \
      itdc = i16[1]; \
      if(ih > 0 && ih < 25 && itdc > 0) \
      { \
        stdigi[ih-1][0] = itdc; \
        stndig[0]++; \
        stndig[(ih+3)/4]++; \
      } \
      i16 += 2; \
    } \
  } \
  if((ind=etNlink(jw,"STN1",0)) > 0) \
  { \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
    { \
      ih = i16[0]; \
      iadc = i16[1]; \
      if(ih > 0 && ih < 25 && iadc > 0) \
      { \
        stdigi[ih-1][1] = iadc; \
      } \
      i16 += 2; \
    } \
  } \
  /*printf("Start Counter data ---\n"); \
  for(i=0; i<stndig[0]; i++) \
  { \
    if(stdigi[i][0]) printf("[%2d] %5d %5d\n",i,stdigi[i][0],stdigi[i][1]); \
  }*/ \
}

#define GETSTDATA_OLD \
  stndig  = 0; \
  for(ih=1; ih<=6; ih++) \
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
