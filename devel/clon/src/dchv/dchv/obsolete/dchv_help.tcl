proc mkStyles {{. styles}} {
    catch {destroy .}
#    toplevel .
#    wm title . " DCHV Online Help"
#    wm iconname . "Help"

    button .ok -text OK -command "destroy ."
    text .t -relief raised -bd 2 -yscrollcommand ".s set" -setgrid true \
	    -width 70 -height 28
    scrollbar .s -relief flat -command ".t yview"
    pack .ok -side bottom -fill x
    pack .s -side right -fill y
    pack .t -expand yes -fill both

    # Set up display styles

    .t tag configure bold -font -Adobe-Courier-Bold-O-Normal-*-120-*
    .t tag configure big -font -Adobe-Courier-Bold-R-Normal-*-140-*
    .t tag configure verybig -font -Adobe-Helvetica-Bold-R-Normal-*-240-*
    if {[tk colormodel .] == "color"} {
	.t tag configure color1 -background #eed5b7
	.t tag configure color2 -foreground red
	.t tag configure raised -background #eed5b7 -relief raised \
		-borderwidth 1
	.t tag configure sunken -background #eed5b7 -relief sunken \
		-borderwidth 1
    } else {
	.t tag configure color1 -background black -foreground white
	.t tag configure color2 -background black -foreground white
	.t tag configure raised -background white -relief raised \
		-borderwidth 1
	.t tag configure sunken -background white -relief sunken \
		-borderwidth 1
    }
    .t tag configure bgstipple -background black -borderwidth 0 \
	    -bgstipple gray25
    .t tag configure fgstipple -fgstipple gray50
    .t tag configure underline -underline on
    .t insert 0.0 {\

insertWithTags .t DCHV user manual color1 color2

    insertWithTags .t {Starting DCHV} underline


    insertWithTags .t {1. Login on clonxx as clasrun. 


2. Check if the VME controller is active.


get_roc_transition dccntrl


The response booted is OK and you can skip to step 3. In case of a different response:


telnet dccntrl


reboot


You need to wait , to allow dccntrl to be booted.


get_roc_transition dccntrl 

If you do not get the booted response, check with the DAQ person about the status of
CODA.
Note: Any crash in the CODA system, or a reset of CODA, can make dccntrl hang. Thus,
it is very important to stop any kind of DCHV activity, including monitoring, before a
CODA reset.


3. Start DCHV. Type:


dchv


Recomended to start DCHV on terminal CLASXT35.

Basic Operations

1. To turn the Drift Chambers ON

From the Group menu select Type 0
Use the left mouse button to click on the large oval labelled DC. That oval should turn
green and the words type0_all will appear in green just below the DC oval. 
From the Mode menu select Set:On

2. To turn the Drift Chambers OFF 

From the Group menu select Type 0. For that
repete the same operations listed in Basic Operations point 1, if previous selection was
different from type0_all. 

3. Resetting a tripped channel

If you have previously selected Type 0, deselect the DC oval by clicking on it with the
right mouse button. From the Group menu select Type 3.
A section that contains one or more tripped channels will appear red in the drift
chamber picture. Click with the left mouse button on one of the red areas, to select that
group of channels.Turn the selected channels on by selecting Set:On from the Mode
menu.
If all of the channels in that group turn on successfully the group will turn blue.


4. Monitoring Drift Chamber Voltages and Currents


From the Group menu select Type 0. Use the left mouse button to click on the large oval
labelled DC. That oval should turn green and the words type0_all will appear in green just
below the DC. From the Mode menu select Monitor:Status to see plots of voltage and
current as a function of channel. 


5. Putting the Drift Chambers in Standby Mode


The voltages on the Drift Chambers can be lowered to a preset standby level. This may
be useful when minor beam tuning is necessary. 
From the Group menu select Type 0. Use the left mouse button to click on the large oval
labelled DC. That oval should turn green and the words type0_all will appear in green
just below the DC. If the type0_all was selected previously, it is not necessary to select it
again. From the Mode menu select Set:Stand By. The command Mode:Set:Restore returns
the voltages to their nominal values. 


General Operations


All operations described below are performed on a selected group of channels. To select
the desired group of channels:
Choose the Type under the Group menu depending on the size of the group you want to
monitor or set. Note that Types 2 and 4 use channel mode in which hardware operations
are performed for each channel separately. Setting and monitoring operations will take
longer for these Types.
With the left mouse button select the desired group on the drift chamber picture. If you
have successfully selected a group of channels, the name of that group will appear in
green at the lower right corner of the drift chamber group picture.
Note that in order to change your group selection you must first deselect a previous
selection by clicking on it with the right mouse button. 

1. Monitor the status of a group of channels.


From the Mode menu select Monitor. You will have several submenu choices:


Status 

This command displays the actual voltage and current for each channel of the selected
group both graphically and in a table. The voltage as a function of channel appears in
the left-most plot. The current as a function of channel appears in the middle plot. The
sum of the currents in 3 regions as a function of time appears in the right-most plot. To
reset the time plot use the Reset button at the top right of the screen. The Print button
will print a summary of the current sums and the mean currents in the different wires on
clonhp printer. 
The numerical fields will show the mean voltages and mean currents for FA, FS, SA, SS,
GA, and GS of the selected detector channel group. At the bottom is a list of the actual
voltage (Vmon) and current (Imon) for each channel.

V0/I0 

Lists the voltage and current parameter settings for the selected group of channels. For
each channel V0 is the set point voltage (not the actual monitored voltage) and I0 is the
trip level. Note: Voltages are listed in units of volts times 10 and currents are listed in
units of microAmps times 100 (a V0 setting of 750 Volts will be listed as 7500).

V1/I1

These are the standby voltage and current settings. 


Vmax/Trip

Vmax is the maximum allowed voltage parameter setting for each channel. Trip is the
time (in 10 times microsec) for which the current must exceed the trip level in order to
trip that channel.

Rup/Rdwn

Rup is the voltage ramp up speed in volts/sec. Rdwn is the voltage ramp down speed in
volts/sec.


All Parameters

Lists all every parameter setting as a function of channel. Most of the column are self
explanatory, except the NAME column. The five digit names have the following
meanings:
First Digit: Region xx
Second Digit: Sector xx
Third Digit: Theta (1=forward;2=intermediate;3=backward)
Fourth Digit: FA=1; FS=2; SA=3; SS=4; GA=5; GS=6
Fifth Digit: doesn't matter.


2. To change CAEN parameter settings

Under Edit, choose the setting you want to edit, such as V0 or I0.
Enter the desired new settings for each superlayer in the boxes to the right. FA = Field
Axial; SS = Sense Stereo, etc.
When all six boxes are correct, choose Save under the Edit menu.
Save will save the values you entered into a computer file.
To set the saved V0 (or I0) values in hardware, choose Set under the Mode menu and
then Parameter V0 (or I0). The set command loads the saved values into the CAEN
hardware.
Note: In case of grouping mode settings (Type0, Type1 and Type3) the set value is taken
from the FA entry box.


3. To operate in the single channel control mode

Select the desired group. Click the Single menubutton in the Mode menu. It will pop up
the listbox with all channels of that selected group. Double click on the channel will
select that channel for further hardware operations. All following operations will be
addressed to that channel. 

}

    .t mark set insert 0.0
    bind . <Any-Enter> "focus .t"
}
}



# The procedure below inserts text into a given text widget and
# applies one or more tags to that text.  The arguments are:
#
# w		Window in which to insert
# text		Text to insert (it's inserted at the "insert" mark)
# args		One or more tags to apply to text.  If this is empty
#		then all tags are removed from the text.

proc insertWithTags {w text args} {
    set start [. index insert]
    . insert insert $text
    foreach tag [. tag names $start] {
	. tag remove $tag $start insert
    }
    foreach i $args {
	. tag add $i $start insert
    }
}
mkStyles
