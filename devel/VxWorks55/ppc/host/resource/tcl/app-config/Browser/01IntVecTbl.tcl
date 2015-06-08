# 01IntVecTbl.tcl - Connected interrupt routine browser extension
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01b,10jul98,jmp  no longer display the vector table icon if not available.
# 01a,10oct95,jmp  written.
#

#
# DESCRIPTION
# This module holds the Tcl code required by the vectors browser
# application. It creates the vectors browser window and calls 
# intRtnGet routine to fill the vectors browser.
#
# RESOURCE FILES
# wpwr/host/resource/tcl/intVecTbl.tcl
#

# source resource file
source [wtxPath host resource tcl]intVecTbl.tcl

# global variable
set intRtnPosted 0	;# Keeps track of whether the vectors browser is posted.


################################################################################
# 
# intRtnDestroyed - vectors browser destroyed routine
#
# This routine is called when the user discards the vectors browser.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc intRtnDestroyed {name} {
    global intRtnPosted
    set intRtnPosted 0
}

################################################################################
#
# intRtnBrowse - vectors browser main routine
#
# This routine is called to fill the vectors browser with data
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc intRtnBrowse {} {

    global intRtnPosted
    set intRtnPosted 1

    # Create vectors hierarchy
    set vecTblTitle "[lindex [wtxHandle] 0]: Vector Table"
    hierarchyCreate -destroy intRtnDestroyed -change $vecTblTitle

    set hierarchyStruct "{Vectors +}"

    hierarchySetStructure $vecTblTitle $hierarchyStruct

    # Fill in the vector table hierarchy
    intRtnUpdate
}

################################################################################
#
# intRtnUpdate - Update the vector table hierarchy
#
# This routine is called to fill the vectors browser with data
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc intRtnUpdate {} {

    global intRtnPosted

    # This routine is recorded in the auxiliary procedure list, then will be
    # called each time the browser is updated. If intVecShow is not running,
    # we just return immediately.
    if {! $intRtnPosted} return

    # get all the connected routines for the default vector table
    set rtnList [intRtnGet 0 -1]

    set vecTblTitle "[lindex [wtxHandle] 0]: Vector Table"
    hierarchyPost $vecTblTitle

    # Update the displayed values
    hierarchySetValues $vecTblTitle "{$rtnList}"
}

################################################################################
#
# vectorsBrowserInit - Initialize vectors browser hierarchy
#
# This routine initializes name and structure for the vectors browser
# hierarchy.
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc vectorsBrowserInit {} {

    # add The Vector Table only if available
    if {![catch {intRtnGet 0 0}]} {
	toolBarItemCreate "vectbl" button {intRtnBrowse} ;# Initialize Toolbar
	browserAuxProcAdd intRtnUpdate
    }
}

# call the entry point
vectorsBrowserInit
