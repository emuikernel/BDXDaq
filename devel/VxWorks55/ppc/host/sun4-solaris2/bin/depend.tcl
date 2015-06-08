# depend.tc - pattern substitution file for dependency lists
#
# modification history
# --------------------
# 01b,21mar01,mrs  Added WIND_BASE processing.
# 01a,12aug96,wmd
#             +yp   written
#*/

# DESCRIPTION
# This file converts path names generated by the compiler back to Makefile
# variables and appends the the variable $(LIBDIR)/ to the object being 
# created so that at compile time the object will get placed where we want
# it. 
#
# 

if {$argc != 1} {
	error "usage : depend.tcl \$\(TGT_DIR\)"
}
set uppat [lindex $argv 0]
#double backslashes
regsub -all {\\}  $uppat {\\\\}  uppat2

set wb1 $env(WIND_BASE)
regsub -all {\\}  $wb1 {\\\\} wind_base

while {[gets stdin line] >= 0} {
        regsub {^.*\.o} $line "\$\(LIBDIR\)/&" newline
	regsub -all $uppat2 $newline "\$\(TGT_DIR\)" newline2
	regsub -all $wind_base $newline2 "\$\(WIND_BASE\)" newline
        puts stdout $newline
}