/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: xia_slit.i */

/* Event flags */
#define s_ainp_mon	1
#define hID_mon	2
#define vID_mon	3
#define hOrient_mon	4
#define vOrient_mon	5
#define stop_mon	6
#define t_event_mon	7
#define l_event_mon	8
#define b_event_mon	9
#define r_event_mon	10
#define h0_event_mon	11
#define v0_event_mon	12
#define width_event_mon	13
#define height_event_mon	14
#define start_H_move	15
#define start_V_move	16
#define wait_H_input	17
#define wait_V_input	18
#define new_H_RB	19
#define new_V_RB	20
#define move_h	21
#define move_v	22

/* Program "xia_slit" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 113
#define NUM_EVENTS 22
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT FALSE
#define DEBUG_OPT TRUE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram xia_slit;

/* Variable declarations */
struct UserVar {
	char	s_ainp[MAX_STRING_SIZE];
	char	s_tinp[MAX_STRING_SIZE];
	char	s_aout[MAX_STRING_SIZE];
	char	s_ieos[MAX_STRING_SIZE];
	char	s_oeos[MAX_STRING_SIZE];
	double	s_tmot;
	short	s_ofmt;
	short	s_ifmt;
	short	s_port;
	short	s_tmod;
	short	s_scan;
	short	s_nord;
	short	s_nrrd;
	short	s_proc;
	int	s_sevr;
	int	s_optioniv;
	short	s_baud;
	short	s_dbit;
	short	s_sbit;
	short	s_prty;
	short	s_mctl;
	short	s_fctl;
	int	debug_flag;
	char	port[MAX_STRING_SIZE];
	char	hID[MAX_STRING_SIZE];
	char	vID[MAX_STRING_SIZE];
	short	hOrient;
	short	vOrient;
	short	hBusy;
	short	h_Busy;
	short	h_BusyOff;
	short	h_BusyDisable;
	short	vBusy;
	short	v_Busy;
	short	v_BusyOff;
	short	v_BusyDisable;
	short	stop;
	short	locate;
	short	init;
	short	enable;
	short	calibrate;
	int	hOuterLimit;
	int	vOuterLimit;
	int	hOrigin;
	int	vOrigin;
	int	hStepDelay;
	int	vStepDelay;
	int	hGearBacklash;
	int	vGearBacklash;
	int	hStPwr;
	int	hStLmts;
	int	hStBan;
	int	hStEcho;
	int	hStLock;
	int	hStAlias;
	int	hStText;
	int	vStPwr;
	int	vStLmts;
	int	vStBan;
	int	vStEcho;
	int	vStLock;
	int	vStAlias;
	int	vStText;
	double	t;
	double	t_event;
	double	t_hi;
	double	t_lo;
	double	t_RB;
	double	load_t;
	double	l;
	double	l_event;
	double	l_hi;
	double	l_lo;
	double	l_RB;
	double	load_l;
	double	b;
	double	b_event;
	double	b_hi;
	double	b_lo;
	double	b_RB;
	double	load_b;
	double	r;
	double	r_event;
	double	r_hi;
	double	r_lo;
	double	r_RB;
	double	load_r;
	double	h0;
	double	h0_event;
	double	h0_hi;
	double	h0_lo;
	double	h0_RB;
	double	load_h0;
	double	v0;
	double	v0_event;
	double	v0_hi;
	double	v0_lo;
	double	v0_RB;
	double	load_v0;
	double	width;
	double	width_event;
	double	width_hi;
	double	width_lo;
	double	width_RB;
	double	load_width;
	double	height;
	double	height_event;
	double	height_hi;
	double	height_lo;
	double	height_RB;
	double	load_height;
	int	error;
	char	errMsg[MAX_STRING_SIZE];
	char	myChar;
	char	*SNLtaskName;
	double	dScratch;
	long	numWords;
	char	word1[MAX_STRING_SIZE];
	char	word2[MAX_STRING_SIZE];
	char	word3[MAX_STRING_SIZE];
	char	word4[MAX_STRING_SIZE];
	char	word5[MAX_STRING_SIZE];
	char	word6[MAX_STRING_SIZE];
	int	hCSW;
	int	vCSW;
	int	h_isMoving;
	int	v_isMoving;
	long	pos_a;
	long	pos_b;
	double	pos_ag;
	double	pos_bg;
	long	l_raw;
	long	r_raw;
	long	t_raw;
	long	b_raw;
	char	id[MAX_STRING_SIZE];
	short	hasID;
	char	hscErrors[14][MAX_STRING_SIZE];
	double	t_old;
	double	l_old;
	double	b_old;
	double	r_old;
	double	width_old;
	double	height_old;
	double	h0_old;
	double	v0_old;
	int	i;
	int	delta;
	int	init_delay;
	int	ainp_fifo_write;
	int	ainp_fifo_read;
	char	ainp_fifo[40][MAX_STRING_SIZE];
	char	scratch[MAX_STRING_SIZE];
	int	hsc_timeout;
	char	hsc_command[MAX_STRING_SIZE];
	char	hsc_response[MAX_STRING_SIZE];
};

/* C code definitions */
# line 89 "../xia_slit.st"
#include <stdio.h>
# line 90 "../xia_slit.st"
#include <math.h>
# line 91 "../xia_slit.st"
#include <string.h>
# line 92 "../xia_slit.st"
 #include <epicsThread.h>

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "startup" in state set "xiahsc" */

/* Delay function for state "startup" in state set "xiahsc" */
static void D_xiahsc_startup(SS_ID ssId, struct UserVar *pVar)
{
# line 616 "../xia_slit.st"
}

/* Event function for state "startup" in state set "xiahsc" */
static long E_xiahsc_startup(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 616 "../xia_slit.st"
	if (TRUE)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "startup" in state set "xiahsc" */
static void A_xiahsc_startup(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 571 "../xia_slit.st"
			seq_pvGet(ssId, 0 /* s_ainp */, 0);
			seq_pvGet(ssId, 2 /* s_aout */, 0);
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 38 /* init */, 2);
			}
# line 573 "../xia_slit.st"
			;
			(pVar->init_delay) = 1.0;
# line 579 "../xia_slit.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 23 /* port */, 0);
# line 582 "../xia_slit.st"
			strcpy((pVar->hscErrors)[0], "Missing Command");
			strcpy((pVar->hscErrors)[1], "Unrecognized Command");
			strcpy((pVar->hscErrors)[2], "Input Buffer Overflow");
			strcpy((pVar->hscErrors)[3], "No new Alias Given");
			strcpy((pVar->hscErrors)[4], "Alias too long");
			strcpy((pVar->hscErrors)[5], "Invalid Field Parameter");
			strcpy((pVar->hscErrors)[6], "Value Out of Range");
			strcpy((pVar->hscErrors)[7], "Parameter is read-only");
			strcpy((pVar->hscErrors)[8], "Invalid/Missing Argument");
			strcpy((pVar->hscErrors)[9], "No Movement Required");
			strcpy((pVar->hscErrors)[10], "Uncalibrated: no motion allowed");
			strcpy((pVar->hscErrors)[11], "Motion out of range");
			strcpy((pVar->hscErrors)[12], "Invalid/missing direction character");
			strcpy((pVar->hscErrors)[13], "Invalid Motor Specified");
# line 597 "../xia_slit.st"
			seq_pvGet(ssId, 24 /* hID */, 2);
			seq_pvGet(ssId, 25 /* vID */, 2);
# line 604 "../xia_slit.st"
			(pVar->t_old) = (pVar->t);
# line 604 "../xia_slit.st"
			(pVar->b_old) = (pVar->b);
			(pVar->l_old) = (pVar->l);
# line 605 "../xia_slit.st"
			(pVar->r_old) = (pVar->r);
			(pVar->h0_old) = (pVar->h0);
# line 606 "../xia_slit.st"
			(pVar->width_old) = (pVar->width);
			(pVar->v0_old) = (pVar->v0);
# line 607 "../xia_slit.st"
			(pVar->height_old) = (pVar->height);
# line 610 "../xia_slit.st"
			(pVar->hOrigin) = (pVar->vOrigin) = 400;
			(pVar->hOuterLimit) = (pVar->vOuterLimit) = 4400;
# line 613 "../xia_slit.st"
			{
				(pVar->enable) = (1);
				seq_pvPut(ssId, 39 /* enable */, 2);
			}
# line 613 "../xia_slit.st"
			;
			{
				(pVar->s_tmot) = (.250);
				seq_pvPut(ssId, 5 /* s_tmot */, 2);
			}
# line 614 "../xia_slit.st"
			;
		}
		return;
	}
}
/* Code for state "disable" in state set "xiahsc" */

/* Delay function for state "disable" in state set "xiahsc" */
static void D_xiahsc_disable(SS_ID ssId, struct UserVar *pVar)
{
# line 625 "../xia_slit.st"
}

/* Event function for state "disable" in state set "xiahsc" */
static long E_xiahsc_disable(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 625 "../xia_slit.st"
	if ((pVar->enable))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "disable" in state set "xiahsc" */
static void A_xiahsc_disable(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 623 "../xia_slit.st"
			(pVar->init_delay) = 1.0;
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 38 /* init */, 2);
			}
# line 624 "../xia_slit.st"
			;
		}
		return;
	}
}
/* Code for state "comm_error" in state set "xiahsc" */

/* Delay function for state "comm_error" in state set "xiahsc" */
static void D_xiahsc_comm_error(SS_ID ssId, struct UserVar *pVar)
{
# line 637 "../xia_slit.st"
	seq_delayInit(ssId, 0, ((10 * 60.0)));
}

/* Event function for state "comm_error" in state set "xiahsc" */
static long E_xiahsc_comm_error(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 637 "../xia_slit.st"
	if ((pVar->init) || !(pVar->s_sevr) || seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "comm_error" in state set "xiahsc" */
static void A_xiahsc_comm_error(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 633 "../xia_slit.st"
			if ((pVar->debug_flag) >= 20)
			{
				printf("(%s,%d): ", "../xia_slit.st", 633);
				printf("comm_error: init=%d  s_sevr=%d", (pVar->init), (pVar->s_sevr));
				printf("\n");
			}
# line 634 "../xia_slit.st"
			(pVar->init_delay) = 1.0;
			(pVar->s_sevr) = 0;
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 38 /* init */, 2);
			}
# line 636 "../xia_slit.st"
			;
		}
		return;
	}
}
/* Code for state "init" in state set "xiahsc" */

/* Delay function for state "init" in state set "xiahsc" */
static void D_xiahsc_init(SS_ID ssId, struct UserVar *pVar)
{
# line 642 "../xia_slit.st"
# line 659 "../xia_slit.st"
# line 708 "../xia_slit.st"
	seq_delayInit(ssId, 0, ((pVar->init_delay)));
# line 770 "../xia_slit.st"
	seq_delayInit(ssId, 1, (0.1));
}

/* Event function for state "init" in state set "xiahsc" */
static long E_xiahsc_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 642 "../xia_slit.st"
	if (!(pVar->enable))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 659 "../xia_slit.st"
	if ((seq_efTest(ssId, hID_mon) || seq_efTest(ssId, vID_mon)))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 708 "../xia_slit.st"
	if ((pVar->init) && seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 2;
		return TRUE;
	}
# line 770 "../xia_slit.st"
	if (!(pVar->init) && seq_delay(ssId, 1))
	{
		*pNextState = 4;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "xiahsc" */
static void A_xiahsc_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
# line 649 "../xia_slit.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 23 /* port */, 0);
# line 654 "../xia_slit.st"
			(pVar->init_delay) = 0;
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
			seq_efClear(ssId, hOrient_mon);
			seq_efClear(ssId, vOrient_mon);
		}
		return;
	case 2:
		{
# line 664 "../xia_slit.st"
			(pVar->error) = 0;
# line 668 "../xia_slit.st"
			if (!strcmp((pVar->hID), (pVar->vID)))
			{
				{
					strcpy((pVar->errMsg), "H & V IDs must be different");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
				(pVar->error)++;
			}
# line 677 "../xia_slit.st"
			if (sscanf((pVar->hID), "XIAHSC-%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
			{
				if (sscanf((pVar->hID), "%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
				{
					if (sscanf((pVar->hID), "%c%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
					{
						{
							strcpy((pVar->errMsg), "H ID not a valid HSC ID");
							seq_pvPut(ssId, 112 /* errMsg */, 2);
							epicsThreadSleep(0.01);
						}
						;
						(pVar->error)++;
					}
				}
			}
# line 685 "../xia_slit.st"
			if (sscanf((pVar->vID), "XIAHSC-%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
			{
				if (sscanf((pVar->vID), "%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
				{
					if (sscanf((pVar->vID), "%c%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
					{
						{
							strcpy((pVar->errMsg), "V ID not a valid HSC ID");
							seq_pvPut(ssId, 112 /* errMsg */, 2);
							epicsThreadSleep(0.01);
						}
						;
						(pVar->error)++;
					}
				}
			}
# line 689 "../xia_slit.st"
			if (!strlen((pVar->hID)))
			{
				{
					strcpy((pVar->errMsg), "define H ID string");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
				(pVar->error)++;
			}
# line 693 "../xia_slit.st"
			if (!strlen((pVar->vID)))
			{
				{
					strcpy((pVar->errMsg), "define V ID string");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
				(pVar->error)++;
			}
# line 699 "../xia_slit.st"
			if ((pVar->error))
			{
				{
					(pVar->error) = ((17));
					seq_pvPut(ssId, 111 /* error */, 2);
				}
				;
			}
# line 702 "../xia_slit.st"
			else
			{
				{
					(pVar->init) = (0);
					seq_pvPut(ssId, 38 /* init */, 2);
				}
				;
				{
					strcpy((pVar->errMsg), "no error");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 703 "../xia_slit.st"
			seq_pvPut(ssId, 111 /* error */, 0);
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
# line 707 "../xia_slit.st"
			(pVar->init_delay) = 30;
		}
		return;
	case 3:
		{
# line 710 "../xia_slit.st"
			{
				(pVar->hBusy) = ((0));
				seq_pvPut(ssId, 28 /* hBusy */, 2);
			}
# line 710 "../xia_slit.st"
			;
			{
				(pVar->vBusy) = ((0));
				seq_pvPut(ssId, 32 /* vBusy */, 2);
			}
# line 711 "../xia_slit.st"
			;
			{
				(pVar->error) = ((0));
				seq_pvPut(ssId, 111 /* error */, 2);
			}
# line 712 "../xia_slit.st"
			;
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 713 "../xia_slit.st"
			;
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 714 "../xia_slit.st"
			;
# line 718 "../xia_slit.st"
			{
				seq_pvGet(ssId, 15 /* s_optioniv */, 0);
				if ((pVar->s_optioniv) == (1))
				{
					{
						(pVar->s_baud) = ((6));
						seq_pvPut(ssId, 16 /* s_baud */, 2);
					}
					;
					{
						(pVar->s_dbit) = ((4));
						seq_pvPut(ssId, 17 /* s_dbit */, 2);
					}
					;
					{
						(pVar->s_sbit) = ((1));
						seq_pvPut(ssId, 18 /* s_sbit */, 2);
					}
					;
					{
						(pVar->s_prty) = ((1));
						seq_pvPut(ssId, 19 /* s_prty */, 2);
					}
					;
					{
						(pVar->s_fctl) = ((1));
						seq_pvPut(ssId, 21 /* s_fctl */, 2);
					}
					;
				}
			}
# line 718 "../xia_slit.st"
			;
# line 722 "../xia_slit.st"
			{
				strcpy((pVar->s_ieos), ("\r\n"));
				seq_pvPut(ssId, 3 /* s_ieos */, 2);
				epicsThreadSleep(0.01);
			}
# line 722 "../xia_slit.st"
			;
			{
				(pVar->s_ifmt) = (0);
				seq_pvPut(ssId, 7 /* s_ifmt */, 2);
			}
# line 723 "../xia_slit.st"
			;
			{
				(pVar->s_nrrd) = (0);
				seq_pvPut(ssId, 12 /* s_nrrd */, 2);
			}
# line 724 "../xia_slit.st"
			;
			{
				strcpy((pVar->s_oeos), ("\r"));
				seq_pvPut(ssId, 4 /* s_oeos */, 2);
				epicsThreadSleep(0.01);
			}
# line 725 "../xia_slit.st"
			;
			{
				(pVar->s_ofmt) = (0);
				seq_pvPut(ssId, 6 /* s_ofmt */, 2);
			}
# line 726 "../xia_slit.st"
			;
			{
				(pVar->s_tmod) = (0);
				seq_pvPut(ssId, 9 /* s_tmod */, 2);
			}
# line 727 "../xia_slit.st"
			;
			{
				(pVar->s_scan) = ((0));
				seq_pvPut(ssId, 10 /* s_scan */, 2);
			}
# line 728 "../xia_slit.st"
			;
			{
				(pVar->s_tmot) = (.250);
				seq_pvPut(ssId, 5 /* s_tmot */, 2);
			}
# line 729 "../xia_slit.st"
			;
# line 731 "../xia_slit.st"
			seq_efClear(ssId, s_ainp_mon);
# line 733 "../xia_slit.st"
			seq_efClear(ssId, wait_H_input);
			seq_efClear(ssId, wait_V_input);
# line 736 "../xia_slit.st"
			seq_efClear(ssId, move_h);
			seq_efClear(ssId, move_v);
			seq_efClear(ssId, new_H_RB);
			seq_efClear(ssId, new_V_RB);
# line 741 "../xia_slit.st"
			{
				(pVar->stop) = ((0));
				seq_pvPut(ssId, 36 /* stop */, 2);
			}
# line 741 "../xia_slit.st"
			;
			epicsThreadSleep(.02);
			seq_efClear(ssId, stop_mon);
# line 745 "../xia_slit.st"
			seq_efClear(ssId, t_event_mon);
# line 745 "../xia_slit.st"
			seq_efClear(ssId, l_event_mon);
			seq_efClear(ssId, b_event_mon);
# line 746 "../xia_slit.st"
			seq_efClear(ssId, r_event_mon);
			seq_efClear(ssId, h0_event_mon);
# line 747 "../xia_slit.st"
			seq_efClear(ssId, width_event_mon);
			seq_efClear(ssId, v0_event_mon);
# line 748 "../xia_slit.st"
			seq_efClear(ssId, height_event_mon);
			{
				(pVar->calibrate) = ((0));
				seq_pvPut(ssId, 40 /* calibrate */, 2);
			}
# line 749 "../xia_slit.st"
			;
# line 751 "../xia_slit.st"
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
			seq_efClear(ssId, hOrient_mon);
			seq_efClear(ssId, vOrient_mon);
# line 760 "../xia_slit.st"
			{
				sprintf((pVar->hsc_command), "!%s %s", "ALL", ("K"));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 760, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 760 "../xia_slit.st"
			;
			epicsThreadSleep(.1);
			{
				sprintf((pVar->hsc_command), "!%s %s", "ALL", ("P"));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 762, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 762 "../xia_slit.st"
			;
			epicsThreadSleep(.1);
			(pVar->ainp_fifo_read) = 0;
			(pVar->ainp_fifo_write) = 0;
# line 769 "../xia_slit.st"
			while (!(pVar->s_sevr))
			{
				{
					{
						(pVar->s_tmod) = ((2));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					seq_efClear(ssId, s_ainp_mon);
					{
						(pVar->s_proc) = (1);
						seq_pvPut(ssId, 13 /* s_proc */, 2);
					}
					;
					for ((pVar->i) = 0; (pVar->i) <= (30); (pVar->i)++)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 767, (pVar->SNLtaskName), 10);
								;
								printf("%s\n", "READ_HSC: got a message.");
								epicsThreadSleep(0.01);
							}
							;
							break;
						}
						epicsThreadSleep(0.02);
					}
				}
				;
				{
					if (seq_efTest(ssId, s_ainp_mon))
					{
						strcpy((pVar->ainp_fifo)[(pVar->ainp_fifo_read)++], (pVar->s_ainp));
						{
							if (((pVar->ainp_fifo_read)) >= 40)
								((pVar->ainp_fifo_read)) = 0;
						}
						;
						(pVar->delta) = (pVar->ainp_fifo_read) - (pVar->ainp_fifo_write);
						if ((pVar->delta) < 0)
							(pVar->delta) += 40;
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 768);
							printf("FIFO_IN: adding msg %d:\n ->'%s'", (pVar->delta), (pVar->s_ainp));
							printf("\n");
						}
						;
					}
				}
				;
			}
		}
		return;
	}
}
/* Code for state "init_limits" in state set "xiahsc" */

/* Delay function for state "init_limits" in state set "xiahsc" */
static void D_xiahsc_init_limits(SS_ID ssId, struct UserVar *pVar)
{
# line 872 "../xia_slit.st"
}

/* Event function for state "init_limits" in state set "xiahsc" */
static long E_xiahsc_init_limits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 872 "../xia_slit.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init_limits" in state set "xiahsc" */
static void A_xiahsc_init_limits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 781 "../xia_slit.st"
			(pVar->error) = 0;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("R 5"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 782);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hStepDelay)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 782);
						printf("%s step delay %d", (pVar->hID), (pVar->hStepDelay));
						printf("\n");
					}
					;
					seq_pvPut(ssId, 45 /* hStepDelay */, 2);
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, (pVar->SNLtaskName), 1);
						;
						printf("%s step delay format error", (pVar->hID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 782 "../xia_slit.st"
			;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("R 6"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 783);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hGearBacklash)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 783);
						printf("%s backlash %d", (pVar->hID), (pVar->hGearBacklash));
						printf("\n");
					}
					;
					seq_pvPut(ssId, 47 /* hGearBacklash */, 2);
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, (pVar->SNLtaskName), 1);
						;
						printf("%s backlash format error", (pVar->hID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 783 "../xia_slit.st"
			;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("R 7"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 784);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hCSW)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 784);
						printf("%s cntrl word data %d", (pVar->hID), (pVar->hCSW));
						printf("\n");
					}
					;
					{
						(pVar->hStPwr) = (((0x03) & (pVar->hCSW)));
						seq_pvPut(ssId, 49 /* hStPwr */, 2);
					}
					;
					{
						(pVar->hStLmts) = ((((0x04) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 50 /* hStLmts */, 2);
					}
					;
					{
						(pVar->hStBan) = ((((0x08) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 51 /* hStBan */, 2);
					}
					;
					{
						(pVar->hStEcho) = ((((0x10) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 52 /* hStEcho */, 2);
					}
					;
					{
						(pVar->hStLock) = ((((0x20) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 53 /* hStLock */, 2);
					}
					;
					{
						(pVar->hStAlias) = ((((0x40) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 54 /* hStAlias */, 2);
					}
					;
					{
						(pVar->hStText) = ((((0x80) & (pVar->hCSW)) ? 1 : 0));
						seq_pvPut(ssId, 55 /* hStText */, 2);
					}
					;
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, (pVar->SNLtaskName), 1);
						;
						printf("%s cntrl word format error", (pVar->hID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 784 "../xia_slit.st"
			;
# line 786 "../xia_slit.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("R 1"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 786, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((0));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				{
					for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							(pVar->hsc_timeout) = 0;
							break;
						}
						epicsThreadSleep(0.02);
					}
					if ((pVar->hsc_timeout) == 1)
					{
						strcpy((pVar->hsc_response), "");
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 786, (pVar->SNLtaskName), 10);
							;
							printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
					}
					else
					{
						strcpy((pVar->hsc_response), (pVar->s_ainp));
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 786);
							printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
							printf("\n");
						}
						;
					}
				}
				;
			}
# line 786 "../xia_slit.st"
			;
# line 790 "../xia_slit.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hOuterLimit)) == 1)
			{
				seq_pvPut(ssId, 41 /* hOuterLimit */, 2);
			}
# line 795 "../xia_slit.st"
			else
			{
				(pVar->error)++;
				if ((pVar->debug_flag) >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 794, (pVar->SNLtaskName), 5);
					;
					printf("%s\n", "Could not read horizontal outer limit");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 796 "../xia_slit.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("R 2"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 796, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((0));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				{
					for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							(pVar->hsc_timeout) = 0;
							break;
						}
						epicsThreadSleep(0.02);
					}
					if ((pVar->hsc_timeout) == 1)
					{
						strcpy((pVar->hsc_response), "");
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 796, (pVar->SNLtaskName), 10);
							;
							printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
					}
					else
					{
						strcpy((pVar->hsc_response), (pVar->s_ainp));
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 796);
							printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
							printf("\n");
						}
						;
					}
				}
				;
			}
# line 796 "../xia_slit.st"
			;
# line 800 "../xia_slit.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hOrigin)) == 1)
			{
				seq_pvPut(ssId, 43 /* hOrigin */, 2);
			}
# line 805 "../xia_slit.st"
			else
			{
				(pVar->error)++;
				if ((pVar->debug_flag) >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 804, (pVar->SNLtaskName), 5);
					;
					printf("%s\n", "Could not read horizontal origin position");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 823 "../xia_slit.st"
			if (!(pVar->error))
			{
				(pVar->pos_ag) = (((0) - ((pVar->hOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->hOuterLimit)) - ((pVar->hOrigin))) / ((400.0)));
				{
					(pVar->l_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 72 /* l_lo */, 2);
				}
				;
				{
					(pVar->l_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 71 /* l_hi */, 2);
				}
				;
				{
					(pVar->r_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 84 /* r_lo */, 2);
				}
				;
				{
					(pVar->r_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 83 /* r_hi */, 2);
				}
				;
				{
					if (((pVar->l_lo)) > ((pVar->r_lo)))
					{
						((pVar->dScratch)) = ((pVar->l_lo));
					}
					else
					{
						((pVar->dScratch)) = ((pVar->r_lo));
					}
				}
				;
				{
					if (((pVar->dScratch)) > (0.0))
					{
						((pVar->width_lo)) = ((pVar->dScratch));
					}
					else
					{
						((pVar->width_lo)) = (0.0);
					}
				}
				;
				seq_pvPut(ssId, 102 /* width_lo */, 0);
				{
					(pVar->width_hi) = ((pVar->l_hi) + (pVar->r_hi));
					seq_pvPut(ssId, 101 /* width_hi */, 2);
				}
				;
				{
					(pVar->h0_hi) = (((pVar->r_hi) - (pVar->l_lo)) / 2);
					seq_pvPut(ssId, 89 /* h0_hi */, 2);
				}
				;
				{
					(pVar->h0_lo) = (((pVar->r_lo) - (pVar->l_hi)) / 2);
					seq_pvPut(ssId, 90 /* h0_lo */, 2);
				}
				;
			}
# line 825 "../xia_slit.st"
			(pVar->error) = 0;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("R 7"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 826);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vCSW)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 826);
						printf("%s cntrl word data %d", (pVar->vID), (pVar->vCSW));
						printf("\n");
					}
					;
					{
						(pVar->vStPwr) = (((0x03) & (pVar->vCSW)));
						seq_pvPut(ssId, 56 /* vStPwr */, 2);
					}
					;
					{
						(pVar->vStLmts) = ((((0x04) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 57 /* vStLmts */, 2);
					}
					;
					{
						(pVar->vStBan) = ((((0x08) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 58 /* vStBan */, 2);
					}
					;
					{
						(pVar->vStEcho) = ((((0x10) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 59 /* vStEcho */, 2);
					}
					;
					{
						(pVar->vStLock) = ((((0x20) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 60 /* vStLock */, 2);
					}
					;
					{
						(pVar->vStAlias) = ((((0x40) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 61 /* vStAlias */, 2);
					}
					;
					{
						(pVar->vStText) = ((((0x80) & (pVar->vCSW)) ? 1 : 0));
						seq_pvPut(ssId, 62 /* vStText */, 2);
					}
					;
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, (pVar->SNLtaskName), 1);
						;
						printf("%s cntrl word format error", (pVar->vID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 826 "../xia_slit.st"
			;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("R 6"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 827);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vGearBacklash)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 827);
						printf("%s backlash %d", (pVar->vID), (pVar->vGearBacklash));
						printf("\n");
					}
					;
					seq_pvPut(ssId, 48 /* vGearBacklash */, 2);
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, (pVar->SNLtaskName), 1);
						;
						printf("%s backlash format error", (pVar->vID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 827 "../xia_slit.st"
			;
			{
				{
					seq_efClear(ssId, s_ainp_mon);
					(pVar->hsc_timeout) = 1;
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("R 5"));
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, (pVar->SNLtaskName), 10);
							;
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
						{
							(pVar->s_scan) = ((0));
							seq_pvPut(ssId, 10 /* s_scan */, 2);
						}
						;
						{
							(pVar->s_tmod) = ((0));
							seq_pvPut(ssId, 9 /* s_tmod */, 2);
						}
						;
						{
							strcpy((pVar->s_ieos), ("\r\n"));
							seq_pvPut(ssId, 3 /* s_ieos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_oeos), ("\r"));
							seq_pvPut(ssId, 4 /* s_oeos */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 2 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
					}
					;
					{
						for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(0.02);
						}
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, (pVar->SNLtaskName), 10);
								;
								printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
								printf("\n");
								epicsThreadSleep(0.01);
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 828);
								printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
								printf("\n");
							}
							;
						}
					}
					;
				}
				;
				if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vStepDelay)) == 1)
				{
					if ((pVar->debug_flag) >= 10)
					{
						printf("(%s,%d): ", "../xia_slit.st", 828);
						printf("%s step delay %d", (pVar->vID), (pVar->vStepDelay));
						printf("\n");
					}
					;
					seq_pvPut(ssId, 46 /* vStepDelay */, 2);
				}
				else
				{
					++(pVar->error);
					if ((pVar->debug_flag) >= 1)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, (pVar->SNLtaskName), 1);
						;
						printf("%s step delay format error", (pVar->vID));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 828 "../xia_slit.st"
			;
# line 830 "../xia_slit.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("R 1"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 830, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((0));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				{
					for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							(pVar->hsc_timeout) = 0;
							break;
						}
						epicsThreadSleep(0.02);
					}
					if ((pVar->hsc_timeout) == 1)
					{
						strcpy((pVar->hsc_response), "");
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 830, (pVar->SNLtaskName), 10);
							;
							printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
					}
					else
					{
						strcpy((pVar->hsc_response), (pVar->s_ainp));
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 830);
							printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
							printf("\n");
						}
						;
					}
				}
				;
			}
# line 830 "../xia_slit.st"
			;
# line 834 "../xia_slit.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vOuterLimit)) == 1)
			{
				seq_pvPut(ssId, 42 /* vOuterLimit */, 2);
			}
# line 839 "../xia_slit.st"
			else
			{
				(pVar->error)++;
				if ((pVar->debug_flag) >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 838, (pVar->SNLtaskName), 5);
					;
					printf("%s\n", "Could not read vertical outer limit");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 841 "../xia_slit.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("R 2"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 841, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((0));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				{
					for ((pVar->i) = (30); (pVar->i); (pVar->i)--)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							(pVar->hsc_timeout) = 0;
							break;
						}
						epicsThreadSleep(0.02);
					}
					if ((pVar->hsc_timeout) == 1)
					{
						strcpy((pVar->hsc_response), "");
						if ((pVar->debug_flag) >= 10)
						{
							printf("<%s,%d,%s,%d> ", "../xia_slit.st", 841, (pVar->SNLtaskName), 10);
							;
							printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->s_tinp));
							printf("\n");
							epicsThreadSleep(0.01);
						}
						;
					}
					else
					{
						strcpy((pVar->hsc_response), (pVar->s_ainp));
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 841);
							printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->s_tinp));
							printf("\n");
						}
						;
					}
				}
				;
			}
# line 841 "../xia_slit.st"
			;
# line 845 "../xia_slit.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vOrigin)) == 1)
			{
				seq_pvPut(ssId, 44 /* vOrigin */, 2);
			}
# line 850 "../xia_slit.st"
			else
			{
				(pVar->error)++;
				if ((pVar->debug_flag) >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 849, (pVar->SNLtaskName), 5);
					;
					printf("%s\n", "Could not read vertical origin position");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 869 "../xia_slit.st"
			if (!(pVar->error))
			{
				(pVar->pos_ag) = (((0) - ((pVar->vOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->vOuterLimit)) - ((pVar->vOrigin))) / ((400.0)));
				{
					(pVar->t_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 66 /* t_lo */, 2);
				}
				;
				{
					(pVar->t_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 65 /* t_hi */, 2);
				}
				;
				{
					(pVar->b_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 78 /* b_lo */, 2);
				}
				;
				{
					(pVar->b_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 77 /* b_hi */, 2);
				}
				;
				{
					if (((pVar->t_lo)) > ((pVar->b_lo)))
					{
						((pVar->dScratch)) = ((pVar->t_lo));
					}
					else
					{
						((pVar->dScratch)) = ((pVar->b_lo));
					}
				}
				;
				{
					if (((pVar->dScratch)) > (0.0))
					{
						((pVar->height_lo)) = ((pVar->dScratch));
					}
					else
					{
						((pVar->height_lo)) = (0.0);
					}
				}
				;
				seq_pvPut(ssId, 108 /* height_lo */, 0);
				{
					(pVar->height_hi) = ((pVar->t_hi) + (pVar->b_hi));
					seq_pvPut(ssId, 107 /* height_hi */, 2);
				}
				;
				{
					(pVar->v0_hi) = (((pVar->t_hi) - (pVar->b_lo)) / 2);
					seq_pvPut(ssId, 95 /* v0_hi */, 2);
				}
				;
				{
					(pVar->v0_lo) = (((pVar->t_lo) - (pVar->b_hi)) / 2);
					seq_pvPut(ssId, 96 /* v0_lo */, 2);
				}
				;
			}
# line 870 "../xia_slit.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 37 /* locate */, 2);
			}
# line 870 "../xia_slit.st"
			;
		}
		return;
	}
}
/* Code for state "idle" in state set "xiahsc" */

/* Delay function for state "idle" in state set "xiahsc" */
static void D_xiahsc_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 882 "../xia_slit.st"
# line 889 "../xia_slit.st"
# line 898 "../xia_slit.st"
# line 905 "../xia_slit.st"
# line 912 "../xia_slit.st"
# line 949 "../xia_slit.st"
# line 981 "../xia_slit.st"
# line 1002 "../xia_slit.st"
# line 1014 "../xia_slit.st"
# line 1030 "../xia_slit.st"
# line 1049 "../xia_slit.st"
# line 1067 "../xia_slit.st"
# line 1075 "../xia_slit.st"
# line 1082 "../xia_slit.st"
	seq_delayInit(ssId, 0, ((0.2)));
# line 1086 "../xia_slit.st"
	seq_delayInit(ssId, 1, ((30)));
}

/* Event function for state "idle" in state set "xiahsc" */
static long E_xiahsc_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 882 "../xia_slit.st"
	if (!(pVar->enable))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 889 "../xia_slit.st"
	if ((pVar->init))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 898 "../xia_slit.st"
	if (seq_efTest(ssId, stop_mon) && (pVar->stop))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
# line 905 "../xia_slit.st"
	if (seq_efTest(ssId, l_event_mon))
	{
		*pNextState = 6;
		*pTransNum = 3;
		return TRUE;
	}
# line 912 "../xia_slit.st"
	if (seq_efTest(ssId, l_event_mon) || seq_efTest(ssId, r_event_mon) || seq_efTest(ssId, t_event_mon) || seq_efTest(ssId, b_event_mon) || seq_efTest(ssId, h0_event_mon) || seq_efTest(ssId, height_event_mon) || seq_efTest(ssId, v0_event_mon) || seq_efTest(ssId, width_event_mon))
	{
		*pNextState = 6;
		*pTransNum = 4;
		return TRUE;
	}
# line 949 "../xia_slit.st"
	if (seq_efTest(ssId, start_H_move))
	{
		*pNextState = 5;
		*pTransNum = 5;
		return TRUE;
	}
# line 981 "../xia_slit.st"
	if (seq_efTest(ssId, start_V_move))
	{
		*pNextState = 5;
		*pTransNum = 6;
		return TRUE;
	}
# line 1002 "../xia_slit.st"
	if ((pVar->locate))
	{
		*pNextState = 5;
		*pTransNum = 7;
		return TRUE;
	}
# line 1014 "../xia_slit.st"
	if ((pVar->calibrate))
	{
		*pNextState = 5;
		*pTransNum = 8;
		return TRUE;
	}
# line 1030 "../xia_slit.st"
	if ((pVar->ainp_fifo_read) != (pVar->ainp_fifo_write))
	{
		*pNextState = 7;
		*pTransNum = 9;
		return TRUE;
	}
# line 1049 "../xia_slit.st"
	if (seq_efTest(ssId, new_H_RB))
	{
		*pNextState = 5;
		*pTransNum = 10;
		return TRUE;
	}
# line 1067 "../xia_slit.st"
	if (seq_efTest(ssId, new_V_RB))
	{
		*pNextState = 5;
		*pTransNum = 11;
		return TRUE;
	}
# line 1075 "../xia_slit.st"
	if ((pVar->stop) && !seq_efTest(ssId, wait_H_input) && !seq_efTest(ssId, wait_V_input))
	{
		*pNextState = 5;
		*pTransNum = 12;
		return TRUE;
	}
# line 1082 "../xia_slit.st"
	if (((pVar->hBusy) || (pVar->vBusy)) && seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 13;
		return TRUE;
	}
# line 1086 "../xia_slit.st"
	if (seq_delay(ssId, 1))
	{
		*pNextState = 5;
		*pTransNum = 14;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "xiahsc" */
static void A_xiahsc_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
# line 888 "../xia_slit.st"
			(pVar->init_delay) = 1;
# line 888 "../xia_slit.st"
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 38 /* init */, 2);
			}
# line 888 "../xia_slit.st"
			;
# line 888 "../xia_slit.st"
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 888 "../xia_slit.st"
			;
# line 888 "../xia_slit.st"
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 888 "../xia_slit.st"
			;
		}
		return;
	case 2:
		{
# line 895 "../xia_slit.st"
			{
				sprintf((pVar->hsc_command), "!%s %s", "ALL", ("K"));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 895, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 895 "../xia_slit.st"
			;
			epicsThreadSleep(.1);
			seq_efClear(ssId, stop_mon);
		}
		return;
	case 3:
		{
# line 904 "../xia_slit.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 904, (pVar->SNLtaskName), 2);
				;
				printf("efTest(l_event_mon) -> premove, l=%f", (pVar->l));
				printf("\n");
				epicsThreadSleep(0.01);
			}
# line 904 "../xia_slit.st"
			;
		}
		return;
	case 4:
		{
# line 911 "../xia_slit.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 911, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "efTest() -> premove");
				epicsThreadSleep(0.01);
			}
# line 911 "../xia_slit.st"
			;
		}
		return;
	case 5:
		{
# line 922 "../xia_slit.st"
			if ((pVar->hBusy))
			{
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 919, (pVar->SNLtaskName), 2);
					;
					printf("%s\n", "efTest(start_H_move) and hBusy");
					epicsThreadSleep(0.01);
				}
				;
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), ("K"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 920, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((1));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				epicsThreadSleep(.5);
			}
# line 924 "../xia_slit.st"
			else
			{
				{
					(pVar->hBusy) = ((1));
					seq_pvPut(ssId, 28 /* hBusy */, 2);
				}
				;
			}
# line 927 "../xia_slit.st"
			if ((pVar->hOrient) == (0))
			{
				(pVar->pos_ag) = (pVar->l);
				(pVar->pos_bg) = (pVar->r);
			}
# line 929 "../xia_slit.st"
			else
			{
				(pVar->pos_ag) = (pVar->r);
				(pVar->pos_bg) = (pVar->l);
			}
# line 930 "../xia_slit.st"
			(pVar->h_isMoving) = 1;
			seq_efSet(ssId, wait_H_input);
			seq_efClear(ssId, start_H_move);
# line 943 "../xia_slit.st"
			while (!(pVar->s_sevr))
			{
				{
					{
						(pVar->s_tmod) = ((2));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					seq_efClear(ssId, s_ainp_mon);
					{
						(pVar->s_proc) = (1);
						seq_pvPut(ssId, 13 /* s_proc */, 2);
					}
					;
					for ((pVar->i) = 0; (pVar->i) <= (30); (pVar->i)++)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 935, (pVar->SNLtaskName), 10);
								;
								printf("%s\n", "READ_HSC: got a message.");
								epicsThreadSleep(0.01);
							}
							;
							break;
						}
						epicsThreadSleep(0.02);
					}
				}
				;
				sscanf((pVar->s_ainp), "%%%s", (pVar->id));
				if (!strcmp((pVar->id), (pVar->hID)))
				{
					if ((pVar->debug_flag) >= 2)
					{
						printf("(%s,%d): ", "../xia_slit.st", 939);
						printf("discarding hID message '%s' id='%s'\n", (pVar->s_tinp), (pVar->id));
						printf("\n");
					}
					;
				}
				else
				{
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							strcpy((pVar->ainp_fifo)[(pVar->ainp_fifo_read)++], (pVar->s_ainp));
							{
								if (((pVar->ainp_fifo_read)) >= 40)
									((pVar->ainp_fifo_read)) = 0;
							}
							;
							(pVar->delta) = (pVar->ainp_fifo_read) - (pVar->ainp_fifo_write);
							if ((pVar->delta) < 0)
								(pVar->delta) += 40;
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 941);
								printf("FIFO_IN: adding msg %d:\n ->'%s'", (pVar->delta), (pVar->s_ainp));
								printf("\n");
							}
							;
						}
					}
					;
				}
			}
# line 944 "../xia_slit.st"
			(pVar->pos_a) = (((pVar->pos_ag)) * ((400.0)) + (0.5) + ((pVar->hOrigin)));
			(pVar->pos_b) = (((pVar->pos_bg)) * ((400.0)) + (0.5) + ((pVar->hOrigin)));
			sprintf((pVar->scratch), "M %ld %ld", (pVar->pos_a), (pVar->pos_b));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 947, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "start_H_move: writing to motor");
				epicsThreadSleep(0.01);
			}
# line 947 "../xia_slit.st"
			;
			{
				sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), (pVar->scratch));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 948, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 948 "../xia_slit.st"
			;
		}
		return;
	case 6:
		{
# line 955 "../xia_slit.st"
			if ((pVar->vBusy))
			{
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), ("K"));
					if ((pVar->debug_flag) >= 10)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 953, (pVar->SNLtaskName), 10);
						;
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->s_scan) = ((0));
						seq_pvPut(ssId, 10 /* s_scan */, 2);
					}
					;
					{
						(pVar->s_tmod) = ((1));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					{
						strcpy((pVar->s_ieos), ("\r\n"));
						seq_pvPut(ssId, 3 /* s_ieos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_oeos), ("\r"));
						seq_pvPut(ssId, 4 /* s_oeos */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 2 /* s_aout */, 2);
						epicsThreadSleep(0.01);
					}
					;
				}
				;
				epicsThreadSleep(.5);
			}
# line 957 "../xia_slit.st"
			else
			{
				{
					(pVar->vBusy) = ((1));
					seq_pvPut(ssId, 32 /* vBusy */, 2);
				}
				;
			}
# line 960 "../xia_slit.st"
			if ((pVar->vOrient) == (0))
			{
				(pVar->pos_ag) = (pVar->t);
				(pVar->pos_bg) = (pVar->b);
			}
# line 962 "../xia_slit.st"
			else
			{
				(pVar->pos_ag) = (pVar->b);
				(pVar->pos_bg) = (pVar->t);
			}
# line 963 "../xia_slit.st"
			(pVar->v_isMoving) = 1;
			seq_efSet(ssId, wait_V_input);
			seq_efClear(ssId, start_V_move);
# line 976 "../xia_slit.st"
			while (!(pVar->s_sevr))
			{
				{
					{
						(pVar->s_tmod) = ((2));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					seq_efClear(ssId, s_ainp_mon);
					{
						(pVar->s_proc) = (1);
						seq_pvPut(ssId, 13 /* s_proc */, 2);
					}
					;
					for ((pVar->i) = 0; (pVar->i) <= (30); (pVar->i)++)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 968, (pVar->SNLtaskName), 10);
								;
								printf("%s\n", "READ_HSC: got a message.");
								epicsThreadSleep(0.01);
							}
							;
							break;
						}
						epicsThreadSleep(0.02);
					}
				}
				;
				sscanf((pVar->s_ainp), "%%%s", (pVar->id));
				if (!strcmp((pVar->id), (pVar->vID)))
				{
					if ((pVar->debug_flag) >= 2)
					{
						printf("(%s,%d): ", "../xia_slit.st", 972);
						printf("discarding vID message '%s' id='%s'\n", (pVar->s_tinp), (pVar->id));
						printf("\n");
					}
					;
				}
				else
				{
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							strcpy((pVar->ainp_fifo)[(pVar->ainp_fifo_read)++], (pVar->s_ainp));
							{
								if (((pVar->ainp_fifo_read)) >= 40)
									((pVar->ainp_fifo_read)) = 0;
							}
							;
							(pVar->delta) = (pVar->ainp_fifo_read) - (pVar->ainp_fifo_write);
							if ((pVar->delta) < 0)
								(pVar->delta) += 40;
							if ((pVar->debug_flag) >= 10)
							{
								printf("(%s,%d): ", "../xia_slit.st", 974);
								printf("FIFO_IN: adding msg %d:\n ->'%s'", (pVar->delta), (pVar->s_ainp));
								printf("\n");
							}
							;
						}
					}
					;
				}
			}
# line 977 "../xia_slit.st"
			(pVar->pos_a) = (((pVar->pos_ag)) * ((400.0)) + (0.5) + ((pVar->vOrigin)));
			(pVar->pos_b) = (((pVar->pos_bg)) * ((400.0)) + (0.5) + ((pVar->vOrigin)));
			sprintf((pVar->scratch), "M %ld %ld", (pVar->pos_a), (pVar->pos_b));
			{
				sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), (pVar->scratch));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 980, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 980 "../xia_slit.st"
			;
		}
		return;
	case 7:
		{
# line 991 "../xia_slit.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 23 /* port */, 0);
			{
				(pVar->locate) = (0);
				seq_pvPut(ssId, 37 /* locate */, 2);
			}
# line 993 "../xia_slit.st"
			;
			{
				sprintf((pVar->hsc_command), "!%s %s", "ALL", ("P"));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 994, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 994 "../xia_slit.st"
			;
			epicsThreadSleep(.1);
# line 1000 "../xia_slit.st"
			while (!(pVar->s_sevr))
			{
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 997, (pVar->SNLtaskName), 10);
					;
					printf("%s\n", "locate: calling READ_HSC");
					epicsThreadSleep(0.01);
				}
				;
				{
					{
						(pVar->s_tmod) = ((2));
						seq_pvPut(ssId, 9 /* s_tmod */, 2);
					}
					;
					seq_efClear(ssId, s_ainp_mon);
					{
						(pVar->s_proc) = (1);
						seq_pvPut(ssId, 13 /* s_proc */, 2);
					}
					;
					for ((pVar->i) = 0; (pVar->i) <= (30); (pVar->i)++)
					{
						if (seq_efTest(ssId, s_ainp_mon))
						{
							if ((pVar->debug_flag) >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../xia_slit.st", 998, (pVar->SNLtaskName), 10);
								;
								printf("%s\n", "READ_HSC: got a message.");
								epicsThreadSleep(0.01);
							}
							;
							break;
						}
						epicsThreadSleep(0.02);
					}
				}
				;
				if (!(pVar->s_sevr))
				{
					if (seq_efTest(ssId, s_ainp_mon))
					{
						strcpy((pVar->ainp_fifo)[(pVar->ainp_fifo_read)++], (pVar->s_ainp));
						{
							if (((pVar->ainp_fifo_read)) >= 40)
								((pVar->ainp_fifo_read)) = 0;
						}
						;
						(pVar->delta) = (pVar->ainp_fifo_read) - (pVar->ainp_fifo_write);
						if ((pVar->delta) < 0)
							(pVar->delta) += 40;
						if ((pVar->debug_flag) >= 10)
						{
							printf("(%s,%d): ", "../xia_slit.st", 999);
							printf("FIFO_IN: adding msg %d:\n ->'%s'", (pVar->delta), (pVar->s_ainp));
							printf("\n");
						}
						;
					}
				}
				;
			}
# line 1001 "../xia_slit.st"
			if ((pVar->debug_flag) >= 10)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1001, (pVar->SNLtaskName), 10);
				;
				printf("%s\n", "locate: s_sevr = TRUE");
				epicsThreadSleep(0.01);
			}
# line 1001 "../xia_slit.st"
			;
		}
		return;
	case 8:
		{
# line 1010 "../xia_slit.st"
			{
				sprintf((pVar->hsc_command), "!%s %s", "ALL", ("0 I"));
				if ((pVar->debug_flag) >= 10)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1010, (pVar->SNLtaskName), 10);
					;
					printf("WRITE_HSC: <%s>", (pVar->hsc_command));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->s_scan) = ((0));
					seq_pvPut(ssId, 10 /* s_scan */, 2);
				}
				;
				{
					(pVar->s_tmod) = ((1));
					seq_pvPut(ssId, 9 /* s_tmod */, 2);
				}
				;
				{
					strcpy((pVar->s_ieos), ("\r\n"));
					seq_pvPut(ssId, 3 /* s_ieos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_oeos), ("\r"));
					seq_pvPut(ssId, 4 /* s_oeos */, 2);
					epicsThreadSleep(0.01);
				}
				;
				{
					strcpy((pVar->s_aout), (pVar->hsc_command));
					seq_pvPut(ssId, 2 /* s_aout */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 1011 "../xia_slit.st"
			epicsThreadSleep(.1);
			{
				(pVar->calibrate) = ((0));
				seq_pvPut(ssId, 40 /* calibrate */, 2);
			}
# line 1012 "../xia_slit.st"
			;
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 37 /* locate */, 2);
			}
# line 1013 "../xia_slit.st"
			;
		}
		return;
	case 9:
		{
# line 1021 "../xia_slit.st"
			{
				(pVar->delta) = (pVar->ainp_fifo_read) - (pVar->ainp_fifo_write);
				if ((pVar->delta) < 0)
					(pVar->delta) += 40;
				strcpy((pVar->scratch), (pVar->ainp_fifo)[(pVar->ainp_fifo_write)++]);
				if ((pVar->debug_flag) >= 10)
				{
					printf("(%s,%d): ", "../xia_slit.st", 1021);
					printf("FIFO_OUT: got message %d:\n ->'%s'", (pVar->delta), (pVar->scratch));
					printf("\n");
				}
				;
				{
					if (((pVar->ainp_fifo_write)) >= 40)
						((pVar->ainp_fifo_write)) = 0;
				}
				;
			}
# line 1021 "../xia_slit.st"
			;
# line 1023 "../xia_slit.st"
			(pVar->id)[0] = (pVar->word1)[0] = (pVar->word2)[0] = (pVar->word3)[0] = (pVar->word4)[0] = (pVar->word5)[0] = (pVar->word6)[0] = 0;
# line 1025 "../xia_slit.st"
			(pVar->numWords) = sscanf((pVar->scratch), "%s %s %s %s %s %s", (pVar->word1), (pVar->word2), (pVar->word3), (pVar->word4), (pVar->word5), (pVar->word6));
			(pVar->hasID) = sscanf((pVar->word1), "%%%s", (pVar->id));
# line 1029 "../xia_slit.st"
			if ((pVar->debug_flag) >= 5 && ((pVar->numWords) > 0))
			{
				printf("xia_slit:fifo_read: (%ld words)...\n'%s'\n", (pVar->numWords), (pVar->scratch));
			}
		}
		return;
	case 10:
		{
# line 1036 "../xia_slit.st"
			seq_efClear(ssId, new_H_RB);
# line 1041 "../xia_slit.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("(%s,%d): ", "../xia_slit.st", 1041);
				printf("efTest(new_H_RB)==1; h_Busy|hBusy=%d, l=%f,l_RB=%f", (pVar->h_Busy) | (pVar->hBusy), (pVar->l), (pVar->l_RB));
				printf("\n");
			}
# line 1041 "../xia_slit.st"
			;
# line 1048 "../xia_slit.st"
			if (!(pVar->hBusy) && !(pVar->h_Busy))
			{
				if ((fabs(((pVar->l)) - ((pVar->l_RB))) > ((1.e-9))))
				{
					(pVar->load_l) = ((pVar->l_RB));
					seq_pvPut(ssId, 74 /* load_l */, 2);
				}
				;
				if ((fabs(((pVar->r)) - ((pVar->r_RB))) > ((1.e-9))))
				{
					(pVar->load_r) = ((pVar->r_RB));
					seq_pvPut(ssId, 86 /* load_r */, 2);
				}
				;
				if ((fabs(((pVar->h0)) - ((pVar->h0_RB))) > ((1.e-9))))
				{
					(pVar->load_h0) = ((pVar->h0_RB));
					seq_pvPut(ssId, 92 /* load_h0 */, 2);
				}
				;
				if ((fabs(((pVar->width)) - ((pVar->width_RB))) > ((1.e-9))))
				{
					(pVar->load_width) = ((pVar->width_RB));
					seq_pvPut(ssId, 104 /* load_width */, 2);
				}
				;
				seq_efClear(ssId, wait_H_input);
			}
		}
		return;
	case 11:
		{
# line 1055 "../xia_slit.st"
			seq_efClear(ssId, new_V_RB);
# line 1066 "../xia_slit.st"
			if (!(pVar->vBusy) && !(pVar->v_Busy))
			{
				if ((fabs(((pVar->t)) - ((pVar->t_RB))) > ((1.e-9))))
				{
					(pVar->load_t) = ((pVar->t_RB));
					seq_pvPut(ssId, 68 /* load_t */, 2);
				}
				;
				if ((fabs(((pVar->b)) - ((pVar->b_RB))) > ((1.e-9))))
				{
					(pVar->load_b) = ((pVar->b_RB));
					seq_pvPut(ssId, 80 /* load_b */, 2);
				}
				;
				if ((fabs(((pVar->v0)) - ((pVar->v0_RB))) > ((1.e-9))))
				{
					(pVar->load_v0) = ((pVar->v0_RB));
					seq_pvPut(ssId, 98 /* load_v0 */, 2);
				}
				;
				if ((fabs(((pVar->height)) - ((pVar->height_RB))) > ((1.e-9))))
				{
					(pVar->load_height) = ((pVar->height_RB));
					seq_pvPut(ssId, 110 /* load_height */, 2);
				}
				;
				seq_efClear(ssId, wait_V_input);
			}
		}
		return;
	case 12:
		{
# line 1073 "../xia_slit.st"
			{
				(pVar->stop) = ((0));
				seq_pvPut(ssId, 36 /* stop */, 2);
			}
# line 1073 "../xia_slit.st"
			;
			seq_efClear(ssId, stop_mon);
		}
		return;
	case 13:
		{
# line 1081 "../xia_slit.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 37 /* locate */, 2);
			}
# line 1081 "../xia_slit.st"
			;
		}
		return;
	case 14:
		{
# line 1084 "../xia_slit.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 37 /* locate */, 2);
			}
# line 1084 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1085, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "IDLE POLL");
				epicsThreadSleep(0.01);
			}
# line 1085 "../xia_slit.st"
			;
		}
		return;
	}
}
/* Code for state "premove" in state set "xiahsc" */

/* Delay function for state "premove" in state set "xiahsc" */
static void D_xiahsc_premove(SS_ID ssId, struct UserVar *pVar)
{
# line 1099 "../xia_slit.st"
# line 1104 "../xia_slit.st"
# line 1109 "../xia_slit.st"
# line 1114 "../xia_slit.st"
# line 1119 "../xia_slit.st"
# line 1124 "../xia_slit.st"
# line 1129 "../xia_slit.st"
# line 1134 "../xia_slit.st"
# line 1223 "../xia_slit.st"
}

/* Event function for state "premove" in state set "xiahsc" */
static long E_xiahsc_premove(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1099 "../xia_slit.st"
	if (seq_efTest(ssId, l_event_mon) && ((pVar->l) == (pVar->l_old)))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 1104 "../xia_slit.st"
	if (seq_efTest(ssId, r_event_mon) && ((pVar->r) == (pVar->r_old)))
	{
		*pNextState = 5;
		*pTransNum = 1;
		return TRUE;
	}
# line 1109 "../xia_slit.st"
	if (seq_efTest(ssId, t_event_mon) && ((pVar->t) == (pVar->t_old)))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
# line 1114 "../xia_slit.st"
	if (seq_efTest(ssId, b_event_mon) && ((pVar->b) == (pVar->b_old)))
	{
		*pNextState = 5;
		*pTransNum = 3;
		return TRUE;
	}
# line 1119 "../xia_slit.st"
	if (seq_efTest(ssId, h0_event_mon) && ((pVar->h0) == (pVar->h0_old)))
	{
		*pNextState = 5;
		*pTransNum = 4;
		return TRUE;
	}
# line 1124 "../xia_slit.st"
	if (seq_efTest(ssId, v0_event_mon) && ((pVar->v0) == (pVar->v0_old)))
	{
		*pNextState = 5;
		*pTransNum = 5;
		return TRUE;
	}
# line 1129 "../xia_slit.st"
	if (seq_efTest(ssId, width_event_mon) && ((pVar->width) == (pVar->width_old)))
	{
		*pNextState = 5;
		*pTransNum = 6;
		return TRUE;
	}
# line 1134 "../xia_slit.st"
	if (seq_efTest(ssId, height_event_mon) && ((pVar->height) == (pVar->height_old)))
	{
		*pNextState = 5;
		*pTransNum = 7;
		return TRUE;
	}
# line 1223 "../xia_slit.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 8;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "premove" in state set "xiahsc" */
static void A_xiahsc_premove(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1096 "../xia_slit.st"
			seq_efClear(ssId, l_event_mon);
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 1097 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1098, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "l==l_old: ->h_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1098 "../xia_slit.st"
			;
		}
		return;
	case 1:
		{
# line 1101 "../xia_slit.st"
			seq_efClear(ssId, r_event_mon);
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 1102 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1103, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "r==r_old: ->h_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1103 "../xia_slit.st"
			;
		}
		return;
	case 2:
		{
# line 1106 "../xia_slit.st"
			seq_efClear(ssId, t_event_mon);
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 1107 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1108, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "t==t_old: ->v_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1108 "../xia_slit.st"
			;
		}
		return;
	case 3:
		{
# line 1111 "../xia_slit.st"
			seq_efClear(ssId, b_event_mon);
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 1112 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1113, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "b==b_old: ->v_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1113 "../xia_slit.st"
			;
		}
		return;
	case 4:
		{
# line 1116 "../xia_slit.st"
			seq_efClear(ssId, h0_event_mon);
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 1117 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1118, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "h0==h0_old: ->h_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1118 "../xia_slit.st"
			;
		}
		return;
	case 5:
		{
# line 1121 "../xia_slit.st"
			seq_efClear(ssId, v0_event_mon);
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 1122 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1123, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "v0==v0_old: ->v_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1123 "../xia_slit.st"
			;
		}
		return;
	case 6:
		{
# line 1126 "../xia_slit.st"
			seq_efClear(ssId, width_event_mon);
			{
				(pVar->h_BusyOff) = (0);
				seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
			}
# line 1127 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1128, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "width==width_old: ->h_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1128 "../xia_slit.st"
			;
		}
		return;
	case 7:
		{
# line 1131 "../xia_slit.st"
			seq_efClear(ssId, height_event_mon);
			{
				(pVar->v_BusyOff) = (0);
				seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
			}
# line 1132 "../xia_slit.st"
			;
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1133, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "height==height_old: ->v_BusyOff");
				epicsThreadSleep(0.01);
			}
# line 1133 "../xia_slit.st"
			;
		}
		return;
	case 8:
		{
# line 1179 "../xia_slit.st"
			if (seq_efTest(ssId, l_event_mon) || seq_efTest(ssId, r_event_mon) || seq_efTest(ssId, h0_event_mon) || seq_efTest(ssId, width_event_mon))
			{
				if (seq_efTest(ssId, l_event_mon) || seq_efTest(ssId, r_event_mon))
				{
					(pVar->width) = (pVar->r) + (pVar->l);
					(pVar->h0) = ((pVar->r) - (pVar->l)) / 2;
				}
				else
				{
					(pVar->l) = (pVar->width) / 2 - (pVar->h0);
					(pVar->r) = (pVar->width) / 2 + (pVar->h0);
				}
				(pVar->error) = 0;
				if (!((((pVar->l_lo)) <= ((pVar->l))) && (((pVar->l)) <= ((pVar->l_hi)))))
				{
					(pVar->l) = (pVar->l_old);
					(pVar->error)++;
				}
				if (!((((pVar->r_lo)) <= ((pVar->r))) && (((pVar->r)) <= ((pVar->r_hi)))))
				{
					(pVar->r) = (pVar->r_old);
					(pVar->error)++;
				}
				if (!((((pVar->h0_lo)) <= ((pVar->h0))) && (((pVar->h0)) <= ((pVar->h0_hi)))))
				{
					(pVar->h0) = (pVar->h0_old);
					(pVar->error)++;
				}
				if (!((((pVar->width_lo)) <= ((pVar->width))) && (((pVar->width)) <= ((pVar->width_hi)))))
				{
					(pVar->width) = (pVar->width_old);
					(pVar->error)++;
				}
				if ((pVar->error))
				{
					if (seq_efTest(ssId, l_event_mon))
					{
						(pVar->load_l) = ((pVar->l));
						seq_pvPut(ssId, 74 /* load_l */, 2);
					}
					;
					if (seq_efTest(ssId, r_event_mon))
					{
						(pVar->load_r) = ((pVar->r));
						seq_pvPut(ssId, 86 /* load_r */, 2);
					}
					;
					if (seq_efTest(ssId, h0_event_mon))
					{
						(pVar->load_h0) = ((pVar->h0));
						seq_pvPut(ssId, 92 /* load_h0 */, 2);
					}
					;
					if (seq_efTest(ssId, width_event_mon))
					{
						(pVar->load_width) = ((pVar->width));
						seq_pvPut(ssId, 104 /* load_width */, 2);
					}
					;
					(pVar->error) = (15);
					{
						strcpy((pVar->errMsg), "H soft limits exceeded");
						seq_pvPut(ssId, 112 /* errMsg */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->h_BusyOff) = (0);
						seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
					}
					;
				}
				else
				{
					{
						strcpy((pVar->errMsg), "no error");
						seq_pvPut(ssId, 112 /* errMsg */, 2);
						epicsThreadSleep(0.01);
					}
					;
					seq_efSet(ssId, start_H_move);
					if ((pVar->debug_flag) >= 2)
					{
						printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1169, (pVar->SNLtaskName), 2);
						;
						printf("%s\n", "premove: efSet(start_H_move)");
						epicsThreadSleep(0.01);
					}
					;
					if (seq_efTest(ssId, l_event_mon) || seq_efTest(ssId, r_event_mon))
					{
						{
							(pVar->load_h0) = ((pVar->h0));
							seq_pvPut(ssId, 92 /* load_h0 */, 2);
						}
						;
						{
							(pVar->load_width) = ((pVar->width));
							seq_pvPut(ssId, 104 /* load_width */, 2);
						}
						;
					}
					else
					{
						{
							(pVar->load_l) = ((pVar->l));
							seq_pvPut(ssId, 74 /* load_l */, 2);
						}
						;
						{
							(pVar->load_r) = ((pVar->r));
							seq_pvPut(ssId, 86 /* load_r */, 2);
						}
						;
					}
				}
				seq_pvPut(ssId, 111 /* error */, 0);
				seq_efClear(ssId, l_event_mon);
				seq_efClear(ssId, r_event_mon);
				seq_efClear(ssId, h0_event_mon);
				seq_efClear(ssId, width_event_mon);
			}
# line 1222 "../xia_slit.st"
			if (seq_efTest(ssId, t_event_mon) || seq_efTest(ssId, b_event_mon) || seq_efTest(ssId, v0_event_mon) || seq_efTest(ssId, height_event_mon))
			{
				if (seq_efTest(ssId, t_event_mon) || seq_efTest(ssId, b_event_mon))
				{
					(pVar->height) = (pVar->t) + (pVar->b);
					(pVar->v0) = ((pVar->t) - (pVar->b)) / 2;
				}
				else
				{
					(pVar->t) = (pVar->height) / 2 + (pVar->v0);
					(pVar->b) = (pVar->height) / 2 - (pVar->v0);
				}
				(pVar->error) = 0;
				if (!((((pVar->t_lo)) <= ((pVar->t))) && (((pVar->t)) <= ((pVar->t_hi)))))
				{
					(pVar->t) = (pVar->t_old);
					(pVar->error)++;
				}
				if (!((((pVar->b_lo)) <= ((pVar->b))) && (((pVar->b)) <= ((pVar->b_hi)))))
				{
					(pVar->b) = (pVar->b_old);
					(pVar->error)++;
				}
				if (!((((pVar->v0_lo)) <= ((pVar->v0))) && (((pVar->v0)) <= ((pVar->v0_hi)))))
				{
					(pVar->v0) = (pVar->v0_old);
					(pVar->error)++;
				}
				if (!((((pVar->height_lo)) <= ((pVar->height))) && (((pVar->height)) <= ((pVar->height_hi)))))
				{
					(pVar->height) = (pVar->height_old);
					(pVar->error)++;
				}
				if ((pVar->error))
				{
					if (seq_efTest(ssId, t_event_mon))
					{
						(pVar->load_t) = ((pVar->t));
						seq_pvPut(ssId, 68 /* load_t */, 2);
					}
					;
					if (seq_efTest(ssId, b_event_mon))
					{
						(pVar->load_b) = ((pVar->b));
						seq_pvPut(ssId, 80 /* load_b */, 2);
					}
					;
					if (seq_efTest(ssId, v0_event_mon))
					{
						(pVar->load_v0) = ((pVar->v0));
						seq_pvPut(ssId, 98 /* load_v0 */, 2);
					}
					;
					if (seq_efTest(ssId, height_event_mon))
					{
						(pVar->load_height) = ((pVar->height));
						seq_pvPut(ssId, 110 /* load_height */, 2);
					}
					;
					(pVar->error) = (15);
					{
						strcpy((pVar->errMsg), "V soft limits exceeded");
						seq_pvPut(ssId, 112 /* errMsg */, 2);
						epicsThreadSleep(0.01);
					}
					;
					{
						(pVar->v_BusyOff) = (0);
						seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
					}
					;
				}
				else
				{
					{
						strcpy((pVar->errMsg), "no error");
						seq_pvPut(ssId, 112 /* errMsg */, 2);
						epicsThreadSleep(0.01);
					}
					;
					seq_efSet(ssId, start_V_move);
					if (seq_efTest(ssId, t_event_mon) || seq_efTest(ssId, b_event_mon))
					{
						{
							(pVar->load_v0) = ((pVar->v0));
							seq_pvPut(ssId, 98 /* load_v0 */, 2);
						}
						;
						{
							(pVar->load_height) = ((pVar->height));
							seq_pvPut(ssId, 110 /* load_height */, 2);
						}
						;
					}
					else
					{
						{
							(pVar->load_t) = ((pVar->t));
							seq_pvPut(ssId, 68 /* load_t */, 2);
						}
						;
						{
							(pVar->load_b) = ((pVar->b));
							seq_pvPut(ssId, 80 /* load_b */, 2);
						}
						;
					}
				}
				seq_pvPut(ssId, 111 /* error */, 0);
				seq_efClear(ssId, t_event_mon);
				seq_efClear(ssId, b_event_mon);
				seq_efClear(ssId, v0_event_mon);
				seq_efClear(ssId, height_event_mon);
			}
		}
		return;
	}
}
/* Code for state "process_response" in state set "xiahsc" */

/* Entry function for state "process_response" in state set "xiahsc" */
static void I_xiahsc_process_response(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 1230 "../xia_slit.st"
	if ((pVar->debug_flag) >= 5)
	{
		printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1230, (pVar->SNLtaskName), 5);
		;
		printf("process_response:entry numWords=%ld", (pVar->numWords));
		printf("\n");
		epicsThreadSleep(0.01);
	}
	;
}

/* Delay function for state "process_response" in state set "xiahsc" */
static void D_xiahsc_process_response(SS_ID ssId, struct UserVar *pVar)
{
# line 1240 "../xia_slit.st"
# line 1243 "../xia_slit.st"
# line 1263 "../xia_slit.st"
# line 1278 "../xia_slit.st"
# line 1319 "../xia_slit.st"
# line 1336 "../xia_slit.st"
# line 1343 "../xia_slit.st"
}

/* Event function for state "process_response" in state set "xiahsc" */
static long E_xiahsc_process_response(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1240 "../xia_slit.st"
	if ((pVar->numWords) == -1)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 1243 "../xia_slit.st"
	if (((pVar->numWords) == 2) && (!strcmp((pVar->word2), "OK;")))
	{
		*pNextState = 5;
		*pTransNum = 1;
		return TRUE;
	}
# line 1263 "../xia_slit.st"
	if (((pVar->numWords) >= 2) && (!strcmp((pVar->word2), "ERROR;")))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
# line 1278 "../xia_slit.st"
	if (((pVar->numWords) == 2) && (!strcmp((pVar->word2), "BUSY;")))
	{
		*pNextState = 5;
		*pTransNum = 3;
		return TRUE;
	}
# line 1319 "../xia_slit.st"
	if (((pVar->numWords) == 4) && (!strcmp((pVar->word4), "DONE;")))
	{
		*pNextState = 8;
		*pTransNum = 4;
		return TRUE;
	}
# line 1336 "../xia_slit.st"
	if (((pVar->numWords) == 5) && (!strcmp((pVar->word2), "OK")))
	{
		*pNextState = 8;
		*pTransNum = 5;
		return TRUE;
	}
# line 1343 "../xia_slit.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 6;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "process_response" in state set "xiahsc" */
static void A_xiahsc_process_response(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
		}
		return;
	case 2:
		{
# line 1249 "../xia_slit.st"
			if ((pVar->numWords) == 2)
			{
				{
					(pVar->error) = ((16));
					seq_pvPut(ssId, 111 /* error */, 2);
				}
				;
				sprintf((pVar->errMsg), "%s: unknown error", (pVar->id));
				seq_pvPut(ssId, 112 /* errMsg */, 0);
			}
# line 1253 "../xia_slit.st"
			else
			{
				sscanf((pVar->word3), "%d",  & (pVar->error));
				seq_pvPut(ssId, 111 /* error */, 0);
				{
					strcpy((pVar->errMsg), (pVar->hscErrors)[(pVar->error)]);
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 1258 "../xia_slit.st"
			if (!strcmp((pVar->word1), (pVar->hID)))
			{
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1255, (pVar->SNLtaskName), 2);
					;
					printf("'%s' reported an error: ->h_BusyOff", (pVar->hID));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->h_BusyOff) = (0);
					seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
				}
				;
				(pVar->h_isMoving) = 0;
			}
# line 1262 "../xia_slit.st"
			else
			if (!strcmp((pVar->word1), (pVar->vID)))
			{
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1259, (pVar->SNLtaskName), 2);
					;
					printf("'%s' reported an error: ->v_BusyOff", (pVar->vID));
					printf("\n");
					epicsThreadSleep(0.01);
				}
				;
				{
					(pVar->v_BusyOff) = (0);
					seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
				}
				;
				(pVar->v_isMoving) = 0;
			}
		}
		return;
	case 3:
		{
# line 1271 "../xia_slit.st"
			if (!strcmp((pVar->id), (pVar->hID)))
			{
				{
					(pVar->hBusy) = ((1));
					seq_pvPut(ssId, 28 /* hBusy */, 2);
				}
				;
			}
# line 1273 "../xia_slit.st"
			else
			{
				{
					(pVar->vBusy) = ((1));
					seq_pvPut(ssId, 32 /* vBusy */, 2);
				}
				;
			}
# line 1277 "../xia_slit.st"
			if ((pVar->error))
			{
				{
					(pVar->error) = (0);
					seq_pvPut(ssId, 111 /* error */, 2);
				}
				;
				{
					strcpy((pVar->errMsg), "no error");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
		}
		return;
	case 4:
		{
# line 1285 "../xia_slit.st"
			sscanf((pVar->word2), "%ld",  & (pVar->pos_a));
			sscanf((pVar->word3), "%ld",  & (pVar->pos_b));
# line 1290 "../xia_slit.st"
			if ((pVar->error))
			{
				{
					(pVar->error) = (0);
					seq_pvPut(ssId, 111 /* error */, 2);
				}
				;
				{
					strcpy((pVar->errMsg), "no error");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 1305 "../xia_slit.st"
			if (!strcmp((pVar->id), (pVar->hID)))
			{
				if ((pVar->debug_flag) >= 5)
				{
					printf("(%s,%d): ", "../xia_slit.st", 1293);
					printf("hDONE: pos_a = %ld, pos_b = %ld", (pVar->pos_a), (pVar->pos_b));
					printf("\n");
				}
				;
				(pVar->pos_ag) = ((((pVar->pos_a)) - ((pVar->hOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->pos_b)) - ((pVar->hOrigin))) / ((400.0)));
				(pVar->h_isMoving) = 0;
				(pVar->l_raw) = (((pVar->l)) * ((400.0)) + (0.5) + ((pVar->hOrigin)));
				(pVar->r_raw) = (((pVar->r)) * ((400.0)) + (0.5) + ((pVar->hOrigin)));
				if ((pVar->debug_flag) >= 10)
				{
					printf("(%s,%d): ", "../xia_slit.st", 1299);
					printf("hRaw; l=%ld, r=%ld", (pVar->l_raw), (pVar->r_raw));
					printf("\n");
				}
				;
				if ((pVar->hOrient) == (0))
				{
					if (((pVar->pos_a) == (pVar->l_raw)) && ((pVar->pos_b) == (pVar->r_raw)))
					{
						(pVar->h_BusyOff) = (0);
						seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
					}
					;
				}
				else
				{
					if (((pVar->pos_a) == (pVar->r_raw)) && ((pVar->pos_b) == (pVar->l_raw)))
					{
						(pVar->h_BusyOff) = (0);
						seq_pvPut(ssId, 30 /* h_BusyOff */, 2);
					}
					;
				}
			}
# line 1318 "../xia_slit.st"
			else
			if (!strcmp((pVar->id), (pVar->vID)))
			{
				if ((pVar->debug_flag) >= 5)
				{
					printf("(%s,%d): ", "../xia_slit.st", 1306);
					printf("vDONE: pos_a = %ld, pos_b = %ld", (pVar->pos_a), (pVar->pos_b));
					printf("\n");
				}
				;
				(pVar->pos_ag) = ((((pVar->pos_a)) - ((pVar->vOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->pos_b)) - ((pVar->vOrigin))) / ((400.0)));
				(pVar->v_isMoving) = 0;
				(pVar->t_raw) = (((pVar->t)) * ((400.0)) + (0.5) + ((pVar->vOrigin)));
				(pVar->b_raw) = (((pVar->b)) * ((400.0)) + (0.5) + ((pVar->vOrigin)));
				if ((pVar->debug_flag) >= 10)
				{
					printf("(%s,%d): ", "../xia_slit.st", 1312);
					printf("vRaw; t=%ld, b=%ld", (pVar->t_raw), (pVar->b_raw));
					printf("\n");
				}
				;
				if ((pVar->vOrient) == (0))
				{
					if (((pVar->pos_a) == (pVar->t_raw)) && ((pVar->pos_b) == (pVar->b_raw)))
					{
						(pVar->v_BusyOff) = (0);
						seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
					}
					;
				}
				else
				{
					if (((pVar->pos_a) == (pVar->b_raw)) && ((pVar->pos_b) == (pVar->t_raw)))
					{
						(pVar->v_BusyOff) = (0);
						seq_pvPut(ssId, 34 /* v_BusyOff */, 2);
					}
					;
				}
			}
		}
		return;
	case 5:
		{
# line 1323 "../xia_slit.st"
			sscanf((pVar->word3), "%ld",  & (pVar->pos_a));
			sscanf((pVar->word4), "%ld",  & (pVar->pos_b));
# line 1328 "../xia_slit.st"
			if ((pVar->error))
			{
				{
					(pVar->error) = (0);
					seq_pvPut(ssId, 111 /* error */, 2);
				}
				;
				{
					strcpy((pVar->errMsg), "no error");
					seq_pvPut(ssId, 112 /* errMsg */, 2);
					epicsThreadSleep(0.01);
				}
				;
			}
# line 1332 "../xia_slit.st"
			if (!strcmp((pVar->id), (pVar->hID)))
			{
				(pVar->pos_ag) = ((((pVar->pos_a)) - ((pVar->hOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->pos_b)) - ((pVar->hOrigin))) / ((400.0)));
			}
# line 1335 "../xia_slit.st"
			else
			{
				(pVar->pos_ag) = ((((pVar->pos_a)) - ((pVar->vOrigin))) / ((400.0)));
				(pVar->pos_bg) = ((((pVar->pos_b)) - ((pVar->vOrigin))) / ((400.0)));
			}
		}
		return;
	case 6:
		{
# line 1341 "../xia_slit.st"
			printf("xia_slit: did not understand reply: '%s' '%s' '%s' '%s' '%s' '%s'\n", (pVar->word1), (pVar->word2), (pVar->word3), (pVar->word4), (pVar->word5), (pVar->word6));
			epicsThreadSleep(.1);
		}
		return;
	}
}
/* Code for state "update_positions" in state set "xiahsc" */

/* Delay function for state "update_positions" in state set "xiahsc" */
static void D_xiahsc_update_positions(SS_ID ssId, struct UserVar *pVar)
{
# line 1391 "../xia_slit.st"
}

/* Event function for state "update_positions" in state set "xiahsc" */
static long E_xiahsc_update_positions(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1391 "../xia_slit.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "update_positions" in state set "xiahsc" */
static void A_xiahsc_update_positions(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1370 "../xia_slit.st"
			if (!strcmp((pVar->id), (pVar->hID)))
			{
				if ((pVar->hOrient) == (0))
				{
					{
						(pVar->l_RB) = ((pVar->pos_ag));
						seq_pvPut(ssId, 73 /* l_RB */, 2);
					}
					;
					{
						(pVar->r_RB) = ((pVar->pos_bg));
						seq_pvPut(ssId, 85 /* r_RB */, 2);
					}
					;
				}
				else
				{
					{
						(pVar->r_RB) = ((pVar->pos_ag));
						seq_pvPut(ssId, 85 /* r_RB */, 2);
					}
					;
					{
						(pVar->l_RB) = ((pVar->pos_bg));
						seq_pvPut(ssId, 73 /* l_RB */, 2);
					}
					;
				}
				{
					(pVar->width_RB) = ((pVar->l_RB) + (pVar->r_RB));
					seq_pvPut(ssId, 103 /* width_RB */, 2);
				}
				;
				{
					(pVar->h0_RB) = (((pVar->r_RB) - (pVar->l_RB)) / 2);
					seq_pvPut(ssId, 91 /* h0_RB */, 2);
				}
				;
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1361, (pVar->SNLtaskName), 2);
					;
					printf("%s\n", "update_positions: efSet(new_H_RB)");
					epicsThreadSleep(0.01);
				}
				;
				seq_efSet(ssId, new_H_RB);
				{
					(pVar->hBusy) = ((0));
					seq_pvPut(ssId, 28 /* hBusy */, 2);
				}
				;
				(pVar->l_old) = (pVar->l_RB);
				(pVar->r_old) = (pVar->r_RB);
				(pVar->h0_old) = (pVar->h0_RB);
				(pVar->width_old) = (pVar->width_RB);
			}
# line 1390 "../xia_slit.st"
			if (!strcmp((pVar->id), (pVar->vID)))
			{
				if ((pVar->vOrient) == (0))
				{
					{
						(pVar->t_RB) = ((pVar->pos_ag));
						seq_pvPut(ssId, 67 /* t_RB */, 2);
					}
					;
					{
						(pVar->b_RB) = ((pVar->pos_bg));
						seq_pvPut(ssId, 79 /* b_RB */, 2);
					}
					;
				}
				else
				{
					{
						(pVar->b_RB) = ((pVar->pos_ag));
						seq_pvPut(ssId, 79 /* b_RB */, 2);
					}
					;
					{
						(pVar->t_RB) = ((pVar->pos_bg));
						seq_pvPut(ssId, 67 /* t_RB */, 2);
					}
					;
				}
				{
					(pVar->height_RB) = ((pVar->t_RB) + (pVar->b_RB));
					seq_pvPut(ssId, 109 /* height_RB */, 2);
				}
				;
				{
					(pVar->v0_RB) = (((pVar->t_RB) - (pVar->b_RB)) / 2);
					seq_pvPut(ssId, 97 /* v0_RB */, 2);
				}
				;
				seq_efSet(ssId, new_V_RB);
				{
					(pVar->vBusy) = ((0));
					seq_pvPut(ssId, 32 /* vBusy */, 2);
				}
				;
				(pVar->t_old) = (pVar->t_RB);
				(pVar->b_old) = (pVar->b_RB);
				(pVar->v0_old) = (pVar->v0_RB);
				(pVar->height_old) = (pVar->height_RB);
			}
		}
		return;
	}
}

/* Exit handler */
static void exit_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/************************ Tables ***********************/

/* Database Blocks */
static struct seqChan seqChan[NUM_CHANNELS] = {
  {"{S}.AINP", (void *)OFFSET(struct UserVar, s_ainp[0]), "s_ainp", 
    "string", 1, 23, 1, 1, 0, 0, 0},

  {"{S}.TINP", (void *)OFFSET(struct UserVar, s_tinp[0]), "s_tinp", 
    "string", 1, 24, 0, 1, 0, 0, 0},

  {"{S}.AOUT", (void *)OFFSET(struct UserVar, s_aout[0]), "s_aout", 
    "string", 1, 25, 0, 0, 0, 0, 0},

  {"{S}.IEOS", (void *)OFFSET(struct UserVar, s_ieos[0]), "s_ieos", 
    "string", 1, 26, 0, 0, 0, 0, 0},

  {"{S}.OEOS", (void *)OFFSET(struct UserVar, s_oeos[0]), "s_oeos", 
    "string", 1, 27, 0, 0, 0, 0, 0},

  {"{S}.TMOT", (void *)OFFSET(struct UserVar, s_tmot), "s_tmot", 
    "double", 1, 28, 0, 1, 0, 0, 0},

  {"{S}.OFMT", (void *)OFFSET(struct UserVar, s_ofmt), "s_ofmt", 
    "short", 1, 29, 0, 0, 0, 0, 0},

  {"{S}.IFMT", (void *)OFFSET(struct UserVar, s_ifmt), "s_ifmt", 
    "short", 1, 30, 0, 0, 0, 0, 0},

  {"{S}.PORT", (void *)OFFSET(struct UserVar, s_port), "s_port", 
    "short", 1, 31, 0, 0, 0, 0, 0},

  {"{S}.TMOD", (void *)OFFSET(struct UserVar, s_tmod), "s_tmod", 
    "short", 1, 32, 0, 0, 0, 0, 0},

  {"{S}.SCAN", (void *)OFFSET(struct UserVar, s_scan), "s_scan", 
    "short", 1, 33, 0, 0, 0, 0, 0},

  {"{S}.NORD", (void *)OFFSET(struct UserVar, s_nord), "s_nord", 
    "short", 1, 34, 0, 1, 0, 0, 0},

  {"{S}.NRRD", (void *)OFFSET(struct UserVar, s_nrrd), "s_nrrd", 
    "short", 1, 35, 0, 0, 0, 0, 0},

  {"{S}.PROC", (void *)OFFSET(struct UserVar, s_proc), "s_proc", 
    "short", 1, 36, 0, 0, 0, 0, 0},

  {"{S}.SEVR", (void *)OFFSET(struct UserVar, s_sevr), "s_sevr", 
    "int", 1, 37, 0, 1, 0, 0, 0},

  {"{S}.OPTIONIV", (void *)OFFSET(struct UserVar, s_optioniv), "s_optioniv", 
    "int", 1, 38, 0, 0, 0, 0, 0},

  {"{S}.BAUD", (void *)OFFSET(struct UserVar, s_baud), "s_baud", 
    "short", 1, 39, 0, 0, 0, 0, 0},

  {"{S}.DBIT", (void *)OFFSET(struct UserVar, s_dbit), "s_dbit", 
    "short", 1, 40, 0, 0, 0, 0, 0},

  {"{S}.SBIT", (void *)OFFSET(struct UserVar, s_sbit), "s_sbit", 
    "short", 1, 41, 0, 0, 0, 0, 0},

  {"{S}.PRTY", (void *)OFFSET(struct UserVar, s_prty), "s_prty", 
    "short", 1, 42, 0, 0, 0, 0, 0},

  {"{S}.MCTL", (void *)OFFSET(struct UserVar, s_mctl), "s_mctl", 
    "short", 1, 43, 0, 0, 0, 0, 0},

  {"{S}.FCTL", (void *)OFFSET(struct UserVar, s_fctl), "s_fctl", 
    "short", 1, 44, 0, 0, 0, 0, 0},

  {"{P}{HSC}debug", (void *)OFFSET(struct UserVar, debug_flag), "debug_flag", 
    "int", 1, 45, 0, 1, 0, 0, 0},

  {"{P}{HSC}port", (void *)OFFSET(struct UserVar, port[0]), "port", 
    "string", 1, 46, 0, 0, 0, 0, 0},

  {"{P}{HSC}hID", (void *)OFFSET(struct UserVar, hID[0]), "hID", 
    "string", 1, 47, 2, 1, 0, 0, 0},

  {"{P}{HSC}vID", (void *)OFFSET(struct UserVar, vID[0]), "vID", 
    "string", 1, 48, 3, 1, 0, 0, 0},

  {"{P}{HSC}hOrient", (void *)OFFSET(struct UserVar, hOrient), "hOrient", 
    "short", 1, 49, 4, 1, 0, 0, 0},

  {"{P}{HSC}vOrient", (void *)OFFSET(struct UserVar, vOrient), "vOrient", 
    "short", 1, 50, 5, 1, 0, 0, 0},

  {"{P}{HSC}hBusy", (void *)OFFSET(struct UserVar, hBusy), "hBusy", 
    "short", 1, 51, 0, 0, 0, 0, 0},

  {"{P}{HSC}h_Busy", (void *)OFFSET(struct UserVar, h_Busy), "h_Busy", 
    "short", 1, 52, 0, 1, 0, 0, 0},

  {"{P}{HSC}h_BusyOff.PROC", (void *)OFFSET(struct UserVar, h_BusyOff), "h_BusyOff", 
    "short", 1, 53, 0, 0, 0, 0, 0},

  {"{P}{HSC}h_BusyDisable", (void *)OFFSET(struct UserVar, h_BusyDisable), "h_BusyDisable", 
    "short", 1, 54, 0, 0, 0, 0, 0},

  {"{P}{HSC}vBusy", (void *)OFFSET(struct UserVar, vBusy), "vBusy", 
    "short", 1, 55, 0, 0, 0, 0, 0},

  {"{P}{HSC}v_Busy", (void *)OFFSET(struct UserVar, v_Busy), "v_Busy", 
    "short", 1, 56, 0, 1, 0, 0, 0},

  {"{P}{HSC}v_BusyOff.PROC", (void *)OFFSET(struct UserVar, v_BusyOff), "v_BusyOff", 
    "short", 1, 57, 0, 0, 0, 0, 0},

  {"{P}{HSC}v_BusyDisable", (void *)OFFSET(struct UserVar, v_BusyDisable), "v_BusyDisable", 
    "short", 1, 58, 0, 0, 0, 0, 0},

  {"{P}{HSC}stop", (void *)OFFSET(struct UserVar, stop), "stop", 
    "short", 1, 59, 6, 1, 0, 0, 0},

  {"{P}{HSC}locate", (void *)OFFSET(struct UserVar, locate), "locate", 
    "short", 1, 60, 0, 1, 0, 0, 0},

  {"{P}{HSC}init", (void *)OFFSET(struct UserVar, init), "init", 
    "short", 1, 61, 0, 1, 0, 0, 0},

  {"{P}{HSC}enable", (void *)OFFSET(struct UserVar, enable), "enable", 
    "short", 1, 62, 0, 1, 0, 0, 0},

  {"{P}{HSC}calib", (void *)OFFSET(struct UserVar, calibrate), "calibrate", 
    "short", 1, 63, 0, 1, 0, 0, 0},

  {"{P}{HSC}hOuterLimitLI", (void *)OFFSET(struct UserVar, hOuterLimit), "hOuterLimit", 
    "int", 1, 64, 0, 1, 0, 0, 0},

  {"{P}{HSC}vOuterLimitLI", (void *)OFFSET(struct UserVar, vOuterLimit), "vOuterLimit", 
    "int", 1, 65, 0, 1, 0, 0, 0},

  {"{P}{HSC}hOriginLI", (void *)OFFSET(struct UserVar, hOrigin), "hOrigin", 
    "int", 1, 66, 0, 1, 0, 0, 0},

  {"{P}{HSC}vOriginLI", (void *)OFFSET(struct UserVar, vOrigin), "vOrigin", 
    "int", 1, 67, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStepDelayLI", (void *)OFFSET(struct UserVar, hStepDelay), "hStepDelay", 
    "int", 1, 68, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStepDelayLI", (void *)OFFSET(struct UserVar, vStepDelay), "vStepDelay", 
    "int", 1, 69, 0, 1, 0, 0, 0},

  {"{P}{HSC}hGearBacklashLI", (void *)OFFSET(struct UserVar, hGearBacklash), "hGearBacklash", 
    "int", 1, 70, 0, 1, 0, 0, 0},

  {"{P}{HSC}vGearBacklashLI", (void *)OFFSET(struct UserVar, vGearBacklash), "vGearBacklash", 
    "int", 1, 71, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatPwrMI", (void *)OFFSET(struct UserVar, hStPwr), "hStPwr", 
    "int", 1, 72, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatLmtsBI", (void *)OFFSET(struct UserVar, hStLmts), "hStLmts", 
    "int", 1, 73, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatBanBI", (void *)OFFSET(struct UserVar, hStBan), "hStBan", 
    "int", 1, 74, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatEchoBI", (void *)OFFSET(struct UserVar, hStEcho), "hStEcho", 
    "int", 1, 75, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatLockBI", (void *)OFFSET(struct UserVar, hStLock), "hStLock", 
    "int", 1, 76, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatAliasBI", (void *)OFFSET(struct UserVar, hStAlias), "hStAlias", 
    "int", 1, 77, 0, 1, 0, 0, 0},

  {"{P}{HSC}hStatTextBI", (void *)OFFSET(struct UserVar, hStText), "hStText", 
    "int", 1, 78, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatPwrMI", (void *)OFFSET(struct UserVar, vStPwr), "vStPwr", 
    "int", 1, 79, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatLmtsBI", (void *)OFFSET(struct UserVar, vStLmts), "vStLmts", 
    "int", 1, 80, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatBanBI", (void *)OFFSET(struct UserVar, vStBan), "vStBan", 
    "int", 1, 81, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatEchoBI", (void *)OFFSET(struct UserVar, vStEcho), "vStEcho", 
    "int", 1, 82, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatLockBI", (void *)OFFSET(struct UserVar, vStLock), "vStLock", 
    "int", 1, 83, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatAliasBI", (void *)OFFSET(struct UserVar, vStAlias), "vStAlias", 
    "int", 1, 84, 0, 1, 0, 0, 0},

  {"{P}{HSC}vStatTextBI", (void *)OFFSET(struct UserVar, vStText), "vStText", 
    "int", 1, 85, 0, 1, 0, 0, 0},

  {"{P}{HSC}t", (void *)OFFSET(struct UserVar, t), "t", 
    "double", 1, 86, 0, 1, 0, 0, 0},

  {"{P}{HSC}t_event", (void *)OFFSET(struct UserVar, t_event), "t_event", 
    "double", 1, 87, 7, 1, 0, 0, 0},

  {"{P}{HSC}t.HOPR", (void *)OFFSET(struct UserVar, t_hi), "t_hi", 
    "double", 1, 88, 0, 1, 0, 0, 0},

  {"{P}{HSC}t.LOPR", (void *)OFFSET(struct UserVar, t_lo), "t_lo", 
    "double", 1, 89, 0, 1, 0, 0, 0},

  {"{P}{HSC}tRB", (void *)OFFSET(struct UserVar, t_RB), "t_RB", 
    "double", 1, 90, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_t.B", (void *)OFFSET(struct UserVar, load_t), "load_t", 
    "double", 1, 91, 0, 0, 0, 0, 0},

  {"{P}{HSC}l", (void *)OFFSET(struct UserVar, l), "l", 
    "double", 1, 92, 0, 1, 0, 0, 0},

  {"{P}{HSC}l_event", (void *)OFFSET(struct UserVar, l_event), "l_event", 
    "double", 1, 93, 8, 1, 0, 0, 0},

  {"{P}{HSC}l.HOPR", (void *)OFFSET(struct UserVar, l_hi), "l_hi", 
    "double", 1, 94, 0, 1, 0, 0, 0},

  {"{P}{HSC}l.LOPR", (void *)OFFSET(struct UserVar, l_lo), "l_lo", 
    "double", 1, 95, 0, 1, 0, 0, 0},

  {"{P}{HSC}lRB", (void *)OFFSET(struct UserVar, l_RB), "l_RB", 
    "double", 1, 96, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_l.B", (void *)OFFSET(struct UserVar, load_l), "load_l", 
    "double", 1, 97, 0, 0, 0, 0, 0},

  {"{P}{HSC}b", (void *)OFFSET(struct UserVar, b), "b", 
    "double", 1, 98, 0, 1, 0, 0, 0},

  {"{P}{HSC}b_event", (void *)OFFSET(struct UserVar, b_event), "b_event", 
    "double", 1, 99, 9, 1, 0, 0, 0},

  {"{P}{HSC}b.HOPR", (void *)OFFSET(struct UserVar, b_hi), "b_hi", 
    "double", 1, 100, 0, 1, 0, 0, 0},

  {"{P}{HSC}b.LOPR", (void *)OFFSET(struct UserVar, b_lo), "b_lo", 
    "double", 1, 101, 0, 1, 0, 0, 0},

  {"{P}{HSC}bRB", (void *)OFFSET(struct UserVar, b_RB), "b_RB", 
    "double", 1, 102, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_b.B", (void *)OFFSET(struct UserVar, load_b), "load_b", 
    "double", 1, 103, 0, 0, 0, 0, 0},

  {"{P}{HSC}r", (void *)OFFSET(struct UserVar, r), "r", 
    "double", 1, 104, 0, 1, 0, 0, 0},

  {"{P}{HSC}r_event", (void *)OFFSET(struct UserVar, r_event), "r_event", 
    "double", 1, 105, 10, 1, 0, 0, 0},

  {"{P}{HSC}r.HOPR", (void *)OFFSET(struct UserVar, r_hi), "r_hi", 
    "double", 1, 106, 0, 1, 0, 0, 0},

  {"{P}{HSC}r.LOPR", (void *)OFFSET(struct UserVar, r_lo), "r_lo", 
    "double", 1, 107, 0, 1, 0, 0, 0},

  {"{P}{HSC}rRB", (void *)OFFSET(struct UserVar, r_RB), "r_RB", 
    "double", 1, 108, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_r.B", (void *)OFFSET(struct UserVar, load_r), "load_r", 
    "double", 1, 109, 0, 0, 0, 0, 0},

  {"{P}{HSC}h0", (void *)OFFSET(struct UserVar, h0), "h0", 
    "double", 1, 110, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0_event", (void *)OFFSET(struct UserVar, h0_event), "h0_event", 
    "double", 1, 111, 11, 1, 0, 0, 0},

  {"{P}{HSC}h0.HOPR", (void *)OFFSET(struct UserVar, h0_hi), "h0_hi", 
    "double", 1, 112, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0.LOPR", (void *)OFFSET(struct UserVar, h0_lo), "h0_lo", 
    "double", 1, 113, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0RB", (void *)OFFSET(struct UserVar, h0_RB), "h0_RB", 
    "double", 1, 114, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_h0.B", (void *)OFFSET(struct UserVar, load_h0), "load_h0", 
    "double", 1, 115, 0, 0, 0, 0, 0},

  {"{P}{HSC}v0", (void *)OFFSET(struct UserVar, v0), "v0", 
    "double", 1, 116, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0_event", (void *)OFFSET(struct UserVar, v0_event), "v0_event", 
    "double", 1, 117, 12, 1, 0, 0, 0},

  {"{P}{HSC}v0.HOPR", (void *)OFFSET(struct UserVar, v0_hi), "v0_hi", 
    "double", 1, 118, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0.LOPR", (void *)OFFSET(struct UserVar, v0_lo), "v0_lo", 
    "double", 1, 119, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0RB", (void *)OFFSET(struct UserVar, v0_RB), "v0_RB", 
    "double", 1, 120, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_v0.B", (void *)OFFSET(struct UserVar, load_v0), "load_v0", 
    "double", 1, 121, 0, 0, 0, 0, 0},

  {"{P}{HSC}width", (void *)OFFSET(struct UserVar, width), "width", 
    "double", 1, 122, 0, 1, 0, 0, 0},

  {"{P}{HSC}width_event", (void *)OFFSET(struct UserVar, width_event), "width_event", 
    "double", 1, 123, 13, 1, 0, 0, 0},

  {"{P}{HSC}width.HOPR", (void *)OFFSET(struct UserVar, width_hi), "width_hi", 
    "double", 1, 124, 0, 1, 0, 0, 0},

  {"{P}{HSC}width.LOPR", (void *)OFFSET(struct UserVar, width_lo), "width_lo", 
    "double", 1, 125, 0, 1, 0, 0, 0},

  {"{P}{HSC}widthRB", (void *)OFFSET(struct UserVar, width_RB), "width_RB", 
    "double", 1, 126, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_width.B", (void *)OFFSET(struct UserVar, load_width), "load_width", 
    "double", 1, 127, 0, 0, 0, 0, 0},

  {"{P}{HSC}height", (void *)OFFSET(struct UserVar, height), "height", 
    "double", 1, 128, 0, 1, 0, 0, 0},

  {"{P}{HSC}height_event", (void *)OFFSET(struct UserVar, height_event), "height_event", 
    "double", 1, 129, 14, 1, 0, 0, 0},

  {"{P}{HSC}height.HOPR", (void *)OFFSET(struct UserVar, height_hi), "height_hi", 
    "double", 1, 130, 0, 1, 0, 0, 0},

  {"{P}{HSC}height.LOPR", (void *)OFFSET(struct UserVar, height_lo), "height_lo", 
    "double", 1, 131, 0, 1, 0, 0, 0},

  {"{P}{HSC}heightRB", (void *)OFFSET(struct UserVar, height_RB), "height_RB", 
    "double", 1, 132, 0, 1, 0, 0, 0},

  {"{P}{HSC}load_height.B", (void *)OFFSET(struct UserVar, load_height), "load_height", 
    "double", 1, 133, 0, 0, 0, 0, 0},

  {"{P}{HSC}error", (void *)OFFSET(struct UserVar, error), "error", 
    "int", 1, 134, 0, 0, 0, 0, 0},

  {"{P}{HSC}errMsg", (void *)OFFSET(struct UserVar, errMsg[0]), "errMsg", 
    "string", 1, 135, 0, 0, 0, 0, 0},

};

/* Event masks for state set xiahsc */
	/* Event mask for state startup: */
static bitMask	EM_xiahsc_startup[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state disable: */
static bitMask	EM_xiahsc_disable[] = {
	0x00000000,
	0x40000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state comm_error: */
static bitMask	EM_xiahsc_comm_error[] = {
	0x00000000,
	0x20000020,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state init: */
static bitMask	EM_xiahsc_init[] = {
	0x0000000c,
	0x60000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state init_limits: */
static bitMask	EM_xiahsc_init_limits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state idle: */
static bitMask	EM_xiahsc_idle[] = {
	0x001fffc0,
	0xf8880000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state premove: */
static bitMask	EM_xiahsc_premove[] = {
	0x00007f80,
	0x00000000,
	0x10400000,
	0x04104104,
	0x00000001,
};
	/* Event mask for state process_response: */
static bitMask	EM_xiahsc_process_response[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state update_positions: */
static bitMask	EM_xiahsc_update_positions[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_xiahsc[] = {
	/* State "startup" */ {
	/* state name */       "startup",
	/* action function */ (ACTION_FUNC) A_xiahsc_startup,
	/* event function */  (EVENT_FUNC) E_xiahsc_startup,
	/* delay function */   (DELAY_FUNC) D_xiahsc_startup,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_startup,
	/* state options */   (0)},

	/* State "disable" */ {
	/* state name */       "disable",
	/* action function */ (ACTION_FUNC) A_xiahsc_disable,
	/* event function */  (EVENT_FUNC) E_xiahsc_disable,
	/* delay function */   (DELAY_FUNC) D_xiahsc_disable,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_disable,
	/* state options */   (0)},

	/* State "comm_error" */ {
	/* state name */       "comm_error",
	/* action function */ (ACTION_FUNC) A_xiahsc_comm_error,
	/* event function */  (EVENT_FUNC) E_xiahsc_comm_error,
	/* delay function */   (DELAY_FUNC) D_xiahsc_comm_error,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_comm_error,
	/* state options */   (0)},

	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_xiahsc_init,
	/* event function */  (EVENT_FUNC) E_xiahsc_init,
	/* delay function */   (DELAY_FUNC) D_xiahsc_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_init,
	/* state options */   (0)},

	/* State "init_limits" */ {
	/* state name */       "init_limits",
	/* action function */ (ACTION_FUNC) A_xiahsc_init_limits,
	/* event function */  (EVENT_FUNC) E_xiahsc_init_limits,
	/* delay function */   (DELAY_FUNC) D_xiahsc_init_limits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_init_limits,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_xiahsc_idle,
	/* event function */  (EVENT_FUNC) E_xiahsc_idle,
	/* delay function */   (DELAY_FUNC) D_xiahsc_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_idle,
	/* state options */   (0)},

	/* State "premove" */ {
	/* state name */       "premove",
	/* action function */ (ACTION_FUNC) A_xiahsc_premove,
	/* event function */  (EVENT_FUNC) E_xiahsc_premove,
	/* delay function */   (DELAY_FUNC) D_xiahsc_premove,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_premove,
	/* state options */   (0)},

	/* State "process_response" */ {
	/* state name */       "process_response",
	/* action function */ (ACTION_FUNC) A_xiahsc_process_response,
	/* event function */  (EVENT_FUNC) E_xiahsc_process_response,
	/* delay function */   (DELAY_FUNC) D_xiahsc_process_response,
	/* entry function */   (ENTRY_FUNC) I_xiahsc_process_response,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_process_response,
	/* state options */   (0)},

	/* State "update_positions" */ {
	/* state name */       "update_positions",
	/* action function */ (ACTION_FUNC) A_xiahsc_update_positions,
	/* event function */  (EVENT_FUNC) E_xiahsc_update_positions,
	/* delay function */   (DELAY_FUNC) D_xiahsc_update_positions,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_update_positions,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "xiahsc" */ {
	/* ss name */            "xiahsc",
	/* ptr to state block */ state_xiahsc,
	/* number of states */   9,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "name=hsc1,P=pfx:,HSC=hsc1:,S=pfx:serial9";

/* State Program table (global) */
struct seqProgram xia_slit = {
	/* magic number */       20000315,
	/* *name */              "xia_slit",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_DEBUG | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void xia_slitRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&xia_slit);
}
epicsExportRegistrar(xia_slitRegistrar);

