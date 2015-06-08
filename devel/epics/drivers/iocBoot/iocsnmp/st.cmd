#!../../bin/linux-x86/snmp
## You may have to change snmp to something else
## everywhere it appears in this file
< envPaths
cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/snmp.dbd")
snmp_registerRecordDeviceDriver(pdbbase)

## Load record instances
### my: dbLoadRecords("db/disk.db")
#dbLoadRecords("db/router0.db","HOST=rt-112-16,ID=5")

dbLoadRecords("db/test.db","HOST=129.57.86.248")


## Set this to see messages from mySub
#var mySubDebug 1

cd ${TOP}/iocBoot/${IOC}
iocInit()
#
#
#
epicsSnmpInit()