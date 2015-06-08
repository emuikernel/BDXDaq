# prjLib.tcl - project manager library
#
# modification history
# --------------------
# 02e,04feb03,rbl  speed up project open for large projects (e.g. godzilla)
# 02d,15may02,rbl  modify last change to fix SPR 77344 -- 
#                  Projects containing user source don't copy correctly.
#                  fix bug where project version was not being written
#                  correctly into workspace. 
# 02d,17may02,cjs  Moving prjDirDefault to Utils.tcl
# 02c,30apr02,cjs  Fix spr 73093 -- copy of downloable project screws up builds
# 02b,10dec01,rbl  fix bug where prjSaveAs was re-generating the old
#                  Makefile...
# 02a,19oct01,cjs  Fix spr 25520 -- use recursive directory creation in
#                  wizards
# 01z,11oct01,rbl  make it possible (still not recommended!) to edit project
#                  files on Windows
# 01z,15oct01,fle  made it use the Wind package
# 01y,27sep01,rbl  update project path functions (prjRelPath, prjFullPath) to
#                  T3 versions to support e.g. WIND_PROJ_BASE,
#                  WIND_SOURCE_BASE, and calling with no project (hProj
#                  argument blank)
#                  write project file to temporary file first when saving, 
#                  to avoid corruption or loss on error or interruption
# 01y,04oct01,cjs  Fixing defaultPrjWarnLogFuncGet
# 01x,08feb01,t_m  Fix for ARM GCC2.97 depends path
# 01w,22mar99,rbl  escaping special regular expression characters in path
#                  substitutions
#                  fixing prjSaveAs so it returns the new filename even 
#                  when no filename is passed in
# 01v,19mar99,rbl  extending the special handling in prjRelPathRelativeTo so
#                  that the new project gets the new project directory in its
#                  -I include paths and in the .cdf search path
# 01u,16mar99,cjs  Make sure that makeGen is supported for project before
#                  calling it (won't work for workspaces, for example)
# 01t,12mar99,cjs  Adding call to makeGen() in project save routines
# 01s,09mar99,rbl  added new description as parameter to prjCopy and
#                  prjSaveAs. Updated prjCreateable to check if any core files
#                  are present and fixed prjOverwritable to correctly handle 
#                  core files.
# 01r,01mar99,rbl  Fix prjRelPathRelativeTo: handle core files first,
#                  then make other substitutions.
# 01q,02feb99,rbl  Changes to handle missing or read-only projects
# 01p,21jan99,cjs  Alter prjCopy() to copy project file in memory, not one on
#                  disk
# 01o,24nov98,jmp  added WIND_HOST_TYPE substitution in prj[Rel|Full]Path.
# 01n,10nov98,cjs  add routine to fetch default warning and logging
#                  output handler   
# 01m,26oct98,ms   on UNIX, only emit debug output if TORNADO_DEBUG is set
# 01l,09sep98,cjs  added prjHasComponents() 
# 01k,04sep98,cjs  added optional params to prjCoreCreate(); do case
#                  insensitive subst in prjRelPath()
# 01j,12aug98,ms   fixed prjCopy, removed prjSaveAs.
# 01h,31jul98,ms   added prjDirDefault
# 01g,29jul98,ms   new syntax for prj[Rel|Full]Path
# 01g,20jul98,ms   prjCreateable fails if directory already exists.
# 01f,07jul98,ms   added hook in prjClose.
# 01e,18jun98,cjs  changed signature of prjStatusLog 
# 01d,18jun98,jmp  added return value to prjTagRemove{}.
# 01c,01jun98,ms   added prjStatusLog and prjWarnLog
# 01c,08apr98,ms   fixed prjSaveAs
# 01b,02apr98,ms   prjInfo hProj changed - now contains timestamp of last mod.
#		   prjClose now defaults to not saving the file.
# 01a,13mar98,ms   written.
#
# DESCRIPTION
# This is the generic project managment library.
# It manages a projects files, build settings, and other project information.
#
# Each project has a "type", such as vxWorks, WDB, bootROM, etc.
# This library also interacts with specific project-type libraries that
# handle project information specific to that project type (for example,
# the vxWorks project manager needs to know about what OS components
# are included).
# Adding support for a new project-type is easy; just add a TCL file
# called prjType_*.tcl. The project library will source that file on
# startup. The new project type library needs to provide certain
# functions in a TCL namespace:
#   create          	launch wizard to create a new project of this type
#   name		the name of this project type (e.g., vxWorks)
#   toolChainListGet	returns the list of compatible toolchains
#   buildRuleListGet   	returns list of available build rules (make targets)
#   buildRuleDefault	returns the default build rule (make targets)
#   buildModify hProj build	modify a build spec for this project type
#   build image		build the current image
#   coreFilesGet        returns list of files (apart from the project file itself) 
#                       that define the "core" of the project. In a 
#                       "save as", these are the files that get copied. 
# The new project-type manager should then call prjTypeAdd to register
# itself with the project manager.
#
# This library provides low-level routines to allow the file manager,
# build manager and project-type managers to store and retrieve information
# in a project.
# Data is stored and retrieved under tags using the routines:
#    prjTagDataSet    hProj tag data
#    prjTagDataGet    hProj tag
#    prjTagListGet    hProj
#    prjTagRemove     hProj tag
# For example, the file manager might store it's data under the tag
# PROJECT_FILES.
#
# The file manager is implemented in prjFileLib.tcl.
# The build manager is implemented in prjBuildLib.tcl.
#
# XXX - if two workspaces open and modify the same project, they will
# conflict with each other. Should we try to avoid this with a lock file?
#

package require Wind

set prjTypes ""
set prjHandleIx 0
variable defaultPrjStatusLogRtn
variable defaultPrjWarnLogRtn
if {("[wtxHostType]" == "x86-win32") || ([info exists env(TORNADO_DEBUG)])} {
	set defaultPrjStatusLogRtn puts
	set defaultPrjWarnLogRtn puts
    set prjStatusLogRtn $defaultPrjStatusLogRtn
    set prjWarnLogRtn $defaultPrjWarnLogRtn
} else {
    proc nop arg {}
	set defaultPrjStatusLogRtn nop
	set defaultPrjWarnLogRtn  nop
    set prjStatusLogRtn $defaultPrjStatusLogRtn
    set prjWarnLogRtn $defaultPrjWarnLogRtn
}
# prjTagData(hProj,tag)
if {[info exists prjTagData]} {
    unset prjTagData
}
# prjInfo(hProj,type)		type = fileName, changed, ...
if {[info exists prjInfo]} {
    unset prjInfo
}

#########################################################################
#
# prjSrcDirDefault - default directory for creating new projects.
#
# This fetches the value of environment variable WIND_SOURCE_BASE if 
# defined, or returns "" otherwise. It returns a UNIX-style path, i.e. with 
# forward slashes. 
#
# RETURNS: value of the environment variable WIND_SOURCE_BASE if defined, or
# "" otherwise
#
# ERRORS: N / A
#
proc prjSrcDirDefault {} {
    if {[info exist ::env(WIND_SOURCE_BASE)]} {
        set srcBase $::env(WIND_SOURCE_BASE)
        ::filePathFix srcBase
	return $srcBase
    }
    return ""
}

###############################################################################
#
# prjRelPath - convert to path names relative to project directory
#
# This function prepares a path string for output to a file 
# (where we want paths to be relative to $(PRJ_DIR),  
# $(WIND_SOURCE_BASE), $(WIND_PROJ_BASE) and $(WIND_BASE)) by making the following
# substitutions in the following order:
#
# $(PRJ_DIR) is substituted for the directory
# $(WIND_SOURCE_BASE) is substituted for the value of the WIND_SOURCE_BASE
# environment variable if this variable is defined
# $(WIND_PROJ_BASE) is substituted for the WIND_PROJ_BASE directory
# ($(WIND_BASE)/target/proj unless the WIND_PROJ_BASE environment variable 
# is defined)
# $(WIND_BASE) is substituted for the WIND_BASE directory
# $(WIND_HOST_TYPE) is substituted for the WIND_HOST_TYPE string
#
# If the hProj parameter is blank, only $(WIND_SOURCE_BASE), $(WIND_PROJ_BASE), 
# $(WIND_BASE) and 
# $(WIND_HOST_TYPE) are substituted. 
#
# SYNOPSIS 
#   prjRelPath  hProj files 
#
# PARAMETERS 
#   hProj:  handle of project
#   files:  string to convert
#
# RETURNS: path with the above substitutions made.  
#
# ERRORS: N / A
#
proc prjRelPath {hProj files} {
    set prjDir ""
    if {$hProj != ""} {
        set prjDir [file dirname [prjInfoGet $hProj fileName]]
    }
    return [prjRelPathRelativeTo $hProj $files $prjDir]
}

###############################################################################
#
# prjFullPath - convert to full path names, from paths relative to 
#               project directory. 
#
# This function prepares a path string read from a file, which is allowed to 
# contain the strings $(PRJ_DIR), $(WIND_SOURCE_BASE), $(WIND_PROJ_BASE), 
# $(WIND_BASE) and $(WIND_HOST_TYPE), by making the following substitutions:
#
# the project directory is substituted for $(PRJ_DIR)
# the WIND_SOURCE_BASE environment variable is substituted 
# for $(WIND_SOURCE_BASE)
# the WIND_PROJ_BASE directory ($(WIND_BASE)/target/proj unless the 
# WIND_PROJ_BASE environment variable is defined) is substituted
# for $(WIND_PROJ_BASE)
# the WIND_BASE directory is substituted for $(WIND_BASE)
# the WIND_HOST_TYPE string is substituted for $(WIND_HOST_TYPE) 
#
# If the hProj parameter is blank, only $(WIND_SOURCE_BASE), 
# $(WIND_PROJ_BASE), $(WIND_BASE) and $(WIND_HOST_TYPE) are substituted. 
#
# SYNOPSIS 
#   prjRelPath  hProj files 
#
# PARAMETERS 
#   hProj:  handle of project
#   files:  string to convert
#
# RETURNS: path with the above substitutions made.  
#
# ERRORS: N / A
#
proc prjFullPath {hProj files} {
    set prjDir ""
    if {$hProj != ""} {
        set prjDir [file dirname [prjInfoGet $hProj fileName]]
    }
    return [prjFullPathRelativeTo $hProj $files $prjDir]
}

#####################################################################################
#
# prjRelPathRelativeTo - convert to path names relative to project directory
#
# This function prepares a path string for output to a file 
# (where we want paths to be relative to $(PRJ_DIR),  
# $(WIND_SOURCE_BASE), $(WIND_PROJ_BASE) and $(WIND_BASE)) by making the 
# following substitutions in the following order:
#
# $(PRJ_DIR) is substituted for the directory
# $(WIND_SOURCE_BASE) is substituted for the value of the WIND_SOURCE_BASE
# environment variable if this variable is defined
# $(WIND_PROJ_BASE) is substituted for the WIND_PROJ_BASE directory
# ($(WIND_BASE)/target/proj unless the WIND_PROJ_BASE environment 
# variable is defined)
# $(WIND_BASE) is substituted for the WIND_BASE directory
# $(WIND_HOST_TYPE) is substituted for the WIND_HOST_TYPE string
#
# If the hProj parameter is blank, only $(WIND_SOURCE_BASE), 
# $(WIND_PROJ_BASE), $(WIND_BASE) and 
# $(WIND_HOST_TYPE) are substituted. 
#
# "core files" (those returned by ${prjType}::coreFilesGet) 
# are handled differently: if $newPrjDir is specified, the 
# paths to the core files are fixed so that the newPrjDir is 
# referenced.
#
# Also -I{$oldPrjDir} is mapped to -I{$newPrjDir}, but paths 
# deeper than this are left pointing to $oldPrjDir. 
#
# If the hProj parameter is blank, only $(WIND_SOURCE_BASE), 
# $(WIND_PROJ_BASE), 
# $(WIND_BASE) and 
# $(WIND_HOST_TYPE) are substituted. 
#
# PARAMETERS 
#
#   hProj:  handle of project
#   files:  string to convert
# newPrjDir: directory, if blank the project directory for hProj is used.
# tag: optional tag argument so that we can give special handling to some 
#      tags. At the moment only WCC__CDF_PATH gets special handling - any paths 
#      which are ${oldPrjDir} (no deeper) get moved to ${newPrjDir}
#
# RETURNS:   files string with substitutions made
# ERRORS: N / A
#
proc prjRelPathRelativeTo {hProj files {newPrjDir ""} {tag ""}} {
	if {$newPrjDir == "" && $hProj != ""} {
		set prjDir [file dirname [prjInfoGet $hProj fileName]]
	} else {
		set prjDir $newPrjDir
	}

        set prjDirParent [file dirname $prjDir]
        set windBase [wtxPath]
        removeTrailingSlash windBase
        set srcBase [prjSrcDirDefault]

	#
	# Handle special cases.
	# Do special cases BEFORE making other substitutions, so that we catch cases where the 
	# old project directory is beneath WIND_BASE...
	#
	if {$newPrjDir != "" && $hProj != ""} {
		set oldPrjDir [file dirname [prjInfoGet $hProj fileName]]
    	        set prjType [prjTypeGet $hProj]

		#
		# core files: $oldPrjDir/$coreFile is mapped to $newPrjDir/$coreFile
		#
		if {[info commands ${prjType}::coreFilesGet] != ""} {
			foreach coreFile [${prjType}::coreFilesGet] {
				::regsubPath -all [EscapeRegexpCharacters "$oldPrjDir/$coreFile"] $files $newPrjDir/$coreFile files
			}
		}

		#
		# -I include paths: -I{$oldPrjDir} is mapped to -I{$newPrjDir}, but paths 
		# deeper than this are left pointing to $oldPrjDir. So we match 
		# -I{$oldPrjDir} followed by a possible "/", then whitespace or "\" continuation or end of string
		#
		::barePathSubstitute files "-I$oldPrjDir" "-I$newPrjDir"
		#
		# WCC__CDF_PATH paths: {$oldPrjDir} is mapped to {$newPrjDir}, but paths 
		# deeper than this are left pointing to $oldPrjDir. So we match 
		# {$oldPrjDir} followed by a possible "/", then whitespace or "\" continuation or end of string
		#
		if {$tag == "WCC__CDF_PATH"} {
			::barePathSubstitute files "$oldPrjDir" "$newPrjDir"
		}
	}

    if {$hProj != ""} {
        ::regsubPath -all "[EscapeRegexpCharacters ${prjDir}](/| |\t)" $files {$(PRJ_DIR)\1} files
        ::regsubPath -all "[EscapeRegexpCharacters ${prjDir}](\$)" $files {$(PRJ_DIR)\1} files

        ::regsubPath -all [EscapeRegexpCharacters $prjDirParent]/ $files {$(PRJ_DIR)/../} files
    }
    if {$srcBase != ""} {
        ::regsubPath -all "[EscapeRegexpCharacters [prjSrcDirDefault]](/| |\t)" $files {$(WIND_SOURCE_BASE)\1} files
        ::regsubPath -all "[EscapeRegexpCharacters [prjSrcDirDefault]](\$)" $files {$(WIND_SOURCE_BASE)\1} files
    }
    ::regsubPath -all "[EscapeRegexpCharacters [prjDirDefault]](/| |\t)" $files {$(WIND_PROJ_BASE)\1} files
    ::regsubPath -all "[EscapeRegexpCharacters [prjDirDefault]](\$)" $files {$(WIND_PROJ_BASE)\1} files
    ::regsubPath -all "[EscapeRegexpCharacters ${windBase}](/| |\t)" $files {$(WIND_BASE)\1} files
    ::regsubPath -all "[EscapeRegexpCharacters ${windBase}](\$)" $files {$(WIND_BASE)\1} files
    ::regsubPath -all [wtxHostType] $files \$\(WIND_HOST_TYPE\) files

    return $files
}

###############################################################################
#
# prjFullPathRelativeTo - convert to full path names, from paths relative to project directory. 
#
# This function prepares a path string read from a file, which is allowed to 
# contain the strings $(PRJ_DIR), $(WIND_SOURCE_BASE), $(WIND_PROJ_BASE), 
# $(WIND_BASE) and $(WIND_HOST_TYPE), by making the following substitutions:
#
# the project directory is substituted for $(PRJ_DIR)
# the WIND_SOURCE_BASE environment variable is substituted for 
# $(WIND_SOURCE_BASE)
# the WIND_PROJ_BASE directory ($(WIND_BASE)/target/proj 
# unless the WIND_PROJ_BASE environment variable is defined) 
# is substituted for $(WIND_PROJ_BASE)
# the WIND_BASE directory is substituted for $(WIND_BASE)
# the WIND_HOST_TYPE string is substituted for $(WIND_HOST_TYPE) 
#
# PARAMETERS 
# hProj: handle of project. If newPrjDir is specified this may be blank
# If hProj and newPrjDir are both blank, only $(WIND_SOURCE_BASE), 
# $(WIND_PROJ_BASE), $(WIND_BASE) and $(WIND_HOST_TYPE) are substituted. 
#   files:  string to convert
# newPrjDir: directory, if blank the project directory for hProj is used.
#
# RETURNS:   files string with substitutions made
# ERRORS: N / A
#
proc prjFullPathRelativeTo {hProj files {newPrjDir ""}} {
    if {$newPrjDir == "" && $hProj != ""} {
	set prjDir [file dirname [prjInfoGet $hProj fileName]]
    } else {
	set prjDir $newPrjDir
    }

    if {$hProj != "" || $newPrjDir != ""} {
        regsub -all {\$\(PRJ_DIR\)} $files \
		    $prjDir files
    }

    set srcBase [prjSrcDirDefault]
    if {$srcBase != ""} {
        regsub -all {\$\(WIND_SOURCE_BASE\)/} $files [prjSrcDirDefault]/ files
    }
    regsub -all {\$\(WIND_PROJ_BASE\)/} $files [prjDirDefault]/ files
    regsub -all {\$\(WIND_BASE\)/} $files [wtxPath] files
    regsub -all {\$\(WIND_HOST_TYPE\)} $files [wtxHostType] files

    # eat a single /../ caused by $(PRJ_DIR)/../ 
    # timw: added -all

    regsub -all /\[^/\\.\]+/\\.\\./ $files "/" files

    return $files
}

###############################################################################
#
# prjInfoGet - get info about the project
#
# This routine is for internal use only
#

proc prjInfoGet {hProj infoType} {
    global prjInfo

    return $prjInfo($hProj,$infoType)
}

###############################################################################
#
# prjInfoSet - set info about the project
#
# This routine is for internal use only
#

proc prjInfoSet {hProj infoType info} {
    global prjInfo

    set prjInfo($hProj,$infoType) $info
}

###############################################################################
#
# prjNameGet - get the name of the project
#
# RETURNS: the name of the project
#

proc prjNameGet hProj {
    return [file tail [file rootname [prjInfoGet $hProj fileName]]]
}

###############################################################################
#
# prjTypeAdd - add a project type
#
# RETURNS: N/A
# ERRORS: N/A
#

proc prjTypeAdd prjType {
    global prjTypes

    if {[catch "namespace eval $prjType {info procs create}" val]} {
	error "invalid project namespace $prjType"
    }
    if {$val != "create"} {
	error "invalid project namespace $prjType"
    }

    lappend prjTypes $prjType
}

###############################################################################
#
# prjTypeRemove - remove a project type
#
# RETURNS: N/A
# ERRORS: non existant project
#

proc prjTypeRemove prjType {
    global prjTypes

    set ix [lsearch $prjTypes $prjType]
    if {$ix == -1} {
        error "$prjType doesn't exist"
    }

    set prjTypes [lreplace $prjTypes $ix $ix]
}

###############################################################################
#
# prjTypeListGet - get a list of available project types
#
# RETURNS: a list of project types
# ERRORS: N/A
#

proc prjTypeListGet {} {
    global prjTypes

    return $prjTypes
}

###############################################################################
#
# prjTypeIsValid - check if a project type is valid
#
# RETURNS: TRUE if the project type is valid, else FALSE
#

proc prjTypeIsValid prjType {
    return [expr [lsearch [prjTypeListGet] $prjType] != -1]
}

###############################################################################
#
# prjHandleValid - check if a project handle is valid
#
# ERRORS: invalid project handle
# RETURNS: TRUE
#

proc prjHandleValidCheck hProj {
    global prjInfo

    if {![info exists prjInfo($hProj,fileName)]} {
	error "invalid project handle $hProj"
    }

    return 1
}

###############################################################################
#
# prjTagDataSet - set project data
#
# This routine saves project data under a given tag
#
# RETURNS: N/A
# ERRORS: invalid project handle, invalid tag
#

proc prjTagDataSet {hProj tag data} {
    prjHandleValidCheck $hProj

    global prjTagData
    set prjTagData($hProj,$tag) $data

    prjInfoSet $hProj changed [clock seconds]
}

###############################################################################
#
# prjTagDataGet - get project data
#
# This routine returns project data stored under a given tag
#

proc prjTagDataGet {hProj tag} {
    prjHandleValidCheck $hProj

    global prjTagData
    if {![info exists prjTagData($hProj,$tag)]} {
	return ""
    }
    return $prjTagData($hProj,$tag)
}

###############################################################################
#
# prjDirtyGet - gets "dirty" state of project
#
# ARGUMENTS: hProj:   project handle
#
# RETURNS:   1 if project is dirty, 0 if project is clean
#
proc prjDirtyGet {hProj} {
	if {[::prjInfoGet $hProj changed] != ""} {
		return 1
	} else {
		return 0
	}
}

###############################################################################
#
# prjProjectFileWritable - checks if the project file is writable
#
# ARGUMENTS: hProj:   project handle
#
# RETURNS:   1 if project is writable, 0 if not writable
#

proc prjProjectFileWritable {hProj} {
	return [file writable [prjInfoGet $hProj fileName]]	
}

###############################################################################
#
# prjTagListGet - return the list of tags under which project data is stored
#
# RETURNS: a list of tags
#

proc prjTagListGet {hProj {prefix ""}} {
    prjHandleValidCheck $hProj

    global prjTagData
    set tags ""
    foreach ix [array names prjTagData $hProj,${prefix}*] {
	regsub $hProj, $ix "" tag
	lappend tags $tag
    }

    return [lsort $tags]
}

###############################################################################
#
# prjTagRemove - remove a data tag (and its associated data) from the project
#

proc prjTagRemove {hProj tag} {
    prjHandleValidCheck $hProj

    global prjTagData
    if {![info exists prjTagData($hProj,$tag)]} {
    	# Tag does not exist, return ERROR
    	return 0
    }

    unset prjTagData($hProj,$tag)
    prjInfoSet $hProj changed [clock seconds]

    return 1
}

###############################################################################
#
# prjSave - write out a project
#
# This routine only saves info if the project has changed
#
# RETURNS: N/A
# ERRORS: invalid project handle
#

proc prjSave hProj {
    catch "prjInfoGet $hProj changed" changed

    if {"$changed" == ""} {
	return
    }

    set prjFile [prjInfoGet $hProj fileName]
    # write to a temporary file to avoid corrupting the project file
    set tempFile $prjFile
    append tempFile ".windtemp"

    set prjFileError ""
    catch {
	set fd [open $tempFile w+]
	
	fconfigure $fd -translation lf
   	puts $fd "Document file - DO NOT EDIT"
    	puts $fd ""
    	foreach tag [prjTagListGet $hProj] {
	    puts $fd "<BEGIN> [prjRelPath $hProj $tag]"
	    puts $fd [prjRelPath $hProj [prjDataSplit \
                [prjTagDataGet $hProj $tag]]]
	    puts $fd "<END>\n"
	}
	close $fd
    } prjFileError

    if {$prjFileError == ""} {
	if {![file exists $prjFile] || [file writable $prjFile]} {
	    file copy -force $tempFile $prjFile
	} else {
	    error "Permission denied: can't overwrite read-only file \
		   $prjFile"
	}
	prjInfoSet $hProj changed ""
	file delete $tempFile
    } else {
	catch {
	    close $fd
	    file delete $tempFile
	}
	error "Save of project [prjNameGet $hProj] failed: $prjFileError"
    }

    # Not the cleanest way to do things.  We generate the makefile here
    # in order to guarantee that it always exists.  This allows
    # subprojects to be built w/o calling buildCmdsGet().  We can't count
    # on makeGen() existing, as this could be a workspace project handle 
    set hiddenNamespace [::prjTypeGet $hProj]_hidden
    if {[namespace eval ${hiddenNamespace} {info procs makeGen}] != ""} {
    	${hiddenNamespace}::makeGen $hProj [file dir \
	    [prjInfoGet $hProj fileName]]/Makefile
    }
}

###############################################################################
#
# prjSaveAs {hProj, fileName, fFailIfExists} 
# - write out a project file to a new fileName.
#
# fFailIfExists defaults to 1 (TRUE)
#
# fFullPaths defaults to 0 (FALSE): prjRelPath is used on filenames. 
# if fFullPaths is set to 1 (TRUE) prjFullPath is used on filenames.
#
# This routine only saves the project file.  The rest has to be dealt w/
# explicitly.  Marks old project file clean. 
#
# RETURNS: The new filename, even when no filename is passed in
# ERRORS: invalid project handle, can't create new file
#
proc prjSaveAs {hProj {fileName ""} {fFailIfExists 1} {newDescription ""}} {
    if {$fileName == ""} {
		prjSave $hProj
		return [prjInfoGet $hProj fileName]
    }

    # Make sure we save all the paths relative to the NEW project directory...
    set oldProj [prjInfoGet $hProj fileName]
    set oldProjName [file rootname [file tail $oldProj]]
    set newProjName [file rootname [file tail $fileName]]
    set oldProjDir [file dir $oldProj]
    set newProjDir [file dirname $fileName]
    set prjType [prjTypeGet $hProj]

    # Error if the file exists
    if {$fFailIfExists && \
	[file exists $fileName]} {
	error "Project Save As: file $fileName exists"
    }
		
    set fd [open $fileName w+]
    fconfigure $fd -translation lf
    set error ""
    catch {
   	puts $fd "Document file - DO NOT EDIT"
   	puts $fd ""
	# write everything except description
   	foreach tag [prjTagListGet $hProj] {
	    if {$tag != "userComments"} {
	    	set tagData [prjTagDataGet $hProj $tag]
                if {$newProjName != ""} {
                    # Replace e.g. oldProject.out, but NOT oldProject/foo.c
                    # in tag data 
                    if {[regexp $oldProjName\\.\[^/\]*$ $tagData] == 1} {
                        regsub -all $oldProjName $tagData $newProjName tagData
                    }
                    # Replace e.g. oldProject.out, but NOT oldProject/foo.c
                    # in tag name as well 
                    if {[regexp $oldProjName\\.\[^/\]*$ $tag] == 1} {
                        regsub -all $oldProjName $tag $newProjName tag
                    }
                } 
		puts $fd "<BEGIN> [prjRelPathRelativeTo $hProj \
		$tag $newProjDir]"

		# pass the tag type to prjRelPathRelativeTo so that WCC__CDF_PATH gets special handling...
		puts $fd [prjRelPathRelativeTo $hProj [prjDataSplit \
			$tagData] $newProjDir $tag]
		puts $fd "<END>\n"
		}
	}

	# write description
	set tag "userComments"
	set tagData $newDescription
	puts $fd "<BEGIN> [prjRelPathRelativeTo $hProj $tag $newProjDir]"
	puts $fd [prjRelPathRelativeTo $hProj [prjDataSplit \
	    $tagData] $newProjDir]
	puts $fd "<END>\n"
    } error
    close $fd

    # If we have successfully saved as, mark the old project clean
    if {$error == ""} {
   	prjInfoSet $hProj changed ""
    } else {
	error "Project Save As failed: $error"
    }

    # Not the cleanest way to do things.  We generate the makefile here
    # in order to guarantee that it always exists.  This allows
    # subprojects to be built w/o calling buildCmdsGet().  We can't count
    # on makeGen() existing, as this could be a workspace project handle 
    set hiddenNamespace [::prjTypeGet $hProj]_hidden
    if {[namespace eval ${hiddenNamespace} {info procs makeGen}] != ""} {
        set hProjNew [prjOpen $fileName]
    	${hiddenNamespace}::makeGen $hProjNew $newProjDir/Makefile
        prjClose $hProjNew
    }

    return $fileName
}

###############################################################################
#
# prjOpen - open a project
#
# RETURNS: a handle to the project
# EXCEPTIONS: no such project
#

proc prjOpen {prjFile {how "r"}} {
    global prjInfo
    global prjHandleIx
    global prjTagData

    set hProj [incr prjHandleIx]
    set fd [open $prjFile $how]
    fconfigure $fd -translation auto

    set prjInfo($hProj,fileName) $prjFile
    set prjInfo($hProj,changed) ""

    set tag ""
    set data ""
    set fileSize [file size $prjFile]
    foreach line [split [prjFullPath $hProj [read $fd $fileSize]] \n] {
		if {[regexp {<BEGIN> .*} $line] == 1} {
			regsub {<BEGIN> } $line "" tag
			continue
			}
		if {"$line" == "<END>"} {
			set prjTagData($hProj,$tag) [prjDataJoin \
			 [string trimright $data \n]]
			set tag ""
			set data ""
			continue
		}
		if {$tag == ""} {
			continue
		}
		set data "${data}${line}\n"
    }
    close $fd

	prjInfoSet $hProj PROJECT_FILE_STATUS OK

    return $hProj
}

###############################################################################
#
# prjOpenDead - open a "dead" project, one who's file cannot be found
#
# RETURNS: a handle to the project
# EXCEPTIONS: none
#

proc prjOpenDead {prjFile} {
    global prjInfo
    global prjHandleIx
    global prjTagData

    set hProj [incr prjHandleIx]

    prjInfoSet $hProj fileName $prjFile
    prjInfoSet $hProj PROJECT_FILE_STATUS CANNOT_FIND_PROJECT_FILE
    prjInfoSet $hProj changed ""

    return $hProj
}

###############################################################################
#
# prjHandleGet - return a handle to a project file
#
# This routine is like prjOpen, but if the project is already open
# it returns an existing handle instead.
# 

proc prjHandleGet prjFile {
    global prjInfo
    global prjHandleIx
    global prjTagData

    foreach ix [array names prjInfo *,fileName] {
	if {$prjInfo($ix) == $prjFile} {
	    return [lindex [split $ix ,] 0]
	}
    }

    return [prjOpen $prjFile]
}


###############################################################################
#
# prjClose - close a project
#
# RETURNS: N/A
# EXCEPTIONS: invalid handle
#

proc prjClose {hProj {save ""}} {
    global prjTagData
    global prjInfo
    prjHandleValidCheck $hProj

    if {$save == "SAVE"} {
	prjSave $hProj
    }

    # call the project managers close hook, if any
    set prjType [prjTypeGet $hProj]
    if {[namespace eval ${prjType} "info procs close"] != ""} {
	${prjType}::close $hProj
    }

    # free project handle memory
    foreach ix [array names prjTagData $hProj,*] {
	unset prjTagData($ix)
    }
    foreach ix [array names prjInfo $hProj,*] {
	unset prjInfo($ix)
    }
}

#########################################################################
#
# prjCreateable - check if a project is creatable
#
# SYNOPSIS 
#   prjCreateable  prjDir prjType 
#
# PARAMETERS 
#   prjDir: project directory
#   prjType: (optional) project type
#
# RETURNS: 1 if project is creatable, 0 otherwise. Prints error message 
# is project not creatable. 
#
# ERRORS: N / A
#
proc prjCreateable {prjDir {prjType ""}} {

    if {$prjType != "" && ![prjTypeIsValid $prjType]} {
	puts "invalid project type $prjType"
        return 0
    }

    if {![file exists $prjDir]} {
	# we can create as many levels of directory as required.
	# first find the lowest level directory that exists...
	set rootDir [file dir $prjDir]
	while {![file exists $rootDir] && $rootDir != [file dir $rootDir]} {
	    set rootDir [file dir $rootDir]
	}

	# now check that this is a root directory and is writable...
	if {![file writable $rootDir]} {
	    error "Can't create directory $prjDir:\
		\n$rootDir is not writable"
	} elseif {![file isdirectory $rootDir]} {
	    error "'$rootDir' is a file"
    }

    # Directory exists, check if we already have a project or any core files in this directory
    } else {
	set prjList [glob -nocomplain $prjDir/*.wpj]
	if {[llength $prjList] > 0} {
		error "Cannot have more than one Project in a directory.\
	   \nDirectory [file nativename $prjDir] already contains a project"
	}
    }

    return 1
}

###############################################################################
#
# prjOverwritable - check if a project is overwritable
# 

proc prjOverwritable {prjFile prjType} {
    set prjDir [file dir $prjFile]

    if {![prjTypeIsValid $prjType]} {
	error "invalid project type $prjType"
    }

	if {![file writable $prjFile]} {
		error "Project file [file nativename $prjFile] cannot be overwritten"
	}

	if {[info commands ${prjType}::coreFilesGet] != ""} {
		foreach coreFile [${prjType}::coreFilesGet] {
			if {[file exists $prjDir/$coreFile] && \
			    ![file writable $prjDir/$coreFile]} {
				error "Project Core File\n\n[file nativename $prjDir/$coreFile]\
				\n\nexists and cannot be overwritten"
			}
		}
	}

    return 1
}

###############################################################################
#
# prjCoreCreate - create a project core
#
# This is a support routine used by various project-type managers
# to initialize the project code.
#

proc prjCoreCreate {prjFile prjType {version ""} {bValidate 1}} {
    set prjDir [file dir $prjFile]

    if {$version == ""} {
        set version [Wind::version]
    }

    if {$bValidate} {
    	if {![prjCreateable $prjFile $prjType]} {
	    error "can't create project $prjFile or type $prjType"
		}

		if {![file exists $prjDir]} {
			file mkdir $prjDir
		}
    }

    set fd [open $prjFile "w+"]
    fconfigure $fd -translation lf
    close $fd

    set hProj [prjOpen $prjFile]
    prjTagDataSet $hProj CORE_INFO_TYPE $prjType
    prjTagDataSet $hProj CORE_INFO_VERSION $version
    return $hProj
}

###############################################################################
#
# prjDelete - delete a project
#
# Also delete the project directory if it becomes empty
#

proc prjDelete prjFile {
    file delete $prjFile

    catch "file delete [file dir $prjFile]"
}

###############################################################################
#
# prjCopy - copy a project file and its "core" files
#

proc prjCopy {hProj newProj {fCheckCreatable 1} {newDescription ""}} {
    set oldProj [prjInfoGet $hProj fileName]
    set oldProjDir [file dir $oldProj]
    set newProjDir [file dir $newProj]

    set prjType [prjTypeGet $hProj]

    # make sure the new project is createable

    if {$fCheckCreatable == 1 && \
	    ![prjCreateable $newProj $prjType]} {
		error "can't create $newProj"
    }

    # copy "core" files from old project directory to new

	if {![file exists $newProjDir]} {
	    file mkdir $newProjDir
	}

    foreach coreFile [${prjType}::coreFilesGet] {
		file copy -force $oldProjDir/$coreFile $newProjDir/$coreFile
    }

	# Now save the project in memory into a new project file. Do not complain if it exists.
    prjSaveAs $hProj $newProj 0 $newDescription
}

###############################################################################
#
# prjTypeGet - get the project type
#

proc prjTypeGet hProj {
    prjHandleValidCheck $hProj

    return [prjTagDataGet $hProj CORE_INFO_TYPE]
}

###############################################################################
#
# prjProjectFileStatusGet - get the project status: OK or CANNOT_FIND_PROJECT_FILE
#

proc prjProjectFileStatusGet hProj {
    prjHandleValidCheck $hProj

    return [prjInfoGet $hProj PROJECT_FILE_STATUS]
}

###############################################################################
#
# prjHasComponents - can this project have components associated
# with it?
#

proc prjHasComponents {hProj} {
    set retval 0
    switch [::prjTypeGet $hProj] {
	::prj_vxWorks {
	    set retval 1
	}
	default {
	    set retval 0
	}
    }
    return $retval
}

###############################################################################
#
# prjVersionGet - get the project version
#

proc prjVersionGet hProj {
    prjHandleValidCheck $hProj

    return [prjTagDataGet $hProj CORE_INFO_VERSION]
}

###############################################################################
#
# prjStatusLog - log status info
# 
# prjStatusLog {msg {numDone ""} {numTotal ""}}

proc prjStatusLog {args} {
    global prjStatusLogRtn
    if {$prjStatusLogRtn != ""} {
    	eval {$prjStatusLogRtn $args} 
    } else {
	error "Error logging routine not initialized"
    }
}

###############################################################################
#
# prjWarnLog - log a warning
# 

proc prjWarnLog msg {
    global prjWarnLogRtn
    $prjWarnLogRtn $msg
}

###############################################################################
#
# prjStatusLogFuncSet - set the status logging function
# 

proc prjStatusLogFuncSet func {
    global prjStatusLogRtn
    set prjStatusLogRtn $func
}

###############################################################################
#
# defaultPrjStatusLogFuncGet - get the default status logging routine 
# 
# defaultPrjStatusLogFuncGet {}

proc defaultPrjStatusLogFuncGet {} {
	variable defaultPrjStatusLogRtn
	return $defaultPrjStatusLogRtn
}

###############################################################################
#
# prjWarnLogFuncSet - set the warning logging function
# 

proc prjWarnLogFuncSet func {
    global prjWarnLogRtn
    set prjWarnLogRtn $func
}


###############################################################################
#
# defaultPrjWarnLogFuncGet - get the default warning logging routine 
# 
# defaultPrjWarnLogFuncGet {}

proc defaultPrjWarnLogFuncGet {} {
	variable defaultPrjWarnLogRtn
	return $defaultPrjWarnLogRtn
}

###############################################################################
#
# prjDataSplit - split long lines
# 

proc prjDataSplit data {
    if {[string length $data] > 80} {
	regsub -all {[ 	]+} $data " \\\n	" data
    }
    return $data
}

###############################################################################
#
# prjDataJoin - join lines that were split
# 

proc prjDataJoin data {
    regsub -all { \\
	} $data " " data 
    return $data
}

# Utils.tcl is needed by prjLib.tcl, for removeTrailingSlash function

source [wtxPath host resource tcl app-config Project]Utils.tcl

if {![info exists prjLibTcl_sourcePrjLibAndUtilsOnly]} {
    set prjTclDir [wtxPath]/host/resource/tcl/app-config/Project
    source $prjTclDir/prjFileLib.tcl
    source $prjTclDir/prjBuildLib.tcl
    foreach file [glob $prjTclDir/prj_*.tcl] {
        source $file
    }
    unset prjTclDir
}
