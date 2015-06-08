h62635
s 00002/00004/00247
d D 1.4 02/09/09 11:48:46 boiarino 5 4
c libHvold.a
c cleanup
e
s 00001/00039/00250
d D 1.3 02/08/26 21:53:44 boiarino 4 3
c include $(CLON_SOURCE)/Hv/flags.mk
e
s 00001/00001/00288
d D 1.2 02/08/25 23:29:54 boiarino 3 1
c src -> s; Motif location still must be fixed
e
s 00000/00000/00000
d R 1.2 02/08/25 23:21:29 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Makefile
e
s 00289/00000/00000
d D 1.1 02/08/25 23:21:28 boiarino 1 0
c date and time created 02/08/25 23:21:28 by boiarino
e
u
U
f e 0
t
T
I 1
#
# Makefile for Hv/s directory
#
D 4

E 4
SHELL = /bin/sh

D 4
HV_HOSTOS = $(shell uname)
HV_ROOT = $(CLON_SOURCE)/Hv
E 4
I 4
include $(CLON_SOURCE)/Hv/flags.mk
E 4

D 4
HV_CFLAGS = -O -c
HV_LIBDIR = $(HV_ROOT)/$(HV_HOSTOS)
D 3
HV_INCLUDEDIR = $(HV_ROOT)/src
E 3
I 3
HV_INCLUDEDIR = $(HV_ROOT)/s
E 3
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
  HV_XMINCLUDEDIR = /opt/SUNWspro/Motif_Solaris24/dt/include
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

E 4
#---------------------------------------------------------------------
#define the C compiler
#---------------------------------------------------------------------

     CC = $(HV_CC)

#---------------------------------------------------------------------
#define rm & mv so as not to return errors
#---------------------------------------------------------------------

     RM =  rm -f
     MV =  mv -f

#---------------------------------------------------------------------
#define the archiver
#---------------------------------------------------------------------

     AR =  ar

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

D 5
     INCLUDES = -I$(HV_INCLUDEDIR)\
        -I$(HV_XINCLUDEDIR)\
        -I$(HV_XMINCLUDEDIR)
E 5
I 5
     INCLUDES = -I. -I$(HV_INCLUDEDIR) -I$(HV_XINCLUDEDIR) -I$(HV_XMINCLUDEDIR)
E 5


#---------------------------------------------------
#program and library names
#---------------------------------------------------

D 5
     LIBNAME = libHv.a
E 5
I 5
     LIBNAME = libHvold.a
E 5

#---------------------------------------------------------------------
#the source files that make up the application
#---------------------------------------------------------------------

SRCS = \
          Hv_attributes.c\
          Hv_balloon.c\
          Hv_barplot.c\
          Hv_box.c\
          Hv_button.c\
          Hv_callback.c\
          Hv_choice.c\
          Hv_color.c\
          Hv_colorbutton.c\
          Hv_compound.c\
          Hv_configure.c\
          Hv_connect.c\
          Hv_container.c\
          Hv_dlogmenus.c\
          Hv_dlogs.c\
          Hv_dlogsupport.c\
          Hv_dlogtext.c\
          Hv_drag.c\
          Hv_draw.c\
          Hv_drawsupport.c\
          Hv_drawtools.c\
          Hv_feedback.c\
          Hv_fileselect.c\
          Hv_fonts.c\
          Hv_fractdone.c\
          Hv_help.c\
          Hv_histo.c\
          Hv_init.c\
          Hv_io.c\
          Hv_items.c\
          Hv_led.c\
          Hv_linklist.c\
          Hv_listdlogs.c\
          Hv_mapitem.c\
          Hv_mapmath.c\
          Hv_mapreader.c\
          Hv_math.c\
          Hv_menubutton.c\
          Hv_menus.c\
          Hv_offscreen.c\
          Hv_optionbutton.c\
          Hv_plotsupport.c\
          Hv_pointer.c\
          Hv_postscript.c\
          Hv_print.c\
          Hv_privdraw.c\
          Hv_psdraw.c\
          Hv_radiodlogs.c\
          Hv_rainbowscale.c\
          Hv_rectsupport.c\
          Hv_region.c\
          Hv_scaledlogs.c\
          Hv_scroll.c\
          Hv_separator.c\
          Hv_slider.c\
          Hv_standard.c\
          Hv_textentryitem.c\
          Hv_textitem.c\
          Hv_undo.c\
          Hv_utilities.c\
          Hv_views.c\
          Hv_virtual.c\
          Hv_wheel.c\
          Hv_worldline.c\
          Hv_worldpolygon.c\
          Hv_worldrect.c\
          Hv_worldwedge.c\
          Hv_xyplot.c\
          Hv_xyplotedit.c\
          Hv_xyplotio.c


#---------------------------------------------------------------------
# The object files (via macro substitution)
#---------------------------------------------------------------------

     OBJS = ${SRCS:.c=.o}

#---------------------------------------------------------------------
#how to make a .o file from a .c file 
#---------------------------------------------------------------------

.c.o :
	$(RM) $@
	$(CC) $(HV_CFLAGS) $(INCLUDES) $<


#--------------------------------------------------------
# Default target

#the tmpfle business is just so mv does not
#give an error if no files are there to move

#--------------------------------------------------------

all: $(LIBNAME)

#---------------------------------------------------------------------
# This rule generates the library using the object files and libraries.
#---------------------------------------------------------------------

$(LIBNAME): $(OBJS)
	$(AR) r $(LIBNAME) $(OBJS)
	$(HV_RANLIB) $(LIBNAME)

#---------------------------------------------------------------------
# This rule install library (type "make install").
#---------------------------------------------------------------------

exports: install
install:
	cp $(LIBNAME) $(CLON_LIB)

#---------------------------------------------------------------------
# This rule cleans up the build directory (type "make clean").
#---------------------------------------------------------------------

distclean: clean
clean: 
	$(RM) $(LIBNAME) *.o core

#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

STANDARD = Hv.h Hv_data.h Hv_private.h Hv_linklist.h Hv_undo.h Hv_attributes.h
PLUSPLOT = $(STANDARD) Hv_xyplot.h
PLUSMAPS = $(STANDARD) Hv_maps.h

Hv_attributes.o:     $(STANDARD)
Hv_balloon.o:        $(STANDARD)
Hv_barplot.o:        $(PLUSPLOT)
Hv_box.o:            $(STANDARD)
Hv_button.o:         $(STANDARD)
Hv_callback.o:       $(STANDARD)
Hv_color.o:          $(STANDARD)
Hv_colorbutton.o:    $(STANDARD)
Hv_compound.o:       $(STANDARD)
Hv_configure.o:      $(STANDARD)
Hv_connect.o:        $(STANDARD)
Hv_container.o:      $(STANDARD)
Hv_dlogmenus.o:      $(STANDARD)
Hv_dlogs.o:          $(STANDARD)
Hv_dlogsupport.o:    $(STANDARD)
Hv_dlogtext.o:       $(STANDARD)
Hv_drag.o:           $(STANDARD)
Hv_draw.o:           $(STANDARD)
Hv_drawsupport.o:    $(STANDARD)
Hv_drawtools.o:      $(STANDARD)
Hv_feedback.o:       $(STANDARD)
Hv_fileselect.o:     $(STANDARD)
Hv_fonts.o:          $(STANDARD)
Hv_fractdone.o:      $(STANDARD)
Hv_help.o:           $(STANDARD)
Hv_histo.o:          $(PLUSPLOT)
Hv_init.o:           $(STANDARD)
Hv_io.o:             $(STANDARD)
Hv_items.o:          $(PLUSMAPS)
Hv_led.o:            $(STANDARD)
Hv_linklist.o:       $(STANDARD) 
Hv_listdlogs.o:      $(STANDARD)
Hv_mapitem.o:        $(PLUSMAPS)
Hv_mapmath.o:        $(PLUSMAPS)
Hv_mapreader.o:      $(PLUSMAPS)
Hv_math.o:           $(STANDARD)
Hv_menubutton.o:     $(STANDARD)
Hv_menus.o:          $(PLUSPLOT)
Hv_offscreen.o:      $(STANDARD)
Hv_optionbutton.o:   $(STANDARD)
Hv_plotsupport.o:    $(PLUSPLOT)
Hv_pointer.o:        $(PLUSPLOT)
Hv_postscript.o:     $(STANDARD)
Hv_print.o:          $(STANDARD)
Hv_privdraw.o:       $(STANDARD)
Hv_psdraw.o:         $(STANDARD)
Hv_choice.o:         $(STANDARD)
Hv_radiodlogs.o:     $(STANDARD)
Hv_rainbowscale.o:   $(STANDARD)
Hv_rectsupport.o:    $(STANDARD)
Hv_region.o:         $(STANDARD)
Hv_scaledlogs.o:     $(STANDARD)
Hv_scroll.o:         $(STANDARD)
Hv_separator.o:      $(STANDARD)
Hv_slider.o:         $(STANDARD)
Hv_standard.o:       $(STANDARD)
Hv_textentryitem.o:  $(STANDARD)
Hv_textitem.o:       $(PLUSPLOT)
Hv_undo.o:           $(STANDARD)
Hv_utilities.o:      $(STANDARD)
Hv_views.o:          $(PLUSPLOT)
Hv_virtual.o:        $(STANDARD)
Hv_wheel.o:          $(STANDARD)
Hv_worldline.o:      $(PLUSPLOT)
Hv_worldpolygon.o:   $(PLUSPLOT)
Hv_worldrect.o:      $(PLUSPLOT) 
Hv_worldwedge.o:     $(PLUSPLOT) 
Hv_xyplot.o:         $(PLUSPLOT) 
Hv_xyplotedit.o:     $(PLUSPLOT) 
Hv_xyplotio.o:       $(PLUSPLOT) 






E 1
