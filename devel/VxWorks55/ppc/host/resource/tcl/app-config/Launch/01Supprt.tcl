# 01Supprt.tcl - technical support tcl library
#
# modification history
# --------------------
# 01v,27apr99,c_s  Fix SPR #27020
# 01u,23jun98,fle  made Tornado Support Request form don't display license
#                  number anymore
# 01t,21nov96,mem  redirect stderr of finger command so exit error status
#		   from finger doesn't abort the set command.
# 01s,26sep96,elp  added installation information in TSR report.
# 01r,19sep96,dbt  removed use of who am i for compatibility with HPUX10.
# 01q,04mar96,jco added userNameGet routine to avoid whoami calls 
#		   (SPR# 6103).
# 01p,05feb96,jco enhanced the mail subject line by making it be at most 50 char
#		   long using the beginning of the synopsis. Changed the 
#		   submissionConfirm routine (made a call to mailSend proc now 
#		   defined in host.tcl). Used the (Ok, Cancel) question style.
# 01o,22nov95,jco moved .wind and .wind/tsr directories creation to 
#		   supportPost proc, removed -p option to mkdir.
# 01n,13nov95,jcf fixed finger to have -hp.
# 01m,13nov95,jcf fixed finger to have -m.
# 01l,03nov95,jco changed tornado-beta@wrs.com to support@wrs.com, changed 
#		   .wpwr into .wind, at first use made created .wpwr and 
#		   .wpwr/tgtsvr, added a save call after submission (spr# 5235).
#		   added X11 release and window manager fields to mail header.
#		    (spr# 5180)
#		   added realLifeNameGet to avoid pipes (spr# 5121).
#		   changed tmp filename in formEditedTest (spr# 5094).
#		   added a warning when bad file names are passed for 
#		    inclusion (spr# 5204), 
#		   allowed the use of "Self Info" in a requesting session 
#		    without loosing edits(spr# 5267), 
#		   changed "Report to" into "Report to (e-mail of in-house
#		     Contact or WRS support), cleaned up.
# 01k,05oct95,c_s  uses new host.tcl interface for terminals, editing, etc.
#                    Subject line of sent mail messages is now done without
#                    using mail command line arguments.
# 01j,30aug95,jco  added product dependant subject field to mail command.
# 01i,23aug95,jco  changed vi -R to shwait more, used textEditor (spr 4493).
# 01h,23aug95,c_s  renamed Submit to "Send Mail" (SPR #4614).
# 01g,20jun95,jco  use of host.tcl defines for print and mail.
# 01f,27may95,jcf  added reportee to profile.  Enhanced product/version field.
# 01e,10apr95,c_s  moved, renamed, made rm's into rm -f's.
# 01d,05apr95,jco  attached noticePosted windows to its parent's dialog,
#		     presetted submitting recipient to tornado-alpha@wrs.com.
# 01c,21mar95,jco  added some sub-windows, ~ and * expansion.
#	            and a bundle of tests and messages.
# 01b,15mar95,jcf  added tsr history.
# 01a,05mar95,jcf  written.
#*/

#
# globals
#

set params		""	; # last supportPost parameters
set mainDialogInUse	0	; # dialogSupport_used boolean

#
# procedures
#

################################################################################
#
# supportPost - prepare the posting of dialogSupport.
#

proc supportPost {product cpu host bsp} {
    global env
    global params
    global mainDialogInUse

    # store parameters in global 

    set params "{$product} {$cpu} {$host} {$bsp}"

    # creates the .wind and .wind/tsr directory if necessary

    if {![file exists $env(HOME)/.wind]} {
	if [catch "exec mkdir $env(HOME)/.wind" result] {
	    error $result
	}
    }

    if {![file exists $env(HOME)/.wind/tsr]} {
	if [catch "exec mkdir $env(HOME)/.wind/tsr" result] {
	    error $result
        }
    }

    # post first the profile form if required

    if {![file exists $env(HOME)/.wind/profile]} {
	profilePost
	return
    }

    # initialize variables

    set unknown	"<unknown>"
    set reference $unknown

    # read profile information

    profileGet

    if {![file exists $env(HOME)/.wind/tsr/index]} {
       if [catch "exec touch $env(HOME)/.wind/tsr/index" result] {
	   error $result
       }
    }

    # filter out from index non existing filenames or files with no cookie

    set tsrHistory [exec cat $env(HOME)/.wind/tsr/index]
    foreach elt $tsrHistory {
	if {![file exists $elt] ||
	    [lindex [eval exec cat $elt] 0] != "WPWR_TSR_HISTORY_COOKIE"} {
	    set locus		[lsearch $tsrHistory $elt]
	    set tsrHistory	[lreplace $tsrHistory $locus $locus]
	}
    }

    # sort history list by decreasing date (actually "decreasing" name)

    set tsrHistory [lsort -decreasing $tsrHistory]

    # save the new index datum

    set fd	[open $env(HOME)/.wind/tsr/index w+]
    puts $fd	$tsrHistory
    close $fd

    # build tsr filename,

    set tsrFileName ""
    tsrFileNameCreate tsrFileName $tsrHistory

    # build the main information list

    set bugvals [list 							\
	$tsrFileName $tsrHistory $product $unknown [exec date] 		\
	$reference $host $cpu $bsp "<Other relevant hardware>"		\
	"<Please use keywords>"						\
	"<Please be specific>\n\n<Demonstration>\n\n<Workaround>"	\
        ]

    # populate the form

    dialogSetValues dialogSupport $bugvals

    # make a copy, add cookie and get rid of TSR File and TSR History fields

    set suppVals [linsert $bugvals 0 WPWR_TSR_HISTORY_COOKIE]
    set suppVals [lreplace $suppVals 1 2]

    # save initial values in file "templateEmpty.tmp" usefull at quit time

    set fd	[open $env(HOME)/.wind/tsr/templateEmpty.tmp w+]
    puts $fd	$suppVals
    close $fd
	
    # post the main dialog

    dialogPost	dialogSupport
    set mainDialogInUse 1
}

################################################################################
#
# tsrFileNameCreate - build a tsr file name.
#
# The name is derived from the date and adapts to existing files in index 
# Name format is: $env(HOME)/.wind/tsr/requestYEAR.MONTH.DAY_LASTNUMBER
#

proc tsrFileNameCreate {name tsrHistory} {
    global env
    upvar $name tsrFile

    set tsrFile		$env(HOME)/.wind/tsr/request
    set dateInfo 	[exec date]

    # append the YEAR. part in tsrFile

    set lastField	[lindex $dateInfo [expr [llength $dateInfo] -1]]
    append tsrFile	[string range $lastField 2 end].

    # encode the monthstring and append it

    set monthList "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"
    for {set i 1} {$i <12} {incr i} {
	if {[lindex $dateInfo 1] == [lindex $monthList [expr $i - 1]]} {
    	    append tsrFile "$i."
	    break
	}
    }

    # append the dayNumber_

    append tsrFile [lindex $dateInfo 2]_

    #
    # build the correct ending number derived from the existing filenames in
    # the index file (one more than the current higher last number)
    # First, extracts the index position of the file name with the same prefix
    # and the higher last number, if any (filter out a non integer ending name)
    #

    set locus [eval [format \
	"lsearch -regexp \$tsrHistory {^$tsrFile\[0-9\]+\$}"]]

    if {$locus == -1} {
	append tsrFile 1 ; # no so prefixed file, last number should be 1
    } {
	set fieldList	[split [lindex $tsrHistory $locus] _ ]
	set lastNum	[lindex $fieldList [expr [llength $fieldList] - 1]]
	append tsrFile	[expr $lastNum + 1]
    }
}

################################################################################
#
# supportUnpost - prepare the unposting of dialogSupport.
#

proc supportUnpost {} {
    global env
    global mainDialogInUse

    exec /bin/rm -f $env(HOME)/.wind/tsr/templateEmpty.tmp
    inclusionUnpost
    submissionUnpost
    profileUnpost

    dialogUnpost dialogSupport
    set mainDialogInUse 0
}

################################################################################
#
# supportTsrSelect - read tsr file and populate the form.
#

proc supportTsrSelect {selectedTsr} {
    global env

    set tsrHistory [exec cat $env(HOME)/.wind/tsr/index]
    set	bugvals [eval exec cat $selectedTsr]

    # delete the cookie field and add 2 fields for TSR File and TSR History

    set	bugvals [lreplace $bugvals 0 0]
    set	bugvals [linsert $bugvals 0 $selectedTsr $tsrHistory]

    dialogSetValues dialogSupport $bugvals
    dialogListSelect dialogSupport -top "TSR History" $selectedTsr
}

################################################################################
#
# supportSave - save a request in a file.
#

proc supportSave {{mode user}} {
    global env

    # read current form information

    set supportVals	[dialogGetValues dialogSupport]
    set fileName	[lindex $supportVals 0]

    # Check and expand the file name

    if {[fileNameCheckExpand fileName save] == -1} {
	return
    }

    # Notice the user to edit something before saving (if required)

    if {$mode == "user" &&[formEditedTest $fileName] != -1} {
	noticePost warning "Current state of support request is unchanged.\n\
		Edit request before saving."
	return
    }

    # add a cookie and get rid of TSR File and TSR History fields

    set supportVals 	[linsert $supportVals 0 WPWR_TSR_HISTORY_COOKIE]
    set supportVals	[lreplace $supportVals 1 2]

    # save current data in file

    set fd	[open $fileName w+]
    puts $fd	$supportVals
    close $fd

    # update tsr list and index file

    set tsrHistory	[exec cat $env(HOME)/.wind/tsr/index]

    if {[lsearch $tsrHistory $fileName] == -1} {
	# create an entry for the new filename
	set tsrHistory 	[linsert $tsrHistory 0 $fileName]

        # sort the list (in case of a deleted then reused filename)
        set tsrHistory 	[lsort -decreasing $tsrHistory]

        # update the TSR History display
        dialogSetValue	dialogSupport "TSR History" $tsrHistory

	# save the modified index data
        set fd 		[open $env(HOME)/.wind/tsr/index w+]
        puts $fd	$tsrHistory
        close $fd
    }
}

################################################################################
#
# supportDelete - deletes entry(ies) in index file.
#
# Note: real files are not deleted.
#

proc supportDelete {} {
    global env

    set tsrHistory	[exec cat $env(HOME)/.wind/tsr/index]
    set fileName	[dialogGetValue dialogSupport "TSR File"]

    # Check and expand the file name

    if {[fileNameCheckExpand fileName delete] == -1} {
	return
    }

    # because of expansion, fileName is now possibly a list of files.

    foreach elt $fileName {
        set index [lsearch $tsrHistory $elt]
        if {$index != -1} {
	    set tsrHistory [lreplace $tsrHistory $index $index]
        }
    }

    # save the new index file

    set fd 	[open $env(HOME)/.wind/tsr/index w+]
    puts $fd	$tsrHistory
    close $fd

    # clear out the product name/version we were working on.

    supportNew
}

################################################################################
#
# supportNew - 
#

proc supportNew {} {
    global params

    supportPost [lindex $params 0] [lindex $params 1] [lindex $params 2] \
		[lindex $params 3]
}

################################################################################
# 
# fileNameCheckExpand - check and expand file name. 
#
# check the file name, expand ~ and * if necessary and/or allowed.
#

proc fileNameCheckExpand {fileName caller} {
    global env
    upvar $fileName nameFile

    # a filename should be specified

    if {$nameFile == ""} {
	noticePost error "Specify file name."
	return -1
    }

    # control the ~ meta character expansion

    if {[regexp {^\~.+$} $nameFile] == 1} {
	# extract the first string before /
	set expandString [split $nameFile /]
	set firstString	[lindex $expandString 0]

	if [catch {file isdirectory $firstString} rep] {
	    noticePost error "$rep"
	    return -1
	}
    }

    # the filename should NOT be a directory name

    if {[file isdirectory $nameFile] == 1} {
	noticePost error "$nameFile is a directory !\nSpecify file name."
	return -1
    }

    # extract the pathString an the fileNameString

    set pathString	[file dirname	$nameFile]
    set fileNameString	[file tail	$nameFile]
    set nameFile	$pathString/$fileNameString

    # test if the pathString directory exists

    if { ![file isdirectory $pathString] } { 
	noticePost error "$pathString: unknown directory."
	return -1
    }

    # control the * meta character expansion

    if {$caller != "save" && [string first * $nameFile] != -1} {
	if {[noticePost question "Expand star?" "OK" "Cancel"] == 1} {

	    # create the ls list of the specified directory

	    set lsList [exec ls -a $pathString]

	    # glob_style matching between fileNameString and elt of lsList

	    set fileList	{}
	    foreach elt $lsList {
		if {[string match $fileNameString $elt]} {
			lappend fileList $pathString/$elt
		}
	    }
	    set nameFile $fileList
	    if {$nameFile == ""} {
		noticePost warning "No match!"
	    }
	} {
	    return -1 ; # cancel the expansion
	}	
    }
    return 1
}

################################################################################
#
# supportSubmit - prepare the posting of dialogSubmission.
#

proc supportSubmit {} {
    global env

    # close eventually unconcerned pop-up

    inclusionUnpost
    profileUnpost

    set fileName [dialogGetValue dialogSupport "TSR File"]

    # Notice the user to edit something before submitting

    if {[formEditedTest $fileName] == 0} {
	noticePost warning "Current state of support \
		request is unchanged.\nEdit request before submitting."
	return
    }

    if {![file exists $env(HOME)/.wind/profile]} {
	profilePost
	return
    }

    profileGet 

    set unknown	"<unknown>"
    set bugvals [dialogGetValues dialogSupport]

    set subjLine "TSR [lindex $bugvals 2] ([lindex $bugvals 10]"
    set subjLine "[string range $subjLine 0 49])"

    set report          "Subject: $subjLine"
    set report "$report\nSYNOPSIS OF PROBLEM     : [lindex $bugvals 10]"
    set report "$report\nAFFECTED MODULE         : [lindex $bugvals 3]"
    set report "$report\nPRODUCT NAME            : [lindex $bugvals 2]"
    set report "$report\nDATE                    : [lindex $bugvals 4]"
    set report "$report\nREPORTED BY             : $user"
    set report "$report\n    CONTACT             : $contact"
    set report "$report\n    COMPANY             : $company"
    set report "$report\n    EMAIL               : $email"
    set report "$report\n    ADDRESS             : $address"
    set report "$report\n    PHONE               : $phone"
    set report "$report\n    CUSTOMER REFERENCE# : [lindex $bugvals 5]"
    set report "$report\nSYSTEM CONFIGURATION    : "
    set report "$report\n    HOST                : [lindex $bugvals 6]"
    set report "$report\n    X11 RELEASE         : $unknown"
    set report "$report\n    WINDOW MANAGER      : $unknown"
    set report "$report\n    ARCH                : [lindex $bugvals 7]"
    set report "$report\n    BSP                 : [lindex $bugvals 8]"
    set report "$report\n    OTHER PERIPHERALS   : [lindex $bugvals 9]"
    set report "$report\nDESCRIPTION OF PROBLEM  :"
    set report "$report\n[lindex $bugvals 11]"
    
    # add installation information 

    if [file readable $env(WIND_BASE)/setup.log] {
    	set report "$report\n\nINSTALLATION HISTORY    :"
	set installFd [open $env(WIND_BASE)/setup.log r]
	set report "$report\n[read $installFd]"
	close $installFd
    }

    # save temporary the formatted global bug report

    set fd [open /tmp/support[pid] w+]
    puts $fd $report

    close $fd

    dialogSetValue dialogSubmission "Mail to" $reportee

    dialogPost dialogSubmission
}

proc realLifeNameGet {} {
    
    set cont [exec finger -mhp [userNameGet] 2> /dev/null]
    set ix1  [lsearch $cont "life:"]
    set ix2  [lsearch $cont "Directory:"]
    return [lrange $cont [incr ix1] [incr ix2 -1]]
}

proc userNameGet {} {
    global env

    return $env(USER)
}


################################################################################
#
# submissionUnpost - clean up temporary file and unpost dialogSubmission.
#

proc submissionUnpost {} {

    exec /bin/rm  -f /tmp/support[pid]
    dialogUnpost dialogSubmission
}

################################################################################
#
# submissionConfirm - send mail and clean quit.
#

proc submissionConfirm {} {

    set mailTo [dialogGetValue dialogSubmission "Mail to"]
    if { $mailTo == "" } {
	noticePost warning "Specify recipient."
	return
    }

    set ccTo [dialogGetValue dialogSubmission "CC to"]

    mailSend $mailTo $ccTo /tmp/support[pid]

    # TSR history must reflects the exact submitted requests 

    supportSave automatic

    submissionUnpost
}

################################################################################
#
# submissionView - display the submitting file.
#

proc submissionView {} {

    viewFile "Support Request View" /tmp/support[pid]
}

################################################################################
#
# submissionPrint - prints the submitting file and unpost dialogSubmission.
#

proc submissionPrint {} {
    global printCmd    

    exec $printCmd /tmp/support[pid]
    submissionUnpost
    dialogUnpost dialogSubmission
}

################################################################################
#
# supportEdit - performs the editing session and update description text field.
#

proc supportEdit {} {
    global env

    submissionUnpost
    profileUnpost
    inclusionUnpost

    set bugvals		[dialogGetValues dialogSupport]
    set description	[lindex $bugvals 11]

    set fd		[open /tmp/support[pid] w+]
    puts $fd		$description
    close $fd
    
    editFile -t "Support Request Description" /tmp/support[pid]

    # read back the new description

    set description	[exec cat /tmp/support[pid]]

    # update the display

    dialogSetValue	dialogSupport "Description of Problem" $description

    # destroy temporary file

    exec /bin/rm -f /tmp/support[pid]
}

################################################################################
#
# inclusionPost - prepare the posting of dialogInclusion.
#

proc inclusionPost {} {

    submissionUnpost
    profileUnpost
    dialogPost dialogInclusion
}

################################################################################
#
# inclusionUnpost - unpost dialogInclusion.
#

proc inclusionUnpost {} {

    dialogUnpost dialogInclusion
}

################################################################################
#
# inclusionConfirm -  performs file(s) inclusion in description text area.
#

proc inclusionConfirm {} {

    set descrData	[dialogGetValue dialogSupport "Description of Problem"]
    set fileNameList	[dialogGetValue dialogInclusion "Include File(s)"]

    if { $fileNameList == "" } {
	noticePost error "Specify at least one file name."
	return
    }

    # performs * expansion if required

    set badFiles ""
    foreach fileName $fileNameList {
	if {[fileNameCheckExpand fileName include] == -1} {
	    return
	    break
	}

        # after expansion, filename is now possibly a list of files

        foreach elt $fileName {
	    if { [file isfile $elt]} {
                set descrData "$descrData\n<************** Start include $elt "
	        set descrData "$descrData**************>\n"
	        set descrData "$descrData [eval exec cat $elt]"
	        set descrData "$descrData\n<************** End include $elt "
	        set descrData "$descrData**************>\n"
	    } {
	        lappend badFiles $elt
	    }
        }
    }

    # report possibly appending failure

    if {[llength $badFiles] > 0} {
        noticePost warning "Can't append ($badFiles) : no such file(s)." 
    }

    # update the display

    dialogSetValue dialogSupport "Description of Problem" $descrData	

    # quit inclusion dialog

    inclusionUnpost
}

################################################################################
#
# formEditedTest - check if form has been edited.
#
# check whether the currently displayed data different from what they where
# in the passed existing file or in the reference initial file.
# this is usefull to notice the user some edits may be lost, or doing something
# before saving or submitting.
#

proc formEditedTest {fileName} {
    global env

    # compare current data with stored file (or templateEmpty.tmp for new files)

    set tsrHistory	[exec cat $env(HOME)/.wind/tsr/index]
    if {[lsearch $tsrHistory $fileName] == -1} {
	set localName	$env(HOME)/.wind/tsr/templateEmpty.tmp
	set selection	0
    } {
	set localName	$fileName
	set selection	1
    }

    # read data, add cookie and get rid of TSR File and TSR History fields

    set supportVals	[dialogGetValues dialogSupport]
    set supportVals	[linsert $supportVals 0 WPWR_TSR_HISTORY_COOKIE]
    set supportVals	[lreplace $supportVals 1 2]

    # save temporary the Support Request displayed values

    set fd	[open /tmp/support2[pid] w+]
    puts $fd	$supportVals
    close $fd

    # compare displayed data and reference file

    catch {exec cmp /tmp/support2[pid] $localName} cmpReturnValue

    # delete the temporary file

    exec /bin/rm -f /tmp/support2[pid]

    if { $cmpReturnValue != ""} {
	return -1 ; # changes have occured
    } elseif {$selection == 0} {
	return 0 ; # No changes and fileName not in index
    } else { 
	return 1 ; # No changes and fileName is in index
    }
}

################################################################################
#
# quitPost - prepare the unposting of dialogSupport.
#

proc quitPost {} {
    global env

    # Notice the user in case some edits could be unsaved

    set fileName [dialogGetValue dialogSupport "TSR File"]
    if {[formEditedTest $fileName] == -1} {
	if {[noticePost question "Support Request not Saved. Really Quit?" \
	    "Cancel" "Quit"] == 1} {
	    return
	} 
    } 

    # quit silently

    supportUnpost
}

################################################################################
#
# profilePost - prepare the posting of dialogProfile.
#


proc profilePost {} {
    global env

    inclusionUnpost
    submissionUnpost

    set unknown		"<unknown>"

    if {![file exists ~/.wind/profile]} {
	set user        [realLifeNameGet]
	set contact	$user
	set company	$unknown
	set email	[userNameGet]@unknown.com
	set phone	$unknown
	set address	$unknown
	set reportee	"support@wrs.com"
    } else {
	profileGet
    }

    set profvals \
	[list $user $contact $company $email $phone $address $reportee]

    dialogSetValues	dialogProfile $profvals
    dialogPost		dialogProfile
}

################################################################################
#
# profileUnpost - unpost dialogProfile.
#

proc profileUnpost {} {

    dialogUnpost dialogProfile
}

################################################################################
# profileGet - read profile from file
#
# As a side effect, variables with the profiles elements are set in 
# the caller's context
#

proc profileGet {} {
    global env
    upvar \
	user user \
	contact contact \
	company company \
	email email \
	phone phone \
	address address \
	reportee reportee

    set profile	[exec cat $env(HOME)/.wind/profile]
    if {[llength $profile] >= 8} {
	# old profile, still containing license number.

	bindNamesToList \
	    {user contact company email phone _license address reportee} \
	    $profile

    } else {
	
	# new profile, not containing license number. 

	bindNamesToList \
	    {user contact company email phone address reportee} \
	    $profile

    }

}

################################################################################
#
# profileSave -  save profile information in file ~/.wind/profile.
#

proc profileSave {} {
    global env
    global mainDialogInUse

    set	fd	[open $env(HOME)/.wind/profile w+]
    puts $fd    [dialogGetValues dialogProfile] 
    close $fd

    profileUnpost

    if {$mainDialogInUse == 0} {
	supportNew
    }
}

#
# dialogs creation
#

#################################################################################
#
# dialog support creation
#

dialogCreate dialogSupport -size 880 700 \
    { \
    {text "TSR File" -y 5 -left 0 -right 68 } \
    {list "TSR History" -top "TSR File" -left 0 -right 68 -height 100 \
        supportTsrSelect} \
    {button "New" -y 25 -left 70 -right 85 supportNew} \
    {button "Edit" -y 25 -left 85+0 -right 100 supportEdit} \
    {button "Save" -top "New+5" -left 70 -right 85 supportSave} \
    {button "Append" -top "New+5" -left 85+0 -right 100 inclusionPost} \
    {button "Delete" -top "Save+5" -left 70 -right 85 supportDelete} \
    {button "Self Info" -top "Save+5" -left 85+0 -right 100 profilePost} \
    {button "Quit" -top "Delete+5" -left 70  -right 85 quitPost}\
    {button "Send Mail" -top "Delete+5" -left 85+0 -right 100 supportSubmit} \
    {text "Product/Release" -left 0 -right 25 -top "TSR History+40"} \
    {text "Affected Module" -left 25+0 -right 50 -top "TSR History+40"} \
    {text "Date" -left 50+0 -right 75 -top "TSR History+40"} \
    {text "Reference #" -left 75+0 -right 100 -top "TSR History+40"} \
    {text "Host/Version" -left 0 -right 25 -top "Product/Release+0"} \
    {text "Architecture" -left 25+0 -right 50 -top "Product/Release+0"} \
    {text "BSP" -left 50+0 -right 75 -top "Product/Release+0"} \
    {text "Peripherals" -left 75+0 -right 100 -top "Product/Release+0"} \
    {multitext "Synopsis Of Problem" -x 5 -height 80 -hspan \
	-top "Host/Version"} \
    {multitext "Description of Problem" -height 300 -hspan \
	-top @ -bottom 100} \
    }

#################################################################################
#
# dialog profile creation
#

dialogCreate dialogProfile -size 600 280 \
    { \
    {text "Reported By" -left 0 -right 33} \
    {text "Contact" -y 5 -left 33+0 -right 66} \
    {text "Company" -y 5 -left 66+0 -right 100} \
    {text "Email Address" -top "Company" -left 0 -right 33} \
    {text "Phone Number" -top "Company" -left 33+0 -right 66} \
    {text "Address" -top "Email Address" -hspan} \
    {text "Report To (e-mail of in-house Contact or WRS support)" \
	-top "Address" -hspan} \
    {button "Save" -width 100 -top \
	"Report To (e-mail of in-house Contact or WRS support)+15" \
	-left 50 -right 75 \
	profileSave} \
    {button "Cancel" -width 100 -top \
	"Report To (e-mail of in-house Contact or WRS support)+15" \
	-left 75 -right 100 \
        profileUnpost} \
    }

#################################################################################
#
# dialog inclusion creation
#

dialogCreate dialogInclusion -size 600 120\
    { \
    {text "Include File(s)" -y 5 -left 0 -hspan} \
    {button "Confirm" -top "Include File(s)+15" -left 25 -right 50 \
	inclusionConfirm} \
    {button "Cancel" -top "Include File(s)+15" -left 50 -right 75 \
	inclusionUnpost} \
    }

#################################################################################
#
# dialog submission creation
#

dialogCreate dialogSubmission -size 600 220\
    { \
    {label "Submitting Form" -y 10 -left 35} \
    {text "Mail to" -top @+15 -left 0 -right 50} \
    {text "CC to" -top @+15 -left 0 -hspan} \
    {button "Print" -width 100 -left 0 -right 25 -top "CC to+10" \
        submissionPrint} \
    {button "View" -width 100 -left 25 -right 50 -top "CC to+10" \
        submissionView} \
    {button "Cancel" -width 100 -left 50 -right 75  -top "CC to+10" \
        submissionUnpost} \
    {button "Send Mail" -width 100 -left 75 -right 100 -top "CC to+10" \
        submissionConfirm} \
    }
