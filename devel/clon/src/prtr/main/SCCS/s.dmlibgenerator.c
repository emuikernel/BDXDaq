h62591
s 00007/00000/00582
d D 1.6 10/07/25 02:38:48 boiarino 6 5
c add chmod
c 
e
s 00031/00040/00551
d D 1.5 10/07/24 15:49:29 boiarino 5 4
c fix bug
c 
e
s 00002/00002/00589
d D 1.4 10/07/24 14:50:56 juang 4 3
c from Juan
e
s 00002/00002/00589
d D 1.3 10/07/21 13:01:42 juang 3 2
c *** empty log message ***
e
s 00541/00000/00050
d D 1.2 10/07/19 23:26:25 boiarino 2 1
c *** empty log message ***
e
s 00050/00000/00000
d D 1.1 10/07/18 23:44:30 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

/* dmlibgenerator.c - generates *.dmcfg files */

#include <stdio.h>
I 6
#include <sys/stat.h>
E 6
#include "bosio.h"
#include "bos.h"
#include "dmlib.h"

I 2
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



#define NCONFIGLINES 25000

#define PROCESS_OBJECT(object,OBJ,nOBJ,OBJgrouping) \
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
             OBJ[nOBJ-1].number==object && \
             OBJ[nOBJ-1].sector==sector && \
             OBJ[nOBJ-1].layer==layer && \
             OBJ[nOBJ-1].wire==(wire-1)) \
		  { \
			/*printf("2\n");*/ \
            OBJ[nOBJ-1].wire = wire; /* keep filling group */ \
		  } \
          else if(OBJgrouping==1 && \
			 (OBJ[nOBJ].number!=object || \
              OBJ[nOBJ].sector!=sector || \
              OBJ[nOBJ].layer!=layer || \
              OBJ[nOBJ].wire!=(wire-1))) \
		  { \
			/*printf("3\n");*/ \
            OBJgrouping=0; /* close grouping */ \
            OBJ[nOBJ].number = object; \
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
          else \
		  { \
			/*printf("4\n");*/ \
            OBJ[nOBJ].number = object; \
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
		}

#define PROCESS_SUBOBJECT(object,subobject,OBJ,nOBJ,OBJgrouping) \
        if(nOBJ<NCONFIGLINES) \
		{ \
          if(nOBJ>1 && \
             OBJgrouping==0 && \
             OBJ[nOBJ-2].parent==OBJ[nOBJ-1].parent && \
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
             OBJ[nOBJ-1].parent==object && \
             OBJ[nOBJ-1].number==subobject && \
             OBJ[nOBJ-1].sector==sector && \
             OBJ[nOBJ-1].layer==layer && \
             OBJ[nOBJ-1].wire==(wire-1)) \
		  { \
			/*printf("2\n");*/ \
            OBJ[nOBJ-1].wire = wire; /* keep filling group */ \
		  } \
          else if(OBJgrouping==1 && \
			 (OBJ[nOBJ].parent!=object || \
			  OBJ[nOBJ].number!=subobject || \
              OBJ[nOBJ].sector!=sector || \
              OBJ[nOBJ].layer!=layer || \
              OBJ[nOBJ].wire!=(wire-1))) \
		  { \
			/*printf("3\n");*/ \
            OBJgrouping=0; /* close grouping */ \
            OBJ[nOBJ].parent = object; \
            OBJ[nOBJ].number = subobject; \
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
          else \
		  { \
			/*printf("4\n");*/ \
            OBJ[nOBJ].parent = object; \
            OBJ[nOBJ].number = subobject; \
            OBJ[nOBJ].sector = sector; \
            OBJ[nOBJ].layer = layer; \
            OBJ[nOBJ].wire = wire; \
            nOBJ ++; \
		  } \
		}

/* get hardware maps from database */ 
E 2
void
I 2
dmdatabase()
{
  FILE *fd;
  int i, ii, jj, jjj, jjjj;
  const char *host = "clasdb";
  const char *database = "dc_chan_status";
  const char *user = "clasuser";
  const char *passwd = "";
  unsigned int port = 0;
  const char *unix_socket = "";
  unsigned long client_flag = 0;

  MYSQL *mysql;
  MYSQL_RES *result;
  MYSQL_ROW row_out;
  char query[8192];
  char specific_request[4096];
  char fname[256];

  int sector, layer, wire;
  int adb_crate, adb_board, tdc_crate, tdc_board, lv_supply, lv_supply_old, lv_fuse;

  int nADBcrate, ADBgrouping;
  ADBCRATE ADBcrate[NCONFIGLINES];

  int nADBboard, ADBboardgrouping;
  ADBBOARD ADBboard[NCONFIGLINES];

  int nTDCcrate, TDCgrouping;
  TDCCRATE TDCcrate[NCONFIGLINES];

  int nTDCboard, TDCboardgrouping;
  ADBBOARD TDCboard[NCONFIGLINES];

  int nLVcrate, LVgrouping;
  LVCRATE LVcrate[NCONFIGLINES];

  int nFUSE, FUSEgrouping;
  LVFUSE FUSE[NCONFIGLINES];







  /*initialize MYSQL structure*/
  mysql = mysql_init(NULL);
  if(mysql == NULL)
  {
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






  /*****************************************/
  /* gets everything we need from database */

  nADBcrate = 0;
  ADBgrouping = 0;

  nADBboard = 0;
  ADBboardgrouping = 0;

  nTDCcrate = 0;
  TDCgrouping = 0;

  nTDCboard = 0;
  TDCboardgrouping = 0;

  nLVcrate = 0;
  LVgrouping = 0;

  nFUSE = 0;
  FUSEgrouping = 0;

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
    {
      printf("mysql_store_result returns NULL, do nothing\n");
    }
    else
    {
	  /*printf("456\n");fflush(stdout);*/
     row_out = mysql_fetch_row(result);
     /*printf("789: 0x%08x\n",row_out);fflush(stdout);*/

      /* get run number */
      if(row_out==NULL)
      {
        /*printf("NO\n")*/;
      }
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
		lv_supply_old = lv_supply;
        lv_supply = (atoi(row_out[15])-1)*3 + atoi(row_out[16]);
        lv_fuse = atoi(row_out[17]);
		/*
        lv_fuse = (lv_supply-1)*24 + lv_fuse;
		*/
		/*
        printf("sector %d, layer %2d, wire %3d --->",sector,layer,wire);
        printf(" ADB crate >%2s< {%d}, ADB board >%2s< {%d}",row_out[0],adb_crate,row_out[1],adb_board);
        printf(" TDC crate >%2s< {%d}, TDC board >%2s< {%d}",row_out[8],tdc_crate,row_out[9],tdc_board);
        printf(" LV supply >%1s_%1s< {%d}, LV fuse >%2s< {%d}\n",row_out[15],row_out[16],lv_supply,row_out[17],lv_fuse);
		*/
		/*
		printf("%d %d %d -> %d (ADBgrouping=%d)\n",sector,layer,wire,adb_board,ADBgrouping);
		*/

D 5
        if(lv_supply_old>0 && lv_supply != lv_supply_old)
		{
		  /* FUSE files */
          ii = lv_supply;
          for(jj=1; jj<=24; jj++)
          {
            /*check if our fuse exist*/
            jjjj=0;
            for(jjj=0; jjj<nFUSE; jjj++)
			{
              if(FUSE[jjj].number==jj) {jjjj=1;break;}
			}
            if(jjjj==0) continue;
E 5

D 5
            sprintf(fname,"dc_fuse_%02d_%02d.dmcfg",ii,jj);
            fd = fopen(fname,"w");
            if(fd!=NULL)
            {
              fprintf(fd,"#\n# configuration file for DC box %02d fuse %02d\n#\nNAME DCFUSE_%02d_%02d  \"BOX %02d FUSE %02d\"\n#\nNORM\n",
D 4
                ii,jj,ii,jj,ii,jj);
E 4
I 4
                ii,jj,ii-1,jj,ii,jj);
E 4
              fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
              for(i=0; i<nFUSE; i++)
              {
                if(FUSE[i].number==jj) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
                  FUSE[i].sector,FUSE[i].layer,FUSE[i].wire);
  	          }
              fprintf(fd,"#\nSTOP\n#\n");
              fclose(fd);
            }
          }

          nFUSE = 0;
          FUSEgrouping = 0;
		}


E 5
        PROCESS_OBJECT(adb_crate,ADBcrate,nADBcrate,ADBgrouping);
        PROCESS_OBJECT(tdc_crate,TDCcrate,nTDCcrate,TDCgrouping);
        PROCESS_OBJECT(lv_supply,LVcrate,nLVcrate,LVgrouping);
D 5
        PROCESS_OBJECT(lv_fuse,FUSE,nFUSE,FUSEgrouping);
E 5
I 5
        /*PROCESS_OBJECT(lv_fuse,FUSE,nFUSE,FUSEgrouping);*/
        PROCESS_SUBOBJECT(lv_supply,lv_fuse,FUSE,nFUSE,FUSEgrouping);
E 5
        PROCESS_SUBOBJECT(adb_crate,adb_board,ADBboard,nADBboard,ADBboardgrouping);
        PROCESS_SUBOBJECT(tdc_crate,tdc_board,TDCboard,nTDCboard,TDCboardgrouping);
      }
    }

    mysql_free_result(result);
  }


  /************************/
  /* create *.dmcfg files */
  
  /* ADB crate files */
  for(ii=1; ii<=30; ii++)
  {
    sprintf(fname,"dc_adb_crate_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
D 3
      fprintf(fd,"#\n# configuration file for DC ADB crate %02d\n#\nNAME DCADBCRATE%02d  \"ADB crate %02d\"\n#\nNORM\n",ii,ii,ii);
E 3
I 3
      fprintf(fd,"#\n# configuration file for DC ADB crate %02d\n#\nNAME DCADBCRATE_%02d  \"ADB crate %02d\"\n#\nNORM\n",ii,ii,ii);
E 3
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

  /* ADB boards files */
  for(ii=1; ii<=30; ii++)
  {
  for(jj=1; jj<=12; jj++)
  {
    sprintf(fname,"dc_adb_board_%02d_%02d.dmcfg",ii,jj);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
      fprintf(fd,"#\n# configuration file for DC ADB crate %02d board %02d\n#\nNAME DCADBBOARD_%02d_%02d  \"ADB crate %02d board %02d\"\n#\nNORM\n",
        ii,jj,ii,jj,ii,jj);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nADBboard; i++)
      {
        if(ADBboard[i].parent==ii && ADBboard[i].number==jj) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          ADBboard[i].sector,ADBboard[i].layer,ADBboard[i].wire);
  	}
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }
  }

  /* TDC crate files */
  for(ii=1; ii<=11; ii++)
  {
    sprintf(fname,"dc_tdc_crate_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
D 3
      fprintf(fd,"#\n# configuration file for DC TDC crate %02d\n#\nNAME DCTDCCRATE%02d  \"TDC crate %02d\"\n#\nNORM\n",ii,ii,ii);
E 3
I 3
      fprintf(fd,"#\n# configuration file for DC TDC crate %02d\n#\nNAME DCTDCCRATE_%02d  \"TDC crate %02d\"\n#\nNORM\n",ii,ii,ii);
E 3
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

  /* TDC boards files */
  for(ii=1; ii<=11; ii++)
  {
  for(jj=1; jj<=25; jj++)
  {
    /*check if our board exist*/
    jjjj=0;
    for(jjj=0; jjj<nTDCboard; jjj++)
	{
      if(TDCboard[jjj].parent==ii && TDCboard[jjj].number==jj) {jjjj=1;break;}
	}
    if(jjjj==0) continue;

    sprintf(fname,"dc_tdc_board_%02d_%02d.dmcfg",ii,jj);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
      fprintf(fd,"#\n# configuration file for DC TDC crate %02d board %02d\n#\nNAME DCTDCBOARD_%02d_%02d  \"TDC crate %02d board %02d\"\n#\nNORM\n",
        ii,jj,ii,jj,ii,jj);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nTDCboard; i++)
      {
        if(TDCboard[i].parent==ii && TDCboard[i].number==jj) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          TDCboard[i].sector,TDCboard[i].layer,TDCboard[i].wire);
  	}
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }
  }

  /* LV supply files */
  for(ii=1; ii<=18; ii++)
  {
    sprintf(fname,"dc_lv_supply_%02d.dmcfg",ii);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
D 4
      fprintf(fd,"#\n# configuration file for DC LV supply %02d\n#\nNAME DCLVSUPPLY%02d  \"LV supply %02d\"\n#\nNORM\n",ii,ii,ii);
E 4
I 4
      fprintf(fd,"#\n# configuration file for DC LV supply %02d\n#\nNAME DCLVSUPPLY_%02d  \"LV supply %02d\"\n#\nNORM\n",ii,ii,ii);
E 4
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


I 5
  /* FUSE files */
  for(ii=1; ii<=18; ii++)
  {
  for(jj=1; jj<=24; jj++)
  {
    /*check if our fuse exist*/
    jjjj=0;
    for(jjj=0; jjj<nFUSE; jjj++)
	{
      if(FUSE[jjj].parent==ii && FUSE[jjj].number==jj) {jjjj=1;break;}
	}
    if(jjjj==0) continue;
E 5

I 5
    sprintf(fname,"dc_fuse_%02d_%02d.dmcfg",ii,jj);
    fd = fopen(fname,"w");
    if(fd!=NULL)
    {
I 6
      chmod(fname,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
E 6
      fprintf(fd,"#\n# configuration file for DC box %02d fuse %02d\n#\nNAME DCFUSE_%02d_%02d  \"BOX %02d FUSE %02d\"\n#\nNORM\n",
                ii,jj,ii,jj,ii,jj);
      fprintf(fd,"#\n#   bankname  sector  layer   wire    place\n#\n");
      for(i=0; i<nFUSE; i++)
      {
        if(FUSE[i].parent==ii && FUSE[i].number==jj) fprintf(fd,"BANK   DC0 %6d %7d %7d       1\n",
          FUSE[i].sector,FUSE[i].layer,FUSE[i].wire);
  	  }
      fprintf(fd,"#\nSTOP\n#\n");
      fclose(fd);
    }
  }
  }
E 5


D 5




E 5

  mysql_close (mysql);

  return;
}


void
E 2
main()
{
  int i,j;
  FILE *fptr;
  int sect=1,wir=1,lyr=1,suplyr=1;
  char fname[128];
  
  dmdatabase();
  exit(0);


  for(sect=1; sect<=1/*6*/; sect++)
	for(suplyr=1; suplyr<=6; suplyr++)
      for(i=1; i<=32; i++)
  {

  sprintf(fname, "dc_%d_%1d_%02d.dmcfg",sect,suplyr,i);
  fptr = fopen(fname, "w");
  if(fptr!=NULL)
  {
    fprintf(fptr,"#\n# configuration for 6x6\n#\n");
    fprintf(fptr,"# NAME XYZZ where X is sector ()1-6, Y is superlayer (1-6), ZZ are groups (1-32)\n");
    fprintf(fptr,"#\nNAME DC%d%d0%d \"bla\"  \n#\n#    bankname   sector   layer   wire     zero\n",sect,suplyr,i);
	for(j=1; j<=6; j++)
	{
      lyr = (suplyr-1)*6 + j;
      wir= i*6;
      fprintf(fptr,"BANK   DC0  %7d   %7d %7d       0\n",sect,lyr,wir-5);
      fprintf(fptr,"BANK   DC0  %7d   %7d %7d       0\n",sect,lyr,wir);
	}
    fprintf(fptr,"#\nSTOP\n#\n");
    fclose(fptr);
  }
  else
  {
     printf("unable to create file\n");
  }
  }

  exit(0);
}
E 1
