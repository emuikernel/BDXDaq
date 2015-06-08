# shellUtilCmd.tcl - Tcl implementation of WindSh "util" commands
#
# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01z,06mar03,lcs  Update printLogo copyright year to 2003.
# 01y,15jan03,lcs  Integrate SPR#26657 this will fix SPR#76840 replaces 
#		   [gets stdin] by [stdinGet].
# 01x,08may02,fmk  SPR 75314 - CTRL-W does not call help on a blank windsh
#                  command line - fix by changing exec windHelp to exec
#                  windman
# 01w,05dec01,tam  used obj  dir name to build path
# 01v,13nov01,fmk  correct path to repeatHost.o and periodHost.o
# 01u,17oct01,gls  update printLogo copyright year to 2002
# 01t,27feb01,hk   update printLogo copyright year to cover 2001.
# 01s,16mar99,ren  Changing copyright notice to 1999
# 01r,02mar99,jmp  set shConfig(LD_COMMON_MATCH_ALL) default value to on (SPR#
#                  23385).
# 01q,13jan99,jmp  added procListGet() for shellprocs and wtxcore.tcl
#                  wtxerrdb.tcl commands completion (SPR# 24202). added
#                  control for repeat entry argument (SPR# 24114).
# 01p,15dec98,fle  added the DSM_HEX_MODE variable to shellConfigList
# 01o,03dec98,jmp  added shConfig LD_COMMON_MATCH_ALL (Workaround for
#                  SPR#23385).
# 01n,18nov98,jmp  added message to indicate when Tornado Help is invoked
#                  (SPR# 23337).
# 01m,03nov98,jmp  modified to source windHelpLib on first CTRL-W.
# 01l,03nov98,j_k  fixed winMachType and ls to use existing and reliable tcl 
#                    functionality rather than a kludge hack.
# 01k,16oct98,dbt  fixed spaces in help() routines.
# 01j,15oct98,dbt  added sysSuspend() and sysResume() documentation in help()
#                  routine.
# 01i,09oct98,jmp  added windshHelp.
#		   modified string allocation method in WindSh.
# 01h,24aug98,jmp  added w & tw commands to help().
# 01g,21aug98,jmp  removed commented code about shellConfig(SH_NAVIGATOR).
# 01f,24jun98,jmp  fixed minor bug introduced by last change.
# 01e,03jun98,jmp  fixed shConfig{} return values.
# 01d,29may98,p_m  set Copyright date to 1998
# 01c,12may98,jmp  speed up WindSh reboot()
# 01b,11may98,jmp  fixed SPR# 21040: rework bootChange so works if vec table
#                  protected.
# 01a,01apr98,jmp  created based on routines moved out from shell.tcl
#

# DESCRIPTION
# This module holds the "util" shellproc of WindSh:
#
# bootChange - prompts for boot line parameters
# cd - change Tcl's working directory.
# h - shell history function
# help - print a synopsis of selected routines
# ls - list files in shell's current working directory.
# period - spawn a task to call a function periodically
# printErrno - print the definition of a specified error status value
# printLogo - print the logo.
# pwd - print working directory (as it is known to the I/O system).
# quit - exit windsh
# reboot - reset the target.
# repeat - spawn a task to call a function repeatedly
# shConfig - WindSh environment configuration tool
# shellHistory - shell history function
# shellPromptSet - set the shell prompt
#   
# This module also contains windsh private routines used by the
# "util" shellprocs
#


#
# set architecture help message if it doesn't exist.
#
if {[info var archHelpMsg] == ""} {
    set archHelpMsg ""
}

##############################################################################
# bootChange - prompts for boot line parameters
#
# SYNOPSIS:
# bootChange
#
# RETURNS:
# NONE
#

shellproc bootChange {args} {
    global __wtxTsInfo

    bindNamesToList {bootLineAddr bootString} [wtxGopherEval \
                     "[shSymAddr sysBootLine] *!$"]

    set newBootString [bootParamsPrompt $bootString]
    if {$newBootString == -1} {
        return -1
    } else {

        # something changed so write the new bootline

        set writeProtected 0
        set pageSize [lindex [lindex $__wtxTsInfo 2] 3]

	# unprotect bootline address space if needed

	if {[catch {set stateGet \
		[lindex [wtxSymFind -name vmStateGet] 1]}] == 0 &&
	    [catch {set stateSet \
		[lindex [wtxSymFind -name vmStateSet] 1]}] == 0} {

	    # vmStateGet and vmStateSet exist so can read and
	    # change protection state of bootline

	    set pageAddr [expr $bootLineAddr / $pageSize * $pageSize]
	    set stateAddr [shFuncCall -nobreak -int [shSymAddr malloc]  4]
	    shFuncCall -nobreak $stateGet 0 $pageAddr $stateAddr
	    if {([wtxGopherEval "$stateAddr <@>"] & 0x0c) == 0x0} {
		set writeProtected 1
		shFuncCall -nobreak $stateSet 0 $pageAddr $pageSize 0x0c 0x04
	    }
	    shFuncCall -nobreak [shSymAddr free] $stateAddr 
	}

	#
	# write the new boot line.
	# To be taken into account the changes must be written by sysNvRamSet()
	# on the target. If this function is not a simple write then we must
	# also update target server's cache to see the changes before
	# rebooting.
	#

        set blk [memBlockCreate -string $newBootString]
	set bootLineLgth [expr [string length $newBootString] + 1]
	set tmpBootLine [wtxMemAlloc $bootLineLgth]
	wtxMemWrite $blk $tmpBootLine
        wtxMemWrite $blk $bootLineAddr

	wtxFuncCall [shSymAddr sysNvRamSet] $tmpBootLine $bootLineLgth

        wtxMemFree $tmpBootLine
	memBlockDelete $blk

        if {$writeProtected} {

           # restore original state if it was protected

           shFuncCall -nobreak $stateSet 0 $pageAddr $pageSize 0x0c 0x0
        }
        return 0
    }
}

##############################################################################
#
# cd - change Tcl's working directory.
#

shellproc cd {args} {
    set dirAddr [lindex $args 0]

    if {$dirAddr != 0} {
	set wd [stringGet $dirAddr]
	cd $wd
	return 0
    }
    puts stdout "usage: cd \"directory\""
    return -1
}

##############################################################################
#
# h - shell history function
#
# SYNOPSIS:
#   h [size]
#
# PARAMETERS:
#   size - new size of shell history
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc h {args} {
    shHistory [lindex $args 0]
    return 0
}

##############################################################################
#
# help - print a synopsis of selected routines
#
# SYNOPSIS:
# help
#
# RETURNS:
# NONE
#

shellproc help {args} {
    global archHelpMsg

    #page 1

    puts stdout "help                        Print this list"
    puts stdout "h         \[n]               Print (or set) shell history"
    puts stdout "i         \[task]            Summary of tasks' TCBs"
    puts stdout "ti        task              Complete info on TCB for task"
    puts stdout "sp        adr,args...       Spawn a task, pri=100, opt=0,\
                  stk=20000"
    puts stdout "sps       adr,args...       Spawn a task, pri=100, opt=0,\
                  stk=20000"
    puts stdout "                            and leave it suspended"
    puts stdout "td        task              Delete a task"
    puts stdout "ts        task              Suspend a task"
    puts stdout "tr        task              Resume a task"
    puts stdout "tw        task              Print info about the object\
    		a task is pending on"
    puts stdout "w                           Summary of tasks' pending\
    		information"
    puts stdout "d         \[adr\[,nunits\[,width]]] Display memory"
    puts stdout "m         adr\[,width]       Modify memory"
    puts stdout "mRegs     \[reg\[,task]]      Modify a task's registers\
                  interactively"
    puts stdout "version                     Print VxWorks version info,\
                  and boot line"

    puts stdout "\nType <CR> to continue, Q<CR> to stop: "
    if {[stdinGet] == "Q"} {
        return 0
    }

    # page 2

    puts stdout "b                               Display breakpoints"
    puts stdout "b         addr\[,task\[,count]]   Set breakpoint"
    puts stdout "bd        addr\[,task]           Delete breakpoint"
    puts stdout "bdall     \[task]                Delete all breakpoints"
    puts stdout "c         \[task\[,addr\[,addr1]]] Continue from breakpoint"
    puts stdout "s         \[task\[,addr\[,addr1]]] Single step"
    puts stdout "l         \[adr\[,nInst]]         List disassembled memory"
    puts stdout "tt        \[task]                Do stack trace on task"

    # print architecture specific help message.

    puts -nonewline stdout $archHelpMsg

    puts stdout "sysSuspend                      Suspend the system"
    puts stdout "sysResume                       Resume the system"

    puts stdout "\nType <CR> to continue, Q<CR> to stop: "
    if {[stdinGet] == "Q"} {
        return 0
    }

    # page 3

    puts stdout "devs                        List devices"
    puts stdout "cd        \"path\"            Set current working path"
    puts stdout "pwd                         Print working path"
    puts stdout "ls        \[\"path\"\[,long]]   List contents of directory"
    puts stdout "ld        \[syms\[,noAbort]\[,\"name\"]] Load stdin,\
                 or file, into memory"
    puts stdout "                                    (syms = add symbols to\
                 table:"
    puts stdout "                                    -1 = none, 0 = globals,\
                 1 = all)"
    puts stdout "lkup      \[\"substr\"]        List symbols in system\
                  symbol table"
    puts stdout "lkAddr    address           List symbol table entries\
                  near address"
    puts stdout "printErrno  value           Print the name of a status\
                  value"
    puts stdout "period    secs,adr,args...  Spawn task to call function\
                  periodically"
    puts stdout "repeat    n,adr,args...     Spawn task to call function n\
                  times (0=forever)"
    puts stdout ""
    puts stdout "NOTE:  Arguments specifying 'task' can be either task ID\
                  or name."

    return 0
}

##############################################################################
#
# ls - list files in shell's current working directory.
#
# This procedure lists the contents of a directory in one or two formats.
# If the directory name is followed by '1', a long listing is issued.
#
# SYNOPSIS
#  ls dirName doLong
#
# PARAMETERS:
#  dirName: name of directory to list
#  doLong: 1 for long listing, 0 otherwise.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc ls {args} {
    global env

    set dirAddr [lindex $args 0]
    set doLong [lindex $args 1]

    if {$dirAddr != 0} {
        set wd [stringGet $dirAddr]
        if {[wtxHostType] == "x86-win32"} {
            if {$doLong} {
                puts stdout [exec $env(COMSPEC) /c dir $wd]
            } else {
                puts stdout [exec $env(COMSPEC) /c dir /w $wd]
            }
        } else {
            if {$doLong} {
                puts stdout [exec ls -l $wd]
            } else {
                puts stdout [exec ls $wd]
            }
        }
        return 0
    }

    if {[wtxHostType] == "x86-win32"} {
        if {$doLong} {
            puts stdout [exec $env(COMSPEC) /c dir]
        } else {
            puts stdout [exec $env(COMSPEC) /c dir /w]
        }
    } else {
        puts stdout [exec ls]
    }
    return 0
}

##############################################################################
#
# period - spawn a task to call a function periodically
#

shellproc period {args} {
    global __wtxCpuType

    bindNamesToList {secs entry a0 a1 a2 a3 a4 a5 a6 a7} $args

    set tool [targetTool]
    
    # check if periodHost support routine is available on Target

    if [catch "wtxSymFind -name periodHost" result] {
	if {[lindex $result 3] == "(SYMTBL_SYMBOL_NOT_FOUND)"} {

	    # XXX PAD This should be in a ressource file somewhere.
	    
            if {[string first gnu $tool] != -1} {
                set toolType gnu
            } elseif {[string first diab $tool] != -1} {
                set toolType diab
            } else {
                error "Unknown tool type $tool. Tool type must be gnu or diab.\n \
                Cannot add repeat support module to Target."
            }
            
	    bindNamesToList {cpuDir cpuFamObjDir} [wtxCpuInfoGet -n -o \
	    					   $__wtxCpuType]
	    regsub $toolType $tool common commonDir
	    set periodModPath [wtxPath target lib $cpuFamObjDir $cpuDir \
	    		       $commonDir objos]
	    set periodModule $periodModPath/periodHost.o

	    if [catch "wtxObjModuleLoad $periodModule" status] {
		error "Cannot add period support module to Target.\n $periodModule ($status)."
	    }
	} else {
	    error $result
	}
    }

    [shellName sp] [targetFunctionGet periodHost] $secs $entry \
	    $a0 $a1 $a2 $a3 $a4 $a5 $a6 $a7
}


##############################################################################
#
# printErrno - print the definition of a specified error status value
#
# This procedure prints the definition of a specified error status value.
# If the error value is null, the error number and definition set by the last
# function called from the shell is displayed.
#
# SYNOPSIS:
# printErrno errNo
#
# PARAMETERS:
# errNo error-status value
#
# RETURNS:
# 0 if errNo exists -1 otherwise
#

shellproc printErrno {args} {
    global errnoTbl
    global lastErrorValue

    set errNoV [expr [lindex $args 0]]
    if {$errNoV == 0} {
	set errNoV $lastErrorValue
    }
    set errNoH [format "0x%x" $errNoV]

    # Check if the target errno table exists. Source it if necessary.

    if ![info exists errnoTbl] {
        source [wtxPath host resource tcl]errnoTbl.tcl
    }

    if [info exists errnoTbl($errNoV)] {
	puts "$errNoH = $errnoTbl($errNoV)"
    } else {
	puts "Unknown errno = $errNoV"
	return -1
    }

    return 0
}

##############################################################################
#
# printLogo - print the logo.
#

set Logo "
      /////   /////   /////   /////   /////       |
     /////   /////   /////   /////   /////        |
    /////   /////   /////   /////   /////         |
    /////   /////   /////   /////   /////         |
   //////  //////  //////  //////  //////         |     
   //////  //////  //////  //////  //////         |     T  O  R  N  A  D  O 
    /////   /////   /////   /////   /////         |                        
    /////   /////   /////   /////   /////         |   	                   
     /////   /////   /////   /////   /////        |   	Development  System
      ////    ////    ////    ////    ////        |                        
       ////    ////    ////    ////    ////       |   	                   
        ////    ////    ////    ////    ////      |   	Host  Based   Shell
         ////    ////    ////    ////    ////     |   	                   
          ////    ////    ////    ////    ////    |   	                   
           ///     ///     ///     ///     ///    |   	Version \
							[wtxTsVersionGet]
           ///     ///     ///     ///     ///    |	
            //      //      //      //      //    |
            //      //      //      //      //    |
            //      //      //      //      //    |
           //      //      //      //      //     |

      Copyright 1995-2003 Wind River Systems, Inc."

shellproc printLogo {args} {
    global Logo
    puts stdout $Logo
    return 0
}

##############################################################################
#
# pwd - print working directory (as it is known to the I/O system).
#

shellproc pwd {args} {
    #puts stdout [wtxGopherEval "[shSymAddr ioDefPath] $"]
    puts stdout [pwd]
    return 0
}

##############################################################################
#
# quit - exit windsh
#

shellproc quit {args} {
    exit 0
}

##############################################################################
#
# reboot - reset the target.
#
# SYNOPSIS:
#   reboot
#
# The WTX protocol request `wtxTargetReset' is generated, which should 
# cause the target to reboot, and the target server to reattach.  After 
# waiting one second, we attempt to restart ourselves.
#   

shellproc reboot {args} {
    puts stdout "Rebooting..."

    if {[llength $args] == 0 || [expr [lindex $args 0]] == 0} {
	catch wtxTargetReset
	msleep 1000
	catch {set oldTimeout [wtxTimeout]}
	catch {wtxTimeout 1}
	shRestart
	catch {wtxTimeout $oldTimeout}
    } else {
	# cancel error handler, so it doesn't try to restart us
	wtxErrorHandler [wtxHandle] ""
	catch wtxTargetReset
	exit -immediate 0
    }

    return -1
}

##############################################################################
#
# repeat - spawn a task to call a function repeatedly
#

shellproc repeat {args} {
    global __wtxCpuType
    
    bindNamesToList {count entry a0 a1 a2 a3 a4 a5 a6 a7} $args
    set entry [stringGet [lindex $args 1]]

    set tool [targetTool]
    
    # check if repeatHost support routine is available on Target

    if [catch {wtxSymFind -name repeatHost} result] {
	if {[lindex $result 3] == "(SYMTBL_SYMBOL_NOT_FOUND)"} {

	    # XXX PAD This should be in a resource file somewhere.

            if {[string first gnu $tool] != -1} {
                set toolType gnu
            } elseif {[string first diab $tool] != -1} {
                set toolType diab
            } else {
                error "Unknown tool type $tool. Tool type must be gnu or diab.\n \
                Cannot add repeat support module to Target."
            }
            
            
	    bindNamesToList {cpuDir cpuFamObjDir} [wtxCpuInfoGet -n -o \
	    					   $__wtxCpuType]
	    regsub $toolType $tool common commonDir
	    set repeatModPath [wtxPath target lib $cpuFamObjDir $cpuDir \
	    		       $commonDir objos]
	    set repeatModule $repeatModPath/repeatHost.o

	    if [catch "wtxObjModuleLoad $repeatModule" status] {
		error "Cannot add repeat support module to Target.\n $repeatModule ($status)."
	    }
	} else {
	    error $result
	}
    }

    # Verify that the given function is valid
    if [catch {wtxMemRead $entry 1}] {
    	puts stdout "usage: repeat (n, func, arg1, arg2, arg3,\
	    arg4, arg5, arg6, arg7, arg8)"
	return
    }

    [shellName sp] [targetFunctionGet repeatHost] $count $entry \
	    $a0 $a1 $a2 $a3 $a4 $a5 $a6 $a7
}

##############################################################################
#
# shellHistory - shell history function
#
# SYNOPSIS:
#   h [size]
#
# PARAMETERS:
#   size - new size of shell history
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc shellHistory {args} {
    shHistory [lindex $args 0]
    return 0
}

##############################################################################
#
# shellPromptSet - set the shell prompt
#
# SYNOPSIS:
#   shellPromptSet string
#
# PARAMETERS:
#   string - address of a string to set the prompt to
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc shellPromptSet {args} {
    set addr [lindex $args 0]
    if {$addr} {
	set str [stringGet $addr]
	shPromptSet $str
    }
    return 0
}


#############################################################################
#
# Locals Procedures (Procedures used by the shellprocs defined in this file)
#

#############################################################################
#
# winMachType - identifies windows machine type
#
# This procedure determines if a PC is running NT or Windows 9x.
#
# RETURNS: String specifying Windows NT or Windows 9x.
#
# Errors: N/A
#
proc winMachType {args} {
    global tcl_platform

    return $tcl_platform(os)
}

##############################################################################
#
# bootParamsPrompt - prompt for boot line parameters
#
# SYNOPSIS:
# bootParamsPrompt string
#
# PARAMETERS:
# string: initial values
#
# RETURNS:
# -1 if nothing changed
# string with new values otherwise
#

proc bootParamsPrompt {string} {

    # initialize the parameters list and the corresponding variables

    set paramsList {"boot device\t" "processor number" "host name\t" \
                    "file name\t" "inet on ethernet (e)" \
                    "inet on backplane (b)" "host inet (h)\t" \
                    "gateway inet (g)" "user (u)\t"
                    "ftp password (pw) (blank = use rsh)"
                    "flags (f)\t" "target name (tn)" "startup script (s)" \
                    "other (o)\t"}
  
    set varList {bootDev procNum hostName fileName ethInet backInet hostInet \
                 gateInet user passwd flags tgtName startup other}
    foreach var $varList {
        set $var ""
    }

    # parse the boot line

    # using extended Tcl
    # set bootDev [ctoken string "("]
    # set string [crange $string 3 end]
    # set procNum [ctoken string ")"]
    # set string [crange $string 1 end]
    # set hostName [ctoken string ":"]
    # set string [crange $string 1 end]
    # set fileName [ctoken string " "]

    # using tcl
    set index [string first "(" $string]
    set bootDev [string range $string 0 [expr $index - 1]]
    set string [string range $string [expr $index + 3] end]
    set index [string first ")" $string]
    set procNum [string range $string 0 [expr $index - 1]]
    set string [string range $string [expr $index + 1] end]
    set index [string first ":" $string]
    set hostName [string range $string 0 [expr $index - 1]]
    set string [string range $string [expr $index + 1] end]
    set index [string first " " $string]
    set fileName [string range $string 0 \
                               [expr ($index > 0) ? ($index - 1) : {"end"}]]
    set string [string range $string [expr $index + 1] \
                             [expr ($index > 0) ? {"end"} : $index]]
    while {[string length $string] > 0} {
	# extended tcl
	# set string [crange $string 1 end]
	# set param [ctoken string "="]
	# set string [crange $string 1 end]
	set index [string first "=" $string]
        set param [string range $string 0 [expr $index - 1]]
        set string [string range $string [expr $index + 1] end]
        switch $param {
             e { set var ethInet}
             b { set var backInet}
             h { set var hostInet}
             g { set var gateInet}
             u { set var user}
             pw { set var passwd}
             f { set var flags}
             tn { set var tgtName}
             s { set var startup}
             o { set var other}
             default {break}
        }
    # set $var [ctoken string " "]
    set index [string first " " $string]
    set $var [string range $string 0 \
                           [expr ($index > 0) ? ($index - 1) : {"end"}]]
    set string [string range $string [expr $index + 1] \
                             [expr ($index > 0) ? {"end"} : $index]]
    }

    # prompt for new value

    puts stdout "\n'.' = clear field;  '-' = go to previous field;  q = quit\n"
    set sthingNew 0
    for {set i 0} {$i < [llength $paramsList]} {incr i} {
        puts -nonewline stdout "[lindex $paramsList $i]\t: \
                                [set [lindex $varList $i]] "
        set newValue [stdinGet]
        if {$newValue == "q"} {
            break
        }
        if {$newValue == ""} {
            continue
        }
        if {$newValue == "-"} {
            incr i [expr ($i >= 1) ? -2 : -1]
            continue
        }
        if {$newValue == "."} {
            set newValue ""
        }
        set [lindex $varList $i] $newValue
        set sthingNew 1
    }

    # rebuild a boot line from the new parameters
    if {$sthingNew} {
        set newString "$bootDev\(0,$procNum)$hostName:$fileName"
        for {set i 4} {$i <= 13} {incr i} {
            if {[set value [set [lindex $varList $i]]] != ""} {
                switch $i {
                    4 { append newString " e=$value"}
                    5 { append newString " b=$value"}
                    6 { append newString " h=$value"}
                    7 { append newString " g=$value"}
                    8 { append newString " u=$value"}
                    9 { append newString " pw=$value"}
                    10 { append newString " f=$value"}
                    11 { append newString " tn=$value"}
                    12 { append newString " s=$value"}
                    13 { append newString " o=$value"}
                }
            }
        }
        return $newString
    }
    return -1
}

##############################################################################
#
# shConfig - WindSh environment configuration tool
#
# SYNOPSIS:
#   shConfig <KEY_WORD> 0|1|on|off|<value>
#

proc shConfig {args} {

    global shellConfig
    global shellConfigList
    global env

    set argc [llength $args]

    case $argc in {

	0 {
	    # display WindSh environment table
	    foreach configName $shellConfigList {
		puts stdout "$configName = $shellConfig($configName)"
	    }
	}

	1 {
	    # display the given WindSh environment variable
	    set controlName [lindex $args 0]

	    if [info exist shellConfig($controlName)] {
		return "$controlName = $shellConfig($controlName)"
	    } else {
		puts stdout "$controlName : not initialized"
		shellConfigHelp
	    }
	}

	2 {
	    # set the given WindSh environment variable
	    set controlName [lindex $args 0]
	    set ctrlArg [lindex $args 1]

	    # Specific WindSh environment variable Handling

	    case $controlName in {
	    	
		{DSM_HEX_MODE \
		 LD_CALL_XTORS \
		 LD_COMMON_MATCH_ALL \
		 LD_SEND_MODULES \
		 SH_GET_TASK_IO} {

		    case $ctrlArg in {
		    	{0 off}	{set shellConfig($controlName) off}
			{1 on}	{set shellConfig($controlName) on}
			default	{
			    puts stdout \
			    "invalid value: shConfig $controlName <0|1|on|off>"
			    return
			}
		    }
		}

		LD_PATH {

		    set pathList [split $ctrlArg ";"]

		    # Check the path list
		    foreach pathName $pathList {
		    	if {![file isdirectory $pathName]} {
			    puts stdout "$pathName: No such directory."
			    return
			}
		    }

		    set shellConfig($controlName) $ctrlArg
		}

		default {
		    set shellConfig($controlName) $ctrlArg
		}
	    }

	    # add new WindSh environment variables to shellConfigList
	    if {[lsearch $shellConfigList $controlName] == -1} {
		lappend shellConfigList $controlName
	    }

	}

	default {
	    # more arguments than expected, display shConfig help
	    shellConfigHelp
	}
    }

    return
}

##############################################################################
#
# shellConfigInit - Initialize WindSh environment table with defaults
#
# SYNOPSIS:
#   shellConfigInit
#

proc shellConfigInit {} {

    global shellConfig shellConfigList
    global env

    set shellConfig(DSM_HEX_MODE) off

    # initialize LD_CALL_XTORS to target strategy

    set shellConfig(LD_CALL_XTORS)	target
    set shellConfig(LD_COMMON_MATCH_ALL) on
    set shellConfig(LD_PATH)		"."
    set shellConfig(LD_SEND_MODULES)	on
    set shellConfig(SH_GET_TASK_IO)	on

    set shellConfigList "DSM_HEX_MODE\
			 LD_CALL_XTORS\
			 LD_COMMON_MATCH_ALL\
			 LD_PATH\
			 LD_SEND_MODULES\
			 SH_GET_TASK_IO"
}

##############################################################################
#
# shellConfigHelp - WindSh environment configuration tool help
#
# SYNOPSIS:
#   shellConfigHelp
#

proc shellConfigHelp {} {

    global shellConfigList

    puts stdout "\nusage: shConfig <KEY_WORD> 0|1|on|off|<value>"
}

##############################################################################
#
# windshCmdLineCleanup - erase temporary message print by windshHelp{}
#
# SYNOPSIS:
#   windshCmdLineCleanup charnum
#
# charnum: message length
#

proc windshCmdLineCleanup {charnum} {

    for {set i 0} {$i < $charnum} {incr i} {
    	append backspace_line "\b"
	append erase_line " "
    }

    puts stdout "$backspace_line$erase_line$backspace_line" nonewline
}

##############################################################################
#
# windshHelp - Call windHelpLib stuff
#
# SYNOPSIS:
#   windshHelp [descName] [keyWord]
#

set windHelpLibInitialized 0
proc windshHelp {{descName ""} {keyWord ""}} {

    global env
    global windHelpLibInitialized

    set initMsg "Initializing Tornado Help ..."
    set dispMsg "Displaying Tornado Help ..."

    if {! $windHelpLibInitialized} {
    	# Source windHelpLib for html on-line help
	puts stdout $initMsg nonewline
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
	set windHelpLibInitialized 1

	# erase message help message...
	windshCmdLineCleanup [string length $initMsg]
    }

    if {$descName == "" || $descName == " "} {

	puts stdout $dispMsg nonewline

	if {[wtxHostType] == "x86-win32"} {
	    exec UITclSh $env(WIND_BASE)/host/resource/doctools/helpDlg.tcl &
	} else {
	    exec windman &
	}

	msleep 3000
	# erase message help message...
	windshCmdLineCleanup [string length $dispMsg]
	return
    }

    if {$keyWord == ""} {
    	# if no keyword given force to look in tornado tools first
	set keyWord "tornado|vxworks"
    }

    set helpLink [windHelpLinkGet -name $descName $keyWord]
    if {$helpLink != ""} {
    	puts stdout $dispMsg nonewline
	windHelpDisplay $helpLink
	# erase message help message...
	windshCmdLineCleanup [string length $dispMsg]
    }
}

###############################################################################
#
# procListGet - return the list of procedures that can be completed
#		using CTRL-D and/or TAB.
#		By default, all the shellprocs and wtx* commands will
#		be returned unless a pattern is given.
#
# pattern: regular expression for the procedures matching
#

proc procListGet {{pattern ""}} {

    global shellProcList
    set returnList ""

    set procList "$shellProcList shConfig [info command "wtx*"]"

    foreach procedure $procList {
    	if [regexp "$pattern" $procedure] {
	    lappend returnList $procedure
	}
    }

    return [lsort $returnList]
}
