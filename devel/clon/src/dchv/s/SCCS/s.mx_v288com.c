h12163
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dchv/vme/s/mx_v288com.c
e
s 01145/00000/00000
d D 1.1 00/08/03 10:40:41 gurjyan 1 0
c date and time created 00/08/03 10:40:41 by gurjyan
e
u
U
f e 0
t
T
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef ushort
#define ushort unsigned short
#endif

#define Q (ushort)0xf0fffe
#define TIMEOUT 11
#define OK 0
#define MEK 1

#define MXSTOP 7
#define MONHOP 50000
#define GMONHOP 50000
#define GSORTMONHOP 50000
#define SETHOP 150000
#define GRSETHOP 150000
#define ADRMHOP 500000

#define RESET_ERR 101
#define WAIT_ERROR 102

ushort *v288adr = (ushort *)0xf0000000;
ushort *statreg = (ushort *)0xf0000002;
ushort *transreg = (ushort *)0xf0000004;
ushort *resetreg = (ushort *)0xf0000006;

static FILE *m_gpf;

/*---------------------------------------------------------*/
/*-------------- Function prototype -----------------------*/
/*---------------------------------------------------------*/
void aspeta(int spas);
int caen_wait(int spas);
int caen_read(int spas);
int send_comm(ushort *vmeaddress, ushort vmedat, int spas);
int caen_reset(void);
int set_mon(char *group_file, int mode, ushort code, ushort com_value);
int rgrp_sort (char *group_file, int spas);
int grp_read(char *group_file, ushort crate_num, ushort group, ushort code);
int grp_adrm(char *group_file,ushort code, ushort group);
int grp_set(int crate_num, ushort group, ushort code, ushort value);
int set_par(char *group_file, ushort code);
int mx_gstatmon(char *group_file, ushort crate_num1, ushort group1, ushort crate_num2, 
                ushort group2, ushort crate_num3, ushort group3);
int mx_gset(int crate_num1, int group1, int crate_num2, int group2, 
            int crate_num3, int group3, ushort code, ushort value);
int grp_paread(char *group_file, ushort crate_num, ushort group, ushort code);
int mx_gparmon(char *group_file, ushort crate_num1, ushort group1, ushort crate_num2, 
               ushort group2, ushort crate_num3, ushort group3,ushort code);

/*---------------------------------------------------------*/
/*--------------------- function delay --------------------*/
/*---------------------------------------------------------*/
void aspeta(int spas)
{
int jj;

while(spas--)jj++;
}

/*---------------------------------------------------------*/
/*---- CAEN Wait function -------------------------------- */
/*---------------------------------------------------------*/
int caen_wait(int spas)
{
int i=0;
ushort q=0;
ushort vmedat;
/*----- wait for responce ---------------------------------*/
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
}
/*---------------------------------------------------------*/
/*---- Wait and Read caen data function ------------------ */
/*---------------------------------------------------------*/
int caen_read(int spas)
{
int i=0;
ushort q=0;
ushort vmedat;
/*------ wait for responce --------------------------------*/
    while(i<=TIMEOUT && q!=Q)     
    {
        (ushort)vmedat = (ushort)*v288adr;
          aspeta(spas);
            if(((ushort)q=(ushort)*statreg)==Q)
            {
              fprintf(m_gpf," %5d",vmedat);
                break;
            }
                    i++;
    }
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
}
/*----------------------------------------------------------*/
/*---- Sort the monitored data -----------------------------*/
/*-----  Needed for hardware group reading -----------------*/
/*----------------------------------------------------------*/
int rgrp_sort (char *group_file, int spas)
{
FILE *gpf_adr;
ushort vmedat;
int ch;
int i=0, j=1, k=0, memb_ind=0;
ushort q=0;
char charname[21];
int z;
static int name[800]; 

/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
    {
       printf("Can't open file %s \n",group_file);
          return 0;
    }
     else {
            while ((ch=getc(gpf_adr)) != EOF)
            {
               memb_ind++;
 fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
            }
          }
/*------------ wait for responce ---------------------------*/
      while(i<=TIMEOUT && q!=Q)     
      {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(spas);
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
    {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(spas);
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
   
    }
             fprintf(m_gpf,"\n");
               return 0;
}
/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
int send_comm(ushort *vmeaddress, ushort vmedat, int spas)
{
int i=0;
ushort q=0;

      while(q!=Q && i<=TIMEOUT)
      {
        (ushort)*vmeaddress = (ushort)vmedat;
          aspeta(spas);
           (ushort)q=(ushort)*statreg;
              i++;
      }
         if(i>TIMEOUT)printf("error code: %d \n",i);
           return((i==TIMEOUT) ? TIMEOUT : OK);
}
/*---------------------------------------------------------*/
/*------Reset_restart mod for caenet controller -----------*/ 
/*---------------------------------------------------------*/
int caen_reset(void)
{
int clr;

/*
         for(i=1; i<=33; i++) 
         {
            send_comm(resetreg,MEK,ADRMHOP);
              puts("caen reset");
         }
*/
           if((clr=send_comm(resetreg,MEK,ADRMHOP)) == TIMEOUT)
              return RESET_ERR;
                else return OK;
}
/*---------------------------------------------------------*/ 
/*-Set and monitor entier conf  file ----------------------*/
/*-----sintax :--------------------------------------------*/ 
/*-setmon( conf_file_name, set=111/mon=333, code, value)---*/
/*-This function is calling the function  caen_read.-------*/
/*---------------------------------------------------------*/
int set_mon(char *group_file, int mode, ushort code, ushort com_value)
{
FILE *gpf_adr;
char charname[21];
int memb_ind = 0;
static int name[800]; 
static int crate_num[800], board_chan[800];
int board, channel;
int ch,i,tmp=0;

puts("START");
/*------ read the config file for the group ---------------*/
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
   puts("Can't open dc_mon.dat");
     printf("DEBUG: %d \n",memb_ind); 
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
                   return 0;
}
/*---------------------------------------------------------*/
/*-------------- Group statusmon function -----------------*/
/*------ it works only for one CAEN crate -----------------*/
/*---------------------------------------------------------*/
int grp_read(char *group_file, ushort crate_num, ushort group, ushort code)
{
  int tmp=0;
   if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
     puts("Can't open dc_mon.dat");

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
/*---- call sorting program to make the output -------------*/
/*---------- the same as from channel mode -----------------*/
    tmp=rgrp_sort(group_file,GSORTMONHOP);
      fclose(m_gpf);
        return 0;
}
/*---------------------------------------------------------*/
/*------ Add or remove channel from the group -------------*/
/*---------------------------------------------------------*/
int grp_adrm(char *group_file,ushort code, ushort group)
{
FILE *gpf_adr;
char charname[21];
int memb_ind = 0;
static int name[800];
static int gcrate_num, gboard_chan[800];
int board, channel;
int ch,i,tmp=0;

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
         return 0;  
}
/*---------------------------------------------------------*/
/*------------ group set operations -----------------------*/
/*--------- works for 1 crate -----------------------------*/
/*---------------------------------------------------------*/
int grp_set(int crate_num, ushort group, ushort code, ushort value)
{
int tmp=0;

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
                   return 0;  
}
/*---------------------------------------------------------*/
/*- Function will set parameters on the conf_file members,-*/ 
/*- taking the current values from setdat.txt. ------------*/
/*- Function will find in the group file the "as" and will-*/
/*- set on current as his corresponding value.-------------*/
/*---------------------------------------------------------*/
int set_par(char *group_file, ushort code)
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

/*---------- read the parameters file ---------------------*/
if(code == 16) setname = "setdat/v0dat.txt";
 else if (code == 17) setname = "setdat/v1dat.txt";
  else if (code == 18) setname = "setdat/i0dat.txt";
   else if (code == 19) setname = "setdat/i1dat.txt";
    else if (code == 20) setname = "setdat/vmaxdat.txt";
   else if (code == 21) setname = "setdat/rupdat.txt";
  else if (code == 22) setname = "setdat/rdwndat.txt";
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
    as = (name/10) - (region*1000) - (sector*100) - (teta*10);
  if (as == 1) valueas = vfa;
   else if (as == 2) valueas = vfs;
    else if (as == 3) valueas = vsa;
     else if (as == 4) valueas = vss;
      else if (as == 5) valueas = vga;
       else if (as == 6) valueas = vgs;
        else return 0;

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
            return 0;
}
/*--------------------------------------------------------*/
/*---------function for reading the status of the---------*/
/*--------channels in grouping mode for 3 caen------------*/ 
/*----crates, with output sorting inside------------------*/
/*---------code is fixed = 65-----------------------------*/
/*------crate_num= - 1 will skeep reading that crate------*/
/*--------------------------------------------------------*/
int mx_gstatmon(char *group_file, ushort crate_num1, ushort group1, 
ushort crate_num2, ushort group2, ushort crate_num3, ushort group3)
{
FILE *gpf_adr;
ushort vmedat;
int ch;
int i=0, j=1, k=0, memb_ind=0, tmp=0;
ushort q=0;
char charname[21];
int z;
/*-----------total number of the channels in DC ------------*/
static int name[800]; 
/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
    {
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
     puts("Can't open dc_mon.dat");
/*----------check crate1 and group1 are not equal to -1------*/
   if(crate_num1>0) 
   {
/*------------organaize carate1, group1 readout -------------*/
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");

if(( tmp=send_comm(v288adr,crate_num1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");

if(( tmp=send_comm(v288adr,65,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group1,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

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
    {
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
    }
                  i=0;
                    k=0;
                      tmp=0;
                          q=0;
                           fprintf(m_gpf,"\n");
/*-------------end of reading crate1, group1----------------*/
   }
                aspeta(MXSTOP);
               printf("end of crate 1 %d \n",j);
   if(crate_num2>0) 
   {
/*------------organaize carate2, group2 readout -------------*/
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");

if(( tmp=send_comm(v288adr,crate_num2,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");

if(( tmp=send_comm(v288adr,65,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group2,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
   printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);

/*------------ wait for responce ---------------------------*/
      while(i<=TIMEOUT && q!=Q)     
      {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)==Q)
            {
              fprintf(m_gpf,"%3d",name[j+1]);
/*--------- to be the same output as for set_mon -----------*/
                fprintf(m_gpf,"%5d",0); 
                   break;
            }
                     i++;
       }
/*----------------- read all valid  data -------------------*/
    for(;;)                      
    {
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
    }
                  i=0;
                    k=0;
                      tmp=0;
                          q=0;
                           fprintf(m_gpf,"\n");
/*-------------end of reading crate2, group2----------------*/
   }
                         aspeta(MXSTOP);
                   printf("end of crate 2 %d \n",j);
   if(crate_num3>0) 
   {
/*------------organaize carate3, group3 readout -------------*/
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");

if(( tmp=send_comm(v288adr,crate_num3,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");

if(( tmp=send_comm(v288adr,65,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group3,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
   printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);
/*------------ wait for responce ---------------------------*/
      while(i<=TIMEOUT && q!=Q)     
      {
         (ushort)vmedat = (ushort)*v288adr;
           aspeta(GSORTMONHOP);
            if(((ushort)q=(ushort)*statreg)==Q)
            {
              fprintf(m_gpf,"%3d",name[j+1]);
/*--------- to be the same output as for set_mon -----------*/
                fprintf(m_gpf,"%5d",0); 
                   break;
            }
                     i++;
       }
/*----------------- read all valid  data -------------------*/
    for(;;)                      
    {
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
    }
/*-------------end of reading crate3, group3----------------*/
printf("end of crate 3 %d \n",j);
   }
      fclose(m_gpf);
        return 0;
printf("memb_ind = %d j = %d \n", memb_ind,j);
}
/*----------------------------------------------------------*/
/*----function for setting caen parameters -----------------*/
/*-----in grouping mode for 3 caen crates ------------------*/
/*------crate_num= - 1 will skeep reading that crate--------*/
/*----------------------------------------------------------*/
int mx_gset(int crate_num1, int group1, int crate_num2, int group2, 
int crate_num3, int group3, ushort code, ushort value)
{
int tmp = 0;

   if(crate_num1>0) 
   {
/*-------------set for crate1, group1 ----------------------*/
if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num1,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
  puts("grpset_error: operation code failed");

if(( tmp=send_comm(v288adr,group1,GRSETHOP))== TIMEOUT )
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
/*----------end for crate1, group1 ------------------------*/
   }
            printf("past crate 1\n");
                 fprintf(m_gpf,"\n");
   if(crate_num2>0) 
   {
/*-------------set for crate2, group2 ----------------------*/
if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num2,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
  puts("grpset_error: operation code failed");

if(( tmp=send_comm(v288adr,group2,GRSETHOP))== TIMEOUT )
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
/*----------end for crate2, group2 -------------------------*/
   }
            printf("past crate 2\n");
                 fprintf(m_gpf,"\n");
   if(crate_num3>0) 
   {
/*-------------set for crate3, group3 ----------------------*/
if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num3,GRSETHOP))== TIMEOUT )
  puts("grpset_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
  puts("grpset_error: operation code failed");

if(( tmp=send_comm(v288adr,group3,GRSETHOP))== TIMEOUT )
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
/*----------end for crate3, group3 -------------------------*/
   }
           printf("past crate 3\n");
                  return 0;
}
/*----------------------------------------------------------*/
/*---function of reding the parameters in grouping mode-----*/
/*-----------works for groups in 1 crate -------------------*/
/*----------------------------------------------------------*/
int grp_paread(char *group_file, ushort crate_num, ushort group, ushort code)
{
FILE *gpf_adr;
ushort vmedat;
int ch;
int i=0, j=1, k=0, memb_ind=0, tmp=0;
ushort q=0;
char charname[21];
int z, k_step;
/*-----------total number of the channels in DC ------------*/
static int name[800]; 

/*------------find the k_step according to the code --------*/
/*-----Group mode readings for parameters-------------------*/ 
/*--- have as usual 3 wors. Olny code 70 have 2 words-------*/
      if(code == 70) k_step = 2;
         else k_step = 3;

/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
    {
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
     puts("Can't open dc_mon.dat");

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
    {
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
    }
                      fclose(m_gpf);
                        return 0;
} 
/*----------------------------------------------------------*/
/*--function for reading the parameters in grouping mode----*/
/*----works for 3 CAEN crates ------------------------------*/
/*------crate_num= - 1 will skeep reading that crate--------*/
/*----------------------------------------------------------*/
int mx_gparmon(char *group_file, ushort crate_num1, ushort group1, 
ushort crate_num2, ushort group2, ushort crate_num3, ushort group3,ushort code)
{
FILE *gpf_adr;
ushort vmedat;
int ch;
int i=0, j=1, k=0, memb_ind=0, tmp=0;
ushort q=0;
char charname[21];
int z, k_step;
/*-----------total number of the channels in DC ------------*/
static int name[800]; 

/*------------find the k_step according to the code --------*/
/*-----Group mode readings for parameters-------------------*/ 
/*--- have as usual 3 wors. Olny code 70 have 2 words-------*/
      if(code == 70) k_step = 2;
         else k_step = 3;

/*-------- read the config file for the group --------------*/
    if ((gpf_adr = fopen(group_file,"r")) == NULL)
    {
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
     puts("Can't open dc_mon.dat");
/*----parametes monitor for caen crate1, group1--------------*/
   if(crate_num1>0) 
   {
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group1,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

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
    {
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
    }
                  i = 0;
                   k = 0;
                    tmp = 0;
                      fprintf(m_gpf,"\n");
/*-----------end of reading crate1, group1------------------*/
   }
printf("end of crate 1 %d \n",j);
   if(crate_num2>0) 
   {
/*----parametes monitor for caen crate2, group2--------------*/
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num2,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group2,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

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
    {
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
    }
                  i = 0;
                   k = 0;
                    tmp = 0;
                     fprintf(m_gpf,"\n"); 
/*-----------end of reading crate2, group2------------------*/
   }
printf("end of crate 2 %d \n",j);
   if(crate_num3>0) 
   {
/*----parametes monitor for caen crate3, group3--------------*/
if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: controller identifier code");
   
if(( tmp=send_comm(v288adr,crate_num3,GMONHOP))== TIMEOUT )
  puts("grpmon_fatal error: can't see CAEN crate");
  
if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
  puts("grpmon_error: operation code failed");

if(( tmp=send_comm(v288adr,group3,GMONHOP))== TIMEOUT )
  puts("grpmon_error: group to buffer failed");

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
    {
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
    }
                  i = 0;
                   k = 0;
                    tmp = 0;
/*-----------end of reading crate3, group3------------------*/
   }
printf("end of crate 3 %d \n",j);
                      fclose(m_gpf);
                        return 0;
} 
/*----------------------------------------------------------*/



E 1
