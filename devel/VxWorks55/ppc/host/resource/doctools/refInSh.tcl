# refInSh - supporting tables for refgen: shellscript input
#
# Copyright 1998-1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01e,03feb99,fle  added elements to SYNOP array for synopsis parsing
# 01d,12jan99,fle  added the SYNOP array
# 01c,03nov98,fle  added SYNSEP separator syntax
# 01b,05jun98,fle  made it accept a : char after SYNOPSIS or DESCRIPTION
#                  keywords
# 01a,15may98,rhp  Initial check-in

###############################################################################
#
# Sh meta-syntax arrays

# SYNTAX array values are regular expressions; use regexp with them.
# NOTE: the "Routine" values are irrelevant, but because the conversion
#       preprocessing pass is unaware of this, rtCn and rtEn are set to
#       "unlikely" placeholders.
# NOTE: tabs are converted to blanks, but only _after_ we snarf a
# library or routine entry.  Thus, boundary descriptions for libraries
# and routines may need to consider the possibility of tabs in the input.

array set SYNTAX {
    {langName}	"shell script file"
    {preTitle}	"^#([ \t]*$|!)"
    {titlStart}	"#"
    {titlEnd}	""
    {libTop}	"\n# (DESCRIPTION|SYNOPSIS)[ \t:]*\n"
    {libTopAlt}	"\n# [0-9][0-9][a-z],[^\n#]+\n[ \t]*\n"
    {libEnd}	"\n[ \t]*\n"
    {libCn}	"\n# ?"
    {rtSt}	""
    {rtCn}	"GIGOgigoGIGOgigoGIGO"
    {rtEn}	"GIGOgigoGIGOgigoGIGO"
    {rtNm}	""
    {declDelim}	""
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
    {cmtSt}		"#"
    {getSyn}		0
    {paramEnd}		"\}"
    {paramIdt}		" "
    {paramNl}		"\n"
    {paramVoid}		"noParamVoid"
    {paramSt}		"\{"
    {procExcept}	"noProcExcept"
    {ptr}		"noPtrPattern"
    {ptrSub}		"noPtrPattern"
    {synEnd}		"\{"
    {synSep}		" "
    {synStart}		"#\n\n"
}

# SYNSUB array values are substitution strings for regsub.

array set SYNSUB {
    {rtMk}	""
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
 
# NOTE! This requires elements from $OUTstr be visible when this
# file is sourced; hence, must source refOut... files prior to
# refIn... files.  This is enforced by the refConfigParse defn in refLib.

set INfmtParam {}

# LOCALdecls is a list of regexps (to insert between $SYNTAX(rtCn) 
# and newline) that declare a subroutine local in this language.

set LOCALdecls {}
