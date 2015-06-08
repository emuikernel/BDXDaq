/* roadMap.c - provides definitions of kernel structures to VxGDB/emulators. */

/* Copyright 1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,27feb93,c_s  written.
*/

#include "vxWorks.h"
#include "private/windLibP.h"
#include "private/kernelLibP.h"
#include "private/taskLibP.h"
#include "qPriNode.h"
#include "private/msgQLibP.h"

int roadmapPresent = 1;

