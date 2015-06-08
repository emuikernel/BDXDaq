##########################################################
# 07InstallLicenseOptions.win32.tcl - Allows external editor options to be configured
#
# Copyright 1995-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01e,17jul02,cmf  edit text
# 01d,12mar02,cmf  Add host specific controls - SPR 73500
# 01c,05feb02,cmf  add & arg to exec of setup
# 01b,22jan02,cmf  Add flags to setup exec
# 01a,15jan02,cmf  written
#

package require Wind

namespace eval InstallLicenseOptionsPage {
	variable m_self
	set m_self(title) "Installation and Licenses"
	set m_self(width) 300
	set m_self(height) 190
	set m_self(xpos) 5
	set m_self(ypos) 5
 
	variable m_parent tornadoOptionsDlg


    ##########################################################
	proc onPageInit {} {
		variable m_parent
		variable m_self

		foreach control [controlListGet] {
			if {![controlExists [lindex $control 2]]} {
				controlCreate $m_parent $control
	    	}
		}
	
    }

    ##########################################################
    proc onPageEnter {} {
	controlsHide 0
    }

    ##########################################################
    proc queryPageExit {} {
	variable m_self
	variable m_parent
	controlsHide 1
	return 1
    }

    ##########################################################
    proc onPageRestore {} {
	variable m_parent
	variable m_self
    }

    ##########################################################
    proc onPageSave {} {
	variable m_parent
    }

    ##########################################################
    proc controlListGet {} {


	variable m_self
	variable m_controlNames
	variable m_parent
 	set wind_host_type [wtxHostType]
  
	if {[string compare $wind_host_type "x86-win32"] == 0} {
		set m_controlNames { \
		    frmReconfigReg \
	    	lblReconfigReg \
			frmFileTypeReg \
			lblFileTypeReg \
			frmLicMgmtConfig \
			lblLicMgmtConfig \
			lblNote \
			butReconfigReg \
			butFileTypeReg \
			butLicMgmtConfig}
	} elseif {[string compare $wind_host_type "sun4-solaris2"] == 0} { 
		set m_controlNames { \
			frmLicMgmtConfig \
			lblLicMgmtConfig \
			lblNote \
			butLicMgmtConfig}
	} else {puts "Error: wind_host_type not x86-win32 or sun4-solaris2"}

	# Reconfigure Tornado Registry and Windows File
	# types only valid for Windows hosts
	if {[string compare $wind_host_type "x86-win32"] == 0} {
		
	############### Reconfigure Tornado Registry at Startup
	set frmReconfigReg_x [expr $m_self(xpos) + 7]
	set frmReconfigReg_y [expr $m_self(ypos) + 15]
	set frmReconfigReg_w 286
	set frmReconfigReg_h 45 

    set lblReconfigReg_x [expr $frmReconfigReg_x + 8]
	set lblReconfigReg_y [expr $frmReconfigReg_y + 12]
	set lblReconfigReg_w [expr $frmReconfigReg_w - 20]
	set lblReconfigReg_h [expr $frmReconfigReg_h - 14] 

    set butReconfigReg_x [expr $frmReconfigReg_x + $frmReconfigReg_w - 95]
    set butReconfigReg_y [expr $frmReconfigReg_y + 16]
    set butReconfigReg_w 80
    set butReconfigReg_h 15 
	
    ############### Windows File Type Registrations
    set frmFileTypeReg_x $frmReconfigReg_x
	set frmFileTypeReg_y \
	    [expr $m_self(ypos) + 18 + $frmReconfigReg_h]
	set frmFileTypeReg_w $frmReconfigReg_w
	set frmFileTypeReg_h $frmReconfigReg_h 

    set lblFileTypeReg_x [expr $frmFileTypeReg_x + 8]
    set lblFileTypeReg_y [expr $frmFileTypeReg_y + 12]
    set lblFileTypeReg_w [expr $frmFileTypeReg_w - 20]
    set lblFileTypeReg_h [expr $frmReconfigReg_h - 14] 
	
	set butFileTypeReg_x $butReconfigReg_x
    set butFileTypeReg_y [expr $frmFileTypeReg_y + 16]
    set butFileTypeReg_w $butReconfigReg_w
    set butFileTypeReg_h $butReconfigReg_h 

    ############### Reconfigure Tornado Registry at Startup
	# this is the x86-win32 host type definition of the
	# following control - look below for solaris version

    set frmLicMgmtConfig_x $frmReconfigReg_x
    set frmLicMgmtConfig_y \
   	    [expr $frmFileTypeReg_y + $frmFileTypeReg_h + 3]
   	set frmLicMgmtConfig_w $frmReconfigReg_w
   	set frmLicMgmtConfig_h $frmReconfigReg_h 

    set lblLicMgmtConfig_x [expr $frmLicMgmtConfig_x + 8]
    set lblLicMgmtConfig_y [expr $frmLicMgmtConfig_y + 12]
	set lblLicMgmtConfig_w [expr $frmLicMgmtConfig_w - 20]
	set lblLicMgmtConfig_h [expr $frmReconfigReg_h   - 14]

    set butLicMgmtConfig_x $butReconfigReg_x
    set butLicMgmtConfig_y [expr $frmLicMgmtConfig_y + 16]
	set butLicMgmtConfig_w $butReconfigReg_w
    set butLicMgmtConfig_h $butReconfigReg_h 
    
    ############### Note
    set lblNote_x [expr $frmReconfigReg_x + 8 ]
	set lblNote_y [expr $frmLicMgmtConfig_y + $frmLicMgmtConfig_h + 3]
	set lblNote_w [expr $frmReconfigReg_w - 10]
	set lblNote_h 25 
        
	}

    # if solaris host - this is first control and can't key
	# off of previous control for location

	############### Reconfigure Tornado Registry at Startup
	
	if {[string compare $wind_host_type "sun4-solaris2"] == 0} {
	    set frmLicMgmtConfig_x [expr $m_self(xpos) + 7]
		    set frmLicMgmtConfig_y \
        [expr $m_self(ypos) + 15]
    	set frmLicMgmtConfig_w 286
    	set frmLicMgmtConfig_h 45 

	    set lblLicMgmtConfig_x [expr $frmLicMgmtConfig_x + 8]
    	set lblLicMgmtConfig_y [expr $frmLicMgmtConfig_y + 12]
		set lblLicMgmtConfig_w [expr $frmLicMgmtConfig_w - 20]
		set lblLicMgmtConfig_h [expr $frmLicMgmtConfig_h - 14]

    	set butLicMgmtConfig_x [expr $frmLicMgmtConfig_x + $frmLicMgmtConfig_w - 95]
    	set butLicMgmtConfig_y [expr $frmLicMgmtConfig_y + 16]
		set butLicMgmtConfig_w 80
    	set butLicMgmtConfig_h 15 
    
    	############### Note
    	set lblNote_x [expr $frmLicMgmtConfig_x + 8 ]
		set lblNote_y [expr $frmLicMgmtConfig_y + $frmLicMgmtConfig_h + 3]
		set lblNote_w [expr $frmLicMgmtConfig_w - 10]
		set lblNote_h 25 

		# return for sun4-solaris2
	    return  [list \
	    [list group -name frmLicMgmtConfig \
			-hidden \
			-title "End User License Management Configuration" \
			-x $frmLicMgmtConfig_x \
			-y $frmLicMgmtConfig_y \
			-w $frmLicMgmtConfig_w \
			-h $frmLicMgmtConfig_h ] \
		[list label -name lblLicMgmtConfig \
			-hidden \
			-title "Allows you to reconfigure your license\nmanagement" \
			-x $lblLicMgmtConfig_x \
			-y $lblLicMgmtConfig_y \
			-w $lblLicMgmtConfig_w \
			-h $lblLicMgmtConfig_h ] \
		[list label -name lblNote \
			-hidden \
			-title "Note: Pressing the button above launches the Tornado Setup program for reconfiguration." \
			-x $lblNote_x \
			-y $lblNote_y \
			-w $lblNote_w \
			-h $lblNote_h ] \
        [list button -name butLicMgmtConfig -title "Reconfigure Licensing" \
        	-hidden \
			-x $butLicMgmtConfig_x \
            -y $butLicMgmtConfig_y \
            -w $butLicMgmtConfig_w \
            -h $butLicMgmtConfig_h \
            -callback {
            	set wind_base ""
            	set wind_base [Wind::base]
            	exec $wind_base/SETUP/SETUP -L &
            }
        ] \
	]
	}

    # return for x86-win32
	if {[string compare $wind_host_type "x86-win32"] == 0} { 

    return  [list \
	    [list group -name frmReconfigReg \
			-title "Reconfigure Tornado Registry at Startup" \
			-hidden \
			-x $frmReconfigReg_x \
			-y $frmReconfigReg_y \
			-w $frmReconfigReg_w \
			-h $frmReconfigReg_h ] \
	    [list group -name frmFileTypeReg \
			-hidden \
			-title "Windows File Type Registrations" \
			-x $frmFileTypeReg_x \
			-y $frmFileTypeReg_y \
			-w $frmFileTypeReg_w \
			-h $frmFileTypeReg_h ] \
	    [list group -name frmLicMgmtConfig \
			-hidden \
			-title "End User License Management Configuration" \
			-x $frmLicMgmtConfig_x \
			-y $frmLicMgmtConfig_y \
			-w $frmLicMgmtConfig_w \
			-h $frmLicMgmtConfig_h ] \
        [list label -name lblReconfigReg \
			-hidden \
			-title "Reconfigure the Tornado Registry for automatic \
				\nstartup as a service, placement in the startup \
				\ngroup, or manual startup." \
			-x $lblReconfigReg_x \
			-y $lblReconfigReg_y \
			-w $lblReconfigReg_w \
			-h $lblReconfigReg_h ] \
        [list label -name lblFileTypeReg \
			-hidden \
			-title "Allows you to register file types that will use \
				\nTornado as the default application." \
                -x $lblFileTypeReg_x \
                -y $lblFileTypeReg_y \
                -w $lblFileTypeReg_w \
                -h $lblFileTypeReg_h ] \
		[list label -name lblLicMgmtConfig \
			-hidden \
			-title "Allows you to reconfigure your license\nmanagement" \
			-x $lblLicMgmtConfig_x \
			-y $lblLicMgmtConfig_y \
			-w $lblLicMgmtConfig_w \
			-h $lblLicMgmtConfig_h ] \
		[list label -name lblNote \
			-hidden \
			-title "Note: Pressing one of the buttons above launches the Tornado Setup program for reconfiguration." \
			-x $lblNote_x \
			-y $lblNote_y \
			-w $lblNote_w \
			-h $lblNote_h ] \
		[list button -name butReconfigReg -title "Reconfigure Registry" \
			-hidden \
			-x $butReconfigReg_x \
			-y $butReconfigReg_y \
			-w $butReconfigReg_w \
			-h $butReconfigReg_h \
			-callback {
				set wind_base [WIND_Env::WIND_BASE_get]
				exec $wind_base/SETUP/SETUP.EXE -r &
			}		        
		] \
		[list button -name butFileTypeReg -title "Register File Types" \
			-hidden \
			-x $butFileTypeReg_x \
			-y $butFileTypeReg_y \
			-w $butFileTypeReg_w \
			-h $butFileTypeReg_h \
			-callback {
				set wind_base [WIND_Env::WIND_BASE_get]
				exec $wind_base/SETUP/SETUP.EXE -a &
            }		        
		] \
        [list button -name butLicMgmtConfig -title "Reconfigure Licensing" \
        	-hidden \
			-x $butLicMgmtConfig_x \
            -y $butLicMgmtConfig_y \
            -w $butLicMgmtConfig_w \
            -h $butLicMgmtConfig_h \
            -callback {
            	set wind_base [WIND_Env::WIND_BASE_get]
                exec $wind_base/SETUP/SETUP.EXE -L &
            }
        ] \
	]
	}
	}

    ##########################################################
    proc controlsHide {bHide} {
		variable m_controlNames
		variable m_parent

		foreach controlName $m_controlNames {
	    	controlHide $m_parent.$controlName $bHide
		}
    }

    ##########################################################
    proc registerPage {} {
		variable m_self

		OptionsPageAdd "$m_self(title)" \
	    	InstallLicenseOptionsPage::onPageInit \
	    	InstallLicenseOptionsPage::onPageEnter \
	    	InstallLicenseOptionsPage::queryPageExit \
	    	InstallLicenseOptionsPage::onPageRestore \
	    	InstallLicenseOptionsPage::onPageSave
	    
    	}

	}

##########################################################
InstallLicenseOptionsPage::registerPage
