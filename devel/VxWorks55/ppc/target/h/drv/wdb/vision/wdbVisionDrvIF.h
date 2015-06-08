/* wdbVisionDrvIF.h - Interface definitions/declations for vision DRIVER */

/* Copyright 1986-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01c,01dec01,g_h  Cleaning for T2.2
01b,05may01,g_h  rename to wdbVisionDrvIF.h
01a,09apr01,rnr  written.
*/

#ifndef __INCwdbVisionDrvIFh
#define __INCwdbVisionDrvIFh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define VDR_SUCCESS                    0 /* Indicates success of requested operation   */
#define VDR_FAILURE                    1 /* Indicates failure of requested operation   */
                                         /*                                            */
#define VDR_WRITE_COMPLETE             0 /* Previously requested WRITE is     done     */
#define VDR_WRITE_PENDING              1 /* Previously requested WRITE is not done     */
                                         /*                                            */
#define VDR_DATA_FOUND                 0 /* Data is     pending on the receive channel */
#define VDR_DATA_NONE                  1 /* Data is NOT pending on the receive channel */

/* typedefs */

typedef unsigned char                  VDR_UCHAR; /* Host independent unsigned 1-byte unit */
typedef unsigned long                  VDR_ULONG; /* Host independent unsigned 4-byte unit */
typedef void *                         VDR_PDATA; /* Host independent void     4-byte unit */

#if defined(__STDC__)

/* Open/Close prototypes */
 
typedef VDR_ULONG (*VDR_OPEN_PTR)      (VDR_PDATA priv);
typedef VDR_ULONG (*VDR_CLOSE_PTR)     (VDR_PDATA priv);

/* Read/Write prototypes */

typedef VDR_ULONG (*VDR_READ_STATUS_PTR)  (VDR_PDATA priv, VDR_ULONG * status);
typedef VDR_ULONG (*VDR_WRITE_STATUS_PTR) (VDR_PDATA priv, VDR_ULONG * status);

/* ReadStatus/WriteStatus prototypes */

typedef VDR_ULONG (*VDR_READ_PTR)      (VDR_PDATA priv,    VDR_UCHAR * pData, 
                                        VDR_ULONG maxSize, VDR_ULONG * rcvBytes);

typedef VDR_ULONG (*VDR_WRITE_PTR)     (VDR_PDATA priv, VDR_UCHAR * pData, VDR_ULONG size);

#else   /* __STDC__ */

/* Open/Close prototypes */
 
typedef VDR_ULONG (*VDR_OPEN_PTR)();
typedef VDR_ULONG (*VDR_CLOSE_PTR)();

/* Read/Write prototypes */

typedef VDR_ULONG (*VDR_READ_STATUS_PTR )();
typedef VDR_ULONG (*VDR_WRITE_STATUS_PTR)();

/* ReadStatus/WriteStatus prototypes */

typedef VDR_ULONG  (*VDR_READ_PTR)();
typedef VDR_ULONG  (*VDR_WRITE_PTR)();

#endif  /* __STDC__ */

/*
 *   This data structure represent the interface exchange between
 *   the High-Level Vision driver ( wdbVisioDrv ) and the Low-Level I/O
 *   subsystem driver.  vDriver code will make  an initialization
 *   call into the Low-Level code.  The Low-Level code will setup
 *   the  function pointer fields with  the addresses of routines 
 *   that perform  that particular operation.  Also the Low-Level
 *   I/O will fill in the PrivateData field with a value which is
 *   of meaining to the Low-Level driver. This data will be given
 *   back to the Low-Level driver on every request.
 */

typedef struct vDriverInterface
    {
    VDR_OPEN_PTR         openFunc;        /* Addr of OPEN             routine */
    VDR_CLOSE_PTR        closeFunc;       /* Addr of CLOSE            routine */
    VDR_READ_PTR         readFunc;        /* Addr of the READ         routine */
    VDR_WRITE_PTR        writeFunc;       /* Addr of the WRITE        routine */
    VDR_READ_STATUS_PTR  readStatusFunc;  /* Addr of the READ STATUS  routine */
    VDR_WRITE_STATUS_PTR writeStatusFunc; /* Addr of the WRITE STATUS routine */
    VDR_PDATA            privateData;     /* Private data space               */
    } V_DRIVER_INTERFACE;

/* function prototypes */

/* Prototype for the Initialization call */

#if defined(__STDC__)

typedef VDR_ULONG (*VDR_INIT_PTR) (V_DRIVER_INTERFACE * interfaceData);

#else   /* __STDC__ */

typedef VDR_ULONG (*VDR_INIT_PTR)();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwdbVisionDrvIFh */
