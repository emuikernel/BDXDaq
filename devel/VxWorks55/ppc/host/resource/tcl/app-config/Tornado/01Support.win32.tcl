##############################################################################
# app-config/Tornado/01Support.win32.tcl - Tornado (Windows) Support E-mail
#
# Copyright (C) 1995-96 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01f,22jul02,cmf  update supported hosts and products
# 01e,02jan02,bwd  Fixed to retrieve customer's license number from WRSLicense
#                  registry value
# 01d,17mar99,rbl  adding keyboard shortcuts, making synopsis and description 
#                  fields -wantenter, and setting focus to first control initially
# 01c,18feb88,ren  fix SPR 24890, update fields for T2 release
# 01b,16feb99,l_k  change dllLoad to use debug version of SupportTcl-d.dll in
#                  Debug. Release version has been used resulting in 2
#                  different copies of MFC42 dll and MFC42D.dll present into
#                  address space simultaneously.
# 01a,10nov98,rbl added mod history, fixed SPR 22965, "Tools-->Support is broken" by fixing 
#                 various syntax errors.

##############################################################################
# SUPPORT MENU


set tclScript {
	##############################################################################
	# SUPPORT VIEW PROCEDURES

	#
	# supportViewInit
	#

	proc supportViewInit {args} {
		global tsrFileList
		global productReleaseList
		global hostVersionList
		global architectureList
		global bspList

		set supportViewControlList {
			{ label -title "&TSR File" -x 7 -y 10 -w 40 -h 8 }
			{ combo -name combo_tsrFile -x 42 -y 8 -w 160 -h 55
				-callback combo_tsrFile_callback }
	  		{ button -title "Sa&ve" -name button_saveTSR -x 207 -y 7 -w 50 -h 14
				-callback button_save_callback }
			{ button -title "&New" -name button_newTsr -x 261 -y 7 -w 50 -h 14
				-callback button_new_callback }
			{ button -title "R&emove" -name button_removeTsr -x 317 -y 7 -w 50 -h 14
				-callback button_rem_callback }
			{ frame -black -x 7 -y 25 -w 360 -h 2 }
			{ label -title "&OS/Version" -x 7 -y 30 -w 45 -h 8 }
	  		{ combo -name combo_hostVersion -x 7 -y 40 -w 70 -h 55
	  				-callback trigger_save_callback }
		   	{ label -title "&Product/Release" -x 87 -y 30 -w 75 -h 8 }
	 		{ combo -name combo_productRelease -x 87 -y 40 -w 70 -h 87 -editable
	 				-callback trigger_save_callback }
			{ label -title "&Date" -x 167 -y 30 -w 45 -h 8 }
			{ text -name edit_date -x 167 -y 40 -w 70 -h 13 -readonly }
			{ label -title "Re&ference No." -x 247 -y 30 -w 60 -h 8 }
			{ text -name edit_referenceNo -x 247 -y 40 -w 60 -h 13
					-callback trigger_save_callback }
			{ button -title "Prof&ile..." -name button_profile -x 317 -y 39 -w 50 -h 14
					-callback profileDialog }
			{ label -title "Architect&ure" -x 7 -y 55 -w 50 -h 8 }
			{ combo -name combo_architecture -x 7 -y 65 -w 70 -h 87 -editable
					-callback trigger_save_callback }
			{ label -title "&BSP" -x 87 -y 55 -w 40 -h 8 }
			{ combo -name combo_bsp -x 87 -y 65 -w 70 -h 87 -editable
					-callback trigger_save_callback }
			{ label -title "Affected &Module" -x 167 -y 55 -w 65 -h 8 }
			{ text -name edit_affectedModule -x 167 -y 65 -w 70 -h 13
					-callback trigger_save_callback }
			{ label -title "Perip&herals" -x 247 -y 55 -w 75 -h 8 }
			{ text -name edit_periphsHardware -x 247 -y 65 -w 120 -h 13
					-callback trigger_save_callback }
			{ label -title "Problem S&ynopsis (Please use keywords)" -x 7 -y 80 -w 200 -h 8 }
			{ text -name edit_productSynopsis -x 7 -y 90 -w 360 -h 22 -multiline
					-wantenter -callback trigger_save_callback }
			{ label -title "Prob&lem Description (Please be specific)" -x 7 -y 115 -w 200 -h 8 }
			{ text -name edit_problemDescription -x 7 -y 125 -w 360 -h 111 -multiline -vscroll
					 -wantenter -callback trigger_save_callback }
			{ group -title "A&ttachments (Please include Setup.log)"  -x 7 -y 238 -w 295 -h 60 }
			{ list -name list_attachments -x 13 -y 250 -w 224 -h 41
				   -callback list_attachments_callback }
			{ button -title "&Add..." -name button_addAttachment -x 245 -y 251 -w 50 -h 14
				-callback attachmentDialog }
			{ button -title "&Remove" -name button_removeAttachment -x 245 -y 270 -w 50 -h 14
				-callback {attachment_button_removeAttachment} }
			{ button -title "&Send Mail" -default -name button_sendMail -x 317 -y 242 -w 50 -h 14
				-callback sendEmailConfirmDialog }
			{ button -title "&Close" -name button_close -x 317 -y 260 -w 50 -h 14
				-callback button_close_callback }
			{ button -title "&Help" -helpbutton -x 317 -y 285 -w 50 -h 14 }
		}
	
		windowTitleSet supportView "Tornado Support Request"

		# setHelpFile {t:\wpwr\host\resource\help\bspw.hlp}
		foreach control $supportViewControlList {
			controlCreate supportView $control
		}
	
		menuItemEnable -bypath {&Tools Su&pport...} 0

		windowExitCallbackSet supportView {
			menuItemEnable -bypath {&Tools Su&pport...} 1
		}
	
		windowQueryCloseCallbackSet supportView supportViewExit
		controlValuesSet 	supportView.combo_hostVersion 			$hostVersionList
		controlValuesSet 	supportView.combo_productRelease 		$productReleaseList
		controlValuesSet 	supportView.combo_architecture 			$architectureList
		controlValuesSet 	supportView.combo_bsp 					$bspList

		newTsrCreate

		controlFocusSet supportView.combo_tsrFile
		windowPropertySet supportView -hori 1 -vert 1
	}

	proc button_close_callback {args} {
		if {[controlEnabled supportView.button_saveTSR] == "1"} {
			set result [messageBox -yesnoc "Do you want to save [controlSelectionGet  supportView.combo_tsrFile -string]"]
			switch $result {
				yes    { button_save_callback; windowClose supportView }
				no     { controlEnable supportView.button_saveTSR 0
					 	 windowClose supportView }
				cancel {}
			}
		} {
			windowClose supportView
		}
	}

	proc supportViewExit {args} {
		if {[controlEnabled supportView.button_saveTSR] == "1"} {
			set result [messageBox -yesnoc "Do you want to save [controlSelectionGet  supportView.combo_tsrFile -string]"]
			switch $result {
				yes    { button_save_callback; return 1 }
				no     { return 1 }
				cancel { return 0 }
			}
		}
		return 1
	}


	proc tsrLoad {args} {
		global tsrFileList
		global tsrDirPath
		global tsrfile
		global hostVersionList
		global prev_tsrFile
		global architectureList
		global bspList
		global productReleaseList

		set tsrFileName [controlSelectionGet supportView.combo_tsrFile -string]
		set prev_tsrFile $tsrFileName

		set tsrFileHandle [open $tsrDirPath/$tsrFileName r]
		fconfigure $tsrFileHandle -translation crlf
		set tsrFileContents [read $tsrFileHandle]
		close $tsrFileHandle

		bindNamesToList {
			tsrfile(host) tsrfile(product) tsrfile(date) tsrfile(reference)
			tsrfile(arch) tsrfile(bsp) tsrfile(module) tsrfile(hardware)
			tsrfile(synopsis) tsrfile(description) tsrfile(attachment)
		} $tsrFileContents

		if {[lsearch $architectureList $tsrfile(arch)] == -1} {
			lappend architectureList $tsrfile(arch)
			controlValuesSet supportView.combo_architecture $architectureList
		}
		if {[lsearch $bspList $tsrfile(bsp)] == -1} {
			lappend bspList $tsrfile(bsp)
			controlValuesSet supportView.combo_bsp $bspList
		}
		if {[lsearch $productReleaseList $tsrfile(product)] == -1} {
			lappend productReleaseList $tsrfile(bsp)
			controlValuesSet supportView.combo_bsp $productReleaseList
		}

		controlValuesSet 	supportView.edit_date 					$tsrfile(date)
		controlValuesSet 	supportView.edit_referenceNo 			$tsrfile(reference)
		controlValuesSet 	supportView.edit_affectedModule 		$tsrfile(module)
		controlValuesSet 	supportView.edit_periphsHardware 		$tsrfile(hardware)
		controlSelectionSet supportView.combo_hostVersion			[lsearch $hostVersionList $tsrfile(host)]
		controlSelectionSet supportView.combo_productRelease -string $tsrfile(product)
		controlSelectionSet supportView.combo_architecture -string	$tsrfile(arch)
		controlSelectionSet supportView.combo_bsp -string			$tsrfile(bsp)
		controlValuesSet 	supportView.edit_problemDescription 	$tsrfile(description)
		controlValuesSet 	supportView.edit_productSynopsis  		$tsrfile(synopsis)
		controlValuesSet	supportView.list_attachments			$tsrfile(attachment)
		controlSelectionSet supportView.list_attachments			-1

		tsrfile_update

		if {[controlEnabled supportView.button_saveTSR] == 1} {
			controlEnable supportView.button_saveTSR 0
		}
	}


	proc newTsrCreate {} {
		global tsrFileList
		global tsrDirPath
		global productReleaseList

		set newTsrName "Request-[clock format [clock seconds] -format {%Y-%m-%d}]"
		set numTsr [llength [glob -nocomplain $tsrDirPath/$newTsrName.*]]
		set numTsr [expr $numTsr + 1]
		set tsrFileName	$newTsrName.$numTsr

		if {[lsearch $tsrFileList $tsrFileName] == -1} {
			lappend tsrFileList $tsrFileName
		}
	
		newTsrTemplate
		controlValuesSet supportView.combo_tsrFile $tsrFileList
		controlSelectionSet supportView.combo_tsrFile -noevent [lsearch $tsrFileList $tsrFileName]
	
		if {[controlEnabled supportView.button_saveTSR] ==	1} {
			controlEnable supportView.button_saveTSR 0
		}
	}


	proc newTsrTemplate {} {
		global curDate
		global latestUserInfo
		global tsrDirPath
		global hostVersionList
		global prev_tsrFile
		global architectureList
		global bspList
		global productReleaseList

		controlValuesSet 	supportView.edit_date 					$curDate
		controlValuesSet 	supportView.edit_referenceNo 			"<Unknown>"
		controlValuesSet 	supportView.edit_affectedModule 		"<Unknown>"
		controlValuesSet 	supportView.edit_problemDescription 	"<Demonstration>\r\n\r\n<Work around>"
		controlValuesSet	supportView.list_attachments			""
		controlSelectionSet supportView.list_attachments			-1

		if {[file exist $tsrDirPath/../latest] == "1"} {
			set userInfoFileHandle [open $tsrDirPath/../latest r]
			set userInfoFileContents [read $userInfoFileHandle]
			close $userInfoFileHandle

			bindNamesToList {
				latestUserInfo(host) latestUserInfo(product) latestUserInfo(date) latestUserInfo(reference)
				latestUserInfo(arch) latestUserInfo(bsp) latestUserInfo(module) latestUserInfo(hardware)
			} $userInfoFileContents

			if {[lsearch $architectureList $latestUserInfo(arch)] == -1} {
				lappend architectureList $latestUserInfo(arch)
				controlValuesSet supportView.combo_architecture $architectureList
			}
			if {[lsearch $bspList $latestUserInfo(bsp)] == -1} {
				lappend bspList $latestUserInfo(bsp)
				controlValuesSet supportView.combo_bsp $bspList
			}
			if {[lsearch $productReleaseList $latestUserInfo(product)] == -1} {
				lappend productReleaseList $latestUserInfo(product)
				controlValuesSet supportView.combo_productRelease $productReleaseList
			}
			controlValuesSet 	supportView.edit_periphsHardware 		$latestUserInfo(hardware)
			controlSelectionSet supportView.combo_hostVersion			[lsearch $hostVersionList $latestUserInfo(host)]
			controlSelectionSet supportView.combo_productRelease -string $latestUserInfo(product)
			controlSelectionSet supportView.combo_architecture -string	$latestUserInfo(arch)
			controlSelectionSet supportView.combo_bsp -string			$latestUserInfo(bsp)
		} {
			controlValuesSet 	supportView.edit_periphsHardware 		"<Other Hardware Devices>"
			controlSelectionSet supportView.combo_hostVersion 			0
			controlSelectionSet supportView.combo_productRelease 		0
			controlSelectionSet supportView.combo_architecture 			0
			controlSelectionSet supportView.combo_bsp					0
		}

		tsrfile_update

		if {[controlEnabled supportView.button_newTsr] ==	1} {
			controlEnable supportView.button_newTsr 0
		}

		if {[controlEnabled supportView.button_saveTSR] ==	1} {
			controlEnable supportView.button_saveTSR 0
		}

		set prev_tsrFile [controlSelectionGet  supportView.combo_tsrFile -string]
	}


	proc tsrfile_update {args} {
		global tsrfile

		set tsrfile(host) 			[controlSelectionGet supportView.combo_hostVersion -string]
		set tsrfile(product) 		[controlValuesGet    supportView.combo_productRelease -edit]
		set tsrfile(date) 			[controlValuesGet 	 supportView.edit_date]
		set tsrfile(reference) 		[controlValuesGet 	 supportView.edit_referenceNo]
		set tsrfile(arch) 			[controlValuesGet 	 supportView.combo_architecture -edit]
		set tsrfile(bsp) 			[controlValuesGet    supportView.combo_bsp -edit]
		set tsrfile(module) 		[controlValuesGet 	 supportView.edit_affectedModule]
		set tsrfile(hardware) 		[controlValuesGet 	 supportView.edit_periphsHardware]
		set tsrfile(synopsis) 		[controlValuesGet 	 supportView.edit_productSynopsis]
		set tsrfile(description)	[controlValuesGet 	 supportView.edit_problemDescription]
		set tsrfile(attachment) 	[controlValuesGet 	 supportView.list_attachments]
	}


	proc button_save_callback {args} {
		global tsrDirPath
		global tsrFileList
		global tsrfile
		global prev_tsrFile

		tsrfile_update

		if [string match {Request*} [lindex $args 0]] {
			set tsrFileName $prev_tsrFile
		} {
			set tsrFileName [controlSelectionGet  supportView.combo_tsrFile -string]
		}

		if {$tsrFileName == ""} {
			set newTsrName "Request-[clock format [clock seconds] -format {%Y-%m-%d}]"
			set numTsr [llength [glob -nocomplain $tsrDirPath/$newTsrName.*]]
			set numTsr [expr $numTsr + 1]
			set tsrFileName	$newTsrName.$numTsr
			if {[lsearch $tsrFileList $tsrFileName] == -1} {
				lappend tsrFileList $newTsrName.$numTsr
			}

			controlValuesSet supportView.combo_tsrFile $tsrFileList
			controlSelectionSet supportView.combo_tsrFile -string $tsrFileName
		}

		set tsrFileHandle [open $tsrDirPath/$tsrFileName w]
		set latestInfoFileHandle [open $tsrDirPath/../latest w]
	
	 	puts $tsrFileHandle [list \
			$tsrfile(host) $tsrfile(product) $tsrfile(date) $tsrfile(reference)	\
			$tsrfile(arch) $tsrfile(bsp) $tsrfile(module) $tsrfile(hardware) \
			$tsrfile(synopsis) $tsrfile(description) $tsrfile(attachment)]
		close $tsrFileHandle

		puts $latestInfoFileHandle [list \
			$tsrfile(host) $tsrfile(product) $tsrfile(date) $tsrfile(reference)	\
			$tsrfile(arch) $tsrfile(bsp) $tsrfile(module) $tsrfile(hardware)]
		close $latestInfoFileHandle

		if {[controlEnabled supportView.button_saveTSR] ==	1} {
			controlEnable supportView.button_saveTSR 0
		}
		if {[controlEnabled supportView.button_newTsr] ==	0} {
			controlEnable supportView.button_newTsr 1
		}
		if {[controlEnabled supportView.button_removeTsr] == 0} {
			controlEnable supportView.button_removeTsr 1
		}
	}


	proc button_new_callback {args} {
		newTsrCreate
	}


	proc button_rem_callback {args} {
		global tsrDirPath
		global tsrFileList
	
		set tsrFileIndex [controlSelectionGet  supportView.combo_tsrFile]
		set tsrFileName [controlSelectionGet  supportView.combo_tsrFile -string]
		set tsrFileList [lreplace $tsrFileList $tsrFileIndex $tsrFileIndex]
		controlValuesSet supportView.combo_tsrFile $tsrFileList

		if {[controlEnabled supportView.button_saveTSR] == "1"} {
			controlEnable supportView.button_saveTSR 0
		}

		controlSelectionSet supportView.combo_tsrFile [expr [llength $tsrFileList] - 1]

		if {[file exist $tsrDirPath/$tsrFileName] == "1"} {
			catch {file delete $tsrDirPath/$tsrFileName}
		}
		
		if {[controlEnabled supportView.button_newTsr] == "0"} {
			controlEnable supportView.button_newTsr 1
		}

		set tsrFileListTmp [glob -nocomplain $tsrDirPath/*]
		if {[llength $tsrFileListTmp] == "0"} {
			button_new_callback
			# controlEnable supportView.button_removeTsr 0
		}
	}


	proc combo_tsrFile_callback {args} {
		global tsrDirPath
		global tsrfile
		global tsrFileList
		global prev_tsrFile

		set eventType [lindex [eventInfoGet supportView] 0]
		set tsrFileSel [controlSelectionGet supportView.combo_tsrFile -string]
		if {$eventType == "selchange" && $tsrFileSel != ""} {
			if {$tsrFileList == ""} {
				controlEnable supportView.button_removeTsr 0
			} {
				controlEnable supportView.button_removeTsr 1
			}

			if {[file exists $tsrDirPath/$tsrFileSel] == "1"} {
				if {[controlEnabled supportView.button_saveTSR] == "1"} {
					set result [messageBox -yesnoc "Do you want to save $prev_tsrFile"]
					switch $result {
						yes    { button_save_callback $prev_tsrFile; tsrLoad }
						no     { tsrLoad }
						cancel { controlSelectionSet supportView.combo_tsrFile -noevent
									[lsearch $tsrFileList $prev_tsrFile] }
					}
				} {
				   tsrLoad
				}
			} {
				newTsrTemplate
			}
	 	}
	}


	proc list_attachments_callback {args} {
		if {[controlValuesGet supportView.list_attachments] == ""} {
			 controlEnable supportView.button_removeAttachment 0
		} {
			 controlEnable supportView.button_removeAttachment 1
		}
	}


	proc trigger_save_callback {args} {
		if {[lindex [eventInfoGet supportView] 0] != "dropdown"} {
			if {[controlEnabled supportView.button_saveTSR] == "0"} {
				controlEnable supportView.button_saveTSR 1
			}
		}
	}


	##############################################################################
	# PROFILE DIALOG PROCEDURES

	#
	# profileDialog
	#
	proc profileDialog {args} {
		set profileDialogControlList {
		    { label -title "&User" -x 5 -y 8 -w 45 -h 8 }
		    { text -name edit_user -x 45 -y 7 -w 150 -h 13 }
		    { label -title "Co&mpany" -x 5 -y 26 -w 45 -h 8 }
		    { text -name edit_company -x 45 -y 25 -w 150 -h 13 }
		    { label -title "Co&ntact" -x 5 -y 44 -w 50 -h 8 }
		    { text -name edit_contact -x 45 -y 43 -w 150 -h 13 }
		    { label -title "Address &1" -x 5 -y 62 -w 45 -h 8 }
		    { text -name edit_staddress -x 45 -y 61 -w 150 -h 13 }
		    { label -title "Address &2" -x 5 -y 80 -w 45 -h 8 }
		    { text -name edit_ctyaddress -x 45 -y 79 -w 150 -h 13 }
		    { label -title "&Phone" -x 5 -y 98 -w 60 -h 8 }
		    { text -name edit_phone -x 45 -y 97 -w 150 -h 13 }
		    { label -title "&E-mail" -x 5 -y 116 -w 50 -h 8 }
		    { text -name edit_email -x 45 -y 115 -w 150 -h 13 }
		    { label -title "&Report To" -x 5 -y 134 -w 50 -h 8 }
		    { text -name edit_reportTo -x 45 -y 133 -w 150 -h 13 }
			{ group -title "Mail Service Configuration" -x 45 -y 152 -w 150 -h 40 }
			{ choice -newgroup -auto -title "M&API" -name radio_mapi -x 60 -y 160 -w 30 -h 14
				-callback  profileDialog_mailService }
			{ choice -auto -title "&SMTP" -name radio_smtp -x 60 -y 175 -w 34 -h 14
				-callback  profileDialog_mailService }
		    { label -title "Hos&t" -x 100 -y 177 -w 30 -h 8 }
		    { text -name edit_smtphost -x 120 -y 175 -w 60 -h 12 }
		    { button -title "&OK" -default -name button_ok -x 5 -y 200 -w 50 -h 14
				-default -callback profileDialog_buttonOk }
		    { button -title "&Cancel" -name button_cancel -x 60 -y 200 -w 50 -h 14
				-callback profileDialog_buttonCancel }
		    { button -title "&Help" -helpbutton -x 145 -y 200 -w 50 -h 14 }
		}

		dialogCreate -name profileDialog -title "Tornado Support Request: User Profile" \
		  -width 200 -height 219 -controls $profileDialogControlList \
		  -init {
			global profile

			controlValuesSet profileDialog.edit_user 		$profile(user)
			controlValuesSet profileDialog.edit_contact 	$profile(contact)
			controlValuesSet profileDialog.edit_company 	$profile(company)
			controlValuesSet profileDialog.edit_email 		$profile(email)
			controlValuesSet profileDialog.edit_phone 		$profile(phone)
			controlValuesSet profileDialog.edit_staddress 	$profile(staddress)
			controlValuesSet profileDialog.edit_ctyaddress 	$profile(ctyaddress)
			controlValuesSet profileDialog.edit_reportTo 	$profile(reportee)
			controlValuesSet profileDialog.edit_smtphost	$profile(smtphost)
			if {$profile(smtphost) != ""} {
				controlCheckSet profileDialog.radio_smtp 1
			} {
				controlCheckSet profileDialog.radio_mapi 1
				controlEnable profileDialog.edit_smtphost 0
			}
		  }
	}


	proc profileDialog_mailService {} {
		global profile

		if {[controlChecked profileDialog.radio_mapi]} {
			controlEnable profileDialog.edit_smtphost 0
			controlValuesSet profileDialog.edit_smtphost ""
			set profile(smtphost) ""
	} else  {
			controlEnable profileDialog.edit_smtphost 1
			if {[controlValuesGet profileDialog.edit_smtphost] == ""} {
				controlValuesSet profileDialog.edit_smtphost "mailhost"
				set profile(smtphost) "mailhost"
			}
		}
	}


	proc profileDialog_buttonCancel {args} {
		windowClose profileDialog
	}


	#
	# profileDialog_buttonOk - Called when the "OK" button is pressed.
	# Update the $profile array and save the entered values to the file
	# '$tsrDirPath/../profile'.
	#
	proc profileDialog_buttonOk {args} {
		global tsrDirPath
		#
		# __checkBlankField - Gets the contents of field '$fieldName' from the
		# Profile dialog; trims leading and trailing whitespace and finally
		# checks to see if the field is blank and sets the appropriate element
		# in the "tmpProfile" array.
		#
		proc __checkBlankField {fieldName arrayElementName} {
			upvar errorField errorField
			upvar tmpProfile tmpProfile

			set tmpProfile($arrayElementName) \
			  [string trimright [string trimleft [controlValuesGet profileDialog.$fieldName]]]
			if {![string length $tmpProfile($arrayElementName)]} {
				set errorField $fieldName
			}
		}

		set errorField ""

		set tmpProfile() ""

		__checkBlankField edit_user user
		if {$errorField == ""} {__checkBlankField edit_company company}
		if {$errorField == ""} {__checkBlankField edit_contact contact}
		if {$errorField == ""} {__checkBlankField edit_phone phone}
		if {$errorField == ""} {__checkBlankField edit_email email}
		if {$errorField == ""} {__checkBlankField edit_staddress staddress}
		if {$errorField == ""} {__checkBlankField edit_ctyaddress ctyaddress}
		if {$errorField == ""} {__checkBlankField edit_reportTo reportee}
		if {$errorField == ""} {
			if [controlChecked profileDialog.radio_smtp] {
			  __checkBlankField edit_smtphost smtphost
			} {
				set tmpProfile(smtphost) ""
			}
		}

		if {$errorField != ""} {
			messageBox -excl "Please do not leave any of the fields blank."
			# @@@ controlFocusSet does not work.
			controlFocusSet profileDialog.$errorField
		} {
			global profile

			# copy the temp. profile into the real profile
			set profile(user)		$tmpProfile(user)
			set profile(contact)	$tmpProfile(contact)
			set profile(company) 	$tmpProfile(company)
			set profile(email)		$tmpProfile(email)
			set profile(phone)		$tmpProfile(phone)
			set profile(staddress)	$tmpProfile(staddress)
			set profile(ctyaddress)	$tmpProfile(ctyaddress)
			set profile(reportee)	$tmpProfile(reportee)
			set profile(smtphost)	$tmpProfile(smtphost)

			set proFile [open $tsrDirPath/../profile w]
			puts $proFile [list \
				$profile(user) $profile(contact) $profile(company) $profile(email) \
				$profile(phone) $profile(license) $profile(staddress) $profile(ctyaddress) \
				$profile(reportee) $profile(smtphost)]
			close $proFile

			windowClose profileDialog
		}
	}


	##############################################################################
	# ATTACHMENT RELATED PROCEDURES

	#
	# attachmentDialog - This brings up a simple dialog so the user can enter
	# the name of a file to attach.
	#
	proc attachmentDialog {args} {
		set selectedFiles [fileDialogCreate -title "Attachment File Browser" \
		                  	-filefilters "All Files (*.*)|*.*||" \
	 	                 	-multisel -okbuttontitle "&Attach" ]

		if {$selectedFiles != ""} {
			global tsrfile

			foreach selectedFile $selectedFiles {
				lappend tsrfile(attachment) $selectedFile
			}
			controlValuesSet supportView.list_attachments $tsrfile(attachment)
			controlSelectionSet supportView.list_attachments [expr [llength $tsrfile(attachment)] - 1]
			controlEnable supportView.button_saveTSR 1
		}
	}


	proc attachment_button_removeAttachment {args} {
		global tsrfile

		set selectedItem [controlSelectionGet  supportView.list_attachments]
		set tsrfile(attachment) [lreplace $tsrfile(attachment) $selectedItem $selectedItem]
		controlValuesSet supportView.list_attachments $tsrfile(attachment)
		controlSelectionSet supportView.list_attachments [expr [llength $tsrfile(attachment)] - 1]
		controlEnable supportView.button_saveTSR 1
	}


	##############################################################################
	# SUPPORT E-MAIL SEND CONFIRMATION DIALOG PROCEDURES

	#
	# sendEmailConfirmDialog
	#
	proc sendEmailConfirmDialog {args} {
		global profile
		global tsrfile
		global report

		if {[controlValuesGet supportView.edit_productSynopsis] == ""} {
			messageBox -excl "Please do not leave the \"Problem Synopsis\" field blank."
			return
		}

		set sendEmailConfirmControlList {
			{ label -title "TSR File " -x 5 -y 5 -w 40 -h 8 }
			{ label -title ""
					-name label_fileName -x 50 -y 5 -w 200 -h 8 }
			{ label -title "E-mail &To" -x 5 -y 22 -w 45 -h 8 }
			{ text -name edit_eMailTo -x 50 -y 20 -w 150 -h 13 }
			{ label -title "&Cc" -x 5 -y 39 -w 45 -h 8 }
			{ text -name edit_Cc -x 50 -y 37 -w 150 -h 13 }
			{ label -title "&Bcc" -x 5 -y 56 -w 50 -h 8 }
			{ text -name edit_Bcc -x 50 -y 54 -w 150 -h 13 }
			{ button -title "&OK" -default -name button_ok -x 5 -y 72 -w 50 -h 14
				-callback send_button_ok_callback }
			{ button -title "C&ancel" -name button_cancel -x 60 -y 72 -w 50 -h 14
				-callback send_button_cancel_callback }
			{ button -title "&Help" -helpbutton -x 150 -y 72 -w 50 -h 14 }
		}

		dialogCreate -name sendEmailConfirmDialog -title "TSR Submission Confirmation" \
		  -w 205 -height 92 -controls $sendEmailConfirmControlList \
		  -init {
			  global profile

			  controlValuesSet sendEmailConfirmDialog.label_fileName [controlSelectionGet  supportView.combo_tsrFile -string]
		      controlValuesSet sendEmailConfirmDialog.edit_eMailTo $profile(reportee)
		      controlValuesSet sendEmailConfirmDialog.edit_Cc $profile(email)
		  }
	}


	proc send_button_cancel_callback {args} {
		windowClose sendEmailConfirmDialog
	}


	proc send_button_ok_callback {args} {
		global profile
		global tsrfile
		global mailDate

		button_save_callback

		# Create report
		set report ""
		append report "SYNOPSIS OF PROBLEM         : $tsrfile(synopsis)\n"
		append report "AFFECTED MODULE             : $tsrfile(module)\n"
		append report "PRODUCT NAME                : $tsrfile(product)\n"
		append report "DATE                        : $tsrfile(date)\n"
		append report "REPORTED BY                 : $profile(user)\n"
		append report "    CONTACT                 : $profile(contact)\n"
		append report "    COMPANY                 : $profile(company)\n"
		append report "    LICENSE NUMBER          : $profile(license)\n"
		append report "    EMAIL                   : $profile(email)\n"
		append report "    ADDRESS                 : $profile(staddress)\n"
		append report "                            : $profile(ctyaddress)\n"
		append report "    PHONE                   : $profile(phone)\n"
		append report "    CUSTOMER REFERENCE#     : $tsrfile(reference)\n"
		append report "SYSTEM CONFIGURATION        :\n"
		append report "    HOST                    : $tsrfile(host)\n"
		append report "    ARCH                    : $tsrfile(arch)\n"
		append report "    BSP                     : $tsrfile(bsp)\n"
		append report "    OTHER PERIPHERALS       : $tsrfile(hardware)\n"
		append report "\nDESCRIPTION OF PROBLEM  :\n$tsrfile(description)\n"

		if ![catch {open [wtxPath]setup.log "r"} fp] {
		    append report "\nINSTALLATION HISTORY:\n"
		    append report [read $fp]
		    close $fp
                }

		set sender $profile(email)

		#
		# _recipientListSplit - Splits the given list wherever a comma is found;
		# if no comma is found, ensures that the returned list is a list with a
		# single element.
		proc _recipientListSplit {recipList} {
			set recipList [string trimright [string trimleft $recipList]]
			if [regsub -all {,[ ]*} $recipList \n recipList] {
				set recipList [split $recipList \n]
			} elseif {$recipList != ""} {
				set recipList [format \{$recipList\}]
			}
			return $recipList
		}
		set toList [_recipientListSplit [controlValuesGet sendEmailConfirmDialog.edit_eMailTo]]
		set ccList [_recipientListSplit [controlValuesGet sendEmailConfirmDialog.edit_Cc]]
		set bccList [_recipientListSplit [controlValuesGet sendEmailConfirmDialog.edit_Bcc]]

		set subject "$tsrfile(product): $tsrfile(synopsis)"
		set msgContents $report
		set server $profile(smtphost)
		set attachments [controlValuesGet supportView.list_attachments]

		windowClose sendEmailConfirmDialog

		beginWaitCursor

		# Are we using the MAPI mail service or the SMTP server?
		if {$profile(smtphost) == ""} {
			set mailCommand {mapiMailSend $toList $ccList $bccList \
								$subject $msgContents $attachments}
		} {
			set mailCommand {smtpMailSend $mailDate $sender $toList $ccList \
								 $bccList $subject $msgContents $server $attachments}
			set msgContents [split $msgContents \n]
		}

		if [catch $mailCommand error] {
			messageBox -stop "Error sending mail: $error."
		} {
			messageBox -info "Tornado Service Request: '[controlSelectionGet  supportView.combo_tsrFile -string]' sent successfully."
		}

		endWaitCursor
	}


	#########
	# smtpMailSend - Using the socket API provided in Tcl, this routine
	# sends e-mail via the SMTP protocol.
	#
	# Arguments include:
	#
	#    sender - "From:" name of the SMTP sender
	#    toList - "To:" a list containing SMTP recipent addresses
	#    ccList - "Cc:" a list containing SMTP "Cc:" recipent addresses
	#    bccList - "Bcc" a list containing SMTP "Bcc:" (blind) recipent addresses
	#    subject - the subject line for the message
	#    msgContents - the name of the file containing the message to be sent
	#    server - the SMTP server name
	#    attachmentList - a list of files to be attached
	#
	proc smtpMailSend_NEW {mailDate sender toList ccList bccList subject \
					   msgContents server attachmentList} {
		set error ""
		set msgFile ""
		set smtpSocket ""

		# Determine the name of this host to be provided to the SMTP server.
		if [catch {info hostname} thisHostName] {
    		set error "unable to determine local IP host name: $thisHostName"
		}

		if {$error == ""} {
			# Connect to the SMTP server
			if {[catch {socket $server "smtp"} smtpSocket] &&
				[catch {socket $server "25"} smtpSocket]} {
				set error "unable to connect to SMTP server '$server'"
				set smtpSocket ""
			}
		}

		if {$error == ""} {
			fconfigure $smtpSocket -blocking 1 -buffering none

			# The first line from the SMTP server should be "220".
			if {[catch {gets $smtpSocket} response] ||
				![string match "220*" $response]} {
	    		set error "SMTP server error: $response"
			}
		}

		if {$error == ""} {
			# "Login" to the SMTP server and response should be "250".
			set message "HELO $thisHostName"
			if {[catch {puts $smtpSocket $message} response] ||
				[catch {flush $smtpSocket} response] ||
				[catch {gets $smtpSocket} response] ||
				![string match "250*" $response]} {
	    		set error "SMTP server error: $response"
			 }
		}

		if {$error == ""} {
			# Announce the sender and the response should be "250".
			set message "MAIL From:<$sender>"
			if {[catch {puts $smtpSocket $message} response] ||
				[catch {flush $smtpSocket} response] ||
				[catch {gets $smtpSocket} response] ||
				![string match "250*" $response]} {
	    		set error "SMTP server does not recognize '$sender': $response"
			}
		}

		if {$error == ""} {
			set recipients [concat $toList $ccList $bccList]
			# Send each recipient name in 'recipients'; for each the response should be "250".
			for {set i 0} {$i < [llength $recipients]} {incr i} {
				set recipient [lindex $recipients $i]
				set message "RCPT To: <$recipient>"
				if {[catch {puts $smtpSocket $message} response] ||
					[catch {flush $smtpSocket} response] ||
					[catch {gets $smtpSocket} response] ||
					![string match "250*" $response]} {
		    		set error "SMTP server does not like '$recipient': $response"
					break
				}
			}
		}

		if {$error == ""} {
			# Indicate the start of the "DATA" section and the response should be "354*".
			set message "DATA"
			if {[catch {puts $smtpSocket $message} response] ||
				[catch {flush $smtpSocket} response] ||
				[catch {gets $smtpSocket} response] ||
				![string match "354*" $response]} {
	    		set error "SMTP not accepting message data: $response"
			}
		}

		if {$error == ""} {
			# Put the header portion of "DATA" together.

			set header ""

			# First the "Date:" field.
			append header "Date: $mailDate\n"

			# Next the "To:" field.
			if {[llength $toList] == "1"} {
				append header "To: [lindex $toList 0]\n"
			} else {
				append header "To: $toList\n"
			}

			# Next the "From:" field.
			append header "From: [lindex $sender 0]\n"

			# Next the "Subject:" field.
			append header "Subject: $subject\n"

			# Next the "Cc:" field, if it exists.
			if {[string length $ccList]} {
				if {[llength $ccList] == "1"} {
					append header "Cc: [lindex $ccList 0]\n"
				} else {
					append header "Cc: $ccList\n"
				}
			}
	
			# Next the "X-Mailer:" field.
			append header "X-Mailer: <Tornado [wtxTsVersionGet] (Windows Version)>\n"

			# Terminate the header with "\n"
			append header "\n"

			# Send the header.
			if {[catch {puts $smtpSocket $header} response] ||
				[catch {flush $smtpSocket} response]} {
				set error "SMTP socket write error: '$response'"
			}
		}

		if {$error == ""} {
			# Put the body of the message together and send it.

			foreach line $msgContents {
				# If the line starts with a ".", make it start with *two* dots.
				if {[string index $line 0] == "."} {
					set line [format ".%s" $line]
				}

				set message $line
				if {[catch {puts $smtpSocket $message} response] ||
					[catch {flush $smtpSocket} response]} {
					set error "SMTP socket write error: '$response'"
					break
				}
			}
		}

		if {$error == ""} {
			# UUEncode each attachment and send each one individually.

			foreach attachment $attachmentList {
				# UUEncode '$attachment'
				if [catch {uuencode $attachment \
						   [file tail $attachment]} tmpFilename] {
					set error $tmpFilename
					break
				}
				# Open the UUEncoded file.
				if [catch {open $tmpFilename r} tmpFile] {
					set error "unable to open temp. file $tmpfilename"
					break;
				}
				# Place a couple of blank lines before each attachment.
				if {[catch {puts $smtpSocket "\n\n"} response] ||
					[catch {flush $smtpSocket} response]} {
					set error "SMTP socket write error: '$response'"
					break;
				}
				# Write the entire contents of the UUEncoded file to SMTP socket.
				set tmpFileContents [split [read $tmpFile] \n]
				foreach line $tmpFileContents {
					# If the line starts with a ".", make it start with *two* dots.
					if {[string index $line 0] == "."} {
						set line [format ".%s" $line]
					}
					if {[catch {puts $smtpSocket $line} response] ||
						[catch {flush $smtpSocket} response]} {
						set error "SMTP socket write error: '$response'"
						break
					}
				}
				catch {flush $smtpSocket}

				catch {close $tmpFile}

				catch {file delete $tmpFilename}
			}
		}

		if {$error == ""} {
			# Terminate the DATA section with a final "." surrounded by
			# new-lines and the response should be "250*".
			set message "\n."
			if {[catch {puts $smtpSocket $message} response] ||
				[catch {flush $smtpSocket} response] ||
				[catch {gets $smtpSocket} response] ||
				![string match "250*" $response]} {
				set error "SMTP socket write error: '$response'"
			}
		}

		if {$error == ""} {
			# "Logoff" the SMTP server and response should be "221".
			set message "QUIT"
			if {[catch {puts $smtpSocket $message} response] ||
				[catch {flush $smtpSocket} response] ||
				[catch {gets $smtpSocket} response] ||
				![string match "221*" $response]} {
	    		set error "SMTP server error: $response"
			}
		}

		# Close away the message file handle.
	 	if {$msgFile != ""} {
	   		close $msgFile
		}

		# Close away the SMTP socket.
	 	if {$smtpSocket != ""} {
			close $smtpSocket
		}

		if {$error != ""} {
			error $error
		}
	}


	#########
	# smtpMailSend - Using the Tclized socket interface provided in the
	# "GensockTcl.dll", this routine sends e-mail via the SMTP protocol.
	# Arguments include:
	#
	#    sender - "From:" name of the SMTP sender
	#    toList - "To:" a list containing SMTP recipent addresses
	#    ccList - "Cc:" a list containing SMTP "Cc:" recipent addresses
	#    bccList - "Bcc" a list containing SMTP "Bcc:" (blind) recipent addresses
	#    subject - the subject line for the message
	#    msgContents - the name of the file containing the message to be sent
	#    server - the SMTP server name
	#    attachmentList - a list of files to be attached
	#
	proc smtpMailSend {mailDate sender toList ccList bccList subject	\
					   msgContents server attachmentList} {
		set error ""
		set msgFile ""
		set smtpSocket ""

        set dllName "GensockTcl.dll"
        if {[uitclinfo debugmode] == 1} {
            set dllName "GensockTcl-d.dll"
        }

        # Open the "GensockTcl[-d].dll"
        if [catch {dllLoad [wtxPath host [wtxHostType] bin]$dllName _Gensocktcl_Init} gensockDll] {
			set error "unable to load '$dllName'"
			set gensockDll ""
		}

		if {$error == ""} {
			# Connect to the SMTP server
			if {[catch {gensockConnect $server "smtp"} smtpSocket] &&
				[catch {gensockConnect $server "25"} smtpSocket]} {
				set error "unable to connect to SMTP server '$server'"
				puts "Error: $smtpSocket"
				set smtpSocket ""
			}
		}

		if {$error == ""} {
			# The first line from the SMTP server should be "220".
			if {[catch "gensockGetLine $smtpSocket" response] ||
				![string match "220*" $response]} {
	    		set error "SMTP server error: $response"
			}
		}

		if {$error == ""} {
			# Fetch the name of this host to be provided to the SMTP server.
			set thisHostName [gensockGetHostname]

			# "Login" to the SMTP server and response should be "250".
			set message "HELO $thisHostName\r\n"
			if {[catch {gensockPutData $smtpSocket $message \
							[string length $message]} response] ||
				[catch {gensockGetLine $smtpSocket} response] ||
				![string match "250*" $response]} {
	    		set error "SMTP server error: $response"
			 }
		}

		if {$error == ""} {
			# Announce the sender and the response should be "250".
			set message "MAIL From:<$sender>\r\n"
			if {[catch {gensockPutData $smtpSocket $message \
							[string length $message]} response] ||
				[catch {gensockGetLine $smtpSocket} response] ||
				![string match "250*" $response]} {
	    		set error "SMTP server does not recognize '$sender': $response"
			}
		}

		if {$error == ""} {
			set recipients [concat $toList $ccList $bccList]
			# Send each recipient name in 'recipients'; for each
			# the response should be "250".
			for {set i 0} {$i < [llength $recipients]} {incr i} {
				if {$error == ""} {
					set recipient [lindex $recipients $i]
					set message "RCPT To: <$recipient>\r\n"
					if {[catch {gensockPutData $smtpSocket $message \
									[string length $message]} response] ||
						[catch {gensockGetLine $smtpSocket} response] ||
						![string match "250*" $response]} {
			    		set error "SMTP server does not like '$recipient': $response"
					}
				}
			}
		}

		if {$error == ""} {
			# Announce the sender and the response should be "354".
			set message "DATA\r\n"
			if {[catch {gensockPutData $smtpSocket $message \
							[string length $message]} response] ||
				[catch {gensockGetLine $smtpSocket} response] ||
				![string match "354*" $response]} {
	    		set error "SMTP not accepting message data: $response"
			}
		}

		if {$error == ""} {
			# Put the header portion of "DATA" together.

			set header ""

			# First the "Date:" field.
			append header "Date: $mailDate\n"

			# Next the "From:" field.
			append header "From: [lindex $sender 0]\n"

			# Next the "Subject:" field.
			append header "Subject: $subject\n"

			# Next the "To:" field.
			if {[llength $toList] == "1"} {
				append header "To: [lindex $toList 0]\n"
			} else {
				append header "To: $toList\n"
			}

			# Next the "Cc:" field, if it exists.
			if {[string length $ccList]} {
				if {[llength $ccList] == "1"} {
					append header "Cc: [lindex $ccList 0]\n"
				} else {
					append header "Cc: $ccList\n"
				}
			}
	
			# Next the "X-Mailer:" field.
			append header "X-Mailer: <Tornado [wtxTsVersionGet] (Windows Version)>\n"

			# Terminate the header with "\n"
			append header "\n"

			# Send the header.
			if {[catch {gensockPutDataBuffered $smtpSocket $header \
						  [string length $header]} response]} {
				set error "SMTP socket write error: '$response'"
			}
		}

		catch {gensockFlushBufferedData $smtpSocket}

		if {$error == ""} {
			# Put the body of the message together and send it.

			foreach line $msgContents {
				# If the line starts with a ".", make it start with *two* dots.
				if {[string index $line 0] == "."} {
					set line [format ".%s" $line]
				}

				# Substitute multiple line breaks ("\r") with single \r.
				regsub -all \r+ $line \r line

				set message "$line\n"
				if {[catch {gensockPutDataBuffered $smtpSocket $message \
							  [string length $message]} response]} {
					set error "SMTP socket write error: '$response'"
					break
				}
			}
		}

		catch {gensockFlushBufferedData $smtpSocket}

		if {$error == ""} {
			# UUEncode each attachment and send each one individually.

			foreach attachment $attachmentList {
				# UUEncode '$attachment'
				if [catch {uuencode $attachment \
						   [file tail $attachment]} tmpFilename] {
					set error $tmpFilename
					break
				}
				# Open the UUEncoded file.
				if [catch {open $tmpFilename r} tmpFile] {
					set error "unable to open temp. file $tmpfilename"
					break;
				}
				# Place a couple of blank lines before each attachment.
				if {[catch {gensockPutDataBuffered $smtpSocket \
							  "\r\n\r\n" 4} response]} {
					set error "SMTP socket write error: '$response'"
					break;
				}
				# Write the entire contents of the UUEncoded file to SMTP socket.
				set tmpFileContents [split [read $tmpFile] \n]
				foreach line $tmpFileContents {
					# If the line starts with a ".", make it start with *two* dots.
					if {[string index $line 0] == "."} {
						set line [format ".%s" $line]
					}
					if [catch {gensockPutDataBuffered $smtpSocket "$line\n" \
								  [expr [string length $line] + 1]} response] {
						set error "SMTP socket write error: '$response'"
						break
					}
				}
				close $tmpFile

				rm $tmpFilename

				catch {gensockFlushBufferedData $smtpSocket}
			}
		}

		if {$error == ""} {
			# Terminate the DATA section with a final "." surrounded by
			# new-lines and the response should be "250*".
			set message "\r\n.\r\n"
			if {[catch {gensockPutData $smtpSocket $message \
						   [string length $message]} response] ||
				[catch {gensockGetLine $smtpSocket} response] ||
				![string match "250*" $response]} {
				set error "SMTP socket message terminate error: '$response'"
			}
		}

		if {$error == ""} {
			# "Logoff" the SMTP server and response should be "221".
			set message "QUIT\r\n"
			if {[catch {gensockPutData $smtpSocket $message \
						   [string length $message]} response] ||
				[catch {gensockGetLine $smtpSocket} response] ||
				![string match "221*" $response]} {
	    		set error "SMTP server logoff error: $response"
			}
		}

		# Close away the message file handle.
	 	if {$msgFile != ""} {
	   		close $msgFile
		}
# Close away the SMTP socket.
	 	if {$smtpSocket != ""} {
			gensockClose $smtpSocket
		}

		# Finally, unload the 'GensockTcl.dll' library.
	 	if {$gensockDll != ""} {
			dllUnload $gensockDll
		}

		if {$error != ""} {
			error $error
		}
	}


	##############################################################################
	# INITIALIZE GLOBAL VARIABLES

	#
	# Set the "tsrDirPath" variable by searching for the ".wind/tsr"
	# directory in the following order:
	#	1. $env(HOMEDRIVE)$env(HOMEPATH)\.wind         (Windows NT)
	# 	1. $env(HOME)\.wind                   (User preferred HOME)
	#	2. $env(WIND_BASE)\.wind  (Tornado tree in Windows 95 or NT)
	#

    set dllName "SupportTcl.dll"
    if {[uitclinfo debugmode] == 1} {
        set dllName "SupportTcl-d.dll"
    }

    if [catch {dllLoad [wtxPath host [wtxHostType] bin]$dllName _Supporttcl_Init} supportDll] {
		messageBox "$supportDll $dllName"
		windowInitCallbackSet supportView ""
		windowClose supportView
		return ""
	}

	if [catch {source [wtxPath host resource tcl]Help.win32.tcl} result] {
		puts "Missing \"[wtxPath host resource tcl]Help.win32.tcl\".  Help will be disable."	
	}

	if {![catch {file exists $env(HOMEDRIVE)/$env(HOMEPATH)/.wind/tsr} result] && $result} {
		set tsrDirPath $env(HOMEDRIVE)/$env(HOMEPATH)/.wind/tsr
	} elseif {![catch {file exists $env(HOME)/.wind/tsr} result] && $result} {
		set tsrDirPath $env(HOME)/.wind/tsr
	} elseif {![catch {file exists [wtxPath].wind/tsr} result] && $result} {
		set tsrDirPath [wtxPath].wind/tsr
	} elseif {![catch {file exists $env(WIND_BASE)/.wind/tsr} result] && $result} {
		set tsrDirPath $env(WIND_BASE)/.wind/tsr
	} else {
		file mkdir "$env(WIND_BASE)/.wind/tsr"
		set tsrDirPath $env(WIND_BASE)/.wind/tsr
	}

	# Initialize the date
	# Mon, 29 Jun 1994 02:15:23 GMT
	set clockval [clock seconds]
	set mailDate [clock format $clockval -format "%a, %d %h %Y %X GMT" -gmt true]
	set curDate  [clock format [clock seconds] -format "%h %d, '%y %R"]

	# Initialize all the "profile" array elements to nothing.
	set profile(user) [ \
		if {[catch {sysRegistryValueRead HKEY_CURRENT_USER {Software\Wind River Systems} name} result]} {
			format "<Unknown>"
		} {
			format $result
		}
	]
	set profile(contact) $profile(user)
	set profile(company) [ \
		if {[catch {sysRegistryValueRead HKEY_CURRENT_USER {Software\Wind River Systems} company} result]} {
			format "<Unknown>"
		} {
			format $result
		}
	]
	set profile(email) ""
	set profile(phone) ""
	set profile(staddress) ""
	set profile(ctyaddress) ""
	set profile(reportee) "support@windriver.com"
	set profile(smtphost) "mailhost"
	set profile(license) ""

	set tsrfile(host) ""
	set tsrfile(product) ""
	set tsrfile(date) ""
	set tsrfile(reference) ""
	set tsrfile(arch) ""
	set tsrfile(bsp) ""
	set tsrfile(module) ""
	set tsrfile(hardware) ""
	set tsrfile(synopsis) ""
	set tsrfile(description) ""
	set tsrfile(attachment)	""

	set latestUserInfo(host) ""
	set latestUserInfo(product) ""
	set latestUserInfo(date) ""
	set latestUserInfo(reference) ""
	set latestUserInfo(arch) ""
	set latestUserInfo(bsp) ""
	set latestUserInfo(module) ""
	set latestUserInfo(hardware) ""

	# Load the user's "profile" from the '$tsrDirPath/../profile' file.
	if {![file exists $tsrDirPath/../profile]} {
		# The profile file does not seem to exist--create one via a dialog.
		profileDialog
	}
	if {![file exists $tsrDirPath/../profile]} {
		windowInitCallbackSet supportView { windowClose supportView }
		return	
	} {
		set proFile [open $tsrDirPath/../profile r]
		set proFileContents [read $proFile]
		close $proFile

		bindNamesToList {
			profile(user) profile(contact) profile(company) profile(email)
			profile(phone) profile(license) profile(staddress) profile(ctyaddress)
			profile(reportee) profile(smtphost)
		} $proFileContents
	}
	# Grab the "License" field from the registry even though it's saved in the profile file.
	set profile(license) [ \
		if {[catch {sysRegistryValueRead HKEY_CURRENT_USER {Software\Wind River Systems} WRSLicense} result]} {
			format "<Unknown>"
		} {
			format $result
		}
	]

	# the global boolean flag that tells whether 'supportView' window has been hacked
	set supportViewDirty 0

	set tsrFileList ""
	set architectureList ""
	set attachmentList ""
	set prev_tsrFile ""

	set tsrFileListTmp [glob -nocomplain $tsrDirPath/*]
	foreach tsr $tsrFileListTmp {
		set lst [split $tsr "/"]
		set last [expr [llength $lst] -1]
		lappend tsrFileList [lindex $lst $last]
	}

	set productReleaseList [list \
		"Tornado [wtxTsVersionGet]" "GNU Toolchain" "Diab Toolchain" "Host Utils." \
		"Target Server" "WTX Registry" \
		"VxWorks Runtime" "WindView"\
		"Documentation" "On-line Help" \
		"n/a" \
	]

	set hostVersionList {
	    "Win NT 4.0" "Win 2000" "Win XP"
		"n/a" "Other (list below)"
	}

	if {[regsub -all -nocase [wtxPath target lib]lib|gnuvx\.a \
		  [glob -nocomplain [wtxPath target lib]*.a] \
		  {} architectureList]} {
	}
	lappend architectureList "n/a"

	set bspList ""
	if {[regsub -all -nocase all|[wtxPath target config] \
		  [glob -nocomplain [wtxPath target config]*] \
		  {} bspList]} {
		set bspList [string toupper $bspList]
	}
	lappend bspList "n/a"

	windowTitleSet supportView "Tornado Support Request"
	windowInitCallbackSet supportView supportViewInit
}

menuItemInsert -bypath -statusmessage "Contact Wind River Systems customer support" \
	-after -callback {
	windowCreate -name supportView \
        -icon [wtxPath host resource bitmaps Tornado]Support.ico \
		-exec $tclScript -width 377 -height 288 \
} {&Tools &Debugger...} "Su&pport..."

