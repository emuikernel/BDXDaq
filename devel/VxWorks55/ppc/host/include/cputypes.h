/* cputypes.h - VxWorks CPU definitions header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
 * cputypes.h is now generated -- DO NOT EDIT
 *
 * To change the boilerplate, edit cputypes.in
 *
 * To add CPU types or otherwise change the substance,
 * edit $WIND_BASE/host/src/target/architecturedb
 */

/*
modification history
--------------------
02d,17oct01,pch  Rename to cputypes.in, revise for use in generating
                 cputypes.h from host/src/target/architecturedb
02c,16aug01,hdn  added PENTIUM2/3/4 support
02b,09jul01,pch  Add PPC405 cases to CPU_FAMILY==PPC
02a,07may01,kab  Fixed PPC enum clash
02a,04may01,scm  add STRONGARM category...
02i,18may01,pai  merged in new Tornado 2.x CPU types
02h,15dec00,s_m  added PowerPC 405 support
02g,15mar01,t_m  fix duplicate ARM definitions
02f,01feb01,t_m  adding xscale
02e,15mar00,zl   merged SH support from T1
02d,05jan01,mem  Initial changes for MIPS32/MIPS64
02c,25sep00,hjg  Rationalised mcf family to 2 cpus.
02b,08may00,dra  Added Venus support.
02a,15mar00,dra  Added Coldfire and SH support.
01z,10sep99,myz  added CW4000_16
01y,19jan98,rs   added MCORE family; CPUs currently supported: MCORE10, MCORE15
01x,18may99,rsh  merge wrs-es.tor1_0_1.mcore-f -> T2
01w,19jan99,dra  Added CW4011, VR4100, VR5000 and VR5400 support.
01v,22sep98,mem  added ULTRASPARC cpu in SPARCV9 family.
01u,05nov97,mem  added SPARCV9 support.
01t,24aug98,tpr  added PowerPC EC 603 support.
01m,13may99,cmc  Added PPC555 support
01s,29jul98,c_c  Changed simpc back to simnt.
01r,28jul98,c_c  Changed simnt to simpc.
01q,17apr98,c_c  ARM merge.
01p,03jun97,cdp  added ARM7TDMI_T.
01o,28aug96,cdp  added ARM.
01n,16apr98,hdn  added support for Pentium and PentiumPro.
01m,30oct97,cym  added SIMNT.
01l,01mar96,ism  added SIMSPARCSOLARIS
01k,29jun96,kkk  added CW4000 & R4650.
01j,16jun96,tpr  added PowerPC 505, PowerPC 602 and PowerPC 860.
01i,12jan96,myz  added I960HX.
01h,31oct95,pad  added PPC processors and family.
01g,28sep95,p_m  added I960JX.
01f,18jul95,p_m  re-installed tron references, they are still used in shell.yac.
01e,18jul95,p_m  added MC68LC040 CPU. removed tron references.
01d,06mar95,p_m  upgraded to 5.2 CPU types numbering.
01c,20jan95,jcf  made more portable.
01b,12nov94,pad  modified cpu numbers to take in account vxSim and simHppa.
01a,08nov94,pad  created
*/

#ifndef __INCcputypesh
#define __INCcputypesh

#ifdef __cplusplus
extern "C" {
#endif

/* CPU types */

/* The Green Hills compiler pre-defines "MC68000"!! */
#ifdef MC68000
#undef MC68000
#endif

/* Also turn off some other stuff that some compilers pre-define ... */
#ifdef I960
#undef I960
#endif
#ifdef PPC
#undef PPC
#endif

/* Ensure CPU_FAMILY undefined initially */
#ifdef CPU_FAMILY
#undef CPU_FAMILY
#endif

/* define CPU to default (SPARC) if undefined */

#ifndef CPU
#define CPU	SPARC
#endif

#define MC68000	1	/* CPU */
#define MC68010	2	/* CPU */
#define MC68020	3	/* CPU */
#define MC68030	4	/* CPU */
#define MC68040	5	/* CPU */
#define MC68LC040	6	/* CPU */
#define MC68060	7	/* CPU */
#define CPU32	8	/* CPU */
#define MC680X0	9	/* CPU_FAMILY */
#define SPARC	10	/* CPU & CPU_FAMILY */
#define SPARClite	11	/* CPU */
#define I960	20	/* CPU_FAMILY */
#define I960CA	21	/* CPU */
#define I960KA	22	/* CPU */
#define I960KB	23	/* CPU */
#define I960JX	24	/* CPU */
#define I960HX	25	/* CPU */
#define TRON	30	/* CPU_FAMILY */
#define G100	31	/* CPU */
#define G200	32	/* CPU */
#define MIPS	40	/* CPU_FAMILY */
#define MIPS32	41	/* CPU */
#define MIPS64	42	/* CPU */
#define AM29XXX	50	/* CPU_FAMILY */
#define AM29030	51	/* CPU */
#define AM29200	52	/* CPU */
#define AM29035	53	/* CPU */
#define SIMSPARCSUNOS	60	/* CPU & CPU_FAMILY */
#define SIMSPARCSOLARIS	61	/* CPU & CPU_FAMILY */
#define SIMHPPA	70	/* CPU & CPU_FAMILY */
#define I80X86	80	/* CPU_FAMILY */
#define I80386	81	/* CPU */
#define I80486	82	/* CPU */
#define PENTIUM	83	/* CPU */
#define PENTIUM2	84	/* CPU */
#define PENTIUM3	85	/* CPU */
#define PENTIUM4	86	/* CPU */
#define PPC	90	/* CPU_FAMILY */
#define PPC601	91	/* CPU */
#define PPC602	92	/* CPU */
#define PPC603	93	/* CPU */
#define PPC604	94	/* CPU */
#define PPC403	95	/* CPU */
#define PPC505	96	/* CPU */
#define PPC860	97	/* CPU */
#define PPCEC603	98	/* CPU */
#define PPC555	99	/* CPU */
#define SIMNT	100	/* CPU & CPU_FAMILY */
#define SPARCV9	110	/* CPU_FAMILY */
#define ULTRASPARC	111	/* CPU */
#define ARM	120	/* CPU_FAMILY */
#define ARM710A	121	/* CPU */
#define ARM7TDMI	122	/* CPU */
#define ARM810	123	/* CPU */
#define ARMSA110	124	/* CPU */
#define ARM7TDMI_T	125	/* CPU */
#define ARMARCH3	126	/* CPU */
#define ARMARCH4	127	/* CPU */
#define ARMARCH4_T	128	/* CPU */
#define STRONGARM	129	/* CPU */
#define SH	130	/* CPU_FAMILY */
#define SH7000	131	/* CPU */
#define SH7600	132	/* CPU */
#define SH7040	133	/* CPU */
#define SH7700	134	/* CPU */
#define SH7410	135	/* CPU */
#define SH7729	136	/* CPU */
#define SH7750	137	/* CPU */
#define MCORE	150	/* CPU_FAMILY */
#define MCORE10	151	/* CPU */
#define MCORE15	152	/* CPU */
#define MCORE200	153	/* CPU */
#define MCORE300	154	/* CPU */
#define COLDFIRE	160	/* CPU_FAMILY */
#define MCF5200	161	/* CPU */
#define MCF5400	162	/* CPU */
#define FRV	170	/* CPU_FAMILY */
#define FR500	171	/* CPU */
#define MAP1000	180	/* CPU & CPU_FAMILY */
#define MAP1000A	181	/* CPU */
#define MAPCA	182	/* CPU */
#define XSCALE	1120	/* CPU */
#define ARMARCH5	1121	/* CPU */
#define ARMARCH5_T	1122	/* CPU */
#define PPC509	2000	/* CPU */
#define PPC405	2001	/* CPU */
#define PPC405F	2002	/* CPU */
#define PPC440	2003	/* CPU */

#if	(CPU==MC68000 || \
	 CPU==MC68010 || \
	 CPU==MC68020 || \
	 CPU==MC68030 || \
	 CPU==MC68040 || \
	 CPU==MC68LC040 || \
	 CPU==MC68060 || \
	 CPU==CPU32)
#define	CPU_FAMILY	MC680X0
#endif	/* CPU_FAMILY==MC680X0 */

#if	(CPU==SPARC || \
	 CPU==SPARClite)
#define	CPU_FAMILY	SPARC
#endif	/* CPU_FAMILY==SPARC */

#if	(CPU==I960CA || \
	 CPU==I960KA || \
	 CPU==I960KB || \
	 CPU==I960JX || \
	 CPU==I960HX)
#define	CPU_FAMILY	I960
#endif	/* CPU_FAMILY==I960 */

#if	(CPU==G100 || \
	 CPU==G200)
#define	CPU_FAMILY	TRON
#endif	/* CPU_FAMILY==TRON */

#if	(CPU==MIPS32 || \
	 CPU==MIPS64)
#define	CPU_FAMILY	MIPS
#endif	/* CPU_FAMILY==MIPS */

#if	(CPU==AM29030 || \
	 CPU==AM29200 || \
	 CPU==AM29035)
#define	CPU_FAMILY	AM29XXX
#endif	/* CPU_FAMILY==AM29XXX */

#if	(CPU==SIMSPARCSUNOS)
#define	CPU_FAMILY	SIMSPARCSUNOS
#endif	/* CPU_FAMILY==SIMSPARCSUNOS */

#if	(CPU==SIMSPARCSOLARIS)
#define	CPU_FAMILY	SIMSPARCSOLARIS
#endif	/* CPU_FAMILY==SIMSPARCSOLARIS */

#if	(CPU==SIMHPPA)
#define	CPU_FAMILY	SIMHPPA
#endif	/* CPU_FAMILY==SIMHPPA */

#if	(CPU==I80386 || \
	 CPU==I80486 || \
	 CPU==PENTIUM || \
	 CPU==PENTIUM2 || \
	 CPU==PENTIUM3 || \
	 CPU==PENTIUM4)
#define	CPU_FAMILY	I80X86
#endif	/* CPU_FAMILY==I80X86 */

#if	(CPU==PPC601 || \
	 CPU==PPC602 || \
	 CPU==PPC603 || \
	 CPU==PPC604 || \
	 CPU==PPC403 || \
	 CPU==PPC505 || \
	 CPU==PPC860 || \
	 CPU==PPCEC603 || \
	 CPU==PPC555 || \
	 CPU==PPC509 || \
	 CPU==PPC405 || \
	 CPU==PPC405F || \
	 CPU==PPC440)
#define	CPU_FAMILY	PPC
#endif	/* CPU_FAMILY==PPC */

#if	(CPU==SIMNT)
#define	CPU_FAMILY	SIMNT
#endif	/* CPU_FAMILY==SIMNT */

#if	(CPU==ULTRASPARC)
#define	CPU_FAMILY	SPARCV9
#endif	/* CPU_FAMILY==SPARCV9 */

#if	(CPU==ARM710A || \
	 CPU==ARM7TDMI || \
	 CPU==ARM810 || \
	 CPU==ARMSA110 || \
	 CPU==ARM7TDMI_T || \
	 CPU==ARMARCH3 || \
	 CPU==ARMARCH4 || \
	 CPU==ARMARCH4_T || \
	 CPU==STRONGARM || \
	 CPU==XSCALE || \
	 CPU==ARMARCH5 || \
	 CPU==ARMARCH5_T)
#define	CPU_FAMILY	ARM
#endif	/* CPU_FAMILY==ARM */

#if	(CPU==SH7000 || \
	 CPU==SH7600 || \
	 CPU==SH7040 || \
	 CPU==SH7700 || \
	 CPU==SH7410 || \
	 CPU==SH7729 || \
	 CPU==SH7750)
#define	CPU_FAMILY	SH
#endif	/* CPU_FAMILY==SH */

#if	(CPU==MCORE10 || \
	 CPU==MCORE15 || \
	 CPU==MCORE200 || \
	 CPU==MCORE300)
#define	CPU_FAMILY	MCORE
#endif	/* CPU_FAMILY==MCORE */

#if	(CPU==MCF5200 || \
	 CPU==MCF5400)
#define	CPU_FAMILY	COLDFIRE
#endif	/* CPU_FAMILY==COLDFIRE */

#if	(CPU==FR500)
#define	CPU_FAMILY	FRV
#endif	/* CPU_FAMILY==FRV */

#if	(CPU==MAP1000 || \
	 CPU==MAP1000A || \
	 CPU==MAPCA)
#define	CPU_FAMILY	MAP
#endif	/* CPU_FAMILY==MAP */

#ifdef __cplusplus
}
#endif

#endif /* __INCcputypesh */
