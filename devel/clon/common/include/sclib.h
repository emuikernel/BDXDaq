#ifndef _SCLIB_

/* piece from sdageom.h */
#define npl_sc  4

#ifdef	__cplusplus
extern "C" {
#endif

/* sclib.h */

typedef struct scdata *SCDataPtr;
typedef struct scdata
{
#ifdef Linux
  unsigned char  slab;     /* layer number */
  unsigned char  layer;     /* strip number */
#else
  unsigned char  layer;
  unsigned char  slab;
#endif
  unsigned short tdcl;		/* tdc left */
  unsigned short adcl;		/* adc left */
  unsigned short tdcr;		/* tdc right */
  unsigned short adcr;		/* adc right */
} SCData;




/* functions */

void scmatch_(int *jw, TRevent *ana, int *isec, DCscid *scid, float x0[npl_sc], int *iflg);
void scmatch(int *jw, TRevent *ana, int isec, DCscid *scid, float x0[npl_sc], int *iflg);

/* macros */

/* get SC raw data; input: jw, isec; output: scndig, scdigi */
#define GETSCDATA \
  /* first, get ADCs from old bank */ \
  scndig = 0; \
  if((ind=etNlink(jw,"SC  ",isec)) > 0) \
  { \
    int i, j, id, idhigh, nrow, place; \
    unsigned short *i16; \
    i16 = (unsigned short *) &jw[ind]; \
    nrow = etNrow(jw,ind); \
    for(i=0; i<nrow; i++) \
    { \
      id = i16[0] & 0xFF; \
      if(id < 1 || id > 57) continue; \
      scdigi[scndig][0] = id; \
      scdigi[scndig][1] = 0; /* reserved for left TDC */ \
      scdigi[scndig][2] = i16[2]; \
      scdigi[scndig][3] = 0; /* reserved for right TDC */ \
      scdigi[scndig][4] = i16[4]; \
      scndig++; \
      if(scndig >= 57) break; \
      i16 += 5; \
    } \
/*printf("\nSC ======================================\n"); \
for(i=0; i<scndig; i++) printf("[%2d] %5d %5d %5d %5d\n", \
scdigi[i][0],scdigi[i][1],scdigi[i][2],scdigi[i][3],scdigi[i][4]);*/ \
    /* now get TDCs from new bank */ \
    if((ind=etNlink(jw,"SCT ",isec)) > 0) \
    { \
      i16 = (unsigned short *) &jw[ind]; \
      nrow = etNrow(jw,ind); \
      for(i=0; i<nrow; i++) \
      { \
        id = i16[0]&0xFF; /*low byte is counter number from 1 to 57*/ \
        idhigh = (i16[0]>>8)&&0x1; /*low bit in high byte is left-right flag*/ \
        if(idhigh==0) place = 1; \
        else          place = 3; \
        if(id < 1 || id > 57) continue; \
        for(j=0; j<scndig; j++) \
        { \
          if(id == scdigi[j][0] && scdigi[j][place] == 0) /* take only first TDC hit */ \
          { \
            scdigi[j][place] = i16[1]; \
            break; \
          } \
        } \
        i16 += 2; \
      } \
    } \
  } \
/*printf("\nSCT =====================================\n"); \
for(i=0; i<scndig; i++) printf("[%2d] %5d %5d %5d %5d\n", \
scdigi[i][0],scdigi[i][1],scdigi[i][2],scdigi[i][3],scdigi[i][4]); \
printf("\n")*/;

#define GETSCDATA_OLD \
  scndig = 0; \
  if((ind=etNlink(jw,"SC  ",isec)) > 0) \
  { \
    int nd, nd2; \
    int i32[7000]; \
    short *i16; \
    i16 = (short *) i32; \
    if((nd = etNdata(jw,ind)) > 0) \
    { \
      for(i=0; i<nd; i++) i32[i] = jw[ind+i]; \
      nd2 = 2*nd - 1; \
      for(i=0; i<nd2; i+=5) \
      { \
        int id; \
        id = i16[i]; \
        if(id < 1 || id > 48) continue; \
        scdigi[scndig][0] = id; \
        scdigi[scndig][1] = i16[i+1]; \
        scdigi[scndig][2] = i16[i+2]; \
        scdigi[scndig][3] = i16[i+3]; \
        scdigi[scndig][4] = i16[i+4]; \
        scndig++; \
        if(scndig >= 48) break; \
      } \
    } \
  }


#ifdef	__cplusplus
}
#endif


#define _SCLIB_
#endif
