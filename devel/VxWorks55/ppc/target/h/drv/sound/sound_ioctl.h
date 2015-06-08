/* sound_ioctl.h - Sound driver generic interface */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,03oct99,spm  created.
*/


#ifndef _INCsound_ioctlh
#define _INCsound_ioctlh

#ifndef linux
#include "ioLib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  unsigned long rate;       /* Samples per second      */
  unsigned int stereo;      /* 0=mono, 1=stereo        */
  unsigned int sampleSize;  /* 8 or 16 bits per sample */
  unsigned int uLaw;        /* 1 = uLaw, 0 = PCM       */
} snd_info_t;

#define SNDCTL_BASE                    0x4000

#define SNDCTL_DSP_SYNC                (SNDCTL_BASE + 0x01)
#define SNDCTL_DSP_GETBLKSIZE          (SNDCTL_BASE + 0x02)

#define SNDCTL_DSP_SPEED               (SNDCTL_BASE + 0x11)
#define SNDCTL_DSP_STEREO              (SNDCTL_BASE + 0x12)
#define SNDCTL_DSP_SAMPLESIZE          (SNDCTL_BASE + 0x13)
#define SNDCTL_DSP_SETFORMAT           (SNDCTL_BASE + 0x14)

#define SNDCTL_GET_INFO                (SNDCTL_BASE + 0x21)
#define SNDCTL_SET_INFO                (SNDCTL_BASE + 0x22)

#define SNDCTL_SET_VOLUME              (SNDCTL_BASE + 0x23)
#define SNDCTL_GET_VOLUME              (SNDCTL_BASE + 0x24)

#ifdef __cplusplus
}
#endif

#endif /* __INCsound_ioctlh */
