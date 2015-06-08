#  for setting lib path for tcl vsn 8 or above

set dir $env(CLON_LIB)

package ifneeded sgml 1.7 [list source [file join $dir sgml.tcl]]
package ifneeded xml 1.9 [list source [file join $dir xml.tcl]]

