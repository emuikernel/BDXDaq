/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: xiahsc.i */

/* Event flags */
#define s_ainp_mon	1
#define hID_mon	2
#define vID_mon	3
#define hOrient_mon	4
#define vOrient_mon	5
#define stop_mon	6
#define t_mon	7
#define l_mon	8
#define b_mon	9
#define r_mon	10
#define h0_mon	11
#define v0_mon	12
#define width_mon	13
#define height_mon	14
#define start_H_move	15
#define start_V_move	16
#define wait_H_input	17
#define wait_V_input	18
#define new_H_RB	19
#define new_V_RB	20
#define move_h	21
#define move_v	22

/* Program "xiahsc" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 65
#define NUM_EVENTS 22
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT TRUE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram xiahsc;

/* Variable declarations */
struct UserVar {
	char	s_ainp[MAX_STRING_SIZE];
	char	s_aout[MAX_STRING_SIZE];
	short	s_baud;
	short	s_dbit;
	short	s_fctl;
	char	s_ieos[MAX_STRING_SIZE];
	short	s_ifmt;
	short	s_nord;
	short	s_nrrd;
	char	s_oeos[MAX_STRING_SIZE];
	short	s_ofmt;
	short	s_proc;
	short	s_prty;
	short	s_sbit;
	short	s_scan;
	int	s_sevr;
	short	s_tmod;
	double	s_tmot;
	char	port[MAX_STRING_SIZE];
	char	hID[MAX_STRING_SIZE];
	char	vID[MAX_STRING_SIZE];
	short	hOrient;
	short	vOrient;
	short	hBusy;
	short	vBusy;
	short	stop;
	short	locate;
	short	init;
	short	enable;
	short	calibrate;
	double	t;
	double	t_hi;
	double	t_lo;
	double	t_RB;
	double	l;
	double	l_hi;
	double	l_lo;
	double	l_RB;
	double	b;
	double	b_hi;
	double	b_lo;
	double	b_RB;
	double	r;
	double	r_hi;
	double	r_lo;
	double	r_RB;
	double	h0;
	double	h0_hi;
	double	h0_lo;
	double	h0_RB;
	double	v0;
	double	v0_hi;
	double	v0_lo;
	double	v0_RB;
	double	width;
	double	width_hi;
	double	width_lo;
	double	width_RB;
	double	height;
	double	height_hi;
	double	height_lo;
	double	height_RB;
	int	error;
	char	errMsg[MAX_STRING_SIZE];
	int	xiahscDebug;
	char	scratch[256];
	char	myChar;
	double	dScratch;
	long	numWords;
	char	word1[MAX_STRING_SIZE];
	char	word2[MAX_STRING_SIZE];
	char	word3[MAX_STRING_SIZE];
	char	word4[MAX_STRING_SIZE];
	char	word5[MAX_STRING_SIZE];
	char	word6[MAX_STRING_SIZE];
	long	pos_a;
	long	pos_b;
	double	pos_ag;
	double	pos_bg;
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
	int	init_delay;
	int	i;
	int	count;
	int	hOrigin;
	int	vOrigin;
	int	hOuterLimit;
	int	vOuterLimit;
	char	hsc_command[MAX_STRING_SIZE];
	char	hsc_response[MAX_STRING_SIZE];
	int	hsc_timeout;
};

/* C code definitions */
# line 193 "../xiahsc.st"
 #include <math.h>
# line 194 "../xiahsc.st"
 #include <string.h>
# line 195 "../xiahsc.st"
 #include <epicsThread.h>

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "startup" in state set "xiahsc" */

/* Delay function for state "startup" in state set "xiahsc" */
static void D_xiahsc_startup(SS_ID ssId, struct UserVar *pVar)
{
# line 377 "../xiahsc.st"
}

/* Event function for state "startup" in state set "xiahsc" */
static long E_xiahsc_startup(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 377 "../xiahsc.st"
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
# line 333 "../xiahsc.st"
			(pVar->xiahscDebug) = 0;
			seq_pvGet(ssId, 0 /* s_ainp */, 0);
			seq_pvGet(ssId, 1 /* s_aout */, 0);
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 27 /* init */, 0);
			}
# line 336 "../xiahsc.st"
			;
			(pVar->init_delay) = 1.0;
# line 342 "../xiahsc.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 18 /* port */, 0);
# line 345 "../xiahsc.st"
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
# line 364 "../xiahsc.st"
			(pVar->t_old) = (pVar->t);
# line 364 "../xiahsc.st"
			(pVar->b_old) = (pVar->b);
			(pVar->l_old) = (pVar->l);
# line 365 "../xiahsc.st"
			(pVar->r_old) = (pVar->r);
			(pVar->h0_old) = (pVar->h0);
# line 366 "../xiahsc.st"
			(pVar->width_old) = (pVar->width);
			(pVar->v0_old) = (pVar->v0);
# line 367 "../xiahsc.st"
			(pVar->height_old) = (pVar->height);
# line 370 "../xiahsc.st"
			(pVar->hOrigin) = (pVar->vOrigin) = 400;
			(pVar->hOuterLimit) = (pVar->vOuterLimit) = 4400;
# line 373 "../xiahsc.st"
			{
				(pVar->enable) = (1);
				seq_pvPut(ssId, 28 /* enable */, 0);
			}
# line 373 "../xiahsc.st"
			;
# line 375 "../xiahsc.st"
			{
				(pVar->s_tmot) = (.250);
				seq_pvPut(ssId, 17 /* s_tmot */, 0);
			}
# line 375 "../xiahsc.st"
			;
		}
		return;
	}
}
/* Code for state "disable" in state set "xiahsc" */

/* Delay function for state "disable" in state set "xiahsc" */
static void D_xiahsc_disable(SS_ID ssId, struct UserVar *pVar)
{
# line 386 "../xiahsc.st"
}

/* Event function for state "disable" in state set "xiahsc" */
static long E_xiahsc_disable(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 386 "../xiahsc.st"
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
# line 384 "../xiahsc.st"
			(pVar->init_delay) = 1.0;
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 27 /* init */, 0);
			}
# line 385 "../xiahsc.st"
			;
		}
		return;
	}
}
/* Code for state "comm_error" in state set "xiahsc" */

/* Delay function for state "comm_error" in state set "xiahsc" */
static void D_xiahsc_comm_error(SS_ID ssId, struct UserVar *pVar)
{
# line 398 "../xiahsc.st"
	seq_delayInit(ssId, 0, ((10 * 60.0)));
}

/* Event function for state "comm_error" in state set "xiahsc" */
static long E_xiahsc_comm_error(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 398 "../xiahsc.st"
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
# line 394 "../xiahsc.st"
			if ((pVar->xiahscDebug) >= 20)
			{
				printf("(%s,%d): ", "../xiahsc.st", 394);
				printf("comm_error: init=%d  s_sevr=%d", (pVar->init), (pVar->s_sevr));
				printf("\n");
			}
# line 395 "../xiahsc.st"
			(pVar->init_delay) = 1.0;
			(pVar->s_sevr) = 0;
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 27 /* init */, 0);
			}
# line 397 "../xiahsc.st"
			;
		}
		return;
	}
}
/* Code for state "init" in state set "xiahsc" */

/* Delay function for state "init" in state set "xiahsc" */
static void D_xiahsc_init(SS_ID ssId, struct UserVar *pVar)
{
# line 403 "../xiahsc.st"
# line 428 "../xiahsc.st"
# line 472 "../xiahsc.st"
	seq_delayInit(ssId, 0, ((pVar->init_delay)));
# line 606 "../xiahsc.st"
	seq_delayInit(ssId, 1, (0.1));
}

/* Event function for state "init" in state set "xiahsc" */
static long E_xiahsc_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 403 "../xiahsc.st"
	if (!(pVar->enable))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 428 "../xiahsc.st"
	if (seq_efTest(ssId, hID_mon) || seq_efTest(ssId, vID_mon))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 472 "../xiahsc.st"
	if ((pVar->init) && seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 2;
		return TRUE;
	}
# line 606 "../xiahsc.st"
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
# line 417 "../xiahsc.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 18 /* port */, 0);
# line 422 "../xiahsc.st"
			(pVar->init_delay) = 0;
			(pVar->init) = 1;
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
			seq_efClear(ssId, hOrient_mon);
			seq_efClear(ssId, vOrient_mon);
		}
		return;
	case 2:
		{
# line 433 "../xiahsc.st"
			(pVar->error) = 0;
# line 437 "../xiahsc.st"
			if (!strcmp((pVar->hID), (pVar->vID)))
			{
				{
					strcpy((pVar->errMsg), "H & V IDs must be different");
					seq_pvPut(ssId, 63 /* errMsg */, 0);
				}
				;
				(pVar->error)++;
			}
# line 443 "../xiahsc.st"
			if (sscanf((pVar->hID), "XIAHSC-%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
			{
				if (sscanf((pVar->hID), "%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
				{
					{
						strcpy((pVar->errMsg), "H ID not a valid HSC ID");
						seq_pvPut(ssId, 63 /* errMsg */, 0);
					}
					;
					(pVar->error)++;
				}
			}
# line 449 "../xiahsc.st"
			if (sscanf((pVar->vID), "XIAHSC-%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
			{
				if (sscanf((pVar->vID), "%c-%ld%s",  & (pVar->myChar),  & (pVar->numWords), (pVar->scratch)) != 2)
				{
					{
						strcpy((pVar->errMsg), "V ID not a valid HSC ID");
						seq_pvPut(ssId, 63 /* errMsg */, 0);
					}
					;
					(pVar->error)++;
				}
			}
# line 453 "../xiahsc.st"
			if (!strlen((pVar->hID)))
			{
				{
					strcpy((pVar->errMsg), "define H ID string");
					seq_pvPut(ssId, 63 /* errMsg */, 0);
				}
				;
				(pVar->error)++;
			}
# line 457 "../xiahsc.st"
			if (!strlen((pVar->vID)))
			{
				{
					strcpy((pVar->errMsg), "define V ID string");
					seq_pvPut(ssId, 63 /* errMsg */, 0);
				}
				;
				(pVar->error)++;
			}
# line 463 "../xiahsc.st"
			if ((pVar->error))
			{
				{
					(pVar->error) = (17);
					seq_pvPut(ssId, 62 /* error */, 0);
				}
				;
			}
# line 466 "../xiahsc.st"
			else
			{
				{
					(pVar->init) = (0);
					seq_pvPut(ssId, 27 /* init */, 0);
				}
				;
				{
					strcpy((pVar->errMsg), "no error");
					seq_pvPut(ssId, 63 /* errMsg */, 0);
				}
				;
			}
# line 467 "../xiahsc.st"
			seq_pvPut(ssId, 62 /* error */, 0);
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
# line 471 "../xiahsc.st"
			(pVar->init_delay) = 30;
		}
		return;
	case 3:
		{
# line 474 "../xiahsc.st"
			{
				(pVar->hBusy) = (0);
				seq_pvPut(ssId, 23 /* hBusy */, 0);
			}
# line 474 "../xiahsc.st"
			;
			{
				(pVar->vBusy) = (0);
				seq_pvPut(ssId, 24 /* vBusy */, 0);
			}
# line 475 "../xiahsc.st"
			;
			{
				(pVar->error) = (0);
				seq_pvPut(ssId, 62 /* error */, 0);
			}
# line 476 "../xiahsc.st"
			;
# line 481 "../xiahsc.st"
			{
				{
					(pVar->s_baud) = (6);
					seq_pvPut(ssId, 2 /* s_baud */, 0);
				}
				;
				{
					(pVar->s_dbit) = (4);
					seq_pvPut(ssId, 3 /* s_dbit */, 0);
				}
				;
				{
					(pVar->s_sbit) = (1);
					seq_pvPut(ssId, 13 /* s_sbit */, 0);
				}
				;
				{
					(pVar->s_prty) = (1);
					seq_pvPut(ssId, 12 /* s_prty */, 0);
				}
				;
				{
					(pVar->s_fctl) = (1);
					seq_pvPut(ssId, 4 /* s_fctl */, 0);
				}
				;
			}
# line 481 "../xiahsc.st"
			;
# line 486 "../xiahsc.st"
			{
				strcpy((pVar->s_ieos), "\n");
				seq_pvPut(ssId, 5 /* s_ieos */, 0);
			}
# line 486 "../xiahsc.st"
			;
			{
				(pVar->s_ifmt) = (0);
				seq_pvPut(ssId, 6 /* s_ifmt */, 0);
			}
# line 487 "../xiahsc.st"
			;
			{
				(pVar->s_nrrd) = (0);
				seq_pvPut(ssId, 8 /* s_nrrd */, 0);
			}
# line 488 "../xiahsc.st"
			;
# line 490 "../xiahsc.st"
			{
				strcpy((pVar->s_oeos), "\r");
				seq_pvPut(ssId, 9 /* s_oeos */, 0);
			}
# line 490 "../xiahsc.st"
			;
			{
				(pVar->s_ofmt) = (0);
				seq_pvPut(ssId, 10 /* s_ofmt */, 0);
			}
# line 491 "../xiahsc.st"
			;
			{
				(pVar->s_tmod) = (0);
				seq_pvPut(ssId, 16 /* s_tmod */, 0);
			}
# line 492 "../xiahsc.st"
			;
			{
				(pVar->s_scan) = (0);
				seq_pvPut(ssId, 14 /* s_scan */, 0);
			}
# line 493 "../xiahsc.st"
			;
# line 495 "../xiahsc.st"
			{
				(pVar->s_tmot) = (.250);
				seq_pvPut(ssId, 17 /* s_tmot */, 0);
			}
# line 495 "../xiahsc.st"
			;
# line 497 "../xiahsc.st"
			seq_efClear(ssId, s_ainp_mon);
# line 499 "../xiahsc.st"
			seq_efClear(ssId, wait_H_input);
			seq_efClear(ssId, wait_V_input);
# line 502 "../xiahsc.st"
			seq_efClear(ssId, move_h);
			seq_efClear(ssId, move_v);
			seq_efClear(ssId, new_H_RB);
			seq_efClear(ssId, new_V_RB);
# line 507 "../xiahsc.st"
			{
				(pVar->stop) = (0);
				seq_pvPut(ssId, 25 /* stop */, 0);
			}
# line 507 "../xiahsc.st"
			;
			epicsThreadSleep(.02);
			seq_efClear(ssId, stop_mon);
# line 511 "../xiahsc.st"
			seq_efClear(ssId, t_mon);
# line 511 "../xiahsc.st"
			seq_efClear(ssId, l_mon);
			seq_efClear(ssId, b_mon);
# line 512 "../xiahsc.st"
			seq_efClear(ssId, r_mon);
			seq_efClear(ssId, h0_mon);
# line 513 "../xiahsc.st"
			seq_efClear(ssId, width_mon);
			seq_efClear(ssId, v0_mon);
# line 514 "../xiahsc.st"
			seq_efClear(ssId, height_mon);
			{
				(pVar->calibrate) = (0);
				seq_pvPut(ssId, 29 /* calibrate */, 0);
			}
# line 515 "../xiahsc.st"
			;
# line 517 "../xiahsc.st"
			seq_efClear(ssId, hID_mon);
			seq_efClear(ssId, vID_mon);
			seq_efClear(ssId, hOrient_mon);
			seq_efClear(ssId, vOrient_mon);
# line 526 "../xiahsc.st"
			{
				if (!(pVar->s_sevr))
				{
					sprintf((pVar->hsc_command), "!%s %s", "ALL", "K");
					if ((pVar->xiahscDebug) >= 10)
					{
						printf("(%s,%d): ", "../xiahsc.st", 526);
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
					}
					;
					{
						(pVar->s_scan) = (0);
						seq_pvPut(ssId, 14 /* s_scan */, 0);
					}
					;
					{
						(pVar->s_tmod) = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 0);
					}
					;
					{
						strcpy((pVar->s_ieos), "\n");
						seq_pvPut(ssId, 5 /* s_ieos */, 0);
					}
					;
					{
						strcpy((pVar->s_oeos), "\r");
						seq_pvPut(ssId, 9 /* s_oeos */, 0);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 1 /* s_aout */, 0);
					}
					;
				}
			}
# line 526 "../xiahsc.st"
			;
			epicsThreadSleep(.1);
# line 536 "../xiahsc.st"
			(pVar->error) = 0;
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), "R 1");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 537);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 537);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 537);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 537 "../xiahsc.st"
			;
# line 541 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hOuterLimit)) != 1)
			{
				(pVar->error)++;
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 540);
					printf("Could not read horizontal outer limit");
					printf("\n");
				}
				;
			}
# line 542 "../xiahsc.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), "R 2");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 542);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 542);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 542);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 542 "../xiahsc.st"
			;
# line 546 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->hOrigin)) != 1)
			{
				(pVar->error)++;
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 545);
					printf("Could not read horizontal origin position");
					printf("\n");
				}
				;
			}
# line 569 "../xiahsc.st"
			if (!(pVar->error))
			{
				(pVar->pos_ag) = (1.0 * (0 - (pVar->hOrigin)) / 400);
				(pVar->pos_bg) = (1.0 * ((pVar->hOuterLimit) - (pVar->hOrigin)) / 400);
				{
					(pVar->l_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 36 /* l_lo */, 0);
				}
				;
				{
					(pVar->l_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 35 /* l_hi */, 0);
				}
				;
				{
					(pVar->r_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 44 /* r_lo */, 0);
				}
				;
				{
					(pVar->r_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 43 /* r_hi */, 0);
				}
				;
				{
					if (((pVar->l_lo)) > ((pVar->r_lo)))
					{
						(pVar->dScratch) = ((pVar->l_lo));
					}
					else
					{
						(pVar->dScratch) = ((pVar->r_lo));
					}
				}
				;
				{
					if (((pVar->dScratch)) > (0.0))
					{
						(pVar->width_lo) = ((pVar->dScratch));
					}
					else
					{
						(pVar->width_lo) = (0.0);
					}
				}
				;
				seq_pvPut(ssId, 56 /* width_lo */, 0);
				{
					(pVar->width_hi) = ((pVar->l_hi) + (pVar->r_hi));
					seq_pvPut(ssId, 55 /* width_hi */, 0);
				}
				;
				{
					(pVar->h0_hi) = (((pVar->r_hi) - (pVar->l_lo)) / 2);
					seq_pvPut(ssId, 47 /* h0_hi */, 0);
				}
				;
				{
					(pVar->h0_lo) = (((pVar->r_lo) - (pVar->l_hi)) / 2);
					seq_pvPut(ssId, 48 /* h0_lo */, 0);
				}
				;
			}
# line 571 "../xiahsc.st"
			(pVar->error) = 0;
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), "R 1");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 572);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 572);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 572);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 572 "../xiahsc.st"
			;
# line 576 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vOuterLimit)) != 1)
			{
				(pVar->error)++;
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 575);
					printf("Could not read vertical outer limit");
					printf("\n");
				}
				;
			}
# line 577 "../xiahsc.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), "R 2");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 577);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 577);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 577);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 577 "../xiahsc.st"
			;
# line 581 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %d",  & (pVar->vOrigin)) != 1)
			{
				(pVar->error)++;
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 580);
					printf("Could not read vertical origin position");
					printf("\n");
				}
				;
			}
# line 603 "../xiahsc.st"
			if (!(pVar->error))
			{
				(pVar->pos_ag) = (1.0 * (0 - (pVar->vOrigin)) / 400);
				(pVar->pos_bg) = (1.0 * ((pVar->vOuterLimit) - (pVar->vOrigin)) / 400);
				{
					(pVar->t_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 32 /* t_lo */, 0);
				}
				;
				{
					(pVar->t_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 31 /* t_hi */, 0);
				}
				;
				{
					(pVar->b_lo) = ((pVar->pos_ag));
					seq_pvPut(ssId, 40 /* b_lo */, 0);
				}
				;
				{
					(pVar->b_hi) = ((pVar->pos_bg));
					seq_pvPut(ssId, 39 /* b_hi */, 0);
				}
				;
				{
					if (((pVar->t_lo)) > ((pVar->b_lo)))
					{
						(pVar->dScratch) = ((pVar->t_lo));
					}
					else
					{
						(pVar->dScratch) = ((pVar->b_lo));
					}
				}
				;
				{
					if (((pVar->dScratch)) > (0.0))
					{
						(pVar->height_lo) = ((pVar->dScratch));
					}
					else
					{
						(pVar->height_lo) = (0.0);
					}
				}
				;
				seq_pvPut(ssId, 60 /* height_lo */, 0);
				{
					(pVar->height_hi) = ((pVar->t_hi) + (pVar->b_hi));
					seq_pvPut(ssId, 59 /* height_hi */, 0);
				}
				;
				{
					(pVar->v0_hi) = (((pVar->t_hi) - (pVar->b_lo)) / 2);
					seq_pvPut(ssId, 51 /* v0_hi */, 0);
				}
				;
				{
					(pVar->v0_lo) = (((pVar->t_lo) - (pVar->b_hi)) / 2);
					seq_pvPut(ssId, 52 /* v0_lo */, 0);
				}
				;
			}
# line 604 "../xiahsc.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 604 "../xiahsc.st"
			;
		}
		return;
	}
}
/* Code for state "idle" in state set "xiahsc" */

/* Delay function for state "idle" in state set "xiahsc" */
static void D_xiahsc_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 612 "../xiahsc.st"
# line 617 "../xiahsc.st"
# line 618 "../xiahsc.st"
# line 619 "../xiahsc.st"
# line 622 "../xiahsc.st"
# line 634 "../xiahsc.st"
# line 643 "../xiahsc.st"
# line 696 "../xiahsc.st"
	seq_delayInit(ssId, 0, (0.1));
# line 710 "../xiahsc.st"
# line 722 "../xiahsc.st"
# line 740 "../xiahsc.st"
# line 758 "../xiahsc.st"
# line 762 "../xiahsc.st"
	seq_delayInit(ssId, 1, (0.25));
# line 765 "../xiahsc.st"
	seq_delayInit(ssId, 2, (5.0));
}

/* Event function for state "idle" in state set "xiahsc" */
static long E_xiahsc_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 612 "../xiahsc.st"
	if (!(pVar->enable))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 617 "../xiahsc.st"
	if ((pVar->s_sevr))
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
# line 618 "../xiahsc.st"
	if (seq_efTest(ssId, hID_mon) || seq_efTest(ssId, vID_mon))
	{
		*pNextState = 3;
		*pTransNum = 2;
		return TRUE;
	}
# line 619 "../xiahsc.st"
	if (seq_efTest(ssId, hOrient_mon) || seq_efTest(ssId, vOrient_mon))
	{
		*pNextState = 3;
		*pTransNum = 3;
		return TRUE;
	}
# line 622 "../xiahsc.st"
	if ((pVar->init))
	{
		*pNextState = 3;
		*pTransNum = 4;
		return TRUE;
	}
# line 634 "../xiahsc.st"
	if (seq_efTest(ssId, stop_mon) && (pVar->stop))
	{
		*pNextState = 4;
		*pTransNum = 5;
		return TRUE;
	}
# line 643 "../xiahsc.st"
	if (seq_efTest(ssId, l_mon) || seq_efTest(ssId, r_mon) || seq_efTest(ssId, t_mon) || seq_efTest(ssId, b_mon) || seq_efTest(ssId, h0_mon) || seq_efTest(ssId, height_mon) || seq_efTest(ssId, v0_mon) || seq_efTest(ssId, width_mon))
	{
		*pNextState = 6;
		*pTransNum = 6;
		return TRUE;
	}
# line 696 "../xiahsc.st"
	if ((seq_efTest(ssId, start_H_move) || seq_efTest(ssId, start_V_move)) && seq_delay(ssId, 0))
	{
		*pNextState = 4;
		*pTransNum = 7;
		return TRUE;
	}
# line 710 "../xiahsc.st"
	if ((pVar->locate))
	{
		*pNextState = 5;
		*pTransNum = 8;
		return TRUE;
	}
# line 722 "../xiahsc.st"
	if ((pVar->calibrate))
	{
		*pNextState = 4;
		*pTransNum = 9;
		return TRUE;
	}
# line 740 "../xiahsc.st"
	if (seq_efTest(ssId, new_H_RB))
	{
		*pNextState = 4;
		*pTransNum = 10;
		return TRUE;
	}
# line 758 "../xiahsc.st"
	if (seq_efTest(ssId, new_V_RB))
	{
		*pNextState = 4;
		*pTransNum = 11;
		return TRUE;
	}
# line 762 "../xiahsc.st"
	if (((pVar->hBusy) || (pVar->vBusy)) && seq_delay(ssId, 1))
	{
		*pNextState = 4;
		*pTransNum = 12;
		return TRUE;
	}
# line 765 "../xiahsc.st"
	if (seq_delay(ssId, 2))
	{
		*pNextState = 4;
		*pTransNum = 13;
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
# line 614 "../xiahsc.st"
			{
				(pVar->error) = (18);
				seq_pvPut(ssId, 62 /* error */, 0);
			}
# line 614 "../xiahsc.st"
			;
			sprintf((pVar->scratch), "communications error: %d", (pVar->s_sevr));
			{
				strcpy((pVar->errMsg), (pVar->scratch));
				seq_pvPut(ssId, 63 /* errMsg */, 0);
			}
# line 616 "../xiahsc.st"
			;
		}
		return;
	case 2:
		{
		}
		return;
	case 3:
		{
		}
		return;
	case 4:
		{
# line 622 "../xiahsc.st"
			(pVar->init_delay) = 1;
# line 622 "../xiahsc.st"
			{
				(pVar->init) = (1);
				seq_pvPut(ssId, 27 /* init */, 0);
			}
# line 622 "../xiahsc.st"
			;
		}
		return;
	case 5:
		{
# line 630 "../xiahsc.st"
			{
				if (!(pVar->s_sevr))
				{
					sprintf((pVar->hsc_command), "!%s %s", "ALL", "K");
					if ((pVar->xiahscDebug) >= 10)
					{
						printf("(%s,%d): ", "../xiahsc.st", 630);
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
					}
					;
					{
						(pVar->s_scan) = (0);
						seq_pvPut(ssId, 14 /* s_scan */, 0);
					}
					;
					{
						(pVar->s_tmod) = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 0);
					}
					;
					{
						strcpy((pVar->s_ieos), "\n");
						seq_pvPut(ssId, 5 /* s_ieos */, 0);
					}
					;
					{
						strcpy((pVar->s_oeos), "\r");
						seq_pvPut(ssId, 9 /* s_oeos */, 0);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 1 /* s_aout */, 0);
					}
					;
				}
			}
# line 631 "../xiahsc.st"
			epicsThreadSleep(.1);
			seq_efClear(ssId, stop_mon);
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 633 "../xiahsc.st"
			;
		}
		return;
	case 6:
		{
		}
		return;
	case 7:
		{
# line 671 "../xiahsc.st"
			if (seq_efTest(ssId, start_H_move))
			{
				if ((pVar->hBusy))
				{
					{
						if (!(pVar->s_sevr))
						{
							sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), "K");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 653);
								printf("WRITE_HSC: <%s>", (pVar->hsc_command));
								printf("\n");
							}
							;
							{
								(pVar->s_scan) = (0);
								seq_pvPut(ssId, 14 /* s_scan */, 0);
							}
							;
							{
								(pVar->s_tmod) = (1);
								seq_pvPut(ssId, 16 /* s_tmod */, 0);
							}
							;
							{
								strcpy((pVar->s_ieos), "\n");
								seq_pvPut(ssId, 5 /* s_ieos */, 0);
							}
							;
							{
								strcpy((pVar->s_oeos), "\r");
								seq_pvPut(ssId, 9 /* s_oeos */, 0);
							}
							;
							{
								strcpy((pVar->s_aout), (pVar->hsc_command));
								seq_pvPut(ssId, 1 /* s_aout */, 0);
							}
							;
						}
					}
					;
					epicsThreadSleep(.1);
				}
				else
				{
					{
						(pVar->hBusy) = (1);
						seq_pvPut(ssId, 23 /* hBusy */, 0);
					}
					;
				}
				if ((pVar->hOrient) == 0)
				{
					(pVar->pos_ag) = (pVar->l);
					(pVar->pos_bg) = (pVar->r);
				}
				else
				{
					(pVar->pos_ag) = (pVar->r);
					(pVar->pos_bg) = (pVar->l);
				}
				strcpy((pVar->word1), (pVar->hID));
				seq_efSet(ssId, wait_H_input);
				seq_efClear(ssId, start_H_move);
				(pVar->pos_a) = (((pVar->pos_ag)) * 400 + 0.5 + (pVar->hOrigin));
				(pVar->pos_b) = (((pVar->pos_bg)) * 400 + 0.5 + (pVar->hOrigin));
			}
# line 692 "../xiahsc.st"
			if (seq_efTest(ssId, start_V_move))
			{
				if ((pVar->vBusy))
				{
					{
						if (!(pVar->s_sevr))
						{
							sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), "K");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 674);
								printf("WRITE_HSC: <%s>", (pVar->hsc_command));
								printf("\n");
							}
							;
							{
								(pVar->s_scan) = (0);
								seq_pvPut(ssId, 14 /* s_scan */, 0);
							}
							;
							{
								(pVar->s_tmod) = (1);
								seq_pvPut(ssId, 16 /* s_tmod */, 0);
							}
							;
							{
								strcpy((pVar->s_ieos), "\n");
								seq_pvPut(ssId, 5 /* s_ieos */, 0);
							}
							;
							{
								strcpy((pVar->s_oeos), "\r");
								seq_pvPut(ssId, 9 /* s_oeos */, 0);
							}
							;
							{
								strcpy((pVar->s_aout), (pVar->hsc_command));
								seq_pvPut(ssId, 1 /* s_aout */, 0);
							}
							;
						}
					}
					;
					epicsThreadSleep(.1);
				}
				else
				{
					{
						(pVar->vBusy) = (1);
						seq_pvPut(ssId, 24 /* vBusy */, 0);
					}
					;
				}
				if ((pVar->vOrient) == 0)
				{
					(pVar->pos_ag) = (pVar->t);
					(pVar->pos_bg) = (pVar->b);
				}
				else
				{
					(pVar->pos_ag) = (pVar->b);
					(pVar->pos_bg) = (pVar->t);
				}
				strcpy((pVar->word1), (pVar->vID));
				seq_efSet(ssId, wait_V_input);
				seq_efClear(ssId, start_V_move);
				(pVar->pos_a) = (((pVar->pos_ag)) * 400 + 0.5 + (pVar->vOrigin));
				(pVar->pos_b) = (((pVar->pos_bg)) * 400 + 0.5 + (pVar->vOrigin));
			}
# line 693 "../xiahsc.st"
			sprintf((pVar->scratch), "M %ld %ld", (pVar->pos_a), (pVar->pos_b));
			{
				if (!(pVar->s_sevr))
				{
					sprintf((pVar->hsc_command), "!%s %s", (pVar->word1), (pVar->scratch));
					if ((pVar->xiahscDebug) >= 10)
					{
						printf("(%s,%d): ", "../xiahsc.st", 694);
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
					}
					;
					{
						(pVar->s_scan) = (0);
						seq_pvPut(ssId, 14 /* s_scan */, 0);
					}
					;
					{
						(pVar->s_tmod) = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 0);
					}
					;
					{
						strcpy((pVar->s_ieos), "\n");
						seq_pvPut(ssId, 5 /* s_ieos */, 0);
					}
					;
					{
						strcpy((pVar->s_oeos), "\r");
						seq_pvPut(ssId, 9 /* s_oeos */, 0);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 1 /* s_aout */, 0);
					}
					;
				}
			}
# line 694 "../xiahsc.st"
			;
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 695 "../xiahsc.st"
			;
		}
		return;
	case 8:
		{
# line 707 "../xiahsc.st"
			strcpy((pVar->port), seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 18 /* port */, 0);
			{
				(pVar->locate) = (0);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 709 "../xiahsc.st"
			;
		}
		return;
	case 9:
		{
# line 718 "../xiahsc.st"
			{
				if (!(pVar->s_sevr))
				{
					sprintf((pVar->hsc_command), "!%s %s", "ALL", "0 I");
					if ((pVar->xiahscDebug) >= 10)
					{
						printf("(%s,%d): ", "../xiahsc.st", 718);
						printf("WRITE_HSC: <%s>", (pVar->hsc_command));
						printf("\n");
					}
					;
					{
						(pVar->s_scan) = (0);
						seq_pvPut(ssId, 14 /* s_scan */, 0);
					}
					;
					{
						(pVar->s_tmod) = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 0);
					}
					;
					{
						strcpy((pVar->s_ieos), "\n");
						seq_pvPut(ssId, 5 /* s_ieos */, 0);
					}
					;
					{
						strcpy((pVar->s_oeos), "\r");
						seq_pvPut(ssId, 9 /* s_oeos */, 0);
					}
					;
					{
						strcpy((pVar->s_aout), (pVar->hsc_command));
						seq_pvPut(ssId, 1 /* s_aout */, 0);
					}
					;
				}
			}
# line 719 "../xiahsc.st"
			epicsThreadSleep(.1);
			{
				(pVar->calibrate) = (0);
				seq_pvPut(ssId, 29 /* calibrate */, 0);
			}
# line 720 "../xiahsc.st"
			;
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 721 "../xiahsc.st"
			;
		}
		return;
	case 10:
		{
# line 725 "../xiahsc.st"
			seq_efClear(ssId, new_H_RB);
# line 739 "../xiahsc.st"
			if (!(pVar->hBusy))
			{
				if ((pVar->l) != (pVar->l_RB))
				{
					(pVar->l) = ((pVar->l_RB));
					seq_pvPut(ssId, 34 /* l */, 0);
				}
				;
				if ((pVar->r) != (pVar->r_RB))
				{
					(pVar->r) = ((pVar->r_RB));
					seq_pvPut(ssId, 42 /* r */, 0);
				}
				;
				if ((pVar->h0) != (pVar->h0_RB))
				{
					(pVar->h0) = ((pVar->h0_RB));
					seq_pvPut(ssId, 46 /* h0 */, 0);
				}
				;
				if ((pVar->width) != (pVar->width_RB))
				{
					(pVar->width) = ((pVar->width_RB));
					seq_pvPut(ssId, 54 /* width */, 0);
				}
				;
				epicsThreadSleep(.02);
				seq_efClear(ssId, l_mon);
				seq_efClear(ssId, r_mon);
				seq_efClear(ssId, h0_mon);
				seq_efClear(ssId, width_mon);
				seq_efClear(ssId, wait_H_input);
			}
		}
		return;
	case 11:
		{
# line 743 "../xiahsc.st"
			seq_efClear(ssId, new_V_RB);
# line 757 "../xiahsc.st"
			if (!(pVar->vBusy))
			{
				if ((pVar->t) != (pVar->t_RB))
				{
					(pVar->t) = ((pVar->t_RB));
					seq_pvPut(ssId, 30 /* t */, 0);
				}
				;
				if ((pVar->b) != (pVar->b_RB))
				{
					(pVar->b) = ((pVar->b_RB));
					seq_pvPut(ssId, 38 /* b */, 0);
				}
				;
				if ((pVar->v0) != (pVar->v0_RB))
				{
					(pVar->v0) = ((pVar->v0_RB));
					seq_pvPut(ssId, 50 /* v0 */, 0);
				}
				;
				if ((pVar->height) != (pVar->height_RB))
				{
					(pVar->height) = ((pVar->height_RB));
					seq_pvPut(ssId, 58 /* height */, 0);
				}
				;
				epicsThreadSleep(.02);
				seq_efClear(ssId, t_mon);
				seq_efClear(ssId, b_mon);
				seq_efClear(ssId, v0_mon);
				seq_efClear(ssId, height_mon);
				seq_efClear(ssId, wait_V_input);
			}
		}
		return;
	case 12:
		{
# line 761 "../xiahsc.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 761 "../xiahsc.st"
			;
		}
		return;
	case 13:
		{
# line 764 "../xiahsc.st"
			{
				(pVar->locate) = (1);
				seq_pvPut(ssId, 26 /* locate */, 0);
			}
# line 764 "../xiahsc.st"
			;
		}
		return;
	}
}
/* Code for state "get_readback_positions" in state set "xiahsc" */

/* Delay function for state "get_readback_positions" in state set "xiahsc" */
static void D_xiahsc_get_readback_positions(SS_ID ssId, struct UserVar *pVar)
{
# line 771 "../xiahsc.st"
# line 849 "../xiahsc.st"
}

/* Event function for state "get_readback_positions" in state set "xiahsc" */
static long E_xiahsc_get_readback_positions(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 771 "../xiahsc.st"
	if (!(pVar->enable))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 849 "../xiahsc.st"
	if (TRUE)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "get_readback_positions" in state set "xiahsc" */
static void A_xiahsc_get_readback_positions(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
# line 777 "../xiahsc.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->hID), "P");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 777);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 777);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 777);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 777 "../xiahsc.st"
			;
# line 801 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %ld %ld",  & (pVar->pos_a),  & (pVar->pos_b)) == 2)
			{
				(pVar->pos_ag) = (1.0 * ((pVar->pos_a) - (pVar->hOrigin)) / 400);
				(pVar->pos_bg) = (1.0 * ((pVar->pos_b) - (pVar->hOrigin)) / 400);
				if ((pVar->hOrient) == 0)
				{
					if ((pVar->pos_ag) != (pVar->l_RB))
					{
						{
							(pVar->l_RB) = ((pVar->pos_ag));
							seq_pvPut(ssId, 37 /* l_RB */, 0);
						}
					}
					;
					if ((pVar->pos_bg) != (pVar->r_RB))
					{
						{
							(pVar->r_RB) = ((pVar->pos_bg));
							seq_pvPut(ssId, 45 /* r_RB */, 0);
						}
					}
					;
				}
				else
				{
					if ((pVar->pos_ag) != (pVar->r_RB))
					{
						{
							(pVar->r_RB) = ((pVar->pos_ag));
							seq_pvPut(ssId, 45 /* r_RB */, 0);
						}
					}
					;
					if ((pVar->pos_bg) != (pVar->l_RB))
					{
						{
							(pVar->l_RB) = ((pVar->pos_bg));
							seq_pvPut(ssId, 37 /* l_RB */, 0);
						}
					}
					;
				}
				if ((pVar->hBusy))
				{
					(pVar->hBusy) = (0);
					seq_pvPut(ssId, 23 /* hBusy */, 0);
				}
				;
				{
					(pVar->width_RB) = ((pVar->l_RB) + (pVar->r_RB));
					seq_pvPut(ssId, 57 /* width_RB */, 0);
				}
				;
				{
					(pVar->h0_RB) = (((pVar->r_RB) - (pVar->l_RB)) / 2);
					seq_pvPut(ssId, 49 /* h0_RB */, 0);
				}
				;
				seq_efSet(ssId, new_H_RB);
				(pVar->l_old) = (pVar->l_RB);
				(pVar->r_old) = (pVar->r_RB);
				(pVar->h0_old) = (pVar->h0_RB);
				(pVar->width_old) = (pVar->width_RB);
			}
# line 809 "../xiahsc.st"
			else
			if (sscanf((pVar->hsc_response), "%*s %*s %s", (pVar->scratch)) == 1)
			{
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 803);
					printf("axis=%s, command=<%s>, response=%s", "H", (pVar->hsc_command), (pVar->hsc_response));
					printf("\n");
				}
				;
				if (!strcmp((pVar->scratch), "BUSY;"))
				{
					if (!(pVar->hBusy))
					{
						(pVar->hBusy) = (1);
						seq_pvPut(ssId, 23 /* hBusy */, 0);
					}
					;
				}
				else
				{
					if ((pVar->xiahscDebug) >= 20)
					{
						printf("(%s,%d): ", "../xiahsc.st", 807);
						printf("could not understand %s", (pVar->hsc_response));
						printf("\n");
					}
					;
				}
			}
# line 815 "../xiahsc.st"
			{
				seq_efClear(ssId, s_ainp_mon);
				(pVar->hsc_timeout) = 1;
				{
					if (!(pVar->s_sevr))
					{
						sprintf((pVar->hsc_command), "!%s %s", (pVar->vID), "P");
						if ((pVar->xiahscDebug) >= 10)
						{
							printf("(%s,%d): ", "../xiahsc.st", 815);
							printf("WRITE_HSC: <%s>", (pVar->hsc_command));
							printf("\n");
						}
						;
						{
							(pVar->s_scan) = (0);
							seq_pvPut(ssId, 14 /* s_scan */, 0);
						}
						;
						{
							(pVar->s_tmod) = (0);
							seq_pvPut(ssId, 16 /* s_tmod */, 0);
						}
						;
						{
							strcpy((pVar->s_ieos), "\n");
							seq_pvPut(ssId, 5 /* s_ieos */, 0);
						}
						;
						{
							strcpy((pVar->s_oeos), "\r");
							seq_pvPut(ssId, 9 /* s_oeos */, 0);
						}
						;
						{
							strcpy((pVar->s_aout), (pVar->hsc_command));
							seq_pvPut(ssId, 1 /* s_aout */, 0);
						}
						;
					}
				}
				;
				{
					if (!(pVar->s_sevr))
					{
						for ((pVar->i) = 15; (pVar->i); (pVar->i)--)
						{
							if (seq_efTest(ssId, s_ainp_mon))
							{
								(pVar->hsc_timeout) = 0;
								break;
							}
							epicsThreadSleep(.02);
						}
						;
						if ((pVar->hsc_timeout) == 1)
						{
							strcpy((pVar->hsc_response), "");
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 815);
								printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", (pVar->hsc_command));
								printf("\n");
							}
							;
						}
						else
						{
							strcpy((pVar->hsc_response), (pVar->s_ainp));
							if ((pVar->xiahscDebug) >= 10)
							{
								printf("(%s,%d): ", "../xiahsc.st", 815);
								printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s", (pVar->hsc_command), (pVar->s_nord), (pVar->hsc_response));
								printf("\n");
							}
							;
						}
						;
					}
				}
				;
			}
# line 815 "../xiahsc.st"
			;
# line 839 "../xiahsc.st"
			if (sscanf((pVar->hsc_response), "%*s %*s %ld %ld",  & (pVar->pos_a),  & (pVar->pos_b)) == 2)
			{
				(pVar->pos_ag) = (1.0 * ((pVar->pos_a) - (pVar->vOrigin)) / 400);
				(pVar->pos_bg) = (1.0 * ((pVar->pos_b) - (pVar->vOrigin)) / 400);
				if ((pVar->vOrient) == 0)
				{
					if ((pVar->pos_ag) != (pVar->t_RB))
					{
						{
							(pVar->t_RB) = ((pVar->pos_ag));
							seq_pvPut(ssId, 33 /* t_RB */, 0);
						}
					}
					;
					if ((pVar->pos_bg) != (pVar->b_RB))
					{
						{
							(pVar->b_RB) = ((pVar->pos_bg));
							seq_pvPut(ssId, 41 /* b_RB */, 0);
						}
					}
					;
				}
				else
				{
					if ((pVar->pos_ag) != (pVar->b_RB))
					{
						{
							(pVar->b_RB) = ((pVar->pos_ag));
							seq_pvPut(ssId, 41 /* b_RB */, 0);
						}
					}
					;
					if ((pVar->pos_bg) != (pVar->t_RB))
					{
						{
							(pVar->t_RB) = ((pVar->pos_bg));
							seq_pvPut(ssId, 33 /* t_RB */, 0);
						}
					}
					;
				}
				if ((pVar->vBusy))
				{
					(pVar->vBusy) = (0);
					seq_pvPut(ssId, 24 /* vBusy */, 0);
				}
				;
				{
					(pVar->height_RB) = ((pVar->t_RB) + (pVar->b_RB));
					seq_pvPut(ssId, 61 /* height_RB */, 0);
				}
				;
				{
					(pVar->v0_RB) = (((pVar->t_RB) - (pVar->b_RB)) / 2);
					seq_pvPut(ssId, 53 /* v0_RB */, 0);
				}
				;
				seq_efSet(ssId, new_V_RB);
				(pVar->t_old) = (pVar->t_RB);
				(pVar->b_old) = (pVar->b_RB);
				(pVar->v0_old) = (pVar->v0_RB);
				(pVar->height_old) = (pVar->height_RB);
			}
# line 847 "../xiahsc.st"
			else
			if (sscanf((pVar->hsc_response), "%*s %*s %s", (pVar->scratch)) == 1)
			{
				if ((pVar->xiahscDebug) >= 5)
				{
					printf("(%s,%d): ", "../xiahsc.st", 841);
					printf("axis=%s, command=<%s>, response=%s", "V", (pVar->hsc_command), (pVar->hsc_response));
					printf("\n");
				}
				;
				if (!strcmp((pVar->scratch), "BUSY;"))
				{
					if (!(pVar->vBusy))
					{
						(pVar->vBusy) = (1);
						seq_pvPut(ssId, 24 /* vBusy */, 0);
					}
					;
				}
				else
				{
					if ((pVar->xiahscDebug) >= 20)
					{
						printf("(%s,%d): ", "../xiahsc.st", 845);
						printf("could not understand %s", (pVar->hsc_response));
						printf("\n");
					}
					;
				}
			}
		}
		return;
	}
}
/* Code for state "premove" in state set "xiahsc" */

/* Delay function for state "premove" in state set "xiahsc" */
static void D_xiahsc_premove(SS_ID ssId, struct UserVar *pVar)
{
# line 857 "../xiahsc.st"
# line 858 "../xiahsc.st"
# line 859 "../xiahsc.st"
# line 860 "../xiahsc.st"
# line 861 "../xiahsc.st"
# line 862 "../xiahsc.st"
# line 864 "../xiahsc.st"
# line 866 "../xiahsc.st"
# line 953 "../xiahsc.st"
}

/* Event function for state "premove" in state set "xiahsc" */
static long E_xiahsc_premove(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 857 "../xiahsc.st"
	if (seq_efTest(ssId, l_mon) && ((pVar->l) == (pVar->l_old)))
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
# line 858 "../xiahsc.st"
	if (seq_efTest(ssId, r_mon) && ((pVar->r) == (pVar->r_old)))
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
# line 859 "../xiahsc.st"
	if (seq_efTest(ssId, t_mon) && ((pVar->t) == (pVar->t_old)))
	{
		*pNextState = 4;
		*pTransNum = 2;
		return TRUE;
	}
# line 860 "../xiahsc.st"
	if (seq_efTest(ssId, b_mon) && ((pVar->b) == (pVar->b_old)))
	{
		*pNextState = 4;
		*pTransNum = 3;
		return TRUE;
	}
# line 861 "../xiahsc.st"
	if (seq_efTest(ssId, h0_mon) && ((pVar->h0) == (pVar->h0_old)))
	{
		*pNextState = 4;
		*pTransNum = 4;
		return TRUE;
	}
# line 862 "../xiahsc.st"
	if (seq_efTest(ssId, v0_mon) && ((pVar->v0) == (pVar->v0_old)))
	{
		*pNextState = 4;
		*pTransNum = 5;
		return TRUE;
	}
# line 864 "../xiahsc.st"
	if (seq_efTest(ssId, width_mon) && ((pVar->width) == (pVar->width_old)))
	{
		*pNextState = 4;
		*pTransNum = 6;
		return TRUE;
	}
# line 866 "../xiahsc.st"
	if (seq_efTest(ssId, height_mon) && ((pVar->height) == (pVar->height_old)))
	{
		*pNextState = 4;
		*pTransNum = 7;
		return TRUE;
	}
# line 953 "../xiahsc.st"
	if (TRUE)
	{
		*pNextState = 4;
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
# line 857 "../xiahsc.st"
			seq_efClear(ssId, l_mon);
		}
		return;
	case 1:
		{
			seq_efClear(ssId, r_mon);
		}
		return;
	case 2:
		{
			seq_efClear(ssId, t_mon);
		}
		return;
	case 3:
		{
			seq_efClear(ssId, b_mon);
		}
		return;
	case 4:
		{
			seq_efClear(ssId, h0_mon);
		}
		return;
	case 5:
		{
			seq_efClear(ssId, v0_mon);
		}
		return;
	case 6:
		{
# line 864 "../xiahsc.st"
			seq_efClear(ssId, width_mon);
		}
		return;
	case 7:
		{
# line 866 "../xiahsc.st"
			seq_efClear(ssId, height_mon);
		}
		return;
	case 8:
		{
# line 910 "../xiahsc.st"
			if (seq_efTest(ssId, l_mon) || seq_efTest(ssId, r_mon) || seq_efTest(ssId, h0_mon) || seq_efTest(ssId, width_mon))
			{
				if (seq_efTest(ssId, l_mon) || seq_efTest(ssId, r_mon))
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
				if (!(((pVar->l_lo) <= (pVar->l)) && ((pVar->l) <= (pVar->l_hi))))
				{
					(pVar->l) = (pVar->l_old);
					(pVar->error)++;
				}
				if (!(((pVar->r_lo) <= (pVar->r)) && ((pVar->r) <= (pVar->r_hi))))
				{
					(pVar->r) = (pVar->r_old);
					(pVar->error)++;
				}
				if (!(((pVar->h0_lo) <= (pVar->h0)) && ((pVar->h0) <= (pVar->h0_hi))))
				{
					(pVar->h0) = (pVar->h0_old);
					(pVar->error)++;
				}
				if (!(((pVar->width_lo) <= (pVar->width)) && ((pVar->width) <= (pVar->width_hi))))
				{
					(pVar->width) = (pVar->width_old);
					(pVar->error)++;
				}
				if ((pVar->error))
				{
					if (seq_efTest(ssId, l_mon))
						seq_pvPut(ssId, 34 /* l */, 0);
					if (seq_efTest(ssId, r_mon))
						seq_pvPut(ssId, 42 /* r */, 0);
					if (seq_efTest(ssId, h0_mon))
						seq_pvPut(ssId, 46 /* h0 */, 0);
					if (seq_efTest(ssId, width_mon))
						seq_pvPut(ssId, 54 /* width */, 0);
					(pVar->error) = 15;
					{
						strcpy((pVar->errMsg), "H soft limits exceeded");
						seq_pvPut(ssId, 63 /* errMsg */, 0);
					}
					;
				}
				else
				{
					{
						strcpy((pVar->errMsg), "no error");
						seq_pvPut(ssId, 63 /* errMsg */, 0);
					}
					;
					seq_efSet(ssId, start_H_move);
					if (seq_efTest(ssId, l_mon) || seq_efTest(ssId, r_mon))
					{
						seq_pvPut(ssId, 46 /* h0 */, 0);
						seq_pvPut(ssId, 54 /* width */, 0);
					}
					else
					{
						seq_pvPut(ssId, 34 /* l */, 0);
						seq_pvPut(ssId, 42 /* r */, 0);
					}
				}
				epicsThreadSleep(.02);
				seq_efClear(ssId, l_mon);
				seq_efClear(ssId, r_mon);
				seq_efClear(ssId, h0_mon);
				seq_efClear(ssId, width_mon);
				seq_pvPut(ssId, 62 /* error */, 0);
			}
# line 952 "../xiahsc.st"
			if (seq_efTest(ssId, t_mon) || seq_efTest(ssId, b_mon) || seq_efTest(ssId, v0_mon) || seq_efTest(ssId, height_mon))
			{
				if (seq_efTest(ssId, t_mon) || seq_efTest(ssId, b_mon))
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
				if (!(((pVar->t_lo) <= (pVar->t)) && ((pVar->t) <= (pVar->t_hi))))
				{
					(pVar->t) = (pVar->t_old);
					(pVar->error)++;
				}
				if (!(((pVar->b_lo) <= (pVar->b)) && ((pVar->b) <= (pVar->b_hi))))
				{
					(pVar->b) = (pVar->b_old);
					(pVar->error)++;
				}
				if (!(((pVar->v0_lo) <= (pVar->v0)) && ((pVar->v0) <= (pVar->v0_hi))))
				{
					(pVar->v0) = (pVar->v0_old);
					(pVar->error)++;
				}
				if (!(((pVar->height_lo) <= (pVar->height)) && ((pVar->height) <= (pVar->height_hi))))
				{
					(pVar->height) = (pVar->height_old);
					(pVar->error)++;
				}
				if ((pVar->error))
				{
					if (seq_efTest(ssId, t_mon))
						seq_pvPut(ssId, 30 /* t */, 0);
					if (seq_efTest(ssId, b_mon))
						seq_pvPut(ssId, 38 /* b */, 0);
					if (seq_efTest(ssId, v0_mon))
						seq_pvPut(ssId, 50 /* v0 */, 0);
					if (seq_efTest(ssId, height_mon))
						seq_pvPut(ssId, 58 /* height */, 0);
					(pVar->error) = 15;
					{
						strcpy((pVar->errMsg), "V soft limits exceeded");
						seq_pvPut(ssId, 63 /* errMsg */, 0);
					}
					;
				}
				else
				{
					seq_efSet(ssId, start_V_move);
					if (seq_efTest(ssId, t_mon) || seq_efTest(ssId, b_mon))
					{
						seq_pvPut(ssId, 50 /* v0 */, 0);
						seq_pvPut(ssId, 58 /* height */, 0);
					}
					else
					{
						seq_pvPut(ssId, 30 /* t */, 0);
						seq_pvPut(ssId, 38 /* b */, 0);
					}
				}
				epicsThreadSleep(.02);
				seq_efClear(ssId, t_mon);
				seq_efClear(ssId, b_mon);
				seq_efClear(ssId, v0_mon);
				seq_efClear(ssId, height_mon);
				seq_pvPut(ssId, 62 /* error */, 0);
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

  {"{S}.AOUT", (void *)OFFSET(struct UserVar, s_aout[0]), "s_aout", 
    "string", 1, 24, 0, 0, 0, 0, 0},

  {"{S}.BAUD", (void *)OFFSET(struct UserVar, s_baud), "s_baud", 
    "short", 1, 25, 0, 0, 0, 0, 0},

  {"{S}.DBIT", (void *)OFFSET(struct UserVar, s_dbit), "s_dbit", 
    "short", 1, 26, 0, 0, 0, 0, 0},

  {"{S}.FCTL", (void *)OFFSET(struct UserVar, s_fctl), "s_fctl", 
    "short", 1, 27, 0, 0, 0, 0, 0},

  {"{S}.IEOS", (void *)OFFSET(struct UserVar, s_ieos[0]), "s_ieos", 
    "string", 1, 28, 0, 0, 0, 0, 0},

  {"{S}.IFMT", (void *)OFFSET(struct UserVar, s_ifmt), "s_ifmt", 
    "short", 1, 29, 0, 0, 0, 0, 0},

  {"{S}.NORD", (void *)OFFSET(struct UserVar, s_nord), "s_nord", 
    "short", 1, 30, 0, 1, 0, 0, 0},

  {"{S}.NRRD", (void *)OFFSET(struct UserVar, s_nrrd), "s_nrrd", 
    "short", 1, 31, 0, 0, 0, 0, 0},

  {"{S}.OEOS", (void *)OFFSET(struct UserVar, s_oeos[0]), "s_oeos", 
    "string", 1, 32, 0, 0, 0, 0, 0},

  {"{S}.OFMT", (void *)OFFSET(struct UserVar, s_ofmt), "s_ofmt", 
    "short", 1, 33, 0, 0, 0, 0, 0},

  {"{S}.PROC", (void *)OFFSET(struct UserVar, s_proc), "s_proc", 
    "short", 1, 34, 0, 1, 0, 0, 0},

  {"{S}.PRTY", (void *)OFFSET(struct UserVar, s_prty), "s_prty", 
    "short", 1, 35, 0, 0, 0, 0, 0},

  {"{S}.SBIT", (void *)OFFSET(struct UserVar, s_sbit), "s_sbit", 
    "short", 1, 36, 0, 0, 0, 0, 0},

  {"{S}.SCAN", (void *)OFFSET(struct UserVar, s_scan), "s_scan", 
    "short", 1, 37, 0, 0, 0, 0, 0},

  {"{S}.SEVR", (void *)OFFSET(struct UserVar, s_sevr), "s_sevr", 
    "int", 1, 38, 0, 1, 0, 0, 0},

  {"{S}.TMOD", (void *)OFFSET(struct UserVar, s_tmod), "s_tmod", 
    "short", 1, 39, 0, 0, 0, 0, 0},

  {"{S}.TMOT", (void *)OFFSET(struct UserVar, s_tmot), "s_tmot", 
    "double", 1, 40, 0, 1, 0, 0, 0},

  {"{P}{HSC}port", (void *)OFFSET(struct UserVar, port[0]), "port", 
    "string", 1, 41, 0, 0, 0, 0, 0},

  {"{P}{HSC}hID", (void *)OFFSET(struct UserVar, hID[0]), "hID", 
    "string", 1, 42, 2, 1, 0, 0, 0},

  {"{P}{HSC}vID", (void *)OFFSET(struct UserVar, vID[0]), "vID", 
    "string", 1, 43, 3, 1, 0, 0, 0},

  {"{P}{HSC}hOrient", (void *)OFFSET(struct UserVar, hOrient), "hOrient", 
    "short", 1, 44, 4, 1, 0, 0, 0},

  {"{P}{HSC}vOrient", (void *)OFFSET(struct UserVar, vOrient), "vOrient", 
    "short", 1, 45, 5, 1, 0, 0, 0},

  {"{P}{HSC}hBusy", (void *)OFFSET(struct UserVar, hBusy), "hBusy", 
    "short", 1, 46, 0, 0, 0, 0, 0},

  {"{P}{HSC}vBusy", (void *)OFFSET(struct UserVar, vBusy), "vBusy", 
    "short", 1, 47, 0, 0, 0, 0, 0},

  {"{P}{HSC}stop", (void *)OFFSET(struct UserVar, stop), "stop", 
    "short", 1, 48, 6, 1, 0, 0, 0},

  {"{P}{HSC}locate", (void *)OFFSET(struct UserVar, locate), "locate", 
    "short", 1, 49, 0, 1, 0, 0, 0},

  {"{P}{HSC}init", (void *)OFFSET(struct UserVar, init), "init", 
    "short", 1, 50, 0, 1, 0, 0, 0},

  {"{P}{HSC}enable", (void *)OFFSET(struct UserVar, enable), "enable", 
    "short", 1, 51, 0, 1, 0, 0, 0},

  {"{P}{HSC}calib", (void *)OFFSET(struct UserVar, calibrate), "calibrate", 
    "short", 1, 52, 0, 1, 0, 0, 0},

  {"{P}{HSC}t", (void *)OFFSET(struct UserVar, t), "t", 
    "double", 1, 53, 7, 1, 0, 0, 0},

  {"{P}{HSC}t.HOPR", (void *)OFFSET(struct UserVar, t_hi), "t_hi", 
    "double", 1, 54, 0, 1, 0, 0, 0},

  {"{P}{HSC}t.LOPR", (void *)OFFSET(struct UserVar, t_lo), "t_lo", 
    "double", 1, 55, 0, 1, 0, 0, 0},

  {"{P}{HSC}tRB", (void *)OFFSET(struct UserVar, t_RB), "t_RB", 
    "double", 1, 56, 0, 1, 0, 0, 0},

  {"{P}{HSC}l", (void *)OFFSET(struct UserVar, l), "l", 
    "double", 1, 57, 8, 1, 0, 0, 0},

  {"{P}{HSC}l.HOPR", (void *)OFFSET(struct UserVar, l_hi), "l_hi", 
    "double", 1, 58, 0, 1, 0, 0, 0},

  {"{P}{HSC}l.LOPR", (void *)OFFSET(struct UserVar, l_lo), "l_lo", 
    "double", 1, 59, 0, 1, 0, 0, 0},

  {"{P}{HSC}lRB", (void *)OFFSET(struct UserVar, l_RB), "l_RB", 
    "double", 1, 60, 0, 1, 0, 0, 0},

  {"{P}{HSC}b", (void *)OFFSET(struct UserVar, b), "b", 
    "double", 1, 61, 9, 1, 0, 0, 0},

  {"{P}{HSC}b.HOPR", (void *)OFFSET(struct UserVar, b_hi), "b_hi", 
    "double", 1, 62, 0, 1, 0, 0, 0},

  {"{P}{HSC}b.LOPR", (void *)OFFSET(struct UserVar, b_lo), "b_lo", 
    "double", 1, 63, 0, 1, 0, 0, 0},

  {"{P}{HSC}bRB", (void *)OFFSET(struct UserVar, b_RB), "b_RB", 
    "double", 1, 64, 0, 1, 0, 0, 0},

  {"{P}{HSC}r", (void *)OFFSET(struct UserVar, r), "r", 
    "double", 1, 65, 10, 1, 0, 0, 0},

  {"{P}{HSC}r.HOPR", (void *)OFFSET(struct UserVar, r_hi), "r_hi", 
    "double", 1, 66, 0, 1, 0, 0, 0},

  {"{P}{HSC}r.LOPR", (void *)OFFSET(struct UserVar, r_lo), "r_lo", 
    "double", 1, 67, 0, 1, 0, 0, 0},

  {"{P}{HSC}rRB", (void *)OFFSET(struct UserVar, r_RB), "r_RB", 
    "double", 1, 68, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0", (void *)OFFSET(struct UserVar, h0), "h0", 
    "double", 1, 69, 11, 1, 0, 0, 0},

  {"{P}{HSC}h0.HOPR", (void *)OFFSET(struct UserVar, h0_hi), "h0_hi", 
    "double", 1, 70, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0.LOPR", (void *)OFFSET(struct UserVar, h0_lo), "h0_lo", 
    "double", 1, 71, 0, 1, 0, 0, 0},

  {"{P}{HSC}h0RB", (void *)OFFSET(struct UserVar, h0_RB), "h0_RB", 
    "double", 1, 72, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0", (void *)OFFSET(struct UserVar, v0), "v0", 
    "double", 1, 73, 12, 1, 0, 0, 0},

  {"{P}{HSC}v0.HOPR", (void *)OFFSET(struct UserVar, v0_hi), "v0_hi", 
    "double", 1, 74, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0.LOPR", (void *)OFFSET(struct UserVar, v0_lo), "v0_lo", 
    "double", 1, 75, 0, 1, 0, 0, 0},

  {"{P}{HSC}v0RB", (void *)OFFSET(struct UserVar, v0_RB), "v0_RB", 
    "double", 1, 76, 0, 1, 0, 0, 0},

  {"{P}{HSC}width", (void *)OFFSET(struct UserVar, width), "width", 
    "double", 1, 77, 13, 1, 0, 0, 0},

  {"{P}{HSC}width.HOPR", (void *)OFFSET(struct UserVar, width_hi), "width_hi", 
    "double", 1, 78, 0, 1, 0, 0, 0},

  {"{P}{HSC}width.LOPR", (void *)OFFSET(struct UserVar, width_lo), "width_lo", 
    "double", 1, 79, 0, 1, 0, 0, 0},

  {"{P}{HSC}widthRB", (void *)OFFSET(struct UserVar, width_RB), "width_RB", 
    "double", 1, 80, 0, 1, 0, 0, 0},

  {"{P}{HSC}height", (void *)OFFSET(struct UserVar, height), "height", 
    "double", 1, 81, 14, 1, 0, 0, 0},

  {"{P}{HSC}height.HOPR", (void *)OFFSET(struct UserVar, height_hi), "height_hi", 
    "double", 1, 82, 0, 1, 0, 0, 0},

  {"{P}{HSC}height.LOPR", (void *)OFFSET(struct UserVar, height_lo), "height_lo", 
    "double", 1, 83, 0, 1, 0, 0, 0},

  {"{P}{HSC}heightRB", (void *)OFFSET(struct UserVar, height_RB), "height_RB", 
    "double", 1, 84, 0, 1, 0, 0, 0},

  {"{P}{HSC}error", (void *)OFFSET(struct UserVar, error), "error", 
    "int", 1, 85, 0, 0, 0, 0, 0},

  {"{P}{HSC}errMsg", (void *)OFFSET(struct UserVar, errMsg[0]), "errMsg", 
    "string", 1, 86, 0, 0, 0, 0, 0},

  {"{P}{HSC}debug", (void *)OFFSET(struct UserVar, xiahscDebug), "xiahscDebug", 
    "int", 1, 87, 0, 1, 0, 0, 0},

};

/* Event masks for state set xiahsc */
	/* Event mask for state startup: */
static bitMask	EM_xiahsc_startup[] = {
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state disable: */
static bitMask	EM_xiahsc_disable[] = {
	0x00000000,
	0x00080000,
	0x00000000,
};
	/* Event mask for state comm_error: */
static bitMask	EM_xiahsc_comm_error[] = {
	0x00000000,
	0x00040040,
	0x00000000,
};
	/* Event mask for state init: */
static bitMask	EM_xiahsc_init[] = {
	0x0000000c,
	0x000c0000,
	0x00000000,
};
	/* Event mask for state idle: */
static bitMask	EM_xiahsc_idle[] = {
	0x0019fffc,
	0x001fc040,
	0x00000000,
};
	/* Event mask for state get_readback_positions: */
static bitMask	EM_xiahsc_get_readback_positions[] = {
	0x00000000,
	0x00080000,
	0x00000000,
};
	/* Event mask for state premove: */
static bitMask	EM_xiahsc_premove[] = {
	0x00007f80,
	0x22200000,
	0x00022222,
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

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_xiahsc_idle,
	/* event function */  (EVENT_FUNC) E_xiahsc_idle,
	/* delay function */   (DELAY_FUNC) D_xiahsc_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_idle,
	/* state options */   (0)},

	/* State "get_readback_positions" */ {
	/* state name */       "get_readback_positions",
	/* action function */ (ACTION_FUNC) A_xiahsc_get_readback_positions,
	/* event function */  (EVENT_FUNC) E_xiahsc_get_readback_positions,
	/* delay function */   (DELAY_FUNC) D_xiahsc_get_readback_positions,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xiahsc_get_readback_positions,
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


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "xiahsc" */ {
	/* ss name */            "xiahsc",
	/* ptr to state block */ state_xiahsc,
	/* number of states */   7,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "name=hsc1, S=ito:serial:b0, P=ito:, HSC=hsc1:";

/* State Program table (global) */
struct seqProgram xiahsc = {
	/* magic number */       20000315,
	/* *name */              "xiahsc",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_DEBUG | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void xiahscRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&xiahsc);
}
epicsExportRegistrar(xiahscRegistrar);

