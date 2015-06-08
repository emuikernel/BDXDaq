/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#ifdef __cplusplus
#  define LINKAGE extern "C"
#else
#  define LINKAGE extern
#endif

LINKAGE int StripTool_main (int, char *[]);


int main (int argc, char *argv[])
{
  return StripTool_main (argc, argv);
}
