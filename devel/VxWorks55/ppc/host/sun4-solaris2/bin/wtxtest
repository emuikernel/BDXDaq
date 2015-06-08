#!/bin/sh
# wtxtest.sh
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01a,06may98,p_b  WTX 2.0 tests
#
# SYNOPSIS
# .tS
# wtxtest <serverIdentifier> [-h] [-n <expr>] [-l [<globexp>]] [-tsload]
#         [-t <timeout>] [-i <invalidAddr>] [-SUPPRESS] [<tests[regexp]> ...]
#         [-r [<files>]]
# .tE
#
# DESCRIPTION
#
# This script just calls wtxtest.tcl, passing the arguments. Refer to
# wtxtest.tcl for OPTIONS.
#
# Note that the code below should use popd and pushd, when these functions
# will be available under Windows 95. (Ok under NT and UNIX).
#

args=$*
oldpwd=`pwd`
cd ${WIND_BASE}/host/src/test/wtxtcl
wtxtcl wtxtest.tcl $args
cd $oldpwd
