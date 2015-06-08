## Example vxWorks startup file

## The following is needed if your board support package doesn't at boot time
## automatically cd to the directory containing its startup script
cd "$IOC_root_classc4/iocBoot/iocclassc4"

< cdCommands
#< ../nfsCommands
< ../network
#< ../users

cd topbin

## You may have to change classc4 to something else
## everywhere it appears in this file
ld 0,0, "classc4.munch"

## Register all support components
cd top
dbLoadDatabase "dbd/classc4.dbd"
classc4_registerRecordDeviceDriver pdbbase

## Load record instances
##dbLoadTemplate "db/user.substitutions"
##dbLoadRecords "db/dbSubExample.db", "user=levon"

## Set this to see messages from mySub
#mySubDebug = 1

## Run this to trace the stages of iocInit
#traceIocInit

dbLoadRecords("db/sixty_hz_common.db", "FIFO=4096, HALF_FIFO=2048")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=0")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=1")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=2")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=3")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=4")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=5")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=6")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=7")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=8")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=9")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=10")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=11")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=12")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=13")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=14")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=15")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=16")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=17")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=18")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=19")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=20")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=21")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=22")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=23")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=24")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=25")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=26")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=27")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=28")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=29")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=30")
dbLoadRecords("db/sixty_hz_macro.db", "FIFO=4096, HALF_FIFO=2048, CHAN=31")

dbLoadRecords("db/scaler_c.db")
dbLoadRecords("db/frwd_scaler.db")
dbLoadRecords("db/stopper.db")

# Load IOC status records
dbLoadRecords("db/iocAdminVxWorks.db","IOC=classc4")

recScalerdebug=0
devScalerdebug=0
# Joerger VSC setup parameters:
#     (1)cards, (2)base address(ext, 256-byte boundary),
#     (3)interrupt vector (0=disable or  64 - 255)
#mv162
#VSCSetup(2, 0x02000000, 200)
#ppc
VSCSetup(2, 0x0a000000, 200)


cd startup
iocInit

## Start any sequence programs
#seq &sncExample, "user=levon"

seq &sixtyHz
#seq &scaler_frwd_restart
#seq &scaler_c_restart

