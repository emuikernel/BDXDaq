##########################################################
# Tornado.tcl - Main window for UNIX version of Tornado Project	
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01q,08oct01,fle  unified Tornado version String
# 01p,27feb01,hk   update AboutTornadoWindow copyright notice to 2001, v2.1.0.
# 01o,17mar99,bjl  Added sourcing of CodeTEST environment file.
# 01n,16mar99,ren  Changing copyright notice to 1995-1999
# 01m,04mar99,jak  remove updatePreferences - no longer needed by ToolOptions
# 01l,04mar99,ren  Moved tree control back into position for MW 3.1.
# 01k,18feb99,jak  add helpfile path
# 01j,12feb99,jak  change help menu
# 01i,01dec98,cjs  remove default toolbar from project window 
# 01h,03nov98,ren  Implemented Menu | File | Open.
# 01g,13oct98,aam  Added safe call to welcomeDlgShow() 
# 01f,08oct98,aam  Fixed problem that allowed an exit without saving projects.
# 01e,28sep98,aam  Source wtxcore.tcl file
# 01d,21sep98,aam  Add a dummy proc "updatePreferences" that exists in the
#                  Windows IDE, but does not exist on UNIX.  The proc need 
#                  exist on UNIX, it does not have to do anything.
# 01c,21sep98,aam  Change any reference to the word "Workspace" to "Project".
# 01c,21sep98,c_s  remove .so from load statement
# 01b,09sep98,jmp  replaced wtxtcl.so by libwtxtcl.so.
# 01a,01aug98,aam  written
#

package require Wind

# Some temporary code used to help do debugging
#
if {0} {
rename source old_source
proc source {args} {
    global src_indent

    lappend src_indent :
    puts "$src_indent $args"
    uplevel old_source $args
    set src_indent [lrange $src_indent 1 [llength $src_indent]]
}
}

# 
# Load needed shared libraries
#

# UITclControls shared library
#
dllLoad libuitclcontrols _UITclControls_Init

# WTX prototocol implementation
#
global env
load $env(WIND_BASE)/host/$env(WIND_HOST_TYPE)/lib/libwtxtcl[info sharedlibextension]
source [wtxPath host resource tcl]wtxcore.tcl

# the help support - should be sourced only after uitcl has been loaded
if {[catch {source [wtxPath host resource tcl]Help.tcl} err]} {
	messageBox "error: unable to source Help.tcl. Context help will be disabled"
}

# Read in the CodeTEST Tcl initialization file.
#
catch {source [wtxPath host resource tcl]CodeTEST.tcl}

namespace eval MainWindow {
    variable  m_menuName MainWindow::menu
    variable m_xpos 0
    variable m_ypos 0
    variable m_width 220
    variable m_height 300
    variable m_title "Tornado Project"
    variable m_name mainwindow
    variable m_menuFilePath {&File {&Open...}}
    variable m_helpFile 	;# the win32 helpfile for this app
    set m_helpFile [wtxPath host resource help]tornadou.hlp

    proc show {} {
	variable m_xpos
	variable m_ypos
	variable m_width
	variable m_height
	variable m_title
	variable m_name
    	variable m_helpFile 

        mainWindowCreate -msdi -statusbar \
	    -xpos $m_xpos -ypos $m_ypos \
	    -width $m_width -height $m_height \
		-helpfile $m_helpFile \
            -name $m_name -title $m_title

	windowQueryCloseCallbackSet $m_name \
	    ::MainWindow::queryClose

	menusCreate

	windowSizeCallbackSet $m_name \
	    ::MainWindow::OnSize

    }

    proc queryClose {} {

	return [::ViewBrowser::queryClose]
    
    }

    proc OnSize {} {
	if {[string match [info commands ::ViewBrowser::OnSize] ""]} {
	    return
	}
	::ViewBrowser::OnSize
    }

    proc onExit {} {
	variable m_name

	windowClose $m_name
    }

    proc OnHelpTopics {} {
    	variable m_helpFile 

	if {![file exists $m_helpFile]} {
		messageBox "error: unable to locate help file \"$m_helpFile\""
		return
	}
	if {[catch {eval "helpLaunch index $m_helpFile 0"} err] } {
		messageBox "error: $err"
	}
    }

    proc menusCreate {} {
	variable m_menuName
	variable m_name

	menuCreate -name $m_menuName
	windowMenuSet $m_name $m_menuName

	#
	# File menus - default file menus
	#	
	menuItemAppend $m_menuName \
	    -name mnuFileMenu \
	    -popup {&File} 

 	menuItemAppend mnuFileMenu -callback \
	    ::FileLaunch::fileSelectionDialogDisplay \
	    {&Open...}

	menuItemAppend mnuFileMenu -separator 
	menuItemAppend mnuFileMenu -callback \
	    { ::MainWindow::onExit } \
	    {Exit}

	#
	# View menu - default view menus
	#
	menuItemAppend $m_menuName \
	    -name mnuViewMenu \
	    -popup {&View} 


	#
	# Tools Menu - default tools menus
	#
	menuItemAppend $m_menuName \
	   -name mnuToolsMenu \
	   -popup {&Tools}

	#
	# Help menus - default help menus
	#	
	menuItemAppend $m_menuName \
	   -name mnuHelpMenu \
	   -popup {&Help}

	menuItemAppend mnuHelpMenu -callback \
	    { MainWindow::OnHelpTopics} \
	    {&Help Topics}

	menuItemAppend mnuHelpMenu -separator

	menuItemAppend mnuHelpMenu -callback \
	    { AboutTornadoWindow::show } \
	    {&About Tornado Project}

    }

    proc controlsCreate {} {
	variable m_name

	set ::ViewBrowser::_windowName $m_name
	set ::ViewBrowser::_treeviewYPos 28
	set ::ViewBrowser::_bBoldWithBraces 1
	set ::ViewBrowser::_hideOnClose 0
	::ViewBrowser::viewBrowserCreate
    }

    proc messageUnavailableInDemo {} {
	messageBox "Not available in demo version"
    }

}

namespace eval AboutTornadoWindow {
    variable m_self
    set m_self(title) "About Tornado Project"
    set m_self(width) 200
    set m_self(height) 100

    proc show {} {
	variable m_self

	dialogCreate -name AboutTornadoWindow \
	    -title $m_self(title) \
	    -w $m_self(width) -h $m_self(height) \
	    -init AboutTornadoWindow::onInit
    }

    proc controlListGet {} {
	variable m_self

	set lblDescription_x 35
	set lblDescription_y 16
	set lblDescription_w 140
	set lblDescription_h 16

	set lblCopyright_x $lblDescription_x
	set lblCopyright_y [expr $lblDescription_y + 16]
	set lblCopyright_w $lblDescription_w
	set lblCopyright_h $lblDescription_h

	set lblVersion_x $lblDescription_x
	set lblVersion_y [expr $lblCopyright_y + 16]
	set lblVersion_w $lblDescription_w
	set lblVersion_h $lblDescription_h

	set cbOK_w 50
	set cbOK_h 14
	set cbOK_x [expr $m_self(width) / 2 - $cbOK_w / 2]
	set cbOK_y [expr $lblVersion_y + 32]

	return [list \
		    [list label -name lblDescription \
			 -title "Tornado Project Tool" \
			 -x $lblDescription_x -y $lblDescription_y \
			 -w $lblDescription_w -h $lblDescription_h] \
		    [list label -name lblCopyright \
			 -title "Copyright 1995-2001 Wind River Systems, Inc." \
			 -x $lblCopyright_x -y $lblCopyright_y \
			 -w $lblCopyright_w -h $lblCopyright_h] \
		    [list label -name lblVersion \
			 -title "Tornado Version [Wind::version]" \
			 -x $lblVersion_x -y $lblVersion_y \
			 -w $lblVersion_w -h $lblDescription_h] \
		    [list button -name cbOK \
			 -title "OK" \
			 -default \
			 -x $cbOK_x -y $cbOK_y \
			 -w $cbOK_w -h $cbOK_h \
			 -callback {windowClose AboutTornadoWindow}]
		]

    }

    proc onInit {} {

	foreach control [controlListGet] {
	    controlCreate AboutTornadoWindow $control
	}
    }

}

namespace eval Tornado {
    variable m_target {}

    proc main {argc cmdline} {
	variable m_target

	if { [ catch {Tornado::parseCmdLineArgs $argc $cmdline} retValue] } {
	    Tornado::usageDisplay
	    return
	} 

	applicationTitleSet "Tornado Project"

	MainWindow::show
	bootComponents
	::SelectedTarget::targetSet $m_target
	MainWindow::controlsCreate

	# Now that the view browser window has been created,
	# we can risk a workspace autoload
	::WelcomeDlg::welcomeDlgShow
    }

    proc bootComponents {} {
	# Boot all the subcomponents of Tornado
	#
	namespace eval :: {

	    # Source in some utilities for running terminals, and editing files.
	    #
	    source [wtxPath host resource tcl app-config all]hostUtils.tcl

	    set cfgdir [wtxPath host resource tcl app-config Tornado]*unix.tcl
	    foreach file [lsort [glob -nocomplain $cfgdir]] {
		if [ catch {source $file} err ] {
		    puts stderr "error: unable to source tcl script\n  \
					\"$file\"\n$err"
		}
	    }

	}

    }

    proc parseCmdLineArgs {argc cmdline} {
	variable m_target

	# target name is optional
	if { $argc > 0 } {
	    if { $argc > 2 } {
		error "ARGS_WRONG_NUMBER"
	    }

	    # Parse the target name
	    set flag [lindex $cmdline 0]

	    # Check for the "-t", targetname, option
	    #
	    if { ! [string match -t $flag] } {
		error "ARGS_BAD_OPTION"
	    }
	    set m_target  [lindex $cmdline 1]
	}
    }

    proc usageDisplay {} {
	puts "Usage: tornado -t <targetName>"
    }
}

Tornado::main $argc $argv
