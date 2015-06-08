
#ifdef Linux

/* altro.c - library for the altro readout */


/*********************************************/
/* SERGEY B.: ADJUSTED FOR CODA USE AUG 2009 */
/****************************************************************/
/*                                                              */
/*  file: fecscope.c                                            */
/*                                                              */
/* This program allows to access the resources of the FEC card  */
/* in a user friendly way and includes some test routines       */
/*                                                              */
/*  Author: Luciano Musa, CERN-PH                               */
/*                                                              */
/*  22. May. 08  LM   created                                   */
/*  27. May. 08  last modified                                  */
/*                                                              */
/****************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <signal.h>

/*
#include "get_input.h"
*/
#include "rcc_time_stamp.h"
#include "fec2rorc_lib.h"
#include "rcu.h"
#include "fec.h"

#define MAX_MEM 6144

/*************/
/* Globals */
/*************/
extern u_int packageId;
extern u_int traceLevel;
int rcu_id;
rorc_node_t rorc_node;
chan_key    *rorc; 

u_int reg_add[17]  ={ A_K1, A_K2, A_K3,A_L1,A_L2,A_L3,A_VFPED,A_PMDTA,A_ADEVL, \
		      A_ZSTHR,A_BCTHR,A_TRCFG,A_DPCFG,A_BFNPT,A_PMADD,A_ERSTR,A_TRCNT};		      

u_int reg_rmask[17]={ M_K1,M_K2,M_K3,M_L1,M_L2,M_L3,M_VFPED,M_PMDTA,M_ADEVL, \
		      M_ZSTHR,M_BCTHR,M_TRCFG,M_DPCFG,M_BFNPT,M_PMADD,M_ERSTR,M_TRCNT};


/*******************/
int
altroInit(int fun, int opt[20])
/*******************/
{  
  static choice, ncycles=1, tofile=0, dsource=0, fsize=0, sib_nempty;
  int ret = 0;
  static u_int mem_add=0, offset=0, max_nr_words, rcode=0, rdata=0,data_mask;
  static u_int ccode=0;
  static u_int fdata[MAX_MEM]={0};
  static char fname[200] = {0};
  u_long rsize=10, osize, word=0x55555555; 
  u_long memdata[MAX_MEM]={0}, memout[MAX_MEM]={0}; 
  u_int loop, index, cycle, tcount, ii;
  tstamp ts1, ts2;
  float delta_t;
  FILE  *file;
  
  /* new added */
  static u_long data_in, data_out=0;
  u_int reg_num = 14;

  /* new new added */
  u_int fec_add, chip_add, global_reg, channel_add, ch_add, bc_add;
  u_long reg_data = 0;   
  u_int reg_id, nwords,max_nwords;
  u_int ped_add;
  u_int act_fec, number_fec, fec_status;
  u_int all_fecs, fec_test_list, rdo_part;
  u_int nr_samples, nr_pretrig, readout_mode, trigger_source, meb;
  u_int zs_enable, zs_thr, zs_minseq, zs_pres, zs_posts;
  u_int bs_enable, bs_mode, bs_fpd;
  u_long data_TRCFG, data_BFNPT,data_DPCFG,data_ZSTHR,data_VFPED;


    
/************************************************/
/*		    RCU OPEN			*/
/************************************************/    

  if (fun == 1) 
  {
    printf("Opens RCU\n");
    rcu_id = opt[0];
    printf("\nWill work with RCU number %d\n",rcu_id);


    switch(rcu_id)
    {
    case 0:
	   rorc_node.revision = RORC_REVISION0;
	   rorc_node.serial   = opt[1];/*RORC_SERIAL0;*/
	   rorc_node.channel  = RORC_CHANNEL0;
	   break;
	case 1:   
	   rorc_node.revision = RORC_REVISION1;
	   rorc_node.serial   = opt[1];/*RORC_SERIAL1;*/
	   rorc_node.channel  = RORC_CHANNEL1;
	   break;
	case 2:   
	   rorc_node.revision 	= RORC_REVISION2;
	   rorc_node.serial	= opt[1];/*RORC_SERIAL2;*/
	   rorc_node.channel   = RORC_CHANNEL2;
	   break;
	case 3:   
	   rorc_node.revision 	= RORC_REVISION3;
	   rorc_node.serial	= opt[1];/*RORC_SERIAL3;*/
	   rorc_node.channel   = RORC_CHANNEL3;
	   break;
	case 4:   
	   rorc_node.revision 	= RORC_REVISION4;
	   rorc_node.serial	= opt[1];/*RORC_SERIAL4;*/
	   rorc_node.channel   = RORC_CHANNEL4;
	   break;
	case 5:   
	   rorc_node.revision 	= RORC_REVISION5;
	   rorc_node.serial	= opt[1];/*RORC_SERIAL5;*/
	   rorc_node.channel   = RORC_CHANNEL5;
	   break;
	default:
	   printf("\n ERROR: your selection is out of range\n");
	   return (-1);
           break;
    }     
     
    ret = RCU_Open(&rorc_node); 
    if(ret)
    {
      DEBUG_TEXT(P_ID_RCU, 10,("RCU_Open() fail: %d\n", ret));
      return ret; 
    }
    DEBUG_TEXT(P_ID_RCU, 80 ,("RORC parameters:\nMinor number:\t%d\nChannel number:\t%d\nStatus:\t%d\n", \
                                rorc->minor, rorc->channel, rorc->status ));     
  } 

/************************************************************************/
/*		   		 RCU CLOSE				*/
/************************************************************************/    
  else if (fun == 2)
  {
    printf("Close RCU\n");
    ret = RCU_Close();
    if(ret)
    { 
      DEBUG_TEXT(P_ID_RCU, 10,("RCU_Close() fail: %d\n", ret));
      return ret; 
    }     
  }       

/************************************************************************/
/*  			FECs ON & RESET		  			*/
/************************************************************************/
  else if (fun == 3)
  {
    printf("FECs ON and RESET\n");
    
    number_fec = opt[0];
    printf("\nNr of FECs ON is %d\n",number_fec);
    act_fec = 0;
    for(loop = 0; loop < number_fec; loop++)
      {
	fec_add = opt[1+loop];
	printf("Address of FEC nr. %d is %d\n",loop,fec_add);
       	act_fec = (act_fec) | (1 << fec_add);  
      }
    
    /* Switch on FECs */
    ret = RCU_ACTFECLIST_Write(act_fec);
    if(ret)
      { 
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_ACTFECLIST_Write() fail: %d\n", ret));
        printf("altro: FECSCOPE() RCU_ACTFECLIST_Write() fail: %d\n", ret);
        return ret; 
      }     

    /* Reading back the ACTFEC Register */
    ret = RCU_ACTFECLIST_Read(&data_out);
    if(ret)
      { 
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_ACTFECLIST_Read() fail: %d\n", ret));
        printf("altro: FECSCOPE() RCU_ACTFECLIST_Read() fail: %d\n", ret);
        return ret; 
      }           
    if (data_out != act_fec)
      {
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t Failed to turn-on all FECS"));
        printf("altro: FECSCOPE() Failed to turn-on all FECS");
        return ret; 
      }           
    
    act_fec = data_out;
    printf("\nThe FECs have the following power status:");
    printf("\nBRANCHA:\t");
    for(loop = 0; loop < 16; loop++)
      {
        fec_status = (data_out >> loop) & 0x1;
        printf("%d ", fec_status);
      }
    printf("\nBRANCHB:\t");
    for(loop = 0; loop < 16; loop++)
      {
        fec_status = (data_out >> (loop+16)) & 0x1;
        printf("%d ", fec_status);
      }
    printf("\n\n", fec_status);
    
    /* delay 500ms */
    ts_delay(500000);

    /* Reset FECs */
    ret = RCU_Exec_FECRESET();
    if(ret)
      {
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_Exec_FECREST() fail: %d", ret));
        return ret; 
      }           
    
    /* Reset FECERRORA and FECERRORB registers */
    /* delay 1ms */
    ts_delay(1000);
    ret = RCU_Exec_CLRREGS();
    if(ret)
      {
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_Exec_CLRREGS() fail: %d", ret));
        return ret; 
      }           
    
    /* Check FECERRORA and FECERROB registers */
    ret = RCU_FECERRA_Read(&data_out);
    if(ret)
      {
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_FECERRA_Read() fail: %d", ret));
        return ret; 
      }           
    if(data_out &0x1)
      printf("\nFECSCOPE()>\tWARNING: ALTRO ERROR asserted on Branch A\n");         
    
    ret = RCU_FECERRB_Read(&data_out);
    if(ret)
      {
        DEBUG_TEXT(P_ID_FEC, 5,("FECSCOPE()>\t RCU_FECERRB_Read() fail: %d", ret));
        return ret; 
      }           
    if(data_out &0x1)
      printf("\nFECSCOPE()>\tWARNING: ALTRO ERROR asserted on Branch B\n");         
    
  }


/************************************************************************/
/*  			ACCESS TO ALTRO REGISTERS  			*/
/************************************************************************/
    else if (fun == 5)
    {   
	  /*
      printf("\nEnter the FEC address (5 bits)");
      fec_add = gethexd(fec_add);
	  */
      fec_add = opt[0];

	  /*
      printf("Enter the address of the ALTRO chip to read (3 bits)");
      chip_add = gethexd(chip_add);
	  */
      chip_add = opt[1];

	  /*
      printf("Do you want to access a Global Register [0=no/1=yes]");
      global_reg = gethexd(global_reg);
	  */
      global_reg = opt[2];
      
      if(!global_reg)
      {
		/*
        printf("Enter the address of the ALTRO channel to read (4 bits)");
        channel_add = gethexd(channel_add);
		*/
        channel_add = opt[3];
      
      ch_add = (chip_add << 4) | channel_add;
      
      
      choice = 1;
      while(choice != 0)
      {
		/*
    	printf("\n");
    	printf("Select an option:\n");
	printf("\n  ALTRO Channel Individual Registers");
	printf("\n  ------------------------------------------\n");
    	printf("   1 Read K1        	    2 Write K1\n");
    	printf("   3 Read K2                4 Write K2\n");
    	printf("   5 Read K3                6 Write K3\n");
    	printf("   7 Read L1                8 Write L1\n");
    	printf("   9 Read L2        	   10 Write L2\n");
    	printf("  11 Read L3               12 Write L3\n");
    	printf("  13 Read VFPED            14 Write VFPED\n");
    	printf("  15 Read PMDTA            16 Write PMDTA\n");
    	printf("  17 Read ADEVL\n");
    	printf("  ============================================\n");
    	printf("   0 Quit\n");
    	printf("Your choice ");
    	choice = getdecd(choice);
		*/

        if ((choice < 0) || (choice > 17)){
      	  printf ("\n ERROR: your selection is out of range\n");  
          return(-1);
        }	

        switch(choice)
	{ case 1: case 2:   reg_id = 0; break;
	  case 3: case 4:   reg_id = 1; break;
	  case 5: case 6:   reg_id = 2; break;
	  case 7: case 8:   reg_id = 3; break;
	  case 9: case 10:  reg_id = 4; break;
	  case 11: case 12: reg_id = 5;	break;
	  case 13: case 14: reg_id = 6;	break;
	  case 15: case 16: reg_id = 7;	break;
	  case 17: case 18: reg_id = 8;	break;
	  case 19: reg_id = 9; break; }

        /* Writing */
        if((choice ==  2) || (choice ==  4) || (choice ==  6) || (choice ==  8) || \
	   (choice == 10) || (choice == 12) || (choice == 14) || (choice == 16))
	{	
	  /*
           printf("Enter data that you want to write ");
           reg_data = gethexd(reg_data);
	  */
	   ret = FEC_ALTRO_Write(fec_add, ch_add, reg_add[reg_id], reg_data);
	   if(ret) {
	     DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_ALTRO_Write() fail: return value = %d", ret));
	     return ret;	   
	   }
	}
        /* Reading */
	else
	{      	
	   ret = FEC_ALTRO_Read(fec_add, ch_add, reg_add[reg_id], &reg_data);
	   if(ret) {
	     DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_ALTRO_Read() fail: return value = %d", ret));
	     return ret;
	   }

	   printf("\nFEC_ADD = 0x%x\tCHIP_ADD = 0x%x\tCHANNEL_ADD = 0x%x", fec_add, chip_add, channel_add);
	   printf("\nREG_ADD = 0x%x\tDATA = 0x%x\n", reg_add[reg_id], reg_data );
        }
      } /* end ofwhile() */   
      } 
    
      else
      {
      choice = 1;
      while(choice != 0)
      {
		/*
    	printf("\n");
    	printf("Select an option:\n");
	printf("\n  ALTRO Global Registers");
	printf("\n  ------------------------------------------\n");
    	printf("   1 Read ZSTHR        	    2 Write ZSTHR\n");
    	printf("   3 Read BCTHR             4 Write BCTHR\n");
    	printf("   5 Read TRCFG             6 Write TRCFG\n");
    	printf("   7 Read DPCFG             8 Write DPCFG\n");
    	printf("   9 Read BFNPT            10 Write BFNPT\n");
    	printf("  11 Read PMADD        	   12 Write PMADD\n");
    	printf("  13 Read ERSTR        	  		 \n");
    	printf("  14 Read TRCNT        	   		 \n");
    	printf("  ============================================\n");
    	printf("   0 Quit\n");
    	printf("Your choice ");
    	choice = getdecd(choice);
		*/

        if ((choice < 0) || (choice > 14)){
      	  printf ("\n ERROR: your selection is out of range\n");  
          return(-1);
        }	

        switch(choice)
	{ case 1: case 2:   reg_id =  9; break;
	  case 3: case 4:   reg_id = 10; break;
	  case 5: case 6:   reg_id = 11; break;
	  case 7: case 8:   reg_id = 12; break;
	  case 9: case 10:  reg_id = 13; break;
	  case 11: case 12: reg_id = 14; break;
	  case 13: 	    reg_id = 15; break;
	  case 14: 	    reg_id = 16; break;}
        
	ch_add = chip_add << 4;
	
        /* Writing */
        if((choice ==  2) || (choice ==  4) || (choice ==  6) || (choice ==  8) || \
	   (choice == 10) || (choice == 12))
	{
           /*
           printf("Enter data that you want to write ");
           reg_data = gethexd(reg_data);
		   */

	   ret = FEC_ALTRO_Write(fec_add, ch_add, reg_add[reg_id], reg_data);
	   if(ret) {
	     DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_ALTRO_Write() fail: return value = %d", ret));
	     return ret;	   
	   }
	}
        /* Reading */
	else
	{      	
	   ret = FEC_ALTRO_Read(fec_add, ch_add, reg_add[reg_id], &reg_data);
	   if(ret) {
	     DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_ALTRO_Read() fail: return value = %d", ret));
	     return ret;
	   }

	   printf("\nFEC_ADD = 0x%x\tCHIP_ADD = 0x%x\tCHANNEL_ADD = 0x%x", fec_add, chip_add, channel_add);
	   printf("\nREG_ADD = 0x%x\tDATA = 0x%x\n", reg_add[reg_id], reg_data );
        }
      } /* end ofwhile() */   
      } /* end else */
    
  } /* end of main if() */
  

/************************************************************************/
/*  				Read ALTRO PMEM  			*/
/************************************************************************/
    else if (fun == 7)
    {   
	  /*
      printf("\nWrite the Channel Address of the Pedestal Memory you want to read");

      printf("\nEnter the FEC address (5 bits) ");
      fec_add = gethexd(fec_add);

      printf("Enter the address of the ALTRO chip (3 bits) ");
      chip_add = gethexd(chip_add);

      printf("Enter the address of the ALTRO channel to read (4 bits) ");
      channel_add = gethexd(channel_add);

      printf("Enter the Pedestal Address (10 bits)");
      ped_add = getdecd(ped_add);

      printf("Enter the numer of words you want to read (max %d)", 1024 - ped_add);
      nwords = getdecd(nwords);
      max_nwords = 1024 - ped_add;
      */

      if(nwords > max_nwords)
      {
      	printf("\nNr. of words too large! %d words will be used\n", max_nwords);
	nwords = max_nwords;
      }
	  /*
      printf("\nDo you want to write the pedestal pattern to file (1=yes 0=no): ");
      tofile = getdecd(tofile);  
	  */
      if (tofile)
      {
        /*
        printf("Enter the path and name of the file to write: ");
        getstrd(fname, fname);
		*/
      }

      /* opening the file to write the results */
      if (tofile)
      {
        file = fopen(fname, "w");
	if (file == 0) 
	{
	  printf("Can't open output file\n");
	  return(-1);
        }
      }    
 
      ch_add = ((chip_add << 4) | channel_add) & 0x7f;
      ret = FEC_PMEM_Read(fec_add, ch_add, offset, nwords, memdata);
      if(ret) {
	DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_PMEM_Read() fail: return value = %d", ret));
	return ret;
      }
      
      if(tofile)
      { 
        fprintf(file,"Content of the Pedestal Memory of: FEC 0x%x\tChannel %d\n\n", fec_add, ch_add); 
	for(loop=0; loop<nwords; loop++)
	  fprintf(file,"Add = %d\t\tData = %d\n", loop+ped_add, memdata[loop]);
      } 
      else 
      {
        printf("Content of the Pedestal Memory of: FEC 0x%x\tChannel %d\n\n", fec_add, ch_add); 
	for(loop=0; loop<nwords; loop++)
	  printf("Add = %d\t\tData = %d\n", loop+ped_add, memdata[loop] & 0x3ff);
      }	
    }  


/************************************************************************/
/*  				WRITE ALTRO PMEM  			*/
/************************************************************************/
    else if (fun == 8)
    {   
	  /*
      printf("\nWrite the Channel Address of the Pedestal Memory you want to write");

      printf("\nEnter the FEC address (5 bits) ");
      fec_add = gethexd(fec_add);

      printf("Enter the address of the ALTRO chip (3 bits) ");
      chip_add = gethexd(chip_add);

      printf("Enter the address of the ALTRO channel to write (4 bits) ");
      channel_add = gethexd(channel_add);

      printf("Enter the Pedestal Address (10 bits)");
      ped_add = getdecd(ped_add);

      printf("Enter the numer of words you want to write (max %d)", 1024 - ped_add);
      nwords = getdecd(nwords);
	  */
      max_nwords = 1024 - ped_add;
      
      if(nwords > max_nwords)
      {
      	printf("\nNr. of words too large! %d words will be used\n", max_nwords);
	nwords = max_nwords;
      }
	  /*
      printf("Select the data source");
      printf("\n 0 = From last RCU_File_Read");
      printf("\n 1 = Incremental pattern");
      printf("\n 2 = Constant pattern\n");
      dsource = getdecd(dsource);
	  */
      if (dsource == 0){
        for (loop = 0; loop < MAX_MEM; loop++)
	  memdata[loop] = fdata[loop];
      }
      else if (dsource == 1){
        for(loop=0; loop < MAX_MEM; loop++)
          memdata[loop] = loop;     
      }
      else{
		/*
        printf("Enter data word 0x");
        word = gethexd(word);
		*/
	data_in = word;
        for (loop = 0; loop < MAX_MEM; loop++)
	  memdata[loop] = word;
      }

    
      ch_add = ((chip_add << 4) | channel_add) & 0x7f;
      
      ret = FEC_PMEM_Write(fec_add, ch_add, offset, nwords, memdata);
      if(ret) {
	DEBUG_TEXT(P_ID_FEC, 5, ("FECSCOPE> FEC_PMEM_Write() fail: return value = %d", ret));
	return ret;
      }  
    }  



/************************************************************************/
/*  				Configuration	  			*/
/************************************************************************/
  else if (fun == 10)
  {
	/*	int ij;
	  for(ij=0; ij<11; ij++) printf("%d ",opt[ij]); */
    printf("\nConfiguration will be applied to all channels\n");  
    
    nr_samples = opt[0];
    printf("Nr. of Time Samples/ Channel is %d\n",nr_samples); 
    
    nr_pretrig = opt[1];
    printf("Nr. of Pre Trigger Samples is %d\n",nr_pretrig); 
    
    zs_enable = opt[2];
    if(zs_enable) printf("Enable Zero Suppression\n"); 
    else printf("Disable Zero Suppression\n");
    if (zs_enable)
      {
        zs_thr = opt[3];
        printf("ZS Threshold is %d\n",zs_thr); 
        zs_minseq = opt[4];
        printf("ZS Minimum Number of Samples in Cluster is %d\n",zs_minseq); 
        zs_pres = opt[5];
        printf("ZS Nr. of Pre-Samples is %d\n",zs_pres); 
        zs_posts = opt[6];
        printf("ZS Nr. of Post-Samples is %d\n",zs_posts); 
      }  	 

    bs_enable = opt[7];
    if(bs_enable) printf("Enable Baseline Subtraction\n"); 
    else printf("Disable Baseline Subtraction\n");
    
    if (bs_enable)
      {
        bs_mode = opt[8];
        printf("Baseline Subtraction Mode is %d\n",bs_mode); 
        bs_fpd = opt[9];
        printf("Baseline Subtraction Fixed Pedestal is %d\n",bs_fpd); 
      }  	 
    else 
      {
	printf("Disable Baseline Subtraction\n");
	bs_mode=0;
	bs_fpd=0;
      }
    
    meb = 1;           /* 0 - 4 event fifo, 1 - 8 event fifo */
    readout_mode=0;    /* 0 - full readout, 1 - sparse readout */
    trigger_source = opt[10];
    if(trigger_source==1) printf("Trigger Source: Software Trigger\n");
    else if(trigger_source==4) printf("Trigger Source: TTC Trigger\n");
    else if(trigger_source==6) printf("Trigger Source: AUX\n");
    else
      {
        printf("Trigger Source setting ERROR \n");
        return(1);
      }
	

/**************** CONFIGURATION OF THE RCU **************/      

    /********************************************/
    /* Configure ALTROIF Register               */
    /********************************************/
    /* [9:0]   Nr. samples/ event		  */
    /* [13:10] clk_ratio	 		  */
    /* [15:14] cstb delay			  */
    /********************************************/
    reg_data = (0x3 <<14) | (0x1 << 10) | nr_samples;
    ret = RCU_ALTROIF_Write(reg_data);
    if(ret)
      {
		DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_ALTROIF_Write fail: %d", ret));
		return ret;
	  }
	else
	  {
		printf("ALTROIF register set to: 0x%x\n",reg_data);
      }

    /********************************************/
    /* Configure TRCFG Register               */
    /********************************************/
    /* [12:0] L2 latency (only for softw TRG)	  */
    /* [13] trigger mode (0 for TPC)            */
    /* [16:14] trigger source  (ttc,hdw,sfw)	  */
    /********************************************/
    //reg_data = (trigger_source <<14) | (0x0 << 10) | 0x1fff; // 200.0 microsec
    //      reg_data = (trigger_source <<14) | (0x0 << 13) | 0x0480; //  28.8 microsec
    //reg_data = (trigger_source <<14) | (0x0 << 13) | 0x03FF; //  25.0 microsec
    //      reg_data = (trigger_source <<14) | (0x0 << 13) | 0x0332; //  20.0 microsec
    reg_data = (trigger_source <<14) | (0x0 << 13) | 0x0260; //  15.0 microsec
    //      reg_data = (trigger_source <<14) | (0x0 << 13) | 0x0214; //  13.0 microsec
    ret = RCU_TRGCONF_Write(reg_data);
   if(ret)  	
      {
		DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_TRGCONF_Write fail: %d\n", ret));
		return ret;
	  }
	else
	  {
		printf("TRGCONF register set to: 0x%x \n",reg_data);
      }

    /********************************************/
    /* Configure RDOMOD Register               */
    /********************************************/  
    /* [0] meb mode (4/8) buffers		  */
    /* [1] exec_seq on SOD/EOD	          */
    /* [2] sparse rdo				  */
    /* [3] disable check rdyrx		  */
    /* [6] drop headers for ampty channels */
    /********************************************/
   reg_data = /*(0x1<<6) | */ (0x0<<3) | (readout_mode <<2) | (0x0 << 1) | meb;
    ret = RCU_RDOMOD_Write(reg_data);
    if(ret)
      {
	DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_RDOMOD_Write fail: %d", ret));
	return ret;
	  }
	else
	  {
		printf("RDOMOD  register set to: 0x%x \n",reg_data);
      }


    /********************************************/
    /* Configure L1 latency register            */
    /********************************************/
    reg_data = 0x20e0; 
    reg_data = 0x0900; 
    ret = RCU_L1_LATENCY_Write(reg_data);
    if(ret)
      {
	DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_L1_LATENCY_Write: %d", ret));
	return ret;
      }

    /********************************************/
    /* Configure L1 Message Latency register    */
    /********************************************/
    reg_data = 0xe04e20; 
    ret = RCU_L1_MSG_LATENCY_Write(reg_data);
    if(ret)
      {
	DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_L1_MSG_LATENCY_Write: %d", ret));
	return ret;
      }


    /********************************************/
    /*   Configure Readout List Memory	  */
    /********************************************/
    ret = RCU_ACTFECLIST_Read(&act_fec);
    if(ret)
      {
	DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_ACTFECLIST_Read: %d", ret));
	return ret;
      }       
      
    /* Configure Parallel Readout on RCU Branches */
    /* How it works:
       1)First it cycles trough branch A to see if there are FECs there.
       2)If there is at least one (say x), it cycles trough branch B to 
         see if there are FECs there.
       3A)If there is at least one which has not been already considered 
         (say y) it writes in the RCU Readout Memory performing parallel 
          readout between x and y
       3B)If there are no FECs on branch B, it writes the Readout Memory 
          only with channel of FEC x on branch A
       4)Finally, after the cycle in point 1), it cycles trough branch B 
         to see if there are FECs there which have not been condered yet. 
         If there are, it writes the Readout Memory considering them.
         Point 4) is used to consider those cases when on branch B there 
         are more FECs that on branch A
      */ 

    int count=16; 
    int branchB=0;
    offset = 0;
    for(fec_add=0; fec_add<16; fec_add++)
    {
      branchB=0; 
      if ((act_fec >> fec_add) & 0x1){ 
	for (count;count<32;count++){
	  if ((act_fec >> count) & 0x1){ 
	    branchB=1;
	    for(index=0; index<128; index++) {
	      memdata[index*2] = (fec_add << 7) | index;
	      memdata[index*2+1] = (count << 7 ) | index;
	    } 
	    ret = RCU_ROLM_Write(offset, 256, memdata); 
	    offset = offset + 256;
	    count++;
	    break; 
	  }
	} 
	if (branchB==0){
	  for(index=0; index<128; index++) {
	    memdata[index] = (fec_add << 7) | index; 
	  } 
	  ret = RCU_ROLM_Write(offset, 128, memdata); 
	  offset = offset + 128;
	}
      }
    } 
    for (count;count<32;count++){
      if ((act_fec >> count) & 0x1){ 
	for(index=0; index<128; index++) {
	  memdata[index] = (count << 7) | index;
	} 
	ret = RCU_ROLM_Write(offset, 128, memdata);
	offset = offset + 128;
      }
    }
    memdata[0]=0x0fff;
    ret = RCU_ROLM_Write(offset, 1, memdata); 
    
    

    /********************************************/
    /*   Configure FECs                	  */
    /********************************************/


    for(fec_add=0; fec_add<32; fec_add++)
    {
	  for(chip_add=0; chip_add<8; chip_add++)
	  {
	    if((act_fec >> fec_add) & 0x1)
	    {
		
		  /* set number of post-trigger samples */
		  data_TRCFG = (nr_samples-nr_pretrig);
		  /*printf("11\n");fflush(stdout);*/
		  ret = FEC_ALTRO_Write(fec_add, chip_add << 4, A_TRCFG, data_TRCFG);
		  /*printf("12\n");fflush(stdout);*/

		  /* set number of pretrigger samples and 8 events fifo*/
		  data_BFNPT = (meb << 4) | nr_pretrig ;
		  ret = FEC_ALTRO_Write(fec_add, chip_add << 4, A_BFNPT, data_BFNPT);
		
		  /* set baseline subtraction mode and zero-suppression  */
		  data_DPCFG = (zs_enable<<19) | (zs_pres<<17) | (zs_posts<<14) | (zs_minseq<<12) | bs_mode ;
		  ret = FEC_ALTRO_Write(fec_add, chip_add << 4, A_DPCFG, data_DPCFG);
		
		  /* set zero-suppression threshold */
		  data_ZSTHR = zs_thr;
		  ret = FEC_ALTRO_Write(fec_add, chip_add << 4, A_ZSTHR, data_ZSTHR);
		
		  /* set fixed pedestal for first baseline subtraction */
		  data_VFPED = bs_fpd ;
		  for (channel_add=0;channel_add<16;++channel_add)
		  {
		    ret = FEC_ALTRO_Write(fec_add,chip_add<<4|channel_add,A_VFPED,data_VFPED);
		  }

		}
	  }
	}

    /**************************************************************/
    /*  Read Back FEC Configuration and Save it in RCU Registers  */
    /**************************************************************/
    int err_TRCFG=0;
    int err_BFNPT=0;
    int err_DPCFG=0;
    int err_ZSTHR=0;
    int err_VFPED=0;
    for(fec_add=0; fec_add<32; fec_add++)
      {
		for(chip_add=0; chip_add<8; chip_add++)
		  {
			if((act_fec >> fec_add) & 0x1)
			  {
				u_long data;
				ret = FEC_ALTRO_Read(fec_add, chip_add << 4, A_TRCFG, &data);
				if (data != data_TRCFG)
				  {
					printf("Error in Configuration of TRCFG Register for FEC %d, ALTRO %d\n", fec_add, chip_add);
					printf("Written data:  %x, Read data : %x\n",data_TRCFG,data);
					err_TRCFG++;
					return err_TRCFG;
				  } 
				else
				  {
					
				  }
				
				ret = FEC_ALTRO_Read(fec_add, chip_add << 4, A_BFNPT, &data);
				if (data != data_BFNPT)
				  {
					printf("Error in Configuration of BFNPT Register for FEC %d, ALTRO %d\n", fec_add, chip_add);
					printf("Written data:  %x, Read data : %x\n",data_BFNPT,data);
					err_BFNPT++;
					return err_BFNPT;
				  } 
				
				ret = FEC_ALTRO_Read(fec_add, chip_add << 4, A_DPCFG, &data);
				if (data != data_DPCFG)
				  {
					printf("Error in Configuration of DPCFG Register for FEC %d, ALTRO %d\n", fec_add, chip_add);
					printf("Written data:  %x, Read data : %x\n",data_DPCFG,data);
					err_DPCFG++;
					return err_DPCFG;
				  }  
				
				ret = FEC_ALTRO_Read(fec_add, chip_add << 4, A_ZSTHR, &data);
				if (data != data_ZSTHR)
				  {
					printf("Error in Configuration of ZSTHR Register for FEC %d, ALTRO %d\n", fec_add, chip_add);
					printf("Written data:  %x, Read data : %x\n",data_ZSTHR,data);
					err_ZSTHR++;
					return err_ZSTHR;
				  } 
				
				for (channel_add=0;channel_add<16;++channel_add)
				  {
					ret = FEC_ALTRO_Read(fec_add,chip_add<<4|channel_add,A_VFPED,&data);
					/*					if (data != data_VFPED )
					  {
						printf("Error in Configuration of VFPED Register for FEC %d, ALTRO %d, CH %d\n", fec_add, chip_add, channel_add);
						printf("Written data:  %x, Read data : %x\n",data_VFPED,data);
						err_VFPED++;
						return err_VFPED; 
						}*/ 
				  } 
			  } 
		  }
      }

	  
    /********************************************/
    /*          Configure RDO_CONFIG1	  	  */
    /********************************************/
    ret = RCU_ALTROCFG1_Write(data_DPCFG);
    if(ret)
    {
	  DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_ALTROCFG1_Read: %d", ret));
	  return ret;
    }       
	else
	{
	  printf("RCU_ALTROCFG1 register set to: 0x%x \n", data_DPCFG);
	}

    /********************************************/
    /*          Configure RDO_CONFIG2	  	  */
    /********************************************/
    reg_data = (meb << 18) | (nr_pretrig << 14) | (nr_samples << 4) | (readout_mode << 3)  | trigger_source;
    ret = RCU_ALTROCFG2_Write(reg_data);
    if(ret)
    {
	  DEBUG_TEXT(P_ID_RCU, 5,("FECSCOPE()> RCU_ALTROCFG2_Read: %d", ret));
	  return ret;
    }       
 	else
	{
	  printf("RCU_ALTROCFG2 register set to: 0x%x \n",reg_data);
	}
 
    /* RCU and FEC Coniguration Completed*/

  }
     

  return(0);
}

#endif
