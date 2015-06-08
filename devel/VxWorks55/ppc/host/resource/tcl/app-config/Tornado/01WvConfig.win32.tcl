# 01WvConfig.win32.tcl - WindView Configuration Dialog
#
# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 04m,10jul02,tcr  Fix SPR 79356 (improve error message)
# 04l,28jun02,cpd  Fix setting of button states
# 04k,23apr02,tcr  Fix logging of VxWorks events (SPR 75150)
# 04j,07jan02,tcr  Change Copyright date
# 04i,20nov01,tcr  add license management
# 04h,18oct01,tcr  add support for VxWorks Events
# 04g,19sep01,cpd  Initial Veloce mods.
#                  Fix SPR#24486, #27096, #27140: Rationalize error messages
#		   in an effort to remove inappropriate ones.
#                  Fix SPR#26770, #26772, #28211: Fix cancel operation
#		   Add network instrumentation code
# 04f,08oct01,fle  unified Tornado version String
# 04e,28apr99,tcr  correct file upload error message selection
# 04d,26apr99,nps  flush event queue before kicking off wvFuncCall.
# 04c,23apr99,nps  maintain single wtxToolAttach connection.
# 04b,20apr99,nps  fix problem with buffer size combo box.
# 04a,19apr99,nps  if a problem occurs when fetching symbols, the code now
#                  determines whether the symbol is actually missing, or the
#                  target [server] is not responding.
# 03z,16apr99,nps  reformat collection configuration dialog to avoid scrollbar
#                  on Unix. Also disable 'Add. Inst.' label when not in that
#                  mode.
# 03y,15apr99,nps  fix faulty string lengths when performing wtxMemAlloc.
# 03x,25mar99,tcr  Correct typo in earlier change, and correct script error
#                  when restarting windview against a target with a log
#                  already loaded
# 03w,22mar99,nps  improve diagnostic for 'path creation failed'.
# 03v,15mar99,tcr  Make the EventLog Periodic Refresh button pop out to
#                  reflect loss of connection to the target
# 03u,10mar99,tcr  Fix SPR 25372. Implement error handler for wtx... functions
#                  and ensure that callers have catch { } where necessary
# 03t,18feb99,tcr  Fix SPR 25175 by adding -parent wvConfig to dialogCreate as
#                  required
# 03s,10feb99,tcr  Fix SPR 24669. Modify wvFuncCall such that calls to the
#		   underlying wtxEventPoll timeout. This allows the UI to be
#		   updated. UI no longer freezes during upload, responsiveness
#		   is improved.
# 03r,15dec98,cjtc for upload via NFS, now check to see that upload IS going
#		   via NFS as expected. If the given NFS device does not exist
#		   the user is informed of the actual upload path that
#		   will be used and given the chance to back out. This is
#		   required due to limitations of netDrv library (which
#		   provides the FTP implementation) not being able to
#		   distinguish between a file and a directory. An upload may
#		   end up going to a non-existant directory or a read-only
#		   file system...at least the user is now warned.
# 03q,02dec98,nps  fix 'set $<value> <var>' typos by removing $.
#                  fix SPR 23136 - "endless PM mode pop-up when target not
#                  responding" This only occurred when driving radio buttons
#                  using keyboard due to different callback pattern.
#                  fix 23179 - "bad focus on Unix in config dialog" by
#                  reinstating windowActiveSet and checking first if
#                  window is already open.
# 03p,25nov98,nps  fix setting of strings for 'not supported' and 'not
#                  responding' states.
# 03o,09nov98,nps  further WV2.0 -> T2 merge.
# 03m,06nov98,cth  added -uiyield to all wvFuncCall calls (spr 22764)
# 03n,22oct98,jp   Change to use 2.0 Windows Registry
# 03l,16oct98,cth  moved wvOpenUpload call to after wvEvtClassSet in proc 
#		   wvConfigInit (SPR 22718)
# 03m,09oct98,ren  tor2_0_x --> tor2_0_0 merge
# 03l,09oct98,ren  fixed my screw up on PC side when I made change 03l
# 03k,05oct98,cjtc workaround for problem in which the wvConfig window can hide
#		   behind the main windView window when sub-windows of wvConfig
#		   are closed (SPR 21406)
#		   "Object Status" in the WindView Control dialog has been
#		   updated to read "Additional Instrumentation" (SPR 22595)
# 03j,02oct98,nps  warn about listener requirement if sock upload path creation
#                  fails.
# 03k,02oct98,nps  remove <ctrl>-Ms
# 03j,01oct98,nps  moved Configuration menu item to 01WindView.win32.tcl.
# 03i,08sep98,nps  remove ioLib as a potential instrumented lib.
# 03i,03sep98,nps  UITcl II update.
# 03h,20aug98,dbs  added -uiyield arg to wvFuncCall to yield control to the 
#                  GUI via UITcl's uiEventProcess function, during the 'wait
#                  for result' phase of the procedure.
# 03h,20aug98,nps  fixed lingering toolAttach.
# 03g,20aug98,nps  'Additional Libraries' now called 'Additional
#                  Instrumentation'.
# 03f,19aug98,cjtc event buffer full handled more gracefully (SPR 22133)
# 03e,07aug98,cjtc fix problem in which a failed upload in post-mortem mode
#		   generated a target exception - still tried to upload the
#		   preserved task names even though the upload path had been
#		   closed by the error (SPR 21869)
# 03d,29jul98,ren  made buttons slightly wider to accomodate fixed width
#                  fonts.
# 03d,27jul98,nps  more on previous fix, refresh keeps record of whether it
#                  has lost contact with target in order to reenable buttons
#                  when appropriate.
# 03c,27jul98,nps  more for SPR 21378 - improve handling of target that does
#                  not support WV.
# 03b,24jul98,nps  fixed invokation from Launch menu (SPR 21455),
#                  checkTarget not visible from here.
# 03a,16jul98,cjtc prevent ui from attempting to close upload path if
#                  wvCloseUpload returns an error - the target will already
#                  have closed it. 
#                  Doing it again causes a memPartFree error (SPR 21461)
# 02z,09jul98,cjtc fix problems with task name preservation in post-mortem
#                  mode (SPR 21350)
# 02y,08jul98,cjtc increase size of reserved area in post mortem region from
#                  8 to 16 (SPR 21755)
# 02x,07jul98,cjtc fix dangling tRBuffMgr task following a path creation
#                  failed error in continuous upload mode (SPR 21474)
# 02w,07jul98,cjtc fix problem in obtaining sysPhysMemTop from target in post
#                  mortem mode configuration (SPR 21559).
#                  Really fixed SPR 21567.
# 02w,19jun98,nps  changed name of 'object status' level.
#                  undid change 02u.
#                  fixed SPR 21378 - handling of non-responding target
#                  fixed SPR 21753 - WindView not listed by Launcher
#                  fixed SPR 21443 - Tcl_Eval errors displayed on console
# 02v,18jun98,nps  fixed SPR 21567.
# 02u,05jun98,nps  Made Config dialog modeless.
# 02t,27may98,nps  SPR #21257: Check that PM buffer ptr is possibly sensible
#                  before performing an rBuffVerify.
#                  brief flirtation with rBuff utilisation meter.
# 02s,05may98,wmd  fix geometries for WindView Collection Configuration dlg so
#                  that it appears correct in both win32 and Unix.
# 02r,01may98,wmd  remove commented lines so that tcl errors do not occur.
# 02q,27apr98,nps  sysPhysMemTop now used when initialising PM region.
#                  use appropriate icon in messageBoxes.
#                  don't read PM config fields when window not present.
# 02p,21apr98,nps  Post-Mortem mode completed!
#                  Restore msg for unrecognised additional lib.
# 02o,20apr98,cth  adding calls to wvLogHeaderCreate/Upload
#             nps  merged back with latest mainwin cut.
# 02n,20apr98,c_s  mainwin fixups.
# 02m,18apr98,nps  do what 02l claims to do :-)
# 02l,18apr98,nps  remove debug comments.
# 02k,17apr98,nps  continuing PM mode task name preservation.
#                  'default' WV control properties are no longer required
#                  to be confirmed by user.
# 02j,15apr98,nps  incorporated PM mode task name preservation.
#                  msgQLib now given its correct name.
# 02i,13apr98,nps  fixed deferred upload 'go to sleep' problem.
#                  Upload configuration button now labelled 'Properties'.
# 02h,27mar98,nps  Don't show link state in deferred mode.
#                  Some additional libs are 'on' by default.
# 02g,26mar98,nps  fixes windview control listing of additional libs.
# 02f,26mar98,nps  fixed reset for new additional instrumentation support.
# 02e,20mar98,nps  remove duplicated messageBox warnings about upload path
#                  failures.
#                  Additional libraries modified to use check box list and
#                  sigLib support added.
# 02d,19mar98,nps  support new 'deferred upload' rBuff option.
#                  rBuff threshold now set to buffer size / 2.
# 02c,18mar98,nps  removed messageBoxs re (supported) additional libraries.
# 02b,18mar98,nps  added support for memLib as an additional library.
#                  grey out additional libs check-boxes if not at
#                  object status level.
# 02a,16mar98,nps  continued PM development.
#                  added support for object instrumentation as
#                  additional libraries
# 029,13mar98,nps  bug fix (you can tell it's Friday 13th).
# 028,13mar98,nps  bug fix and some more PM work.
# 027,12mar98,nps  Detect if rBuffCreate fails.
#                  Added config reset button.
#                  Correctly create post-mortem partition.
#                  Substitute local host name rather than tgtsvr host name
#                  when constructing upload paths.
#                  Support 'upload to graph option'.
# 026,06mar98,nps  Split config dialog into two.
#                  Don't offer fooLib as an additional library.
#                  Added some further support for additional libraries.
#                  Improved post-mortem dialog.
# 025,03mar98,nps  Added some support for post-mortem mode.
#                  Make sure config.ok is only initialised once
# 024,02mar98,nps  incorporate various fixes added in Alameda.
#                  fixed upSock ref in wvUploadTsfsSock.
# 023,16feb98,nps  Rejigged Tools menu.
# 022,13feb98,nps  bug fix - call wvEvtLogStop when stopping WV.
# 021,13feb98,nps  Added some buffer parameter validation.
# 020,04feb98,nps  Changed names of upload paths.
# 01f,03feb98,nps  reread config from registry each time we start WV
# 01e,02feb98,nps  now supports a configuration overview window.
# 01d,27jan98,nps  Updated to support fixes to instrumentation classes.
# 01c,19jan98,nps  Further target integration - now interworks with triggers.
#                  Supports deferred upload.
# 01b,12jan98,nps  Target integration
#                  - minimum buffers raised to 2 (enforced minimum)
#                  - reworked upload path
# 01b,17dec97,c_s  WindView 2.0 development
# 01a,11nov97,c_s  written.
#*/

package require Wind

# globals

set wvGlobal(hostOrIP) ""
set wvGlobal(port) 0
set wvGlobal(libState) ""

if [catch {info hostname} wvGlobal(localHost)] {
    messageBox "Unable to determine local IP host name : $wvGlobal(localHost)"
    set wvGlobal(localHost) "unknown"
}

set defaultUpGraph ""
set defaultUpGraph [append defaultUpGraph /tgtsvr/TCP: $wvGlobal(localHost)]
set defaultUpTsfsS ""
set defaultUpTsfsS [append defaultUpTsfsS /tgtsvr/TCP: $wvGlobal(localHost) :6164]
set defaultUpSock ""
set defaultUpSock  [append defaultUpSock $wvGlobal(localHost) :6164]

set wvGlobal(DefKeyVal) {
    { minBufCnt	2	}
    { maxBufCnt	10	}
    { bufSize	"32 KB" }
    { upload	0       }
    { upGraph	""}
    { upTsfsF	"/tgtsvr/eventLog.wvr"}
    { upTsfsS	""}
    { upSock	""}
    { upFile	"/eventLog.wvr"}
    { defer     1       }
    { cont      0       }
    { postm     0       }
    { base	1       }
    { extraLibs	{{taskLib 1} {semLib 1} {msgQLib 1} {wdLib 1} {eventLib 1} {memLib 0} {sigLib 0} {netLib 0} } }
    { sysMemTop 0x0     }
    { physTop   0x0     }
    { netInstLevel 0}
}

# The uploadConfig array holds values being edited in the upload configuration
# dialog. If the user accepts the configuration, they are transferred to 
# wvGlobal, otherwise they are discarded.

set uploadConfig(reg.minBufCnt) 0
set uploadConfig(reg.maxBufCnt) 0
set uploadConfig(reg.bufSize) 0
set uploadConfig(reg.upload) 0
set uploadConfig(reg.upGraph) 0
set uploadConfig(reg.upTsfsF) 0
set uploadConfig(reg.upTsfsS) 0
set uploadConfig(reg.upSock)  0
set uploadConfig(reg.upFile)  0
set uploadConfig(reg.defer) 0
set uploadConfig(reg.cont)  0
set uploadConfig(reg.postm) 0
set uploadConfig(prevPMmode) 0
set uploadConfig(reg.physTop) 0
set uploadConfig(reg.sysMemTop) 0
set uploadConfig(reg.netInstLevel) 0

set wvGlobal(config.changed)    0
set wvGlobal(uplinkActive) 0
set wvGlobal(rBuffID) 0
set wvGlobal(targetOK) 1
set wvGlobal(wvEvtBufferFullNotify) -1
set wvGlobal(prevPMmode) 0

set wvGlobal(addLibList) {taskLib semLib msgQLib wdLib eventLib memLib sigLib netLib}

proc wtxMemAllocStr {str} {

    set len [string length $str]
    incr len
    set ptr [wtxMemAlloc $len]
    return $ptr
}

proc wvConfigDialog {targetName} {
    global wvGlobal

    if {[wvVerifyConnection] != 1} {
        return
    }

    # Copies of parameters (possibly) modified by 2nd dialog
    global uploadConfig

    if [windowExists wvConfig] {
        return
    }

    # Refresh Details

    set wvGlobal(targetName) $targetName
    set ix [string last @ $targetName]

    if {$ix != -1} {
        incr ix
        set wvGlobal(hostName) [string range $targetName $ix end]
    } else {
        set wvGlobal(hostName) hostnameOrIP
    }

    # Copy elements from global into uploadConfig
    foreach element [array names uploadConfig] {
        set uploadConfig($element) $wvGlobal($element)
    }

    dialogCreate -name wvConfig \
	-title "Collection Configuration" \
	-init "wvConfigInit" \
	-w 165 -h 180 \
	-helpfile $wvGlobal(helpFilePath) \
	-controls {
	    { group -title "Events" 
	      -x 5 -y 5 -w 155 -h 130} 
	    { label -title "Base Events" -x 15 -y 15 -w 60 -h 8 }
	    { combo -name base -callback wvOnBaseEvent -x 15 -y 25 -w 135 -h 50 }
	    { label -name AILabel -title "Additional Instrumentation" -x 15 -y 45 -w 135 -h 8 }

	    { checklist -name xtraLibs -callback wvXtraLibs -x 15 -y 55 -w 135 -h 75}

	    { button -name configUpload -title "Properties..."
              -callback wvConfigUpload
	      -x 10 -y 140 -w 50 -h 15}
	    { button -name configReset -title "Reset"
              -callback wvConfigResetCheck
	      -x 70 -y 140 -w 50 -h 15}

	    { button -default -name okButt 
	      -title "OK" -callback wvConfigAccept 
              -x 45 -y 160 -w 50 -h 15} 
	    { button -name cancelButt -title "Cancel" 
    	      -callback wvConfigCancel
    	      -x 105 -y 160 -w 50 -h 15} 

        }
}



##############################################################################
#
# wvXtraLibs - call back with special processing of netLib instrumentation
# 
# This is called when the selection changes in the 'additional libraries'
# box in the config window. It checks whether the netLib entry is being
# selected. If it is, a dialog pops up to select the instrumentation level.
# 
#
# RETURNS: n/a
# 

proc wvXtraLibs {} {
    global wvGlobal
    global libState
    global netIndex

    # save current checklist states
    set libState [controlValuesGet wvConfig.xtraLibs]        
    
    if { $wvGlobal(libState) == $libState } { return }
    
    set wvGlobal(libState)	$libState
    
    # Find the netLib entry in the libs list
    set netIndex [lsearch $libState *netLib*]
	
    # if checklist item is not the netLib one, or it is not found,
    # do nothing more (probably need to remove the registry and 
    # restart windview to create a new one)
    if { $netIndex != [controlSelectionGet wvConfig.xtraLibs] } {

    	return
    }	

    ##  Here we check to see if the checkbox was ON and has been turned OFF
    ##  If it has then all we do is return with the netlib off
    if {[lindex $libState $netIndex] == "netLib 0"} {

	return	
    }

    ##  Now we look to see if the target has the required entry point
    ##  If not, warn the user and reset the checkbox to OFF
    if {$wvGlobal(wvNetEnable) == 0} {

	# Turn checkbox off...
	set libState [lreplace $libState $netIndex $netIndex {netLib 0}]	
	set wvGlobal(libState) $libState
	controlValuesSet wvConfig.xtraLibs $libState
  	wvAlertNoNetLib
	
	return
    }


    # this line is important...
    controlValuesSet wvConfig.xtraLibs $libState

    # pop up a dialog    

    dialogCreate -name netLibConfig \
	-title "Network Instrumentation Configuration" \
	-init "wvNetLibConfigInit" \
	-w 180 -h 100 \
	-helpfile $wvGlobal(helpFilePath) \
	-controls {
	    { button -default -name okButt 
	    -title "OK" -callback wvNetLibConfigAccept 
            -x 35 -y 80 -w 50 -h 14} 
	    { button -name cancelButt -title "Cancel" 
    	    	-callback wvNetLibConfigCancel
    	        -x 95 -y 80 -w 50 -h 14} 

	    	{ label -title "Priority Level" -x 15 -y 15 -w 1200 -h 8 }
	    	{ combo -name instrumentationLevel -x 15 -y 30 -w 153 -h 50 }	    
        }        
    set wvGlobal(libState) $libState
}

##############################################################################
#
# wvAlertNoNetLib - show an error dialog
# 
# Create an error dialog
#
# RETURNS: n/a
# 

proc wvAlertNoNetLib {} {
    
    messageBox "Target does not have network instrumentation"
	
}
	
##############################################################################
#
# wvNetLibConfigInit - set up the network instrumentation config dialog
# 
# This loads the strings representing levels of instrumentation into a 
# combo-box. The correct one is then selected.
# 
#
# RETURNS: n/a
# 

proc wvNetLibConfigInit {} {
    global uploadConfig

    # load strings into combo-box    
    controlValuesSet netLibConfig.instrumentationLevel {
        "EMERGENCY"
	"ALERT"
	"CRITICAL"
	"ERROR"
	"WARNING"
	"NOTICE"
	"INFO"
	"VERBOSE"
    }
    
    # Try to retrieve stored level. If error (possibly because no entry in
    # registry yet,) set it to default.
    if { [catch {controlSelectionSet netLibConfig.instrumentationLevel $uploadConfig(reg.netInstLevel)}]} {
	set uploadConfig(reg.netInstLevel) 0
	controlSelectionSet netLibConfig.instrumentationLevel $uploadConfig(reg.netInstLevel)
    }
}

##############################################################################
#
# wvNetLibConfigCancel - 'cancel' button callback from netLibConfig dialog
# 
# Closes netLibConfig dialog without saving changes.
# 
#
# RETURNS: n/a
# 

proc wvNetLibConfigCancel {} {
    global uploadConfig
    global netIndex
    global libState

    windowClose netLibConfig

    set libState [lreplace $libState $netIndex $netIndex {netLib 0}]
    
    # restore the state of the checkboxes
    controlValuesSet wvConfig.xtraLibs $libState
}


##############################################################################
#
# wvNetLibConfigAccept - 'Ok' button callback from netLibConfig dialog
# 
# Closes netLibConfig dialog and saves changes.
# 
#
# RETURNS: n/a
# 

proc wvNetLibConfigAccept {} {
    global uploadConfig
    global netIndex
    global libState

    # Turn checkbox on...
    set libState [lreplace $libState $netIndex $netIndex {netLib 1}]

    controlValuesSet wvConfig.xtraLibs $libState
    
    set uploadConfig(reg.netInstLevel) [controlSelectionGet netLibConfig.instrumentationLevel]
    windowClose netLibConfig
}

proc wvOnBaseEvent {} {

    set base [controlSelectionGet wvConfig.base]

    if {$base != 2} {
        controlEnable wvConfig.xtraLibs 0
        controlEnable wvConfig.AILabel  0

    } else {
        controlEnable wvConfig.xtraLibs 1
        controlEnable wvConfig.AILabel  1
    }
}

proc wvConfigUpload {} {
    # Required for helpfile path
    global wvGlobal

    # Results kept here until configAccept
    global uploadConfig

    global defaultUpGraph
    global defaultUpTsfsS
    global defaultUpSock

    if [windowExists wvUploadConfig] {
        return 0
    }

    if {$uploadConfig(reg.upGraph) == ""} {
        set uploadConfig(reg.upGraph) $defaultUpGraph
    }

    if {$uploadConfig(reg.upTsfsS) == ""} {
        set uploadConfig(reg.upTsfsS) $defaultUpTsfsS
    }

    if {$uploadConfig(reg.upSock) == ""} {
        set uploadConfig(reg.upSock) $defaultUpSock
    }

    dialogCreate -name wvUploadConfig \
        -parent wvConfig \
	-title "Upload Configuration" \
	-init "wvConfigUploadInit" \
	-w 165 -h 200 \
	-helpfile $wvGlobal(helpFilePath) \
	-controls {
	    { button -default -name okButt 
	      -title "OK" -callback wvConfigUploadAccept 
              -x 25 -y 180 -w 50 -h 15} 
	    { button -name cancelButt -title "Cancel" 
    	      -callback wvConfigUploadCancel
    	      -x 90 -y 180 -w 50 -h 15} 

	    { group -title "Buffer Configuration" 
	      -x 5 -y 5 -w 155 -h 80} 
	    { label -name lMinBuf -title "Minimum Buffer Count" 
	      -x 15 -y 25 -w 70 -h 13}
	    { text -name minBufCnt
	      -x 95 -y 23 -w 55 -h 13 }
	    { label -name lMaxBuf -title "Maximum Buffer Count"
	      -x 15 -y 45 -w 72 -h 13}
	    { text -name maxBufCnt
	      -x 95 -y 43 -w 55 -h 13 }
	    { label -name lBufSiz -title "Buffer Size"
	      -x 15 -y 65 -w 35 -h 8}
	    { combo -name bufSize
	      -x 95 -y 64 -w 56 -h 70}


	    { label -name lsysMemTop -title "sysMemTop" 
	      -x 15 -y 25 -w 70 -h 13 -hidden}
	    { text -name sysMemTop 
	      -x 95 -y 23 -w 55 -h 13 -hidden}

	    { label -name lPhysTop -title "sysPhysMemTop" 
	      -x 15 -y 55 -w 70 -h 13 -hidden}
	    { text -name physTop 
	      -x 95 -y 53 -w 55 -h 13 -hidden}

	    { group -title "Upload Path" 
	      -x 5 -y 90 -w 155 -h 83} 
	    { combo -name upload -callback onUploadSelect
	      -x 15 -y 105 -w 136 -h 60}
	    { text -name uploadPath
	      -x 15 -y 123 -w 136 -h 14 }
            { choice -auto -name defer -title "Deferred"
              -callback onPMselect
              -x 15 -y 144 -w 60 -h 10 }
            { choice -auto -name cont -title "Continuous"
              -callback onPMselect
              -x 15 -y 156 -w 60 -h 10 }
            { choice -auto -name postm -title "Post-Mortem"
              -callback onPMselect
              -x 95 -y 144 -w 60 -h 10 }
	}
}

proc onPMselect {} {
    global uploadConfig

    set pmMode [controlChecked wvUploadConfig.postm]

    if {$uploadConfig(prevPMmode) == $pmMode} {
        return
    }

    set uploadConfig(prevPMmode) $pmMode

    if {$pmMode == 1} {

        wvConfigPMInit

        controlEnable wvUploadConfig.minBufCnt 0
        controlEnable wvUploadConfig.maxBufCnt 0
        controlEnable wvUploadConfig.bufSize   0

        controlHide wvUploadConfig.minBufCnt   1
        controlHide wvUploadConfig.lMinBuf     1
        controlHide wvUploadConfig.maxBufCnt   1
        controlHide wvUploadConfig.lMaxBuf     1
        controlHide wvUploadConfig.bufSize     1
        controlHide wvUploadConfig.lBufSiz     1

        controlHide wvUploadConfig.lsysMemTop  0
        controlHide wvUploadConfig.sysMemTop   0
        controlHide wvUploadConfig.lPhysTop    0
        controlHide wvUploadConfig.physTop     0
    } else {
        controlEnable wvUploadConfig.minBufCnt 1
        controlEnable wvUploadConfig.maxBufCnt 1
        controlEnable wvUploadConfig.bufSize   1

        controlHide wvUploadConfig.minBufCnt   0
        controlHide wvUploadConfig.lMinBuf     0
        controlHide wvUploadConfig.maxBufCnt   0
        controlHide wvUploadConfig.lMaxBuf     0
        controlHide wvUploadConfig.bufSize     0
        controlHide wvUploadConfig.lBufSiz     0

        controlHide wvUploadConfig.lsysMemTop  1
        controlHide wvUploadConfig.sysMemTop   1
        controlHide wvUploadConfig.lPhysTop    1
        controlHide wvUploadConfig.physTop     1
    }
}

proc wvConfigValuesGet {selectedTarget} {

    if {$selectedTarget == ""} {
        messageBox "No target selected."
        return
    }

    wvConfigDialog $selectedTarget
}

proc wvConfigInit {} {
    global wvGlobal

    controlValuesSet wvConfig.base {
        "Context Switch"
	"Task State Transition"
	"Additional Instrumentation"
    }

    switch $wvGlobal(reg.base) {
        default -
        1 {controlSelectionSet wvConfig.base 0}
        3 {controlSelectionSet wvConfig.base 1}
        7 {controlSelectionSet wvConfig.base 2}
    }

    controlValuesSet wvConfig.xtraLibs $wvGlobal(reg.extraLibs)
    set wvGlobal(libState) $wvGlobal(reg.extraLibs)
    
}

proc wvConfigUploadInit {} {
    global wvGlobal
    global uploadConfig

    controlValuesSet wvUploadConfig.minBufCnt $uploadConfig(reg.minBufCnt)
    controlValuesSet wvUploadConfig.maxBufCnt $uploadConfig(reg.maxBufCnt)
    controlValuesSet wvUploadConfig.bufSize {
        "1 KB" "2 KB" "4 KB" "8 KB" "16 KB" "32 KB" "64 KB"
	"128 KB" "256 KB" "512 KB" "1 MB"
    }

    controlSelectionSet wvUploadConfig.bufSize -string $uploadConfig(reg.bufSize)

    controlValuesSet wvUploadConfig.upload {
        "Direct to Graph"
        "File via TSFS"
        "Socket via TSFS"
        "Socket via TCP/IP"
        "File via NFS"
    }

    controlSelectionSet wvUploadConfig.upload $uploadConfig(reg.upload)

    switch $uploadConfig(reg.upload) {
        0 {
           controlValuesSet wvUploadConfig.uploadPath $wvGlobal(localHost)
           controlEnable wvUploadConfig.uploadPath 0
          }
        1 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upTsfsF)
           controlEnable wvUploadConfig.uploadPath 1
          }
        2 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upTsfsS)
           controlEnable wvUploadConfig.uploadPath 1
          }
        3 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upSock)
           controlEnable wvUploadConfig.uploadPath 1
          }
        4 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upFile)
           controlEnable wvUploadConfig.uploadPath 1
          }
    }

    controlCheckSet wvUploadConfig.defer $uploadConfig(reg.defer)
    controlCheckSet wvUploadConfig.cont  $uploadConfig(reg.cont)
    controlCheckSet wvUploadConfig.postm $uploadConfig(reg.postm)
}

proc wvConfigAccept {} {
    global wvGlobal
    global uploadConfig

    set wvGlobal(config.ok) 1
    set wvGlobal(config.changed) 1

    # Copy elements from uploadConfig into global
    foreach element [array names uploadConfig] {
        set wvGlobal($element) $uploadConfig($element)
    }    

    switch [controlSelectionGet wvConfig.base] {
        default -
        0 {set wvGlobal(reg.base) 1}
        1 {set wvGlobal(reg.base) 3}
        2 {set wvGlobal(reg.base) 7}
    }

    set wvGlobal(reg.extraLibs) [controlValuesGet wvConfig.xtraLibs]

    wvConfigRegistryStore

    if {$wvGlobal(configUp.ok) != 1} {
        messageBox "Please configure the event data upload path"

        set wvGlobal(config.ok) 0

        wvConfigUpload

        return
    }

    windowClose wvConfig

    if {$wvGlobal(reg.postm) != 0} {
	set wvGlobal(configPM.ok) [wvConfigPMAccept]
    }

    uploadRefreshView

    # Requires eventLogging.uploadInfo window
}

# Accepted parameters are loaded into uploadConfig array. If the config
# dialog is accepted, they will be transferred to wvGlobal in wvConfigAccept

proc wvConfigUploadAccept {} {
    global wvGlobal
    global uploadConfig

    set pmMode [controlChecked wvUploadConfig.postm]

    if {$pmMode == 1} {
        if { [wvConfigPMAccept] == 0} {
            messageBox -stopicon "Cannot continue without valid PM configuration"
            return
        }
    }

    switch [controlSelectionGet wvUploadConfig.upload] {
	2 {
	    ## uploading TSFS
	    if { [wvConfigPathTsfsSockCheck [controlValuesGet wvUploadConfig.uploadPath]] == 0} {
		return
	    }
	}
	3 {
	    ## uploading TCP/IP
	    if { [wvConfigPathTCPSockCheck [controlValuesGet wvUploadConfig.uploadPath]] == 0} {
		return
	    }
	}
    }

    set wvGlobal(configUp.ok) 1
    set wvGlobal(config.changed) 1

    # validate data;
    # get values out of dialog and store them 
    # in a global variable

    if ![wvIntVal wvUploadConfig.minBufCnt V minBufCnt origVal 0 65535     ] return
    set uploadConfig(reg.minBufCnt) $origVal
    if ![wvIntVal wvUploadConfig.maxBufCnt V maxBufCnt origVal 1 0x7fffffff] return
    set uploadConfig(reg.maxBufCnt) $origVal
    if ![wvIntVal wvUploadConfig.bufSize   S bufSize   origVal 1 0x200000  ] return
    set uploadConfig(reg.bufSize)   $origVal

    if {$uploadConfig(reg.minBufCnt) > $uploadConfig(reg.maxBufCnt) ||
        $uploadConfig(reg.minBufCnt) == 1} {
        messageBox -stopicon "Invalid Buffer Configuration"
        return
    }

    set uploadConfig(reg.upload) [controlSelectionGet wvUploadConfig.upload]

    switch $uploadConfig(reg.upload) {
        0 {set uploadConfig(reg.upGraph) [controlValuesGet wvUploadConfig.uploadPath] }
        1 {set uploadConfig(reg.upTsfsF) [controlValuesGet wvUploadConfig.uploadPath] }
        2 {set uploadConfig(reg.upTsfsS) [controlValuesGet wvUploadConfig.uploadPath] }
        3 {set uploadConfig(reg.upSock) [controlValuesGet wvUploadConfig.uploadPath] }
        4 {set uploadConfig(reg.upFile) [controlValuesGet wvUploadConfig.uploadPath] }
        default {messageBox -stopicon "Error in wvConfigAccept"}
    }

    set uploadConfig(reg.defer) [controlChecked wvUploadConfig.defer]
    set uploadConfig(reg.cont)  [controlChecked wvUploadConfig.cont]
    set uploadConfig(reg.postm) [controlChecked wvUploadConfig.postm]

    wvConfigRegistryStore
    windowClose wvUploadConfig

#   uploadRefreshView

    # Requires eventLogging.uploadInfo window

    #
    # The following windowActiveSet should not really be necessary, and it
    # isn't on an NT host. On a solaris host, particularly with the CDE window
    # manager, the wvConfig window can sometimes "hide" behind the main
    # windView window when the wvUploadConfig window is closed down. Forcing
    # the wvConfig window active now will cure this problem and is harmless on
    # NT, where it doesn't really need it.
    #

    windowActiveSet wvConfig
}

proc wvConfigPathTsfsSockCheck {path} {
    global wvGlobal

    set colonIndex [string first : $path]

    if {$colonIndex == -1} {
        endWaitCursor
        messageBox -stopicon " Please configure the upload path for Socket via TSFS using the\n\
			      form:  '/tgtsvr/TCPhostname:port' or '/tgtsvr/TCP:IP_address:port'"
        return 0
    }

    set hostPortString [string range $path $colonIndex end]
    set hostPortString [string trimleft $hostPortString :]

    set colonIndex [string first : $hostPortString]

    if {$colonIndex == -1} {
        endWaitCursor
        messageBox -stopicon " Please configure the upload path for Socket via TSFS using the\n\
			      form:  '/tgtsvr/TCP:hostname:port' or '/tgtsvr/TCP:IP_address:port'"
        return 0
    }

    set wvGlobal(hostOrIP) [string range $hostPortString 0 $colonIndex]
    set wvGlobal(hostOrIP) [string trimright $wvGlobal(hostOrIP) :]
    set wvGlobal(port)     [string range $hostPortString $colonIndex end]
    set wvGlobal(port)     [string trimleft $wvGlobal(port) :]

    return 1
}

proc wvConfigPathTCPSockCheck {path} {
    global wvGlobal

    set colonIndex [string first : $path]

    if {$colonIndex == -1} {
        endWaitCursor
        messageBox -stopicon " Please configure the upload path for Socket via TCP/IP\n\
			      using the form:  'hostname:port' or 'IP_address:port'"
        return 0
    }

    set wvGlobal(hostOrIP) [string range $path 0 $colonIndex]
    set wvGlobal(hostOrIP) [string trimright $wvGlobal(hostOrIP) :]
    set wvGlobal(port)     [string range $path $colonIndex end]
    set wvGlobal(port)     [string trimleft $wvGlobal(port) :]

    return 1
}

proc wvConfigPMInit {} {
    global wvGlobal
    global uploadConfig

    if { [windViewBindToTarget $wvGlobal(targetName)] != 1} {
        return
    }

    if {$wvGlobal(reg.sysMemTop) == 0} {
        if [catch {wtxSymFind -name "sysMemTop"} symInfo] {
            set wvGlobal(reg.sysMemTop) 0
        } else {
            set wvGlobal(sysMemTop) [lindex $symInfo 1]
            set uploadConfig(reg.sysMemTop) [wtxDirectCall $wvGlobal(sysMemTop)]
        }
    }

    if {$wvGlobal(reg.physTop) == 0} {
        if [catch {wtxSymFind -name "sysPhysMemTop"} symInfo] {
            set wvGlobal(reg.physTop) 0
        } else {
            set wvGlobal(sysPhysMemTop) [lindex $symInfo 1]
            set uploadConfig(reg.physTop) [wtxDirectCall $wvGlobal(sysPhysMemTop)]
        }
    }

    #wtxToolDetach

    controlValuesSet wvUploadConfig.sysMemTop [format "0x%08x" $uploadConfig(reg.sysMemTop)]
    controlValuesSet wvUploadConfig.physTop   [format "0x%08x" $uploadConfig(reg.physTop)]
}

# Return 1 if configuration is ok, 0 otherwise

proc wvConfigPMAccept {} {
    global wvGlobal
    global uploadConfig

    if [windowExists wvUploadConfig] {
        set memTop  [controlValuesGet wvUploadConfig.sysMemTop]
        set physTop [controlValuesGet wvUploadConfig.physTop]
    } else {
        set memTop  $wvGlobal(reg.sysMemTop)
        set physTop $wvGlobal(reg.physTop)
    }

    if {$memTop >= $physTop} {
        return 0
    }

    # If we're called from inside upload config window,
    # just update the uploadConfig copy
    if [windowExists wvUploadConfig] {
	set uploadConfig(reg.sysMemTop) $memTop
	set uploadConfig(reg.physTop)   $physTop
    } else {
	set wvGlobal(reg.sysMemTop) $memTop
	set wvGlobal(reg.physTop)   $physTop
    }

    windViewBindToTarget $wvGlobal(targetName)
    catch {wvConfigGetSyms}

    if [catch {set PMbuff [wtxGopherEval "$wvGlobal(reg.sysMemTop) @l"]} result] {
        # Will end up here if configured / retrieved sysMemTop address
        # does not exist on target...

        catch { wtxToolDetach }

        return 0
    }

    # Verify potential buffer ptr is possibly sensible

    if {$PMbuff > $memTop && $PMbuff < $physTop} {

        # Verify potential buffer ptr is actually sensible

        if {[wtxDirectCall $wvGlobal(rBuffVerify) $PMbuff] == 0} {
            messageBox -informationicon "Existing post-mortem log found"

            set wvGlobal(rBuffID) $PMbuff

            set endianFlag "-[string range [targetEndian] 0 0]"
            set argLocal [memBlockCreate $endianFlag]
            memBlockSet -l $argLocal 0 $PMbuff 
            catch {wtxMemWrite $argLocal $wvGlobal(evtBufferId)}

            # Retrieve ptr to task name preservation buffer
	    # timr Trap any errors from wtx... layer. 
            if [catch {set wvGlobal(taskBuffID)  [wtxGopherEval "$wvGlobal(reg.sysMemTop) <+4 @l>"]} result] {
		catch [wtxToolDetach]

	        return 0
	    }
	    # timr
            if [catch {set wvGlobal(logHeaderId) [wtxGopherEval "$wvGlobal(reg.sysMemTop) <+8 @l>"]} result] {
		catch {wtxToolDetach}

	        return 0
	    }
        } 
    }
    return 1
}

proc onUploadSelect {} {
    global uploadConfig
    global wvGlobal

    set upload [controlSelectionGet wvUploadConfig.upload]

    switch $upload {
        0 {
           controlValuesSet wvUploadConfig.uploadPath $wvGlobal(localHost)
           controlEnable wvUploadConfig.uploadPath 0
          }
        1 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upTsfsF)
           controlEnable wvUploadConfig.uploadPath 1
          }
        2 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upTsfsS)
           controlEnable wvUploadConfig.uploadPath 1
          }
        3 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upSock)
           controlEnable wvUploadConfig.uploadPath 1
          }
        4 {
           controlValuesSet wvUploadConfig.uploadPath $uploadConfig(reg.upFile)
           controlEnable wvUploadConfig.uploadPath 1
          }
    }
}

proc wvConfigGetSyms {} {
    global wvGlobal

    # Assumes we are already connected to the target

    # We need to make sure the kernel supports
    # WV by checking for the tell tale symbols

    set symsNeeded "rBuffLibInit rBuffCreate rBuffReset rBuffDestroy rBuffVerify memSysPartId pmPartId evtBufferId wvEvtLogStart wvEvtLogStop wvObjInst wvSigInst wvUploadStart wvUploadStop wvEvtClassClearAll wvEvtClassSet wvEvtLogInit wvLogHeaderCreate wvLogHeaderUpload memPartCreate wvTaskNamesPreserve wvTaskNamesUpload"

    foreach sym $symsNeeded {
        if [catch {wtxSymFind -name $sym} symInfo] {
            endWaitCursor

	    if {[windViewBindToTarget $wvGlobal(targetName)] == 0} {
                messageBox -stopicon "The target $wvGlobal(targetName) is not responding"
                return 0
            } else {
                catch [wtxToolDetach]
            }

            messageBox -stopicon "The target $wvGlobal(targetName) does not currently include WindView support. If you have purchased WindView for this target, you need to include component WINDVIEW in this image."
            catch [wtxToolDetach]
            return 0
        }
        set wvGlobal($sym) [lindex $symInfo 1]
    }

    #
    # Get optional wvEvtBufferFullNotify symbol (should be present on an 
    # up-to-date target, but won't matter too much if it isn't)
    #

    set sym "wvEvtBufferFullNotify"
    if [catch {wtxSymFind -name $sym} symInfo] {
        if {$wvGlobal($sym) == -1} {

	    #
	    # print this message only once, not every time this routine
            # is called. Using -1 as the "first time through" indicator
	    #

	    messageBox "Can't find $sym on target\nEvent Buffer Full notification will be disabled"
	}
        set wvGlobal($sym) 0
    } else {
        set wvGlobal($sym) [lindex $symInfo 1]
    }

    # This symbol is present if VxWorks events are available
    set wvGlobal(wvEventInst) 0

    if {[catch {wtxSymFind -name "wvEventInst"} symInfo] == 0} {
    
	set wvGlobal(wvEventInst) [lindex $symInfo 1]        
    }

    if {[catch {wtxSymFind -name "wvNetInit"} symInfo] == 0} {
	set wvGlobal(wvNetInit) [lindex $symInfo 1]
    }
    
    # This function controls network instrumentation    
    if {[catch {wtxSymFind -name "wvNetEnable"} symInfo] != 0} {
    	set wvGlobal(wvNetEnable) 0
	set netIndex [lsearch $wvGlobal(reg.extraLibs) *netLib* ]
	set wvGlobal(reg.extraLibs) [lreplace $wvGlobal(reg.extraLibs) $netIndex $netIndex {netLib 0}]
	sysRegistryValueWrite HKEY_CURRENT_USER $wvGlobal(regRoot) extraLibs $wvGlobal(reg.extraLibs)
    } else {
	set wvGlobal(wvNetEnable) [lindex $symInfo 1]
    }
    
    return 1
}


proc wvInitGraph {} {
    global wvGlobal

    if {$wvGlobal(reg.upload) == 0} {

        if [catch {set wvGlobal(graphPort) [wvTargetDocCreate]} result] {
            puts "Failed to create graph for upload : $result"
        }

        if {$wvGlobal(graphPort) == 0} {
            messageBox -stopicon "Error creating graph for upload"
            return 0
        }
    } else {
        set wvGlobal(graphPort) 0
    }

    set wvGlobal(graphPath) [append "" /tgtsvr/TCP: $wvGlobal(localHost) : $wvGlobal(graphPort)]

    return 1
}


proc initStartWV {targetName} {
    global wvGlobal
    global wvConfig

    beginWaitCursor

    set wvGlobal(targetName) $targetName

    set wvGlobal(uplinkActive) 0

    if {[wvConfigGetSyms] == 0} {
        endWaitCursor

        return 0
    }

    # Clear Event log class

    # timr 
    # Catch errors from wtx... layer
    if { [catch {wtxDirectCall $wvGlobal(wvEvtClassClearAll)}] != 0} {
	endWaitCursor
        messageBox -stopicon "rBuff support cannot be initialised on the target"
        wtxToolDetach
        endWaitCursor

        return 0
    }

    # Check that rBuff library is initialised

    if {[catch {wtxDirectCall $wvGlobal(rBuffLibInit)}] != 0} {
        endWaitCursor
        messageBox -stopicon "rBuff support cannot be initialised on the target"
        wtxToolDetach
        endWaitCursor

        return 0
    }

    set endianFlag "-[string range [targetEndian] 0 0]"

    # Have a look on the target for an rBuff

    if {$wvGlobal(reg.defer) == 1} {
        #timr
	# Catch errors from wtx... layer
        catch {set wvGlobal(rBuffID) [wtxGopherEval "$wvGlobal(evtBufferId) @l"] }

        if {$wvGlobal(rBuffID) == 0} {
            set getNewRBuff 1
        } else {
            # An rBuff already exists

            # If the config hasn't changed just reset the buffer

            if {$wvGlobal(config.changed) == 0} {
	        #timr
		# Catch errors from wtx... layer
		catch {wtxDirectCall $wvGlobal(rBuffReset) $wvGlobal(rBuffID)}
                set getNewRBuff 0
            } else {
	        #timr
		# Catch errors from wtx... layer
		catch {wtxDirectCall $wvGlobal(rBuffDestroy) $wvGlobal(rBuffID)}

                set wvGlobal(rBuffID) 0
                set getNewRBuff 1
            }
        }
    } else {
        set getNewRBuff 1
    }

    set rBuffLocal [memBlockCreate $endianFlag]

    if {$getNewRBuff == 1} {
        set argLocal [memBlockCreate $endianFlag]

        set arg [wtxMemAlloc 28]

        if {$wvGlobal(reg.postm) == 0} {

           # Create a normal rBuff as specified
	    # timr
	    # Set default value to avoid exception later, then catch any exception from wtx...
	    set wvGlobal(srcPartitionID) 0
            catch {set wvGlobal(srcPartitionID) [wtxGopherEval "$wvGlobal(memSysPartId) @l"]}

            memBlockSet -l $argLocal 0 $wvGlobal(srcPartitionID)
            memBlockSet -l $argLocal 4 $wvGlobal(reg.minBufCnt)
            memBlockSet -l $argLocal 8 $wvGlobal(reg.maxBufCnt)

            switch $wvGlobal(reg.bufSize) {
                "1 KB"   {set size 1024}
                "2 KB"   {set size 2048}
                "4 KB"   {set size 4096}
                "8 KB"   {set size 8192}
                "16 KB"  {set size 16384}
                "32 KB"  {set size 32768}
                "64 KB"  {set size 65536}
                "128 KB"  {set size 131072}
                "256 KB"  {set size 262144}
                "512 KB"  {set size 524288}
                "1 MB"  {set size 1048576}
            }

            memBlockSet -l $argLocal 12 $size
        } else {

            # In Post-Mortem mode so make a partition and then a buffer
            # in specified region

            # Reserve first 16 bytes in memory region for preserving ptrs
            # in a known location. The size of the memory partition available
            # for the post-mortem log therefore needs to be reduced by the
            # same amount so that it still fits into the overall amount of
            # memory available. (SPR 21567). Changed from 8 bytes to 16 bytes
            # in SPR 21755.
            #
	    # i.e.
	    #     PM_Size + 16 = reg.physTop - reg.sysMemTop
            #
            # Pointers held in this area are:
            #     sysMemTop       : evtBufferId
            #     sysMemTop + 4   : taskBuffId
            #     sysMemTop + 8   : logHeaderId
            #     sysMemTop + 12  : unused
            #

            set wvGlobal(PM_Size) [expr $wvGlobal(reg.physTop) - $wvGlobal(reg.sysMemTop) - 16]
            set wvGlobal(PM_Base) [expr $wvGlobal(reg.sysMemTop) + 16]

	    #timr
	    # Catch any exception from wtx... layer 
            catch {set wvGlobal(srcPartitionID) [wtxDirectCall $wvGlobal(memPartCreate) $wvGlobal(PM_Base) $wvGlobal(PM_Size)]}

    	    set partLocal [memBlockCreate $endianFlag]
            memBlockSet -l $partLocal 0 $wvGlobal(srcPartitionID)
    	    wtxMemWrite $partLocal $wvGlobal(pmPartId)
            memBlockDelete $partLocal

            if {$wvGlobal(srcPartitionID) == 0} {
                messageBox -stopicon "Error creating post-mortem memory region"
		endWaitCursor

                return 0
            }

            # we should now have a new partition above system memory

            # Now create rBuff for event data

            # Set rBuff parameters - src partition, min and max buff count

            memBlockSet -l $argLocal 0 $wvGlobal(srcPartitionID)
            memBlockSet -l $argLocal 4 10
            memBlockSet -l $argLocal 8 10

            # set size of buffer taking into account task name buffer, rBuff
            # and individual buffer structs

            # Note 5% of available space is reserved for task-names

            set size [expr ((($wvGlobal(PM_Size) - 1024) / 20) * 19) / 10]

            memBlockSet -l $argLocal 12 $size
        }

        # threshold
        memBlockSet -l $argLocal 16 [expr $size / 2]

        # error handler
        memBlockSet -l $argLocal 20    0

        # options
        if {$wvGlobal(reg.cont) == 1} {
            set rBuffOption 0
        } else {
            # Set 'DEFERRED' option (so threshold sem isn't given)
            set rBuffOption 8
        }

        if {$wvGlobal(reg.postm) == 1} {
            # Set 'WRAPAROUND' option
            set rBuffOption [expr $rBuffOption | 1]
        }

        memBlockSet -l $argLocal 24    $rBuffOption

        wtxMemWrite $argLocal $arg

	#timr
	# Set default value to avoid exception later, and put catch around the wtx... function
	set wvGlobal(rBuffID) 0
        catch {set wvGlobal(rBuffID) [wtxDirectCall $wvGlobal(rBuffCreate) $arg]}

        wtxMemFree $arg
        memBlockDelete $argLocal

        if {$wvGlobal(rBuffID) == 0} {
            messageBox -stopicon "Event Buffer creation failed - too large?"
            endWaitCursor

            return 0
        }

        # rBuffLocal allocated above

        memBlockSet -l $rBuffLocal 0 $wvGlobal(rBuffID)

        wtxMemWrite $rBuffLocal $wvGlobal(evtBufferId)

        # don't delete rBuffLocal, we made need it later

        if {$wvGlobal(reg.postm) == 1} {

	    #
            # Arrange for task names to be preserved...
            # This has been moved to be after the ring buffer has been created
            # By doing this, the tRBuffMgr task has already been started
            # and it's name will therefore appear in the task names
            # preservation buffer which is about to be created. Previously,
            # this buffer was created before the ring buffer manager was
            # spawned, meaning that it was not put in the initial task names
            # hash table on its creation. Even when the ring buffer was created
            # it still did not get put into the hash table, because logging had
            # not been started at this time. (SPR 21350)
            #

            set wvGlobal(PM_TN_Size) 64

            ## Preserving task names"

	    #timr
	    # Set default value to avoid exception later, then catch any exception from the wtx... layer
	    set wvGlobal(taskBuffID) 0

	    catch {set wvGlobal(taskBuffID) [wtxDirectCall $wvGlobal(wvTaskNamesPreserve) $wvGlobal(srcPartitionID) $wvGlobal(PM_TN_Size)]}

            if {$wvGlobal(taskBuffID) == 0} {
                messageBox -stopicon "Creation of Post-Mortem task name preservation buffer failed"
                endWaitCursor

                return 0
            }
	}
    }

    #
    # set the event buffer full notification flag on the target to zero
    # since the ring buffer has just been created/reset and is evidently
    # empty
    #

    if {$wvGlobal(wvEvtBufferFullNotify) != 0} {
	# the symbol exists on the target

    	wvTargetLongWrite $wvGlobal(wvEvtBufferFullNotify) 0
    }

    # Bind the event buffer to all of the event points

    # timr
    catch {wtxDirectCall $wvGlobal(wvEvtLogInit) $wvGlobal(rBuffID)}

    # Clear all Object instrumentation
    # (this is not optimal, I admit)

    # wvObjInst OBJ_TASK, NULL, INSTRUMENT_OFF
    # wvObjInst OBJ_SEM,  NULL, INSTRUMENT_OFF
    # wvObjInst OBJ_MSG,  NULL, INSTRUMENT_OFF
    # wvObjInst OBJ_WD,   NULL, INSTRUMENT_OFF
    # wvObjInst OBJ_MEM,  NULL, INSTRUMENT_OFF
    # wvSigInst INSTRUMENT_OFF

    #timr
    # Make these safe from exception   
    catch {wtxDirectCall $wvGlobal(wvObjInst) 1 0 2}
    catch {wtxDirectCall $wvGlobal(wvObjInst) 2 0 2}
    catch {wtxDirectCall $wvGlobal(wvObjInst) 3 0 2}
    catch {wtxDirectCall $wvGlobal(wvObjInst) 4 0 2}
    catch {wtxDirectCall $wvGlobal(wvObjInst) 5 0 2}
    catch {wtxDirectCall $wvGlobal(wvSigInst) 2}

    if {$wvGlobal(wvEventInst) != 0} {
        catch {wtxDirectCall $wvGlobal(wvEventInst) 2}
    }

    set evtClass $wvGlobal(reg.base)

    # Now go through the selected additional libraries and turn them on

    if {$wvGlobal(reg.base) == 7} {
        foreach selection $wvGlobal(reg.extraLibs) {

            set lib   [lindex $selection 0]
            set isSet [lindex $selection 1]

            if {$isSet} {
                switch $lib {
		    taskLib {
			# wvObjInst OBJ_TASK, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 1 0 1}
		    }

		    semLib {
			# wvObjInst OBJ_SEM, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 2 0 1}
		    }

		    msgQLib {
			# wvObjInst OBJ_MSG, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 3 0 1}
		    }

		    wdLib {
			# wvObjInst OBJ_WD, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 4 0 1}
		    }

		    sigLib {
			# wvSigInst OBJ_WD, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvSigInst) 1}
		    }

                    eventLib {
                        # messageBox "eventLib"
                        if {$wvGlobal(wvEventInst) != 0} {
                            catch {wtxDirectCall $wvGlobal(wvEventInst) 1}
                        }
                    }

		    memLib {
			# wvObjInst OBJ_WD, NULL, INSTRUMENT_ON
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 5 0 1}
		    }

		    ioLib {
			#timr - added catch
			catch {wtxDirectCall $wvGlobal(wvObjInst) 6 0 1}
		    }

		    netLib {
			if {$wvGlobal(wvNetEnable) != 0} {
		    	    # symbol is present on the target: Call with the 
			    # correct instrumentation level.			
    		    	    catch {wtxDirectCall $wvGlobal(wvNetInit)}
    		    	    catch {wtxDirectCall $wvGlobal(wvNetEnable) [expr $wvGlobal(reg.netInstLevel) +1 ]}
			} else {
		    	    # show error dialog
			    wvAlertNoNetLib
			}
		    }

		    default {
			messageBox -stopicon "Additional library not recognised"
			endWaitCursor

			return
		    }
                }
            }
        }
    }

    #
    # Set the logging level.
    #

    #timr - added catch
    catch {wtxDirectCall $wvGlobal(wvEvtClassSet) $evtClass}

    #
    # The logging level was set, so now open the upload channel if 
    # upload should happen continuously.  If continuous upload then the
    # log's header will be created and uploaded later.  If upload is 
    # deferred, then it should be created now, and uploaded later.  
    # Creating the header now will accurately capture the tasks currently
    # running in the system.
    #

    if {$wvGlobal(reg.cont) == 1} {
        if {[wvOpenUpload] == 0} {
            endWaitCursor
            return 0
        }
        set wvGlobal(uplinkActive) 1
    } else {

	#
	# Upload is deferred.  Capture the log header now.  Set up for
	# post-mortem buffering if requested.
	# 

        set wvGlobal(uplinkActive) 0

        #timr
	# Set safe default value, then catch exception from wtx... 
	set wvGlobal(logHeaderId) 0

        catch {set wvGlobal(logHeaderId) [wtxDirectCall $wvGlobal(wvLogHeaderCreate) $wvGlobal(srcPartitionID)]}

        if {$wvGlobal(logHeaderId) == 0} {
            messageBox -stopicon "Error : wvLogHeaderCreate"
            endWaitCursor

            return
        }

        if {$wvGlobal(reg.postm) == 1} {

            # Here's where we reuse rBuffLocal

            memBlockSet -l $rBuffLocal 4 $wvGlobal(taskBuffID)
            memBlockSet -l $rBuffLocal 8 $wvGlobal(logHeaderId)

            wtxMemWrite $rBuffLocal $wvGlobal(reg.sysMemTop)
        }
    }

    memBlockDelete $rBuffLocal

    # We are now in a position to start logging!
    endWaitCursor

    return 1
}


proc wvOpenUpload {} {
    global wvGlobal

    if {$wvGlobal(uplinkActive) == 1} {
        messageBox -stopicon "wvOpenUpload: Uplink already exists!"
        return 0
    }

    if {[wvInitGraph] == 0} {
        return 0
    }

    # Check that rBuff library is initialised

    switch $wvGlobal(reg.upload) {
        0 {
            set pathResult [wvUploadTsfsSock $wvGlobal(targetName)]
	    if {$pathResult == "failed"} { return 0 }
	}

        1 -
        4 {
            ## "Upload to \[TSFS\] file
            set pathResult [wvUploadFile $wvGlobal(targetName)]
	    if {$pathResult == "failed"} { return 0 }
        }

        2 {
            ## Upload TSFS to socket
            set pathResult [wvUploadTsfsSock $wvGlobal(targetName)]
	    if {$pathResult == "failed"} { return 0 }
        }

        3 {
            ## Upload TCP/IP
            set pathResult [wvUploadTCP $wvGlobal(targetName)]
	    if {$pathResult == "failed"} { return 0 }
        }
    }

    #
    # By now, a ring buffer will have been created along with it's
    # rBuffMgr. If we are in continuous upload mode and the upload
    # path creation fails, the ring buffer will still be empty and
    # the rBuffMgr will be left dangling forever unless we take steps
    # to delete them. To do this, call the same tidy-up code as at
    # the end of a successful upload.
    #

    if {$pathResult <= 0} {
    	if {$wvGlobal(reg.cont) == 1} {
	    wvUploadCloseTidy
        }
	if {$pathResult == -1} {
	    statusMessageSet -normal "Upload cancelled by user"
	    return 0
	}

	switch $wvGlobal(reg.upload) {

	    0 {
		messageBox -stopicon "Path creation failed -\n\nPlease ensure that the target server file system (TSFS) is enabled."
	    }
	    1 {
		messageBox -stopicon "Path creation failed -\n\nPlease ensure that the target server file system (TSFS) is enabled and configured to support 'read/write'"
	    }

	    2 {
		messageBox -stopicon "Path creation failed -\n\nPlease ensure that the target server file system (TSFS) is enabled.\nAlso ensure socket/port listener is present."
	    }

	    3 {
		messageBox -stopicon "Path creation failed -\n\nPlease ensure socket/port listener is present"
	    }

	    4 {
		messageBox -stopicon "Path creation failed -\n\nPlease ensure your target supports NFS"
	    }

	    default {
		messageBox -stopicon "Path creation failed"
	    }
	}
        return 0
    }

    #
    # If upload is continuous, then the log-header will be created and 
    # uploaded just after starting the uploader, so that its taskname will
    # be part of the header.  If deferred, the header was created as soon
    # as the buffer was made.  Now we upload it so it is the first thing
    # in the log.
    #

    if {$wvGlobal(reg.cont) != 1} {

	# Set partition id if one doesn't exist
	if ![info exists wvGlobal(srcPartitionID)] {
	    set wvGlobal(srcPartitionID) 0
            catch {set wvGlobal(srcPartitionID) [wtxGopherEval "$wvGlobal(memSysPartId) @l"]}
        }

	if ![info exists wvGlobal(logHeaderId)] {
            set callArgs [lappend wvGlobal(wvLogHeaderCreate) $wvGlobal(srcPartitionID)]
            set wvGlobal(logHeaderId) [wvFuncCall -uiyield $callArgs]
        }

        set callArgs [lappend wvGlobal(wvLogHeaderUpload) $wvGlobal(logHeaderId) $wvGlobal(pathID)]
        set result [wvFuncCall -uiyield $callArgs]
    }

    # Get the uploader started and waiting

    set callArgs [lappend wvGlobal(wvUploadStart) $wvGlobal(rBuffID) $wvGlobal(pathID) $wvGlobal(reg.cont)]

    set wvGlobal(uploadID) [wvFuncCall -uiyield $callArgs]

    if {$wvGlobal(uploadID) == 0} {
        wtxToolDetach
        return 0
    }

    #
    # If uploading continously, take a snapshot of the tasks in the system,
    # and create a header (done by wvLogHeaderCreate).  The upload task is
    # already started, so that taskname will be in the snapshot.  If upload
    # is deferred, the header has already been created and uploaded.
    #

    if {$wvGlobal(reg.cont) == 1} {
    	set callArgs [lappend wvGlobal(wvLogHeaderCreate) $wvGlobal(srcPartitionID)]
        set wvGlobal(logHeaderId) [wvFuncCall -uiyield $callArgs]

        # Upload the header.

        set callArgs [lappend wvGlobal(wvLogHeaderUpload) $wvGlobal(logHeaderId) $wvGlobal(pathID)]
        set result [wvFuncCall -uiyield $callArgs]
    }

    set wvGlobal(uplinkActive) 1

    uploadRefreshView

    return 1
}




proc wvUploadCloseTidy {} {
    global wvGlobal

    # Now we've emptied the buffer on the target, delete it...

    #timr - added catch
    catch {set PM_partId [wtxGopherEval "$wvGlobal(pmPartId) @l"]}

    if {$wvGlobal(reg.postm) == 1 && $PM_partId == 0} {
	## zapping PM region
    	catch {wtxMemSet $wvGlobal(reg.sysMemTop) [expr $wvGlobal(reg.physTop) - $wvGlobal(reg.sysMemTop)] 0}
    } else {
	# destroying ring buffer
        #timr - added catch
	catch {wtxDirectCall $wvGlobal(rBuffDestroy) $wvGlobal(rBuffID)}
    }


    # set evtBufferId on target to NULL

    set endianFlag "-[string range [targetEndian] 0 0]"

    set memLocal [memBlockCreate $endianFlag]

    memBlockSet -l $memLocal 0 0

    wtxMemWrite $memLocal $wvGlobal(evtBufferId)

    set wvGlobal(rBuffID) 0

    if {$wvGlobal(reg.postm) == 1} {
        # we are Resetting rBuff ID for PM mode
        wtxMemWrite $memLocal $wvGlobal(reg.sysMemTop)
    }

    memBlockDelete $memLocal

    set wvGlobal(uplinkActive) 0

}




proc wvCloseUpload {} {
    global wvGlobal

    set returnVal 0

    if {$wvGlobal(uplinkActive) == 1} {

        # Stop logging in case it's still active

        #timr - added catch
	catch {wtxDirectCall $wvGlobal(wvEvtLogStop)}
        catch {wtxDirectCall $wvGlobal(wvEvtClassClearAll)}


        set callArgs [lappend wvGlobal(wvUploadStop) $wvGlobal(uploadID)]
	set wvUploadStopStatus 0
        catch { set wvUploadStopStatus [wvFuncCall -uiyield $callArgs] } exception

        if {$wvUploadStopStatus == 0} {
    	    if {$wvGlobal(reg.postm) == 1} {
            	set callArgs [lappend wvGlobal(wvTaskNamesUpload) $wvGlobal(taskBuffID) $wvGlobal(pathID)]
            	set result [wvFuncCall -uiyield $callArgs]
            	if {$result != 0} {
                    messageBox "uploaded task names : FAILED"
                    set returnVal -1
            	}
	    }
	} else { 
            set returnVal -1
        }

	if {$wvUploadStopStatus == 0} {

            switch $wvGlobal(reg.upload) {
                1 -
                4 {
                ## Closing \[TSFS\] file uplink

                set callArgs [lappend wvGlobal(fileUploadPathClose) $wvGlobal(pathID)]

                catch {wvFuncCall -uiyield $callArgs}
                }

                0 -
                2 {
                ## Closing TSFS to socket uplink
                set callArgs [lappend wvGlobal(tsfsUploadPathClose) $wvGlobal(pathID)]
                catch { wvFuncCall -uiyield $callArgs }
                }

                3 {
                ## Closing TCP/IP uplink
		#timr - added catch
		catch {wtxDirectCall $wvGlobal(sockUploadPathClose) $wvGlobal(pathID)}
                }      
	    }
        }

	catch {wvUploadCloseTidy}
    }

    uploadRefreshView
    return $returnVal
}

proc wvSymListGet {symsNeeded} {
    global wvGlobal

    foreach sym $symsNeeded {
        if [catch {wtxSymFind -name $sym} symInfo] {
	    set wvGlobal($sym) 0
        } else {
            set wvGlobal($sym) [lindex $symInfo 1]
        }
     }
     endWaitCursor
}


proc wvUploadFilePathCheck {path} {
    global wvGlobal
    global offset

    if {[string first "/tgtsvr" $path] == 0} {
	# puts "uploading via tsfs"
	return 1
    } elseif {[string compare "/null" $path] == 0} {
	# puts "uploading to bit bucket"
	return 1
    } else {
	# puts "attempting to upload via nfs ..."

    	set endianFlag "-[string range [targetEndian] 0 0]"

	#
	# Let's find out which device the IO system is going to try
	# to use for the upload.
    	# We are going to use the target routine 
    	#   DEV_HDR *iosDevFind
    	#	    (
    	#	    char  *name,       /* name of the device               */
    	#	    char  **pNameTail  /* where to put ptr to tail of name */
    	#	    )
    	# to see if the requested path is in the target's device table
    	# 

	# set up arguments to the target function call

    	set argLocal [memBlockCreate $endianFlag -string $path]
    	set argPath [wtxMemAllocStr $path]
    	wtxMemWrite $argLocal $argPath
    	set pNameTail [wtxMemAlloc 4]

    	# call the target function iosDevFind

    	set callArgs [lappend wvGlobal(iosDevFind) $argPath $pNameTail]
    	set pDevHdr [wvFuncCall -uiyield $callArgs]

    	wtxMemFree $argPath
    	wtxMemFree $pNameTail
    	memBlockDelete $argLocal

	if {$pDevHdr == 0} {
	    messageBox -stopicon "VxWorks is unable to determine upload device"
	    return 0
	}

    	# go get the actual driver number that's going to be used

    	catch {set uploadDrvNum  [lindex [wtxGopherEval "$pDevHdr +$offset(DEV_HDR,drvNum) @w "] 0]}
    	catch {set uploadDrvName [lindex [wtxGopherEval "$pDevHdr +$offset(DEV_HDR,name)   *$ "] 0]}

	# Let's find out about the nfs installation on the target

	wvSymListGet "nfsDrv nfsDrvNumGet"

	set nfsDrvNum 0
	if {$wvGlobal(nfsDrv) != 0} {
	    # nfs is linked in. See if we can get the driver number

	    if {$wvGlobal(nfsDrvNumGet) != 0} {
    	    	set nfsDrvNum [wvFuncCall -uiyield $wvGlobal(nfsDrvNumGet)]
	    }
	}

	#
	# Based on the values of nfsDrv, nfsDrvNum and uploadDrvNum
	# we can determine what we want to do
	#

	if {$nfsDrvNum != $uploadDrvNum} {
	    set msg [format {%s%s%s%s} \
		    "Configured path \"$path\" is not an NFS device.\n\n" \
		    "Configured path has been resolved to \"$uploadDrvName$path\"\n" \
		    "which MUST exist and be writeable for the upload to succeed.\n\n" \
		    "Continue?\n" ]

	    set result [messageBox -yesno $msg]
	    if {$result == "yes"} {
		return 1
	    } else {
	    	return -1
	    }
	}

	return 1
    }
}



proc wvUploadFile {targetName} {
    global wvGlobal

    # Assumes we are already connected to the target

    # OK, we need to make sure the kernel supports
    # this path by checking for the tell tale symbols

    set symsNeeded "fileUploadPathCreate fileUploadPathClose iosDevFind"

    if {$wvGlobal(reg.upload) == 4} {
	foreach sym $symsNeeded {
	    if [catch {wtxSymFind -name $sym} symInfo] {
		endWaitCursor
		messageBox -stopicon "The target $targetName does not support upload via file\n\
				      \nThe symbol '$sym' is missing on the target"
		return "failed"
	    }

	    set wvGlobal($sym) [lindex $symInfo 1]
	}
    } else {
	## We need a couple of extra symbols for tsfs otherwise we may be
	## attempting to upload to a path which cannot be created!

	set symsNeeded "$symsNeeded tsfsUploadPathCreate tsfsUploadPathClose"
	foreach sym $symsNeeded {
	    if [catch {wtxSymFind -name $sym} symInfo] {
		endWaitCursor
		messageBox -stopicon "The target $targetName does not support the upload mode: File via TSFS\n\
				      \nThe symbol '$sym' is missing on the target"
		return "failed"
	    }

	    set wvGlobal($sym) [lindex $symInfo 1]
	}
    }

    # We have the symbols we need

    # Check the library is initialised?

    # Create the upload path as requested

    switch $wvGlobal(reg.upload) {
        1 {set path $wvGlobal(reg.upTsfsF)}
        4 {set path $wvGlobal(reg.upFile)}
        0 -
        2 -
        3 {
            endWaitCursor
            messageBox -stopicon "Unexpected File Upload selection"
            return "failed"
          }
    }

    #
    # This upload is to a file path
    # Check to see if the supplied path exists as a device on
    # the target
    #

    set result [wvUploadFilePathCheck $path]
    if {$result <= 0} {
	return $result
    }

    set endianFlag "-[string range [targetEndian] 0 0]"
    set argLocal [memBlockCreate $endianFlag -string $path]

    set arg [wtxMemAllocStr $path]

    wtxMemWrite $argLocal $arg

    set callArgs [lappend wvGlobal(fileUploadPathCreate) $arg 0x600]

    set wvGlobal(pathID) [wvFuncCall -uiyield $callArgs]

    wtxMemFree $arg
    memBlockDelete $argLocal

    if {$wvGlobal(pathID) == 0} {
        # fileUploadPathCreate failed
        endWaitCursor

        return 0
    }

    return 1
}

proc wvUploadTsfsSock {targetName} {
    global wvGlobal

    # Assumes we are already connected to the target

    # OK, we need to make sure the kernel supports
    # this path by checking for the tell tale symbols

    set symsNeeded "tsfsUploadPathCreate tsfsUploadPathClose"

    foreach sym $symsNeeded {
        if [catch {wtxSymFind -name $sym} symInfo] {
            endWaitCursor
            messageBox -stopicon "The target $targetName does not support upload via TSFS sockets:\n\
		       \nThe symbol '$sym' is missing on the target"
            return "failed"
        }

        set wvGlobal($sym) [lindex $symInfo 1]
    }

    # We have the symbols we need

    # Check the library is initialised?

    switch $wvGlobal(reg.upload) {
        0 {set path $wvGlobal(graphPath)}
        2 {set path $wvGlobal(reg.upTsfsS)}
        1 -
        3 -
        4 {
            endWaitCursor
            messageBox -stopicon "Unexpected Upload selection"
            return "failed"
          }
    }

    # Create the upload path as requested

    ##  Check the pathname before we use the hostname and port
    if { [wvConfigPathTsfsSockCheck $path] == 0} { return "failed"}

    # now finish up the hostname

    set endianFlag "-[string range [targetEndian] 0 0]"
    set argLocal [memBlockCreate $endianFlag -string $wvGlobal(hostOrIP)]

    set arg [wtxMemAllocStr $wvGlobal(hostOrIP)]
    wtxMemWrite $argLocal $arg

    set callArgs [lappend wvGlobal(tsfsUploadPathCreate) $arg $wvGlobal(port)]

    set wvGlobal(pathID) [wvFuncCall -uiyield $callArgs]

    wtxMemFree $arg
    memBlockDelete $argLocal

    if {$wvGlobal(pathID) == 0} {
        # tsfsUploadPathCreate failed
        endWaitCursor

        return 0
    }

    return 1
}

proc wvUploadTCP {targetName} {
    global wvGlobal

    # Assumes we are already connected to the target
    # OK, we need to make sure the kernel supports
    # this path by checking for the tell tale symbols

    set symsNeeded "sockUploadPathCreate sockUploadPathClose"

    foreach sym $symsNeeded {
        if [catch {wtxSymFind -name $sym} symInfo] {
            endWaitCursor
            messageBox -stopicon "The target $targetName does not support upload via TCP/IP\n\
		       \nThe symbol '$sym' is missing on the target"
            return "failed"
        }

        set wvGlobal($sym) [lindex $symInfo 1]
    }

    # We have the symbols we need

    ##  Check the pathname before we use the hostname and port
    if { [wvConfigPathTCPSockCheck $wvGlobal(reg.upSock)] == 0} { return "failed"}

    set endianFlag "-[string range [targetEndian] 0 0]"
    set argLocal [memBlockCreate $endianFlag -string $wvGlobal(hostOrIP)]

    set arg [wtxMemAllocStr $wvGlobal(hostOrIP)]

    wtxMemWrite $argLocal $arg

    #timr - added catch
    set wvGlobal(pathID) 0
    catch {set wvGlobal(pathID) [wtxDirectCall $wvGlobal(sockUploadPathCreate) $arg $wvGlobal(port)]}

    wtxMemFree $arg
    memBlockDelete $argLocal

    if {$wvGlobal(pathID) == 0} {
        # sockUploadPathCreate failed
        endWaitCursor

        return 0
    }

    return 1
}

##############################################################################
#
# wvFuncCall - call a function synchronously (poll for the return value)
#
# The function is called via wtxFuncCall.  Then, we poll the target server
# waiting for the event that contains the return value of the function to
# arrive.  When it does, the return value supplied by WTX is returned by
# this function.  Events not pertaining to the function call are not expected
# and will result in an error being flagged.
#
# SYNOPSIS:
#   wvFuncCall [-int | -float | -double] [-nobreak] func [a0 ... a9]
#
# PARAMETERS:
#   -int, -float, -double: type of result
#   -uiyield: yield control to UI whilst waiting for result
#   func: address of function to call
#   a0...a9: integer function arguments
#
# RETURNS:
#   The return value of the function called on the target.
#
# ERRORS: N/A
#
# CAVEATS:
#   This function discards events it receives that do not pertain to the
#   function call while waiting for the return-value event.

set callTaskOptions 0x1b
set lastErrorValue  0

proc wvFuncCall {args} {
    global callTaskOptions
    global lastErrorValue

    set func 0
    set alist ""
    set type "-int"
    set nobreak 0
    set uiyield 0

    foreach arg $args {
	switch -glob -- $arg {
	    -uiyield    {set uiyield 1}
	    -i*		{set type "-int"}
	    -f*		{set type "-float"}
	    -d*		{set type "-double"}
	    default	{
		if {$func == 0} {
		    set func $arg
		} else {
		    lappend alist $arg
		}
	    }
	}
    }
    
    if {[llength $alist] > 10} {
	error "Too many arguments to function (limit: 10)"
    }

    set opt $callTaskOptions

    if {$type == "-float" || $type == "-double"} {
	# call the function with VX_FP_TASK set.
	set opt [expr $callTaskOptions | 0x8]
    }

    set fcExpr "wtxFuncCall $type -opt $opt $func $alist"

   # flush the queue
    while {[wtxEventGet] != ""} {}
    
    catch {set funcCallId [eval $fcExpr]}

    while 1 {
	if {$uiyield == 1} {
	    uiEventProcess
	}
	
	# timr
	# Safe method to check for event arrival
	# This causes wtxEventPoll to exit after a timeout, rather than stay in the routine
	# The loop continues, and the UI is serviced regularly

	catch {set event [wtxEventPoll 10 1]} result

	if {[lindex $event 0] == "EXCEPTION"
           || [lindex $event 0] == "TEXT_ACCESS"} {
		return 0
	}

	if {[lindex $event 0] == "CALL_RETURN"
	    && [lindex $event 1] == $funcCallId} {
	    if {[lindex $event 3]} {
		set lastErrorValue [expr [lindex $event 3]]
	    }            
	    return [lindex $event 2]
	}
    }
}

##############################################################################
#
# wvConfigBrowse - Browse for a file to which a log will be written
#
# This function is currently not supported.  It allows the user to select a
# file into which the data from the target buffer will be written.  To 
# implement this funtion you need to add a button to the wvUploadConfig dialog
# which has the callback wvConfigBrowse. disbling and enabling of the button
# on upload mode is advisable.
#
proc wvConfigBrowse {} {
    global wvGlobal

    set fname [fileDialogCreate -savefile -overwriteprompt -pathmustexist -filefilters "WV Event Log files (*.wvr)|*.wvr||" -okbuttontitle "OK" -title "Event Log File"]

    set fname [string trim $fname "{}"]

    set upload [controlSelectionGet wvUploadConfig.upload]

    if {$upload == 0} {
        set fname [format "/tgtsvr/%s" $fname]
        set wvGlobal(reg.upTsfsF) $fname
    }

    if {$upload == 4} {
        set wvGlobal(reg.upFile) $fname
    }

    controlValuesSet wvUploadConfig.uploadPath $fname
}

proc wvIntVal {dlgItem method varName origVarName minVal maxVal} {
    upvar $origVarName origVar
    upvar $varName var

    switch -regexp $method {
        [Ss]	{set origVar [controlValuesGet $dlgItem -edit]}
	default {set origVar [controlValuesGet $dlgItem]}
    }

    # parse out an integer value and a scale factor.
    # we accept an initial integer, in decimal or 0x123 form,
    # possibly followed by a scale factor of k, kb, m, or mb
    # (case insensitive).  

    set c [regexp {([1-9][0-9]*|0x[0-9a-fA-F]+) *([Kk][BbOo]?|[Mm][BbOo]?)?} \
        $origVar matchall matchInt matchScale]

    if {$c != 1} {
        controlFocusSet $dlgItem
	messageBox "Please supply an integer value."
	return 0
    }
        
    set scale 1 
    switch -regexp $matchScale {
        [Kk].*	{set scale 1024}
	[Mm].*	{set scale [expr 1024*1024]}
        default	{set scale 1}
    }

    set var [expr $scale * $matchInt]

    if {$var < $minVal || $var > $maxVal} { 
        controlFocusSet $dlgItem
	messageBox "Value out of range."
	return 0
    }

    return 1
}

proc wvConfigCancel {args} {
    windowClose wvConfig
}

proc wvConfigUploadCancel {args} {
    global wvGlobal

    windowClose wvUploadConfig

    #
    # The following windowActiveSet should not really be necessary, and it
    # isn't on an NT host. On a solaris host, particularly with the CDE window
    # manager, the wvConfig window can sometimes "hide" behind the main
    # windView window when the wvUploadConfig window is closed down. Forcing
    # the wvConfig window active now will cure this problem and is harmless on
    # NT, where it doesn't really need it.
    #

    windowActiveSet wvConfig
}

##############################################################################
#
# setButtonsForNoTarget - Reset control buttons when target is no responding
# 
# This function resets the buttons on the control dialog when the target is
# not responding for some reason
#
# RETURNS: n/a
# 

proc setButtonsForNoTarget {} {
    global wvGlobal
    
        # Target not responding

        #catch {wtxToolDetach} result
	windViewUnbindFromTarget $wvGlobal(targetName)

        lappend uploadInfo "{Not responding} {}"
        controlValuesSet eventLogging.uploadInfo $uploadInfo

        controlEnable wvTB.wvConfigButt 0
        controlEnable wvTB.wvUploadButt 0
        controlEnable wvTB.wvLookButt 1
        controlEnable wvTB.wvLookTimerButt 0
        controlEnable wvTB.wvStartButt 0
	controlEnable wvTB.wvStopButt 0

        # Disable Look on timer, but don't know how to toggle button!
	#controlCheckSet wvTB.wvLookTimerButt 0
	set wvGlobal(timerButtonActive) 0
        autoTimerCallbackSet eventLogging 0

        set wvGlobal(targetOK) 0
}

proc uploadRefreshView {} {
    global wvGlobal

    set uploadInfo    {}

    set handleList [wtxHandle]

    if { [llength $handleList] > 0 } {
        set currentHandle [lindex $handleList 0]
    }

    set connectedToTarget [windViewBindToTarget $wvGlobal(targetName)]
    if {$connectedToTarget == 1} {

        if {[wvConfigGetSyms] == 0} {

            # Target does not support WV 2.0

	    ## call the function to set the buttons
	    setButtonsForNoTarget
	    
	    ## here we just replace the 'Not responding' text with
	    ## 'not supported'
            lappend uploadInfo "{Not supported} {}"
            controlValuesSet eventLogging.uploadInfo $uploadInfo

            return 0
        }
    } else {
	## call function to set our button states
	setButtonsForNoTarget

	return 0
    }

    if {$wvGlobal(config.ok) == 0} {

        controlEnable wvTB.wvUploadButt 0

        wvConfigDialog $wvGlobal(targetName)

        if {$wvGlobal(config.ok) == 0} {
            lappend uploadInfo "{Not configured} {}"
            controlValuesSet eventLogging.uploadInfo $uploadInfo

	    set wvGlobal(timerButtonActive) 0
	    autoTimerCallbackSet eventLogging 0
            messageBox -stopicon "Cannot continue without a valid configuration"
            return 0
        }
    }

    if {$connectedToTarget == 1} {

        catch {set wvGlobal(rBuffID) [wtxGopherEval "$wvGlobal(evtBufferId) @l"]}
	catch {loggingStoppedCheck}

        controlEnable wvTB.wvLookButt 1
        controlEnable wvTB.wvLookTimerButt 1

	setStartStopButtons
    } else {
	## call function to set our button states
	setButtonsForNoTarget

        return 0
    }

    if {$wvGlobal(targetOK) == 0} {

        # target has been not responding (or somesuch)

        # Reset controls

        controlEnable wvTB.wvConfigButt 1
        controlEnable wvTB.wvUploadButt 1
        controlEnable wvTB.wvLookButt 1
        controlEnable wvTB.wvLookTimerButt 1
	setStartStopButtons

        set wvGlobal(targetOK) 1
    }

    switch $wvGlobal(reg.base) {
    1 {lappend uploadInfo "{Base Events} {Context Switch}"}
    3 {lappend uploadInfo "{Base Events} {Task State Transition}"}
    7 {lappend uploadInfo "{Base Events} {Additional Instrumentation}"}
    }

    set libList {}

    foreach lib $wvGlobal(reg.extraLibs) {
        if {[lindex $lib 1] == 1} {
            lappend libList [lindex $lib 0]
        }
    }

    if {$wvGlobal(reg.base) == 7} {
        if {$libList != {}} {
            lappend uploadInfo "{Additional Events} {$libList}"
        } else {
            lappend uploadInfo "{Additional Events} {(none)}"
        }
    }

    if {$wvGlobal(reg.defer) == 1} {
        lappend uploadInfo "{Upload Mode} {Deferred}"
    } else {
        if {$wvGlobal(reg.cont) == 1} {
            lappend uploadInfo "{Upload Mode} {Continuous}"
        } else {
            lappend uploadInfo "{Upload Mode} {Post-Mortem}"
        }
    }

    switch $wvGlobal(reg.upload) {
    0 {lappend uploadInfo "{Upload Path} {Direct to Graph}"}
    1 {lappend uploadInfo "{Upload Path} {File via TSFS}"}
    2 {lappend uploadInfo "{Upload Path} {Socket via TSFS}"}
    3 {lappend uploadInfo "{Upload Path} {Socket via TCP/IP}"}
    4 {lappend uploadInfo "{Upload Path} {File via NFS}"}
    }

    if {$wvGlobal(reg.cont) == 1} {
        if {$wvGlobal(uplinkActive) == 1} {
            lappend uploadInfo "{Link State} {Connected}"
        } else {
            lappend uploadInfo "{Link State} {Disconnected}"
        }
    }

    if {$wvGlobal(rBuffID) == 0} {
        lappend uploadInfo "{Event Buffer} {None}"

        controlEnable wvTB.wvUploadButt 0
    } else {

        set rBuffInfoList []

        catch {set rBuffInfoList [rBuffInfoGet $wvGlobal(rBuffID)]} result

        if {$rBuffInfoList == []} {
            lappend uploadInfo "{Event Buffer} {None}"

            controlEnable wvTB.wvUploadButt 0
        } else {

            lappend uploadInfo [format "{Event Buffer} {0x%x}" $wvGlobal(rBuffID)]

            set buffSize    [lindex $rBuffInfoList 4]
            set currBuffs   [lindex $rBuffInfoList 5]
            set maxBuffs    [lindex $rBuffInfoList 8]
            set maxABuffs   [lindex $rBuffInfoList 9]
            set content     [lindex $rBuffInfoList 11]
            set peakContent [lindex $rBuffInfoList 18]

            set percent [expr ($content * 100) / ($buffSize * $currBuffs)]

            set peakPercent [expr ($peakContent * 100) / ($buffSize * $maxBuffs)]

            lappend uploadInfo [format "{Current Content} {0x%x bytes}" $content]
            lappend uploadInfo [format "{        Percent} {%d%%}" $percent]
            lappend uploadInfo [format "{        Buffers} {%d}" $currBuffs]
            lappend uploadInfo [format "{Peak Content} {0x%x bytes}" $peakContent]
            lappend uploadInfo [format "{        Percent} {%d%%}" $peakPercent]
            lappend uploadInfo [format "{        Buffers} {%d (max %d)}" $maxABuffs $maxBuffs]

            if {$wvGlobal(reg.cont) == 0} {
                controlEnable wvTB.wvUploadButt 1
            } else {
                controlEnable wvTB.wvUploadButt 0
            }
        }
    }

    #catch {wtxToolDetach} result

#   set rBuffUtil [expr (20 * $peakPercent) / 100]
#   statusMessageSet -progress 20 [expr int($rBuffUtil)] "Event buffer utilisation"
#   uiEventProcess

    catch {controlValuesSet eventLogging.uploadInfo $uploadInfo} result

    if { [llength $handleList] > 0 } {
        if { [catch {wtxHandle $currentHandle}] != 0} {
	    set handle [wtxHandle]
	}
    }
    return 1
}

proc isWindViewLogging {} {
    global wvGlobal

    #
    # Get evtAction symbol
    #
    set evtAction "evtAction"
    if [catch {wtxSymFind -name $evtAction} symInfo] {
        if {$evtAction == -1} {

	    #
	    # print this message only once, not every time this routine
            # is called. Using -1 as the "first time through" indicator
	    #

	    messageBox "Can't determine logging state from target"
	return 0
	}
        return 0
    } else {
        set evtAction [lindex $symInfo 1]
    }

    if [catch {set evtActionResult [wtxGopherEval "$evtAction @l"]} result] {

        catch { wtxToolDetach }
	setButtonsForNoTarget
        return 0
    }

    ## We use a mask of 0x100 because the symbol evtAction holds the state of 
    ## Windview and Triggering logging. WindView uses the 9th bit and thus 
    ## will be ON when 0x100 and off when 0
    set evtActionResult [expr $evtActionResult & 0x100]

    if {$evtActionResult == 0} {
	return 0
    } else {
	return 1
    }
}

proc wvConfigRegistryInit {} {
    global wvGlobal
    global wvGlobal(DefKeyVal)

    # If the values for the WV configuration persistent settings
    # don't exist, set them up now.

    set hkcu HKEY_CURRENT_USER
    set root $wvGlobal(regRoot)
    foreach KeyVal $wvGlobal(DefKeyVal) {
	set val [lindex $KeyVal 0]
	set data [lindex $KeyVal 1]

	if [catch {sysRegistryValueRead $hkcu $root $val} result] {
	    sysRegistryValueWrite $hkcu $root $val $data
	    set wvGlobal(reg.$val) $data
	} else {
	    # the value already existed... set up the registry
	    # value global table.
	    set wvGlobal(reg.$val) $result
	}
    }
}

proc wvConfigResetCheck {} {

    if [windowExists resetCheck] {
        return
    }

    dialogCreate -name resetCheck \
        -parent wvConfig \
	-title "Reset Confirmation" \
	-w 165 -h 75 -nocontexthelp\
	-controls {
            { label -title "Are you sure you want to reset the Collection and Upload Configuration?\n\nYou will be unable to cancel the changes." \
              -x 15 -y 07 -w 140 -h 35} 
	    { button -default -name okButt 
	      -title "Yes" -callback wvConfigRegistryReset
              -x 15 -y 52 -w 50 -h 14} 
	    { button -name cancelButt -title "No" 
    	      -callback wvConfigResetCancel
    	      -x 100 -y 52 -w 50 -h 14} 
        }

}


proc wvConfigResetCancel {} {

    windowClose resetCheck

    #
    # The following windowActiveSet should not really be necessary, and it
    # isn't on an NT host. On a solaris host, particularly with the CDE window
    # manager, the wvConfig window can sometimes "hide" behind the main
    # windView window when the wvUploadConfig window is closed down. Forcing
    # the wvConfig window active now will cure this problem and is harmless on
    # NT, where it doesn't really need it.
    #

    windowActiveSet wvConfig
}

proc wvConfigRegistryReset {} {
    global wvGlobal
    global uploadConfig

    windowClose resetCheck

    #
    # The following windowActiveSet should not really be necessary, and it
    # isn't on an NT host. On a solaris host, particularly with the CDE window
    # manager, the wvConfig window can sometimes "hide" behind the main
    # windView window when the wvUploadConfig window is closed down. Forcing
    # the wvConfig window active now will cure this problem and is harmless on
    # NT, where it doesn't really need it.
    #

    windowActiveSet wvConfig

    # Reset the WV configuration persistent settings

    set hkcu HKEY_CURRENT_USER
    set root $wvGlobal(regRoot)

    foreach KeyVal $wvGlobal(DefKeyVal) {
	set val [lindex $KeyVal 0]
	set data [lindex $KeyVal 1]

	if [catch {sysRegistryValueWrite $hkcu $root $val $data} result] {
            messageBox "Error occured maintaining WindView configuration values in Windows Registry"
	} else {
	    set wvGlobal(reg.$val) $data
	    set uploadConfig(reg.$val) $data
	}
    }

    switch $wvGlobal(reg.base) {
        default -
        1 {controlSelectionSet wvConfig.base 0}
        3 {controlSelectionSet wvConfig.base 1}
        7 {controlSelectionSet wvConfig.base 2}
    }

    controlValuesSet wvConfig.xtraLibs $wvGlobal(reg.extraLibs)
}

proc wvConfigRegistryStore {} {
    global wvGlobal

    foreach KeyVal $wvGlobal(DefKeyVal) {
        set hkcu HKEY_CURRENT_USER
	set root $wvGlobal(regRoot)
	set val [lindex $KeyVal 0]

	sysRegistryValueWrite $hkcu $root $val $wvGlobal(reg.$val)
    }
}

proc wvConfigLaunch {} {
    global wvGlobal

    if { $wvGlobal(standalone) } {
        set tgtsvr_selected [selectedTargetValue]
    } else {
        set tgtsvr_selected [selectedTargetGet]
    }

    if {$tgtsvr_selected != ""} {
       wvConfigValuesGet $tgtsvr_selected
    }
}

proc wvConfigUiInit {} {
    global wvGlobal

    uplevel #0 source [wtxPath host resource tcl]wtxcore.tcl
    uplevel #0 source [wtxPath host resource tcl]shelcore.tcl
}

# Make sure config.ok is only initialised once

if [catch {set test $wvGlobal(config.ok)} result] {
    set wvGlobal(config.ok)   0
    set wvGlobal(configUp.ok) 1
    set wvGlobal(configPM.ok) 0
}

##############################################################################
#
# wtx errors will cause this routine to be executed
# We delete the handle, and rely on the caller to catch the exception
# The only errors trapped here are the serious comms errors

proc windViewErrorHandler {hwtx cmd err tag} {
global wvGlobal

    set errName [wtxErrorName $err]

    if {$errName == "AGENT_COMMUNICATION_ERROR" \
	    || $errName == "EXCHANGE_NO_TRANSPORT" \
	    || $errName == "AGENT_UNSUPPORTED_REQUEST" \
	    || $errName == "SVR_TARGET_NOT_ATTACHED"  \
	    || $errName == "EXCHANGE_TRANSPORT_DISCONNECT" } {

       	    wtxToolDetach

            controlEnable wvTB.wvConfigButt 0
            controlEnable wvTB.wvLookButt 1
            controlEnable wvTB.wvLookTimerButt 0
            controlEnable wvTB.wvStartButt 0
	    controlEnable wvTB.wvStopButt 0

	    autoTimerCallbackSet eventLogging 0
	    set wvGlobal(timerButtonActive) 0

	    # controlCheckSet wvTB.wvLookTimerButt 0

            set wvGlobal(targetOK) 0

	} 

    error $err
}
wvConfigRegistryInit
