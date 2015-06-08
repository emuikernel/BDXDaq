h54644
s 00051/00000/00000
d D 1.1 06/07/01 09:29:16 boiarino 1 0
c date and time created 06/07/01 09:29:16 by boiarino
e
u
U
f e 0
t
T
I 1
# @(#)Login 1.14 90/11/01 SMI

#  .login for clasrun account

#
#  This file is maintained in the CLAS code management system
#  All changes made must be done through the CMS, otherwise
#   the changes may be lost!
#
#

#  general terminal characteristics...ejw
stty erase "^H" kill "^U" intr "^C" eof "^D" susp "^Z" hupcl ixon ixoff tostop tabs
setenv TERM xterm
set term=vt100

#  aliases...ejw
source $CLON/common/scripts/clasrun.alias

# tcsh completions from tcsh ftp site (make, rlogin, etc.)...ejw
source $CLON/common/scripts/complete.tcsh

#
# epics for clon01 and clon02 only
#

if (($HOST == clon01)||($HOST == clon02)) then

  alias setup_epics source /home/epics/R3.13.4/epicsB/prod/scripts/setup_epics
  setup_epics

  alias scaler_mcc "setenv DISPLAY op08.acc.jlab.org:0 ; " \
        "medm -x -cleanup $APP/scaler/medm/scaler_g.adl >& /dev/null &"
  alias scaler_mcc1 "setenv DISPLAY op08.acc.jlab.org:0 ; " \
        "medm -x -cleanup $APP/scaler/medm/frwd_scaler.adl >& /dev/null &"
  alias pcscan_mcc "setenv DISPLAY op08.acc.jlab.org:0 ; " \
        "/home/epics/medm/medm_solaris_2.2.5 -x -cleanup $APP/pc_motor_x/medm/pc_position.adl >& /dev/null &"
  alias tomcc1 "scaler_mcc1"
  alias tomcc "scaler_mcc"

endif

#
echo "-"
echo "- .login has been executed -"
echo "-"
echo "- Welcome to the CLAS online run account on "`date`
echo "-"
echo "-"
#

E 1
