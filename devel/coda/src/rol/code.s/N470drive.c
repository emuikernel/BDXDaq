
/* v288com.c */

#ifdef VXWORKS

/*---------------------------------------------------------*/
/*-XACH----------------------------------------------------*/ 
/*-----Driver for v288  vme board caenet controller--------*/         
/*-----Auther Vardan Gyurjyan------------------------------*/                 
/*---------------------------------------------------------*/

#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* mvme2306 */
#define V288ADR 0xfa100000

#ifdef VXWORKSPPC
volatile UINT16 *v288adr =  (UINT16 *)  V288ADR;
volatile UINT16 *statreg =  (UINT16 *) (V288ADR+2);
volatile UINT16 *transreg = (UINT16 *) (V288ADR+4);
volatile UINT16 *resetreg = (UINT16 *) (V288ADR+6);
#else
UINT16 *v288adr = (UINT16 *)0xf0000000;
UINT16 *statreg = (UINT16 *)0xf0000002;
UINT16 *transreg = (UINT16 *)0xf0000004;
UINT16 *resetreg = (UINT16 *)0xf0000006;
#endif

#define QQ (UINT16)0xfffe
#define TIMEOUT 1111
#define OK 0
#define MEK (UINT16)1

/*
#define MONHOP 500000
#define GMONHOP 500000
#define GSORTMONHOP 50000
#define SETHOP 1500000
#define GRSETHOP 1500000
#define ADRMHOP 5000000
*/
#define MONHOP 3000000
#define GMONHOP 1000000
#define GSORTMONHOP 100000
#define SETHOP 30000000
#define GRSETHOP 3000000
#define ADRMHOP 10000000

#define RESET_ERR 101
#define WAIT_ERROR 102


static FILE *m_gpf;

char charname[21];
static int name[800];

/*---------------------------------------------------------*/
/*-------------- Function prototype -----------------------*/
/*---------------------------------------------------------*/
void aspeta(int spas);
int caen_wait(int spas);
int caen_read(int spas);
int send_comm(volatile UINT16 *vmeaddress, UINT16 vmedat, int spas);
int caen_reset(void);
int set_mon(char *group_file, int mode, UINT16 code, UINT16 com_value);
int grp_read(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code);
int grp_adrm(char *group_file,UINT16 code, UINT16 group);
int grp_set(int crate_num, UINT16 group, UINT16 code, UINT16 value);
int set_par(char *group_file, UINT16 code);
int grp_paread(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code);
void driven470(UINT16 id, UINT16 channel, UINT16 oper, UINT16 value);
void n470reset();

/*---------------------------------------------------------*/
/*--------------------- function delay --------------------*/
/*---------------------------------------------------------*/


#define NTICKS 1000 /* the number of ticks per second */

IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
int    sysClkRateGet();
STATUS sysClkRateSet(int);

/* sleep() for vxworks - delay n * 10(msec) */
void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}



void
aspeta(int spas)
{
  int jj;

  /*printf("aspeta reached\n");*/

  while(spas--) jj++;
}

/*---------------------------------------------------------*/
/*---- CAEN Wait function -------------------------------- */
/*---------------------------------------------------------*/
int
caen_wait(int spas)
{
  int i=0;
  UINT16 q=0;
  UINT16 vmedat;

  /*printf("caen_wait reached\n");*/

  /*----- wait for responce ---------------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
	aspeta(spas);
    vmedat = *v288adr;
    aspeta(spas);
    if((q=*statreg) == QQ)
    {
      if(i>=TIMEOUT) printf("caen_wait_error: %x \n",i);
      return(vmedat);
      break;
    }
    i++;
  }
  puts("caen_wait_error: OOPS");

  return WAIT_ERROR;
}

/*---------------------------------------------------------*/
/*---- Wait and Read caen data function ------------------ */
/*---------------------------------------------------------*/
int
caen_read(int spas)
{
  int i=0;
  UINT16 q=0;
  UINT16 vmedat;

  /*printf("caen_read reached\n");*/
  
  /*------ wait for responce --------------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
	aspeta(spas);
    vmedat = *v288adr;
    aspeta(spas);
    if((q=*statreg)==QQ)
    {
      fprintf(m_gpf," %5d",vmedat);
      break;
    }
    i++;
  }

  /*------- read all valid  data ----------------------------*/
  for(;;)                      
  {
	aspeta(spas);
    vmedat = *v288adr;
    aspeta(spas);
    if((q=*statreg)!=QQ) break;
    fprintf(m_gpf," %5d",vmedat);
  }
  fprintf(m_gpf,"\n");

  return(0);
}

/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
int
send_comm(volatile UINT16 *vmeaddress, UINT16 vmedat, int spas)
{
  int i=0;
  UINT16 q=0;

  /*printf("send_comm reached\n");*/

  while(q!=QQ && i<=TIMEOUT)
  {
	aspeta(spas);
    *vmeaddress = vmedat;
    aspeta(spas);
    q = *statreg;
    i++;
  }
  if(i>=TIMEOUT) printf("error code: %d \n",i);
  /*printf("q=0x%08x\n",q);*/

  return((i==TIMEOUT) ? TIMEOUT : OK);
}

/*---------------------------------------------------------*/
/*------Reset_restart mod for caenet controller -----------*/ 
/*---------------------------------------------------------*/
int
caen_reset(void)
{
  int i=0;
  UINT16 q=0;

  /*printf("caen_reset reached\n");*/

  while(q!=QQ && i<=11)
  {
	aspeta(ADRMHOP);
    *resetreg = MEK;
    aspeta(ADRMHOP);
    q = *statreg;
    /*printf("q=0x%08x\n",q);*/
    i++;
  }
  if(i>11) printf("error code: %d, q=0x%08x\n",i,q);

  return((i==11) ? 11 : OK);
}

void
n470reset(){
int tmp = 0;

  /* wrtie into transmit register*/
  if(( tmp=send_comm(resetreg, MEK,SETHOP)) == TIMEOUT)
    puts("set_error: transmission register access failed");
}
/**
 * Driver for N470 module
 *
 * @param id of the module obtaind manualyy using F99 front panel function
 * @param channel number
 * @param operation code.Look the table 5 of the N470 manual
 * @param value of the set operation.
 *
 * @author Vardan Gyurjyan 05/24/2005
 ************************************************************************/
void
driven470(UINT16 id, UINT16 channel,UINT16 oper,UINT16 value){
  
  UINT16 opcode;
  int i=0;
  UINT16 q=0;
  UINT16 vmedat;
  int tmp = 0;

  /* write controller identification number */
  if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
    puts("set_fatal error: controller identifier code");
  
  /* write address number of the module*/
  if(( tmp=send_comm(v288adr,id,SETHOP))== TIMEOUT )
    puts("set_fatal error: n470 identification number, obtained by F99");
  
  /* write code of the operation*/
  opcode = ((UINT16)channel << 8) | (UINT16)oper;
  /*   printf("opcode = 0x%08x\n",opcode);*/

  if(( tmp=send_comm(v288adr,opcode,SETHOP))== TIMEOUT )
    puts("set_fatal error: code of operation, Table 5, N470 manual");
  
  if(oper>2&&oper<10)
  {
    /*write the set vale*/
    if(( tmp=send_comm(v288adr,value,SETHOP))== TIMEOUT )
      puts("set_fatal error: value");
  }

  /* wrtie into transmit register*/
  if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
    puts("set_error: transmission register access failed");
  
  /*wait*/
  if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
    printf("set_error: error code from slave = %x  %d\n",tmp,i);
  
   
  
  if(oper<=2){
    /*------ wait for responce --------------------------------*/
     printf("Data: ");
    while(i<=TIMEOUT && q!=QQ)     
      {
	aspeta(MONHOP);
	vmedat = *v288adr;
	/*	 printf(" %5d \n",vmedat);*/
	aspeta(MONHOP);
	q=*statreg;
	/*printf("q=0x%08x\n",q);*/
	if(q==QQ)
	  {
	    printf(" %5d",vmedat);
	    break;
	  }
	i++;
      }
    
    /*------- read all valid  data ----------------------------*/
    for(;;)                      
      {
	aspeta(MONHOP);
	vmedat = *v288adr;
	/*	 printf(" %5d \n",vmedat);*/
	aspeta(MONHOP);
	q=*statreg;
	/*	printf("q=0x%08x\n",q);*/
	if(q!=QQ) break;
	printf(" %5d",vmedat);
      }
  }else {
    printf("done\n");
  }
}


/*---------------------------------------------------------*/ 
/*-Set and monitor entier conf  file ----------------------*/
/*-----sintax :--------------------------------------------*/ 
/*-setmon( conf_file_name, set=111/mon=333, code, value)---*/
/*-This function is calling the function  caen_read.-------*/
/*---------------------------------------------------------*/
int
set_mon(char *group_file, int mode, UINT16 code, UINT16 com_value)
{
  FILE *gpf_adr;
  char charname[21];
  int memb_ind = 0;
  static int name[800]; 
  static int crate_num[800], board_chan[800];
  int board, channel;
  int ch,i,tmp=0;

  /*printf("set_mon reached\n");*/

  puts("START");
/*------ read the config file for the group ---------------*/
  if((gpf_adr = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  else
  {
    while((ch=getc(gpf_adr)) != EOF)
    {
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d", charname, 
        &name[memb_ind], &crate_num[memb_ind], &board, &channel);
      board_chan[memb_ind] = ((UINT16)board << 8) | (UINT16)channel;
    }
  }
  fclose(gpf_adr);

  if(mode==111) 
  {
    /*---------- setting --------------------------------------*/
    for(i=1;i<=memb_ind;i++) 
    {
      if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
        puts("set_fatal error: controller identifier code");
   
      if(( tmp=send_comm(v288adr,crate_num[i],SETHOP))== TIMEOUT )
        puts("set_fatal error: can't see CAEN crate");
  
      if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
        puts("set_error: operation code failed");
  
      if(( tmp=send_comm(v288adr,board_chan[i],SETHOP))== TIMEOUT )
        puts("set_error: board_chan to buffer failed");

      if(( tmp=send_comm(v288adr,com_value,SETHOP))== TIMEOUT )
        puts("set_error: value to buffer failed");

      if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
        puts("set_error: transmission register access failed");

      if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
        printf("set_error: error code from slave = %x  %d\n",tmp,i);
    }
  }
  else 
  {
    /*------------------ monitoring --------------------------*/
    if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
    {
      puts("Can't open dc_mon.dat");
    }
    else
    {
      printf("DEBUG: %d \n",memb_ind); 
    }

    for(i=1;i<=memb_ind;i++) 
    {
      if(( tmp=send_comm(v288adr,1,MONHOP)) == TIMEOUT )
        printf("mon_fatal error: controller identifier code %d \n",tmp);
   
      if(( tmp=send_comm(v288adr,crate_num[i],MONHOP)) == TIMEOUT )
        printf("mon_fatal error: can't see CAEN crate %d \n", tmp);
  
      if(( tmp=send_comm(v288adr,code,MONHOP)) == TIMEOUT )
        printf("mon_error: operation code failed %d \n",tmp);
  
      if(( tmp=send_comm(v288adr,board_chan[i],MONHOP))== TIMEOUT )
        printf("mon_error: board_chan to buffer failed %d \n", tmp);
    
      if(( tmp=send_comm(transreg, MEK,MONHOP)) == TIMEOUT)
        printf("mon_error: transmission register access failed %d \n",tmp);

      fprintf(m_gpf,"%3d ",name[i]);  
      tmp=caen_read(MONHOP);
    }
    fclose(m_gpf);
  }
  puts("STOP");

  return(0);
}

/*---------------------------------------------------------*/
/*-------------- Group statusmon function -----------------*/
/*------ it works only for one CAEN crate -----------------*/
/*---------------------------------------------------------*/
/*
exec grp_read("c1_all.iconf",1,9,65)
*/
int
grp_read(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code)
{
  int tmp=0;
  FILE *gpf_adr;
  UINT16 vmedat;
  int ch,loop;
  int i=0, j=1, k=0, memb_ind=0;
  UINT16 q=0;
  int z;
/*
  char charname[21];
  static int name[800]; 
*/

  /*printf("grp_read reached\n");*/

  for(loop=0; loop<=20; loop++) charname[loop] = ' ';
  for(loop=0; loop<=799; loop++) name[loop] = 0;

  /*-------- read the config file for the group --------------*/
  if((gpf_adr = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  else
  {
    while ((ch=getc(gpf_adr)) != EOF)
    {
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
    }
  }
  fclose(gpf_adr);

  /*----------organize the reading-----------------------------*/
  if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
  {
    puts("Can't open dc_mon.dat");
  }
  else
  {
    printf("Open 'dc_mon.dat' file for 'organize the reading'\n"); 
  }

  if(( tmp=send_comm(v288adr,1,GMONHOP)) == TIMEOUT )
    puts("grpmon_fatal error: controller identifier code");
   
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP)) == TIMEOUT )
    puts("grpmon_fatal error: can't see CAEN crate");
  
  if(( tmp=send_comm(v288adr,code,GMONHOP)) == TIMEOUT )
    puts("grpmon_error: operation code failed");

  if(( tmp=send_comm(v288adr,group,GMONHOP)) == TIMEOUT )
    puts("grpmon_error: group to buffer failed");

  if(( tmp=send_comm(transreg,MEK,GMONHOP)) == TIMEOUT)
    printf("grpmm_gpfon_error: transmission register access failed %d\n",tmp);

  /*---- format the output---------------------- -------------*/
  /*------------ wait for responce ---------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
	aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if((q = *statreg)==QQ)
    {
      /*printf("vmedat=0x%08x q=0x%08x ! (name[1]=%3d)\n",vmedat,q,name[1]);*/
      fprintf(m_gpf,"%3d",name[1]);
      /*--------- to be the same output as for set_mon -----------*/
      fprintf(m_gpf,"%5d",0); 
      break;
    }
    /*printf("q=0x%08x ...\n",q);*/
    i++;
  }


  /*----------------- read all valid  data -------------------*/
  for(;;)
  {

    /* */
	aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if( (q=*statreg) != QQ )
    {
      /*printf("q=0x%08x - break\n",q);*/
      break;
    }

    /* */
    k++;
    if(k>5) 
    {
      k=1; 
      j++;
      fprintf(m_gpf,"\n");
      fprintf(m_gpf,"%3d",name[j]);

      /*---------- to be the same output as for set_mon ----------*/
      fprintf(m_gpf,"%5d",0);        
      fprintf(m_gpf," %5d",vmedat);
    }
    else
    {
      fprintf(m_gpf," %5d",vmedat);
    }

  }
  fprintf(m_gpf,"\n");

  fclose(m_gpf);

  return(0);
}

/*---------------------------------------------------------*/
/*------ Add or remove channel from the group -------------*/
/*---------------------------------------------------------*/
int
grp_adrm(char *group_file,UINT16 code, UINT16 group)
{
  FILE *gpf_adr;
  char charname[21];
  int memb_ind = 0;
  static int name[800];
  static int gcrate_num, gboard_chan[800];
  int board, channel;
  int ch,i,tmp=0;

  /*printf("grp_adrm reached\n");*/

  if((gpf_adr = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  else 
  {
    while((ch=getc(gpf_adr)) != EOF)
    {
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d",
        charname, &name[memb_ind], &gcrate_num, &board, &channel);
      gboard_chan[memb_ind] = ((UINT16)board << 8) | (UINT16)channel;
    }
  }
  fclose(gpf_adr);

  for(i=1;i<=memb_ind;i++) 
  {
    if(( tmp=send_comm(v288adr,1,ADRMHOP))== TIMEOUT )
      puts("grpadrm_fatal error: controller identifier code");

    if(( tmp=send_comm(v288adr,gcrate_num,ADRMHOP))== TIMEOUT )
      puts("grpadrm_fatal error: can't see CAEN crate");

    if(( tmp=send_comm(v288adr,code,ADRMHOP))== TIMEOUT )
      puts("grpadrm_error: operation code failed");

    if(( tmp=send_comm(v288adr,group,ADRMHOP))== TIMEOUT )
      puts("grpadrm_error: operation code failed");

    if(( tmp=send_comm(v288adr,gboard_chan[i],ADRMHOP))== TIMEOUT )
      puts("grpadrm_error: board_chan to buffer failed");

    if(( tmp=send_comm(transreg, MEK,ADRMHOP)) == TIMEOUT)
      puts("grpadrm_error: transmission register access failed");

    if((tmp=caen_wait(ADRMHOP)) == WAIT_ERROR)
      printf("grp_adrm: error code from slave = %x  %d\n",tmp,i);
  }

  return(0);  
}

/*---------------------------------------------------------*/
/*------------ group set operations -----------------------*/
/*--------- works for 1 crate -----------------------------*/
/*---------------------------------------------------------*/
int
grp_set(int crate_num, UINT16 group, UINT16 code, UINT16 value)
{
  int tmp=0;

  /*printf("grp_set reached\n");*/

  if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
    puts("grpset_fatal error: controller identifier code");
   
  if(( tmp=send_comm(v288adr,crate_num,GRSETHOP))== TIMEOUT )
    puts("grpset_fatal error: can't see CAEN crate");
  
  if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
    puts("grpset_error: operation code failed");

  if(( tmp=send_comm(v288adr,group,GRSETHOP))== TIMEOUT )
    puts("grpset_error: group to buffer failed");

  if(value>0) 
  {
    if(( tmp=send_comm(v288adr,value,GRSETHOP))== TIMEOUT )
      puts("grpset_error: value to buffer failed");
  }

  if(( tmp=send_comm(transreg, MEK,GRSETHOP)) == TIMEOUT)
    printf("grpset_error: transmission register access failed %d \n",tmp);

  if((tmp=caen_wait(GRSETHOP)) == WAIT_ERROR)
    printf("grp_set: error code from slave = %x \n",tmp);

  return(0);  
}

/*---------------------------------------------------------*/
/*- Function will set parameters on the conf_file members,-*/ 
/*- taking the current values from setdat.txt. ------------*/
/*- Function will find in the group file the "as" and will-*/
/*- set on current as his corresponding value.-------------*/
/*---------------------------------------------------------*/
int
set_par(char *group_file, UINT16 code)
{
  FILE *gpf_name, *gpf_value;
  char charname[21],*setname;
  int tmp = 0;
  int name; 
  int crate_num, board, channel, board_chan;
  int ch;
  int vfa, vfs, vsa, vss, vga, vgs;
  int valueas = 0;
  int region, sector, teta, as;

  /*printf("set_par reached\n");*/

  /*---------- read the parameters file ---------------------*/
  if(code == 16)       setname = "setdat/v0dat.txt";
  else if (code == 17) setname = "setdat/v1dat.txt";
  else if (code == 18) setname = "setdat/i0dat.txt";
  else if (code == 19) setname = "setdat/i1dat.txt";
  else if (code == 20) setname = "setdat/vmaxdat.txt";
  else if (code == 21) setname = "setdat/rupdat.txt";
  else if (code == 22) setname = "setdat/rdwndat.txt";
  else if (code == 23) setname = "setdat/tripdat.txt";
  else return(0);

  if((gpf_value = fopen(setname,"r"))==NULL)
  {
    printf("Can't open file %s \n",setname);
    return(0);
  }
  else 
  {
    while((ch=getc(gpf_value)) != EOF)
    {
      fscanf(gpf_value," %d %d %d %d %d %d \n", 
        &vfa,&vfs,&vsa,&vss,&vga,&vgs);
    }
  }
  fclose(gpf_value);

  /*-------- read the config file -------------- ------------*/
  if((gpf_name = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  while((ch=getc(gpf_name)) != EOF)
  {
    fscanf(gpf_name,"%s %d %d %d %d", 
      charname,&name,&crate_num, &board, &channel);
    board_chan = ((UINT16)board << 8) | (UINT16)channel;

    /*---- decoding the code_names ----------------------------*/
    region = name/10000;
    sector = (name/1000) - (region*10);
    teta = (name/100) - (region*100) - (sector*10);
    as = (name/10) - (region*1000) - (sector*100) - (teta*10);
    if (as == 1)      valueas = vfa;
    else if (as == 2) valueas = vfs;
    else if (as == 3) valueas = vsa;
    else if (as == 4) valueas = vss;
    else if (as == 5) valueas = vga;
    else if (as == 6) valueas = vgs;
    else return(0);
    printf("name = %d as = %d value = %d \n",name,as, valueas);

    if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
      puts("set_fatal error: controller identifier code");
   
    if(( tmp=send_comm(v288adr,crate_num,SETHOP))== TIMEOUT )
      puts("set_fatal error: can't see CAEN crate");
  
    if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
      puts("set_error: operation code failed");
  
    if(( tmp=send_comm(v288adr,board_chan,SETHOP))== TIMEOUT )
      puts("set_error: board_chan to buffer failed");

    if(( tmp=send_comm(v288adr,valueas,SETHOP))== TIMEOUT )
      puts("set_error: value to buffer failed");

    if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
      puts("set_error: transmission register access failed");

    if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
      printf("set_error: error code from slave = %x \n",tmp);

    valueas = 0;
  }

  return(0);
}

/*----------------------------------------------------------*/
/*---function of reding the parameters in grouping mode-----*/
/*-----------works for groups in 1 crate -------------------*/
/*----------------------------------------------------------*/
int
grp_paread(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code)
{
  FILE *gpf_adr;
  UINT16 vmedat;
  int ch;
  int i=0, j=1, k=0, memb_ind=0, tmp=0;
  UINT16 q=0;
  int z, k_step,loop;

  /*printf("grp_paread reached\n");*/

  /*-----------total number of the channels in DC ------------*/

  for(loop=0;loop<=20;loop++) charname[loop] = ' ';
  for(loop=0;loop<=799;loop++)name[loop]=0;

  /*------------find the k_step according to the code --------*/
  /*-----Group mode readings for parameters-------------------*/ 
  /*--- have as usual 3 wors. Olny code 70 have 2 words-------*/
  if(code == 70) k_step = 2;
  else           k_step = 3;

  /*-------- read the config file for the group --------------*/
  if((gpf_adr = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  else
  {
    while((ch=getc(gpf_adr)) != EOF)
    {
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
    } 
    fclose(gpf_adr);
  }

  /*----- open the file for keeping the monitored data --------*/
  if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
  {
    puts("Can't open dc_mon.dat");
  }
  else
  {
    printf("DEBUG: open file for 'keeping the monitored data'\n"); 
  }

  if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
    puts("grpmon_fatal error: controller identifier code");
   
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP))== TIMEOUT )
    puts("grpmon_fatal error: can't see CAEN crate");
  
  if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
    puts("grpmon_error: operation code failed");

  if(( tmp=send_comm(v288adr,group,GMONHOP))== TIMEOUT )
    puts("grpmon_error: group to buffer failed");

  if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
    printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);

  /*------------ wait for responce ---------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
    aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if((q=*statreg)==QQ)
    {
      fprintf(m_gpf,"%3d",name[1]);
      /*--------- to be the same output as for set_mon -----------*/
      fprintf(m_gpf,"%5d",0); 
      break;
    }
    i++;
  }

  /*----------------- read all valid  data -------------------*/
  for(;;)                      
  {
    aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if((q=*statreg)!=QQ) break;
    k++;
    if(k>k_step) 
    {
      k=1; 
      j++;
      fprintf(m_gpf,"\n");
      fprintf(m_gpf,"%3d",name[j]);
      /*---------- to be the same output as for set_mon ----------*/
      fprintf(m_gpf,"%5d",0);        
      fprintf(m_gpf," %5d",vmedat);
    }
    else
	{
      fprintf(m_gpf," %5d",vmedat);
    }
  }
  fprintf(m_gpf,"\n");
  fclose(m_gpf);

  return(0);
} 










int
test()
{
  char *group_file = "c1_all.iconf";
  UINT16 crate_num = 1;
  UINT16 group = 9;
  UINT16 code = 65;

  int tmp=0;
  FILE *gpf_adr;
  UINT16 vmedat;
  int ch,loop;
  int i=0, j=1, k=0, memb_ind=0;
  UINT16 q=0;
  int z;

  char charnam[21];
  int nam[800]; 

  printf("test reached\n");

  for(loop=0; loop<21; loop++) charnam[loop] = ' ';
  for(loop=0; loop<800; loop++) nam[loop] = 0;


  /*-------- read the config file for the group --------------*/
  if((gpf_adr = fopen(group_file,"r")) == NULL)
  {
    printf("Can't open file %s \n",group_file);
    return(0);
  }
  else
  {
    while ((ch=getc(gpf_adr)) != EOF)
    {
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d",charnam,&nam[memb_ind],&z,&z,&z);
      /*printf(">%s< %d\n",charnam,nam[memb_ind]);*/
    }
  }
  fclose(gpf_adr);




  /*----------organize the reading-----------------------------*/
  if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
  {
    puts("Can't open dc_mon.dat");
  }
  else
  {
    printf("Open 'dc_mon.dat' file for 'organize the reading'\n"); 
  }

  if(( tmp=send_comm(v288adr,1,GMONHOP)) == TIMEOUT )
    puts("grpmon_fatal error: controller identifier code");
   
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP)) == TIMEOUT )
    puts("grpmon_fatal error: can't see CAEN crate");
  
  if(( tmp=send_comm(v288adr,code,GMONHOP)) == TIMEOUT )
    puts("grpmon_error: operation code failed");

  if(( tmp=send_comm(v288adr,group,GMONHOP)) == TIMEOUT )
    puts("grpmon_error: group to buffer failed");

  if(( tmp=send_comm(transreg,MEK,GMONHOP)) == TIMEOUT)
    printf("grpmm_gpfon_error: transmission register access failed %d\n",tmp);





  /*---- format the output---------------------- -------------*/
  /*------------ wait for responce ---------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
    aspeta(GSORTMONHOP);  /* !!! need delay here !!! */
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if( (q=*statreg) == QQ )
    {
      /*printf(" vmedat=0x%08x  q=0x%08x !!! (nam[1]=%3d)\n",vmedat,q,nam[1]);*/


      /*printf("---> 0x%08x\n",vmedat);*/
      /*tsleep(1000);*/

      fprintf(m_gpf,"%3d",nam[1]);
      /*--------- to be the same output as for set_mon -----------*/
      fprintf(m_gpf,"%5d",0); 
      break;
    }
    /*printf(" vmedat=0x%08x  q=0x%08x ...\n",vmedat,q);*/
    i++;
  }



  /*----------------- read all valid  data -------------------*/
  for(;;)
  {

    /* */
	aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if( (q=*statreg) != QQ )
    {
      printf("q=0x%08x - break\n",q);
      break;
    }


    /* */
    k++;
    if(k>5) 
    {
      k=1; 
      j++;
      fprintf(m_gpf,"\n");
      fprintf(m_gpf,"%3d",nam[j]);

      /*---------- to be the same output as for set_mon ----------*/
      fprintf(m_gpf,"%5d",0);        
      fprintf(m_gpf," %5d",vmedat);
    }
    else
    {
      fprintf(m_gpf," %5d",vmedat);
    }

  }
  fprintf(m_gpf,"\n");

  fclose(m_gpf);

  return(0);
}

#else

void
N470drive_dummy()
{
  return;
}

#endif
