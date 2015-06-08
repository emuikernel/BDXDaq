
#ifdef Linux

/****************************************************************/
/*                                                              */
/*  file: fec_lib.c                                             */
/*                                                              */
/* Library of functions to access the FEC v2.0 via the DDL 	*/
/*								*/
/* The Library contains  main sections				*/
/* SECTION 1:  							*/
/* SECTION 2:  							*/
/* SECTION 3:  							*/
/* SECTION 4:  							*/
/*								*/
/* Author: 		L. Musa (CERN)                          */
/* Created 		22 May 2008				*/
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

#include "fec2rorc_lib.h"
#include "rcu.h"
#include "fec.h"

#include "rcc_error.h"
#include "rcc_time_stamp.h"

    
/************************/
/* 	Globals 	*/
/************************/
u_int rcu_is_open;
u_int packageId=102;
u_int traceLevel=0;

/************************************************************************************************/
u_int FEC_Address_Build(u_int bc_al, u_int bcast, u_int fec_add, u_int ch_add, u_int reg_add)   
/************************************************************************************************/
{ 
  u_int address;
  u_int bit, par;
  u_int ii;
  
  if(bc_al) 
    ch_add = 0;
    
  address = (bcast<<18) | (bc_al<<17) | (fec_add<<12) | (ch_add<<5) | reg_add; 
  
  /* caluculate the parity bit */
  bit = 0;
  for(ii =0; ii<18; ii++) {
     bit = (address >> ii) & 0x1;
     par = (par + bit) & 0x1;
  }   
  
  return address;
}

/************************************************************************************************/
int FEC_Sequence_Build(u_int imcode, u_int address, u_int data, u_int *im_seq)   
/************************************************************************************************/
{ 
  int ret;
  
  im_seq[0] = (imcode << 20) | address;

  if(imcode == FEC_WR)
  {
     im_seq[1] = data;
     im_seq[2] = (IS_CMD << 20) | ENDSEQ;
     DEBUG_TEXT(P_ID_FEC, 50, ("FEC_Sequence_Build()>\nim_seq[0] = 0x%x\nim_seq[1] = 0x%x\nim_seq[2] = 0x%x", \
                                im_seq[0], im_seq[1], im_seq[2]));
  }
  else
  {
     im_seq[1] = (IS_CMD << 20) | ENDSEQ;
     DEBUG_TEXT(P_ID_FEC, 50, ("FEC_Sequence_Build()>\nim_seq[0] = 0x%x\nim_seq[1] = 0x%x", \
                                im_seq[0], im_seq[1]));
  }  
  return (0);
  
}
    
/************************************************************************************************/
int FEC_WPMEM_Sequence_Build(u_int address, u_int offset, u_int nwords, u_int *ped_data, u_int *im_seq)   
/************************************************************************************************/
{ 
  int ret;
  u_int n, ii;
  u_int chip_add=0, ch_add=0;
   
  chip_add = (address >> 4) & 0xff;
  ch_add = address & 0xf;

  for(n=0; n<nwords; n++)
  {
    ii = 4*n;
    im_seq[ii]	 = (FEC_WR << 20) | (chip_add << 9) | A_PMADD;
    im_seq[ii+1] =  n + offset;
    im_seq[ii+2] = (FEC_WR << 20) | (chip_add << 9) | (ch_add<<5) | A_PMDTA;
    im_seq[ii+3] =  ped_data[n];
  }
  im_seq[4*nwords] = (IS_CMD << 20) | ENDSEQ;  
  return (0);  
}         

/************************************************************************************************/
int FEC_RPMEM_Sequence_Build(u_int address, u_int offset, u_int nwords, u_int *im_seq)   
/************************************************************************************************/
{ 
  int ret;
  u_int n, ii;
  u_int chip_add=0, ch_add=0;
  
  chip_add = (address >> 4) & 0xff;
  ch_add = address & 0xf;

  for(n=0; n<nwords; n++)
  {
    ii = 3*n;
    im_seq[ii] 	 = (FEC_WR << 20) | (chip_add << 9) | A_PMADD;
    im_seq[ii+1] =  n + offset;
    im_seq[ii+2] = (FEC_RD << 20) | (chip_add << 9) | (ch_add<<5) | A_PMDTA;
  }
  im_seq[3*nwords] = (IS_CMD << 20) | ENDSEQ;
  return (0);  
}         

     
/************************************************************************************************/
int FEC_ALTRO_Read(u_int fec_add, u_int ch_add, u_int reg_add, u_int *reg_data)   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int imword;
  u_int address;
  u_int imem_data[2];
  u_long resm_data[2];

  RCUISOPEN;

  DEBUG_TEXT(P_ID_FEC, 20 ,("ALTRO_FEC_Read()>\nFec_Add = 0x%x\nCh_Add = 0x%x\nReg_Add = 0x%x", \
                             fec_add, ch_add, reg_add));

  /* Prepare and write Instruction Sequence into the IMEM */
  address = FEC_Address_Build(ALTRO, NBCAST, fec_add, ch_add, reg_add);  

  ret = FEC_Sequence_Build(FEC_RD, address, 0, imem_data);  
  DEBUG_TEXT(P_ID_FEC, 50, ("FEC_ALTRO_Read()>\nimem_data[0] = 0x%x\nimem_data[1] = 0x%x\n", \
                                imem_data[0], imem_data[1]));  

  ret = RCU_IMEM_Write(0, FEC_RD_LEN, imem_data);  
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Read()>\tRCU_IMEM_Write fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  } 

  /* Execute Instruction Sequence */
  ret = RCU_Exec_EXECSEQ();
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Read()>\tRCU_Exec_Sequence() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }   
  
  /* Checking if the result is ready */
/*  ts_delay(1000);  *//* temporary! discuss with Attiq and Changzhou */
  usleep(1000); 

  /* Reading the result */
  ret = RCU_RESM_Read(0, 3, resm_data);
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Read()>\tRCU_RESM_Read() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }     

  /* Checking the result */  
  if( ((resm_data[0] >> 20) & 0xf) != FEC_RD ) 
  {
     DEBUG_TEXT(P_ID_FEC, 5, ("FEC_ALTRO_Read()>\nFEC_RD result not found in RESM"));
     return(FEC_ALTRO_Read_FAIL);
  }
  else if((resm_data[0] >> 19) & 0x1)
  { 
     DEBUG_TEXT(P_ID_FEC, 5, ("FEC_ALTRO_Read()>\nFEC_RD failed!\nError Code = 0x%x", resm_data[1]));
     return(FEC_ALTRO_Read_FAIL);
  }
  else
     *reg_data = resm_data[1] & 0xfffff;
  
  DEBUG_TEXT(P_ID_FEC, 30, ("FEC_ALTRO_Read()> SUCCESSFUL!")); 
  return(FEC_LIB_SUCCESS);
}


/************************************************************************************************/
int
FEC_ALTRO_Write(u_int fec_add, u_int ch_add, u_int reg_add, u_int reg_data)   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int imword;
  u_int address;
  u_int imem_data[2];
  u_long resm_data[2];

  RCUISOPEN;

  DEBUG_TEXT(P_ID_FEC, 20 ,("ALTRO_FEC_Write()>\nFec_Add = 0x%x\nCh_Add = 0x%x\nReg_Add = 0x%x\nReg_Data = 0x%x", \
                             fec_add, ch_add, reg_add, reg_data));

  /* Prepare and write Instruction Sequence into the IMEM */
  address = FEC_Address_Build(ALTRO, NBCAST, fec_add, ch_add, reg_add);  

  ret = FEC_Sequence_Build(FEC_WR, address, reg_data, imem_data);  
  DEBUG_TEXT(P_ID_FEC, 50, ("FEC_ALTRO_Write()>\nimem_data[0] = 0x%x\nimem_data[1] = 0x%x\nimem_data[2] = 0x%x\n", \
                                imem_data[0], imem_data[1], imem_data[2]));

  ret = RCU_IMEM_Write(0, FEC_WR_LEN, imem_data);  
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Write()>\tRCU_IMEM_Write fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  } 

  /* Execute Instruction Sequence */
  /*printf("Ask Attiq ???\n");fflush(stdout);need to prevent seg fault for -O2*/
  ret = RCU_Exec_EXECSEQ();
  /*printf("Ask Attiq !!!\n");fflush(stdout);need to prevent seg fault for -O2*/
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Write()>\tRCU_Exec_Sequence() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }   

  /* Waiting for the sequence to be executed */
/*  ts_delay(1000); */ /* temporary! discuss with Attiq and Changzhou */
  usleep(1000); 
  
  DEBUG_TEXT(P_ID_FEC, 30, ("FEC_ALTRO_Write()> SUCCESSFUL!")); 
  return(FEC_LIB_SUCCESS);
}

/************************************************************************************************/
int FEC_BC_Read(u_int fec_add, u_int reg_add, u_int *reg_data)   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int imword;
  u_int address;
  u_int imem_data[2];
  u_long resm_data[2];

  RCUISOPEN;

  DEBUG_TEXT(P_ID_FEC, 20 ,("FEC_BC_Read()>\nFec_Add = 0x%x\nReg_Add = 0x%x", \
                             fec_add, reg_add));

  /* Prepare and write Instruction Sequence into the IMEM */
  address = FEC_Address_Build(BC, NBCAST, fec_add, 0, reg_add);  

  ret = FEC_Sequence_Build(FEC_RD, address, 0, imem_data);  
  DEBUG_TEXT(P_ID_FEC, 50, ("FEC_BC_Read()>\nimem_data[0] = 0x%x\nimem_data[1] = 0x%x\n", \
                                imem_data[0], imem_data[1]));  

  ret = RCU_IMEM_Write(0, FEC_RD_LEN, imem_data);  
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_BC_Read()>\tRCU_IMEM_Write fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  } 

  /* Execute Instruction Sequence */
  ret = RCU_Exec_EXECSEQ();
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_BC_Read()>\tRCU_Exec_Sequence() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }   
  
  /* Checking if the result is ready */
/*  ts_delay(1000);  *//* temporary! discuss with Attiq and Changzhou */
  usleep(1000); 

  /* Reading the result */
  ret = RCU_RESM_Read(0, 3, resm_data);
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_ALTRO_Read()>\tRCU_RESM_Read() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }     

  /* Checking the result */  
  if( ((resm_data[0] >> 20) & 0xf) != FEC_RD ) 
  {
     DEBUG_TEXT(P_ID_FEC, 5, ("FEC_BC_Read()>\nFEC_RD result not found in RESM"));
     return(FEC_ALTRO_Read_FAIL);
  }
  else if((resm_data[0] >> 19) & 0x1)
  { 
     DEBUG_TEXT(P_ID_FEC, 5, ("FEC_BC_Read()>\nFEC_RD failed!\nError Code = 0x%x", resm_data[1]));
     return(FEC_ALTRO_Read_FAIL);
  }
  else
     *reg_data = resm_data[1] & 0xfffff;
  
  DEBUG_TEXT(P_ID_FEC, 30, ("FEC_ALTRO_Read()> SUCCESSFUL!")); 
  return(FEC_LIB_SUCCESS);
}


/************************************************************************************************/
int FEC_PMEM_Read(u_int fec_add, u_int ch_add, u_int offset, u_int nwords, u_int *pmem_data)   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int n;
  u_int address;
  u_int imem_data[S_IMEM]={0};
  u_long resm_data[S_RESM]={0};

  RCUISOPEN;

  DEBUG_TEXT(P_ID_FEC, 20 ,("FEC_PMEM_Read()>\nFec_Add = 0x%x\nCh_Add = %d", \
                             fec_add, ch_add));

  /* Prepare and write Instruction Sequence into the IMEM */

  address = (fec_add << 7) | ch_add;

  ret = FEC_RPMEM_Sequence_Build(address, offset, nwords, imem_data);  
  DEBUG_TEXT(P_ID_FEC, 50, ("FEC_PMEM_Read()>\nimem_data[0] = 0x%x\nimem_data[1] = 0x%x\n....", \
                                imem_data[0], imem_data[1]));  

  ret = RCU_IMEM_Write(offset, 3*nwords+1, imem_data);  
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_PMEM_Read()>\tRCU_IMEM_Write fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  } 

  /* Execute Instruction Sequence */
  ret = RCU_Exec_EXECSEQ();
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_PMEM_Read()>\tRCU_Exec_Sequence() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }   
  
  /* Checking if the result is ready */
  ts_delay(10000);  /* temporary! discuss with Attiq and Changzhou */

  /* Reading the result */
  ret = RCU_RESM_Read(0, 4*nwords, resm_data);
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_PMEM_Read()>\tRCU_RESM_Read() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }     

  /* Checking the result */  
  for(n=0; n<nwords; n++)
  {
    if( ((resm_data[4*n] >> 21) & 0xf) != FEC_WR ) 
    {
       DEBUG_TEXT(P_ID_FEC, 5, ("FEC_PMEM_Read()>\nFEC_RD result of location %d not found in RESM", n));
       return(FEC_ALTRO_Read_FAIL);
    }
    else if( ((resm_data[4*n+2] >> 21) & 0xf) != FEC_RD ) 
    {
       DEBUG_TEXT(P_ID_FEC, 5, ("FEC_PMEM_Read()>\nFEC_RD result of location %d not found in RESM", n));
       return(FEC_ALTRO_Read_FAIL);
    }     
    else if((resm_data[4*n+2] >> 20) & 0x1)
    { 
       DEBUG_TEXT(P_ID_FEC, 5, ("FEC_PMEM_Read()>\nFEC_RD failed!\nError Code = 0x%x", resm_data[2*n]));
       return(FEC_ALTRO_Read_FAIL);
    }
    else
       pmem_data[n] = resm_data[4*n+3] & 0x3ff;
  }

  DEBUG_TEXT(P_ID_FEC, 30, ("FEC_ALTRO_Read()> SUCCESSFUL!")); 
  return(FEC_LIB_SUCCESS);
}

/************************************************************************************************/
int FEC_PMEM_Write(u_int fec_add, u_int ch_add, u_int offset, u_int nwords, u_int *pmem_data)   
/************************************************************************************************/
{ 
  int ret = 0;
  u_int address;
  u_int imem_data[S_IMEM]={0};

  RCUISOPEN;

  DEBUG_TEXT(P_ID_FEC, 20 ,("FEC_PMEM_Write()>\nFec_Add = 0x%x\nCh_Add = %d", \
                             fec_add, ch_add));

  /* Prepare and write Instruction Sequence into the IMEM */

  address = (fec_add << 7) | ch_add;

  ret = FEC_WPMEM_Sequence_Build(address, offset, nwords, pmem_data, imem_data);  
  DEBUG_TEXT(P_ID_FEC, 50, ("FEC_PMEM_Write()>\nimem_data[0] = 0x%x\nimem_data[1] = 0x%x\n....", \
                                imem_data[0], imem_data[1]));  

  ret = RCU_IMEM_Write(offset, 4*nwords+1, imem_data);  
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_PMEM_Write()>\tRCU_IMEM_Write fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  } 

  /* Execute Instruction Sequence */
  ret = RCU_Exec_EXECSEQ();
  if (ret) 
  {    
    DEBUG_TEXT(P_ID_FEC, 5 , ("FEC_PMEM_Write()>\tRCU_Exec_Sequence() fail: returned value = %d", ret)); 
    return(RCU_LIB_FAIL);
  }   

  /* Waiting for the completion of the execution of the sequence */
  ts_delay(10000);  /* temporary! discuss with Attiq and Changzhou */    

  DEBUG_TEXT(P_ID_FEC, 30, ("FEC_ALTRO_Read()> SUCCESSFUL!")); 
  return(FEC_LIB_SUCCESS);

}

#endif
