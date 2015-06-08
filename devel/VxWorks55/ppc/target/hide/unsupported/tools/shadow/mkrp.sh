#!/bin/sh
#
# mkrp -- create relative pathname reference from one directory to another
#
# modification history
# --------------------
# 01a,11jul90,rbr  Written.
#
# SYNOPSIS
# \fBmkrp \fIdir1\fP \fIdir2\fP
#
# DESCRIPTION
# \fBmkrp\fR creates a relative pathname (\fIi.e.\fR, a name not beginning
# with \f(CW/\fR) that would reference \fIdir1\fR if the current working
# directory were \fIdir2\fR.
# .PP
# \fBmkrp\fR fails if \fIdir2\fR is missing or if there is no
# way to reference \fIdir1\fR therefrom.
# EXAMPLES
# .DS
# .nf
# 
# .ft CW
# % mkrp / /bin
# ..
# % mkrp /usr/lib /usr/bin
# ../lib
# % mkrp usr/adm etc
# ../usr/adm
# % mkrp /fs1/a /fs2/b
# ../../fs1/a
# .ft R
# 
# .fi
# .DE
# AUTHOR
# Roger Rohrbach
#*/

echo $1 $2 |

awk '
{
    if (NF < 2 || index($1, "/") == 1 && index($2, "/") != 1)
	exit(1);

    split($1, object, "/");	# the directory being referenced
    split($2, subject, "/");    # the directory from which to reference

    # Determine common prefix of the two pathnames.
    for (branch = 1; branch == 1 || object[branch] || subject[branch]; ++branch)
	if (subject[branch] != object[branch])
	    break;

    # Get the remainder of object (ends up as tail of result).
    for (depth = branch; object[depth]; ++depth)
    {
	if (!tail)
	    tail = object[depth];
	else
	    tail = tail "/" object[depth];
    }

    # Count the remaining directory components of subject.
    for (depth = branch; subject[depth]; ++depth)
	;

    if (depth > branch)
    {
	# Subject is either a subdirectory of, or shares a parent
	# directory with, object.  Create a ".." reference of depth <depth>.

        for (depth = depth - branch; depth > 0; --depth)
	    if (!head)
		head = "..";
	    else
	        head = "../" head;
    }
    else
    {
	# Subject is either equal to or is a parent directory of object.
	head = ".";
    }

    printf("%s", head);

    if (length(tail))
	printf("/%s", tail);

    printf("\n");
}'
