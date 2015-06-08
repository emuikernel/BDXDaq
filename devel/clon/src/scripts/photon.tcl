# Photon Run_sheet
# by VHG and TA
# 04.07.98
#============================
global photon_trig_conf radiator scaler_config1 scaler_config2 scaler_config3 scaler_config4 PS_converter collimator1 collimator2 PC_converter TAC RadPhi et_coincidence tagger_prescaling runtype tk_mt_cur Tag_cur PS_cur

set font1 -Adobe-Times-Bold-R-Normal-*-140-*

proc positionWindow w {
    wm geometry $w +300+300
}
 
global env clonparms clonwsrc
global archivfile arcomment aropper texto j

#global global alarm_thr_scale alarm_thr

#proc LL { a } {

#global alarm_thr_scale alarm_thr
#global env clonparms clonwsrc

#set  alarm_thr [ $alarm_thr_scale get ]

#}

#set alarm_thr_scale .phot.container4.f9.entry




set clonparms $env(CLON_PARMS)
set clonwsrc $env(CLON_SOURCE)

set alarm ${clonparms}/photon/alarm.txt

set arcomment "/usr/local/clas/parms/runsheet/arcomment.txt"
set aropper "/usr/local/clas/parms/runsheet/aropper.txt"
set printfile "/usr/local/clas/parms/runsheet/temprint.txt"


# photon trigger conf read
set f [open ${clonparms}/photon/g2/trigger_config.dat r]
gets $f line
set photon_trig_conf $line
#puts $trig_conf
close $f

set f [open ${clonparms}/photon/g2/scaler_config.dat r]
gets $f line
set scaler_config1 $line
gets $f line
set scaler_config2 $line
gets $f line
set scaler_config3 $line
gets $f line
set scaler_config4 $line
close $f
set f [open ${clonparms}/photon/g2/tagger_prescale.dat r]
gets $f line
set tagger_prescaling $line
close $f

set ps_1 0
set ps_2 0
set ps_3 0
set ps_4 0
set pc_1 0
set pc_2 0
set pc_3 0
set pc_4 0
set tac_1 0
set tac_2 0
set tac_3 0
set tac_4 0
if { $scaler_config1 == "PS-Delay"} {set ps_1 1}
if { $scaler_config1 == "  "} {set pc_1 1}
if { $scaler_config1 == "TAC"} {set tac_1 1}

if { $scaler_config2 == "PS"} {set ps_2 1}
if { $scaler_config2 == "        "} {set pc_2 1}
if { $scaler_config2 == "TAC"} {set tac_2 1}

if { $scaler_config3 == "PS"} {set ps_3 1}
if { $scaler_config3 == "        "} {set pc_3 1}
if { $scaler_config3 == "TAC"} {set tac_3 1}

if { $scaler_config4 == "PS-Delay"} {set ps_4 1}
if { $scaler_config4 == "  "} {set pc_4 1}
if { $scaler_config4 == "TAC"} {set tac_4 1}



toplevel .phot -class Dialog 
wm title .phot "Photon Run Sheet"
positionWindow .phot

frame .phot.buttons -bg DeepSkyBlue4
button .phot.buttons.psheet -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text "Previous" -relief raised -font $font1   -bd 7 -command { 
set f [open  /usr/local/clas/parms/runsheet/prevphoton.txt r]
gets $f line

set radiator [lindex $line 0]
set PS_converter [lindex $line 1]
set PC_converter [lindex $line 2]
set TAC [lindex $line 3]
set RadPhi [lindex $line 4]
set et_coincidence [lindex $line 5]
set collimator1 [lindex $line 6]
set collimator2 [lindex $line 7]
set tagger_prescaling [lindex $line 8]
close $f
}


button .phot.buttons.cancel -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Cancel -relief raised -font $font1   -bd 7 -command {
set runtype " "
destroy .phot 
}

button .phot.buttons.done -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Done -relief raised -font $font1   -bd 7 -command { 
#set f [open $alarm w+]
#puts $f $alarm_thr 
#close $f
if { $radiator == "-" || $PS_converter == "-" || $PC_converter == "-" || $TAC == "-" || $RadPhi == "-" || $et_coincidence == "-" || $collimator1 == "-" || $collimator2 == "-"   
} {

toplevel .dlg -class Dialog
wm title .dlg "Alarm"
positionWindow .dlg
frame .dlg.top -relief raised -bd 1 -bg DeepSkyBlue4
pack .dlg.top -side top -fill both
frame .dlg.bot -relief raised -bd 1 -bg DeepSkyBlue4
pack .dlg.bot -side bottom -fill both

message .dlg.top.msg -width 3i -text "Not Complete Entry" -font -Adobe-Times-Bold-R-Normal-*-180-*
pack .dlg.top.msg -side right -expand 1 -fill both -padx 3m -pady 3m

button .dlg.bot.bt -width 11 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Return -relief raised  -bd 7 -command {
destroy .dlg
#focus $oldFocus
}
pack .dlg.top.msg -side right -expand 1 -fill both -padx 3m -pady 3m
pack .dlg.bot.bt -side bottom   
} else {

set f [open /usr/local/clas/parms/runsheet/prevphoton.txt w+]
puts $f "$radiator $PS_converter $PC_converter $TAC $RadPhi $et_coincidence $collimator1 $collimator2 $tagger_prescaling"
close $f

destroy .phot
}
}



#====== triger_type selection ==============================
#set m .phot.f1.label1.trgtype

    frame .phot.f1 -bd 2 -bg DeepSkyBlue4
    label .phot.f1.entry1  -relief sunken -font $font1   -bd 3 -width 55  -fg black -bg azure3 -textvariable photon_trig_conf -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .phot.f1.label1 -relief raised -font $font1   -bd 4 -width 12 -text "Trigger type" 
    label .phot.f1.label2 -fg yellow -bg DeepSkyBlue4 -width 12 -text "* PHOTON *" -font -Adobe-Times-Bold-R-Normal-*-180-* 

    pack .phot.f1.label1 -side left -padx 0m
    pack .phot.f1.entry1 -side left -padx 1m
    pack .phot.f1.label2 -side left -padx 3m


#===================scaler coin. ===============
frame .phot.f0 -bd 2 -bg DeepSkyBlue4
label .phot.f0.label1  -fg black -relief sunken -font $font1   -bd 3 -width 16  -fg black -bg azure3 -text " scaler1 odd" 
label .phot.f0.label2  -fg black -relief sunken -font $font1   -bd 3 -width 16  -fg black -bg azure3 -text " scaler1 even" 
label .phot.f0.label3  -fg black -relief sunken -font $font1   -bd 3 -width 16  -fg black -bg azure3 -text " scaler2 odd" 
label .phot.f0.label4  -fg black -relief sunken -font $font1   -bd 3 -width 16  -fg black -bg azure3 -text " scaler2 even" 

    frame .phot.f2 -bd 2 -bg DeepSkyBlue4
checkbutton .phot.f2.b1 -text "PS Delay" -variable ps_1 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f2.b2 -text "PS        " -variable ps_2 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f2.b3 -text "PS        " -variable ps_3 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f2.b4 -text "PS Delay" -variable ps_4 -width 13 -relief raised -font $font1  -fg black -state disabled


    frame .phot.f3 -bd 2 -bg DeepSkyBlue4
checkbutton .phot.f3.b1 -text "          " -variable pc_1 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f3.b2 -text "        " -variable pc_2 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f3.b3 -text "        " -variable pc_3 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f3.b4 -text "          " -variable pc_4 -width 13 -relief raised -font $font1  -fg black -state disabled

    frame .phot.f4 -bd 2 -bg DeepSkyBlue4
checkbutton .phot.f4.b1 -text "TAC       " -variable tac_1 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f4.b2 -text "TAC       " -variable tac_2 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f4.b3 -text "TAC       " -variable tac_3 -width 13 -relief raised -font $font1  -fg black -state disabled
checkbutton .phot.f4.b4 -text "TAC       " -variable tac_4 -width 13 -relief raised -font $font1  -fg black -state disabled


pack .phot.f0.label1 .phot.f0.label2 .phot.f0.label3 .phot.f0.label4 -side left -padx 8m -anchor w
pack .phot.f2.b1 .phot.f2.b2 .phot.f2.b3 .phot.f2.b4 -side left -padx 8m -anchor w
pack .phot.f3.b1 .phot.f3.b2 .phot.f3.b3 .phot.f3.b4 -side left -padx 8m -anchor w
pack .phot.f4.b1 .phot.f4.b2 .phot.f4.b3 .phot.f4.b4 -side left -padx 8m -anchor w

#=================================================================
#============= sector radiator number and target magnet current===

    frame .phot.f6 -bd 2 -bg DeepSkyBlue4

    label .phot.f6.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 


    label .phot.f6.chap1 -width 5  -bd 4 -fg black -bg azure3 -relief raised -font $font1   -text " A " 
    label .phot.f6.label1 -relief raised -font $font1   -bd 4  -fg black -bg azure3 -width 15 -text " PS Current"
    label .phot.f6.number1 -width 11 -fg black -bg azure3 -relief sunken -font $font1   -bd 3 -textvariable PS_cur 

    label .phot.f6.chap2 -width 5  -bd 4 -fg black -bg azure3 -relief raised -font $font1   -text " A " 
    label .phot.f6.label2 -relief raised -font $font1   -bd 4  -fg black -bg azure3 -width 15 -text "Tagger Current" 
    label .phot.f6.number2 -width 11 -fg black -bg azure3 -relief sunken -font $font1   -bd 3 -textvariable Tag_cur -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .phot.f6.label1 -side left -padx 0m
    pack .phot.f6.number1 -side left -padx 1m
    pack .phot.f6.chap1 -side left -padx 0m

    pack .phot.f6.label -side left -padx 2m

    pack .phot.f6.label2 -side left -padx 0m
    pack .phot.f6.number2 -side left -padx 1m
    pack .phot.f6.chap2 -side left -padx 0m


#=====================================================================

catch { set PS_cur  [ exec get_epics PSPECIRBCK ] } result

#set PS_cur $tk_mt_cur mini torus value
#------------------------------------------------------------
# settings
set radiator - 
set PS_converter -
set PC_converter -
set TAC -
set RadPhi -
set et_coincidence -
set collimator1 -
set collimator2 -

#=====================================================================
    frame .phot.container1 -bd 2 -bg DeepSkyBlue4
    set cont1 .phot.container1 

    frame $cont1.f8 -bd 2 -bg DeepSkyBlue4

    menubutton $cont1.f8.radiator -relief raised -font $font1   -bd 4  -bg yellow2 -fg black -width 12 -text "Radiator" -menu $cont1.f8.radiator.m 
    label $cont1.f8.entry1  -relief sunken -font $font1   -bd 3 -width 15  -fg black -bg azure3 -textvariable radiator

    set m [menu $cont1.f8.radiator.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

$m add command -label "  A (3x10-4)  " -command {set radiator "A"}
    $m add separator
$m add command -label "  B (1x10-4)" -command {set radiator "B"}
    $m add separator
$m add command -label "  C (2x10-5)  " -command {set radiator "C"}
     $m add separator
$m add command -label "  D (2x10-5) " -command {set radiator "D"}

    pack $cont1.f8.radiator $cont1.f8.entry1 -side left -anchor w

    frame $cont1.f9 -bd 2 -bg DeepSkyBlue4

    menubutton $cont1.f9.collimator1 -relief raised -font $font1   -bd 4 -width 20  -bg yellow2 -fg black -text "Primary Collimator" -menu $cont1.f9.collimator1.m 
    label $cont1.f9.entry1  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable collimator1

    set m [menu $cont1.f9.collimator1.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label " IN  " -command {set collimator1 " IN"}
    $m add separator
    $m add command -label " OUT " -command {set collimator1 "OUT"}

    pack $cont1.f9.collimator1 $cont1.f9.entry1 -side left -anchor w

    frame $cont1.f10 -bd 2 -bg DeepSkyBlue4

    menubutton $cont1.f10.collimator2 -relief raised -font $font1   -bd 4 -width 20  -bg yellow2 -fg black -text "                  " -menu $cont1.f10.collimator2.m 
    label $cont1.f10.entry1  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable collimator2

    set m [menu $cont1.f10.collimator2.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label "     " -command {set collimator2 " "}
    $m add separator
    $m add command -label "     " -command {set collimator2 " "}

    pack $cont1.f10.collimator2 $cont1.f10.entry1 -side left -anchor w -fill x

	pack $cont1.f8 $cont1.f9 $cont1.f10 -side left -anchor w

##
##
##
    frame .phot.container2 -bd 2 -bg DeepSkyBlue4
    set cont2 .phot.container2 

    frame $cont2.f8 -bd 2 -bg DeepSkyBlue4

    menubutton $cont2.f8.ps -relief raised -font $font1   -bd 4 -width 12  -bg yellow2 -fg black -text "PS Converter" -menu $cont2.f8.ps.m 
    label $cont2.f8.entry1  -relief sunken -font $font1   -bd 3 -width 15  -fg black -bg azure3 -textvariable PS_converter

    set m [menu $cont2.f8.ps.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

$m add command -label "   0.01  " -command {set PS_converter "0.01"}
    $m add separator
$m add command -label "  0.001  " -command {set PS_converter "0.001"}
    $m add separator
$m add command -label " 0.0001  " -command {set PS_converter "0.0001"}

    pack $cont2.f8.ps $cont2.f8.entry1 -side left

    frame $cont2.f9 -bd 2 -bg DeepSkyBlue4

    menubutton $cont2.f9.pc -relief raised -font $font1   -bd 4 -width 20  -bg yellow2 -fg black -text "            " -menu $cont2.f9.pc.m 
    label $cont2.f9.entry1  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable PC_converter

    set m [menu $cont2.f9.pc.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label "     " -command {set PC_converter "   "}
    $m add separator
    $m add command -label "     " -command {set PC_converter "   "}

    pack $cont2.f9.pc $cont2.f9.entry1 -side left

    frame $cont2.f10 -bd 2 -bg DeepSkyBlue4

    menubutton $cont2.f10.tac -relief raised -font $font1   -bd 4 -width 20  -bg yellow2 -fg black -text " TAC " -menu $cont2.f10.tac.m 
    label $cont2.f10.entry1  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable TAC

    set m [menu $cont2.f10.tac.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label " IN  " -command {set TAC " IN"}
    $m add separator
    $m add command -label " OUT " -command {set TAC "OUT"}

    pack $cont2.f10.tac $cont2.f10.entry1 -side left

	pack $cont2.f8 $cont2.f9 $cont2.f10 -side left -fill x

#=====================================================================
    frame .phot.container3 -bd 2 -bg DeepSkyBlue4
    set cont3 .phot.container3 

    frame $cont3.f8 -bd 2 -bg DeepSkyBlue4

    menubutton $cont3.f8.radiator -relief raised -font $font1   -bd 4  -bg yellow2 -fg black -width 12 -text "ET_coin." -menu $cont3.f8.radiator.m 
    label $cont3.f8.entry1  -relief sunken -font $font1   -bd 3 -width 15  -fg black -bg azure3 -textvariable et_coincidence

    set m [menu $cont3.f8.radiator.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

$m add command -label "  ON        " -command {set et_coincidence " ON"}
    $m add separator
$m add command -label "  OFF       " -command {set et_coincidence "OFF"}

    pack $cont3.f8.radiator $cont3.f8.entry1 -side left

    frame $cont3.f9 -bd 2 -bg DeepSkyBlue4

    label $cont3.entry1  -relief sunken -font $font1   -bd 3 -width 48  -fg black -bg azure3 -textvariable  tagger_prescaling -font -Adobe-Times-Bold-R-Normal-*-140-*
    label $cont3.label1 -relief raised -font $font1   -bd 4 -width 17 -text "Prescale" 

	pack $cont3.f8 $cont3.f9 $cont3.label1 $cont3.entry1 -side left -fill x

##
##################################################################
    frame .phot.container4 -bd 2 -bg DeepSkyBlue4
    set cont4 .phot.container4 

    frame $cont4.f9 -bd 2 -bg DeepSkyBlue4

    menubutton $cont4.f9.collimator1 -relief raised -font $font1   -bd 4 -width 11  -bg yellow2 -fg black -text "      " -menu $cont4.f9.collimator1.m 
    label $cont4.f9.entry1  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable RadPhi

    set m [menu $cont4.f9.collimator1.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label "     " -command {set RadPhi "   "}
    $m add separator
    $m add command -label "     " -command {set RadPhi "   "}

    pack $cont4.f9.collimator1 $cont4.f9.entry1  -side left


    menubutton $cont4.f9.coll -relief raised -font $font1   -bd 4 -width 11  -bg yellow2 -fg black -text "Prescale" -menu $cont4.f9.coll.m 
    label $cont4.f9.entrycoll  -relief sunken -font $font1   -bd 3 -width 10  -fg black -bg azure3 -textvariable prescaleonof

    set m [menu $cont4.f9.coll.m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*]

    $m add command -label " ON  " -command {set  tagger_prescaling  " ON"}
    $m add separator
    $m add command -label " OFF " -command {set tagger_prescaling "OFF"}


    pack $cont4.f9.coll   -side left -padx 3m 

    pack $cont4.f9 -side left


pack .phot.buttons.psheet -side left -expand 1 -fill x
pack .phot.buttons.done -side left -expand 1 -fill x
pack .phot.buttons.cancel -side left -expand 1 -fill x

pack  .phot.f1 .phot.f0 .phot.f2 .phot.f3 .phot.f4 .phot.f6 $cont1 $cont2 $cont3 $cont4 .phot.buttons  -side top -fill x










