# makeDefaultGen.tcl - generate makefile of a project
#
# Copyright 2001 Wind River Systems, Inc.
#
# modification history
# ------------------------
# 01b,29jan02,j_s clarify error message
# 01a,23jan02,j_s give an error if prjTagData is empty due to invisible
#		  characters have been introduced into .wpj file
#
# DESCRIPTION
# This script generate makefile for a project.

source [wtxPath]/host/resource/tcl/app-config/Project/cmpScriptLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/wccLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/dataDocLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/prjLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/prjFileLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/prjBuildLib.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/tc_cplus.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/prj_vxWorks.tcl
source [wtxPath]/host/resource/tcl/app-config/Project/Utils.tcl

set prjFile [lindex $::argv 0]
set prjDir [lindex $::argv 1]
set prjDirMigrated [lindex $::argv 2]

set hPrjOrigin [prjOpen $prjFile]
if ![info exists prjTagData] {
    error "Error: the format of $prjFile is unrecognizable. This is most likely due to invisible characters."
}
set prjType [prjTypeGet $hPrjOrigin]
${prjType}_hidden::makeGen $hPrjOrigin ${prjDir}/Makefile.default
file rename ${prjDir}/Makefile.default ${prjDirMigrated}/Makefile.default

