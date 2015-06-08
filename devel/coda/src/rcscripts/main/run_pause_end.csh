#!/usr/bin/csh

#  run_pause_end.csh

#  Coda script to perform pause tasks

#  E.Wolin, 3-oct-97


#  do pause tasks

#  smartsockets broadcast
rc_transition -a clasprod -d run_control -file    pause  >& /dev/null &

#  done
exit(0)
