# app-config/Tornado/01ExtCmdInterface.win32.tcl - 
#				provides an interface to external commands.
#
# Copyright (C) 1995-97 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01f,01jul02,rbl  fixing 79350 - some simple Tools->Customize commands
#                  don't work
# 01e,10mar99,jak  remove backslashes before macro substitution
# 01d,25feb99,l_k fixed SPR 24821
# 01c,13jan99,jak added project macros
# 01b,09oct98,jak moved macro handling here 
# 01a,09sep98,jak created
#

# DESCRIPTION
#   This script contains the routines run external commands. This is a win32-
# specific script. 
#

#
# ExtCmdInterface:: runs external commands as tools or builds
#
namespace eval ExtCmdInterface {


	#
	# runCmd 	<tool or build> <cmd line> [<working dir> <window title> 
	# 			<use output window> <close on exit> <prompt for args>]
	# 
	# For builds, the command is make.exe, and output goes to the 
	# build output window.
	#		 	
	proc runCmd { args } {

		if {[llength $args] < 2} {
			error "ExtCmdInterface::runCmd: wrong # args"
			return
		}
        #puts "ExtCmdInterface::runCmd $args\n"

   		set type [lindex $args 0]
		set lParams [lreplace $args 0 0]

		# default values
		set arrOpts(0)  ""		;# cmd line
		set arrOpts(1)  ""  	;# working dir
		set arrOpts(2)  "" 		;# window title
		foreach i { 3 4 5 } {	;# output window, close, prompt
			set arrOpts($i) 0	
		}

		# load with params
		set i 0
		foreach param $lParams {
			set arrOpts($i) $param
			incr i
		}

		# patch workingDir as required: first convert forward slashes 
                # to backslashes ...
                
                regsub -all {/} $arrOpts(1) {\\} arrOpts(1) 

                # ... and now make any trailing backslash into a 
                # double backslash. This is because the working dir is 
                # eventually (in CConsoleAppDocument::Create) passed
                # to runcmd.exe in quotes, and a single backslash will 
                # escape the quote. See MSDN on
                # "Parsing C Command-Line Arguments". 
                # This is somewhat of a hack: probably a better place
                # to fix this would be in CConsoleAppDocument::Create, where
                # runcmd.exe is invoked. 

                regsub {\\$} $arrOpts(1) {\\\\} arrOpts(1) 

   		switch  $type {
       	  tool {
    		# toolLaunch requires " to be passed literally as \"
    		regsub -all "\"" $arrOpts(0) "\\\"" arrOpts(0) 

        	set arrOpts(1) [string trim $arrOpts(1)]

		puts stderr "Command: $arrOpts(0)"

            # launch external process with these options:
            # title 
	    # comdLine workingdir
            # capture stdin, capture stdout, capture stderr,
            # prompt for args, close window on exit,
            # run as mdi child
            if {[catch {toolLaunch $arrOpts(2) \
                       $arrOpts(0) $arrOpts(1) \
                       0 $arrOpts(3) $arrOpts(3) \
                       $arrOpts(5) $arrOpts(4) \
                       $arrOpts(3) } err]} {
                   error $err
            }
          }
          build {
            # launch an external process with output to Build output window
            set cmdLine [wtxPath host x86-win32]bin/make.exe
            
            # next 2 lines are SPR 24821 fix:
            regsub -all {/} $cmdLine {\\} cmdPath 
            set cmdLine $cmdPath

            append cmdLine " $arrOpts(0)"

            #puts "ExtCmdInterface::runCmd buildLaunch $arrOpts(2) $cmdLine $arrOpts(1)\n"

            if {[catch {buildLaunch $arrOpts(2) \
						$cmdLine $arrOpts(1)} err]} {
                   error $err
            }
          }
		  default {
			error "ExtCmdInterface::runCmd: first param should be tool | build"
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
								{line "Line number"} \
								{column "Column number"} \
								{textsel "Selected text"} \
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
		# double backslash because of regsub behaviour
		regsub -all {\\} $with {\\\\} with
    	if {[regsub -all $lmacro $line "$with" line]} { 
								# (if macro was used)
            if {[string trim $with]==""} { ;# but had no value
				set macro [lindex $lmacro 0]
                puts stderr \
					"warning: macro $macro has no value in current context"
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
        		set lineNo ""
        		set colNo ""
        		set selText ""

				# Note: all path macros will be substituted to get backslashes
				# for ext. use, but will be used with fwd slashes for tcl 
        		catch {
            		set filePath [currentFileGet]
					set filePath [string trim $filePath]
					if {$filePath == ""} { ;# try the workspace
        				set handle [::Workspace::selectionGet]
        				set objectType [::Object::evaluate $handle Type]
						set fileTypes \
					{ projectObject sourceFileObject dependObject derivedObject }
						if {[lsearch -exact $fileTypes $objectType] >= 0 } {
            				set filePath [::Object::evaluate $handle Name]
						}
					}
            		regsub -all {\\} $filePath {/} fPath ;# for int. tcl
					regsub -all {/} $filePath {\\} filePath ;# for ext. use
            		set fileDir [file dirname $fPath]
					regsub -all {/} $fileDir {\\} fileDir ;# for ext. use
            		set fileName [file tail $fPath]
            		set baseName [file rootname $fileName]
            		set lineNo [currentFileLineGet]
            		set colNo [currentFileColumnGet]
            		set selText [currentFileTextSelectionGet]
        		} err

        		set line [substMacroWith {\$filepath} $line $filePath]
        		set line [substMacroWith {\$filedir} $line $fileDir]
        		set line [substMacroWith {\$filename} $line $fileName]
        		set line [substMacroWith {\$basename} $line $baseName]
        		set line [substMacroWith {\$line} $line $lineNo]
        		set line [substMacroWith {\$column} $line $colNo]
        		set line [substMacroWith {\$textsel} $line $selText]
			}

			project {
				set projDir ""
				set buildDir ""
				set derivedObj ""

				catch {
        			set projDir [::Workspace::projectDirGet]
					regsub -all {/} $projDir {\\} projDir;# for ext. use
        			set buildDir [::Workspace::projectBuildDirGet]
					regsub -all {/} $buildDir {\\} buildDir;# for ext. use
        			set derivedObj [::Workspace::fileDerivedObjectGet]
				} err

        		set line [substMacroWith {\$projdir} $line $projDir]
        		set line [substMacroWith {\$builddir} $line $buildDir]
        		set line [substMacroWith {\$derivedobj} $line $derivedObj]
			}

			target {
        		set targetName ""
        		set target ""

        		catch { set targetName [currentTargetGet] } err
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

        		set wind_base [WIND_Env::WIND_BASE_get]
        		set wind_host_type [wtxHostType]
            	set wind_registry [WIND_Env::WIND_REGISTRY_get] 

        		set line [substMacroWith {\$wind_base} $line $wind_base]
        		set line [substMacroWith {\$wind_host_type} $line \
																$wind_host_type]
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
