##############################################################################
# app-config/Tornado/BuildMenu.tcl - backwards compatibility from
# salvaged from 01Project.win32.tcl of that Tornado 1.0.1 era
#
# Copyright (C) 1998-1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01l,27jun02,cjs  complete fix for spr 73599
# 01k,29may02,cjs  Add support for picking toolchain
# 01j,17may02,cjs  Fix spr 76336: build menu doesn't display BSPs containing
#                  the word 'all'
# 01i,08may02,cjs  Fix spr 74683: adding .bin targets to build menu; also, spr
#                  73564 (warning about not all builds being valid for all
#                  bsps)
# 01h,23mar99,jak  add note to dialog
# 01g,15mar99,cjtc Extended use of the new build menu to include the Standard
#		   BSP Builds menu selection. Extension to SPR #25459
# 01f,12mar99,jak  fix help button
# 01e,11mar99,cjtc Replaced the old Build Boot Roms popup menu tree with a new
#		   dialog box invoked from Build->Build Boot ROM. SPR #25459
# 01d,26feb99,cjs  Expose bootrom targets always
# 01c,23nov98,cjs  Remove extra brace causing obscure bug
# 01b,20nov98,cjs  remove debug puts statement
# 01a,30oct98,cjs  salvaged from 01Project.win32.tcl in a T 1.0.1 view 
# 

namespace eval ::OldBuildMenu {

	variable buildMenuText {S&tandard BSP Builds...}
	variable _bootromBuildMenuText {Build B&oot ROM...} 
	variable _bspList ""
	
	proc bspListGet {} {
		variable _bspList

		if {$_bspList == ""} {
			# Collect the list of installed BSPs.
			set bspList [appRegistryEntryRead -default "" \
                ProjectMenu IncludeBSPs]
			if {$bspList == ""} {  
                set bspList [bspsEnumerate]
            }

            # truncate them so just name displays, and sort
            foreach path $bspList {
                lappend _bspList [file tail $path]
            }
			set _bspList [lsort $_bspList]
		}
		return $_bspList
	}

	proc bootromBuildMenuAdd {insertionPoint} {
		variable _bootromBuildMenuText

		set buildMenu \
		    [menuItemInsert -bypath \
		    -callback \
		    	{::OldBuildMenu::onBuild "Build Boot ROM" "bootRom"} \
		    $insertionPoint $_bootromBuildMenuText]

			
		# Finally add a menu separator after everything that is created above.
		menuItemInsert -separator \
			-bypath [list [lindex $insertionPoint 0] $_bootromBuildMenuText]
	}

	proc oldBuildMenuAdd {insertionPoint} {
		variable buildMenuText

		set buildMenu \
		    [menuItemInsert -bypath \
		    -callback \
		    	{::OldBuildMenu::onBuild "Build VxWorks" "vxWorks"} \
		    $insertionPoint $buildMenuText]

		# Finally add a menu separator after everything that is created above.
		menuItemInsert -separator \
			-bypath [list [lindex $insertionPoint 0] $buildMenuText]
	}
	
	# XXX -- fix me and use me
	proc oldBuildMenuDelete {insertionPoint} {
		variable buildMenuText
			[lindex $insertionPoint 0] $buildMenuText
		menuItemDelete -bypath -string [list \
			[lindex $insertionPoint 0] $buildMenuText]
	}

	proc subMenuInsert {bsp insertionMenu subMenuTitle targets} {
		set subMenu [menuItemAppend $insertionMenu -popup $subMenuTitle]
		foreach target $targets {
			set prompt [format "Build %s" $target]
			menuItemAppend $subMenu \
				-callback [list ::OldBuildMenu::launchBspBuild \
					$bsp $target] $target
		}
	}

	#
	# launchBspBuild - The eventual callback for any of the BSP menu
	# targets created on the Build menu.  This callback launches the
	# build and checks for errors.
	#
	proc launchBspBuild {args} {
		set bsp [lindex $args 0]
		set makeTarget [lindex $args 1]
                if {[llength $args] > 2} {
                    set makeTarget "$makeTarget TOOL=[lindex $args 2]" 
                }

		if {[catch {wrapper_buildLaunch "Build $bsp $makeTarget" \
			$makeTarget [wtxPath target config $bsp]} error]} {
			messageBox -excl "Unable to launch build of $bsp\
				'$makeTarget'. Cause: $error."
		}
	}




    #
    # buildDlgInit
    #
    # callback procedure called when the 
    # Build dialog is initialised.
    #

    proc buildDlgInit {imageType} {
    
	beginWaitCursor

	# Enumerate the BSPs

	set bspList [bspListGet]

	if {$imageType == "bootRom"} {

	    # Get rid of the simulators (they can't have bootroms)
	    
	    set badBspList {simpc solaris hpux}

	    # Declare the "Boot ROM" targets.
	
	    set buildImages {
	        bootrom
		    bootrom.hex
		    bootrom.bin
	        bootrom_uncmp
		    bootrom_uncmp.hex
		    bootrom_uncmp.bin
	        bootrom_res
			bootrom_res.hex
			bootrom_res.bin
		        clean	
	    }
	} else {
	
	    # We are a building vxWorks image

	    set badBspList ""

	    # Declare VxWorks images
	    
	    set buildImages {
	    	vxWorks
		vxWorks.st 
		vxWorks_rom.hex
		vxWorks.res_rom.hex 
		vxWorks.st_rom.hex
		vxWorks.res_rom_nosym.hex
	        clean
	    }
	}

	controlValuesSet buildDlg.buildCombo $buildImages
        set build [appRegistryEntryRead -default [lindex $buildImages 0] \
            Workspace DefaultBspBuildTarget]

        if {[lsearch $buildImages $build] == -1} {
            set build [lindex $buildImages 0]
        }

        controlSelectionSet buildDlg.buildCombo -string $build

	# Exclude bsps which cannot be built
	
	foreach badBsp $badBspList {
	    set i [lsearch $bspList $badBsp]
	    if {$i != -1} {
		set bspList [lreplace $bspList $i $i]
	    }
	}

	controlValuesSet buildDlg.bspCombo $bspList
        set bspInfo [defaultBasisBspGet [wtxHostType]]
        set bsp [file tail [lindex $bspInfo 0]]
        if {[lsearch $bspList $bsp] == -1} {
            set bsp [lindex $bspList 0]
        }
	controlSelectionSet buildDlg.bspCombo -string $bsp 

        # Setting the bsp populated the tool list
        set tc [lindex $bspInfo 1]
        set toolchainList [controlValuesGet buildDlg.toolchainCombo]
        if {[lsearch $toolchainList $tc] == -1} {
            set tc [lindex $toolchainList 0]
        }
	controlSelectionSet buildDlg.toolchainCombo -string $tc
	endWaitCursor

    }

    #
    #
    #
    proc onBspSelect {} {
        set bsp [controlSelectionGet buildDlg.bspCombo -string]
        if {$bsp != ""} {
            set bsp [wtxPath target config $bsp]
            set toolchainInfo [getToolChainInfoForBsp $bsp]
            set toolchainList [lindex $toolchainInfo 0]
            set defaultTool [lindex $toolchainInfo 1]
            controlValuesSet buildDlg.toolchainCombo $toolchainList
            controlSelectionSet buildDlg.toolchainCombo -string $defaultTool
        }
    }

    #
    # onBuildOK
    #
    # callback procedure called when the user his OK in the
    # Build dialog
    #

    proc onBuildOK {} {
	
	set bspSelected [controlSelectionGet buildDlg.bspCombo -string]
	set buildSelected [controlSelectionGet buildDlg.buildCombo -string]
	set toolchainSelected \
            [controlSelectionGet buildDlg.toolchainCombo -string]

	if {[string length $bspSelected] == 0  ||
	    [string length $buildSelected] == 0 } {
	    messageBox "Please select a BSP and an Image to build"
	} else {
            defaultBasisBspSet [wtxPath target config $bspSelected] \
                $toolchainSelected
            appRegistryEntryWrite Workspace DefaultBspBuildTarget $buildSelected
	    windowClose buildDlg
	    launchBspBuild $bspSelected $buildSelected $toolchainSelected
	}
    }
	
    #
    # onBuild
    #
    # callback procedure to create the Build dialog box
    #

    proc onBuild {title imageType} {

	dialogCreate \
	    -name buildDlg -title $title \
	    -width 222 -height 116 \
	    -init {::OldBuildMenu::buildDlgInit $imageType} \
	    -controls \
		[list \
    		    [list label -name bspListLbl \
		        -title "Select a BSP:" \
			-xpos 7 -ypos 7 -width 85 -height 9] \
		    [list combo -name bspCombo \
			    -xpos 7 -ypos 18 -width 100 -height 70 \
			    -callback onBspSelect] \
		    [list label -name buildsComboLbl \
		        -title "Select an Image to build:" \
			-xpos 115 -ypos 7 -width 185 -height 9] \
		    [list label -name toolchainsComboLbl \
		        -title "Select a tool:" \
			-xpos 7 -ypos 34 -width 185 -height 9] \
		    [list combo -name buildCombo \
			-xpos 115 -ypos 18 -width 100 -height 70] \
		    [list combo -name toolchainCombo \
			    -xpos 115 -ypos 34 -width 100 -height 70] \
			[list label -name noteLbl \
			-title  "Note: These builds are invoked within the installed \
            		BSP's directory, and therefore do not include \
            		configuration changes made via any Project. \
					Depending on hardware configuration and image size, \
					not all images shown will build and/or run." \
			-xpos 7 -ypos 52 -width 200 -height 40] \
		    [list button -name okButton \
			-callback ::OldBuildMenu::onBuildOK \
			-title "OK" \
			-xpos 7 -ypos 95 -width 50 -height 14] \
		    [list button -name cancelButton \
		        -callback "windowClose buildDlg" \
			-title "Cancel" \
			-xpos 65 -ypos 95 -width 50 -height 14] \
		    [list button -name helpButton \
			-title "Help" \
		        -helpbutton \
			-xpos 165 -ypos 95 -width 50 -height 14] \
		]

    }
}

