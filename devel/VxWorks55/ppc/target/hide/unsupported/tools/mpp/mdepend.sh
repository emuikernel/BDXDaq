#!/bin/sh -
#
# mdepend - generate dependencies for a makefile
#
# Copyright 1984-1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02b,28jun96,c_s  changed $(FOO) to \$(FOO) where the former might be 
#                  expanded by Korn-compatible shells.
# 02a,28jun96,p_m  reintroduced dat changes that were removed by accident.
# 01z,22mar96,dat  $OBJS is recreated to drop non-existant source modules
# 01y,19mar96,dat  use $OBJS as object list to (AR). 
# 01x,15nov95,ism  added "01p,15dec93,c_s" change after tornado-windview merge
# 01w,18jun95,srh  changed C++ compiler macro to $(CXX)
# 01v,14may95,srh  added -+ option to $CPP invocation when source is C++.
# 01u,17may95,srh  added support for C++ source files, plus a few comments.
# 01t,25may95,jcf  changed VX_*_BASE to WIND_BASE/target/
# 01s,15oct93,cd   added support for SDE toolchain.
# 01r,19feb94,gae  added 'as' rule for hp tool.
# 01q,11jan94,gae  allowed vxsim pseudo-objects to slip thru the fine mesh.
# 01p,28sep94,kdl  changed "make" to "$(MAKE)" to use environment variable;
#		   added -o flag to specify output file name.
# 01p,15dec93,c_s  added a fourth class of file, "specialobjects," that are
#                  to be archived but for which dependencies and build
#                  instructions should not be generated.
# 01o,27sep93,rrr  changed for source component release
# 01n,13feb93,jdi  added documentation.
# 01m,03oct92,yp   changed "sgi" to "sgimips"
# 01l,28sep92,yp   fixed hardwiring of paths in dependencies.
# 01k,21sep92,yp   made man rule use DOC_FILES instead of *.*
# 01j,04sep92,yp   fixed typo in "sunmips" check
# 01i,03sep92,yp   added $(ASFLAGS) to MIPS assembler call
# 01h,01sep92,yp   fixed -eq test for dec. changed to check for TOOL instead of
#		   VX_HOST_TYPE. added dec and sun to those using mips assembler
#		   added documentation rules
# 01g,23aug92,jcf  backed out v01f.
# 01f,23aug92,jcf  changed library/object dependency to include all objects.
# 01e,17aug92,yp   changed archiving message to echo $(AR)
# 01d,13aug92,jcf  recreate archive if not writable (doesn't exist).
# 01c,02aug92,yp   changed hostname mips to magnum; brought the archiving 
#		   mechanism back to src directobies; added rule to support 
#		   parallel builds.
# 01b,27jul92,rrr  fixed shell.o and statTbl.o warnings.
# 01a,29jun92,rrr  written.
#
# SYNOPSIS:
# .tS
# mdepend [-f <infile>] [-o <outfile>] [<compilerflags>] -- <subdirs> -- <libfiles> -- <files>
#         -- <specialobjects>
# .tE
#
# DESCRIPTION
# This tools adds lists of targets and dependencies to a makefile. 
#
# If the '-f' option is specified, <infile> is the name of a partially-built
# makefile to which dependency lists are added.  If an output file name is
# specified using the '-o' option, that name is used for the created
# makefile that contains the dependency information.  If no output file
# name is specified, the dependency data is appended to <infile>.
# 
# The items in <subdirs> should be subdirectories of the current directory.
# For each of these items, a make target is defined with a dependency for a
# makefile called
# .BR Makefile ,
# which is expected in that subdirectory.
# 
# The items in <libFiles> should be C, C++, or assembly language source files.
# For each of these items, a make target is defined with the dependencies
# obtained by running the file through the C preprocessor with the `-M'
# option.  The target compiles the source file, and places the resulting
# object module in a library archive.
# 
# The items in <files> are treated like the items in <libfiles> but are not
# placed in a library archive.
#
# The items in <specialobjects> should be .o files.  They are added to
# $LIBOBJS as other objects are but no attempt is made to automatically
# generate dependencies for them.  This is to allow modules with special
# build requirements to be placed in a MakeSkel.  The MakeSkel should contain
# complete dependency and build rules for these objects.
#
# SECTION: 3
# NOROUTINES
#*/

IN=Makefile			# default makefile name is "Makefile"
OUT=Makefile			# default makefile name is "Makefile"

CPP=$WIND_BASE/host/$WIND_HOST_TYPE/bin/cpp
CP=cp

if [ X"$1" = "X-f" ] ; then
    IN=$2
    shift; shift
fi

if [ X"$1" = "X-o" ] ; then	# if output file name specified
    OUT=$2
    if test -f $IN ; then	# if input file really exists...
        $CP $IN $OUT		# start with contents of input file
    fi
    shift; shift
else
    OUT=$IN			# otherwise, input file becomes output file
fi



if [ $# = 0 ] ; then
	echo 'usage: mdepend [-f makefile] [flags] -- file ...'
	exit 1
fi

while :
do
    if [ X"$1" = "X--" ] ; then
	shift;
	break;
    fi
    CFLAGS="$CFLAGS $1"
    shift;
done

while :
do
    if [ X"$1" = "X--" ] ; then
	shift;
	break;
    fi
    if [ X"$1" = X"" ] ; then
	break;
    fi
    SUBDIRS="$SUBDIRS $1"
    shift;
done

while :
do
    if [ X"$1" = "X--" ] ; then
	shift;
	break;
    fi
    if [ X"$1" = X"" ] ; then
	break;
    fi

    i=`echo $1 | sed -e "s/\\.o//g"`
    if [ "$i" = shell -o $i = statTbl -o \
	"$i" = uptyLib -o $i = usimLib -o $i = uhostLib ] ; then
	    OBJS="$OBJS $1"
    elif test -f $i.c ; then
	    CARG="$CARG $i"
	    OBJS="$OBJS $1"
    elif test -f $i.s ; then
	    SARG="$SARG $i"
	    OBJS="$OBJS $1"
    elif test -f $i.cpp ; then
	    CPARG="$CPARG $i"
	    OBJS="$OBJS $1"
    fi
    shift;
done

while :
do
    if [ X"$1" = "X--" ] ; then
	shift;
	break;
    fi
    if [ X"$1" = X"" ] ; then
	break;
    fi

    i=`echo $1 | sed -e "s/\\.o//g"`
    if test -f $i.c ; then
	    CARG2="$CARG2 $i"
    elif test -f $i.s ; then
	    SARG2="$SARG2 $i"
    elif test -f $i.cpp ; then
	    CPARG2="$CPARG2 $i"
	    OBJS2="$OBJS2 $1"
    fi
    shift;
done

while :
do
    if [ X"$1" = "X--" ] ; then
        shift;
        break;
    fi
    if [ X"$1" = X"" ] ; then
        break;
    fi

    OBJS="$OBJS $1"
    shift;
done


TMP=$OUT

trap 'rm -f $TMP ; exit 1' 1 2 3 13 15

if [ X"$SUBDIRS" != X ] ; then
    set $SUBDIRS
    echo "subdirs: \\" >> $TMP
    while :
    do
	if [ $# = "1" ] ; then
	    echo "	$1/Makefile" >> $TMP
	    break;
	else
	    echo "	$1/Makefile \\" >> $TMP
	    shift;
	fi
    done
    echo '	for DIR in $(SUBDIRS) ; \
	do ( cd $$DIR; $(MAKE) CPU=$(CPU) TOOL=$(TOOL) $(TARGET) ) ; \
	done
' >> $TMP
    for SUBD in $SUBDIRS
    do
	echo "$SUBD/Makefile:	$SUBD/MakeSkel" >> $TMP
	echo "	(\$(RM) \$@; cd $SUBD; \$(WIND_BASE)/host/\$(WIND_HOST_TYPE)/bin/mpp > Makefile)" >> $TMP
    done
else
    echo 'subdirs:' >> $TMP
fi

# Make documentation
echo 'man::' >> $TMP
if [ X"$SUBDIRS" != X ] ; then
    echo '	for DIR in $(SUBDIRS) ; \
	do ( cd $$DIR; $(MAKE) CPU=$(CPU) TOOL=$(TOOL) man ) ; \
	done
' >> $TMP
fi
echo '	$(WIND_BASE)/host/$(WIND_HOST_TYPE)/bin/mg $(DOC_FILES)
' >>$TMP


if [ X"$OBJS" != "X" ] ; then
    set $OBJS
    echo "LIBOBJS= \\" >> $TMP
    while :
    do
      if [ $# = "1" ] ; then
          echo "      \$(LIBDIR)"$1 >> $TMP
          break;
      else
          echo "      \$(LIBDIR)"$1" \\" >> $TMP
          shift;
      fi
    done

echo '
' >> $TMP

    set $OBJS
    echo "OBJS= \\" >> $TMP
    while :
    do
      if [ $# = "1" ] ; then
          echo "      "$1 >> $TMP
          break;
      else
          echo "      "$1" \\" >> $TMP
          shift;
      fi
    done

echo '
objs:	$(LIBOBJS)

lib:    $(UP)lib/$(LIBNAME)

$(UP)lib/$(LIBNAME): $(LIBOBJS)
	@ if [ -w $@ ]; then \
	    (cd $(LIBDIR); \
	    echo "$(AR) cru " $@ $(OBJS); \
	    $(AR) cru ../$(LIBNAME) $(OBJS); \
	    $(RANLIB) ../$(LIBNAME)) \
	else \
	    (cd $(LIBDIR); \
	    $(RM) ../$(LIBNAME); \
	    echo "$(AR) cru " $@ $(LIBDIR)"*.o"; \
	    $(AR) cru ../$(LIBNAME) *.o; \
	    $(RANLIB) ../$(LIBNAME)) \
	fi

' >> $TMP

for i
	in $CARG
	do
		$CPP -M $CFLAGS $i.c | sed -e "/:/s/^/\$(LIBDIR)/" -e s:$WIND_BASE":\$(WIND_BASE):g" -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
		echo "	\$(RM) \$(LIBDIR)"$i.o >> $TMP
		echo "	\$(CC) \$(CFLAGS) -c -o \$(LIBDIR)"$i.o $i.c >> $TMP
		echo "" >> $TMP
	done

for i
	in $CPARG
	do
		$CPP -+ -M $CFLAGS $i.cpp | sed -e "/:/s/^/\$(LIBDIR)/" -e s:$WIND_BASE":\$(WIND_BASE):g" -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
		echo "	\$(RM) \$(LIBDIR)"$i.o >> $TMP
		echo "	\$(CXX) \$(C++FLAGS) -c -o \$(LIBDIR)"$i.o $i.cpp >> $TMP
		echo "" >> $TMP
	done
	
for i
	in $SARG
	do
	if [ X"$TOOL" = "Xmips" -o X"$TOOL" = "Xsgimips" -o X"$TOOL" = "Xdecmips" \
	    -o X"$TOOL" = "Xsunmips" -o X"$TOOL" = "Xsde" -o X"$TOOL" = "Xhp" ] ; then
	    $CPP -M $CFLAGS $i.s | sed -e "s/\.s\.o/.o/" -e "/:/s/^/\$(LIBDIR)/" -e s:$WIND_BASE":\$(WIND_BASE):g" -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
	    echo "	\$(RM) \$(LIBDIR)$i.o" >> $TMP
	    echo "	\$(AS) \$(ASFLAGS) \$(CC_DEFINES) \$(CC_INCLUDE) -o \$(LIBDIR)$i.o $i.s" >> $TMP
	else
	    $CPP -M $CFLAGS $i.s | sed -e "s/\.s\.o/.o/" -e "/:/s/^/\$(LIBDIR)/" -e s:$WIND_BASE":\$(WIND_BASE):g"  -e s:$WIND_BASE":\$(WIND_BASE):g">> $TMP
	    echo "	\$(RM) \$(LIBDIR)$i.o" >> $TMP
	    echo "	\$(WIND_BASE)/host/\$(WIND_HOST_TYPE)/bin/cpp -E \$(CC_DEFINES) \$(CC_INCLUDE) $i.s | \$(AS) \$(ASFLAGS) -o \$(LIBDIR)$i.o -" >> $TMP
	fi
	done

else
echo 'lib:' >> $TMP
fi

for i
	in $CARG2
	do
	    $CPP -M $CFLAGS $i.c | sed -e s:$WIND_BASE":\$(WIND_BASE):g"  -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
	done

for i
	in $CPARG2
	do
	    $CPP -+ -M $CFLAGS $i.cpp | sed -e s:$WIND_BASE":\$(WIND_BASE):g"  -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
	done

for i
	in $SARG2
	do
	    $CPP -M $CFLAGS $i.s | sed -e "s/\.s\.o/.o/" -e s:$WIND_BASE":\$(WIND_BASE):g"  -e s:$WIND_BASE":\$(WIND_BASE):g" >> $TMP
	done
exit 0
