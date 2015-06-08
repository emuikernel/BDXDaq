/* if_seeq.h - Seeq 8005 Ethernet interface header */

/* Copyright 1984-1996 Wind River Systems, Inc. */
/*
modification history
--------------------
01a,21apr96,mem  written.
*/

#ifndef __INCif_seeqh
#define __INCif_seeqh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Control block definitions for SEEQ 8005 (Ethernet) chip.
 */

#ifndef	_ASMLANGUAGE

/*
 * Register layout.
 */
typedef struct				/* SEEQ_DEVICE */
    {
    volatile USHORT *	pCmd;		/* command register */
    volatile USHORT *	pStat;		/* status register */
    volatile USHORT *	pCfg1;		/* config register #1 */
    volatile USHORT *	pCfg2;		/* config register #2 */
    volatile USHORT *	pRxend;		/* receive area end register */
    volatile USHORT *	pBwind;		/* buffer window register */
    volatile USHORT *	pRxptr;		/* receive pointer register */
    volatile USHORT *	pTxptr;		/* receive pointer register */
    volatile USHORT *	pDma;		/* DMA address register */
    } SEEQ_DEVICE;

#endif /* _ASMLANGUAGE */

/* Definitions for fields and bits in the SEEQ_DEVICE */

/* Command register */
#define SEEQ_CMD_DMA_EN		0x0001	/* DMA interrupt enable */
#define SEEQ_CMD_RX_EN		0x0002	/* Receive interrupt enable */
#define SEEQ_CMD_TX_EN		0x0004	/* Transmit interrupt enable */
#define SEEQ_CMD_BUF_EN		0x0008	/* Buffer window interrupt enable */
#define SEEQ_CMD_DMA_ACK	0x0010	/* DMA interrupt acknowledge */
#define SEEQ_CMD_RX_ACK		0x0020	/* Receive interrupt acknowledge */
#define SEEQ_CMD_TX_ACK		0x0040	/* Transmit interrupt acknowledge */
#define SEEQ_CMD_BUF_ACK	0x0080	/* Buffer window int acknowledge */
#define SEEQ_CMD_DMA_ON		0x0100	/* set DMA on */
#define SEEQ_CMD_RX_ON		0x0200	/* set Receive on */
#define SEEQ_CMD_TX_ON		0x0400	/* set Transmit on */
#define SEEQ_CMD_DMA_OFF	0x0800	/* set DMA off */
#define SEEQ_CMD_RX_OFF		0x1000	/* set Receive off */
#define SEEQ_CMD_TX_OFF		0x2000	/* set Transmit off */
#define SEEQ_CMD_FIFO_READ	0x4000	/* fifo read */
#define SEEQ_CMD_FIFO_WRITE	0x8000	/* fifo write */

/* Command register shorthand */
#define SEEQ_CMD_ALL_ACK	0x00F0	/* acknowledge all interrupts */
#define SEEQ_CMD_ALL_OFF	0x3800	/* turn everything off */

/* Status register */
#define SEEQ_STAT_DMA_EN	0x0001	/* DMA interrupt enable */
#define SEEQ_STAT_RX_EN		0x0002	/* Receive interrupt enable */
#define SEEQ_STAT_TX_EN		0x0004	/* Transmit interrupt enable */
#define SEEQ_STAT_BUF_EN	0x0008	/* Buffer window interrupt enable */
#define SEEQ_STAT_DMA_INT	0x0010	/* DMA interrupt */
#define SEEQ_STAT_RX_INT	0x0020	/* Receive interrupt */
#define SEEQ_STAT_TX_INT	0x0040	/* Transmit interrupt */
#define SEEQ_STAT_BUF_INT	0x0080	/* Buffer window int */
#define SEEQ_STAT_DMA_ON	0x0100	/* set DMA on */
#define SEEQ_STAT_RX_ON		0x0200	/* set Receive on */
#define SEEQ_STAT_TX_ON		0x0400	/* set Transmit on */
#define SEEQ_STAT_FIFO_FULL	0x2000	/* DMA fifo full */
#define SEEQ_STAT_FIFO_EMPTY	0x4000	/* DMA fifo empty */
#define SEEQ_STAT_FIFO_DIR	0x8000	/* DMA fifo direction 0->wr 1->rd */

#define SEEQ_STAT_CMD_MASK	0x000f	/* */

/* config1 register */
#define SEEQ_CONF1_ADDR0	0x0000	/* station address 0 */
#define SEEQ_CONF1_ADDR1	0x0001	/* station address 1 */
#define SEEQ_CONF1_ADDR2	0x0002	/* station address 2 */
#define SEEQ_CONF1_ADDR3	0x0003	/* station address 3 */
#define SEEQ_CONF1_ADDR4	0x0004	/* station address 4 */
#define SEEQ_CONF1_ADDR5	0x0005	/* station address 5 */
#define SEEQ_CONF1_PROM		0x0006	/* address PROM */
#define SEEQ_CONF1_TXEND	0x0007	/* Tx end area */
#define SEEQ_CONF1_LBUFFER	0x0008	/* Local buffer memory */
#define SEEQ_CONF1_IVEC		0x0009	/* Interrupt vector */
#define SEEQ_CONF1_DMA_BURST_LENGTH_1	0x0000	/* dma burst length 1 */
#define SEEQ_CONF1_DMA_BURST_LENGTH_4	0x0040	/* dma burst length 4 */
#define SEEQ_CONF1_DMA_BURST_LENGTH_8	0x0080	/* dma burst length 8 */
#define SEEQ_CONF1_DMA_BURST_LENGTH_16	0x00c0	/* dma burst length 16 */
#define SEEQ_CONF1_DMA_BURST_INTERVAL_0	0x0000	/* continuous */
#define SEEQ_CONF1_ADDR0_EN	0x0100	/* station address 0 enable */
#define SEEQ_CONF1_ADDR1_EN	0x0200	/* station address 1 enable */
#define SEEQ_CONF1_ADDR2_EN	0x0400	/* station address 2 enable */
#define SEEQ_CONF1_ADDR3_EN	0x0800	/* station address 3 enable */
#define SEEQ_CONF1_ADDR4_EN	0x1000	/* station address 4 enable */
#define SEEQ_CONF1_ADDR5_EN	0x2000	/* station address 5 enable */
#define SEEQ_CONF1_RX_SPEC		0x0000	/* specific addr receive */
#define SEEQ_CONF1_RX_SPEC_BROAD	0x4000	/* spec & broadcast */
#define SEEQ_CONF1_RX_SPEC_MULTI	0x8000	/* spec, broad & multicast */
#define SEEQ_CONF1_RX_PROMISCUOUS	0xc000	/* promiscuous mode */

/* config2 register */
#define SEEQ_CONF2_BYTE_SWAP	0x0001	/* byte swap */
#define SEEQ_CONF2_RESET	0x8000	/* Reset chip */

/* Rx header status */
#define SEEQ_RX_HSTAT_CHAIN	0x40	/* chain continue */
#define SEEQ_RX_HSTAT_RECEIVED	0x80	/* 0 -> receive packet header */

/* Rx packet status */
#define SEEQ_RX_PSTAT_OVERSIZE	0x01	/* oversize packet */
#define SEEQ_RX_PSTAT_CRC	0x02	/* crc error */
#define SEEQ_RX_PSTAT_DRIBBLE	0x04	/* dribble packet */
#define SEEQ_RX_PSTAT_SHORT	0x08	/* short packet */
#define SEEQ_RX_PSTAT_DONE	0x80	/* packet done */

/* Tx command byte */
#define SEEQ_TX_CMD_BABBLE_EN	0x01	/* enable babble int */
#define SEEQ_TX_CMD_COLL_EN	0x02	/* enable collision int */
#define SEEQ_TX_CMD_COLL16_EN	0x04	/* enable 16 collisions int */
#define SEEQ_TX_CMD_SUCC_EN	0x08	/* enable success int */
#define SEEQ_TX_CMD_DATA	0x20	/* data follows */
#define SEEQ_TX_CMD_CHAIN	0x40	/* not last packet */
#define SEEQ_TX_CMD_XMIT	0x80	/* TX packet type */

/* Tx packet status */
#define SEEQ_TX_PSTAT_BABBLE	0x01	/* */
#define SEEQ_TX_PSTAT_COLL	0x02	/* */
#define SEEQ_TX_PSTAT_COLL16	0x04	/* */
#define SEEQ_TX_PSTAT_DONE	0x80	/* packet done */


#ifdef __cplusplus
}
#endif

#endif /* __INCif_seeqh */
