# macrosInfo.tcl - Macros description resource file
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01g,31mar99,cjs  Exposing LIBS in downloadable projects
# 01f,09feb99,cjs  Changing description for various build macros so that
#                  warning is more prominent
# 01e,19jan99,cjs  removed rule info and put in prj_ files 
# 01d,11nov98,sn   replaced LD_PARTIAL_FLAGS by LD_PARTIAL and updated
#                  help-text accordingly
# 01c,26Oct98,ms   added other macro properties - rules, components, ...
# 01b,27Aug98,ms   added descriptions for vxApp rules.
# 01a,05jun98,jmp  written.
#
# DESCRIPTION
# This file contains array's information about macros and rules used
# in the build. Note: additional macros and rules can be added by
# adding to the tcl arrays below. This can be done in this file,
# or in a ".addin.tcl" file (any file ending in that suffix is automatically
# sourced when the project facility begins). We recommend using
# the name "buildInfo<XXX>.addin.tcl".
#
# macrosOfProj(projType):
#	A list of macros supported for a given project type
# macroDesc($MACRO):
#	Quick description of the macro, this info is displayed by the IDE.
#	The default is no description.
# macroDefault($MACRO):
#	Default value for this macro.
#	The default is <unknown>, in which case the code generator
#	won't write the macro to the Makefile until it is explicitly set.
#	Most build macros for vxWorks are explicitly set when the project
#	is created from the BSP.
# macroRules($MACRO):
#	Build rules for which this macro is used (as a regexp).
#	For example, the macro HEX_FLAGS has macroRules=rom, meaning
#	it only applies for build images containing the word "rom".
#	The default is all build rules.
# macroCpu($MACRO):
#	Target CPU's for which this macro is used (as a regexp).
#	For example the IMI_* macros are only used for I960 CPU's.
#	The default is all CPU's.
# macroComps($MACRO):
#	INCLUDE_* components for which this macro applies (as a regexp).
#	The macro is only written to the Makefile if at least one
#	of the corresponding components is included.
#	The default is all components (e.g., always write out this macro).
#
# macroHide:
#	A list of macros to hide from the GUI macro editor.
#	Use this if you have added a "property page" to configure the
#	macros (e.g., CFLAGS and CC are configured via a property page
#	for the compiler, which is a nicer interface).
#	Usage: lappend macroHide <myMacros>
#

set macroHide [list CPP NM SIZE AR CC CFLAGS AS CFLAGS_AS LD LDFLAGS	\
	OPTION_OBJECT_ONLY OPTION_INCLUDE_DIR OPTION_DEFINE_MACRO	\
	DEFAULT_RULE DEPEND_GEN_UTIL LDDEPS]

set macrosOfProj(::prj_vxWorks) 		\
	[list ROM_TEXT_ADRS ROM_SIZE RAM_LOW_ADRS RAM_HIGH_ADRS \
		LD_PARTIAL LIBS HEX_FLAGS ROM_IMI_OFFSET	\
		IMI_DATA_ADRS IMI_TEXT_ADRS IMI_SPEC IBR	\
		POST_BUILD_RULE EXTRA_MODULES]

set macrosOfProj(::prj_vxApp)		\
	[list POST_BUILD_RULE ARCHIVE PRJ_LIBS]

set macroDesc(ROM_TEXT_ADRS) \
	"Warning: also defined in config.h; the two definitions must match! \
	Address of the ROM entry point."
set macroRule(ROM_TEXT_ADRS)	rom
set macroDesc(ROM_SIZE) \
	"Warning: also defined in config.h; the two definitions must match! \
	Number of bytes available in the ROM."
set macroRule(ROM_SIZE)		rom
set macroDesc(RAM_LOW_ADRS) \
	"Warning: also defined in config.h; the two definitions must match! \
	Beginning address to use for the VxWorks run-time in RAM."
set macroDesc(RAM_HIGH_ADRS) \
	"Warning: also defined in config.h; the two definitions must match! \
	RAM address where the boot ROM data segment is loaded.\
	Must be a high enough value to ensure loading VxWorks does\
	not overwrite part of the ROM program."

set macroDesc(LD_PARTIAL) \
	"Linker command used to merge several .o's into a single .o"

set macroDesc(LIBS) \
	"WARNING: LIBS includes libraries against which vxWorks is linked"
set macroDesc(PRJ_LIBS) \
	"Libraries against which a downloadable app is linked"
set macroDesc(EXTRA_MODULES) \
	"Extra object modules to link into the vxWorks image, in addition\
	 to the objects produced by the project source files."

set macroDesc(HEX_FLAGS) \
	"Option flags for the program (such as hex, coffHex, or elfHex)\
	that converts a boot program into S-records or the equivalent."
set macroRule(HEX_FLAGS)	rom

# I960 flags for building the IMI (initial memory image) in ROM images.
set macroDesc(ROM_IMI_OFFSET) \
	"Initial Memory Image offset"
set macroRule(ROM_IMI_OFFSET)	rom
set macroCpu(ROM_IMI_OFFSET)	I960
set macroDesc(IMI_DATA_ADRS) \
	"Initial Memory Image Data address"
set macroRule(IMI_DATA_ADRS)	rom
set macroCpu(IMI_DATA_ADRS)	I960
set macroDesc(IMI_TEXT_ADRS) \
	"Initial Memory Image Text address"
set macroRule(IMI_TEXT_ADRS)	rom
set macroCpu(IMI_TEXT_ADRS)	I960
set macroDesc(IMI_SPEC) \
	"XXX - DESCRIPTION"
set macroRule(IMI_SPEC)		rom
set macroCpu(IMI_SPEC)		I960
set macroDesc(IBR) \
	"Initialization Boot Record module."
set macroRule(IBR)		rom
set macroCpu(IBR)		I960

set macroDesc(ARCHIVE) \
	"Application library name."
set macroRule(ARCHIVE) archive

set macroDesc(POST_BUILD_RULE) \
	"Shell commands to execute after the build has completed."
