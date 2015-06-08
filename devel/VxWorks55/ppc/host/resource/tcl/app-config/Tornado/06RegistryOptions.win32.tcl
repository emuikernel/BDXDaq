##########################################################
# 06RegistryOptions.win32.tcl - page for configuration of WIND_REGISTRY 
#
# Copyright 1995-98 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01g,21jun02,cmf  Add comment for SPR 76758
# 01f,02apr99,jak  fix bug on cancel
# 01e,17mar99,rbl  all choice controls in group must be explicitly initialized
#                  adding keyboard shortcuts
# 01d,11jan99,jak  rework logic 
# 01c,30nov98,jak  fix save problem
# 01b,23nov98,jak  change localHost to localhost
# 01a,02nov98,jak  written 
#
#
# DESCRIPTION
#
#

namespace eval RegistryOptions {
    variable m_parent 
    variable m_self 
    variable m_controls
	variable arrExes
	variable m_regVars

	set m_parent tornadoOptionsDlg

    set m_self(title) "Tornado Registry"
    set m_self(width) 300
    set m_self(height) 190
    set m_self(xpos) 5
    set m_self(ypos) 5


    if {[uitclinfo debugmode]} {
        set arrExes(regd) [wtxPath  host x86-win32 bin]wtxregd-d.exe
    } else {
        set arrExes(regd) [wtxPath  host x86-win32 bin]wtxregd.exe
    }


	#///////////////////////////////////
	# Options API implementation 

	#
	# onPageInit - initializes controls list, creates controls and hides them
	#	should be called every time the options dialog is created and shown
	#
    proc onPageInit {} {
		variable m_parent 
		variable m_controls 
    	variable m_regVars

    	set m_controls(names) [list \
			regOptIDC_STATIC_1 \
			regOptIDC_CHOICE_1 \
			regOptIDC_STATIC_2 \
			regOptIDC_CHOICE_2 \
			regOptIDC_STATIC_3 regOptIDC_EDIT_2 \
			regOptIDC_STATIC_4 \
        						] 

    	set m_controls(creationLists) [list \
			[list group -title "Registry Type" -name regOptIDC_STATIC_1  \
            -xpos [compX 7] -ypos [compY 15] -width 286 -height 78  \
			-hidden \
        	] \
            [list choice -title "&Local registry" -name regOptIDC_CHOICE_1 \
           -auto -newgroup \
			-callback {RegistryOptions::onChoice regOptIDC_CHOICE_1} \
           -xpos [compX 15] -ypos [compY 27] -width 77 -height 10  \
			-hidden \
           ] \
           [list  choice -title "&Remote registry" -name regOptIDC_CHOICE_2 \
			-auto \
			-callback {RegistryOptions::onChoice regOptIDC_CHOICE_2} \
           -xpos [compX 15] -ypos [compY 57] -width 83 -height 10  \
			-hidden \
           ] \
           [list  label -title "&Host name / IP address:" \
			-name regOptIDC_STATIC_3  \
           -xpos [compX 30] -ypos [compY 71] -width 80 -height 8  \
			-hidden \
           ] \
           [list  text -name regOptIDC_EDIT_2 \
           -xpos [compX 111] -ypos [compY 69] -width 95 -height 14  \
			-hidden \
           ] \
		   [list label \
			-title "(You may need to start a registry daemon or service manually on this host.)" \
			-name regOptIDC_STATIC_2 \
           	-xpos [compX 30] -ypos [compY 41] -width 250 -height 12  \
			-hidden \
		   ] \
           [list label -title "Note: Each Tornado Registry keeps track of \
			a set of available targets. Changing this value can affect tools \
            already attached to a target in this session.\n\n\
            You must restart Tornado for any changes to take effect\n\n\
            If you are using Symbol and Module Synchronization, the default setting of \
            Local registry is not sufficient.  You must check Remote registry and set to your \
            host name or IP address even if it is not remote (see the symSyncLib pages in the \
            VxWorks Reference Manual for more details)."\
            			-name regOptIDC_STATIC_4 \
           	-xpos [compX 15] -ypos [compY 100] -width 275 -height 80  \
			-hidden \
           ] \
									]

		foreach control $m_controls(creationLists) {
	    	controlCreate $m_parent $control
		}

        set m_regVars(host) [WIND_Env::WIND_REGISTRY_get] 

        if {$m_regVars(host) == "localhost"} {
			set m_regVars(type) localD
        } else {
			set m_regVars(type) remote 
        }
	
		# the page has been created. set the stage to init on entry:
		set m_regVars(secondInit) 0 
	}

	#
	# onPageInit_2 - a second init routine that gets called only
	#				if this page is clicked on, and not when all pages
	#				get initialized
	proc onPageInit_2 {} {
		global env
		variable m_parent 
    	variable m_regVars

        beginWaitCursor

		switch $m_regVars(type) { 
			localD {
            	controlCheckSet $m_parent.regOptIDC_CHOICE_2 0
            	controlCheckSet $m_parent.regOptIDC_CHOICE_1 1
			}
        	remote {
            	controlCheckSet $m_parent.regOptIDC_CHOICE_1 0
            	controlCheckSet $m_parent.regOptIDC_CHOICE_2 1
			}
        }

        endWaitCursor
    }

    proc onPageEnter {} {
		variable m_parent
		variable m_regVars

		controlsHide 0

		# this should get done only once:
		if {!$m_regVars(secondInit)} {
			onPageInit_2	
			set m_regVars(secondInit) 1
		}
    }

    proc queryPageExit {} {
		variable m_parent
		variable m_regVars

		# if nothing has been selected:
		if {![controlChecked $m_parent.regOptIDC_CHOICE_1] && \
			![controlChecked $m_parent.regOptIDC_CHOICE_2]} {
                messageBox "Please select a registry type"
                return 0
		} 
		
		if {$m_regVars(type) == "remote"} {
            set remoteHost [controlTextGet $m_parent.regOptIDC_EDIT_2]

			# validate the remote registry entries:
            set remoteHost [string trim $remoteHost]
            if {$remoteHost == ""} {
                messageBox "Remote host name/ IP address is required"
                return 0
            }

			# test the remote registry:
			set old_registry [WIND_Env::WIND_REGISTRY_get]
            if {[catch {WIND_Env::WIND_REGISTRY_set $remoteHost} err]} {
                messageBox "Unable to save value. cause: '$err'"
				return 0
            }
            beginWaitCursor
            set reachable [verifyReg]
            endWaitCursor
            catch {WIND_Env::WIND_REGISTRY_set $old_registry} err
            if {!$reachable} {
                messageBox "Remote registry specified is unreachable."
                return 0
            }

            set m_regVars(host) $remoteHost 
		}

		controlsHide 1
		return 1
    }

    proc onPageRestore {} {
    }

    proc onPageSave {} {
		variable m_parent
		variable m_regVars
		variable arrExes 

		if {$m_regVars(type) == "remote"} {
			set regHost $m_regVars(host)
		} 
		if {$m_regVars(type) == "localD"} {
			set regHost localhost
		}
		
        if {[catch {WIND_Env::WIND_REGISTRY_set $regHost} err]} {
            messageBox "Unable to save selection. cause: '$err'"
			return 0
        }
		
    }
	
	#
	# compX - computes absolute X position
	#
	proc compX {x} {
		variable m_self

		return [expr $m_self(xpos) + $x]
	}

	#
	# compY - computes absolute Y position
	#
	proc compY {y} {
		variable m_self

		return [expr $m_self(ypos) + $y]
	}

    proc controlsHide {bHide} {
		variable m_controls
		variable m_parent

		foreach controlName $m_controls(names) {
	    	controlHide $m_parent.$controlName $bHide
		}
    }

	#
	# registerPage - adds this page to options tabbed dlg
	#
    proc registerPage {} {
		variable m_self

		OptionsPageAdd "$m_self(title)" \
	    	RegistryOptions::onPageInit \
	    	RegistryOptions::onPageEnter \
	    	RegistryOptions::queryPageExit \
	    	RegistryOptions::onPageRestore \
	    	RegistryOptions::onPageSave
	    
    }

	#///////////////////////////////////
	# control callbacks

	proc onButton {bID} {
		variable m_parent
	}

	proc onChoice {cID} {
		variable m_parent
		variable m_regVars

		# don't do anything if called for an "uncheck" event
		if {![controlChecked $m_parent.$cID]} {
			return
		}

		switch $cID {
			regOptIDC_CHOICE_1 {
				set m_regVars(type) localD
            	if {![catch {WIND_Env::WIND_REGISTRY_set localhost} err]} {
				}
                controlEnable $m_parent.regOptIDC_EDIT_2 0
			}
			regOptIDC_CHOICE_2 {
				set m_regVars(type) remote 
                controlEnable $m_parent.regOptIDC_EDIT_2 1
				if {$m_regVars(host) != "localhost"} {
               		controlTextSet $m_parent.regOptIDC_EDIT_2 $m_regVars(host)
				}
			}
		}
	}

    proc verifyReg {} {
        if { [catch {wtxInfoQ .* tgtsvr} wtxResult]} {
            switch $wtxResult {
                REGISTRY_NAME_NOT_FOUND { ;# local registry, no tgtsvrs
                    return 1
                }
                API_REGISTRY_UNREACHABLE { ;# registry error
                    return 0
                }
                default { ;# unknown, may be an error
                    puts stderr "registry error: $wtxResult"
                    return 0
                }
            }
        } else {
            return 1
        }
    }
}

# Register this as an options page
RegistryOptions::registerPage
