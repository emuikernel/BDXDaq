##########################################################
# BuildOutputOptions.unix.tcl - Allows build output options to be configured
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,06sep98,aam  written
#


namespace eval BuildOutputPage {
    variable m_self
    set m_self(title) "Build Output"
    set m_self(width) 300
    set m_self(height) 190
    set m_self(xpos) 5
    set m_self(ypos) 5

    variable m_controlNames {lblBuildOutput}
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

	set lblBuildOutput_x [expr $m_self(xpos) + 5]
	set lblBuildOutput_y [expr $m_self(ypos) + 45]
	set lblBuildOutput_h 16
	set lblBuildOutput_w 90

	return  [list \
		    [list label -name lblBuildOutput \
			 -hidden -title "To be implemented" \
			 -x $lblBuildOutput_x -y $lblBuildOutput_y \
			 -h $lblBuildOutput_h -w $lblBuildOutput_w]
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
	    BuildOutputPage::onPageInit \
	    BuildOutputPage::onPageEnter \
	    BuildOutputPage::queryPageExit \
	    BuildOutputPage::onPageRestore \
	    BuildOutputPage::onPageSave
	    
    }

}

# Disabled page until it is completed.
#
# BuildOutputPage::registerPage
