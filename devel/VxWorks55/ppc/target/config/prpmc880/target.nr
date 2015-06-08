'\" t
.so wrs.an
.\" PrPMC880/target.nr - Motorola PrPMC target-specific documentation
.\"
.\" Copyright 2002 Wind River Systems, Inc.
.\" Copyright 1999-2003 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01a,20jun03,simon  Written (from version 01t of prpmc800/target.nr).
.\"
.\"
.TH "PrPMC880" T "Motorola PrPMC880" "Rev: 10 August 03" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola PrPMC880"

.SH "INTRODUCTION"
This manual entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.  This BSP is compatible with Wind River's
Tornado 2.2 development environment.

This BSP encompasses the PRPMC880 board. It consists of the MPC7447 processor 
with integrated L1 and L2 cache, the MV64360 system controller, up to 1GB of 
ECC DDR memory, two 10/100/1000 ethernet ports,  one RS232 serial ports, 
up to 32MB soldered FLASH, up to 64MB socketed FLASH, one 133MHz 64-bit 
PCI/PCI-X busses,  and one Real-Time Clock.

.SS "OPERATING MODES"
The PrPMC880 BSP operates in PCI Host mode (installed in PMC site 2 of the
PrPMC carrier) and limited slave mode (installed in PMC site 1 of the
PrPMC carrier).  In limited slave mode, the BSP will come up to a bootrom
prompt.  By default, the slave PrPMC880 does not use its on-board Ethernet
device.  PCI inbound and outbound translation register are programmed
so that the slave can see the Monarch's DRAM and the Monarch PrPMC880 can
see the slave's DRAM.  There is currently no inter Monarch to slave
communication.

In PCI Host mode, the BSP performs PCI auto-configuration at startup.

.SS "CAUTIONS and WARNINGS"
.IP "\f3Preparing the PrPMCBASE-001 for VxWorks\f1"
In previous non-system cPCI boards, the 21554 non-transparent PCI-to-PCI bridge
was configured for software initialization. To address a start-up timing issue
under PPC-Bug, the 21554 is configured for self-initialization as delivered
from the factory. Before using the PrPMCBASE-001 with the default VxWorks
configuration, the 21554 must be configured for software initialization.

The 21554 initialization mode is controlled by bit 2 (LSB=0) of offset 0x31 in
an I2C SROM attached to the 21554. The state of this bit can be altered from
the BUG command line using the I2C SROM byte editor as follows( because the 
motoload in PrPMC880 does not support 21554, please use other prpmc board 
to change the bit):

.CS
   ppcbug>srom;d

   Device Address =$0000A000 (N/Y)? y
   Reading SROM into Local Buffer.....
   $00 (&000) 80? <return>
   $01 (&001) 00? <return>
   .
   . Continue to press <return> until byte 0x31 is reached.
   .
   $31 (&049) 00? 04 (or 00 for BUG mode)
   $32 (&050) 00? .
   Update SROM (Y/N)? y
   Writing SROM from Local Buffer.....
   Verifying SROM with Local Buffer...
.CE
 
The following symptoms are the result of attempted operation with the wrong
21554 initialization mode:
 
.TS C
center;
l l
l l .
Env	Symptom
-
BUG	T{
Everything appears normal, but a "ver" command from the cPCI Host (MCP750,
CPV500, etc.) does not report the presence of a PrPMCBASE-001.
T}
VxWorks	T{
The mis-configured PrPMCBASE-001 can consume all available PCI allocation space
and prevent the proper configuration of other boards in the cPCI chassis. If in
doubt, re-build the PCI Host BSP with INCLUDE_SHOW_ROUTINES defined and display
the PCI header of the 21554 on the PrPMCBASE-001 using the pciHeaderShow command
at the debug console of the PCI Host. If BAR2 and/or BAR3 are non-zero using
the pre-built PrPMC BSP binary image, the PrPMCBASE-001 is mis-configured.
T}
.TE

.IP "Hardware Environment"
This BSP was developed using a MCG PrPMC Compact PCI Carrier (PrPMCBASE-001) as
the base board and a PrPMC880 installed in PMC site 2. These instructions assume 
that a PrPMC Carrier/PrPMC880 combination are in use. Instructions for modifying 
the BSP for use with a customer-designed carrier board are contained later in 
this entry.

IMPORTANT NOTE :
The PRPMC_CARRIER 3850 ver B board does not have non-transparent bridge so 
INCLUDE_DEC2155X must be undefined in the BSP. 
When using MCG PrPMC Compact PCI Carrier(except 3850 VerB) as the base board
and install a PrPMC880 in PMC site 2, the serial port and rear ethernet port 
can not work.   

.IP "Compact PCI Backpanel Clocks"
The PrPMCBASE requires the presence of valid clocks on the Compact PCI backpanel
which are driven by the system processor board (MCP750, CPV5000, etc.). Proper
operation of the PrPMCBASE therefore requires that a system processor be
installed in the Compact PCI chassis's system slot (right-most slot as viewed
from the front of the chassis).

.IP "Memory ECC Protection"
This BSP supports ECC memory and configures the memory controller for ECC
operation by default. If ECC protection is not desired, undefine INCLUDE_ECC in
config.h, rebuild both the bootrom and kernel images and re-flash the bootroms.

.SS "Boot ROMS"
The PRPMC880 supports two banks of FLASH memory. Bank A (32-64 MBytes) consists
of two Intel StrataFlash 3.3 volt devices configured to operate in 32-bit mode
and is soldered onboard.  Bank B is located on the add on mezzanine card and 
consists of four PLCC sockets which will be populated with 8-bit wide Intel/AMD
29lv040 devices. This will provide a minimum of 1 MB of socketed FLASH memory
for boot option configured as a 32-bit wide port. Either bank can be used as
the boot bank, When Bank B is on, PrPMC880 will select Bank B as the boot 
bank. if not, then PrPMC880 will select Bank A. The boot bank (either
A or B) is always located at 0xFF800000. The bank A is located at 0xF0000000.

.IP "Boot Line Parameters"
A limited non-volatile storage capability is implemented using a 256-byte
Serial EEPROM (SROM). The entire SROM contents are reserved for storage of the
VxWork boot line.

To load VxWorks, and for more information, follow the instructions in the
\f2Tornado User's Guide: Getting Started.\f1

.SS "Jumpers"
The following jumpers are relevant to VxWorks configuration:

.TS E
expand;
lf3 lf3 lf3 lf3
l l l lw(2.6i) .
.sp .5
Board	Jumper	Function	Description
_
PrPMC880	N/A	N/A	T{
No jumpers on PrPMC880.
T}
.TE

.SH "FEATURES"
The following subsections list all supported and unsupported features, as well
as any feature interaction.

.SS "Supported Features"
The following features of the PrPMC880 board family are supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 7
.sp .5
Feature Description
_
Processors	T{
MPC7447
Up to 133MHz bus clock 
T}
FLASH	T{
32MB-64MB Flash BANK A (32-bit)
8MB Flash BANK B
T}
DRAM	T{
Up to 1GB DDR SDRAM
T}
NVRAM	T{
(8K)Atmel AT24C64
T}
RTC    T{
MAX6900
T}
Peripherals	T{
One async serial ports; 
Two 10/100/1000 Ethernet interfaces; 
T}
PCI Interface	T{
64-bit PCI/PCI-X; complies with \f2PCI Local Bus Specification\f1,
Revision 2.1
T}
Miscellaneous	T{
MV64360 DMA Controller supported
T}
.TE

.SS "Unsupported Features"
The following features of the PrPMC880 board family are not supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Feature	Description
_
Miscellaneous	T{
T}
.TE

.SS "Feature Interactions"
None known.

.SH "HARDWARE DETAILS"
This section details device drivers and board hardware elements.

.IP "Memory ECC Protection"
This BSP supports ECC memory and, by default, disables ECC support in the 
system memory controller by undefining INCLUDE_ECC in config.h.  To enable
ECC support simply change #undef INCLUDE_ECC, in config.h, to 
#define INCLUDE_ECC.  Disabling ECC support will result in a slight
performance increase, since partial (less than 64-bit) writes to memory 
will not require a RMW transaction as described in section "8.6 Read Modify 
Write" in Rev 0.98 of the MV64360 System Controller manual. 

.SS "Devices"
The device drivers and libraries included with this BSP are:

.TS C
center;
rw(1.0i) lw(3.7i) .
vxMpscUart:	T{
MV64360 MPSC driver (serial port).
T}
AuxClk:	T{
Motorola timer driver for auxiliary clock.
T}
I2c:	T{
I2C support.
T}
Mpic:	T{
MV64360 interrupt controller driver.
T}
Phb:	T{
Motorola PCI bus bridge driver.
T}
Smc:	T{
Motorola System Memory Controller.
T}
Dma:	T{
MV64360 DMA Controller Driver.
T}
MAX6900:	T{
MAX6900 RTC device driver.
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
MPC7445/MPC7447 L2 Cache support.
T}
sysMotVpd:	T{
Vital Product Data Support.
T}
sysMotVpdShow:	T{
Vital Product Data Show routines.
T}
.TE

.SS "Dec2155x PCI-to-PCI Non-Transparent Bridge Support"
This BSP contains support for the Dec2155x non-transparent PCI-to-PCI
bridge located on the PrPMCBASE-001 board. This device provides read/write
access to and from the Compact PCI bus (cPCI).

The following support is provided:
.IP
Up to 4 user configurable downstream cPCI to local PCI windows.
.IP
Up to 2 user configurable upstream local PCI to cPCI windows.
.IP
Support for in-bound "doorbell" interrupts.
.IP
Support for cPCI backpanel interrupts.
.IP
cPCI to local CPU address translation.
.IP
Local CPU to cPCI address translation.
.IP
Build-time validation of Dec2155x configuration parameters.

.SS "Dec2155x Support Limitations"
The PReP standard does not support 64-bit PCI addressing. Therefore, this
BSP does not provide support for 64-bit addressing through the Dec2155x.

There is a limitation when the cPCI to local PCI or cPCI to local CPU
address translation routines are presented with a cPCI address which maps
into a downstream window on the local board. The translation will succeed
and return an address, but when that address is accessed, the Dec2155x
will attempt to access one of its own downstream windows. The transfer
will fail because PCI devices cannot access themselves on the cPCI bus.
Depending on how error detection is configured, the result will be
invalid data or a PCI Master Abort.

Interrupt vectors are provided for the interrupts associated with
Dec2155x Hot Swap Power State transitions, Intelligent I/O (I2O), and the
Upstream Memory 2 Base Address Register but no other support for these
features is provided.

During system startup, the Dec2155x must be configured and unlocked
before the host enumerates the cPCI bus. To meet this timing requirement,
the Dec2155x is configured by the vxWorks boot ROM image. If changes to
the Dec2155x configuration are made, new boot ROMs are required in
addition to a new kernel. For proper operation, the Dec2155x
configuration in the Boot ROMs must match the configuration used by the
kernel.

The Dec2155x places certain limitations on window sizes and translation
values. This BSP adheres to those limitations and provides
build-time parameter checking to help avoid misconfigurations.
Modifications to the default Dec2155x configuration provided in this BSP
must be made with care to avoid invalid configurations. Information on
the default Dec2155x configuration provided by this BSP is presented in
the next section and modification guidelines appear later in this entry.

.SS "Dec2155x Default Configuration"
The default Dec2155x configuration supports a host processor (MCP750) and
up to 7 cPCI peripheral boards. The following interoperability is supported:

.IP
Host access to PrPMC880 CSR and the low 4MB of PrPMC880 DRAM.
.IP
PrPMC880 access to the low 4MB of host DRAM.
.IP
PrPMC880 access to peer cPCI boards.
.LP

The BSP provides these features using the following Dec2155x
configuration:

\f3Primary CSR and Downstream Memory 0 BAR:\f1
.TS C
center;
r lw(3.2i) .
Size:	T{
4MB
T}
Direction:	T{
In-Bound (cPCI to PrPMC880)
T}
cPCI Adrs:	T{
Dynamic (assigned by host)
T}
Local PCI Adrs:	T{
PCI_SLV_MEM_BUS (Dynamic - Harrier inbound translation base reg 0 value*)
T}
Local CPU Adrs:	T{
PCI_SLV_MEM_LOCAL (0x00000000 by convention)
T}
Use:	T{
R/W access to CSR (low 4KB) and PrPMC880 DRAM (above 4KB)
T}
.TE

* - Note that the MV64360 inbound translation base register 0 value is
dynamically programmed at startup to map to low-order DRAM.  The
dynamically programmed value is dependent upon the value which is
dynamically programmed by the PCI autoconfiguration routine into the
MV64360's Inbound Translation Base Address Register 0.  The amount of
DRAM mapped through this inbound register set is dependent upon the
setting of PCI2DRAM_MAP_SIZE in "config.h". 

\f3Upstream I/O or Memory 0 BAR:\f1
.TS C
center;
r lw(3.2i) .
Size:	T{
4MB
T}
Direction:	T{
Out-Bound (PrPMC880 to cPCI)
T}
cPCI Adrs:	T{
CPCI_MSTR_MEM_BUS (0x80000000 by convention)
T}
Local PCI Adrs:	T{
Dynamic (assigned by PrPMC880)
T}
Local CPU Adrs:	T{
Dynamic (based on local PCI adrs)
T}
Use:	T{
R/W access to host DRAM
T}
.TE


\f3Upstream Memory 1 BAR:\f1
.TS C
center;
r lw(3.2i) .
Size:	T{
32MB
T}
Direction:	T{
Out-Bound (PrPMC880 to cPCI)
T}
cPCI Adrs:	T{
Base cPCI address of the host's dynamic PCI configuration area (0x00000000 for
the default MCP750 BSP)
T}
Local PCI Adrs:	T{
Dynamic (assigned by PrPMC880)
T}
Local CPU Adrs:	T{
Dynamic (based on local PCI adrs)
T}
Use:	T{
R/W access to cPCI devices
T}
.TE

The remaining Dec2155x Base Address Registers are not used by the BSP and
are available for use by the application.

.SS "Dec2155x Address Translation:
Due to the dynamic nature of PCI address allocation, the locations of the
upstream Dec2155x windows move as devices are added to the PrPMC880 PCI
bus. Since these windows map the cPCI space into the local PrPMC880 PCI
and CPU address spaces, their positions determine where the cPCI
resources appear when viewed by the PrPMC880 CPU and any PCI devices resident
on the PrPMC carrier board. Likewise, the downstream windows move as cPCI
devices are added and removed. The downstream windows are used to map the
on-board PCI and DRAM resources into the cPCI address space for access by the
host and other cPCI devices.

To assist with address translation, two translation routines are
provided by this BSP:

.TS C
center;
rw20 lw(3.2i) .
sysLocalToBusAdrs()	T{
Translates a local CPU address to an equivalent cPCI or local PCI memory or
I/O address.
T}

sysBusToLocalAdrs()	T{
Translates a cPCI or local PCI memory or I/O space address to a local CPU
equivalent address.
T}
.TE

.TS C
center;
l l .
\f3NOTE:\f1	T{
The translations performed by sysLocalToBusAdrs() and
sysBusToLocalAdrs() are not symmetrical if one of the endpoints is the Compact
PCI bus. sysLocalToBusAdrs() translates by locating a downstream window which
makes the local CPU address visible in the cPCI address space.
sysBusToLocalAdrs() performs a similar operation by locating an upstream window
which makes the cPCI address visible in the local CPU address space. Since the
two sets of windows map different areas of the local address space,
the translation is not reversible.
T}
.TE

.SS "Accessing Dec2155x CSR Registers"
Due to dynamic PCI address allocation, the PCI address assigned to the
Dec2155x CSR area cannot be known until runtime. To determine the
assigned address, it is necessary to read the Secondary CSR memory
BAR (or the Secondary CSR I/O BAR if I/O space is to be used).

The following code fragment derives the CPU address of the Scratchpad 0
register using its PCI memory space address:

.CS
    UINT32 bar;

    /* get the contents of the secondary CSR memory BAR
       (see note below) */

    if (pciConfigInLong (0, DEC2155X_PCI_DEV_NUMBER, 0,
                         DEC2155X_CFG_SEC_CSR_MEM_BAR,
                         &bar) != OK)
        {
        return (ERROR);
        }

    /* calculate the local PCI address of the scratchpad 0
       register */

    bar += DEC2155X_CSR_SCRATCHPAD0;

    /* convert the result to the CPU equivalent address */

    if (sysBusToLocalAdrs (PCI_SPACE_MEM_PRI, (char *)bar,
                         (char **)&bar) != OK)
        {
        return (ERROR);
        }

    return (bar);
.CE

.TS C
center;
l l .
\f3NOTE:\f1	T{
Using the constant DEC2155X_PCI_DEV_NUMBER ensures that the
on-board Dec2155x is read. If a search of the local PCI bus had
been performed using the Dec2155x device ID, the returned Bus,
Device and Function numbers may have corresponded to a Dec2155x
part found on an installed PMC card.
T}
.TE

Once the local CPU address is known, the cPCI address can be derived by
adding the following code fragment before returning the result:

.CS
    if (sysLocalToBusAdrs (PCI_SPACE_MEM_SEC,
                           (char *)bar,
                           (char **)&bar) != OK)
        return (ERROR);
    else
        return (bar);
.CE

.SS "Internal Dec2155x Interrupt Sources"
At start-up, all Dec2155x interrupt sources are masked and cleared.
Before unmasking an interrupt, an application ISR service routine must be
attached to the appropriate Dec2155x interrupt vector using intConnect().
Multiple ISR service routines can be connected to each vector if required
by the application. Once the handler is attached, the interrupt can be
enabled and disabled by calling sysDec2155xIntEnable() or
sysDec2155xIntDisable() as required. Interrupt vector definitions for the
Dec2155x internal interrupt sources are defined in prpmc880.h.

Unique interrupt vectors are provided for each of the 16 bits in the
Dec2155x Secondary IRQ register. Bit 0 (LSB) corresponds to
DEC2155X_DOORBELL0_INT_VEC with the remaining bits mapped in sequence.
These doorbell interrupts can be used for host-to-PrPMCBASE or
PrPMCBASE-to-PrPMCBASE event notification. The Dec2155x interrupt handler
clears these interrupts which simplifies the application ISR.

Individual interrupt vectors are also provided for Dec2155x Hot Swap
Power State and I2O in-bound list events. The Dec2155x interrupt handler
also clears these interrupts.

The 64 Upstream Memory 2 BAR Page Crossing interrupts are all presented
on a single interrupt vector and the application ISR is responsible for
clearing the bits serviced. Calls to sysDec2155xIntEnable() and
sysDec2155xIntDisable() enable or disable all 64 interrupts.

The Dec2155x interrupt handler provides a default service routine for all
unclaimed interrupt vectors, including the Upstream Memory 2 BAR Page
Crossing interrupt. The default routine reports the event and clears the
interrupt source.

.SS "Compact PCI Backpanel Interrupts"
The Dec2155x can generate cPCI backpanel interrupts using any of the bits
in the Primary IRQ register if they have been un-masked by the host. The
following code fragment generates a compact PCI backpanel interrupt by
setting bit 15 (MSB) of the Primary IRQ register:

.CS
    if (sysBusIntGen (DEC2155X_DOORBELL15_INT_LVL,
                      DEC2155X_DOORBELL15_INT_VEC) != OK)
        return (ERROR);
.CE

Note that the cPCI bus does not provide an interrupt vector to the host.
The vector number passed to sysBusIntGen() simply identifies which bit in
the register to set. It is the host's responsibility to locate the
interrupt source and clear the interrupt.

.SH "BSP CONFIGURATION"
Most BSP configuration values are taken from on-board Vital Product Data
(VPD) and Serial Presence Detect (SPD) serial EEPROMs. If invalid VPD or SPD
information is suspected or reported, defining VPD_ERRORS_NONFATAL in config.h 
may permit operation using default parameters.
These build switches are intended for use during debug only as they hard-code
non-optimized SDRAM timing and other VPD information. 

.SS "PCI Dynamic Allocation Spaces"
PCI0_MSTR_IO_SIZE, PCI0_MSTR_MEMIO_SIZE and PCI0_MSTR_MEM_SIZE control
the sizes of the available PCI address spaces.  PCI0_MSTR_MEMIO_LOCAL
(set to 1-Gigabyte (0x40000000) by default), controls the starting
local address of the PCI memory space.  The windows defined by these
parameters must be large enough to accommodate all of the PCI memory
and I/O space requests found during PCI autoconfiguration.  If they
are not, some devices will not be autoconfigured.  For all prpmc880s
on the same PCI bus, the value assigned to PCI0_MSTR_MEMIO_LOCAL must
be greater than or equal to the value of the largest amount of DRAM
among these boards.  PCI0_MSTR_MEMIO_LOCAL must also have the same
numeric value for all of the boards which appear on the same PCI bus.

.TS C
center;
l l .
\f3NOTE:\f1	T{
PCI auto-configuration is performed by the bootroms. Any changes to
PCI0_MSTR_MEMIO_LOCAL, PCI0_MSTR_IO_SIZE, PCI0_MSTR_MEMIO_SIZE or 
PCI0_MSTR_MEM_SIZE requires the creation of a new bootrom image.
T}
.TE

By default, the companion MCP750 BSP allocates a 32MB area aligned to a
32MB boundary for dynamic PCI configuration. To access peer PrPMC880 DRAM
areas, an upstream window must be opened which matches the size of the
host's dynamic PCI configuration area. For translation to work correctly,
the host's dynamic PCI configuration area must be aligned to a multiple
of the area's size and the corresponding Dec2155x upstream translation
register must contain the area's base cPCI address (not CPU address).
Since this BSP supports peer-to-peer access between PrPMC880 DRAM areas,
the default dynamic PCI configuration area for the PrPMC880 is 64MB aligned
to a 64MB boundary which satisfies these requirements.

In addition to the peer access window, sufficient space must also be
available for mapping the host DRAM upstream window and any space
required by PrPMCBASE-resident PCI devices. A margin must also be allowed
for areas that are unusable due to window alignment requirements.

If the application does not require peer-to-peer PrPMC880 DRAM access, the
large 32MB window used to contain the host's dynamic PCI configuration
area can be eliminated with a corresponding decrease in the required
PrPMC880 dynamic PCI configuration area. If peer-to-peer doorbell
interrupts are still required, the doorbell interrupt registers of peer
PrPMCBASE boards may be accessed through an I/O window which has much smaller
CPU address space requirements. This would require re-configuring the
default BSP to access host DRAM through Upstream Memory 1 BAR and using
the Upstream I/O or Memory 0 BAR to access the peer PrPMCBASE doorbell
interrupt registers.

.SS "Altering the Default Dec2155x Configuration:
Altering the Dec2155x configuration requires the careful consideration of
several items:

.IP
Dec2155x window sizes and alignment.
.IP
Dec2155x translation values.
.IP
The size and alignment of the host's dynamic PCI configuration area.
.IP
The size and alignment of the PrPMC880's dynamic PCI configuration area.
.LP

The Dec2155x window parameters are controlled by #defines in config.h.
There are three defines associated with each window:

.TS C
center;
rw11 lw(4.0i) .
\&..._SIZE	T{
determines the size of the window in bytes and must be an
integral power of two. The minimum size for a PCI I/O
space window is 64 bytes. The minimum size for a PCI
memory space window is 4KB. To disable a window, set the
size to 0. Note that the Dec2155x will not allow the
Primary CSR and Downstream Memory 0 BAR to be
disabled. If the size of this window is set to zero, the
Dec2155x will default to a 4KB window.
T}

\f3NOTE:\f1	T{
If a window value is not a power of 2, or is below the minimum size, sysLib.c
will not compile.
T}
.TE

.TS C
center;
rw11 lw(4.0i) .
\&..._TYPE	T{
determines the type of the window and any placement
restrictions. For proper operation, the window must be
configured for placement anywhere in the 32-bit PCI
address space.
T}
.TE

.TS C
center;
rw11 lw(4.0i) .
\&..._TRANS	T{
determines the base address of the window on the target
PCI bus. It is important to remember that this is a local
PCI address (downstream window) or a cPCI address
(upstream window). The translation value chosen must be
an even multiple if the window size.
T}

\f3NOTE:\f1	T{
If the translation value is not a multiple of the window
size, sysLib.c will not build.
T}
.TE

The default window sizes can be reduced without altering the sizes of the
dynamic PCI configuration area. However, if the required values are
significantly reduced from the default values, reducing the size of the
dynamic PCI configuration area reduces the size of the MMU page tables at
the ratio of 128:1 (a 128KB reduction saves 1KB of MMU table space).

.SS "Shared Memory Support"

The PrPMC880 BSP supports shared memory backplane communication with
the MCP750 or CPV5000 as the Compact PCI host node.  The Wind River
documentation provides a great deal of information regarding shared
memory concepts.  The section below provides tutorial style information
regarding the setup of a shared memory system involving the PrPMC880/PrPMCBASE
and either a MCP750 or a CPV5000.

.TS C
center;
l l .
\f3NOTE:\f1	T{
Wind River shared memory support is available for Monarch/Slave
configurations with the anchor and shared memory pool residing on the
Monarch.  Shared memory support is also available for
MCP750/Monarch/Slave configurations in which the Monarch and Slave are
configured together on a single carrier card.  Multiple carrier cards
with Monarch/Slave configurations are possible in the same shared
memory setup as long as the shared memory pool is configured on the
system-slot board (MCP750 or CPV5000).
T}
.TE

Setting up a working shared memory system involves proper setting of
certain "config.h" parameters and proper setting of boot parameters
via the "c" command from the boot prompt.  There are three components
involved in shared memory communication which must be configured
properly to create a working system:

.IP "Anchor:"
This is an area of memory which must be accessible to all nodes
participating in shared memory backplane communication.  The anchor
points to the actual shared memory buffer pool which must be located
in the same memory space as the anchor itself.  The associated
"config.h" parameter is SM_ANCHOR_ADRS.  In certain configurations,
nonzero nodes will "poll" for the location of the anchor.  "config.h"
#define's which come into play for polling are SM_OFF_BOARD,
SYS_SM_SYSTEM_MEM_POLL, and SYS_SM_BUS_NUMBER.

.IP "Master node:"
This node is always designated as node zero.  It is the node which
sets up the anchor and shared memory pool.  Once the anchor and shared
memory pool is set up, the master node acts as a peer with the other
nodes.  The node number (0 in this case) is one of the boot parameters
which can be set up with the "c" command from the bootline prompt.

.IP "Sequential addressing:"
This is governed by a "config.h" parameter, INCLUDE_SM_SEQ_ADDR and is
used when sequential IP addresses are assigned to the participating
nodes.  Node zero is assigned the lowest IP address, followed by nodes
1, 2 etc. which are assigned the subsequent and sequential IP addresses.
The advantage of sequential addressing is that fewer boot parameters
must be specified to configure the system.
.LP

The following restrictions apply to shared memory configurations.

.TS C
center;
l l .
1)	T{
Node zero must not boot over the shared memory interface.  Only
nonzero nodes are allowed to boot over the shared memory "sm"
interface.
T}
2)	T{
The location of the anchor must be statically determinable by the
master node (node 0).  That is, the location of the anchor must
either be a build-time static parameter or it must be able to
be communicated to the master node via the "sm=xxxxxxxx" boot
configuration parameter.  The nonzero nodes need not know the
location of the anchor at build or boot time but can be configured
to poll for the anchor dynamically.
T}
.TE

.TS C
center;
l l .
\f3NOTE:\f1	T{
Another piece of shared memory terminology is "host node".
The "host node" is the node which configures the compact PCI bus
during startup initialization.  In a system consisting of an MCP750
and one or more PrPMCBASE boards, the "host node" is the MCP750.  Don't
confuse "host node" with "master node".  "Master node" is simply a
synonym for "node 0".  The "host node" may or may not be the "master
node".  Note also that the "host node" need not necessarily be a VxWorks
node.
T}
.TE

Below are the crucial "config.h" parameters involved in shared memory:

.IP "CPCI_MSTR_MEM_BUS (address):"
The parameter is used to identify the address at which the
system-slot board's DRAM will be configured.  This is dependent 
on the host
board used and is defined in "config.h".  The explanation says
to set the value to 0x80000000 for a MCP750 host (default) or
0x00000000 for a CPV5000 host.

.IP "SM_OFF_BOARD (TRUE or FALSE):"
The parameter has a configurable value of either TRUE or FALSE
and directly determines the value of SM_ANCHOR_ADRS (the anchor
address).

If SM_OFF_BOARD is defined as FALSE, then the anchor is on-board
and SM_ANCHOR_ADRS is defined to be
LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET.
LOCAL_MEM_LOCAL_ADRS is defined as 0x0 in "config.h" and
SM_ANCHOR_OFFSET is defined as 0x4100 in "config.h" to work with
an MCP750.  SM_ANCHOR_OFFSET needs to be changed to 0x1100 in
"config.h" to work with a CPV5000.

If defined as TRUE, then SM_ANCHOR_ADRS is defined as a function
call: sysSmAnchorAdrs() (defined in "sysLib.c").  This function
will dynamically poll, at system startup, various locations
(explained below) for the exact location of the shared memory
anchor.

Note that if "sm=xxxxxxxx" is used as a boot parameter, then
SM_OFF_BOARD has no effect.  The value of "xxxxxxxx" will be used
as the anchor location regardless of the setting of SM_OFF_BOARD.
If simply "sm" is used as a boot parameter, then SM_OFF_BOARD is
queried at initialization time to determine if polling is required
or not.

.IP "SYS_SM_BUS_NUMBER:"
This can be #define'd to be either SYS_LOCAL_PCI_BUS_NUMBER or
SYS_BACKPLANE_BUS_NUMBER.  If a system-slot board such as the MCP750 is
participating in shared memory, then SYS_SM_BUS_NUMBER must be #define'd to
be SYS_BACKPLANE_BUS_NUMBER.  This signifies the fact that the Monarch
and/or slave must generate traffic on the compactPCI bus in order for
the shared memory system to function properly.  If only a Monarch and
Slave are involved in the shared memory system then all of the
activity for shared memory is restricted to the local PCI bus.  In
that case SYS_SM_BUS_NUMBER should be #define'd as
SYS_LOCAL_PCI_BUS_NUMBER.

.IP "SYS_SM_SYSTEM_MEM_POLL (#define or #undef):"
This define only has an effect if anchor polling is called for
(because SM_OFF_BOARD is defined as TRUE and "sm" is used with no
"=xxxxxxxx").  In this case, simply defining SYS_SM_SYSTEM_MEM_POLL
will cause the node to poll for the anchor at compact PCI bus
address CPCI_MSTR_MEM_BUS + SM_ANCHOR_OFFSET (0x80004100).
"System memory" (which is the host node's DRAM) will
be included as one of the locations where the anchor might reside.
Note that other locations may be polled as well (explained later).

Not defining SYS_SM_SYSTEM_MEM_POLL will prevent the polling of
system  memory for the anchor.

.IP "SYS_SM_ANCHOR_POLL_LIST (#define or #undef):"
This define has an effect only if polling is called for (see
SM_OFF_BOARD explained above).  When defined,
SYS_SM_ANCHOR_POLL_LIST allows a list of devices, identified by
device/vendor ID and subsystem ID/subsystem vendor ID to be
specified as candidates for the anchor location.  Devices which
appear on the bus #define'd by SYS_SM_BUS_NUMBER (described
above) are found and if they
appear on the list defined by SYS_SM_ANCHOR_POLL_LIST, they are
checked to see if they contain the shared memory anchor.  The memory
accessed by the BAR at configuration offset "N" (where "N" is also defined in 
SYS_SM_ANCHOR_POLL_LIST) is queried.  The block of memory viewable
through this BAR is examined at address offset SM_ANCHOR_OFFSET (defined
in "config.h).
If SYS_SM_ANCHOR_POLL_LIST is not defined, ALL devices on the 
specified bus will be considered candidates for the anchor location and
will be polled.  If SYS_SM_ANCHOR_POLL_LIST defined but empty, NO
devices on bus will be considered candidates for
the anchor location.  In that case, the only location polled would
be system memory if SYS_SM_SYSTEM_MEM_POLL (see above) was defined.

.IP "INCLUDE_SM_SEQ_ADDR (#define or #undef)"
If "undef'ed", sequential addressing is disabled.  This symbol
is defined by default.
.LP

Consider a system consisting of an MCP750 (host node) and two PrPMCBASE
boards. The following three configurations are the only ones possible:

.TS C
center;
l l .
1)	T{
MPC750 (contains anchor) with one or more PrPMC Base cards, each
card containing either a Monarch or Monarch and slave.
T}
2)	T{
MCP750 and PrPMCBASE card (Monarch only) with the Monarch containing
the anchor.  Note a slave is not allowed in this configuration.
T}
3)	T{
PrPMCBASE card with Monarch and Slave installed, Monarch contains
the anchor.  Note that a system-slot card such as the MCP750 is
required to be present but does not participate in shared memory.
T}
.TE

Below is a description of how each of the above systems would be
configured.  Crucial "config.h" and boot parameter settings for an
example system are given.  In each example, SYS_SM_ANCHOR_POLL_LIST
was defined to contain information identifying the Dec2155x bridge
chip (present on the PrPMCBASE board).  See "config.h" for the example of
how this was done.

.TS C
center;
l l .
1)	T{
Three nodes, MCP750 master (node 0, anchor), PrPMC Base with Monarch/Slave:
Note sequential addressing is turned off.
T}
.TE
.CS

   MCP750:

        #define SM_OFF_BOARD FALSE
        #undef  SYS_SM_SYSTEM_MEM_POLL
        #undef  INCLUDE_SM_SEQ_ADDR

        boot device          : dc
        processor number     : 0
        host name            : sunray
        inet on ethernet (e) : 144.191.16.191
        inet on backplane (b): 144.140.200.1:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.191.16.253
        target name (tn)     : gamma


   PrPMCBASE/PrPMC880-1 (Monarch):

        #define INCLUDE_DEC2155X
        #define SM_OFF_BOARD TRUE
        #define SYS_SM_SYSTEM_MEM_POLL
        #define SYS_BACKPLANE_BUS_NUMBER    1
	#define SYS_SM_BUS_NUMBER SYS_BACKPLANE_BUS_NUMBER
        #undef  INCLUDE_SM_SEQ_ADDR

        boot device          : sm
        processor number     : 1
        host name            : sunray
        inet on ethernet (e) : 144.191.16.192
        inet on backplane (b): 144.140.200.2:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.140.200.1
        target name (tn)     : alpha

   PrPMCBASE/PrPMC880-2 (Slave):

        (same "config.h" setup as Monarch)

        boot device          : sm
        processor number     : 2
        host name            : sunray
        inet on ethernet (e) : 144.191.16.193
        inet on backplane (b): 144.140.200.3:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.140.200.1
        target name (tn)     : beta
.CE

Note that multiple PrPMCBASE cards, each with a Monarch/Slave
pair could be added to this setup.  Each different prpmc880 would have
to configured with a different "processor number", 
"inet on ethernet", "inet on backplane", and "target name".

.TS C
center;
l l .
2)	T{
Two nodes, MCP750 (node 1), PrPMCBASE with Monarch (node 0, anchor):
T}
.TE
.CS

   MCP750:

        #define SM_OFF_BOARD TRUE
        #define SYS_BACKPLANE_BUS_NUMBER    1
        #undef SYS_SM_SYSTEM_MEM_POLL
        #undef INCLUDE_SM_SEQ_ADDR 

        boot device          : sm
        processor number     : 1
        host name            : sunray
        inet on ethernet (e) : 144.191.16.192
        inet on backplane (b): 144.140.200.2:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.140.200.1
        target name (tn)     : gamma

   PrPMCBASE/PrPMC880-1 (Monarch - slave not allowed):

        #define INCLUDE_DEC2155X
        #define SM_OFF_BOARD FALSE
        #define SYS_BACKPLANE_BUS_NUMBER    1
        #define SYS_SM_SYSTEM_MEM_POLL
	#define SYS_SM_BUS_NUMBER SYS_BACKPLANE_BUS_NUMBER
        #undef INCLUDE_SM_SEQ_ADDR

        boot device          : geisc0
        processor number     : 0
        host name            : sunray
        inet on ethernet (e) : 144.191.16.191
        inet on backplane (b): 144.140.200.1:ffffff00 
	host inet (h)        : 144.191.16.163 
        gateway inet (g)     : 144.191.16.253
        target name (tn)     : alpha

.CE

.TS C
center;
l l .
3)	T{
Two nodes, PrPMC880-1 master (node 0, anchor), PrPMC880-2 slave
T}
.TE
.CS


   PrPMCBASE/PrPMC880-1 (Monarch):

        #define SM_OFF_BOARD FALSE
        #define SYS_BACKPLANE_BUS_NUMBER    1  (Not Used)
	#define SYS_LOCAL_PCI_BUS_NUMBER 0
	#define SYS_SM_BUS_NUMBER SYS_LOCAL_PCI_BUS_NUMBER
        #undef  SYS_SM_SYSTEM_MEM_POLL
        #undef  INCLUDE_SM_SEQ_ADDR

        boot device          : geisc0
        processor number     : 0
        host name            : sunray
        inet on ethernet (e) : 144.191.16.192
        inet on backplane (b): 144.140.200.1:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.191.16.253
        target name (tn)     : alpha

   PrPMCBASE/PrPMC880-1 (Slave):

        #define INCLUDE_DEC2155X
        #define SM_OFF_BOARD TRUE
        #define SYS_BACKPLANE_BUS_NUMBER    1   (Not Uesd)
	#define SYS_LOCAL_PCI_BUS_NUMBER 0
	#define SYS_SM_BUS_NUMBER SYS_LOCAL_PCI_BUS_NUMBER
        #define SYS_SM_SYSTEM_MEM_POLL
        #undef  INCLUDE_SM_SEQ_ADDR

        boot device          : sm
        processor number     : 1
        host name            : sunray
        inet on ethernet (e) : 144.191.16.193
        inet on backplane (b): 144.140.200.2:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.140.200.1
        target name (tn)     : beta
.CE


.SS "Memory Maps"
On-board RAM for these boards always appears at address 0x00000000 locally.

Dynamic memory sizing is supported.  By default, LOCAL_MEM_AUTOSIZE is
defined so memory is auto-sized at hardware initialization time.
If auto-sizing is not selected, LOCAL_MEM_SIZE must be set to the actual size
of DRAM memory available on the board to ensure all memory is available.
The default fixed RAM size is set to 64MB (see LOCAL_MEM_SIZE in config.h).

Note that LOCAL_MEM_SIZE only controls the amount of memory mapped by the MMU.
It does not control the amount of memory detected and configured by the Bootrom.
The amount of physical memory indicated by the Serial Presence Detect data
determines the memory controller configuration and, if enabled, the ECC
initialization range. If hardware memory problem is suspected, the Bootrom can
be configured to ignore the Serial Presence Detect data and program the
memory controller with a set of default parameters. 

.SS "Interrupts"
The system interrupt vector table has 256 entries.  Vectors for the various
devices on the buses are assigned hierarchically as follows:

.TS E
expand;
lf3 lf3
l lw(2.6i) .
.ne 6
.sp .5
Vector# Assigned to
_
0x00 - 0x1f  MV64360 Main Cause (low) interrupts
0x20 - 0x3f  MV64360 Main Cause (high) interrupts
0x40 - 0x4f  MV64360 GPP interrupts
0x50 - 0xff  Unassigned
.TE

The interrupt number is numerically equivalent to interrupt vector
and interrupt level an is computed in the following way:

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
.ne 6
.sp .5
Pin#	Vector# I/O	Polarity	Source
_
0	NA	O	High	Unused
1	NA	I	High	Unused
2	NA	O	High	Unused
3	NA	I	High	Unused
4	NA	I	High	Unused
5	NA	I	High	Unused
6	0x46	I	Low	Watchdog WDNMI# interrupt
7	NA	I	Low	BCM5421S PHY interrupts (ORed)
8	NA	I	High	Unused
9	NA	I	High	Unused
10	NA	I	High	Unused
11	NA	I	High	Unused
12	NA	I	High	Unused
13	NA	I	High	Unused
14	NA	I	High	Unused
15	NA	I	High	Unused
16	0x50	I	Low	PCI-PMC0 INTA# PMC1 INTC#
17	0x51	I	Low	PCI-PMC0 INTB# PMC1 INTD#
18	0x52	I	Low	PCI-PMC0 INTC# PMC1 INTA#
19	0x53	I	Low	PCI-PMC0 INTD# PMC1 INTB#
20	NA	I	High	Unused
21	NA	I	High	Unused
22	NA	I	High	Unused
23	NA	I	High	Unused
24	NA	I	High	Unused
25	NA	O	Low	Watchdog WDE# output
26	NA	O	Low	Watchdog WDNMI# output
27	NA	I	High	Unused
28	NA	I	High	Unused
29	NA	I	High	Unused
30	NA	I	High	Unused
31	NA	I	High	Unused
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
auto-configuration library. When INCLUDE_AUTOCONF is defined (default), the BSP
will automatically locate and configure installed PCI devices. When
INCLUDE_AUTOCONF is not defined (intended for debug use only), add-in PCI
devices will not be located or configured.

When PCI auto-configuration is selected, the auto-configuration library will be
called from sysHwInit to discover and configure the installed PCI devices and
bridges. Device configuration includes the following PCI information:

.IP "Base Address Registers (BARs)"
Space in the address map is dynamically allocated to each valid BAR detected.
Allocation pools are maintained for the following PCI address spaces:

16-Bit PCI I/O

32-Bit PCI I/O

PCI Memory I/O (non-prefetchable memory)

PCI Memory (pre-fetchable)


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

.IP "Transparent PCI-to-PCI Bridge Configuration"
Transparent PCI-to-PCI bridges encountered during PCI auto-configuration will
be configured as necessary and devices detected behind the bridge will be
configured as described above. Bridge configuration consists of the following:

Primary Bus Number, Secondary Bus Number and Subordinate Bus Number are
filled in according to the bridge's position in the system.

I/O Base and Limit registers are configured as required to forward PCI
transactions to PCI devices detected and configured beyond the bridge.

Memory Base and Memory Limit registers are configured as required to forward
PCI transactions to PCI devices detected and configured beyond the bridge.

Command Register = I/O enabled, Memory enabled and Bus Master enabled.

Cache Line Size = _CACHE_ALIGN_SIZE/4

Primary Latency Timer = PCI_LAT_TIMER

Secondary Latency Timer = PCI_LAT_TIMER

.SS "Serial Configuration"
The PRPMC880's one asynchronous serial interfaces is provided by the MPSC0
in the MV64360. An AMP 15-pin INFOPORT Series III PC card low profile 
connector is placed on the front panel of the PrPMC Carrier board and is 
configured as a DTE connection.

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"
SCSI is not available on the PrPMC880 board family.

.SS "MV64360 DMA Configuration"
To enable DMA support using the MV64360, change the #undef INCLUDE_MV64360_DMA
in config.h to #define.  Instructions for configuring DMA Descriptor Lists are
contained in sysMv64360Dma.c and sysMv64360Dma.h.

.SS "Network Configuration"
The PrPMC880 has two Ethernet port which is 10/100/1000 Mbps full duplex Ethernet
ports compatible using a RJ45 jack on the front panel for connection to this facility.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX or 1000baseT.  

The Media Access Control (Ethernet) address for each port is obtained from VPD.  

.SS "PCI Access"
The 64-bit PCI/PCI-X busses are fully supported under the
.I PCI Local Bus Specification, Revision 2.1.
All configuration space accesses are made with BDF (bus number, device number, 
function number) format calls in the pciConfigLib module.  For more information,
refer to the man pages.

.SS "Using Customer-Designed Carrier Boards"
To use a customer-designed carrier board, the following changes must be made to
this BSP:

In config.h:
Modify the definition of PCI_x_Dyy_ROUTE (x = bus, yy = device)to match the configuration
of your hardware. These macro maps PCI IDSEL numbers to interrupt inputs.
The 4 columns in the table map INTA, INTB, INTC and INTD respectively as
viewed from the PrPMC site. The default table matches the MCG PrPMCBASE-001
Carrier board.

The values of PCI_MSTR_IO_SIZE, PCI_MSTR_MEMIO_SIZE and PCI_MSTR_MEM_SIZE may
also require modifications depending in the required PCI dynamic configuration
areas required.

.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1	- shared memory
    \f3geisc0\f1- Rear Ethernet (10baseT, 100baseTX, or 1000baseT)
    \f3geisc1\f1- Rear Ethernet (10baseT, 100baseTX, or 1000baseT)

Motorola's Motload can be used to download and run VxWorks.
Consult the relevant user's manuals for details.

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP boot ROM:
.CS
    cd target/config/prpmc880
    make clean
    make bootrom_uncmp.bin
    cp bootrom_uncmp.bin /tftpboot/bootrom_uncmp.bin
.CE

Power down the board and attach mezzanine card(with motoload) to the board.
Connect the Ethernet and console serial port cables, then power the board back
up.

.SS "Flashing the Boot ROM Using Motorola MotLoad:"
The file is transferred from the TFTP host to the target board using
the `tftpGet' command.  Important: You must have a TFTP server running on your
host's subnet for the `tftpGet' command to succeed.  The file name must be set
to the location of the binary file on the TFTP host.  The binary file must be
stored in the directory identified for TFTP accesses, but the file name is
a relative path and does not include the \f3/tftpboot\f1 directory name:

.CS
   MotLoad> tftpGet -c10.193.146.41 -s10.193.146.16 -d/dev/enet0 -fboot.bin
   Network Loading from: /dev/enet0
   Loading File: boot.bin
   Load Address: 005FB000

   Client IP Address      = 10.193.146.41
   Server IP Address      = 10.193.146.16
   Gateway IP Address     = 10.193.146.253
   Subnet IP Address Mask = 255.255.255.0

   Network File Load in Progress...

   MotLoad>
.CE

After the file is loaded onto the target, the `flashProgram' command is used
to put it into soldered FLASH parts.
For 32M solodered Flash:

.CS
   MotLoad>flashProgram -n0xfff00 -o0x1f00100
.CE

For 64M solodered Flash:

.CS
   MotLoad>flashProgram -n0xfff00 -o0x3f00100
.CE

When the flashProgram command is finished, power down the board. Then power
the board back up.

.SH "SPECIAL CONSIDERATIONS"
.SS "Delivered Objects"
The delivered objects are: `bootrom.hex', `vxWorks', `vxWorks.sym', and
`vxWorks.st'.

.SS "Make Targets"
The make targets are listed as the names of object-format files.  Append `.hex'
to each to derive a hex-format file name and append .bin for binary format.

 `bootrom'
 `bootrom_uncmp'
 `bootrom_res_high'  (`bootrom_res' does not build)
 `vxWorks' (with `vxWorks.sym')
 `vxWorks_rom'
 `vxWorks.st'
 `vxWorks.st_rom'
 `vxWorks.res_rom_res_low' (`vxWorks.res_rom' does not build)
 `vxWorks.res_rom_nosym_res_low' (`vxWorks.res_rom_nosym' does not build)

.SS "Special Routines"

.SH "BOARD LAYOUT"
The diagrams below show flash EEPROM locations relevant to VxWorks
configuration:

.ne 4i

.bS

              PrPMC880
____________________________________
|              P14          P12     |
|           =========== =========== |
|              P13          P11     |
|           =========== =========== |
|                PMC connectors     |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
|_________________________----______|
                         10/100/1000base T
                         Serial port
.bE

For the PrPMCBASE-001 board, the debug and 10baseT/100baseTX ports appear on the
front panel.

.ne 4i

.bS

                                   PMCBASE
____________________________________________________________________________
|                                                                          |
|  =========== =========== =========== ===========                         |
|                                                                          |
|  =========== =========== =========== ===========                         |
|         PMC slot 2              PMC slot 1                               |
|        (SYSCON Slot)                                                     |
|                                                                          |
|      +-----+ +-----+                                                     |
|      |XU2  | |XU1  |                                                     |
|      |     | |     |                                                     |
|      +-----+ +-----+                                                     |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                                          |
|                                                   J29 (ROM Ctrl) --> L   |
|                                                                          |
|                                                                          |
|__.......................___.......................____----___----___----_|
     PCI Mezzanine Card        PCI Mezzanine Card      10/100  Com2   Com1
          Cutout                    Cutout             base T  Slot   Slot
                                                                2      1
.bE
    Key:
    X	vertical jumper installed
    :	vertical jumper absent
    -	horizontal jumper installed
    "	horizontal jumper absent
    0	switch off
    1	switch on
    U	three-pin vertical jumper, upper jumper installed
    D	three-pin vertical jumper, lower jumper installed
    L	three-pin horizontal jumper, left jumper installed
    R	three-pin horizontal jumper, right jumper installed

.SH "SEE ALSO"
.tG "Getting Started,"
.pG "Configuration"

.SH "BIBLIOGRAPHY"
.iB "Motorola PrPMC880 Processor PMC Programmer's Reference Guide,"
.iB "Motorola Computer Group Online Documentation,
.iB  http://library.mcg.mot.com/mcg/boards"
.iB "MPC7450 RISC Microprocessor Family User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "IEEE P1386.X Draft 0.5 - Processor Mezzanine Specification (PrPMC),"
.iB "IEEE P1386.1 Draft 2.0 - PCI Mezzanine Card Specification (PMC),"
.iB "IEEE P1386 Draft 2.0 - Common Mezzanine Card Specification (CMC),"
.iB "Peripheral Component Interconnect (PCI) Local Bus Specification, Rev 2.1,"
.iB "PCI to PCI Bridge Architecture Specification 2.0,"
.iB "PICMG 2.0 D2.14 CompactPCI Specification,"
.iB "Digital Semiconductor 2155x PCI-to-PCI Bridge for Embedded Applications Hardware Reference Manual,"






