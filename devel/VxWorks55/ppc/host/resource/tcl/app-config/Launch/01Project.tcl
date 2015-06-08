# Project.tcl - Support for launching the Tornado Project tool.
#
# modification history
# --------------------
#*/


# Add a Project activation button to bottom of the launcher

objectCreate app btProject Project {launchProject}


# Add Project support to "Support" menu item

menuButtonCreate Support "Project..." W {supportRequest "Project 1.0" }


proc launchProject {} {
    global tgtsvr_selected

    if {$tgtsvr_selected != ""} {
	exec tornado -t "$tgtsvr_selected" &
    } else {
	exec tornado &
    }

}

