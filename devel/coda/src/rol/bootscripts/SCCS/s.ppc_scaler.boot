h51747
s 00030/00000/00000
d D 1.1 06/05/26 10:30:40 boiarino 1 0
c date and time created 06/05/26 10:30:40 by boiarino
e
u
U
f e 0
t
T
I 1

# ppc_scaler.boot - generic boot script for all VME crate controllers

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


taskDelay(sysClkRateGet()*5)


E 1
