#!../../bin/linux-x86_64/LVMPOD

## You may have to change LVMPOD to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/LVMPOD.dbd"
LVMPOD_registerRecordDeviceDriver pdbbase

# Initialize the MPOD driver
# First argument is the IP name 
# Second argument is the asyn port name
# Third argument is the number of slot for the chassis
drvMPODConfig( "halldmpod3", "halldmpod3", 10 )
#drvMPODConfig( "halldmpod3", "junkmpod", 10 )
#drvMPODConfig( "halldmpod1", "halldmpod1", 10 )

dbLoadRecordsMPOD()

cd ${TOP}/iocBoot/${IOC}
iocInit

#
# Initialize the SNMP driver
#
epicsSnmpInit()

