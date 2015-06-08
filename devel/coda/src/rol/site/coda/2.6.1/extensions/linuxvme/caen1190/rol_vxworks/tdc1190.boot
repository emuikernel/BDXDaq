# Boot file for CODA ROC 2.6.1 
# PowerPC version for MV6100 with Tsi148 VME<->PCI bridge

# Add route to outside world (from 29 subnet to 120 subnet)
#mRouteAdd("129.57.120.0","129.57.29.1",0xfffffc00,0,0)
# Add Routes for Multicast Support
mRouteAdd("224.0.0.0","129.57.29.0",0xf0000000,0,0)

# Load host table entries
< /daqfs/home/abbottd/VXKERN/vxhosts.boot

# Setup environment to load coda_roc
putenv "MSQL_TCP_HOST=dafarm28"
putenv "EXPID=DAQDEVEL"
putenv "TCL_LIBRARY=/daqfs/coda/2.6.1/common/lib/tcl7.4"
putenv "ITCL_LIBRARY=/daqfs/coda/2.6.1/common/lib/itcl2.0"
putenv "DP_LIBRARY=/daqfs/coda/2.6.1/common/lib/dp"
putenv "SESSION=daqSession"

# Load Tempe DMA Library
ld< /daqfs/coda/2.6/extensions/tempeDma/usrTempeDma.o
# Setup Address and data modes for transfers
#
#  usrVmeDmaConfig(addrType, dataType, sstMode);
#
#  addrType = 0 (A16)    1 (A24)    2 (A32)
#  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
#  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
#
usrVmeDmaConfig(2,3,2)


# Load cMsg Stuff
cd "/daqfs/coda/2.6.1/cMsg/vxworks-ppc"
ld< lib/libcmsgRegex.o
ld< lib/libcmsg.o

# Load the ROC
cd "/daqfs/coda/2.6.1/VXWORKSPPC/bin"
ld < coda_roc_rc3.4

# Spawn the ROC
taskSpawn ("ROC",200,8,250000,coda_roc,"","-s","daqSession","-objects","vxroc ROC")

#load the tdc1190 library
ld < /daqfs/home/moffit/work/caen1190/caen1190Lib.o

NC1190=1
C1190_ADDR=0x260000
C1190_OFFSET=0x10000
#Initialize the 1190s
tdc1190Init(C1190_ADDR,C1190_OFFSET,NC1190,0)



