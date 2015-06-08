#!/bin/sh

# genScript.sh
#
# a shell script to generate linker scripts.
#
# TODO:
# 1. convenience labels like etext, edata, end, and wrs_kernel_*_{start,end}
#    are primarily used by ROM kernels and can most likely be omitted from RAM
#    kernels. But they're harmless and it makes script sharing easier.
# 2. "classic" convenience labels (etext/edata/end) should be phased out in
#    favor of the newer ones. Further, DAT has requested that we make the new
#    names conform to wrsInitialCapitals style.
# 3. Due to the template generation strategy employed here, the final output
#    may look a little awkwardly formatted and certainly is not as clean and
#    simple as the early versions of the Diab scripts. Suggestions welcome.
#    I actually think this might not be such a bad application for (gasp!) Tcl,
#    so a future Tcl rewrite (which also helps windows users) is possible.
# 4. Currently GNU aligns everything to 16. This is probably overkill on many
#    chips, and especially once everyone is ELF, the internal section alignment
#    should take care of things -- except for convenience labels which need to
#    guarantee 4 byte alignment for use with copyLongs and fillLongs.
# 5. General convergence...

mod_history()
{
cat <<'EOF'
/*
modification history
--------------------
01l,29may02,tpw  Diab: pad out sections, needed for MIPS byteswap. SPR 78076
01k,24apr02,sn   GNU: SPR 75835 - added support for relocatable script link.OUT
01j,19apr02,tpw  Diab: provide _etext for ColdFire. SPR 75122
01i,06feb02,sn   Both: place .got2 to handle tramp.o (_x_gnu_tramp.o).
01h,25jan02,tpw  GNU: give debug sections explicit 0 addresses.
01g,14jan02,tpw  Diab: place .eh_frame data, in case of insistent GNU objects.
01f,14dec01,tpw  Diab: provide both underscore options of wrs_kernel* here too.
01e,05dec01,tpw  Both: Add .boot and .reset section support, for PPC 4xx series
                 and Book E.
01d,05dec01,tpw  Diab: Add .frame_info section to text region.
01c,03dec01,jab  Diab: replaced LOAD() with AT()
01b,14nov01,tpw  ALL: Correct some more bootrom/kernel issues.
01b,06nov01,tpw  GNU: Add /DISCARD/ directive to discard uninteresting sections
01a,01nov01,tpw  gnu script written
01a,29oct01,jab  diab script written
*/

EOF
}

filename="$1"
TOOL="$2"
shift 2
style="$*"

align=16
am1=`expr $align - 1`

bind=""
[ "$TOOL" = diab ] && bind=BIND
dot_boot_text="
  .boot $bind(wrs_kernel_text_start + wrs_kernel_rom_size - 0x800) :
  {
  *(.boot)
  }
  .reset $bind(wrs_kernel_text_start + wrs_kernel_rom_size - 4) :
  {
  *(.reset)
  }
"

diab_data_group_text="  }
  GROUP BIND(_VX_START_DATA) : {"

named_debug_sections="
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }"

rom_at=""
dot_boot=""
diab_data_group=""

zero=""
wrs_kernel_text_start="wrs_kernel_text_start = .; _wrs_kernel_text_start = .;"
wrs_kernel_text_end="wrs_kernel_text_end = .; _wrs_kernel_text_end = .;"
etext="etext = .; _etext = .;"
wrs_kernel_data_start="wrs_kernel_data_start = .; _wrs_kernel_data_start = .;"
wrs_kernel_data_end="wrs_kernel_data_end = .; _wrs_kernel_data_end = .;"
edata="edata = .; _edata = .;"
wrs_kernel_bss_start="wrs_kernel_bss_start = .; _wrs_kernel_bss_start = .;"
wrs_kernel_bss_end="wrs_kernel_bss_end = .; _wrs_kernel_bss_end = .;"
end="end = .; _end = .;"

gp_mips_diab="_gp = ((. + $am1) & ~$am1) + 0x7ff0;"
gp_mips_gnu="_gp = ALIGN(16) + 0x7ff0;		/* set gp for MIPS startup code */"

sec_align_diab=". = ((. + 3) & ~3);"
sec_align_gnu=". = ALIGN($align);"

for arg
do
    case "$arg" in
	AOUT)		named_debug_sections=""				;;
	ROM)		diab_data_group="$diab_data_group_text"		;
			rom_at=" AT(etext)"				;;
	RAM)		;;
	DOTBOOT)	dot_boot="$dot_boot_text"			;;
	OUT)            zero="0"
			wrs_kernel_text_start=""
			wrs_kernel_text_end=""
			etext=""
			wrs_kernel_data_start=""
			wrs_kernel_data_end=""
			edata=""
			wrs_kernel_bss_start=""
			wrs_kernel_bss_end=""
			end=""
			gp_mips_gnu=""
			gp_mips_diab=""
			sec_align_gnu=""
			sec_align_diab=""
			;;
	*)		echo "$0: '$arg' unknown." 1>&2	;	exit 1	;;
    esac
done

cat <<EOF
/* $filename - $TOOL linker command file for VxWorks: $style */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/* This file was automatically generated using tool/common/ldscripts. */

EOF

mod_history

if [ "$TOOL" = diab ]; then
cat <<EOF
SECTIONS {
  GROUP BIND(_VX_START_TEXT) : {
    .text (TEXT) : {
      $wrs_kernel_text_start
      *(.text) *(.rdata) *(.rodata)
      *(.init) *(.fini)
      *(.frame_info)
      *(.sdata2)
      $sec_align_diab
    }
    $sec_align_diab
    $etext
    $wrs_kernel_text_end
$diab_data_group
    .data (DATA)$rom_at : {
      $wrs_kernel_data_start
      *(.data)
      *(.eh_frame)	/* Some GNU objects insist on having .eh_frame data. */
      *(.ctors)
      *(.dtors)
      /* set gp for MIPS startup code */
      $gp_mips_diab
      *(.got2)
      *(.sdata)
      $sec_align_diab
    }
    $sec_align_diab
    $edata
    $wrs_kernel_data_end
    .bss (BSS) : {
      $wrs_kernel_bss_start
      *(.sbss)
      *(.bss)
      $sec_align_diab
    }
    $sec_align_diab
    $end
    $wrs_kernel_bss_end
  }
$dot_boot}
EOF
else
# presume $TOOL = gnu
cat <<EOF
ENTRY(_start)
SECTIONS
{
  .text     $zero :
  {
    $wrs_kernel_text_start
    *(.text) *(.text.*) *(.stub) *(.gnu.warning) *(.gnu.linkonce.t*)
    KEEP(*(.init)) KEEP(*(.fini))
    *(.glue_7t) *(.glue_7) *(.rdata)			/* for ARM */
    *(.mips16.fn.*) *(.mips16.call.*) *(.reginfo)	/* for MIPS */
    *(.rodata) *(.rodata.*) *(.gnu.linkonce.r*) *(.rodata1)
    *(.sdata2) *(.sbss2)				/* for PPC */
    $sec_align_gnu
  }
  $sec_align_gnu
  $wrs_kernel_text_end
  $etext
  .data   $zero :$rom_at
  {
    $wrs_kernel_data_start
    *(.data) *(.data.*) *(.gnu.linkonce.d*) SORT(CONSTRUCTORS) *(.data1)
    *(.eh_frame) *(.gcc_except_table)
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    $gp_mips_gnu
    /* got*, dynamic, sdata*, lit[48], and sbss should follow _gp */
    *(.got.plt) *(.got) *(.dynamic)
    *(.got2)
    *(.sdata) *(.sdata.*) *(.lit8) *(.lit4)
    $sec_align_gnu
  }
  $sec_align_gnu
  $edata
  $wrs_kernel_data_end
  .bss     $zero  :
  {
    $wrs_kernel_bss_start
    *(.sbss) *(.scommon) *(.dynbss) *(.bss) *(COMMON)
    $sec_align_gnu
  }
  $sec_align_gnu
  $end
  $wrs_kernel_bss_end
$dot_boot
  /DISCARD/ :
  {
  *(.note)				/* seen on Pentium ELF */
  *(.comment)				/* seen on PPC & SH ELF */
  *(.pdr)				/* seen on MIPS ELF */
  }$named_debug_sections
}
EOF
fi
