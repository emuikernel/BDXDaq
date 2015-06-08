# Makefile - makefile for bsp/src/drv/scsi
#
# modification history
# --------------------
# 01k,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01j,12oct01,tam  added repackaging support
# 01i,16aug01,hdn  added PENTIUM2/3/4 support
# 01h,21jun00,dat  added sym895Lib.[co]
# 01g,25aug98,sut  added module ncr710CommLib.c
# 01f,03jun98,dat  fixed EXTRA_INCLUDE to use TGT_DIR
# 01e,29may98,dbt  added support for pentium
# 01d,06sep97,dds  made aic7880Lib.c CPU specific.
# 01c,10jul97,dds  added module aic7880Lib.c
# 01b,04nov96,p_m  Added objects added since 01j of MakeSkel
# 01a,18jun96,yp   created from 01j of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the drv library.
#
# This file also contains rules for building an NCR script .c file from the
# .n script file.  The .c file is then copied into the src/drv/scsi directory
# where it is then included with the library built in src/drv/scsi.
# The documentation includes in the .n file is extracted and added to 
# the final .c file.
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=	mb87030Lib.c ncr5390Lib.c ncr5390Lib1.c ncr5390Lib2.c \
                ncr710Lib.c ncr710Lib2.c ncr810Lib.c wd33c93Lib.c \
		aic7880Lib.c wd33c93Lib1.c  wd33c93Lib2.c ncr710CommLib.c \
		sym895Lib.c

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS_COMMON = mb87030Lib.o ncr5390Lib.o ncr5390Lib1.o ncr5390Lib2.o \
              ncr710Lib.o ncr710init.o ncr710Lib2.o ncr710init2.o \
              ncr810Lib.o ncr810init.o wd33c93Lib.o wd33c93Lib1.o \
	      wd33c93Lib2.o ncr710CommLib.o sym895Lib.o

OBJS_I80386 = aic7880Lib.o

OBJS_I80486 = aic7880Lib.o

OBJS_PENTIUM = aic7880Lib.o

OBJS_PENTIUM2 = aic7880Lib.o

OBJS_PENTIUM3 = aic7880Lib.o

OBJS_PENTIUM4 = aic7880Lib.o

OBJS         = $(OBJS_COMMON) $(OBJS_$(CPU))

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries

# note: in order to compile script the script compilers should be in the path

CPP         = cpp -C -P $(CCFLAGS) $(CC_DEFINES)
NCRCPP      = n710psun4
NCRCC       = n710csun4
NCRH1       = $(TGT_DIR)/h/drv/scsi/ncr710Script1.h


# Build rules for ncrXXXinit.c. The .c files are created from the .n files.
# However, this process is not automated due to the availability of the
# nasm compiler on SUN hosts. The following procedure applies to pre-processing
# the .n file and making it ready for nasm compilation. The objective is
# to use the C pre-processor to create a temporary .i file from a .n file.
# The .i file can then be compiled using nasm on any host, in order to
# get the .c file. The .c file is then compiled by this Makefile.

#Example:
#          $ cpp -P < ncr810init.n > ncr810init.i
#     On the host where nasm resides
#          $ nasm -a 825 -o ncr810init.c ncr810init.i

