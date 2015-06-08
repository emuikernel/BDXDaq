h22422
s 00005/00000/01039
d D 1.9 05/06/02 15:41:33 boiarino 10 9
c add mytest
e
s 00200/00090/00839
d D 1.8 05/06/02 12:23:10 boiarino 9 8
c add N470 support; redesign whole file to allow multiple
c v288 controllers in one crate
e
s 00008/00000/00921
d D 1.7 03/12/16 22:29:00 boiarino 8 7
c .
e
s 00676/00442/00245
d D 1.6 03/09/25 18:08:14 boiarino 7 6
c modified version for PPC (mvme2306) - a number of changes
c were made to make v288 happy:
c     - use 'volatile' for board registers
c     - remove casting
c     - increase existing delays
c     - set extra delays before 'v288adr' register access
c some cosmetic as well
c 
e
s 00134/00070/00553
d D 1.5 03/09/24 15:00:49 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00622
d D 1.4 01/12/04 10:00:15 gurjyan 5 4
c *** empty log message ***
e
s 00011/00005/00612
d D 1.3 01/12/04 09:59:45 gurjyan 4 3
c *** empty log message ***
e
s 00002/00001/00615
d D 1.2 01/10/19 16:32:10 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dchv/vme/s/v288com.c
e
s 00616/00000/00000
d D 1.1 00/08/03 10:40:42 gurjyan 1 0
c date and time created 00/08/03 10:40:42 by gurjyan
e
u
U
f e 0
t
T
I 6

/* v288com.c */

E 6
I 1
/*---------------------------------------------------------*/
/*-XACH----------------------------------------------------*/ 
/*-----Driver for v288  vme board caenet controller--------*/         
/*-----Auther VHG------------------------------------------*/                 
/*-----CEBAF 05/22/97--------------------------------------*/
/*-----updated 08/08/97------------------------------------*/
/*-----added group operations 15/08/97---------------------*/                
/*-----added mixed group operations 11/22/97---------------*/
/*---------------------------------------------------------*/

I 7
/* Sergey Boyarinov (Sep 2003): modified for PPC controller:
    - use 'volatile' for board registers
    - remove casting
    - increase existing delays
    - set extra delays before 'v288adr' register access
*/

E 7
I 3
#include <vxWorks.h>
I 7
#include <taskLib.h>
E 7
E 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

D 7
#ifndef ushort
#define ushort unsigned short
#endif
E 7

I 7
D 9
#define V288ADR 0xfa000000
E 9
I 9
#define V288ADR1 0xfa000000 /* that board controls DC HV */
#define V288ADR2 0xfa100000 /* that board controls TPC HV */
E 9

E 7
I 6
D 9
#ifdef VXWORKSPPC
D 7
#define Q (ushort)0xfbfffe
ushort *v288adr = (ushort *)0xfa000000;
ushort *statreg = (ushort *)0xfa000002;
ushort *transreg = (ushort *)0xfa000004;
ushort *resetreg = (ushort *)0xfa000006;
E 7
I 7
volatile UINT16 *v288adr =  (UINT16 *)  V288ADR;
volatile UINT16 *statreg =  (UINT16 *) (V288ADR+2);
volatile UINT16 *transreg = (UINT16 *) (V288ADR+4);
volatile UINT16 *resetreg = (UINT16 *) (V288ADR+6);
E 7
#else
E 6
D 7
#define Q (ushort)0xf0fffe
D 6
#define TIMEOUT 1111
E 6
I 6
ushort *v288adr = (ushort *)0xf0000000;
ushort *statreg = (ushort *)0xf0000002;
ushort *transreg = (ushort *)0xf0000004;
ushort *resetreg = (ushort *)0xf0000006;
E 7
I 7
UINT16 *v288adr = (UINT16 *)0xf0000000;
UINT16 *statreg = (UINT16 *)0xf0000002;
UINT16 *transreg = (UINT16 *)0xf0000004;
UINT16 *resetreg = (UINT16 *)0xf0000006;
E 7
#endif
E 9

D 7

#define TIMEOUT 111111
E 7
I 7
#define QQ (UINT16)0xfffe
#define TIMEOUT 1111
E 7
E 6
#define OK 0
D 7
#define MEK 1
E 7
I 7
#define MEK (UINT16)1
E 7

I 8
/*
E 8
D 7
#define MONHOP 50000
#define GMONHOP 50000
#define GSORTMONHOP 5000
#define SETHOP 150000
#define GRSETHOP 150000
#define ADRMHOP 500000
E 7
I 7
#define MONHOP 500000
#define GMONHOP 500000
#define GSORTMONHOP 50000
#define SETHOP 1500000
#define GRSETHOP 1500000
#define ADRMHOP 5000000
I 8
*/
D 9
#define MONHOP 1000000
E 9
I 9
#define MONHOP 1000000 /*3000000 ? */
E 9
#define GMONHOP 1000000
#define GSORTMONHOP 100000
#define SETHOP 3000000
#define GRSETHOP 3000000
#define ADRMHOP 10000000
E 8
E 7

#define RESET_ERR 101
#define WAIT_ERROR 102

D 6
ushort *v288adr = (ushort *)0xf0000000;
ushort *statreg = (ushort *)0xf0000002;
ushort *transreg = (ushort *)0xf0000004;
ushort *resetreg = (ushort *)0xf0000006;
E 6

static FILE *m_gpf;

I 4
char charname[21];
D 7
static int name[800]; 
E 7
I 7
static int name[800];

E 7
E 4
/*---------------------------------------------------------*/
/*-------------- Function prototype -----------------------*/
/*---------------------------------------------------------*/
I 9

int send_comm(UINT32 addr, UINT32 offset, UINT16 vmedat, int spas);
int caen_wait(UINT32 addr, int spas);

E 9
void aspeta(int spas);
D 9
int caen_wait(int spas);
E 9
int caen_read(int spas);
D 7
int send_comm(ushort *vmeaddress, ushort vmedat, int spas);
E 7
I 7
D 9
int send_comm(volatile UINT16 *vmeaddress, UINT16 vmedat, int spas);
E 9
E 7
int caen_reset(void);
D 7
int set_mon(char *group_file, int mode, ushort code, ushort com_value);
int grp_read(char *group_file, ushort crate_num, ushort group, ushort code);
int grp_adrm(char *group_file,ushort code, ushort group);
int grp_set(int crate_num, ushort group, ushort code, ushort value);
int set_par(char *group_file, ushort code);
int grp_paread(char *group_file, ushort crate_num, ushort group, ushort code);
E 7
I 7
int set_mon(char *group_file, int mode, UINT16 code, UINT16 com_value);
int grp_read(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code);
int grp_adrm(char *group_file,UINT16 code, UINT16 group);
int grp_set(int crate_num, UINT16 group, UINT16 code, UINT16 value);
int set_par(char *group_file, UINT16 code);
int grp_paread(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code);
E 7

/*---------------------------------------------------------*/
/*--------------------- function delay --------------------*/
/*---------------------------------------------------------*/
I 7


#define NTICKS 1000 /* the number of ticks per second */

IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
int    sysClkRateGet();
STATUS sysClkRateSet(int);

/* sleep() for vxworks - delay n * 10(msec) */
E 7
D 6
void aspeta(int spas)
E 6
I 6
void
I 7
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}



I 9
/*----------------------------------------------------------*/
/*----------------- active loop delay ----------------------*/
/*----------------------------------------------------------*/
E 9
void
E 7
aspeta(int spas)
E 6
{
D 6
int jj;

while(spas--)jj++;
E 6
I 6
  int jj;
D 7
  printf("aspeta reached\n");
  while(spas--)jj++;
E 7
I 7

  /*printf("aspeta reached\n");*/

  while(spas--) jj++;
E 7
E 6
}

I 9
/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
/*
     addr   - base address of v288 board
     offset - register offset (in bytes) we want to access:
                 0 - base reg
                 2 - stat reg
                 4 - trans reg
                 6 - reset reg
     vmedat - data to write into the register
     spas   -  parameter for delay active loop
 */
int
send_comm(UINT32 addr, UINT32 offset, UINT16 vmedat, int spas)
{
  volatile UINT16 *vmeaddress = (volatile UINT16 *) (addr+offset);
  volatile UINT16 *statreg = (volatile UINT16 *) (addr+2);
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

E 9
/*---------------------------------------------------------*/
/*---- CAEN Wait function -------------------------------- */
/*---------------------------------------------------------*/
D 7
int caen_wait(int spas)
E 7
I 7
int
D 9
caen_wait(int spas)
E 9
I 9
caen_wait(UINT32 addr, int spas)
E 9
E 7
{
I 9
  volatile UINT16 *v288adr = (volatile UINT16 *) addr;
  volatile UINT16 *statreg = (volatile UINT16 *) (addr+2);
E 9
D 6
int i=0;
ushort q=0;
ushort vmedat;
/*----- wait for responce ---------------------------------*/
E 6
I 6
  int i=0;
D 7
  ushort q=0;
  ushort vmedat;
printf("caen_wait reached\n");
E 7
I 7
  UINT16 q=0;
  UINT16 vmedat;
E 7

I 7
  /*printf("caen_wait reached\n");*/

E 7
  /*----- wait for responce ---------------------------------*/
E 6
D 7
   while(i<=TIMEOUT && q!=Q)     
   {
         (ushort)vmedat = (ushort)*v288adr;
          aspeta(spas);
       if(((ushort)q=(ushort)*statreg)==Q)
       {
        if(i>=TIMEOUT)printf("caen_wait_error: %x \n",i);
          return vmedat;
           break;
       }
               i++;
   }
        puts("caen_wait_error: OOPS");
           return WAIT_ERROR;
E 7
I 7
D 9
  while(i<=TIMEOUT && q!=QQ)     
E 9
I 9
  while(q!=QQ && i<=TIMEOUT)     
E 9
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
E 7
}
I 6

E 6
/*---------------------------------------------------------*/
/*---- Wait and Read caen data function ------------------ */
/*---------------------------------------------------------*/
D 6
int caen_read(int spas)
E 6
I 6
int
caen_read(int spas)
E 6
{
I 9
  volatile UINT16 *v288adr = (volatile UINT16 *) V288ADR1;
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR1+2);
E 9
D 6
int i=0;
ushort q=0;
ushort vmedat;
/*------ wait for responce --------------------------------*/
E 6
I 6
  int i=0;
D 7
  ushort q=0;
  ushort vmedat;
E 7
I 7
  UINT16 q=0;
  UINT16 vmedat;
E 7

D 7
printf("caen_read reached\n");
E 7
I 7
  /*printf("caen_read reached\n");*/
E 7

  /*------ wait for responce --------------------------------*/
E 6
D 7
    while(i<=TIMEOUT && q!=Q)     
E 7
I 7
  while(i<=TIMEOUT && q!=QQ)     
  {
	aspeta(spas);
    vmedat = *v288adr;
    aspeta(spas);
    if((q=*statreg)==QQ)
E 7
    {
D 7
        (ushort)vmedat = (ushort)*v288adr;
          aspeta(spas);
            if(((ushort)q=(ushort)*statreg)==Q)
            {
              fprintf(m_gpf," %5d",vmedat);
                break;
            }
                    i++;
E 7
I 7
      fprintf(m_gpf," %5d",vmedat);
      break;
E 7
    }
D 7
/*------- read all valid  data ----------------------------*/
     for(;;)                      
     {
          (ushort)vmedat = (ushort)*v288adr;
            aspeta(spas);
              if(((ushort)q=(ushort)*statreg)!=Q) break;
                fprintf(m_gpf," %5d",vmedat);
     }
                  fprintf(m_gpf,"\n");
                    return 0;
E 7
I 7
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
E 7
}
I 6

E 6
D 9
/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
D 6
int send_comm(ushort *vmeaddress, ushort vmedat, int spas)
E 6
I 6
int
D 7
send_comm(ushort *vmeaddress, ushort vmedat, int spas)
E 7
I 7
send_comm(volatile UINT16 *vmeaddress, UINT16 vmedat, int spas)
E 7
E 6
{
D 6
int i=0;
ushort q=0;
E 6
I 6
  int i=0;
D 7
  ushort q=0;
E 7
I 7
  UINT16 q=0;
E 7
E 6

I 6
D 7
printf("send_comm reached\n");
E 6
      while(q!=Q && i<=TIMEOUT)
      {
        (ushort)*vmeaddress = (ushort)vmedat;
          aspeta(spas);
           (ushort)q=(ushort)*statreg;
              i++;
      }
         if(i>TIMEOUT)printf("error code: %d \n",i);
           return((i==TIMEOUT) ? TIMEOUT : OK);
E 7
I 7
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
E 7
}
I 6

E 9
E 6
/*---------------------------------------------------------*/
/*------Reset_restart mod for caenet controller -----------*/ 
/*---------------------------------------------------------*/
D 6
int caen_reset(void)
E 6
I 6
int
caen_reset(void)
E 6
{
I 9
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR1+2);
  volatile UINT16 *resetreg = (volatile UINT16 *) (V288ADR1+6);
E 9
D 6
int i=0;
ushort q=0;
E 6
I 6
  int i=0;
D 7
  ushort q=0;
E 7
I 7
  UINT16 q=0;
E 7
E 6

I 6
D 7
printf("caen_reset reached\n");
E 6
      while(q!=Q && i<=11)
      {
        (ushort)*resetreg = (ushort)MEK;
          aspeta(ADRMHOP);
           (ushort)q=(ushort)*statreg;
              i++;
      }
         if(i>11)printf("error code: %d \n",i);
           return((i==11) ? 11 : OK);
/*
         for(i=1; i<=33; i++) 
         {
            send_comm(resetreg,MEK,ADRMHOP);
              puts("caen reset");
         }
E 7
I 7
  /*printf("caen_reset reached\n");*/
E 7

D 7
           if((clr=send_comm(resetreg,MEK,ADRMHOP)) == TIMEOUT)
              return RESET_ERR;
                else return OK;
*/
E 7
I 7
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
E 7
}
I 6

E 6
/*---------------------------------------------------------*/ 
/*-Set and monitor entier conf  file ----------------------*/
/*-----sintax :--------------------------------------------*/ 
/*-setmon( conf_file_name, set=111/mon=333, code, value)---*/
/*-This function is calling the function  caen_read.-------*/
/*---------------------------------------------------------*/
D 6
int set_mon(char *group_file, int mode, ushort code, ushort com_value)
E 6
I 6
int
D 7
set_mon(char *group_file, int mode, ushort code, ushort com_value)
E 7
I 7
set_mon(char *group_file, int mode, UINT16 code, UINT16 com_value)
E 7
E 6
{
D 6
FILE *gpf_adr;
char charname[21];
int memb_ind = 0;
static int name[800]; 
static int crate_num[800], board_chan[800];
int board, channel;
int ch,i,tmp=0;
E 6
I 6
  FILE *gpf_adr;
  char charname[21];
  int memb_ind = 0;
  static int name[800]; 
  static int crate_num[800], board_chan[800];
  int board, channel;
  int ch,i,tmp=0;
E 6

D 6
puts("START");
E 6
I 6
D 7
printf("set_mon reached\n");
E 7
I 7
  /*printf("set_mon reached\n");*/
E 7

  puts("START");
E 6
/*------ read the config file for the group ---------------*/
D 7
   if ((gpf_adr = fopen(group_file,"r")) == NULL)
   {
       printf("Can't open file %s \n",group_file);
        return 0;
   }
     else {
       while ((ch=getc(gpf_adr)) != EOF)
       {
         memb_ind++;
          fscanf(gpf_adr,"%s %d %d %d %d", charname, 
           &name[memb_ind], &crate_num[memb_ind], &board, &channel);
            board_chan[memb_ind] = ((ushort)board << 8) | (ushort)channel;
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
E 7
I 7
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
D 9
      if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,1,SETHOP))== TIMEOUT )
E 9
        puts("set_fatal error: controller identifier code");
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num[i],SETHOP))== TIMEOUT )
  puts("set_fatal error: can't see CAEN crate");
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,crate_num[i],SETHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,crate_num[i],SETHOP))== TIMEOUT )
E 9
        puts("set_fatal error: can't see CAEN crate");
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
  puts("set_error: operation code failed");
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,code,SETHOP))== TIMEOUT )
E 9
        puts("set_error: operation code failed");
E 7
  
D 7
if(( tmp=send_comm(v288adr,board_chan[i],SETHOP))== TIMEOUT )
  puts("set_error: board_chan to buffer failed");
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,board_chan[i],SETHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,board_chan[i],SETHOP))== TIMEOUT )
E 9
        puts("set_error: board_chan to buffer failed");
E 7

D 7
if(( tmp=send_comm(v288adr,com_value,SETHOP))== TIMEOUT )
  puts("set_error: value to buffer failed");
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,com_value,SETHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,com_value,SETHOP))== TIMEOUT )
E 9
        puts("set_error: value to buffer failed");
E 7

D 7
if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
  puts("set_error: transmission register access failed");
E 7
I 7
D 9
      if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
E 9
I 9
      if(( tmp=send_comm(V288ADR1,4, MEK,SETHOP)) == TIMEOUT)
E 9
        puts("set_error: transmission register access failed");
E 7

D 7
if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
printf("set_error: error code from slave = %x  %d\n",tmp,i);
      }
     }
       else 
       {
/*------------------ monitoring --------------------------*/
if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
I 6
  {
E 6
   puts("Can't open dc_mon.dat");
E 7
I 7
D 9
      if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
E 9
I 9
      if((tmp=caen_wait(V288ADR1, SETHOP)) == WAIT_ERROR)
E 9
        printf("set_error: error code from slave = %x  %d\n",tmp,i);
    }
E 7
I 6
  }
D 7
else
E 7
I 7
  else 
E 7
  {
E 6
D 7
     printf("DEBUG: %d \n",memb_ind); 
I 6
  }
E 6
          for(i=1;i<=memb_ind;i++) 
          {
if(( tmp=send_comm(v288adr,1,MONHOP)) == TIMEOUT )
  printf("mon_fatal error: controller identifier code %d \n",tmp);
E 7
I 7
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
D 9
      if(( tmp=send_comm(v288adr,1,MONHOP)) == TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,1,MONHOP)) == TIMEOUT )
E 9
        printf("mon_fatal error: controller identifier code %d \n",tmp);
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num[i],MONHOP)) == TIMEOUT )
  printf("mon_fatal error: can't see CAEN crate %d \n", tmp);
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,crate_num[i],MONHOP)) == TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,crate_num[i],MONHOP)) == TIMEOUT )
E 9
        printf("mon_fatal error: can't see CAEN crate %d \n", tmp);
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,MONHOP)) == TIMEOUT )
  printf("mon_error: operation code failed %d \n",tmp);
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,code,MONHOP)) == TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,code,MONHOP)) == TIMEOUT )
E 9
        printf("mon_error: operation code failed %d \n",tmp);
E 7
  
D 7
if(( tmp=send_comm(v288adr,board_chan[i],MONHOP))== TIMEOUT )
  printf("mon_error: board_chan to buffer failed %d \n", tmp);
E 7
I 7
D 9
      if(( tmp=send_comm(v288adr,board_chan[i],MONHOP))== TIMEOUT )
E 9
I 9
      if(( tmp=send_comm(V288ADR1,0,board_chan[i],MONHOP))== TIMEOUT )
E 9
        printf("mon_error: board_chan to buffer failed %d \n", tmp);
E 7
    
D 7
if(( tmp=send_comm(transreg, MEK,MONHOP)) == TIMEOUT)
  printf("mon_error: transmission register access failed %d \n",tmp);
fprintf(m_gpf,"%3d ",name[i]);  
tmp=caen_read(MONHOP);
           }
              fclose(m_gpf);
        }
                puts("STOP");
                   return 0;
E 7
I 7
D 9
      if(( tmp=send_comm(transreg, MEK,MONHOP)) == TIMEOUT)
E 9
I 9
      if(( tmp=send_comm(V288ADR1,4, MEK,MONHOP)) == TIMEOUT)
E 9
        printf("mon_error: transmission register access failed %d \n",tmp);

      fprintf(m_gpf,"%3d ",name[i]);  
      tmp=caen_read(MONHOP);
    }
    fclose(m_gpf);
  }
  puts("STOP");

  return(0);
E 7
}
I 6

E 6
/*---------------------------------------------------------*/
/*-------------- Group statusmon function -----------------*/
/*------ it works only for one CAEN crate -----------------*/
/*---------------------------------------------------------*/
I 7
/*
exec grp_read("c1_all.iconf",1,9,65)
*/
E 7
D 6
int grp_read(char *group_file, ushort crate_num, ushort group, ushort code)
E 6
I 6
int
D 7
grp_read(char *group_file, ushort crate_num, ushort group, ushort code)
E 7
I 7
grp_read(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code)
E 7
E 6
{
I 9
  volatile UINT16 *v288adr = (volatile UINT16 *) V288ADR1;
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR1+2);
E 9
D 6
int tmp=0;
FILE *gpf_adr;
ushort vmedat;
D 4
int ch;
E 4
I 4
int ch,loop;
E 4
int i=0, j=1, k=0, memb_ind=0;
ushort q=0;
D 4
char charname[21];
E 4
int z;
E 6
I 6
  int tmp=0;
  FILE *gpf_adr;
D 7
  ushort vmedat;
E 7
I 7
  UINT16 vmedat;
E 7
  int ch,loop;
  int i=0, j=1, k=0, memb_ind=0;
D 7
  ushort q=0;
E 7
I 7
  UINT16 q=0;
E 7
  int z;
E 6
I 4
/*
D 6
char charname[21];
E 4
static int name[800]; 
E 6
I 6
  char charname[21];
  static int name[800]; 
E 6
D 4

E 4
I 4
*/
I 6

D 7
printf("grp_read reached\n");
E 7
I 7
  /*printf("grp_read reached\n");*/
E 7

E 6
D 7
 for(loop=0;loop<=20;loop++) charname[loop] = ' ';
 for(loop=0;loop<=799;loop++)name[loop]=0;
E 4
/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
E 7
I 7
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
E 7
    {
D 7
       printf("Can't open file %s \n",group_file);
          return 0;
E 7
I 7
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
E 7
    }
D 7
     else {
            while ((ch=getc(gpf_adr)) != EOF)
            {
               memb_ind++;
 fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
            }
          }
             fclose(gpf_adr);
/*----------organize the reading-----------------------------*/
if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
D 6
     puts("Can't open dc_mon.dat");
E 6
I 6
{
  puts("Can't open dc_mon.dat");
}
else
{
  printf("DEBUG: open file for 'organize the reading'\n"); 
}
E 7
I 7
  }
  fclose(gpf_adr);
E 7
E 6

D 7
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");
E 7
I 7
  /*----------organize the reading-----------------------------*/
  if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
  {
    puts("Can't open dc_mon.dat");
  }
  else
  {
    printf("Open 'dc_mon.dat' file for 'organize the reading'\n"); 
  }

D 9
  if(( tmp=send_comm(v288adr,1,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,1,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_fatal error: controller identifier code");
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,crate_num,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_fatal error: can't see CAEN crate");
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,code,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,code,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_error: operation code failed");
E 7

D 7
if(( tmp=send_comm(v288adr,group,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,group,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,group,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_error: group to buffer failed");
E 7

D 7
if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
   printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);
/*---- format the output---------------------- -------------*/
/*------------ wait for responce ---------------------------*/
      while(i<=TIMEOUT && q!=Q)     
      {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)==Q)
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
E 7
I 7
D 9
  if(( tmp=send_comm(transreg,MEK,GMONHOP)) == TIMEOUT)
E 9
I 9
  if(( tmp=send_comm(V288ADR1,4,MEK,GMONHOP)) == TIMEOUT)
E 9
    printf("grpmm_gpfon_error: transmission register access failed %d\n",tmp);

  /*---- format the output---------------------- -------------*/
  /*------------ wait for responce ---------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
	aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if((q = *statreg)==QQ)
E 7
    {
D 7
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)!=Q) break;
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
          else fprintf(m_gpf," %5d",vmedat);
   
E 7
I 7
      /*printf("vmedat=0x%08x q=0x%08x ! (name[1]=%3d)\n",vmedat,q,name[1]);*/
      fprintf(m_gpf,"%3d",name[1]);
      /*--------- to be the same output as for set_mon -----------*/
      fprintf(m_gpf,"%5d",0); 
      break;
E 7
    }
D 7
             fprintf(m_gpf,"\n");
      fclose(m_gpf);
        return 0;
E 7
I 7
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
E 7
}
I 6

E 6
/*---------------------------------------------------------*/
/*------ Add or remove channel from the group -------------*/
/*---------------------------------------------------------*/
D 6
int grp_adrm(char *group_file,ushort code, ushort group)
E 6
I 6
int
D 7
grp_adrm(char *group_file,ushort code, ushort group)
E 7
I 7
grp_adrm(char *group_file,UINT16 code, UINT16 group)
E 7
E 6
{
D 6
FILE *gpf_adr;
char charname[21];
int memb_ind = 0;
static int name[800];
static int gcrate_num, gboard_chan[800];
int board, channel;
int ch,i,tmp=0;
E 6
I 6
  FILE *gpf_adr;
  char charname[21];
  int memb_ind = 0;
  static int name[800];
  static int gcrate_num, gboard_chan[800];
  int board, channel;
  int ch,i,tmp=0;
D 7
printf("grp_adrm reached\n");
E 7
E 6

D 7
     if ((gpf_adr = fopen(group_file,"r")) == NULL)
     {
         printf("Can't open file %s \n",group_file);
          return 0;
     }
       else 
       {
         while ((ch=getc(gpf_adr)) != EOF)
         {
           memb_ind++;
            fscanf(gpf_adr,"%s %d %d %d %d", charname, 
             &name[memb_ind],  &gcrate_num, &board, &channel);
              gboard_chan[memb_ind] = ((ushort)board << 8) | (ushort)channel;
         }
       }
               fclose(gpf_adr);
     for(i=1;i<=memb_ind;i++) 
     {
if(( tmp=send_comm(v288adr,1,ADRMHOP))== TIMEOUT )
  puts("grpadrm_fatal error: controller identifier code");
E 7
I 7
  /*printf("grp_adrm reached\n");*/
E 7

D 7
if(( tmp=send_comm(v288adr,gcrate_num,ADRMHOP))== TIMEOUT )
  puts("grpadrm_fatal error: can't see CAEN crate");
E 7
I 7
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
E 7

D 7
if(( tmp=send_comm(v288adr,code,ADRMHOP))== TIMEOUT )
  puts("grpadrm_error: operation code failed");
E 7
I 7
  for(i=1;i<=memb_ind;i++) 
  {
D 9
    if(( tmp=send_comm(v288adr,1,ADRMHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,1,ADRMHOP))== TIMEOUT )
E 9
      puts("grpadrm_fatal error: controller identifier code");
E 7

D 7
if(( tmp=send_comm(v288adr,group,ADRMHOP))== TIMEOUT )
  puts("grpadrm_error: operation code failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,gcrate_num,ADRMHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,gcrate_num,ADRMHOP))== TIMEOUT )
E 9
      puts("grpadrm_fatal error: can't see CAEN crate");
E 7

D 7
if(( tmp=send_comm(v288adr,gboard_chan[i],ADRMHOP))== TIMEOUT )
  puts("grpadrm_error: board_chan to buffer failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,code,ADRMHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,code,ADRMHOP))== TIMEOUT )
E 9
      puts("grpadrm_error: operation code failed");
E 7

D 7
if(( tmp=send_comm(transreg, MEK,ADRMHOP)) == TIMEOUT)
  puts("grpadrm_error: transmission register access failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,group,ADRMHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,group,ADRMHOP))== TIMEOUT )
E 9
      puts("grpadrm_error: operation code failed");
E 7

D 7
if((tmp=caen_wait(ADRMHOP)) == WAIT_ERROR)
printf("grp_adrm: error code from slave = %x  %d\n",tmp,i);
     }
         return 0;  
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,gboard_chan[i],ADRMHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,gboard_chan[i],ADRMHOP))== TIMEOUT )
E 9
      puts("grpadrm_error: board_chan to buffer failed");

D 9
    if(( tmp=send_comm(transreg, MEK,ADRMHOP)) == TIMEOUT)
E 9
I 9
    if(( tmp=send_comm(V288ADR1,4, MEK,ADRMHOP)) == TIMEOUT)
E 9
      puts("grpadrm_error: transmission register access failed");

D 9
    if((tmp=caen_wait(ADRMHOP)) == WAIT_ERROR)
E 9
I 9
    if((tmp=caen_wait(V288ADR1, ADRMHOP)) == WAIT_ERROR)
E 9
      printf("grp_adrm: error code from slave = %x  %d\n",tmp,i);
  }

  return(0);  
E 7
}
I 6

E 6
/*---------------------------------------------------------*/
/*------------ group set operations -----------------------*/
/*--------- works for 1 crate -----------------------------*/
/*---------------------------------------------------------*/
D 6
int grp_set(int crate_num, ushort group, ushort code, ushort value)
E 6
I 6
int
D 7
grp_set(int crate_num, ushort group, ushort code, ushort value)
E 7
I 7
grp_set(int crate_num, UINT16 group, UINT16 code, UINT16 value)
E 7
E 6
{
D 6
int tmp=0;
E 6
I 6
  int tmp=0;
D 7
printf("grp_set reached\n");
E 7
E 6

D 7
if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: controller identifier code");
E 7
I 7
  /*printf("grp_set reached\n");*/

D 9
  if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,1,GRSETHOP))== TIMEOUT )
E 9
    puts("grpset_fatal error: controller identifier code");
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: can't see CAEN crate");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,crate_num,GRSETHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,crate_num,GRSETHOP))== TIMEOUT )
E 9
    puts("grpset_fatal error: can't see CAEN crate");
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
  puts("grpset_error: operation code failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,code,GRSETHOP))== TIMEOUT )
E 9
    puts("grpset_error: operation code failed");
E 7

D 7
if(( tmp=send_comm(v288adr,group,GRSETHOP))== TIMEOUT )
  puts("grpset_error: group to buffer failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,group,GRSETHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,group,GRSETHOP))== TIMEOUT )
E 9
    puts("grpset_error: group to buffer failed");
E 7

D 7
   if(value>0) 
   {
if(( tmp=send_comm(v288adr,value,GRSETHOP))== TIMEOUT )
  puts("grpset_error: value to buffer failed");
   }
if(( tmp=send_comm(transreg, MEK,GRSETHOP)) == TIMEOUT)
  printf("grpset_error: transmission register access failed %d \n",tmp);
E 7
I 7
  if(value>0) 
  {
D 9
    if(( tmp=send_comm(v288adr,value,GRSETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,value,GRSETHOP))== TIMEOUT )
E 9
      puts("grpset_error: value to buffer failed");
  }
E 7

D 7
if((tmp=caen_wait(GRSETHOP)) == WAIT_ERROR)
  printf("grp_set: error code from slave = %x \n",tmp);
                   return 0;  
E 7
I 7
D 9
  if(( tmp=send_comm(transreg, MEK,GRSETHOP)) == TIMEOUT)
E 9
I 9
  if(( tmp=send_comm(V288ADR1,4, MEK,GRSETHOP)) == TIMEOUT)
E 9
    printf("grpset_error: transmission register access failed %d \n",tmp);

D 9
  if((tmp=caen_wait(GRSETHOP)) == WAIT_ERROR)
E 9
I 9
  if((tmp=caen_wait(V288ADR1, GRSETHOP)) == WAIT_ERROR)
E 9
    printf("grp_set: error code from slave = %x \n",tmp);

  return(0);  
E 7
}
I 6

E 6
/*---------------------------------------------------------*/
/*- Function will set parameters on the conf_file members,-*/ 
/*- taking the current values from setdat.txt. ------------*/
/*- Function will find in the group file the "as" and will-*/
/*- set on current as his corresponding value.-------------*/
/*---------------------------------------------------------*/
D 7
int set_par(char *group_file, ushort code)
E 7
I 7
int
set_par(char *group_file, UINT16 code)
E 7
{
D 6
FILE *gpf_name, *gpf_value;
char charname[21],*setname;
int tmp = 0;
int name; 
int crate_num, board, channel, board_chan;
int ch;
int vfa, vfs, vsa, vss, vga, vgs;
int valueas = 0;
int region, sector, teta, as;
E 6
I 6
  FILE *gpf_name, *gpf_value;
  char charname[21],*setname;
  int tmp = 0;
  int name; 
  int crate_num, board, channel, board_chan;
  int ch;
  int vfa, vfs, vsa, vss, vga, vgs;
  int valueas = 0;
  int region, sector, teta, as;
D 7
printf("set_par reached\n");
E 7
E 6

D 7
/*---------- read the parameters file ---------------------*/
if(code == 16) setname = "setdat/v0dat.txt";
 else if (code == 17) setname = "setdat/v1dat.txt";
E 7
I 7
  /*printf("set_par reached\n");*/

  /*---------- read the parameters file ---------------------*/
  if(code == 16)       setname = "setdat/v0dat.txt";
  else if (code == 17) setname = "setdat/v1dat.txt";
E 7
  else if (code == 18) setname = "setdat/i0dat.txt";
D 7
   else if (code == 19) setname = "setdat/i1dat.txt";
    else if (code == 20) setname = "setdat/vmaxdat.txt";
   else if (code == 21) setname = "setdat/rupdat.txt";
E 7
I 7
  else if (code == 19) setname = "setdat/i1dat.txt";
  else if (code == 20) setname = "setdat/vmaxdat.txt";
  else if (code == 21) setname = "setdat/rupdat.txt";
E 7
  else if (code == 22) setname = "setdat/rdwndat.txt";
D 7
 else if (code == 23) setname = "setdat/tripdat.txt";
else return 0;
     if ((gpf_value = fopen(setname,"r"))==NULL)
     {
       printf("Can't open file %s \n",setname);
        return 0;
     }
       else 
       {
         while ((ch=getc(gpf_value)) != EOF)
         {
            fscanf(gpf_value," %d %d %d %d %d %d \n", 
              &vfa,&vfs,&vsa,&vss,&vga,&vgs);
         }
       }
                fclose(gpf_value);
/*-------- read the config file -------------- ------------*/
   if ((gpf_name = fopen(group_file,"r")) == NULL)
   {
     printf("Can't open file %s \n",group_file);
       return 0;
   }
     while ((ch=getc(gpf_name)) != EOF)
     {
        fscanf(gpf_name,"%s %d %d %d %d", 
         charname,&name,&crate_num, &board, &channel);
          board_chan = ((ushort)board << 8) | (ushort)channel;
/*---- decoding the code_names ----------------------------*/
 region = name/10000;
  sector = (name/1000) - (region*10);
   teta = (name/100) - (region*100) - (sector*10);
E 7
I 7
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
E 7
    as = (name/10) - (region*1000) - (sector*100) - (teta*10);
D 7
  if (as == 1) valueas = vfa;
   else if (as == 2) valueas = vfs;
E 7
I 7
    if (as == 1)      valueas = vfa;
    else if (as == 2) valueas = vfs;
E 7
    else if (as == 3) valueas = vsa;
D 7
     else if (as == 4) valueas = vss;
      else if (as == 5) valueas = vga;
       else if (as == 6) valueas = vgs;
        else return 0;
D 3

E 3
I 3
  printf("name = %d as = %d value = %d \n",name,as, valueas);
E 3
if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
  puts("set_fatal error: controller identifier code");
E 7
I 7
    else if (as == 4) valueas = vss;
    else if (as == 5) valueas = vga;
    else if (as == 6) valueas = vgs;
    else return(0);
    printf("name = %d as = %d value = %d \n",name,as, valueas);

D 9
    if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,1,SETHOP))== TIMEOUT )
E 9
      puts("set_fatal error: controller identifier code");
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num,SETHOP))== TIMEOUT )
  puts("set_fatal error: can't see CAEN crate");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,crate_num,SETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,crate_num,SETHOP))== TIMEOUT )
E 9
      puts("set_fatal error: can't see CAEN crate");
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
  puts("set_error: operation code failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,code,SETHOP))== TIMEOUT )
E 9
      puts("set_error: operation code failed");
E 7
  
D 7
if(( tmp=send_comm(v288adr,board_chan,SETHOP))== TIMEOUT )
  puts("set_error: board_chan to buffer failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,board_chan,SETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,board_chan,SETHOP))== TIMEOUT )
E 9
      puts("set_error: board_chan to buffer failed");
E 7

D 7
if(( tmp=send_comm(v288adr,valueas,SETHOP))== TIMEOUT )
  puts("set_error: value to buffer failed");
E 7
I 7
D 9
    if(( tmp=send_comm(v288adr,valueas,SETHOP))== TIMEOUT )
E 9
I 9
    if(( tmp=send_comm(V288ADR1,0,valueas,SETHOP))== TIMEOUT )
E 9
      puts("set_error: value to buffer failed");
E 7

D 7
if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
  puts("set_error: transmission register access failed");
E 7
I 7
D 9
    if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
E 9
I 9
    if(( tmp=send_comm(V288ADR1,4, MEK,SETHOP)) == TIMEOUT)
E 9
      puts("set_error: transmission register access failed");
E 7

D 7
if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
printf("set_error: error code from slave = %x \n",tmp);
          valueas = 0;
     }
            return 0;
E 7
I 7
D 9
    if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
E 9
I 9
    if((tmp=caen_wait(V288ADR1, SETHOP)) == WAIT_ERROR)
E 9
      printf("set_error: error code from slave = %x \n",tmp);

    valueas = 0;
  }

  return(0);
E 7
}
I 6

E 6
/*----------------------------------------------------------*/
/*---function of reding the parameters in grouping mode-----*/
/*-----------works for groups in 1 crate -------------------*/
/*----------------------------------------------------------*/
D 6
int grp_paread(char *group_file, ushort crate_num, ushort group, ushort code)
E 6
I 6
int
D 7
grp_paread(char *group_file, ushort crate_num, ushort group, ushort code)
E 7
I 7
grp_paread(char *group_file, UINT16 crate_num, UINT16 group, UINT16 code)
E 7
E 6
{
I 9
  volatile UINT16 *v288adr = (volatile UINT16 *) V288ADR1;
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR1+2);
E 9
D 6
FILE *gpf_adr;
ushort vmedat;
int ch;
int i=0, j=1, k=0, memb_ind=0, tmp=0;
ushort q=0;
D 4
char charname[21];
E 4
D 5
int z, k_step;
E 5
I 5
int z, k_step,loop;
E 6
I 6
  FILE *gpf_adr;
D 7
  ushort vmedat;
E 7
I 7
  UINT16 vmedat;
E 7
  int ch;
  int i=0, j=1, k=0, memb_ind=0, tmp=0;
D 7
  ushort q=0;
E 7
I 7
  UINT16 q=0;
E 7
  int z, k_step,loop;
D 7
printf("grp_paread reached\n");
E 6
E 5
/*-----------total number of the channels in DC ------------*/
E 7
D 4
static int name[800]; 
E 4

I 4
D 7
 for(loop=0;loop<=20;loop++) charname[loop] = ' ';
 for(loop=0;loop<=799;loop++)name[loop]=0;
E 7
I 7
  /*printf("grp_paread reached\n");*/
E 7

E 4
D 7
/*------------find the k_step according to the code --------*/
/*-----Group mode readings for parameters-------------------*/ 
/*--- have as usual 3 wors. Olny code 70 have 2 words-------*/
      if(code == 70) k_step = 2;
         else k_step = 3;
E 7
I 7
  /*-----------total number of the channels in DC ------------*/
E 7

D 7
/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
E 7
I 7
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
E 7
    {
D 7
       printf("Can't open file %s \n",group_file);
          return 0;
    }
     else {
            while ((ch=getc(gpf_adr)) != EOF)
            {
               memb_ind++;
 fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
            } 
             fclose(gpf_adr);
          }
/*----- open the file for keeping the monitored data --------*/
   if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
I 6
   {
E 6
     puts("Can't open dc_mon.dat");
I 6
   }
   else
   {
     printf("DEBUG: open file for 'keeping the monitored data'\n"); 
   }
E 7
I 7
      memb_ind++;
      fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
    } 
    fclose(gpf_adr);
  }
E 7
E 6

D 7
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");
E 7
I 7
  /*----- open the file for keeping the monitored data --------*/
  if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
  {
    puts("Can't open dc_mon.dat");
  }
  else
  {
    printf("DEBUG: open file for 'keeping the monitored data'\n"); 
  }

D 9
  if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,1,GMONHOP))== TIMEOUT )
E 9
    puts("grpmon_fatal error: controller identifier code");
E 7
   
D 7
if(( tmp=send_comm(v288adr,crate_num,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,crate_num,GMONHOP))== TIMEOUT )
E 9
    puts("grpmon_fatal error: can't see CAEN crate");
E 7
  
D 7
if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,code,GMONHOP))== TIMEOUT )
E 9
    puts("grpmon_error: operation code failed");
E 7

D 7
if(( tmp=send_comm(v288adr,group,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");
E 7
I 7
D 9
  if(( tmp=send_comm(v288adr,group,GMONHOP))== TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,group,GMONHOP))== TIMEOUT )
E 9
    puts("grpmon_error: group to buffer failed");
E 7

D 7
if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
   printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);
/*------------ wait for responce ---------------------------*/
      while(i<=TIMEOUT && q!=Q)     
      {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)==Q)
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
E 7
I 7
D 9
  if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
E 9
I 9
  if(( tmp=send_comm(V288ADR1,4, MEK,GMONHOP)) == TIMEOUT)
E 9
    printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);

  /*------------ wait for responce ---------------------------*/
  while(i<=TIMEOUT && q!=QQ)     
  {
    aspeta(GSORTMONHOP);
    vmedat = *v288adr;
    aspeta(GSORTMONHOP);
    if((q=*statreg)==QQ)
E 7
    {
D 7
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)!=Q) break;
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
          else fprintf(m_gpf," %5d",vmedat);
E 7
I 7
      fprintf(m_gpf,"%3d",name[1]);
      /*--------- to be the same output as for set_mon -----------*/
      fprintf(m_gpf,"%5d",0); 
      break;
E 7
    }
D 7
                  fprintf(m_gpf,"\n");
                      fclose(m_gpf);
                        return 0;
E 7
I 7
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
E 7
} 
D 7
/*----------------------------------------------------------*/
E 7



I 7







int
test()
{
I 9
  volatile UINT16 *v288adr = (volatile UINT16 *) V288ADR1;
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR1+2);
E 9
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

D 9
  if(( tmp=send_comm(v288adr,1,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,1,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_fatal error: controller identifier code");
   
D 9
  if(( tmp=send_comm(v288adr,crate_num,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,crate_num,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_fatal error: can't see CAEN crate");
  
D 9
  if(( tmp=send_comm(v288adr,code,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,code,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_error: operation code failed");

D 9
  if(( tmp=send_comm(v288adr,group,GMONHOP)) == TIMEOUT )
E 9
I 9
  if(( tmp=send_comm(V288ADR1,0,group,GMONHOP)) == TIMEOUT )
E 9
    puts("grpmon_error: group to buffer failed");

D 9
  if(( tmp=send_comm(transreg,MEK,GMONHOP)) == TIMEOUT)
E 9
I 9
  if(( tmp=send_comm(V288ADR1,4,MEK,GMONHOP)) == TIMEOUT)
E 9
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
I 9



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
driven470(UINT16 id, UINT16 channel, UINT16 oper, UINT16 value)
{
  volatile UINT16 *v288adr = (volatile UINT16 *) V288ADR2;
  volatile UINT16 *statreg = (volatile UINT16 *) (V288ADR2+2);  
  UINT16 opcode;
  int i=0;
  UINT16 q=0;
  UINT16 vmedat;
  int tmp = 0;

  /* write controller identification number */
  if(( tmp=send_comm(V288ADR2,0,1,SETHOP))== TIMEOUT )
    puts("set_fatal error: controller identifier code");
  
  /* write address number of the module*/
  if(( tmp=send_comm(V288ADR2,0,id,SETHOP))== TIMEOUT )
    puts("set_fatal error: n470 identification number, obtained by F99");
  
  /* write code of the operation*/
  opcode = ((UINT16)channel << 8) | (UINT16)oper;
  /*   printf("opcode = 0x%08x\n",opcode);*/

  if(( tmp=send_comm(V288ADR2,0,opcode,SETHOP))== TIMEOUT )
    puts("set_fatal error: code of operation, Table 5, N470 manual");
  
      if(oper>2){
  /*write the set vale*/
  if(( tmp=send_comm(V288ADR2,0,value,SETHOP))== TIMEOUT )
    puts("set_fatal error: value");
      }

  /* wrtie into transmit register*/
  if(( tmp=send_comm(V288ADR2,4,MEK,SETHOP)) == TIMEOUT)
    puts("set_error: transmission register access failed");
  
  /*wait*/
  if((tmp=caen_wait(V288ADR2, SETHOP)) == WAIT_ERROR)
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

I 10
void
mytest()
{
  printf("mytest reached\n");
}
E 10
E 9
E 7
E 1
