

/* BOS system for VXWORKS */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "ttbosio.h"

/********* ddl ***********/

#ifdef VXWORKS
volatile static int nddl_;
volatile static TTDDL ddl_[NTTDDL]; /* actual index starts 1 !!! */
#else
static int nddl_;
static TTDDL ddl_[NTTDDL]; /* actual index starts 1 !!! */
#endif

void
clonbanks_()
{
  FILE *f;
  int i;
  char str[STRLENGTH], key[STRLENGTH];
  char fn[] = CLON_PARMS"/clonbanks.ddl";

  if( (f=fopen(fn,"r")) == NULL )
  {
    printf("clonbanks: Can't open file >%s<\n",fn);
    exit(1);
  }

printf("adr1(nddl_)=0x%08x\n",&nddl_);
  nddl_ = 1;
  while(fgets(str,STRLENGTH,f))
  {
    sscanf(str,"%s",key);
    if(key[0] != '!')
    {
      if(!strncmp(key,"TABLE",5))
      {
        ddl_[nddl_].ncol = 0;
        sscanf(str,"%s %s %s",key,ddl_[nddl_].name,ddl_[nddl_].fmt);
        ddl_[nddl_].nname = strlen(ddl_[nddl_].name);
        ddl_[nddl_].nfmt = strlen(ddl_[nddl_].fmt);

		/*printf("=>%s<=>%s<=>%s<=\n",key,ddl[nddl].name,ddl[nddl].fmt);*/
        if(!strncmp(ddl_[nddl_].fmt,"B8",2)) ddl_[nddl_].lfmt = 4;
        else if(!strncmp(ddl_[nddl_].fmt,"B08",3)) ddl_[nddl_].lfmt = 4;
        else if(!strncmp(ddl_[nddl_].fmt,"B16",3)) ddl_[nddl_].lfmt = 2;
        else if(!strncmp(ddl_[nddl_].fmt,"B32",3)) ddl_[nddl_].lfmt = 1;
        else if(!strncmp(ddl_[nddl_].fmt,"A",1)) ddl_[nddl_].lfmt = 1;
        else /* set format 'I' if not described */
        {
          ddl_[nddl_].nfmt = 1;
          ddl_[nddl_].fmt[0] = 'I';
          ddl_[nddl_].fmt[1] = '\0';
          ddl_[nddl_].lfmt = 1;
        }

        while(fgets(str,STRLENGTH,f))
        {
          sscanf(str,"%s",key);
          if(!strncmp(key,"END",3)) break;
          if(key[0] != '!')
          {
            ddl_[nddl_].ncol ++;
            if(ddl_[nddl_].ncol != atoi(key)) /* to make sure !!! */
            {
              printf("clonbanks: syncronization error: %d != %d\n",ddl_[nddl_].ncol,atoi(key));
              printf("clonbanks: in string >%s<\n",key);
              printf("clonbanks: exit\n");
              exit(0);
            }
          }
        }
        nddl_ ++;
      }
    }
  }

  {
    int i;
    printf(" N   name (nname)     fmt (nfmt)     ncol\n");
    for(i=1; i<nddl_; i++) printf("[%2d]   >%s< (%d)         >%s< (%d)     %d\n",
              i,ddl_[i].name,ddl_[i].nname,ddl_[i].fmt,ddl_[i].nfmt,ddl_[i].ncol);
  }

  fclose(f);
  return;
}
/********* ddl ***********/


/* returns iddl index of bank 'name' in ddl[] array */

int
bosMgetid_(char *name)
{
  int i, len, lenn;

  /* count the number of signs removing trailing spaces */
  lenn = strlen(name);
  if(lenn > 4) {printf("bosMgetid_: ERROR: lenn=%d\n",lenn);lenn=4;}
  len = lenn;
  while(name[len-1]==' ') len--;

  for(i=1; i<nddl_; i++)
  {
    /*printf("bosMgetid_: name >%4.4s< len=%d\n",name,len);*/
    if(len == ddl_[i].nname)
      if( !strncmp(name,ddl_[i].name,len) ) return(i);
  }
 
  printf("bosMgetid_: ERROR: name >%4.4s< does not described, update clonbanks.ddl file !!!\n",name);
  printf("bosMgetid_: lenn=%d, len=%d\n",lenn,len); 
  printf("bosMgetid_: name=>%s<\n",name); 
  printf("bosMgetid_: strlen(name)=%d\n",strlen(name)); 

printf("try again ..\n");

  lenn = strlen(name);
  if(lenn > 4) {printf("bosMgetid_: ERROR: lenn=%d\n",lenn);lenn=4;}
  len = lenn;
  while(name[len-1]==' ') len--;
  printf("bosMgetid_: lenn=%d, len=%d\n",lenn,len); 
  for(i=1; i<nddl_; i++)
  {
    /*printf("bosMgetid_: name >%4.4s< len=%d\n",name,len);*/
    if(len == ddl_[i].nname)
      if( !strncmp(name,ddl_[i].name,len) ) return(i);
  }

  return(-99);
}

int
bosMgetname_(int id, char name[9])
{
  int i;

  if(id > 0 && id < nddl_)
  {
    for(i=0; i<8; i++) name[i] = ' ';
    strncpy(name,ddl_[id].name,ddl_[id].nname);
    name[8] = '\0';
  }
  else
  {
    return(-1);
  }

  return(0);
}

/* returns the number of columns in bank 'name' */

int
bosMgetncol_(char *name)
{
  int id;

  id = bosMgetid_(name);
  if(id < 0) {printf("1: bosMgetid_ returns %d\n",id); exit(0);}

  return(ddl_[id].ncol);
}

/* returns the format code 'lfmt' in bank 'name' */

int
bosMgetlfmt_(char *name)
{
  int id;

  id = bosMgetid_(name);
  if(id < 0) {printf("2: bosMgetid_ returns %d\n",id); exit(0);}

  return(ddl_[id].lfmt);
}

/* sending message bank to data stream */

int
bosMmsg_(int *jw, char *bankname, int banknum, char *message)
{
  int i, ind, nch, len, nwords;
  char *ch;

  /* count the number of words in the message */
  nch = strlen(message);
  nwords = (nch+3)/4;

  /* create message bank */
  if((ind = bosMopen_(jw,bankname,banknum,1,nwords)) <= 0)
  {
    printf("bosMmsg: ERROR 1\n");
    return(0);
  }
  else
  {
    len = NHEAD;
    ch = (char *)&jw[ind+1];
    strcpy(ch,message);
    for(i=nch; i<nwords*4; i++) ch[i] = ' ';
    bosMclose_(jw,ind,1,nwords);
    len += nwords;

    return(len);
  }

}


/*

   COMPRESSED FORMAT : 'jw' have to be set to second word of CODA header; that
   work containes buffer length, which will be used as pointer to last opened bank;
   parameter NWBOS has to be defined at compiling step ( -DNWBOS=4096 for example )

   BOS array looks like that:

         exclusive_length
           coda word
         id     |     nr       <- jw
              nrow
         ...............
         ...............
              data
         ...............
         ...............

*/

/* to get NWBOS 
#ifndef VXWORKS
#include <pthread.h>
#endif
#include "circbuf.h"
*/

void
tttest_(char *str)
{
  printf("%s",str);
  printf(" adr(nddl_)=%x NWBOS=%d\n",&nddl_,NWBOS);

  return;
}


/* init Mini-BOS array */

void
bosMinit_(int *jw)
{
  jw[ILEN] = 1;

  return;
}


/*
 link bank (compressed format)
*/

int
bosMlink_(int *jw, char *name, int nr)
{
  int id, ind, tst, ncol, fmtlen;

  id = bosMgetid_(name);
  if(id < 0) {printf("bosMlink_: ERROR: bosMgetid_ returns %d\n",id); exit(0);}
  tst = (id<<16) + nr;

  ind = IDNR;
  if(jw[ind]<=0) return(0); /* ... */

  while(ind < jw[ILEN])
  {
    if(jw[ind] <= 0)
	{
      printf("bosMlink: ERROR: nrow=%d (ind=%d name>%4.4s< nr=%d id=%d)\n",
        jw[ind],ind,name,nr,id);
      return(0);
    }
	/*printf("loop1: ind=%d jw[ILEN]=%d\n",ind,jw[ILEN]);fflush(stdout);*/
    /* check if we found bank we are looking for */
    if(jw[ind-IDNR] == tst) return(ind);

    /* get current bank parameters and bump index */
    id = (jw[ind-IDNR] >> 16) & 0xFFFF;
    ncol = ddl_[id].ncol;
	/*printf("id=%d ncol=%d nrow=%d\n",id,ncol,jw[ind]);fflush(stdout);
	  printf("lfmt=%d\n",ddl[id].lfmt);*/
    fmtlen = 4 / ddl_[id].lfmt;
    ind += ( NHEAD + (jw[ind] * ncol * fmtlen + 3) / 4) ;
	/*printf("loop2: ind=%d jw[ILEN]=%d\n",ind,jw[ILEN]);fflush(stdout);*/
  }

  return(0);  
}


/*
 open bank (compressed format)
*/

int
bosMopen_(int *jw, char *name, int nr, int ncol, int nrow)
{
  int id, ind, fmtlen;

  /*printf("bosMopen_: name=%4.4s nr=%d ncol=%d nrow=%d\n",name,nr,ncol,nrow);*/
  id = bosMgetid_(name);
  if(id < 0) {printf("4: bosMgetid_ returns %d\n",id); exit(0);}
  fmtlen = 4 / ddl_[id].lfmt;
  if(ncol != ddl_[id].ncol)
  {
    /*printf("ttbosio.c: wrong ncol=%d, use default value %d\n",ncol,ddl[id].ncol);*/
    ncol = ddl_[id].ncol;
  }

  /* if space not enough - do not create a bank but return zero */

  if(jw[ILEN] + NHEAD + ((ncol * nrow * fmtlen + 3) / 4) >= NWBOS) return(0);

  /* shift bank pointer to next bank */

  ind = jw[ILEN];

  /* fill bank header */

  jw[ind-IDNR]  = (id << 16) + nr;
  jw[ind-IDATA] = nrow;

  /* update array length */

  jw[ILEN] += (NHEAD + ((ncol * nrow * fmtlen + 3) / 4));
  /*printf("bosMopen_: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  /*printf("bosMopen_: ind=%d (%d 0x%08x)\n",ind,jw[ind-IDATA],&jw[ind-IDATA]);*/
  return(ind);
}


/*
 close bank (compressed format)
 if nrow = 0 - remove header, which was created by bosMopen_()
*/

int
bosMclose_(int *jw, int ind, int ncol, int nrow)
{
  int id, fmtlen;

  /*printf("bosMclose_: ind=%d ncol=%d nrow=%d\n",ind,ncol,nrow);*/
  id = (jw[ind-IDNR] >> 16) & 0xffff;
  fmtlen = 4 / ddl_[id].lfmt;

  if(ncol != ddl_[id].ncol)
  {
	/*printf("bosMclose_: wrong ncol=%d, use default value %d\n",ncol,ddl_[id].ncol);*/
    ncol = ddl_[id].ncol;
  }

  /* check if it is the last bank */

  if(ind != (jw[ILEN] - (NHEAD + ((ncol * jw[ind-IDATA] * fmtlen + 3) / 4)) ))
  {
    printf("bosMclose_: ERROR: Only last bank can be closed !!!\n");
    printf("bosMclose_: ind=%d != %d (%d %d %d 0x%08x %d >0x%08x)\n",ind,
      (jw[ILEN] - (NHEAD + ((ncol * jw[ind-IDATA] * fmtlen + 3) / 4)) ),
      jw[ILEN],ncol,ind,jw[ind-IDATA],fmtlen, &jw[ind-IDATA] );
    exit(0);
  }

  /* decrease jw[ILEN] on an amount of words set by bosMopen_() */
  /* because of that amount will be recalculated here */
  jw[ILEN] -= ((ncol * jw[ind-IDATA] * fmtlen + 3) / 4);
  /*printf("bosMopen_: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  /* calculate new data length */

  jw[ind-IDATA] = nrow;

  /* if space not enough or zero bank length - skip bank and return zero */

  if(ind + ((ncol * nrow * fmtlen + 3) / 4) >= NWBOS || jw[ind-IDATA] == 0)
  {
    jw[ind-IDATA] = - NHEAD;
  }

  /* update array length */

  jw[ILEN] += ((ncol * nrow * fmtlen + 3) / 4); /* + NHEAD already done !!! */
  /*printf("bosMclose_: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  return(NHEAD + ((ncol * nrow * fmtlen + 3) / 4));
}


