# by vhg --
#
# 

set w .text
catch {destroy $w}
toplevel $w
wm title $w "Diman Help"
wm iconname $w "text"
positionWindow $w

frame $w.buttons
pack  $w.buttons -side bottom -expand y -fill x -pady 2m
button $w.buttons.dismiss -text Dismiss -command "destroy $w"
pack $w.buttons.dismiss -side left -expand 1

text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	-height 5
scrollbar $w.scroll -command "$w.text yview"
pack $w.scroll -side right -fill y
pack $w.text -expand yes -fill both
$w.text insert 0.0 \
{HELP ~~  HELP~~  HELP~~  HELP~~  HELP~~~ HELP~~ HELP~~ 

User interfaces are hard to build ;-)

}
$w.text mark set insert 0.0
