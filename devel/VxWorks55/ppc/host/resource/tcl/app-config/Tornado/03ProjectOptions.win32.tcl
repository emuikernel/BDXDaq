##########################################################
# 03ProjectOptions.win32.tcl - Allows external editor options to be configured
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,05nov01,rbl  changing wording of tornado 1.0.1 compatibility section
# 01b,16oct01,rbl  adding WIND_PROJ_BASE and WIND_SOURCE_BASE options
# 01a,09sep98,cjs  written
#


namespace eval ProjectOptionsPage {
    variable m_self
    set m_self(title) "Project"
    set m_self(width) 300
    set m_self(height) 190
    set m_self(xpos) 5
    set m_self(ypos) 5
    set m_self(bShowOldProjectMenu) 0 
    set m_self(bShowCompDefSheet) 0 
    set m_self(windProjBase) [file nativename [wtxPath target proj]]
    set m_self(windSourceBase) "" 

    variable m_controlNames ""
    variable m_parent tornadoOptionsDlg

    proc onPageInit {} {
	variable m_parent
	variable m_self

	foreach control [controlListGet] {
	    if {![controlExists [lindex $control 2]]} {
		controlCreate $m_parent $control
	    }
	}
	
	controlCheckSet ${m_parent}.bShowCompDefSheet \
	    $m_self(bShowCompDefSheet)
	controlCheckSet ${m_parent}.bShowOldProjectMenu \
	    $m_self(bShowOldProjectMenu)
	controlTextSet ${m_parent}.txtWindProjBase \
	    $m_self(windProjBase)
	controlTextSet ${m_parent}.txtWindSourceBase \
	    $m_self(windSourceBase)
    }

    proc onPageEnter {} {
	controlsHide 0
    }

    proc queryPageExit {} {
	variable m_self
	variable m_parent
	controlsHide 1
        set windProjBase [controlTextGet ${m_parent}.txtWindProjBase]
        set windSourceBase [controlTextGet ${m_parent}.txtWindSourceBase]
        filePathFix windProjBase
        filePathFix windSourceBase
        if { $windProjBase != "" && \
            ![file isdirectory $windProjBase] } {
            messageBox "[file nativename $windProjBase] is not a valid directory\
                \n\nPlease enter a valid directory for WIND_PROJ_BASE"
	    return 0
        }
        if { $windSourceBase != "" && \
            ![file isdirectory $windSourceBase] } {
            messageBox "[file nativename $windSourceBase] is not a valid directory\
                \n\nPlease enter a valid directory for WIND_SOURCE_BASE"
	    return 0
        }
	return 1
    }

    proc onPageRestore {} {
	variable m_parent
	variable m_self

	set m_self(bShowOldProjectMenu) [appRegistryEntryRead -default 0 \
	    Workspace ShowOldProjectMenu]
	set m_self(bShowCompDefSheet) [::PSViewer::defPageShowRead]
	set m_self(windProjBase) [appRegistryEntryRead \
            -default [file nativename [wtxPath target proj]]  \
	    Workspace WindProjBase]
	set m_self(windSourceBase) [appRegistryEntryRead \
            -default ""  \
	    Workspace WindSourceBase]
    }

    proc onPageSave {} {
	variable m_parent
	set value [controlChecked ${m_parent}.bShowOldProjectMenu]
	appRegistryEntryWrite Workspace ShowOldProjectMenu $value
	if {$value} {
		::addBuildsCustomizeMenu
	} else {
		::removeBuildsCustomizeMenu
	}

	set value [controlChecked ${m_parent}.bShowCompDefSheet]
	::PSViewer::defPageShowWrite $value
        set windProjBase [controlTextGet ${m_parent}.txtWindProjBase]
        set windSourceBase [controlTextGet ${m_parent}.txtWindSourceBase]
        removeTrailingSlash windProjBase
        removeTrailingSlash windSourceBase
        if {$windProjBase == ""} {
            set windProjBase [file nativename [wtxPath target proj]]
        }
        appRegistryEntryWrite Workspace WindProjBase \
            $windProjBase
        appRegistryEntryWrite Workspace WindSourceBase \
            $windSourceBase
        filePathFix windProjBase
        filePathFix windSourceBase

        set ::env(WIND_PROJ_BASE) $windProjBase
        set ::env(WIND_SOURCE_BASE) $windSourceBase
    }

    proc controlListGet {} {
	variable m_self
	variable m_controlNames
	variable m_parent
	set m_controlNames { \
	    frmProjectMenu \
	    lblProjectMenu \
	    frmExtProjectOptions \
	    bShowOldProjectMenu \
	    bShowCompDefSheet \
            frmBuildEnvOptions \
            lblWindProjBase \
            txtWindProjBase \
            butWindProjBase \
            lblWindSourceBase \
            txtWindSourceBase \
            butWindSourceBase
	}

	set frmProjectMenu_x [expr $m_self(xpos) + 7]
	set frmProjectMenu_y [expr $m_self(ypos) + 15]
	set frmProjectMenu_w 286
	set frmProjectMenu_h 88 

	set bShowOldProjectMenu_x [expr $frmProjectMenu_x + 6]
	set bShowOldProjectMenu_y \
	    [expr $frmProjectMenu_y + 72]
	set bShowOldProjectMenu_w 150 
	set bShowOldProjectMenu_h 10 

	set lblProjectMenu_x [expr $frmProjectMenu_x + 8]
	set lblProjectMenu_y [expr $frmProjectMenu_y + 12]
	set lblProjectMenu_w [expr $frmProjectMenu_w - 20]
	set lblProjectMenu_h 60 

	set frmExtProjectOptions_x $frmProjectMenu_x
	set frmExtProjectOptions_y \
	    [expr $m_self(ypos) + 18 + $frmProjectMenu_h]
	set frmExtProjectOptions_w $frmProjectMenu_w
	set frmExtProjectOptions_h 27 

	set frmBuildEnvOptions_x $frmProjectMenu_x
	set frmBuildEnvOptions_y \
	    [expr $frmExtProjectOptions_y + \
            $frmExtProjectOptions_h + 3]
	set frmBuildEnvOptions_w $frmProjectMenu_w
	set frmBuildEnvOptions_h 44 

        set lblWindProjBase_x [expr $frmBuildEnvOptions_x + 8]
        set lblWindProjBase_y [expr $frmBuildEnvOptions_y + 12]
	set lblWindProjBase_w 150
	set lblWindProjBase_h 12 

        set txtWindProjBase_y $lblWindProjBase_y
	set txtWindProjBase_w 100
        set txtWindProjBase_x [expr $frmBuildEnvOptions_x + \
            $frmBuildEnvOptions_w - $txtWindProjBase_w - 22]
	set txtWindProjBase_h 12 

        set butWindProjBase_y $lblWindProjBase_y
	set butWindProjBase_w 14
        set butWindProjBase_x [expr $txtWindProjBase_x + \
            $txtWindProjBase_w + 2]
	set butWindProjBase_h 12 
    
        set lblWindSourceBase_x $lblWindProjBase_x
        set lblWindSourceBase_y [expr $lblWindProjBase_y + 14]
	set lblWindSourceBase_w $lblWindProjBase_w
	set lblWindSourceBase_h $lblWindProjBase_h 

        set txtWindSourceBase_y $lblWindSourceBase_y
	set txtWindSourceBase_w $txtWindProjBase_w
        set txtWindSourceBase_x $txtWindProjBase_x
	set txtWindSourceBase_h $txtWindProjBase_h 

        set butWindSourceBase_y $lblWindSourceBase_y
	set butWindSourceBase_w $butWindProjBase_w
        set butWindSourceBase_x $butWindProjBase_x
	set butWindSourceBase_h $butWindProjBase_h 

	set bShowCompDefSheet_x $bShowOldProjectMenu_x
	set bShowCompDefSheet_y \
	    [expr $frmExtProjectOptions_y + 12]
	set bShowCompDefSheet_w $bShowOldProjectMenu_w
	set bShowCompDefSheet_h 7 

	return  [list \
		    [list group -name frmProjectMenu \
			-title "Enable extended Build Options" \
			-hidden \
			-x $frmProjectMenu_x \
			-y $frmProjectMenu_y \
			-w $frmProjectMenu_w \
			-h $frmProjectMenu_h ] \
		    [list group -name frmExtProjectOptions \
			-hidden \
			-title "Component Properties" \
			-x $frmExtProjectOptions_x \
			-y $frmExtProjectOptions_y \
			-w $frmExtProjectOptions_w \
			-h $frmExtProjectOptions_h ] \
		    [list group -name frmBuildEnvOptions \
			-hidden \
			-title "Build Environment Variables" \
			-x $frmBuildEnvOptions_x \
			-y $frmBuildEnvOptions_y \
			-w $frmBuildEnvOptions_w \
			-h $frmBuildEnvOptions_h ] \
		    [list label -name lblProjectMenu \
			-hidden \
			-title "Check the following option if you wish to: \
				\n\na) Perform old-style builds in BSP directories \
					 (BSP targets will appear in the Build menu) \
				\n\nb) Use custom build \
				menu items" \
			-x $lblProjectMenu_x \
			-y $lblProjectMenu_y \
			-w $lblProjectMenu_w \
			-h $lblProjectMenu_h ] \
		    [list boolean -name bShowOldProjectMenu \
			-auto -hidden \
			-title "Enable extended Build Options" \
			-x $bShowOldProjectMenu_x \
			-y $bShowOldProjectMenu_y \
			-w $bShowOldProjectMenu_w \
			-h $bShowOldProjectMenu_h ] \
		    [list boolean -name bShowCompDefSheet \
			-auto -hidden \
			-title "Show advanced component properties" \
			-x $bShowCompDefSheet_x \
			-y $bShowCompDefSheet_y \
			-w $bShowCompDefSheet_w \
			-h $bShowCompDefSheet_h ] \
		    [list label -name lblWindProjBase \
			-hidden \
			-title "Project Path (WIND_PROJ_BASE)" \
			-x $lblWindProjBase_x \
			-y $lblWindProjBase_y \
			-w $lblWindProjBase_w \
			-h $lblWindProjBase_h ] \
		    [list label -name lblWindSourceBase \
			-hidden \
			-title "Source Path (WIND_SOURCE_BASE)" \
			-x $lblWindSourceBase_x \
			-y $lblWindSourceBase_y \
			-w $lblWindSourceBase_w \
			-h $lblWindSourceBase_h ] \
    	            [list text -name txtWindProjBase \
                        -hidden \
                        -x $txtWindProjBase_x \
                        -y $txtWindProjBase_y \
                        -w $txtWindProjBase_w \
                        -h $txtWindProjBase_h ] \
		    [list button -name butWindProjBase -title "..." \
                        -hidden \
			-x $butWindProjBase_x \
                        -y $butWindProjBase_y \
                        -w $butWindProjBase_w \
                        -h $butWindProjBase_h \
                        -callback {
                            set initialDir [controlTextGet \
                                ${::ProjectOptionsPage::m_parent}.txtWindProjBase]
			    set windProjBaseDir [dirBrowseDialogCreate \
			        -initialdir $initialDir \
			        -title "WIND_PROJ_BASE Directory" -okbuttontitle "Ok"]
                            if {$windProjBaseDir != ""} {
                                controlTextSet \
                                    ${::ProjectOptionsPage::m_parent}.txtWindProjBase \
                                    $windProjBaseDir
                            }
		        } \
		    ] \
    	            [list text -name txtWindSourceBase \
                        -hidden \
                        -x $txtWindSourceBase_x \
                        -y $txtWindSourceBase_y \
                        -w $txtWindSourceBase_w \
                        -h $txtWindSourceBase_h ] \
		    [list button -name butWindSourceBase -title "..." \
                        -hidden \
			-x $butWindSourceBase_x \
                        -y $butWindSourceBase_y \
                        -w $butWindSourceBase_w \
                        -h $butWindSourceBase_h \
                        -callback {
                            set initialDir [controlTextGet \
                                ${::ProjectOptionsPage::m_parent}.txtWindSourceBase]
			    set windSourceBaseDir [dirBrowseDialogCreate \
			        -initialdir $initialDir \
			        -title "WIND_SOURCE_BASE Directory" -okbuttontitle "Ok"]
                            if {$windSourceBaseDir != ""} {
                                controlTextSet \
                                    ${::ProjectOptionsPage::m_parent}.txtWindSourceBase \
                                    $windSourceBaseDir
                            }
		        } \
		    ] \
		]
			 
    }

    proc controlsHide {bHide} {
	variable m_controlNames
	variable m_parent

	foreach controlName $m_controlNames {
	    controlHide $m_parent.$controlName $bHide
	}
    }

    proc registerPage {} {
	variable m_self

	OptionsPageAdd "$m_self(title)" \
	    ProjectOptionsPage::onPageInit \
	    ProjectOptionsPage::onPageEnter \
	    ProjectOptionsPage::queryPageExit \
	    ProjectOptionsPage::onPageRestore \
	    ProjectOptionsPage::onPageSave
	    
    }

}

ProjectOptionsPage::registerPage
