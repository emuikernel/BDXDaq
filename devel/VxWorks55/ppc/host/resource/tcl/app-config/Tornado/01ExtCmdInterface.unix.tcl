# app-config/Tornado/01ExtCmdInterface.unix.tcl - 
#				provides an interface to external commands.
#
# Copyright (C) 1995-97 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01d,13jan99,jak  added project macros
# 01c,30nov98,jak fixed - option
# 01b,09oct98,jak moved macro handling here
# 01a,09sep98,jak created
#

# DESCRIPTION
#   This script contains the routines run external commands. This is a UNIX-
# specific script 
#

#
# ExtCmdPromptUI:: - dlg to prompt for arguments
#
namespace eval ExtCmdPromptUI {

	variable executeOk 
	variable strCmdLine 

	proc promptForArgs { strCmd } {
		variable executeOk 

		set executeOk IDCANCEL 
		variable strCmdLine ""
    		dialogCreate \
        		-name eciIDD_PROMPT \
        		-title "Execute command" \
        		-width 263 -height 47 \
			-init { \
				controlCreate eciIDD_PROMPT [list \
             		 		text -name IDC_EDIT1  \
            				-border -xpos 55 -ypos 7 \
					-width 195 -height 14  \
				]
				controlTextSet eciIDD_PROMPT.IDC_EDIT1 $strCmd
			} \
        		-controls { \
             		{ button -title "Execute" -name IDOK -default \
                        -xpos 172 -ypos 26 -width 40 -height 14  \
            		-callback ExtCmdPromptUI::onPromptOk \
             		}
             		{ button -title "Cancel" -name IDCANCEL  \
            		-border \
            		-xpos 221 -ypos 26 -width 35 -height 14  \
            		-callback ExtCmdPromptUI::onPromptCancel \
             		}
             		{ label -title "Command line:" -name IDC_STATIC  \
                      	-xpos 7 -ypos 8 -width 50 -height 14  \
             		}
			}
		return $executeOk
	}

	proc onPromptOk {} {
		variable strCmdLine
		variable executeOk 

		set strCmdLine [controlTextGet eciIDD_PROMPT.IDC_EDIT1]
		set strCmdLine [string trim $strCmdLine]
		if {$strCmdLine == ""} {
			messageBox "A command is required"
			return
		}
		set executeOk IDOK 
		windowClose eciIDD_PROMPT
	}

	proc onPromptCancel {} {
		variable executeOk 

		set executeOk IDCANCEL 
		windowClose eciIDD_PROMPT
	}

	proc getCmdLine {} {
		variable strCmdLine

		return $strCmdLine
	}

}

#
# ExtCmdInterface:: - UNIX implementation to launch external commands
#
namespace eval ExtCmdInterface {


	variable arrOpts

	proc resetOpts {} {
		variable arrOpts

		# default values
		set arrOpts(0)  "command" 	;# command
		set arrOpts(1)  "" 		;# working dir	
		set arrOpts(2)  "" 		;# window title
		foreach i { 3 4 5 } {		;# output, close, prompt 
			set arrOpts($i) 0	
		}
		set arrOpts(abort) 0
	}

	#
	# runCmd 	<"tool" | "build"> <cmd line str> 
	#		[<working dir str> <window title str> 
	# 	 	<use output window 1|0> <close on exit 1|0> <prompt for args 1|0>]
	# DESCRIPTION
	#		 	
	# builds - an xterm is launched with the title specified. make is the command used.
	#	The directory is changed, and the command is echoed on the xterm and then 
	#	executed. After execution, the xterm waits for Return.
	#
	# tools - "use output window" means display the results in a new xterm. 
	#	An xterm is launched with the title specified. The command is echoed
	#	and then executed. If "close on exit" is not specified, the xterm 
	#	waits for Return. 
	#	If "prompt for args" is specified, a dialog prompts for args 
	#	with the cmd before the command is executed.	
	#
	proc runCmd { args } {
		variable arrOpts

		if {[llength $args] < 2} {
			error "ExtCmdInterface::runCmd: wrong # args"
			return
		}

   		set type [lindex $args 0]
		set lParams [lreplace $args 0 0]

		resetOpts

		# load with params
		set i 0
		foreach param $lParams {
			set arrOpts($i) $param
			incr i
		}

   		switch  $type {
       	  		tool {
				if {$arrOpts(5)} { ;# prompt for args
				    if {[ExtCmdPromptUI::promptForArgs $arrOpts(0)] == \
							"IDCANCEL"} {
						return
				    } else {
					set arrOpts(0) [ExtCmdPromptUI::getCmdLine]
				    }
				}
				if { [catch {
					set workingDir [string trim $arrOpts(1)]
					set currentDir ""
					if {$workingDir != ""} {
						set currentDir [pwd]
						cd $workingDir
					}
				} err ]} {
					error $err
				}	
				if {!$arrOpts(3)} { ;# no new xterm reqrd - execute as a background process
					set cmd ""
					set opts ""
					set listCmdLine [split $arrOpts(0)]
					set cmd [lindex $listCmdLine 0]
					set len [llength $listCmdLine]
					if {$len > 1} {
						set opts [lreplace $listCmdLine 0 0]
					}
					if { [catch {
						# exec -- <cmd> <arg1> <arg2> <arg3>...<argn> <background>
						# here the line is assembled, and evaled, so that the args are spilt
						set execLine "exec -- $cmd $opts &"
						eval $execLine
						if {$currentDir != ""} {
							cd $currentDir
						}
					} err ]} {
						error $err
					} 
					return
				}
				if { [catch {
					if {$arrOpts(4)} { ;# close on exit
						set cmdLine "echo $arrOpts(0)\;$arrOpts(0)"
					} else { 
						set cmdLine "echo $arrOpts(0)\;$arrOpts(0)\;echo press Return to exit\;read aKey"
					}
					# runTerminal <title> <asBackground> <shellCmd> <shellOpt> <cmd1;cmd2;...cmdN>
					runTerminal $arrOpts(2) 1 /bin/sh -c "$cmdLine"
					if {$currentDir != ""} {
						cd $currentDir
					}
				} err ]} {
					error $err
				}
            		}
          		build {
				if { [catch {
					set workingDir [string trim $arrOpts(1)]
					set currentDir ""
					set buildCmd [wtxPath host [wtxHostType]]bin/make
					append buildCmd $arrOpts(0)
					set cmdLine "echo $buildCmd\;$buildCmd\;echo press Return to exit\;read aKey"
					if {$workingDir != ""} {
						set currentDir [pwd]
						cd $workingDir
					}
					# runTerminal <title> <asBackground> <shellCmd> <shellOpt> <cmd1;cmd2;...cmdN>
					runTerminal $arrOpts(2) 1 /bin/sh -c "$cmdLine"
					if {$currentDir != ""} {
						cd $currentDir
					}
				} err ]} {
					error $err
				}
          		}
			default {
				error "first arg must be tool or build"
			}
		}
	}
	

}

#
# MacroHandler:: handles macro substitution 
#
namespace eval MacroHandler {

	# types of macros available:
	variable listMacroTypes
	set listMacroTypes { \
							file \
							project \
							target \
							tornado \
						}


	# macros available and their names:
	variable listMacros(file) 
	set listMacros(file) { \
								{filepath "File path"} \
								{filedir "Dir name"} \
								{filename "File name"} \
								{basename "Base name"} \
							} 
    	variable listMacros(project)
	set listMacros(project) { \
					{projdir "Project dir"} \
					{builddir "Project build dir"} \
					{derivedobj "Derived object"} \
					}
	variable listMacros(target)
	set listMacros(target) { \
								{targetName "Target name"} \
								{target "Target"} \
							}
	variable listMacros(tornado)
	set listMacros(tornado)	{ \
								{wind_base "Tornado inst. dir"} \
								{wind_host_type "Tornado host type"} \
								{wind_registry "Tornado registry"} \
							}

	# getMacroTypeList - returns a list of all types of macros
	# 				(context is ignored for now)
	proc getMacroTypeList { context } {
		variable listMacroTypes

		return $listMacroTypes
	}

	# getMacroList - returns a list of all of macros and their names 
	proc getMacroList {macroType} {
		variable listMacros

		if {[info exists listMacros($macroType)]} {
			return $listMacros($macroType)
		} else {
			return {}
		}
	}


    # Aux proc. to substitute a macro and generate a warning
    # if the macro had no value. lmacro must be the macro name as a list
    proc substMacroWith {lmacro line with} {
    	if {[regsub -all $lmacro $line "$with" line]} { 
					# (if macro was used)
            if {[string trim $with]==""} { ;# but had no value
				set macro [lindex $lmacro 0]
                puts stderr "warning: macro $macro has no value in current context"
            }
    	}
        return $line
    }

    # substMacros - returns line with all macros for context substituted
    # 				(context is ignored for now)
    proc substMacros {context type line} {
        global env

	switch $type {

		file {
        		set filePath ""
        		set fileDir ""
        		set fileName ""
        		set baseName ""

        		catch {
        			set handle [::Workspace::selectionGet]
        			set objectType [::Object::evaluate $handle Type]
				set fileTypes \
					{ projectObject sourceFileObject dependObject derivedObject }
				if {[lsearch -exact $fileTypes $objectType] >= 0 } {
            				set filePath [::Object::evaluate $handle Name]
				}
            			set fPath $filePath
            			set fileDir [file dirname $fPath]
            			set fileName [file tail $fPath]
            			set baseName [file rootname $fileName]
        		} err

        		set line [substMacroWith {\$filepath} $line $filePath]
        		set line [substMacroWith {\$filedir} $line $fileDir]
        		set line [substMacroWith {\$filename} $line $fileName]
        		set line [substMacroWith {\$basename} $line $baseName]
		}

            	project {
			set projDir ""
			set buildDir ""
			set derivedObj ""

			catch {
				set projDir [::Workspace::projectDirGet]
				set buildDir [::Workspace::projectBuildDirGet]
				set derivedObj [::Workspace::fileDerivedObjectGet]
			} err

			set line [substMacroWith {\$projdir} $line $projDir]
			set line [substMacroWith {\$builddir} $line $buildDir]
			set line [substMacroWith {\$derivedobj} $line $derivedObj]
		}

		target {
        		set targetName ""
        		set target ""

        		catch { set targetName [SelectedTarget::targetGet] } err
        		set iAt [string first @ $targetName]
        		if {$iAt > 0} {
            		incr iAt -1
            		set target [string range $targetName 0 $iAt]
        		}
        		# important: don't switch the order of targetName and target -
        		#               will result in incorrect subst.

        		set line [substMacroWith {\$targetName} $line $targetName]
        		set line [substMacroWith {\$target} $line $target]
		}

		tornado {
        		set wind_base ""
        		set wind_host_type ""
        		set wind_registry ""

        		set wind_base [wtxPath]
        		#remove trailing slash:
        		set upto [string length $wind_base]
        		incr upto -2
        		set wind_base [string range $wind_base 0 $upto]
        		set wind_host_type [wtxHostType]
        		if {[info exists env(WIND_REGISTRY)]} {
            		set wind_registry $env(WIND_REGISTRY)
        		} else {
            		set wind_registry ""
        		}

        		set line [substMacroWith {\$wind_base} $line $wind_base]
        		set line [substMacroWith {\$wind_host_type} $line $wind_host_type]
        		set line [substMacroWith {\$wind_registry} $line $wind_registry]
		}

	}

        return $line
    }
	
    # substAllMacros - returns line with all available macros substituted
    # 				(context is ignored for now)
    proc substAllMacros {context line} {
	foreach macroType [getMacroTypeList $context] {
		set line [substMacros $context $macroType $line]
	}
        return $line
    }

}
