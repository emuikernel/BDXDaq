#!/usr/bin/csh

#  run_download_end.csh

#  Coda script to perform download tasks

#  E.Wolin, 3-oct-97


#  do download tasks

#  smartsockets broadcast
rc_transition -a clasprod -d run_control -file    download  >& /dev/null &


exit(0)
