/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: Keithley2kDMM.i */

/* Event flags */
#define ch_mode_mon	1
#define ch_mode_sel_mon	2
#define dmm_chan_mon	3
#define dmm_chan2_mon	4
#define read_complete_mon	5
#define init_dmm_mon	6

/* Program "Keithley2kDMM" */
#include "seqCom.h"

#define NUM_SS 2
#define NUM_CHANNELS 98
#define NUM_EVENTS 6
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram Keithley2kDMM;

/* Variable declarations */
struct UserVar {
	int	i;
	int	scan_chan;
	long	previous_mode;
	long	previous_chan;
	long	ch_mode[22];
	long	ch_on_off[22];
	long	ch_raw[22];
	long	dmm_modes[15];
	long	ch_mode_sel;
	long	done_read;
	long	dmm_chan;
	long	dmm_chan2;
	long	dmm_ch_range;
	long	dmm_delay_read;
	long	dmm_read;
	long	Dmm_raw;
	long	read_complete;
	long	single_multi;
	long	init_dmm;
	long	do_init;
	char	ch_close[MAX_STRING_SIZE];
	char	model[MAX_STRING_SIZE];
	long	channels;
	char	dmm_units[MAX_STRING_SIZE];
	char	ch_units[MAX_STRING_SIZE];
	char	*unit_strings[15];
	char	*P;
	char	*Dmm;
	char	pvname[MAX_STRING_SIZE];
	char	close_format[MAX_STRING_SIZE];
	long	mode;
	long	chan;
};

/* C code definitions */
# line 43 "../Keithley2kDMM.st"
 #include "stdlib.h"
# line 44 "../Keithley2kDMM.st"
 #include "string.h"

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "mode_change" */

/* Delay function for state "init" in state set "mode_change" */
static void D_mode_change_init(SS_ID ssId, struct UserVar *pVar)
{
# line 253 "../Keithley2kDMM.st"
}

/* Event function for state "init" in state set "mode_change" */
static long E_mode_change_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 253 "../Keithley2kDMM.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "mode_change" */
static void A_mode_change_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 223 "../Keithley2kDMM.st"
			seq_efSet(ssId, ch_mode_mon);
			seq_efSet(ssId, ch_mode_sel_mon);
			seq_efSet(ssId, dmm_chan_mon);
			(pVar->unit_strings)[0] = "VDC";
			(pVar->unit_strings)[1] = "VAC";
			(pVar->unit_strings)[2] = "OHM";
			(pVar->unit_strings)[3] = "OHM";
			(pVar->unit_strings)[4] = "HZ";
			(pVar->unit_strings)[5] = "DegC";
			(pVar->unit_strings)[6] = "DegC";
			(pVar->unit_strings)[7] = "DegC";
			(pVar->unit_strings)[8] = "DegC";
			(pVar->unit_strings)[9] = "DegC";
			(pVar->unit_strings)[10] = "DegC";
			(pVar->unit_strings)[11] = "DegC";
			(pVar->unit_strings)[12] = "DegC";
			(pVar->unit_strings)[13] = "ADC";
			(pVar->unit_strings)[14] = "AAC";
# line 242 "../Keithley2kDMM.st"
			(pVar->P) = seq_macValueGet(ssId, "P");
			(pVar->Dmm) = seq_macValueGet(ssId, "Dmm");
			strcpy((pVar->model), seq_macValueGet(ssId, "model"));
			seq_pvPut(ssId, 94 /* model */, 0);
			(pVar->channels) = atoi(seq_macValueGet(ssId, "channels"));
			seq_pvPut(ssId, 95 /* channels */, 0);
# line 250 "../Keithley2kDMM.st"
			if (strcmp((pVar->model), "2000") == 0)
			{
				strcpy((pVar->close_format), "rout:clos (@%d)");
			}
# line 252 "../Keithley2kDMM.st"
			else
			{
				strcpy((pVar->close_format), "rout:clos (@1%2.2d)");
			}
		}
		return;
	}
}
/* Code for state "monitor_mode_changes" in state set "mode_change" */

/* Delay function for state "monitor_mode_changes" in state set "mode_change" */
static void D_mode_change_monitor_mode_changes(SS_ID ssId, struct UserVar *pVar)
{
# line 276 "../Keithley2kDMM.st"
# line 289 "../Keithley2kDMM.st"
# line 295 "../Keithley2kDMM.st"
# line 301 "../Keithley2kDMM.st"
# line 312 "../Keithley2kDMM.st"
}

/* Event function for state "monitor_mode_changes" in state set "mode_change" */
static long E_mode_change_monitor_mode_changes(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 276 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, ch_mode_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 289 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, ch_mode_sel_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 295 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, dmm_chan_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 301 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, dmm_chan2_mon) && ((pVar->channels) > 10))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
# line 312 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, init_dmm_mon))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitor_mode_changes" in state set "mode_change" */
static void A_mode_change_monitor_mode_changes(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 275 "../Keithley2kDMM.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->channels); (pVar->i)++)
			{
				(pVar->mode) = (pVar->ch_mode)[(pVar->i)];
				if ((strcmp((pVar->model), "2700") == 0) && ((pVar->i) > 19))
				{
					(pVar->mode) = (pVar->mode) + 13;
				}
				sprintf((pVar->ch_units), "%s", (pVar->unit_strings)[(pVar->mode)]);
				sprintf((pVar->pvname), "%s%sch%d_units.VAL", (pVar->P), (pVar->Dmm), (pVar->i) + 1);
				seq_pvAssign(ssId, 97 /* ch_units */, (pVar->pvname));
				seq_pvPut(ssId, 97 /* ch_units */, 0);
			}
		}
		return;
	case 1:
		{
# line 279 "../Keithley2kDMM.st"
			sprintf((pVar->dmm_units), "%s", (pVar->unit_strings)[(pVar->ch_mode_sel)]);
			seq_pvPut(ssId, 96 /* dmm_units */, 0);
			seq_pvPut(ssId, 66 /* dmm_modes */ + ((pVar->ch_mode_sel)), 0);
			(pVar->previous_mode) = (pVar->ch_mode_sel);
# line 288 "../Keithley2kDMM.st"
			if ((pVar->ch_mode_sel) == 3)
			{
				sprintf((pVar->ch_close), "rout:open:all");
				seq_pvPut(ssId, 93 /* ch_close */, 0);
				(pVar->previous_chan) = -1;
			}
		}
		return;
	case 2:
		{
# line 292 "../Keithley2kDMM.st"
			sprintf((pVar->ch_close), (pVar->close_format), (pVar->dmm_chan) + 1);
			seq_pvPut(ssId, 93 /* ch_close */, 0);
			(pVar->previous_chan) = (pVar->dmm_chan);
		}
		return;
	case 3:
		{
# line 298 "../Keithley2kDMM.st"
			sprintf((pVar->ch_close), (pVar->close_format), (pVar->dmm_chan2) + 11);
			seq_pvPut(ssId, 93 /* ch_close */, 0);
			(pVar->previous_chan) = (pVar->dmm_chan2) + 10;
		}
		return;
	case 4:
		{
# line 311 "../Keithley2kDMM.st"
			if ((pVar->init_dmm) == 1)
			{
				(pVar->scan_chan) = 0;
				(pVar->previous_mode) = -1;
				(pVar->previous_chan) = -1;
				(pVar->init_dmm) = 0;
				seq_pvPut(ssId, 91 /* init_dmm */, 0);
				seq_pvPut(ssId, 92 /* do_init */, 0);
			}
		}
		return;
	}
}
/* Code for state "init" in state set "read_meter" */

/* Delay function for state "init" in state set "read_meter" */
static void D_read_meter_init(SS_ID ssId, struct UserVar *pVar)
{
# line 324 "../Keithley2kDMM.st"
}

/* Event function for state "init" in state set "read_meter" */
static long E_read_meter_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 324 "../Keithley2kDMM.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "read_meter" */
static void A_read_meter_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 319 "../Keithley2kDMM.st"
			(pVar->scan_chan) = 0;
			(pVar->previous_mode) = -1;
			(pVar->previous_chan) = -1;
			seq_efClear(ssId, read_complete_mon);
			seq_pvPut(ssId, 92 /* do_init */, 0);
		}
		return;
	}
}
/* Code for state "wait_read" in state set "read_meter" */

/* Delay function for state "wait_read" in state set "read_meter" */
static void D_read_meter_wait_read(SS_ID ssId, struct UserVar *pVar)
{
# line 328 "../Keithley2kDMM.st"
}

/* Event function for state "wait_read" in state set "read_meter" */
static long E_read_meter_wait_read(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 328 "../Keithley2kDMM.st"
	if ((pVar->done_read) == 1)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "wait_read" in state set "read_meter" */
static void A_read_meter_wait_read(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "read_channel" in state set "read_meter" */

/* Delay function for state "read_channel" in state set "read_meter" */
static void D_read_meter_read_channel(SS_ID ssId, struct UserVar *pVar)
{
# line 350 "../Keithley2kDMM.st"
# line 379 "../Keithley2kDMM.st"
# line 382 "../Keithley2kDMM.st"
}

/* Event function for state "read_channel" in state set "read_meter" */
static long E_read_meter_read_channel(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 350 "../Keithley2kDMM.st"
	if ((pVar->single_multi) == 0)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 379 "../Keithley2kDMM.st"
	if (((pVar->single_multi) != 0) && ((pVar->ch_on_off)[(pVar->scan_chan)] == 0))
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
# line 382 "../Keithley2kDMM.st"
	if (((pVar->single_multi) != 0) && ((pVar->ch_on_off)[(pVar->scan_chan)] != 0))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "read_channel" in state set "read_meter" */
static void A_read_meter_read_channel(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 336 "../Keithley2kDMM.st"
			if ((pVar->ch_mode_sel) != (pVar->previous_mode))
			{
				sprintf((pVar->dmm_units), "%s", (pVar->unit_strings)[(pVar->ch_mode_sel)]);
				seq_pvPut(ssId, 96 /* dmm_units */, 0);
				seq_pvPut(ssId, 66 /* dmm_modes */ + ((pVar->ch_mode_sel)), 0);
			}
# line 338 "../Keithley2kDMM.st"
			(pVar->chan) = (pVar->dmm_chan);
			if ((pVar->dmm_ch_range) == 1)
				(pVar->chan) = (pVar->dmm_chan2) + 10;
# line 344 "../Keithley2kDMM.st"
			if ((pVar->chan) != (pVar->previous_chan))
			{
				sprintf((pVar->ch_close), (pVar->close_format), (pVar->chan) + 1);
				seq_pvPut(ssId, 93 /* ch_close */, 0);
				(pVar->previous_chan) = (pVar->chan);
			}
# line 345 "../Keithley2kDMM.st"
			(pVar->previous_mode) = (pVar->ch_mode_sel);
			(pVar->read_complete) = 0;
			seq_pvPut(ssId, 89 /* read_complete */, 0);
			seq_efClear(ssId, read_complete_mon);
			seq_pvPut(ssId, 87 /* dmm_read */, 0);
		}
		return;
	case 1:
		{
# line 353 "../Keithley2kDMM.st"
			(pVar->mode) = (pVar->ch_mode)[(pVar->scan_chan)];
# line 359 "../Keithley2kDMM.st"
			if ((strcmp((pVar->model), "2700") == 0) && ((pVar->scan_chan) > 19))
			{
				(pVar->mode) = (pVar->mode) + 13;
			}
			if ((pVar->mode) != (pVar->previous_mode))
			{
				seq_pvPut(ssId, 66 /* dmm_modes */ + ((pVar->mode)), 0);
				(pVar->previous_mode) = (pVar->mode);
			}
			if ((pVar->scan_chan) != (pVar->previous_chan))
			{
				sprintf((pVar->ch_close), (pVar->close_format), (pVar->scan_chan) + 1);
				seq_pvPut(ssId, 93 /* ch_close */, 0);
				(pVar->previous_chan) = (pVar->scan_chan);
			}
# line 369 "../Keithley2kDMM.st"
			(pVar->read_complete) = 0;
			seq_pvPut(ssId, 89 /* read_complete */, 0);
			seq_efClear(ssId, read_complete_mon);
# line 376 "../Keithley2kDMM.st"
			if ((pVar->mode) == 1)
				seq_pvPut(ssId, 86 /* dmm_delay_read */, 0);
			else
				seq_pvPut(ssId, 87 /* dmm_read */, 0);
		}
		return;
	case 2:
		{
		}
		return;
	}
}
/* Code for state "single_read_response" in state set "read_meter" */

/* Delay function for state "single_read_response" in state set "read_meter" */
static void D_read_meter_single_read_response(SS_ID ssId, struct UserVar *pVar)
{
# line 389 "../Keithley2kDMM.st"
}

/* Event function for state "single_read_response" in state set "read_meter" */
static long E_read_meter_single_read_response(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 389 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, read_complete_mon) && (pVar->read_complete))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "single_read_response" in state set "read_meter" */
static void A_read_meter_single_read_response(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 386 "../Keithley2kDMM.st"
			seq_pvPut(ssId, 88 /* Dmm_raw */, 0);
			(pVar->done_read) = 0;
			seq_pvPut(ssId, 82 /* done_read */, 0);
		}
		return;
	}
}
/* Code for state "multi_read_response" in state set "read_meter" */

/* Delay function for state "multi_read_response" in state set "read_meter" */
static void D_read_meter_multi_read_response(SS_ID ssId, struct UserVar *pVar)
{
# line 394 "../Keithley2kDMM.st"
}

/* Event function for state "multi_read_response" in state set "read_meter" */
static long E_read_meter_multi_read_response(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 394 "../Keithley2kDMM.st"
	if (seq_efTestAndClear(ssId, read_complete_mon) && (pVar->read_complete))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "multi_read_response" in state set "read_meter" */
static void A_read_meter_multi_read_response(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 393 "../Keithley2kDMM.st"
			seq_pvPut(ssId, 44 /* ch_raw */ + ((pVar->scan_chan)), 0);
		}
		return;
	}
}
/* Code for state "next_channel" in state set "read_meter" */

/* Delay function for state "next_channel" in state set "read_meter" */
static void D_read_meter_next_channel(SS_ID ssId, struct UserVar *pVar)
{
# line 399 "../Keithley2kDMM.st"
# line 404 "../Keithley2kDMM.st"
}

/* Event function for state "next_channel" in state set "read_meter" */
static long E_read_meter_next_channel(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 399 "../Keithley2kDMM.st"
	if ((pVar->scan_chan) < ((pVar->channels) - 1))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 404 "../Keithley2kDMM.st"
	if ((pVar->scan_chan) >= ((pVar->channels) - 1))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "next_channel" in state set "read_meter" */
static void A_read_meter_next_channel(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 398 "../Keithley2kDMM.st"
			(pVar->scan_chan)++;
		}
		return;
	case 1:
		{
# line 401 "../Keithley2kDMM.st"
			(pVar->scan_chan) = 0;
			(pVar->done_read) = 0;
			seq_pvPut(ssId, 82 /* done_read */, 0);
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
  {"{P}{Dmm}ch1_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[0]), "ch_mode[0]", 
    "long", 1, 7, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch2_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[1]), "ch_mode[1]", 
    "long", 1, 8, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch3_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[2]), "ch_mode[2]", 
    "long", 1, 9, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch4_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[3]), "ch_mode[3]", 
    "long", 1, 10, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch5_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[4]), "ch_mode[4]", 
    "long", 1, 11, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch6_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[5]), "ch_mode[5]", 
    "long", 1, 12, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch7_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[6]), "ch_mode[6]", 
    "long", 1, 13, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch8_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[7]), "ch_mode[7]", 
    "long", 1, 14, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch9_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[8]), "ch_mode[8]", 
    "long", 1, 15, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch10_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[9]), "ch_mode[9]", 
    "long", 1, 16, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch11_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[10]), "ch_mode[10]", 
    "long", 1, 17, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch12_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[11]), "ch_mode[11]", 
    "long", 1, 18, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch13_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[12]), "ch_mode[12]", 
    "long", 1, 19, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch14_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[13]), "ch_mode[13]", 
    "long", 1, 20, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch15_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[14]), "ch_mode[14]", 
    "long", 1, 21, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch16_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[15]), "ch_mode[15]", 
    "long", 1, 22, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch17_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[16]), "ch_mode[16]", 
    "long", 1, 23, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch18_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[17]), "ch_mode[17]", 
    "long", 1, 24, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch19_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[18]), "ch_mode[18]", 
    "long", 1, 25, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch20_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[19]), "ch_mode[19]", 
    "long", 1, 26, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch21_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[20]), "ch_mode[20]", 
    "long", 1, 27, 1, 1, 0, 0, 0},

  {"{P}{Dmm}ch22_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode[21]), "ch_mode[21]", 
    "long", 1, 28, 1, 1, 0, 0, 0},

  {"{P}{Dmm}Ch1_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[0]), "ch_on_off[0]", 
    "long", 1, 29, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch2_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[1]), "ch_on_off[1]", 
    "long", 1, 30, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch3_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[2]), "ch_on_off[2]", 
    "long", 1, 31, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch4_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[3]), "ch_on_off[3]", 
    "long", 1, 32, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch5_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[4]), "ch_on_off[4]", 
    "long", 1, 33, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch6_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[5]), "ch_on_off[5]", 
    "long", 1, 34, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch7_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[6]), "ch_on_off[6]", 
    "long", 1, 35, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch8_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[7]), "ch_on_off[7]", 
    "long", 1, 36, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch9_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[8]), "ch_on_off[8]", 
    "long", 1, 37, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch10_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[9]), "ch_on_off[9]", 
    "long", 1, 38, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch11_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[10]), "ch_on_off[10]", 
    "long", 1, 39, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch12_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[11]), "ch_on_off[11]", 
    "long", 1, 40, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch13_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[12]), "ch_on_off[12]", 
    "long", 1, 41, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch14_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[13]), "ch_on_off[13]", 
    "long", 1, 42, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch15_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[14]), "ch_on_off[14]", 
    "long", 1, 43, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch16_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[15]), "ch_on_off[15]", 
    "long", 1, 44, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch17_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[16]), "ch_on_off[16]", 
    "long", 1, 45, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch18_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[17]), "ch_on_off[17]", 
    "long", 1, 46, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch19_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[18]), "ch_on_off[18]", 
    "long", 1, 47, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch20_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[19]), "ch_on_off[19]", 
    "long", 1, 48, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch21_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[20]), "ch_on_off[20]", 
    "long", 1, 49, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch22_on_off.VAL", (void *)OFFSET(struct UserVar, ch_on_off[21]), "ch_on_off[21]", 
    "long", 1, 50, 0, 1, 0, 0, 0},

  {"{P}{Dmm}Ch1_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[0]), "ch_raw[0]", 
    "long", 1, 51, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch2_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[1]), "ch_raw[1]", 
    "long", 1, 52, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch3_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[2]), "ch_raw[2]", 
    "long", 1, 53, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch4_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[3]), "ch_raw[3]", 
    "long", 1, 54, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch5_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[4]), "ch_raw[4]", 
    "long", 1, 55, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch6_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[5]), "ch_raw[5]", 
    "long", 1, 56, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch7_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[6]), "ch_raw[6]", 
    "long", 1, 57, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch8_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[7]), "ch_raw[7]", 
    "long", 1, 58, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch9_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[8]), "ch_raw[8]", 
    "long", 1, 59, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch10_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[9]), "ch_raw[9]", 
    "long", 1, 60, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch11_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[10]), "ch_raw[10]", 
    "long", 1, 61, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch12_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[11]), "ch_raw[11]", 
    "long", 1, 62, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch13_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[12]), "ch_raw[12]", 
    "long", 1, 63, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch14_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[13]), "ch_raw[13]", 
    "long", 1, 64, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch15_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[14]), "ch_raw[14]", 
    "long", 1, 65, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch16_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[15]), "ch_raw[15]", 
    "long", 1, 66, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch17_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[16]), "ch_raw[16]", 
    "long", 1, 67, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch18_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[17]), "ch_raw[17]", 
    "long", 1, 68, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch19_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[18]), "ch_raw[18]", 
    "long", 1, 69, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch20_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[19]), "ch_raw[19]", 
    "long", 1, 70, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch21_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[20]), "ch_raw[20]", 
    "long", 1, 71, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Ch22_raw.PROC", (void *)OFFSET(struct UserVar, ch_raw[21]), "ch_raw[21]", 
    "long", 1, 72, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_dcv.PROC", (void *)OFFSET(struct UserVar, dmm_modes[0]), "dmm_modes[0]", 
    "long", 1, 73, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_acv.PROC", (void *)OFFSET(struct UserVar, dmm_modes[1]), "dmm_modes[1]", 
    "long", 1, 74, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_ohm2w.PROC", (void *)OFFSET(struct UserVar, dmm_modes[2]), "dmm_modes[2]", 
    "long", 1, 75, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_ohm4w.PROC", (void *)OFFSET(struct UserVar, dmm_modes[3]), "dmm_modes[3]", 
    "long", 1, 76, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_freq.PROC", (void *)OFFSET(struct UserVar, dmm_modes[4]), "dmm_modes[4]", 
    "long", 1, 77, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempj.PROC", (void *)OFFSET(struct UserVar, dmm_modes[5]), "dmm_modes[5]", 
    "long", 1, 78, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempk.PROC", (void *)OFFSET(struct UserVar, dmm_modes[6]), "dmm_modes[6]", 
    "long", 1, 79, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempn.PROC", (void *)OFFSET(struct UserVar, dmm_modes[7]), "dmm_modes[7]", 
    "long", 1, 80, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempt.PROC", (void *)OFFSET(struct UserVar, dmm_modes[8]), "dmm_modes[8]", 
    "long", 1, 81, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempe.PROC", (void *)OFFSET(struct UserVar, dmm_modes[9]), "dmm_modes[9]", 
    "long", 1, 82, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempr.PROC", (void *)OFFSET(struct UserVar, dmm_modes[10]), "dmm_modes[10]", 
    "long", 1, 83, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_temps.PROC", (void *)OFFSET(struct UserVar, dmm_modes[11]), "dmm_modes[11]", 
    "long", 1, 84, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_tempb.PROC", (void *)OFFSET(struct UserVar, dmm_modes[12]), "dmm_modes[12]", 
    "long", 1, 85, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_dci.PROC", (void *)OFFSET(struct UserVar, dmm_modes[13]), "dmm_modes[13]", 
    "long", 1, 86, 0, 0, 0, 0, 0},

  {"{P}{Dmm}conf_aci.PROC", (void *)OFFSET(struct UserVar, dmm_modes[14]), "dmm_modes[14]", 
    "long", 1, 87, 0, 0, 0, 0, 0},

  {"{P}{Dmm}ch_mode_sel.VAL", (void *)OFFSET(struct UserVar, ch_mode_sel), "ch_mode_sel", 
    "long", 1, 88, 2, 1, 0, 0, 0},

  {"{P}{Dmm}done_read.VAL", (void *)OFFSET(struct UserVar, done_read), "done_read", 
    "long", 1, 89, 0, 1, 0, 0, 0},

  {"{P}{Dmm}dmm_chan.VAL", (void *)OFFSET(struct UserVar, dmm_chan), "dmm_chan", 
    "long", 1, 90, 3, 1, 0, 0, 0},

  {"{P}{Dmm}dmm_chan2.VAL", (void *)OFFSET(struct UserVar, dmm_chan2), "dmm_chan2", 
    "long", 1, 91, 4, 1, 0, 0, 0},

  {"{P}{Dmm}dmm_ch_range.VAL", (void *)OFFSET(struct UserVar, dmm_ch_range), "dmm_ch_range", 
    "long", 1, 92, 0, 1, 0, 0, 0},

  {"{P}{Dmm}dmm_delay_read.PROC", (void *)OFFSET(struct UserVar, dmm_delay_read), "dmm_delay_read", 
    "long", 1, 93, 0, 0, 0, 0, 0},

  {"{P}{Dmm}dmm_read.PROC", (void *)OFFSET(struct UserVar, dmm_read), "dmm_read", 
    "long", 1, 94, 0, 0, 0, 0, 0},

  {"{P}{Dmm}Dmm_raw.PROC", (void *)OFFSET(struct UserVar, Dmm_raw), "Dmm_raw", 
    "long", 1, 95, 0, 0, 0, 0, 0},

  {"{P}{Dmm}read_complete.VAL", (void *)OFFSET(struct UserVar, read_complete), "read_complete", 
    "long", 1, 96, 5, 1, 0, 0, 0},

  {"{P}{Dmm}single_multi.VAL", (void *)OFFSET(struct UserVar, single_multi), "single_multi", 
    "long", 1, 97, 0, 1, 0, 0, 0},

  {"{P}{Dmm}init_dmm.VAL", (void *)OFFSET(struct UserVar, init_dmm), "init_dmm", 
    "long", 1, 98, 6, 1, 0, 0, 0},

  {"{P}{Dmm}init_string.PROC", (void *)OFFSET(struct UserVar, do_init), "do_init", 
    "long", 1, 99, 0, 0, 0, 0, 0},

  {"{P}{Dmm}ch_close.VAL", (void *)OFFSET(struct UserVar, ch_close[0]), "ch_close", 
    "string", 1, 100, 0, 0, 0, 0, 0},

  {"{P}{Dmm}model.VAL", (void *)OFFSET(struct UserVar, model[0]), "model", 
    "string", 1, 101, 0, 0, 0, 0, 0},

  {"{P}{Dmm}channels.VAL", (void *)OFFSET(struct UserVar, channels), "channels", 
    "long", 1, 102, 0, 0, 0, 0, 0},

  {"{P}{Dmm}units.VAL", (void *)OFFSET(struct UserVar, dmm_units[0]), "dmm_units", 
    "string", 1, 103, 0, 0, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, ch_units[0]), "ch_units", 
    "string", 1, 104, 0, 0, 0, 0, 0},

};

/* Event masks for state set mode_change */
	/* Event mask for state init: */
static bitMask	EM_mode_change_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state monitor_mode_changes: */
static bitMask	EM_mode_change_monitor_mode_changes[] = {
	0x0000005e,
	0x00000000,
	0x00000000,
	0x00000040,
};

/* State Blocks */

static struct seqState state_mode_change[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_mode_change_init,
	/* event function */  (EVENT_FUNC) E_mode_change_init,
	/* delay function */   (DELAY_FUNC) D_mode_change_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_mode_change_init,
	/* state options */   (0)},

	/* State "monitor_mode_changes" */ {
	/* state name */       "monitor_mode_changes",
	/* action function */ (ACTION_FUNC) A_mode_change_monitor_mode_changes,
	/* event function */  (EVENT_FUNC) E_mode_change_monitor_mode_changes,
	/* delay function */   (DELAY_FUNC) D_mode_change_monitor_mode_changes,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_mode_change_monitor_mode_changes,
	/* state options */   (0)},


};

/* Event masks for state set read_meter */
	/* Event mask for state init: */
static bitMask	EM_read_meter_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state wait_read: */
static bitMask	EM_read_meter_wait_read[] = {
	0x00000000,
	0x00000000,
	0x02000000,
	0x00000000,
};
	/* Event mask for state read_channel: */
static bitMask	EM_read_meter_read_channel[] = {
	0xe0000000,
	0x0007ffff,
	0x00000000,
	0x00000002,
};
	/* Event mask for state single_read_response: */
static bitMask	EM_read_meter_single_read_response[] = {
	0x00000020,
	0x00000000,
	0x00000000,
	0x00000001,
};
	/* Event mask for state multi_read_response: */
static bitMask	EM_read_meter_multi_read_response[] = {
	0x00000020,
	0x00000000,
	0x00000000,
	0x00000001,
};
	/* Event mask for state next_channel: */
static bitMask	EM_read_meter_next_channel[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000040,
};

/* State Blocks */

static struct seqState state_read_meter[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_read_meter_init,
	/* event function */  (EVENT_FUNC) E_read_meter_init,
	/* delay function */   (DELAY_FUNC) D_read_meter_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_init,
	/* state options */   (0)},

	/* State "wait_read" */ {
	/* state name */       "wait_read",
	/* action function */ (ACTION_FUNC) A_read_meter_wait_read,
	/* event function */  (EVENT_FUNC) E_read_meter_wait_read,
	/* delay function */   (DELAY_FUNC) D_read_meter_wait_read,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_wait_read,
	/* state options */   (0)},

	/* State "read_channel" */ {
	/* state name */       "read_channel",
	/* action function */ (ACTION_FUNC) A_read_meter_read_channel,
	/* event function */  (EVENT_FUNC) E_read_meter_read_channel,
	/* delay function */   (DELAY_FUNC) D_read_meter_read_channel,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_read_channel,
	/* state options */   (0)},

	/* State "single_read_response" */ {
	/* state name */       "single_read_response",
	/* action function */ (ACTION_FUNC) A_read_meter_single_read_response,
	/* event function */  (EVENT_FUNC) E_read_meter_single_read_response,
	/* delay function */   (DELAY_FUNC) D_read_meter_single_read_response,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_single_read_response,
	/* state options */   (0)},

	/* State "multi_read_response" */ {
	/* state name */       "multi_read_response",
	/* action function */ (ACTION_FUNC) A_read_meter_multi_read_response,
	/* event function */  (EVENT_FUNC) E_read_meter_multi_read_response,
	/* delay function */   (DELAY_FUNC) D_read_meter_multi_read_response,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_multi_read_response,
	/* state options */   (0)},

	/* State "next_channel" */ {
	/* state name */       "next_channel",
	/* action function */ (ACTION_FUNC) A_read_meter_next_channel,
	/* event function */  (EVENT_FUNC) E_read_meter_next_channel,
	/* delay function */   (DELAY_FUNC) D_read_meter_next_channel,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_read_meter_next_channel,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "mode_change" */ {
	/* ss name */            "mode_change",
	/* ptr to state block */ state_mode_change,
	/* number of states */   2,
	/* error state */        -1},


	/* State set "read_meter" */ {
	/* ss name */            "read_meter",
	/* ptr to state block */ state_read_meter,
	/* number of states */   6,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=13LAB:, Dmm=DMM1, channels=10, model=2000";

/* State Program table (global) */
struct seqProgram Keithley2kDMM = {
	/* magic number */       20000315,
	/* *name */              "Keithley2kDMM",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void Keithley2kDMMRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&Keithley2kDMM);
}
epicsExportRegistrar(Keithley2kDMMRegistrar);

