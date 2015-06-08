#!/usr/bin/csh

#  run_boot_end.csh

#  Coda script to perform boot tasks

#  E.Wolin, 3-oct-97


#  do boot tasks

#  smartsockets broadcast
rc_transition -a clasprod -d run_control -file    boot  >& /dev/null &

#  done
exit(0)

