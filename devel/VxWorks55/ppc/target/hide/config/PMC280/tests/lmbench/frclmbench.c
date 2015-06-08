/* frclmbench.c - FORCE COMPUTERS Lmbench wrappars */

/*
##############################################################################
#
# Project: PangeaDB
#
##############################################################################
#
# $Id: frclmbench.c,v 1.1.1.4 2003/07/09 15:03:03 anish Exp $
#
# ETQ
# FORCE COMPUTERS IDC
# Anish Gupta
# Bangalore, India
#
# Remarks: Note! This material is proprietary of FORCE COMPUTERS and may
# only be viewed under the terms of a signed non-disclosure agreement,
# or a source code license agreement! Copying, compilation, modification,
# distribution or any other use whatsoever of this material is strictly
# prohibited without written permission of FORCE COMPUTERS GmbH.
# The information in this document is subject to change without notice
# and should not be construed as a commitment by FORCE COMPUTERS.
# Neither FORCE COMPUTERS, nor the authors assume any responsibility for
# the use or reliability of this document or the described software.
#
# Copyright (c) 2000, FORCE COMPUTERS IDC. All rights reserved.
#
##############################################################################
------------------------------------------------------------------------------
$Log: frclmbench.c,v $
Revision 1.1.1.4  2003/07/09 15:03:03  anish
EAU release first iteartion

Revision 1.2  2003/06/27 13:10:43  anish
Cleared warnings except lmbench

Revision 1.1.1.2  2003/06/26 09:43:13  anish
EAU release base code

Revision 1.1  2003/05/30 16:12:01  shailendra
Adding mem_perf and frclmbench.c

Revision 1.1.1.1  2003/05/30 15:20:59  anish
Release T1.0.1/B1.0 testtool

Revision 1.1.1.1  2003/03/07 10:39:09  anish
added files in misc and memtest



------------------------------------------------------------------------------
modification history
--------------------
01a,5Mar03,anish Ceated file
*/

/*
DESCRIPTION:
This file contains routines which are helpful for running LMbench tool.
The purpose of the routines is to make easier running the LMbench tool.

This file provides the following functionalities:

   - frcTestbwMem :  Runs bw_mem of standard LMbench tool.
   - frcTestlatMemRead: Runs lat_mem_rd of standard LMbench tool.

INCLUDE FILES: ../main/testtool.h

SEE ALSO: lmbench directory
*/




#include<vxWorks.h>
#include<stdio.h>


#define BWMEM_MAX 0x200000


void print_bytes();
/*******************************************************************************
*
* frcTestbwMem - Runs bw_mem of standard LMbench tool.
*
* Runs bwMem function for read, write, read-write and copy
* with differnt size starting from 1KB to BWMEM_MAX.
*
* RETURNS: OK/ERROR
*
*/
STATUS frcTestbwMem (void)
{
  unsigned int size = 1024;

  printf
    ("\n*********************************\nRead \n   Size        Time         Bandwidth\n");
  for (size = 1024; size <= BWMEM_MAX; size *= 2)
    bwMem (size, "rd");

  printf
    ("\n*********************************\nWrite \n   Size      Time        Bandwidth\n");
  for (size = 1024; size <= BWMEM_MAX; size *= 2)
    bwMem (size, "wr");

  printf
    ("\n*********************************\nRead-Write \n   Size      Time        Bandwidth\n");
  for (size = 1024; size <= BWMEM_MAX; size *= 2)
    bwMem (size, "rdwr");

  printf
    ("\n*********************************\nCopy \n   Size      Time        Bandwidth\n");
  for (size = 1024; size <= BWMEM_MAX; size *= 2)
    bwMem (size, "cp");
  printf
    ("\n------------------------------------------------------------------\n");
  return OK;
}

/*******************************************************************************
*
* frcTestlatMemRead - Runs lat_mem_rd of standard LMbench tool.
*
* Runs latMemRd function for a stride size of 32.
*
* RETURNS: OK/ERROR
*
*/


STATUS frcTestlatMemRead (void)
{
  char *ptr;
  unsigned int size, stride;
  size = 1000000;
  stride = 32;

#ifdef LATMEMRD_MANUAL
  printf ("\nEnter stride:");
  scanf ("%d", &stride);

  size = 10000;
  printf ("Enter size:");
  scanf ("%d", &size);
#endif
  /*Stride */
  ptr = (char *) malloc (size);
  if (ptr == NULL)
    {
      perror ("(malloc) ");
      return ERROR;
    }
  printf ("\nlat_mem_rd size=\n");
  print_bytes (size);
  printf ("  stride=%d", stride);
  latMemRead (ptr, size, stride);
  return OK;
}


void print_bytes(int size)
{
printf (" %d MB",size/(1024*1024));
}
