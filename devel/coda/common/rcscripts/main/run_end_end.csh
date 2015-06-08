#!/usr/bin/csh

#  run_end_end.csh

#  Coda script runs at end of end transition

#  E.Wolin, 21-apr-98

# set trigger configuration file for end
download_trig_config -mode end -nogui

#  get current run number
set runnum=`run_number`

# start eor comment program
eor_comment_fix clasprod $runnum &

#  fcup scalers to map (ask offline if we need it)
(setenv RUN_NUMBER $runnum; echo " "; echo run number: $runnum; echo " "; eor2map;) >>& $CLON_LOG/run_log/eor2map.log &

# get_eor_scalers updates DB
#  get_eor_scalers,run_log_files,run_log_end must run sequentially
(setenv RUN_NUMBER $runnum; echo " "; echo run number: $runnum; echo " "; echo starting: `date`; echo "eor:"; get_eor_scalers; echo "files:"; run_log_files -a clasprod; echo "end:"; run_log_end -a clasprod -s clasprod; echo ending: `date`;) >>& $CLON_LOG/run_log/run_log_files_end.log &


#  reset run lock in clastrig2
reset_ts_runlock


#  smartsockets broadcast
rc_transition -a clasprod -d run_control -file    end  >& /dev/null &


#  end of run printouts...sleep to allow run to finish ending
(sleep 45; a2ps $CLON_PARMS/scalers/evmon_scaler.sum;) >& /dev/null &

pulser_stop

#  done
exit(0)



