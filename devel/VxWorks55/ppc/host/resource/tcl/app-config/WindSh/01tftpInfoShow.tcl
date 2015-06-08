# 01tftpInfoShow.tcl - Implementation of shell tftpInfoShow commands in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh tftpInfoShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/tftpInfoShowCore.tcl
#

# source the data extraction and formatting routines

source [wtxPath host resource tcl]tftpInfoShowCore.tcl


###############################################################################
#
# tftpInfoShow - get TFTP status information
#
# This command prints information associated with TFTP descriptor <pTftpDesc>.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# EXAMPLE:
# A call to tftpInfoShow() might look like:
# 
# .CS
#     -> tftpInfoShow tftpDesc
#            Connected to yuba [69]
#            Mode: netascii  Verbose: off  Tracing: off
#            Rexmt-interval: 5 seconds, Max-timeout: 25 seconds
#     value = 0 = 0x0
#     ->
# .CE
#
# RETURNS: N/A
#
# ERRORS: 
# invalid tftp descriptor
#

shellproc tftpInfoShow {args} {

    bindNamesToList {descriptor} $args

    if {!$descriptor} {error "invalid tftp descriptor"}

    set tftpInfo [tftpInfoGet $descriptor]

    bindNamesToList {tftpVerbose \
		     tftpTrace   \
		     tftpReXmit  \
		     tftpTimeout \
		     mode        \
		     connected   \
		     serverName  \
		     serverPort} $tftpInfo

    if {$connected} {
	puts stdout [format "\tConnected to %s \[%d\]" $serverName\
		 [twoBytesPack $serverPort]]
    } else {
	puts stdout "\tNot connected"
    }

    puts stdout [format "\tMode: %s  Verbose: %s  Tracing: %s" \
		$mode $tftpVerbose $tftpTrace]
    puts stdout [format "\tRexmt-interval: %d seconds, Max-timeout: %d seconds"\
			$tftpReXmit $tftpTimeout]
}
