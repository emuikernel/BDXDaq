/* ncr710init.c - script programs for the NCR 710 */

/* Copyright 1989-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/* 
Modification history
--------------------
03f,20jul92,eve    Added documentation. 
03e,20jul92,eve    Rename end label to endTransact.
03d,03jul92,eve    Include NCR_COMPILER_BUG in the file.
03c,04mar92,eve    Start disconnect implementation.
03b,16jan92,eve    Add ackMsg1 ,ackAtnMsg and endAbort, remove 
		    script test proc.
03a,30oct91,eve    Remove int capability in end label,the scsi 
		    status is process by the ncr710Lib driver.
02a,26oct91,eve    Add enable disable timout between selection
		    step.
01a,23oct91,eve    Created and add comments
*/

/*
INTERNAL
NCR710 FEATURE USED
This chip has code which performs some action on the scsi bus.
The description level of the code is close to an assembly language and 
dedicated for scsi bus operations. 
The opcode is a pair of 32bit word that allow some basic operations in the 
code (jump, tests values) and on the scsi (select ,assert line):

block move instruction.
  move from <offset> when PHASE_NAME
  ........
I/O instructions 
  set target 
  wait DISCONNECT
  wait RESELECT
  select from <offset>,@jump
  .........  
read/write register instructions
  move REG_NAME to SBFR
  .........  
 
transfert control instructions
  jump <Label> 
  int <value> when PHASE_NAME
  ........... 
  move memory instruction
  never use.  
 
The previous example does not included all of the instruction set, 
see the NCR data reference manual.
Another key point is the capability of the script to perform some 
limited indirect addressing to get information from the cpu memory.
This is used to get Target ID to select it, to get data, message pointer
and count during transfer phase.  This avoids having to patch the script
at the run time to specify count, data pointer and target ID.

Script can tranfer directly, with its DMA core, a data block from/to the 
scsi bus from/to cpu memory.
The instruction involved is :
        move from <offset> when PHASE_NAME
 
In this case the offset is hard coded in the ncr710Script.h and indexed the
right field in the NCR_CTL structure (see #include"drv/scsi/ncr710Script1.h"). 
 
The field looks like :
    aligned 32 bit @->00xxxxxx  24bit count
                    ->ZZZZZZZZ  32bit address pointer.
 
The offset is added to the content of the DNAD register, loaded at 
the run time with the base address of the structure.
 
In the same way the select instruction uses a field like:
        select from <offset> ,<Alternate jump @>

 where offset + DNAD points to a field like:
aligned 32 bit@->0000|0000|xxxx xxxx|xPPP  |SSSS|0000|0000
                           Target ID|Period|Offset

The Period and offset are used when a target supports the synchronous 
transfert.

INTERRUPT SOURCE
The chip has three main kind of interrupt, scsi, dma interrupt and 
script  interrupt.  The scsi interrupts are maskable individually with 
the sien register.
.CS
SIEN
  7    M/A Mismatch interrupt(initiator)/ ATN assert (Target)
       Used in the driver to detect a phase change during data/msg
       transfert.The ncr is never used in target mode.
  6    FCMP function complete Not used (masked).
  5    STO scsi Bus Time out
       Used to get timeout on select.(**)
  4    SEL selected or reselected Not used in the driver.(masked).
  3    SGE scsi Gross error. Used in the driver.
  2    UDC unexpected disconnect.
       Used to detect unexpected target disconnection or an expected 
       disconnect if the target received a "device reset message".
  1    RST scsi reset received (not used).(masked)(***)
  0    PAR parity error not used.(masked)(***)

(**)    The script disable the timeout capability by setting the bit 7 
        in CTEST7 register.  Because the ncr will generate also a timeout
        interrupt  if no activity occur on the scsi bus after 250ms,
        and not only during a scsi select.

(***)  This case should be processed, but not included in the driver today.
.CE

The Dma interrupts are maskable with the DIEN register:
.CS
DIEN
  7   Reserved
  6   Reserved
  5   BF bus fault.Memory acces error.
  4   Aborted ,means that the abort bit have been set in ISTAT reg
      This case never happen (Abort bit not used).
  3   SSI single step interrupt.Not used in the operational mode.
      This bit is used in single step debug mode in the driver,
      allowing to execute script step by step..
  2   SIR script interrupt,used to detect a script intruction
      <int ...>.
  1   WTD Watchdog interrupt.This case never happens because
      The watchdog timeout value is set to 0 in DWT register.
  0   IID illegal instruction detected.  Recieved in two
      cases :1-Bad opcode, 2-count set to 0 in the <move> opcode.
.CE

The other register involved  in  the  interrupt  process  is ISTAT.
This register is the ONLY READABLE/WRITABLE  register during a script 
execution.
.CS
 ISTAT
 7   ABRT Abort current instruction.Not used
 6   RST  software reset used to set the chip to a known state.
 5   SIGP signal process bit.This bit is used in the ncr driver to support
     the connect disconnect to get an interrupt to start a new scsi 
     command.(SEE CONNECT/DISCONNECT ...).
 4   Reserved.
 3   Connected ,not used.
 2   Reserved.
 1   SIP scsi interrupt pending.Used at interrupt
     level to detect and clear SIEN interrupt.
 0   DIP dma interrupt pending.Used at interrupt
     level to detect and clear DIEN interrupt.
.CE
The interrupt are cleared by reading the register source, and by 
checking  the DIP and SIP bit in ISTAT.  Because the chip has a stack 
interrupt ,the DIEN and SIEN registers are read until the ISTAT bits 
are cleared to be sure there is no interrupt pending.

The last interrupt source is the script <INT> opcode. 
This instruction is  used to interrupt the cpu if the scsi 
transaction involves a cpu process.
.CS
    int <int value> [<condition><value>] (pg 6-10 of the programmers guide).
.CE
The <int value> is used in the driver code to detect the scsi condition  
and takes a decision for the next step of the transaction.  
This value is loaded from the opcode and read by the  CPU in the 
DSPS register.

CONNECT/DISCONNECT AND NEW COMMAND START

When connect/disconnect is enabled, the target could reconnect at any time.
This is notified to the target by an identify message send by the initiator
after a successful selection of the target.
The  disconnect is always sent to an initiator by a DISCONNECT message.
The reconnect could occur at any arbitration phase.  
To be able to detect it at any time the chip has to wait for a reconnect 
on the scsi ,which is done with the <wait reconnect> script opcode:
  wait RECONNECT,<Alternate @>
  <next opcode>
.CS
  If a reconnect occurs, then the <next opcode>
  is executed.If the SIGP bit is set the script
  jump to <Alternate @>
.CE

But a reconnect could also occurs during a  <select>  opcode for  another  
device (it looses the arbitration).  The <wait RECONNECT> instruction is 
the idle script entry point.  They are two way to abort this instruction:

  1-The Sigp bit is set
  2-A reconnect,or a select occur ,in this case
    select means that the initiator is selected as
    a target on the scsi bus

The sigp bit is used to detect and start a new command at interrupt level. 
It is set at task level each time that a new command has to be run.


.CS
SCRIPT START FLOW CHART
The reselected path is driven by a physical scsi reconnect/select.

IDLE ENTRY POINT
<waitSelect>
Wait for a reselect ,Alternate @-------->
and test sigp ?                         | SIGP set or Select to start
  |                                     |
  |<-reselect                           /
  |                       No    <checkNewCmd>
  |                     /-------Check if we are connected ?
<reselect>              |       clear SIGP
save Targ/init ID       |       Wait for reselect,<@select as a target>
from LCRC register      |       Jump to <reselect>      |
Get identify            |                               |
Msg in.                 <startNewCmd>                   |
int Cpu <reselected>    test and clear SIGP     int Cpu <SELECT AS A TARGET>
                        1) SIGP set
                        int Cpu <NEW COMMAND>
                        2) SIGP clear
                        int Cpu <BAD NEW COMD>
.CE
a) RESELECT
At interrupt level, the target ID from the saved LCRC register (LCRC contain
the data bus value that is <the target ID> | <initiator ID>) and the LUN 
extracted from the identify message sent by the target are used to index 
an array that keep track of each PHYS_DEV nexus (Max = 64) in the  SIOP  
structure (driver structure).

b) NEW COMMAND
A global variable <pNcrCtlCmd> in the SIOP structure  allows the script to be 
start at a selected entry point with the script address included in this 
current nexus pointer in pNcrCtlCmd.

c)SELECT AS A TARGET
That an error case (we dont support the target  mode)  ,restart script at 
the IDLE point.

d)BAD NEW CMD
It's a bogus start command restart script at the IDLE Point.

SCRIPT REMARKS
This script performs a scsi transaction.  This script is a part of 
the usual scsi phase routine.  This code is only compatible with the 
ncr710 because it uses an indirect addressing mode to avoid relocation 
at run time.
All of the external values will be relative to the DSA register.
DSA register has to be loaded whith the address of the ncrCtrl host memory
structure (see #include"drv/scsi/ncr710Script1.h").
Scratch3 register is used to save the LCRC register. LCRC is stamped with
the initiator ID and target ID when a reconnect (initiator is selected
by a target) occurs.
SCRATCH0 is used to hold phase requested to be able to process a phase 
mismatch during any data or message phase.  Usually that occur before a 
legal disconnect from the target (save pointer and disconnect message).
 */

#define NCR_COMPILER_BUG
#include"drv/scsi/ncr710Script1.h"

extern UINT waitSelect[];
extern UINT reselect[];
extern UINT checkNewCmd[];
extern UINT startNewCmd[];
extern UINT selWithAtn[];
extern UINT selWithoutAtn[];
extern UINT contTransact[];
extern UINT endTransact[];
extern UINT msgOut1[];
extern UINT msgOut3[];
extern UINT outputData[];
extern UINT checkOut[];
extern UINT inputData[];
extern UINT msg1[];
extern UINT msg2[];
extern UINT msg3[];
extern UINT selectAddr[];
extern UINT ackMsg1[];
extern UINT ackMsg3[];
extern UINT checkPhData[];
extern UINT ackAtnMsg[];
extern UINT asortPh[];
extern UINT endAbort[];
extern UINT reserOutPh[];
extern UINT reserInPh[];
extern UINT reselSelect[];
extern UINT last_datap[];
UINT last_datap_idx EQ 0;
UINT waitSelect_idx EQ 1;
UINT reselect_idx EQ 2;
UINT checkNewCmd_idx EQ 3;
UINT startNewCmd_idx EQ 4;
UINT reselSelect_idx EQ 5;
UINT selWithAtn_idx EQ 6;
UINT selWithoutAtn_idx EQ 7;
UINT contTransact_idx EQ 8;
UINT endTransact_idx EQ 9;
UINT msgOut1_idx EQ 10;
UINT msgOut3_idx EQ 11;
UINT outputData_idx EQ 12;
UINT checkOut_idx EQ 13;
UINT inputData_idx EQ 14;
UINT msg1_idx EQ 15;
UINT msg2_idx EQ 16;
UINT msg3_idx EQ 17;
UINT selectAddr_idx EQ 18;
UINT ackMsg1_idx EQ 19;
UINT ackMsg3_idx EQ 20;
UINT checkPhData_idx EQ 21;
UINT ackAtnMsg_idx EQ 22;
UINT asortPh_idx EQ 23;
UINT reserOutPh_idx EQ 24;
UINT reserInPh_idx EQ 25;
UINT endAbort_idx EQ 26;
UINT script_ptr_sz EQ 108; 
UINT *script_ptr[] EQ {
last_datap,
waitSelect,
reselect,
checkNewCmd,
startNewCmd,
reselSelect,
selWithAtn,
selWithoutAtn,
contTransact,
endTransact,
msgOut1,
msgOut3,
outputData,
checkOut,
inputData,
msg1,
msg2,
msg3,
selectAddr,
ackMsg1,
ackMsg3,
checkPhData,
ackAtnMsg,
asortPh,
reserOutPh,
reserInPh,
endAbort,
};

ULONG	last_datap[] = {
	0x98080000,	0x00000000

};

ULONG	waitSelect[] = {
	0x7A1B1000,	0x00000000,
	0x78340A00,	0x00000000,
	0x50000000,	(UINT)checkNewCmd,
	0x80080000,	(UINT)reselect

};

ULONG	reselect[] = {
	0x60000200,	0x00000000,
	0x72230000,	0x00000000,
	0x6A370000,	0x00000000,
	0x1F000000,	     0x34 ,
	0x98080000,	RECONNECT_PROCESS

};

ULONG	checkNewCmd[] = {
	0x74011000,	0x00000000,
	0x800C0000,	(UINT)startNewCmd,
	0x74162000,	0x00000000,
	0x50000000,	(UINT)selectAddr,
	0x80080000,	(UINT)reselect

};

ULONG	startNewCmd[] = {
	0x74162000,	0x00000000,
	0x80840000,	0x00000010,
	0x98080000,	BAD_NEW_CMD,
	0x80080000,	(UINT)waitSelect,
	0x98080000,	NEW_COMMAND_PROCESS

};

ULONG	reselSelect[] = {
	0x98080000,	RECONNECT_IN_SELECT

};

ULONG	selWithAtn[] = {
	0x7C1BEF00,	0x00000000,
	0x78340A00,	0x00000000,
	0x43000000,	(UINT)checkNewCmd,
	0x830B0000,	(UINT)endTransact,
	0x860B0000,	(UINT) msgOut1,
	0x9E020000,	MSGOUT_EXPECT

};

ULONG	selWithoutAtn[] = {
	0x7C1BEF00,	0x00000000,
	0x78340A00,	0x00000000,
	0x42000000,	(UINT)checkNewCmd,
	0x80080000,	(UINT) contTransact

};

ULONG	contTransact[] = {
	0x830B0000,	(UINT)endTransact,
	0x860B0000,	(UINT)msgOut1,
	0x830B0000,	(UINT)endTransact,
	0x870B0000,	(UINT)msg1,
	0x9A020000,	BAD_PH_BEFORE_CMD,
	0x7A1B1000,	0x00000000,
	0x78340200,	0x00000000,
	0x1A000002,	0x1C ,
	0x870B0000,	(UINT)msg2,
	0x830A0000,	(UINT)endTransact,
	0x810A0000,	(UINT)inputData,
	0x800A0000,	(UINT)outputData,
	0x98080000,	BAD_PH_AFTER_CMD

};

ULONG	endTransact[] = {
	0x7C1BEF00,	0x00000000,
	0x78340300,	0x00000000,
	0x1B000003,	    0x2C ,
	0x9F030000,	MSGIN_EXPECT_AFTER_ST,
	0x78340700,	0x00000000,
	0x1F000007,	0x0C ,
	0x98040000,	BAD_MSG_INSTEAD_CMDCOMP,
	0x60000040,	0x00000000,
	0x7C1BEF00,	0x00000000,
	0x48000000,	0x00000000,
	0x98080000,	GOOD_END

};

ULONG	msgOut1[] = {
	0x78340600,	0x00000000,
	0x1E000006,	0x04 ,
	0x820B0000,	(UINT)contTransact,
	0x830A0000,	(UINT)contTransact,
	0x810A0000,	(UINT)inputData,
	0x800A0000,	(UINT)outputData,
	0x870A0000,	(UINT)msg1,
	0x98080000,	PH_UNKNOWN

};

ULONG	msgOut3[] = {
	0x78340600,	0x00000000,
	0x1E000006,	0x04 ,
	0x820B0000,	(UINT)contTransact,
	0x830A0000,	(UINT)contTransact,
	0x810A0000,	(UINT)inputData,
	0x800A0000,	(UINT)outputData,
	0x870A0000,	(UINT)msg3,
	0x98080000,	PH_UNKNOWN

};

ULONG	outputData[] = {
	0x78340000,	0x00000000,
	0x18000000,	0x24 

};

ULONG	checkOut[] = {
	0x830B0000,	(UINT)endTransact,
	0x870B0000,	(UINT)msg3,
	0x80080000,	(UINT) asortPh

};

ULONG	inputData[] = {
	0x78340100,	0x00000000,
	0x19000001,	0x24 ,
	0x80080000,	(UINT)checkOut

};

ULONG	msg1[] = {
	0x78340700,	0x00000000,
	0x1F000007,	0x0C ,
	0x808C0001,	0x00000028,
	0x980C0002,	SAVDATP_BEFORE_CMD,
	0x980C0003,	RESTORE_POINTER,
	0x808C0004,	0x00000028,
	0x980C0007,	REJECT_MSG1,
	0x98080000,	BAD_MSGIN_BEFORE_CMD,
	0x60000040,	0x00000000,
	0x1F000000,	0x14 ,
	0x98080000,	EXTMSG_BEFORE_CMD,
	0x60000040,	0x00000000,
	0x48000000,	0x00000000,
	0x98080000,	DISC_BEFORE_CMD

};

ULONG	msg2[] = {
	0x78340700,	0x00000000,
	0x1F000007,	0x0C ,
	0x808C0001,	0x00000020,
	0x980C0002,	SAVDATP_AFTER_CMD,
	0x980C0003,	RESTORE_POINTER,
	0x808C0004,	0x00000020,
	0x98080000,	BAD_MSG_AFTER_CMD,
	0x60000040,	0x00000000,
	0x1F000000,	0x14 ,
	0x98080000,	EXTMSG_AFTER_CMD,
	0x60000040,	0x00000000,
	0x48000000,	0x00000000,
	0x98080000,	DISC_AFTER_CMD

};

ULONG	msg3[] = {
	0x78340700,	0x00000000,
	0x1F000007,	0x0C ,
	0x808C0001,	0x00000020,
	0x980C0002,	SAVDATP_AFTER_DATA,
	0x980C0003,	RESTORE_POINTER,
	0x808C0004,	0x00000020,
	0x98080000,	BAD_MSG_AFTER_DATA,
	0x60000040,	0x00000000,
	0x1F000000,	0x14 ,
	0x98080000,	EXTMSG_AFTER_DATA,
	0x60000040,	0x00000000,
	0x48000000,	0x00000000,
	0x98080000,	DISC_AFTER_DATA

};

ULONG	selectAddr[] = {
	0x98080000,	SELECT_AS_TARGET

};

ULONG	ackMsg1[] = {
	0x60000040,	0x00000000,
	0x870B0000,	(UINT)msg1,
	0x80080000,	(UINT) asortPh

};

ULONG	ackMsg3[] = {
	0x60000040,	0x00000000,
	0x870B0000,	(UINT)msg3,
	0x80080000,	(UINT) checkPhData

};

ULONG	checkPhData[] = {
	0x810B0000,	(UINT)inputData,
	0x800A0000,	(UINT)outputData,
	0x860A0000,	(UINT)msgOut3,
	0x830A0000,	(UINT)endTransact,
	0x820A0000,	(UINT) contTransact,
	0x870A0000,	(UINT)msg3,
	0x840A0000,	(UINT)reserOutPh,
	0x850A0000,	(UINT)reserInPh,
	0x98080000,	PH_UNKNOWN

};

ULONG	ackAtnMsg[] = {
	0x58000008,	0x00000000,
	0x60000040,	0x00000000,
	0x78340600,	0x00000000,
	0x1E000006,	0x04 ,
	0x86030000,	(UINT) asortPh,
	0x98080000,	PH_UNKNOWN

};

ULONG	asortPh[] = {
	0x60000008,	0x00000000,
	0x810B0000,	(UINT)inputData,
	0x800A0000,	(UINT)outputData,
	0x860A0000,	(UINT)msgOut1,
	0x830A0000,	(UINT)endTransact,
	0x820A0000,	(UINT) contTransact,
	0x870A0000,	(UINT)msg1,
	0x840A0000,	(UINT)reserOutPh,
	0x850A0000,	(UINT)reserInPh,
	0x98080000,	PH_UNKNOWN

};

ULONG	reserOutPh[] = {
	0x78340400,	0x00000000,
	0x98080000,	RES_OUT_DETECTED

};

ULONG	reserInPh[] = {
	0x78340500,	0x00000000,
	0x98080000,	RES_IN_DETECTED

};

ULONG	endAbort[] = {
	0x58000008,	0x00000000,
	0x60000040,	0x00000000,
	0x78340600,	0x00000000,
	0x1E000006,	0x04 ,
	0x48000000,	0x00000000,
	0x98080000,	ABORT_CLEAR_END

};

ULONG	relocation[] = {
	0x98080000,	(UINT)last_datap,
	0x0F000001,	sizeof(last_datap),
	0x98080000,	(UINT)waitSelect,
	0x0F000001,	sizeof(waitSelect),
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x98080000,	(UINT)reselect,
	0x0F000001,	sizeof(reselect),
	0x80080000,	0x0000001C,
	0x98080000,	(UINT)checkNewCmd,
	0x0F000001,	sizeof(checkNewCmd),
	0x80080000,	0x0000000C,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x98080000,	(UINT)startNewCmd,
	0x0F000001,	sizeof(startNewCmd),
	0x80080000,	0x0000001C,
	0x98080000,	(UINT)reselSelect,
	0x0F000001,	sizeof(reselSelect),
	0x98080000,	(UINT)selWithAtn,
	0x0F000001,	sizeof(selWithAtn),
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x98080000,	(UINT)selWithoutAtn,
	0x0F000001,	sizeof(selWithoutAtn),
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x98080000,	(UINT)contTransact,
	0x0F000001,	sizeof(contTransact),
	0x80080000,	0x00000004,
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x0000003C,
	0x80080000,	0x00000044,
	0x80080000,	0x0000004C,
	0x80080000,	0x00000054,
	0x80080000,	0x0000005C,
	0x98080000,	(UINT)endTransact,
	0x0F000001,	sizeof(endTransact),
	0x80080000,	0x00000014,
	0x80080000,	0x0000002C,
	0x98080000,	(UINT)msgOut1,
	0x0F000001,	sizeof(msgOut1),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x80080000,	0x0000002C,
	0x80080000,	0x00000034,
	0x98080000,	(UINT)msgOut3,
	0x0F000001,	sizeof(msgOut3),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x80080000,	0x0000002C,
	0x80080000,	0x00000034,
	0x98080000,	(UINT)outputData,
	0x0F000001,	sizeof(outputData),
	0x80080000,	0x0000000C,
	0x98080000,	(UINT)checkOut,
	0x0F000001,	sizeof(checkOut),
	0x80080000,	0x00000004,
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x98080000,	(UINT)inputData,
	0x0F000001,	sizeof(inputData),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x98080000,	(UINT)msg1,
	0x0F000001,	sizeof(msg1),
	0x80080000,	0x0000000C,
	0x80080000,	0x0000004C,
	0x98080000,	(UINT)msg2,
	0x0F000001,	sizeof(msg2),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000044,
	0x98080000,	(UINT)msg3,
	0x0F000001,	sizeof(msg3),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000044,
	0x98080000,	(UINT)selectAddr,
	0x0F000001,	sizeof(selectAddr),
	0x98080000,	(UINT)ackMsg1,
	0x0F000001,	sizeof(ackMsg1),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x98080000,	(UINT)ackMsg3,
	0x0F000001,	sizeof(ackMsg3),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x98080000,	(UINT)checkPhData,
	0x0F000001,	sizeof(checkPhData),
	0x80080000,	0x00000004,
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x80080000,	0x0000002C,
	0x80080000,	0x00000034,
	0x80080000,	0x0000003C,
	0x98080000,	(UINT)ackAtnMsg,
	0x0F000001,	sizeof(ackAtnMsg),
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x98080000,	(UINT)asortPh,
	0x0F000001,	sizeof(asortPh),
	0x80080000,	0x0000000C,
	0x80080000,	0x00000014,
	0x80080000,	0x0000001C,
	0x80080000,	0x00000024,
	0x80080000,	0x0000002C,
	0x80080000,	0x00000034,
	0x80080000,	0x0000003C,
	0x80080000,	0x00000044,
	0x98080000,	(UINT)reserOutPh,
	0x0F000001,	sizeof(reserOutPh),
	0x98080000,	(UINT)reserInPh,
	0x0F000001,	sizeof(reserInPh),
	0x98080000,	(UINT)endAbort,
	0x0F000001,	sizeof(endAbort),
	0x80080000,	0x0000001C,
	0x60000040,	0x00000000

};

