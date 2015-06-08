# refInCpp.tcl - supporting tables for refgen: C++ source input
#
# Copyright 1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01d,30nov01,wsl  fix EXPAND handling
# 01c,23feb99,fle  updated to use // for comment mark instead of /* and */
# 01b,03feb99,fle  added elements to SYNOP array for synopsis parsing
# 01a,05jan99,fle  written, based on refInC.tcl v.01m

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# C++ meta-syntax arrays

# SYNTAX array values are regular expressions; use regexp with them.
# NOTE: tabs are converted to blanks, but only _after_ we snarf a
# library or routine entry.  Thus, boundary descriptions for libraries
# and routines may need to consider the possibility of tabs in the input.

array set SYNTAX {
    {langName}	"C++ source file"
    {preTitle}	""
    {titlStart}	"//"
    {titlEnd}	""
    {libTop}	"\n// ?(DESCRIPTION|SYNOPSIS)\[ :\]*\n"
    {libTopAlt}	"\n//\[ \t]*\n+//\[ \t]*\n"
    {libEnd}	"\n\[ \t]*\n"
    {libCn}	"\n// ?"
    {rtSt}	"//////////////////////////////////////////*\[ \t]*"
    {rtCn}	"\n// ?"
    {rtEn}	"\n\[ \t]*\n"
    {rtNm}	"[-_a-zA-Z0-9\~:]*"
    {declDelim}	"\n\[ \t]*\n+"
    {rtDfltHd}	"DESCRIPTION\n"
}

# SYNOP is an array of list of SYNOPSIS related syntax
#
# -cmtEnd :	specifies the string that identifies a comment end
# -cmtSt :	specifies the string that identifies a comment start
# -getSyn :	specifies if refgen has to get the synopsis form the procedure
#		declaration, or if the synopsis is already in the procedure
#		header
# -paramEnd :	specifies the string that identifies the parameters declaration
#		end
# -paramIdt :	specifies the pattern to indent routine's parameters
# -paramNl :	parameter new line pattern
# -paramVoid :	void routine parameter
# -paramSt :	specifies the string that identifies the parameters declaration
#		start 
# -procExcept :	pattern that should not be processed in the synopses formatting
# -ptr :	parameter pointer pattern
# -ptrSub :	substitution pattern to pointer pattern
# -synEnd :	specifies the pattern that should end the extra strings after
#		the paramEnd string
# -synSep :	parameter separator mark
# -synStart :	extra synopsis pattern to reach after parameters processing
#

array set SYNOP {
    {cmtEnd}		"\n"
    {cmtSt}		"//"
    {getSyn}		1
    {paramEnd}		")"
    {paramIdt}		"    "
    {paramNl}		"\n"
    {paramVoid}		"void"
    {paramSt}		"("
    {procExcept}	"noProcExcept"
    {ptr}		"(\[^/])\\*(\[^/])"
    {ptrSub}		"\\1\* \\2"
    {synEnd}		":|;|\{|\#elif"
    {synSep}		","
    {synStart}		"\n\[^/]\[^/]"
}

# SYNSUB array values are substitution strings for regsub.

array set SYNSUB {
    {rtMk}	"&()"
    {rtUnmk}	0
}

# SYNSEP array values for separation marks in doc names.

array set SYNSEP {
    {rtSep}	";"
    {rtSepSub}	" "
}

# INfmtSynop is a list of transformations to perform on
# function synopses in this language, prior to any output-dependent formatting.
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set INfmtSynop {}

# INfmtRawSynop is a list of transformations to perform on functions synopses
# in this language. The goal of those transformations is to have a "not-tagged"
# synopsis
#
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set INfmtRawSynop {}

# INfmtParam is a list of transformations to tease the contents of a 
# PARAMETERS section from the per-argument comments of a subroutine/
# procedure definition.
 
set INfmtParam {}

# LOCALdecls is a list of regexps that declare a subroutine local
# in this language.

set LOCALdecls "
    {\n\[ ]*LOCAL\[ ]+\[_a-zA-Z0-9* ]*\n}
    {\n\[ ]*LOCAL\[ ]+\[_a-zA-Z0-9* ()]*\n?}
    {\n\[ ]*static\[ ]+\[_a-zA-Z0-9* ]*\n}
    {\n\[ ]*static\[ ]+\[_a-zA-Z0-9* ()]*\n?}
"

###############################################################################
#
# refInHExp - expand header data-structure definition
#
# Extract from header-file text <htxt> the definition of <name>.
# This procedure is in the input-language specification because finding
# the boundaries of such definitions is tightly dependent on syntax.
#
# SYNOPSIS
#   refInHExp <htxt> <name>
#
# PARAMETERS
#   htxt : text of header file containing definition
#   name : name of definition to be expanded
#
# RETURNS: The portion of <htxt> that defines <name>.
#
# ERRORS: N/A
#

proc refInHExp { htxt name } {

    set defn {}     ;# default, for empty result if not found

    regsub -all {typedef} $htxt "\x81" htxt
        # note that \x81 is not printing, and is used as a marker

    regexp "\n\x81\[ \t\]+(struct|enum)\[ \t\n\]\[^\x81\]*$name;\[^\n]*\n" \
	    $htxt defn
    regsub "\x81" $defn "typedef" defn

    return $defn
}