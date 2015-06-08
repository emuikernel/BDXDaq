/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* gtDma.h - DMA functions header and definitions
*
* DESCRIPTION:
*     This file gives the user a complete interface to the powerful DMA engines,
*     including functions for controling the priority mechanism.
*     To fully understand the capabilities of the DMA engines please spare some
*     time to go trough the spec.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtDmah 
#define __INCgtDmah

/* includes */
#include "gtCore.h"

 
/* defines */
#define DMA_FIRST_ENGINE    DMA_ENG_0
#define DMA_LAST_ENGINE     DMA_ENG_3
#define MIN_DMA_WINDOW_SIZE 0x10000

#define IS_INVALID_ENGINE(engine)                                              \
                    ((engine) < DMA_FIRST_ENGINE || DMA_LAST_ENGINE < (engine))

/* for pizza arbiter */
#define DMA_ROUND_ROBIN                             0xe4e4e4e4  

/*  source DTLs */
#define DMA_DTL_8BYTES			            NO_BIT
#define DMA_DTL_16BYTES			            BIT6
#define DMA_DTL_32BYTES			            (BIT6|BIT7)
#define DMA_DTL_64BYTES			            (BIT6|BIT7|BIT8)
#define DMA_DTL_128BYTES                            BIT8
                                               
/* Destenation DTL */                          
#define DMA_DEST_DTL_8BYTES		            BIT1
#define DMA_DEST_DTL_16BYTES		            BIT0
#define DMA_DEST_DTL_32BYTES		            (BIT0|BIT1)
#define DMA_DEST_DTL_64BYTES		            (BIT0|BIT1|BIT2)
#define DMA_DEST_DTL_128BYTES                       BIT2
                                           
/* defines command for the LOW control register*/

#define DMA_HOLD_SOURCE_ADDR		            BIT3
#define DMA_ACK_ASSERT_AFTER_TWO_TCLK               BIT4
#define DMA_HOLD_DEST_ADDR		            BIT5
#define DMA_NON_CHAIN_MODE  		            BIT9
#define DMA_CHAIN_MODE  		            NO_BIT
#define DMA_INTERRUPT_EVERY_NULL_POINTER            BIT10
#define DMA_BLOCK_TRANSFER_MODE		            BIT11
#define DMA_DEMAND_MODE			            NO_BIT
#define DMA_CHANNEL_ENABLE		            BIT12
#define DMA_FETCH_NEXT_RECORD		            BIT13
#define DMA_ACTIVITY_STATUS                         BIT14
#define DMA_REQ_DIRECTION_BY_SOURCE                 NO_BIT
#define DMA_REQ_DIRECTION_BY_DEST                   BIT15
#define DMA_REQ_IS_LEVEL_INPUT                      NO_BIT
#define DMA_REQ_IS_EDGE_TRIGGERD_INPUT              BIT16
#define DMA_CLOSE_DESCRIPTOR_ENABLE	            BIT17
#define DMA_ENABLE_EOT                              BIT18
#define DMA_FETCH_NEXT_DES_ON_EOT	            NO_BIT
#define DMA_CHANNEL_HALT_ON_EOT  	            BIT19
#define DMA_CHANNEL_ABORT		            BIT20
/* Note: When using the source/destenation address override 
         feature , window 1 must be allocated for PCI0 and window 2 
         must be allocated for PCI1 */ 
#define DMA_SOURCE_ADDR_IN_PCI0	                    BIT21
#define DMA_SOURCE_ADDR_IN_PCI1	                    BIT22
#define DMA_DEST_ADDR_IN_PCI0		            BIT23
#define DMA_DEST_ADDR_IN_PCI1		            BIT24
#define DMA_REC_ADDR_IN_PCI0		            BIT25
#define DMA_REC_ADDR_IN_PCI1		            BIT26
#define DMA_ACK_ASSERT_ONE_TCK_ON_TRAN              NO_BIT
#define DMA_ACK_ASSERT_ON_ACT_TRAN_ON_DEVICE_BUS    BIT27
#define DMA_REQ_FROM_DMA_REQ_PIN	            NO_BIT
#define DMA_REQ_FROM_TIMER_COUNTER	            BIT28
#define DMA_ACK_ASSERT_ON_ACCESS_TO_DEST            BIT29
#define DMA_ACK_ASSERT_ON_ACCESS_TO_SRC             BIT30
#define DMA_64K_DESCRIPTOR_MODE                     NO_BIT
#define DMA_16M_DESCRIPTOR_MODE                     BIT31
                                               
/************************************************/
/*    Definitions for memory space attributes   */
/************************************************/
#define DMA_NO_ATTR                                 NO_BIT
#ifdef  INCLUDE_SNOOP_SUPPORT
    /* If the target interface of the region is DRAM bank,these attributes 
       select whether its cache coherent or not (PPC only)*/
    #define DMA_WT_CACHE_COHERENT                   BIT12
    #define DMA_WB_CACHE_COHERENT                   BIT13    
    /* PCI-X No Snoop (NS) attributes */
    #define DMA_NS_ATTR_IS_NOT_ASSERTED             NO_BIT
    #define DMA_NS_ATTR_IS_ASSERTED                 BIT10
#endif /* INCLUDE_SNOOP_SUPPORT */
/* If the target interface is PCI0 or PCI1 these definitions 
       controls the data swap */
#define DMA_PCI_BYTE_SWAP                           NO_BIT
#define DMA_PCI_NO_SWAP                             BIT8
#define DMA_PCI_BYTE_AND_WORD_SWAP                  BIT9
#define DMA_PCI_WORD_SWAP                           (BIT9|BIT8)
/* These attributes select PCI I/O or memory space */
#define DMA_PCI_IO_SPACE                            NO_BIT
#define DMA_PCI_MEMORY_SPACE                        BIT11
/* These attributes control the PCI REQ64* mode  */
#define DMA_PCI_ALWAYS_ASSERT_REQ64                 NO_BIT
#define DMA_PCI_REQ64_ACCORDING_TO_DATA_SIZE        BIT12


/* typedefs */
typedef enum _dmaEngine{DMA_ENG_0, DMA_ENG_1, DMA_ENG_2, DMA_ENG_3} DMA_ENGINE;

/* 8 regions can be defind by base,size and attributes. Each region can
   be associated with a target interface (DRAM, GT_DEVICE, SRAM...)  */
typedef enum _dmaMemSpace{DMA_WINDOW0,DMA_WINDOW1,DMA_WINDOW2,DMA_WINDOW3,
                          DMA_WINDOW4,DMA_WINDOW5,DMA_WINDOW6,DMA_WINDOW7
                         } DMA_MEM_SPACE;

typedef enum _dmaMemSpaceTarget{DMA_DRAM_CS0 = BIT9|BIT10|BIT11,
                                DMA_DRAM_CS1 = BIT8|BIT10|BIT11,
                                DMA_DRAM_CS2 = BIT8|BIT9|BIT11|BIT12,
                                DMA_DRAM_CS3 = BIT8|BIT9|BIT10,
                                DMA_DEV_CS0 = BIT9|BIT10|BIT11|BIT12|BIT0,
                                DMA_DEV_CS1 = BIT8|BIT10|BIT11|BIT12|BIT0,
                                DMA_DEV_CS2 = BIT8|BIT9|BIT11|BIT12|BIT0,
                                DMA_DEV_CS3 = BIT8|BIT9|BIT10|BIT12|BIT0,
                                DMA_DEV_BOOT = BIT8|BIT9|BIT10|BIT11|BIT0,
                                DMA_SRAM = BIT1,
#ifdef INCLUDE_CPU_MAPPING
                                DMA_60X_BUS = BIT11|BIT1,
#endif /* INCLUDE_CPU_MAPPING */
                                DMA_PCI_0 = BIT0|BIT1,
                                DMA_PCI_1 = BIT2
                               } DMA_MEM_SPACE_TARGET;

typedef enum _dmaMemSpaceAccess{DMA_NO_ACCESS = NO_BIT,
                                DMA_READ_ONLY = BIT0,
                                DMA_FULL_ACCESS = BIT0|BIT1
                               } DMA_MEM_SPACE_ACCESS;

/* status codes */
typedef enum _dmaStatus{DMA_CHANNEL_IDLE, DMA_CHANNEL_BUSY,
                        DMA_NO_SUCH_CHANNEL, DMA_OK, DMA_GENERAL_ERROR
                       } DMA_STATUS;

/*******************************************************************************
*
*   Pizza arbiter slices Struct: Defined by user and used for updating the
*   DMA Arbiter with priority for each channel.
*   By default DMA_PIZZA is configured to ROUND_ROBIN.
*   Each field represent one of 16 slices, in order to configurate it to one
*   of the four engine simply write the engine number at the slice field.  
*   
*******************************************************************************/    

typedef struct _dmaPizza
{
    unsigned char slice0;
    unsigned char slice1; 
    unsigned char slice2;
    unsigned char slice3;
    unsigned char slice4;
    unsigned char slice5;
    unsigned char slice6;
    unsigned char slice7;
    unsigned char slice8;
    unsigned char slice9;
    unsigned char slice10;
    unsigned char slice11;
    unsigned char slice12;
    unsigned char slice13;
    unsigned char slice14;
    unsigned char slice15;
} DMA_PIZZA;          

/* dma descriptor structure */
typedef struct _dmaRecord
{
    unsigned int ByteCnt;
    unsigned int SrcAdd;
    unsigned int DestAdd;
    unsigned int NextRecPtr;
} DMA_RECORD;

/*
 * The source and destination for the IDMA engines can be DRAM, SRAM or PCI.
 */
#define DMA_SRC_DRAM    0x0
#define DMA_SRC_SRAM    0x1
#define DMA_SRC_PCI     0x2

#define DMA_TGT_DRAM    0x0
#define DMA_TGT_SRAM    0x1
#define DMA_TGT_PCI     0x2

/* Dma API list */

/* Configured a command to a selected DMA engine. */
bool gtDmaCommand(DMA_ENGINE engine,unsigned int command);

/* Performs DMA transfer according to: Engine, Byte Count,source & destination
   Addresses and a pointer to the first descriptor (chain mode) */
DMA_STATUS gtDmaTransfer(DMA_ENGINE engine, unsigned int sourceAddr,
                         unsigned int destAddr, unsigned int numOfBytes,
                         unsigned int command,  char srcIf, char tgtIf,
                         DMA_RECORD *pNextRecordPointer);

/* Checks if a given DMA engine is active (bit 14 in the dma control register)*/
DMA_STATUS gtDmaIsChannelActive(DMA_ENGINE engine);

/* Disable a give DMA engines */
bool gtDmaEngineDisable(DMA_ENGINE engine);

/* Update the pizza arbiter registers with the struct DMA_PIZZA content */
bool gtDmaUpdateArbiter(DMA_PIZZA *pPriorityStruct);

/* the Atlantis IDMA has its own address decoding map that is de-coupled from 
   the CPU interface address decoding windows. These windows can be configured 
   by this function. */
bool gtDmaSetMemorySpace(DMA_MEM_SPACE memSpace,
                         DMA_MEM_SPACE_TARGET memSpaceTarget,
                         unsigned int memSpaceAttr,
                         unsigned int baseAddress,
                         unsigned int size);

/* This function modifies the attributes of a given DMA memory window */
void gtDmaSetMemorySpaceAttr(DMA_MEM_SPACE memSpace,unsigned int memSpaceAttr);

/* This functions sets access attributes for each of the four possible engines 
   to one of the pre-defined DMA memory spaces. */
bool gtDmaSetEngineAccessProtect(DMA_ENGINE engine,DMA_MEM_SPACE memSpace,
                                 DMA_MEM_SPACE_ACCESS access);

#endif /* __INCgtDmah */
