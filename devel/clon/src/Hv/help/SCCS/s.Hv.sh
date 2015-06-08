h18002
s 00000/00000/00000
d R 1.2 02/08/25 23:22:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/help/Hv.sh
e
s 00143/00000/00000
d D 1.1 02/08/25 23:22:30 boiarino 1 0
c date and time created 02/08/25 23:22:30 by boiarino
e
u
U
f e 0
t
T
I 1
#************************* MAJOR MOD 11/16/96 *************************

#  To ease the development in a multiplatform environment, this
#  new version of the Hv environment script uses `uname` to do
#  what is appropriate for that architecture (at JLAB)

#**********************************************************************


# This file offers assistance in creating the environment
# variables necessary to build Hv and the Hv demos.

# There are 12 variables that need to be set, after which
# a "make" in the Hv source directory should build the
# Hv library and subsequent "makes" in the demo directories
# should build those as well.

# A Table of the variables is given below with a short description.
# A MORE COMPLETE definition along with example settings is provided
# later in this file.

# The 12 variables are:

# 1)  HV_CFLAGS       //machine & compiler dependent compiler options
# 2)  HV_LIBDIR       //complete path to where libHv.a is found. This
#                       is not relevant for building the library, but
#                       for building Hv demos & applications later
# 3)  HV_INCLUDEDIR   //complete path to where the Hv header files reside
# 4)  HV_RANLIB       //specifies whether "ranlib" must be called on the
#                       completed library 
# 5)  HV_XINCLUDEDIR  //complete path to where the X include files reside
# 6)  HV_XMINCLUDEDIR //complete path to where the Motif include files reside
# 7)  HV_XLIBDIR      //complete path to where X libraries are found
# 8)  HV_XMLIBDIR     //complete path to where Motif libraries are found
# 9)  HV_STANDARDLIBS //machine dependent list of required system libs
# 10) HV_XLIBS        //list of X and Xt libraries
# 11) HV_XMLIBS       //list of Motif libraries
# 12) HV_CC           //the c compiler

# Go to the section below for each of these variables. The
# "most common" choice is uncommented. If that is OK, then
# move on. If not, comment-it-out and either uncomment one
# of the variants or add your peculiar definition. When
# you are done, "source" this file.

# some basic env vars used by the main 12

	setenv HV_HOSTOS `uname` 	
	setenv HV_ROOT   $HOME/Hv

# note: for a system wide installation HV_ROOT might be
# elsewhere, such as /usr/local/Hv

#first set all 12 "defaults"  and then change on a platform
#by platform basis

	setenv HV_CFLAGS  '-O -c'
	setenv HV_LIBDIR  $HV_ROOT/$HV_HOSTOS
	setenv HV_INCLUDEDIR  $HV_ROOT/src
	setenv HV_RANLIB  ranlib
	setenv HV_XINCLUDEDIR  /usr/include/X11
	setenv HV_XMINCLUDEDIR /usr/include/X11 
	setenv HV_XLIBDIR /usr/lib 
	setenv HV_XMLIBDIR /usr/lib
	setenv HV_STANDARDLIBS  '-lm'
	setenv HV_XLIBS  '-lXt -lX11'
	setenv HV_XMLIBS  '-lXm'
        setenv HV_CC 'cc'

if ($HV_HOSTOS == "AIX") then
  	echo "Setting Hv environment variables for architecture: "$HV_HOSTOS
	setenv HV_CFLAGS  '-g -c'
	setenv HV_STANDARDLIBS  '-lPW -lm'
	setenv HV_XLIBS  '-lXt -lXext -lX11'

else if ($HV_HOSTOS == "SunOS") then
	echo "Setting Hv environment variables for architecture: "$HV_HOSTOS
	setenv HV_XINCLUDEDIR  /usr/openwin/include
	setenv HV_XMINCLUDEDIR  /opt/SUNWspro/Motif_Solaris24/dt/include
	setenv HV_XLIBDIR  /usr/openwin/lib
	setenv HV_XMLIBDIR /usr/dt/lib

else if ($HV_HOSTOS == "HP-UX") then
  	echo "Setting Hv environment variables for architecture: "$HV_HOSTOS

# next line worked on HP-UX 9.*, with Hv_CC being just 'cc'

#	setenv HV_CFLAGS '-O -Aa -D_INCLUDE_POSIX_SOURCE -c'

# the cc (the ANSI compiler)  on our HPC160 running HPUX 10.20 toasted us
# with preprocessor errors that I don't want to track down, so used good
# 'ole reliable gcc. (Why is HP always the most painful plaform??)
 
        setenv HV_CC  'gcc'

	setenv HV_XINCLUDEDIR  /usr/include/X11R5
	setenv HV_XMINCLUDEDIR  /usr/include/Motif1.2
	setenv HV_XLIBDIR  /usr/lib/X11R5
	setenv HV_XMLIBDIR  /usr/lib/Motif1.2
	setenv HV_RANLIB  echo

#  HP variant IF YOU GET "Too many defines" error from preprocessor
#        setenv HV_CFLAGS '-O -Aa -D_INCLUDE_POSIX_SOURCE -c -Wp,-H,256000'


else if ($HV_HOSTOS == "Linux") then
  	echo "Setting Hv environment variables for architecture: "$HV_HOSTOS
	setenv HV_CFLAGS  '-g -m486 -ansi -c'
	setenv HV_XINCLUDEDIR  /usr/X11R6/include
	setenv HV_XMINCLUDEDIR  /usr/X11R6/include
	setenv HV_XLIBDIR  /usr/X11R6/lib
	setenv HV_XMLIBDIR  /usr/lib/Motif1.2
	setenv HV_XLIBS  '-lXt -lXpm -lXext -lX11'
	setenv HV_CC 'gcc'

else if ($HV_HOSTOS == "IRIX") then
  	echo "Setting Hv environment variables for architecture: "$HV_HOSTOS
	setenv HV_STANDARDLIBS  '-lPW -lm -lmalloc'
	setenv HV_XLIBS  '-lXt -lX11_s'
	setenv HV_RANLIB  echo

# DEC alpha: uname -> "OSF1"

else if ($HV_HOSTOS == "OSF1") then
	setenv HV_RANLIB  echo
	setenv HV_STANDARDLIBS  '-ldnet_stub -lPW -lm'

else 
  	echo "Using GENERIC Hv environment variables for unknown architecture: "$HV_HOSTOS
	setenv HV_CFLAGS  '-g -c'
endif


# We're done.
        echo 'Hv development variables defined'








E 1
