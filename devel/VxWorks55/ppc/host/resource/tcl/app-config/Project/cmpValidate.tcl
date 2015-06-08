# cmpValidate.tcl - component descriptors validation test library
#
# Copyright 1998-2001, Wind River Systems, Inc. Alameda, CA
#
# modification history
# --------------------
# 01a,20feb02,cym  written.
#
# DESCRIPTION
#
# This library contains test routines to evaluate the component gallery
# accessible to a given project.  It identifies common errors and oversights
# in component descriptions.
#
# USAGE
#
# Invoke with a project file.  The test will use the CxrDoc for that project's
# CPU/TOOL.
# 	cmpValidate foo.wpj
#

source [wtxPath host resource tcl app-config Project cmpScriptLib.tcl]

###############################################################################
#
# cmpFolderCheck - check the folder and selection structure of the components
#
# This routine validates the component descriptions by verifying that all
# components reside in a selection or folder, that all selections and
# folders contain at least one child, and that all folders and selections
# have FOLDER_ROOT as an ancestor.  This ensures that all components are
# accessible from the FOLDER_ROOT node in the GUI.
#

proc cmpFolderCheck hProj {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set cxrSet2 [$cxrDoc setCreate]

    $cxrSet = [$cxrDoc instances Component]
    $cxrSet2 = [$cxrSet get _CHILDREN]
    $cxrSet2 = [$cxrSet2 get CHILDREN]

    $cxrSet = [$cxrSet - [$cxrSet2 contents]]

    if {[$cxrSet size] > 0} {
	puts "Error: These components are not in any folder or selection:"
	puts [$cxrSet contents]
	puts ""
    }

    $cxrSet = [$cxrDoc instances Folder]
    $cxrSet = [$cxrSet + [$cxrDoc instances Selection]]
    $cxrSet2 = [$cxrSet get CHILDREN]
    $cxrSet2 = [$cxrSet2 get _CHILDREN]
    
    $cxrSet = [$cxrSet - [$cxrSet2 contents]]

    if {[$cxrSet size] > 0} {
	puts "Warning: These folders/selections have no CHILDREN:"
	puts [$cxrSet contents]
	puts ""
    }

    $cxrSet = [$cxrDoc instances Folder]
    $cxrSet = [$cxrSet + [$cxrDoc instances Selection]]
    
    $cxrSet2 = FOLDER_ROOT

    while {[$cxrSet2 get CHILDREN] != ""} {
        $cxrSet = [$cxrSet - [$cxrSet2 contents]]
        $cxrSet2 = [$cxrSet & [$cxrSet2 get CHILDREN]]
    }

    if {[$cxrSet size] > 0} {
	puts "Error: These folders/selections are not accessible through FOLDER_ROOT:"
	puts [$cxrSet contents]
	puts ""
    }
}

###############################################################################
# 
# cmpParamCheck - verify parameter existence.
#
# This routine verifies that all parameters are CFG_PARAMS for at least one
# component, and that all CFG_PARAMS specified by components exist as Parameter
# objects.
#

proc cmpParamCheck hProj {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set cxrSet2 [$cxrDoc setCreate]

    $cxrSet = [$cxrDoc instances Component]
    $cxrSet = [$cxrSet get CFG_PARAMS]
    $cxrSet2 = [$cxrDoc instances Parameter]

    if {[llength [$cxrSet2 - [$cxrSet contents]]] > 0 } {
	puts "Error: These parameters are not associated with any component:"
	puts "[$cxrSet2 - [$cxrSet contents]]"
	puts ""
    }

    if {[llength [$cxrSet - [$cxrSet2 contents]]] > 0 } {
	puts "Error: These Parameters are referenced by components, but undefined :"
	puts "[$cxrSet - [$cxrSet2 contents]]"
	puts ""
    }
}

###############################################################################
# 
# cmpAssociationCheck - verify component reference existence.
#
# This routine checks that all components specified as EXCLUDES, REQUIRES, or
# INCLUDE_WHEN in component descriptions exist as component or selection
# objects.
#

proc cmpAssociationCheck hProj {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set cxrSet2 [$cxrDoc setCreate]

    $cxrSet = [$cxrDoc instances Component]
    $cxrSet = [$cxrSet + [$cxrDoc instances Selection]]
    $cxrSet2 = [$cxrSet get REQUIRES]
    if {[llength [$cxrSet2 - [$cxrSet contents]]] > 0} {
        puts "Error: These components are REQUIREd by other components, but undefined:"
        puts "[$cxrSet2 - [$cxrSet contents]]"
	puts ""
    }

    $cxrSet2 = [$cxrSet get EXCLUDES]
    if {[llength [$cxrSet2 - [$cxrSet contents]]] > 0} {
        puts "Error: A component EXCLUDES each of these undefined components: "
        puts "[$cxrSet2 - [$cxrSet contents]]"
	puts ""
    }

    $cxrSet2 = [$cxrSet get INCLUDE_WHEN]
    if {[llength [$cxrSet2 - [$cxrSet contents]]] > 0} {
        puts "Error: These components are undefined, but marked as INCLUDE_WHEN : "
        puts "[$cxrSet2 - [$cxrSet contents]]"
	puts ""
    }

}

###############################################################################
# 
# cmpInitCheck - validate INIT_RTNs and InitGroups
#
# This routine checks that all components with INIT_RTNs are in the INIT_ORDER
# of a corresponding InitGroup, and that all components in InitGroups have
# INIT_RTNs.  If a component has an INIT_RTN, but is not in an InitGroup,
# its initialization routine will not be called.
# 

proc cmpInitCheck hProj {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set cxrSet2 [$cxrDoc setCreate]
    set cmpsWithInit ""

    $cxrSet = [$cxrDoc instances Component]
    foreach  comp [$cxrSet contents] {
	$cxrSet2 = $comp
	if {[$cxrSet2 get INIT_RTN] != ""} {
	    lappend cmpsWithInit $comp
	}
    }

    $cxrSet = [$cxrDoc instances InitGroup]
    $cxrSet2 = [$cxrSet get INIT_ORDER]
    $cxrSet2 = [$cxrSet2 & [$cxrDoc instances Component]]
    $cxrSet = $cmpsWithInit

    if {[llength [$cxrSet2 - [$cxrSet contents]]] > 0 } {
	puts "Warning: These components are present in InitGroups, but have \
	    no INIT_RTNs :"
	puts "[$cxrSet2 - [$cxrSet contents]]"
	puts ""
    }

    if {[llength [$cxrSet - [$cxrSet2 contents]]] > 0 } {
	puts "Serious Error: These components have INIT_RTNS, but are not \
	    present in any InitGroup :"
	foreach grouplessComp [$cxrSet - [$cxrSet2 contents]] {
 	    $cxrSet2 = $grouplessComp
	    puts "$grouplessComp (INIT_RTN : ([$cxrSet2 get INIT_RTN])"
	}
	puts ""
    }
}

set hProj [cmpProjOpen [lindex $argv 0]]
cmpFolderCheck $hProj
cmpInitCheck $hProj
cmpParamCheck $hProj
cmpAssociationCheck $hProj
