#ifndef _RORC_DDL_
#define _RORC_DDL_

/********************************************************
*   rorc_ddl.h                                          *
*   last update:        22/09/2008                      *
*   written by: Peter Csato and Ervin Denes             *
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rorc_lib.h"
#include "ddl_def.h"

/* macros */

// #define ddlCheckStatus(dev) (*(*(dev)).mbef & PRORC_NE_IMB1)
#define ddlCheckStatus(dev) rorcCheckStatus(dev)
#define ddlStopDataTransfer(prorc, to, stw) rorcStopTrigger(prorc, to, stw)
#define ST_DEST(fw)  ((unsigned short)( (fw) & 0xf))               // 4 bits  0- 3
#define ST_CODE(fw)  ((unsigned short)(((fw) & 0xf0) >> 4))        // 4 bits  4- 7
#define ST_TRID(fw)  ((unsigned short)(((fw) & 0xf00) >> 8))       // 4 bits  8-11

#define FW_DAY(fw)   ((unsigned short)(((fw) & 0x1f000) >> 12))    // 5 bits 12-16
#define FW_MONTH(fw) ((unsigned short)(((fw) & 0x1e0000) >> 17))   // 4 bits 17-20
#define FW_YEAR(fw)  ((unsigned short)(((fw) & 0x1e00000) >> 21))  // 4 bits 21-24
#define FW_VERS(fw)  ((unsigned short)(((fw) & 0x7e000000) >> 25)) // 6 bits 25-30
#define FW_ERROR(fw) ((unsigned short)(((fw) & 0x80000000) >> 31)) // 1 bit  31 

#define HW_TYPE(fw)  ((unsigned short)(((fw) & 0x3c000000) >> 26)) // 4 bits 26-29
#define HW_MAN(fw)   ((unsigned short)(((fw) & 0x3c00000) >> 22))  // 4 bits 22-25
#define HW_SER(fw)   ((unsigned short)(((fw) & 0x3ff000) >> 12))   //10 bits 12-21

/* structures  and typedefs */

typedef union 
{
    struct 
    {
      unsigned char dest   :  4;
      unsigned char code   :  4;
      unsigned char trid   :  4;
      unsigned long param  : 19;
      unsigned char error  :  1;
    } part;
    unsigned long stw;
} stword_t;

struct readinfo{
        unsigned length;
        unsigned long *address;
        int bank;
        };

/* Global variables */

volatile int dbg;

/****************************************************************
*             own functions                                     *
****************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif

/***************************************************************/
int ddlFindDiuVersion(rorc_pci_dev_t *dev);

/***************************************************************/
unsigned long ddlLinkUp(rorc_pci_dev_t *prorc, int master, int print, int stop, long long int time);

/* link_up tries to reset and initialize (if master) the DDL link.
 * Parameter:  link     link#-1 (0 or 1) of the link
 *             master   if !=0 then tries to initialize the link,
 *                      otherwise only reset the link
 *             print    if !=0 prints link status
 *             stop     if !=0 returns when offline state is reached
 *             time     # of cycles to wait for command sending and replies
 * Returns:    DIU status word or -1 if no status word cen be read 
 *             The DIU status word reflects the status of the link after
 *             resetting it.
 *             Important bits of the status word are
 *             bit 31   must be 0, otherwise one of the error bit is set
 *             bit 29   must be 0, otherwise the receiver is not synchronised 
 *             bit 28   must be 0, otherwise there is no signal on the line   
 *             bit 14   must be 0, otherwise the other end is in failure state
 *             bits 13 and 12 must be 0 1 which means DIU is in offline state
 *                             or     0 0 which means the link is active.
 */                            

/***************************************************************/
unsigned long ddlResetSiu(rorc_pci_dev_t *prorc, int print, int trial, 
                                                         long long int time);

/* ddlResetSIU tries to reset the SIU.
 *             print    # of link status prints (-1 means: no limit)
 *             trial    # of status checks
 *             time     # of cycles to wait for command sending and replies
 * Returns:    SIU status word or -1 if no status word can be read
 */

/***************************************************************/
void ddlInterpretStw(stword_t stw);

/***************************************************************/
void ddlInterpretIFSTW(rorc_pci_dev_t *prorc, __u32 ifstw, char* pref,
                                                           char* suff);
/*
 * Interpret DIU or SIU IFSTW
 */

/***************************************************************/
void ddlInterpretHW(rorcHwSerial_t hw, int destiantion);
/*
 * Interpret DIU or SIU hardware version status
 */

/***************************************************************/
void ddlInterpretFW(unsigned long fw);
/*
 * Interpret DIU or SIU firmewre version status
 */

/***************************************************************/
void interpretFESTW(stword_t stw);
/*
 * Interpret FEE status word
 */

/***************************************************************/
void interpretFEMUSTW(unsigned address, stword_t stw);
/*
 * Interpret FEMU status word
 */

/***************************************************************/
int ddlSendCommandAndWaitReply(rorc_pci_dev_t *dev,
                               __u32           fee_command,
                               __u32           fee_address,
                               long long int   timeout,
                               stword_t        *stw,
                               int             expected,
                               int             *n_reply);

/***************************************************************/
int ddlSendCommand(rorc_pci_dev_t *dev,
                   int             dest,
                   __u32           command,
                   int             transid,
                   __u32           param,
                   long long int   time);
 
/* ddlSendCommand sends one command to the given link.
 * Parameters: dev      pointer to pRorc device. It defines the link
 *                      where the command will be sent
 *             dest     command destination: 0 RORC, 1 DIU, 2 SIU, 4 FEE.
 *                      if -1 then the full command is in the
 *                      command field
 *             command  command code
 *             transid  transaction id
 *             param    command parameter,
 *                      or the full command if dest == -1
 *             time     if > 0 then test if command can be sent and
 *                      wait as many cycles if necessary.
 *
 * Returns:
 *                      0       if command sent
 *                      time    if the command not sent in timeout.
 */


/****************************************************************/
int ddlReadDataBlock(rorc_pci_dev_t   *dev,
                     unsigned long    data_phys,
                     unsigned long    return_phys,
                     rorcReadyFifo_t  *rf,
                     __u32            fee_address,
                     long long int    timeout,
                     stword_t         *stw,
                     int              *n_reply,
                     int              *step);



/****************************************************************/
int ddlWriteDataBlock(rorc_pci_dev_t        *dev,
                      unsigned long          data_phys,
                      unsigned long          data_length,
                      unsigned long          return_phys,
                      volatile unsigned long *return_addr,
                      __u32                  fee_address,
                      long long int          timeout,
                      stword_t               *stw,
                      int                    *n_reply,
                      int                    *step);



/****************************************************************/
int ddlSendData(rorc_pci_dev_t         *dev,
                unsigned long           sourceaddr,
                unsigned long           length,
                unsigned long           return_phys,
                volatile unsigned long *return_virt,
                long long int           time,
                int                     wait_ready);

/******************************************************************************
* This function is used to send data block                                    *
* Parameters: dev         pointer to pRORC device,                            *
*             sourceaddr  the physical address of the data to be sent         *
*             length      data lenght in words                                *
*             return_phys return physical address                             *
*             return_virt virtual pointer to return address                   *
*             time        if > 0 then wait as many cycles and test if previou *
*                         transfer done.                                      *
*             wait_ready  if !=0 then wait for DMA ending                     *
*                                                                             *
* Returns:   RORC_LINK_NOT_ON  link is not on, data can not be sent           *
*            RORC_NOT_ABLE     if the previous download was not finished      *
*            RORC_TIMEOUT      if the given transfer is not done in timeout   * 
*            RORC_STATUS OK    if the transfer done                           *
******************************************************************************/


/********************************************************/
int ddlWriteJTAGBlock(rorc_pci_dev_t        *dev,
                      unsigned long          data_phys,
                      unsigned long          data_length,
                      unsigned long          return_phys,
                      volatile unsigned long *return_addr,
                      __u32                  fee_address,
                      long long int          timeout,
                      stword_t               *stw,
                      int                    *n_reply,
                      int                    *step);


/****************************************************************/
int ddlSendJTAG(rorc_pci_dev_t         *dev,
	        unsigned long           sourceaddr,
	        unsigned long           length,
	        unsigned long           return_phys,
	        volatile unsigned long *return_virt,
	        long long int           time);

/******************************************************************************
* This function is used to send JTAG block                                    *
* Parameters: dev         pointer to pRORC device,                            *
*             sourceaddr  the physical address of the JTAG data to be sent    *
*             length      data lenght in words                                *
*             return_phys return physical address                             *
*             return_virt virtual pointer to return address                   *
*             time        if > 0 then wait as many cycles and test if the     *
*                         JTAG response arrived                               *
*                                                                             *
* Returns:   RORC_STATUS OK    if the transfer done                           *
*            RORC_LINK_NOT_ON  link is not on, data can not be sent           *
*            RORC_TIMEOUT      if the response does not arrived in timeout    *
******************************************************************************/


/****************************************************************/
stword_t ddlReadStatus(rorc_pci_dev_t *prorc);

/****************************************************************/
long long int ddlWaitStatus(rorc_pci_dev_t *prorc, long long int timeout);

/* Checks whether status mail box not empty in timeout
 *
 * Parameters: prorc    pointer to pRORC device
 *             timeout  # of check cycles
 *
 * Returns:    # of executed cycles
 *
*/

/****************************************************************/
__u8 ddlReadHW(rorc_pci_dev_t *prorc, int dest, int address, long long int time);
/****************************************************************/
rorcHwSerial_t ddlSerial(rorcHandle_t prorc, int dest, long long int time);

/****************************************************************/
unsigned long ddlReadFW(rorc_pci_dev_t *prorc, int destination, 
                                                          long long int time);

/****************************************************************/
unsigned long ddlReadDiu(rorc_pci_dev_t *prorc, int transid, long long int time);

/****************************************************************/
unsigned long ddlReadSiu(rorc_pci_dev_t *prorc, int transid, long long int time);

/****************************************************************/
char *ddlPrintStatus(rorc_pci_dev_t *prorc, int dest, int prefix);

/****************************************************************/
stword_t ddlReadFESTW(rorc_pci_dev_t *prorc, int fee_addr, int transid, 
                                                            long long int time);

/***************************************************************/
stword_t ddlReadCTSTW(rorc_pci_dev_t *prorc, int transid, int destination, 
                                                           long long int time);

/****************************************************************/
int ddlDiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int ddlSetSiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int ddlClearSiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw);

/*****************************************************************/
int ddlSetSiuTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int ddlStartTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int ddlReadPRBS(rorcHandle_t dev, int transid, long long int time, stword_t *stw);

/****************************************************************/
int ddlStopTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int rorcStartTrigger(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int ddlStartBlockWrite(rorcHandle_t dev, unsigned long fee_address,
                       long long int timeout, stword_t *stw);

/****************************************************************/
int ddlStartBlockRead(rorcHandle_t dev, unsigned long fee_address,
                      long long int timeout, stword_t *stw);

/****************************************************************/
int rorcStopTrigger(rorcHandle_t handle, long long int timeout, stword_t *stw);

/****************************************************************/
int rorcSendJSTART(rorcHandle_t dev, long long int timeout, stword_t *stw);

/****************************************************************/
int rorcArmDDL(rorc_pci_dev_t *dev, int option);
/****************************************************************/

/****************************************************************/
int rorcArmFeic(rorc_pci_dev_t *dev,  int           pattern,
                                      int           evlen,
                                      int           trig,
                                      int           flctrl,
                                      int           trdis,
                                      int           seed,
                                      long long int time,
                                      int *rounded_len);
/****************************************************************/

#ifdef __cplusplus
}
#endif

#endif
