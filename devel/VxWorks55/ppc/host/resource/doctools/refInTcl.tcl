# refInTcl - supporting tables for refgen: Tcl input
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01h,05aug99,dgp  correctly preserve spaces in lines beginning with a space,
#                  handle DESCRIPTION
# 01g,03feb99,fle  added elements to SYNOP array for synopsis parsing
# 01f,02dec98,fle  allowed the semi-column character after the DESCRIPTION
#                  header field
# 01e,03nov98,fle  added SYNSEP separator syntax
# 01d,02sep98,fle  changed SYNTAX(libCn) int \n#?[ \t]*
# 01c,10aug98,fle  linksUpdate adaptation : removed sigle quotes from
#                  substitution patterns
# 01b,07jul98,rhp  make SYNTAX(rtSt) describe entire line
# 01a,15may98,rhp  Initial check-in

###############################################################################
#
# Tcl meta-syntax arrays

# SYNTAX array values are regular expressions; use regexp with them.
# NOTE: tabs are converted to blanks, but only _after_ we snarf a
# library or routine entry.  Thus, boundary descriptions for libraries
# and routines may need to consider the possibility of tabs in the input.

array set SYNTAX {
    {langName}	"Tcl source file"
    {preTitle}	""
    {titlStart}	"#"
    {titlEnd}	""
    {libTop}	"\n\# +(DESCRIPTION|SYNOPSIS)\[ :\]*\n"
    {libTopAlt}	"\n\# \[0-9\]\[0-9\]\[a-z\],\[^\n\#\]+\n\[ \t\]*\n"
    {libEnd}	"\n\[ \t\]*\n"
    {libCn}	"\n\# ?"
    {rtSt}	"\n\##########* *"
    {rtCn}	"\n\# ?"
    {rtEn}	"\n\[^\#\]"
    {rtNm}	"[^ \n\]*"
    {declDelim}	"\n\[^\#\]"
    {rtDfltHd}	"(DESCRIPTION|SYNOPSIS)\n"
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
    {cmtSt}		"\[^\n]+#"
    {getSyn}		0
    {paramEnd}		"\}"
    {paramIdt}		" "
    {paramNl}		"\n"
    {paramVoid}		"noParamVoid"
    {paramSt}		"\{"
    {procExcept}	"noProcExcept"
    {ptr}		"noPtrPattern"
    {ptrSubst}		"noPtrSubst"
    {synEnd}		"\n#\[ \t]*\n"
    {synSep}		" "
    {synStart}		"#\[ \t]+SYNOPSIS\[ \t]*:?\[ \t]*\n(#\[ \t]*\\.tS\[ \t]*\n)?#\[ \t]*"
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

set INfmtSynop {

    {
	{#[^
]*
}
	{ }
	{Remove argument comments, collapse args to single line.}
    }

    {
	{
+}
	{ }
	{Remove any remaining newlines.}
    }

    {
	{ +}
	{ }
	{Normalize spaces.}
    }
}

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
 
# NOTE! This "knows" bits of the refgen-private markup.  
# Extend refMarkup.tcl to avoid that?

set INfmtParam "
    {
	{PARAMETERS\[ \t\]*:?\[ \t\]*\n}
	{\\ts}
	{start parameters mark}
    }

    {
	{\n#?\[ \t\]*\n}
	{\n\n\\te\n}
	{end parameters mark}
    }

    {
	{ *(\[^ \\{\\}]+) +#(\[^\n]*)\n}
	{\n<\\1> | \\2}
	{Argument descriptions; no default value.}
    }

    {
	{ *\\{(\[^ \\{\\}]+) +(\[^\n]*) *\\} *#(\[^\n]*)\n}
	{\n\[<\\1>] | \\3 (Default value: \\2)}
	{Argument descriptions; default value.}
    }
"

# LOCALdecls is a list of regexps (to insert between $SYNTAX(rtCn) 
# and newline) that declare a subroutine local in this language.

set LOCALdecls {}
