/* sample.c - a sample source module */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,15sep93,jmm  incorporated changes from the Porting Kit
01g,25may93,jdi  more tweaks for 5.1.
01f,23may93,jdi  updated for BSP kit 1.1 (5.1).
01e,29feb92,jdi  updated for BSP kit.
01d,17aug91,jdi  more changes.
01c,08may90,jdi  style, spelling & format; plus new information.
01b,02nov83,dnw  fixed spelling.
01a,01nov83,dnw  written.
*/

/*
SYNOPSIS
  For libraries, omit this section!
          It will be generated from the routine descriptions.
  For routines, the declaration (automatically generated).
  For tools, the command-line syntax.

DESCRIPTION
This is a sample of a properly formatted source module.  A module formatted
in this way can have its manual entry generated automatically by the tool
"mangen".  Mangen pulls the documentation from the source module's comment
blocks and subroutine declarations.

The manual entry thus created is nroff/troff source placed in the file
<name>.nr where <name> is the basename of the source module.

For libraries, a synopsis manual entry is generated for the library as a
whole, and a manual entry is generated for each subroutine in the library.
The synopsis section consists of the titles and calling sequence of
each routine, followed by a formatted version of this initial block of
comments.  See the subroutines below for a more detailed discussion of how
manual entries for individual subroutines are generated.  The following
command would be used to generate a "library" type manual entry for this
module:
.CS
    % mangen sample.c
.CE
For tools, including shellscripts, the generated manual entry consists of a
formatted version of this initial block of comments.  The following command
would be used to generate a "tool" type manual entry for this module:
.CS
    % mangen -s sample.c
.CE
See the commentary in mangen or its manual entry for more information
about the use of its various command-line options.

FORMAT
Leading spaces in this section are meaningful in the following way:  nroff
will fill/justify everything except lines that begin with a space.  This
is useful since you can easily enter simple displays just by indenting:

    line one
    line two
    line three

You can use tabs fearlessly because the entire source module is initially
expanded to spaces before processing.  Tabs are expanded to every 8
columns.

However, remember that the width of the man page screen display is only 60
characters.  If text in an indented display like the one above should
exceed this, it will appear ragged and difficult to read.  More elaborate
indentation formats, such as hanging indents, should be created using
nroff/troff requests or macros.

Nroff/troff requests and macros are passed through transparently.  The WRS
formatting macros, wrs.an, are simply extensions to the standard UNIX man
macros.  For an example of macro use, see the itemized list using the .IP
macro below under ERRORS.

Blank lines are preserved in the resulting output.

SUBHEADINGS
Any subheadings can be specified.  A subheading is designated by a line
containing an upper-case letter in column one and no lower-case letters
anywhere on the line.  Alternatively, a subheading can also be followed by a
colon and then followed by lower-case text as in the example below ("EXAMPLE:")

The colon will be stripped for consistency.  Only one space character
should follow the colon.

The subheading DESCRIPTION will automatically be inserted if one
is not supplied at the top of the first section.

EXAMPLE: Example sections are frequently very useful.

SECTION: 1
By default, mangen assigns the manual entry to section "1",; however,
the special keyword SECTION can be used to explicitly assign a different
section number.

RESTRICTIONS
The module's initial comment blocks must be arranged as follows:

    1st block:  module title line
    2nd block:  mod history
    3rd block:  descriptive text

For the mod history and all descriptive text blocks, the open-comment
(slash, star) and close-comment (star, slash) must be entered as the
only characters on a line.

Remember that C comment indicators cannot be imbedded within text, because
C does not allow nested comments.  However, to depict a comment block in
an example, replace the start (*) with an ampersand (@); for example:
.CS
    /@ an imbedded comment block @/
.CE

ERRORS
In the process of generating the manual entry, a few checks are made:
.IP (1) 4
The module name (the 1st word of the title line) must
exactly match the source file name.
.IP (2)
A module description section (a block comment following
the mod history) must be present.
.IP (3)
The subroutine name (the 1st word of the routine title line)
must exactly match the actual declared routine name.
.LP
All such errors are reported and will abort processing.  Unfortunately,
there is no way to get a status return from an awk program; thus the
mangen script tries to continue, with weird results.

INTERNAL
A special subheading INTERNAL may be included.
This section will not appear in the generated manual entry.

BUGS:
What we really want is not "mangen" that generates the manual entry from
the program, but rather "progen" that generates the program from the manual
entry.

SEE ALSO: mangen (for a complete reference to keywords and
command-line options),
.pG "Coding Conventions,"
.I "Guidelines for Documentation"
(Wind River Systems)
*/


#include ...

/*******************************************************************************
*
* gronk - a sample routine
*
* This is an example of a properly formatted routine.  If mangen is run as a
* library (default), then in addition to the module manual entry, a manual
* entry will be generated for each individual routine in the module, except
* for those routines explicitly declared LOCAL ("static") or designated
* "no-manual" (for how to do this see the zonk() routine in the source for
* sample.c).
*
* The rules are much the same as for the module description section above.
* The routine title line is followed by the routine description section.
* The leading "* " (asterisk plus space) at the beginning of each of these
* lines will be stripped.  The subheading DESCRIPTION will automatically be
* inserted if one is not supplied.  Any other subheadings can be inserted as
* described above.  NOTE: Do not include a SYNOPSIS section since it will be
*
* ARGUMENTS
* There must be a comment for each argument to the subroutine.  Again, as
* with any "no-fill" display, care must be taken that the total line length
* does not exceed 60 characters.
* generated automatically from the routine declaration below!
*
* RETURNS: Every manual entry for a routine should have a RETURNS section.
* If there is no return value (routines declared "void"), simply put N/A.
*
* INTERNAL
* A special subheading INTERNAL may be included.  This section will not
* appear in the generated manual entry.  This is a good place to put
* internal or algorithmic information that the user need not know (and
* therefore should not know!).
*/

int gronk
    (
    int a,  /* the first arg */
    int b,  /* the second arg */
    int *c  /* pointer where to return the "c" value */
    )

    {
    ...
    }

/*******************************************************************************
*
* bonk - another routine
*
* This is another global routine...
*
* RETURNS: N/A
*/

void bonk
    (
    int a  /* the arg */
    )

    {
    ...
    }

/*******************************************************************************
*
* honk - yet another routine, but this one is local
*
* This is a local routine.  No manual entry will be generated for this
* routine.
*
* RETURNS: N/A
*/

LOCAL void honk
    (
    int a  /* the arg */
    )

    {
    ...
    }

/*******************************************************************************
*
* zonk - a non-local routine without a manual entry
*
* Sometimes it is desirable to suppress the manual entry for a non-LOCAL
* routine.  This can be done by including the the word NOMANUAL
* anywhere in its description.
*
* This routine is not declared LOCAL.  However, no manual entry will
* be generated because of the following statement:
*
* NOMANUAL
*
* RETURNS: N/A
*/

void zonk
    (
    int a  /* the arg */
    )

    {
    ...
    }

