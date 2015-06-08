/****************************************************************/
/*                                                              */
/*  file: rcu.h 	                                        */
/*                                                              */
/* This is the header file for the rcu library	 		*/
/*                                                              */
/* Created 		10 May 2008				*/
/* Last Modified 	15 Jul 2008				*/
/*								*/
/****************************************************************/

/* RORC parameters */
#define RORC_REVISION0	4	
#define RORC_SERIAL0	7165
#define RORC_CHANNEL0	0

#define RORC_REVISION1	4	
#define RORC_SERIAL1	7165
#define RORC_CHANNEL1	1

#define RORC_REVISION2	4	
#define RORC_SERIAL2	7165
#define RORC_CHANNEL2	0

#define RORC_REVISION3	4	
#define RORC_SERIAL3	7165
#define RORC_CHANNEL3	1

#define RORC_REVISION4	4	
#define RORC_SERIAL4	7165
#define RORC_CHANNEL4	0

#define RORC_REVISION5	4	
#define RORC_SERIAL5	7165
#define RORC_CHANNEL5	1

/* Global registers and commands */
#define RCU_WRD_RD       0x20000

/* RCU addressable memories */
#define A_IMEM  0x0000
#define A_ROLM  0x1000
#define A_RESM  0x2000
#define A_HITM  0x5000
#define A_DATM  0x5800

/* RCU memory sizes */
#define S_IMEM 	4096 
#define S_ROLM  4096
#define S_RESM  6144
#define S_HITM	128
#define S_DATM  256

/* RCU memory mask */
#define M_IMEM 	0x003fffff 
#define M_ROLM  0x00000fff
#define M_RESM  0x01ffffff
#define M_HITM	0xffffffff
#define M_DATM  0x000fffff


/* RCU CORE Registers Addresses*/
/* Read&Write */
#define A_ACTFECLIST    0x5100	
#define A_ALTROIF	0x5101
#define A_TRGCONF	0x5102
#define A_RDOMOD	0x5103
#define A_ALTROCFG1	0x5104
#define A_ALTROCFG2	0x5105
#define A_RCU_VERSION	0x5106
#define A_BP_VERSION	0x5107
#define A_RCU_ID	0x5108
/* Read-only */
#define A_FECERRA       0x5110	
#define A_FECERRB	0x5111	
#define A_RDOERR	0x5112	
#define A_ALTROBUS	0x5114
#define A_BUSSTTRSF	0x5115
#define A_RCUBUSBUSY	0x5116
#define A_MEBCNT	0x511B
#define A_SWTTRGCNT	0x511C
#define A_AUXTRGCNT	0x511D
#define A_TTCL2ACNT	0x511E
#define A_TTCL2RCNT	0x511F
#define A_DSTBACNT    	0x5120
#define A_DSTBBCNT    	0x5121
#define A_TRSFACNT    	0x5122
#define A_TRSFBCNT     	0x5123
#define A_ACKACNT      	0x5124
#define A_ACKBCNT      	0x5125
#define A_CSTBACNT    	0x5126
#define A_CSTBBCNT    	0x5127  
#define A_DSTBNUMA 	0x5128
#define A_DSTBNUMB 	0x5129
#define A_ADDRMISMATCH	0x512A
#define A_BLENMISMATCH	0x512B
#define A_ABDFSM	0x512C
#define A_RDOFSM	0x512D
#define A_ISFSM		0x512E
#define A_EVTMANFSM	0x512F

/* RCU CORE Registers Sizes */
#define S_ACTFECLIST    1
#define S_ALTROIF	1
#define S_TRGCONF	1
#define S_RDOMOD	1
#define S_ALTROCFG1	1
#define S_ALTROCFG2	1
#define S_RCU_VERSION	1
#define S_BP_VERSION	1
#define S_RCU_ID	1
#define S_FECERRA       1
#define S_FECERRB	1
#define S_RDOERR	1
#define S_ALTROBUS	1
#define S_BUSSTTRSF	1
#define S_RCUBUSBUSY	1
#define S_MEBCNT	1
#define S_SWTTRGCNT	1
#define S_AUXTRGCNT	1
#define S_TTCL2ACNT	1
#define S_TTCL2RCNT	1
#define S_DSTBACNT    	1
#define S_DSTBBCNT    	1
#define S_TRSFACNT    	1
#define S_TRSFBCNT     	1
#define S_ACKACNT      	1
#define S_ACKBCNT      	1
#define S_CSTBACNT    	1
#define S_CSTBBCNT    	1
#define S_DSTBNUMA 	1
#define S_DSTBNUMB 	1
#define S_ADDRMISMATCH	1
#define S_BLENMISMATCH	1
#define S_ABDFSM	1
#define S_RDOFSM	1
#define S_ISFSM		1
#define S_EVTMANFSM	1

/* RCU CORE Registers Masks */
#define M_REGISTERS	0xffffffff
#define M_ACTFECLIST    0xffffffff
#define M_ALTROIF	0x3ffff
#define M_TRGCONF	0x1ffff
#define M_RDOMOD	0xf
#define M_ALTROCFG1	0xfffff
#define M_ALTROCFG2	0x7f
#define M_RCU_VERSION	0xffffff
#define M_BP_VERSION	0x1
#define M_RCU_ID	0x1ff
#define M_FECERRA       0xffffffff
#define M_FECERRB	0xffffffff
#define M_RDOERR	0xfff001ff
#define M_ALTROBUS	0xff
#define M_BUSSTTRSF	0xff
#define M_RCUBUSBUSY	0x3
#define M_MEBCNT	0xf
#define M_SWTTRGCNT	0xfffff
#define M_AUXTRGCNT	0xfffff
#define M_TTCL2ACNT	0xfffff
#define M_TTCL2RCNT	0xfffff
#define M_DSTBACNT  	0xfffff
#define M_DSTBBCNT   	0xfffff
#define M_TRSFACNT   	0xfff
#define M_TRSFBCNT   	0xfff
#define M_ACKACNT    	0xfff
#define M_ACKBCNT     	0xfff
#define M_CSTBACNT  	0xfff
#define M_CSTBBCNT   	0xfff 
#define M_DSTBNUMA      0xfff
#define M_DSTBNUMB      0xfff
#define M_ADDRMISMATCH	0xfff
#define M_BLENMISMATCH	0xfff
#define M_ABDFSM	0x3fffff
#define M_RDOFSM	0x7fff
#define M_ISFSM		0x7fff
#define M_EVTMANFSM	0xfff


/* RCU CORE Commands */
#define A_CONFEC	0x5303
#define A_EXECSEQ	0x5304
#define A_ABORTSEQ	0x5305
#define A_SWTTRG	0x5306
#define A_CLRREGS	0x5307
#define A_CLRRDRX	0x5308
#define A_CLRTRGCNT	0x5309
#define A_ARBITERIRQ	0x5310

/* SIF Registers (DDL FESTRD)*/
/* Read-only */
#define A_SIFSTATUS	0x0
#define A_SIFERRORS	0x10000
/* Read&Write */
#define A_SIFBLREG	0x20000

/* SIF Registers (DDL FESTRD)*/
#define S_SIFSTATUS	1
#define S_SIFERRORS	1
#define S_SIFBLREG	1

/* SIF Registers (DDL FESTRD)*/	 
#define M_SIFSTATUS	0x73f0		/* to be checked with Csaba */
#define M_SIFERRORS	0x3f		/* to be checked with Csaba */
#define M_SIFBLREG	0xffff		/* to be checked with Csaba */

/* SIF Commands (DDL FECTRL)*/
#define A_SIFRESET	0x00001
#define A_RESETALL	0x00002
#define A_RCURESET	0x00004
#define A_FECRESET	0x00008
#define A_CLEARSIB	0x00010
#define A_CLEARSOB	0x00020
#define A_CLEARERR	0x10000
#define A_SETBLREG	0x20000

/* RCU TTC_Receiver Registers Addresses */
/* Read&Write */
#define A_CONTROL		0x4000
#define A_ROI_CONFIG1		0x4002
#define A_ROI_CONFIG2		0x4003
#define A_L1_LATENCY		0x4006
#define A_L2_LATENCY		0x4007
#define A_ROI_LATENCY		0x4009
#define A_L1_MSG_LATENCY	0x400a
/* Read-only */
#define A_PRE_PULSE_CNT		0x400b
#define A_BCID_LOCAL		0x400c
#define A_L0_COUNTER		0x400d
#define A_L1_COUNTER		0x400e
#define A_L1_MSG_COUNTER	0x400f
#define A_L2A_COUNTER		0x4010
#define A_L2R_COUNTER		0x4011
#define A_ROI_COUNTER		0x4012
#define A_BUNCH_COUNTER		0x4013
#define A_HAMNG_ERR_CNT		0x4016
#define A_ERROR_CNT		0x4017
#define A_BUFFERED_EVENTS	0x4020
#define A_DAQ_HEADER		0x4021
#define A_EVENT_INFO		0x4028
#define A_EVENT_ERROR		0x4029
#define A_L1_MSG_HEADER		0x4030
#define A_L1_MSG_DATA		0x4031
#define A_L2A_MSG_HEADER 	0x4035
#define A_L2A_MSG_DATA		0x4036
#define A_L2R_MSG_HEADER	0x403d
#define A_ROI_MSG_HEADER	0x403e
#define A_ROI_MSG_DATA		0x403f
#define A_FIFO_DAQ_HEADER	0x4081

/* RCU TTC_Receiver Registers Sizes */
#define S_CONTROL		1
#define S_ROI_CONFIG1		1
#define S_ROI_CONFIG2		1
#define S_L1_LATENCY		1
#define S_L2_LATENCY		1
#define S_ROI_LATENCY		1
#define S_L1_MSG_LATENCY	1
#define S_PRE_PULSE_CNT		1
#define S_BCID_LOCAL		1
#define S_L0_COUNTER		1
#define S_L1_COUNTER		1
#define S_L1_MSG_COUNTER	1
#define S_L2A_COUNTER		1
#define S_L2R_COUNTER		1
#define S_ROI_COUNTER		1
#define S_BUNCH_COUNTER		1
#define S_HAMNG_ERR_CNT		1
#define S_ERROR_CNT		1
#define S_BUFFERED_EVENTS	1
#define S_DAQ_HEADER		7
#define S_EVENT_INFO		1
#define S_EVENT_ERROR		1
#define S_L1_MSG_HEADER		1
#define S_L1_MSG_DATA		4
#define S_L2A_MSG_HEADER 	1
#define S_L2A_MSG_DATA		7
#define S_L2R_MSG_HEADER	1
#define S_ROI_MSG_HEADER	1
#define S_ROI_MSG_DATA		3
#define S_FIFO_DAQ_HEADER	1

/* RCU TTC_Receiver Registers Masks */
#define M_CONTROL		0xffffff
#define M_ROI_CONFIG1		0x3ffff
#define M_ROI_CONFIG2		0x3ffff
#define M_L1_LATENCY		0xffff
#define M_L2_LATENCY		0xffffffff	
#define M_ROI_LATENCY		0xffffffff	
#define M_L1_MSG_LATENCY	0xffffffff	
#define M_PRE_PULSE_CNT		0xffff
#define M_BCID_LOCAL		0xfff
#define M_L0_COUNTER		0xffff
#define M_L1_COUNTER		0xffff
#define M_L1_MSG_COUNTER	0xffff
#define M_L2A_COUNTER		0xffff
#define M_L2R_COUNTER		0xffff
#define M_ROI_COUNTER		0xffff
#define M_BUNCH_COUNTER		0xfff
#define M_HAMNG_ERR_CNT		0xffffffff
#define M_ERROR_CNT		0xffffffff
#define M_BUFFERED_EVENTS	0x1f
#define M_DAQ_HEADER		0xffffffff
#define M_EVENT_INFO		0xfff
#define M_EVENT_ERROR		0xfffffff
#define M_L1_MSG_HEADER		0xfff
#define M_L1_MSG_DATA		0xfff
#define M_L2A_MSG_HEADER	0xfff
#define M_L2A_MSG_DATA		0xfff
#define M_L2R_MSG_HEADER	0xfff
#define M_ROI_MSG_HEADER	0xfff
#define M_ROI_MSG_DATA		0xfff
#define M_FIFO_DAQ_HEADER	0xffffffff

/* RCU TTC Receiver Commands */
#define A_TMODULE_RESET		0x4001
#define A_TRESET_COUNTERS	0x4004
#define A_TISSUE_TEST_MODE	0x4005
#define A_TFIFO_READ_EN		0x4080

/* RCU MSM REGISTERS ADDRESSES */
#define A_MSM_RESULT          	0x8002
#define A_MSM_ERROR            	0x8003
#define A_MSM_INTMODE  		0x8004
#define A_MSM_SCADD           	0x8005
#define A_MSM_SCDATA         	0x8006

/* RCU MSM REGISTERS MASKS */
#define M_MSM_RESULT         	0x1fffff
#define M_MSM_ERROR          	0x3
#define M_MSM_INTMODE 		0x3
#define M_MSM_SCADD          	0xffff
#define M_MSM_SCDATA        	0xffff

/* RCU MSM REGISTERS SIZES */
#define S_MSM_RESULT         	1
#define S_MSM_ERROR          	1
#define S_MSM_INTMODEclear  		1
#define S_MSM_SCADD          	1r
#define S_MSM_SCDATA        	1

/* RCU MSM COMMANDS */
#define A_MSM_SCCMD1		0x8010
#define A_MSM_ERROR_RST		0x8011
#define A_MSM_RESULT_RST	0x8012
#define A_MSM_SCCMD2		0xC000

/* IS Instructions */
#define FEC_RD		0x0
#define FEC_WR		0x2
#define FEC_CMD		0x1
#define IS_CMD		0x3

/* IS Internal Codes */
#define LOOP		0x10000
#define WAIT		0x20000
#define ENDSEQ		0x80000
#define ENDMEM		0xF0000

/* ABDFSM STATES */

/*********/
/* Types */
/*********/
typedef struct 
{
  u_int id;    /* The bank number */
  u_int size;  /* The size of the bank */
  u_int mask;  /* The width of the bank */	
  char *name;  /* The bank name */
} rcu_bank_t;

typedef struct 
{
  u_int revision;  /* The rorc device revision number */
  u_int serial;    /* The rorc device serial number */
  u_int channel;   /* The rorc device channel number */	
} rorc_node_t;

typedef struct
{
  u_int nsamples_ev;	/* legal values are 0x0 -> 0x3ff */
  u_int clk_ratio;	/* legal values are -------------*/
  u_int cstb_delay;	/* legal values are 0, 1, 2, 3	*/
  u_int safe_mode;	/* 00 do not check; 01 check for TPC; 10 check for PHOS; 11 check for FMD */
} altroif_t;

typedef struct
{
  u_int l2_latency;	/* legal values are 0 -> 8191 */
  u_int trg_mode;	/* legal values are 0 and 1 */
  u_int trg_source;	/* legal values are 0, 1, 2, 3, 4 */
} trgconf_t;

typedef struct 
{
  u_int meb_mode;	/* legal values are 0 and 1 */
  u_int exec_seq;  	/* legal values are 0 and 1 */
  u_int sparse_rdo;  	/* legal values are 0 and 1 */
  u_int check_rdyrx;  	/* legal values are 0 and 1 */
} rdomod_t;

typedef struct
{
  u_int bc1_cfg;  	/* legal values are 0 -> 31 */
  u_int bc2_cfg;   	/* legal values are 0 -> 127 */
  u_int zs_cfg;  	/* legal values are 0 -> 255 */
} altrocfg1_t;

typedef struct
{
  u_int ptrg;    	/* legal values are 0 -> 15 */
  u_int nbuf;    	/* legal values are 0 and 1 */
  u_int flt_en;  	/* legal values are 0 and 1 */
  u_int pwsv;    	/* legal values are 0 and 1 */
} altrocfg2_t;

typedef struct
{
  u_int year;		/* legal values are 0 -> 255 */
  u_int month;		/* legal values are 1 -> 12 */
  u_int day;		/* legal values are 1 -> 31 */
} rcu_version_t;

typedef struct
{
  u_int altro_error;	
/* write SM */
  u_int w_error_b_cstb;	
  u_int w_ack_b_cstb;	
  u_int w_error_b_ack;	
  u_int w_no_ack;	
  u_int w_error_w_ack;	
  u_int w_ack_not_rel;	
  u_int w_error_bcast;	
/* read SM */
  u_int r_error_b_cstb;	
  u_int r_error_b_ack;	
  u_int r_ack_b_cstb;	
  u_int r_error_w_ack;	/* to be checked */
  u_int r_no_ack;	
  u_int r_ack_not_rel;	
/* rdo FSM */
  u_int write_fsm_error;	
  u_int error_b_trsf;	
  u_int no_trsf;
  u_int error_w_trsf;
  u_int trsf_not_rel;
} fecerr_t;

typedef struct
{

  u_int evlen_rdo_error;
  u_int error_b_trsf;
  u_int no_trsf;
  u_int wrong_ndstb;
  u_int trsf_not_rel;
  u_int scevlen_error;
  u_int rd_rx_error;  
  u_int ch_add_mismatch;
  u_int blen_mismatch;
  u_int fsm_evlen;
  u_int fsm_wrA;
  u_int fsm_wrB;	
} rdoerr_t;

typedef struct
{
  u_int	cstb_A;
  u_int	ackn_A;
  u_int trsf_A;
  u_int cstb_B;
  u_int ackn_B;
  u_int trsf_B;
  u_int L1;
  u_int L2;
} altrobus_t;

typedef struct
{
  u_int side;
  u_int sector;
  u_int partition;
} rcuid_t;

typedef struct
{
  u_int IS_busy;
  u_int IS_fail;
} rcubus_busy_t;    

/* TTC Receiver registers*/
typedef struct 
{
  u_int sbc_on;	        /* serial B channel ON - 1 bit */
  u_int dis_err_mask;   /* disable error masking - 1 bit */
  u_int en_roi_dec;     /* enable RoI decoding - 1 bit */
  u_int l0_support;     /* L0 via TTC - 1 bit */
  u_int l2a_fifo_smask; /* L2a FIFO storage mask - 1 bit */
  u_int l2r_fifo_smask; /* L2r FIFO storage mask - 1 bit */
  u_int l2_tout_smask;  /* L2 timeout FIFO storage mask - 1 bit */
  u_int l1_msg_mask;    /* L1a message mask - 1 bit */
  u_int bunch_cnt_ovflw;/* Bunch Counter Overflow - 1 bit */
  u_int run_active;     /* RUN active (set by SOD and reset by EOD) - 1 bit */
  u_int busy;           /* receiving trigger sequence - 1 bit */
  u_int cdh_version;    /* 4 bits*/  
} control_t;

typedef struct
{
  u_int l0_l1_latency;   /* 12 bits */
  u_int accept_window;  /* +- N clock cycles around the L0-L1 latency - 4 bits */
} l1_latency_t;

typedef struct
{
  u_int max_latency;   /* 16 bits */
  u_int min_latency;   /* 16 bits */
} latency_t;

typedef struct
{
  u_int nr_sbit_err;   /* Number of singl-bit Hamming errors - 16 bits */
  u_int nr_dbit_err;  /* Number of double-bit Hamming errors - 16 bits */
} hamng_err_cnt_t;

typedef struct
{
  u_int nr_msgdec_err;    /* Number of message decoding errors - 16 bits */
  u_int nr_seqtmovr_err; /* Number of errors related to sequence and timeout - 16 bits*/
} error_cnt_t;

typedef struct
{
  u_int roi_en;           /* RoI enable - 1 bit */
  u_int roi_announced;    /* RoI announced - 1 bit */
  u_int roi_received;     /* RoI received - 1 bit */
  u_int within_roi;       /* Within RoI - 1 bit */
  u_int calib_swt_trg;    /* Calibration or Software Trigger type - 4 bits */
  u_int swt_event;        /* Software Trigger event - 1 bit */
  u_int cal_event;        /* Calibration Trigger Event - 1 bit */
  u_int ev_l2r_trg;       /* Event with L2 reject - 1 bit */
  u_int ev_l2a_trg;       /* Event with a L2a - 1 bit */
  u_int has_payload;      /* Include payload - 1 bit */
  u_int sclk_phase_trg;   /* Phase of Sampling Clock wrt trigger - 5 bits */  
} event_info_t;

typedef struct
{
  u_int serB_stop_error;  /* Serial B stop bit error */
  u_int	hmng_sbit;	 /* Hamming single-bit error (individ. addressed) */
  u_int	hmng_dbit;	 /* Hamming double-bit error (individ. addressed)*/
  u_int	hmng_sbit_bcast; /* Hamming single-bit error (broadcast) */
  u_int	hmng_dbit_bcast; /* Hamming double-bit error (broadcast)*/
  u_int	wrong_msg_addr;	 /* Wrong Message Address */
  u_int	incompl_l1_msg;	 /* Incomplete L1 message*/
  u_int	incompl_l2a_msg; /* Incomplete L2a message */
  u_int	incompl_roi_msg; /* Incomplete RoI message */
  u_int	ttcrx_addr_err;	 /* TTCrx Address error (not 0x0003) */
  u_int	spur_l0; 	 /* Spurious L0 */
  u_int	miss_l0;	 /* Missing L0 */
  u_int	spur_l1;	 /* Spurious L1 */
  u_int	boundary_l1;     /* L1 outside aceptance window */
  u_int	miss_l1;	 /* Missing L1 */
  u_int	l1_out_window;   /* L1 message outside legal timeslot */
  u_int	l1_msg_miss;	 /* L1 message missing or timeout */
  u_int	l2_out_window;	 /* L2 message outside legal timeslot */
  u_int	l2_msg_miss;	 /* L2 message missing or timeout */
  u_int	roi_out_window;	 /* RoI message outside legal timeslot */
  u_int	roi_msg_miss;	 /* RoI message missing or timeout */
  u_int	pre_pulse_err;	 /* Prepulse error (=0 possible future use) */
  u_int	l1_msg_error;	 /* L1 message content error */
  u_int	l2_msg_error;	 /* L2 message content error */
  u_int	roi_msg_error;	 /* RoI message content error */
} event_error_t;

/* Safety & Monitoring Module registers*/
typedef struct
{
  u_int err1;             /* to be checked - 1 bit */
  u_int err2;             /* to be checked - 1 bit */
} msm_error_t;

typedef struct
{
  u_int en_int_A;         /* Enable interrupt branch A - 1 bit */
  u_int en_int_B;         /* Enable interrupt branch B - 1 bit*/
} msm_intmode_t;

typedef struct
{
  u_int bc_reg_addr;     /* Board Controller Address - 8 bits */
  u_int fec_addr;	 /* FEC address - 5 bits */
  u_int bcast;		 /* Broadcast mode - 1 bit */
  u_int rnw;		 /* Read no Write - 1 bit */
} msm_scadd_t;

typedef struct
{
  u_int sib_not_empty;	 /* SIF Input Buffer not empty */
  u_int sob_not_empty;	 /* SIF Output Buffer not empty */
  u_int sib_al_full;	 /* SIF Input Buffer almost full */
  u_int sob_al_full;	 /* SIF Output Buffer almost full */
  u_int sif_bus_req;	 /* SIF has asserted a bus request (to be checked)*/ 
  u_int sif_bus_grant;	 /* SIF has got the bus grant (to be checked) */
  u_int sif_bwr_active;	 /* Block write ongoing */
  u_int sif_brd_active;	 /* Block read ongoing */ 
  u_int sif_rdo_active;	 /* readout ongoing */
} sif_status_t;

typedef struct
{
  u_int sif_wr_err;	 /* SIF block write error */
  u_int sif_rd_err;	 /* SIF block read error */
  u_int sif_rdo_err;	 /* SIF readout error */
  u_int ddl_err;	 /* DDL error */
  u_int sif_blen_err;	 /* SIF block length error */
  u_int sif_wr_tout;	 /* SIF write timeout */
  u_int sif_rd_tout;	 /* SIF block read timeout */
} sif_error_t;
  

/* MJ: arbitrary package identifer. Should be greater then 100 to avouid overlap with standard ATLAS identifiers */
#define P_ID_RCU 101

/* MJ: Declare the error codes: */
#define RCU_SUCCESS 0
#define RCU_NOTOPEN 1
#define RCU_RANGE   2
#define RCU_ODD_BYTES 3
#define RCU_FEC2RORC_FAIL 4
#define RCU_SIB_TIMEOUT 5
#define RCU_LIB_FAIL 6

/* MJ: Macro to check if the library has been opened */
#define RCUISOPEN {if(!rcu_is_open) \
                   { \
		      printf("\n RCU IS NOT OPEN\n"); \
		      return(RCU_NOTOPEN); \
		    } }

/* MJ - start */
#if (RCUDEBUG)
#define DEBUG_TEXT(my_package, level, text)\
  {\
    if ((my_package == packageId) || (traceLevel == 0))\
      if (traceLevel >= level)\
      {\
        printf("\nDebug(%d):\n", my_package); \
	printf text; \
	printf("\n"); \
      }\
  }
#else
  #define DEBUG_TEXT(my_package, level, text)
#endif
/* MJ - end */



/***********************************/ 
/*Official functions of the library*/
/***********************************/ 
#ifdef __cplusplus
extern "C" {
#endif

int RCU_Open(rorc_node_t *node);
int RCU_Close(void);
int RCU_SIB_Empty_Wait (void);   
int RCU_Memory_Read (rcu_bank_t *membank, u_int offset, u_int nwords, u_long *memdata);
int RCU_IMEM_Read (u_int offset, u_int nwords, u_long *memdata);

#ifdef __cplusplus
}
#endif
