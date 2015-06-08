/* if_vlan.h - VLAN ethernet board transparent mode library */
 
/* Copyright 1990-1994, PEP Modular Computers */
/* Copyright 1984-1994, Wind River Systems, Inc. */

/*
modification history
--------------------
01e,07jan19,haw  removed include of copyright, fixed spacing, indentation
01d,14apr93,haw  renamed to if_vlan.h (used to be vlanLib.h),
                 moved it to this directory, copyied definition of 
                 VLAN structure to this file, moved all includes to the driver
01c,02jul92,haw  added definition of TM_V104, added version in COMMON_WS
01b,04feb92,haw  changed LAN_MAX_SIZE from 1512 to 1514
01a,18dec91,haw  modified from EMOD version to transparent mode
*/
 
#ifndef __INCif_vlanh
#define __INCif_vlanh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VLAN_VERSION
#define TM_V104         0x104

#define EADDRS          6

#define GOTTRAILERTYPE  0
#define GOTTYPE         1
#define GOTNOTYPE       2
 
/* VLAN address offsets and constants */

#define VLAN_START_ADDR         0xFFFC
#define VLAN_ID_ADDR            0xFFFA

#define VLAN_SOFT_START_ADDR1   0xFF04  /* see VLAN firmware by MAT */
#define VLAN_SOFT_START_ADDR2   0xFF00
#define VLAN_FIRM_ADDR          0x00007026
#define VLAN_FIRM_CODE          1789

/* AMD 7990 ETHERNET CONTROLLER (LANCE) EQUATES */

#define LAN_MIN_SIZE    60
#define LAN_MAX_SIZE    1514

/* CSR0 EQUATES */

#define CSR0_ERR        0x8000           
#define CSR0_BABL       0x4000           
#define CSR0_CERR       0x2000           
#define CSR0_MISS       0x1000           
#define CSR0_MERR       0x0800           
#define CSR0_RINT       0x0400           
#define CSR0_TINT       0x0200           
#define CSR0_IDON       0x0100           
#define CSR0_INTR       0x0080           
#define CSR0_INEA       0x0040           
#define CSR0_RXON       0x0020           
#define CSR0_TXON       0x0010           
#define CSR0_TDMD       0x0008           
#define CSR0_STOP       0x0004           
#define CSR0_STRT       0x0002           
#define CSR0_INIT       0x0001           

/* Lance init block definition */

typedef struct
    {
    UINT16  mode;
#define PROM    0x8000
    UINT16  ether_15;
    UINT16  ether_31;
    UINT16  ether_47;
    UINT16  filter_15;
    UINT16  filter_31;
    UINT16  filter_47;
    UINT16  filter_63;
    UINT16  rdra_15;
    UINT8   rlen;
    UINT8   rdra_23;
    UINT16  tdra_15;
    UINT8   tlen;
    UINT8   tdra_23;
    } INIT_BLOCK;

/* Receiver ring descriptor definition */

typedef struct
    {
    UINT16  addr_15;
    UINT8   mode;
    UINT8   addr_23;
    UINT16  bcnt;
    UINT16  mcnt;
    } RECV_DESC;

/* Receiver mode definitions */

#define RRD_OWN        0x80           
#define RRD_ERR        0x40           
#define RRD_FRAM       0x20           
#define RRD_OFLO       0x10           
#define RRD_CRC        0x08           
#define RRD_BUFF       0x04           
#define RRD_STP        0x02           
#define RRD_ENP        0x01           

/* Transmitter ring descriptor definition */

typedef struct
    {
    UINT16  addr_15;    /* bits 15:00 of transmit buffer */
    UINT8   mode;       /* mode */
    UINT8   addr_23;    /* bits 23:16 of transmit buffer */
    UINT16  bcnt;       /* message byte count */
    UINT16  tdra_6;     /* errors */
    } TRAN_DESC;

/* Transmitter mode definitions */

#define TRD_OWN        0x80           
#define TRD_ERR        0x40           
#define TRD_MORE       0x10           
#define TRD_ONE        0x08           
#define TRD_DEF        0x04           
#define TRD_STP        0x02           
#define TRD_ENP        0x01           

/* Transmit tdra_6 definitions */

#define TRD_BUFF       0x8000           
#define TRD_UFLO       0x4000           
#define TRD_LCOL       0x1000           
#define TRD_LCAR       0x0800           
#define TRD_RTRY       0x0400           

/* Communication Workspace used by the driver */

#define SEND          8     /* number of send buffers */
#define RECV         16     /* number of recv buffers */
#define RECV_SIZE 0x600     /* size of buffers (recv/send) */

/* this struct must be located in VLAN VME ram at first address */

typedef struct
    {
    volatile UINT16 readCSR0;
                            /* = LANCE csr0 after a read request */
    volatile INT16 reqReadCSR0;
                            /* when set to 1 VLAN cpu copies LANCE csr0 */
                            /* to csr0_read and clears req_csr0_read */
    volatile UINT16 writeCSR0;       
                            /* VLAN cpu copies this value to LANCE csr0 */
                            /* when req_csr0_write == 1 and clears req */
    volatile INT16 reqWriteCSR0;

    volatile UINT16 reqClearIRQ;
                            /* set to 1 -> VLAN cpu allowed to give */
                            /* other VME irq (after setting req to 0*/

    UINT8 intLevel;         /* vme irq level to be used by VLAN */
    UINT8 intVector;        /* vme irq vector to be used by VLAN */

    char *vlanBaseVME;      /* address of VLAN VME mem seen by vm20 */

    UINT8 ether_addr_1;     /* addr specific for VLAN board */
    UINT8 ether_addr_2; 
    UINT8 ether_addr_3; 
    UINT8 ether_addr_4; 
    UINT8 ether_addr_5; 
    UINT8 ether_addr_6; 

    INT16 version;
    INT16 res [3];

    INT32 copyFlag;         /* flag for send function: */
                            /* 0 - do not copy, lance can access local data */
                            /* 1 - copy the data */

    RECV_DESC *ringRxFst,   /* pointer to start recv ring */
              *ringRxLst,   /* pointer to last recv ring */
              *ringRxNxt;   /* pointer to current recv ring */

    TRAN_DESC *ringTxFst,   /* pointer to start send ring */
              *ringTxLst,   /* pointer to last send ring */
              *ringTxNxt;   /* pointer to current send ring */

    INT32 r_lock, t_lock;   /* recv/send function locks */
    INIT_BLOCK initBlock;
                            /* this area is used to initialize the */
                            /* lance chip. */
    char descRx [1040],     /* reserved for recv ring, its size */
                            /* must be = 4*rblk_count + 8 */
         descTx [1040];     /* reserved for send ring, its size */
                            /* must be = 4*tblk_count + 8 */

    char recvBuf [RECV_SIZE*RECV];
                            /* reserved for receive data by lance */
    char sendBuf [RECV_SIZE*SEND];
                            /* reserved for send data by lance */
    } COMMON_WS; 

typedef struct
    {
    struct  arpcom vs_ac;       /* common ethernet structures */
    int vlanIntLevel;           /* vme interrupt level */
    int vlanIntVec;             /* vme interrupt vector */
    BOOL taskLevelActive;       /* netTask is currently processing packets */
    COMMON_WS *comWoSp;         /* pointer to VLAN RAM base */
    SEM_ID transmitSem;         /* semaphore to lock transmission */
    } VLAN_SOFTC;

#define vs_if           vs_ac.ac_if     /* network-visible interface */
#define vs_enaddr       vs_ac.ac_enaddr /* hardware ethernet address */

/*
 * the structure arpcom looks in detail this way:
 *  struct ifnet ac_if;         ( struct ifnet )
 *  UINT8 ac_enaddr [6];        ( ethernet hardware address )
 *  UINT32 ac_ipaddr.s_addr;    ( copy of ip address )
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCif_vlanh */
