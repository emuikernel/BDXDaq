# routestatShowCore.tcl - Implementation of routestatShow core routines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This module is the Tcl code for the routestatShow core routines.
#
# RESOURCE FILES
#


##############################################################################
#
# rtstatInfoGet - get routing statistics
#
# The Gopher script gets fields of the rtstat structure defined in
# /net/route.h.
#
# .CS
# struct  rtstat
#     {
#     short       rts_badredirect;        /* bogus redirect calls */
#     short       rts_dynamic;            /* routes created by redirects */
#     short       rts_newgateway;         /* routes modified by redirects */
#     short       rts_unreach;            /* lookups which failed */
#     short       rts_wildcard;           /* lookups satisfied by a wildcard */
#     };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: 
#   a list of 5 items:
#   rts_badredirect rts_dynamic rts_newgateway rts_unreach rts_wildcard
#
# ERRORS: 
#   route statistics not found
#

proc rtstatInfoGet {} {

    # get the address of the route statistics from the rtstat symbol 
    # if it exists

    set rtstatAddr [symAddrGet rtstat "route statistics not found"]
    return [wtxGopherEval "$rtstatAddr @w @w @w @w @w"]
}
