#!/usr/bin/csh

#  run_download_er.csh

#  Coda script to perform download tasks

#  E.Wolin, V.Gyurjyan, 15-apr-02


# Force run configuration file selection
select_trigger -mode download

# use select_trigger exit status
exit($status)
