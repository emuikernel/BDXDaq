# memAdrs.tcl  - Tcl script wich return text,data,bss address in arrayName
#		 it returns too cpu name, tool name, cpu number
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01d,28jan98,p_b  Adapted for WTX 2.0
# 01c,10may95,f_v     added MC68020 part
# 01b,05may95,f_v     added flag parameter.
# 01a,10apr95,f_v     written.
#

proc memAdrs { arrayName {flag 0}} {
  upvar $arrayName start

  set info [wtxTsInfoGet]
  set cpuNb [lindex [lindex $info 2] 0]
  
  case $cpuNb in {

     { 3  } {    set start(text) 0x070000
		 set start(data) 0x170000
		 set start(bss)  0x270000
		 set cpuName MC68020
		 set toolName gnu
	     }

     { 5  } {    set start(text) 0x070000
		 set start(data) 0x170000
		 set start(bss)  0x270000
		 set cpuName MC68040
		 set toolName gnu
	     }

     { 10 } {    set start(text) 0x170000
		 set start(data) 0x270000
		 set start(bss)  0x370000
		 set cpuName SPARC
		 set toolName gnu
	     }

     { 60 } {    set start(text) 0x170000
		 set start(data) 0x270000
		 set start(bss)  0x370000
		 set cpuName SIMSPARCSUNOS
		 set toolName gnu
	     }

     { 21 } {    set start(text) 0x090000
		 set start(data) 0x190000
		 set start(bss)  0x290000
		 set cpuName I960CA
		 set toolName gnucoff
	     }

     { 52 } {    set start(text) 0x4009cd2c
		 set start(data) 0x4019cd2c
		 set start(bss)  0x4029cd2c
		 set cpuName AM29200
		 set toolName gnucoff
	     }

      default  { error "ERROR : This CPU is undefined "}

  }

  if { $flag == 0} {
      return "$cpuName $toolName $cpuNb" 
  } else {
      return $cpuNb
  }
}
