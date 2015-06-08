# 01Adm.tcl - administration tcl library
#
# modification history
# --------------------
# 01y,30nov98,jmp  removed Admin->IntallTape (SPR #23113).
# 01x,22jun98,fle  removed License Manager utilities
# 01w,05sep97,jmp  modified checkDevice{} to suppress leading&trailing spaces
#		   in device name (SPR# 4608).
# 01v,05nov96,jco  updated to use new graphical setup
# 01u,30oct96,p_m  Made License menu return active licenses
# 01t,21oct96,p_m  Adapted to Elan license management
# 01s,30aug96,elp  changed wpwrVersion{} into wtxTsVersionGet{}.
# 01r,05feb96,jco  changed the LRsubmissionConfirm routine (made a call to
#		    mailSend proc now defined in host.tcl).
# 01q,21nov95,jco  removed the isfile test in checkDevice (spr# 5505), used
#		   -sr flag with uname to fully discriminate between host types
#		   and added full path to lanscan and hostid (spr# 5506), 
#		   removed callbacks to entry texts "Tornado root path" and
#		    "Device Name (opt)" to avoid persistent error-message
#		   (spr# 5525), added taggedListRange proc.
# 01p,18nov95,jco  changed remaining mailCmdL to mailCmd global name.
# 01o,14nov95,jco  use lanscan to get host id on hp hosts in getServerHostId.
# 01n,11nov95,jco  used string compare for hostid to avoid out of range
#	 	    floating value possibly interpretation.
# 01m,27oct95,jco  used wpwrVersion, fixed the FLEXlm server get id facility
#		    (spr# 5060).
# 01l,05oct95,c_s  uses new host.tcl interfaces for teminals, editing, etc.
# 01k,11sep95,jco  fixed spr#4615. (preload version field for license request).
# 01j,30aug95,jco  added subject field to mail command.
# 01i,23aug95,jco  changed vi -R to shwait more (part of spr 4493).
# 01h,23aug95,c_s  added some fixes for SPR 4493, 4614.
# 01g,08jul95,jcf  changed to root of tree before calling installOption.
# 01f,20jun95,jco  changed rsh to rsh -n (allows launch in bg).
# 01e,20jun95,jco  changed license request submitting address.
# 01d,20jun95,jco  added license management support.
# 01c,19jun95,jco  fit to modified shwait.
# 01b,15jun95,jcf  cleanup.
# 01a,15jun95,jco  written, derived from 01WView.tcl and 01Supprt.tcl.
#

###############################################################################
#
# installCD - Tornado CD installation
#

proc installCD {} {
    global installCDCmd

    if {[noticePost question \
	"CD already mounted?" "Install" "Cancel"] == 1} {
	exec $installCDCmd &
    }
}

###############################################################################
#
# ftpWrsConnect - FTP connection to WRS
#

proc ftpWrsConnect {} {
    global wrsFtpCmd

    eval [concat {runTerminal "Tornado FTP Connection" 1 shwait} $wrsFtpCmd]
}

###############################################################################
#
# taggedListRange - extract the inner part of a doubly tagged list.
#
# The extracted material is ranging from startTag to stopTag.
# This procedure is used to handle possibly mixed answer message and error
# message when executing a remote shell call (especially on HP host, the
# remote shell call starts first by sourcing the user shell initialization file
# which is rarely compatible with other hosts settings (ie:stty commands)).
#
# RETURNS: the extracted piece (if not empty) or string "tagRangeError"
#
 
proc taggedListRange {taggedList startTag stopTag} {
 
    set start [lsearch $taggedList $startTag]
    set stop  [lsearch $taggedList $stopTag]
    if {$start == "-1" || $stop == "-1" || [expr $start +1] == $stop} {
        return tagRangeError
    }
    return [lrange $taggedList [incr start] [incr stop -1]]
}
