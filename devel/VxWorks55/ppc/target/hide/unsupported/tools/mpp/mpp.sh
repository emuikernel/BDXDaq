#!/bin/sh
#
# mpp - generate a makefile from a makefile skeleton
#
# Copyright 1984-1993 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01f,09jun95,p_m  manual update.
# 01e,25may95,jcf  changed VX_*_BASE to WIND_BASE/target/
# 01d,24may95,p_m  changed VX_HOST_TYPE to WIND_HOST_TYPE
# 01c,13feb93,jdi  added documentation.
# 01b,12aug92,yp   added defines for VXHOST
# 01a,29jun92,rrr  written.
#
# SYNOPSIS:
# .tS
# mpp > <makefile>
# .tE
#
# DESCRIPTION
# This tool generates a makefile on standard output by processing the file
# .BR MakeSkel ,
# which must be in the current directory, with the
# .B m4
# macro language processor.  Any
# .B m4
# commands may be used in the
# .BR MakeSkel .
# 
# The
# .B m4
# commands can use the variables VXHOST and VX_MAKE, which are set by mpp.
# VXHOST is set to the current host type, and VX_MAKE points to the directory
# .BR $WIND_BASE/target/h/make .
# 
# By using mpp, common information found in all makefiles is contained in a
# single file in the directory indicated by VX_MAKE.
#
# SEE ALSO:
# UNIX documentation for
# .BR m4 (1)
#
# SECTION: 3
# NOROUTINES
#*/

MAKESKEL=MakeSkel
if [ $# = 0 ] ; then
    ( echo "# Makefile" ; \
    echo "# CREATED BY mpp" ; \
    echo "#       FROM $MAKESKEL" ; \
    echo "#         ON `date`" ; \
    echo "#"
    echo "define(VXHOST,$WIND_HOST_TYPE)" ; \
    echo "define(VX_MAKE,$WIND_BASE/target/h/make)" ) | m4 - $MAKESKEL
else
    ( echo "# Makefile" ; \
    echo "# CREATED BY mpp" ; \
    echo "#       FROM $MAKESKEL" ; \
    echo "#         ON `date`" ; \
    echo "#" ; \
    echo "define(PASS2,$1)" ; \
    echo "define(VXHOST,$WIND_HOST_TYPE)" ; \
    echo "define(VX_MAKE,$WIND_BASE/target/h/make)" ) | m4 - $MAKESKEL
fi
