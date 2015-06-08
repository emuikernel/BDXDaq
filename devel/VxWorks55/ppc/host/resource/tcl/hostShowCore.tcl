# hostShowCore.tcl - Implementation of hostShow core routines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the hostShow core routines. 
#
# RESOURCE FILES
# 


##############################################################################
#
# hostEntryList - get the list of host entries
#
# The Gopher script gets the 'node' field of the HOSTENTRY structure defined
# in hostLib.h:
# .CS
#     typedef struct
#         {
#         NODE node;
#         HOSTNAME hostName;
#         struct in_addr netAddr;
#         } HOSTENTRY;
#
#     typedef struct hostname_struct
#         {
#         struct hostname_struct *link;
#         char *name;
#         } HOSTNAME;
#
#     typedef struct in_addr 
#         {
#         u_long s_addr;
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of ids corresponding to host entries
#
# ERRORS: N/A
#

proc hostEntryList {} {

    # get the address of the host list from the hostList symbol if it exists

    set hostListStart [symValueGet hostList "Host list not found"]
    return [wtxGopherEval "$hostListStart {!*}"]
}

##############################################################################
#
# hostNameAddr - get the IP address and the name of the host
#
# The Gopher script gets fields of the HOSTENTRY structure defined in
# hostLib.h:
# .CS
#     typedef struct
#         {
#         NODE node;
#         HOSTNAME hostName;
#         struct in_addr netAddr;
#         } HOSTENTRY;
#
#     typedef struct hostname_struct
#         {
#         struct hostname_struct *link;
#         char *name;
#         } HOSTNAME;
#
#     typedef struct node
#         {
#         struct node *next;
#         struct node *previous;
#         } NODE;
#
#     typedef struct in_addr 
#         {
#         u_long s_addr;
#         };
# .CE
#
# SYNOPSIS:
#   hostNameAddr hostEntry
#
# PARAMETERS:
#   hostEntry : id corresponding to one host entry
#
# RETURNS:
#   a list of 5 items:
#	the IP address (four bytes)
#	name
#	
# ERRORS: N/A
#

proc hostNameAddr {hostEntry} {

    # Get the s_addr and the name fields from the HOSTENTRY structure

    return [wtxGopherEval "$hostEntry +16 @b@b@b@b -8 <*$>"]
}

##############################################################################
#
# hostAliasesList - get the list of host aliases
#
# The Gopher script gets fields of the HOSTENTRY structure defined in 
# hostLib.h:
# .CS
#     typedef struct
#         {
#         NODE node;
#         HOSTNAME hostName;
#         struct in_addr netAddr;
#         } HOSTENTRY;
#
#     typedef struct hostname_struct
#         {
#         struct hostname_struct *link;
#         char *name;
#         } HOSTNAME;
#
#     typedef struct node
#         {
#         struct node *next;
#         struct node *previous;
#         } NODE;
#
#     typedef struct in_addr 
#         {
#         u_long s_addr;
#         };
# .CE
#
# SYNOPSIS:
#   hostAliasesList hostEntry
#
# PARAMETERS:
#   hostEntry : id corresponding to one host entry
#
# RETURNS:
#   a list of host aliases
#
# ERRORS: N/A
#

proc hostAliasesList {hostEntry} {

    # Get the name field from the HOSTENTRY structure

    return [wtxGopherEval "$hostEntry +8 *{ +4 <* $>  -4* }"]
}

