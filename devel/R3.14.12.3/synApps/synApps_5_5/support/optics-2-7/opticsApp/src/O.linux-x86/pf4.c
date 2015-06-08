/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: pf4.i */

/* Event flags */
#define b1_mon	1
#define b2_mon	2
#define b3_mon	3
#define b4_mon	4
#define monoenergy_mon	5
#define selectenergy_mon	6
#define localenergy_mon	7
#define debug_flag_mon	8
#define bankctl_mon	9
#define filpos_mon	10
#define f1_mon	11
#define f2_mon	12
#define f3_mon	13
#define f4_mon	14
#define z1_mon	15
#define z2_mon	16
#define z3_mon	17
#define z4_mon	18

/* Program "pf4" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 41
#define NUM_EVENTS 18
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT FALSE
#define DEBUG_OPT TRUE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram pf4;

/* Variable declarations */
struct UserVar {
	int	b1;
	int	b2;
	int	b3;
	int	b4;
	double	monoenergy;
	int	selectenergy;
	double	localenergy;
	double	filterAl;
	double	filterTi;
	double	filterGl;
	double	trans;
	double	invtrans;
	int	debug_flag;
	long	bitflg;
	short	bitsts;
	short	bankctl;
	short	filpos;
	char	a00[MAX_STRING_SIZE];
	char	a01[MAX_STRING_SIZE];
	char	a02[MAX_STRING_SIZE];
	char	a03[MAX_STRING_SIZE];
	char	a04[MAX_STRING_SIZE];
	char	a05[MAX_STRING_SIZE];
	char	a06[MAX_STRING_SIZE];
	char	a07[MAX_STRING_SIZE];
	char	a08[MAX_STRING_SIZE];
	char	a09[MAX_STRING_SIZE];
	char	a10[MAX_STRING_SIZE];
	char	a11[MAX_STRING_SIZE];
	char	a12[MAX_STRING_SIZE];
	char	a13[MAX_STRING_SIZE];
	char	a14[MAX_STRING_SIZE];
	char	a15[MAX_STRING_SIZE];
	double	f1;
	double	f2;
	double	f3;
	double	f4;
	short	z1;
	short	z2;
	short	z3;
	short	z4;
	int	b1state;
	int	b2state;
	int	b3state;
	int	b4state;
	char	msg[256];
	char	*SNLtaskName;
	short	bits[16];
	double	xmit[16];
	double	energytimeout;
};

/* C code definitions */
# line 24 "../pf4.st"
#include <stdio.h>
# line 25 "../pf4.st"
#include <math.h>
# line 26 "../pf4.st"
#include <string.h>
# line 109 "../pf4.st"

# line 110 "../pf4.st"
static double TiAbsorb(double);
# line 111 "../pf4.st"
static double GlassAbsorb(double);
# line 112 "../pf4.st"
static double AlAbsorpLength(double);
# line 113 "../pf4.st"
static long numInArray(long,short*,short);
# line 114 "../pf4.st"
static void sortDecreasing(int,double*,short*,int);
# line 115 "../pf4.st"
static double thickZ(int,int,int,int,int,int,int,int,int,int,double,double,double,double);
# line 116 "../pf4.st"
static void RecalcFilters(double,short*,double*,int,int,int,int,int,int,double,double,double,double);
# line 116 "../pf4.st"


/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "pf4" */

/* Delay function for state "init" in state set "pf4" */
static void D_pf4_init(SS_ID ssId, struct UserVar *pVar)
{
# line 172 "../pf4.st"
}

/* Event function for state "init" in state set "pf4" */
static long E_pf4_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 172 "../pf4.st"
	if (TRUE)
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "pf4" */
static void A_pf4_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 125 "../pf4.st"
			(pVar->energytimeout) = 0.0;
			(pVar->SNLtaskName) = seq_macValueGet(ssId, "name");
# line 128 "../pf4.st"
			seq_pvGet(ssId, 0 /* b1 */, 2);
			seq_pvGet(ssId, 1 /* b2 */, 2);
			seq_pvGet(ssId, 2 /* b3 */, 2);
			seq_pvGet(ssId, 3 /* b4 */, 2);
			seq_pvGet(ssId, 16 /* filpos */, 2);
			seq_pvGet(ssId, 15 /* bankctl */, 2);
			seq_pvGet(ssId, 4 /* monoenergy */, 2);
			seq_pvGet(ssId, 12 /* debug_flag */, 2);
# line 137 "../pf4.st"
			seq_pvGet(ssId, 33 /* f1 */, 2);
			seq_pvGet(ssId, 34 /* f2 */, 2);
			seq_pvGet(ssId, 35 /* f3 */, 2);
			seq_pvGet(ssId, 36 /* f4 */, 2);
# line 142 "../pf4.st"
			seq_pvGet(ssId, 37 /* z1 */, 2);
			seq_pvGet(ssId, 38 /* z2 */, 2);
			seq_pvGet(ssId, 39 /* z3 */, 2);
			seq_pvGet(ssId, 40 /* z4 */, 2);
# line 147 "../pf4.st"
			{
				(pVar->localenergy) = ((pVar->monoenergy));
				seq_pvPut(ssId, 6 /* localenergy */, 2);
			}
# line 147 "../pf4.st"
			;
# line 149 "../pf4.st"
			seq_efClear(ssId, b1_mon);
			seq_efClear(ssId, b2_mon);
			seq_efClear(ssId, b3_mon);
			seq_efClear(ssId, b4_mon);
# line 154 "../pf4.st"
			seq_efClear(ssId, f1_mon);
			seq_efClear(ssId, f2_mon);
			seq_efClear(ssId, f3_mon);
			seq_efClear(ssId, f4_mon);
# line 159 "../pf4.st"
			seq_efClear(ssId, z1_mon);
			seq_efClear(ssId, z2_mon);
			seq_efClear(ssId, z3_mon);
			seq_efClear(ssId, z4_mon);
# line 164 "../pf4.st"
			seq_efClear(ssId, filpos_mon);
			seq_efClear(ssId, bankctl_mon);
			seq_efClear(ssId, selectenergy_mon);
			seq_efClear(ssId, localenergy_mon);
			seq_efClear(ssId, monoenergy_mon);
			seq_efClear(ssId, debug_flag_mon);
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 170, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "init complete");
				epicsThreadSleep(0.01);
			}
# line 170 "../pf4.st"
			;
		}
		return;
	}
}
/* Code for state "filterBits" in state set "pf4" */

/* Delay function for state "filterBits" in state set "pf4" */
static void D_pf4_filterBits(SS_ID ssId, struct UserVar *pVar)
{
# line 188 "../pf4.st"
}

/* Event function for state "filterBits" in state set "pf4" */
static long E_pf4_filterBits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 188 "../pf4.st"
	if (TRUE)
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "filterBits" in state set "pf4" */
static void A_pf4_filterBits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 180 "../pf4.st"
			{
				(pVar->filpos) = (numInArray(16, (pVar->bits), (((pVar->b4) << 3) | ((pVar->b3) << 2) | ((pVar->b2) << 1) | (pVar->b1))));
				seq_pvPut(ssId, 16 /* filpos */, 2);
			}
# line 180 "../pf4.st"
			;
			{
				(pVar->bitflg) = ((1 << (pVar->filpos)));
				seq_pvPut(ssId, 13 /* bitflg */, 2);
			}
# line 181 "../pf4.st"
			;
			{
				(pVar->bitsts) = ((pVar->bits)[(pVar->filpos)]);
				seq_pvPut(ssId, 14 /* bitsts */, 2);
			}
# line 182 "../pf4.st"
			;
# line 184 "../pf4.st"
			(pVar->bankctl) = ((pVar->bankctl)) ? 2 : 0;
			seq_pvPut(ssId, 15 /* bankctl */, 2);
# line 187 "../pf4.st"
			seq_efClear(ssId, filpos_mon);
		}
		return;
	}
}
/* Code for state "filterPos" in state set "pf4" */

/* Delay function for state "filterPos" in state set "pf4" */
static void D_pf4_filterPos(SS_ID ssId, struct UserVar *pVar)
{
# line 224 "../pf4.st"
}

/* Event function for state "filterPos" in state set "pf4" */
static long E_pf4_filterPos(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 224 "../pf4.st"
	if (TRUE)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "filterPos" in state set "pf4" */
static void A_pf4_filterPos(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 197 "../pf4.st"
			(pVar->b1state) = ((pVar->bits)[(pVar->filpos)] & 0x1);
			(pVar->b2state) = ((pVar->bits)[(pVar->filpos)] & 0x2);
			(pVar->b3state) = ((pVar->bits)[(pVar->filpos)] & 0x4);
			(pVar->b4state) = ((pVar->bits)[(pVar->filpos)] & 0x8);
# line 203 "../pf4.st"
			if ((pVar->b1state))
			{
				(pVar->b1) = (1);
				seq_pvPut(ssId, 0 /* b1 */, 2);
			}
# line 203 "../pf4.st"
			;
			if ((pVar->b2state))
			{
				(pVar->b2) = (1);
				seq_pvPut(ssId, 1 /* b2 */, 2);
			}
# line 204 "../pf4.st"
			;
			if ((pVar->b3state))
			{
				(pVar->b3) = (1);
				seq_pvPut(ssId, 2 /* b3 */, 2);
			}
# line 205 "../pf4.st"
			;
			if ((pVar->b4state))
			{
				(pVar->b4) = (1);
				seq_pvPut(ssId, 3 /* b4 */, 2);
			}
# line 206 "../pf4.st"
			;
# line 209 "../pf4.st"
			epicsThreadSleep((0.20));
# line 212 "../pf4.st"
			if (!(pVar->b1state))
			{
				(pVar->b1) = (0);
				seq_pvPut(ssId, 0 /* b1 */, 2);
			}
# line 212 "../pf4.st"
			;
			if (!(pVar->b2state))
			{
				(pVar->b2) = (0);
				seq_pvPut(ssId, 1 /* b2 */, 2);
			}
# line 213 "../pf4.st"
			;
			if (!(pVar->b3state))
			{
				(pVar->b3) = (0);
				seq_pvPut(ssId, 2 /* b3 */, 2);
			}
# line 214 "../pf4.st"
			;
			if (!(pVar->b4state))
			{
				(pVar->b4) = (0);
				seq_pvPut(ssId, 3 /* b4 */, 2);
			}
# line 215 "../pf4.st"
			;
# line 217 "../pf4.st"
			{
				(pVar->bitflg) = ((1 << (pVar->filpos)));
				seq_pvPut(ssId, 13 /* bitflg */, 2);
			}
# line 217 "../pf4.st"
			;
			{
				(pVar->bitsts) = ((pVar->bits)[(pVar->filpos)]);
				seq_pvPut(ssId, 14 /* bitsts */, 2);
			}
# line 218 "../pf4.st"
			;
# line 220 "../pf4.st"
			seq_efClear(ssId, b1_mon);
			seq_efClear(ssId, b2_mon);
			seq_efClear(ssId, b3_mon);
			seq_efClear(ssId, b4_mon);
		}
		return;
	}
}
/* Code for state "recalcBank" in state set "pf4" */

/* Delay function for state "recalcBank" in state set "pf4" */
static void D_pf4_recalcBank(SS_ID ssId, struct UserVar *pVar)
{
# line 236 "../pf4.st"
}

/* Event function for state "recalcBank" in state set "pf4" */
static long E_pf4_recalcBank(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 236 "../pf4.st"
	if (TRUE)
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "recalcBank" in state set "pf4" */
static void A_pf4_recalcBank(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 232 "../pf4.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 232, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "recalcBank");
				epicsThreadSleep(0.01);
			}
# line 232 "../pf4.st"
			;
# line 234 "../pf4.st"
			(pVar->bankctl) = ((pVar->bankctl)) ? 3 : 0;
			seq_pvPut(ssId, 15 /* bankctl */, 2);
		}
		return;
	}
}
/* Code for state "bankControl" in state set "pf4" */

/* Delay function for state "bankControl" in state set "pf4" */
static void D_pf4_bankControl(SS_ID ssId, struct UserVar *pVar)
{
# line 245 "../pf4.st"
# line 299 "../pf4.st"
}

/* Event function for state "bankControl" in state set "pf4" */
static long E_pf4_bankControl(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 245 "../pf4.st"
	if ((pVar->bankctl) == 0)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 299 "../pf4.st"
	if (TRUE)
	{
		*pNextState = 6;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "bankControl" in state set "pf4" */
static void A_pf4_bankControl(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 243 "../pf4.st"
			sprintf((pVar->msg), "filter bank switched off (%d)", (pVar->bankctl));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 244, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 244 "../pf4.st"
			;
		}
		return;
	case 1:
		{
# line 250 "../pf4.st"
			if ((pVar->bankctl) == 1)
				sprintf((pVar->msg), "filter bank changed value to %d", (pVar->bankctl));
# line 267 "../pf4.st"
			else
			if ((pVar->bankctl) == 2)
			{
				if ((pVar->z1) == 0 || (pVar->z2) == 0 || (pVar->z3) == 0 || (pVar->z4) == 0)
				{
					(pVar->filterAl) = (thickZ(0, (pVar->bankctl), (pVar->b1), (pVar->b2), (pVar->b3), (pVar->b4), (pVar->z1), (pVar->z2), (pVar->z3), (pVar->z4), (pVar->f1), (pVar->f2), (pVar->f3), (pVar->f4)));
					seq_pvPut(ssId, 7 /* filterAl */, 2);
				}
				;
				if ((pVar->z1) == 1 || (pVar->z2) == 1 || (pVar->z3) == 1 || (pVar->z4) == 1)
				{
					(pVar->filterTi) = (thickZ(1, (pVar->bankctl), (pVar->b1), (pVar->b2), (pVar->b3), (pVar->b4), (pVar->z1), (pVar->z2), (pVar->z3), (pVar->z4), (pVar->f1), (pVar->f2), (pVar->f3), (pVar->f4)));
					seq_pvPut(ssId, 8 /* filterTi */, 2);
				}
				;
				if ((pVar->z1) == 2 || (pVar->z2) == 2 || (pVar->z3) == 2 || (pVar->z4) == 2)
				{
					(pVar->filterGl) = (thickZ(2, (pVar->bankctl), (pVar->b1), (pVar->b2), (pVar->b3), (pVar->b4), (pVar->z1), (pVar->z2), (pVar->z3), (pVar->z4), (pVar->f1), (pVar->f2), (pVar->f3), (pVar->f4)));
					seq_pvPut(ssId, 9 /* filterGl */, 2);
				}
				;
				{
					(pVar->trans) = ((pVar->xmit)[(pVar->filpos)]);
					seq_pvPut(ssId, 10 /* trans */, 2);
				}
				;
				if ((pVar->trans) > 0.0)
				{
					(pVar->invtrans) = (1 / (pVar->trans));
					seq_pvPut(ssId, 11 /* invtrans */, 2);
				}
				;
				{
					(pVar->bitsts) = ((pVar->bits)[(pVar->filpos)]);
					seq_pvPut(ssId, 14 /* bitsts */, 2);
				}
				;
				{
					(pVar->bitflg) = ((1 << (pVar->filpos)));
					seq_pvPut(ssId, 13 /* bitflg */, 2);
				}
				;
				(pVar->bankctl) = ((pVar->bankctl)) ? 1 : 0;
				seq_pvPut(ssId, 15 /* bankctl */, 2);
				sprintf((pVar->msg), "filter bank changed value to %d", (pVar->bankctl));
			}
# line 294 "../pf4.st"
			else
			if ((pVar->bankctl) == 3)
			{
				seq_pvGet(ssId, 6 /* localenergy */, 2);
				seq_efClear(ssId, localenergy_mon);
				RecalcFilters((pVar->localenergy), (pVar->bits), (pVar->xmit), (pVar->debug_flag), (pVar->bankctl), (pVar->z1), (pVar->z2), (pVar->z3), (pVar->z4), (pVar->f1), (pVar->f2), (pVar->f3), (pVar->f4));
				sprintf((pVar->a00), "%.2e", (pVar->xmit)[0]);
				seq_pvPut(ssId, 17 /* a00 */, 2);
				sprintf((pVar->a01), "%.2e", (pVar->xmit)[1]);
				seq_pvPut(ssId, 18 /* a01 */, 2);
				sprintf((pVar->a02), "%.2e", (pVar->xmit)[2]);
				seq_pvPut(ssId, 19 /* a02 */, 2);
				sprintf((pVar->a03), "%.2e", (pVar->xmit)[3]);
				seq_pvPut(ssId, 20 /* a03 */, 2);
				sprintf((pVar->a04), "%.2e", (pVar->xmit)[4]);
				seq_pvPut(ssId, 21 /* a04 */, 2);
				sprintf((pVar->a05), "%.2e", (pVar->xmit)[5]);
				seq_pvPut(ssId, 22 /* a05 */, 2);
				sprintf((pVar->a06), "%.2e", (pVar->xmit)[6]);
				seq_pvPut(ssId, 23 /* a06 */, 2);
				sprintf((pVar->a07), "%.2e", (pVar->xmit)[7]);
				seq_pvPut(ssId, 24 /* a07 */, 2);
				sprintf((pVar->a08), "%.2e", (pVar->xmit)[8]);
				seq_pvPut(ssId, 25 /* a08 */, 2);
				sprintf((pVar->a09), "%.2e", (pVar->xmit)[9]);
				seq_pvPut(ssId, 26 /* a09 */, 2);
				sprintf((pVar->a10), "%.2e", (pVar->xmit)[10]);
				seq_pvPut(ssId, 27 /* a10 */, 2);
				sprintf((pVar->a11), "%.2e", (pVar->xmit)[11]);
				seq_pvPut(ssId, 28 /* a11 */, 2);
				sprintf((pVar->a12), "%.2e", (pVar->xmit)[12]);
				seq_pvPut(ssId, 29 /* a12 */, 2);
				sprintf((pVar->a13), "%.2e", (pVar->xmit)[13]);
				seq_pvPut(ssId, 30 /* a13 */, 2);
				sprintf((pVar->a14), "%.2e", (pVar->xmit)[14]);
				seq_pvPut(ssId, 31 /* a14 */, 2);
				sprintf((pVar->a15), "%.2e", (pVar->xmit)[15]);
				seq_pvPut(ssId, 32 /* a15 */, 2);
				{
					(pVar->bankctl) = (2);
					seq_pvPut(ssId, 15 /* bankctl */, 2);
				}
				;
				sprintf((pVar->msg), "filter bank changed value to %d", (pVar->bankctl));
			}
# line 296 "../pf4.st"
			else
				sprintf((pVar->msg), "invalid filter bank value of %d", (pVar->bankctl));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 298, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 298 "../pf4.st"
			;
		}
		return;
	}
}
/* Code for state "bankOff" in state set "pf4" */

/* Delay function for state "bankOff" in state set "pf4" */
static void D_pf4_bankOff(SS_ID ssId, struct UserVar *pVar)
{
# line 314 "../pf4.st"
# line 316 "../pf4.st"
	seq_delayInit(ssId, 0, ((0.10)));
}

/* Event function for state "bankOff" in state set "pf4" */
static long E_pf4_bankOff(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 314 "../pf4.st"
	if ((pVar->bankctl))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 316 "../pf4.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "bankOff" in state set "pf4" */
static void A_pf4_bankOff(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 306 "../pf4.st"
			seq_pvGet(ssId, 5 /* selectenergy */, 2);
			seq_pvGet(ssId, 6 /* localenergy */, 2);
			seq_pvGet(ssId, 4 /* monoenergy */, 2);
# line 310 "../pf4.st"
			seq_efClear(ssId, localenergy_mon);
# line 312 "../pf4.st"
			sprintf((pVar->msg), "filter bank switched on (%d)", (pVar->bankctl));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 313, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 313 "../pf4.st"
			;
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "idle" in state set "pf4" */

/* Delay function for state "idle" in state set "pf4" */
static void D_pf4_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 326 "../pf4.st"
# line 337 "../pf4.st"
# line 354 "../pf4.st"
# line 366 "../pf4.st"
# line 373 "../pf4.st"
# line 383 "../pf4.st"
# line 393 "../pf4.st"
# line 400 "../pf4.st"
# line 407 "../pf4.st"
# line 419 "../pf4.st"
	seq_delayInit(ssId, 0, ((0.10)));
}

/* Event function for state "idle" in state set "pf4" */
static long E_pf4_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 326 "../pf4.st"
	if (seq_efTestAndClear(ssId, debug_flag_mon))
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
# line 337 "../pf4.st"
	if (seq_efTestAndClear(ssId, localenergy_mon))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 354 "../pf4.st"
	if (seq_efTestAndClear(ssId, selectenergy_mon))
	{
		*pNextState = 6;
		*pTransNum = 2;
		return TRUE;
	}
# line 366 "../pf4.st"
	if ((pVar->selectenergy) && seq_efTestAndClear(ssId, monoenergy_mon))
	{
		*pNextState = 6;
		*pTransNum = 3;
		return TRUE;
	}
# line 373 "../pf4.st"
	if (seq_efTestAndClear(ssId, bankctl_mon))
	{
		*pNextState = 4;
		*pTransNum = 4;
		return TRUE;
	}
# line 383 "../pf4.st"
	if (seq_efTestAndClear(ssId, f1_mon) || seq_efTestAndClear(ssId, f2_mon) || seq_efTestAndClear(ssId, f3_mon) || seq_efTestAndClear(ssId, f4_mon))
	{
		*pNextState = 6;
		*pTransNum = 5;
		return TRUE;
	}
# line 393 "../pf4.st"
	if (seq_efTestAndClear(ssId, z1_mon) || seq_efTestAndClear(ssId, z2_mon) || seq_efTestAndClear(ssId, z3_mon) || seq_efTestAndClear(ssId, z4_mon))
	{
		*pNextState = 6;
		*pTransNum = 6;
		return TRUE;
	}
# line 400 "../pf4.st"
	if (seq_efTestAndClear(ssId, b4_mon) || seq_efTestAndClear(ssId, b3_mon) || seq_efTestAndClear(ssId, b2_mon) || seq_efTestAndClear(ssId, b1_mon))
	{
		*pNextState = 1;
		*pTransNum = 7;
		return TRUE;
	}
# line 407 "../pf4.st"
	if (seq_efTestAndClear(ssId, filpos_mon) && (pVar->bankctl))
	{
		*pNextState = 2;
		*pTransNum = 8;
		return TRUE;
	}
# line 419 "../pf4.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 6;
		*pTransNum = 9;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "pf4" */
static void A_pf4_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 324 "../pf4.st"
			sprintf((pVar->msg), "Debug level changed to %d", (pVar->debug_flag));
			if ((pVar->debug_flag) >= 1)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 325, (pVar->SNLtaskName), 1);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 325 "../pf4.st"
			;
		}
		return;
	case 1:
		{
# line 331 "../pf4.st"
			{
				(pVar->selectenergy) = (0);
				seq_pvPut(ssId, 5 /* selectenergy */, 2);
			}
# line 331 "../pf4.st"
			;
			epicsThreadSleep((0.01));
			seq_efClear(ssId, selectenergy_mon);
# line 335 "../pf4.st"
			sprintf((pVar->msg), "local energy changed to %f", (pVar->localenergy));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 336, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 336 "../pf4.st"
			;
		}
		return;
	case 2:
		{
# line 350 "../pf4.st"
			if ((pVar->selectenergy))
			{
				{
					(pVar->localenergy) = ((pVar->monoenergy));
					seq_pvPut(ssId, 6 /* localenergy */, 2);
				}
				;
				epicsThreadSleep((0.01));
				seq_efClear(ssId, localenergy_mon);
				(pVar->bankctl) = ((pVar->bankctl)) ? 3 : 0;
				seq_pvPut(ssId, 15 /* bankctl */, 2);
			}
# line 352 "../pf4.st"
			sprintf((pVar->msg), "using energy local/monochromator changed to %d", (pVar->selectenergy));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 353, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 353 "../pf4.st"
			;
		}
		return;
	case 3:
		{
# line 359 "../pf4.st"
			{
				(pVar->localenergy) = ((pVar->monoenergy));
				seq_pvPut(ssId, 6 /* localenergy */, 2);
			}
# line 359 "../pf4.st"
			;
			epicsThreadSleep((0.01));
			seq_efClear(ssId, localenergy_mon);
# line 363 "../pf4.st"
			(pVar->energytimeout) = (1.50);
			sprintf((pVar->msg), "monochromator energy changed to %f", (pVar->monoenergy));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 365, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 365 "../pf4.st"
			;
		}
		return;
	case 4:
		{
# line 371 "../pf4.st"
			sprintf((pVar->msg), "filter bank changed to %d, going to state bankControl", (pVar->bankctl));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 372, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 372 "../pf4.st"
			;
		}
		return;
	case 5:
		{
# line 378 "../pf4.st"
			sprintf((pVar->msg), "filters thickness changed to f1=%g, f2=%g, f3=%g, f3=%g", (pVar->f1), (pVar->f2), (pVar->f3), (pVar->f4));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 379, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 379 "../pf4.st"
			;
# line 381 "../pf4.st"
			(pVar->bankctl) = ((pVar->bankctl)) ? 3 : 0;
			seq_pvPut(ssId, 15 /* bankctl */, 2);
		}
		return;
	case 6:
		{
# line 388 "../pf4.st"
			sprintf((pVar->msg), "filters material index changed to z1=%d, z2=%d, z3=%d, z3=%d", (pVar->z1), (pVar->z2), (pVar->z3), (pVar->z4));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 389, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 389 "../pf4.st"
			;
# line 391 "../pf4.st"
			(pVar->bankctl) = ((pVar->bankctl)) ? 3 : 0;
			seq_pvPut(ssId, 15 /* bankctl */, 2);
		}
		return;
	case 7:
		{
# line 398 "../pf4.st"
			sprintf((pVar->msg), "filter bits changed to (%1.1d%1.1d%1.1d%1.1d), going to state filterBits", (pVar->b4), (pVar->b3), (pVar->b2), (pVar->b1));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 399, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 399 "../pf4.st"
			;
		}
		return;
	case 8:
		{
# line 405 "../pf4.st"
			sprintf((pVar->msg), "filter position changed to %d, going to state filterPos", (pVar->filpos));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../pf4.st", 406, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 406 "../pf4.st"
			;
		}
		return;
	case 9:
		{
# line 417 "../pf4.st"
			if ((pVar->energytimeout) > 0.0 && ((pVar->energytimeout) = (pVar->energytimeout) - (0.10)) <= 0.0)
			{
				(pVar->bankctl) = ((pVar->bankctl)) ? 3 : 0;
				seq_pvPut(ssId, 15 /* bankctl */, 2);
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../pf4.st", 416, (pVar->SNLtaskName), 2);
					;
					printf("%s\n", "energy timeout expired, force recalculation");
					epicsThreadSleep(0.01);
				}
				;
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
  {"{B1}", (void *)OFFSET(struct UserVar, b1), "b1", 
    "int", 1, 19, 1, 1, 0, 0, 0},

  {"{B2}", (void *)OFFSET(struct UserVar, b2), "b2", 
    "int", 1, 20, 2, 1, 0, 0, 0},

  {"{B3}", (void *)OFFSET(struct UserVar, b3), "b3", 
    "int", 1, 21, 3, 1, 0, 0, 0},

  {"{B4}", (void *)OFFSET(struct UserVar, b4), "b4", 
    "int", 1, 22, 4, 1, 0, 0, 0},

  {"{M}", (void *)OFFSET(struct UserVar, monoenergy), "monoenergy", 
    "double", 1, 23, 5, 1, 0, 0, 0},

  {"{P}{H}useMono", (void *)OFFSET(struct UserVar, selectenergy), "selectenergy", 
    "int", 1, 24, 6, 1, 0, 0, 0},

  {"{P}{H}E:local", (void *)OFFSET(struct UserVar, localenergy), "localenergy", 
    "double", 1, 25, 7, 1, 0, 0, 0},

  {"{P}{H}filterAl", (void *)OFFSET(struct UserVar, filterAl), "filterAl", 
    "double", 1, 26, 0, 0, 0, 0, 0},

  {"{P}{H}filterTi", (void *)OFFSET(struct UserVar, filterTi), "filterTi", 
    "double", 1, 27, 0, 0, 0, 0, 0},

  {"{P}{H}filterGlass", (void *)OFFSET(struct UserVar, filterGl), "filterGl", 
    "double", 1, 28, 0, 0, 0, 0, 0},

  {"{P}{H}trans{B}", (void *)OFFSET(struct UserVar, trans), "trans", 
    "double", 1, 29, 0, 0, 0, 0, 0},

  {"{P}{H}invTrans{B}", (void *)OFFSET(struct UserVar, invtrans), "invtrans", 
    "double", 1, 30, 0, 0, 0, 0, 0},

  {"{P}{H}debug{B}", (void *)OFFSET(struct UserVar, debug_flag), "debug_flag", 
    "int", 1, 31, 8, 1, 0, 0, 0},

  {"{P}{H}bitFlag{B}", (void *)OFFSET(struct UserVar, bitflg), "bitflg", 
    "long", 1, 32, 0, 0, 0, 0, 0},

  {"{P}{H}status{B}", (void *)OFFSET(struct UserVar, bitsts), "bitsts", 
    "short", 1, 33, 0, 0, 0, 0, 0},

  {"{P}{H}bank{B}", (void *)OFFSET(struct UserVar, bankctl), "bankctl", 
    "short", 1, 34, 9, 1, 0, 0, 0},

  {"{P}{H}fPos{B}", (void *)OFFSET(struct UserVar, filpos), "filpos", 
    "short", 1, 35, 10, 1, 0, 0, 0},

  {"{P}{H}fPos{B}.ZRST", (void *)OFFSET(struct UserVar, a00[0]), "a00", 
    "string", 1, 36, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.ONST", (void *)OFFSET(struct UserVar, a01[0]), "a01", 
    "string", 1, 37, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.TWST", (void *)OFFSET(struct UserVar, a02[0]), "a02", 
    "string", 1, 38, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.THST", (void *)OFFSET(struct UserVar, a03[0]), "a03", 
    "string", 1, 39, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.FRST", (void *)OFFSET(struct UserVar, a04[0]), "a04", 
    "string", 1, 40, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.FVST", (void *)OFFSET(struct UserVar, a05[0]), "a05", 
    "string", 1, 41, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.SXST", (void *)OFFSET(struct UserVar, a06[0]), "a06", 
    "string", 1, 42, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.SVST", (void *)OFFSET(struct UserVar, a07[0]), "a07", 
    "string", 1, 43, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.EIST", (void *)OFFSET(struct UserVar, a08[0]), "a08", 
    "string", 1, 44, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.NIST", (void *)OFFSET(struct UserVar, a09[0]), "a09", 
    "string", 1, 45, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.TEST", (void *)OFFSET(struct UserVar, a10[0]), "a10", 
    "string", 1, 46, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.ELST", (void *)OFFSET(struct UserVar, a11[0]), "a11", 
    "string", 1, 47, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.TVST", (void *)OFFSET(struct UserVar, a12[0]), "a12", 
    "string", 1, 48, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.TTST", (void *)OFFSET(struct UserVar, a13[0]), "a13", 
    "string", 1, 49, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.FTST", (void *)OFFSET(struct UserVar, a14[0]), "a14", 
    "string", 1, 50, 0, 0, 0, 0, 0},

  {"{P}{H}fPos{B}.FFST", (void *)OFFSET(struct UserVar, a15[0]), "a15", 
    "string", 1, 51, 0, 0, 0, 0, 0},

  {"{P}{H}f1{B}", (void *)OFFSET(struct UserVar, f1), "f1", 
    "double", 1, 52, 11, 1, 0, 0, 0},

  {"{P}{H}f2{B}", (void *)OFFSET(struct UserVar, f2), "f2", 
    "double", 1, 53, 12, 1, 0, 0, 0},

  {"{P}{H}f3{B}", (void *)OFFSET(struct UserVar, f3), "f3", 
    "double", 1, 54, 13, 1, 0, 0, 0},

  {"{P}{H}f4{B}", (void *)OFFSET(struct UserVar, f4), "f4", 
    "double", 1, 55, 14, 1, 0, 0, 0},

  {"{P}{H}Z1{B}", (void *)OFFSET(struct UserVar, z1), "z1", 
    "short", 1, 56, 15, 1, 0, 0, 0},

  {"{P}{H}Z2{B}", (void *)OFFSET(struct UserVar, z2), "z2", 
    "short", 1, 57, 16, 1, 0, 0, 0},

  {"{P}{H}Z3{B}", (void *)OFFSET(struct UserVar, z3), "z3", 
    "short", 1, 58, 17, 1, 0, 0, 0},

  {"{P}{H}Z4{B}", (void *)OFFSET(struct UserVar, z4), "z4", 
    "short", 1, 59, 18, 1, 0, 0, 0},

};

/* Event masks for state set pf4 */
	/* Event mask for state init: */
static bitMask	EM_pf4_init[] = {
	0x00000000,
	0x00000000,
};
	/* Event mask for state filterBits: */
static bitMask	EM_pf4_filterBits[] = {
	0x00000000,
	0x00000000,
};
	/* Event mask for state filterPos: */
static bitMask	EM_pf4_filterPos[] = {
	0x00000000,
	0x00000000,
};
	/* Event mask for state recalcBank: */
static bitMask	EM_pf4_recalcBank[] = {
	0x00000000,
	0x00000000,
};
	/* Event mask for state bankControl: */
static bitMask	EM_pf4_bankControl[] = {
	0x00000000,
	0x00000004,
};
	/* Event mask for state bankOff: */
static bitMask	EM_pf4_bankOff[] = {
	0x00000000,
	0x00000004,
};
	/* Event mask for state idle: */
static bitMask	EM_pf4_idle[] = {
	0x0107fffe,
	0x00000004,
};

/* State Blocks */

static struct seqState state_pf4[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_pf4_init,
	/* event function */  (EVENT_FUNC) E_pf4_init,
	/* delay function */   (DELAY_FUNC) D_pf4_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_init,
	/* state options */   (0)},

	/* State "filterBits" */ {
	/* state name */       "filterBits",
	/* action function */ (ACTION_FUNC) A_pf4_filterBits,
	/* event function */  (EVENT_FUNC) E_pf4_filterBits,
	/* delay function */   (DELAY_FUNC) D_pf4_filterBits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_filterBits,
	/* state options */   (0)},

	/* State "filterPos" */ {
	/* state name */       "filterPos",
	/* action function */ (ACTION_FUNC) A_pf4_filterPos,
	/* event function */  (EVENT_FUNC) E_pf4_filterPos,
	/* delay function */   (DELAY_FUNC) D_pf4_filterPos,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_filterPos,
	/* state options */   (0)},

	/* State "recalcBank" */ {
	/* state name */       "recalcBank",
	/* action function */ (ACTION_FUNC) A_pf4_recalcBank,
	/* event function */  (EVENT_FUNC) E_pf4_recalcBank,
	/* delay function */   (DELAY_FUNC) D_pf4_recalcBank,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_recalcBank,
	/* state options */   (0)},

	/* State "bankControl" */ {
	/* state name */       "bankControl",
	/* action function */ (ACTION_FUNC) A_pf4_bankControl,
	/* event function */  (EVENT_FUNC) E_pf4_bankControl,
	/* delay function */   (DELAY_FUNC) D_pf4_bankControl,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_bankControl,
	/* state options */   (0)},

	/* State "bankOff" */ {
	/* state name */       "bankOff",
	/* action function */ (ACTION_FUNC) A_pf4_bankOff,
	/* event function */  (EVENT_FUNC) E_pf4_bankOff,
	/* delay function */   (DELAY_FUNC) D_pf4_bankOff,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_bankOff,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_pf4_idle,
	/* event function */  (EVENT_FUNC) E_pf4_idle,
	/* delay function */   (DELAY_FUNC) D_pf4_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_pf4_idle,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "pf4" */ {
	/* ss name */            "pf4",
	/* ptr to state block */ state_pf4,
	/* number of states */   7,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "name=pf4,P=,H=,F=,B1=,B2=,B3=,B4=,M=";

/* State Program table (global) */
struct seqProgram pf4 = {
	/* magic number */       20000315,
	/* *name */              "pf4",
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
/* Global C code */
# line 430 "../pf4.st"




double AlAbsorpLength(double keV)
{
    double Wcoef0[]={1.90195,-0.00120447,4.3745e-7,8.68635e-11,3.40793e-15,-1.05816e-19,5.83389e-25};
    double Wcoef1[]={-1625.33,0.328256,-2.68391e-5,1.26554e-9,-2.41557e-14,2.12864e-19,-7.28743e-25};
    double kink = 26797.5;
    double* pointer;
    double sum,power;
    double eV;
    int i;

    eV = keV * 1000;
    if (eV>60000.) eV = 60000.;
    pointer = (eV<kink) ? Wcoef0 : Wcoef1;
    sum = 0.;
    power=1.;
    for( i=0; i<7; i++ )
    {
        sum += pointer[i] * power;
        power *= eV;
    }
    return sum;
}



double TiAbsorb(double keV)
{
    double eV;
    double mu;

    eV = keV * 1000.;
    if( eV < 1e3 ) mu = 0;
    else if( eV < 4966.4 )
    {
        double c0=0.00092284;
        double c1=2.5891e+08;
        double powA=-2.6651;
        mu = c0+c1*pow(eV,powA);
    }
    else
    {

        double offset = 5.63768167444831e-5;
        double amp = 24061652313.4169;
        double powB = -2.91380053083527;
        double intercept = -0.268162843203489;
        double slope = 3.74221014277593e-5;

        double ampExp = -1.05663835782997;
        double invTau = -0.000570785180739491;
        double extra = (eV < 6456) ? (intercept+slope*eV) : (ampExp*exp(invTau*eV));
        mu = (offset + amp*pow(eV,powB) + extra);
    }
    return( 1./mu );
}
# 502 "../pf4.st"
double GlassAbsorb(double keV)
{
    double absLength;
    double c0,c1,c2;
    double c3,c4,c5,c6,c7;
    double keV2, keVn;
    keV2 = keV*keV;

    if( keV < 2 )
        absLength = 0;
    else if( keV < 2.472 )
    {
        c0 = 0.5059463974;
        c1 = -0.1259565387;
        c2 = 0.01763933889;
        absLength = c0 + c1*keV + c2*keV2;
    }
    else if( keV < 3.6084 )
    {
        c0 = 0.4570603245;
        c1 = -0.08869920063;
        c2 = 0.01032934773;
        absLength = c0 + c1*keV + c2*keV2;
    }
    else if( keV < 4.0385 )
    {
        c0 = 0.3708574258;
        c1 = -0.04453063888;
        c2 = 3.979930821e-3;
        absLength = c0 + c1*keV + c2*keV2;
    }
    else if( keV < 7.112 )
    {
        c0 = 0.2830642538;
        c1 = -0.0223186563;
        c2 = 1.412011413e-3;
        absLength = c0 + c1*keV + c2*keV2;
    }
    else
    {
        c0 = 0.2715022686;
        c1 = -0.02428526798;
        c2 = 2.984228845e-3;
        c3 = -2.003675391e-4;
        c4 = 7.983398893e-6;
        c5 = -1.869726202e-7;
        c6 = 2.378962632e-9;
        c7 = -1.270082060e-11;
        absLength = c0 + c1*keV;
        keVn = keV2; absLength += c2*keVn;
        keVn *= keV; absLength += c3*keVn;
        keVn *= keV; absLength += c4*keVn;
        keVn *= keV; absLength += c5*keVn;
        keVn *= keV; absLength += c6*keVn;
        keVn *= keV; absLength += c7*keVn;
    }
    absLength *= keV*keV*keV;
    return( absLength );
}


void RecalcFilters(double keV,short* bits,double* xmit,int d,int b,int z1,
                    int z2,int z3,int z4,double f1,double f2,double f3,double f4)
{
    double absLenAl,absLenTi,absLenGlass;
    double xAl,xTi,xGlass;
    int i;

    if (d >= 10) printf("\nRecalcFilters: keV=%g\n",keV);
    absLenAl = AlAbsorpLength(keV);
    absLenTi = TiAbsorb(keV);
    absLenGlass = GlassAbsorb(keV);
    for (i=0;i<16;i++)
    {
        if (b)
        {
            xAl = (z1==0 && 1&i) ? f1 : 0;
            xAl += (z2==0 && 2&i) ? f2 : 0;
            xAl += (z3==0 && 4&i) ? f3 : 0;
            xAl += (z4==0 && 8&i) ? f4 : 0;
            xTi = (z1==1 && 1&i) ? f1 : 0;
            xTi += (z2==1 && 2&i) ? f2 : 0;
            xTi += (z3==1 && 4&i) ? f3 : 0;
            xTi += (z4==1 && 8&i) ? f4 : 0;
            xGlass = (z1==2 && 1&i) ? f1 : 0;
            xGlass += (z2==2 && 2&i) ? f2 : 0;
            xGlass += (z3==2 && 4&i) ? f3 : 0;
            xGlass += (z4==2 && 8&i) ? f4 : 0;
            if (d >= 10)
            {
                printf("       in RecalcFilters %3d, Aluminum = %g,   Titanium = %g,   Glass = %g\n",i,xAl,xTi,xGlass);
            }
            xmit[i] = exp(-xAl*1000./absLenAl);
            xmit[i] *= exp(-xTi*1000./absLenTi);
            xmit[i] *= exp(-xGlass*1000./absLenGlass);
        }
        else
            xmit[i] = 1.;
        bits[i] = i;
    }
    if (b) sortDecreasing(d,xmit,bits,16);
}


void sortDecreasing(int d,double* arr,short* bits,int n)
{
    int ii, jj;
    double a;
    short b;

    if (d >= 9)
    {
        printf("       before sorting, bits=%d",bits[0]);
        for (ii=1; ii<n; ii++)
        {
            printf(",%d", bits[ii]);
        }
        printf("\n");
        if (d >= 10)
        {
            printf("                   transmit=%g",arr[0]);
            for (ii=1; ii<n/3; ii++)
            {
                printf(", %g", arr[ii]);
            }
            printf("\n");
        }
    }

    for (jj=1;jj<n;jj++)
    {
        a = arr[jj];
        b = bits[jj];
        ii = jj-1;
        while(ii>=0 && arr[ii]<a)
        {
            arr[ii+1] = arr[ii];
            bits[ii+1] = bits[ii];
            ii--;
        }
        arr[ii+1] = a;
        bits[ii+1] = b;
    }

    if (d >= 8)
    {
        printf("       after sorting, bits=%d",bits[0]);
        for (ii=1; ii<n; ii++)
        {
            printf(",%d", bits[ii]);
        }
        printf("\n");
        if (d >= 10)
        {
            printf("                   transmit=%g",arr[0]);
            for (ii=1; ii<n/3; ii++)
            {
                printf(", %g", arr[ii]);
            }
            printf("\n");
        }
    }
}



double thickZ(int z,int b,int b1,int b2,int b3,int b4,int z1,int z2,int z3,
              int z4,double f1,double f2,double f3,double f4)
{
    double sum = 0.0;

    if( b )
    {
        sum += (b1 && (z1==z)) ? f1 : 0;
        sum += (b2 && (z2==z)) ? f2 : 0;
        sum += (b3 && (z3==z)) ? f3 : 0;
        sum += (b4 && (z4==z)) ? f4 : 0;
    }

    return sum;
}



long numInArray(long N,short arr[],short value)
{
    int i;

    for( i=0; i<N; i++ ) if( value==arr[i] ) return i;
    return 0;
}



#include "epicsExport.h"


/* Register sequencer commands and program */

void pf4Registrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&pf4);
}
epicsExportRegistrar(pf4Registrar);

