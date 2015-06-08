##############################################################################
# app-config/Tornado/01SelectedTarget.win32.tcl - target-specific host code
#
# Copyright (C) 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01e,10dec01,rbl  fix problems starting and downloading to full simulator
# 01d,23mar99,jak  fix bug
# 01c,19mar99,jak  change sim kill calls
# 01b,02mar99,ren  Adding unload method.
# 01a,11nov98,cjs  fixed bug in existing simulator detection
# ??? 
# 
source [wtxPath host resource tcl app-config Tornado]Download.tcl

namespace eval SelectedTarget {

    proc targetGet {} {
		return [::selectedTargetGet]
    }

    proc targetSet {target} {
		::select_checkTgtSvr $target
    }

    proc download {cpu filepaths} {
		if {[lindex $filepaths 0] != ""} {
	    	::onDownloadFiles $filepaths $cpu
		}
    }

	proc unload {cpu filepaths} {
		::onUnloadFiles $filepaths $cpu
	}

	# True if simulator is up and its target server is current 
	proc isSimulatorActive {} {
		set retval 0
		set retval [::IDESimInterface::simIsActive]
		return $retval
	}

	proc simulatorKill {} {
		set retval "ok"
		set hSim [::IDESimInterface::simHandleGet]
		if {$hSim > 0} {
			if {[catch {::IDESimInterface::simKill $hSim} error]} {
				messageBox -ok -stopicon "Couldn't kill simulator: $error"
				set retval "cancel"
			}
		} else {
			# unable to get the handle
			messageBox -ok -stopicon \
				"Unable to obtain a handle to the simulator that is running. \
				\nPlease kill it before proceeding."
			set retval "cancel"
		}
		return $retval
	}

	proc isSimulatorTSActive {} {
		set retval 0
		set target [::SelectedTarget::targetGet]
		if {[string match "vxsim*" $target]} {
			set retval 1
		}
		return $retval
	}

	# Get the last launched, or failing that, default, path for the simulator
	proc defaultSimPathGet {hostType} {
		set path [::IDESimInterface::simPathGet]
		if {$path == ""} {
			switch $hostType {
				"x86-win32" {
					set path [wtxPath target config simpc]vxWorks.exe
				}
				default {
				}
			}
		}
		return $path
	}

	# (Re)start the simulator with the specified image
	proc simulatorStart {imagePath} {
		set retval "ok" 
		
		# If we have sim-lite only, and there is a simulator running,
                # tell user it must be killed
		if {![IDESimInterface::simIsFullInstalled] && \
                    [IDESimInterface::simFirstActivePnoGet] != -1  } {
			set simHandle [::IDESimInterface::simHandleGet]
			if {$simHandle > 0} { 
				# handle is available, offer to kill
				set retval [messageBox -okcancel -questionicon \
					"A simulator is already running. This must be killed \
					\nbefore proceeding. Click on Ok to continue."]
				if {$retval == "ok"} {
					if {[catch { \
						::IDESimInterface::simKill $simHandle} error]} {
						messageBox -ok -stopicon "Simulator couldn't be \
								killed: $error"
						set retval cancel
					}
					# Give simulator 2 seconds to die:
					beginWaitCursor
					set tEnd [clock seconds]
					incr tEnd 2 
					while {[clock seconds] != $tEnd} { }
					endWaitCursor
				}
			} else {
				# handle is not available, let them kill the sim themselves
				messageBox -ok \
					"A simulator is already running. Please exit that \
					\nsimulator and then start this one."
				set retval cancel
			}

			# Launch simulator
			if {[string match "ok" $retval]} {
				if {[catch {::IDESimInterface::simLaunch \
					$imagePath} error]} {
					messageBox -ok -stopicon "simulator launch failed: $error"
					set retval cancel
				}
			}
		} else {

			# Simulator is not running, and must be started from scratch
			if {[catch {::IDESimInterface::simLaunch \
				$imagePath} error]} {
				messageBox -ok -stopicon "Simulator launch failed: $error"
				set retval "cancel"
			}

		}

		return $retval
	}
}
