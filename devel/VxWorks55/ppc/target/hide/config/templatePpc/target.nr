'\" t
.so wrs.an
.\" <bspname>/target.nr - <bspname> target specific documentation
.\"
.\" Copyright 1984-1997 Wind River Systems, Inc.
.\"
.\" modification history
.\" --------------------
.\" TODO -  Remove the template modification history and begin a new history
.\"         starting with version 01a and growing the history upward with
.\"         each revision.
.\" 
.\" 01f,28aug01,dgp  change manual pages to reference entries per SPR 23698
.\" 01e,13jun97,sbs  added quotes for bspname on the title line for refgen
.\"                  (SPR #29106).
.\" 01e,13jun97,dat  more suggestions.
.\" 01d,11jun97,dat  added information from tech support suggestions
.\" 01c,11mar97,dat  comments from reviewers
.\" 01b,23jan97,dat  added more notes about documentation
.\" 01a,16dec96,ms   derived from mv162
.\"
.\" TODO - This entire document is an example.  Update and replace all
.\" documentation as needed. Replace all references to "bspname" with
.\" the actual BSP name.  Similarly replace all references to Vendor and
.\" Model. Information within brackets [ and ], are instructions to the
.\" writer of the new document.  Information outside of brackets is sample
.\" information.
.\"
.TH "bspname" T "Vendor and Model" "Rev: 11 Jun 97" "TORNADO REFERENCE: VXWORKS"

.SH "NAME"
.aX "Vendor and Model"

.SH "INTRODUCTION"
This reference entry provides board-specific information necessary to run
VxWorks for the template BSP. Before using a board with VxWorks,
verify that the board runs in the factory configuration by using
vendor-supplied ROMs and jumper settings and checking the RS-232
connection.

[All sections of this document are required.  If a section does not
apply then say 'Does not apply.']

.SS "Boot ROMs"
[Describe the type and size of the boot ROMs delivered with the BSP, if any.]

Install VxWorks boot ROMs as follows:

.TS
center;
lf3 lf3
a a .
.sp .25
ROM Socket
\_
even	U1
odd	U2
.TE

[Does the BSP support non-volatile RAM? If not, the boot parameters
are not preserved whenever the system is powered off.  This will force
the user to re-enter them each time, or to build a boot ROM with the 
DEFAULT_BOOT_LINE defined specifically for the one target.]

To load VxWorks, and for more information, follow the instructions in the
.I "Tornado User's Guide: Getting Started."

.SS "Jumpers"
[Describe the relevent jumper settings for the board.  Only document those
items that relate to VxWorks.  Usually this is just the EPROM size
selection to match the delivered boot ROMs, and for VME boards, the jumper
controlling system controller functions.]

.TS E
expand;
cf3 s s
lf3 lf3 lf3
l l lw(3i) .
.ne 5
Vendor and Model
.sp .25
Jumper	Function	Description
_
J1	Function of J1	T{
Description of jumper J1
T}
J2	Function of J2	T{
Description of jumper J2
T}
.TE
.PP
For details of jumper configuration, see the board diagram at the end
of this entry.  Also see the board's hardware manual for other jumpers
not directly related to VxWorks operation.

.SH "FEATURES"
This section describes all features of the board, supported or not.
It documents all configurations of the board and the interaction between
features and configuration items.

.SS "Supported Features"
[Describe all the features of the board supported by this BSP.  Together
with the list of unsupported features every feature of the board should
be identified.  Refer to the manufacturer's information to obtain a list
of features for the board.  Insure that all features are described as
being either supported or unsupported.]

[In cases where the board has optional features identify each option
that is supported.  Do not forget to identify the supported memory options
on a board.  Sometimes manufacturers change memory options and memory
speeds and the BSP may need to be modified to support the new features.
Do not say 'supports all memory options'.  Identify each supported 
option individually; i.e. 'supports 4MB, 16MB, and 32MB memory options'.]

[All supported features are presumed to have been tested.  If a supported
feature has not been tested by the BSP writer, that should be identified.]

.SS "Unsupported Features"
[Describe all board features that are not supported by the BSP.  With
the supported features described above, all board features and configurations
should be identified in this documentation.]

.SS "Feature Interactions"
[Identify any areas where one feature or configuration of the board
impacts other features.  An example would be a board that has a very
large memory option.  When the large memory option is installed, the
VME A24 memory space might be inaccessible and becomes unsupported.]

.SH "HARDWARE DETAILS"
This section documents the details of the device drivers and board
hardware elements.

.SS "Devices"
The chip drivers included are:

    templateSio.c - template serial driver
    templateTimer.c - template timer driver
    templateVme.c - template VMEbus driver
    templateNvram.c - template non-volatile RAM driver

[Describe the device drivers for this board.  Are there special driver
options that must be used with this board?  Read the specific driver
documentation and describe any special options used with this board.]

.SS "Memory Maps"
[Provide a detailed memory map of the local bus, and all additional 
busses from a bus master point of view.  Identify only slave devices 
actually on each bus, not their apparent address on some other bus.  (i.e. 
if a serial device is a PCI device, do not list it on the local bus memory 
map, just the PCI memory map).  Identify all addresses that the user can 
adjust by changing config.h, as being software controllable.  Identify all 
bus master devices (DMA) on each bus.]

.SS "Shared Memory"
[If appropriate, identify the main bus and the type of shared memory 
support provided, if any.   Identify the type of mailbox support and 
Test-And-Set support used.  Identify any special support, or 
consideration, necessary from other boards in a multi-board system.]

.SS "Interrupts"
[Provide a list of interrupts/exceptions by priority and/or vector 
number.  Describe any special interrupt connection routines or 
considerations used with this BSP.]

.SS "Serial Configuration"
[Identify the default configuration for all serial ports.  Document any 
special considerations: i.e. hardware flow control cannot be disabled thus 
requiring a  jumper to be installed when no terminal device is connected.]

.SS "SCSI Configuration"
[Are there any known SCSI configuration limitations?  Does the BSP 
support SCSI-1 or SCSI-2 ?  Describe any special connectors or adaptors.  
Does the board include terminators on-board?  Is there a jumper to 
activate the termination network?]

.SS "Network Configuration"
[Describe the Ethernet ports on the board.
Identify how the MAC address is obtained 
or specified for each port.]

.SS "VME Access"
[If VME is present, describe the default parameters for the master windows
and how to change them, if possible. (master access means accessing some
other board from this board).]

Describe the default parameters for the slave access windows, and how to
change them, if possible. (Slave access means an access from another bus
master).
The normal VxWorks default is to enable the slave access window(s) only on
CPU 0, as part of the routine sysProcNumSet().  Otherwise, slave accesses
are not normally permitted.]

.SS "PCI Access"
[If PCI is present, describe the configuration access methods used to
access PCI configuration space.  Describe any address mapping from local
addresses to PCI addresses and vice versa.  Describe how the user can
alter this, if appropriate.]

.SS "Boot Devices"
[Describe the methods and devices available for booting VxWorks on this 
board. (sm, SCSI, Ethernet, vendor-bug  ROMs, open firmware, etc).]

.SS "Boot Methods"
[Describe the network support available for booting; bootp, proxy-arp, 
ftp, tftp, etc.]

.SS "ROM Considerations"
[How are ROMs made and installed? (How do they split?)  Can FLASH 
ROMs be burned on board? If so, how?]

.SH "SPECIAL CONSIDERATIONS"
This section describes miscellaneous information that the user needs
to know about the BSP.

.SS "Delivered Objects"
[Which pre-compiled objects are delivered with the BSP?]

.SS "Make Targets"
[If there is any special information about a specific make target, then
it must be explained.
Identify any standard targets that do not build or run correctly.
(bootrom, bootrom_uncmp, bootrom.res_rom, vxWorks, vxWorks.st,
vxWorks.res_rom, vxWorks.res_rom_nosym, vxWorks_rom).]

.SS "Special Routines"
[Are there any unique BSP routines that a user might use?]

.SS "Known Problems"
[Is there a Known Problem List?  Any deviations or exceptions from 
published standards, including Wind Tech Notes, should be listed as known
problems.  Are there any special issues relating to cache and/or snooping?]

.SH "BOARD LAYOUT"
The diagram below shows jumpers relevant to VxWorks configuration.

.bS
______________________________              _______________________________
|             P1             | Board Model  |             P2              |
|                            ----------------                      ____   |
|                                                                 |    |  |
|                                                          pin 1  \e____|  |
|                                                                  U38    |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|  X - JUMPER J1                                                          |
|                                                                         |
|_________________________________________________________________________|
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
.pG "Configuration," 
.pG "Architecture Appendix"

.SH "BIBLIOGRAPHY"
[Provide a bibliography of all manuals and data sheets relevent to the 
board. For example:]

.iB "Intel i960RP Microprocessor User's Manual"
