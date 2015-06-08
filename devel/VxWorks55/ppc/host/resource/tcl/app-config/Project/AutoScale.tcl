# AutoScale.tcl - Code for the auto scale 'Explain' button shown from
# DepDlg.tcl
#
# modification history
# --------------------
# 01d,08jul02,cjs  Fix spr 79395 -- autoscale info ok button not working
# 01c,05mar99,cjs  Fixing path used in component find dialog color clearing
#                  logic
# 01b,01feb99,ren  Adding help button.
# 01a,19oct,cjs  Written.
# 
#############################################################

set ns ::ComponentViewServer::gui::AutoScaleExplainDlg
namespace eval $ns {
	variable _lastSheet "Symbols"
	variable _autoScaleData

	proc autoScaleExplain {data} {
		variable _autoScaleData

		# Convert this structure to an array
		set hProj [::Object::evaluate [::Object::evaluate \
			[::Workspace::selectionGet] Project] Handle]
		set prjName [::Workspace::projectNameGet $hProj] 
		set cDoc [::Workspace::cxrDocGet $prjName]
		set cSet [$cDoc setCreate]
		foreach struct $data {
			set userModule [lindex $struct 0]
			set syms [lindex $struct 1]
			set mods [lindex $struct 2]

			# Convert the components to friendly names 
			set comps [lindex $struct 3]
			$cSet = $comps
			set comps \
				[::ComponentViewServer::idToName $cSet]
			set _autoScaleData($userModule) [list $syms $mods $comps]
		}
		$cSet delete

		set ns ::ComponentViewServer::gui::AutoScaleExplainDlg
		dialogCreate -name explainDlg -title "AutoScale -- more info" \
			-w 271 -h 207 -init ${ns}::onInit \
			-controls [list \
				[list button -name okbutton -title OK \
					-x 111 -y 183 -w 50 -h 14 -callback ${ns}::onOk] \
				[list label -name lbl1 \
					-title "Select an application or BSP module from the list \
						below.\nThe tabbed panes indicate the symbols \
						imported by the selected application or BSP module \
						and the modules and components that define these \
						symbols.\nThese modules and components will be \
						dragged in to resolve the external symbols." \
					-x 7 -y 7 -w 256 -h 40] \
				[list combo -name comboUserModule -x 7 -y 60 -w 256 -h 57 \
					-callback ${ns}::onSelChange] \
				[list label -name lbl2 -title "User and BSP modules" \
					-x 7 -y 49 -w 74 -h 10] \
				[list tab -name tabctrl -x 7 -y 79 -w 256 -h 98 \
					-callback ${ns}::onTabClick] \
			]
	}

	proc onInit {} {
		variable _autoScaleData
		variable _lastSheet

		foreach pane {Symbols Modules Components} {
			controlTabsAdd explainDlg.tabctrl $pane
			controlCreate explainDlg [list list -name ${pane}Sheet \
				-title $pane -x 10 -y 98 \
				-w 250 -h 79]
			controlHide explainDlg.${pane}Sheet 1
		}

		controlSelectionSet explainDlg.tabctrl -string $_lastSheet
		onTabClick
		controlValuesSet explainDlg.comboUserModule \
			[array names _autoScaleData]
		controlSelectionSet explainDlg.comboUserModule 0 
	}

	proc onTabClick {} {
		variable _lastSheet
		controlHide explainDlg.${_lastSheet}Sheet 1
		set curSheet [controlSelectionGet explainDlg.tabctrl -string]
		controlHide explainDlg.${curSheet}Sheet 0
		set _lastSheet $curSheet
	}

	proc onSelChange {} {
		variable _autoScaleData
		set curSel [controlSelectionGet explainDlg.comboUserModule -string]
		set syms [lindex $_autoScaleData($curSel) 0]
		controlValuesSet explainDlg.SymbolsSheet $syms
		set modules [lindex $_autoScaleData($curSel) 1]
		controlValuesSet explainDlg.ModulesSheet $modules
		set components [lindex $_autoScaleData($curSel) 2]
		controlValuesSet explainDlg.ComponentsSheet $components
	}

	proc onOk {} {
		windowClose explainDlg
	}
}
