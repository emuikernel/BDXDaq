# gnuInfoGen.tcl - generate info on the GNU tool family
#
# modification history
# --------------------
# 01t,28jan03,rbl  check in makefiles rather than generating them - this
#                  fixes SPR 82348, Two different users can not start 
#                  Tornado on the same UNIX machine
# 01s,20may02,rbl  show status when building toolchain info at IDE startup
#                  time
# 01r,15may02,cjs  Fix spr 71679 -- toolchain info file generation should
#                  write to temporary files to writeable location
# 01q,30apr02,cym  adding HEX_FLAGS
# 01p,17dec01,rbl  make tcInfo_* production robust against diab not being
#                  installed
# 01o,05nov01,rbl  switch to -g, no optimization for project builds
# 01n,24oct01,rbl  get dependency generation working with diab compiler
# 01m,17sep01,rbl  merge changes from T3: (1) make generation of
#                  tcInfo_<family> more robust (2) add LD_PARTIAL (3) use make
#                  to extract CPU and TOOL (4) parse makemacros using regsub
#                  so that "windows hack" is not needed (5) make sure that
#                  absolute paths don't show up in tcInfo_<family>
# 01n,25sep01,dat  moved makefiles to h/tool/, renamed vendor to family
# 01m,17sep01,sn  reinstated 01i and 01k (initial problems fixed by 01j)
# 01l,31may01,kab  Removed -nostdinc from simulator
# 01k,28mar01,sn  Removed SIMNT hacks (no longer necessary)
# 01j,12mar01,sn  Spell -x assembler-with-cpp without the space to 
#                 ensure correct handling by cppFlagsGet
# 01k,19jan01,mem  Undo change 01i, breaks rebuild all from project facility.
# 01j,12jan01,mem rewrote detection loop to not assume TOOL=VENDOR, but
#		  just rather a subset.
# 01i,06dec00,sn  removed -xc from tool_cpp
# 01h,16mar99,pr  removed -Ox option from compiler flags (SPR 25729)
# 01g,26oct98,ms  reworked for diab integration.
# 01f,25aug98,ms  modified to output a table, instead of TCL code.
#		  that way 3rd party vendors don't have to learn TCL.
#		  made it work with non-gnu toolchains.
# 01e,11aug98,ren refined previous check in.  Now, nm called without -g
#                 option only for those three architectures.
# 01d,11aug98,ren removed -g option from nm commandline.
#                 necessary for PPC, SIMSOLARIS and 960 architectures,
#                 but shouldn't affect others.
# 01c,27jul98,ms  added nm, size, and ar utilities. Fixed sim flags parsing.
# 01b,25jun98,ms  combined C and C++ compiler into one tool. Added LDFLAGS.
# 01a,08apr98,ms  written.
#
# DESCRIPTION
# This library generates inforamation on the GNU (or other) tool family
# suitable for importing into tc_cplus.tcl (which exposes the
# information to the configuration tool).
#
# USAGE
# wtxtcl gnuInfoGen.tcl [family]
#

# need prjLib.tcl for prjRelPath functionality - to avoid absolute paths in 
# tcInfo_<family>

package require Wind

set prjLibTcl_sourcePrjLibAndUtilsOnly 1
source [wtxPath host resource tcl app-config Project]prjLib.tcl
unset prjLibTcl_sourcePrjLibAndUtilsOnly

# begin namespace ::toolInfoGenProgress

namespace eval ::toolInfoGenProgress {
    variable gui    0
    variable family gnu

proc progressInit {toolFamily} {
    variable gui
    variable family

    set family $toolFamily

    if {[info commands dialogCreate] == "dialogCreate"} {
        set gui 1
    }
    if {$gui} {
        #
        # create separate dialog for each tool family. 
        # when I tried to reuse the dialog after doing a 
        # windowClose on it, the dialog would not update -
        # probably windowClose needs to process events to 
        # clean up properly. 
        #

        dialogCreate -modeless -keepontop \
            -name dlProgressDlg_$family -title \
            "Preparing Tornado for first use" \
            -xpos 0 -ypos 0 \
            -width 200 -height 70 -nocontexthelp \
            -controls [list \
                [list label -name status -title "" \
                    -xpos 7 -ypos 7 -width 185 -height 9] \
                [list label -name error -title "" \
                    -xpos 7 -ypos 21 -width 185 -height 18] \
                [list progressmeter -left -name filePrgrssCtrl \
                    -xpos 8 -ypos 42 -width 184 -height 14] \
            ]
	controlPropertySet dlProgressDlg_$family.filePrgrssCtrl \
            -foreground Black -background DarkBlue 
        controlValuesSet dlProgressDlg_$family.filePrgrssCtrl 0
    }
}

proc progressSetTaskBeginText {message} {
    variable gui
    variable family
    if {$gui} {
        controlTextSet dlProgressDlg_$family.status $message
    }
    puts -nonewline $message
}

proc progressSetTaskEndText {message} {
    variable gui
    variable family
    if {$gui} {
        controlTextSet dlProgressDlg_$family.status $message
    }
    puts $message
}

proc progressUpdate {percent} {
    variable gui
    variable family
    if {$gui} {
        controlValuesSet dlProgressDlg_$family.filePrgrssCtrl $percent
    }
    puts -nonewline "."
}

proc progressErrorReport {message} {
    variable gui
    variable family
    if {$gui} {
        controlTextSet dlProgressDlg_$family.error $message
    }
    puts -nonewline "\n\[$message\]\n"
}

proc progressFinish {} {
    variable gui
    variable family
    if {$gui} {
        windowClose dlProgressDlg_$family
    }
}

}

# end namespace ::toolInfoGenProgress

if {$argc == 1} {
    set family [lindex $argv 0]
} else {
    set family gnu
}

#
# if this is called from a higher level makefile that had CPU and 
# TOOL passed in on the command line, this will mess up the toolchain
# information gathering. These values may also be passed on through
# the environment variable MAKEFLAGS. To avoid this we unset the 
# environment variables CPU, TOOL and MAKEFLAGS, and reset them when 
# we are done
#

array set saveEnv {}
foreach envVar {CPU TOOL MAKEFLAGS ADDED_CFLAGS} {
    if {[info exists env($envVar)]} {
        set saveEnv($envVar) $env($envVar)
        set env($envVar) ""
    }
}

# use makefiles to get the default build
# flags for each tool family

set makefile [wtxPath host resource tcl app-config Project]Makefile
set cpuToolMakefile [wtxPath host resource tcl app-config Project]Makefile.getCPUTOOL

# determine the list of toolchains by looking for CPU= and TOOL= in files make.*${family}*

::toolInfoGenProgress::progressInit $family
::toolInfoGenProgress::progressSetTaskBeginText "Finding $family toolchains"

# finding toolchains takes roughly 25 % of the time ...

set findPercent 25
set extractPercent [expr 100 - $findPercent]

set toolchainList ""
set makeStubs [glob -nocomplain [wtxPath]/target/h/tool/${family}/make.*${family}*]
set nMakeStubs [llength $makeStubs]
set iMakeStub 0
foreach makeStub $makeStubs {
    incr iMakeStub
    set makeStub [file tail $makeStub]
    regsub -all "make." $makeStub "" toolchain
    set cpuWithNOP [exec make --no-print-directory -f $cpuToolMakefile \
        getCPU TOOLCHAIN=${toolchain} TOOL_FAMILY=${family}]
    set toolWithNOP [exec make --no-print-directory -f $cpuToolMakefile \
        getTOOL TOOLCHAIN=${toolchain} TOOL_FAMILY=${family}]

    # strip off "NOP" from beginning and remove whitespace
    set cpuForToolChain($toolchain) [string trim [lindex $cpuWithNOP 1]]
    set toolForToolChain($toolchain) [string trim [lindex $toolWithNOP 1]]

    if {$cpuForToolChain($toolchain) != "" && \
        $toolForToolChain($toolchain) != ""} {
        lappend toolchainList $toolchain
        ::toolInfoGenProgress::progressUpdate [expr int( double($iMakeStub) / $nMakeStubs * $findPercent)]
    } else {
        ::toolInfoGenProgress::progressErrorReport "skipping $makeStub \n(does not define CPU and TOOL)"
    }
}
::toolInfoGenProgress::progressSetTaskEndText "Done"

# use the Makefile to gather info on each cpu/tool pair

::toolInfoGenProgress::progressSetTaskBeginText "Getting $family toolchain information"
set iMakeStub 0
set nToolchains [llength $toolchainList]
foreach toolchain $toolchainList {
    incr iMakeStub
    set cpu $cpuForToolChain($toolchain)
    set tool $toolForToolChain($toolchain)
    ::toolInfoGenProgress::progressUpdate [expr int( $findPercent + \
        double($iMakeStub) / $nToolchains * $extractPercent)]
    set lines [exec make -f $makefile makeMacros \
        CPU=$cpu TOOL_FAMILY=${family} TOOL=$tool \
        CC_INCLUDE= CC_DEFINES= ADDED_C++FLAGS=$(DEFINE_CC) \
        TOOL_FAMILY=${family} BSP2PRJ=TRUE]

    foreach line [split $lines \n] {
        set macro [lindex $line 1]
	if {[regsub {[^=]*=} $line {} val] != 1} {
	    continue
	}
	set val   [string trim $val]
	switch -- $macro {
	    CC	{
		set tool_cc($toolchain) [lindex $val 0]
		set tool_as($toolchain) [lindex $val 0]
		set tool_cpp($toolchain) "[lindex $val 0] -E -P"
	    }
	    CPP	{
		if {"$family" != "gnu"} {
		    set tool_cpp($toolchain) "$val"
		}
	    }            
	    CC_ARCH_SPEC {
		set tool_cc_arch_spec($toolchain) "$val"
	    }
	    OPTION_DEPEND	{
		set tool_depend($toolchain) "$val"
	    }
	    DEPEND_GEN_UTIL	{
		set tool_depend_gen_util($toolchain) "$val"
	    }
	    LD	{
		set tool_ld($toolchain) "$val"
	    }
	    LDFLAGS	{
		set flags_ld($toolchain) "$val"
	    }
	    LD_PARTIAL	{
		set tool_ldp($toolchain) "$val"
	    }
	    LD_PARTIAL_FLAGS	{
		set flags_ldp($toolchain) "$val"
	    }
	    HEX_FLAGS {
		set flags_hex($toolchain) "$val"
            }
	    CFLAGS_PROJECT {
		set flags_cc($toolchain) "$val"
            }
	    CFLAGS_AS_PROJECT {
		set flags_as($toolchain) "$val"
            }
	    AR {
		set util_ar($toolchain) "$val"
	    }
	    NM {
		switch -glob $cpu { 
		    SIMSPARC*  { set util_nm($toolchain) "$val" }
		    PPC*       { set util_nm($toolchain) "$val" }
		    *960*      { set util_nm($toolchain) "$val" }
		    default   {set util_nm($toolchain) "$val -g"}
		}
	    }
	    SIZE {
		set util_size($toolchain) "$val"
	    }
	}
    }
}
::toolInfoGenProgress::progressSetTaskEndText "Done"
::toolInfoGenProgress::progressFinish

# XXX - some hacks to adjust the info computed above
# SIMSPARCSOLARIS's BSP overrides the definition of LD.

set tool_ld(SIMSPARCSOLARISgnu) ccsimso
set flags_ldr(SIMSPARCSOLARISgnu) "-nostdlib -r"

# write out tcInfo_${family}

set toolchainInfoFile [wtxPath host resource tcl app-config Project]tcInfo_${family}
file delete -force $toolchainInfoFile
set fd [open ${toolchainInfoFile}.temp w+]
fconfigure $fd -translation lf
puts $fd "# tcInfo_${family} - generated by gnuInfoGen.tcl\n\n"
foreach toolchain [lsort $toolchainList] {
    set cpu $cpuForToolChain($toolchain)
    set tool $toolForToolChain($toolchain)
    puts $fd ""
    puts $fd "$toolchain"
    puts $fd "	CPU	$cpu"
    puts $fd "	TOOL	$tool"
    puts $fd "	TOOL_FAMILY	$family"
    puts $fd "	CC	$tool_cc($toolchain)"
    puts $fd "	CPP	$tool_cpp($toolchain)"
    puts $fd "	AS	$tool_as($toolchain)"
    puts $fd "	LD	[prjRelPath "" $tool_ld($toolchain)]"
    puts $fd "	LD_PARTIAL	[prjRelPath "" $tool_ldp($toolchain)]"
    puts $fd "	NM	$util_nm($toolchain)"
    puts $fd "	SIZE	$util_size($toolchain)"
    puts $fd "	AR	$util_ar($toolchain)"
    puts $fd "	CFLAGS_AS	[prjRelPath "" $flags_as($toolchain)]"
    puts $fd "	CFLAGS	[prjRelPath "" $flags_cc($toolchain)]"
    puts $fd "	LDFLAGS	[prjRelPath "" $flags_ld($toolchain)]"
    puts $fd "	LD_PARTIAL_FLAGS	[prjRelPath "" $flags_ldp($toolchain)]"
    puts $fd "	HEX_FLAGS	[prjRelPath "" $flags_hex($toolchain)]"
    puts $fd "	OPTION_DEPEND   $tool_depend($toolchain)"
    if {[info exists tool_depend_gen_util($toolchain)]} {
	puts $fd "	DEPEND_GEN_UTIL	$tool_depend_gen_util($toolchain)"
    }
    if {[info exists tool_cc_arch_spec($toolchain)]} {
	puts $fd "	CC_ARCH_SPEC	$tool_cc_arch_spec($toolchain)"
    }
}
close $fd

file rename -force ${toolchainInfoFile}.temp ${toolchainInfoFile}

# reset CPU, TOOL, MAKEFLAGS environment variables

foreach envVar [array names saveEnv] {
        set env($envVar) $saveEnv($envVar)
}

