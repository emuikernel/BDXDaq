# windman/helpDlg.tcl - UITclSh GUI for quick help retrieving
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01p,12aug00,wsl  Change heading to neutral "Tornado" without a version
#                  number
# 01o,12aug00,wsl  Change version temporarily for IxWorks release
# 01n,18nov99,jgn  update version number for docs
# 01m,26may99,fle  added icons in the doc tree
# 01l,30mar99,dgp  doc: final editing changes
# 01k,30mar99,fle  doc : added it to doc as windman doc
# 01j,11mar99,fle  prepended all calls to windHelpDisplay with the file://
#                  pattern for windHelpDisplay to check if the file exists
# 01i,10mar99,fle  made it able to display files each time we are on a leaf
# 01h,03mar99,fle  added check on all calls to windHelpDisplay to generate
#                    error messages
# 01g,25nov98,fle  removed commandeParse and changed calls to windHelpLib
# 01f,05nov98,fle  changed GUI title bar
# 01e,22oct98,fle  bound the Return key with the text widget in the Index tab
#                  + set the buttons out of the tabs
#                  + add icon
#                  + resized everything
# 01d,21oct98,fle  fixed some bugs in the tree control
# 01c,15oct98,fle  removed debug text
# 01b,14oct98,fle  added commande-line parsing for manually launched help
#                  + bound list double click to display help file
# 01a,08oct98,fle  written
#

#
# DESCRIPTION
# 
# SYNOPSIS
# .tS
#   windman
# .tE
#
# The Tornado software suite includes the <Tornado Online Manuals>, a hypertext
# publication with complete reference information for Tornado and VxWorks. This
# information can be displayed from any of the `Help' menus of the Tornado tools
# or by invoking `windman' from the UNIX command line.
#
# The `windman' interface is a UITcl interface with two tabs that allow you to
# navigate around the Tornado reference information. This interface provides
# both a table of contents and a quick search index. It contains two sheets:
#
# .SS "CONTENT SHEET"
# This sheet allows you to navigate through the Wind River reference files tree
# structure. You can navigate through the books, chapters, and topics by
# expanding the tree branches. When you reach the leaves of the tree (the
# function or library names), double-clicked on a name to open a web browser
# displaying the reference entry.
#
# .SS "INDEX SHEET"
# This sheet has two fields that allow you to locate topics in 
# different ways.
#
# .iP "Entry Name Field" 10 3
# This text field allows you to type the name of the function or library you
# wish to retrieve. As you type each letter the selection moves to the closest
# name found in the entry list field. This is a case-sentitive interface.
#
# .iP "Entry List Field"
# This is a complete list of all the available routine and library references.
# It allows you to select any item from the list.
# .LP
#
# Double-click on the selected list item or click the Display button to open a
# browser window containing the reference entry.
#
# SEE ALSO
# `refgen', `syngen', `htmlBook', `htmlLink', `windHelpLib'
#
# NOROUTINES
#

#
# main entry point
#

global UICtrlDllId
global wtxtclDllId

# set dll names according to the platform

set UICtrlDllName UITclControls.dll
set wtxtclDllName wtxTcl.dll
set UICtrlDllInit _UITclControls_Init
set wtxtclDllInit _wtxtcl_Init

global tcl_platform
if { $tcl_platform(platform) == "unix" } {
    set UICtrlDllName libuitclcontrols
    set wtxtclDllName libwtxtcl
    set wtxtclDllInit Wtxtcl_Init
} elseif { $tcl_platform(platform) == "windows" } {
    if {![catch {uitclinfo debugmode} debugMode]} {
	if {$debugMode} {
	    set UICtrlDllName "$UICtrlDllName-d.dll"
	}
    }
}

# load the needed DLLs

set UICtrlDllId [dllLoad $UICtrlDllName $UICtrlDllInit]
set wtxtclDllId [dllLoad $wtxtclDllName $wtxtclDllInit]

# source the windHelpLib.tcl Tcl library to get access to its facilities

source "[wtxPath host resource doctools]windHelpLib.tcl"

################################################################################
#
# HelpDlg - UITclSh GUI for quick help retrieving main name space
#
# This name space defines all the needed dialogs and sheets for the help dialog
# tool. It contains two tabs : Index tab and Content tab. See above for further
# details
#
# This namespace contains the following procedure :
#
# createWindow	- help dialog main entry point
# initProc	- callback procedure of the main entry point
# onTabClicked	- callback procedure to invoke when a tab is clicked
# sheetRegister	- sheet registering procedure
#

namespace eval ::HelpDlg {

    # global variables

    variable dims
    variable helpEntryList
    variable helpBookList
    variable _tabs
    variable _lastSheet
    array set _self [list \
	tabsheetX 7 \
	tabsheetY 7 \
	tabsheetW 280 \
	tabsheetH 225 \
    ]

    # initialize Wind Help etries

    set ::HelpDlg::helpBookList {}
    set ::HelpDlg::helpEntryList {}
    
    # get the list of books and items from global list of help entries

    set globalHelpList [windHelpListGet]
    set ::HelpDlg::helpEntryList [lsort -dictionary [windHelpListGet name]]

    # sort book list and unset huge global help list

    set ::HelpDlg::helpBookList [windHelpTreeGet [windHelpDocDirGet]]
    unset globalHelpList

    ############################################################################
    #
    # sheetRegister - sheet registering procedure
    #
    # This procedure allows to register for a new sheet in the help GUI
    #
    # SYNOPSIS
    #   sheetRegister <namespace>
    #
    # PARAMETERS
    #   namespace : name of the namespace where the new sheet is defined
    #
    # RETURNS : N/A
    #
    # ERRORS : N/A
    #

    proc sheetRegister {namespace} {
	variable _tabs
	set _tabs([set ${namespace}::_name]) $namespace
    }

    ############################################################################
    #
    # createWindow - help dialog main entry point
    #
    # This procedure just creates the main window of the help GUI and gets the
    # right icon to use with it.
    #
    # SYNOPSIS
    #   createWindow
    #
    # PARAMETERS : N/A
    #
    # RETURNS : N/A
    #
    # ERRORS : N/A
    #

    proc createWindow {} {

	global tcl_platform		;# tcl variables

	# The UNIX icon is a little big bigger (48 * 48), and its background is
	# filled. WIN32 is a 32 * 32 max size icon, and its backgound is
	# transparent

        if { $tcl_platform(platform) == "unix" } {
	    set iconName "[wtxPath host resource bitmaps windman]help.unix.ico"
	} else {
	    set iconName "[wtxPath host resource bitmaps windman]help.ico"
	}

	# now let's create the dialog itself

	dialogCreate -name helpDlgWindow -title "Tornado Online Manuals"\
		     -w 294 -h 261 -init initProc -minibox -icon $iconName
    }

    ############################################################################
    #
    # initProc - callback procedure of the main entry point
    #
    # This procedure is the initialisation procedure of the whole help GUI. It
    # declares all the sheets and controls, then populate the help data base.
    #
    # SYNOPSIS
    #   initProc
    #
    # PARAMETERS : N/A
    #
    # RETURNS : N/A
    #
    # ERRORS: N/A
    #

    proc initProc {} {
	variable _self
	variable _lastSheet

	# create the dialog window

	controlCreate helpDlgWindow \
		      [list tab -name tabctrl \
			    -callback ::HelpDlg::onTabClicked \
			    -xpos $_self(tabsheetX) \
			    -ypos $_self(tabsheetY) \
			    -width $_self(tabsheetW) \
			    -height $_self(tabsheetH)]

	controlCreate helpDlgWindow \
		      [list button -name displaybutton -defaultbutton \
				   -title &Display -x 177 -y 240 -w 50 -h 14 \
				   -callback ::IndexTab::onDisplay]

	controlCreate helpDlgWindow \
		      [list button -name cancelbutton -title &Cancel \
				   -x 237 -y 240 -w 50 -h 14 \
				   -callback ::IndexTab::onCancel]

	set tabList {::ContentTab ::IndexTab}

	# initializes control callback and displayed items for each tab

	foreach tab $tabList {

	    controlTabsAdd helpDlgWindow.tabctrl [set ${tab}::_name]
	    foreach ctrl [set ${tab}::_ctrlList] {
		controlCreate helpDlgWindow $ctrl
	    }

	    # controls have been created for that sheet, call for the local
	    # initialization procedure

	    ${tab}::InitProc

	    # hide creation from sheets to user

	    foreach ctrlName [set ${tab}::_ctrlNames] {
		controlHide helpDlgWindow.$ctrlName 1
	    }
	}

	# always set first tab as the default one

	controlSelectionSet helpDlgWindow.tabctrl 0
	set _lastSheet [controlSelectionGet helpDlgWindow.tabctrl -string]

	# Overcome a UITcl bug: controlSelectionSet doesn't produce an event

	onTabClicked
    }

    ############################################################################
    #
    # onTabClicked - callback procedure to invoke when a tab is clicked
    #
    # This procedure is the callback used when one of the tab is clicked. It
    # updates the states of the tabs, and calls for the onHide and onShow of
    # the hiding / showing tabs
    #
    # SYNOPSIS
    #   onTabClicked
    #
    # PARAMETERS : N/A
    #
    # RETURNS : N/A
    #
    # ERRORS: N/A
    #

    proc onTabClicked {} {

	variable _tabs			;# list of tabs
	variable _lastSheet		;# last used sheet

	set currentSheet [controlSelectionGet helpDlgWindow.tabctrl -string]
	set hideSheetNs $_tabs($_lastSheet)
	set showSheetNs $_tabs($currentSheet)
	set _lastSheet $currentSheet

	# hide previous sheet. At the first invocation, previous sheet is
	# also the next sheen, then dont hide it

	if { $showSheetNs != $hideSheetNs } {
	    foreach hideCtrl [set ${hideSheetNs}::_ctrlNames] {
		controlHide helpDlgWindow.$hideCtrl 1
	    }
	    ${hideSheetNs}::onHide
	}

	${showSheetNs}::onShow
	foreach showCtrl [set ${showSheetNs}::_ctrlNames] {
	    controlHide helpDlgWindow.$showCtrl 0
	}
    }

    ############################################################################
    #
    # IndexTab - Index tab declaration and related procedures
    #
    # This namespace defines the `Index' Tab where to access quickly the help
    # topics. it contains two fields
    #
    # TEXT FIELD
    # Just type some few letters in that field to retrieve the topic that
    # matches those letters
    #
    # LIST FIELD
    # This list contains all the available topics, and a double click on any
    # of the values should popup a web browser on the associated html file.
    #
    # On a simple click, it just updates the TEXT FIELD.
    #
    # It is directly bound to the TEXT FIELD, and it focuses automatically on
    # the closest item name that matches the TEXT FIELD content.
    #
    # This namespace contains the following procedures :
    #
    # IndexTab::InitProc	- initialisation procedure of the IndexTab tab
    # IndexTab::onCancel	- callback procedure for cancel button
    # IndexTab::onChar		- callback procedure to Index tab text widget
    # IndexTab::onDisplay	- callback routine for the display call
    # IndexTab::onHide		- single routine when Index tab hides
    # IndexTab::onSelect	- initialisation procedure of the ContentTab tab
    # IndexTab::onShow		- init routine when Index tab shows up
    #

    namespace eval ::IndexTab {

	variable prevItem "None"
	variable _name "&Index"
	variable _ctrlNames {textlbl text listlbl list}

	variable _ctrlList [list \
	    [list label -name textlbl -title "&Type the first few letters of \
					      the word you're looking for." \
			-x 17 -y 35 -w 260 -h 10] \
	    [list text -name text -wantenter \
		       -x 17 -y 50 -w 260 -h 12 -callback ::IndexTab::onChar] \
	    [list label -name listlbl -title "Click the index &Entry you want, \
					      then click Display." \
			-x 17 -y 75 -w 260 -h 10] \
	    [list list -name list -x 17 -y 90 -w 260 -h 135 \
		       -callback ::IndexTab::onSelect] \
	]

	########################################################################
	#
	# IndexTab::InitProc - initialisation procedure of the IndexTab tab
	#
	# This routine contins the initialisation to proceed in the Index tab
	# at the very first initialisation. It is called only once.
	#
	# For the Index tab, it only initialises the list content towards the
	# helpEntryList variable
	#
	# SYNOPSIS
	#   ::IndexTab::InitProc
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc InitProc {} {
	    controlValuesSet helpDlgWindow.list $::HelpDlg::helpEntryList
	}

	########################################################################
	#
	# IndexTab::onHide - single routine when Index tab hides
	#
	# This is the routine where to declare actions to proceed each time the
	# Index tab hides.
	#
	# SYNOPSIS
	#   ::IndexTab::onHide
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onHide {} {
	}

	########################################################################
	#
	# IndexTab::onShow - init routine when Index tab shows up
	#
	# This procedure sets the focus on the text widget when the Index tab
	# shows up
	#
	# SYNOPSIS
	#   ::IndexTab::onShow
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onShow {} {
	    controlFocusSet helpDlgWindow.text
	    controlCallbackSet helpDlgWindow.displaybutton ::IndexTab::onDisplay
	    controlValuesSet helpDlgWindow.displaybutton "&Display"
	}

	########################################################################
	#
	# IndexTab::onChar - callback procedure to Index tab text widget
	#
	# Each time a character is typed in the text widget from the Index tab,
	# a lookup is done in the help data base to retrieve in the Idex tab
	# list the closest name matching what is in the text widget.
	#
	# SYNOPSIS
	#   ::IndexTab::onChar
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onChar {} {

	    set text [controlTextGet helpDlgWindow.text]
	    if {$text == ""} {
		controlSelectionSet helpDlgWindow.list -noevent 0
	    } else {

		# replace all the regexp reserved charaters with their escape
		# sequence

		regsub -all "\\\[" $text "\\\\\[" text
		regsub -all "\\\]" $text "\\\\\]" text
		regsub -all "\\\." $text "\\\\\." text

		set entryNum [windHelpEntryLookup -regexp $text \
						  $::HelpDlg::helpEntryList]
		if { $entryNum != -1 } {
		    controlSelectionSet helpDlgWindow.list -noevent $entryNum
		}
	    }
	}

	########################################################################
	#
	# IndexTab::onDisplay - callback routine for the display call
	#
	# When an item is to be displayed, first get the related link to that
	# item through a call to `windHelpLinkGet', then if everything is fine,
	# display it by calling the `windHelpDisplay' procedure
	#
	# SYNOPSIS
	#   ::IndexTab::onDisplay
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onDisplay {} {

	    set helpName [controlSelectionGet helpDlgWindow.list -string]
	    set helpLink [windHelpLinkGet -name $helpName]

	    if { $helpLink == {} } {

		# windHelpLinkGet was unable to get the related link to
		# <helpTopic>. Popup a message box to inform user.

		set errMsg "Can't find any help link related to $helpTopic"

		messageBox -ok -exclamationicon $errMsg
		return
	    }

	    set status [windHelpDisplay "file://$helpLink"]
	    if { [lindex $status 0] != "OK" } {

		# windHelpDisplay failed, warn the user

		set errMsg "Error while trying to display html help $helpLink"
		set errList [lrange $status 1 end]
		foreach errElt $errList {
		    append errMsg "\n$errElt"
		}
		messageBox -ok -exclamationicon $errMsg
	    }
	}

	########################################################################
	#
	# IndexTab::onCancel - callback procedure for cancel button
	#
	# When the `Cancel' button is pressed, then the we exit from the Tornado
	# reference help. This procedure also unloads the loaded dlls that were
	# loaded at startup.
	#
	# SYNOPSIS
	#   ::IndexTab::onCancel
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onCancel {} {
	    global wtxtclDllId		;# wtxtcl dll load ID
	    global UICtrlDllId		;# UITcl controls dll load ID

	    dllUnload $wtxtclDllId
	    dllUnload $UICtrlDllId
	    windowClose helpDlgWindow
	}

	########################################################################
	#
	# IndexTab::onSelect - initialisation procedure of the ContentTab tab
	#
	# When an item name is selected in the list, then we check if it was
	# also the previously selected item. If so, it is a double click on this
	# item : display it.
	#
	# Also, when an item is selected in the list, the text widget is updated
	# according to it.
	#
	# SYNOPSIS
	#   ::IndexTab::onSelect
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onSelect {} {

	    variable prevItem

	    set selectedItem [controlSelectionGet helpDlgWindow.list -string]
	    controlValuesSet helpDlgWindow.text $selectedItem

	    if { $selectedItem == $prevItem } {

		# this is a double-click on a tree element, let's display the
		# html file related to the selected item

		set helpLink [windHelpLinkGet -name $selectedItem]
		set status [windHelpDisplay "file://$helpLink"]

		# check that the windHelpDisplay was all right

		if { [lindex $status 0] != "OK" } {
		    set errMsg "Error while trying to display html help \
				$helpLink"
		    set errList [lrange $status 1 end]
		    foreach errElt $errList {
			append errMsg "\n$errElt"
		    }
		    messageBox -ok -exclamationicon $errMsg
		}
		set prevItem "None"
	    } else {
		set prevItem $selectedItem
	    }
	}

	# Register the sheet

	::HelpDlg::sheetRegister ::IndexTab
    }

    ############################################################################
    #
    # ContentTab - Content tab declaration and related procedures
    #
    # This namespace defines the `Content' Tab where to access book / chapter /
    # topics inside a tree control. It allows to navigate through the WRS
    # reference Manuals like in books.
    #
    # This namespace contains the following procedures :
    #
    # ContentTab::InitProc	- initialisation procedure of the ContentTab tab
    # ContentTab::onBookClose	- callback procedure to close a tree branch
    # ContentTab::onBookOpen	- callback procedure to open a tree branch
    # ContentTab::onCancel	- callback procedure for cancel button
    # ContentTab::onDisplay	- callback procedure to display help topic
    # ContentTab::onHide	- ContentTab tab hiding callback procedure
    # ContentTab::onSelect	- callback procedure for tree control select
    # ContentTab::onShow	- ContentTab tab showing callback procedure
    #

    namespace eval ::ContentTab {

	variable prevItem "None"		;# previously selected item
	variable _name "&Content"
	variable _ctrlNames {contentLbl content}
	variable _ctrlList [list \
	    [list label -name contentLbl -title "Click a &Book and then Open." \
			-x 17 -y 35 -w 260 -h 10] \
	    [list tree -name content -x 17 -y 50 -w 260 -h 170 \
		       -callback ::ContentTab::onSelect] \
	]

	########################################################################
	#
	# ContentTab::InitProc - initialisation procedure of the ContentTab tab
	#
	# This is the procedure where to put all the initialisation phases of
	# the ContentTab tab. This procedure is called at start time, and is
	# never called again.
	#
	# SYNOPSIS
	#   ::ContentTab::InitProc
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc InitProc {} {
	    controlValuesSet helpDlgWindow.content $::HelpDlg::helpBookList

	    # get the list of icons for the tree

	    if { [catch {controlPropertySet helpDlgWindow.content \
					    -imagelist "Icons.dll \
							IDB_HELP_SHEET \
							IDB_HELP_OPEN_BOOK \
							IDB_HELP_BOOK" } \
			iconStatus] } {

		messageBox -ok -exclamationicon \
			   "Image list for Help dialog didn't load :\
			   $iconStatus"
	    }

	    # the HELP_SHEET icon has been assigned to all the tree element.
	    # Only the third level element should have that Icon, but as they
	    # are the more numerous, we assign SHEET for all items, then BOOK
	    # for all items of first level and second level

	    foreach book $::HelpDlg::helpBookList {

		# assign the BOOK icon to the book name

		set bookName [lindex $book 0]
		controlItemPropertySet helpDlgWindow.content -imageno 2 \
				       [list $bookName]

		set chapterTree [lrange $book 1 end]
		foreach chapter $chapterTree {

		    # assign the BOOK icon to the chapter

		    set chaperName [lindex $chapter 0]
		    controlItemPropertySet helpDlgWindow.content -imageno 2 \
					   [list $bookName $chaperName]
		}
	    }
	}

	########################################################################
	#
	# ContentTab::onHide - ContentTab tab hiding callback procedure
	#
	# This is the procedure where to put all actions to execute when the
	# Content tab is hiding behind another tab
	#
	# SYNOPSIS
	#   ::ContentTab::onHide
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onHide {} {
	}

	########################################################################
	#
	# ContentTab::onShow - ContentTab tab showing callback procedure
	#
	# This is the procedure where to put all actions to execute when the
	# Content tab is showing above another tab
	#
	# SYNOPSIS
	#   ::ContentTab::onShow
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onShow {} {

	    controlFocusSet helpDlgWindow.content

	    set selectedItem [controlSelectionGet helpDlgWindow.content]

	    if { $selectedItem == "" } {
		return
	    }

	    set elementChildren [controlValuesGet helpDlgWindow.content \
						  -childrenonly $selectedItem]

	    # if the element children is an empty string, we are on a leaf,
	    # let's do the necessary : rename the Open / Close button in Display

	    if { ($elementChildren == "") } {

		# change the callback if needed

		controlCallbackSet helpDlgWindow.displaybutton {

		    set topic [controlSelectionGet helpDlgWindow.content]
		    ::ContentTab::onDisplay  [lindex $topic end]
		}

		controlValuesSet helpDlgWindow.displaybutton "&Display"

	    } else {

		# this is not a leaf, but a branch. Get the branch state
		# (expanded or not) and set the Open / Close button state
		# in accordance with it

		set expandState [controlItemPropertyGet helpDlgWindow.content \
							-expand $selectedItem]

		if { $expandState == 0 } {

		    # the current node is not expanded. The action towards this
		    # node should be "Open". Update the button, and tree icon
		    # if needed

		    if { [controlValuesGet helpDlgWindow.displaybutton]
			 != "&Open" } {

			controlItemPropertySet helpDlgWindow.content -imageno 2\
					       $selectedItem
			controlCallbackSet helpDlgWindow.displaybutton \
					   ::ContentTab::onBookOpen
			controlValuesSet helpDlgWindow.displaybutton "&Open"
		    }

		} else {

		    # the current node is expanded. Default action should be
		    # "Close". Update the button text and tree icon if needed

		    if { [controlValuesGet helpDlgWindow.displaybutton] != 
			 "Cl&ose" } {

			controlItemPropertySet helpDlgWindow.content -imageno 1\
					       $selectedItem
			controlCallbackSet helpDlgWindow.displaybutton \
					   ::ContentTab::onBookClose
			controlValuesSet helpDlgWindow.displaybutton "Cl&ose"
		    }
		}
	    }

	}

	########################################################################
	#
	# ContentTab::onBookOpen - callback procedure to open a tree branch
	#
	# When the `Open' button is pressed, then the selected is expanded, and
	# the button is renamed `Close'. The routine associated to that `Close'
	# button is then onBookClose
	#
	# SYNOPSIS
	#   ::ContentTab::onBookOpen
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onBookOpen {} {

	    set selectedName [controlSelectionGet helpDlgWindow.content]
	    controlItemPropertySet helpDlgWindow.content -expand 1 -imageno 1 \
				   $selectedName
	    controlCallbackSet helpDlgWindow.displaybutton \
			       ::ContentTab::onBookClose
	    controlValuesSet helpDlgWindow.displaybutton "Cl&ose"
	}

	########################################################################
	#
	# ContentTab::onBookClose - callback procedure to close a tree branch
	#
	# When the `Close' button is pressed, then the selected is unexpanded,
	# and the button is renamed `Open'. The routine associated to that
	# `Open' button is then onBookOpen
	#
	# SYNOPSIS
	#   ::ContentTab::onBookClose
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onBookClose {} {
	    set selectedName [controlSelectionGet helpDlgWindow.content]
	    controlItemPropertySet helpDlgWindow.content -expand 0 -imageno 2 \
				   $selectedName
	    controlCallbackSet helpDlgWindow.displaybutton \
		::ContentTab::onBookOpen
	    controlValuesSet helpDlgWindow.displaybutton "&Open"
	}

	########################################################################
	#
	# ContentTab::onDisplay - callback procedure to display help topic
	#
	# When the `Display' button is pressed, the <helpTopic> parameter is
	# used to call for the `windHelpDisplay' procedure that pops up a web
	# browser on the <helpTopic> link retrieved by `windHelpLinkGet'
	#
	# SYNOPSIS
	#   ::ContentTab::onDisplay <helpTopic>
	#
	# PARAMETERS
	#   helpTopic : the topic to display in the web browser
	#
	# RETURNS: N/A
	#
	# ERRORS:
	#   No link associated with <helpTopic>
	#   Unable to open link in Web browser
	#

	proc onDisplay { helpTopic } {

	    set helpLink [windHelpLinkGet -name $helpTopic]

	    if { $helpLink == {} } {

		# windHelpLinkGet was unable to get the related link to
		# <helpTopic>. Popup a message box to inform user.

		set errMsg "Can't find any help link related to $helpTopic"

		messageBox -ok -exclamationicon $errMsg
		return
	    }

	    set status [windHelpDisplay "file://$helpLink"]

	    if { [lindex $status 0] != "OK" } {

		# execution of the windHelpDisplay routine failed. Let's prompt
		# the user about it

		set errMsg "Error while trying to display html help $helpLink"
		set errList [lrange $status 1 end]
		foreach errElt $errList {
		    append errMsg "\n$errElt"
		}
		messageBox -ok -exclamationicon $errMsg
	    }
	}

	########################################################################
	#
	# ContentTab::onCancel - callback procedure for cancel button
	#
	# When the `Cancel' button is pressed, then the we exit from the Tornado
	# reference help. This procedure also unloads the loaded dlls that were
	# loaded at startup.
	#
	# SYNOPSIS
	#   ::ContentTab::onCancel
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onCancel {} {

	    global wtxtclDllId		;# wtxtcl dll load ID
	    global UICtrlDllId		;# UITcl controls dll load ID

	    dllUnload $wtxtclDllId
	    dllUnload $UICtrlDllId
	    windowClose helpDlgWindow
	}

	########################################################################
	#
	# ContentTab::onSelect - callback procedure for tree control select
	#
	# This routine is called when any of the tree items is clicked. It can
	# be any of the tree elements : the branches, the leaves, or the expand
	# `+' elements
	#
	# SYNOPSIS
	#   ::ContentTab::onSelect
	#
	# PARAMETERS: N/A
	#
	# RETURNS: N/A
	#
	# ERRORS: N/A
	#

	proc onSelect {} {

	    variable prevItem		;# previously selected item

	    # if an item has been expanded, then select it

	    if { [controlEventGet helpDlgWindow.content] == "expanding" } {

		set expandedItem [controlExpandingItemGet helpDlgWindow.content]
		if { $expandedItem != "" } {
		    controlSelectionSet helpDlgWindow.content -noevent \
					$expandedItem
		}
	    }

	    set selectedItem [controlSelectionGet helpDlgWindow.content]

	    set elementChildren [controlValuesGet helpDlgWindow.content \
						  -childrenonly $selectedItem]

	    # if the element children is an empty string, we are on a leaf,
	    # let's do the necessary : rename the Open / Close button in Display

	    if { ($elementChildren == "") } {

		# change the callback if needed

		if { [controlValuesGet helpDlgWindow.displaybutton] != \
		     "&Display" } {

		    controlCallbackSet helpDlgWindow.displaybutton {

			set topic [controlSelectionGet helpDlgWindow.content]
			::ContentTab::onDisplay  [lindex $topic end]
		    }

		    controlValuesSet helpDlgWindow.displaybutton "&Display"
		}

		# if this is a double click on the item, the <prevItem> value
		# should be equal to the current selected item value. If so,
		# display the selected topic

		if { $prevItem == $selectedItem } {
		    ::ContentTab::onDisplay [lindex $selectedItem end]
		    set prevItem "None"
		} else {
		    set prevItem $selectedItem
		}

	    } else {

		# this is not a leaf, but a branch. Get the branch state
		# (expanded or not) and set the Open / Close button state
		# in accordance with it

		set expandState [controlItemPropertyGet helpDlgWindow.content \
							-expand $selectedItem]

		if { $expandState == 0 } {

		    # the current node is not expanded. The action towards this
		    # node should be "Open". Update the button, and tree icon
		    # if needed

		    if { [controlValuesGet helpDlgWindow.displaybutton]
			 != "&Open" } {

			controlItemPropertySet helpDlgWindow.content -imageno 2\
					       $selectedItem
			controlCallbackSet helpDlgWindow.displaybutton \
					   ::ContentTab::onBookOpen
			controlValuesSet helpDlgWindow.displaybutton "&Open"
		    }

		} else {

		    # the current node is expanded. Default action should be
		    # "Close". Update the button text and tree icon if needed

		    if { [controlValuesGet helpDlgWindow.displaybutton] != 
			 "Cl&ose" } {

			controlItemPropertySet helpDlgWindow.content -imageno 1\
					       $selectedItem
			controlCallbackSet helpDlgWindow.displaybutton \
					   ::ContentTab::onBookClose
			controlValuesSet helpDlgWindow.displaybutton "Cl&ose"
		    }
		}

		set prevItem $selectedItem
	    }
	}

	# Register the sheet

	::HelpDlg::sheetRegister ::ContentTab
    }

    # Entry point 

    createWindow
}
