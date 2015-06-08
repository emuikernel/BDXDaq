#!/usr/bin/csh

#  run_prestart_er.csh

#  Coda script to perform ER prestart tasks
#  ER is the first to execute

#  E.Wolin, 28-apr-98
# V. Gyurjyan 06-July-99


#  get script option if it exists
if( ${#argv} < 1 ) then
    set script_option=""
else
    set script_option=$argv[1]
endif


#  get current run number
set runnum=`run_number`


# set trigger configuration file for prestart
download_trig_config -mode prestart


#  display run sheet...transition fails if operator selects "abort"
(echo " "; echo starting: `date`; echo run number: $runnum; echo " ";rlComment) >>& $CLON_LOG/run_log/run_log_comment.log


run_log_comment -a clasprod >>& $CLON_LOG/run_log/run_log_comment.log

set rlc_status = $status
(echo ending `date`; echo " ";) >>& $CLON_LOG/run_log/run_log_comment.log


#  operator selected "ok", do smartsockets broadcast, update rcstate file
#  otherwise cancel run lock
if ( $rlc_status == 0 ) then
  rc_transition -a clasprod -d run_control -file    prestart >& /dev/null &
else
  reset_ts_runlock &
endif

#  done
exit($rlc_status)
exit(0)



