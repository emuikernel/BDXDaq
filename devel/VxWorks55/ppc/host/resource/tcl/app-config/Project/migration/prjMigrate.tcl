# prjMigrate.tcl - project migration tool
#
# Copyright 2002 Wind River Systems, Inc.
#
# modification history
# ------------------------
# 01j,09apr02,j_s  add buildRuleMigrate/buildRuleQuery
# 01i,19mar02,j_s  improve help info and documentation; do what wccDependsGet 
#                  does in componentMigrate; log PARAMETERS that don't have
#		   default value
# 01h,12feb02,j_s  BSP_DIR field should be in form of relative path
# 01g,06feb02,j_s  bspMigrate should return the result
# 01f,24jan02,j_s  improve log info; modify makefile comparing code to ignore
#		   blank lines and comment lines; add 'prjFileCRRemove' to 
#		   get rid of 'crlf' in wpj file; on Windows, do an additional
#		   case insensitive comparison if normal(case sensitive)
#                  comparison finds change when examining makefile;
#		   add 'prjFilesMigrate' and 'bspMigrate'; wcc migration
# 01e,21jan02,j_s  environment variable PATH should be switched as well 
#		   when WIND_BASE is switched
# 01d,17jan02,j_s  migrate PROJECT_FILES in the same way as macros are
# 01c,15jan02,j_s  more substitution of '/' for '\'; special treatment when
#		   project is not under WIND_BASE is needed on Solaris
# 01b,10jan02,j_s  Correct errors in SYNOPSIS section; fix the problem that
#		   dataInit is not called for 'info' mode
# 01a,11dec01,j_s  improved migrateLog; replaced '\' with '/' in file path to
#		   make it work on Windows; revised documentation; fixed a bug 
#		   in when to call dataInit
#
# DESCRIPTION
# Project Migration Tool is a data driven tool that helps customer to migrate 
# project of one version of Tornado to project of another version of Tornado.
# It is built on top of Tornado project facility. It can work in two modes:
# AUTO and QUERY. 
#
# When run in AUTO mode the tool copy the project into the destination Tornado
# installation, backup Makefile if it has been manually modified, migrate .wpj 
# file and regenerate Makefile and configuration files. In AUTO mode, the
# algorithm used by prjMigrate to migrate .wpj file is as follows:
#     S - the old version project that is being migrated
#     D - the result new version project 
#
#     1. find out the bsp which S is based on, refer it as SBSP, refer the
#        new version bsp with same name as DBSP;
#     2. create an old version project based on SBSP, refer it as OP;
#     3. find out the differences between S and OP:
#         * what have been added to OP in S - referred as (S - OP)
#         * what have been removed from OP in S - referred as (OP - S)
#     4. create a new version project NP based on DBSP;
#     5. apply to NP the changes made to OP in S to get D, so D can be
#        represented as D = NP + (S - OP) - (OP - S).
#
# We can see that the above algorithm is really to migrate the difference
# between the project being migrated and the project created directly out
# of the base bsp (changed or not)
#
# So the key requirement for the algorithm to work correctly is:
#     * SBSP and DBSP are peers - this means if SBSP is as shipped on old CD,
#       then DBSP must be as shipped on new CD; if SBSP has been changed, then
#       DBSP must have the equivalent changes. Of course, they must be in same
#       name.
#
# The development scenario which the algorithm is targeted on is:
#     * customers create projects based on shipped bsps, and all changes are
#       made to projects, the bsps remain the same as shipped. Now
#       customers want to migrate their projects.
#
# There is another development scenario:
#     * customer started working from bsp, made some changes to bsps, then
#       create projects based on the changed bsps and want to migrate 
#       their projects
#
# For customers under the second scenario, they have to get a peer bsp in
# the new CD installation before using prjMigrate. This means they have to
# migrate the bsp first.
#
# QUERY mode allows customer to find out what equivalent value should
# be used for build macros/components in new environment.
#
# SYNOPSIS
# \ss
#    AUTO mode:
#    prjMigrate   -h[elp]
#               | { -windbase <original tornado> -type <project type> 
#		      [-newproject <prjDirMigrated>] <project to migrate>
#		  }
#    
#    QUERY mode:
#    prjMigrate   -h[elp]
#               | { -windbase <original tornado> -bsp <bsp name> 
#                   -tool <toolchain> 
#		      [
#			{   -component <component list>
#		          | -macro <build macro> [-value <macro value>]
#		        }
#		      ]
#		  }
# \se
#
# PARAMETERS
#
#  -h[elp]		  : print out help info
#  -windbase <original tornado>: the installation directory of the original 
#				 Tornado
#  -type <project type>   : the project type which the current project is being
#                           migrated to, either vxWorks or vxApp
#  -newproject <prjDirMigrated>: the directory for Migrated project
#  <project to migrate>   : an existing project
#  -bsp	<bsp name>	  : bsp name
#  -tool <toolchain>	  : tool chain, e.g. gnu or diab
#  -macro <build macro>	  : the build macro to query
#  -value <macro value>
#			  : optional, current value of the macro to query. 
#			    It consists of customer's changes to the default
#			    value of original Tornado. The query result will 
#			    be the default value of new Tornado applied with 
#			    customer's changes. If omitted, the query result 
#			    will be the default value of new Tornado
#  - component <components>
#			  : a list of components. The query output will be the
#			    list of equivalent components of the new Tornado
#
# EXAMPLES
#
#    Following operation converts a T2.0 project 'Project0' in target/proj 
#    to T2.2 vxWorks project, the T2.0 installation directory is '/tmp/t20':
#
#    prjMigrate -windbase /tmp/t20 -type vxWorks target/proj/Project0
#
#    As result of the above operation, directory target/proj/Project0 will 
#    be copied to the target/proj/Project0Migrated of the T2.2 installation;
#    'Makefile' is backed up if it has been modified manually; '.wpj' will be 
#    migrated to work for T2.2; makefile and configuration files will be 
#    regenerated, source files will remain unchanged.
#
#    Following operation shows that list of build macro which can be queried on:
#
#      prjMigrate -windbase /tmp/t20 -bsp ads860 -tool gnu
#
#    Following operation query on build macro CFLAGS of ads860, gnu combination:
#
#      prjMigrate -windbase /tmp/t20 -bsp ads860 -tool gnu -macro CFLAGS -value "-mcpu=860"
#
#    Following operation query on components INCLUDE_STREAMS and INCLUDE_HTTP:
#
#      prjMigrate -windbase /tmp/t20 -bsp ads860 -tool gnu -component "INCLUDE_STREAMS INCLUDE_HTTP"
#

source [wtxPath]host/resource/tcl/app-config/Project/cmpScriptLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/wccLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/dataDocLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjFileLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjBuildLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/tc_cplus.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prj_vxWorks.tcl
source [wtxPath]host/resource/tcl/app-config/Project/Utils.tcl
source [wtxPath]host/resource/tcl/app-config/Project/migration/cmpDiffLib.tcl

namespace eval prjMigrate {

    # home directory of Project Migration Tool where conversion tables are
    set homedir [wtxPath]host/resource/tcl/app-config/Project/migration
    regsub -all {\\} $homedir "/" homedir

    array set argument {}

    # GOOD or BAD
    set status GOOD

    # auto, query or info
    set mode ""

    # directory of the dynamically created default project of destination 
    # Tornado for purpose of get the new content of build specification
    set prjTemporaryDir ""
    set hPrjTemp 0

    # project handle of the source project
    set hPrjOld 0

    # project handle of the destination project
    set hPrjMigrated 0

    # file descriptor of the log file
    set fdLog 0

    # the list of tag patterns of the content of wpj file
    set tagPatternList {}

    # memory copy of conversion table
    array set tabConv {}

    # the registration list of migration methods
    set querySupported {\
			buildRuleMigrate \
			buildMacroMigrate \
			componentMigrate \
			mxrLibMigrate \
			cdfPathMigrate \
			prjFilesMigrate \
			bspMigrate
		       }

    # bsp migrate table

    set bspNameMigrated ""
    array set bspNameMigratedOf {}
    set bspNameMigratedOf(pcPentiumpro) pcPentium2

    # wcc migrate table

    array set wccNameMigratedOf {}
    set wccNameMigratedOf(pcPentiumpro) pcPentium2

    # the name of the array for the original default build specs, figured out
    # on fly
    set tcDataVar ""

    set libExt ""
    set versionSource ""
}

# import version specific information into namespace prjMigrate

source [wtxPath]host/resource/tcl/app-config/Project/migration/infoLookupTables.tcl

##############################################################################
#
# prjMigrate::usagePrint - print out usage info of prjMigrate
#
# SYNOPSIS
#   usagePrint
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::usagePrint {} {
    puts "DESCRIPTION\n"
    puts "Project Migration Tool is a data driven tool that helps customer "
    puts "to migrate project of one version of Tornado to project of to "
    puts "another version of Tornado. It is built on top of Tornado project"
    puts "facility. It can work in two modes: AUTO and QUERY." 
    puts "\nWhen run in AUTO mode the tool copy the project into the"
    puts "destination Torando installation, backup Makefile if it has been"
    puts "manually modified, migrate .wpj file and regenerate Makefile and"
    puts "configuration files. In AUTO mode, the algorithm used by prjMigrate"
    puts "to migrate .wpj file is as follows:"
    puts "    S - the old version project that is being migrated"
    puts "    D - the result new version project"
    puts ""
    puts "    1. find out the bsp which S is based on, refer it as SBSP, refer"
    puts "       the new version bsp with same name as DBSP;"
    puts "    2. create an old version project based on SBSP, refer it as OP;"
    puts "    3. find out the differences between S and OP:"
    puts "        * what have been added to OP in S - referred as (S - OP)"
    puts "        * what have been removed from OP in S - referred as (OP - S)"
    puts "    4. create a new version project NP based on DBSP;"
    puts "    5. apply to NP the changes made to OP in S to get D, so D can be"
    puts "       represented as D = NP + (S - OP) - (OP - S)."
    puts ""
    puts "We can see that the above algorithm is really to migrate the"
    puts "difference between the project being migrated and the project"
    puts "created directly out of the base bsp (changed or not)"
    puts ""
    puts "So the key requirement for the algorithm to work correctly is:"
    puts "    * SBSP and DBSP are peers - this means if SBSP is as shipped"
    puts "      on old CD, then DBSP must be as shipped on new CD; if SBSP"
    puts "      has been changed, then DBSP must have the equivalent chagnes."
    puts "      Of course, they must be in same name."
    puts ""
    puts "The developement scenario which the algorithm is targeted on is:"
    puts "    * customers create projects based on shipped bsps, and all"
    puts "      changes are made to projects, the bsps remain the same as"
    puts "      shipped. Now customers want to migrate their projects."
    puts ""
    puts "There is another development scenario:"
    puts "    * customer started working from bsp, made some changes to bsps,"
    puts "      then created projects based on the changed bsps and want to"
    puts "      migrate their projects"
    puts ""
    puts "For customers under the second scenario, they have to get a peer bsp"
    puts "in the new CD installation before using prjMigrate. This means they"
    puts "have to migrate the bsp first."
    puts ""
    puts "QUERY mode allows customer to find out what equivalent value should"
    puts "be used for build macros/components in new environment.\n"
    puts "SYNOPSIS\n"
    puts "    AUTO mode:"
    puts "    prjMigrate   -h\[elp\]"
    puts "               | \{ -windbase <original tornado> -type <project type>"
    puts "                   \[ -newproject <prjDirMigrated>\] <project to migrate>"
    puts "                 \}"
    puts ""
    puts "    QUERY mode:"
    puts "    prjMigrate   -h\[elp\]"
    puts "               | \{ -windbase <original tornado> -bsp <bsp name>"
    puts "                   -tool <toolchain>"
    puts "                   \["
    puts "                     \{   -component <component list>"
    puts "                       | -macro <build macro> \[-value <macro value>\]"
    puts "                     \}"
    puts "                   \]"
    puts "                 \}"
    puts "PARAMETERS\n"
    puts "  -h\[elp\]                     : print out help info"
    puts ""
    puts "  -windbase <original tornado>: the installation directory of the"
    puts "                                original Tornado"
    puts ""
    puts "  -type <project type>        : the project type which the current"
    puts "                                project is being migrated to,"
    puts "                                either vxWorks, or vxApp"
    puts ""
    puts "  -newproject <prjDirMigrated>: optional, the directory for the"
    puts "                                migrated project"
    puts ""
    puts "  <project to migrate>        : an existing project"
    puts ""
    puts "  -bsp <bsp name>             : bsp name"
    puts ""
    puts "  -tool <toolchain>           : tool chain, e.g. gnu or diab"
    puts ""
    puts "  -macro <build macro>        : the build macro to query"
    puts ""
    puts "  -value <macro value>        : optional, current value of the macro"
    puts "                                to query. It consists of customer's"
    puts "                                changes to the default value of"
    puts "                                original Tornado. The query result"
    puts "                                will be the default value of new"
    puts "                                Tornado applied with customer's"
    puts "                                changes. If omitted, the query result"
    puts "                                will be the default value of new"
    puts "                                Tornado"
    puts ""
    puts "  -component <components>     : a list of components. The query"
    puts "                                output will be the list of equivalent"
    puts "                                components of the new Tornado"
}

##############################################################################
#
# prjMigrate::continueOrNot - Ask user whether the tool should continue
#
# SYNOPSIS
#   continueOrNot
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::continueOrNot {} {
    puts "Enter 'y' to continue, enter any other key to exit:"
    gets stdin userChoice
    if {$userChoice != "y"} {
	error "User chose to exit"
    }
}

##############################################################################
#
# prjMigrate::argsGet - obtains and process all command line arguments
#
# SYNOPSIS
#   argsGet
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::argsGet {} {
    variable argument

    set argument(help) ""
    set argument(windbase) ""
    set argument(type) ""
    set argument(prjDir) ""
    set argument(prjDirMigrated) ""
    set argument(bsp) ""
    set argument(tool) ""
    set argument(macro) ""
    set argument(value) ""
    set argument(components) ""

    set count [llength $::argv]

    while {$count > 0} {
	set arg [string tolower [lindex $::argv 0]]

	switch -regexp -- $arg {
	    ^-h[e]*[l]*[p]* {
		usagePrint
		return 0
	    }
	    ^-w[i]*[n]*[d]*[b]*[a]*[s]*[e]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(windbase) [lindex $::argv 0]
		regsub -all {\\} $argument(windbase) "/" argument(windbase)
	    }
	    ^-c[o]*[m]*[p]*[o]*[n]*[e]*[n]*[t]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(components) [lindex $::argv 0]
	    }
	    ^-ty[p]*[e]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(type) [lindex $::argv 0]
	    }
	    ^-b[s]*[p]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(bsp) [lindex $::argv 0]
	    }
	    ^-to[o]*[l]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(tool) [lindex $::argv 0]
	    }
	    ^-m[a]*[c]*[r]*[o]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(macro) [lindex $::argv 0]
	    }
	    ^-v[a]*[l]*[u]*[e]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(value) [lindex $::argv 0]
	    }
	    ^-n[e]*[w]*[p]*[r]*[o]*[j]*[e]*[c]*[t]* {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(prjDirMigrated) [lindex $::argv 0]
	    }
	    default {
		set argument(prjDir) [lindex $::argv 0]
	    }
	}
	# shift
	set ::argv [lreplace $::argv 0 0]
	incr count -1
    }
    argsValidate
    return 1
}

##############################################################################
#
# prjMigrate::argsValidate - validate command line arguments
#
# SYNOPSIS
#   argsValidate
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::argsValidate {} {
    variable argument
    variable prjTypeValid
    variable prjTemporaryDir
    variable hPrjOld
    variable cpuList
    variable archOf
    variable tcValid
    variable mode
    variable bspSupport
    variable fdLog
    variable versionSource

    global prjTagData

    set bspSupport [array names cpuList]

    # '-windbase' has to be specified

    if {$argument(windbase) != ""} {

        if {![file exists $::env(WIND_BASE)/target/proj]} {
	    error "Not in a valid Tornado context: WIND_BASE is $::env(WIND_BASE)"
	}
	if {![file exists $argument(windbase)/setup.log]} {
	    error "Invalid argument: no Tornado is installed in \
		  $argument(windbase)"
      	}
	set cmpDiff::WIND_BASE1 $argument(windbase)
	set cmpDiff::WIND_BASE2 $::env(WIND_BASE)
	regsub -all {\\} $cmpDiff::WIND_BASE2 "/" cmpDiff::WIND_BASE2
	set cmpDiff::argument(v1) [versionDetect $cmpDiff::WIND_BASE1]
	set cmpDiff::argument(v2) [versionDetect $cmpDiff::WIND_BASE2]
	set versionSource $cmpDiff::argument(v1)
    } else {
        error "Installation directory of original Tornado is required."
    }

    # if '-type' is specified, then the tool enters AUTO mode

    if {$argument(type) != ""} {
	if {[lsearch $prjTypeValid $argument(type)] == -1} {
	    error "Invalid argument: $argument(type) is not a valid project type"
	}

	if {$argument(prjDir) == ""} {
	    error "Missing argument: no project is specified"
	} elseif {![regexp {^(/|[a-zA-Z]:)} $argument(prjDir)]} {

	    # path is specified in relative form

	    if {[file exists $argument(prjDir)]} {
		set argument(prjDir) [pwd]/$argument(prjDir)
	    } elseif {[file exists \
		       $argument(windbase)/target/proj/$argument(prjDir)]} {
		set argument(prjDir) \
		    $argument(windbase)/target/proj/$argument(prjDir)
	    } else {
		error "Invalid argument: neither $argument(windbase)/target/proj
		       nor [pwd] contain $argument(prjDir)"
	    }
	} elseif {![file exists $argument(prjDir)]} {
	    error "Invalid argument: directory $argument(prjDir) doesn't exist"
	}

	regsub -all {\\} $argument(prjDir) "/" argument(prjDir)
	set prjFiles [glob -nocomplain $argument(prjDir)/*.wpj]
	set prjFileN [llength $prjFiles]
	if {$prjFileN > 1} {
	    error "Invalid argument: there are $prjFileN .wpj files in \
		   $argument(prjDir) while only 1 is expected"
	}
	if {$prjFileN == 0} {
	    error "Invalid argument: no project file(.wpj) in \
	           $argument(prjDir)"
	}
	set argument(prjFile) [lindex $prjFiles 0]
	if {$argument(prjDirMigrated) == ""} {
	    set argument(prjDirMigrated) \
		$::env(WIND_BASE)/target/proj/[file tail $argument(prjDir)]Migrated
	    regsub -all {\\} $argument(prjDirMigrated) "/" argument(prjDirMigrarted)
	} elseif {![regexp {^(/|[a-zA-Z]:)} $argument(prjDirMigrated)]} {
	    set argument(prjDirMigrated) [pwd]/$argument(prjDirMigrated)
	    regsub -all {\\} $argument(prjDirMigrated) "/" argument(prjDirMigrarted)
	}
	if [file exists $argument(prjDirMigrated)] {
	    error "\nDirectory exists: $argument(prjDirMigrated). \nCan't \
		   migrate $argument(prjDir) to $argument(prjDirMigrated).\
		   \nPlease specify a non-existent destination."
        }
	set hPrjOld [prjOpen $argument(prjFile)]

        if {$argument(type) == "vxWorks"} {
	    set argument(bsp) [prjTagDataGet $hPrjOld WCC__CURRENT]
            if {[lsearch $bspSupport $argument(bsp)] == -1} {
	        migrateLog $fdLog "Bsp $argument(bsp) is not supported. \
		    Build specifications are carried over intact. Customer's \
		    inspection advised."
		exit
	    }
        }

	set tc [prjBuildTcGet $hPrjOld [prjBuildCurrentGet $hPrjOld]]
	set argument(tool) [${tc}::tool]

	set mode auto
    } else {

	# '-type' is not specified, tool enters QUERY mode
	# query is done on project of type vxWorks

	set argument(type) vxWorks
	if {$argument(bsp) == ""} {
	    error "Missing argument: neither a project type nor a bsp is specified"
	} elseif {[lsearch $bspSupport $argument(bsp)] == -1} {
	    error "Unsupported bsp: $argument(bsp)"
	}

	if {$argument(tool) == ""} {
	    error "Missing argument: no toolchain specified"
	} elseif {[lsearch $tcValid $argument(tool)] == -1} {
	    error "Invalid argument: $argument(tool) is not a valid tool chain"
	}

	set mode query
	if {$argument(macro) == ""} {
	    if {$argument(components) == ""} {
		set mode info
	    }
	}
    }

    # determine the directory of the dynamically created default project
    # there are two categories: bsp-derived project, non-bsp-derived project

    if {$argument(bsp) != ""} {
        set prjTemporaryDir $::env(WIND_BASE)/target/proj/$argument(bsp)_$argument(type)
	regsub -all {\\} $prjTemporaryDir "/" prjTemporaryDir
        set cmpDiff::argument(cpu) $cpuList($argument(bsp))
        set cmpDiff::argument(tool) $argument(tool)
        set cmpDiff::argument(arch) $archOf($cmpDiff::argument(cpu))
    } else {
	set tc [prjBuildTcGet $hPrjOld [prjBuildCurrentGet $hPrjOld]]
	regsub {::} $tc "" tcContent
	set prjTemporaryDir $::env(WIND_BASE)/target/proj/${tcContent}_$argument(type)
	regsub -all {\\} $prjTemporaryDir "/" prjTemporaryDir
    }
}

##############################################################################
#
# prjMigrate::versionDetect - derive version info from setup.log
#
# SYNOPSIS
#   versionDetect windbase
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::versionDetect {windbase} {
    variable versionCD

    if {![file exists $windbase/setup.log]} {
	error "No setup.log in $windbase."
    }

    # get the TDK number from line 2 of setup.log

    set fd [open $windbase/setup.log "r"]
    while {[gets $fd line] >= 0} {
        if [regexp {TDK-[0-9]*-ZC-[0-9]*} $line] {
	    regexp {TDK-[0-9]*-ZC-[0-9]*} $line tdkNumber
	    break
	}
    }
    if ![info exists tdkNumber] {
	error "TDK number is missing from ${windbase}/setup.log ."
    }
    if {[info exists versionCD($tdkNumber)]} {
	return $versionCD($tdkNumber)
    } else {
	error "Unknown CD: $tdkNumber."
    }
}

##############################################################################
#
# prjMigrate::tcGet - get tool chain information of current session
#
# SYNOPSIS
#   tcGet
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::tcGet {} {
    variable mode
    variable argument
    variable cpuList
    variable hPrjMigrated

    if {$mode != "auto"} {
	set tc ::tc_$cpuList($argument(bsp))$argument(tool)
	return $tc
    } else {
	file copy $argument(prjDir) $argument(prjDirMigrated)
	set prjFileMigrated \
	    $argument(prjDirMigrated)/[file tail $argument(prjFile)]
	set hPrjMigrated [prjOpen $prjFileMigrated]
	return [prjBuildTcGet $hPrjMigrated [prjBuildCurrentGet $hPrjMigrated]]
    }
}

##############################################################################
#
# prjMigrate::migrateLog - log actions/error/warning taken during migration
#
# SYNOPSIS
#   migrateLog  fd description
#
# PARAMETERS: 
#   fd : descriptor of log file
#   description: description of the actions/error/warning
#
# RETURNS: N / A 
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::migrateLog {fd description} {
    if {$fd != 0} {
        puts $fd "\n------\n$description"
    } else {
	puts "\n------\n$description"
    }
}

##############################################################################
#
# prjMigrate::buildRuleQuery - Query on build rules
#
# SYNOPSIS
#   buildRuleQuery rule value
#
# PARAMETERS: 
#   macro: a build rule to query
#   value: the current value of the rule
#
# RETURNS: If a value provided, then add the differences between the value and 
#	   the original default value to the new default value, the resulted 
#          value is returned.
#	   If no value provided, then the new default value will be returned.
#	   "NOP" is returned if error happened in 'auto' mode
#
# ERRORS:  * Unknow build rule 
#
# NOMANUAL
#

proc prjMigrate::buildRuleQuery {rule {value ""}} {
    variable argument
    variable mode
    variable hPrjTemp
    variable hPrjMigrated
    variable status
    variable tcDataVar
    variable fdLog

    # tcDataVar holds the name of the array for build spec content

    global $tcDataVar
    set ruleList [array name $tcDataVar]
    if {[lsearch $ruleList $rule]  == -1} {
	if {$mode == "query"} {
	    error "Unknown build rule: $rule"
	} else {
	    set status BAD
	    migrateLog $fdLog "Unknown build rule: $rule. Just copy it over."
	    return "NOP"
	}
    }
    if {$value == ""} {
	return [prjRelPath $hPrjTemp \
	       [prjBuildRuleGet $hPrjTemp [prjBuildCurrentGet $hPrjTemp] $rule]]
    } else {

	# Since valOldDefault is in relative form, we can expand it for
	# whatever project we like. Here, it has to be expanded for
	# hPrjMigrated in order to be comparable to customer's value
	# This is needed for Solaris only.

        set valOldDefault [set [set tcDataVar]($rule)]
	set valOldDefault [prjFullPath $hPrjMigrated $valOldDefault]
	set valOldDefault [prjRelPath $hPrjMigrated $valOldDefault]
	set valNewDefault [prjRelPath $hPrjTemp \
			   [prjBuildRuleGet $hPrjTemp [prjBuildCurrentGet $hPrjTemp] $rule]]

	return [mergeCustomDfltOldDfltNew $value $valOldDefault $valNewDefault]
    }
}

##############################################################################
#
# prjMigrate::buildRuleMigrate - calling stub of buildRuleQuery
#
# SYNOPSIS
#   buildRuleMigrate tag
#
# PARAMETERS: 
#
# RETURNS: return what returned by buildRuleQuery
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::buildRuleMigrate {tag} {
    variable hPrjMigrated
    variable hPrjOld
    global prjTagData

    regsub {BUILD_RULE_} $tag "" rule
    set val [buildRuleQuery $rule \
	      [prjRelPath $hPrjMigrated $prjTagData($hPrjMigrated,$tag)]]
    set val [orderMatch $val $prjTagData($hPrjOld,$tag)]

    # format build rules as follows:
    #     {target :} {build commands}
    if {[llength $val] > 2} {
        return [list [lrange $val 0 1] [join [lrange $val 2 end]]]
    } else {
	return $val
    }
}

##############################################################################
#
# prjMigrate::buildMacroQuery - Query on build macros
#
# SYNOPSIS
#   buildMacroQuery macro value
#
# PARAMETERS: 
#   macro: a macro to query
#   value: the current value of the macro
#
# RETURNS: If a value provided, then add the differences between the value and 
#	   the original default value to the new default value, the resulted 
#          value is returned.
#	   If no value provided, then the new default value will be returned.
#	   "NOP" is returned if error happened in 'auto' mode
#
# ERRORS:  * Unknow build macro 
#	   * No knowledge of macros for bsp, toolchain combination
#
# NOMANUAL
#

proc prjMigrate::buildMacroQuery {macro {value ""}} {
    variable argument
    variable mode
    variable hPrjTemp
    variable hPrjMigrated
    variable status
    variable tcDataVar
    variable fdLog

    # tcDataVar holds the name of the array for build spec content

    global $tcDataVar
    set macroList [array name $tcDataVar]
    if {[lsearch $macroList $macro]  == -1} {
	if {$mode == "query"} {
	    error "Unknown build macro: $macro"
	} else {
	    set status BAD
	    migrateLog $fdLog "Unknown build macro: $macro. Just copy it over."
	    return "NOP"
	}
    }
    if {$value == ""} {
	return [prjRelPath $hPrjTemp \
	       [prjBuildMacroGet $hPrjTemp [prjBuildCurrentGet $hPrjTemp] $macro]]
    } else {

	# Since valOldDefault is in relative form, we can expand it for
	# whatever project we like. Here, it has to be expanded for
	# hPrjMigrated in order to be comparable to customer's value
	# This is needed for Solaris only.

        set valOldDefault [set [set tcDataVar]($macro)]
	set valOldDefault [prjFullPath $hPrjMigrated $valOldDefault]
	set valOldDefault [prjRelPath $hPrjMigrated $valOldDefault]
	set valNewDefault [prjRelPath $hPrjTemp \
			   [prjBuildMacroGet $hPrjTemp [prjBuildCurrentGet $hPrjTemp] $macro]]

	return [mergeCustomDfltOldDfltNew $value $valOldDefault $valNewDefault]
    }
}

##############################################################################
#
# prjMigrate::prjFilesQuery - Query on PROJECT_FILES
#
# SYNOPSIS
#   prjFilesQuery macro value
#
# PARAMETERS: 
#   macro: a macro to query
#   value: the current value of the macro
#
# RETURNS: If a value provided, then add the differences between the value and 
#	   the original default value to the new default value, the resulted 
#          value is returned.
#	   If no value provided, then the new default value will be returned.
#	   "NOP" is returned if error happened in 'auto' mode
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::prjFilesQuery {macro {value ""}} {
    variable argument
    variable mode
    variable hPrjTemp
    variable hPrjMigrated
    variable status
    variable tcDataVar
    variable fdLog
    global prjTagData

    # tcDataVar holds the name of the array for default value

    global $tcDataVar
    set macroList [array name $tcDataVar]
    if {[lsearch $macroList $macro]  == -1} {
	if {$mode == "query"} {
	    error "Unknown macro: $macro"
	} else {
	    set status BAD
	    migrateLog $fdLog "Unknown build macro: $macro. Just copy it over."
	    return "NOP"
	}
    }
    if {$value == ""} {
	if [info exists prjTagData($hPrjTemp,$macro)] {
	    return [prjRelPath $hPrjTemp $prjTagData($hPrjTemp,$macro)]
	} else {
	    return ""
        }
    } else {

	# Since valOldDefault is in relative form, we can expand it for
	# whatever project we like. Here, it has to be expanded for
	# hPrjMigrated in order to be comparable to customer's value
	# This is needed for Solaris only.

        set valOldDefault [set [set tcDataVar]($macro)]
	set valOldDefault [prjFullPath $hPrjMigrated $valOldDefault]
	set valOldDefault [prjRelPath $hPrjMigrated $valOldDefault]

	# valNewDefault has to be expanded and contract for hPrjMigrated
	# in order to be meaningful in the migrated project

	set valNewDefault [prjRelPath $hPrjTemp \
			   $prjTagData($hPrjTemp,$macro)]
        set valNewDefault [prjFullPath $hPrjMigrated $valNewDefault]
        set valNewDefault [prjRelPath $hPrjMigrated $valNewDefault]

	return [mergeCustomDfltOldDfltNew $value $valOldDefault $valNewDefault]
    }
}

##############################################################################
#
# prjMigrate::buildMacroMigrate - calling stub of buildMacroQuery
#
# SYNOPSIS
#   buildMacroMigrate tag
#
# PARAMETERS: 
#
# RETURNS: return what returned by buildMacroQuery
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::buildMacroMigrate {tag} {
    variable hPrjMigrated
    variable hPrjOld
    global prjTagData

    regsub {BUILD_.*_MACRO_} $tag "" macro
    set val [buildMacroQuery $macro \
	      [prjRelPath $hPrjMigrated $prjTagData($hPrjMigrated,$tag)]]
    return [orderMatch $val $prjTagData($hPrjOld,$tag)]
}

##############################################################################
#
# prjMigrate::prjFilesMigrate - calling stub of prjFilesQuery
#
# SYNOPSIS
#   prjFilesMigrate tag
#
# PARAMETERS: 
#   tag - name of the field to migrate
#
# RETURNS: return what returned by prjFilesQuery
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::prjFilesMigrate {tag} {
    variable hPrjMigrated
    variable hPrjOld
    global prjTagData

    set val [prjFullPath $hPrjMigrated \
			[prjFilesQuery $tag [prjRelPath $hPrjMigrated \
			  $prjTagData($hPrjMigrated,$tag)]]]
    return [orderMatch $val $prjTagData($hPrjOld,$tag)]
}

##############################################################################
#
# prjMigrate::parameterMigrate - Query on parameters
#
# SYNOPSIS
#   parameterMigrate wcc params
#
# PARAMETERS: 
#   wcc: wcc value
#   params: optional, a list of parameters, default is empty 
#
# RETURNS: N / A
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::parameterMigrate {wcc {params ""}} {
    variable hPrjMigrated
    variable versionSource
    global prjTagData

    # paramMigrate contains the knowledge of how a configuration parameter
    # should be migrated. It is an array of new configuration parameters
    # indexed by their corresponding old parameters. An blank element means
    # removal of the indexing parameter

    variable paramMigrate

    # paramMigrationKnown is two dimension array indexed by $param,NAME or
    # param,VALUE. paramMigrationKnown($param,NAME) contains NAME migration
    # info while paramMigrationKnown($param,VALUE) contains VALUE migration
    # info

    set paramMigrationKnown [array names paramMigrate ${versionSource},*]
    regsub -all ${versionSource}, $paramMigrationKnown "" paramMigrationKnown
    regsub -all {,NAME} $paramMigrationKnown "" paramMigrationKnown
    regsub -all {,VALUE} $paramMigrationKnown "" paramMigrationKnown
    $cmpDiff::cxrdoc1Set = $paramMigrationKnown
    set paramMigrationKnown [$cmpDiff::cxrdoc1Set contents]
    foreach param $params {
	if {[lsearch $paramMigrationKnown $param] != -1} {

	    # Since paramMigrate is indexed with old parameters, 
	    # so VALUE migration must be done before NAME migration.
	    # Otherwise VALUE migration info might get lost.

	    if [info exists paramMigrate($versionSource,$param,VALUE)] {
		set prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param) \
		    $paramMigrate($versionSource,$param,VALUE)
	    }
	    if [info exists paramMigrate($versionSource,$param,NAME)] {
	        if {$paramMigrate($versionSource,$param,NAME) == ""} {
	            unset \
			prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param)
	        } else {
		    set val $prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param)
	            unset \
			prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param)
		    set prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$paramMigrate($versionSource,$param,NAME)) $val
		}
	    }
	}
    }
    return "NOP"
}

##############################################################################
#
# prjMigrate::componentQuery - Query on components
#
# SYNOPSIS
#   componentQuery comps wcc
#
# PARAMETERS: 
#   comps: a list of components 
#   wcc: optional, wcc value, default is blank
#
# RETURNS: a list of components of the new Tornado that are equivalent to the
#	   list of components of the original Tornado
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::componentQuery {comps {wcc ""}} {
    variable hPrjMigrated
    variable fdLog
    variable mode
    variable versionSource
    global prjTagData

    # compMigrate contains the knowledge of how a component should be
    # migrated. It is an array of new components indexed by their
    # corresponding old components

    variable compMigrate

    set compMigrationUnknown {}
    set compMigrated {}

    foreach cmp $comps {
	if [info exists compMigrate($versionSource,$cmp,MIGRATION)] {

            # if the knowledge of migration of a component is available,
	    # then apply it

	    set compMigrated [concat $compMigrated $compMigrate($versionSource,$cmp,MIGRATION)]

	} elseif [info exists compMigrate($versionSource,$cmp,LOG)] {

	    # log the message

 	    if {$mode == "auto"} {
	        migrateLog $fdLog "${cmp}: $compMigrate($versionSource,$cmp,LOG)"
	    } elseif {$mode == "query"} {
		puts "${cmp}:"
		puts "$compMigrate($versionSource,$cmp,LOG)"
	    }
	    lappend compMigrated $cmp

	} else {

	    # collect those components for which migration knowledge is
	    # not available

	    lappend compMigrationUnknown $cmp
	}
    }

    set compMigrated [concat $compMigrated $compMigrationUnknown]
    if {$mode == "auto"} {
        cmpDiff::cmpDiff $compMigrated
    }

    if {$wcc != ""} {
        # migrate configuration parameters
        # 1 - migrate existing parameters;
        # 2 - add new parameters if any;

        set paramList [array names prjTagData \
			     $hPrjMigrated,WCC_${wcc}_PARAM_*]
        regsub -all ${hPrjMigrated}, $paramList "" paramList
        regsub -all {WCC_[^ ]*_PARAM_} $paramList "" paramList
        parameterMigrate $wcc $paramList

	# cmpDiff::cxrdoc2Set has to be used because it's for the destination
	# installation

        $cmpDiff::cxrdoc2Set = $compMigrated
        $cmpDiff::cxrdoc2Set = [$cmpDiff::cxrdoc2Set get CFG_PARAMS]
	set paramListNew [$cmpDiff::cxrdoc2Set - $paramList]
        foreach param $paramListNew {
	    if ![info exists \
		      prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param)] {
		$cmpDiff::cxrdoc2Set = $param
		set prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param) \
		    [$cmpDiff::cxrdoc2Set get DEFAULT]
		if {$prjTagData($hPrjMigrated,WCC_${wcc}_PARAM_$param) == ""} {
		    puts "Warning: $param of \
			  [$cmpDiff::cxrdoc2Set get _CFG_PARAMS] has no \
			  default value. User has to provide a value for it."
		    migrateLog $fdLog "Warning: $param of \
				       [$cmpDiff::cxrdoc2Set get _CFG_PARAMS] \
				       has no default value. User has to \
				       provide a value for it."
		}
	    }
   	}
    }
    return $compMigrated
}

##############################################################################
#
# prjMigrate::componentMigrate - calling stub of componentQuery
#
# SYNOPSIS
#   componentMigrate tag
#
# PARAMETERS: 
#   tag: a projet data tag
#
# RETURNS: return what returned by componentQuery
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::componentMigrate {tag} {
    variable hPrjMigrated
    variable hPrjTemp
    variable tcDataVar
    variable fdLog
    variable wccNameMigratedOf
    global prjTagData
    global $tcDataVar

    regexp {WCC_(.*)_COMPONENTS} $tag dummy wcc
    if ![info exists [set tcDataVar]($tag)] {
	migrateLog $fdLog "Default component list is not available. \
			   Migrate component one by one."
        return [componentQuery $prjTagData($hPrjMigrated,$tag) $wcc]
    }
    set wccNameMigrated $wcc
    if [info exists wccNameMigratedOf($wcc)] {
	set wccNameMigrated $wccNameMigratedOf($wcc)
    }
    set valOldDefault [set [set tcDataVar]($tag)]
    set valNewDefault [wccComponentListGet $hPrjTemp $wccNameMigrated]
    set value [wccComponentListGet $hPrjMigrated $wcc]
    set valueMerged \
	[mergeCustomDfltOldDfltNew $value $valOldDefault $valNewDefault]
    set components [componentQuery $valueMerged $wcc]

    # emulate what wccDependsGet does; can't use wccDependsGet directly
    # because 'components' is currently not in any project yet

    set cxrDoc [cxrDocCreate $hPrjTemp]
    set mxrDoc [mxrDocCreate $hPrjTemp]
    set cmpSet [$cxrDoc setCreate]
    $cmpSet = $components

    $cmpSet = [cxrSubtree $cmpSet $mxrDoc]
    set components [$cmpSet instances Component]

    $cmpSet delete

    return $components
}

##############################################################################
#
# prjMigrate::compMxrLibGet - get ARCHIVE of the component configuration of
#			      the migrated project
#
# SYNOPSIS
#   compMxrLibGet hProj
#
# PARAMETERS: 
#   hProj - project handle
#
# RETURNS: value of ARCHIVE of the configuration
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::compMxrLibGet {hProj} {
    set wcc [wccCurrentGet $hProj]

    set cxrDoc [cxrDocCreate $hProj]
    set cmpSet [$cxrDoc setCreate]

    $cmpSet = [wccComponentListGet $hProj $wcc]
    return [$cmpSet get ARCHIVE]
}

##############################################################################
#
# prjMigrate::bspMigrate - migrate BSP_DIR
#
# SYNOPSIS
#   bspMigrate
#
# PARAMETERS:
#
# RETURNS: N / A
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::bspMigrate {{dummy ""}} {
    variable hPrjMigrated
    variable argument
    variable bspNameMigratedOf
    global prjTagData

    if [info exists bspNameMigratedOf($argument(bsp))] {
        set prjTagData($hPrjMigrated,BSP_DIR) \
	    [file dirname $prjTagData($hPrjMigrated,BSP_DIR)]/$bspNameMigratedOf($argument(bsp))
    }
    return "NOP"
}

##############################################################################
#
# prjMigrate::mxrLibMigrate - migrate MXR_LIBS
#
# SYNOPSIS
#   mxrLibMigrate
#
# PARAMETERS:
#
# RETURNS: N / A
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::mxrLibMigrate {{dummy ""}} {
    variable hPrjMigrated
    global prjTagData

    set prjTagData($hPrjMigrated,WCC__MXR_LIBS) [compMxrLibGet $hPrjMigrated]
    return "NOP"
}

##############################################################################
#
# prjMigrate::cdfPathMigrate - migrate CDF_PATH
#
# SYNOPSIS
#   cdfPathMigrate
#
# PARAMETERS:
#
# RETURNS: N / A
#
# ERRORS:  N / A 
#
# NOMANUAL
#

proc prjMigrate::cdfPathMigrate {{dummy ""}} {
    variable hPrjMigrated
    variable hPrjTemp
    global prjTagData

    lappend prjTagData($hPrjMigrated,WCC__CDF_PATH) \
	   [wtxPath]target/config/comps/vxWorks/tool/\$(TOOL_FAMILY) 
    set val [orderMatch $prjTagData($hPrjMigrated,WCC__CDF_PATH) $prjTagData($hPrjTemp,WCC__CDF_PATH)]
    set prjTagData($hPrjMigrated,WCC__CDF_PATH) $val
    return "NOP"
}

##############################################################################
#
# prjMigrate::dataInitCompAndParam - read in data of component and parameter
#				     migration
#
# SYNOPSIS
#   dataInitCompAndParam
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::dataInitCompAndParam {} {
    variable homedir

    # read in component and parameters migration database

    source $homedir/compMigrate.dat
    source $homedir/paramMigrate.dat
}

##############################################################################
#
# prjMigrate::dataInit - initialize common data structures
#
# SYNOPSIS
#   dataInit
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::dataInit {} {
    variable argument
    variable prjTemporaryDir
    variable hPrjTemp
    variable mode
    variable tcDataVar
    variable hPrjMigrated
    variable libExt
    variable homedir
    variable bspNameMigrated
    variable bspNameMigratedOf

    # tcDataVar holds the name of the array of build spec content

    set tc [tcGet]
    regsub {::} $tc "" tcContent
    if {$argument(bsp) != ""} {
        set tcDataVar $tcContent$argument(bsp)
	set buildSpecSourceType bsp
	set buildSpecSource $argument(bsp)
	if [info exists bspNameMigratedOf($argument(bsp))] {
	    set bspNameMigrated $bspNameMigratedOf($argument(bsp))
	} else {
	    set bspNameMigrated $argument(bsp)
	}
    } else {
	set tcDataVar ${tcContent}vxApp
	set buildSpecSourceType vxApp
	set buildSpecSource $tc
    }
    global $tcDataVar

    set tcDataFile $argument(windbase)/buildSpecContent
    set windbaseCurrent $::env(WIND_BASE)
    regsub -all {\\} $windbaseCurrent "\\\\\\\\" windbasePattern
    regsub -all {\\} $windbaseCurrent "/" windbaseCurrent
    set pathCurrent $::env(PATH)
    regsub -all $windbasePattern $pathCurrent $argument(windbase) pathOld

    # switch to context of the original Tornado installation
    # buildSpecCollect.tcl loads tcl files of project facility

    set ::env(WIND_BASE) $argument(windbase)
    set ::env(PATH) $pathOld

    puts "Getting build specification from $::env(WIND_BASE), please wait ..."
    exec $::env(WIND_BASE)/host/$::env(WIND_HOST_TYPE)/bin/wtxtcl ${homedir}/buildSpecCollect.tcl $buildSpecSourceType $buildSpecSource >@stdout
    puts "Done."

    # switch back to context of the new Tornado installation

    set ::env(PATH) $pathCurrent
    set ::env(WIND_BASE) $windbaseCurrent

    source $tcDataFile

    if {$mode == "info"} {
	set macroList [array names $tcDataVar]
	regsub -all {WCC_[^ ]*} $macroList "" macroList
	regsub -all {PROJECT_FILES} $macroList "" macroList
	puts "\nQuery is supported on following build macros:\n"
	puts "[join $macroList]"
	exit
    } else {

	# create a project for purpose of getting default build spec content

        set bspDir $::env(WIND_BASE)/target/config/$bspNameMigrated
	regsub -all {\\} $bspDir "/" bspDir
	if {![file exists $prjTemporaryDir/prjTemp.wpj]} {
	    if {$argument(type) == "vxWorks"} {
                ::prj_vxWorks_hidden::vxProjCreate \
		    $prjTemporaryDir/prjTemp.wpj $bspDir
	    } elseif {$argument(type) == "vxApp"} {
		::prj_vxApp_hidden::projCreate $prjTemporaryDir/prjTemp.wpj $tc
	    }
	}
        set hPrjTemp [prjOpen $prjTemporaryDir/prjTemp.wpj]
    }
}

##############################################################################
#
# prjMigrate::dataInitMigration - initialize migration data structures
#
# SYNOPSIS
#   dataInitMigration
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::dataInitMigration {} {
    variable argument
    variable status
    variable hPrjOld
    variable hPrjTemp
    variable tagPatternList
    variable tabConv
    variable querySupported
    variable homedir
    variable fdLog

    # the name of conversion table is composed of source version name(e.g.,t20), 
    # source project type, destination version name, destination project type 
    # and the suffix 'convtab'

    set versionFrom [versionDetect $argument(windbase)]
    regsub -all {\.} $versionFrom "" versionFrom
    set typeFrom [prjTypeGet $hPrjOld]
    regsub {::prj_} $typeFrom "" typeFrom
    set versionTo [versionDetect $::env(WIND_BASE)]
    regsub -all {\.} $versionTo "" versionTo
    set typeTo [prjTypeGet $hPrjTemp]
    regsub {::prj_} $typeTo "" typeTo
    set dataFile ${homedir}/t${versionFrom}[string tolower $typeFrom]t${versionTo}[string tolower $typeTo].convtab

    if {![file exists $dataFile]} {
	error "Missing data file: $dataFile"
    }

    set fd [open $dataFile]
    while {[gets $fd line] != -1} {
	if [regexp {^#} $line] {
	    continue
	}
	set entry [split $line ,]
	set tagPattern [lindex $entry 0]
	lappend tagPatternList $tagPattern
	set tabConv($tagPattern,ACTION) [lindex $entry 1]
	set tabConv($tagPattern,DATA) [lindex $entry 2]
	set tabConv($tagPattern,METHOD) [lindex $entry 3]
	if {$tabConv($tagPattern,ACTION) == "MIGRATE"} {
	    if {[lsearch $querySupported $tabConv($tagPattern,METHOD)] == -1} {
		set status BAD
		migrateLog $fdLog "Method $tabConv($tagPattern,METHOD) is not valid. \
			Action MIGRATE can't be carried out on $tagPattern. \
			Action COPY will be carried out instead."
		puts "Method $tabConv($tagPattern,METHOD) is not valid. \
			Action MIGRATE can't be carried out on $tagPattern. \
			Action COPY will be carried out instead."
		continueOrNot
		migrateLog $fdLog "User chose to continue with COPY on $tagPattern"
		set tabConv($tagPattern,ACTION) COPYINSTEAD
	    }
	}
    }
    close $fd

    dataInitCompAndParam
}

##############################################################################
#
# prjMigrate::orderMatch - to make sure the common content between list A and
#                          list B are in the order as they are in list B
#
# SYNOPSIS
#   orderMatch listToOrder listOrdered
#
# PARAMETERS:
#   listToOrder - list which content needs reordering
#   listOrdered - list which content order is desired
#
# RETURNS: re-ordered listToOrder
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::orderMatch {listToOrder listOrdered} {
    set listResult {}
    foreach element $listToOrder {
	if {[lsearch $listResult $element] == -1} {
	    set idx [lsearch $listOrdered $element]
	    if {$idx != -1} {
		set elemsBefore [lrange $listOrdered 0 [expr $idx - 1]]
		foreach item $elemsBefore {
		    if {([lsearch $listToOrder $item] != -1) &&
			([lsearch $listResult $item] == -1)} {
			lappend listResult $item
		    }
		}
		set listOrdered [lrange $listOrdered [expr $idx + 1] end]
	    }
	    lappend listResult $element
	}
    }
    return [join $listResult]
}

##############################################################################
#
# prjMigrate::mergeCustomDfltOldDfltNew - the initiative of creating this
#					  routine is to migrate build macro as
#					  follows: determine customer's change
#					  to original default value 
#					  (addition/removal); apply the changes
#					  to new default value
# SYNOPSIS
#   mergeCustomDfltOldDfltNew valueCustom dfltValueOld dfltValueNew
#
# PARAMETERS:
#   valueCustom - customer's value
#   dfltValueOld - original default value
#   dfltValueNew - new default value
#
# RETURNS: a list with the merged result
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::mergeCustomDfltOldDfltNew {valueCustom dfltValueOld dfltValueNew} {
    # determine customer's change to the original default value of the macro

    set valueCustom [join $valueCustom]
    set dfltValueOld [join $dfltValueOld]
    set dfltValueNew [join $dfltValueNew]

    set optionsRemoved {}
    foreach option $dfltValueOld {
	if {[lsearch $valueCustom $option] == -1} {
	    lappend optionsRemoved $option
	}
    }
    set optionsAdded {}
    foreach option $valueCustom {
	if {[lsearch $dfltValueOld $option] == -1} {
 	    lappend optionsAdded $option
	}
    }

    # apply customer's change to the new default value of the macro

    foreach option $optionsRemoved {
        set idx [lsearch $dfltValueNew $option]
	if {$idx != -1} {
	    set dfltValueNew [lreplace $dfltValueNew $idx $idx]
	}
    }
    foreach option $optionsAdded {
        if {[lsearch $dfltValueNew $option] == -1} {
	    lappend dfltValueNew $option
        }
    }
    return $dfltValueNew
}

##############################################################################
#
# prjMigrate::prjFileCRRemove - convert 'crlf' to 'lf' in wpj file
#
# SYNOPSIS
#   prjFileCRRemove prjFile
#
# PARAMETERS:
#   prjFile - project file name
#
# RETURNS:    N / A
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::prjFileCRRemove {prjFile} {
    set fd [open $prjFile "r"]
    fconfigure $fd -translation lf
    set lines [read $fd]
    if [regexp \r $lines] {
	close $fd
        set fd [open $prjFile "r+"]
        fconfigure $fd -translation lf 
        set lines [read $fd]
        regsub -all \r $lines "" lines
        seek $fd 0
        puts -nonewline $fd $lines
    }
    close $fd
}

##############################################################################
#
# prjMigrate::migrate - migrate project
#
# SYNOPSIS
#   migrate
#
# PARAMETERS: N / A
#
# RETURNS: 
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc prjMigrate::migrate {} {
    variable status
    variable hPrjMigrated
    variable hPrjOld
    variable hPrjTemp
    variable tagPatternList
    variable tabConv
    variable fdLog
    variable argument
    variable bspNameMigrated
    variable wccNameMigratedOf
    global prjTagData

    # migrate existing fields

    foreach tagPattern $tagPatternList {
        set tags [array names prjTagData $hPrjMigrated,$tagPattern]
	regsub -all ${hPrjMigrated}, $tags "" tags

	switch $tabConv($tagPattern,ACTION) {
	    COPY { 

		# COPY must be explicitly carried out. For tags matching more 
		# than one tag patterns, the last match takes effect. The tag 
		# is expanded for 'hPrjMigrated', it has to be contracted 
		# for 'hPrjMigrated' then re-expanded for 'hPrjOld' to get the 
		# tag good for 'hPrjOld'

		foreach tag $tags {
		    set tagRelPath [prjRelPath $hPrjMigrated $tag]
		    set tagFullPathOld [prjFullPath $hPrjOld $tagRelPath]

		    # this check is necessary because some new tag might
		    # not exist in the old project but added to the migrated
		    # project

		    if [info exists prjTagData($hPrjOld,$tagFullPathOld)] {
		        set prjTagData($hPrjMigrated,$tag) \
			    [prjFullPath $hPrjMigrated \
			    [prjRelPath $hPrjOld $prjTagData($hPrjOld,$tagFullPathOld)]]
		    }
		}
	    }
	    COPYINSTEAD {

		# COPY must be explicitly carried out. For tags matching more 
		# than one tag patterns, the last match takes effect. The tag 
		# is expanded for 'hPrjMigrated', it has to be contracted for
		# 'hPrjMigrated' then re-expaned for 'hPrjOld' to get the tag 
		# good for 'hPrjOld'

		foreach tag $tags {
		    set status BAD
		    set tagRelPath [prjRelPath $hPrjMigrated $tag]
		    set prjTagData($hPrjMigrated,$tag) \
			[prjFullPath $hPrjMigrated \ 
			    [prjRelPath $hPrjOld $prjTagData($hPrjOld,[prjFullPath $hPrjOld $tagRelPath)]]
		    migrateLog $fdLog "Specified MIGRATE can't be done, COPY instead, \
			    user audition recommended:\n$tag \
			    \n $prjTagData($hPrjMigrated,$tag)"
		}
	    }
	    MAP {
		foreach tag $tags {
		    set prjTagData($hPrjMigrated,$tag) \
			$tabConv($tagPattern,DATA)
		}
	    }
	    MIGRATE {
		foreach tag $tags {
		    set val [$tabConv($tagPattern,METHOD) $tag]
		    if {$val != "NOP"} {
	                set prjTagData($hPrjMigrated,$tag) $val
		    }
		}
	    }
	    default {
		foreach tag $tags {
		    set status BAD
		    migrateLog $fdLog "Action for tag pattern \
		       $tabConv($tagPattern,ACTION) is not defined, \
		       COPY by default, user audition recommended:\n \
		       $tagPattern \n $prjTagData($hPrjMigrated,$tagPattern)"
		}
	    }
	}
    }

    # New tags may have been added to project in new release, add them if any

    set tagListNew [array names prjTagData $hPrjTemp,*]
    regsub -all ${hPrjTemp}, $tagListNew "" tagListNew

    # macro like PRJ_DIR has been expanded, in order to do compare tags between
    # projects, the expanded tag has to be contracted

    set tagListNewRel {}
    foreach tag $tagListNew {
	lappend tagListNewRel [prjRelPath $hPrjTemp $tag]
    }
    regsub -all $bspNameMigrated $tagListNewRel "$argument(bsp)" tagListNewRel
    set tagListOld [array names prjTagData $hPrjOld,*]
    regsub -all ${hPrjOld}, $tagListOld "" tagListOld
    set tagListOldRel {}
    foreach tag $tagListOld {
	lappend tagListOldRel [prjRelPath $hPrjOld $tag]
    }
    foreach tag $tagListNewRel {
	if {[lsearch $tagListOldRel $tag] == -1} {
	    set idx [lsearch $tagListNewRel $tag]
	    set tagExpandedTemp [lindex $tagListNew $idx]
	    set tagExpandedMigrated [prjFullPath $hPrjMigrated $tag]
	    set prjTagData($hPrjMigrated,$tagExpandedMigrated) \
		[prjRelPath $hPrjTemp $prjTagData($hPrjTemp,$tagExpandedTemp)]
	}
    }

    # In case of bsp/wcc name change, replace argument(bsp) with 
    # bspNameMigrated, wcc with wccNameMigratedOf($wcc) in both tag and value

    set wcc [wccCurrentGet $hPrjMigrated]
    set tagList [array names prjTagData $hPrjMigrated,*]
    foreach tag $tagList {
	if [regexp {WCC_} $tag] {
	    if [info exists wccNameMigratedOf($wcc)] {
	        regsub -all $wcc $tag $wccNameMigratedOf($wcc) tagMigrated
		regsub -all $wcc $prjTagData($tag) $wccNameMigratedOf($wcc) \
		    valMigrated
		unset prjTagData($tag)
		set prjTagData($tagMigrated) $valMigrated
	    }
	} else {
	    if {$argument(bsp) != $bspNameMigrated} {
		set tagMigrated [prjRelPath $hPrjMigrated $tag]
		regsub -all $argument(bsp) $tagMigrated $bspNameMigrated \
		    tagMigrated
		set tagMigrated [prjFullPath $hPrjMigrated $tagMigrated]
		set valMigrated [prjRelPath $hPrjMigrated $prjTagData($tag)]
		regsub -all $argument(bsp) $valMigrated $bspNameMigrated \
		    valMigrated
		set valMigrated [prjFullPath $hPrjMigrated $valMigrated]
		unset prjTagData($tag)
		set prjTagData($tagMigrated) $valMigrated
	    }
  	}
    }
}

proc prjMigrate::run {} {
    variable status
    variable mode
    variable argument
    variable prjTemporaryDir
    variable hPrjTemp
    variable hPrjMigrated
    variable fdLog
    variable libExt
    variable homedir

    if {$::env(WIND_HOST_TYPE) == "x86-win32"} {
	set libExt "dll"
    } else {
	set libExt "so"
    }

    if [argsGet] {

        if {$mode != "auto"} {
	    if {($argument(macro) != "") || ($mode == "info")} {

	        # initialize mode-independent data structures

	        dataInit

		puts "[buildMacroQuery $argument(macro) $argument(value)]"
	    } elseif {$argument(components) != ""} {
		dataInitCompAndParam
		puts "[componentQuery $argument(components)]"
	    }
	} elseif {$mode == "auto"} {

	    # initialize mode-independent data structures

	    dataInit

	    set fdLog [open $argument(prjDirMigrated)/migration.log "a+"]
	    migrateLog $fdLog "Migration of $argument(prjDir)"

            # initialized data structure specific to AUTO mode

	    dataInitMigration
	    
    	    file copy $argument(prjDirMigrated)/Makefile \
	      	      $argument(prjDirMigrated)/Makefile.origin

            set windbaseCurrent $::env(WIND_BASE)
            regsub -all {\\} $windbaseCurrent "\\\\\\\\" windbasePattern
	    regsub -all {\\} $windbaseCurrent "/" windbaseCurrent
            set pathCurrent $::env(PATH)
            regsub -all $windbasePattern $pathCurrent $argument(windbase) \
			pathOld

    	    # switch to context of the original Tornado installation

    	    set ::env(WIND_BASE) $argument(windbase)
	    set ::env(PATH) $pathOld

 	    # generate default Makefile (Makefile.default) with the original 
	    # project facility makeDefaultGen.tcl loads tcl files of the 
	    # project facility To get a comparable Makefile.default, the 
	    # original project must be in the place where its current Makefile
	    # (Makefile.origin) got created, otherwise the Makefile.default and
	    # Makefile.origin will have different PRJ_DIR for sure

	    if ![catch {prjFileCRRemove $argument(prjFile)} errMsg] {
	        exec $::env(WIND_BASE)/host/$::env(WIND_HOST_TYPE)/bin/wtxtcl \
		     ${homedir}/makeDefaultGen.tcl $argument(prjFile) \
		     $argument(prjDir) $argument(prjDirMigrated) >@stdout
	    } else {
		error "Can't remove \\\r from $argument(prjFile).\n$errMsg"
	    }

            # switch back to context of the new Tornado installation

            set ::env(PATH) $pathCurrent
            set ::env(WIND_BASE) $windbaseCurrent

	    # compare Makefile.default and Makefile.origin

	    set fDefault [open $argument(prjDirMigrated)/Makefile.default "r"]
	    set fOrigin [open $argument(prjDirMigrated)/Makefile.origin "r"]

	    set customerChange "NO"
	    set lineDefault ""
	    set lineOrigin ""
	    while {[gets $fDefault lineDefault] >= 0} {
		if {[regexp {^[ \t]*$} $lineDefault] || \
		    [regexp {^#} $lineDefault]} {
		    set lineDefault ""
		    continue
		}
		while {[gets $fOrigin lineOrigin] >= 0} { 
		    if {![regexp {^[ \t]*$} $lineOrigin] && \
			![regexp {^#} $lineOrigin]} {
			break
		    }
		    set lineOrigin ""
		}
		if {$lineOrigin == ""} {
		    break
		}

		set len [llength $lineDefault]
		if {([llength $lineOrigin]) != $len} {
		    set customerChange "YES"
		    break
		}

		set idx 0
		while {$len > $idx} {
		    set itemOrigin [lindex $lineOrigin $idx]
		    set itemDefault [lindex $lineDefault $idx]
		    if {"$itemOrigin" != "$itemDefault"} {
			set itemOriginLower [string tolower $itemOrigin]
			set itemDefaultLower [string tolower $itemDefault]
			if {"$itemOriginLower" != "$itemDefaultLower"} {
			    set customerChange "YES"
			    break
			} else {
			    migrateLog $fdLog "Makefile: Consider \
				$itemOrigin and $itemDefault to be equal."
			}
		    }
		    incr idx
		}
		if {$customerChange == "YES"} {
		    break
		}
		set lineDefault ""
		set lineOrigin ""
	    }
	    if {$customerChange == "NO"} {
		if {$lineDefault != ""} {
		    set customerChange "YES"
		} else {
		    while {[gets $fOrigin lineOrigin] >= 0} {
			if ![regexp {[ \t]*} $lineOrigin] {
		            set customerChange "YES"
			    break
			}
		    }
		}
	    }
	    if {$customerChange == "YES"} {
		migrateLog $fdLog "Customer has modified Makefile. \
				   Customer's Makefile has been renamed to \
				   Makefile.origin. Makefile.default is the \
				   Makefile which customer's changes are based \
				   on. Customers are reponsible \
				   for bringing their changes over into new \
				   Makefile.\n\n"
		migrateLog $fdLog ">>>$lineDefault\n<<<$lineOrigin\n"
		close $fDefault
		close $fOrigin
	    } else {
		close $fDefault
		close $fOrigin
		file delete $argument(prjDirMigrated)/Makefile.default
		file delete $argument(prjDirMigrated)/Makefile.origin
	    }
		    
            # migrate project

	    migrate

   	    # mark the project as dirty to make sure it'll be saved

	    prjInfoSet $hPrjMigrated changed [clock seconds]
	    prjClose $hPrjMigrated SAVE

	    if {$status == "GOOD"} {
		migrateLog $fdLog "Migration is complete!"
	    } else {
		migrateLog $fdLog "Please make sure above logged ACTIONS/CHANGES are OK before proceed."
	    }
	    close $fdLog
	    puts "\n\nThe converted project is in $argument(prjDirMigrated)"
	    puts "\nPlease examine $argument(prjDirMigrated)/migration.log"
            prjClose $hPrjTemp
	}
    }
}

if [catch {prjMigrate::run} prjMigrateError] {
    puts "$prjMigrateError"
    if {$prjMigrate::hPrjTemp != 0} {
        file delete -force $prjMigrate::prjTemporaryDir
    }
}
