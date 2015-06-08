'\" t
.so wrs.an
.\" PrPMC800/target.nr - Motorola PrPMC target-specific documentation
.\"
.\" Copyright 2002 Wind River Systems, Inc.
.\" Copyright 1999-2001 Motorola, Inc. All Rights Reserved
.\"
.\" modification history
.\" --------------------
.\" 01t,19jul02,dtr  Changing Shared Memory config description.
.\" 01s,29mar02,dtr  Adding comments for TFFS with strataflash.
.\" 01r,21mar02,kab  remove ref to elfToBin (obs in T2.2); 
.\"		     add supported target list.
.\" 01q,16nov01,dtr  Small changes for dec21143,gei mods. TFFS support added.
.\" 01p,16nov01,scb  Minor mods to cover last minute T2.1 updates.
.\" 01o,10oct01,scb  Added info regarding new mappings and shared memory.
.\" 01n,18sep01,srr  Replaced i82559 driver references with FEI driver.
.\"                  Updated Bank A & B FLASH references.
.\" 01m,10aug01,srr  Added instructions PrPMC-G and Carrier 1 boards.
.\" 01l,21jun01,srr  Added instructions for enabling Harrier DMA driver.
.\" 01k,14jun01,srr  Updated Monarch prpmc800 instructions for slave's Ethernet
.\"                  to increase the size of PCI_MSTR_MEMIO_SIZE.
.\" 01j,08jun01,srr  Updated instructions for prpmc800 slave using Ethernet.
.\" 01i,14may01,blk  Added instructions for prpmc800 slave on mcpn765
.\" 01h,09may01,bvc  Removed Known Limitations entire empty section
.\" 01g,08may01,bvc  Removed Known Limitations on shared memory
.\" 01f,15jan01,krp  Added section on Known Limitations
.\" 01e,10jan01,krp  Added TFFS in the unsupported featues section
.\" 01e,09jan01,krp  Corrected minor spelling errors in the file.
.\" 01d,08jan01,krp  Changed the board series part number description.     
.\" 01c,05jan01,krp  Added note no WRS SM supported in slave device.
.\" 01b,20dec00,krp  Added tab characters for format tables correctly.
.\" 01a,20nov00,krp  Written (from version 01g of prpmc750/target.nr).
.\"
.\"
.TH "PrPMC800" T "Motorola PrPMC800/PrPMC800EXT" "Rev: 10 August 01" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "Motorola PrPMC800/PrPMC800EXT"

.SH "INTRODUCTION"
This manual entry provides board-specific information necessary to run
VxWorks.  Before using a board with VxWorks, verify that the board runs in the
factory configuration by using vendor-supplied ROMs and jumper settings and
checking the RS-232 connection.

This BSP encompasses the PrPMC800 and PrPMC800EXT Processor PMC single-board
computers and the MCG Compact PCI PrPMC Carrier board (PrPMCBASE-001).
The PrPMC800 is a processor PMC module based on the PowerPlus III architecture.
It consists of the MPC75x or MPC7410 processor and L2 backside cache, the 
Harrier System Controller/PCI Host Bridge ASIC, up to 32MB to 256MB of 
ECC-protected SDRAM, a 10 BaseT/100 Base TX ethernet channel and an RS232 
serial debug port.

The PrPMC800/PrPMC800EXT series part numbers are of the form:

    PrPMC800-wxyz

    where
        w = CPU Type
           1 = MPC7410 with Ethernet
           2 = MPC7410 without Ethernet (66MHz PCI)
           5 = MPC750 with Ethernet
           6 = MPC750 without Ethernet (66MHz PCI)

        x = CPU Speed
           2 = 450MHz  

        y = ECC SDRAM size
           4 =	64 MB
           5 =	128 MB
           6 =	256 MB
           7 =	512 MB

        z = Ethernet Configuration (if present)
           1 =	Ethernet out front of PrPMC
           9 =	Ethernet out the PCI connector of the PrPMC

For example, an PrPMC800-5251 denotes a 450 MHz PowerPC 750-based board having
128MB of ECC SDRAM in the standard PMC form factor with Ethernet out the front.
Note that not all product combinations are available. Standard equipment
includes 8MB FLASH (9MB on the extended PMC form factor).

The BAT registers are not supported in the current cache management strategy.

.SS "OPERATING MODES"
The PrPMC800 BSP operates in PCI Host mode (installed in PMC site 2 of the
PrPMC carrier) and limited slave mode (installed in PMC site 1 of the
PrPMC carrier).  In limited slave mode, the BSP will come up to a bootrom
prompt.  By default, the slave PrPMC800 does not use its on-board Ethernet
device.  PCI inbound and outbound translation register are programmed
so that the slave can see the Monarch's DRAM and the Monarch PrPMC800 can
see the slave's DRAM.  There is currently no inter Monarch to slave
communication.

In PCI Host mode, the BSP performs PCI auto-configuration at
startup. If a Dec/Intel 21143 is present on Bus 0, the ethernet drivers will be
attached to the device and full ethernet I/O and network boot capabilities will
be available. If multiple 21143 devices are present, the ethernet driver will be
attached to the device with the lowest IDSEL.

The Monarch can control the slave's Ethernet device by defining  
INCLUDE_SECONDARY_ENET and defining PCI_IDSEL_SEC_LAN to 18.

The slave PrPMC800 can control its own on-board Ethernet device by
defining SLAVE_OWNS_ETHERNET in config.h.

.SS "CAUTIONS and WARNINGS"
.IP "\f3Preparing the PrPMCBASE-001 for VxWorks\f1"
In previous non-system cPCI boards, the 21554 non-transparent PCI-to-PCI bridge
was configured for software initialization. To address a start-up timing issue
under PPC-Bug, the 21554 is configured for self-initialization as delivered
from the factory. Before using the PrPMCBASE-001 with the default VxWorks
configuration, the 21554 must be configured for software initialization.

The 21554 initialization mode is controlled by bit 2 (LSB=0) of offset 0x31 in
an I2C SROM attached to the 21554. The state of this bit can be altered from
the BUG command line using the I2C SROM byte editor as follows:

.CS
   PPC-Bug>srom;d

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

.IP "Save a Copy of PPC-Bug"
The FLASH ROMs installed in the Bank B sockets of the PrPMCBASE-001 and
PrPMC800EXT may not contain a BUG image. Before installing a VxWorks bootrom
image in FLASH ROM Bank A, verify that an operational copy of PPC-Bug is
present in Bank B of the PrPMCBASE-001 or the PrPMC800EXT. If necessary, copy
the BUG image from Bank A to Bank B by entering the following line at the BUG
prompt:

.CS
   PPC-Bug>pflash f0000000:100000 ff800000
.CE

Do not overwrite Bank A until the BUG image in Bank B is known to be
operational. Failure to preserve an operational copy of the BUG can render the
board inoperable until a replacement FLASH set can be obtained from the factory.

.IP "Hardware Environment"
This BSP was developed using a MCG PrPMC Compact PCI Carrier (PrPMCBASE-001) as
the base board and a PrPMC800EXT/PrPMC800 installed in PMC site 2. These
instructions assume that a PrPMC Carrier/PrPMC800 or PrPMC Carrier/PrPMC800EXT
combination are in use. Instructions for modifying the BSP for use with a
customer-designed carrier board are contained later in this entry.

The PrPMC800 BSP also supports the MCG PrPMC Carrier 1 board (5 PMC sites)
and the MCG PrPMC-G board with 82543 Gigabit Ethernet support.  To select
between the three of these boards, the value in config.h for CARRIER_TYPE
must be set to PRPMC_BASE (default), PRPMC_CARRIER_1, or PRPMC_G. CARRIER_TYPE
is not available in the Tornado project facility.

IMPORTANT NOTE :
The PRPMC_CARRIER_1 board does not have dec21554 bridge so INCLUDE_DEC2155X 
must be undefined in the BSP. JP4 must be set to 1-2 on PRPMC_CARRIER_1 
board(default). 

.IP "Bank B (Socketed FLASH)) Voltage"
The PrPMCBASE-001 and the PrPMC800EXT use Am29LV040 3.3-volt (only) socketed
FLASH parts. These parts are not 5-volt tolerant. The MVME2400 also uses these
3.3-volt parts as socketed FLASH and its sockets may be used to program FLASH
parts if a dedicated programmer is not available.

.IP "Debug Consoles"
The PrPMCBASE routes the serial outputs from the PMC sites as follows:
.TS C
center;
lf3 lf3
l l .
PMC Site	Serial Port Connection
-
1 (non-Monarch)	Front Panel COM1
2 (Monarch)	Front Panel COM2
.TE

Therefore, access to the debug console of the Monarch PrPMC (installed at PMC
site 2) requires a DTE cable connected to COM2 (not COM1). Likewise, access to
the debug console of the non-Monarch PrPMC requires a DTE cable connected to
COM1.

.IP "Compact PCI Backpanel Clocks"
The PrPMCBASE requires the presence of valid clocks on the Compact PCI backpanel
which are driven by the system processor board (MCP750, CPV5000, etc.). Proper
operation of the PrPMCBASE therefore requires that a system processor be
installed in the Compact PCI chassis's system slot (right-most slot as viewed
from the front of the chassis).

.IP "Processor Address and Data Bus Parity"
This BSP supports parity protection of the processor's address bus and data bus.
This option is enabled by default. If parity protection is not desired, undefine
INCLUDE_BPE in config.h, rebuild both the bootrom and kernel images and re-flash
the bootroms.

.IP "Memory ECC Protection"
This BSP supports ECC memory and configures the memory controller for ECC
operation by default. If ECC protection is not desired, undefine INCLUDE_ECC in
config.h, rebuild both the bootrom and kernel images and re-flash the bootroms.

.SS "Boot ROMS"
.IP "PrPMC800"
The PrPMC800 supports two banks of FLASH memory. Bank A (8-32 MBytes) is onboard
FLASH while Bank B is optional FLASH on the host board and accessed through the
PMC connector. FLASH bank B selection is controlled by a jumper on the
PrPMC800EXT when the PrPMC800EXT is installed on a customer-designed carrier.

.TS C
center;
l l .
\f3NOTE:\f1	T{
When the PrPMC800EXT is used with the PrPMCBASE-001 Compact PCI PMC carrier
board, either the PrPMC800EXT's Bank B (socketed FLASH) parts and boot ROM
selection jumper or the Carrier board's Bank B (socketed FLASH) parts and boot
ROM selection jumper must be depopulated to avoid bus contention issues.
T}
.TE

.IP "PrPMCBASE-001"
The Compact PCI PrPMC Carrier board has one 1MB bank of socketed FLASH composed
of two AMD Am29LV040 FLASH parts (Bank B). A boot FLASH selection jumper is
present on the PrPMC Carrier board which selects between FLASH bank A (soldered
onto the PrPMC800/PrPMC800EXT) or FLASH bank B (socketed) on the Carrier board.

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
PrPMC800	N/A	N/A	T{
No jumpers on PrPMC800.
T}
PrPMC800EXT	J3	ROM controller	T{
Install the jumper across pins 2 and 3 to select Bank A (socketed FLASH).
Install the jumper across pins 1 and 2 to select Bank B (soldered FLASH)
[factory configuration].
T}
PrPMCBASE-001	J29	ROM controller	T{
Install the jumper across pins 2 and 3 to select Bank A (socketed FLASH).
Install the jumper across pins 1 and 2 to select Bank B (soldered FLASH)
[factory configuration].
T}
.TE

.TS C
center;
l l .
\f3NOTE:\f1	T{
When the PrPMC800EXT is used with the PrPMCBASE-001 Compact PCI PMC carrier
board, either the PrPMC800EXT's socketed Bank B FLASH parts and boot ROM
selection jumper or the Carrier board's socketed Bank B FLASH parts and boot
ROM selection jumper must be depopulated to avoid bus contention issues.
T}
.TE

For jumper configuration details, see the board diagrams at the end of
this entry and in the hardware manual.

.SH "FEATURES"
The following subsections list all supported and unsupported features, as well
as any feature interaction.

.SS "Supported Features"
The following features of the PrPMC800 board family are supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 7
.sp .5
Feature Description
_
Processors	T{
MPC750-450Mhz
MPC7410-450Mhz
Up to 100MHz bus clock (derived from PCI bus clock)
T}
FLASH	T{
8MB-32MB Flash BANK A (16-bit)
Optional on the base board upto 512MB Flash BANK B (16-bit).
T}
DRAM	T{
32 to 256MB ECC SDRAM; auto-sized or fixed
T}
Peripherals	T{
one async serial debug port,
10baseT/100baseTX Ethernet interface (Extended PrPMC and PrPMCBASE-001 only)
T}
PCI Interface	T{
32-bit address, 32-bit data; complies with \f2PCI Local Bus Specification\f1,
Revision 2.1
T}
TFFS	T{
True File Flash System for the Intel StrataFlash memory is supported.
T}
Miscellaneous	T{
RESET switch (Extended PrPMC)
Harrier DMA Controller supported
T}
.TE

.SS "Unsupported Features"
The following features of the PrPMC800 board family are not supported:

.TS E
expand;
lf3 lf3
lw13 lw(3.7i) .
.ne 6
.sp .5
Feature	Description
_
System	T{
Watchdog Timers
T}
PCI Interface	T{
64-bit data; The hardware will perform 64-bit transfers if requested by an
external PCI-master device, but does not generate 64-bit transactions in
normal operation.
T}
Miscellaneous	T{
ABORT switch (Extended PrPMC)
T}
.TE

.SS "Feature Interactions"
.IP "MPIC Spurious Interrupts"
A race condition can exist between PCI write posting and interrupt
processing which can cause an MPIC spurious interrupt.  The problem occurs
when a device's interrupt service routine writes to clear the interrupt
source and then returns to the interrupted code. When the PCI bus is very
busy, the write takes a while to get onto the bus and reach the
interrupting device. During this time, the device's interrupt will remain
asserted. If the PowerPC re-enables external interrupts before the PCI
write has reached the interrupting device, the processor will see the
interrupt still asserted and re-enter the MPIC interrupt routines.

When the MPIC handler reads the vector, the MPIC reports a spurious
interrupt because the PCI write has generally completed by then and the
device's interrupt has now been cleared.

.IP "Spurious Interrupt Workaround"
Modify the driver to perform a read from the PCI device (after writing to
the device to clear the interrupt) to ensure that
the write has fully propagated.  sysPciOutWordConfirm() will do this
automatically. Note that sysPciOutWordConfirm() reads from the address
written which may cause an undesirable side-effect depending on the design
of the hardware. If it does, just add a read from any safe location on the
device. The primary goal is force to the write out of the posting queues before
proceeding.

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
I2C support.
T}
Mpic:	T{
Motorola MPIC interrupt controller driver.
T}
Phb:	T{
Motorola PCI bus bridge driver.
T}
Smc:	T{
Motorola System Memory Controller.
T}
Dma:	T{
Motorola DMA Controller.
T}
fei82557End:	T{
10baseT/100baseTX Intel 82557/9 Ethernet driver.
T}
dec21x40End:	T{
10baseT/100baseTX DEC 21x4x Ethernet driver.
T}
gei82543End:	T{
10baseT/100baseTX/1000BaseT Intel 82543 Ethernet driver.
T}
dec2155xCpci:	T{
DEC 2155x Non-Transparent PCI-to-PCI Bridge support.
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
SmcShow:	T{
System Memory Controller configuration Show routine.
T}
sysCache:	T{
MPC750/MPC7410 L2 1MB/2MB Cache support.
T}
sysMotVpd:	T{
Vital Product Data Support.
T}
sysMotVpdShow:	T{
Vital Product Data Show routines.
T}
sysMotVpdUtil:	T{
Vital Product Data Utility routines.
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
Host access to PrPMC800 CSR and the low 4MB of PrPMC800 DRAM.
.IP
PrPMC800 access to the low 4MB of host DRAM.
.IP
PrPMC800 access to peer cPCI boards.
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
In-Bound (cPCI to PrPMC800)
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
R/W access to CSR (low 4KB) and PrPMC800 DRAM (above 4KB)
T}
.TE

* - Note that the Harrier inbound translation base register 0 value is
dynamically programmed at startup to map to low-order DRAM.  The
dynamically programmed value is dependent upon the value which is
dynamically programmed by the PCI autoconfiguration routine into the
Harrier's Inbound Translation Base Address Register 0.  The amount of
DRAM mapped through this inbound register set is dependent upon the
setting of PCI2DRAM_MAP_SIZE in "config.h".  The default amount is
16MB which is the minimum amount which can be mapped to allow the
fei (ethernet) driver to function.

\f3Upstream I/O or Memory 0 BAR:\f1
.TS C
center;
r lw(3.2i) .
Size:	T{
4MB
T}
Direction:	T{
Out-Bound (PrPMC800 to cPCI)
T}
cPCI Adrs:	T{
CPCI_MSTR_MEM_BUS (0x80000000 by convention)
T}
Local PCI Adrs:	T{
Dynamic (assigned by PrPMC800)
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
Out-Bound (PrPMC800 to cPCI)
T}
cPCI Adrs:	T{
Base cPCI address of the host's dynamic PCI configuration area (0x00000000 for
the default MCP750 BSP)
T}
Local PCI Adrs:	T{
Dynamic (assigned by PrPMC800)
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
upstream Dec2155x windows move as devices are added to the PrPMC800 PCI
bus. Since these windows map the cPCI space into the local PrPMC800 PCI
and CPU address spaces, their positions determine where the cPCI
resources appear when viewed by the PrPMC800 CPU and any PCI devices resident
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
Dec2155x internal interrupt sources are defined in prpmc800.h.

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
information is suspected or reported, defining NONFATAL_VPD_ERRORS, BYPASS_VPD
and/or BYPASS_SPD in config.h may permit operation using default parameters.
These build switches are intended for use during debug only as they hard-code
non-optimized SDRAM timing and other VPD information. Since the SDRAM timing is
configured by the Bootrom, changing the state of BYPASS_SPD requires rebuilding
the Bootrom image and re-flashing.

.SS "PCI Dynamic Allocation Spaces"
PCI_MSTR_IO_SIZE, PCI_MSTR_MEMIO_SIZE and PCI_MSTR_MEM_SIZE control
the sizes of the available PCI address spaces.  PCI_MSTR_MEMIO_LOCAL
(set to 1-Gigabyte (0x40000000) by default), controls the starting
local address of the PCI memory space.  The windows defined by these
parameters must be large enough to accommodate all of the PCI memory
and I/O space requests found during PCI autoconfiguration.  If they
are not, some devices will not be autoconfigured.  For all prpmc800s
on the same PCI bus, the value assigned to PCI_MSTR_MEMIO_LOCAL must
be greater than or equal to the value of the largest amount of DRAM
among these boards.  PCI_MSTR_MEMIO_LOCAL must also have the same
numeric value for all of the boards which appear on the same PCI bus.

.TS C
center;
l l .
\f3NOTE:\f1	T{
PCI auto-configuration is performed by the bootroms. Any changes to
PCI_MSTR_MEMIO_LOCAL, PCI_MSTR_IO_SIZE, PCI_MSTR_MEMIO_SIZE or 
PCI_MSTR_MEM_SIZE requires the creation of a new bootrom image.
T}
.TE

By default, the companion MCP750 BSP allocates a 32MB area aligned to a
32MB boundary for dynamic PCI configuration. To access peer PrPMC800 DRAM
areas, an upstream window must be opened which matches the size of the
host's dynamic PCI configuration area. For translation to work correctly,
the host's dynamic PCI configuration area must be aligned to a multiple
of the area's size and the corresponding Dec2155x upstream translation
register must contain the area's base cPCI address (not CPU address).
Since this BSP supports peer-to-peer access between PrPMC800 DRAM areas,
the default dynamic PCI configuration area for the PrPMC800 is 64MB aligned
to a 64MB boundary which satisfies these requirements.

In addition to the peer access window, sufficient space must also be
available for mapping the host DRAM upstream window and any space
required by PrPMCBASE-resident PCI devices. A margin must also be allowed
for areas that are unusable due to window alignment requirements.

If the application does not require peer-to-peer PrPMC800 DRAM access, the
large 32MB window used to contain the host's dynamic PCI configuration
area can be eliminated with a corresponding decrease in the required
PrPMC800 dynamic PCI configuration area. If peer-to-peer doorbell
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
The size and alignment of the PrPMC800's dynamic PCI configuration area.
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

The PrPMC800 BSP supports shared memory backplane communication with
the MCP750 or CPV5000 as the Compact PCI host node.  The Wind River
documentation provides a great deal of information regarding shared
memory concepts.  The section below provides tutorial style information
regarding the setup of a shared memory system involving the PrPMC800/PrPMCBASE
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


   PrPMCBASE/PrPMC800-1 (Monarch):

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

   PrPMCBASE/PrPMC800-2 (Slave):

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
pair could be added to this setup.  Each different prpmc800 would have
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

   PrPMCBASE/PrPMC800-1 (Monarch - slave not allowed):

        #define INCLUDE_DEC2155X
        #define SM_OFF_BOARD FALSE
        #define SYS_BACKPLANE_BUS_NUMBER    1
        #define SYS_SM_SYSTEM_MEM_POLL
	#define SYS_SM_BUS_NUMBER SYS_BACKPLANE_BUS_NUMBER
        #undef INCLUDE_SM_SEQ_ADDR

        boot device          : fei0
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
Two nodes, PrPMC800-1 master (node 0, anchor), PrPMC800-2 slave
T}
.TE
.CS


   PrPMCBASE/PrPMC800-1 (Monarch):

        #define SM_OFF_BOARD FALSE
        #define SYS_BACKPLANE_BUS_NUMBER    1  (Not Used)
	#define SYS_LOCAL_PCI_BUS_NUMBER 0
	#define SYS_SM_BUS_NUMBER SYS_LOCAL_PCI_BUS_NUMBER
        #undef  SYS_SM_SYSTEM_MEM_POLL
        #undef  INCLUDE_SM_SEQ_ADDR

        boot device          : fei0
        processor number     : 0
        host name            : sunray
        inet on ethernet (e) : 144.191.16.192
        inet on backplane (b): 144.140.200.1:ffffff00
        host inet (h)        : 144.191.16.163
        gateway inet (g)     : 144.191.16.253
        target name (tn)     : alpha

   PrPMCBASE/PrPMC800-1 (Slave):

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
It does not control the amount of memory detected and configured by the 
Bootrom. The amount of physical memory indicated by the Serial Presence Detect
data determines the memory controller configuration and, if enabled, the ECC
initialization range. If hardware memory problem is suspected, the Bootrom can
be configured to ignore the Serial Presence Detect data and program the
memory controller with a set of default parameters. For more information on
this feature, see the BYPASS_SPD note in config.h.

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
00 - 0f	[User defined]
10 - 1f	All MPIC interrupts
20 - 23	system timers
24 - 27	system interprocessor dispatch
28	board detected internal errors
29 - 5f	[User defined]
60 - 72	Dec2155x interrupts
73 - ff	[User defined]
.TE

Vector numbers not in the table are not used by this BSP.

The Multi-Processor Interrupt Controller (MPIC) sets system interrupt
priorities and serves as controller of all external interrupts.  Each
of its 16 interrupt control registers, designated IRQ0 through IRQ15, can be
programmed with a relative priority from 15, the highest, to 0, the lowest.  A
priority of zero effectively disables the interrupt.  All but three of the 16
control registers has been hardwired to a particular interrupt source.  The IRQ
number and priority assignments are as follows:

.TS E
expand;
lf3 lf3 lf3
l l lw(2.6i) .
.ne 6
.sp .5
MPIC IRQ	Polarity	IRQ Source
_
IRQ0	High	Host INT 0 - Board Specific
IRQ1	N/A	Not used
IRQ2	Low	Board Specific interrupt (non-PCI)
IRQ3	Low	Watch Dog Timer 1 or 2
IRQ4	N/A	Not used 
IRQ5	N/A	Not used 
IRQ6	Low	HOSTINT1 
IRQ7	Low	HOSTINT2
IRQ8	Low	HOSTINT3
IRQ9	Low	PCI INTA
IRQ10	Low	PCI INTB or On-board Ethernet
IRQ11	Low	PCI INTC
IRQ12	Low	PCI INTD
IRQ13	N/A	Not used 
IRQ14	N/A	Not used 
IRQ15	N/A	Not used 
.TE

For further details, refer to the appropriate board's reference guide.

There are only four PCI bus interrupts: A, B, C, and D.  They are shared among
all PCI bus devices and do not have levels.  PCI bus interrupts are wired
directly to the MPIC and, therefore, have preassigned system vector numbers
and interrupt levels.

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
The single debug port on the PrPMC800 board family is implemented in a
TLC16550 UART. The RJ-45 jack is placed on the front panel of the PrPMC Carrier
board and is configured as a DTE connection.

By default, the serial port is configured as asynchronous, 9600 baud, with
1 start bit, 8 data bits, 1 stop bit, no parity, and no hardware or software
handshake.  Hardware handshake using RTS/CTS is a supported option.

.SS "SCSI Configuration"
SCSI is not available on the PrPMC800 board family.

.SS "Harrier DMA Configuration"
To enable DMA support using the Harrier, change the #undef INCLUDE_HARRIER_DMA
in config.h to #define.  Instructions for configuring DMA Descriptor Lists are
contained in harrierDma.c and harrierDma.h.

.SS "Network Configuration"
The PrPMC800 has one Ethernet port which is 10baseT and 100baseTX
compatible using a RJ45 jack on the front panel for connection to this facility.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX.  For the PrPMC800 family, an Intel 82559 chip is used.  

The Media Access Control (Ethernet) address for each port is obtained from a
serial ROM connected to the Ethernet chip.  

The PrPMCBASE-001 has one Ethernet port which is 10baseT and 100baseTX
compatible using a RJ45 jack on the front panel for connection to this facility.

Support for the carrier boards DEC21143 exists in the BSP by defining
INCLUDE_DEC_END and INCLUDE_SECONDARY_ENET.

The Ethernet driver automatically senses and configures the port as 10baseT or
100baseTX.  The Ethernet driver is compatible with both DEC2104x and DEC2114x
devices.

The Media Access Control (Ethernet) address for each port is obtained from a
serial ROM connected to the DEC21143 chip.  

.SS "Configuring the PrPMC800 networking on the PrPMCBASE-001"
If the PrPMC800 is paired with the PrPMCBASE-001 carrier board, both
Ethernet devices will be auto-configured but the Ethernet driver will be
attached to the Ethernet device on the PrPMC800 ("fei0").  To use the
carrier board's DEC21143 as device name "dc1", then the following items
must be defined in config.h:

.CS
1. #define INCLUDE_DEC_END

2. #define INCLUDE_SECONDARY_ENET
.CE

If the PrPMC800 is depopulated without an Intel 82559 chip, then to use
the PrPMCBASE-001 carrier board's DEC21143 as the primary Ethernet connection
with the device name of "dc0", the following changes must occur in config.h:

.CS
1. #define INCLUDE_DEC_END

2. #undef  INCLUDE_SECONDARY_ENET
.CE

IMPORTANT NOTE:
The DEC21143 will only work on the Monarch and not the Slave.

.SS "Configuring the PrPMC800 networking on the PrPMC Carrier 1"
If the Primary Ethernet device on the PrPMC Carrier 1 board is the PrPMC800
onboard i82559 device, then PCI_CARRIER_1_PRI_BUS in config.h must be set to
0 (default).  Otherwise, if the Primary Ethernet device is one of the two
PrPMC Carrier 1 onboard i82559 devices, the value must be set to 1.
To access all three i82559 devices (including the one on-board the PrPMC800),
change config.h to:

1. #define  INCLUDE_TERTIARY_ENET

.SS "Configuring the PrPMC800 networking on the PrPMC-G"
The Primary Ethernet on the PrPMC-G is the 82543 Gigabit Ethernet.
The 82543 Gigabit Ethernet driver does require that the whole of local dram 
be mapped onto the pci. You will need to set PCI2DRAM_MAP_SIZE to the size of 
the dram, it is set to 64MB as default for a PrPMC_G board.

The Secondary Ethernet on the PrPMC-G is the PrPMC800 on-board i82559 Ethernet.
The 82543 Gigabit Ethernet Driver does not use the value of PCI_IDSEL_PRI_LAN,
so if INCLUDE_SECONDARY_ENET is defined, there are no other changes required
to be made for the on-board i82559 to be used as the Secondary Ethernet.


.SS "Configuring the PrPMC800 to use its own ethernet on the MCPN765"
Under the current VxWorks software configuration, a PrPMC800 that
is placed on an MCPN765 does not have access to its own i82559
Ethernet chip.  The MCPN765 will auto configure the Ethernet chip,
but the PrPMC800 will not be able to use it.  To enable this access, 
the MCPN765 and PrPMC800 BSPs must be changed as follows:

.CS
In the MCPN765 config.h file:

1. Change the value of PCI_MSTR_MEMIO_SIZE so it is AT LEAST twice the size
   of the PrPMC800's DRAM_SIZE.  It also must be large enough to handle
   any additional PCI devices that are auto configured after the PrPMC800
   so it may need to be larger than twice the size of the PrPMC800's
   DRAM_SIZE.

2. Search for INTERRUPT_ROUTING_TABLE and change the device number 18
   line in the table from:

   { 0xff,                 /* device number 18 PMC 2B Connector */ \
     0xff,\
     0xff,\
     0xff },\

   to:

   { PCI_INTB_VEC,         /* device number 18 PMC 2B Connector */ \
     0xff,\
     0xff,\
     0xff },\

3. Search for INIT_EXT_SRC9 and change PRIORITY_LVL3 to PRIORITY_LVL0.

In the PrPMC800 config.h file:

1. Search for SLAVE_OWNS_ETHERNET and #define it.

2. Change PCI_MSTR_MEMIO_SIZE to make sure that
   (DRAM_SIZE + PCI_MSTR_MEMIO_SIZE) on the MCPN765 equals
   (PCI_MSTR_MEMIO_LOCAL + PCI_MSTR_MEMIO_SIZE) on the PrPMC800.
.CE

.TS C
center;
l l .
\f3NOTE:\f1	T{
The MCPN765's PMC Site 2 (top slot) has the Request/Grant and IDSEL
lines for access to the PrPMC800 onboard Ethernet chip.  However, 
these lines are not available on PMC Site 1 of the MCPN765 board, 
so the MCPN765 cannot auto configure the PrPMC800's Ethernet device.
Therefore, the PrPMC800's Ethernet chip cannot be used for any
communication when the PrPMC800 is installed on PMC Site 1.
T}
.TE
After these changes are made, the bootrom and kernel software must be
rebuilt and installed for both BSPs.

.SS "Configuring the PrPMC800 to use its own ethernet on a PrPMCBase with a PrPMC750 Monarch"

.CS
In the PrPMC750 config.h file:

1. Change the value of PCI_MSTR_MEMIO_SIZE so it is AT LEAST twice the size
   of the PrPMC800's DRAM_SIZE.  It also must be large enough to handle
   any additional PCI devices that are auto configured after the PrPMC800
   so it may need to be larger than twice the size of the PrPMC800's
   DRAM_SIZE.

2. Search for INTERRUPT_ROUTING_TABLE and change the device number 18
   line in the table from:

   { PCI_INTC_VEC, /* device number 18 PMC 1 Alt Device */ \
     0xff,\
     0xff,\
     0xff },\

   to:

   { PCI_INTB_VEC, /* device number 18 PMC 1 Alt Device */ \
     0xff,\
     0xff,\
     0xff },\

3. Search for INIT_EXT_SRC11 and change PRIORITY_LVL7 to PRIORITY_LVL0.

In the PrPMC800 config.h file:

1. Search for SLAVE_OWNS_ETHERNET and #define it.

2. Change PCI_MSTR_MEMIO_SIZE to make sure that
   (DRAM_SIZE + PCI_MSTR_MEMIO_SIZE) on the PrPMC750 equals
   (PCI_MSTR_MEMIO_LOCAL + PCI_MSTR_MEMIO_SIZE) on the PrPMC800.

.CE
After these changes are made, the bootrom and kernel software must be
rebuilt and installed for both BSPs.







Sergey-->

.SS "Configuring the PrPMC800 to use its own ethernet on a PrPMCBase with a PrPMC800 Monarch"

.CS
In the PrPMC800 Monarch config.h file:

1. Search for SLAVE_OWNS_ETHERNET and #define it.

2. Change the value of PCI_MSTR_MEMIO_SIZE so it is AT LEAST twice the size
   of the PrPMC800 Slave's DRAM_SIZE.  It also must be large enough to handle
   any additional PCI devices on the PrPMCBase that are auto configured after
   the PrPMC800 Slave's Harrier, so it may need to be larger than twice the
   size of the PrPMC800's DRAM_SIZE.

In the PrPMC800 Slave config.h file:

1. Search for SLAVE_OWNS_ETHERNET and #define it.

2. Change PCI_MSTR_MEMIO_SIZE and PCI_MSTR_MEMIO_SIZE to have the
   same value on both Monarch and Slave.

.CE
After these changes are made, the bootrom and kernel software must be
rebuilt and installed for both BSPs.






.SS "PCI Access"
The 32-bit PCI bus is fully supported under the
.I PCI Local Bus Specification, Revision 2.1.
The 64-bit extensions are not supported.  All configuration space accesses
are made with BDF (bus number, device number, function number) format calls
in the pciConfigLib module.  For more information, refer to the man pages
.I prpmc800_xxpciXxx.


.SS "Using Customer-Designed Carrier Boards"
To use a customer-designed carrier board, the following changes must be made to
this BSP:

In config.h:
Modify the definition of INTERRUPT_ROUTING_TABLE to match the configuration
of your hardware. This table maps PCI IDSEL numbers to MPIC interrupt inputs.
The 4 columns in the table map INTA, INTB, INTC and INTD respectively as
viewed from the PrPMC site. The default table matches the MCG PrPMCBASE-001
Carrier board.

Modify the definitions of INIT_EXT_SRC0 through INIT_EXT_SRC15 to reflect
the desired MPIC interrupt priorities. To disable an interrupt, set it's
priority level to 0.

The values of PCI_MSTR_IO_SIZE, PCI_MSTR_MEMIO_SIZE and PCI_MSTR_MEM_SIZE may
also require modifications depending in the required PCI dynamic configuration
areas required.


.SS "Bootrom Errors"
Errors encountered during the early stages of the bootrom execution are saved
in the processor's general purpose registers as bit flags. Once the system is able to
report these errors, they are logged in the following form:

.CS
    Bootrom Error: Group = X, Code = 0xXXXXXXXX
.CE

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

.TS C
center;
l l .
\f3NOTE:\f1	T{
When multiple errors are present simultaneously, the error bits are OR'd
together.
T}
.TE

.SS "Boot Devices"
The supported boot devices are:

    \f3sm\f1	- shared memory
    \f3er\f1	- Primary Ethernet (10baseT or 100baseTX)
    \f3dc1\f1	- Secondary Ethernet (10baseT or 100baseTX)


Motorola's PPC-Bug can be used to download and run VxWorks.
Consult the relevant user's manuals for details.

.SS "Boot Methods"
The boot methods are affected by the boot parameters.  If no password is
specified, RSH (remote shell) protocol is used.  If a password is specified,
FTP protocol is used, or, if the flag is set, TFTP protocol is used.

These protocols are used for both Ethernet and shared memory boot devices.

.SS "ROM Considerations"
Use the following command sequence on the host to re-make the BSP boot ROM:
.CS
    cd target/config/prpmc800
    make clean
    make bootrom_uncmp.bin
    cp bootrom_uncmp.bin /tftpboot/bootrom_uncmp.bin
.CE

Power down the board and switch the ROM jumper to select socketed FLASH.
Connect the Ethernet and console serial port cables, then power the board back
up.

.SS "Flashing the Boot ROM Using Motorola PPC-Bug:" 1
Using `niot', the Client IP Address, Server IP Address, and Gateway IP Address
must be set up for the user's specific environment:

.CS
   PPC-Bug>niot
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
   PPC-Bug>
.CE

The file is transferred from the TFTP host to the target board using
the `niop' command.  Important: You must have a TFTP server running on your
host's subnet for the `niop' command to succeed.  The file name must be set to
the location of the binary file on the TFTP host.  The binary file must be
stored in the directory identified for TFTP accesses, but the file name is
a relative path and does not include the \f3/tftpboot\f1 directory name:

.CS
   PPC-Bug>niop
   Controller LUN =00?
   Device LUN     =00?
   Get/Put        =G?
   File Name      =? boot.bin
   Memory Address =00004000?
   Length         =00000000?
   Byte Offset    =00000000?

   PPC-Bug>
.CE

After the file is loaded onto the target, the `pflash' command is used
to put it into Bank A (soldered FLASH) parts.

.CS
   PPC-Bug>pflash 4000:fff00 f0000100
.CE

When the command is finished, power down the board and switch the ROM
jumper to select Bank A (soldered FLASH).  Then power the board back up.

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
The PrPMC800 does not contain an on-board oscillator for generating the 
processor bus clock. The processor bus clock is generated using a PLL which 
multiplies the system PCI bus frequency up to create the CPU bus frequency. 
The maximum PPC bus frequency (66 Mhz or 100 Mhz) is determined at module 
assembly time by populating the appropriate select resistors. The 100Mhz bus 
mode will be the standard configuration, but the 66Mhz bus mode may be 
available for low power configurations. The bus speed value (in Hertz) may be 
accessed using the macro MEMORY_BUS_SPEED which is defined in prpmc800.h.

.SS "TFFS support"
With TFFS included into your system, you will need to copy the cfiscs.c 
driver to the BSP from target/src/drv/tffs/. The file needs to be modified 
by defining BUFFER_WRITE_BROKEN. You must recompile the file 
eg make cfiscs.o. Then add this object to those pulled in via the Makefile 
using MACH_EXTRA.

.SH "BOARD LAYOUT"
The diagrams below show flash EEPROM locations and jumpers relevant to VxWorks
configuration:

For the PrPMC800EXT, the 10baseT/100baseTTX port appears through the front
panel opening reserved for PMC slot 1 when installed on the PrPMCBASE-001.

.ne 4i

.bS

              PrPMC800
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
| +-----+                           |
| |XU1	|                           |
| |     |                           |
| +-----+                           |
| +-----+                           |
| |XU2  |                           |
| |     |                           |
| +-----+           _               |
|                  | |-             |
|           Serial | |-             |
|           Debug  | |-             |
|           Port   | |-             |
|                  | |-             |
|                  |_|-             |
|                                   |
|_________________________----______|
                         10/100
                         base T
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
.iB "Motorola PrPMC800 Processor PMC Programmer's Reference Guide,"
.iB "Motorola Computer Group Online Documentation,
.iB  http://library.mcg.mot.com/mcg/boards"
.iB "Motorola PowerPC 60X RISC Microprocessor User's Manual,"
.iB "Motorola PowerPC Microprocessor Family: The Programming Environments,"
.iB "DECchip 21143 PCI Fast Ethernet LAN Controller Hardware Reference Manual,"
.iB "IEEE P1386.X Draft 0.5 - Processor Mezzanine Specification (PrPMC),"
.iB "IEEE P1386.1 Draft 2.0 - PCI Mezzanine Card Specification (PMC),"
.iB "IEEE P1386 Draft 2.0 - Common Mezzanine Card Specification (CMC),"
.iB "Peripheral Component Interconnect (PCI) Local Bus Specification, Rev 2.1,"
.iB "PCI to PCI Bridge Architecture Specification 2.0,"
.iB "PICMG 2.0 D2.14 CompactPCI Specification,"
.iB "Digital Semiconductor 2155x PCI-to-PCI Bridge for Embedded Applications Hardware Reference Manual,"






