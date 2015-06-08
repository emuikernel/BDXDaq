if {[catch {package require Tcl}]} return

package ifneeded Wind 1.0 "\
    [list tclPkgSetup $dir Wind 1.0 {
{Wind.tcl source {Wind::hostType Wind::base Wind::which Wind::user Wind::tmpDirGet Wind::path Wind::version Wind::key Wind::appDataDirGet Wind::patchlevel}}
}]; \
    [list source [ file join $dir Log.tcl]]; \
"
