'\" t
.so wrs.an
.\" MVME6100/target.nr - Motorola MVME6100 target-specific documentation
.\"
.\" Copyright 1984-2000 Wind River Systems, Inc.
.\" Copyright 2004 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01jj,02dec04,cak Tornado Project documentation. 
.\" 01ii,24aug04,cak Enhancements to documentation.
.\" 01hh,03aug04,scb Added error handling writeup.
.\" 01gg,07jul04,cak Added IPMC support.
.\" 01ff,30jun04,cak Added support for the Maxim DS1621 Digital Thermometer
.\"		     and Thermostat.
.\" 01ee,15jun04,cak Added support for the MV64360 Timer/Counters.
.\" 01dd,07jun04,cak Added support for the MV64360 watchdog timer.
.\" 01cc,01jun04,cak Added support for programming a boot image into either
.\"		     FLASH bank (boot block) from the VxWorks kernel.
.\" 01bb,17may04,cak Added blurb about "Relocation value does not fit 
.\"		     in 24 bits" error message.
.\" 01aa,11may04,cak Removed IPMC support.
.\" 01z,05may04,scb  sysMv64360SpecialMem() support.
.\" 01y,03may04,cak  Added gevInit to gevEdit instructions.
.\" 01x,21apr04,scb  Clean up issues regarding shared memory and BusTas.
.\" 01w,16apr04,cak  Added TFFS Support.
.\" 01v,08apr04,scb  Fix dangling paragraph.
.\" 01u,17mar04,scb  Tempe DMA words.
.\" 01t,02mar04,cak  Updated jumper table, with regards to the IPMC
.\"		     support, and modified list of show routines. 
.\" 01s,25feb04,scb  Add some "show" routines to "Special Routines" section
.\" 01r,24feb04,cak  Updated.
.\" 01q,09feb04,scb  Tempe mapping support.
.\" 01p,05mar03,cak  IPMC SCSI Support.
.\" 01o,10jan03,efb  MV64360 geisc ethernet instructions.
.\" 01n,08jan03,cak  IPMC Serial Support.
.\" 01m,23oct02,cak  NVRAM/RTC/Failsafe Timer modifications.
.\" 01l,26aug02,cak  Replaced 2.1 references with 2.2
.\" 01k,18jul02,yyz  Added AuxClock support instructions.
.\" 01j,18jul02,cak  Added INCLUDE_ECC.
.\" 01i,12jul02,scb  Spec change results in two COM ports instead of three.
.\" 01h,11jul02,scb  Expanded pflash instructions.
.\" 01g,02jul02,scb  Point reader to memory map in "config.h".
.\" 01f,13Jun02,efb  Added DMA support instructions.
.\" 01e,04jun02,cak  Edited PCI Auto-config and VPD support.
.\" 01d,13may02,cak  Edited system memory (DRAM) support.
.\" 01c,25apr02,cak  Added cache support.
.\" 01b,02apr02,scb  Added information regarding interrupt handling.
.\" 01a,06mar02,cak  Written (from ver 01h of map820/target.nr).
.\"
.\"
.TH "MVME6100" T "Motorola MVME6100" "Rev: 07 Feb 04" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola MVME6100"

.SH "INTRODUCTION"
This manual entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.  This BSP is compatible with Wind River's
Tornado 2.2.1 development environment.

This BSP encompasses the MVME6100 Single Board Computer. It consists of 
the MPC7457 processor with integrated L1 and L2 cache as well as
backside L3 cache, the MV64360 system controller, two PMC card
slots, up to 2GB of ECC DDR memory, dual 10/100/1000 ethernet ports,  two 
async serial ports, two 64MB soldered FLASH banks, two 133MHz 64-bit 
PCI/PCI-X busses, Tempe (Tsi148) PCI-X to VME bridge, one PMCspan slot, 
and 32KB NVRAM/Real-Time Clock/Failsafe Timer.

The MVME6100 requires no transition module.  The IPMC761 and IPMC712 optional
PMC modules will allow 761 and 712 transition modules to be used.

.SS "Boot ROMS"
The MV6100 supports two banks of FLASH memory (A and B).  Each bank consists 
of two Intel StrataFlash 3.3 volt devices configured to operate in 16-bit mode,
to form a 32-bit FLASH port, and are soldered onboard.  Either bank can be 
used as the boot bank, which will be switch selectable.  The switch 
effectively swaps the chip selects (MV64360) of the two FLASH banks.
The OFF position (default) will select FLASH bank A as the boot bank.
The ON position will select FLASH bank B.  The boot bank (either A or B),
referenced as MV64360_BOOTCS_BASE_ADDR in the BSP, is always located at 
0xF8000000. The alternate bank (either A or B), referenced as
MV64360_DEVCS0_BASE_ADDR in the BSP, is located at 0xF4000000. 

Unlike many previous Motorola boards, the PowerPC reset vector (0xfff00000)
does not "map" to the beginning of the boot bank.  The reset vector maps to
the last 1MB of the boot bank.  So, for example, if the boot bank is 64MB,
then the reset vector will be mapped to the 63rd MB - or at an offset of
63MB into the bank.  The section "Flashing the Boot ROM Using Motorola 
MotLoad:", located near the end of this file, provides instructions for
flashing VxWorks images into the flash banks.

.IP "Boot Line Parameters"
To load VxWorks, and for more information, follow the instructions in the
\f2Tornado User's Guide: Getting Started.\f1

.SS "Jumpers"
The following jumpers and switches are relevant to VxWorks configuration:

.TS E
expand;
lf3 lf3 lf3 lf3
l l l lw(2.6i) .
.sp .5
Jumper		Function		Description
_
S4(3/6)		Bank A Write Protection		T{
ON for No Bank A Write-Protect.
OFF for Bank A Write-Protect.
T}
J29		COM1 serial port.		T{
Header COM1 UART.
T}
J7		VME System Controller		T{
Install the jumper across pins 1 and 2 for always SYSCON.
Install the jumper across pins 2 and 3 for no SYSCON.
Install NO jumpers for auto SYSCON.
T}
S4(2/7)		FLASH Boot Bank Select		T{
OFF [factory configuration] for FLASH Bank A.
ON for FLASH Bank B.
T}
S3(1/16)		SROM Initialization		T{
OFF [factory configuration] to disable SROM init.
ON to enable SROM init.
T}
S3(2/15)		I2C EEPROM Write Protect		T{
OFF [factory configuration] to write protect all I2C EEPROMs.
ON for no EEPROM write protect.
T}
S4(1/8)		Safe Start ENV		T{
OFF [factory configuration] for normal ENV settings.
ON for safe ENV settings.
T}
S4(4/5)		FLASH Bank B Boot Block Write-Protect		T{
OFF [factory configuration] for Bank B boot block write-protect.
ON for no Bank B boot block write-protect.
T}
J30		PMC/Ethernet Port 2 Controller		T{
This jumper is relevant for 01-W3878F boards only.  This jumper has 
been changed to resistors on 01-W3912F boards.
Front panel GigE (port 2), install jumpers across (A3,B3) through (A10,B10).
For rear ethernet (100TX), install jumpers across (B3,C3) through (B6,C6).
For rear ethernet (1000TX), install jumpers across (B3,C3) through (B10,C10).
For IPMC761 mode, install jumpers across (B1,C1) through (B2,C2).
For IPMC712 mode, install jumpers across (B1,C1) and across (C2,D2).
For PMC mode to P2, install jumpers across (C1,D1) through (C10,D10).
T}
J10,J15-J18,J25-J28		PMC/IPMC IO Select		T{
These jumpers are relevant for 01-W3878F boards only.  These jumpers
have been changed to resistors on 01-W3912F boards.
Install the jumper across pins 1 and 2 to select PMC2 P2 IO for PMC Mode.
Install the jumper across pins 2 and 3 to select IPMC P2 IO (extended SCSI) for IPMC Mode.
T}
.TE

For jumper and switch configuration details, see the hardware manual.

.SH "FEATURES"
The following subsections list all supported and unsupported features, as well
as any feature interaction.

.SS "Supported Features"
The following features of the MV6100 board family are supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 7
.sp .5
Feature	Description
_
Processors	T{
MPC7457
Up to 133MHz bus clock 
T}
FLASH	T{
64MB Flash BANK A 
64MB Flash BANK B
T}
TFFS	T{
True Flash File System support for both
flash banks (I28F256 parts).  Only 32 MB 
of each bank is used for TFFS as the last part
of the flash has to be reserved for the boot 
image.
T}
DRAM	T{
Up to 2GB DDR SDRAM
T}
Tempe (Tsi148) chip	T{
PCI-X to VMEbus bridge
Major features:
A16, A24, A32 and A64 address
SCT, BLT, MBLT, 2eVME, 2eSST protocols
DMA controller
T}
NVRAM	T{
32KB (MK48T37)
T}
RTC, Alarm Clock, and Failsafe Timer	T{
MK48T37
T}
Peripherals	T{
Two async serial debug ports; 
Two 10/100/1000 Ethernet interfaces; 
T}
PCI Interface	T{
64-bit PCI/PCI-X; complies with \f2PCI Local Bus Specification\f1,
Revision 2.1
T}
Miscellaneous	T{
RESET switch 
MV64360 DMA Controller
MV64360 PHB
MV64360 I2C Controller
MV64360 Interrupt Controller
MV64360 Gigabit Ethernet Controller
MV64360 Watchdog Timer
MV64360 Timer/Counters
Maxim DS1621 Digital Thermometer and Thermostat
T}
.TE

.SS "Unsupported Features"
The following features of the MV6100 board family are not supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Feature	Description
_
Miscellaneous	T{
ABORT switch 
T}
.TE

.SS "Feature Interactions"

Errata MPC7457CE Rev.5, 2003 errata #15 "Six outstanding miss
requests may stall the processor" and #20 "BTIC must not be enabled
by software".  At this time these errata only apply to the MPC7455/57
revision 0100 and 0101 processors.  These errata affect the following
files: romInit.s, config.h, and sysL3Cache.c.  More details regarding
the software workarounds can be found in the above mentioned files.

.SS "Known Problems"

A Tempe (Tsi148) errata specifies that VME bus locking and unlocking can
allow two masters on the VME bus for a short period of time.  This
errata applies to Rev 1 of the Tempe chip and will be corrected on
subsequent revs.  Because of this errata, it is not recommended that 
the sysVmeBusLockTas() function be exercised on Tempe Rev 1 boards.
To ensure that VxWorks shared memory does not use sysVmeBusLockTas(),
ANY_BRD_IN_CHASSIS_NOT_RMW should be #undef'ed.  #undef'ing this
symbol will cause Tempe RMW cycle generation to be used in the bus
test and set function.  As a result of this errata, the following 
restrictions apply when using the MVME6100 in a "mixed chassis" of boards:

The MV6100 cannot be used with older boards equipped with Universe I
chips.  The Universe I chip is incompatible with the RMW method used
by the MVME6100.  When MVME6100s arrive with Rev 2 of the Tempe chip
(bus unlocking errata fixed) then the MVME6100 and the older Universe
I chip equipped board could be used with bus locking HW assist for
busTas by setting: #define ANY_BRDS_IN_CHASSIS_NOT_RMW.

The node which is built with SM_OFF_BOARD set to FALSE is the node
which hosts the semaphores.  It is referred to as the master node.
The MVME5500 cannot be used as the master node with an MVME6100.  This
configuration is possible only with MVME6100s which are equipped with
Rev 2 of the Tempe (Tsi148) chip.  Rev 2 (and later) revs of the Tempe
chip will have the bus locking errata fixed.  Use of a Rev 2 Tempe
equipped MVME6100 with an MVME5500 as the master node is achieved
by building each BSP vith #define ANY_BRDS_IN_CHASSIS_NOT_RMW.

Note that even though the MVME5500 cannot host the semaphore with Rev
1 MVME6100 boards, it can be used in a "mixed chassis" with the
MVME6100 by having the MVME6100 host the semaphore.  To use the
MVME5500 in a shared-memory configuration with the MVME6100 hosting
the semaphore, we must build each bootrom and kernel with #undef
ANY_BRDS_IN_CHASSIS_NOT_RMW.

.SH "HARDWARE DETAILS"
This section details device drivers and board hardware elements.

.SS "Devices"
The device drivers and libraries included with this BSP are:

.TS C
center;
rw(1.0i) lw(3.7i) .
i8250Sio:	T{
Intel 8250 UART driver (serial port).
T}
AuxClk:	T{
Motorola timer driver for auxiliary clock.
T}
I2c:	T{
MV64360 I2C controller driver.
T}
isaDma:	T{
ISA DMA controller device (I8237) utilities/support driver.
T}
Mpic:	T{
MV64360 interrupt controller driver.
T}
Phb:	T{
Motorola PCI bus bridge driver.
T}
Smc:	T{
MV64360 System Memory Controller.
T}
Dma:	T{
MV64360 DMA Controller Driver.
T}
Wdog:	T{
MV64360 Watchdog Timer Driver.
T}
TmrCntr:	T{
MV64360 Timer/Counter Driver.
T}
Ds1621TempSensor:	T{
Maxim DS1621 Digital Thermometer and Thermostat driver.
T}
Tempe (Tsi148):	T{
Tempe PCI-X to VME support, include DMA.
T}
m48t37:	T{
M48T37 Timekeeper SRAM device driver.
T}
ns8730xSuperIo:	T{
Super I/O controller driver.
T}
pciAutoConfigLib:	T{
PCI autoconfiguration library.
T}
pciConfigLib:	T{
PCI configuration library.
T}
pciConfigShow:	T{
Show routines of PCI bus library.
T}
ppcDecTimer:	T{
PowerPC decrementer timer driver (system clock).
T}
smcShow:	T{
System Memory Controller configuration and SPD Show routines.
T}
sysL2Cache:	T{
MPC7455/57 L2 Cache support.
T}
sysL3Cache:	T{
MPC7455/57 L3 Cache support.
T}
sysFailsafe:	T{
Failsafe (watchdog) Timer support routines.
T}
sysMotVpd:	T{
Vital Product Data Support.
T}
sysMotVpdShow:	T{
Vital Product Data Show routines.
T}
sysRtc:	T{
Real-Time Clock and Alarm Clock support routines.
T}
z8530Sio(IPMC):	T{
Zilog 8536 UART driver.
T}
sym895Lib(IPMC):	T{
SYM53C895A SCSI driver.
T}
.TE

.SS "Tornado Project Considerations"

A component description file (01bsp.cdf) is included with this BSP to
allow configuration of the following items, from within the Tornado
Project facility:

(1) MV64360 IDMA Controller

Allows DMA Controller support to be enabled/disabled, and allows the 
setting of IDMA_SNOOP_ON, IDMA_DST_BURST_LIMIT, and IDMA_SRC_BURST_LIMIT.

(2) Error Handling Suite

Allows the error handling suite support to be enabled/disabled.

(3) SGS-Thompson M48T37V TimeKeeper SRAM Real-Time Clock

Allows the real-time clock support to be enabled/disabled.

(4) SGS-Thompson M48T37V TimeKeeper SRAM Failsafe(watchdog) Timer

Allows the failsafe timer support to be enabled/disabled.

(5) Processor Dynamic Power Management

Allows the processor dynamic power management to be enabled/disabled.

(6) Flash Boot Image Programming

Allows the boot image programming support to be enabled/disabled.
In order for sysFlashDevsMount() to be called at start up it
is necessary to add 'sysApplInit();' to usrAppInit() in usrAppInit.c
from within the project facility.  sysFlashDevsMount() mounts the 
FLASH devices and makes them available to the VxWorks IO system.

(7) MV64360 Watchdog Timer Support

Allows the mv64360's watchdog timer support to be enabled/disabled.

(8) Maxim DS1621 Temp Sensor Support

Allows the support for the Maxim DS1621 Temperature sensor to be 
enabled/disabled. 

(9) MV64360 Timer/Counter Support

This allows the support for the MV64360s timer/counters to be 
enabled/disabled.

(10) AuxClock Timer/Counter Selection

This allows the user to select which of the mv64360's timer/counters 
to use for the vxWorks Aux Clock.  This parameter can be found under
the "AUX Clock" component in Project.

There are a number of other user-configurable items available in this
BSP, in config.h.  However, components have not been created for these
other items because of the restriction that these items must be configured
the same way in the bootloader as they are in the VxWorks kernel.  Since
the Project components are not used when building the bootloader, and 
therefore have no effect on it, these items must be configured within
config.h prior to the creation of your Project.  Once that is done the
Project facility can be used to create both the bootloader and kernel
images.
 
.SS "Memory ECC Protection"

This BSP supports ECC memory and, by default, enables ECC support in the 
system memory controller by defining INCLUDE_ECC in config.h.  To disable 
ECC support simply change #define INCLUDE_ECC, in config.h, to 
#undef INCLUDE_ECC.  Disabling ECC support will result in a slight
performance increase, since partial (less than 64-bit) writes to memory 
will not require a RMW transaction as described in section "8.6 Read Modify 
Write" in Rev 0.98 of the MV64360 System Controller manual. 

.SS "MV64360 System Memory Controller"

The MV64360's system memory controller (DDR SDRAM) registers contain a 
number of fields that can be configured for system optimization.  The 
following definitions, located in config.h, are used to configure these 
fields.  The current settings are optimized for general VxWorks 
application.  It is not recommended that you change these settings, unless 
it is necessary for your specific application.  To enable/disable or 
change these default settings simply modify these definitions.  Only 
those fields described below can be modified.  See the MV64360 System
Controller manual for a more complete description of each of 
these fields.  Any modification of these settings will necessitate a 
rebuilding of the bootrom, since this is where the memory controller is
initialized.
 
(1) SDRAM Configuration Register

#define MV64360_DDR_SDRAM_CFG_DFLT              0x80000000

Bits[31:26] Read Buffer Assignment per each interface

By default we will set the Gb unit to use read buffer 1 while all 
other units will use read buffer 0.
 
(2) SDRAM Address Control Register

#define MV64360_DDR_SDRAM_ADDR_CTRL_DFLT        0x00000002

Bits[3:0] SDRAM Address Select

By default this will be set to 0x2.
 
(3) SDRAM Open Pages Control Register

#define MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_DFLT  0x00000000

Bits[15:0] Open Page Enable

By default this will be set to 0x0.
 
(4) SDRAM Interface Crossbar Control (Low) Register

#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_LO_DFLT  0x00765432

.CS
Bits[3:0]   Slice 0 device controller "pizza" arbiter
Bits[7:4]   Slice 1
Bits[11:8]  Slice 2
Bits[15:12] Slice 3
Bits[19:16] Slice 4
Bits[23:20] Slice 5
Bits[27:24] Slice 6
Bits[31:28] Slice 7
.CE

By default this will be set to 0x00765432.
 
(5) SDRAM Interface Crossbar Control (High) Register

#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_HI_DFLT  0x00765432

.CS
Bits[3:0]   Slice 8 
Bits[7:4]   Slice 9 
Bits[11:8]  Slice 10 
Bits[15:12] Slice 11 
Bits[19:16] Slice 12 
Bits[23:20] Slice 13 
Bits[27:24] Slice 14 
Bits[31:28] Slice 15 
.CE

By default this will be set to 0x00765432.
 
(6) SDRAM Interface Crossbar Timeout Register

#define MV64360_DDR_SDRAM_IF_XBAR_TMOUT_DFLT    0x000100FF

.CS
Bits[7:0] Crossbar Arbiter Timeout Preset Value
Bit[16]   Crossbar Arbiter Timer Enable
.CE

By default this will be set to 0x000100FF.

.SS "TrueFFS Flash File System"

This BSP supports TrueFFS Flash File System on either, or both, FLASH bank
A and FLASH bank B.  TFFS is disabled by default.  To enable TFFS for FLASH
bank A change #undef INCLUDE_TFFS_FLASHA, in config.h, to 
#define INCLUDE_TFFS_FLASHA.  To enable TFFS for FLASH bank B change
#undef INCLUDE_TFFS_FLASHB, in config.h, to #define INCLUDE_TFFS_FLASHB.
The size is set to 32MB.  We do this because the FLASH banks
are each 64MB in size, but in order to protect any boot images
that may reside in the 63rd MB of the FLASH bank - either firmware
or another VxWorks image - and also conform to the TFFS restriction that
the flash bank size must be a power of 2 (1,2,4,8,16,32,...) we are not
able to utilize more than 32 MB of the FLASH.

After enabling TFFS support and rebuilding the kernel, the TFFS drives
will be available as "RFA0" (FLASH bank A) and "RFA1" (FLASH bank B), as
support is provided to automatically mount the TFFS drives during kernel
initialization.  If you wish to use TFFS to boot the VxWorks kernel you 
will need to rebuild the bootrom.

.SS "Boot Image FLASH Programming from VxWorks Kernel"

This BSP supports FLASH programming of a boot image (into the boot block)
of either FLASH bank A (/flash0) or FLASH bank B (/flash1). This support 
is disabled by default.  To enable FLASH bank programming change #undef
INCLUDE_BOOT_IMAGE_PROGRAM to #define INCLUDE_BOOT_IMAGE_PROGRAM in config.h.

This will provide two utilities for programming a boot image into the 
boot block of the FLASH devices. One utility allows you to download
the boot image via the network, and one utility allows you to specify
a locally resident boot image (ie. on disk, tffs, etc.).

The device names are /flash0 for FLASH bank A, and /flash1 for FLASH bank B.

.CS
-> devs
drv name                
  0 /null               
  1 /tyCo/0             
  1 /tyCo/1             
  5 sunburn:            
  6 /vio                
  7 /flash0             
  7 /flash1             
.CE

The utility to program a file that has been downloaded from a remote host is:

.CS
 programRemoteBootImage
    -n  filename to download
    -d  devicename of FLASH bank to be programmed
    -h  host dotted ip address (optional - boot host as default)
    -g  gateway dotted ip address (optional - boot gateway as default)
    syntax: remoteBootImage "-n<fname>","-d<devname>","-h<host>","-g<gateway>"
.CE

Either RSH, FTP, or TFTP will be used to download the file, based on the
vxWorks boot parameters that are specified.  If the "flags" boot parameter 
is set to 0x80 (for TFTP) then TFTP will be used.  Otherwise, if a 
"password" is specified, then FTP will be used.  If neither of the previous 
two conditions are met then RSH will be used to download the file.

The utility to program a "local" file is:

.CS
 programLocalBootImage
    -n  filename to open
    -d  devicename of FLASH bank to be programmed
    syntax: localBootImage "-n<fname>","-d<devname>"
.CE

The file name specified must be a locally accessible file, located on
a hard disk, a TFFS drive, or another such local medium.

Be very careful when using this utility that you do not overwrite the
MotLoad firmware image, if that is not your intent, as this could be
troublesome to fix.

.SS "Firmware RAM and ROM Booting"

The definition FIRMWARE_BOOT, in config.h, allows one to use the MOTLoad 
firmware command 'execProgram' to begin execution of VxWorks code rather 
than having the processor, upon powerup, automatically begin executing 
code from the cold boot location (PowerPC reset vector).  This provides 
flexibility in just where the VxWorks image can reside and still be 
invoked for execution.  FIRMWARE_BOOT is #undef'ed by default.

The bootloader or kernel code which is executed via MOTLoad's execProgram 
command must have been previously loaded into RAM or alternatively placed 
into flash memory.  The commentary which follows explains this process.
 
To execute a RAM-resident VxWorks "boot" image (rather than a flash 
resident "boot" image), perform the following:
 
(a) Change #undef FIRMWARE_BOOT to #define FIRMWARE_BOOT.

(b) In mv6100A.h, ROM_BASE_ADRS may need to be adjusted.  It is currently 
    set, for RAM booting, based on the assumption that there is 512MB of 
    DRAM.  However, this should be adjusted to the actual amount of DRAM 
    on the board minus 1MB, in order to leave room for the vxWorks boot 
    image at the top of DRAM.

    ie. DRAM - 0x100000  (currently set to 0x1ff00000, if FIRMWARE_BOOT 
			  is #define'd)

(c) Since ROM_TEXT_ADRS and ROM_WARM_ADRS are defined in both the 
    Makefile (hard-coded) and config.h (based on the value of ROM_BASE_ADRS) 
    it is necessary to adjust the values of ROM_TEXT_ADRS and ROM_WARM_ADRS 
    in the Makefile.  The values used are dependent upon the value of 
    ROM_BASE_ADRS as discussed in (b) above.  For a board with 512MB of 
    DRAM they would be 0x1ff00000 and 0x1ff00004, respectively.

Please note that if FIRMWARE_BOOT is defined then USER_RESERVED_MEM will 
be defined as 0x100000 (1MB) in order to preserve the boot image in the top 
1 MB of the DRAM.
 
To use the firmware to boot vxWorks from DRAM use the following commands 
(sustitute the appropriate values where necessary).  The values used for 
the tftp download address and the execP load address are based on a board 
with 512MB of DRAM:

.CS 
tftpGet -cXXX.XXX.XXX.XXX -fboot.bin -sXXX.XXX.XXX.XXX -d/dev/enetx -a1FF00000
execProgram -l1FF00000 -s100000
.CE

If you use this boot method, keep in mind that the bootloader/kernel image
will need to be downloaded every time the board experiences a "cold reset"
(ie. power-on reset).  A software reset ("reboot" or "CTRL-X") can still
be performed, as the image is located in a "protected" region of DRAM
(protected via USER_RESERVED_MEM).
 
A "cold boot" (or powerup boot) always causes the image flashed into the 
last megabyte of the boot flash to be executed.  The execution address 
for cold boot start is the PPC reset vector at 0xfff00100, which is in 
the last megabyte of the 4-gigabyte address space.  We may want to use 
a flash bank to hold an image for execution but we also want to protect 
the "cold boot" image that is present in the last megabyte of flash memory 
in this bank.  We can use the first megabyte of flash memory, or any other
megabyte but the last (boot block), for our executable image and use the 
MOTLoad "execProgram" command to execute that image - thus protecting the 
"cold boot" image which is located in the last megabyte of flash memory.  
The explanation which follows describes how to do this.
 
Remember that no matter how the flash bank selection jumper is set, the 
base address (address of the first megabyte) of the boot flash bank is 
always 0xf8000000.  Likewise the address of the non-boot flash bank is 
always 0xf4000000.
 
To execute a vxWorks "boot" image from ROM, perform the following:

(a) Change #undef FIRMWARE_BOOT to #define FIRMWARE_BOOT.

(b) In mv6100A.h, ROM_BASE_ADRS will need to be set to the address in the 
    flash bank where the image is located, and from which the code will be 
    executed via the MOTLoad "execProgram" command.  Since we are specifying 
    an address other than the "boot block", the image that is resident in the 
    last megabyte (cold boot image) will be protected.  So, if your image 
    to be executed is located in the first megabyte of the boot flash bank 
    you would set ROM_BASE_ADRS to be 0xf8000000.  If the image is located 
    in the first megabyte of the non-boot flash bank you would set 
    ROM_BASE_ADRS to be 0xf4000000.

(c) Since ROM_TEXT_ADRS and ROM_WARM_ADRS are defined in both the Makefile
    (hard-coded) and config.h (based on the value of ROM_BASE_ADRS) it is 
    necessary to adjust the values of ROM_TEXT_ADRS and ROM_WARM_ADRS in 
    the Makefile.  The values used are dependent upon the value of 
    ROM_BASE_ADRS as discussed in (b) above.  When booting from the 
    first megabyte of the non-boot flash bank they would be 0xf4000000 and 
    0xf4000004, respectively.  When booting from the first megabyte of the
    boot flash bank they would be 0xf8000000 and 0xf8000004, respectively.

(d) If FIRMWARE_BOOT is defined, USER_RESERVED_MEM will be set to 0x100000 
    (1MB).  The reason for this is to setup the firmware booting for DRAM 
    booting by default, rather than ROM booting.  If left as is the top 
    1MB of DRAM will be reserved, which is not necessary when booting from 
    ROM.  To change this simply change USER_RESERVED_MEM, located near the 
    end of this file, to 0x0.
 
To use the firmware to boot vxWorks from ROM use the following commands 
(substitute the appropriate values where necessary).  The execP load 
address is dependent upon which flash bank the VxWorks image is located in,
in this example it is located at the beginning of the non-boot flash bank
(0xf4000000):

.CS
tftpGet -cXXX.XXX.XXX.XXX -fboot.bin -sXXX.XXX.XXX.XXX -d/dev/enetx
flashProgram -o0 -nfff00 -d/dev/flashx
execProgram -lf4000000 -s100000
.CE

.SS MV64360 I2C Support

I2C support is provided by the MV64360 I2C interface.  The following I2C 
devices are available on the MVME6100:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Address	Device
_
$A0	Memory SPD (for Bank 0 and 1)
$A2	Memory SPD (for Bank 2 and 3)
$A6	MV64360 User Defined Initialization SROM
$A8	Configuration VPD
$AA	User VPD
$90	DS1621 Temperature Sensor
.TE

The routines sysMotI2cWrite() and sysMotI2cRead() are provided to read 
from, and write to, the I2C devices.  Below is a description of the 
arguments that are required for these routines:

.CS
STATUS sysMotI2cWrite
    (
    UINT32 devAddr,     /* I2C address of target device */
    UINT32 devOffset,   /* offset within target device to write */
    UINT32 devAddrBytes,/* number of address bytes of device */
    UCHAR  *pBfr,       /* pointer to data to be written */
    UINT32 numBytes     /* number of bytes to write */
    )
STATUS sysMotI2cRead
    (
    UINT32 devAddr,     /* I2C address of target device */
    UINT32 devOffset,   /* offset within target device to read */
    UINT32 devAddrBytes,/* number of address bytes of device */
    UCHAR  *pBfr,       /* pointer to read buffer */
    UINT32 numBytes     /* number of bytes to read */
    )
.CE

The macro VALID_I2C_WRT_ADDR is provided as a software protection to 
prevent inadvertent writes to the I2C devices, such as the SPDs and 
VPD.  VALID_I2C_WRT_ADDR can be found in mv6100A.h.

.SS MV64360 Device Bus

The MVME6100 uses three of the available five device banks of the MV64360.
The following is a list of the devices and registers that are available
via these device banks.  Included is the address at which each of these
are mapped and accessible.

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Bank	Assigned to	Address
_	
Device Bank 0	FLASH bank A or B	0xF4000000
Device Bank 1	System Status Register 1	0xF1D00000
	System Status Register 2	0xF1D00001
	System Status Register 3	0xF1D00002
	Presence Detect Register	0xF1D00004
	SW Readable Header/Switch	0xF1D00005
	Timebase Enable Register	0xF1D00006
	Geographical Address Register	0xF1D00007
	NVRAM	0xF1D10000 - 0xF1D17FFF
	COM 0	0xF1D20000 - 0xF1D20FFF
	COM 1	0xF1D21000 - 0xF1D20FFF
Boot Bank	FLASH bank A or B	0xF8000000
.TE

.SS "Cache Support"

The MVME6100 contains L1 and L2 cache integrated within the MPC7457 
processor, and contains back side L3 cache.  L1 cache support is 
controlled by the definition INCLUDE_CACHE_SUPPORT, which is #define'd 
by default in configAll.h.  L2 cache support is controlled by the 
definition INCLUDE_CACHE_L2, which is #define'd by default in config.h.  
L3 cache support is controlled by the definition INCLUDE_CACHE_L3, 
which is #define'd by default in config.h.  It is recommended that you
rebuild the bootrom and the kernel images after making any modifications 
to the cache support.  

The following are the legal cache configurations:

(a) L1 cache only

(b) L1 and L2 cache only

(c) L1, L2, and L3 (as cache or private memory) cache

(d) L1 cache, L3 cache configured as private memory

The following features of the L2 cache can be configured (these are 
relevant only if INCLUDE_CACHE_L2 is defined):
 
(1) Number of prefetch engines enabled

The number of prefetch engines enabled can range from none (disabled) 
to 3.  This is controlled with the MSSCR0_L2_PREFETCH_ENGINES definition 
in config.h. The default number of prefetch engines is 1 (MSSCR0_L2PFE_ONE). 
Valid settings include: MSSCR0_L2PFE_NONE, MSSCR0_L2PFE_TWO, and 
MSSCR0_L2PFE_THREE.
 
(2) Enable/Disable L2 Data Parity Checking

L2 data parity checking can be enabled/disabled with the 
ENABLE_L2_DATA_PARITY definition in config.h.  Parity checking is enabled 
by default.

(3) Mode

The L2 cache can be configured for Instruction-only mode, Data-only mode, 
or both (instructions and data) with the L2_MODE definition in config.h.  
The default is to allow cacheing of both data and instructions 
(L2_MODE_INST_AND_DATA).  Valid settings also include: L2_MODE_I_ONLY 
and L2_MODE_D_ONLY.
 
(4) L2 Replacement Algorithm

The L2 cache replacement algorithm can be configured for Default 
replacement algorithm or Secondary replacement algorithm.  The default 
is to use the secondary replacement algorithm (L2_REP_ALG_SECONDARY).  
To use the default algorithm set L2_REP_ALG, in config.h, to 
L2_REP_ALG_DEFAULT.

The following features of the L3 Cache can be configured (these are 
relevant only if INCLUDE_CACHE_L3 is defined):
 
(1) Mode

The L3 cache can be configured for Instruction-only mode, Data-only mode, 
or both (instructions and data) with the L3_MODE definition in config.h.  
The default is to allow cacheing of data and instructions 
(L3_MODE_INST_AND_DATA).  Valid settings also include: L3_MODE_I_ONLY 
and L3_MODE_D_ONLY.

See "Errata Workaround" below for more information.
 
(2) L3 Replacement Algorithm

The L3 cache replacement algorithm can be configured for Default 
replacement algorithm or Secondary replacement algorithm.  The default 
is to use the secondary replacement algorithm (L3_REP_ALG_SECONDARY).  
To use the default algorithm set L3_REP_ALG to L3_REP_ALG_DEFAULT.
 
(3) L3 Private Memory

L3 private memory support is controlled by the definition 
INCLUDE_CACHE_L3_PM, in config.h, which is disabled (#undef'ed) by 
default.  If enabled the L3 private memory will be located at 0xF1200000.

Private memory is not available until you call sysL3CachePmEnable().
To configure 1MB of L3 cache as private memory, after having #define'd
INCLUDE_CACHE_L3_PM, and having rebuilt the bootrom and kernel:

.CS
sysL3CachePmEnable(0x100000) 
.CE

To configure 2MB of L3 cache as private memory:

.CS
sysL3CachePmEnable(0x200000) 
.CE

0x100000 (1MB) and 0x200000 (2MB) are the only valid sizes for the
private memory.  If you call sysL3CachePmEnable() with any other
sizes, ERROR will be returned.

The following routines are provided for disabling and reenabling
the L3 cache:

.CS
sysL3CacheFlushDisable() 
sysL3CacheInvEnable() 
.CE

To disable private memory:

.CS
sysL3CachePmDisable()
sysL2CacheFlush()
.CE

After disabling the private memory, it is recommended that you
flush the L2 cache.  This will remove any "stale" private memory
data that may still exist in the cache.

Errata Workaround (MPC7455/7457 Revisions 0100 & 0101)
 
Errata #15 "Six outstanding miss requests may stall the processor" 
in MPC7457CE Rev.5, 12/2003 specifies that a processor configured 
with 2MB of L3 cache must adhere to either of the following two
software workarounds:

(1) Operate in 1MB cache, 1MB private memory mode, in which case 
    the mode of the cache can be instruction-only, data-only, or 
    both instruction and data, or

(2) Operate the L3 cache (2MB) in instruction-mode only.

To implement these workarounds the definition 
ERRATA_WORKAROUND_2MB_IONLY has been created.  If you wish to operate 
in cache-only mode (ie. no private memory) this definition will
determine which of the above two workarounds is chosen.  For example, 
if ERRATA_WORKAROUND_2MB_IONLY is defined then the full 2MB of cache 
will be configured to operate in instruction-only mode.  However, if 
it is not defined (#undef'd), then only 1MB of the cache will be 
utilized and the mode will be determined by L3_MODE.  If you decide 
to enable the private memory support then ERRATA_WORKAROUND_2MB_IONLY 
will be used to determine the initial configuration of the L3 cache, 
which is necessary to perform prior to configuring and enabling the 
private memory by calling the sysL3CachePmEnable() routine in 
sysL3Cache.c.  Ultimately the mode of the non-private memory portion 
of cache will be set according to L3_MODE as well.

Default L3 Cache Configuration in the MVME6100 BSP is as follows:

The L3 cache is enabled, private memory is disabled, "data and 
instructions" mode is specified, and option (2) of the errata 
workarounds is chosen by undefining ERRATA_WORKAROUND_2MB_IONLY.
This means that, for revisions "0100" and "0101" of the MPC7455/7457 
processors, only 1MB of the L3 cache is utilized and it is configured 
for data and instructions.  To determine what revision of the processor
you have use the utility sysPvrGet().  This will return the contents of
the processor version register which contains fields for the revision
number.

.SS "Maxim DS1621 Digital Thermometer and Thermostat Support"

This driver is not (in and of itself) intended to be safe for 
multi-threaded environments.  That support is intended for the 
application software that overlays this driver.

The DS1621 measures temperatures from -55C to +125C.
Data is read from/written via a 2-wire serial interface
(I2C).  The temperature reading is provided in a 9-bit, two's
complement reading by issuing the READ TEMPERATURE command.  The first
byte transmitted is the MSB.  We will not use the LSB.  Instead, higher
precision temperature readings will be obtained using the counter and
slope registers.

The DS1621 can be programmed to perform continuous consecutive
conversions (continuous-conversion mode) or to perform single
conversions on command (one-shot mode).  This is user-selectable.
Note that the DS1621 always powers up in a low power
idle state, and the Start Convert T command must be used to
initiate conversions.

The DS1621 can also operate in two different modes: thermostat
and thermometer.  In thermometer mode the DS1621's interrupts
will be disabled, and TH and TL will be set to their
absolute maximum and minimum, respectively.  In this mode
application software can poll the chip for current temperature
using the supplied interface routines.

In thermostat mode the DS1621's interrupts will be enabled
and TH and TL will be set to user-specified values.
When the DS1621's temperature meets or exceeds the value stored
in the high temperature trip register (TH), the output becomes active
and will stay active until the temperature falls below the
temperature stored in the low temperature trigger register (TL).
For this reason the interrupt handler for the DS1621 will disable
the interrupt and then call a user-specified interrupt handler,
if an interrupt handler has been connected.  The interrupts will
not be enabled again until the DS1621's enable routine is called.
This will allow the user's application code to poll the chip to
obtain the temperature, perhaps until it falls below the value
contained in TL, and then reenable the DS1621 once it does.  If
the interrupt were not disabled the chip would continue to signal
an interrupt until the temperature drops below TL.

The active state for the output is programmable so that the active
state may either be a logic "1" or a logic "0".  This is done with
the POL (1) bit in the configuration register, and is board
dependent.

The DS1621 contains the following registers: Configuration Register,
Temperature Register, Temperature High Register, Temperature Low Register,
Counter Register, and Slope Register.

This library provides the following routines:

.CS
STATUS sysDs1621Show (void)
.CE

This routine will display to current board temperature, the value of
the Temperature High register, the value of the Temperature Low register,
and a few configuration options of the DS1621.  INCLUDE_SHOW_ROUTINES must
be defined in order to use this routine.

.CS
STATUS sysDs1621Connect (FUNCPTR)
.CE

This routine can be called to connect a user-specified interrupt handler
to the DS1621's interrupt.  sysDs1621Connect() allows only one routine
to be connected and multiple calls to sysDs1621Connect result in
only the last call being the effective one.

.CS
STATUS sysDs1621ConversionStart (void)
.CE

This routine can be used to start the DS1621's temperature conversion.

.CS
STATUS sysDs1621ConversionStop (void)
.CE

This routine can be used to stop the DS1621's temperature conversion.

.CS
STATUS sysDs1621THRegWrite (UINT8, UINT8)
.CE

This routine can be used to write to the Temperature High register.

.CS
STATUS sysDs1621THRegRead (UINT8 *, UINT8 *)
.CE

This routine can be used to read from the Temperature High register.

.CS
STATUS sysDs1621TemperatureHighShow (void)
.CE

This routine can be used to display the temperature programmed in the
Temperature High register.

.CS
STATUS sysDs1621TLRegWrite (UINT8, UINT8)
.CE

This routine can be used to write to the Temperature Low register.

.CS
STATUS sysDs1621TLRegRead (UINT8 *, UINT8 *)
.CE

This routine can be used to read from the Temperature Low register.

.CS
STATUS sysDs1621TemperatureLowShow (void)
.CE

This routine can be used to display temperature programmed in the
Temperature Low register.

.CS
UINT8 sysDs1621ConfigRegRead (void)
.CE

This routine can be used to read from the Configuration register.

.CS
STATUS sysDs1621ConfigRegWrite (UINT8)
.CE

This routine can be used to write to the Configuration register.

.CS
UINT8 sysDs1621CounterRegRead (void)
.CE

This routine can be used to read from the Counter register.

.CS
UINT8 sysDs1621SlopeRegRead (void)
.CE

This routine can be used to read from the Slope register.

.CS
STATUS sysDs1621TempRegsInterpret (UINT32, UINT8, UINT8, DS1621_TEMP *)
.CE

This routine is used to convert the temperature register values into
"real" temperatures.

.CS
STATUS sysDs1621TemperatureGet (DS1621_TEMP *)
.CE

This routine can be used to obtain the temperature of the board.

.CS
STATUS sysDs1621TemperatureShow (void)
.CE

This routine is used to display the temperature of the board.

.CS
STATUS sysDs1621Config (UINT32, UINT32, DS1621_TEMP *, DS1621_TEMP *)
.CE

This routine is used to configure the DS1621.  The parameters required for
this routine are: the conversion mode to be used (DS1621_CONV_MODE_CONT or
DS1621_CONV_MODE_1SHOT), the operating mode (DS1621_MODE_THERMOSTAT or
DS1621_MODE_THERMOMETER), and the values for Temperature High and for
Temperature Low.  The structure for Temperature High and Temperature Low are
as follows:

.CS
typedef struct ds1621Temp
    {
    int integer;        /* integer portion of temperature */
    UINT32 fraction;    /* fraction (1/1000th) portion of temperature */
    } DS1621_TEMP;
.CE

This routine must be called before enabling the DS1621.  Once the DS1621 has
been enabled it is possible to reconfigure without explicitly calling the
sysDs1621Disable() routine.  However, the routine will call sysDs1621Disable()
in order to temporarily disable the DS1621 while it reconfigures it.  After
which it will call sysDs1621Enable to reenable it.

.CS
STATUS sysDs1621Enable (void)
.CE

This routine is used to enable (ie. start) the DS1621 once it has been
configured by calling sysDs1621Config() above.

.CS
STATUS sysDs1621Disable (void)
.CE

This routine is used to disable (ie. stop) the DS1621.

The routine sysDs1621Test() located in sysDs1621TempSensor.c shows some
examples of how these routines can be used.
 
.SS "MV64360 Watchdog Timer Support"

This BSP supports the MV64360 watchdog timer.  This support is disabled 
by default.  To enable MV64360 watchdog timer support change #undef
INCLUDE_MV64360_WDOG, in config.h, to #define INCLUDE_MV64360_WDOG.

The following routines are provided for the watchdog timer:

STATUS sysMv64360WdogStart (seconds, mode)

This routine has two input parameters: the first is the number of seconds
until watchdog expiration, and the second is the watchdog timer mode.  The
mode can be either WDOG_MODE_INTERRUPT (0) or WDOG_MODE_RESET (1).

STATUS sysMv64360WdogStop ()

This routine will stop the watchdog timer by disabling it.  The only way to
restart it is to call sysMv64360WdogStart() again.

STATUS sysMv64360WdogService ()

This routine will service the watchdog timer.  Servicing the watchdog
periodically will prevent it from expiring.

.SS "MV64360 Timer/Counter Support"

This BSP supports the MV64360 timer/counters.  This support is disabled 
by default.  To enable MV64360 timer/counter support change #undef
INCLUDE_MV64360_TMRCNTR, in config.h, to #define INCLUDE_MV64360_TMRCNTR.

There are a total of four identical timer/counters on the MV64360,
each 32-bits wide.  Each timer/counter can be selected to operate as
a timer or a counter.  Each timer/counter increments with every Tclk
rising edge.  The MV64360 timer/counters are driven by the CPU bus clock.

In Counter mode, the counter counts down to zero, stops, and issues
an interrupt.

In Timer mode, the timer counts down, issues an interrupt when it
reaches zero, reloads itself to the programmed value, and continues
to count.

Besides four timer/counter registers, there are three other timer
related registers:

Timer/Counter Control Register -
Bit 0 is the Timer/Counter Enable bit of timer/counter 0.  Writing
a value of "0" disables timer/counter 0, while writing a value of
"1" enables timer/counter 0.  When in timer mode, the count continues
as long as this bit is set to '1'.  Bit 1 is the Timer/Counter Mode bit,
the timer/counter 0 is in timer mode if this bit is set to '1'. Bits 8
and 9 are the enable and mode bits for timer/counter 1, bits 16 and 17
are the enable and mode bits for timer/counter 2, and bits 24 and 25 are
the enable and mode bits for timer/counter 3.

Timer/Counter Interrupt Cause Register -
Bit 0 is set to 1 upon timer 0 terminal count.  It's cleared by writing
a value of "0".  Writing a value of "1" has no effect.  Bit 1 is the
terminal count for timer/counter 1, bit 2 is the terminal count for
timer/counter 2, and bit 3 is the terminal count for timer/counter 3.

Timer/Counter Interrupt Mask Register -
Writing a value of "0" to bit 0 disables timer/counter 0 interrupt.
Writing a value of "1" to bit 0 enables timer/counter 0 interrupt.
Bit 1 is the interrupt mask for timer/counter 1, bit 2 is the interrupt
mask for timer/counter 2, and bit 3 is the interrupt mask for
timer/counter 3.

Keep in mind that one of the timer/counters is used for the VxWorks
auxClk, so only 3 are available for use.  In config.h, the definition
MV64360_AUXCLK_TMR is used to define which of the timer/counters is used
to implement the AuxClk, and is user-configurable.

The following routines are provided for the timer/counters:

STATUS sysMv64360TmrCntrStart (timer, seconds, mode)

This routine has three input parameters: the first is the number of the
timer, the second is the number of seconds until timer/counter expiration,
and the third is the timer/counter mode.  The mode can be either 
MV64360_TMRCNTR_MODE_TMR (0), for timer mode, or MV64360_TMRCNTR_MODE_CNTR 
(1), for counter mode.

STATUS sysMv64360TmrCntrDisable (timer)

This routine will disable the given timer/counter.  The only way to
restart it is to call sysMv64360TmrCntrStart() again.

Two routines are also provided for connecting and disconnecting user-defined
interrupt handlers to the timer/counter interrupts:

STATUS sysMv64360TmrCntrConnectRoutine (routine, arg, timer)

STATUS sysMv64360TmrCntrDisconnectRoutine (timer)

Before connecting a new routine be sure to disconnect the old routine by
first calling sysMv64360TmrCntrDisconnectRoutine().

The current (real-time) value of a given timer/counter can be retrieved using
the following routine:

UINT32 sysMv64360GetTmrCntrValue (timer)

.SS "System Control and Status Registers"

Definitions are provided for access to the MVME6100's system control and 
status registers.  These definitions are located in mv6100A.h and are as 
follows:

.CS
SYSTEM_STATUS_REG_1
SYSTEM_STATUS_REG_2
SYSTEM_STATUS_REG_3
PRESENCE_DETECT_REG
CFG_HDR_SWITCH_REG
TIME_BASE_ENB_REG
.CE

.SS "Processor Support"

The MVME6100 contains the MPC7457 processor.

The MVME6100 BSP provides software workarounds for the following two errata:

a) Errata #20 "BTIC must not be enabled by software"

According to this errata, BTIC must not be enabled for processor revisions 1.0 
and 1.1.   Processor revisions after 1.1 have had this problem fixed and the 
BSP will set the BTIC bit when running on these later processor types.

b) Errata #15 "Six Outstanding miss requests may stall the processor"

More information regarding the software workarounds for this errata 
can be found in the "Cache Support" section.

Altivec support is controlled by the definition INCLUDE_ALTIVEC 
located in config.h.  Altivec support is disabled by default.

Dynamic Power Management support is controlled by the definition 
INCLUDE_DPM located in config.h.  It is disabled by default.

.SS "Shared Memory Support"

On all boards, shared memory across the backplane can also be used as a
network interface.  The name of the shared memory interface is `sm'.  
The BSP can be configured for shared memory support by 
#define'ing INCLUDE_SM_NET in config.h.

Shared memory network communications requires a signaling method and a method
of mutually exclusive memory resource access.  Signaling can be done using
software polling or interrupts.  By default, mailbox interrupts are used and
SM_INT_TYPE is set to SM_INT_MAILBOX_1.  To use polling, \f3#define\f1
SM_INT_TYPE as SM_INT_NONE.

There are master and slave windows into VME address space to access the VME
mailbox registers so that each CPU can send and receive shared memory
interrupts using single-byte mailboxes.
The windows map a 4KB region in A32 space at address 0xFB000000 + (0x1000 *
CPU #) into the Tempe chip registers.  This configuration allows one
processor to generate a mailbox interrupt in another processor by accessing the
other processor's mailbox register.  Each CPU has a
master window covering the A32 addresses 0xFB000000 through 0xFB00ffff
representing CPU numbers 0 through 15.  Each CPU's slave window maps the
appropriate address for that CPU to the Tempe chip's register set.

On the processor node compiled with SM_OFF_BOARD set to FALSE, an inbound 
window is opened using Tempe's inbound window 0 to map the shared-memory 
anchor and shared-memory pool.

The inbound access is shown here (obtained from sysTempeInWinShow(0)):

.CS 
Inbound window 0: ENABLED,  Size = 00000000_00040000
 
raw: 00000000 08000000 00000000 0803fff0 ffffffff f8000000 80000faf
VME Base:  00000000_08000000    PCI Base:  00000000_00000000
VME Limit: 00000000_0803ffff    PCI Limit: 00000000_0003ffff
  Attributes:
  Read-ahead threshold - when FIFO competely empty
  Virtual FIFO size    - 64 bytes
  2eSST Mode           - 160 MB/s
  Cycle response       - 2eSSTB (Two Edge Source Synchronous Broadcast)
 2eSST (Two Edge Source Synchronous nonBroadcast)
 2eVME (Two Edge VMEbus)
 MBLT (Multiplexed Block Transfer)
 BLT (Block Transfer)
  Address space        - A32
  VME AM response      - Supervisor, nonSupervisor, Program, Data
.CE

The algorithm for Tempe (Tsi148) window configuration is as
follows:

.CS
If this is not the anchor node (SM_OFF_BOARD == TRUE)
Then
  No A32 inbound mappings are programmed for access to DRAM.
  Note that all nodes must have inbound mapping to reach their mailbox's 
  so that shared memory doorbells can be hit by any other node.  This is 
  taken care of by the RAI register programming on Universe equipped 
  boards and the CRG programming by Tempe equipped boards.
Else (this is the anchor node ...)
  A single inbound mapping through Tempe window 0 is configured, mapping 
  DRAM address 0x00000000 through 0x0003ffff to VME bus address 
  0x08000000 through 0x0803ffff.
.CE

Shared memory resource mutual exclusion is implemented in the sysBusTas
function.  Two modes of hardware assist are available to implement
the test and set function across the VME bus.  They are: bus locking
and RMW cycle generation.  The method used is selected in config.h
via the #define/#undef state of ANY_BRDS_IN_CHASSIS_NOT_RMW.  Under 
"Known Problems" in this document a restriction on the choice of hardware 
assist for Tsi148 Rev 1 equipped boards is described.

The following is taken from config.h:

.CS
#undef ANY_BRDS_IN_CHASSIS_NOT_RMW  /* required for Rev 1 of Tempe chip */

#undef VME_TAS_HW_ASSIST
#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
#   define VME_TAS_HW_ASSIST    VME_BUS_LOCK
#else
#   define VME_TAS_HW_ASSIST    VME_BUS_RMW
#endif
.CE

In addition, a limitation of the Discovery II host bridge chip requires 
that snooping on BUS 0 (the bus that the Tempe PCI-X to VME bridge is on) 
be disabled to prevent a potential PCI lock for a snoop cycle which cannot 
be serviced (see Discovery II App-note 85), hence the setting for 
PCI_DMA_SNOOP_BUS_0_ON below.  Also, since snooping is disabled the shared 
memory anchor and memory pool must be in a non-cacheable region.  To 
accomplish this a #define for SM_CACHE_STATE controls the cacheing 
attribute for this area.

The following is taken from config.h:

.CS
#if defined(INCLUDE_SM_NET)
#   define INCLUDE_SM_COMMON
#   define INCLUDE_BSD
#   define INCLUDE_SM_NET_SHOW
#   define INCLUDE_NET_DRV
#   define INCLUDE_NET_LIB
#   ifdef  STANDALONE
#          define STANDALONE_NET
#   endif  /* STANDALONE */
#   undef  PCI_DMA_SNOOP_BUS_0_ON
#endif /* defined(INCLUDE_SM_NET) */

#if ((defined INCLUDE_SM_NET) && (SM_OFF_BOARD == FALSE))
#   define SM_CACHE_STATE  VM_STATE_CACHEABLE_NOT
#else
#   define SM_CACHE_STATE  VM_STATE_CACHEABLE
#endif
.CE

The following is the entry for sysPhysMemDesc[] in sysLib.c:

.CS
    {
    (void *) ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    (void *) ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    SM_BASE - ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | SM_CACHE_STATE |
    VM_STATE_MEM_COHERENCY
    },
.CE

.SS "Memory Maps"

On-board RAM always appears at address 0x00000000 locally.

Dynamic memory sizing is supported.  By default, LOCAL_MEM_AUTOSIZE is
defined so memory is auto-sized at hardware initialization time.
If auto-sizing is not selected, LOCAL_MEM_SIZE must be set to the actual size
of DRAM memory available on the board to ensure all memory is available.
The default fixed RAM size is set to 32MB (see LOCAL_MEM_SIZE in config.h).

Note that LOCAL_MEM_SIZE only controls the amount of memory mapped by the MMU.
It does not control the amount of memory detected and configured by the Bootrom.
The amount of physical memory indicated by the Serial Presence Detect data
determines the memory controller configuration.

.CS

 CPU Space                                   PCI (|) or VME (}) Space
 (default values)

 LOCAL_MEM_LOCAL_ADRS =        -----------------  0x00000000
              (0x00000000)    |      DRAM       |
                              :                 :
                              :                 :
 VME_A32_MSTR_LOCAL =          -----------------  VME_A32_MSTR_BUS
              (0x80000000)    |                 } (0x08000000)
                              | VME A32 space   } .
                              |    256MB        } . Tempe outbound window 1
                              | (0x10000000)    } .
              (0x87ffffff)    |                 } (0x0fffffff)
                              |.................}
                              | Unused A32      }
              (0x8fff0000)    |.................} (0xfb000000)
                              |                 } .
                              |                 } . Tempe outbound window 0
                              |                 } .
              (0x8fffffff)    |                 } (0xfb00ffff)
 VME_A24_MSTR_LOCAL =          -----------------
 VME_A32_MSTR_LOCAL +         |                 } (0x00000000)
 VME_A32_MSTR_SIZE =          | VME A24 space   } .
              (0x90000000)    |    16MB         } . Tempe outbound window 2
                              | (0x01000000)    } .
              (0x90ffffff)    |                 } (0x00ffffff)
 VME_A16_MSTR_LOCAL =          -----------------
 VME_A24_MSTR_LOCAL +         |                 } (0x00000000)
 VME_A24_MSTR_SIZE =          | VME A16 space   } .
              (0x91000000)    |    64KB         } . Tempe outbound window 3
                              | (0x00010000)    } .
              (0x910fffff)    |                 } (0x0000ffff)
                               -----------------
                              :                 :
                              :  Not used       :
                              :  239 MB         :
                              : (0x0ef00000)    :
                              : Available for   :
                              : expansion of    :
                              : VME space       :
 PCI0_MSTR_MEMIO_LOCAL =       ----------------- PCI0_MSTR_MEMIO_BUS =
              (0xa0000000)    |  Bus 0.0        |  PCI0_MSTR_MEMIO_LOCAL =
                              | NonPrefetch PCI |  (0xa0000000)
                              | Memory space    |
                              |      8MB        |
                              | (0x00800000)    |
                              |                 |
 PCI0_MSTR_MEM_LOCAL =         ----------------- PCI0_MSTR_MEM_BUS =
 PCI0_MSTR_MEMIO_LOCAL +      |  Bus 0.0        |  PCI0_MSTR_MEM_LOCAL
 PCI0_MSTR_MEMIO_SIZE =       | Prefetch PCI    |  (0xa0800000)
              (0xa0800000)    | Memory space    |
                              |    8MB          |
                              | (0x00800000)    |
 PCI1_MSTR_MEMIO_LOCAL =       ----------------- PCI1_MSTR_MEMIO_BUS
 PCI0_MSTR_MEM_LOCAL +        |  Bus 1.0        |  PCI1_MSTR_MEMIO_LOCAL
 PCI0_MSTR_MEM_SIZE           | NonPrefetch PCI |  (0xa1000000)
              (0xa1000000)    | Memory space    |
                              |    16MB         |
                              | (0x00800000)    |
 PCI1_MSTR_MEM_LOCAL =         ----------------- PCI1_MSTR_MEM_BUS =
 PCI1_MSTR_MEMIO_LOCAL +      |  Bus 1.0        |  PCI1_MSTR_MEM_LOCAL
 PCI1_MSTR_MEMIO_SIZE =       | Prefetch PCI    |  (0xa2000000)
              (0xa2000000)    | Memory space    |
                              |    8MB          |
                              | (0x00800000)    |
 ISA_MSTR_IO_LOCAL =           ----------------- ISA_MSTR_IO_BUS =
 PCI1_MSTR_MEM_LOCAL +        | Bus 1.0 16-bit  |   (0x00000000)
 PCI1_MSTR_MEM_SIZE           | I/O space       |
              (0xa2800000)    |    64KB         |
                              | (0x00010000)    |
 PCI1_MSTR_IO_LOCAL =          ----------------- PCI1_MSTR_IO_BUS =
 ISA_MSTR_IO_LOCAL +          | Bus 1.0 32-bit  |   (0x00010000)
 ISA_MSTR_IO_SIZE =           |   I/O space     |
              (0xa2810000)    |   8MB - 64KB    |
                              | (0x007f0000)    |
 PCI0_MSTR_IO_LOCAL            ----------------- PCI0_MSTR_IO_BUS =
 PCI1_MSTR_IO_LOCAL +         | Bus 0.0 32-bit  |   (0x00800000)
 PCI1_MSTR_IO_SIZE =          | I/O space       |
              (0xa3000000)    |    8MB          |
                              | (0x00800000)    |
              (0xa3800000)     ----------------- (0x01000000)
                              :                 :
                              : Not used,       :
                              : Available for   :
                              : expansion of    :
                              : PCI spaces      :
                              :                 :
                              :                 :
 MV64360_REG_BASE =            -----------------
              (0xf1000000)    | MV64360         :
                              | internal regs.  :
                              |    64K          :
                              | (0x00010000)    :
              (0xf1010000)     -----------------
                              :     Unused      :
 MV64360_INTEGRATED_SRAM_BASE  -----------------
            = (0xf1100000)    : Integrated SRAM :
                              :     256K        :
                              : (0x00040000)    :
                              : Used for Enet   :
                              : Descriptors     :
              (0xf1140000)     -----------------
                              :     Unused      :
 L3_CACHE_PRIVATE_MEMORY_BASE  -----------------
              (0xf1200000)    :   L3 Cache      :
                              : Private Memory  :
                              :   (OPTIONAL)    :
                              :  1MB - 2MB      :
              (0xf1400000)     -----------------
                              :     Unused      :
                              :                 :
                              :                 :
 MV64360_DEVCS1_BASE_ADDR =    -----------------
              (0xf1d00000)    | Device bank     :
                              |    1MB          :
                              | (0x00100000)    :
                              |                 :
              (0xf1e00000)     -----------------
                              : Not used        :
                              :     34MB        :
                              :                 :
 MV64360_DEVCS0_BASE_ADDR =    -----------------
              (0xf4000000)    | Non-Boot Flash  :
                              |     Bank        :
                              |     64MB        :
                              |                 :
                              |                 :
 MV64360_BOOTCS_BASE_ADDR =    -----------------
              (0xf8000000)    | Boot Flash Bank :
                              |     128MB       :
                              |                 :
                              |                 :
                              |                 :
              (0xffffffff)     -----------------

.CE

PCI Mapped Memory consists of non-prefetchable and prefetchable.  
Each of the MV64360's PCI busses has its own non-prefetchable 
and prefetchable memory spaces.  

The maximum amount of non-prefetchable PCI memory is defined by 
the user as PCI0_MSTR_MEMIO_SIZE and PCI1_MSTR_MEMIO_SIZE.  The 
maximum amount of prefetchable PCI memory is defined by the user 
as PCI0_MSTR_MEM_SIZE and PCI1_MSTR_MEM_SIZE.  This is a one-to-one 
mapping.  The address used on the processor bus is the address used 
on the PCI memory bus.  

PCI Mapped I/O consists of 16 and 32 bit I/O. Only bus 1.0 contains 
16 bit I/O space.  The amount of 16 bit I/O, for bus 1.0, is 
configured by ISA_MSTR_IO_SIZE and defaults to 64KB in size. This is 
not a one-to-one mapping.  This area begins at 0x0 on the PCI I/O 
bus (1.0), and ISA_MSTR_IO_LOCAL on the processor bus.  For Bus 1.0 
the I/O space will start at 0x00000000 on the PCI I/O bus.  For 
Bus 0.0 the I/O space (32-bit only) will not start at 0x00000000, but 
at the top of Bus 1.0's I/O space.  This is to prevent overlap between 
bus addresses.  The amount of 32 bit I/O is configured by 
PCI0_MSTR_IO_SIZE and PCI1_MSTR_IO_SIZE.

The following definitions, found in config.h, control the sizes of
the PCI and VME windows:

.CS
#define VME_A64_MSTR_SIZE       0x00000000      /* 0 MB */
#define VME_A32_MSTR_SIZE       0x10000000      /* 256 MB */
#define VME_A24_MSTR_SIZE       0x01000000      /* 16 MB */
#define VME_A16_MSTR_SIZE       0x00010000      /* 64 KB */
#define PCI0_MSTR_MEMIO_SIZE    0x00800000      /* 8 MB */
#define PCI0_MSTR_MEM_SIZE      0x00800000      /* 8 MB */
#define PCI0_MSTR_IO_SIZE       0x00800000      /* 8 MB */
#define PCI1_MSTR_MEMIO_SIZE    0x01000000      /* 16 MB */
#define PCI1_MSTR_MEM_SIZE      0x00800000      /* 8 MB */
#define ISA_MSTR_IO_SIZE        0x00010000      /* 64 kbytes */
#define PCI1_MSTR_IO_SIZE       0x007f0000      /* 8 MB - 64 KB */
.CE

Be aware that these values alter entries in the sysPhysMemDesc[] 
array (in sysLib.c) and affect the size of the resulting MMU tables 
at the rate of 128:1.  For each 128K of address space, a 1K piece 
of RAM will be used for MMU tables.  If the region is very large, 
modifications to sysLib.c can be made to use BAT (Block Address 
Translation) registers instead of MMU page tables to map the memory.
 
Also, since PCI auto-configuration is performed by the bootroms, 
changing any of these values requires the creation of new bootroms.

.SS "Interrupts"
The system interrupt vector table has 256 entries.  Vectors for the various
devices on the buses are assigned hierarchically as follows:

.TS E
expand;
lf3 lf3
lw16 lw(3.7i) .
.sp .5
Vector#	Assigned to
_
`0x00 - 0x1f'	T{
MV64360 Main Cause (low) interrupts
T}
`0x20 - 0x3f'	T{
MV64360 Main Cause (high) interrupts
T}
`0x40 - 0x5f'	T{
MV64360 GPP interrupts
T}
`0x60 - 0x76'	T{
Tempe (Tsi148) PCI-X to VMe related interrupts.
T}
`0x77 - 0x80'	T{
IPMC (ISA) interrupts
T}
`0x81 - 0xbf'	T{
Unassigned
T}
`0xc0 - 0xff'	T{
Available for user application software.
T}
.TE

As the above table shows, the BSP uses interrupt vectors (numbers)
beginning with 0 and proceeding to numerically higher values.  Interrupts
such as VME bus interrupts which can be assigned a value by the user
should be confined to numbers in the range 0xc0 thru 0xff to avoid
conflict with system required interrupts.  It is suggested that
application software avoid the use of 0xff as an interrupt vector.  Although
a legal number, 0xff often is associated with a nonresponding PCI read and
may cause confusion when debugging.

The interrupt number is numerically equivalent to interrupt vector
and interrupt level is computed in the following way:

If the interrupt is one of the "main cause low" interrupts, the
interrupt number (vector) is numerically equivalent to the associated
bit number in the MV64360 main interrupt cause low register.  If the interrupt
is a "main cause high" interrupt, the interrupt number is equal to the
main interrupt cause high bit number plus 0x20 (hex).

In addition to the interrupts associated with the MV64360 main
interrupt cause register.  The MV64360 contains a 32-bit multi purpose
port (MPP).  The MPP pins can be configured as external interrupt pins
through association with the General Purpose Pins (GPP).  In addition
some of the MPP pins are configured for control and status purposes.
The table below summarizes the MPP pin assignments.

.TS E
expand;
lf3 lf3 lf3 lf3 lf3
l l l l lw(2.6i) .
.sp .5
Pin#	Vector#	I/O	Polarity	Source
_
0	0x40	I	High	COM0/COM1
1	NA	I	High	Unused
2	0x42	I	Low	Abort interrupt
3	0x43	I	Low	RTC & thermostat interrupts (ORed)
4	NA	I	High	Unused
5	0x45	I	High	IPMC761 interrupt
6	0x46	I	Low	Watchdog WDNMI# interrupt
7	NA	I	Low	BCM5421S PHY interrupts (ORed)
8	NA	O	High	PCI 1.0 PMC slot 0 agent grant
9	NA	I	High	PCI 1.0 PMC slot 0 agent request
10	NA	O	High	PCI 1.0 PMC slot 1 agent grant 
11	NA	I	High	PCI 1.0 PMC slot 1 agent request
12	NA	O	High	PCI 1.0 PMC slot 0 grant
13	NA	I	High	PCI 1.0 PMC slot 0 request
14	NA	O	High	PCI 1.0 PMC slot 1 grant	
15	NA	I	High	PCI 1.0 PMC slot 1 request
16	0x50	I	Low	PCI 1.0 - PMC0 INTA# PMC1 INTC#
17	0x51	I	Low	PCI 1.0 - PMC0 INTB# PMC1 INTD#
18	0x52	I	Low	PCI 1.0 - PMC0 INTC# PMC1 INTA#
19	0x53	I	Low	PCI 1.0 - PMC0 INTD# PMC1 INTB#
20	0x54	I	Low	PCI 0.0 - VME LINT0# PMCspan INT 2#
21	0x55	I	Low	PCI 0.0 - VME LINT1# PMCspan INT 3#
22	0x56	I	Low	PCI 0.0 - VME LINT2# PMCspan INT 0#
23	0x57	I	Low	PCI 0.0 - VME LINT3# PMCspan INT 1#
24	NA	O	High	MV64360 SROM init active
25	NA	O	Low	Watchdog WDE# output
26	NA	O	Low	Watchdog WDNMI# output
27	0x5b	I	Low	Reserved future interrupt
28	NA	O	High	Tempe VMEbus grant
29	NA	I	High	Tempe VMEbus request
30	NA	O	High 	PCI6520 (PMCspan bridge) grant	
31	NA	I	High	PCI6520 (PMCspan bridge) request
.TE

For further details, refer to the appropriate board's reference guide.

Each "interrupt" Pin# from the above table is associated with a bit
in a local GPP interrupt cause register.  In addition, one of the four
"main interrupt cause low" bits (24, 25, 26, or 27) is set indicating
that a GPP related interrupt has occurred.

The MV64360 interrupt controller does not directly support interrupt 
priorities.  Software configuration can be performed however, which 
affects the order in which interrupt sources are checked upon 
occurrence of an external processor interrupt.  

The main cause interrupt priority table (ICI_MAIN_INT_PRIORITIES), 
located in config.h, defines the order in which bits in the two 
interrupt main cause registers (low and high) are checked when an 
interrupt occurs.  The table is organized with paired-entries.  The 
first component of the pair is the low or high cause register designation, 
the second component of the pair is the bit number within that register.  
The ordering of the table specifies the order in which bits are checked 
in the cause registers when an interrupt occurs.  Entries at the beginning 
of the table are checked first so to give an interrupt cause bit a higher 
software priority, put it near the beginning of the list.

In a similar manner to the "main interrupt priorities", there is a list 
representing the GPP interrupt priorities (GPP_LOCAL_INT_PRIORITIES), 
also located in config.h.  Each element in the list is a GPP interrupt 
number.  Each number represents a bit position in the GPP interrupt cause 
register.  When a GPP interrupt occurs (via bits 24, 25, or 26 of the 
main interrupt cause register), the GPP interrupt handler will examine 
the GPP local cause bits in the order specified by the list.  By 
rearranging the ordering of bits the priority in which GPP interrupts are 
checked can be modified.

The tempe interrupt can be prioritized as well with TEMPE_INT_PRIORITIES 
in config.h.

The following macros have been provided in order to define the intEnable 
and intDisable routines associated with the user reserved interrupt vectors.  
These are filled with NULL for now but should be filled with a valid 
intEnable and intDisable function pointer when the user actually needs to 
hook functions to these interrupt numbers.  See INT_UTIL_EXTRA inside 
of "mv6100A.h".

.CS
#define USR_INT_ENABLE  NULL    /* Replace with valid intEnable fcn ptr */
#define USR_INT_DISABLE NULL    /* Replace with valid intDisable fcn ptr */

#define INT_USR_EXTRA ,{ USR_INT_NUM_MIN,USR_INT_NUM_MAX, \
                         USR_INT_ENABLE, USR_INT_DISABLE }
.CE

The following table summarizes the default interrupt configuration
for the MVME6100 BSP.

.TS E
expand;
lf3 lf3 lf3 lf3 lf3 lf3
l l l l l lw(3.8i) .
.sp .5
`Interrupt Number'	`Interrupt Source'	`Vector Definition'	`Interrupt Definition'	`Interrupt Handler'
_
0	Reserved	ICI_MICL_INT_NUM_0
1	Device Bus Error	ICI_MICL_INT_NUM_1
2	DMA Error	ICI_MICL_INT_NUM_2	IDMA_ERROR_INT	sysMv64360DmaHandleErrInt
3	CPU Error	ICI_MICL_INT_NUM_3
4	IDMA Chan 0 Completion	ICI_MICL_INT_NUM_4	IDMA_CHAN0_COMPLETION_INT	sysMv64360DmaHandleInt
5	IDMA Chan 1 Completion	ICI_MICL_INT_NUM_5	IDMA_CHAN1_COMPLETION_INT	sysMv64360DmaHandleInt
6	IDMA Chan 2 Completion	ICI_MICL_INT_NUM_6	IDMA_CHAN2_COMPLETION_INT	sysMv64360DmaHandleInt
7	IDMA Chan 3 Completion	ICI_MICL_INT_NUM_7	IDMA_CHAN3_COMPLETION_INT	sysMv64360DmaHandleInt
8	Timer 0	ICI_MICL_INT_NUM_8	TMR_CNTR0_INT_VEC	sysMv64360TmrCntrInt
9	Timer 1	ICI_MICL_INT_NUM_9		sysMv64360TmrCntrInt
10	Timer 2	ICI_MICL_INT_NUM_10		sysMv64360TmrCntrInt
11	Timer 3	ICI_MICL_INT_NUM_11		sysMv64360TmrCntrInt
12	PCI0	ICI_MICL_INT_NUM_12		
13	SRAM Parity Error	ICI_MICL_INT_NUM_13		
14	Gigabit Ethernet Error	ICI_MICL_INT_NUM_14	GE_INT_VEC_ENET_UNIT_ERR	
15	Serial Ports Error	ICI_MICL_INT_NUM_15		
16	PCI1	ICI_MICL_INT_NUM_16		
17	DRAM ECC Error	ICI_MICL_INT_NUM_17		
18	WD Reached its NMI Threshold	ICI_MICL_INT_NUM_18		
19	WD Reached Terminal Count	ICI_MICL_INT_NUM_19		
20	PCI0 Inbound	ICI_MICL_INT_NUM_20		
21 	PCI0 Outbound	ICI_MICL_INT_NUM_21		
22	PCI1 Inbound	ICI_MICL_INT_NUM_22		
23	PCI1 Outbound	ICI_MICL_INT_NUM_23		
24	CPU 1 GPP[7:0]	ICI_MICL_INT_NUM_24		
25	CPU 1 GPP [15:8]	ICI_MICL_INT_NUM_25		
26	CPU 1 GPP [23:16]	ICI_MICL_INT_NUM_26		
27	CPU 1 GPP [31:24]	ICI_MICL_INT_NUM_27		
28	Summary of CPU 1 Doorbell Cause Register	ICI_MICL_INT_NUM_28		
29 	Reserved	ICI_MICL_INT_NUM_29		
30	Reserved	ICI_MICL_INT_NUM_30		
31	Reserved	ICI_MICL_INT_NUM_31		
32	Gigabit Ethernet 0 Summary	ICI_MICH_INT_NUM_0		
33	Gigabit Ethernet 1 Summary	ICI_MICH_INT_NUM_1		
34	Gigabit Ethernet 2 Summary	ICI_MICH_INT_NUM_2		
35	Reserved	ICI_MICH_INT_NUM_3		
36	SDMA0	ICI_MICH_INT_NUM_4		
37	TWSI Interrupt	ICI_MICH_INT_NUM_5		
38	SDMA1	ICI_MICH_INT_NUM_6		
39	BRG	ICI_MICH_INT_NUM_7		
40	MPSC0	ICI_MICH_INT_NUM_8		
41	MPSC1	ICI_MICH_INT_NUM_9		
42	Gigabit Ethernet 0 Rx Summary	ICI_MICH_INT_NUM_10		
43	Gigabit Ethernet 0 Tx Summary	ICI_MICH_INT_NUM_11		
44	Gigabit Ethernet 0 Misc Summary	ICI_MICH_INT_NUM_12		
45	Gigabit Ethernet 1 Rx Summary	ICI_MICH_INT_NUM_13		
46	Gigabit Ethernet 1 Tx Summary	ICI_MICH_INT_NUM_14		
47 	Gigabit Ethernet 1 Misc Summary	ICI_MICH_INT_NUM_15		
48	Gigabit Ethernet 2 Rx Summary	ICI_MICH_INT_NUM_16		
49	Gigabit Ethernet 2 Tx Summary	ICI_MICH_INT_NUM_17		
50 	Gigabit Ethernet 3 Misc Summary	ICI_MICH_INT_NUM_18		
51	Reserved	ICI_MICH_INT_NUM_19		
52	Reserved	ICI_MICH_INT_NUM_20		
53	Reserved	ICI_MICH_INT_NUM_21		
54	Reserved	ICI_MICH_INT_NUM_22		
55	Reserved	ICI_MICH_INT_NUM_23		
56	CPU 0 GPP[7:0]	ICI_MICH_INT_NUM_24	GPP_INT_VEC_0_7	gppIntHandler
57	CPU 0 GPP[15:8]	ICI_MICH_INT_NUM_25	GPP_INT_VEC_8_15	gppIntHandler
58	CPU 0 GPP[23:16]	ICI_MICH_INT_NUM_26	GPP_INT_VEC_16_23	gppIntHandler
59	CPU 0 GPP[31:24]	ICI_MICH_INT_NUM_27	GPP_INT_VECL_24_31	gppIntHandler
60	Summary of CPU 0 Doorbell Cause Register	ICI_MICH_INT_NUM_28		
61	Reserved	ICI_MICH_INT_NUM_29		
62	Reserved	ICI_MICH_INT_NUM_30		
63	Reserved	ICI_MICH_INT_NUM_31		
64	COM0/COM1	GPP_IC_INT_NUM_0	COM0_INT_VEC,COM1_INT_VECT	i8250Int
65	Unused	GPP_IC_INT_NUM_1		
66	Abort Interrupt	GPP_IC_INT_NUM_2		
67	RTC and Thermostat Interrupts	GPP_IC_INT_NUM_3	RTC_INT_VEC	m48t37Int
68	Unused	GPP_IC_INT_NUM_4		
69	IPMC Interrupts	GPP_IC_INT_NUM_5	PIB_INT_VEC	sysIbcIntHandler
70	MV64360 WDNMI# Interrupt	GPP_IC_INT_NUM_6		
71	BCM5421S PHY Interrupts	GPP_IC_INT_NUM_7		
72	PCI Bus 1 PMC Slot 0 Agent Grant	GPP_IC_INT_NUM_8		
73	PCI Bus 1 PMC Slot 0 Agent Request	GPP_IC_INT_NUM_9		
74	PCI Bus 1 PMC Slot 1 Agent Grant	GPP_IC_INT_NUM_10		
75	PCI Bus 1 PMC Slot 1 Agent Request	GPP_IC_INT_NUM_11		
76	PCI Bus 1 PMC Slot 0 Grant	GPP_IC_INT_NUM_12		
77	PCI Bus 1 PMC Slot 0 Request	GPP_IC_INT_NUM_13		
78	PCI Bus 1 PMC Slot 1 Grant	GPP_IC_INT_NUM_14		
79	PCI Bus 1 PMC Slot 1 Request	GPP_IC_INT_NUM_15		
80	PCI Bus 1 Interrupts PCI-PMC0 INTA# (devices 4 & 5), PMC1 INTC# (devices 6 & 7)	GPP_IC_INT_NUM_16	PCI_PMC0_INTA,PCI_PMC1_INTC
81	PCI Bus 1 Interrupts PCI-PMC0 INTB# (devices 4 & 5), PMC1 INTD# (devices 6 & 7)	GPP_IC_INT_NUM_17	PCI_PMC0_INTB,PCI_PMC1_INTD
82	PCI Bus 1 Interrupts PCI-PMC0 INTC# (devices 4 & 5), PMC1 INTA# (devices 6 & 7)	GPP_IC_INT_NUM_18	PCI_PMC0_INTC,PCI_PMC1_INTA
83	PCI Bus 1 Interrupts PCI-PMC0 INTD# (devices 4 & 5), PMC1 INTB# (devices 6 & 7)	GPP_IC_INT_NUM_19	PCI_PMC0_INTD,PCI_PMC1_INTB
84	PCI Bus 0 Interrupt PCI-VME INT0, Tempe LINT0# (device 5), PMCSpan INT 2#	GPP_IC_INT_NUM_20	TEMPE_INT_LVL,PCI_VME_LINT0,PCI_BR_INT2	sysTempeVmeIntr
85	PCI Bus 0 Interrupt PCI-VME INT1, Tempe LINT1# (device 5), PMCSpan INT 3#	GPP_IC_INT_NUM_21	PCI_VME_LINT1,PCI_BR_INT3
86	PCI Bus 0 Interrupt PCI-VME INT2, Tempe LINT2# (device 5), PMCSpan INT 0#	GPP_IC_INT_NUM_22	PCI_VME_LINT2,PCI_BR_INT0
87	PCI Bus 0 Interrupt PCI-VME INT3, Tempe LINT3# (device 5), PMCSpan INT 1#	GPP_IC_INT_NUM_23	PCI_VME_LINT3,PCI_BR_INT1
88	MV64360 SROM Initialization Active (InitAct)	GPP_IC_INT_NUM_24		
89	Watchdog Timer Expired Output (WDE#)	GPP_IC_INT_NUM_25		
90	Watchdog Timer NMI Output (WDNMI#)	GPP_IC_INT_NUM_26		
91	Reserved	GPP_IC_INT_NUM_27		
92	Tempe ASIC (VMEBus) Grant	GPP_IC_INT_NUM_28		
93	Tempe ASIC (VMEBus) Request	GPP_IC_INT_NUM_29		
94	PCI6520 (PMCSpan Bridge) Grant	GPP_IC_INT_NUM_30		
95	PCI6520 (PMCSpan Bridge) Request	GPP_IC_INT_NUM_31		
96	Tempe(Tsi148) IRQ 1	TEMPE_INT_NUM_IRQ1		
97	Tempe(Tsi148) IRQ 2	TEMPE_INT_NUM_IRQ2		
98	Tempe(Tsi148) IRQ 3	TEMPE_INT_NUM_IRQ3		
99	Tempe(Tsi148) IRQ 4	TEMPE_INT_NUM_IRQ4		
100	Tempe(Tsi148) IRQ 5	TEMPE_INT_NUM_IRQ5		
101	Tempe(Tsi148) IRQ 6	TEMPE_INT_NUM_IRQ6		
102	Tempe(Tsi148) IRQ 7	TEMPE_INT_NUM_IRQ7		
103	Tempe(Tsi148) AC Fail	TEMPE_INT_NUM_ACFL		
104	Tempe(Tsi148) System Fail	TEMPE_INT_NUM_SYSFL		
105	Tempe(Tsi148) Interrupt Acknowledge	TEMPE_INT_NUM_IACK		
106	Tempe(Tsi148) VMEBus IRQ Edge	TEMPE_INT_NUM_VIEEN		
107	Tempe(Tsi148) VMEBus Error	TEMPE_INT_NUM_VERR		
108	Tempe(Tsi148) PCI Bus Error	TEMPE_INT_NUM_PERR		
109	Tempe(Tsi148) Mailbox 0	TEMPE_INT_NUM_MB0		sysMailboxInt		
110	Tempe(Tsi148) Mailbox 1	TEMPE_INT_NUM_MB1		
111	Tempe(Tsi148) Mailbox 2	TEMPE_INT_NUM_MB2		
112	Tempe(Tsi148) Mailbox 3	TEMPE_INT_NUM_MB3		
113	Tempe(Tsi148) Location Monitor 0	TEMPE_INT_NUM_LM0		
114	Tempe(Tsi148) Location Monitor 1	TEMPE_INT_NUM_LM1		
115	Tempe(Tsi148) Location Monitor 2	TEMPE_INT_NUM_LM2		
116	Tempe(Tsi148) Location Monitor 3	TEMPE_INT_NUM_LM3		
117	Tempe(Tsi148) DMA 0	TEMPE_INT_NUM_DMA0		
118	Tempe(Tsi148) DMA 1	TEMPE_INT_NUM_DMA1		
119	Reserved - ISA			
120	Reserved - ISA			
121	Reserved - ISA			
122	IPMC Serial	COM3_INT_LVL	COM3_INT_LVL	i8250Int
123	IPMC Serial	COM2_INT_LVL	COM2_INT_LVL	i8250Int
124	Reserved - ISA			
125	Reserved - ISA			
126	Reserved - ISA			
127	Reserved - ISA			
128	IPMC Serial	Z85230_INT_LVL	Z85230_INT_LVL	z8530Int
.TE

Dealing with VME Bus IRQx interrupts:

The following code fragments illustrate how to connect interrupt handlers 
for VME bus interrupts IRQ1 and IRQ2 to vectors 0xc0 and 0xc2 respectively 
and how to generate the associated VME bus interrupts.

Notice the following: We are using vectors 0xc0 and 0xc2 which are contained 
within the user reserved set of interrupt vectors.  This is because the 
actual interrupt vectors associated with IRQ1, IRQ2 ... IRQ7 are user 
selectable and we don't want to interfere with vector numbers which are already
specifically assigned.  Also we are confining ourselves to even numbered 
vectors (not using vector 0xc1 for example) because older boards such as the 
MVME5100 (which use the Universe PCI-VME bridge chip) can only deal with even 
numbered bus interrupt vectors.

The functions vmeIrq1Intr() and vmeIrq2Intr() are the actual interrupt service 
routines associated with IRQ1 and IRQ2 interrupts and are not presented below.

Connecting the interrupts:

.CS
    void irqConnect()
        {

        /* Connect IRQ1 interrupt to vector 0xc0 */

        printf("intConnect(0xc0,vmeIrq1Intr,0xc0);\n");
        if (intConnect(0xc0,vmeIrq1Intr,0xc0) == ERROR)
            {
            printf("Could not intConnect(0xc0,vmeIrq1Intr,0xc0)\n\r");
            }
        printf("sysIntEnable(1);\n\r");
        sysIntEnable(1);        /* Enable IRQ1 interrupt */

        /* Connect IRQ2 interrupt to vector 0xc2 */

        printf("intConnect(0xc2,vmeIrq2Intr,0xc2);\n");
        if (intConnect(0xc2,vmeIrq2Intr,0xc2) == ERROR)
            {
            printf("Could not intConnect(0xc2,vmeIrq2Intr,0xc2)\n");
            }
        printf("sysIntEnable(2);\n");
        sysIntEnable(2);        /* Enable IRQ2 interrupt */
        }
.CE

The following code fragment works in conjunction with the previous fragment 
which connected the interrupts.  Once the interrupts are connected via the 
irqConnect() routine, the functions vmeIrq1Intr() and vmeIrq2Intr() can be 
activated by actually generating the bus interrupts with the irqGen() routine 
which follows.  Notice that the sysBusIntGen() takes two parameters - the 
first is the IRQ number (1 thru 7) and the second is the actual vector number 
which was used in the previous routine intConnect() calls.

Generating the interrupts:

.CS
    void irqGen()
        {

        /* Generate Bus Interrupt IRQ1 via vector 0xc0 */

        printf("sysBusIntGen(1,0xc0);\n");

        /*
         * The interrupt generated via the call which follows
         * will ultimately cause vmeIrq1Intr() to be activated.
         */

        sysBusIntGen(1,0xc0);

        printf("sysBusIntGen(2,0xc2);\n");

        /*
         * The interrupt generated via the call which follows
         * will ultimately cause vmeIrq2Intr() to be activated.
         */

        sysBusIntGen(2,0xc2);
        }
.CE

.SS "PCI Auto-Configuration"
To simplify the addition of PCI-based add-in cards, the BSP provides a PCI
auto-configuration library. 

The auto-configuration is called from sysHwInit to discover and configure 
the installed PCI devices and bridges. Device configuration includes the 
following PCI information:

.IP "Base Address Registers (BARs)"
Space in the address map is dynamically allocated to each valid BAR detected.
Allocation pools are maintained for the following PCI address spaces:

16-Bit PCI I/O

32-Bit PCI I/O

PCI Memory I/O (non-prefetchable memory)

PCI Memory (pre-fetchable memory)


.IP "Interrupt Routing"
The correct interrupt vector number is placed in the intLine register of the
device's PCI header. To connect to the device's interrupt, simply call
intConnect with the value read from intLine.

.IP "PCI Header Completion"
The PCI auto-configuration library fills in the remainder of the PCI header as
follows:

Cache Line Size = _CACHE_ALIGN_SIZE/4

Latency Timer = PCI_LAT_TIMER

Command Register = I/O enabled, Memory enabled and Bus Master enabled.

.SS "IPMC Devices"

Note that the hardware and BSP must match in regards to the IPMC module.
Do not attempt to define IPMC features without installing an IPMC module.
IPMC features should be defined when an IPMC is installed.  IPMC support
is controlled with the definition INCLUDE_IPMC found in config.h.  IPMC
support is disabled by default.  To enable IPMC support change #undef
INCLUDE_IPMC to #define INCLUDE_IPMC.  See the Jumpers & Switches section 
for details on how jumpers should be configured for IPMC support.

The Marvell Discovery 2 controller utilized on the MVE6100 only supports 
IDSEL connection to PCI signals AD[31:16] when performing address stepping 
for PCI configuration cycles.  However, the "old" IPMC761/712 (01-W3559F0xB/
1065-08-001-0201) utilize a Winbond controller chip to implement serial and 
parallel ports and the IDSEL line for the Winbond chip is connected to PCI 
signal AD[11] on PMC connector pin P11-48.  Therefore, the MVME6100 doesn't 
currently support the operation of the serial and parallel ports on the 
"old" IPMC761/712.  Note that the MVME6100 does currently support the SCSI 
port on the "old" IPMC761/712 since SCSI has its own private IDSELA pin on 
PMC connector pin P12-25.

The "new" IPMC761/712s (01-W3559F0xC/01-W3909F0xA) support the serial and
parallel ports by implementing a switch on the IPMC712 and IPMC761 boards 
that allows either PCI AD[11] on PMC connector pin P11-48 or IDSELB on PMC 
connector P12-34 to connect to the IDSEL line for the Winbond chip.  Connection 
to AD[11] will be selected when using the IPMC 712/761 with MCG VME boards 
released prior to the MVME6100.  Connection to IDSELB will be selected when 
using the IPMC712/761 with the MVME6100 board and all boards after it.

.SS "Serial Configuration"
The MVME6100's two asynchronous serial interfaces are provided by the 
EXAR ST16C554D Quad UART device connected to the MV64360's device controller
bus.  COM0 is wired to the front panel RJ45 connector.  COM1 is wired to a 
ten-pin 0.100" planar header located on the MVME6100.  An optional IPMC761 
or IPMC712 PMC and transition module will add four additional interfaces 
available through the rear panel 761/712 transition board.  By default 
COM2 and COM3 are defined as the SuperI/O serial ports and COM4 and COM5 
are the Z8536/Z85230 serial ports.

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"

SCSI is implemented via the IPMC module.  To include SCSI support change
#undef INCLUDE_IPMC (INCLUDE_SCSI) , in config.h, to #define INCLUDE_IPMC.
Enabling SCSI support through the INCLUDE_IPMC definition in config.h will 
add the following support:

.CS
#ifdef INCLUDE_SCSI
#   define INCLUDE_SCSI2        /* Use SCSI2 library, not SCSI1 */
#   define INCLUDE_DISK_UTIL    /* DOS Utilities */
#   define INCLUDE_SCSI_BOOT    /* include ability to boot from SCSI */
#   define INCLUDE_DOSFS        /* file system to be used */
#   define SCSI_AUTO_CONFIG     /* scan bus for devices on startup */
#   define SCSI_WIDE_ENABLE     /* enable wide SCSI, 16-bit data xfrs */
#   undef SYS_SCSI_CONFIG       /* call sysScsiConfig in sysScsi.c */
#   define INCLUDE_SYM_895      /* Symbios 895 support */
#   define ULTRA2_SCSI          /* Ultra2 SCSI */
#endif /* INCLUDE_SCSI */
.CE

In order for the SYM53C895A to perform to its full potential - of
wide ultra2 SCSI - the driver needs to be configured using the routine
scsiTargetOptionsSet().  The following is an example showing how to
configure the driver, the code would be added to sysScsiConfig() in
sysScsi.c:

.CS
    scsiId = SCSI_SET_OPT_ALL_TARGETS;
    which = ( SCSI_SET_OPT_XFER_PARAMS | SCSI_SET_OPT_WIDE_PARAMS);

    options.minPeriod = 6;
    options.maxOffset = 255;
    options.xferWidth = 1;

    if (scsiTargetOptionsSet (pSysScsiCtrl, scsiId, &options, which)
        == ERROR)
        {
        printf ("Could not set target option parameters\n");
        return (ERROR);
        }
.CE

In order for sysScsiConfig() to execute during initialization 
#define SCSI_AUTO_CONFIG, in config.h, must be changed to
#undef SCSI_AUTO_CONFIG, and #undef SYS_SCSI_CONFIG must be
changed to #define SYS_SCSI_CONFIG.

We found that in order to boot from a SCSI hard drive
(VxWorks 5.5.1), the following changes needed to be made in 
bootConfig.c (located in the config/all subdirectory):

In the scsiLoad() routine remove the following section of code:

.CS
pCbio = dpartDevCreate((CBIO_DEV_ID) pScsiBlkBootDev,
         		NUM_PARTITIONS_DISK_BOOT,
               		usrFdiskPartRead);

if (NULL == pCbio)
    {
    printErr ("scsiLoad: dpartDevCreate returned NULL.\n");
    return (ERROR);
    }

/* initialize the boot block device as a dosFs device named <bootDir> */

if (ERROR == dosFsDevCreate(bootDir,
            dpartPartGet(pCbio,PARTITION_DISK_BOOT),
	    20, NONE))
   {
    printErr ("scsiLoad: dosFsDevCreate returned ERROR.\n");
    return (ERROR);
    }
.CE

And, replace it with:

.CS
if (dosFsDevInit (bootDir, pScsiBlkBootDev, NULL) == NULL)
    {
    printErr ("dosFsDevInit failed.\n");
    return (ERROR);
    }
.CE

.SS "MV64360 DMA Configuration"

To enable support for the MV64360 DMA controller, change the 
#undef INCLUDE_MV64360_DMA, in config.h, to #define INCLUDE_MV64360_DMA.  

Burst limit choices for the IDMA controller are 8, 16, 32, 64 
or 128 bytes.  The higher the burst limit, the faster the transfer.  The 
DMA buffer address should be aligned to burst limit granularity for best 
performance.  Since snooping is working on a cache line basis, any access 
to SDRAM interface unit that requires snooping must not cross the cache 
line boundary.  Therefore, if snooping is turned on, the IDMA burst limit 
must not exceed 32 bytes.

IDMA snooping is enabled by default to achieve cache coherency for IDMA on 
the MV64360.  Be aware that turning off IDMA snooping will result in 
disabling snoop transaction even in the cache coherent regions.  Undef 
IDMA_SNOOP_ON, only if snoop transaction is not necessary, in order to 
achieve better IDMA performance.

Turning off IDMA snooping will destroy data integrity if the destination 
buffer resides in a cacheable region of memory which requires cache coherency.

The following is taken from config.h:

.CS
#   define   IDMA_SNOOP_ON

#   ifdef   IDMA_SNOOP_ON
#       define  IDMA_DST_BURST_LIMIT IDMA_CNTL_DBURST_LIMIT_32B
#       define  IDMA_SRC_BURST_LIMIT IDMA_CNTL_SBURST_LIMIT_32B
#   else
#       define  IDMA_DST_BURST_LIMIT IDMA_CNTL_DBURST_LIMIT_128B
#       define  IDMA_SRC_BURST_LIMIT IDMA_CNTL_SBURST_LIMIT_128B
#   endif   /* IDMA_SNOOP_ON */
.CE

The DMA initialization routine must be called once before any other DMA routine 
is invoked. Failure to do so will result in status error returns from all other 
DMA functions. This initialization is handled by sysHwInit2 whenever 
INCLUDE_MV64360_DMA is defined in config.h.

To start DMA for a particular channel, invoke the start routine with the 
desired addresses, byte count and transfer attributes. These parameters are 
specified via separate descriptor and attribute structures defined in sysDma.h.

DMA is supported from DRAM to DRAM, DRAM to PCI, PCI to DRAM, and PCI to PCI 
by appropriate selection of the source and destination addresses supplied 
in the DMA descriptor. The starting address for either or both source and 
destination may be optionally held (no increment) during the transfer.

The user interrupt handler, if provided, must conform to the rules for 
standard VxWorks interrupt service routines. It is called with a current 
channel parameter during handling of both normal completion and error 
interrupts. Since this routine is called by the driver interrupt handlers, the 
user routine is not responsible for clearing interrupts. Interrupts are 
cleared after the user routine returns to the driver's handler.

.CS
sysDmaStart - Configure and start the DMA controller.
.CE

This function sets up the DMA controller for a block mode or chain mode DMA 
transfer. The user must fill out the DMA descriptor and DMA attribute 
parameters according to device specifications.  Transfer options are set 
according to dmaAttrib. The descriptor registers are programmed according 
to dmaDesc. Interrupts for the given channel are enabled and the transfer 
is initiated.

Both block mode and chain mode are supported.  To invoke chain mode, 
nextDescPtr of dmaDesc must have a nonzero value.  Otherwise, if nextDescPtr 
of dmaDesc is zero, the function sets up the DMA transfer in block mode.

The user has three options for being notified of the completion of a DMA 
transaction depending on the contents of the userHandler field in the 
dmaAttrib parameter. The first is to have the DMA start routine wait for 
the done interrupt. The start routine then returns with DMA status 
(dmaDesc.userHandler == WAIT_FOREVER).  The second option is to provide a 
user routine for the DMA interrupt handler to call upon completion 
(dmaDesc.userHandler == user routine). The routine must follow the rules 
for executing inside an ISR by not calling waits, prints, etc. The sysDmaStart 
routine returns immediately so the user task can proceed with its execution. 
The third option is for no user interrupt handler, but still allows 
sysDmaStart to return immediately (dmaDesc.userHandler == 0). In the latter 
two cases sysDmaStatus can be called later to get the DMA results.

Status is returned by sysDmaStart when the dmaStatus parameter is not NULL 
and when using dmaDesc.userHandler == WAIT_FOREVER or by calling sysDmaStatus 
directly.  In any case, the status should be interpreted using the 
appropriate device status structure defined in mv64360Dma.h.

This routine returns OK, if the channel started successfully, or ERROR is 
returned if the driver is not initialized, or the channel is invalid or busy, 
or a parameter is invalid.

.CS
sysDmaStatus - Read and return DMA status.
.CE

For the given channel, the routine will provide status information by placing 
it into a user provided DMA status structure reference by dmaStatus. This 
structure should match the device status structure defined in mv64360Dma.h. 
and be interpreted as such. Status information can provide an indication of 
successful DMA completion, in-progress, or error conditions along with 
supporting diagnostic information. See the device specific function call for 
details in sysMv64360Dma.c.

This routine returns OK if the dmaStatus is valid, or ERROR if the driver is 
not initialized, or the channel is invalid or the dmaStatus reference is null. 
The dmaStatus contents will not be valid.

.CS
sysDmaAbort - Initiate an abort of the current DMA operation.
.CE

This routine will initiate an abort of the current DMA operation for the given 
valid channel.  The channel must actually be busy.  The transfer is aborted 
after the current minimum transfer segment completes.  The function waits for 
the abort to complete before returning.

This routine returns OK if the abort succeeded, and ERROR if the driver is not 
initialized or the channel number is invalid or the channel is already stopped.

.CS
sysDmaPause - Initiate a pause of the current DMA operation.
.CE

This routine will initiate a pause of the current DMA operation for the given 
valid channel.  If the channel is busy, pause the channel.  The channel will 
pause after completing any current transfer segment in progress.

This routine returns OK if the pause succeeded, and ERROR if the driver is not 
initialized, or the channel is invalid, or the channel was already paused.

.CS
sysDmaResume - Resume a previously paused DMA operation.
.CE

This routine resumes a previously paused DMA operation for a given valid 
channel.  The channel is verified to not be active and that there are bytes 
remaining to transfer (a non-zero byte count). If so the channel is 
activated to continue with the transfer.

This routine returns OK if the resume succeeded, and ERROR is returned if the 
driver is not initialized, or the channel is invalid, or the channel is not 
paused (paused meaning inactive with a non-zero byte count descriptor register).

The following structure defines the basic DMA transfer parameters:

.CS
typedef struct sysDmaDesc
    {
    UINT32 byteCount;      /* Number of bytes to transfer */
    UINT32 sourceAddr;     /* Starting address of source data */
    UINT32 destAddr;       /* Starting address of destination */
    UINT32 nextDescPtr;    /* Next descriptor pointer */
    } DMA_DESCRIPTOR;
.CE

The following structure defines user controlled attributes for a given DMA 
transfer. The userHandler routine provides an optional hook for notification 
upon completion of interrupt occurrence. The value of this field provides 
three notification options based on its value: Wait in the DMA start routine 
until the completion interrupt occurs (userHandler == WAIT_FOREVER); return 
immediately from the start routine, but call a user routine from the interrupt
handler upon DMA completion (userHandler == user routine); or return 
immediately without a user interrupt handler call (userHandler == 0).

.CS
typedef void (*HANDLER)(UINT32 chan);
typedef struct sysDmaAttrib
    {
    BOOL    holdSourceAddr;   /* Do not increment the source address */
    BOOL    holdDestAddr;     /* Do not increment the destination address */
    HANDLER userHandler;      /* User defined interrupt completion routine */
    } DMA_ATTRIBUTES;
.CE

.SS "Optimal MV64360 DMA Performance"

In order to configure the BSP for optimal MV64360 DMA performance the 
following must be done:

(1) Disable the Caches

The caches are disabled by #undef'ing INCLUDE_CACHE_SUPPORT, INCLUDE_CACHE_L2, 
and INCLUDE_CACHE_L3 in config.h.

(2) Destination Burst Size

The Dest Burst Size is configured in config.h with the define 
IDMA_DST_BURST_LIMIT.  It can be set to either IDMA_CTRL_DBURST_LIMIT_32B or 
IDMA_CTRL_DBURST_LIMIT_128B.  The setting of the burst limit is controlled 
by the IDMA_SNOOP_ON definition in config.h.  If IDMA_SNOOP_ON is defined 
then the burst limit will be set to 32 bytes, for cache coherency.  If not 
defined the burst limit will be set to 128 bytes.  For optimal performance 
the burst limit must be set to 128 bytes. See "MV64360 DMA Support" for more 
information.

(3) Source Burst Size

The Src Burst Size is configured in config.h with the define 
IDMA_SRC_BURST_LIMIT.  It can be set to either IDMA_CTRL_SBURST_LIMIT_32B or 
IDMA_CTRL_SBURST_LIMIT_128B.  The setting of the burst limit is controlled 
by the IDMA_SNOOP_ON definition in config.h.  If IDMA_SNOOP_ON is defined 
then the burst limit will be set to 32 bytes, for cache coherency.  If not 
defined the burst limit will be set to 128 bytes.   For optimal performance 
the burst limit should be set to 128 bytes.  See "MV64360 DMA Support" for 
more information.

.SS "Tempe (Tsi148) DMA Configuration"

The Tempe PCI-VME bridge chip contains a DMA controller.  This BSP provides 
a driver for that DMA controller.  No special configuration switches need 
to be modified to configure Tempe (Tsi148) DMA support. 

The following is a synopsis of the driver routines:

.CS
sysTempeDmaStart()
.CE

A Tempe DMA is started by calling sysTempeDmaStart with a channel specified, 
DMA descriptor structure, DMA attribute structure and a DMA status structure 
which will receive the status of the DMA.

.CS
  STATUS sysTempeDmaStart
      (
      UINT32 chan,                    /@ channel number (0 or 1) @/
      TEMPE_DMA_DESCRIPTOR * dmaDesc, /@ descriptor @/
      TEMPE_DMA_ATTRIBUTES * dmaAttr, /@ attributes @/
      TEMPE_DMA_STATUS     * dmaStat  /@ DMA status, (or NULL) @/
      )
.CE

The channel parameter (chan) must contain either 0 or 1, depending on which 
channel the DMA is to run.  The two channels can be run concurrently.

The DMA descriptor parameter (dmaDesc) specifies the information which can 
change for each link-list block.  This includes a source address, destination 
address, byte count and VME bus attributes along with a 'next' pointer which
optionally points to another dmaDesc type parameter.  If the 'next' field 
of the DMA descriptor parameter is NULL (0) then a "direct mode" DMA 
transaction is performed.  If the 'next' field is non-NULL then it must point 
to another valid DMA descriptor parameter which will be the second in a 
linked-list chain.  The second DMA descriptor in a linked-list chain can 
point to yet another DMA descriptor and so on, the last DMA descriptor in the 
chain will contain NULL in its 'next' pointer.  A linked-list chain can be as 
long as desired.

The DMA Attribute parameter (dmaAttr) specifies the information needed to 
program the DMA control register for the particular channel.  Once the 
attributes are set for a particular call, they remain unaltered for the 
duration of the DMA operation, even if it is a chained-mode operation.  The 
information in this parameter includes bus usage information (DMA block 
sizes, back-off timers, flush on aborted read options) as well as an 
optional 'userHandler' field which points to a user-written function which 
gains control when the DMA finishes.  Use of the 'userHandler' field is 
optional, if it contains NULL then the sysTempeDmaStart() function returns 
to the caller immediately after the DMA is initiated, allowing for 
concurrent processing while the DMA is underway.  If the 'userHandler' field 
contains WAIT_FOREVER then the sysTempeDmaStart() function will not return 
until the DMA operation has completed (successfully or with an error).  
While the function is waiting for the DMA to finish it is blocked on a 
semaphore so that other tasks may run during DMA operation.  If 'userHandler' 
contains something other than NULL or WAIT_FOREVER, it is assumed to be a 
pointer to a user written function.  In this case sysTempeDmaStart() will 
return immediately after initiating the DMA and the user written handler 
will be called after the DMA completes.  The user written handler will be 
operating in the context of an interrupt service routine so it is important 
that this function not perform any blocking calls.  It should execute as 
quickly as possible since other interrupt service processing is held off 
while it executes.

The DMA status parameter will be returned containing information about the 
current state of the DMA, including current source address, current 
destination address, current link address, decrementing byte counter and a 
current upper and lower time base register capture.  The status information 
is returned immediately after the DMA is started and can be subsequently 
obtained via calls to sysTempeDmaStatus().  The dmaStat parameter can be 
NULL when calling sysTempeDmaStart(), in which case no status is returned.

.CS
sysTempeDmaStatus()
.CE

A Tempe DMA status can be obtained by calling sysTempeDmaStatus with a channel 
specified and a structure parameter which receives the status data.

.CS
  STATUS sysTempeDmaStatus
      (
      UINT32       chan,               /@ Channel to get status @/
      TEMPE_DMA_STATUS *dmaStatus      /@ Current status of channel @/
      )
.CE

The status is read from the Tempe chip at the time of the call to 
sysTempeDmaStatus().  Included in the status are values of the channels 
status register, current source address, current destination address, current 
link address, remaining byte count and an upper and lower timebase register 
value.

.CS
sysTempeDmaAbort()
.CE

An active DMA operation can be terminated by a call to sysTempeDmaAbort().  
The only parameter is a channel number which must be 0 or 1.  Once a channel 
has been aborted it cannot be resumed.  A new call to sysTempeDmaStart() 
must be made to initiate a new DMA.

.CS
  STATUS sysTempeDmaAbort
      (
      UINT32   chan             /@ Channel to be aborted @/
      )
.CE

.CS
sysTempeDmaPause()
.CE

A link-list DMA operation can be paused after the current link-list descriptor 
portion of the DMA has completed and before the next descriptor in the list 
begins processing.  The only parameter is the channel which is to be paused.

.CS
  STATUS sysTempeDmaPause
      (
      UINT32   chan             /@ Channel to be paused @/
      )
.CE

After the channel has been paused, it can be "resumed" via a call to 
sysTempeDmaResume().

.CS
sysTempeDmaResume()
.CE

A link-list DMA operation which has previously been paused can be resumed 
with a call to sysTempeDmaResume().  A channel number is the only required 
parameter.

.CS
  STATUS sysTempeDmaResume
      (
      UINT32   chan             /@ Channel to be resumed @/
      )
.CE

Below is a template for setting up a call to sysTempeDmaStart(). See 
"tempe.h" for a breakdown of these structures.

.CS
TEMPE_DMA_DESCRIPTOR dmaDescSample =
    {
    0x01000000,        /@ Source Address 'sourceAddr' @/
    0x81000000,        /@ Destination Address 'destAddr'  @/
    0x00010000,        /@ Number of bytes to transfer 'byteCount' @/
    0,                 /@ 2eSST Broadcast select 'bcastSelect2esst' @/

        /@ The following apply to the source of the DMA @/

        {
        VME_DMA_PCI,   /@ Bus 'srcVmeAttr.bus' @/
        VME_SST320,    /@ 2eSST rate 'srcVmeAttr.vme2esstRate' @/
        VME_MODE_A32,  /@ address mode 'srcVmeAttr.addressMode' @/
        VME_D32,       /@ data width 'srcVmeAttr.dataWidth' @/
        VME_2eSST_OUT, /@ xfer out protocol 'srcVmeAttr.xferProtocolOut' @/
        TRUE,          /@ supervisor access type 'srcVmeAttr.superAccessType' @/
        TRUE           /@ program access type 'srcVmeAttr.pgmAccessType' @/
        },

        /@ The following apply to the destination of the DMA @/

        {
        VME_DMA_VME,   /@ Bus 'dstVmeAttr.bus' @/
        VME_SST320,    /@ 2eSST rate 'dstVmeAttr.vme2esstRate' @/
        VME_MODE_A32,  /@ address mode 'dstVmeAttr.addressMode' @/
        VME_D32,       /@ data width 'dstVmeAttr.dataWidth' @/
        VME_2eSST_OUT, /@ xfer out protocol 'dstVmeAttr.xferProtocolOut' @/
        TRUE,          /@ supervisor access type 'dstVmeAttr.superAccessType' @/
        TRUE           /@ program access type 'dstVmeAttr.pgmAccessType' @/
        },
    NULL               /@ Next descriptor if chained mode 'next' @/
    };

TEMPE_DMA_ATTRIBUTES dmaAttribSample =
    {
        {
        4096,   /@ PCI bus max block size 'busUsg.maxPciBlockSize' @/
        64,     /@ PCI bus back-off timer (usec) 'busUsg.pciBackOffTimer' @/
        4096,   /@ VME bus max block size 'busUsg.maxVmeBlockSize' @/
        64,     /@ VME bus back-off timer 'busUsg.vmeBackOffTimer' @/
        FALSE,  /@ VME flush on aborted read? 'busUsg.vmeFlshOnAbtRead' @/
        FALSE   /@ PCI flush on aborted read? 'busUsg.pciFlshOnAbtRead' @/
        },
    NULL        /@ User-defined completion routine 'userHandler' @/
    };
.CE

To start the DMA on channel 0 with the above parameters (turning on the 
"don't return until done" option), put the following into your code:

.CS
    TEMPE_DMA_DESCRIPTOR dmaDesc;
    TEMPE_DMA_ATTRIBUTES dmaAttr;
    TEMPE_DMA_STATUS     dmaStat;
    int                  chan;  /@ channel number @/

    dmaDesc = dmaDescSample;
    dmaAttr = dmaAttrSample;
    chan = 0;
    dmaAttr.userHandler = WAIT_FOREVER; /@ Don't return till DMA done @/

    if (sysTempeDmaStart (chan, &dmaDesc, &dmaAttr, &dmaStat) == ERROR)
        printf("channel 0: DMA start error\n");
.CE

To change the source, destination and byte count and start another DMA, 
this time on channel 1 do the following:

.CS
    dmaDesc.sourceAddr = 0x02000000;
    dmaDesc.destAddr   = 0x81000000;
    dmaDesc.byteCount  = 0x00020000;
    chan = 1;

    if (sysTempeDmaStart (chan, &dmaDesc, &dmaAttr, &dmaStat) == ERROR)
        printf("channel 1: DMA start error\n");
.CE

.SS "PCI Snooping"

The MV64360 has two PCI busses which allow access to DRAM (bus 0 and bus 1). 
The following two definitions, in config.h, control the snooping attribute 
for the PCI access control base 0 registers (programmed inside of 
"sysMv64260Phb.c", function sysMv64360PhbInit()):

.CS
#define PCI_DMA_SNOOP_BUS_0_ON
#define PCI_DMA_SNOOP_BUS_1_ON
.CE

The registers are referenced as PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0 and 
PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1.  If snooping is turned off for either bus 
by #undef'ing the appropriate #define, it is important to either turn off 
cacheing for that region of memory or ensure that all software which accesses 
the region of memory covered by  the associated PCI access control register 
performs explicit cache control directives (e.g. flusing, invalidating) to 
maintain cache coherency.  When shared memory services are enabled, the 
snooping for PCI bus 0 is turned off - see "Shared Memory Support" for more 
information.

The utility sysMv64360SpecialMem.c is provided to aid in the snooping and 
cacheing configuration for a particular system.

.SS "Optimizing PCI and PCI-X Performance"

In order to configure the BSP for optimal PCI and PCI-X performance the 
following must be done:

(1) Disable the Caches

The caches are disabled by #undef'ing INCLUDE_CACHE_SUPPORT, INCLUDE_CACHE_L2, 
and INCLUDE_CACHE_L3 in config.h.

(2) Configure the PCI Burst Limit

The PCI Burst Limit is configured in sysMv64360Phb.c.  Bits 9:8 of 
PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0 and PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1 are 
controlled by the definitions PCI_DMA_SNOOP_BUS_0_ON and 
PCI_DMA_SNOOP_BUS_1_ON.  When snooping is enabled for a bus by 
#define'ing either definition in config.h, the burst limit will be 
programmed as 00 (for 32 bytes), if #undef'ed to disable snooping then the 
burst limit will be programmed as 10 (for 128 bytes).  Optimal performance 
is achieved by #undef'ing these definitions, thereby setting the burst limit 
to 128 bytes.

(3) Configure the PCI Read Size

The PCI Read Size is configured in sysMv64360Phb.c in the 
PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0 and PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1 registers.  
Bits 11:10 are always set to 11 (256-byte prefetch) regardless of whether 
snooping is enabled or not.

(4) Disable PCI Snooping

The PCI snooping is configured in sysMv64360Phb.c in the sysMv64360PhbInit() 
routine.  Bits 3:2 in PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0 and 
PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1 are controlled by the definitions 
PCI_DMA_SNOOP_BUS_0_ON and PCI_DMA_SNOOP_BUS_1_ON.  When snooping is enabled 
for a bus by #define'ing either definition in config.h, the "snoop" will be 
programmed as 10 (for WB Cache), if #undef'ed to disable snooping then the 
"snoop" will be programmed as 00 (for no cache coherency).

(5) Fast Back to Back

Fast Back to Back is configured in sysMv64360Phb.c.  Bit 9 "FastBTBEn" in 
PCIIF_STATUS_AND_CMD_PCICFG is set to 1 to enable fast back to back (unless 
IPMC SCSI support is enabled in config.h).

(6) Combining

Combining is configured in sysMv64360Phb.c.  Bits 4 "MWrCom" and 5 "MRdCom" 
are set to 0 as per errata Fer PCI-#9 "Combine and Retry Problems in PCI 
2.2 Mode".

.SS "Network Configuration"
The MV64360 provides two 10/100/1000 bps full duplex ethernet ports.   
Port 0 (LAN1) is accessible from a front panel RJ45 connector. Port 1
(LAN2) is accessible from either a front panel RJ45 connector or can
be configured for rear P2 access - controlled by jumper J30.   

Support for Port 0 is controlled by INCLUDE_PRIMARY_ENET in config.h.
Support for Port 1 is controlled by INCLUDE_SECONDARY_ENET  in config.h.  
Only support for port 0 is defined and initialized by default. To disable 
the MV64360 ethernet, INCLUDE_PRIMARY_ENET  and INCLUDE_SECONDARY_ENET 
must both be undefined in config.h. Additionally, the Makefile must be 
modified to remove the driver object file mv64360End.obj from the 
MACH_EXTRA line to allow error-free compilation.

The ethernet driver automatically senses and configures a port as 10baseT,
100baseT, or 1000baseT based on the maximum capability advertised by the 
port's network link partner.   

The ethernet descriptors are stored in the MV64360's integrated SRAM for 
optimal performance.

For optimal Ethernet performance it is necessary to increase the 
"default data pool ", by editing the following parameters in 
target/h/netBufLib.h:

.CS
#define NUM_NET_MBLKS   1600    /* no. mBlks to initialize */
#define MAX_MBLK_TYPES  256     /* max number of mBlk types */
#define NUM_64          160     /* no. 64 byte clusters */
#define NUM_128         160     /* no. 128 byte clusters */
#define NUM_256         140     /* no. 256 byte clusters */
#define NUM_512         140     /* no. 512 byte clusters */
#define NUM_1024        125     /* no. 1024 byte clusters */
#define NUM_2048        125     /* no. 2048 byte clusters */
.CE

Currently, the default network stack in Tornado 2.2.1 does NOT 
support checksum offloading in the protocol layer. 

.SS Non-Volatile Memory

The "base address" of the NVRAM/RTC is located at 
MV64360_DEVCS1_BASE_ADDR + 0x10000, (0x100000 offset into the 
"device bank"), and extends for 32KB up to 
MV64360_DEVCS1_BASE_ADDR + 0x17fff.  The first 256 bytes are 
skipped, the second 256 bytes are  used to store the vxWorks 
boot line parameters.  From the "base address" plus the offset 
of 0x7ff0 thru an offset of 0x7fff (16 bytes), are the M48T37 
registers.

The NVRAM_BASE address is used in the sysNvWrite and sysNvRead 
routines in sysLib.c.  In order to write to/read from NVRAM 
using the sysLib routines (sysNvRead & sysNvWrite), you need 
only specify an offset (ie. NV_RAM_BASE + offset).

.SS "Failsafe Timer"

Support for a failsafe ( ie. watchdog ) timer is provided.  The failsafe
timer is implemented with the M48T37 Timekeeper SRAM. This support is not 
part of the standard VxWorks watchdog library, wdLib.  Failsafe timer 
expiration can be reported via a maskable interrupt or via a board reset 
event.  The timeout lengths range from 0 (disable) to 31 seconds.

Failsafe timer support can be included in the BSP by defining
INCLUDE_FAILSAFE in config.h.  This support by default is excluded.  There
is only one failsafe timer on the board, so only one failsafe timer can
be established at any given time.

The failsafe timer is disabled at power-up and after a reset.  The
failsafe timer support routines are defined in sysFailsafe.c.

In order to use the failsafe timer, the user will need to first call
sysFailsafeSet().  The routine takes as parameters the number of
seconds until expiration and whether or not to generate a board reset
upon expiration.  If reset is set to FALSE, an interrupt occurs, if reset
is set to TRUE, a board reset occurs.  Passing a value of 0 for seconds
will disable the failsafe timer.  Once the timer has been set, subsequent
calls to sysFailsafeSet() will extend the timer for the specified number
of seconds.

A call to sysFailsafeCausedReset() will determine whether
the failsafe timer caused the last board reset.  This information will
be lost if a call to sysAlarmSet() is made prior to calling
sysFailsafeCausedReset().

A call to sysFailsafeCancel() will disable the failsafe timer.  The
current failsafe timer settings can be retrieved with a call to
sysFailsafeGet().  The current failsafe timer settings can be displayed
with a call to sysFailsafeShow(), this displays the current settings
not the number of seconds until timer expiration.  The routine
sysFailsafeIntr() is the failsafe timer interrupt handler.  In order to
define your own interrupt handler, simply edit this routine.

.SS "Real-Time Clock and Alarm Clock"

Support for a real-time clock and an alarm clock are provided.  The
real-time and alarm clocks are implemented with the M48T37 Timekeeper 
SRAM.

Real-time and alarm clock support are included in the BSP by defining
INCLUDE_RTC in config.h.  This support by default is excluded.

When the real-time clock information matches the alarm clock settings
an interrupt will be generated.

Once set, the alarm clock will retain its settings upon a board reset.
The real-time and alarm clock support routines are defined in sysRtc.c.

The real-time clock can be set in your application code with a call to 
sysRtcSet().  The following information needs to be supplied in order 
to set the RTC: century, year, month, day of month, day of week, hour, 
minute, and second.  The current RTC settings can be retrieved with a 
call to sysRtcGet().  The current RTC date and time can be displayed 
with a call to sysRtcShow().  However, the real-time clock must be set 
via sysRtcSet() or via the MotLoad "set" command the first time the 
board is used.  The sysRtcDateTimeHook() routine is provided as a hook 
to the vxWorks dosFsLib as a means of providing the date and time for 
file timestamps.

The alarm clock can be programmed in the following five ways:
.CS
  Method           Configurable Parameters
 ------------------------------------------------------------
 Once a month      Date, hour, minute, second
 Once a day        Hour, minute, second
 Once an hour      Minute, second
 Once a minute     Second
 Once a second     (none)
.CE

The alarm clock is set with a call to sysAlarmSet().  This routine takes
a method and the alarm clock parameters as arguments.  The alarm clock can
be cancelled with a call to sysAlarmCancel().  The current alarm clock
settings can be retrieved with a call to sysAlarmGet().  The current alarm
clock settings can be displayed with a call to sysAlarmShow().  The routine
sysAlarmIntr() is the alarm clock interrupt handler.  In order to define your
own interrupt handler, simply edit this routine.

.SS "Auxiliary Clock"

Support for the AuxClock is provided.  The AuxClock can be implemented with 
any one of the four MV64360 Timer/Counters.  This is user-configurable in 
config.h with the definition MV64360_AUXCLK_TMR, by default it is defined
as 0, so timer/counter 0 is used to implement the AuxClock.

AuxClock support can be included in the BSP by defining INCLUDE_AUX_CLK in
config.h.  This support by default is excluded.  The AuxClock support
routines are defined in sysMv64360AuxClk.c.

.SS "PCI Access"
The 64-bit PCI/PCI-X busses are fully supported under the
.I PCI Local Bus Specification, Revision 2.1.
All configuration space accesses are made with BDF (bus number, device number, 
function number) format calls in the pciConfigLib module.  For more 
information, refer to the man pages.

.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1	- shared memory
    \f3geisc0\f1	- Primary Ethernet (10baseT, 100baseTX, or 1000baseT)
    \f3geisc1\f1	- Secondary Ethernet (10baseT, 100baseTX, or 1000baseT)
    \f3tffs\f1 - TFFS
    \f3scsi\f1  - SCSI

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP bootrom file:
.CS
    cd target/config/mv6100
    make clean
    make bootrom_uncmp
    elfToBin <bootrom_uncmp >boot.bin
    chmod 666 boot.bin
    cp boot.bin /tftpboot/boot.bin
.CE

.SS "Flashing the Boot ROM Using Motorola MotLoad:"

The MVME6100 boards come with the Motorola MotLoad firmware installed 
in the FLASH banks.  This firmware can be used to download and FLASH 
the VxWorks bootrom into either of the FLASH banks.  

First set some MOTLoad global variables to conform to your particular
operational environment.  This is done via a series of `gevEdit'
commands: These variables, when set, remain in NVRAM through power
cycles and can later be changed, if desired,  with MOTLoad `gevDelete'
and `gevEdit' commands.  Note: Before using any of the gevXxx routines,
for the first time, gevInit must be run.  gevInit() need only be run one
time.  This routine initializes and clears the MotLoad global environmental
variable area in NVRAM.

.CS

  MVME6100> gevEdit mot-/dev/enet0-cipa
  (Blank line terminates input.)
  123.111.32.90


  Update Global Environment Area of NVRAM (Y/N)? y

  MVME6100> gevEdit mot-/dev/enet0-sipa
  (Blank line terminates input.)
  123.111.32.180


  Update Global Environment Area of NVRAM (Y/N)? y


  MVME6100> gevEdit mot-/dev/enet0-gipa
  (Blank line terminates input.)
  123.111.32.1


  Update Global Environment Area of NVRAM (Y/N)? y


  MVME6100> gevEdit mot-/dev/enet0-file
  (Blank line terminates input.)
  mydir/boot.bin


  Update Global Environment Area of NVRAM (Y/N)? y
  MVME6100>

.CE

The above sequence sets the client IP address (IP address of the MVME6100)
to 123.111.32.90, the IP address of the server to 123.111.32.180, the
IP address of the gateway to 123.111.32.1 and the tftp file name to
"mydir/boot.bin".  Note that we have used "dev/enet0" as the ethernet
device involved in the download.  You can use another device, such as
"dev/enet1" if you wish, use of the `netShow' command will display
which interfaces are "up" and available for use in the download operation.

The file is transferred from the TFTP host to the target board using
the `tftpGet' command.  IMPORTANT: You must have a TFTP server running
on your host's subnet for the `tftpGet' command to succeed.  The file
name must be set to the location of the binary file on the TFTP host.
The binary file must be stored in the directory identified for TFTP
accesses, but the file name is a relative path and does not include
the \f3/tftpboot\f1 directory name:
Now that the MOTLoad global variables have been set into NVRAM, you
can perform the tftp load of the file image with the following
command:

.CS
   MVME6100>tftpGet -d/dev/enet0
.CE

Notice that we have specified "/dev/enet0" as the interface.  This is
the same interface that was specified with the `gevEdit' command.  If
no interface is specified on the `tftpGet' command line, MOTLoad
defaults to "dev/enet0".  Also take note that you can override the
NVRAM settings previously set via the `gevEdit' command by specifying
additional MOTLoad options on the `tftpGet' command line.  The "-c"
option can override the client IP address, the "-s" option can
override the server IP address, the "-g" option can override the
gateway IP address, and the "-f" option can override the file name.

After the file is loaded onto the target, the `flashProgram' command is used
to put it into soldered FLASH parts.

.CS
   MVME6100>flashProgram -oxxxxxxxx -nfff00 -d/dev/flash0 
.CE

The offset "xxxxxxxx" to use in the `flashProgram' command depends upon
the size of the non-boot FLASH bank (it is assumed that the file will be
flashed into the current non-boot bank, in order to preserve the firmware in
the boot bank).

MotLoad has a command, "flashShow", that presents information
necessary to calculate offset.  The output looks something like this:

.CS
/dev/flash0  F4000000  04000000 ....  No ...
/dev/flash1  F8000000  04000000 ....  Yes  ...
.CE

In this example, flash0 refers to Bank A and flash1 refers to Bank B.
The starting address and size is displayed for each of the banks.  The
Yes/No refers to whether that bank is currently the boot bank.  In this
example the boot bank is Bank B.

To determine the offset at which to flash the bootrom the following 
calculation can be used:

(size non-boot bank) - 1MB + 0x100

Using the example output above we would get:

0x04000000 - 0x00100000 + 0x100 = 0x03F00100

This is the offset to use in place of xxxxxxxx in the flashProgram 
command.

When the flashProgram command is finished, power down the board and switch 
boot banks - this is controlled via switch S4.  Then power the board back up.

Here are some examples.

To flash VxWorks into Flash Bank 0 (A) with MotLoad running in Flash 
Bank 1 (B):

.CS
flashProgram -o03f00100 -nfff00 -v
.CE

To flash Vxworks into Flash Bank 1 (B) with MotLoad running in Flash 
Bank 0 (A):

.CS
flashProgram -d/dev/flash1 -o03f00100 -nfff00 -v
.CE

The MotLoad firmware can also be copied from one bank to another.

To flash MotLoad from Flash Bank 0 (A) into Flash Bank 1 (B):

.CS
flashProgram -d/dev/flash1 -o03f00000 -n100000 -sf7f00000 -v
.CE

To flash MotLoad from Flash Bank 1 (B) into Flash Bank 0 (A):

.CS
flashProgram -o03f00000 -n100000 -sfbf00000 -v
.CE

.SH "SPECIAL CONSIDERATIONS"

.SS "Delivered Objects"
The delivered objects are: `bootrom', `bootrom.bin', `bootrom_uncmp',
`bootrom_uncmp.bin', `vxWorks', `vxWorks.sym', `vxWorks.st', `vxWorks_rom',
`vxWorks_rom.bin', `vxWorks.st_rom', `vxWorks.st_rom', and `vxWorks.st_rom.bin'.

.SS "Make Targets"
The make targets are listed as the names of object-format files.  Append `.hex'
to each to derive a hex-format file name.

 `bootrom'
 `bootrom_uncmp'
 `bootrom_res'  (builds, does not run)
 `bootrom_res_high'  (builds, does not run)
 `vxWorks' (with `vxWorks.sym')
 `vxWorks_rom'
 `vxWorks.st'
 `vxWorks.st_rom'
 `vxWorks.res_rom' (builds, does not run)
 `vxWorks.res_rom_res_low' (builds, does not run)
 `vxWorks.res_rom_nosym'  (builds, does not run)
 `vxWorks.res_rom_nosym_res_low'  (builds, does not run)
 `vxWorks_romResident'  (builds, does not run)
 `vxWorks_romCompress'

.SS "Relocation value does not fit in 24 bits"

Due to the large amount of DRAM available on the MVME6100 board you may
encounter the "Relocation value does not fit in 24 bits" error message, and
be unable to download object modules to your target.

As a solution, you can rebuild the kernel images and/or the object modules you
are downloading using the longcall compilation flag.  For building from the 
command line you must add the ADDED_CFLAGS rule to the Makefile, or if using
the Project facility you must modify the Compilation Rule.

If using the gnu compiler:

.CS

ADDED_CFLAGS=-mlongcall 

.CE

If using the diab compiler:

.CS

ADDED_CFLAGS=-Xcode-absolute-far

.CE

Note: ROM resident images built with the above compilation flags will not
boot.  This is intended only for non-ROM resident kernel images and
downloadable object modules.

As an alternative, the amount of DRAM made available to the VxWorks kernel can
be reduced to 16MB.  To do this, change #define LOCAL_MEM_AUTOSIZE in config.h
to #undef LOCAL_MEM_AUTOSIZE, and change LOCAL_MEM_SIZE to 0x01000000 (16MB).
The above changes will not impact the programming of the System Memory 
Controller, all of the DRAM will still be configured.  However, the kernel will
only be aware of the first 16MB.

.SH "SPECIAL ROUTINES"

.SS "sysMv64360SpecialMem.c"
The file "sysMv64360SpecialMem.c" contains the sysMv64360SpecialMem function
which allows the user to modify the characteristics of memory with
regard to PCI access.  The user may modify the cacheing attribute,
base address, size, snoop character, mburst size and read
transaction size.  The change applies to a single PCI access
control window on one of the two PCI busses.  This function can
be used to configure memory for the fastest possible VME-based
DMA access.  To achieve the fastest possible DMA speed using the
Tsi148 (Tempe bridge) engine, it is necessary to configure memory
as noncacheable, nonsnoopable and modify the burst and read sizes
for the source and destination memory.  Details of how to do this
are contained inside of ""sysMv64360SpecialMem.c".

.SS "Error Handling Suite"

`Introduction'

This error handling suite is included via #define of INCLUDE_ERROR_HANDLING.

This module implements generic error handling support.  Software which
detects an error, either through interrupts or via soft scan of
hardware can make use of this module to "dispose" of the error.
Disposition can consist of displaying an immediate indication of the
error and/or saving the information about the error in a log which can
be queried later.  An optional user-written function can also be
called upon occurrence of a particular error.  This module does not
itself detect errors but instead offers supporting functions which
allow for handling of errors, which are detected elsewhere in the
system, in a uniform way.

`Connecting to the Error Handling Module'

Before making use of the error handling services software must
"connect" with the error handling module.  This is a two-step process
consisting first of (1) Domain Initialization and secondly of (2)
Error Registrations.  After these two steps are completed actual
logging of specific errors can be performed.

Before proceeding a distinction must be made between error
"registration" and error "logging".

Error "registration" is done once for each possible error.  It is
through "registration" that the error handling module is presented
with a unique error ID which will be monitored by application software
as the system runs.  Note that "registering" and error does not mean
that the error has occurred, it just means that this particular error
is being monitored and could potentially be logged.

Error "logging" is what is done when application software detects an
error and informs the error handling module that the error has
actually occurred.  The error handling module then disposes of the
error according to rules which were established when the error was
registered.  Thus it can be seen that to successfully log an error,
that error must first have been registered.

`(1) Domain Initialization'

The error handling module considers a "domain" to be a software
subsystem which deals with a major component of the system.  Examples
of domains would be "CPU", "Host Bridge", "PCI", "VME Bridge"
etc.  The user has some flexibility in defining these domains.  A
domain need not necessarily be confined to actual hardware on the board,
it could be a user application such as "Widget".   It is anticipated
that there will be at most a small handful of domains associated with
a specific system.  

A "domain" is identified by two things:

`a)'  An error domain ID.  Such an ID consists of a 32-bit identifier,
of which only the top byte actually identifies the domain.
For example the host bridge might have a domain ID of 0x02000000.
(Here the high order byte "0x02" identifies this as the host
bridge domain,  the CPU domain might have an ID of 0x01000000).
The domain ID is important because every error which is eventually
logged and has an error ID of the form 0x02xxxxxx will be assumed 
to be associated with the host bridge domain because of "0x02" as 
the high order byte.

       and

`b)'  An string e.g. "Disco 2" (which will be used in display of
subsequently logged errors).  This string can be a maximum
of 8 characters in length.

Each domain can contain three functions important to the error handling
module: 

`1)' An error scanning function - 'errorScan (errorId)':
This function, when called with a specific error ID, will "scan"
for the assertion of the error associated with this ID.  This
is useful for errors which are not associated with interrupts and
must be "polled" via soft read of a hardware register.

`2)' An error clearing function - 'errorClear (errorId)':
This function, when called with a specific error ID, will "clear"
an asserted error.  Again this could be used in association with
non-interrupt generating errors to clear status bits which
were set when the error was asserted.

`3)' An error formatting function - 'errorFormat (errorId)'
This function is called by the error handling module to format
an error message associated with a particular error.  This
function would be called by the error handling module in
preparation to display the message on the console.

A domain can have all three of these functions but it is not required 
to have any of them.

Each separate domain (e.g. "CPU", "PCI", "Widget") must separately
establish initialization with the error handling module.  

A single function call from application code into the error handling
module will initialize a specific domain.  An example of how to do
this for a VME bridge domain called "Tsi148" is presented below in
the following code fragment:

.CS
    /* Forward references to this domain's helper functions */

    char * tsi148ErrorFormat (ERROR_ID id);
    BOOL   tsi148ErrorScan   (ERROR_ID id);
    BOOL   tsi148ErrorClear  (ERROR_ID id);

    /* Error module tells me about his functions in here */

    ERROR_UTILS * myErrorUtils;

    /*
     * I, "Tsi148" domain, tell error module about 
     * my helper functions in here.
     */

    ERROR_USR_FUNCS tsi148ErrorFuncs;

    ...

    /* Initialize pointers to my format, scan, and clear functions */

    tsi148ErrorFuncs.myErrorFormat = tsi148ErrorFormat;
    tsi148ErrorFuncs.myErrorScan = tsi148ErrorScan;
    tsi148ErrorFuncs.myErrorClear = tsi148ErrorClear;

    if (_func_errorHandlerInit == NULL)
	{
	logMsg ("Error Handling Suite not linked\n",0,0,0,0,0,0);
	return(FALSE);
	}

    /* Here's the call which initializes the domain !!! */

    myErrorUtils = (* _func_errorHandlerInit) (0x03000000,"Tsi148", 
					       &tsi148ErrorFuncs);

    /* Now test to see if domain initialization actually worked */

    if (myErrorUtils.errorLog == NULL)
	{

	/*
	 * myErrorUtils.errorLog == NULL is the error handling module's
	 * way of telling me the the domain initialization failed.
         */

	printf ("Tsi148 error domain initialization FAILED\n")
	}
    else
	{

	/*
	 * Domain initialization succeeded:
	 * Now myErrorUtils.errorRegister points to the error handling
	 * module's error registration function and
	 * myErrorUtils.errorLog points to the error handling module's
	 * error logging function
         */

	printf ("Tsi148 error domain initialization SUCCEEDED\n");

	}

	...

   /* Here are my actual helper functions - more on this later */

   char * tsi148ErrorFormat (ERROR_ID id)
       {
       ... /* More said about this later */
       }

   BOOL tsi148ErrorScan   (ERROR_ID id)
       {
       ...  /* More said about this later */
       }

   BOOL widgetErrorClear  (ERROR_ID id)
       {
       ... /* More said about this later */
       }


.CE

Notice the information that is exchanged during this domain
initialization.  The application (domain) code informs the error
handling module of its domain ID, domain ASCII string, error
formatting function, error scanning function and error clearing
function.  The error handling module in turn informs the application
(domain) code of its error registration function and error logging
function.

`(2) Error Registrations'

After a domain has initialized as described above, it must "register"
each error that it is going to monitor and could potentially "log".
This is done by calling the "error registration" function, whose
pointer was given to the domain initialization code during in the
previous step.

The following code fragment (a continuation of the fragment
illustrating domain initialization) accomplishes this:

.CS
        /*
	 * Forward reference to error handler function 
         * and declaration of error handler function parameter
         */

	void tsi148ErrorHandler (int param);
	int errorParam = 0x1234;

        /*
         * This tells how we wish to "dispose" of the error whenever
         * it is logged.
         */

        ERROR_DISPOSITION disposition;

        ...

        disposition = ( ERROR_DISP_SAVE |
                        ERROR_DISP_IMMEDIATE_DISPLAY |
		        ERROR_DISP_FUNCTION_CALL ); 

	/* Here is the call that actually registers an error */

	if (* myErrorUtils.errorRegister)(0x03010201,
                                          disposition,
				          tsi148ErrorHandler,
	                                  errorParam);

        ...


   /*
    * This function will be executed when the 0x03010201 error
    * is actually "logged" because 'disposition' included
    * ERROR_DISP_FUNCTION_CALL and we did provide a nonNull
    * function pointer: tsi148ErrorHandler
    */

   void tsi148ErrorHandler     (int param)
       {
       logMsg("tsi148 error handler called with argument %d\n",param);
       }

.CE

Note several things about the above example:

The function call which actually performs the registration is done
through a function pointer which was given to the domain code during
domain initialization (myErrorUtils.errorRegister).  

There are four parameters used in this function call: An error ID
(0x03010201), a "disposition", a pointer to an error handling
function, and an error handling function parameter.

The error code in this example is 0x03010201.  Notice that the high
order byte 0x03 matches the high order byte used to initialize the
"Tsi148" domain.  This is important because the error handling module
will refer to that domain's error formatting, error scanning and/or
error clearing function when it is asked to deal with this error.

The disposition in this example enables all of the active
possibilities: "save", "immediate display" and "function call".
"Save" (ERROR_DISP_SAVE) means that any future calls into the error
module to "log" an occurrence of this error will cause the information
about the error to be saved in an internal error log which can later
be displayed.  "Immediate display" (ERROR_DISP_IMMEDIATE_DISPLAY)
means that when an error is logged, an immediate display (to the
console) of this error occurrence will be performed by the error
module.  "Function call" (ERROR_DISP_FUNCTION_CALL) means that
when this particular error is "logged", the last two parameters in the 
registration call will be used to construct a function call with a 
specified parameter.  That function can be anything that the user 
desires.  

The third parameter (function pointer) can be NULL if no function
is to be called when the error is logged.  In that case, even if
ERROR_DISP_FUNCTION_CALL is part of the "disposition" no function
will be called.

There is one other possible "disposition".  If the registration
"disposition" is ERROR_DISP_IGNORE, then no action is taken by the
error handling module when the error is logged.

An error can be registered more than one time.  Each error is uniquely
identified by an error ID so if a call is made to register an error ID
which has been previously registered, the information saved from the
previous registration is overwritten.  This can be useful if
application code wants to change the disposition of an error at
run time.

An error can be registered any time after the system is up, even
after error logging has begun.

`Error Logging'

Once the domain has been initialized, and a particular error has
been registered it is possible that the error will assert itself.
If the software is written so that it can detect the assertion of
the error, it can call the error handling module to "log" the
error.  The following code fragment continues the example:
   
.CS

   ERROR_MSG myErrorMsg;
   
   ...

   /* Tsi148 code detects a "bus" error and wishes to log it */
      
   myErrorMsg.id = 0x03010201	/* ID of bus error, previously registered */

   myErrorMsg.params[0] = <status reg value, read from HW = 0x00002000>
   myErrorMsg.params[1] = <access address, read from HW = 0xc000ffff>

   (* myErrorUtils.errorLog) (&myErrorMsg);

.CE

That's it.  Logging the error simply involves loading an ERROR_MSG
structure with the ID of the error to log (that ID must have
previously been registered).  Also up to six parameters are available
to provide additional information about the error.  In this example
two of those parameters are loaded with HW register values.  The
function called to actually log the error is pointed at by a function
pointer which we learned about during domain initialization.

`What Happens When an Error is Logged?'

The error logging is initiated when the application code makes a call
to the error handling module's error logging function.  The application
code knows which function to call because the logging function pointer 
was passed to it during domain initialization.

The error handling module first attaches a "time tag", to the error
message.  This tag indicates the date and time that the error log
request was received.

The error handling module then checks to see if the error ID has been
previously registered.  If it has not then the logging request is
rejected and ERROR is returned to the logging requestor.

Now that the error handling module knows that the error is registered,
it checks the disposition of the error which was set during the last
registration call for that particular error ID.  If the error
disposition is ERROR_DISP_IGNORE there is no more work to do and the
error handling module returns OK.

The error disposition is not "ignore" so the next disposition
possibility checked is ERROR_DISP_FUNCTION_CALL.  If this disposition
is set, and the error function pointer which was presented at
registration time is not NULL, the user function is called with the
parameter that was registered.  Note that execution is still in the
same thread as the error logging call so if the function which is
doing the error logging is an interrupt service routine it should be
careful to obey ISR rules (don't do any blocking calls).

Once the disposition of "function call" has been handled, the error
logging request is posted to a message queue which is being read by a
separate execution thread called the "error task".  The error logging
execution thread then returns OK to the caller.

The error task will read the message queue to obtain the error logging
request.  It will check the registration disposition to see if it
includes ERROR_DISP_IMMEDIATE_DISPLAY.  If it does, then an indication
that the error occurred is displayed at this time (more about how this
error display is formatted is presented later).

Once the "display immediately" disposition has been handled by the
error task, it checks to see if the disposition also includes
ERROR_DISP_SAVE.  If it does, then the information included in the
original error message (along with the time tag which was added) is
saved in a circular error logging table.  The size of this table is
governed by ERROR_LOG_TABLE_SIZE.  The entry is placed at the end of
the table.  If the table is full and there is no room for this entry
to be saved, the oldest saved entry is replaced by this one.  Thus the
table will contain only the latest errors logged.

The handling of the logging request is now complete.

`Formatting Error Messages For Display'

During the domain initialization, the application code presents to
the error handling module three function pointers.  One of these
is the error message formatting pointer.  Continuing our example,
here is how the formatting function might look for the "Tsi148"
domain:

.CS
    char * tsi148ErrorFormat (ERROR_ID id);
        {
	switch (id)
	    {
	    case 0x03010201:
		return ("Bus error: status = 0x%08x, addr = 0x%08x");
            case 0x03010202:
                ....
	    default: 
		return (NULL);
	    }
        }
.CE

Notice that the formatting string does NOT include actual parameters
for the formatting specifiers.  This is intended.  Also it does not
include newline characters, these are not needed either.

Here we see the Tsi148 domain's error message formatting function.
Since a pointer to this very function was passed to the error handling
module during domain initialization for "Tsi148" and the error ID
of the error being logged begins with "0x03", the error handling
module knows to call this function to assist it in formatting a
displayable message.  In the error logging example above, we logged
a Tsi148 bus error (error ID = 0x03010201).  Now our registration
disposition for this particular error included 
ERROR_DISP_IMMEDIATE_DISPLAY.  Here is how the error handling module
might display this error.

.CS
	ErrLog:  JUL 21 15:36:38 "Tsi148  " (03010201):
                Bus error: status = 0x00002000, addr = 0xc000ffff
.CE

Notice the components of the error message:

ErrLog: Preface is always part of the message to identify this as
an "error log" message.

"JUL 21 15:36:38" - is obtained from the "time tag" which the
error handling module attached to the error message when it was 
logged.

"Tsi148  " - Is the ASCII domain string which was saved by the
error handling module during domain initialization of error type
0x03000000.

"(03010201)" - Is the error ID which was passed to the error handling
module by the application when the error was logged.

"Bus error: status = 0x00002000, addr = 0xc000fffff" is constructed
from two components:  When the error was logged, two parameters
(out of a possible maximum of six) were presented to the error handling
module in the logging message parameter elements, errorMsg.params[0], 
and errorMsg.params[1].  The errorTask checks the domain initialization
for this error and sees that an error formatting function had
been specified.  It calls this function with the error ID to obtain
the formatting string which it uses, along with the parameters passed
in errorMsg.params[0-1] to construct the string to display.

Notice that if a formatting function had not been specified, then
the error task would have only displayed the first line:

.CS
	ErrLog:  JUL 21 15:36:38 "Tsi148  " (03010201):
.CE

If certain errors in the domain do not require a formatted message
component to be displayed, the formatting function should simply
return NULL for those particular error IDs.

`Error handling Show Routines'

When INCLUDE_SHOW_ROUTINES is #define'd the following functions
are available:

.CS
sysErrorDomainShow (void)
sysErrorRegisterShow (mask, compare)
sysErrorLogShow (mask, compare)
.CE

Examples of use are:

.CS

-> sysErrorDomainShow ()

Error Domain Initializations: sysErrorDomainShow ()

No.     Domain        Error ID      FmtFcn     ScanFcn    ClearFcn
------------------------------------------------------------------
 1      Disco II      02xxxxxx           0           0           0
 2      CPU           01xxxxxx           0           0           0
 3      Tsi148        03xxxxxx      114c40      114c90      114cbc
 4      PCI           04xxxxxx           0           0           0
 5      Widget        0fxxxxxx      114a2c      114a4c      114a8c

.CE

The above example shows five different domains have initialized.
Two of these domains ("Tsi148" and "Widget") have registered formatting
scanning and clearing functions.  The other domains ("Disco II",
"CPU", and "PCI") have initialized but they have specified NULL
pointers for their formatting, scanning and clearing functions.

.CS

-> sysErrorRegisterShow (0,0)

Error Registrations: sysErrorRegisterShow (0x00000000, 0x00000000)

No.  Error ID              Disposition   Fcn       Param
--------------------------------------------------------
  1  03010201("Tsi148")    dsp.sav.      0         0
       Msg: "Bus error: status = 0x%08x, addr = 0x%08x"
  2  0f010101("Widget")    dsp.sav.      0         0
  3  0f010201("Widget")    fcn.dsp.sav.  114acc    16
       Msg: "Widget count error, received = %d, actual = %d"
  4  01010001("CPU")       sav.          0         0
  5  04010101("PCI")       ignore        0         0

.CE

This shows that five errors have been registered.  The error IDs
associated with each error are displyed along with the domain name
string.  Also a disposition field is presented indicating combinations
"user function call" (fcn), "immediate display" (dsp), and/or "save in
log file" (sav) or simply "ignore".  In the above table, we see that
error code 0x03010201 and 0x0f010201 are both associated with a
formatting string (a format helper function was provided at domain
initialization and it indeed returns a non-NULL formatting string).
Also error code 0x0f010201 is associated with a logging function at
0x114acc which will be called with parameter 16 when this particular
error is logged.

sysErrorRegisterShow() takes two arguments: 'mask' and 'compare'.
Several of the functions in the error handling suite take these two
arguments.  They are used to filter error IDs which appear in a table.
Only error IDs which pass the filter are acted on by the function.

The filtering operation is:

.CS
  if ((tableID & mask) == compare)
      {

      /* tableID passes through filter */

      }
  else
      {

      /* tableID is blocked by filter */

      }
.CE

So in the above example if we had used 'mask' of 0xff000000 and
'compare' of 0x03000000 by entering:

.CS
-> sysErrorRegisterShow (0xff000000, 0x03000000)
.CE

Only the "Tsi148" error IDs would would have been displayed since
0x03xxxxxx is the only type of ID which will pass this filter.
Using (0,0) as the mask and compare allows causes everything to
match.  Since (0,0) is the default VxWorks argument passage when
no arguments are entered, simply entering:

.CS

-> sysErrorRegisterShow

.CE

Would cause the entire table to be displayed.

.CS

-> sysErrorLogRawShow (0, 0)

Errors currently logged: sysErrorLogRawDisplay (0x0, 0x0)

No.  time     ID       p0       p1       p2       p3       p4       p5
----------------------------------------------------------------------------
0001 4100e5d3 01010001 00000000 11111111 22222222 33333333 44444444 55555555
0002 4100e5d5 0f010201 00000020 00000022 22222222 33333333 44444444 55555555
0003 4100e5d7 03100015 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0004 4100e5d8 03100016 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0005 4100e5d9 03100017 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0006 4100e5d9 03100018 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0007 4100e5da 03100019 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
.CE

This command displays in "raw" format the errors which have been saved
in the log file.  Note that this command also uses 'mask' and 'compare'
filtering.  The display show the time tag, error ID, and the six parameters
which were part of the ERROR_MSG when the error was logged.  

This same display can also be presented in a more readable format via:

.CS

-> sysErrorLogShow (0, 0)

Errors currently logged: sysErrorLogDisplay (0x0, 0x0)

0001:  JUL 23 10:17:55 "CPU     " (01010001):
0002:  JUL 23 10:17:57 "Widget  " (0f010201):
       Widget count error, received = 32, actual = 34
0003:  JUL 23 10:17:59 "Tsi148  " (03100015):
0004:  JUL 23 10:18:00 "Tsi148  " (03100016):
0005:  JUL 23 10:18:01 "Tsi148  " (03100017):
0006:  JUL 23 10:18:01 "Tsi148  " (03100018):
0007:  JUL 23 10:18:02 "Tsi148  " (03100019):

.CE

Notice that sysErrorLogRawShow and sysErrorLogShow are aliases
to the functions sysErrorLogRawDisplay and sysErrorLogDisplay.
The 'Display' functions which display the actual error log are 
considered essential any time INCLUDE_ERROR_HANDLING is #define'd.
Thus they are included even if INCLUDE_SHOW_ROUTINES is not #define'd.
Also notice in the formatted display above that error code 0x0f010201
has an associated formatted string which is displayed as port of the
table.  

.CS
-> sysErrorHandlerShow
.CE

performs the following:

.CS
-> sysErrorDomainShow()
-> sysErrorRegisterShow (0, 0)
-> sysErrorLogRawShow (0, 0)
-> sysErrorLogShow (0,0)
.CE

`Error Scanning and Clearing Function'

The error scanning function which is optionally presented to the
error handling module at domain initialization time should scan for
assertion of an error associated with the parameter error ID.  It's
up to the user as to what to do if the error is asserted.  Since 
this is a boolean function, it could return TRUE or FALSE depending
on whether or not the error is asserted.  Another likely thing that
it might do is to actually make an error logging call if it finds
that the error is asserted.

The error clearing function is also a boolean function.  It is 
up to the user what this function does but returning TRUE if
the error was actually cleared might be useful.

`Error Log Table Management Routines'

There are several error log table management routines which
are presented below:

sysErrorScan (mask, compare) will go through the error registration
table and for each registered ID which passes the 'mask'/'compare'
filter it will call the associated scanning routine if such a routine
exists.

sysErrorClear (mask, compare) is similar to sysErrorScan() except
it will call the error clearing routine if it exists.

sysErrorDispositionIgnore (mask, compare) will change the disposition
to 'ignore' for any registered error IDs which pass the filter.

sysErrorDispositionSave (mask, compare) will add 'save' to the
disposition of any error IDs which pass the filter.

sysErrorDispositionDisplay (mask, compare) will add the 'immediate
display' disposition to any error IDs which pass the filter.

sysErrorDispositionFunction (mask, compare) will add the 'function
call' disposition to any error IDs which pass the filter.  Note that
the already registered error should have a function registered for
calling prior to use of this command.  If the function pointer is
NULL this command has no real functional effect.

sysErrorLogRemove (mask, compare) will remove from the log file,
any logged errors whose ID passes the filter.  This will free up
some room in the table for new errors.

.SS "Additional 'show' routines"

In addition to the standard VxWorks "Show" routines, special "Show"
routines have been added to display specific BSP and board related
items.  These routines are compiled in to the kernel when INCLUDE_SHOW_ROUTINES
is #define'd.  These show routines are:

.TS C
center;
rw(1.0i) lw(3.7i) .
vpdShow	T{
Display the board's VPD (Vital Product Data).
T}
sysMv64360SpdShow	T{
Display the raw SPD (Serial Presence Detect) data.
T}
sysMv64360DmaWindowsShow	T{
Display DMA Window Configuration.
T}
sysMv64360SmcShow	T{
Display the MV64360 DDR SDRAM Controller Registers.
T}
mv64360DumpRegs	T{
Display and label every MV64360 host bridge register.
T}
mv64360CPUWindowShow	T{
Display all of the MV64360 host bridge access window base addresses and sizes.
T}
sysTempeShow	T{
Display and label every Tempe (Tsi148) PCI-X to VME bridge registers.
T}
sysTempeWinShow	T{
Display all Tempe PCI-X to VME bridge window mappings with formatting.
T}
sysTempeOutWinShow	T{
Display a specified Tempe bridge outbound window setting with formatting.
T}
sysTempeInWinShow	T{
Display a specified Tempe bridge inbound window setting with formatting.
T}
intConnectShow	T{
Display relevant parameters associated with all intConnect()ed interrupts.
T}
sysPhysMemDescShow	T{
Display the entries in the sysPhysMemDesc[] array.
T}
memoryMapShow	T{
Display #define's associated with mapping various memory spaces.
T}
translationWindowShow	T{
Display host bridge translation window array used in BusToLocal conversions.
T}
pciCfgHeaderShowAll	T{
Display every valid PCI header with formatting.
T}
pciCfgShowAll	T{
Dump the first 0x100 bytes of all valid PCI headers.
T}
pciAccessCtlShow	T{
Display all of the MV64360 PCI access control registers in a formatted manner.
T}
sysMv64360TmrCntrShow	T{
Display the current state of all four of the MV64360's Timer/Counters.
T}
sysRtcShow	T{
Display current RTC time and date on the user's display.
T}
sysAlarmShow	T{
Display current alarm clock settings on the user's display.
T}
sysCpuBusSpd	T{
routine to calculate the speed of the CPU bus
T}
.TE

Below are the console invokable routines which deal with error handling.

.TS C
center;
rw(1.0i) lw(3.7i) .
sysErrorHandlerHelp	T{
Display all of the console invokable routines dealing with error handling.
T}
sysErrorHandlerShow	T{
Display all of the error handling tables.
T}
sysErrorDomainShow	T{
Display currently initialid error handling domains.
T}
sysErrorRegisterShow	T{
Display error IDs of currently "registered" errors.
T}
sysErrorLogRawShow	T{
Display currently logged errors in "raw" format.
T}
sysErrorLogShow	T{
Display currently logged errors in full format.
T}
sysErrorLogDisplay	T{
Same as sysErrorLogShow.
T}
sysErrorLogRemove	T{
Remove selected errors from the error logging table.
T}
sysErrorDispositionIgnore	T{
Set disposition of selected errors to "ignore".
T}
sysErrorDispositionSave	T{
Adds disposition of "save" to selected errors.
T}
sysErrorDispositionDisplay	T{
Adds disposition of "display" to seleted errors.
T}
sysErrorDispostionFunction	T{
Adds disposition of "function call" to seleted errors.
T}
.TE

The following is sample output from some of the show
routines:

(1) pciCfgHeaderShowAll() 

.CS
PCI Device bus=0 dev=0 func=0
vendor ID =				0x11ab
device ID =				0x6460
command register =			0x0217
status register =			0x22b0
revision ID =				0x03
class code =				0x05
sub class code =			0x80
programming interface =			0x00
cache line =				0x08
latency time =				0x40
header type =				0x80
BIST =					0x00
base address 0 =			0x00000008
base address 1 =			0x00000000
base address 2 =			0x20000008
base address 3 =			0x00000000
base address 4 =			0xa0800008
base address 5 =			0x00000000
cardBus CIS pointer =			0x00000000
sub system vendor ID =			0x0000
sub system ID =				0x0000
expansion ROM base address =		0x00000000
interrupt line =			0x00
interrupt pin =				0x01
min Grant =				0x00
max Latency =				0x00
.CE

(2) pciCfgShowAll()

.CS
PCI Device bus=0 dev=0 func=0
Class: Memory Controller  Subclass: Other Memory Controller
00000000: ab 11 60 64 17 02 b0 22  03 00 80 05 08 40 80 00   ..`d...".....@..
00000010: 08 00 00 00 00 00 00 00  08 00 00 20 00 00 00 00   ........... ....
00000020: 08 00 80 a0 00 00 00 00  00 00 00 00 00 00 00 00   ................
00000030: 00 00 00 00 40 00 00 00  00 00 00 00 00 01 00 00   ....@...........
00000040: 01 48 09 7e 00 00 00 00  03 50 00 00 00 00 00 00   .H.~.....P......
00000050: 05 60 80 00 00 00 00 00  00 00 00 00 00 00 00 00   .`..............
00000060: 00 68 30 00 00 00 83 21  06 00 00 00 00 00 00 00   .h0....!........
00000070: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
00000080: ab 11 60 64 17 02 b0 22  03 00 80 05 08 40 80 00   ..`d...".....@..
00000090: 08 00 00 00 00 00 00 00  08 00 00 20 00 00 00 00   ........... ....
000000a0: 08 00 80 a0 00 00 00 00  00 00 00 00 00 00 00 00   ................
000000b0: 00 00 00 00 40 00 00 00  00 00 00 00 00 01 00 00   ....@...........
000000c0: 01 48 09 7e 00 00 00 00  03 50 00 00 00 00 00 00   .H.~.....P......
000000d0: 05 60 80 00 00 00 00 00  00 00 00 00 00 00 00 00   .`..............
000000e0: 00 68 30 00 00 00 83 21  06 00 00 00 00 00 00 00   .h0....!........
000000f0: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
.CE

(3) mv64360CPUWindowShow()

.CS
Window             Base-Address,Size
CS0                0x00000000,0x10000000
CS1                0x10000000,0x10000000
DevCS0             0xf4000000,0x04000000
DevCS1             0xf1d00000,0x00100000
BootCS             0xf8000000,0x08000000
PCI Bus 0 IO       0xa3000000,0x00800000
PCI Bus 0 Mem 0    0xa0000000,0x00800000
PCI Bus 0 Mem 1    0xa0800000,0x00800000
PCI Bus 0 Mem 2    0x80000000,0x20000000
PCI Bus 1 IO       0xa2800000,0x00800000
PCI Bus 1 Mem 0    0xa1000000,0x01000000
PCI Bus 1 Mem 1    0xa2000000,0x00800000
Integrated SRAM    0xf1100000
Internal Space     0xf1000000
.CE

(4) mv64360DumpRegs()

.CS
CPU Interface Registers:
CPUIF_CS0_BASE_ADDR                        @ 0xf1000008 = 0x00000000
CPUIF_CS0_SIZE                             @ 0xf1000010 = 0x00000fff
CPUIF_CS1_BASE_ADDR                        @ 0xf1000208 = 0x00001000
CPUIF_CS1_SIZE                             @ 0xf1000210 = 0x00000fff
CPUIF_CS2_BASE_ADDR                        @ 0xf1000018 = 0x00000000
CPUIF_CS2_SIZE                             @ 0xf1000020 = 0x00000000
CPUIF_CS3_BASE_ADDR                        @ 0xf1000218 = 0x00000000
CPUIF_CS3_SIZE                             @ 0xf1000220 = 0x00000000
CPUIF_DEVCS0_BASE_ADDR                     @ 0xf1000028 = 0x0000f400
CPUIF_DEVCS0_SIZE                          @ 0xf1000030 = 0x000003ff
CPUIF_DEVCS1_BASE_ADDR                     @ 0xf1000228 = 0x0000f1d0
CPUIF_DEVCS1_SIZE                          @ 0xf1000230 = 0x0000000f
CPUIF_DEVCS2_BASE_ADDR                     @ 0xf1000248 = 0x00000000
CPUIF_DEVCS2_SIZE                          @ 0xf1000250 = 0x00000000
CPUIF_DEVCS3_BASE_ADDR                     @ 0xf1000038 = 0x00000000
CPUIF_DEVCS3_SIZE                          @ 0xf1000040 = 0x00000000
.CE

(5) intConnectShow()

.CS
   Vector :  Handler  (Arg)

 14 (0x0e):  0x123724 (0x0)
 32 (0x20):  0x12734c (0x1df49950)
 56 (0x38):  0x107cc4 (0xf104)
 57 (0x39):  0x107cc4 (0xf104)
 58 (0x3a):  0x107cc4 (0xf104)
 59 (0x3b):  0x107cc4 (0xf104)
 64 (0x40):  0x135dd8 (0x22a8ec)
             0x135dd8 (0x22a934) - chained
 71 (0x47):  0x1274d0 (0x1df49950)
 84 (0x54):  0x10d338 (0x0)
 96 (0x60):  0x10d51c (0x1)
 97 (0x61):  0x10d51c (0x2)
 98 (0x62):  0x10d51c (0x3)
 99 (0x63):  0x10d51c (0x4)
100 (0x64):  0x10d51c (0x5)
101 (0x65):  0x10d51c (0x6)
102 (0x66):  0x10d51c (0x7)
.CE

(6) sysPhysMemDescShow()

.CS
Entry 0:
  void *virtualAddr     = 0x00000000
  void *physicalAddr    = 0x00000000
  void len              = 0x00004000
  void initialStateMask = 0x0000007f
  void initialState     = 0x00000055

Entry 1:
  void *virtualAddr     = 0x00004000
  void *physicalAddr    = 0x00004000
  void len              = 0x0000c000
  void initialStateMask = 0x0000007f
  void initialState     = 0x00000055

Entry 2:
  void *virtualAddr     = 0x00010000
  void *physicalAddr    = 0x00010000
  void len              = 0x00030000
  void initialStateMask = 0x0000007f
  void initialState     = 0x00000055
.CE

(7) memoryMapShow()

.CS
VME_MEM_LOCAL_START                      = 0x80000000

VME_A32_MSTR_LOCAL                       = 0x80000000
VME_A32_MSTR_BUS                         = 0x08000000
VME_A32_MSTR_SIZE                        = 0x10000000
VME_A32_MSTR_END                         = 0x90000000

VME_A24_MSTR_LOCAL                       = 0x90000000
VME_A24_MSTR_BUS                         = 0x00000000
VME_A24_MSTR_SIZE                        = 0x01000000
VME_A24_MSTR_END                         = 0x91000000

VME_A16_MSTR_LOCAL                       = 0x91000000
VME_A16_MSTR_BUS                         = 0x00000000
VME_A16_MSTR_SIZE                        = 0x00010000
VME_A16_MSTR_END                         = 0x91010000

VME_MEM_LOCAL_END                        = 0x91010000
VME_MEM_SIZE                             = 0x20000000


PCI_MEM_LOCAL_START                      = 0xa0000000

PCI0_MEM_LOCAL_START                     = 0xa0000000

PCI0_MSTR_MEMIO_LOCAL                    = 0xa0000000
PCI0_MSTR_MEMIO_BUS                      = 0xa0000000
PCI0_MSTR_MEMIO_SIZE                     = 0x00800000
PCI0_MEMIO_LOCAL_END                     = 0xa0800000

PCI0_MSTR_MEM_LOCAL                      = 0xa0800000
PCI0_MSTR_MEM_BUS                        = 0xa0800000
PCI0_MSTR_MEM_SIZE                       = 0x00800000
PCI0_MEM_LOCAL_END                       = 0xa1000000

PCI1_MEM_LOCAL_START                     = 0xa1000000

PCI1_MSTR_MEMIO_LOCAL                    = 0xa1000000
PCI1_MSTR_MEMIO_BUS                      = 0xa1000000
PCI1_MSTR_MEMIO_SIZE                     = 0x01000000
PCI1_MEMIO_LOCAL_END                     = 0xa2000000

PCI1_MSTR_MEM_LOCAL                      = 0xa2000000
PCI1_MSTR_MEM_BUS                        = 0xa2000000
PCI1_MSTR_MEM_SIZE                       = 0x00800000
PCI1_MEM_LOCAL_END                       = 0xa2800000

PCI_MEM_LOCAL_END                        = 0xa2800000

PCI_IO_LOCAL_START                       = 0xa2800000

PCI1_IO_LOCAL_START                      = 0xa2800000

ISA_MSTR_IO_LOCAL                        = 0xa2800000
ISA_MSTR_IO_BUS                          = 0x00000000
ISA_MSTR_IO_SIZE                         = 0x00010000

PCI1_MSTR_IO_LOCAL                       = 0xa2810000
PCI1_MSTR_IO_BUS                         = 0x00010000
PCI1_IO_SIZE                             = 0x00800000
PCI1_IO_LOCAL_END                        = 0xa3000000

PCI0_IO_LOCAL_START                      = 0xa3000000
PCI0_MSTR_IO_LOCAL                       = 0xa3000000
PCI0_MSTR_IO_BUS                         = 0x00800000
PCI0_MSTR_IO_SIZE                        = 0x00800000

PCI0_IO_LOCAL_END                        = 0xa3800000

PCI_IO_LOCAL_END                         = 0xa3800000
.CE

(8) translationWindowShow()

.CS
           sysPhbCpuToPciWin       sysPhbPciToCpuWin

Window 0
  winType    0x00000000              0x00000000
  winBase    0xa0000000              0xa0000000
  winLimit   0xa07fffff              0xa07fffff

Window 1
  winType    0x00000000              0x00000000
  winBase    0xa0800000              0xa0800000
  winLimit   0xa0ffffff              0xa0ffffff

Window 2
  winType    0x00000000              0x00000000
  winBase    0x80000000              0x80000000
  winLimit   0x9fffffff              0x9fffffff

Window 3
  winType    0x00000001              0x00000001
  winBase    0xa3000000              0x00800000
  winLimit   0xa37fffff              0x00ffffff

Window 4
  winType    0x00000000              0x00000000
  winBase    0xa1000000              0xa1000000
  winLimit   0xa1ffffff              0xa1ffffff

Window 5
  winType    0x00000000              0x00000000
  winBase    0xa2000000              0xa2000000
  winLimit   0xa27fffff              0xa27fffff

Window 6
  winType    0x00000001              0x00000001
  winBase    0xa2800000              0x00000000
  winLimit   0xa2ffffff              0x007fffff

Window 7
  winType    0x00000000              0x00000000
  winBase    0x00000000              0x00000000
  winLimit   0x0fffffff              0x0fffffff

Window 8
  winType    0x00000000              0x00000000
  winBase    0x10000000              0x10000000
  winLimit   0x1fffffff              0x1fffffff

Window 9
  winType    0x00000000              0x00000000
  winBase    0x00000000              0x00000000
  winLimit   0x0fffffff              0x0fffffff

Window 10
  winType    0x00000000              0x00000000
  winBase    0x10000000              0x10000000
  winLimit   0x1fffffff              0x1fffffff
.CE

(9) vpdShow()

.CS
Type = VPD Revision (packet type = 0x0f)
 Size = 0x04
Value = 00 03 00 00 

 Type = External Clock Speed (in HZ) (packet type = 0x06)
 Size = 0x05
Value = 133333333

 Type = Product Identifier (packet type = 0x01)
 Size = 0x08
Value = MVME6100

 Type = Factory Assembly Number (packet type = 0x02)
 Size = 0x0c
Value = 01-W3878F02B

 Type = Serial Number (packet type = 0x03)
 Size = 0x07
Value = 7099387

 Type = L2 Cache Configuration (packet type = 0x0e)
 Size = 0x0f
Value = FF FF FF FF FF FF FF FF 
        FF 01 FF 01 FF FF FF 

 Type = L3 Cache Configuration (packet type = 0x19)
 Size = 0x0a
Value = 01 01 00 02 03 00 00 00 
        01 00 
Type = Microprocessor Type (packet type = 0x09)
 Size = 0x04
Value = 7457

 Type = EEPROM CRC (packet type = 0x0a)
 Size = 0x04
Value = E8C860E0

 Type = Ethernet Address (packet type = 0x08)
 Size = 0x07
Value = 00 01 AF 0D 86 10 00 

 Type = Ethernet Address (packet type = 0x08)
 Size = 0x07
Value = 00 01 AF 0D 86 11 01 

 Type = FLASH Memory Configuration (packet type = 0x0b)
 Size = 0x0c
Value = 00 89 00 03 10 02 02 10 
        10 00 78 08

 Type = FLASH Memory Configuration (packet type = 0x0b)
 Size = 0x0c
Value = 00 89 00 03 10 02 02 10 
        10 01 78 08 

 Type = Termination Marker (packet type = 0xff)
.CE

(10) sysMv64360DmaWindowsShow()

.CS
Window  Base-Address,Size  Remap    Attributes
0       00000000,10000000  00000000 DRAM
                                    CS[0]
                                    WB cache coherency
1       10000000,10000000  00000000 DRAM
                                    CS[1]
                                    WB cache coherency
4       a3000000,00800000  None     PCI 0
                                    No Swap
                                    PCI-X NS Not Asserted
                                    PCI I/O space
                                    REQ64# by Data Size
5       a2800000,00800000  None     PCI 1
                                    No Swap
                                    PCI-X NS Not Asserted
                                    PCI I/O Space
                                    REQ64# by Data Size
6       a0000000,00800000  None     PCI 0
                                    No Swap
                                    PCI-X NS Not Asserted
                                    PCI memory space
                                    REQ64# by Data Size
7       a1000000,01000000  None     PCI 1
                                    No Swap
                                    PCI-X NS Not Asserted
                                    PCI Memory Space
                                    REQ64# by Data Size
.CE

(11) sysMv64360SmcShow()

.CS
MV64360 DDR SDRAM Controller Registers:
---------------------------------------
Chip Select 0 Base Addr:  0x00000000
Chip Select 0 Size:       0x00000FFF
Chip Select 1 Base Addr:  0x00001000
Chip Select 1 Size:       0x00000FFF
Chip Select 2 Base Addr:  0x00000000
Chip Select 2 Size:       0x00000000
Chip Select 3 Base Addr:  0x00000000
Chip Select 3 Size:       0x00000000
Base Address Enable:      0x000620CC
SDRAM Configuration:      0x8024840D
SDRAM Timing (Low):       0x11501220
SDRAM Timing (High):      0x00000009
SDRAM Address Control:    0x00000012
SDRAM Open Pages Control: 0x00000000
SDRAM Mode:               0x00000022
SDRAM IF Xbar Ctrl(Low):  0x00765432
SDRAM IF Xbar Ctrl(High): 0x00765432
SDRAM IF Xbar Timeout:    0x000100FF
SDRAM Error Data (Low):   0x00000000
SDRAM Error Data (High):  0x00000000
SDRAM Error Address:      0x00000000
SDRAM Received ECC:       0x00000000
SDRAM Calculated ECC:     0x00000000
SDRAM ECC Control:        0x00000000
SDRAM ECC Counter:        0x00000000
.CE

(12) sysMv64360SpdShow()

.CS
I2C Address: A0
---------------
Byte 0 = 0x80
Byte 1 = 0x07
Byte 2 = 0x07
Byte 3 = 0x0D
Byte 4 = 0x0A
Byte 5 = 0x01
Byte 6 = 0x48
Byte 7 = 0x00
...
.CE

(13) sysTempeShow()

.CS
PCI Configuration: Header

TEMPE_DEVI_VENI            @ 0xA0000000 = 0xE3104801
TEMPE_STAT_CMMD            @ 0xA0000004 = 0x06003002
TEMPE_CLAS_REVI            @ 0xA0000008 = 0x01008006
TEMPE_HEAD_MLAT_CLSZ       @ 0xA000000C = 0x00800000
TEMPE_MBARL                @ 0xA0000010 = 0x040000A0
TEMPE_MBARU                @ 0xA0000014 = 0x00000000
TEMPE_SUBI_SUBV            @ 0xA000002C = 0xE3100000
TEMPE_CAPP                 @ 0xA0000034 = 0x40000000
TEMPE_MXLA_MNGN_INTP_INTL  @ 0xA000003C = 0x54010000

PCI Configuration: PCI-X Capabilities

TEMPE_PCIXCAP              @ 0xA0000040 = 0x07000000
TEMPE_PCIXSTAT             @ 0xA0000044 = 0x28007305
.CE

(14) sysTempeOutWinShow()

.CS
Outbound window 0: ENABLED,  Size = 00000000_00010000

raw: 00000000 8fff0000 00000000 8fff0000 00000000 6b010000 00000000 80040042
PCI Base:  00000000_8fff0000    VME Base:  00000000_fb000000
PCI Limit: 00000000_8fffffff    VME Limit: 00000000_fb00ffff
     2eSST Broadcast Select: 0x00000
     Attributes:
        Memory Prefetch      - disabled
        Prefetch size        - 2 cache lines
        2eSST Mode           - 160 MB/s
        Transfer Mode        - SCT (Single Cycle Transfer)
        VME Data Bus Width   - 32 bit
        VME AM code          - nonSupervisor, nonProgram
        VME Address mode     - A32
.CE

(15) sysTempeInWinShow()

.CS
Inbound window 0: ENABLED,  Size = 00000000_00040000

raw: 00000000 08000000 00000000 0803fff0 ffffffff f8000000 80000faf
VME Base:  00000000_08000000    PCI Base:  00000000_00000000
VME Limit: 00000000_0803ffff    PCI Limit: 00000000_0003ffff
     Attributes:
        Read-ahead threshold - when FIFO competely empty
        Virtual FIFO size    - 64 bytes
        2eSST Mode           - 160 MB/s
        Cycle response       - 2eSSTB (Two Edge Source Synchronous Broadcast)
                               2eSST (Two Edge Source Synchronous nonBroadcast)
                               2eVME (Two Edge VMEbus)
                               MBLT (Multiplexed Block Transfer)
                               BLT (Block Transfer)
        Address space        - A32
        VME AM response      - Supervisor, nonSupervisor, Program, Data
.CE

.SS "Additional Utilities"

The following utilities are provide for reading from and writing to 
specific registers within the processor:

.TS C
center;
rw(1.0i) lw(3.7i) .
sysL2crGet	T{
read from L2CR register
T}
sysMsscr0Get	T{
read from MSSCR0 register
T}
sysL3crGet	T{
read from L3CR register
T}
sysL3ohcrGet	T{
read from L3OHCR register
T}
sysL3itcr0Get	T{
read from L3ITCR0 register
T}
sysL3itcr1Get	T{
read from L3ITCR1 register
T}
sysL3itcr2Get	T{
read from L3ITCR2 register
T}
sysL3itcr3Get	T{
read from L3ITCR3 register
T}
sysTimeBaseLGet	T{
Get lower half of Time Base Register
T}
sysTimeBaseUGet	T{
Get upper half of Time Base Register
T}
sysHid1Get	T{
read from HID1 register
T}
sysHid0Get	T{
read from HID0 register
T}
sysGetDec	T{
read from the Decrementer register SPR22
T}
sysL3pmGet	T{
read from L3PM (Private Memory Control Register)
T}
sysPvrGet	T{
read from PVR (Processor Version Register) 
T}
.TE

.SS "Board Layout"

Keep in mind that jumpers J10, J15-J18, J25-J28, and J30 only exist on
01-W3878F versions of the MVME6100.  They have been replaced with 
resistors on the 01-W3912F version of the MVME6100.

.ne 4i

.bS
   --                         MVME6100
   \  \___________________________________________________________________
    | |                                                      ....J29     |
   /  |         ... J7 (VME system controller)               .....       -==
   ---|                                                   ||     ||      -==
      |                                      ------       ||     ||      -==
      |                                      | S4 |       ||     ||      -==
      |                                      ------       ||     ||      -==
      |                                                   ||     ||      -==
      |                                      ------       ||     ||      -==
      |                                      |    |       ||     ||      -==
      |                                      | S3 |       ||     ||      -==
      |                                      |    |                      -==
      |                                      ------       ||     ||      -==
      |                                                   ||     ||      -==
      |                                           PMC     ||     ||      -==
      |                                        Connector  ||     ||      -==
      |                                                   ||     ||      -==
      |                                                   ||     ||      -==
      |                                                   ||     ||      -==
      |                                                   ||     ||      -==
      |                                                                  -==
      |                                                                  -==
      |                                                   || ... ||      |
      |                                                   || ... ||      |
      |                                                   || ... ||      |
      |                                                   || ... ||      |
      |                                                   || ... || J30  |
      |                                                   || ... || .... |
      |                                                   || ... || .... |
      |                                           PMC     || ... || .... |
      |                                        Connector     ...    .... |
      |                                                   || J10 || .... |
      |                                                   ||J15- || .... |
      |                                                   || J18 || .... |
      |                                                   ||J25- || .... |
      |                                                   || J28 || .... |    
      |                                                   ||     || .... |
      |                                                   ||     ||      -==
      |                                                   ||     ||      -==
      |                                                                  -==
      |                                                                  -==
      |                                                                  -==
      |                                                                  -==
      |                                                                  -==
      |-                                                                 -==
GigE    |                                                                -==
 geisc1 |                                                                -==
      |-                                                                 -==
      |-                                                                 -==
GigE    |                                                                -==
 geisc0 |                                                                -==
      |-                                                                 -==
      |-                                                                 -==
        |                                                                -==
COM0    |                                                                -==
      |-                                                                 -==
   ---|                                                                  -==
   \  |                                                                  |
    | |___________________________________________________________________
   /  /
   ---
.bE


.ne 4i

.SH "SEE ALSO"
.tG "Getting Started,"
.pG "Configuration"

.SH "BIBLIOGRAPHY"
.iB "Motorola Computer Group Online Documentation,"
.iB "Motorola Engineering Specification and Programming Model for MVME6100,"
.iB "Motorola PowerPC 60X RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "IEEE P1386.1 Draft 2.0 - PCI Mezzanine Card Specification (PMC),"
.iB "IEEE P1386 Draft 2.0 - Common Mezzanine Card Specification (CMC),"
.iB "Motorola MPC7450 RISC Microprocessor Family User's Manual, Rev 2,"
.iB "Marvell MV64360 System Controller for PowerPC Processors Manual, Rev 0.98,"
.iB "Peripheral Component Interconnect (PCI) Local Bus Specification, Rev 2.1,"
