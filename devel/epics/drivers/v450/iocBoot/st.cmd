## Example vxWorks startup file

## The following is needed if your board support package doesn't at boot time
## automatically cd to the directory containing its startup script
cd "$EPICS/drivers/v450/iocBoot"

< cdCommands
#< ../nfsCommands

cd topbin

## You may have to change v450 to something else
## everywhere it appears in this file
ld 0,0, "v450.munch"

## Register all support components
cd top
dbLoadDatabase "dbd/v450.dbd"
v450_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadTemplate "db/v450_PRB.substitutions"
#dbLoadRecords "db/v450.db", "card=0,signal=0"
#dbLoadRecords "db/v450.db", "card=0,signal=1"
#dbLoadRecords "db/v450.db", "card=0,signal=2"


cd startup
iocInit

## Start any sequence programs
#seq &sncxxx, "user=bonneau"
