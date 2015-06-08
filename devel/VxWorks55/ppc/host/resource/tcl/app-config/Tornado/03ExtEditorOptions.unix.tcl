##########################################################
# ExtEditorOptions.unix.tcl - Allows external editor options to be configured
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,06sep98,aam  written
#


namespace eval ExtEditorPage {
    variable m_self
    set m_self(title) "Editor"
    set m_self(width) 300
    set m_self(height) 190
    set m_self(xpos) 5
    set m_self(ypos) 5

    variable m_controlNames {lblExtEditor}
    variable m_parent tornadoOptionsDlg

    proc onPageInit {} {
	variable m_parent

	foreach control [controlListGet] {
	    controlCreate $m_parent $control
	}

    }

    proc onPageEnter {} {

	controlsHide 0
    }

    proc queryPageExit {} {

	controlsHide 1
	return 1
    }

    proc onPageRestore {} {
    }

    proc onPageSave {} {
    }

    proc controlListGet {} {
	variable m_self

	set lblExtEditor_x [expr $m_self(xpos) + 5]
	set lblExtEditor_y [expr $m_self(ypos) + 25]
	set lblExtEditor_h 16
	set lblExtEditor_w 90

	return  [list \
		    [list label -name lblExtEditor \
			 -hidden -title "To be implemented" \
			 -x $lblExtEditor_x -y $lblExtEditor_y \
			 -h $lblExtEditor_h -w $lblExtEditor_w]
		]
			 
    }

    proc controlsHide {bHide} {
	variable m_controlNames
	variable m_parent

	foreach controlName $m_controlNames {
	    controlHide $m_parent.$controlName $bHide
	}
    }

    proc registerPage {} {
	variable m_self

	OptionsPageAdd "$m_self(title)" \
	    ExtEditorPage::onPageInit \
	    ExtEditorPage::onPageEnter \
	    ExtEditorPage::queryPageExit \
	    ExtEditorPage::onPageRestore \
	    ExtEditorPage::onPageSave
	    
    }

}

# Disable options page until it is completed.
#
# ExtEditorPage::registerPage
