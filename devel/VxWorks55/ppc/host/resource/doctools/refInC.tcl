# refInC - supporting tables for refgen: C source input
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01q,03oct01,wsl  fix EXPAND capability
# 01p,05aug99,dgp  correctly preserve spaces in lines beginning with a space,
#                  handle DESCRIPTION
# 01o,01mar99,fle  added set_terminate to the synopses exception list
# 01n,28jan99,fle  added so elements to SYNOP array
# 01m,24dec98,fle  added SYNOP and other stuff for syngen
# 01l,09nov98,fle  re-introduces fix for one line LOCAL or static function
#                  declarations
# 01k,03nov98,fle  added SYNSEP separator syntax
# 01j,14oct98,fle  added tilde and column characters to SYNTAX(rtNm) for C++
#                  conveniance
# 01i,31aug98,fle  changed SYNTAX(rtEn)
# 01h,29jul98,fle  changed LOCALdecls regexp : * -> +
# 01g,12jul98,rhp  improve LOCALdecls to allow * in decl line
# 01f,07jul98,rhp  rewrite LOCALdecls to recognize names w/numerics
# 01e,07jul98,rhp  correct SYNTAX(rtSt) to avoid overlap with SYNTAX(rtCn)
# 01d,22jun98,rhp  refine refInHExp (for expansion of header typedefs)
# 01c,05jun98,fle  made it accept a : char after SYNOPSIS or DESCRIPTION
#                  keywords
# 01b,02jun98,rhp  better defn for SYNTAX(rtEn)
# 01a,15may98,rhp  Initial check-in

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# C meta-syntax arrays

# SYNTAX array values are regular expressions; use regexp with them.
# NOTE: tabs are converted to blanks, but only _after_ we snarf a
# library or routine entry.  Thus, boundary descriptions for libraries
# and routines may need to consider the possibility of tabs in the input.

array set SYNTAX {
    {langName}	"C source file"
    {preTitle}	""
    {titlStart}	"/\\*"
    {titlEnd}	"\\*/"
    {libTop}	"\n(DESCRIPTION|SYNOPSIS)\[ :\]*\n"
    {libTopAlt}	"\n\\*/ *\n+/\\* *\n"
    {libEnd}	"\n\\*/"
    {libCn}	"\n"
    {rtSt}	"\n/\\*\\*\\*\\*\\*\\*\\*\\*\\*\\** *"
    {rtCn}	"\n\\* ?"
    {rtEn}	"\\*/"
    {rtNm}	"[-_a-zA-Z0-9\~:]*"
    {declDelim}	"\\*/\n*"
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
    {cmtEnd}		"*/"
    {cmtSt}		"/*"
    {getSyn}		1
    {paramEnd}		")"
    {paramIdt}		"    "
    {paramNl}		"\n"
    {paramVoid}		"void"
    {paramSt}		"("
    {procExcept}	"(\nvoid\[ \t]+\\(\\*signal)|(\nextern void \\(\\*set_new_handler)|(\nextern void \\(\\*set_terminate)"
    {ptr}		"(\[^/])\\*(\[^/])"
    {ptrSub}		"\\1\* \\2"
    {synEnd}		"\{"
    {synSep}		","
    {synStart}		"*/"
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

set INfmtSynop {
    {
	"(\n\[ \t]*)(FAST|register)\[ \t]+"
	"\\1"
	{get rid of all the FAST or register parameters}
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

set INfmtRawSynop {
}

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
# The procedure looks for the first valid typedef before the typedef name,
# and returns the entire definition.
#
# SYNOPSIS
#   refInHExp <htxt> <name>
#
# PARAMETERS
#   htxt text of header file containing definition
#   name : name of definition to be expanded
#
# RETURNS: The portion of <htxt> that defines <name>.
#
# ERRORS: N/A
#

proc refInHExp { htxt name } {

    set defn {}

    regsub -all {typedef} $htxt "\x81" htxt
        # note that \x81 is not printing, and is used as a marker

    regexp "\n\x81\[ \t\]+(struct|enum)\[ \t\n\]\[^\x81\]*$name;\[^\n]*\n" \
	    $htxt defn
    regsub "\x81" $defn "typedef" defn

    return $defn
}
