/* usrUsbAudioDemo.c - USB Audio Demo */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01b,08dec01,wef	fixed some warnings
01a,05mar00,wef	Created - adapted from usbTool.
*/

/*
DESCRIPTION

This file demonstrates the use of the USB stack, either the UHCI or OHCI host 
controller driver, and the USB audio class driver.  It is assumed that this 
source will file be included using the project facility to pull in the 
INCLUDE_USB_AUDIO_DEMO. In addtion this file assumes the USB stack has already
been initialized and a host controller has been attached.  Finally, this 
demonstration als makes the assumption that the target system has wav files 
stored on a ATA type device.  

USB audio Demo instructions

When yoru vxWorks image boots, use the demo by entering any of the following 
commands at the vxWorks prompt:

play [file] 	Plays a wave file.  The file name is a file that exists in the
		current	working directory.

stop		stops song that is currently playing

up		raises the volume level

down		lowers the volume level

mute		brings the volume level to a tolerable level

disVol		shows the current volume level

This demo works with any of the supported USB speakers listed in the 
.I "USB Developer's Kit User's Guide"

*/

/* Include files */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ioLib.h"
#include "ctype.h"
#include "semLib.h"
#include "usrConfig.h"

#include "usb/usbPlatform.h"		/* Basic definitions */
#include "usb/ossLib.h"			/* OS abstraction definitions */
#include "usb/tools/cmdParser.h"	/* Command parser util funcs */


#include "usb/tools/wavFormat.h"	/* Microsoft .wav file format */
#include "usb/usbAudio.h"		/* USB audio definitions */
#include "drv/usb/usbSpeakerLib.h"	/* USB speaker SEQ_DEV driver */

/* Defines */

#define	AUDIO_BFR_SIZE				0x8000
#define MAX_NUM_SONGS				14


/* Locals */

long globalVolume = 0xd000;

LOCAL BOOL spkrInitialized = FALSE;


/*************************************************************************
*
* spkrAttachCallback - receives attach callbacks from speaker SEQ_DEV driver
*
* RETURNS: N/A
*/

LOCAL SEQ_DEV *pSpkrSeqDev = NULL;

LOCAL VOID spkrAttachCallback
	(
	pVOID arg,					/* caller-defined argument */
	SEQ_DEV *pSeqDev,				/* pointer to affected SEQ_DEV */
	UINT16 attachCode				/* defined as USB_KBD_xxxx */
	)

    {

    if (attachCode == USB_SPKR_ATTACH)
	{
	if (pSpkrSeqDev == NULL)
	    {
	    if (usbSpeakerSeqDevLock (pSeqDev) != OK)
		printf ("usbSpeakerSeqDevLock() returned ERROR\n");
	    else
		{
		pSpkrSeqDev = pSeqDev;
		}
	    }
	else
	    {
	    printf ("Another channel already in use, ignored.\n");
	    }
	}
    else
	{
	if (pSeqDev == pSpkrSeqDev)
	    {
	    if (usbSpeakerSeqDevUnlock (pSeqDev) != OK)
		printf ("usbSpeakerSeqDevUnlock() returned ERROR\n");

	    pSpkrSeqDev = NULL;
	    }
	}
    }


/***************************************************************************
*
* audioThread - Dumps audio data to usbSpeakerLib
*
* By convention, <param> is the file handle for the file to be played and 
* the global "wavDataLength" should be the length of the data chunk.  The
* file position should be set to the beginning of the data in the data chunk.
*
* This thread closes the file after reading all data.
*
* RETURNS: N/A
*/

BOOL audioThreadBusy = FALSE;
LOCAL UINT32 wavDataLen;
LOCAL    BOOL stopFlag = FALSE;

VOID audioThread
	(
	pVOID param
	)

    {
    FILE *wavFile = (FILE *) param;
    pUINT8 pBfr;
    UINT32 remDataLen = wavDataLen;
    UINT32 actLen;


    stopFlag = FALSE;

    /* Create a buffer for audio data */

    if ((pBfr = malloc (AUDIO_BFR_SIZE)) == NULL)
	{
	printf ("Out of memory creating audio buffer.\n");
	}
    else
	{
	/* open the audio stream. */

	if ((*pSpkrSeqDev->sd_ioctl) (pSpkrSeqDev, 
				      USB_SPKR_IOCTL_OPEN_AUDIO_STREAM, 0) != OK)
	    {
	    printf ("IOCTL OPEN_AUDIO_STREAM returned ERROR.\n");
	    }
	else
	    {
	    /* Read audio data and pass it to usbSpeakerLib. */

	    while ((stopFlag == FALSE) && remDataLen > 0 &&
		    (actLen = fread (pBfr, 
				     1, 
				     min (remDataLen, AUDIO_BFR_SIZE), 
				     wavFile)) > 0)
		{
		if ((*pSpkrSeqDev->sd_seqWrt) (pSpkrSeqDev, 
					       actLen,
					       pBfr, 
					       FALSE) 
					     != OK)
		    {
		    printf ("sd_seqWrt() returned ERROR.\n");
		    break;
		    }
		else
		    {
		    remDataLen -= actLen;
		    }
		}

	    /* Mark the end of the audio stream. */

	    if ((*pSpkrSeqDev->sd_ioctl) (pSpkrSeqDev, 
		    USB_SPKR_IOCTL_CLOSE_AUDIO_STREAM, 0) != OK)
		{
		printf ("IOCTL CLOSE_AUDIO_STREAM returned ERROR.\n");
		}
	    }

	free (pBfr);
	}

    /* Close the input file. */

    fclose (wavFile);

    audioThreadBusy = FALSE;

    }

/*************************************************************************
*
* enterThread - waits for user to press [enter]
*
* RETURNS:	N/A
*/

LOCAL BOOL enterPressed;

LOCAL VOID enterThread
	(
	pVOID param
	)

    {
    FILE *fout = (FILE *) param;
    char bfr [256];

    fprintf (fout, "Press [enter] to terminate polling.\n");
    gets (bfr);
    enterPressed = TRUE;
}



/*************************************************************************
*
* waitForSpeaker - waits for a speaker to be connected
*
* RETURNS: OK if speaker connected, else ERROR
*/


STATUS waitForSpeaker (void)
    {
    THREAD_HANDLE thread;


    /* Create thread to watch for keypress */

    enterPressed = FALSE;

    if (OSS_THREAD_CREATE (enterThread, 
			   (pVOID) NULL, 
			   OSS_PRIORITY_INHERIT, 
			   "tEnter",
			   &thread) 
			 != OK)
	{
	printf ("Error creating tEnter task.\n");
	return ERROR;
	}


    /* Wait for a speaker to be attached. */

    if (pSpkrSeqDev == NULL)
	{
	printf ("Waiting for speaker to be attached...\n");
	printf ("press enter to stop polling...\n");
	while (!enterPressed && pSpkrSeqDev == NULL)
		OSS_THREAD_SLEEP (1);
	}


    /* kill keypress thread */

    OSS_THREAD_DESTROY (thread);

    if (enterPressed)
	return ERROR;


    return OK;

    }



/***************************************************************************
*
* parseWavFile - parses and displays info about a .wav file
*
* Attempts to play the .wav file.
*
* NOTE: If this function returns TRUE, the caller SHOULD NOT close the
* wavFile.  That will be done automatically when playing is finished.
* 
* RETURNS: OK if able to play file, else ERROR.
*/


LOCAL STATUS parseWavFile 
	(
	FILE *wavFile
	)

    {
    RIFF_HDR riffHdr;
    char wavSig [RIFF_WAV_DATA_SIG_LEN];
    RIFF_CHUNK_HDR chunkHdr;
    WAV_FORMAT_CHUNK fmtChunk;
    UINT32 fileLen;
    UINT32 chunkLen;
    int i;

    USB_SPKR_AUDIO_FORMAT fmt;
    THREAD_HANDLE thread;

    /* Check the RIFF/WAV header. */

    if (fseek (wavFile, 0L, SEEK_SET) != 0) 
	{
	printf ("Cannot seek to beginning of file.\n");
	return ERROR;
	}

    if (fread (&riffHdr, 1, sizeof (riffHdr), wavFile) < sizeof (riffHdr) ||
	fread (&wavSig, 1, sizeof (wavSig), wavFile) < sizeof (wavSig)) 
	{
	printf ("Unexpected end of file reading RIFF header.\n");
	return ERROR;
	}

    if (memcmp (&riffHdr.signature, RIFF_HDR_SIG, RIFF_HDR_SIG_LEN) != 0 ||
	memcmp (wavSig, RIFF_WAV_DATA_SIG, RIFF_WAV_DATA_SIG_LEN) != 0) 
	{
	printf ("Not a .wav file.\n");
	return ERROR;
	}


    /* Read and display known chunks */

    fileLen = FROM_LITTLEL (riffHdr.length) + sizeof (riffHdr);
    /*  printf (".wav file size = %lu bytes.\n", (unsigned long) fileLen);*/

    while ((UINT32) ftell (wavFile) < fileLen)
	{
	/*printf ("\n");*/

	/* Read the next chunk header. */

	if (fread (&chunkHdr, 1, sizeof (chunkHdr), wavFile) < sizeof (chunkHdr))
	    {
	    printf ("Unexpected end of file reading chunk header.\n");
	    return ERROR;
	    }

	/*printf ("ChunkId = ");*/
	for (i = 0; i < RIFF_CHUNK_ID_LEN; i++)
		/*printf ("%c", chunkHdr.chunkId [i]);*/ /*REMOVE THIS ->*/;

	chunkLen = FROM_LITTLEL (chunkHdr.length);
	/*printf ("\nChunkLen = %lu\n", (unsigned long) chunkLen);*/


	/* If we recognize the chunkId, then display the chunk. */

	if (memcmp (chunkHdr.chunkId, RIFF_WAV_FMT_CHUNK_ID, RIFF_CHUNK_ID_LEN) == 0)
	    {
	    /* Read the format chunk. */

	    if (fread (&fmtChunk, 1, sizeof (fmtChunk), wavFile) < sizeof (fmtChunk))
		{
		printf ("Unexpected end of file reading format chunk.\n");
		return ERROR;
		}

	    if (fmtChunk.formatTag == WAV_FMT_MS_PCM)
		    /*printf ("bitsPerSample = %d\n", 
			    FROM_LITTLEW (fmtChunk.fmt.msPcm.bitsPerSample));*/ ;


	    /* Attempt to set the audio format to match */

	    if (FROM_LITTLEW (fmtChunk.formatTag) == WAV_FMT_MS_PCM)
		{
		memset (&fmt, 0, sizeof (fmt));

		fmt.formatTag = USB_AUDIO_TYPE1_PCM;
		fmt.formatType = USB_AUDIO_FORMAT_TYPE1;
		fmt.channels = FROM_LITTLEW (fmtChunk.channels);
		fmt.subFrameSize = FROM_LITTLEW (fmtChunk.blockAlign) / 
			FROM_LITTLEW (fmtChunk.channels);
		fmt.bitRes = FROM_LITTLEW (fmtChunk.fmt.msPcm.bitsPerSample);
		fmt.sampleFrequency = FROM_LITTLEL (fmtChunk.samplesPerSec);

		if (pSpkrSeqDev->sd_ioctl (pSpkrSeqDev, 
					   USB_SPKR_IOCTL_SET_AUDIO_FORMAT, 
					   (int) &fmt) 
					 == OK)
		    {
		    printf ("\nusbSpeakerLib format set successfully.\n");
		    }
		else
		    {
		    printf ("\nFailed to set usbSpeakerLib format.\n");
		    return ERROR;
		    }
		}
	    }
	else if (memcmp (chunkHdr.chunkId, RIFF_WAV_DATA_CHUNK_SIG, RIFF_CHUNK_ID_LEN) == 0)
	    {
	    /* data chunk found */

	    /* launch thread to dump audio data. */

	    wavDataLen = FROM_LITTLEL (chunkHdr.length);

	    if (OSS_THREAD_CREATE (audioThread, 
				   (pVOID) wavFile, 
				   OSS_PRIORITY_INHERIT, 
				   "tPlay", 
				   &thread) 
				 != OK)
		{
		printf ("Cannot create audio play thread.\n");
		return ERROR;
		}
	    else
		{
		audioThreadBusy = TRUE;
		return OK;
		}
	    }
	else
	    {
	    /* Skip over the chunk. */

	    fseek (wavFile, chunkLen, SEEK_CUR);
	    }
	}

    return ERROR;

    }


/*************************************************************************
*
* play - sends a .wav file to a speaker
*
* RETURNS:  OK or ERROR
*/

STATUS play (char * fileName)

    {
    FILE *f;

    /* Make sure usbSpeakerLib is initialized and a speaker is available */

    if (!spkrInitialized)
	{
	printf ("USB speaker SEQ_DEV driver not initialized.\n");
	return ERROR;
	}

    if (audioThreadBusy)
	{
	printf ("audioThread is busy.\n");
	return ERROR;
	}

    if (waitForSpeaker () != OK)
	return ERROR;


    /* Attempt to open the file. */

    if ((f = fopen (fileName, "rb")) == NULL) 
	{
	printf ("Unable to open '%s'.\n", fileName);
	return ERROR;
	}


    /* Parse the file */
    if (parseWavFile (f) != OK)
	fclose (f);


    return OK;

    }


/*************************************************************************
*
* stop - halts the playing of a wav
*
* RETURNS:  OK or ERROR
*/

STATUS stop (void)

    {


    /* Make sure usbSpeakerLib is initialized and a speaker is available */

    if (!spkrInitialized)
	{
	printf ("USB speaker SEQ_DEV driver not initialized.\n");
	return ERROR;
	}

    if (!audioThreadBusy)
	{
	printf ("No audio is being played.\n");
	return ERROR;
	}


    stopFlag = TRUE;

    return OK;

    }


/*************************************************************************
*
* vol - displays or sets speaker volume
*
* RETURNS:  OK or ERROR
*/

LOCAL STATUS vol
    (
    long volume			/* Generic parameter passed down */
    )

    {
    short volSetting;
    UINT16 channels;
    pUSB_SPKR_CHANNEL_CAPS pCaps;
    UINT16 i;


    /* verify a speaker is available */

    if (!spkrInitialized)
	{
	printf ("USB speaker SEQ_DEV driver not initialized.\n");
	return ERROR;
	}

    if (waitForSpeaker () != OK)
	return ERROR;


    /* Try to get channel information. */

    if ((*pSpkrSeqDev->sd_ioctl) (pSpkrSeqDev, 
				  USB_SPKR_IOCTL_GET_CHANNEL_COUNT, 
				  (int) 
				  &channels) 
				!= OK)
	{
	printf ("IOCTL GET_CHANNEL_COUNT returned ERROR.\n");
	return ERROR;
	}

    if ((*pSpkrSeqDev->sd_ioctl) (pSpkrSeqDev, 
				  USB_SPKR_IOCTL_GET_CHANNEL_CAPS, 
				  (int) 
				  &pCaps) 
				!= OK)
	{
	printf ("IOCTL GET_CHANNEL_CAPS returned ERROR.\n");
	return ERROR;
	}


    /* Get volume parameter (if specified). */

    /* If volume specified, then set it, else display current volume info */

    for (i = 0; i <= channels; i++)
	{
	printf ("Channel %d: ", i);

	if (!pCaps [i].volume.supported)
	    {
	    printf ("Volume not supported.\n");
	    }
	else
	    {
	    if (volume != -1)
		{
		/* Set volume */

		globalVolume = (long) volSetting = (short) (volume & 0xffff);

		if ((*pSpkrSeqDev->sd_ioctl) (pSpkrSeqDev, 
					      USB_SPKR_IOCTL_SET_VOLUME, 
					      (int) ((i << 16) | ((UINT16) volSetting))) 
					   == OK)
		    {
		    printf ("Volume set to %hx.\n", volSetting);
		    }
		else
		    {
		    printf ("Error setting volume.\n");
		    }
		}
	    else
		{
		/* Show volume settings. */

		printf ("res = %hx, min = %hx, max = %hx, cur = %hx.\n",
			pCaps [i].volume.res, pCaps [i].volume.min, 
			pCaps [i].volume.max, pCaps [i].volume.cur);
		}
	    }
	}		

    return OK;
    }

/*************************************************************************
*
* disVol - displays volume levels in hex
*
* RETURNS:  OK or ERROR
*/


STATUS disVol (void)
    {
    return (vol(-1));
    }
	
/*************************************************************************
*
* up - increases the volume
*
* RETURNS:  OK or ERROR
*/


STATUS up (void)
    {
    return (vol(globalVolume + 0x200));
    }

	
/*************************************************************************
*
* down - increases the volume
*
* RETURNS:  OK or ERROR
*/


STATUS down (void)
    {
 
    
    return (vol(globalVolume - 0x200));
    }

/*************************************************************************
*
* down - increases the volume
*
* RETURNS:  OK or ERROR
*/


STATUS mute (void)
    {
    return (vol(0xd000));
    }



/*************************************************************************
*
* usbSpkrInit - initializes USB speaker SEQ_DEV driver
*
* RETURNS: ERROR or OK
*/

UINT16 usbSpkrInit()
    {

    if (spkrInitialized)
	{
	printf ("USB speaker SEQ_DEV driver already initialized.\n");
	return ERROR;
	}


    /*  Initialize the speaker driver */

    if (usbSpeakerDevInit () == OK)
	{
	printf ("usbSpeakerDevInit() returned OK\n");
	spkrInitialized = TRUE;

	/* Register for attach notification */

	if (usbSpeakerDynamicAttachRegister ((USB_SPKR_ATTACH_CALLBACK)spkrAttachCallback, 
					     (pVOID)NULL) != OK)
	    {
	    printf ("usbSpeakerDynamicAttachRegister() returned ERROR\n");
	    return OK;
	    }

	}
    else
	printf ("usbSpeakerDevInit() returned ERROR\n");

    audioThreadBusy = FALSE;

    return OK;

    }




/*************************************************************************
*
* usrUsbAudioDemo - Entry point to USB audio demo
*
* RETURNS:  OK or ERROR
*/

extern STATUS usrUsbAudioDemo (void)
    {
    char * directoryName;
    
    
    /* Initialize the speaker class driver */
    
    usbSpkrInit();


    directoryName = "/ata0";

    /* Initialize the storage device */

    if (usrAtaConfig(0, 0, directoryName) != OK)
	{
	printf ("ATA configuration failed.\n");
	return ERROR;
	}
    

    return OK;
    }



