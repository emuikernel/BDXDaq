##############################################################################
# 02EditorOptions.win32.tcl - Tornado for Windows options dialog implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01e,21dec98,rbl   Fixed SPR 22865, "Automatic Reload of Externally Modified Files" 
#                   Editor option is broken, by adding AutoReload registry entry. Tidied
#                   up structure of reading and writing registry entries to use assoc. 
#                   arrays rather than lists. 
# 01d,13nov98,rbl Changed layout so it isn't so cramped. 
# 01c,11nov98,rbl	Added File Format options to support saving files in UNIX or DOS 
#					format.	
# 01b,19oct98,rbl	Fixed SPR 9832, Tornado crashes when setting tab size to zero:
#					only allow tab sizes 1-16. Change defaults to set horizontal
#					scrolling on by default.
# 01a,01jun98,j_k  written
#

#
# Do not source this file if the platform is not win32
#
if {![string match *win32* [wtxHostType]]} return

namespace eval EditorPage {
	# Integer and string registry items. 
	#
	# The structure of arrayEditorInteger(String)RegEntries is copied from 
	# arrayEditorDefaultInteger(String)RegEntries
	# so you only have to define the integer (string) items once in 
	# arrayEditorDefaultInteger(String)RegEntries, below.
	#
	# To add a new item:
	#
	# 1. Put a default value into arrayEditorDefaultInteger(String)RegEntries below
	# 2. Add code to init {} to initialize the corresponding control from the 
	# value in arrayEditorInteger(String)RegEntries
	# 3. Add code to exit {} (or a callback function for the control) to set
	# the value in arrayEditorInteger(String)RegEntries from the control
	#
	# If you change any default values, you need to also change the 
	# defaults in CWrsIdeApp::ReadProfileItems()
	#
	variable arrayEditorIntegerRegEntries
	variable arrayEditorDefaultIntegerRegEntries
	variable arrayEditorStringRegEntries
	variable arrayEditorDefaultStringRegEntries

	# Integer registry items. 

	set arrayEditorDefaultIntegerRegEntries(UserSet) "0"
	set arrayEditorDefaultIntegerRegEntries(AttributePane) "1"
	set arrayEditorDefaultIntegerRegEntries(HorizontalScrolling) "1"
	set arrayEditorDefaultIntegerRegEntries(MaxUndoLevels) "512"
	set arrayEditorDefaultIntegerRegEntries(SaveBeforeRunning) "1"
	set arrayEditorDefaultIntegerRegEntries(TabStops) "8"
	set arrayEditorDefaultIntegerRegEntries(VerticalScrolling) "1"
	set arrayEditorDefaultIntegerRegEntries(AutoReload) "1"

	# String registry items. 

	set arrayEditorDefaultStringRegEntries(FileFormat) "Automatic"

	# Controls

	variable gEditorPageCtrls [list winSettGrp horzScrlBar verScrlBar \
			attrPane tabSizeLbl tabSizeEdit maxUndoLbl maxUndoLvlsEdit \
			saveOptGrp saveBefore autoReload \
			fileFormatGrp fileFormatDos fileFormatUnix fileFormatAutomatic ]

	proc onFileFormat {format} {
		variable arrayEditorStringRegEntries
		set arrayEditorStringRegEntries(FileFormat) $format
	}

	proc init {} {
		variable arrayEditorIntegerRegEntries
		variable gEditorPageCtrls
		variable arrayEditorStringRegEntries

		set Controls [list \
			[list group -hidden -name winSettGrp -title "Window Settings" \
				-xpos 12 -ypos 20 -width 286 -height 24] \
			[list boolean -hidden -auto -name horzScrlBar -title "Horizontal Scrollbar" \
				-xpos 20 -ypos 30 -width 80 -height 10] \
			[list boolean -hidden -auto -name verScrlBar -title "Vertical Scrollbar" \
				-xpos 110 -ypos 30 -width 70 -height 10] \
			[list boolean -hidden -auto -name attrPane -title "Attribute Pane" \
				-xpos 200 -ypos 30 -width 80 -height 10] \
			[list boolean -hidden -auto -disabled -name dragDropEdit \
				-title "Drag-and-drop text editing" \
				-xpos 170 -ypos 44 -width 100 -height 10] \
			[list label -hidden -name tabSizeLbl -title "Tab Size:" \
				-xpos 12 -ypos 54 -width 30 -height 9] \
			[list text -hidden -name tabSizeEdit -xpos 50 -ypos 52 -width 40 -height 12] \
			[list label -hidden -name maxUndoLbl -title "Maximum Undo Levels:" \
				-xpos 146 -ypos 54 -width 74 -height 9] \
			[list text -hidden -name maxUndoLvlsEdit -xpos 228 -ypos 52 -width 40 -height 12] \
			[list group -hidden -name saveOptGrp -title "Save Options" \
				-xpos 12 -ypos 68 -width 286 -height 24] \
			[list boolean -hidden -auto -name saveBefore \
				-title "Save before running Tools/Builds" \
				-xpos 20 -ypos 78 -width 118 -height 10] \
			[list boolean -hidden -auto -name autoReload \
				-title "Automatic reload of externally modified files" \
				-xpos 146 -ypos 78 -width 148 -height 10] \
			[list group -hidden -name fileFormatGrp -title "File Format" \
				-xpos 12 -ypos 96 -width 286 -height 54] \
	            	[list choice -title "DOS End of Line" -name fileFormatDos \
                		-auto -newgroup \
				-callback "EditorPage::onFileFormat DOS" \
                		-xpos 20 -ypos 108 -width 130 -height 10  ] \
	            	[list choice -title "UNIX End of Line" -name fileFormatUnix \
				-callback "EditorPage::onFileFormat UNIX" \
		            -auto -xpos 20 -ypos 122 -width 130 -height 10 ] \
	            	[list choice -title "Automatic (preserve existing format)" -name fileFormatAutomatic \
				-callback "EditorPage::onFileFormat Automatic" \
		            -auto -xpos 20 -ypos 136 -width 130 -height 10 ] \
		]

		foreach ctrl $Controls {
			controlCreate tornadoOptionsDlg $ctrl
		}
		
		# Set controls corresponding to integer registry entries

		controlCheckSet tornadoOptionsDlg.attrPane $arrayEditorIntegerRegEntries(AttributePane)
		controlCheckSet tornadoOptionsDlg.horzScrlBar $arrayEditorIntegerRegEntries(HorizontalScrolling)
		controlTextSet tornadoOptionsDlg.maxUndoLvlsEdit $arrayEditorIntegerRegEntries(MaxUndoLevels)
		controlCheckSet tornadoOptionsDlg.saveBefore $arrayEditorIntegerRegEntries(SaveBeforeRunning)
		controlTextSet tornadoOptionsDlg.tabSizeEdit $arrayEditorIntegerRegEntries(TabStops)
		controlCheckSet tornadoOptionsDlg.verScrlBar $arrayEditorIntegerRegEntries(VerticalScrolling)
		controlCheckSet tornadoOptionsDlg.autoReload $arrayEditorIntegerRegEntries(AutoReload)

		# Set controls corresponding to string registry entries

		switch $arrayEditorStringRegEntries(FileFormat) {
			DOS		{controlCheckSet tornadoOptionsDlg.fileFormatDos 1}
			UNIX		{controlCheckSet tornadoOptionsDlg.fileFormatUnix 1}
			Automatic	{controlCheckSet tornadoOptionsDlg.fileFormatAutomatic 1}
		}
	}

	proc enter {} {
		variable gEditorPageCtrls

		foreach ctrl $gEditorPageCtrls {
			controlShow tornadoOptionsDlg.$ctrl 1
		}
	}

	proc exit {} {
		variable arrayEditorIntegerRegEntries
		variable arrayEditorStringRegEntries
		variable gEditorPageCtrls

		set check [controlChecked tornadoOptionsDlg.attrPane]
		if {$check == ""} {
			set check 1
		}
		set arrayEditorIntegerRegEntries(AttributePane) $check

		set check [controlChecked tornadoOptionsDlg.horzScrlBar]
		if {$check == ""} {
			set check 0
		}
		set arrayEditorIntegerRegEntries(HorizontalScrolling) $check

		set data [controlTextGet tornadoOptionsDlg.maxUndoLvlsEdit]
		if {![regexp {^ *[0-9]+} $data data] ||
			[expr $data] > 999 || [expr $data] < 1} {
			messageBox "Please enter a valid number between 1 and 999 for Maximum Undo Levels"
			return 0
		}
		set arrayEditorIntegerRegEntries(MaxUndoLevels) $data

		set check [controlChecked tornadoOptionsDlg.saveBefore]
		if {$check == ""} {
			set check 1
		}
		set arrayEditorIntegerRegEntries(SaveBeforeRunning) $check

		set data [controlTextGet tornadoOptionsDlg.tabSizeEdit]
		if {![regexp {^ *[0-9]+} $data data] ||
			[expr $data] > 16 || [expr $data] < 1} {
			messageBox "Please enter a valid number between 1 and 16 for Tab Size"
			return 0
		}
		set arrayEditorIntegerRegEntries(TabStops) $data

		set check [controlChecked tornadoOptionsDlg.verScrlBar]
		if {$check == ""} {
			set check 1
		}
		set arrayEditorIntegerRegEntries(VerticalScrolling) $check

		set check [controlChecked tornadoOptionsDlg.autoReload]
		if {$check == ""} {
			set check 1
		}
		set arrayEditorIntegerRegEntries(AutoReload) $check

		# Set flag to indicate that User has changed options
		set arrayEditorIntegerRegEntries(UserSet) 1
		
		foreach ctrl $gEditorPageCtrls {
			controlShow tornadoOptionsDlg.$ctrl 0
		}
		return 1
	}

	proc restore {} {
		variable arrayEditorIntegerRegEntries
		variable arrayEditorOrigIntegerRegEntries
		variable arrayEditorDefaultIntegerRegEntries
		variable arrayEditorStringRegEntries
		variable arrayEditorOrigStringRegEntries
		variable arrayEditorDefaultStringRegEntries

		# read the editor information (integer registry values)		
		foreach key [array names arrayEditorDefaultIntegerRegEntries] {
			set arrayEditorOrigIntegerRegEntries($key) [appRegistryEntryRead \
				-int -default $arrayEditorDefaultIntegerRegEntries($key) Editor $key ]
		}

		# read the editor information (string registry values)		
		foreach key [array names arrayEditorDefaultStringRegEntries] {
			set arrayEditorOrigStringRegEntries($key) [appRegistryEntryRead \
				-default $arrayEditorDefaultStringRegEntries($key) Editor $key ]
		}
 
		# if the user has set the entries, or used defaults:

		set bUserHasSetOptions $arrayEditorOrigIntegerRegEntries(UserSet)

		if {$bUserHasSetOptions == 1} {
			# integer entries
			foreach key [array names arrayEditorDefaultIntegerRegEntries] {
				set arrayEditorIntegerRegEntries($key) $arrayEditorOrigIntegerRegEntries($key) 
			}

			#string entries
			foreach key [array names arrayEditorDefaultStringRegEntries] {
				set arrayEditorStringRegEntries($key) $arrayEditorOrigStringRegEntries($key) 
			}
		} else { ;# tornado editor defaults
			# integer defaults
			foreach key [array names arrayEditorDefaultIntegerRegEntries] {
				set arrayEditorIntegerRegEntries($key) $arrayEditorDefaultIntegerRegEntries($key) 
			}

			# string defaults
			foreach key [array names arrayEditorDefaultStringRegEntries] {
				set arrayEditorStringRegEntries($key) $arrayEditorDefaultStringRegEntries($key) 
			}
		}
	}

	proc save {} {
		variable arrayEditorStringRegEntries
		variable arrayEditorIntegerRegEntries

		# write the integer values
		foreach key [array names arrayEditorIntegerRegEntries] {
			appRegistryEntryWrite -int Editor $key $arrayEditorIntegerRegEntries($key)
		}

		# write the string values
		foreach key [array names arrayEditorStringRegEntries] {
			appRegistryEntryWrite Editor $key $arrayEditorStringRegEntries($key)
		}

	}
}

OptionsPageAdd Editor EditorPage::init EditorPage::enter \
	EditorPage::exit EditorPage::restore EditorPage::save
