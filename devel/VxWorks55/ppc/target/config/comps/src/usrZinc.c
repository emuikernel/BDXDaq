/* usrZinc.c - Zinc Application Framework initialization */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
DESCRIPTION
This file launches the Zinc demo, which exists in the VxWorks archive as
part of the Zinc library.

NOMANUAL
*/

#ifndef  __INCusrZincc
#define  __INCusrZincc

extern int launchZincDemo();

int zincDemo()
{
	return (launchZincDemo());
}

#endif /* __INCusrZincc */
