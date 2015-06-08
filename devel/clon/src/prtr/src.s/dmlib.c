
#undef USE_CA

#define USE_TREE

/* dmlib.c - data monitor library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#ifdef USE_CA
#include "cadef.h"
#endif

#include "dmlib.h"
#include "bos.h"
#include "bosio.h"
#include "ttbosio.h"

#define PRINT 1

#define MAXSTRLEN   1000

#define MAX(x,y) ((x) > (y) ? (x) : (y))

static int    nsets;
static DMSET *sets;

static int maxbankid, maxbanknum, maxhighid, maxlowid, maxplace;
static DMBANKIDS *dmbankid;


#ifdef USE_CA
static unsigned  nelem = 0; /* the number of elements to be sent over CA */
static char     *caname = "test10"; /* channel name */
static chid      chan;

static int ncalls = 0;
#endif



/* print tree */
void
dmtreeprint()
{
  int i1, i2, i3, i4, itmp;

  printf("\n================ Print New Tree =================\n");
  for(i1=0; i1<maxbankid; i1++)
  {
    for(i2=0; i2<maxbanknum; i2++)
	{
      for(i3=0; i3<maxhighid; i3++)
	  {
        for(i4=0; i4<maxlowid; i4++)
	    {
          if( (itmp = dmbankid[i1].banknum[i2].highid[i3].lowid[i4].nsets) > 0)
		  {
            printf("bankid %d banknum %d highid %d lowid %d nsets %d\n",i1,i2,i3,i4,itmp);
		  }
	    }
	  }
	}
  }

  printf("\n=============================================\n");

  return;
}

/* print sets */
void
dmsetsprint()
{
  int i, j, itmp;

  printf("\n================ Print Sets =================\n");
  for(i=0; i<nsets; i++)
  {
    printf("[%3d] count %d\n",i, sets[i].count);
  }
  printf("\n=============================================\n");

  return;
}
void
dmsetsreset()
{
  int i, j, itmp;

  for(i=0; i<nsets; i++)
  {
    sets[i].count = 0;
  }

  return;
}

int
dmsetsoutput(int *nsetout, int counts[NSETS], char setnames[NSETS][NAMLEN])
{
  int nset, ret, i;

  ret = 0;
  nset = nsets;
  if(nset > NSETS)
  {
    nset = NSETS;
    ret = nsets;
  }
  *nsetout = nset;

  for(i=0; i<nsets; i++)
  {
    counts[i] = sets[i].count;
    strncpy(setnames[i],sets[i].sname,NAMLEN);
	/*
	printf("=[%1d]= >%s<\n",i,sets[i].sname);
	printf("=[%1d]= >%s<\n",i,sets[i].title);
	*/
  }

  return(ret);
}

/* read and parse config file */
int
dmparse(char *fname, int iset)
{
  FILE *fd;
  int i, j, k, m, n, mlist, firstbankline, par1[NPARAM], par2[NPARAM];
  char *ch, key[7], str[100];
  char bankname[NAMLEN];

  printf("dmparse: file >%s<\n",fname);

  /* read file counting space */
  firstbankline = 0;
  if(fd = fopen(fname,"r"))
  {
    mlist = 0;
    do
    {
      if(!fgets(str,99,fd)) break;
      if(str[0] == '#') continue; /* skip comment line */
      if(!strncmp(str,"STOP",4)) break;
	  /*
      printf("%s",str);
      printf("len=%d\n",strlen(str));
	  */

      if(!strncmp(str,"NAME",4))
      {
        ;
      }
      else if(!strncmp(str,"NORM",4))
      {
        ;
      }
      else if(!strncmp(str,"BANK",4)) /* comes in pairs */
      {
        if(firstbankline == 0) /* first line: extract starting params */
        {
          firstbankline = 1;
          for(i=0; i<NPARAM; i++) {par1[i] = 0; par2[i] = -1;}
          sscanf(&str[4],"%s %i %i %i %i",
              bankname,&par1[0],&par1[1],&par1[2],&par1[3]);
        }
        else /* second line: extrach ending params and scan */
        {
          firstbankline = 0;
          sscanf(&str[4],"%s %i %i %i %i",
              bankname,&par2[0],&par2[1],&par2[2],&par2[3]);
          for(i=par1[0]; i<=par2[0]; i++)
            for(j=par1[1]; j<=par2[1]; j++)
              for(k=par1[2]; k<=par2[2]; k++)
                for(m=par1[3]; m<=par2[3]; m++)
                  mlist ++;
        }
      }
      else if(!strncmp(str,"HIST",4))
      {
        mlist ++;
      }
      else
      {
        printf("unknown key - exit.\n"); /* ignore comments and unknown keys */
        exit(0);
      }
    } while(1 == 1);
    fclose(fd);
  }
  else
  {
    printf("dmparse: error opening config file >%s< - exit\n",fname);
    exit(0);
  }

  /* allocate space */
  sets[iset].nbanks = mlist;
  printf("--> mlist = %d (x sizeof(DMBANK)=%d)\n",sets[iset].nbanks,sizeof(DMBANK));
  sets[iset].banks = (DMBANK *) calloc(mlist,sizeof(DMBANK));

  /* read file again filling up structure */
  firstbankline = 0;
  if(fd = fopen(fname,"r"))
  {
    mlist = 0;

    /* some cleanup */
    for(i=0; i<NAMLEN; i++) sets[iset].sname[i] = ' ';
    for(i=0; i<TITLEN; i++) sets[iset].title[i] = ' ';
    for(i=0; i<NAMLEN; i++) sets[iset].snorm[i] = ' ';

    do
    {
      if(!fgets(str,99,fd)) break;
      if(str[0] == '#') continue; /* skip comment line */
      if(!strncmp(str,"STOP",4)) break;
	  /*
      printf("%s",str);
      printf("len=%d\n",strlen(str));
	  */

      if(!strncmp(str,"NAME",4))
      {
        sscanf(&str[4],"%s",sets[iset].sname);
        j = 0;
        for(i=0; i<strlen(str)-1; i++) if(str[i]=='"') {j=i+1; break;}
        if(j==0) {printf("dmparse: wrong format - exit.\n");exit(0);}
        ch = (char *) &str[j];
        j = 0;
        for(i=0; i<strlen(str)-j; i++) if(ch[i]=='"') {j=i; break;}
        if(j==0) {printf("dmparse: wrong format - exit.\n");exit(0);}
        ch[j] = '\0';
        strcpy(sets[iset].title,ch);
        printf("[%d]  sname >%s<\n",iset,sets[iset].sname);
        printf("[%d]  title >%s<\n",iset,sets[iset].title);
      }
      else if(!strncmp(str,"NORM",4))
      {
        sscanf(&str[4],"%s",sets[iset].snorm);
        printf("  snorm >%s<\n",sets[iset].snorm);
      }
      else if(!strncmp(str,"BANK",4))
      {



        if(firstbankline == 0) /* first line: extract starting params */
        {
          firstbankline = 1;
          for(i=0; i<NPARAM; i++) {par1[i] = 0; par2[i] = -1;}
          sscanf(&str[4],"%s %i %i %i %i",
              bankname,&par1[0],&par1[1],&par1[2],&par1[3]);
        }
        else /* second line: extrach ending params and scan */
        {
          firstbankline = 0;
          sscanf(&str[4],"%s %i %i %i %i",
              bankname,&par2[0],&par2[1],&par2[2],&par2[3]);
          for(i=par1[0]; i<=par2[0]; i++)
            for(j=par1[1]; j<=par2[1]; j++)
              for(k=par1[2]; k<=par2[2]; k++)
                for(m=par1[3]; m<=par2[3]; m++)
                {
                  strcpy(sets[iset].banks[mlist].banktype,"BANK");
				  strcpy(sets[iset].banks[mlist].bankname,bankname);
                  sets[iset].banks[mlist].par[0] = i;
                  sets[iset].banks[mlist].par[1] = j;
                  sets[iset].banks[mlist].par[2] = k;
                  sets[iset].banks[mlist].par[3] = m;
                  sets[iset].banks[mlist].bankid = bosMgetid(sets[iset].banks[mlist].bankname);
				  /*
                  printf("  par: %4.4s %4.4s %i %i %i %i\n",
                    sets[iset].banks[mlist].banktype,
                    sets[iset].banks[mlist].bankname,
                    sets[iset].banks[mlist].par[0],
                    sets[iset].banks[mlist].par[1],
                    sets[iset].banks[mlist].par[2],
                    sets[iset].banks[mlist].par[3]);
				  */
                  mlist ++;
                }
        }
      }
      else if(!strncmp(str,"HIST",4))
      {
        strcpy(sets[iset].banks[mlist].banktype,"HIST");
        strcpy(sets[iset].banks[mlist].bankname,"HIST");
        sscanf(&str[4],"%i %i %i",
               &sets[iset].banks[mlist].par[0],
               &sets[iset].banks[mlist].par[1],
               &sets[iset].banks[mlist].par[2]);
        printf("  par: %4.4s %4.4s %i %i %i\n",
               sets[iset].banks[mlist].banktype,
               sets[iset].banks[mlist].bankname,
               sets[iset].banks[mlist].par[0],
               sets[iset].banks[mlist].par[1],
               sets[iset].banks[mlist].par[2]);
        mlist ++;
      }
      else
      {
        printf("unknown key - exit.\n"); /* ignore comments and unknown keys */
        exit(0);
      }

    } while(1 == 1);

    fclose(fd);
  }
  else
  {
    printf("dmparse: error opening config file >%s< - exit\n",fname);
    exit(0);
  }

  return(0);
}


/*
  dmconfig() - reads dmlib configuration files (extension *.dmcfg)
     dir contains the name of the directory where files are located;
     if dir = "" files assume in $CLON_PARMS/cmon/dmlib/
 */

int
checkbankid(int id)
{
  int i, j, len, iset, mlist, itmp;

  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
        if(sets[iset].banks[mlist].bankid == id)
        {
          /*printf("checkbankid: Found bankid=%d\n",id);*/
          return(1);
		}
      }
    }
  }

  /*printf("checkbankid: Does NOT found bankid=%d\n",id);*/
  return(0);
}


int
dmconfig(char *dir)
{
  DIR *dd;
  struct dirent *de;
  int msets;
  FILE *fd;
  char *ch, *parm, *sss, tmp[MAXSTRLEN], dname[MAXSTRLEN], fname[MAXSTRLEN];
  char key[7], str[100];
  int i, j, len, iset, mlist, itmp;
  int bankid, banknum, highid, lowid, place, index;
  int i1,i2,i3,i4;

  clonbanks();

  printf("dmconfig: dir>%s< len=%d\n",dir,strlen(dir));

  len = strlen(dir);
  strncpy(tmp,dir,MAXSTRLEN);
  sss = tmp;
  i = 0;
  while(*sss++ == ' ' && i<len) i++; /* skip spaces in the begining */
  if(i==len)
  {
    printf("dmconfig: i=%d len=%d\n",i,len);
    printf("dmconfig: use default directory in CLON_PARMS area\n");
    if((parm = getenv("CLON_PARMS")) == NULL)
    {
      printf("dmconfig: env. variable CLON_PARMS is not set - exit.\n");
      exit(0);
    }
    sprintf(dname,"%s/cmon/dmlib/",parm);
  }
  else
  {
    sss--;
    printf("dmconfig 2: sss>%s< len=%d\n",sss,strlen(sss));
    len = strlen(sss);
    while(sss[len-1]==' ') len--; /* remove trailing spaces */
    if(sss[len-1]=='/')
    {
      sss[len] = '\0';
    }
    else
    {
      sss[len++] = '/';
      sss[len] = '\0';
    }
    printf("dmconfig: sss>%s< len=%d\n",sss,strlen(sss));
    printf("dmconfig: looking in directory >%s<\n",sss);
    sprintf(dname,"%s",sss);
  }

  printf("dmconfig: using config files from >%s< directory\n",dname);
  if((dd = opendir(dname)) == NULL)
  {
    printf("dmconfig: config files dir >%s< does not exist - exit.\n",dname);
    exit(0);
  }


  /* scan directory and count sets */
  nsets = 0;
  while((de = readdir(dd)) != NULL)
  {
    /*printf("file >%s< (len=%d) inode=%d offset=%d len=%d\n",
      de->d_name,strlen(de->d_name),de->d_ino,de->d_off,de->d_reclen);*/
    len = strlen(de->d_name);
    if(len > 6)
    {
      ch = (char *) &de->d_name[len-6];
      if(!strncmp(ch,".dmcfg",6))
      {
        printf("dmconfig: found file >%s<\n",de->d_name);
        nsets ++;
      }
    }
  }
  closedir(dd);
  printf("dmconfig: found %d sets\n",nsets);


  /* allocate memory for sets */
  sets = (DMSET *) calloc(nsets,sizeof(DMSET));
  

  /* Some defaults */
  /* ............. */


  /* scan directory again and fill sets */
  if((dd = opendir(dname)) == NULL)
  {
    printf("dmconfig: config files dir >%s< does not exist - exit.\n",dname);
    exit(0);
  }
  else
  {
    msets = 0;
    while((de = readdir(dd)) != NULL)
    {
      /*printf("file >%s< (len=%d) inode=%d offset=%d len=%d\n",
        de->d_name,strlen(de->d_name),de->d_ino,de->d_off,de->d_reclen);*/
      len = strlen(de->d_name);
      if(len > 6)
      {
        ch = (char *) &de->d_name[len-6];
        if(!strncmp(ch,".dmcfg",6))
        {
          sprintf(fname,"%s%s",dname,de->d_name);
          dmparse(fname,msets);
          msets ++;
        }
      }
      if(msets >= nsets) break; /* if second readdir gets more files */
    }
    nsets = msets; /* if second readdir gets less files */
    closedir(dd);
  }



#ifdef USE_TREE
  /********************/
  /* fills out 'tree' */

  maxbankid = maxbanknum = maxhighid = maxlowid = maxplace = 0;

  /*loop over list of sets looking for the maximum parameters values*/
  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
        maxbankid = MAX(maxbankid,sets[iset].banks[mlist].bankid);
        maxbanknum = MAX(maxbanknum,sets[iset].banks[mlist].par[0]);
        maxhighid = MAX(maxhighid,sets[iset].banks[mlist].par[1]);
        maxlowid = MAX(maxlowid,sets[iset].banks[mlist].par[2]);
        maxplace = MAX(maxplace,sets[iset].banks[mlist].par[3]);
      }
    }
  }

  /*increment to use in arrays, for example bankid=5 needs 6-dim array to be used as bankid[5]*/
  maxbankid++;
  maxbanknum++;
  maxhighid++;
  maxlowid++;
  maxplace++;

  /* search tree allocation and filling */
  dmbankid = (DMBANKIDS *) calloc(maxbankid,sizeof(DMBANKIDS));
  for(i1=0; i1<maxbankid; i1++)
  {
    if(checkbankid(i1)) dmbankid[i1].banknum = (DMBANKNUMS *) calloc(maxbanknum,sizeof(DMBANKNUMS));
    else {continue;}

    for(i2=0; i2<maxbanknum; i2++)
	{
      dmbankid[i1].banknum[i2].highid = (DMHIGHIDS *) calloc(maxhighid,sizeof(DMHIGHIDS));
      for(i3=0; i3<maxhighid; i3++)
	  {
        dmbankid[i1].banknum[i2].highid[i3].lowid = (DMLOWIDS *) calloc(maxlowid,sizeof(DMLOWIDS));
        for(i4=0; i4<maxlowid; i4++)
	    {
          dmbankid[i1].banknum[i2].highid[i3].lowid[i4].nsets = 0;
	    }
	  }
	}
  }


  /* count the number of sets in every tree branch */
  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
        bankid  = sets[iset].banks[mlist].bankid;
        banknum = sets[iset].banks[mlist].par[0];
        highid  = sets[iset].banks[mlist].par[1];
        lowid   = sets[iset].banks[mlist].par[2];

        dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets ++;
		/*
        printf("NEW: [%d][%d][%d][%d].nsets=%d\n",bankid,banknum,highid,lowid,
          dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets);
		*/
      }
    }
  }


  /* allocate the ends of tree branches */
  for(i1=0; i1<maxbankid; i1++)
  {
    if(!checkbankid(i1)) continue;
    for(i2=0; i2<maxbanknum; i2++)
	{
      for(i3=0; i3<maxhighid; i3++)
	  {
        for(i4=0; i4<maxlowid; i4++)
	    {
          itmp = dmbankid[i1].banknum[i2].highid[i3].lowid[i4].nsets;
          dmbankid[i1].banknum[i2].highid[i3].lowid[i4].nsets = 0;
          dmbankid[i1].banknum[i2].highid[i3].lowid[i4].setid = (int *) calloc(itmp,sizeof(int));
	    }
	  }
	}
  }

  /* fills the ends of tree branches */
  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
        bankid  = sets[iset].banks[mlist].bankid;
        banknum = sets[iset].banks[mlist].par[0];
        highid  = sets[iset].banks[mlist].par[1];
        lowid   = sets[iset].banks[mlist].par[2];

        itmp = dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets;
        dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[itmp] = iset;
        dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets ++;
      }
    }
  }
#endif


  for(i=0; i<nsets; i++) sets[i].count = 0;


  /*dmtreeprint();*/

#ifdef USE_CA
  nelem = nsets;
  {
    int status;

    /* Initialize CA */
  	SEVCHK(ca_task_initialize(),"Unable to initialize");

    /* get the channel id:  channel ASCII name,       ptr to chid */
  	status = ca_search(	      caname,		              &chan);
  	SEVCHK(status, "Bad Channel Name?")

	/* wait for the channel's IOC to be found */
	status = ca_pend_io(1.0);
	if(status == ECA_TIMEOUT)
    {
	  printf("%s: Not Found\n",caname);
	  exit(-1);
	}
  	SEVCHK(status, NULL)
 
	if(nelem>ca_element_count(chan))
    {
	  printf("Specified element count =%d IOC element count=%d\n",
			 nelem,ca_element_count(chan));
	  exit(-1);
	}
  }
#endif

  return(0);
}



void
dmlib(int *jw)
{
  int i, j, k, kk, nbanks, bankid, banknum, treeid, ncol, nrow, lfmt, index, itmp, iset, mlist;
  int ind, ind1, indlist[LISTLEN], highid, lowid, place;
  unsigned int *ptr1, *end1;   /* pointer to the B32 and I format banks */
  unsigned short *ptr2, *end2; /* pointer to the B16 format banks */
  unsigned char *ptr4, *end4;  /* pointer to the B08 format banks */
  char ch, name[5];

  /* reminder: &jw[ind-1] contains length, data starts from &jw[ind] */
  indlist[0] = LISTLEN;
  nbanks = etLget(jw,"*",indlist);

  /*indlist[1] contains the number of banks, pointers starts from indlist[2]*/
  if(nbanks != indlist[1])
  {
    printf("dmlib: nbanks = %d %d\n",nbanks,indlist[1]);
    printf("dmlib: not enough space in indlist array - lost some banks\n");
  }


  /* loop over banks */
  for(i=0; i<indlist[1]; i++)
  {

    ind = indlist[i+2];

    strncpy(name,(char *)&jw[ind-1-INAM],4);
	/*	
printf("ind=%d, name >%4.4s<\n",ind,name);
	*/
    name[4] = '\0';
    bankid = bosMgetid(name);
    /*if(bankid>=maxbankid) continue;*/
    lfmt = bosMgetlfmt(name);
    banknum = jw[ind-1-INR];
    /*if(banknum>=maxbankid) continue;*/
    ncol = etNcol(jw, ind);
    nrow = etNrow(jw, ind);
    ind1 = ind + etNdata(jw, ind); /* first index after bank */

	/*
    printf("name >%4.4s< bankid=%d number=%d -> treeid=%d\n",
                           name, bankid, banknum, treeid);
    printf("   ncol=%d nrow=%d lfmt=%d\n",ncol,nrow,lfmt);
	*/


/*TODO: MUST RECOGNIZE B16 from B32 etc - different HI/LI ID's !!!???*/
/*RIGHT NOW WORKS FOR B16 ONLY !!!???*/


      ptr2 = (unsigned short *)&jw[ind];
      end2 = ptr2 + (ncol*nrow);
      /*printf("ptr2=0x%08x(%d) end2=0x%08x(%d)\n",
        ptr2,(unsigned int)ptr2,end2,(unsigned int)end2);*/
      for(k=0; k<nrow; k++)
      {
		/*printf("bankid=%d highlowid=%d\n",bankid,highlowid[bankid]);*/
        {
          lowid = (*ptr2)&0xFF;
          highid = ((*ptr2)>>8)&0xFF;
          /*printf("bankid=%d highid=%d,lowid=%d\n",bankid,highid,lowid);*/
		  /*
if(banknum==1)
{
  if(highid>30) printf("[%2d] ==> bankid=%d highid=%d,lowid=%d\n",k,bankid,highid,lowid);
  else printf("[%2d] --> bankid=%d highid=%d,lowid=%d\n",k,bankid,highid,lowid);
}
*/
		  /* what was the purpose to loop over all cols ??? */
          /*for(j=1; j<ncol; j++)*/
          for(j=0; j<1; j++)
          {
            place = j;
 
  /*
  printf("new--> %d %d %d %d\n",
		 bankid,banknum,highid,lowid);fflush(stdout);
  printf("new==> %x\n",dmbankid);fflush(stdout);
  printf("new==>    %x\n",dmbankid[bankid].banknum);fflush(stdout);
  printf("new==>       %x\n",dmbankid[bankid].banknum[banknum].highid);fflush(stdout);
  */
  /*
  printf("new==>          %x\n",dmbankid[bankid].banknum[banknum].highid[highid].lowid);fflush(stdout);
  printf("new--> nsets=%d\n",
    dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets);fflush(stdout);
  */



#ifdef USE_TREE
  if(dmbankid!=NULL && bankid<maxbankid)
  {
	if(dmbankid[bankid].banknum!=NULL && banknum<maxbanknum)
	{
	  if(dmbankid[bankid].banknum[banknum].highid!=NULL && highid<maxhighid)
	  {
		if(dmbankid[bankid].banknum[banknum].highid[highid].lowid!=NULL && lowid<maxlowid)
		{
          if(dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets>0)
          {
            /*printf("new [%d]: ",
			  dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets);fflush(stdout);*/
            for(kk=0; kk<dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets; kk++)
            {
              itmp = dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[kk];
              /*printf("%d ",itmp);*/
              sets[itmp].count ++;
	        }
            /*printf("\n");*/
          }
		}
	  }
	}
  }
#else
  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
        if( bankid == sets[iset].banks[mlist].bankid &&
            banknum == sets[iset].banks[mlist].par[0] &&
            highid == sets[iset].banks[mlist].par[1] &&
            lowid == sets[iset].banks[mlist].par[2])
		{
          sets[iset].count ++;
		}
      }
    }
  }
#endif


  /*
  printf("[%d] [%d] [%d] [%d] -> %d %d\n",bankid,banknum,highid,lowid,
		 sets[tree[treeid].hlpl[index].setid[kk]].count,
         dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[kk].count);
  */

          }
        }

        ptr2+=ncol ; /* goto next row */
      }


  } /* loop over banks */



  /* test channel access */

#ifdef USE_CA
  ncalls ++;
  if(ncalls >= 100000)
  {
    int status;
    float *pvalue;

	pvalue = (float *) calloc(ca_element_count(chan),sizeof(*pvalue));
	if(!pvalue)
    {
	  exit(-1);
	}

    /* fills arrays of values */
	for(i=0; i<nelem; i++)
    {
	  /*pvalue[i] = rand();*/
      pvalue[i] = sets[i].count;
      sets[i].count = 0;
	}

	/* write values if any */
	if(nelem)
    {
	  status = ca_array_put(DBR_FLOAT, nelem, chan, pvalue);
  	  SEVCHK(status, NULL);
	}

	/*
	 *	read the current value as a string
	 *	(put will always occur first since it was sent first)
	 */
	status = ca_array_get(DBR_FLOAT, ca_element_count(chan), chan, pvalue);
  	SEVCHK(status, NULL)

	/* pend for gets to return */
	status = ca_pend_io(10.0);
	if(status == ECA_TIMEOUT)
    {
	  printf("%s: Get Timed Out\n",caname);
	  exit(-1);
	}
  	SEVCHK(status, NULL)

	/* print the values */
	for(i=0;i<ca_element_count(chan);i++)
    {
	  printf("%s[%04d] = %f\n", caname, i, pvalue[i]);
	}

	free(pvalue);

    ncalls = 0;
  }

#endif

  return;
}

