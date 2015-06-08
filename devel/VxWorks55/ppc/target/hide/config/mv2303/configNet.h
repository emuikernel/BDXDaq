/* configNet.h - network configuration header */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,12jun02,kab  SPR 74987: cplusplus protection
01i,19aug99,dmw  Added support for secondary Ethernet.
01h,11mar99,tm   ADDED DEC_USR_PHY_CHK to default user flagset
01g,04mar98,dat  Restored PReP addressing setup
01f,19jan98,gnn  made addresses slave to new, patched BSP, addresses.
01e,20nov97,gnn  fixed spr#9555, configurable numRds and numTds
01d,03oct97,gnn  added function prototype for load function
01c,25sep97,gnn  SENS beta feedback fixes
01b,02jun97,map  updated ivec and ilevel.
01a,25apr97,map  written.

*/
 
#ifndef INCnetConfigh
#define INCnetConfigh

#ifdef __cplusplus
    extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"

#define DEC_LOAD_FUNC	dec21x40EndLoad
#define DEC_BUFF_LOAN   1

/* <devAdrs>:<PCIadrs>:<ivec>:<ilevel>:<numRds>:<numTds>:<memBase>:<memSize>:<userFlags> */

#ifdef EXTENDED_VME
#  define   DEC_LOAD_STRING  "0xfe020000:0x0:0x12:0x12:-1:-1:-1:0:0x80800000"
#else
#  define   DEC_LOAD_STRING  "0x81020000:0x80000000:0x12:0x12:-1:-1:-1:0:0x80800000"
#endif

IMPORT END_OBJ* DEC_LOAD_FUNC (char*, void*);

END_TBL_ENTRY endDevTbl [] =
{
    { 0, DEC_LOAD_FUNC, DEC_LOAD_STRING, DEC_BUFF_LOAN, NULL, FALSE},
#ifdef INCLUDE_SECONDARY_ENET
    { 1, DEC_LOAD_FUNC, DEC_LOAD_STRING, DEC_BUFF_LOAN, NULL, FALSE},
#endif /* INCLUDE_SECONDARY_ENET */
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
};

#ifdef INCLUDE_SECONDARY_ENET
#   undef IP_MAX_UNITS
#   define IP_MAX_UNITS 2
#endif /* INCLUDE_SECONDARY_ENET */

#ifdef __cplusplus
    }
#endif

#endif /* INCnetConfigh */
