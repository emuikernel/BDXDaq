#!/usr/bin/csh

#  run_download_eb.csh
#  Coda script to perform download tasks
#     (use it if there is no ER in configuration)



# Force run configuration file selection
select_trigger -mode download

# use select_trigger exit status
exit($status)
