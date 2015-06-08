# ppc_vme.boot - generic boot script for all VME crate controllers

#  standard CLAS libraries
cd "$CODA/VXWORKS_ppc/obj"
ld < all_rocs.o

#set env variables
cd "$CODA/VXWORKS_ppc/bootscripts"
< vxbootenv.boot

# load CLAS translation library
cd "$CODA/VXWORKS_ppc/lib"
ld < libcodatt.so

# load coda_roc
cd "$CODA/VXWORKS_ppc/bin"
ld < coda_roc
ld < tcpServer.o
ld < blaster.o
ld < blastee.o

# load libraries used by first readout list
cd "$CODA/VXWORKS_ppc/lib"
ld < librol.so





###
### following part for parallel readout and dual cpu
###

### Load Interrupt and mempart libraries
cd "$CODA/VXWORKS_ppc/obj"
ld < vmeIntLib.o
ld < dmaPLib.o
dmaPartInit()

### Load User readout routines
ld < vmeUserLib.o
vmeUserInit()




taskDelay(sysClkRateGet()*5)





