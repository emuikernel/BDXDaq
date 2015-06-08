#!/usr/bin/csh

#  run_config_end.csh

#  Coda script to perform config tasks

#  E.Wolin, 3-oct-97


#  do config tasks

#  smartsockets broadcast
rc_transition -a clasprod -d run_control -file    config  >& /dev/null &

#  done
exit(0)
