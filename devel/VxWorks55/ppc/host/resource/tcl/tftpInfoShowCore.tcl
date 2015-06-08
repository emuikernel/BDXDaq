# tftpInfoShowCore.tcl - Implementation of tftpInfoShow core routines in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the tftpInfoShow core routines. 
#
# RESOURCE FILES
# 


##############################################################################
#
# tftpInfoGet - get info associated with TFTP descriptor
#
# The Gopher script gets fields of the TFTP_DESC structure defined in
# tftpLib.h:
# .CS
#     typedef struct tftp_desc
#         {
#         char        mode [32];
#         BOOL        connected;
#         char        serverName [128];
#         struct sockaddr_in serverAddr;
#         int         sock;
#         u_short     serverPort;
#         char        fileName [TFTP_FILENAME_SIZE];
#         } TFTP_DESC;
#
#
# SYNOPSIS:
#   tftpInfoGet descriptor
#
# PARAMETERS:
#   descriptor : TFTP descriptor
#
# RETURNS:
#   a list of 8 items:
#       tftpVerbose,
#       tftpTrace,
#       tftpReXmit,
#       tftpTimeout,
#       mode,
#       connected,
#       serverName,
#       serverPort (two bytes)
#
# ERRORS:
#   can't get tftp status
# 

proc tftpInfoGet {descriptor} {

    set tftpVerbose [symValueGet tftpVerbose "tftpVerbose symbol not found"]
    set tftpTrace [symValueGet tftpTrace "tftpTrace symbol not found"]
    set tftpReXmit [symValueGet tftpReXmit "tftpReXmit symbol not found"]
    set tftpTimeout [symValueGet tftpTimeout "tftpTimeout symbol not found"]

    if {$tftpVerbose} {
	set tftpVerbose "on"
    } else {
	set tftpVerbose "off"
    }
    if {$tftpTrace} {
        set tftpTrace "on"
    } else {
        set tftpTrace "off"
    }

    # grab fields: mode, connected, serverName, serverPort (two bytes)

    if [catch {wtxGopherEval "$descriptor < $ ><+0x20 @><+0x24 $ >\
                        <+0xb8 @b @b>"} result ] {

	# this case could occure when the tftp descriptor is wrong

	error "can't get tftp status"
    }

    return "$tftpVerbose $tftpTrace $tftpReXmit $tftpTimeout \
		[lrange $result 0 2] [list [lrange $result 3 end]]"
}
