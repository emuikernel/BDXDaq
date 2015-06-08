# refgen.tcl - Tornado Reference documentation generator
#
# Copyright 1998-2000 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01v,07mar02,wsl  fix generation of *.lib files
# 01u,11feb02,wsl  change LIB file into individual whateverLib.lib files,
#                  ditto RTN
# 01t,16jan02,wsl  compute honest dependency rule
# 01s,15nov01,wsl  add IDL input format
# 01r,09oct01,wsl  fix and document EXPAND/EXPANDPATH
# 01q,31mar01,wsl  implement minor change in response to review
# 01p,29mar01,wsl  changes necessary for automated build with text output
# 01o,11oct00,dat  added -fname to include file name in help entries SPR35181
# 01n,05jun00,jmp  added -nameOutFile option to refgen.
# 01m,06mar00,dgp  change default book & chapter values
# 01l,11oct99,dgp  merge final changes from pubs_doctools branch
# 01k,08oct99,dgp  update documentation of parameters for C++ and CTcl files
# 01j,29sep99,dgp  accept target.nr or target.ref as input, where target.nr
#                  requires -mg flag
# 01i,29aug99,dgp  update comments and formatting
# 01h,15jul99,dgp  make inFile variable global to support changes to table
#                  processing
# 01g,18mar99,fle  added the -sort option to alphabetically sort functions in
#                  generated document
# 01f,18dec98,fle  mad it able to support object oriented stuff
# 01e,30nov98,p_m  Updated documentation
# 01d,09nov98,fle  passed book name and chapter name to refFormatControl
#                  procedure
# 01c,26oct98,fle  made it full WIN32 compatible
# 01b,23sep98,fle  added -category option
# 01a,09sep98,fle  written.
#

#
# DESCRIPTION
# This tool implements a table-driven process
# for extraction of documentation from source.  Input tables define a
# "meta-syntax" that specifies the details of how documentation is
# embedded in source files for a particular programming language.
# Similarly, output tables define the mark-up details of the
# documentation output.
#
# OVERALL CONVENTIONS
# Some conventions about how documentation is embedded in source code
# do not depend on the source language, and can therefore not be
# changed from the configuration tables.
# 
# \sh Overall Input Conventions
# 
# Routines are organized into <libraries>, and each library
# begins with a DESCRIPTION section.  If a DESCRIPTION heading is
# not present, the description section is taken to be the first
# comment block after the modification history.  Some input languages
# (such as shellscript) may optionally begin with a section headed
# SYNOPSIS instead.
#
# The first line in a library source file is a one-line title
# in the following format:
#    <sourceFileName> - <simple description>
# 
# That is, the line begins (after whatever start-of-comment character
# is required) with the name of the file containing it, separated by
# space, hyphen, and space from a simple description of the library.
# 
# The first line in a routine's description (after the
# source-language-dependent routine delimiter) is a one-line title in
# the same format.
# 
# Routine descriptions are taken to begin after the routine-title line,
# whether or not a DESCRIPTION tag is present explicitly.
# 
# Section headings are specified by all-caps strings beginning at a
# newline and ending with either a newline or a colon.  
# 
# Italics, notably including <Text variables>--that is, words in the
# documentation that are not typed literally, but are instead meant to be 
# replaced by some value specific to each instance of use--are marked in the
# source by paired angle brackets.  Thus, to get the
# output <textVar>, type `\<textVar\>'.
# 
# Boldface words are obtained as follows: 
# \is
# \i `General mechanism:' 
# surround a word with single quotes in the
# source.
# \i `Special words:' 
# words ending in "Lib" or in a recognized filename
# suffix are automatically rendered in bold.  For example, fileName.c,
# object.o, myStuff.tcl all appear in boldface.
# \ie
# 
# Simple lists can be constructed by indenting lines in the source
# from the margin (or from the comment-continuation character, if one
# is required in a particular source language).  For example:
#    line one
#    line two
# 
# \sh Overall Output Conventions
# Library descriptions are automatically prefaced by a synopsis of
# the routines in that library, constructed from the title lines of
# all routines.
# 
# For most input languages, a `SYNOPSIS' section is supplied
# automatically for each routine as well, extracted from the routine
# definition in a language-dependent manner specified in the input
# meta-syntax tables.  Input languages that do not support this have
# empty strings for `$SYNTAX(declDelim)'; in such languages, the
# `SYNOPSIS' section must be explicitly present as part of the
# subroutine comments.
# 
# For some languages, the routine definition is also used to create
# a `PARAMETERS' section automatically.
# 
# The online form of documentation is assumed to fit into a
# structure involving a parent file (which includes a list of
# libraries) and a routine index.  Several of the procedures in this
# library require names or labels for these files, in order to include
# links to them in the output.  The parent file and routine index need
# not actually exist at the time that procedures in this library
# execute.
# 
# `DESCRIPTION' tags are supplied automatically for all description
# sections, whether or not the tag is present in the source file.
# 
# `SEE ALSO' sections are always present in the output for routine
# descriptions, whether or not they are present in the source.  SEE
# ALSO sections for routine descriptions automatically include a
# reference to the containing library.  
# 
# OUTPUT DIRECTIVES
# The following keywords, always spelled in all capital letters
# and appearing at the start of a line, alter the text that
# is considered for output.  Some directives accept an argument
# in a specific format, on the same line.
#
# \is
# \i `NOROUTINES'
# Do not generate subroutine documentation from this file 
# (must appear in the library section).
# \i `NO MANUAL (one word)'
# Suppresses the section where it appears: either an entire routine's
# documentation, or the library documentation.  Routine documentation
# can also be suppressed in language-specific ways, specified by
# matching a regular expression in the meta-syntactic list
# `LOCALdecls'.  See `refDocGen' for a command line option that
# overrides this.
# \i `INTERNAL'
# Suppresses a section (that is, all text from the directive until the
# next heading, if any).  See `refDocGen' for a command line option
# that overrides this.
# \i `APPEND' <filename>
# Include documentation from <filename> in the output as if its source
# were appended to the file containing the `APPEND' keyword.  This is
# especially useful when a library has been broken into several pieces
# for manageability, but still constitutes one logical library.
# Note that this directive is merely a text substitution.  If you append
# a file in a different directory, then further APPEND or EXPAND directives,
# may give unexpected results.
# \i `EXPAND' <filename> <typename>
# Extract the definition of <typename> (either a `struct' or an `enum')
# from the header file <filename>.
# If <filename> begins with a slash (/), its path root is `WIND_BASE'
# unless overridden with the `-exbase' option.  Otherwise, refgen searches
# for <filename> in its `EXPANDPATH' list, which defaults to the current
# working directory.
# \i `EXPANDPATH' <pathname>
# Sets the list of directories to be searched when processing an `EXPAND'
# directive.  If <pathname> begins with a slash (/), its path root is
# `WIND_BASE' unless overridden with the `-exbase' option.  Otherwise,
# <pathname> is appended to the current working directory.  If no
# `EXPANDPATH' directive is supplied, the list consists of only the
# current working directory.  Only one `EXPANDPATH' directive is legal,
# and it should appear before any `EXPAND' directives, preferably in the
# library description.  Note that the current working directory is
# not automatically added to an explicit `EXPANDPATH'.  Note also, that
# if there is more than one header file with the same name, `refgen' 
# searches those files until it finds a type definition or runs out of files.
# \ie
#
# Examples:
# \is
# \i `APPEND' long2Lib.c
# Adds the text of `long2Lib.c' to the end of the file.
# \i `EXPANDPATH' `.:/target/h:/target/h/sys'
# Defines the search path to be
# `.:$WIND_BASE/target/h:$WIND_BASE/target/h/sys'
# \i `EXPANDPATH' `target/h:target/h/sys:.'
# Defines the search path to be `./target/h:./target/h/sys:.'
# \i `EXPAND' `taskLib.h' `WIND_LCB'
# Causes `refgen' to look for a type definition for WIND_LCB in each taskLib.h
# in the `EXPANDPATH' until it finds one.
# \i `EXPAND' `../../h/taskLib.h' `WIND_LCB'
# Without any `EXPANDPATH' statement, `refgen' takes the relative path
# from the current working directory (this example would presumably refer to
# `$WIND_BASE/target/h/'taskLib.h), but if an `EXPANDPATH' statement
# does exist, it looks relative to each of the directories in that list.
# \ie
# 
# EXPLICIT MARKUP
# `refgen' supports a simple markup language that is meant to be
# inconspicuous in source files, while supporting most common output
# needs.
# 
# The following table summarizes `refgen' explicit markup (numbered
# notes appear below the table):
# 
# \ts
#  Note |Markup                       |Description
# ----- +-----------------------------+----------------------
#       |\/" <text to end of line>    |Comment in documentation.
#       |\'text ...\' or \`text ...\' |Boldface text.
#       |\<...\>                      |Italicized text.
#  [1]  |\\\\  or \//                 |The character \/.
#  [2]  |\/\< \/\> \/\& \/\` \/\'     |The characters \< \> \& \` \'.
#       |\\\|                         |The character \| within a table.
#  [3]  |\/ss ... \/se                |Preformatted text.
#       |\/cs ... \/ce                |Literal text (code).
#       |\/bs ... \/be                |Literal text, with smaller display.
#  [4]  |\/is \/i ... \/i ... \/ie    |Itemized list.
#  [5]  |\/ml \/m ... \/m ... \/me    |Marker list.
#  [6]  |\/ts ... \/te                |A table.
#       |\/sh text                    |A subheading.
#       |\/tb reference               |A reference followed by newline.
# \te
# 
# \sh Notes on Markup
# \ml
# \m [1]
# The escape sequence \\\\ is easier to remember for backslash,
# but \// is sometimes required if the literal backslash is to appear
# among other text that might be confused with escape sequences.
# \// is always safe.
# 
# \m [2]
# \< and \> must be escaped to distinguish from embedded
# HTML tags.  \& must be escaped to distinguish from HTML character 
# entities.  Single quotes must be escaped to distinguish from the refgen
# automatic bolding convention.
# 
# \m [3]
# Newlines and whitespace are preserved between \/ss and \/se, but
# formatting is not otherwise disabled.  These are useful for including
# references to text variables in examples.
# 
# \m [4]
# \/is and \/ie mark the beginning and end of a list of items.
# \/i is only supported between \/ls and \/le.  It marks the start
# of an item: that is, it forces a paragraph break, and exempts the
# remainder of the line where it appears from an increased indentation
# otherwise applied to paragraphs between \/is and \/ie.
#
# Thus, the following:
# \cs
#    \is
#    \i Boojum
#    A boojum is a rare
#    tree found in Baja
#    California.
#    \i Snark
#    A snark is a different
#    matter altogether.
#    \ie
# \ce
# 
# Yields output approximately like the following:
#
# \is
# \i Boojum
# A boojum is a rare tree found in Baja California.
#
# \i Snark
# A snark is a different matter altogether.
# \ie
# 
# \m [5]
# \/ml and \/me delimit marker lists; they differ from itemized lists
# in the output format--the marker beside \/m appears on the same
# line as the start of the following paragraph, rather than above.
# 
# \m [6]
# Between \/ts and \/te, a whole region of special syntax reigns,
# though it is somewhat simplified from the original mangen's table syntax.
# 
# Three conventions are central:
# 
#  (a) Tables have a heading section, and a body section; these
# are delimited by a line containing only the characters '-' (hyphen), 
# '+' (plus) and '|' (stile or bar), and horizontal whitespace.
# 
#  (b) Cells in a table, whether in the heading or the body,
# are delimited by the | character.  \| may be used to represent
# the | character.
# 
#  (c) Newline marks the end of a row in either heading or body.
#
# Thus, for example, the following specifies a three-column table
# with a single heading row:
# \cs
#    \ts
#       Key | Name       | Meaning
#       ----|------------|------------
#        \\& | ampersand  | bitwise AND
#        \| | stile      | bitwise OR
#        #  | octothorpe | bitwise NAND
#    \te
# \ce
#
# \ts
# Key | Name       | Meaning
# ----+------------+------------
#  \& | ampersand | bitwise AND
#  \| | stile | bitwise OR
#  #  | octothorpe | bitwise NAND
# \te
#
# The cells need not align in the source, though it is easier to
# read it (and easier to avoid errors while editing) if they do.
# \me
# 
# INTERNAL
# An additional, intentionally un-advertised piece of markup permits
# escaping to the output language.  Any text between \/hs and \/he is
# passed through unaltered to the output file. Thus, you may include
# raw HTML (or whatever output format is in use) with something like
# the following:
# \cs
#    \hs
#    <p><strong>Do <font size=6>NOT</font> 
#    use this kind of thing permanently or lightly.</strong>
#    <p>
#    \he
# \ce
# 
# The example shown above has the following effect:
# \hs
# <p><strong>Do <font size=6>NOT</font> 
# use this kind of thing lightly.</strong>
# <p>
# \he
# 
# This pass-through feature is meant primarily for experimentation,
# and secondarily as an escape hatch.  Resist the temptation to
# use it in published documentation.  Any documentation that contains
# this escape sequence immediately becomes impossible to output
# to another output language.
#
# SYNOPSIS
# \ss
#    refgen [-book <bookName>] [-chapter <chapterName>] [-config <configFile>]
#           [-cpp] [-expath <pathList>] [-exbase <basedir>] [-h] [-int] [-fname]
#           [-l <logFile>] [-mg] [-out <outDir>] [-sort] [-verbose] <fileList>
# \se
#
# PARAMETERS
#   -book : This option allow to define which book the documented
#           routine/library will belong to. The default value is
#           "VxWorks API Reference"
#   -chapter :  Related to the `-book' option, this option allows to set the
#               documented routine / library chapter. The default value is set
#               to "VxWorks Reference Libraries"
#   -category :  Related to the `-book' option, this option allows to set the
#               documented routine / library category. It can be used for
#               example to differentiate routines available for different
#               host platforms
#   -config <configname> : <configname> is of the form <in-format>2<out-format>,
#                          where <in-format> is the file type (valid options are
#                          Auto, Asm, bsp, C, Cpp, CTcl, File, Java, Pcl, sh,
#                          and Tcl) The Auto option deduces the file type from
#                          the file extension.  <out-format> can be any of
#                          default, html, or txt.  The default option is html.
#                          The default for <configname> is Auto2default.  A file
#                          with a name formatted like <configname> must exist
#                          for the actual configuration refgen attempts after
#                          any automatic selection and defaults.
#   -cpp : This option specifies that the list of given files is to be treated
#          as C++ files. A special processing is then done to recover from all
#          the class members
#   -dependencies : Do not process input files, but print the dependency part
#                   of the build rule for the input file.
#   -expath <pathList> : Preempt `EXPANDPATH' settings recorded within files
#                        with the explicitly-supplied colon-delimited path list
#   -exbase <basedir> : To simplify working under incomplete trees, use
#                       <basedir> rather than WIND_BASE as the root for
#                       expand-file searching
#   -fname : Include the file name in the help entry description.  This will
#            help to identify the library name in the help cross reference
#            entries.
#   -int : If this optional parameter is present, format all available
#          documentation, even if marked `INTERNAL' or `NO MANUAL', and even for
#          local routines
#   -l <logFile> : specifies that <logFile> is to be used to log refgen errors,
#                  if `-l' option is not specified, the stdandard output
#                  is used
#   -mg : Convert from mangen markup "on the fly" (in memory, without saving
#         the converted source file)
#   -nameOutFile : Same as -dependencies.  (Deprecated)
#   -out <outputDirectoryName> : Save output documentation file in
#                                <outputDirectoryName>
#   -sort : sorts the routines list (if any). This makes the generated doc file
#           have the routines sorted alphabetically
#   -verbose : Print reassuring messages to indicate something is happening
#   <sourceFileList> : Any other parameters are taken as names of source files
#                      from which to extract and format documentation
#
# EXAMPLE
# 
# Generate HTML manual pages for a BSP sysLib.c file:
#
# \ss
# 	% cd $WIND_BASE/target/config/myBsp
#       % refgen -mg  -book BSP_Reference -chapter <myBsp> -category <myBsp> \
#                -out $WIND_BASE/vxworks/bsp/myBsp sysLib.c
# \se
#
# INCLUDES
#   refLib.tcl
# 
# SEE ALSO: 
# \tb VxWorks Programmer's Guide: C Coding Conventions,
# `htmlLink', `htmlBook', `windHelpLib'
#
# NOROUTINES
#

# INTERNAL
#
# PROCEDURES:
#
# refgen		- refgen main procedure
# refgenCommandeParse	- refgen commande parser
# refgenUsage		- refgen usage displayer

###############################################################################
#
# refgen - top-level documentation extractor
#
# After having parsed the command line, and got the parameters from it, this
# routine gets the associated Markup for each file (C2html associated to .c
# extensions) or it uses <fConfig> for all the files if the -config flags has
# been set in the command line
#
# It then iterates for each file, and gets the file libraries and routines
# descriptions.
#
# The libraries help entries are saved in the LIB files while routines help
# entries are saved in the RTN file (see `helpEntryLib' for further
# informations on those files)
#
# SYNOPSIS
#    refgen
#
# PARAMETERS: N/A
# 
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refgen {} {

    global env                          ;# for WIND_BASE override
    global inFile                       ;# input file to use

    # Input-language description (see refIn*.tcl)

    global INfmtParam
    global INfmtSynop
    global LOCALdecls
    global SYNSUB			;# substitution patterns table
    global SYNTAX 			;# SYNTAX definition table
    global SYNOP 			;# synopsis syntax definition table
    global SYNSEP 			;# SYNTAX separation definition table

    # Output-language description (see refOut*.tcl)

    global OutCAt
    global OUTfmt			;# output formatter table
    global OUTfmtCode
    global OUTfmtDis
    global OUTlMk
    global OUTstr
    global OUTsuffix			;# output file suffix

    # Refgen-specific markup definitions (see refMarkup.tcl)

    global refDisable
    global refMarkSpans

    # Internal state

    global ErrFileName
    global needMangenCvt
    global printInternal
    global srcPath
    global CEPATHS

    # configuration global variables

    global cEBase			;# expand paths base
    global cEpath			;# expand paths
    global fConfig			;# config flag from input line
    global fileList			;# list of files to be parsed
    global isCpp			;# special processing for C++ ?
    global needMangenCvt		;# do we need mangen convertion
    global outDir			;# output doc files directory
    global printInternal		;# do we print internal routines ?
    global docCategory			;# category the doc belongs to
    global docBookName			;# documents book name
    global docChapterName		;# documents chapter name
    global saySomething			;# verbose mode indicator
    global sortRoutines			;# do we have to sort the routines ?
    global onlyOutRule			;# return the dependency rule
    global includeFileName		;# add file name to help entry

    set rtnLinks {}
    set libLinks {}
    set fileListToDelete {}

    # transform path into full compatible path

    regsub -all "\\\\" $outDir "/" outDir

    # Expand-path override

    set CEPATHS [split $cEpath :]

    # Private refgen markup 
    # (nontrivial pieces; others are translated in OUTstr)

    source "${srcPath}refMarkup.tcl"

    # source the help entry library

    source "${srcPath}helpEntryLib.tcl"

    # Basic output formatting (and accum link info):

    if {$saySomething} {
	puts "\nrefgen: BASIC FORMATTING"
	puts -nonewline "refgen: created "
    }

    # parse the -config argument

    if { ! [regexp {^([A-Za-z]+)2([A-Za-z]+)$} $fConfig \
	    match inFormat outFormat]} {
	error "\nInvalid config flag"
    }

    # enforce the default output format

    if { "$outFormat" == "default" } {
	set outFormat html
    }

    # take care of the C++ processing
    # note that with this processing the -cpp flag is unnecessary --
    # it can be replaced with a -config Cpp2default flag

    if { "$inFormat" == "Cpp" } {
	set isCpp 1
    }

    if { $isCpp } {
	set inFormat Cpp
	set fileConfig "Cpp2$outFormat"
	eval [refConfigParse $fileConfig]
	set fileList [refCPPFileProcess $fileList $outDir]
	set fileListToDelete $fileList
    }

    foreach inFile $fileList {

	# initialize routine list and library list

	set libList {}
	set rtnList {}

	if { "$inFormat" == "Auto" } {
	    # Select default config based on file suffix.

	    set fs [file extension $inFile]
	    switch $fs {
		".c"	{set fileConfig C2}
		".tcl"	{set fileConfig Tcl2}
		".sh"	{set fileConfig sh2}
		".pcl"	{set fileConfig pcl2}
		".s"	{set fileConfig Asm2}
		".nr"	{set fileConfig bsp2}
		".ref"	{set fileConfig bsp2}
		".java"	{set fileConfig Java2}
		".idl"  {set fileConfig Idl2}
		default {set fileConfig C2}
	    }
	    append fileConfig $outFormat
	} else {
	    set fileConfig "${inFormat}2$outFormat"
        }

	# Set default values for all standard configurable parameters
	# Go through refConfigParse to avoid dual maintenance of
	# defaults and C2html
	# Override defaults with user's choice if present
	# (may be only a partial override)

	eval [refConfigParse $fileConfig]

	# keep the name of the input file for error messages

        set ErrFileName $inFile        ;# context for error messages

	# for building dependencies, only output the names of the output files

	if { $onlyOutRule } {
	    puts -nonewline [ nameFileDoc $inFile $outDir $defLibIx \
		    $defLibLab $defRtnIx $defRtnLab $docBookName \
		    $docChapterName $docCategory $sortRoutines]
	    return
	} else {
	    set entryList [refFormatControl $inFile $outDir $defLibIx \
		    $defLibLab $defRtnIx $defRtnLab $docBookName \
		    $docChapterName $docCategory $sortRoutines]

	    set outFile [lindex $entryList 0]            ;# output filename
	}

	if {$saySomething} {
	    puts -nonewline "$outFile "
	}

	# update the libraries list

        set lib [lindex $entryList 1]
	set libName [lindex $lib 0]
	set libDesc [lindex $lib 1]
	set libPattern [refNameDecorate LibRx $libName]
	set libLink [refTargetMake Lib $outFile $libName "top"]
	lappend libList [list $libName $libDesc $libPattern $libLink \
			      $docBookName $docChapterName $docCategory {}]

	# update the libraries list

	set routineList [lindex $entryList 2]
	foreach routine $routineList {

	    set rtnName [lindex $routine 0]
	    set rtNum [refRtnNumGet $rtnName]
	    set rtnDesc [lindex $routine 1]

	    # the routine name may contain spaces, make refLnkForm handle this
	    # correctly by giving him the rtnName between braces through a call
	    # to list

	    set rtnDecorated [refLnkForm $outFile [list $rtnName] [list $rtNum]]

	    if {$includeFileName} {
		set rtnDesc "\[$inFile\] $rtnDesc"
	    }

	    lappend rtnList [list [refRtnNameGet $rtnName] $rtnDesc \
				  [lindex [lindex $rtnDecorated 0] 0] \
				  [lindex [lindex $rtnDecorated 0] 1] \
				  $docBookName $docChapterName $docCategory {}]
	}

	# name the .lib and .rtn files after the output file

	set libFileName [file join $outDir [file rootname $outFile].lib]
	set rtnFileName [file join $outDir [file rootname $outFile].rtn]

	# save library entry in <whateverLib>.lib

	set libEntryList {}
	foreach lib $libList {
	    set libEntryList [entryListUpdate $libEntryList $lib]
	}
	entryListInFileSave $libEntryList "$libFileName"

	# save routine entries in <whateverLib>.rtn

	set rtnEntryList {}
	foreach routine $rtnList {
	    set rtnEntryList [entryListUpdate $rtnEntryList $routine]
	}
	entryListInFileSave $rtnEntryList "$rtnFileName"
    }

    # delete the temp files created

    foreach fileName $fileListToDelete {
	catch [file delete $fileName]
    }

    if {$saySomething} {
	puts "\nDone. "
    }
}

################################################################################
#
# refgenCommandeParse - refgen commande parser
#
# This routine parses and verifies refgen commande line arguments, and updates
# some global variables according to those arguments.
#
# SYNOPSIS
#   refgenCommandeParse argv argc
#
# PARAMETERS
#   argv : arguments list vector
#   argc : arguments counter
#
# RETURNS: N/A
#
# ERRORS:
#   Displays help message on arguments error
#

proc refgenCommandeParse { argv argc } {

    # global variables

    global cEBase			;# expand paths base
    global cEpath			;# expand paths
    global fConfig			;# config flag from command line
    global fileList			;# list of files to be parsed
    global isCpp			;# special processing for C++ ?
    global logFileName			;# log file location
    global needMangenCvt		;# do we need mangen convertion
    global outDir			;# output doc files directory
    global printInternal		;# do we print internal routines ?
    global docCategory			;# category the doc belongs to
    global docBookName			;# documents book name
    global docChapterName		;# documents chapter name
    global saySomething			;# verbose mode indicator
    global sortRoutines			;# do we have to sort the routines ?
    global onlyOutRule			;# return the dependency rule
    global includeFileName		;# add file name to help entry

    for {set ix 0} {$ix < $argc} {incr ix} {

	switch -- [lindex $argv $ix] {

	    "-book" {
		incr ix
		regsub -all "_" [lindex $argv $ix] " " docBookName
	    }

	    "-category" {
		incr ix
		regsub -all "_" [lindex $argv $ix] " " docCategory
	    }

	    "-chapter" {
		incr ix
		regsub -all "_" [lindex $argv $ix] " " docChapterName
	    }

	    "-config" {

		# Set the two components of the config flag separately.
		# Multiple -config flags are allowed and useful.
		# It would be better to define separate input and output
		# config flags.

		incr ix
		set newConfig [lindex $argv $ix]
		if { ! [regexp {^([A-Za-z]+)2([A-Za-z]+)$} $newConfig \
			match inFormat outFormat] } {
		    error "\nInvalid config flag"
		}

		if { $inFormat != "Auto" } {
		    regsub {^([A-Za-z]+)} $fConfig "$inFormat" fConfig
		}
		if { $outFormat != "default" } {
		    regsub {([A-Za-z]+)$} $fConfig "$outFormat" fConfig
		}
	    }

	    "-cpp" {
		# note that the "-config Cpp2default" flag accomplishes the
		# same thing as -cpp, and multiple -config flags are allowed.

		set isCpp 1
	    }

	    "-expath" {
		incr ix
		set cEpath [lindex $argv $ix]
	    }

	    "-exbase" {
		incr ix
		set cEBase [lindex $argv $ix]
	    }

	    "-h" {
		refgenUsage
		exit
	    }

	    "-int" {
		set printInternal 1
	    }

	    "-fname" {
		set includeFileName 1
	    }

	    "-l" {
		incr ix
		set tmpLog [lindex $argv $ix]
		if { [file exists $tmpLog] && [file writable $tmpLog] } {
		    catch [file delete $tmpLog]
		    set logFileName $tmpLog
		} elseif { [file writable [file dirname $tmpLog]] } {
		    set logFileName $tmpLog
		} else {
		    refErrorLog "refgen wrong <logFile> parameter" \
				"file is not writable"
		    set logFileName ""
		}
	    }

	    "-mg" {
		set needMangenCvt 1
	    }

	    "-out" {
		incr ix
		set outDir [lindex $argv $ix]
	    }

	    "-sort" {
		set sortRoutines 1
	    }

	    "-dependencies" {
		set onlyOutRule 1
	    }

	    "-nameOutFile" {  ;# for backward compatibility only
		set onlyOutRule 1
	    }

	    "-verbose" {
		set saySomething 1
		puts "refgen: Verbose mode ON"
	    }

	    default {
		lappend fileList [lindex $argv $ix]
	    }
	}
    }
}

################################################################################
#
# refgenInit - refgen initialisation procedure.
#
# This procedure declares and pre-sets all the global variables, then it sources
# all the needed libraries.
#
# SYNOPSIS
#   refgenInit
#
# PARAMETERS: N/A
#
# ERRORS: N/A
#
# SEE ALSO
# refLib
#

proc refgenInit {} {

    # global variables

    global cEBase			;# expand paths base
    global cEpath			;# expand paths
    global env				;# global user's environment
    global fConfig			;# config flag from command line
    global fileList			;# list of files to be parsed
    global isCpp			;# is there C++ file processing ?
    global logFileName			;# location of the log file
    global needMangenCvt		;# do we need mangen convertion
    global outDir			;# output doc files directory
    global printInternal		;# do we print internal routines ?
    global docBookName			;# documents book name
    global docChapterName		;# documents chapter name
    global docCategory			;# doc category
    global saySomething			;# verbose mode indicator
    global sortRoutines			;# do we have to sort the routines ?
    global srcPath			;# global tcl source path
    global onlyOutRule			;# return the dependency rule
    global includeFileName		;# add file name to help entries
    global groupLabel                   ;# string applying to following rtns

    # global variables initialization

    set cEBase $env(WIND_BASE)
    set cEpath {}
    set fConfig Auto2default
    set fileList {}
    set isCpp 0
    set includePathList {}
    set logFileName ""
    set needMangenCvt 0
    set outDir "."
    set printInternal 0
    set docCategory ""
    set docBookName "VxWorks API Reference"
    set docChapterName "VxWorks Reference Libraries"
    set saySomething 0
    set sortRoutines 0
    set srcPath "[file dirname [info script]]/"
    set onlyOutRule 0
    set includeFileName 0
    set groupLabel {}

    # source windHelp library from Tornado tree if possible, or from local dir
    # else

    if { [info exists env(WIND_BASE)] } {
	source "$env(WIND_BASE)/host/resource/doctools/refLib.tcl"
    } else {
	source refLib.tcl
    }
}

################################################################################
#
# refgenUsage - refgen usage display help
#
# This routine just displays refgen help message and exits.
#
# SYNOPSIS
#   refgenUsage
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc refgenUsage {} {

    puts stdout "\nrefgen : source file documentation generator\n"
    puts stdout "usage :\n"
    puts stdout "refgen\t\[-book <bookName>\] \[-chapter <chapterName>\]"
    puts stdout "\t\t\[-config <configFile>\] \[-expath <pathList>\]"
    puts stdout "\t\t\[-exbase <basedir>\] \[-h\] \[-int\] \[-fname\] \[-mg\]"
    puts stdout "\t\t\[-out <outDir>\] \[-verbose\] <fileList>\n"
    puts stdout "with :\n"
    puts stdout "\t-book <bookName>\t: specifies the doc book the documents"
    puts stdout "\t\t\t\t  will belong to"
    puts stdout "\t-chapter <chapterName>\t: uses <chapterName> as the chapter"
    puts stdout "\t\t\t\t  the documents will belong to"
    puts stdout "\t-config <configFile>\t: the config file to use to generate"
    puts stdout "\t\t\t\t  documentation"
    puts stdout "\t-cpp : the list of files is to be taken as C++ files"
    puts stdout "\t-expath <pathList>\t: uses the column separated path list to"
    puts stdout "\t\t\t\t  replace in expand path with"
    puts stdout "\t-exbase <baseDir>\t: To simplify working under incomplete"
    puts stdout "\t\t\t\t  trees, use <basedir> rather than WIND_BASE as"
    puts stdout "\t\t\t\t  the root for expand-file searching."
    puts stdout "\t-h\t\t\t: prints this help message"
    puts stdout "\t-int\t\t\t: even NOROUTINES or NOMANUAL sections are"
    puts stdout "\t\t\t\t  documented"
    puts stdout "\t-l <logFile>\t\t: specifies log file name"
    puts stdout "\t-fname\t\t\t: include file name in help entries"
    puts stdout "\t-mg\t\t\t: Convert from mangen markup \"on the fly\""
    puts stdout "\t-out <outDir>\t\t: uses <outDir> as the out doc directory"
    puts stdout "\t-sort\t\t\t: makes the routines be alphabetically sorted"
    puts stdout "\t-verbose\t\t: sets verbose mode on"
    puts stdout "\t<fileList>\t\t: Any other element is taken as a file name\n"
}

#
# main - the refgen main entry
#

refgenInit
refgenCommandeParse $argv $argc
refgen
