/* pccardLib.h - PCMCIA network interface header*/ 

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,30apr02,pmr  SPR 76487: fixed configType comment.
01c,11dec97,hdn  added TFFS support for flash PC card.
01b,22feb96,hdn  cleaned up.
01a,19oct95,hdn  written. 
*/


#ifndef __INCpccardLibh
#define __INCpccardLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


#define ERROR_FIND	-2	/* return code from Enabler */
#define PCCARD_3V	3	/* 3V for Vcc */
#define PCCARD_5V	5	/* 5V for Vcc or Vpp */
#define PCCARD_12V	12	/* 12V for Vpp */
#define PCCARD_IOWIN0	0	/* IO window 0 */
#define PCCARD_IOWIN1	1	/* IO window 1 */

#define PCCARD_FLASH	4	/* flash memory PC card */


typedef struct pccardEnabler
    {
    int		type;		/* type of a PC Card */
    void	*pResource;	/* pointer to PC card resources */
    int		resourceNumEnt;	/* number of PC card resource elements */
    FUNCPTR	enableRtn;	/* enable routine */
    FUNCPTR	showRtn;	/* show routine */
    } PCCARD_ENABLER;

typedef struct pccardResource
    {
    int		vcc;		/* 3,5:  Vcc */
    int		vpp;		/* 5,12: Vpp */
    int		ioStart[2];	/* start IO address */
    int		ioStop[2];	/* stop IO address */
    int		ioExtraws;	/* 0-2: extra wait state */
    int		memStart;	/* start host address */
    int		memStop;	/* stop host address */
    int		memExtraws;	/* 0-2: extra wait state */
    int		memOffset;	/* offset card address */
    int		memLength;	/* size of the memory */
    } PCCARD_RESOURCE;

typedef struct ataResource	/* PCCARD ATA resources */
    {
    PCCARD_RESOURCE resource;	/* must be the first member */
    int		ctrlType;	/* controller type: IDE_LOCAL or ATA_PCMCIA */
    int		drives;		/* 1,2: number of drives */
    int		intVector;	/* interrupt vector */
    int		intLevel;	/* IRQ level */
    int		configType;	/* or'd configuration flags */
    int		semTimeout;	/* timeout seconds for sync semaphore */
    int		wdgTimeout;	/* timeout seconds for watch dog */
    int		sockTwin;	/* socket number for twin card */
    int		pwrdown;	/* power down mode */
    } ATA_RESOURCE;

typedef struct sramResource	/* PCCARD SRAM resources */
    {
    PCCARD_RESOURCE resource;	/* must be the first member */
    } SRAM_RESOURCE;

typedef struct eltResource	/* PCCARD ELT resources */
    {
    PCCARD_RESOURCE resource;	/* must be the first member */
    int		intVector;	/* interrupt vector */
    int		intLevel;	/* IRQ level */
    int		rxFrames;	/* # of receive frames (0=default) */
    int		connector;	/* Ethernet connector to use */
    } ELT_RESOURCE;

typedef struct tffsResource	/* PCCARD TFFS resources */
    {
    PCCARD_RESOURCE resource;	/* must be the first member */
    } TFFS_RESOURCE;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	pccardMount		(int sock, char *pName);
extern STATUS	pccardMkfs		(int sock, char *pName);
extern STATUS	pccardEltEnabler	(int sock, ELT_RESOURCE *pEltResource,
			 		 int numEnt, FUNCPTR showRtn);
extern STATUS	pccardAtaEnabler	(int sock, ATA_RESOURCE *pAtaResource,
			 		 int numEnt, FUNCPTR showRtn);
extern STATUS	pccardSramEnabler	(int sock, SRAM_RESOURCE *pSramResource,
			 		 int numEnt, FUNCPTR showRtn);
extern STATUS	pccardTffsEnabler	(int sock, TFFS_RESOURCE *pTffsResource,
			 		 int numEnt, FUNCPTR showRtn);
extern void	pccardShowInit		(void);

#else

extern STATUS	pccardMount		();
extern STATUS	pccardMkfs		();
extern STATUS	pccardEltEnabler	();
extern STATUS	pccardAtaEnabler	();
extern STATUS	pccardSramEnabler	();
extern STATUS	pccardTffsEnabler	();
extern void	pccardShowInit		();

#endif  /* __STDC__ */


#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif  /* __INCpccardLibh */
