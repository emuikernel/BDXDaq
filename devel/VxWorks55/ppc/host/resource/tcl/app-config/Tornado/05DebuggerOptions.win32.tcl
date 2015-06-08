##############################################################################
# 05DebuggerOptions.win32.tcl - Debugger options dialog implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01f,19mar99,j_k  Part of fix for SPR #24845.
# 01e,19oct98,rbl  Fixed bug where MemoryFormat and MemorySize were saved as ints
# 01d,06oct98,j_k  debugger memory settings were being ignored.
# 01c,22sep98,j_k  added turning on or off opcodes in disassembly display.
# 01b,07aug98,j_k  implemented debugger options page
# 01a,01jun98,j_k  written
#

#
# Do not source this file if the platform is not win32
#
if {![string match *win32* [wtxHostType]]} return

namespace eval DebuggerPage {
    set lDbgRegEntries {}
    set lOrigDbgRegEntries {}
    set bSaveDockingState 0

    variable gDebuggerPageCtrls [list dbgGeneralGrp hexDisplayBool \
	    bDockingViews bHaltOnAttach\
            autoAttachGrp bNeverAttch bSmartAttch bAlwaysAttch \
            dbgMemWndGrp defFmtLbl defFmtCmb defSzeLbl defSzeCmb refreshMemOnStop]

    proc init {} {
        variable lDbgRegEntries
        variable lOrigDbgRegEntries

        variable gDebuggerPageCtrls

        set Controls [list \
            [list group -name dbgGeneralGrp -title "General" \
                -xpos 12 -ypos 20 -width 286 -height 30] \
            [list boolean -auto -name hexDisplayBool -title "He&xadecimal display" \
	        -xpos 20 -ypos 32 -width 80 -height 12] \
            [list boolean -auto -name bDockingViews -title "Doc&king views" \
	        -xpos 129 -ypos 32 -width 70 -height 12] \
            [list boolean -auto -title "Always halt after attaching to a task" \
                -name bHaltOnAttach -xpos 20 -ypos 55 -width 130 -height 10] \
            [list group -name autoAttachGrp -title "Auto attach to tasks" \
                -xpos 155 -ypos 70 -width 115 -height 50] \
            [list choice -newgroup -auto -title "Never" -name bNeverAttch \
                -xpos 160 -ypos 80 -width 105 -height 9] \
            [list choice -auto -title "Only if not already attached" -name bSmartAttch \
                -xpos 160 -ypos 92 -width 105 -height 9] \
            [list choice -auto -title "Always" -name bAlwaysAttch \
                -xpos 160 -ypos 104 -width 105 -height 9] \
            [list group -name dbgMemWndGrp -title "Memory Window" \
	        -xpos 12 -ypos 70 -width 135 -height 63] \
            [list label -name defFmtLbl -title "Format:" \
	        -xpos 20 -ypos 83 -width 30 -height 9] \
            [list combo -name defFmtCmb \
	        -xpos 50 -ypos 81 -width 85 -height 60] \
            [list label -name defSzeLbl -title "Size:" \
	        -xpos 20 -ypos 99 -width 30 -height 9] \
            [list combo -name defSzeCmb \
	        -xpos 50 -ypos 97 -width 85 -height 60] \
            [list boolean -auto -name refreshMemOnStop \
	        -title "Always refresh on debugger stop" \
	        -xpos 20 -ypos 115 -width 120 -height 12] \
        ]
        set listNoUsed [list \
        [list group -name dbgDisassGrp -title "Disassembly" \
	        -xpos 12 -ypos 120 -width 190 -height 25] \
        [list boolean -auto -name showOpcodesBool -title "Show Opcodes" \
	        -xpos 19 -ypos 132 -width 70 -height 9] \
            ]

        foreach ctrl $Controls {
            lappend ctrl -hidden
            controlCreate tornadoOptionsDlg $ctrl
        }
        controlValuesSet tornadoOptionsDlg.defFmtCmb \
	        [list Hex/ASCII Octal Hex Decimal "Unsigned decimal" \
		        Binary Float Address Instruction Char String]

        controlValuesSet tornadoOptionsDlg.defSzeCmb \
	        [list Byte Halfword Word "Giant (8 bytes)"]

        controlCheckSet tornadoOptionsDlg.hexDisplayBool [lindex $lDbgRegEntries 0]
        controlCheckSet tornadoOptionsDlg.refreshMemOnStop [lindex $lDbgRegEntries 3]

        controlSelectionSet tornadoOptionsDlg.defFmtCmb -string [lindex $lDbgRegEntries 1]
        controlSelectionSet tornadoOptionsDlg.defSzeCmb -string [lindex $lDbgRegEntries 2]

        set rdButton [lindex $lDbgRegEntries 4]
        if {$rdButton == 2} {
            controlCheckSet tornadoOptionsDlg.bNeverAttch 0
            controlCheckSet tornadoOptionsDlg.bSmartAttch 0
            controlCheckSet tornadoOptionsDlg.bAlwaysAttch 1
        } elseif {$rdButton == 1} {
            controlCheckSet tornadoOptionsDlg.bNeverAttch 0
            controlCheckSet tornadoOptionsDlg.bSmartAttch 1
            controlCheckSet tornadoOptionsDlg.bAlwaysAttch 0
        } else {
            controlCheckSet tornadoOptionsDlg.bNeverAttch 1
            controlCheckSet tornadoOptionsDlg.bSmartAttch 0
            controlCheckSet tornadoOptionsDlg.bAlwaysAttch 0
        }
        controlCheckSet tornadoOptionsDlg.bDockingViews [lindex $lDbgRegEntries 5]
        controlCheckSet tornadoOptionsDlg.bHaltOnAttach [lindex $lDbgRegEntries 6]
    }

    proc enter {} {
        variable gDebuggerPageCtrls

        foreach ctrl $gDebuggerPageCtrls {
            controlShow tornadoOptionsDlg.$ctrl 1
        }
    }

    proc exit {} {
        variable lDbgRegEntries
        variable gDebuggerPageCtrls

        foreach ctrl $gDebuggerPageCtrls {
            controlShow tornadoOptionsDlg.$ctrl 0
        }

        set lDbgRegEntries {}
        lappend lDbgRegEntries [controlChecked tornadoOptionsDlg.hexDisplayBool]

        lappend lDbgRegEntries [controlSelectionGet tornadoOptionsDlg.defFmtCmb -string]
        lappend lDbgRegEntries [controlSelectionGet tornadoOptionsDlg.defSzeCmb -string]
        lappend lDbgRegEntries [controlChecked tornadoOptionsDlg.refreshMemOnStop]

        set rdButton 0
        if {[controlChecked tornadoOptionsDlg.bSmartAttch] == 1} {
            set rdButton 1
        } elseif {[controlChecked tornadoOptionsDlg.bAlwaysAttch] == 1} {
            set rdButton 2
        }

        lappend lDbgRegEntries $rdButton
        lappend lDbgRegEntries [controlChecked tornadoOptionsDlg.bDockingViews]

        lappend lDbgRegEntries [controlChecked tornadoOptionsDlg.bHaltOnAttach]

        return 1
    }

    proc restore {} {
        variable lDbgRegEntries
        variable lOrigDbgRegEntries
        variable bSaveDockingState

        set bSaveDockingState [appRegistryEntryRead -int -default 1 Debugger DockingViews]
        lappend lDbgRegEntries [appRegistryEntryRead -int -default 0 Debugger HexDisplay]
        lappend lDbgRegEntries [appRegistryEntryRead -default "Hex/ASCII" Debugger MemoryFormat]
        lappend lDbgRegEntries [appRegistryEntryRead -default "Byte" Debugger MemorySize]
        lappend lDbgRegEntries [appRegistryEntryRead -int -default 1 Debugger MemoryRefreshOnStop]
        lappend lDbgRegEntries [appRegistryEntryRead -int -default 0 Debugger AutoAttach]
        lappend lDbgRegEntries $bSaveDockingState
        lappend lDbgRegEntries [appRegistryEntryRead -int -default 1 Debugger HaltOnAttach]

        set lOrigDbgRegEntries $lDbgRegEntries
    }

    proc save {} {
        variable lDbgRegEntries
        variable lOrigDbgRegEntries
        variable bSaveDockingState

        set bDocking [lindex $lDbgRegEntries 5]
        appRegistryEntryWrite -int Debugger HexDisplay [lindex $lDbgRegEntries 0]
        appRegistryEntryWrite Debugger MemoryFormat [lindex $lDbgRegEntries 1]
        appRegistryEntryWrite Debugger MemorySize [lindex $lDbgRegEntries 2]
        appRegistryEntryWrite -int Debugger MemoryRefreshOnStop [lindex $lDbgRegEntries 3]
        appRegistryEntryWrite -int Debugger AutoAttach [lindex $lDbgRegEntries 4]
        appRegistryEntryWrite -int Debugger DockingViews $bDocking
        appRegistryEntryWrite -int Debugger HaltOnAttach [lindex $lDbgRegEntries 6]

        if {$bDocking != $bSaveDockingState} {
            appRegistryEntryWrite -int CmdLineWindow bDocking $bDocking
            appRegistryEntryWrite -int BacktraceWindow bDocking $bDocking
            appRegistryEntryWrite -int VariablesWindow bDocking $bDocking
            appRegistryEntryWrite -int WatchWindow bDocking $bDocking
            appRegistryEntryWrite -int RegistersWindow bDocking $bDocking
            appRegistryEntryWrite -int MemoryWindow bDocking $bDocking
        }

        crosswind eval "setPersitentSettings"
    }
}

OptionsPageAdd "Debugger" DebuggerPage::init DebuggerPage::enter \
	DebuggerPage::exit DebuggerPage::restore DebuggerPage::save
