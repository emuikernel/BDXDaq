# 03DownloadOptionsPage.win32.tcl - prop page for download 
#
# modification history
# --------------------
# 01b,03jun02,cjs  Make sure controls are hidden when departing from page
# 01a,23may02,cjs  Rewrote 
#
# DESCRIPTION
# Windows-only property page for setting download options from
# project facility
#
#############################################################

namespace eval DownloadOptionsPage {

    #############################################################
    #                           DATA
    #############################################################

    variable _window tornadoOptionsDlg
    variable _strTitle "Download"
    variable _primaryCtrls ""

    variable _lastComboSelection ""

    variable _strDefaultOptions "LOAD_GLOBAL_SYMBOLS|LOAD_COMMON_MATCH_ALL|LOAD_CPLUS_XTOR_AUTO"
    variable _listRegistryKey [list Download Options]

    # These are flag values for various options 
    array set _ctrlsToFlags { \
        boolLoadLocalSyms LOAD_LOCAL_SYMBOLS \
        boolLoadGlobSyms LOAD_GLOBAL_SYMBOLS \
        choiceCommonMatchNone LOAD_COMMON_MATCH_NONE \
        choiceCommonMatchUser LOAD_COMMON_MATCH_USER \
        choiceCommonMatchAll LOAD_COMMON_MATCH_ALL \
        boolLoadHiddenMod    LOAD_HIDDEN_MODULE \
        choiceCplusXtorAuto LOAD_CPLUS_XTOR_AUTO \
        choiceCplusXtorManual LOAD_CPLUS_XTOR_MANUAL \
    }
    array set _flagsToCtrls {
        LOAD_LOCAL_SYMBOLS boolLoadLocalSyms \
        LOAD_GLOBAL_SYMBOLS boolLoadGlobSyms \
        LOAD_COMMON_MATCH_NONE choiceCommonMatchNone \
        LOAD_COMMON_MATCH_USER choiceCommonMatchUser \
        LOAD_COMMON_MATCH_ALL choiceCommonMatchAll \
        LOAD_HIDDEN_MODULE boolLoadHiddenMod \
        LOAD_CPLUS_XTOR_AUTO choiceCplusXtorAuto \
        LOAD_CPLUS_XTOR_MANUAL choiceCplusXtorManual \
    }

    # This is descriptive help for various options
    array set _tooltips {
        boolLoadLocalSyms "Only local symbols are added to the system symbol table" \
        boolLoadGlobSyms "Only external symbols are added to the system symbol table" \
        choiceCommonMatchNone "Allocate common symbols, but do not search for any matching symbols" \
        choiceCommonMatchUser "Allocate common symbols, but search for matching symbols in user-loaded modules" \
        choiceCommonMatchAll "Allocate common symbols, but search for matching symbols in user-loaded modules and the target-system core file" \
        boolLoadHiddenMod "Load the module but make it invisible to WTX_MSG_MODULE_LIST" \
        choiceCplusXtorAuto "C++ ctors/dtors management is explicitly turned on " \
        choiceCplusXtorManual "C++ ctors/dtors management is explicitly turned off" \
    }

    variable _sortedCategoryList \
    [ list \
       "System Symbol Table" "Common Symbol Resolution" \
       "C++ Constructors" "Misc" \
    ]

    # These are controls managing the various options
    # Note: Keep _sortedCategoryList (above) in sync
    array set _data [list \
        "System Symbol Table" \
	[list \
            [list boolean -name boolLoadLocalSyms \
                -title "Load local symbols (LOAD_LOCAL_SYMBOLS)" \
                -callback {DownloadOptionsPage::onOption boolLoadLocalSyms} \
                -auto -newgroup -hidden \
                -x 18 -y 52 -w 240 -h 24 \
            ] \
            [list boolean -name boolLoadGlobSyms \
                -title "Load global symbols (LOAD_GLOBAL_SYMBOLS)" \
                -callback {DownloadOptionsPage::onOption boolLoadGlobSyms} \
                -auto -hidden \
                -x 18 -y 68 -w 240 -h 24 \
            ] \
        ] \
    ]

    array set _data [list \
        "Common Symbol Resolution" \
	[list \
            [list choice -name choiceCommonMatchNone \
                -title "Don't match common symbols (LOAD_COMMON_MATCH_NONE)" \
                -callback {DownloadOptionsPage::onOption choiceCommonMatchNone} \
                -auto -newgroup -hidden \
                -x 18 -y 52 -w 240 -h 24 \
            ] \
            [list choice -name choiceCommonMatchUser \
                -title "Match user symbols (LOAD_COMMON_MATCH_USER)" \
                -callback {DownloadOptionsPage::onOption choiceCommonMatchUser} \
                -auto -hidden \
                -x 18 -y 68 -w 240 -h 24 \
            ] \
            [list choice -name choiceCommonMatchAll \
                -title "Match all symbols (LOAD_COMMON_MATCH_ALL)" \
                -callback {DownloadOptionsPage::onOption choiceCommonMatchAll} \
                -auto -hidden \
                -x 18 -y 84 -w 240 -h 24 \
            ] \
        ] \
    ]

    array set _data [list \
        "Misc" \
        [list \
            [list boolean -name boolLoadHiddenMod \
                -title "Load, defeat WTX_MSG_MODULE_LIST (LOAD_HIDDEN_MODULE)" \
                -callback {DownloadOptionsPage::onOption boolLoadHiddenMod} \
                -auto -newgroup -hidden \
                -x 18 -y 52 -w 240 -h 24 \
            ] \
        ] \
    ]

    array set _data [list \
        "C++ Constructors" \
        [list \
            [list choice -name choiceCplusXtorAuto \
                -title "Fire constructors/destructors (LOAD_CPLUS_XTOR_AUTO)" \
                -callback {DownloadOptionsPage::onOption choiceCplusXtorAuto} \
                -auto -newgroup -hidden \
                -x 18 -y 52 -w 240 -h 24 \
            ] \
            [list choice -name choiceCplusXtorManual \
                -title "Don't fire constructors/destructors (LOAD_CPLUS_XTOR_MANUAL)" \
                -callback {DownloadOptionsPage::onOption choiceCplusXtorManual} \
                -auto -hidden \
                -x 18 -y 68 -w 240 -h 24 \
            ] \
        ] \
    ]

    #############################################################
    #                   Property Page Callbacks 
    #############################################################

    proc onPageInit {} {
        createControls
        set options [GetWtxObjModuleLoadOptions]
        controlsStatesSet $options
    }

    
    proc onPageEnter {} {
        variable _window
        variable _lastComboSelection

        set curSel [controlSelectionGet ${_window}.comboCategory -string] 
        if {$curSel == "" || $_lastComboSelection == ""} {
            set _lastComboSelection [defaultCategoryGet]
            controlSelectionSet ${_window}.comboCategory \
                -string $_lastComboSelection
            set curSel [controlSelectionGet ${_window}.comboCategory -string] 
        }
        hideControls $curSel 0
    }

    proc queryPageExit {} {
        variable _window

        set curSel [controlSelectionGet ${_window}.comboCategory -string] 
        hideControls $curSel 1
        return 1
    }

    proc onPageRestore {} {
    }

    proc onPageSave {} {
        variable _window

        set options [controlsStatesGet]
	SetWtxObjModuleLoadOptions $options

        set category [controlSelectionGet ${_window}.comboCategory -string]
        hideControls $category 1
    }

    #############################################################
    #                   Helper Routines 
    #############################################################

    proc categoryListGet {} {
        variable _sortedCategoryList
        return $_sortedCategoryList
    }

    proc defaultCategoryGet {} {
        return "System Symbol Table" 
    }

    proc createControls {} {
        variable _window
        variable _primaryCtrls
        variable _data

        # The controls created immediately below must be added to _primaryCtrls 
        # for tracking
        set _primaryCtrls { \
            comboCategory grpOptions buttonUseDefaults lblCurrentDesc \
        }

        controlCreate $_window "group -name grpOptions -hidden \
            -x 14 -y 36 -w 270 -h 130"
        controlCreate $_window "combo -name comboCategory -hidden \
            -callback DownloadOptionsPage::onSelChangeComboCategory \
            -x 14 -y 22 -w 120 -h 48"
        controlCreate $_window "button -name buttonUseDefaults -hidden \
            -title {Set &Defaults} \
            -callback DownloadOptionsPage::onSetDefault \
            -x 234 -y 170 -w 50 -h 14"
        controlCreate $_window "label -name lblCurrentDesc -hidden \
            -title {For more info, see wtxObjModuleLoad} \
            -x 18 -y 140 -w 260 -h 24"

        controlValuesSet ${_window}.comboCategory [categoryListGet]


        foreach category [categoryListGet] {
            if {$category != ""} {
                foreach spec [set _data($category)] {
                    set name [lindex $spec 2]
                    controlCreate $_window $spec
                }
            }
        }
    }

    proc hideControls {category bHide} {
        variable _primaryCtrls
        variable _window
        variable _data

        foreach ctrl $_primaryCtrls {
	    controlHide ${_window}.$ctrl $bHide
        }

        if {$category != ""} {
            foreach spec [set _data($category)] { 
                set name [lindex $spec 2]
                controlHide ${_window}.$name $bHide
            } 
        }
    }

    # Someone or something has selected a new category of flags;
    # hide some and display a new set
    proc onSelChangeComboCategory {} {
        variable _window
        variable _lastComboSelection

        set eventInfo [controlEventGet ${_window}.comboCategory]

        if {[string match selchange [lindex $eventInfo 0]]} {
            set newComboSelection \
                [controlSelectionGet ${_window}.comboCategory -string]
            hideControls $_lastComboSelection 1
            set _lastComboSelection $newComboSelection
            hideControls $newComboSelection 0
        }

        controlFocusSet ${_window}.comboCategory
        controlTextSet ${_window}.lblCurrentDesc \
            "(Selection an option with the mouse to get details)"
    }

    # Callback whenever someone makes a choice
    # Gives help
    proc onOption {me} {
        variable _window
        variable _tooltips
        set desc "Currently focused option: [set _tooltips($me)]"
        controlTextSet ${_window}.lblCurrentDesc $desc
    }

    # Set controls' states based on options string
    proc controlsStatesSet {options} {
        variable _window
        variable _flagsToCtrls
        variable _ctrlsToFlags

        # unset all
        foreach ctrl [array names _ctrlsToFlags] {
            controlCheckSet ${_window}.$ctrl 0
        }

        # set those active 
        set options [split $options |]
        foreach option $options {
            # Note: some flags are combinatorial, and involve multiple ctrls
            set ctrls [getCtrlsForOption $option]
            foreach ctrl $ctrls {
                controlCheckSet ${_window}.$ctrl 1
            } 
        }
    }

    # Return an options string based on controls' states 
    proc controlsStatesGet {} {
        variable _window
        variable _ctrlsToFlags

        set flags ""
        foreach ctrl [array names _ctrlsToFlags] {
            set bSet [controlChecked ${_window}.$ctrl]

            if {$bSet} {
                set flag [set _ctrlsToFlags($ctrl)]
                if {$flags != ""} {
                    set flags "${flags}|$flag"
                } else {
                    set flags $flag
                }
            }
        }
        return $flags
    }

    # Given an option, map it to the control to enable it
    proc getCtrlsForOption {option} {
        variable _flagsToCtrls
	if {[info exists _flagsToCtrls($option)]} {
            return [set _flagsToCtrls($option)]
        } else {
            if {[string match $option LOAD_ALL_SYMBOLS]} {
                return [list \
                    [getCtrlsForOption $LOAD_LOCAL_SYMBOLS]
                    [getCtrlsForOption $LOAD_GLOBAL_SYMBOLS]]
            } elseif {[string match $option LOAD_NO_SYMBOLS]} { 
                return ""
            } else {
                return ""
            }
        }
    }

    # Read options from the registry
    proc GetWtxObjModuleLoadOptions {} {
        variable _strDefaultOptions
        variable _listRegistryKey
        eval appRegistryEntryRead -default \
            $_strDefaultOptions $_listRegistryKey
    }

    # Get the defaults, ignoring the registry 
    proc GetDefaultWtxObjModuleLoadOptions {} {
        variable _strDefaultOptions
        return $_strDefaultOptions
    }

    # Write options to the registry
    proc SetWtxObjModuleLoadOptions {options} {
        variable _listRegistryKey
        eval appRegistryEntryWrite $_listRegistryKey [list $options]
    }

    # Callback; user has requested the defaults
    proc onSetDefault {} {
        variable _window
        set defaultOptions [GetDefaultWtxObjModuleLoadOptions]
        controlsStatesSet $defaultOptions 

        controlFocusSet ${_window}.comboCategory
        controlTextSet ${_window}.lblCurrentDesc \
            "(Selection an option with the mouse to get details)"
    }
    
    #########################################################

    OptionsPageAdd $_strTitle \
	    [namespace current]::onPageInit \
	    [namespace current]::onPageEnter \
	    [namespace current]::queryPageExit \
	    [namespace current]::onPageRestore \
	    [namespace current]::onPageSave	    
}
