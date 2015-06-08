# Download.tcl - Tornado (Windows and UNIX) object download code 
#
# Copyright (C) 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01h,23may02,cjs  fix spr 75926
# 01g,11mar99,ren  Adding auto C/C++ source language detect to download.
# 01f,02mar99,ren  Adding unload proc
# 01e,24feb99,ren  Partial fix for SPR 23385, load options determined by
#                  [GetWtxObjModuleLoadOptions]
# 01d,24nov98,j_k  Part of fix for SPRs #23183 and #22974.
# 01c,11nov98,cjs  cleanup to fix bug where running simulator is not detected 
# 01b,06oct98,j_k  made it more robust.
# 01l,23sep98,cjs  moved download code from 01Launch.win32.tcl,
#                  so that it can be shared on unix
#

set bDownloadCancelled 0
set dwnLdGlobal(dwnLdErrs) ""

proc dwnLdErrTimerCB {errText} {
    windowTimerCallbackSet dwnLdErrs 0 ""
    controlTextSet dwnLdErrs.dwnLdErrorText $errText
}

proc downloadErrorDlgShow {args} {
    proc errDlgInit {args} {
        set chkState [controlChecked dwnLdErrs.bWriteToFile]
        controlEnable dwnLdErrs.fileNameText $chkState
        controlEnable dwnLdErrs.fileBrowseDlgButton $chkState

        windowTimerCallbackSet dwnLdErrs -milliseconds 100 "dwnLdErrTimerCB $args"
    }

    dialogCreate -name dwnLdErrs -parent dlProgressDlg -title "Download Error" -width 280 -height 213 \
        -init "errDlgInit $args" -controls [list \
            [list label -name lb11 -title "Unresolved symbols encountered while downloading:" \
                -xpos 7 -ypos 7 -width 170 -height 9] \
            [list text -readonly -multiline -autovscroll -name dwnLdErrorText \
                -xpos 7 -ypos 20 -width 266 -height 150] \
            [list boolean -auto -name bWriteToFile -title "&Save log to file:" \
                -callback {
                    set chkState [controlChecked dwnLdErrs.bWriteToFile]
                    controlEnable dwnLdErrs.fileNameText $chkState
                    controlEnable dwnLdErrs.fileBrowseDlgButton $chkState
                } -xpos 7 -ypos 175 -width 62 -height 12] \
            [list text -name fileNameText -xpos 70 -ypos 175 -width 187 -height 12] \
            [list button -name fileBrowseDlgButton -title "..." -callback {
                    set pathName [fileDialogCreate -filefilters "All Files (*.*)|*.*||" \
	                -title "Error Log File" -okbuttontitle "OK" -savefile -overwriteprompt]
                    if {$pathName != ""} {
                        controlTextSet dwnLdErrs.fileNameText [lindex $pathName 0]
                    }
                } -xpos 260 -ypos 175 -width 14 -height 12] \
            [list button -default -name closeWnd -title "&Close" -callback {
                set chkState [controlChecked dwnLdErrs.bWriteToFile]
                set fileName [controlTextGet dwnLdErrs.fileNameText]
                if {$chkState == 1} {
                    set fileName [controlTextGet dwnLdErrs.fileNameText]
                    set fHandle ""
                    if {$fileName != "" && ![catch {open $fileName "w+"} fHandle]} {
                        puts $fHandle [controlTextGet dwnLdErrs.dwnLdErrorText]
                        close $fHandle
                    } else {
                        messageBox -stopicon "Invalid log filename!\n$fHandle"
                    }
                }
                windowClose dwnLdErrs
            } -xpos 213 -ypos 192 -width 60 -height 14]
        ]
}

proc startDownload {fileNames targetCPU strDownloadOptions} {
    controlPropertySet dlProgressDlg.filePrgrssCtrl -foreground Black -background DarkBlue

    windowExitCallbackSet dlProgressDlg "stopDownload 1"
    controlValuesSet dlProgressDlg.filePrgrssCtrl 0

    controlTextSet dlProgressDlg.currntFileName "Attaching to target..."
    windowTimerCallbackSet dlProgressDlg -milliseconds 50 \
	[list onFinalDownload $fileNames $targetCPU 0 2 $strDownloadOptions]
}

proc onFinalDownload {args} {
    global dwnLdGlobal
    global bDownloadCancelled
    global cpuType

    windowTimerCallbackSet dlProgressDlg 0 ""

    set fileNames [lindex $args 0]
    set nFiles [llength $fileNames]
    set targetCPU [lindex $args 1]
    set index [lindex $args 2]
    set mode [lindex $args 3]
	set strDownloadOptions [lindex $args 4]

    if {$mode == 1} {
        controlValuesSet dlProgressDlg.filePrgrssCtrl 0
        if [catch {wtxObjModuleLoadStart $strDownloadOptions \
					   [lindex $fileNames $index]} err] {
            messageBox "$err"
            stopDownload 0
            return
        }
        windowTimerCallbackSet dlProgressDlg -milliseconds 200 \
			[list onFinalDownload $fileNames $targetCPU $index 0 $strDownloadOptions]
    } elseif {$mode == 2} {

        set tgtsvr [::SelectedTarget::targetGet]
        if {![catch {wtxToolAttach $tgtsvr download} returnVal] &&
            ![catch {wtxTsInfoGet} tsInfo]} {

	        set typeId [lindex [lindex $tsInfo 2] 0]
                if {$targetCPU == -1} {     ;# if nothing was provided
	            set targetCPU $cpuType($typeId)
                }
		set status yes
	        if {$cpuType($typeId) != $targetCPU} {
		    if {$nFiles == 1} {
			set msg "Warning: '$fileNames' was built \
			    for CPU '$targetCPU',\nwhereas target CPU \
			    is '$cpuType($typeId).  Continue?"
		    } else {
			set msg "Warning: your files were built for CPU \
			    '$targetCPU',\nwhereas target CPU is \
			    $cpuType($typeId).  Continue?"
		    }
                    set status [messageBox -yesno $msg]
		}
		if {[string match $status yes]} { 
                    controlTextSet dlProgressDlg.currntFileName [fileNameAbbreviate [lindex $fileNames $index]]
                    controlPropertySet dlProgressDlg.aviPlayer -playstate 1
                    windowTimerCallbackSet dlProgressDlg -milliseconds 100 \
						 [list onFinalDownload $fileNames $targetCPU $index 1 $strDownloadOptions]
                } else {
                    stopDownload 0
	        }
        } else {
            messageBox -stopicon "Unable attach to the target server.\n$returnVal"
            stopDownload 0
        }
        return
    } elseif {$mode == 0} {
        if {![catch {wtxObjModuleLoadProgressReport} goOn]} {
            if {[llength $goOn] == 3 && ![string match 0x* [lindex $goOn 0]]} {
                if {[lindex $goOn 1] != 0} {
                    set progress [expr (100 * [lindex $goOn 2]) / [lindex $goOn 1]]
                    controlValuesSet dlProgressDlg.filePrgrssCtrl [expr (100 * [lindex $goOn 2]) / [lindex $goOn 1]]
                }
                windowTimerCallbackSet dlProgressDlg -milliseconds 5 \
				[list onFinalDownload $fileNames $targetCPU $index 0 $strDownloadOptions]
            } else {
                set goOn [split $goOn \n]
                if {[llength $goOn] > 1} {
                    set goOn [lrange $goOn 1 end]
                    set dwnLdGlobal(dwnLdErrs) \
                        [format "%s%s" $dwnLdGlobal(dwnLdErrs) "Errors while downloading [lindex $fileNames $index]:"]
                    foreach line $goOn {
                        set dwnLdGlobal(dwnLdErrs) "$dwnLdGlobal(dwnLdErrs)\r\n$line"
                    }
                    set dwnLdGlobal(dwnLdErrs) "$dwnLdGlobal(dwnLdErrs)\r\n\r\n"

                    set answer yes
                    if {$index < [expr $nFiles - 1]} {
                        set answer [messageBox -yesno "Encountered unresolved symbols \
                            while downloading.\n\nContinue downloading?"]
                    }
		    if {[string match $answer no]} { 
                        stopDownload 0
                    }
                }
                incr index
                if {$index < $nFiles} {
                    controlTextSet dlProgressDlg.currntFileName [fileNameAbbreviate [lindex $fileNames $index]]
                    windowTimerCallbackSet dlProgressDlg -milliseconds 100 \
					[list onFinalDownload $fileNames $targetCPU $index 1 $strDownloadOptions]
                } else {
                    stopDownload 0
                }
            }
        } elseif [string match {*WTX Error*} $goOn] {
            messageBox "$goOn"
            stopDownload 0
            return
        }
    }
}

proc stopDownload {bCancel} {
    global bDownloadCancelled
    global dwnLdGlobal

    windowExitCallbackSet dlProgressDlg ""
    windowTimerCallbackSet dlProgressDlg 0 ""
    
    controlPropertySet dlProgressDlg.aviPlayer -playstate 0
    set bDownloadCancelled $bCancel

    if {$bCancel == 1} {
        # Cancel while the module has been loaded.

        if [catch "wtxObjModuleLoadCancel" res] {
            puts "wtxObjModuleLoadCancel failed. cause: $res"
        }
    }

    if [catch "wtxToolDetach" err] {
        puts "error detaching from target: $err"
    }

    if {$dwnLdGlobal(dwnLdErrs) != ""} {
        downloadErrorDlgShow $dwnLdGlobal(dwnLdErrs)
    }

    windowClose dlProgressDlg
}


proc conjugateWas { list } {
	if {[llength $list] > 1 } {
		return "were"
	} else {
		return "was"
	}
}

proc onUnloadFiles {objFiles targetCPU} {
	set tgtsvr [SafeGetSelectedTargetServer]
	set listFailedObjectUnloads [list ]
	set listSuccessfulObjectUnloads [list ]
	if {[catch {
		set wtxhandleTargetServer [wtxToolAttach $tgtsvr unload]
	}] } {
		error "Could not attach to target $tgtsvr."
	}

	foreach objFile $objFiles {
		if {[catch { 
			set wtxidObject [lindex [wtxObjModuleFind $objFile] 0]
			wtxObjModuleUnload $wtxidObject
			lappend listSuccessfulObjectUnloads $objFile
		}]} {
			lappend listFailedObjectUnloads $objFile
		}			
	}
	catch { wtxToolDetach $wtxhandleTargetServer }
	if { [llength $listFailedObjectUnloads] > 0 } {
		#build a very informative error message
		set strSpacer ", "
		set listFailedObjectUnloads [FileTails $listFailedObjectUnloads]
		set listSuccessfulObjectUnloads [FileTails $listSuccessfulObjectUnloads]
		set strError "[join $listFailedObjectUnloads $strSpacer]\
            [conjugateWas $listFailedObjectUnloads] not loaded on\
            $tgtsvr"
		if { [llength $listSuccessfulObjectUnloads] > 0 } {
			append strError " but [join $listSuccessfulObjectUnloads $strSpacer]\
            [conjugateWas $listSuccessfulObjectUnloads] successfully unloaded."
		} else {
			append strError "."
		}
		error $strError
	}
}

proc SafeGetSelectedTargetServer { } {
	set tgtsvr [::SelectedTarget::targetGet]
    if {$tgtsvr == ""} {
        error "No target server selected.  Please select\
	        a target server from the list and then re-try."
	} else {
		return $tgtsvr
	}
}


##############################################################################
#
# onDownloadFiles - downloads files to the target.
#
# downloads the list of files to the target server.  For now if a second 
#   argument is provided and its value is 1, debugger is used to download
#   the files.  Otherwise it attaches to the currently selected target server
#   and downloads the files.  It also shows a progress meter on the status
#   bar.  In future, downloading using the debugger will be discontinued and
#   the progress meter show the true progress rather than the progress that
#   it shows now is after each is complete.
#
# SYNOPSIS:
#   onDownloadFiles
#
# PARAMETERS: 
#     args - list of filename and if should use debugger to download
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc onDownloadFiles {objFiles targetCPU} {
    global cpuType
    global launcherGlobals
    global bDownloadCancelled
    global dwnLdGlobal

    set strDownloadOptions [DownloadOptionsPage::GetWtxObjModuleLoadOptions]

    set dwnLdGlobal(dwnLdErrs) ""
    set tgtsvr [SafeGetSelectedTargetServer]

	# Write Download>LastDir to the registry
    if {$objFiles != ""} {
    	set fileDirs {}
        regsub -all {\\} $objFiles {\\\\} objFiles
        set nextDir ""
        foreach objFile $objFiles {
            set nextDir [file dirname $objFile]
            lappend fileDirs $nextDir
        }
        appRegistryEntryWrite Download LastDir $nextDir
	}

    set filenames $objFiles
    set bDownloadCancelled 0
    beginWaitCursor

	set aviFilePath [wtxPath host resource bitmaps Tornado]FileDownload.avi
	
	# replace all forward slashes with backslashes
	regsub -all {/} $aviFilePath {\\} aviFilePath
	
	dialogCreate -name dlProgressDlg -title "Downloading Files" \
	-width 200 -height 90 -nocontexthelp \
	-init [list startDownload $filenames $targetCPU $strDownloadOptions] \
	-controls [list \
			   [list animation -center -name aviPlayer -transparent \
				-avifile $aviFilePath \
				-xpos 7 -ypos 7 -width 189 -height 25] \
			   [list label -name currntFileName -title "" \
				-xpos 7 -ypos 35 -width 185 -height 9] \
			   [list progressmeter -left -name filePrgrssCtrl \
				-xpos 8 -ypos 49 -width 184 -height 14] \
			   [list button -default -name cancel -callback "stopDownload 1" \
				-title "Cancel" -xpos 72 -ypos 68 -width 50 -height 14] \
			  ]

    endWaitCursor

    if {$bDownloadCancelled != 0} {
		# Update the project with the target server...for later use
		::Workspace::projectTSSet $tgtsvr
        statusMessageSet "Download completed successfully."
    }
}

##############################################################################
#
# fileNameAbbreviate - shorten the filename for GUI displaying purpose
#
# This procedure shorten filename if it's longer than 40 chars.  The shorten
# filename will be in the form firstDir/.../lastDir/fileName.  There is no
# warranty that the resulted filename will be any shorter than the original one.
#
# SYNOPSIS
# fileNameAppreviate <fileName>
#
# PARAMETERS:
#    fileName : a path filename
#
# RETURNS: shorten filename
#
# ERRORS: N/A
#

proc fileNameAbbreviate {fileName} {
    set shortName $fileName

    if {[string length $fileName] > 40} {
        if [regexp {(^[^/]+)/.*/([^/]+/[^/]+$)} $fileName junk\
            firstPart lastPart] {
            set shortName "$firstPart/.../$lastPart"
        }
    }
    return $shortName
}

