h44988
s 00000/00000/00000
d R 1.2 00/09/21 14:33:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/EC_laser_check.csh
e
s 00019/00000/00000
d D 1.1 00/09/21 14:33:30 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
#!/bin/csh -f
#
# schedule by VG or EW
# $CLON_WSOURCE/scripts/s 
# EC_laser_check.csh
#

set output = `$CLON_BIN/EC_laser_status`
set value = $status
echo "value="$value

if ($value == 1) then
 echo "sending message laser on" 
 echo laser is on | mailx -s "laser on" giovankl@jmu.edu ;
else
 echo "laser not on"
endif

exit
E 1
