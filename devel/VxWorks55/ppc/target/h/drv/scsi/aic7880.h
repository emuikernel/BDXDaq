/* aic7880.h - AIC 7880 SCSI Host Adapter header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,29sep98,fle  made it refgen parsable
01d,17mar98,sbs  added AIC7880 PCI specific definitions.
                 added import of aic7880Intr().
01c,05aug97,dds  removed #include "him/custom.h".
01c,05aug97,dds  removed #include "him/custom.h".
01b,11jul97,dds  added #include "him/custom.h".
01a,19jun97,dds  created.
*/

/*
DESCRIPTION

This is the header file for the AIC-7880 driver
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "semLib.h"
#include "scsiLib.h"

extern int himDebug;

#undef HIM_DEBUG

#define HIM_MSG          logMsg

#define HIM_DEBUG_MSG                                                  \
    if (himDebug)                                                      \
        HIM_MSG

/* Adaptec AIC 7880 PCI SCSI Host Adapter definitions */

#define AIC7880_PCI_VENDOR_ID       0x9004
#define AIC7880_PCI_DEVICE_ID       0x8178

/* AIC 7880 SCSI Host Adapter Segment Pointer structure */

typedef struct segmentPtr
    {
    int dataPtr;
    int dataLen;
    } SEG_PTR;

/* AIC 7880 SCSI Host Adapter Thread Structure */

typedef struct aic7880Thread 	    
    {
    SCSI_THREAD     scsiThread;	/* generic SCSI thread structure  */
    struct sp     * pScb; 	/* ptr to SCSI Command block      */
    SEG_PTR       * segPtr;     /* Scatter/Gather Segment Pointer */
    } AIC_7880_THREAD;

/* AIC 7880 SCSI Controller Structure */

typedef struct                     
    {
    SCSI_CTRL         scsiCtrl;          /* generic SCSI controller info */
    struct cfp      * aic7880CfpStruct;  /* pointer to a 7880 SCSI Ctrlr */
    AIC_7880_THREAD * pHwThread;         /* ptr to current thread        */
    } AIC_7880_SCSI_CTRL;

/* Interrupt codes  */

#define INTMASK                  0xe0  

#define HA_NOT_FOUND             0x00
#define HA_FOUND                 0x01
#define HA_FOUND_DISABLED        0x81

/* Sync offset and period (SXFER register) */

#define AIC_7880_MIN_REQ_ACK_OFFSET   1   /* Minimum sync offset        */
#define AIC_7880_MAX_REQ_ACK_OFFSET   8   /* Maximum sync offset        */

#define AIC_7880_DBLSPD_50                    12
#define AIC_7880_DBLSPD_64                    16
#define AIC_7880_DBLSPD_75                    19

#define AIC_7880_PERIOD_100                   25
#define AIC_7880_PERIOD_125                   31
#define AIC_7880_PERIOD_150                   37
#define AIC_7880_PERIOD_175                   43
#define AIC_7880_PERIOD_200                   49
#define AIC_7880_PERIOD_225                   55
#define AIC_7880_PERIOD_250                   61
#define AIC_7880_PERIOD_275                   67

#define AIC_7880_MAX_XFER_WIDTH                2

#define SIMPLE_QUEUE_TAG                    0x00
#define HEAD_OF_QUEUE_TAG                   0x01
#define ORDERED_QUEUE_TAG                   0x02

/* SCB command status */

#define AIC_7880_REQUEST_IN_PROGRESS  0x00  /* SCSI request in progress     */
#define AIC_7880_CMD_COMPLETE         0x01  /* SCSI command completed       */
#define AIC_7880_REQUEST_ABORTED      0x02  /* SCSI request aborted         */
#define AIC_7880_CMD_COMP_WITH_ERROR  0x04  /* error executing SCSI request */
#define AIC_7880_INVALID_REQUEST      0x08  /* invalid SCSI request         */

/* host adapter status */

#define AIC_7880_NO_STATUS            0x00  /* host adapter stat unavailable */
#define AIC_7880_CMD_ABORT            0x04  /* cmd aborted by host           */
#define AIC_7880_CMD_ABORT_BY_HA      0x05  /* cmd aborted by host adapter   */
#define AIC_7880_UNEXPECTED_BUS_FREE  0x13  /* unexpected bus free           */
#define AIC_7880_PHASE_MISMATCH       0x14  /* phase mismatch detected       */
#define AIC_7880_HA_HW_ERROR          0x20  /* host adapter H/W error        */
#define AIC_7880_BUS_RESET            0x22  /* SCSI bus reset                */
#define AIC_7880_BUS_RESET_OTHER_DEV  0x23  /* SCSI bus reset from other dev */
#define AIC_7880_SELECT_TIMEOUT       0x11  /* SCSI selection timeout        */
#define AIC_7880_REQ_SENSE_FAILED     0x1b  /* SCSI Request Sense Failed     */
#define AIC_7880_SCSI_BUS_RESET       0x44  /* SCSI bus reset                */

/* imports */

IMPORT VOID  aic7880Intr ();

/*
 *  AIC7880LIB.C
 */

AIC_7880_SCSI_CTRL * aic7880CtrlCreate (int busNo, int devNo, int scsiBusId);

#ifdef __cplusplus
}
#endif
