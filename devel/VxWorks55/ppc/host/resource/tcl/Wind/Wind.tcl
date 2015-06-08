# Wind.tcl - Wind River specific routines

# Copyright 2001-2002 Wind River Systems, Inc.

#
# modification history
# --------------------
# 01j,25mar03,rbl  change version number to 2.2.1
# 01i,29jan03,rbl  add patchVersion function
# 01h,28may02,cmf  change tmpDirGet to check for tmp directories before
#                  calling appDirGet
# 01g,22may02,cjs  Move filepathQuoteIfNecessary() to Utils.tcl
# 01f,22may02,cjs  Put filenameQuoteIfNecessary() into the Wind namespace
# 01e,17may02,fle  fixed appDataDirGet not working on solaris
# 01d,05may02,cmf  change tmpDirGet to not use hard coded array
# 01c,04oct01,fle  added Wind::path procedure
# 01b,26mar01,fle  added freebsd hosts to windHostTypeGet, and added tmpDirGet
#                  procedure
# 01a,26feb01,fle  written
#

#
# DESCRIPTION
#
# This namespace defines several of the Wind River specific calls. It allows
# to sometimes get rid of wtxtcl.
#
# Namely, for Tcl scripts that were calling wtxtcl just because they needed
# `wtxPath' or `wtxHostType', it is now possible to call this package.
#
# EXAMPLES
# How to replace a call to `wtxtcl' `wtxPath':
# \cs
# % package require Wind
# 1.0
# % Wind::path resource tcl
# /usr/tornado/resource/tcl/
# %
# \ce
#
# Or how to replace `wtxtcl' `wtxHostType' :
# \cs
# % package require Wind
# 1.0
# % Wind::hostType
# sun4-solaris2
# %
# \ce
#

namespace eval Wind {

    variable mHostType		""		;# Wind River Host type
    variable mBase		""		;# Wind River home base
    variable mUserName		""		;# user's name
    variable mInitialised	0		;# Wind status
    variable mVersion		""		;# Tornado version
    variable mKey		""		;# Tornado Win32 Registry key
    variable mAppDataDir	""		;# applications data dir
    variable mTmpDir		""		;# Wind River Temp dir

    variable mTmpDirList	{"/var/tmp" "/usr/tmp" "/tmp"}
    
    variable COMPANY_NAME	"Wind River Systems"
    variable PRODUCT_NAME	"Tornado"
    variable TOR_VERSION	"2.2"		;# tornado current version
    variable REG_DIR		"Software\\Wind River Systems\\Tornado"
    variable APPDATA		"Application Data"
    variable LOCAL_SETTINGS	"Local Settings"
    variable WIND_PRIVATE_DIR	".wind"
    variable WINDOWS_HOSTTYPE	"x86-win32"
    variable M_PATCHLEVEL	".1"
}

################################################################################
#
# Wind::hostType - gets the WIND_HOST_TYPE for Wind namespace
#
# SYNOPSIS
# \cs
#   Wind::hostType
# \ce
#
# Depending on the host platform and the host OS, this procedure returns the
# Wind Host type. The returned value is the same as the Unix wind_host_type
# value :
#
# \ibs
# \ibi `x86-win32' for Windows hosts
# \ibi `x86-linux2' for Unix Linux hosts
# \ibi `sun4-solaris2' for Sun Solaris 2 hosts
# \ibi `x86-freebsd4' for FreeBSD 4 hosts
# \ibe
#
# RETURNS:
# A String associated to this host platform
#

proc Wind::hostType {} {

    global env			;# user's environment definition
    global tcl_platform		;# tcl platform definitions

    variable mHostType		;# Wind River Host type

    # first check if it is initialised or not

    if {$mHostType == ""} {

	# check if it is not already defined in user environment

	if {[info exist env(WIND_HOST_TYPE)]} {
	    set mHostType $env(WIND_HOST_TYPE)
	} else {
	    if {$tcl_platform(platform) == "windows"} {
		set mHostType  "x86-win32"
	    } elseif {$tcl_platform(platform) == "unix"} {
		if {$tcl_platform(os) == "Linux"} {
		    set mHostType  "x86-linux2"
		} elseif {$tcl_platform(os) == "SunOS"} {
		    set mHostType  "sun4-solaris2"
		} elseif {$tcl_platform(os) == "FreeBSD"} {
		    set mHostType  "x86-freebsd4"
		} else {
		    Log::error "Unknown Unix Host Type : $tcl_platform(os)"
		}
	    } else {
		Log::error "Unknown Platform : $tcl_platform(platform)"
	    }
	}
    }

    return $mHostType
}

################################################################################
#
# Wind::path - returns the Tornado Base directory plus given arguments
#
# SYNOPSIS
# \cs
#   Wind::path [args]
# \ce
#
# PARAMETERS
#   args : A list of arguments to concatenate with Tornado Base directory
#
# EXAMPLES:
# Get the Tornado base directory
# \cs
# % package require Wind
# 1.0
# % Wind::path
# C:/Tornado/
# %
# \ce
#
# Get Tornado path to init.tcl in the resource directory
# \cs
# % package require Wind
# 1.0
# % set fileName [Wind::path host resource tcl]init.tcl
# C:/Tornado/host/resource/tcl/init.tcl
# \ce
#
# RETURNS:
# A File Path made of Tornado Base directory and given arguments.
#
# SEE ALSO : `Wind::base'
#

proc Wind::path {args} {

    # check if it is a set or a get Wind Base.

    set windPath [Wind::base]

    if {$args != ""} {

	# append arguments to the path to get

	foreach argument $args {
	    set windPath [file join $windPath $argument]
	}
    }

    if {[Wind::hostType] == "x86-win32"} {
	set windPath [format "%s\\" $windPath]
    } else {
	set windPath [format "%s/" $windPath]
    }

    return $windPath
}

################################################################################
#
# Wind::base - gets the Tornado base directory
#
# SYNOPSIS
# \cs
#   Wind::base [newBase]
# \ce
#
# This procedure looks for user's environment variable WIND_BASE. If it
# esists, it is returned.
#
# If <newBase> was given as an argument, the new Wind base is set to that
# value only if it is a valid directory.
#
# PARAMETERS
#   newBase : The new WIND_BASE to be used (must be a valid directory)
#
# RETURNS:
# The Tornado base directory path
#
# EXAMPLES:
# Get the Tornado directory path :
# \cs
# % package require Wind
# 1.0
# % Wind::base
# C:\Tornado
# %
# \ce
#
# Set it to a new location :
# \cs
# % package require Wind
# 1.0
# % Wind::base D:\T2
# D:\T2
# %
# \ce
#

proc Wind::base {{newBase ""}} {

    global env			;# user's environment variables

    variable mBase		;# Wind base directory

    # check if it is a set or a get Wind Base.

    if {$newBase == ""} {

	# user wants to get the Wind Base

	if {$mBase == ""} {
	    if {[info exists env(WIND_BASE)]} {
		set mBase [file nativename $env(WIND_BASE)]
	    } else {
		Log::error "Cannot find WIND_BASE environment variable"
	    }
	}
    } else {

	# user wants to specify a new Wind Base, just check that it exists
	# and is a directory

	if {[file exists $newBase] && [file isdirectory $newBase]} {
	    set mBase $newBase
	} else {
	    Log::error "Invalid wind base : $newBase"
	}
    }

    return $mBase
}

################################################################################
#
# Wind::which - parses the user's PATH to find the best <executable>
#
# SYNOPSIS
# \cs
#   Wind::which executable
# \ce
#
# This procedure parses the user's PATH environment variable (if any). If there
# is now PATH variable, "." adn ".." are used.
#
# On windows hosts, if <executable> does not end with .exe, the procedure will
# add it, and look for <executable>`.exe'.
#
# The first matching occurrence will be returned.
#
# PARAMETERS
#   executable : name of the executable file to look for
#
# RETURNS: The path of <executable> if found, or "" else
#
# EXAMPLE
# Look for the wtxtcl executable :
# \cs
# % package require Wind
# 1.0
# % Wind::which
# Wind::which wtxtcl
# /usr/tornado/host/sun4-solaris2/bin/wtxtcl
# %
# \ce
#

proc Wind::which {executable} {

    global tcl_platform			;# system platform information
    global env				;# user's environment

    # check for the usrer's PATH

    if {[info exists env(PATH)]} {
	set path $env(PATH)
    } else {
	set path "."
    }

    switch -- $tcl_platform(platform) {

	"windows" {

	    if {[file extension $executable] != ".exe"} {
		set executable [format "%s.exe" $executable]
	    }

	    # split the path list on semi column characters

	    set tmpPathList [split $path ";"]
	}

	"unix"
	    -
	default {

	    # do not add any .exe extension to the file
	    # split the path list on column characters

	    set tmpPathList [split $path ":"]
	}
    }

    # append some useful paths

    set pathList {}
    lappend tmpPathList "."
    lappend tmpPathList ".."

    foreach tmpPath $tmpPathList {

	if {($tmpPath != "") &&
	    ([lsearch -exact $pathList $tmpPath] == -1) &&
	    ([file exists $tmpPath])} {

	    lappend pathList $tmpPath
	}
    }

    # now look for the given executable in each and every path from the path
    # list

    set execPath ""

    foreach path $pathList {

	set executablePath [file join $path $executable]

	if {[file executable $executablePath]} {

	    set execPath $executablePath
	    break
	}
    }

    return $execPath
}

################################################################################
#
# Wind::user - returns possible name of the current user
#
# SYNOPSIS
# \cs
#   Wind::user [newUser]
# \ce
#
# This procedure returns the name of the current user if it could be found.
#
# If the <newUser> argument is given, then the name of the current user is set
# for <newUser>.
#
# If <newUser> is not provided, the procedure will look for the User defined
# environment variables : USER, USERNAME, and if non of them exist, it will try
# to call for the global tcl_platform(user) variable.
#
# PARAMETERS
#   newUser : name of the current user to use in subsequent calls
#
# RETURNS: The name of the current user
#
# EXAMPLES
# Get the current user
# \cs
# % package require Wind
# 1.0
# % Wind::user
# fredl
# %
# \ce
#
# Set a new user name :
#
# \cs
# % package require Wind
# 1.0
# % Wind::user wind_user
# wind_user
# % Wind::user
# wind_user
# %
# \ce
#

proc Wind::user {{newUser ""}} {

    global env			;# user environment variables

    variable mUserName		;# this Wind environment user name

    if {$newUser == ""} {

	if {$mUserName == ""} {

	    # try to find the user

	    if {[info exists env(USER)]} {
		set mUserName [string trim $env(USER)]
	    } elseif {[info exists env(USERNAME)]} {
		set mUserName [string trim $env(USERNAME)]
	    } elseif {[info exists tcl_platform(user)]} {
		set mUserName [string trim $tcl_platform(user)]
	    } else {
		set mUserName unknown
	    }
	}
    } else {
	set mUserName $newUser
    }

    return $mUserName
}

################################################################################
#
# Wind::appDataDirGet - get a valid application data directory
#
# SYNOPSIS
# \cs
#   Wind::appDataDirGet
# \ce
#
# PARAMETERS: N/A
#
# This procedure gets a Valid Wind River Application Data directory path. If the
# WIND_DATADIR environment variable is defined, its value is returned, but no
# check is made to see if it points to a writable directory.
#
# If the directory this procedure retuns does not exist, it is recursively
# created.
#
# WINDOWS PLATFORMS
#
# If the `USERPROFILE' environment variable exists, this procedure returns
#
# \cs
# USERPROFILE/Local Settings/Application Data/Wind River Systems/Tornado/<vers>
# \ce
#
# Where <vers> is the Tornado version as returned by `Wind::version'
#
# UNIX PLATFORMS
#
# If the `HOME' environment variable exists, this procedure returns
#
# \cs
# HOME/.wind
# \ce
#
# RETURNS: The Wind River Application Data directory path
#
# ERRORS:
# \is
# \i No USERPROFILE environment variable (Windows)
# The USERPROFILE environment variable does not exist, thus it is impossible to
# return a valid Application Data Directory. Please define the WIND_DATADIR.
# \i No HOME environment variable (Unix)
# The HOME environment variable does not exist, thus it is impossible to
# return a valid Application Data Directory. Please define the WIND_DATADIR.
# \ie
#
# ERRORS: N/A
#
# SEE ALSO: Wind::tmpDirGet
#
# INTERNAL: When modifying this procedure, please also modify the
# wpwrAppDataDirGet () routine in host/src/libwpwr/util/wpwrutil.c
#

proc Wind::appDataDirGet {} {

    global	env

    variable	APPDATA
    variable	COMPANY_NAME
    variable	LOCAL_SETTINGS
    variable	PRODUCT_NAME
    variable	TOR_VERSION
    variable	WIND_PRIVATE_DIR
    variable	WINDOWS_HOSTTYPE

    variable	mAppDataDir

    set appDataDir ""

    # first check the cached data dir

    if {$mAppDataDir != ""} {
	return $mAppDataDir
    }

    # check the WIND_DATADIR environment variable

    if {[info exists ::env(WIND_DATADIR)]} {	
	set mAppDataDir $::env(WIND_DATADIR)
	return $mAppDataDir
    } 

    # compute the app data dir depending on the host type

    if {[Wind::hostType] == $WINDOWS_HOSTTYPE} {

	if {[info exists ::env(USERPROFILE)]} {	

	    set appDataDir [file join $::env(USERPROFILE) $LOCAL_SETTINGS \
				      $APPDATA $COMPANY_NAME $PRODUCT_NAME \
				      $TOR_VERSION]
	} else {
	    error "No USERPROFILE environment variable"
	}
    } else {
	
	# unix platforms

	if {[info exists ::env(HOME)]} {

	    set appDataDir [file join $::env(HOME) $WIND_PRIVATE_DIR]
	} else {
	    error "No HOME environment variable defined"
	}
    }

    # create the app data dir if needed

    if {! [file exists $appDataDir]} {

	if {[catch {file mkdir $appDataDir} mkdirStatus]} {
	    error "Could not create $appDataDir : $mkdirStatus"
	}
    }

    set mAppDataDir $appDataDir

    return $mAppDataDir
}

################################################################################
#
# Wind::tmpDirGet - get a valid temp directory
#
# SYNOPSIS
# \cs
#   Wind::tmpDirGet
# \ce
#
# PARAMETERS: N/A
#
# This procedure looks for a valid temp directory for the current user. 
#
# On Windows
# It first tries to get the Wind Data Dir by calling `Wind::appDataDirGet'
#
# If it fails, then `TEMP' and `TMP' environment variables are checked, and
# the first to point to a valid (existing and writable) directory, is
# returned.
#
# On Unix Platforms
# The `TEMP' and `TMP' environment variables are checked, and
# the first to point to a valid (existing and writable) directory, is
# returned.
#
# If there is still no valid temp dir, /var/tmp, /usr/tmp and /tmp directories
# are checked. Still the first existing and writable is returned.
#
# If all of these fail, it will try to use the application data directory
# by calling `Wind::appDataDirGet'
#
# If all those methods failed, the procedure exist on error "Could not find a\
# valid temp directory. Please define WIND_DATADIR environment variable."
#
# RETURNS: Path to a valid temp directory.
#
# ERRORS:
# \is
# \i "Could not find a valid temp directory ..."
# All the methods described above to get a valid temp directory failed. It is
# recommended to create the WIND_DATADIR environment variable.
# \ie
#
# SEE ALSO: Wind::appDataDirGet
#
# INTERNAL: When modifying this procedure, please also modify the
# wpwrTmpDirGet () routine in host/src/libwpwr/util/wpwrutil.c
#

proc Wind::tmpDirGet {} {

    global	env

    variable	mTmpDir
    variable	mTmpDirList
    variable	WINDOWS_HOSTTYPE

    # set tmpDir to NULL
    set tmpDir ""
	
    # on Unix
    if {[Wind::hostType] != $WINDOWS_HOSTTYPE} {

	# check for the TEMP and TMP environment variables
	# then check for /var/tmp, /usr/tmp, and /tmp
        if {([info exists env(TEMP)]) && ([file isdirectory $env(TEMP)]) &&
            ([file writable $env(TEMP)])} {

	    set tmpDir $env(TEMP)

	} elseif {([info exists env(TMP)]) && ([file isdirectory $env(TMP)]) &&
            ([file writable $env(TMP)])} {

	    set tmpDir $env(TMP)

	} else {
            foreach dir $mTmpDirList {

	        if {([file isdirectory $dir]) && ([file writable $dir])} {

                    set tmpDir $dir
                    break
                }
            }
	}
	
	# if tmpDir remains unset, 
        if {$tmpDir == ""} {

            # all else fails, use Application Data Directory 		
            if {[catch {Wind::appDataDirGet} tmpDir]} {
		set tmpDir ""
	    }
        }
	
    # On Windows, use Application Data Directory first, then check for TEMP, TMP
    # variables	
    } else {
 	#  Use Application Data Directory

        if {[catch {Wind::appDataDirGet} tmpDir]} {

	    set tmpDir ""	
	    # check for the TEMP and TMP environment variables

	    if {([info exists env(TEMP)]) && ([file isdirectory $env(TEMP)]) &&
	        ([file writable $env(TEMP)])} {

	        set tmpDir $env(TEMP)

	    } elseif {([info exists env(TMP)]) && ([file isdirectory $env(TMP)]) &&
	        ([file writable $env(TMP)])} {

                set tmpDir $env(TMP)
            } 
	
        }
    }

    # if tmpDir is still not set, exit on error

    if {$tmpDir == ""} {

        error "Could not find a valid temp directory. Please define\
            WIND_DATADIR environment variable."
    }


    set mTmpDir $tmpDir
    return $mTmpDir
}

################################################################################
#
# Wind::version - returns the current Tornado version
#
# SYNOPSIS
# \cs
#    Wind::version
# \ce
#
# In in a `wtxtcl' shell, the result of a call to `wtxTsVersionGet' is returned.
# If in a `tclsh' shell, the version is taken from a defined Tornado version.
#
# RETURNS: The current tornado version
#
# EXAMPLES
# Get the Wind version :
#
# \cs
# % package require Wind
# 1.0
# % Wind::version
# 2.2
# %
# \ce
#

proc Wind::version {} {

    variable mVersion
    variable TOR_VERSION

    if { $mVersion == "" } {
	if { [info commands wtxTsVersionGet] != "wtxTsVersionGet" } {
	    set mVersion $TOR_VERSION
	} else {
	    set mVersion [wtxTsVersionGet]
	}
    }

    return $mVersion
}

################################################################################
#
# Wind::key - get the Tornado Win32 Registry key
#
# SYNOPSIS
# \cs
#   Wind::key
# \ce
#
# This procedure creates the string that defines the where the Tornado
# informations are stored in the Win32 registry
#
# There is no <rootName>, i.e., the HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER
# is not prepended to the key.
#
# RETURNS: The Tornado Win32 Registry key
#
# EXAMPLES
# Just get the Tornado Win32 Registry key :
# \cs
# % package require Wind
# 1.0
# % Wind::key
# Software\Wind River Systems\Tornado 2.2
# %
# \ce
#
# SEE ALSO: `Wind::version'
#

proc Wind::key {} {

    variable mKey
    variable REG_DIR

    if { $mKey == "" } {

	# first get the Tornado version with only 2 numbers

	set version [Wind::version]

	regexp "(\[0-9]+)\.(\[0-9]+)" $version full major minor

	if { $minor == "" } {
	    set version $major
	} else {
	    set version [format "%s.%s" $major $minor]
	}

	# now create the registry key itself

	set mKey [format "%s %s" $REG_DIR $version]
    }

    return $mKey
}

################################################################################
#
# Wind::patchlevel - returns the current Tornado patch version
#
# SYNOPSIS
# \cs
#    Wind::patchlevel
# \ce
#
# FIXME: this probably should be added to wtxtcl e.g. to wpwrutil.c
# 
# RETURNS: The current tornado patch level
#
# EXAMPLES
# Get the version and patch level:
#
# \cs
# % package require Wind
# 1.0
# % Wind::version
# 2.2
# % Wind::patchlevel
# CP-1
# %
# \ce
#

proc Wind::patchlevel {} {
	variable M_PATCHLEVEL

    return $M_PATCHLEVEL
}

