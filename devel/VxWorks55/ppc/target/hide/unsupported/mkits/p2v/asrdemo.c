/* asrdemo.c - p2v Transition Kit Demo Application */

/* Copyright 1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,22jan97,jmb  Cleanup.
01b,20jan97,jmb  Added KILL code.
01a,14jan97,jmb  written.
*/

/*
 * This program demos the use of pSOS+ events, asynchronous signals,
 * semphores, and task registers.
 *
 * To build asrdemo, put this file in your BSP directory
 * and do "make asrdemo.o".  
 *
 * To run asrdemo:
 *     > sp asrdemo,'x'
 *
 * Where x is any character.  This character will be the suffix
 * for all task names spawned in the demo, and will be the character
 * printed to stdout that represents transmitter pulses.  This allows you to
 * differentiate the outputs of multiple instantiations of the demo.
 * 
 * The amplitude and frequency of the pulses output by the transmitter
 * are 1 unit by default.  They can be increased up to a value of 16
 * by sending events 1-16 to the amplitude and frequency controllers.
 * To pause the transmitter, send event = -1 to the amplitude controller.
 * To resume the transmitter, send event = -2 to the amplitude controller.
 * To kill the transmitter, send event = -3 to the amplitude controller.
 */

#include "/vobs/wpwr/target/unsupported/mkits/p2v/p2vLib.h"

/* Define Transmitter register set */

#define AMP_REG 1		
#define FREQ_REG 2
#define STAT_REG 3
#define SEM_REG 4
#define SUFFIX_REG 5

/* Define max input values from user and scroll rate of pulses */
 
#define MAXAMP 16
#define MAXFREQ 16
#define SYS_CLOCK_RATE 60
#define MAX_PULSES_PER_SECOND 5

/* Define input events to amplitude controller indicating start/stop */

#define STOP_CODE 0xffffffff
#define START_CODE 0xfffffffe
#define KILL_CODE 0xfffffffd

/* Define input signal to transmitter ASR indicating start/stop */

#define STOP_CODE_SHORT 0xffff
#define START_CODE_SHORT 0xfffe
#define KILL_CODE_SHORT	0xfffd

/* Define values of transmitter status register */

#define RUNNING 1
#define STOPPED 0

/**********************************************************************
 *   Amplitude Controller
 *
 *   Make sure amplitude value is in the range 1-MAXAMP,
 *   or is the stop code (-1) or start code (-2).
 *   If no error, forward the value to the right half
 *   of the transmitter's ASR register.
 */
void
sendAmp (unsigned int tid)
   {
   unsigned int event;
   unsigned int amp;
   for (;;)
      {
      ev_receive (0xffffffff, EV_ANY, 0, &event);

      if (event > MAXAMP)
         if (event != STOP_CODE && event != START_CODE &&
	     event != KILL_CODE)
	    {
	    printf("Amplitude must be in range 1-16, not = %d\n",event);
	    continue;
	    }
      amp = event & 0xffff;
      as_send (tid, amp);
      }
   }

/****************************************************************
 *   Frequency Controller
 *
 *   Make sure frequency value is in the range 1-MAXFREQ.
 *   If no error, forward the value to the left half
 *   of the transmitter's ASR register.
 */
void
sendFreq (unsigned int tid)
   {
   unsigned int event;
   unsigned int freq;
   for (;;)
      {
      ev_receive(0xffffffff, EV_ANY, 0, &event);

      if (event > MAXFREQ)
	 {
	 printf("Frequency must be in range 1-16, not = %d\n",event);
	 continue;
	 }
      freq = event << 16;
      as_send (tid, freq);
      }
   }

/*********************************************************************
 *   Transmitter ASR
 */
void
chatterASR (int dummy1, int dummy2, int dummy3, unsigned int events)
   {
   unsigned int amp;
   unsigned int freq;
   unsigned int smid;
   unsigned int status;
   unsigned int suffix;
   unsigned int tid;
   char taskName[4];

   if (events == 0)   			/* This should never happen */
      return;
/*
 *  Extract frequency from left short word.
 */
   freq = events >> 16;

   if (freq)
      {
      t_setreg (0, FREQ_REG, freq);  	/* update frequency register */
      return;                        /* all done */
      }
/*
 *  Extract amplitude from right short word.
 */
   amp = events & 0xffff;

   if (amp < (MAXAMP + 1))  			
      {
      t_setreg(0,AMP_REG,amp);		/* update amplitude register */
      return;				/* all done */
      }

/* Handle special cases of amplitude being start, stop, or kill signal 
 *
 *  Start/stop logic:
 *     If we get start code and we're running, do nothing.
 *     If we get stop code and we're stopped, do nothing.
 *     If we get start code and we're stopped, give the semaphore twice.
 *        It's necessary to give it twice because, we never want the
 *        ASR to block, just the transmitter.  Set the status to RUNNING.
 *     If we get stop code and we're running, take the semaphore until
 *        it is no longer available.  (It's possible to take it more than
 *        once since multiple ASR's can occur before transmitter resumes.)
 *        Set the status to STOPPED.
 */
   t_getreg (0, STAT_REG, &status);

   if (amp == START_CODE_SHORT )
      {
      if (status == STOPPED)  	
	 {
	 t_getreg (0, SEM_REG, &smid);
	 sm_v (smid);
	 sm_v (smid);
	 t_setreg (0, STAT_REG, RUNNING);
	 }
      }
   else if (amp == STOP_CODE_SHORT)
      {
      if (status == RUNNING)
	 {
	 t_getreg (0, SEM_REG, &smid);
         while ( sm_p (smid, SM_NOWAIT, 0) == EOK )
	   ;
         t_setreg (0, STAT_REG, STOPPED);
	 }
      }
   else /* amp == KILL_CODE_SHORT */
      {
      /* Delete the start/stop semaphore */

      t_getreg (0, SEM_REG, &smid);
      sm_delete (smid);

      /* Delete the amplitude controller task */

      t_getreg (0, SUFFIX_REG, &suffix);
      strcpy (taskName, "AMP");
      taskName[3] = suffix;
      if (t_ident (taskName, 0, &tid) == EOK) 
         t_delete(tid);

      /* Delete the frequency controller task */

      strcpy (taskName, "FRQ");
      taskName[3] = suffix;
      if (t_ident (taskName, 0, &tid) == EOK) 
         t_delete(tid);

      /* Self-destruct */
      t_delete (0);
      }

   return;

   }

/*********************************************************************
 *   Transmitter 
 */

void
chatter(unsigned int value)
   {
   unsigned int amp;
   unsigned int freq;
   unsigned int status;
   unsigned int smid;
   char semName[4];
   unsigned int baseDelay;
   char wave [2 * MAXAMP + 1];
   int j;
   int i=0;

   /* Initialize Waveform */

   for (j = 0; j < (2 * MAXAMP + 1); j++)
      wave[j] = ' ';
   wave[2*MAXAMP+1] = '\0';
   t_setreg (0, AMP_REG, 1);  /* lowest amplitude */
   t_setreg (0, FREQ_REG, 1);  /* lowest frequency */
   t_setreg (0, SUFFIX_REG, value);  /* used when handling kill signal */

   /* Set the delay corresponding to the highest frequency */

   baseDelay = SYS_CLOCK_RATE / MAX_PULSES_PER_SECOND; 

   /* Create a synchronization semaphore */

   strcpy (semName, "SMT");
   semName[3] = value;
   sm_create (semName, 1, 0, &smid);   /* Initial value = 1, status RUNNING */

   /* Put it in taskreg 3 for access by ASR */

   t_setreg (0, SEM_REG, smid);

   /* Set initial status to RUNNING */

   t_setreg (0, STAT_REG, RUNNING);

   /* Install ASR */

   as_catch ((FUNCPTR) chatterASR, 0);

   for(;;)
      {

      /* Check whether to stop transmitting */

      t_getreg (0, STAT_REG, &status);

      if (status == STOPPED)  
	 sm_p (smid, 0, 0);

      /*  Get amplitude and frequency */

      t_getreg (0, AMP_REG, &amp);
      t_getreg (0, FREQ_REG, &freq);

      /* Transmit one pulse */

      if (0x1 & i++) 
	wave[MAXAMP - amp] = value;
      else
	wave[MAXAMP + amp] = value;

      printf("%s\n",wave);

      if (0x1 & i)                  /* clean-up output line for next pass */
	wave[MAXAMP + amp] = ' ';
      else
	wave[MAXAMP - amp] = ' ';

      /* Sleep till time to transmit again */

      tm_wkafter((MAXFREQ * baseDelay) / freq); 
      }
   }

/*
 *  Main Program
 */
void
asrdemo(int suffix)
   {
   UINT32 tid;
   UINT32 txid;
   char xmitName[4];
   char freqName[4];
   char ampName[4];
   unsigned int args[4];
   args[1] = 0;
   args[2] = 0;
   args[3] = 0;
   
   /*  Spawn the transmitter task */

   strcpy (xmitName, "XMT");
   xmitName[3] = suffix;
   args[0] = suffix;
   t_create (xmitName, 153, 2000, 1000, 0x0, &txid);
   t_start (txid, 0x0, chatter, args);
   
   /*  Spawn the amplitude controller task */

   args[0] = txid;
   strcpy (ampName, "AMP");
   ampName[3] = suffix;
   t_create (ampName, 154, 2000, 1000, 0x0, &tid);
   t_start (tid, 0x0, sendAmp, args);
   
   /*  Spawn the frequency controller task */

   strcpy (freqName, "FRQ");
   freqName[3] = suffix;
   t_create (freqName, 154, 2000, 1000, 0x0, &tid);
   t_start(tid, 0x0, sendFreq, args);
   }
