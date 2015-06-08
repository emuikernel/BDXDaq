h53360
s 00002/00002/00023
d D 1.2 11/01/24 14:57:24 boiarino 2 1
c *** empty log message ***
e
s 00025/00000/00000
d D 1.1 06/05/26 10:30:29 boiarino 1 0
c date and time created 06/05/26 10:30:29 by boiarino
e
u
U
f e 0
t
T
I 1
#all.boot
#boot script invoked by all rocs
#E.Wolin  9-jul-97
#Last modified by VHG, 08.19.98


#add login users
###cd "$CODA/VXWORKS_ppc/bootscripts"
###< loginadd.boot

#add IP hosts
cd "$CODA/VXWORKS_ppc/bootscripts"
< hostadd.boot


#set interrupt rate of the system clock
sysClkRateSet(100)

#set nfs authorization
D 2
nfsAuthUnixSet("clonfs2",2508,146)
E 2
I 2
nfsAuthUnixSet("clonfs1",2508,146)
nfsMount("clonfs1","/vol/logs/run_log","/nfs/usr/clas/logs/run_log")
E 2


D 2

E 2


E 1
