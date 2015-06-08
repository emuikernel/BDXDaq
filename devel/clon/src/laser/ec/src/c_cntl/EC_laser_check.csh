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
