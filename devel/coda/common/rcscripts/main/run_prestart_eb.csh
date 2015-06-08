#!/usr/bin/csh

#  run_prestart_eb.csh

#  Coda script to perform prestart tasks

#  E.Wolin, 24-apr-98


#  sync begin run events
echo " "; echo starting: `date`; echo run number: `run_number`; echo " "; 
sync_begin_run_event
echo ending: `date`; echo " "


#  done
exit(0)
