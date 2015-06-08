# 99Config.tcl - Browser's configuration panel
#
# Copyright 1994-1995 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01d,10nov95,pad  Adapted to changes in 01Spy.tcl.
#		   Cosmetic changes to follow WRS Tcl coding conventions.
# 01c,09oct95,pad  Added reference to Target Server in window title (SPR #5110).
# 01b,31may95,pad  renamed into 99Config.tcl
# 01a,24may95,pad  created
#*/

# DESCRIPTION
# This file holds all the Tcl code required to display a configuration panel
# that would allow to modify the browser modes and default values.
# The configuration panel is called when the config button, in the tool bar,
# is pressed.
# Note: this file should be read after all other app customization files are
# read since it relies on global variables that are declared in these files.
#
# RESOURCE FILES: N/A
#

##############################################################################
#
# configBoxInit - global variable initialization and object creations
#
# This procedure creates the configuration dialog and initializes it.
#
# SYNOPSIS:
# configBoxInit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc configBoxInit {} {
    global spyInterval
    global updateIntv
    global spyFrequency

    # Creation of the Config button that calls the configuration panel.
    # This button is installed in the Browser toolbar.

    toolBarItemCreate config        	button 	{browserConfig}

    # Creation of the Configuration panel

    dialogCreate configPanel -size 290 345 \
	{ \
	{label "Symbol sort:"} \
	{boolean "Alphabetical"} \
	{label "Spy mode:"} \
	{boolean "Differential"} \
	{text "Spy report time (sec):" -left 0 -right 90} \
	{text "Spy collect frequency (ticks/sec):" -left 0 -right 90} \
	{text "Browser update time (sec):" -left 0 -right 90} \
	{button "Apply" -top "Browser update time (sec):+15" \
	    -left 10 -right 40 configSet} \
	{button "Cancel" -top "Browser update time (sec):+15" \
	    -left 60 -right 90 configPanelUnpost} \
    }

    # Set the default values in the configuration panel

    set val1 [expr $spyInterval / 1000]
    set val2 [expr $updateIntv / 1000]

    dialogSetValue configPanel "Spy report time (sec):" $val1 \
			   "Spy collect frequency (ticks/sec):" $spyFrequency \
			   "Browser update time (sec):" $val2
}

##############################################################################
#
# browserConfig - display the configuration box
#
# This procedure posts the Configuration panel and sets its title. It is
# invoked when a the Config button is clicked in the Browser toolbar.
#
# SYNOPSIS:
# browserConfig
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browserConfig {} {
    dialogPost configPanel
    dialogTitleSet configPanel "[lindex [wtxHandle] 0]: browser config"
}

##############################################################################
#
# configSet - set the browser's configuration 
#
# This procedure sets the browser's configuration as it is defined in the
# configuration panel. It gets the status and contents of each button and
# text field in the configuration panel and sets the browser's configuration
# accordingly.
#
# This procedure is called when the Apply button of the configuration panel is
# pressed.
#
# SYNOPSIS:
# configSet
#
# PARAMETERS: N/A
#
# RETURNS: N/A
# 
# ERRORS: N/A
#

proc configSet {} {
    global symSortByName
    global displayValue
    global updateIntv
    global spyInterval
    global spyFrequency
    global updateMode
    global symSortMode
    global autoUpdateStr
    global noAutoUpdateStr
    global symSortNumStr
    global symSortAlphaStr
    global spyMode
    global spyCumul
    global spyDiffer
    global browserUpdate

    # Get contents of configuration panel

    set configInfo [dialogGetValues configPanel]

    # Set symbol sort mode (by address / by name)

    set symSortByName [lindex $configInfo 0]

    # Set spy mode (cumulative / differential). Note that the config box gives
    # opposite values for the spy mode.

    set displayValue [expr ([lindex $configInfo 1] > 0) ? 0 : 1]

    # Update Spy window's title

    spyTaskDisplaySet

    # Update browser's status bar

    if {$displayValue} {
	set spyMode $spyCumul
	browserStatusUpdate
    } else {
	set spyMode $spyDiffer
	browserStatusUpdate
    }

    # Set spy interval. If text field is empty, default to 10 sec

    set spyInterval [lindex $configInfo 2]
    if {$spyInterval == ""} {
	set spyIntervalNum 10
    } else {
	if {([scan $spyInterval "%d" spyIntervalNum] != 1) || \
	    ($spyInterval == 0)} {
	    noticePost error "Spy interval must be an integer greater than 0."
	    return
	}
    }

    set msec [expr $spyIntervalNum * 1000]
    set spyInterval $msec

    # Set spy frequency. If text field is empty, default to 100 ticks/sec

    set spyFrequency [lindex $configInfo 3]
    if {$spyFrequency == ""} {
	set spyFrequency 100
    } else {
	if {([scan $spyFrequency "%d" spyFrequencyNum] != 1) || \
	    ($spyFrequency == 0)} {
	    noticePost error "Spy frequency must be an integer greater than 0."
	    return
	}
	set spyFrequency $spyFrequencyNum
    }

    # Set automatic update interval. If text field is empty, default to 10 sec

    set updateIntv [lindex $configInfo 4]
    if {$updateIntv == ""} {
	set updateIntvNum 10
    } else {
	if {([scan $updateIntv "%d" updateIntvNum] != 1) || \
	    ($updateIntvNum == 0)} {
	    noticePost error "Browser interval must be an integer greater than 0."
	    return
	}
    }

    # Update status bar
 
    if {$browserUpdate} {
	set updateMode $autoUpdateStr
	browserStatusUpdate
    }

    if {$symSortByName} {
	set symSortMode $symSortAlphaStr
	browserStatusUpdate
    } else {
	set symSortMode $symSortNumStr
	browserStatusUpdate
    }

    set msec [expr $updateIntvNum * 1000]
    set updateIntv $msec

    # Remove configuration panel

    dialogUnpost configPanel
}

##############################################################################
#
# configPanelUnpost - remove the configuration panel
#
# This procedure removes the Configuration panel from the screen. It is called
# either when the configuration box Cancel button is pressed.
#
# SYNOPSIS:
# configPanelUnpost
#
# PARAMETERS: N/A
#
# RETURNS: N/A
# 
# ERRORS: N/A
#

proc configPanelUnpost {} {
    dialogUnpost configPanel
}

##############################################################################
#
# 99Config.tcl - initializations
#
# This part of the file does all the necessary initializations to get a runable
# configuration box.
# 

configBoxInit
