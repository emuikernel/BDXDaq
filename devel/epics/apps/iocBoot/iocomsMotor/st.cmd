## Example vxWorks startup file

## The following is needed if your board support package doesn't at boot time
## automatically cd to the directory containing its startup script
cd "/usr/clas12/release/0.1/epics/apps/iocBoot/iocomsMotor"
#cd ${TOP}/iocBoot/${IOC}

< cdCommands
#< ../nfsCommands

cd topbin

## You may have to change omsMotor to something else
## everywhere it appears in this file
ld 0,0, "omsMotorVx.munch"

## Register all support components
cd top
dbLoadDatabase "dbd/omsMotorVx.dbd"
omsMotorVx_registerRecordDeviceDriver pdbbase
#
#
# OMS stepper motor driver
#
recMotordebug = 11
devOMSdebug   = 11
drvOMSdebug   = 11
#
# OMS VME driver setup parameters: 
#     (1)cards, (2)base address(short, 16-byte boundary), 
#     (3)interrupt vector (0=disable or  64 - 255), (4)interrupt level (1 - 6),
#     (5)motor task polling rate (min=1Hz,max=60Hz)
omsSetup(1, 0x8000, 180, 5, 10)
#omsSetup(1, 0x2000, 180, 5, 10)

## Load record instances
#cd "${TOP}/iocBoot/${IOC}"
dbLoadTemplate "db/omsMotor.substitutions"
#dbLoadRecords "db/omsMotor.db", "user=hovanes"

#cd dbLoadRecords("db/motor.db","motor_name=hps, card=0, slot=3, srev=2000, urev=0.2, direction=Pos, velo=0.2, accl=0.5")

cd startup
iocInit

## Start any sequence programs
#seq &sncxxx, "user=hovanes"
