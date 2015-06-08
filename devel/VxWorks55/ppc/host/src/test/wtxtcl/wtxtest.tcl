# wtxTest.tcl - TCL script, wtxtcl test suite launcher (UNIX)
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01z,05mar02,fle  made it exec on its own executable name
# 01y,23aug01,dgp  add note per SPR 5997: wtxtest not installed by default
# 01x,24mar99,dgp  doc: make UNIX only, final editing changes
# 01w,05mar99,p_b  cleanUp.
# 01v,18jan99,p_b  rename wtxCommonProc in wtxTestLib.tcl
# 01u,27may98,fle  doc : made file header correct for refgen
# 01t,06may98,p_b  WTX 2.0 tests
# 01s,28jan98,p_b  written, mainly inspired from obsolete wtxtest.sh
#
# SYNOPSIS
# .tS
# tclsh wtxtest.tcl <serverIdentifier> [-h] [-n <expr>] [-l [<globexp>]]
#         [-t <timeout>] [-i <invalidAddr>] [-SUPPRESS] [<tests[regexp]> ...]
#         [-tsload][-r [<files>]]
# .tE
#
# DESCRIPTION
# This tool thoroughly checks each WTX (Wind River Tool
# Exchange) protocol service.  Several unit tests test various features of
# the WTX services.
# This test tool, based on exclusively TCL scripts is a convenient way to run
# the WTX services test suite from the UNIX shell.
#
# When started, the test tool needs to connect to a specified target server
# (see `tgtsvr') whose identifier is given as a required parameter.  It is
# possible to use a regular expression for the server identifier.
# For example, the following will run all the tests on vxsim1@aven:
# .CS
#     aven% tclsh wtxtest.tcl sim1
# .CE
# If there is an ambiguity in your target server name, an error message
# is displayed.
#
# The <testName> parameter is optional.  If omitted, the test tool will apply
# all tests in the test suite (use the `-l' option to see the list of tests).
# It is possible to run a subset of tests by using a glob-style expression.
# For example, the following will run all the memory tests:
# .CS
#     aven% tclsh wtxtest.tcl vxsim1@aven Mem
# .CE
# The following will run all the context and event tests:
# .CS
#     aven% tclsh wtxtest.tcl vxsim1@aven Cont Eve
# .CE
#
# NOTE
# 'wtxtest' is available in the Tornado distribution but it is not installed 
# by default. If you want to use it, you must install it using SETUP. In
# the "Select Products" dialog box, highlight the appropriate Tornado object
# and click "Details." Check "Tornado WTX Test Suite" in the "Select Parts"
# dialog box and complete your installation.
#
# OPTIONS
# .iP "-h" 10 3
# Print a help message about test tool usage and options.
# .iP "-i <expr>"
# This option must be followed by a TCL expression.  This expression is
# evaluated before any other work by `wtxtcl'.  This option is used mainly
# to debug the test suite.
# .iP "-l [<globexp>]"
# List all the unit tests in the test suite.  It is possible to get only the
# tests that match a certain pattern by specifying a glob-style expression.
# .iP "-t <timeout>"
# This option allows a timeout to be set for all the tests in the test suite.
# .iP "-I <invalidAddr>"
# This option allows an invalid address to be specified when it cannot be
# determined by the test suite.
# .iP "-SUPPRESS"
# Do not run tests with invalid addresses.
# .iP "-tsload"
# Use direct access by target server to load an object module.
# .iP "-r [<files>]"
# Create the reference files required to test the target server in loopback
# mode.  This option should be invoked only if the reference files are
# missing.
# .LP
#
# ENVIRONMENT VARIABLES
# .iP "WIND_BASE" 10 3
# specifies the root location of the Tornado tree.
# .LP
#
# FILES
# The files in the following directories are required by the test tool:
# .iP "`/tmp' and `/dev' " 10
# directories for temporary files. 
# .iP "$WIND_BASE/host/src/test/wtxtcl " 10 3
# directory for unit test files.
# .iP "$WIND_BASE/target/lib/obj<CPUtool>test "
# directory for loader test files.
# .iP "$WIND_BASE/host/resource/test "
# directory for resource files required to run the test suite.
# .LP
#
# SEE ALSO
# `tgtsvr',
# `launch',
# `wtxregd',
# .I "Tornado API User's Guide"
#
# NOROUTINES
#


#*************************************************************************
#
# showHelp - Display help for wtxtest.tcl usage
#
#

proc showHelp { } {
    
    set usage "usage : wtxtest tgtsvr \[-h\] \[-n expr\] \[-l \[globexp\]\] \[-t timeout\] \[-i invalid address\] \[-SUPPRESS\] \[tests ...\] \[-r \[files\]\]" 
    
    puts stdout ""
    puts stdout "This command executes one or more WTX protocol test(s)"
    puts stdout ""
    puts stdout $usage
    puts stdout ""
    puts stdout "Options available are:"
    puts stdout "-h\[elp\]         print this message"
    puts stdout "-n\[initial\]     execute initial Tcl expression"
    puts stdout "-l\[ist\]         print test list"
    puts stdout "-t\[imeout\]      set a timeout for all wtx commands"
    puts stdout "-i\[nvalid addr\] set an invalid address for tests"
    puts stdout "-SUPPRESS       to not run tests with invalid addresses"
    puts stdout "-tsload         use direct access by target server to load\
	    an object module"
    puts stdout "-r\[eference\]    create reference files"
    puts stdout ""
    
}

#*************************************************************************
#
# appendStringToFile - Provides a "echo" Unix like (or "echo" NT-95 like)
#  function. Needed for NT/UNIX compatibility.
#

proc appendStringToFile { source dest flag } {
    
    # source is a string
    # dest is a filename
    # flag == 1, the file is created
    
    if { $flag == 1} {
	set idDest [open $dest w+]
	seek $idDest 0
    } else {
	set idDest [open $dest a]
    }
    
    puts $idDest $source
    
    close $idDest
}

#*************************************************************************
#
# fileCat - Provides a "cat" Unix like (or "type" NT-95 like) function
#           Needed for NT/UNIX compatibility.
#

proc fileCat { source dest flag } {
    
    # source is a filename
    # dest is a filename
    # flag == 1, the file is created
    
    set idSrc [open $source r]
    
    if { $flag == 1} {
	set idDest [open $dest w+]
	seek $idDest 0
    } else {
	set idDest [open $dest a]
    }
    
    seek $idSrc 0
    
    while { ![eof $idSrc] } {
	
	gets $idSrc var
	puts $idDest $var 
	
    }
    
    close $idSrc
    close $idDest
}

#*************************************************************************
#
# checkWtxTcl - Check if wtxtcl can be run
#
#

proc checkWtxTcl { } {
    
    if [catch {exec [wtxtclPathGet] < genEvent.tcl}] {
	puts stdout "wtxtcl can not be run"
	return 0
    }
    
    return 1
    
}

#*************************************************************************
#
# checkTargetServer - check target server validity 
#
#

proc checkTargetServer { tgtSvrName } {
    
    global filesCnt
    
    if { $tgtSvrName == "" } {
	
	showHelp
	return 0
	
    } else {
	
	fileCat checkServerName.tcl $filesCnt 1
	appendStringToFile "checkServerName $tgtSvrName" $filesCnt 0
	if [catch {exec [wtxtclPathGet] $filesCnt}] {
	    puts stdout "Target server name $tgtSvrName is invalid !"
	    return 0
	}
	
	return 1
	
    }
    
}

#*************************************************************************
#
# buildListTestFiles - Build the list of valid test that will be run 
#
#

proc buildListTestFiles { paramList createRef regExpr\
	listFiles handleCfgFile } {
    
    global filesCnt
    global testNames
    global correctFiles
    
    source $handleCfgFile
    set testNames ""
    set testNamesNT ""
    
    foreach param $paramList {
	for {set i 1} { $i <= $LAST_TEST } {incr i} {
	    if {[string match *$param* $wtxFunct($i)]} {
		lappend testNames "$wtxFunct($i)"
	    }
	}
    }
    
    # If this client runs under NT, removes test files
    # that are not supposed to run under this system.
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
	foreach param $testNames {
	    set OkToAppend 1
	    for {set i 1} { $i <= $LAST_TEST_NOT_NT } {incr i} {
		if {[string match *$testsNotToRunUnderNT($i)* $param]} {
		    set OkToAppend 0
		}
	    }
	    if { $OkToAppend == 1 } { lappend testNamesNT "$param" }
	}
	set testNames $testNamesNT
    }
    
    # If listFiles, then print files and exit.
    if { $listFiles == 1 } {
	puts stdout ""
	foreach param $testNames {
	    puts stdout $param
	}
	puts stdout ""
	exit 1
    }
    
    # Fills main file with wtxTestLib.tcl (Always)
    fileCat wtxTestLib.tcl $filesCnt 1
    
    # If createRef, fills main file with wtxCreateRef.tcl and run it
    if { $createRef == 1 } {
	fileCat wtxCreateRef.tcl $filesCnt 0
	appendStringToFile "wtxCreateRef $tgtSvrName $argv" $filesCnt 0
	exec [wtxtclPathGet] $filesCnt >&@ stdout
    }
    
    # If regExpr, fills main file with passed args and run it
    if { $regExpr != "" } {
	appendStringToFile "$regExpr" $filesCnt 0
	exec [wtxtclPathGet] $filesCnt >&@ stdout
    }
    
    # Fills main file with all tests to be run 
    foreach param $testNames {
	if [file exists $param.tcl] {
	    set correctFiles "$correctFiles $param"
	    fileCat $param.tcl $filesCnt 0
	}
    }
    
}

#*************************************************************************
#
# initListFiles - Initialization of misc files
#
#

proc initListFiles {} {
    
    global testsListFile
    global filesCnt
    global env
    set wtxTestTmp wtxTestFile[clock seconds]
    
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
	
	set resdir $env(WIND_BASE)\\host\\resource\\test
	set testsListFile $resdir\\testScriptConfig.tcl
	set filesCnt $env(TMP)\\$wtxTestTmp
	
    } else {
	
	set resdir $env(WIND_BASE)/host/resource/test
	set testsListFile $resdir/testScriptConfig.tcl
	set filesCnt /tmp/$wtxTestTmp
    }
    
    return $testsListFile
    
}

#*************************************************************************
#
# main - Main procedure
#

proc main {argc argv} {
    
    global filesCnt
    global testNames
    global correctFiles
    
    set paramList ""
    set correctFiles ""
    set invalidAddr 0 
    set tsLoadByTS 0
    set createRef 0
    set listFiles 0
    set regExpr ""
    set timeOut 0
    set i 0
    
    set handleCfgFile [initListFiles]
    
    # Build list of tests and Get misc parameters
    
    while { $i <= $argc } {
	switch -exact -- [lindex $argv $i] {
	    "-?" { 
		showHelp
		exit 0 
	    }
	    "-h" {
		showHelp 
		exit 0
	    }
	    "-H" {
		showHelp 
		exit 0
	    }
	    "-n" {
		set regExpr [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-N" {
		set regExpr [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-l" { set listFiles 1 }
	    "-L" { set listFiles 1 }
	    "-t" {
		set timeOut [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-T" {
		set timeOut [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-i" {
		set invalidAddr [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-tsload" { set tsLoadByTS 1 }
	    "-TSLOAD" { set tsLoadByTS 1 }
	    "-I" {
		set invalidAddr [lindex $argv [expr $i + 1]]
		incr i
	    }
	    "-suppress" { set invalidAddr "SUPPRESS" }
	    "-SUPPRESS" { set invalidAddr "SUPPRESS" }
	    "-r" { set createRef 1 }
	    "-R" { set createRef 1 }
	    default { set paramList "$paramList [lindex $argv $i]" }
	}
	incr i
    }
    
    # Check if wtxtcl can be run
    if { ![checkWtxTcl] } { return 0 }
    
    # Check Target server
    set tgtSvrName [lindex $argv 0]
    if { ![checkTargetServer $tgtSvrName] } { return 0 }
    
    # if no test has been specified, run all tests.
    if { [lindex $paramList 1] == "" } {
	set paramList "wtx"
    }
    
    buildListTestFiles $paramList $createRef $regExpr $listFiles $handleCfgFile
    
    # Run all the tests
    puts stdout ""
    puts stdout "                      WTX PROTOCOL TEST "
    puts stdout "                      ----------------- "
    puts stdout ""
    
    appendStringToFile "mainCommonProc { $correctFiles } $tgtSvrName $timeOut\
	    $invalidAddr $tsLoadByTS" $filesCnt 0
    exec [wtxtclPathGet] $filesCnt >&@ stdout 
    
    # Clean up
    file delete $filesCnt
    
}

################################################################################
#
# wtxtclPathGet - get path of wtxtcl executable currently used
#
# RETURNS: The path to this wtxtcl executable, without the extension
#

proc wtxtclPathGet {} {

    return [file rootname [info nameofexecutable]]
}

main $argc $argv












