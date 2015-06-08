#!../../bin/linux-x86/A6551

## You may have to change A6551 to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/A6551.dbd"
A6551_registerRecordDeviceDriver pdbbase

## Configure devices
drvAsynIPPortConfigure("L0",129.57.86.39:1234,0,0,0)

## Load record instances
## Call one for each with sector, layer and GPIB ID
dbLoadRecords("/home/kliv/epics/drivers/A6551App/iocBoot/iocA6551App/A6551.db","S=1,L=1,G=5,PORT=L0,ADDR=24,IMAX=2000,OMAX=2000")

cd ${TOP}/iocBoot/${IOC}
iocInit();

