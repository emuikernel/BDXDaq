#
# flags setting for the Hv directory
#

HV_HOSTOS = $(shell uname)
HV_ROOT = $(CLON)/source/Hv

HV_CFLAGS = -O -c
HV_LIBDIR = $(HV_ROOT)/s
HV_INCLUDEDIR = $(HV_ROOT)/s
HV_RANLIB = ranlib
HV_XINCLUDEDIR = /usr/include/X11
HV_XMINCLUDEDIR = /usr/include/X11 
HV_XLIBDIR = /usr/lib 
HV_XMLIBDIR = /usr/lib
HV_STANDARDLIBS = -lm
HV_XLIBS = -lXt -lX11
HV_XMLIBS = -lXm
HV_CC = cc



ifeq ("$(HV_HOSTOS)","SunOS")
  HV_XINCLUDEDIR  = /usr/openwin/include
  HV_XLIBDIR      = /usr/openwin/lib
  HV_XMLIBDIR     = /usr/dt/lib
endif

#ifeq ("$(HV_HOSTOS)","Linux")
##  	echo "Setting Hv environment variables for architecture: "$(HV_HOSTOS)
#	HV_CFLAGS='-g -m486 -ansi -c'
#	HV_XINCLUDEDIR=/usr/X11R6/include
#	HV_XMINCLUDEDIR=/usr/X11R6/include
#	HV_XLIBDIR=/usr/X11R6/lib
#	HV_XMLIBDIR=/usr/lib/Motif1.2
#	HV_XLIBS='-lXt -lXpm -lXext -lX11'
#	HV_CC='gcc'
#else 
##  	echo "Using GENERIC environment for unknown architecture:"$(HV_HOSTOS)
#	HV_CFLAGS='-g -c'
#endif

