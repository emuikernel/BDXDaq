#ifdef Linux

/****************************************************************/
/*                                                              */
/*  file: rcu_lib.c                                             */
/*                                                              */
/* Library of functions to access the RCU v2.0 via the DDL	*/
/*								*/
/* The Library contains 4 main sections				*/
/* SECTION 1:  Auxiliary functions				*/
/* SECTION 2:  Memory access functions				*/
/* SECTION 3:  Registers functions				*/
/* SECTION 4:  Commands functions				*/
/*								*/
/* Author: 		L. Musa (CERN)                          */
/* Created 		10 May 2008				*/
/* Last Modified 	22 May 2008				*/
/*								*/
/****************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <signal.h>

#include "rorc_lib.h"
#include "fec2rorc_lib.h"
#include "rcu.h"

#include "rcc_error.h"
#include "rcc_time_stamp.h"

    
/************************/
/* 	Globals 	*/
/************************/

rcu_bank_t rcu_imem, rcu_resm, rcu_rolm, rcu_hitm, rcu_datm_A0L, rcu_datm_A0H, rcu_datm_A1L, rcu_datm_A1H, \
           rcu_datm_B0L, rcu_datm_B0H, rcu_datm_B1L, rcu_datm_B1H;
rcu_bank_t *rcu_mem[12]={&rcu_imem, &rcu_resm, &rcu_rolm, &rcu_hitm, &rcu_datm_A0L, &rcu_datm_A0H, &rcu_datm_A1L, &rcu_datm_A1H, \
                         &rcu_datm_B0L, &rcu_datm_B0H, &rcu_datm_B1L, &rcu_datm_B1H};


fecerr_t fecerra_s, fecerrb_s;
rdoerr_t rdoerr_s;
altrobus_t altrobus_s, bussttrsf_s;
rcuid_t rcuid_s;
rcubus_busy_t rcubus_busy_s;
sif_status_t sif_status_s;
sif_error_t sif_error_s;
control_t control_s;
l1_latency_t l1_latency_s;
latency_t l2_latency_s, roi_latency_s, l1_msg_latency_s;
error_cnt_t error_cnt_s;
event_info_t event_info_s;
event_error_t event_error_s;
msm_error_t msm_error_s;
msm_intmode_t msm_intmode_s;
msm_scadd_t msm_scadd_s;

u_int rcu_is_open;
extern u_int packageId, traceLevel;
extern chan_key *rorc;

/************************************************************************************************/
/************************************************************************************************/
/*			SECTION	1 --  A U X I L I A R Y   F U N C T I O N S			*/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
int RCU_Open(rorc_node_t *node)
/************************************************************************************************/
{  
  int ret; 
  u_int loop;
  
  if (rcu_is_open) 
  {
    rcu_is_open++;             /* keep track of multiple open calls */
	printf("mine: RCU_IS_OPEN = %d\n", rcu_is_open);
    DEBUG_TEXT(P_ID_RCU, 80, ("RCU_OPEN()> RCU_IS_OPEN = %d", rcu_is_open));
    return (RCU_SUCCESS);
  }  
  rcu_imem.id    = A_IMEM;
  rcu_imem.size  = S_IMEM;
  rcu_imem.mask  = M_IMEM;
  rcu_imem.name	 = "Instruction Memory";

  rcu_rolm.id    = A_ROLM;
  rcu_rolm.size  = S_ROLM;
  rcu_rolm.mask  = M_ROLM;
  rcu_rolm.name	 = "Readout List memory";

  rcu_resm.id    = A_RESM;
  rcu_resm.size  = S_RESM;
  rcu_resm.mask  = M_RESM;
  rcu_resm.name	 = "Result Memory";

  rcu_hitm.id    = A_HITM;
  rcu_hitm.size  = S_HITM;
  rcu_hitm.mask  = M_HITM;
  rcu_hitm.name	 = "Hit List Memory"; 

  rcu_datm_A0L.id    = A_DATM;
  rcu_datm_A0L.size  = S_DATM;
  rcu_datm_A0L.mask  = M_DATM;
  rcu_datm_A0L.name  = "Data Memory A0L";  

  rcu_datm_A0H.id    = A_DATM+256;
  rcu_datm_A0H.size  = S_DATM;
  rcu_datm_A0H.mask  = M_DATM;
  rcu_datm_A0H.name  = "Data Memory A0H";  

  rcu_datm_A1L.id    = A_DATM+512;
  rcu_datm_A1L.size  = S_DATM;
  rcu_datm_A1L.mask  = M_DATM;
  rcu_datm_A1L.name  = "Data Memory A1L";  

  rcu_datm_A1H.id    = A_DATM+768;
  rcu_datm_A1H.size  = S_DATM;
  rcu_datm_A1H.mask  = M_DATM;
  rcu_datm_A1H.name  = "Data Memory A1H";  

  rcu_datm_B0L.id    = A_DATM+1024;
  rcu_datm_B0L.size  = S_DATM;
  rcu_datm_B0L.mask  = M_DATM;
  rcu_datm_B0L.name  = "Data Memory B0L";  

  rcu_datm_B0H.id    = A_DATM+1280;
  rcu_datm_B0H.size  = S_DATM;
  rcu_datm_B0H.mask  = M_DATM;
  rcu_datm_B0H.name  = "Data Memory B0H";  

  rcu_datm_B1L.id    = A_DATM+1536;
  rcu_datm_B1L.size  = S_DATM;
  rcu_datm_B1L.mask  = M_DATM;
  rcu_datm_B1L.name  = "Data Memory B1L";  

  rcu_datm_B1H.id    = A_DATM+1792;
  rcu_datm_B1H.size  = S_DATM;
  rcu_datm_B1H.mask  = M_DATM;
  rcu_datm_B1H.name  = "Data Memory B1H";  
      

  /* Get RORC channel */
  DEBUG_TEXT(P_ID_RCU, 100,("node->revision = %d,\nnode->serial = %d,\nnode->channel = %d\n", node->revision, node->serial,node->channel));
  rorc = get_channel (node->revision, node->serial, node->channel);
  if (rorc->status != R_OPEN){
    printf ("Could not open rorc device: minor=%i, channel=%i, err=%i\n",
	    rorc->minor, rorc->channel, rorc->err);
    return (rorc->err);
  }
  DEBUG_TEXT(P_ID_RCU, 80 ,("RORC parameters:\nMinor number:\t%d\nChannel number:\t%d\nStatus:\t%d\n",rorc->minor,rorc->channel,rorc->status ));
  
  /* reset SIU, RORC, DIU */ 
  rorcReset (&(rorc->rorc), RORC_RESET_RORC); // RORC
  usleep (200); 
  rorcReset (&(rorc->rorc), RORC_RESET_SIU);  // SIU 
  usleep (200);
  rorcReset (&(rorc->rorc), RORC_RESET_DIU);  // DIU
  usleep (200);
  rorcReset (&(rorc->rorc), RORC_RESET_DIU);  // DIU
  usleep (200);
  rorcReset (&(rorc->rorc), RORC_RESET_RORC); // RORC
  usleep (200); 
  
  /* Open the rcc_time_satmp library */
  ret = ts_open(1, TS_DUMMY);
  if (ret)
    rcc_error_print(stdout, ret);

  DEBUG_TEXT(P_ID_RCU, 10 ,("RCU_Open: RCU library opened\n"));
  
  rcu_is_open = 1;
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_OPEN()> RCU_IS_OPEN = %d", rcu_is_open));
  
  return (RCU_SUCCESS);
}


/************************************************************************************************/
int RCU_Close(void)
/************************************************************************************************/
{  
  RCUISOPEN;
    
  if (rcu_is_open > 1) 
    rcu_is_open--;
  else 
    rcu_is_open = 0;

  /* RORC: cleanup library, close physmem, etc. */
  release_channel (rorc);

  /* Close the rcc_time_satmp library */
  ts_close(TS_DUMMY);  

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_Close()> RCU_IS_OPEN = %d", rcu_is_open));
  DEBUG_TEXT(P_ID_RCU, 10 ,("RCU_Close()> RCU library closed\n"));
  
  return(RCU_SUCCESS);
}



/************************************************************************************************/
int RCU_SIB_Empty_Wait (void)   
/************************************************************************************************/
{ 
  int ret = 0;
  int sif_status = 0;
  int sif_errors  = 0;
  u_int sib_nempty = 1;
  u_int tcount = 0;  
 
  while(sib_nempty)
  { 
     DEBUG_TEXT(P_ID_RCU, 50 ,("RCU_SIB_Empty_Wait()> Calling ddl_statusReadout(rorc, SIFSTATUS)")); 				
     sif_status = ddl_statusReadout(rorc, A_SIFSTATUS);
     if (sif_status < 0) {
        DEBUG_TEXT(P_ID_RCU, 5, ("RCU_SIB_Empty_Wait()> error meassage\nError in ddl_statusReadout:0x%x",sif_status));
        return (RCU_FEC2RORC_FAIL);
     }     

     sib_nempty = (sif_status >> 4) & 0x1;
     if(sib_nempty)
     {
       sif_errors = ddl_statusReadout(rorc, A_SIFERRORS);
       DEBUG_TEXT(P_ID_RCU, 5,("RCU_SIB_Empty_Wait()>\tWaiting for SIB to get empty\nSIFSTATUS =0x%x\tSIFERROR = 0x%x", \
       sif_status & M_SIFSTATUS, sif_errors & M_SIFERRORS));
     }

     DEBUG_TEXT(P_ID_RCU, 50, ("RCU_SIB_Empty_Wait()>\tSIF Status Register = %d",sif_status));
     
     if(tcount == 100){
       DEBUG_TEXT(P_ID_RCU, 5, ("RCU_SIB_Empty_Wait()>\tAccess to SIB timeout (10 ms elapsed)\nSIB STATUS = 0x%x\tSIF_ERRORS = 0x%x\tTCOUNT = %d\n", sif_status, sif_errors, tcount));
       return (RCU_SIB_TIMEOUT);
     }

     ts_delay(100);
     tcount++;
  }
     
  DEBUG_TEXT(P_ID_RCU, 5, ("RCU_SIB_Empty_Wait()>\tSIB STATUS = 0x%x\tSIF_ERRORS = 0x%x\tTCOUNT = %d", sif_status, sif_errors, tcount));
  return(RCU_SUCCESS);
}

/************************************************************************************************/
u_int RCU_File_Read(char *name, u_int size, u_int data[])
/************************************************************************************************/
{
  u_int loop;
  FILE *inf;
  
  /* open the input file */
  inf = fopen(name, "r");
  if (inf == 0) 
  {
    printf("RCU_File_Read:  Can't open input file\n");
    return(-1);
  }

  /* read  the file */
  for (loop = 0; loop < size; loop++) 
  {
    fscanf(inf,"%x",&data[loop]);
    if (feof(inf)) 
    {  
      printf("RCU_File_Read: The file is too short\n");
      return(-1);
    }
  }
  fclose(inf);
  return(0);
}

  
/************************************************************************************************/
u_int RCU_File_Write(char *name, u_int size, u_int data[])
/************************************************************************************************/
{
  u_int loop;
  FILE *outf;

  outf = fopen(name, "a+");
  if (outf==0) 
  {
    printf("RCU_File_Write:  Can't open output file\n");
    return(-1);
  }
  
  /* write  the file */
  for (loop = 0; loop < size; loop++)
    fprintf(outf,"%x\n",data[loop]);

  fclose(outf);
  return(0);  
}


/************************************************************************************************/
/************************************************************************************************/
/*			SECTION	2 	 M E M O R I E S					*/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
int RCU_Memory_Read(rcu_bank_t *membank, u_int offset, u_int nwords, u_long memdata[])   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int loop;
  u_int osize;
  tstamp ts1, ts2;
  float delta_t;  

  RCUISOPEN;

  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_Memory_Read: memory bank = 0x%04x, nwords = %d\n", membank->id, nwords));

  if ((nwords + offset) > membank->size) 
  {    
    DEBUG_TEXT(P_ID_RCU, 5 , ("RCU_Memory_Read: Parameter nwords is out of range [0,%d]\n", membank->size)); 
    return(RCU_RANGE);
  } 

  /* Reading nwords words from RCU Memory */
  /* 1. Instruct the RCU to read n words in the next read */ 

  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_sendCommand()")); 							
  ret = ddl_sendCommand (rorc, RCU_WRD_RD + nwords); 
  if (ret != 0) 
  {
    DEBUG_TEXT(P_ID_RCU, 5, ("Error in ddl_senCommand(): %i", ret));						
    return (RCU_FEC2RORC_FAIL);
  } 
      
  /* 2. Reading from Memory into SIF Output Buffer */ 
  
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_clock()"));				
  ts_clock(&ts1);

  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_readBlock()"));		
  ret = ddl_readBlock (rorc, membank->id + offset, memdata, nwords);
  osize = ret;
  
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_clock()"));  			
  ts_clock(&ts2);
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_duration()"));  
  delta_t = ts_duration(ts1, ts2);
  
  if (osize != nwords){
    DEBUG_TEXT(P_ID_RCU, 5, ("Error in ddl_readBlock(): expected %d words, received %d words", nwords, osize));
    return (RCU_ODD_BYTES);
  }
  
  DEBUG_TEXT(P_ID_RCU, 30, ("%d bytes received in %f seconds\n", osize, delta_t)); 
 
  return(RCU_SUCCESS);
}


/************************************************************************************************/
int RCU_IMEM_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_IMEM_Read: nwords = %d\n", nwords));
  ret = RCU_Memory_Read(&rcu_imem, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_RESM_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_RESM_Read: nwords = %d\n", nwords));
  ret = RCU_Memory_Read(&rcu_resm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_ROLM_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_ROLM_Read: nwords = %d\n", nwords));
  ret = RCU_Memory_Read(&rcu_rolm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_HITM_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_IMEM_Read: nwords = %d\n", nwords));
  ret = RCU_Memory_Read(&rcu_hitm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A0L_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20,("RCU_DATM_A0L_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A0L address =\t%x\n", rcu_datm_A0L.id));
  ret = RCU_Memory_Read(&rcu_datm_A0L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A0H_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A0H_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A0H address =\t%x\n", rcu_datm_A0H.id));
  ret = RCU_Memory_Read(&rcu_datm_A0H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A1L_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A1L_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A1L address =\t%x\n", rcu_datm_A1L.id));
  ret = RCU_Memory_Read(&rcu_datm_A1L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A1H_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A1H_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A1H address =\t%x\n", rcu_datm_A1H.id));
  ret = RCU_Memory_Read(&rcu_datm_A1H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B0L_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B0L_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B0L address =\t%x\n", rcu_datm_B0L.id));
  ret = RCU_Memory_Read(&rcu_datm_B0L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B0H_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B0H_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B0H address =\t%x\n", rcu_datm_B0H.id));
  ret = RCU_Memory_Read(&rcu_datm_B0H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B1L_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B1L_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B1L address =\t%x\n", rcu_datm_B1L.id));
  ret = RCU_Memory_Read(&rcu_datm_B1L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B1H_Read(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B1H_Read: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B1H address =\t%x\n", rcu_datm_B1H.id));
  ret = RCU_Memory_Read(&rcu_datm_B1H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_Memory_Write(rcu_bank_t *membank, u_int offset, u_int nwords, u_long memdata[])   
/************************************************************************************************/
{ 
  int ret = 0;
  tstamp ts1, ts2;
  float delta_t;  

  RCUISOPEN;

  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_Memory_Write: memory bank = 0x%04x nwords = %d\n", membank->id, nwords));

  if ((nwords + offset) > membank->size) 
  {    
    DEBUG_TEXT(P_ID_RCU, 5 , ("RCU_Memory_Write: Parameter nwords is out of range [0,%d]\n", membank->size)); 
    return(RCU_RANGE);
  } 
  
  /* Writing nwords words from RCU Memory */

  /* Checking if the SIF Input Buiffer is empty */
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling RCU_SIB_Empty_Wait()")); 							
  ret = RCU_SIB_Empty_Wait ();
  if (ret != 0)
  {
    DEBUG_TEXT(P_ID_RCU, 1, ("RCU_Memory_Write()>\tError in RCU_SIB_Empty_Wait() before Write Block: return = %d", ret));						
    return (RCU_LIB_FAIL);
  }
      
  /* writing nwords of data into the SIF Input Buffer (SIB) */
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_clock()"));				
  ts_clock(&ts1);

  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_writeBlock()")); 							
  ret = ddl_writeBlock (rorc, memdata, nwords, membank->id + offset);
  if (ret != 0) 
  {
    DEBUG_TEXT(P_ID_RCU, 1, ("\nError in ddl_writeBlock(): %i", ret));						
    return (RCU_LIB_FAIL);
  } 
  
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_clock()"));  			
  ts_clock(&ts2);
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ts_duration()"));  
  delta_t = ts_duration(ts1, ts2);
      
  /* Checking if the SIF Input Buiffer is empty */
  DEBUG_TEXT(P_ID_RCU, 50 ,("Calling RCU_SIB_Empty_Wait()")); 							
  ret = RCU_SIB_Empty_Wait ();
  if (ret != 0)
  {
    DEBUG_TEXT(P_ID_RCU, 1, ("\nRCU_Memory_Write()>\tError in RCU_SIB_Empty_Wait() after Block Write: %i", ret));						
    return (RCU_LIB_FAIL);
  }

  return(RCU_SUCCESS);
}


/************************************************************************************************/
int RCU_IMEM_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  u_long busbusy;
  
  do {
    ret = RCU_RCUBUSBUSY_Read(&busbusy);
    if (ret) return ret;
    if (busbusy&0x01);
    DEBUG_TEXT(P_ID_RCU, 50 ,("RCU_IMEM_Write: RCU still busy with excecuting last sequence\n"));
  } while (busbusy&0x01);


  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_IMEM_Write: nwords = %d\n", nwords));
  ret = RCU_Memory_Write(&rcu_imem, offset, nwords, memdata);
  return ret;
}

/************************************************************************************************/
int RCU_RESM_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_RESM_Write: nwords = %d\n", nwords));
  ret = RCU_Memory_Write(&rcu_resm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_ROLM_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_ROLM_Write: nwords = %d\n", nwords));
  ret = RCU_Memory_Write(&rcu_rolm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_HITM_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_HITM_Write: nwords = %d\n", nwords));
  ret = RCU_Memory_Write(&rcu_hitm, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A0L_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A0L_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A0L address =\t%x\n", rcu_datm_A0L.id));
  ret = RCU_Memory_Write(&rcu_datm_A0L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A0H_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A0H_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A0H address =\t%x\n", rcu_datm_A0H.id));
  ret = RCU_Memory_Write(&rcu_datm_A0H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A1L_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A1L_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A0H address =\t%x\n", rcu_datm_A1L.id));
  ret = RCU_Memory_Write(&rcu_datm_A1L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_A1H_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_A1H_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory A1H address =\t%x\n", rcu_datm_A1H.id));
  ret = RCU_Memory_Write(&rcu_datm_A1H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B0L_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B0L_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B0L address =\t%x\n", rcu_datm_B0L.id));
  ret = RCU_Memory_Write(&rcu_datm_B0L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B0H_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B0H_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B0H address =\t%x\n", rcu_datm_B0H.id));
  ret = RCU_Memory_Write(&rcu_datm_B0H, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B1L_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B1L_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B1L address =\t%x\n", rcu_datm_B1L.id));
  ret = RCU_Memory_Write(&rcu_datm_B1L, offset, nwords, memdata);
  return ret;
}


/************************************************************************************************/
int RCU_DATM_B1H_Write(u_int offset, u_int nwords, u_long memdata[])
/************************************************************************************************/
{
  int ret;
  
  DEBUG_TEXT(P_ID_RCU, 20 ,("RCU_DATM_B1H_Write: nwords = %d\n", nwords));
  DEBUG_TEXT(P_ID_RCU, 80,("DATA Memory B1H address =\t%x\n", rcu_datm_B1H.id));
  ret = RCU_Memory_Write(&rcu_datm_B1H, offset, nwords, memdata);
  return ret;
}





/************************************************************************************************/
/************************************************************************************************/
/*			SECTION	3 	 R E G I S T E R S					*/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
int RCU_Reg_Read(u_int reg, u_long *data)
/************************************************************************************************/
{
  u_int valid_core_reg = 0; 
  u_int valid_sif_reg = 0; 
  u_int valid_reg;
  u_int osize;
  int ret;
  
  RCUISOPEN;

  DEBUG_TEXT(P_ID_RCU, 10, ("RCU_Reg_Read: Function called for reg = %x", reg));
  
  /* RCU CORE Registers  */
  if (( reg >= A_ACTFECLIST)  && (reg <= A_RCU_ID))
     valid_core_reg = 1;
      
  else if ((reg >= A_FECERRA)  && (reg <= A_EVTMANFSM))
     valid_core_reg = 1;

  /* RCU TTC_Receiver Registers (several are multiple registers see rcu.h */ 
  else if (( reg == A_CONTROL)  || (reg == A_ROI_CONFIG1) || (reg == A_ROI_CONFIG2) || \
  	   (reg == A_L1_LATENCY) || (reg == A_L2_LATENCY) || \
	   ((reg >= A_ROI_LATENCY) && (reg <= A_BUNCH_COUNTER)) || \
	   (reg == A_HAMNG_ERR_CNT) || (reg == A_ERROR_CNT) || \
	   ((reg >= A_BUFFERED_EVENTS) && (reg <= A_EVENT_ERROR)) || \
	   ((reg >= A_L1_MSG_HEADER) && (reg <= (A_ROI_MSG_DATA + 2))) || \
	   (reg == A_FIFO_DAQ_HEADER))
     valid_core_reg = 1;
     
  /* RCU MSM Registers */
  else if ( (reg == A_MSM_RESULT) || (reg == A_MSM_ERROR) || (reg == A_MSM_INTMODE) || \
    	    (reg == A_MSM_SCADD) || (reg == A_MSM_SCDATA) )
     valid_core_reg = 1;

  /* SIF Registers  */  
  else if((reg == A_SIFSTATUS) || (reg == A_SIFERRORS) || (reg == A_SIFBLREG))
     valid_sif_reg = 1;   

  valid_reg = valid_core_reg | valid_sif_reg;

  if(!valid_reg)
  {    
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_Reg_Read: Unknown register %d\n", reg));
    return(RCU_RANGE); 
  }

  else if(valid_core_reg)
  {
     /* 1. Instruct the RCU to read 1 word in the next read */  
     DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_sendCommand()")); 							
     ret = ddl_sendCommand (rorc, RCU_WRD_RD + 1);
     if (ret != 0) 
     {
	DEBUG_TEXT(P_ID_RCU, 5, ("\nError in ddl_senCommand(): %i", ret));						
	return (RCU_FEC2RORC_FAIL);
     } 

     /* 2. Reading from Register into SIF Output Buffer */ 
     DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_readBlock()"));		
     ret = ddl_readBlock (rorc, reg, data, 1);
     osize = ret;

     if (osize != 1){
        DEBUG_TEXT(P_ID_RCU, 5, ("\nError in ddl_readBlock(): expected 1 word, received %d words", osize));
        return (RCU_ODD_BYTES);
     }

     DEBUG_TEXT(P_ID_RCU, 80, ("RCU_Reg_Read() successful! REG value is: 0x%x", *data));
     return(RCU_SUCCESS);
  }
  
  else
  {
     ret = ddl_statusReadout(rorc, reg);
     *data = ret;
     if (ret < 0){
        DEBUG_TEXT(P_ID_RCU, 5, ("\nRCU_Reg_Read()> Error in ddl_statusReadout(): %i\n",ret));
        return (RCU_FEC2RORC_FAIL);        
     }
     DEBUG_TEXT(P_ID_RCU, 80, ("RCU_Reg_Read()> successful completion!"));
     return(RCU_SUCCESS);
  }
}  


/************************************************************************************************/
int RCU_Reg_Write(u_int reg, u_long data)
/************************************************************************************************/
{
  u_int valid_core_reg = 0; 
  u_int valid_sif_reg = 0; 
  u_int valid_reg; 
  u_int read_only_reg =0;
  u_int ccode;
  int ret;
  
  RCUISOPEN;
  
  DEBUG_TEXT(P_ID_RCU, 10, ("RCU_Reg_Write: Function called for reg = %x\n", reg));

  /* RCU CORE Registers & Commands */
  if (( reg >= A_ACTFECLIST)  && (reg <= A_RCU_ID))
     valid_core_reg = 1; 
      
  else if ((reg >= A_FECERRA)  && (reg <= A_BLENMISMATCH))
  { valid_core_reg = 1; read_only_reg =1; }
  
  else if ( ((reg >= A_CONFEC) && (reg <= A_CLRTRGCNT)) || (reg == A_ARBITERIRQ))
  { valid_core_reg = 1;}

  /* RCU TTC_Receiver Registers & Commands (several are multiple registers see rcu.h */ 
  else if (( reg == A_CONTROL)  || (reg == A_ROI_CONFIG1) || (reg == A_ROI_CONFIG2) || \
  	   (reg == A_L1_LATENCY) || (reg == A_L2_LATENCY) ||  (reg == A_ROI_LATENCY) || (reg == A_L1_MSG_LATENCY))
     valid_core_reg = 1; 

  else if (((reg >= A_PRE_PULSE_CNT) && (reg <= A_BUNCH_COUNTER)) || \
	   (reg == A_HAMNG_ERR_CNT) || (reg == A_ERROR_CNT) || \
	   ((reg >= A_BUFFERED_EVENTS) && (reg <= A_EVENT_ERROR)) || \
	   ((reg >= A_L1_MSG_HEADER) && (reg <= (A_ROI_MSG_DATA + 2))) || \
	   (reg == A_FIFO_DAQ_HEADER))
  { valid_core_reg = 1; read_only_reg =1; }
  
  else if ( (reg == A_TMODULE_RESET) || (reg == A_TRESET_COUNTERS) || \
            (reg == A_TISSUE_TEST_MODE) || (reg == A_TFIFO_READ_EN) )
  { valid_core_reg = 1;}
	    
  
  /* RCU MSM Registers */
  else if ( (reg == A_MSM_INTMODE) || (reg == A_MSM_SCADD) || (reg == A_MSM_SCDATA) )
     valid_core_reg = 1;

  else if ( (reg == A_MSM_RESULT) || (reg == A_MSM_ERROR) )
  { valid_core_reg = 1; read_only_reg =1; }

  /* SIF Registers  */  
  else if((reg == A_SIFSTATUS) || (reg == A_SIFERRORS))
  { valid_sif_reg = 1; read_only_reg =1; }    

  else if (reg == A_SIFBLREG)
     valid_sif_reg = 1; 

  valid_reg = valid_core_reg | valid_sif_reg;

  if (!valid_reg)
  {    
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_Reg_Write: Unknown register %x\n", reg));
    return(RCU_RANGE); 
  }
  else if (read_only_reg)
  {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_Reg_Write: Read-only Register %x\n", reg));
    return(RCU_RANGE); 
  } 

  else if (valid_core_reg)
  {
     /* Checking if the SIF Input Buiffer is empty */
     DEBUG_TEXT(P_ID_RCU, 50 ,("Calling RCU_SIB_Empty_Wait()")); 							
     ret = RCU_SIB_Empty_Wait ();
     if (ret != 0)
     {
        DEBUG_TEXT(P_ID_RCU, 5, ("\nError in RCU_SIB_Empty_Wait(): %i", ret));						
        return (RCU_LIB_FAIL);
     }
      
     /* writing 1 word of data into the SIF Input Buffer (SIB) */
     DEBUG_TEXT(P_ID_RCU, 50 ,("Calling ddl_writeBlock()")); 							
     ret = ddl_writeBlock (rorc, &data, 1, reg);
     if (ret != 0) 
     {
       DEBUG_TEXT(P_ID_RCU, 5, ("\nError in ddl_writeBlock(): %i", ret));						
       return (RCU_LIB_FAIL);
     } 
      
     /* Checking if the SIF Input Buiffer is empty */
     DEBUG_TEXT(P_ID_RCU, 50 ,("Calling RCU_SIB_Empty_Wait()")); 							
     ret = RCU_SIB_Empty_Wait ();
     if (ret != 0)
     {
       DEBUG_TEXT(P_ID_RCU, 5, ("\nError in RCU_SIB_Empty_Wait(): %i", ret));						
       return (RCU_LIB_FAIL);
     }
     
     return (RCU_SUCCESS);
  }
  
  else
  {
     ccode = reg + (data & 0xffff);
     ret = ddl_sendCommand(rorc, ccode);
     if(ret < 0) {
        DEBUG_TEXT(P_ID_RCU, 5, ("\nRCU_Lib: RCU_Reg_Write() error meassage\nError in ddl_statusReadout: %i\n",ret));
        return (RCU_FEC2RORC_FAIL);        
     }
     return (RCU_SUCCESS);     
  }        
}  


/************************************************************************************************/
/*			        RCU CORE REGISTERS (READ)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_ACTFECLIST_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ACTFECLIST, data);
   *data = *data & M_ACTFECLIST;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register ACTFECLIST:\t%x", *data));
   return ret;
} 

/************************************************************************************************/
int RCU_ALTROIF_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ALTROIF, data);
   *data = *data & M_ALTROIF;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register ALTROIF:\t%x", *data));
   return ret;
} 

/************************************************************************************************/
int RCU_TRGCONF_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_TRGCONF, data);
   *data = *data & M_TRGCONF;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register TRGCONF:\t%x", *data));
   return ret;
} 

/************************************************************************************************/
int RCU_RDOMOD_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RDOMOD, data);
   *data = *data & M_RDOMOD;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register RDOMOD:\t%x", *data));
   return ret;      
} 

/************************************************************************************************/
int RCU_ALTROCFG1_Read(u_long *data)
/************************************************************************************************/
{
int ret;
   ret = RCU_Reg_Read(A_ALTROCFG1, data);
   *data = *data & M_ALTROCFG1;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register ALTROCFG1:\t%x", *data));
   return ret;   
} 

/************************************************************************************************/
int RCU_ALTROCFG2_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ALTROCFG2, data);
   *data = *data & M_ALTROCFG2;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register ALTROCFG2:\t%x", *data));
   return ret;   
} 

/************************************************************************************************/
int RCU_RCU_VERSION_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RCU_VERSION, data);
   *data = *data & M_RCU_VERSION;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register RCU_VERSION:\t%x", *data));
   return ret;      
} 

/************************************************************************************************/
int RCU_BP_VERSION_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BP_VERSION, data);
   *data = *data & M_BP_VERSION;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register BP_VERSION:\t%x", *data));
   return ret;      
} 

/************************************************************************************************/
int RCU_FECERRA_Read(u_long *data_p)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_FECERRA, data_p);
   DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERRA_Read()> RCU_Reg_Read() returns %d", ret));
   *data_p = *data_p & M_FECERRA;
   DEBUG_TEXT(P_ID_RCU, 50, ("RCU_FECERRA_Read()> Content of Register FECERRA: %x", *data_p));
   return ret;         
} 

/************************************************************************************************/
int RCU_FECERRB_Read(u_long *data)
/************************************************************************************************/
{
int ret; 

   ret = RCU_Reg_Read(A_FECERRB, data);
   *data = *data & M_FECERRB;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register FECERRB:\t%x", data));
   return ret;      
} 

/************************************************************************************************/
int RCU_RDOERR_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RDOERR, data);
   *data = *data & M_RDOERR;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register RDOERR:\t%x", *data));
   return ret;         
} 

/************************************************************************************************/
int RCU_ALTROBUS_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ALTROBUS, data);
   *data = *data & M_ALTROBUS;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register ALTROBUS:\t%x", *data));
   return ret;         
} 

/************************************************************************************************/
int RCU_BUSSTTRSF_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BUSSTTRSF, data);
   *data = *data & M_BUSSTTRSF;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register BUSSTTRSF:\t%x", *data));
   return ret;         
} 

/************************************************************************************************/
int RCU_RCUBUSBUSY_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RCUBUSBUSY, data);
   *data = *data & M_RCUBUSBUSY;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register RCUBUSBUSY:\t%x", *data));
   return ret;         
} 

/************************************************************************************************/
int RCU_RCUID_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RCU_ID, data);
   *data = *data & M_RCU_ID;
   DEBUG_TEXT(P_ID_RCU, 50, ("Content of Register RCUID:\t%x", *data));
   return ret;         
} 

/************************************************************************************************/
int RCU_SWTTRGCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_SWTTRGCNT, data);
   *data = *data & M_SWTTRGCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_AUXTRGCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_AUXTRGCNT, data);
   *data = *data & M_AUXTRGCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_TTCL2ACNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_TTCL2ACNT, data);
   *data = *data & M_TTCL2ACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_TTCL2RCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_TTCL2RCNT, data);
   *data = *data & M_TTCL2RCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_DSTBACNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_DSTBACNT, data);
   *data = *data & M_DSTBACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_DSTBBCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_DSTBBCNT, data);
   *data = *data & M_DSTBACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_TRSFACNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_TRSFACNT, data);
   *data = *data & M_TRSFACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_TRSFBCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_TRSFBCNT, data);
   *data = *data & M_TRSFBCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_ACKACNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ACKACNT, data);
   *data = *data & M_ACKACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_ACKBCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ACKBCNT, data);
   *data = *data & M_ACKBCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_CSTBACNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_CSTBACNT, data);
   *data = *data & M_CSTBACNT;
   return ret;
} 

/************************************************************************************************/
int RCU_CSTBBCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_CSTBBCNT, data);
   *data = *data & M_CSTBBCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_DSTBNUMA_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret =  RCU_Reg_Read(A_DSTBNUMA, data);
   *data = *data & M_DSTBNUMA;
   return ret;
} 

/************************************************************************************************/
int RCU_DSTBNUMB_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_DSTBNUMB, data);
   *data = *data & M_DSTBNUMB;
   return ret;
} 

/************************************************************************************************/
int RCU_MEBCNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MEBCNT, data);
   *data = *data & M_MEBCNT;
   return ret;
} 

/************************************************************************************************/
int RCU_ADDRMISMATCH_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ADDRMISMATCH, data);
   *data = *data & M_ADDRMISMATCH;
   return ret;
} 

/************************************************************************************************/
int RCU_BLENMISMATCH_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BLENMISMATCH, data);
   *data = *data & M_BLENMISMATCH;
   return ret;
} 

/************************************************************************************************/
int RCU_ABDFSM_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ABDFSM, data);
   *data = *data & M_ABDFSM;
   return ret;
} 

/************************************************************************************************/
int RCU_RDOFSM_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_RDOFSM, data);
   *data = *data & M_RDOFSM;
   return ret;
} 

/************************************************************************************************/
int RCU_ISFSM_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ISFSM, data);
   *data = *data & M_ISFSM;
   return ret;
} 

/************************************************************************************************/
int RCU_EVTMANFSM_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_EVTMANFSM, data);
   *data = *data & M_EVTMANFSM;
   return ret;
} 



/************************************************************************************************/
/************************************************************************************************/
/*			RCU TTC RECEIVER REGISTERS (READ)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_CONTROL_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_CONTROL, data);
   *data = *data & M_CONTROL;
   return ret;
} 

/************************************************************************************************/
int RCU_ROI_CONFIG1_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ROI_CONFIG1, data);
   *data = *data & M_ROI_CONFIG1;
   return ret;
} 

/************************************************************************************************/
int RCU_ROI_CONFIG2_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ROI_CONFIG2, data);
   *data = *data & M_ROI_CONFIG2;
   return ret;
} 


/************************************************************************************************/
int RCU_L1_LATENCY_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L1_LATENCY, data);
   *data = *data & M_L1_LATENCY;
   return ret;
} 

/************************************************************************************************/
int RCU_L2_LATENCY_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L2_LATENCY, data);
   *data = *data & M_L2_LATENCY;
   return ret;
} 

/************************************************************************************************/
int RCU_ROI_LATENCY_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ROI_LATENCY, data);
   *data = *data & M_ROI_LATENCY;
   return ret;
} 

/************************************************************************************************/
int RCU_L1_MSG_LATENCY_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L1_MSG_LATENCY, data);
   *data = *data & M_L1_MSG_LATENCY;
   return ret;
} 

/************************************************************************************************/
int RCU_PRE_PULSE_CNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_PRE_PULSE_CNT, data);
   *data = *data & M_PRE_PULSE_CNT;
   return ret;
} 

/************************************************************************************************/
int RCU_BCID_LOCAL_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BCID_LOCAL, data);
   *data = *data & M_BCID_LOCAL;
   return ret;
} 

/************************************************************************************************/
int RCU_L0_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L0_COUNTER, data);
   *data = *data & M_L0_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_L1_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L1_COUNTER, data);
   *data = *data & M_L1_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_L1_MSG_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L1_MSG_COUNTER, data);
   *data = *data & M_L1_MSG_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_L2A_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L2A_COUNTER, data);
   *data = *data & M_L2A_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_L2R_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L2R_COUNTER, data);
   *data = *data & M_L2R_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_ROI_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ROI_COUNTER, data);
   *data = *data & M_ROI_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_BUNCH_COUNTER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BUNCH_COUNTER, data);
   *data = *data & M_BUNCH_COUNTER;
   return ret;
} 

/************************************************************************************************/
int RCU_HAMNG_ERR_CNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_HAMNG_ERR_CNT, data);
   *data = *data & M_HAMNG_ERR_CNT;
   return ret;
} 

/************************************************************************************************/
int RCU_ERROR_CNT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ERROR_CNT, data);
   *data = *data & M_ERROR_CNT;
   return ret;
} 

/************************************************************************************************/
int RCU_BUFFERED_EVENTS_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_BUFFERED_EVENTS, data);
   *data = *data & M_BUFFERED_EVENTS;
   return ret;
} 

/************************************************************************************************/
int RCU_DAQ_HEADER_Read(u_long *data)
/************************************************************************************************/
{
   u_int loop;
   int ret;

   for (loop = 0; loop <7; loop++)
   {
      ret = RCU_Reg_Read(A_DAQ_HEADER, data);
      if (!ret)
         return ret;
      data[loop] = data[loop] & M_DAQ_HEADER;
   } 
   return ret;
}

/************************************************************************************************/
int RCU_EVENT_INFO_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_EVENT_INFO, data);
   *data = *data & M_EVENT_INFO;
   return ret;
} 

/************************************************************************************************/
int RCU_EVENT_ERROR_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_EVENT_ERROR, data);
   *data = *data & M_EVENT_ERROR;
   return ret;
} 

/************************************************************************************************/
int RCU_L1_MSG_HEADER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L1_MSG_HEADER, data);
   *data = *data & M_L1_MSG_HEADER;
   return ret;
} 

/************************************************************************************************/
int RCU_L1_MSG_DATA_Read(u_long *data)
/************************************************************************************************/
{
   u_int loop;
   int ret;

   for (loop = 0; loop <3; loop++)
   {
      ret = RCU_Reg_Read(A_L1_MSG_DATA, data);
      if (!ret)
         return ret;
      data[loop] = data[loop] & M_L1_MSG_DATA;
   } 
   return ret;      
} 

/************************************************************************************************/
int RCU_L2A_MSG_HEADER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L2A_MSG_HEADER, data);
   *data = *data & M_L2A_MSG_HEADER;
   return ret;
} 

/************************************************************************************************/
int RCU_L2A_MSG_DATA_Read(u_long *data)
/************************************************************************************************/
{
   u_int loop;
   int ret;

   for (loop = 0; loop <7; loop++)
   {
      ret = RCU_Reg_Read(A_L2A_MSG_DATA, data);
      if (!ret)
         return ret;
      data[loop] = data[loop] & M_L2A_MSG_DATA;
   } 
   return ret;      
}

/************************************************************************************************/
int RCU_L2R_MSG_HEADER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_L2R_MSG_HEADER, data);
   *data = *data & M_L2R_MSG_HEADER;
   return ret;
} 
 
/************************************************************************************************/
int RCU_ROI_MSG_HEADER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_ROI_MSG_HEADER, data);
   *data = *data & M_ROI_MSG_HEADER;
   return ret;
} 

/************************************************************************************************/
int RCU_ROI_MSG_DATA_Read(u_long *data)
/************************************************************************************************/
{
   u_int loop;
   int ret;

   for (loop = 0; loop <3; loop++)
   {
      ret = RCU_Reg_Read(A_ROI_MSG_DATA, data);
      if (!ret)
         return ret;
      data[loop] = data[loop] & M_ROI_MSG_DATA;
   } 
   return ret;     
}

/************************************************************************************************/
int RCU_FIFO_DAQ_HEADER_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_FIFO_DAQ_HEADER, data);
   *data = *data & M_FIFO_DAQ_HEADER;
   return ret;
} 

/************************************************************************************************/
/*				RCU SIF REGISTERS (READ)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_SIFSTATUS_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_SIFSTATUS, data);
   *data = *data & M_SIFSTATUS;
   return ret;
} 

/************************************************************************************************/
int RCU_SIFERRORS_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_SIFERRORS, data);
   *data = *data & M_SIFERRORS;
   return ret;
} 

/************************************************************************************************/
int RCU_SIFBLREG_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_SIFBLREG, data);
   *data = *data & M_SIFBLREG;
   return ret;
} 

/************************************************************************************************/
/*				RCU MSM REGISTERS (READ)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_MSM_ERROR_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MSM_ERROR, data);
   *data = *data & M_MSM_ERROR;
   return ret;
} 

/************************************************************************************************/
int RCU_MSM_INTMODE_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MSM_INTMODE, data);
   *data = *data & M_MSM_INTMODE;
   return ret;
} 

/************************************************************************************************/
int RCU_MSM_SCADD_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MSM_SCADD, data);
   *data = *data & M_MSM_SCADD;
   return ret;
} 

/************************************************************************************************/
int RCU_MSM_SCDATA_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MSM_SCDATA, data);
   *data = *data & M_MSM_SCDATA;
   return ret;
} 

/************************************************************************************************/
int RCU_MSM_RESULT_Read(u_long *data)
/************************************************************************************************/
{
int ret;

   ret = RCU_Reg_Read(A_MSM_RESULT, data);
   *data = *data & M_MSM_RESULT;
   return ret;
} 


/************************************************************************************************/
/*			        RCU CORE REGISTERS (WRITE)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_ACTFECLIST_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ACTFECLIST, data);
} 

/************************************************************************************************/
int RCU_ALTROIF_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ALTROIF, data);
} 

/************************************************************************************************/
int RCU_TRGCONF_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_TRGCONF, data);
} 

/************************************************************************************************/
int RCU_RDOMOD_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_RDOMOD, data);
} 

/************************************************************************************************/
int RCU_ALTROCFG1_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ALTROCFG1, data);
} 

/************************************************************************************************/
int RCU_ALTROCFG2_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ALTROCFG2, data);
} 

/************************************************************************************************/
int RCU_BP_VERSION_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_BP_VERSION, data);
} 

/************************************************************************************************/
int RCU_RCUID_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_RCU_ID, data);
} 

/************************************************************************************************/
/*			RCU TTC RECEIVER REGISTERS (WRITE)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_CONTROL_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_CONTROL, data);
} 

/************************************************************************************************/
int RCU_ROI_CONFIG1_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ROI_CONFIG1, data);
} 

/************************************************************************************************/
int RCU_ROI_CONFIG2_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ROI_CONFIG2, data);
} 

/************************************************************************************************/
int RCU_L1_LATENCY_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_L1_LATENCY, data);
} 

/************************************************************************************************/
int RCU_L2_LATENCY_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_L2_LATENCY, data);
} 

/************************************************************************************************/
int RCU_ROI_LATENCY_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_ROI_LATENCY, data);
} 

/************************************************************************************************/
int RCU_L1_MSG_LATENCY_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_L1_MSG_LATENCY, data);
} 


/************************************************************************************************/
/*				RCU MSM REGISTERS (WRITE)					*/
/************************************************************************************************/
/************************************************************************************************/
int RCU_MSM_INTMODE_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_MSM_INTMODE, data);
} 

/************************************************************************************************/
int RCU_MSM_SCADD_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_MSM_SCADD, data);
} 

/************************************************************************************************/
int RCU_MSM_SCDATA_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_MSM_SCDATA, data);
} 


/************************************************************************************************/
/*				RCU SIF REGISTERS (WRITE)					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_SIFBLREG_Write(u_long data)
/************************************************************************************************/
{
   return RCU_Reg_Write(A_SIFBLREG, data);
} 


/************************************************************************************************/
/*					DECODING OF REGISTERS 					*/
/************************************************************************************************/

/************************************************************************************************/
int RCU_ALTROIF_decode(u_int data, altroif_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->nsamples_ev   = data & 0x3ff; 
  parameters->clk_ratio     = (data >> 10) & 0xf;
  parameters->cstb_delay    = (data >> 14) & 0x3;
  parameters->safe_mode	    = (data >> 16) & 0x3;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_TRGCONF_decode(u_int data, trgconf_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->l2_latency    = data & 0x1fff; 
  parameters->trg_mode      = (data >> 13) & 0x1;
  parameters->trg_source    = (data >> 14) & 0x7;
  return(RCU_SUCCESS);
}

/* new added */
/************************************************************************************************/
int RCU_RDOMOD_decode(u_int data, rdomod_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->meb_mode      = data & 0x1; 
  parameters->exec_seq      = (data >> 1) & 0x1;
  parameters->sparse_rdo    = (data >> 2) & 0x1;
  parameters->check_rdyrx   = (data >> 3) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_ALTROCFG1_decode(u_int data, altrocfg1_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->bc1_cfg        = data & 0x1f; 
  parameters->bc2_cfg        = (data >> 5) & 0x7f;
  parameters->zs_cfg         = (data >> 12) & 0xff;
  return(RCU_SUCCESS);
}
  
/************************************************************************************************/
int RCU_ALTROCFG2_decode(u_int data, altrocfg2_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->ptrg           = data & 0xf;
  parameters->nbuf           = (data >> 4) & 0x1;
  parameters->flt_en         = (data >> 5) & 0x1;
  parameters->pwsv           = (data >> 6) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_RCU_VERSION_decode(u_int data, rcu_version_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->year      = data & 0xff;
  parameters->month     = (data >>  8) & 0xff;
  parameters->day       = (data >> 16) & 0xff;  
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_FECERR_decode(u_long data, fecerr_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN; 

  parameters->altro_error	= data & 0x1;
  parameters->w_error_b_cstb	= (data >>  1) & 0x1;
  parameters->w_ack_b_cstb	= (data >>  2) & 0x1;
  parameters->w_error_b_ack	= (data >>  3) & 0x1;
  parameters->w_no_ack		= (data >>  4) & 0x1;
  parameters->w_error_w_ack	= (data >>  5) & 0x1;
  parameters->w_ack_not_rel	= (data >>  6) & 0x1;
  parameters->w_error_bcast	= (data >>  7) & 0x1;
  parameters->r_error_b_cstb	= (data >>  8) & 0x1;
  parameters->r_error_b_ack	= (data >>  9) & 0x1;
  parameters->r_ack_b_cstb	= (data >> 10) & 0x1;
  parameters->r_error_w_ack	= (data >> 11) & 0x1;
  parameters->r_no_ack		= (data >> 12) & 0x1; 
/* one is missing -- to be vierified in the VHDL code */
  parameters->r_ack_not_rel	= (data >> 13) & 0x1;
  parameters->write_fsm_error	= (data >> 14) & 0x1;
  parameters->error_b_trsf	= (data >> 15) & 0x1;
  parameters->no_trsf		= (data >> 16) & 0x1;
  parameters->error_w_trsf	= (data >> 17) & 0x1;
  parameters->trsf_not_rel  	= (data >> 18) & 0x1; 
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERR_decode()> testpoint3"));
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_RDOERR_decode(u_long data, rdoerr_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->evlen_rdo_error	=  data & 0x01;
  parameters->error_b_trsf	= (data >> 1) & 0x01;
  parameters->no_trsf		= (data >> 2) & 0x01;
  parameters->wrong_ndstb	= (data >> 3) & 0x01;
  parameters->trsf_not_rel	= (data >> 4) & 0x01;
  parameters->scevlen_error	= (data >> 5) & 0x01;
  parameters->rd_rx_error       = (data >> 6) & 0x01;
  parameters->ch_add_mismatch   = (data >> 7) & 0x01;
  parameters->blen_mismatch     = (data >> 8) & 0x01;
  parameters->fsm_evlen		= (data >>20) & 0xf;
  parameters->fsm_wrA		= (data >>24) & 0xf;
  parameters->fsm_wrB		= (data >>28) & 0xf;
  return(RCU_SUCCESS);		
}

/************************************************************************************************/
int RCU_ALTROBUS_decode(u_int data, altrobus_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->cstb_A           = data & 0x1;
  parameters->ackn_A           = (data >> 1) & 0x1;
  parameters->trsf_A           = (data >> 2) & 0x1;
  parameters->cstb_B           = (data >> 3) & 0x1;
  parameters->ackn_B           = (data >> 4) & 0x1;
  parameters->trsf_B           = (data >> 5) & 0x1;
  parameters->L1	       = (data >> 6) & 0x1;
  parameters->L2               = (data >> 7) & 0x1;    
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_BUSSTTRSF_decode(u_int data, altrobus_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->cstb_A           = data & 0x1;
  parameters->ackn_A           = (data >> 1) & 0x1;
  parameters->trsf_A           = (data >> 2) & 0x1;
  parameters->cstb_B           = (data >> 3) & 0x1;
  parameters->ackn_B           = (data >> 4) & 0x1;
  parameters->trsf_B           = (data >> 5) & 0x1;
  parameters->L1	       = (data >> 6) & 0x1;
  parameters->L2               = (data >> 7) & 0x1;    
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_RCUBUSBUSY_decode(u_int data, rcubus_busy_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->IS_busy           = data & 0x1;
  parameters->IS_fail           = (data >> 1) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_RCUID_decode(u_int data, rcuid_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->side 	= (data >> 8) & 0x1;
  parameters->sector    = (data >> 3) & 0x1f;
  parameters->partition	= data & 0x7;
  return(RCU_SUCCESS);
}


/* TTC registers*/
/************************************************************************************************/
int RCU_CONTROL_decode(u_int data, control_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->sbc_on            = data & 0x1;
  parameters->dis_err_mask      = (data >> 1) & 0x1;
  parameters->en_roi_dec        = (data >> 2) & 0x1;
  parameters->l0_support        = (data >> 3) & 0x1;
  parameters->l2a_fifo_smask    = (data >> 8) & 0x1;
  parameters->l2r_fifo_smask    = (data >> 9) & 0x1;
  parameters->l2_tout_smask     = (data >> 10) & 0x1;
  parameters->l1_msg_mask       = (data >> 11) & 0x1;
  parameters->bunch_cnt_ovflw   = (data >> 16) & 0x1;
  parameters->run_active        = (data >> 17) & 0x1;
  parameters->busy              = (data >> 18) & 0x1;
  parameters->cdh_version       = (data >> 20) & 0xf;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_L1_LATENCY_decode(u_int data, l1_latency_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->l0_l1_latency       = data & 0xfff;
  parameters->accept_window      = (data >> 12) & 0xf;
  
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_LATENCY_decode(u_int data, latency_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->max_latency       = data & 0xffff;
  parameters->min_latency       = (data >> 16) & 0xffff;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_HAMNG_ERR_CNT_decode(u_int data, hamng_err_cnt_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->nr_sbit_err	= data & 0xffff;
  parameters->nr_dbit_err       = (data >> 16) & 0xffff;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_ERROR_CNT_decode(u_int data, error_cnt_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->nr_msgdec_err        = data & 0xffff;
  parameters->nr_seqtmovr_err      = (data >> 16) & 0xffff;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_EVENT_INFO_decode(u_int data, event_info_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->roi_en                = data & 0x1;
  parameters->roi_announced         = (data >> 1) & 0x1;
  parameters->roi_received          = (data >> 2) & 0x1;
  parameters->within_roi            = (data >> 3) & 0x1;
  parameters->calib_swt_trg         = (data >> 4) & 0xF;
  parameters->swt_event             = (data >> 8) & 0x1;
  parameters->cal_event             = (data >> 9) & 0x1;
  parameters->ev_l2r_trg            = (data >> 10) & 0x1;
  parameters->ev_l2a_trg            = (data >> 11) & 0x1;
  parameters->has_payload           = (data >> 12) & 0x1;
  parameters->sclk_phase_trg        = (data >> 13) & 0x1f;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_EVENT_ERROR_decode(u_int data, event_error_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->serB_stop_error  	= data & 0x1;
  parameters->hmng_sbit	 	= (data >>  1) & 0x1;
  parameters->hmng_dbit	 	= (data >>  2) & 0x1;
  parameters->hmng_sbit_bcast	= (data >>  3) & 0x1;
  parameters->hmng_dbit_bcast	= (data >>  4) & 0x1;
  parameters->wrong_msg_addr	= (data >>  5) & 0x1; 
  parameters->incompl_l1_msg	= (data >>  6) & 0x1;
  parameters->incompl_l2a_msg	= (data >>  7) & 0x1; 
  parameters->incompl_roi_msg	= (data >>  8) & 0x1; 
  parameters->ttcrx_addr_err	= (data >>  9) & 0x1; 
  parameters->spur_l0	 	= (data >> 10) & 0x1; 
  parameters->miss_l0		= (data >> 11) & 0x1; 
  parameters->spur_l1		= (data >> 12) & 0x1; 
  parameters->boundary_l1     	= (data >> 13) & 0x1; 
  parameters->miss_l1		= (data >> 14) & 0x1; 
  parameters->l1_out_window	= (data >> 15) & 0x1; 
  parameters->l1_msg_miss	= (data >> 16) & 0x1; 
  parameters->l2_out_window	= (data >> 17) & 0x1; 
  parameters->l2_msg_miss	= (data >> 18) & 0x1; 
  parameters->roi_out_window	= (data >> 19) & 0x1; 
  parameters->roi_msg_miss	= (data >> 20) & 0x1; 
  parameters->pre_pulse_err	= (data >> 21) & 0x1; 
  parameters->l1_msg_error	= (data >> 22) & 0x1; 
  parameters->l2_msg_error	= (data >> 23) & 0x1; 
  parameters->roi_msg_error	= (data >> 24) & 0x1; 
  
  return(RCU_SUCCESS);
}

/* Safety & Monitoring Module registers*/
/************************************************************************************************/
int RCU_MSM_ERROR_decode(u_int data, msm_error_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->err1	= data & 0x1;
  parameters->err2	= (data >> 1) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_MSM_INTMODE_decode(u_int data, msm_intmode_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->en_int_A	= data & 0x1;
  parameters->en_int_B	= (data >> 1) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_MSM_SCADD_decode(u_int data, msm_scadd_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->bc_reg_addr	= data & 0xff;
  parameters->fec_addr		= (data >>  8) & 0x1f;
  parameters->bcast		= (data >> 13) & 0x1;
  parameters->rnw		= (data >> 14) & 0x1;
  return(RCU_SUCCESS);
}


/* SIU Interface Module (SIF) */
/************************************************************************************************/
int RCU_SIF_STATUS_decode(u_int data, sif_status_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->sib_not_empty	= (data >>  4) & 0x1;
  parameters->sob_not_empty	= (data >>  5) & 0x1;
  parameters->sib_al_full	= (data >>  6) & 0x1;
  parameters->sob_al_full	= (data >>  7) & 0x1;
  parameters->sif_bus_req	= (data >>  8) & 0x1;
  parameters->sif_bus_grant	= (data >>  9) & 0x1;
  parameters->sif_bwr_active	= (data >> 12) & 0x1;
  parameters->sif_brd_active	= (data >> 13) & 0x1;
  parameters->sif_rdo_active	= (data >> 14) & 0x1;
  return(RCU_SUCCESS);
}

/************************************************************************************************/
int RCU_SIF_ERROR_decode(u_int data, sif_error_t *parameters)
/************************************************************************************************/
{
  RCUISOPEN;

  parameters->sif_wr_err	=  data        & 0x1;
  parameters->sif_rd_err	= (data >>  1) & 0x1;
  parameters->sif_rdo_err	= (data >>  2) & 0x1;
  parameters->ddl_err		= (data >>  3) & 0x1;
  parameters->sif_blen_err	= (data >>  4) & 0x1;
  parameters->sif_wr_tout	= (data >>  5) & 0x1;
  parameters->sif_rd_tout	= (data >>  6) & 0x1;
  return(RCU_SUCCESS);
}


/************************************************************************************************/
int RCU_ALTROIF_Get() 
/************************************************************************************************/
{
  altroif_t *parameters;
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROIF_Get(): Calling RCU_ALTROIF_Read()"));  
  ret = RCU_ALTROIF_Read(&data);
  if (ret) 
  {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ALTROIF_Read() failed:\t%d", ret));
    return ret;
  }
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROIF_Get(): RCU_ALTROIF_Read() successful"));  

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROIF_Get(): Calling RCU_ALTROIF_Decode()"));  
  ret = RCU_ALTROIF_decode(data, parameters);
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROIF_Get(): RCU_ALTROIF_Decode() successful"));  

  printf("\n\n\nContent of Register ALTROIF");   
  printf("\n----------------------------");   
  printf("\nNr. samples/event\t = %d", parameters->nsamples_ev); 
  printf("\nclk_ratio\t\t = %d", parameters->clk_ratio); 
  printf("\ncstb_delay\t\t = %d", parameters->cstb_delay); 
  printf("\nsafe mode\t\t = %d", parameters->safe_mode);
  printf("\n----------------------------\n\n\n");   
  
  return ret;
  }

/************************************************************************************************/
int RCU_TRGCONF_Get() 
/************************************************************************************************/
{
  trgconf_t *parameters;
  u_long data;
  u_int ret;

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_TRGCONF_Get(): Calling RCU_TRGCONF_Read()"));    
  ret = RCU_TRGCONF_Read(&data);
  if (ret)
  {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_TRGCONF_Read() failed:\t%d", ret));   
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_TRGCONF_Get()> Calling RCU_TRGCONF_Decode()"));  
  ret = RCU_TRGCONF_decode(data, parameters);

  printf("\n\n\nContent of Register TRGCONF");   
  printf("\n----------------------------");   
  printf("\nL1->L2 latency\t = %d", parameters->l2_latency); 
  printf("\nTrigger Mode\t = %d", parameters->trg_mode); 
  printf("\nTrigger source\t = %d", parameters->trg_source); 
  printf("\n----------------------------\n\n\n");     
  
  return ret;
}

/************************************************************************************************/
int RCU_RDOMOD_Get() 
/************************************************************************************************/
{
  rdomod_t *parameters;
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RDOMOD_Get()> Calling RCU_RDOMOD_Read()"));      
  ret = RCU_RDOMOD_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_RDOMOD_Get()> RCU_RDOMOD_Read() failed:\t%d", ret));     
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RDOMOD_Get()> Calling RCU_RDOMOD_Decode()"));    
  ret = RCU_RDOMOD_decode(data, parameters);
 
  printf("\n\n\nContent of Register RDOMOD");   
  printf("\n------------------------------------");   
  printf("\nMEB mode\t\t\t = %d", parameters->meb_mode); 
  printf("\nExec sequence upon SOD/EOD\t = %d", parameters->exec_seq); 
  printf("\nSparse Readout Enable\t\t = %d", parameters->sparse_rdo);
  printf("\nNo Check on RDYRX upon trigger\t = %d", parameters->check_rdyrx); 
  printf("\n------------------------------------\n\n\n");     
  
  return ret; 
}

/************************************************************************************************/
int RCU_ALTROCFG1_Get() 
/************************************************************************************************/
{
  altrocfg1_t *parameters;
  u_long data;
  u_int ret;

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROCFG1_Get()> Calling RCU_ALTROCFG1_Read()"));        
  ret = RCU_ALTROCFG1_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ALTROCFG1_Get()> RCU_ALTROCFG1_Read() failed:\t%d", ret));       
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROCFG1_Get()> Calling RCU_ALTROCFG1_Decode()"));    
  ret = RCU_ALTROCFG1_decode(data, parameters);
  
  printf("\n\n\nContent of Register ALTROCFG1");   
  printf("\n--------------------------------------------");   
  printf("\nBaseline Correction  I Configuration\t = %x", parameters->bc1_cfg); 
  printf("\nBaseline Correction II Configuration\t = %x", parameters->bc2_cfg); 
  printf("\nZero Suppression Configuration\t\t = %x", parameters->zs_cfg);
  printf("\n--------------------------------------------\n\n\n");     
    
  return ret;
}

/************************************************************************************************/
int RCU_ALTROCFG2_Get() 
/************************************************************************************************/
{
  altrocfg2_t *parameters;
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROCFG2_Get()> Calling RCU_ALTROCFG2_Read()"));        
  ret = RCU_ALTROCFG2_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ALTROCFG2_Get()> RCU_ALTROCFG2_Read() failed:\t%d", ret));       
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROCFG2_Get()> Calling RCU_ALTROCFG2_Decode()"));    
  ret = RCU_ALTROCFG2_decode(data, parameters);

  printf("\n\n\nContent of Register ALTROCFG2");   
  printf("\n----------------------------");   
  printf("\nNr. pre-trigger samples\t = %x", parameters->ptrg); 
  printf("\nNr. of buffers\t\t = %x", parameters->nbuf); 
  printf("\nFilter Enable\t\t = %x", parameters->flt_en);
  printf("\nPower save mode\t\t = %x", parameters->pwsv);
  printf("\n----------------------------\n\n\n");     
    
  return ret;  
}

/************************************************************************************************/
int RCU_RCU_VERSION_Get() 
/************************************************************************************************/
{
  rcu_version_t *parameters;
  u_long data;
  u_int ret;

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCU_VERSION_Get()> Calling RCU_RCU_VERSION_Read()"));        
  ret = RCU_RCU_VERSION_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_RCU_VERSION_Get()> RCU_RCU_VERSION_Read() failed:\t%d", ret));            
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCU_VERSION_Get()> Calling RCU_RCU_VERSION_Decode()"));    
  ret = RCU_RCU_VERSION_decode(data, parameters);

  printf("\n\n\nContent of Register RCU_VERSION");   
  printf("\n-------------------------------");   
  printf("\nYear\t = %02x", parameters->year); 
  printf("\nMonth\t = %02x", parameters->month); 
  printf("\nDay\t = %x", parameters->day);
  printf("\n-------------------------------\n\n\n");   
    
  return ret;    
}

/************************************************************************************************/
int RCU_FECERRA_Get() 
/************************************************************************************************/
{
/*  fecerr_t *parameters; */
  u_long data;
  u_int ret;
 
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERRA_Get()> Calling RCU_FECERRA_Read()"));        
  ret = RCU_FECERRA_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_FECERRA_Get()> RCU_FECERRA_Read() failed:\t%d", ret));              
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERRA_Get()> Calling RCU_FECERRB_Decode()"));    
  ret = RCU_FECERR_decode(data, &fecerra_s);
  
  printf("\n\n\nContent of Register FECERRA");   
  printf("\n----------------------------------------------------");   
  printf("\nALTRO Error was asserted\t\t\t = %x", fecerra_s.altro_error); 
  printf("\n----------------------------------------------------");   
  printf("\nWrite SM: ALTRO Error asserted before CSTB\t = %x", fecerra_s.w_error_b_cstb); 
  printf("\nWrite SM: ACKN asserted before CSTB\t\t = %x", fecerra_s.w_ack_b_cstb); 
  printf("\nWrite SM: ALTRO Error asserted before ACKN\t = %x", fecerra_s.w_error_b_ack);
  printf("\nWrite SM: ACKN not asserted\t\t\t = %x", fecerra_s.w_no_ack);
  printf("\nWrite SM: ALTRO Error asserted while ACKN\t = %x", fecerra_s.w_error_w_ack);  
  printf("\nWrite SM: ACKN not released\t\t\t = %x", fecerra_s.w_ack_not_rel);
  printf("\nWrite SM: ALTRO Error in exec of Broadcast\t = %x", fecerra_s.w_error_bcast);    
  printf("\n----------------------------------------------------");   
  printf("\nRead  SM: ALTRO Error asserted before CSTB\t = %x", fecerra_s.r_error_b_cstb); 
  printf("\nRead  SM: ACKN asserted before CSTB\t\t = %x", fecerra_s.r_ack_b_cstb); 
  printf("\nRead  SM: ALTRO Error asserted before ACKN\t = %x", fecerra_s.r_error_b_ack);
  printf("\nRead  SM: ACKN not asserted\t\t\t = %x", fecerra_s.r_no_ack);
  printf("\nRead  SM: ALTRO Error asserted while ACKN\t = %x", fecerra_s.r_error_w_ack);  
  printf("\nRead  SM: ACKN not released\t\t\t = %x", fecerra_s.r_ack_not_rel);
  printf("\n----------------------------------------------------");   
  printf("\nRdout SM: Write FSM Error\t\t\t = %x", fecerra_s.write_fsm_error); 
  printf("\nRdout SM: ALTRO Error asserted before TRSF\t = %x", fecerra_s.error_b_trsf); 
  printf("\nRdout SM: TRSF not asserted\t\t\t = %x", fecerra_s.no_trsf);
  printf("\nRdout SM: ALTRO Error asserted while TRSF\t = %x", fecerra_s.error_w_trsf);
  printf("\nRdout SM: TRSF not released\t\t\t = %x", fecerra_s.trsf_not_rel);  
  printf("\n----------------------------------------------------\n\n\n");   
  
  printf("\nreturn = %d\n", ret);  
  return ret;      
}

/************************************************************************************************/
int RCU_FECERRB_Get() 
/************************************************************************************************/
{
/*  fecerr_t *parameters; */
  u_long data;
  u_int ret;
 
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERRB_Get()> Calling RCU_FECERRB_Read()"));        
  ret = RCU_FECERRB_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_FECERRB_Get()> RCU_FECERRB_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_FECERRB_Get()> Calling RCU_FECERRB_Decode()"));    
  ret = RCU_FECERR_decode(data, &fecerrb_s);
  
  printf("\n\n\nContent of Register FECERRB");   
  printf("\n----------------------------------------------------");   
  printf("\nALTRO Error was asserted\t\t\t = %x", fecerrb_s.altro_error); 
  printf("\n----------------------------------------------------");   
  printf("\nWrite SM: ALTRO Error asserted before CSTB\t = %x", fecerrb_s.w_error_b_cstb); 
  printf("\nWrite SM: ACKN asserted before CSTB\t\t = %x", fecerrb_s.w_ack_b_cstb); 
  printf("\nWrite SM: ALTRO Error asserted before ACKN\t = %x", fecerrb_s.w_error_b_ack);
  printf("\nWrite SM: ACKN not asserted\t\t\t = %x", fecerrb_s.w_no_ack);
  printf("\nWrite SM: ALTRO Error asserted while ACKN\t = %x", fecerrb_s.w_error_w_ack);  
  printf("\nWrite SM: ACKN not released\t\t\t = %x", fecerrb_s.w_ack_not_rel);
  printf("\nWrite SM: ALTRO Error in exec of Broadcast\t = %x", fecerrb_s.w_error_bcast);    
  printf("\n----------------------------------------------------");   
  printf("\nRead  SM: ALTRO Error asserted before CSTB\t = %x", fecerrb_s.r_error_b_cstb); 
  printf("\nRead  SM: ACKN asserted before CSTB\t\t = %x", fecerrb_s.r_ack_b_cstb); 
  printf("\nRead  SM: ALTRO Error asserted before ACKN\t = %x", fecerrb_s.r_error_b_ack);
  printf("\nRead  SM: ACKN not asserted\t\t\t = %x", fecerrb_s.r_no_ack);
  printf("\nRead  SM: ALTRO Error asserted while ACKN\t = %x", fecerrb_s.r_error_w_ack);  
  printf("\nRead  SM: ACKN not released\t\t\t = %x", fecerrb_s.r_ack_not_rel);
  printf("\n----------------------------------------------------");   
  printf("\nRdout SM: Write FSM Error\t\t\t = %x", fecerrb_s.write_fsm_error); 
  printf("\nRdout SM: ALTRO Error asserted before TRSF\t = %x", fecerrb_s.error_b_trsf); 
  printf("\nRdout SM: TRSF not asserted\t\t\t = %x", fecerrb_s.no_trsf);
  printf("\nRdout SM: ALTRO Error asserted while TRSF\t = %x", fecerrb_s.error_w_trsf);
  printf("\nRdout SM: TRSF not released\t\t\t = %x", fecerrb_s.trsf_not_rel); 
  printf("\n----------------------------------------------------\n\n\n");   
    
  printf("\nreturn = %d\n", ret);  
  return ret;      
}

/************************************************************************************************/
int RCU_RDOERR_Get() 
/************************************************************************************************/
{
/*  rdoerr_t *parameters; */
  u_long data;
  u_int  ret;
  
  char *FSM_WR[16] = {"IDLE","SAVE_DATA",&"WAIT_EVLEN","ASSERTBUS","WAITACKN","CLRCNT","WAITBC","CHECK_ACKN",\
                 "UNDEFINED","UNDEFINED","UNDEFINED","UNDEFINED","WAITDONE","ENDWRITE","UNDEFINED","DLY1CYCLE"}; 
  
  char *FSM_EVLEN[16] = {"IDLE","CHECK_HLM_OVER","CHECK_FEC","FINISH","WAIT_DONE","WAIT_DATA","END_EVLEN","EVLEN_RDO",\
                    "UNDEFINED","UNDEFINED","UNDEFINED","UNDEFINED","DELAY","UNDEFINED","UNDEFINED","UNDEFINED"}; 


  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RDOERR_Get()> Calling RCU_RDOERR_Read()"));        
  ret = RCU_RDOERR_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_RDOERR_Get()> RCU_RDOERR_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RDOERR_Get()> Calling RCU_RDOERR_Decode()"));    
  ret = RCU_RDOERR_decode(data, &rdoerr_s);

  printf("\n\n\nContent of Register RDOERR");   
  printf("\n----------------------------------------------------");   
  printf("\nChannel Address Mismatch\t\t\t = %x", rdoerr_s.ch_add_mismatch); 
  printf("\nBlock Length Mismatch\t\t\t\t = %x", rdoerr_s.blen_mismatch); 
  printf("\nEvent Length Readout Error\t\t\t = %x", rdoerr_s.evlen_rdo_error);
  printf("\nALTRO Error asserted before TRSF\t\t = %x", rdoerr_s.error_b_trsf);
  printf("\nTRSF not asserted\t\t\t\t = %x", rdoerr_s.no_trsf);  
  printf("\nWrong number of DSTB (not 4)\t\t\t = %x", rdoerr_s.wrong_ndstb);
  printf("\nTRSF not released\t\t\t\t = %x", rdoerr_s.trsf_not_rel);    
  printf("\nScan Event Length Error\t\t\t\t = %x", rdoerr_s.scevlen_error); 
  printf("\nRDYRX signal not asserted or released too early\t = %x", rdoerr_s.rd_rx_error);
  printf("\nEVENT LENGTH READOUT FSM\t\t\t = %s", FSM_EVLEN[rdoerr_s.fsm_evlen]); 
  printf("\nABD BRANCH A WR FSM\t\t\t\t = %s", FSM_WR[rdoerr_s.fsm_wrA]); 
  printf("\nABD BRANCH B WR FSM\t\t\t\t = %s", FSM_WR[rdoerr_s.fsm_wrB]);   
  printf("\n----------------------------------------------------\n\n\n");     
    
  return ret;  
}


/************************************************************************************************/
int RCU_ALTROBUS_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROBUS_Get()> Calling RCU_ALTROBUS_Read()"));        
  ret = RCU_ALTROBUS_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ALTROBUS_Get()> RCU_ALTROBUS_Read() failed:\t%d", ret));              
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ALTROBUS_Get()> Calling RCU_ALTROBUS_Decode()"));    
  ret = RCU_ALTROBUS_decode(data, &altrobus_s);

  printf("\n\n\nContent of Register ALTROBUS");   
  printf("\n--------------------");   
  printf("\ncstb branch A\t = %x", altrobus_s.cstb_A); 
  printf("\nackn branch A\t = %x", altrobus_s.ackn_A); 
  printf("\ntrsf branch A\t = %x", altrobus_s.trsf_A); 
  printf("\ncstb branch B\t = %x", altrobus_s.cstb_B); 
  printf("\nackn branch B\t = %x", altrobus_s.ackn_B); 
  printf("\ntrsf branch B\t = %x", altrobus_s.trsf_B); 
  printf("\nL1 branch A & B\t = %x", altrobus_s.L1); 
  printf("\nL2 branch A & B\t = %x", altrobus_s.L2);   
  printf("\n--------------------\n\n\n");     
    
  return ret;    
}

/************************************************************************************************/
int RCU_BUSSTTRSF_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_BUSSTTRSF_Get()> Calling RCU_BUSSTTRSF_Read()"));        
  ret = RCU_BUSSTTRSF_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_BUSSTTRSF_Get()> RCU_BUSSTTRSF_Read() failed:\t%d", ret));              
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_BUSSTTRSF_Get()> Calling RCU_BUSSTTRSF_Decode()"));    
  ret = RCU_BUSSTTRSF_decode(data, &bussttrsf_s);

  printf("\n\n\nContent of Register BUSSTTRSF");   
  printf("\n------------------------------------");   
  printf("\ncstb branch A upon trsf\t\t = %x", bussttrsf_s.cstb_A); 
  printf("\nackn branch A upon trsf\t\t = %x", bussttrsf_s.ackn_A); 
  printf("\ntrsf branch A upon trsf\t\t = %x", bussttrsf_s.trsf_A); 
  printf("\ncstb branch B upon trsf\t\t = %x", bussttrsf_s.cstb_B); 
  printf("\nackn branch B upon trsf\t\t = %x", bussttrsf_s.ackn_B); 
  printf("\ntrsf branch B upon trsf\t\t = %x", bussttrsf_s.trsf_B); 
  printf("\nL1 branch A & B upon trsf\t = %x", bussttrsf_s.L1); 
  printf("\nL2 branch A & B upon trsf\t = %x", bussttrsf_s.L2);   
  printf("\n------------------------------------\n\n\n");     
    
  return ret;    
}

/************************************************************************************************/
int RCU_RCUBUSBUSY_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCUBUSBUSY_Get()> Calling RCU_RCUBUSBUSY_Read()"));        
  ret = RCU_RCUBUSBUSY_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_RCUBUSBUSY_Get()> RCU_RCUBUSBUSY_Read() failed:\t%d", ret));              
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCUBUSBUSY_Get()> Calling RCU_RCUBUSBUSY_Decode()"));    
  ret = RCU_RCUBUSBUSY_decode(data, &rcubus_busy_s);

  printf("\n\n\nContent of Register RCUBUSBUSY");   
  printf("\n------------------------------------");   
  printf("\nInstruction Sequencer busy\t = %x", rcubus_busy_s.IS_busy); 
  printf("\nInstruction Sequencer fail\t = %x", rcubus_busy_s.IS_fail); 
  printf("\n------------------------------------\n\n\n");     
    
  return ret;    
}

/************************************************************************************************/
int RCU_RCUID_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCUID_Get()> Calling RCU_RCUID_Read()"));        
  ret = RCU_RCUID_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_RCUID_Get()> RCU_RCUID_Read() failed:\t%d", ret));              
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_RCUID_Get()> Calling RCU_RCUID_Decode()"));    
  ret = RCU_RCUID_decode(data, &rcuid_s);

  printf("\n\n\nContent of Register RCUID");   
  printf("\n------------------------------------");   
  printf("\nTPC Side (A=0, C=1)\t\t = %d", rcuid_s.side); 
  printf("\nSector (0->17)\t\t\t = %d", rcuid_s.sector); 
  printf("\nReadout Partition (0->5)\t = %d", rcuid_s.partition); 
  printf("\n------------------------------------\n\n\n");     
    
  return ret;    
}

/************************************************************************************************/
int RCU_SIF_STATUS_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_SIF_STATUS_Get()> Calling RCU_SIF_STATUS_Read()"));        
  ret = RCU_SIFSTATUS_Read(&data);
  if (ret) { 
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_SIF_STATUS_Get()> RCU_SIF_STATUS_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_SIF_STATUS_Get()> Calling RCU_SIF_STATUS_Decode()"));    
  ret = RCU_SIF_STATUS_decode(data, &sif_status_s);

  printf("\n\n\nContent of Register SIF_STATUS");   
  printf("\n----------------------------");   
  printf("\nSIB not empty\t\t = %x", sif_status_s.sib_not_empty); 
  printf("\nSOB not empty\t\t = %x", sif_status_s.sob_not_empty); 
  printf("\nSIB almost full\t\t = %x", sif_status_s.sib_al_full); 
  printf("\nSOB almost full\t\t = %x", sif_status_s.sob_al_full); 
  printf("\nSIF bus request\t\t = %x", sif_status_s.sif_bus_req); 
  printf("\nSIF has bus grant\t = %x", sif_status_s.sif_bus_grant); 
  printf("\nSIF Block write active\t = %x", sif_status_s.sif_bwr_active); 
  printf("\nSIF Block read active\t = %x", sif_status_s.sif_brd_active); 
  printf("\nSIF readout active\t = %x", sif_status_s.sif_rdo_active); 
  printf("\n----------------------------\n\n\n");     
    
  return ret;  
}

/************************************************************************************************/
int RCU_SIF_ERROR_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_SIF_ERROR_Get()> Calling RCU_SIF_ERROR_Read()"));        
  ret = RCU_SIFERRORS_Read(&data); 
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_SIF_ERROR_Get()> RCU_SIF_ERROR_Read() failed:\t%d", ret));              
    return ret;
  }
      
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_SIF_ERROR_Get()> Calling RCU_SIF_ERROR_Decode()"));    
  ret = RCU_SIF_ERROR_decode(data, &sif_error_s);
  
  printf("\n\n\nContent of Register SIF_ERROR");   
  printf("\n-----------------------------");   
  printf("\nSIF block write error\t = %x", sif_error_s.sif_wr_err); 
  printf("\nSIF block read error\t = %x", sif_error_s.sif_rd_err); 
  printf("\nSIF readout error\t = %x", sif_error_s.sif_rdo_err); 
  printf("\nSIF DDL error\t\t = %x", sif_error_s.ddl_err); 
  printf("\nSIF block length error\t = %x", sif_error_s.sif_blen_err); 
  printf("\nSIF write timeout\t = %x", sif_error_s.sif_wr_tout); 
  printf("\nSIF read timeout\t = %x", sif_error_s.sif_rd_tout); 
  printf("\n-----------------------------\n\n\n");     
    
  return ret;   
}

/************************************************************************************************/
int RCU_CONTROL_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_CONTROL_Get()> Calling RCU_CONTROL_Read()"));        
  ret = RCU_CONTROL_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_CONTROL_Get()> RCU_CONTROL_Read() failed:\t%d", ret));                
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_CONTROL_Get()> Calling RCU_CONTROL_Decode()"));      
  ret = RCU_CONTROL_decode(data, &control_s);
  
  printf("\n\n\nContent of Register CONTROL");   
  printf("\n----------------------------");   
  printf("\nSerial B channel ON\t = %x", control_s.sbc_on); 
  printf("\nDisable error mask\t = %x", control_s.dis_err_mask); 
  printf("\nEnable RoI decoding\t = %x", control_s.en_roi_dec); 
  printf("\nL0 via TTC\t\t = %x", control_s.l0_support); 
  printf("\nL2A FIFO storage mask\t = %x", control_s.l2a_fifo_smask); 
  printf("\nL2R FIFO storage mask\t = %x", control_s.l2r_fifo_smask); 
  printf("\nL2 timeout storage mask\t = %x", control_s.l2_tout_smask); 
  printf("\nL1a message mask\t = %x", control_s.l1_msg_mask);   
  printf("\nBunch counter overflow\t = %x", control_s.bunch_cnt_ovflw); 
  printf("\nRun Active\t\t = %x", control_s.run_active); 
  printf("\nReceiving trigger seq\t = %x", control_s.busy); 
  printf("\nCDH version\t\t = %x", control_s.cdh_version); 
  printf("\n----------------------------\n\n\n");     
    
  return ret;    
}

/************************************************************************************************/
int RCU_L1_LATENCY_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L1_LATENCY_Get()> Calling RCU_L1_LATENCY_Read()"));        
  ret = RCU_L1_LATENCY_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_L1_LATENCY_Get()> RCU_L1_LATENCY_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L1_LATENCY_Get()> Calling RCU_L1_LATENCY_Decode()"));      
  ret = RCU_L1_LATENCY_decode(data, &l1_latency_s);
  
  printf("\n\n\nContent of Register L1_LATENCY");   
  printf("\n-------------------------------------");   
  printf("\nL0->L1 latency\t\t\t = %x", l1_latency_s.l0_l1_latency); 
  printf("\n+-N cycles around L0->L1\t = %x", l1_latency_s.accept_window); 
  printf("\n-------------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_L2_LATENCY_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L2_LATENCY_Get()> Calling RCU_L2_LATENCY_Read()"));        
  ret = RCU_L2_LATENCY_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_L2_LATENCY_Get()> RCU_L2_LATENCY_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L2_LATENCY_Get()> Calling RCU_LATENCY_Decode()"));      
  ret = RCU_LATENCY_decode(data, &l2_latency_s);

  printf("\n\n\nContent of Register L2_LATENCY");   
  printf("\n------------------------------");   
  printf("\nMax latency\t = %x", l2_latency_s.max_latency); 
  printf("\nMin latency\t = %x", l2_latency_s.min_latency); 
  printf("\n------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_ROI_LATENCY_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ROI_LATENCY_Get()> Calling RCU_ROI_LATENCY_Read()"));        
  ret = RCU_ROI_LATENCY_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ROI_LATENCY_Get()> RCU_ROI_LATENCY_Read() failed:\t%d", ret));              
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ROI_LATENCY_Get()> Calling RCU_LATENCY_Decode()"));      
  ret = RCU_LATENCY_decode(data, &roi_latency_s);

  printf("\n\n\nContent of Register ROI_LATENCY");   
  printf("\n-------------------------------");   
  printf("\nMax latency\t = %x", roi_latency_s.max_latency); 
  printf("\nMin latency\t = %x", roi_latency_s.min_latency); 
  printf("\n-------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_L1_MSG_LATENCY_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L1_MSG_LATENCY_Get()> Calling RCU_L1_MSG_LATENCY_Read()"));        
  ret = RCU_L1_MSG_LATENCY_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_L1_MSG_LATENCY_Get()> RCU_L1_MSG_LATENCY_Read() failed:\t%d", ret));		 
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_L1_MSG_LATENCY_Get()> Calling RCU_LATENCY_Decode()"));      
  ret = RCU_LATENCY_decode(data, &l1_msg_latency_s);

  printf("\n\n\nContent of Register L1_MSG_LATENCY");   
  printf("\n----------------------------------");   
  printf("\nMax latency\t = 0x%x", l1_msg_latency_s.max_latency); 
  printf("\nMin latency\t = 0x%x", l1_msg_latency_s.min_latency); 
  printf("\n----------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_HAMNG_ERR_CNT_Get(hamng_err_cnt_t *parameters) 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  ret = RCU_HAMNG_ERR_CNT_Read(&data);
  if (ret) 
    return ret;
  return RCU_HAMNG_ERR_CNT_decode(data, parameters);
}

/************************************************************************************************/
int RCU_ERROR_CNT_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ERROR_CNT_Get()> Calling RCU_ERROR_CNT_Read()"));        
  ret = RCU_ERROR_CNT_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_ERROR_CNT_Get()> RCU_ERROR_CNT_Read() failed:\t%d", ret));		 
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_ERROR_CNT_Get()> Calling RCU_ERROR_CNT_Decode()"));      
  ret = RCU_ERROR_CNT_decode(data, &error_cnt_s);

  printf("\n\n\nContent of Register ERROR_CNT");   
  printf("\n----------------------------------------------------------");   
  printf("\nNr of message decoding errors\t\t\t = 0x%x", error_cnt_s.nr_msgdec_err); 
  printf("\nNr of errors related to sequence and timeout\t = 0x%x", error_cnt_s.nr_seqtmovr_err); 
  printf("\n----------------------------------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_EVENT_INFO_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_EVENT_INFO_Get()> Calling RCU_EVENT_INFO_Read()"));        
  ret = RCU_EVENT_INFO_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_EVENT_INFO_Get()> RCU_EVENT_INFO_Read() failed:\t%d", ret));		 
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_EVENT_INFO_Get()> Calling RCU_EVENT_INFO_Decode()"));      
  ret = RCU_EVENT_INFO_decode(data, &event_info_s);
  
  printf("\n\n\nContent of Register EVENT_INFO");   
  printf("\n--------------------------------------");   
  printf("\nRoI Enable\t\t\t = 0x%x", event_info_s.roi_en); 
  printf("\nRoI announced\t\t\t = 0x%x", event_info_s.roi_announced); 
  printf("\nRoI received\t\t\t = 0x%x", event_info_s.roi_received); 
  printf("\nWithin RoI\t\t\t = 0x%x", event_info_s.within_roi); 
  printf("\nCalibration or Software Trigger\t = 0x%x", event_info_s.calib_swt_trg); 
  printf("\nSoftware Trigger Event\t\t = 0x%x", event_info_s.swt_event); 
  printf("\nCalibration Trigger Event\t = 0x%x", event_info_s.cal_event); 
  printf("\nEvent with L2 reject\t\t = 0x%x", event_info_s.ev_l2r_trg); 
  printf("\nEvent with L2a\t\t\t = 0x%x", event_info_s.ev_l2a_trg); 
  printf("\nInclude payload\t\t\t = 0x%x", event_info_s.has_payload); 
  printf("\nPhase of SCLK wrt trigger\t = 0x%x", event_info_s.sclk_phase_trg); 
  printf("\n--------------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_EVENT_ERROR_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_EVENT_ERROR_Get()> Calling RCU_EVENT_ERROR_Read()"));        
  ret = RCU_EVENT_ERROR_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_EVENT_ERROR_Get()> RCU_EVENT_ERROR_Read() failed:\t%d", ret));		 
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_EVENT_ERROR_Get()> Calling RCU_EVENT_ERROR_Decode()"));      
  ret = RCU_EVENT_ERROR_decode(data, &event_error_s);

  printf("\n\n\nContent of Register EVENT_ERROR");   
  printf("\n----------------------------------------------------------");   
  printf("\nSerial B stop bit error\t\t\t\t = 0x%x", event_error_s.serB_stop_error); 
  printf("\nHamming single-bit error (individ. addressed)\t = 0x%x", event_error_s.hmng_sbit); 
  printf("\nHamming double-bit error (individ. addressed)\t = 0x%x", event_error_s.hmng_dbit); 
  printf("\nHamming single-bit error (broadcast)\t\t = 0x%x", event_error_s.hmng_sbit_bcast); 
  printf("\nHamming double-bit error (broadcast)\t\t = 0x%x", event_error_s.hmng_dbit_bcast); 
  printf("\nWrong Message Address\t\t\t\t = 0x%x", event_error_s.wrong_msg_addr); 
  printf("\nIncomplete L1 message\t\t\t\t = 0x%x", event_error_s.incompl_l1_msg); 
  printf("\nIncomplete L2a message\t\t\t\t = 0x%x", event_error_s.incompl_l2a_msg); 
  printf("\nIncomplete RoI message\t\t\t\t = 0x%x", event_error_s.incompl_roi_msg); 
  printf("\nTTCrx Address error (not 0x0003)\t\t = 0x%x", event_error_s.ttcrx_addr_err); 
  printf("\nSpurious L0\t\t\t\t\t = 0x%x", event_error_s.spur_l0); 
  printf("\nMissing L0\t\t\t\t\t = 0x%x", event_error_s.miss_l0); 
  printf("\nSpurious L1\t\t\t\t\t = 0x%x", event_error_s.spur_l1); 
  printf("\nL1 outside aceptance window\t\t\t = 0x%x", event_error_s.boundary_l1); 
  printf("\nMissing L1\t\t\t\t\t = 0x%x", event_error_s.miss_l1); 
  printf("\nL1 message outside legal timeslot\t\t = 0x%x", event_error_s.l1_out_window); 
  printf("\nL1 message missing or timeout\t\t\t = 0x%x", event_error_s.l1_msg_miss); 
  printf("\nL2 message outside legal timeslot\t\t = 0x%x", event_error_s.l2_out_window); 
  printf("\nL2 message missing or timeout\t\t\t = 0x%x", event_error_s.l2_msg_miss); 
  printf("\nRoI message outside legal timeslot\t\t = 0x%x", event_error_s.roi_out_window); 
  printf("\nRoI message missing or timeout\t\t\t = 0x%x", event_error_s.roi_msg_miss); 
  printf("\nPrepulse error (=0 possible future use)\t\t = 0x%x", event_error_s.pre_pulse_err); 
  printf("\nL1 message content error\t\t\t = 0x%x", event_error_s.l1_msg_error); 
  printf("\nL2 message content error\t\t\t = 0x%x", event_error_s.l2_msg_error); 
  printf("\nRoI message content error\t\t\t = 0x%x", event_error_s.roi_msg_error); 
  printf("\n----------------------------------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_MSM_ERROR_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_ERROR_Get()> Calling RCU_MSM_ERROR_Read()"));        
  ret = RCU_MSM_ERROR_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_MSM_ERROR_Get()> RCU_MSM_ERROR_Read() failed:\t%d", ret));		 
    return ret;
  }

  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_ERROR_Get()> Calling RCU_MSM_ERROR_Decode()"));      
  ret = RCU_MSM_ERROR_decode(data, &msm_error_s);

  printf("\n\n\nContent of Register MSM_ERROR");   
  printf("\n------------------------------");   
  printf("\nError 1 (tbc)\t = 0x%x", msm_error_s.err1); 
  printf("\nError 2 (tbc)\t = 0x%x", msm_error_s.err2); 
  printf("\n------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_MSM_INTMODE_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_INTMODE_Get()> Calling RCU_MSM_INTMODE_Read()"));        
  ret = RCU_MSM_INTMODE_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_MSM_INTMODE_Get()> RCU_MSM_INTMODE_Read() failed:\t%d", ret));		 
    return ret;
  }
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_INTMODE_Get()> Value returned by RCU_MSM_INTMODE_Read(): 0x%x", data));        
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_INTMODE_Get()> Calling RCU_MSM_INTMODE_Decode()"));      
  ret = RCU_MSM_INTMODE_decode(data, &msm_intmode_s);
  
  printf("\n\n\nContent of Register MSM_INTMODE");   
  printf("\n----------------------------------------");   
  printf("\nEnable interrupt Branch A\t = 0x%x", msm_intmode_s.en_int_A); 
  printf("\nEnable interrupt Branch B\t = 0x%x", msm_intmode_s.en_int_B); 
  printf("\n----------------------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
int RCU_MSM_SCADD_Get() 
/************************************************************************************************/
{
  u_long data;
  u_int ret;
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_SCADD_Get()> Calling RCU_MSM_SCADD_Read()"));        
  ret = RCU_MSM_SCADD_Read(&data);
  if (ret) {
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_MSM_SCADD_Get()> RCU_MSM_SCADD_Read() failed:\t%d", ret));		 
    return ret;
  }
  
  DEBUG_TEXT(P_ID_RCU, 80, ("RCU_MSM_SCADD_Get()> Calling RCU_MSM_SCADD_Decode()"));      
  ret = RCU_MSM_SCADD_decode(data, &msm_scadd_s);
  
  printf("\n\n\nContent of Register MSM_SCADD");   
  printf("\n------------------------------------");   
  printf("\nBC Internal Address\t = 0x%x", msm_scadd_s.bc_reg_addr); 
  printf("\nFEC address\t\t = 0x%x", msm_scadd_s.fec_addr); 
  printf("\nBroadcast mode\t\t = 0x%x", msm_scadd_s.bcast); 
  printf("\nRead no Write\t\t = 0x%x", msm_scadd_s.rnw); 
  printf("\n---------- -------------------------\n\n\n");     
    
  return ret;     
}

/************************************************************************************************/
/************************************************************************************************/
/*			SECTION	4 	 C O M M A N D S					*/
/************************************************************************************************/
/************************************************************************************************/

/************************************************************************************************/
int
RCU_Exec_Command(u_long command, u_int data)
/************************************************************************************************/
{
  u_int valid_core_command = 0;
  u_int valid_sif_command = 0;
  u_int valid_command;
  int ret;

  RCUISOPEN;

  DEBUG_TEXT(P_ID_RCU, 10, ("RCU_Exec_Command: Function called for command = %x\n", command));

  /* RCU CORE Commands  */
  if ( ((command >= A_CONFEC)  && (command <= A_CLRTRGCNT)) || (command == A_ARBITERIRQ) )
     valid_core_command = 1; 
      
  /* RCU TTC_Receiver Commands */ 
  else if ( ( command == A_TMODULE_RESET)  || (command == A_TRESET_COUNTERS) || \
            (command == A_TISSUE_TEST_MODE) || (command == A_TFIFO_READ_EN) )
     valid_core_command =1; 

  /* RCU MSM Commands  */
  else if ( (command == A_MSM_SCCMD1) || (command == A_MSM_ERROR_RST) || \
            (command == A_MSM_RESULT_RST) )
     valid_core_command = 1;
     
  else if (command == A_MSM_SCCMD2)
  {
     valid_core_command = 1;
     command = command || (data & 0xfff);
  }
  
  /* SIF Commands */  
  else if ( (command == A_SIFRESET) || (command == A_RCURESET) || (command == A_RESETALL) || \
            (command == A_FECRESET) || (command == A_CLEARSIB) || (command == A_CLEARSOB) || \
	    (command == A_CLEARERR) )
     valid_sif_command = 1; 

  else if (command == A_SETBLREG)
  { 
     valid_sif_command = 1; 
     command = command || (data & 0xffff);
  }

  valid_command = valid_core_command | valid_sif_command;

  if (!valid_command)
  {    
    DEBUG_TEXT(P_ID_RCU, 5, ("RCU_Exec_Command: Unknown command %x\n", command));
    return(RCU_RANGE); 
  }
  else if (valid_core_command)
     return RCU_Reg_Write(command, 0x0);
  else
  {
     ret = ddl_sendCommand(rorc, command);
     if(ret < 0){
        DEBUG_TEXT(P_ID_RCU, 5, ("\nRCU_Lib: RCU_Exec_Command() error meassage\nError in ddl_sendCommand: %i\n",ret));
        return (RCU_FEC2RORC_FAIL);        
     }
     return ret;
  }
}  

/************************************************************************************************/
int RCU_Exec_CONFEC()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CONFEC, 0);
} 

/************************************************************************************************/
int RCU_Exec_EXECSEQ()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_EXECSEQ, 0);
} 

/************************************************************************************************/
int RCU_Exec_ABORTSEQ()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_ABORTSEQ, 0);
} 

/************************************************************************************************/
int RCU_Exec_SWTTRG()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_SWTTRG, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLRREGS()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLRREGS, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLRRDRX()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLRRDRX, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLRTRGCNT()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLRTRGCNT, 0);
} 

/************************************************************************************************/
int RCU_Exec_ARBITERIRQ()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_ARBITERIRQ, 0);
} 

/************************************************************************************************/
int RCU_Exec_SIFRESET()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_SIFRESET, 0);
} 

/************************************************************************************************/
int RCU_Exec_RCURESET()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_RCURESET, 0);
} 

/************************************************************************************************/
int RCU_Exec_RESETALL()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_RESETALL, 0);
} 

/************************************************************************************************/
int RCU_Exec_FECRESET()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_FECRESET, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLEARSIB()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLEARSIB, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLEARSOB()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLEARSOB, 0);
} 

/************************************************************************************************/
int RCU_Exec_CLEARERR()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_CLEARERR, 0);
} 

/************************************************************************************************/
int RCU_Exec_SETBLREG(u_int data)
/************************************************************************************************/
{
  return RCU_Exec_Command(A_SETBLREG, data);
} 

/************************************************************************************************/
int RCU_Exec_TMODULE_RESET()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_TMODULE_RESET, 0);
} 

/************************************************************************************************/
int RCU_Exec_TRESET_COUNTERS()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_TRESET_COUNTERS, 0);
} 

/************************************************************************************************/
int RCU_Exec_TISSUE_TEST_MODE()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_TISSUE_TEST_MODE, 0);
} 

/************************************************************************************************/
int RCU_Exec_TFIFO_READ_EN()
/************************************************************************************************/
{
  return RCU_Exec_Command(A_TFIFO_READ_EN, 0);
} 

#endif
