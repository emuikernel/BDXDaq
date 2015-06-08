/* if_pn.h -  proNET-80 driver header file  */

/* @(#)if_pn.h	2.2 8/20/86	(c) 1986 Proteon, Inc. */

/*
modification history
--------------------
01b,03sep88,gae  changed PNTIMEOUT from 10.
01a,06nov87,rdc  written.
*/

#ifndef	INCpnh
#define	INCpnh

struct pn_header {
    /* the first two fields are required by the hardware */
    u_char  vh_dhost;  /* destination address */
    u_char  vh_shost;  /* source address */
    /* the next three fields are the local network header */
    u_char  vh_version;    /* header version */
    u_char  vh_type;       /* packer type => protocol number */
    u_short vh_info;       /* protocol-specific information */
};

#define RING_VERSION	2	/* current version of v2lni header */

/*
 * Packet types (protocol numbers) in proNET protocol header
 * Other types are defined, but are proprietary.
 */
#define	RING_IP		1
#define	RING_IPTrailer	2	/* really, 3 = 512 bytes */
				/*         4 = 1024 bytes */
				/*         5 = 1536 bytes */
				/* it's really very messed-up! */
#define	RING_IPNTrailer	4	/* not a number, but a range */
#define RING_ARP	3	/* the next three conflict with trailers */
#define RING_HDLC	4
#define RING_VAXDB	5
#define RING_RINGWAY	6
#define RING_RINGWAYM	8
#define	RING_NOVELL	10
#define RING_PUP	12
#define RING_XNS	14
#define	RING_DIAGNOSTICS 15	/* protocol type for testing */
#define	RING_ECHO	16

#define PN_BROADCAST	0xFF	/* hardware-defined broadcast address */
#define PN_GRPOF(c)	((c >> 4) & 0xF)	/* extract group number */
#define PN_MBROF(c)	(c & 0xF)		/* extract member number */
#define PN_GRPCAST(c)	(!PN_MBROF(c))		/* is it a group broadcast */
#define PN_REGOFF	0x4000	/* address offset into the registers */
#define RCMD_CHAN	0
#define RCV_CHAN	1
#define XCMD_CHAN	2
#define XMT_CHAN	3
#define TBUFSIZE	0x400			/* in lwords */
#define RBUFSIZE	0x4000			/* in bytes */

#define RDMA	0x8000		/* DMA_INT bit 15 */
#define TDMA	0x80		/* DMA_INT bit 7  */
#define NXM	0x10		/* DMA_INT bit 4  */
#define RSTRDMA	0x4000		/* DMA_INT bit 14 resets RDMA */
#define RSTTDMA 0x40		/* DMA_INT bit 6 resets TDMA  */
#define RSTNXM  0x10		/* DMA_INT bit 4 resets NXM   */

/* the following structure is the 82258 register map
 * address offset for these registers is 0x4000
 * sizeof(dmac_regs) should be 0x100
 * therefore sizeof(struct dmac_regs) should be 0x40
 */
struct dmac_regs {		/* all dummy fields are place holders */
    u_char  gcr;		/* general command register */
    u_char  dummy0;
    u_char  scr;		/* subchannel register */
    u_char  dummy1;
    u_short gsr;		/* general status register */
    u_short dummy2;		/* missing in 82258 register map */
    u_short gmr;		/* general mode register */
    u_char  gbr;		/* general burst register */
    u_char  dummy3;
    u_char  gdr;		/* general delay register */
    u_char  dummy4;
    u_short dummy5;		/* missing in 82258 register map */
    u_char  csr;		/* channel[0-3] status registers */
    u_char  dummy6;
    u_short dar;		/* channel[0-3] data assembly registers */
    u_short maskr;		/* channel[0-3] mask registers */
    u_short cmpr;		/* channel[0-3] compare registers */
    u_char  mivr;		/* multiplexer interrupt vector register */
    u_char  dummy7;
    u_char  lvr;		/* last vector register */
    u_char  dummy8;
    u_short dummy9;		/* missing in 82258 register map */
    u_short dummy10;		/* missing in 82258 register map */
    u_short cprL;		/* channel[0-3] command pointer registers */
    u_char  cprH;
    u_char  dummy11;
    u_short sprL;		/* channel[0-3] source pointer registers */
    u_char  sprH;
    u_char  dummy12;
    u_short dprL;		/* channel[0-3] dest pointer registers */
    u_char  dprH;
    u_char  dummy13;
    u_short ttprL;		/* channel[0-3] translate table pointer regs */
    u_char  ttprH;
    u_char  dummy14;
    u_short lprL;		/* channel[0-3] list pointer registers */
    u_char  lprH;
    u_char  dummy15;
    u_short dummy16;		/* missing in 82258 register map */
    u_short dummy17;		/* missing in 82258 register map */
    u_short bcrL;		/* channel[0-3] byte count registers */
    u_char  bcrH;
    u_char  dummy18;
    u_short ccrL;		/* channel[0-3] command registers */
    u_char  ccrH;
    u_char  dummy19;
};
/* these registers only appear once in the dmac_regs array */
#define GCR_OF(c)	((c)->pn_dmac[0].gcr)
#define SCR_OF(c)	((c)->pn_dmac[0].scr)
#define GSR_OF(c)	((c)->pn_dmac[0].gsr)
#define GMR_OF(c)	((c)->pn_dmac[0].gmr)
#define GBR_OF(c)	((c)->pn_dmac[0].gbr)
#define GDR_OF(c)	((c)->pn_dmac[0].gdr)
#define MIVR_OF(c)	((c)->pn_dmac[3].mivr)
#define LVR_OF(c)	((c)->pn_dmac[3].lvr)

/* the following are interrupt vector, dma page, and csr registers
 * address offset for this structure is 0x4000 + sizeof(dmac_regs)
 */
struct ctl_regs {		/* all dummy fields are place holders */
    u_short rdmaint;		/* rcv dma interrupt vector */
    u_short dummy0;
    u_short tdmaint_nxm;	/* xmt dma and nxm interrupt vector */
    u_short dummy1;
    u_short rcsrint;		/* rcv done interrupt vector */
    u_short dummy2;
    u_short tcsrint;		/* xmt done interrupt vector */
    u_short dummy3;
    u_short pronet_pg;		/* high 8 bits for pronet page addr in DMA */
    u_short dummy4;
    u_short cmd_pg;		/* high 8 bits for cmd page addr in DMA */
    u_short dummy5;
    u_short rcvdata_pg;		/* high 8 bits for rcv data page addr in DMA */
    u_short dummy6;
    u_short xmtdata_pg;		/* high 8 bits for xmt data page addr in DMA */
    u_short dummy7;
    u_short tcsr;		/* xmt csr */
    u_short dummy8;
    u_short dummy9;		/* tbufad can only be written to at 0x4126! */
    u_short tbufad;		/* xmt buffer addr reg */
    u_short tbufda;		/* xmt buffer data reg */
    u_short rfsreg;		/* refuse reg */
    u_short dma_int;		/* dma and interrupt control reg */
    u_short dummy10;
    u_short dummy11;		/* rcsr can only be written to at 0x4132! */
    u_short rcsr;		/* rcv csr */
    u_short rcntr;		/* rcv count reg (R) */
    u_short rcounter;		/* rcv buffer input counter (W) */
    u_short rstatr;		/* rcv status reg (R) */
    u_short rstcntr;		/* reset rcntr reg (W) */
    u_short rbufdmaptr;		/* rcv buffer DMA pointer */
    u_short dummy12;
};

#define PN_ORI		01	/* originate/xmt/send pkt */
#define PN_CEN		01	/* enable copy of rcv pkt into RBUFFER */
#define PN_JNR		02	/* enable relay in wire center for join ring */
#define PN_MEN		04	/* enable CTL modem */
#define PN_INI		020	/* create token on current ORIGR if RNOK */
#define PN_RST  	040	/* reset xmt/rcv to known state */
#define PN_IEN		0100	/* IEN: ORIGC(CTL), OTIM(TCSR) or CNTR(RCSR) */
#define PN_RDY		0200	/* xmt - TCSR or rcv - RCNTR & RSTATR ready */
#define PN_REF		0400	/* last pkt sent wasn't accepted */
#define PN_CSW		0400	/* valid count and status waiting to be xfer */
#define PN_BDF		02000	/* last pkt sent had format error */
#define PN_TMO		04000	/* timed out waiting for ORIGC(CTL) */
#define PN_PER		04000	/* parity error detected */
#define PN_JND		010000	/* CTL is acknowledging a JOINR(RCSR) cmd */
#define PN_NOK		020000	/* NOTOKEN or NOFLAG timer has expired */
#define PN_RTI		040000	/* reset xmt/rcv int */
#define PN_IRQ		0100000	/* xmt/rcv csr int request */

#define PN_TBITS \
    "\10\20IRQ\17RTI\16NOK\15JND\14TMO\13BDF\11REF\10RDY\7IEN\6RST\5INI\1ORI"
#define PN_RBITS \
    "\10\20IRQ\17RTI\16NOK\15JND\14PER\11CSW\10RDY\7IEN\6RST\3MEN\2JNR\1CEN"
#define PN_SBITS 	"\10\3PERR\2RBDF\1ROVR"

#define PERR	04		/* parity error bit in RSTATR */
#define RBDF	02		/* bad format error bit in RSTATR */
#define ROVR	01		/* pkt length overflow error bit in RSTATR */

struct pn_regs {
    u_short pn_rbuffer[PN_REGOFF>>1];	/* rcv buffer */
    struct dmac_regs pn_dmac[4];	/* 82258 reg map */
    struct ctl_regs  pn_ctl;		/* int vec, dma pg, and csrs reg map */
};

#define PNTERR	(PN_TMO | PN_BDF | PN_REF)	/* any xmt error */
#define PNRERR	(PERR | RBDF | ROVR)		/* any rcv error */

#define PNRETRY		10	/* xmt retry limit */
#define PNIDENTRETRY	10	/* identify retry limit */
#define PNIDENTSUCC	 5	/* number of successful sends required */
#define PNTIMEOUT	10	/* seconds before a xmt timeout */

/* 82258 DMAC specific stuff */
struct short_cmd {
    u_short type1_cmd;
    u_short sprL;
    u_char  sprH;
    u_char  dummy0;
    u_short dprL;
    u_char  dprH;
    u_char  dummy1;
    u_short bcrL;
    u_char  bcrH;
    u_char  dummy2;
    u_short csr;
};

struct long_cmd {
    struct short_cmd shcmd;
    u_short ext_cmd;
    u_short mask;
    u_short cmpr;
    u_short ttprL;
    u_char  ttprH;
    u_char  dummy0;
};

struct debug_cmd {
    struct long_cmd cmdbl;
    u_short updated_sprL;
    u_char  updated_sprH;
    u_char  dummy0;
    u_short updated_dprL;
    u_char  updated_dprH;
    u_char  dummy1;
    u_short updated_bcrL;
    u_char  updated_bcrH;
    u_char  dummy2;
};

#define gsr_of(chan,addr)	((addr)[0].gsr >> (chan))

#endif	INCpnh
