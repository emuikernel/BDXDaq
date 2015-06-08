# Wizard.tcl - wizard helper implementation file
#
# modification history
# --------------------
# 01s,16oct01,rbl  fix bug in progressShow so that progress controls can be
#                  hidden when progress updates finish
# 01r,30mar99,rbl  Fixing SPR 26202, "Project: old "wand" bitmap flashes up
#                  when hitting the Back button to return to first wizard
#                  sheet"
# 01q,24mar99,rbl  set default wizard bitmap only for first sheet. So you only
#                  have to override it for first sheet and override will
#                  persist to next sheets...
# 01p,18feb99,jak  add -helpbutton 
# 01o,04jan99,ren  fixed SPR 23602
# 01n,24nov98,cjs  add accelerators to Wizard buttons
# 01m,12nov98,ren  adjusted layout to accomodate status bargraph
# 01l,26oct98,cjs  lowered meter to accomodate more controls
# 01k,16oct98,cjs  cleaned up error handling in onFinishButton() 
# 01j,12oct98,ren  Sometimes next button would be draw too large.  Now fixed.
# 01i,18sep98,cjs  display name of wizard and a page number in dialog title 
# 01h,18sep98,cjs  initialize location variables in one-time only init
#                  and exit routines
# 01g,14sep98,ren  Got rid of extra / chars in wizard bitmap path.
# 01f,10sep98,cjs  use controlPropertySet -defaultbutton in
#				   buttonFocusSet()
# 02e,06aug98,cjs  changed to wizardControlsEnable() to
#                  wizardControlStatesGet/Set()
# 02d,30jun98,cjs  added buttonFocusSet() 
# 02c,23jun98,cjs  centered dialog(s) 
# 02b,18jun98,cjs  added progress meter and feedback support
#                  via progressShow() 
# 02a,08apr98,cjs  substantially modified; now uses a set of
#                  structs rather than lists of lists.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# This package allows a wizard to be created by creating the
# following set of data structures, and then making a call to
# WizardRun()
#	 
#############################################################

namespace eval ::WizardCore {
	variable _Stack
	variable _FSM
	variable _SheetStructs
	variable _ReturnValue
	variable _ExitStatus
	variable _bFinishProcRan 0
	variable _abortCallback ""
	variable _initProc
	variable _exitProc
	variable _nextbuttonDims
        set buttonRow 200
        array set _nextbuttonDims [list -x 191 -y $buttonRow -w 50 -h 14 ]
	variable _controls [ list \
		[concat button -name _nextbutton -title [list "&Next >"] \
			 [array get _nextbuttonDims] \
			-defaultbutton -callback ::WizardCore::onNextButton] \
		[list button -name helpbutton -title "&Help" \
			-helpbutton \
			-x 7 -y $buttonRow -w 50 -h 14] \
		[list button -name cancelbutton -title "&Cancel" \
			-x 71 -y $buttonRow -w 50 -h 14 \
			-callback ::WizardCore::onCancelButton] \
		[list button -name _backbutton -title "< &Back" \
			-x 137 -y $buttonRow \
			-w 50 -h 14 -callback ::WizardCore::onBackButton] \
		[list button -name _finishbutton -title "&Finish" \
			-x 257 -y $buttonRow \
			-w 50 -h 14 -callback ::WizardCore::onFinishButton] \
		[list frame -name _frame -black \
			-x 7 -y [expr { $buttonRow - 6}] \
			-width 300 -height 2] \
		[list bitmap -name _bmpimage \
			-title "[wtxPath host resource \
			bitmaps Tornado]wizard.bmp" \
			-x 4 -y 4 -w 50 -h 14] \
    	[list label -name lblprogress -title "" -x 16 -y 154 \
			-w 70 -h 24] \
    	[list label -name lblprogress2 -title "" -x 16 -y 154 \
			-w 200 -h 24] \
    	[list meter -name meterprogress -x 90 -y 163 -w 150 \
			-h 22] \
	]

	proc initProc {structs idx} {
		set struct [lindex $structs $idx]
		set proc [lindex $struct 1]
		eval $proc
	}
 
	# Return value indicates whether it's safe to continue
	proc exitProc {structs idx} {
		set retval 1 
		set struct [lindex $structs $idx]
		set proc [lindex $struct 2]
		if {$proc != ""} {
			set retval [eval $proc]
		}
		return $retval
	} 

	proc sheetCtrlsGet {structs idx} {
		set struct [lindex $structs $idx]
		set sheets [lindex $struct 0]
	}

	###########################################################################
	#
	# WizardRun - Run a wizard, given the following arguments:
	# 	fsm - a list of states (lists) with the following format:
	# 		screens x buttons x sheet_states, cardinality n x 3 x n
	#		the three buttons are 0 = back, 1 = next, 2 = finish
	#		in the case of back and finish, -1 indicates the button
	#			shouldn't be enabled, any other value means it should
	#		for the next button (1), the value is the index of the next screen
	#   Each record of the sheet structure list contains the following:
	#	controlList - a list containing controls to be created
	#		(and cleaned up) for the screen.  Any bitmaps will use a
	#		shared bitmap resource is the upper left corner of the screen
	#	initProc - a routine that should be run before a screen is shown.
	#	exitProc - a routines, that should be run prior to leaving a
	#  		screen.  The routine must return non-zero to grant the user
	#		permission to move on, and zero if there is a validation error.
	#	sheetStateProc - a routine that can be called by the wizard to
	#		return an arbitrary state value which has significance to the
	#		finite state machine relative to the current screen.  This
	#		state will be used in determining when and how to advance to
	#		the next screen.
	# 	The final two arguments, initProc and exitProc, are called to do
	#   	one-time initialization and cleanup for the wizard
	#
	# Return values -- {statusCode wizardValue}
	#	statusCode is 0 for successful run, 1 for error or abort
	#	wizardValue is a wizard-specific value
	#
	proc WizardRun {fsm sheetStructs initProc exitProc} {
		variable _ReturnValue ""
		variable _ExitStatus -1 
		variable _bFinishProcRan 0
		variable _FSM $fsm
		variable _SheetStructs $sheetStructs
		variable _initProc $initProc
		variable _exitProc $exitProc
	
		dialogCreate -name wizardDialog -title "VxWorks Wizard" \
			-width 314 -height 220 \
			-init ::WizardCore::wizardInit \
			-exit ::WizardCore::onExit
			
		return [list $_ExitStatus $_ReturnValue]
	}
	
	proc returnValueSet {value} {
		global _ReturnValue
		set _ReturnValue $value
	}
	
	###########################################################################
	###########################################################################
	# Private Implementation
	###########################################################################
	###########################################################################
	
	# Create all the sheets' controls (hidden)
	proc wizardInit {} {
		variable _controls
		foreach control $_controls {
			controlCreate wizardDialog $control
		}
		controlHide wizardDialog.lblprogress 1
		controlHide wizardDialog.lblprogress2 1
		controlHide wizardDialog.meterprogress 1
		controlPropertySet wizardDialog.meterprogress \
			-background Blue \
			-foreground Black

		variable _Stack
		set _Stack -1
		showWizardPage
	}
	
	proc wizardControlStatesSet {stateList} {
		set controls {helpbutton cancelbutton _backbutton \
			_nextbutton _finishbutton}

		# When disabling, memorize the current button states
		# so that they can be restored
		set idx 0
		foreach button $controls {
			controlEnable wizardDialog.$button [lindex $stateList $idx]
			incr idx
		}
	}

	proc wizardControlStatesGet {} {
		set controls {helpbutton cancelbutton _backbutton \
			_nextbutton _finishbutton}
		foreach button $controls {
			if {[controlEnabled wizardDialog.$button]} {
				lappend retval 1
			} else {
				lappend retval 0
			}
		}
		return $retval
	}

	proc GetCtrlField {ctrlSpec field} {
		set idx [lsearch $ctrlSpec $field]
		if {$idx == -1} {
			error "Control specified as '$ctrlSpec' doesn't contain '$field'"
		} else {
			if {[llength $ctrlSpec] <= [expr $idx + 1]} {
				set error "Control specified as '$ctrlSpec'"
				set error "$error doesn't contain '$field'"
				error $error
			} else {
				return [lindex $ctrlSpec [expr $idx + 1]]
			}
		}
	}
	
	proc onBackButton {} {
		hideWizardPage 0
		pop
		showWizardPage
	}
	
	proc onNextButton {} {
		set idx [peek]
		set idx [FSM $idx 1 [GetSheetState $idx]]
		if {[hideWizardPage] == 1} {
			push $idx
	    	showWizardPage
		}
	}
	
	
	proc onFinishButton {} {
		variable _SheetStructs
		variable _ReturnValue
		variable _ExitStatus
		variable _bFinishProcRan
		set _bFinishProcRan 1

		set idx [peek]
		if {![catch {exitProc $_SheetStructs $idx} _ReturnValue]} {
			set _ExitStatus 0 
			cleanUp
			windowClose wizardDialog
		} else {
			set _ExitStatus -1 
			cleanUp
			windowClose wizardDialog
		}
	}

	proc onExit {} {
		variable _bFinishProcRan
		variable _ExitStatus

		# If onFinishButton has already run, then this isn't
		# an abort
		if {!$_bFinishProcRan} {
			set _ExitStatus 1
			cleanUp
		}
	}

	proc onCancelButton {} {
		variable _ExitStatus
		set _ExitStatus 1
		cleanUp
		windowClose wizardDialog
	}

	# Invoke the cleanup callback prior to exiting
	proc cleanUp {} {
		variable _exitProc
		eval $_exitProc
	}

	# Call exitProc, and if it returns a 1, proceed
	proc hideWizardPage {{validate 1}} {
		variable _SheetStructs
	
		set idx [peek]
	
		# Call the page's exit routine, if it has one, and
		# hide the sheet if the routine returns non-zero (true)
		
		if {$validate} {	
			set retval [exitProc $_SheetStructs $idx]
		} else {
			set retval 1
		}
	
		if {$retval != 0} {
	    	# Hide the current page
			DestroySheet $idx
		}
		return $retval
	}
	
	# Call initProc for the page, then show it
	proc showWizardPage {} {
		variable _initProc
		variable _SheetStructs
	
		# Get current sheet index
		set idx [peek]

		# First time only: run the wizard init proc, then continue 
		if {$idx == -1} {
			set idx 0
			eval $_initProc
			pop
			push $idx
		}

		# Set the window title
		set ns [namespace qualifiers $_initProc]
		windowTitleSet wizardDialog \
			[format "%s:  step %d" [${ns}::name] [expr $idx + 1]]

		# Enable the correct buttons
	    if {[FSM $idx 0 0] == -1} {
			controlEnable wizardDialog._backbutton 0
	   	} else { 
			controlEnable wizardDialog._backbutton 1
		}
	    
		if {[FSM $idx 1 [GetSheetState $idx]] == -1} {
			controlEnable wizardDialog._nextbutton 0
		} else {
			controlEnable wizardDialog._nextbutton 1
		}
	    
		if {[FSM $idx 2 [GetSheetState $idx]] == -1} {
			controlEnable wizardDialog._finishbutton 0
		} else {
			controlEnable wizardDialog._finishbutton 1
		}

		# Show the current Sheet by creating its controls
	    CreateSheet $idx
	
		# Call the page's init routine
		initProc $_SheetStructs $idx
	}
		
	proc FSM {sheet button state} {
		variable _FSM
		set retval [lindex [lindex [lindex $_FSM $sheet] $button] $state]
	#puts "FSM($sheet,$button,$state) = $retval"
		return $retval
	}
	
	proc GetSheetState {idx} {
		variable _SheetStructs
		set struct [lindex $_SheetStructs $idx]
		set proc [lindex $struct 3]
		return [eval $proc] 
	}
	
	proc CreateSheet {sheetIdx} {
		variable _SheetStructs
	
		# Create all the controls
		set sheet [sheetCtrlsGet $_SheetStructs $sheetIdx]
		foreach ctrl $sheet {
			# Use shared bitmap resource for bitmap
			if {[string compare [lindex $ctrl 0] bitmap] == 0} {
				controlValuesSet "wizardDialog._bmpimage" \
					[GetCtrlField $ctrl -title]
			} else {
				controlCreate wizardDialog $ctrl 
			}
		}
	}
	
	proc DestroySheet {sheetIdx} {
		variable _SheetStructs
	
		# Destroy all the controls
		set sheet [sheetCtrlsGet $_SheetStructs $sheetIdx]
		foreach ctrl $sheet {
			# Use shared bitmap resource for bitmap
			if {[string compare [lindex $ctrl 0] bitmap] != 0} {
				controlDestroy [format "wizardDialog.%s" \
					[GetCtrlField $ctrl -name]]
			}
		}
	}
	
	proc push {idx} {
		variable _Stack
		if {$_Stack != ""} {
			set _Stack "$idx $_Stack"
		} else {
			set _Stack $idx
		}
	}
	
	proc pop {} {
		variable _Stack
		set elem [car $_Stack] 
		set _Stack [cdr $_Stack]
		return $elem
	}
	
	proc peek {} {
		variable _Stack
		return [car $_Stack]
	}

	###########################################################################
	#
	# progressShow - callback for use w/ prjStatusLog to give user feedback
        #
        # If progressShow is called with no arguments, or a single blank
        # argument, the status controls are hidden. 
	#
	# RETURNS: N/A
	# ERRORS: non existant project
	# 
	# DESCRIPTION: this proc takes 0 to 4 args
	# 	caption - a text string to display by itself, or with a meter
	#	current - the current meter position
	#	max - the maximum meter position
	#	abort - a callback to be called if the user presses the abort button
	#			(not currently implemented)
	#
	proc progressShow {args} {
		variable _abortCallback
		set args [lindex $args 0]

		set caption [lindex $args 0]
		if {[llength $args] == 4} {
			set abortCallback [lindex $args 3]
		} 
		if {[llength $args] >= 3} {
			set current [lindex $args 1]
			set max [lindex $args 2]
		} elseif {[llength $args] <= 1} {
			set current 0 
			set max 0 
		        if {$caption == ""} {
			    # If the caption is "", the meter should be hidden
			    controlHide wizardDialog.lblprogress 1
			    controlHide wizardDialog.lblprogress2 1
			    controlHide wizardDialog.meterprogress 1
			    return
                        }
		}

		# If we have numbers, show the meter and the smaller label
		if {$max} {
			controlHide wizardDialog.lblprogress2 1
			controlShow wizardDialog.lblprogress 1
			controlShow wizardDialog.meterprogress 1 
			controlValuesSet wizardDialog.meterprogress \
				[list 0 $max $current $current] 
			uiEventProcess -allowpaintonly
		} else {
			# Otherwise, no meter, larger label
			controlHide wizardDialog.lblprogress 1 
			controlHide wizardDialog.meterprogress 1 
			controlShow wizardDialog.lblprogress2 1 
		}

		# Set the text, if it has changed
		set oldText [controlValuesGet wizardDialog.lblprogress]
		if {![string match $oldText $caption]} {
			controlTextSet wizardDialog.lblprogress $caption
			controlTextSet wizardDialog.lblprogress2 $caption
		}
	}

	proc buttonFocusSet {idx} {
		variable _lastButton
		switch $idx {
			0 {
				set button _backbutton
			}
			1 {
				set button _nextbutton
			}
			2 {
				set button _finishbutton
			}
			default {
				error "buttonFocusSet: bad button index '$idx'"
			}
		}
		if {[info exists _lastButton]} {
			controlPropertySet wizardDialog.$_lastButton -defaultbutton 0
		}
		controlPropertySet wizardDialog.$button -defaultbutton 1
		set _lastButton $button
		controlFocusSet wizardDialog.$button
	}
}
