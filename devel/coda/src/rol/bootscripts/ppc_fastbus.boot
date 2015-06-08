#
# ppc_fastbus.boot - generic boot script for all FASTBUS crate controllers
#

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




##loading Fastbus libs
#cd "$CODA_VXWORKSPPC/lib"
#ld < libsfifb.o

##loading fastbus utilities
#cd "$CLON_VXWORKS/code"
#ld < fbutil_ppc.o
#ld < adc1881_ppc.o
#ld < test_1872.o
#ld < test_1877.o
#ld < test_1877S.o
#ld < test_1881.o
#ld < fb_diag.o


ladr = 0
sysBusToLocalAdrs(0x39,0xe00000,&ladr)
fb_init_1(ladr)





###
### following part for parallel readout and dual cpu
###

### Load Interrupt and mempart libraries
cd "$CODA/VXWORKS_ppc/obj"
ld < sfiIntLib.o
ld < dmaPLib.o
dmaPartInit()

### Load User readout routines
ld < sfiUserLib.o
sfiUserInit()


taskDelay(sysClkRateGet()*10)





