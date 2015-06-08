/* sysTempeShow.c - Show Tempe registers */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01d,02apr04,scb Formatting nit so sysTempeWinShow() is fixed.
01c,18mar04,scb Don't display VIACKx in sysTempeShow(), only display enabled
		windows in sysTempeWinShow().
01b,27feb04,scb Fix diab compiler warnings.
01a,03sep03,scb Initial writing.
*/

/*
DESCRIPTION

This file contains the "show routine" support for the Tempe chip.
*/

/* includes */

#include "tempe.h"

/* defines */

#define tempeRegPrint(reg)\
    { \
    printf("%-25s  @ 0x%08X = 0x%08X\n",#reg,\
    reg+TEMPE_REG_BASE,(UINT32)(TEMPE_READ32(TEMPE_REG_BASE,reg)));\
    }

/* typedefs */

/* globals */

/* locals */

/* externals */

void getOutWinVals (UINT32, VME_OUT_MAP *); 
void getInWinVals (UINT32, VME_IN_MAP *); 
IMPORT UINT32 TEMPE_REG_BASE; 	/* Address of Tempe 4K CGR space */

/* forward declarations */

void sysTempeShow (void);
void sysTempeOutWinShow (UINT32);
void sysTempeInWinShow (UINT32);
void sysTempeWinShow (void);

/******************************************************************************
*
* sysTempeShow - Show the registers in the Tempe PCI-VME bridge chip
*
* This routine displays every register in the Tempe PCI-VME bridge chip
*
* RETURNS: NA
*/

void sysTempeShow (void)
    {
    printf("\nPCI Configuration: Header\n\n");

    tempeRegPrint (TEMPE_DEVI_VENI);
    tempeRegPrint (TEMPE_STAT_CMMD);
    tempeRegPrint (TEMPE_CLAS_REVI);
    tempeRegPrint (TEMPE_HEAD_MLAT_CLSZ);
    tempeRegPrint (TEMPE_MBARL);
    tempeRegPrint (TEMPE_MBARU);
    tempeRegPrint (TEMPE_SUBI_SUBV);
    tempeRegPrint (TEMPE_CAPP);
    tempeRegPrint (TEMPE_MXLA_MNGN_INTP_INTL);

    printf("\nPCI Configuration: PCI-X Capabilities\n\n");

    tempeRegPrint (TEMPE_PCIXCAP);
    tempeRegPrint (TEMPE_PCIXSTAT);

    printf("\nOutbound Functions: Outbound Translation 0\n\n");

    tempeRegPrint (TEMPE_OTSAU0);
    tempeRegPrint (TEMPE_OTSAL0);
    tempeRegPrint (TEMPE_OTEAU0);
    tempeRegPrint (TEMPE_OTEAL0);
    tempeRegPrint (TEMPE_OTOFU0);
    tempeRegPrint (TEMPE_OTOFL0);
    tempeRegPrint (TEMPE_OTBS0);
    tempeRegPrint (TEMPE_OTAT0);

    printf("\nOutbound Functions: Outbound Translation 1\n\n");

    tempeRegPrint (TEMPE_OTSAU1);
    tempeRegPrint (TEMPE_OTSAL1);
    tempeRegPrint (TEMPE_OTEAU1);
    tempeRegPrint (TEMPE_OTEAL1);
    tempeRegPrint (TEMPE_OTOFU1);
    tempeRegPrint (TEMPE_OTOFL1);
    tempeRegPrint (TEMPE_OTBS1);
    tempeRegPrint (TEMPE_OTAT1);

    printf("\nOutbound Functions: Outbound Translation 2\n\n");

    tempeRegPrint (TEMPE_OTSAU2);
    tempeRegPrint (TEMPE_OTSAL2);
    tempeRegPrint (TEMPE_OTEAU2);
    tempeRegPrint (TEMPE_OTEAL2);
    tempeRegPrint (TEMPE_OTOFU2);
    tempeRegPrint (TEMPE_OTOFL2);
    tempeRegPrint (TEMPE_OTBS2);
    tempeRegPrint (TEMPE_OTAT2);

    printf("\nOutbound Functions: Outbound Translation 3\n\n");

    tempeRegPrint (TEMPE_OTSAU3);
    tempeRegPrint (TEMPE_OTSAL3);
    tempeRegPrint (TEMPE_OTEAU3);
    tempeRegPrint (TEMPE_OTEAL3);
    tempeRegPrint (TEMPE_OTOFU3);
    tempeRegPrint (TEMPE_OTOFL3);
    tempeRegPrint (TEMPE_OTBS3);
    tempeRegPrint (TEMPE_OTAT3);

    printf("\nOutbound Functions: Outbound Translation 4\n\n");

    tempeRegPrint (TEMPE_OTSAU4);
    tempeRegPrint (TEMPE_OTSAL4);
    tempeRegPrint (TEMPE_OTEAU4);
    tempeRegPrint (TEMPE_OTEAL4);
    tempeRegPrint (TEMPE_OTOFU4);
    tempeRegPrint (TEMPE_OTOFL4);
    tempeRegPrint (TEMPE_OTBS4);
    tempeRegPrint (TEMPE_OTAT4);

    printf("\nOutbound Functions: Outbound Translation 5\n\n");

    tempeRegPrint (TEMPE_OTSAU5);
    tempeRegPrint (TEMPE_OTSAL5);
    tempeRegPrint (TEMPE_OTEAU5);
    tempeRegPrint (TEMPE_OTEAL5);
    tempeRegPrint (TEMPE_OTOFU5);
    tempeRegPrint (TEMPE_OTOFL5);
    tempeRegPrint (TEMPE_OTBS5);
    tempeRegPrint (TEMPE_OTAT5);

    printf("\nOutbound Functions: Outbound Translation 6\n\n");

    tempeRegPrint (TEMPE_OTSAU6);
    tempeRegPrint (TEMPE_OTSAL6);
    tempeRegPrint (TEMPE_OTEAU6);
    tempeRegPrint (TEMPE_OTEAL6);
    tempeRegPrint (TEMPE_OTOFU6);
    tempeRegPrint (TEMPE_OTOFL6);
    tempeRegPrint (TEMPE_OTBS6);
    tempeRegPrint (TEMPE_OTAT6);

    printf("\nOutbound Functions: Outbound Translation 7\n\n");

    tempeRegPrint (TEMPE_OTSAU7);
    tempeRegPrint (TEMPE_OTSAL7);
    tempeRegPrint (TEMPE_OTEAU7);
    tempeRegPrint (TEMPE_OTEAL7);
    tempeRegPrint (TEMPE_OTOFU7);
    tempeRegPrint (TEMPE_OTOFL7);
    tempeRegPrint (TEMPE_OTBS7);
    tempeRegPrint (TEMPE_OTAT7);

#if 0
    /*
     * The TEMPE_VIACKx registers, if read for display will
     * throw a VME bus exception which appears in the TEMPE_VEAT
     * register.  This could be undesireable, so these registers
     * are not printed here.  Change the "#if 0" to "if 1" if
     * you want these register in the display but be aware that
     * you will corrupt the TEMPE_VEAT register if you do this.
     */

    printf("\nOutbound Functions: VMEbus Interrupt Acknowledge\n\n");

    tempeRegPrint (TEMPE_VIACK1);
    tempeRegPrint (TEMPE_VIACK2);
    tempeRegPrint (TEMPE_VIACK3);
    tempeRegPrint (TEMPE_VIACK4);
    tempeRegPrint (TEMPE_VIACK5);
    tempeRegPrint (TEMPE_VIACK6);
    tempeRegPrint (TEMPE_VIACK7);
#endif

    printf("\nOutbound Functions: RMW\n\n");

    tempeRegPrint (TEMPE_RMWAU);
    tempeRegPrint (TEMPE_RMWAL);
    tempeRegPrint (TEMPE_RMWEN);
    tempeRegPrint (TEMPE_RMWC);
    tempeRegPrint (TEMPE_RMWS);

    printf("\nOutbound Functions: VMEbus Control\n\n");

    tempeRegPrint (TEMPE_VMCTRL);
    tempeRegPrint (TEMPE_VCTRL);
    tempeRegPrint (TEMPE_VSTAT);

    printf("\nOutbound Functions: PCI control/status\n\n");

    tempeRegPrint (TEMPE_PSTAT);

    printf("\nVME Filters: VME filter\n\n");

    tempeRegPrint (TEMPE_VMEFL);

    printf("\nVME Exception: VME Exception Status\n\n");

    tempeRegPrint (TEMPE_VEAU);
    tempeRegPrint (TEMPE_VEAL);
    tempeRegPrint (TEMPE_VEAT);

    printf("\nPCI Error: PCI Error Status\n\n");

    tempeRegPrint (TEMPE_EDPAU);
    tempeRegPrint (TEMPE_EDPAL);
    tempeRegPrint (TEMPE_EDPXA);
    tempeRegPrint (TEMPE_EDPXS);
    tempeRegPrint (TEMPE_EDPAT);

    printf("\nInbound Functions: Inbound Translation 0\n\n");

    tempeRegPrint (TEMPE_ITSAU0);
    tempeRegPrint (TEMPE_ITSAL0);
    tempeRegPrint (TEMPE_ITEAU0);
    tempeRegPrint (TEMPE_ITEAL0);
    tempeRegPrint (TEMPE_ITOFU0);
    tempeRegPrint (TEMPE_ITOFL0);
    tempeRegPrint (TEMPE_ITAT0);

    printf("\nInbound Functions: Inbound Translation 1\n\n");

    tempeRegPrint (TEMPE_ITSAU1);
    tempeRegPrint (TEMPE_ITSAL1);
    tempeRegPrint (TEMPE_ITEAU1);
    tempeRegPrint (TEMPE_ITEAL1);
    tempeRegPrint (TEMPE_ITOFU1);
    tempeRegPrint (TEMPE_ITOFL1);
    tempeRegPrint (TEMPE_ITAT1);

    printf("\nInbound Functions: Inbound Translation 2\n\n");

    tempeRegPrint (TEMPE_ITSAU2);
    tempeRegPrint (TEMPE_ITSAL2);
    tempeRegPrint (TEMPE_ITEAU2);
    tempeRegPrint (TEMPE_ITEAL2);
    tempeRegPrint (TEMPE_ITOFU2);
    tempeRegPrint (TEMPE_ITOFL2);
    tempeRegPrint (TEMPE_ITAT2);

    printf("\nInbound Functions: Inbound Translation 3\n\n");

    tempeRegPrint (TEMPE_ITSAU3);
    tempeRegPrint (TEMPE_ITSAL3);
    tempeRegPrint (TEMPE_ITEAU3);
    tempeRegPrint (TEMPE_ITEAL3);
    tempeRegPrint (TEMPE_ITOFU3);
    tempeRegPrint (TEMPE_ITOFL3);
    tempeRegPrint (TEMPE_ITAT3);

    printf("\nInbound Functions: Inbound Translation 4\n\n");

    tempeRegPrint (TEMPE_ITSAU4);
    tempeRegPrint (TEMPE_ITSAL4);
    tempeRegPrint (TEMPE_ITEAU4);
    tempeRegPrint (TEMPE_ITEAL4);
    tempeRegPrint (TEMPE_ITOFU4);
    tempeRegPrint (TEMPE_ITOFL4);
    tempeRegPrint (TEMPE_ITAT4);

    printf("\nInbound Functions: Inbound Translation 5\n\n");

    tempeRegPrint (TEMPE_ITSAU5);
    tempeRegPrint (TEMPE_ITSAL5);
    tempeRegPrint (TEMPE_ITEAU5);
    tempeRegPrint (TEMPE_ITEAL5);
    tempeRegPrint (TEMPE_ITOFU5);
    tempeRegPrint (TEMPE_ITOFL5);
    tempeRegPrint (TEMPE_ITAT5);

    printf("\nInbound Functions: Inbound Translation 6\n\n");

    tempeRegPrint (TEMPE_ITSAU6);
    tempeRegPrint (TEMPE_ITSAL6);
    tempeRegPrint (TEMPE_ITEAU6);
    tempeRegPrint (TEMPE_ITEAL6);
    tempeRegPrint (TEMPE_ITOFU6);
    tempeRegPrint (TEMPE_ITOFL6);
    tempeRegPrint (TEMPE_ITAT6);

    printf("\nInbound Functions: Inbound Translation 7\n\n");

    tempeRegPrint (TEMPE_ITSAU7);
    tempeRegPrint (TEMPE_ITSAL7);
    tempeRegPrint (TEMPE_ITEAU7);
    tempeRegPrint (TEMPE_ITEAL7);
    tempeRegPrint (TEMPE_ITOFU7);
    tempeRegPrint (TEMPE_ITOFL7);
    tempeRegPrint (TEMPE_ITAT7);

    printf("\nInbound Functions: Inbound Translation GCSR\n\n");

    tempeRegPrint (TEMPE_GBAU);
    tempeRegPrint (TEMPE_GBAL);
    tempeRegPrint (TEMPE_GCSRAT);

    printf("\nInbound Functions: Inbound Translation CRG\n\n");

    tempeRegPrint (TEMPE_CBAU);
    tempeRegPrint (TEMPE_CBAL);
    tempeRegPrint (TEMPE_CSRAT);

    printf("\nInbound Functions: Inbound Translation CR/CSR\n\n");

    tempeRegPrint (TEMPE_CROU);
    tempeRegPrint (TEMPE_CROL);
    tempeRegPrint (TEMPE_CRAT);

    printf("\nInbound Functions: Inbound Translation Location Monitor\n\n");

    tempeRegPrint (TEMPE_LMBAU);
    tempeRegPrint (TEMPE_LMBAL);
    tempeRegPrint (TEMPE_LMAT);

    printf("\nInterrupt Functions: VMEbus Interrupt Control\n\n");

    tempeRegPrint (TEMPE_64BCU);
    tempeRegPrint (TEMPE_64BCL);
    tempeRegPrint (TEMPE_BPGTR);
    tempeRegPrint (TEMPE_BPCTR);
    tempeRegPrint (TEMPE_VICR);

    printf("\nInterrupt Functions: Local Bus Interrupt Control\n\n");

    tempeRegPrint (TEMPE_INTEN);
    tempeRegPrint (TEMPE_INTEO);
    tempeRegPrint (TEMPE_INTS);
    tempeRegPrint (TEMPE_INTC);
    tempeRegPrint (TEMPE_INTM1);
    tempeRegPrint (TEMPE_INTM2);

    printf("\nDMA Functions: DMA Controller 0\n\n");

    tempeRegPrint (TEMPE_DCTL0);
    tempeRegPrint (TEMPE_DSTA0);
    tempeRegPrint (TEMPE_DCSAU0);
    tempeRegPrint (TEMPE_DCSAL0);
    tempeRegPrint (TEMPE_DCDAU0);
    tempeRegPrint (TEMPE_DCDAL0);
    tempeRegPrint (TEMPE_DCLAU0);
    tempeRegPrint (TEMPE_DCLAL0);
    tempeRegPrint (TEMPE_DSAU0);
    tempeRegPrint (TEMPE_DSAL0);
    tempeRegPrint (TEMPE_DDAU0);
    tempeRegPrint (TEMPE_DDAL0);
    tempeRegPrint (TEMPE_DSAT0);
    tempeRegPrint (TEMPE_DDAT0);
    tempeRegPrint (TEMPE_DNLAU0);
    tempeRegPrint (TEMPE_DNLAL0);
    tempeRegPrint (TEMPE_DCNT0);
    tempeRegPrint (TEMPE_DDBS0);

    printf("\nDMA Functions: DMA Controller 1\n\n");

    tempeRegPrint (TEMPE_DCTL1);
    tempeRegPrint (TEMPE_DSTA1);
    tempeRegPrint (TEMPE_DCSAU1);
    tempeRegPrint (TEMPE_DCSAL1);
    tempeRegPrint (TEMPE_DCDAU1);
    tempeRegPrint (TEMPE_DCDAL1);
    tempeRegPrint (TEMPE_DCLAU1);
    tempeRegPrint (TEMPE_DCLAL1);
    tempeRegPrint (TEMPE_DSAU1);
    tempeRegPrint (TEMPE_DSAL1);
    tempeRegPrint (TEMPE_DDAU1);
    tempeRegPrint (TEMPE_DDAL1);
    tempeRegPrint (TEMPE_DSAT1);
    tempeRegPrint (TEMPE_DDAT1);
    tempeRegPrint (TEMPE_DNLAU1);
    tempeRegPrint (TEMPE_DNLAL1);
    tempeRegPrint (TEMPE_DCNT1);
    tempeRegPrint (TEMPE_DDBS1);

    printf("\nGCSR register group\n\n");

    tempeRegPrint (TEMPE_GCSR_DEVI_VENI);
    tempeRegPrint (TEMPE_GCSR_STAT_CMMD);
    tempeRegPrint (TEMPE_GCSR_SRMAR0_3);
    tempeRegPrint (TEMPE_GCSR_SRMAR4_7);
    tempeRegPrint (TEMPE_GCSR_MBOX0);
    tempeRegPrint (TEMPE_GCSR_MBOX1);
    tempeRegPrint (TEMPE_GCSR_MBOX2);
    tempeRegPrint (TEMPE_GCSR_MBOX3);

    printf("\nCR/CSR register group\n\n");

    tempeRegPrint (TEMPE_CSR_CSRBCR);
    tempeRegPrint (TEMPE_CSR_CSRBSR);
    tempeRegPrint (TEMPE_CSR_CBAR);

    }

/******************************************************************************
*
* sysOutWinShow - Show the Tempe outbound settings for one mapping window.
*
* This routine displays in a human readable format, the mapping for one
* of the Tempe outbound window mapping register sets.
*
* RETURNS: NA
*/

void sysTempeOutWinShow 
    (
    UINT32 winSet	/* Window set number to display */
    )
    {
    VME_OUT_MAP v;
    UINT32 pciBottomU;
    UINT32 pciBottomL;
    UINT32 pciTopU;
    UINT32 pciTopL;
    UINT32 vmeBottomU; 
    UINT32 vmeBottomL;
    UINT32 vmeTopU; 
    UINT32 vmeTopL;
    UINT32 otbsx;
    UINT32 otatx;
    UINT32 sizeU;
    UINT32 sizeL;
    UINT64 val64;

    if (winSet > TEMPE_OUTBOUND_WINDOW_COUNT)
	return;

    getOutWinVals (winSet, &v);

    pciBottomU = v.otsauImg;
    pciBottomL = v.otsalImg;
    pciTopU    = v.oteauImg;
    pciTopL    = v.otealImg | 0x0000ffff;
    otbsx      = v.otbsImg;
    otatx      = v.otatImg;

    /* Compute the VME "bottom" address set */

    val64 = MAKE64(pciBottomU, pciBottomL) +
	    MAKE64(v.otofuImg, v.otoflImg);
    UNMAKE64(val64, vmeBottomU, vmeBottomL);

    /* Compute the VME "top" address set */

    val64 = MAKE64(pciTopU, pciTopL) +
	    MAKE64(v.otofuImg, v.otoflImg);
    UNMAKE64(val64, vmeTopU, vmeTopL);

    /* Compute the size of the window */

    val64 = MAKE64(pciTopU, pciTopL) -
	    MAKE64(pciBottomU, pciBottomL) + (UINT64)(1);
    UNMAKE64(val64, sizeU, sizeL);

    printf("\n");
    printf ("Outbound window %d: ",winSet);

    if (otatx & (1<<31))
	printf("ENABLED,");
    else
	printf("NOT ENABLED,");

    printf("  Size = %08x_%08x\n",sizeU, sizeL);
    printf("\n");

    printf ("raw: %08x %08x %08x %08x %08x %08x %08x %08x\n",
             v.otsauImg, v.otsalImg, v.oteauImg, v.otealImg,
             v.otofuImg, v.otoflImg, v.otbsImg, v.otatImg);

    printf("PCI Base:  %08x_%08x    VME Base:  %08x_%08x\n",
		    pciBottomU, pciBottomL, vmeBottomU, vmeBottomL);
    printf("PCI Limit: %08x_%08x    VME Limit: %08x_%08x\n",
		    pciTopU, pciTopL, vmeTopU, vmeTopL);

    printf("     2eSST Broadcast Select: 0x%05x\n", otbsx);
    printf("     Attributes:\n");
    
    printf("        Memory Prefetch      - ");
    if (otatx & (1<<18))
	printf("disabled\n");
    else
	printf("enabled\n");

    printf("        Prefetch size        - ");
    switch ((otatx >> 16) & 0x3)
	{
	case VME_RD_PREFETCH_2_CACHE_LINES:
	    printf("2 cache lines\n");
	    break;
	case VME_RD_PREFETCH_4_CACHE_LINES:
	    printf("4 cache lines\n");
	    break;
	case VME_RD_PREFETCH_8_CACHE_LINES:
	    printf("8 cache lines\n");
	    break;
	case VME_RD_PREFETCH_16_CACHE_LINES:
	    printf("16 cache lines\n");
	    break;
        }

    printf("        2eSST Mode           - ");
    switch ((otatx >> 11) & 0x7)
	{
	case VME_SST160:
	    printf("160 MB/s\n");
	    break;
	case VME_SST267:
	    printf("267 MB/s\n");
	    break;
	case VME_SST320:
	    printf("320 MB/s\n");
	    break;
        default:
	    printf("UNKNOWN (reserved value)\n");
	}

    printf("        Transfer Mode        - ");
    switch ((otatx >> 8) & 0x7)
	{
        case VME_SCT_OUT:
	    printf("SCT (Single Cycle Transfer)\n");
	    break;
        case VME_BLT_OUT:
	    printf("BLT (Block Transfer)\n");
	    break;
        case VME_MBLT_OUT:
	    printf("MBLT (Multiplexed Block Transfer)\n");
	    break;
        case VME_2eVME_OUT:
	    printf("2eVME (Two Edge VMEbus)\n");
	    break;
        case VME_2eSST_OUT:
	    printf("2eSST (Two Edge Source Synchronous)\n");
	    break;
        case VME_2eSST_BCAST_OUT:
	    printf("2eSSTB (Two Edge Source Synchronous Broadcast)\n");
	    break;
        default:
	    printf("UNKNOWN (reserved value)\n");
	}

    printf("        VME Data Bus Width   - ");
    switch ((otatx >> 6) & 0x3)
        {
	case VME_D16: 
	    printf("16 bit\n");
	    break;
	case VME_D32: 
	    printf("32 bit\n");
	    break;
        default:
	    printf("UNKNOWN (reserved value)\n");
        }

    printf("        VME AM code          - ");
    if ((otatx & (1 << 5)) != 0)
	printf("Supervisor");
    else
        printf("nonSupervisor");
    if ((otatx & (1 << 4)) != 0)
	printf(", Program\n");
    else
        printf(", nonProgram\n");

    printf("        VME Address mode     - ");

    switch ((otatx >> 0) & 0xf)
	{
        case VME_MODE_A16:
	    printf("A16\n");
	    break;
        case VME_MODE_A24:
	    printf("A24\n");
	    break;
        case VME_MODE_A32:
	    printf("A32\n");
	    break;
        case VME_MODE_A64:
	    printf("A64\n");
	    break;
        case VME_MODE_CRCSR:
	    printf("CR/CSR\n");
	    break;
        case VME_MODE_USER1:
	    printf("User1 (AM %c0100xx)\n",'%');
	    break;
        case VME_MODE_USER2:
	    printf("User2 (AM %c0101xx)\n",'%');
	    break;
        case VME_MODE_USER3:
	    printf("User3 (AM %c0110xx)\n",'%');
	    break;
        case VME_MODE_USER4:
	    printf("User4 (AM %c0111xx)\n",'%');
	    break;
	}
    printf("\n");
    }

/******************************************************************************
*
* sysInWinShow - Show the Tempe inbound settings for one mapping window.
*
* This routine displays in a human readable format, the mapping for one
* of the Tempe inbound window mapping register sets.
*
* RETURNS: NA
*/

void sysTempeInWinShow 
    (
    UINT32 winSet	/* Window set number to display */
    )
    {
    VME_IN_MAP v;
    UINT32 pciBottomU;
    UINT32 pciBottomL;
    UINT32 pciTopU;
    UINT32 pciTopL;
    UINT32 vmeBottomU; 
    UINT32 vmeBottomL;
    UINT32 vmeTopU; 
    UINT32 vmeTopL;
    UINT32 itatx;
    UINT32 sizeU;
    UINT32 sizeL;
    UINT32 hold;
    UINT64 val64;

    if (winSet > TEMPE_INBOUND_WINDOW_COUNT)
	return;

    getInWinVals (winSet, &v);

    vmeBottomU = v.itsauImg;
    vmeBottomL = v.itsalImg;
    vmeTopU    = v.iteauImg;
    vmeTopL    = v.itealImg | 0x0000000f;
    itatx      = v.itatImg;

    /* Compute the PCI bottom address set */

    val64 = MAKE64(vmeBottomU, vmeBottomL) +
	    MAKE64(v.itofuImg, v.itoflImg);
    UNMAKE64(val64, pciBottomU, pciBottomL);

    /* Compute the PCI top address set */

    val64 = MAKE64(vmeTopU, vmeTopL) +
	    MAKE64(v.itofuImg, v.itoflImg);
    UNMAKE64(val64, pciTopU, pciTopL);

    /* Compute the size of the window */

    val64 = MAKE64(vmeTopU, vmeTopL) -
	    MAKE64(vmeBottomU, vmeBottomL) + (UINT64)(1);
    UNMAKE64(val64, sizeU, sizeL);

    printf("\n");
    printf ("Inbound window %d: ",winSet);

    if (itatx & (1<<31))
	printf("ENABLED,");
    else
	printf("NOT ENABLED,");

    printf("  Size = %08x_%08x\n",sizeU, sizeL);
    printf("\n");

    printf ("raw: %08x %08x %08x %08x %08x %08x %08x\n",
	    v.itsauImg, v.itsalImg, v.iteauImg, v.itealImg,
            v.itofuImg, v.itoflImg, v.itatImg);

    printf("VME Base:  %08x_%08x    PCI Base:  %08x_%08x\n",
		    vmeBottomU, vmeBottomL, pciBottomU, pciBottomL);
    printf("VME Limit: %08x_%08x    PCI Limit: %08x_%08x\n",
		    vmeTopU, vmeTopL, pciTopU, pciTopL);

    printf("     Attributes:\n");
    
    printf("        Read-ahead threshold - ");
    if (itatx & (1<<18))
	printf("when FIFO half empty\n");
    else
	printf("when FIFO competely empty\n");

    printf("        Virtual FIFO size    - ");
    switch ((itatx >> 16) & 0x3)
	{
	case VME_VFS_FIFO_64:
	    printf("64 bytes\n");
	    break;
	case VME_VFS_FIFO_128:
	    printf("128 bytes\n");
	    break;
	case VME_VFS_FIFO_256:
	    printf("256 bytes\n");
	    break;
	case VME_VFS_FIFO_512:
	    printf("512 bytes\n");
	    break;
        }

    printf("        2eSST Mode           - ");
    switch ((itatx >> 12) & 0x7)
	{
	case VME_SST160:
	    printf("160 MB/s\n");
	    break;
	case VME_SST267:
	    printf("267, 160 MB/s\n");
	    break;
	case VME_SST320:
	    printf("320, 267, 160 MB/s\n");
	    break;
        default:
	    printf("UNKNOWN (reserved value)\n");
	}

    printf("        Cycle response       - ");
    if (((itatx >> 7) & 0x1f) == 0)
	printf("none\n");
    hold = 0;
    if ((itatx & (1 << 11)) != 0)
	{
	printf("2eSSTB (Two Edge Source Synchronous Broadcast)\n");
	hold++;
	}
    if ((itatx & (1 << 10)) != 0)
	{
	if (hold > 0)
            printf("                               ");
	printf("2eSST (Two Edge Source Synchronous nonBroadcast)\n");
	hold++;
	}
    if ((itatx & (1 << 9)) != 0)
	{
	if (hold > 0)
            printf("                               ");
	printf("2eVME (Two Edge VMEbus)\n");
	hold++;
	}
    if ((itatx & (1 << 8)) != 0)
	{
	if (hold > 0)
            printf("                               ");
	printf("MBLT (Multiplexed Block Transfer)\n");
	hold++;
	}
    if ((itatx & (1 << 7)) != 0)
	{
	if (hold > 0)
            printf("                               ");
	printf("BLT (Block Transfer)\n");
	}

    printf("        Address space        - ");
    switch ((itatx >> 4) & 0x7)
        {
	case VME_SPACE_A16: 
	    printf("A16\n");
	    break;
	case VME_SPACE_A24: 
	    printf("A24\n");
	    break;
	case VME_SPACE_A32: 
	    printf("A32\n");
	    break;
	case VME_SPACE_A64: 
	    printf("A64\n");
	    break;
        default:
	    printf("UNKNOWN (reserved value)\n");
        }

    printf("        VME AM response      - ");
    hold = 0;
    if ((itatx & (1 << 3)) != 0)
	{
	printf("Supervisor");
	hold++;
	}
    if ((itatx & (1 << 2)) != 0)
	{
	if (hold > 0)
	    printf(", ");
	printf("nonSupervisor");
	hold++;
	}
    if ((itatx & (1 << 1)) != 0)
	{
	if (hold > 0)
	    printf(", ");
	printf("Program");
	hold++;
	}
    if ((itatx & (1 << 0)) != 0)
	{
	if (hold > 0)
	    printf(", ");
	printf("Data");
	}
    if (hold == 0)
	printf("none");
    printf("\n");
    printf("\n");
    }

/******************************************************************************
*
* sysTempeWinShow - Show all Tempe address mapping windows.
*
* This routine displays in a human readable format, the mapping for each
* of the eight enabled outbound and inbound Tempe window mapping sets.  Only
* "enabled" windows are displayed.
*
* RETURNS: NA
*/

void sysTempeWinShow (void)
    {
    int i;
    VME_OUT_MAP vo;
    VME_IN_MAP  vi;

    for (i = 0; i < TEMPE_OUTBOUND_WINDOW_COUNT; i++)
	{
        getOutWinVals (i, &vo);
        if (vo.otatImg & (1<<31))	/* Enabled so show it */
	    sysTempeOutWinShow(i);
	else
	    printf("\n\nOutbound window %d: NOT ENABLED\n",i);
	}

    for (i = 0; i < TEMPE_INBOUND_WINDOW_COUNT; i++)
	{
	getInWinVals (i, &vi);
        if (vi.itatImg & (1<<31))	/* Enabled so show it */
	    sysTempeInWinShow(i);
	else
	    printf("\n\nInbound window %d: NOT ENABLED\n",i);
	}
    }
