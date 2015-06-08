# refOutTxt - supporting tables for refgen: output to plain text
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,15nov01,wsl  add IDL input format
# 01h,13apr01,wsl  change routine separator (again)
# 01g,12apr01,wsl  redesign routine separator so it won't be confused with
#                  board diagrams
# 01f,10apr01,wsl  add missing elements to OUTlMk array
# 01e,01apr01,wsl  add some missing elements in OUTstr
# 01d,28mar01,wsl  fix numerous regexp errors
# 01c,17nov99,rbl  Remove \\nl line break markup from text format (this
#                  becomes <br> in html format)
# 01b,22sep98,fle  added CELL ending markup
# 01a,16jul98,rhp  Initial check-in

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTsuffix - filename suffix for output files
#

set OUTsuffix txt

###############################################################################
#
# refStartMkup - emit markup for start of output file
#
# Like the rest of the ref*Mkup proc family, this uses <args> rather
# than the fixed number of arguments desired so that, if the same proc in
# another output format requires additional arguments, this definition 
# need not change.
#
# PARAMETERS
# \ts
# <args>   |
# Index    | Meaning
# ---------+----------------------------
# 0        | (ignored)
# 1        | (ignored)
# 2        | Library name without suffix
# \te
#
# RETURNS: markup for start of output file.

proc refStartMkup {args} {
    set libName [lindex $args 2]
    set z "[string toupper $libName]\n"

    # FIXME! I have no Tcl manual while writing this, so I cannot check
    # if there's a simpler way to do the following; seems like there
    # ought to be.  The goal is to make a string of "=" of the
    # same length as $libName.  If there's a simpler way, replace this!

    regsub -all "." $libName "=" sep

    append z "$sep\n"
    return $z
}

###############################################################################
#
# refTitleMkup - emit markup for output-file title
#
# Like the rest of the ref*Mkup proc family, this uses <args> rather
# than the fixed number of arguments desired so that, if the same proc in
# another output format requires additional arguments, this definition 
# need not change.
#
# PARAMETERS
# \ts
# <args> |
# Index    | Meaning
# ---------+----------------------------
# 0        | Name of library, routine, procedure...
# 1        | Description of same
# 2        | (ignored)
# 3        | text to insert at top
# \te
# 
# RETURNS: markup for output-file title

proc refTitleMkup {args} {
    global OUTfmt

    set ttlName [refFmtString $OUTfmt [lindex $args 0]]
    set ttlDesc [refFmtString $OUTfmt [lindex $args 1]]
    set ttlInsert [lindex $args 3]

    if {0 != [string length $ttlDesc]} {

	set z "$ttlInsert\nNAME\n$ttlName - $ttlDesc\n\n"

    } else {

	set z "$ttlInsert\nNAME\n$ttlName\n\n"

    }	
    
    return $z
}

###############################################################################
#
# refNavbarMkup - emit markup for navigation bar
#
# Like the rest of the ref*Mkup proc family, this uses <args> rather
# than the fixed number of arguments desired so that, if the same proc in
# another output format requires additional arguments, this definition 
# need not change.
#
# In this format, all arguments are ignored.
#
# RETURNS: Empty string.

proc refNavbarMkup {args} {
    return {}
}

###############################################################################
#
# refTblsepMkup - emit markup for separator between table heading and body
#
# Like the rest of the ref*Mkup proc family, this uses <args> rather
# than the fixed number of arguments desired so that, if the same proc in
# another output format requires additional arguments, this definition 
# need not change.
#
# PARAMETERS
# \ts
# <args> |
# Index    | Meaning
# ---------+----------------------------
# 0        | Number of columns in table
# \te
#
# HEURISTIC: Allow 15 chars per column, up to a maximum of 80 chars
#
# RETURNS: markup for separator between table heading and body

proc refTblsepMkup {args} {
    set nCols [lindex $args 0]

    set w [expr $nCols * 15]
    set w [expr {$w > 80 ? 80 : $w}]

    set z "\n"
    set i 0
    while {[incr i] < $w} {
	append z -
    }

    return "$z"
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTstr - output boilerplate table
#
# This table includes only those output boilerplate strings that do not
# require any parameters filled in for any implemented output format.
#
# The proc family ref*Mkup captures parametric boilerplate.
#
# NOTE: the SUBHEAD element has a tab in the blank line.  This is
# essential as a heading-marker in order to process the INTERNAL keyword.

array set OUTstr {

{SEP} {
_____________________________________ @@@ __________________________________
}

{INDENT} {}

{DEINDENT} {}

{STOP}  {
}

{NL} {
}

{PAR} {

}

{ENDPAR} {}

{CODE} {
}

{ENDCODE} {
}

{SMCODE} {
}

{ENDSMCODE} {
}

{SUBHEAD} {
	
}

{FIXEDSP} { }

{TSTART} {
}

{TEND} {
}

{TROWS} {
}

{TROWE} {}

{TCELL} {}

{TCELLE} {}

{THCELL} {}

{THCELLE} {}

{LIST} {
}

{ENDLIST} {
}

{PREitem} {}

{POSTitem} {
}

{MLIST} {
}

{ENDMLIST} {
}

{PREmark} {}

{POSTmark} {
}

{SYNdelim} {
-----
}

{GENPARENT} {}

{RTNSDESC} {}

{RTNSDESCEND} {}

{BLIST} {}

{BLISTEND} {}

{PREbullet} {}

{POSTbullet} {}
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTfmt - automatic decorations for ordinary text
#
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set OUTfmt {
{
    {
SECTION[^
]*
}
    {
}
    {Obsolete keyword; ignore.}
}

{
    {
NOROUTINES[^
]*
}
    {
}
    {Keyword already processed by the time this is used; remove.}
}

{
    {\\<}
    {<}
    {Less-than character}
}

{
    {\\>}
    {>}
    {Greater-than character}
}

{
    {\\&}
    {\&}
    {Ampersand character -- don't know why the backslash is necessary,
     but it is.}
}

{
    {\\\|}
    {|}
    {Pipe character -- extra backslash is necessary.}
}

{
    {(^|[^\a-zA-Z0-9])`([^']+)'}
    {\1\2}
    {Bold in other output formats; just remove markup, here.}
}

{
    {(^|[^\a-zA-Z0-9])'([^']+)'}
    {\1\2}
    {Bold in other output formats; just remove markup, here.}
}

{
    {\\nl
?}
    {
}
    {new line markup - remove if already followed by newline}
}

{
    {\\tb ([^
]*)}
    {\1} 
    {ref title -- can't italicize, so remove}
}

{
    {\\`}
    {`}
    {Single quote.  Depends on auto-bolding not allowing \ before quote.}
}

{
    {\\'}
    {'}
    {Single quote.  Depends on auto-bolding not allowing \ before quote.}
}

{
    {^([A-Z][A-Z ]*)[
:] *}
    {
	
\1
}
    {Heading, when at start of string. May end in newline or colon.  
     TAB IN BLANK LINE IS ESSENTIAL for "INTERNAL" processing; matches
     OUTstr(SUBHEAD). }
}

{
    {
([A-Z][A-Z ]*)[
:] *}
    {
	
\1
}
    {Heading, other than start position.  Ends with newline or colon.
     TAB IN BLANK LINE IS ESSENTIAL for "INTERNAL" processing; matches
     OUTstr(SUBHEAD). }
}

{
    {\\sh[ ]+([^
]*)
}
    {
\1:
}
    {small heading}
}

{
    {\\"[^
]*
}
    {}
    {In-doc comment, extends to end of line; remove from output.}
}

{
    "\n\[\n \]*\n"
    "\n\n"
    {Cleanup: do not use more than one blank line in a row.}
}

{
    {(\\\\)|\\/}
    {\\}
    {Backslash character, escape escaped.
    MUST BE LAST BACKSLASH SEQUENCE IN LIST.}
}

}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTfmtCode - automatic decorations for code
#
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set OutCAt {
    {
	{/@}
	{/*}
	{Substitute C start-comment sequence for examples.}
    }

    {
	{@/}
	{*/}
	{Substitute C end-comment sequence for examples.}
    }
}

set OUTfmtCode [concat $OutCAt {
    {
	{\\<}
	{<}
	{Less-than character.}
    }

    {
	{\\>}
	{>}
	{Greater-than character.}
    }

    {
	{\\&}
	{\&}
	{Ampersand character -- don't know why the backslash is required.}
    }

    {
	{(^|[^\a-zA-Z0-9])`([^']+)'}
	{\1\2}
	{"Bold", using paired quotes. Remove markup.}
    }

    {
	{(^|[^\a-zA-Z0-9])'([^']+)'}
	{\1\2}
	{"Bold", using repeated apostrophes.  Remove markup.}
    }

    {
	{\\`}
	{`}
	{Single quote.  Depends on auto-bolding not allowing \ before quote.}
    }

    {
	{\\'}
	{'}
	{Single quote.  Depends on auto-bolding not allowing \ before quote.}
    }

    {
	{(\\\\)|\\/}
	{\\}
	{Backslash character, escape escaped.
	MUST BE LAST BACKSLASH SEQUENCE IN LIST.}
    }

}]


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTfmtDis - replacements to disable all output formatting in a string
#
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set OUTfmtDis {
}


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTlMk - markup related to establishing links
# 
# The array elements are simple strings, not regexps.
#
# The `*NmSt'/`*NmEn' pairs are used to recognize boundaries of linkable
# names, <not> to format those names in the first place.  Their prefixes
# permit recognizing language-specific formatting.
#
# The remaining elements are used to insert links.  `lnSt' marks the start
# of a link; `lnLab' marks the start of the label, or visible tag, for 
# the link; and `lnEnd' marks the end of the link.

array set OUTlMk {
    {AsmSbNmSt} {}
    {AsmSbNmEn} {\(\)}
    {CSbNmSt} {}
    {CSbNmEn} {\(\)}
    {CppSbNmSt} {}
    {CppSbNmEn} {\(\)}
    {JavaSbNmSt} {}
    {JavaSbNmEn} {\(\)}
    {PclSbNmSt} {}
    {PclSbNmEn} {}
    {TclSbNmSt} {}
    {TclSbNmEn} {}
    {CTclSbNmSt} {}
    {CTclSbNmEn} {}
    {LibSbNmSt} {}
    {LibSbNmEn} {}

    {AsmRxNmSt} {}
    {AsmRxNmEn} {\(\)}
    {CRxNmSt} {}
    {CRxNmEn} {\(\)}
    {CppRxNmSt} {}
    {CppRxNmEn} {\(\)}
    {IdlRxNmSt} {}
    {IdlRxNmEn} {\(\)}
    {JavaRxNmSt} {}
    {JavaRxNmEn} {\(\)}
    {PclRxNmSt} {}
    {PclRxNmEn} {}
    {TclRxNmSt} {}
    {TclRxNmEn} {}
    {CTclRxNmSt} {}
    {CTclRxNmEn} {}
    {LibRxNmSt} {}
    {LibRxNmEn} {}

    {lnSt}   {}
    {lnAnch} {}
    {lnLab}  {}
    {lnEnd}  {}
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#
# OUTlSelfLess - regsub arguments to remove self-references 
#
# Self-references inserted by the automatic linking support, in HTML
# output files, are fairly easily recognized: an <a href= ...> 
# separated by exactly one newline from a NAME heading.
#
# These replacements remove them.

set OUTlSelfLess {}
