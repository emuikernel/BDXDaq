# defs.gnu - definitions for GNU toolchain builds
#
# modification history
# --------------------
# 02f,06may02,sn   SPR 76627 - Use CC to link by default
# 02e,03may02,tpw  LD_SCRIPT_DOWNLOAD cannot be derived from LD_SCRIPT_RAM.
#                  It meshes poorly with scripts not named link.RAM (PPC4xx).
# 02d,01may02,sn   Restore TOOL_VENDOR (some people use it)
# 02c,24apr02,sn   SPR 75835 - collapse sections into text/data/bss before download
# 02b,24apr02,sn   SPR 75123 - always munch -c (avoiding idiosyncratic asm behaviour)
# 02a,25apr02,pai  introduced HLL_PREFIX so that prepended underscores could
#                  be redefined in architecture/toolchain fragments (SPR 73382)
# 01z,21mar02,rbl  adding macro so that project facility can add -fvolatile to
#                  bsp files
# 01y,19mar02,dat  Adding PIC flags for bootInit.o (72825)
# 01x,13mar02,sn   SPR 74080 - added large project support for Diab
# 01w,12feb02,sn   removed obsolete refs to GCC_EXEC_PREFIX
# 01v,06feb02,t_m  change ranlib to ranlib$(TOOLENV)
# 01u,21jan02,sn   removed obsolete WFC flags
# 01t,13dec01,jab  added make variable BOOTCONFIG_FLAGS
# 01s,05dec01,tpw  Use TGT_DIR to find linker scripts.
# 01r,13nov01,tam  changed definition of CC_LIB (repackaging)
# 01q,05nov01,rbl  let project facility know about optimization flags
# 01p,09nov01,tam  fixed use of OPTION_PASS_TO_LD; renamed TOOL_CONFIG
# 01o,02nov01,dat  adding -w to dependency generation options
# 01n,30oct01,tpw  Take generic defaults from new fragment defs.common
# 01m,29oct01,jab  added new ld script variables
# 01l,26oct01,tam  fixed Wl usage after merge
# 01k,19oct01,j_s  define cyclic link support macros
#		   add TOOL_CONFIG
# 01j,24oct01,dat  Relocating tool specific macros
# 01i,24oct01,jab  added new dependency generation variables
# 01h,23oct01,jab  added variables from defs.default cleanup
# 01g,19oct01,jab  added make variables for dependency generation option
# 01f,18oct01,dat  Moving tool specific defines out of target/h/make
# 01e,12sep01,sn   added LARGE_PROJECT, MUNCH_FLAGS and LD_PARTIAL
# 01d,05jun01,mem  Added EXTRACT_HEX_FLAG, EXTRACT_BIN_FLAG, VMA_START & VMA_FLAGS
# 01c,13mar01,sn   added defn of OPTION_LANG_ASM
# 01b,10jan01,sn   added defn of OPTION_LANG_C
# 01a,16oct00,sn   wrote
#
# DESCRIPTION
# This file contains default definitions and flags for builds that use the 
# GNU toolchain.

## generic defaults from defs.common come first, so they can be overridden

include $(TGT_DIR)/h/tool/common/defs.common

# get TOOL_COMMON_DIR, TOOL_DIAB and TOOL_GNU from TOOL

TOOL_FAMILY	= gnu
TOOL_COMMON_DIR	= $(subst gnu,common,$(TOOL))
TOOL_DIAB	= $(subst gnu,diab,$(TOOL))
TOOL_GNU	= $(TOOL)

# Deprecated, use TOOL_FAMILY instead
TOOL_VENDOR     = gnu

# Preprocessor option forcing every input file to be treated as a "C" file
OPTION_LANG_C=-xc

# Preprocessor option forcing every input file to be treated as a assembly
# file requiring C preprocessing
OPTION_LANG_ASM=-xassembler-with-cpp

# Library containing compiler support routines
CC_LIB=


OPTION_PP		= -E
OPTION_PP_AS		= -P
OPTION_PP_NOLINENO	= -P
OPTION_PP_COMMENT	= -C
OPTION_DOLLAR_SYMBOLS	= -fdollars-in-identifiers
OPTION_DEPEND		= -M -w
OPTION_DEPEND_GEN	= -M -MG -w
OPTION_DEPEND_C		= $(OPTION_DEPEND_GEN)
OPTION_DEPEND_AS	= $(OPTION_DEPEND)
OPTION_NO_WARNINGS	= -w
OPTION_ANSI		= -ansi
OPTION_OBJECT_ONLY	= -c
OPTION_INCLUDE_DIR	= -I
# this line has a space at the end ("-o ")
OPTION_OBJECT_NAME	= -o 
OPTION_EXE_NAME		= $(OPTION_OBJECT_NAME)
OPTION_DEFINE_MACRO	= -D
OPTION_PIC		= -fpic

CC_OPTIM_DRIVER		= -fvolatile
CC_OPTIM_NORMAL		= -O -fstrength-reduce -fcombine-regs
CC_OPTIM_TARGET		= -O -fvolatile

CC_WARNINGS_ALL		= -Wall
CC_WARNINGS_NONE	=

CC_COMPILER		= -ansi 
C++_COMPILER		= -ansi

# used by project facility

CC_PROJECT_OPTIMIZE_FLAGS = -O -O1 -O2 -O3 -O4
PROJECT_BSP_FLAGS_EXTRA = $(filter -fvolatile,$(CC_OPTIM_TARGET))

# For generating PIC code
# For T2.2, CC_PIC is only defined for MIPS architecture see defs.mips.
# CC_PIC		= $(OPTION_PIC)


## toolchain binaries

AS              	= as$(TOOLENV)
CC              	= cc$(TOOLENV)
CF			= CC
CXX			= $(CC)
LD              	= ld$(TOOLENV)
RANLIB          	= ranlib$(TOOLENV)
CPP			= $(CC) -E -P
CPP_PRINT_MACROS        = $(CPP) -dM -E -pipe $(CC_INCLUDE) $(CC_DEFINES)
TCL			= wtxtcl
OBJCPY	         	= objcopy
SIZE            	= size$(TOOLENV)
NM              	= nm$(TOOLENV)
AR              	= ar$(TOOLENV)

MUNCH           	= wtxtcl $(WIND_BASE)/host/src/hutils/munch.tcl $(MUNCH_FLAGS)
LARGE_PROJECT   	= true
OPTION_INDIRECT_FILE    = @
OPTION_NM_INDIRECT_FILE = @

MUNCH_FLAGS     	= -c $(TOOLENV)
NM_GLOB_FLAGS		= -g


HLL_PREFIX              = _

ROM_ENTRY               = $(HLL_PREFIX)romInit
SYS_ENTRY               = $(HLL_PREFIX)sysInit
USR_ENTRY               = $(HLL_PREFIX)usrInit

BOOTCONFIG_FLAGS	=


LDFLAGS         	= -X -N

LD_ENTRY_OPT		= -e
LD_PARTIAL_FLAGS	= -X -r

# Wherever possible we use the compiler driver as a frontend to
# the linker. In particular this facilitates the GNU -frepo
# template repository system. Configurations using GNU Tools
# but non-GNU linkers (such as SIMHPPA)  should define
# GNU_USE_LD_TO_LINK = yes

ifeq ($(GNU_USE_LD_TO_LINK),)
BASE_LD_PARTIAL_FLAGS   = $(filter-out -r -nostdlib,$(LD_PARTIAL_FLAGS))
LD_PARTIAL              = $(CC) -r -nostdlib $(subst -,$(OPTION_PASS_TO_LD)-,$(BASE_LD_PARTIAL_FLAGS))
OPTION_PASS_TO_LD       = -Wl,
else
LD_PARTIAL            = $(LD) $(LD_PARTIAL_FLAGS)
OPTION_PASS_TO_LD     =
endif

# These are flags to pass to the linker the last time we do a
# relocatable link. They never get used by a bootable project
# and they only get used once by a downloadable project.

LD_SCRIPT_DOWNLOAD      = -T $(TGT_DIR)/h/tool/gnu/ldscripts/link.OUT
LD_COLLAPSE_SECTIONS    = $(LD_SCRIPT_DOWNLOAD)
LD_PARTIAL_LAST_FLAGS   = $(LD_COLLAPSE_SECTIONS)

LD_SCRIPT_RAM	= -T $(TGT_DIR)/h/tool/gnu/ldscripts/link.RAM
LD_SCRIPT_ROM	= -T $(TGT_DIR)/h/tool/gnu/ldscripts/link.ROM

# for cyclic link support

LD_START_GROUP  	= --start-group
LD_END_GROUP		= --end-group
LD_PARTIAL_START_GROUP	= $(OPTION_PASS_TO_LD)$(LD_START_GROUP)
LD_PARTIAL_END_GROUP	= $(OPTION_PASS_TO_LD)$(LD_END_GROUP)


LD_LOW_FLAGS    	= -Ttext $(RAM_LOW_ADRS)
LD_HIGH_FLAGS   	= -Ttext $(RAM_HIGH_ADRS)
RES_LOW_FLAGS   	= -Ttext $(ROM_LINK_ADRS) -Tdata $(RAM_LOW_ADRS)
RES_HIGH_FLAGS  	= -Ttext $(ROM_LINK_ADRS) -Tdata $(RAM_HIGH_ADRS)

# For 960 IMI
LD_IMI_ADRS		= -Ttext $(IMI_TEXT_ADRS) -Tdata $(IMI_DATA_ADRS)
