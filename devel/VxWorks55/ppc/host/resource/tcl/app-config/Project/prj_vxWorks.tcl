# prj_vxWorks.tcl - vxWorks project library
#
# Copyright 1998-2001, Wind River Systems, Inc. Alameda, CA
#
# modification history
# --------------------
# 03p,03feb03,rbl  fix problems parsing headerMacros arising from last fix
# 03o,28jan03,rbl  Fis spr 34798: Project Facility incorrectly transfers
#				   NETIF_USR_ENTRIES from config.h.
# 03n,03jun02,cjs  Fix spr 78255: errors during autoscale
# 03m,13may02,rbl  fix problem where bsps which define WDB_COMM_NETWORK
#                  end up without WDB (SPR 77253)
#                  also fix problem where autoscale didn't realize a source
#                  file had been removed (SPR 75941)
# 03l,02may02,cjs  fix spr 76601 -- archives must be written to makefile
#                  w/o surrounding {}'s supplied by Tcl 
# 03k,25apr02,cjs  Report build errors during autoscale (spr 73344)
# 03j,21mar02,rbl  change flags so that only bsp code gets compiled with
#                  -fvolatile / -Xmemory-is-volatile flag. This flag greatly
#                  reduces the optimization of user code.
# 03i,05feb02,sn   pass TOOL and CPU (if specified) to make headerMacros
# 03h,31jan02,cym  changed regsub while parsing BSP make macros to use and 
#		   explicit return instead of \n. (fixing malt4kc builds)
# 03g,28jan02,sn   added INCLUDE_CPLUS to stripCompsGet (although
#                  this only affects bootroms, which aren't
#                  currently supported ... !)
# 03f,11dec01,rbl  fix bspRulesParse on Windows
#                  make .hex and .bin rules buildable from gui
#                  add separate rom rules when creating from bsp
# 03e,07dec01,rbl  fix error redirection - merge from T3
# 03d,04dec01,j_s  renamed OS_LIBS to VX_OS_LIBS in bspVxOsLibsGet,
#		   this was left out in last checkin; explicitly remove
#		   '{' and '}' around $(VX_OS_LIBS)
# 03c,13nov01,j_s  LIBS now consists of VX_OS_LIBS and other archives. 
#		   VX_OS_LIBS contains all archives that are present 
#		   in LD_LINK_PATH and $(TGT_DIR)/lib/lib$(CPU)$(TOOL)vx.a.
# 03b,06nov01,rbl  add tool-dependent cdf directories
#                  fix LD_LINK_PATH on Windows
# 03a,02nov01,j_s  modified the way MXR_LIBS gets derived and set 
#                  in vxProjCreate; adapted autoscale to it
# 02y,15oct01,rbl  adding functionality to get CPU and TOOL from bsp Makefile,
#                  for use by new project wizard
#                  move archDirFind to tc_cplus.tcl
#                  support typing "make" in project directory for backwards
#                  compatibility
#                  support for building projects with diab and gnu
# 02z,23oct01,t_m  XScale->XSCALE
# 02y,16oct01,cym  adding non-source BOOT_EXTRA (SPR #28765)
# 02x,20sep01,rbl  rename "vendor" to "tool" for consistency
#                  with the makefile TOOL variable
#                  remove absolute paths from Makefile
#                  (merge from T3) removing hack which 
#                  (for Windows only) replaced \'s with /'s in bsp make 
#                  macros
# 02z,05oct01,dat  Gnu-diab infrastructure, added BSP_STUBS support (69150)
# 02y,04oct01,cym  adding non-source MACH_DEP/MACH_EXTRA to EXTRA_MODULES
#		   SPR #27955
# 02x,20sep01,dat  Adding TOOL_FAMILY
# 02w,07sep01,dat  Gnu-Diab infrastructure
# 02v,29aug01,hdn  added PENTIUM2/3/4 support
# 02v,24aug01,pch  Add diagnosis of undefined toolchain
# 02u,13jun01,agf  add logic to check host type on stdout/stderr syntax
# 02u,22may01,pch  Fix regexp to recognize '#include "prjParams.h"' even
#                  if there is space between the '#' and 'include'.
# 02p,31may01,rec  add rule for sysALib.o: $(PRJ_DIR)/prjParams.h
# 02o,04may01,scm  Add STRONGARM support...
# 02t,09may01,agf  undoing last change, @ breaks Win hosts
# 02s,16apr01,agf  fix syntax for redirecting output to stdout/stderr
# 02n,01mar01,t_m  merge in XScale changes
# 02s,01feb01,t_m  adding XScale
# 02n,12may00,zl   added Hitachi SuperH (SH) support
# 02m,16apr99,j_k  back-off from previous fix and make it right.
# 02l,13apr99,j_k  part of fix for SPR #26620.
# 02n,28feb01,sn   write stderr into current dir rather than bspDir
#                  (latter may be read only; this is true for
#                  the host validation test).
# 02r,18jan01,mem  Update MIPS cpu types.
# 02q,17nov00,dh   Adding Coldfire support (merge from 3soft)
# 02p,01nov00,sdk  BSP parser doesn't work correctly if there is no extra
#                  space before "\n". This was seen during FR-V porting. Now
#                  parser addes one extra space before "\n" if it is already
#                  not present.
# 02o,08jun00,yvp  added FRV support
# 02n,01dec99,rsh  add mcore label to register target/comps/vxWorks/arch/mcore
#                  directory
# 02o,10nov99,dra  Added CW400x support.
# 02n,02nov99,dra  Added VR* MIPS and ULTRASPARC processor support.
# 02m,16apr99,j_k  back-off from previous fix and make it right.
# 02l,13apr99,j_k  part of fix for SPR #26620.
# 02k,09mar99,rbl  adding description argument to create
# 02j,25feb99,rbl  Changes to handle missing or read-only projects
# 02i,25feb99,cjs  Add bootrom support for future use 
# 02h,19feb99,ren  Added handling of a parameter with no value
# 02g,19jan99,cjs  make prjBuildRuleListGet() sensitive to bsp
# 02f,17nov98,cjs  changed some references to 'vxWorks' to 'VxWorks' 
# 02e,10nov98,ms   added LINK_DATASYMS handling
# 02d,04nov98,ms   configGen now generates separated arrays for importing
#		   text and data symbols (fix for SIMHP linker).
# 02c,27oct98,ms   reworked LIBS so diab tools can use gnu archives.
# 02b,26oct98,ms   new build interface.
# 02a,15oct98,ms   better error messages thrown if exec fails.
# 01z,09oct98,ms   autoscale now uses mod* routines, has verbose mode.
#		   when parsing BSP Makefile, prepend $bspDir to LIBS.
#		   emit #undefs for excluded components in configGen.
# 01y,09sep98,cjs  call wccValidate() to update error status cache in prj
# 01x,03sep98,ms   make rom*.* build rules visable in the GUI.
# 01w,03sep98,jmp  modified bspRulesParse{} so that it ignores all custom
#                  rules of the form *bootrom*.
# 01v,26aug98,ms   makeGen - component ARCHIVE's now output to Makefile
# 01u,19aug98,ms   INCLUDE_WDB_COMM_NETWORK excludes INCLUDE_WDB_SYS.
# 01t,14aug98,ms   respect INIT_BEFORE adjustments in prjConfig.c generator.
# 01s,03aug98,jmp  replaced dependancies by dependencies.
# 01r,31jul98,ms   conditionally include prjParams.h in BSPs config.h
# 01q,29jul98,ms   update calls to prj[Rel|Full]Path. Code generation rework;
#		   sysConfig.c renamed to prjConfig.c. configGen.h renamed
#		   to prjComps.h and prjParams.h.
# 01p,27jul98,ms   use wccMxrLibs[GS]et to store list of archives to analyze
# 01p,22jul98,ms   handle relative paths in custom BSP rules.
# 01o,20jul98,ms   makeGen only emits one set of COMPONENTS.
#		   bspMakeParse doesn't choke on certain inputs.
#		   parsing macro parameters of type "exists" fixed.
#		   remove project file if project creation doesn't complete.
#		   add extra BSP build flags to assembler (not just cc).
#		   ensure arrays makeMacros and usrRules start empty.
# 01n,14jul98,ms   emit comments in prjConfig.c. Add DO NOT EDIT warning in
#		   all generated files. Emit all configuration parameters.
# 01m,10jul98,jmp  added vpath to the generated Makefile. subsitute $(MAKE) by
#                  $(MAKE) -f $(PRJ_DIR)/Makefile in custom rules.
# 01l,08jul98,ms   fixed bug in bspMakeParse. ran through dos2unix.
# 01l,07jul98,ms   remove network components if INCLUDE_NETWORK is not defined.
#		   add INCLUDE_IP in INCLUDE_NETWORK is defined.
#		   reworked bspMakeParse to be more efficient.
#		   added close hook to clean up dataDocs.
#		   derivedObjsGet now returns only binary files.
#		   changed default buildSpec name to "default".
# 01k,01jul98,cjs  changed project name to reflect usage scenario 
# 01j,01jul98,jmp  added MACH_DEP and MACH_EXTRA to macros filter.
# 01i,29jun98,jmp  speed up the build by making h/make and config/comps the
#                  default read-only dependancy path, this can be change from
#                  the GUI.
#		   added the possility to get user defined BSP Makefile macros.
#		   moved build-configuration macros after defs.project include.
# 01h,25jun98,ms   parse BSP toolchain flag overrides.
#		   add generated files to the project.
#		   each buildspec duilds its objs in a subdir.
#		   reworked derivedObjsGet to return all objs.
#		   various other cleanups.
# 01h,18jun98,ms   added parsing of macro parameters
# 01g,18jun98,jmp  added bspRulesParse{}, added rules support to makeGen{}.
# 01f,05jun98,ms   add usrAppInit.c stub file to all new projects.
# 01e,03jun98,cjs  updated syntax of derivedObjectsGet
# 01d,01jun98,ms   added to imageMacroList; called prj[Status|Warn]Log,
#                  added derivedObjectsGet, buildRule[GS]et
# 01c,22apr98,cjs  wrap load of cxrdoc in debug conditionals
# 01b,03apr98,ms   only generate the Makefile if it is older than the prjFile.
#		   don't save the prjFile to disk before a build.
# 01a,18mar98,ms   written.
#
# DESCRIPTION
# This library adds vxWorks project handling to the generic project
# management library.
# It adds the following things that are specific to vxWorks:
# * Project make-macros like RAM_LOW_ADRS, etc. These macros
#   are stored in the project under the tag VXWORKS_MAKE_MACRO
# * Component configurations. These are stored under the tag
#   VXWORKS_COMPONENTS_<name>.
#


# Call to the boot application for bootroms 
proc bootAppGet {} {
    return {taskSpawn ("tBoot", BOOT_LOADER_PRIORITY, \
	BOOT_LOADER_OPTIONS, BOOT_LOADER_STACK, bootCmdLoop, \
	0,0,0,0,0,0,0,0,0,0);}
}

# List of components that must be stripped for a bootrom
proc stripCompsGet {} {
    set cmps {
	INCLUDE_DEBUG \
	INCLUDE_DOS_DISK \
	INCLUDE_ENV_VARS \
	INCLUDE_FLOATING_POINT \
	INCLUDE_FORMATTED_IO \
	INCLUDE_HW_FP \
	INCLUDE_LOADER \
	INCLUDE_LPT \
	INCLUDE_MMU_BASIC \
	INCLUDE_MMU_FULL \
	INCLUDE_NET_INIT \
	INCLUDE_NTPASSFS \
	INCLUDE_PASSFS \
	INCLUDE_POSIX_AIO \
	INCLUDE_POSIX_MQ \
	INCLUDE_POSIX_SEM \
	INCLUDE_POSIX_SIGNALS \
	INCLUDE_SELECT \
	INCLUDE_SHOW_ROUTINES \
	INCLUDE_SIGNALS \
	INCLUDE_SM_OBJ \
	INCLUDE_SPY \
	INCLUDE_SW_FP \
	INCLUDE_SYM_TBL \
	INCLUDE_STANDALONE_SYM_TBL \
	INCLUDE_NET_SYM_TBL \
	INCLUDE_STAT_SYM_TBL \
	INCLUDE_CTORS_DTORS \
	INCLUDE_CPLUS \
	INCLUDE_CPLUS_MIN \
	INCLUDE_HTTP \
	INCLUDE_RBUFF \
	INCLUDE_WINDVIEW \
	INCLUDE_SHELL \
	INCLUDE_SECURITY \
	INCLUDE_DEMO \
	INCLUDE_UGL \
	INCLUDE_JAVA \
	INCLUDE_HTML \
	INCLUDE_SYM_TBL_SYNC \
	INCLUDE_TIMEX \
	INCLUDE_WDB \
	INCLUDE_WINDMP \
	USER_B_CACHE_ENABLE \
	USER_D_CACHE_ENABLE \
	USER_I_CACHE_ENABLE ;# (for sparc and x86 only ???) \
    }
    return $cmps
}

# List of components that must in a bootrom
proc addCompsGet {} {
    set comps {
	INCLUDE_CACHE_SUPPORT \
	INCLUDE_EXC_HANDLING \
	INCLUDE_IO_SYSTEM \
	INCLUDE_LOGGING \
	INCLUDE_MEM_MGR_FULL \
	INCLUDE_PIPES \
	INCLUDE_USER_APPL \
    }
}


#########################################################################
#
# cpuAndToolFromBspGet - get default CPU and TOOL from a bsp Makefile
#
# This function quickly parses the bsp Makefile. It does not use
# make makeMacros, simply for speed reasons. 
#
# PARAMETERS 
#   bspDir: bsp directory
#
# RETURNS: {<CPU> <TOOL>}, e.g. {PPC603 gnu}
#
proc cpuAndToolFromBspGet {bspDir} {
    set makeFd [open $bspDir/Makefile r]
    set cpu ""
    set tool ""
    while {[gets $makeFd line] >= 0} {
	if {![regexp {^ *include .*} $line]	&& \
	    ![regexp {^ *#.*} $line]		&& \
	    ![regexp {^ +$} $line]		&& \
	    [string length $line] != 0		} {

	    # look for lines of the form "macro = value"
	    if {[regexp {([^=]*)=(.*)} $line dummy macro val] != 1} {
		continue
	    }
	    set macro [string trim $macro]
	    set val   [string trim $val]
	    # make sure "macro" has no spaces (i.e., really a macro definition)
	    if {[regexp {.*[ 	]+.*} $macro] == 1} {
		continue
	    }

            if {$macro == "CPU"} {
                set cpu $val
            }
            if {$macro == "TOOL"} {
                set tool $val
            }
            if {$cpu != "" && $tool != ""} {
                break
            }
	}
    }

    close $makeFd
    return [list $cpu $tool]
}


source  [wtxPath]/host/resource/tcl/app-config/Project/wccLib.tcl

namespace eval ::prj_vxWorks_hidden {

###############################################################################
#
# preciousComponents - return a list of components that should not be removed
# 

proc preciousComponents {} {
    return [list INCLUDE_SYSHW_INIT  INCLUDE_SYS_START INCLUDE_MEM_MGR_BASIC \
		INCLUDE_SYSCLK_INIT INCLUDE_KERNEL]
}

#########################################################################
#
# basedOnSimulator - is this project based on a simulator bsp?
#
# RETURNS: 1 if based on simulator, 0 otherwise
#
# PARAMETERS 
#   hProj: project handle
#
# ERRORS: N / A
#
proc basedOnSimulator {hProj} {
    set simulators [list hpux solaris simpc]
    set bsp [file tail [prjTagDataGet $hProj BSP_DIR]]
    if {[lsearch $simulators $bsp] == -1} {
        return 0
    } else {
        return 1
    }
}

###############################################################################
#
# autoscale - return info on how to scale vxWorks to the application
#
# RETURNS: a list of required INCLUDES
#
#
# The verbose mode option helps track down why extra components
# are required by the application modules.
# Verbose mode returns a list of lists. The first list is
# the required INCLUDES.
# The subsequent lists are of the form:
#	{appModule libSymsExtra libModulesExtra componentsExtra}
# Where appModule is an application object module.
# libSymsExtra is a list of symbols it imports from the archive, which
# would not otherwise be pulled in by the component configuration.
# libModulesExtra is the set of modules in the archive that are linked
# in, which would not otherwize be pulled in by the component configuration.
# componentsExtra is the set of extra components being pulled in.
#
# set throwErrors <> "" to cause any build errors to throw an error
#

proc autoscale {hProj {verbose ""} {throwErrors ""}} {
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set build  [prjBuildCurrentGet $hProj]
    set wcc    [wccCurrentGet $hProj]
    set tc     [prjBuildTcGet $hProj $build]
    set libs   [concat [wccMxrLibsGet $hProj] \
		       [prjBuildMacroGet $hProj $build LIBS]]
    set libs   [mxrLibsExpand $hProj $libs $tc]

    # build project objects and analyze them with the libraries

    prjStatusLog "compiling project objects..."

    if {[catch {prjBuild $hProj partialImage.o "" complain} errorMsg]} {
        if {$throwErrors != ""} {
            error $errorMsg 
        }
    }

    foreach obj [lindex [objsGet $hProj] 0] {
        lappend objs $prjDir/$build/$obj
    }

    regsub -all {[^ ]*((prjConfig.o)|(linkSyms.o)|(partialImage.o))} \
			$objs {} objs

    set objs "$objs [prjBuildMacroGet $hProj $build EXTRA_MODULES]"

    prjStatusLog "analyzing module dependencies..."
    set modDataDoc [modAnalyze "$objs $libs" [${tc}::name]]

    # compute modules and components requires by project objects

    set mxrDoc [mxrDocCreate $hProj]
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    $cxrSet = [set modSubtree [modSubtree $modDataDoc $objs]]
    $cxrSet = "[$cxrSet get _MODULES] [preciousComponents]"
    set includes [cxrSubtree $cxrSet $mxrDoc]

    # in verbose mode, tell why these components are included

    if {"$verbose" == "verbose"} {
	set mxrSet [$mxrDoc setCreate]
	$cxrSet = [wccComponentListGet $hProj $wcc]
	$mxrSet = [cSymsMangle \
		"[$cxrSet get LINK_SYMS] [$cxrSet get LINK_DATASYMS]" $tc]
	$mxrSet = [$mxrSet get exportedBy]
	$mxrSet = [$mxrSet + [$cxrSet get MODULES]]
	set componentModules [mxrSubtree $mxrSet]
	$mxrSet = $componentModules
	set librarySyms "[$mxrSet get exports] [$mxrSet get declares]"
	set includes [list $includes]

	foreach usrMod $objs {
	    $mxrSet = [modInfo $modDataDoc $usrMod imports]
	    $mxrSet = [$mxrSet - $librarySyms]
	    set syms [lsort [$mxrSet instances Symbol]]
	    if {"$syms" == ""} {
		continue
	    }
	    $mxrSet = [modCalls $modDataDoc $usrMod]
	    set definingModules [$mxrSet - "$objs $componentModules"]
	    if {"$definingModules" == ""} {
		continue
	    }
	    $mxrSet = [modSubtree $mxrDoc $definingModules]
	    $mxrSet = [$mxrSet - $componentModules]
	    set modules [lsort [$mxrSet instances Module]]
	    $cxrSet = $modules
	    $cxrSet = [$cxrSet get _MODULES]
	    $cxrSet = [$cxrSet - [wccComponentListGet $hProj $wcc]]
	    set components [lsort [$cxrSet instances Component]]
	    lappend includes [list $usrMod $syms $modules $components]
	}

	$mxrSet delete
	$modDataDoc delete
    }

    $cxrSet delete
    return $includes
}

###############################################################################
#
# macrosToComponents - translate old INCLUDE_* macros to components
# 
proc macrosToComponents {macros hProj} {
    set cmpLib [cxrDocCreate $hProj]
    set cmpSet [$cmpLib setCreate]

    # remove components with missing nested macros (e.g., INCLUDE_AOUT
    # is not included unless it's nested macro, INCLUDE_LOADER, is
    # also defined.

#    This is faster, but requires MACRO_NEST be an association
#    set cmpSet2 [$cmpLib setCreate]
#    $cmpSet = $macros
#    $cmpSet2 = [$cmpSet get _MACRO_NEST]
#    $cmpSet2 = [$cmpSet2 - $macros]
#    $cmpSet2 = [$cmpSet2 get MACRO_NEST]
#    puts "XXX - removing nested components [$cmpSet2 contents]"
#    set components [$cmpSet - $cmpSet2]
#    $cmpSet2 delete
    set components ""
    foreach macro $macros {
        # skip if nested

        $cmpSet = $macro
        set nestedMacros [$cmpSet get MACRO_NEST]
        if {"$nestedMacros" != ""} {
            set included true
            foreach nestedMacro $nestedMacros {
                if {[lsearch $macros $nestedMacro] == -1} {
                    set included false
                    break
                }
            }
            if {"$included" == "false"} {
                continue
            }
        }
        lappend components $macro
    }

    # add in new components as needed

    set components [concat $components [preciousComponents]]

    if {[lsearch $components INCLUDE_SHELL] != -1} {
	lappend components INCLUDE_SHELL_BANNER
    }

    if {[set ix [lsearch $components INCLUDE_ANSI_ALL]] != -1} {
	set components [lreplace $components $ix $ix]
	set components [concat $components 		\
			INCLUDE_ANSI_ASSERT    		\
			INCLUDE_ANSI_CTYPE      	\
			INCLUDE_ANSI_LOCALE     	\
			INCLUDE_ANSI_MATH       	\
			INCLUDE_ANSI_STDIO      	\
			INCLUDE_ANSI_STDLIB     	\
			INCLUDE_ANSI_STRING     	\
			INCLUDE_ANSI_TIME		\
			INCLUDE_ANSI_STDIO_EXTRA]
    }
	
    if {[set ix [lsearch $components INCLUDE_POSIX_ALL]] != -1} {
	set components [lreplace $components $ix $ix]
	set components [concat $components 		\
			INCLUDE_POSIX_AIO 		\
			INCLUDE_POSIX_AIO_SYSDRV	\
			INCLUDE_POSIX_FTRUNC		\
			INCLUDE_POSIX_MEM		\
			INCLUDE_POSIX_MQ		\
			INCLUDE_POSIX_SCHED		\
			INCLUDE_POSIX_SEM		\
			INCLUDE_POSIX_SIGNALS		\
			INCLUDE_POSIX_TIMERS		\
			INCLUDE_POSIX_CLOCKS]
    }

    if {[set ix [lsearch $components INCLUDE_CONFIGURATION_5_2]] != -1} {
	set components [lreplace $components $ix $ix]
	set components [concat $components	\
		INCLUDE_LOADER 			\
		INCLUDE_NET_SYM_TBL		\
		INCLUDE_RLOGIN			\
		INCLUDE_SHELL			\
		INCLUDE_SHOW_ROUTINES		\
		INCLUDE_SPY			\
		INCLUDE_STARTUP_SCRIPT		\
		INCLUDE_STAT_SYM_TBL		\
		INCLUDE_SYM_TBL			\
		INCLUDE_TELNET			\
		INCLUDE_UNLOADER		\
		INCLUDE_DEBUG]
    }

    if {([lsearch $macros INCLUDE_TTY_DEV] != -1) && \
	([lsearch $macros INCLUDE_TYCODRV_5_2] == -1)} {
	lappend components INCLUDE_SIO
    }

    # remove obsolete INCLUDE_* macros

    set obsolete {(INCLUDE_RDB)|(INCLUDE_DELETE_5_0)|(INCLUDE_ANSI_5_0)|(INCLUDE_SEM_OLD)|(INCLUDE_CPLUS_BOOCH)|(INCLUDE_CPLUS_HEAP)|(INCLUDE_MC68881)|(INCLUDE_SPARC_FPU)|(INCLUDE_R3010)|(INCLUDE_PPC_FPU)|(INCLUDE_I80387)}

    regsub -all $obsolete $components "" components

    # warn user of any unknown INCLUDE_* macros still left

    $cmpSet = $components
    set unknowns [$cmpSet - [$cmpSet instances]]
    if {[llength $unknowns] > 0} {
	prjWarnLog "ignoring unknown components $unknowns"
    }

    if {[$cmpSet & INCLUDE_CACHE_SUPPORT] == "INCLUDE_CACHE_SUPPORT"} {
	    lappend components INCLUDE_CACHE_ENABLE
    }

    set components [$cmpSet instances]
    lappend components INCLUDE_BOOT_LINE_INIT

    # remove all networking subcomponents if INCLUDE_NETWORK is not defined.
    # add INCLUDE_IP (new) if INCLUDE_NETWORK is defined.

    set cmps [$cmpSet setCreate]
    if {[$cmpSet & INCLUDE_NETWORK] != "INCLUDE_NETWORK"} {
	$cmps = FOLDER_NETWORK
	$cmps = [cxrSubfolders $cmps]
	set components [$cmpSet - [$cmps instances Component]]
    } else {
	if {[$cmpSet & INCLUDE_WDB_COMM_NETWORK] == \
			"INCLUDE_WDB_COMM_NETWORK"} {
	    $cmps = INCLUDE_WDB_SYS
	    set mxrDoc [mxrDocCreate $hProj]
            $cmps = [cxrSupertree $cmps $mxrDoc $components]
	    set components [$cmpSet - [$cmps instances Component]]
	}
	if {[$cmpSet & INCLUDE_END] != "INCLUDE_END"} {
	    lappend components INCLUDE_BSD
	}
	set components [concat $components [list INCLUDE_IP \
		INCLUDE_LOOPBACK INCLUDE_ARP_API INCLUDE_NET_HOST_SETUP]]
    }

    $cmps delete
    $cmpSet delete

    return $components
}


###############################################################################
#
# bspVxOsLibsGet - get macro VX_OS_LIBS from bsp
#

proc bspVxOsLibsGet {hProj bspDir {cpuType ""} {toolType ""}} {
    variable makeMacrosReferred

    # use cached information if possible

    if [info exists makeMacrosReferred($hProj,VX_OS_LIBS)] {
	return $makeMacrosReferred($hProj,VX_OS_LIBS)
    }

    set makeCmd "make -C $bspDir makeMacrosReferred BSP2PRJ=TRUE"
    if {$cpuType != ""} {
	set makeCmd "$makeCmd CPU=$cpuType"
    }
    if {$toolType != ""} {
	set makeCmd "$makeCmd TOOL=$toolType"
    }

    #
    # If any of the exec'd commands writes to standard error 
    # (e.g. a compiler warning) and standard error isn't redirected, 
    # then exec will return an error. To avoid this we use 
    # "2>@ stderr", which redirects standard error to 
    # standard error. This has no effect other than to prevent 
    # exec from returning an error on e.g. a compiler warning
    #
    
    if {[catch {set lines "[eval exec $makeCmd 2>@ stderr]"} \
		 err]} {
	catch {eval exec $makeCmd} err
 	error "$err\n\"$makeCmd\" failed"
    }

    foreach line [split $lines \n] {
	if {[regsub {[^=]*=} $line {} val] != 1} {
	    continue
	}
	set macro [lindex $line 1]
	set val   [string trim $val]
	::filePathFix val
	set makeMacrosReferred($hProj,$macro) $val
    }

    if ![info exists makeMacrosReferred($hProj,VX_OS_LIBS)] {
	set makeMacrosReferred($hProj,VX_OS_LIBS) ""
    }

    return $makeMacrosReferred($hProj,VX_OS_LIBS)
}

###############################################################################
#
# bspMakeParse - parse a BSP Makefile
#

proc bspMakeParse {bspDir {cpuType ""} {toolType ""} } {
    variable makeMacros

    if {[info exists makeMacros]} {
	unset makeMacros
    }

    set makeCmd "make -C $bspDir makeMacros BSP2PRJ=TRUE"
    if {$cpuType != ""} {
	set makeCmd "$makeCmd CPU=$cpuType"
    }
    if {$toolType != ""} {
	set makeCmd "$makeCmd TOOL=$toolType"
    }

    #
    # If any of the exec'd commands writes to standard error 
    # (e.g. a compiler warning) and standard error isn't redirected, 
    # then exec will return an error. To avoid this we use 
    # "2>@ stderr", which redirects standard error to 
    # standard error. This has no effect other than to prevent 
    # exec from returning an error on e.g. a compiler warning
    #
    
    if {[catch {set lines "[eval exec $makeCmd 2>@ stderr]"} \
		 err]} {
	catch {eval exec $makeCmd} err
 	error "$err\n\"$makeCmd\" failed"
    }

    foreach line [split $lines \n] {
	if {[regsub {[^=]*=} $line {} val] != 1} {
	    continue
	}
	set macro [lindex $line 1]
	set val   [string trim $val]
	if {"$macro" == "LIBS"} {
		# convert relative paths
                ::filePathFix val
		set libs ""
		foreach lib $val {
		    if {[file exists $bspDir/$lib]} {
			set lib $bspDir/$lib
		    }
		    lappend libs $lib
		}
		set makeMacros(LIBS) $libs
		continue
	}
	if {"$macro" == "LD_LINK_PATH"} {
	    # fix paths
            ::filePathFix val
	    set makeMacros(LD_LINK_PATH) $val
	    continue
	}
	set makeMacros($macro) $val
    }

    # Macro list that we do not want to add to project
    set macroToSkip "CPU TOOL TOOL_FAMILY BOARD TGT_DIR SED RELEASE TARGET_DIR VENDOR LD_LOW_FLAGS"

    set makeFd [open $bspDir/Makefile r]
    set switchLowHigh ""
    while {[gets $makeFd line] >= 0} {
	if {![regexp {^ *include .*} $line]	&& \
	    ![regexp {^ *#.*} $line]		&& \
	    ![regexp {^ +$} $line]		&& \
	    [string length $line] != 0		} {

	    # look for lines of the form "macro = value"
	    if {[regexp {([^=]*)=(.*)} $line dummy macro val] != 1} {
		continue
	    }
	    set macro [string trim $macro]
	    set val   [string trim $val]
	    # make sure "macro" has no spaces (i.e., really a macro definition)
	    if {[regexp {.*[ 	]+.*} $macro] == 1} {
		continue
	    }
	    # concatanate lines ending in a backslash
	    while {"[lindex $val end]" == "\\"} {
		set val [string trimright $val "\\"]
		gets $makeFd line
		set val "$val [string trim $line]"
	    }

	    if {"$macro" == "LD_LOW_FLAGS"} {
		if {"$val" == "LD_HIGH_FLAGS"} {
		    set switchLowHigh true
		    continue
		}
		set makeMacros(LD_RAM_FLAGS) $val
		continue
	    }
	    if {"$macro" == "LIBS"} {
		# convert relative paths
                ::filePathFix val
		set libs ""
		foreach lib $val {
		    if {[regexp {([\/].)|(.:)} [string range $lib 0 1]] == 0} {
			set lib $bspDir/$lib
		    }
		    lappend libs $lib
		}
		set makeMacros(LIBS) $libs
		continue
	    }
	    if {"$macro" == "LD_LINK_PATH"} {
		# fix paths
                ::filePathFix val
		set makeMacros(LD_LINK_PATH) $val
		continue
	    }
	    if {[lsearch $macroToSkip $macro] == -1} {
	        set makeMacros($macro) $val
	    }
	}

	if  {"$switchLowHigh" != ""} {
	    set switchLowHigh ""
	    set low $makeMacros(RAM_LOW_ADRS)
	    set makeMacros(RAM_LOW_ADRS) $makeMacros(RAM_HIGH_ADRS)
	    set makeMacros(RAM_HIGH_ADRS) $low
	}
    }

    close $makeFd
}

###############################################################################
#
# bspMakeGet - get info on a BSP make-macro
#

proc bspMakeGet {{macro ""}} {
    variable makeMacros

    if {"$macro" == ""} {
	return [array names makeMacros]
    }

    if {![info exists makeMacros($macro)]} {
	return ""
    }

    return "$makeMacros($macro)"
}

###############################################################################
#
# bspFilesGet - get the set of files in a BSP
#
# RETURNS: a list of three elements, the first containing the BSP source
# files, the second containing the BSP boot files, and the third containing
# other BSP objects of unknown type
#
# INTERNAL
# Care must be taken to put the first file in MACH_DEP (typically sysALib.o)
# as the first project file.
# The reason is that the project Make system needs to put that file
# first on the final link line.
#

proc bspFilesGet bspDir {
    set files ""
    set bootFiles ""
    set otherObjs ""
    set otherBootObjs ""

    set bspFiles [glob $bspDir/*.c $bspDir/*.s $bspDir/*.cpp $bspDir/*.cxx]
    set bspObjs  [bspMakeGet MACH_DEP]
    set bootObjs [bspMakeGet BOOT_EXTRA]
    set firstObj [lindex $bspObjs 0]

    foreach file $bspFiles {
	set obj [file rootname [file tail $file]].o
	if {[lsearch $bspObjs $obj] != -1} {
	    if {$obj == $firstObj} {
		set files [concat $file $files]
	    } else {
		lappend files $file
	    }
	    regsub $obj $bspObjs "" bspObjs
	    if {[lsearch $bootObjs $obj] != -1} {
		lappend otherBootObjs $obj
	        regsub $obj $bootObjs "" bootObjs
	    }
	    continue
	}
	if {[lsearch $bootObjs $obj] != -1} {
	    lappend bootFiles $file
	    regsub $obj $bootObjs "" bootObjs
	    continue
	}
    }

    set objs [string trim [concat $bspObjs]]
    foreach obj $objs {
	lappend otherObjs $bspDir/$obj
    }

    set objs [string trim [concat $bootObjs]]
    foreach obj $objs {
	lappend otherBootObjs $bspDir/$obj
    }

    return [list $files $bootFiles $otherObjs $otherBootObjs]
}


###############################################################################
#
# bspRulesParse - parse a BSP Makefile for user defined rules
#
proc bspRulesParse {bspDir} {
    variable usrRules                   ;# rules array
    if {[info exists usrRules]} {
	unset usrRules
    }

    set makefile "$bspDir/Makefile"     ;# Makefile to parse
    set targetList ""                   ;# list of rules found in Makefile
    set findRule 0                      ;# flag indicating if we found a rule
    set incompleteLine 0                ;# flag indicating that current line
                                         # is not complete
    set ruleToSkip ".*bootrom.*"	;# ignores *bootrom* rules
    set addCurrentRule 1		;# add current rule (1) or skip it (0)

    # open file to parse
    set makeFd [open $makefile r]

    while {[gets $makeFd line] >= 0} {

        #
        # Skip include lines, macros lines, comments lines and blank lines
        #
        if {([regexp "^ *include .*"    $line]  || \
             [regexp "^.*=.*$"          $line]  || \
             [regexp "^ *#.*"           $line]  || \
             [regexp "(^ +$|^$)"        $line]) &&  $findRule == 0 } {

            # nothing to do

        } elseif {[regexp "^\[^\t\].*:.*$" $line]} {

            # We find a new rule if line match "target : dependencies"

            set findRule 1
            set target [lindex [split $line \;] 0]
            set targetName [lindex [split $target \:] 0]
            regsub " *$" $targetName "" targetName

	    if {[regexp $ruleToSkip $targetName]} {
	    	set addCurrentRule 1 ; # always add current rule
	    } else {
	    	set addCurrentRule 1
	    }

	    if {$addCurrentRule} {
		lappend targetList $targetName
		lappend usrRules($targetName) $target
	    }

            # check if current line complete is ended by "\"
            # which means "to be continued"
            if {[regexp {^.*\\ *$} $target]} {
                set incompleteLine 1
            } else {
                set incompleteLine 0
            }

            #
            # the first commands may appear on the same line than
            # the dependencies, with a semicolon as separator:
            #
            # targets : dependencies ; command1 ; command2
            #           command3
            #           ...
            #
            set firstCommands [split $line \;]
            set firstCommands [lrange $firstCommands 1 end]

            if {$firstCommands != ""} {

                foreach command $firstCommands {
                    regsub "(^\t*|^ *)" $command "" command
		    regsub -all {\$\(MAKE\)} $line \
		    	"\$(MAKE) -f \$(PRJ_DIR)/Makefile" line

		    if {$addCurrentRule} {
			lappend usrRules($targetName) $command
		    }
                }
            }

        } elseif {$incompleteLine} {

            regsub "(^\t*|^ *)" $line "" line
	    regsub -all {\$\(MAKE\)} $line \
		"\$(MAKE) -f \$(PRJ_DIR)/Makefile" line

	    if {$addCurrentRule} {
		lappend usrRules($targetName) $line
	    }

            if {![regexp {^.*\\ *$} $line]} {
                set incompleteLine 0
            }

        } elseif {$findRule && $line != "" && [regexp "^\t+.*" $line]} {
            # if line is not null and start by a tabulation
            regsub "(^\t*|^ *)" $line "" line
	    regsub -all {\$\(MAKE\)} $line \
		"\$(MAKE) -f \$(PRJ_DIR)/Makefile" line
            
	    if {$addCurrentRule} {
		lappend usrRules($targetName) $line
	    }
        }
    }

    close $makeFd

    # if rule references BSP source files, prepend $bspDir

    set bspSources [glob -nocomplain $bspDir/*.s $bspDir/*.c \
	$bspDir/*.cpp $bspDir/*.cxx $bspDir/*.h]
    foreach target $targetList {
	foreach source $bspSources {
	    set source [file tail $source]
	    regsub -all "\[ \t\]$source" $usrRules($target) \
		" $bspDir/$source" usrRules($target)
	}
    }

    foreach target $targetList {
        foreach line $usrRules($target) {
            prjStatusLog $line
        }
    }

    return $targetList
}

###############################################################################
#
# bspRulesGet - parse a BSP Makefile for user defined rules
#
proc bspRulesGet {{rule ""}} {
    variable usrRules           ;# rules array

    if {"$rule" == ""} {
        return [array names usrRules]
    }

    if {![info exists usrRules($rule)]} {
        error "no such rule $rule"
    }

    return "$usrRules($rule)"
}

###############################################################################
#
# bspDirValid - check if a BSP directory is valid
# 

proc bspDirValid bspDir {
    if {(![file isdirectory $bspDir]) || \
            (![file readable $bspDir/sysLib.c])} {
        return 0
    }
    return 1
}

###############################################################################
#
# vxProjCreate - create a vxWorks project from a BSP
# 

proc vxProjCreate {prjFile bspDir {prjBootromFile ""} {description ""} {cpuType ""} {toolType ""}} {

    if {![prjCreateable $prjFile ::prj_vxWorks]} {
        error "can't create project $prjFile"
    }
    if {$prjBootromFile != ""} {
    	if {![prjCreateable $prjFile ::prj_vxWorks]} {
            error "can't create project $prjBootromFile"
    	}
    }
    if {![bspDirValid $bspDir]} {
        error "not a BSP directory: $bspDir"
    }

    # parse the BSP's headers and Makefile

    prjStatusLog "parsing the BSP Makefile..."

    bspMakeParse $bspDir $cpuType $toolType
    prjStatusLog "extract user defined rules from BSP Makefile..."
    bspRulesParse $bspDir
    set vxWorksRules [bspRulesGet ""]
    prjStatusLog "parsing the BSP's config.h..."
    set fd [open $bspDir/config.h r]
    set buf [read $fd]
    close $fd
    if {[regexp {#[ 	]*include[ 	]*"prjParams.h.*} $buf] == 0} {
	set tail "#if defined(PRJ_BUILD)\n#include \"prjParams.h\"\n#endif"
	puts "Modifying $bspDir/config.h. Appending\n$tail"
	set fd [open $bspDir/config.h w+]
	puts $fd "$buf\n$tail"
	close $fd
    }

    set makeCmd "make -C $bspDir headerMacros BSP2PRJ=TRUE"
    if {$cpuType != ""} {
	set makeCmd "$makeCmd CPU=$cpuType"
    }
    if {$toolType != ""} {
	set makeCmd "$makeCmd TOOL=$toolType"
    }

    #
    # If any of the exec'd commands writes to standard error 
    # (e.g. a compiler warning) and standard error isn't redirected, 
    # then exec will return an error. To avoid this we use 
    # "2>@ stderr", which redirects standard error to 
    # standard error. This has no effect other than to prevent 
    # exec from returning an error on e.g. a compiler warning
    #

    if {[catch {set lines "[eval exec $makeCmd 2>@ stderr]"} \
		 err]} {
	catch {eval exec $makeCmd} err
 	error "$err\n\"$makeCmd\" failed"
    }

    # extract all the key info from the BSP

    prjStatusLog "creating the project file..."
    set cpu   [bspMakeGet CPU]
    set tool  [bspMakeGet TOOL]
    set files [bspFilesGet $bspDir]
    set prjFiles [lindex $files 0]
    set prjBootFiles [lindex $files 1]
    set otherObjs [lindex $files 2]
    set otherBootObjs [lindex $files 3]

    # create an empty project
    set hProj [prjCoreCreate $prjFile ::prj_vxWorks]
    set cmpDir [wtxPath]target/config/comps
    prjTagDataSet $hProj BSP_DIR $bspDir
    set prjDir     [file dirname $prjFile]

	# set the description
	::prjTagDataSet $hProj userComments $description

    # create a default build spec

    prjStatusLog "creating a default build spec for $cpu $tool ..."
    set tc [lindex [prjTcListGet $cpu $tool C++] 0]
    if {"$tc" == ""} {
	error "${cpu}$tool not defined.\nCheck \$WIND_BASE/host/resource/tcl/app-config/Project/tcInfo_*"
    }
    set cc [prjRelPath $hProj [${tc}::toolFindByFile foo.c]]
    set as [prjRelPath $hProj [${tc}::toolFindByFile foo.s]]
    set build [prjBuildCreate $hProj default $tc]
    set cflags [prjBuildFlagsGet $hProj $build $cc]
    set asflags [prjBuildFlagsGet $hProj $build $as]
    prjBuildFlagsSet $hProj $build $cc "$cflags [bspMakeGet EXTRA_CFLAGS]"
    prjBuildFlagsSet $hProj $build $as "$asflags [bspMakeGet EXTRA_CFLAGS]"

    # Macro list that must not appear in the Macro Editor GUI
    set macroToSkip [list CC CFLAGS AS CFLAGS_AS CXX C++FLAGS LD LDFLAGS \
    	EXTRA_CFLAGS NM SIZE AR CPU TOOL TOOL_FAMILY MACH_DEP MACH_EXTRA \
	LIB_EXTRA EXTRA_DEFINE]

    # Get whole macro list
    set macroList [bspMakeGet]

    foreach macro $macroList {
	if {[lsearch $macroToSkip $macro] == -1} {
	    set val [bspMakeGet $macro]
	    if {"$val" != ""} {

		# remove the '{' and '}' around $(VX_OS_LIBS)

		regsub -all {\{\$\(VX_OS_LIBS\)\}} $val "\$\(VX_OS_LIBS\)" val
	    	prjBuildMacroSet $hProj $build $macro $val
	    }
	}
    }

    set extraModulesDefinition [prjBuildMacroGet $hProj $build EXTRA_MODULES]
    set extraModulesDefinition [concat $extraModulesDefinition $otherObjs]
    prjBuildMacroSet $hProj $build EXTRA_MODULES $extraModulesDefinition
    if {$otherObjs != ""} {
        prjStatusLog "adding source-less BSP objects $otherObjs to EXTRA_MODULES."
    }

    prjBuildMacroSet $hProj $build BOOT_EXTRA $otherBootObjs
    if {$otherObjs != ""} {
        prjStatusLog "adding source-less BSP objects $otherBootObjs to BOOT_EXTRA."
    }

    foreach rule $vxWorksRules {
        set val [bspRulesGet $rule]
        if {"$val" != ""} {
            prjBuildRuleSet $hProj $build $rule $val
        }
    }

    # create a component configuration
    # XXX - create a .cdf file for all unknown BSP INCLUDE_* macros

    prjStatusLog "creating a component configuration..."
    set archDir [archDirFind $cpu]
    set cdfPath [list $cmpDir/vxWorks $cmpDir/vxWorks/arch/$archDir \
        $cmpDir/vxWorks/tool/\$(TOOL_FAMILY) \
	$bspDir [file dirname $prjFile]]
    set wcc [file tail $bspDir]
    wccCreate $hProj $wcc
    wccCdfPathSet $hProj $cdfPath

    wccMxrLibsSet $hProj ""
    prjStatusLog "calculating included components..."
    set tmpl [split $lines "\n"]
    foreach l $tmpl {

        # During FR-V arch port it was seen that, bsp parser will not
        # parse the output of "make headerMacros" correctly if an extra
        # space is not present at the end of line.
        #
        # Somehow previous pre-processors were adding an extra space at the
        # end of the line before "\n". And a new one does not do it.
        # 
        # Because of this, this code was added to put a space before "\n"
        # if it is not present.
        #
        # Note !! This code affects all the architectures. 
        #

        if { [string index $l [expr  [string length $l] -1]] != " "} {
            append l " "
        }

        # read macros of form "#define macro defn" and
        # "#define macro(arg1, arg2, arg3) defn" into macros array

        regsub -all {(#define[ 	]+)([^ 	
\(]+(\([^\)]*\))?)([^
]*)} $l {\2} macroName
        regsub -all {(#define[ 	]+)([^ 	
\(]+(\([^\)]*\))?)([^
]*)} $l {\4} macros($macroName)
       
    }

    # convert some macros to new component names
    switch [string trim $macros(WDB_MODE)] {
	WDB_MODE_TASK	{set macros(INCLUDE_WDB_TASK) 1}
	WDB_MODE_EXTERN	{set macros(INCLUDE_WDB_SYS) 1}
	WDB_MODE_DUAL	{set macros(INCLUDE_WDB_TASK) 1
			 set macros(INCLUDE_WDB_SYS) 1}
    }
    set macros(INCLUDE_[string trim $macros(WDB_COMM_TYPE)]) 1
    set d [cxrDocCreate $hProj]
    set s [$d setCreate]
    $s = [array names macros]
    set includeMacros [array names macros INCLUDE*]
    set components [macrosToComponents $includeMacros $hProj]
    wccComponentAdd $hProj $wcc $components
    wccComponentAdd $hProj $wcc INCLUDE_USER_APPL
    wccComponentAdd $hProj $wcc [wccDependsGet $hProj $wcc]
    prjStatusLog "calculating parameter values..."
    foreach param [$s instances Parameter] {
	if {"[wccParamTypeGet $hProj $param]" == "exists"} {
	    set macros($param) TRUE
	}
		wccParamSet $hProj $wcc $param [string trim $macros($param)]
    }
    $s delete

    # add files to project

    prjStatusLog "adding files to project..."
    foreach file $prjFiles {
        prjFileAdd $hProj $file
        bspFileFlagsSet $hProj $build $file 
    }
    foreach file $prjBootFiles {
        prjFileAdd $hProj $file
        prjFileInfoSet $hProj $file BOOT_FILE TRUE
        bspFileFlagsSet $hProj $build $file 
    }
    if {[file exists $bspDir/romInit.s]} {
	prjFileAdd $hProj $bspDir/romInit.s
	prjFileInfoSet $hProj $bspDir/romInit.s customRule romInit.o
	prjFileInfoSet $hProj $bspDir/romInit.s BOOT_FILE TRUE
	prjBuildRuleSet $hProj $build romInit.o \
	 [list {romInit.o : } \
	 {$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS_AS) \
	  $(PROJECT_BSP_FLAGS_EXTRA) $(ROM_FLAGS_EXTRA) $< -o $@}]

	prjFileAdd $hProj $cmpDir/src/romStart.c
	prjFileInfoSet $hProj $cmpDir/src/romStart.c customRule romStart.o
	prjFileInfoSet $hProj $cmpDir/src/romStart.c BOOT_FILE TRUE
	prjBuildRuleSet $hProj $build romStart.o \
	  [list {romStart.o : } \
	  {$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS) \
	  $(PROJECT_BSP_FLAGS_EXTRA) $(ROM_FLAGS_EXTRA) $< -o $@}]
    }
    set fd [open [wtxPath]target/config/comps/src/stubAppInit.c r]
    set buf [read $fd]
    close $fd
    set fd [open $prjDir/usrAppInit.c w+]
    puts $fd $buf
    close $fd
    prjFileAdd $hProj $prjDir/usrAppInit.c
    prjStatusLog "generating configuration files..."
    ::prj_vxWorks_hidden::configGen $hProj
    prjFileAdd $hProj $prjDir/prjConfig.c
    bspFileFlagsSet $hProj $build $prjDir/prjConfig.c 
    prjFileAdd $hProj $prjDir/linkSyms.c
    bspFileFlagsSet $hProj $build $prjDir/linkSyms.c 
    prjStatusLog "computing file dependencies..."
    prjFileBuildInfoGen $hProj

    # Validate the component set and cache the status 

    prjStatusLog "Validating the configuration..."
    ::wccValidate $hProj $wcc
    prjStatusLog "done"

    # add the other build specs: rom, romCompress, romResident
    # don't add them for the simulators - they don't make sense
    if {![basedOnSimulator $hProj]} {
        prjBuildCopy $hProj default default_rom
        prjBuildRuleCurrentSet $hProj default_rom vxWorks_rom

        prjBuildCopy $hProj default default_romCompress
        prjBuildRuleCurrentSet $hProj default_romCompress vxWorks_romCompress

        prjBuildCopy $hProj default default_romResident
        prjBuildRuleCurrentSet $hProj default_romResident vxWorks_romResident

        # restore current build to default
        prjBuildCurrentSet $hProj default
    }

    # close the project to save all the info

    prjStatusLog "saving the project..."
    prjClose $hProj SAVE

    # If a bootrom project was also requested, create it now
    if {$prjBootromFile != ""} {
        prjStatusLog "creating boot rom project..."

	# Copy the bootable project
	set hProj [prjOpen $prjFile "r"]
	::prjCopy $hProj $prjBootromFile
	prjClose $hProj

	# Open the new copy for manipulation
	set hProj [prjOpen $prjBootromFile "r"]
	set hSpec [prjBuildCurrentGet $hProj]

	# Set the extended project type
        prjTagDataSet $hProj CORE_INFO_SUBTYPE bootrom

	# Add a comment
	set description "Boot Rom image for '$prjFile'"
	::prjTagDataSet $hProj userComments $description

	# Remove the components we don't want
	set wcc [wccCurrentGet $hProj]
	wccComponentRemove $hProj $wcc [stripCompsGet]

	# Add the components we require
	wccComponentAdd $hProj $wcc [addCompsGet]

	# Define the USR_APPL_INIT macro to start the boot app
	prjBuildMacroSet $hProj $hSpec USR_APPL_INIT [bootAppGet] 

	# Done!
	prjClose $hProj SAVE 
    }

    prjStatusLog "done"
}

###############################################################################
#
# bspFileFlagsSet - set up a custom rule to compile file with bsp flags
# 
# Basically this is to add -fvolatile (gnu)
# or -Xmemory-is-volatile (diab) to bsp files, where they
# are necessary if there are poorly written drivers. They
# are not necessary and in fact bad for optimization in
# application code. 
#
proc bspFileFlagsSet {hProj build file} {
    set tc [prjBuildTcGet $hProj $build]
    set objs [prjFileInfoGet $hProj $file objects]
    set tool [${tc}::toolFindByFile $file]
    set toolMacro [${tool}::toolMacro]
    set flagsMacro [${tool}::flagsMacro]
    prjFileInfoSet $hProj $file customRule $objs
    set rule [list "$objs : " \
     "$($toolMacro) $(OPTION_OBJECT_ONLY) $($flagsMacro) \
      $(PROJECT_BSP_FLAGS_EXTRA) $file -o $@"]
    prjBuildRuleSet $hProj $build $objs $rule
}

###############################################################################
#
# makeGen - generate a Makefile for a vxWorks project
#

proc makeGen {hProj Makefile} {
    set prjFile [prjInfoGet $hProj fileName]
    set hSpec [prjBuildCurrentGet $hProj]
    set builds [prjBuildListGet $hProj]
    set fd [open $Makefile w+]
    fconfigure $fd -translation lf
    set prjFiles [prjFileListGet $hProj]
    set objs [objsGet $hProj]
    set prjObjs [lindex $objs 0] 
    set bootObjs [lindex $objs 1]
    set bspDir [prjTagDataGet $hProj BSP_DIR]

    puts $fd "# Makefile generated by the project manager\n#\n"
    puts $fd ""
    puts $fd "# GENERATED: [clock format [clock seconds]]"
    puts $fd "# DO NOT EDIT - file is regenerated whenever the project changes"
    puts $fd ""
    puts $fd "\n## core information\n"
    puts $fd "ifeq ($(wildcard Makefile),)"
    puts $fd "PRJ_DIR        = .."
    puts $fd "else"
    puts $fd "PRJ_DIR        = ."
    puts $fd "endif"
    puts $fd "PRJ_FILE       = [prjRelPath $hProj [file tail $prjFile]]"
    puts $fd "PRJ_TYPE       = vxWorks"
    puts $fd "PRJ_OBJS       = $prjObjs"
    puts $fd "BOOT_OBJS      = $bootObjs $(BOOT_EXTRA)"
    puts $fd "BUILD_SPEC     = $hSpec"
    puts $fd "BSP_DIR        = [prjRelPath $hProj $bspDir]"
    puts $fd "TGT_DIR        = $(WIND_BASE)/target"
    puts $fd "\n\n"

    puts $fd "## set searching directories for dependencies\n"
    puts $fd "vpath %.c \$(BSP_DIR)"
    puts $fd "vpath %.cpp \$(BSP_DIR)"
    puts $fd "vpath %.cxx \$(BSP_DIR)\n"

    puts $fd "\n## build-configuration info\n"
    foreach build $builds {
	set tc [prjBuildTcGet $hProj $build]
	puts $fd "ifeq ($(BUILD_SPEC),$build)"
	puts $fd "CPU            = [${tc}::cpu]"
	puts $fd "TOOL           = [${tc}::tool]"
	puts $fd "TOOL_FAMILY    = [${tc}::family]"
	puts $fd "DEFAULT_RULE   = [prjBuildRuleCurrentGet $hProj $build]"
	puts $fd "endif\n"
    }

    puts $fd "\n## component-configuration info\n"
    set wcc [wccCurrentGet $hProj]
    set cpu [${tc}::cpu]
    set tool [${tc}::tool]
    set componentLibs ""
    set libs [prjBuildMacroGet $hProj $build LIBS]
    foreach lib [wccMxrLibsGet $hProj] {
	if {[lsearch $libs $lib] == -1} {
	    lappend componentLibs $lib
	}
    }
    regsub "[wtxPath]target/lib/lib${cpu}${tool}vx.a" $componentLibs \
		"" componentLibs
    regsub -all {[\{\}]} $componentLibs "" componentLibs

    puts $fd "COMPONENTS = [prjDataSplit [lsort [wccComponentListGet \
		$hProj $wcc]]]"
    puts $fd "COMPONENT_LIBS = $componentLibs"

    puts $fd "\n\ninclude $(TGT_DIR)/h/make/defs.project\n"

    puts $fd "\n## build-configuration info\n"
    foreach build $builds {
	set tc [prjBuildTcGet $hProj $build]
	puts $fd "ifeq ($(BUILD_SPEC),$build)"
	foreach macro [prjBuildMacroListGet $hProj $build "" 1]  {
	    puts $fd [format "%-16s = %s" $macro \
		[prjRelPath $hProj [prjBuildMacroGet $hProj $build $macro]]]
	}
	puts $fd "endif\n"
    }

    puts $fd "# override make definitions only below this line\n\n"
    puts $fd "# override make definitions only above this line\n"
    puts $fd "include $(TGT_DIR)/h/make/rules.project\n"

    puts $fd "\n## build-configuration info\n"
    puts $fd "sysALib.o: $(PRJ_DIR)/prjParams.h\n"
    foreach build $builds {
	puts $fd "ifeq ($(BUILD_SPEC),$build)"
	foreach file $prjFiles {
	    # skip files with custom rules - custom rules generated later
	    if {"[prjFileInfoGet $hProj $file customRule]" != ""} {
		continue
	    }
	    foreach object [prjFileInfoGet $hProj $file objects] {
		puts $fd "\n${object}:"
		foreach cmd [prjRelPath $hProj [prjBuildCmdsGet $hProj \
				 $build $file]] {
		    puts $fd "\t$cmd"
		}
	    }
	}
	puts $fd "endif\n"
    }

    puts $fd "\n## dependencies\n"
    foreach file $prjFiles {
	set dependencies [prjFileInfoGet $hProj $file dependencies]
	set roDependPath [prjDepPathGet $hProj $build]

	foreach depDir $roDependPath {
	    regsub -all "${depDir}(\[^ \])* " $dependencies "" dependencies
	}

	foreach object [prjFileInfoGet $hProj $file objects] {
	    puts $fd "\n${object}: [prjDataSplit \
		[prjRelPath $hProj [concat $file $dependencies]]]\n"
	}
    }

    # add user defined rules
    set targetList [prjBuildRuleListGet $hProj $hSpec]

    if {"$targetList" != ""} {
        puts $fd "\n## user defined rules\n"
        foreach target $targetList {
            set i 1
            set rule [prjBuildRuleGet $hProj $hSpec $target]
            if {$rule == ""} {
                continue
            }

            foreach line $rule {
                if {$i} {
                    # print the target line (no tabulation)
                    puts $fd [prjRelPath $hProj $line]
                    set i 0
                } else {
                    # print the command lines (with tabulation)
                    puts $fd "\t[prjRelPath $hProj $line]"
                }
            }

            puts $fd ""
        }
    }

    close $fd
}


###############################################################################
#
# objsGet - get the list of project objects and the list of boot objects
# RETURNS list: <project objects, boot objects>
#

proc objsGet {hProj} {
    set prjFiles [prjFileListGet $hProj]
    set prjObjs ""
    set bootObjs ""
    foreach file $prjFiles {
	set objs [prjFileInfoGet $hProj $file objects]
	if {[prjFileInfoGet $hProj $file BOOT_FILE] != ""} {
	    if {"$objs" == "romInit.o"} {
		set bootObjs [concat $objs $bootObjs]
		continue
	    }
	    set bootObjs [concat $bootObjs $objs]
	    continue
	}
	if {"$objs" == "sysALib.o"} {
	    set prjObjs [concat $objs $prjObjs]
	    continue
	}
	foreach obj $objs {
	    if {"[string range [file extension $obj] 0 1]" == ".o"} {
		lappend prjObjs $obj
	    }
	}
    }
    return [list $prjObjs $bootObjs]
}

###############################################################################
#
# configGen - generate configuration files for a project
#
# This routine generates prjConfig.c, prjComps.h, prjParams.h, and linkSyms.c
# for a vxWorks system or bootrom project.
# 

proc configGen {hProj {components ""}} {
    set projType   [::[prjTypeGet $hProj]::name]
    set prjDir     [file dirname [prjInfoGet $hProj fileName]]
    set cmpLib     [cxrDocCreate $hProj]
    set cmpSet     [$cmpLib setCreate]
    set wcc	   [wccCurrentGet $hProj]
    set macros	   [wccParamListGet $hProj $wcc]

    if {"$components" == ""} {
	set components [wccComponentListGet $hProj $wcc]
    } else {
	$cmpSet = $components
	$cmpSet = "$macros [$cmpSet get CFG_PARAMS]"
	set macros [$cmpSet instances Parameter]
    }

    set startDir [pwd]
    cd $prjDir

    # compute the set of included components

    $cmpSet =  [$cmpLib instances Component]
    set s      [$cmpLib setCreate]
    $s = $components

    set includes [lsort [$s instances]]
    $cmpSet = [$cmpLib instances Component]
    set excludes [$cmpSet - $components]
    # XXX - some BSPs won't compile if INCLUDE_SCSIX is not defined
    if {[lsearch INCLUDE_SCSI $includes] == -1} {
	regsub -all {(INCLUDE_SCSI1)|(INCLUDE_SCSI2)} $excludes {} excludes
    }

    ## write out prjComps.h

    set fd [open prjComps.h w+]
    fconfigure $fd -translation lf
    puts $fd "/* prjComps.h - dynamically generated configuration header */\n"
    puts $fd ""
    puts $fd "/*"
    puts $fd "GENERATED: [clock format [clock seconds]]"
    puts $fd "DO NOT EDIT - file is regenerated whenever the project changes"
    puts $fd "*/"
    puts $fd ""
    puts $fd "#ifndef INCprjCompsh"
    puts $fd "#define INCprjCompsh"
    puts $fd "\n/*** INCLUDED COMPONENTS ***/\n"
    foreach component $includes {
        puts $fd "#define $component"
    }
    puts $fd "\n#endif /* INCprjCompsh */"
    close $fd

    ## write out prjParams.h

    set fd [open prjParams.h w+]
    fconfigure $fd -translation lf
    puts $fd "/* prjParams.h - dynamically generated configuration header */\n"
    puts $fd ""
    puts $fd "/*"
    puts $fd "GENERATED: [clock format [clock seconds]]"
    puts $fd "DO NOT EDIT - file is regenerated whenever the project changes"
    puts $fd "*/"
    puts $fd ""
    puts $fd "#ifndef INCprjParamsh"
    puts $fd "#define INCprjParamsh"
    puts $fd "\n\n/*** INCLUDED COMPONENTS ***/\n"
    foreach component $includes {
        puts $fd "#define $component"
    }
    foreach component $excludes {
        puts $fd "#undef $component"
    }
    puts $fd "\n\n/*** PARAMETERS ***/\n"
    foreach macro $macros {
        puts $fd "#undef  $macro"
	if { ![wccParamHasValue $hProj $wcc $macro] } {
		continue
	}
	set type [wccParamTypeGet $hProj $macro]
	set val  [wccParamGet $hProj $wcc $macro]		
	if {"$type" == "exists"} {
		if {"$val" == "TRUE"} {
			puts $fd "#define $macro"
		}
		continue
	}
	puts $fd "#define $macro [wccParamGet $hProj $wcc $macro]"
    }
    puts $fd "\n#endif /* INCprjParamsh */"
    close $fd

    ## write out linkSyms.c

    $s = $includes
    $cmpSet = [join [$s get LINK_SYMS]]
    set syms [lsort [$cmpSet instances]]
    $cmpSet = [join [$s get LINK_DATASYMS]]
    set dataSyms [lsort [$cmpSet instances]]
    set fd [open linkSyms.c w+]
    fconfigure $fd -translation lf
    puts $fd "/* linkSyms.c - dynamically generated configuration file */\n"
    puts $fd ""
    puts $fd "/*"
    puts $fd "GENERATED: [clock format [clock seconds]]"
    puts $fd "DO NOT EDIT - file is regenerated whenever the project changes"
    puts $fd "*/"
    puts $fd ""
    puts $fd "typedef int (*FUNC) ();"
    foreach sym $syms {
        puts $fd "extern int $sym ();"
    }
    foreach sym $dataSyms {
        puts $fd "extern int $sym;"
    }
    puts $fd "\nFUNC linkSyms \[\] = \{"
    foreach sym $syms {
        puts $fd "    $sym,"
    }
    puts $fd "    0"
    puts $fd "\};\n"
    puts $fd "\nint * linkDataSyms \[\] = \{"
    foreach sym $dataSyms {
        puts $fd "    &$sym,"
    }
    puts $fd "    0"
    puts $fd "\};\n"
    close $fd
    
    ## write out prjConfig.c
    
    if {$projType == "bootrom"} {
        # XXX - hack for now
        file copy [wtxPath]target/config/all/bootConfig.c prjConfig.c
	cd $startDir
        return
    }
    
    set fd [open prjConfig.c w+]
    fconfigure $fd -translation lf
    puts $fd "/* prjConfig.c - dynamicaly generated configuration file */\n"
    puts $fd ""
    puts $fd "/*"
    puts $fd "GENERATED: [clock format [clock seconds]]"
    puts $fd "DO NOT EDIT - file is regenerated whenever the project changes."
    puts $fd "This file contains the non-BSP system initialization code"
    puts $fd "for $projType."
    puts $fd "*/"

    puts $fd "\n\n/* includes */\n"
    $cmpSet = [join [$s get HDR_FILES]]
    puts $fd "#include \"vxWorks.h\""
    puts $fd "#include \"config.h\""
    foreach header [lsort [$cmpSet contents]] {
        puts $fd "#include \"$header\""
    }

    puts $fd "\n\n/* imports */\n"
    puts $fd "IMPORT char etext \[\];                   /* defined by loader */"
    puts $fd "IMPORT char end \[\];                     /* defined by loader */"
    puts $fd "IMPORT char edata \[\];                   /* defined by loader */"

    set bspDir [prjTagDataGet $hProj BSP_DIR]                               
    puts $fd "\n\n/* BSP_STUBS */\n"                                        
    $cmpSet = [join [$s get BSP_STUBS]]                                     
    foreach bspStub [lsort [$cmpSet contents]] {                            
	puts $fd "#include \"$bspStub\""                                    
	if { [file exists ${bspDir}/$bspStub] == 0} {                       
	    puts "Missing BSP_STUB $bspStub, copying template into $bspDir" 
	    set srcStub [wtxPath target config comps src]$bspStub           
	    file copy $srcStub ${bspDir}/$bspStub                           
        }                                                                   
    }                                                                       
                                                                             
    puts $fd "\n\n/* configlettes */\n"
    $cmpSet = [join [$s get CONFIGLETTES]]
    puts $fd "#include \"sysComms.c\""
    foreach configlette [lsort [$cmpSet contents]] {
        puts $fd "#include \"$configlette\""
    }

    # start with the root InitGroups
    $s = [$cmpLib instances InitGroup]	;# all InitGroups
    set initOrder [join [$s get INIT_ORDER]]
    $s = [$s - $initOrder]			;# root InitGroups
  
    foreach initGroup [$s contents] {
        doInitGroup $initGroup $fd $cmpLib $includes
    }
    close $fd

    cd $startDir
}

###############################################################################
#
# doInitGroup - process an InitGroup for output
#
# This procedure is recurvie, doing depth-first processing of the init groups.
# If none all the "child" initGroups are empty, and this initGroup
# has no included components, then we don't bother to output this initGroup.
# 

proc doInitGroup {initGroup fd cmpLib includes} {
    set s     [$cmpLib setCreate]

    $s = $initGroup
    set synopsis  [$s get SYNOPSIS]
    set prototype [$s get PROTOTYPE]
    if {"$prototype" == ""} {
	set prototype "void [lindex [$s get INIT_RTN] 0] (void)"
    }
    set proc [lindex $prototype 1]

    set initOrder [join [$s get INIT_ORDER]]

    # adjust initOrder to respect INIT_BEFORE requests

    set tmp [$cmpLib setCreate]
    foreach item $initOrder {
	$tmp = $item
	if {[set initBefore [$tmp get INIT_BEFORE]] == ""} {
	    continue
	}
	set oldIx [lsearch $initOrder $item]
	set newIx $oldIx
	foreach cmp $initBefore {
	    set cmpIx [lsearch $initOrder $cmp]
	    if {$cmpIx < $newIx} {
		set newIx $cmpIx
	    }
	}
	if {$newIx == $oldIx} {
	    continue
	}
	set initOrder [lreplace $initOrder $oldIx $oldIx]
	set initOrder [linsert  $initOrder $newIx $item]
    }
    $tmp delete

    # compute included subgroups and components

    set includedSubgroups ""
    $s = $initOrder
    foreach group [$s instances InitGroup] {
	if {[doInitGroup $group $fd $cmpLib $includes] != 0} {
	    lappend includedSubgroups $group
	}
    }

    $s = [$s & [concat $includes $includedSubgroups]]
    set includedItems [$s & [concat $includes $includedSubgroups]]
    if {[llength $includedItems] == 0} {
	$s      delete
	return 0
    }

    puts $fd "\n\n/******************************************************************************\n*\n* $proc - $synopsis\n*/\n"
    puts $fd "$prototype"
    puts $fd "    \{"

    # XXX - it would be much faster if we could just do $s contents,
    # but that only works if $s contents is ordered in the initOrder.
    # unfortunalty in the current engine $s contents is a random order.
    # That's OK for now since performance is fast enough.
    foreach item $initOrder {
	if {[lsearch $includedItems $item] == -1} {
	    continue
	}
	$s = $item
	set desc [$s get SYNOPSIS]
	if {"$desc" == ""} {
	    set desc [$s get NAME]
	}
	puts $fd [format "    %-35s %s" [$s get INIT_RTN] "/* $desc */"]
    }
    puts $fd "    \}\n"

    $s      delete
}

}

namespace eval ::prj_vxWorks {

###############################################################################
#
# create - create a vxWorks project
#

proc create {prjFile bspDir {bootromFile ""} {description ""} {cpu ""} {tool ""}} {

    if {[catch {::prj_vxWorks_hidden::vxProjCreate \
	$prjFile $bspDir "$bootromFile" "$description" "$cpu" "$tool"} err]} {
	catch {file delete -force [file dirname $prjFile]}
	error $err
    }
}

###############################################################################
#
# name - return the name of this project type
#

proc name {} {
    return "Create a bootable VxWorks image (custom configured)"
}

###############################################################################
#
# toolChainListGet - get the list of toolChains compatible with vxWorks
#

proc toolChainListGet {} {
    return [prjTcListGet {} {} C++]
    }

###############################################################################
#
# coreFilesGet - returns list of files (apart from the project file itself)
#                that define the "core" of the project. In a 
#                "save as", these are the files that get copied. 
#

proc coreFilesGet {} {
    return "linkSyms.c prjComps.h prjConfig.c prjParams.h usrAppInit.c"
    }

###############################################################################
#
# close - hook to call on project close
# 

proc close hProj {
    cxrDocDelete $hProj
    mxrDocDelete $hProj    
}

###############################################################################
#
# buildRuleListGet - return the list of build rules for vxWorks
#

proc buildRuleListGet {hProj} {
    set retval vxWorks
    if {![::prj_vxWorks_hidden::basedOnSimulator $hProj]} {
	set retval [concat $retval \
	    vxWorks_rom \
	    vxWorks_rom.bin \
	    vxWorks_rom.hex \
            vxWorks_romCompress \
            vxWorks_romCompress.bin \
            vxWorks_romCompress.hex \
	    vxWorks_romResident \
	    vxWorks_romResident.bin \
	    vxWorks_romResident.hex ]
    }
    return $retval
}

###############################################################################
#
# buildRuleDefault - return the default build rule
#

proc buildRuleDefault {hProj} {
    if {[string match [prjTagDataGet $hProj CORE_INFO_SUBTYPE] \
	bootrom]} {
	return vxWorks ;# This will need to be fixed!
    } else { 
     	return vxWorks
    }
}

##############################################################################
#
# buildCustomRuleListGet - return the list of custom build rules
#

proc buildCustomRuleListGet {} {
    return [::prj_vxWorks_hidden::bspRulesGet]
}

###############################################################################
#
# buildModify - modify a build spec for vxWorks projects
#
# After a buildSpec is created from toolchain defaults, this
# routine is called to make whatever modifications are needed
# for vxWorks projects.
# This routine:
# * adds appropriate -IXX flags for the vxWorks headers
# * adds -DCPU=$(CPU) -DTOOL=$(TOOL) -DTOOL_FAMILY=$(TOOL_FAMILY)
# * If another build-spec exists, it copies the BSP-specific build-macros
#   from that spec:
#	RAM_HIGH_ADRS. RAM_LOW_ADRS, ROM_SIZE, ROM_TEXT_ADRS,
#	*IMI* flags (for I960).
#   It also copies any defines and includes from that spec.
#   that were not defaults. That way if the user adds custom -D and -I
#   options to a build, those will be preserved when a new spec is
#   created.
#

proc buildModify {hProj build} {
    set tc [prjBuildTcGet $hProj $build]
    set cc [${tc}::toolFindByFile foo.c]
    set as [${tc}::toolFindByFile foo.s]
    set tool [${tc}::tool]

    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set tgtDir [wtxPath target]
    set bspDir [prjTagDataGet $hProj BSP_DIR]
    set inc [prjBuildMacroGet $hProj $build OPTION_INCLUDE_DIR]
    set def [prjBuildMacroGet $hProj $build OPTION_DEFINE_MACRO]
    set extraCppFlags "${inc}$prjDir ${inc}$bspDir ${inc}${tgtDir}h 	\
		${inc}${tgtDir}config/comps/src ${inc}${tgtDir}src/drv	\
		${def}CPU=[${tc}::cpu] ${def}TOOL_FAMILY=[${tc}::family] \
		${def}TOOL=${tool} ${def}PRJ_BUILD"
    set cflags   [prjBuildFlagsGet $hProj $build $cc]
    set asflags  [prjBuildFlagsGet $hProj $build $as]
    prjBuildFlagsSet $hProj $build $cc  "$cflags $extraCppFlags"
    prjBuildFlagsSet $hProj $build $as  "$asflags $extraCppFlags"

    prjDepPathAdd $hProj $build [prjDepPathDefaults]

    # try to copy BSP-specific build macros from an existing buildSpec

    set buildList [prjBuildListGet $hProj]
    if {[llength $buildList] > 1} {
	set oldBuild [lindex $buildList 0]
	set oldTc [prjBuildTcGet $hProj $oldBuild]

	# Copy macros from old build if they exist in the old spec,
	# but not in the new. This will get things like RAM_HIGH_ADRS, etc,
	# without changing the toolchain values

	foreach macro [prjBuildMacroListGet $hProj $oldBuild "" 1] {
	    set oldVal [prjBuildMacroGet $hProj $oldBuild $macro]
	    set newVal [prjBuildMacroGet $hProj $build $macro]
	    if {("$oldVal" != "") && ("$newVal" == "")} {
		prjBuildMacroSet $hProj $build $macro $oldVal
	    }
	}

	# Copy custom C-preprocessor flags

	foreach buildFlag "CFLAGS CFLAGS_AS" {
	    set tcFlags [${oldTc}::macroDefaultValGet $buildFlag]
	    foreach flag [prjBuildMacroGet $hProj $oldBuild $buildFlag] {
                set searchFlag $flag

                # if old flag is e.g. -DTOOL=diab, match
                # -DTOOL=* in new flag. If we match, skip this
                # flag. Otherwise we end up with e.g. 
                # -DTOOL=diab -DTOOL=gnu

                if {[string match -D* $searchFlag]} {
	            regsub {=[^=]*} $searchFlag {=} searchFlag
                    append searchFlag *
                }
		if {[lsearch $tcFlags $searchFlag] != -1} {
		    continue
		}
		if {[lsearch $extraCppFlags $searchFlag] != -1} {
		    continue
		}
		prjBuildMacroSet $hProj $build $buildFlag \
			"[prjBuildMacroGet $hProj $build $buildFlag] $flag"
	    }
	}
    }
}

###############################################################################
#
# buildCmdGet - get the command needed to build an image
#
# RETURNS: a pair "command directory", so that invoking
# command in directory will build the desired image
#

proc buildCmdGet {hProj {rule ""} {spec ""}} {

    # mTime - return the time of last modification of a file
    # regenerate the Makefile if it is older than the project file

    proc mTime file {
	if {![file exists $file]} {
	    return 0
	}
	file stat $file statInfo
	return $statInfo(mtime)
    }

    set prjDir [file dir [prjInfoGet $hProj fileName]]
    set makeModTime [mTime $prjDir/Makefile]
    set prjModTime  [prjInfoGet $hProj changed]
    if {"$prjModTime" == ""} {
        set prjModTime  [mTime [prjInfoGet $hProj fileName]]
    }
    if {$makeModTime < $prjModTime} {
        ::prj_vxWorks_hidden::makeGen $hProj $prjDir/Makefile
    }

    # return the info

    if {"$spec" == ""} {
	set spec [prjBuildCurrentGet $hProj]
    }
    catch "file mkdir $prjDir/$spec"
    if {"$rule" != ""} {
	set rule "DEFAULT_RULE=[lindex $rule end] $rule"
    }
    return [list "make -f ../Makefile BUILD_SPEC=$spec $rule" "$prjDir/$spec"]
}

###############################################################################
#
# derivedObjsGet - get the derived objects produced the project
#

proc derivedObjsGet {hProj image} {
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set build  [prjBuildCurrentGet $hProj]
    set tc     [prjBuildTcGet $hProj $build]
    set buildObjs ""

    foreach file [prjFileListGet $hProj] {
        if {"[set tool [::${tc}::toolFindByFile $file]]" == ""} {
            continue
        }
        foreach obj [::${tool}::outputFiles $file] {
            lappend buildObjs $prjDir/$build/$obj
        }
    }

    if {"$tc" == "::tc_SIMNTgnu"} {
	set image vxWorks.exe
    }

    set buildObjs "$buildObjs $prjDir/$build/$image"

    return $buildObjs
}

prjTypeAdd ::prj_vxWorks
}
