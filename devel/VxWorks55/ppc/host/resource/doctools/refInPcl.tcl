# refInPcl - supporting tables for refgen: pcl source input
#
# Copyright 1998-2000 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,30nov01,wsl  fix EXPAND handling and cater to old synopsis format
# 01h,03dec99,elg  Update to accept more than one input parameter.
# 01g,03feb99,fle  added elements to SYNOP array for synopsis parsing
# 01f,12jan99,fle  added the SYNOP array
# 01e,03nov98,fle  added SYNSEP separator syntax
# 01d,07aug98,fle  modified the refInHExp's regular expression
# 01c,07jul98,rhp  make SYNTAX(rtSt) describe entire line
# 01b,05jun98,fle  made it accept a : char after SYNOPSIS or DESCRIPTION
#                  keywords
# 01a,28may98,rhp  Initial check-in

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# "pcl" meta-syntax arrays

# SYNTAX array values are regular expressions; use regexp with them.
# NOTE: tabs are converted to blanks, but only _after_ we snarf a
# library or routine entry.  Thus, boundary descriptions for libraries
# and routines may need to consider the possibility of tabs in the input.

array set SYNTAX {
    {langName}	"protocol description file"
    {preTitle}	""
    {titlStart}	"/\\*"
    {titlEnd}	"\\*/"
    {libTop}	"\n(DESCRIPTION|SYNOPSIS)[ :]*\n"
    {libTopAlt}	"modification history[^\\*]+\\*/\n+/\\*\n"
    {libEnd}	"\n\\*/"
    {libCn}	"\n"
    {rtSt}	"\n/\\*\\*\\*\\*\\*\\*\\*\\*\\*\\** *"
    {rtCn}	"\n\\* ?"
    {rtEn}	"\\*/"
    {rtNm}	"[_a-zA-Z0-9]*"
    {declDelim}	"\\*/\n*"
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
    {cmtEnd}		"*/"
    {cmtSt}		"/*"
    {getSyn}		1
    {paramEnd}		")"
    {paramIdt}		"    "
    {paramNl}		"\n"
    {paramVoid}		"void"
    {paramSt}		"("
    {procExcept}	"\nvoid\[ \t]+\\(\\*signal"
    {ptr}		"(\[^/])\\*(\[^/])"
    {ptrSub}		"\\1\* \\2"
    {synEnd}		"\{"
    {synSep}		","
    {synStart}		"*/"
}

# SYNSUB array values are substitution strings for regsub.

array set SYNSUB {
    {rtMk}	"`&'"
    {rtUnmk}	1
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

set INfmtSynop "
    {
        {\\(none\\)}
        {none}
        {Update markup on the fly -- remove parentheses around 'none'}
    }
    {
	{\[ \t\n]*(\[^ \t\n]+)\[^(]*\\((\[^\\)]+)\\).*}
	{INPUT: \\2\nOUTPUT: \\1\n}
	{Extract argument type and label INPUT, extract result type
         and label OUTPUT; discard rest of pseudo-synopsis.}
    }
"
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

set LOCALdecls {}

###############################################################################
#
# refInHExp - expand header data-structure definition
#
# Extract from header-file text <htxt> the definition of <name>.
# This procedure is in the input-language specification because finding
# the boundaries of such definitions is tightly dependent on syntax.
#
# RETURNS: The portion of <htxt> that defines <name>.

proc refInHExp {
    htxt         # text of header file containing definition
    name         # name of definition to be expanded
    } {
    set defn {}     ;# default, for empty result if not found

    regsub -all {typedef} $htxt "\x81" htxt
        # note that \x81 is not printing, and is used as a marker

    regexp "\n\x81\[ \t\]+(struct|enum)\[ \t\n\]\[^\x81\]*$name;\[^\n]*\n" \
	    $htxt defn
    regsub "\x81" $defn "typedef" defn

    return $defn
}
