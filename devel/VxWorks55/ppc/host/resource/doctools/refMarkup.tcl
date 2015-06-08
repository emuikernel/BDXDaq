# refMarkup - private refgen markup
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,19may99,fle  Made the bullet list able to have multi line descriptions
# 01b,27jul98,fle  added markup for iT and rL nroffs
# 01a,15may98,rhp  Initial check-in

################################################################################
#
# refMarkSpans array describes "spanning" markup--
# things with a beginning and an end, and any special stuff in between
# 
# NOTE! These are regexps, in order to implement rules abt esc-seq boundaries
#
# NOTE!! To avoid tripping the limit on number of parens in a regexp, 
# when several of these are combined, the patterns that describe beginnings
# of spans MUST have a newline after the escape sequence.
# The same rule is applied to end-markers for symmetry.

array set refMarkSpans {
    {pre}		"\\\\ss *\n"
    {preEnd}		"\\\\se *\n"
    {stet}		"\\\\cs *\n"
    {stetEnd}		"\\\\ce *\n"
    {smStet}		"\\\\bs *\n"
    {smStetEnd}		"\\\\be *\n"
    {html}		"\\\\hs *\n"
    {htmlEnd}		"\\\\he *\n"
    {tblStart}		"\\\\ts *\n"
    {tblEnd}		"\\\\te *\n"
    {tblCol}		"\\\\tc\[ \t\n]+"
    {tblRow}		"\\\\tr\[ \t\n]+"
    {tblSplit}		"\n([ \|]*[-\+]+[ \|]*)+\n"
    {iList}		"\\\\is *\n"
    {iListEnd}		"\\\\ie *\n"
    {item}		"\\\\i +([^\n]*)\n"
    {mList}		"\\\\ml *\n"
    {mark}		"\\\\m +([^\n]*)\n"
    {mListEnd}		"\\\\me *\n"
    {indent}		"\\\\its"
    {indentEnd}		"\\\\ite"
    {sepLine}		"\\\\rl"
    {rtnsDesc}		"\\\\sts"
    {rtnsDescEnd}	"\\\\ste"
    {bList}		"\\\\ibs"
    {bullet}		"\\\\ibi\[ \t]+(\[^\n]*)\n"
    {bListEnd}		"\\\\ibe"
}


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
# refDisable specifies replacements that disable refgen-specific markup
#
# This list contains patterns and subspecs for regsub -all
# Each list element is in turn a triplet:
#   - regexp to find
#   - replacement spec
#   - comment describing what this array value is for (ignored when running).

set refDisable "
    {
	{\\\\}
	{\\/}
	{Backslash introduces all ref-specific markup.  Disable with
	special-purpose backslash-slash esc seq, so that no backslash
        can wind up adjacent to other escape sequences.}
    }

"
