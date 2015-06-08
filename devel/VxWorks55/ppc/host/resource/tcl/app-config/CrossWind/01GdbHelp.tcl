# 01GdbHelp.tcl - interface to GDB's on-line help
#
# Copyright 1995 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,27oct95,c_s  fixed dialog layout (SPR #5205).
# 01a,30may95,c_s  written.
#*/

menuButtonCreate    Help                "GDB Online"    G \
    {dialogPost "Help On Topic"}

# Initialize help dialog. 

textWindowCreate "GDB Online Help" -size 550 250
dialogCreate "Help On Topic" -size 290 100 \
	{{button "OK" -left 0 -right 50 -bottom . gdbTopicHelp} 
         {button "Dismiss" -left 50 -right 100 -bottom .
	     {dialogUnpost "Help On Topic"}}
	 {text "Topic" -hspan -top . -bottom OK gdbTopicHelp}}
    
# help procedures

proc gdbTopicHelp {} {
    global gdbHelpStack 
    set topic [dialogGetValue "Help On Topic" "Topic"]
    # clear stack--this is the start of a new traversal
    set gdbHelpStack ""
    gdbHelpFind $topic
    textWindowPost "GDB Online Help"
}

proc gdbHelpTraverse {helpline} {
    global gdbHelpStack
    if [regexp "\(.*\) -- .*" $helpline all topic] {
	gdbHelpFind $topic
    } {
	if {[set hsLen [llength $gdbHelpStack]] > 1} {
	    # throw away top entry.
	    set gdbHelpStack [lrange $gdbHelpStack 0 [expr $hsLen - 2]]
	    # find entry under top
	    set oldTopic [lindex $gdbHelpStack [expr $hsLen - 2]]
	    # throw it away too--it'll be pushed back in a moment
	    set gdbHelpStack [lrange $gdbHelpStack 0 [expr $hsLen - 3]]

	    gdbHelpFind $oldTopic
	} {
	    set gdbHelpStack ""
	    gdbHelpFind ""
	}
    }
}

proc gdbHelpFind {topic} {
    global gdbHelpStack
    if {[catch {set helpOnTopic [downtcl gdb help $topic]}] == 0} {
	if {$topic != ""} {
	    lappend gdbHelpStack $topic
	}
	textWindowSet "GDB Online Help" $helpOnTopic
    }
}
    
# Module initialization

set gdbHelpStack ""


