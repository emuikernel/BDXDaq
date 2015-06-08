# cmpScriptLib.tcl - TCL scripting commands for project manipulation

# Copyright 1984-2001 Wind River Systems, Inc.

# modification history
# --------------------
# 01q,24jan03,nrj  merged changes made by Jifei Song (1/15/03)
#                  modify cmpBuildMacroGet to call prjBuildMacroGet with right
#                  number of arguments
# 01p,12jun02,cjs  Fix spr 78618 -- doc bug
# 01o,15may02,rbl  fixing SPR 77280, "Project facility messes up
#                  dependencies on a selection  e.g. can't change WDB
#                  connection"
# 01n,23apr02,rbl  fix cmpProjCreate to cope with bsp directories outside the
#                  tornado tree
# 01m,10nov01,dat  Adding cmpSave command
# 01l,05nov01,dat  Fixing cmpBuildSpecCreate
# 01k,01oct01,dat  Gnu-diab infrastructure
# 01j,13nov98,ms   updated docs for refgen.
# 01i,26oct98,ms   fixed cmpCurrentRuleSet.
# 01h,11oct98,ms   added autoscale
# 01g,25sep98,ms   library routines can now be added to a config via cmpProc*
# 01f,15sep98,ms   beefed up cmpTest.
# 01e,03sep98,ms   reworked cmpBuild. Fixed cmpFileRemove yet again.
# 01d,12aug98,ms   fixed cmpFile[Add|Remove]
# 01c,06aug98,ms   added cmpLoad
# 01b,30jul98,ms   added cmpFile[Add|Remove|ListGet], cmpBuildFlagsAdd,
#		   cmpBuildRuleSet, cmpValidate, cmpProjCopy
# 01a,27jul98,ms   written

# DESCRIPTION
# Tornado comes with a project facility that allows one to graphically
# organize source files into projects, change the build rules, and
# configure the operating system. See the Tornado Users Guide for
# details on what facilities the project facility offers.
# This library provides a way to perform these tasks in a script.
# To use this library, one must first source this library from
# wtxtcl. As an example, suppose you wanted to do a clean build of
# your project every night. One could write a script as follows:
#
# example.tcl:
# .CS
#    source [wtxPath host resource tcl app-config Project]cmpScriptLib.tcl
#    cmpProjOpen  myProjectFile.wpj
#    cmpBuild     clean
#    cmpBuild
# .CE
#
# One can execute the script from a UNIX or Windows command shell by
# typing "wtxtcl example.tcl."
# Alternatively, one can type the scripting commands directly into the
# wtxtcl shell.
# 
# SEE ALSO
#    dataDocLib

source  [wtxPath]/host/resource/tcl/app-config/Project/prjLib.tcl
set cmpProjHandle ""
set cmpInteractive ""

###############################################################################
#
# cmpProjOpen - open a project
#
# This routine opens a project file and sets it as the active project
# for all subsequent scripting commands. This first closes
# any project that was previously opened.
#
# PARAMETERS
#    prjFile : project file to open
#
# RETURNS: N/A
# 
proc cmpProjOpen prjFile {
    global cmpProjHandle
    if {"$cmpProjHandle" != ""} {
	cmpProjClose
    }
    set cmpProjHandle [prjOpen $prjFile]
}

###############################################################################
#
# cmpProjClose - close the currently open project
#
# Close and save the currently open project.
#
# RETURNS: N/A
#
# SEE ALSO: cmpSave
# 

proc cmpProjClose {} {
    global cmpProjHandle
    prjClose $cmpProjHandle SAVE
    set cmpProjHandle ""
}

###############################################################################
#
# cmpProjCreate - create a project and open it
#
# This routine creates a project and opens it. If the project already
# exists, it just opens it.
# The last argument to this routine is <prjType>, which can be either
# vxWorks (a system image project), or vxApp (a downloadable application).
# The default is vxWorks.
# The project file to be created must live in a directory which does not
# already exists, and by convension should end in a .wpj extension.
#
# For vxWorks projects, arg should be the name of a BSP directory in
# target/config (e.g., pc386)
# If no project file name is specified, a default project name of
# $(WIND_BASE)/target/proj/<bsp>_<tool>/<bsp>_<tool>.wpj is used, where <bsp>
# is the name of the BSP directory.
#
# For vxApp projects, arg should be the name of a toolchain, in the
# form ::tc_<CPUTOOL> (e.g., ::tc_PPC604gnu).
#
# PARAMETERS
#    arg : prjType-specific argument
#    prjFile : name of the project file to be created
#    prjType : type of project (vxWorks or vxApp)
#    cpu     : optional CPU override (vxWorks only)
#    tool    : optional TOOL override (vxWorks only)
#
# RETURNS: N/A
#

proc cmpProjCreate {bspFile {prjFile ""} {prjType vxWorks} {cpu ""} {tool ""} } {
    global cmpProjHandle

    if {"$cmpProjHandle" != ""} {
	cmpProjClose
    }

    if {"$prjType" == "vxApp"} {
	set tc $bspFile
	::prj_vxApp::create $prjFile $tc
	set cmpProjHandle [prjOpen $prjFile]
	return
    }
    if {$bspFile == [file tail $bspFile]} {
        set bspDir  [wtxPath]target/config/$bspFile
        set bspName $bspFile
    } else {
        set bspDir $bspFile
        set bspName [file tail $bspFile]
    }
    if {"$prjFile" == ""} {
	set prjFile [wtxPath]target/proj/${bspName}_${tool}/${bspName}_${tool}.wpj
    }
    if {![file exists $prjFile]} {
	::prj_vxWorks::create $prjFile $bspDir "" "" $cpu $tool
	set cmpProjHandle [prjOpen $prjFile]
	::prj_vxWorks_hidden::makeGen $cmpProjHandle \
		[file dir $prjFile]/Makefile
	prjClose $cmpProjHandle
    }
    set cmpProjHandle [prjOpen $prjFile]
}

###############################################################################
#
# cmpProjCopy - create a copy of an existing project and open it
#
# Make a copy of an existing project, and open the copy. This is useful
# if you want to make configuration changes but not disturb the
# original project.
# The project file being created has the same restrictions as described
# in cmpProjCreate.
#
# PARAMETERS
#    newPrjFile : project file to create
#    oldPrjFile : existing project file
#
# SEE ALSO
#    cmpProjCreate
#
# RETURNS: N/A
#

proc cmpProjCopy {newPrjFile oldPrjFile} {
    global cmpProjHandle
    if {"$cmpProjHandle" != ""} {
	cmpProjClose
    }

    prjCopy $oldPrjFile $newPrjFile
    set cmpProjHandle [prjOpen $newPrjFile]
}

###############################################################################
#
# cmpProjDelete - delete the currently open project
#
# Delete the currently open project.
# 
# RETURNS: N/A
# 

proc cmpProjDelete {} {
    global cmpProjHandle
    set prjDir [file dir [prjInfoGet $cmpProjHandle fileName]]
    cmpProjClose
    file delete -force $prjDir
}

###############################################################################
#
# cmpProjHandleGet - get the handle to the currently open project
#
# This routine returns the "project handle" to the currently open
# project. Since this library supports only one open project
# at a time, one does not need to use this routine in order to
# use this library.
# However this library is built on top of lower-level libraries that
# must be passed a project handle. These libraries are currently
# not documented
#
# RETURNS: a project handle
# 

proc cmpProjHandleGet {} {
    global cmpProjHandle
    return $cmpProjHandle
}

###############################################################################
#
# cmpBuild - build the project
#
# This routine builds the project using the currently active buildSpec.
# The build output goes to stdout and stderr.
# If no rule is specified, it builds the default rule for the
# currently active buildSpec.
# The "clean" rule removes all binaries.
# 
# PARAMETERS
#    rule : (optional) build rule to invoke
#
# RETURNS: N/A
# 

proc cmpBuild {{rule ""}} {
    global cmpProjHandle
    set dependGenFiles ""
    foreach file [prjFileListGet $cmpProjHandle] {
	if {[prjFileInfoGet $cmpProjHandle $file dependDone] == ""} {
	    lappend dependGenFiles $file
	}
    }
    if {[set nFiles [llength $dependGenFiles]] != 0} {
        set doneFiles 0
        prjStatusLog "generating file dependencies"
        foreach file $dependGenFiles {
            set fileTail [file tail $file]
            prjStatusLog "generating dependencies ($fileTail)" \
                         $doneFiles $nFiles
            prjFileBuildInfoGen $cmpProjHandle $file
            incr doneFiles
        }
        prjStatusLog "done"
    }

    prjSave $cmpProjHandle
    prjBuild $cmpProjHandle $rule
}

###############################################################################
#
# cmpBuildRuleSet - set the build rule for the project
#
# This routine sets the default build rule for the active buildSpec
# within the currently open project. Available build rules depend
# on the project type. For example, vxWorks projects support clean,
# vxWorks, vxWorks_rom, vxWorks_romResident, and vxWorks_romCompress.
# vxApp projects support linkedObjs, archive, and objects.
#
# PARAMETERS
#    rule : build rule to set
#
# RETURNS: N/A
# 

proc cmpBuildRuleSet rule {
    global cmpProjHandle
    set build [prjBuildCurrentGet $cmpProjHandle]
    prjBuildRuleCurrentSet $cmpProjHandle $build $rule
}

###############################################################################
#
# cmpBuildSpecSet - set the active buildSpec
#
# A project can contain one or more buildSpecs. This routine
# is used to set the active buildSpec.
# 
# PARAMETERS
#    build : name of the buildSpec to set as active
#
# RETURNS: N/A
#

proc cmpBuildSpecSet build {
    global cmpProjHandle
    prjBuildCurrentSet $cmpProjHandle $build
}

###############################################################################
#
# cmpBuildSpecCreate - create a new buildSpec
#
# Create a new buildSpec for the project, and set it as
# the active buildSpec.
# One can create a buildSpec by copying an existing buildSpec,
# or create a new one for a different toolchain. If <arg>
# is of the form ::tc_<CPUTOOL> (e.g., ::tc_PPC604gnu), then
# a build is created for that toolchain. Otherwise <arg> is
# interpreted to be the name of an existing build.
#
# PARAMETERS
#    build : name of the buildSpec to create
#    arg : name of a toolchain or existing build
# 
# RETURNS: N/A
#

proc cmpBuildSpecCreate {build arg} {
    global cmpProjHandle
    if {[info procs ::${arg}::name] == ""} {
	prjBuildCopy $cmpProjHandle $arg $build
    } else {
	prjBuildCreate $cmpProjHandle $build $arg
    }
    prjBuildCurrentSet $cmpProjHandle $build
}

###############################################################################
#
# cmpBuildMacroSet - set a build macro
#
# Set the value of a build macro for the active buildSpec.
# Build macros include RAM_LOW_ADRS, CFLAGS, etc.
#
# PARAMETERS
#    macro : name of the build macro to set
#    value : value to assign the macro
# 
# RETURNS: N/A
#

proc cmpBuildMacroSet {macro value} {
    global cmpProjHandle
    prjBuildMacroSet $cmpProjHandle $macro $value
}

###############################################################################
#
# cmpBuildMacroGet - get a build macro
#
# Get the value of a build macro for the active buildSpec.
# Build macros include RAM_LOW_ADRS, CFLAGS, etc.
#
# PARAMETERS
#    macro : name of the build macro to get
# 
# RETURNS: The value of the macro
#

proc cmpBuildMacroGet macro {
    global cmpProjHandle
    
    prjBuildMacroGet $cmpProjHandle [prjBuildCurrentGet $cmpProjHandle] $macro
}

###############################################################################
#
# cmpBuildFlagsAdd - add flags to the build
#
# This routine is just a quick way to append to the build macros
# CFLAGS and CFLAGS_AS. It could also be acheived using
# cmpBuildMacro[GS]et
#
# PARAMETERS
#    flags : flags to add to CFLAGS and CFLAGS_AS
# 
# RETURNS: N/A
#

proc cmpBuildFlagsAdd flags {
    global cmpProjHandle
    set hProj $cmpProjHandle
    set build [prjBuildCurrentGet $hProj]

    set tc [prjBuildTcGet $hProj $build]
    set cc [${tc}::toolFindByFile foo.c]
    set as [${tc}::toolFindByFile foo.s]

    set cflags   "$flags [prjBuildFlagsGet $hProj $build $cc]"
    set asflags  "$flags [prjBuildFlagsGet $hProj $build $as]"

    prjBuildFlagsSet $hProj $build $cc  $cflags
    prjBuildFlagsSet $hProj $build $as  $asflags
}

###############################################################################
#
# cmpProjTcGet - get project's active toolchain
#
# NOMANUAL
#

proc cmpProjTcGet {} {
    global cmpProjHandle
    set build [prjBuildCurrentGet $cmpProjHandle]
    return [prjBuildTcGet $cmpProjHandle $build]
}

###############################################################################
#
# cmpAdd - add components to vxWorks
#
# This routine adds one or more optional vxWorks components to
# the project. For example, one can "cmpAdd INCLUDE_NFS" to add
# the NFS client to the vxWorks image.
# If a folder is specified (e.g., FOLDER_NETWORK), then all
# default components below that folder are added.
# The tool will also add any other components needed by the ones
# being added, as determined by dependency analysis.
# If the global tcl variable cmpInteractive is set to "true", then
# the user will be prompted to confirm before the components
# and dependencies are added.
#
# PARAMETERS
#    components : list of components to add
#
# RETURNS: N/A
# 

proc cmpAdd {components} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set cxrDoc [cxrDocCreate $cmpProjHandle]
    set mxrDoc [mxrDocCreate $cmpProjHandle]
    set cmpSet [$cxrDoc setCreate]
    set cmpSetOld [$cxrDoc setCreate]

    $cmpSet = $components
    while {[set children [$cmpSet get DEFAULTS]] != ""} {
	$cmpSet = [$cmpSet instances Component]
	$cmpSet = [$cmpSet + $children]
    }
    $cmpSet = [$cmpSet instances Component]
    $cmpSet = [$cmpSet - [wccComponentListGet $cmpProjHandle $wcc]]
    if {[$cmpSet contents] == ""} {
	prjWarnLog "no components to add"
	return
    }
    $cmpSetOld = [wccComponentListGet $cmpProjHandle $wcc]
    set components [$cmpSet instances Component]
    set dependancies [cxrDependsGet $cmpSet $cmpSetOld $mxrDoc]
    $cmpSet delete
    $cmpSetOld delete

    puts "adding [lsort $components]\n"
    if {"$dependancies" != ""} {
	puts "NOTE: also adding dependencies\n[lsort $dependancies]\n"
    }

    if {"$cmpInteractive" != ""} {
	puts -nonewline "OK to proceed (y for yes, anything else to cancel)? "
	gets stdin reply
	if {"$reply" != "y"} {
	    puts "canceled"
	    return
        }
    }
    wccComponentAdd $cmpProjHandle $wcc "$components $dependancies"
}

###############################################################################
#
# cmpRemove - remove components from vxWorks
#
# This routine removes one or more optional vxWorks components from
# the project. For example, one can "cmpRemove INCLUDE_NFS" to remove
# the NFS client from the vxWorks image.
# If a folder is specified (e.g., FOLDER_NETWORK), then all
# components below that folder are removed.
# The tool will also remove any components that require by the ones
# being removed, as determined by dependency analysis.
# If the global tcl variable cmpInteractive is set to "true", then
# the user will be prompted to confirm before the components
# and dependencies are removed.
#
# PARAMETERS
#    components : list of components to remove
#
# RETURNS: N/A
# 

proc cmpRemove {components} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set cxrDoc [cxrDocCreate $cmpProjHandle]
    set mxrDoc [mxrDocCreate $cmpProjHandle]
    set cmpSet [$cxrDoc setCreate]

    $cmpSet = $components
    while {[set children [$cmpSet get CHILDREN]] != ""} {
	$cmpSet = [$cmpSet instances Component]
	$cmpSet = [$cmpSet + $children]
    }
    $cmpSet = [$cmpSet & [wccComponentListGet $cmpProjHandle $wcc]]
    if {[$cmpSet contents] == ""} {
	prjWarnLog "no components to remove"
	$cmpSet delete
	return
    }
    set components [$cmpSet contents]
    set wccList [wccComponentListGet $cmpProjHandle $wcc]

    $cmpSet = [cxrSupertree $cmpSet $mxrDoc $wccList]
    $cmpSet = [$cmpSet & $wccList]
    set dependancies [$cmpSet - $components]
    set cmps [$cmpSet contents]
    $cmpSet delete

    puts "removing [lsort $components]\n"
    if {"$dependancies" != ""} {
	puts "WARNING: also removing dependencies\n[lsort $dependancies]\n"
    }

    if {"$cmpInteractive" != ""} {
	puts -nonewline "OK to proceed (y for yes, anything else to cancel)? "
	gets stdin reply
	if {"$reply" != "y"} {
	    puts "canceled"
	    return
        }
    }
    wccComponentRemove $cmpProjHandle $wcc $cmps
}

###############################################################################
#
# cmpListGet - get a list of components in vxWorks
#
# This routine returns the list of components in the current vxWorks
# project.
#
# RETURNS: a list of components
# 

proc cmpListGet {} {
    global cmpProjHandle
    set wcc [wccCurrentGet $cmpProjHandle]
    return [lsort [wccComponentListGet $cmpProjHandle $wcc]]
}

###############################################################################
#
# cmpProcAdd - add library routine(s) to the configuration
#
# This routine adds a library routine to the configuration.
# So if a user knows they want the routine "i", but don't know
# what INCLUDE_* component is belogs to (in this case none), they
# can still bring it into the vxWorks configuration and also
# bring in all components that it needs via dependency analysis.
#
# The implimentation is to create a .cdf file in the project
# directory that corresponds to the routine.
# We will wait until the GUI has this feature before publishing
# it.
#
# NOMANUAL
#

proc cmpProcAdd {routineList} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set hProj $cmpProjHandle
    set dependancies [wccProcDependsGet $hProj $wcc $routineList]
    if {"$dependancies" != ""} {
	puts "NOTE: also adding dependencies\n[lsort $dependancies]\n"
    }

    if {"$cmpInteractive" != ""} {
	puts -nonewline "OK to proceed (y for yes, anything else to cancel)? "
	gets stdin reply
	if {"$reply" != "y"} {
	    puts "canceled"
	    return
        }
    }
    wccProcAdd $hProj $wcc $routineList
    wccComponentAdd $hProj $wcc $dependancies
}

###############################################################################
#
# cmpProcRemove - remove library routine(s) from a vxWorks configuration
#
# See cmpProcAdd
#
# NOMANUAL
#

proc cmpProcRemove {routineList} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set hProj $cmpProjHandle
    wccProcRemove $hProj $wcc $routineList
}

###############################################################################
#
# cmpProcListGet - get a list of library routine(s) in the configuration
#
# See cmpProcAdd
#
# NOMANUAL
#

proc cmpProcListGet {} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set hProj $cmpProjHandle
    wccProcListGet $hProj $wcc
}

###############################################################################
#
# cmpBackup - backup the current component set
#
# Backup a component configuration.
# We will wait until the GUI has this feature before publishing it.
#
# NOMANUAL
# 

proc cmpBackup backUpName {
    global cmpProjHandle
    set wcc [wccCurrentGet $cmpProjHandle]
    wccDelete $cmpProjHandle $backUpName
    wccCreate $cmpProjHandle $backUpName $wcc
    wccCurrentSet $cmpProjHandle $wcc
}

###############################################################################
#
# cmpRestore - restore a backed up component set
# 
# Restore a backed-up component configuration.
# We will wait until the GUI has this feature before publishing it.
#
# NOMANUAL
# 

proc cmpRestore backUpName {
    global cmpProjHandle
    set wcc [wccCurrentGet $cmpProjHandle]
    wccDelete $cmpProjHandle $wcc
    wccCreate $cmpProjHandle $wcc $backUpName
}

###############################################################################
#
# cmpParamSet - set a parameter in the current project
#
# This routine sets the value of a component parameter. For example,
# the component INCLUDE_IOS has parameter MAX_FDS (max # file descriptors
# that can be open at once). One can use "cmpParamSet MAX_FDS 60" to
# increase the value to 60.
#
# RETURNS: N/A
# 

proc cmpParamSet {param value} {
    global cmpProjHandle
    set wcc [wccCurrentGet $cmpProjHandle]
    wccParamSet $cmpProjHandle $wcc $param $value
}

###############################################################################
#
# cmpParamGet - get a parameter
# 
# This routine gets the value of a component parameter. For example,
# the component INCLUDE_IOS has parameter MAX_FDS (max # file descriptors
# that can be open at once). One can use "cmpParamGet MAX_FDS" to
# get the current value of that parameter.
#
# RETURNS: the parameter value, or an empty string if no such parameter exists

proc cmpParamGet param {
    global cmpProjHandle
    set wcc [wccCurrentGet $cmpProjHandle]
    return [wccParamGet $cmpProjHandle $wcc $param]
}

###############################################################################
#
# cmpFileListGet - get a list of files in the project
#
# This routine returns the list of source files in the project.
#
# RETURNS: a list of files in the project
# 

proc cmpFileListGet {} {
    global cmpProjHandle
    return [prjFileListGet $cmpProjHandle]
}

###############################################################################
#
# cmpFileAdd - add a file to the project
#
# This routine adds a source file to the project
#
# PARAMETERS
#	file : full path to the file being added
#

proc cmpFileAdd file {
    global cmpProjHandle
    prjFileAdd $cmpProjHandle $file
}

###############################################################################
#
# cmpFileRemove - remove a file from the project
#
# This routine removes a source file from the project
#
# PARAMETERS
#	file : full path to the file being removed
#

proc cmpFileRemove file {
    global cmpProjHandle
    prjFileRemove $cmpProjHandle $file
}

###############################################################################
#
# cmpLoad - dynamically load components
#
# This routine dynamically loads one or more components to a
# running target. To use this, you must first be connected
# to a tgtsvr using wtxToolAttach.
# This routine will extract all needed object modules from the
# vxWorks archive, and link them together. It will also build the
# initialization code needed to initialize the component(s) and
# all dependent components. It then loads the modules and the
# initialization code, and calls the initialization code.
#
# Example: To load the vxWorks target shell, do "cmpLoad FOLDER_SHELL"
#
#
# PARAMETERS
#    cmps : components to load
#    mods : (optional) set of additional object modules to load
#
# RETURNS: N/A
#
# SEE ALSO
#	wtxToolAttach
#

proc cmpLoad {cmps {mods ""}} {
    set hProj [cmpProjHandleGet]

    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    $cxrSet = $cmps
    while {[set children [$cxrSet get DEFAULTS]] != ""} {
	$cxrSet = [$cxrSet instances Component]
	$cxrSet = [$cxrSet + $children]
    }
    set cmps [$cxrSet instances Component]
    $cxrSet delete
    wccComponentLoad $hProj $cmps $mods
}

###############################################################################
#
# autoscale - automatically scale vxWorks
#
# configure vxWorks components based on what is required by the
# project source files. The default is to tell you what scaling can/should
# be done, and why. Passing parameter "terse" prints only terse information.
# Passing paramter "shrink" causes autoscaling to actually be done.
#
# PARAMETERS
#    arg : (optional) default is verbose. can also be shrink or terse.
#
# RETURNS: N/A
#

proc autoscale {{verbose verbose}} {
    set hProj [cmpProjHandleGet]
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set cxrSetOrig [$cxrDoc setCreate]
    $cxrSetOrig = [cmpListGet]
    global cmpInteractive

    if {"$verbose" == "shrink"} {
	set cmpInteractive ""
	cmpRemove [cmpListGet]
	set cmpInteractive true
    }

    set autoscaleInfo [prj_vxWorks_hidden::autoscale $hProj verbose]

    $cxrSet = [lindex $autoscaleInfo 0]
    puts "need to add: [$cxrSet - [$cxrSetOrig instances]]"
    puts "can remove: [$cxrSetOrig - [$cxrSet instances]]"

    if {"$verbose" == "verbose"} {
	foreach info [lrange $autoscaleInfo 1 end] {
	    puts "module [lindex $info 0]"
	    puts "imports symbols [lindex $info 1]"
	    puts "pulls in extra modules [lindex $info 2]"
	    if {"[lindex $info 3]" == ""} {
		continue
	    }
	    puts "which corresponds to extra components [lindex $info 3]"
	}
    }

    if {"$verbose" == "shrink"} {
	set cmpInteractive ""
	set extra INCLUDE_USER_APPL
	if {"[$cxrSetOrig & INCLUDE_SIO]" == "INCLUDE_SIO"} {
	    lappend extra INCLUDE_SIO
	}
	cmpAdd "$extra [lindex $autoscaleInfo 0]"
	set cmpInteractive true
	cmpBuild
    }

    $cxrSet     delete
    $cxrSetOrig delete
}

###############################################################################
#
# cmpUnavailListGet - list unavailable components
#
# This routine returns a list of components that are known but not
# available. A component is not available if it is not installed
# in the tree (e.g., an optional product), or it requires an
# unavailable component.
# 
# RETURNS: a list of components
#

proc cmpUnavailListGet {} {
    set hProj [cmpProjHandleGet]
    set cxrDoc [cxrDocCreate $hProj]
    set mxrDoc [mxrDocCreate $hProj]
    return [cxrUnavailListGet $cxrDoc $mxrDoc]
}

###############################################################################
#
# cmpValidate - validate the component configuration
#
# Check if the current component configuration is valid.
# It returns a list of the form {{cmp1 err1} {cmp2 err2} ...}
# where cmpX is the name of a misconfigured component, and
# errX is an error string describing what is wrong with the
# component.
#
# RETURNS: an error list, or an empty string if the configuration is valid.
# 

proc cmpValidate {} {
    global cmpProjHandle
    global cmpInteractive
    set wcc [wccCurrentGet $cmpProjHandle]
    set info [wccValidate $cmpProjHandle $wcc]
    if {"$cmpInteractive" != ""} {
	foreach elem $info {
	    puts $elem
	}
	return
    }
    return $info
}

###############################################################################
#
# cmpTest - test the component descriptor files for errors
#
# This routine tests the component descriptor files for semantic
# errors. For example, it makes sure all components are contained
# in exactly one folder. It prints all diagnostic information
# to stdout. This routine is a diagnostic tool for component
# developers to test that their new components were written
# correctly.
# It has two optional parameters. <components> allows one to
# restrict the set of components being tested.
# <buildTest> allows one to verify that each component can
# be built individually. The most common reason a component
# would fail the build test is if the component descriptor
# does not have a HDR_FILE containing a prototype for the
# components initialization routine.
#
# PARAMETERS
#    components : (optional) list of components to test (default is all)
#    buildTest : (optional) pass any value to see if components can build
#
# RETURNS: N/A
# 

proc cmpTest  {{components ""} {buildTest ""}} {
    global cmpProjHandle
    mxrDocValidate [mxrDocCreate $cmpProjHandle]
    cxrDocValidate $cmpProjHandle $components $buildTest
}

###############################################################################
#
# cmpInfo - get info about a set of components
#
# Print basic information about a component, including the
# subtree (other components required by this component), and
# supertree (other components that require this component).
# The subtree and supertree are computed using the engine's
# dependency analyizer.
# This routine is primarily used by component authors who
# want to check on the scalability of their components.
#
# PARAMETERS
#    components : list of components to analyze
#
# RETURNS: N/A
# 

proc cmpInfo components {
    global cmpProjHandle
    set mxrDoc [mxrDocCreate $cmpProjHandle]
    set cxrDoc [cxrDocCreate $cmpProjHandle]
    set cmpSet [$cxrDoc setCreate]
    $cmpSet = $components
    foreach line [cxrCmpInfoGet $cmpSet $mxrDoc] {
	puts $line
    }

    $cmpSet delete
}


###############################################################################
#
# cmpSave - save the current project info
#
# Update the project file with the latest configuration.  No update occurs
# if the project configuration has not changed.
#
# PARAMETERS: N/A
#
# SEE ALSO: cmpBuild
#
# RETURNS: N/A
# 

proc cmpSave {} {
    global cmpProjHandle

    # update file, if changes have been made
    prjSave $cmpProjHandle
}


