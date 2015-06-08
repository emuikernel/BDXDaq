/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: Io.i */

/* Event flags */
#define Emono_mon	1
#define cnt_mon	2
#define ticks_mon	3
#define IoDebug_mon	4
#define icChannel_mon	5
#define E_using_mon	6
#define VperA_mon	7
#define v2f_mon	8
#define xN2_mon	9
#define xAr_mon	10
#define xHe_mon	11
#define xAir_mon	12
#define xCO2_mon	13
#define activeLen_mon	14
#define deadFront_mon	15
#define deadRear_mon	16
#define kapton1_mon	17
#define kapton2_mon	18
#define HePath_mon	19
#define airPath_mon	20
#define Be_mon	21
#define dEff_mon	22
#define ArPcntr_mon	23

/* Program "Io" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 57
#define NUM_EVENTS 23
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram Io;

/* Variable declarations */
static double	Emono;
static short	cnt;
static double	clock_rate;
static double	ticks;
static double	s2;
static double	s3;
static double	s4;
static double	s5;
static double	s6;
static double	s7;
static double	s8;
static double	s9;
static double	s10;
static double	s11;
static double	s12;
static double	s13;
static double	s14;
static double	s15;
static char	nm2[MAX_STRING_SIZE];
static char	nm3[MAX_STRING_SIZE];
static char	nm4[MAX_STRING_SIZE];
static char	nm5[MAX_STRING_SIZE];
static char	nm6[MAX_STRING_SIZE];
static char	nm7[MAX_STRING_SIZE];
static char	nm8[MAX_STRING_SIZE];
static char	nm9[MAX_STRING_SIZE];
static char	nm10[MAX_STRING_SIZE];
static char	nm11[MAX_STRING_SIZE];
static char	nm12[MAX_STRING_SIZE];
static char	nm13[MAX_STRING_SIZE];
static char	nm14[MAX_STRING_SIZE];
static char	nm15[MAX_STRING_SIZE];
static double	flux;
static double	ionPhotons;
static double	ionAbs;
static double	detector;
static short	IoDebug;
static short	icChannel;
static char	icName[MAX_STRING_SIZE];
static double	E_using;
static double	VperA;
static double	v2f;
static double	xN2;
static double	xAr;
static double	xHe;
static double	xAir;
static double	xCO2;
static double	activeLen;
static double	deadFront;
static double	deadRear;
static double	kapton1;
static double	kapton2;
static double	HePath;
static double	airPath;
static double	Be;
static double	dEff;
static short	ArPcntr;
static char	new_msg[256];
static double	updateRate;
static char	*SNLtaskName;
static short	Io_debug;

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 19 "../Io.st"
#include "epicsThread.h"
# line 120 "../Io.st"
 #include <stdlib.h>
# line 121 "../Io.st"
 #include <math.h>
# line 123 "../Io.st"
 static void EvalFlux(double aln, double dln1, double dln2, double dHe, double dAir, double dBe, double energy);
# line 124 "../Io.st"
 static double photon(double cps, double work, double VtoF, double Volt_per_A, double aln, double deadL, double Kapton, int gasId, double energy);
# line 125 "../Io.st"
 static double absorb(int id, double energy);
# line 126 "../Io.st"
 static double absH(double energy);
# line 127 "../Io.st"
 static double absHe(double energy);
# line 128 "../Io.st"
 static double absBe(double energy);
# line 129 "../Io.st"
 static double absC(double energy);
# line 130 "../Io.st"
 static double absN(double energy);
# line 131 "../Io.st"
 static double absO(double energy);
# line 132 "../Io.st"
 static double absAr(double energy);
# line 133 "../Io.st"
 static double absArPhoto(double energy);

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "ionChamber" */

/* Delay function for state "init" in state set "ionChamber" */
static void D_ionChamber_init(SS_ID ssId, struct UserVar *pVar)
{
# line 222 "../Io.st"
}

/* Event function for state "init" in state set "ionChamber" */
static long E_ionChamber_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 222 "../Io.st"
	if (TRUE)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "ionChamber" */
static void A_ionChamber_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 149 "../Io.st"
			SNLtaskName = seq_macValueGet(ssId, "name");
			Io_debug = 0;
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 151, SNLtaskName, "init start");
				epicsThreadSleep(0.1);
			}
# line 151 "../Io.st"
			;
			IoDebug = Io_debug;
			seq_efClear(ssId, IoDebug_mon);
			updateRate = 10.0;
			seq_pvGet(ssId, 0 /* Emono */, 0);
			seq_pvGet(ssId, 40 /* VperA */, 0);
			seq_pvGet(ssId, 2 /* clock_rate */, 0);
			seq_pvGet(ssId, 3 /* ticks */, 0);
			seq_pvGet(ssId, 4 /* s2 */, 0);
			seq_pvGet(ssId, 5 /* s3 */, 0);
			seq_pvGet(ssId, 6 /* s4 */, 0);
			seq_pvGet(ssId, 7 /* s5 */, 0);
			seq_pvGet(ssId, 8 /* s6 */, 0);
			seq_pvGet(ssId, 9 /* s7 */, 0);
			seq_pvGet(ssId, 10 /* s8 */, 0);
			seq_pvGet(ssId, 11 /* s9 */, 0);
			seq_pvGet(ssId, 12 /* s10 */, 0);
			seq_pvGet(ssId, 13 /* s11 */, 0);
			seq_pvGet(ssId, 14 /* s12 */, 0);
			seq_pvGet(ssId, 15 /* s13 */, 0);
			seq_pvGet(ssId, 16 /* s14 */, 0);
			seq_pvGet(ssId, 17 /* s15 */, 0);
# line 174 "../Io.st"
			{
				icChannel = (2);
				seq_pvPut(ssId, 37 /* icChannel */, 0);
			}
# line 175 "../Io.st"
			{
				VperA = (100000000.);
				seq_pvPut(ssId, 40 /* VperA */, 0);
			}
# line 176 "../Io.st"
			{
				v2f = (100000.);
				seq_pvPut(ssId, 41 /* v2f */, 0);
			}
# line 177 "../Io.st"
			{
				xAir = (1.);
				seq_pvPut(ssId, 45 /* xAir */, 0);
			}
# line 178 "../Io.st"
			{
				xN2 = (0.);
				seq_pvPut(ssId, 42 /* xN2 */, 0);
			}
# line 179 "../Io.st"
			{
				xAr = (0.);
				seq_pvPut(ssId, 43 /* xAr */, 0);
			}
# line 180 "../Io.st"
			{
				xHe = (0.);
				seq_pvPut(ssId, 44 /* xHe */, 0);
			}
# line 181 "../Io.st"
			{
				xCO2 = (0.);
				seq_pvPut(ssId, 46 /* xCO2 */, 0);
			}
# line 182 "../Io.st"
			{
				activeLen = (60.);
				seq_pvPut(ssId, 47 /* activeLen */, 0);
			}
# line 183 "../Io.st"
			{
				deadFront = (17.5);
				seq_pvPut(ssId, 48 /* deadFront */, 0);
			}
# line 184 "../Io.st"
			{
				deadRear = (17.5);
				seq_pvPut(ssId, 49 /* deadRear */, 0);
			}
# line 185 "../Io.st"
			{
				kapton1 = (.001);
				seq_pvPut(ssId, 50 /* kapton1 */, 0);
			}
# line 186 "../Io.st"
			{
				kapton2 = (.001);
				seq_pvPut(ssId, 51 /* kapton2 */, 0);
			}
# line 187 "../Io.st"
			{
				HePath = (0.);
				seq_pvPut(ssId, 52 /* HePath */, 0);
			}
# line 188 "../Io.st"
			{
				airPath = (0.);
				seq_pvPut(ssId, 53 /* airPath */, 0);
			}
# line 189 "../Io.st"
			{
				Be = (0.);
				seq_pvPut(ssId, 54 /* Be */, 0);
			}
# line 190 "../Io.st"
			{
				dEff = (1.);
				seq_pvPut(ssId, 55 /* dEff */, 0);
			}
# line 191 "../Io.st"
			{
				ArPcntr = (0);
				seq_pvPut(ssId, 56 /* ArPcntr */, 0);
			}
# line 192 "../Io.st"
			{
				flux = (0.);
				seq_pvPut(ssId, 32 /* flux */, 0);
			}
# line 193 "../Io.st"
			{
				ionPhotons = (0.);
				seq_pvPut(ssId, 33 /* ionPhotons */, 0);
			}
# line 194 "../Io.st"
			{
				ionAbs = (1.);
				seq_pvPut(ssId, 34 /* ionAbs */, 0);
			}
# line 195 "../Io.st"
			{
				detector = (0.);
				seq_pvPut(ssId, 35 /* detector */, 0);
			}
# line 197 "../Io.st"
			{
				E_using = (Emono);
				seq_pvPut(ssId, 39 /* E_using */, 0);
			}
# line 197 "../Io.st"
			;
			seq_efClear(ssId, Emono_mon);
			seq_efClear(ssId, E_using_mon);
			seq_efClear(ssId, VperA_mon);
			seq_efClear(ssId, cnt_mon);
			seq_efClear(ssId, ticks_mon);
			seq_efClear(ssId, icChannel_mon);
			seq_efClear(ssId, v2f_mon);
			seq_efClear(ssId, xN2_mon);
			seq_efClear(ssId, xAr_mon);
			seq_efClear(ssId, xHe_mon);
			seq_efClear(ssId, xAir_mon);
			seq_efClear(ssId, xCO2_mon);
			seq_efClear(ssId, activeLen_mon);
			seq_efClear(ssId, deadFront_mon);
			seq_efClear(ssId, deadRear_mon);
			seq_efClear(ssId, kapton1_mon);
			seq_efClear(ssId, kapton2_mon);
			seq_efClear(ssId, HePath_mon);
			seq_efClear(ssId, airPath_mon);
			seq_efClear(ssId, Be_mon);
			seq_efClear(ssId, dEff_mon);
			seq_efClear(ssId, ArPcntr_mon);
# line 221 "../Io.st"
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 221, SNLtaskName, "init complete");
				epicsThreadSleep(0.1);
			}
# line 221 "../Io.st"
			;
		}
		return;
	}
}
/* Code for state "idle" in state set "ionChamber" */

/* Delay function for state "idle" in state set "ionChamber" */
static void D_ionChamber_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 233 "../Io.st"
# line 239 "../Io.st"
# line 247 "../Io.st"
# line 254 "../Io.st"
# line 267 "../Io.st"
# line 276 "../Io.st"
# line 285 "../Io.st"
# line 300 "../Io.st"
# line 311 "../Io.st"
# line 320 "../Io.st"
# line 331 "../Io.st"
# line 338 "../Io.st"
# line 342 "../Io.st"
	seq_delayInit(ssId, 0, (updateRate));
}

/* Event function for state "idle" in state set "ionChamber" */
static long E_ionChamber_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 233 "../Io.st"
	if (seq_efTestAndClear(ssId, IoDebug_mon))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 239 "../Io.st"
	if (seq_efTestAndClear(ssId, E_using_mon))
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
# line 247 "../Io.st"
	if (seq_efTestAndClear(ssId, Emono_mon))
	{
		*pNextState = 2;
		*pTransNum = 2;
		return TRUE;
	}
# line 254 "../Io.st"
	if (seq_efTestAndClear(ssId, VperA_mon))
	{
		*pNextState = 2;
		*pTransNum = 3;
		return TRUE;
	}
# line 267 "../Io.st"
	if (seq_efTestAndClear(ssId, ArPcntr_mon))
	{
		*pNextState = 2;
		*pTransNum = 4;
		return TRUE;
	}
# line 276 "../Io.st"
	if (seq_efTestAndClear(ssId, dEff_mon))
	{
		*pNextState = 2;
		*pTransNum = 5;
		return TRUE;
	}
# line 285 "../Io.st"
	if (seq_efTestAndClear(ssId, icChannel_mon) || seq_efTestAndClear(ssId, v2f_mon))
	{
		*pNextState = 2;
		*pTransNum = 6;
		return TRUE;
	}
# line 300 "../Io.st"
	if (seq_efTestAndClear(ssId, xN2_mon) || seq_efTestAndClear(ssId, xHe_mon) || seq_efTestAndClear(ssId, xAr_mon) || seq_efTestAndClear(ssId, xAir_mon) || seq_efTestAndClear(ssId, xCO2_mon))
	{
		*pNextState = 2;
		*pTransNum = 7;
		return TRUE;
	}
# line 311 "../Io.st"
	if (seq_efTestAndClear(ssId, activeLen_mon) || seq_efTestAndClear(ssId, deadFront_mon) || seq_efTestAndClear(ssId, deadRear_mon))
	{
		*pNextState = 2;
		*pTransNum = 8;
		return TRUE;
	}
# line 320 "../Io.st"
	if (seq_efTestAndClear(ssId, HePath_mon) || seq_efTestAndClear(ssId, airPath_mon))
	{
		*pNextState = 2;
		*pTransNum = 9;
		return TRUE;
	}
# line 331 "../Io.st"
	if (seq_efTestAndClear(ssId, kapton1_mon) || seq_efTestAndClear(ssId, kapton2_mon) || seq_efTestAndClear(ssId, Be_mon))
	{
		*pNextState = 2;
		*pTransNum = 10;
		return TRUE;
	}
# line 338 "../Io.st"
	if (seq_efTestAndClear(ssId, cnt_mon) || seq_efTestAndClear(ssId, ticks_mon))
	{
		*pNextState = 2;
		*pTransNum = 11;
		return TRUE;
	}
# line 342 "../Io.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 2;
		*pTransNum = 12;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "ionChamber" */
static void A_ionChamber_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 228 "../Io.st"
			seq_pvGet(ssId, 36 /* IoDebug */, 0);
			Io_debug = IoDebug;
			seq_efClear(ssId, IoDebug_mon);
			sprintf(new_msg, "changed debug flag to %d", Io_debug);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 232, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 232 "../Io.st"
			;
		}
		return;
	case 1:
		{
# line 236 "../Io.st"
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 236, SNLtaskName, "monochromator energy (pretend) changed");
				epicsThreadSleep(0.1);
			}
# line 236 "../Io.st"
			;
			seq_pvGet(ssId, 39 /* E_using */, 0);
			seq_efClear(ssId, E_using_mon);
		}
		return;
	case 2:
		{
# line 242 "../Io.st"
			if (Io_debug >= 2)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 242, SNLtaskName, "monochromator (actual) changed");
				epicsThreadSleep(0.1);
			}
# line 242 "../Io.st"
			;
			seq_pvGet(ssId, 0 /* Emono */, 0);
			{
				E_using = (Emono);
				seq_pvPut(ssId, 39 /* E_using */, 0);
			}
# line 244 "../Io.st"
			;
			seq_efClear(ssId, E_using_mon);
			seq_efClear(ssId, Emono_mon);
		}
		return;
	case 3:
		{
# line 250 "../Io.st"
			seq_pvGet(ssId, 40 /* VperA */, 0);
			seq_efClear(ssId, VperA_mon);
			sprintf(new_msg, "changed, Current gain to =%g", VperA);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 253, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 253 "../Io.st"
			;
		}
		return;
	case 4:
		{
# line 257 "../Io.st"
			seq_pvGet(ssId, 56 /* ArPcntr */, 0);
			seq_efClear(ssId, ArPcntr_mon);
# line 264 "../Io.st"
			if (ArPcntr)
			{
				dEff = 1. - exp(-(4 * 2.54) * absArPhoto(E_using));
				dEff *= exp(-(.005 * 2.54) * absorb(5, E_using));
				seq_pvPut(ssId, 55 /* dEff */, 0);
				seq_efClear(ssId, dEff_mon);
			}
# line 265 "../Io.st"
			sprintf(new_msg, "changed, ArPcntr=%d,  set dEff = %g", ArPcntr, dEff);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 266, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 266 "../Io.st"
			;
		}
		return;
	case 5:
		{
# line 270 "../Io.st"
			seq_pvGet(ssId, 55 /* dEff */, 0);
			{
				ArPcntr = (0);
				seq_pvPut(ssId, 56 /* ArPcntr */, 0);
			}
# line 272 "../Io.st"
			seq_efClear(ssId, ArPcntr_mon);
			seq_efClear(ssId, dEff_mon);
			sprintf(new_msg, "changed, dEff=%g", dEff);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 275, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 275 "../Io.st"
			;
		}
		return;
	case 6:
		{
# line 279 "../Io.st"
			seq_pvGet(ssId, 37 /* icChannel */, 0);
			seq_pvGet(ssId, 41 /* v2f */, 0);
			seq_efClear(ssId, icChannel_mon);
			seq_efClear(ssId, v2f_mon);
			sprintf(new_msg, "changed, icChannel=%d, v2f=%g", icChannel, v2f);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 284, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 284 "../Io.st"
			;
		}
		return;
	case 7:
		{
# line 288 "../Io.st"
			seq_pvGet(ssId, 42 /* xN2 */, 0);
			seq_pvGet(ssId, 44 /* xHe */, 0);
			seq_pvGet(ssId, 43 /* xAr */, 0);
			seq_pvGet(ssId, 45 /* xAir */, 0);
			seq_pvGet(ssId, 46 /* xCO2 */, 0);
			seq_efClear(ssId, xN2_mon);
			seq_efClear(ssId, xHe_mon);
			seq_efClear(ssId, xAr_mon);
			seq_efClear(ssId, xAir_mon);
			seq_efClear(ssId, xCO2_mon);
			sprintf(new_msg, "changed, xN2=%g, xAr=%g, xHe=%g, xAir=%g, xCO2=%g", xN2, xAr, xHe, xAir, xCO2);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 299, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 299 "../Io.st"
			;
		}
		return;
	case 8:
		{
# line 303 "../Io.st"
			seq_pvGet(ssId, 47 /* activeLen */, 0);
			seq_pvGet(ssId, 48 /* deadFront */, 0);
			seq_pvGet(ssId, 49 /* deadRear */, 0);
			seq_efClear(ssId, activeLen_mon);
			seq_efClear(ssId, deadFront_mon);
			seq_efClear(ssId, deadRear_mon);
			sprintf(new_msg, "changed, activeLen=%g, deadFront=%g, deadRear=%g", activeLen, deadFront, deadRear);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 310, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 310 "../Io.st"
			;
		}
		return;
	case 9:
		{
# line 314 "../Io.st"
			seq_pvGet(ssId, 52 /* HePath */, 0);
			seq_pvGet(ssId, 53 /* airPath */, 0);
			seq_efClear(ssId, HePath_mon);
			seq_efClear(ssId, airPath_mon);
			sprintf(new_msg, "changed, HePath=%g, airPath=%g", HePath, airPath);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 319, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 319 "../Io.st"
			;
		}
		return;
	case 10:
		{
# line 323 "../Io.st"
			seq_pvGet(ssId, 50 /* kapton1 */, 0);
			seq_pvGet(ssId, 51 /* kapton2 */, 0);
			seq_pvGet(ssId, 54 /* Be */, 0);
			seq_efClear(ssId, kapton1_mon);
			seq_efClear(ssId, kapton2_mon);
			seq_efClear(ssId, Be_mon);
			sprintf(new_msg, "changed, kapton1=%g, kapton2=%g, Be=%g", kapton1, kapton2, Be);
			if (Io_debug >= 1)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 330, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 330 "../Io.st"
			;
		}
		return;
	case 11:
		{
# line 334 "../Io.st"
			seq_efClear(ssId, cnt_mon);
			seq_efClear(ssId, ticks_mon);
			sprintf(new_msg, "counter cycled");
			if (Io_debug >= 2)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 337, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 337 "../Io.st"
			;
		}
		return;
	case 12:
		{
# line 341 "../Io.st"
			if (Io_debug >= 3)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 341, SNLtaskName, "periodic update");
				epicsThreadSleep(0.1);
			}
# line 341 "../Io.st"
			;
		}
		return;
	}
}
/* Code for state "UpdateDisplay" in state set "ionChamber" */

/* Delay function for state "UpdateDisplay" in state set "ionChamber" */
static void D_ionChamber_UpdateDisplay(SS_ID ssId, struct UserVar *pVar)
{
# line 371 "../Io.st"
}

/* Event function for state "UpdateDisplay" in state set "ionChamber" */
static long E_ionChamber_UpdateDisplay(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 371 "../Io.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "UpdateDisplay" in state set "ionChamber" */
static void A_ionChamber_UpdateDisplay(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 348 "../Io.st"
			seq_pvGet(ssId, 39 /* E_using */, 0);
			seq_efClear(ssId, E_using_mon);
			EvalFlux(activeLen, deadFront, deadRear, HePath, airPath, Be, E_using);
			if (icChannel == 2)
			{
				seq_pvGet(ssId, 18 /* nm2 */, 0);
				{
					sprintf(icName, nm2);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 352 "../Io.st"
			if (icChannel == 3)
			{
				seq_pvGet(ssId, 19 /* nm3 */, 0);
				{
					sprintf(icName, nm3);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 353 "../Io.st"
			if (icChannel == 4)
			{
				seq_pvGet(ssId, 20 /* nm4 */, 0);
				{
					sprintf(icName, nm4);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 354 "../Io.st"
			if (icChannel == 5)
			{
				seq_pvGet(ssId, 21 /* nm5 */, 0);
				{
					sprintf(icName, nm5);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 355 "../Io.st"
			if (icChannel == 6)
			{
				seq_pvGet(ssId, 22 /* nm6 */, 0);
				{
					sprintf(icName, nm6);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 356 "../Io.st"
			if (icChannel == 7)
			{
				seq_pvGet(ssId, 23 /* nm7 */, 0);
				{
					sprintf(icName, nm7);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 357 "../Io.st"
			if (icChannel == 8)
			{
				seq_pvGet(ssId, 24 /* nm8 */, 0);
				{
					sprintf(icName, nm8);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 358 "../Io.st"
			if (icChannel == 9)
			{
				seq_pvGet(ssId, 25 /* nm9 */, 0);
				{
					sprintf(icName, nm9);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 359 "../Io.st"
			if (icChannel == 10)
			{
				seq_pvGet(ssId, 26 /* nm10 */, 0);
				{
					sprintf(icName, nm10);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 360 "../Io.st"
			if (icChannel == 11)
			{
				seq_pvGet(ssId, 27 /* nm11 */, 0);
				{
					sprintf(icName, nm11);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 361 "../Io.st"
			if (icChannel == 12)
			{
				seq_pvGet(ssId, 28 /* nm12 */, 0);
				{
					sprintf(icName, nm12);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 362 "../Io.st"
			if (icChannel == 13)
			{
				seq_pvGet(ssId, 29 /* nm13 */, 0);
				{
					sprintf(icName, nm13);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 363 "../Io.st"
			if (icChannel == 14)
			{
				seq_pvGet(ssId, 30 /* nm14 */, 0);
				{
					sprintf(icName, nm14);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 364 "../Io.st"
			if (icChannel == 15)
			{
				seq_pvGet(ssId, 31 /* nm15 */, 0);
				{
					sprintf(icName, nm15);
					seq_pvPut(ssId, 38 /* icName */, 0);
				}
			}
# line 365 "../Io.st"
			seq_pvPut(ssId, 32 /* flux */, 0);
			seq_pvPut(ssId, 33 /* ionPhotons */, 0);
			seq_pvPut(ssId, 34 /* ionAbs */, 0);
			seq_pvPut(ssId, 35 /* detector */, 0);
			sprintf(new_msg, "   in Updatedisplay, flux = %g,   detector = %g", flux, detector);
			if (Io_debug >= 3)
			{
				printf("<%s,%d,%s> %s\n", "../Io.st", 370, SNLtaskName, new_msg);
				epicsThreadSleep(0.1);
			}
# line 370 "../Io.st"
			;
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
  {"{MONO}", (void *)&Emono, "Emono", 
    "double", 1, 24, 1, 1, 0, 0, 0},

  {"{VSC}.CNT", (void *)&cnt, "cnt", 
    "short", 1, 25, 2, 1, 0, 0, 0},

  {"{VSC}.FREQ", (void *)&clock_rate, "clock_rate", 
    "double", 1, 26, 0, 1, 0, 0, 0},

  {"{VSC}.S1", (void *)&ticks, "ticks", 
    "double", 1, 27, 3, 1, 0, 0, 0},

  {"{VSC}.S2", (void *)&s2, "s2", 
    "double", 1, 28, 0, 1, 0, 0, 0},

  {"{VSC}.S3", (void *)&s3, "s3", 
    "double", 1, 29, 0, 1, 0, 0, 0},

  {"{VSC}.S4", (void *)&s4, "s4", 
    "double", 1, 30, 0, 1, 0, 0, 0},

  {"{VSC}.S5", (void *)&s5, "s5", 
    "double", 1, 31, 0, 1, 0, 0, 0},

  {"{VSC}.S6", (void *)&s6, "s6", 
    "double", 1, 32, 0, 1, 0, 0, 0},

  {"{VSC}.S7", (void *)&s7, "s7", 
    "double", 1, 33, 0, 1, 0, 0, 0},

  {"{VSC}.S8", (void *)&s8, "s8", 
    "double", 1, 34, 0, 1, 0, 0, 0},

  {"{VSC}.S9", (void *)&s9, "s9", 
    "double", 1, 35, 0, 1, 0, 0, 0},

  {"{VSC}.S10", (void *)&s10, "s10", 
    "double", 1, 36, 0, 1, 0, 0, 0},

  {"{VSC}.S11", (void *)&s11, "s11", 
    "double", 1, 37, 0, 1, 0, 0, 0},

  {"{VSC}.S12", (void *)&s12, "s12", 
    "double", 1, 38, 0, 1, 0, 0, 0},

  {"{VSC}.S13", (void *)&s13, "s13", 
    "double", 1, 39, 0, 1, 0, 0, 0},

  {"{VSC}.S14", (void *)&s14, "s14", 
    "double", 1, 40, 0, 1, 0, 0, 0},

  {"{VSC}.S15", (void *)&s15, "s15", 
    "double", 1, 41, 0, 1, 0, 0, 0},

  {"{VSC}.NM2", (void *)&nm2[0], "nm2", 
    "string", 1, 42, 0, 0, 0, 0, 0},

  {"{VSC}.NM3", (void *)&nm3[0], "nm3", 
    "string", 1, 43, 0, 0, 0, 0, 0},

  {"{VSC}.NM4", (void *)&nm4[0], "nm4", 
    "string", 1, 44, 0, 0, 0, 0, 0},

  {"{VSC}.NM5", (void *)&nm5[0], "nm5", 
    "string", 1, 45, 0, 0, 0, 0, 0},

  {"{VSC}.NM6", (void *)&nm6[0], "nm6", 
    "string", 1, 46, 0, 0, 0, 0, 0},

  {"{VSC}.NM7", (void *)&nm7[0], "nm7", 
    "string", 1, 47, 0, 0, 0, 0, 0},

  {"{VSC}.NM8", (void *)&nm8[0], "nm8", 
    "string", 1, 48, 0, 0, 0, 0, 0},

  {"{VSC}.NM9", (void *)&nm9[0], "nm9", 
    "string", 1, 49, 0, 0, 0, 0, 0},

  {"{VSC}.NM10", (void *)&nm10[0], "nm10", 
    "string", 1, 50, 0, 0, 0, 0, 0},

  {"{VSC}.NM11", (void *)&nm11[0], "nm11", 
    "string", 1, 51, 0, 0, 0, 0, 0},

  {"{VSC}.NM12", (void *)&nm12[0], "nm12", 
    "string", 1, 52, 0, 0, 0, 0, 0},

  {"{VSC}.NM13", (void *)&nm13[0], "nm13", 
    "string", 1, 53, 0, 0, 0, 0, 0},

  {"{VSC}.NM14", (void *)&nm14[0], "nm14", 
    "string", 1, 54, 0, 0, 0, 0, 0},

  {"{VSC}.NM15", (void *)&nm15[0], "nm15", 
    "string", 1, 55, 0, 0, 0, 0, 0},

  {"{P}flux", (void *)&flux, "flux", 
    "double", 1, 56, 0, 0, 0, 0, 0},

  {"{P}ionPhotons", (void *)&ionPhotons, "ionPhotons", 
    "double", 1, 57, 0, 0, 0, 0, 0},

  {"{P}ionAbs", (void *)&ionAbs, "ionAbs", 
    "double", 1, 58, 0, 0, 0, 0, 0},

  {"{P}detector", (void *)&detector, "detector", 
    "double", 1, 59, 0, 0, 0, 0, 0},

  {"{P}debug", (void *)&IoDebug, "IoDebug", 
    "short", 1, 60, 4, 1, 0, 0, 0},

  {"{P}scaler", (void *)&icChannel, "icChannel", 
    "short", 1, 61, 5, 1, 0, 0, 0},

  {"{P}scaler.DESC", (void *)&icName[0], "icName", 
    "string", 1, 62, 0, 0, 0, 0, 0},

  {"{P}E_using", (void *)&E_using, "E_using", 
    "double", 1, 63, 6, 1, 0, 0, 0},

  {"{P}VperA", (void *)&VperA, "VperA", 
    "double", 1, 64, 7, 1, 0, 0, 0},

  {"{P}v2f", (void *)&v2f, "v2f", 
    "double", 1, 65, 8, 1, 0, 0, 0},

  {"{P}xN2", (void *)&xN2, "xN2", 
    "double", 1, 66, 9, 1, 0, 0, 0},

  {"{P}xAr", (void *)&xAr, "xAr", 
    "double", 1, 67, 10, 1, 0, 0, 0},

  {"{P}xHe", (void *)&xHe, "xHe", 
    "double", 1, 68, 11, 1, 0, 0, 0},

  {"{P}xAir", (void *)&xAir, "xAir", 
    "double", 1, 69, 12, 1, 0, 0, 0},

  {"{P}xCO2", (void *)&xCO2, "xCO2", 
    "double", 1, 70, 13, 1, 0, 0, 0},

  {"{P}activeLen", (void *)&activeLen, "activeLen", 
    "double", 1, 71, 14, 1, 0, 0, 0},

  {"{P}deadFront", (void *)&deadFront, "deadFront", 
    "double", 1, 72, 15, 1, 0, 0, 0},

  {"{P}deadRear", (void *)&deadRear, "deadRear", 
    "double", 1, 73, 16, 1, 0, 0, 0},

  {"{P}kapton1", (void *)&kapton1, "kapton1", 
    "double", 1, 74, 17, 1, 0, 0, 0},

  {"{P}kapton2", (void *)&kapton2, "kapton2", 
    "double", 1, 75, 18, 1, 0, 0, 0},

  {"{P}HePath", (void *)&HePath, "HePath", 
    "double", 1, 76, 19, 1, 0, 0, 0},

  {"{P}airPath", (void *)&airPath, "airPath", 
    "double", 1, 77, 20, 1, 0, 0, 0},

  {"{P}Be", (void *)&Be, "Be", 
    "double", 1, 78, 21, 1, 0, 0, 0},

  {"{P}efficiency", (void *)&dEff, "dEff", 
    "double", 1, 79, 22, 1, 0, 0, 0},

  {"{P}ArPcntr", (void *)&ArPcntr, "ArPcntr", 
    "short", 1, 80, 23, 1, 0, 0, 0},

};

/* Event masks for state set ionChamber */
	/* Event mask for state init: */
static bitMask	EM_ionChamber_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state idle: */
static bitMask	EM_ionChamber_idle[] = {
	0x00fffffe,
	0x00000000,
	0x00000000,
};
	/* Event mask for state UpdateDisplay: */
static bitMask	EM_ionChamber_UpdateDisplay[] = {
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_ionChamber[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_ionChamber_init,
	/* event function */  (EVENT_FUNC) E_ionChamber_init,
	/* delay function */   (DELAY_FUNC) D_ionChamber_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ionChamber_init,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_ionChamber_idle,
	/* event function */  (EVENT_FUNC) E_ionChamber_idle,
	/* delay function */   (DELAY_FUNC) D_ionChamber_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ionChamber_idle,
	/* state options */   (0)},

	/* State "UpdateDisplay" */ {
	/* state name */       "UpdateDisplay",
	/* action function */ (ACTION_FUNC) A_ionChamber_UpdateDisplay,
	/* event function */  (EVENT_FUNC) E_ionChamber_UpdateDisplay,
	/* delay function */   (DELAY_FUNC) D_ionChamber_UpdateDisplay,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ionChamber_UpdateDisplay,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "ionChamber" */ {
	/* ss name */            "ionChamber",
	/* ptr to state block */ state_ionChamber,
	/* number of states */   3,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=7idd:Io:,MONO=7id:HHLM:energy_get,VSC=7idd:scaler1";

/* State Program table (global) */
struct seqProgram Io = {
	/* magic number */       20000315,
	/* *name */              "Io",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ 0,
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
/* Global C code */
# line 381 "../Io.st"



void EvalFlux(
double aln,
double dln1,
double dln2,
double dHe,
double dAir,
double dBe,
double energy)
{

 double AirAbs;
 double HeAbs;
 double KapAbs;
 double BeAbs;
 double fillAbsorb;
 double frontAbs;
 double cps;


 aln = aln / 10.;
 dln1 = dln1 / 10.;
 dln2 = dln2 / 10.;
 dHe = dHe / 10.;
 dAir = dAir / 10.;
 dBe = dBe * 2.54;

 switch (icChannel) {
 case 2: cps = s2; break;
 case 3: cps = s3; break;
 case 4: cps = s4; break;
 case 5: cps = s5; break;
 case 6: cps = s6; break;
 case 7: cps = s7; break;
 case 8: cps = s8; break;
 case 9: cps = s9; break;
 case 10: cps = s10; break;
 case 11: cps = s11; break;
 case 12: cps = s12; break;
 case 13: cps = s13; break;
 case 14: cps = s14; break;
 case 15: cps = s15; break;
 default:
  cps = 0;
 }
 cps /= (ticks/clock_rate);


 flux = 0.;
 flux += xHe * photon(cps,29.6, v2f,VperA,aln,dln1,kapton1,1,energy);
 flux += xN2 * photon(cps,36.3, v2f,VperA,aln,dln1,kapton1,2,energy);
 flux += xAr * photon(cps,24.4, v2f,VperA,aln,dln1,kapton1,3,energy);
 flux += xAir* photon(cps,35.36,v2f,VperA,aln,dln1,kapton1,4,energy);
 flux += xCO2* photon(cps,35.36,v2f,VperA,aln,dln1,kapton1,7,energy);


 AirAbs = exp(-dAir*absorb(4,energy));
 HeAbs = exp(-dHe * absorb(1,energy) );
 KapAbs = exp(-(kapton2+kapton1)*2.54 * absorb(6,energy) );
 BeAbs = exp(-dBe * absorb(5,energy) );
 fillAbsorb = absorb(1,energy)*xHe + absorb(2,energy)*xN2 + absorb(3,energy)*xAr + absorb(4,energy)*xAir + absorb(7,energy)*xCO2;

 ionAbs = exp(-(dln1+dln2+aln) * fillAbsorb);
 detector = flux * AirAbs * HeAbs * KapAbs * BeAbs * ionAbs * dEff;
 frontAbs = exp( -2.54*kapton1*absorb(6,energy) - dln1*fillAbsorb );
 ionPhotons = (1.- exp(-aln * fillAbsorb)) * frontAbs * flux;
}


double photon(
double cps,
double work,
double VtoF,
double Volt_per_A,
double aln,
double deadL,
double Kapton,
int gasId,
double energy)
{


 double convHe = 6.647;
 double rhoHe = 0.0001785;
 double convN = 23.26;
 double rhoN = 0.00125;
 double convAr = 66.32;
 double rhoAr = 0.001784;
 double edgeAr = 3.202;
 double sum,E1,E2,E3;
 double part1,part2,part3;
 double photo;

 static double aHe[4] = { 6.06488, -3.29055, -0.107256, 0.0144465 };
 static double aN[4] = { 11.2765, -2.65400, -0.200445, 0.0200765 };
 static double aAr1[4] = { 13.9491, -1.82276, -0.328827, 0.0274382 };
 static double aAr2[4] = { 12.2960, -2.63279, -0.073660, 0.0 };
 static double aO[4] = { 11.7130, -2.57229, -0.205893, 0.0199244 };
 static double aC[4] = { 10.6879, -2.71400, -0.200530, 0.0207248 };

 E1 = log(energy);
 E2 = E1 * E1;
 E3 = E2 * E1;
 photo = 0;


 if (gasId == 1) {
  sum = exp(aHe[0] + aHe[1]*E1 + aHe[2]*E2 + aHe[3]*E3);
  photo = sum * rhoHe / convHe;
 }
 else if (gasId == 2) {
  sum = exp(aN[0] + aN[1]*E1 + aN[2]*E2 + aN[3]*E3);
  photo = sum * rhoN / convN;
 }
 else if (gasId == 3) {
  if (E1 > edgeAr) sum = exp(aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
  else sum = exp(aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);
  photo = sum * rhoAr / convAr;
 }
 else if (gasId == 4) {
  sum = exp(aN[0] + aN[1]*E1 + aN[2]*E2 + aN[3]*E3);
  photo = sum * 0.000922 / convN;
  sum = exp(aO[0] + aO[1]*E1 + aO[2]*E2 + aO[3]*E3);
  photo += sum * 0.000266 / convN;
  if (E1 > edgeAr) sum = exp(aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
  else sum = exp(aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);
  photo += sum * 1.66E-5 / convAr;
 }
 else if (gasId == 7) {
  sum = exp(aC[0] + aC[1]*E1 + aC[2]*E2 + aC[3]*E3);
  photo = sum * 0.0005396 / convN;
  sum = exp(aO[0] + aO[1]*E1 + aO[2]*E2 + aO[3]*E3);
  photo += sum * 0.0014374 / convN;
 }


 part1 = cps * work / (1.602E-19 * VtoF * Volt_per_A * energy * 1000.);
 part2 = 1 - exp(-photo * aln);
 part3 = exp(absorb(gasId,energy) * deadL) * exp(absorb(6,energy) * Kapton * 2.54);

 return (part1 * part3 / part2);
}


double absorb(
int id,
double energy)
{
# 540 "../Io.st"
 double rhoH;
 double rhoHe;
 double rhoBe;
 double rhoC;
 double rhoN;
 double rhoO;
 double rhoAr;

 switch (id) {
 case 1:
  rhoHe = 0.0001785;
  return(absHe(energy)*rhoHe);
 case 5:
  rhoBe = 1.848;
  return(absBe(energy)*rhoBe);
 case 2:
  rhoN = 0.00125;
  return(absN(energy)*rhoN);
 case 4:
  rhoN = 0.000922;
  rhoO = 0.000266;
  rhoAr = 1.66E-5;
  return(absN(energy)*rhoN + absO(energy)*rhoO + absAr(energy)*rhoAr);
 case 3:
  rhoAr = 0.001784;
  return(absAr(energy)*rhoAr);
 case 6:
  rhoC = 0.981;
  rhoH = 0.037;
  rhoO = 0.297;
  rhoN = 0.105;
  return(absC(energy)*rhoC + absH(energy)*rhoH + absO(energy)*rhoO + absN(energy)*rhoN);
 case 7:
  rhoC = 0.0005396;
  rhoO = 0.0014374;
  return(absC(energy)*rhoC + absO(energy)*rhoO);
 }
 return 0;
}





double absH(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 2.44964E+00,-3.34953E+00,-4.71370E-02, 7.09962E-03 };
 static double b[4] = {-1.19075E-01,-9.37086E-01,-2.00538E-01, 1.06587E-02 };
 static double c[4] = {-2.15772E+00, 1.32685E+00,-3.05620E-01, 1.85025E-02 };


 conv = 1.674;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return( (photo+coherent+compton)/conv);
}





double absHe(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 6.06488E+00,-3.29055E+00,-1.07256E-01, 1.44465E-02 };
 static double b[4] = { 1.04768E+00,-8.51805E-02,-4.03527E-01, 2.69398E-02 };
 static double c[4] = {-2.56357E+00, 2.02536E+00,-4.48710E-01, 2.79691E-02 };


 conv = 6.647;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absBe(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 9.04511E+00,-2.83487E+00,-2.10021E-01, 2.29526E-02 };
 static double b[4] = { 2.00860E+00,-4.61920E-02,-3.37018E-01, 1.86939E-02 };
 static double c[4] = {-6.90079E-01, 9.46448E-01,-1.71142E-01, 6.51413E-03 };


 conv = 14.96;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absC(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.06879E+01,-2.71400E+00,-2.00530E-01, 2.07248E-02 };
 static double b[4] = { 3.10861E+00,-2.60580E-01,-2.71974E-01, 1.35181E-02 };
 static double c[4] = {-9.82878E-01, 1.46693E+00,-2.93743E-01, 1.56005E-02 };


 conv = 19.94;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absN(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.12765E+01,-2.65400E+00,-2.00445E-01, 2.00765E-02 };
 static double b[4] = { 3.47760E+00,-2.15762E-01,-2.88874E-01, 1.51312E-02 };
 static double c[4] = {-1.23693E+00, 1.74510E+00,-3.54660E-01, 1.98705E-02 };


 conv = 23.26;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absO(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.17130E+01,-2.57229E+00,-2.05893E-01, 1.99244E-02 };
 static double b[4] = { 3.77239E+00,-1.48539E-01,-3.07124E-01, 1.67303E-02 };
 static double c[4] = {-1.73679E+00, 2.17686E+00,-4.49050E-01, 2.64733E-02 };


 conv = 26.57;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absAr(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 double edge;
 static double a1[4] = { 1.39491E+01,-1.82276E+00,-3.28827E-01, 2.74382E-02 };
 static double a2[4] = { 1.22960E+01,-2.63279E+00,-7.36600E-02, 0.};
 static double b[4] = { 5.21079E+00, 1.35618E-01,-3.47214E-01, 1.84333E-02 };
 static double c[4] = {-6.82105E-01, 1.74279E+00,-3.17646E-01, 1.56467E-02 };


 conv = 66.32;
 edge = 3.202;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 if (energy>edge) photo = exp( a1[0] + a1[1]*E1 + a1[2]*E2 + a1[3]*E3);
 else photo = exp( a2[0] + a2[1]*E1 + a2[2]*E2 + a2[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}


double absArPhoto(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double rho;
 double edge;
 static double aAr1[4] = {13.9491, -1.82276, -0.328827, 0.0274382 };
 static double aAr2[4] = {12.2960, -2.63279, -0.073660, 0.0 };

 conv = 66.32;
 rho = 0.001784;
 edge = 3.202;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 if (energy>edge) photo = exp( aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
 else photo = exp( aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);

 return (photo / conv * rho);
}



#include "epicsExport.h"


/* Register sequencer commands and program */

void IoRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&Io);
}
epicsExportRegistrar(IoRegistrar);

