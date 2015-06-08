h48614
s 00000/00413/00849
d D 1.5 10/07/19 22:31:15 boiarino 5 4
c *** empty log message ***
e
s 00001/00001/01261
d D 1.4 10/07/18 23:53:48 boiarino 4 3
c *** empty log message ***
e
s 00172/00018/01090
d D 1.3 10/07/18 23:43:50 boiarino 3 2
c *** empty log message ***
e
s 00542/00233/00566
d D 1.2 10/07/18 18:15:16 boiarino 2 1
c *** empty log message ***
e
s 00799/00000/00000
d D 1.1 07/06/28 23:24:36 boiarino 1 0
c date and time created 07/06/28 23:24:36 by boiarino
e
u
U
f e 0
t
T
I 1

D 2
#define USE_CA
E 2
I 2
#undef USE_CA
E 2

I 3
#define USE_TREE

E 3
/* dmlib.c - data monitor library */

#include <stdio.h>
#include <stdlib.h>
I 2
#include <string.h>
#include <errno.h>
E 2
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
D 2
#define MAXBANKID    200 /* MUST be the number of banks in *.ddl !!! */
#define MAXBANKNUM 10000 /* MUST be the max bank number !!! */
#define MAXTREE (MAXBANKID*MAXBANKNUM)
E 2

D 2
#define TREEID ((bankid<<nbitbanknum)+banknum)
#define GETBANKID(word) (((word)>>nbitbanknum)&maxbankid)
#define GETBANKNUM(word) ((word)&maxbanknum)

/* high and low id's */
#define INDEX2 (((highid<<nbitplace)<<nbitlowid)+(lowid<<nbitplace)+place)
/* one B32/I id */
#define INDEX1 ((lowid<<nbitplace)+place)
/* no id */
#define INDEX0 (place)
/* decoding */
#define GETHIGHID(word) ((((word)>>nbitlowid)>>nbitplace)&maxhighid)
#define GETLOWID(word) (((word)>>nbitplace)&maxlowid)
#define GETPLACE(word) ((word)&maxplace)


E 2
#define MAX(x,y) ((x) > (y) ? (x) : (y))

D 2
static int maxbankid, maxbanknum, maxhighid, maxlowid, maxplace;
static int nbitbankid, nbitbanknum, nbithighid, nbitlowid, nbitplace;
static int maxtree, maxindex;

static int *highlowid; /* highlowid[bankid]=2 if id's in B16,
                                           =1 if id in B32 or I,
                                           =0 if no id (HEAD bank etc) */
E 2
static int    nsets;
static DMSET *sets;
D 2
static DMTREE *tree;
E 2

I 2
static int maxbankid, maxbanknum, maxhighid, maxlowid, maxplace;
static DMBANKIDS *dmbankid;


E 2
#ifdef USE_CA
static unsigned  nelem = 0; /* the number of elements to be sent over CA */
static char     *caname = "test10"; /* channel name */
static chid      chan;

static int ncalls = 0;
#endif

D 2
/* print tree */
E 2
I 2


D 5

#include <mysql/mysql.h>


/*define the string, included_tables, to be the necessary list of tables to support the query*/
#define included_tables "\
tdcmuxmap,\
stbadbmap,\
locwire2stb,\
layer2loclayer,\
wire2locwire,\
sec2adbcrate,\
stbpin2adbpin,\
adb2mux,\
muxpin2tdcpin,\
adbpin2connhalf,\
adbpin2muxpin,\
adbcrate_location,\
tdccrate_location,\
fusebox\
"



/*define the string, table_connect, to be the necessary join of the tables
required to define the entire signal wire to tdc mapping*/
#define table_connect "\
sec2adbcrate.adbcrateloc=tdcmuxmap.adbcrateloc and \
layer2loclayer.layer= wire2locwire.layer and \
wire2locwire.locwire=locwire2stb.locwire and \
layer2loclayer.loclayer=locwire2stb.loclayer and \
wire2locwire.stbboardloc=stbadbmap.stbboardloc and \
locwire2stb.stbconnector=stbadbmap.stbconnector and \
stbadbmap.suplayer=locwire2stb.suplayer and \
locwire2stb.suplayer=layer2loclayer.suplayer and \
layer2loclayer.suplayer=stbpin2adbpin.suplayer and \
stbpin2adbpin.suplayer=sec2adbcrate.suplayer and \
stbpin2adbpin.stbpin=locwire2stb.stbpin and \
stbpin2adbpin.adbpin=adbpin2connhalf.adbpin and \
stbpin2adbpin.adbpin=adbpin2muxpin.adbpin and \
adbpin2muxpin.muxpin=muxpin2tdcpin.muxpin and \
adbpin2connhalf.adbconnhalf=adb2mux.adbconnhalf and \
adb2mux.adbconnector=stbadbmap.adbconnector and \
adb2mux.pinmatch=adbpin2muxpin.pinmatch and \
adb2mux.muxconnector=tdcmuxmap.muxconnector and \
tdcmuxmap.adbslot=stbadbmap.adbslot and \
adbcrate_location.adbcrateloc=tdcmuxmap.adbcrateloc and \
tdccrate_location.tdccrateloc=tdcmuxmap.tdccrateloc and \
fusebox.sector=sec2adbcrate.sector and \
fusebox.region=layer2loclayer.region and \
fusebox.fusenum=wire2locwire.stbboardloc\
"



/*define the string, get_columns, to be those things you wish to get*/
#define get_columns "\
tdcmuxmap.adbcrateloc,\
tdcmuxmap.adbslot,\
sec2adbcrate.adbcratename,\
adb2mux.adbconnector,\
adbpin2muxpin.adbpin,\
adb2mux.muxconnector,\
muxpin2tdcpin.muxpin,\
adb2mux.siglength,\
tdcmuxmap.tdccrateloc,\
tdcmuxmap.tdcslot,\
tdcmuxmap.tdcconnector,\
muxpin2tdcpin.tdcpin,\
sigcablename,\
adbcrate_location.spaceframe_location,\
tdccrate_location.spaceframe_location,\
fusebox.sector,\
fusebox.region,\
fusebox.fusenum,\
wire2locwire.stbboardloc\
"

/*
layer2loclayer.region,\
sec2adbcrate.sector\
*/



#define NCONFIGLINES 1024

D 3
#define PROCESS_OBJECT(OBJ,nOBJ,OBJgrouping) \
E 3
I 3
#define PROCESS_OBJECT(object,OBJ,nOBJ,OBJgrouping) \
E 3
        if(nOBJ<NCONFIGLINES) \
		{ \
          if(nOBJ>1 && \
             OBJgrouping==0 && \
             OBJ[nOBJ-2].number==OBJ[nOBJ-1].number && \
             OBJ[nOBJ-2].sector==OBJ[nOBJ-1].sector && \
             OBJ[nOBJ-2].layer==OBJ[nOBJ-1].layer && \
             OBJ[nOBJ-2].wire==(OBJ[nOBJ-1].wire-1) && \
             OBJ[nOBJ-1].wire==(wire-1)) \
		  { \
			/*printf("1\n");*/ \
            OBJ[nOBJ-1].wire = wire; \
            OBJgrouping=1; /* open grouping, if have already wires N, N+1 and current wire is N+2 */ \
		  } \
          else if(OBJgrouping==1 && \
D 3
             OBJ[nOBJ-1].number==adb_crate && \
E 3
I 3
             OBJ[nOBJ-1].number==object && \
E 3
             OBJ[nOBJ-1].sector==sector && \
             OBJ[nOBJ-1].layer==layer && \
             OBJ[nOBJ-1].wire==(wire-1)) \
		  { \
			/*printf("2\n");*/ \
            OBJ[nOBJ-1].wire = wire; /* keep filling group */ \
		  } \
          else if(OBJgrouping==1 && \
D 3
			 (OBJ[nOBJ].number!=adb_crate || \
E 3
I 3
			 (OBJ[nOBJ].number!=object || \
E 3
              OBJ[nOBJ].sector!=sector || \
              OBJ[nOBJ].layer!=layer || \
              OBJ[nOBJ].wire!=(wire-1))) \
		  { \
			/*printf("3\n");*/ \
            OBJgrouping=0; /* close grouping */ \
D 3
            OBJ[nOBJ].number = adb_crate; \
E 3
I 3
            OBJ[nOBJ].number = object; \
E 3
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
          else \
		  { \
			/*printf("4\n");*/ \
D 3
            OBJ[nOBJ].number = adb_crate; \
E 3
I 3
            OBJ[nOBJ].number = object; \
E 3
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
		}

/* get hardware maps from database */ 
E 2
void
D 2
dmtreeprint()
E 2
I 2
dmdatabase()
E 2
{
D 2
  int i, j, itmp;
E 2
I 2
  FILE *fd;
D 3
  int i, ii;
E 3
I 3
  int i, ii, jj;
E 3
  const char *host = "clasdb";
  const char *database = "dc_chan_status";
  const char *user = "clasuser";
  const char *passwd = "";
  unsigned int port = 0;
  const char *unix_socket = "";
  unsigned long client_flag = 0;
E 2

D 2
  printf("\n================ Print Tree =================\n");
  for(i=0; i<maxtree; i++)
E 2
I 2
  MYSQL *mysql;
  MYSQL_RES *result;
  MYSQL_ROW row_out;
  char query[8192];
  char specific_request[4096];
  char fname[256];

  int sector, layer, wire;
D 3
  int adb_crate, adb_board, tdc_crate, tdc_board, lv_supply, lv_fuse;
E 3
I 3
  int adb_crate, adb_board, tdc_crate, tdc_board, lv_supply, lv_supply_old, lv_fuse;
E 3

  int nADBcrate, ADBgrouping;
  ADBCRATE ADBcrate[NCONFIGLINES];

I 3
  int nTDCcrate, TDCgrouping;
  TDCCRATE TDCcrate[NCONFIGLINES];
E 3

I 3
  int nLVcrate, LVgrouping;
  LVCRATE LVcrate[NCONFIGLINES];
E 3

I 3
  int nFUSE, FUSEgrouping;
  LVFUSE FUSE[NCONFIGLINES];
E 3




I 3



E 3
  /*initialize MYSQL structure*/
  mysql = mysql_init(NULL);
  if(mysql == NULL)
E 2
  {
D 2
    for(j=0; j<maxindex; j++)
E 2
I 2
    printf("mysql_init failed\n");
    return;
  }

  /* connect to the database */
  mysql = mysql_real_connect( mysql,
                              host,
                              user,
                              passwd,
                              database,
                              port,
                              unix_socket,
                              client_flag);
  if(mysql == NULL)
  {
    printf("mysql_real_connect failed\n");
    return;
  }




I 3


  /*****************************************/
  /* gets everything we need from database */

E 3
  nADBcrate = 0;
  ADBgrouping = 0;
I 3
  nTDCcrate = 0;
  TDCgrouping = 0;
  nLVcrate = 0;
  LVgrouping = 0;
E 3

I 3
  nFUSE = 0;
  FUSEgrouping = 0;

E 3
  for(sector=1; sector<=6; sector++)
  for(layer=1; layer<=36; layer++)
  for(wire=1; wire<=192; wire++)
  {
	/*
    printf("sector=%d layer=%d wire=%d\n",sector,layer,wire);
	*/

    /*define the string, specific_reques*/
    sprintf(specific_request,"sec2adbcrate.sector=%d and layer2loclayer.layer=%d and wire=%d",
		  sector,layer,wire);

    /*
    printf("specific_request >%s<\n",specific_request);
    */

    /*form mysql query*/
    sprintf(query,"select %s from %s where %s and %s",
		  get_columns,included_tables,table_connect,specific_request);

    /*
    printf("query >%s<\n",query);
    */

    mysql_query(mysql,query);
    result = mysql_store_result(mysql);

    /*
    printf("123: result=0x%08x, errno=%d\n",result,errno);
    */
  
    if(result==NULL)
E 2
    {
D 2
      if(tree[i].hlpl[j].nsets > 0)
E 2
I 2
      printf("mysql_store_result returns NULL, do nothing\n");
    }
    else
    {
	  /*printf("456\n");fflush(stdout);*/
     row_out = mysql_fetch_row(result);
     /*printf("789: 0x%08x\n",row_out);fflush(stdout);*/

      /* get run number */
      if(row_out==NULL)
E 2
      {
D 2
        printf("tree %d index %d nsets %d\n",i,j,tree[i].hlpl[j].nsets);
        printf("bankid=%d banknum=%d highid=%d lowid=%d place=%d)\n",
          GETBANKID(i),GETBANKNUM(i),
          GETHIGHID(j),GETLOWID(j),GETPLACE(j));
E 2
I 2
        /*printf("NO\n")*/;
E 2
      }
I 2
      else
      {
		/*printf("11\n");fflush(stdout);*/
        ii = mysql_num_fields(result);
		/*printf("12\n");fflush(stdout);*/
	    
		/*
        printf("The number of returned elements is %d\n",ii);
        for(i=0; i<ii; i++) printf("[%2d] >%s<\n",i,row_out[i]);
		*/

        adb_crate = atoi(row_out[0]);
        adb_board = atoi(row_out[1]);
        tdc_crate = atoi(row_out[8]);
        tdc_board = atoi(row_out[9]);
I 3
		lv_supply_old = lv_supply;
E 3
        lv_supply = (atoi(row_out[15])-1)*3 + atoi(row_out[16]);
        lv_fuse = atoi(row_out[17]);
D 3

E 3
		/*
I 3
        lv_fuse = (lv_supply-1)*24 + lv_fuse;
		*/
		/*
E 3
        printf("sector %d, layer %2d, wire %3d --->",sector,layer,wire);
        printf(" ADB crate >%2s< {%d}, ADB board >%2s< {%d}",row_out[0],adb_crate,row_out[1],adb_board);
        printf(" TDC crate >%2s< {%d}, TDC board >%2s< {%d}",row_out[8],tdc_crate,row_out[9],tdc_board);
        printf(" LV supply >%1s_%1s< {%d}, LV fuse >%2s< {%d}\n",row_out[15],row_out[16],lv_supply,row_out[17],lv_fuse);
		*/
		/*
		printf("%d %d %d -> %d (ADBgrouping=%d)\n",sector,layer,wire,adb_board,ADBgrouping);
		*/

D 3
        PROCESS_OBJECT(ADBcrate,nADBcrate,ADBgrouping);
E 3
I 3
        if(lv_supply_old>0 && lv_supply != lv_supply_old)
		{
		  /* FUSE files */
          ii = lv_supply;
          for(jj=1; jj<=24; jj++)
          {
            sprintf(fname,"dc_fuse_%02d_%02d.dmcfg",ii,jj);
            fd = fopen(fname,"w");
            if(fd!=NULL)
            {
              fprintf(fd,"#\n# configuration file for DC box %02d fuse %02d\n#\nNAME DCFUSE_%02d_%02d  \"BOX %02d FUSE %02d\"\n#\nNORM\n",
                ii,jj,ii,jj,ii,jj);
              fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
              for(i=0; i<nFUSE; i++)
              {
D 4
                if(FUSE[i].number==ii) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
E 4
I 4
                if(FUSE[i].number==jj) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
E 4
                  FUSE[i].sector,FUSE[i].layer,FUSE[i].wire);
  	          }
              fprintf(fd,"#\nSTOP\n#\n");
              fclose(fd);
            }
          }

          nFUSE = 0;
          FUSEgrouping = 0;
		}


        PROCESS_OBJECT(adb_crate,ADBcrate,nADBcrate,ADBgrouping);
        PROCESS_OBJECT(tdc_crate,TDCcrate,nTDCcrate,TDCgrouping);
        PROCESS_OBJECT(lv_supply,LVcrate,nLVcrate,LVgrouping);
        PROCESS_OBJECT(lv_fuse,FUSE,nFUSE,FUSEgrouping);
E 3
      }
E 2
    }
I 2

    mysql_free_result(result);
E 2
  }
I 2


  /************************/
  /* create *.dmcfg files */
  
  /* ADB crate files */
  for(ii=1; ii<=30; ii++)
  {
    sprintf(fname,"dc_adb_crate_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
      fprintf(fd,"#\n# configuration file for DC ADB crate %02d\n#\nNAME DCADBCRATE%02d  \"ADB crate %02d\"\n#\nNORM\n",ii,ii,ii);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nADBcrate; i++)
      {
        if(ADBcrate[i].number==ii) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          ADBcrate[i].sector,ADBcrate[i].layer,ADBcrate[i].wire);
  	}
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }

I 3
  /* TDC crate files */
  for(ii=1; ii<=11; ii++)
  {
    sprintf(fname,"dc_tdc_crate_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
      fprintf(fd,"#\n# configuration file for DC TDC crate %02d\n#\nNAME DCTDCCRATE%02d  \"TDC crate %02d\"\n#\nNORM\n",ii,ii,ii);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nTDCcrate; i++)
      {
        if(TDCcrate[i].number==ii) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          TDCcrate[i].sector,TDCcrate[i].layer,TDCcrate[i].wire);
  	}
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }


  /* LV supply files */
  for(ii=1; ii<=18; ii++)
  {
    sprintf(fname,"dc_lv_supply_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
      fprintf(fd,"#\n# configuration file for DC LV supply %02d\n#\nNAME DCLVSUPPLY%02d  \"LV supply %02d\"\n#\nNORM\n",ii,ii,ii);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nLVcrate; i++)
      {
        if(LVcrate[i].number==ii) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          LVcrate[i].sector,LVcrate[i].layer,LVcrate[i].wire);
  	}
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }










E 3
  mysql_close (mysql);

  return;
}




E 5
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

E 2
  printf("\n=============================================\n");

  return;
}

I 2
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

E 2
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
I 2
	  /*
E 2
      printf("%s",str);
      printf("len=%d\n",strlen(str));
I 2
	  */
E 2

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
D 3
  printf("--> mlist = %d\n",sets[iset].nbanks);
  for(i=0; i<mlist; i++)
  {
D 2
    sets[iset].banks = (DMBANK *) malloc(mlist*sizeof(DMBANK));
E 2
I 2
    sets[iset].banks = (DMBANK *) calloc(mlist,sizeof(DMBANK));
E 2
  }
E 3
I 3
  printf("--> mlist = %d (x sizeof(DMBANK)=%d)\n",sets[iset].nbanks,sizeof(DMBANK));
  sets[iset].banks = (DMBANK *) calloc(mlist,sizeof(DMBANK));
E 3

  /* read file again filling up structure */
  firstbankline = 0;
  if(fd = fopen(fname,"r"))
  {
    mlist = 0;
I 2

    /* some cleanup */
    for(i=0; i<NAMLEN; i++) sets[iset].sname[i] = ' ';
    for(i=0; i<TITLEN; i++) sets[iset].title[i] = ' ';
    for(i=0; i<NAMLEN; i++) sets[iset].snorm[i] = ' ';

E 2
    do
    {
D 2
      /* some cleanup */
      for(i=0; i<NAMLEN; i++) sets[iset].sname[i] = ' ';
      for(i=0; i<NAMLEN; i++) sets[iset].snorm[i] = ' ';
      for(i=0; i<TITLEN; i++) sets[iset].title[i] = ' ';

E 2
      if(!fgets(str,99,fd)) break;
      if(str[0] == '#') continue; /* skip comment line */
      if(!strncmp(str,"STOP",4)) break;
I 2
	  /*
E 2
      printf("%s",str);
      printf("len=%d\n",strlen(str));
I 2
	  */
E 2

      if(!strncmp(str,"NAME",4))
      {
        sscanf(&str[4],"%s",sets[iset].sname);
D 2
        printf("  sname >%s<\n",sets[iset].sname);
E 2
        j = 0;
        for(i=0; i<strlen(str)-1; i++) if(str[i]=='"') {j=i+1; break;}
        if(j==0) {printf("dmparse: wrong format - exit.\n");exit(0);}
        ch = (char *) &str[j];
        j = 0;
        for(i=0; i<strlen(str)-j; i++) if(ch[i]=='"') {j=i; break;}
        if(j==0) {printf("dmparse: wrong format - exit.\n");exit(0);}
        ch[j] = '\0';
        strcpy(sets[iset].title,ch);
D 2
        printf("  title >%s<\n",sets[iset].title);
E 2
I 2
        printf("[%d]  sname >%s<\n",iset,sets[iset].sname);
        printf("[%d]  title >%s<\n",iset,sets[iset].title);
E 2
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
D 3
                  sets[iset].banks[mlist].bankid =
                              bosMgetid(sets[iset].banks[mlist].bankname);
E 3
I 3
                  sets[iset].banks[mlist].bankid = bosMgetid(sets[iset].banks[mlist].bankname);
E 3
I 2
				  /*
E 2
                  printf("  par: %4.4s %4.4s %i %i %i %i\n",
                    sets[iset].banks[mlist].banktype,
                    sets[iset].banks[mlist].bankname,
                    sets[iset].banks[mlist].par[0],
                    sets[iset].banks[mlist].par[1],
                    sets[iset].banks[mlist].par[2],
                    sets[iset].banks[mlist].par[3]);
D 2

E 2
I 2
				  */
E 2
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
I 3
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
E 3
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
D 2
  int maxbankid1, maxbanknum1, maxhighid1, maxlowid1, maxplace1;
E 2
I 2
  int i1,i2,i3,i4;
E 2

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
D 2
  sets = (DMSET *) malloc(nsets*sizeof(DMSET));
E 2
I 2
  sets = (DMSET *) calloc(nsets,sizeof(DMSET));
E 2
  

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



I 3
#ifdef USE_TREE
E 3
  /********************/
  /* fills out 'tree' */

D 2
  maxbankid1 = maxbanknum1 = maxhighid1 = maxlowid1 = maxplace1 = 0;
E 2
I 2
  maxbankid = maxbanknum = maxhighid = maxlowid = maxplace = 0;

  /*loop over list of sets looking for the maximum parameters values*/
E 2
  for(iset=0; iset<nsets; iset++)
  {
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
    {
      if(!strncmp(sets[iset].banks[mlist].banktype,"BANK",4))
      {
D 2
        maxbankid1 = MAX(maxbankid1,sets[iset].banks[mlist].bankid);
        maxbanknum1 = MAX(maxbanknum1,sets[iset].banks[mlist].par[0]);
        maxhighid1 = MAX(maxhighid1,sets[iset].banks[mlist].par[1]);
        maxlowid1 = MAX(maxlowid1,sets[iset].banks[mlist].par[2]);
        maxplace1 = MAX(maxplace1,sets[iset].banks[mlist].par[3]);
E 2
I 2
        maxbankid = MAX(maxbankid,sets[iset].banks[mlist].bankid);
        maxbanknum = MAX(maxbanknum,sets[iset].banks[mlist].par[0]);
        maxhighid = MAX(maxhighid,sets[iset].banks[mlist].par[1]);
        maxlowid = MAX(maxlowid,sets[iset].banks[mlist].par[2]);
        maxplace = MAX(maxplace,sets[iset].banks[mlist].par[3]);
E 2
      }
    }
  }

D 2
  /* increment all values to use as array dimension */
  maxbankid1++;
  maxbanknum1++;
  maxhighid1++;
  maxlowid1++;
  maxplace1++;
E 2
I 2
  /*increment to use in arrays, for example bankid=5 needs 6-dim array to be used as bankid[5]*/
  maxbankid++;
  maxbanknum++;
  maxhighid++;
  maxlowid++;
  maxplace++;
E 2

D 2
  maxtree = maxbankid1*maxbanknum1;
  maxindex = maxhighid1*maxlowid1*maxplace1;
  printf("dmconfig: old values: %8d %8d (%8d %8d %8d %8d %8d)\n",
      maxtree,maxindex,maxbankid1,maxbanknum1,maxhighid1,maxlowid1,maxplace1);
E 2
I 2
  /* search tree allocation and filling */
  dmbankid = (DMBANKIDS *) calloc(maxbankid,sizeof(DMBANKIDS));
  for(i1=0; i1<maxbankid; i1++)
  {
D 3
    dmbankid[i1].banknum = (DMBANKNUMS *) calloc(maxbanknum,sizeof(DMBANKNUMS));
E 3
I 3
    if(checkbankid(i1)) dmbankid[i1].banknum = (DMBANKNUMS *) calloc(maxbanknum,sizeof(DMBANKNUMS));
    else {continue;}

E 3
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
E 2

D 2
  /* rounding to highest bit */
  itmp = 1;
  maxbankid = 1;
  nbitbankid = 0;
  while(maxbankid1>0) {maxbankid1>>=1; nbitbankid++;}
  for(i=0; i<(nbitbankid-1); i++) {itmp<<=1; maxbankid |= itmp;}
E 2

D 2
  itmp = 1;
  maxbanknum = 1;
  nbitbanknum = 0;
  while(maxbanknum1>0) {maxbanknum1>>=1; nbitbanknum++;}
  for(i=0; i<(nbitbanknum-1); i++) {itmp<<=1; maxbanknum |= itmp;}

  maxtree = (maxbankid<<nbitbanknum)+maxbanknum;;


  itmp = 1;
  maxhighid = 1;
  nbithighid = 0;
  while(maxhighid1>0) {maxhighid1>>=1; nbithighid++;}
  for(i=0; i<(nbithighid-1); i++) {itmp<<=1; maxhighid |= itmp;}

  itmp = 1;
  maxlowid = 1;
  nbitlowid = 0;
  while(maxlowid1>0) {maxlowid1>>=1; nbitlowid++;}
  for(i=0; i<(nbitlowid-1); i++) {itmp<<=1; maxlowid |= itmp;}

  itmp = 1;
  maxplace = 1;
  nbitplace = 0;
  while(maxplace1>0) {maxplace1>>=1; nbitplace++;}
  for(i=0; i<(nbitplace-1); i++) {itmp<<=1; maxplace |= itmp;}

  maxindex = (maxhighid<<(nbitlowid+nbitplace))
            +(maxlowid<<nbitplace)+maxplace;


  printf("dmconfig: new values: %8d %8d (%8d %8d %8d %8d %8d)\n",
      maxtree, maxindex, maxbankid, maxbanknum, maxhighid, maxlowid, maxplace);

  printf("dmconfig: max:   0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
      maxbankid, maxbanknum, maxhighid, maxlowid, maxplace);

  printf("dmconfig: nbits:   %8d   %8d   %8d   %8d   %8d\n",
      nbitbankid, nbitbanknum, nbithighid, nbitlowid, nbitplace);




  /* allocate highlowid */
  highlowid = (int *) malloc(maxbankid*sizeof(int));
  for(i=0; i<maxbankid; i++) highlowid[i] = -1;

  /* allocate and initialize 'tree' */
  tree = (DMTREE *) malloc(maxtree*sizeof(DMTREE));
  for(i=0; i<maxtree; i++)
E 2
I 2
  /* count the number of sets in every tree branch */
  for(iset=0; iset<nsets; iset++)
E 2
  {
D 2
    tree[i].hlpl = (DMHLPL *) malloc(maxindex*sizeof(DMHLPL));
    for(j=0; j<maxindex; j++)
E 2
I 2
    for(mlist=0; mlist<sets[iset].nbanks; mlist++)
E 2
    {
D 2
      tree[i].hlpl[j].nsets = 0;
      tree[i].hlpl[j].setid = (int *) malloc(nsets*sizeof(int));
E 2
I 2
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
E 2
    }
  }


I 2
  /* allocate the ends of tree branches */
  for(i1=0; i1<maxbankid; i1++)
  {
I 3
    if(!checkbankid(i1)) continue;
E 3
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
E 2

D 2

  /* fill 'tree' */
E 2
I 2
  /* fills the ends of tree branches */
E 2
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
D 2
        place   = sets[iset].banks[mlist].par[3];
E 2

D 2
        highlowid[bankid] = 0;
        if(lowid>=0) highlowid[bankid] ++;
        if(highid>=0) highlowid[bankid] ++;
        if(highlowid[bankid]==0)      index = INDEX0;
        else if(highlowid[bankid]==1) index = INDEX1;
        else                          index = INDEX2;

		/*
printf("==> 0x%x 0x%x 0x%x 0x%x 0x%x -> %d(0x%08x) %d(0x%08x) %d\n",
bankid,banknum,highid,lowid,place,TREEID,TREEID,index,index,
highlowid[bankid]);
		*/

        itmp = tree[TREEID].hlpl[index].nsets;
        tree[TREEID].hlpl[index].setid[itmp]=iset;
        tree[TREEID].hlpl[index].nsets = itmp + 1;
E 2
I 2
        itmp = dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets;
        dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[itmp] = iset;
        dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].nsets ++;
E 2
      }
    }
  }
I 3
#endif
E 3

I 3

E 3
  for(i=0; i<nsets; i++) sets[i].count = 0;

D 2
  dmtreeprint();
E 2

I 2
  /*dmtreeprint();*/
E 2

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
D 2
  int i, j, k, nbanks, bankid, banknum, treeid, ncol, nrow, lfmt, index;
E 2
I 2
D 3
  int i, j, k, kk, nbanks, bankid, banknum, treeid, ncol, nrow, lfmt, index, itmp;
E 3
I 3
  int i, j, k, kk, nbanks, bankid, banknum, treeid, ncol, nrow, lfmt, index, itmp, iset, mlist;
E 3
E 2
  int ind, ind1, indlist[LISTLEN], highid, lowid, place;
  unsigned int *ptr1, *end1;   /* pointer to the B32 and I format banks */
  unsigned short *ptr2, *end2; /* pointer to the B16 format banks */
  unsigned char *ptr4, *end4;  /* pointer to the B08 format banks */
  char ch, name[5];

D 2

E 2
  /* reminder: &jw[ind-1] contains length, data starts from &jw[ind] */
  indlist[0] = LISTLEN;
  nbanks = etLget(jw,"*",indlist);

D 2

E 2
  /*indlist[1] contains the number of banks, pointers starts from indlist[2]*/
  if(nbanks != indlist[1])
  {
    printf("dmlib: nbanks = %d %d\n",nbanks,indlist[1]);
    printf("dmlib: not enough space in indlist array - lost some banks\n");
  }

I 2

E 2
  /* loop over banks */
  for(i=0; i<indlist[1]; i++)
  {

    ind = indlist[i+2];

    strncpy(name,(char *)&jw[ind-1-INAM],4);
I 2
	/*	
printf("ind=%d, name >%4.4s<\n",ind,name);
	*/
E 2
    name[4] = '\0';
    bankid = bosMgetid(name);
D 2
    if(bankid>=maxbankid) continue;
E 2
I 2
    /*if(bankid>=maxbankid) continue;*/
E 2
    lfmt = bosMgetlfmt(name);
    banknum = jw[ind-1-INR];
D 2
    if(banknum>=maxbankid) continue;
E 2
I 2
    /*if(banknum>=maxbankid) continue;*/
E 2
    ncol = etNcol(jw, ind);
    nrow = etNrow(jw, ind);
    ind1 = ind + etNdata(jw, ind); /* first index after bank */
D 2
    treeid = TREEID;
E 2

	/*
    printf("name >%4.4s< bankid=%d number=%d -> treeid=%d\n",
                           name, bankid, banknum, treeid);
    printf("   ncol=%d nrow=%d lfmt=%d\n",ncol,nrow,lfmt);
	*/
D 2
    /* process one bank */
    if(lfmt==1)
    {
      ptr1 = (unsigned int *)&jw[ind-1];
      end1 = ptr1 + (ncol*nrow);
      /*printf("ptr1=0x%08x(%d) end1=0x%08x(%d)\n",
        ptr1,(unsigned int)ptr1,end1,(unsigned int)end1);*/
      for(k=0; k<nrow; k++)
      {
E 2

D 2
        if(highlowid[bankid]==0)
        {
          for(index=0; index<ncol; index++)
          {
            if(index>=maxindex) break;
            if(tree[treeid].hlpl[index].nsets>0)
            {
			  /*
    printf("name >%4.4s< bankid=%d number=%d -> treeid=%d\n",
                           &jw[ind-1-INAM], bankid, banknum, treeid);
    printf("   ncol=%d nrow=%d lfmt=%d\n",ncol,nrow,lfmt);
              printf("------- yes1: index=%d, nsets=%d\n",
                index,tree[treeid].hlpl[index].nsets);
			  */
              for(k=0; k<tree[treeid].hlpl[index].nsets; k++)
              {
                /*printf(" %d",tree[treeid].hlpl[index].setid[k]);*/
                sets[tree[treeid].hlpl[index].setid[k]].count ++;
              }
              /*printf("\n");*/
            }
          }
        }
        else if(highlowid[bankid]!=-1)
        {
          printf("dmlib: ERROR: unknown highlowid[%d]=%d\n",
                                                    bankid,highlowid[bankid]);
          exit(0);
        }
E 2

D 2
        ptr1+=ncol ; /* goto next row */
      }
    }
    else if(lfmt==2)
    {
      ptr2 = (unsigned short *)&jw[ind-1];
E 2
I 2
/*TODO: MUST RECOGNIZE B16 from B32 etc - different HI/LI ID's !!!???*/
/*RIGHT NOW WORKS FOR B16 ONLY !!!???*/


      ptr2 = (unsigned short *)&jw[ind];
E 2
      end2 = ptr2 + (ncol*nrow);
      /*printf("ptr2=0x%08x(%d) end2=0x%08x(%d)\n",
        ptr2,(unsigned int)ptr2,end2,(unsigned int)end2);*/
      for(k=0; k<nrow; k++)
      {
		/*printf("bankid=%d highlowid=%d\n",bankid,highlowid[bankid]);*/
D 2
        if(highlowid[bankid]==2)
E 2
        {
          lowid = (*ptr2)&0xFF;
          highid = ((*ptr2)>>8)&0xFF;
D 2
          for(j=1; j<ncol; j++)
E 2
I 2
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
E 2
          {
            place = j;
D 2
            index = INDEX2;
			/*printf("index: %d %d\n",index,maxindex);*/
            if(index>=maxindex) break;
            if(tree[treeid].hlpl[index].nsets>0)
E 2
I 2
 
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


I 3

#ifdef USE_TREE
E 3
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
E 2
            {
D 2
			  /*
    printf("name >%4.4s< bankid=%d number=%d -> treeid=%d\n",
                           &jw[ind-1-INAM], bankid, banknum, treeid);
    printf("   ncol=%d nrow=%d lfmt=%d\n",ncol,nrow,lfmt);
              printf("------- yes2: index=%d, nsets=%d\n",
                index,tree[treeid].hlpl[index].nsets);
			  */
              for(k=0; k<tree[treeid].hlpl[index].nsets; k++)
              {
                printf(" %d",tree[treeid].hlpl[index].setid[k]);
                sets[tree[treeid].hlpl[index].setid[k]].count ++;
              }
              /*printf("\n");*/
            }
E 2
I 2
              itmp = dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[kk];
              /*printf("%d ",itmp);*/
              sets[itmp].count ++;
	        }
            /*printf("\n");*/
E 2
          }
I 2
		}
	  }
	}
  }
I 3
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


E 3
  /*
  printf("[%d] [%d] [%d] [%d] -> %d %d\n",bankid,banknum,highid,lowid,
		 sets[tree[treeid].hlpl[index].setid[kk]].count,
         dmbankid[bankid].banknum[banknum].highid[highid].lowid[lowid].setid[kk].count);
  */

          }
E 2
        }
D 2
        else if(highlowid[bankid]!=-1)
        {
          printf("dmlib: ERROR: unknown highlowid[%d]=%d\n",
                                                    bankid,highlowid[bankid]);
          exit(0);
        }
E 2

        ptr2+=ncol ; /* goto next row */
      }

D 2
    }
    else if(lfmt==4)
    {
      printf("dmlib: NEVER COME HERE !!!\n");
      exit(0);
      ptr4 = (unsigned char *)&jw[ind-1];
      end4 = ptr4 + (ncol*nrow);
      printf("ptr4=0x%08x(%d) end4=0x%08x(%d)\n",
      ptr4,(unsigned int)ptr4,end4,(unsigned int)end4);
    }
E 2

D 2

E 2
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

E 1
