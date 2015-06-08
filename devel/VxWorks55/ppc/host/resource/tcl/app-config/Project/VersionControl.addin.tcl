######################################################################
#
# 01VersionControl.addin.tcl - Provides context menus for doing version
# control operations, like checkin and checkout of files in a project.
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,13aug98,cjs  added support for dependObjects 
# 01a,11aug98,axm  created from existing code 
# 
#
######################################################################

namespace eval ::VersionControl {

    proc menuMatch {type server handle} {
	if {[string match $type Object]} {
	    set objectType [::Object::evaluate $handle Type]
	    switch -exact $objectType {
		dependObject - 
		sourceFileObject {
		    # Source files
		    set filePath [::Object::evaluate $handle Name]
		    set fileName [file tail $filePath]
		    if {[info commands vcIsConfigured] == "vcIsConfigured"} {
			set vcNotCfg ![vcIsConfigured]
			::ViewBrowser::menuAdd "Check&out $fileName..." \
			    [list vcUIShow "checkout" $filePath] $vcNotCfg
			::ViewBrowser::menuAdd "Check&in $fileName..." \
			    [list vcUIShow "checkin" $filePath] $vcNotCfg
		    }
		}
		projectObject {
			# Projects
			set filePath [::Object::evaluate $handle Name]
			set projectName [file tail $filePath] 
               		if {[info commands vcIsConfigured] == \
				"vcIsConfigured"} {
				set vcNotCfg ![vcIsConfigured]
				::ViewBrowser::menuAdd "Check&out \
					$projectName..." \
					[list vcUIShow "checkout" $filePath] \
					$vcNotCfg
				::ViewBrowser::menuAdd \
					"Check&in $projectName..." \
					[list vcUIShow "checkin" $filePath] \
					$vcNotCfg
			}
		}
	    }
	}
    }

    proc OnEvent {cookie eventInfo} {
    }

    proc Init {} {
	::ViewBrowser::menuServerRegister ::VersionControl {sourceFileObject}
    }
}

::Workspace::addinRegister ::VersionControl
