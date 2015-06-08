# infoLookupTables.tcl - information of CDs, architecures, cpus, bsps
#
# Copyright 2002 Wind River Systems, Inc.
#
# modification history
# ------------------------
# 01e,11jun02,j_s  add TDK-14620 and TDK-14637
# 01d,04jun02,j_s  update TDK information
# 01c,08mar02,j_s  update bsp list
# 01b,04feb02,j_s  in pre-Veloce releases, cpu for pcPentiumpro had been
#                  PENTIUM, not PENTIUM2
# 01a,14jan02,j_s  add TDK # to CD version mapping for Veloce
#
# DESCRIPTION
# This file contains the mapping table between TDK number and Tornado version,
# between bsp and cpu, between cpu and its architecture. The mapping tables
# are required by project migration tool

namespace eval prjMigrate {
    set prjTypeValid {vxApp vxWorks}
    set tcValid {gnu diab}
    set versionCD(TDK-12834-ZC-00) 2.0
    set versionCD(TDK-12835-ZC-00) 2.0
    set versionCD(TDK-12836-ZC-00) 2.0
    set versionCD(TDK-12837-ZC-00) 2.0
    set versionCD(TDK-12838-ZC-00) 2.0
    set versionCD(TDK-12839-ZC-00) 2.0
    set versionCD(TDK-12840-ZC-00) 2.0
    set versionCD(TDK-12841-ZC-00) 2.0
    set versionCD(TDK-12842-ZC-00) 2.0
    set versionCD(TDK-12982-ZC-00) 2.0
    set versionCD(TDK-12834-ZC-01) 2.0
    set versionCD(TDK-12835-ZC-01) 2.0
    set versionCD(TDK-12836-ZC-01) 2.0
    set versionCD(TDK-12837-ZC-01) 2.0
    set versionCD(TDK-12838-ZC-01) 2.0
    set versionCD(TDK-12839-ZC-01) 2.0
    set versionCD(TDK-12840-ZC-01) 2.0
    set versionCD(TDK-12841-ZC-01) 2.0
    set versionCD(TDK-12842-ZC-01) 2.0
    set versionCD(TDK-12834-ZC-02) 2.0.2
    set versionCD(TDK-12835-ZC-02) 2.0.2
    set versionCD(TDK-12837-ZC-02) 2.0.2
    set versionCD(TDK-12838-ZC-02) 2.0.2
    set versionCD(TDK-12839-ZC-02) 2.0.2
    set versionCD(TDK-12840-ZC-02) 2.0.2
    set versionCD(TDK-13937-ZC-00) 2.1
    set versionCD(TDK-14357-ZC-00) 2.2
    set versionCD(TDK-14358-ZC-00) 2.2
    set versionCD(TDK-14359-ZC-00) 2.2
    set versionCD(TDK-14360-ZC-00) 2.2
    set versionCD(TDK-14361-ZC-00) 2.2
    set versionCD(TDK-14362-ZC-00) 2.2
    set versionCD(TDK-14363-ZC-00) 2.2
    set versionCD(TDK-14364-ZC-00) 2.2
    set versionCD(TDK-14365-ZC-00) 2.2
    set versionCD(TDK-14380-ZC-00) 2.2
    set versionCD(TDK-14381-ZC-00) 2.2
    set versionCD(TDK-14382-ZC-00) 2.2
    set versionCD(TDK-14383-ZC-00) 2.2
    set versionCD(TDK-14384-ZC-00) 2.2
    set versionCD(TDK-14385-ZC-00) 2.2
    set versionCD(TDK-14386-ZC-00) 2.2
    set versionCD(TDK-14387-ZC-00) 2.2
    set versionCD(TDK-14620-ZC-00) 2.2
    set versionCD(TDK-14621-ZC-00) 2.2
    set versionCD(TDK-14622-ZC-00) 2.2
    set versionCD(TDK-14623-ZC-00) 2.2
    set versionCD(TDK-14624-ZC-00) 2.2
    set versionCD(TDK-14625-ZC-00) 2.2
    set versionCD(TDK-14626-ZC-00) 2.2
    set versionCD(TDK-14627-ZC-00) 2.2
    set versionCD(TDK-14628-ZC-00) 2.2
    set versionCD(TDK-14630-ZC-00) 2.2
    set versionCD(TDK-14631-ZC-00) 2.2
    set versionCD(TDK-14632-ZC-00) 2.2
    set versionCD(TDK-14633-ZC-00) 2.2
    set versionCD(TDK-14634-ZC-00) 2.2
    set versionCD(TDK-14635-ZC-00) 2.2
    set versionCD(TDK-14636-ZC-00) 2.2
    set versionCD(TDK-14637-ZC-00) 2.2
    set cpuList(ads8260) PPCEC603
    set cpuList(ads860) PPC860
    set cpuList(assabet) PPC860
    set cpuList(atlas4kc) MIPS32
    set cpuList(atlas5261) MIPS64
    set cpuList(bcm1250_cpu0) MIPS64
    set cpuList(bcm1250_cpu1) MIPS64
    set cpuList(bdmr4011) CW4011
    set cpuList(brutus) STRONGARM
    set cpuList(cma220) ARMARCH3
    set cpuList(cma221) ARMARCH4
    set cpuList(cp7000) MIPS64
    set cpuList(cpn5360) PENTIUM3
    set cpuList(cpv3060) PPC860
    set cpuList(cpv5000) PENTIUM
    set cpuList(cpv5300) PENTIUM2
    set cpuList(cpv5350) PENTIUM3
    set cpuList(ddb5074) MIPS64
    set cpuList(ddb5476) MIPS64
    set cpuList(ddb5477) MIPS64
    set cpuList(ebsa1500) STRONGARM
    set cpuList(ebsa285) STRONGARM
    set cpuList(ebony) PPC440
    set cpuList(ev64120) MIPS64
    set cpuList(evb403) PPC403
    set cpuList(evb555) PPC555
    set cpuList(evb7055f) SH7055
    set cpuList(evsM200) MCORE15
    set cpuList(fads302) MC68000
    set cpuList(fads302en) MC68000
    set cpuList(fr500eb) FR500
    set cpuList(fr500vdk) FR500
    set cpuList(hmse7751) SH7750
    set cpuList(hsisbcarm7) ARMARCH4
    set cpuList(iacsflP2) PENTIUM2
    set cpuList(iacsflP3) PENTIUM3
    set cpuList(idp5200) MCF5200
    set cpuList(idt79s334a) MIPS32
    set cpuList(integrator720t) ARMARCH4
    set cpuList(integrator740t) ARMARCH4
    set cpuList(integrator7t) ARMARCH4
    set cpuList(integrator920t) ARMARCH4
    set cpuList(integrator940t) ARMARCH4
    set cpuList(integrator720t_t) ARMARCH4_T
    set cpuList(integrator740t_t) ARMARCH4_T
    set cpuList(integrator7t_t) ARMARCH4_T
    set cpuList(integrator920t_t) ARMARCH4_T
    set cpuList(integrator940t_t) ARMARCH4_T
    set cpuList(integrator946es) ARMARCH5
    set cpuList(integrator946es_t) ARMARCH5_T
    set cpuList(integrator966es) ARMARCH4
    set cpuList(integrator966es_t) ARMARCH4_T
    set cpuList(iq80310) XSCALE
    set cpuList(ixm1200) STRONGARM
    set cpuList(ixp1200eb) STRONGARM
    set cpuList(ixp1200eb_be) STRONGARM
    set cpuList(keuka) PPC405F
    set cpuList(lsi4101) CW4000_16
    set cpuList(m5206ec3) MCF5200
    set cpuList(m5272c3) MCF5200
    set cpuList(m5307c3) MCF5200
    set cpuList(m5407c3) MCF5400
    set cpuList(malta) MIPS32
    set cpuList(malta4kc) MIPS32
    set cpuList(malta4kec) MIPS32
    set cpuList(malta5kc) MIPS32
    set cpuList(mbx860) PPC860
    set cpuList(mcp750) PPC604
    set cpuList(mcpn750) PPC604
    set cpuList(mcpn765) PPC604
    set cpuList(mmccmb1200) MCORE15
    set cpuList(ms7615se) SH7600
    set cpuList(ms7622se) SH7600
    set cpuList(ms7709Ase) SH7700
    set cpuList(ms7727se) SH7700
    set cpuList(ms7729se) SH7700
    set cpuList(ms7729se_le) SH7700
    set cpuList(ms7750se) SH7700
    set cpuList(ms7750se_le) SH7700
    set cpuList(ms7751se) SH7700
    set cpuList(ms7751seT) SH7700
    set cpuList(ms7751se_le) SH7700
    set cpuList(mtx603) PPC603
    set cpuList(mtx604) PPC604
    set cpuList(mv147) MC68020
    set cpuList(mv162) MC68040
    set cpuList(mv162lc) MC68LC040
    set cpuList(mv167) MC68040
    set cpuList(mv172) MC68060
    set cpuList(mv177) MC68060
    set cpuList(mv2100) PPC603
    set cpuList(mv2700) PPC604
    set cpuList(mv2303) PPC603
    set cpuList(mv2304) PPC604
    set cpuList(mv2400) PPC604
    set cpuList(mv2603) PPC603
    set cpuList(mv3603) PPC603
    set cpuList(mv2604) PPC604
    set cpuList(mv3604) PPC604
    set cpuList(mv5100) PPC604
    set cpuList(pb1000) MIPS32
    set cpuList(pc386) I80386
    set cpuList(pc486) I80486
    set cpuList(pcPentium) PENTIUM
    set cpuList(pcPentiumpro) PENTIUM
    set cpuList(pcPentium2) PENTIUM2
    set cpuList(pcPentium3) PENTIUM3
    set cpuList(pcPentium4) PENTIUM4
    set cpuList(pid740t) ARMARCH4
    set cpuList(pid920t) ARMARCH4
    set cpuList(pid940t) ARMARCH4
    set cpuList(pid7t) ARMARCH4
    set cpuList(pid7t_be) ARMARCH4
    set cpuList(pid7t_t) ARMARCH4_T
    set cpuList(pid7t_tbe) ARMARCH4_T
    set cpuList(pid740t_t) ARMARCH4_T
    set cpuList(pid920t_t) ARMARCH4_T
    set cpuList(pid940t_t) ARMARCH4_T
    set cpuList(ppmc750) PPC604
    set cpuList(prpmc800) PPC604
    set cpuList(prpmc600) PPC603
    set cpuList(rh5432) MIPS64
    set cpuList(rh5500) MIPS64
    set cpuList(rockHopper) MIPS64
    set cpuList(sb1250) MIPS64
    set cpuList(sdb4122eagle) MIPS32
    set cpuList(sdb4131eagle) MIPS32
    set cpuList(simpc) SIMNT
    set cpuList(solaris) SIMSPARCSOLARIS
    set cpuList(sp7400) PPC604
    set cpuList(sp745x) PPC604
    set cpuList(sp74xx) PPC604
    set cpuList(sp750) PPC604
    set cpuList(sp755) PPC604
    set cpuList(sp7xx) PPC604
    set cpuList(sp8240) PPC603
    set cpuList(sp824x) PPC603
    set cpuList(spruce) PPC604
    set cpuList(umeteor) MIPS32
    set cpuList(vmware) PENTIUM
    set cpuList(walnut) PPC405
    set cpuList(wrMdpBasic8xx) PPC860
    set cpuList(wrMdpPro8xx) PPC860
    set cpuList(wrPpmc4kc) MIPS32
    set cpuList(wrPpmc74xx) PPC604
    set cpuList(wrPpmc750cx) PPC604
    set cpuList(wrPpmc7xx) PPC604
    set cpuList(wrPpmc8260) PPC603
    set cpuList(wrPpmci80310) XSCALE
    set cpuList(wrSbcArm7) ARMARCH4
    set cpuList(wrSbcArm7_be) ARMARCH4
    set cpuList(wrSbcArm7_t) ARMARCH4_T
    set cpuList(wrSbcArm7_tbe) ARMARCH4_T
    set cpuList(wrSbc360) CPU32
    set cpuList(wrSbc405gp) PPC405
    set cpuList(wrSbc824x) PPC603
    set cpuList(wrSbc8255) PPC603
    set cpuList(wrSbc8260) PPC603
    set cpuList(wrSbc8260Atm) PPC603
    set cpuList(wrScout) PPC604
    set cpuList(yk750) PPC604

    set archOf(PPCEC603) ppc
    set archOf(PPC860) ppc
    set archOf(PPC403) ppc
    set archOf(PPC555) ppc
    set archOf(PPC405F) ppc
    set archOf(PPC603) ppc
    set archOf(PPC604) ppc
    set archOf(PPC405) ppc
    set archOf(MIPS32) mips
    set archOf(MIPS64) mips
    set archOf(CW4011) mips
    set archOf(CW4000_16) mips
    set archOf(STRONGARM) arm
    set archOf(ARMARCH3) arm
    set archOf(ARMARCH4) arm
    set archOf(ARMARCH4_T) arm
    set archOf(I80386) i86
    set archOf(I80486) i86
    set archOf(PENTIUM) i86
    set archOf(PENTIUM2) i86
    set archOf(PENTIUM3) i86
    set archOf(PENTIUM4) i86
    set archOf(SH7055) sh
    set archOf(SH7750) sh
    set archOf(SH7600) sh
    set archOf(SH7700) sh
    set archOf(MCORE15) mcore
    set archOf(MC68000) mc68k
    set archOf(MC68020) mc68k
    set archOf(MC68040) mc68k
    set archOf(MC68LC040) mc68k
    set archOf(MC68060) mc68k
    set archOf(MCF5200) coldFire
    set archOf(MCF5400) coldFire
    set archOf(XSCALE) arm
    set archOf(FR500) frv
    set archOf(SIMNT) simnt
    set archOf(SIMSPARCSOLARIS) simsolaris
}
