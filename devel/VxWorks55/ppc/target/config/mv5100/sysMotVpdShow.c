/* sysMotVpdShow.c - Vital Product Data Show Routines. */

/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,05sep00,dmw  Restored comma in vpdPktPrintHex arguments.
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from version 01a of mcpn765/vpdShow.c).
*/

/*
DESCRIPTION
This file contains the Vital Product Data Show utility routines. To use these
routines, define INCLUDE_SHOW_ROUTINES in config.h.
*/

#include "vxWorks.h"
#include "config.h"
#include "stdio.h"
#include "sysMotVpd.h"

/* externals */

IMPORT STATUS sysVpdInit();
IMPORT VPD_PACKET * sysVpdPkts[]; /* board vpd packets */


/*******************************************************************************
*
* vpdPktPrintType - print the vpd packet type in ascii
*
* This routine prints the vpd packet type in ascii.
*
* RETURNS: N/A
*
*/

void vpdPktPrintType
    (
    UCHAR type /* packet type */
    )
    {
    UCHAR * p;

    /* setup the proper ascii string */

    switch (type)
        {
        case VPD_PID_PID:
            p = "Product Identifier";
            break;

        case VPD_PID_FAN:
            p = "Factory Assembly Number";
            break;

        case VPD_PID_SN:
            p = "Serial Number";
            break;

        case VPD_PID_PCO:
            p = "Product Configuration Options";
            break;

        case VPD_PID_ICS:
            p = "Internal Clock Speed (in HZ)";
            break;

        case VPD_PID_ECS:
            p = "External Clock Speed (in HZ)";
            break;

        case VPD_PID_RCS:
            p = "Reference Clock Speed (in HZ)";
            break;

        case VPD_PID_EA:
            p = "Ethernet Address";
            break;

        case VPD_PID_MT:
            p = "Microprocessor Type";
            break;

        case VPD_PID_CRC:
            p = "EEPROM CRC";
            break;

        case VPD_PID_FMC:
            p = "FLASH Memory Configuration";
            break;

        case VPD_PID_VLSI:
            p = "VLSI Revisions/Versions";
            break;

        case VPD_PID_PCS:
            p = "PCI  Clock Speed (in HZ)";
            break;

        case VPD_PID_L2:
            p = "L2 Cache Configuration";
            break;

        case VPD_PID_TERM:
            p = "Termination Marker";
            break;

        default:
            p = "Unknown";
            break;
        }

    /* print the string followed by the numeric packet code. */

    printf ("%s (packet type = 0x%02x)\n", p, type);
    }


/*******************************************************************************
*
* vpdPktPrintAscii - print the data area of an ascii vpd packet.
*
* This routine prints the data area of an ascii packet. the number of bytes
* printed is taken from the packet header.
*
* RETURNS: N/A
*
* SEE ALSO: vpdPktPrintDecimal(), vpdPktPrintHex() 
*/

void vpdPktPrintAscii
    (
    VPD_PACKET *pVpdPkt /* pointer to VPD packet */
    )
    {
    UINT32 idx;

    /* for each byte in the data area, print the current byte. */

    for (idx = 0; idx < pVpdPkt->size; idx++)
        printf ("%c", pVpdPkt->data[idx]);

    printf ("\n");

    }


/*******************************************************************************
*
* vpdPktPrintDecimal - print the data area of an decimal vpd packet.
*
* This routine prints the data area of an decimal packet. the data width (byte,
* half-word, or word) is taken from the packet header.
*
* RETURNS: N/A
*
* SEE ALSO: vpdPktPrintAscii(), vpdPktPrintHex() 
*/

void vpdPktPrintDecimal
    (
    VPD_PACKET *pVpdPkt /* pointer to VPD packet */
    )
    {

    /* print the data area in decimal depending on the data size. */

    switch (pVpdPkt->size)
        {
        case sizeof(UCHAR):
            printf ("%d", *(UCHAR *)&pVpdPkt->data[0]);
            break;

        case sizeof(UINT16):
            printf ("%d", *(UINT16 *)&pVpdPkt->data[0]);
            break;

        case sizeof(UINT32):
            printf ("%d", *(UINT32 *)&pVpdPkt->data[0]);
            break;

        /* if the data size is unknown, indicate a field size error. */

        default:
            printf ("********");
            break;

        }
    printf ("\n");

    }


/*******************************************************************************
*
* vpdPktPrintHex - print the data area of an binary vpd packet in hex.
*
* This routine prints the data area of an binary packet. the number of bytes
* printed is taken from the packet header. An input parameter is passed which
* controls the formatting of the displayed hex data. If the flag is set, the
* data bytes are printed contiguously, (0x01020304...). If the flag is cleared,
* the data bytes are printed individually (0x01 0x02 0x03 0x04...). The output
* is formatted to limit the number of columns used to display the data. The
* maximum number of bytes per line varies depending on the formatting flag
* pvreiously mentioned.
*
* RETURNS: N/A
*
* SEE ALSO: vpdPktPrintAscii(), vpdPktPrintDecimal() 
*/

void vpdPktPrintHex
    (
    VPD_PACKET *pVpdPkt,  /* pointer to VPD packet */
    BOOL       contiguous /* print contiguously or not flag */
    )
    {
    UINT32 idx;
    UINT32 limit;

    /* set the number of bytes per row based on display format. */

    limit = (contiguous) ? 16 : 8;

    for (idx = 0; idx < pVpdPkt->size; ++idx)
        {

        /*
         * if a new line is required, output a carriage return and align
         * the data field with the one in the previous line.
         */

        if ((idx != 0) && (idx % limit) == 0)
            printf ("\n        ");

        /*
         * print the data byte. if printing contiguous bytes and this is a new
         * line or printing individual bytes, print the "0x" prefix sequence.
         * if printing individual bytes, print a trailing space character.
         */

        printf ("%s%02x%c",
               ( !contiguous || ((idx % limit) == 0) ) ? "0x" : "",
               pVpdPkt->data[idx],
               (contiguous) ? '\0' : ' '); 
        }

    /* add a trailing carriage return. */

    printf ("\n");
    }


/*******************************************************************************
*
* vpdPktPrint - display the contents of a vpd packet
*
* This routine prints the contents of a vpd packet. 
*
* RETURNS: N/A
*
*/

void vpdPktPrint
    (
    VPD_PACKET *pVpdPkt   /* pointer to VPD packet */
    )
    {

    /* print the field name and value for the packet type. */

    printf (" Type = ");
    vpdPktPrintType(pVpdPkt->type);

    /*
     * if this isn't a termination or illegal packet, then print the contents
     * of the size and data fields based on the packet type.
     */

    if ((pVpdPkt->type != VPD_PID_TERM) &&
        (pVpdPkt->type != VPD_PID_GI) )
        {
        printf (" Size = 0x%02x\n", pVpdPkt->size);
        printf ("Value = ");
        
        switch (pVpdPkt->type)
            {
            case VPD_PID_PID:
            case VPD_PID_FAN:
            case VPD_PID_SN:
            case VPD_PID_MT:
                vpdPktPrintAscii(pVpdPkt);
                break;

            case VPD_PID_ICS:
            case VPD_PID_ECS:
            case VPD_PID_PCS:
            case VPD_PID_RCS:
                vpdPktPrintDecimal(pVpdPkt);
                break;
    
            case VPD_PID_PCO:
            case VPD_PID_EA:
            case VPD_PID_CRC:
            case VPD_PID_FMC:
            case VPD_PID_VLSI:
            case VPD_PID_L2:
                vpdPktPrintHex(pVpdPkt, TRUE);
                break;
    
            default:
                vpdPktPrintHex(pVpdPkt, FALSE);
                break;
            }
        }

    /* print a trailing carriage return. */

    printf ("\n");
    }


/*******************************************************************************
*
* vpdPrint - display the contents of a vpd structure
*
* This routine loops through the vpd packet pointer array and prints the
* contents of each packet.
*
* RETURNS: OK, if successful or ERROR if unsuccessful.
*
* SEE ALSO: vpdPktPrint()
*/

STATUS vpdPrint
    (
    VPD_PACKET **pVpdPkts   /* pointer to VPD packets */
    )
    {
    UCHAR type;

    /*
     * if the first entry in the vpd packet pointer array is null, the vpd
     * data is invalid. return an error indication.
     */

    if (*pVpdPkts == NULL)
        return (ERROR);

    /*
     * loop through each pointer in the vpd packet pointer array and print
     * the contents of each packet. continue until the termination packet is
     * reached.
     */

    do
        {
        type = (*pVpdPkts)->type;

        vpdPktPrint(*pVpdPkts);

        pVpdPkts++;

        } while (type != VPD_PID_TERM);

    return (OK);
    }


/*******************************************************************************
*
* vpdShow - display the board's vpd data
*
* If required, this routine reads the board-level vital product data from a
* serial eeprom and, if valid, displays the vpd contents.
*
* RETURNS: OK, if successful or ERROR if unsuccessful.
*
* SEE ALSO: N/A
*/

STATUS vpdShow (void)
    {

    /*
     * if the first entry in the vpd packet pointer array is null, then
     * the vpd data needs to be read into memory and parsed.
     */

    if (sysVpdPkts[0] == NULL)
        {
        printf ("Reading VPD data.\n\n");
        if (sysVpdInit () != OK )
            {
            printf ("VPD structure invalid.\n");

            return (ERROR);
            }
        }

    /* print the vpd data. */

    return (vpdPrint (&sysVpdPkts[0]));
    }
