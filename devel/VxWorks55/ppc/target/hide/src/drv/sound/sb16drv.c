/* sb16drv.c - Sound Blaster 16 driver implemented for Cyrix MediaGXi CPU */
/* $Revision: 1.5 $ */
/* Copyright 1999-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,21sep01,dat  Fix ANSI violations for diab compiler
01c,27jan00,dmh  Corrected loop bug for audio on small buffers
01b,27oct99,spm  Sounds clean now, but record is still not implemented.
01a,03oct99,spm  created.  8 bit sound is still not working right.
*/

#include "vxWorks.h"
#include "errno.h"
#include "fcntl.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "iv.h"
#include "logLib.h"
#include "lstLib.h"
#include "msgQLib.h"
#include "semLib.h"
#include "stdlib.h"
#include "string.h"
#include "taskLib.h"

#include "drv/sound/sb16drv.h"

/* System function not in any include file.
 */
/* extern char *sysDmaMalloc (int); */

char snd_dmaBuffer[MAX_DMA_MSGS * MAX_DMA_SIZE];

LOCAL int DrvNum;

static const MIXER_INFO mixerTable[] =
{
  {"master",  SB_MIXER_MASTER_DEV,  31, 3, 0},
  {"pcm",     SB_MIXER_PCM_DEV,     31, 3, 0},
  {"synth",   SB_MIXER_SYNTH_DEV,   31, 3, 0},
  {"cd",      SB_MIXER_CD_DEV,      31, 3, 0},
  {"line",    SB_MIXER_LINE_DEV,    31, 3, 0},
  {"mic",     SB_MIXER_MIC_DEV,     31, 3, 0},
  {"speaker", SB_MIXER_SPEAKER_DEV,  3, 6, 1},
  {"igain",   SB_MIXER_OGAIN_DEV,    3, 6, 1},
  {"ogain",   SB_MIXER_IGAIN_DEV,    3, 6, 1},
  {"treble",  SB_MIXER_TREBLE_DEV,   7, 4, 1},
  {"bass",    SB_MIXER_BASS_DEV,     7, 4, 1},
  {NULL,      0,                     0, 0, 0}
};

/**
    uLaw to PCM table
    Used only for uLaw 8 bit.
*/
char ulaw_dsp[] = {
     3,    7,   11,   15,   19,   23,   27,   31,
    35,   39,   43,   47,   51,   55,   59,   63,
    66,   68,   70,   72,   74,   76,   78,   80,
    82,   84,   86,   88,   90,   92,   94,   96,
    98,   99,  100,  101,  102,  103,  104,  105,
    106,  107,  108,  109,  110,  111,  112,  113,
    113,  114,  114,  115,  115,  116,  116,  117,
    117,  118,  118,  119,  119,  120,  120,  121,
    121,  121,  122,  122,  122,  122,  123,  123,
    123,  123,  124,  124,  124,  124,  125,  125,
    125,  125,  125,  125,  126,  126,  126,  126,
    126,  126,  126,  126,  127,  127,  127,  127,
    127,  127,  127,  127,  127,  127,  127,  127,
    128,  128,  128,  128,  128,  128,  128,  128,
    128,  128,  128,  128,  128,  128,  128,  128,
    128,  128,  128,  128,  128,  128,  128,  128,
    253,  249,  245,  241,  237,  233,  229,  225,
    221,  217,  213,  209,  205,  201,  197,  193,
    190,  188,  186,  184,  182,  180,  178,  176,
    174,  172,  170,  168,  166,  164,  162,  160,
    158,  157,  156,  155,  154,  153,  152,  151,
    150,  149,  148,  147,  146,  145,  144,  143,
    143,  142,  142,  141,  141,  140,  140,  139,
    139,  138,  138,  137,  137,  136,  136,  135,
    135,  135,  134,  134,  134,  134,  133,  133,
    133,  133,  132,  132,  132,  132,  131,  131,
    131,  131,  131,  131,  130,  130,  130,  130,
    130,  130,  130,  130,  129,  129,  129,  129,
    129,  129,  129,  129,  129,  129,  129,  129,
    128,  128,  128,  128,  128,  128,  128,  128,
    128,  128,  128,  128,  128,  128,  128,  128,
    128,  128,  128,  128,  128,  128,  128,  128,
};

/* Useful macros.
 */
#define IS_16BITS(x) ((x)->info.sampleSize == 16)

/* Device Methods...
 */
static int sndCreate (SND_DEV *pDev, char *fileName, int mode);
static int sndOpen (SND_DEV *pDev, char *fileName, int flags, int mode);
static int sndClose (int devId);
static int sndRead (int devId, char *buffer, int maxbytes);
static int sndWrite (int devId, char *buffer, int nbytes);
static int sndIoctl (int devId, int function, int arg);

static int dspClose (int devId);
static int dspRead (int devId, char *buffer, int maxbytes);
static int dspWrite (int devId, char *buffer, int nbytes);
static int dspIoctl (int devId, int function, int arg);

static int mixerClose (int devId);
static int mixerRead (int devId, char *buffer, int maxbytes);
static int mixerWrite (int devId, char *buffer, int nbytes);
static int mixerIoctl (int devId, int function, int arg);

/* Interrupt handler and Helper task
 */
static void dspInterrupt (SND_DEV *pDev);
static int dspHelperTask (SND_DEV *pDev);

/* DMA buffer management routines
 */
static int createDmaBuffer (void);
static char *getDmaBuffer (SND_DEV *pDev);
static void freeDmaBuffer (SND_DEV *pDev);

/* Low level register access routines.
 */
static int dsp_init (SND_DEV *pDev);
static int dsp_version (SND_DEV *pDev);
static int dsp_reset (SND_DEV *pDev);

/* OPL3 functions...
 */
static int opl3_init (SND_DEV *pDev);

/* Mixer functions...
 */
static int mixer_init (SND_DEV *pDev);

/* Inlined small functions
 */
static __inline__ int dsp_command (SND_DEV *pDev, unsigned char val)
{
  int i;

  for (i = 10000; i; i--)
    if ((sysInByte (SBP(pDev, STATUS)) & 0x80) == 0)
    {
      sysOutByte (SBP(pDev, COMMAND), val);
      return 1;
    }
  return 0;
}

static __inline__ int opl3_command (unsigned short port,
				    unsigned char reg, unsigned char val)
{
  sysOutByte (port, reg);
  sysInByte (port);
  sysInByte (port);

  sysOutByte (port + 1, val);
  sysInByte (port + 1);
  sysInByte (port + 1);

  return 1;
}

static __inline__ int dsp_ack_8bit (SND_DEV * pDev)
{
  return sysInByte (SBP(pDev, DATA_AVAIL));
}

static __inline__ int dsp_ack_16bit (SND_DEV * pDev)
{
  return sysInByte (SBP(pDev, DATA_AVAIL_16));
}

static __inline__ void mixer_write (SND_DEV *pDev,
				    unsigned char reg, unsigned char data)
{
  sysOutByte (SBP(pDev, MIXER_ADDR), reg);
  sysOutByte (SBP(pDev, MIXER_DATA), data);
}

static __inline__ unsigned char mixer_read (SND_DEV *pDev, unsigned char reg)
{
  unsigned char result;

  sysOutByte (SBP(pDev, MIXER_ADDR), reg);
  result = sysInByte (SBP(pDev, MIXER_DATA));
  return result;
}

static __inline__ void mixer_set_level (SND_DEV * pDev, char *name, int value)
{
  const MIXER_INFO *info;
  int lvalue = value%256;
  int rvalue = value/256;

  for (info=mixerTable; info->name && strcmp (info->name, name); info++);

  if (info->name)
  {
      if (lvalue > 100) lvalue = 100;
      else if (lvalue<0) lvalue = 0;
      
      lvalue = (lvalue * info->max_value / 100) << info->shift;

      if (rvalue > 100) rvalue = 100;
      else if (rvalue<0) rvalue = 0;
      
      rvalue = (rvalue * info->max_value / 100) << info->shift;

      mixer_write (pDev, info->reg, lvalue);
      mixer_write (pDev, info->reg + 1, rvalue);
  }
}

static __inline__ int mixer_get_level (SND_DEV * pDev, char *name)
{
  const MIXER_INFO *info;

  for (info=mixerTable; info->name && strcmp (info->name, name); info++);

  if (info->name)
  {
      int lvalue = mixer_read (pDev, info->reg) >> info->shift;
      int rvalue = mixer_read (pDev, info->reg + 1) >> info->shift;
      lvalue = lvalue * 100 / info->max_value;
      rvalue = rvalue * 100 / info->max_value;
      return rvalue << 8 | lvalue;
  }
  return 0;
}

/* Routine to initiate Sound blaster DMAs.
 */
static void dsp_output (DSP_FD *pDsp, int length)
{
  SND_DEV *pDev = pDsp->dev.pDev;
  unsigned long count;
  unsigned char format;

  /* setup the sample rate.
   */
  dsp_command (pDev, SB_DSP_SAMPLE_RATE_IN);
  dsp_command (pDev, pDsp->info.rate >> 8);
  dsp_command (pDev, pDsp->info.rate & 0xff);

  dsp_command (pDev, SB_DSP_SAMPLE_RATE_OUT);
  dsp_command (pDev, pDsp->info.rate >> 8);
  dsp_command (pDev, pDsp->info.rate & 0xff);

  /* Determine the proper format of the output.
   */
  if (IS_16BITS(pDsp))
    format = pDsp->info.stereo ?
      SB_DSP4_MODE_SIGN_STEREO : SB_DSP4_MODE_SIGN_MONO;
  else
    format = pDsp->info.stereo ?
      SB_DSP4_MODE_UNS_STEREO : SB_DSP4_MODE_UNS_MONO;

  /* Tell the sound hardware how many samples.
   */
  count = (IS_16BITS(pDsp) ? (length >> 1) : length) - 1;

  dsp_command (pDev, IS_16BITS(pDsp) ? SB_DSP4_OUT16_AI : SB_DSP4_OUT8_AI);
  dsp_command (pDev, format);
  dsp_command (pDev, count & 0xff);
  dsp_command (pDev, count >> 8);
  dsp_command (pDev, IS_16BITS(pDev->pDsp) ?
	       SB_DSP_DMA16_ON : SB_DSP_DMA8_ON);
}

/* Device creation routines...
 */
STATUS sb16Drv (void)
{

  if (DrvNum > 0) return OK;

  DrvNum = iosDrvInstall (sndCreate, NULL, sndOpen, sndClose,
			  sndRead, sndWrite, sndIoctl);

  return (DrvNum == ERROR) ? ERROR : OK;
}

STATUS sb16DevCreate (char *devName, int port, int irq, int dma8, int dma16)
{
  SND_DEV *pDev;

  if (DrvNum < 1)
  {
    errno = S_ioLib_NO_DRIVER;
    return ERROR;
  }

  pDev = (SND_DEV *)malloc (sizeof(SND_DEV));
  if (!pDev) return ERROR;

  bzero ((char *)pDev, sizeof(SND_DEV));

  pDev->port   = port;
  pDev->irq    = irq;
  pDev->dma8   = dma8;
  pDev->dma16  = dma16;

  pDev->devSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
  pDev->intSem = semCCreate (SEM_Q_FIFO, 0);
  pDev->bufSem = semCCreate (SEM_Q_FIFO, MAX_DMA_MSGS);
  pDev->dmaQ   = msgQCreate (MAX_DMA_MSGS, sizeof (DMA_MSG), MSG_Q_FIFO);

  pDev->dmaIndex = 0;

  if (createDmaBuffer () < 0)
  {
    free (pDev);
    return ERROR;
  }

  if (dsp_init (pDev) < 0)
  {
    free (pDev);
    return ERROR;
  }

  /* This belongs in the mixer device...
   */
  if (mixer_init (pDev) < 0)
  {
    free (pDev);
    return ERROR;
  }

  if (iosDevAdd (&pDev->devHdr, devName, DrvNum) == ERROR)
  {
    free ((char *)pDev);
    return ERROR;
  }

  pDev->tid = taskSpawn ("tSndTask", TASK_PRIORITY, TASK_OPTIONS,
			 TASK_STACK_SIZE, dspHelperTask, (int)pDev,
			 0, 0, 0, 0, 0, 0, 0, 0, 0);

  if (pDev->tid == ERROR)
  {
    free (pDev);
    return ERROR;
  }

  intConnect (INUM_TO_IVEC (INT_VEC_GET (irq)), dspInterrupt, (int)pDev);

  return OK;
}

/* Device methods...
 */
static int sndCreate (SND_DEV *pDev, char *fileName, int mode)
{
  return ERROR;
}

static int sndOpen (SND_DEV *pDev, char *fileName, int flags, int mode)
{
  SND_FD *pSnd = NULL;

  if (semTake (pDev->devSem, 30 * sysClkRateGet()))
  {
    errno = S_ioLib_DEVICE_ERROR;
    return ERROR;
  }

  if (strcmp (fileName, "/dsp") == 0)
  {
    DSP_FD *pDsp;

    if (pDev->pDsp)
    {
      errno = EBUSY;
      goto error;
    }

    if (! (pDsp = malloc (sizeof(DSP_FD))))
    {
      errno = ENOMEM;
      goto error;
    }

    pDsp->dev.pDev    = pDev;
    pDsp->dev.fdClose = dspClose;
    pDsp->dev.fdRead  = dspRead;
    pDsp->dev.fdWrite = dspWrite;
    pDsp->dev.fdIoctl = dspIoctl;

    pDsp->info.rate       = RATE_MAX;
    pDsp->info.stereo     = 1;
    pDsp->info.sampleSize = 16;
    pDsp->info.uLaw       = 0; /* PCM format is default */

    pDev->pDsp = pDsp;

    pSnd = (SND_FD *)pDsp;
  }
  else if (strcmp (fileName, "/mixer") == 0)
  {
    MIXER_FD *pMixer;

    if (pDev->pMixer)
    {
      errno = EBUSY;
      goto error;
    }

    if (! (pMixer = malloc (sizeof(MIXER_FD))))
    {
      errno = ENOMEM;
      goto error;
    }

    pMixer->dev.pDev    = pDev;
    pMixer->dev.fdClose = mixerClose;
    pMixer->dev.fdRead  = mixerRead;
    pMixer->dev.fdWrite = mixerWrite;
    pMixer->dev.fdIoctl = mixerIoctl;

    pDev->pMixer = pMixer;

    pSnd = (SND_FD *)pMixer;
  }
  else
  {
    errno = ENODEV;
    goto error;
  }

error:
  semGive (pDev->devSem);
  return pSnd ? (int)pSnd : ERROR;
}

static int sndClose (int devId)
{
  SND_FD *pSnd = (SND_FD *)devId;

  return pSnd->fdClose (devId);
}

static int sndRead (int devId, char *buffer, int maxbytes)
{
  SND_FD *pSnd = (SND_FD *)devId;

  return pSnd->fdRead (devId, buffer, maxbytes);
}

static int sndWrite (int devId, char *buffer, int nbytes)
{
  SND_FD *pSnd = (SND_FD *)devId;

  return pSnd->fdWrite (devId, buffer, nbytes);
}

static int sndIoctl (int devId, int function, int arg)
{
  SND_FD *pSnd = (SND_FD *)devId;

  return pSnd->fdIoctl (devId, function, arg);
}

/* DSP device methods.
 */
static int dspClose (int devId)
{
  DSP_FD *pDsp = (DSP_FD *)devId;
  SND_DEV *pDev = pDsp->dev.pDev;

  if (semTake (pDev->devSem, 30 * sysClkRateGet()))
  {
    errno = S_ioLib_DEVICE_ERROR;
    return ERROR;
  }

  while (pDev->taskBusy) taskDelay (1);

  pDev->pDsp = NULL;
  free (pDsp);

  semGive (pDev->devSem);

  return OK;
}

static int dspRead (int devId, char *buffer, int maxbytes)
{
  errno = S_ioLib_UNKNOWN_REQUEST;
  return ERROR;
}

static int dspWrite (int devId, char *buffer, int nbytes)
{
  DSP_FD *pDsp = (DSP_FD *)devId;
  SND_DEV *pDev = pDsp->dev.pDev;
  int bytesLeft = nbytes;
  int x;
  DMA_MSG mDma;

  /**
    Convert from uLaw to PCM if needed -- note, we only
    support 8-bit uLaw
  */
  if (pDsp->info.uLaw) {
      for (x=0; x<nbytes; x++) {
          buffer[x] = ulaw_dsp[((unsigned char)buffer[x])];
      } /* endfor */
  } /* endfor */

  /* Check if we were recording.  If so, wait till all is clear.
   */
  if (pDev->pDsp->dmaDirection != O_WRONLY)
    while (pDev->taskBusy) taskDelay (1);

  while (bytesLeft > 0)
  {
    mDma.buffer    = getDmaBuffer (pDev);
    mDma.length    = bytesLeft > MAX_DMA_SIZE ? MAX_DMA_SIZE : bytesLeft;
    mDma.direction = O_WRONLY;

    memcpy (mDma.buffer, buffer, mDma.length);
    if (mDma.length<MAX_DMA_SIZE) {
        bzero(mDma.buffer+mDma.length,MAX_DMA_SIZE-mDma.length);
    } /* endif */


    msgQSend (pDev->dmaQ, (char *)&mDma, sizeof (mDma),
	      WAIT_FOREVER, MSG_PRI_NORMAL);

    buffer    += mDma.length;
    bytesLeft -= mDma.length;
  }

  return nbytes - bytesLeft;
}

static int dspIoctl (int devId, int function, int arg)
{
  DSP_FD *pDsp = (DSP_FD *)devId;
  SND_DEV *pDev = pDsp->dev.pDev;
  union arg_union
  {
    int i;
    long l;
    int *pInt;
    long *pLong;
    snd_info_t *pInfo;
  } u_arg;
  
  u_arg.i = arg;

  switch (function)
  {
    case SNDCTL_DSP_SYNC:
      while (pDev->taskBusy) taskDelay (1);
      return OK;

    case SNDCTL_DSP_GETBLKSIZE:
      *u_arg.pInt = MAX_DMA_SIZE;
      return OK;

    case SNDCTL_DSP_SPEED:
      {
	int i = *u_arg.pLong;

	if (i < RATE_MIN) i = RATE_MIN;
	if (i > RATE_MAX) i = RATE_MAX;
	while (pDev->taskBusy) taskDelay (1);
	pDsp->info.rate = i;
	return OK;
      }

    case SNDCTL_DSP_STEREO:
      while (pDev->taskBusy) taskDelay (1);
      pDsp->info.stereo = *u_arg.pInt;
      return OK;

    case SNDCTL_DSP_SAMPLESIZE:
      if (*u_arg.pInt == 8 || *u_arg.pInt == 16)
      {
	while (pDev->taskBusy) taskDelay (1);
	pDsp->info.sampleSize = *u_arg.pInt;
	return OK;
      }
      break;
    case SNDCTL_DSP_SETFORMAT:
      pDsp->info.uLaw = *u_arg.pInt;
      return OK;
      break;
    case SNDCTL_GET_INFO:
      *u_arg.pInfo = pDsp->info;
      return OK;

    case SNDCTL_SET_INFO:
      while (pDev->taskBusy) taskDelay (1);
      pDsp->info = *u_arg.pInfo;
      return OK;
  }
  errno = S_ioLib_UNKNOWN_REQUEST;
  return ERROR;
}

/* Mixer device methods.
 */
static int mixerClose (int devId)
{
  MIXER_FD *pMixer = (MIXER_FD *)devId;
  SND_DEV *pDev = pMixer->dev.pDev;

  if (semTake (pDev->devSem, 30 * sysClkRateGet()))
  {
    errno = S_ioLib_DEVICE_ERROR;
    return ERROR;
  }

  pDev->pMixer = NULL;
  free (pMixer);

  semGive (pDev->devSem);

  return OK;
}

static int mixerRead (int devId, char *buffer, int maxbytes)
{
  errno = S_ioLib_DEVICE_ERROR;
  return ERROR;
}

static int mixerWrite (int devId, char *buffer, int nbytes)
{
  errno = S_ioLib_DEVICE_ERROR;
  return ERROR;
}

static int mixerIoctl (int devId, int function, int arg)
{
  MIXER_FD *pMixer = (MIXER_FD *)devId;
  union arg_union
  {
    int i;
    long l;
    int *pInt;
    long *pLong;
    snd_info_t *pInfo;
  } u_arg;
  
  u_arg.i = arg;

  switch (function)
  {
    case SNDCTL_SET_VOLUME:
      {
	int volume = *u_arg.pLong;
	mixer_set_level (pMixer->dev.pDev, "master", volume);
	return OK;
      }

    case SNDCTL_GET_VOLUME:
      {
	*u_arg.pLong = mixer_get_level (pMixer->dev.pDev, "master");

	return OK;
      }
  }
  errno = S_ioLib_UNKNOWN_REQUEST;
  return ERROR;
}


static void processOutput (SND_DEV *pDev, DMA_MSG mDma)
{
  int dmaChannel = IS_16BITS(pDev->pDsp) ? pDev->dma16 : pDev->dma8;

  if (pDev->pDsp->dmaDirection != O_WRONLY ||
      pDev->pDsp->dmaChannel   != dmaChannel)
  {
    pDev->dmaAuto = 0;

    pDev->pDsp->dmaDirection = mDma.direction;
    pDev->pDsp->dmaChannel   = dmaChannel;
  }

  if (pDev->dmaAuto)
  {
    if (mDma.length != MAX_DMA_SIZE)
    {
      /* If this was the interrupt from the middle of the block,
       * then wait around for the next one.
       */
      if (pDev->dmaAuto)
      {
	pDev->dmaAuto = 0;

	if (semTake (pDev->intSem, 5 * sysClkRateGet()))
	  logMsg ("SB16: Interrupt timeout\n", 0, 0, 0, 0, 0, 0);

	freeDmaBuffer (pDev);
      }

      /* We are done with auto init mode.
       */
      dsp_command (pDev, IS_16BITS(pDev->pDsp) ?
		   SB_DSP_DMA16_EXIT : SB_DSP_DMA8_EXIT);

      /* Setup a one-shot.
       */
      dmaSetup (DMA_MODE_SINGLE | DMA_MODE_WRITE,
		mDma.buffer, mDma.length, dmaChannel);

      /* Send out this block.
       */
      dsp_output (pDev->pDsp, mDma.length);
    }
  }
  else
  {
    dsp_command (pDev, SB_DSP_DMA8_OFF);
    dsp_command (pDev, SB_DSP_DMA16_OFF);

    if (mDma.buffer == snd_dmaBuffer && mDma.length == MAX_DMA_SIZE)
    {
      pDev->dmaAuto = 2;

      dmaSetup (DMA_MODE_SINGLE | DMA_MODE_WRITE | DMA_MODE_AUTO_ENABLE,
		snd_dmaBuffer, MAX_DMA_MSGS * MAX_DMA_SIZE, dmaChannel);

      /* Check for the next buffer available when half the buffer
       * has been transferred.
       */
      mDma.length >>= 1;
    }
    else
    {
      dmaSetup (DMA_MODE_SINGLE | DMA_MODE_WRITE,
		mDma.buffer, mDma.length, dmaChannel);
    }

    /* Send out this block.
     */
    dsp_output (pDev->pDsp, mDma.length);
  }
}

/* Interrupt handler and Helper task
 */
static void dspInterrupt (SND_DEV *pDev)
{
  int status;

  sysOutByte (SBP(pDev, MIXER_ADDR), SB_MIXER_IRQ_STAT);
  status = sysInByte (SBP(pDev, MIXER_DATA));

  /* 8bit DMA interrupt
   */
  if (status & 1)
  {
    dsp_ack_8bit (pDev);
    semGive (pDev->intSem);
  }

  /* 16bit DMA interrupt
   */
  if (status & 2)
  {
    dsp_ack_16bit (pDev);
    semGive (pDev->intSem);
  }
}

static int dspHelperTask (SND_DEV *pDev)
{
  DMA_MSG mDma;

  while (1)
  {
    pDev->taskBusy = pDev->dmaAuto = 0;

    dsp_command (pDev, SB_DSP_SPEAKER_OFF);
    dsp_command (pDev, SB_DSP_DMA8_OFF);
    dsp_command (pDev, SB_DSP_DMA16_OFF);

    if (msgQReceive (pDev->dmaQ, (char *)&mDma,
		     sizeof (mDma), WAIT_FOREVER) != sizeof (mDma))
      return 0;

    pDev->taskBusy = 1;
    dsp_command (pDev, SB_DSP_SPEAKER_ON);

    do
    {
      switch (mDma.direction)
      {
	case O_WRONLY:
	  processOutput (pDev, mDma);
	  break;
	case O_RDONLY:
	  break;
      }

      /* wait for a DMA interrupt
       */
      if (semTake (pDev->intSem, 5 * sysClkRateGet()))
	logMsg ("SB16: Interrupt timeout\n", 0, 0, 0, 0, 0, 0);

      if (pDev->dmaAuto)
      {
	/* We can do this, because we know the stream will be
	 * non-contiguous if any properties change (rate, direction, etc).
	 */
	while (pDev->dmaAuto)
	{
	  pDev->dmaAuto--;

	  if (pDev->dmaAuto)
	  {
	    if (msgQReceive (pDev->dmaQ, (char *)&mDma,
			     sizeof (mDma), NO_WAIT) != sizeof (mDma))
	    {
	      pDev->dmaAuto = 0;
	    }
	  }
	  else
	  {
	    /* Great, we can keep going with 2 more interrupts to follow.
	     */
	    pDev->dmaAuto = 2;

	    freeDmaBuffer (pDev);
	  }

	  /* wait for the next DMA interrupt
	   */
	  if (semTake (pDev->intSem, 5 * sysClkRateGet()))
	    logMsg ("SB16: Interrupt timeout\n", 0, 0, 0, 0, 0, 0);
	}

	/* We can't do any more DMA continuously.
	 */
	dsp_command (pDev, IS_16BITS(pDev->pDsp) ?
		     SB_DSP_DMA16_EXIT : SB_DSP_DMA8_EXIT);
      }

      freeDmaBuffer (pDev);

    } while (msgQReceive (pDev->dmaQ, (char *)&mDma,
			  sizeof (mDma), NO_WAIT) == sizeof (mDma));
  }
}


/* DMA buffer management routines
 */

/* DMA buffers are never grabbed by the interrupt routine
 */
static int createDmaBuffer (void)
{
/*   snd_dmaBuffer = sysDmaMalloc (MAX_DMA_MSGS * MAX_DMA_SIZE); */

  bzero (snd_dmaBuffer, MAX_DMA_MSGS * MAX_DMA_SIZE);

  return snd_dmaBuffer ? OK : ERROR;
}

static char *getDmaBuffer (SND_DEV *pDev)
{
  char *dmaBuffer;

  semTake (pDev->bufSem, WAIT_FOREVER);

  semTake (pDev->devSem, WAIT_FOREVER);
  dmaBuffer = snd_dmaBuffer + pDev->dmaIndex * MAX_DMA_SIZE;
  pDev->dmaIndex = (pDev->dmaIndex + 1) % MAX_DMA_MSGS;
  semGive (pDev->devSem);

  return dmaBuffer;
}

/* DMA buffers are always freed by the interrupt routine
 */
static void freeDmaBuffer (SND_DEV *pDev)
{
  semGive (pDev->bufSem);
}


/* Low level register access routines.
 */
static int dsp_init (SND_DEV *pDev)
{
  int version;

  /* Initialization the DSP
   */
  if (dsp_reset (pDev) < 0)
  {
    logMsg ("SB16: [0x%x] reset failed... 0x%x\n",
	    pDev->port, sysInByte (SBP(pDev, READ)), 0, 0, 0, 0);
    return -ENODEV;
  }

  /* Get the version number of the DSP
   */
  if ((version = dsp_version (pDev)) < 0)
  {
    logMsg ("SB16: [0x%x] get version failed... 0x%x\n",
	    pDev->port, sysInByte (SBP(pDev, READ)), 0, 0, 0, 0);
    return -ENODEV;
  }

  pDev->version = version;

  switch (pDev->version >> 8)
  {
    case 1: case 2: case 3:
      logMsg ("SB16: [0x%x] DSP chip version %i.%i is not supported"
	      " with the SB16 code\n", pDev->port, pDev->version >> 8,
	      pDev->version & 0xff, 0, 0, 0);
      return -ENODEV;
    case 4:
      break;
    default:
      logMsg ("SB16: [0x%x] unknown DSP chip version %i.%i\n",
	      pDev->port, pDev->version >> 8, pDev->version & 0xff, 0, 0, 0);
      return -ENODEV;
  }

  dsp_command (pDev, SB_DSP_SPEAKER_OFF);

  opl3_init (pDev);

  return 0;
}

static int dsp_version (SND_DEV *pDev)
{
  int i;
  unsigned int result = -1;

  sysIntDisablePIC (pDev->irq);
  dsp_command (pDev, SB_DSP_GET_VERSION);
  for (i = 100000; i; i--)
    if (sysInByte (SBP(pDev, DATA_AVAIL)) & 0x80)
    {
      result = (short) sysInByte (SBP(pDev, READ)) << 8;
      break;
    }
  for (i = 100000; i; i--)
    if (sysInByte (SBP(pDev, DATA_AVAIL)) & 0x80)
    {
      result |= (short) sysInByte (SBP(pDev, READ));
      break;
    }
  sysIntEnablePIC (pDev->irq);
  return result;
}

static int dsp_reset (SND_DEV *pDev)
{
  int i;

  sysIntDisablePIC (pDev->irq);
  sysOutByte (SBP(pDev, RESET), 1);
  sysDelay ();
  sysOutByte (SBP(pDev, RESET), 0);
  sysDelay ();
  for (i = 0; i < 1000 && !(sysInByte (SBP(pDev, DATA_AVAIL)) & 0x80); i++);
  i = sysInByte (SBP(pDev, READ));
  sysIntEnablePIC (pDev->irq);
  if (i == 0xaa) return 0;

  return -ENODEV;
}


/* OPL3 functions...
 */
static int opl3_init (SND_DEV *pDev)
{
  int i;

  /* Reset opl3 timers 1 and 2
   */
  opl3_command (SBP(pDev, OPL3_LEFT), TIMER_CONTROL_REGISTER,
		TIMER1_MASK | TIMER2_MASK);

  /* Reset the IRQ of the FM chip
   */
  opl3_command (SBP(pDev, OPL3_LEFT), TIMER_CONTROL_REGISTER, IRQ_RESET);

  i = sysInByte (SBP(pDev, OPL3_LEFT));

  if (i == 0x00 || i == 0x0f)
  {
    opl3_command (SBP(pDev, OPL3_RIGHT), OPL3_MODE_REGISTER, 0x00);
    opl3_command (SBP(pDev, OPL3_RIGHT), OPL3_MODE_REGISTER, OPL3_ENABLE);
    opl3_command (SBP(pDev, OPL3_RIGHT), CONNECTION_SELECT_REGISTER, 0x00);

    for (i=0; i<9; i++)
      opl3_command (SBP(pDev, OPL3_LEFT), KEYON_BLOCK, 0);

    opl3_command (SBP(pDev, OPL3_LEFT), TEST_REGISTER, ENABLE_WAVE_SELECT);
    opl3_command (SBP(pDev, OPL3_LEFT), PERCUSSION_REGISTER, 0x00);

    opl3_command (SBP(pDev, OPL3_RIGHT), OPL3_MODE_REGISTER, 0x00);
  }
  else
    logMsg ("SB16: [0x%x] OPL3 detect failed... 0x%x\n",
	    pDev->port, i, 0, 0, 0, 0);

  return 0;
}

/* Mixer stuff.  Really belongs in it's own device....
 */
static int mixer_init (SND_DEV *pDev)
{
  int irq;
  int mpu;

  switch (pDev->irq)
  {
    case 2: case 9: irq = 0x01; break;
    case 5:         irq = 0x02; break;
    case 7:         irq = 0x04; break;
    case 10:        irq = 0x08; break;
    default:        irq = 0x00;
  }

  mixer_write (pDev, 0x0, 0x0); /* reset mixer */
  sysDelay ();

  mpu = mixer_read (pDev, SB_MIXER_MPU_NR) & ~0x06;

  mixer_write (pDev, SB_MIXER_IRQ_NR, irq);
  mixer_write (pDev, SB_MIXER_DMA_NR, (1 << pDev->dma16) | (1 << pDev->dma8));

  mixer_write (pDev, SB_MIXER_MPU_NR, mpu & 0x02); /* disable mpu */

  /* Turn on all inputs.
   */
  mixer_write (pDev, SB_MIXER_OUTPUT_SW, 0x1f);

  /* Set volumes to reasonable values for now...
   */
  mixer_set_level (pDev, "master", 90 << 8 | 90);
  mixer_set_level (pDev, "pcm", 90 << 8 | 90);

  mixer_set_level (pDev, "speaker", 0);
  mixer_set_level (pDev, "ogain", 0);
  mixer_set_level (pDev, "igain", 0);

  mixer_set_level (pDev, "treble", 50);
  mixer_set_level (pDev, "bass",   50);

  return 0;
}



/**
$Log: sb16drv.c,v $
Revision 1.5  1999/12/18 00:01:38  steveh
Corrected loop bug for audio on small buffers.
Connected up fwd,back and stop buttons.
Attempted to connect up logic to display link that mouse is over,
but api seems to not work for this.

Revision 1.4  1999/12/03 06:52:09  steveh
Created new IOCTL to set 8-bit uLaw format in SB driver.
Moved uLaw code from java to driver.

*/


