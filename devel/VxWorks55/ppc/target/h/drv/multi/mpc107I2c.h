/* mpc107I2c.h - MPC107 I2C registers definition's and values  */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06jun00,bri written
*/

#ifndef	__INCmpc107I2ch
#define	__INCmpc107I2ch

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "mpc107.h"

/* defines */

/* command identifiers for I2C */

#define MPC107_I2C_READOP	0       /* read operation */
#define MPC107_I2C_WRITOP	1	/* write operation */
#define MPC107_I2C_FRMTOP	2	/* format operation */
#define MPC107_I2C_CHCKOP	3	/* check operation */


/* MPC107 configuration registers for I2C interface  */

#define MPC107_I2C_EUMBBAR 0x3000

#define MPC107_I2C_I2CADR (MPC107_I2C_EUMBBAR + 0x0)  /* I2C address reg */
#define MPC107_I2C_I2CFDR (MPC107_I2C_EUMBBAR + 0x4)  /* I2C freq div reg */
#define MPC107_I2C_I2CCR  (MPC107_I2C_EUMBBAR + 0x8)  /* I2C control reg */
#define MPC107_I2C_I2CSR  (MPC107_I2C_EUMBBAR + 0xc)  /* I2C status reg */
#define MPC107_I2C_I2CDR  (MPC107_I2C_EUMBBAR + 0x10) /* I2C data reg */


/* I2C Register masks */

#define MPC107_I2CADR_MASK 	0x000000FE     /* Address Mask */
#define MPC107_I2CFDR_MASK	0x0000003F     /* Freq Div Mask */
#define MPC107_I2CCR_MASK	0x000000FC     /* Control Reg Mask */
#define MPC107_I2CSR_MASK	0x000000F7     /* Staus Reg Mask */
#define MPC107_I2CDR_MASK	0x000000FF     /* Data Reg Mask */


/* MPC107 I2C Control register bit definitions  */

#define MPC107_I2CCR_MEN	0x00000080	/* module enable */
#define MPC107_I2CCR_MIEN	0x00000040	/* module interrupt enable */
#define MPC107_I2CCR_MSTA	0x00000020	/* master/slave mode */
#define MPC107_I2CCR_MTX	0x00000010	/* transmit/receiver mode  */
#define MPC107_I2CCR_TXAK	0x00000008	/* transfer ack enable */
#define MPC107_I2CCR_RSTA	0x00000004	/* repeat start */


/* MPC107 I2C Status register Bit values */

#define MPC107_I2CSR_MCF	0x00000080	/* data transferring */
#define MPC107_I2CSR_MAAS	0x00000040	/* addressed as a slave */
#define MPC107_I2CSR_MBB	0x00000020	/* bus busy */
#define MPC107_I2CSR_MAL	0x00000010	/* arbitration lost */
#define MPC107_I2CSR_SRW	0x00000004	/* slave read/write */
#define MPC107_I2CSR_MIF	0x00000002	/* module interrupt */
#define MPC107_I2CSR_RXAK	0x00000001	/* receive ack */


/* Frequency Divider Register's definitions */

#define MPC107_DFFSR_DATA	0x10  /* Digital Filter Freq Sampling rate */
#define MPC107_FDR_DATA		0x20  /* Frequency Divider Ratio */

/* Default value of  Freq Divider Reg */

#define MPC107_I2CFDR_DEFAULT	 (MPC107_FDR_DATA) | (MPC107_DFFSR_DATA << 8)



/* Address Data Register's definition */

#define MPC107_ADR_DATA 	      0x10  /* Slave Address */
#define MPC107_I2CADR_DEFAULT	      (MPC107_ADR_DATA) /* Default  */
                                                        /* value of */
                                                        /* Add Data Reg */
#define MPC107_I2C_DEVICE_ADDR_MASK   0xf7  /* Device Address Mask */

#define MPC107_I2C_DEVICE_READ_ENABLE 0x1   /* Read enable */


/* Control Register Definitions */

#define MPC107_I2CCR_DEFAULT   MPC107_I2CCR_MSTA  /* Default value of */
                                                  /* Control Register */


/* General definitions */

#define MPC107_I2C_WRITE            0x0 	/* Write */
#define MPC107_I2C_READ             0x1         /* Read  */
#define MPC107_I2C_READ_OR_WRITE    0x2         /* Read OR  write */
#define MPC107_I2C_READ_AND_WRITE   0x3         /* Read AND write */
#define MPC107_I2C_READ_ANDOR_WRITE 0x4         /* Read AND OR write */


/* error codes */

#define MPC107_I2C_ERR_CYCLE_START	1	/* start cycle */
#define MPC107_I2C_ERR_CYCLE_STOP	2	/* stop cycle */
#define MPC107_I2C_ERR_CYCLE_READ	3	/* read cycle */
#define MPC107_I2C_ERR_CYCLE_WRITE	4	/* write cycle */
#define MPC107_I2C_ERR_CYCLE_ACKIN	5	/* acknowledge in cycle */
#define MPC107_I2C_ERR_CYCLE_ACKOUT	6	/* acknowledge out cycle */
#define MPC107_I2C_ERR_KNOWN_STATE	7	/* known state */
#define MPC107_I2C_ERR_CYCLE_TIMEOUT	(1<<16)	/* cycle timed out */



/* typedefs */

/* driver command packet */

typedef struct mpc107I2cCmdPckt
    {
    INT32 	command;	/* command identifier */
    INT32 	status;		/* status (error code) */
    UINT32 	memoryAddress;  /* memory address */
    UINT32 	nBytes;	   	/* number of bytes to transfer */
    }MPC107_I2C_CMD_PCKT;



/* function declarations */

IMPORT VOID	mpc107I2cInit ();
IMPORT INT32 	mpc107i2cRead  (UINT32 deviceAddress, UINT32 numBytes,
                                char *pBuf);
IMPORT INT32 	mpc107i2cWrite (UINT32 deviceAddress, UINT32 numBytes,
                                char *pBuf);

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107I2ch */

