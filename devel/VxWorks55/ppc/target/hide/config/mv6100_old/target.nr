'\" t
.so wrs.an
.\" MVME6100/target.nr - Motorola MVME6100 target-specific documentation
.\"
.\" Copyright 1984-2000 Wind River Systems, Inc.
.\" Copyright 2004 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01bb17may04,cak  Added blurb about "Relocation value does not fit 
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

.SS "Boot ROMS"
The MV6100 supports two banks of FLASH memory (A and B).  Each bank consists 
of two Intel StrataFlash 3.3 volt devices configured to operate in 16-bit mode,
to form a 32-bit FLASH port, and are soldered onboard.  Either bank can be 
used as the boot bank, which will be switch selectable.  The switch 
effectively swaps the chip selects (MV64360) of the two FLASH banks.
The OFF position (default) will select FLASH bank A as the boot bank.
The ON position will select FLASH bank B.  The boot bank (either A or B) 
is always located at 0xF8000000. The alternate bank (either A or B) is 
located at 0xF4000000. 

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
Front panel GigE (port 2), install jumpers across (A3,B3) through (A10,B10).
For rear ethernet (100TX), install jumpers across (B3,C3) through (B6,C6).
For rear ethernet (1000TX), install jumpers across (B3,C3) through (B10,C10).
For IPMC761 mode, install jumpers across (B1,C1) through (B2,C2)(IPMC is not
supported at this time).
For IPMC712 mode, install jumpers across (B1,C1) and across (C2,D2)(IPMC is not
supported at this time).
For PMC mode to P2, install jumpers across (C1,D1) through (C10,D10).
T}
J10,J15-J18,J25-J28	PMC/IPMC IO Select	T{
Install the jumper across pins 1 and 2 to select PMC2 P2 IO for PMC Mode.
Install the jumper across pins 2 and 3 to select IPMC P2 IO (extended SCSI) for IPMC Mode (IPMC is not supported at this time).
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
Feature Description
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
of the flash has to be used for boot image.
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
To ensure sure that VxWorks shared memory does not use sysVmeBusLockTas(),
ANY_BRD_IN_CHASSIS_NOT_RMW should bue #undef'ed.  #undef'ing this
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
which is hosts the semaphores.  It is referred to as the master node.
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
Tempe (Tsi148):	T{
Tempe PCI-X to VME support, include DMA.
T}
m48t37:	T{
M48T37 Timekeeper SRAM device driver.
T}
ns8730xSuperIo:	T{
Super I/O controller driver (hardware does not support this at this time).
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
Zilog 8536 UART driver (hardware does not support this at this time).
T}
sym895Lib(IPMC):	T{
SYM53C895A SCSI driver (hardware does not support this at this time).
T}
.TE

.SS "Memory ECC Protection"

This BSP supports ECC memory and, by default, enables ECC support in the 
system memory controller by defining INCLUDE_ECC in config.h.  To disable 
ECC support simply change #define INCLUDE_ECC, in config.h, to 
#undef INCLUDE_ECC.  Disabling ECC support will result in a slight
performance increase, since partial (less than 64-bit) writes to memory 
will not require a RMW transaction as described in section "8.6 Read Modify 
Write" in Rev 0.98 of the MV64360 System Controller manual. 

.SS "TrueFFS Flash File System"

This BSP supports TrueFFS Flash File System on either, or both, FLASH bank
A and FLASH bank B.  TFFS is disabled by default.  To enable TFFS for FLASH
bank A change #undef INCLUDE_TFFS_FLASHA, in config.h, to 
#define INCLUDE_TFFS_FLASHA.  To enable TFFS for FLASH bank B change
#undef INCLUDE_TFFS_FLASHB, in config.h, to #define INCLUDE_TFFS_FLASHB.
The size is set to 32MB.  We do this because the FLASH banks
are each 64MB in size, but in order to protect any boot images
that may reside in the 63rd MB of the FLASH bank - either firmware
or another VxWorks image, and also conform to the TFFS restriction that
the flash bank size must a power of 2 (1,2,4,8,16,32,...) we are not
able to utilize more than 32 MB of the FLASH.

Note: Support is also provided to automatically mount the TFFS drives
during kernel initialization, if TFFS support is enabled.

.SS "PCI Dynamic Allocation Spaces"
PCIx_MSTR_IO_SIZE, PCIx_MSTR_MEMIO_SIZE, PCIx_MSTR_MEM_SIZE, and 
ISA_MSTR_IO_SIZE (where "x" is "0" or "1" relating to MV64360 bus 0.0 and 1.0)
control the sizes of the available PCI address spaces. There is one set of
definitions for each bus.  The windows defined by these parameters must be 
large enough to accommodate all of the PCI memory and I/O space requests 
found during PCI autoconfiguration. If they are not, some devices will not be 
autoconfigured.  These definitions can be found in config.h.

.TS C
center;
l l .
\f3NOTE:\f1	T{
PCI auto-configuration is performed by the bootroms. Any changes to
PCIx_MSTR_IO_SIZE, PCIx_MSTR_MEMIO_SIZE, PCIx_MSTR_MEM_SIZE, or 
ISA_MSTR_IO_SIZE (where "x" is "0" or "1") requires the creation of a new 
bootrom image.
T}
.TE

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

Shared memory resource mutual exclusion is implemented in the sysBusTas
function.  Two modes of hardware assist are available to implement
the test and set function across the VME bus.  They are: bus locking
and RMW cycle generation.  The method used is selected in config.h
via the #define/#undef state of ANY_BRDS_IN_CHASSIS_NOT_RMW.  See
config.h for details.  Also under "Known Problems" in this document
a restriction on the choice of hardware assist for Tsi148 Rev 1 equipped
boards is described.

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
`0x81 - 0xbf'   T{
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

The MV64360 interrupt controller does not directly support interrupt
priorities.  Software configuration can be performed however, which
affects the order in which interrupts sources are checked upon
occurrence of an external processor interrupt.  The #define for
ICI_MAIN_INT_PRIORITIES specifies the order in which MV64360 main
interrupt cause bits are checked.  

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
Pin#	Vector# I/O	Polarity	Source
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

When one of these four main interrupt cause bits indicates a GPP
interrupt, the interrupt handler will expect to see one or more of the
interrupt bits from the above GPP set to be active.  The order in
which the GPP interrupt set is checked is governed by the #define
GPP_LOCAL_INT_PRIORITIES.

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

At this time the MVME6100 does not support IPMC.  However, software support
still remains.  INCLUDE_IPMC (mv6100A.h) must remain #undef'ed.

.SS "Serial Configuration"
The MVME6100's two asynchronous serial interfaces are provided by the 16550
compatible UART controllers in the MV64360.  COM0 is wired to the front panel
RJ45 connector.  COM1 is wired to a ten-pin 0.100" planar header located on
the MVME6100.  

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"

At this time the MVME6100 does not support SCSI.  However, software support
still remains.  See "IPMC Devices" above.

SCSI is implemented via the IPMC module.  To include SCSI support change
#undef INCLUDE_IPMC, in config.h, to #define INCLUDE_IPMC.

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

.SS "MV64360 DMA Configuration"
To enable DMA support using the MV64360, change the #undef INCLUDE_MV64360_DMA
in config.h to #define.  Instructions for configuring and using DMA functions
are contained in mv64360Dma.c and mv64360Dma.h. 

.SS "Tempe (Tsi148) DMA Configuration"
No special configuration switches need to be modified to configure Tempe
(Tsi148) DMA support.  The file "sysTempeDma.c" contains some example
code for invoking Tempe DMA.

.SS "Network Configuration"
The MV64360 provides two 10/100/1000 bps full duplex ethernet ports.   
Port 0 (LAN1) is accessible from a front panel RJ45 connector. Port 1
(LAN2) is accessible from either a front panel RJ45 connector or can
be configured for rear P2 access - controlled by jumper J30.  Both ports 
are defined and initialized by default. To disable the MV64360 ethernet, 
INCLUDE_GEISC_END must be undefined in config.h. Additionally, the 
Makefile must be modified to remove the driver object file mv64360End.obj 
from the MACH_EXTRA line to allow error-free compilation.

The ethernet driver automatically senses and configures a port as 10baseT,
100baseT, or 1000baseT based on the maximum capability advertised by the 
port's network link partner.   

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

Support for the AuxClock is provided.  The AuxClock is implemented with the
MV64360 Timer/Counter 0.

AuxClock support can be included in the BSP by defining INCLUDE_AUXCLK in
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
boot banks - this is controller via switch S4.  Then power the board back up.

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

.SS "Special Routines"

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
.TE

.SH "Board Layout"

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
