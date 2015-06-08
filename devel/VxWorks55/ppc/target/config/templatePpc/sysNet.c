/* sysNet.c - template system network initialization */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
TODO -  Remove the template modification history and begin a new history
        starting with version 01a and growing the history upward with
        each revision.

01e,28aug01,dgp  change manual pages to reference entries per SPR 23698
01d,10mar97,dat  comments from reviewers
01c,28jan97,dat  fixed title line, added some comments.
01b,17jan97,dat  removed sysBusXxx, added sysNetHwInit2() and INCLUDE_NETWORK
01a,22oct96,ms   derived from the mv147 BSP
*/

/*
DESCRIPTION
This library contains board-specific routines for network subsystems.

The generic network interface drivers each require different routines
from the BSP. For example, the if_ei driver (Intel 82596 Ethernet)
requires seven BSP support routines to be defined. Consult the reference
entry of the appropriate driver for details.

Note: Generic network interface drivers are not provided in source
code form; therefore they are not #included.
*/

/* This file contributes nothing if INCLUDE_NETWORK is not defined */

#ifdef INCLUDE_NETWORK

/* includes */

#include "vxLib.h"


/******************************************************************************
*
* sysNetHwInit - initialize the network interface
*
* This routine initializes the network hardware to a quiescent state.  It
* does not connect interrupts.
*
* Only polled mode operation is possible after calling this routine.
* Interrupt mode operation is possible after the memory system has been
* initialized and sysNetHwInit2() has been called.
*/

void sysNetHwInit (void)
    {
    }

/******************************************************************************
*
* sysNetHwInit2 - initialize additional features of the network interface
*
* This routine completes initialization needed for interrupt mode operation
* of the network device drivers.  Interrupt handlers can be connected.
* Interrupt or DMA operations can begin.
*/

void sysNetHwInit2 (void)
    {
    }

/*
 * TODO - add BSP support routines for network interface drivers. 
 *
 * See the reference entries for the specific drivers for their support 
 * requirements.
 */

#endif /* INCLUDE_NETWORK */
