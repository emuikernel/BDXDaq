#
# simple epics boot example
#
# usage:
#   cd "$CODA/VXWORKS_ppc/bootscripts"
#   < epics.boot
#
cd "$CLON/R3.14.8.2/base-3.14.8.2/bin/vxWorks-ppc604_long"
ld < ioctest.munch
###sysAtReboot not found. epicsExit will not be called by reboot
cd "$CLON/R3.14.8.2/base-3.14.8.2"
dbLoadDatabase("dbd/ioctest.dbd",0,0)
ioctest_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("db/dbExample1.db","user=boiarino")
dbLoadRecords("db/dbExample2.db","user=boiarino,no=1,scan=1 second")
dbLoadRecords("db/dbExample2.db","user=boiarino,no=2,scan=2 second")
dbLoadRecords("db/dbExample2.db","user=boiarino,no=3,scan=5 second")
dbLoadRecords("db/dbSubExample.db","user=boiarino")
iocInit()
dbl
