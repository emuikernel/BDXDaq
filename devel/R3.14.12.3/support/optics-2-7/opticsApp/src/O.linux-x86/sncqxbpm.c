/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: sncqxbpm.i */

/* Event flags */
#define s_ainp_mon	1
#define signal_mode_mon	2
#define buflen_mon	3
#define gain_mon	4
#define period_s_mon	5
#define started	6

/* Program "sncqxbpm" */
#include "seqCom.h"

#define NUM_SS 2
#define NUM_CHANNELS 109
#define NUM_EVENTS 6
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT TRUE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram sncqxbpm;

/* Variable declarations */
static char	s_ainp[MAX_STRING_SIZE];
static char	s_aout[MAX_STRING_SIZE];
static short	s_baud;
static short	s_dbit;
static short	s_fctl;
static char	s_idel[MAX_STRING_SIZE];
static short	s_ifmt;
static short	s_nord;
static short	s_nrrd;
static char	s_odel[MAX_STRING_SIZE];
static short	s_ofmt;
static short	s_proc;
static short	s_prty;
static short	s_sbit;
static short	s_scan;
static int	s_sevr;
static short	s_tmod;
static double	s_tmot;
static char	port[MAX_STRING_SIZE];
static int	bpm_address;
static short	init;
static short	enable;
static int	error;
static char	errMsg[MAX_STRING_SIZE];
static int	debug_flag;
static double	pos_x;
static double	pos_y;
static double	current_total;
static double	current_a;
static double	current_b;
static double	current_c;
static double	current_d;
static unsigned long	raw_a;
static unsigned long	raw_b;
static unsigned long	raw_c;
static unsigned long	raw_d;
static long	current_low_raw;
static short	current_low;
static short	current_ok;
static short	signal_mode;
static short	buflen;
static short	buflen_lo;
static short	buflen_hi;
static int	gain;
static double	period_s;
static double	period_s_LOPR;
static double	period_s_HOPR;
static double	gx;
static double	gy;
static short	set_offsets;
static short	set_defaults;
static double	settling;
static char	gainStr[6][MAX_STRING_SIZE];
static char	signal_modeStr[3][MAX_STRING_SIZE];
static double	gainTrim[24];
static long	offset[24];
static char	scratch[256];
static int	i;
static char	bpm_command[MAX_STRING_SIZE];
static char	bpm_response[MAX_STRING_SIZE];
static char	SNLtaskName[MAX_STRING_SIZE];
static int	retry;
static double	updateDelay;
static int	oldGain;
static int	newGain;
static int	channel;
static double	factor;
static long	long_internal;
static char	debug_str[MAX_STRING_SIZE];

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 281 "../sncqxbpm.st"
 #include <math.h>
# line 282 "../sncqxbpm.st"
 #include <string.h>
# line 283 "../sncqxbpm.st"
 #include <epicsThread.h>

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "startup" in state set "sncqxbpm" */

/* Delay function for state "startup" in state set "sncqxbpm" */
static void D_sncqxbpm_startup(SS_ID ssId, struct UserVar *pVar)
{
# line 315 "../sncqxbpm.st"
}

/* Event function for state "startup" in state set "sncqxbpm" */
static long E_sncqxbpm_startup(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 315 "../sncqxbpm.st"
	if (TRUE)
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "startup" in state set "sncqxbpm" */
static void A_sncqxbpm_startup(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 297 "../sncqxbpm.st"
			seq_efClear(ssId, started);
# line 299 "../sncqxbpm.st"
			seq_pvGet(ssId, 0 /* s_ainp */, 0);
			seq_pvGet(ssId, 1 /* s_aout */, 0);
			{
				init = (1);
				seq_pvPut(ssId, 20 /* init */, 2);
			}
# line 301 "../sncqxbpm.st"
			;
			{
				period_s_LOPR = (0.05);
				seq_pvPut(ssId, 45 /* period_s_LOPR */, 2);
			}
# line 302 "../sncqxbpm.st"
			;
# line 307 "../sncqxbpm.st"
			strcpy(SNLtaskName, seq_macValueGet(ssId, "name"));
			strcpy(port, seq_macValueGet(ssId, "S"));
			seq_pvPut(ssId, 18 /* port */, 0);
			{
				current_low = (1);
				seq_pvPut(ssId, 37 /* current_low */, 2);
			}
# line 310 "../sncqxbpm.st"
			;
			{
				current_ok = (0);
				seq_pvPut(ssId, 38 /* current_ok */, 2);
			}
# line 311 "../sncqxbpm.st"
			;
			{
				set_offsets = (0);
				seq_pvPut(ssId, 49 /* set_offsets */, 2);
			}
# line 312 "../sncqxbpm.st"
			;
			updateDelay = 3;
			seq_efSet(ssId, started);
		}
		return;
	}
}
/* Code for state "init" in state set "sncqxbpm" */

/* Delay function for state "init" in state set "sncqxbpm" */
static void D_sncqxbpm_init(SS_ID ssId, struct UserVar *pVar)
{
# line 322 "../sncqxbpm.st"
# line 368 "../sncqxbpm.st"
}

/* Event function for state "init" in state set "sncqxbpm" */
static long E_sncqxbpm_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 322 "../sncqxbpm.st"
	if (!enable)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 368 "../sncqxbpm.st"
	if (init)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "sncqxbpm" */
static void A_sncqxbpm_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 321 "../sncqxbpm.st"
			if (debug_flag >= 0)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 321, SNLtaskName, 0);
				;
				printf("%s\n", "software disabled");
				epicsThreadSleep(0.01);
			}
# line 321 "../sncqxbpm.st"
			;
		}
		return;
	case 1:
		{
# line 327 "../sncqxbpm.st"
			{
				init = (0);
				seq_pvPut(ssId, 20 /* init */, 2);
			}
# line 327 "../sncqxbpm.st"
			;
# line 331 "../sncqxbpm.st"
			{
				s_baud = (7);
				seq_pvPut(ssId, 2 /* s_baud */, 2);
			}
# line 331 "../sncqxbpm.st"
			;
# line 331 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 331 "../sncqxbpm.st"
			;
			{
				s_dbit = (4);
				seq_pvPut(ssId, 3 /* s_dbit */, 2);
			}
# line 332 "../sncqxbpm.st"
			;
# line 332 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 332 "../sncqxbpm.st"
			;
			{
				s_prty = (1);
				seq_pvPut(ssId, 12 /* s_prty */, 2);
			}
# line 333 "../sncqxbpm.st"
			;
# line 333 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 333 "../sncqxbpm.st"
			;
			{
				s_sbit = (1);
				seq_pvPut(ssId, 13 /* s_sbit */, 2);
			}
# line 334 "../sncqxbpm.st"
			;
# line 334 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 334 "../sncqxbpm.st"
			;
			{
				s_fctl = (1);
				seq_pvPut(ssId, 4 /* s_fctl */, 2);
			}
# line 335 "../sncqxbpm.st"
			;
# line 335 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 335 "../sncqxbpm.st"
			;
			{
				s_tmot = (0.250);
				seq_pvPut(ssId, 17 /* s_tmot */, 2);
			}
# line 336 "../sncqxbpm.st"
			;
			{
				s_ofmt = (0);
				seq_pvPut(ssId, 10 /* s_ofmt */, 2);
			}
# line 337 "../sncqxbpm.st"
			;
			{
				s_ifmt = (0);
				seq_pvPut(ssId, 6 /* s_ifmt */, 2);
			}
# line 338 "../sncqxbpm.st"
			;
			{
				strcpy(s_odel, "\n");
				seq_pvPut(ssId, 9 /* s_odel */, 2);
				epicsThreadSleep(0.01);
			}
# line 339 "../sncqxbpm.st"
			;
# line 339 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 339 "../sncqxbpm.st"
			;
			{
				strcpy(s_idel, "\n");
				seq_pvPut(ssId, 5 /* s_idel */, 2);
				epicsThreadSleep(0.01);
			}
# line 340 "../sncqxbpm.st"
			;
# line 340 "../sncqxbpm.st"
			{
				epicsThreadSleep(2 / 60.);
			}
# line 340 "../sncqxbpm.st"
			;
			{
				s_nrrd = (0);
				seq_pvPut(ssId, 8 /* s_nrrd */, 2);
			}
# line 341 "../sncqxbpm.st"
			;
			{
				s_scan = (0);
				seq_pvPut(ssId, 14 /* s_scan */, 2);
			}
# line 342 "../sncqxbpm.st"
			;
			seq_efClear(ssId, s_ainp_mon);
# line 357 "../sncqxbpm.st"
			{
				sprintf(bpm_command, "*RST%d", bpm_address, 0, 0, 0);
				sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command);
				if (debug_flag >= 15)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 357, SNLtaskName, 15);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				if (s_tmod != 1)
				{
					s_tmod = (1);
					seq_pvPut(ssId, 16 /* s_tmod */, 2);
				}
				;
				{
					epicsThreadSleep(1 / 60.);
				}
				;
				if (1 == 1)
				{
					if (strcmp(s_aout, bpm_command))
					{
						{
							strcpy(s_aout, bpm_command);
							seq_pvPut(ssId, 1 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
					else
					{
						{
							s_proc = (1);
							seq_pvPut(ssId, 11 /* s_proc */, 2);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
				}
				else
				{
					strcpy(bpm_response, "");
					for (retry = 0; retry < 3; retry++)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						for (i = 0; i < 0.25; i++)
						{
							{
								epicsThreadSleep(1 / 60.);
							}
							;
							if (seq_efTest(ssId, s_ainp_mon))
								break;
						}
						if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
						{
							sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
							if (debug_flag >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 357, SNLtaskName, 10);
								;
								printf("%s\n", debug_str);
								epicsThreadSleep(0.01);
							}
							;
							strcpy(bpm_response, s_ainp + 1);
							break;
						}
					}
				}
			}
# line 357 "../sncqxbpm.st"
			;
			{
				epicsThreadSleep(6 / 60.);
			}
# line 358 "../sncqxbpm.st"
			;
			seq_efSet(ssId, gain_mon);
			seq_efSet(ssId, signal_mode_mon);
			seq_efSet(ssId, period_s_mon);
		}
		return;
	}
}
/* Code for state "disable" in state set "sncqxbpm" */

/* Delay function for state "disable" in state set "sncqxbpm" */
static void D_sncqxbpm_disable(SS_ID ssId, struct UserVar *pVar)
{
# line 380 "../sncqxbpm.st"
}

/* Event function for state "disable" in state set "sncqxbpm" */
static long E_sncqxbpm_disable(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 380 "../sncqxbpm.st"
	if (enable)
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "disable" in state set "sncqxbpm" */
static void A_sncqxbpm_disable(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 378 "../sncqxbpm.st"
			{
				init = (1);
				seq_pvPut(ssId, 20 /* init */, 2);
			}
# line 378 "../sncqxbpm.st"
			;
			if (debug_flag >= 0)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 379, SNLtaskName, 0);
				;
				printf("%s\n", "software re-enabled");
				epicsThreadSleep(0.01);
			}
# line 379 "../sncqxbpm.st"
			;
		}
		return;
	}
}
/* Code for state "comm_error" in state set "sncqxbpm" */

/* Delay function for state "comm_error" in state set "sncqxbpm" */
static void D_sncqxbpm_comm_error(SS_ID ssId, struct UserVar *pVar)
{
# line 392 "../sncqxbpm.st"
	seq_delayInit(ssId, 0, ((0.5 * 60.0)));
}

/* Event function for state "comm_error" in state set "sncqxbpm" */
static long E_sncqxbpm_comm_error(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 392 "../sncqxbpm.st"
	if (init || !s_sevr || seq_delay(ssId, 0))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "comm_error" in state set "sncqxbpm" */
static void A_sncqxbpm_comm_error(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 388 "../sncqxbpm.st"
			sprintf(debug_str, "comm_error: init=%d  s_sevr=%d", init, s_sevr);
			if (debug_flag >= 20)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 389, SNLtaskName, 20);
				;
				printf("%s\n", debug_str);
				epicsThreadSleep(0.01);
			}
# line 389 "../sncqxbpm.st"
			;
			s_sevr = 0;
			{
				init = (1);
				seq_pvPut(ssId, 20 /* init */, 2);
			}
# line 391 "../sncqxbpm.st"
			;
		}
		return;
	}
}
/* Code for state "idle" in state set "sncqxbpm" */

/* Delay function for state "idle" in state set "sncqxbpm" */
static void D_sncqxbpm_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 399 "../sncqxbpm.st"
# line 405 "../sncqxbpm.st"
# line 409 "../sncqxbpm.st"
# line 422 "../sncqxbpm.st"
# line 449 "../sncqxbpm.st"
# line 456 "../sncqxbpm.st"
# line 504 "../sncqxbpm.st"
	seq_delayInit(ssId, 0, (period_s - 0.05 + updateDelay));
}

/* Event function for state "idle" in state set "sncqxbpm" */
static long E_sncqxbpm_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 399 "../sncqxbpm.st"
	if (!enable)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 405 "../sncqxbpm.st"
	if (s_sevr)
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 409 "../sncqxbpm.st"
	if (init)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 422 "../sncqxbpm.st"
	if (seq_efTestAndClear(ssId, gain_mon))
	{
		*pNextState = 4;
		*pTransNum = 3;
		return TRUE;
	}
# line 449 "../sncqxbpm.st"
	if (seq_efTestAndClear(ssId, signal_mode_mon) || seq_efTestAndClear(ssId, buflen_mon))
	{
		*pNextState = 4;
		*pTransNum = 4;
		return TRUE;
	}
# line 456 "../sncqxbpm.st"
	if (seq_efTestAndClear(ssId, period_s_mon))
	{
		*pNextState = 4;
		*pTransNum = 5;
		return TRUE;
	}
# line 504 "../sncqxbpm.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 4;
		*pTransNum = 6;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "sncqxbpm" */
static void A_sncqxbpm_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
# line 402 "../sncqxbpm.st"
			{
				error = (2);
				seq_pvPut(ssId, 22 /* error */, 2);
			}
# line 402 "../sncqxbpm.st"
			;
			sprintf(scratch, "communications error: %d", s_sevr);
			{
				strcpy(errMsg, scratch);
				seq_pvPut(ssId, 23 /* errMsg */, 2);
				epicsThreadSleep(0.01);
			}
# line 404 "../sncqxbpm.st"
			;
		}
		return;
	case 2:
		{
# line 408 "../sncqxbpm.st"
			if (debug_flag >= 20)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 408, SNLtaskName, 20);
				;
				printf("%s\n", "user re-initialized");
				epicsThreadSleep(0.01);
			}
# line 408 "../sncqxbpm.st"
			;
		}
		return;
	case 3:
		{
# line 413 "../sncqxbpm.st"
			sprintf(debug_str, "gain changed to %s (%d)", gainStr[gain], gain);
			if (debug_flag >= 5)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 414, SNLtaskName, 5);
				;
				printf("%s\n", debug_str);
				epicsThreadSleep(0.01);
			}
# line 414 "../sncqxbpm.st"
			;
			{
				sprintf(bpm_command, ":CONF%d:CURR:RANG %d", bpm_address, gain + 1, 0, 0);
				sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command);
				if (debug_flag >= 15)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 415, SNLtaskName, 15);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				if (s_tmod != 1)
				{
					s_tmod = (1);
					seq_pvPut(ssId, 16 /* s_tmod */, 2);
				}
				;
				{
					epicsThreadSleep(1 / 60.);
				}
				;
				if (1 == 1)
				{
					if (strcmp(s_aout, bpm_command))
					{
						{
							strcpy(s_aout, bpm_command);
							seq_pvPut(ssId, 1 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
					else
					{
						{
							s_proc = (1);
							seq_pvPut(ssId, 11 /* s_proc */, 2);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
				}
				else
				{
					strcpy(bpm_response, "");
					for (retry = 0; retry < 3; retry++)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						for (i = 0; i < 0.25; i++)
						{
							{
								epicsThreadSleep(1 / 60.);
							}
							;
							if (seq_efTest(ssId, s_ainp_mon))
								break;
						}
						if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
						{
							sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
							if (debug_flag >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 415, SNLtaskName, 10);
								;
								printf("%s\n", debug_str);
								epicsThreadSleep(0.01);
							}
							;
							strcpy(bpm_response, s_ainp + 1);
							break;
						}
					}
				}
			}
# line 415 "../sncqxbpm.st"
			;
# line 420 "../sncqxbpm.st"
			updateDelay = 3;
			updateDelay = 0.01;
		}
		return;
	case 4:
		{
# line 427 "../sncqxbpm.st"
			if (buflen < buflen_lo)
			{
				buflen = (buflen_lo);
				seq_pvPut(ssId, 40 /* buflen */, 2);
			}
# line 427 "../sncqxbpm.st"
			;
			if (buflen > buflen_hi)
			{
				buflen = (buflen_hi);
				seq_pvPut(ssId, 40 /* buflen */, 2);
			}
# line 428 "../sncqxbpm.st"
			;
			{
				epicsThreadSleep(2 / 60.);
			}
# line 429 "../sncqxbpm.st"
			;
			seq_efClear(ssId, buflen_mon);
# line 436 "../sncqxbpm.st"
			if (signal_mode == 0)
			{
				sprintf(debug_str, "gain changed to %s (%d)", signal_modeStr[signal_mode], signal_mode);
				if (debug_flag >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 434, SNLtaskName, 5);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				{
					sprintf(bpm_command, ":CONF%d:SINGLE", bpm_address, 0, 0, 0);
					sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command);
					if (debug_flag >= 15)
					{
						printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 435, SNLtaskName, 15);
						;
						printf("%s\n", debug_str);
						epicsThreadSleep(0.01);
					}
					;
					if (s_tmod != 1)
					{
						s_tmod = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 2);
					}
					;
					{
						epicsThreadSleep(1 / 60.);
					}
					;
					if (1 == 1)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
					}
					else
					{
						strcpy(bpm_response, "");
						for (retry = 0; retry < 3; retry++)
						{
							if (strcmp(s_aout, bpm_command))
							{
								{
									strcpy(s_aout, bpm_command);
									seq_pvPut(ssId, 1 /* s_aout */, 2);
									epicsThreadSleep(0.01);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							else
							{
								{
									s_proc = (1);
									seq_pvPut(ssId, 11 /* s_proc */, 2);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							for (i = 0; i < 0.25; i++)
							{
								{
									epicsThreadSleep(1 / 60.);
								}
								;
								if (seq_efTest(ssId, s_ainp_mon))
									break;
							}
							if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
							{
								sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
								if (debug_flag >= 10)
								{
									printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 435, SNLtaskName, 10);
									;
									printf("%s\n", debug_str);
									epicsThreadSleep(0.01);
								}
								;
								strcpy(bpm_response, s_ainp + 1);
								break;
							}
						}
					}
				}
				;
			}
# line 442 "../sncqxbpm.st"
			if (signal_mode == 1)
			{
				sprintf(debug_str, "gain changed to %s (%d) with buffer length = %d", signal_modeStr[signal_mode], signal_mode, buflen);
				if (debug_flag >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 440, SNLtaskName, 5);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				{
					sprintf(bpm_command, ":CONF%d:AVGCURR %d", bpm_address, buflen, 0, 0);
					sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command);
					if (debug_flag >= 15)
					{
						printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 441, SNLtaskName, 15);
						;
						printf("%s\n", debug_str);
						epicsThreadSleep(0.01);
					}
					;
					if (s_tmod != 1)
					{
						s_tmod = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 2);
					}
					;
					{
						epicsThreadSleep(1 / 60.);
					}
					;
					if (1 == 1)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
					}
					else
					{
						strcpy(bpm_response, "");
						for (retry = 0; retry < 3; retry++)
						{
							if (strcmp(s_aout, bpm_command))
							{
								{
									strcpy(s_aout, bpm_command);
									seq_pvPut(ssId, 1 /* s_aout */, 2);
									epicsThreadSleep(0.01);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							else
							{
								{
									s_proc = (1);
									seq_pvPut(ssId, 11 /* s_proc */, 2);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							for (i = 0; i < 0.25; i++)
							{
								{
									epicsThreadSleep(1 / 60.);
								}
								;
								if (seq_efTest(ssId, s_ainp_mon))
									break;
							}
							if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
							{
								sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
								if (debug_flag >= 10)
								{
									printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 441, SNLtaskName, 10);
									;
									printf("%s\n", debug_str);
									epicsThreadSleep(0.01);
								}
								;
								strcpy(bpm_response, s_ainp + 1);
								break;
							}
						}
					}
				}
				;
			}
# line 448 "../sncqxbpm.st"
			if (signal_mode == 2)
			{
				sprintf(debug_str, "gain changed to %s (%d) with buffer length = %d", signal_modeStr[signal_mode], signal_mode, buflen);
				if (debug_flag >= 5)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 446, SNLtaskName, 5);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				{
					sprintf(bpm_command, ":CONF%d:WDWCURR %d", bpm_address, buflen, 0, 0);
					sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command);
					if (debug_flag >= 15)
					{
						printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 447, SNLtaskName, 15);
						;
						printf("%s\n", debug_str);
						epicsThreadSleep(0.01);
					}
					;
					if (s_tmod != 1)
					{
						s_tmod = (1);
						seq_pvPut(ssId, 16 /* s_tmod */, 2);
					}
					;
					{
						epicsThreadSleep(1 / 60.);
					}
					;
					if (1 == 1)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
					}
					else
					{
						strcpy(bpm_response, "");
						for (retry = 0; retry < 3; retry++)
						{
							if (strcmp(s_aout, bpm_command))
							{
								{
									strcpy(s_aout, bpm_command);
									seq_pvPut(ssId, 1 /* s_aout */, 2);
									epicsThreadSleep(0.01);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							else
							{
								{
									s_proc = (1);
									seq_pvPut(ssId, 11 /* s_proc */, 2);
								}
								;
								{
									epicsThreadSleep(1 / 60.);
								}
								;
							}
							for (i = 0; i < 0.25; i++)
							{
								{
									epicsThreadSleep(1 / 60.);
								}
								;
								if (seq_efTest(ssId, s_ainp_mon))
									break;
							}
							if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
							{
								sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
								if (debug_flag >= 10)
								{
									printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 447, SNLtaskName, 10);
									;
									printf("%s\n", debug_str);
									epicsThreadSleep(0.01);
								}
								;
								strcpy(bpm_response, s_ainp + 1);
								break;
							}
						}
					}
				}
				;
			}
		}
		return;
	case 5:
		{
# line 452 "../sncqxbpm.st"
			if (period_s < period_s_LOPR)
			{
				period_s = (period_s_LOPR);
				seq_pvPut(ssId, 44 /* period_s */, 2);
			}
# line 452 "../sncqxbpm.st"
			;
			if (period_s > period_s_HOPR)
			{
				period_s = (period_s_HOPR);
				seq_pvPut(ssId, 44 /* period_s */, 2);
			}
# line 453 "../sncqxbpm.st"
			;
			sprintf(debug_str, "checking update period: %g s", period_s);
			if (debug_flag >= 5)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 455, SNLtaskName, 5);
				;
				printf("%s\n", debug_str);
				epicsThreadSleep(0.01);
			}
# line 455 "../sncqxbpm.st"
			;
		}
		return;
	case 6:
		{
# line 460 "../sncqxbpm.st"
			{
				sprintf(bpm_command, ":READ%d:CURRALL?", bpm_address, 0, 0, 0);
				sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 0, bpm_command);
				if (debug_flag >= 15)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 460, SNLtaskName, 15);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				if (s_tmod != 0)
				{
					s_tmod = (0);
					seq_pvPut(ssId, 16 /* s_tmod */, 2);
				}
				;
				{
					epicsThreadSleep(1 / 60.);
				}
				;
				if (0 == 1)
				{
					if (strcmp(s_aout, bpm_command))
					{
						{
							strcpy(s_aout, bpm_command);
							seq_pvPut(ssId, 1 /* s_aout */, 2);
							epicsThreadSleep(0.01);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
					else
					{
						{
							s_proc = (1);
							seq_pvPut(ssId, 11 /* s_proc */, 2);
						}
						;
						{
							epicsThreadSleep(1 / 60.);
						}
						;
					}
				}
				else
				{
					strcpy(bpm_response, "");
					for (retry = 0; retry < 3; retry++)
					{
						if (strcmp(s_aout, bpm_command))
						{
							{
								strcpy(s_aout, bpm_command);
								seq_pvPut(ssId, 1 /* s_aout */, 2);
								epicsThreadSleep(0.01);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						else
						{
							{
								s_proc = (1);
								seq_pvPut(ssId, 11 /* s_proc */, 2);
							}
							;
							{
								epicsThreadSleep(1 / 60.);
							}
							;
						}
						for (i = 0; i < 0.25; i++)
						{
							{
								epicsThreadSleep(1 / 60.);
							}
							;
							if (seq_efTest(ssId, s_ainp_mon))
								break;
						}
						if (!s_sevr && seq_efTest(ssId, s_ainp_mon))
						{
							sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout, s_ainp);
							if (debug_flag >= 10)
							{
								printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 460, SNLtaskName, 10);
								;
								printf("%s\n", debug_str);
								epicsThreadSleep(0.01);
							}
							;
							strcpy(bpm_response, s_ainp + 1);
							break;
						}
					}
				}
			}
# line 460 "../sncqxbpm.st"
			;
			sprintf(debug_str, "serial.SEVR: %d", s_sevr);
			if (debug_flag >= 5)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 462, SNLtaskName, 5);
				;
				printf("%s\n", debug_str);
				epicsThreadSleep(0.01);
			}
# line 462 "../sncqxbpm.st"
			;
# line 503 "../sncqxbpm.st"
			if (!s_sevr)
			{
				sscanf(bpm_response, "%lu%lu%lu%lu",  & raw_a,  & raw_b,  & raw_c,  & raw_d);
				seq_pvPut(ssId, 32 /* raw_a */, 0);
				seq_pvPut(ssId, 33 /* raw_b */, 0);
				seq_pvPut(ssId, 34 /* raw_c */, 0);
				seq_pvPut(ssId, 35 /* raw_d */, 0);
				sprintf(debug_str, "bpm_command=<%s>  bpm_response=<%s>", bpm_command, bpm_response);
				if (debug_flag >= 12)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 471, SNLtaskName, 12);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				sprintf(debug_str, "raw: a=%ld  b=%ld  c=%ld  d=%ld", raw_a, raw_b, raw_c, raw_d);
				if (debug_flag >= 12)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 474, SNLtaskName, 12);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				{
					current_a = (long_internal = raw_a - offset[((gain) * 4 + (1) - 1)], gainTrim[((gain) * 4 + (1) - 1)] * (long_internal));
					seq_pvPut(ssId, 28 /* current_a */, 2);
				}
				;
				{
					current_b = (long_internal = raw_b - offset[((gain) * 4 + (2) - 1)], gainTrim[((gain) * 4 + (2) - 1)] * (long_internal));
					seq_pvPut(ssId, 29 /* current_b */, 2);
				}
				;
				{
					current_c = (long_internal = raw_c - offset[((gain) * 4 + (3) - 1)], gainTrim[((gain) * 4 + (3) - 1)] * (long_internal));
					seq_pvPut(ssId, 30 /* current_c */, 2);
				}
				;
				{
					current_d = (long_internal = raw_d - offset[((gain) * 4 + (4) - 1)], gainTrim[((gain) * 4 + (4) - 1)] * (long_internal));
					seq_pvPut(ssId, 31 /* current_d */, 2);
				}
				;
				{
					current_total = (current_a + current_b + current_c + current_d);
					seq_pvPut(ssId, 27 /* current_total */, 2);
				}
				;
				{
					pos_x = (gx * (current_b - current_d) / (current_b + current_d));
					seq_pvPut(ssId, 25 /* pos_x */, 2);
				}
				;
				{
					pos_y = (gy * (current_a - current_c) / (current_a + current_c));
					seq_pvPut(ssId, 26 /* pos_y */, 2);
				}
				;
				{
					current_low = ((raw_a < current_low_raw) && (raw_b < current_low_raw) && (raw_c < current_low_raw) && (raw_d < current_low_raw));
					seq_pvPut(ssId, 37 /* current_low */, 2);
				}
				;
				{
					current_ok = ((raw_a >= current_low_raw) && (raw_b >= current_low_raw) && (raw_c >= current_low_raw) && (raw_d >= current_low_raw));
					seq_pvPut(ssId, 38 /* current_ok */, 2);
				}
				;
			}
		}
		return;
	}
}
/* Code for state "startup" in state set "sncqxbpm_tools" */

/* Delay function for state "startup" in state set "sncqxbpm_tools" */
static void D_sncqxbpm_tools_startup(SS_ID ssId, struct UserVar *pVar)
{
# line 518 "../sncqxbpm.st"
}

/* Event function for state "startup" in state set "sncqxbpm_tools" */
static long E_sncqxbpm_tools_startup(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 518 "../sncqxbpm.st"
	if (seq_efTest(ssId, started))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "startup" in state set "sncqxbpm_tools" */
static void A_sncqxbpm_tools_startup(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "idle" in state set "sncqxbpm_tools" */

/* Delay function for state "idle" in state set "sncqxbpm_tools" */
static void D_sncqxbpm_tools_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 550 "../sncqxbpm.st"
# line 576 "../sncqxbpm.st"
}

/* Event function for state "idle" in state set "sncqxbpm_tools" */
static long E_sncqxbpm_tools_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 550 "../sncqxbpm.st"
	if (set_offsets)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 576 "../sncqxbpm.st"
	if (set_defaults)
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "sncqxbpm_tools" */
static void A_sncqxbpm_tools_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 524 "../sncqxbpm.st"
			oldGain = gain;
			{
				epicsThreadSleep(60 / 60.);
			}
# line 525 "../sncqxbpm.st"
			;
# line 544 "../sncqxbpm.st"
			for (newGain = 0; newGain < 6; newGain++)
			{
				if (!set_offsets)
					break;
				{
					gain = (newGain);
					seq_pvPut(ssId, 43 /* gain */, 2);
				}
				;
				if (!set_offsets)
					break;
				epicsThreadSleep(settling);
				if (!set_offsets)
					break;
				sprintf(debug_str, "new gain=%d", gain);
				if (debug_flag >= 18)
				{
					printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 533, SNLtaskName, 18);
					;
					printf("%s\n", debug_str);
					epicsThreadSleep(0.01);
				}
				;
				{
					offset[((gain) * 4 + (1) - 1)] = (raw_a);
					seq_pvPut(ssId, 85 /* offset */ + (((gain) * 4 + (1) - 1)), 2);
				}
				;
				{
					offset[((gain) * 4 + (2) - 1)] = (raw_b);
					seq_pvPut(ssId, 85 /* offset */ + (((gain) * 4 + (2) - 1)), 2);
				}
				;
				{
					offset[((gain) * 4 + (3) - 1)] = (raw_c);
					seq_pvPut(ssId, 85 /* offset */ + (((gain) * 4 + (3) - 1)), 2);
				}
				;
				{
					offset[((gain) * 4 + (4) - 1)] = (raw_d);
					seq_pvPut(ssId, 85 /* offset */ + (((gain) * 4 + (4) - 1)), 2);
				}
				;
				for (channel = 1; channel <= 4; channel++)
				{
					sprintf(debug_str, "  offset[%d]=%ld", channel, offset[((gain) * 4 + (channel) - 1)]);
					if (debug_flag >= 19)
					{
						printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 542, SNLtaskName, 19);
						;
						printf("%s\n", debug_str);
						epicsThreadSleep(0.01);
					}
					;
				}
			}
# line 545 "../sncqxbpm.st"
			{
				gain = (oldGain);
				seq_pvPut(ssId, 43 /* gain */, 2);
			}
# line 545 "../sncqxbpm.st"
			;
			{
				set_offsets = (0);
				seq_pvPut(ssId, 49 /* set_offsets */, 2);
			}
# line 546 "../sncqxbpm.st"
			;
			{
				epicsThreadSleep(2 / 60.);
			}
# line 547 "../sncqxbpm.st"
			;
			sprintf(debug_str, "restored to previous gain #%d", gain);
			if (debug_flag >= 18)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 549, SNLtaskName, 18);
				;
				printf("%s\n", debug_str);
				epicsThreadSleep(0.01);
			}
# line 549 "../sncqxbpm.st"
			;
		}
		return;
	case 1:
		{
# line 553 "../sncqxbpm.st"
			if (debug_flag >= 18)
			{
				printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 553, SNLtaskName, 18);
				;
				printf("%s\n", "restore default calibrations");
				epicsThreadSleep(0.01);
			}
# line 553 "../sncqxbpm.st"
			;
			{
				signal_mode = (2);
				seq_pvPut(ssId, 39 /* signal_mode */, 2);
			}
# line 554 "../sncqxbpm.st"
			;
			{
				buflen = (30);
				seq_pvPut(ssId, 40 /* buflen */, 2);
			}
# line 555 "../sncqxbpm.st"
			;
			{
				current_low_raw = (1000);
				seq_pvPut(ssId, 36 /* current_low_raw */, 2);
			}
# line 556 "../sncqxbpm.st"
			;
			{
				period_s = (0.1);
				seq_pvPut(ssId, 44 /* period_s */, 2);
			}
# line 557 "../sncqxbpm.st"
			;
			{
				gx = (4.5);
				seq_pvPut(ssId, 47 /* gx */, 2);
			}
# line 558 "../sncqxbpm.st"
			;
			{
				gy = (4.5);
				seq_pvPut(ssId, 48 /* gy */, 2);
			}
# line 559 "../sncqxbpm.st"
			;
			{
				settling = (2.5);
				seq_pvPut(ssId, 51 /* settling */, 2);
			}
# line 560 "../sncqxbpm.st"
			;
# line 573 "../sncqxbpm.st"
			for (newGain = 0; newGain < 6; newGain++)
			{
				if (newGain == 0)
				{
					factor = 350e-9;
				}
				if (newGain == 1)
				{
					factor = 700e-9;
				}
				if (newGain == 2)
				{
					factor = 1400e-9;
				}
				if (newGain == 3)
				{
					factor = 7e-6;
				}
				if (newGain == 4)
				{
					factor = 70e-6;
				}
				if (newGain == 5)
				{
					factor = 700e-6;
				}
				factor /= 10;
				factor /= 1e5;
				for (channel = 1; channel <= 4; channel++)
				{
					{
						gainTrim[((newGain) * 4 + (channel) - 1)] = (factor);
						seq_pvPut(ssId, 61 /* gainTrim */ + (((newGain) * 4 + (channel) - 1)), 2);
					}
					;
				}
			}
# line 574 "../sncqxbpm.st"
			{
				set_defaults = (0);
				seq_pvPut(ssId, 50 /* set_defaults */, 2);
			}
# line 574 "../sncqxbpm.st"
			;
			{
				epicsThreadSleep(2 / 60.);
			}
# line 575 "../sncqxbpm.st"
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
  {"{S}.AINP", (void *)&s_ainp[0], "s_ainp", 
    "string", 1, 7, 1, 1, 0, 0, 0},

  {"{S}.AOUT", (void *)&s_aout[0], "s_aout", 
    "string", 1, 8, 0, 0, 0, 0, 0},

  {"{S}.BAUD", (void *)&s_baud, "s_baud", 
    "short", 1, 9, 0, 0, 0, 0, 0},

  {"{S}.DBIT", (void *)&s_dbit, "s_dbit", 
    "short", 1, 10, 0, 0, 0, 0, 0},

  {"{S}.FCTL", (void *)&s_fctl, "s_fctl", 
    "short", 1, 11, 0, 0, 0, 0, 0},

  {"{S}.IEOS", (void *)&s_idel[0], "s_idel", 
    "string", 1, 12, 0, 0, 0, 0, 0},

  {"{S}.IFMT", (void *)&s_ifmt, "s_ifmt", 
    "short", 1, 13, 0, 0, 0, 0, 0},

  {"{S}.NORD", (void *)&s_nord, "s_nord", 
    "short", 1, 14, 0, 1, 0, 0, 0},

  {"{S}.NRRD", (void *)&s_nrrd, "s_nrrd", 
    "short", 1, 15, 0, 0, 0, 0, 0},

  {"{S}.OEOS", (void *)&s_odel[0], "s_odel", 
    "string", 1, 16, 0, 0, 0, 0, 0},

  {"{S}.OFMT", (void *)&s_ofmt, "s_ofmt", 
    "short", 1, 17, 0, 0, 0, 0, 0},

  {"{S}.PROC", (void *)&s_proc, "s_proc", 
    "short", 1, 18, 0, 1, 0, 0, 0},

  {"{S}.PRTY", (void *)&s_prty, "s_prty", 
    "short", 1, 19, 0, 0, 0, 0, 0},

  {"{S}.SBIT", (void *)&s_sbit, "s_sbit", 
    "short", 1, 20, 0, 0, 0, 0, 0},

  {"{S}.SCAN", (void *)&s_scan, "s_scan", 
    "short", 1, 21, 0, 0, 0, 0, 0},

  {"{S}.SEVR", (void *)&s_sevr, "s_sevr", 
    "int", 1, 22, 0, 1, 0, 0, 0},

  {"{S}.TMOD", (void *)&s_tmod, "s_tmod", 
    "short", 1, 23, 0, 0, 0, 0, 0},

  {"{S}.TMOT", (void *)&s_tmot, "s_tmot", 
    "double", 1, 24, 0, 1, 0, 0, 0},

  {"{P}port", (void *)&port[0], "port", 
    "string", 1, 25, 0, 0, 0, 0, 0},

  {"{P}address", (void *)&bpm_address, "bpm_address", 
    "int", 1, 26, 0, 1, 0, 0, 0},

  {"{P}init", (void *)&init, "init", 
    "short", 1, 27, 0, 1, 0, 0, 0},

  {"{P}enable", (void *)&enable, "enable", 
    "short", 1, 28, 0, 1, 0, 0, 0},

  {"{P}error", (void *)&error, "error", 
    "int", 1, 29, 0, 0, 0, 0, 0},

  {"{P}errMsg", (void *)&errMsg[0], "errMsg", 
    "string", 1, 30, 0, 0, 0, 0, 0},

  {"{P}debug", (void *)&debug_flag, "debug_flag", 
    "int", 1, 31, 0, 1, 0, 0, 0},

  {"{P}pos:x", (void *)&pos_x, "pos_x", 
    "double", 1, 32, 0, 0, 0, 0, 0},

  {"{P}pos:y", (void *)&pos_y, "pos_y", 
    "double", 1, 33, 0, 0, 0, 0, 0},

  {"{P}current:total", (void *)&current_total, "current_total", 
    "double", 1, 34, 0, 0, 0, 0, 0},

  {"{P}current:a", (void *)&current_a, "current_a", 
    "double", 1, 35, 0, 0, 0, 0, 0},

  {"{P}current:b", (void *)&current_b, "current_b", 
    "double", 1, 36, 0, 0, 0, 0, 0},

  {"{P}current:c", (void *)&current_c, "current_c", 
    "double", 1, 37, 0, 0, 0, 0, 0},

  {"{P}current:d", (void *)&current_d, "current_d", 
    "double", 1, 38, 0, 0, 0, 0, 0},

  {"{P}current:a:raw", (void *)&raw_a, "raw_a", 
    "unsigned long", 1, 39, 0, 0, 0, 0, 0},

  {"{P}current:b:raw", (void *)&raw_b, "raw_b", 
    "unsigned long", 1, 40, 0, 0, 0, 0, 0},

  {"{P}current:c:raw", (void *)&raw_c, "raw_c", 
    "unsigned long", 1, 41, 0, 0, 0, 0, 0},

  {"{P}current:d:raw", (void *)&raw_d, "raw_d", 
    "unsigned long", 1, 42, 0, 0, 0, 0, 0},

  {"{P}current:low:raw", (void *)&current_low_raw, "current_low_raw", 
    "long", 1, 43, 0, 1, 0, 0, 0},

  {"{P}current:low", (void *)&current_low, "current_low", 
    "short", 1, 44, 0, 0, 0, 0, 0},

  {"{P}current:ok", (void *)&current_ok, "current_ok", 
    "short", 1, 45, 0, 0, 0, 0, 0},

  {"{P}mode", (void *)&signal_mode, "signal_mode", 
    "short", 1, 46, 2, 1, 0, 0, 0},

  {"{P}buflen", (void *)&buflen, "buflen", 
    "short", 1, 47, 3, 1, 0, 0, 0},

  {"{P}buflen.LOPR", (void *)&buflen_lo, "buflen_lo", 
    "short", 1, 48, 0, 1, 0, 0, 0},

  {"{P}buflen.HOPR", (void *)&buflen_hi, "buflen_hi", 
    "short", 1, 49, 0, 1, 0, 0, 0},

  {"{P}gain", (void *)&gain, "gain", 
    "int", 1, 50, 4, 1, 0, 0, 0},

  {"{P}period", (void *)&period_s, "period_s", 
    "double", 1, 51, 5, 1, 0, 0, 0},

  {"{P}period.LOPR", (void *)&period_s_LOPR, "period_s_LOPR", 
    "double", 1, 52, 0, 1, 0, 0, 0},

  {"{P}period.HOPR", (void *)&period_s_HOPR, "period_s_HOPR", 
    "double", 1, 53, 0, 1, 0, 0, 0},

  {"{P}GX", (void *)&gx, "gx", 
    "double", 1, 54, 0, 1, 0, 0, 0},

  {"{P}GY", (void *)&gy, "gy", 
    "double", 1, 55, 0, 1, 0, 0, 0},

  {"{P}set_offsets", (void *)&set_offsets, "set_offsets", 
    "short", 1, 56, 0, 1, 0, 0, 0},

  {"{P}set_defaults", (void *)&set_defaults, "set_defaults", 
    "short", 1, 57, 0, 1, 0, 0, 0},

  {"{P}settling", (void *)&settling, "settling", 
    "double", 1, 58, 0, 1, 0, 0, 0},

  {"{P}gain.ZRST", (void *)&gainStr[0][0], "gainStr[0]", 
    "string", 1, 59, 0, 1, 0, 0, 0},

  {"{P}gain.ONST", (void *)&gainStr[1][0], "gainStr[1]", 
    "string", 1, 60, 0, 1, 0, 0, 0},

  {"{P}gain.TWST", (void *)&gainStr[2][0], "gainStr[2]", 
    "string", 1, 61, 0, 1, 0, 0, 0},

  {"{P}gain.THST", (void *)&gainStr[3][0], "gainStr[3]", 
    "string", 1, 62, 0, 1, 0, 0, 0},

  {"{P}gain.FRST", (void *)&gainStr[4][0], "gainStr[4]", 
    "string", 1, 63, 0, 1, 0, 0, 0},

  {"{P}gain.FVST", (void *)&gainStr[5][0], "gainStr[5]", 
    "string", 1, 64, 0, 1, 0, 0, 0},

  {"{P}mode.ZRST", (void *)&signal_modeStr[0][0], "signal_modeStr[0]", 
    "string", 1, 65, 0, 1, 0, 0, 0},

  {"{P}mode.ONST", (void *)&signal_modeStr[1][0], "signal_modeStr[1]", 
    "string", 1, 66, 0, 1, 0, 0, 0},

  {"{P}mode.TWST", (void *)&signal_modeStr[2][0], "signal_modeStr[2]", 
    "string", 1, 67, 0, 1, 0, 0, 0},

  {"{P}r1:A1", (void *)&gainTrim[0], "gainTrim[0]", 
    "double", 1, 68, 0, 1, 0, 0, 0},

  {"{P}r1:B1", (void *)&gainTrim[1], "gainTrim[1]", 
    "double", 1, 69, 0, 1, 0, 0, 0},

  {"{P}r1:C1", (void *)&gainTrim[2], "gainTrim[2]", 
    "double", 1, 70, 0, 1, 0, 0, 0},

  {"{P}r1:D1", (void *)&gainTrim[3], "gainTrim[3]", 
    "double", 1, 71, 0, 1, 0, 0, 0},

  {"{P}r2:A1", (void *)&gainTrim[4], "gainTrim[4]", 
    "double", 1, 72, 0, 1, 0, 0, 0},

  {"{P}r2:B1", (void *)&gainTrim[5], "gainTrim[5]", 
    "double", 1, 73, 0, 1, 0, 0, 0},

  {"{P}r2:C1", (void *)&gainTrim[6], "gainTrim[6]", 
    "double", 1, 74, 0, 1, 0, 0, 0},

  {"{P}r2:D1", (void *)&gainTrim[7], "gainTrim[7]", 
    "double", 1, 75, 0, 1, 0, 0, 0},

  {"{P}r3:A1", (void *)&gainTrim[8], "gainTrim[8]", 
    "double", 1, 76, 0, 1, 0, 0, 0},

  {"{P}r3:B1", (void *)&gainTrim[9], "gainTrim[9]", 
    "double", 1, 77, 0, 1, 0, 0, 0},

  {"{P}r3:C1", (void *)&gainTrim[10], "gainTrim[10]", 
    "double", 1, 78, 0, 1, 0, 0, 0},

  {"{P}r3:D1", (void *)&gainTrim[11], "gainTrim[11]", 
    "double", 1, 79, 0, 1, 0, 0, 0},

  {"{P}r4:A1", (void *)&gainTrim[12], "gainTrim[12]", 
    "double", 1, 80, 0, 1, 0, 0, 0},

  {"{P}r4:B1", (void *)&gainTrim[13], "gainTrim[13]", 
    "double", 1, 81, 0, 1, 0, 0, 0},

  {"{P}r4:C1", (void *)&gainTrim[14], "gainTrim[14]", 
    "double", 1, 82, 0, 1, 0, 0, 0},

  {"{P}r4:D1", (void *)&gainTrim[15], "gainTrim[15]", 
    "double", 1, 83, 0, 1, 0, 0, 0},

  {"{P}r5:A1", (void *)&gainTrim[16], "gainTrim[16]", 
    "double", 1, 84, 0, 1, 0, 0, 0},

  {"{P}r5:B1", (void *)&gainTrim[17], "gainTrim[17]", 
    "double", 1, 85, 0, 1, 0, 0, 0},

  {"{P}r5:C1", (void *)&gainTrim[18], "gainTrim[18]", 
    "double", 1, 86, 0, 1, 0, 0, 0},

  {"{P}r5:D1", (void *)&gainTrim[19], "gainTrim[19]", 
    "double", 1, 87, 0, 1, 0, 0, 0},

  {"{P}r6:A1", (void *)&gainTrim[20], "gainTrim[20]", 
    "double", 1, 88, 0, 1, 0, 0, 0},

  {"{P}r6:B1", (void *)&gainTrim[21], "gainTrim[21]", 
    "double", 1, 89, 0, 1, 0, 0, 0},

  {"{P}r6:C1", (void *)&gainTrim[22], "gainTrim[22]", 
    "double", 1, 90, 0, 1, 0, 0, 0},

  {"{P}r6:D1", (void *)&gainTrim[23], "gainTrim[23]", 
    "double", 1, 91, 0, 1, 0, 0, 0},

  {"{P}r1:A2", (void *)&offset[0], "offset[0]", 
    "long", 1, 92, 0, 1, 0, 0, 0},

  {"{P}r1:B2", (void *)&offset[1], "offset[1]", 
    "long", 1, 93, 0, 1, 0, 0, 0},

  {"{P}r1:C2", (void *)&offset[2], "offset[2]", 
    "long", 1, 94, 0, 1, 0, 0, 0},

  {"{P}r1:D2", (void *)&offset[3], "offset[3]", 
    "long", 1, 95, 0, 1, 0, 0, 0},

  {"{P}r2:A2", (void *)&offset[4], "offset[4]", 
    "long", 1, 96, 0, 1, 0, 0, 0},

  {"{P}r2:B2", (void *)&offset[5], "offset[5]", 
    "long", 1, 97, 0, 1, 0, 0, 0},

  {"{P}r2:C2", (void *)&offset[6], "offset[6]", 
    "long", 1, 98, 0, 1, 0, 0, 0},

  {"{P}r2:D2", (void *)&offset[7], "offset[7]", 
    "long", 1, 99, 0, 1, 0, 0, 0},

  {"{P}r3:A2", (void *)&offset[8], "offset[8]", 
    "long", 1, 100, 0, 1, 0, 0, 0},

  {"{P}r3:B2", (void *)&offset[9], "offset[9]", 
    "long", 1, 101, 0, 1, 0, 0, 0},

  {"{P}r3:C2", (void *)&offset[10], "offset[10]", 
    "long", 1, 102, 0, 1, 0, 0, 0},

  {"{P}r3:D2", (void *)&offset[11], "offset[11]", 
    "long", 1, 103, 0, 1, 0, 0, 0},

  {"{P}r4:A2", (void *)&offset[12], "offset[12]", 
    "long", 1, 104, 0, 1, 0, 0, 0},

  {"{P}r4:B2", (void *)&offset[13], "offset[13]", 
    "long", 1, 105, 0, 1, 0, 0, 0},

  {"{P}r4:C2", (void *)&offset[14], "offset[14]", 
    "long", 1, 106, 0, 1, 0, 0, 0},

  {"{P}r4:D2", (void *)&offset[15], "offset[15]", 
    "long", 1, 107, 0, 1, 0, 0, 0},

  {"{P}r5:A2", (void *)&offset[16], "offset[16]", 
    "long", 1, 108, 0, 1, 0, 0, 0},

  {"{P}r5:B2", (void *)&offset[17], "offset[17]", 
    "long", 1, 109, 0, 1, 0, 0, 0},

  {"{P}r5:C2", (void *)&offset[18], "offset[18]", 
    "long", 1, 110, 0, 1, 0, 0, 0},

  {"{P}r5:D2", (void *)&offset[19], "offset[19]", 
    "long", 1, 111, 0, 1, 0, 0, 0},

  {"{P}r6:A2", (void *)&offset[20], "offset[20]", 
    "long", 1, 112, 0, 1, 0, 0, 0},

  {"{P}r6:B2", (void *)&offset[21], "offset[21]", 
    "long", 1, 113, 0, 1, 0, 0, 0},

  {"{P}r6:C2", (void *)&offset[22], "offset[22]", 
    "long", 1, 114, 0, 1, 0, 0, 0},

  {"{P}r6:D2", (void *)&offset[23], "offset[23]", 
    "long", 1, 115, 0, 1, 0, 0, 0},

};

/* Event masks for state set sncqxbpm */
	/* Event mask for state startup: */
static bitMask	EM_sncqxbpm_startup[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state init: */
static bitMask	EM_sncqxbpm_init[] = {
	0x18000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state disable: */
static bitMask	EM_sncqxbpm_disable[] = {
	0x10000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state comm_error: */
static bitMask	EM_sncqxbpm_comm_error[] = {
	0x08400000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state idle: */
static bitMask	EM_sncqxbpm_idle[] = {
	0x1840003c,
	0x00080000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_sncqxbpm[] = {
	/* State "startup" */ {
	/* state name */       "startup",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_startup,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_startup,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_startup,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_startup,
	/* state options */   (0)},

	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_init,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_init,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_init,
	/* state options */   (0)},

	/* State "disable" */ {
	/* state name */       "disable",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_disable,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_disable,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_disable,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_disable,
	/* state options */   (0)},

	/* State "comm_error" */ {
	/* state name */       "comm_error",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_comm_error,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_comm_error,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_comm_error,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_comm_error,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_idle,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_idle,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_idle,
	/* state options */   (0)},


};

/* Event masks for state set sncqxbpm_tools */
	/* Event mask for state startup: */
static bitMask	EM_sncqxbpm_tools_startup[] = {
	0x00000040,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state idle: */
static bitMask	EM_sncqxbpm_tools_idle[] = {
	0x00000000,
	0x03000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_sncqxbpm_tools[] = {
	/* State "startup" */ {
	/* state name */       "startup",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_tools_startup,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_tools_startup,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_tools_startup,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_tools_startup,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_sncqxbpm_tools_idle,
	/* event function */  (EVENT_FUNC) E_sncqxbpm_tools_idle,
	/* delay function */   (DELAY_FUNC) D_sncqxbpm_tools_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_sncqxbpm_tools_idle,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "sncqxbpm" */ {
	/* ss name */            "sncqxbpm",
	/* ptr to state block */ state_sncqxbpm,
	/* number of states */   5,
	/* error state */        -1},


	/* State set "sncqxbpm_tools" */ {
	/* ss name */            "sncqxbpm_tools",
	/* ptr to state block */ state_sncqxbpm_tools,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "name=qxbpm, P=iad:xbpm:, S=lax:serialOI:a4";

/* State Program table (global) */
struct seqProgram sncqxbpm = {
	/* magic number */       20000315,
	/* *name */              "sncqxbpm",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ 0,
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_DEBUG | OPT_NEWEF),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void sncqxbpmRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&sncqxbpm);
}
epicsExportRegistrar(sncqxbpmRegistrar);

