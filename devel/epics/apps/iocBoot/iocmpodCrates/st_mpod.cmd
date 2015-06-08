
< envPaths

epicsEnvSet("WO", "WIENER-CRATE-MIB::output")
epicsEnvSet("MIB","WIENER-CRATE-MIB::")

cd ${TOP}

## Register all support components
#dbLoadDatabase ("dbd/mpodCrates.dbd")
#mpodCrates_registerRecordDeviceDriver(pdbbase)

dbLoadDatabase ("dbd/mpodCrates.dbd")
mpodCrates_registerRecordDeviceDriver(pdbbase)

devSnmpSetDebug( 0 )

# Incase of RHEL6.4 there might be problem with net-snmp
# The following line may need to be uncommented, but will run slow
#devSnmpSetDebug( 6 )
#devSnmpSetMaxOidsPerReq("halldmpod1", 1)
#devSnmpSetSnmpVersion("halldmpod1","SNMP_VERSION_2c")
#devSnmpSetParam(ReadStarvationMSec, 50000)
#devSnmpSetParam(PassivePollMSec, 5000)

# Load record instances
#dbLoadRecords ( "db/caSecurity.db" )

#dbLoadRecords ( "db/iocStatus.db",  "IOC=${IOC}" )

# Load VME control related record instances
#epicsEnvSet("SCAN","1 second")
#dbLoadRecords("db/mpod_chassis.db","HOST=129.57.36.152,COMMUNITY=guru,SCAN=${SCAN},CR=1")
#dbLoadRecords("db/mpv_8008l.db","HOST=129.57.36.152,COMMUNITY=guru,SCAN=${SCAN},CR=1,CH=1")

dbLoadTemplate "db/mpod_chassis.substitutions"

####dbLoadRecords("db/test.db","HOST=hvsvt1.jlab.org")


#asSetFilename("ca_security.txt")

## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd ${TOP}/iocBoot/ioc${IOC}
iocInit
#epicsSnmpInit()

dbl

## Start any sequence programs
#seq sncExample, "user=hovanes"


