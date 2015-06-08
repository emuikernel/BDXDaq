# 01TgtSvrConfigure.win32.tcl - Target Server Configuration dialog
#
# Copyright (C) 1995-99 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,04jun02,cmf  remove call to Wind::tmpDirGet because target server cannot
#                  handle tsfs filenames with spaces at this time
# o1j,11apr03,cmf fix SPR# 75466 and SPR# 73273
# 01i,21jan02,tcr enable TSFS r/w by default
# 01h,24sep01,c_c  Added '-noG' option
# 01g,21apr99,jak  fixed bug initializing Mem Cache and Auth file 
# 01f,17mar99,rbl  all choice controls in group must be explicitly initialized
# 01e,17feb99,jak add "Miscellaneous, Other options", fixed bug in "New"
# 01d,15feb99,jak  add portmapper option
# 01c,10feb99,l_k  -noenter removed from # commandline tsCfg_ctrlList
# 01b,09feb99,jak  remove references to on_targetManage_help
# 01a,18jan99,jak renamed from 01TargetServer.win32.tcl
#				  moved code for tgtsvr management into 01TgtSvrManage.win32.tcl
#
#
# DESCRIPTION
# This script contains procedures that create the target server Configure dialog
#
#*/

##############################################################################
#
# ACCELERATORS IN USE
#
#	Top level:
#		A,C,D,E,G,H,L,M,N,P,R
#	Property: Authorizaions
#		F,K
#	Property: Backend
#		Top level:
#			B
# 		Backend: known:
#			I,Y
# 		Backend: serial:
#			S,T 
# 		Backend: unknown:
#			O	
#	Property: Core file and Symbols
#		B,F,I,O,S,T,Y
#	Property: Memory Cache Size
#		T,Y
#	Property: TSFS
#		F,T,W,Y 
#	Property: Console and Redirection
#		I,S,W	
#	Property: Logging 
#		B,F,I,S,W	
#	Property: Miscellaneous 
#		O,U	
#

# Registry entries:
#   [section key value]
#
#   [Target tgtsvrDescrList descrList] 	descrlist is a list of ts descriptions
#   [Target description cfgList ]   cfg is a list specifying the cfg for a ts
#

package require Wind

##############################################################################
# GLOBAL VARIABLES
#

# target server menu items
set listTSMenuItems {}

set listBackends(known) [list \
							netrom netrom-d \
							wdbpipe wdbpipe-d \
							wdbrpc wdbrpc-d \
							wdbserial wdbserial-d \
						]

set listBackends(serial) [list \
							wdbserial wdbserial-d \
						]

# target server data base variables

set tsCfg_regEntries ""
set tsCfg_arrays(0) ""
set tsCfg_descrList ""
set tsCfg_new ""

set tsCfg_defaultSelection 1
set tsCfg_defaultBackend 0
set tsCfg_currentSelection -1
set tsCfg_maxSelections 9 ;# there are 9 control lists, but only 6 selections

set tsCfg_debugMode [uitclinfo debugmode]

# setting list of available backends:
# 	-d's for debug mode, non-d's otherwise
set tsCfg_backendList {}
set bkenddir [wtxPath host [wtxHostType] lib backend]
foreach fileName [lsort [glob -nocomplain $bkenddir/*.dll]] {
    if { [regexp {[Dd][Ll][Ll]} [file extension $fileName]] &&
     [file isfile $fileName] } {
		set backendName [file rootname [file tail $fileName]]
		if {[string match {*-d} $backendName]} {
			if {$tsCfg_debugMode} {
    			lappend tsCfg_backendList $backendName 
			}
		} else {
			if {!$tsCfg_debugMode} {
    			lappend tsCfg_backendList $backendName 
			}
		}
    }
}

set tsCfg_baud_rates {9600 19200 38400 57600 115200}
set tsCfg_deviceList {COM1 COM2 COM3 COM4}
set tsCfg_saved_description ""
if {$tsCfg_debugMode} {
	set tsCfg_tgtsvr "tgtsvr-d.exe"
} else {
	set tsCfg_tgtsvr "tgtsvr.exe"
}

set tsCfg_commandLine ""
set tsCfg_currentDescr ""

# option variables for the target server command line
set tsCfg_serverName ""
set tsCfg_usersFileName ""
set tsCfg_userLock ""
set tsCfg_coreFileName ""
set tsCfg_objecFormat ""
set tsCfg_nosyms ""
set tsCfg_allsyms ""
set tsCfg_synchro ""
set tsCfg_baudRate ""
set tsCfg_hostTtyDevice ""
set tsCfg_verboseMode -V 
set tsCfg_consoleMode ""
set tsCfg_displayHost ""
set tsCfg_memCacheSize ""
set tsCfg_backendType ""
set tsCfg_backendTimeout ""
set tsCfg_backendResendNumber ""
set tsCfg_FS_path ""
set tsCfg_tgtNameIP ""
set tsCfg_device ""
set tsCfg_version ""
set tsCfg_redirectChild "" 
set tsCfg_closeWindowOnExit "" 
set tsCfg_menuItem 0 
set tsCfg_backendOpt ""
set tsCfg_redirectIO ""
set tsCfg_console ""
set tsCfg_redirectShell ""
set tsCfg_backend_logfile ""
set tsCfg_backend_logsize ""
set tsCfg_wtx_logfile ""
set tsCfg_wtx_logsize ""
set tsCfg_wtx_logfilter ""
set tsCfg_omf ""
set tsCfg_usepm ""
set tsCfg_memNoGrowth ""
set tsCfg_miscOpt ""

set tsCfg_timeout_tmp ""
set tsCfg_retry_tmp ""
set tsCfg_cacheSize_tmp ""

# authorization

set wind_uid [if [info exists env(WIND_UID)] {format "= $env(WIND_UID)"} \
	     {format "not set"}]

set tsCfg_ctrlList(0) [list \
    { choice -title "None" -name tgtsvr_authNone_choice -auto -hidden
	     -callback on_tgtsvr_authNone -x 14 -y 127 -w 33 -h 10 -y 3333 } \
    [list boolean -title "Loc&k on Startup (WIND_UID $wind_uid)" \
		  -name tgtsvr_authLock_choice -auto -hidden \
		  -callback on_tgtsvr_authLock -x 14 -y 112 -w 150 -h 10 ] \
    { label -title "User ID &File" -name tgtsvr_authUserIDFile_label -hidden
	    -x 14 -y 130 -w 50 -h 10 } \
    { text -name tgtsvr_authFilePath_text  -hidden
	   -callback on_tgtsvr_authFilePath_text -x 58 -y 128 -w 127 -h 12 } \
    { button -title "..." -name tgtsvr_authFilePath_button  -hidden
	     -callback on_tgtsvr_authFilePath_button -x 190 -y 128 -w 13 \
	     -h 12 } \
]
set tsCfg_ctrlItem(0) "Authorizations"
set tsCfg_ctrlNames(0) {
    tgtsvr_authNone_choice tgtsvr_authLock_choice
    tgtsvr_authUserIDFile_label tgtsvr_authFilePath_text
    tgtsvr_authFilePath_button
}


# backend selection

set tsCfg_ctrlList(1) {
    { label -title "Available &Back Ends" -name tgtsvr_backend_available_label \
	    -hidden -x 14 -y 112 -w 70 -h 8 }
    { list -name tgtsvr_backend_available_list -hidden \
	   -callback on_tgtsvr_backend_available -x 14 -y 124 -w 65 -h 52 }
    { label -title "T&imeout (sec)" -name tgtsvr_backend_timeout_label -hidden \
	  -x 90 -y 112 -w 60 -h 9 }
    { text -name tgtsvr_backend_timeout_text -hidden \
	   -callback on_tgtsvr_backend_timeout -x 90 -y 124 -w 50 -h 12 }
    { label -title "Re-tr&y (Count)" -name tgtsvr_backend_resend_label -hidden \
	    -x 153 -y 112 -w 50 -h 9 }
    { text -name tgtsvr_backend_resend_text -hidden \
	   -callback on_tgtsvr_backend_resend -x 153 -y 124 -w 50 -h 12 }
    { label -title "&Other Options" -name tgtsvr_backend_opt_label -hidden \
	  -x 90 -y 124 -w 65 -h 9 }
    { text -name tgtsvr_backend_opt_text -hidden \
	   -callback on_tgtsvr_backend_opt -x 90 -y 136 -w 65 -h 12 }
}
set tsCfg_ctrlItem(1) "Back End"
set tsCfg_ctrlNames(1) {
    tgtsvr_backend_available_label
    tgtsvr_backend_available_list
    tgtsvr_backend_timeout_label
    tgtsvr_backend_timeout_text
    tgtsvr_backend_resend_label
    tgtsvr_backend_resend_text
    tgtsvr_backend_opt_label
    tgtsvr_backend_opt_text
}

# core file and symbols

set tsCfg_ctrlList(2) {
    { choice -title "F&ile Path From Target (If Available)" \
	     -name tgtsvr_targetPath_choice -hidden
	     -newgroup -auto -callback on_tgtsvr_targetPath_choice -x 14 \
	     -y 112 -w 129 -h 10 }
    { choice -title "&File" -name tgtsvr_coreFile_choice -hidden -auto \
	     -callback on_tgtsvr_coreFile_choice -x 14 -y 128 -w 26 -h 10 }
    { text -name tgtsvr_coreFile_text -hidden \
	   -callback on_tgtsvr_coreFile_text -x 43 -y 128 -w 143 -h 12 }
    { button -title "..." -name tgtsvr_coreFilePathBrowse_button -hidden \
	     -callback on_tgtsvr_coreFilePathBrowse -x 190 -y 128 -w 13 -h 12 }
    { frame -white -name tgtsvr_coreFile_frame1 -w 1 -h 1 -hidden}	
    { choice -title "Glo&bal Symbols" -name tgtsvr_globalSymbols_choice \
	     -newgroup -auto -hidden -callback on_tgtsvr_globalSymbols -x 14 \
	     -y 145 -w 60 -h 10 }
    { choice -title "All &Symbols" -name tgtsvr_allSymbols_choice -auto \
	     -hidden -callback on_tgtsvr_allSymbols -x 87 -y 145 -w 50 -h 10 }
    { choice -title "N&o Symbols" -name tgtsvr_noSymbols_choice -auto -hidden \
	     -callback on_tgtsvr_noSymbols -x 147 -y 145 -w 50 -h 10 }
    { boolean -title "S&ynchronize Target/Host Symbol Tables" \
	      -name tgtsvr_synchro_bool -auto -hidden \
	      -callback on_tgtsvr_synchro -x 14 -y 160 -w 150 -h 10 }
    { label -title "Objec&t Module Format" -name tgtsvr_omf_label \
		-hidden -x 14 -y 174 -w 75 -h 8 } \
    { text -name tgtsvr_omf_text \
	   -callback on_tgtsvr_omf_text -hidden -x 90 -y 172 -w 40 \
	   -h 12 } \
}
set tsCfg_ctrlItem(2) "Core File and Symbols"
set tsCfg_ctrlNames(2) {
    tgtsvr_targetPath_choice
    tgtsvr_coreFile_choice
    tgtsvr_coreFile_text
    tgtsvr_coreFilePathBrowse_button
    tgtsvr_coreFile_frame1
    tgtsvr_globalSymbols_choice
    tgtsvr_allSymbols_choice
    tgtsvr_noSymbols_choice
    tgtsvr_synchro_bool
	tgtsvr_omf_label
	tgtsvr_omf_text
}

# cache size

set tsCfg_ctrlList(3) {
    { choice -title "Defaul&t (1 M Bytes)" -newgroup \
	     -name tgtsvr_memCacheDefault_choice -hidden -auto \
	     -callback on_tgtsvr_memCacheDefault -x 14 -y 112 -w 175 -h 10 }
    { choice -title "&Specif&y (K Bytes)" -name tgtsvr_memCache_choice -hidden \
	  -auto -callback on_tgtsvr_memCache -x 14 -y 128 -w 70 -h 10 }
    { text -name tgtsvr_memCache_text -hidden \
	   -callback on_tgtsvr_memCache_text -x 85 -y 128 -w 40 -h 12 }
}
set tsCfg_ctrlItem(3) "Memory Cache Size"
set tsCfg_ctrlNames(3) {
    tgtsvr_memCacheDefault_choice
    tgtsvr_memCache_choice
    tgtsvr_memCache_text
}

# target server file system 

set tsCfg_ctrlList(4) [list \
		{ boolean -title "Enable &File System" -name tgtsvr_FS_path_bool -hidden
		  -auto -callback on_tgtsvr_FS_path_bool -x 14 -y 112 -w 75 -h 10 } \
		{ label -title "Roo&t" -name tgtsvr_FS_path_label -hidden \
		  -x 24 -y 128 -w 30 -h 10 } \
		{ text -name tgtsvr_FS_rootPath_text -hidden
		  -callback on_tgtsvr_FS_rootPath_text -x 45 -y 126 -w 100 -h 12 } \
    	{ button -title "..." -name tgtsvr_FS_rootPath_button -hidden \
	     	-callback on_tgtsvr_FS_rootPath_button -x 150 -y 126 -w 13 -h 12 } \
		{ choice -title "Read onl&y" -name tgtsvr_FS_read_choice -hidden
		  -newgroup -auto -callback on_tgtsvr_FS_rw_choice \
		  -x 24 -y 140 -w 100 -h 10 } \
		{ choice -title "Read / &Write" -name tgtsvr_FS_rw_choice -hidden
		  -auto -callback on_tgtsvr_FS_rw_choice -x 24 -y 152 -w 100 -h 10 } \
	]

set tsCfg_ctrlItem(4) "Target Server File System"
set tsCfg_ctrlNames(4) {
		tgtsvr_FS_path_bool
		tgtsvr_FS_path_label
		tgtsvr_FS_rootPath_text
		tgtsvr_FS_rootPath_button
		tgtsvr_FS_read_choice
		tgtsvr_FS_rw_choice
	}



# console and redirection

set tsCfg_ctrlList(5) [list \
    	{ boolean -title "Redirect Target &IO" -name tgtsvr_redirectIO_bool \
	    -callback on_tgtsvr_redirectIO_bool \
		-hidden -auto -x 14 -y 112 -w 150 -h 10 } \
		{ boolean -title "Create Console &Window" -name tgtsvr_console_bool \
	    -callback on_tgtsvr_console_bool \
		-hidden -auto -x 14 -y 128 -w 90 -h 10 } \
		{ boolean -title "Redirect Target &Shell" \
		-name tgtsvr_redirectShell_bool \
	    -callback on_tgtsvr_redirectShell_bool \
		-hidden -auto -x 120 -y 128 -w 80 -h 10 } \
	]
set tsCfg_ctrlItem(5) "Console and Redirection"
set tsCfg_ctrlNames(5) {
	tgtsvr_redirectIO_bool
    tgtsvr_console_bool
	tgtsvr_redirectShell_bool
}



# logging

set tsCfg_ctrlList(6) [list \
    { boolean -title "&Backend Log File" -name tgtsvr_backend_log_bool \
		-callback on_tgtsvr_backend_log_bool \
		-hidden -auto -x 14 -y 114 -w 70 -h 8 } \
    { text -name tgtsvr_backend_logfile_text \
	   -callback on_tgtsvr_backend_logfile_text -hidden -x 85 -y 112 -w 100 \
	   -h 12 } \
    { button -title "..." -name tgtsvr_backend_logfile_button -hidden \
	     -callback on_tgtsvr_backend_logfile_button -x 190 -y 112 -w 13 -h 12 } \
    { label -title "Max. &Size:" -name tgtsvr_backend_logsize_label \
	     -hidden -x 30 -y 130 -w 50 -h 12 } \
    { text -name tgtsvr_backend_logsize_text \
	   -callback on_tgtsvr_backend_logsize_text \
	   -hidden -x 65 -y 128 -w 35 -h 12 } \
    { boolean -title "&WTX Log File" -name tgtsvr_wtx_log_bool \
	   -callback on_tgtsvr_wtx_log_bool \
		-hidden -auto -x 14 -y 150 -w 70 -h 8 } \
    { text -name tgtsvr_wtx_logfile_text \
	   -callback on_tgtsvr_wtx_logfile_text \
	   -hidden -x 85 -y 148 -w 100 \
	   -h 12 } \
    { button -title "..." -name tgtsvr_wtx_logfile_button -hidden \
	   -callback on_tgtsvr_wtx_logfile_button \
	     -x 190 -y 148 -w 13 -h 12 } \
    { label -title "Max. S&ize:" -name tgtsvr_wtx_logsize_label \
	     -hidden -x 30 -y 166 -w 50 -h 12 } \
    { text -name tgtsvr_wtx_logsize_text \
	   -callback on_tgtsvr_wtx_logsize_text \
	   -hidden -x 65 -y 164 -w 35 -h 12 } \
    { label -title "&Filter:" -name tgtsvr_wtx_logfilter_label \
	     -hidden -x 105 -y 166 -w 50 -h 12 } \
    { text -name tgtsvr_wtx_logfilter_text \
	   -callback on_tgtsvr_wtx_logfilter_text \
	   -hidden -x 125 -y 164 -w 78 -h 12 } \
	]
set tsCfg_ctrlItem(6) "Logging"
set tsCfg_ctrlNames(6) {
    tgtsvr_backend_log_bool
    tgtsvr_backend_logfile_text
    tgtsvr_backend_logfile_button
    tgtsvr_backend_logsize_label
    tgtsvr_backend_logsize_text
    tgtsvr_wtx_log_bool
    tgtsvr_wtx_logfile_text
    tgtsvr_wtx_logfile_button
    tgtsvr_wtx_logsize_label
    tgtsvr_wtx_logsize_text
    tgtsvr_wtx_logfilter_label
    tgtsvr_wtx_logfilter_text
}


# miscellaneous - portmapper 

set tsCfg_ctrlList(7) [list \
		{ boolean -title "&Use portmapper (for Tornado 1.0 compatibility)" \
		-name tgtsvr_misc_usepm_bool -hidden
	  	-auto -callback on_tgtsvr_misc_usepm_bool -x 14 -y 112 -w 175 -h 12 } \
    	{ label -title "&Other Options" -name tgtsvr_misc_opt_label -hidden \
	  	-x 14 -y 128 -w 65 -h 9 } \
    	{ text -name tgtsvr_misc_opt_text -hidden \
	   	-callback on_tgtsvr_misc_opt -x 65 -y 126 -w 100 -h 12 } \
	]
set tsCfg_ctrlItem(7) "Miscellaneous"
set tsCfg_ctrlNames(7) {
	tgtsvr_misc_usepm_bool
	tgtsvr_misc_opt_label
	tgtsvr_misc_opt_text
}

# Target memory pool 

set tsCfg_ctrlList(8) [list \
		{ boolean -title "Don't allow automatic &growth of tgtsvr memory pool" \
		-name tgtsvr_memory_growth_bool -hidden
	  	-auto -callback on_tgtsvr_memNoGrowth -x 14 -y 112 -w 175 -h 12 } \
	]
set tsCfg_ctrlItem(8) "Target Memory Pool"
set tsCfg_ctrlNames(8) {
	tgtsvr_memory_growth_bool
}


# baud rate and port
set tsCfg_ctrlList(9) {
    { label -title "&Speed (bps)" -name tgtsvr_backend_baud_label -hidden \
	    -x 152 -y 142 -w 50 -h 9}
    { combo -name tgtsvr_backend_baud_combo -hidden \
	    -callback on_tgtsvr_backend_baud -x 152 -y 153 -w 50 -h 40}
    { label -title "Serial Por&t" -name tgtsvr_backend_device_label -hidden \
	    -x 90 -y 142 -w 60 -h 9 }
    { combo -name tgtsvr_backend_device_combo \
	    -callback on_tgtsvr_backend_device -hidden -x 90 -y 153 -w 50 \
	    -h 50 }
}
set tsCfg_ctrlNames(9) {
    tgtsvr_backend_baud_label
    tgtsvr_backend_baud_combo
    tgtsvr_backend_device_label
    tgtsvr_backend_device_combo
}

# commandline
set tsCfg_ctrlList(10) {
    { label -title "Command Line" -name tgtsvr_commandLine_label -hidden -x 7 \
	    -y 216 -w 55 -h 8 }
    { text -name tgtsvr_commandLine_text -multiline -readonly -vscroll \
	   -x 7 -y 226 -w 205 -h 30 }
}
set tsCfg_ctrlNames(10) {tgtsvr_commandLine_label tgtsvr_commandLine_text}

# last row - user commands
set tsCfg_ctrlList(11) {
    { button -title "OK" -name tgtsvr_OK_button -defaultbutton \
	     -callback on_tgtsvr_OK -x 7 -y 261 -w 48 -h 14 }
    { button -title "Cancel" -name tgtsvr_cancel_button \
	     -callback on_tgtsvr_cancel -x 111 -y 261 -w 48 -h 14 }
    { button -helpbutton -title "&Help" -name tgtsvr_help_button \
	     -x 166 -y 261 -w 48 -h 14 }
}
# if more controls are added to this entry, the function show_last_row must also
# be updated
set tsCfg_ctrlNames(11) {
    tgtsvr_OK_button tgtsvr_cancel_button tgtsvr_help_button
}


################################################################################
#
# controlInit
#
# Create all control

proc controlInit {args} {
    global tsCfg_ctrlList
    global tsCfg_ctrlNames
   
    for {set elem 0} {$elem <  [array size tsCfg_ctrlList]} {incr elem} {
	set nItems [llength $tsCfg_ctrlList($elem)]
	for {set ix 0} {$ix < $nItems} {incr ix} {
	    set ctrl [lindex $tsCfg_ctrlList($elem) $ix]
	    set name [lindex $tsCfg_ctrlNames($elem) $ix]
	    if {[controlExists tsCfgDlg.$name] == 0} {
		controlCreate tsCfgDlg $ctrl
	    } else {
		controlHide tsCfgDlg.$name 1
	    }
	}
    }
}


################################################################################
#
# get_values_from_tsCfg_arrays
#
proc get_values_from_tsCfg_arrays {description} {
    global tsCfg_arrays
    global tsCfg_descrList

    foreach descr $tsCfg_descrList {
	if {[string match $descr $description]} {
	    return $tsCfg_arrays($description)
	}
    }
    return ""
}

################################################################################
#
# read_all_tgtsvr_entries
#
# 	Reads tgtsvr entries in the registry and displays them in
#	the tgtsvr descriptions list box.
#

proc read_all_tgtsvr_entries {args} {
    global tsCfg_arrays
    global tsCfg_descrList
    global tsCfg_regEntries

    set tsCfg_regEntries [appRegistryEntryRead Target tgtsvrDescrList]
    if {$tsCfg_regEntries != ""} {
		# get working copy of registry entries
		set tsCfg_descrList [appRegistryEntryRead Target tgtsvrDescrList]
		foreach entry $tsCfg_descrList {
			if {$entry == ""} {
				continue
			}
	    	set tsCfg_arrays($entry) [appRegistryEntryRead Target $entry]
	    	# write each entries to list box
	    	set listContents [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
	    	set newContents [lappend listContents $entry]
	    	controlValuesSet tsCfgDlg.tgtsvr_descr_list $newContents
		}
    }
}


################################################################################
#
# write_all_tgtsvr_descriptions
#

proc write_all_tgtsvr_descriptions {args} {
    global tsCfg_arrays
    global tsCfg_descrList

    set tsCfg_descrList [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
	set writtenList {}
    if {$tsCfg_descrList != ""} {
		foreach index $tsCfg_descrList {
			if {$index != ""} {
	    		appRegistryEntryWrite Target $index $tsCfg_arrays($index)
				lappend writtenList $index
			}
		}
    }
    appRegistryEntryWrite Target tgtsvrDescrList $writtenList
}

################################################################################
#
# ts_checkForSpaces
#
# 	Returns 1 if spaces found in the parameter passed, 0 otherwise
#

proc ts_checkForSpaces {descriptor} {
    global tsCfg_serverName
    global tsCfg_usersFileName
    global tsCfg_coreFileName
    global tsCfg_memCacheSize
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_backend_logfile
    global tsCfg_tgtNameIP

    set warning "has a space character within it, please remove it."
    if {$tsCfg_serverName != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_serverName 1 1]] || [lrange $tsCfg_serverName 2 2] != "" } {
	    messageBox "Error: '$descriptor' - Target Server Name $warning"
	    return 1
	}
    }
    if {$tsCfg_usersFileName != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_usersFileName 1 1]] || [lrange $tsCfg_usersFileName 2 2] != "" } {
	    messageBox "Error: '$descriptor' - User ID File $warning"
	    return 1
	}
    }
    if {$tsCfg_coreFileName != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_coreFileName 1 1]]  || [lrange $tsCfg_coreFileName 2 2] != "" } {
	    messageBox "Error: '$descriptor' - Core File  $warning"
	    return 1
	}
    }
    if {$tsCfg_memCacheSize  != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_memCacheSize 1 1]] || [lrange $tsCfg_memCacheSize 2 2] != "" } {
	    messageBox "Error: '$descriptor' - Memory Cache Size option $warning"
	    return 1
	}
    }
    if {$tsCfg_backendTimeout != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_backendTimeout 1 1]] || [lrange $tsCfg_backendTimeout 2 2] != "" } {
	    messageBox "Error: '$descriptor' - Timeout option $warning"
	    return 1
	}
    }
    if {$tsCfg_backendResendNumber != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_backendResendNumber 1 1]] || [lrange $tsCfg_backendResendNumber 2 2] != "" } {
	    messageBox "Error: '$descriptor' - Re-try option $warning"
	    return 1
	}
    }
    if {$tsCfg_backend_logfile != ""} {
	if { [regexp {[ ]} [lrange $tsCfg_backend_logfile 1 1]] || [lrange $tsCfg_backend_logfile 2 2] != ""  } {
	    messageBox "Error: '$descriptor' -  Backend Log File $warning"
	    return 1
	}
    }
    if {$tsCfg_tgtNameIP != ""} {
	if { [regexp {[ ]} $tsCfg_tgtNameIP] == 1} {
	    messageBox "Error: '$descriptor' - Target Name/ IP Address $warning"
	    return 1
	}
    }

       return 0
}



#======================================================================
# change or click routines for main dialog
#
#======================================================================
proc on_tgtsvr_name_text {args} {
    global tsCfg_serverName

    set value [controlValuesGet tsCfgDlg.tgtsvr_name_text]
    if { $value == "" } {
	set tsCfg_serverName ""
    } else {
	set tsCfg_serverName [concat -n $value]
 	}
    tgtsvr_update_command_line
}

#======================================================================
#
# isNumeric  - returns 1 if value is empty or numeric, else returns 0
#
#======================================================================
proc isNumeric {value} {

    if {$value == ""} {
	return 1
    }

    regexp {^[0-9]*} $value matching
    if {![string match $value $matching]} {
	return 0
    } else {
	return 1
    }
}


#----------------------------------------------------------
proc on_tgtsvr_menu_item {args} {
    global tsCfg_commandLine
    global tsCfg_currentDescr
    global tsCfg_menuItem

    if {[controlChecked tsCfgDlg.tgtsvr_menuItem_bool]} {
		set tsCfg_menuItem 1
    } else {
		set tsCfg_menuItem 0
    }

    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgt_name_ipadds {args} {
    global tsCfg_tgtNameIP

    set value [controlValuesGet tsCfgDlg.tgt_name_ip_text]
    if { $value == "" } {
		set tsCfg_tgtNameIP ""
    } else {
		set tsCfg_tgtNameIP $value
 	}
    tgtsvr_update_command_line

}


#=====================================================================
# backend_selection click routines
#
#	The folloing section comprises the various click routines
#	that were specified by the control create statements for the
#	backend selection controls.
#
#=====================================================================

proc on_tgtsvr_backend_available {args} {
    global tsCfg_backendType
    global tsCfg_baudRate
    global tsCfg_device

    set args [controlEventGet tsCfgDlg.tgtsvr_backend_available_list]
    if {$args == "selchange"} {
		set bkEndName \
		[controlSelectionGet tsCfgDlg.tgtsvr_backend_available_list -string]
		set tsCfg_backendType "-B $bkEndName"
		display_backend_config
    }
    tgtsvr_update_command_line

}

#----------------------------------------------------------
proc on_tgtsvr_backend_timeout {args} {
    global tsCfg_backendTimeout
    global tsCfg_timeout_tmp

    set value [controlValuesGet tsCfgDlg.tgtsvr_backend_timeout_text]
    if {![isNumeric $value]} {
	messageBox "Invalid value, please enter a number."
	controlValuesSet tsCfgDlg.tgtsvr_backend_timeout_text $tsCfg_timeout_tmp
	return
    }

    set txCfg_retry_tmp $value

    if { $value == "" } {
	set tsCfg_backendTimeout ""
    } else {
	set tsCfg_backendTimeout [concat -Bt $value]
 	}
    tgtsvr_update_command_line
    
}

#----------------------------------------------------------
proc on_tgtsvr_backend_baud {args} {
    global tsCfg_baudRate

    set args [controlEventGet tsCfgDlg.tgtsvr_backend_baud_combo]
    if {$args == "selchange"} {
	set bps [controlSelectionGet tsCfgDlg.tgtsvr_backend_baud_combo -string]
	if {$bps != 9600} {
	    set tsCfg_baudRate  "-b $bps"
	} else {
	    # bps is the default, set to null the baudRate
	    set tsCfg_baudRate  ""
	}
    }
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_backend_device {args} {
    global tsCfg_device

    set args [controlEventGet tsCfgDlg.tgtsvr_backend_device_combo]
    if {$args == "selchange"} {
	set device [controlSelectionGet tsCfgDlg.tgtsvr_backend_device_combo -string]
	if {![string match $device "COM2"]} {
	    set tsCfg_device "-d $device"
	} else {
	    set tsCfg_device ""
	}
    }
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_backend_resend {args} {
    global tsCfg_backendResendNumber
    global tsCfg_retry_tmp

    set value [controlValuesGet tsCfgDlg.tgtsvr_backend_resend_text]
    if {![isNumeric $value]} {
	messageBox "Invalid value, please enter a number."
	controlValuesSet tsCfgDlg.tgtsvr_backend_resend_text $tsCfg_retry_tmp
	return
    }

    set txCfg_retry_tmp $value

    if { $value == "" } {
	set tsCfg_backendResendNumber ""
    } else {
	set tsCfg_backendResendNumber [concat -Br $value]
 	}
    tgtsvr_update_command_line
}



#----------------------------------------------------------
proc on_tgtsvr_backend_opt {args} {
    global tsCfg_backendOpt

	set value [controlValuesGet tsCfgDlg.tgtsvr_backend_opt_text]
	set value [string trim $value]
	if {$value == ""} {
		set tsCfg_backendOpt {}
	} else {
		set tsCfg_backendOpt [concat "" $value]
	}
    tgtsvr_update_command_line
}

#=====================================================================
# coreFile click routines
#=====================================================================

#----------------------------------------------------------
proc on_tgtsvr_targetPath_choice {args} {
    global tsCfg_coreFileName

    controlEnable tsCfgDlg.tgtsvr_coreFile_text 0
    set tsCfg_coreFileName ""
    controlValuesSet tsCfgDlg.tgtsvr_coreFile_text ""
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_coreFile_choice {args} {
    global tsCfg_coreFileName

	if {[controlChecked tsCfgDlg.tgtsvr_coreFile_choice]} {
    	controlEnable tsCfgDlg.tgtsvr_coreFile_text 1
    	controlEnable tsCfgDlg.tgtsvr_coreFilePathBrowse_button 1
	} else {
    	controlEnable tsCfgDlg.tgtsvr_coreFile_text 0
    	controlEnable tsCfgDlg.tgtsvr_coreFilePathBrowse_button 0
	}
    if {[controlValuesGet tsCfgDlg.tgtsvr_coreFile_text] != ""} {
		set value [controlValuesGet tsCfgDlg.tgtsvr_coreFile_text]
		set tsCfg_coreFileName [concat -c $value]	
    } else {
		set tsCfg_coreFileName [concat -c "myCoreFile"]
		controlValuesSet tsCfgDlg.tgtsvr_coreFile_text "myCoreFile"
    }
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_coreFile_text {args} {
    global tsCfg_coreFileName

    set value [controlValuesGet tsCfgDlg.tgtsvr_coreFile_text]
    if {[controlChecked tsCfgDlg.tgtsvr_coreFile_choice]} {
        if {[string match "-c" [lindex $value 0]]} {
	    	set tsCfg_coreFileName $value
        } else {
	    	set tsCfg_coreFileName [concat -c $value]
        }
    } else {
		set tsCfg_coreFileName ""
    }
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_coreFilePathBrowse {args} {

    set pathName [join [fileDialogCreate  -filefilters "All Files (*.*)|*.*||" -pathmustexist \
	-filemustexist -title "Core File" -okbuttontitle "Select" -nochangedir]]
    if {$pathName != ""} {
		controlValuesSet tsCfgDlg.tgtsvr_coreFile_text $pathName
		# this should trigger on_tgtsvr_coreFile_text, and 
		# set tsCfg_coreFileName
    }
}


#----------------------------------------------------------
proc on_tgtsvr_allSymbols {args} {
    global tsCfg_allsyms
    global tsCfg_nosyms

    if {[controlChecked tsCfgDlg.tgtsvr_allSymbols_choice]} {
	set tsCfg_allsyms -A
	set tsCfg_nosyms ""
    } else {
	set tsCfg_allsyms ""
    }	
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_noSymbols {args} {
    global tsCfg_nosyms
    global tsCfg_allsyms

    if {[controlChecked tsCfgDlg.tgtsvr_noSymbols_choice]} {
	set tsCfg_nosyms -N
	# turn off all symbol choice value
	set  tsCfg_allsyms ""
    } else {
	set tsCfg_nosyms ""
    }	
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_globalSymbols {args} {
    global tsCfg_nosyms
    global tsCfg_allsyms

    if {[controlChecked tsCfgDlg.tgtsvr_globalSymbols_choice]} {
	# turn off all symbol choice value
	set tsCfg_nosyms ""
	set tsCfg_allsyms ""
    }
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_synchro {args} {
    global tsCfg_synchro
    global tsCfg_currentSelection

    if { [controlChecked tsCfgDlg.tgtsvr_synchro_bool] } {
        set tsCfg_synchro -s
        controlCheckSet tsCfgDlg.tgtsvr_synchro_bool -noevent 1
    } else {
        set tsCfg_synchro ""
        controlCheckSet tsCfgDlg.tgtsvr_synchro_bool -noevent 0
    }
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_omf_text {args} {
	global tsCfg_omf

    set value [controlValuesGet tsCfgDlg.tgtsvr_omf_text]
    if { $value == "" } {
		set tsCfg_omf ""
    } else {
		set tsCfg_omf [concat -f $value]
 	}

	tgtsvr_update_command_line
}


#=====================================================================
# Target Server File System click routines
#=====================================================================

#----------------------------------------------------------
proc on_tgtsvr_FS_path_bool {args} {
	global tsCfg_FS_path

       	
   	if {[controlChecked tsCfgDlg.tgtsvr_FS_path_bool]} {
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_text 1
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_button 1
		controlEnable tsCfgDlg.tgtsvr_FS_read_choice   1
		controlEnable tsCfgDlg.tgtsvr_FS_rw_choice     1

		if {$tsCfg_FS_path == ""} {
			# give a default root path, make it readonly
			set tsCfg_FS_path [concat -R "C:\\"]
			controlValuesSet tsCfgDlg.tgtsvr_FS_rootPath_text "C:\\"
			controlCheckSet tsCfgDlg.tgtsvr_FS_rw_choice 1
		}
	} else {
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_text 0
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_button 0
		controlEnable tsCfgDlg.tgtsvr_FS_read_choice   0
		controlEnable tsCfgDlg.tgtsvr_FS_rw_choice     0

		controlValuesSet tsCfgDlg.tgtsvr_FS_rootPath_text ""
		controlCheckSet tsCfgDlg.tgtsvr_FS_read_choice 0 
		controlCheckSet tsCfgDlg.tgtsvr_FS_rw_choice 0
		set tsCfg_FS_path ""
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_FS_rootPath_text {args} {
	global tsCfg_FS_path

	set value [controlValuesGet tsCfgDlg.tgtsvr_FS_rootPath_text]
	set value [string trim $value]
	if {$value != ""} {
		if {[controlChecked tsCfgDlg.tgtsvr_FS_path_bool]} {
	    	if {[string match "-R" [lindex $value 0]]} {
				set tsCfg_FS_path $value
	    	} else {
				set tsCfg_FS_path [concat -R $value]
	    	}
            if {[controlChecked tsCfgDlg.tgtsvr_FS_rw_choice]} {
	            set tsCfg_FS_path [concat $tsCfg_FS_path -RW]
	        }
	    }
	} else {
		set tsCfg_FS_path ""
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_FS_rootPath_button {args} {
	global tsCfg_FS_path

    set dName [dirBrowseDialogCreate -title "Select Directory" ]
    if {$dName != ""} {
         controlTextSet tsCfgDlg.tgtsvr_FS_rootPath_text $dName
    }
}

#----------------------------------------------------------
proc on_tgtsvr_FS_rw_choice {args} {
	global tsCfg_FS_path
	set value [controlValuesGet tsCfgDlg.tgtsvr_FS_rootPath_text]
	set value [string trim $value]
	if {$value != ""} {
    	if {[controlChecked tsCfgDlg.tgtsvr_FS_rw_choice]} {
			set tsCfg_FS_path [concat -R $value -RW]
		} else {
			set tsCfg_FS_path [concat -R $value]
		}
    }

	tgtsvr_update_command_line
}


#=====================================================================
# Console and redirection click routines
#=====================================================================

#----------------------------------------------------------
proc on_tgtsvr_redirectIO_bool {} {
	global tsCfg_redirectIO

	if {[controlChecked tsCfgDlg.tgtsvr_redirectIO_bool]} {
		set tsCfg_redirectIO [concat "" "-redirectIO"]
	} else {
		set tsCfg_redirectIO "" 
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_console_bool {} {
	global tsCfg_console
	global tsCfg_redirectShell

	if {[controlChecked tsCfgDlg.tgtsvr_console_bool]} {
		controlEnable tsCfgDlg.tgtsvr_redirectShell_bool 1
		set tsCfg_console [concat "" "-C"]
	} else {
		controlCheckSet tsCfgDlg.tgtsvr_redirectShell_bool 0
		controlEnable tsCfgDlg.tgtsvr_redirectShell_bool 0
		set tsCfg_console ""
		set tsCfg_redirectShell "" 
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_redirectShell_bool {} {
	global tsCfg_redirectShell

	if {[controlChecked tsCfgDlg.tgtsvr_redirectShell_bool]} {
		set tsCfg_redirectShell [concat "" "-redirectShell"]
	} else {
		set tsCfg_redirectShell "" 
	}

	tgtsvr_update_command_line
}

#=====================================================================
# memory cache size click routines
#=====================================================================
proc on_tgtsvr_memCacheDefault {args} {
    global  tsCfg_memCacheSize

    if {![controlChecked tsCfgDlg.tgtsvr_memCacheDefault_choice]} {
		return
	}

    controlEnable tsCfgDlg.tgtsvr_memCache_text 0
    if {$tsCfg_memCacheSize != ""} {
		set tsCfg_memCacheSize ""
		controlValuesSet tsCfgDlg.tgtsvr_memCache_text ""
    }
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_memCache {args} {
    global  tsCfg_memCacheSize

    if {![controlChecked tsCfgDlg.tgtsvr_memCache_choice]} {
		return
	}
    controlEnable tsCfgDlg.tgtsvr_memCache_text 1

    if {[lindex $tsCfg_memCacheSize 1] != "" } {
		set totalCache [lindex $tsCfg_memCacheSize 1]
		set size [expr $totalCache / 1024]
		controlValuesSet tsCfgDlg.tgtsvr_memCache_text $size 
    } else {
		set size 1024
		set totalCache [expr $size * 1024]
		controlValuesSet tsCfgDlg.tgtsvr_memCache_text 1024
    }
    set tsCfg_memCacheSize [concat -m $totalCache]
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_memCache_text {args} {
    global  tsCfg_memCacheSize
    global tsCfg_cacheSize_tmp

    if {[controlChecked tsCfgDlg.tgtsvr_memCache_choice]} {

		set text [controlValuesGet tsCfgDlg.tgtsvr_memCache_text]
		if {![isNumeric $text]} {
	    	messageBox "Invalid value, please enter a number."
	    	controlValuesSet tsCfgDlg.tgtsvr_memCache_text $tsCfg_cacheSize_tmp
	    	return
		}

		set tsCfg_cacheSize_tmp $text

		if {$text != ""} {
	    	set text [expr $text * 1024]
		} else {
	    	set text [expr 1024 * 1024]
		}

		if {[controlChecked tsCfgDlg.tgtsvr_memCache_choice] == 1} {
	    	set tsCfg_memCacheSize [concat -m $text]
		} else {
	    	set tsCfg_memCacheSize ""
		}
		tgtsvr_update_command_line
    }
}

#=====================================================================
# authorization click routines
#=====================================================================

proc on_tgtsvr_authNone {args} {
    global tsCfg_usersFileName
    global tsCfg_userLock

    set  tsCfg_usersFileName ""
    set tsCfg_userLock ""
    controlCheckSet tsCfgDlg.tgtsvr_authNone_choice -noevent 1
    controlCheckSet tsCfgDlg.tgtsvr_authLock_choice -noevent 0
    controlValuesSet tsCfgDlg.tgtsvr_authFilePath_text ""
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_authLock {args} {
    global tsCfg_usersFileName
    global tsCfg_userLock

    if {[controlChecked tsCfgDlg.tgtsvr_authLock_choice]} {
	set tsCfg_userLock -L
    } else {
	set tsCfg_userLock ""
    }
    tgtsvr_update_command_line
}


#----------------------------------------------------------
proc on_tgtsvr_authUserIDFile {args} {
    global tsCfg_usersFileName
    global tsCfg_userLock

    set value [controlValuesGet tsCfgDlg.tgtsvr_authFilePath_text]
    controlCheckSet tsCfgDlg.tgtsvr_authNone_choice 0
    if {$value == ""} {
	set tsCfg_usersFileName ""
    } else {
	set tsCfg_usersFileName [concat -u $value]
    }
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_authFilePath_text {args} {
    global tsCfg_usersFileName
    global tsCfg_userLock

    set value [controlValuesGet tsCfgDlg.tgtsvr_authFilePath_text]

    if {$value != ""} {
	set tsCfg_usersFileName [concat -u $value]
    } else {
	set tsCfg_usersFileName ""
    }
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_authFilePath_button {args} {

    set pathName [join [fileDialogCreate  -filefilters "All Files (*.*)|*.*||" -pathmustexist \
	-filemustexist -title "User ID File" -okbuttontitle "Select" -nochangedir]]
    if {$pathName != ""} {
	set tsCfg_usersFileName $pathName
	controlValuesSet tsCfgDlg.tgtsvr_authFilePath_text $tsCfg_usersFileName
    }
}


#=====================================================================
# logging click routines
#=====================================================================

#----------------------------------------------------------
proc on_tgtsvr_backend_log_bool {args} {
    global tsCfg_backend_logfile
    global tsCfg_backend_logsize

   	if {[controlChecked tsCfgDlg.tgtsvr_backend_log_bool]} {
		controlEnable tsCfgDlg.tgtsvr_backend_logfile_text 1
		controlEnable tsCfgDlg.tgtsvr_backend_logfile_button 1
		controlEnable tsCfgDlg.tgtsvr_backend_logsize_text 1
	} else {
		controlEnable tsCfgDlg.tgtsvr_backend_logfile_text 0
		controlEnable tsCfgDlg.tgtsvr_backend_logfile_button 0
		controlEnable tsCfgDlg.tgtsvr_backend_logsize_text 0
		controlValuesSet tsCfgDlg.tgtsvr_backend_logfile_text ""
		controlValuesSet tsCfgDlg.tgtsvr_backend_logsize_text ""
		set tsCfg_backend_logfile ""
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_backend_logfile_text {args} {
    global tsCfg_backend_logfile

    set value [controlValuesGet tsCfgDlg.tgtsvr_backend_logfile_text]
    if { $value == "" } {
		set tsCfg_backend_logfile ""
    } else {
		set tsCfg_backend_logfile [concat -Bd $value]
 	}
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_backend_logfile_button {args} {
    global tsCfg_backend_logfile

    set pathName [join [fileDialogCreate -filefilters "All Files (*.*)|*.*||" \
	-title "Log File" -okbuttontitle "OK" -savefile -overwriteprompt]]
    if {$pathName != ""} {
		set tsCfg_backend_logfile $pathName
		controlValuesSet tsCfgDlg.tgtsvr_backend_logfile_text $tsCfg_backend_logfile
    }

}

#----------------------------------------------------------
proc on_tgtsvr_backend_logsize_text {args} {
    global tsCfg_backend_logsize

    set value [controlValuesGet tsCfgDlg.tgtsvr_backend_logsize_text]
    if { $value == "" } {
		set tsCfg_backend_logsize ""
    } else {
    	if {![isNumeric $value]} {
			messageBox "Invalid value, please enter a number."
			controlValuesSet tsCfgDlg.tgtsvr_backend_logsize_text "" 
			return
		}
		set tsCfg_backend_logsize [concat -Bm $value]
 	}
    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_wtx_log_bool {args} {
    global tsCfg_wtx_logfile

   	if {[controlChecked tsCfgDlg.tgtsvr_wtx_log_bool]} {
		controlEnable tsCfgDlg.tgtsvr_wtx_logfile_text 1
		controlEnable tsCfgDlg.tgtsvr_wtx_logfile_button 1
		controlEnable tsCfgDlg.tgtsvr_wtx_logsize_text 1
		controlEnable tsCfgDlg.tgtsvr_wtx_logfilter_text 1
	} else {
		controlEnable tsCfgDlg.tgtsvr_wtx_logfile_text 0
		controlEnable tsCfgDlg.tgtsvr_wtx_logfile_button 0
		controlEnable tsCfgDlg.tgtsvr_wtx_logsize_text 0
		controlEnable tsCfgDlg.tgtsvr_wtx_logfilter_text 0
		controlValuesSet tsCfgDlg.tgtsvr_wtx_logfile_text ""
		controlValuesSet tsCfgDlg.tgtsvr_wtx_logsize_text ""
		controlValuesSet tsCfgDlg.tgtsvr_wtx_logfilter_text ""
		set tsCfg_backend_logfile ""
	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_wtx_logfile_text {args} {
	global tsCfg_wtx_logfile

    set value [controlValuesGet tsCfgDlg.tgtsvr_wtx_logfile_text]
    if { $value == "" } {
		set tsCfg_wtx_logfile ""
    } else {
		set tsCfg_wtx_logfile [concat -Wd $value]
 	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_wtx_logfile_button {args} {
    global tsCfg_wtx_logfile

    set pathName [join [fileDialogCreate -filefilters "All Files (*.*)|*.*||" \
	-title "Log File" -okbuttontitle "OK" -savefile -overwriteprompt]]
    if {$pathName != ""} {
		set tsCfg_wtx_logfile $pathName
		controlValuesSet tsCfgDlg.tgtsvr_wtx_logfile_text $tsCfg_wtx_logfile
    }

}

#----------------------------------------------------------
proc on_tgtsvr_wtx_logsize_text {args} {
	global tsCfg_wtx_logsize

    set value [controlValuesGet tsCfgDlg.tgtsvr_wtx_logsize_text]
    if { $value == "" } {
		set tsCfg_wtx_logsize ""
    } else {
    	if {![isNumeric $value]} {
			messageBox "Invalid value, please enter a number."
			controlValuesSet tsCfgDlg.tgtsvr_wtx_logsize_text "" 
			return
		}
		set tsCfg_wtx_logsize [concat -Wm $value]
 	}

	tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_wtx_logfilter_text {args} {
	global tsCfg_wtx_logfilter

    set value [controlValuesGet tsCfgDlg.tgtsvr_wtx_logfilter_text]
    if { $value == "" } {
		set tsCfg_wtx_logfilter ""
    } else {
		set tsCfg_wtx_logfilter [concat -Wf $value]
 	}

	tgtsvr_update_command_line
}

#=====================================================================
# Miscellaneous click routines
#=====================================================================

#----------------------------------------------------------
proc on_tgtsvr_misc_usepm_bool {args} {
	global tsCfg_usepm

   	if {[controlChecked tsCfgDlg.tgtsvr_misc_usepm_bool]} {
		set tsCfg_usepm "-use_portmapper" 
    } else {
		set tsCfg_usepm ""
 	}

	tgtsvr_update_command_line
}

#=====================================================================
# Target memory pool
#=====================================================================
proc on_tgtsvr_memNoGrowth {args} {
    global tsCfg_memNoGrowth

    if {[controlChecked tsCfgDlg.tgtsvr_memory_growth_bool]} {
	set tsCfg_memNoGrowth "-noG" 
    } else {
	set tsCfg_memNoGrowth ""
    }

    tgtsvr_update_command_line
}

#----------------------------------------------------------
proc on_tgtsvr_misc_opt {args} {
    global tsCfg_miscOpt

    set value [controlValuesGet tsCfgDlg.tgtsvr_misc_opt_text]
    set value [string trim $value]
    if {$value == ""} {
        set tsCfg_miscOpt {}
    } else {
        set tsCfg_miscOpt [concat "" $value]
    }
    tgtsvr_update_command_line
}

#=====================================================================
# launch click routines
#=====================================================================
proc on_tgtsvr_launch {args} {
    global tsCfg_commandLine
    global tsCfg_currentDescr
    global tsCfg_regEntries
    global tsCfg_debugMode

 	# save the
    save_in_tsCfg_arrays $tsCfg_currentDescr

    tgtsvr_update_command_line

    # check for "correctness"
    set descriptor [ts_checkForCorrectness]
    tgtsvr_restore_config $tsCfg_currentDescr
    if {$descriptor != ""} {
	return
    }

	targetServerCustomLaunch $tsCfg_commandLine

    on_tgtsvr_OK 1
}


#=====================================================================
# tgtsvr_save_entries
#=====================================================================
proc tgtsvr_save_entries {args} {
    global tsCfg_currentDescr
    global tsCfg_regEntries
    global tsCfg_descrList
        global tsCfg_arrays


    if {$args == ""} {
		# delete previous registry contents
		if {$tsCfg_regEntries != ""} {
	    	foreach regEntry $tsCfg_regEntries {
				appRegistryEntryDelete Target $regEntry
	    	}
		}

		# save all current descriptors to the registry
		write_all_tgtsvr_descriptions

		# save last target selected
		set selected [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]
		if {$selected != ""} {
			appRegistryEntryWrite Target lastTgtsvrSaved $selected
		} else {
			appRegistryEntryWrite Target lastTgtsvrSaved ""
		}
    } else {
		# save only the descriptor passed to the registry
		appRegistryEntryWrite Target $args $tsCfg_arrays($args)
    }
    
    # update the list of current registry entries
    set tsCfg_regEntries $tsCfg_descrList
}


#---------------------------------------------------------------------
# tgtsvr_update_command_line
#
#---------------------------------------------------------------------
proc tgtsvr_update_command_line {args} {
    global listBackends
    global tsCfg_tgtsvr
    global tsCfg_commandLine
    global tsCfg_tgtNameIP
    global tsCfg_serverName
    global tsCfg_usersFileName
    global tsCfg_userLock
    global tsCfg_coreFileName
    global tsCfg_objecFormat
    global tsCfg_nosyms
    global tsCfg_allsyms
    global tsCfg_synchro
    global tsCfg_baudRate
    global tsCfg_hostTtyDevice
    global tsCfg_verboseMode
    global tsCfg_consoleMode
    global tsCfg_displayHost
    global tsCfg_memCacheSize
    global tsCfg_backendType
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_backend_logfile
    global tsCfg_FS_path
    global tsCfg_device
    global tsCfg_version
    global tsCfg_currentSelection
    global tsCfg_displayHost
    global tsCfg_descrList
    global tsCfg_backendOpt
    global tsCfg_redirectIO 
    global tsCfg_console 
    global tsCfg_redirectShell
    global tsCfg_backend_logsize
    global tsCfg_wtx_logfile
    global tsCfg_wtx_logsize
    global tsCfg_wtx_logfilter
    global tsCfg_omf 
    global tsCfg_usepm 
    global tsCfg_memNoGrowth
    global tsCfg_miscOpt
    global tsCfg_debugMode

    if {$tsCfg_descrList == ""} {
		set tsCfg_commandLine ""
		return
    }

    if {$tsCfg_tgtsvr == ""} {
		if {$tsCfg_debugMode} {
    		set tsCfg_tgtsvr "tgtsvr-d.exe"
		} else {
    		set tsCfg_tgtsvr "tgtsvr.exe"
		}
    }

    set backendType [lindex $tsCfg_backendType 1]

    # Use values set by the controls and form the command line:
	# (any empty values should be removed with the concat command)
    set tsCfg_commandLine [concat \
		$tsCfg_tgtsvr $tsCfg_tgtNameIP $tsCfg_serverName \
		$tsCfg_usersFileName $tsCfg_userLock \
		$tsCfg_objecFormat \
		$tsCfg_nosyms $tsCfg_allsyms $tsCfg_synchro \
		$tsCfg_hostTtyDevice $tsCfg_verboseMode \
		$tsCfg_consoleMode $tsCfg_memCacheSize \
		$tsCfg_backendType \
		$tsCfg_backendTimeout $tsCfg_backendResendNumber \
		$tsCfg_backend_logfile $tsCfg_baudRate $tsCfg_device \
		$tsCfg_backendOpt \
		$tsCfg_version $tsCfg_displayHost \
		$tsCfg_FS_path \
		$tsCfg_redirectIO $tsCfg_console $tsCfg_redirectShell \
		$tsCfg_backend_logsize \
		$tsCfg_wtx_logfile $tsCfg_wtx_logsize $tsCfg_wtx_logfilter \
		$tsCfg_omf $tsCfg_usepm $tsCfg_memNoGrowth $tsCfg_miscOpt \
		$tsCfg_coreFileName] 

    if {[llength $tsCfg_commandLine] == 1} {
	    set tsCfg_commandLine ""
    }

    # display the command line on the dialog 
    if {$tsCfg_currentSelection != -1} {
		controlValuesSet tsCfgDlg.tgtsvr_commandLine_text $tsCfg_commandLine
    }
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# hide_backend_controls - hides all controls for backends
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc hide_backend_controls {args} {
	hide_backend_specific_controls
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# hide_backend_specific_controls - hides controls specific to backends
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc hide_backend_specific_controls {} {
	config_chk_hide_common_controls
	config_chk_hide_serial_controls
	config_chk_hide_unknown_controls
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# config_chk_hide_common_controls - controls common to wdbrpc,wdbpipe
#												wdbserial & netrom
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc config_chk_hide_common_controls {args} {
	controlHide tsCfgDlg.tgtsvr_backend_timeout_label 1
	controlHide tsCfgDlg.tgtsvr_backend_timeout_text 1
	controlHide tsCfgDlg.tgtsvr_backend_resend_label 1
	controlHide tsCfgDlg.tgtsvr_backend_resend_text 1
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# config_chk_hide_serial_controls
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc config_chk_hide_serial_controls {args} {
	controlHide tsCfgDlg.tgtsvr_backend_baud_label 1
	controlHide tsCfgDlg.tgtsvr_backend_baud_combo 1
	controlHide tsCfgDlg.tgtsvr_backend_device_label 1
	controlHide tsCfgDlg.tgtsvr_backend_device_combo 1
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# config_chk_hide_unknown_controls - controls for unknown backends 
#												
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc config_chk_hide_unknown_controls {args} {
	controlHide tsCfgDlg.tgtsvr_backend_opt_label 1
	controlHide tsCfgDlg.tgtsvr_backend_opt_text 1
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_backend_serial
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_backend_serial {args} {
    global tsCfg_device
    global tsCfg_baudRate
    global tsCfg_baud_rates
    global tsCfg_deviceList
    global tsCfg_descrList

	# port and speed
	controlHide tsCfgDlg.tgtsvr_backend_baud_label 0
	controlHide tsCfgDlg.tgtsvr_backend_baud_combo 0
	controlHide tsCfgDlg.tgtsvr_backend_device_label 0
	controlHide tsCfgDlg.tgtsvr_backend_device_combo 0

    if {$tsCfg_descrList != ""}	{
		controlValuesSet tsCfgDlg.tgtsvr_backend_device_combo $tsCfg_deviceList
		if {$tsCfg_device != ""} {
	    	set indx [lsearch $tsCfg_deviceList [lindex $tsCfg_device 1]]
	    	controlSelectionSet tsCfgDlg.tgtsvr_backend_device_combo \
				-noevent $indx
		} else {
	    	controlSelectionSet tsCfgDlg.tgtsvr_backend_device_combo -noevent 1
	    	# default value is COM2, no need to display in commmand line : 
			# set tsCfg_device "-d COM2"
	    	set tsCfg_device ""
		}

		controlValuesSet tsCfgDlg.tgtsvr_backend_baud_combo $tsCfg_baud_rates
		if {$tsCfg_baudRate != ""} {
	    	controlSelectionSet tsCfgDlg.tgtsvr_backend_baud_combo \
				-noevent -string [lindex $tsCfg_baudRate 1]
		} else {
	    	controlSelectionSet tsCfgDlg.tgtsvr_backend_baud_combo -noevent 0
	    	# default is 9600, no need to display in command line
	    	set tsCfg_baudRate ""
		}
    } else {
     	controlValuesSet tsCfgDlg.tgtsvr_backend_device_combo ""
		controlValuesSet tsCfgDlg.tgtsvr_backend_baud_combo  ""
    }
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_backend_known - display timeout and retry 
#								selection for known backends 
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_backend_known {args} {
    global tsCfg_descrList
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
	global tsCfg_backendOpt

	# reset vars that don't apply:
	set tsCfg_backendOpt ""	

	# timeout and retry
	controlHide tsCfgDlg.tgtsvr_backend_timeout_label 0
	controlHide tsCfgDlg.tgtsvr_backend_timeout_text 0
	controlHide tsCfgDlg.tgtsvr_backend_resend_label 0
	controlHide tsCfgDlg.tgtsvr_backend_resend_text 0

    if {$tsCfg_descrList != ""}	{
 		if {$tsCfg_backendTimeout != "" } {
	    	controlValuesSet tsCfgDlg.tgtsvr_backend_timeout_text \
							[lindex $tsCfg_backendTimeout 1]
		} else {
    	    controlValuesSet tsCfgDlg.tgtsvr_backend_timeout_text ""
		}

		if {$tsCfg_backendResendNumber != ""} {
		    controlValuesSet tsCfgDlg.tgtsvr_backend_resend_text \
										[lindex $tsCfg_backendResendNumber 1]
		} else {
		    controlValuesSet tsCfgDlg.tgtsvr_backend_resend_text ""
		}
	} else {
    	controlValuesSet tsCfgDlg.tgtsvr_backend_timeout_text ""
		controlValuesSet tsCfgDlg.tgtsvr_backend_resend_text ""
	}
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_backend_unknown - display unknown option
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_backend_unknown {args} {
    global tsCfg_descrList
    global tsCfg_backendOpt
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_device
    global tsCfg_baudRate

	# reset vars that don't apply:
	set tsCfg_backendTimeout ""	
	set tsCfg_backendResendNumber ""	
    set tsCfg_device ""
    set tsCfg_baudRate ""

	# extra option 
	controlHide tsCfgDlg.tgtsvr_backend_opt_label 0
	controlHide tsCfgDlg.tgtsvr_backend_opt_text 0

    if {$tsCfg_descrList != ""}	{
		if {$tsCfg_backendOpt != ""} {
			controlValuesSet tsCfgDlg.tgtsvr_backend_opt_text $tsCfg_backendOpt 
		} else {
			controlValuesSet tsCfgDlg.tgtsvr_backend_opt_text ""
		}
	} else {
		controlValuesSet tsCfgDlg.tgtsvr_backend_opt_text ""
	}
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_common - display  tgt name, tgtsvr name and 
#					if it's a menu item
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_common {args} {
    global tsCfg_tgtNameIP
	global tsCfg_serverName
    global tsCfg_menuItem

	# display/select tgt name or ip chosen 
	controlValuesSet tsCfgDlg.tgt_name_ip_text $tsCfg_tgtNameIP

	# display/select tgtsvr name or ip chosen 
	if {$tsCfg_serverName != ""} {
	 	controlValuesSet tsCfgDlg.tgtsvr_name_text \
									[lreplace $tsCfg_serverName 0 0]
	} else {
	        controlValuesSet tsCfgDlg.tgtsvr_name_text ""
	}

	# display/select if its a menu item
    if {$tsCfg_menuItem != 0} {
		controlCheckSet tsCfgDlg.tgtsvr_menuItem_bool 1
    } else {
		controlCheckSet tsCfgDlg.tgtsvr_menuItem_bool 0
    }
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_backend_config
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_backend_config {args} {
    global tsCfg_descrList
	global tsCfg_backendType
	global tsCfg_currentSelection
	global tsCfg_backendTimeout
	global tsCfg_backendResendNumber
	global tsCfg_backend_logfile
	global tsCfg_serverName
	global tsCfg_tgtNameIP
	global tsCfg_backendOpt
	global listBackends 

    if {$tsCfg_currentSelection == 1} {
		set config [lindex $tsCfg_backendType 1]

		# display/select backend chosen 
		controlSelectionSet tsCfgDlg.tgtsvr_backend_available_list \
			-noevent -string $config

		# hide specific controls to begin with:
		hide_backend_specific_controls	

		# if a backend is selected, display its options:
		if {$config!=""} {

		  # serial options:
    	  if {[lsearch -exact $listBackends(serial) $config] >= 0 } {
	    	display_backend_serial
		  }

		  # resend, and timeout:
    	  if {[lsearch -exact $listBackends(known) $config] >= 0 } {
			display_backend_known
		  }

		  # misc. backend option:
    	  if {[lsearch -exact $listBackends(known) $config] < 0 } {
			display_backend_unknown
		  }
		}


    } else {    # not backend controls, hide the backend controls
		hide_backend_controls	
    }
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_corefile
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_corefile {args} {
    global tsCfg_coreFileName
    global tsCfg_allsyms
    global tsCfg_nosyms
    global tsCfg_synchro
	global tsCfg_omf

    if {$tsCfg_coreFileName != ""} {
    	set fileName [string trim $tsCfg_coreFileName]
		set fileName [string range $fileName 3 end]
		controlCheckSet tsCfgDlg.tgtsvr_coreFile_choice 1
		controlCheckSet tsCfgDlg.tgtsvr_targetPath_choice 0
		controlEnable tsCfgDlg.tgtsvr_coreFile_text 1
		controlValuesSet tsCfgDlg.tgtsvr_coreFile_text $fileName
    } else {
		controlEnable tsCfgDlg.tgtsvr_coreFile_text 0
		controlCheckSet tsCfgDlg.tgtsvr_targetPath_choice 1
		controlCheckSet tsCfgDlg.tgtsvr_coreFile_choice 0
		controlValuesSet tsCfgDlg.tgtsvr_coreFile_text ""
    }

    if {$tsCfg_allsyms != ""} {
	controlCheckSet tsCfgDlg.tgtsvr_allSymbols_choice 1
	controlCheckSet tsCfgDlg.tgtsvr_noSymbols_choice 0
	controlCheckSet tsCfgDlg.tgtsvr_globalSymbols_choice 0
    }		

    if {$tsCfg_nosyms != ""} {
	controlCheckSet tsCfgDlg.tgtsvr_noSymbols_choice 1
	controlCheckSet tsCfgDlg.tgtsvr_allSymbols_choice 0
	controlCheckSet tsCfgDlg.tgtsvr_globalSymbols_choice 0
    }

    if {$tsCfg_nosyms == "" && $tsCfg_allsyms == ""} {
	controlCheckSet tsCfgDlg.tgtsvr_globalSymbols_choice 1
	controlCheckSet tsCfgDlg.tgtsvr_allSymbols_choice 0
	controlCheckSet tsCfgDlg.tgtsvr_noSymbols_choice 0
    }

    if {[string match $tsCfg_synchro "-s"]} {
	controlCheckSet tsCfgDlg.tgtsvr_synchro_bool 1
    } else {
	controlCheckSet tsCfgDlg.tgtsvr_synchro_bool 0
    }


	if {$tsCfg_omf != ""} {
    	set fmt [string trim $tsCfg_omf]
		set fmt [string range $fmt 3 end]
	    controlValuesSet tsCfgDlg.tgtsvr_omf_text $fmt
	} else {
	    controlValuesSet tsCfgDlg.tgtsvr_omf_text "" 
	}

    tgtsvr_update_command_line
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_TSFS
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_TSFS {args} {
     global tsCfg_FS_path

	if {$tsCfg_FS_path == ""} {
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_text 0
		controlEnable tsCfgDlg.tgtsvr_FS_read_choice   0
		controlEnable tsCfgDlg.tgtsvr_FS_rw_choice     0

		controlCheckSet tsCfgDlg.tgtsvr_FS_path_bool 0
        controlValuesSet tsCfgDlg.tgtsvr_FS_rootPath_text ""
    } else {
		controlEnable tsCfgDlg.tgtsvr_FS_rootPath_text 1
		controlEnable tsCfgDlg.tgtsvr_FS_read_choice   1
		controlEnable tsCfgDlg.tgtsvr_FS_rw_choice     1
		controlCheckSet tsCfgDlg.tgtsvr_FS_path_bool 1

    	set rootName [string trim $tsCfg_FS_path]
		set rootName [string range $rootName 3 end] ;# get rid of -R

        if {[string match {* -RW} $rootName]} {
			# get rid of -RW:
			set upto [string first " -RW" $rootName] 
			incr upto -1
			set rootName [string range $rootName 0 $upto] 
			controlValuesSet tsCfgDlg.tgtsvr_FS_rootPath_text $rootName
		    controlCheckSet tsCfgDlg.tgtsvr_FS_rw_choice     1
		    controlCheckSet tsCfgDlg.tgtsvr_FS_read_choice   0
		} else {
			controlValuesSet tsCfgDlg.tgtsvr_FS_rootPath_text $rootName
		    controlCheckSet tsCfgDlg.tgtsvr_FS_rw_choice     0
		    controlCheckSet tsCfgDlg.tgtsvr_FS_read_choice   1
        }
     }

    tgtsvr_update_command_line
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_consoleRedirection
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_consoleRedirection {args} {
    global tsCfg_console
    global tsCfg_redirectIO
    global tsCfg_redirectShell

	if {$tsCfg_redirectIO == ""} {
		controlCheckSet tsCfgDlg.tgtsvr_redirectIO_bool 0
	} else {
		controlCheckSet tsCfgDlg.tgtsvr_redirectIO_bool 1
	}

	if {$tsCfg_console == ""} {
		controlCheckSet tsCfgDlg.tgtsvr_console_bool 0
		controlCheckSet tsCfgDlg.tgtsvr_redirectShell_bool 0
		controlEnable tsCfgDlg.tgtsvr_redirectShell_bool 0
	} else {
		controlCheckSet tsCfgDlg.tgtsvr_console_bool 1 
		controlEnable tsCfgDlg.tgtsvr_redirectShell_bool 1 
		if {$tsCfg_redirectShell != ""} {
			controlCheckSet tsCfgDlg.tgtsvr_redirectShell_bool 1 
		}
	}

}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_memCacheSize
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_memCacheSize {args} {
    global tsCfg_memCacheSize

    if {$tsCfg_memCacheSize != ""} {
		controlCheckSet tsCfgDlg.tgtsvr_memCacheDefault_choice 0
		controlCheckSet tsCfgDlg.tgtsvr_memCache_choice 1
    } else {
		controlCheckSet tsCfgDlg.tgtsvr_memCache_choice 0
		controlCheckSet tsCfgDlg.tgtsvr_memCacheDefault_choice 1
		controlValuesSet tsCfgDlg.tgtsvr_memCache_text ""
		controlEnable tsCfgDlg.tgtsvr_memCache_text 0
    }
    tgtsvr_update_command_line
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_authorizations
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_authorizations {args} {
    global tsCfg_usersFileName
    global tsCfg_userLock

    if {$tsCfg_usersFileName == "" && $tsCfg_userLock ==  ""} {
	controlCheckSet tsCfgDlg.tgtsvr_authNone_choice 1
	controlValuesSet tsCfgDlg.tgtsvr_authFilePath_text ""
    }
    if {$tsCfg_userLock != ""} {
	controlCheckSet tsCfgDlg.tgtsvr_authLock_choice 1
    } else {
	# turn off the lock choice button
	controlCheckSet tsCfgDlg.tgtsvr_authLock_choice 0
    }
    if {$tsCfg_usersFileName != ""} {
	set filename [lreplace $tsCfg_usersFileName 0 0 ]
	controlValuesSet tsCfgDlg.tgtsvr_authFilePath_text $filename
    } else {
     	controlValuesSet tsCfgDlg.tgtsvr_authFilePath_text ""
    }
}

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_logging
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_logging {args} {
	global tsCfg_backend_logfile
	global tsCfg_backend_logsize
	global tsCfg_wtx_logfile
	global tsCfg_wtx_logsize
	global tsCfg_wtx_logfilter

	if {$tsCfg_backend_logfile != ""} {
		controlCheckSet tsCfgDlg.tgtsvr_backend_log_bool 1
    	set fileName [string trim $tsCfg_backend_logfile]
		set fileName [string range $fileName 4 end]
	    controlValuesSet tsCfgDlg.tgtsvr_backend_logfile_text $fileName 
		if {$tsCfg_backend_logsize != ""} {
    		set fileSize [string trim $tsCfg_backend_logsize]
			set fileSize [string range $fileSize 4 end]
	    	controlValuesSet tsCfgDlg.tgtsvr_backend_logsize_text $fileSize
		}
	} else {
		controlCheckSet tsCfgDlg.tgtsvr_backend_log_bool 0
	}

	if {$tsCfg_wtx_logfile != ""} {
		controlCheckSet tsCfgDlg.tgtsvr_wtx_log_bool 1
    	set fileName [string trim $tsCfg_wtx_logfile]
		set fileName [string range $fileName 4 end]
	    controlValuesSet tsCfgDlg.tgtsvr_wtx_logfile_text $fileName 
		if {$tsCfg_wtx_logsize != ""} {
    		set fileSize [string trim $tsCfg_wtx_logsize]
			set fileSize [string range $fileSize 4 end]
	    	controlValuesSet tsCfgDlg.tgtsvr_wtx_logsize_text $fileSize
		}
		if {$tsCfg_wtx_logfilter != ""} {
    		set filter [string trim $tsCfg_wtx_logfilter]
			set filter [string range $filter 4 end]
	    	controlValuesSet tsCfgDlg.tgtsvr_wtx_logfilter_text $filter
		}
	} else {
		controlCheckSet tsCfgDlg.tgtsvr_wtx_log_bool 0
	}
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_misc
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_misc {args} {
	global tsCfg_usepm
	global tsCfg_miscOpt

	if {$tsCfg_usepm != ""} {
	    controlCheckSet tsCfgDlg.tgtsvr_misc_usepm_bool 1
	} else {
	    controlCheckSet tsCfgDlg.tgtsvr_misc_usepm_bool 0
	}

	if {$tsCfg_miscOpt != ""} {
		controlValuesSet tsCfgDlg.tgtsvr_misc_opt_text $tsCfg_miscOpt
	} else {
		controlValuesSet tsCfgDlg.tgtsvr_misc_opt_text ""
	}
}


#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# display_memory
#
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
proc display_memory {args} {
    global tsCfg_memNoGrowth

    if {$tsCfg_memNoGrowth != ""} {
	controlCheckSet tsCfgDlg.tgtsvr_memory_growth_bool 1
    } else {
	controlCheckSet tsCfgDlg.tgtsvr_memory_growth_bool 0
    }
}


#---------------------------------------------------------------------
# show_tgtsvr_configuration_choices
#
#---------------------------------------------------------------------
proc show_tgtsvr_configuration_choices {ctrlIndx prevSelection} {
    global tsCfg_ctrlList
    global tsCfg_ctrlNames
    global tsCfg_backendType
    global tsCfg_currentSelection
    global tsCfg_backendList
    global tsCfg_timeout_tmp
    global tsCfg_retry_tmp
    global tsCfg_cacheSize_tmp

    if {$prevSelection != $ctrlIndx || $prevSelection != -1} {
		# Un-hide the newly selected configuration
		set index 0
		set ctrlnames $tsCfg_ctrlNames($ctrlIndx)
		foreach ctrl $tsCfg_ctrlList($ctrlIndx) {
	    	set name [lindex $ctrlnames $index]
	    	controlHide tsCfgDlg.$name 0
	    	incr index
		}
    }

	# display the tgt name /ip and tgtsvr name
	display_common

    switch $ctrlIndx {

	0 	{ 	;# authorizations 
	    display_authorizations
	}

	1	{   ;# backends	

	    # display the available backend list
	    controlValuesSet tsCfgDlg.tgtsvr_backend_available_list $tsCfg_backendList

	    # display backend configuration controls
	    display_backend_config
	}

	2	{	;# core file
	    display_corefile
	}

	3	{	;# memory cache size
	    display_memCacheSize
	}

	4	{	;# TSFS
			display_TSFS
	}

	5	{	;# console and redirection
	    display_consoleRedirection
	}

	6	{	;# logging 
	    display_logging
	}

	7   {   ;# miscellaneous 
		display_misc
	}

	8   {   ;# Target Memory 
		display_memory
	}

    }
}

#----------------------------------------------------------------------
# proc show_command_line
#
#----------------------------------------------------------------------
proc show_command_line {args} {
    global tsCfg_ctrlList
    global tsCfg_ctrlNames

    set index 0
    set ctrlnames "tgtsvr_commandLine_label tgtsvr_commandLine_text"

    foreach name $ctrlnames {
        controlHide tsCfgDlg.$name 0
        incr index
    }

}

#----------------------------------------------------------------------
# proc show_last_row
#
#----------------------------------------------------------------------
proc show_last_row {args} {
    global tsCfg_ctrlList
    global tsCfg_ctrlNames

    set index 0
    set ctrlnames "tgtsvr_OK_button tgtsvr_cancel_button tgtsvr_help_button"

    foreach name $ctrlnames {
	controlHide tsCfgDlg.$name 0
	incr index
    }
}

#-----------------------------------------------------------------------
# tgtsvr_hide_config
#
#
#------------------------------------------------------------------------
proc tgtsvr_hide_config {ctrlIndx} {
    global tsCfg_ctrlNames

    if {$ctrlIndx > -1} {
      	foreach ctrl $tsCfg_ctrlNames($ctrlIndx) {	
	    controlHide  tsCfgDlg.$ctrl 1
     	}
	hide_backend_controls	
    }
}


#----------------------------------------------------------------------
# select_configuration_init
#
#	initialize the combo box values for the configuration selection
#
#----------------------------------------------------------------------
proc select_configuration_init {args} {
    global tsCfg_ctrlItem
    global tsCfg_currentSelection
    global tsCfg_maxSelections
    global tsCfg_defaultSelection
    global tsCfg_backendType
    global tsCfg_defaultBackend
    global tsCfg_backendList
    global tsCfg_saved_description
    global tsCfg_descrList

    # set up combo box values
    for {set ix 0}	{$ix < $tsCfg_maxSelections} {incr ix} {
		lappend selectItems $tsCfg_ctrlItem($ix)
    }
    controlValuesSet tsCfgDlg.configure $selectItems
    set prevSelection $tsCfg_currentSelection
    set tsCfg_currentSelection $tsCfg_defaultSelection

    # set up initial view to be backend selection
    if {$tsCfg_descrList != ""} {
	if {$tsCfg_currentSelection == -1} {
	    controlSelectionSet tsCfgDlg.configure -noevent $tsCfg_defaultSelection
	
	} else {
	    controlSelectionSet tsCfgDlg.configure -noevent $tsCfg_currentSelection
	}
    } else {
	set tsCfg_currentSelection $tsCfg_defaultSelection
    }

    show_tgtsvr_configuration_choices $tsCfg_currentSelection $prevSelection
    controlValuesSet tsCfgDlg.tgtsvr_backend_available_list $tsCfg_backendList
    set tsCfg_currentSelection [controlSelectionGet tsCfgDlg.configure]
    if {$tsCfg_descrList != ""} {
	controlSelectionSet tsCfgDlg.tgtsvr_backend_available_list -noevent \
	    [lsearch $tsCfg_backendList [lindex $tsCfg_backendType 1]]
    }
    if {$tsCfg_saved_description != ""} {
	controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent \
	    [lsearch $tsCfg_descrList $tsCfg_saved_description]
    }
}


#----------------------------------------------------------------------
# redraw_config_area
#
#----------------------------------------------------------------------
proc redraw_config_area {ctrlIndx} {
    global tsCfg_currentSelection
    global tsCfg_ctrlList

    if {$ctrlIndx != -1} {
	if {$ctrlIndx != $tsCfg_currentSelection} {
	    # erase the previous area
	    tgtsvr_hide_config $tsCfg_currentSelection
	}
	# draw new selection "window"
	set prevSelection $tsCfg_currentSelection
	set tsCfg_currentSelection $ctrlIndx
	show_tgtsvr_configuration_choices $ctrlIndx $prevSelection
	return 1
    } else {
	return 0
    }
}

#----------------------------------------------------------------------
# on_config_selected
#
#  	The user has selected a configuration item to update or modify
#
#----------------------------------------------------------------------
proc on_config_selected {args} {
    global tsCfg_currentSelection
    global tsCfg_ctrlList
    global tsCfg_ctrlItem
    # See if it is cuurentSelection, if so just update commandLine
    # otherwise need to kill display and display new selection

    set newCtrl [controlSelectionGet tsCfgDlg.configure]
    if {$tsCfg_currentSelection != -1 && $tsCfg_currentSelection != $newCtrl} {

	redraw_config_area $newCtrl
	tgtsvr_update_command_line
    }

}

#=====================================================================
# get_last_saved_tgtsvr_description
#=====================================================================
proc get_last_saved_tgtsvr_description {args} {
    global tsCfg_saved_description
    global tsCfg_currentDescr

    set last_descr [appRegistryEntryRead "TARGET" "lastTgtsvrSaved"]

    if {$last_descr != ""} {
	set tsCfg_saved_description $last_descr
	set tsCfg_currentDescr $last_descr
	tgtsvr_restore_config $last_descr
	controlValuesSet tsCfgDlg.tgtsvr_descr_text $last_descr
    }
}

#=====================================================================
# tgtsv_restore_config
#=====================================================================
proc tgtsvr_restore_config {args} {
    global tsCfg_arrays
    global tsCfg_tgtsvr
    global tsCfg_commandLine
    global tsCfg_tgtNameIP
    global tsCfg_serverName
    global tsCfg_usersFileName
    global tsCfg_userLock
    global tsCfg_coreFileName
    global tsCfg_objecFormat
    global tsCfg_nosyms
    global tsCfg_allsyms
    global tsCfg_synchro
    global tsCfg_baudRate
    global tsCfg_hostTtyDevice
    global tsCfg_verboseMode
    global tsCfg_consoleMode
    global tsCfg_displayHost
    global tsCfg_memCacheSize
    global tsCfg_backendType
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_backend_logfile
    global tsCfg_FS_path
    global tsCfg_device
    global tsCfg_version
    global tsCfg_currentSelection
    global tsCfg_menuItem
    global tsCfg_timeout_tmp
    global tsCfg_retry_tmp
    global tsCfg_cacheSize_tmp
    global tsCfg_redirectChild
    global tsCfg_closeWindowOnExit
    global tsCfg_backendOpt
    global tsCfg_redirectIO 
    global tsCfg_console 
    global tsCfg_redirectShell 
    global tsCfg_backend_logsize
    global tsCfg_wtx_logfile
    global tsCfg_wtx_logsize
    global tsCfg_wtx_logfilter
    global tsCfg_omf
    global tsCfg_miscOpt
    global tsCfg_usepm
    global tsCfg_memNoGrowth

    set description [lindex $args 0]
    set doDisplay [lindex $args 1]

    if {$description != ""} {
	# restore values read to global variables
	# Note: 1)ignore redirectToChild, closeWindowOnExit and Verbose mode
	#	2)if the index is invalid, Tcl should return empty string 	
	#
	set lst [get_values_from_tsCfg_arrays $description]
	set indx 0
	set tsCfg_tgtsvr [lindex $lst $indx]
	incr indx
	set tsCfg_tgtNameIP [lindex $lst $indx]
	incr indx
	set tsCfg_serverName [lindex $lst $indx]
	incr indx
	set tsCfg_usersFileName [lindex $lst $indx]
	incr indx
	set tsCfg_userLock [lindex $lst $indx]
	incr indx
	set tsCfg_coreFileName [lindex $lst $indx]
	incr indx
	set tsCfg_objecFormat [lindex $lst $indx]
	incr indx
	set tsCfg_nosyms [lindex $lst $indx]
	incr indx
	set tsCfg_allsyms [lindex $lst $indx]
	incr indx
	set tsCfg_synchro [lindex $lst $indx]
	incr indx
	set tsCfg_hostTtyDevice [lindex $lst $indx]
	incr indx
	set tsCfg_verboseMode -V 
	incr indx
	set tsCfg_backendTimeout [lindex $lst $indx]
	incr indx
	set tsCfg_consoleMode [lindex $lst $indx]
	incr indx
	set tsCfg_memCacheSize [lindex $lst $indx]
	incr indx
	set tsCfg_backendType [lindex $lst $indx]
	incr indx
	set tsCfg_backendResendNumber [lindex $lst $indx]
	incr indx
	set tsCfg_backend_logfile [lindex $lst $indx]
	incr indx
	set tsCfg_version [lindex $lst $indx]
	incr indx
	set tsCfg_baudRate [lindex $lst $indx]
	incr indx
	set tsCfg_device [lindex $lst $indx]
	incr indx
	set tsCfg_menuItem [lindex $lst $indx]
	incr indx
	set tsCfg_redirectChild "" 
	incr indx
	set tsCfg_closeWindowOnExit "" 
	incr indx
    	set tsCfg_FS_path [lindex $lst $indx]
    	incr indx
	set tsCfg_backendOpt [lindex $lst $indx]
	incr indx
	set tsCfg_redirectIO [lindex $lst $indx]
	incr indx
	set tsCfg_console [lindex $lst $indx] 
	incr indx
	set tsCfg_redirectShell [lindex $lst $indx] 
	incr indx
	set tsCfg_backend_logsize [lindex $lst $indx] 
	incr indx
	set tsCfg_wtx_logfile [lindex $lst $indx] 
	incr indx
	set tsCfg_wtx_logsize [lindex $lst $indx] 
	incr indx
	set tsCfg_wtx_logfilter [lindex $lst $indx] 
	incr indx
	set tsCfg_omf [lindex $lst $indx] 
	incr indx
	set tsCfg_usepm [lindex $lst $indx] 
	incr indx
	set tsCfg_memNoGrowth [lindex $lst $indx] 
	incr indx
    	set tsCfg_miscOpt [lindex $lst $indx] 
	
	set tsCfg_timeout_tmp [lindex $tsCfg_backendTimeout 1]
	set tsCfg_retry_tmp [lindex $tsCfg_backendResendNumber 1]
	set tsCfg_cacheSize_tmp $tsCfg_memCacheSize

	if {$doDisplay != -1} {
	    if {[redraw_config_area $tsCfg_currentSelection]} {
		tgtsvr_update_command_line
	    }
	}
    }
}


#=====================================================================
# on_tgtsvr_cancel
#=====================================================================
proc on_tgtsvr_cancel {args} {

    select_configuration_init
    windowClose tsCfgDlg
}


#=====================================================================
#  on_tgtsvr_clone
#=====================================================================
proc on_tgtsvr_clone {args} {
    global tsCfg_currentDescr
    global tsCfg_menuItem

    set description [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]

    save_in_tsCfg_arrays $description

    if {$description != 0} {
	if {![string match $tsCfg_currentDescr $description]} {
	    tgtsvr_restore_config $description
	}
	# rename and highlight the selected entry
	set list [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
	if {[string match  "*(Copy*)" $description]} {
	    regexp {(.+) \(Copy [0-9]+\)} $description dummy new_descr
	    # find the copy string
	    regexp {.+\(Copy ([0-9]+)\)} $description dummy numstr
	    if {$numstr == ""} {
		set numstr 1
		set new_entry [format "%s (Copy %d)" $new_descr $numstr]
	    } else {
		incr numstr
		set new_entry [format "%s (Copy %d)" $new_descr $numstr]
		while {[lsearch $list $new_entry] != -1} {
		 	incr numstr
		    set new_entry [format "%s (Copy %d)" $new_descr $numstr]
		}
	    }
	
	} elseif { [lsearch $list "$description (Copy*)"] != -1 } {
	    set found -1
	    set string "$description (Copy*"
	    for {set index 0} {$index < [llength $list]} {incr index} {
		if {[string match $string [join [lrange $list $index $index]]]} {
		    set found $index
		}
	    }
	    set description [lindex $list $found]
	    regexp {(.+) \(Copy [0-9]+\)} $description dummy new_descr
	    # find the copy string
	    regexp {.+\(Copy ([0-9]+)\)} $description dummy numstr
	    if {$numstr == ""} {
		set numstr 1
	    } else {
		incr numstr
	    }
	    set new_entry [format "%s (Copy %d)" $new_descr $numstr]

	} else {
	    set cnt 1
	    set new_entry [format "%s (Copy %d)" $description $cnt]
	}
	set newlist [lappend list $new_entry]

	# turn off the menuItem setting for the new copy
	set tsCfg_menuItem 0
	controlCheckSet tsCfgDlg.tgtsvr_menuItem_bool 0

	# save the cloned variables in tsCfg_arrays and display 
	# the new name in the list box
	save_in_tsCfg_arrays $new_entry

	# select the string in the descr list
	set list [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
	set newlist [lappend list $new_entry]
	controlValuesSet tsCfgDlg.tgtsvr_descr_list $newlist
	set tsCfg_currentDescr $new_entry
	controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent -string $new_entry
	controlSelectionSet tsCfgDlg.tgtsvr_descr_text -noevent -string $new_entry
	controlValuesSet tsCfgDlg.tgtsvr_descr_text $new_entry
    } else {
	messageBox "You must select a valid description entry to clone a configuration."
    }
    
}


#=====================================================================
# tgtsvr_defaults
#=====================================================================
proc tgtsvr_defaults {args} {
    global tsCfg_tgtsvr
    global tsCfg_commandLine
    global tsCfg_tgtNameIP
    global tsCfg_serverName
    global tsCfg_usersFileName
    global tsCfg_userLock
    global tsCfg_coreFileName
    global tsCfg_objecFormat
    global tsCfg_nosyms
    global tsCfg_allsyms
    global tsCfg_synchro
    global tsCfg_baudRate
    global tsCfg_hostTtyDevice
    global tsCfg_verboseMode
    global tsCfg_consoleMode
    global tsCfg_displayHost
    global tsCfg_memCacheSize
    global tsCfg_backendType
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_backend_logfile
    global tsCfg_FS_path
    global tsCfg_device
    global tsCfg_version
    global tsCfg_currentSelection
    global tsCfg_defaultSelection
    global tsCfg_menuItem
    global tsCfg_backendOpt
    global tsCfg_redirectIO 
    global tsCfg_console 
    global tsCfg_redirectShell 
    global tsCfg_backendOpt
    global tsCfg_backend_logsize
    global tsCfg_wtx_logfile
    global tsCfg_wtx_logsize
    global tsCfg_wtx_logfilter
    global tsCfg_omf 
    global tsCfg_usepm 
    global tsCfg_memNoGrowth
    global tsCfg_miscOpt

    global tsCfg_debugMode

    if {$tsCfg_debugMode} {
    	set tsCfg_tgtsvr "tgtsvr-d.exe"
    } else {
    	set tsCfg_tgtsvr "tgtsvr.exe"
    }
    set tsCfg_serverName ""
    set tsCfg_usersFileName ""
    set tsCfg_userLock ""
    set tsCfg_coreFileName ""
    set tsCfg_objecFormat ""
    set tsCfg_nosyms ""
    set tsCfg_allsyms ""
    set tsCfg_synchro ""
    set tsCfg_baudRate ""
    set tsCfg_hostTtyDevice ""
    set tsCfg_verboseMode -V
    set tsCfg_consoleMode ""
    set tsCfg_displayHost ""
    set tsCfg_memCacheSize ""
    set tsCfg_backendType ""
    set tsCfg_backendTimeout ""
    set tsCfg_backendResendNumber ""
    set tsCfg_backend_logfile ""

    
    set tsCfg_FS_path [concat -R "C:\\" -RW]

    set tsCfg_tgtNameIP ""
    set tsCfg_device ""
    set tsCfg_version ""
    set tsCfg_redirectChild ""
    set tsCfg_closeWindowOnExit ""
    set tsCfg_menuItem 0
    set tsCfg_backendOpt ""
    set tsCfg_redirectIO ""
    set tsCfg_console "" 
    set tsCfg_redirectShell ""
    set tsCfg_backend_logsize ""
    set tsCfg_wtx_logfile ""
    set tsCfg_wtx_logsize ""
    set tsCfg_wtx_logfilter ""
    set tsCfg_omf  ""
    set tsCfg_usepm  ""
    set tsCfg_memNoGrowth ""
    set tsCfg_miscOpt ""
    set tsCfg_backendOpt ""

    controlSelectionSet tsCfgDlg.configure -noevent $tsCfg_defaultSelection
    set prevSelection $tsCfg_currentSelection
    set tsCfg_currentSelection $tsCfg_defaultSelection
    show_tgtsvr_configuration_choices $tsCfg_defaultSelection $prevSelection
}



#=====================================================================
# fill_ts_array_element
#=====================================================================
proc fill_ts_array_element  {args} {
    global tsCfg_tgtsvr
    global tsCfg_tgtNameIP
    global tsCfg_serverName
    global tsCfg_usersFileName
    global tsCfg_userLock
    global tsCfg_coreFileName
    global tsCfg_objecFormat
    global tsCfg_nosyms
    global tsCfg_allsyms
    global tsCfg_synchro
    global tsCfg_baudRate
    global tsCfg_hostTtyDevice
    global tsCfg_verboseMode
    global tsCfg_consoleMode
    global tsCfg_memCacheSize
    global tsCfg_backendType
    global tsCfg_backendTimeout
    global tsCfg_backendResendNumber
    global tsCfg_backend_logfile
    global tsCfg_FS_path
    global tsCfg_device
    global tsCfg_version
    global tsCfg_menuItem
    global tsCfg_redirectChild
    global tsCfg_closeWindowOnExit
    global tsCfg_backendOpt
    global tsCfg_redirectIO
    global tsCfg_console
    global tsCfg_redirectShell
    global tsCfg_backend_logsize
    global tsCfg_wtx_logfile
    global tsCfg_wtx_logsize
    global tsCfg_wtx_logfilter
    global tsCfg_omf
    global tsCfg_usepm
    global tsCfg_memNoGrowth
    global tsCfg_miscOpt

    set save_vars [list \
		$tsCfg_tgtsvr $tsCfg_tgtNameIP $tsCfg_serverName \
    	$tsCfg_usersFileName $tsCfg_userLock \
    	$tsCfg_coreFileName $tsCfg_objecFormat $tsCfg_nosyms $tsCfg_allsyms $tsCfg_synchro \
    	$tsCfg_hostTtyDevice $tsCfg_verboseMode $tsCfg_backendTimeout \
    	$tsCfg_consoleMode $tsCfg_memCacheSize \
    	$tsCfg_backendType $tsCfg_backendResendNumber \
    	$tsCfg_backend_logfile $tsCfg_version \
    	$tsCfg_baudRate $tsCfg_device $tsCfg_menuItem $tsCfg_redirectChild \
    	$tsCfg_closeWindowOnExit \
		$tsCfg_FS_path $tsCfg_backendOpt \
		$tsCfg_redirectIO $tsCfg_console $tsCfg_redirectShell \
		$tsCfg_backend_logsize \
		$tsCfg_wtx_logfile $tsCfg_wtx_logsize $tsCfg_wtx_logfilter \
		$tsCfg_omf $tsCfg_usepm $tsCfg_memNoGrowth $tsCfg_miscOpt]
    return $save_vars

}

proc printArray {args} {
    global tsCfg_arrays
    global tsCfg_descrList

    foreach descr $tsCfg_descrList {
	puts "$tsCfg_arrays($descr)"
    }

}

#=====================================================================
# save_in_tsCfg_arrays
#
#	Saves the passed descriptor in tsCfg_arrays and also writes
#	the descriptor out to the tgtsvr_descr_list control
#
#=====================================================================
proc save_in_tsCfg_arrays {descriptor} {
    global tsCfg_arrays
    global tsCfg_descrList
    global tsCfg_coreFileName

    if {$descriptor != ""} {
		set save_vars [fill_ts_array_element]
		# add new entry to the tsCfg_arrays structure
		set tsCfg_arrays($descriptor) $save_vars
		set tsCfg_descrList [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
    }
}

#=====================================================================
# on_tgtsvr_new
#=====================================================================
proc on_tgtsvr_new {args} {
    global tsCfg_descrList
    global tsCfg_arrays
    global tsCfg_currentDescr
    global tsCfg_currentSelection
    global tsCfg_backendType
    global tsCfg_tgtNameIP
    global tsCfg_saved_description

    set list [controlValuesGet tsCfgDlg.tgtsvr_descr_list]

 	if {$list != ""} {
		save_in_tsCfg_arrays $tsCfg_currentDescr
    }

    # create a unique name using Target Server Descriptor
    set text [format "%s" "Configuration "]
    for {set n 1} {$n < 100} {incr n} {
	if {[lsearch $list $text$n] == -1} {
	    set text $text$n
	    set newlist [lappend list $text]
	    break
	}
    }

    controlValuesSet tsCfgDlg.tgtsvr_descr_text $text
    controlValuesSet tsCfgDlg.tgtsvr_descr_list $newlist
    set tsCfg_descrList $newlist

    # clear out globals or use defaults for new configuration
    redraw_config_area 1
    tgtsvr_defaults
    set tsCfg_saved_description ""
    save_in_tsCfg_arrays $text
    controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent [lsearch $list $text]
    set tsCfg_currentDescr $text
    tgtsvr_enable_buttons
    tgtsvr_enable_controls
    tgtsvr_update_command_line	
}

#----------------------------------------------------------------------
# ts_checkForCorrectness
#
#----------------------------------------------------------------------
proc ts_checkForCorrectness {args} {
    global tsCfg_backendType
    global tsCfg_tgtNameIP
    global tsCfg_coreFileName
    global tsCfg_descrList

  	foreach descriptor $tsCfg_descrList {
		tgtsvr_restore_config $descriptor -1

		if {[ts_checkForSpaces $descriptor]} {
	    	return $descriptor
		}

		if {[string trim $tsCfg_tgtNameIP] == ""} {
			messageBox "'$descriptor' requires a target name or IP address."
			return	$descriptor
		}

		if {[lindex $tsCfg_coreFileName 0] == "-c"} {
	 		if {[lindex $tsCfg_coreFileName 1] == ""} {
				messageBox \
					"'$descriptor' requires a core filename to be specified."
				return $descriptor
	    	}
		}
    }
    return ""
}


#----------------------------------------------------------------------
#  on_tgtsvr_OK
#
#	Save the descriptions in tsCfg_descrList to the registry
#
#----------------------------------------------------------------------
proc on_tgtsvr_OK {args} {
    global tsCfg_arrays
    global tsCfg_currentDescr
    global tsCfg_saved_description
    global tsCfg_descrList
    global tsCfg_menuItem
    global tsCfg_redirectChild
    global tsCfg_commandLine
    global tsCfg_closeWindowOnExit
    global tsCfg_currentSelection
    global tsCfg_regEntries

    set saveNotNeeded [lindex $args 0]
    # get descriptors  and put into persistent storage
    if {$tsCfg_currentDescr != "" && $saveNotNeeded == ""} {
		controlSelectionSet tsCfgDlg.tgtsvr_descr_text \
					-noevent -string $tsCfg_currentDescr

		#  save description
		save_in_tsCfg_arrays $tsCfg_currentDescr
    }

    set descriptor [ts_checkForCorrectness]
    if {$descriptor != ""} {
		tgtsvr_restore_config $tsCfg_currentDescr
		return
    }
    

	ts_updateMenuItems

    # delete previous registry contents  and save the current descriptor list
  	tgtsvr_save_entries

    # set so that when we come up next time, the default selection is displayed
    set tsCfg_currentSelection -1
    select_configuration_init
    windowClose tsCfgDlg
}

#----------------------------------------------------------------------
# on_tgtsvr_descr_list
#
#----------------------------------------------------------------------
proc on_tgtsvr_descr_list {args} {
    global tsCfg_currentDescr

    set selectedTarget [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]
    save_in_tsCfg_arrays $tsCfg_currentDescr

    # restore contentes of the selection to global variables and redisplay
    tgtsvr_restore_config $selectedTarget
    
    set tsCfg_currentDescr $selectedTarget
    controlValuesSet tsCfgDlg.tgtsvr_descr_text $selectedTarget
}


#----------------------------------------------------------------------
# on_tgtsvr_descr_text
#
#----------------------------------------------------------------------
proc on_tgtsvr_descr_text {args} {
    global tsCfg_currentDescr
    global tsCfg_descrList

    set list [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
    set modText [controlValuesGet tsCfgDlg.tgtsvr_descr_text]

    # see if descr list is empty, if so do nothing
    if {$list == "" && [llength $list] == 0} {
	return
    }
    # see if the modified descriptor is the only one in the list box
    if {[lsearch $list $modText] != -1 && [llength $list] == 1} {
	set tsCfg_currentDescr $modText
    }

    set found -1
    for {set index 0} {$index < [llength $list]} {incr index} {
	set name [lindex $list $index]
	if {[string match $name $tsCfg_currentDescr]} {
	    set found $index
	}
    }

    if {$found == -1} {
	set found [expr [llength $list] - 1]
    }
    # read the modified text and write it to the descr list and make it current descr
    set list [lreplace $list $found $found $modText]
    controlValuesSet tsCfgDlg.tgtsvr_descr_list $list
    set tsCfg_currentDescr $modText
    controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent $found
    set tsCfg_descrList [controlValuesGet tsCfgDlg.tgtsvr_descr_list]
}

#----------------------------------------------------------------------
# tgtsvr_disable_buttons
#
#----------------------------------------------------------------------
proc tgtsvr_disable_buttons {args} {

    controlEnable tsCfgDlg.tgtsvr_launch_button 0
    controlEnable tsCfgDlg.tgtsvr_remove_button 0
    controlEnable tsCfgDlg.tgtsvr_clone_button 0
    tgtsvr_update_command_line
}


#----------------------------------------------------------------------
# tgtsvr_enable_buttons
#
#----------------------------------------------------------------------
proc tgtsvr_enable_buttons {args} {

    controlEnable tsCfgDlg.tgtsvr_launch_button 1
    controlEnable tsCfgDlg.tgtsvr_remove_button 1
    controlEnable tsCfgDlg.tgtsvr_clone_button 1
    tgtsvr_update_command_line
}


#----------------------------------------------------------------------
# tgtsvr_disable_controls
#
#----------------------------------------------------------------------
proc tgtsvr_disable_controls {args} {

    controlEnable tsCfgDlg.tgtsvr_name_text 0
    controlEnable tsCfgDlg.tgtsvr_descr_text 0
    controlEnable tsCfgDlg.tgtsvr_backend_available_list 0
    controlEnable tsCfgDlg.tgtsvr_backend_resend_text 0
    controlEnable tsCfgDlg.tgtsvr_backend_timeout_text 0
    controlEnable tsCfgDlg.tgtsvr_backend_logfile_text 0
    controlEnable tsCfgDlg.tgtsvr_backend_logfile_button 0
    controlEnable tsCfgDlg.tgtsvr_backend_opt_text 0
    controlEnable tsCfgDlg.configure 0
    controlEnable tsCfgDlg.tgtsvr_backend_device_combo 0
    controlEnable tsCfgDlg.tgtsvr_backend_baud_combo 0
    controlEnable tsCfgDlg.tgt_name_ip_text 0
}

#----------------------------------------------------------------------
# tgtsvr_enable_controls
#----------------------------------------------------------------------
proc tgtsvr_enable_controls {args} {

    controlEnable tsCfgDlg.tgtsvr_name_text 1
    controlEnable tsCfgDlg.tgtsvr_descr_text 1
    controlEnable tsCfgDlg.tgtsvr_backend_available_list 1
    controlEnable tsCfgDlg.tgtsvr_backend_resend_text 1
    controlEnable tsCfgDlg.tgtsvr_backend_timeout_text 1
    controlEnable tsCfgDlg.tgtsvr_backend_logfile_text 1
    controlEnable tsCfgDlg.tgtsvr_backend_opt_text 1
    controlEnable tsCfgDlg.configure 1
    controlEnable tsCfgDlg.tgtsvr_backend_device_combo 1
    controlEnable tsCfgDlg.tgtsvr_backend_baud_combo 1
    controlEnable tsCfgDlg.tgt_name_ip_text 1
}


#----------------------------------------------------------------------
# on_tgtsvr_remove_description
#
#----------------------------------------------------------------------
proc on_tgtsvr_remove_description {args} {
    global tsCfg_device
    global tsCfg_version
    global tsCfg_descrList
    global tsCfg_arrays
    global tsCfg_currentDescr
    global tsCfg_menuItem
    global tsCfg_currentSelection

    # get the selected string and remove from the tgtsvr_descr_list
    set selection [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]

    set index [controlSelectionGet tsCfgDlg.tgtsvr_descr_list]
    set list [controlValuesGet tsCfgDlg.tgtsvr_descr_list]

    # find the matching selection and remove it from list and write list back out
    set indx [lsearch -exact $list $selection]
    set new_list [lreplace $list $indx $indx]
    controlValuesSet  tsCfgDlg.tgtsvr_descr_list $new_list
    set tsCfg_descrList $new_list

    # to save some memory, make deleted array element null
    if {$selection != ""} {
		set tsCfg_arrays($selection) ""
	}

    # select the next available descriptor (down, then up)
    set count [llength tsCfgDlg.tgtsvr_descr_list]

    if {$count == 0} {
		controlValuesSet tsCfgDlg.tgtsvr_descr_text	""
		set tsCfg_currentDescr ""

		#disable the controls
		tgtsvr_disable_buttons
		tgtsvr_disable_controls
		set tsCfg_descrList ""
		controlValuesSet tsCfgDlg.tgtsvr_commandLine_text ""
		controlValuesSet tsCfgDlg.tgtsvr_name_text ""
		redraw_config_area 1
		return
    } elseif {$index >= $count} {
		controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent [expr $index - 1]
		set selection [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]
		set tsCfg_currentDescr $selection
		controlValuesSet tsCfgDlg.tgtsvr_descr_text $selection
    } else {
		controlSelectionSet tsCfgDlg.tgtsvr_descr_list -noevent [expr $index]
		set selection [controlSelectionGet tsCfgDlg.tgtsvr_descr_list -string]
		set tsCfg_currentDescr $selection
		controlValuesSet tsCfgDlg.tgtsvr_descr_text  $selection
    }

    # restore the values from the new selection and display them
    tgtsvr_restore_config $selection
    redraw_config_area 1

}

#----------------------------------------------------------------------
# targetServerCustomLaunch
#
#	Launches a target server given a command line, passed either as a
#	list or as a string. 
#----------------------------------------------------------------------
proc targetServerCustomLaunch {args} {
	set cmdLine [join $args " "]
	set debugMode [uitclinfo debugmode]

	if {[string match {tgtsvr.exe *} $cmdLine] && $debugMode} {
				set ans [messageBox -yesno \
				"This will launch a target server without Windows\n \
				debugging information (tgtsvr.exe). Continue?"]
				if {$ans== "no"} {
					return
				}
	}
	if {[string match {tgtsvr-d.exe *} $cmdLine] &&!$debugMode} {
				set ans [messageBox -yesno \
					"This will launch a target server with Windows\n \
					debugging information (tgtsvr-d.exe). Continue?"]
				if {$ans== "no"} {
					return
				}
	}
	puts stderr "Command: $cmdLine"
	if {[catch {toolLaunch "Target Server" $cmdLine "" 0 0 0 0 0 0} err]} {
		messageBox "error: '$err'"
	} 
}


#----------------------------------------------------------------------
# targetServerCustomizeDialog
#
#  	Initialize and setup the target server configuration dialog
#
#----------------------------------------------------------------------
proc targetServerCustomizeDialog {args} {
    global tsCfg_tsCfg_ctrlList
    global tsCfg_defaultSelection
    global wind_uid
    global tsCfg_arrays

    controlCreate tsCfgDlg [list label -title "Tar&get Server Descriptions" \
	  -x 7 -y 5 -w 100 -h 9]
    controlCreate tsCfgDlg	{ list -name tgtsvr_descr_list -callback on_tgtsvr_descr_list
	  -x 7 -y 16 -w 151 -h 38 }
    controlCreate tsCfgDlg	{ label -title "&Description"
	  -x 7 -y 48 -w 72 -h 8 }
    controlCreate tsCfgDlg	{ text -name tgtsvr_descr_text -callback on_tgtsvr_descr_text
	  -x 53 -y 46 -w 106 -h 12 }
     controlCreate tsCfgDlg { boolean -title "&Add description to menu" \
	      					-name tgtsvr_menuItem_bool \
	      					-callback on_tgtsvr_menu_item \
							-auto -x 53 -y 62 -w 100 -h 9 }
    controlCreate tsCfgDlg	{ label -title "Target Server Na&me"
	  -x 7 -y 76 -w 70 -h 9 }
    controlCreate tsCfgDlg	{ text -name tgtsvr_name_text -callback on_tgtsvr_name_text
	  -x 80 -y 74 -w 79 -h 12 }
    controlCreate tsCfgDlg { label -title "Targ&et Name/IP Address" -name tgt_name_ip_label \
	   -x 7 -y 201 -w 100 -h 9}
    controlCreate tsCfgDlg { text -name tgt_name_ip_text -callback on_tgt_name_ipadds \
	  -x 90 -y 198 -w 68 -h 12}
    controlCreate tsCfgDlg	{ button -title "&New" -name tgtsvr_add
	  -callback on_tgtsvr_new
	  -x 167 -y 16 -w 45 -h 14 }
    controlCreate tsCfgDlg 	{ button -title "&Copy" -name tgtsvr_clone_button
	  -callback on_tgtsvr_clone
	  -x 167 -y 34 -w 45 -h 14 }
    controlCreate tsCfgDlg	{ button -title "&Remove" -name tgtsvr_remove_button
	  -callback on_tgtsvr_remove_description
	  -x 167 -y 52 -w 45 -h 14 }

    controlCreate tsCfgDlg	{ button -title "&Launch" -name tgtsvr_launch_button
	  -callback on_tgtsvr_launch
	  -x 59 -y 261 -w 48 -h 14 }

    controlCreate tsCfgDlg {group -name tgtsvr_selection_group \
							-title "Target Server &Properties" \
							-x 7 -y 95 -w 205 -h 98 }
    controlCreate tsCfgDlg {combo -callback on_config_selected \
							-name configure -x 100 -y 93 -w 100 -h 65}
    controlInit

    if {$wind_uid == "not set"} {
	controlEnable tsCfgDlg.tgtsvr_authLock_choice 0
    }

    # read target server data base descriptions from the registry
    read_all_tgtsvr_entries

    # display the last saved target server description
    get_last_saved_tgtsvr_description

    select_configuration_init

    if {[controlValuesGet tsCfgDlg.tgtsvr_descr_list] == ""} {
		tgtsvr_update_command_line

		controlValuesSet tsCfgDlg.tgtsvr_descr_text	""
		set tsCfg_currentDescr ""

		#disable the controls
		tgtsvr_disable_buttons
		tgtsvr_disable_controls
		set tsCfg_descrList ""
		redraw_config_area 1
    }
    
    show_command_line
    show_last_row

    # show the command line for the last saved target server description if any
    tgtsvr_update_command_line

    # windowExitCallbackSet targetServerCustomizeDialogExit

}

proc ts_updateMenuItems	{args} {
    global tsCfg_descrList
    global tsCfg_currentSelection
    global tsCfg_commandLine
    global tsCfg_menuItem
	global listTSMenuItems

    # remove all items previously added from the menu
    foreach menuItem $listTSMenuItems {
    	if {[catch {menuItemDelete -bypath -string $menuItem} err]} {
			puts "menuItemDelete -bypath -string $menuItem"
		}
    }
	set listTSMenuItems {}

	set menuBarItem {&Tools}
	set menuParentItem "&Target Server"
	set menuPoint [list $menuBarItem $menuParentItem] 
    set nItems [llength $tsCfg_descrList]
    for {set ix 0} {$ix < $nItems} {incr ix} {
		set menuItem [lindex $tsCfg_descrList $ix]
		# Because of the way the registry entries were written,
		# each config is restored (but not drawn) and then the cmd 
		# line is read:
		set tsCfg_currentSelection -1
		tgtsvr_restore_config $menuItem -1
		tgtsvr_update_command_line

		if {$tsCfg_menuItem == 1} {
        	set menuItem [string trim $menuItem]
        	if {$menuItem == ""} { continue}
        	if {![menuExists [list $menuPoint $menuItem]]} {
            	menuItemAppend \
					-statusmessage "Launch target server '$menuItem'" \
            		-callback \
					[list targetServerCustomLaunch $tsCfg_commandLine] \
                	-bypath $menuPoint $menuItem
            	lappend listTSMenuItems \
					[list $menuBarItem $menuParentItem $menuItem]
        	}
		}
    }

    # if there were no custom stuff, add the default to menu
	set defMenuItem "No Launches"
    if {[llength $listTSMenuItems] == 0} {
        menuItemAppend \
            -cmduicallback ::disableTSMenuItem \
            -bypath $menuPoint $defMenuItem 
        menuItemEnable -bypath $menuPoint 0
        lappend listTSMenuItems [list $menuBarItem $menuParentItem $defMenuItem]
    }
}

#
# A Command UI Update routine that always disables the menu item
#
proc disableTSMenuItem {} {
    global enableflag
    global checkflag
    set enableflag 0
    set checkflag 0
}

#----------------------------------------------------------------------
# targetServerCustomizeDialogExit
#
#  	Window exit routine for the target server configuration dialog
#
#----------------------------------------------------------------------
proc targetServerCustomizeDialogExit {args} {
    select_configuration_init
}


#----------------------------------------------------------------------
# targetServerCustomizeDialogCreate
#
#----------------------------------------------------------------------
proc targetServerCustomizeDialogCreate {} {
    dialogCreate -name tsCfgDlg \
	-init targetServerCustomizeDialog \
	-exit targetServerCustomizeDialogExit \
	-title "Configure Target Servers" \
	-width 218 -height 279
}


#----------------------------------------------------------------------
# targetServerMenusCreate - creates the target server related menus
#
#----------------------------------------------------------------------
proc targetServerMenusCreate {} {
	menuItemInsert -bypath -popup -after {&Tools &Debugger...} "&Target Server"

	menuItemAppend -bypath -statusmessage "Configure target servers" \
    	-callback targetServerCustomizeDialogCreate \
    	{&Tools "&Target Server"} "&Configure..."

	menuItemAppend -separator -bypath \
    	{&Tools "&Target Server" &Configure...}
}

set tsCfg_regEntries [appRegistryEntryRead Target tgtsvrDescrList]
if {$tsCfg_regEntries != ""} {
    # get working copy of registry entries
    set tsCfg_descrList $tsCfg_regEntries
    foreach entry $tsCfg_descrList {
		if {$entry != ""} {
			set tsCfg_arrays($entry) [appRegistryEntryRead Target $entry]
		}
    }
}

targetServerMenusCreate
ts_updateMenuItems
