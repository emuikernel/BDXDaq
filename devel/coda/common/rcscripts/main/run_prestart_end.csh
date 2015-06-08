#!/usr/bin/csh

#  run_prestart_end.csh

#  Coda script to perform tasks at end of prestart transition

#  E.Wolin, 28-apr-98


#  get current run number
set runnum=`run_number`



#  read pretrigger and channel discriminator hardware, insert data into database, datastream, files
#  create begin run events, etc.

(echo " "; echo starting: `date`; echo run number: $runnum; echo " "; dimanc mon all all; run_log_begin -a clasprod; get_ts_program; begin_run_event -s clasprod; echo ending: `date`; echo " ";) >>& $CLON_LOG/run_log/run_begin.log &


#  beam energy and currents to map (bor2map is link to electron or photon version)
###donotneedanymore???(echo " "; echo run number: $runnum; echo " "; $CLON_PARMS/run_log/bor2map;) >>& $CLON_LOG/run_log/bor2map.log &

#  done
exit(0)
