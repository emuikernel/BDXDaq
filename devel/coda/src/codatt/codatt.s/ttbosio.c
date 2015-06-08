

/* BOS system for VXWORKS */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coda.h"
#include "ttbosio.h"

/********* ddl ***********/

#ifdef VXWORKS

#include <semLib.h>
SEM_ID ddlSem;

static int nddl;
/*static TTDDL ddl[NTTDDL];*/
static TTDDL *ddl;
#else
static int nddl;
static TTDDL ddl[NTTDDL]; /* actual index starts 1 !!! */
#endif

void
clonbanks()
{
  FILE *f;
  int i;
  char str[STRLENGTH], key[STRLENGTH];
  char fn[] = CLON_PARMS"/clonbanks.ddl";

  printf("clonbanks reached\n"); fflush(stdout);

  if( (f=fopen(fn,"r")) == NULL )
  {
    printf("clonbanks: Can't open file >%s<\n",fn); fflush(stdout);
    exit(1);
  }
  else
  {
    printf("clonbanks: use file >%s<\n",fn); fflush(stdout);
  }

#ifdef VXWORKS
  /* allocate 'ddl' structure */
  if( ( ddl = (TTDDL *) malloc(NTTDDL*sizeof(TTDDL)) ) == NULL)
  {
    printf("clonbanks: Can't allocate memory for 'ddl'\n"); fflush(stdout);
    exit(1);    
  }

  ddlSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
#endif

  printf("adr1(nddl)=0x%08x, int: %d bytes, long: %d bytes\n",
         &nddl,sizeof(int),sizeof(long)); fflush(stdout);
  nddl = 1;
  while(fgets(str,STRLENGTH,f))
  {
    sscanf(str,"%s",key);
    if(key[0] != '!')
    {
      if(!strncmp(key,"TABLE",5))
      {
        ddl[nddl].ncol = 0;
        sscanf(str,"%s %s %s",key,ddl[nddl].name,ddl[nddl].fmt);
        ddl[nddl].nname = strlen(ddl[nddl].name);
        ddl[nddl].nfmt = strlen(ddl[nddl].fmt);

		/*printf("=>%s<=>%s<=>%s<=\n",key,ddl[nddl].name,ddl[nddl].fmt);*/
        if(!strncmp(ddl[nddl].fmt,"B8",2)) ddl[nddl].lfmt = 4;
        else if(!strncmp(ddl[nddl].fmt,"B08",3)) ddl[nddl].lfmt = 4;
        else if(!strncmp(ddl[nddl].fmt,"B16",3)) ddl[nddl].lfmt = 2;
        else if(!strncmp(ddl[nddl].fmt,"B32",3)) ddl[nddl].lfmt = 1;
        else if(!strncmp(ddl[nddl].fmt,"A",1)) ddl[nddl].lfmt = 1;
        else /* set format 'I' if not described */
        {
          ddl[nddl].nfmt = 1;
          ddl[nddl].fmt[0] = 'I';
          ddl[nddl].fmt[1] = '\0';
          ddl[nddl].lfmt = 1;
        }

        while(fgets(str,STRLENGTH,f))
        {
          sscanf(str,"%s",key);
          if(!strncmp(key,"END",3)) break;
          if(key[0] != '!')
          {
            ddl[nddl].ncol ++;
            if(ddl[nddl].ncol != atoi(key)) /* to make sure !!! */
            {
              printf("clonbanks: syncronization error: %d != %d\n",ddl[nddl].ncol,atoi(key));
              printf("clonbanks: in string >%s<\n",key);
              printf("clonbanks: exit\n");
              exit(0);
            }
          }
        }
        nddl ++;
      }
    }
  }

  {
    int i;
    printf(" N   name (nname)     fmt (nfmt)     ncol\n");
    for(i=1; i<nddl; i++) printf("[%2d]   >%s< (%d)         >%s< (%d)     %d\n",
              i,ddl[i].name,ddl[i].nname,ddl[i].fmt,ddl[i].nfmt,ddl[i].ncol);
  }

  fclose(f);
  return;
}

/********* ddl ***********/

int
bosMgetid(char *name)
{
  int i, j, diff, len, lenn, nddl1;
  unsigned int *ptr;
  char *ch;

  /* count the number of signs removing trailing spaces */

  /*lenn = strlen(name);*/
  lenn=0;
  ch = name;
  if(ch[0] == '\0') lenn = 0; 
  else if(ch[1] == '\0') lenn = 1; 
  else if(ch[2] == '\0') lenn = 2; 
  else if(ch[3] == '\0') lenn = 3; 
  else if(ch[4] == '\0') lenn = 4;
 
  if(lenn > 4 || lenn <= 1)
  {
    printf("bosMgetid: ERROR: lenn=%d\n",lenn);
    ptr = (unsigned int *)name;
    printf("???: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
      ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
    lenn=4;
  }
  len = lenn;
  while(name[len-1]==' ') len--;

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  nddl1 = nddl;
  for(i=1; i<nddl1; i++)
  {
    /*printf("bosMgetid: name >%4.4s< len=%d\n",name,len);*/

	/*
    if(len == ddl[i].nname)
      if( !strncmp(name,ddl[i].name,len) ) return(i);
	*/
    if(len == ddl[i].nname)
    {
      diff = 0;
      for(j=0; j<len; j++)
      {
        if(name[j] != ddl[i].name[j])
        {
          diff = 1;
          break;
	    }
	  }
      if(diff == 0)
      {
#ifdef VXWORKS
        semGive(ddlSem); 
#endif
        return(i);
      }
    }



  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  printf("bosMgetid: ERROR: name >%4.4s< does not described, update clonbanks.ddl file !!!\n",name);
  printf("bosMgetid: lenn=%d, len=%d, nddl1=%d\n",lenn,len,nddl1); 
  printf("bosMgetid: name=>%s<\n",name);
  /*printf("bosMgetid: strlen(name)=%d\n",strlen(name));*/

printf("try again ..\n");

  lenn=0;
  ch = name;
  if(ch[0] == '\0') lenn = 0;
  else if(ch[1] == '\0') lenn = 1; 
  else if(ch[2] == '\0') lenn = 2; 
  else if(ch[3] == '\0') lenn = 3; 
  else if(ch[4] == '\0') lenn = 4; 
  if(lenn > 4 || lenn <= 1)
  {
    printf("bosMgetid: ERROR: lenn=%d\n",lenn);
    ptr = (unsigned int *)name;
    printf("!!!: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
      ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
    lenn=4;
  }
  len = lenn;
  while(name[len-1]==' ') len--;
  printf("bosMgetid: lenn=%d, len=%d\n",lenn,len); 
  for(i=1; i<nddl; i++)
  {
    /*printf("bosMgetid: name >%4.4s< len=%d\n",name,len);*/
    if(len == ddl[i].nname)
    {
      diff = 0;
      for(j=0; j<len; j++)
      {
        if(name[j] != ddl[i].name[j])
        {
          diff = 1;
          break;
	    }
	  }
      if(diff == 0) return(i);
    }
  }

  printf("bosMgetid: ERROR: name >%4.4s< does not described, update clonbanks.ddl file !!!\n",name);
  printf("bosMgetid: lenn=%d, len=%d, nddl1=%d\n",lenn,len,nddl1); 
  printf("bosMgetid: name=>%s<\n",name); 

  printf("no way !!\n");

  return(-99);
}


/* returns bank name */

int
bosMgetname(int id, char name[9])
{
  int i, len;

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  if(id > 0 && id < nddl)
  {
    for(i=0; i<8; i++) name[i] = ' ';
    len = ddl[id].nname;
    if(len > 8)
    {
      printf("bosMgetname: ERROR: ddl[%d].nname = %d\n",id,ddl[id].nname);
      len = 8;
    }
    strncpy(name,ddl[id].name,len);
    name[len] = '\0';
  }
  else
  {
    printf("bosMgetname: ERROR: id = %d (must be between 1 and %d)\n",id,nddl-1);
#ifdef VXWORKS
    semGive(ddlSem); 
#endif
    return(-1);
  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  return(0);
}

/* returns the number of columns in bank 'name' */

int
bosMgetncol(char *name)
{
  int id, ncol;

  id = bosMgetid(name);
  if(id < 0) {printf("1: bosMgetid returns %d\n",id); exit(0);}

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  ncol = ddl[id].ncol;
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  return(ncol);
}

/* returns the format code 'lfmt' in bank 'name' */

int
bosMgetlfmt(char *name)
{
  int id, lfmt;

  id = bosMgetid(name);
  if(id < 0) {printf("2: bosMgetid returns %d\n",id); exit(0);}

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  lfmt = ddl[id].lfmt;
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  return(lfmt);
}

/* sending message bank to data stream */

int
bosMmsg(int *jw, char *bankname, int banknum, char *message)
{
  int i, ind, nch, len, nwords;
  char *ch;

  /* count the number of words in the message */
  nch = strlen(message);
  nwords = (nch+3)/4;

  /* create message bank */
  if((ind = bosMopen(jw,bankname,banknum,1,nwords)) <= 0)
  {
    return(0);
  }
  else
  {
    len = NHEAD;
    ch = (char *)&jw[ind+1];
    strcpy(ch,message);
    for(i=nch; i<nwords*4; i++) ch[i] = ' ';
    bosMclose(jw,ind,1,nwords);
    len += nwords;

    return(len);
  }

}


/*

   COMPRESSED FORMAT : 'jw' have to be set to second word of CODA header; that
   word containes buffer length, which will be used as pointer to last opened bank;
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
tttest(char *str)
{
  printf("%s",str);
  printf(" adr(nddl)=%x NWBOS=%d\n",&nddl,NWBOS);

  return;
}


/* init Mini-BOS array */

void
bosMinit(int *jw)
{
  jw[ILEN] = 1;

  return;
}


/*
 link bank (compressed format)
*/

int
bosMlink(int *jw, char *name, int nr)
{
  int id, ind, tst, ncol, fmtlen;

  id = bosMgetid(name);
  if(id < 0) {printf("bosMlink: ERROR: bosMgetid returns %d\n",id); exit(0);}
  tst = (id<<16) + nr;

  ind = IDNR;
  if(jw[ind]<=0) return(0); /* ... */

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  while(ind < jw[ILEN])
  {
    if(jw[ind] <= 0)
	{
      printf("bosMlink: ERROR: nrow=%d (ind=%d name>%4.4s< nr=%d id=%d)\n",
        jw[ind],ind,name,nr,id);
#ifdef VXWORKS
      semGive(ddlSem); 
#endif
      return(0);
    }
	/*printf("loop1: ind=%d jw[ILEN]=%d\n",ind,jw[ILEN]);fflush(stdout);*/
    /* check if we found bank we are looking for */
    if(jw[ind-IDNR] == tst)
    {
#ifdef VXWORKS
      semGive(ddlSem); 
#endif
      return(ind);
    }

    /* get current bank parameters and bump index */
    id = (jw[ind-IDNR] >> 16) & 0xFFFF;
    ncol = ddl[id].ncol;
	/*printf("id=%d ncol=%d nrow=%d\n",id,ncol,jw[ind]);fflush(stdout);
	  printf("lfmt=%d\n",ddl[id].lfmt);*/
    fmtlen = 4 / ddl[id].lfmt;
    ind += ( NHEAD + (jw[ind] * ncol * fmtlen + 3) / 4) ;
	/*printf("loop2: ind=%d jw[ILEN]=%d\n",ind,jw[ILEN]);fflush(stdout);*/
  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  return(0);  
}


/*
 open bank (compressed format)
*/

int
bosMopen(int *jw, char *name, int nr, int ncol, int nrow)
{
  int id, ind, fmtlen;

  id = bosMgetid(name);
  if(id < 0) {printf("4: bosMgetid returns %d\n",id); exit(0);}
#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  fmtlen = 4 / ddl[id].lfmt;
  if(ncol != ddl[id].ncol)
  {
    /*printf("ttbosio.c: wrong ncol=%d, use default value %d\n",ncol,ddl[id].ncol);*/
    ncol = ddl[id].ncol;
  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  /* if space not enough - do not create a bank but return zero */

  if(jw[ILEN] + NHEAD + ((ncol * nrow * fmtlen + 3) / 4) >= NWBOS) return(0);

  /* shift bank pointer to next bank */

  ind = jw[ILEN];

  /* fill bank header */

  jw[ind-IDNR]  = (id << 16) + nr;
  jw[ind-IDATA] = nrow;

  /* update array length */

  jw[ILEN] += (NHEAD + ((ncol * nrow * fmtlen + 3) / 4));
  /*printf("bosMopen: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  return(ind);
}


/*
 close bank (compressed format)
 if nrow = 0 - remove header, which was created by bosMopen()
*/

int
bosMclose(int *jw, int ind, int ncol, int nrow)
{
  int id, fmtlen;

  id = (jw[ind-IDNR] >> 16) & 0xffff;
#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  fmtlen = 4 / ddl[id].lfmt;

  if(ncol != ddl[id].ncol)
  {
    /*printf("ttbosio.c: wrong ncol=%d, use default value %d\n",ncol,ddl[id].ncol);*/
    ncol = ddl[id].ncol;
  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  /* check if it is the last bank */

  if(ind != (jw[ILEN] - (NHEAD + ((ncol * jw[ind-IDATA] * fmtlen + 3) / 4)) ))
  {
    printf("bosMclose: ERROR: Only last bank could be closed !!!\n");
    exit(0);
  }

  /* decrease jw[ILEN] on an amount of words set by bosMopen() */
  /* because of that amount will be recalculated here */
  jw[ILEN] -= ((ncol * jw[ind-IDATA] * fmtlen + 3) / 4);
  /*printf("bosMopen: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  /* calculate new data length */

  jw[ind-IDATA] = nrow;

  /* if space not enough or zero bank length - skip bank and return zero */

  if(ind + ((ncol * nrow * fmtlen + 3) / 4) >= NWBOS || jw[ind-IDATA] == 0)
  {
    jw[ind-IDATA] = - NHEAD;
  }

  /* update array length */

  jw[ILEN] += ((ncol * nrow * fmtlen + 3) / 4); /* + NHEAD already done !!! */
  /*printf("bosMclose: ILEN=%d jw[ILEN]=%d\n",ILEN,jw[ILEN]);*/

  return(NHEAD + ((ncol * nrow * fmtlen + 3) / 4));
}


/* OBSOLETE, SEE RELEASE 1.4 */


/*
compress bos array - reduces BOS headers from 10 to 2 words;
bos array becomes uncompartible with BOS functions untill
bosUncomp() will be called

     jw - address of BOS array
     buf - address to put compressed BOS array ( could be equal jw !!!)
     len - length of BOS array
*/

int
bosCompArray(int *jw, int *buf, int len)
{
  char *name;
  int *in, *out, *end, nw, i, notfound;

  return(out-buf);
}

/*
uncompress bos array; return length of out array in long words
*/

int
bosUncompArray(int *inn, int len, int *outt)
{
  int *in, *out, *end;
  int i, nw, ncol, iddl;

  return(in - inn);
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* init BOS array */
void
bosinit(int *jw, int ndim)
{
  /* set bank pointer to the index where data begins */
  jw[ILEN] = NHEAD - 1 /*INXT*/; /* ILEN=-2, IDATA=0, NHEAD=2 */

  /* data length (-NHEAD) for begining */
  jw[jw[ILEN]-IDATA] = - NHEAD;

  return;
}

/* returns the number of words left */
int
bosleftspace(int *jw)
{
  int id_old, ind_old, nwords_old;

  /* get old 'ind' */
  ind_old = jw[ILEN];

  /* calculate the number of words in current bank */
  if(jw[ind_old-IDATA] > 0)
  {
    id_old = (jw[ind_old-IDNR]>>16)&0xFFFF; /* get old 'id' */
#ifdef VXWORKS
    semTake(ddlSem, WAIT_FOREVER);
#endif
    nwords_old = (ddl[id_old].ncol*jw[ind_old-IDATA]*(4/ddl[id_old].lfmt)+3)/4;
#ifdef VXWORKS
    semGive(ddlSem); 
#endif
  }
  else
  {
    nwords_old = jw[ind_old-IDATA];
  }
  /*printf("bosleftspace returns %d\n",NWBOS - (jw[ILEN] + nwords_old + 1));*/

  return( NWBOS - (jw[ILEN] + nwords_old + 1) );
}

/* open bank */
int
bosNopen(int *jw, int nama, int nr, int ncol, int nrow)
{
  int i, id, ind, fmtlen, ind_new, itmp, ind_old, id_old, nwords_old;
  char *ch, name[5] = {' ',' ',' ',' ','\0'};

  if(nrow < 0)
  {
    printf("bosNopen: nrow = %d\n",nrow);
    return(-2);
  }

  ch = (char *)&nama;
  for(i=0; i<4; i++) name[i] = ch[i];
  id = bosMgetid(name);
  if(id < 0)
  {
    printf("bosNopen: bosMgetid returns %d\n",id);
    printf("bosNopen: nama was 0x%08x\n",nama);
    printf("bosNopen: name was >%s<\n",name);
    return(-99);
  }

#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  if(ddl[id].lfmt <= 0) /* observed with MVME5500 without semTake .. */
  {
    printf("ERROR: bosNopen: id=%d, ddl[id].lfmt=%d\n",id,ddl[id].lfmt);
    exit(0);
  }
  fmtlen = 4 / ddl[id].lfmt;

  /* if 'ncol' is different from table value, use table value */
  if(ncol != ddl[id].ncol)
  {
    printf("bosNopen: ncol was %d, replaced by %d\n",ncol,ddl[id].ncol);
    ncol = ddl[id].ncol;
  }

  /* get old 'ind' and calculate the number of words in previous bank */
  ind_old = jw[ILEN];
  if(jw[ind_old-IDATA] > 0)
  {
    id_old = (jw[ind_old-IDNR]>>16)&0xFFFF; /* get old 'id' */
    nwords_old = (ddl[id_old].ncol*jw[ind_old-IDATA]*(4/ddl[id_old].lfmt)+3)/4;
  }
  else if(jw[ind_old-IDATA] == 0)
  {
    /*nwords_old = jw[ind_old-IDATA];*/
    nwords_old = -NHEAD; /* discard previous bank */
    /*printf("bosNopen: discard previous bank\n");*/
  }
  else
  {
    nwords_old = jw[ind_old-IDATA];
    if(nwords_old != -NHEAD) printf("bosNopen: nwords_old=%d\n",nwords_old);
  }
#ifdef VXWORKS
  semGive(ddlSem); 
#endif

  /* calculate new 'ind' based on information from previous bosNclose() */
  ind_new = ind_old + nwords_old + NHEAD;

  /* if space not enough - do not create a bank but return zero */
  if((itmp = ind_new + ((ncol*nrow*fmtlen+3)/4)) >= NWBOS)
  {
    printf("bosNopen: space not enough: (%d = %d + %d + %d + %d) >= %d\n",
      itmp,ind_old, nwords_old, NHEAD, ncol*nrow/fmtlen, NWBOS);
    return(-NWBOS);
  }

  /* shift bank pointer to next bank */
  if(ind_new <=0)
  {
    printf("bosNopen: ERROR: ind = %d !!! (%d %d %d)\n",
      ind,ind_old,nwords_old,NHEAD);
  }

  ind = jw[ILEN] = ind_new;
  /*printf("bosNopen: ind =%d nr=%d\n",ind,nr);*/

  /* fill bank header (2 words: compressed format) */
  jw[ind-IDATA]  = nrow;
  jw[ind-IDNR] = ((id<<16) + nr);
  /*printf("bosNopen: header: 0x%08x (%d), 0x%08x (%d)\n",
    jw[ind-IDNR],jw[ind-IDNR],jw[ind-IDATA],jw[ind-IDATA]);*/

  return(ind);
}

/* close bank */
int
bosNclose(int *jw, int ind, int ncol, int nrow)
{
  int fmtlen, i, id, nwords;

  if(nrow < 0)
  {
    printf("bosNclose: nrow = %d\n",nrow);
    return(-2);
  }

  id = (jw[ind-IDNR]>>16)&0xFFFF;
  if(id < 0) {printf("6: bosMgetid returns %d\n",id); exit(0);}
#ifdef VXWORKS
  semTake(ddlSem, WAIT_FOREVER);
#endif
  fmtlen = 4 / ddl[id].lfmt;

  /* if 'ncol' is different from table value, use table value */
  if(ncol != ddl[id].ncol) ncol = ddl[id].ncol;
#ifdef VXWORKS
  semGive(ddlSem); 
#endif
  
  /* check if it the last bank */
  if(ind != jw[ILEN])
  {
    printf("bosNclose: Only last bank could be closed !!!\n");
    return(-1);
  }

  /* adjust length for last bank */
  jw[ind-IDATA] = nrow;
  nwords = (ncol * nrow * fmtlen + 3) / 4;
  /*printf("nwords=%d\n",nwords);*/

  /* if space not enough or zero length bank - skip bank and return zero */
  if(ind + nwords >= NWBOS || nwords == 0) nwords = - NHEAD;
  /*
  if(jw[ind-IDATA]>0)
    printf("bosNclose: returns %d (0x%08x 0x%08x)\n",nwords+NHEAD,
      jw[ind-IDNR],jw[ind-IDATA]);
  */

  return(nwords + NHEAD);
}




