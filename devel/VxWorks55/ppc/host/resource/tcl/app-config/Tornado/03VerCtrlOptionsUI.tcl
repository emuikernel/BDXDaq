##########################################################
# VerCtrlOptionsUI.win32.tcl - UI for version control options configuration 
#
# Copyright 1995-98 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01a,09sep98,jak  written - extended aam's work
#
#
# DESCRIPTION
#	This script is meant to be sourced by a platform-specific script.
# It assumes that the namespace VerCtrlDefaults is available. The UI here is
# meant to appear on an options dialog to be used to configure ver. ctrl. cmds	
#
#
# Registry entries:
#   [section key value]
#   [VersionControl checkinCmd string]
#   [VersionControl checkoutCmd string]

namespace eval VerCtrlOptionsUI {
    variable m_parent 
    variable m_self 
    variable m_controls
	variable m_cmds

	set m_parent tornadoOptionsDlg

    set m_self(title) "Version Control"
    set m_self(width) 300
    set m_self(height) 190
    set m_self(xpos) 5
    set m_self(ypos) 5

	set m_cmds(checkin) "<undefined>"
	set m_cmds(checkout) "<undefined>"


	#///////////////////////////////////
	# Options API implementation 

	#
	# onPageInit - initializes controls list, creates controls and hides them
	#	should be called every time the options dialog is created and shown
	#
    proc onPageInit {} {
		variable m_parent 
		variable m_controls 
		variable m_cmds

		set m_cmds(checkin) "<undefined>"
		set m_cmds(checkout) "<undefined>"

    	set m_controls(names) [list \
			vcOptIDC_STATIC1 vcOptIDC_STATIC2 vcOptIDC_STATIC3 vcOptIDC_STATIC4 \
			vcOptIDC_EDIT1 vcOptIDC_EDIT2 \
			vcOptIDC_BUTTON1 vcOptIDC_BUTTON2 vcOptIDC_STATIC5 vcOptIDC_COMBO1]

    	set m_controls(creationLists) [list \
			[list group -title "Commands" -name vcOptIDC_STATIC1  \
            -xpos [compX 7] -ypos [compY 15] -width 286 -height 68 \
			-hidden \
        	] \
        	[list label -title "Checkin:" -name vcOptIDC_STATIC2  \
            -xpos [compX 15] -ypos [compY 29] -width 33 -height 8  \
			-hidden \
        	] \
        	[list text -name vcOptIDC_EDIT1  \
        	-xpos [compX 52] -ypos [compY 27] -width 195 -height 14  \
			-hidden \
        	] \
        	[list button -title " > " -name vcOptIDC_BUTTON1 -menubutton \
            -xpos [compX 254] -ypos [compY 27] -width 14 -height 14  \
			-callback {VerCtrlOptionsUI::onButton vcOptIDC_BUTTON1} \
			-hidden \
        	] \
        	[list label -title "Checkout:" -name vcOptIDC_STATIC3  \
            -xpos [compX 15] -ypos [compY 47] -width 33 -height 8  \
			-hidden \
        	] \
        	[list text -name vcOptIDC_EDIT2  \
        	-xpos [compX 52] -ypos [compY 45] -width 195 -height 14  \
			-hidden \
        	] \
        	[list button -title " > " -name vcOptIDC_BUTTON2  -menubutton \
            -xpos [compX 254] -ypos [compY 45] -width 14 -height 14  \
			-callback {VerCtrlOptionsUI::onButton vcOptIDC_BUTTON2} \
			-hidden \
        	] \
        	[list label -title "Defaults:" -name vcOptIDC_STATIC4 \
            -xpos [compX 159] -ypos [compY 65] -width 50 -height 8 \
			-hidden \
        	] \
        	[list combo -name vcOptIDC_COMBO1 \
            -xpos [compX 192] -ypos [compY 63] -width 76 -height 60  \
			-callback {VerCtrlOptionsUI::onCombo vcOptIDC_COMBO1} \
			-hidden \
        	] \
        	[list label -name vcOptIDC_STATIC5 \
			-title "Note: If you require more functionality, please use \
					Tools, Customize." \
            -xpos [compX 15] -ypos [compY 90] -width 275 -height 30  \
			-hidden \
        	] 
									]

		# advanced button, for future use:
		if {0} {
        	[list button -title "Advanced..." -name vcOptIDC_BUTTON3  \
            -xpos [compX 15] -ypos [compY 90] -width 50 -height 14  \
			-callback {VerCtrlOptionsUI::onButton vcOptIDC_BUTTON3} \
			-hidden \
        	] 
		}

		foreach control $m_controls(creationLists) {
	    	controlCreate $m_parent $control
		}
	
		# initialize the combo for systems:
		controlValuesSet $m_parent.vcOptIDC_COMBO1 \
			[VerCtrlDefaults::getSysList]

		# initialize the commands:
		set tmpVal [appRegistryEntryRead VersionControl checkinCmd]
		if {$tmpVal != ""} {
    		set  m_cmds(checkin) $tmpVal
		}
		set tmpVal [appRegistryEntryRead VersionControl checkoutCmd]
		if {$tmpVal != ""} {
    		set m_cmds(checkout) $tmpVal
		}		
		controlTextSet $m_parent.vcOptIDC_EDIT1 $m_cmds(checkin)
		controlTextSet $m_parent.vcOptIDC_EDIT2 $m_cmds(checkout)
    }

    proc onPageEnter {} {
		variable m_cmds
		variable m_parent
		controlsHide 0
		controlTextSet $m_parent.vcOptIDC_EDIT1 $m_cmds(checkin)
		controlTextSet $m_parent.vcOptIDC_EDIT2 $m_cmds(checkout)
    }

    proc queryPageExit {} {
		variable m_cmds
		variable m_parent
		set m_cmds(checkin) [controlTextGet $m_parent.vcOptIDC_EDIT1] 
		set m_cmds(checkout) [controlTextGet $m_parent.vcOptIDC_EDIT2]
		controlsHide 1
		return 1
    }

    proc onPageRestore {} {
    }

    proc onPageSave {} {
		variable m_cmds
		variable m_parent
		set m_cmds(checkin) [controlTextGet $m_parent.vcOptIDC_EDIT1] 
		set m_cmds(checkout) [controlTextGet $m_parent.vcOptIDC_EDIT2]
    	appRegistryEntryWrite VersionControl checkinCmd $m_cmds(checkin)
    	appRegistryEntryWrite VersionControl checkoutCmd $m_cmds(checkout)
    }
	
	#
	# compX - computes absolute X position
	#
	proc compX {x} {
		variable m_self

		return [expr $m_self(xpos) + $x]
	}

	#
	# compY - computes absolute Y position
	#
	proc compY {y} {
		variable m_self

		return [expr $m_self(ypos) + $y]
	}

    proc controlsHide {bHide} {
		variable m_controls
		variable m_parent

		foreach controlName $m_controls(names) {
	    	controlHide $m_parent.$controlName $bHide
		}
    }

	#
	# registerPage - adds this page to options tabbed dlg
	#
    proc registerPage {} {
		variable m_self

		OptionsPageAdd "$m_self(title)" \
	    	VerCtrlOptionsUI::onPageInit \
	    	VerCtrlOptionsUI::onPageEnter \
	    	VerCtrlOptionsUI::queryPageExit \
	    	VerCtrlOptionsUI::onPageRestore \
	    	VerCtrlOptionsUI::onPageSave
	    
    }

	#///////////////////////////////////
	# control callbacks

	proc onButton {bID} {
		global gMenuToShow

		switch $bID {
			vcOptIDC_BUTTON1 {
				set gMenuToShow vcOptIDM_CIMACROS 
				menuCreate -name vcOptIDM_CIMACROS -context
                menuItemAppend vcOptIDM_CIMACROS -callback \
                    {VerCtrlOptionsUI::onMenu vcOptIDM_CIMACROS browse} \
                    "Browse..."
                menuItemAppend vcOptIDM_CIMACROS -separator
				menuItemAppend vcOptIDM_CIMACROS -callback \
					{VerCtrlOptionsUI::onMenu vcOptIDM_CIMACROS filename} \
					"File name"
				menuItemAppend vcOptIDM_CIMACROS -callback \
					{VerCtrlOptionsUI::onMenu vcOptIDM_CIMACROS comment} \
					"Comment"
				return 1
			}
			vcOptIDC_BUTTON2 {
				set gMenuToShow vcOptIDM_COMACROS 
				menuCreate -name vcOptIDM_COMACROS -context 
                menuItemAppend vcOptIDM_COMACROS -callback \
                    {VerCtrlOptionsUI::onMenu vcOptIDM_COMACROS browse} \
                    "Browse..."
                menuItemAppend vcOptIDM_COMACROS -separator
				menuItemAppend vcOptIDM_COMACROS -callback \
					{VerCtrlOptionsUI::onMenu vcOptIDM_COMACROS filename} \
					"File name"
				menuItemAppend vcOptIDM_COMACROS -callback \
					{VerCtrlOptionsUI::onMenu vcOptIDM_COMACROS comment} \
					"Comment"
				return 1
			}
			vcOptIDC_BUTTON3 {
				# calls on Tools, Customize
				::showCustomizeUI tools
			}
		}
	}

	proc onMenu {mID mItemID} {
		variable m_parent

		switch $mID {
			vcOptIDM_CIMACROS {
				switch $mItemID {
            		browse {
                		set pathName [lindex \
                    		[fileDialogCreate -title "Select Executable" \
							-okbuttontitle "Select" ] \
                            0]
                		if {$pathName != ""} {
                  			controlTextInsert $m_parent.vcOptIDC_EDIT1 \
                                    			-cursor $pathName 
                		}
                		return 1
            		}
					filename {
                  		controlTextInsert $m_parent.vcOptIDC_EDIT1 \
                                    			-cursor "\$filename"
						return 1
					}
					comment {
                  		controlTextInsert $m_parent.vcOptIDC_EDIT1 \
                                    			-cursor "\$comment"
						return 1
					}
				}
			}
			vcOptIDM_COMACROS {
				switch $mItemID {
            		browse {
                		set pathName [lindex \
                    		[fileDialogCreate -title "Select Executable" \
							-okbuttontitle "Select" ] \
                            0]
                		if {$pathName != ""} {
                  			controlTextInsert $m_parent.vcOptIDC_EDIT2 \
                                    			-cursor $pathName 
                		}
                		return 1
            		}
					filename {
                  		controlTextInsert $m_parent.vcOptIDC_EDIT2 \
                                    			-cursor "\$filename"
						return 1
					}
					comment {
                  		controlTextInsert $m_parent.vcOptIDC_EDIT2 \
                                    			-cursor "\$comment"
						return 1
					}
				}
			}
		}
	}

	proc onCombo {cID} {
		variable m_parent
		set event [controlEventGet $m_parent.$cID]
		if {$event == "selchange"} {
			set iSel [controlSelectionGet $m_parent.$cID]
			if {$iSel < 0} {return}
			set ciCmd [VerCtrlDefaults::getCmdDefault checkin $iSel] 
			controlTextSet $m_parent.vcOptIDC_EDIT1 $ciCmd
			set coCmd [VerCtrlDefaults::getCmdDefault checkout $iSel] 
			controlTextSet $m_parent.vcOptIDC_EDIT2 $coCmd
		}
	}

}

# Register this as an options page
VerCtrlOptionsUI::registerPage
