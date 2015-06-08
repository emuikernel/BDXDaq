# WView.tcl - WindView support
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01e,08oct01,fle  unified Tornado version String
# 01d,10sep98,p_m  set version to 2
# 01c,08jan97,c_s  update parameters to windview call
# 01b,05jan96,jco  added "..." to menu button label.
# 01a,25may95,jcf  extracted from Launch.tcl.

package require Wind

# Add a WindView activation button to bottom of the launcher

objectCreate app windview    WindView 	{launchWindView}


# Add WindView support to "Support" menu item

menuButtonCreate Support "WindView..." W {supportRequest \
					  "WindView [Wind::version]" }


proc launchWindView {} {
    global tgtsvr_selected

    if {$tgtsvr_selected != ""} {
	exec windview -xrm "WindView.targetHostName: $tgtsvr_selected" &
    } else {
	exec windview &
    }

}
