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
dbLoadTemplate "db/v450.substitutions"
dbLoadRecords "db/v450.db", "card=0,signal=0"
dbLoadRecords "db/v450.db", "card=0,signal=1"
dbLoadRecords "db/v450.db", "card=0,signal=2"
dbLoadRecords "db/v450.db", "card=0,signal=3"
dbLoadRecords "db/v450.db", "card=0,signal=4"
dbLoadRecords "db/v450.db", "card=0,signal=5"
dbLoadRecords "db/v450.db", "card=0,signal=6"
dbLoadRecords "db/v450.db", "card=0,signal=7"
dbLoadRecords "db/v450.db", "card=0,signal=8"
dbLoadRecords "db/v450.db", "card=0,signal=9"
dbLoadRecords "db/v450.db", "card=0,signal=10"
dbLoadRecords "db/v450.db", "card=0,signal=11"
dbLoadRecords "db/v450.db", "card=0,signal=13"
dbLoadRecords "db/v450.db", "card=0,signal=14"
dbLoadRecords "db/v450.db", "card=0,signal=15"

dbLoadRecords "db/v450.db", "card=0,signal=16" 
#dbLoadRecords "db/v450.db", "card=0,signal=17"
#dbLoadRecords "db/v450.db", "card=0,signal=18"
#dbLoadRecords "db/v450.db", "card=0,signal=19"
#dbLoadRecords "db/v450.db", "card=0,signal=20"

cd startup
iocInit

## Start any sequence programs
#seq &sncxxx, "user=bonneau"
