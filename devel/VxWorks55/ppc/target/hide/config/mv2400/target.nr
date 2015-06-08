'\" t
.so wrs.an
.\" PowerPlusII/target.nr - Motorola PowerPlusII target-specific documentation
.\"
.\" Copyright 1984-2002 Wind River Systems, Inc.
.\" Copyright 1996,1997,1998,1999 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01a,16apr02,dat  Update for T2.2, Removed ref to booting via PPCBug (30625)
.\" 01n,21mar02,kab  remove ref to elfToBin (obs in T2.2); 
.\"		     add supported target list.
.\" 01m,27aug01,dgp  change manual pages to reference entries per SPR 23698
.\" 01l,22nov99,srr  Extended VME memory map uses VME_A32_MSTR_LOCAL now.
.\" 01k,01jul99,rhv  Incorporating WRS code review changes.
.\" 01j,19may99,rhv  Added 256MB option and additional updates.
.\" 01i,12may99,rhv  Updated address maps.
.\" 01h,03may99,rhv  Added warning about pre-fetchable memory below bridges.
.\" 01g,14apr99,rhv  Added writeup about PCI Auto-Configuration.
.\" 01f,05feb99,mdp  Cleaned up 2300 refs, SENS info, and DRAM info.
.\" 01f,05feb99,mdp  Added writeup on SENS support.
.\" 01e,28jan99,rhv  Adding a warning to re-flash after changing the state of
.\"                  TOLERATE_CONFIG_ERRORS.
.\" 01d,28jan99,dmw  Added more detailed memory map descriptions.
.\" 01c,27jan99,rhv  Added Bootrom error descriptions.
.\" 01b,14jan99,rhv  Added more 2400 info, removed references to 2300 and
.\"                  updated the copyright.
.\" 01a,15dec98,mdp  Written (from version 01u of mv2304/target.nr).
.\"
.\"
.TH "mv24xx" T "Motorola PowerPlusII" "Rev: 19 May 99" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola MVME24xx"

.SH "INTRODUCTION"
This reference entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.

This BSP encompasses only the MVME2400 single-board computer. All MVME2400
boards require no transition module.  The series part numbers are of the form:

    MVME24xy-z

    where
        x = CPU Speed
           0 = 233Mhz 750
           3 = 350MHz 750

        y = ECC SDRAM size
           1 =  32 MB
           2 =  64 MB
           3 = 128 MB
           4 = 256 MB

        z = Front panel
           1 = Scanbe
           3 = IEEE 1101

For example, an MVME2431-3 denotes a 350 MHz PowerPC 750-based board having
32MB of ECC SDRAM with an IEEE 1101 front panel. Note that not all product
combinations are available. Standard equipment includes 9MB FLASH.

/*Sergey*/
The BAT registers are not supported in the current cache management strategy.

.SS "Boot ROMS"
The MVME2400 boards have two sets of flash EEPROM (FLASH).  One set of two
AMD Am29F040 FLASH is socketed (sockets XU1 and XU2) and contains Motorola's
PPC4-Bug.  The other set of E28f400 FLASH is soldered in on the back of the
boards.  The VxWorks boot kernel resides in the soldered FLASH. See
\f2Hardware Details: ROM Considerations\f1 for information about loading and
writing the boot kernel image to the soldered FLASH.

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
J9	System controller	T{
Install the jumper across pins 2 and 3, if you wish to operate in
"automatic" system controller mode [factory configuration].  Install the
jumper across pins 1 and 2, if the board is not to be the system controller
under any circumstances.  And remove the jumper if the board is to be the
system controller in all cases.
T}
J8	ROM controller	T{
Install the jumper across pins 2 and 3 to select the socketed FLASH.
Install the jumper across pins 1 and 2 to select the soldered FLASH
[factory configuration].
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
The following features of the MVME24xx board family are supported:

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
T}
FLASH	T{
1MB socketed (16-bit wide)
8MB soldered (32-bit wide)
T}
DRAM	T{
32, 64, 128MB SDRAM, two-way interleaved; auto-sized or fixed
T}
NVRAM	T{
8KB (MK48T59/559)
T}
Peripherals	T{
one async serial debug port
10baseT/100baseTX Ethernet interface
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
RESET switch
8-Bit Software Readable Header
T}
.TE

.SS "Unsupported Features"
The following features of the MVME24xx board family are not supported:

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
MK48T59/559; only NVRAM portion is used
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
    `dec21x4x[End].obj' - 10baseT/100baseTX DEC 21x4x Ethernet driver
    `byteNvRam' - byte-oriented generic non-volatile RAM driver
    `sl82565IntrCtl' - PIB interrupt controller driver
    `hawkAuxClk' - Motorola Hawk auxiliary clock library.
    `hawkI2c' - Motorola Hawk I2C serial EEPROM driver.
    `hawkMpic' - Motorola Hawk MPIC interrupt controller driver
    `pciConfigLib' - PCI configuration library
    `pciAutoConfigLib' - PCI auto-configuration library
    `universe' - Tundra Universe chip VME-to-PCI interface driver
    `sysVpd' - Board-Specific Vital Product Data library
    `vpdUtil' - "Generic" Vital Product Data library
.fi

The `sl82565IntrCtl' module implements the Winbond W83C353 PCI-to-ISA Bridge
(PIB) driver.  The module was developed originally for the Symphonic
Laboratories SL82565 PIB which has been succeeded by the Winbond device.

.SS "SENS support"
This BSP supports SENS (Scalable Enhanced Network Stack).  To build
a SENS kernel and bootrom make sure you have installed the following
in the order presented:

.IP "1)"
Wind River's Tornado 1.0.1.
.IP "2)"
The Motorola MVME2400 BSP.
.IP "3)"
Wind River's SENS support.
.LP
The unmodified BSP builds without SENS support. To configure
the BSP to contain SENS support, make the following adjustments
in the "mv2400" subdirectory and rebuild:

.IP "1)"
In "Makefile" comment out the MACH_EXTRA line which contains "dec21x4x.obj"
and uncomment the MACH_EXTRA line which contains "dec21x4xEnd.obj".

.IP "2)"
In "config.h" change "#undef INCLUDE_END" to "#define INCLUDE_END"

.IP "3)"
From the "mv2400" subdirectory perform:
.CS
  cp ./sens/* .
.CE

.IP "4)"
Rebuild the bootroms and kernel.

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
disucssed under 
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
0xFF000000	16MB	T{
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

When changing modes -- for example, from standard VxWorks (pseudo-PREP-compliant
mapping) to Extended VME mapping -- all MVME24xx boards should be configured
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
mailbox registers so that each CPU can send and receive shared memory interrupts
using single-byte mailboxes.
The windows map a 4KB region in A32 space at address 0xFB000000 + (0x1000 *
CPU #) into the Universe chip registers.  This configuration allows one
processor to generate a SIG1 interrupt in another processor by accessing the
other processor's mailbox register and setting the SIG1 bit.  Each CPU has a
master window covering the A32 addresses 0xFB000000 through 0xFB00ffff
representing CPU numbers 0 through 15.  Each CPU's slave window maps the
appropriate address for that CPU to the Universe chip's register set.

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

VMEbus ownership is necessary for a number of reasons.  First, there is no
hardware support to propagate PowerPC atomic TAS instructions to the
Universe chip.  Second, the Universe chip (rev 1.0) has a bug which prevents
proper generation of read-modify-write (RMW) cycles on the VMEbus.  Third,
these boards have no support for propagating true atomic VME RMW cycles to
local processor memory.

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
Motorola MVME162 as the master board and an MVME2400 as a slave.  The mv162
BSP assumes that support exists for atomic TAS/RMW cycles on to and off of
all boards in the system.  Furthermore, the local 68040 CPU can access and 
alter its memory \f2between\f1 VMEbus cycles.  Therefore, this system
configuration does not work because there is no way to ensure atomic access
to a semaphore by the MVME2400 board.

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
   04	Debug serial port
.TE

Vector numbers not in the table are not used by this BSP.

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
   10	ISA PICs
   12	PCI Ethernet
   15	PCI Universe VME INT 0
   16	PCI Universe VME INT 1
   17	PCI Universe VME INT 2
   18	PCI Universe VME INT 3
   19	PCI PMC1/PCIX INTA, PMC2 INTB
   1a	PCI PMC1/PCIX INTB, PMC2 INTC
   1b	PCI PMC1/PCIX INTC, PMC2 INTD
   1c	PCI PMC1/PCIX INTD, PMC2 INTA
   1d	LM/SIG (mailbox) 0
   1e	LM/SIG (mailbox) 1
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
IRQ0	8	Winbond PIB [all ISA interrupts]
IRQ1	N/A	[Not Used]
IRQ2	14	Ethernet
IRQ3	N/A	[Not Used]
IRQ4	N/A	[Not Used]
IRQ5	10	Universe LINT0 [all Universe/VME interrupts]
IRQ6	0	Universe LINT1
IRQ7	0	Universe LINT2
IRQ8	0	Universe LINT3
IRQ9	7	PCI PMC1/PCIX INTA, PMC2 INTB
IRQ10	6	PCI PMC1/PICX INTB, PMC2 INTC
IRQ11	5	PCI PMC1/PICX INTC, PMC2 INTD
IRQ12	4	PCI PMC1/PICX INTD, PMC2 INTA
IRQ13	0	LM/SIG Interrupt 0
IRQ14	15	LM/SIG Interrupt 1 (mailbox)
IRQ15	N/A	[Not used]
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
auto-configuration library. When INCLUDE_AUTOCONF is defined, the BSP will
automatically locate and configure installed PCI devices. When
INCLUDE_AUTOCONF is not defined, add-in PCI devices will not be located or
configured.

If PCI auto-configuration is selected, the auto-cofiguration library will be
called from sysHwInit to discover and configure the installed PCI devices and
bridges. Device configuration includes the following PCI information:

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
NOTE: Due to a descrepancy in the PCI Auto-Configuration code, pre-fetchable
memory requests below a PCI-to-PCI bridge are not currently honored. The address
space requested by a fre-fetachable Base Address Register will be allocated from
the non-prefetchable pool.

.SS "Serial Configuration"
The single debug port on the MVME24xx board family is implemented in a
TLC16550 UART.  It is an ISA bus device.  The RJ-45 jack is placed on the front
panel of the board and is configured as a DTE connection.

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"
SCSI is not available on the MVME24xx board family.

.SS "Network Configuration"
All boards have one Ethernet port which is 10baseT and 100baseTX compatible.
The MVME24xx boards have an RJ45 jack on their front panel for connection
to this facility.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX.  The Ethernet driver is compatible with both DEC2104x and DEC2114x
devices (for the 24xx family, a DEC21143 chip is used).

The Media Access Control (Ethernet) address for each port is obtained from a
serial ROM contained in the DEC21143 chip.  If the address is not found in
serial ROM, the driver attempts to read it from NVRAM.

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
VME_Axx_xxx_SIZE macro (in config.h) to 0.  Only the macros in config.h are
considered user options.  Macros in mv2400.h should not be changed by
the user.

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
are made with BDF (bus number, device number, function number) format calls in
the pciConfigLib module.  For more information, refer to the reference entries 
.I mv24xx_pciXxx.

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

NOTE: When multiple errors are present simultaneuously, the error bits are OR'd
together.

.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1 - shared memory
    \f3dc\f1 - Ethernet (10baseT or 100baseTX)

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP boot ROM:
.CS    
    cd target/config/mv2400
    make clean
    make bootrom_uncmp.bin
    cp bootrom_uncmp.bin /tftpboot/boot.bin
.CE

Power down the board and switch the ROM jumper to select socketed FLASH.
Connect the Ethernet and console serial port cables, then power the board back
up.

.SS "Flashing the Boot ROM Using Motorola PPC4-Bug:" 1
At the PPC4-Bug prompt, start the system clock then set up the network transfer
from a TFTP host using `niot'.  To start the system clock, the \f3set\f1
command must be used.  The format is: set MMDDYYhhmm  where MM is month, DD is
day of month, YY is year, hh is hour (24-hour format), and mm is minutes.  This
command starts the system clock and sets the current date and time.

.CS
   PPC4-Bug>set 1016971302
.CE

Using `niot', the Client IP Address, Server IP Address, and Gateway IP Address
must be set up for the user's specific environment:

.CS
   PPC4-Bug>niot
   Controller LUN =00?
   Device LUN     =00?
   Node Control Memory Address =00FA0000?
   Client IP Address      =123.123.10.100? 123.321.12.123
   Server IP Address      =123.123.18.105? 123.321.21.100
   Subnet IP Address Mask =255.255.255.0?
   Broadcast IP Address   =255.255.255.255?
   Gateway IP Address     =123.123.10.254? 123.321.12.254
   Boot File Name ("NULL" for None)     =? .

   Update Non-Volatile RAM (Y/N)? y
   PPC4-Bug>
.CE

The file is transferred from the TFTP host to the target board using
the `niop' command.  Important: You must have a TFTP server running on your
host's subnet for the `niop' command to succeed.  The file name must be set to
the location of the binary file on the TFTP host.  The binary file must be
stored in the directory identified for TFTP accesses, but the file name is
a relative path and does not include the \f3/tftpboot\f1 directory name:

.CS
   PPC4-Bug>niop
   Controller LUN =00?
   Device LUN     =00?
   Get/Put        =G?
   File Name      =? boot.bin
   Memory Address =00004000?
   Length         =00000000?
   Byte Offset    =00000000?

   PPC4-Bug>
.CE

After the file is loaded onto the target, the `pflash' command is used
to put it into soldered FLASH parts.

.CS
   PPC4-Bug>pflash 4000:fff00 ff000100
.CE

When the command is finished, power down the board and switch the ROM
jumper to select soldered FLASH.  Then power the board back up.

.SH "SPECIAL CONSIDERATIONS"
This section describes miscellaneous information concerning this BSP and its
use.

.SS "Delivered Objects"
The delivered objects are: `bootrom.hex', `vxWorks', `vxWorks.sym', and
`vxWorks.st'.

.SS "Make Targets"
The make targets are listed as the names of object-format files.  Append `.hex'
to each to derive a hex-format file name.

.nf
`bootrom'
`bootrom_uncmp'
`bootrom_res_high'  (`bootrom_res' does not build)
`vxWorks' (with `vxWorks.sym')
`vxWorks_rom'
`vxWorks.st'
`vxWorks.st_rom'
`vxWorks.res_rom_res_low' (`vxWorks.res_rom' does not build)
`vxWorks.res_rom_nosym_res_low' (`vxWorks.res_rom_nosym' does not build)
.fi

.SS "Special Routines"
For these boards, the value of the CPU clock speed is read from the VPD 
information using the macro MEMORY_BUS_SPEED which is defined
in mv2400.h.  For example:

.CS
   clkFreqMhz = MEMORY_BUS_SPEED;
.CE

.SS "VME Interrupt Vectors"
Interrupt vectors chosen to generate normal VME interrupts under program
control must be even numbers.  VME interrupt service routines (ISRs) servicing
VME interrupts received by the Universe chip need only be able to service
even vector numbers.

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

.SS "Known Problems"
Older generation VME backplanes often do not have slot 1 (the system controller
slot) hard-wired for interrupt acknowledge (IACK) daisy chain operation,
leaving this to be done by a board plugged in to the slot.  New VME backplanes
usually have the left-most slot P1 connector hard-wired so that pin A20 (IACK)
is connected to A21 (IACKIN).  On old VME backplanes, the user must add a
jumper between pins A20 and A21 on the wire wrap pins behind the P1 connector
of slot 1.

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
0xFF000000	16MB	T{
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

For the MVME24xx boards, the debug and 10baseT/100baseTX ports appear on the
front panel.

.ne 4i

.bS
______________________________               ______________________________
|             P1             |    MVME24xx   |             P2              |
|                            ----------------                              |
|                             Does not use a                               |
|                            Transition Module                             |
|                                                                          |
|                                                                          |
|                        +----+  +----+                                    |
|                        |    |  |    |                                    |
|                        |    |  |    |                                    |
|                        +----+  +----+ <== PPC4-Bug Firmware              |
|                         XU2     XU1                                      |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                        8-Position  +--+  |
|                                                         Software   |  |  |
|                                                         Readable   |  |  |
|                                                          Header    +--+  |
|                                                                          |
|                                                                          |
|                                                     J7  (SCON)   --> L   |
|                                                     J8  (ROM ctrl) --> D |
|                                                                          |
|                                                                          |
|     Debug   10/100BaseT      PMC 2                   PMC 1               |
|_____-----____-----___.......................__......................_____|
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
.iB "Motorola MVME24xx Series Single Board Computer Programmer's Reference Guide,"
.iB "Motorola PowerPC 750 RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "DECchip 21143 PCI Fast Ethernet LAN Controller Hardware Reference Manual,"
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

