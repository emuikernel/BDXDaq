
/* uthbook.c - mini-hbook package (Sergey Boyarinov) */

/* %d for VXWORKS, %f if not !!! - TO DO */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uthbook.h"
#ifdef VXWORKS
#include "ttbosio.h"
#endif

#define MIN(x,y) ((x) < (y) ? (x) : (y))

#ifndef VXWORKS

static UThistf *histlocal;

void
uthsetlocal(UThistf *hist)
{
  histlocal = hist;
printf("set &hist[0].nbinx=0x%08x 0x%08x\n",&hist[0].nbinx,&histlocal[0].nbinx);

  return;
}

void
uthgetlocal(UThistf **hist)
{
  *hist = histlocal;
printf("get &hist[0].nbinx=0x%08x 0x%08x\n",&hist[0]->nbinx,&histlocal[0].nbinx);

  return;
}

void
uthbook1_(int *id, char *title, int *nbinx, float *xmin, float *xmax, int len)
{
  char *tit;

  while(title[len-1]==' ') len--; /* remove traling spaces */
  tit = (char *) malloc(len+1);
  strncpy((char *)tit, (char *)title, len);
  tit[len] = '\0';

  uthbook1(histlocal, *id, tit, *nbinx, *xmin, *xmax);
}
#endif

void
uthbook1(hstr *hist, int id, char *title, int nbinx, hvar xmin, hvar xmax)
{
  int i, nch;

  if(id <=0 || id >=NHIST)
  {
    printf("uthbook1: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  if(hist[id].nbinx > 0)
  {
    /*printf("uthbook1: WARN id=%d already exist - will be replaced\n",id);*/
    hist[id].nbinx = 0;
    /*printf("uthbook1: free histogram buffer ... ");*/
    free(hist[id].buf);
    /*printf("done.\n");*/
  }

  hist[id].entries = 0;
  hist[id].xunderflow = 0;
  hist[id].xoverflow = 0;
  hist[id].xmin = xmin;

  hist[id].dx = (xmax - xmin)/(hvar)nbinx;
  /* check bin size consistency */
  if(hist[id].dx <= HZERO)
  {
    printf("uthbook1: ERROR dx=%d must be > 0\n",hist[id].dx);
    return;
  }
  if( (xmin + nbinx*hist[id].dx) < xmax)
  {
    hist[id].nbinx = (xmax - xmin)/hist[id].dx + 1;
    hist[id].xmax = xmin + ((hvar)nbinx)*hist[id].dx;
    printf("uthbook1: WARN rebining for [%d]: nbinx=%d xmax=%d\n",
      id,hist[id].nbinx,hist[id].xmax);
  }
  else
  {
    hist[id].nbinx = nbinx;
    hist[id].xmax = xmax;
  }

  hist[id].nbiny = 0; /* to recognize 1-dim from 2-dim */

  nch = strlen(title);
  hist[id].nwtitle = (nch+3)/4;
  hist[id].title = (char *) malloc(4*hist[id].nwtitle);
  strncpy(hist[id].title,title,nch);
  for(i=nch; i<4*hist[id].nwtitle; i++) hist[id].title[i] = ' ';

  hist[id].buf = (hvar *) calloc(hist[id].nbinx,sizeof(hvar));
  if(hist[id].buf==NULL) {printf("uthbook: ERROR in calloc()\n");exit(0);}

#ifdef VXWORKS
  if(hist[id].nbinx <= MAXI2 && hist[id].nbiny <= MAXI2 &&
     hist[id].xmin <= MAXI2 && hist[id].xmax <= MAXI2 &&
     hist[id].ymin <= MAXI2 && hist[id].ymax <= MAXI2)
  {
    hist[id].ifi2_book = 1;
    hist[id].ifi2_fill = 1;
  }
  else
  {
    hist[id].ifi2_book = 0;
    hist[id].ifi2_fill = 0;
  }
#endif
  /*printf("uthbook1: [%d] nbinx=%d xmin=%d xmax=%d nwtitle=%d\n",
    id,hist[id].nbinx,hist[id].xmin,hist[id].xmax,hist[id].nwtitle);*/

  return;
}

#ifndef VXWORKS
void
uthbook2_(int *id, char *title, int *nbinx, float *xmin, float *xmax,
                                int *nbiny, float *ymin, float *ymax, int len)
{
  char *tit;

  while(title[len-1]==' ') len--; /* remove traling spaces */
  tit = (char *) malloc(len+1);
  strncpy((char *)tit, (char *)title, len);
  tit[len] = '\0';

  uthbook2(histlocal, *id, tit, *nbinx, *xmin, *xmax, *nbiny, *ymin, *ymax);
}
#endif

void
uthbook2(hstr *hist, int id, char *title, int nbinx, hvar xmin, hvar xmax,
                        int nbiny, hvar ymin, hvar ymax)
{
  int i, nch;

  if(id <=0 || id >=NHIST)
  {
    printf("uthbook2: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  if(hist[id].nbinx > 0 || hist[id].nbiny > 0)
  {
    /*printf("uthbook2: WARN id=%d already exist - will be replaced\n",id);*/
    hist[id].nbinx = 0;
    hist[id].nbiny = 0;
    return;
  }

  hist[id].entries = 0;
  hist[id].xunderflow = 0;
  hist[id].xoverflow = 0;
  hist[id].yunderflow = 0;
  hist[id].yoverflow = 0;
  hist[id].xmin = xmin;
  hist[id].ymin = ymin;

  hist[id].dx = (xmax - xmin)/(hvar)nbinx;
  if(hist[id].dx <= HZERO)
  {
    printf("uthbook2: ERROR dx=%d must be > 0\n",hist[id].dx);
    return;
  }
  /* SEE CHECK IN 1D !!! */
  hist[id].dy = (ymax - ymin)/(hvar)nbiny;
  if(hist[id].dy <= HZERO)
  {
    printf("uthbook2: ERROR dy=%d must be > 0\n",hist[id].dy);
    return;
  }
  /* SEE CHECK IN 1D !!! */

  hist[id].nbinx = nbinx;
  hist[id].xmax = xmax;
  hist[id].nbiny = nbiny;
  hist[id].ymax = ymax;

  nch = strlen(title);
  hist[id].nwtitle = (nch+3)/4;
  hist[id].title = (char *) malloc(4*hist[id].nwtitle);
  strncpy(hist[id].title,title,nch);
  for(i=nch; i<4*hist[id].nwtitle; i++) hist[id].title[i] = ' ';

  hist[id].buf2 = (hvar **) calloc(nbinx,sizeof(hvar));
  if(hist[id].buf2==NULL) {printf("uthbook: ERROR1 in calloc()\n");exit(0);}
  for(i=0; i<nbinx; i++)
  {
    hist[id].buf2[i] = (hvar *) calloc(nbiny,sizeof(hvar));
    if(hist[id].buf2[i]==NULL) {printf("uthbook: ERROR2 in calloc()\n");exit(0);}
  }
  printf("uthbook2: [%d] nbinx=%d xmin=%d xmax=%d nbiny=%d ymin=%d ymax=%d\n",
    id,hist[id].nbinx,hist[id].xmin,hist[id].xmax,
       hist[id].nbiny,hist[id].ymin,hist[id].ymax);

  return;
}


#ifndef VXWORKS
void
uthfill_(int *id, float *x, float *y, float *weight)
{
  uthfill(histlocal, *id, *x, *y, *weight);
}
#endif

void
uthfill(hstr *hist, int id, hvar x, hvar y, hvar weight)
{
  int ibinx, ibiny;

  if(id <=0 || id >=NHIST)
  {
    /*printf("uthfill: ERROR id=%d, must be from 1 to %d\n",id,NHIST);*/
    fflush(stdout);
    return;
  }

  if(hist[id].nbinx <= 0)
  {
    printf("uthbook1: ERROR id=%d is not booked\n",id);
    fflush(stdout);
    return;
  }

  hist[id].entries += weight;
  if(x < hist[id].xmin)
  {
    hist[id].xunderflow += weight;
#ifdef VXWORKS
    if(hist[id].xunderflow > MAXI2) hist[id].ifi2_fill = 0;
#endif
  }
  else if(x >= hist[id].xmax)
  {
    hist[id].xoverflow += weight;
#ifdef VXWORKS
    if(hist[id].xoverflow > MAXI2) hist[id].ifi2_fill = 0;
#endif
  }
  else
  {
    ibinx = (int)((x - hist[id].xmin) / hist[id].dx);
    if(hist[id].nbiny == 0) /* 1-dim */
    {
      hist[id].buf[ibinx] += weight;
#ifdef VXWORKS
      if(hist[id].buf[ibinx] > MAXI2) hist[id].ifi2_fill = 0;
#endif
    }
    else                    /* 2-dim */
    {
      if(y < hist[id].ymin)
      {
        hist[id].yunderflow += weight;
      }
      else if(y >= hist[id].ymax)
      {
        hist[id].yoverflow += weight;
      }
      else
      {
        ibiny = (int)((y - hist[id].ymin) / hist[id].dy);
        hist[id].buf2[ibinx][ibiny] += weight;
      }
    }
  }

  return;
}


/* reset histogram contents and change title optionaly (not yet !) */

void
uthreset(hstr *hist, int id, char *title)
{
  int ibinx, ibiny;

  if(id <=0 || id >=NHIST)
  {
    printf("uthreset: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  if(hist[id].nbinx <= 0)
  {
    printf("uthreset: ERROR id=%d is not booked\n",id);
    return;
  }

  hist[id].entries = 0;
  hist[id].xunderflow = 0;
  hist[id].xoverflow = 0;
  hist[id].yunderflow = 0;
  hist[id].yoverflow = 0;

  if(hist[id].nbiny == 0) /* 1-dim */
  {
    for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      hist[id].buf[ibinx] = 0;
  }
  else                    /* 2-dim */
  {
    for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
        hist[id].buf2[ibinx][ibiny] = 0;
  }

  return;
}


void
uthprint(hstr *hist, int id)
{
  int i;

  if(id <=0 || id >=NHIST)
  {
    printf("uthprint: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  if(hist[id].nbinx <= 0)
  {
    printf("uthprint: ERROR id=%d is not booked\n",id);
    return;
  }
  
  if(hist[id].nbiny > 0)
  {
    printf("uthprint: ERROR id=%d: 2-dim cannot be printed  - sorry\n",id);
    return;
  }

  for(i=0; i<hist[id].nbinx; i++)
  {
    printf("[%d] --> %d\n",i,hist[id].buf[i]);
  }
  printf("hist %d: xunderflow=%d xoverflow=%d\n",
    id,hist[id].xunderflow,hist[id].xoverflow);

  return;
}


/* report histogram contents as array */
/* 'choice' and 'num' for 2-dim hists only - not done yet !!! */

void
uthunpak(hstr *hist, int id, hvar *content, char *choice, int num)
{
  int i;

  if(id <=0 || id >=NHIST)
  {
    printf("uthunpak: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  if(hist[id].nbinx <= 0)
  {
    printf("uthunpak: ERROR id=%d is not booked\n",id);
    return;
  }
  
  if(hist[id].nbiny > 0)
  {
    printf("uthunpak: ERROR id=%d: 2-dim is not suppported, use 'uthij'\n",id);
    return;
  }

  for(i=0; i<hist[id].nbinx; i++)
  {
    content[i] = hist[id].buf[i];
  }

  return;
}


/* return contents of the bin (i,j) of 2-dim histogram */

hvar
uthij(hstr *hist, int id, int ibinx, int ibiny)
{
  hvar tmp;
  /*printf("=> %d %d %d\n",id,ibinx,ibiny);fflush(stdout);*/
  tmp = hist[id].buf2[ibinx][ibiny];
  /*printf("=> %f\n",tmp);fflush(stdout);*/

  return(tmp);
}


/* returns histogram parameters */

void
uthgive(hstr *hist, int id, char *title, int *nbinx, hvar *xmin,
        hvar *xmax, int *nbiny, hvar *ymin, hvar *ymax, int *titlelen)
{
  int i, len;

  if(id <=0 || id >=NHIST)
  {
    printf("uthgive: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return;
  }

  len = MIN(*titlelen,4*hist[id].nwtitle);
  for(i=0; i<*titlelen; i++) title[i] = ' '; /* fill all 'title' by ' ' */
  strncpy(title,hist[id].title,len);
  *titlelen = len;

  *nbinx = hist[id].nbinx;
  *xmin = hist[id].xmin;
  *xmax = hist[id].xmax;

  if(hist[id].nbiny > 0)
  {
    *nbiny = hist[id].nbiny;
    *ymin = hist[id].ymin;
    *ymax = hist[id].ymax;
  }
  else
  {
    *nbiny = 0;
  }

  return;
}


void
uthidall(hstr *hist, int *ids, int *n)
{
  int id, nid;

  nid = 0;
  for(id=0; id<NHIST; id++)
  {
    if(hist[id].nbinx > 0) ids[nid++] = id; 
  }
  *n = nid;
  /*printf("uthidall: nid=%d (%d %d %d ...)\n",nid,ids[0],ids[1],ids[2]);*/

  return;
}


/* returns the number of entries */

int
uthentries(hstr *hist, int id)
{
  if(id <=0 || id >=NHIST)
  {
    printf("uthentries: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  return(hist[id].entries);
}


/* save histograms in a form of BOS banks */

/*
#if defined(VXWORKS) || defined(Linux)
*/
#ifdef VXWORKS


/*
 !!! THE HISTOGRAM WILL BE CLEANED UP !!!

  HISI bank format:

    bank number = histogram id
    bank format : 'I'
    bank contents:

    jw[ 0] - histogram format (0-normal, 1-"bin-contents")
    jw[ 1] - nbinx
    jw[ 2] - xmin
    jw[ 3] - xmax
    jw[ 4] - xunderflow
    jw[ 5] - xoverflow
    jw[ 6] - nbiny
    jw[ 7] - ymin
    jw[ 8] - ymax
    jw[ 9] - yunderflow
    jw[10] - yoverflow
    jw[11] - entries

    jw[12] - title length (words)
    jw[13] - jw[xx] - title

    jw[xx+1] - jw[yy] - data

 */

int
uth2bos(hstr *hist, int id, long *jw)
{
  int ibinx, ibiny, ind, len, ncol, nrow, nbins, packed;
  unsigned long *buf, *start;
  unsigned short *buf2, *start2;

  if(id <=0 || id >=NHIST)
  {
    /*printf("uth2bos1: ERROR id=%d, must be from 1 to %d\n",id,NHIST);*/
    return(0);
  }

  len = 0;
  ncol = 1;
  nrow = hist[id].nbinx;
  if(hist[id].nbiny > 0) nrow = nrow * hist[id].nbiny;

  if(hist[id].ifi2_book && hist[id].ifi2_fill)
  {
    if( (ind = bosMopen_(jw,"HISS",id,ncol,nrow)) > 0)
    {
      buf2 = start2 = (unsigned short *)&jw[ind+1];

	  /* NORMAL or PACKED */
      nbins = 0;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        if(hist[id].buf[ibinx] > 0) nbins++;
      }
      packed = PACKED_I2;
      if(hist[id].nbiny == 0 && hist[id].nbinx < nbins*2) packed = NORMAL_I2;
      if(hist[id].nbiny > 0 && hist[id].nbinx < nbins*3) packed = NORMAL_I2;

      *buf2++ = packed;
      *buf2++ = hist[id].nbinx;
      *buf2++ = hist[id].xmin;
      *buf2++ = hist[id].xmax;
      *buf2++ = hist[id].xunderflow;
      *buf2++ = hist[id].xoverflow;
      *buf2++ = hist[id].nbiny;
      *buf2++ = hist[id].ymin;
      *buf2++ = hist[id].ymax;
      *buf2++ = hist[id].yunderflow;
      *buf2++ = hist[id].yoverflow;
      *buf2++ = hist[id].entries;

      *buf2++ = 2*hist[id].nwtitle;
      strncpy((char *)buf2,hist[id].title,4*hist[id].nwtitle);
      buf2 += 2*hist[id].nwtitle;

      if(packed == NORMAL_I2)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            *buf2++ = hist[id].buf[ibinx];
            hist[id].buf[ibinx] = 0;
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              *buf2++ = hist[id].buf2[ibinx][ibiny];
              hist[id].buf2[ibinx][ibiny] = 0;
            }
          }
        }
      }
      else if(packed == PACKED_I2)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            if(hist[id].buf[ibinx] > 0)
            {
              *buf2++ = ibinx;
              *buf2++ = hist[id].buf[ibinx];
              hist[id].buf[ibinx] = 0;
            }
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              if(hist[id].buf2[ibinx][ibiny] > 0)
              {
                *buf2++ = ibinx;
                *buf2++ = ibiny;
                *buf2++ = hist[id].buf2[ibinx][ibiny];
                hist[id].buf2[ibinx][ibiny] = 0;
              }
            }
          }
        }
      }

      nrow = (int)(buf2 - start2);

      len = bosMclose_(jw,ind,ncol,nrow);
    }
  }
  else
  {
    if( (ind = bosMopen_(jw,"HISI",id,ncol,nrow)) > 0)
    {
      buf = start = (unsigned long *)&jw[ind+1];

	  /* NORMAL or PACKED */
      nbins = 0;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        if(hist[id].buf[ibinx] > 0) nbins++;
      }
      packed = PACKED_I4;
      if(hist[id].nbiny == 0 && hist[id].nbinx < nbins*2) packed = NORMAL_I4;
      if(hist[id].nbiny > 0 && hist[id].nbinx < nbins*3) packed = NORMAL_I4;

      *buf++ = packed;
      *buf++ = hist[id].nbinx;
      *buf++ = hist[id].xmin;
      *buf++ = hist[id].xmax;
      *buf++ = hist[id].xunderflow;
      *buf++ = hist[id].xoverflow;
      *buf++ = hist[id].nbiny;
      *buf++ = hist[id].ymin;
      *buf++ = hist[id].ymax;
      *buf++ = hist[id].yunderflow;
      *buf++ = hist[id].yoverflow;
      *buf++ = hist[id].entries;

      *buf++ = hist[id].nwtitle;
      strncpy((char *)buf,hist[id].title,4*hist[id].nwtitle);
      buf += hist[id].nwtitle;

      if(packed == NORMAL_I4)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            *buf++ = hist[id].buf[ibinx];
            hist[id].buf[ibinx] = 0;
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            { 
              *buf++ = hist[id].buf2[ibinx][ibiny];
              hist[id].buf2[ibinx][ibiny] = 0;
            }
          }
        }
      }
      else if(packed == PACKED_I4)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            if(hist[id].buf[ibinx] > 0)
            {
              *buf++ = ibinx;
              *buf++ = hist[id].buf[ibinx];
               hist[id].buf[ibinx] = 0;
            }
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              if(hist[id].buf2[ibinx][ibiny] > 0)
              {
                *buf++ = ibinx;
                *buf++ = ibiny;
                *buf++ = hist[id].buf2[ibinx][ibiny];
                hist[id].buf2[ibinx][ibiny] = 0;
              }
            }
          }
        }
      }

      nrow = (int)(buf - start);

      len = bosMclose_(jw,ind,ncol,nrow);
    }
  }

  hist[id].entries = 0;
  hist[id].xunderflow = 0;
  hist[id].xoverflow = 0;
  hist[id].yunderflow = 0;
  hist[id].yoverflow = 0;

  if(hist[id].nbinx <= MAXI2 && hist[id].nbiny <= MAXI2 &&
     hist[id].xmin <= MAXI2 && hist[id].xmax <= MAXI2 &&
     hist[id].ymin <= MAXI2 && hist[id].ymax <= MAXI2)
  {
    hist[id].ifi2_book = 1;
    hist[id].ifi2_fill = 1;
  }
  else
  {
    hist[id].ifi2_book = 0;
    hist[id].ifi2_fill = 0;
  }

  /*logMsg("ncol=%d nrow=%d len=%d\n",ncol,nrow,len,0,0,0);*/

  return(len);
}






/* exactly the same but use bosNopen/bosNclose instead of bosMopen/bosMclose;
 MUST do that until ROL2 will be transformed to compact (2 word) BOS format */

int
uth2bos1(hstr *hist, int id, long *jw)
{
  int ibinx, ibiny, ind, len, ncol, nrow, nbins, packed;
  unsigned long *buf, *start;
  unsigned short *buf2, *start2;

  if(id <=0 || id >=NHIST)
  {
    /*printf("uth2bos1: ERROR id=%d, must be from 1 to %d\n",id,NHIST);*/
    return(0);
  }

  len = 0;
  ncol = 1;
  nrow = hist[id].nbinx;
  if(hist[id].nbiny > 0) nrow = nrow * hist[id].nbiny;

  if(hist[id].ifi2_book && hist[id].ifi2_fill)
  {
    if( (ind = bosNopen(jw,*((unsigned long *)"HISS"),id,ncol,nrow)) > 0)
    {
      buf2 = start2 = (unsigned short *)&jw[ind+1];

	  /* NORMAL or PACKED */
      nbins = 0;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        if(hist[id].buf[ibinx] > 0) nbins++;
      }
      packed = PACKED_I2;
      if(hist[id].nbiny == 0 && hist[id].nbinx < nbins*2) packed = NORMAL_I2;
      if(hist[id].nbiny > 0 && hist[id].nbinx < nbins*3) packed = NORMAL_I2;

      *buf2++ = packed;
      *buf2++ = hist[id].nbinx;
      *buf2++ = hist[id].xmin;
      *buf2++ = hist[id].xmax;
      *buf2++ = hist[id].xunderflow;
      *buf2++ = hist[id].xoverflow;
      *buf2++ = hist[id].nbiny;
      *buf2++ = hist[id].ymin;
      *buf2++ = hist[id].ymax;
      *buf2++ = hist[id].yunderflow;
      *buf2++ = hist[id].yoverflow;
      *buf2++ = hist[id].entries;

      *buf2++ = 2*hist[id].nwtitle;
      strncpy((char *)buf2,hist[id].title,4*hist[id].nwtitle);
      buf2 += 2*hist[id].nwtitle;

      if(packed == NORMAL_I2)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            *buf2++ = hist[id].buf[ibinx];
            hist[id].buf[ibinx] = 0;
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              *buf2++ = hist[id].buf2[ibinx][ibiny];
              hist[id].buf2[ibinx][ibiny] = 0;
            }
          }
        }
      }
      else if(packed == PACKED_I2)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            if(hist[id].buf[ibinx] > 0)
            {
              *buf2++ = ibinx;
              *buf2++ = hist[id].buf[ibinx];
              hist[id].buf[ibinx] = 0;
            }
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              if(hist[id].buf2[ibinx][ibiny] > 0)
              {
                *buf2++ = ibinx;
                *buf2++ = ibiny;
                *buf2++ = hist[id].buf2[ibinx][ibiny];
                hist[id].buf2[ibinx][ibiny] = 0;
              }
            }
          }
        }
      }

      nrow = (int)(buf2 - start2);

      len = bosNclose(jw,ind,ncol,nrow);
    }
  }
  else
  {
    if( (ind = bosNopen(jw,*((unsigned long *)"HISI"),id,ncol,nrow)) > 0)
    {
      buf = start = (unsigned long *)&jw[ind+1];

	  /* NORMAL or PACKED */
      nbins = 0;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        if(hist[id].buf[ibinx] > 0) nbins++;
      }
      packed = PACKED_I4;
      if(hist[id].nbiny == 0 && hist[id].nbinx < nbins*2) packed = NORMAL_I4;
      if(hist[id].nbiny > 0 && hist[id].nbinx < nbins*3) packed = NORMAL_I4;

      *buf++ = packed;
      *buf++ = hist[id].nbinx;
      *buf++ = hist[id].xmin;
      *buf++ = hist[id].xmax;
      *buf++ = hist[id].xunderflow;
      *buf++ = hist[id].xoverflow;
      *buf++ = hist[id].nbiny;
      *buf++ = hist[id].ymin;
      *buf++ = hist[id].ymax;
      *buf++ = hist[id].yunderflow;
      *buf++ = hist[id].yoverflow;
      *buf++ = hist[id].entries;

      *buf++ = hist[id].nwtitle;
      strncpy((char *)buf,hist[id].title,4*hist[id].nwtitle);
      buf += hist[id].nwtitle;

      if(packed == NORMAL_I4)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            *buf++ = hist[id].buf[ibinx];
            hist[id].buf[ibinx] = 0;
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            { 
              *buf++ = hist[id].buf2[ibinx][ibiny];
              hist[id].buf2[ibinx][ibiny] = 0;
            }
          }
        }
      }
      else if(packed == PACKED_I4)
      {
        if(hist[id].nbiny == 0) /* 1-dim */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            if(hist[id].buf[ibinx] > 0)
            {
              *buf++ = ibinx;
              *buf++ = hist[id].buf[ibinx];
               hist[id].buf[ibinx] = 0;
            }
          }
        }
        else                    /* 2-dim */ /* NEED CHECK !!! */
        {
          for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
          {
            for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
            {
              if(hist[id].buf2[ibinx][ibiny] > 0)
              {
                *buf++ = ibinx;
                *buf++ = ibiny;
                *buf++ = hist[id].buf2[ibinx][ibiny];
                hist[id].buf2[ibinx][ibiny] = 0;
              }
            }
          }
        }
      }

      nrow = (int)(buf - start);

      len = bosNclose(jw,ind,ncol,nrow);
    }
  }

  hist[id].entries = 0;
  hist[id].xunderflow = 0;
  hist[id].xoverflow = 0;
  hist[id].yunderflow = 0;
  hist[id].yoverflow = 0;

  if(hist[id].nbinx <= MAXI2 && hist[id].nbiny <= MAXI2 &&
     hist[id].xmin <= MAXI2 && hist[id].xmax <= MAXI2 &&
     hist[id].ymin <= MAXI2 && hist[id].ymax <= MAXI2)
  {
    hist[id].ifi2_book = 1;
    hist[id].ifi2_fill = 1;
  }
  else
  {
    hist[id].ifi2_book = 0;
    hist[id].ifi2_fill = 0;
  }

  /*logMsg("ncol=%d nrow=%d len=%d\n",ncol,nrow,len,0,0,0);*/

  return(len);
}


#else

/*
 !!! THE HISTOGRAM WILL BE CLEANED UP !!!
 !!! TITLE CAN BE LOST IF READING ON ANOTHER PLATFORM !!! 

  HISF bank format:

    bank number = histogram id
    bank format : 'F'
    bank contents:

    jw[ 0] - histogram format (3-normal, 13-"bin-contents")
    jw[ 1] - nbinx
    jw[ 2] - xmin
    jw[ 3] - xmax
    jw[ 4] - xunderflow
    jw[ 5] - xoverflow
    jw[ 6] - nbiny
    jw[ 7] - ymin
    jw[ 8] - ymax
    jw[ 9] - yunderflow
    jw[10] - yoverflow
    jw[11] - entries

    jw[12] - title length (words)
    jw[13] - jw[xx] - title

    jw[xx+1] - jw[yy] - data

 */

int
uth2bos(hstr *hist, int id, long *jw)
{
  int ibinx, ibiny, ind, len, ncol, nrow, nbins, packed;
  float *buf;

  /*printf("hist=0x%08x\n",hist);*/

  if(id <=0 || id >=NHIST)
  {
    printf("uth2bos: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  len = 0;
  ncol = 1;
  nrow = hist[id].nbinx;
  if(hist[id].nbiny > 0) nrow = nrow * hist[id].nbiny;
  nrow += (13 + hist[id].nwtitle);

  /*printf("0 %d %d %d\n",id,ncol,nrow);*/

  if( (ind = etNcreate(jw,"HISF",id,ncol,nrow)) > 0)
  {
    float f[12];
    buf = (float *)&jw[ind];

	/*printf("1\n");*/
    *buf++ = f[0] = (float)NORMAL_F;
    *buf++ = f[1] = (float)hist[id].nbinx;
    *buf++ = f[2] =        hist[id].xmin;
    *buf++ = f[3] =        hist[id].xmax;
    *buf++ = f[4] =        hist[id].xunderflow;
    *buf++ = f[5] =        hist[id].xoverflow;
    *buf++ = f[6] = (float)hist[id].nbiny;
    *buf++ = f[7] =        hist[id].ymin;
    *buf++ = f[8] =        hist[id].ymax;
    *buf++ = f[9] =        hist[id].yunderflow;
    *buf++ = f[10]=        hist[id].yoverflow;
    *buf++ = f[11]= (float)hist[id].entries;
    /*printf("uth2bos: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
    f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);*/

	/*printf("2\n");*/
    *buf++ = (float)hist[id].nwtitle;
    strncpy((char *)buf,hist[id].title,4*hist[id].nwtitle);
    buf += hist[id].nwtitle;
	/*printf("3\n");*/

    if(hist[id].nbiny == 0) /* 1-dim */
    {
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        *buf++ = hist[id].buf[ibinx];
        hist[id].buf[ibinx] = 0.0;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
        { 
          *buf++ = hist[id].buf2[ibinx][ibiny];
          hist[id].buf2[ibinx][ibiny] = 0.0;
        }
      }
    }
  }

  /*printf("uth2bos: id=%d ncol=%d nrow=%d entries=%d\n",
  id,ncol,nrow,hist[id].entries);*/
  hist[id].entries = 0;
  hist[id].xunderflow = 0.0;
  hist[id].xoverflow = 0.0;

  return(len);
}


/* restore histograms from BOS banks; see also CMgetHISF() in root */
/* NEED CHECK !!! */

int
utbos2h(hstr *hist, int id, long *jw)
{
  int i, ind, nw2, nch, ibinx, ibiny, packed;
  float *fbuf, *fstop, f[12];

  if(id <=0 || id >=NHIST)
  {
    printf("uth2bos: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  if((ind=etNlink(jw,"HISF",id)) <= 0)
  {
    /*printf("uth2bos: HISF [%d] not found\n",id);*/
    return(0);
  }
  /*printf("uth2bos: HISF [%d] found\n",id);*/

  /*printf("uth2bos: HISF [%d] 1\n",id);fflush(stdout);*/
  if(hist[id].nbinx > 0 || hist[id].nbiny > 0)
  {
    /*printf("uth2bos: WARN id=%d already exist -  replace it\n",id);*/
    free(hist[id].title);
    if(hist[id].nbiny == 0)
    {
      free(hist[id].buf);
    }
    else
    {
      for(i=0; i<hist[id].nbinx; i++) free(hist[id].buf2[i]);
      free(hist[id].buf2);
    }
    hist[id].nbinx = 0;
  }

  nw2 = etNcol(jw,ind)*etNrow(jw,ind);
  /*printf("uth2bos: nw2=%d (ncol=%d nrow=%d)\n",
    nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);*/

  fbuf = (float *)&jw[ind];
  fstop = fbuf + nw2 - 1;

  packed              = f[0] = *fbuf++;
  hist[id].nbinx      = f[1] = *fbuf++;
  hist[id].xmin       = f[2] = *fbuf++;
  hist[id].xmax       = f[3] = *fbuf++;
  hist[id].xunderflow = f[4] = *fbuf++;
  hist[id].xoverflow  = f[5] = *fbuf++;
  hist[id].nbiny      = f[6] = *fbuf++;
  hist[id].ymin       = f[7] = *fbuf++;
  hist[id].ymax       = f[8] = *fbuf++;
  hist[id].yunderflow = f[9] = *fbuf++;
  hist[id].yoverflow  = f[10]= *fbuf++;
  hist[id].entries    = f[11]= *fbuf++;

  /*printf("uth2bos: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
    f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);*/

  hist[id].nwtitle = *fbuf++;
  nch = 4*hist[id].nwtitle + 1;
  hist[id].title = (char *) malloc(nch);
  strncpy(hist[id].title, (char *)fbuf, nch-1);
  hist[id].title[nch-1] = '\0';
  fbuf += hist[id].nwtitle;

  hist[id].dx = (hist[id].xmax - hist[id].xmin)/(float)hist[id].nbinx;

  if(packed == NORMAL_F)
  {
    if(hist[id].nbiny == 0) /* 1-dim */
    {
      hist[id].buf = (float *) calloc(hist[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        hist[id].buf[ibinx] = *fbuf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      hist[id].dy = (hist[id].ymax - hist[id].ymin)/(float)hist[id].nbiny;

      hist[id].buf2 = (float **) calloc(hist[id].nbinx,sizeof(int));
      for(i=0; i<hist[id].nbinx; i++)
        hist[id].buf2[i] = (float *) calloc(hist[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
        {
          hist[id].buf2[ibinx][ibiny] = *fbuf++;
        }
      }
    }
  }
  else
  {
    printf("uth2bos: PACKED format for HISF is not supported yet\n");
    return(0);
  }

  return(id);
}

#endif
