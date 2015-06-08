'\" t
.so wrs.an
.\" mv5100/target.nr - Motorola PowerPlusII target-specific documentation
.\"
.\" Copyright 1984-2001 Wind River Systems, Inc.
.\" Copyright 1996-2001 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01a,17apr02,kab  Update CPU types
.\" 01o,13mar02,kab  Update TFFS; add HawkII to KPL
.\" 01n,28oct01,kab  SPR #70441: fei replace er; VME map reserved mem;
.\"		     Wide SCSI cannibalizes PMC slot 2 J23.
.\" 01m,04sep01,efb  Added note about Universe II's location monitors being
.\"                  available for user applications.
.\" 01l,25apr01,srr  Cleanup as per WRS code review.
.\" 01k,09mar01,rhk  Minor cleanup, removed warning about use of the
.\" 		     ANY_BRDS_IN_CHASSIS_NOT_RMW define.
.\" 01j,26sep00,cak  Corrected Boot ROM details.
.\" 01i,26sep00,cak  Update TrueFFS reference for TrueFFS support patch.
.\" 01h,14sep00,cak  Added TrueFFS reference.
.\" 01g,16aug00,dmw  Update following Wind River coding standards review.
.\" 01f,07aug00,dmw  Corrected pflash instructions.
.\" 01e,26may00,dmw  Added RTC Watchdog timer.
.\" 01d,15may00,dmw  More information regarding IPMC761 in Devices section.
.\" 01c,12may00,dmw  Section regarding i8250Sio driver in Known Limitations.
.\" 01b,12may00,dmw  Cleanup following review.
.\" 01a,13mar00,dmw  Written (from version 01l of mcpn765/target.nr).
.\"
.\"
.TH "mv5100" T "Motorola PowerPlusII" "Rev: 18 Apr 02" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola MVME51xx"

.SH "INTRODUCTION"
This manual entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.

This BSP encompasses only the MVME5100 single-board computer. MVME5100
boards require no transition module.  The IPMC761 optional PMC module will
allow a standard 761 transition module to be used.  The series part numbers 
are of the form:

    MVME51uw-x1yz

    where

        uw = CPU Speed
           00 = 450MHz 750
           01 = 400MHz 7400
	   06 = 4xxMHz 755
	   10 = xxxMHz 7410

        x = L2 Cache
           0 = 1 MB
           2 = 2 MB

        y = ECC SDRAM size
           3 =  64 MB
           4 = 128 MB
           6 = 512 MB

        z = Front panel
           1 = Scanbe
           3 = IEEE 1101

.SS "Boot ROMS"
The MVME5100 boards have two sets of flash EEPROM (FLASH).  One set of two
AMD Am29F040 FLASH is socketed (sockets XU1 and XU2) and contains Motorola's
PPC6-Bug.  The other set of AM29LV323 FLASH is soldered in on reference
designators U48, U64, U65, and U66.  The VxWorks boot kernel 
typically resides in the 
soldered FLASH. See \f2Hardware Details: ROM Considerations\f1 for 
information about loading and writing the boot kernel image to the soldered 
FLASH.

These boards have non-volatile RAM; thus, boot parameters are preserved
whenever the system is powered off.

To load VxWorks, and for more information, follow the instructions in the
\f2Tornado User's Guide: Getting Started.\f1

.SS "Jumpers"
The following jumpers are relevant to VxWorks configuration: 

.TS E
expand;
lf3 lf3 lf3
l l lw(2.6i) .
.ne 28
.sp .5
Jumper	Function	Description
_
J1	Riscwatch Header	T{
Install Riscwatch debugger to this connector.
T}
J2  PAL programming header for manufacturing.	T{
To be used by Motorola Computer Group Manufacturing only.
T}
J10, J17  Front Panel Ethernet port2.	T{
Jumper both 1,3 and 2,4 for front panel Ethernet port2.
Jumper both 3,5 and 4,6 for P2 Ethernet port2.
T}
J4  P2 Ethernet port2.	T{
Jumper pins 1,2; 3,4; 5,6; and 7,8 for P2 Ethernet.  For front panel
port2 Ethernet, install NO jumpers at J4
T}
J5  COM2 serial port.	T{
Header for Hawk's X-bus COM2 UART.
T}
J6, J20	IPMC761 mode.	T{
On both 1x3 header jumper the same.  Only jumper 1,2 for PMC mode.
Jumper 2,3 for IPMC761 mode.
T}
J7	ROM controller	T{
Install the jumper across pins 2 and 3 to select the socketed FLASH.
Install the jumper across pins 1 and 2 to select the soldered FLASH
[factory configuration].
T}
J15	System Controller	T{
Install the jumper across pins 1 and 2 for no SYSCON.
Install the jumper across pins 2 and 3 for auto SYSCON.
Install NO jumpers for Always SYSCON.
T}
J16	Flash protection header	T{
Install the jumper across pins 1 and 2 to enable flash programming.
Install the jumper across pins 2 and 3 to disables flash programming.
T}
.TE

For details of jumper configuration, see the board diagram at the end of
this entry and in the hardware manual.

Note that ROM controller jumpers should be set to select socketed FLASH until
VxWorks boot code is written to soldered FLASH, after which the jumpers should
be restored to the factory configuration of soldered FLASH.

.SH "FEATURES"
The following subsections list all supported and unsupported features, as well
as any feature interaction.

.SS "Supported Features"
The following features of the MVME51xx board family are supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 7
.sp .5
Feature	Description
_
Processors	T{
MPC750; 100MHz bus clock
MPC755; 100MHz bus clock
MPC7400; 100MHz bus clock
MPC7410; 100MHz bus clock
T}
FLASH	T{
1MB socketed (16-bit wide),
8MB soldered (32-bit wide)
T}
DRAM	T{
Up to 512MB SDRAM, expandable to 1GB with add-on mezzanine.
T}
NVRAM	T{
8KB (M48T37V)
T}
Peripherals	T{
two async serial debug ports,
two 10baseT/100baseTX Ethernet interfaces
T}
ISA Interface	T{
full 64KB memory and I/O space
T}
PCI Interface	T{
32-bit address, 32-bit data; complies with \f2PCI Local Bus Specification\f1,
Revision 2.1
T}
VME Interface	T{
32-bit address, 32-bit data PCI bus interface;
A32/A24/A16, D32/D16/D08 master and slave;
programmable interrupter and interrupt handler;
full system controller function;
two location monitor/signal registers;
DMA controller (in direct mode only).
T}
Miscellaneous	T{
RESET switch,
8-Bit Software Readable Header
T}
.TE

.SS "Unsupported Features"
The following features of the MVME51xx board family are not supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Feature	Description
_
Hawk	T{
Watchdog Timers
T}
RTC	T{
M48T37V; NVRAM and Watchdog Timer
T}
ISA Interface	T{
ISA RTC and DMA controllers
T}
PCI Interface	T{
64-bit data; The hardware will perform 64-bit transfers if requested by an
external PCI-master device, but does not generate 64-bit transactions in
normal operation.
T}
VME Interface	T{
D64(MBLT); DMA controller in linked list mode (only direct mode is supported).
T}
Miscellaneous	T{
ABORT switch, 4 status LEDs
T}
.TE

.SS "Feature Interactions"
None known.

.SH "HARDWARE DETAILS"
This section details device drivers and board hardware elements.

.SS "Devices"
The device drivers and libraries included with this BSP are:

.nf
    `i8250Sio' - Intel 8250 UART driver (debug port)
    `ppcDecTimer' - PowerPC decrementer timer driver (system clock)
    `byteNvRam' - byte-oriented generic non-volatile RAM driver
    `hawkAuxClk' - Motorola Hawk auxiliary clock library.
    `hawkI2c' - Motorola Hawk I2C serial EEPROM driver.
    `hawkMpic' - Motorola Hawk MPIC interrupt controller driver
    `m48tWdt' - STMicroelectronics Watchdog Timer driver.
    `pciConfigLib' - PCI configuration library
    `pciAutoConfigLib' - PCI auto-configuration library
    `universe' - Tundra Universe chip VME-to-PCI interface driver
    `sysVpd' - Board-Specific Vital Product Data library
    `vpdUtil' - "Generic" Vital Product Data library
.fi

.SS "IPMC761 Devices"
The device drivers and libraries included for IPMC761 support are:

.nf
    `z8530Sio' - Zilog 8536 UART driver
    `sym895Lib' - SCSI driver
.fi

Note that the hardware and BSP must match in regards to the IPMC761 module.  
Do not attempt to define IPMC761 features without installing a IPMC761 module.
IPMC761 features should be defined when a IPMC761 is installed.  Jumpers J6
and J20 must match IPMC761 presence as well.

.SS "Memory Maps"
On-board RAM for these boards always appears at address 0x0 locally.
Its slave address on the VMEbus is set by registers in the Universe ASIC.
Local RAM-to-VMEbus mapping is defined in config.h

Dynamic memory sizing is supported.  By default, LOCAL_MEM_AUTOSIZE is
defined so memory is auto-sized at hardware initialization time.
If auto-sizing is not selected, LOCAL_MEM_SIZE must be set to the actual size
of DRAM memory available on the board to ensure all memory is available
and VME addressing occurs properly.  The default fixed RAM size is set to 32MB
(see LOCAL_MEM_SIZE in config.h).

There are two basic memory mappings. The default for Extended VMEbus access
is discussed here. Another for the optional pseudo-PReP memory model is
discussed under 
.I SPECIAL CONSIDERATIONS.

.SS "Extended VME Memory Model:" 1

The following table describes the address mapping created for the Extended VME
A32 model from the CPU point of view:

.TS E
expand;
lf3 lf3 lf3
l l lw(2.5i) .
.ne 6
.sp .5
Start	Size	Access to
_
0x0	LOCAL_MEM_SIZE	SDRAM
	(32MB - 512MB)

LOCAL_MEM_SIZE	T{
(VME_A32_MSTR_LOCAL - LOCAL_MEM_SIZE)
T}	[not used]

VME_A32_MSTR_LOCAL	128MB	T{
PCI MEM (A32 VME space)
T}
	0xEA000000 (max)

0xFA000000	16MB	T{
PCI MEM (A24 VME space)
T}
0xFB000000	64KB	T{
PCI MEM (VME REG. [A32] space)
T}
0xFB010000	0x00FE0000	[not used]
0xFBFF0000	64KB	T{
PCI MEM (A16 VME space)
T}
0xFC000000	256KB	T{
MPIC Reg space
T}
0xFC040000	0x00FC0000	[not used]
0xFD000000	8 MB	T{
PCI MEM I/O space
T}
0xFD800000	8 MB	T{
PCI MEM space
T}
0xFE000000	16KB	Legacy ISA I/O space
0xFE001000	4KB	T{
On-board VME Mailbox (inside ISA Legacy I/O space)
T}
0xFE004000	48KB	T{
16-bit PCI I/O space
T}
0xFE010000	8MB	T{
32-bit PCI I/O space
T}
0xFE810000	0x00770000	[not used]
0xFEF80000	128KB	T{
Hawk regs.
T}
0xF4000000	64MB	T{
ROM space (No PCI/VME)
T}
.TE

Note that the macro VME_A32_MSTR_LOCAL determines the base address for
PCI VME A32 addresses.  This value is 0x10000000 (256MB) by default and needs
to be adjusted by the user if there is more than 256MB of local DRAM in 
config.h.

In order to use the optional pseudo-PReP mapping configuration, simply change
the \f3#define\f1 EXTENDED_VME line to read \f3#undef\f1 EXTENDED_VME in
config.h.
Remember to set LOCAL_MEM_SIZE to the actual amount of DRAM on the board if
auto-sizing is not selected.  Failure to do so can cause unpredictable results
for A32 masters and slaves.

In order to modify the Extended VME mapping configuration, make the necessary
changes in config.h and, possibly, sysLib.c.

In config.h, \f3#define\f1 the VME window variables.

In sysLib.c, edit the sysPhysMemDesc[] page table to modify the A32 VME
window if you modify the sysBatDesc[] BAT register table.  The BAT registers
allow mapping of up to 1GB of data address space.  Although the BAT registers
are not supported in the current cache management strategy, you can use them
for non-cacheable, data-only address regions, like the VME A32 address space.

When changing modes -- for example, from standard VxWorks 
(pseudo-PREP-compliant
mapping) to Extended VME mapping -- all MVME51xx boards should be configured
the same way.  The kernels will not work together in a mixed configuration
unless the memory and VME mappings are compatible for all boards.

.SS "Shared Memory"
On all boards, shared memory across the backplane can also be used as a
network interface.  The name of the shared memory is `sm'.

Shared memory network communications requires a signaling method and a method
of mutually exclusive memory resource access.  Signalling can be done using
software polling or interrupts.  By default, mailbox interrupts are used and
SM_INT_TYPE is set to SM_INT_MAILBOX_1.  To use polling, \f3#define\f1
SM_INT_TYPE as SM_INT_NONE.

There are master and slave windows into VME address space to access the VME
mailbox registers so that each CPU can send and receive shared memory 
interrupts using single-byte mailboxes.
The windows map a 4KB region in A32 space at address 0xFB000000 + (0x1000 *
CPU #) into the Universe chip registers.  This configuration allows one
processor to generate a SIG1 interrupt in another processor by accessing the
other processor's mailbox register and setting the SIG1 bit.  Each CPU has a
master window covering the A32 addresses 0xFB000000 through 0xFB00ffff
representing CPU numbers 0 through 15.  Each CPU's slave window maps the
appropriate address for that CPU to the Universe chip's register set.

Note that the Universe II location monitors are no longer used for shared
memory interrupt notification. All four Universe II location monitors are 
available for user applications and have been left in their default state 
(disabled and unmapped).

Shared memory resource mutual exclusion (spin lock) is implemented using
test-and-set (TAS) and clear operations on byte-sized semaphores.
If the \f3#define\f1 SM_TAS_TYPE is set to SM_TAS_SOFT, only a software TAS
routine is used.  Software TAS is usually good enough for shared memory
networking; however, VxMP requires the use of hardware TAS.  Enable hardware
TAS by setting SM_TAS_TYPE to SM_TAS_HARD.  Hardware TAS and clear operations
are performed by the sysBusTas() and sysBusTasClear() routines, respectively,
and invoke pseudo-atomic operations.

True atomic operations are those which cannot be preempted at the hardware
level and appear on a bus as a single-cycle instruction.  Pseudo-atomic
operations are composed of multiple instruction cycles executed on a
bus that is locked (owned) by the processor executing the instructions.

The routine sysBusTas() performs pseudo-atomic TAS operations by disabling
interrupts (to prevent deadlocks) and locking ownership of the VMEbus.  This
routine waits up to 10 microseconds to lock the bus.  If bus ownership has not
been achieved at the end of this period, the routine returns FALSE, the same as
it would if the semaphore had already been set.

VMEbus ownership is necessary for a couple of reasons.  First, there is no
hardware support to propagate PowerPC atomic TAS instructions to the
Universe chip.  Second, these boards have no support for propagating
true atomic VME RMW cycles to local processor memory.

To ensure proper clearing of the semaphore, use the sysBusTasClear().  This
routine also disables interrupts and locks the VMEbus while accessing the
semaphore.  It waits up to 10 microseconds to gain bus ownership.  But, even if
the bus is not owned after this period, the routine attempts to clear the
semaphore.

If one board uses software TAS, then \f2all\f1 boards on a shared
memory backplane must use it.

When hardware TAS is enabled, special consideration must be given to the
overall system design and board locations in the VME card rack.  If all VME
boards on a backplane use the special hardware TAS methods utilized in this BSP,
there should be no problems.  If boards with differing TAS/RMW capabilities are
used together, then either the first (master) board, which hosts the shared
memory, must use the hardware TAS method utilized in this BSP, or the shared
memory must reside on a separate VME global memory board.

As an example of a hardware TAS system that cannot work, consider using a
Motorola MVME162 as the master board and an MVME5100 as a slave.  The mv162
BSP assumes that support exists for atomic TAS/RMW cycles on to and off of
all boards in the system.  Furthermore, the local 68040 CPU can access and 
alter its memory \f2between\f1 VMEbus cycles.  Therefore, this system
configuration does not work because there is no way to ensure atomic access
to a semaphore by the MVME5100 board.

.SS "Interrupts"
The system interrupt vector table has 256 entries.  Vectors for the various
devices on the buses are assigned hierarchically as follows:

.TS C
center;
lf3 lf3
l lw(2.6i) .
.ne 6
.sp .5
Vector#	Assigned to
_
00 - 0f	ISA IRQ numbers 0 - 15
10 - 1f	All MPIC interrupts
20 - 23	Hawk timers
24 - 27	Hawk interprocessor dispatch
   28  	Hawk detected internal errors
29 - 55	[User defined]
56 - 5f	Universe-specific interrupts
60 - ff	[User defined]
.TE

The specific ISA vector number assignments are:

.TS C
center;
lf3 lf3
l lw(2.6i) .
.ne 6
.sp .5
Vector#	Assigned to
_
   02	[Cascade interrupt from PIC2]
   03	SuperI/O asynchronous com port 2.
   04	SuperI/O asynchronous com port 1.
   09	Z8536/Z85230 com ports 1 and 2 (wired or'ed).
.TE

Vector numbers not in the table are not used by this BSP.  Note that
an optional IPMC761 must be installed for ISA vector support.

The standard ISA Intel 8259 Programmable Interrupt Controllers (PICs) assert
their interrupts through the HawkMPIC as an external interrupt.  The external
interrupt vector numbers are:

.TS C
center;
lf3 lf3
l lw(2.6i) .
.ne 14
.sp .5
Vector#	Assigned to
_
   10	ISA PICs (If IPMC761 is used)
   11	16550 UARTS port 1 and 2
   12	PCI Ethernet Port 1 (Front panel or P2)
   15	PCI Universe VME INT 0
   16	PCI Universe VME INT 1
   17	PCI Universe VME INT 2
   18	PCI Universe VME INT 3
   19	PCI PMC1/PCIX INTA, PMC2 INTB
   1a	PCI PMC1/PCIX INTB, PMC2 INTC
   1b	PCI PMC1/PCIX INTC, PMC2 INTD
   1c	PCI PMC1/PCIX INTD, PMC2 INTA
   1d	PCI Ethernet Port 2 (Front panel only)
   1f	RTC Watchdog Timer
.TE

Vector numbers not in the table are not used by this BSP.

The Hawk Multi-Processor Interrupt Controller (MPIC) sets system interrupt
priorities and serves as controller of all external interrupts.  Each
of its 16 interrupt control registers, designated IRQ0 through IRQ15, can be
programmed with a relative priority from 15, the highest, to 0, the lowest.  A
priority of zero effectively disables the interrupt.  All but one of the 16
control registers has been hardwired to a particular interrupt source.  The IRQ
number and priority assignments are as follows:

.TS E
expand;
lf3 lf3 lf3
l l lw(2.6i) .
.ne 6
.sp .5
Hawk MPIC IRQ	Priority	IRQ Source
_
IRQ0	9	Winbond PIB [all ISA interrupts]
IRQ1	8	COM1/COM2
IRQ2	14	Ethernet Port 1
IRQ3	0	Hawk WDT1/WDT2
IRQ4	N/A	[Not Used]
IRQ5	10	Universe LINT0 [all Universe/VME interrupts]
IRQ6	0	Universe LINT1
IRQ7	0	Universe LINT2
IRQ8	0	Universe LINT3
IRQ9	7	PCI PMC1/PCIX INTA, PMC2 INTB
IRQ10	7	PCI PMC1/PICX INTB, PMC2 INTC
IRQ11	7	PCI PMC1/PICX INTC, PMC2 INTD
IRQ12	7	PCI PMC1/PICX INTD, PMC2 INTA
IRQ13	13	Ethernet Port 2
IRQ14	0	Abort_L
IRQ15	15	RTC Watchdog timer
.TE

For further details, refer to the appropriate board's reference guide.

There are only four PCI bus interrupts: A, B, C, and D.  They are shared among
all PCI bus devices and do not have levels.  PCI bus interrupts are wired
directly to the MPIC and, therefore, have pre-assigned system vector numbers
and interrupt levels.  The user enables one or more PCI interrupts and connects
vectored ISRs to the system by following these steps:

.IP "1)"
Identify the PCI interrupt letter(s) as required by the  application. Based on 
this, identify the associated system interrupt level from the following tables:

            Primary PCI Bus
            ----------------
            A = PMC_INT_LVL1
            B = PMC_INT_LVL2
            C = PMC_INT_LVL3
            D = PMC_INT_LVL4

            Secondary PCI Bus
            -----------------
            A = PMC_INT_LVL4
            B = PMC_INT_LVL3
            C = PMC_INT_LVL2
            D = PMC_INT_LVL1

.IP "2)"
Define the vector for each PCI interrupt as follows:
INT_VEC_IRQ0 + PMC_INT_LVLx where x is 1, 2, 3, or 4, as determined above.
.IP "3)"
In the application code, perform intConnect() for
each vector and its associated ISR.
.IP "4)"
Perform sysIntEnable() for each identified system interrupt level.
.IP "5)"
When the application has finished, perform sysIntDisable() for each identified 
level.

.SS "PCI Auto-Configuration"
To simplify the addition of PCI-based add-in cards, the BSP provides a PCI
auto-configuration library. The BSP will automatically locate and configure 
installed PCI devices. 

The auto-cofiguration library is called from sysHwInit to discover and 
configure the installed PCI devices and bridges. Device configuration includes 
the following PCI information:

.IP "Base Address Registers (BARs)"
Space in the address map is dynamically allocated to each valid BAR detected.
Allocation pools are maintained for the following PCI address spaces:


16-Bit PCI I/O
.br
32-Bit PCI I/O
.br
PCI Memory I/O (non-prefetchable memory)
.br
PCI Memory (pre-fetchable)

.IP "Interrupt Routing"
The correct interrupt vector number is placed in the intLine register of the
device's PCI header. To connect to the devices's interrupt, simply call
intConnect with the value read from intLine.

.IP "PCI Header Completion"
The PCI auto-configuration library fills in the remainder of the PCI header as
follows:

Cache Line Size  = _CACHE_ALIGN_SIZE/4
.br
Latency Timer    = PCI_LAT_TIMER
.br
Command Register = I/O enabled, Memory enabled and Bus Master enabled.

.IP "PCI-to-PCI Bridge Configuration"
PCI-to-PCI bridges encountered during PCI auto-configuration will be configured
as necessary and devices detected behind the bridge will be configured as
described above. Bridge configuration consists of the following:

Primary Bus Number, Secondary Bus Number and Subordinate Bus Number are
filled in according to the bridge's position in the system.
.sp
I/O Base and Limit registers are configured as required to forward PCI
transactions to PCI devices detected and configured beyond the bridge.
.sp
Memory Base and Memory Limit registers are configured as required to forward
PCI transactions to PCI devices detected and configured beyond the bridge.
.sp
Command Register = I/O enabled, Memory enabled and Bus Master enabled.
.sp
Cache Line Size         = _CACHE_ALIGN_SIZE/4
.br
Primary Latency Timer   = PCI_LAT_TIMER
.br
Secondary Latency Timer = PCI_LAT_TIMER
.sp
NOTE: Due to a discrepancy in the PCI Auto-Configuration code, pre-fetchable
memory requests below a PCI-to-PCI bridge are not currently honored. The address
space requested by a pre-fetachable Base Address Register will be allocated from
the non-prefetchable pool.

.SS "Serial Configuration"
Two serial ports on the MVME51xx board family are implemented with
TL16550C UARTs.  The UARTs reside off the Hawk's X-bus device.  A single RJ-45 
jack is placed on the front panel of the board and is configured as a DTE 
connection.  The second serial port is connected via the J5 header.

By default, the serial ports are configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

If the IPMC761 module is used, four additional ports are available through
rear panel 761 transition.  By default COM2 and COM3 are defined as the 
SuperI/O serial ports and COM4 and COM5 are the Z8536/Z85230 serial ports.

.SS "SCSI Configuration"
SCSI is implemented on the MVME51xx board family via the IPMC761 module.
The SCSI device used is the SYM53C895A.
Only SCSI 2 is supported by the SYM53C895A.

Enabling SCSI for the IMPC761 module (setting J6 and J20 to 1-2) uses 
several pins that are shared with PMC slot 1 J23; these pins are no
longer available to J23.  See the mv5100 User Manual for complete
description.

.SS "Network Configuration"
All boards have two Ethernet ports which are 10baseT and 100baseTX compatible.
The MVME51xx boards have two RJ45 jacks on their front panel for connection
to these facilities.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX.  

The Media Access Control (Ethernet) address for each port is obtained from a
serial ROM contained connected to the i82559 chip.  This address is defined
during board manufacturing and cannot be changed.

.SS "VME Access"
VMEbus accesses can be classified as either master or slave.  A master access
is one in which the accessing processor has bus mastership (it owns the bus)
and is addressing resources on another VME board (the slave board).  The
master addresses the off-board resources through a memory mapping mechanism
which assigns portions of the local address space to the various VME address
spaces.  These local memory regions are windows onto the VMEbus.  Each
window is individually configured with a set of base addresses -- one for the
local bus, the other for the VMEbus -- and a window size.

A slave access is one in which slave VME processors allow access to
their resources from the various VME address spaces through slave windows.

The normal VxWorks default is to enable the slave access windows only on
CPU 0, as part of the routine sysProcNumSet().  Otherwise, slave accesses
are normally not permitted.

The default configuration maps all local memory onto VME A32.  There are no A24
or A16 slave windows.

There is no support for the A64/D64 VME extensions.

To disable any VME master or slave window, just set the appropriate
VME_Axx_xxx_SIZE macro (in config.h) to 0.

NOTE:  Only the macros in config.h are considered user options.
       Macros in mv5100.h should not be changed by the user.

There are two addressing models supported: the default Extended VME A32 
and one for the optional pseudo-PReP address model.  For more information 
on the pseudo-PReP model, see 
.I SPECIAL CONSIDERATIONS.

The following lists the window parameters that the user may change in config.h
for both models:

.CS
    #define VME_A32_MSTR_BUS  0x08000000
    #define VME_A32_MSTR_SIZE 0x08000000  /* (128MB) */
    #define VME_A24_MSTR_BUS  0x00000000
    #define VME_A24_MSTR_SIZE 0x01000000  /* (16MB) */
    #define VME_A16_MSTR_SIZE 0x00010000  /* (64KB) */

    #define VME_A32_SLV_LOCAL LOCAL_MEM_LOCAL_ADRS
    #define VME_A32_SLV_BUS   VME_A32_MSTR_BUS
    #define VME_A32_SLV_SIZE  LOCAL_MEM_SIZE
.CE

The Extended VME A32 Memory Model provides extended mapping to VME A32 space.
The A32 window size can extend to address more than 3.5GB on the VMEbus.

.ne 10
The master window address mappings are as follows:

.TS E
expand;
cf3 s s s
lf3 lf3 lf3 lf3
l l l l .
.ne 6
.sp .5
VME Master
Address Space	VME Base Address	Size	Local Base Address
_
A16	0x0000	64KB	0xFBFF0000
A24	0x000000	16MB	0xFA000000
A32	VME_A32_MSTR_LOCAL	128MB	VME_A32_MSTR_LOCAL
A32 (Mailbox)	0xFB000000	4KB	0xFB000000
.TE

The slave window address mappings are as follows:

.TS E
expand;
cf3 s s s
lf3 lf3 lf3 lf3
l l l l .
.ne 6
.sp .5
VME Slave 
Address Space	VME Base Address	Size	Local Base Address
_
A16 (none)
A24 (none)
A32	0x00000000	128MB	0x00000000
A32 (Mailbox)	0xFB000000	4KB	T{
0x00001000
.br
(PCI I/O Space)
T}
.TE

.SS "VME DMA Support"
DMA support is implemented as a synchronous "VxWorks driver",
that is, the calling task will be blocked until the DMA transfer has
terminated.  However, the driver itself is a polled driver, and it will
not relinquish the CPU waiting for an interrupt; instead, it will enter
a busy loop periodically sampling the DMA transfer status for termination.
A major intended use of this driver is to transfer TCP/IP packets
(packet size approx. 2K).  In light of its' intended use and to keep this
driver as simple as possible, only direct-mode operations will be
implemented, that is, linked-list mode will not be supported.

This driver is strictly non-sharable; however, it contains no guards
to prevent multiple tasks from calling it simultaneously.  It assumes
that the application layer will provide atomic access to this driver
through the use of a semaphore or similar guards.

As a precaution,
it is recommended by the Tundra User's Manual that the calling
task set up a background timer to prevent an infinite wait
caused by a system problem.  Also, tasks transferring large
blocks of data should lower their priority level to allow other
tasks to run, and tasks transferring small blocks of data
should use bcopy() instead of calling this driver.

.SS "PCI Access"
The 32-bit PCI bus is fully supported under the 
.I PCI Local Bus Specification, Revision 2.1. 
The 64-bit extensions are not supported.  All configuration space accesses 
are made with BDF (bus number, device number, function number) format calls 
in the pciConfigLib module.

The PCI address mappings are affected by the VME address model selected.  See 
.I SPECIAL CONSIDERATIONS.

The Extended VME A32 address model produces the following PCI address mapping:

.TS E
expand;
cf3 s s
lf3 lf3 lf3
l l l .
.ne 6
PCI I/O Space Access
.sp .5
Start	Size	Access to
_
0x00000000	16KB	ISA Legacy I/O space
0x00001000	4KB (fixed)	T{
On-board VME mailbox (inside ISA Legacy space)
T}
0x00004000	48KB	16-bit PCI I/O
0x00010000	8MB	32-bit PCI I/O space
.TE

.TS E
expand;
cf3 s s
lf3 lf3 lf3
l l l .
.ne 6
PCI MEM Space Access
.sp .5
Start	Size	Access to
_
0x00000000	LOCAL_MEM_SIZE	DRAM space
	(32MB - 512MB)

VME_A32_MSTR_LOCAL	~3.7GB (max)	VME A32 master space
	128MB (std)


0xFA000000	16MB (max)	VME A24 master space [1]
0xFB000000	64KB (fixed)	VME mailbox (A32) space
0xFBFF0000	64KB (max)	VME A16 master space
0xFC000000	256KB (fixed)	MPIC REGS
0xFD000000	8MB	PCI MEM I/O
0xFD800000	8MB	PCI MEM
.TE

NOTE: [1] A24 and A32 address ranges must not overlap.

.ne 6

.SS "BSP Configuration"
Most BSP configuration values are taken from on-board Vital Product Data
(VPD) and Serial Presence Detect (SPD) serial EEPROMs. If invalid VPD or SPD
information is suspected or reported, defining NONFATAL_VPD_ERRORS,
BYPASS_VPD_PCO and/or BYPASS_SPD in config.h may permit operation using default
parameters. These defines are intended for use during debug only as they
hard-code non-optimized SDRAM timing and other VPD information. Changing the
state of any of these defines requires the rebuilding the Bootrom image and
re-flashing.

.SS "Bootrom Errors"
Errors encountered during the early stages of the bootrom execution are saved
in the Hawk's general purpose registers as bit flags. Once the system is able to
report these errors, they are logged in the following form:

    Bootrom Error: Group = X, Code = 0xXXXXXXXX


The following errors are defined for this BSP:
.TS E
expand;
lf3 lf3 lf3
c l l .
.ne 4
.sp .5
Group	Bit Pattern	Meaning
_
A	0x80000000	Unable to read bus frequency from VPD.
A	0x40000000	Using default SDRAM Timing.
.TE

NOTE: When multiple errors are present simultaneously, the error bits are OR'd
together.

.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1 - shared memory
    \f3fei\f1 - Ethernet (10baseT or 100baseTX)
    \f3scsi\f1 - SCSI

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP boot ROM:
.CS    
    cd target/config/mv5100
    make clean
    make bootrom.bin
    chmod 666 bootrom.bin
    cp bootrom.bin /tftpboot/boot.bin
.CE

Power down the board and switch the ROM jumper to select socketed FLASH.
Connect the Ethernet and console serial port cables, then power the board back
up.

.SS "Flashing the Boot ROM Using Motorola PPC6-Bug:" 1
At the PPC6-Bug prompt, start the system clock then set up the network transfer
from a TFTP host using `niot'.  To start the system clock, the \f3set\f1
command must be used.  The format is: set MMDDYYhhmm  where MM is month, DD is
day of month, YY is year, hh is hour (24-hour format), and mm is minutes.  This
command starts the system clock and sets the current date and time.

.CS
   PPC6-Bug>set 1016021302
.CE

Using `niot', the Client IP Address, Server IP Address, and Gateway IP Address
must be set up for the user's specific environment:

.CS
   PPC6-Bug>niot
   Controller LUN =00?
   Device LUN     =00?
   Node Control Memory Address =<xxxxxxxx>?  *Do not touch* 
   Client IP Address      =123.123.10.100? 123.321.12.123
   Server IP Address      =123.123.18.105? 123.321.21.100
   Subnet IP Address Mask =255.255.255.0?
   Broadcast IP Address   =255.255.255.255?
   Gateway IP Address     =123.123.10.254? 123.321.12.254
   Boot File Name ("NULL" for None)     =? .

   Update Non-Volatile RAM (Y/N)? y
   PPC6-Bug>
.CE

The file is transferred from the TFTP host to the target board using
the `niop' command.  Important: You must have a TFTP server running on your
host's subnet for the `niop' command to succeed.  The file name must be set to
the location of the binary file on the TFTP host.  The binary file must be
stored in the directory identified for TFTP accesses, but the file name is
a relative path and does not include the \f3/tftpboot\f1 directory name:

.CS
   PPC6-Bug>niop
   Controller LUN =00?
   Device LUN     =00?
   Get/Put        =G?
   File Name      =? boot.bin
   Memory Address =00004000?
   Length         =00000000?
   Byte Offset    =00000000?

   PPC6-Bug>
.CE

After the file is loaded onto the target, the `pflash' command is used
to put it into soldered FLASH parts.  

Note that the MVME5100 can have varying sized soldered FLASH parts.  The 
base address of ROM bank A is programmed by PPC6-Bug depending on the size 
found in VPD.  To determine the FLASH base address, dump the Hawk's ROM 
base A address register at 0xfef80050:

.CS
PC6-Bug>md 0xfef80000
FEF80000  10574803 00000000 00010200 00000000  .WH.............
FEF80010  85850000 00000000 00080000 00000000  ................
PPC6-Bug>
FEF80020  63000000 00000000 00000001 00000000  c...............
FEF80030  05290000 00000000 0FF210E0 00000000  .)..............
PPC6-Bug>
FEF80040  00000000 00000000 00000000 00000000  ................
FEF80050  F40C0006 00000000 FF800006 00000000  ................
          ^^
.CE

This is the destination address to be used in the pflash command below.  
Use the high order byte at 0xfef80050 as the high order byte of the 
destination address:

.CS
   PPC6-Bug>pflash 4000:fff00 f4000100
                              ^^
.CE

When the command is finished, power down the board and switch the ROM
jumper to select soldered FLASH.  Then power the board back up.

.SH "SPECIAL CONSIDERATIONS"
This section describes miscellaneous information concerning this BSP and its
use.

.SS "Make Targets"
The make targets are listed as the names of object-format files.  
Append `.hex' to each to derive a hex-format file name.

.nf
`bootrom'
`bootrom_uncmp'
`vxWorks' (with `vxWorks.sym')
`vxWorks_rom'
`vxWorks.st'
`vxWorks.st_rom'
.fi

.SS "Special Routines"
For these boards, the value of the CPU clock speed is read from the VPD 
information using the macro MEMORY_BUS_SPEED which is defined
in mv5100.h.  For example:

.CS
   clkFreqMhz = MEMORY_BUS_SPEED;
.CE

.SS "VME Interrupt Vectors"
Interrupt vectors chosen to generate normal VME interrupts under program
control must be even numbers.  VME interrupt service routines (ISRs) servicing
VME interrupts received by the Universe chip need only be able to service
even vector numbers due to the Universe chip only supporting even vector
numbers.

The Universe chip used on this board can be configured to generate VME bus
interrupts in response to DMA status, PCI bus conditions, and by specific
command from software.  During the VME interrupt acknowledge (IACK) cycle,
the STATUS/ID register of the Universe chip transmits an 8-bit interrupt
vector to the VME bus.  The seven most significant bits are the vector number
(hence the need for even vector numbers) and the least significant bit (LSB) is
set according to how the Universe is configured to respond to the IACK cycle.
If the interrupt was generated by software and the IACK cycle is received, the
Universe can be configured to send an acknowledging interrupt (SW_IACK)
back to the software over the PCI bus.  If the SW_IACK interrupt is enabled,
the LSB is set to 0, otherwise, it is set to 1.

The Universe chip can also be configured to receive VME interrupts.  However,
the Universe is designed to mask out the least significant bit of the vector
number returned by the interrupting device.  Therefore, the ISR servicing
the VME interrupt only receives the seven most significant bits of the
vector number from the Universe chip receiving the interrupt.

Note that, if software specifies an odd number as the interrupt vector to be
transmitted during the IACK cycle, the STATUS/ID register will truncate it to
an even number.  Also, if any interrupting VME device sends an odd vector
number, the vector number returned by the Universe to an ISR is truncated to
an even vector number.  There is no configuration option to compensate for this
feature of the Universe chip.

.SS "TrueFFS"
The standard Wind River TFFS product is supported in this release;
a patch from Motorola is no longer required.

The MV5100 comes with four AMD AM29LV323C 32Mbit devices. They are soldered
on the base board as 16MB of FLASH bank A memory. The default setup puts
the VxWorks bootrom into this bank of FLASH memory. TrueFFS, an optional
product of Tornado and VxWorks, can co-exist with the bootrom in the same
FLASH bank in distinct blocks.

The file sysTffs.c in the BSP configures TrueFFS for this board. The flash
driver amd29LvMtd.c, found in target/src/drv/tffs, contains the MTD for
the AMD AM29LV323 and AM29LV160D flash parts. 

.IP "Building With TrueFFS"
To build the BSP with TrueFFS support from the command line, edit the BSP's
config.h file to change #undef INCLUDE_TFFS to #define INCLUDE_TFFS. The
following macros may also be added to facilitate the example
below (assuming use of network loading):

.CS
    #define INCLUDE_SHELL
    #define INCLUDE_NET_SYM_TBL
    #define INCLUDE_LOADER
.CE

If you use Tornado's project facility, the following should be done:

    Include "TrueFFS Flash File System" in your project which is located
    at Hardware -> Peripherals in the project tree.

    Include "Initialize Symbol Table" in your project which is located
    at Development Tool Components -> Symbol Table Components -> Symbol
    Table Initialization Components in the project tree.

    Include "Shell Banner", "Target Debugging", and "Target Shell" in
    your project which is located at Development Tool Components -> Target
    Shell Components in the project tree.

Rebuild the bootrom and the BSP by following the steps in the ROM section.
Note that the bootrom needs to be re-programmed for the changes to take
effect.

.IP "Using TrueFFS"
After booting with the new bootrom, the bank A FLASH is mapped at address
0xF4000000 with 16MB of memory contained in 71 sectors. 
The macro FLASH_BASE_ADRS has been changed to 0xF4000000 so the 16MB of
soldered FLASH (ROM bank A) will be fully addressable.
Sectors 0 through 3
(1 MB) are reserved for the bootrom, and sectors 4 through 70 can be used
by TrueFFS. tffsRawio() can be used to perform a one time setup, using the
syntax:

.CS
    tffsRawio(drive_no, function_no, first_unit, number_of_units)
.CE

For example, the following commands can be issued on the target shell.
This will erase and format the FLASH for use by TrueFFS, and mount it as
"/flash" device in VxWorks. The tffsRawio() and sysTffsFormat() commands
may take up to ten minutes each to complete. Interrupting the board during
this time risks corrupting the bootrom in FLASH:

.CS
    /* warning: data in flash will be lost! */
    tffsRawio (0, 3, 4, 60);
    sysTffsFormat ();
    usrTffsConfig (0, 0, "/flash");
.CE

You should now be able to use VxWorks I/O and dosFs commands to access the
FLASH device as if it were a disk. Use `devs' and `dosFsShow' to examine the 
device. After rebooting, use only usrTffsConfig() to re-mount the FLASH
device and previous changes to the file system should be preserved. For
more information about the TrueFFS Flash File System, refer to the
"TrueFFS for Tornado Programmer's Guide".

.SS "Known Problems" 

A 64-bit VME read or write that is interrupted by
a VME RMW cycle can cause the HawkII to lock up, effectively halting
the board in a tight loop.  This is a hardware bug in either Hawk or
Universe (still under investigation at time of release): MCG has
HW-simulated the problem.  Boards with HawkIII and UniverseIID parts
do not appear to exhibit this failure, but no conclusion had been
published as of March 2002.

Workaround: Do not #undef ANY_BRD_IN_CHASSIS_NOT_RMW; the VOWN software
TAS will not fail, but does cause some loss of performance.
If using H/W TAS, that is, if #undef ANY_BRD_IN_CHASSIS_NOT_RMW,
then do not use 64-bit VME accesses.

Older generation VME backplanes often do not have slot 1 (the system controller
slot) hard-wired for interrupt acknowledge (IACK) daisy chain operation,
leaving this to be done by a board plugged in to the slot.  New VME backplanes
usually have the left-most slot P1 connector hard-wired so that pin A20 (IACK)
is connected to A21 (IACKIN).  On old VME backplanes, the user must add a
jumper between pins A20 and A21 on the wire wrap pins behind the P1 connector
of slot 1.

Under heavy IPMC761 serial interrupts, an interrupt may be lost.  This
causes the COM port to lock up until the board is reset.  This problem exists
in the FCS release of the Tornado 2.0 FCS i8250Sio driver.  In the FCS
release of Tornado 2.0.2, a newer version of the i8250Sio.c driver has been
tested with the mv5100 BSP and does not lose interrupts.

.SS "Pseudo-PReP Memory Model"
The following table describes the modified PowerPC Reference Platform (PReP)
address maps created for VME from the CPU point of view.  Tornado-compatible
mapping deviates only slightly from the model.

.TS E
expand;
lf3 lf3 lf3
l l lw(2.5i) .
.ne 6
.sp .5
Start	Size	Access to
_
0x0	LOCAL_MEM_SIZE	DRAM
	(32MB - 256MB)

LOCAL_MEM_SIZE	T{
(0x80000000 - LOCAL_MEM_SIZE)
T}	[Not used]

0x80000000	16KB	Legacy ISA I/O space
0x80001000	4KB	T{
On-board VME mailbox acess (inside ISA Legacy I/O space)
T}
0x80004000	48KB	T{
16-bit PCI I/O space
T}
0x80010000	8MB	T{
32-bit PCI I/O space
T}
0x80810000	0x3F7F0000	[Not Used]
0xC0000000	8MB	T{
PCI MEM I/O space
T}
0xC0800000	16MB	T{
32-bit PCI MEM space
T}
0xC1800000	0x16800000	[Not Used]
0xD8000000	128MB (max)	T{
PCI MEM (A32 VME space)
T}
0xE0000000	16MB	T{
PCI MEM (A24 VME space)
T}
0xE1000000	0x0EFF0000	[Not used]
0xEFFF0000	64KB	T{
PCI MEM (A16 VME space)
T}
0xF0000000	64KB	T{
PCI MEM (VME REG. [A32] space)
T}
0xF0010000	0x0BFF0000	[Not used]
0xFC000000	256KB	MPIC Reg space
0xFC040000	0x02F40000	[Not used]
0xFEF80000	128KB	Hawk's regs.
0xFEFA0000	0x00060000	[Not used]
0xF4000000	64MB	T{
ROM space (No PCI/VME)
T}
.TE

.SS "VME Access in the Pseudo-PReP Memory Model" 1

The pseudo-PReP memory model does not offer much address space for mapping
VME master windows.  Only 128MB of A32 space is available.  The 128MB window
can be mapped anywhere in VME A32 space by setting the macro VME_A32_MSTR_BUS
in config.h.  The full A16 and A24 master window address spaces are mapped into
the system.

The master window address mappings are as follows:

.TS E
expand;
cf3 s s s
lf3 lf3 lf3 lf3
l l l l .
.ne 6
.sp .5
VME Master
Address Space	VME Base Address	Size	Local Base Address
_
A16	0x0000	64KB	0xEFFF0000
A24	0x000000	16MB	0xE0000000
A32	0x08000000	128MB	0xD8000000
A32 (Mailbox)	0x40000000	4KB	0xF0000000
.TE

The slave window address mappings are as follows:

.TS E
expand;
cf3 s s s
lf3 lf3 lf3 lf3
l l l l .
.ne 6
.sp .5
VME Slave
Address Space	VME Base Address	Size	Local Base Address
_
A16 (none)
A24 (none)
A32	0x00000000	128MB	0x00000000
A32 (Mailbox)	0x40000000	4KB	0x00001000 (PCI I/O space)
.TE

.SS "PCI Access in the Pseudo-PReP Memory Model" 1

The default pseudo-PReP mapping from the PCI bus point of view is:

.TS E
expand;
cf3 s s
lf3 lf3 lf3
l l l .
.ne 6
PCI I/O Space Access
.sp .5
Start	Size	Access to
_
0x00000000	16KB	ISA Legacy I/O space
0x00001000	4KB (fixed)	T{
VME mailbox slave space (inside ISA Legacy space)
T}
0x00004000	48KB	16-bit PCI I/O
0x00010000	8MB	32-bit PCI I/O space
.TE

.TS E
expand;
cf3 s s
lf3 lf3 lf3
l l l .
.ne 6
PCI MEM Space Access
.sp .5
Start	Size	Access to
_
0x00000000	8MB	PCI MEM I/O
0x00800000	16MB	PCI MEM
0x18000000	16MB (std)	VME A32 master space
0x20000000	16MB (max)	VME A24 master space
0x2FFF0000	64KB (max)	VME A16 master space
0x30000000	64KB (fixed)	VME mailbox (A32) master space
0x3C000000	256KB (fixed)	MPIC REGS
0x80000000	LOCAL_MEM_SIZE	DRAM space (32MB - 256MB)
.TE

.SH "BOARD LAYOUT"
The diagram below shows flash EEPROM locations and jumpers relevant to VxWorks
configuration:

For the MVME51xx boards, the debug and 10baseT/100baseTX ports appear on the
front panel.

.ne 4i

.bS
______________________________               ______________________________
|             P1             |    MVME51xx   |             P2              |
|                            ----------------                              |
|                                     J5(COM2 header)                      |
|                                             J6(L)                        |
|                                                                    J20(L)|
|                                                                          |
|                        +----+  +----+                                    |
|                        |    |  |    |                                    |
|                        |    |  |    |                                    |
|                        +----+  +----+ <== PPC6-Bug Firmware              |
|                         XU1     XU2                                      |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                  J7(U)                   |
|                                                     J16(D)               |
|                                                                          |
|                                                     J15(D)               |
|                                                                          |
|                                                                          |
|                                                   LAN2    LAN1           |
|           PMC 2                   PMC 1         10/100T 10/100T  Debug   |
|__.......................__......................_-----___-----___-----___|
.bE

    Key: 
    X  vertical jumper installed
    :  vertical jumper absent
    -  horizontal jumper installed
    "  horizontal jumper absent
    0  switch off
    1  switch on
    U  three-pin vertical jumper, upper jumper installed
    D  three-pin vertical jumper, lower jumper installed
    L  three-pin horizontal jumper, left jumper installed
    R  three-pin horizontal jumper, right jumper installed

.SH "SEE ALSO"
.tG "Getting Started,"
.pG "Configuration"

.SH "BIBLIOGRAPHY"
.iB "Motorola MVME51xx Series Single Board Computer Programmer's Reference Guide,"
.iB "Motorola Computer Group Online Documentation, http://library.mcg.mot.com/mcg/boards"
.iB "Motorola PowerPC 750 RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "Intel 82559 Design Kit,"
.iB "SGS-Thompson MK48T59/559 CMOS 8K x 8 TIMEKEEPER SRAM Data Sheet,"
.iB "Winbond W83C553 Enhanced System I/O Controller with PCI Arbiter Data Book,"
.iB "Tundra Universe User Manual,"
.iB "Tundra Universe Device Errata,"
.iB "ANSI/VITA 1-1994 VME64 Specification,"
.iB "ANSI/IEEE 1014-1987 Versatile Backplane Bus: VMEbus,"
.iB "IEEE P1386 Draft 2.0 - Common Mezzanine Card Specification (CMC),"
.iB "IEEE P1386.1 Draft 2.0 - PCI Mezzanine Card Specification (PMC),"
.iB "IEEE Standard 1284 Bidirectional Parallel Port Interface Specification,"
.iB "Peripheral Component Interconnect (PCI) Local Bus Specification, Rev 2.1,"
.iB "PCI to PCI Bridge Architecture Specification 2.0,"
.iB "ANSI X3.131.1990 Small Computer System Interface-2 (SCSI-2) Draft Document"

