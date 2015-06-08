#!/bin/sh
#
# shadow \- create private workspace with links to shared files
#
# modification history
# --------------------
# 01d,10feb92,gae  added "exit 0" per DEC.
# 01c,08apr91,rbr  Fixed bug: directory wasn't processed if already existent.
# 01b,16jul90,rbr  Added NOSHADOW; documented -n; accept sym. link as old-path.
# 01a,11jul90,rbr  Written.
#
# SYNOPSIS
# \fBshadow [ -srvn ] [ -x \fIdirectory\fP ] \fIold-path\fP [ \fInew-path\fP ]
#
# DESCRIPTION
# \fBshadow\fR creates a private workspace (directory tree) with
# references via symbolic links to a shared set of files.  Users may
# subsequently replace selected links with private copies of files,
# permitting concurrent development by several programmers on a software
# project, using a minimum of disk storage.
# .PP
# The program copies the directory structure rooted at \fIold-path\fR,
# ignoring RCS directories and any directories specified with the
# \fB-x\fR option (several may be used)
# or in the NOSHADOW environment variable.
# The new directory tree is rooted at \fInew-path\fR, which
# is created if necessary.  If \fInew-path\fR is not specified,
# the current working directory is used.
# Symbolic links are made to all non-directory files in \fIold-path\fR,
# except as modified by the various options.  A symbolic link is made
# to each RCS directory as well, unless otherwise requested with the
# \fB-r\fR option.
# If old-path is a relative pathname, the links will be relative.
# .PP
# If the \fB-s\fR ("sources only") option is used, links will be
# made to files which have corresponding
# RCS files only.  This is useful for omitting links
# to derived files (object and executable files, makefiles, etc.)
# OPTIONS
# .nf
# \fB-s\fR		make links to RCS-administered files only
# \fB-r\fR		do not make links to RCS directories
# \fB-v\fR		print verbose running commentary
# \fB-n\fR		don't actually create directories or links
# \fB-x \fIdirectory\fR	ignore \fIdirectory\fR in \fIold-path\fR
# .fi
# ENVIRONMENT VARIABLES
# .nf
# \fBNOSHADOW\fR	list of directories to ignore as though specified with \fB-x\fR.
# .fi
# EXAMPLES
# .DS
# .nf
# 
# .ft CW
# shadow -sr /usr/vw
# shadow -v -x man -x mansrc /usr/vw
# .ft R
# 
# .fi
# .DE
# SEE ALSO
# .B
# ln(1), rcs(1)
# BUGS
# Directory entries in \fIold-path\fR that are symbolic links
# are created as links, not directories, in \fInew-path\fR.
# .PP
# An option to use hard links is yet to be added.
# .\" .PP
# .\" An option to link just the root instead of the leaves
# .\" for arbitrary directories has been considered unwise
# .\" for the time being.
# .\" .PP
# .\" If both pathnames are entries in the same directory,
# .\" the last directory name components cannot start with
# .\" the same substring, \fIe.g.\fR, \fBdir1\fR and \fBdir2\fR,
# .\" or \fBshadow\fR will not do anything.
#*/


# standard argument processing template

cmd=`basename $0`
usage="Usage: $cmd [ -srvn ] [ -x directory ] old-path [ new-path ]"
minargs=1 maxargs=2
sourcesOnly=false readOnly=false verbose=false debug=false

set -$- `getopt 'srvnx:' "$@" 2>&1`
badopt=$?

for arg in $*
do
    case "$arg" in
	"getopt:")
	    echo $2 $3 $4 $5 >&2; break;;
	"--")
	    shift; break;;
	"-s")
	    sourcesOnly=true
	    shift;;
	"-r")
	    readOnly=true
	    shift;;
	"-v")
	    verbose=true
	    shift;;
	"-n")
	    debug=true
	    shift;;
	"-x")
	    NOSHADOW="$NOSHADOW $2"
	    shift; shift;;
    esac
done

if
    [ $badopt != 0 ] ||
    {
	[ $# -lt $minargs ] && echo "Missing argument(s)." >&2
    } ||
    {
	[ $# -gt $maxargs ] && echo "Too many arguments." >&2
    }
then
    echo "$usage" >&2
    exit 1
fi


# build regular expression from NOSHADOW

for d in $NOSHADOW
do
    exclude="$exclude$or/$d$|/$d/"
    or="|"
done


# normalize pathnames

relative=false

case "$1" in
    /*)   oldPath=$1;;
    ../*) oldPath=`cd $1; pwd`; relative=true;;
    *)	  oldPath=`cd $1; pwd`;;
esac

cwd=`pwd`

case "$2" in
    /*) newPath=$2;;
    "") newPath=$cwd;;
    *)	newPath=$cwd/$2;;
esac


# ignore working directory if encountered recursively

expr "$newPath" : "^$oldPath" >/dev/null &&
    exclude="$exclude$or/`basename $cwd`(\/.*)*$"


# main loop
#
# The `cd' is necessary in case oldPath is a symbolic link in
# which case `find' would fail.  This in turn necessitates
# tacking $oldPath back on for `mkrp'.

(cd $oldPath; find . -type d ! -name RCS -print) |
    if [ "$exclude" ]
    then
	egrep -v "$exclude"
    else
	cat
    fi |
	sed "s:^\.:$oldPath:" |
while read oldDir
do
    newDir=`echo $oldDir | sed "s:$oldPath:$newPath:"`

    if $relative
    then
    	refOldDir=`mkrp $oldDir $newDir`    # reference by relative path
    else
	refOldDir=$oldDir
    fi

    # create newDir
    if [ $newDir != $cwd ]
    then
	if [ ! -d $newDir ]
	then
	    $verbose && echo "+ mkdir $newDir" >&2
	    $debug || mkdir $newDir
	fi
    fi

    if [ -d $oldDir/RCS ]
    then
	rcs=true
    else
	rcs=false
    fi

    if $sourcesOnly
    then
	# link to working files for each RCS file

	if $rcs
	then
	    files=`ls $oldDir/RCS |
		sed "s:^:$refOldDir/:;s:,v::"`
	else
	    continue
	fi
    else
	# link all files from $oldDir

	files=`ls -F $oldDir |
	    sed "s:^:$refOldDir/:;/\/$/d;s/[@\*]$//;/RCS$/d"`
    fi

    # make links to files

    if [ "$files" ]
    then
        $verbose && echo "+     ln -s $refOldDir/* ." >&2
        $debug || ln -s $files $newDir
    fi

    # link to RCS directory if appropriate
    $readOnly ||
    if $rcs
    then
	$verbose && echo "+     ln -s $refOldDir/RCS ." >&2
	$debug || ln -s $refOldDir/RCS $newDir
    fi
done

exit 0
