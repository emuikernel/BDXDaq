# 01WindView.tcl - shell extensions for WindView
#
# Copyright 1995-1996 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01c,02dec98,dbt  modified string allocation method in WindSh.
# 01b,18oct96,c_s  fix SPR #5970.
# 01a,03apr95,c_s  written.
#*/

##############################################################################
#
# wvHostInfoShow - show host connection attributes
#

shellproc wvHostInfoShow {args} {
    set pHostInfo [shSymAddr pHostInfo]
    if {$pHostInfo == 0} {
        puts "WindView host information not configured"
	return -1
    }

    set hostInfo [wtxGopherEval "$pHostInfo * $ +30 @w"]
    set hostName [lindex $hostInfo 0]
    set port [lindex $hostInfo 1]
    puts [format "Host IP Address  : %s" $hostName]
    puts [format "Host Port Number : %d" $port]
}

##############################################################################
#
# evtBufferToFile - copy post-mortem eventlog to host file via target server
#

shellproc evtBufferToFile {args} {

    set string [stringGet [lindex $args 0]]
    if {$string != 0} {
	set outFile	$string
    } else {
        set outFile	"out.wvr"
    }

    # make filename end in .wvr

    if {! [string match "*.wvr" $outFile]} {
	append outFile .wvr
    }

    # Read the elements of the "pEvtDblBuffers" structure.

    set ppBufInfo [shSymAddr pEvtDblBuffers]
    set pBufInfo [wtxGopherEval "$ppBufInfo @"]
    if {$pBufInfo == 0} {
	puts stdout "post-mortem buffer not configured"
	return -1
    }

    set bufInfo [wtxGopherEval "$pBufInfo @@@@"]
    bindNamesToList {ixInUse bufInUse bufInReserve ixInReserve} $bufInfo
    
    # If the buffers are empty, do nothing.

    if {$ixInReserve == $bufInReserve && $ixInUse == $bufInUse} {
	puts stdout "post-mortem buffer empty"
	return -1
    }
    # First fetch the reserve side of the buffer, if it's not empty
    # (it's considered empty if it only contains one EVENT_CONFIG,
    # which is 30 bytes.
    set evtCfgSize 30
    set partNum 1
    set nb [expr $ixInReserve - ($bufInReserve - $evtCfgSize)]
    if {$nb > $evtCfgSize} {
	puts stdout "doing part $partNum ($nb bytes)"
	set b [wtxMemRead [expr $bufInReserve - $evtCfgSize] $nb]
	memBlockWriteFile $b $outFile
	incr partNum
	memBlockDelete $b
    }
    # Now a similar thing for the buffer in use.  If we've already
    # done one buffer, set evtCfgSize to 0 so we won't record two of
    # them.
    if {$partNum > 1} {
	set evtCfgSize 0
    }
    set nb [expr $ixInUse - ($bufInUse - $evtCfgSize)]
    if {$nb > 0} {
	puts stdout "doing part $partNum ($nb bytes)"
	set b [wtxMemRead [expr $bufInUse - $evtCfgSize] $nb]
	if {$partNum > 1} {
	    memBlockWriteFile -append $b $outFile
	} else {
	    memBlockWriteFile $b $outFile
	}
	memBlockDelete $b
    }
    puts stdout "done"
}
