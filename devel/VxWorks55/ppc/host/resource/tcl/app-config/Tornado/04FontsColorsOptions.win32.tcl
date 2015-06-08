##############################################################################
# 04FontsColorsOptions.win32.tcl - Tornado for Windows options dialog implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01h,16may99,j_k  Part of fix for SPR #27439.
# 01g,10feb99,rbl  Adding color for dead projects (grey by default)
# 01f,25nov98,j_k  Part of fix for SPR #23488.
# 01e,02nov98,cjs  Changed name of project facility items 
# 01d,30oct98,rbl  Fixed restoring of default colors - this was restoring your last saved 
#		   change rather than the true defaults.  
# 01c,19oct98,rbl  Clear out list of colors in restore {}. Fixes bug where
#			    your last color changes were not reflected in the options dialog
# 01b,08sep98,cjs  added colors for config tool 
# 01a,01jun98,j_k  written
#

#
# Do not source this file if the platform is not win32
#
if {![string match *win32* [wtxHostType]]} return

set colors [list Black White Red Green Blue Yellow Magenta BlueGreen \
		DarkGray LightGray Maroon DarkGreen DarkBlue Bile Purple DarkBlueGreen]

set lColorRegEntries {}
set lDefColorRegEntries {}
set lOrigColorRegEntries {}

lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	SourceText		(default)
lappend lDefColorRegEntries [list 255 255 255 128 0 0]		;#	BreakpointLine
lappend lDefColorRegEntries [list 0 128 0 255 255 255]		;#	Comment
lappend lDefColorRegEntries [list 128 128 128 255 255 255]	;#	DisassemblyLine
lappend lDefColorRegEntries [list 0 0 255 255 255 255]		;#	CKeyword
lappend lDefColorRegEntries [list 0 0 255 255 255 255]		;#	CPPKeyword
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	Number
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	FloatingPoint
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	String
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	Identifier
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	InspectWindow
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	BuildOutput
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	RegistersWindow
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	LocalsWindow
lappend lDefColorRegEntries [list 255 255 255 0 0 0 ]		;#	TextSelection
lappend lDefColorRegEntries [list 0 0 0 255 255 0]			;#	CurrentLine
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	MemoryWindow
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	BacktraceWindow
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	Debugger
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#	Shell
lappend lDefColorRegEntries [list 0 0 0 0 255 0]			;#	PreviousContext
lappend lDefColorRegEntries [list 255 0 0 255 255 255]		;#	RegistersHighlight
lappend lDefColorRegEntries [list 255 0 0 255 255 255]		;#	LocalsHighlight
lappend lDefColorRegEntries [list 255 0 0 255 255 255]		;#	InspectHighlight
lappend lDefColorRegEntries [list 255 0 0 255 255 255]		;#	BacktraceHighlight
lappend lDefColorRegEntries [list 255 255 255 0 0 128]		;#	CurrentTag
lappend lDefColorRegEntries [list 255 0 0 255 255 255]		;#  ComponentError
lappend lDefColorRegEntries [list 0 128 0 255 255 255]		;#  ComponentHighlight 
lappend lDefColorRegEntries [list 0 0 0 255 255 255]		;#  ComponentNormal
lappend lDefColorRegEntries [list 128 128 128 255 255 255]	;#  DeadProject

set lColorRegItems [list SourceText BreakpointLine Comment \
	DisassemblyLine CKeyword CPPKeyword Number FloatingPoint  \
	String Identifier InspectWindow BuildOutput RegistersWindow \
	LocalsWindow TextSelection CurrentLine MemoryWindow \
	BacktraceWindow Debugger Shell PreviousContext \
	RegistersHighlight LocalsHighlight InspectHighlight \
	BacktraceHighlight CurrentTag ConfigError ConfigHighlight ConfigNormal DeadProject]

set lSyntaxColoring {}
set lOrigSyntaxColoring {}

set lFontRegEntries {}
set lOrigFontRegEntries {}
set lFontRegItems [list FontInfo]
global saveFontFamilies
global saveFontStyles
global saveFontStyleValues

namespace eval FontsColorsPage {

	variable lColorItems [list "Source Text (Default)" \
		"Breakpoint Line" "Comment" "Disassembly Line" "C Keyword" \
		"C++ Keyword" "Number" "Floating Point" "String" "Identifier" \
		"Inspect Window" "Build Output" "Registers Window" \
		"Locals Window" "Text Selection" "Current Line" "Memory Window" \
		"Backtrace Window" "Debugger" "Shell" "Previous Context" \
		"Registers Highlight" "Locals Highlight" "Inspect Highlight" \
		"Backtrace Highlight" "Current Error/Tag" \
		"Component Error" "Component Highlight" "Component Normal" "Missing Project File"]

	variable gFontsColorsPageCtrls [list \
		fontGrp fontNames fontStyles fontSizes fontTypes sampleGrp fontSample \
		clrFrame bSyntaxHiliting itemsList restoreSelDefault restoreAllDefault \
		fgClrLbl fgClrBox bgClrLbl bgClrBox]

	variable gColorsGrpCtrls [list \
		itemsList restoreSelDefault restoreAllDefault \
		fgClrLbl fgClrBox bgClrLbl bgClrBox]

	proc init {} {
		global lColorRegItems
		global lColorRegEntries
		global lFontRegEntries
		global lSyntaxColoring
                global saveFontFamilies
                global saveFontStyles
                global saveFontStyleValues

		variable gFontsColorsPageCtrls
		variable lColorItems

                set sampleText [localeStringGet "AaBbCcDdEe"]

		set Controls [list \
			[list group -hidden -name fontGrp -title "Font" \
				-xpos 12 -ypos 20 -width 165 -height 50] \
			[list combo -hidden -sort -name fontNames \
				-callback "FontsColorsPage::OnNewFontSelected" \
				-xpos 17 -ypos 32 -width 80 -height 50] \
			[list combo -hidden -name fontStyles \
				-callback "FontsColorsPage::onNewFontStyleSelected" \
				-xpos 100 -ypos 32 -width 74 -height 50] \
			[list combo -hidden -name fontSizes \
				-callback "FontsColorsPage::onNewFontItemSel fontSizes" \
				-xpos 17 -ypos 50 -width 40 -height 50] \
			[list combo -hidden -name fontTypes \
				-callback "FontsColorsPage::onNewFontItemSel fontTypes" \
				-xpos 64 -ypos 50 -width 60 -height 50] \
			[list group -hidden -name sampleGrp -title Sample\
				-xpos 180 -ypos 20 -width 116 -height 46] \
			[list label -hidden -border -vcenter -center -name fontSample \
				-title $sampleText \
				-xpos 186 -ypos 30 -width 104 -height 30] \
			[list group -hidden -name clrFrame \
				-xpos 12 -ypos 79 -width 286 -height 108] \
			[list boolean -hidden -auto -name bSyntaxHiliting \
				-callback FontsColorsPage::onSyntaxHighlight -title "&Syntax Coloring " \
				-xpos 22 -ypos 79 -width 64 -height 9] \
			[list list -hidden -nointegralheight \
				-name itemsList -callback FontsColorsPage::onNewColorItemSel \
				-xpos 20 -ypos 92 -width 90 -height 85] \
			[list label -hidden -title "&Foreground Color:" -name fgClrLbl \
				-xpos 120 -ypos 100 -width 70 -height 9] \
			[list colorpicker -hidden -nocustom -minimumset \
				 -name fgClrBox -callback "FontsColorsPage::onItemColorChange fgClrBox"\
				-xpos 184 -ypos 98 -width 70 -height 14] \
			[list label -hidden -title "&Background Color:" -name bgClrLbl \
				-xpos 120 -ypos 118 -width 70 -height 9] \
			[list colorpicker -hidden -nocustom -minimumset \
				 -name bgClrBox -callback "FontsColorsPage::onItemColorChange bgClrBox" \
				-xpos 184 -ypos 116 -width 70 -height 14] \
			[list button -hidden -title "Restore to &Default" -name restoreSelDefault \
				-callback "FontsColorsPage::onRestoreSelItem" \
				-xpos 120 -ypos 135 -width 80 -height 14] \
			[list button -hidden -title "Restore All to &Default" -name restoreAllDefault \
				-callback "FontsColorsPage::onRestoreAllItems" \
				-xpos 120 -ypos 155 -width 80 -height 14] \
		]

		foreach ctrl $Controls {
			controlCreate tornadoOptionsDlg $ctrl
		}

		set saveEntries $lFontRegEntries
                set fontFamilies [enumerateFontFamilies -fixedfontsonly]

                foreach family $fontFamilies {
                    if {[info exists saveFontFamilies([lindex $family 0])]} {
                        if {[lsearch $saveFontFamilies([lindex $family 0]) \
                            [lindex $family 1]] == -1} {
                            lappend saveFontFamilies([lindex $family 0]) \
                                [lindex $family 1]
                        }
                    } else {
                        lappend saveFontFamilies([lindex $family 0]) \
                            [lindex $family 1]
                    }
                    if {[info exists saveFontStyles([lindex $family 1])]} {
                        if {[lsearch $saveFontStyles([lindex $family 1]) \
                            [lindex $family 0]] == -1} {
                            lappend saveFontStyles([lindex $family 1]) \
                                [lindex $family 0]
                        }
                    } else {
                        lappend saveFontStyles([lindex $family 1]) \
                            [lindex $family 0]
                    }
                    if {[info exists saveFontStyleValues([lindex $family 1])]} {
                        if {[lsearch $saveFontStyleValues([lindex $family 1]) \
                            [lindex $family 2]] == -1} {
                            lappend saveFontStyleValues([lindex $family 1]) \
                                [lindex $family 2]
                        }
                    } else {
                        lappend saveFontStyleValues([lindex $family 1]) \
                            [lindex $family 2]
                    }
                }

                set fontFamilyNames [array names saveFontFamilies]

		controlValuesSet tornadoOptionsDlg.fontNames $fontFamilyNames
		controlSelectionSet tornadoOptionsDlg.fontNames 0

		controlValuesSet tornadoOptionsDlg.fontTypes [list Regular Italic Bold "Bold Italic"]
		controlSelectionSet tornadoOptionsDlg.fontTypes 0

		controlSelectionSet tornadoOptionsDlg.fontNames -string [lindex $lFontRegEntries 0]
		controlSelectionSet tornadoOptionsDlg.fontSizes -string [lindex $lFontRegEntries 1]

		set bBold [lindex $lFontRegEntries 2]
		set bItalic [lindex $lFontRegEntries 3]

		set type "Regular"
		if {$bBold == 1 && $bItalic == 1} {
			set type "Bold Italic"
		} elseif {$bBold == 1 && $bItalic == 0} {
			set type "Bold"
		} elseif {$bBold == 0 && $bItalic == 1} {
			set type "Italic"
		}
		controlSelectionSet tornadoOptionsDlg.fontTypes -string $type

                set restoredCharset [lindex $lFontRegEntries 4]
                foreach name [array names saveFontStyleValues] {
                    if {$saveFontStyleValues($name) == $restoredCharset} {
                        controlSelectionSet tornadoOptionsDlg.fontStyles -string $name
                        break
                    }
                }
		set lFontRegEntries $saveEntries

		controlCheckSet tornadoOptionsDlg.bSyntaxHiliting $lSyntaxColoring
		initColorsSection

		redrawSampleText
	}

	proc onSyntaxHighlight {} {
		variable gColorsGrpCtrls

		set check [controlChecked tornadoOptionsDlg.bSyntaxHiliting]
                
                # since project and debugger releated color items are in the list
                # of items that can be configured, disabling the following
                # control does not make any sense.  syntax hilighting should
                # independent of otehr colors.
                return

		foreach ctrl $gColorsGrpCtrls {
			controlEnable tornadoOptionsDlg.$ctrl $check
		}
	}

	proc redrawSampleText {} {
                global saveFontStyleValues
		global lColorRegEntries

		set selFont [controlSelectionGet tornadoOptionsDlg.fontNames -string]
		set selStyle [controlSelectionGet tornadoOptionsDlg.fontStyles -string]
		set selSize [controlSelectionGet tornadoOptionsDlg.fontSizes -string]
		set style [controlSelectionGet tornadoOptionsDlg.fontTypes -string]

		set fontInfo [list $selFont $selSize]
		if {$style == "Regular"} {
                    lappend fontInfo 0
                    lappend fontInfo 0
		} elseif {$style == "Italic"} {
                    lappend fontInfo 0
                    lappend fontInfo 1
		} elseif {$style == "Bold"} {
                    lappend fontInfo 1
                    lappend fontInfo 0
		} elseif {$style == "Bold Italic"} {
                    lappend fontInfo 1
                    lappend fontInfo 1
		}
                if [info exists saveFontStyleValues($selStyle)] {
                    lappend $fontInfo $saveFontStyleValues($selStyle)
                } else {
                    lappend $fontInfo 0
                }

		set index [controlSelectionGet tornadoOptionsDlg.itemsList]
		if {$index != ""} {
			set colors [lindex $lColorRegEntries $index]
			set fgClr [lrange $colors 0 2]
			set bgClr [lrange $colors 3 5]

			controlPropertySet tornadoOptionsDlg.fontSample \
				-background [colorFromRGBGet $bgClr] -foreground [colorFromRGBGet $fgClr]
		}

		controlPropertySet tornadoOptionsDlg.fontSample -font $fontInfo
	}

	proc OnNewFontSelected {} {
            global saveFontFamilies
            global saveFontStyles

            set event [controlEventGet tornadoOptionsDlg.fontNames]
            if {$event == "selchange"} {
                set fontStyleNames \
                    $saveFontFamilies([controlSelectionGet tornadoOptionsDlg.fontNames -string])
                controlValuesSet tornadoOptionsDlg.fontStyles $fontStyleNames
                controlSelectionSet tornadoOptionsDlg.fontStyles 0
            }
	}

        proc onNewFontStyleSelected {} {
            global saveFontFamilies
            global saveFontStyles

            set event [controlEventGet tornadoOptionsDlg.fontStyles]
            if {$event == "selchange"} {
                set prevSize [controlSelectionGet tornadoOptionsDlg.fontSizes -string]
                set prevType [controlSelectionGet tornadoOptionsDlg.fontTypes -string]

                set selFont [controlSelectionGet tornadoOptionsDlg.fontNames -string]
                set availableSizes [enumerateFontSizes $selFont]
                controlValuesSet tornadoOptionsDlg.fontSizes $availableSizes

                if {[lsearch $availableSizes $prevSize] != -1} {
                    controlSelectionSet tornadoOptionsDlg.fontSizes -string $prevSize
                } else {
                    controlSelectionSet tornadoOptionsDlg.fontSizes 0
                }
                controlSelectionSet tornadoOptionsDlg.fontTypes -string $prevType
            }
        }

	proc onNewFontItemSel {ctrl} {
		set event [controlEventGet tornadoOptionsDlg.$ctrl]
		if {$event == "selchange"} {
                    redrawSampleText
		}
	}

	proc initColorsSection {} {
		global lColorRegEntries
		variable lColorItems

		controlValuesSet tornadoOptionsDlg.itemsList $lColorItems 

		set index 0
		foreach item $lColorRegEntries {
			set fgClr [lrange $item 0 2]
			set bgClr [lrange $item 3 end]
			controlItemPropertySet tornadoOptionsDlg.itemsList -index $index \
				-background [colorFromRGBGet $bgClr] -foreground [colorFromRGBGet $fgClr]
			incr index
		}
		if {[controlChecked tornadoOptionsDlg.bSyntaxHiliting] == "1"} {
			controlSelectionSet tornadoOptionsDlg.itemsList 0
		}
	}

	proc onItemColorChange {ctrl} {
		global lColorRegEntries

		set event [controlEventGet tornadoOptionsDlg.$ctrl]
		if {$event != "selchange"} {
			return
		}

		set colors {}
		set index [controlSelectionGet tornadoOptionsDlg.itemsList]

		set fgClr [controlSelectionGet tornadoOptionsDlg.fgClrBox]
		set bgClr [controlSelectionGet tornadoOptionsDlg.bgClrBox]

		controlItemPropertySet tornadoOptionsDlg.itemsList -index $index \
			-background [colorFromRGBGet $bgClr] -foreground [colorFromRGBGet $fgClr]

		lappend colors [lindex $fgClr 0]
		lappend colors [lindex $fgClr 1]
		lappend colors [lindex $fgClr 2]

		lappend colors [lindex $bgClr 0]
		lappend colors [lindex $bgClr 1]
		lappend colors [lindex $bgClr 2]

		set lColorRegEntries [lreplace $lColorRegEntries $index $index $colors]
		
		onNewColorItemSel
	}

	# Restore selected color item to default
	proc onRestoreSelItem {} {
		global lColorRegEntries
		global lDefColorRegEntries

		set index [controlSelectionGet tornadoOptionsDlg.itemsList]
		set lColorRegEntries [lreplace $lColorRegEntries $index $index [lindex $lDefColorRegEntries $index]]

		set colors [lindex $lColorRegEntries $index]
		set fgClr [lrange $colors 0 2]
		set bgClr [lrange $colors 3 5]

		controlItemPropertySet tornadoOptionsDlg.itemsList -index $index \
			-background [colorFromRGBGet $bgClr] -foreground [colorFromRGBGet $fgClr]

		controlSelectionSet tornadoOptionsDlg.fgClrBox $fgClr
		controlSelectionSet tornadoOptionsDlg.bgClrBox $bgClr
	}

	proc onRestoreAllItems {} {
		global lColorRegEntries
		global lDefColorRegEntries

		set lColorRegEntries $lDefColorRegEntries
		initColorsSection
	}

	proc onNewColorItemSel {} {
		global lColorRegItems
		global lColorRegEntries

		set index [controlSelectionGet tornadoOptionsDlg.itemsList]

		set colors [lindex $lColorRegEntries $index]
		set fgClr [lrange $colors 0 2]
		set bgClr [lrange $colors 3 5]

		controlSelectionSet tornadoOptionsDlg.fgClrBox $fgClr
		controlSelectionSet tornadoOptionsDlg.bgClrBox $bgClr

		redrawSampleText
	}

	proc enter {} {
		variable gFontsColorsPageCtrls

		foreach ctrl $gFontsColorsPageCtrls {
			controlShow tornadoOptionsDlg.$ctrl 1
		}
	}

	proc exit {} {
		global lColorRegItems
		global lColorRegEntries
		global lSyntaxColoring
		global lFontRegEntries
                global saveFontStyleValues

		variable gFontsColorsPageCtrls

		set lSyntaxColoring [controlChecked tornadoOptionsDlg.bSyntaxHiliting]

		set lFontRegEntries {}
		lappend lFontRegEntries [controlSelectionGet tornadoOptionsDlg.fontNames -string]
		lappend lFontRegEntries [controlSelectionGet tornadoOptionsDlg.fontSizes -string]
		set type [controlSelectionGet tornadoOptionsDlg.fontTypes -string]

		if {$type == "Regular"} {
			lappend lFontRegEntries 0
			lappend lFontRegEntries 0
		} elseif {$type == "Italic"} {
			lappend lFontRegEntries 0
			lappend lFontRegEntries 1
		} elseif {$type == "Bold"} {
			lappend lFontRegEntries 1
			lappend lFontRegEntries 0
		} elseif {$type == "Bold Italic"} {
			lappend lFontRegEntries 1
			lappend lFontRegEntries 1
		}
                set selFontStyle [controlSelectionGet tornadoOptionsDlg.fontStyles -string]
                lappend lFontRegEntries $saveFontStyleValues($selFontStyle)

		foreach ctrl $gFontsColorsPageCtrls {
                    controlShow tornadoOptionsDlg.$ctrl 0
		}
		return 1
	}

	proc restore {} {
		global lOrigColorRegEntries
		global lDefColorRegEntries
		global lColorRegEntries
		global lColorRegItems

		global lSyntaxColoring
		global lOrigSyntaxColoring

		global lFontRegEntries
		global lOrigFontRegEntries
		global lFontRegItems

		# Clear out the list of colors
		set lOrigColorRegEntries {}

		# Now read the color items from the registry
		set index 0
		foreach item $lColorRegItems {
			lappend lOrigColorRegEntries [appRegistryEntryRead -default [lindex $lDefColorRegEntries $index] Colors $item]
			incr index
		}
		set lColorRegEntries $lOrigColorRegEntries

		set lOrigSyntaxColoring [appRegistryEntryRead -int -default 1 Colors SyntaxColoring]
		set lSyntaxColoring $lOrigSyntaxColoring

		# next read the font information
		set font [appRegistryEntryRead -default "" Editor FontInfo]
		if {$font == ""} {
			set font [appRegistryEntryRead -default "" Editor Font]
			if {$font != ""} {
				set font [split $font ,]
				# -13,0,0,0,400,0,0,0,0,1,2,1,49,Courier
				lappend lOrigFontRegEntries [lindex $font 13]
				set nextItem 10										;# font size
				regsub -all {\-} [lindex $font 0] {} nextItem
				lappend lOrigFontRegEntries $nextItem

				set nextItem 0										;# font weight Bold is 700, Normal is 400
				if {[lindex $font 4] == "700"} {
					set nextItem 1
				}
				lappend lOrigFontRegEntries $nextItem

				lappend lOrigFontRegEntries [lindex $font 5]		;# if italic
			} else {
			    set font [list Courier 10 0 0 Western]
			    set lOrigFontRegEntries $font
                        }

		} else {
			set lOrigFontRegEntries $font
		}
		set lFontRegEntries $lOrigFontRegEntries
	}

	proc save {} {
		global lOrigColorRegEntries
		global lColorRegEntries
		global lColorRegItems

		global lOrigSyntaxColoring
		global lSyntaxColoring

		global lOrigFontRegEntries
		global lFontRegEntries
		global lFontRegItems

		# first write the color items
		set index 0
		foreach item $lColorRegItems {
			appRegistryEntryWrite Colors $item [lindex $lColorRegEntries $index]
			if {[lindex $lColorRegEntries $index] != [lindex $lOrigColorRegEntries $index]} {
				onColorsChange 1
			}
			incr index
		}
		appRegistryEntryWrite -int Colors SyntaxColoring $lSyntaxColoring
		if {$lSyntaxColoring != $lOrigSyntaxColoring} {
			onColorsChange 1
		}

		# next write the font information
		appRegistryEntryWrite Editor FontInfo $lFontRegEntries
		if {$lFontRegEntries != $lOrigFontRegEntries} {
			onFontsChange 1
		}
	}
}

OptionsPageAdd "Fonts/Colors" FontsColorsPage::init FontsColorsPage::enter \
	FontsColorsPage::exit FontsColorsPage::restore FontsColorsPage::save
