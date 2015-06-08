'\" t
.so wrs.an
.\" NexGen/target.nr - Motorola NexGen target-specific documentation
.\"
.\" Copyright 2002 Wind River Systems, Inc.
.\" Copyright 1996-1998 Motorola, Inc.
.\"
.\" modification history
.\" --------------------
.\" 01x,30apr02,sbs  Update for Tornado 2.2
.\" 01w,21mar02,kab  remove ref to elfToBin (obs in T2.2); 
.\"		     add supported target list.
.\" 01v,18sep01,dgp  change manual entry to reference entry per SPR 23698
.\" 01u,07aug98,tb   added support for VMEbus DMA
.\" 01t,12jun98,mas  added support for later rev boards; corrected sections on
.\"		     board layout, jumpers, interrupts, and the bibliography
.\"		     (SPR 21403).
.\" 01s,08apr98,dat  chg'd pciIomapLib to pciConfigLib, chg'd dec21140.obj to
.\"                  if_dc.o
.\" 01r,05nov97,mas  added Universe Device Errata to bibliography (SPR 9717).
.\" 01q,31oct97,mas  added more info on Universe VME interrupts (SPR 9438).
.\" 01p,16oct97,mas  added info on using PPC1-Bug 'set' command (SPR 9482).
.\" 01o,10oct97,mas  added known problem with VME int handling (SPR 9438).
.\" 01n,19sep97,mas  added comments on VME interrupt vector numbers (SPR 9310).
.\" 01m,27aug97,dgp  doc: final editing
.\" 01l,13aug97,mas  split into seperate files for each board family.
.\" 01k,24jul97,mas  added VME interrupt info and guidelines; added MPIC
.\"		     priority scheme (SPR 8956).
.\" 01j,17jul97,mas  added dynamic memory sizing info (SPR 8824).
.\" 01i,09jul97,mas  added serial ports 3 & 4 (SPR 8566) and PPC1-Bug flash ROM 
.\"                  info.  Rewritten to meet new guidelines.
.\" 01h,30apr97,mas  added extended VME, mv360x and mv230x info (SPR 8410).
.\" 01g,02apr97,dat  added VME config info, SPR 8271, fixed model nbr table
.\" 01f,05mar97,mas  changed GET_CPU_SPEED to MEMORY_BUS_SPEED; deleted ref to
.\"		     CPU_SPEED_MHZ; added tftp server required to burn flash
.\"		     (SPR 8114).
.\" 01e,18feb97,mas  clarified use of transition modules and board diagram
.\"                  (SPR 7772, 7811, 7832).
.\" 01d,10jan97,dat  cleaned up flash loading documentation
.\"             mas
.\" 01c,02jan97,wlf  doc: cleanup.
.\" 01b,01jan97,dat  added mod history
.\" 01a,01sep96,mot  written (Motorola Comp. Grp)
.\"
.\"
.TH "mv230x" T "Motorola NexGen" "Rev: 30 Apr 97" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola MVME2301-2308"

.SH "INTRODUCTION"
This reference entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.

The Motorola NexGen series of boards consists of three families: MVME230x,
MVME260x, and MVME360x.  This BSP encompasses only the MVME230x family.

The MVME2300 board family consists of single-board computers based on the
PowerPC 603 and 604 microprocessors.  The MVME2300 board family has the same
basic architecture as the MVME2600 board family, however, there are no
SCSI, keyboard, graphics, or parallel ports, no auxiliary clock, and no L2
cache.  All MVME2300 boards come with a 200MHz processor and require no
transition module.  The series part numbers are of the form:

    MVME230x

    where
        x = ECC DRAM size
           1 = 200MHz MPC603e,  16MB
           2 = 200MHz MPC603e,  32MB
           3 = 200MHz MPC603e,  64MB
           4 = 200MHz MPC603e, 128MB
           5 = 200MHz MPC604ev,  16MB
           6 = 200MHz MPC604ev,  32MB
           7 = 200MHz MPC604ev,  64MB
           8 = 200MHz MPC604ev, 128MB

For example, an MVME2302 denotes a PowerPC 603e-based board running at
200MHz, having 32MB of ECC DRAM, and requiring no transition module.
Standard equipment includes 5MB FLASH.

The BAT registers are not supported in the current cache management strategy.

.SS "Boot ROMS"
The MVME2300 boards have two sets of flash EEPROM (FLASH).  One set of two
AMD Am29F040 FLASH is socketed (sockets XU1 and XU2) and contains Motorola's
Open Firmware or, on later revisions, Motorola's PPC1-Bug.  The other set of E28f400
FLASH is soldered in on the back of the boards.  The VxWorks boot kernel resides
in the soldered FLASH.
See \f2Hardware Details: ROM Considerations\f1 for information about loading and
writing the boot kernel image to the soldered FLASH.

These boards have non-volatile RAM; thus, boot parameters are preserved
whenever the system is powered off.

To load VxWorks, and for more information, follow the instructions in the
\f2Tornado User's Guide: Getting Started.\f1

.SS "Jumpers"
The following jumpers are relevant to VxWorks configuration.

Board revision A:
.TS E
expand;
lf3 lf3 lf3
l l lw(2.6i) .
.ne 28
.sp .5
Jumper	Function	Description
_
J7	System controller	T{
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

Board revisions B and later:
.TS E
expand;
lf3 lf3 lf3
l l lw(2.6i) .
.ne 28
.sp .5
Jumper	Function	Description
_
J16	System controller	T{
Install the jumper across pins 2 and 3, if you wish to operate in
"automatic" system controller mode [factory configuration].  Install the
jumper across pins 1 and 2, if the board is not to be the system controller
under any circumstances.  And remove the jumper if the board is to be the
system controller in all cases.
T}
J15	ROM controller	T{
Install the jumper across pins 2 and 3 to select the socketed FLASH.
Install the jumper across pins 1 and 2 to select the soldered FLASH
[factory configuration].
T}
.TE

For details of board revision and jumper configuration, see the board diagrams
at the end of this document and in the manual \f2Motorola MVME2300-Series VME
Processor Module Installation and Use.\f1

Note that ROM controller jumpers should be set to select socketed FLASH until
VxWorks boot code is written to soldered FLASH, after which the jumpers should
be restored to the factory configuration of soldered FLASH.

.SH "FEATURES"
The following subsections list all supported and unsupported features, as well
as any feature interaction.

.SS "Supported Features"
The following features of the MVME2300 board family are supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 7
.sp .5
Feature	Description
_
Processors	T{
MPC603, MPC604; 33 and 66MHz bus clock
T}
FLASH	T{
1MB socketed (16-bit wide)
T}
DRAM	T{
16, 32, 64, 128MB, two-way interleaved; auto-sized or fixed
T}
NVRAM	T{
8KB (MK48T59/559)
T}
Peripherals	T{
one async serial debug port 10baseT/100baseTX Ethernet interface
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
Programmable DMA with direct mode support
T}
Miscellaneous	T{
RESET switch
T}
.TE

.SS "Unsupported Features"
The following features of the MVME230x board family are not supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Feature	Description
_
DRAM	T{
ECC protection
T}
RTC	T{
MK48T59/559; only NVRAM portion is used
T}
ISA Interface	T{
ISA RTC and DMA controllers
T}
PCI Interface	T{
64-bit data
T}
VME Interface	T{
D64(MBLT); programmable DMA controller with linked list support
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
    `ppcDecTimer' - PowerPC decrementer timer driver (system and timestamp clock)
    `if_dc' - 10baseT/100baseTX DEC 21140 Ethernet driver
    `byteNvRam' - byte-oriented generic non-volatile RAM driver
    `sl82565IntrCtl' - PIB interrupt controller driver
    `ravenMpic' - Motorola Raven MPIC interrupt controller driver
    `pciConfigLib' - PCI configuration library
    `universe' - Tundra Universe chip VME-to-PCI interface driver
.fi

The `sl82565IntrCtl' module implements the Winbond W83C353 PCI-to-ISA Bridge
(PIB) driver.  The module was developed originally for the Symphonic
Laboratories SL82565 PIB which has been succeeded by the Winbond device.

.SS "Memory Maps"
On-board RAM for these boards always appears at address 0x0 locally.
Its slave address on the VMEbus is set by registers in the Universe ASIC.
Local RAM-to-VMEbus mapping is defined in config.h

Dynamic memory sizing is supported.  By default, LOCAL_MEM_AUTOSIZE is
defined so memory is auto-sized at hardware initialization time.
If auto-sizing is not selected, LOCAL_MEM_SIZE must be set to the actual size
of DRAM memory available on the board to ensure all memory is available
and VME addressing occurs properly.  The default fixed RAM size is set to 16MB
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
l l lw(1.8i) .
.ne 6
.sp .5
Start	Size	Access to
_
0x0	LOCAL_MEM_SIZE (16MB min)	DRAM
LOCAL_MEM_SIZE	(0x10000000 - LOCAL_MEM_SIZE)	[not used]
0x10000000	0xEA000000	T{
PCI MEM (max. A32 VME space)
T}
0x10000000	128MB	T{
PCI MEM (default A32 VME space)
T}
0xFA000000	16MB	T{
PCI MEM (A24 VME space)
T}
0xFB000000	64KB	T{
PCI MEM (VME REG. (A32) space)
T}
0xFB010000	0x00FE0000	[not used]
0xFBFF0000	64KB	T{
PCI MEM (A16 VME space)
T}
0xFC000000	256KB	T{
MPIC Reg space
T}
0xFC040000	0x00FC0000	[not used]
0xFD000000	16MB	PCI MEM space
0xFE000000	8MB	PCI I/O space
0xFE800000	0x00780000	[not used]
0xFEF80000	128KB	T{
Falcon/Raven regs.
T}
0xFF000000	16MB	T{
ROM space (No PCI/VME)
T}
.TE

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
mapping) to Extended VME mapping -- all MVME2300 boards should be configured
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
test-and-set (TAS) and clear operations on 32-bit semaphores.
The TAS and clear operations must be atomic operations.
If the \f3#define\f1 SM_TAS_TYPE is set to SM_TAS_SOFT, only a software TAS
routine is used.  Software TAS is usually good enough for shared memory
networking; however, 
if one board uses software TAS, then \f2all\f1 boards on a shared
memory backplane must use it.
VxMP requires the use of hardware TAS.  Enable hardware
TAS by setting SM_TAS_TYPE to SM_TAS_HARD.  Hardware TAS and clear operations
are performed by the sysBusTas() and sysBusTasClear() routines, respectively.

True atomic operations are those which cannot be preempted at the hardware
level and appear on a bus as a single-cycle instruction.  Pseudo-atomic
operations are composed of multiple instruction cycles executed on a
bus that is locked (owned) by the processor executing the instructions.
VMEbus ownership is necessary for two reasons.
First, the Universe I chip has a bug which prevents
proper generation of RMW cycles on the VMEbus.  And second,
boards with the Universe I and early boards with a Universe II have no support
for propagating true atomic VME RMW cycles to local processor memory.
A third kind of atomic operation is the PowerPC atomic access (lwarx/stwcx
sequence) where atomicity is not guaranteed, but the interruption of atomicity
is detected.  Only newer revision boards (rev D or later) can propagate 
a true atomic VME RMW cycle to local processor memory.  

To determine the revision of your board, see "Board Revision Determination"
below.

The type of atomic operation used by a board is defined in tables found in
the headers of sysBusTas() and sysBusTasClear().  Use these tables to assist
in defining the state of the macro ANY_BRDS_IN_CHASSIS_NOT_RMW.

When the sysBusTas() routine performs a true-atomic TAS
operation, it disables interrupts (to prevent deadlocks) 
and performs a VME RMW cycle.
To perform a pseudo-atomic TAS operation,
it disables interrupts (to prevent deadlocks) and 
locks ownership of the VMEbus.
This routine waits up to 10 microseconds to lock the bus.  
If bus ownership has not
been achieved at the end of this period, the routine returns FALSE, the same as
it would if the semaphore had already been set.
To perform a PowerPC atomic access, it disables interrupts (to prevent
deadlocks) and issues the lwarx/stwcx sequence.

When the sysBusTasClear() routine performs a true-atomic clear operation, it
disables interrupts (to prevent deadlocks) and performs a VME RMW cycle.
To perform a pseudo-atomic clear operation, it
also disables interrupts and locks the VMEbus while
accessing the semaphore.
It waits up to 10 microseconds to gain bus ownership.  But, even if
the bus is not owned after this period, the routine attempts to clear the
semaphore.
The sysBusTasClear() routine does not perform a PowerPC atomic access.
Instead, it can perform a simple clear operation when all boards in the 
system are capable of performing a VME RMW cycle.

Special consideration must be given to boards not using this BSP in the
overall system design.  A board that utilizes RMW as its TAS operation
can be used as a master if and only if
all other boards (slaves) in the system utilize a RMW capability.  There
are no restrictions when boards not using this BSP are used as a slave.

.SS "Board Revision Determination"
Since components are now present on both sides of most boards, we
define the top of the board as the side on which the VME connecters
P1 and P2 are mounted.

On the bottom of the board, between the P1 and P2 VME connectors, there
is a yellow silk screened box and the letters "S/N". 
This is the serial number of the board, and in that area there should 
be a sticker with a bar code and a serial number.

Immediately below the serial number region will be a number
in yellow silk-screen. Immediately following this number should be
a sticker, with a few digits followed by a letter (e.g. 06C). This
final letter is the board revision level.

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
20 - 23	Raven timers
24 - 27	Raven interprocessor dispatch
   28  	Raven detected internal errors
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
their interrupts through the Raven MPIC as an external interrupt.  The external
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
   11	Falcon-ECC error
   12	PCI Ethernet
   15	PCI Universe VME INT 0
   16	PCI Universe VME INT 1
   17	PCI Universe VME INT 2
   18	PCI Universe VME INT 3
   19	PCI PMC1/PMC2 INTA
   1a	PCI PMC1/PMC2 INTB
   1b	PCI PMC1/PMC2 INTC
   1c	PCI PMC1/PMC2 INTD
   1d	LM/SIG (mailbox) 0
   1e	LM/SIG (mailbox) 1
.TE

Vector numbers not in the table are not used by this BSP.

The Raven Multi-Processor Interrupt Controller (MPIC) sets system interrupt
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
Raven MPIC IRQ	Priority	IRQ Source
_
IRQ0	8	Winbond PIB [all ISA interrupts]
IRQ1	0	Falcon ECC Error
IRQ2	14	Ethernet
IRQ3	3	SCSI [not available]
IRQ4	0	Graphics [not available]
IRQ5	10	Universe LINT0 [all Universe/VME interrupts]
IRQ6	0	Universe LINT1
IRQ7	0	Universe LINT2
IRQ8	0	Universe LINT3
IRQ9	7	PCI PMC1 INTA / PMC2 INTB  /* PMC2 INTD rev A boards */
IRQ10	6	PCI PMC1 INTB / PMC2 INTC  /* PMC2 INTA rev A boards */
IRQ11	5	PCI PMC1 INTC / PMC2 INTD  /* PMC2 INTB rev A boards */
IRQ12	4	PCI PMC1 INTD / PMC2 INTA  /* PMC2 INTC rev A boards */
IRQ13	0	LM/SIG Interrupt 0
IRQ14	15	LM/SIG Interrupt 1 (mailbox)
IRQ15	N/A	[Not used]
.TE

For further details, refer to the appropriate board's reference guide.  For
information on board revisions, refer to the BOARD LAYOUT section at the end
of this document.

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
Perform intEnable() for each identified system interrupt level.
.IP "5)"
When the application has finished, perform intDisable() for each identified 
level.

.SS "Serial Configuration"
The single debug port on the MVME2300 board family is implemented in a Zilog
Z8250 ESCC.  It is an ISA bus device.  The RJ-45 jack is placed on the front
panel of the board and is configured as a DCE connection.

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"
SCSI is not available on the MVME2300 board family.

.SS "Network Configuration"
All boards have one Ethernet port which is 10baseT and 100baseTX compatible.
The MVME2300 boards have an RJ45 jack on their front panel for connection
to this facility.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX.  The Ethernet driver is compatible with both DEC21040 and DEC21140
devices.

The Media Access Control (Ethernet) address for each port is obtained from a
serial ROM contained in the DEC21140 chip.  If the address is not found in
serial ROM, the driver attempts to read it from NVRAM at offset 0x202c.

.SS "VME Access"
A VME access is one in which one VME board obtains VMEbus mastership and 
accesses resources on another VME board.  The VMEbus master board initiates 
the access through a "master window" which resides on the VMEbus master 
board and which translates addresses which originate on the VMEbus master 
board's local bus and go out to the VMEbus.  The VMEbus slave board responds 
to the access through a "slave window" which resides on the VMEbus slave board 
and translates addresses which come in from the VMEbus and go to the 
VMEbus slave's local bus.  A VMEbus slave or master "window" is defined by an 
association between a base address on the VMEbus, the associated base 
address on the local bus, and a window size.  Different windows may be 
defined to facilitate accessing the various VMEbus spaces (A32, A24 etc.).  

The normal VxWorks default is to define shared resources on CPU 0 (master
node) and enable access to these resources via a slave window on the master
node.  In addition, each and every node 
in the system will configure a VME slave
window to allow access to an associated mailbox register.

The default configuration maps all local memory onto VME A32.  There are 
no A24 or A16 slave windows.

There is no support for the A64/D64 VME extensions.

To disable any VME master or slave window, just set the appropriate
VME_Axx_xxx_SIZE macro (in config.h) to 0.  Only the macros in config.h are
considered user options.  Macros in mv2600.h should not be changed by
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
lf3 lf3 lf3 lf3
lf3 lf3 lf3 lf3
l l l l .
.ne 6
.sp .5
VME Master
Address Space	VME Base Address	Size	Local Base Address
_
A16	0x0000	64KB	0xFBFF0000
A24	0x000000	16MB	0xFA000000
A32	0x10000000	128MB	0x10000000
A32 (Mailbox)	0xFB000000	4KB	0xFB000000
.TE

The slave window address mappings are as follows:

.TS E
expand;
lf3 lf3 lf3 lf3
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
A32 (Mailbox)	0xFB000000	4KB	0x00001000 (PCI bus)
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
are made with BDF (bus number, device number, function number) format calls 
in the pciConfigLib module.  For more information, refer to the reference entry
.I mv230x_pciXxx.

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
0x00000000	8MB	PCI I/O space
0x00000000	64KB	ISA I/O space
0x00001000	4KB (fixed)	VME mailbox slave space
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
0x00000000	16MB (min)	DRAM space
0x10000000	~3.7GB (max)	VME A32 master space
  	128MB (std)
0x20000000	16MB (max)	VME A24 master space [1]
0xFB000000	64KB (fixed)	VME mailbox (A32) space
0xFBFF0000	64KB (max)	VME A16 master space
0xFC000000	256KB (fixed)	MPIC REGS
.TE

NOTE: [1] A24 and A32 address ranges must not overlap.

.ne 6
.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1 - shared memory
    \f3dc\f1 - Ethernet (10baseT or 100baseTX)

Motorola's Open Firmware and PPC1-Bug can be used to download and run VxWorks.
Consult the relevant user's manuals for details.

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP boot ROM:
.CS    
    cd target/config/mv230x
    make clean
    make bootrom
    cp bootrom.bin /tftpboot/boot.bin
.CE

Power down the board and switch the ROM jumper to select socketed FLASH.
Connect the Ethernet and console serial port cables, then power the board back
up.

.SS "Flashing the Boot ROM Using Motorola PPC1-Bug:" 1
At the PPC1-Bug prompt, start the system clock then set up the network transfer
from a TFTP host using `niot'.  To start the system clock, the \f3set\f1
command must be used.  The format is: set MMDDYYhhmm  where MM is month, DD is
day of month, YY is year, hh is hour (24-hour format), and mm is minutes.  This
command starts the system clock and sets the current date and time.

.CS
   PPC1-Bug>set 1016971302
.CE

Using `niot', the Client IP Address, Server IP Address, and Gateway IP Address
must be set up for the user's specific environment:

.CS
   PPC1-Bug>niot
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
   PPC1-Bug>
.CE

The file is transferred from the TFTP host to the target board using
the `niop' command.  Important: You must have a TFTP server running on your
host's subnet for the `niop' command to succeed.  The file name must be set to
the location of the binary file on the TFTP host.  The binary file must be
stored in the directory identified for TFTP accesses, but the file name is
a relative path and does not include the \f3/tftpboot\f1 directory name:

.CS
   PPC1-Bug>niop
   Controller LUN =00?
   Device LUN     =00?
   Get/Put        =G?
   File Name      =? boot.bin
   Memory Address =00004000?
   Length         =00000000?
   Byte Offset    =00000000?

   PPC1-Bug>
.CE

After the file is loaded onto the target, the `pflash' command is used
to put it into soldered FLASH parts.

.CS
   PPC1-Bug>pflash 4000:FFF00 ff000100
.CE

When the command is finished, power down the board and switch the ROM
jumper to select soldered FLASH.  Then power the board back up.

.SS "Flashing the Boot ROM Using Motorola Open Firmware:" 1

From the "ok" prompt on the console, use the `load' command to get the image
into RAM.  You must have a TFTP server running on your host's subnet for the
`load' command to succeed.  The command takes the following form:

.CS
    load /pci/ethernet@e:<host IP>,<file path/name>,<target IP>[,<gateway IP>]
.CE

.SS Note: 1
The modifiable parameter \f3load-base\f1 is set to the load-address of a
binary image to be loaded.  The factory preset value is 0x400000.

For example, \f3load-base\f1 must be modified to allow for the reserved 0x100
bytes at the beginning of a VxWorks boot image:

.CS
  ok load-base h# 100 + to load-base

  ok load /pci/ethernet@e:144.191.1.8,boot.bin,144.191.1.7

  Boot device: /pci/ethernet@e:144.191.1.8,boot.bin,144.191.1.7
  File and args:

  ok load-base h# 100 - to load-base
.CE

From the "ok" prompt, determine the starting memory address of soldered FLASH:

.CS
  ok 50 fal-l@
  fef80050  ff0b0006
            ^^^
.CE

Use the indicated first three nibbles followed by five zeros as the start
address.  In this example, the start address is ff000000.
(Note: "58 fal-l@" would return the socketed FLASH start address.)

From the "ok" prompt, use the \f3gflash\f1 command to program the image into
FLASH.  The command takes the following form:

.CS
    <start addr> <size> <flash start addr> (gflash)
.CE

In order to load the boot image into soldered FLASH, modify `load-base' as
follows:

.CS
  ok load-base 100000 ff000000 (gflash)

  Erasing ...
  Programming ...
  Verifying ....
  ok
.CE

Power down the board and switch the ROM jumper to select soldered FLASH.  Then
power the board back up.

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
For these boards, the value of the CPU clock speed is read from the CPU 
configuration register using the macro MEMORY_BUS_SPEED which is defined
in mv2600.h.  For example:

.CS
   clkFreqMhz = MEMORY_BUS_SPEED;
.CE

.SS "VME Interrupt Vectors"
Interrupt vectors chosen to generate normal VME interrupts under program
control must be even numbers.  

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

The Universe chip can also be configured to receive VME interrupts.  

Note that, if software specifies an odd number as the interrupt vector to be
transmitted during the IACK cycle, the STATUS/ID register will truncate it to
an even number.  
There is no configuration option to compensate for this
feature of the Universe chip.

.SS "Known Problems"
The Universe chip provides both a VME interface and a PCI-VME bridge.  The
older Universe chip (Universe I) has numerous flaws that cause initialization
difficulties and prevent guaranteed atomic VMEbus read-modify-write (RMW)
cycles.  It is also not capable of receiving more than one VME bus interrupt
level under conditions of frequent interrupts; it may lock up and prevent
further VME bus interrupt activity.  For further information, refer to
\f2Tundra Universe Device Errata.\f1

A redesigned Universe chip (Universe II) is forthcoming 
from Tundra that addresses the known flaws.  
Later revisions of Motorola boards may utilize the new chip.

The Motorola Raven chip has a flaw which ignores PCI bus `LOCK' signals during
access of local memory from the PCI bus.  Thus, an atomic RMW transaction
from the Universe chip is not guaranteed to be atomic in local memory.  A new
chip, Raven 3, is forthcoming from Motorola and addresses this flaw.

Contact a Motorola representative for details on the new chips.

Older generation VME backplanes often do not have slot 1 (the system controller
slot) hard-wired for interrupt acknowledge (IACK) daisy chain operation,
leaving this to be done by a board plugged in to the slot.  Because the
MVME2600 family of Motorola boards does not do this, VME interrupts may not
be sensed by an MVME2600 board used as a system controller in an old VME
backplane.  New VME backplanes usually have the left-most slot P1 connector
hard-wired so that pin A20 (IACK) is connected to A21 (IACKIN).  On old VME
backplanes, the user must add a jumper between pins A20 and A21 on the wire wrap
pins behind the P1 connector of slot 1.

.SS "Pseudo-PReP Memory Model"
The following table describes the modified PowerPC Reference Platform (PReP)
address maps created for VME from the CPU point of view.  Tornado-compatible
mapping deviates only slightly from the model.

.TS E
expand;
lf3 lf3 lf3
l l lw(1.8i) .
.ne 6
.sp .5
Start	Size	Access to
_
0x0	LOCAL_MEM_SIZE (16MB min)	DRAM
LOCAL_MEM_SIZE	(0x80000000 - LOCAL_MEM_SIZE)	[Not used]
0x80000000	8MB	PCI I/O space
0x80800000	0x3f800000	[Not used]
0xC0000000	16MB	PCI MEM space
0xC1000000	0x17000000	[Not used]
0xD8000000	128MB	T{
PCI MEM (max. A32 VME space)
T}
0xE0000000	16MB	T{
PCI MEM (A24 VME space)
T}
0xE1000000	0x0EFF0000	[Not used]
0xEFFF0000	64KB	T{
PCI MEM (A16 VME space)
T}
0xF0000000	64KB	T{
PCI MEM (VME REG. (A32) space)
T}
0xF0010000	0x0BFF0000	[Not used]
0xFC000000	256KB	MPIC Reg space
0xFC040000	0x02F40000	[Not used]
0xFEF80000	128KB	Falcon/Raven regs.
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

.ne 3
The master window address mappings are as follows:

.TS E
expand;
lf3 lf3 lf3 lf3
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
lf3 lf3 lf3 lf3
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
A32 (Mailbox)	0x40000000	4KB	0x00001000 (PCI bus)
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
0x00000000	8MB	PCI I/O space
0x00000000	64KB	ISA I/O space
0x00001000	4KB (fixed)	VME mailbox slave space
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
0x80000000	16MB (min)	DRAM space
0x18000000	16MB (std)	VME A32 master space
0x20000000	16MB (max)	VME A24 master space
0x2FFF0000	64KB (max)	VME A16 master space
0x30000000	64KB (fixed)	VME mailbox (A32) space
0x7C000000	256KB (fixed)	MPIC REGS
.TE

.SH "BOARD LAYOUT"
The diagrams below show flash EEPROM locations and jumpers relevant to VxWorks
configuration.  Boards prior to REV B have a markedly different board layout.

To determine if the board is REV B or later, refer to the circuit board
revision number which is printed on the back side of the board, in the rear
between the P1 and P2 connectors, just below the board serial number.  The
revision number is printed on a white background.  A number such as "08C" will
appear.  The last character ("C" in this example) indicates the revision level.

For all MVME230x boards, the debug and 10baseT/100baseTX ports appear on the
front panel.

.ne 4i

.bS
                                   REV A
______________________________               ______________________________
|             P1             |    MVME230x   |             P2              |
|                            ----------------                              |
|                             Does not use a                               |
|                            Transition Module                             |
|                                                                          |
|                                                                          |
|                                    +----+                                |
|                                   X|    |                                |
|                                   U|    |                                |
|                                   1+----+ <== Open Firmware              |
|                                    +----+     or PPC1-Bug                |
|                                   X|    |                                |
|                                   U|    |    (soldered flash on          |
|                                   2+----+     back of board)             |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|           A  R                                      J7  (SCON)   --> L   |
|           b  e                                      J8  (ROM ctrl) --> D |
|           o  s                                                           |
|           r  e 10/100                                                    |
|     Debug t  t BaseT           PMC 2                   PMC 1             |
|_____-----______-----___......................__....................._____|
            U  U
.bE

.bS
                              REV B and later
______________________________               ______________________________
|             P1             |    MVME230x   |             P2              |
|                            ----------------                              |
|                             Does not use a                               |
|                            Transition Module                             |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                         (soldered flash on back of board)                |
|                                                                          |
|                                        +----+  +----+                    |
|                         PPC1-Bug ==>  X|    | X|    |                    |
|                                       U|    | U|    |                    |
|                                       2+----+ 1+----+                    |
|           A  R                                     J16  (SCON)   --> L   |
|           b  e                                     J15  (ROM ctrl) --> D |
|           o  s                                                           |
|           r  e 10/100                                                    |
|     Debug t  t BaseT           PMC 2                   PMC 1             |
|_____-----______-----___......................__....................._____|
            U  U
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
.iB "Motorola MVME2300-Series VME Processor Module Installation and Use,"
.iB "Motorola MVME2300-Series VME Processor Module Programmer's Reference Guide,"
.iB "Motorola PowerPC 603 RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC 604 RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "DECchip 21140 PCI Fast Ethernet LAN Controller Hardware Reference Manual,"
.iB "National Semiconductor PC87308VUL (Super I/O Enhanced Sidewinder Lite) PC Controller Manual,"
.iB "SGS-Thompson MK48T59/559 CMOS 8K x 8 TIMEKEEPER SRAM Data Sheet,"
.iB "Zilog SCC (Serial Communications Controller) User's Manual,"
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

