# OldMangen.tcl - support tables for refgen: conversion from roff-based source
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02i,29oct01,wsl  merge in .CS/.CE bug fix from T2 VOB:
#                  fix bug allowing strings inside .CS/.CE to affect
#                  formatting
# 02h,09oct01,wsl  fix handling of EXPANDPATH
# 02g,21aug01,wsl  fix infinite loop bug
# 02f,08aug01,wsl  allow tabs for spacing in tables SPR 67239
# 02e,06oct99,dgp  correct processing of marker lists and tables with
#                  multi-line cells
# 02d,05oct99,dgp  correct conversion of .nf...fi to \ts...\te for
#                  single-spaced list
# 02c,26aug99,dgp  revise handling of multi-cell tables, fix macro and
#                  multi-line synopsis conversion
# 02b,16jul99,dgp  change table processing in refPrevCleanup to include
#                  delimiters for multi-line tables
# 02a,14jul99,dgp  correct handling of bullet lists, fix .iB, .br, \f1, .sT
# 01z,12jul99,dgp  fix misc substitutions (.iT, .LP, .PP)
# 01y,12jul99,dgp  fix substitutions, including .I, .pG, .tG, .iB to \tb,
#		   kill LINTLIBRARY, BSP-specific notes
# 01x,10jul99,dgp  fix list handling for both \ml...\me and \is...\ie, including 
#		   generating marker lists when appropriate, spacing and
#                  .SS and .SH terminations
# 01w,19mar99,fle  fixed a problem with tables and multiline cells (newline
#                  characters not handled correctly)
# 01v,13mar99,jdi  Changed boilerplate .nT and .sA text about BSP-specific
#                  reference entries.
# 01u,11mar99,fle  fixed a problem with list handling just before the end of
#                  routine / lib declaration
# 01t,26feb99,fle  fixed end of list handling
# 01s,26feb99,fle  fixed a problem with .iP list end detection
# 01r,22feb99,fle  made it handle the n-roff \e mark
# 01q,28jan99,fle  made all .iP marks be always converted into list, not
#                  tables anymore
# 01p,02dec98,fle  modified .iP handling
# 01o,05nov98,fle  added possibility of having also an end of table separator
# 01n,16oct98,fle  fixed another trouble with tables processing
# 01m,02oct98,fle  fixed a problem with tables processing
# 01l,28sep98,fle  took .TH markup into account
#                  + took care of .nb markup
# 01k,22sep98,fle  added markup for .nT and .sA
# 01j,27jul98,fle  added .iT and .rL to n-roff translated elements
# 01i,16jul98,rhp  Recognize killable nroff even if numbers have neg sign
# 01h,22jun98,rhp  Enhanced for target.nr conversion, and added tolerance
#                  for common .iP spacing error
# 01g,02jun98,rhp  Improved .[iI]P converter
# 01f,02jun98,rhp  Increased tolerance of ill-formed EXPAND
# 01e,28may98,rhp  Added code to fix entrenched incorrect usage of EXPAND
#                  during conversion; fixed boundary condition in iP conversion
# 01d,26may98,rhp  Fixed initialization bug in refPrevCleanup.
# 01c,19may98,rhp  Corrected relative-to-absolute conversion for EXPANDPATH
# 01b,18may98,rhp  Added refPrevCleanup to eliminate EXPANDPATH redundancy.
# 01a,15may98,rhp  Initial check-in

# DESCRIPTION
# This file defines refgen replacements for certain roff constructs that were
# allowed in the original mangen.

################################################################################
#
# ALTmg - HTML replacements for roffish constructs embedded in source
#
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).
# Order dependencies are OK; this list is processed in the order it appears
# below (topmost element first).
#
# The entire list is applied after refPrevIter (below) so that these
# transformations can clean up stray bits of formatting that might be
# meaningful to refPrevIter (but weren't actually used).

set ALTmg "
    {
        {($SYNTAX(libCn)|$SYNTAX(rtCn))\\.tS}
        {\\1\\\\ss}
        {Start of code block, permitting internal markup.}
    } 
    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.tE}
        {\\1\\\\se}
        {End of code block, permitting internal markup.}
    } 

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\[ \t]*\\.ft\[^\n]*\n}
	{\n}
	{Font specification. Kill}
    }

    {
        {($SYNTAX(libCn)|$SYNTAX(rtCn))\\.CS\[^\n]*\n}
        {\\1\\\\cs\n}
        {Start of code block, normal size, no internal markup.}
    } 
    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.CE}
        {\\1\\\\ce}
        {End of code block, normal size, no internal markup.}
    } 

    {
        {($SYNTAX(libCn)|$SYNTAX(rtCn))\\.bS}
        {\\1\\\\bs}
        {Start of code block, reduced size, no internal markup.}
    } 

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.bE}
        {\\1\\\\be}
        {End of code block, reduced size, no internal markup.}
    } 

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\\\\\"(\[^\n]*)}
	{\\1\\\\\"\\2}
	{In-docn full-line comment.}
    }

    {
        {($SYNTAX(libCn)|$SYNTAX(rtCn))\\.br(\[ ]*)\n}
        {\\1}
        {Break command}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))(\\.nf *)\n}
	{\\1\\\\ts\n}
	{Ask for non-spacing paragraph; convert to table.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))(\\.fi *)\n}
	{\\1\\\\te\n}
	{Ask for end of non-spacing paragraph; convert to table.}
    }

    {
	{(^|$SYNTAX(libCn)|$SYNTAX(rtCn))\'\\\\\" t *\n}
	{}
	{Roffish mystery; kill.}
    }

    {
	{\\\\%}
	{}
	{Roffish do-not-hyphenate; kill : refgen already takes care of it.}
    }

    {
	{\\\\e}
	{\\\\}
	{Roffish backslash; kill e character}
    }

    {
	{(^|$SYNTAX(libCn)|$SYNTAX(rtCn))(\\.(sp|vs|PD|RS|RE)\[^\n]*)\n}
	{\\1\n}
	{Roffish vertical spacing or indent amelioration; kill.}
    }

    {
	{(^|$SYNTAX(libCn)|$SYNTAX(rtCn))(\\.ne\[^\n]*)\n}
	{\\1\n}
	{Roffish indent amelioration; kill.}
    }

    {
	{(^|$SYNTAX(libCn)|$SYNTAX(rtCn))\\.so wrs.an *\n}
	{}
	{Roffish macro include; kill.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TH \"?(\[^\"]*)\"? T \"(\[^\"]*)\" \"(\[^\"]*)\" \"(\[^\"]*)\"}
	{\\1\\\\TITLE \\2 - \\3}
	{Roffish header settings}
    }

    {
	{\\\\\\(mi}
	{ - }
	{roffish subtract symbol, also used for an empty cell}
    }

    {
	{\\\\\\(rg}
	{\\\\reg }
	{registered mark}
    }

    {
	{\\\\\\(co}
	{\\\\copy }
	{copyright mark}
    }

    {
	{\\\\\\&}
	{}
	{Roffish zero-width char; kill.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.SH \"?(\[^\n\"]*)\"?}
	{\\1\\2}
	{Explicitly-marked heading. No markup; rely on all-caps convention.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.aX \"?(\[^\n\"]*)\"?}
	{\\1`\\2'}
	{Roffish keyword output-cum-name; just preserve name.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.SS \"?(\[^\n\"]*)\"?}
	{\\1\\\\sh \\2}
	{Explicitly-marked subheading.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\[rR]L}
	{\\1\\\\rl}
	{Separation line}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\[iI]T \"?(\[^\n\"]*)\"?}
	{\\1\\2}
	{One line indentation mark used for routine lists; kill}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\[sS]T \"?(\[^\n\"]*)\"?}
	{\\1\\\\sts\\1\\2\\1\\\\ste}
	{C++ class library subtitle}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\[LP]P}
	{\\1\\1}
	{Explicit paragraph break.}
    }

    {
       {($SYNTAX(libCn)|$SYNTAX(rtCn))\\.I \"?(\[^\n\"]*)\"?(\[^\n\"]*)}
       {\\1\\tb \\2 \\3}
       {Italics.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.IR \"?(\[^\n\"]*)\"? \"?(\[^\n\"]*)\"?}
	{\\1<\\2>\\3}
	{Italics, then Roman.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.B \"?(\[^\n\"]*)\"?(\[^\n\"]*)}
	{\\1`\\2'\\3}
	{Boldface.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.BR \"?(\[^\n\"]*)\"? \"?(\[^\n\"]*)\"?}
	{\\1`\\2'\\3}
	{Boldface, then Roman.}
    }

    {
	{\\\\f2(\[^\\\\]*)\\\\f\[1P]}
	{<\\1>}
	{Italics.}
    }

    {
	{\\\\f3(\[^\\\\]*)\\\\f\[1P]}
	{`\\1'}
	{Boldface.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.pG \"?(\[^\n\"]*)\"?}
	{\\1\\tb VxWorks Programmer's Guide: \\2 }
	{Shorthand for VPG xref.}
    }

    {
	{/\\* LINTLIBRARY \\*/\n}
	{\n}
	{call to lint : kill}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.tG \"?(\[^\n\"]*)\"?}
	{\\1\\tb Tornado User's Guide: \\2 }
	{Shorthand for TUG xref.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.iB \"?(\[^\n\"]*)\"?}
	{\\1\\tb \\2 }
	{Bibliographic entries in BSPs.}
    }

    {
	{\\\\\\(mu}
	{x}
	{Multiplication symbol; replace by x to avoid font dependencies.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.T&\[ \t]*}
	{\\1.TE\\1\\1.TS;}
	{Change of table format.  Usage appears to make it simpler to
        treat as end of one table, start of another; we do that here.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))(\[\\_=]+\[\\_ =|\t]*\n)(\\.TE\[^\n]*\n)}
	{\\1------------------------------\n\\3}
	{table end delimiter just before a .TE}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))(\\.TS\[^_]*)($SYNTAX(libCn)|$SYNTAX(rtCn))\[\\_=]+\[\\_ =|\t]*\n}
	{\\1\\2\\3------------------------------\n}
	{Table heading delimiter, but only after .TS}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TS\[^;]*;\[ \t]*\n}
	{\\1\\ts\n}
	{Start of table. We preserve the tbl format section so cleanup
         can extract spanning info (needed for target.nr).}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TE\[ \t]*\n}
	{\\1\\te\n}
	{End of table.}
    }

    {
	{($SYNTAX(libCn)|$SYNTAX(rtCn))\\.nT\[ \t]*\n}
	{\\1NOTE : This is a generic page for a BSP-specific routine; this\n\*\
	 description  contains general information only. To determine if this\n\*\
	 routine is supported by your BSP, or for information specific to your\n\*\
	 BSP's version of this routine, see the reference pages for your BSP.\n}
	{BSP specific note}
    }

    {
	{,? ?\\.sA}
	{ and BSP-specific reference pages for this routine.}
	{See also BSP specific doc pages}
    }

"




###############################################################################
#
# refPrevIter - iterative conversion-specific work
#
# Iteratively convert tabs within tables to | so that this
# replacement doesn't have to be applied indiscriminately later.
#
# Convert the following embedded-roff through iteration,
# due to their setting local state and other complexities:
# \cs
#   .IP ...  { .LP | .lE | HEADING | .SS | .SH }
#   .iP ...  { .LP | .lE  | HEADING | .SS | .SH }
# \ce
# NOTE: We don't think there are any examples of .lE but are
# leaving it in just in case.
#
# RETURNS: Converted text.

proc refPrevIter {txt} {

    upvar SYNTAX SYNTAX

    set zz {}
    set iList 0
    set mList 0
    set match {}

    # Start by replacing tabs with |, but only within .TS/.TE.
    # This should work even if tables are nested, since there must be an
    # equal number of start and end markers within the outer table.

    while { [regexp -indices "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TS\[ \t\n]+"\
                    $txt tableStart] == 1 &&
            [regexp -indices "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TE\[ \t]*\n"\
                    $txt tableEnd] == 1 } {

        set indexStart [lindex $tableStart 1]
        set indexEnd   [lindex $tableEnd   0]
	if { $indexStart > $indexEnd } {
            puts stdout "Error: Unmatched table end (.TE)"
	    regsub "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TE\[ \t]*(\n)" $txt \
		   "\n" txt
	    continue
	}	    

        set preTable  [string range $txt 0 $indexStart]
        set inTable   [string range $txt [expr $indexStart + 1] \
                                         [expr $indexEnd - 1]]
        set postTable [string range $txt $indexEnd end]

        regsub -all "\t" $inTable { | } inTable

        set txt {}
        append txt $preTable $inTable $postTable

        # now mark the one set of markers

        regsub "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TS(\[ \t\n]+)" $txt \
		{\1.TSZzzZ\2} txt
        regsub "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TE\[ \t]*(\n)" $txt \
		{\1.TEZzzZ\2} txt
    }

    # restore marked .TS/.TE so that other processing can take place

    regsub -all "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TSZzzZ(\[ \t\n]+)" $txt \
		{\1.TS\2} txt
    regsub -all "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.TEZzzZ\[ \t]*(\n)" $txt \
		{\1.TE\2} txt

    # tabs fixed -- now proceed with the more complex processing

    while { [regexp -indices "($SYNTAX(libCn)|$SYNTAX(rtCn))\\.\[Ii]P\[ \n]*"\
		    $txt indIP indPfx] == 1 } {

	set i [lindex $indIP 0]
	set e [lindex $indIP 1]

	# shortcut hereafter: assume same continuation prefix throughout

	set PFX [eval "string range \$txt $indPfx"]
	regsub -all {[.*+?()|{}^$]} $PFX {\\&} ePFX ;# for use in regexps

	# move leading text before item to result, removing extra \n's

	if { [regexp -indices ^($ePFX) $txt match] } {
            append zz [string range $txt [string length $PFX] [expr $i - 1]]
        } else {
            append zz [string range $txt 0 [expr $i - 1]]
        }

	set txt [string range $txt $e end]

	# start or continue itemized or marker list

	if {$iList|$mList} {
            append zz "$PFX"   ;# usually includes newline...
        } else {
            if [regexp {^ +([-0-9.()]+)} $txt match] {
                append zz "$PFX\\ml$PFX"
                set mList 1
            } else {
                append zz "$PFX\\is$PFX"
                set iList 1
            }
	    set ipFs {}; set ipFe {}
	}

	# Extract data from arguments in remainder of .[iI]P line
	# (too much variability to do this in same regexp as found .[iI]P)

	set e [expr [string first "\n" $txt] - 1]
	set ipArgs [string trim [string range $txt 0 $e]]
	set txt [string range $txt [expr 1 + $e] end]

	set ipf {}		;# if no match, we want empty string, not unset
	set ipMark {-}		;# if no argument, user wanted dashed list

	if { {"} == [string index $ipArgs 0] } {
	    regexp {"([^"]+)" *"?([0-9]*)"? *([123]?)} $ipArgs \
		    {} ipMark {} ipf 
	} else {
	    regexp {([^ ]+) *"?([0-9]*)"? *([123]?)} $ipArgs \
		    {} ipMark {} ipf 
	}

        # if ipArgs is incorrectly set to "P", change to "-"

        if { {P} == $ipMark } {
            set ipMark "-"
        }

	if { [string length $ipf] > 0 } {

	    switch $ipf {

		2 {
		    # put the .iP title in italics

		    set ipFs {<}
		    set ipFe {>}
		}

		3 {
		    # put the .iP title in bold

		    set ipFs {`}
		    set ipFe {'}
		}

		default {
		    # don't set any font spec on .iP title

		    set ipFs {}
		    set ipFe {}
		}
	    }
	}

        if {$mList} {
            append zz "\\m $ipFs$ipMark$ipFe$PFX"
        } else {
            append zz "\\i $ipFs$ipMark$ipFe$PFX"
        }

	# Whew.  Now get actual indented para text.
	# Current sequence of indented paras may end in:
	#   a- another .[iI]P, 
	#      in which case we loop without changing inList et al
	#   b- .LP or .lE or .SH or .SS or an all-caps HEADING, or end of
	#      library-description or routine-description block,
	#      in which case we reset inList et al and loop for
	#      more .[iI]P sequences
	#   c- end of string, in which case we're gloriously done

        # frequent error in source: varying numbers of spaces after 
        # continuation prefix. Tolerate and correct in conversion
        # by allowing spaces after ePFX below.

	# skip anything inside .CS/.CE

	while {[ regexp -indices \
		"(($ePFX) *(\\. *\[iI]P|\\.lE|\\.LP|\\.S\[SH]|\[A-Z]\[A-Z ]*(\n|:\[^\n]*\n)))|$SYNTAX(libEnd)|$SYNTAX(rtEn)" \
		$txt iEnd ]} {
	    set foundStart [regexp -indices "$ePFX *\\.CS" $txt cStart]
	    set foundEnd   [regexp -indices "$ePFX *\\.CE" $txt cEnd]

	    if { $foundStart && $foundEnd && \
		    [lindex $cStart 0] < [lindex $iEnd 0] && \
		    [lindex $cEnd 0]   > [lindex $iEnd 0] } {
		set cText [string range $txt 0 [lindex $cEnd 1]]
		set txt [string range $txt [expr [lindex $cEnd 1] + 1] end]
		append zz $cText
	    } else {
		break
	    }
	}

	if {0 == [ regexp -indices \
		"(($ePFX) *(\\. *\[iI]P|\\.lE|\\.LP|\\.S\[SH]|\[A-Z]\[A-Z ]*(\n|:\[^\n]*\n)))|$SYNTAX(libEnd)|$SYNTAX(rtEn)" \
		$txt iEnd ]} {

	    # EOS; grab trailer, then conversion done.

	    set txt [string range $txt [string length $PFX] end]
            if {$mList} {
                append zz "$PFX$txt\\me$PFX"
            } else {
                append zz "$PFX$txt\\ie$PFX"
            }
	    set txt {}
	    break

	} else {

	    set hit [eval "string range \$txt $iEnd"]

	    if {[regexp "$PFX\\.\[iI]P" $hit]} {

		# another IP; loop.
		# head of loop will accumulate indented-para text

		continue
	    } 

	    set cEnd [expr [lindex $iEnd 0] - 1]
	    set ctxt "[string range $txt 0 $cEnd] $PFX\\ie$PFX"

            if {[regexp "$ePFX\\.((lE)|(LP))" $hit]} {

		# list is explicitly closed

                if {$iList} {
                    set ctxt "[string range $txt 0 $cEnd]$PFX\\ie$PFX"
                } else {
                    set ctxt "[string range $txt 0 $cEnd]$PFX\\me$PFX"
                }

            } else {

		# list is closed by .SH, .SS, or HEADING

                if {[regexp ($SYNTAX(libEnd)|$SYNTAX(rtEn)) $hit]} {
                    if {$iList} {
                        set ctxt "[string range $txt 0 $cEnd]$PFX\\ie$PFX"
                    } else {
                        set ctxt "[string range $txt 0 $cEnd]$PFX\\me$PFX"
                    }

                # implicit end of indented list

                set ctxt "$ctxt$hit"

                } else {

                    # implicit end of indented list

                    if {$iList} {
			# test change
                        set ctxt "[string range $txt 0 $cEnd] \\ie$PFX"
                    } else {
                        set ctxt "[string range $txt 0 $cEnd] \\me$PFX"
                    }

                    set ctxt "$ctxt$hit"
                }
            }

	    # kill continuation-prefix at start of ctxt

            if {[regexp ^$ePFX $ctxt]} {

                # eliminate leading \n if necessary

                set ctxt "[string range $ctxt [string length $PFX] end]"
            }

	    append zz $ctxt
	    set iList 0; set mList 0; set ipFs {}; set ipFe {}

	    if { [regexp \
		  "$SYNTAX(libEnd)|$SYNTAX(rtEn)|\[\.SS\]|\[\.SH\]|\[A-Z]\[A-Z ]*
		  (\n|:\[^\n]*\n)" $hit] } {

                # the list has been stopped by a heading or title or 
                # an end of routine / library pattern. Don't add the PFX pattern
                # it could be unneeded and disturbing

                set txt [string range $txt [expr [lindex $iEnd 1] +1] end]
	    } else {
		set txt "$PFX[string range $txt [expr [lindex $iEnd 1] +1] end]"
	    }
	    continue
	}
    }

    # do some processing on the item lists in order to transform them in
    # table if item title is small enough

    set zz [refListToBullet $zz]

    # now get back the remaining text and return it

    append zz $txt
    return $zz
}

################################################################################
#
# refListToBullet - transforms an itemised list in bullet list if needed
#
# This procedure takes <text> and makes all the itemised lists that have "-" for
# title, be bullet lists.
#
# SYNOPSIS
#   refListToBullet <text>
#
# PARAMETERS
#   text : the text containing lists to be transformed
#
# RETURNS: the transformed text
#
# ERRORS: N/A
#

proc refListToBullet { text } {

    global SYNTAX		;# language specific syntax array

    set processedText ""
    set trailingText $text
    set listIx [string first "\\is" $text]

    # isolate the list

    if { $listIx == -1 } {
	return $text
    } else { 
	set processedText [string range $text 0 [expr $listIx -1]]
    }

    # process the text while there are lists in it

    while { $listIx != -1 } {

	set needBullet 1
	set listEndIx [string first "\\ie" $trailingText]

	# if no end of list was encountered, set it to the end of the text

	if { $listEndIx == -1 } {
	    set listEndIx [expr [string length $trailingText] - 1]
	} else {
	    set listEndIx [expr $listEndIx + 3]
	}

	# now isolate the list

	set listContent [string range $trailingText $listIx [expr $listEndIx-1]]
	set trailingText [string range $trailingText $listEndIx end]
	set needBullet 1
	set trailingList $listContent

	# now get all the list titles

	while { [regexp -indices "\\\\i\[ ]+(\[^\n]*)\n" $trailingList \
			{} titleIx]} {

	    if { [string range $trailingList [lindex $titleIx 0] \
			 [lindex $titleIx 1]] != "-" } {

		set needBullet 0
	    }

	    # let's continue on next title

	    set trailingList [string range $trailingList [lindex $titleIx 1] \
				     end]
	}

	# convert into table if needed

	if { $needBullet } {
	    regsub -all "\\\\is" $listContent "\\\\ibs" listContent
	    regsub -all "(\\\\i -)($SYNTAX(rtCn)|SYNTAX(libCn))*" \
			$listContent "\\\\ibi " listContent
	    regsub -all "\\\\ie" $listContent "\\\\ibe" listContent
	}

	append processedText $listContent

	set listIx [string first "\\is" $trailingText]

	if { $listIx != -1 } {
	    append processedText [string range $trailingText 0 \
					 [expr $listIx - 1]]
	} else {
	    append processedText $trailingText
	}
    }

    return $processedText
}


###############################################################################
#
# refPrevCleanup - final conversion-specific work
#
# Two kinds of cleanup remain at this point: dealing with EXPAND,
# and dealing with fancy tables from `target.nr' source.
#
# PROCESSING EXPAND
# The new `EXPAND' is backward compatible with the old `EXPAND' with one
# exception: for some reason, the old `EXPAND' required a routine-style
# line prefix even for libraries.  This should be corrected in the
# source, but it is corrected here anyway.
#
# PROCESSING TABLES
# \sh Tab Character
# In mangen days, some tables were permitted to use the tab character
# as a comment delimiter, instead of switching to the visible character `|'.
# This happened at least in `target.nr' files.  A loop in `refPrefCleanup'
# searches for tabs within \ts ... \te and converts them to ` | '.
# 
# NOTE: this could cause problems if people ever used tabs for alignment
# while | was the tab delimiter!  If that situation arises, we may need
# to make this more intelligent, perhaps even loking for the nroff tab 
# definition.
#
# FURTHER NOTE: this did cause problems.  The substitution has been moved to
# the earlier proc `refPrevIter'.
# 
# \sh Multiline Cells
# Multiline cells in mangen tables are delimited by T{ and T} followed
# by newlines.  We must find such cells, get rid of the beginning and end
# markers, and escape all newlines in the cell.
#
# SYNOPSIS
#   refPrevCleanup <txt>
#
# PARAMATERS
#   txt : text in process of conversion, requiring final cleanup
#
# RETURNS: Cleaned-up, converted text.
#
# ERRORS: N/A
#

proc refPrevCleanup { txt } {

    upvar SYNTAX SYNTAX

    # fix EXPAND lines that had wrong prefix in original:

    set fTxt {}
    while {1 == [regexp -indices\
	    "($SYNTAX(libCn)|$SYNTAX(rtCn))(EXPAND \[^\n]*)\n" \
	    $txt iHit {} iExp]} {
	set preTxt  [string range $txt 0 [expr [lindex $iHit 0] - 1]]
	set exp [eval string range \$txt $iExp]
	set txt [string range $txt [expr [lindex $iHit 1] + 1] end]

	# Note! Ordering of the two alternatives in regexp below
	# is important, and assumes rtCn is unlikely to be substring of libCn
	# unless the two are identical.

	regexp "($SYNTAX(rtCn)|$SYNTAX(libCn))" $txt pfx
	append fTxt "$preTxt$pfx$exp\n"
    }
    append fTxt $txt

    # clean up fancy roffish tables

    set txt {}
    while {[regexp -indices\
	    "($SYNTAX(libCn)|$SYNTAX(rtCn))\\\\ts *\n" $fTxt iHit]} {

	# there is an embedded table. Let's process it.

	set preTxt [string range $fTxt 0 [expr [lindex $iHit 0] -1]]

	if {![regexp -indices\
		"($SYNTAX(libCn)|$SYNTAX(rtCn))\\\\te *\n" \
		[string range $fTxt [lindex $iHit 0] end] iEnd]} {
	
	    # it was impossible to find the end of the table. Inform user there
	    # was no end-table markup

	    refErrorExit "Table boundary not found in conversion."

        } else {

	    set tend [expr [lindex $iHit 0] + [lindex $iEnd 1]]
	    set tbl [string range $fTxt [lindex $iHit 0] $tend]
	    set fTxt [string range $fTxt [expr $tend + 1] end]

	    # extract tbl format section. To do so, keep everything in the table
	    # but the table declaration (\\te), then look for the first "."
	    # at the end of a line. If it is before the first | character
	    # then there is table formatting. Remove it.

	    set haveTblFormat 0
	    set firstPipePos [string first "|" $tbl]
	    set firstEndDotPos [string first ".\n" $tbl]

            if { ($firstEndDotPos != -1) && ($firstPipePos != -1) &&
	         ($firstEndDotPos < $firstPipePos) } {

		# there is table formatting. Let's remove it and keep it in the
		# tf variable. Also set the haveTblFormat to 1.

		set haveTblFormat 1
		regexp -indices "(\\ts\[^\n\]*\n)" $tbl ixes
		set tf [string range $tbl [expr [lindex $ixes 1] + 1] \
				     $firstEndDotPos]
		set tblA [string range $tbl 0 [lindex $ixes 1]]
		set tblB [string range $tbl [expr $firstEndDotPos + 2] end]
		set tbl "$tblA$tblB"
	    } else {

		set tf 1

	    }

	    # Find and fix multiline cells
	    # (Do this first, so we can escape any special chars within them)
            # NOTE: it is assumed that the T{ T} cell is the last in the row. 

	    #set nTbl {}

            while { [regexp "(.*)(\n\[^\n\]*)T\{\n*(\[^\}\]*)T\}\n*(.*)" $tbl \
	                    match preCell preRow inCell postCell] > 0 } {
	        # remove any last newlines from multi-cell
		regsub "\n+$" $inCell "" tmpCell

		# create empty cells for each row
		regsub -all {[^|]+} $preRow " " emptyCells

		# put empty cells at start of every row
                regsub -all "\n" $tmpCell "\n$emptyCells" inCell

                # reconstruct table
		set tbl {}
                append tbl $preCell $preRow $inCell "\n" $postCell
	    }

	    #while {-1 != [set mli [string first "T\{\n" $tbl]]} {

		# figure out how many columns the table has

		#if {$tf != 1} {
		#    regexp "($SYNTAX(libCn)|$SYNTAX(rtCn))\[^\n\]+\\." $tf cols
		#    set num [regsub -all " " $cols "\|" out]
		#} else {
		#    set num 1
		#}

		# locate multiline cells

		#append nTbl [string range $tbl 0 [expr $mli - 1]]
		#set tbl [string range $tbl $mli end]

		#if {-1 == [set mle [string first "T\}" $tbl]]} {
		#    refErrorExit "roff multiline table cell not terminated"
		#}

		#set mlc [string range $tbl \
		#		0 [expr $mle + [string length "T\}"] - 1]]
		#set tbl [string range $tbl [string length $mlc] end]

		# get rid of embedded comment start whenever we are in a multi
		# line cell, and if there is a comment at the beginning of
		# each line

		#regsub -all "$SYNTAX(rtCn)" $mlc { } mlc
		#regsub -all "$SYNTAX(libCn)" $mlc { } mlc

		# get rid of all the multi-line cell delimiters. Don't invert
		# order with previous line

		#regsub -all "(T\{|T\})" $mlc {} mlc
		#regsub -all "\[\n|\t]" $mlc {\\\\&} mlc

                # add new multi-line cell delimiters.

		#set mlcSt {}

		#switch $num {
		#    1 {
		#	while { [string length $mlc] > 75 } {
		#	    regexp -indices " \[^ ]*$" [string range $mlc 0 75] word
		#	    append mlcSt [string range $mlc 0 [lindex $word 0]]
		#	    append mlcSt "\n"
		#	    set mlc [string range $mlc [expr ([lindex $word 0]+1)] end]
		#	}  
		#    }
		#    2 {
                #        while { [string length $mlc] > 65 } {
                #            regexp -indices " \[^ ]*$" [string range $mlc 0 65] word  
                #            append mlcSt [string range $mlc 0 [lindex $word 0]]
                #            append mlcSt "\n        \| "
                #            set mlc [string range $mlc [expr ([lindex $word 0]+1)] end]
                # 	}
		#    }
                #    3 {
                #        while { [string length $mlc] > 55 } {
                #            regexp -indices " \[^ ]*$" [string range $mlc 0 55] word  
                #            append mlcSt [string range $mlc 0 [lindex $word 0]]
                #            append mlcSt "\n         \|         \|"
                #            set mlc [string range $mlc [expr ([lindex $word 0]+1)] end]
                #        }
		#    }
                #    4 {
                #        while { [string length $mlc] > 45 } {
                #            regexp -indices " \[^ ]*$" [string range $mlc 0 45] word  
                #            append mlcSt [string range $mlc 0 [lindex $word 0]]
                #            append mlcSt "\n         \|         \|          \| "
                #            set mlc [string range $mlc [expr ([lindex $word 0]+1)] end]
                #        }
		#    }
                #    5 {
                #        while { [string length $mlc] > 35 } {
                #            regexp -indices " \[^ ]*$" [string range $mlc 0 35] word  
                #            append mlcSt [string range $mlc 0 [lindex $word 0]]
                #            append mlcSt "\n         \|         \|          \|         \| "
                #            set mlc [string range $mlc [expr ([lindex $word 0]+1)] end]
                #        }
		#    }
                # }
                #append mlcSt $mlc
 		#append nTbl $mlcSt
	    #}
	    #set tbl "$nTbl$tbl"

	    # Translate invisible tabs in tables to visible column delimiters
	    # (see NOTE in proc description)

	    # make tabs visible -- removed to allow tabs for formatting
            # in non-nroff tables

	    # regsub -all "\t" $tbl { | } vTbl
	    regsub -all "\t" $tbl { } vTbl

	    # use format section to insert extra cells where roff tbl spanned

	    if {1 == $haveTblFormat} {

		# We skip row indices 0 and 1 because our table-grabber always 
		# starts the table with an empty line, then \ts;
                # tf is one off, w/no analog to \ts, so we extend it to match

		set tf "{} [split $tf \n]"
		set vTbl [refStringSplit $vTbl \n]
		set vte [llength $vTbl]; set i 1     ;# boring loop control
		set tfli [expr [llength $tf] - 1]    ;# last index for tf

		while {$vte > [incr i]} {
		    set vtl [lindex $vTbl $i]
		    if {[regexp -- "------------------------------" $vtl]} {
			break  ;# assume heading is only place w/spans
		    }
		    set tfl [lindex $tf [expr $i<$tfli?$i:$tfli]]
		    set tfl [split $tfl]
		    set vtl [refStringSplit $vtl |]
		    
		    set li -1; set le [llength $tfl]
		    while {$le > [incr li]} {
			if {"s" == "[lindex $tfl $li]"} {
			    set vtl [linsert $vtl $li {}]
			}
		    }

		    set vtl [join $vtl |]
		    set vTbl [lreplace $vTbl $i $i $vtl]
		}

		set vTbl [join $vTbl \n]
	    }

	    append txt "$preTxt$vTbl"
	}
    }
    append txt $fTxt

    return $txt
}
