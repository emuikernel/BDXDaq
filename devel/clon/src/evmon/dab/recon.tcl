#!/opt/sfw/bin/expect
spawn /bin/tcsh
expect "clon"
send "source ~/.login\r"
expect "clon"
send "source ~/recsis_online/recon/recon_env\r"
expect "clon"
send "cd /hist/recon\r"
expect "clon"
send "setenv DD_NAME clasprod\r"
expect "clon"
send "/home/clasrun/recsis_online/bin/SunOS/recon_online -t /home/clasrun/recsis_online/recon/clasrun_init\r"
expect "Global memory section name:"
send "go\r"
interact
