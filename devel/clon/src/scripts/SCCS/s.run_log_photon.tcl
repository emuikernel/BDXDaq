h00090
s 00006/00003/00747
d D 1.4 04/11/12 10:01:17 sergpozd 5 4
c Some changes in reading mon_det_p.txt files
e
s 00003/00003/00747
d D 1.3 04/10/22 11:14:49 sergpozd 4 3
c removed " -1 -1 " from "dimanc" inputs
e
s 00006/00006/00744
d D 1.2 00/08/24 11:46:12 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 98/03/03 11:34:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 run_log/s/run_log_photon.tcl
e
s 00750/00000/00000
d D 1.1 98/03/03 11:34:13 wolin 1 0
c date and time created 98/03/03 11:34:13 by wolin
e
u
U
f e 0
t
T
I 1
#  run_log_comment.tcl

# by vhg, ejw, 22-jan-98


global archivfile arcomment aropper texto j 
global date runtype runnum beamcurrent target raster tk_b_ener tk_t_cur tk_mt_cur trig_conf chan_conf ec_inner_h ec_outer_h ec_total_h sc_h cc_h ec_inner_l ec_outer_l ec_total_l sc_l cc_l shiftcrue logbook page comment beamcr_scale logbook_scale page_scale

global .f3.entry2 .f17.text .f15.entry1 .f15.entry2
global env clonparms clonwsrc


  set clonparms $env(CLON_PARMS)
D 3
 set clonwsrc $env(CLON_WSOURCE)
E 3
I 3
 set clonwsrc $env(CLON_SOURCE)
E 3

proc JJ { a } {
global page page_scale
global env clonparms clonwsrc

set page [ $page_scale get ]
}


proc KK { a } {
global logbook logbook_scale
global env clonparms clonwsrc

set logbook [ $logbook_scale get ]
}


proc LL { a } {

global beamcurrent beamcr_scale
global env clonparms clonwsrc

set beamcurrent [ $beamcr_scale get ]
set beamcurrent  [expr $beamcurrent/10.0]
}

D 3
set monpath ${clonwsrc}/diman/gui/mondat
E 3
I 3
set monpath ${clonparms}/diman/mondat
E 3
 
D 3
set archivfile "/home/clasrun/runsheet/archiv.txt"
set arcomment "/home/clasrun/runsheet/arcomment.txt"
set aropper "/home/clasrun/runsheet/aropper.txt"
set printfile "/home/clasrun/runsheet/temprint.txt"
E 3
I 3
set archivfile "$clonparms/runsheet/archiv.txt"
set arcomment "$clonparms/runsheet/arcomment.txt"
set aropper "$clonparms/runsheet/aropper.txt"
set printfile "$clonparms/runsheet/temprint.txt"
E 3
set hin 0
set j 0
set runtype " "
set beamcurrent 0.0
set target " "
set raster " "
set logbook 0
set page 0
set date " "
set comment " "
set texto .f17.text
set beamcr_scale .f3.entry2
set logbook_scale .f15.entry1
set page_scale .f15.entry2


# get the data on date, thresholds, conf files, etc.

set date [exec date]
set newdate $date
catch { exec get_prescale_factors } result
catch { exec get_ts_l1mask } result


# level1 mask read
set f [open ${clonparms}/ts/current_ts_l1mask.txt r]
for {set i 1} { $i<=8} {incr i 1} { gets $f line }
set l1_mask $line
#puts $l1_mask
close $f

# prescale read
set f [open ${clonparms}/ts/current_prescale.txt r]
gets $f line 
set prescale $line
#puts $prescale
close $f


# trigger conf read
set f [open ${clonparms}/trigger/current_trig_config.txt r]
gets $f line 
set trig_conf $line
#puts $trig_conf
close $f

# channel conf read
#set f [open ${clonparms}/channel/current_trig_config.txt r]
#gets $f line 
set chan_conf ${clonparms}/channel/config/discr1.cfg
#close $f

set runnum $run
set tk_b_ener $b_ener
set tk_t_cur $t_cur
set tk_mt_cur $mt_cur

set newrun $run
set new_b_ener $b_ener
set new_t_cur $t_cur
set new_mt_cur $mt_cur

proc positionWindow w {
    wm geometry $w +300+300
}

wm title . "Run_Sheet"
positionWindow .

frame .buttons -bg DeepSkyBlue4
pack  .buttons -side bottom -expand y -fill x -pady 2m 
button .buttons.previous -width 4 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text "<<" -relief raised  -bd 7 -command {
readfile 
}
button .buttons.ptext -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text "Prevcom" -relief raised  -bd 7 -command {
set comment " "
set f [open $arcomment r]
set comment [read $f]
$texto delete 1.0 end
$texto mark set here 1.0
$texto insert here $comment
close $f
#$texto insert end $comment
#puts $comment
}
button .buttons.psheet -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text "Print" -relief raised  -bd 7 -command {
printsheet
}

button .buttons.new -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text New -relief raised  -bd 7 -command {
set runtype " "
$beamcr_scale set  0.0
set raster " "
set target " "
set logbook 0
$logbook_scale set 0
set page 0
$page_scale set 0
set comment " "
$texto delete 1.0 end
#$texto insert end $comment
set shiftcrue " "
}
button .buttons.cancel -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Abort -relief raised  -bd 7 -command {
set abort -2
destroy . 
}
button .buttons.done -width 7 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Ok -relief raised  -bd 7 -command {
  if { $target == " " } { 
toplevel .dlg -class Dialog
wm title .dlg "Alarm"
positionWindow .dlg
frame .dlg.top -relief raised -bd 1 -bg DeepSkyBlue4
pack .dlg.top -side top -fill both
frame .dlg.bot -relief raised -bd 1 -bg DeepSkyBlue4
pack .dlg.bot -side bottom -fill both

message .dlg.top.msg -width 3i -text "Target is not specified!" -font -Adobe-Times-Bold-R-Normal-*-180-*
pack .dlg.top.msg -side right -expand 1 -fill both -padx 3m -pady 3m

button .dlg.bot.bt -width 11 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -text Return -relief raised  -bd 7 -command {
destroy .dlg
#focus $oldFocus
}
pack .dlg.bot.bt -side left -expand 1   
} else {
set comment [string trimright [.f17.text get 1.0 120.0]]
regsub -all {'} $comment {''} comment
writefile
destroy .
}
}

pack .buttons.previous -side left -expand 1
#pack .buttons.enter -side left -expand 1
#pack .buttons.next -side left -expand 1
pack .buttons.ptext -side left -expand 1
pack .buttons.new -side left -expand 1
pack .buttons.psheet -side left -expand 1
pack .buttons.done -side left -expand 1
pack .buttons.cancel -side left -expand 1


    frame .f13 -bd 2 -bg DeepSkyBlue4
    label .f13.entry1 -width 31 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -relief sunken  -bd 3 -textvariable date
    label .f13.label1 -relief raised  -bd 4 -fg black -bg azure3 -width 13 -text "Date" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f13.label2 -fg yellow -bg DeepSkyBlue4 -width 12 -text "* CLAS *" -font -Adobe-Times-Bold-R-Normal-*-180-* 
    label .f13.label3 -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text " " 
    pack .f13.label1 -side left -padx 0m
    pack .f13.entry1 -side left -padx 1m 
    pack .f13.label3 -side left -padx 4m
    pack .f13.label2 -side left -padx 0m

set m .f1.label1.rtype
set mb .f1.label1.rtype.beam
set mc .f1.label1.rtype.calib
set hvh .f1.label2.hv
set hvt .f1.label2.hv.trip

    frame .f1 -bd 2 -bg DeepSkyBlue4
    label .f1.entry1  -relief sunken  -bd 3 -width 28  -fg black -bg azure3 -textvariable runtype -font -Adobe-Times-Bold-R-Normal-*-140-*
    menubutton .f1.label1 -relief raised  -bd 4 -fg black -bg yellow2 -font -Adobe-Times-Bold-R-Normal-*-140-* -width 12 -text "Run Type" -menu $m
    label .f1.entry2 -width 7 -fg black -bg azure3 -relief sunken -font -Adobe-Times-Bold-R-Normal-*-140-* -bd 3 -textvariable runnum
    label .f1.label2 -relief raised  -bd 4 -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* -width 12 -text "Run Number " 

    label .f1.label3 -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 3 -text " " 

    pack .f1.label1 -side left -padx 0m
    pack .f1.entry1 -side left -padx 1m
    pack .f1.label3 -side left -padx 2m
    pack .f1.label2 -side left -padx 1m
    pack .f1.entry2 -side left -padx 0m 


menu $m  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*
$m add cascade -label "Beam" -menu $mb 
$m add separator
$m add cascade -label "Calib" -menu $mc
$m add separator
$m add command -label "Cosmic" -command { set runtype cosmic}

menu $mb  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*
$mb add command -label "Photon" -command { set runtype beam_photon}
$mb add separator
$mb add command -label "Electron" -command {set runtype beam_electron }

menu $mc  -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*
$mc add command -label "ADC-Pedestal" -command {set runtype calib_ADC-pedestal }
$mc add separator
$mc add command -label "TDC-1872" -command {set runtype calib_TDC-1872 } 
$mc add separator
$mc add command -label "TOF-laser" -command {set runtype calib_TOF-laser }
$mc add separator
$mc add command -label "Misc" -command {set runtype calib_misc } 


########################## BEAM ENERGY #######################

    frame .f2 -bd 2 -bg DeepSkyBlue4

    label .f2.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 
    label .f2.chap1 -width 5  -bd 4 -fg black -bg azure3 -relief raised  -text "MeV" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f2.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 15 -text "Beam energy" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f2.number1 -width 11 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable tk_b_ener -font -Adobe-Times-Bold-R-Normal-*-140-*

    label .f2.chap2 -width 5  -bd 4 -fg black -bg azure3 -relief raised  -text "A" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f2.label2 -relief raised  -bd 4  -fg black -bg azure3 -width 11 -text "I_Torus" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f2.number2 -width 11 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable tk_t_cur -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f2.label1 -side left -padx 0m
    pack .f2.number1 -side left -padx 1m
    pack .f2.chap1 -side left -padx 0m

    pack .f2.label -side left -padx 2m

    pack .f2.label2 -side left -padx 0m
    pack .f2.number2 -side left -padx 1m
    pack .f2.chap2 -side left -padx 0m

########################################################################

########################## Next line #######################

    frame .f21 -bd 2 -bg DeepSkyBlue4

    label .f21.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 
    label .f21.chap1 -width 5  -bd 4 -fg black -bg azure3 -relief raised  -text "A" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f21.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 15 -text "I_Minitorus" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f21.number1 -width 11 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable tk_mt_cur -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f21.label1 -side left -padx 0m
    pack .f21.number1 -side left -padx 1m
    pack .f21.chap1 -side left -padx 0m


########################## Next line #######################

    frame .f26 -bd 2 -bg DeepSkyBlue4

    label .f26.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 21 -text "Trigger config file" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f26.number1 -width 47 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable trig_conf -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f26.label1 -side left -padx 0m
    pack .f26.number1 -side left -padx 1m

########################################################################
########################## Next line #######################

    frame .f27 -bd 2 -bg DeepSkyBlue4

    label .f27.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 21 -text "Channel config file" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f27.number1 -width 47 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable chan_conf -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f27.label1 -side left -padx 0m
    pack .f27.number1 -side left -padx 1m

########################################################################
########################## Level1 mask #######################

    frame .f28 -bd 2 -bg DeepSkyBlue4

    label .f28.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 21 -text "Level1 mask " -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f28.number1 -width 47 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable l1_mask -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f28.label1 -side left -padx 0m
    pack .f28.number1 -side left -padx 1m

########################################################################
########################## prescale #######################

    frame .f32 -bd 2 -bg DeepSkyBlue4

    label .f32.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 21 -text "Prescale factor" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f32.number1 -width 47 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable prescale -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f32.label1 -side left -padx 0m
    pack .f32.number1 -side left -padx 1m

########################################################################
########################## level1 discr #######################

    frame .f29 -bd 2 -bg DeepSkyBlue4
    label .f29.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 
    label .f29.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 14 -text "L1_thr mV" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f29.label2 -relief raised  -bd 4  -fg black -bg azure3 -width 10 -text "EC_inner" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f29.label3 -relief raised  -bd 4  -fg black -bg azure3 -width 10 -text "EC_outer" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f29.label4 -relief raised  -bd 4  -fg black -bg azure3 -width 10 -text "EC_total" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f29.label5 -relief raised  -bd 4  -fg black -bg azure3 -width 10 -text "SC" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f29.label6 -relief raised  -bd 4  -fg black -bg azure3 -width 10 -text "CC" -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f29.label1 -side left -padx 0m
    pack .f29.label2 -side left -padx 0m
    pack .f29.label3 -side left -padx 0m
    pack .f29.label4 -side left -padx 0m
    pack .f29.label5 -side left -padx 0m
    pack .f29.label6 -side left -padx 0m

########################################################################
########################## Level1 high #######################
    frame .f30 -bd 2 -bg DeepSkyBlue4
    label .f30.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 
    label .f30.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 14 -text "High" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f30.number1 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_inner_h -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f30.number2 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_outer_h -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f30.number3 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_total_h -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f30.number4 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable sc_h -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f30.number5 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable cc_h -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f30.label1 -side left -padx 0m
#    pack .f30.label -side left -padx 2m
    pack .f30.number1 -side left -padx 0m
    pack .f30.number2 -side left -padx 0m
    pack .f30.number3 -side left -padx 0m
    pack .f30.number4 -side left -padx 0m
    pack .f30.number5 -side left -padx 0m

########################################################################
########################## Level1 lo #######################

    frame .f31 -bd 2 -bg DeepSkyBlue4
    label .f31.label -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 2 -text "  " 
    label .f31.label1 -relief raised  -bd 4  -fg black -bg azure3 -width 14 -text "Lo" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f31.number1 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_inner_l -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f31.number2 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_outer_l -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f31.number3 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable ec_total_l -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f31.number4 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable sc_l -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f31.number5 -width 10 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable cc_l -font -Adobe-Times-Bold-R-Normal-*-140-*


    pack .f31.label1 -side left -padx 0m
#    pack .f31.label -side left -padx 2m
    pack .f31.number1 -side left -padx 0m
    pack .f31.number2 -side left -padx 0m
    pack .f31.number3 -side left -padx 0m
    pack .f31.number4 -side left -padx 0m
    pack .f31.number5 -side left -padx 0m

########################################################################


    frame .f3 -bd 2 -bg DeepSkyBlue4
    label .f3.entry -width 5 -fg black -bg azure3 -relief sunken  -bd 3 -textvariable beamcurrent -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f3.label2 -width 5  -bd 4 -fg black -bg azure3 -relief raised  -text "nA" -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f3.label -relief raised  -bd 4  -fg black -bg yellow2 -width 13 -text " I_beam request" -font -Adobe-Times-Bold-R-Normal-*-140-*  
scale .f3.entry2 -orient horizontal -length 8c -from 0 -to 1000 -bg DeepSkyBlue4 -fg DeepSkyBlue4 -command { LL }
    pack .f3.label -side left -padx 0m
    pack .f3.entry -side left -padx 1m
    pack .f3.label2 -side left -padx 1m
    pack .f3.entry2 -side left -padx 0m 


set t .f4.label1.target
set t4he .f4.label1.target.4he
set t3he .f4.label1.target.3he
set th2 .f4.label1.target.h2
set td2 .f4.label1.target.4d2
set tweel .f4.label1.target.wheel
set r .f4.label2.raster

    frame .f4 -bd 2 -bg DeepSkyBlue4
    label .f4.entry1 -width 21 -fg black -relief sunken  -bd 3 -bg azure3 -textvariable target -font -Adobe-Times-Bold-R-Normal-*-140-*
    menubutton .f4.label1 -relief raised  -bd 4 -fg black -bg yellow2  -width 12 -text " Target"  -menu $t -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f4.entry2 -width 11 -fg black -relief sunken  -bd 3 -bg azure3 -textvariable raster -font -Adobe-Times-Bold-R-Normal-*-140-* 
    menubutton .f4.label2 -relief raised  -bd 4 -fg black -bg yellow2  -width 12 -text " Mini raster" -menu $r -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f4.label3 -relief flat  -bd 0 -fg black -bg DeepSkyBlue4  -width 4 -text " " 

    pack .f4.label1 -side left -padx 0m
    pack .f4.entry1 -side left -padx 1m
    pack .f4.label3 -side left -padx 2m
    pack .f4.label2 -side left -padx 0m
    pack .f4.entry2 -side left -padx 1m 


menu $t -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*
$t add command -label "4He" -command { set target 4He_full}
$t add separator
$t add command -label "3He" -command { set target 3He_full}
$t add separator
$t add command -label "H2" -command { set target H2_full}
$t add separator
$t add command -label "D2" -command { set target D2_full}
$t add separator
$t add cascade -label "Wheel" -menu $tweel
$t add separator
$t add command -label "Empty"  -command { set target empty}
$t add separator
$t add command -label "Other"  -command { set target other}

menu $tweel -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-* 
$tweel add command -label "Position 1" -command {set target wheel_1}
$tweel add separator
$tweel add command -label "Position 2" -command {set target wheel_2}
$tweel add separator
$tweel add command -label "Position 3" -command {set target wheel_3}
$tweel add separator
$tweel add command -label "Position 4" -command {set target wheel_4}
$tweel add separator
$tweel add command -label "Position 5" -command {set target wheel_5}
$tweel add separator
$tweel add command -label "Position 6" -command {set target wheel_6}
$tweel add separator
$tweel add command -label "Position 7" -command {set target wheel_7}
$tweel add separator
$tweel add command -label "Position 8" -command {set target wheel_8}
$tweel add separator
$tweel add command -label "Position 9" -command {set target wheel_9}
$tweel add separator
$tweel add command -label "Position 10" -command {set target wheel_10}
$tweel add separator
$tweel add command -label "Position 11" -command {set target wheel_11}
$tweel add separator
$tweel add command -label "Position 12" -command {set target wheel_12}

menu $r -tearoff false -fg black -bg azure3 -font -Adobe-Times-Bold-R-Normal-*-140-*
$r add command -label "On" -command { set raster ON} 
$r add separator 
$r add command -label "Off" -command {set raster OFF}

    frame .f14 -bd 2 -bg DeepSkyBlue4
    entry .f14.entry -width 55 -fg black -bg azure3 -textvariable shiftcrue -font -Adobe-Times-Bold-R-Normal-*-140-*
    label .f14.label -relief raised  -bd 4 -fg black -bg yellow2 -width 13 -text " Shift oper" -font -Adobe-Times-Bold-R-Normal-*-140-* 
    pack .f14.label -side left
    pack .f14.entry -side left -padx 1m 

    frame .f15 -bd 2  -bg DeepSkyBlue4 
    scale .f15.entry1 -orient horizontal -length 3c -from 0 -to 10 -bg DeepSkyBlue4 -command { KK }
    label .f15.label1 -relief raised  -bd 4 -fg black -bg yellow2 -width 13 -text " Log book" -font -Adobe-Times-Bold-R-Normal-*-140-* 
    scale .f15.entry2 -orient horizontal -length 3c -from 0 -to 300 -bg DeepSkyBlue4 -command { JJ } 
    label .f15.label2 -relief raised  -bd 4 -fg black -bg yellow2 -width 13 -text " Page" -font -Adobe-Times-Bold-R-Normal-*-140-*
    pack .f15.label1 -side left  
    pack .f15.entry1 -side left -padx 3m 
    pack .f15.label2 -side left -padx 3m 
    pack .f15.entry2 -side left -padx 3m 

    frame .f16 -bd 2 -bg DeepSkyBlue4
    label .f16.label -relief raised  -fg black -bg yellow2 -bd 4 -width 13 -text " Comments" -font -Adobe-Times-Bold-R-Normal-*-140-*
    pack .f16.label -side left

    frame .f17 -bd 2 -bg DeepSkyBlue4
    text .f17.text -relief sunken -bd 2 -fg black -bg azure3 -yscrollcommand ".f17.scroll set" -setgrid 1 -height 5 -width 65 -font -Adobe-Times-Bold-R-Normal-*-140-*
scrollbar .f17.scroll -bg azure3 -command ".f17.text yview"
pack .f17.scroll -side right -fill y
pack .f17.text 

#----------- packing all ----------------------------
pack  .f13 .f1 .f3 .f4 .f2 .f21 .f26 .f27 .f28 .f32 .f29 .f30 .f31 .f14 .f15 .f16 .f17    -side top -fill x


#-----------------------------reading the thresholds------------------------------------
#-------------------------------reading ec level1---------------------------------------
D 4
catch { exec dimanc mon p ec -1 -1 } result
E 4
I 4
catch { exec dimanc mon p ec } result
E 4
set f [open $monpath/mon_ec_p.txt r]

D 5
for { set i 1} { $i<=6} {incr i 1}  { gets $f line } 
E 5
I 5
#---  skip 6 lines  ----------------------------------------------
for { set i 1} { $i<=7} {incr i 1}  { gets $f line } 
E 5
set ecpin_sec1 [lindex $line 0]
set ecpin_sec2 [lindex $line 1]
set ecpin_sec3 [lindex $line 2]
set ecpin_sec4 [lindex $line 3]
set ecpin_sec5 [lindex $line 4]
set ecpin_sec6 [lindex $line 5]
set ec_inner_h [expr ($ecpin_sec1+$ecpin_sec2+$ecpin_sec3+$ecpin_sec4+$ecpin_sec5+$ecpin_sec6)/6]
gets $f line
set ecpout_sec1 [lindex $line 0]
set ecpout_sec2 [lindex $line 1]
set ecpout_sec3 [lindex $line 2]
set ecpout_sec4 [lindex $line 3]
set ecpout_sec5 [lindex $line 4]
set ecpout_sec6 [lindex $line 5]
set ec_outer_h [expr ($ecpout_sec1+$ecpout_sec2+$ecpout_sec3+$ecpout_sec4+$ecpout_sec5+$ecpout_sec6)/6]
gets $f line
set ecptot_sec1 [lindex $line 0]
set ecptot_sec2 [lindex $line 1]
set ecptot_sec3 [lindex $line 2]
set ecptot_sec4 [lindex $line 3]
set ecptot_sec5 [lindex $line 4]
set ecptot_sec6 [lindex $line 5]
set ec_total_h [expr ($ecptot_sec1+$ecptot_sec2+$ecptot_sec3+$ecptot_sec4+$ecptot_sec5+$ecptot_sec6)/6]
gets $f line
set ecpinlo_sec1 [lindex $line 0]
set ecpinlo_sec2 [lindex $line 1]
set ecpinlo_sec3 [lindex $line 2]
set ecpinlo_sec4 [lindex $line 3]
set ecpinlo_sec5 [lindex $line 4]
set ecpinlo_sec6 [lindex $line 5]
set ec_inner_l [expr ($ecpinlo_sec1+$ecpinlo_sec2+$ecpinlo_sec3+$ecpinlo_sec4+$ecpinlo_sec5+$ecpinlo_sec6)/6]
gets $f line
set ecpoutlo_sec1 [lindex $line 0]
set ecpoutlo_sec2 [lindex $line 1]
set ecpoutlo_sec3 [lindex $line 2]
set ecpoutlo_sec4 [lindex $line 3]
set ecpoutlo_sec5 [lindex $line 4]
set ecpoutlo_sec6 [lindex $line 5]
set ec_outer_l [expr ($ecpoutlo_sec1+$ecpoutlo_sec2+$ecpoutlo_sec3+$ecpoutlo_sec4+$ecpoutlo_sec5+$ecpoutlo_sec6)/6]
gets $f line
set ecptotlo_sec1 [lindex $line 0]
set ecptotlo_sec2 [lindex $line 1]
set ecptotlo_sec3 [lindex $line 2]
set ecptotlo_sec4 [lindex $line 3]
set ecptotlo_sec5 [lindex $line 4]
set ecptotlo_sec6 [lindex $line 5]
set ec_total_l [expr ($ecptotlo_sec1+$ecptotlo_sec2+$ecptotlo_sec3+$ecptotlo_sec4+$ecptotlo_sec5+$ecptotlo_sec6)/6]
close $f
#--------------------------------------------------------------------------------------
#-----------------------------reading cc level1----------------------------------------
D 4
catch { exec dimanc mon p cc -1 -1 } result
E 4
I 4
catch { exec dimanc mon p cc } result
E 4
set f [open $monpath/mon_cc_p.txt r]

D 5
for { set i 1} { $i<=4} {incr i 1}  { gets $f line } 
E 5
I 5
#---  skip 5 lines  ----------------------------------------------
for { set i 1} { $i<=6} {incr i 1}  { gets $f line } 
E 5
set cctivhigh1 [lindex $line 0]
set cctivlo1 [lindex $line 1]
set cctivhigh2 [lindex $line 2]
set cctivlo2 [lindex $line 3]
set thr1 $cctivhigh1
set thr2 $cctivlo1
set thr11 $cctivhigh2
set thr22 $cctivlo2
set cc_h [ expr ($cctivhigh1+$cctivhigh2)/2]
set cc_l [ expr ($cctivlo1+$cctivlo2)/2]
close $f
#---------------------------------------------------------------------------------------
#-----------------------------reading sc level1-----------------------------------------
D 4
catch { exec dimanc mon p sc -1 -1 } result
E 4
I 4
catch { exec dimanc mon p sc } result
E 4
set f [open $monpath/mon_sc_p.txt r]

D 5
for { set i 1} { $i<=3} {incr i 1}  { gets $f line } 
E 5
I 5
#---  skip 5 lines  ----------------------------------------------
for { set i 1} { $i<=5} {incr i 1}  { gets $f line } 
E 5
set i 0
set scthmean 0
set scwidmean 0 
    while {[gets $f line ] >= 0 } {
incr i
set scthr_pg($i) [lindex $line 0]
set scthmean [expr $scthmean+$scthr_pg($i)]
}
set sc_h [expr $scthmean/$i]
set sc_l -
close $f
#---------------------------------------------------------------------------------------
set new_ec_inner_h $ec_inner_h
set new_ec_outer_h $ec_outer_h
set new_ec_total_h $ec_total_h
set new_sc_h $sc_h
set new_cc_h $cc_h
set new_ec_inner_l $ec_inner_l
set new_ec_outer_l $ec_outer_l
set new_ec_total_l $ec_total_l
set new_sc_l $sc_l
set new_cc_l $cc_l


#focus .f1.entry



proc writefile { } {
global archivfile arcomment aropper texto j
global date runtype runnum beamcurrent target raster tk_b_ener tk_t_cur tk_mt_cur trig_conf chan_conf ec_inner_h ec_outer_h ec_total_h sc_h cc_h ec_inner_l ec_outer_l ec_total_l sc_l cc_l shiftcrue logbook page comment beamcr_scale logbook_scale page_scale
global env clonparms clonwsrc

if { $runtype == " "} { set runtype *}
if { $raster == " "} { set raster *}
if { $logbook == " "} { set logbook 0}
if { $page == " "} { set page 0}

puts  "$date $runtype $runnum $beamcurrent $target $raster $tk_b_ener $tk_t_cur $tk_mt_cur $trig_conf $chan_conf $ec_inner_h $ec_outer_h $ec_total_h $sc_h $cc_h $ec_inner_l $ec_outer_l $ec_total_l $sc_l $cc_l $logbook $page " 

set f [open $archivfile w+]
puts $f " $runtype $beamcurrent $target $raster $logbook $page "

close $f

set ff [open $aropper w+]
puts $ff "$shiftcrue "
close $ff

set ff [open $arcomment w+]
puts $f  $comment 
close $f

printsheet

}


proc readfile  { } {
global archivfile arcomment aropper texto j
global date runtype runnum beamcurrent target raster tk_b_ener tk_t_cur tk_mt_cur trig_conf chan_conf ec_inner_h ec_outer_h ec_total_h sc_h cc_h ec_inner_l ec_outer_l ec_total_l sc_l cc_l shiftcrue logbook page comment beamcr_scale logbook_scale page_scale
global env clonparms clonwsrc

#puts "SIMON $j"
set f [open $archivfile r]
gets $f line
set runtype [lindex $line 0] 
set beamcurrent [lindex $line 1]
#puts $beamcurrent
$beamcr_scale set [expr $beamcurrent*10.0]
set target [lindex $line 2]
set raster [lindex $line 3]
set logbook [lindex $line 4]
$logbook_scale set $logbook
set page [lindex $line 5]
$page_scale set $page
close $f


set f [open $aropper r]
gets $f line
set  shiftcrue $line
close $f

}

proc printsheet { } {

global date runtype runnum beamcurrent target raster tk_b_ener tk_t_cur tk_mt_cur trig_conf chan_conf ec_inner_h  ec_outer_h ec_total_h  cc_h ec_inner_l ec_outer_l ec_total_l sc_h sc_l cc_l shiftcrue logbook page comment beamcr_scale logbook_scale page_scale l1_mask prescale comment printfile env
global env clonparms clonwsrc

set rootdir $env(CLON_PARMS)
if {[catch {set env(DD_NAME)}]==1} {
    set session "clasprod"
} else {
    set session $env(DD_NAME)
}
set exten [format "%06d" $runnum]
set f [open ${rootdir}/run_log/archive/comment_${session}_${exten}.txt w]

puts $f "#  generated by run_log_comment on [exec date]" 
puts $f ""
puts $f "*DATE* - $date "
puts $f ""
puts $f "*RUN_TYPE* - $runtype"
puts $f ""
puts $f "*RUN_NUMBER* - $runnum" 
puts $f ""
puts $f "*BEAM_REQUEST* - $beamcurrent"                          
puts $f ""
puts $f "*TARGET* - $target"
puts $f ""
puts $f "*MINI_RASTER* - $raster"
puts $f ""
puts $f "*BEAM_ENERGY* - $tk_b_ener Mev"
puts $f ""      
puts $f "*TORUS_CURRENT* - $tk_t_cur A"
puts $f ""
puts $f "*MINI_CURRENT* - $tk_mt_cur    "
puts $f ""
set a [split $trig_conf /]
set b [split $chan_conf /]
set p [lindex $a 7]
set c [lindex $b 7]
puts $f "*TRIG_CONFIG* - $p"
puts $f ""
puts $f "*CHANNEL_CONFIG* - $c"
puts $f ""
puts $f "*L1_MASK* - $l1_mask"
puts $f ""
puts $f "*PRESCALE* - $prescale"
puts $f ""
puts $f "*L1 high thresholds in Mv*  "
puts $f "*EC_INNER_HI*  -   $ec_inner_h  "
puts $f "*EC_OUTER_HI*  -   $ec_outer_h "
puts $f "*EC_TOTAL_HI*  -   $ec_total_h  "
puts $f "*SC_HI*        -   $sc_h  "
puts $f "*CC_HI*        -   $cc_h "
puts $f ""
puts $f "*L1 lo thresholds in Mv*  "
puts $f "*EC_INNER_LO*  -   $ec_inner_l  "
puts $f "*EC_OUTER_LO*  -   $ec_outer_l "
puts $f "*EC_TOTAL_LO*  -   $ec_total_l  "
puts $f "*SC_LO*        -   $sc_l  "
puts $f "*CC_LO*        -   $cc_l "
puts $f ""
puts $f "*OPERATORS* - $shiftcrue"
puts $f ""
puts $f "*LOG_BOOK* - $logbook "
puts $f ""
puts $f "*LOG_PAGE* - $page"
puts $f ""
puts $f "*IGNORE*"
puts $f "N"
puts $f ""
puts $f "*COMMENT*"
puts $f ""
puts $f $comment

close $f

catch { exec lp -dclonhp ${rootdir}/run_log/archive/comment_${session}_${exten}.txt } result 

}







E 1
