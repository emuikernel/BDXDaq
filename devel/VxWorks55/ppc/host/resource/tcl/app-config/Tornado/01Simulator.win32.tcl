###############################################################################
#
# app-config/Tornado/01Simulator.win32.tcl - support routines for 
#			 integrated simulator launch 
#
# Copyright (C) 1998-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01w,15apr02,fle  SPR#73266 : added note about this SPR in error messages
# 01v,30jan02,jmp  improved full simulator handling.
# 01u,21sep01,jmp  allow to start simulators from processor #0 to #15 (SPR
#                  #69985).
# 01t,20feb01,jco  added memory size option, for Personal JWorks 3.1 customers
# 01s,02apr99,jak  fix bug while launching from proj
# 01r,29mar99,jak  remove kbd shortcuts on OK and Cancel
# 01q,19mar99,jak  add help buttons
# 01p,17mar99,jak  make launch options appear only for full sim
# 01o,17mar99,rbl  added keyboard shortcuts.
#                  all choice controls in group must be explicitly initialized
# 01n,15mar99,jak  minor GUI cleanup
# 01m,04mar99,jak  add test and text for full simulator
# 01l,15feb99,jak  add support for processor number, use registry start from
#					01Launch.win32.tcl 
# 01k,08feb99,jak  add launch toolbar button 
# 01j,23nov98,jak  change localHost to localhost
# 01i,23nov98,jmp  changed Default simulator to Standard simulator.
# 01h,02nov98,jak  changed WIND_REGISTRY logic to read from Windows registry 
# 01g,13oct98,jak  resized dialogs and controls
# 01f,01oct98,jp   changed default Registry storage path
# 01e,25sep98,jak  reworked to add namespace and API for external use 
# 01d,18aug98,jak  reworked to remove random wait times, used simulatorLaunch API
# 01c,29jul98,j_k  changed references of simpc back to simnt
# 01b,28jul98,fle  changed reference to simnt into simpc
# 01a,19jun98,jak  created 
#

# DESCRIPTION
# 	This script contains the routines to launch the integrated simulator 
#


#///////////////////////////////////////////
# IDESimInterface - routines that interface the IDE with vxsim
#
namespace eval IDESimInterface {

	global env
	variable arrStates	;# states of image, tgtsvr and registry
	variable arrExes	;# executable names of image, tgtsvr
	variable arrNames	;# other values to be saved
	variable arrHandles ;# handles, like that of simulator last launched

	set arrHandles(vxsim) -1	
	set arrNames(pNo) 0
	set arrNames(pNoMax) 15	;# largest processor number 

	set arrExes(defaultImage) [wtxPath target config simpc]vxWorks.exe
									#(default image)
	if {[uitclinfo debugmode]} {
		set arrExes(tgtsvr) [wtxPath host x86-win32 bin]tgtsvr-d.exe
	} else {
		set arrExes(tgtsvr) [wtxPath host x86-win32 bin]tgtsvr.exe
	}

	set tsfsRoot c:\\  ;# default 
	if {[info exists env(TEMP)]} {
		set tsfsRoot $env(TEMP)
	} elseif {[info exists env(TMP)]} {
		set tsfsRoot $env(TMP)
	} elseif {[info exists env(HOME)]} {
		set tsfsRoot $env(HOME)
	} 
	set tsfsRoot [file join $tsfsRoot tsfs]
	set arrNames(tgtsvrDefIntOpts) "-V -B wdbpipe -R $tsfsRoot -RW"
	set arrNames(tgtsvrDefFullOpts) "-V -R $tsfsRoot -RW"
	set arrNames(tgtsvrDefOpts) $arrNames(tgtsvrDefIntOpts)
	set arrNames(simBaseAddr) "192.168.255"	;# simulator base address

	# to fix tgtsvr bug
	if {![info exists env(WIND_UID)]} {
		set env(WIND_UID) 0
	}


	proc resetVars {} {
		variable arrStates
		variable arrExes
		variable arrNames

		set arrStates(vxsim) 0
		set arrStates(registry) 0 	
		set arrStates(tgtsvr) 0 
		set arrNames(tgtSvrList) {}
		# default standard memory size of vxSim NT is 2 Mb
		set arrNames(stdDefMemSize)	2000000
		# default custom memory size of vxSim NT is 16 Mb
		set arrNames(cstDefMemSize)	16000000

	}


	#///////////////////////////////////////////
	# Image dialog - creation and callback procs
	#
	proc showImageDlg {imagePath pNo} {
		variable arrNames

		proc initImageDlg {imagePath pNo} {
			variable arrNames

			if {![simIsFullInstalled]} {
            	controlCreate IDD_DIALOG_sim \
					[list label -title "(Not configurable for VxSim-Lite)" \
                	-name IDC_STATIC8_sim  \
                	-xpos 105 -ypos 151 -width 110 -height 8  \
            		]
				controlEnable IDD_DIALOG_sim.IDC_EDIT2_sim 0
			}

			set imagePath [string trim $imagePath]
			if {(![simIsDefault $imagePath]) && ($imagePath!= "")} {	
				# it's a custom image
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE1_sim 0
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE2_sim 1
				set simExe [file nativename $imagePath]
				controlTextSet IDD_DIALOG_sim.IDC_EDIT1_sim $simExe
				controlTextSet IDD_DIALOG_sim.IDC_EDIT2_sim $pNo
				controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(cstDefMemSize)
				return
			}

			# if it's not a custom image, display the last selection
       		set imagePath [appRegistryEntryRead Simulator lastImageLaunched]
			if {($imagePath == "") || ([simIsDefault $imagePath])} { 
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE2_sim 0
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE1_sim 1
			      controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(stdDefMemSize)
				# choice callback also sets the imagepath
			} else {
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE1_sim 0
				controlCheckSet IDD_DIALOG_sim.IDC_CHOICE2_sim 1
				controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(cstDefMemSize)
				set simExe [file nativename $imagePath]
				controlTextSet IDD_DIALOG_sim.IDC_EDIT1_sim $simExe
			}

			controlTextSet IDD_DIALOG_sim.IDC_EDIT2_sim $pNo
		}

		proc onImageDlgChoice {choice} {
			variable arrExes
			variable arrNames

			set simExe vxWorks.exe
			if {$choice == "default"} {
				controlEnable IDD_DIALOG_sim.IDC_BUTTON1_sim 0
				set simExe $arrExes(defaultImage)
				controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(stdDefMemSize)
			} else {
				controlEnable IDD_DIALOG_sim.IDC_BUTTON1_sim 1
   				set simExe [lindex \
				[glob -nocomplain [wtxPath target proj]/*/default/vxWorks.exe] \
					0]
				controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(cstDefMemSize)
			}
			set simExe [file nativename $simExe]
			controlTextSet IDD_DIALOG_sim.IDC_EDIT1_sim $simExe 
			# note that this value can be overridden after the callback
		}

		proc onSimChoice {choice} {
		    variable arrNames

		    if {$choice == "int"} {
			# set integrated simulator options

			set launchOpts $arrNames(tgtsvrDefIntOpts)

		    } else {
		    	# set full simulator options

			# determine full simulator address according to the
			# selected processor number
		        set simExt [expr $arrNames(pNo) + 1]
			set simAddr "$arrNames(simBaseAddr).$simExt"

			# determine simulator name:
			# processor 0 --> vxsim
			# processor 1 --> vxsim1

			if {$arrNames(pNo) > 0} {
			    set simName "vxsim$arrNames(pNo)"
			} else {
			    set simName vxsim
			}
			set launchOpts "$arrNames(tgtsvrDefFullOpts) \
					$simAddr -n $simName"
		    }

		    # set target server options for selected simulator
		    controlTextSet IDD_DIALOG_tgtsvr.IDC_EDIT1_det $launchOpts 

		    # save simulator type
		    appRegistryEntryWrite Simulator simType $choice 
		}

		proc onImageDlgBrowse {} {
    		set fName [join [fileDialogCreate  \
					-filefilters \
					"Executable Files (*.exe)|*.exe| All Files (*.*)|*.*||" \
					-pathmustexist \
    	 			-filemustexist -title "Select VxWorks Image" \
					-okbuttontitle "&Select" ] \
						]
    		if {$fName != ""} { 
				controlTextSet IDD_DIALOG_sim.IDC_EDIT1_sim $fName
			}
		}

		proc onImageDlgPno {} {
			set value [controlTextGet IDD_DIALOG_sim.IDC_EDIT2_sim]
			if {![isNumeric $value]} {
				messageBox "Please enter a number"
				controlTextSet IDD_DIALOG_sim.IDC_EDIT2_sim 0
			}
		}

            proc onImageDlgMems {} {
			variable arrNames

			set value [controlTextGet IDD_DIALOG_sim.IDC_EDIT3_sim]
			if {![isNumeric $value]} {
				messageBox "Please enter a number"
				if {[controlChecked IDD_DIALOG_sim.IDC_CHOICE1_sim]} {
					controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(stdDefMemSize)
				} else {
					controlTextSet IDD_DIALOG_sim.IDC_EDIT3_sim $arrNames(cstDefMemSize)
				}
			}
		}

		proc onImageDlgOk {} {
			variable arrExes	
			variable arrStates
			variable arrNames

    		set simExe [controlTextGet IDD_DIALOG_sim.IDC_EDIT1_sim]
			# fwd slash and trim the simExe name
			set simExe [file join $simExe]
    		set simExe [string trim $simExe]

    		if {$simExe == ""} {
    			messageBox "Select a VxWorks image"
    			return
    		}

    		if {![file exists $simExe]} {
        		messageBox "Cannot locate image specified"
        		return
    		}

			set pNo [controlTextGet IDD_DIALOG_sim.IDC_EDIT2_sim]

			if {($pNo < 0) || \
				($pNo > $arrNames(pNoMax)) } {
        		messageBox \
					"Only processor numbers 0 - $arrNames(pNoMax) supported"
        		return
			}

			if {[simIsFullInstalled]} {
				set msgOnlyOne \
				"    A VxWorks simulator with processor number $pNo \n \
				is already running on this machine.\n \
				Click on OK to connect to this simulator or click on Cancel \n \
				to make another selection."
				set cancelAction "return"
			} else {
				set msgOnlyOne \
				"    A VxWorks simulator is already running on this machine.\n \
                Click on OK to connect to this simulator. \n     \
                   To launch a different simulator Cancel, exit the current\n \
                simulator and launch another simulator."
				set cancelAction "	windowClose IDD_DIALOG_sim\n \
									return"

			}

			# check for sim running with this processor number
			if {[::isSimulatorActive $pNo]} {
				puts stderr "SimLaunch: simulator $pNo already running"
				set ans [messageBox -okcancel $msgOnlyOne] 
				if {$ans == "ok"} { ;# try and connect to this simulator
					set arrStates(vxsim) 1
					set arrNames(pNo) $pNo
					windowClose IDD_DIALOG_sim
					return
				}  else {	;# back to the dialog, or close dialog, as reqrd
					eval $cancelAction
				}
			}

			set arrNames(pNo) $pNo

			set memSize [controlTextGet IDD_DIALOG_sim.IDC_EDIT3_sim]

			# no sim running, try to start the one selected:
			if {![IDESimInterface::runExe image $simExe $pNo $memSize]} { 
				return 
			}

			# the image name is trimmed and fwd slashed
       		appRegistryEntryWrite Simulator lastImageLaunched $simExe

			windowClose IDD_DIALOG_sim
		}

		proc onImageDlgCancel {} {
			windowClose IDD_DIALOG_sim
		}

		proc isNumeric {value} {
    		regexp {^[0-9]*} $value matching
    		if {![string match $value $matching]} {
    			return 0
    		} else {
    			return 1
    		}
		}

		set pNoMax $arrNames(pNoMax)
		set title $arrNames(simTitle)
		if {[simIsFullInstalled]} {
			set simDescr \
			"VxSim includes a fully-operational version of WindView. \
			WindView support for additional targets is available separately."
		} else {
			set simDescr \
			"The `VxSim-Lite' Simulator is installed, including a \
			fully-operational version of WindView for VxSim-Lite. A \
			complete version of VxSim which adds networking capabilities \
			is available separately, as is WindView support for additional \
			targets."
		}

		dialogCreate \
    		-name IDD_DIALOG_sim \
    		-title "$title Launch: Select Image and Options" \
    		-width 235 -height 213 \
    		-init {IDESimInterface::initImageDlg $imagePath $pNo} \
    		-controls [list \
        	{ button -title "OK" -name IDOK_sim \
            -xpos 70 -ypos 194 -width 50 -height 14  \
			-callback IDESimInterface::onImageDlgOk \
        	} \
        	{ button -title "Cancel" -name IDCANCEL_sim  \
            -xpos 124 -ypos 194 -width 50 -height 14  \
			-callback IDESimInterface::onImageDlgCancel \
        	}  \
        	{ button -title "&Help" -name IDHELP_sim  \
            -xpos 178 -ypos 194 -width 50 -height 14  \
			-helpbutton \
        	}  \
            { group -title "VxWorks Image" -name IDC_STATIC3_sim  \
            -xpos 7 -ypos 7 -width 221 -height 128  \
            }  \
            { frame \
				-black \
                -name IDC_STATIC7_sim  \
                -xpos 13 -ypos 18 -width 208 -height 48  \
            } \
            [list label \
				-title $simDescr \
                -name IDC_STATIC6_sim  \
                -xpos 17 -ypos 22 -width 200 -height 40  \
            ] \
            { label \
				-title "Select the executable VxWorks image to be launched:" \
                -name IDC_STATIC1_sim  \
                -xpos 11 -ypos 70 -width 180 -height 8  \
            } \
            { choice -title "&Standard simulator" -name IDC_CHOICE1_sim \
                -auto -newgroup \
                -callback "IDESimInterface::onImageDlgChoice default" \
                -xpos 11 -ypos 81 -width 70 -height 10 \
            } \
            { choice -title "C&ustom-built simulator" -name IDC_CHOICE2_sim \
                -callback "IDESimInterface::onImageDlgChoice custom" \
                -auto -xpos 11 -ypos 95 -width 85 -height 10  \
            } \
            { button -title "&Browse..." -name IDC_BUTTON1_sim  \
                -xpos 98 -ypos 94 -width 50 -height 14  \
				-callback IDESimInterface::onImageDlgBrowse \
            } \
            { label -title "&Image:" \
                -name IDC_STATIC2_sim  \
                -xpos 11 -ypos 117 -width 25 -height 8  \
            } \
            { text -name IDC_EDIT1_sim  \
                -xpos 40 -ypos 115 -width 180 -height 12 \
            } \
            { group -title "Launch Options" -name IDC_STATIC4_sim  \
            -xpos 7 -ypos 139 -width 221 -height 50 \
            } \
            [list label -title "&Processor no.: (0-${pNoMax})" \
                -name IDC_STATIC5_sim  \
                -xpos 11 -ypos 151 -width 70 -height 8  \
            ] \
            { text -name IDC_EDIT2_sim  \
                -xpos 80 -ypos 149 -width 20 -height 14 \
				-callback IDESimInterface::onImageDlgPno \
            } \
            [list label -title "Memory Size (bytes)" \
                -name IDC_STATIC5_sim  \
                -xpos 11 -ypos 172 -width 70 -height 8  \
            ] \
            { text -name IDC_EDIT3_sim  \
                -xpos 80 -ypos 170 -width 50 -height 14 \
				-callback IDESimInterface::onImageDlgMems \
            } \

				]	
	}


	#///////////////////////////////////////////
	# Registry Dialog - creation and callback procs
	#
	proc showRegDlg {} {
		variable arrNames

		proc initRegDlg {} {
			global env

			set windReg [WIND_Env::WIND_REGISTRY_get] 

			if {[string length [string trim $windReg]] == 0} { ;#default to local
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_remReg 0
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_locReg 1
			} elseif {$windReg == "localhost"} {
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_remReg 0
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_locReg 1
			} else {
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_locReg 0
				controlCheckSet IDD_DIALOG_reg.IDC_CHOICE_remReg 1
				controlTextSet IDD_DIALOG_reg.IDC_EDIT_regHost $windReg 
			}
		}

		proc onRegDlgChoice { choice } {
			variable arrNames	

			if {$choice == "local" } {
				controlEnable IDD_DIALOG_reg.IDC_EDIT_regHost 0
			} else {
				controlEnable IDD_DIALOG_reg.IDC_EDIT_regHost 1
			}
		}

		proc onRegDlgOk {} {
			global env
			variable arrExes 
			variable arrStates

			set local [controlChecked IDD_DIALOG_reg.IDC_CHOICE_locReg]
			set remote [controlChecked IDD_DIALOG_reg.IDC_CHOICE_remReg]

			if {$local} {
				if {![IDESimInterface::runExe registry]} {
					return
				}
			} 

			if {$remote} {
				set remoteHost [controlTextGet IDD_DIALOG_reg.IDC_EDIT_regHost] 
				set remoteHost [string trim $remoteHost]
				if {$remoteHost == ""} {
					messageBox "Enter remote host name / IP address"
					return
				} 
				if {[catch {WIND_Env::WIND_REGISTRY_set $remoteHost} err]} {
					messageBox "Unable to save selection. cause: '$err'"
				}
				beginWaitCursor
				set reachable [verifyReg]
				endWaitCursor
				if {!$reachable} {
					messageBox "Remote registry specified is unreachable."
					return
				}
			} 

			set arrStates(registry)  1	
			windowClose IDD_DIALOG_reg
		}

		proc onRegDlgCancel {} {
			variable arrStates	

			set arrStates(registry) 0
			windowClose IDD_DIALOG_reg
		}

		set title $arrNames(simTitle)
    	dialogCreate \
        	-name IDD_DIALOG_reg \
        	-title "$title Launch: Locate Tornado Registry" \
        	-width 235 -height 104 \
        	-init IDESimInterface::initRegDlg \
        	-controls { \
            { button -title "OK" -name IDOK_reg \
            -xpos 70 -ypos 85 -width 50 -height 14  \
            -callback IDESimInterface::onRegDlgOk \
            }
            { button -title "Cancel" -name IDCANCEL_reg  \
            -xpos 124 -ypos 85 -width 50 -height 14  \
            -callback IDESimInterface::onRegDlgCancel \
            }
        	{ button -title "&Help" -name IDHELP_reg  \
            -xpos 178 -ypos 85 -width 50 -height 14  \
			-helpbutton \
			}
            { group -title "Tornado Registry" -name IDC_STATIC  \
            -xpos 7 -ypos 7 -width 221 -height 74  \
            }
            { label -title "A Tornado Registry is required \
                        but not found in your environment." \
            -xpos 11 -ypos 18 -width 210 -height 8  \
            }
            { choice -title "Start &local registry" -name IDC_CHOICE_locReg  \
            -auto -newgroup -callback {IDESimInterface::onRegDlgChoice local} \
            -xpos 11 -ypos 31 -width 77 -height 10  \
            }
            { choice -title "Use &remote registry" -name IDC_CHOICE_remReg  \
            -auto -callback {IDESimInterface::onRegDlgChoice remote} \
            -xpos 11 -ypos 48 -width 83 -height 10  \
            }
            { label -title "H&ost name / IP address" -name IDC_STATIC  \
            -xpos 12 -ypos 60 -width 80 -height 8  \
            }
            { text -name IDC_EDIT_regHost  \
            -xpos 93 -ypos 58 -width 95 -height 14  \
            }
        }
	}



	#///////////////////////////////////////////
	# Target Server Dialog - creation and callback procs
	#
	proc showTSDlg {} {
		variable arrNames

		proc initTSDlg {} {
			variable arrNames
    		controlCreate IDD_DIALOG_tgtsvr [list label \
        		-title "A Target Server named \"$arrNames(tgtsvr)\" is required \
                        and will be started." \
        	-xpos 13 -ypos 18 -width 210 -height 25  \
                                    ]
    		controlCreate IDD_DIALOG_tgtsvr [list button \
        		-title "&Details >>" -name IDC_BUTTON_det  \
            	-xpos 160 -ypos 41 -width 50 -height 14  \
       								] 
    		controlCallbackSet IDD_DIALOG_tgtsvr.IDC_BUTTON_det \
        		"IDESimInterface::onTSDlgDetails expand $arrNames(tgtsvr)"

			# hidden controls:
    		controlCreate IDD_DIALOG_tgtsvr [list frame \
				-name IDC_FRAME_det \
				-hidden \
            	-xpos 13 -ypos 60 -width 210 -height 1 \
									]
    		controlCreate IDD_DIALOG_tgtsvr [list label \
				-name IDC_STATIC1_det \
				-hidden \
            	-xpos 13 -ypos 64 -width 100 -height 12 \
				-title "&Target server launch options:" \
									]
    		controlCreate IDD_DIALOG_tgtsvr [list text \
				-name IDC_EDIT1_det \
				-hidden \
            	-xpos 13 -ypos 78 -width 210 -height 28 \
				-multiline -vscroll \
									]
		controlCreate IDD_DIALOG_tgtsvr [list choice \
				-newgroup -auto -title "Integrated simulator" \
				-name IDC_CHOICE_intSim \
				-xpos 13 -ypos 112 -width 210 -height 14 \
				-callback "IDESimInterface::onSimChoice int" ]

		controlCreate IDD_DIALOG_tgtsvr [list choice \
				-auto -title "Full simulator" \
				-name IDC_CHOICE_fullSim \
				-xpos 13 -ypos 127 -width 210 -height 14 \
				-callback "IDESimInterface::onSimChoice full" ]

    		controlCreate IDD_DIALOG_tgtsvr [list button \
				-name IDC_BUTTON2_det \
				-hidden \
				-xpos 13 -ypos 147 -width 65 -height 14 \
				-title "&Restore Defaults" \
				-callback "IDESimInterface::onTSDlgDetailsRestore"
									]
		
			# set launch options with registry value or default value
			# even if the box is not visible, it needs to be set
			set launchOpts [appRegistryEntryRead Simulator tgtsvrOptions]
			if {$launchOpts== ""} {
			    set launchOpts $arrNames(tgtsvrDefOpts) 
			}

			controlTextSet IDD_DIALOG_tgtsvr.IDC_EDIT1_det $launchOpts 
			
			# get type of last simulator started
			set simType [appRegistryEntryRead Simulator simType]

			# set simulator type selection according to last
			# simulator started

			if {$simType == "full"} {
			    controlCheckSet \
			    	IDD_DIALOG_tgtsvr.IDC_CHOICE_fullSim 1
			} else {
			    controlCheckSet \
			    	IDD_DIALOG_tgtsvr.IDC_CHOICE_intSim 1
			}
		}

		proc onTSDlgDetails {expandOrShrink name} {
			switch $expandOrShrink {
				expand {
					set expandOrShrink shrink
					set buttonText "<< &Details"
					set bHidden 0 
				}
				shrink {
					set expandOrShrink expand 
					set buttonText "&Details >>"
					set bHidden 1 
				}
			}
			# reset the callback and text for the details button:
			controlTextSet IDD_DIALOG_tgtsvr.IDC_BUTTON_det $buttonText
    		controlCallbackSet IDD_DIALOG_tgtsvr.IDC_BUTTON_det \
        		"IDESimInterface::onTSDlgDetails $expandOrShrink $name"

			showTSDlgDetailsCtrls $bHidden
		}

		proc showTSDlgDetailsCtrls {bHidden} {
			# increase or decrease size of dlg & group
			# and move OK and Cancel buttons
			if {$bHidden} {
				windowSizeSet IDD_DIALOG_tgtsvr 237 92
				controlSizeSet IDD_DIALOG_tgtsvr.IDC_GROUP_1 223 62
				controlPositionSet IDD_DIALOG_tgtsvr.IDOK_tgtSvr 72 74
				controlPositionSet IDD_DIALOG_tgtsvr.IDCANCEL_tgtSvr 126 74
				controlPositionSet IDD_DIALOG_tgtsvr.IDHELP_tgtSvr 180 74
			} else {
				windowSizeSet IDD_DIALOG_tgtsvr 237 195 
				controlSizeSet IDD_DIALOG_tgtsvr.IDC_GROUP_1 223 165 
				controlPositionSet IDD_DIALOG_tgtsvr.IDOK_tgtSvr 72 177 
				controlPositionSet IDD_DIALOG_tgtsvr.IDCANCEL_tgtSvr 126 177
				controlPositionSet IDD_DIALOG_tgtsvr.IDHELP_tgtSvr 180 177
			}

			# hide or show widgets
			set widgetList { IDC_FRAME_det \
							IDC_STATIC1_det IDC_BUTTON2_det \
							IDC_EDIT1_det }
			foreach widget $widgetList {
				controlHide IDD_DIALOG_tgtsvr.$widget $bHidden
			} 
		}

		proc onTSDlgDetailsRestore {} {
			variable arrNames

			# is full simulator selected ?

			set fullSim [controlChecked \
					IDD_DIALOG_tgtsvr.IDC_CHOICE_fullSim]

			if {$fullSim == 1} {
			    # set full simulator options

			    # determine full simulator address according to the
			    # selected processor number
			    set simExt [expr $arrNames(pNo) + 1]
			    set simAddr "$arrNames(simBaseAddr).$simExt"

			    # determine simulator name:
			    # processor 0 --> vxsim
			    # processor 1 --> vxsim1

			    if {$arrNames(pNo) > 0} {
				set simName "vxsim$arrNames(pNo)"
			    } else {
				set simName vxsim
			    }
			    set launchOpts "$arrNames(tgtsvrDefFullOpts) \
					    $simAddr -n $simName"
			    controlTextSet IDD_DIALOG_tgtsvr.IDC_EDIT1_det \
						$launchOpts
			} else {
			    controlTextSet IDD_DIALOG_tgtsvr.IDC_EDIT1_det \
						$arrNames(tgtsvrDefIntOpts)
			}
		}

		proc onTSDlgOk {} {
			# collect the launch options from the details box 
			# and save them for next time
			set tsLaunchOpts \
				[controlTextGet IDD_DIALOG_tgtsvr.IDC_EDIT1_det]
       		appRegistryEntryWrite Simulator tgtsvrOptions $tsLaunchOpts 

			# launch tgtsvr and stay in the dialog if there's an error 
			if {![IDESimInterface::runExe tgtsvr $tsLaunchOpts]} {
				return
			}
    		windowClose IDD_DIALOG_tgtsvr
			tsInitCheck
		}

		proc onTSDlgCancel {} {
    		windowClose IDD_DIALOG_tgtsvr
		}

		set title $arrNames(simTitle)
    	dialogCreate \
    	-name IDD_DIALOG_tgtsvr \
    	-title "$title Launch: Launch Target Server" \
    	-width 237 -height 92 \
    	-init IDESimInterface::initTSDlg \
    	-controls { \
        { button -title "OK" -name IDOK_tgtSvr \
            -callback IDESimInterface::onTSDlgOk \
            -xpos 72 -ypos 74 -width 50 -height 14  \
        }
        { button -title "Cancel" -name IDCANCEL_tgtSvr  \
            -xpos  126 -ypos 74 -width 50 -height 14  \
            -callback IDESimInterface::onTSDlgCancel \
        }
        { button -title "&Help" -name IDHELP_tgtSvr  \
            -xpos 180 -ypos 74 -width 50 -height 14  \
			-helpbutton \
		}
        { group -title "Target Server" -name IDC_GROUP_1  \
            -xpos 7 -ypos 7 -width 223 -height 62  \
        }
    		}

	}


	#  
	# runExe - gateway to external commands
	# 
	proc runExe {type {arg1 ""} {arg2 ""} {arg3 ""}} {
		variable arrExes 
		variable arrStates
		variable arrHandles
		variable arrNames 

		switch $type {

			image {
				beginWaitCursor	
				set imageName $arg1
				set pNo $arg2
				set size $arg3
				set workingDir [file dirname $imageName] 
				puts "SimLaunch: $imageName, processor $pNo"
# JCO: Here under, the size of the VxWorks is introduced as a kind of hack: size has been forgotten at first
# when designing the interface and since simulatorLaunch is implemented as cpp code to avoid patching 
# the simulator we abuse the imageName field (which is not checked in the cpp code), to insert 
# the size argument.
				if {[catch { \
					set arrHandles(vxsim) \
						[::simulatorLaunch "$imageName /r$size" $pNo $workingDir 30000] \
				} err ]} {
					endWaitCursor	
					puts stderr $err
					messageBox $err
					set arrStates(vxsim) 0
					return 0
				}
 
				set arrStates(vxsim) 1
				endWaitCursor	
				return 1
			}

			registry {
				# proc defined in 01Launch.win32.tcl:
				return [::localRegistryStart]
			}

			tgtsvr {
				# first the executable name and the tgt name
				set cmdLine "$arrExes(tgtsvr) $arrNames(tgt)"
				# now the options
				append cmdLine " $arg1" 
				puts stderr "Command: $cmdLine"
				set workingDir [wtxPath host x86-win32]bin\\

    			# launch external process with these options:
    			# capture stdin, capture stdout, capture stderr,
    			# prompt for args, close window on exit,
    			# run as mdi child
    			if {[catch {::toolLaunch "Target Server: $cmdLine" \
                        $cmdLine $workingDir \
                        0 0 0 \
                        0 0 \
                        0} err]} {
                	puts "\nerror: '$err'\n"
					return 0
    			}
				return 1
			}
		}
	}


    proc checkSimLite {imagePath} {
        variable arrStates

        if {[::isSimulatorActive]} {
            puts stderr "SimLaunch: VxSim-Lite already running"
            set ans [messageBox -okcancel \
                "    A VxWorks simulator is already running on this machine.\n \
                Click on OK to connect to this simulator. \n     \
                    To launch a different simulator Cancel, exit the current\n \
                simulator and launch another simulator."]
            if {$ans == "ok"} {
                set arrStates(vxsim) 1
            } else {
                set arrStates(vxsim) 0
            }
        } else {
            showImageDlg $imagePath 0
        }
    }


	# Hacked to implement a modeless dialog with a modal
	# - in initproc, autotimer is set to call checkReg_2 right after 
	# UI shows(100ms)
	# - in exitproc, autotimer is set to none
	proc checkReg {} {
		variable arrStates
		dialogCreate -name IDD_DIALOG_search  -notitle -width 145 -height 17 \
				-noframe \
				-controls { \
					{label -title "Locating Registry and Target Server ..." \
					-name search_lbl1 -xpos 3 -ypos 3 -width 140 -height 14 } \
			 		} \
				-init { \
			autoTimerCallbackSet IDD_DIALOG_search \
				-milliseconds 100 IDESimInterface::checkReg_2 \
					} \
                -exit "autoTimerCallbackSet  IDD_DIALOG_search 0 {}"
		if {!$arrStates(registry)} {
			showRegDlg
		}
	}

	proc checkReg_2 {} {
		variable arrStates

		beginWaitCursor	
		set arrStates(registry) [verifyReg]
		endWaitCursor
		windowClose IDD_DIALOG_search
	}


	proc verifyReg {} {
		variable arrNames
    	if {! [catch {wtxInfoQ .* tgtsvr} wtxResult]} {
			set arrNames(tgtSvrList) {}
        	foreach targetServer $wtxResult {
            	lappend arrNames(tgtSvrList) [lindex $targetServer 0]
        	}
    	} else {
			# local registry, no tgtsvrs
			if {$wtxResult == "REGISTRY_NAME_NOT_FOUND"} {
				return 1
			}
			if {$wtxResult == "API_REGISTRY_UNREACHABLE"} {
				return 0
			}
			puts stderr "registry error : $wtxResult"
		}
		return 1
	}

	proc checkTS {pNo} {
		variable arrNames
		variable arrStates

		# set the name for the target
		if {$pNo > 0} {
			set arrNames(tgt) vxsim${pNo}
		} else {
			set arrNames(tgt) vxsim
		}

		# name for the target server
		set arrNames(tgtsvr) ""
		append arrNames(tgtsvr) $arrNames(tgt) @ [info hostname]

		if {[lsearch -exact $arrNames(tgtSvrList) $arrNames(tgtsvr)] < 0} {
			showTSDlg
		} else {
			set arrStates(tgtsvr) 1
		}
	}

	# Hacked to implement a modeless dialog with a modal
	proc tsInitCheck {} {
		dialogCreate -name IDD_DIALOG_TSInit  -notitle -width 100 -height 17 \
				-noframe \
				-controls { \
					{label -title "Launching Target Server ..." \
					-name TS_lbl1 -xpos 3 -ypos 3 -width 95 -height 14 } \
			 		} \
				-init { \
					autoTimerCallbackSet IDD_DIALOG_TSInit \
						-milliseconds 100 IDESimInterface::tsInitCheck_2 \
					} \
                -exit "autoTimerCallbackSet  IDD_DIALOG_TSInit 0 {}"
	}

    ############################################################################
    #
    # IDESimInterface::tsInitCheck_2 - check target server initialisation
    #
    # SYNOPSIS
    # \ts
    #   tsInitCheck_2
    # \te
    #
    # This procedure keeps on trying to attach to the target server, with a
    # timeout of 30 seconds
    #

    proc tsInitCheck_2 {} {
	variable arrNames
	variable arrStates

	set timeout 30 
	set startTime [clock seconds]
	beginWaitCursor	

	while {[catch {set hWTX [wtxToolAttach $arrNames(tgtsvr) SimLaunch] } \
		      err]} {

	    if {[string match {*API_TOOL_DISCONNECTED*} $err]} {
		endWaitCursor
		windowClose IDD_DIALOG_TSInit
		set arrStates(tgtsvr) 0
		puts stderr "SimLaunch: target server launch failed"
		messageBox "Target server launch failed"
		return
	    }

	    set curTime [clock seconds]
	    if {[expr $curTime - $startTime] >= $timeout} {
		endWaitCursor
		windowClose IDD_DIALOG_TSInit
		set arrStates(tgtsvr) 0
		puts stderr "SimLaunch: timed out waiting for target server"
		messageBox "Target Server \"$arrNames(tgtsvr)\" is not\
			    responding.\nThis may be due to slow network\
			    response.\n\nNOTE : If the problem persists, please\
			    see the Tornado Migration Guide,\
			    \n            section \"Tornado Registry\""
		return
	    } 

	    # let's wait for one second before calling again wtxToolAttach

	    after 1000
	}

	wtxToolDetach $hWTX 
	endWaitCursor
	windowClose IDD_DIALOG_TSInit
	set arrStates(tgtsvr) 1
    }


	#///////////////////////////////////////////
	# API - procs for safe external use 
	#

	#
	# simIsActive - returns whether an active simulator can be detected
	# args
	#	pNo - processor number
	#
	# return - 1 if yes, 0 if not
	#
	proc simIsActive {{pNo 0}} {
		return [::isSimulatorActive $pNo]
	}

	#
	# simLaunch - launches the simulator with these steps:
	#			1.displays a dialog for selecting image and processor number
	#			2.checks for the registry and starts if reqrd
	#			3.checks for a tgtsvr for the simulator and starts if reqrd
	#			4.selects the tgtsvr in the selection box
	#
	# args 
	#	 imagePath  - image to launch instead of step 1
	#	 pNo		- processor number
	#
	proc simLaunch {{imagePath ""} {pNo 0}} {
		variable arrStates
		variable arrNames
		variable arrExes

		resetVars	;# so that all checks are made again

		if {[simIsFullInstalled]} {
			# for full-sim, bring up the dialog 
			# for selection of image and processor

			# get an unused processor number
			set pNo [simFirstInactivePnoGet]
			if {$pNo == -1} {
			    set pNo 0
			}
			showImageDlg $imagePath $pNo
			# note: this sets state of vxsim and sets chosen pNo
		} else {
			# for vxsim-lite, check if simulator is running first
			checkSimLite $imagePath
		}
		if {!$arrStates(vxsim)} { return }

		# check on the registry 
		checkReg ;# sets state of registry
		if {!$arrStates(registry)} { return }	

		# check on the tgt svr & start the tgtsvr if reqrd
		checkTS	$arrNames(pNo) ;# sets state of tgtsvr
		if {!$arrStates(tgtsvr)} { return }

		# attach to the tgtsvr 
		::select_checkTgtSvr $arrNames(tgtsvr) 

	}

	#
	# simHandleGet - returns the handle to the active simulator
	#
	# return - handle, -1 if no active simulator 
	#
	proc simHandleGet {} {
		variable arrHandles

		return $arrHandles(vxsim) 
	}

	#
	# simPathGet - returns the path of the last simulator launched from the IDE
	#
	# return - path of the simulator, empty string if unknown
	#
	proc simPathGet {} {
       	set simExe [appRegistryEntryRead Simulator lastImageLaunched]
		if {$simExe == ""} {
			return ""
		}
		return [file nativename $simExe] 
	}	

	#
	# simKill - kills a simulator identified by a handle 
	#
	# arg - handle of the simulator to kill
	# return - success of kill 
	#
	proc simKill { handle } {
		variable arrHandles

		if {[catch { ::simulatorKill $handle } err]} {
			return 0
		} else {
			if {$handle == $arrHandles(vxsim)} {
				set arrHandles(vxsim) -1
			}
			return 1
		}
	}

	#
	# simFirstActivePnoGet - returns the processor number of the first active 
	#						simulator found
	# return - processor number of the first active simulator, 
	#			-1 if none is active 
	#
	proc simFirstActivePnoGet {} {
		variable arrNames

		for {set pNo 0} {$pNo <= $arrNames(pNoMax)} {incr pNo} {
			if {[::isSimulatorActive $pNo]} {
				return $pNo
			}
		}

		# none were active
		return -1
	}	

	#
	# simFirstInactivePnoGet - returns the processor number of the first
	# inactive simulator found
	# return - processor number of the first inactive simulator, 
	#			-1 if none is inactive 
	#
	proc simFirstInactivePnoGet {} {
		variable arrNames

		for {set pNo 0} {$pNo <= $arrNames(pNoMax)} {incr pNo} {
			if {![::isSimulatorActive $pNo]} {
				return $pNo
			}
		}

		# none were inactive
		return -1
	}	

	#
	# simIsFullInstalled - returns whether the full simulator has been installed 
	#
	# return - 1 if yes, 0 if not
	#
	proc simIsFullInstalled {} {
		return [file exists "[wtxPath target config simpc]fullSimInstalled.flag"]
	}

	# now that proc has been defined, check if vxsim full has been installed
	if {[simIsFullInstalled]} {
		set arrNames(simTitle) VxSim
	} else {
		set arrNames(simTitle) VxSim-Lite
	}

	#
	# simIsDefault - returns whether an image is the default simulator 
	#
	# return - 1 if yes, 0 if not
	#
	proc simIsDefault {imagePath} {
			variable arrExes

			set defSimExe [file nativename $arrExes(defaultImage)]
			set defSimExe [string toupper $defSimExe]
			set simExe [file nativename $imagePath]
			set simExe [string trim [ string toupper $simExe]]
			if {$simExe == $defSimExe} {
				return 1
			} else {
				return 0
			}
	}
}


#///////////////////////////////////////////
# MENU INTEGRATION 
#
menuItemInsert \
	-callback {IDESimInterface::simLaunch} \
	-after -bypath \
	{&Tools &Debugger...} {S&imulator...}


#///////////////////////////////////////////
# TOOLBAR INTEGRATION 
#
controlCreate launch \
    [list toolbarbutton \
			-name simulator \
			-callback {IDESimInterface::simLaunch} \
			-tooltip "Launch Simulator" \
        -bitmap "[wtxPath host resource bitmaps Launch controls]Simulator.bmp"]

