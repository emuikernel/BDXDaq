# wtxcore.tcl - convenience arrays and functions for wtxtcl
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# wtxcore.tcl is now generated -- DO NOT EDIT
#
# To change the boilerplate or TCL code, edit wtxcore.in
#
# To add or change CPU type definitions, edit
# $WIND_BASE/host/src/target/architecturedb
#
# modification history
# --------------------
# 01w,14nov01,fmk  change targetTool() to use wtxTargetToolNameGet
# 01v,07aug01,pch  Rename to wtxcore.in, revise for use in generating
#                  wtxcore.tcl from host/src/target/architecturedb
# 01u,16aug01,hdn  added PENTIUM2/3/4 support
# 01t,10jul01,pch  Fix vxCpuFamily vector for 405/405F/509
# 01s,10jul01,kab  Fixed cpuFamily vector for 405/405F/509
# 01r,07may01,kab  Changed PPC405/405F/509 enum
# 01r,22jun01,tpw  Add targetGdbSuffix to obtain the proper GDB suffix from a
#                  CPU ID.
# 01y,17may01,h_k  changed vxCpuFamily(129) and vxCpuFamily(1120) to ARM
# 01x,09may01,h_k  added for {$cpuFamily($__wtxCpuType) == "thumb"} &&
#                  {[targetEndian] == "BIG_ENDIAN"} in targetTool
# 01w,04may01,scm  add STRONGARM support
# 01v,30apr01,h_k  added for {$cpuFamily($__wtxCpuType) == "arm"} &&
#                  {[targetEndian] == "BIG_ENDIAN"} in targetTool
# 01q,26oct00,s_m  rename 405 cpu types
# 01p,12oct00,sm   added cpu types for 405gp and 405gf
# 01o,31mar99,cmc  Add PPC555 cpu type
# 01u,11dec00,scm  replace ARMSA2 reference with XScale
# 01t,31aug00,scm  add sa2 support...
# 01s,16nov98,cdp  added support for ARMARCH3/ARMARCH4/ARMARCH4_T.
# 01p,20apr00,zl   added sh family support for targetTool
# 01o,15mar00,zl   merged SH support from T1
# 01w,05jan01,mem  Initial changes for MIPS32/MIPS64
# 01v,08may00,dra  Added Venus support.
# 01u,10sep99,myz  merged CW4000 ES FCS3 and added a new cpu type CW4000_16.
# 01q,17mar00,dra  added Coldfire support.
# 01p,11feb98,rs   added MCORE processors and family
# 01o,18may99,rsh  merge wrs-es.tor1_0_1.mcore-f -> T2
# 01p,04nov99,dra  Added sparc64 support.
# 01o,20jan99,dra  Added CW4000, CW4011, VR4100, VR5000 and VR5400 support.
# 01p,05nov97,mem  added SPARCV9 support.
# 01n,24aug98,tpr  added PowerPC EC 603 support.
# 01r,29jul98,j_k  changed references of simpc back to simnt
# 01q,28jul98,fle  changed reference to simnt into simpc
# 01p,01jul98,c_s  WindView 2 -> Tornado 2 Merge
# 01o,27apr98,fle  added PENTIUM and PENTIUMPRO to CPU list
# 01n,19jun97,cdp  added support for Thumb (ARM7TDMI_T).
# 01m,28aug96,cdp  added support for ARM family.
# 01v,19mar98,pdn  added SIMNT support.
# 01u,12mar98,dbt  fixed targetTool() routine for i960 architecture.
# 01t,18mar98,jmb  HPSIM merge of jmb patch from 11jun97
#                  added asmClass for hppa.
# 01s,18mar98,jmb  HPSIM merge of Mark Mason patch from 20nov96:
#                  added more architectures to targetTool.
# 01r,18mar98,jmb  HPSIM merge of Mark Mason patch from 04nov96:
#                  added targetTool proc.
# 01q,06aug97,jmp  added PowerPC 505 and 860 to vxCpuFamily array (SPR #8981).
# 01p,01mar96,ism  added SIMSPARCSOLARIS support.
# 01o,27jun96,kkk  added CW4000 & R4650.
# 01n,17jun96,tpr  added PowerPC 505 and PowerPC 860.
# 01m,23may96,pad  added default CPU number 0 (SPR #6591).
# 01l,12feb96,mem  added MIPS support. added targetEndian proc
# 01k,26jan96,p_m  added I960HX support.
#		   added Copyright line.
# 01j,20dec95,p_m  added vxCpuFamily(24) for I960JX (SPR# 5711).
#		   added comment telling to update all necessary variables
#		   when a new CPU is supported.
# 01i,31oct95,pad  added PPC processors and family.
# 01h,30oct95,s_w  added I960JX *family*
# 01g,28sep95,p_m  added I960JX.
# 01f,12jun95,c_s  added wtxErrorName.
# 01e,15may95,c_s  added wtxObjModuleLoad timeout.
# 01d,03may95,f_v  added vxCpuFamily array
# 01c,24apr95,c_s  took care of 5.2 CPU numbering.
# 01b,06mar95,p_m  took care of 5.2 CPU naming.
# 01a,22jan95,c_s  extracted from shelcore.tcl.
#*/

# cpuType(): a mapping of the numeric CPU type to the VxWorks architecture
# name.

set cpuType(0)		UNKNOWN

# cpuFamily(): a mapping of the numeric CPU family to the VxWorks architecture
# family name.  This name is used as a key to load the proper architecture-
# specific module for shell support (sh-<family>.tcl). 

# vxCpuFamily is used to get loader object format - see wtxCommonProc.tcl

# asmClass(): a mapping of CPU family name to the disassembler format name
# used by memBlockDis.

# cpuGdbSuffix(): a mapping of cpuType names to GDB program suffix strings
# when those differ from the corresponding cpuFamily() entry.  This is used
# by targetGdbSuffix (the GDB version of targetTool).

set cpuType(1)		MC68000
set cpuType(2)		MC68010
set cpuType(3)		MC68020
set cpuType(4)		MC68030
set cpuType(5)		MC68040
set cpuType(6)		MC68LC040
set cpuType(7)		MC68060
set cpuType(8)		CPU32
set cpuType(10)		SPARC
set cpuType(11)		SPARClite
set cpuType(21)		I960CA
set cpuType(22)		I960KA
set cpuType(23)		I960KB
set cpuType(24)		I960JX
set cpuType(25)		I960HX
set cpuType(31)		G100
set cpuType(32)		G200
set cpuType(41)		MIPS32
set cpuType(42)		MIPS64
set cpuType(51)		AM29030
set cpuType(52)		AM29200
set cpuType(53)		AM29035
set cpuType(60)		SIMSPARCSUNOS
set cpuType(61)		SIMSPARCSOLARIS
set cpuType(70)		SIMHPPA
set cpuType(81)		I80386
set cpuType(82)		I80486
set cpuType(83)		PENTIUM
set cpuType(84)		PENTIUM2
set cpuType(85)		PENTIUM3
set cpuType(86)		PENTIUM4
set cpuType(91)		PPC601
set cpuType(92)		PPC602
set cpuType(93)		PPC603
set cpuType(94)		PPC604
set cpuType(95)		PPC403
set cpuType(96)		PPC505
set cpuType(97)		PPC860
set cpuType(98)		PPCEC603
set cpuType(99)		PPC555
set cpuType(100)		SIMNT
set cpuType(111)		ULTRASPARC
set cpuType(121)		ARM710A
set cpuType(122)		ARM7TDMI
set cpuType(123)		ARM810
set cpuType(124)		ARMSA110
set cpuType(125)		ARM7TDMI_T
set cpuType(126)		ARMARCH3
set cpuType(127)		ARMARCH4
set cpuType(128)		ARMARCH4_T
set cpuType(129)		STRONGARM
set cpuType(131)		SH7000
set cpuType(132)		SH7600
set cpuType(133)		SH7040
set cpuType(134)		SH7700
set cpuType(135)		SH7410
set cpuType(136)		SH7729
set cpuType(137)		SH7750
set cpuType(151)		MCORE10
set cpuType(152)		MCORE15
set cpuType(153)		MCORE200
set cpuType(154)		MCORE300
set cpuType(161)		MCF5200
set cpuType(162)		MCF5400
set cpuType(171)		FR500
set cpuType(180)		MAP1000
set cpuType(181)		MAP1000A
set cpuType(182)		MAPCA
set cpuType(1120)		XSCALE
set cpuType(1121)		ARMARCH5
set cpuType(1122)		ARMARCH5_T
set cpuType(2000)		PPC509
set cpuType(2001)		PPC405
set cpuType(2002)		PPC405F
set cpuType(2003)		PPC440
set cpuFamily(1)		m68k
set cpuFamily(2)		m68k
set cpuFamily(3)		m68k
set cpuFamily(4)		m68k
set cpuFamily(5)		m68k
set cpuFamily(6)		m68k
set cpuFamily(7)		m68k
set cpuFamily(8)		m68k
set cpuFamily(10)		sparc
set cpuFamily(11)		sparc
set cpuFamily(21)		i960
set cpuFamily(22)		i960
set cpuFamily(23)		i960
set cpuFamily(24)		i960
set cpuFamily(25)		i960
set cpuFamily(31)		tron
set cpuFamily(32)		tron
set cpuFamily(41)		mips
set cpuFamily(42)		mips
set cpuFamily(51)		am29k
set cpuFamily(52)		am29k
set cpuFamily(53)		am29k
set cpuFamily(60)		simsp
set cpuFamily(61)		simso
set cpuFamily(70)		simhp
set cpuFamily(81)		i86
set cpuFamily(82)		i86
set cpuFamily(83)		i86
set cpuFamily(84)		i86
set cpuFamily(85)		i86
set cpuFamily(86)		i86
set cpuFamily(91)		ppc
set cpuFamily(92)		ppc
set cpuFamily(93)		ppc
set cpuFamily(94)		ppc
set cpuFamily(95)		ppc
set cpuFamily(96)		ppc
set cpuFamily(97)		ppc
set cpuFamily(98)		ppc
set cpuFamily(99)		ppc
set cpuFamily(100)		simnt
set cpuFamily(111)		sparcv9
set cpuFamily(121)		arm
set cpuFamily(122)		arm
set cpuFamily(123)		arm
set cpuFamily(124)		arm
set cpuFamily(125)		thumb
set cpuFamily(126)		arm
set cpuFamily(127)		arm
set cpuFamily(128)		thumb
set cpuFamily(129)		arm
set cpuFamily(131)		sh
set cpuFamily(132)		sh
set cpuFamily(133)		sh
set cpuFamily(134)		sh
set cpuFamily(135)		sh
set cpuFamily(136)		sh
set cpuFamily(137)		sh
set cpuFamily(151)		mcore
set cpuFamily(152)		mcore
set cpuFamily(153)		mcore
set cpuFamily(154)		mcore
set cpuFamily(161)		coldfire
set cpuFamily(162)		coldfire
set cpuFamily(171)		frv
set cpuFamily(180)		map
set cpuFamily(181)		map
set cpuFamily(182)		map
set cpuFamily(1120)		arm
set cpuFamily(1121)		arm
set cpuFamily(1122)		thumb
set cpuFamily(2000)		ppc
set cpuFamily(2001)		ppc
set cpuFamily(2002)		ppc
set cpuFamily(2003)		ppc
set vxCpuFamily(1)		MC680X0
set vxCpuFamily(2)		MC680X0
set vxCpuFamily(3)		MC680X0
set vxCpuFamily(4)		MC680X0
set vxCpuFamily(5)		MC680X0
set vxCpuFamily(6)		MC680X0
set vxCpuFamily(7)		MC680X0
set vxCpuFamily(8)		MC680X0
set vxCpuFamily(10)		SPARC
set vxCpuFamily(11)		SPARC
set vxCpuFamily(21)		I960
set vxCpuFamily(22)		I960
set vxCpuFamily(23)		I960
set vxCpuFamily(24)		I960
set vxCpuFamily(25)		I960
set vxCpuFamily(31)		TRON
set vxCpuFamily(32)		TRON
set vxCpuFamily(41)		MIPS
set vxCpuFamily(42)		MIPS
set vxCpuFamily(51)		AM29XXX
set vxCpuFamily(52)		AM29XXX
set vxCpuFamily(53)		AM29XXX
set vxCpuFamily(60)		SIMSPARCSUNOS
set vxCpuFamily(61)		SIMSPARCSOLARIS
set vxCpuFamily(70)		SIMHPPA
set vxCpuFamily(81)		I80X86
set vxCpuFamily(82)		I80X86
set vxCpuFamily(83)		I80X86
set vxCpuFamily(84)		I80X86
set vxCpuFamily(85)		I80X86
set vxCpuFamily(86)		I80X86
set vxCpuFamily(91)		PPC
set vxCpuFamily(92)		PPC
set vxCpuFamily(93)		PPC
set vxCpuFamily(94)		PPC
set vxCpuFamily(95)		PPC
set vxCpuFamily(96)		PPC
set vxCpuFamily(97)		PPC
set vxCpuFamily(98)		PPC
set vxCpuFamily(99)		PPC
set vxCpuFamily(100)		SIMNT
set vxCpuFamily(111)		SPARCV9
set vxCpuFamily(121)		ARM
set vxCpuFamily(122)		ARM
set vxCpuFamily(123)		ARM
set vxCpuFamily(124)		ARM
set vxCpuFamily(125)		ARM
set vxCpuFamily(126)		ARM
set vxCpuFamily(127)		ARM
set vxCpuFamily(128)		ARM
set vxCpuFamily(129)		ARM
set vxCpuFamily(131)		SH
set vxCpuFamily(132)		SH
set vxCpuFamily(133)		SH
set vxCpuFamily(134)		SH
set vxCpuFamily(135)		SH
set vxCpuFamily(136)		SH
set vxCpuFamily(137)		SH
set vxCpuFamily(151)		MCORE
set vxCpuFamily(152)		MCORE
set vxCpuFamily(153)		MCORE
set vxCpuFamily(154)		MCORE
set vxCpuFamily(161)		COLDFIRE
set vxCpuFamily(162)		COLDFIRE
set vxCpuFamily(171)		FRV
set vxCpuFamily(180)		MAP
set vxCpuFamily(181)		MAP
set vxCpuFamily(182)		MAP
set vxCpuFamily(1120)		ARM
set vxCpuFamily(1121)		ARM
set vxCpuFamily(1122)		ARM
set vxCpuFamily(2000)		PPC
set vxCpuFamily(2001)		PPC
set vxCpuFamily(2002)		PPC
set vxCpuFamily(2003)		PPC
set asmClass(m68k)		m68k
set asmClass(sparc)		sparc
set asmClass(i960)		i960
set asmClass(tron)		tron
set asmClass(mips)		mips
set asmClass(am29k)		am29k
set asmClass(simsp)		sparc
set asmClass(simso)		sparc
set asmClass(simhp)		hppa
set asmClass(hppa)		hppa
set asmClass(i86)		i86
set asmClass(ppc)		ppc
set asmClass(simnt)		simnt
set asmClass(sparcv9)		sparcv9
set asmClass(sparc64)		sparc
set asmClass(arm)		arm
set asmClass(arm)		arm
set asmClass(arm)		arm
set asmClass(thumb)		thumb
set asmClass(sh)		sh
set asmClass(mcore)		mcore
set asmClass(coldfire)		coldfire
set asmClass(frv)		frv
set asmClass(map)		map
set cpuGdbSuffix(MIPS64)		mips64
set cpuGdbSuffix(SIMSPARCSUNOS)		sparc
set cpuGdbSuffix(SIMHPPA)		hppa
set cpuGdbSuffix(ULTRASPARC)		sparc64
set cpuGdbSuffix(ARM7TDMI_T)		thumb
set cpuGdbSuffix(ARMARCH4_T)		thumb
set cpuGdbSuffix(ARMARCH5_T)		thumb

#
# A procedure that returns the error name given a WTX Tcl error result string.
#

proc wtxErrorName {wtxErr} {
    if [regexp {WTX Error [0-9A-Fa-fx]+ \((.*)\).*} $wtxErr all errName] {
	return $errName
    } 
    return ""
}

#
# Give a 2-minute timeout for object module loads.
#

if { ![info exists wtxCmdTimeout(wtxObjModuleLoad)]} {
    set wtxCmdTimeout(wtxObjModuleLoad) 120
}

##############################################################################
#
# targetEndian - determine the endianness of the target.
#
# SYNOPSIS:
#   targetEndian
#
# PARAMETERS:
#   none
#
# RETURNS:
#   BIG_ENDIAN, LITTLE_ENDIAN or UNKNOWN_ENDIAN
#
# ERRORS:

proc targetEndian {} {
    set endian [lindex [lindex [wtxTsInfoGet] 2] 4]
    if {$endian == 1234} {
	return BIG_ENDIAN
    }
    if {$endian == 4321} {
	return LITTLE_ENDIAN
    }
    return UNKNOWN_ENDIAN
}

##############################################################################
#
# targetTool - determine the TOOL= value of the target.
#
# SYNOPSIS:
#   targetTool
#
# PARAMETERS:
#   none
#
# RETURNS:
#   The tool used to compile the target.  Usually "gnu".
#
# INTERNAL:
#   This should really query the target server to determine this info.
#
# ERRORS:

proc targetTool {} {

    return [wtxTargetToolNameGet]
}

##############################################################################
#
# targetGdbSuffix - determine the GDB suffix for this target.
#
# SYNOPSIS:
#   targetGdbSuffix cpuId
#
# PARAMETERS:
#   cpuId: the numeric CPU type of the target in question.
#
# RETURNS:
#   The GDB architecture suffix for this target.  Typically $cpuFamily(cpuId)
#   but is different for some CPU's; these exceptions are centralized here.
#
# ERRORS:

proc targetGdbSuffix { cpuId } {
    global cpuType
    global cpuGdbSuffix
    global cpuFamily

    set cputype $cpuType($cpuId)

    if { [info exists cpuGdbSuffix($cputype)] } {
	return $cpuGdbSuffix($cputype)
    }
    return $cpuFamily($cpuId)
}
