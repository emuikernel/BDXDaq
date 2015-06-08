# CrossWind.tcl - Debugger GUI Tcl implementation file
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 03b,04mar03,tpw  Partial fix for SPR 86310, rename PPC 'fpcr' to 'fpscr'.
# 03a,08apr02,jab  fixed ppc non-gpr register window display
# 02z,04apr02,jab  fixed arm register display problem (SPR 75049)
# 02y,21feb02,h_k  chenged the SH-DSP registers order (SPR #73524).
# 02x,02oct01,zl   Removed obsolete shdspRegisterVector and 
#                  shdspRegisterStructure, optimized SH shRegisterVector.
# 02w,15jul01,tpw  Add MIPS large register value support to unix crosswind.
# 02v,27jun01,tpw  Add mips64 register lists.
# 02w,28may01,kab  Changed hasAltivec position in WTX_MSG_TS_INFO for
#		   backward compat.
# 02v,07may01,tpw  Remove gratuitous blank line.
# 02u,25oct00,rsh  add dsp support for SH
# 02t,15mar00,zl   merged SH support from T1
# 02v,13feb01,dtr  Adding in fpp and altivec probe.
# 02u,07feb01,dtr  Adding check for FPU on processor.
# 02t,24jan01,dtr  Looking at changes to Crosswind for Altivec processor.
# 02u,19sep00,tpw  Clone m68k (sans floating point) to create coldfire.
# 02t,18may99,rsh  merge wrs-es.tor1_0_1.mcore-f -> T2
# 02t,30oct99,dra  Removed fsr from MIPS register list.
# 02t,06jun99,dra  added SPARCV9 support.
# 02s,26apr99,tpw  Change name of hppa arg[0-3] registers to match GDB change.
#                  (SPR #26935)
# 02r,24feb99,dbt  no longer try to print "cpsr" register value in register
#                  window for sparc processors (SPR #23838).
# 02q,19feb99,jmp  added startNetscapeFromUI variable to workaround (SPRs
#                  #24072, #24300).
# 02p,08feb99,dbt  added support for hardware breakpoints.
# 02o,04feb99,dbt  added a menu to attach the debugger to the target system
#                  from the UI (Fixed SPR #24884).
# 02n,19nov98,jmp  no longer source windHelpLib.tcl on startup, source it on
#                  first windHelp invocation.
# 02m,10nov98,tpw  add support for new 'status exception' message.
# 02l,04nov98,jmp  added Manuals index entry to the help menu.
# 02k,30sep98,jmp  modified help menus to use windHelpLib instead of windman.
# 02j,14sep98,fle  modified fetchDisassemblyRange since wtxMemDisassemble new
#                    output format
# 02i,09sep98,p_m  Source button now says C/C++
# 02h,08sep98,jmp  fixed debuggerPid value.
# 02g,08sep98,jmp  merged ARM support.
# 02f,20apr98,dbt  removed tclX references.
# 02e,19dec97,jdi  changed windman call to use -m option (-k has bug).
# 02e,19sep97,fle  Modified for using wtxMemDisassemble service => erased all 
#                    calls to memBlockDis ().
#                  + modified fetchDisassemblyRange for the wtxMemDisassemble
#                    service now takes care of read-only data at the end of
#                    procedure code
#                  + got rid of makeDisasmSymList() (not used anymore)
#                  + got rid of cwSymbolicAddr() (using wtxMemDisassemble one)
#                  + got rid of cwDemangle() (wrong and not used anymore)
# 02d,17dec96,elp  added target type in newtarget protocol message.
# 02c,05nov96,dvs  changed calls to memBlockDis to use asmClass
# 02i,07nov96,sjw  Allow forgetHandler to handle multiple files, SPR #6895
# 02h,17oct96,sjw  Put a catch around the wtxMemRead in fetchDisassemblyRange
#		   in case the address is bad (due to bad PC value)
# 02g,05sep96,pad  The attached Target Server name is now displayed in the
#                  GUI's title bar
# 02f,30aug96,pad  the available target list is now displayed in a separate
#                   dialog so that it can be updated.
#                   Instated calls to noticePost, instead of dialogPost
#                   where they ought to be. Strengthened taskAttachCb{}.
#                   Re-organized code following WRS Tcl coding conventions.
# 02e,12aug96,jmb  added ease patch (kab) for HPSIM support.
# 02d,25jul96,jco  removed the entry in File menu for downloading again
#		    last downloaded file and made instead the starting
#		    directory for the file selector tool be the directory 
#		    name of the last downloaded file.
# 02c,23jul96,jco  added the memory of the latest downloaded object file.
# 02b,04jun96,kkk  added mips support. (from ease)
# 02a,24may96,c_s  fix SPR #6559.
# 01z,01apr96,jco  added addTrans to correctly handle hight addresses 
#		    comparisons in makeDisasmSymList routine.
# 01y,15mar96,c_s  Sped up disassembly (using WTX); added routine cmt. blocks
# 01x,09feb96,yp   fixed reg var decl for ppc
# 01w,20jan95,tpr  Added register definition for PowerPC.
# 01v,11dec95,c_s  Fixed SPR 5663.
# 01u,03dec95,c_s  Fixed SPRs 5567, 5529, 4622.
# 01t,16nov95,c_s  clean up disassembly to take into account read-only data
#                    inserted between procedures.
# 01s,14nov95,c_s  turn GDB signal messages into something meaningful for 
#                    crosswind users.  Do busy animation while running
#                    program from GUI.
# 01r,11nov95,c_s  fix SPR #5408.
# 01q,06nov95,jco  added About menu.
# 01p,27oct95,c_s  added download dialog (SPR #5156).  Made toolbar buttons
#                    persistent.  Added busy-animation code.  Made it so 
#                    here marker is grayed out during program execution.
#                    Added task attachment dialog and interruption support.
#                    Moved init file to new location (~/.wind).
# 01o,10oct95,c_s  motion commands don't listen to GDB's result; the debugger
#                    now sends program signal/stop information via events.
#                    Use new editFile interface provided in host.tcl.
#                    Now, dragging a toolbar button to the `button' button
#                    deletes it.
# 01n,28sep95,s_w  rename i386* and 68k* variables to i86* and m68k* as
#                    part of SPR 4916 fix
# 01m,17aug95,c_s  catch Tcl error that might be returned from
#                    gdbStackFrameTags.  sourceBkptToggle fixed (SPR #4687).
#                    added register information for i960.  Motion commands
#                    (step, cont, next, until etc.) now listen to GDB's result
#                    and pop up a window if something goes wrong (SPR #4621
#                    etc.)
# 01l,30jun95,s_w  fixed bug in getting m68k status register - use $ps not $sr
# 01k,23jun95,c_s  fixed print and print*: they should not use downtcl because
#                    we wish to see the result of the invocation.
# 01j,20jun95,jcf  updated manual.
# 01i,07jun95,c_s  now use downtcl -noresult rather than ttySend for most
#                    interactive commands.  'Displays' menu item removed.
#                    You can now 'tbreak' a selected routine.  We now sort
#                    extension files before sourcing them.  Fixed arrays of 
#                    dimension > 2.  Added i86 support.
# 01h,19may95,c_s  name change.
# 01g,07may95,c_s  event handling reworked.  Now handler procs have explicit
#                    argument lists for safer argument handling.  Autodisplays
#                    are now fulfilled at the ready signal.  Destroying an 
#                    autodisplay window now sends the "undisplay" command to 
#                    GDB so that it will stay in sync.
# 01f,25apr95,c_s  use wtxPath; now gets backtrace info and auto-display data
#                    when we get `status ready';  now gets existing server 
#                    list directly from wtxtcl.
# 01e,10apr95,c_s  removed some debug printing.
# 01d,10apr95,c_s  added button making dialog.  Added target and run/stop
#                    display support.  Now sources app-config dir.  Makes
#                    a menu of WTX servers known at startup time. When program
#                    exits or is killed, arrow goes away.
# 01c,02apr95,c_s  we now call exit rather than quit.  Added sourceBkptToggle
#                    which is used with mouse-click breakpointing.
#                    Updated hierarchy highlight calls to match new 
#                    interface.
# 01b,02jan95,c_s  minor tweaks, now that hierarchy callbacks include name
#                    and value.
# 01a,10jul94,c_s  written.
#*/

# globals

set initFileDir		$env(HOME)/.wind		;# user's custom dir
set initFileName	${initFileDir}/crosswind.tcl	;# user's custom file

set debugProtocolMessages	0	;# A flag that controls whether
					;# protocol messages are printed to
					;# stdout when received for debugging
					;# purposes.

set oldBtFrameNum	0	;# previous backtrace frame number
set lastContextSource	0	;# true if a source file is set
set pendingContext	""	;# context of the current frame

set downloadFilter	"*.o"	;# only object files are considered by default

set needAuxProcs	0	;# no auxiliary procedure executed by default
set cwAuxProcList	""	;# list of proc to call when target prog stops

set needStack 0			;# true when the backtrace window needs update
#set currentTarget	"???"	;# current target status
set pendingDisplays	""	;# list of auto-display updates to do
set pendingDispSetup	""	;# list of inspect window updates to do
set readyCommandQueue	""	;# list of commands to execute when ready
set debuggerPid		-1	;# process ID of the underlying GDB
set gdbReady		1	;# true if GDB can answer commands
set readyCount		0
set hwBpListIsPosted	0	;# true if hardware breakpoint list dialog is 
				;# posted
set hwBpTypeList	""	;# available hardware breakpoint types

set cwBusyMessage	"Cannot issue command:\nGDB is busy."

# Hardware breakpoints Poll interval: 5 seconds. At this interval, we see if 
# the list of hardware breakpoints has changed.

set hwBpPollInterval 5000       

# On HPUX, netscape cannot be started from the Launcher, Browser, Crosswind
# (See SPR# 24072, 24300). We defined startNetscapeFromUI to FALSE, so
# when the HTML help is invoked on HPUX, windHelpDisplay{} check if the
# variable startNetscapeFromUI is defined to FALSE, if yes, we display
# an error message.

set startNetscapeFromUI FALSE
set hasAltivec 0 ;#This is set to 1 if an altivec unit is avail on target
set hasFpp 0     ;#This is set to 1 if a Floating point unit is avail on target

################################################################################
#
# createForms - create all menus and buttons
#
# This routine create all the required menus, buttons and dialogs of CrossWind.
#
# SYNOPSIS:
# createForms
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc createForms {} {

    menuCreate		File	F 
    menuButtonCreate	File	"Download..."	D 	downloadDialog
    menuButtonCreate	File	"Quit"		Q	exit

    menuCreate		Targets		T 
    menuButtonCreate	Targets		"Attach Task..."	A \
	taskAttach
    menuButtonCreate	Targets		"Attach System"		A \
	systemAttach
    menuButtonCreate	Targets		"Detach Task/System"	D \
	{cwDownTcl -noresult gdb detach}
    menuButtonCreate	Targets		"Kill Task"		K \
	{cwDownTcl -noresult gdb kill}
    menuButtonCreate	Targets		-
    menuButtonCreate	Targets		"Connect Target Servers..."	C \
	targetListPost

    menuCreate		Source	S 
    menuButtonCreate	Source	"C/C++"		C	{sourceDisplayMode 0}
    menuButtonCreate	Source	"Assembly"	A	{sourceDisplayMode 2}
    menuButtonCreate	Source	"Mixed"		M	{sourceDisplayMode 1}   

    menuCreate		Tcl	c
    menuButtonCreate	Tcl	"Reread Home"	H	tclRereadHome
    menuButtonCreate	Tcl	"Reread All"	A	tclRereadAll

    menuCreate		Windows		W
    menuButtonCreate	Windows		"Registers"	R \
	{setUpRegisterWindow}
    menuButtonCreate	Windows		"Backtrace"	B \
	{hierarchyPost Backtrace}

    menuCreate		About	A
    menuButtonCreate	About	"Tornado"	T	{tornadoVersion}

    menuCreate -help	Help	H
    menuButtonCreate	Help	"On CrossWind"		O	{crosswindHelp}
    menuButtonCreate	Help	"Manuals contents"	M	{tornadoHelp}
    menuButtonCreate    Help    "Manuals index"         i       {windmanHelp}


    toolBarItemCreate	break 	button	{actOnSelection break} \
	"dragReturnString break" \
	"sourceBpDelete"
    toolBarItemCreate	tbreak	button	{actOnSelection tbreak} \
	"dragReturnString tbreak" \
        "sourceBpDisable"
    toolBarItemCreate	hbreak	button	{hwBpListPost}
    toolBarItemCreate	halt	button  {interruptDebugger}

    toolBarItemCreate	sep-1	space

    toolBarItemCreate	step	button	stepButton
    toolBarItemCreate	next 	button	nextButton
    toolBarItemCreate	sep-2	space
    toolBarItemCreate	cont	button	{gdbMotionCommand cont}
    toolBarItemCreate	finish	button	{gdbMotionCommand finish}

    toolBarItemCreate	sep-3	space
    toolBarItemCreate	up	button	{cwDownTcl -noresult gdb up}
    toolBarItemCreate	down	button	{cwDownTcl -noresult gdb down}
    toolBarItemCreate	sep-4	space
    toolBarItemCreate	edit	button	{sourceEdit {trytodrag}}\
	"dragReturnString editor"
    toolBarItemCreate	print	button	{printSelection}
    toolBarItemCreate	printstar	button	{printStarSelection}
    toolBarItemCreate	inspect		button	{inspectSelection}
    toolBarItemCreate	inspectstar	button	{inspectStarSelection}
    toolBarItemCreate	sep-5	space
    toolBarItemCreate	button	button	{dialogPost Button} \
	{dragReturnString ""} \
	{deleteToolbarButton}

    # Toolbar button creation dialog

    dialogCreate Button -size 250 230 {
	{button OK -left 0 -right 50 -bottom . \
	    {makeButton; dialogUnpost Button}}
	{button Cancel -left 50 -right 100 -bottom . {dialogUnpost Button}}
	{text Name -hspan -top .}
	{multitext Action -hspan -top Name -bottom OK}
    }

    # Attach to task dialog

    dialogCreate "Attach" -size 200 270 {
	{button Attach -left 0 -right 50 -bottom . taskAttachCb}
	{button Cancel -left 50 -right 100 -bottom . {dialogUnpost "Attach"}}
	{list Task -hspan -top . -bottom Attach}}

    # Avalaible target list dialog

    dialogCreate "Connect" -size 300 200 {
	{button Connect -left 0 -right 50 -bottom . targetConnectCb}
	{button Cancel -left 50 -right 100 -bottom . {dialogUnpost "Connect"}}
	{list "Target Servers" -hspan -top . -bottom Connect}}

    # Set hardware breakpoints dialog

    dialogCreate "Set Hardware Breakpoint..." -size 250 250 {
	{button "OK" -left 0 -right 50 -bottom . hwBpSet}
	{button Cancel -left 50 -right 100 -bottom . \
			    {dialogUnpost "Set Hardware Breakpoint..."}}
	{text "Address or Symbol Name:" -hspan -bottom OK}
	{list "Breakpoint Type:" -hspan -top . \
					-bottom "Address or Symbol Name:"}
    }

    # List hardware breakpoints dialog

    dialogCreate "Hardware Breakpoint List" -size 600 200 {
	{button Delete -left 0 -right 33 -bottom . hwBpDelete}
	{button "Add..." -left 33 -right 66 -bottom . hwBpSetPost}
	{button Cancel -left 66 -right 100 -bottom . hwBpListUnpost}
	{list "Hardware Breakpoints" -hspan -top . -bottom Cancel}
    }
}

##############################################################################
#
# gdbMotionCommand - run GDB motion command with popup result 
#
# Runs the command in $args.
#
# SYNOPSIS:
#   gdbMotionCommand
#
# PARAMETERS:
#   args: motion command
#
# RETURNS: N/A
#
# ERRORS: N/A

proc gdbMotionCommand {args} {
    global gdbReady
    
    wsBusyAnimation on
    if {$gdbReady} {
	cwDownTcl -noresult gdb $args
    } else {
	addReadyCommand cwDownTcl -noresult gdb $args
    }
}

##############################################################################
#
# stepButton - callback attached to toolbar "step" button
#
# GDB is asked to "step" or "stepi", depending on the current value of 
# srcDisplayMode.
#
# SYNOPSIS:
#   stepButton
#
# PARAMETERS:
#   all are ignored.
#
# RETURNS: N/A
#
# ERRORS: N/A

proc stepButton {args} {
    global srcDisplayMode
    global lastContextSource

    if {$srcDisplayMode != 0 || ! $lastContextSource} {
	gdbMotionCommand stepi
    } else {
	gdbMotionCommand step
    }
}

##############################################################################
#
# nextButton - callback attached to toolbar "next" button
#
# GDB is asked to "next" or "nexti", depending on the current value of 
# srcDisplayMode.
#
# SYNOPSIS:
#   nextButton
#
# PARAMETERS:
#   all are ignored.
#
# RETURNS: N/A
#
# ERRORS: N/A

proc nextButton {args} {
    global srcDisplayMode
    global lastContextSource

    if {$srcDisplayMode != 0 || ! $lastContextSource} {
	gdbMotionCommand nexti
    } else {
	gdbMotionCommand next
    }
}

##############################################################################
#
# printSelection - ask GDB to print the selected variable.
#
# GDB is asked to print as a variable the text selected via the mouse.
#
# SYNOPSIS:
#   printSelection
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc printSelection {} {
    set sel [checkSelection]
    if {$sel != ""} {
	ttySend "print $sel\n"
    }
}

##############################################################################
#
# printStarSelection - ask GDB to print the selected pointer contents.
#
# GDB is asked to print as a pointer contents the text selected via the mouse.
#
# SYNOPSIS:
#   printStarSelection
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc printStarSelection {} {
    set sel [checkSelection]
    if {$sel != ""} {
	ttySend "print * $sel\n"
    }
}

##############################################################################
#
# printSelectionType - ask GDB to print the selected type
#
# GDB is asked to print as a type the text selected via the mouse.
#
# SYNOPSIS:
#   printSelectionType
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc printSelectionType {} {
    set sel [checkSelection]
    if {$sel != ""} {
	cwDownTcl -noresult gdb ptype $sel
    }
}

##############################################################################
#
# inspectSelection - display a pop-up with a variable contents
#
# A pop-up windows is displayed, which holds the contents of a variable
# selected via the mouse.
#
# SYNOPSIS:
#   inspectSelection
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc inspectSelection {} {
    set sel [checkSelection]
    if {$sel != ""} {
	cwDownTcl -noresult gdb disp/W $sel
    }
}

##############################################################################
#
# inspectStarSelection - display a pop-up with a pointer contents
#
# A pop-up windows is displayed, which holds the contents of a pointer
# selected via the mouse.
#
# SYNOPSIS:
#   inspectStarSelection
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc inspectStarSelection {} {
    set sel [checkSelection]
    if {$sel != ""} {
	cwDownTcl -noresult gdb disp/W * $sel
    }
}

##############################################################################
#
# actOnSelection -
#
# (what ?)
#
# SYNOPSIS:
#   actOnSelection command
#
# PARAMETERS:
#   command:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc actOnSelection {command} {
    set sel [selection]
    if {$sel != ""} {
	cwDownTcl -noresult gdb $command $sel
    } else {
	cwDownTcl -noresult gdb $command
    }
}

##############################################################################
#
# checkSelection - check whether a text has been selected
#
# This routine checks whether or not a text has been selected with the mouse
# prior to clicking the 'print' or 'inspect' buttons.
#
# SYNOPSIS:
#   checkSelection
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc checkSelection {} {
    set sel [selection]
    if {$sel == ""} {
	noticePost warning "Select a word with the\nleft mouse button first."
	return ""
    }
    return $sel
}

##############################################################################
#
# sourceDrop - control of Source Widget.
#
# This routine controls the Source Widget drop behavior
#
# SYNOPSIS:
#   sourceDrop args
#
# PARAMETERS:
#   args: one of "break", "tbreak", "cont", "here", "editor" or "#event".
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceDrop {args} {

    set aspec [lrange $args 1 end]

    case [lindex $args 0] { 

	# A breakpoint at the current source line is requested.
	"break"		{sourceBkptSet $aspec}
	"tbreak"	{sourceTBkptSet $aspec}
	"cont"		{sourceUntilSet $aspec}
	"here"		{sourceUntilSet $aspec}
	"editor"	{sourceEdit $aspec}

	"#event"	{sourceWvEvent $aspec}
    }
}

##############################################################################
#
# formGdbAddress - convert a (what ?) to GDB address
#
# This routine converts a (what ?) into an address GDB can understand.
#
# SYNOPSIS:
#   formGdbAddress aspec
#
# PARAMETERS:
#   aspec:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc formGdbAddress {aspec} {

    # if args came in "braced up", remove outer layer of braces

    if {[llength $aspec] == 1} {
	set aspec [lindex $aspec 0]
    }
    
    set file [lindex $aspec 0]
    set line [lindex $aspec 1]

    if {$file == "address"} {
	return [format "*%#x" $line]
    } else {
	return [file tail $file]:$line
    }
}

##############################################################################
#
# sourceBkptSet - set a breakpoint
#
# This routine sets a breakpoint at location where the "break" symbol has
# been put.
#
# SYNOPSIS:
#   sourceBkptSet args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceBkptSet {args} {
    cwDownTcl -noresult gdb break [formGdbAddress $args]
}

##############################################################################
#
# sourceBkptToggle -
#
# (what ?)
#
# SYNOPSIS:
#  sourceBkptToggle args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceBkptToggle {args} {
    # find out if there's a breakpoint on this line.

    set file [file tail [lindex $args 0]]
    set line [lindex $args 1]
    if {$file == "address"} {
	set address [lindex $args 1]
    } else {
	set address -1
    }

    foreach mark [markList] {
	if {[string match "*break" [lindex $mark 1]]} {
	    set mFile [lindex $mark 2]
	    set mLine [lindex $mark 3]
	    set mAddress [lindex $mark 4]
	    
	    if {$mAddress == $address 
	        || ($mFile == $file && $mLine == $line)} {
		if {[scan [lindex $mark 0] "bp%d" bpNum] == 1} {
		    cwDownTcl -noresult gdb delete $bpNum
		    return
		}
	    }
	}
    }

    cwDownTcl -noresult gdb break [formGdbAddress $args]
}

##############################################################################
#
# sourceTBkptSet - set a task breakpoint
#
# This routine sets a task breakpoint at location where the "tbreak" symbol has
# been put.
#
# SYNOPSIS:
#   sourceTBkptSet args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceTBkptSet {args} {
    cwDownTcl -noresult gdb tbreak [formGdbAddress $args]
}

##############################################################################
#
# sourceUntilSet -
#
# (what ?)
#
# SYNOPSIS:
#  sourceUntilSet args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceUntilSet {args} {
    gdbMotionCommand until [formGdbAddress $args]
}

##############################################################################
#
# sourceWvEvent -
#
# (what ?)
#
# SYNOPSIS:
#  sourceWvEvent args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceWvEvent {arg} {
    set address [lindex [lindex [lindex $arg 3] 1] 0]
    cwDownTcl gdb list *$address
}

##############################################################################
#
# sourceEdit - edit the selected source file
#
# This routine is called when the "editor" symbol is dropped within the
# source window. It calls the text editor.
#
# SYNOPSIS:
#  sourceEdit args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceEdit {arg} {
    global env

    if {$arg == "trytodrag"} {
	noticePost error "Drag the edit button to the source line to edit."
	return
    }

    set file [lindex $arg 0]
    set line [lindex $arg 1]


    if {$file == "address"} {
	noticePost error "Can't edit assembly code.\nChoose a source line."
	return
    }

    editFile -background $file $line
}
    
##############################################################################
#
# sourceDrag - mark area drag source behavior
#
# Mark area drag source behavior.  Dragging from the mark area 
# simply drags the name of the mark at the position of the 
# cursor.
#
# SYNOPSIS:
#  sourceDrag args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceDrag {args} {
    return $args
}

##############################################################################
#
# sourceBpDelete - delete a breakpoint symbol.
#
# SYNOPSIS:
#  sourceBpDelete markName
#
# PARAMETERS:
#   markName:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceBpDelete {markName} {
    actOnMark $markName delete
}

##############################################################################
#
# sourceBpDisable - disable a breakpoint symbol.
#
# SYNOPSIS:
#  sourceBpDisable markName
#
# PARAMETERS:
#   markName:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc sourceBpDisable {markName} {
    actOnMark $markName disable
}

##############################################################################
#
# actOnMark - mark widget click behavior
#
# Mark widget click behavior.  actOnMark is called whenever a mark is 
# clicked in the mark area. 
#
# SYNOPSIS:
#  actOnMark mark action
#
# PARAMETERS:
#   mark:
#   action
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc actOnMark {mark action} {
    if {$action == "delete"} {
	if {[regexp {bp([0-9]*)} $mark dummy bpnum]} {
	    cwDownTcl -noresult gdb delete $bpnum
	    return
	}
    }
    if {$action == "disable"} {
	if {[regexp {bp([0-9]*)} $mark dummy bpnum]} {
	    cwDownTcl -noresult gdb disable $bpnum
	    return
	}
    }
}

##############################################################################
#
# markDrop - mark area drop behavior
#
# Mark area drop behavior
#
# SYNOPSIS:
#  markDrop args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc markDrop {args} {

    # if the message has the form "here <filename> <line number>", 
    # construct an until command that will advance execution to that 
    # point.

    if {[llength $args] == 3} {
	set mark [lindex $args 0]
	set fname [lindex $args 1]
	set lno [lindex $args 2]

	if {$mark == "here" && $fname != "address"} {
	    gdbMotionCommand until [file tail $fname]:$lno
	}
    }
}

##############################################################################
#
# makeButton - create a button
#
# This routine dynamically creates a new button in the button bar.
#
# SYNOPSIS:
#  makeButton
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc makeButton {} {
    global initFileName
    set buttonInfo [dialogGetValues Button]
    set bName [lindex $buttonInfo 0]
    set bAction [lindex $buttonInfo 1]

    if {$bName != "" && $bAction != ""} {

	# if there's one or more newlines at the start or end,
	# get rid of them, then append a newline.  Since extra and/or missing
	# newlines aren't very visible in the dialog, we want to protect 
	# the user from (a) defining a button with no newline, which won't
	# execute until return is hit, and (b) defining a button with 
	# multiple newlines, which will cause gdb to execute the command
	# and then repeat it!

	set bAction [format "%s\n" [string trim $bAction "\n"]]
	toolBarItemCreate $bName button [list ttySend $bAction]

	# Now we want to make the button persistent.  Store its definition
	# in the init file, creating that file if it doesn't exist.

	if {! [file exists $initFileName]} {
	    exec touch $initFileName
	}

	if {[file writable $initFileName]} {

	    # open the file and read in all the lines.  Accumulate them
	    # in "cwLine."

	    set cwLine ""
	    set cwTcl [open $initFileName r]
	    
	    while {[gets $cwTcl line] != -1} {
		lappend cwLine $line
	    }

	    close $cwTcl

	    # Replace the newlines in the definition with the \n 
	    # escape sequence.  Similarly, " goes to \", \ -> \\.
	    
	    # first double the backslashes we find.

	    set tmp ""
	    for {set ix 0} {$ix < [string length $bAction]} {incr ix} {
		if {[set c [string index $bAction $ix]] == "\\"} {
		    set tmp "$tmp\\\\"
		} else {
		    set tmp "$tmp$c"
		}
	    }

	    regsub -all "\n" $tmp "\\n" tmp2
	    regsub -all "\"" $tmp2 "\\\"" newAction

	    # form the new definition string.

	    set newDef [list toolBarItemCreate $bName button]
	    set newDef [format "%s {ttySend \"%s\"}" $newDef $newAction]

	    # Find a line that contains a definition of this button.
	    # Override it with the new definition.

	    set replaced 0
	    set ix 0
	    set newLines ""
	    foreach line $cwLine {
		if [string match "toolBarItemCreate $bName *" $line] {
		    lappend newLines $newDef
		    set replaced 1
		} else {
		    lappend newLines $line
		}
	    }

	    # If we got through that and the definition isn't replaced, 
	    # add it now.

	    if {! $replaced} {
		lappend newLines $newDef
	    }

	    # Write the file back out.

	    set cwTcl [open $initFileName w]
	    foreach line $newLines {
		puts $cwTcl $line
	    }

	    close $cwTcl
	}
    }
}


#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# DEBUG ENGINE PROTOCOL HANDLING
#

##############################################################################
#
# protocolMessageHandler - dispatch debug-engine protocol message
#
# This routine is the main dispatch center for the debug-engine protocol
# messages.
#
# SYNOPSIS:
#  protocolMessageHandler
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc protocolMessageHandler {args} {
    global debugProtocolMessages

    if {$debugProtocolMessages} {
	puts stdout $args
    }

    set messageHandler [lindex $args 0]Handler

    if {[info procs $messageHandler] == ""} {
	puts stdout [format "unhandled protocol message: %s" \
		$args]
    } else {
	eval [concat $messageHandler [lrange $args 1 end]]
    }
}

##############################################################################
#
# contextHandler - handle "context" protocol message
#
# This routine responds to the "context" protocol message, a request
# by the debug engine to display a source context and annotate it.  The 
# first argument is the frame number (where zero represents the innermost
# frame, and the numbers ascend from there).  The following three arguments
# are the source file, line number, and machine address.  The source file
# may be the string "nil" if the source file is not known to the debug 
# engine (and in this case the line number is ignored).
#
# SYNOPSIS:
#  protocolMessageHandler frame file lno addr
#
# PARAMETERS:
#   frame: frame number
#   file:  source file name
#   lno:   line number
#   addr:  machine address
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc contextHandler {frame file lno addr} {
    global oldBtFrameNum
    global pendingContext
    global lastContextSource

    if {$file == "nil"} {
	set lastContextSource 0
    } else {
	set lastContextSource 1
    }

    if {$frame > 0} {
	# If the frame number is >0, we mark it as an ancestor.
	# Sometimes GDB simply writes -1 for the frame number--
	# we treat that as the "here" frame.
	set markType "ancestor"
    } else {
	set frame 0
	set markType "here"
    }
    
    set pendingContext [list $markType $frame $file $lno $addr]

    hierarchyUnhighlight Backtrace "{Call Stack} $oldBtFrameNum"
    hierarchyHighlight   Backtrace "{Call Stack} $frame"

    set oldBtFrameNum $frame
}

##############################################################################
#
# listHandler - handle "list" protocol message
#
# This routine responds to the "list" protocol message, a request from the 
# debug engine to display a certan source or machine context.  If the 
# first argument is "address", the second argument will contain the address
# to display in machine format.  Otherwise the two arguments represent the
# source file and line number.
#
# SYNOPSIS:
#  listHandler file lno
#
# PARAMETERS:
#   file:  source file name, or "address" keyword
#   lno:   line number, or address to display
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc listHandler {file lno} {
    global srcDisplayMode
    if {$file == "address"} {
	addReadyCommand sourceContextDisplay nil 0 $lno
    } else {
	if {$srcDisplayMode == 2} {
	    addReadyCommand findAssembly $file $lno
	} else {
	    addReadyCommand sourceContextDisplay $file $lno -1
	}
    }
}

##############################################################################
#
# architectureHandler - handle "architecture" protocol message.
#
# This routine responds to the "architecture" protocol message,
# which announces the architecture of the debugged target.  We expect this
# message early, and record the result.  It is used to dispatch architecture-
# specific procedures in this file.
#
# SYNOPSIS:
#  architectureHandler arch
#
# PARAMETERS:
#   arch:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc architectureHandler {arch} {
    global architecture
    set architecture $arch
}

##############################################################################
#
# statusHandler - handle "status" protocol message.
#
# This routine responds to the "status" message.  This message reports
# changes in the execution status of the target program.  The argument
# describes what the new status is (ex. "stop" for when the program has
# stopped at a breakpoint).  We signal text windows so that they may
# refresh their context and refresh the backtrace window also after 
# fetching an up-to-date list of stack frame tags from the debug engine.
#
# SYNOPSIS:
#  statusHandler status args
#
# PARAMETERS:
#   status: one of "run", "target", "stop", "exception", "busy" or "ready".
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc statusHandler {status args} {
    global architecture
    global currentTarget
    global gdbReady
    global debuggerPid
    global needStack
    global needAuxProcs
    global displayExpression
    global pendingDisplays
    global pendingDispSetup
    global displayExpression
    global readyCommandQueue
    global pendingContext
    global cwAuxProcList
    global readyCount

    set gdbReady 0
    if {$status == "stop"} {
	set needStack 1
	set needAuxProcs 1
    } elseif {$status == "exception"} {
	set needStack 1
	set needAuxProcs 1
    } elseif {$status == "busy"} {
	# start the busy animation.
	wsBusyAnimation on
	return
    } else {
	if {$status == "ready"} {

	    incr readyCount
	    set gdbReady 1
	    wsBusyAnimation off

	    # If we haven't learned GDB's pid yet, do that now.

	    if {$debuggerPid == -1 && $readyCount > 1} {
		# HPUX seems to like it if we give GDB a moment
		# to start up.  So, we only do this the second
		# time GDB signals its readiness.
		set debuggerPid [lindex [cwDownTcl pid] 0]
	    }

	    # Display the current context.

	    if {$pendingContext != ""} {
		set pc $pendingContext
		set pendingContext ""
		bindNamesToList {markType frame file lno addr} $pc
		sourceContextDisplay $file $lno $addr
		markSet here $markType [file tail $file] $lno $addr
	    }

	    # time to update the backtrace window.

	    if {$needStack} {

		set needStack 0

		# reconfigure backtrace window
		if [catch {cwDownTcl gdbStackFrameTags} frameTags] {
		    # GDB got sick trying to figure out the call stack.
		    # Just put "unknown" in level 1 of the stack.

		    hierarchySetValues Backtrace "\{unknown\}"
		} else {
		    hierarchySetValues Backtrace "\{$frameTags\}"
		}

		if {[hierarchyExists Registers] 
		    && [hierarchyIsDisplayed Registers]} {
		    hierarchySetValues Registers \
			    [${architecture}RegisterVector]
		}

	    }

	    if {$needAuxProcs} {

		set needAuxProcs 0

		# now any auxiliary procedures the user has asked for.

		foreach auxProc $cwAuxProcList {
		    $auxProc
		}
	    }

	    # If there are any commands to execute in the readyCommandQueue,
	    # execute them now.

	    set rcq $readyCommandQueue
	    set readyCommandQueue ""
	    foreach command $rcq {
		eval $command
	    }

	    # If there are any pending inspect window setups, process them 
	    # now.

	    set psetup $pendingDispSetup
	    set pendingDispSetup ""
	    foreach setup $psetup {
		set dispNum [lindex $setup 0]
		set dispExpr [lindex $setup 1]
		set displayExpression($dispNum) $dispExpr

		if [catch {cwDownTcl gdb pptype $dispExpr} exType] {
		    # error -- something is faulty about the expression,
		    # or GDB's protocol extension can't understand its type
		    unset displayExpression($dispNum)
		} else {
		    hierarchyCreate -destroy displayDestroyed \
			-change -special \
			[displayName $dispNum] \
			"displayTraverse $dispNum"
		    hierarchySetStructure [displayName $dispNum] \
			[collectDataMembers $exType $dispExpr "" 0]
		    lappend pendingDisplays $dispNum
		}
	    }

	    # If there are any pending auto-display updates, process them
	    # now.

	    set pd $pendingDisplays
	    set pendingDisplays ""
	    foreach dispNum $pd {
		if [info exists displayExpression($dispNum)] {
		    if [catch \
			    {cwDownTcl gdb ppval $displayExpression($dispNum)} \
			    exVal] {
			# error--maybe the expression can't be displayed or is 
			# defective somehow.
		    } else {
			set dname [displayName $dispNum]
			if [hierarchyExists $dname] {
			    hierarchySetValues $dname $exVal
			    if {![hierarchyIsDisplayed $dname]} {
				hierarchyPost $dname
			    }
			}
		    }
		}
	    }
	    
	    return
	}
    }

    case $status {
	stop		{debugStatusSet "$currentTarget/Stopped"}
	exception	{debugStatusSet "$currentTarget/Exception"}
	run		{debugStatusSet "$currentTarget/Running"; fixRunMark}
	target		{setTargetStatus $args}
	*		{debugStatusSet "$msg"}
    }
}

##############################################################################
#
# fixRunMark - shade out the "here" mark when execution resumes.
#
# This routine replaces the "here" mark with a shaded-out mark after the 
# target resumes execution, so the user won't think the program is still 
# stopped there.
#
# SYNOPSIS:
#  fixRunMark
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc fixRunMark {} {
    if {[set info [markInfo here]] != ""} {
	bindNamesToList {type file line addr} $info
	markSet here ancestor $file $line $addr
    }
}

##############################################################################
#
# noticeHandler - handle notice protocol messages
#
# This routine reacts to a notice message by posting a notice box.
#
# SYNOPSIS:
#  noticeHandler type args
#
# PARAMETERS:
#   type: type of the message
#   args: message
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc noticeHandler {type args} {

    # Suppress the message that would appear on receipt of a ^C.

    if {$type == "warning" && $args == "Program terminated with SIGINT."} {
	return
    }

    # Convert the message about SIGTRAP to one about an unexpected breapoint.

    if {$type == "warning" && $args == "Program terminated with SIGTRAP."} {
	catch {noticePost warning \
		"Unexpected breakpoint\n(perhaps one set by another tool)"}
	return
    }

    # post a notice containing the result.
    catch {noticePost $type $args}
}

##############################################################################
# 
# setTargetStatus - modify the debug status.
#
# This routine decides what to put in the debug status window based
# on a new configuration of the target stack.
#
# SYNOPSIS:
#  setTargetStatus stack
#
# PARAMETERS:
#   stack: type of the stack
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc setTargetStatus {stack} {
    global currentTarget

    case [lindex $stack 0] {
	exec		{set currentTarget "executable"}
	child		{set currentTarget "process"}
	wtx		{set currentTarget "WTX server"}
	wtxsystem	{set currentTarget "WTX System"}
	wtxtask		{set currentTarget "WTX Task"}
	*		{set currentTarget "[lindex $stack 0]"}
    }

    debugStatusSet "Target: $currentTarget"

    # generally a change in the target stack means the program isn't 
    # "stopped" anywhere anymore--it has gone away.  Remove the marks
    # that suggest the program is executing.

    markDelete here
    markDelete ancestor
}

##############################################################################
#
# bpHandler - handle "bp" protocol message
# 
# This routine responds to the "bp" protocol message, which indicates that 
# a breakpoint has been set.  The arguments of the message are the bp number,
# the enable status (integer boolean), the temporary status (integer boolean),
# the source file (or "nil" if it is not known), the line of the source file
# (ignored if the file is "nil"), and the machine address where the breakpoint
# is set.
#
# SYNOPSIS:
#  bpHandler num enable disp file lno addr
#
# PARAMETERS:
#  num:    breakpoint number
#  enable: breakpoint status
#  disp:   breakpoint temporary status
#  file:   source file name
#  lno:    line number
#  addr:   breakpoint machine address
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc bpHandler {num enable disp file lno addr} {
    set markName "bp$num"
    if {! $enable} {
	if {$disp} {
	    set markType "tdbreak"
	} else { 
	    set markType "dbreak"
	}
    } else {
	if {$disp} {
	    set markType "tbreak"
	} else { 
	    set markType "break"
	}
    }

    markSet $markName $markType [file tail $file] \
	    $lno $addr
}

##############################################################################
#
# bpdelHandler - handle "bpdel" protocol message
#
# This routine responds to the "bpdel" protocol message, which indicates
# that a breakpoint has been deleted.  We delete the breakpoint mark from
# the mark list and redisplay all marks (an inefficient means of erasing
# the mark corresponding to the deleted breakpoint).  The argument of the 
# message is the number of the breakpoint that has been deleted.
#
# SYNOPSIS:
#  bpdelHandler bpnum
#
# PARAMETERS:
#   bpnum: breakpoint number
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc bpdelHandler {bpnum} {
    set markName bp$bpnum
    markDelete $markName
}

##############################################################################
#
# uptclHandler - handle "uptcl" protocol message
#
# This routine responds to the "uptcl" protocol message.  This message
# is a catch-all means for the debug engine to request Tcl evaluations
# to take place in the context of the GUI's Tcl interpreter.  We simply
# evaluate the rest of the message (perhaps removing the outermost layer
# of quoting).
#
# SYNOPSIS:
#  uptclHandler args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc uptclHandler {args} {
    if {[llength $args] == 1} {
	# the string has been "braced up" to prevent evaluations
	# in the lower Tcl interpreter.  We must remove these braces
	# to allow evaluation here.  Or maybe it's just a one-word
	# command.  Either way, this will work to evaluate it.

	eval [lindex $args 0]
    } else {
	eval $args
    }
}

##############################################################################
#
# forgetHandler - handle "forget" protocol message
#
# This routine responds to the "forget" protocol message.  This message
# means that a source module should be considered out of date with respect
# to the machine code loaded on the target and so the source code should
# be flushed from the display cache.
#
# SYNOPSIS:
#  forgetHandler files
#
# PARAMETERS:
#   files: source file name list
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc forgetHandler {args} {
    
    foreach filename $args {
	sourceContextForget [file tail $filename]
    }

}


##############################################################################
#
# newtargetHandler - handle "newtarget" protocol message
#
# This routine responds to the "newtarget" protocol message.  This
# message means that a new target has been attached to (or perhaps an 
# old target has been reattached to.)  In any case we must assume that
# all cached information is invalid.
#
# SYNOPSIS:
#  newtargetHandler
#
# PARAMETERS: 
#	type: type of target connection (wtx, vxworks)
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc newtargetHandler {type} {
    sourceContextForgetAll
    if {$type == "wtx"} {
	addReadyCommand titleBarUpdate
	addReadyCommand hwBpDialogInit
    }
}

##############################################################################
#
# titleBarUpdate - update the title bar with the attached server's name
#
# This routine updates the title bar of the GUI with the name of the target
# server currently attached to.
#
# SYNOPSIS:
#  titleBarUpdate
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc titleBarUpdate {} {
    set tgtname [cwDownTcl wtxTargetName]
    wsTitleSet "CrossWind : $tgtname"
}


##############################################################################
#
# dispsetupHandler - set up for a newly displayed expression
#
# This routine sets up for a newly displayed expression.  We get 
# a number and an expression to display.  Record this in a global array,
# so we can look it up later.  Then fetch the type in protocol format, 
# and create a dialog with that type's structure in it.  We'll display
# it when the debug engine tells us its value might have changed.
#
# SYNOPSIS:
#  dispsetupHandler dispNum args
#
# PARAMETERS:
#   dispNum:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

#global displayExpression ()

proc dispsetupHandler {dispNum args} {
    global pendingDispSetup

    # crack open any extraneous braces Tcl might have wrapped around the 
    # argument to protect them, as might happen if the object being displayed
    # contained an array reference (e.g., foo[6].bar would come down to 
    # us as {foo[6].bar}).  By referencing the 0'th element of every
    # 'list', we will rip off any of this extraneous quoting.

    set dispExpr ""
    foreach a $args {
	append dispExpr [lindex $a 0] " "
    }

    # Append this display setup to the list of those to be processed when 
    # GDB is idle.

    lappend pendingDispSetup [list $dispNum $dispExpr]
}

##############################################################################
#
# dispreqHandler -
#
# SYNOPSIS:
#  dispreqHandler dispNum
#
# PARAMETERS:
#   dispNum:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc dispreqHandler {dispNum} {
    global pendingDisplays

    if {[lsearch $pendingDisplays $dispNum] == -1} {
	lappend pendingDisplays $dispNum
    }
}

##############################################################################
#
# dispdelHandler -
#
# SYNOPSIS:
#  dispdelHandler dispNum
#
# PARAMETERS:
#   dispNum:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc dispdelHandler {dispNum} {
    set name [displayName $dispNum]
    if {$name != ""} {
	hierarchyDestroy [displayName $dispNum]
    }
}

##############################################################################
#
# displayDestroyed -
#
# SYNOPSIS:
#  displayDestroyed dispName
#
# PARAMETERS:
#   dispName:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc displayDestroyed {dispName} {
    scan $dispName "(%d)" dispNum
    cwDownTcl -noresult gdb undisplay $dispNum
}

##############################################################################
#
# displayName -
#
# SYNOPSIS:
#  displayName dispNum
#
# PARAMETERS:
#   dispNum:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc displayName {dispNum} {
    global displayExpression
    if {[info exists displayExpression($dispNum)]} {
	return "($dispNum) $displayExpression($dispNum)"
    }
    return ""
}

##############################################################################
#
# displayTraverse -
#
# SYNOPSIS:
#  displayTraverse args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc displayTraverse {args} {
    global displayExpression

    set dNum [lindex $args 0]
    set dStack [lrange $args 1 end]
    set baseExp "$displayExpression($dNum)"

    # Throw away the last element of dStack and reverse the order of the
    # rest and separate them with "."s.  That's the path to the traversed
    # element.

    set dStack [lrange $dStack 0 [expr [llength $dStack]-2]]
    set path ""
    set isRef -1
    foreach d $dStack {
	set element [lindex $d 0]

	# remove any trailing ampersand which indicates a reference;
	# it's not really part of the name of the object (and hence 
	# would confuse the debugger).  If the first element is a 
	# reference, then we don't use * to dereference it as we would 
	# a pointer.

	set l [string length $element]
	if {[string index $element [expr $l - 1]] == "&"} {
	    if {$isRef == -1} {
		set isRef 1
	    }
	    set element [string range $element 0 [expr $l - 2]]
	}
	    
	if {$isRef == -1} {
	    set isRef 0
	}

	if {[regexp {^[0-9]+$} $element]} {
	    # it's an array index.
	    set path "\[$element\]$path"
	} else {
	    set path ".$element$path"
	}
    }

    # if this is a reference, the last thing will be "&".  Then
    # don't dereference, just print the object. Otherwise, it's 
    # a pointer, so dereference with "*".

    if {$isRef == 1} {
	cwDownTcl -noresult gdb disp/W ($baseExp)$path
    } else {
	cwDownTcl -noresult gdb disp/W *($baseExp)$path
    }
}

##############################################################################
#
# collectDataMembers -
#
# SYNOPSIS:
#  collectDataMembers item name prefix level
#
# PARAMETERS:
#   item:
#   name:
#   prefix:
#   level:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc collectDataMembers {item name prefix level} {
    if {[llength $item] == 1} {
	set item [lindex $item 0]
    }

    if {[lindex $item 0] == "struct" || [lindex $item 0] == "union"} {

	set fieldlist [lindex $item 2]
	set retstring ""
	if {$name != ""} {
	    set retstring "\{\"$name\" "
	}
	set retstring "$retstring \{"

	while {[llength $fieldlist] >= 2} {
	    set substring [collectDataMembers [lindex $fieldlist 0]  \
		    [lindex $fieldlist 1] \
		    "    $prefix" [expr $level+1]]
	    set retstring "$retstring$substring"
	    set fieldlist [lrange $fieldlist 2 end]
	}
	if {$name != ""} {
	    set retstring "$retstring \} "
	}

	return "$retstring \} "
    }

    if {[lindex $item 0] == "\[\]"} {
	# It's an array.

	set basetype [lindex [lindex $item 2] 0]

	if [isAggregate $basetype] {
	    # It's a pointer to a compound object.
            set substring [collectDataMembers [lindex $item 2] \
		    "" "    $prefix" [expr $level+1]]
	    if {$name == ""} {
		return "{+ $substring}"
	    } else {
		return "{$name {+ $substring}} "
	    }
	} else {
	    if {$name == ""} {
		return +
	    } else {
		return "{$name +} "
	    }
	}
    }

    # decorate references with a trailing ampersand.

    if {[lindex $item 0] == "&"} {
	set name "${name}&"
    }

    if {([lindex $item 0] == "*" || [lindex $item 0] == "&") \
	    && ([lindex [lindex $item 1] 0] == "struct" \
	    || [lindex [lindex $item 1] 0] == "union" \
            || [lindex [lindex $item 1] 0] == "*")} {
	# Pointer to a struct or union. Prefix the name with
	# '*:' so it will be treated as a traversible element.
	return "\"*:$name\" "
    } else {
	return "\"$name\" "
    }
}

##############################################################################
#
# isAggregate -
#
# SYNOPSIS:
#  isAggregate basetype
#
# PARAMETERS:
#   basetype: one of "struct" or "union".
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc isAggregate {basetype} {
    return [expr \
	{$basetype == "struct" || $basetype == "union" || $basetype == {[]}}]
}

##############################################################################
#
# exposeAllDisplays -
#
# SYNOPSIS:
#  exposeAllDisplays
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc exposeAllDisplays {} {
    foreach disp [hierarchyList] { 
	hierarchyPost $disp
    }
}

##############################################################################
#
# debugHandler -
#
# SYNOPSIS:
#  debugHandler args
#
# PARAMETERS:
#   args:
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc debugHandler {args} {
    set ix 0
    foreach arg $args {
	puts stdout [format "%2d: %s" $ix $arg]
	incr ix
    }
}

##############################################################################
#
# addReadyCommand - add a Tcl command in the Ready queue
#
# This routine adds a Tcl command in the Ready queue so that it will be 
# evaluated later, when GDB is ready.
#
# SYNOPSIS:
#  addReadyCommand args
#
# PARAMETERS:
#   args: Tcl procedure name
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc addReadyCommand {args} {
    global readyCommandQueue

    lappend readyCommandQueue $args
}

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# REGISTER WINDOW PROGRAMMING
#

##############################################################################
#
# setUpRegisterWindow -
#
# SYNOPSIS:
#  setUpRegisterWindow
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc setUpRegisterWindow {} {
    global architecture

    if {$architecture == "" || $architecture == "unknown"} {
	noticePost error "The debugger has not indicated\nits architecture!"
    } else {
	hierarchyCreate -change -special Registers
	hierarchySetStructure Registers [${architecture}RegisterStructure]
	hierarchySetValues Registers [${architecture}RegisterVector]
	hierarchyPost Registers
    }
}

proc infoRegVector { args } {
    set regValues {}
    if { [catch "cwDownTcl gdb info prefetch-registers $args" regs] } {
	set regs {}
    }
    set regs [split $regs \n]
    foreach reg $regs {
	lappend regValues [lindex $reg 1]
    }
    return $regValues
}

##############################################################################
#
# m68kRegisterStructure - 
#
# SYNOPSIS:
#  m68kRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the mc68k registers
#
# ERRORS: N/A
#

proc m68kRegisterStructure {} {
    return "
    {\"68k Registers\" {
        {Data     {d0 d1 d2 d3 d4 d5 d6 d7}}  
        {Address  {a0 a1 a2 a3 a4 a5 a6 a7}}  
        {Other    {pc sr} } } }"
}

##############################################################################
#
# m68kRegisterVector - 
#
# SYNOPSIS:
#  m68kRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the mc68k's register values.
#
# ERRORS: N/A
#

proc m68kRegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$d0 \$d1 \$d2 \$d3 \$d4 \$d5 \$d6 \$d7 \
	    \$a0 \$a1 \$a2 \$a3 \$a4 \$a5 \$fp \$sp \
	    \$pc \$ps]

    return "{{[lrange $rvec 0 7]} \
	    {[lrange $rvec 8 15]} \
	    {[lrange $rvec 16 17]}}"
}

##############################################################################
#
# coldfireRegisterStructure - 
#
# SYNOPSIS:
#  coldfireRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the coldfire registers
#
# ERRORS: N/A
#

proc coldfireRegisterStructure {} {
    return "
    {\"ColdFire Registers\" {
        {Data     {d0 d1 d2 d3 d4 d5 d6 d7}}  
        {Address  {a0 a1 a2 a3 a4 a5 a6 a7}}  
        {Other    {pc sr} } } }"
}

##############################################################################
#
# coldfireRegisterVector - 
#
# SYNOPSIS:
#  coldfireRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the coldfire's register values.
#
# ERRORS: N/A
#

proc coldfireRegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$d0 \$d1 \$d2 \$d3 \$d4 \$d5 \$d6 \$d7 \
	    \$a0 \$a1 \$a2 \$a3 \$a4 \$a5 \$fp \$sp \
	    \$pc \$ps]

    return "{{[lrange $rvec 0 7]} \
	    {[lrange $rvec 8 15]} \
	    {[lrange $rvec 16 17]}}"
}

##############################################################################
#
# sparcRegisterStructure - 
#
# SYNOPSIS:
#  sparcRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the sparc registers
#
# ERRORS: N/A
#

proc sparcRegisterStructure {} {
    return "
    {\"SPARC Registers\" {
        {In     {i0 i1 i2 i3 i4 i5 i6 i7}}  
        {Local  {l0 l1 l2 l3 l4 l5 l6 l7}}  
        {Out    {o0 o1 o2 o3 o4 o5 o6 o7}}  
        {Global {g0 g1 g2 g3 g4 g5 g6 g7}}  
        {Other  {pc npc psr y wim tbr fpsr} } } }"
}

##############################################################################
#
# sparcRegisterVector - 
#
# SYNOPSIS:
#  sparcRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the sparc's register values.
#
# ERRORS: N/A
#

proc sparcRegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$i0 \$i1 \$i2 \$i3 \$i4 \$i5 \$fp \$i7 \
	    \$l0 \$l1 \$l2 \$l3 \$l4 \$l5 \$l6 \$l7 \
	    \$o0 \$o1 \$o2 \$o3 \$o4 \$o5 \$sp \$o7 \
	    \$g0 \$g1 \$g2 \$g3 \$g4 \$g5 \$g6 \$g7 \
	    \$pc \$npc \$psr \$y \$wim \$tbr \$fpsr]
	    
    return "{{[lrange $rvec 0 7]} {[lrange $rvec 8 15]}
            {[lrange $rvec 16 23]} {[lrange $rvec 24 31]}
            {[lrange $rvec 32 38]}}"
}

##############################################################################
#
# sparc64RegisterStructure - 
#
# SYNOPSIS:
#  sparc64RegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the sparcv9 registers
#
# ERRORS: N/A
#

proc sparc64RegisterStructure {} {
    return "
    {\"SPARCV9 Registers\" {
        {In     {i0 i1 i2 i3 i4 i5 fp i7}}  
        {Local  {l0 l1 l2 l3 l4 l5 l6 l7}}  
        {Out    {o0 o1 o2 o3 o4 o5 sp o7}}  
        {Global {g0 g1 g2 g3 g4 g5 g6 g7}}  
        {Other  {pc npc ccr asi pstate cwp tt tl pil y} } } }"
}

##############################################################################
#
# sparc64RegisterVector - 
#
# SYNOPSIS:
#  sparc64RegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the sparcv9's register values.
#
# ERRORS: N/A
#

proc sparc64RegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$i0 \$i1 \$i2 \$i3 \$i4 \$i5 \$fp \$i7 \
	    \$l0 \$l1 \$l2 \$l3 \$l4 \$l5 \$l6 \$l7 \
	    \$o0 \$o1 \$o2 \$o3 \$o4 \$o5 \$sp \$o7 \
	    \$g0 \$g1 \$g2 \$g3 \$g4 \$g5 \$g6 \$g7 \
	    \$pc \$npc \$ccr \$asi \$pstate \$cwp \$tt \$tl \$pil \$y]
	    
    return "{{[lrange $rvec 0 7]} {[lrange $rvec 8 15]}
            {[lrange $rvec 16 23]} {[lrange $rvec 24 31]}
            {[lrange $rvec 32 41]}}"
}

##############################################################################
#
# simsoRegisterStructure -
#
# SYNOPSIS:
#  simsoRegisterStructure
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the simso registers
#
# ERRORS: N/A
#

proc simsoRegisterStructure {} {
    return "
    {\"SPARC Registers\" {
        {In     {i0 i1 i2 i3 i4 i5 i6 i7}}  
        {Local  {l0 l1 l2 l3 l4 l5 l6 l7}}  
        {Out    {o0 o1 o2 o3 o4 o5 o6 o7}}  
        {Global {g0 g1 g2 g3 g4 g5 g6 g7}}  
        {Other  {pc npc psr y wim tbr fpsr} } } }"
}

##############################################################################
#
# simsoRegisterVector -
#
# SYNOPSIS:
#  simsoRegisterVector
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the simso's register values.
#
# ERRORS: N/A
#

proc simsoRegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$i0 \$i1 \$i2 \$i3 \$i4 \$i5 \$fp \$i7 \
	    \$l0 \$l1 \$l2 \$l3 \$l4 \$l5 \$l6 \$l7 \
	    \$o0 \$o1 \$o2 \$o3 \$o4 \$o5 \$sp \$o7 \
	    \$g0 \$g1 \$g2 \$g3 \$g4 \$g5 \$g6 \$g7 \
	    \$pc \$npc \$psr \$y \$wim \$tbr \$fpsr]
	    
    return "{{[lrange $rvec 0 7]} {[lrange $rvec 8 15]}
            {[lrange $rvec 16 23]} {[lrange $rvec 24 31]}
            {[lrange $rvec 32 38]}}"
}

##############################################################################
#
# i86RegisterStructure - 
#
# SYNOPSIS:
#  i86RegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the ix86 registers
#
# ERRORS: N/A
#

proc i86RegisterStructure {} {
    return "
    {\"i86 Registers\" {eax ebx ecx edx esi edi ebp esp eflags pc}}"
}

##############################################################################
#
# i86RegisterVector - 
#
# SYNOPSIS:
#  i86RegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the ix86's register values.
#
# ERRORS: N/A
#

proc i86RegisterVector {} {
    return [list [cwDownTcl gdbEvalScalar \
	    \$eax \$ebx \$ecx \$edx \$esi \$edi \$ebp \$esp \$eflags \$pc]]
}

##############################################################################
#
# i960RegisterStructure - 
#
# SYNOPSIS:
#  i960RegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the i960 registers
#
# ERRORS: N/A
#

proc i960RegisterStructure {} {
    return "{\"i960 Registers\" {
	{Local {pfp sp rip r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15}}
	{Global {g0 g1 g2 g3 g4 g5 g6 g7 g8 g9 g10 g11 g12 g13 g14 fp}}
	{Control {pcw ac tc}}}}"
}

##############################################################################
#
# i960RegisterVector - 
#
# SYNOPSIS:
#  i960RegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the i960's register values.
#
# ERRORS: N/A
#

proc i960RegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$pfp \$sp \$rip \$r3 \$r4 \$r5 \$r6 \$r7 \
	    \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$r14 \$r15 \
	    \$g0 \$g1 \$g2 \$g3 \$g4 \$g5 \$g6 \$g7 \
	    \$g8 \$g9 \$g10 \$g11 \$g12 \$g13 \$g14 \$fp \
	    \$pcw \$ac \$tc]

    return "{{[lrange $rvec 0 15]} {[lrange $rvec 16 31]}
            {[lrange $rvec 32 34]}}"
}


##############################################################################
#
# mipsRegisterStructure - 
#
# SYNOPSIS:
#  mipsRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS: 
#   A hierarchy level holding the mips registers
#
# ERRORS: N/A
#

proc mipsRegisterStructure {} {
    return "{\"MIPS Registers\" {
        {Control { sr pc cause lo hi } }
        {General { zero at v0 v1 a0 a1 a2 a3
                t0 t1 t2 t3 t4 t5 t6 t7
                s0 s1 s2 s3 s4 s5 s6 s7
                t8 t9 k0 k1 gp sp s8 ra }}}}"
    }

##############################################################################
#
# mipsRegisterVector - 
#
# SYNOPSIS:
#  mipsRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS: 
#   a vector holding the mips' register values.
#
# ERRORS: N/A
#

proc mipsRegisterVector {} {
    set regvec [downtcl gdb info prefetch-registers \
         \$sr \$pc \$cause \$lo \$hi \
         \$zero \$at \$v0 \$v1 \$a0 \$a1 \$a2 \$a3 \
         \$t0 \$t1 \$t2 \$t3 \$t4 \$t5 \$t6 \$t7 \
         \$s0 \$s1 \$s2 \$s3 \$s4 \$s5 \$s6 \$s7 \
         \$t8 \$t9 \$k0 \$k1 \$gp \$sp \$s8 \$ra]
    set rvec {}
    foreach reg [split $regvec \n] {
	set r [split $reg]
	lappend rvec [lindex $r 1]
    }

    return [list [list [lrange $rvec 0 4] [lrange $rvec 5 end]]]
}

##############################################################################
#
# mips64RegisterStructure - 
#
# SYNOPSIS:
#  mips64RegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS: 
#   A hierarchy level holding the mips64 registers
#
# ERRORS: N/A
#

proc mips64RegisterStructure {} {
    return [mipsRegisterStructure]
    }

##############################################################################
#
# mips64RegisterVector - 
#
# SYNOPSIS:
#  mips64RegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS: 
#   a vector holding the mips64 register values.
#
# ERRORS: N/A
#

proc mips64RegisterVector {} {
    return [mipsRegisterVector]
}

##############################################################################
#
# ppcRegisterStructure - 
#
# SYNOPSIS:
#  ppcRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the ppc registers
#
# ERRORS: N/A
#

proc ppcRegisterStructure {} {
    global hasAltivec
    global hasFpp
# Here we get target info accessing the wtx calls.
    if [catch {downtcl wtxTargetName} tgt] {
	return "no target"
    }
    if {[catch {downtcl wtxToolAttach} [string trim $tgt] ] != 1 } { 
	puts stdout [catch {downtcl wtxToolAttach} [string trim $tgt] ] 
    }
    if [catch {downtcl wtxTsInfoGet} tsInfo ] {
	return "no target info"
    }

# This indexing is based on WTX_MSG_TS_INFO but it is organised into a
# list in wtformat.c
    set cpuDesc [lindex $tsInfo 2]
    set hasFpp [lindex $cpuDesc 1]
    set hasAltivec [ lindex $cpuDesc 5]
# The register list is split up into 4 distinct groups. 
# It assumes that regList1 is always there 
# ie global and general group of registers.
    set regList1 " \
        { \"PowerPC Registers\" {
	{Global {r0 r1/sp r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16 \
		 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31}}
	{General    {lr ctr pc cr xer } } "
    set regList2 " \
	{Floating-Point \
                {f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13 f14 f15 f16\
	         f17 f18 f19 f20 f21 f22 f23 f24 f25 f26 f27 f28 f29 f30 f31\
                 fpscr}}"
    set regList3 " \
        {Vector {v0 v1 v2 v3 v4 v5 v6 v7 v8 v9 v10 v11 v12 v13 v14 v15 v16\
		 v17 v18 v19 v20 v21 v22 v23 v24 v25 v26 v27 v28 v29 v30 v31\
		 vscr vrsave } }"
     set regList4 " } }"
     set regList  $regList1
     if { $hasFpp == 1 } { 
	 set regList  "$regList $regList2" 
     }
     if { $hasAltivec == 1 } {
	 set regList "$regList $regList3" 
     }
# The final list returned will return FPU and Altivec registers only if 
# present on the target attached to.     
     return "$regList $regList4"
}

##############################################################################
#
# ppcRegisterVector - 
#
# SYNOPSIS:
#  ppcRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the ppc's register values.
#
# ERRORS: N/A
#

proc ppcRegisterVector {} {
global hasAltivec  
global hasFpp
# The register vector must match up with the register set in 
# ppcRegisterStructure. The choice about wether FPU and Altivec present depends
# on result obtained in ppcRegisterStructure.
  set rvec1 [ downtcl gdbEvalScalar \
	    \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \$r8 \$r9 \$r10 \$r11 \
	    \$r12 \$r13 \$r14 \$r15 \$r16 \$r17 \$r18 \$r19 \$r20 \$r21 \$r22 \
	    \$r23 \$r24 \$r25 \$r26 \$r27 \$r28 \$r29 \$r30 \$r31 ]
  set rvec4 [ downtcl gdbEvalScalar \$lr \$ctr \$pc \$cr \$xer ] 
  if { $hasFpp == 1 } { 
     set rvec2 [ infoRegVector \
	    f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 \
	    f12 f13 f14 f15 f16 f17 f18 f19 f20 f21 f22 \
	    f23 f24 f25 f26 f27 f28 f29 f30 f31 fpscr ]
  }
  if { $hasAltivec == 1 } {
# gdbAltivec is a new function to enable access to altivec size registers
# The result is in the format 0xnnnnnnnn_nnnnnnnn_nnnnnnnn_nnnnnnnn. 
     set rvec3 [ downtcl  gdbEvalAltivec \
	    \$v0 \$v1 \$v2 \$v3 \$v4 \$v5 \$v6 \$v7 \$v8 \$v9 \$v10 \$v11 \
	    \$v12 \$v13 \$v14 \$v15 \$v16 \$v17 \$v18 \$v19 \$v20 \$v21 \$v22 \
	    \$v23 \$v24 \$v25 \$v26 \$v27 \$v28 \$v29 \$v30 \$v31 \$vrsave \$vscr]
  }

  set rvecList "{$rvec1} {$rvec4}"
  if { $hasFpp == 1 } {
      set rvecList1 "{$rvec2}"
      set rvecList "$rvecList $rvecList1" 
  }
  if { $hasAltivec == 1 } {
      set rvecList2 "{$rvec3}"
      set rvecList "$rvecList $rvecList2" 
  }
  set rvecList "{$rvecList}"
  
  return $rvecList

}

##############################################################################
#
# hppaRegisterStructure - 
#
# SYNOPSIS:
#  hppaRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the hppa registers
#
# ERRORS: N/A
#

proc hppaRegisterStructure {} {
    return "
    {\"PA-RISC Registers\" {
	{Control {pc npc psw mask cr11}}
	{General {r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16
		    r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29
		    r30 r31}}
	{Space   {sr0 sr1 sr2 sr3 sr4 sr5 sr6 sr7} } } }"
}

##############################################################################
#
# hppaRegisterVector - 
#
# SYNOPSIS:
#  hppaRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the hppa's register values.
#
# ERRORS: N/A
#

proc hppaRegisterVector {} {

    # These register names have to match the names used in gdbRegs(),
    # which need to match the names used in tm.h.

    set rvec [downtcl gdbEvalScalar \
	    \$pcoqh \$pcoqt \$ipsw \$flags \$sar \
	    \$r1 \$rp \$r3 \$r4 \$r5 \$r6 \$r7 \$r8 \
	    \$r9 \$r10 \$r11 \$r12 \$r13 \$r14 \$r15 \$r16 \
	    \$r17 \$r18 \$r19 \$r20 \$r21 \$r22 \$r23 \$r24 \
	    \$r25 \$r26 \$dp \$ret0 \$ret1 \$sp \$r31 \
	    \$sr0 \$sr1 \$sr2 \$sr3 \$sr4 \$sr5 \$sr6 \$sr7]

    return "{{[lrange $rvec 0 4]} \
	    {[lrange $rvec 5 35]} \
	    {[lrange $rvec 36 end]}}"
}

##############################################################################
#
# mcoreRegisterStructure - 
#
# SYNOPSIS:
#  mcoreRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the M.CORE registers
#
# ERRORS: N/A
#

proc mcoreRegisterStructure {} {
    return "
    {\"M.CORE Registers\" {
        {General  {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15}}  
        {Other    {pc psr} } } }"
}

##############################################################################
#
# mcoreRegisterVector - 
#
# SYNOPSIS:
#  mcoreRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the M.CORE's register values.
#
# ERRORS: N/A
#

proc mcoreRegisterVector {} {
    set rvec [cwDownTcl gdbEvalScalar \
	    \$sp \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
	    \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$r14 \$r15 \
	    \$pc \$psr]

    return "{{[lrange $rvec 0 15]} \
	    {[lrange $rvec 16 17]}}"
}

##############################################################################
#
# armRegisterStructure -
#
# SYNOPSYS:
#  armRegisterStructure
# 
# PARAMETERS: N/A
#
# ERRORS: N/A

proc armRegisterStructure {} {
    set regs [downtcl gdb info prefetch-registers]
    set rvec {}
    foreach reg [split $regs \n] {
	set r [lindex $reg 0]
	if { [string trim $r] != "" } {
	    lappend rvec $r
	}
    }

    return "{\"ARM Registers\" {$rvec}}"
}

##############################################################################
#
# armRegisterVector -
#
# SYNOPSYS:
#  armRegisterVector
#
# PARAMETERS: N/A
#
# ERRORS: N/A

proc armRegisterVector {} {
    set regs [downtcl gdb info prefetch-registers]
    set rvec {}
    foreach reg [split $regs \n] {
	set r [lindex $reg 1]
	if { [string trim $r] != "" } {
	    lappend rvec $r
	}
    }

    return "{$rvec}"
}

##############################################################################
#
# shRegisterStructure - 
#
# SYNOPSIS:
#  shRegisterStructure 
#
# PARAMETERS: N/A
#
# RETURNS:
#   A hierarchy level holding the sh registers
#
# ERRORS: N/A
#

proc shRegisterStructure {} {
    global __wtxCpuType
    global cpuType
    global hasFpp

    if {[catch {downtcl wtxTargetName} tgt]} {
	puts stdout "$tgt"
    }
    if {[catch {downtcl wtxToolAttach} [string trim $tgt] ] != 1 } { 
	puts stdout [catch {downtcl wtxToolAttach} [string trim $tgt] ] 
    }
    if [catch {downtcl wtxTsInfoGet} tsInfo ] {
	puts stdout "$tsInfo"
    }

    set cpuDesc [lindex $tsInfo 2]
    set hasFpp  [lindex $cpuDesc 1]
    set __wtxCpuType [lindex $cpuDesc 0]

    switch -regexp $cpuType($__wtxCpuType) {
        SH7000 {
            return "
            {\"SH Registers\"
                {{General {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 fp sp}}
                 {System {mach macl pr pc}}
                 {Control {sr gbr vbr}}}}"
        }
        SH7600 {
            return "
            {\"SH Registers\" 
	        {{General {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 fp sp}}
	         {System {mach macl pr pc}}
	         {Control {sr gbr vbr}}}}
            {\"DSP Registers\"
                {{Control {rs re dsr mod}}
                 {Data {a0g a0 a1g a1 m0 m1 x0 x1 y0 y1}}}}"
        }
        SH7700 {
            if { $hasFpp } {
                return "
                {\"sh Registers\" 
	           {{General {r0 r1 r2 r3 r4 r5 r6 r7 
                              r8 r9 r10 r11 r12 r13 fp sp}}
                    {System {mach macl pr pc}}
	            {Control {sr gbr vbr}}}}
                {\"Floating Point Registers\"
	            {{Control {fpul fpscr}}
                     {Data {fr0 fr1 fr2 fr3 fr4 fr5 fr6 fr7 
	                    fr8 fr9 fr10 fr11 fr12 fr13 fr14 fr15}}}}"
            } else {
                return "
                {\"sh Registers\" 
	           {{General {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 fp sp}}
	            {System {mach macl pr pc}}
	            {Control {sr gbr vbr}}}}
                {\"DSP Registers\"
                    {{Control {rs re dsr mod}}
                     {Data {a0g a0 a1g a1 m0 m1 x0 x1 y0 y1}}}}"
            }
        }
        SH7750 {
            return "
            {\"sh Registers\" 
               {{General {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 fp sp}}
                {System {mach macl pr pc}}
	        {Control {sr gbr vbr}}}}
            {\"Floating Point Registers\"
               {{Control {fpul fpscr}}
                {Data {fr0 fr1 fr2 fr3 fr4 fr5 fr6 fr7
                       fr8 fr9 fr10 fr11 fr12 fr13 fr14 fr15
                       xf0 xf1 xf2 xf3 xf4 xf5 xf6 xf7
                       xf8 xf9 xf10 xf11 xf12 xf13 xf14 xf15}}}}"
        }
        default {
            return "
            {\"SH Registers\"
                {{General {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 fp sp}}
                 {System {mach macl pr pc}}
                 {Control {sr gbr vbr}}}}"
        }
    }
}

##############################################################################
#
# shRegisterVector - 
#
# SYNOPSIS:
#  shRegisterVector 
#
# PARAMETERS: N/A
#
# RETURNS:
#   a vector holding the SH's register values.
#
# ERRORS: N/A
#

proc shRegisterVector {} {
    global __wtxCpuType
    global cpuType
    global hasFpp
 
    switch -regexp $cpuType($__wtxCpuType) {
        SH7000 {
            set rvec [cwDownTcl gdbEvalScalar \
	        \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	        \$mach \$macl \$pr \$pc \
	        \$sr \$gbr \$vbr]

            return "{{[lrange $rvec 0  15]}
	             {[lrange $rvec 16 19]}
	             {[lrange $rvec 20 22]}}"
        }
        SH7600 {
            set rvec [cwDownTcl gdbEvalScalar \
	        \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	        \$mach \$macl \$pr \$pc \
	        \$sr \$gbr \$vbr \
	        \$rs \$re \$dsr \$mod \
                \$a0g \$a0 \$a1g \$a1 \$m0 \$m1 \$x0 \$x1 \$y0 \$y1]

            return "{{[lrange $rvec 0  15]}
	             {[lrange $rvec 16 19]}
	             {[lrange $rvec 20 22]}}
	            {{[lrange $rvec 23 26]}
	             {[lrange $rvec 27 36]}}"
        }
        SH7700 {
            if { $hasFpp } {
                set rvec [cwDownTcl gdbEvalScalar \
	            \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                    \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	            \$mach \$macl \$pr \$pc \
	            \$sr \$gbr \$vbr \
	            \$fpul \$fpscr \$fr0 \$fr1 \$fr2 \$fr3 \$fr4 \$fr5 \$fr6 \
	            \$fr7 \$fr8 \$fr9 \$fr10 \$fr11 \$fr12 \$fr13 \$fr14 \$fr15]

                return "{{[lrange $rvec 0  15]}
	                 {[lrange $rvec 16 19]}
	                 {[lrange $rvec 20 22]}}
	                {{[lrange $rvec 23 24]}
	                 {[lrange $rvec 25 40]}}" 
            } else {
                set rvec [cwDownTcl gdbEvalScalar \
	            \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                    \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	            \$mach \$macl \$pr \$pc \
	            \$sr \$gbr \$vbr \
	            \$rs \$re \$dsr \$mod \
                    \$a0g \$a0 \$a1g \$a1 \$m0 \$m1 \$x0 \$x1 \$y0 \$y1]

                return "{{[lrange $rvec 0  15]}
	                 {[lrange $rvec 16 19]}
	                 {[lrange $rvec 20 22]}}
	                {{[lrange $rvec 23 26]}
	                 {[lrange $rvec 27 36]}}"
            }
        }
        SH7750 {
                set rvec [cwDownTcl gdbEvalScalar \
	            \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                    \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	            \$mach \$macl \$pr \$pc \
	            \$sr \$gbr \$vbr \
	            \$fpul \$fpscr \$fr0 \$fr1 \$fr2 \$fr3 \$fr4 \$fr5 \$fr6 \
	            \$fr7 \$fr8 \$fr9 \$fr10 \$fr11 \$fr12 \$fr13 \$fr14 \$fr15\
	            \$xf0 \$xf1 \$xf2 \$xf3 \$xf4 \$xf5 \$xf6 \$xf7\
	            \$xf8 \$xf9 \$xf10 \$xf11 \$xf12 \$xf13 \$xf14 \$xf15]

                return "{{[lrange $rvec 0  15]} 
	                 {[lrange $rvec 16 19]} 
             	         {[lrange $rvec 20 22]}} 
                        {{[lrange $rvec 23 24]}  
 	                 {[lrange $rvec 25 56]}}" 
        }
        default {
            set rvec [cwDownTcl gdbEvalScalar \
	        \$r0 \$r1 \$r2 \$r3 \$r4 \$r5 \$r6 \$r7 \
                \$r8 \$r9 \$r10 \$r11 \$r12 \$r13 \$fp \$sp \
	        \$mach \$macl \$pr \$pc \
	        \$sr \$gbr \$vbr]

            return "{{[lrange $rvec 0  15]}
	             {[lrange $rvec 16 19]}
	             {[lrange $rvec 20 22]}}"
        }
    }
}

##############################################################################
#
# windowEvaluate -
#
# A generic window collector that evaluates a GDB expression.
#
# SYNOPSIS:
#  windowEvaluate args 
#
# PARAMETERS:
#   args:
#
# RETURNS:
#   The result of the evaluation
#
# ERRORS: N/A
#

proc windowEvaluate {args} {
    #
    # The last argument is the signal, which we ignore.
    #
    set gdbExpr [lrange $args 0 [expr [llength $args]-2]]
    return [cwDownTcl gdb $gdbExpr]
}

##############################################################################
#
# dragReturnString -
#
# SYNOPSIS:
#  dragReturnString string args 
#
# PARAMETERS:
#   string:
#   args: not used
#
# RETURNS:
#   The string.
#
# ERRORS: N/A
#

proc dragReturnString {string args} {
    return $string
}

##############################################################################
#
# tclRereadHome - re-read the user's customization file
#
# This routine forces the Tcl interpretor embedded into GDB to re-avaluate
# the contents of the user's customization file.
#
# SYNOPSIS:
#  tclRereadHome
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#


proc tclRereadHome {} {
    global env
    global initFileName

    if [file readable $initFileName] {
	uplevel #0 source $initFileName
    }
}

##############################################################################
#
# tclRereadAll - re-read the all the Tcl resource files
#
# This routine forces the Tcl interpretor embedded into GDB to re-avaluate
# the contents of the user's customization file and all the application
# dependent Tcl files.
#
# SYNOPSIS:
#  tclRereadAll
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tclRereadAll {} {
    global env

    uplevel #0 source [wtxPath host resource tcl]CrossWind.tcl
    tclRereadHome
}

##############################################################################
#
# crosswindHelp - get help about CrossWind
#
# This procedure runs the windHelp tool, set on the crosswind command.
#
# SYNOPSIS:
#  crosswindHelp
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc crosswindHelp {} {

    if {[info proc windHelpDisplay] == ""} {
    	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    set htmlAddr [windHelpLinkGet -name crosswind]

    if {$htmlAddr != ""} {
	windHelpDisplay $htmlAddr
    } else {
    	error "Tornado help files not installed"
    }
}

##############################################################################
#
# tornadoHelp - get help about Tornado.
#
# This procedure runs the windHelp tool.
#
# SYNOPSIS:
# tornadoHelp
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc tornadoHelp {} {
    global docDir

    if {[info proc windHelpDisplay] == ""} {
    	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    windHelpDisplay $docDir/books.html
}


################################################################################
#
# windmanHelp - launch windman
#
# This procedure runs the windman tool
#
# SYNOPSIS:
#	windmanHelp
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc windmanHelp {} {
    exec windman &
}


#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# Procedures for acquiring disassembly from the debug engine
#

##############################################################################
#
# lineAddressRange - determine target addresses
#
# This routine finds the range of target addresses corresponding
# to the given source line.
#
# SYNOPSIS:
#  lineAddressRange sourceFile sourceLine
#
# PARAMETER:
#  sourceFile: source file name
#  sourceLine: line number
#
# RETURNS:
#  the begin and end addresses matching the source line.
#
# ERRORS: N/A

proc lineAddressRange {sourceFile sourceLine} {

    set infoLine [cwDownTcl gdb info line [file tail $sourceFile]:$sourceLine]

    if {! [string match  {*contains no code*} $infoLine] \
	    && ! [string match {*out of range*} $infoLine]} {
	set a0 0
	set a1 0
	regexp {.*(0x[0-9a-fA-F]*).*(0x[0-9a-fA-F]*).*} \
		$infoLine total a0 a1
	
	if {$a1 > $a0} {
	    return [concat $a0 $a1]
	} else {
	    return ""
	}
    } else {
	return ""
    }
}

##############################################################################
#
# disassembleRange - fetch the disassembly of the given target memory range.
#
# SYNOPSIS:
#  disassembleRange loAddr hiAddr
#
# PARAMETER:
#  loAddr: low address of disassembly
#  hiAddr: high address of disassembly
#
# RETURNS:
#  the disassembly codes within this range.
#
# ERRORS: N/A

proc disassembleRange {loAddr hiAddr} {
    return [fetchDisassemblyRange $loAddr $hiAddr]
}

##############################################################################
#
# fetchDisassemblyRange - fetch disassembly of a memory block (using WTX) 
#
# Queries the target server and WTXTCL for the 
# disassembly of the given target memory range.  The format expected by 
# the caller of this routine is 
#
# lowAddress  --should be equal to the loAddr supplied to this routine
# highAddress --the first byte not contained in the disassembly
# numeric-address <symbolic-address>: instruction [commentary]
# ...
#
# This routine is designed to message the output of wtxtcl's wtxMemDisassemble
# command into this format.
#
# SYNOPSIS:
#  fetchDisassemblyRange loAddr hiAddr
#
# PARAMETER:
#  loAddr: low address of disassembly
#  hiAddr: high address of disassembly
#
# RETURNS:
#  the disassembly codes within this range.
#
# ERRORS: N/A
#

proc fetchDisassemblyRange {loAddr hiAddr} {

    uplevel #0 source [wtxPath host resource tcl]wtxcore.tcl

    set symbolList {}

    if [catch {cwDownTcl wtxTargetName} tgt] {return}
    wtxToolAttach [string trim $tgt]

    # get the list of disassembled instructions from the target server

    set dis [wtxMemDisassemble -address -opcodes $loAddr 0 $hiAddr]
    set dis [split $dis \n]

    # get rid of the last \n empty element

    set length [llength $dis]
    set dis [lrange $dis 0 [expr $length - 2]]

    set disTop $loAddr
    set symbolList [wtxSymListGet -value $loAddr]
    set baseSym [lindex [lindex $symbolList 0] 0]

    # get the base symbol address and stick the 0x prefix if needed

    set baseAddr [lindex [lindex $symbolList 0] 1]
    if {! [string match 0x* $baseAddr]} {
	set baseAddr 0x$baseAddr
    }

    # for each line, we want to construct something in GDB
    # disassembly format:  "<symbol[+offset]>: instr operands"

    set clean 1

    while {$clean} {
	set clean 0
	set dlen [llength $dis]
	set ix 0

	foreach dline $dis {

	    if {$ix == $dlen - 1} {

		# we have reached the end of the disassembled lines to get,
		# detach from target server, update last line, and return

		wtxToolDetach
		set nextAddr [lindex $dline 2]
		if {! [string match 0x* $nextAddr]} {
		    set nextAddr 0x$nextAddr
                }
		return [list $loAddr $nextAddr $outLines]
	    }

	    # if the address doesn't start with "0x", stick it on.

	    set addr [lindex $dline 2]
	    if {! [string match 0x* $addr]} {
		set addr 0x$addr
	    }

	    incr ix

	    # see if a symbol name matches that address

	    if { [set symbolName [lindex $dline 0]] != {} } {
		set baseSym $symbolName
		set baseAddr [lindex $dline 2]
		if {! [string match 0x* $baseAddr]} {
		    set baseAddr 0x$baseAddr
		}
	    }

	    # get offset between symbol name and current address


	    set offset [expr $addr - $baseAddr]
	    if {! [string match 0x* $offset]} {
		set offset 0x$offset
	    }

	    set disassembledInstruction [lindex $dline 4]

	    if {[addTrans $offset] > 0} {
                lappend outLines [format "%#x <%s + %#x>: %s\n" \
					 $addr $baseSym $offset \
					 $disassembledInstruction]

            } else {
		lappend outLines [format "%#x <%s>: %s\n" \
					 $addr $baseSym \
					 $disassembledInstruction]
            }
	}
    }

    wtxToolDetach
    error "could not fetch disassembly"
}

##############################################################################
#
# addTrans - transpose an address for comparison purpose.
#
# this is required to avoid some hight addresses being interpreted as negative 
# values. We shift right once (arithmetic right shift) and mask to force a 
# zero at the MSB site of a 32 bit integer.
#
# SYNOPSIS:
#  addTrans addr
#
# PARAMETER:
#  addr: address to transpose
#
# RETURNS:
#  the transposed address.
#
# ERRORS: N/A

proc addTrans {addr} {

    set addrT [expr $addr >> 1]
    set addrT [expr $addrT & 0x7fffffff] 
    return $addrT
}

##############################################################################
#
# fetchDisassemblyRange.old - get disassemlby of a memory block (using GDB)
#
# Queries the debug engine for the disassembly of 
# the given target memory range.  The format expected by the caller of 
# this routine is 
#
# lowAddress  --should be equal to the loAddr supplied to this routine
# highAddress --the first byte not contained in the disassembly
# numeric-address <symbolic-address>: instruction [commentary]
# ...
#
# This routine is designed to massage the output of GDB's disass
# command into this format.  Trailing newlines and commentary are 
# removed from the GDB output and the relevant addresses are prepended.
#
# GDB won't tell us what the address of the "next" byte is after the 
# disassembly, which is very useful information to have!  To synthesize
# it, we don't consume the last disassembled instruction, and use its 
# address as the endpoint of the dump.  Since this function is used
# to disassemble blocks of target memory for the disassembly cache,
# the wasted effort is negligible.
#
# SYNOPSIS:
#  fetchDisassemblyRange.old loAddr hiAddr
#
# PARAMETER:
#  loAddr: low address of disassembly
#  hiAddr: high address of disassembly
#
# RETURNS:
#  the disassembly codes within this range.
#
# ERRORS: N/A

proc fetchDisassemblyRange.old {loAddr hiAddr} {
    set dLines [split [getTgtSvrDisasm $loAddr $hiAddr] "\n"]

    # Clean up the disassembly.  If there was a block of read-only data between
    # two procedures, it might have desynchronized the disassembly.  Repeatedly
    # scan the list until these have been removed.

    set lastPreenAddr 0
    set cleanFlag 1
    while {$cleanFlag} {

	set cleanFlag 0
	set oldSym ""
	set oldOffset 0
	set ix 0
	foreach dLine $dLines {
	    set off 0
	    
	    # Examine the address of the instruction by cracking the 
	    # line "<base+offset>: instr" with a regular expression.
	    # If we see that symbol "base" is different than the one 
	    # before it, and the offset is nonzero, restart the disassembly
	    # at the exact address "base" and throw away the desynchronized
	    # portion.

	    if {[regexp {^(0x[0-9a-f]+) <([^+]*)(\+([0-9]+))?>:.*} \
		    $dLine whole addr sym xoff off] > 0} {
		if {$oldSym != ""} {
		    # Beware of mst_exext...it need not be aligned,
		    # but we shouldn't react if it isn't, because it
		    # can point inside a procedure
		    # Sometimes entry points have two labels
		    # (e.g.: .mul, _mul).  GDB might disassemble
		    # the first instruction with the symbol name .mul,
		    # and subsequent ones with the symbol name _mul.
		    # So we don't consider it a problem if the symbol
		    # name changes after an instruction with offset 0.
		    if {$oldSym != $sym && $sym != "mst_etext" &&
		        $off != "" && $off != 0 && $oldOffset != 0} {

			if {$addr != $lastPreenAddr} {
			    set lastPreenAddr $addr

			    set cleanFlag 1
			    set newDLines [getTgtSvrDisasm 
				    [expr $addr-$off] $hiAddr]
			    set dLines [lrange $dLines 0 [expr $ix-1]]
			    set dLines [concat $dLines $newDLines]
			    break
			}
		    }

		    set oldSym $sym
		    set oldOffset $off

		} else {
		    set oldSym $sym
		    set oldOffset $off
		}
	    }
	    incr ix
	}
    }

    set dLength [llength $dLines]
    
    # Use the line of the last disassembled instruction as the end address 
    # of the block, then throw it away.

    set lastDLine [lindex $dLines [expr $dLength - 2]]
    set dLines [lrange $dLines 0 [expr $dLength - 3]]

    # now the first token of lastDLine will be the "real" end of 
    # the disassembly range.

    set realEndAddr [lindex $lastDLine 0]
    
    wsSync
    return [list $loAddr $realEndAddr $dLines]
}

##############################################################################
#
# sourceDisplayMode - set the source display mode.
#
# Valid mode numbers are:
#
# 0: C mode
# 1: Mixed C/Assembly mode
# 2: Pure Assembly mode
#
# SYNOPSIS:
#  sourceDisplayMode args
#
# PARAMETER:
#  args: source display mode
#
# RETURNS:
#  the source display mode.
#
# ERRORS: N/A

proc sourceDisplayMode {args} {
    global srcDisplayMode
    global __srcDisplayMode
    global gdbReady

    if {$srcDisplayMode != $__srcDisplayMode} {
	# The GUI can currently set the value of this Tcl variable
	# incorrectly, so we maintain a shadow.
	set srcDisplayMode $__srcDisplayMode
    }

    set context [sourceContext]
    if {$srcDisplayMode == ""} {
	set srcDisplayMode 0
	set oldSrcDisplayMode 0
    } else {
	set oldSrcDisplayMode $srcDisplayMode
    }

    if {$args != ""} {
	set srcDisplayMode $args
	set __srcDisplayMode $args
    }

    if {$srcDisplayMode != $oldSrcDisplayMode} {
	if {[markInfo here] == ""} {
	    # There's no here mark, so GDB didn't help us position the 
	    # screen.  We have to do it ourselves.

	    if {$context != ""} {

		set src [lindex $context 0]
		set ctx [lindex $context 1]

		switch $srcDisplayMode {
		    2 {
			# If we are switching to assembly, try to find 
			# the right address to display.
			if {$src != "nil"} {
			    set type [lindex $ctx 0]
			    if {$type == "address"} {
				set addr [lindex $ctx 1]
				if {$gdbReady} {
				    sourceContextDisplay nil 0 $addr
				} else {
				    noticePost warning "GDB is busy."
				}
			    } else {
				# must be some kind of file.  Try to 
				# figure out the address of the line.
				findAssembly $type [lindex $ctx 1]
			    }
			}
		    }
		    0 {
			# If we are switching to C mode, try to find the right 
			# line.

			if {[lindex $ctx 0] == "address"} {
			    set addr [lindex $ctx 1]
			    cwDownTcl -noresult gdb list *$addr
			} else {
			    # must be in mixed mode, looking at a 
			    # source line. easy enough.
			    if {[lindex $ctx 0] != ""} {
				sourceContextDisplay \
				    [lindex $ctx 0] [lindex $ctx 1]
			    }
			}
		    }
		    1 {
			if {[lindex $ctx 0] == "address"} {
				set addr [lindex $ctx 1]
				cwDownTcl -noresult gdb list *$addr
			} else {
			    if {[lindex $ctx 0] != ""} {
				sourceContextDisplay \
				    [lindex $ctx 0] [lindex $ctx 1]
			    }
			}
		    }
		}
	    }

	    sourceRedisplay
	} else {
	    sourceRedisplay
	    markGoto here
	}
    }

    return $srcDisplayMode
}

##############################################################################
#
# findAssembly - find machine address corresponding to a source line
#
# This calls GDB's info line command and interprets the result.  If the 
# result is valid, sourceContextDisplay is called to set the code window
# to the indicated address.
#
# SYNOPSIS:
#  findAssembly file line
#
# PARAMETER:
#  file: source file name
#  line: line number
#
# RETURNS:
#  the machine address of the line
#
# ERRORS: N/A

proc findAssembly {file line} {
    set f [file tail $file]
    set result [cwDownTcl gdb info line ${f}:${line}]
    # crack first address
    if {[regexp {(0x[0-9a-f]+) } $result whole addr]} {
	sourceContextDisplay nil 0 $addr
    }
}
    
##############################################################################
#
# backtraceCallback - set the debug engine to the selected frame
#
# This routine is called when a stack level is clicked in the
# backtrace window.  Generate a command that will set the debug engine
# to the indicated frame (represented by a number, where zero
# represents the innermost frame).
#
# SYNOPSIS:
#  backtraceCallback args
#
# PARAMETER:
#  args:
#
# RETURNS: N/A
#
# ERRORS: N/A

proc backtraceCallback {args} {
    set frame [lindex [lindex $args 0] 0]
    cwDownTcl -noresult gdb frame $frame
}

##############################################################################
#
# deleteToolbarButton - removes definition of toolbar button from init file
#
# SYNOPSIS:
#  deleteToolbarButton args
#
# PARAMETER:
#  args:
#
# RETURNS: N/A
#
# ERRORS: N/A

proc deleteToolbarButton {args} {
    global env
    global initFileName
    global initFileDir

    if {$args != ""} {
	toolBarItemDelete $args

	# delete the definition from the init file.

	if {[file writable $initFileName] && [file writable $initFileDir]} {
	    exec rm -f ${initFileName}.bak
	    exec mv $initFileName ${initFileName}.bak
	    set filter "^toolBarItemCreate"
	    lappend filter $args
	    append filter " button"

	    # if resulting file  has no lines, grep will exit abnormally,
	    # so we catch errors here.

	    catch {exec grep -v $filter ${initFileName}.bak \
		    > $initFileName}
	}
    }
}

##############################################################################
#
# downloadDialog - pop up download dialog box.
#
# SYNOPSIS:
#  downloadDialog
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc downloadDialog {} {
    global downloadFilter
    global initFileDir

    set startDir .
    if [file exists $initFileDir/.crosswind_last_downloaded] {
        set lastFile [exec cat $initFileDir/.crosswind_last_downloaded]
        if {$lastFile != "" } {
	    if [file exists $lastFile] {
		set startDir [file dirname $lastFile]
	    }
	}
    }

    if {$startDir == "."} {
	cwDownTcl -noresult cd .
	set gdbWdir [cwDownTcl pwd]
    } else {
	set gdbWdir $startDir
    }
    cd [string trim $gdbWdir "\n"]
    set result [noticePost fileselect Download Load $downloadFilter]
    if {$result != ""} {
	ttySend "load $result\n"
	exec /bin/rm -f $initFileDir/.crosswind_last_downloaded
	exec echo $result > $initFileDir/.crosswind_last_downloaded
    }
}

##############################################################################
#
# cwAuxProcAdd - add a procedure to call when program execution stops
#
# This routine adds a procedure to the list of those to be called when 
# target program execution stops.
#
# SYNOPSIS:
#  cwAuxProcAdd auxproc
#
# PARAMETER:
#  auxproc: procedure to add
#
# RETURNS: N/A
#
# ERRORS: N/A

proc cwAuxProcAdd {auxproc} {
    global cwAuxProcList

    if {[lsearch $cwAuxProcList $auxproc] == -1} {
	lappend cwAuxProcList $auxproc
    }
}


##############################################################################
#
# taskAttach - present task attachment dialog.
#
# SYNOPSIS:
#  taskAttach
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc taskAttach {} {
    global gdbReady

    if {! $gdbReady} {
	noticePost error "Cannot attach to task:\nGDB is busy."
	return
    }
    
    if [catch "cwDownTcl activeTaskNameMap" tMap] {
	noticePost error "Unable to retrieve list of active tasks.\nMake sure debugger is connected to a VxWorks target.\n\n[string trim $tMap \n]"
	
	return 
    }

    set tLen [llength $tMap]
    set tList ""

    for {set ix 0} {$ix < $tLen} {incr ix 2} {
	set tName [lindex $tMap [expr $ix + 1]]
	if {$tName != "tWdbTask" && $tName != "tNetTask"} {
	    lappend tList [format "%12.12s (0x%08x)" $tName \
			       [lindex $tMap [expr $ix]]]
	}
    }

    dialogSetValue "Attach" "Task" $tList
    dialogPost "Attach"
}

##############################################################################
#
# systemAttach - attach the debugger to the target system context
#
# SYNOPSIS:
#  systemAttach
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc systemAttach {} {
    global gdbReady

    if {! $gdbReady} {
	noticePost error "Cannot attach to system:\nGDB is busy."
	return
    }
    ttySend "attach system\n"    
}

##############################################################################
#
# taskAttachCb - callback for task attachment dialog.
#
# SYNOPSIS:
#  taskAttachCb
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc taskAttachCb {} {
    if [catch {dialogGetValue "Attach" "Task"} item] {
	noticePost error "Unable to attach."
	return 
    }

    if {$item == ""} {
	noticePost error "Select a task to attach to."
	return
    }

    set taskId ""
    regexp {.* \((0x[0-9a-fA-F]+)\)} $item all taskId

    if {$taskId == ""} {
	noticePost error "Invalid task."
	return
    }

    dialogUnpost "Attach"
    ttySend "attach $taskId\n"
}

##############################################################################
#
# interruptDebugger - send an interrupt signal to debugger
#
# SYNOPSIS:
#  interruptDebugger
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc interruptDebugger {} {
    global debuggerPid

    if {$debuggerPid != -1} {
	exec kill -INT $debuggerPid
    }
}

##############################################################################
#
# cwDownTcl - ask debugger to perform a Tcl evaluation, if it is not busy.
#
# SYNOPSIS:
#  cwDownTcl
#
# PARAMETER:
#  args: Tcl expression to evaluate
#
# RETURNS: N/A
#
# ERRORS: N/A

proc cwDownTcl {args} {
    global gdbReady
    global cwBusyMessage

    if {! $gdbReady} {
	error $cwBusyMessage
    } else {
	return [eval [concat downtcl $args]]
    }
}

##############################################################################
#
# targetListPost - present target connection dialog.
#
# This routine displays the target attachment dialog when the button
# "Connect Target Servers...", in the "Targets" menu is clicked.
#
# SYNOPSIS:
#  targetListPost
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc targetListPost {} {
    global gdbReady

    if {! $gdbReady} {
	noticePost error "Cannot attach to target:\nGDB is busy."
	return
    }
    
    if [catch "wtxInfoQ" result] {
	noticePost error "Unable to retrieve list of available targets."
	return 
    }

    foreach server $result {
	if {[lindex $server 1] == "tgtsvr"} {
	    set target [lindex $server 0]
	    lappend targetList $target
	}
    }

    dialogSetValue "Connect" "Target Servers" $targetList
    dialogPost "Connect"
}

##############################################################################
#
# targetConnectCb - callback for target connection dialog.
#
# This routine sends a connection command to GDB when a target server
# is selected in the target attachment dialog.
#
# SYNOPSIS:
#  targetConnectCb
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A

proc targetConnectCb {} {
    if [catch {dialogGetValue "Connect" "Target Servers"} item] {
	noticePost error "Unable to connect."
	return 
    }

    if {$item == ""} {
	noticePost error "Select a target to attach to."
	return
    }

    wsTitleSet "CrossWind : $item"
    dialogUnpost "Connect"
    ttySend "target wtx $item\n"
}

##############################################################################
#
# topLevelErrorHandler - catches Tcl errors that reach the top level
#
# Define a new error handler that treats the toplevel error "GDB is busy" 
# specially: just put up a notice, don't offer to show the Tcl error stack.
#

proc topLevelErrorHandler {err} {
    global errorInfo
    global cwBusyMessage
    
    if {$err == $cwBusyMessage} {
	noticePost error $cwBusyMessage
	return
    }

    oldErrHandler $err
}

##############################################################################
#
# hwBpDialogInit - initialize hardware breakpoints dialogs
#
# This routine initializes hardware breakpoint dialogs. It is called each
# time Crosswind is connected/reconnected to a target server.
#
# SYNOPSIS:
# hwBpDialogInit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc hwBpDialogInit {} {
    global hwBpListIsPosted
    global hwBpTypeList

    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    # get hardware breakpoint access types available on this architecture

    if [catch {cwDownTcl set hwBpTypeList} hwBpTypeList] {set hwBpTypeList ""}

    # unpost (if any) obsolete "Set Hardware Breakpoint" dialog

    dialogUnpost "Set Hardware Breakpoint..."

    # Update title and content of "Hardware Breakpoint List" dialog

    dialogTitleSet "Hardware Breakpoint List" \
			"CrossWind $tgt: Hardware breakpoint List"
    hwBpListUpdate 1
}

##############################################################################
#
# hwBpSet - set hardware breakpoint on the target
#
# This routine sets a hardware breakpoint on the target according informations
# from the "hardware breakpoint set" dialog.
#
# SYNOPSIS:
# hwBpSet
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
#    gdb errors if the symbol is not found
#    "Select a hardware breakpoint type." if no hardware breakpoint type
#	selection in the dialog
#    errors returned by wtxEventpointAdd
#

proc hwBpSet {} {
    global hwBpTypeList

    # check WTX connection

    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    # read symbol address/name from dialog

    set value [dialogGetValue "Set Hardware Breakpoint..." \
    					"Address or Symbol Name:"]

    if {"$value" == ""} {
    	noticePost error "Enter an address or a symbol name."
	return
    }

    # check if symbol specified is a name or a address

    if [regexp {^[0-9].*} $value] {
	set addr $value
    } else {
	catch {cwDownTcl gdb tclprint/x &$value} result
	set addr  [lindex [lindex [lindex $result 0] 1] 0]
	if {! [regexp {^0x.*} $addr]} {
	    # clean gdb error message (remove "{" and "}" characters).

	    set result [string trim $result "\n"]
	    regsub "^\{+" $result "" result
	    regsub "\}+$" $result "" result
	    noticePost error "Error: $result"
	    return
	}	
    }

    # read hardware breakpoint type specified in the dialog 

    set hwBpType [dialogGetValue "Set Hardware Breakpoint..." \
    						"Breakpoint Type:"]

    if {"$hwBpType" == ""} {
    	noticePost error "Select a hardware breakpoint type."
	return
    }

    # fill hardware breakpoints informations

    foreach hwBp $hwBpTypeList {
    	if {"$hwBpType" == [lindex $hwBp 2]} {
	   set access [lindex $hwBp 0]
	   break
	}
    }

    if {[string trim [cwDownTcl wtxAgentModeGet]] == "AGENT_MODE_EXTERN"} {
	set cType CONTEXT_SYSTEM
    } else {
	set cType CONTEXT_TASK
    }

    set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
		     [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]

    # add hardware breakpoint

    if {[catch {cwDownTcl wtxEventpointAdd WTX_EVENT_HW_BP $addr 0 $access \
	    $cType 0 $action 0 0 0} err]} {
	if {[regexp "AGENT_HW_BP_REGS_EXHAUSTED" $err]} {
	    noticePost error "Cannot set hardware breakpoint: all \
			    hardware breakpoint registers are in use."
	} else {
	    noticePost error "Error: $err"
	}	
	return
    }
   
    # update hardware breakpoint list dialog

    hwBpListUpdate 1

    dialogUnpost "Set Hardware Breakpoint..."
}

##############################################################################
#
# hwBpListUnpost - unpost the hardware breakpoint list dialog.
#
# This routine unposts the hardware breakpoint list dialog
#
# SYNOPSIS:
# hwBpListUnpost
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
# "No support for hardware breakpoints." if hardware breakpoints are not
#  supported on this architecture.
#

proc hwBpListUnpost {} {
    global hwBpListIsPosted

    dialogUnpost "Hardware Breakpoint List"

    set hwBpListIsPosted 0
    }

##############################################################################
#
# hwBpListPost - post the dialog to list hardware breakpoints
#
# This routine posts the dialog that lists all the hardware breakpoints
# set on the target
#
# SYNOPSIS:
# hwBpListPost
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
# "No support for hardware breakpoints." if hardware breakpoints are not
#  supported on this architecture.
#

proc hwBpListPost {} {
    global hwBpListIsPosted
    global hwBpTypeList

    # check WTX connection

    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    # first check if hardware breakpoints are supported on this
    # architecture.

    if {[string trim $hwBpTypeList] == ""} {
    	noticePost error "No support for hardware breakpoints."
	return
    }

    # post dialog and set title

    dialogPost "Hardware Breakpoint List"
    dialogTitleSet "Hardware Breakpoint List" \
			    "CrossWind $tgt: Hardware breakpoint List"

    set hwBpListIsPosted 1
    hwBpListUpdate 0
    }
    
##############################################################################
#
# hwBpDelete - delete a hardware breakpoint
#
# This routine deletes the hardware breakpoint selection in the hardware
# breakpoint list dialog.
#
# SYNOPSIS:
# hwBpDelete 
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
# "Breakpoint not set by CrossWind." if we try to delete a breakpoint
#	not set by the current session.
#

proc hwBpDelete {} {
    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    set value [dialogGetValue "Hardware Breakpoint List" "Hardware Breakpoints"]
    if {"$value" == ""} {return}

    # Check if breakpoint has been set by Crosswind (indicated by *)

    if {![regexp { \*$} $value]} {
    	noticePost error "Breakpoint not set by current CrossWind session."
	return
    }

    # delete the breakpoint

    if [catch {cwDownTcl wtxEventpointDelete [lindex $value 1]} error] {
    	noticePost error "Error: $error"
	hwBpListUnpost
	return
    }

    # Update hardware breakpoint list dialog

    hwBpListUpdate 1
}

##############################################################################
#
# hwBpSetPost - post the dialog to set a hardware breakpoint
#
# This routine posts the dialog that allows the user to set a hardware
# breakpoint.
#
# SYNOPSIS:
# hwBpSetPost
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc hwBpSetPost {} {
    global hwBpTypeList

    # check WTX connection

    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    # first check if hardware breakpoints are supported on this
    # architecture.

    if {[string trim $hwBpTypeList] == "" } {
    	noticePost error "No support for hardware breakpoints."
	return
    }

    set hwBpList ""
    foreach hwBp $hwBpTypeList {
    	lappend hwBpList [lindex $hwBp 2]
    }

    # post the dialog

    dialogSetValue "Set Hardware Breakpoint..." \
    				"Address or Symbol Name:" [selection]
    dialogSetValue "Set Hardware Breakpoint..." "Breakpoint Type:" $hwBpList
    dialogPost "Set Hardware Breakpoint..."
}

##############################################################################
#
# hwBpListUpdate - update the hardware breakpoint list
#
# This routine updates the hardware breakpoint list in the crosswind's 
# hardware breakpoint list panel.
# It registred itself in the update mechanism so that it is called at fixed
# intervals.
#
# SYNOPSIS:
# hwBpListUpdate once
#
# PARAMETERS:
# once: 0 if the routine must registred itself in the update mechanism, 1 else.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc hwBpListUpdate {once} {
    global hwBpListIsPosted
    global hwBpPollInterval
    global hwBpTypeList

    # check if hardware breakpoint list dialog is posted

    if {$hwBpListIsPosted == 0} {return}

    set hwBpList ""

    # check WTX connecction

    if [catch {cwDownTcl wtxTargetName} tgt] {return}

    if {[string trim $hwBpTypeList] == "" } {return}

    # read gdb tool ID (used later)

    set gdbToolId [cwDownTcl wtxToolIdGet]

    # save the selection to restore it later

    set selectedValue [dialogGetValue "Hardware Breakpoint List" \
				"Hardware Breakpoints"]

    if [catch {cwDownTcl wtxEventpointListGet} bpList] {
        noticePost error "Error updating hardware breakpoint list: $bpList"
	hwBpListUnpost
    	return
    }

    foreach bpt $bpList {
	if {[lindex [lindex $bpt 0] 0] == "WTX_EVENT_HW_BP"} {

	    set bpAddr [lindex [lindex $bpt 0] 1]
	    catch {cwDownTcl gdb print/a $bpAddr} result
	    set bpAddr [lrange $result 2 end]	

	    set bpTask [lindex [lindex $bpt 1] 1]
	    if {$bpTask == 0} {
		set bpTask "all"
	    }
	    if {[lindex [lindex $bpt 1] 0] == "CONTEXT_SYSTEM"} {
		set bpTask "SYSTEM"
	    } else {
		if {[lindex [lindex $bpt 1] 0] == "CONTEXT_ANY_TASK"} {
		    set bpTask "all"
		}
	   }

	    set access [lindex [lindex $bpt 0] 3]
	    set bpTypeName "(hard-unknown)" 

	    foreach hwBp $hwBpTypeList {
		if {[lindex $hwBp 0] == $access} {
		    set bpTypeName [lindex $hwBp 1]
		    break
		}
	    }
	    set hwBpNum [lindex $bpt 4]

	    set string "[format "Num: %-3d  Type: %-20s  Task: %-8s  Addr: %s" \
	    				$hwBpNum $bpTypeName $bpTask $bpAddr]"

	    # Check if this hardware breakpoint has been set by the current
	    # tool. If yes, add a "*" and the end of the breakpoint descripion

	    if {$gdbToolId == [lindex $bpt 3]} {
		set string "$string *"
	    }

	    lappend hwBpList $string
	}
    }

    # refresh information window

    dialogSetValue "Hardware Breakpoint List" "Hardware Breakpoints" $hwBpList

    # restore user's selection

    dialogListSelect "Hardware Breakpoint List" \
    				"Hardware Breakpoints" $selectedValue

    if {!$once && $hwBpPollInterval > 0} {
	delayedEval $hwBpPollInterval "hwBpListUpdate 0"
    }
}

################################################################################
#
# Crosswind.tcl - Initialization
#
# This part of the file does all the necessary initialization and sourcing to
# get a runable CrossWind application.
# The user's resource file, if any, is also sourced here.
#

# Create all the buttons and menus

createForms

# Set up source display mode.

if {! [info exists srcDisplayMode]} {
    set srcDisplayMode "0"
    set __srcDisplayMode "0"
}

# Initialize backtrace window.

hierarchyCreate Backtrace "backtraceCallback"
hierarchySetStructure Backtrace {{"Call Stack" +}}

# Setting dummy values makes sure it appears on the screen with a little
# vertical space allocated.

hierarchySetValues Backtrace {{" " " " " " " " " "}}

# Initialize target display.

debugStatusSet "No Target"

# Load host customization.

source [wtxPath host resource tcl app-config all]host.tcl

# wtxcore.tcl needed for Hitachi SH special case

source [wtxPath host resource tcl]wtxcore.tcl

# Override the top level error handler provided by host.tcl.  

if {[info proc oldErrHandler] == ""} {
    rename topLevelErrorHandler oldErrHandler
}

# Load app customization.

set cfgdir [wtxPath host resource tcl app-config CrossWind]*.tcl
foreach file [lsort [glob -nocomplain $cfgdir]] {source $file}

# Read in the user's home-directory Tcl initialization file.

if [file exists $initFileName] {
    source $initFileName
}
