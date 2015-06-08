# bsp2prj.tcl - tcl script to create a project from a BSP build.
# 
# Copyright 1984-2002, Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,03jun02,rbl  new fix for bsps outside Tornado tree
# 01j,31may02,cjs  Rollback/overwrite version 5; breaks nightly builds'
#                  project generation
# 01i,23apr02,rbl  fix to cope with bsp directories outside the tornado tree
# 01h,27mar02,dat  Adding new 'clean' option for SPR 72823
# 01g,16jan02,dat  removed build_spec creations, they are done elsewhere now
# 01f,05nov01,dat  Adding additional Builds to default project
# 01e,01oct01,dat  Adding optional CPU TOOL arguments
# 01d,22jan01,sn   allow base directory for project creation to be overridden
#                  by setting WIND_PROJ_BASE 
# 01c,24sep98,ms   undid 01b - workspace procs no longer callable without GUI.
# 01b,07sep98,ms   create a workspace document as well as a project document
# 01a,28aug98,ms   written
#
# DESCRIPTION
# This script is used to generate a project from a BSP.  Optionally, the CPU
# and TOOL make values can be changed.  After the project is created, at least
# one of the defined build specs will be built.  Optionally, all 4 possible
# specs will be built.
#
# The default build specs and their default target are:
# .CS
#     BUILD_SPEC             RULE
#     default                vxWorks
#     default_rom            vxWorks_rom
#     default_romCompressed  vxWorks_romCompressed
#     default_romResident    vxWorks_romResident
# .CE
#
# This script uses the cmpScriptLib.tcl script as its main interface for 
# project creation and manipulation.  The actual project creation code
# though is in the prj_vxWorks.tcl script.  This script is normally invoked
# by the make rules in target/h/make/rules.bsp.
#
# The base project directory can be set by defining the environment
# variable WIND_PROJ_BASE. This feature is used by the host validation
# test. If WIND_PROJ_BASE is not set the base directory defaults to
# $WIND_BASE/target/proj.
#
# If 1 argument is present, it is $CPU (TOOL will default to 'gnu')
# If 2 arguments are present, it is $CPU and $TOOL
# Otherwise, take CPU and TOOL values from bsp/Makefile
#
# If optional 3rd argument is 'all', then 4 build specs are built. Projects
# are always created with all 4 build specs now, this option will do a build
# for all of them.  Without them, only the default (vxWorks) build_spec will
# be built.
#
# If optional 3rd argument is 'clean' then the script will clean up temp and
# intermediate files from all project build specs.

set CPU ""
set TOOL "gnu"
set ALL_BUILDS ""

if {$argc >= 1} {
	set CPU [lindex $argv 0]
	}
if {$argc >= 2} {
	set TOOL [lindex $argv 1]
	} 
if {$argc >= 3} {
	set ALL_BUILDS [lindex $argv 2]
	}
if {$argc >= 4} {
	puts "argc $argc"
	exit (1)
	}



source [wtxPath]/host/resource/tcl/app-config/Project/cmpScriptLib.tcl
source [wtxPath host resource tcl app-config Project]Utils.tcl
source [wtxPath host resource tcl app-config Project]WorkspaceDoc.tcl

set bsp [pwd]

# Cope with oddities of pwd when using Clearcase, by replacing
# e.g. /vobs/.wpwr_vxw_drv/target/config with /vobs/wpwr/target/config

set targetConfigFromWtxPath [wtxPath target]config
cd $targetConfigFromWtxPath
set targetConfigFromPwd [pwd]
cd $bsp
regsub $targetConfigFromPwd $bsp $targetConfigFromWtxPath bsp

# determine the base project directory and ensure it exists

set projBase [wtxPath]target/proj

if {[info exists env(WIND_PROJ_BASE)]} {
    set projBase $env(WIND_PROJ_BASE)
}

# create the main project directory if does not exist

file mkdir $projBase

# setup some directory and file names

set projDir $projBase/[file tail $bsp]_$TOOL
set projFile $projDir/[file tail $bsp]_$TOOL.wpj
set workspaceFile $projDir/[file tail $bsp]_$TOOL.wsp

# procedure to clean a single build spec

proc buildSpecClean {dir} {
    if {[file exists $dir]} {
	foreach temp [glob -nocomplain $dir/*.o] {
	    file delete -force $temp
	}
	file delete -force $dir/ctdt.c
	file delete -force $dir/symTbl.c
    }
}

# For clean operation, clean all 4 of the default build specs.

if { $ALL_BUILDS == "clean" } {
	# perform a clean operation for all project build specs
	buildSpecClean $projDir/default
	buildSpecClean $projDir/default_rom
	buildSpecClean $projDir/default_romCompress
	buildSpecClean $projDir/default_romResident
	return 0
}

# create the project

file delete -force $projDir
cmpProjCreate $bsp $projFile vxWorks $CPU $TOOL

# Close/reopen the project to save the .wpj file to disk
# This creates a fence between project creation and project build failures

cmpBuildSpecSet default
cmpProjClose
cmpProjOpen $projFile

if {$ALL_BUILDS == "all"} {

    # Build all 4 build specs

    # build the default build spec first

    cmpBuild

    # build the other build_specs

    cmpBuildSpecSet default_rom
    cmpBuild

    cmpBuildSpecSet default_romCompress
    cmpBuild

    cmpBuildSpecSet default_romResident
}
# final build, either default, or default_romResident
cmpBuild

# Make sure the default build spec is 'default' before finishing

cmpBuildSpecSet default

cmpProjClose

