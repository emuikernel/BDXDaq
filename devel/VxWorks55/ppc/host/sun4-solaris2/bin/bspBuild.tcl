# bspBuild.tcl - tcl script to list the bsp Makefiles for a CPU
# 
# modification history
# --------------------
# 01e,22oct01,dat  Ignore BSP build errors
# 01d,06apr01,mrs  Fix quoting.
# 01c,14apr98,ms   only build BSPs if the TOOL matches (in addition to CPU)
# 01b,23feb98,elp  do not read Makefile comment lines to prevent errors
# 01a,18aug96,yp   written
#
# DESCRIPTION
# Given a CPU name as the first arg, and the special flags to pass to make,
# and a list of text Makefiles to seach, this script searches the Makefiles 
# for a definition of that CPU and calls Make to build the directories
# where there were matches. Called my target/config/Makefile

set usage "wtxtcl -f bspBuild.tcl <CPU> <TOOL> <TARGET> <TGT_DIR> \[makefiles\ ...] "

if {$argc < 4} {
	puts "$usage"
	exit -1
	}

if {$argc < 5} {
	puts "Makefile list is empty .. Bailing out .."
	exit 
	}
# null arguments to pass
set mfList ""

# get CPU def from arg list
set CPU [lindex $argv 0]

# get TOOL def from arg list
set TOOL [lindex $argv 1]

# get TARGET def from arg list
set TARGET [lindex $argv 2]

# get TGT_DIR def from arg list
set TGT_DIR [lindex $argv 3]

# the args left are Makefile paths where each Makefile needs to be 
# examined for CPU and TOOL definitions. If the definition matches what
# we are looking for, then we call on make to build that directory. 
# if we catch an error we simply bail out after spitting out the
# message recieved.

set dirs ""
foreach mf [lrange $argv 4 end] {
    set f [open $mf r]
    set bspCpu ""
    set bspTool ""
    while {[gets $f line] >= 0} {
        if {[string index $line 0] == "#"} {
	    continue
	}
	set field1 [lindex $line 0]
	if {$field1 == "CPU"} {
	    set bspCpu [lindex $line 2]
	}
	if {$field1 == "TOOL"} {
	    set bspTool [lindex $line 2]
	}
    }
    close $f

    if {("$bspCpu" == "$CPU") && ("$bspTool" == "$TOOL")} {
	regsub "/Makefile" $mf "" dirToBuild
	lappend dirs $dirToBuild
    }
}

foreach dirToBuild $dirs {
    # echo directory we are building for diagnostics
    puts ""
    puts "Building $dirToBuild ..."
    # echo make command for diagnostics
    puts "make -C $dirToBuild CPU=$CPU TOOL=$TOOL \
	TARGET=$TARGET TGT_DIR=$TGT_DIR $TARGET"

    set tgtdir $TGT_DIR
    regsub -all {\\}  $tgtdir {\\\\} TGT_DIR

    set status [catch "exec make -C $dirToBuild \
	CPU=$CPU TOOL=$TOOL TARGET=$TARGET TGT_DIR=$TGT_DIR \
	$TARGET" make_log] 

    # spitout log for all to see
    puts "$make_log"

    # removed check to see if build failed or if TCL failed.
    # it wasn't able to tell the difference.
}
