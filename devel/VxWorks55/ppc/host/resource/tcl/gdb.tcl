# gdb.tcl - gdb specific Tcl commands loaded on gdb startup
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 03c,04mar03,tpw  Partial fix for SPR 86310, rename PPC 'fpcr' to 'fpscr'.
# 03b,28jan03,tpw  Merge proc gdbIgnoreVioWriteEvents from SPR 82648 fix branch
# 03a,10jan03,tpw  Workaround SPR 6887 et al., spaces in task names crash GDB.
# 02z,30dec02,tpw  Fix SPR 85389, incorrect offsets for simso regs %i0 and %i4
# 02y,07may02,tpw  Correct ARM register list so the index of 'ps' is correct.
# 02x,07may02,jab  sync coldfire sr/pc with new reg_set layout
# 02w,05apr02,tpw  Use wtxPath, not WIND_BASE, to find .wind/gdb.tcl
# 02v,27feb02,tpw  Add transitional support for by-name indexing of gdbRegs.
# 02u,21sep01,tpw  Adjust Altivec register list to better match the compiler.
# 02t,10may01,tpw  Improve regSetSizes handling so trailing zeros are optional.
# 02u,18feb01,dtr  Some code comments.
# 02t,15jan01,dtr  Adding support for Altivec Register set.
# 02s,19oct00,zl   moved SH setup to gdbShSetup in sh-sh.tcl.
# 02r,23mar00,tpw  Make win32 event getting more CPU friendly.
# 02r,15mar00,zl   merged SH support from T1
# 02r,23mar00,tpw  Make win32 event getting more CPU friendly.
# 01z,19sep00,tpw  Clone m68k (sans floating point) to create coldfire.
# 01y,28apr98,ur  added M.CORE support.
# 02r,18may99,rsh  merge wrs-es.tor1_0_1.mcore-f -> T2
# 02s,04nov99,dra  Changed sparcv9 to sparc64 for gdb config.
# 02r,06jun99,dra  added SPARCV9 support.
# 02q,07apr99,dbt  Check connection with target server before removing hardware
#                  breakpoints. (SPR #26116).
# 02p,09mar99,tpw  Update SIMHPPA register info to match GDB's idea of FP regs
#                  there.
# 02o,10feb99,dbt  modified gdbTargetCloseHook() to remove all hardware
#                  breakpoints set by CrossWind before exiting.
# 02n,16oct98,tpw  moved gdbEventProcTaskFilter logic into GDB where it belongs.
# 02m,15oct98,dbt  handle only vio events with channel between 0 and 255.
# 02l,13oct98,tpw  commented out 'sdetach' logic, GDB handles it now.
# 02k,02oct98,c_s  don't use polling on W32; cgdb handles that now.
# 02j,21sep98,tpw  add gdbTaskStatusGet, get running/suspended status for tasks.
# 02i,21sep98,tpw  added check for valid task Id in gdbTaskAttachHook.
# 02h,10sep98,tpw  commented out 'gdbWrsBehavior' logic, GDB handles it now.
# 02g,09sep98,tpw  commented out load/unload processing, GDB handles it now.
# 02f,29jul98,j_k  changed references of simpc back to simnt
# 02e,28jul98,fle  changed reference to simnt into simpc
# 02d,21may98,tdl added gdb simnt register set
# 01u,17oct97,tpw remove two debug puts's in TMG patch of 01t below.
# 01t,09oct97,tpw check-in TMG patch for thumb support.
# 01s,27sep96,cdp added ARM support.
# 02c,09apr98,dbs fix gdbWtxCommandsInit to catch commands that fail
#                 to create properly.
# 02b,18mar98,jmb HPSIM merge, Mark Mason patch 05nov96:
#                 removed retVal field, moved flags field in REG_SET.
# 02b,18mar98,jmb HPSIM merge, Mark Mason patch 29oct96:
#                 removed sig_regs field from HPPA. 
# 01z,27feb98,dbt changed wtxEventpointAdd flags (SPR #6229).
# 01y,19mar96,jco added asmClass for gdbCheckConfiguration during merge from
#              pr vxsim solaris.
# 01x,19mar96,ism set family to sparc for simsp.
# 01w,11nov96,sjw Changes to make Gdb emulator friendly - only get task list in
#                 system mode after tasking has been enabled.
# 01v,18oct96,pad Now call cplusStratShow at startup.
# 01u,17oct96,sjw correct demange{} params, protect task list gopher, allow
#                 task listing to be enabled and disabled, support setting
#                 breakpoints from gdb via gdbBreakpointAdd{}, track system
#                 attachment, support query of event status, handle GDB_STOP
#                 and GDB_EVAL events, add numerous gdb commands bound to
#                 Tcl commands.
# 01t,13aug96,jmb added ease patch (mem) for HPSIM support.
# 01s,04jun96,mem added MIPS support.
# 01r,15may96,s_w fix SPR 6544 by eating up pending events before attaching
#                 to a task (e.g. event from breakpoint set in shell).
# 01q,05mar96,cah correctly add FPU registers for PowerPC
# 01p,01mar96,s_w fix SPR 5716, check for existance of task before looking
#		  at the deletion safety TCB field.
# 01o,26feb96,s_w fix SPR 6063, in task mode filter out events not from
#		  current task being debugged.
# 01n,29jan96,tpr corrected the register sizes for PowerPC.
# 01m,20jan96,tpr added PowerPC definition.
# 01l,23jan96,s_w fix SPR 5885, typo in error string
# 01k,21dec95,wmd added sourcing of gdb.tcl from WIND_BASE/.wind (SPR 5765)
# 01j,01dec95,s_w add gdbTaskAttachHook and gdbSystemAttachHook that get
#		  get called before an attach occurs. Allows checking of
#		  VX_UNBREAKABLE task option (SPR 5575)
# 01i,14nov95,s_w change error message in task kill hook
# 01h,09nov95,s_w add gdbTaskKillHook (see SPR 5090 and 5421) and 
#		  gdbSystemKillHook for completeness. Add gdbWtxErrorHook
#		  which gets called when a C API (only) call causes a 
#		  WTX error (part of fix for SPR 4591).
# 01g,07nov95,s_w make loading and unloading of modules on receipt of
#		  OBJ_[UN]LOADED events optional.
# 01f,07nov95,s_w add code to read users gdb.tcl file, add Tcl proc
#		  to get and dispatch events received by gdb. Add hook
#		  procs called on target, task and system attach/detach
#		  SPR 4860, 5047
# 01e,28sep95,s_w add gdb register set info (SPR 4916)
# 01d,13sep95,s_w add gdbIORedirect and gdbIOClose to support redirection
#		  of task and global IO by Gdb (SPR 4592)
# 01c,05sep95,s_w add gdbTaskNameToId so gdb can convert task names to
#		  task ids (for SPR 4587). Correct use of wtxPath to avoid
#		  trailing slash in file names.
# 01b,14jun95,s_w override the shell.tcl WTX error handler. Change
#		  gdbCpuTypeNameGet to take a type number arg.
# 01a,08jun95,s_w written

# Register set info used by Gdb, format is:
#
# $gdbRegs(<cpuFamily>,numRegs) == <number of gdb registers>
# $gdbRegs(<cpuFamily>,regSetSizes) == {<IU reg set size>, <FPU ...>, ...}
# $gdbRegs(<cpuFamily>,<gdbRegNum>) == {<offset> <regSize> <regSet> <regName>}
#
# where <offset> is the offset in the register set data as returned by the
# target agent, <regSize> is the size in bytes, <regSet> is the register set
# that the register is stored in and <regName> is the gdb register name.
#
# dtr Note: that regSetSizes array has increased for extra altivec. Not really 
# required but just implicitly states size of zero for each family of 
# processors where not present.

global gdbRegs

# sparc family register info
set gdbRegs(sparc,numRegs)	72
set gdbRegs(sparc,regSetSizes)	{160 136}

# global registers g0-g7 
set gdbRegs(sparc,0)  { 0x20 4 IU g0 }
set gdbRegs(sparc,1)  { 0x24 4 IU g1 }
set gdbRegs(sparc,2)  { 0x28 4 IU g2 }
set gdbRegs(sparc,3)  { 0x2c 4 IU g3 }
set gdbRegs(sparc,4)  { 0x30 4 IU g4 }
set gdbRegs(sparc,5)  { 0x34 4 IU g5 }
set gdbRegs(sparc,6)  { 0x38 4 IU g6 }
set gdbRegs(sparc,7)  { 0x3c 4 IU g7 }

# out registers o0-o5 sp and o7
set gdbRegs(sparc,8)  { 0x40 4 IU o0 }
set gdbRegs(sparc,9)  { 0x44 4 IU o1 }
set gdbRegs(sparc,10) { 0x48 4 IU o2 }
set gdbRegs(sparc,11) { 0x4c 4 IU o3 }
set gdbRegs(sparc,12) { 0x50 4 IU o4 }
set gdbRegs(sparc,13) { 0x54 4 IU o5 }
set gdbRegs(sparc,14) { 0x58 4 IU sp }
set gdbRegs(sparc,15) { 0x5c 4 IU o7 } 

# local registers l0-l7 
set gdbRegs(sparc,16) { 0x60 4 IU l0 }
set gdbRegs(sparc,17) { 0x64 4 IU l1 }
set gdbRegs(sparc,18) { 0x68 4 IU l2 } 
set gdbRegs(sparc,19) { 0x6c 4 IU l3 }
set gdbRegs(sparc,20) { 0x70 4 IU l4 }
set gdbRegs(sparc,21) { 0x74 4 IU l5 }
set gdbRegs(sparc,22) { 0x78 4 IU l6 }
set gdbRegs(sparc,23) { 0x7c 4 IU l7 }

# in registers i0-i8 
set gdbRegs(sparc,24) { 0x80 4 IU i0 }
set gdbRegs(sparc,25) { 0x84 4 IU i1 }
set gdbRegs(sparc,26) { 0x88 4 IU i2 }
set gdbRegs(sparc,27) { 0x8c 4 IU i3 }
set gdbRegs(sparc,28) { 0x90 4 IU i4 }
set gdbRegs(sparc,29) { 0x94 4 IU i5 }
set gdbRegs(sparc,30) { 0x98 4 IU i6 }
set gdbRegs(sparc,31) { 0x9c 4 IU i7 }

# floating point registers f0-f31 
set gdbRegs(sparc,32) { 0x00 4 FPU f0 } 
set gdbRegs(sparc,33) { 0x04 4 FPU f1 } 
set gdbRegs(sparc,34) { 0x08 4 FPU f2 }
set gdbRegs(sparc,35) { 0x0c 4 FPU f3 }
set gdbRegs(sparc,36) { 0x10 4 FPU f4 }
set gdbRegs(sparc,37) { 0x14 4 FPU f5 }
set gdbRegs(sparc,38) { 0x18 4 FPU f6 }
set gdbRegs(sparc,39) { 0x1c 4 FPU f7 }
set gdbRegs(sparc,40) { 0x20 4 FPU f8 }
set gdbRegs(sparc,41) { 0x24 4 FPU f9 }
set gdbRegs(sparc,42) { 0x28 4 FPU f10 }
set gdbRegs(sparc,43) { 0x2c 4 FPU f11 }
set gdbRegs(sparc,44) { 0x30 4 FPU f12 }
set gdbRegs(sparc,45) { 0x34 4 FPU f13 }
set gdbRegs(sparc,46) { 0x38 4 FPU f14 }
set gdbRegs(sparc,47) { 0x3c 4 FPU f15 }
set gdbRegs(sparc,48) { 0x40 4 FPU f16 }
set gdbRegs(sparc,49) { 0x44 4 FPU f17 }
set gdbRegs(sparc,50) { 0x48 4 FPU f18 }
set gdbRegs(sparc,51) { 0x4c 4 FPU f19 }
set gdbRegs(sparc,52) { 0x50 4 FPU f20 }
set gdbRegs(sparc,53) { 0x54 4 FPU f21 }
set gdbRegs(sparc,54) { 0x58 4 FPU f22 }
set gdbRegs(sparc,55) { 0x5c 4 FPU f23 }
set gdbRegs(sparc,56) { 0x60 4 FPU f24 }
set gdbRegs(sparc,57) { 0x64 4 FPU f25 }
set gdbRegs(sparc,58) { 0x68 4 FPU f26 }
set gdbRegs(sparc,59) { 0x6c 4 FPU f27 }
set gdbRegs(sparc,60) { 0x70 4 FPU f28 }
set gdbRegs(sparc,61) { 0x74 4 FPU f29 }
set gdbRegs(sparc,62) { 0x78 4 FPU f30 }
set gdbRegs(sparc,63) { 0x7c 4 FPU f31 }

set gdbRegs(sparc,64) { 0x14 4 IU y    }
set gdbRegs(sparc,65) { 0x08 4 IU psr  } 
set gdbRegs(sparc,66) { 0x0c 4 IU wim  }
set gdbRegs(sparc,67) { 0x10 4 IU tbr  }
set gdbRegs(sparc,68) { 0x00 4 IU pc   }
set gdbRegs(sparc,69) { 0x04 4 IU npc  }
set gdbRegs(sparc,70) { 0x84 4 FPU fpsr }
# "cpsr" register not available from VxWorks 
set gdbRegs(sparc,71) { -1  -1 FPU cpsr  }

# sparc64 family register info
set gdbRegs(sparc64,numRegs)	125
set gdbRegs(sparc64,regSetSizes)	{304 264}

# global registers g0-g7
set gdbRegs(sparc64,0)  { 0x28 8 IU g0 }
set gdbRegs(sparc64,1)  { 0x30 8 IU g1 }
set gdbRegs(sparc64,2)  { 0x38 8 IU g2 }
set gdbRegs(sparc64,3)  { 0x40 8 IU g3 }
set gdbRegs(sparc64,4)  { 0x48 8 IU g4 }
set gdbRegs(sparc64,5)  { 0x50 8 IU g5 }
set gdbRegs(sparc64,6)  { 0x58 8 IU g6 }
set gdbRegs(sparc64,7)  { 0x60 8 IU g7 }

# out registers o0-o5 sp and o7
set gdbRegs(sparc64,8)  { 0x68 8 IU o0 }
set gdbRegs(sparc64,9)  { 0x70 8 IU o1 }
set gdbRegs(sparc64,10) { 0x78 8 IU o2 }
set gdbRegs(sparc64,11) { 0x80 8 IU o3 }
set gdbRegs(sparc64,12) { 0x88 8 IU o4 }
set gdbRegs(sparc64,13) { 0x90 8 IU o5 }
set gdbRegs(sparc64,14) { 0x98 8 IU sp }
set gdbRegs(sparc64,15) { 0xa0 8 IU o7 } 

# local registers l0-l7 
set gdbRegs(sparc64,16) { 0xa8 8 IU l0 }
set gdbRegs(sparc64,17) { 0xb0 8 IU l1 }
set gdbRegs(sparc64,18) { 0xb8 8 IU l2 } 
set gdbRegs(sparc64,19) { 0xc0 8 IU l3 }
set gdbRegs(sparc64,20) { 0xc8 8 IU l4 }
set gdbRegs(sparc64,21) { 0xd0 8 IU l5 }
set gdbRegs(sparc64,22) { 0xd8 8 IU l6 }
set gdbRegs(sparc64,23) { 0xe0 8 IU l7 }

# in registers i0-i8 
set gdbRegs(sparc64,24) { 0xe8 8 IU i0 }
set gdbRegs(sparc64,25) { 0xf0 8 IU i1 }
set gdbRegs(sparc64,26) { 0xf8 8 IU i2 }
set gdbRegs(sparc64,27) { 0x100 8 IU i3 }
set gdbRegs(sparc64,28) { 0x108 8 IU i4 }
set gdbRegs(sparc64,29) { 0x110 8 IU i5 }
set gdbRegs(sparc64,30) { 0x118 8 IU i6 }
set gdbRegs(sparc64,31) { 0x120 8 IU i7 }

# float registers f0-f31 (four bytes each)
set gdbRegs(sparc64,32) { 0x00 4 FPU f0 } 
set gdbRegs(sparc64,33) { 0x04 4 FPU f1 } 
set gdbRegs(sparc64,34) { 0x08 4 FPU f2 }
set gdbRegs(sparc64,35) { 0x0c 4 FPU f3 }
set gdbRegs(sparc64,36) { 0x10 4 FPU f4 }
set gdbRegs(sparc64,37) { 0x14 4 FPU f5 }
set gdbRegs(sparc64,38) { 0x18 4 FPU f6 }
set gdbRegs(sparc64,39) { 0x1c 4 FPU f7 }
set gdbRegs(sparc64,40) { 0x20 4 FPU f8 }
set gdbRegs(sparc64,41) { 0x24 4 FPU f9 }
set gdbRegs(sparc64,42) { 0x28 4 FPU f10 }
set gdbRegs(sparc64,43) { 0x2c 4 FPU f11 }
set gdbRegs(sparc64,44) { 0x30 4 FPU f12 }
set gdbRegs(sparc64,45) { 0x34 4 FPU f13 }
set gdbRegs(sparc64,46) { 0x38 4 FPU f14 }
set gdbRegs(sparc64,47) { 0x3c 4 FPU f15 }
set gdbRegs(sparc64,48) { 0x40 4 FPU f16 }
set gdbRegs(sparc64,49) { 0x44 4 FPU f17 }
set gdbRegs(sparc64,50) { 0x48 4 FPU f18 }
set gdbRegs(sparc64,51) { 0x4c 4 FPU f19 }
set gdbRegs(sparc64,52) { 0x50 4 FPU f20 }
set gdbRegs(sparc64,53) { 0x54 4 FPU f21 }
set gdbRegs(sparc64,54) { 0x58 4 FPU f22 }
set gdbRegs(sparc64,55) { 0x5c 4 FPU f23 }
set gdbRegs(sparc64,56) { 0x60 4 FPU f24 }
set gdbRegs(sparc64,57) { 0x64 4 FPU f25 }
set gdbRegs(sparc64,58) { 0x68 4 FPU f26 }
set gdbRegs(sparc64,59) { 0x6c 4 FPU f27 }
set gdbRegs(sparc64,60) { 0x70 4 FPU f28 }
set gdbRegs(sparc64,61) { 0x74 4 FPU f29 }
set gdbRegs(sparc64,62) { 0x78 4 FPU f30 }
set gdbRegs(sparc64,63) { 0x7c 4 FPU f31 }

# floating point registers f32-f62 (8 bytes each; even regs only)
set gdbRegs(sparc64,64) { 0x80 8 FPU f32 } 
set gdbRegs(sparc64,65) { 0x88 8 FPU f34 } 
set gdbRegs(sparc64,66) { 0x90 8 FPU f36 }
set gdbRegs(sparc64,67) { 0x98 8 FPU f38 }
set gdbRegs(sparc64,68) { 0xa0 8 FPU f40 }
set gdbRegs(sparc64,69) { 0xa8 8 FPU f42 }
set gdbRegs(sparc64,70) { 0xb0 8 FPU f44 }
set gdbRegs(sparc64,71) { 0xb8 8 FPU f46 }
set gdbRegs(sparc64,72) { 0xc0 8 FPU f48 }
set gdbRegs(sparc64,73) { 0xc8 8 FPU f50 }
set gdbRegs(sparc64,74) { 0xd0 8 FPU f52 }
set gdbRegs(sparc64,75) { 0xd8 8 FPU f54 }
set gdbRegs(sparc64,76) { 0xe0 8 FPU f56 }
set gdbRegs(sparc64,77) { 0xe8 8 FPU f58 }
set gdbRegs(sparc64,78) { 0xf0 8 FPU f60 }
set gdbRegs(sparc64,79) { 0xf8 8 FPU f62 }

set gdbRegs(sparc64,80) { 0x00 4 IU pc }
set gdbRegs(sparc64,81) { 0x04 4 IU npc }
set gdbRegs(sparc64,82) { 0x0b 1 IU ccr }
# GDB only supports 32-bits of fsr
set gdbRegs(sparc64,83) { 0x104 4 FPU fsr }
set gdbRegs(sparc64,84) { -1  -1 IU fprs }
set gdbRegs(sparc64,85) { 0x1c 4 IU y} 
set gdbRegs(sparc64,86) { 0x0c 1 IU asi } 
set gdbRegs(sparc64,87) { -1  -1 IU ver }
set gdbRegs(sparc64,88) { -1  -1 IU tick }
set gdbRegs(sparc64,89) { 0x18 4 IU pil } 
set gdbRegs(sparc64,90) { 0x0d 2 IU pstate }
set gdbRegs(sparc64,91) { -1  -1 IU tstate }
set gdbRegs(sparc64,92) { -1  -1 IU tba }
set gdbRegs(sparc64,93) { 0x14 4 IU tl }
set gdbRegs(sparc64,94) { 0x10 4 IU tt }
set gdbRegs(sparc64,95) { -1  -1 IU tpc }
set gdbRegs(sparc64,96) { -1  -1 IU tnpc }
set gdbRegs(sparc64,97) { -1  -1 IU wstate }
set gdbRegs(sparc64,98) { 0x0f 1 IU cwp  } 
set gdbRegs(sparc64,99) { -1  -1 IU cansave }
set gdbRegs(sparc64,100) { -1  -1 IU canrestore } 
set gdbRegs(sparc64,101) { -1  -1 IU cleanwin } 
set gdbRegs(sparc64,102) { -1  -1 IU otherwin } 
set gdbRegs(sparc64,103) { -1  -1 IU asr16 } 
set gdbRegs(sparc64,104) { -1  -1 IU asr17 } 
set gdbRegs(sparc64,105) { -1  -1 IU asr18 } 
set gdbRegs(sparc64,106) { -1  -1 IU asr19 } 
set gdbRegs(sparc64,107) { -1  -1 IU asr20 } 
set gdbRegs(sparc64,108) { -1  -1 IU asr21 } 
set gdbRegs(sparc64,109) { -1  -1 IU asr22 } 
set gdbRegs(sparc64,110) { -1  -1 IU asr23 } 
set gdbRegs(sparc64,111) { -1  -1 IU asr24 } 
set gdbRegs(sparc64,112) { -1  -1 IU asr25 } 
set gdbRegs(sparc64,113) { -1  -1 IU asr26 } 
set gdbRegs(sparc64,114) { -1  -1 IU asr27 } 
set gdbRegs(sparc64,115) { -1  -1 IU asr28 } 
set gdbRegs(sparc64,116) { -1  -1 IU asr29 } 
set gdbRegs(sparc64,117) { -1  -1 IU asr30 } 
set gdbRegs(sparc64,118) { -1  -1 IU asr31 } 
set gdbRegs(sparc64,119) { -1  -1 IU icc } 
set gdbRegs(sparc64,120) { -1  -1 IU xcc } 
set gdbRegs(sparc64,121) { -1  -1 IU fcc0 } 
set gdbRegs(sparc64,122) { -1  -1 IU fcc1 } 
set gdbRegs(sparc64,123) { -1  -1 IU fcc2 } 
set gdbRegs(sparc64,124) { -1  -1 IU fcc3 } 

# simso family register info
set gdbRegs(simso,numRegs)	72
set gdbRegs(simso,regSetSizes)	{172 136}

# global registers g0-g7 
set gdbRegs(simso,0)  { 0x2c 4 IU g0 }
set gdbRegs(simso,1)  { 0x30 4 IU g1 }
set gdbRegs(simso,2)  { 0x34 4 IU g2 }

set gdbRegs(simso,3)  { 0x38 4 IU g3 }
set gdbRegs(simso,4)  { 0x3c 4 IU g4 }
set gdbRegs(simso,5)  { 0x40 4 IU g5 }
set gdbRegs(simso,6)  { 0x44 4 IU g6 }
set gdbRegs(simso,7)  { 0x48 4 IU g7 }

# out registers o0-o5 sp and o7
set gdbRegs(simso,8)  { 0x4c 4 IU o0 }
set gdbRegs(simso,9)  { 0x50 4 IU o1 }
set gdbRegs(simso,10) { 0x54 4 IU o2 }
set gdbRegs(simso,11) { 0x58 4 IU o3 }
set gdbRegs(simso,12) { 0x5c 4 IU o4 }
set gdbRegs(simso,13) { 0x60 4 IU o5 }
set gdbRegs(simso,14) { 0x64 4 IU sp }
set gdbRegs(simso,15) { 0x68 4 IU o7 } 

# local registers l0-l7 
set gdbRegs(simso,16) { 0x6c 4 IU l0 }
set gdbRegs(simso,17) { 0x70 4 IU l1 }
set gdbRegs(simso,18) { 0x74 4 IU l2 } 
set gdbRegs(simso,19) { 0x78 4 IU l3 }
set gdbRegs(simso,20) { 0x7c 4 IU l4 }
set gdbRegs(simso,21) { 0x80 4 IU l5 }
set gdbRegs(simso,22) { 0x84 4 IU l6 }
set gdbRegs(simso,23) { 0x88 4 IU l7 }

# in registers i0-i8 
set gdbRegs(simso,24) { 0x8c 4 IU i0 }
set gdbRegs(simso,25) { 0x90 4 IU i1 }
set gdbRegs(simso,26) { 0x94 4 IU i2 }
set gdbRegs(simso,27) { 0x98 4 IU i3 }
set gdbRegs(simso,28) { 0x9c 4 IU i4 }
set gdbRegs(simso,29) { 0xa0 4 IU i5 }
set gdbRegs(simso,30) { 0xa4 4 IU i6 }
set gdbRegs(simso,31) { 0xa8 4 IU i7 }

# floating point registers f0-f31 
set gdbRegs(simso,32) { 0x00 4 FPU f0 } 
set gdbRegs(simso,33) { 0x04 4 FPU f1 } 
set gdbRegs(simso,34) { 0x08 4 FPU f2 }
set gdbRegs(simso,35) { 0x0c 4 FPU f3 }
set gdbRegs(simso,36) { 0x10 4 FPU f4 }
set gdbRegs(simso,37) { 0x14 4 FPU f5 }
set gdbRegs(simso,38) { 0x18 4 FPU f6 }
set gdbRegs(simso,39) { 0x1c 4 FPU f7 }
set gdbRegs(simso,40) { 0x20 4 FPU f8 }
set gdbRegs(simso,41) { 0x24 4 FPU f9 }
set gdbRegs(simso,42) { 0x28 4 FPU f10 }
set gdbRegs(simso,43) { 0x2c 4 FPU f11 }
set gdbRegs(simso,44) { 0x30 4 FPU f12 }
set gdbRegs(simso,45) { 0x34 4 FPU f13 }
set gdbRegs(simso,46) { 0x38 4 FPU f14 }
set gdbRegs(simso,47) { 0x3c 4 FPU f15 }
set gdbRegs(simso,48) { 0x40 4 FPU f16 }
set gdbRegs(simso,49) { 0x44 4 FPU f17 }
set gdbRegs(simso,50) { 0x48 4 FPU f18 }
set gdbRegs(simso,51) { 0x4c 4 FPU f19 }
set gdbRegs(simso,52) { 0x50 4 FPU f20 }
set gdbRegs(simso,53) { 0x54 4 FPU f21 }
set gdbRegs(simso,54) { 0x58 4 FPU f22 }
set gdbRegs(simso,55) { 0x5c 4 FPU f23 }
set gdbRegs(simso,56) { 0x60 4 FPU f24 }
set gdbRegs(simso,57) { 0x64 4 FPU f25 }
set gdbRegs(simso,58) { 0x68 4 FPU f26 }
set gdbRegs(simso,59) { 0x6c 4 FPU f27 }
set gdbRegs(simso,60) { 0x70 4 FPU f28 }
set gdbRegs(simso,61) { 0x74 4 FPU f29 }
set gdbRegs(simso,62) { 0x78 4 FPU f30 }
set gdbRegs(simso,63) { 0x7c 4 FPU f31 }

set gdbRegs(simso,64) { 0x14 4 IU y    }
set gdbRegs(simso,65) { 0x08 4 IU psr  } 
set gdbRegs(simso,66) { 0x0c 4 IU wim  }
set gdbRegs(simso,67) { 0x10 4 IU tbr  }
set gdbRegs(simso,68) { 0x00 4 IU pc   }
set gdbRegs(simso,69) { 0x04 4 IU npc  }
set gdbRegs(simso,70) { 0x84 4 FPU fpsr }
# "cpsr" register not available from VxWorks 
set gdbRegs(simso,71) { -1  -1 FPU cpsr  }

# simnt family register info

set gdbRegs(simnt,numRegs)	21
set gdbRegs(simnt,regSetSizes)	{40 92}

# data registers 
set gdbRegs(simnt,0)  {0x1c 4 IU eax}
set gdbRegs(simnt,1)  {0x18 4 IU ecx}
set gdbRegs(simnt,2)  {0x14 4 IU edx}
set gdbRegs(simnt,3)  {0x10 4 IU ebx}
set gdbRegs(simnt,4)  {0x0c 4 IU esp}
set gdbRegs(simnt,5)  {0x08 4 IU ebp}
set gdbRegs(simnt,6)  {0x04 4 IU esi}
set gdbRegs(simnt,7)  {0x00 4 IU edi}
set gdbRegs(simnt,8)  {0x20 4 IU eflags}
set gdbRegs(simnt,9)  {0x24 4 IU pc }

# floating point registers
set gdbRegs(simnt,10) {0x0c 10 FPU st0}
set gdbRegs(simnt,11) {0x16 10 FPU st1}
set gdbRegs(simnt,12) {0x20 10 FPU st2}
set gdbRegs(simnt,13) {0x2a 10 FPU st3}
set gdbRegs(simnt,14) {0x34 10 FPU st4}
set gdbRegs(simnt,15) {0x3e 10 FPU st5}
set gdbRegs(simnt,16) {0x48 10 FPU st6}
set gdbRegs(simnt,17) {0x52 10 FPU st7}
set gdbRegs(simnt,18) {0x00 4  FPU fpc}
set gdbRegs(simnt,19) {0x04 4  FPU fps}
set gdbRegs(simnt,20) {0x08 4  FPU fpt}

# m68k family register info

set gdbRegs(m68k,numRegs)	29
set gdbRegs(m68k,regSetSizes)	{72 108}

# data registers
set gdbRegs(m68k,0)  {0x00 4 IU d0}
set gdbRegs(m68k,1)  {0x04 4 IU d1}
set gdbRegs(m68k,2)  {0x08 4 IU d2}
set gdbRegs(m68k,3)  {0x0c 4 IU d3}
set gdbRegs(m68k,4)  {0x10 4 IU d4}
set gdbRegs(m68k,5)  {0x14 4 IU d5}
set gdbRegs(m68k,6)  {0x18 4 IU d6}
set gdbRegs(m68k,7)  {0x1c 4 IU d7}

# address registers
set gdbRegs(m68k,8)  {0x20 4 IU a0}
set gdbRegs(m68k,9)  {0x24 4 IU a1}
set gdbRegs(m68k,10) {0x28 4 IU a2}
set gdbRegs(m68k,11) {0x2c 4 IU a3}
set gdbRegs(m68k,12) {0x30 4 IU a4}
set gdbRegs(m68k,13) {0x34 4 IU a5}

set gdbRegs(m68k,14) {0x38 4 IU fp}
set gdbRegs(m68k,15) {0x3c 4 IU sp}

# Between sp and ps there are 2 bytes of padding which are included
# here as part of ps. This because gdb treats the ps as 4 bytes
	
# "ps" also known as "sr" status register to VxWorks
set gdbRegs(m68k,16) {0x40 4 IU ps}    
set gdbRegs(m68k,17) {0x44 4 IU pc}

# floating point registers
set gdbRegs(m68k,18) {0x0c 12 FPU fp0}
set gdbRegs(m68k,19) {0x18 12 FPU fp1}
set gdbRegs(m68k,20) {0x24 12 FPU fp2}
set gdbRegs(m68k,21) {0x30 12 FPU fp3}
set gdbRegs(m68k,22) {0x3c 12 FPU fp4}
set gdbRegs(m68k,23) {0x48 12 FPU fp5}
set gdbRegs(m68k,24) {0x54 12 FPU fp6}
set gdbRegs(m68k,25) {0x60 12 FPU fp7}
set gdbRegs(m68k,26) {0x00 4 FPU fpcontrol}
set gdbRegs(m68k,27) {0x04 4 FPU fpstatus }
set gdbRegs(m68k,28) {0x08 4 FPU fpiaddr  }

# coldfire family register info

set gdbRegs(coldfire,numRegs)	18
set gdbRegs(coldfire,regSetSizes)	{72}

# data registers
set gdbRegs(coldfire,0)  {0x00 4 IU d0}
set gdbRegs(coldfire,1)  {0x04 4 IU d1}
set gdbRegs(coldfire,2)  {0x08 4 IU d2}
set gdbRegs(coldfire,3)  {0x0c 4 IU d3}
set gdbRegs(coldfire,4)  {0x10 4 IU d4}
set gdbRegs(coldfire,5)  {0x14 4 IU d5}
set gdbRegs(coldfire,6)  {0x18 4 IU d6}
set gdbRegs(coldfire,7)  {0x1c 4 IU d7}

# address registers
set gdbRegs(coldfire,8)  {0x20 4 IU a0}
set gdbRegs(coldfire,9)  {0x24 4 IU a1}
set gdbRegs(coldfire,10) {0x28 4 IU a2}
set gdbRegs(coldfire,11) {0x2c 4 IU a3}
set gdbRegs(coldfire,12) {0x30 4 IU a4}
set gdbRegs(coldfire,13) {0x34 4 IU a5}

set gdbRegs(coldfire,14) {0x38 4 IU fp}
set gdbRegs(coldfire,15) {0x3c 4 IU sp}

# Between sp and ps there are 2 bytes of padding which are included
# here as part of ps. This because gdb treats the ps as 4 bytes
	
# "ps" also known as "sr" status register to VxWorks
# As of T2.1.0/Coldfire, MAC support was inserted IN THE MIDDLE.
set gdbRegs(coldfire,16) {0x4a 2 IU ps}    
set gdbRegs(coldfire,17) {0x4c 4 IU pc}


# i86 family register info

set gdbRegs(i86,numRegs)	21
set gdbRegs(i86,regSetSizes)	{40 92}

# data registers 
set gdbRegs(i86,0)  {0x1c 4 IU eax}
set gdbRegs(i86,1)  {0x18 4 IU ecx}
set gdbRegs(i86,2)  {0x14 4 IU edx}
set gdbRegs(i86,3)  {0x10 4 IU ebx}
set gdbRegs(i86,4)  {0x0c 4 IU esp}
set gdbRegs(i86,5)  {0x08 4 IU ebp}
set gdbRegs(i86,6)  {0x04 4 IU esi}
set gdbRegs(i86,7)  {0x00 4 IU edi}
set gdbRegs(i86,8)  {0x20 4 IU eflags}
set gdbRegs(i86,9)  {0x24 4 IU pc }

# floating point registers
set gdbRegs(i86,10) {0x0c 10 FPU st0}
set gdbRegs(i86,11) {0x16 10 FPU st1}
set gdbRegs(i86,12) {0x20 10 FPU st2}
set gdbRegs(i86,13) {0x2a 10 FPU st3}
set gdbRegs(i86,14) {0x34 10 FPU st4}
set gdbRegs(i86,15) {0x3e 10 FPU st5}
set gdbRegs(i86,16) {0x48 10 FPU st6}
set gdbRegs(i86,17) {0x52 10 FPU st7}
set gdbRegs(i86,18) {0x00 4  FPU fpc}
set gdbRegs(i86,19) {0x04 4  FPU fps}
set gdbRegs(i86,20) {0x08 4  FPU fpt}

set gdbRegs(i960,numRegs)	39
set gdbRegs(i960,regSetSizes)	{140 64}

# data registers
set gdbRegs(i960,0)  {0x00 4 IU pfp}
set gdbRegs(i960,1)  {0x04 4 IU sp }
set gdbRegs(i960,2)  {0x08 4 IU rip}
set gdbRegs(i960,3)  {0x0c 4 IU r3 }
set gdbRegs(i960,4)  {0x10 4 IU r4 }
set gdbRegs(i960,5)  {0x14 4 IU r5 }
set gdbRegs(i960,6)  {0x18 4 IU r6 }
set gdbRegs(i960,7)  {0x1c 4 IU r7 }
set gdbRegs(i960,8)  {0x20 4 IU r8 }
set gdbRegs(i960,9)  {0x24 4 IU r9 }
set gdbRegs(i960,10) {0x28 4 IU r10}
set gdbRegs(i960,11) {0x2c 4 IU r11}
set gdbRegs(i960,12) {0x30 4 IU r12}
set gdbRegs(i960,13) {0x34 4 IU r13}
set gdbRegs(i960,14) {0x38 4 IU r14}
set gdbRegs(i960,15) {0x3c 4 IU r15}
set gdbRegs(i960,16) {0x40 4 IU g0 }
set gdbRegs(i960,17) {0x44 4 IU g1 }
set gdbRegs(i960,18) {0x48 4 IU g2 }
set gdbRegs(i960,19) {0x4c 4 IU g3 }
set gdbRegs(i960,20) {0x50 4 IU g4 }
set gdbRegs(i960,21) {0x54 4 IU g5 }
set gdbRegs(i960,22) {0x58 4 IU g6 }
set gdbRegs(i960,23) {0x5c 4 IU g7 }
set gdbRegs(i960,24) {0x60 4 IU g8 }
set gdbRegs(i960,25) {0x64 4 IU g9 }
set gdbRegs(i960,26) {0x68 4 IU g10}
set gdbRegs(i960,27) {0x6c 4 IU g11}
set gdbRegs(i960,28) {0x70 4 IU g12}
set gdbRegs(i960,29) {0x74 4 IU g13}
set gdbRegs(i960,30) {0x78 4 IU g14}
set gdbRegs(i960,31) {0x7c 4 IU fp }
set gdbRegs(i960,32) {0x80 4 IU pcw}
set gdbRegs(i960,33) {0x84 4 IU ac }
set gdbRegs(i960,34) {0x88 4 IU tc }

# floating point registers
set gdbRegs(i960,35) {0x00 10 FPU fp0}
set gdbRegs(i960,36) {0x10 10 FPU fp1}
set gdbRegs(i960,37) {0x20 10 FPU fp2}
set gdbRegs(i960,38) {0x30 10 FPU fp3}

# MIPS family register info
#
# The MIPS family register info is calculated based on the target CPU
# type and the target endianness.
#

# SuperH (SH) family register info
#
# The SH family register info is calculated based on the target CPU
# type when gdbShSetup is invoked from sh-sh.tcl in 
#




##############################################################################
#
# PowerPC family register info

set gdbRegs(ppc,numRegs)	112
set gdbRegs(ppc,regSetSizes)	{156 260 0 0 0 0 0 532}

# General Purpose Registers 
set gdbRegs(ppc,0)  {0x00 4 IU r0}
set gdbRegs(ppc,1)  {0x04 4 IU r1}
set gdbRegs(ppc,2)  {0x08 4 IU r2}
set gdbRegs(ppc,3)  {0x0c 4 IU r3}
set gdbRegs(ppc,4)  {0x10 4 IU r4}
set gdbRegs(ppc,5)  {0x14 4 IU r5}
set gdbRegs(ppc,6)  {0x18 4 IU r6}
set gdbRegs(ppc,7)  {0x1c 4 IU r7}
set gdbRegs(ppc,8)  {0x20 4 IU r8}
set gdbRegs(ppc,9)  {0x24 4 IU r9}
set gdbRegs(ppc,10)  {0x28 4 IU r10}
set gdbRegs(ppc,11)  {0x2c 4 IU r11}
set gdbRegs(ppc,12)  {0x30 4 IU r12}
set gdbRegs(ppc,13)  {0x34 4 IU r13}
set gdbRegs(ppc,14)  {0x38 4 IU r14}
set gdbRegs(ppc,15)  {0x3c 4 IU r15}
set gdbRegs(ppc,16)  {0x40 4 IU r16}
set gdbRegs(ppc,17)  {0x44 4 IU r17}
set gdbRegs(ppc,18)  {0x48 4 IU r18}
set gdbRegs(ppc,19)  {0x4c 4 IU r19}
set gdbRegs(ppc,20)  {0x50 4 IU r20}
set gdbRegs(ppc,21)  {0x54 4 IU r21}
set gdbRegs(ppc,22)  {0x58 4 IU r22}
set gdbRegs(ppc,23)  {0x5c 4 IU r23}
set gdbRegs(ppc,24)  {0x60 4 IU r24}
set gdbRegs(ppc,25)  {0x64 4 IU r25}
set gdbRegs(ppc,26)  {0x68 4 IU r26}
set gdbRegs(ppc,27)  {0x6c 4 IU r27}
set gdbRegs(ppc,28)  {0x70 4 IU r28}
set gdbRegs(ppc,29)  {0x74 4 IU r29}
set gdbRegs(ppc,30)  {0x78 4 IU r30}
set gdbRegs(ppc,31)  {0x7c 4 IU r31}

# CPU State registers

set gdbRegs(ppc,32)  {0x80 4 IU msr}
set gdbRegs(ppc,33)  {0x84 4 IU lr}
set gdbRegs(ppc,34)  {0x88 4 IU ctr}
set gdbRegs(ppc,35)  {0x8c 4 IU pc}
set gdbRegs(ppc,36)  {0x90 4 IU cr}
set gdbRegs(ppc,37)  {0x94 4 IU xer}
set gdbRegs(ppc,38)  {0x98 4 IU mq}

# floating point registers and csr

set gdbRegs(ppc,39)  {0x00 8 FPU f0}
set gdbRegs(ppc,40)  {0x08 8 FPU f1}
set gdbRegs(ppc,41)  {0x10 8 FPU f2}
set gdbRegs(ppc,42)  {0x18 8 FPU f3}
set gdbRegs(ppc,43)  {0x20 8 FPU f4}
set gdbRegs(ppc,44)  {0x28 8 FPU f5}
set gdbRegs(ppc,45)  {0x30 8 FPU f6}
set gdbRegs(ppc,46)  {0x38 8 FPU f7}
set gdbRegs(ppc,47)  {0x40 8 FPU f8}
set gdbRegs(ppc,48)  {0x48 8 FPU f9}
set gdbRegs(ppc,49)  {0x50 8 FPU f10}
set gdbRegs(ppc,50)  {0x58 8 FPU f11}
set gdbRegs(ppc,51)  {0x60 8 FPU f12}
set gdbRegs(ppc,52)  {0x68 8 FPU f13}
set gdbRegs(ppc,53)  {0x70 8 FPU f14}
set gdbRegs(ppc,54)  {0x78 8 FPU f15}
set gdbRegs(ppc,55)  {0x80 8 FPU f16}
set gdbRegs(ppc,56)  {0x88 8 FPU f17}
set gdbRegs(ppc,57)  {0x90 8 FPU f18}
set gdbRegs(ppc,58)  {0x98 8 FPU f19}
set gdbRegs(ppc,59)  {0xa0 8 FPU f20}
set gdbRegs(ppc,60)  {0xa8 8 FPU f21}
set gdbRegs(ppc,61)  {0xb0 8 FPU f22}
set gdbRegs(ppc,62)  {0xb8 8 FPU f23}
set gdbRegs(ppc,63)  {0xc0 8 FPU f24}
set gdbRegs(ppc,64)  {0xc8 8 FPU f25}
set gdbRegs(ppc,65)  {0xd0 8 FPU f26}
set gdbRegs(ppc,66)  {0xd8 8 FPU f27}
set gdbRegs(ppc,67)  {0xe0 8 FPU f28}
set gdbRegs(ppc,68)  {0xe8 8 FPU f29}
set gdbRegs(ppc,69)  {0xf0 8 FPU f30}
set gdbRegs(ppc,70)  {0xf8 8 FPU f31}
# floating point status and control register for PPC
set gdbRegs(ppc,71)  {0x100 4 FPU fpscr}
# dead registers to keep the list contiguous 
set gdbRegs(ppc,72)  {-1 -1 IU X72}
set gdbRegs(ppc,73)  {-1 -1 IU X73}
set gdbRegs(ppc,74)  {-1 -1 IU X74}
set gdbRegs(ppc,75)  {-1 -1 IU X75}
set gdbRegs(ppc,76)  {-1 -1 IU X76}
set gdbRegs(ppc,77)  {-1 -1 IU X77}
# Vector registers ,vcsr and vrsave
set gdbRegs(ppc,78)  {0x00 16 AV v0}
set gdbRegs(ppc,79)  {0x10 16 AV v1}
set gdbRegs(ppc,80)  {0x20 16 AV v2}
set gdbRegs(ppc,81)  {0x30 16 AV v3}
set gdbRegs(ppc,82)  {0x40 16 AV v4}
set gdbRegs(ppc,83)  {0x50 16 AV v5}
set gdbRegs(ppc,84)  {0x60 16 AV v6}
set gdbRegs(ppc,85)  {0x70 16 AV v7}
set gdbRegs(ppc,86)  {0x80 16 AV v8}
set gdbRegs(ppc,87)  {0x90 16 AV v9}
set gdbRegs(ppc,88)  {0xa0 16 AV v10}
set gdbRegs(ppc,89)  {0xb0 16 AV v11}
set gdbRegs(ppc,90)  {0xc0 16 AV v12}
set gdbRegs(ppc,91)  {0xd0 16 AV v13}
set gdbRegs(ppc,92)  {0xe0 16 AV v14}
set gdbRegs(ppc,93)  {0xf0 16 AV v15}
set gdbRegs(ppc,94)  {0x100 16 AV v16}
set gdbRegs(ppc,95)  {0x110 16 AV v17}
set gdbRegs(ppc,96)  {0x120 16 AV v18}
set gdbRegs(ppc,97)  {0x130 16 AV v19}
set gdbRegs(ppc,98)  {0x140 16 AV v20}
set gdbRegs(ppc,99)  {0x150 16 AV v21}
set gdbRegs(ppc,100)  {0x160 16 AV v22}
set gdbRegs(ppc,101)  {0x170 16 AV v23}
set gdbRegs(ppc,102)  {0x180 16 AV v24}
set gdbRegs(ppc,103)  {0x190 16 AV v25}
set gdbRegs(ppc,104)  {0x1a0 16 AV v26}
set gdbRegs(ppc,105)  {0x1b0 16 AV v27}
set gdbRegs(ppc,106)  {0x1c0 16 AV v28}
set gdbRegs(ppc,107)  {0x1d0 16 AV v29}
set gdbRegs(ppc,108)  {0x1e0 16 AV v30}
set gdbRegs(ppc,109)  {0x1f0 16 AV v31}
#  altivec vscr for PPC note padding of 12 bytes
set gdbRegs(ppc,110)  {0x200 4 AV vscr}
#  altivec vrsave
set gdbRegs(ppc,111)  {0x210 4 AV vrsave}

# hppa family register info

set gdbRegs(hppa,numRegs)	128
set gdbRegs(hppa,regSetSizes)	{264 256}

set gdbRegs(hppa,0)     {0x0000 4 IU flags}
set gdbRegs(hppa,1)     {0x001c 4 IU gr1}
set gdbRegs(hppa,2)     {0x0020 4 IU rp}
set gdbRegs(hppa,3)     {0x0024 4 IU gr3}
set gdbRegs(hppa,4)     {0x0028 4 IU gr4}
set gdbRegs(hppa,5)     {0x002c 4 IU gr5}
set gdbRegs(hppa,6)     {0x0030 4 IU gr6}
set gdbRegs(hppa,7)     {0x0034 4 IU gr7}
set gdbRegs(hppa,8)     {0x0038 4 IU gr8}
set gdbRegs(hppa,9)     {0x003c 4 IU gr9}
set gdbRegs(hppa,10)    {0x0040 4 IU gr10}
set gdbRegs(hppa,11)    {0x0044 4 IU gr11}
set gdbRegs(hppa,12)    {0x0048 4 IU gr12}
set gdbRegs(hppa,13)    {0x004c 4 IU gr13}
set gdbRegs(hppa,14)    {0x0050 4 IU gr14}
set gdbRegs(hppa,15)    {0x0054 4 IU gr15}
set gdbRegs(hppa,16)    {0x0058 4 IU gr16}
set gdbRegs(hppa,17)    {0x005c 4 IU gr17}
set gdbRegs(hppa,18)    {0x0060 4 IU gr18}
set gdbRegs(hppa,19)    {0x0064 4 IU gr19}
set gdbRegs(hppa,20)    {0x0068 4 IU gr20}
set gdbRegs(hppa,21)    {0x006c 4 IU gr21}
set gdbRegs(hppa,22)    {0x0070 4 IU gr22}
set gdbRegs(hppa,23)    {0x0074 4 IU arg3}
set gdbRegs(hppa,24)    {0x0078 4 IU arg2}
set gdbRegs(hppa,25)    {0x007c 4 IU arg1}
set gdbRegs(hppa,26)    {0x0080 4 IU arg0}
set gdbRegs(hppa,27)    {0x0084 4 IU dp}
set gdbRegs(hppa,28)    {0x0088 4 IU ret0}
set gdbRegs(hppa,29)    {0x008c 4 IU ret1}
set gdbRegs(hppa,30)    {0x0090 4 IU sp}
set gdbRegs(hppa,31)    {0x0094 4 IU gr31}
set gdbRegs(hppa,32)    {0x00b8 4 IU sar}
set gdbRegs(hppa,33)    {0x0008 4 IU pcoqh}
set gdbRegs(hppa,34)    {0x0010 4 IU pcsqh}
set gdbRegs(hppa,35)    {0x000c 4 IU pcoqt}
set gdbRegs(hppa,36)    {0x0014 4 IU pcsqt}
set gdbRegs(hppa,37)    {0x00bc 4 IU eiem}
set gdbRegs(hppa,38)    {0x00c0 4 IU iir}
set gdbRegs(hppa,39)    {0x00c4 4 IU isr}
set gdbRegs(hppa,40)    {0x00c8 4 IU ior}
set gdbRegs(hppa,41)    {0x00cc 4 IU ipsw}
set gdbRegs(hppa,42)    {0x00d0 4 IU _goto}
set gdbRegs(hppa,43)    {0x00a8 4 IU sr4}
set gdbRegs(hppa,44)    {0x0098 4 IU sr0}
set gdbRegs(hppa,45)    {0x00bc 4 IU sr1}
set gdbRegs(hppa,46)    {0x00a0 4 IU sr2}
set gdbRegs(hppa,47)    {0x00a4 4 IU sr3}
set gdbRegs(hppa,48)    {0x00a8 4 IU sr5}
set gdbRegs(hppa,49)    {0x00b0 4 IU sr6}
set gdbRegs(hppa,50)    {0x00b4 4 IU sr7}
set gdbRegs(hppa,51)    {0x00d8 4 IU cr0}
set gdbRegs(hppa,52)    {0x00dc 4 IU cr8}
set gdbRegs(hppa,53)    {0x00e0 4 IU cr9}
set gdbRegs(hppa,54)    {0x00d4 4 IU ccr}
set gdbRegs(hppa,55)    {0x00e4 4 IU cr12}
set gdbRegs(hppa,56)    {0x00e8 4 IU cr13}
set gdbRegs(hppa,57)    {0x00ec 4 IU cr24}
set gdbRegs(hppa,58)    {0x00f0 4 IU cr25}
set gdbRegs(hppa,59)    {0x00f4 4 IU cr26}
set gdbRegs(hppa,60)    {0x00f8 4 IU mpsfu_high}
set gdbRegs(hppa,61)    {0x00fc 4 IU mpsfu_low}
set gdbRegs(hppa,62)    {0x0100 4 IU mpsfu_ovflo}
set gdbRegs(hppa,63)    {0x0000 4 IU pad}
set gdbRegs(hppa,64)    {0x0000 4 FPU fpsr}
set gdbRegs(hppa,65)    {0x0004 4 FPU fpe1}
set gdbRegs(hppa,66)    {0x0008 4 FPU fpe2}
set gdbRegs(hppa,67)    {0x000c 4 FPU fpe3}
set gdbRegs(hppa,68)    {0x0010 4 FPU fpe4}
set gdbRegs(hppa,69)    {0x0014 4 FPU fpe5}
set gdbRegs(hppa,70)    {0x0018 4 FPU fpe6}
set gdbRegs(hppa,71)    {0x001c 4 FPU fpe7}
set gdbRegs(hppa,72)    {0x0020 4 FPU fr4}
set gdbRegs(hppa,73)    {0x0024 4 FPU fr4R}
set gdbRegs(hppa,74)    {0x0028 4 FPU fr5}
set gdbRegs(hppa,75)    {0x002c 4 FPU fr5R}
set gdbRegs(hppa,76)    {0x0030 4 FPU fr6}
set gdbRegs(hppa,77)    {0x0034 4 FPU fr6R}
set gdbRegs(hppa,78)    {0x0038 4 FPU fr7}
set gdbRegs(hppa,79)    {0x003c 4 FPU fr7R}
set gdbRegs(hppa,80)    {0x0040 4 FPU fr8}
set gdbRegs(hppa,81)    {0x0044 4 FPU fr8R}
set gdbRegs(hppa,82)    {0x0048 4 FPU fr9}
set gdbRegs(hppa,83)    {0x004c 4 FPU fr9R}
set gdbRegs(hppa,84)    {0x0050 4 FPU fr10}
set gdbRegs(hppa,85)    {0x0054 4 FPU fr10R}
set gdbRegs(hppa,86)    {0x0058 4 FPU fr11}
set gdbRegs(hppa,87)    {0x005c 4 FPU fr11R}
set gdbRegs(hppa,88)    {0x0060 4 FPU fr12}
set gdbRegs(hppa,89)    {0x0064 4 FPU fr12R}
set gdbRegs(hppa,90)    {0x0068 4 FPU fr13}
set gdbRegs(hppa,91)    {0x006c 4 FPU fr13R}
set gdbRegs(hppa,92)    {0x0070 4 FPU fr14}
set gdbRegs(hppa,93)    {0x0074 4 FPU fr14R}
set gdbRegs(hppa,94)    {0x0078 4 FPU fr15}
set gdbRegs(hppa,95)    {0x007c 4 FPU fr15R}
set gdbRegs(hppa,96)    {0x0080 4 FPU fr16}
set gdbRegs(hppa,97)    {0x0084 4 FPU fr16R}
set gdbRegs(hppa,98)    {0x0088 4 FPU fr17}
set gdbRegs(hppa,99)    {0x008c 4 FPU fr17R}
set gdbRegs(hppa,100)   {0x0090 4 FPU fr18}
set gdbRegs(hppa,101)   {0x0094 4 FPU fr18R}
set gdbRegs(hppa,102)    {0x0098 4 FPU fr19}
set gdbRegs(hppa,103)    {0x009c 4 FPU fr19R}
set gdbRegs(hppa,104)    {0x00a0 4 FPU fr20}
set gdbRegs(hppa,105)    {0x00a4 4 FPU fr20R}
set gdbRegs(hppa,106)    {0x00a8 4 FPU fr21}
set gdbRegs(hppa,107)    {0x00ac 4 FPU fr21R}
set gdbRegs(hppa,108)    {0x00b0 4 FPU fr22}
set gdbRegs(hppa,109)    {0x00b4 4 FPU fr22R}
set gdbRegs(hppa,110)    {0x00b8 4 FPU fr23}
set gdbRegs(hppa,111)    {0x00bc 4 FPU fr23R}
set gdbRegs(hppa,112)    {0x00c0 4 FPU fr24}
set gdbRegs(hppa,113)    {0x00c4 4 FPU fr24R}
set gdbRegs(hppa,114)    {0x00c8 4 FPU fr25}
set gdbRegs(hppa,115)    {0x00cc 4 FPU fr25R}
set gdbRegs(hppa,116)    {0x00d0 4 FPU fr26}
set gdbRegs(hppa,117)    {0x00d4 4 FPU fr26R}
set gdbRegs(hppa,118)    {0x00d8 4 FPU fr27}
set gdbRegs(hppa,119)    {0x00dc 4 FPU fr27R}
set gdbRegs(hppa,120)    {0x00e0 4 FPU fr28}
set gdbRegs(hppa,121)    {0x00e4 4 FPU fr28R}
set gdbRegs(hppa,122)    {0x00e8 4 FPU fr29}
set gdbRegs(hppa,123)    {0x00ec 4 FPU fr29R}
set gdbRegs(hppa,124)    {0x00f0 4 FPU fr30}
set gdbRegs(hppa,125)    {0x00f4 4 FPU fr30R}
set gdbRegs(hppa,126)    {0x00f8 4 FPU fr31}
set gdbRegs(hppa,127)    {0x00fc 4 FPU fr31R}

# ARM family register info

set gdbRegs(arm,numRegs)        26
set gdbRegs(arm,regSetSizes)    {68}

# data registers 
set gdbRegs(arm,0)   {0x00 4 IU r0}
set gdbRegs(arm,1)   {0x04 4 IU r1}
set gdbRegs(arm,2)   {0x08 4 IU r2}
set gdbRegs(arm,3)   {0x0C 4 IU r3}
set gdbRegs(arm,4)   {0x10 4 IU r4}
set gdbRegs(arm,5)   {0x14 4 IU r5}
set gdbRegs(arm,6)   {0x18 4 IU r6}
set gdbRegs(arm,7)   {0x1C 4 IU r7}
set gdbRegs(arm,8)   {0x20 4 IU r8}
set gdbRegs(arm,9)   {0x24 4 IU r9}
set gdbRegs(arm,10)  {0x28 4 IU r10}
set gdbRegs(arm,11)  {0x2C 4 IU r11}
set gdbRegs(arm,12)  {0x30 4 IU r12}
set gdbRegs(arm,13)  {0x34 4 IU r13}
set gdbRegs(arm,14)  {0x38 4 IU r14}
set gdbRegs(arm,15)  {0x3C 4 IU pc}

set gdbRegs(arm,16)  {-1 -1 FPU f0}
set gdbRegs(arm,17)  {-1 -1 FPU f1}
set gdbRegs(arm,18)  {-1 -1 FPU f2}
set gdbRegs(arm,19)  {-1 -1 FPU f3}
set gdbRegs(arm,20)  {-1 -1 FPU f4}
set gdbRegs(arm,21)  {-1 -1 FPU f5}
set gdbRegs(arm,22)  {-1 -1 FPU f6}
set gdbRegs(arm,23)  {-1 -1 FPU f7}
set gdbRegs(arm,24)  {-1 -1 FPU fps}

set gdbRegs(arm,25)  {0x40 4 IU cpsr}

# floating point registers TBD

# M.CORE family register info

set gdbRegs(mcore,numRegs)	18
set gdbRegs(mcore,regSetSizes)	{72}

# general registers
set gdbRegs(mcore,0)  {0x00 4 IU sp}
set gdbRegs(mcore,1)  {0x04 4 IU r1}
set gdbRegs(mcore,2)  {0x08 4 IU r2}
set gdbRegs(mcore,3)  {0x0c 4 IU r3}
set gdbRegs(mcore,4)  {0x10 4 IU r4}
set gdbRegs(mcore,5)  {0x14 4 IU r5}
set gdbRegs(mcore,6)  {0x18 4 IU r6}
set gdbRegs(mcore,7)  {0x1c 4 IU r7}
set gdbRegs(mcore,8)  {0x20 4 IU r8}
set gdbRegs(mcore,9)  {0x24 4 IU r9}
set gdbRegs(mcore,10) {0x28 4 IU r10}
set gdbRegs(mcore,11) {0x2c 4 IU r11}
set gdbRegs(mcore,12) {0x30 4 IU r12}
set gdbRegs(mcore,13) {0x34 4 IU r13}
set gdbRegs(mcore,14) {0x38 4 IU r14}
set gdbRegs(mcore,15) {0x3c 4 IU r15}

set gdbRegs(mcore,16) {0x40 4 IU psr}    
set gdbRegs(mcore,17) {0x44 4 IU pc}

##############################################################################
#
# gdbNumRegsGet - get number of gdb registers for current target
#
# RETURNS
#   register count or raises error

proc gdbNumRegsGet {family} {
    global gdbRegs

    if {! [info exists gdbRegs($family,numRegs)]} {
	error "No register info available for CPU family $family"
    } else {
	return $gdbRegs($family,numRegs)
    }
}

##############################################################################
#
# gdbRegSetSizesGet - get register set sizes for current target
#
# RETURNS
#   register set sizes string

proc gdbRegSetSizesGet {family args} {
    global gdbRegs
    if {! [info exists gdbRegs($family,regSetSizes)]} {
	error "No register set size info available for CPU family $cpuFamily"
    } else {
	set numRegSets 8;	# WTX_REG_SET_LAST from host/include/wtxtypes.h
	if { [llength $args] > 0 } {
	    # If we are given this, then it's a new GDB that tells us what
	    # the real WTX_REG_SET_LAST value is. Old GDB's didn't do this.
	    set numRegSets [lindex $args 0]
	}
	set regSetSizes $gdbRegs($family,regSetSizes)
	while { [llength $regSetSizes] < $numRegSets } {
	    lappend regSetSizes 0
	}
	return $regSetSizes
    }
}

##############################################################################
#
# gdbRegInfoGet - get gdb register info for given CPU family
#
# RETURNS
#   register info string

proc gdbRegsInfoGet { family regNum } {
    global gdbRegs

    if {! [info exists gdbRegs($family,$regNum)]} {
	error "No register info for CPU family $family register $regNum"
    } else {
	set regInfo $gdbRegs($family,$regNum)
	return [format "%d %d %d" [lindex $regInfo 0] [lindex $regInfo 1] [wtxEnumFromString REG_SET_TYPE [lindex $regInfo 2]]]
    }    
}

##############################################################################
#
# gdbRegInfoGetByName - get gdb register info for given CPU family, by reg name
#
# RETURNS
#   register info string

proc gdbRegsInfoGetByName { family regName } {
    global gdbRegByName
    global gdbRegs

    if {! [info exists gdbRegByName($family)]} {
	# error out if no information exists for this family
	set gdbRegByName($family) [gdbNumRegsGet $family]
	set numglob {[0-9]*}
	foreach frn [array names gdbRegs $family,$numglob] {
	    set regInfo $gdbRegs($frn)
	    set gdbRegByName($family,[lindex $regInfo 3]) $regInfo
	}
    }
    if {! [info exists gdbRegByName($family,$regName)]} {
	# report that the register does not exist. Not an error.
	return "-1 -1 [wtxEnumFromString REG_SET_TYPE IU]"
    } else {
	set regInfo $gdbRegByName($family,$regName)
	return [format "%d %d %d" [lindex $regInfo 0] [lindex $regInfo 1] [wtxEnumFromString REG_SET_TYPE [lindex $regInfo 2]]]
    }
}

##############################################################################
#
# gdbCheckConfiguration - check compatibility of gdb with current target
#
# RETURNS
#   raises error on incompatibility

proc gdbCheckConfiguration { gdbArch } {
    global cpuFamily
    global asmClass
    global __wtxCpuType

    set family $asmClass($cpuFamily($__wtxCpuType))
    
   # if {$gdbArch == "simso"} {
   #     set gdbArch sparc
   # }

    if {$family == "thumb"} {	# Ugly cludge!!!! /kgb
        set family "arm"
    }
    if { $gdbArch == "sparc64" } {
        set family "sparc"
    }

    if {$asmClass($gdbArch) != $family} {
	error "Gdb configuration ($gdbArch) incompatible with target ($family)"
    }

    if { $family == "sh" } {
	gdbShSetup
    }

}

##############################################################################
#
# gdbMakeCommandName - builds a command name from a Tcl proc name
#
# This routines takes a Tcl proc name and converts it to a form that gdb
# can use for a command name. Leading underscores are trimmed and upper
# case letters are lowercased with a preceeding `-' ie. "wtxTargetName"
# becomes "wtx-target-name" and "__foo" becomes "foo"
#
# SYNOPSIS:
#   gdbMakeCommandName name
#
# RETURNS
#   name string

proc gdbMakeCommandName { name } {
    regsub -all {([A-Z])} $name {-\1} newName
    return [string tolower [string trimleft $newName _]]
}

##############################################################################
#
# demangle - no-op name demangler
#
# This routine does a no-op demangle and is a temporary stand-in for a real
# demangle command in gdb.  It overcomes the problem of using shell Tcl procs
# when the windsh demangle command isn't present.
#
# SYNOPSIS:
#   demangle style name
#
# RETURNS:
#   name

proc demangle { style name mode } {
    return $name
}


##############################################################################
#
# gdbWtxCommandsInit - add wtx Tcl commands to gdb command list
#
# This routine adds all the Tcl commands starting with "wtx" to the 
# gdb command list using gdbMakeCommandName to form gdb friendly names
#
# RETURNS: 
#   N/A

proc gdbWtxCommandsInit {} {
    foreach cmd [info command wtx*] {
	catch {gdb tclproc [gdbMakeCommandName $cmd] $cmd} dummy
    }
}


##############################################################################
#
# gdbWtxErrorHandler - WTX error handler for gdb tcl calls.
#
# This routine is used to override the error handler set by the shell
#
# RETURNS: N/A

proc gdbWtxErrorHandler {hwtx cmd err tag} {

    # Just resubmit the error so it is reported by gdb.
    error $err

}


##############################################################################
#
# gdbWindShellInit - initialize windShell Tcl module
#
# This routine sets up the windShell module and makes the windShell commands
# available from gdb
#
# RETURNS:
#   N/A

proc gdbWindShellInit {} {
    global shellProcList

    # Load shell if needed
    if { [info command shellInit] == "" } {
	uplevel #0 source [wtxPath host resource tcl]shell.tcl
    }
    
    # Initialize shell
    shellInit

    # Print C++ strategy

    [shellName cplusStratShow]

    # Override its setting of the error handler that is for windsh use only.
    wtxErrorHandler [wtxHandle] gdbWtxErrorHandler

    foreach cmd $shellProcList {
	set shellCmd [shellName $cmd]
	gdb tclproc wind-[gdbMakeCommandName $shellCmd] $shellCmd
    }
}


##############################################################################
#
# gdbWindTaskListGet - get a list of tasks from Wind kernel
#
# This routine fetches a list of tasks running from the Wind kernel for
# use by Gdb.  gdbWindTaskListGet is called indirectly by gdb using
# gdbTaskListGet.
#
# RETURNS:
#    List of tasks and task info

proc gdbWindTaskListGet {} {
    global gdbTargetMultiTasking
    global offset

    # Check if activeQHead is initialized
    if { ! $gdbTargetMultiTasking } {

        # Check if activeQHead was initialized since
        # last check

        set gdbTargetMultiTasking [targetInitCheck]

        if { ! $gdbTargetMultiTasking } {
            error "Not implemented!" 
        }

        # Initialize threads support now that
        # multi-tasking is enabled

	# Maybe need a gdbWindShellInit here...
    }

    if [catch { wtxGopherEval "[shSymAddr activeQHead] * 
        {
	< 
	-$offset(WIND_TCB,activeNode) !
	<+$offset(WIND_TCB,name) *$>
	<+$offset(WIND_TCB,pStackBase) @>
	<+$offset(WIND_TCB,pStackEnd) @>
	<+$offset(WIND_TCB,regs) !>
	> 
	*
        }"} result] {
	    error "Can't get task list"
	} else {
	    return $result
	}
}


##############################################################################
#
# gdbTaskListGet - get a list of tasks form the target kernel
#
# This routine fetches a list of tasks running on the target for use
# by gdb.  It does so by calling a kernel specific Tcl routine - currently
# only the Wind kernel is supported so the call is hard coded.
#
# RETURNS:
#    List of tasks and task info.

proc gdbTaskListGet {} {
    global gdbTaskListEnabled

    # FIXME: Should vector through a pointer based on kernel type

    if {$gdbTaskListEnabled} {
	set ret {}
	foreach item [gdbWindTaskListGet] {
	    regsub -all { } $item {_} item
	    regsub -all "\t" $item {^I} item
	    lappend ret $item
	}
	return $ret
    } else {
	return ""
    }
}

##############################################################################
#
# gdbTaskNameToId - convert a task name to a task ID
#
# This routine takes a task name and converts it to a task ID
#
# RETURNS:
#    The task ID or 0 on error

proc gdbTaskNameToId { name } {
    # FIXME: Should vector through a pointer based on kernel type

    # Only try to convert names starting with "t"
    if { [string range $name 0 0] != "t" } {
	return 0
    } else {
	return [taskNameToId $name]
    }
}


##############################################################################
#
# gdbCpuTypeNameGet - converts a CPU type number to a name
#
# RETURNS:
#   A string representation of the CPU type eg "MC68020"

proc gdbCpuTypeNameGet { cpuTypeNum } {
    global cpuType

    # FIXME: Should vector through a pointer based on kernel type

    return $cpuType($cpuTypeNum)
}


##############################################################################
#
# gdbSystemRun - do a run command in system mode
#
# This routine spawns of a new task for gdb given the entry point and the
# arguments. It relies on Wind kernel facilites to work in both system and
# task mode.
#
# RETURNS:
#   N/A

proc gdbSystemRun { entryName {arg0 0} {arg1 0} {arg2 0} {arg3 0} {arg4 0} } {
    # FIXME: Should vector through a pointer based on kernel type
    
    # 
    # Note: there is currently no way to tell what mode the agent is in
    # to we first try to create a new task using task mode facilites and
    # if that fails try system mode.
    #

    gdb call excJobAdd (taskSpawn,"tDbgSys",100,0,20000,$entryName,$arg0,$arg1,$arg2,$arg3,$arg4)
}


##############################################################################
#
# gdbTargetFileOpen - open a file on the target
#
# This routine uses shFuncCall to open a file on the target system
#
# RETURNS:
#   The fd number of the new file

proc gdbTargetFileOpen { filename mode } {
    
    set openFunc [shSymAddr "open"]
    set block [memBlockCreate -string $filename]
    set pFilename [wtxMemAlloc [lindex [memBlockInfo $block] 0]]

    wtxMemWrite $block $pFilename
    set fileFd [shFuncCall -int $openFunc $pFilename $mode 0 0 0 0 0 0 0 0]
    wtxMemFree $pFilename

    memBlockDelete $block

    if {$fileFd == -1} {
	error "Could not open target file $filename"
    }
    
    return $fileFd
}

##############################################################################
#
# gdbIORedirect - performs I/O redirections on a task or globally
#
# RETURNS:
#   N/A

proc gdbIORedirect { inFile outFile {taskId -1} } {

    global vioOut
    global vioIn
    global fdIn
    global fdOut
    global hostFdIn
    global hostFdOut

    # Figure out if out and in are the same file
    if {[string compare $inFile $outFile] == 0} {
	set shared 1
    } else {
	set shared 0
    }

    # If both input and output are "-" then nothing to do
    if { $shared && ($inFile == "-") } {
	return
    }

    # Ignore any previous redirections
    set vioIn  0
    set vioOut 0
    set fdIn   -1
    set fdOut  -1
    set hostFdOut -1
    set hostFdIn  -1

    # Open host based files for the input and output, or a single r/w file,
    # with a VIO channel redirected to/from the host file. Only do this if
    # the file is host and not target based.  Target names are prefixed with
    # '@' as in windShell.

    if { $shared && ([string index $inFile 0] != "@") } {

	# Use O_RDWR | O_CREAT, mode is rwrwrw
	set vioIn [wtxVioChanGet]
	set hostFdIn [wtxOpen -channel $vioIn $inFile 0x0202 0666]

    } else {
	
	if { $inFile != "-" && ([string index $inFile 0] != "@") } {
	    # Use O_RDONLY, mode is ignored
	    set vioIn [wtxVioChanGet]
	    set hostFdIn [wtxOpen -channel $vioIn $inFile 0x0 0]
	}
	if { $outFile != "-" && ([string index $outFile 0] != "@") } {
	    # Use O_WRONLY | O_CREAT, mode is rwrwrw
	    set vioOut [wtxVioChanGet]
	    set hostFdOut [wtxOpen -channel $vioOut $outFile 0x0201 0666]
	}
	
    }
    
    # Open files on the target now to redirect the input and output to.
    # If the input/output name starts with a '@' then use that, otherwise
    # use the appropriate VIO channel opened above.

    if { $shared } {

	if { [string index $inFile 0] != "@" } {
	    set file [format "/vio/%d" $vioIn]
	} else {
	    # Trim '@' prefix
	    set file [string range $inFile 1 end]
	}

	if {[regexp {^[0-9]+$} $file]} {
	    set fdIn $file
	} else {
	    set fdIn [gdbTargetFileOpen $file 0x02]
	}
	set fdOut $fdIn

    } else {

	if { $inFile != "-" } {

	    if { [string index $inFile 0] != "@" } {
		set file [format "/vio/%d" $vioIn]
	    } else {
		# Trim '@' prefix
		set file [string range $inFile 1 end]	
	    }
	    
	    if {[regexp {^[0-9]+$} $file]} {
		set fdIn $file
	    } else {
		set fdIn [gdbTargetFileOpen $file 0x0]
	    }
		
	}
	if { $outFile != "-" } {

	    if { [string index $outFile 0] != "@" } {
		set file [format "/vio/%d" $vioOut]
	    } else {
		# Trim '@' prefix
		set file [string range $outFile 1 end]	
	    }
	    
	    if {[regexp {^[0-9]+$} $file]} {
		set fdOut $file
	    } else {
		set fdOut [gdbTargetFileOpen $file 0x1]
	    }

	}

    }

    # Now call a function to redirect the output 

    if { $taskId == -1 } {

	set redirFunc [shSymAddr "ioGlobalStdSet"]

	if {$fdIn != -1} {
	    shFuncCall -int $redirFunc 0 $fdIn  0 0 0 0 0 0 0 0
	}
	if {$fdOut != -1} {
	    shFuncCall -int $redirFunc 1 $fdOut 0 0 0 0 0 0 0 0
	    shFuncCall -int $redirFunc 2 $fdOut 0 0 0 0 0 0 0 0
	}

    } else {

	set redirFunc [shSymAddr "ioTaskStdSet"]
	
	if {$fdIn != -1} {
	    shFuncCall -int $redirFunc $taskId 0 $fdIn  0 0 0 0 0 0 0
	}
	if {$fdOut != -1} {
	    shFuncCall -int $redirFunc $taskId 1 $fdOut 0 0 0 0 0 0 0
	    shFuncCall -int $redirFunc $taskId 2 $fdOut 0 0 0 0 0 0 0
	}
    }	

    # If there wasn't a file opened on the target side then set fds
    # back to -1 so that gdbIOClose wont try to close them

    if {[regexp {^@[0-9]+$} $inFile]} {
	set fdIn -1
    }
    if {[regexp {^@[0-9]+$} $outFile]} {
	set fdOut -1
    }
    
    return
}

##############################################################################
#
# gdbIOClose - closes file redirections by previous gdbIORedirect
#
# RETURNS:
#   0 on success, -1 on failure

proc gdbIOClose {} {

    global vioOut
    global vioIn
    global hostFdIn
    global hostFdOut
    global fdIn
    global fdOut

    set closeFunc [shSymAddr close]

    # Close up open target file descriptors (ignoring errors). However,
    # don't close fd's 0-2 because these are the global fd's.

    if {$fdIn != -1} {
	catch { shFuncCall -int $closeFunc $fdIn  0 0 0 0 0 0 0 0 0 }
	set fdIn  -1
    } 
	
    if { ($fdOut != -1) && ($fdOut != $fdIn) } {
	catch { shFuncCall -int $closeFunc $fdOut 0 0 0 0 0 0 0 0 0 }
	set fdOut -1
    }


    # Close any host based files and release reserved VIO channels
    if {$hostFdIn != -1} {
	catch { wtxClose $hostFdIn }
	set hostFdIn -1
	catch { wtxVioChanRelease $vioIn }
	set vioIn 0
    }
    
    if {$hostFdOut != -1} {
	catch { wtxClose $hostFdOut }
	set hostFdOut -1
	catch { wtxVioChanRelease $vioOut }
	set vioOut 0
    }

    return
}




##############################################################################
#
# gdbIgnoreVioWriteEvents - set gdb's behaviour on VIO_WRITE.
#
# This routine is provided for users to invoke either during start up
# or during a debug session to configure whether or not VIO_WRITE events
# are ignored or handled.
#
# RETURNS:
#    N/A
 
proc gdbIgnoreVioWriteEvents {status} {
    switch -exact -- $status {
        on {
            catch { wtxUnregisterForEvent "^VIO_WRITE" }
        }
        off {
            catch { wtxUnregisterForEvent "^VIO_WRITE$" }
        }
        default {
            error "Please specify one argument 'on' or 'off'."
        }
    }
}


##############################################################################
#
# gdbTargetTclInit - initialize target specific Tcl routines
#
# This routine is called by gdb everytime a new target is connected to
# It queries the target type and does appropriate initializations for it.
# Currently on the Wind kernel is supported.
#
# RETURNS:
#    N/A

proc gdbTargetTclInit {} {

    global gdbTargetMultiTasking

    set tsInfo [wtxTsInfoGet]
    set rtType [lindex [lindex $tsInfo 3] 0]

    # First check that the runtime type is supported
    if {$rtType == 1} {

	# Don't know if this is true yet
	set gdbTargetMultiTasking 0

	# Got a VxWorks wind kernel so suck in the shell stuff
	gdbWindShellInit

    } else {

	error "gdbTargetTclInit: unsupported runtime type $rtType"

    }

    return
}


##############################################################################
#
# gdbBreakpointAdd - add a breakpoint for gdb
#
# This proc adds a breakpoint for context <contextId> of type <contextType>
# for the address <bpAddress>
#
# RETURNS: a hexadecimal string representing the breakpoint id
#

proc gdbBreakpointAdd { contextType contextId bpAddress } {
    set result [wtxEventpointAdd WTX_EVENT_TEXT_ACCESS $bpAddress \
		    $contextType $contextId ACTION_STOP|ACTION_NOTIFY 0 0 0]
    return $result
}



###############################################################################
#
# gdbWtxErrorHook - a hook called when a WTX error occurs
#
# This proc is called when an error is caused by a WTX request called by
# gdb itself (rather than in Tcl code). If this proc return a non-error,
# non-empty result then it is displayed as a warning string and the user
# is given the opportunity to detach from the current target. If the yes
# answers in the affirmative then all targets are popped and the current
# command is aborted with error. If no then the WTX error is handled as it
# would normal be by gdb.
#
# RETURNS: See description

proc gdbWtxErrorHook { errMsg } {
    return
}

###############################################################################
#
# gdbTaskAttachHook - a hook called just before attaching to a task
#
# This routine is called just before gdb attaches to a task
# If it raises an error the attach is aborted by gdb with the error raised. 
# It it returns the value "0" gdb doesn't take any further action. 
# It if returns any value that is not "0" then gdb will continue with
# the attach itself.
#
# RETURNS: See description

proc gdbTaskAttachHook { taskId } {
    global tcbOptionBit
    global gdbAttachAny

    set taskNameMap [activeTaskNameMap]
    set listIx [lsearch $taskNameMap [format "0x%x" $taskId]]
    if {$listIx == -1} {
	# unknown task ID
	error "invalid task $taskId"
    }

    if { ! $gdbAttachAny } {
	set options [lindex [taskInfoGetVerbose $taskId] 11]
    
	if {($options & $tcbOptionBit(VX_UNBREAKABLE))} {
	    error "cannot attach to task with VX_UNBREAKABLE option set"
	}
    }

#[TPW] This functionality has been moved into GDB itself.
#    # Eat up an pending events in the event queue.  Because we are
#    # not yet attached to a task any breakpoints will automatically
#    # be ignored but events like OBJ_LOADED or VIO_WRITE will be
#    # processed normally.  This fixes SPR 6544
#
#    while { [gdbEventGetAndDispatch] != "" } {}

    # Hunky dory
    return
}

###############################################################################
#
# gdbTaskStatusGet - get task status
#
# RETURNS
#   task status 

proc gdbTaskStatusGet { tid } {
    global offset

    set query "$tid <+$offset(WIND_TCB,status) @>"
    if {[catch {wtxGopherEval $query} result]} {
	error "Error getting task $tid status"
    }

    return $result
}

###############################################################################
#
# gdbSystemAttachHook - a hook called just before an attach command is executed
#
# This routine is called just before gdb attaches to the system.
# If it raises an error the attach is aborted by gdb with the error raised. 
# It it returns the value "0" gdb doesn't take any further action. 
# It if returns any value that is not "0" then gdb will continue with
# the attach itself.
#
# RETURNS: See description

proc gdbSystemAttachHook {} {
    return
}

###############################################################################
#
# gdbTargetOpenHook - a hook called when target is attached to
#
# This proc is called just after the wtxToolAttach call in gdb, at a point
# where any failure will result in gdbTargetCloseHook being called. Any
# error in this routine wall cause the target open to fail (and call the
# close hook).
#
# RETURNS:
#   N/A

proc gdbTargetOpenHook {} {
    return
}

###############################################################################
#
# gdbTargetCloseHook - a hook called just before the target is detached from
#
# This proc is called after gdb has cleaned up all its events for a target
# but before the actual wtxToolDetach call so the current WTX handle is
# still valid and connected.  Any errors in this routine are printed as
# warnings but otherwise ignored.
#
# RETURNS:
#   N/A

proc gdbTargetCloseHook {} {

    gdbHwBpRemoveAll

    return
}

###############################################################################
#
# gdbHwBpRemoveAll - remove all hardware breakpoints set by gdb
#
# This proc is called to remove all hardware breakpoints set by gdb
#
# RETURNS:
#   N/A

proc gdbHwBpRemoveAll {} {

    # check WTX connection

    if [catch {wtxTargetName} tgt] {return}
    
    # check connection with target server
    
    if [catch {wtxTsInfoGet} tsInfo] {return}

    # read gdb tool ID

    if [catch {wtxToolIdGet} gdbToolId] {return}

    # get eventpoint list

    if [catch {wtxEventpointListGet} bpList] {return}

    # remove all hardware breakpoints set by Crosswind UI
    
    foreach bpt $bpList {
    	if {[lindex [lindex $bpt 0] 0] == "WTX_EVENT_HW_BP" &&
	    [lindex $bpt 3] == $gdbToolId} {
	    catch {wtxEventpointDelete [lindex $bpt 4]}
	}
    }
}	

###############################################################################
#
# gdbTaskOpenHook - a hook called when a task is attached too
#
# This routine is called after gdb has successfully attach to a task
# It is passed the task ID as an argument (in hex format). <gdbTaskAttached>
# and <gdbCurrentTask> are set appropriately to indicate that gdb is now
# debugging a task.
#
# RETURNS:
#   N/A

proc gdbTaskOpenHook {taskId} {
    global gdbTaskAttached
    global gdbCurrentTask

    set gdbTaskAttached 1
    set gdbCurrentTask $taskId
    return
}


###############################################################################
#
# gdbTaskCloseHook - a hook called when a task is detached from
#
# This routine is called when gdb detaches from a task, after all eventpoints
# are removed but while the task is still suspended. It is passed the task 
# ID as an argument (in hex format). <gdbTaskAttached> is cleared to record
# that gdb is no longer debugging a task.
#
# RETURNS:
#   N/A

proc gdbTaskCloseHook {taskId} {
    global gdbTaskAttached

    set gdbTaskAttached 0
    return
}


###############################################################################
#
# gdbSystemOpenHook - a hook called when the system is attached too
#
# This routine is called after gdb has successfully attach to the target
# agent in system mode (when the system should be suspended in external
# mode).
#
# RETURNS:
#   N/A

proc gdbSystemOpenHook {} {
    global gdbSystemAttached

    set gdbSystemAttached 1
    return
}

###############################################################################
#
# gdbSystemCloseHook - a hook called when the system is dettached from
#
# This routine is called after gdb has successfully detached from the
# agent in system mode (when it should be back in task mode but with the
# system still suspended).
#
# RETURNS:
#   N/A

proc gdbSystemCloseHook {} {
    global gdbSystemAttached

    set gdbSystemAttached 0
    return
}


###############################################################################
#
# gdbTaskKillHook - a hook called just before a task is killed
#
# This routine is called just before gdb does a task kill.  If it raises
# an error the kill is aborted by gdb with the error raised. It it returns
# the value "0" gdb doesn't take any further action. It if returns any value
# that is not "0" then gdb will continue with the kill itself.
#
# RETURNS:
#   N/A

proc gdbTaskKillHook { taskId } {

    global offset

    # Try to suspend the task in question to validate its id
    # There doesn't appear to be any other easy way to do this, but
    # since you can only kill when the task is suspended already it
    # doesn't matter, since it doesn't change the task state.

    if [catch {wtxContextSuspend CONTEXT_TASK $taskId} result] {
	# Task probably doesn't exist any more but we don't care
	# We fall through an let Gdb print the error if the kill
	# fails

    } else {
	# Look at the safeCnt value in the TCB
	set result [wtxGopherEval "$taskId <+$offset(WIND_TCB,safeCnt) @>"]

	# No need to resume the task, its supposed to be suspended...
	if {$result > 0} {
	    # Start the task up again since we aren't going to delete it
	    error "cannot kill a task that is safe from deletion"
	}
    }

    # Let it rip!
    return
}


###############################################################################
#
# gdbSystemKillHook - a hook called just before the system is killed
#
# This routine is called just before gdb does a kill in system mode.
# If it raises an error the kill is aborted by gdb with the error raised. 
# It it returns the value "0" gdb doesn't take any further action. 
# It if returns any value that is not "0" then gdb will continue with
# the kill itself.
#
# RETURNS: See description

proc gdbSystemKillHook {} {
    return
}


##############################################################################
#
# gdbInputTimeoutHook - a hook called every time the user input times out
#
# This proc is called from the GUI<->gdb input protocol loop every time a
# timeout occurs.  It is an opportunity to go and scavenge for commands,
# pending events or other miscellaneous housework.
#
# RETURNS: nothing

proc gdbInputTimeoutHook {} {
    global gdbPendingCommands

    while {[llength $gdbPendingCommands] > 0} {
	catch {eval [lindex $gdbPendingCommands 0]}
	set gdbPendingCommands [lrange $gdbPendingCommands 1 end]
    }
}


###############################################################################
#
# gdbEventGetAndDispatch - get an event from the target server and dispatch
#
# This routine gets one event from the target server and then dispatches
# it to any registered event handler procs.  The event string is then
# passed back to the caller for any further processing.
#
# RETURNS: event string or empty string
#

proc gdbEventGetAndDispatch {} {

    global gdbEventPollTime
    global gdbEventPollRetry
    global gdbEventStatus
    global tcl_platform

    set gdbEventStatus CONTINUE

    if {$tcl_platform(platform) == "unix"} {

	# Get the event (a simple poll)
	set event [wtxEventPoll $gdbEventPollTime $gdbEventPollRetry]

    } else { 

	set ix 0
	while {[set event [wtxEventGet]] == "" && $ix < $gdbEventPollRetry} {
	    msleep $gdbEventPollTime
	    incr ix
	}

    }
	
    if {$event == ""} {

	# No event, just return nothing
	return

    } else {

	# An event was received - now try to process it
	if [catch {gdbEventDispatch $event} result] {
	    puts stdout "Event dispatch error: $result"
	    return $event
	} else {
	    return $result
	}

    }

}

###############################################################################
#
# gdbEventContinue - indicate if gdb should stop waiting for events
#
# When the event processing mechanism thinks gdb should stop processing events
# it sets the global $gdbEventStatus to DETACH or INTERRUPT and then
# after returning from the event get routine gdb will call gdbEventContinue.
# If the result is INTERRUPT then gdb behaves as if an interrupt occurred,
# if it is DETACH then gdb behaves as if the user requested a detach.
#
# RETURNS:
#    the gdbEventStatus which is INTERRUPT, DETACH or some other value
 
proc gdbEventContinue {} {
    global gdbEventStatus

    set result $gdbEventStatus
    set gdbEventStatus CONTINUE

    return $result
}


###############################################################################
#
# gdbEventDispatch - dispatch events received by gdb
#
# This routine dispatches all events received by gdb to an event specific
# Tcl event proc.  If no event proc has been defined then a default event
# proc is called which in most cases should just return the event string
# unchanged. The result returned by the event proc called is returned to
# gdb and if not further action by gdb is required the result should be
# an empty string, otherwise the event proc may rewrite or synthesis a 
# new event string as it desires.  Gdb will then carry out its default
# handling of events using the returned event string.
#
# The global array gdbEventProcs() defines the name of the event proc
# to call on each event type, if none is found gdbEventProcDefault is
# called.
#
# RETURNS:
#    Event string for gdb to process

proc gdbEventDispatch {event} {
    global gdbEventProcs
    global gdbEventShowAll

    set result $event

    if {$gdbEventShowAll} { 
	puts stdout "$event" 
    }
    
    set eventType [lindex $event 0]

    if [info exists gdbEventProcs($eventType)] {
	return [$gdbEventProcs($eventType) $event]
    } else {
	# The default handler
	return [$gdbEventProcs(DEFAULT) $event]
    }

}

##############################################################################
#
# gdbEventProcDefault - default handler for events
#
# RETURNS: event string unchanged

proc gdbEventProcDefault { event } {
    return $event
}


##############################################################################
#
# gdbEventProcVioWrite - handle VIO write events
#
# RETURNS: nothing

proc gdbEventProcVioWrite { event } {

    set vioChan [lindex $event 1]
    set mblk [lindex $event 2]
    if {$mblk != ""} {
	# The data of the VIO is in the memory block returned with the
	# event.  Dump that block to stdout and free it.

	# Dump only vio channesl between 0 and 255 (The ones we can obtain
	# with wtxVioChannelGet). Others VIO channels can be used for other
	# purpose.

	if {($vioChan <= 0xff) && ($vioChan >= 0)} {
	    memBlockWriteFile $mblk -
	}
	memBlockDelete $mblk
    }

    # Gobble the event
    return
}


#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbEventProcLoad - handle OBJ_LOADED events
#
# RETURNS: nothing
#
#proc gdbEventProcLoad { event } {
#    global gdbAutoLoad
#
#    set moduleName [lindex $event 2]
#
#    if {$gdbAutoLoad} {
#	puts stdout "Auto-loading object module: $moduleName"
#	if [catch {gdb add-symbol-file $moduleName} result] {
#	    puts "Failed."
#	}
#    } else {
#	puts stdout "Another tool has loaded object module: $moduleName (ignored)"
#    }
#
#    return
#}

##############################################################################
#
# gdbEventProcGdbStop - handle GDB_STOP events
#
# This proc handles GDB_STOP events which are used to stop gdb from debugging
# its current task.  The GDB_STOP event has one argument which is either
# INTERRUPT or DETACH.  When INTERRUPT is supplied the effect is exactly as
# if the user hit the interrupt key or button.  The current task is suspended
# and control returned to the user.  When DETACH is supplied the effect is
# to detach gdb from the task, leaving it running and return control to the
# user.  Other values have no effect but may be added in the future.
#  
# RETURNS: nothing

proc gdbEventProcGdbStop { event } {
    global gdbEventStatus
    set gdbEventStatus [lindex $event 1]

    return
}


##############################################################################
#
# gdbEventProcGdbEval - process GDB_EVAL events
#
# This proc processes the GDB_EVAL events that might arrive during Gdb
# execution.  The GDB_EVAL event contains a Tcl list of Tcl commands to 
# execute at the next convenient point.  This currently entails queuing 
# the commands, breaking out of any Gdb event wait loop and detaching from
# the current task.  The next user input timeout will cause the events to
# be executed from gdbInputTimeoutHook
#
# RETURNS: nothing

proc gdbEventProcGdbEval { event } {
    global gdbPendingCommands
    global gdbEventStatus

    set event [lrange $event 1 end]
    while {[llength $event] > 0} {
	lappend gdbPendingCommands [lindex $event 0]
	set event [lrange $event 1 end]
    }
    set gdbEventStatus DETACH

    return
}


#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbEventProcUnload - handle OBJ_UNLOADED events
#
# RETURNS: nothing
#
#proc gdbEventProcUnload { event } {
#    global gdbAutoUnload
#
#    set moduleName [lindex $event 2]
#
#    if {$gdbAutoUnload} {
#	puts stdout "Auto-unloading object module: $moduleName"
#	if [catch {gdb unload $moduleName} result] {
#	    puts "Failed."
#	}
#    } else {
#	puts stdout "Another tool has unloaded object module: $moduleName (ignored)"
#    }
#
#    return
#}

#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbEventProcTaskFilter - event proc to TaskFilter out unwanted context events
#
# This event proc must be installed for all context specific event types
# if gdb is to function properly.  It uses <gdbCurrentTask> to determine
# if the received event is relevant for gdb and quoshes or passes on the
# event appropriately.  Setting the debug variable <gdbEventShowAll> will
# cause a message to be printed whenever an event is ignored.
#
# RETURNS: event or nothing
#
#proc gdbEventProcTaskFilter { event } {
#    global gdbTaskAttached
#    global gdbCurrentTask
#    global gdbEventShowAll
#
#
#    if {! $gdbTaskAttached} {
#	return $event
#    }
#
#    if {[lindex $event 0] == "TEXT_ACCESS"} {
#	set contextId [lindex $event 1]
#    } else {
#	set contextId [lindex $event 2]
#    }
#
#    if { $gdbCurrentTask == $contextId } {
#	return $event
#    }
#
#    if { $gdbEventShowAll } {
#	puts "Event for task $contextId ignored (not current task)"
#    }
#    return ""
#}


#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbCmd{Target,Attach,Quit} - replace default gdb target, attach, quit
#
# These three commands are used to override the standard gdb commands so that
# we can detach from any attached task/target first.  This avoids annoying
# messages and questions about something running already and would we like
# to kill it.
#
# Setting $gdbClassicBehavior will disable this "improvement".  These Tcl
# procs are bound to the gdb commands 'target', 'attach' and 'quit' respectively
#
# RETURNS: N/A
#
#proc gdbCmdTarget { args } {
#    global gdbWrsBehavior
#
#    if {$gdbWrsBehavior} { gdb detach }
#    gdb target--0 $args
#    return
#}
#gdb tclproc target gdbCmdTarget
#
#proc gdbCmdAttach { args } {
#    global gdbWrsBehavior
#
#    if {$gdbWrsBehavior} { gdb detach }
#    gdb attach--0 $args
#    return
#}
#gdb tclproc attach gdbCmdAttach
#
#proc gdbCmdQuit { args } {
#    global gdbWrsBehavior
#
#    if {$gdbWrsBehavior} { gdb detach }
#    gdb quit--0 $args
#    return
#}
#gdb tclproc quit gdbCmdQuit


#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbCmdSDetach - detach from task without resuming it
#
# Bound to gdb command 'sdetach'
#
# RETURNS: N/A
#
#proc gdbCmdSDetach { args } {
#    gdb detach 1
#    return
#}
#gdb tclproc sdetach gdbCmdSDetach

    
#[TPW] This functionality has been moved into GDB itself.
##############################################################################
#
# gdbCmdSAttach - attach to another task leaving current task suspended
#
# Bound to gdb command 'sattach <task>'
#
# RETURNS: N/A
#
#proc gdbCmdSAttach { args } {
#    global gdbWrsBehavior
#
#    if {$gdbWrsBehavior} { gdb detach 1}
#    gdb attach--0 $args
#    return
#}
#gdb tclproc sattach gdbCmdSAttach


##############################################################################
#
# gdbCmdTaskingOn/Off - enable/disable getting of task list in system mode
#
# These commands, bound to threads-on and threads-off control the fetching
# of the kernel task list using gdbWindTaskListGet.  In system mode the user
# may be debugging before taskLib has been initialized thus causing errors
# if the Gopher is used to fetch the task list.  Using 'threads-off' will
# disable fetching of the thread list. Using 'threads-on' will enable fetching
# of the thread list at the next 'continue', 'step' or 'next' command.
# 
# RETURNS: N/A

proc gdbCmdTaskingOn { } {
    global gdbTaskListEnabled

    set gdbTaskListEnabled 1
    return
}
gdb tclproc tasking-on gdbCmdTaskingOn

proc gdbCmdTaskingOff { } {
    global gdbTaskListEnabled

    set gdbTaskListEnabled 0
    return
}
gdb tclproc tasking-off gdbCmdTaskingOff



##############################################################################
#
# Initialization starts here. Gdb loads this module once the first
# time a target is connected to.
#
##############################################################################

# Set up global variables for I/O redirection
set vioIn  0
set vioOut 0
set fdIn   -1
set fdOut  -1
set hostFdOut -1
set hostFdIn  -1

set gdbPendingCommands  {}
set gdbEventStatus	CONTINUE
set gdbTaskListEnabled	1

set gdbTargetMultiTasking	0

set gdbEventProcs(DEFAULT)	gdbEventProcDefault
set gdbEventProcs(VIO_WRITE)	gdbEventProcVioWrite
#[TPW] This functionality has been moved into GDB itself.
#set gdbEventProcs(OBJ_LOADED)	gdbEventProcLoad
#[TPW] This functionality has been moved into GDB itself.
#set gdbEventProcs(OBJ_UNLOADED) gdbEventProcUnload

#[TPW] This functionality has been moved into GDB itself.
#set gdbEventProcs(CTX_EXIT)	gdbEventProcTaskFilter
#set gdbEventProcs(EXCEPTION)	gdbEventProcTaskFilter
#set gdbEventProcs(TEXT_ACCESS)	gdbEventProcTaskFilter
set gdbEventProcs(GDB_STOP)	gdbEventProcGdbStop
set gdbEventProcs(GDB_EVAL)	gdbEventProcGdbEval

set gdbEventPollTime		100
set gdbEventPollRetry		2
set gdbEventShowAll		0

#[TPW] This functionality has been moved into GDB itself.
#set gdbWrsBehavior	1

set gdbAttachAny	0

#[TPW] This functionality has been moved into GDB itself.
#set gdbAutoLoad		1
#[TPW] This functionality has been moved into GDB itself.
#set gdbAutoUnload	1

set gdbTaskAttached	0
set gdbCurrentTask	0

# Add in WTX command bindings, shell bindings are done on first target
# connect (when the shell Tcl is first initialized)

gdbWtxCommandsInit

# Source the Tcl initialization file in the %WIND_BASE% directory if any

if {![catch {file exists [wtxPath .wind]gdb.tcl} result] && $result} {
    uplevel #0 source [wtxPath .wind]gdb.tcl
}

# Read in users gdb.tcl file if available

if [info exists env(HOME)] {
    if [file exists $env(HOME)/.wind/gdb.tcl] {
	uplevel #0 source $env(HOME)/.wind/gdb.tcl
    }
}
