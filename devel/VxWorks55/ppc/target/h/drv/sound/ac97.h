/* ac97.h - AC97 header file */

/* Copyright 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,18apr01,dmh public api provided for manually adjusting codec registers
01b,31oct00,dmh  extended to support ac97mixer.c
01a,14mar00,jlb  written.
*/

#ifndef __INCac97h
#define __INCac97h

#include <iosLib.h>
#include <semLib.h>

#if __cplusplus
extern "C" {
#endif

/*

  DESCRIPTION 
 
  This file provides definitions for the Intel Audio Codec 97 
 
  */


#define  AC97_RESET              0x0000      /* Reset */
#define  AC97_MASTER_VOL_STEREO  0x0002      /* Line Out */
#define  AC97_HEADPHONE_VOL      0x0004       
#define  AC97_MASTER_VOL_MONO    0x0006     
#define  AC97_MASTER_TONE        0x0008      
#define  AC97_PCBEEP_VOL         0x000a 
#define  AC97_PHONE_VOL          0x000c 
#define  AC97_MIC_VOL            0x000e  
#define  AC97_LINEIN_VOL         0x0010 
#define  AC97_CD_VOL             0x0012  
#define  AC97_VIDEO_VOL          0x0014     
#define  AC97_AUX_VOL            0x0016  
#define  AC97_PCMOUT_VOL         0x0018   
#define  AC97_RECORD_SELECT      0x001a     
#define  AC97_RECORD_GAIN        0x001c
#define  AC97_RECORD_GAIN_MIC    0x001e
#define  AC97_GENERAL_PURPOSE    0x0020
#define  AC97_3D_CONTROL         0x0022
#define  AC97_MODEM_RATE         0x0024
#define  AC97_POWER_CONTROL      0x0026

/* registers 0x0028 - 0x0058 are reserved */

/* AC'97 2.0 */
#define AC97_EXTENDED_ID        0x0028  /* Extended Audio ID */
#define AC97_EXTENDED_STATUS    0x002A  /* Extended Audio Status */
#define AC97_PCM_FRONT_DAC_RATE 0x002C  /* PCM Front DAC Rate */
#define AC97_PCM_SURR_DAC_RATE  0x002E  /* PCM Surround DAC Rate */
#define AC97_PCM_LFE_DAC_RATE   0x0030  /* PCM LFE DAC Rate */
#define AC97_PCM_LR_DAC_RATE    0x0032  /* PCM LR DAC Rate */
#define AC97_PCM_MIC_ADC_RATE   0x0034  /* PCM MIC ADC Rate */
#define AC97_CENTER_LFE_MASTER  0x0036  /* Center + LFE Master Volume */
#define AC97_SURROUND_MASTER    0x0038  /* Surround (Rear) Master Volume */
#define AC97_RESERVED_3A        0x003A  /* Reserved */
/* range 0x3c-0x58 - MODEM */

/* registers 0x005a - 0x007a are vendor reserved */

#define AC97_VENDOR_ID1         0x007c
#define AC97_VENDOR_ID2         0x007e

/* volume control bit defines */

#define AC97_MUTE               0x8000
#define AC97_MICBOOST           0x0040
#define AC97_LEFTVOL            0x3f00
#define AC97_RIGHTVOL           0x003f

/* record mux defines */

#define AC97_RECMUX_MIC         0x0000
#define AC97_RECMUX_CD          0x0101
#define AC97_RECMUX_VIDEO       0x0202      /* not used */
#define AC97_RECMUX_AUX         0x0303      
#define AC97_RECMUX_LINE        0x0404      
#define AC97_RECMUX_STEREO_MIX  0x0505
#define AC97_RECMUX_MONO_MIX    0x0606
#define AC97_RECMUX_PHONE       0x0707


/* general purpose register bit defines */

#define AC97_GP_LPBK            0x0080      /* Loopback mode */
#define AC97_GP_MS              0x0100      /* Mic Select 0=Mic1, 1=Mic2 */
#define AC97_GP_MIX             0x0200      /* Mono output select 0=Mix, 1=Mic */
#define AC97_GP_RLBK            0x0400      /* Remote Loopback - Modem line codec */
#define AC97_GP_LLBK            0x0800      /* Local Loopback - Modem Line codec */
#define AC97_GP_LD              0x1000      /* Loudness 1=on */
#define AC97_GP_3D              0x2000      /* 3D Enhancement 1=on */
#define AC97_GP_ST              0x4000      /* Stereo Enhancement 1=on */
#define AC97_GP_POP             0x8000      /* Pcm Out Path, 0=pre 3D, 1=post 3D */


/* powerdown control and status bit defines */

/* status */
#define AC97_PWR_MDM            0x0010      /* Modem section ready */
#define AC97_PWR_REF            0x0008      /* Vref nominal */
#define AC97_PWR_ANL            0x0004      /* Analog section ready */
#define AC97_PWR_DAC            0x0002      /* DAC section ready */
#define AC97_PWR_ADC            0x0001      /* ADC section ready */

/* control */
#define AC97_PWR_PR0            0x0100      /* ADC and Mux powerdown */
#define AC97_PWR_PR1            0x0200      /* DAC powerdown */
#define AC97_PWR_PR2            0x0400      /* Output mixer powerdown (Vref on) */
#define AC97_PWR_PR3            0x0800      /* Output mixer powerdown (Vref off) */
#define AC97_PWR_PR4            0x1000      /* AC-link powerdown */
#define AC97_PWR_PR5            0x2000      /* Internal Clk disable */
#define AC97_PWR_PR6            0x4000      /* HP amp powerdown */
#define AC97_PWR_PR7            0x8000      /* Modem off - if supported */

/* useful power states */
#define AC97_PWR_D0             0x0000      /* everything on */
#define AC97_PWR_D1             AC97_PWR_PR0 | AC97_PWR_PR1 | AC97_PWR_PR4
#define AC97_PWR_D2             AC97_PWR_PR0 | AC97_PWR_PR1 | \
                                AC97_PWR_PR2 | AC97_PWR_PR3 | AC97_PWR_PR4
#define AC97_PWR_D3             AC97_PWR_PR0 | AC97_PWR_PR1 | \
                                AC97_PWR_PR2 | AC97_PWR_PR3 | AC97_PWR_PR4
#define AC97_PWR_ANLOFF         AC97_PWR_PR2 | AC97_PWR_PR3 

/* Total number of defined registers.  */
#define AC97_REG_CNT    64

                   struct ac97_dev;
struct ac97mixtable_entry;
struct ac97_fd;

typedef struct ac97mixtable_entry
    {
    int		reg:8;
    unsigned	bits:4;
    unsigned	ofs:4;
    unsigned	stereo:1;
    unsigned	mute:1;
    unsigned	recidx:4;
    unsigned        mask:1;
    } AC97MIXTABLE_ENTRY;

typedef struct ac97_fd
    {
    struct ac97_dev  * pDev;
    UINT32             devs;
    UINT32           recSrc;
    UINT32          recDevs;
    SEM_ID            semId;
    } AC97_FD;

typedef struct ac97_dev
    {
    DEV_HDR             devHdr;
    FUNCPTR               init;
    FUNCPTR               read;
    FUNCPTR              write;
    UINT32                 key;
    AC97MIXTABLE_ENTRY mix[32];
    AC97_FD           mixer_fd;
    } AC97_DEV;
    

#ifndef AC97MIXER_C
/* public api for custom applications 
   note: an AC97_DEV* may be obtained using iosDevFind, 
   iosNextDevGet, or iosFdDevFind*/

extern void ac97Write(AC97_DEV * pDev, UINT32 reg, UINT16 val);
extern UINT16 ac97Read(AC97_DEV * pDev, UINT32 reg);

/* api to be used only by sound controller drivers
   since only sound controllers understand how to read
   and write to AC97 registers */

extern STATUS ac97MixerDrv();
extern AC97_DEV* ac97MixerDevCreate(DEV_HDR* pDev, FUNCPTR  rd, FUNCPTR wr);
#else 
STATUS ac97MixerDrv();
AC97_DEV* ac97MixerDevCreate(DEV_HDR* pDev, FUNCPTR rd, FUNCPTR wr);
void ac97Write(AC97_DEV * pDev, UINT32 reg, UINT16 val);
UINT16 ac97Read(AC97_DEV * pDev, UINT32 reg);
#endif


#if __cplusplus
} /* extern "C" */
#endif

#endif  /* __INCac97h */
