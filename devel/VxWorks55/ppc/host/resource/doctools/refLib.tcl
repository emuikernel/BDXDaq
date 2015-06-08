# refLib.tcl - extract documentation from source comments library
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02z,04dec01,wsl  repair SYNOPSIS for PCL files
# 02y,03dec01,wsl  allow extra whitespace in EXPAND directives
# 02x,15nov01,wsl  add processing for IDL input format
# 02w,09oct01,wsl  fix handling of EXPANDPATH
# 02v,21aug01,wsl  speed up dependency build by fixing nameFileDoc
# 02u,10aug01,wsl  fix table formatting -- SPR 67239
# 02t,07aug01,wsl  fix bug in synopsis adding spurious '{' SPRs 65619, 67414
# 02s,05jun00,jmp  add a proc which return the doc file name.
# 02r,13apr00,dgp  correct error processing in refRtnsFormat
# 02q,01mar00,dgp  correct Error output
# 02p,25feb00,dgp  correct SYNOPSIS formatting and error processing
# 02o,08nov99,fle  fixed refDefnExpand procedure
# 02n,06oct99,dgp  correct error in bullet-list processing
# 02m,29sep99,dgp  cleanup comments, etc, accept target.nr or target.ref input
# 02l,27sep99,dgp  handle protocol synopsis correctly by adding refSynPclGet and 
#		   associated routines and by changing refSynGet to refSynCGet
# 02k,23sep99,dgp  fix param formatting error in refSynFormat
# 02j,08sep99,dgp  modify refSynFormat to handle complex parameters by adding
#                  refParamsParse and refParamsSplitPair, and modify refParamAlign 
#		   by adding refParamShort and refParamLong; remove refParamTypeGet,
#		   refParamSepGet, refParamNameGet, and refParamCommentGet
# 02i,31aug99,dgp  correct handling of macro synopses, synopses with ..., 
#                  SEE ALSO where lib name is included in routine names, and
#                  multi-line tables in target.nr
# 02h,09aug99,dgp  correctly preserve spaces in lines beginning with a space,
#                  handle DESCRIPTION
# 02g,15jul99,dgp  modify table handling to allow more readable layout of
#                  target.nr tables, correct SEE ALSO self-reference for libraries
# 02f,18mar99,fle  made it possible for refFormatControl able to sort elements
# 02e,09mar99,fle  fixed a bug with refSynFormat that was cuting some last
#                  characters
# 02d,01mar99,fle  allowed throw exception handling
# 02c,26feb99,fle  fixed a problem with .iP list end detection
# 02b,25feb99,fle  fixed a problem with TITLE markup
# 02a,22feb99,fle  fixed problem for C++ over loaded functions
# 01z,28jan99,fle  slightly changed the behaviour of refSeeAlso procdure
# 01y,18dec98,fle  made it able to support object oriented stuff
# 01x,09nov98,fle  suppressed the routine toplevel linkisation
# 01w,26oct98,fle  made SYNOPSIS and PARAMETERS fields have special processing
# 01v,20oct98,fle  fixed a trouble with embedded slashes in library names
# 01u,14oct98,fle  adapted to some of the C++ stuff
# 01t,01oct98,fle  added recognition of end of routine description while in
#                  the PARAMETERS section
# 01s,28sep98,fle  changed library name and description on title markup
#                  encouter
# 01r,07sep98,fle  removed refDocGen and sent it as refgen procedure in
#                    refgen.tcl
# 01q,18aug98,fle  added the IMPORT files
# 01p,22jul98,fle  modified LIBLINKS and RTNLINKS formats
# 01o,16jul98,rhp  permit inserting navigation bar within title,
#                  so navbar is visible after jump to rtn anchor
# 01n,15jul98,rhp  fixed anchor bug using 3rd arg to refTitleMkup
# 01m,15jul98,rhp  defined refIndexBuild
# 01l,14jul98,rhp  use rName rather than rNameRaw for formatted rtn titles;
# 01k,07jul98,rhp  tolerate unorthodox spacing of routine title lines
# 01j,07jul98,rhp  give error message with context on failure to parse
#                  subroutine titles
# 01i,29jun98,rhp  added --exbase option to override WIND_BASE when expanding;
#                  updated --out syntax for consistency w/other options;
#                  added \" as in-doc comment; improved refMangenInterpret;
#                  added --verbose option to refDocGen;
#                  saved link information, and proto-TOC, in auxiliary files
#                  in output directory;
#                  added `refDocLink' procedure to cross-link output across
#                  separate runs
# 01h,05jun98,fle  defaulted out put dir to . rather than / (bug I introduced)
# 01g,05jun98,fle  added a --out option to specify the output file location
# 01f,28may98,rhp  Correct spelling of sh2html in refDocGen, 
#                  and add pcl file recognition
# 01e,28may98,rhp  Accept library names that differ from filename spelling.
# 01d,20may98,rhp  Fixed consecutive-EXPAND bug in refDefnExpand
# 01c,19may98,rhp  Add --mg option to refDocGen; split refMangenCvt into
#                  refMangenCvt (outer), refMangenInterpret (inner)
# 01b,18may98,rhp  Add cleanup phase to mangen converter.
# 01a,15may98,rhp  Initial check-in.

# Record path to this file, in order to find siblings

set srcPath "[file dirname [info script]]/"

# Support argument comments in proc

source "${srcPath}procLib.tcl"

#
# DESCRIPTION
#
# This library contains all the ref procedures related to the regfgen utility.
#

#
# INTERNAL
# This Tcl source file contains the following procedures :
#
# refAppendList		- extract list of files to append
# refConfigParse	- parse configuration spec
# refDefnExpand		- interpret `EXPAND' keyword
# refEntrySuppress	- remove certain entries
# refErrorExit		- quit, with a simple message and a positive return code
# refExphdrFind		- locate and record a header file for `EXPAND'
# refExppathSave	- save `EXPANDPATH' information
# refFilesClose		- close list of files
# refFileSnarf		- return text of a file
# refFilesOpen		- open input and output files
# refFmtString		- apply specified list of substitutions to a string
# refFormatControl	- main documentation-extraction work procedure
# refIndexBuild		- convert raw list of headings into formatted index
# refLibGet		- extract library documentation from text
# refLibSeeAlso		- add lib reference to routine docn
# refListFormat		- format interior of item list or mark list
# refLnkForm		- build link targets from lib/routine list
# refMangenCvt		- save converted mangen output to new file
# refMangenInterpret	- preprocessing pass to convert from older mangen
#			  conventions
# refNameDecorate	- embellish name in argument according to language,
#			  format
# refParamAlign		- align parameters on less than 80 chars wide lines
# refParamLong		- handle long parameters of { type {} comment } format
# refParamShort		- align short parameters of {type {[name]} comment} format
# refParamsParse	- split the parameters list into list of three-item lists
# refPassageSuppress	- remove marked passages
# refRtnsFormat		- extract all routine documentation from text
# refSectSuppress	- remove sections with subhead INTERNAL
# refSpacePreserve	- insert fixed spaces
# refSpanFormat		- apply refgen-specific markup
# refSpecialFormat	- format a zone of text subject to special rules
# refSplitPair		- divide a pair into {type} and {name} portions
# refStringSplit	- extension of `split' to support escape
# refSynCFormat		- reformat a C synopsis to be compliant with WRS coding conventions
# refSynPclFormat	- reformat a protocol synopsis to be compliant with WRS coding 
#			- conventions
# refSynGen		- collect subroutine synopses
# refTabExpand		- convert tab characters to appropriate number of blanks
# refTableFormat	- convert refgen table syntax into output-language table
# refTargetMake		- produce link for name in library

################################################################################
#
# refErrorLog - logs a refgen error into a log
#
# SYNOPSIS
# \ss
#   refErrorLog <errorSrc> <errorMsg>
# \se
#
# PARAMETERS
#   errorSrc : command that lead to the error message
#   errorMsg : caught error message while calling <errorSrc>
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refErrorLog { errorSrc errorMsg } {

    global logFileName			;# location of the log file

    # find the error channel where to log

    if { $logFileName != "" } {
	set errChannel [open $logFileName a]
    } else {
	set errChannel stdout
    }

    # log the error message

    puts $errChannel "*** Error ***"
    puts $errChannel "$errorSrc\n$errorMsg\n"

    # close err channel if needed

    if { $errChannel != "stdout" } {
	close $errChannel
    }
}


###############################################################################
#
# refErrorExit - quit, with a simple message and a positive return code
#
# When not running interactively,
# output an error message <txt> to `stderr' and quit with a positive
# return code.  If a particular return status is desired, supply it as
# <code>.
#
# When running interactively, just a cover for Tcl `error' primitive.
#
# SYNOPSIS
# \ss
#   refErrorExit <txt> [<code>]
# \se
#
# PARAMETERS
#   txt : message for stderr
#   code : status code to return from process
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refErrorExit { txt {code 1} } {

    global ErrFileName

    if {[info exists ErrFileName]} {
	set errTag "refgen: in $ErrFileName"
    } else {
	set errTag "refgen"
    }
    puts stderr "($errTag)\n$txt\n"
    exit $code
}

###############################################################################
#
# refTabExpand - convert tab characters to appropriate number of blanks
#
# Convert tabs to spaces using the simple convention that tab stops
# appear at fixed intervals (by default, every 8 characters) per line.
#
# SYNOPSIS
# \ss
#   refTabExpand <txt> [<tabSize>] [<tab>] [<nl>]
# \se
#
# PARAMETERS
#   txt : text to modify
#   tabSize :  number of columns a tab covers
#   tab : tab character
#   nl : newline character
#
# RETURNS: Text similar in appearance to <txt>, but with all tabs replaced
# by one or more blanks.
#
# ERRORS: N/A
#

proc refTabExpand { txt {tabSize 8} {tab \t} {nl \n} } {

    foreach line [split $txt $nl] {
	set expLine {}
	
	set segList [split $line $tab]

	# Note last element of segList did not end in tab; thus,
	# we must exclude it from the expansion in the loop below.

	set lsl [llength $segList]
	foreach seg [lrange $segList 0 [expr $lsl - 2]] {
	    append seg { }  ;# tab is always worth at least one space
	    while {0 != [expr [string length $seg] % $tabSize]} {
		append seg { }
	    }
	    append expLine $seg
	}

	# But do include the text of the last segment in result!

	append expLine [lindex $segList [expr $lsl - 1]]

	lappend expTxt "$expLine"
    }    
    return [join $expTxt $nl]
}

###############################################################################
#
# refMangenCvt - save converted mangen output to new file
#
# Generate a new file named <outFile> suitable for formatting with
# refLib, based on the existing file <inFile>, intended for
# formatting with the first-generation WRS `mangen' tool.
# 
# The optional argument <lang> identifies the source language of the
# file to convert.  The remaining optional argument <how> defines the
# conversion specification; its default value, `OldMangen', is the
# only value available without writing a new conversion spec.
# 
# SYNOPSIS
# \ss
#     refMangenCvt <inFile> <outFile> [<lang>] [<how>]
# \se
#
# PARAMETERS
#   inFile : source file marked up for old mangen, w/embedded roff
#   outFile : converted source file
#   lang : source language (for defining meta-syntax tables)
#   how : conversion-spec filename without .tcl suffix
#
# EXAMPLE
# The following example converts a C file (C is the default source
# language) named sample.c, leaving the output in newSample.c:
# \cs
# refMangenCvt sample.c newSample.c
# \ce
# 
# RETURNS: N/A
# 
# SEE ALSO: `refMangenInterpret'
#

proc refMangenCvt { inFile outFile {lang C} {how OldMangen} } {

    global srcPath

    set fs [refFilesOpen $inFile $outFile]

    set sourceCode [refMangenInterpret [read [lindex $fs 0]] $lang $how]

    # get rid of any bullet lists that have been put in erroneously

    regsub -all "\\\\ibs" $sourceCode "\\ml" sourceCode
    regsub -all "\\\\ibi " $sourceCode "\\m -\n" sourceCode
    regsub -all "\\\\ibe" $sourceCode "\\me" sourceCode

    # Write outfile, and quit
    puts [lindex $fs 1] $sourceCode
    refFilesClose $fs
}

###############################################################################
#
# refMangenInterpret - preprocessing pass to convert from mangen conventions
#
# Convert documentation text that was intended for formatting with the
# first-generation WRS `mangen' tool into text suitable for formatting
# with refDocGen.
# 
# The optional argument <lang> identifies the source language of the
# file to convert.  The remaining optional argument <how> defines the
# conversion specification; its default value, `OldMangen', is the
# only value available without writing a new conversion spec.
# 
# SYNOPSIS
# \ss
#   refMangenInterpret <sourceCode> [<lang>] [<how>]
# \se
#
# PARAMETERS
#   sourceCode : source file text
#   lang : source language (for defining meta-syntax tables)
#   how : conversion-spec filename without .tcl suffix
#
# CONVERSION SPECIFICATION
# The file OldMangen.tcl is the only conversion spec supplied with refLib.
# In principle, other conversion specs could be defined.  The conversion spec 
# has three parts:
# \ml
# \m 1.
# A list of replacements to be performed, where each list element
# begins with the following two elements:
#   A regular expression to find.
#   A replacement specification for it.
#
# Additional sub-elements, if any, are ignored; they may be used for comments.
# 
# \m 2.
# An arbitrary Tcl conversion procedure named `refPrevIter' to
# perform any state-dependent (hence iterative) transformations.
# `refPrevIter' must take exactly one argument, the text to be
# converted, and returns the transformed text as its explicit result.
# 
# \m 3.
# A final-cleanup procedure named `refPrevCleanup' to do whatever
# final work is desirable in conversion.  This procedure too takes the
# text in process as its only argument, and returns an improved version
# of the same text as its result.
# \me
#
# RETURNS: Converted text.
#
# ERRORS: N/A
#

proc refMangenInterpret { sourceCode {lang C} {how OldMangen} } {

    global srcPath

    source "${srcPath}refIn$lang.tcl" ;# make SYNTAX array available 

    source "${srcPath}$how.tcl"      ;# defines ALTmg list and refPrevIter proc
                                     ;# see OldMangen.tcl

    set sourceCode [refPrevIter $sourceCode]
    set sourceCode [refFmtString $ALTmg $sourceCode] ;# MUST come second
    set sourceCode [refPrevCleanup $sourceCode]

    return $sourceCode
}

###############################################################################
#
# refConfigParse - parse configuration spec
#
# `refConfigParse' reads a simple configuration file that specifies
# the input and output formats for refgen, and perhaps other parameters.
# Its output is an executable Tcl string that, when run through `eval', 
# defines the tables (usually, by sourcing a file of table definitions) and
# whatever other parameters are specified in the configuration.
#
# By arbitrary fiat, the convention for naming refgen config files is
# to use no suffix.
#
# CONFIGURATION FILE FORMAT
# Comments are allowed, beginning with '#' and extending to newline.
# Blank (all-whitespace) lines are also allowed.
# Other lines are value pairs: two words per line, with the first word
# specifying a parameter name, and the second word its value.  "Words"
# in this context are words in the sense of the Tcl parser:
# non-whitespace strings, or strings enclosed in quotes, separated by
# whitespace.  Lines containing other than two words outside of
# comments constitute errors; configuration files containing errors
# are rejected.
#
# PARAMETER NAMES
# The following parameter names are supported.  Correctly-formed
# lines that specify unknown parameters are permitted, and ignored.
# Any recognized parameter must have a value in the range defined for
# that parameter.
# 
# `IN'
# Input language.  Recognized values: C Tcl
#
# `OUT'
# Output language.  Recognized value: Html
#
# `LIBIX'
# Library index specification: a quoted pair, specifying respectively
# the filename of a library index and the text label to refer to that file.
# For example: 
# \cs
# LIBS.html Libraries
# \ce
#
# `ROUTIX'
# Routine or procedure index specification: a quoted pair, specifying
# respectively the filename of a routine index and the text label to
# refer to that file.  For example:
# \cs
# PROCS.html Procedures
# \ce
# 
# SYNOPSIS
# \ss
#   refConfigParse fileName
# \se
#
# PARAMETERS
#   fileName : name of configuration file (including any suffix)
#
# RETURNS: executable Tcl string ready for eval.
#
# ERRORS: N/A
#

proc refConfigParse { fileName } {

    global srcPath

    if [catch {open "${srcPath}$fileName" r} conFid] {
	refErrorExit "Error: cannot open configuration file \"${srcPath}$fileName\""
    }
    set configSpec [read $conFid]
    close $conFid

    # Eliminate comments

    regsub -all "#\[^\n]*\n" $configSpec {} configSpec

    # Turn list of pairs into array; incidentally, this checks pairs syntax

    if [catch {array set config "$configSpec"}] {
	refErrorExit "cannot parse \"$fileName\" as configuration file."
    }

    set zz {}
    foreach spec [array names config] {
	switch $spec {
	    IN {
		# ignored here; must be last, hence is done explicitly below
	    }
	    OUT {
		append zz "source ${srcPath}refOut$config($spec).tcl\n"
	    }
	    LIBIX {
		append zz "set defLibIx \"[lindex $config($spec) 0]\"\n"
		append zz "set defLibLab \"[lindex $config($spec) 1]\"\n"
	    }
	    ROUTIX {
		append zz "set defRtnIx \"[lindex $config($spec) 0]\"\n"
		append zz "set defRtnLab \"[lindex $config($spec) 1]\"\n"
	    }
	    ROUTINES {
		set pr [expr {"no" != "$config($spec)"}]
		append zz "set prtRoutines $pr\n"
	    }
	    default {
		puts "Ignoring unknown parameter \"$spec\" in $fileName."
	    }
	}
    }
	
    # IN parameter must be read last, so it can benefit from other config

    append zz "source ${srcPath}refIn$config(IN).tcl\n"
    append zz "set inLang \"$config(IN)\"\n"  ;# caller needs inLang for links
    return $zz
}


###############################################################################
#
# refFilesOpen - open input and output files
#
# This routine opens <inFile> for reading and <outFile> for writing.  It discards
# any pre-existing contents of <outFile> by default; if append mode is
# desired for <outFile>, specify the mode in optional argument <outMode>.
#
# SYNOPSIS
# \ss
#   refFilesOpen inFile outFile [outMode]
# \se
#
# PARAMETERS
#  inFile : Opened for reading only
#  outFile : Opened for writing
#  outMode : Specify nondefault mode if append desired
#
# RETURNS: List of two file identifiers: INPUT OUTPUT
#
# ERRORS: N/A
#

proc refFilesOpen { inFile outFile {outMode {RDWR CREAT TRUNC}} } {

    if [catch {open $inFile RDONLY} inFid] {
	refErrorExit "cannot open input file \"$inFile\""
    }
    if [catch {open $outFile $outMode} outFid] {
	close $inFid
	refErrorExit "cannot open output file \"$outFile\""
    }

    return [list $inFid $outFid]
}

###############################################################################
#
# refFilesClose - close list of files
#
# Close a list of files, given their file IDs in the list <fileList>.
# For example, the result of `refFilesOpen' is a suitable argument for
# `refFilesClose'.
# 
# RETURNS: N/A
#

proc refFilesClose {fileList} {
    foreach fid $fileList {
	close $fid
    }
}


###############################################################################
#
# refLibGet - extract library documentation from text
#
# Given the text of a sourcefile after its title line as the argument
# <srcText>, locate and return the library description.
#
# SYNOPSIS
# \ss
#   refLibGet <srcText>
# \se
#
# PARAMETERS
#   srcText : string; source-file text, including lib description
#
# RETURNS: library documentation
#
# ERRORS: N/A
#

proc refLibGet { srcText } {

    global SYNTAX	;# meta-syntax table for source input language

    upvar inFile inF	;# input file name; needed only for error message

    # Library description extends from first instance of $SYNTAX(libtop)
    # (or, failing that, of $SYNTAX(libTopAlt))
    # to next instance thereafter of $SYNTAX(libEnd).
    # We must use regexp here rather than "string first" because the
    # values in the SYNTAX array are regular expressions.

    if {0 == [regexp -indices $SYNTAX(libTop) $srcText rz]} {

	# No libTop; try libTopAlt, and supply DESCRIPTION tag.

	if {0 == [regexp -indices $SYNTAX(libTopAlt) $srcText rz]} {

	    # No libTopAlt either; give up.

	    refErrorExit "Cannot find \"$inF\" library description."
	} else {
	    set lib "\nDESCRIPTION[string range $srcText [lindex $rz 1] end]"
	}
    } else {
	
	# standard libTop
	# may start with more than one tag--leave whatever is there.

	set lib [string range $srcText [lindex $rz 0] end]
    }

    regexp -indices $SYNTAX(libEnd) $lib rz

    set lib [string range $lib 0 [lindex $rz 0]]
    regsub -all $SYNTAX(libCn) $lib "\n" lib

    return $lib
}

################################################################################
#
# refClassNameGet - get the name of the current class
#
# This procedure returns the name of the current class. It checks that
# there is a class identifier in <txt> and returns its name.
#
# If there is no class name, then the "" is returned.
#
# SYNOPSIS
# \ss
#   refClassNameGet <txt>
# \se
#
# PARAMETERS:
#   txt : the text where to look for the class name
#
# RETURNS: The name of the current class.
#
# ERRORS: N/A
#

proc refClassNameGet { txt } {

    global SYNTAX		;# langage specific syntax array
    global curClass		;# current class

    if { ! [regexp -indices $SYNTAX(classStart) $txt classDecIx] } {
	return $curClass
    }
}

################################################################################
#
# refRtnTagGet - get the tag for a routine name
#
# This procedure uses <rtnName> to generate the name of the associated tag. It
# uses the SYNSEP global variable to generat that associated flag, replacing
# the SYNSEP(rtSep) tag with an underscore.
#
# If no SYNSEP(rtSep) tag is found, then the <rtnName> is used raw.
#
# SYNOPSIS
# \ss
#   refRtnTagGet <rtnName>
# \se
#
# PARAMETERS
#   rtnName : the name of the item to generate tag with
#
# RETURNS: The generated tag
#
# ERRORS: N/A
#

proc refRtnTagGet { rtnName } {

    global SYNSEP		;# separation marks array

    regsub -all $SYNSEP(rtSep) $rtnName "_" rtnName

    return $rtnName
}

################################################################################
#
# refRtnNumGet - get the routine number (for overloaded routines)
#
# This procedure just extracts the routine number from the given <rtnName>. It
# looks for an occurence of the SYNSEP(rtSep) pattern, and returns only the
# number just after that pattern.
#
# SYNOPSYS
#   refRtnNumGet <rtnName>
#
# PARAMETERS
#   rtnName : the routine name from which to extract routine number
#
# RETURNS: the routine number or 0 if no routine number found
#
# ERRORS: N/A
#
# EXAMPLE
#
# For an overloaded routine :
#
# \cs
#   refRtnNumGet {operator;new;1}
#   1
# \ce
#
# For a not overloaded routine :
#
# \cs
#   refRtnNumGet {semTake}
#   0
# \ce
#

proc refRtnNumGet { rtnName } {

    global SYNSEP		;# separation marks array

    set rtnNum 0
    set cutRtnName $rtnName

    if { [regexp $SYNSEP(rtSep) $rtnName] } {

	set splitRtnName [split $rtnName $SYNSEP(rtSep)]
	set lastElt [lindex $splitRtnName end]

	if { [scan $lastElt "%i" rtnNum] == 1 } {

	    if { $rtnNum != $lastElt } {

		# it was just a string beginning with a number, not what we were
		# looking for

		set rtnNum 0
		set cutRtnName $rtnName
	    }

	    set splitRtnName [lrange $splitRtnName 0 \
				     [expr [llength $splitRtnName] - 2]]
	    set cutRtnName [join $splitRtnName $SYNSEP(rtSep)]
	}
    }

    return $rtnNum
}

################################################################################
#
# refRtnNameGet - get the routine real name
#
# This procedure gets the routine real name of the given <rtnName> by only
# keeping characters until the first occurrence of a SYNSEP(rtSep).
#
# SYNOPSIS
# \ss
#   refRtnNameGet <rtnName>
# \se
#
# PARAMETERS
#   rtnName : name of the routine to get name from
#
# RETURNS: The real name of the routine
#
# ERRORS: N/A
#

proc refRtnNameGet { rtnName } {

    global SYNSEP		;# separation marks array

    regsub "$SYNSEP(rtSep)\[0-9]+" $rtnName {} rtnName

    return $rtnName
}

################################################################################
#
# refCPPFileProcess - process files for C++ parsing
#
# This procedure performs a preprocessing of the files as C++ files. It looks
# for the IMPLEMENTS tag to get the header file name and the implemented class
# name from each file of the given <fileList>.
#
# Each file implementing the same class is then saved in a temp file called
# <outDir>/<className>.i where <className> is the class name retrieved through
# the IMPLEMENTS tag.
#
# The list of those temp files is then returned, used for `refgen' processing,
# and then deleted.
#
# SYNOPSIS
# \ss
#   refCPPFileProcess <fileList> <outDir>
# \se
#
# PARAMETERS
#   fileList : the list of files to be processed a C++ files
#   outDir : the output directory where to write the temp files
#
# RETURNS: The list of created temp files.
#
# ERRORS: N/A
#

proc refCPPFileProcess { fileList outDir } {

    global SYNTAX		;# language specific syntax array
    global SYNOP		;# language specific synopsis syntax array

    array set fileClass {}	;# array of classes by file
    set classList {}		;# list of the available classes
    set tempFileList {}		;# list of the temp files created (to return)

    # first create the output directory if necessary

    if { ! [file exists $outDir] } {
	if { [catch "file mkdir $outDir" mkdirStatus] } {
	    refErrorLog "file mkdir $outDir" $mkdirStatus
	    return $tempFileList
	}
    }

    # verify that <outDir> is really a directory and not a file

    if { ! [file isdirectory $outDir] } {
	refErrorLog "refCPPFileProcess arguments error" \
		       "$outDir is not a directory"
    }

    foreach fileName $fileList {

	# get the file content and look for the IMPLEMENT keyword

	if { [catch "open $fileName r" fileId] } {
	    refErrorLog "open $fileName r" $fileId
	    continue
	}

	set fileContent [read $fileId]
	close $fileId

	# look for the IMPLEMENTS keyword, then store all the file contents in 
	# an array

	if { [regexp -indices "$SYNTAX(titlStart)\[ \t]+IMPLEMENTS" \
		     $fileContent implIx] } {

	    # isolate the IMPLEMENTS line.

	    set implements [string range $fileContent \
					 [expr [lindex $implIx 1] + 1] end]

	    set implements [string range $implements 0 \
					 [string first \n $implements]]

	    # get rid of the comment end markup

	    set commentEndMark [string first $SYNOP(cmtEnd) $implements]

	    if { $commentEndMark != -1 } {
		set implements [string range $implements 0 \
				       [expr $commentEndMark - 1]]
	    }

	    # just keep the header file name and the class name

	    set implements [string trim $implements]
	    set implLen [string length $implements]

	    # the class name is considered to be the las word of the line

	    set classNameStart [string wordstart $implements $implLen]
	    set className [string range $implements $classNameStart $implLen]

	    # the header file name should be everything but the last word

	    set classHeaderFile [string range $implements 0 \
					[expr $classNameStart - 1]]
	    set classHeaderFile [string trim $classHeaderFile]

	    # look if that class name already exists, and if not, create the
	    # and initialise the array cell for it

	    if { [lsearch $classList $className] == -1} {

		lappend classList $className

		# get the content of the header file specified by the
		# IMPLEMENTS keyword

		if { [catch "open $classHeaderFile r" fileId] } {
		    refErrorLog "open $classHeaderFile r" $fileId
		    continue
		}

		set headerFileContent [read $fileId]
		close $fileId

		# save the header file conntent as the first part of the
		# classFile cell content

		set classFile($className) $headerFileContent
	    }

	    # append the current file content to the appropriate table cell

	    append classFile($className) $fileContent
	}
    }

    # we now have the classFile array cotaining <n> classes and the files
    # contents that implements them. Lets save those contents in a temp file

    foreach className $classList {

	# create the generated file name

	set fileName [file join $outDir "$className.i"]
	if { [catch "open $fileName w" fileId] } {
	    refErrorLog "open $fileName w" $fileId
	    continue
	}

	lappend tempFileList $fileName

	# write the array cell content into the temp file

	set fileContent [lindex [array get classFile $className] 1]
	puts $fileId $fileContent

	close $fileId
    }

    return $tempFileList
}

###############################################################################
#
# refRtnsFormat - extract all routine documentation from the text
#
# Given the contents of a sourcefile as the string <srcText>, extract
# and format all the routine documentation blocks.
#
# Using the <rParent> paramater allows to name more precisely the link to the
# index file in the navigation bar of the routine description
#
# VARIANT: If <rGoal> is set to `SYNOPSIS', only routine titles and
# synopses are extracted.
# 
# SIDE EFFECT: The variable `rtnTitles' is set in the calling scope to
# the collection of all routine titles.
#
# SYNOPSYS
#   refRtnsFormat srcText rIndex rILabel rGoal
#
# PARAMETERS
#   srcText : contents of entire sourcefile (string)
#   rIndex : filename of rtn index (need not exist yet; for navigation)
#   rParent : parent container name
#   rILabel : human-readable label for routine index
#   rGoal : set to `SYNOPSIS' if calling to retrieve only synopsis info
#	    defaulted to `ALL'
#
# RETURNS: a routines list. Each routine element is composed of <routineName>,
#          <routineTitle> <routineDesc>
#
# ERRORS: N/A
#

proc refRtnsFormat { srcText rIndex rParent rILabel {rGoal ALL} } {

    # meta-syntax tables, in/out

    global SYNTAX
    global SYNOP		;# language specific synopsis array
    global SYNSUB		;# routine name substitution pattern
    global SYNSEP		;# syntax separator array
    global OUTstr		;# output format substitution patterns
    global INfmtSynop
    global INfmtParam
    global printInternal	;# do we have to get internal calls
    global srcPath		;# path to source file for error processing
    global inFile		;# file name for source
    global groupLabel           ;# string applying to following rtns

    set synNoMore [expr {"$rGoal" == "SYNOPSIS"}]

    if !$synNoMore {
	upvar rtnTitles rtt                ;# side-effect list of rtns
	upvar ttLibNm myLibName            ;# to fill in SEE ALSO ref
    }

    # Each routine description extends from an instance of the 
    # regular expression $SYNTAX(rtSt) to the subsequent instance of
    # the regular expression $SYNTAX(rtEn).  Note that the latter
    # must be defined to include synopsis source (e.g. declarations)
    # unless a SYNOPSIS is provided.

    array set rtnFound {}	;# array of found routines and synopses
    set rtnDesc {}		;# routines description
    set rtns {}			;# default, in case no rtns found
    set rtt {}

    while {[regexp -indices "$SYNTAX(rtSt)\n" $srcText rz]} {

	# look for a change in the group label

	if { [info exists SYNTAX(grpLabel)] } {
	    if { [regexp -indices $SYNTAX(grpLabel) $srcText gspot] } {
		if { [lindex $gspot 0] < [lindex $rz 0] } {
		    regexp $SYNTAX(grpLabel) $srcText {} groupLabel
		}
	    }
	}

	set srcText [string range $srcText [lindex $rz 0] end]

	# we have found a routine start pattern. Now look for the routine end

	if {0 == [regexp -indices $SYNTAX(rtEn) $srcText rz]} {
	    refErrorExit "Error: Cannot find end of entry after:\n\
		    [string range $srcText 0 320]... in $srcPath$inFile\n"
	}

	# now just keep the intersting routine description in nRtn, while
	# srcText contains the remaining of the text

	set zz [lindex $rz 1]
	set nRtn [refTabExpand [string range $srcText 0 $zz]]
	set srcText [string range $srcText [expr $zz + 1] end]

	# add the separation character to the authorized members of routines
	# names

	set rtNameWithSep [split $SYNTAX(rtNm) {}]
	set pattEndIndex [lsearch -exact $rtNameWithSep "]"]
	if { $pattEndIndex != -1 } {
	    set rtNameWithSep [linsert $rtNameWithSep $pattEndIndex \
				       $SYNSEP(rtSep)]
	    set rtNameWithSep [join $rtNameWithSep {}]
	} else {
	    set rtNameWithSep $SYNTAX(rtNm)
	}

	# stop here if there is a NOMANUAL section

	if { (! $printInternal) && ([regexp "NOMANUAL" $nRtn]) ||
	     (! [regexp \
		 "$SYNTAX(rtSt)\[^\n]*($SYNTAX(rtCn))+ *($rtNameWithSep) +-" \
		 $nRtn]) } {
	    continue
	}

	if { [regexp "$SYNTAX(rtCn)SYNOPSIS *\n" $nRtn] } {
	    set rSynop ""
	} elseif { $SYNOP(getSyn) && ($SYNTAX(langName) == \
		"C source file") } {
	    set rSynop [refSynCGet $srcText]
	} elseif { $SYNOP(getSyn) && ($SYNTAX(langName) == \
		"IDL source file") } {
	    set rSynop [refSynCGet $srcText]
	} elseif { $SYNOP(getSyn) && ($SYNTAX(langName) == \
		"protocol description file") } {
	    set rSynop [refSynPclGet $srcText]
	} else {
	    set rSynop ""
	}

	# Determine whether to print current routine

	set nRtn [refEntrySuppress "$nRtn\n$rSynop" $SYNTAX(rtCn) NO_LOCAL]
	if {0 == [string length $nRtn]} {
	    continue
	}

	# Locate and extract parts of title line

	if {0 == [regexp -indices \
         "$SYNTAX(rtSt)\[^\n]*($SYNTAX(rtCn))+ *($rtNameWithSep) +- +(\[^\n]*)"\
	  $nRtn iNext {} iRName iRTitle]} {
            refErrorExit "\nError: Cannot parse routine title near:\n\
		    [string range $nRtn [lindex $iRName 0] [lindex $iRName 1]]...\n \
		    in $srcPath$inFile"
        }

	set rNameRaw [eval "string range \$nRtn $iRName"]
	regsub $SYNSEP(rtSep) $rNameRaw $SYNSEP(rtSepSub) rNameRaw
	regsub {.*} $rNameRaw $SYNSUB(rtMk) rName

	# check to see if the routine is already defined (i.e. overloaded)

	set rtnNum 1
	set altRNameRaw $rNameRaw
	while { [info exists rtnFound($altRNameRaw)] } {
	    set altRNameRaw "$rNameRaw$SYNSEP(rtSep)$rtnNum"
	    incr rtnNum
	}

	set rNameRaw $altRNameRaw

        set rTitle [eval "string range \$nRtn $iRTitle"]

	if $synNoMore {
	    append rtns "$rNameRaw - $rTitle \n\n"
	} else {
	    append rtt " $rName - $rTitle \n"
	}

	# Begin formatted routine output using title line

	set fRtn "$OUTstr(SEP)"
	if {0 != [string length $rIndex]} {
	    if {0 == [string length $rILabel]} {
		set rILabel $rIndex
	    }
	    set navbar [refNavbarMkup $rIndex $rILabel "$rParent : "]
	} else {
	    set navbar {}
	}

	append fRtn [refTitleMkup $rName $rTitle [refRtnTagGet $rNameRaw] \
				  $navbar]

	# Discard title portion of raw routine desc

	set nRtn [string range $nRtn [expr 1 + [lindex $iNext 1]] end]

	if {0 != [string length $SYNTAX(declDelim)]} {

	    # Format declaration

	    regexp -indices "($SYNTAX(declDelim))\[^\n]*" $nRtn {} iNext

	    set rSynFmt [refFmtString $INfmtSynop $rSynop]  ;# input-lang dep

	    # Synopsis only, or full doc?

	    if $synNoMore {
		append rtns $rSynFmt
		append rtns $OUTstr(SYNdelim)
		continue
	    }

	    # add the synopsis or input/output to the beginning of the description

	    if { $rSynFmt != "" } {
		if { $SYNTAX(langName) == "protocol description file" } {
		    set sRtn [refSynPclFormat $rSynFmt]
		} else {
		    set sRtn "SYNOPSIS\n\\ss\n"
		    append sRtn $rSynFmt
		    append sRtn "\\se\n"
		}
	    } else {
		set sRtn ""
	    }

	} else {
      
	    # no synopsis to trim off, later; just end marker itself

	    set sRtn {}
	    set relg [string length $SYNTAX(rtEn)]
	    set iNext "[expr [string length $nRtn] - $relg] end"
	}

	# Optional PARAMETERS section, if language input specifies how to get 
	# and if at least some transformation applied (in case input not
	# recognized, e.g. if declaration violates WRS doc conventions).

	if {0 != [string length $INfmtParam]} {
	    set rParam [refFmtString $INfmtParam $rSynop]
	    if {$rParam != $rSynop} {
		append sRtn "\nPARAMETERS\n$rParam"
	    }
	}

	# Peel off comment-block continuation prefixes from rtn desc

	set nRtn [string range $nRtn 0 [lindex $iNext 0]]

	regsub -all $SYNTAX(rtCn) $nRtn "\n" nRtn

	# Various per-routine transformations

	set nRtn [refPassageSuppress $nRtn]
	set nRtn [refExppathSave $nRtn]     ;# updates global EPATHS
	set nRtn [refDefnExpand $nRtn]

	# Process files-to-append.  We do it one routine at a time,
	# so that routines from appended files can also APPEND more files.

	foreach appf [refAppendList nRtn] {
	    append srcText [refFileSnarf $appf]
	}

	# Default heading for routine description

	if {0 != [string length [set D $SYNTAX(rtDfltHd)]]} {
	    set nRtn [string trim $nRtn]   ;# Normalize to check default head
	    set Dend [expr [string length $D] - 1]
	    if { 0 == [regexp $D [string range $nRtn 0 $Dend]]} {
		set nRtn "DESCRIPTION\n$nRtn"
		set nRtn [refSynopsisFormat $nRtn]
	    }
	}

	set nRtn [refLibSeeAlso $myLibName $nRtn]    ;# mention parent lib
	set nRtn "$sRtn$nRtn"                        ;# automatic portions

	set nRtn [refParametersFormat "\n$nRtn"]

	append fRtn [refSpanFormat $nRtn]    ;# apply all refgen formatting
	append rtns $fRtn

	lappend rtnDesc [list $rNameRaw $rTitle $fRtn]
	set rtnFound($rNameRaw) [refSynopsisGet "\n$nRtn"]
    }

    return $rtnDesc
}

################################################################################
#
# refSynopsisGet - get the given routine synopsis
#
# This procedure extracts the synopsis form the given <txt> and returns it.
#
# SYNOPSIS
# \ss
#   refSynopsisGet <txt>
# \se
#
# PARAMETERS
#   txt : text from which to extract a synopsis
#
# RETURNS: The found synopsis or "" if no synopsis has been found.
#
# ERRORS: N/A
#

proc refSynopsisGet { txt } {

    set foundSyn ""

    if { [regexp -indices "\n\[ \t]*SYNOPSIS" $txt synIx] } {
	set txt [string range $txt [expr [lindex $synIx 1] + 1] end]
    } else {
	return $foundSyn
    }

    if { [regexp -indices "\n\[ \t\]*\[A-Z0-9]\[A-Z0-9]+" $txt synEndIx] } {

	set foundSyn [string range $txt 0 [lindex $synEndIx 0]]
	set foundSyn [string trim $foundSyn]

	# get rid of all the beginning \n

	while { [string first "\n" $foundSyn] == 0 } {
	    set foundSyn [string range $foundSyn 1 end]
	}

	# get rid of all the trailing \n

	set synLen [expr [string length $foundSyn] - 1]
	while { [string range $foundSyn $synLen $synLen] == "\n" } {
	    set foundSyn [string range $foundSyn 0 [expr $foundSyn - 1]]
	    set synLen [expr [string length $foundSyn] - 1]
	}
    }

    return $foundSyn
}

################################################################################
#
# refCommentIndexListGet - get a list of indices of the start and end of comments
#
# This procedure returns a list of the indices of the comments included in the
# given <text>. It looks for all the occurrences of <cmtSt>, associates
# its <cmtEnd>, and returns the complete list of indices.
#
# SYNOPSIS
# \ss
#   refCommentIndexListGet <text> <cmtSt> <cmtEnd>
# \se
#
# PARAMETERS
#   text : the text containing the list of comments to get
#   cmtSt : form of a comment start
#   cmtEnd : form of a comment end
#
# RETURNS: The list of comments indices, or an empty list if no comments are found.
#
# ERRORS: N/A
#

proc refCommentIndexListGet { text cmtSt cmtEnd } {

    set commentIxList {}
    set commentText $text
    set commentStartLen [string length $cmtSt]
    set commentEndLen [string length $cmtEnd]
    set commentStart [string first $cmtSt $commentText]
    set cutTextLen 0

    # for all the text, get the comments locations and store them into a list

    while { $commentStart != -1 } {

	# get the end of the comment mark

	set commentEnd [string first $cmtEnd $commentText]

	if { $commentEnd == -1 } {

	    # no end of comment after a comment start ... humm, lets continue

	    set commentText [string range $commentText \
				    [expr $commentStart + $commentStartLen + 1]\
				    end]

	    set cutTextLen [expr $cutTextLen + \
				 [expr $commentStart + $commentStartLen + 1]]

	    set commentStart [string first $cmtSt $commentText]
	    continue
	}

	# the end of comment has been found, add it to the commentIxList

	set commentStart [expr $commentStart + $cutTextLen]
	set commentEnd [expr $commentEnd + $commentEndLen + $cutTextLen]
	set thisCommentIx [list $commentStart $commentEnd]

	lappend commentIxList $thisCommentIx
        set commentText [string range $text $commentEnd end]
	set cutTextLen $commentEnd
	set commentStart [string first $cmtSt $commentText]
    }

    return $commentIxList
}


################################################################################
#
# refSynPclGet - get the first protocol routine synopsis and formats it
#
# This procedure extracts the first synopsis from the given <text>, which
# must start at the beginning of the synopsis as determined by a match with
# SYNTAX(synStart).
#
# Then the `refSynPclGet' procedure acts like a parser and looks for the end of
# the procedure declaration as an interpreter would. 
#
# SYNOPSIS
# \ss
#   refSynPclGet <text>
# \se
#
# PARAMETERS
#   text : the text containing the synopsis to get
#
# RETURNS: The found synopsis or an empty string if no synopsis is found.
#
# ERRORS: N/A
#

proc refSynPclGet { text } {

    global SYNOP                ;# language specific synopsis syntax array
    global SYNTAX               ;# language specific syntax array

    set foundSyn ""

    # look for the parameter start occurrence

    set paramStart [string first $SYNOP(paramSt) $text]
    if { $paramStart == -1 } {
        return $foundSyn
    }

    # limit the text to the next function declaration or to the end of the
    # text if it seems there is no other function declaration

    if { [regexp -indices $SYNTAX(rtSt) $text nextRtIx] } {
        set text [string range $text 0 [expr [lindex $nextRtIx 0] - 1]]
    }
    return $text
}


################################################################################
#
# refSynPclFormat - reformat a C synopsis to be compliant with WRS coding conventions
#
# This routine takes <syn> as the synopsis to be reformatted. The
# transformations applied to the synopsis extract the return type (OUTPUT) and
# the argument type (INPUT).
#
# EXAMPLE
# \ss
# (none) WDB_CONTEXT_CONT
#     (
#     WDB_CTX *
#     )
# 
#     {
#     ...
#     }
# \se
# becomes
# \ss
# INPUT: (none)
# OUTPUT: WDB_CTX
# \se
# SYNOPSIS
# \ss
#   refSynPclFormat <syn>
# \se
#
# PARAMETERS
#   syn : the synopsis to be reformatted
#
# RETURNS: The re-formatted synopsis.
#
# ERRORS: N/A
#

proc refSynPclFormat { syn } {

    global SYNOP                ;# language specific synopsis syntax array
    global SYNTAX               ;# language specific syntax array


    # the synopsis should have been extracted, format it and return it

    set synopsis [string trim $syn]
    set synopsis [split $synopsis "\n"]
    foreach line $synopsis {
        if { [regexp "^INPUT:" $line] } {
            append foundSyn "$line\n\\ss\n"
	} elseif { [regexp "^OUTPUT:\[\t ]*(\[_a-zA-Z0-9]+)" $line \
		{} typename] } {
	    append foundSyn "\\se\n\nOUTPUT:\n\\ss\n$typename\n\\se\n"
	} elseif { [regexp "^\[\t ]*(\[_a-zA-Z0-9]+)" $line {} typename] } {
	    append foundSyn "$typename\n"
	}
    }
    return $foundSyn
}


################################################################################
#
# refSynCGet - get the first C routine synopsis
#
# This procedure extracts the first synopsis from the given <text>, which
# must start at the beginning of the synopsis as determined by a match with 
# SYNTAX(synStart).
#
# Then the `refSynCGet' procedure acts like a parser and looks for the end of
# the procedure declaration as an interpreter would.
#
# SYNOPSIS
# \ss
#   refSynCGet <text>
# \se
#
# PARAMETERS
#   text : the text containing the synopsis to get
#
# RETURNS: The found synopsis or an empty string if no synopsis is found.
#
# ERRORS: N/A
#

proc refSynCGet { text } {

    global SYNOP		;# language specific synopsis syntax array
    global SYNTAX		;# language specific syntax array

    set foundSyn ""

    # look for the parameter start occurrence

    set paramStart [string first $SYNOP(paramSt) $text]
    if { $paramStart == -1 } {
	return $foundSyn
    }

    # limit the text to the next function declaration or to the end of the
    # text if it seems there is no other function declaration

    if { [regexp -indices $SYNTAX(rtSt) $text nextRtIx] } {
	set text [string range $text 0 [expr [lindex $nextRtIx 0] - 1]]
    }

    # now look for the related endParam mark. In order to avoid to encounter
    # that mark inside comments block, just don't care about anything inside
    # comments marks

    # first collect all the comments locations

    set commentIxList [refCommentIndexListGet $text $SYNOP(cmtSt) \
					      $SYNOP(cmtEnd)]

    # commentIxList contains the list of indexes of the comments inside the
    # synopsis, look for declaration end markup outside these comments

    # now get the list of all the paramStart and paramEnd occurrences indices

    set paramStartIxList [lsort -integer [refIxListGet $text $SYNOP(paramSt)]]
    set paramEndIxList [lsort -integer [refIxListGet $text $SYNOP(paramEnd)]]

    # create a list with both indices

    set bothIxList {}
    set firstStartParam [lindex $paramStartIxList 0]
    foreach paramStartIx $paramStartIxList {
	lappend bothIxList $paramStartIx
    }
    foreach paramEndIx $paramEndIxList {
	if { $paramEndIx >= $firstStartParam } {
	    lappend bothIxList $paramEndIx
	}
    }

    set bothIxList [lsort -integer $bothIxList]

    # now associate the first occurrence of a start parameter mark with its
    # associated end param mark

    set nStartParam 1
    set startParamIx [lindex $bothIxList 0]
    set endParamIx $startParamIx
    set nEndParam 0
    set bothIxList [lrange $bothIxList 1 end]

    while { ($nStartParam != $nEndParam) && ([llength $bothIxList] != 0) } {

	set paramIx [lindex $bothIxList 0]

	if { [lsearch $paramStartIxList $paramIx] != -1 } {

	    # this is a start param mark, increment the start param count

	    if { ! [refIsInComment $paramIx $commentIxList] } {
		incr nStartParam
	    }

	} else {

	    # this is an end param mark, increment the end param count

	    if { ! [refIsInComment $paramIx $commentIxList] } {
		incr nEndParam
		set endParamIx $paramIx
	    }
	}

	set bothIxList [lrange $bothIxList 1 end]
    }

    set foundSyn [string range $text 0 $endParamIx]

    # get the trailing text if necessary

    set trailText [string range $text [expr $endParamIx + 1] end]

    set trailSyn ""
    if { [regexp -indices $SYNOP(synEnd) $trailText synEndIx] } {

	set trailSyn [string range $trailText 0 [expr [lindex $synEndIx 0] - 1]]
    }

    # the synopsis should have been extracted, format it and return it

    set foundSyn [refSynCFormat $foundSyn $trailSyn]

    return $foundSyn
}


################################################################################
#
# refSynCFormat - reformat a C synopsis to be compliant with WRS coding conventions
#
# This routine takes <syn> as the synopsis to be reformatted. The
# transformations applied to the synopsis align all the types, names, and
# comments, leaving at least two spaces between names and comments. In the
# case of pointers to function calls, which also include the arguments for the
# called function, the entire argument is either fit into the same {type, name}
# range as simple arguments or is allowed to run into the comment area with its
# comment on a separate line and aligned with the other comments.
#
# EXAMPLES
# \ss
# END_OBJ * muxBind
#     (
#     char *         pName,                /* interface name, for example, ln, ei,... */
#     int            unit,                 /* unit number */
#     STATUS (*stShtdnRt) (void*, void*),  /* routine to shutdown the stack */
#     STATUS (*stTxRest) (void*, void*)    /* routine to tell stack it can transmit */
#     )
# \se
#
# \ss
# END_OBJ* muxBind
#     (
#     char *   pName,          /* interface name, for example, ln, ei,... */
#     int      unit,           /* unit number */
#     BOOL (*stackRcvRtn) (void*, long, M_BLK_ID, LL_HDR_INFO *, void*),
#                              /* receive function to be called. */
#     STATUS (*stackShutdownRtn) (void*, void*),
#                              /* routine to call to shutdown the stack */
#     void (*stackErrorRtn) (END_OBJ*, END_ERR*, void*),
#                              /* routine to call on an error. */
#     long     type,           /* protocol type from RFC1700 and many */
#                              /* other sources (for example, 0x800 is IP) */
#     char*    pProtoName,     /* string name for protocol  */
#     void*    pSpare,         /* per protocol spare pointer  */
#     STATUS (*stackTxRestartRtn) (void*, void*)
#                              /* routine to tell the stack it can transmit */
#     )
# \se
#
# The <trailSyn> variable specifies the synopsis trailing elements that should
# not be treated as synopsis elements, but that should appear in the synopsis
# (for example, the thrown exception).
#
# SYNOPSIS
# \ss
#   refSynCFormat <syn> [<trailSyn>]
# \se
#
# PARAMETERS
#   syn : the synopsis to be reformatted
#   trailSyn : trailing elements of synopsis
#
# RETURNS: The re-formatted synopsis.
#
# ERRORS: N/A
#

proc refSynCFormat { syn {trailSyn ""} } {

    global SYNOP                ;# language specific synopsis syntax array
    global SYNTAX               ;# language specific syntax array
    global INfmtSynop           ;# substitutions to apply to synopsis
    global groupLabel           ;# string applying to following rtns

    # see if this synopsis describes an exception

    if { [regexp $SYNOP(procExcept) $syn] } {

        if { $trailSyn != "" } {
            return "$syn $trailSyn"
        } else {
            return $syn
        }
    }

    # first get rid of all the synopsis formatting

    foreach fmtSynop $INfmtSynop {
        regsub -all [lindex $fmtSynop 0] $syn [lindex $fmtSynop 1] syn
    }

    # the following doesn't seem to be possible through $INfmtSynop
    # because $groupLabel must be substituted

    if { "$SYNTAX(langName)" == "IDL source file" } {
	regsub "^(\[^\\(\]*)\\(\n(.*)$" $syn \
		"\\1(\n    $groupLabel \*  pThis,\n\\2" syn
	regsub "^(\[^\\(\]*)\\( *(void|VOID)?\\)(.*)$" $syn \
		"\\1\n(\n$groupLabel \* pThis,\n)\n\\2" syn
    }

    # isolate start of parameters section

    set paramStartIx [string first $SYNOP(paramSt) $syn]
    if { $paramStartIx == -1 } {
        return $syn
    }

    # isolate the parameters from function declaration

    set synStart [string range $syn 0 [expr $paramStartIx - 1]]

    regsub -all "\n" [string trim $synStart] " " synStart
    regsub -all "\t" $synStart " " synStart
    regsub -all "\[ ]+" $synStart " " synStart

    # synStart is all right now. Let's embellish parameters

    set paramEnd [string last $SYNOP(paramEnd) $syn]
    if { $paramEnd == -1 } {
        return $syn
    }

    # isolate end of synopsis

    set synEnd $trailSyn
    regsub -all "\t" $synEnd " " synEnd
    regsub -all "\[ ]+" $synEnd " " synEnd

    # apply a bit of formatting now

    set parameters [string range $syn [expr $paramStartIx + 1] \
                           [expr $paramEnd - 1]]

    while { ([string range $parameters end end] == "\n") ||
            ([string range $parameters end end] == " ") } {

        set parameters [string range [string trim $parameters] 0 \
                               [expr [string length $parameters] - 2]]
    }

    # at this point, if we have an ANSI-type parameter which has none or
    # (void) arguments, we can reformat and return without going through
    # all the manipulation which is meaningless in these cases
    # we test, format, and return

    if { $parameters == "" } {
	set synopsis "$synStart $SYNOP(paramSt)$SYNOP(paramEnd)"
	return $synopsis
    } else {
	if { $parameters == "void" } {
	    set synopsis "$synStart $SYNOP(paramSt)$parameters$SYNOP(paramEnd)"
	    return $synopsis
	}
    }

    regsub -all $SYNOP(ptr) $parameters $SYNOP(ptrSub) parameters
    if { ($SYNOP(cmtSt) != "\n") && ($SYNOP(cmtEnd) != "\n") } {
        regsub -all "\n" $parameters " " parameters
    }
    regsub -all "\t" $parameters " " parameters
    regsub -all "\[ ]+" $parameters " " parameters

    # all the members should now be separated by a space, let's paste all the
    # multi-line comment together

    set parameters [refParamCommentOneLineSet $parameters]

    # the parameters list should now look more consistant; now divide it into
    # arguments and comments

    set parameters [refParamsParse $parameters]

    # paramList contains a list of parameters, with three members
    #   - first is the parameter type
    #   - second is the parameter name
    #   - last is the parameter comment
    #
    # The next few actions will try to align all the parameters types, names and
    # comments, in order to have a 80 chars wide line.

    set parameters [refParamAlign $parameters]

    set synopsis $synStart$parameters
    return $synopsis
}


################################################################################
#
# refParamsParse - split the parameters list into a list of three-item lists
#
# SYNOPSIS
# \ss
#   refParamsParse <list>
# \se
#
# PARAMETERS
#   list : list of arguments with no end-of-lines or white space
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refParamsParse { list } {

    global SYNOP                ;# language specific synopsis syntax array

    set openCom {}
    set paramList {}

    # divide before and after each comment

    regsub -all {/\*} $list "| /\*" openCom
    regsub -all {\*/} $openCom "\*/ |" paramList
    set paramList [string trim $paramList {|}]
    set paramList [split $paramList |]

    set curChar 0
    set newList {}
    set outList {}
    set last 0
    set ix1 0
    set parensNestLvl 0
    set x {}
    set blankElem [list $x $x $x]
    set item $blankElem

    # now take each element, test it, and write it to the appropriate subitem
    # in blankElem in order to create a three-item list element for formatting

    # first find out how many elements are in the list, and test each iteration
    # to see if we are working with the last element and if so, set the last flag

    set number [llength $paramList]

    foreach elem $paramList {
        set elem [string trim $elem]
        incr ix1
        if { $ix1 == $number } {
            set last 1
        }

    # if it is a comment, write it to the third subitem and write the whole
    # triplet to the output list

        if { [regexp {/\*} $elem] } {
            set item [lreplace $item 2 2 $elem]
            lappend outList $item
            set item $blankElem
            continue
        }

    # if it is not a comment, find out how many letters are in it and process
    # it letter by letter

        set length [string length $elem]
        set chars [split $elem {}]
        set newChars {}
        set ix2 0

        foreach curChar $chars {
            incr ix2

    # when the last letter is reached, process newChars to see if it is a
    # simple or complex object; write to the first and/or second subitem;
    # then see if there is another element; if not, write the triplet to the
    # output list

            if { $ix2 == $length } {

                append newChars $curChar
                if { [regexp {\(|\)} $newChars] } {
                    set item [lreplace $item 0 0 $newChars]
                } else {

                    # split the type and name into two parts using a
                    # function call and return a pair
                    set pair [refSplitPair $newChars]
                    set item [lreplace $item 0 1 [lindex $pair 0] [lindex $pair 1]]
                }
                if {$last == 1 } {
                    lappend outList $item
                }

            } else {

    # keep track of the nesting level of ()'s, and otherwise just look for commas
    # write each character to newChars

                switch $curChar {
                    (   {
                        set parensNestLvl [ expr {$parensNestLvl + 1} ]
                        append newChars $curChar
                    }
                    )   {
                        set parensNestLvl [ expr {$parensNestLvl - 1} ]
                        append newChars $curChar
                    }
                    , {

    # when there is a comma at nest level 0, process newChars to see if it is a
    # simple or complex object; write it to the first and/or second subitem;
    # then (because we know this is not the last letter and therefore we have
    # another argument not separated by a comment) write the triplet to the
    # output list, and zero out the template

                        if { ($parensNestLvl == 0) } {
                            append newChars $curChar
                            if { [regexp {\(|\)} $newChars] } {
                                set item [lreplace $item 0 0 $newChars]
                            } else {

                                set pair [refSplitPair $newChars]
                                set item [lreplace $item 0 1 \
                                        [lindex $pair 0] [lindex $pair 1]]
                                set newChars {}
                            }
                            lappend outList $item
                            set item $blankElem
                        } else {
                            append newChars $curChar
                        }
                    }
                    default {
                        append newChars $curChar
                    }
                }
            }
        }
    }

    return $outList
}


################################################################################
#
# refSplitPair - divide a pair into {type} and {name} portions
#
# This routine takes a string, separates the last word into the {name} element
# and puts the remaining characters into the {type} element.  It returns a two-
# element list.
#
# SYNOPSIS
# \ss
#   refSplitPair <characters>
# \se
#
# PARAMETERS
#   characters : character list ending with a comma
#
# RETURNS: A {type} {name} pair.
#
# ERRORS: N/A
#

proc refSplitPair { characters } {

    set type {}
    set name {}

    # get rid of the empty leading and trailing elements and divide into words

    set characters [string trim $characters]
    set splitParam [split $characters " "]

    while { [lindex $splitParam end] == {} } {
        set splitParam [lrange $splitParam 0 [expr [llength $splitParam] - 2]]
    }

    # keep everything but the last word in paramType;
    # move the last word to paramName

    set paramName [lindex [lrange $splitParam end end] 0]
    set paramName [string trim $paramName]
    set paramIx [string last $paramName $characters]
    set paramType [string range $splitParam 0 [ expr $paramIx - 1 ]]
    set paramType [string trim $paramType]
    lappend altList $paramType $paramName

    return $altList

}


################################################################################
#
# refParamAlign - align parameters on less than 80 chars wide lines
#
# This procedure takes the list of parameters (type, name, comment) from the given
# <paramList>, and tries to align everything as specified in the coding
# conventions.
#
# SYNOPSIS
# \ss
#   refParamAlign <paramList>
# \se
#
# PARAMETERS
#   paramList : the list of parameters to align
#
# RETURNS: A string including all the aligned parameters.
#
# ERRORS: N/A
#

proc refParamAlign { paramList } {

    global SYNOP                ;# language specific synopsis syntax array

    # gather information about types and names to know the string size
    # of each of them to align them

    set maxTypeLen 0
    set maxNameLen 0
    set maxCmpxLen 0
    set complex 0

    foreach param $paramList {

        if { [lindex $param 1] == "" } {
            set cmpxLen [string length [lindex $param 0]]
            if {$maxCmpxLen < $cmpxLen } {
                set maxCmpxLen $cmpxLen
            }
        } else {
            set typeLen [string length [lindex $param 0]]
            set nameLen [string length [lindex $param 1]]

            if { $maxTypeLen < $typeLen } {
                set maxTypeLen $typeLen
            }

            if { $maxNameLen < $nameLen } {
                set maxNameLen $nameLen
            }
        }
    }

    # calculate the column at which to align names (depends on type max len), and
    # the column at which to align comments (depends on name align column, and
    # name max length)

    set maxTypeLen [expr $maxTypeLen + [string length $SYNOP(paramIdt)]]
    set nameColumn [expr $maxTypeLen  + 1]
    set commentColumn [expr $nameColumn + $maxNameLen + 1]
    if { $commentColumn < 30 } {
        set commentColumn 30
    }
    if { $commentColumn > 50 } {
        set commentColumn 50
    }

    if { $maxCmpxLen > $commentColumn } {
        set complex 1
    }

    # see if we need to handle complex arguments and calculate the relationship
    # to nameColumn and commentColumn

    if { $complex == 0 } {
        set paramList [refParamShort $paramList $nameColumn $commentColumn]
        set paramList "$SYNOP(paramNl)$SYNOP(paramIdt)$SYNOP(paramSt)$SYNOP(paramNl)$paramList$SYNOP(paramIdt)$SYNOP(paramEnd)$SYNOP(paramNl)"

    } else {

        set paramList [refParamLong $paramList $nameColumn $commentColumn $maxCmpxLen]
        set paramList "$SYNOP(paramNl)$SYNOP(paramIdt)$SYNOP(paramSt)$SYNOP(paramNl)$paramList$SYNOP(paramIdt)$SYNOP(paramEnd)$SYNOP(paramNl)"
    }

    return $paramList
}


################################################################################
#
# refParamShort - align short parameters of {type {[name]} comment} format
#
# This procedure takes care of arguments that have the short argument format.
# It aligns the name and comment elements.
#
# If appending a comment to a line makes it more than 80 characters, then the
# comment is split into a multi-line comment. It is repeatedly split until there
# is no more-than-80 character line remaining.
#
# SYNOPSIS
# \ss
#   refParamShort <list> <nameCol> <commentCol>
# \se
#
# PARAMETERS
#   list          : the parameters to be formatted
#   nameCol       : the column where name should be aligned
#   commentCol    : the column where column should be aligned
#
# RETURNS: The formatted parameters.
#
# ERRORS: N/A
#

proc refParamShort { list nameCol commentCol } {

    global SYNOP

    set alignedParams ""
    foreach item $list {

        # append the parameter type and complete with tabs until nameCol

        # append the parameter type and complete with tabs until nameCol

        set paramLine "$SYNOP(paramIdt)[lindex $item 0]"
        set paramLine [refParamSpaceComplete $paramLine $nameCol]

        # append the parameter name and complete with tabs until commentCol

        append paramLine [lindex $item 1]

        if { [lindex $item 2] != {} } {

            set paramLine [refParamSpaceComplete $paramLine $commentCol]

            # append the parameter comment the pameter new line pattern
            #
            # If the comment makes tha line be too long (more than 80 characters
            # it will be splitted until the line is a less than 80 characters
            # line. If it is not possible, then the maximum will be done.

            set paramLine [refParamCommentAdd $paramLine [lindex $item 2]]
        }

        append paramLine "$SYNOP(paramNl)"
        append alignedParams $paramLine
    }

    return $alignedParams
}


################################################################################
#
# refParamLong -  handle long parameters of { type {} comment } format
#
# This procedure takes care of arguments that have the long argument format.
# It aligns comment elements with the comments of short-format arguments.
#
# If appending a comment to a line makes it more than 80 characters, then the
# comment is split into a multi-line comment. It is repeatedly split until there
# is no more-than-80 character line remaining.
#
# SYNOPSIS
# \ss
#   refParamLong <list> <nameCol> <commentCol> <maxCmpxLength>
# \se
#
# PARAMETERS
#   list          : the parameters to be formatted
#   nameCol       : the column where name should be aligned
#   commentCol    : the column where column should be aligned
#   maxCmpxLength : the length of the longest complex argument
#
# RETURNS: The formatted parameters.
#
# ERRORS: N/A
#

proc refParamLong { list nameCol commentCol maxCmpxLength } {

    global SYNOP

    set alignedParams ""
    foreach item $list {

        # find out if this is a simple or complex argument

        if { [lindex $item 1] != {} } {
            # if it's simple, append the parameter type and complete with
            # tabs until nameCol

            set paramLine "$SYNOP(paramIdt)[lindex $item 0]"
            set paramLine [refParamSpaceComplete $paramLine $nameCol]

            # append the parameter name and complete with tabs until commentCol

            append paramLine [lindex $item 1]

            if { [lindex $item 2] != {} } {

                set paramLine [refParamSpaceComplete $paramLine $commentCol]

                # append the parameter comment the pameter new line pattern
                #
                # If the comment makes tha line be too long (more than 80 characters
                # it will be split until the line is less than 80 characters.
                # If this is not possible, then the maximum will be done.

                set paramLine [refParamCommentAdd $paramLine [lindex $item 2]]
            }

        } else {

            # if it's complex, the comment needs to go on a
            # separate line; handle it appropriately

            set paramLine "$SYNOP(paramIdt)[lindex $item 0]"
            append paramLine [lindex $item 1]
            append paramLine "$SYNOP(paramNl)"
            set line {}
            set line [refParamSpaceComplete $line $commentCol]
            set line [refParamCommentAdd $line [lindex $item 2]]
            append paramLine $line
        }

            append paramLine "$SYNOP(paramNl)"
            append alignedParams $paramLine

    }

    return $alignedParams
}

################################################################################
#
# refParamCommentAdd - adds a parameter comment to the given parameter line
#
# This procedure adds the attached comment to a parameter line. It takes care of
# The maximum 80 characters wide line that synopses line should never reach.
#
# If appending a comment to a line makes it be more than 80 characters, then the
# comment is splitted into a parameter multi-line comment, and this until there
# is no more-than-80 caharcters line
#
# SYNOPSIS
# \ss
#   refParamCommentAdd <inputLine> <comment>
# \se
#
# PARAMETERS
#   inputLine : the line which to add parameter comment to
#   comment : the comment to add to the parameters line
#
# RETURNS: The <paramLine> appended with the comment (splitted if needed)
#
# ERRORS: N/A
#

proc refParamCommentAdd { inputLine comment } {

    global SYNOP		;# language specific synopsis syntax array

    set inputLineLen [string length $inputLine]
    set cmtMarkLen [string length "$SYNOP(cmtSt)  $SYNOP(cmtEnd)"]
    set commentLen [string length $comment]

    # if comment is short enough, just add it

    if { [expr $inputLineLen + $commentLen] <= 80 } {
	append inputLine $comment
	return $inputLine
    }

    # if the comment has to be split onto more than one line,
    # create the <inputLineLen> spaces line

    set spaces ""
    for {set ix 0} {$ix < $inputLineLen} {incr ix} {
	append spaces " "
    }

    set commentWords [split $comment " "]
    set thisWordLen 0
    set thisWord ""
    set prevWord ""

    # add words to the current comment line until it is over 80 characters long

    foreach word $commentWords {

	set prevWord $thisWord
	append thisWord "$word "
	set thisWordLen [string length $thisWord]

	if { [expr $inputLineLen + $cmtMarkLen + $thisWordLen] > 80 } {

	    if { $prevWord == "" } {

		# just one word inside comment makes it longer than 80 chars
		# let's append it anyway and continue

		append inputLine $thisWord
		set thisWord ""

		# do some resetting

		set prevWord ""

		if { $word != [lindex $commentWords end] } {

		    if { $SYNOP(cmtEnd) != $SYNOP(paramNl) } {
			append inputLine "$SYNOP(cmtEnd)$SYNOP(paramNl)$spaces"
		    } else {
			append inputLine "$SYNOP(cmtEnd)$spaces"
		    }
		    append inputLine "$SYNOP(cmtSt) "
		}


	    } else {

		# the previous comment was all right, but not this one. Go back
		# to previous comment

		set thisWord "$word "

		# do some resetting

		set prevWord ""

		if { $word != [lindex $commentWords end] } {

		    if { $SYNOP(cmtEnd) != $SYNOP(paramNl) } {
			append inputLine "$SYNOP(cmtEnd)$SYNOP(paramNl)$spaces"
		    } else {
			append inputLine "$SYNOP(cmtEnd)$spaces"
		    }

		    append inputLine "$SYNOP(cmtSt) $thisWord"
		}

	    }

	# append the word to the comment line only if it does not make it be
	# more than 80 characters a line

	} else {
	    append inputLine "$word "
	}
    }

    return $inputLine
}

################################################################################
#
# refParamSpaceComplete - complete a line with spaces
#
# This procedure adds spaces enough to the given <line> until it reaches the
# given <column>.
#
# SYNOPSIS
# \ss
#   refParamSpaceComplete <line> <column>
# \se
#
# PARAMETERS
#   line : the string to complete with tabs
#   column : the column that tabs must reach
#
# RETURNS: The completed line
#
# ERRORS: N/A
#

proc refParamSpaceComplete { line column } {

    set strLen [string length $line]

    # we now have the string length, add the needed tbas to it

    while { $strLen  < $column } {
	append line " "
	incr strLen
    }

    return $line
}


################################################################################
#
# refParamCommentOneLineSet - sets the multiline comment in one line comments
#
# This procedure takes all the multiline comments from the given <synStr>, and
# turns them into one line comments. This makes it possible to know what is/is not
# within a comment, and also to properly format comments that need to go on more
# than one line when the type, name, and comment are aligned.
#
# SYNOPSIS
# \ss
#   refParamCommentOneLineSet <synStr>
# \se
#
# PARAMETERS
#   synStr : the string that may have multiline comments in it
#
# RETURNS: the modified <synStr> with one line comments
#
# ERRORS: N/A
#

proc refParamCommentOneLineSet { synStr } {

    global SYNOP		;# language specific synopsis syntax array

    set cmtIxList [refCommentIndexListGet $synStr  $SYNOP(cmtSt) $SYNOP(cmtEnd)]
    set cmtIxListLen [llength $cmtIxList]
    set patternLen [string length "$SYNOP(cmtEnd) $SYNOP(cmtSt)"]

    # get a <n> spaces variable to replace multi-lines pattern with spaces

    set spaces ""
    for {set ix 0} {$ix < $patternLen} {incr ix} {
	append spaces " "
    }

    for {set ix 0} {$ix < [expr $cmtIxListLen - 1]} {incr ix} {
	set curCmtIx [lindex $cmtIxList $ix]
	set nextCmtIx [lindex $cmtIxList [expr $ix + 1]]

	# check if two comments indices are not separated just by a space

	if { [lindex $curCmtIx 1] == [expr [lindex $nextCmtIx 0] - 1] } {

	    # replace the multiline comment pattern with spces (to keep the same
	    # number of characters in string yet

	    set cmtEnd [expr [lindex $curCmtIx 1] - 1 - \
			     [string length $SYNOP(cmtEnd)]]

	    set cmtSt [expr [lindex $nextCmtIx 0] + \
			    [string length $SYNOP(cmtSt)]]

	    set strStart [string range $synStr 0 $cmtEnd]
	    set strEnd [string range $synStr $cmtSt end]
	    set synStr "$strStart$spaces$strEnd"
	}
    }

    # now get rid of the extra spaces we introduced

    regsub -all "\[ ]+" $synStr " " synStr
    return $synStr
}


################################################################################
#
# refIsInComment - looks if given <ix> is in a comment or not
#
# Given <ix>, this procedure looks into the <commentIxList> to see if <ix>
# is inside a comment or not.
#
# SYNOPSIS
# \ss
#   refIsInComment <ix> <commentIxList>
# \se
#
# PARAMETERS
#   ix : the index to look if in comment or not
#   commentIxList : the list of comment indices
#
# RETURNS: 1 if <ix> is within a comment, 0 else
#
# ERRORS: N/A
#

proc refIsInComment { ix commentIxList } {

    foreach commentIx $commentIxList {
	if { ($ix > [lindex $commentIx 0]) && ($ix < [lindex $commentIx 1]) } {
	    return 1
	}
    }

    return 0
}

################################################################################
#
# refIxListGet - gets the list of indices of <mark> in <text>
#
# SYNOPSIS
# \ss
#   refIxListGet <text> <mark>
# \se
#
# PARAMETERS
#   text : the text where to look for the <mark> markup
#   mark : the markup to look for in <text>
#
# RETURNS: The list of indices of <mark> in <text> or empty list if no <mark>
# were found
#
# ERRORS: N/A
#

proc refIxListGet { text mark } {

    # misc variables initialisation

    set ixList {}
    set tmpText $text
    set cutTextLen 0
    set markLen [string length $mark]
    set ix [string first $mark $tmpText]

    # look for all occurrences of <mark>

    while { $ix != -1 } {

	# append this <mark> occurrence to list of indices

	lappend ixList [expr $ix + $cutTextLen]

	# update the parsed text

	set tmpText [string range $tmpText [expr $ix + $markLen] end]
	set cutTextLen [expr $cutTextLen + $ix + $markLen]
	set ix [string first $mark $tmpText]
    }

    return $ixList
}

################################################################################
#
# refSeeAlsoFormat - processing for the See Also field
#
# This procedure processes all the SEE ALSO entries so that they can be linked
# bye htmlLink later.
#
# It takes every entry (entries should be separated by comas or by <CR>), and
# puts it between simple quotes (`...'). If the entry begins with a dot, it is
# not put between quotes, cause it should mean that there will be some markup
# processing on it.
#
# Also, if entries already contain simple quoted elements, they stay unchanged.
#
# If the SEE ALSO field contains parentheses, the link will be done later, they
# then stay unchanged.
#
# The italicized items are not changed, because it is assumed that the refer to
# a well known item that will be linked later.
#
# SYNOPSIS
# \ss
#     refSeeAlsoFormat <seeAlsoList>
# \se
#
# PARAMETERS
#   seeAlsoList : list of see also's as it appears in the source file
#
# RETURNS: the processed list of see alsos
#
# ERRORS : N/A
#
# EXAMPLE
# \cs
#   refSeeAlsoFormat "refLibSeeAlso, refLib, cRoutine(), `bolded', <italic>"
#   `refLibSeeAlso', `refLib', cRoutine(), `bolded', <italic>
# \ce
#
# SEE ALSO
# refLibSeeAlso, refLib
#

proc refSeeAlsoFormat { seeAlsoList } {

    set returnedSeeAlso ""

    # we have to only keep the members of the SEE ALSO field. Any beginning of
    # a new field after the SEE ALSO field should not be taken into account.

    if { [regexp -indices "\n\n\[-A-Z_0-9]+" $seeAlsoList ixes] } {
	set seeAlsoListOnly [string range $seeAlsoList 0 \
					  [expr [lindex $ixes 0] - 1]]

	set remainingText [string range $seeAlsoList [lindex $ixes 0] end]
    } else {
	set seeAlsoListOnly $seeAlsoList
	set remainingText ""
    }

    # first split all the SEE ALSOs by line (on \n character)

    set seeAlsoLine [split $seeAlsoListOnly \n]

    foreach line $seeAlsoLine {

	# now split each line on the coma character

	set seeAlsoComa [split $line ","]
	set seeAlsoProcessedComa {}

	foreach seeAlso $seeAlsoComa {

	    set seeAlso [string trim $seeAlso]

	    # first look if there is some markup inside or not. The string has
	    # been cleared from any appended or prepended spaces, so we can just
	    # check if the first character is a "." or not.
	    #
	    # don't do any processing for C routines that have parenthesis
	    # appended, they will be processed later.
	    #
	    # don't do any process for already quoted elements, and don't change
	    # italicised items. Elements that contain spaces won't be processed
	    # as far as routine or library names should not contain spaces, and
	    # the SEE ALSO field is to link to other libraries or routines.

	    if { $seeAlso != "" } {
		if { ([string first "." $seeAlso] == 0) ||
		      ([string first "( )" $seeAlso] != -1) ||
		      ([string first "()" $seeAlso] != -1) ||
		      ([regexp "<\[^>]*>.*" $seeAlso]) ||
		      ([regexp "\[ \t\]+" $seeAlso]) ||
		      ([regexp "\`\[^\']*\'.*" $seeAlso]) ||
		      ([regexp "\'\[^\']*\'.*" $seeAlso])} {

		    lappend seeAlsoProcessedComa $seeAlso

		} else {
		    lappend seeAlsoProcessedComa "`$seeAlso'"
		}
	    } else {
		lappend seeAlsoProcessedComa " "
	    }
	}

	# all the see alsos have been processed for that line. Append it to the
	# returned value, then process next line

	set processedLine [join $seeAlsoProcessedComa ", "]
	if { $processedLine != "" } {
	    append returnedSeeAlso "$processedLine\n"
	}
    }

    return "$returnedSeeAlso$remainingText"
}

###############################################################################
#
# refLibSeeAlso - add lib reference to routine docn
#
# Given an unformatted routine description block <rtnTxt>, augment it
# by adding a SEE ALSO reference to the containing library.  If no SEE
# ALSO section is present, add that heading as well.
#
# SYNOPSIS
# \ss
#   refLibSeeAlso libName rtnTxt
# \se
#
# PARAMETERS
#   libName : library name to insert as default SEE ALSO reference
#   rtnTxt : unformatted text of routine description
#
# RETURNS
#   Updated SEE ALSO rubrique
#
# ERRORS: N/A
#

proc refLibSeeAlso { libName rtnTxt } {

    if {[regexp -indices "\nSEE ALSO *(:|\n) *" $rtnTxt iSA]} {

	# keep only the SEE ALSO field in range

	set z [string range $rtnTxt 0 [expr [lindex $iSA 0] - 1]]

	set seeAlsoList [string range $rtnTxt [expr [lindex $iSA 1] + 1] end]

	# make SEE ALSO members linkables

	set seeAlsoList [refSeeAlsoFormat $seeAlsoList]

	# look if the library is already in it. If no, just add it

	if { ! [regexp "$libName\[\\n, \]" $seeAlsoList] } {
	    append z "\nSEE ALSO\n`$libName', "
	} else {
	    append z "\nSEE ALSO\n"
	}

	append z $seeAlsoList

    } else {

	set z "$rtnTxt\nSEE ALSO\n`$libName'\n"
    }

    return $z
}

###############################################################################
#
# refFmtString - apply specified list of substitutions to a string
#
# Given a documentation block <str>, apply the formatting specified by
# the list <fmtList>.  Each element of <fmtList> must itself be a
# list, as follows:
#  --First element: a regular expression specifying something to replace
#  --Second element: a corresponding replacement specification
#  --Remaining elements, if any, are ignored; they are handy places to
# put comments in the source for such lists.
#
# SYNOPSIS
# \ss
#   refFmtString <fmtList> <str>
# \se
#
# PARAMETERS
#   fmtList : list of lists - each element contains regexp pattern, subst
#   str : string to apply replacements in
#
# RETURNS: Formatted homogeneous block.
#
# ERRORS: N/A
#

proc refFmtString { fmtList str } {

    foreach pats $fmtList {
	set n [regsub -all [lindex $pats 0] $str [lindex $pats 1] str]
    }
    return $str
}


###############################################################################
#
# refSpacePreserve - insert fixed spaces
#
# Implements the convention of preserving spaces in lines
# that begin with at least one space.
#
# SYNOPSIS
# \ss
#   refSpacePreserve <sp> <str>
# \se
#
# PARAMETERS
#   sp : spelling of hard space in current output format
#   str : string that may contain indented lines
#
# RETURNS: Replacement for <str> that preserves required spaces.
#
# ERRORS: N/A
#

proc refSpacePreserve { sp str } {

    set newStr {}          ;# in case no hits at all

    while {[regexp -indices "\n +\[^\n]+" $str leadSp]} {
	append newStr [string range $str 0 [expr [lindex $leadSp 0] - 1]]
	regsub -all { } [eval "string range \$str $leadSp"] $sp spLine
	append newStr $spLine
	set str [string range $str [expr [lindex $leadSp 1] + 1] end]
    }
    return "$newStr$str"
}

###############################################################################
#
# refPassageSuppress - remove marked passages
#
# This procedure removes arbitrary passages from each instance of the 
# keyword `SUPPRESS' to the next instance of the keyword `ENDSUPPRESS'.
# Nesting is <not> supported.
# 
# Each keyword must occur on a line by itself; that is, preceded only
# by newline, and ending with (at most) whitespace before a 
# newline.
#
# The procedure is designed to be called before formatting.
#
# NOTE: Unlike other suppression routines, this routine <does not change
# its behavior if the> `printInternal' <flag is set>.  Use this keyword
# pair only for documentation corresponding to code that is commented out
# or ignored with #ifdef or the like.   For documentation of interest
# only to those with source access, use `NOMANUAL' or `INTERNAL' instead
# of `SUPPRESS'...`ENDSUPPRESS'.
#
# SUPPRESS
# This is a test of the keyword pair implemented
# by this procedure.
# ENDSUPPRESS
#
# SYNOPSIS
# \ss
#   refPassageSuppress <str>
# \se
#
# PARAMETERS
#   str : text that may be suppressed
#
# RETURNS: <str> with indicated passages omitted.
#
# ERRORS: N/A
#

proc refPassageSuppress { str } {

    while {1 == [regexp -indices "\n\[ \t]*SUPPRESS\[ \t]*\n" $str i]} {
	append zz [string range $str 0 [expr "[lindex $i 0] - 1"]]
	set str [string range $str [lindex $i 1] end]

	if { 0 == [regexp -indices \
		"\n\[ \t]*ENDSUPPRESS\[ \t]*\n" $str e]} {
	    set str ""
	} else {
	    set str [string range $str [lindex $e 1] end]
	}
    }
    append zz $str
    return $zz
}

###############################################################################
#
# refEntrySuppress - remove certain entries
#
# Unless printInternal is set to 1, this procedure returns the empty string
# whenever a substring of <str> matches either <prefix> followed by the
# letters "NOMANUAL" followed by whitespace and newline, or one of the 
# elements of the language-dependent list `LOCALdecls'.  (<prefix> usually
# contains newline as well, but that is up to the caller.)
#
# `addLocal' variable is to specify if the local declaration have to be
# suppressed or not. If set to `NO_LOCAL', then all the `local' declaration
# inside the <str> will be suppressed. If set to `KEEP_LOCAL', the will be kept.# Its default value is set to `NO_LOCAL'
#
# SYNOPSIS
# \ss
#   refEntrySuppress <str> <prefix> <addLocal>
# \se
#
# PARAMETERS
#   str : text that may be suppressed
#   prefix : prefix to ignore preceding suppression triggers
#   addLocal : specify if local declarations have to be suppressed
#
# RETURNS: Either <str> or the empty string.
#
# ERRORS: N/A
#

proc refEntrySuppress { str prefix {addLocal NO_LOCAL}} {

    global printInternal
    global LOCALdecls

    if { $addLocal == "NO_LOCAL" } {
	set pats $LOCALdecls
    } else {
	set pats {}
    }

    lappend pats "$prefix\[ \t]*NOMANUAL\[ \t]*\n"

    set kill 0
    set i 0
    set e [llength $pats]

    while {($kill == 0) && ($i < $e)} {
	set kill [regexp [lindex $pats $i] $str]
	incr i
    }

    if {$kill && !$printInternal} {
	return ""
    } else {
	regsub -all -- "$prefix\[ \t]*NOMANUAL\[ \t]*" $str "" str
	return $str
    }
}

###############################################################################
#
# refSectSuppress - remove sections with subhead INTERNAL
#
# Remove from the formatted text <str> any sections headed INTERNAL,
# unless the global switch `printInternal' is set to 1.
# Must be called after formatting, because subheads may be explicit
# or implicit; doing it after formatting allows both to be treated
# identically.
#
# SYNOPSIS
# \ss
#   refSectSuppress <str>
# \se
#
# PARAMETERS
#   str : text that may include INTERNAL sections
#
# RETURNS: Contents of <str> not headed INTERNAL, or <str> if
# `printInternal' set.
#
# ERRORS: N/A
#

proc refSectSuppress { str } {

    global OUTstr		;# output file string formatter
    global printInternal	;# do we have to print internal ?

    if {1 == "$printInternal"} {

	return $str

    } else {

	set p [string length $OUTstr(SUBHEAD)]

	while {-1 < [set i [string first "$OUTstr(SUBHEAD)INTERNAL" $str]]} {

	    append zz [string range $str 0 [expr "$i - 1"]]
	    set str [string range $str [expr "$i + $p"] end]

	    if { -1 == [set e [string first $OUTstr(SUBHEAD) $str]]} {
		set str ""
	    } else {
		set str [string range $str $e end]
	    }
	}
	append zz $str
	return $zz
    }
}

################################################################################
#
# refSpanFormat - apply refgen-specific markup
#
# The simpler parts of refgen-specific markup are implemented through
# the output-table mappings.  For example, the following escape sequences
# that represent a single character are implemented that way:
# \cs
# \< \> \& \\
# \ce
#
# Those replacements are performed by subordinate procedures.
#
# This procedure implements directly the more difficult special
# markup, such as `\\cs...\\ce' to demarcate examples where no
# formatting is permitted except the simple substitution of `/@...@/'
# to the C comment delimiters.
#
# SYNOPSIS
# \ss
#   refSpanFormat <txt>
# \se
#
# PARAMETERS
#   txt : text string containing documentation
#
# RETURNS: Formatted version of <txt>.
#
# ERRORS: N/A
#

proc refSpanFormat { txt } {

    global OUTfmt
    global OUTstr		;# from output-definition tables
    global refMarkSpans		;# refgen markup definitions
    global inFile		;# name of source file

    # Spanning-format sections; these include
    #  - literal sections: disable any apparent markup therein.
    #  - tables
    #  - lists

    # store in sss beginning marks (for marks composed of a beginning and an
    # ending mark)

    set		sss	"($refMarkSpans(pre))|"
    append	sss	"($refMarkSpans(stet))|"
    append	sss	"($refMarkSpans(smStet))|"
    append	sss	"($refMarkSpans(html))|"
    append	sss	"($refMarkSpans(tblStart))|"
    append	sss	"($refMarkSpans(iList))|"
    append	sss	"($refMarkSpans(indent))|"
    append	sss	"($refMarkSpans(rtnsDesc))|"
    append	sss	"($refMarkSpans(bList))|"
    append	sss	"($refMarkSpans(mList))"

    while { [regexp -indices $sss $txt iSpan] } {
	# leading text before span:

	set zf [string range $txt 0 [expr [lindex $iSpan 0] - 1]]
	set zf [refFmtString $OUTfmt $zf]		;# apply output format
	append zz [refSpacePreserve $OUTstr(FIXEDSP) $zf]	;# indents

	# span:

	set hit [eval "string range \$txt $iSpan"]
	set txt [string range $txt [expr [lindex $iSpan 1] + 1] end]

	# NOTE: because tables and lists may contain other spans,
	#       refTableFormat and refListFormat recurse back through
        #       this procedure.

	switch -regexp -- $hit \
		$refMarkSpans(pre) {
	            set f [refSpecialFormat $txt $refMarkSpans(preEnd) \
			    {refFmtString $OUTfmtCode} zi]
	            append zz "$OUTstr(CODE)$f$OUTstr(ENDCODE)"
	} \
		$refMarkSpans(stet) {
	            set f [refSpecialFormat $txt $refMarkSpans(stetEnd) \
			    {refFmtString [concat $OutCAt $OUTfmtDis]} zi]
	            append zz "$OUTstr(CODE)$f$OUTstr(ENDCODE)"
	} \
		$refMarkSpans(smStet) {
	            set f [refSpecialFormat $txt $refMarkSpans(smStetEnd) \
			    {refFmtString [concat $OutCAt $OUTfmtDis]} zi]
	            append zz "$OUTstr(SMCODE)$f$OUTstr(ENDSMCODE)"
	} \
		$refMarkSpans(html) {
	            set f [refSpecialFormat $txt $refMarkSpans(htmlEnd) \
			    {string trim} zi]
	            append zz "\n$f\n"
	} \
		$refMarkSpans(tblStart) {
	            set f [refSpecialFormat $txt $refMarkSpans(tblEnd) \
			    {refTableFormat} zi]
	            append zz "$OUTstr(TSTART)$f$OUTstr(TEND)"
	} \
		$refMarkSpans(iList) {
	            set f [refSpecialFormat $txt $refMarkSpans(iListEnd) \
			    {refListFormat item} zi]
	            append zz "$OUTstr(LIST)$f$OUTstr(ENDLIST)"
	} \
		$refMarkSpans(mList) {
	            set f [refSpecialFormat $txt $refMarkSpans(mListEnd) \
			    {refListFormat mark} zi]
	            append zz "$OUTstr(MLIST)$f$OUTstr(ENDMLIST)"
	} \
		$refMarkSpans(indent) {
	            set f [refSpecialFormat $txt $refMarkSpans(indentEnd) \
			    {refListFormat mark} zi]
	            append zz "$OUTstr(INDENT)$f$OUTstr(DEINDENT)"
	} \
		$refMarkSpans(rtnsDesc) {
	            set f [refSpecialFormat $txt $refMarkSpans(rtnsDescEnd) \
			    {refListFormat mark} zi]
	            append zz "$OUTstr(RTNSDESC)$f$OUTstr(RTNSDESCEND)"
	} \
		$refMarkSpans(bList) {
	            set f [refSpecialFormat $txt $refMarkSpans(bListEnd) \
			    {refListFormat bullet} zi]
	            append zz "$OUTstr(BLIST)$f$OUTstr(BLISTEND)"
	} \
                default {
	            refErrorExit "INTERNAL Error, processing\
			    \"...[string range $txt 0 30]... in\" $inFile"
	}

	set txt [string range $txt $zi end]	;# zi set by refSpecialFormat
    }


    # trailing fragment after last span (or whole thing if no spans)

    set zf [refFmtString $OUTfmt $txt]			;# apply output format
    append zz [refSpacePreserve $OUTstr(FIXEDSP) $zf]	;# indents

    # optionally suppress sections marked INTERNAL

    set zz [refSectSuppress $zz]

    # Done transforming text.

    return $zz
}

###############################################################################
#
# refSpecialFormat - format a zone of text subject to special rules
#
# This auxiliary procedure for `refSpanFormat' takes <txt>, a string 
# beginning at a span whose end is marked <se>.  It locates the endpoint,
# and records the location in its caller's variable named <eName>.
# It then formats the leading fragment of <txt> by evaluating the expression
# <fmtExpr> with the fragment added as a trailing argument, and surrounds it
# with the strings <os> and <oe>.
#
# SYNOPSIS
# \ss
#   refSpecialFormat <txt> <se> <fmtExpr> <eName>
# \se
#
# PARAMETERS
#   txt : string starting at beginning of special span
#   se : marker for span end
#   fmtExpr : start of formatting expression
#   eName : name of var to hold index of special-span end
#
# RETURNS: Formatted version of marked special span.
#
# ERRORS: N/A
#
# SEE ALSO: `refSpanFormat'
#

proc refSpecialFormat { txt se fmtExpr eName } {

    # Callers may need these globals:

    global OutCAt
    global OUTfmtCode
    global OUTfmtDis
    global inFile		;# source file name

    upvar $eName zi

    if {0 == [regexp -indices $se $txt zi]} {
	refErrorExit " no \"$se\" found after \"[string range $txt 0 30]...\"\
	    in $inFile"
    }
    set zf [string range $txt 0 [expr [lindex $zi 0] - 1]]
    set zf [eval [concat $fmtExpr {$zf}]]
    set zi [expr [lindex $zi 1] + 1]

    return $zf
}


###############################################################################
#
# refStringSplit - extension of `split' to support escape
#
# This procedure does the same thing as the Tcl `split' primitive, except
# that it honors an escape character; thus, the splitting char may be
# included in an element of the result list by prefacing it with the
# escape.  For example, the following line:
# \cs
# % refStringSplit {This|is|a\|test} | \\
# \ce
# produces a list that matches the following input:
# \cs
# {This} {is} {a|test}
# \ce
#
# SYNOPSIS
# \ss
#   refStringSplit <delim> [<delim> [<esc>]]
# \se
#
# PARAMETERS
#   str : string to split
#   delim : delimiter; where to split
#   esc : escape character
#
# RETURNS: A list of N+1 elements, where N is the number of un-escaped 
# <delim> characters in <str>.
#
# ERRORS: N/A
#

proc refStringSplit { str {delim { }} {esc \\} } {

    set zz ""; set ele ""
    set e [string length $str]

    for {set i 0} {$i < $e} {incr i} {
	set char [string index $str $i]
	switch -- $char \
	    "$esc"   {
	        set next [string index $str [expr 1 + $i]]
                if {"$delim" == "$next"} {
		    incr i
		    append ele $delim
		} else {
		    append ele $esc
		}
	    } \
	    "$delim" {
		lappend $zz $ele
		set ele {}
	    } \
	    default  {
		append ele $char
            }
    }
    lappend $zz $ele
}

###############################################################################
#
# refTableFormat - convert refgen table syntax into output-language table
#
# This routine formats a table from a refgen formatted file to the output format
#
# SYNOPSIS
# \ss
#   refTableFormat reftbl
# \se
#
# PARAMETERS
#   reftbl	table, marked up per refgen conventions
#
# RETURNS: output formatted table
#
# ERRORS: N/A
#

proc refTableFormat { reftbl } {

    global OUTstr
    global refMarkSpans
    global inFile

    # get places where to find column or line separators

    if [regexp -indices "$refMarkSpans(tblSplit)" $reftbl iSep] {
	set hEnd [expr [lindex $iSep 0] - 1]
	set bStart [expr [lindex $iSep 1] + 1]
    } else {
	set hEnd -1   ;# no heading; table is all body
	set bStart 0
    }

    set maxCells 0
    set tblEnd 0

    # take care about table head line

    set tblHead {}
    if { $hEnd != -1 } {

	foreach line [refStringSplit [string range $reftbl 0 $hEnd] "\n"] {

	    append tblHead "$OUTstr(TROWS)$OUTstr(THCELL)"
	    set cc [llength [set outCells [refStringSplit $line |]]]
	    set maxCells [expr {$maxCells>$cc ? $maxCells : $cc}]
	    set fCells {}

	    foreach cell $outCells {
		lappend fCells [refSpanFormat $cell]
	    }

	    set outLine [join $fCells "$OUTstr(THCELLE)$OUTstr(THCELL)"]
	    append tblHead "$outLine$OUTstr(TROWE)"
	}
    }

    # take care of the table body

    set tblBody {}

    set tableRows [refStringSplit [string range $reftbl $bStart end] "\n"]
    set nRows [expr [llength $tableRows] - 2]

    # if we're processing a target.nr or target.ref file, convert 
    # multiple cells to single cell
    ## remove this -- it breaks too much -- fixes have been made elsewhere

    #if {$inFile == "target.nr" || $inFile == "target.ref"} {
    #	regsub -all "\} \{ +\\|\[ +\\|\]+" $tableRows "" tableRows
    #}

    # now format cells

    foreach line $tableRows {

	if { ($line == [lindex $tableRows $nRows]) && \
	     ([regexp {^[-+|]*$} $line] > 0) && \
             ([string length $line] > 10) } {

	    set tblEnd 1

	} else {

	    append tblBody "$OUTstr(TROWS)$OUTstr(TCELL)"
	    set cc [llength [set outCells [refStringSplit $line |]]]
	    set maxCells [expr {$maxCells>$cc ? $maxCells : $cc}]
	    set fCells {}

	    foreach cell $outCells {
		lappend fCells [refSpanFormat $cell]
	    }

	    set outLine [join $fCells "$OUTstr(TCELLE)$OUTstr(TCELL)"]
	    append tblBody "$outLine$OUTstr(TROWE)"
	}
    }

    if {-1 != $hEnd} {
	set tbl $tblHead
	append tbl [refTblsepMkup $maxCells]
    }

    append tbl $tblBody  ;# recall append same as set if var unset

    if { $tblEnd } {
	append tbl [refTblsepMkup $maxCells]
    }

    append tbl $OUTstr(TROWE)
    return $tbl
}

###############################################################################
#
# refListFormat - format interior of item list or mark list
#
# This procedure translates \/i or \/m, within an item list or a mark list,
# into whatever is appropriate to begin an item for that kind of list
# in the current output language.
#
# SYNOPSIS
# \ss
#   refListFormat <w> <txt>
# \se
#
# PARAMETERS
#   w : what kind of list: `item' or `mark'
#   txt : list innards, marked up per refgen conventions
#
# RETURNS: <txt>, marked up per output-format conventions.
#
# ERRORS: N/A
#

proc refListFormat { w txt } {

    global OUTfmt		;# automatic decorations for ordinary text
    global OUTstr		;# output boilerplate table
    global refMarkSpans		;# array describes "spanning" markup
    global ErrFileName		;# name of the current file

    set ntxt {}
    while { [regexp -indices $refMarkSpans($w) $txt iHit iLab] } {

	# start markup has been found

	set ih [lindex $iHit 0]

	# if a markup has been found, only go to that markup and copy
	# already parsed text into ntxt

	if { $ih != 0 } {
	    append ntxt [refSpanFormat [string range $txt 0 [expr $ih - 1]]]
	}

	set b [eval "string range \$txt $iLab"]
	append ntxt "$OUTstr(PRE$w)[refFmtString $OUTfmt $b]$OUTstr(POST$w)"

	set txt [string range $txt [expr 1 + [lindex $iHit 1]] end]
    }

    if {0 != [string length $txt]} {
	append ntxt [refSpanFormat $txt]
    }

    return $ntxt
}

###############################################################################
#
# refSynGen - collect subroutine synopses
#
# Use this procedure to collect synopsis-only output from <inFile>, 
# appending it to synopsis-resource file <synFile>.
# Runs `refConfigParse' to set up configuration, optionally
# specified with the <fConfig> argument.
# 
# If the argument <internal> is set to the string "INTERNAL", sets
# `printInternal' so that all documentation available is formatted,
# even if it would normally be suppressed.
#
# NOTE: This routine is not supported for Sh, Tcl, or CTcl input
# (pending a specification of whether, and if so what, synopsis output
# is desired there).
#
# SYNOPSIS
# \ss
#   refSynGen <inFile> <synFile> [<fConfig> [<internal>]]
# \se
#
# PARAMETERS
#   inFile : Name of source file to extract synopses from
#   synFile : Name of output file to append synopses to.
#   fConfig : Name of doc-extraction config file.
#   internal : "INTERNAL" if internal-only docn wanted.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refSynGen { inFile synFile {fConfig {}} {internal {}} } {

    global LOCALdecls		;# local var declaractions
    global OUTstr		;# output strings array
    global printInternal	;# do we document this ?
    global SYNSUB		;# synopsis
    global SYNTAX		;# syntax array

    # Include local routines iff arg "internal" specified as text INTERNAL
    set printInternal [expr {"INTERNAL" == "$internal"}]

    # Set default values for all standard configurable parameters
    # Go through refConfigParse to avoid dual maintenance of 
    # defaults and C2html.

    eval [refConfigParse C2html]

    # Override defaults with user's choice if present
    # (may be only a partial override)
    if {0 != [string length "$fConfig"]} {
	eval [refConfigParse $fConfig]
    }

    set fids [refFilesOpen $inFile $synFile {RDWR CREAT}]

    # Grab the entire source file

    set sourceText [read [lindex $fids 0]]

    # Grab routine synopses

    set rtnSyns [refRtnsFormat $sourceText {} {} SYNOPSIS]

    foreach routineDesc $rtnSyns {
	puts [lindex $fids 1] [lindex $routineDesc 2]
    }


    # Done

    refFilesClose $fids
}

###############################################################################
#
# refAppendList - extract list of files to append
#
# The keyword `APPEND' directs `refgen' to consider the contents of other
# files as if they appeared at the end of the current file.  This procedure
# lists files indicated by `APPEND' and removes the original keyword.
#
# SYNOPSIS
# \ss
#   refAppendList <txtName>
# \se
#
# PARAMETERS
#   txtName : name of unformatted documentation block
#
# RETURNS: List of files to append, in explicit result; documentation block 
# with `APPEND' keywords removed, in caller variable named <txtName>.
#
# ERRORS: N/A
#

proc refAppendList { txtName } {

    upvar $txtName txt

    set flst {}

    while { [regexp "(.*)\nAPPEND (\[^\n]*)(\n.*)" $txt {} txt fn tmp] } {
	lappend flst $fn
	set txt "$txt$tmp"
    }
	
        
    return $flst
}

###############################################################################
#
# refExppathSave - save `EXPANDPATH' information
#
# The keyword `EXPANDPATH' directs `refgen' to add one or more
# paths to the list of directories to search when an `EXPAND' keyword
# is encountered.
#
# This procedure interprets `EXPANDPATH' keywords within the
# documentation block <txt>, and deletes the lines
# containing the keyword from that documentation block.
#
# Each path in a colon-delimited `EXPANDPATH' list is added to the
# global `EPATHS'.  Relative order is preserved, and new entries are
# added in front of existing `EPATHS' entries.  "Redundant" entries are 
# permitted, as they may change search order (front-first).
#
# NOTE: Paths that appear to be absolute are interpreted relative to
# $cEBase, which defaults to $WIND_BASE, but can be changed with the
# -exbase input option.  Relative paths are interpreted relative to the
# current working directory.  True absolute paths are not supported,
# because they cannot be portable from one installation to another.
#
# SYNOPSIS
#   refExppathSave <txt>
#
# PARAMETERS:
#   txt : unformatted documentation block
#
# RETURNS: Documentation block <txt> with `EXPANDPATH' keywords excised.
# 
# SEE ALSO: `refExphdrFind'

proc refExppathSave { txt } {

    global env
    global cEBase
    global EPATHS

    set base $cEBase
    set tps {}          ;# in case no hits at all
    while {1 == \
	    [regexp "(.*)\nEXPANDPATH (\[^\n]*)(\n.*)" $txt {} txt pl t]} {
    	set txt "$txt$t"
	set nps [split $pl :]

	# because regexp does longest-match-first, this loop actually 
	# captures EXPANDPATH entries in inverse order of appearance.

	set tps [concat $tps $nps]
    }

    set rps {}
    foreach d $tps {
	set d [string trim $d]
	switch -- [string index $d 0] {
	    /       {lappend rps "$base$d"}
	    \\      {lappend rps "$base$d"}
	    default {lappend rps $d}
	}
    }

    set EPATHS [concat $rps $EPATHS]
    if { [llength $EPATHS] == 0 } {
	set EPATHS [list {.}]
    }
    return $txt
}

###############################################################################
#
# refDefnExpand - interpret `EXPAND' keyword
# 
# The keyword `EXPAND' takes two arguments: a header-file name,
# and the name of a definition to extract from it.
# `refDefnExpand' seeks the header file relative to a list of paths
# taken from two sources: 
#  First, any paths specified on the `refgen' command line
# with the `--expath=' option;
#  Second, the collection of paths specified so far in the file being
# processed with the `EXPANDPATH' keyword.
# 
# This routine searches a documentation passage for instances of `EXPAND'.
# For each instance, it parses both arguments, parcels out the resulting work,
# and replaces the line containing the `EXPAND' keyword with the definition
# extracted from the header file.  `EXPAND' output is automatically
# formatted as a code example; there is no need to surround the keyword
# line with markup to request code formatting.
#
# NOTE: The subordinate procedure `refInHExp' requires knowledge
# of the input language, and as such is defined in an input-language
# dependent file (for example, refInC.tcl).
#
# SYNOPSIS
# \ss
#   refDefnExpand <txt>
# \se
#
# PARAMETERS
#   txt : documentation passage to search for `EXPAND'
#
# ERRORS: If the required definition cannot be found, `refDefnExpand' emits
# a warning message to the standard-error output stream, and leaves the
# line containing the EXPAND keyword in the output, preceded by the string
# "(failed)".
#
# RETURNS: <txt> with instances of `EXPAND' replaced by the requested
# definitions.
# 
# SEE ALSO: `refDocGen', `refExphdrFind', `refExppathSave', 
# as well as `refInHExp' definitions in `refIn*.tcl' files.

proc refDefnExpand { txt } {

    global EPATHS
    global EXPdict
    global CEPATHS
    global ErrFileName
    global inFile		;# source file name
    global srcPath		;# path to source file

    # the following should allow more flexibility with spaces in the
    # EXPAND keyword

    while {[regexp "(.*)\nEXPAND\[\t ]+(\[^\n\t ]+)\[\t ]+(\[^\n\t ]+)\[\t ]*(\n.*)" \
                 $txt {} txt hn dn t] } {

    	if {([info exists hn]) && ([info exists dn])} {

	    set deftext [refExphdrFind $hn $dn]
	    set txt "$txt\n\\cs\n$deftext\\ce\n$t"

	} else {

	    # unrecognized EXPAND syntax; one or another arg missing

	    set mft [expr [string length $txt] - 50]
	    refErrorExit "Error: Cannot parse EXPAND (missing arg?) after \n\
		    \t...[string range $txt $mft end] in $srcPath$inFile\n"
	}
    }
    return $txt
}

###############################################################################
#
# refExphdrFind - locate type definition for `EXPAND'
#
# The keyword `EXPAND' takes two arguments: a header-file name (without
# path information), and a type name to extract from it.
#
# This routine handles both arguments for a single `EXPAND': it searches
# the directories listed in the global `EPATHS' for the first header file
# named <hfile> that contains a definition for the type name.
# 
# NOTE
# For speed, the text of any headers found is retained in case it is
# needed later.
# 
# FIXME
# Add command-line override to `EPATHS'.
#
# SYNOPSIS
# \ss
#   refExphdrFind <hfile> <dname>
# \se
#
# PARAMETERS:
#   hfile : name of header file to find
#   dname : name of type definition to find
#
# RETURNS: text of type definition
# 
# SEE ALSO: `refExppathSave'

proc refExphdrFind { hfile dname} {

    global cEBase
    global CEPATHS
    global EPATHS
    global EXPdict
    global ErrFileName

    set pathList [concat $CEPATHS $EPATHS]
    set firstChar [string range $hfile 0 0]

    if { ("$firstChar" == "/") || ("$firstChar" == "\\") } {
	set hfile [string range $hfile 1 end]
	set pathList [list $cEBase]
    }

    set defin {}
    foreach d $pathList {

	if { [file exists "$d/$hfile"] } {

	    if { ! [info exists EXPdict("$d/$hfile")] } {

		# new hit

		set EXPdict("$d/$hfile") [refFileSnarf "$d/$hfile"]

	    }

	    set defin [refInHExp $EXPdict("$d/$hfile") $dname]
	    if { [info exists defin] && [string length $defin] > 0 } {
		break
	    }
	}
    }

    if { (! [info exists defin]) || [string length $defin] == 0 } {
	set defin "\n(failed) EXPAND $hfile $dname\n"
	puts stderr "(refgen: in $ErrFileName) \n\
			    \tWARNING: failed EXPAND $hfile $dname\n"
    }

    return $defin
}

###############################################################################
#
# refFileSnarf - return text of a file
#
# Open file <fName>, read its entire text, and close it.
#
# SYNOPSIS
# \ss
#   refFileSnarf <fName>
# \se
#
# PARAMETERS
#   fName : file name
#
# RETURNS: Text of file <fName>.
#
# ERRORS: N/A
#

proc refFileSnarf { fName } {

    if [catch {open $fName RDONLY} fId] {
	refErrorExit "cannot open file \"$fName\" to append its contents"
    }

    set text [read $fId]

    close $fId
    return $text
}

###############################################################################
#
# refFormatControl - main documentation-extraction work procedure
#
# Given the name of a source file <inFile>, create an output file, 
# then write formatted documentation there.  The output filename is
# constructed as the basename of <inFile> with a suffix dependent
# on the output format.  For example, if generating HTML output from 
# comments in foo.c, the output file is foo.html.
#
# The optional arguments are for the benefit of online documentation
# formats; they are used only to write links to a library-oriented
# table of contents (<parentFile>, <parentLabel>) and to a
# routine-oriented index (<rtnIndex>, <rtnILabel>).  Neither of these
# files need exist at the time `refFormatControl' runs.
#
# SYNOPSIS
# \ss
#   refFormatControl inFile [<outDir> [<parentFile> [<parentLabel> [<rtnIndex>
#                    [<rtnILabel> [<category>] [sortRoutines]]]]]]
# \se
#
# PARAMETERS
#   inFile : name of source file to extract docn from
#   outDir : output file directory
#   parentFile : name of "parent" file
#   parentLabel : human-readable label for "parent"
#   rtnIndex : name of subroutine index (need not exist)
#   rtnILabel : human-readable label for subroutine index
#   category : the document category
#   sortRoutines : says if routines will be alphaetically sorted or not
#
# RETURNS: List of names documented; first the output filename, then the
# library name as spelled in the file itself, then names
# of all routines for which documentation was extracted.
#
# ERRORS: N/A
#

proc refFormatControl { inFile {outDir {.}} {parentFile {}} {parentLabel {}} \
			{rtnIndex {}} {rtnILabel {}} {bookName ""} \
			{chapterName} {category ""} {sortRoutines 0} } {

    # global variables

    global EPATHS		;# support globals for EXPAND
    global EXPdict
    global SYNTAX		;# meta-syntax tables, in/out
    global OUTstr 		;# string formatters for output
    global OUTfmt
    global OUTsuffix
    global needMangenCvt	;# FLAG: source markup is mangen
    global SYNSUB		;# routine names synopsis substitution pattern 

    set EPATHS {}		;# in-file EXPAND path specs
    array set EXPdict {}	;# headerfiles for EXPAND
    upvar prtRoutines prtRoutines	;# flag: print routines by dflt?

    set rz {}			;# accum result here

    # create output directory if needed

    if { ! [file isdirectory $outDir] } {
	if { [catch "file mkdir $outDir" mkdirStatus] } {
	    refErrorExit "Cannot create directory $outDir :\n\n$mkdirStatus"
	}
    }

    # set output file name

    set baseName [file rootname [file tail $inFile]]
    set outFile "$baseName.$OUTsuffix"
    set outPath "$outDir/$outFile"
    set fids [refFilesOpen $inFile $outPath]

    # get title line as the first line of file

    gets [lindex $fids 0] ttLine

    if { {} != $SYNTAX(preTitle) } {
	while {[regexp $SYNTAX(preTitle) $ttLine]} {   ;# not at title line yet
	    gets [lindex $fids 0] ttLine               ;# try again
	}
    }

    # if first line is not a title line, try again with next line

    while { ! [regexp \
	      "$SYNTAX(titlStart) *(\[^ \.]*)\.\[^ ]* *- *(.*)$SYNTAX(titlEnd)"\
	      $ttLine {} ttLibNm ttDesc] && (! [eof [lindex $fids 0]]) } {

	gets [lindex $fids 0] ttLine
    }

    # If the above fails to define ttLibNm, we have the wrong syntax table

    if {![info exists ttLibNm]} {
	refErrorExit "Cannot parse \"$inFile\" as a $SYNTAX(langName)."
    }


    # redefine output name if title line says "realname/filename"
    # (useful in BSP docn)

    if {[regexp "^(\[^/]*)/(\[^/]*$)" $ttLibNm {} ttLibNew ttLibOrg]} {

	set ttLibNm $ttLibNew
	set baseName $ttLibNew
	close [lindex $fids 1]
	file delete -force $outPath
	set outFile "$ttLibNew.$OUTsuffix"
	set outPath "$outDir/$outFile"
	set fids [refFilesOpen $inFile $outPath] 

    } elseif {"$ttLibNm" != "$baseName"} {

	puts stderr "(refgen: in $inFile) \n\
		     \tWARNING: basename is \"$baseName\" but\
		     name in title is \"$ttLibNm\"\n"
    }
    
    # result record of output file is used to cross-link other
    # doc; thus we don't want $outDir in it, which is the path from sourcedir
    # but we do want ./ as placeholder for navigation,
    # to replace with fuller relative path in links from other subdirs

    lappend rz "./$outFile"

    # Grab the entire remainder of source file

    set sourceText [read [lindex $fids 0]]
    if { (1 == $needMangenCvt) || [regexp "\.nr" $inFile] } {
	upvar inLang inLang
	set sourceText [refMangenInterpret $sourceText $inLang]
    }

    # look for TITLE markup to replace library description and title

    set titlePattern "TITLE (\[^\n]*)"
    set titlePattern "\\\\$titlePattern"

    if { [regexp  -indices $titlePattern $sourceText ixes] } {

	set altLibDesc [string range $sourceText [expr [lindex $ixes 0] + 7] \
						 [lindex $ixes 1]]

	regsub -all "/" [lindex $altLibDesc 0] "-" ttLibNm 
	set ttLibNm [string range $altLibDesc 0 \
				  [expr [string first " - " $altLibDesc] - 1]]
	set ttDesc [string range $altLibDesc \
				 [expr [string first " - " $altLibDesc] + 3] \
				 end]
    }

    # Top-of-file boilerplate

    if { $category != "" } {
	set fileTitle "$ttLibNm \[$category\]"
    } else {
	set fileTitle $ttLibNm
    }

    puts [lindex $fids 1] "[refStartMkup $inFile $outPath $fileTitle]\
			   $OUTstr(SEP)"

    if {0 != [string length $parentFile]} {	
	if {0 == [string length $parentLabel]} {
	    set parentLabel $parentFile
	}
	set navbar [refNavbarMkup $parentFile $chapterName "$bookName : "]
    } else {
	set navbar {}
    }

    # begin writing in doc file

    puts [lindex $fids 1] [refTitleMkup $fileTitle $ttDesc top $navbar]
    lappend rz [list $ttLibNm $ttDesc]

    # Copy of source text for possible reference from EXPAND

    set EXPdict($inFile) [list {} $sourceText]

    # Raw material for library description

    set libDesc [refTabExpand [refLibGet $sourceText]]
    set libDesc [refPassageSuppress $libDesc]
    set libDesc [refExppathSave $libDesc]   ;# updates global EPATHS

    foreach appf [refAppendList libDesc] {
	append sourceText [refFileSnarf $appf]
    }

    set libDesc [refSynopsisFormat $libDesc]
    set libDesc [refParametersFormat $libDesc]

    # Suppress lib description if marked NOMANUAL and no override

    set libDesc [refEntrySuppress $libDesc {} KEEP_LOCAL]

    if {0 != [string length $libDesc]} {

	# Now do EXPAND macros.  No point if lib was suppressed!

	set libDesc [refDefnExpand $libDesc]
    }

    # Deal with routines

    if { $prtRoutines && \
	 ( ! [regexp "\nNOROUTINES\[ \]*\n" $libDesc]) } {

	# Format docn for routines, store in "rtnDesc"
	# NOTE: as side effect, this call leaves titles in "rtnTitles"

	set rtnDesc [refRtnsFormat $sourceText $rtnIndex $chapterName \
				   $rtnILabel]
	set sourceText {}   ;# Don't need full source any more; free its memory

	# prepare library synopsis

	set libSyn "[string toupper $rtnILabel]\n\n"
	set rtnList {}

	# sort routines if needed

	if { $sortRoutines } {
	    set rtnDesc [lsort -index 0 -dictionary $rtnDesc]
	}

	foreach foundRoutine $rtnDesc {

	    # stash copy of routine title lines in proto-TOC

	    lappend rtnList [list [lindex $foundRoutine 0] \
				  [lindex $foundRoutine 1]]

	    # Add routine titles to library description, if we're printing it

	    regsub {.*} [lindex $foundRoutine 0] $SYNSUB(rtMk) rName
	    append libSyn "$rName  -  [lindex $foundRoutine 1]\\nl\n"
	}

	# format it to the output format

	lappend rz $rtnList
	set libDesc "[refSpanFormat $libSyn$libDesc]"

    } else {
	if {0 != [string length $libDesc]} {
	    set libDesc [refSpanFormat $libDesc]
	} 
	set rtnDesc {}
    }

    # Write library description and routine descriptions

    puts [lindex $fids 1] $libDesc
    foreach foundRoutine $rtnDesc {
	puts [lindex $fids 1] [lindex $foundRoutine 2]
    }

    # End-of-file boilerplate

    puts [lindex $fids 1] $OUTstr(STOP)
    refFilesClose $fids
    return $rz
}

###############################################################################
#
# refLnkForm - build link targets from lib/routine list
#
# SYNOPSIS
# \ss
#   refLnkForm <lib> <lraw> <lNum>
# \se
#
# PARAMETERS
#   lib : filename, including output suffix
#   lraw : simple list of routine names
#   lNum : list of numbers for overloded routines
#
# RETURNS: Qualified list of routines in library 
#
# ERRORS: N/A
#

proc refLnkForm { lib lraw lNum } {

    global OUTlMk
    global OUTsuffix
    upvar inLang lng

    set lcooked {}

    set ix 0

    foreach nm $lraw {

	# get the nm associated number for refTargetMake

	set nmNum [lindex $lNum $ix]

	lappend lcooked [list [refNameDecorate ${lng}Rx $nm] \
			      [refTargetMake ${lng} $lib $nm {}]]

	incr ix
    }

    return $lcooked
}

###############################################################################
#
# refNameDecorate - embellish name in argument according to language, format
#
# SYNOPSIS
# \ss
#     refNameDecorate <lng> <name>
# \se
#
# PARAMETERS
#   lng : input language name
#   name : name to decorate
#
# RETURNS: embellished name
#
# ERRORS: N/A
#

proc refNameDecorate { lng name } {

    global OUTlMk

    return "$OUTlMk(${lng}NmSt)$name$OUTlMk(${lng}NmEn)"
}

###############################################################################
#
# refTargetMake - produce link for name in library
#
# This procedure uses the <lng> laguage specific markup to generate the link
# to the <lib> library.
#
# The <linkType> parameter allows to specify the kind of link that shoud be
# generated. The allowed values are `top' or `self'. While in `top' mode, the
# generated llink is not of type <lib>#<lib>, but <lib>#top.
#
# SYNOPSIS
# \ss
#   refTargetMake <lng> <lib> <nm> [<linkType>]
# \se
#
# PARAMETERS
#   lng : input language name
#   lib : name of library
#   nm : name of link target
#   linkType : the kind of link that has to be generated
#
# RETURNS: embellished name
#
# ERRORS: N/A
#
# EXAMPLE
#
# For a self <linkType>
#
# \cs
#   refTargetMake C ./fooLib.html foo self
#   <b><i><a href="./fooLib.html#foo_1">foo</a></i>(\&nbsp;)</b>
# \ce
#
# For a top <linkType>
#
# \cs
#   refTargetMake C ./fooLib.html foo top
#   <b><i><a href="./fooLib.html#top">foo</a></i>(\&nbsp;)</b>
# \ce
#

proc refTargetMake { lng lib nm {linkType "self"} } {

    global OUTlMk

    set linkNum [refRtnNumGet $nm]
    set nm [refRtnNameGet $nm]

    # care if the link is a self or a top link

    if { $linkType != "top" } {
	set link "$OUTlMk(lnSt)$lib$OUTlMk(lnAnch)$nm"
    } else {
	set link "$OUTlMk(lnSt)$lib$OUTlMk(lnAnch)top"
    }

    # append a link number if necessary

    if { $linkNum != 0 } {
	append link "_$linkNum"
    }

    append link "$OUTlMk(lnLab)$nm$OUTlMk(lnEnd)"
    set link [refNameDecorate ${lng}Sb $link]

    return $link
}

###############################################################################
#
# refIndexBuild - convert raw list of headings into formatted index
#
# As a side effect, `refDocGen' leaves in its output directory several
# files of global information extracted from the files whose documentation
# it formats.
#
# This routine is designed to use as its input two of those files:
# \ml
# \m `LIB'
# A list of the overall titles of the generated documentation.  For C and
# Tcl source, these are library titles; for Sh source, they are simply
# script names; other source types may have other meanings for the overall
# documented unit.
#
# \m `RTN'
# A list of the individual subroutine or procedure (for C and Tcl, 
# respectively) names with entries in the generated documentation.
# \me
# 
# In either case, `refIndexBuild' generates a formatted, alphabetized
# list of the titles; the output file is an "index" in the sense that, if
# links are inserted by including the resulting file in a `htmlLink'
# run, it is useful as an entry point for navigation.  The output
# file is left in the current working directory.
# 
# UPLINKS
# The output file assumes its "parent" is a file whose pathname is
# specified as `$OUTstr(GENPARENT)' in the output specification.
# For example, for HTML output, the parent is assumed to be a file
# called `index.html' in the same directory as the output file.
# 
# ALPHABETIC GROUPS
# `refIndexBuild' separates the alphabetized index entries into groups
# that start with the same letter, and inserts an alphabetic heading
# before each such group.  "Start" here means the first character
# after an optional prefix; see the discussion of the <pfx> argument
# below.
# 
# ARGUMENTS
# The input file name is identified by the argument <tocfile>.  The output
# file name is not given explicitly; instead, it is taken from the 
# configuration data in the <fConfig> argument.  
# 
# The <lr> argument specifies which of the two cases (library or routine
# index) is being handled; it must be the literal text `Lib' or the literal
# text `Rtn'.   The only effect of this argument is to select from the
# two pairs of index filename and label defined in the configuration file.
# 
# The <fConfig> argument must be one of the configuration files
# accepted by `refDocGen'; the point of extracting the output filename
# from that file is to ensure consistency with the up-links inserted
# by `refDocGen' in its output.  As with `refDocGen', the
# configuration file also specifies the output format (for example,
# HTML).  If no value is provided for <fConfig>, `C2html' is assumed.
# <You must specify other configurations by name> even if you did
# not do so for `refDocGen'.  `refDocGen' can often deduce an
# appropriate configuration from the file extension of the source file,
# but `refIndexBuild' has no way of making a similar deduction.
# 
# Finally, the <pfx> argument is for cases in which all entries in
# the file begin with a common prefix (such as "i2o").  The alphabetic grouping
# performed by `refIndexBuild' would not be interesting in such cases
# if only the first character could be used for grouping.  If you specify
# a value for <pfx> of length N, the (N+1)th character is used for alphabetic
# grouping instead of the first.  Thus, for example, if you specify `i2o'
# as the value of <pfx>, `refIndexBuild' looks for alphabetic groups
# by examining the fourth character of each name.
# 
# NOTE: Although <pfx> is used only for its length, it is important
# that this facility be used <only> when all entries do in fact have
# the same prefix; otherwise the sort order will not produce reasonable
# results for grouping at the (N+1)th character.  If the entries do not
# in fact all begin with the same prefix, leave <pfx> empty.
#
#
# SYNOPSIS
# \ss
#   refIndexBuild <tocfile> [<lr> [<fConfig> [<pfx>]]]
# \se
#
# PARAMETERS
#   tocfile : Name of file containing raw list
#   lr : Must be "Lib" (default) or "Rtn"
#   fConfig : Name of doc-extraction config file
#   pfx : Constant prefix for all entries, if any
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refIndexBuild { tocfile {lr {Lib}} {fConfig {}} {pfx {}} } {

    global OUTfmt

    # Config tells us what to call the output file (def${lr}Ix)
    # as well as what its title should be (def${lr}Lab)

    # Go through refConfigParse to avoid dual maintenance of 
    # defaults and C2html.

    eval [refConfigParse C2html]

    # Override defaults with user's choice if present
    # (may be only a partial override)
    if {0 != [string length "$fConfig"]} {
	eval [refConfigParse $fConfig]
    }

    # Get TOC data

    set toc [refFileSnarf $tocfile]

    # TOC may contain redundant entries if users ran formatter repeatedly;
    # therefore, sort and uniquify, and while we're at it 
    # also insert initial-letter headings

    set toc [split $toc \n]
    set e [llength [set toc [lsort $toc]]]
    set last [lindex $toc [set i 0]]
    set letterHead [string index [string trim $last] [string length $pfx]]

    lappend gToc "\n[string toupper $letterHead]\n"
    lappend gToc $last
    while {[incr i] < $e} {
	if {"$last" != "[lindex $toc $i]"} {
	    set last [lindex $toc $i]
	    set lh [string index [string trim $last] [string length $pfx]]
	    if {"$letterHead" != "$lh"} {
		set letterHead $lh
		lappend gToc "\n[string toupper $letterHead]\n"
	    }
	    lappend gToc $last
	}
    }
    set toc [join $gToc "\n "]

    if [catch {open [set "def${lr}Ix"] {RDWR CREAT TRUNC}} outFid] {
	refErrorExit "cannot open output file \"[set def${lr}Ix]\""
    }

    # Top-of-file boilerplate

    puts $outFid "[refStartMkup $tocfile \
	    [set "def${lr}Ix"] [set "def${lr}Lab"]] $OUTstr(SEP)"

    puts $outFid [refNavbarMkup $OUTstr(GENPARENT) "up" ""]

    set nmTag [set "def${lr}Lab"]

    puts $outFid [refTitleMkup $nmTag {} $nmTag {}]

    # Format and append TOC 

    set toc [refFmtString $OUTfmt $toc]
    puts $outFid $toc

    # End-of-file boilerplate

    puts $outFid $OUTstr(STOP)
    close $outFid
}

################################################################################
#
# refSynopsisFormat - re-format the SYNOPSIS section of the given text
#
# This procedure reformats the SYNOPSIS section of the given <txt>. It looks for
# the occurrence of the SYNOPSIS keyword in the given <txt>, and considers
# everything as part of the synopsis until it encounters a "\\se" tag.
#
# It then sticks the SYNOPSIS section after the NAME section (that should always
# be there), wherever it may have found the SYNOPSIS section in the given <txt>.
#
# SYNOPSIS
# \ss
#   refSynopsisFormat <txt>
# \se
#
# PARAMETERS
#   txt : the text in which to look for the SYNOPSIS section
#
# RETURNS : the modified <txt>
#
# ERRORS: N/A
#

proc refSynopsisFormat { txt } {

    global SYNTAX		;# language-related help syntax

    if { [regexp -indices "\nSYNOPSIS\[ \t\]*\n" $txt ixes] } {

	set firstPart [string range $txt 0 [expr [lindex $ixes 0] - 1]]
	set synopsis [string range $txt [expr [lindex $ixes 1] + 1] end]

	if { [regexp -indices "\n\\\\se\n" $synopsis ixes] } {
	    set secondPart [string range $synopsis [lindex $ixes 1] end]
	    set synopsis [string range $synopsis 0 [lindex $ixes 1]]
	    return "SYNOPSIS\n$synopsis\n$firstPart\n$secondPart\n"
	} else {
	    return "SYNOPSIS\n$synopsis\n$firstPart\n"
	}
    }

    return $txt
}

################################################################################
#
# refParametersFormat - re-format the PARAMETERS section of the given text
#
# This procedure reformats the PARAMETERS section of the given <txt>. It looks
# for the occurrence of the PARAMETERS keyword in the given <txt>, and considers
# everything as part of the parameters until it encounters an uppercase word or
# a title markup.
#
# SYNOPSIS
# \ss
#   refParametersFormat <txt>
# \se
#
# PARAMETERS
#   txt : the text in which to look for the PARAMETERS section
#
# RETURNS : the modified <txt>
#
# ERRORS: N/A
#

proc refParametersFormat { txt } {

    global SYNTAX			;# language related syntax

    if { [regexp -indices "\nPARAMETERS?\[ \t\]*(:|-)?\[ \t\]*\n" \
		 $txt paramHeaderIndex] } {

	# a PARAMETERS pattern has been found, keep text from here to the
	# end

	set firstPart [string range $txt 0 [expr [lindex $paramHeaderIndex 0] \
						 - 1]]

	set rtnParameters "PARAMETERS\n\\ts\n"
	set paramList [string range $txt [lindex $paramHeaderIndex 1] end]

	if { [regexp -indices "\n\[A-Z\]+\[ \t]*:?\[^\n]*\n" \
		      $paramList paramsIx] } {

	    # keep it as the parameters descriptions until next section
	    # assuming next section is in comment and is in upper case
	    # format

	    set secondPart [string range $paramList [lindex $paramsIx 0] end]

	    set paramList [string range $paramList 0 [lindex $paramsIx 0]]

#	    regsub -all -- $SYNTAX(libCn) $paramList "\n" paramList
	    set paramList [split $paramList \n]
	    set nParams [llength $paramList]

	    # parameters retrieving loop

	    for {set ix 0} {$ix < $nParams} {incr ix} {

		set line [string trim [lindex $paramList $ix]]
		if { $line == {} } {
		    continue
		}

		# check if it is a new parameter or just remaining description
		# of previous parameter

		if { [regexp "(.*) ?: (.*)" $line {} paramName paramDesc] } {

		    set curParamName [string trim $paramName]
		    set curParamDesc [string trim $paramDesc]

		    # look for next line content

		    set nextLineNum [expr $ix + 1]
		    while { $nextLineNum < $nParams } {

			set nextLine [string trim [lindex $paramList \
							  $nextLineNum]]
			if { [regexp "(.*) ?: (.*)" $nextLine] } {

			    # next line starts a new parameter description, just
			    # go out of the loop

			    set nextLineNum $nParams
			} else {

			    # this is the remaining part of a parameter
			    # description begun on the above line, append it to
			    # the current parameter description

			    append curParamDesc " $nextLine"
			    incr nextLineNum
			    incr ix
			}
		    }

		    append rtnParameters "`$curParamName' | - | $curParamDesc\n"
		}
	    }

	} else {
	    return $txt
	}

	append rtnParameters "\\te\n"
    } else {
	return $txt
    }

    return "$firstPart\n$rtnParameters\n$secondPart"
}

#############################################################################
#
# nameFileDoc - proc which gives the doc file generated name 
#
# this routine is a copy of the first part of the routine refFormatControl
#
# SYNOPSIS
#   nameFileDoc inFile [<outDir> [<ptrentFile> [<parentLabel> [<rtnIndex>
#                    [<rtnILabel> [<category>] [sortRoutines]]]]]]
#
# PARAMETERS
#   inFile : name of source file to extract docn from
#   outDir : output file directory
#   parentFile : name of "parent" file
#   parentLabel : human-readable label for "parent"
#   rtnIndex : name of subroutine index (need not exist)
#   rtnILabel : human-readable label for subroutine index
#   category : the document category
#   sortRoutines : says if routines will be alphaetically sorted or not
#
# RETURNS: full path name of intended output file
#
# ERRORS: N/A
#
#

proc nameFileDoc { inFile {outDir {.}} {parentFile {}} {parentLabel {}} \
                        {rtnIndex {}} {rtnILabel {}} {bookName ""} \
                        {chapterName} {category ""} {sortRoutines 0} } {

    # global variables

    global SYNTAX               ;# meta-syntax tables, in/out
    global OUTsuffix
    # set output file name

    set baseName [file rootname [file tail $inFile]]
    set outFile "$baseName.$OUTsuffix"
    set outPath "$outDir/$outFile"
    set fileId [open $inFile RDONLY]

    # get title line as the first line of file

    gets $fileId ttLine

    if { {} != $SYNTAX(preTitle) } {
        while {[regexp $SYNTAX(preTitle) $ttLine]} {   ;# not at title line yet
            gets $fileId ttLine                        ;# try again
        }
    }

    # if first line is not a title line, try again with next line

    while { ! [regexp \
              "$SYNTAX(titlStart) *(\[^ \.]*)\.\[^ ]* *- *(.*)$SYNTAX(titlEnd)"\
              $ttLine {} ttLibNm ttDesc] && (! [eof [lindex $fids 0]]) } {

        gets $fileId ttLine
    }

    # If the above fails to define ttLibNm, we have the wrong syntax table

    if {![info exists ttLibNm]} {
        refErrorExit "Cannot parse \"$inFile\" as a $SYNTAX(langName)."
    }

    # redefine output name if title line says "realname/filename"
    # (useful in BSP docn)

    if {[regexp "^(\[^/]*)/(\[^/]*$)" $ttLibNm {} ttLibNew ttLibOrg]} {

        set ttLibNm $ttLibNew
        set baseName $ttLibNew
        set outFile "$ttLibNew.$OUTsuffix"
        set outPath "$outDir/$outFile"

    } elseif {"$ttLibNm" != "$baseName"} {

        puts stderr "(refgen: in $inFile) \n\
                     \tWARNING: basename is \"$baseName\" but\
                     name in title is \"$ttLibNm\"\n"
    }

    close $fileId
    puts "$outPath"

}
