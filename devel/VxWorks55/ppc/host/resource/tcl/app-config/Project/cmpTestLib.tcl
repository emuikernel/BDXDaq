# cmpTestLib.tcl - component test library
#
# some tests build on top of the scripting libarary
# cmpScriptLib.tcl

source [wtxPath host resource tcl app-config Project cmpScriptLib.tcl]
set cmpInteractive true

# configuration paramters

set bspFile mv2604
set bspFile mv177
set bspFile pcPentium
set bspFile pc386
set bspFile pid7t
set bspFile mv1300
set bspFile solaris
set bspFile mv162
set bspFile ads860
set bspFile diab1604
set bspFile mv1604
set bspFile ep960jx
set cmpTgtsvr ""
if {[wtxHostType] == "x86-win32"} {set bspFile simpc}
if {[wtxHostType] == "parisc-hpux10"} {set bspFile hpux}

# create the project file (or just open if it already exists)

cmpProjCreate $bspFile

###############################################################################
#
# cmpSizeTest - 
# 

proc cmpSizeTest {} {
    set hProj [cmpProjHandleGet]
    set d [mxrDocCreate $hProj]
    set s [$d setCreate]
    $s = udpLib.o
    $s = [mxrSubtree $s]
    puts "size of udpLib.o subtree = [mxrSizeGet $s]"
    set numInts 0
    foreach sym [$s get declares] {
	$s = $sym
	if {[$s get size] == 4} {
	    incr numInts
	} else {
	    puts "[$s contents] has size [$s get size]"
	}
    }
    puts "numInts = $numInts"
    $s delete
}

###############################################################################
#
# cxrConfigAllDiff - check the component library contents vs configAll.h
#

proc cxrConfigAllDiff {} {
    set hProj [cmpProjHandleGet]
    set cxrDoc [cxrDocCreate $hProj]
    set components [$cxrDoc setCreate]
    $components = [$cxrDoc instances Component]

    set fd [open [wtxPath]/target/config/all/configAll.h r]
    set includes [$cxrDoc setCreate]
    foreach line [split [read $fd] \n] {
        if {[regexp {INCLUDE_[A-Z_0-9]*} $line macro] != 0} {
            $includes = [$includes + $macro]
        }
    }
    close $fd

    puts "obsolete macros: [$includes - [$components contents]]"
    puts "new components: [$components - [$includes contents]]"
}

###############################################################################
#
# cmpImageCheck - check an image
#

proc cmpImageCheck {} {
    set hProj [cmpProjHandleGet]
    set tc [[cmpProjTcGet]::name]
    set build [prjBuildCurrentGet $hProj]
    set image [file dirname [prjInfoGet $hProj fileName]]/$build/vxWorks

    set d [modAnalyze $image $tc]
    set syms [modInfo $d [$d instances Module] exports]
    set mxrDoc [mxrDocCreate $hProj]
    set modules [lsort [modInfo $mxrDoc $syms exportedBy]]
    $d delete

    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]    
    $cxrSet = $modules
    $cxrSet = [$cxrSet get _MODULES]
    set extraComponents [$cxrSet - [cmpListGet]]

    puts "Analysis of $image:"
    puts "modules = $modules"
    puts "extra components = $extraComponents"
}

###############################################################################
#
# bspProjImageDiff - find differences between a BSP and project vxWorks image
#


proc bspProjImageDiff {} {
    set hProj [cmpProjHandleGet]
    set tc [cmpProjTcGet]
    set bspDir [prjTagDataGet $hProj BSP_DIR]
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set vxBsp $bspDir/vxWorks
    set vxPrj $prjDir/default/vxWorks

    puts "reading vxWorks archive module database..."
    set mxrDoc [mxrDocCreate $hProj]
    puts "reading $vxBsp symbols..."
    set bspDoc [mxrDataDocCreate $tc $vxBsp]
    puts "reading $vxPrj symbols..."
    set prjDoc [mxrDataDocCreate $tc $vxPrj]
    puts "done"

    set bspSet [$mxrDoc setCreate]
    $bspSet = [$bspDoc instances]
    set bspSyms [$bspDoc instances Symbol]
    $bspSet = [$bspSet get exportedBy]

    set prjSet [$mxrDoc setCreate]
    $prjSet = [$prjDoc instances]
    set prjSyms [$prjDoc instances Symbol]
    $prjSet = [$prjSet get exportedBy]

    set bspExtra [$mxrDoc setCreate]

    puts "\nmodules in project, but not in BSP:"
    puts "[lsort [$prjSet - [$bspSet instances]]]"

    puts "\nmodules in BSP, but not in project:"
    $bspExtra = [$bspSet - [$prjSet instances]]
    puts "[lsort [$bspExtra instances]]"

    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set bspCmpSet [$cxrDoc setCreate]
    set prjCmpSet [$cxrDoc setCreate]
    $bspCmpSet = [$bspSet instances]
    $bspCmpSet = [$bspCmpSet get _MODULES]
    $prjCmpSet = [$prjSet instances]
    $prjCmpSet = [$prjCmpSet get _MODULES]

    puts "\ncomponents in BSP, but not in project:"
    set extraBspCmps [lsort [$bspCmpSet - [$prjCmpSet instances]]]
    puts $extraBspCmps

    puts "\ncomponents in project, but not in BSP:"
    set extraPrjCmps [lsort [$prjCmpSet - [$bspCmpSet instances]]]
    puts $extraPrjCmps

    puts "\n Analyzing extra project components..."
    foreach component $extraPrjCmps {
	$cxrSet = $component
	$cxrSet = [cxrSubtree $cxrSet $mxrDoc]
	puts "$component drags in [$cxrSet - [$bspCmpSet instances]]"
    }

    puts "\nsymbols in project, but not in BSP:"
    $prjSet = $prjSyms
    $bspSet = $bspSyms
    puts "[lsort [$prjSet - [$bspSet contents]]]"

    puts "\nsymbols in BSP, but not in project:"
    puts "[lsort [$bspSet - [$prjSet contents]]]"

    $cxrSet	delete
    $prjCmpSet	delete
    $bspSet	delete
    $bspExtra	delete
    $prjSet	delete
    $bspDoc	delete
    $prjDoc	delete
}

proc imageDiff {obj1 obj2 cpu} {
    set archive [wtxPath target lib lib${cpu}gnuvx.a]

    puts "reading vxWorks archive module database..."
    set mxrDoc [modAnalyze $archive ${cpu}gnu]
    puts "reading $obj1 symbols..."
    set obj1Doc [modAnalyze $obj1 ${cpu}gnu]
    puts "reading $obj2 symbols..."
    set obj2Doc [modAnalyze $obj2 ${cpu}gnu]
    puts "done"

    set obj1Set [$mxrDoc setCreate]
    $obj1Set = [$obj1Doc instances Symbol]	;# obj1Set = symbols in obj1
    $obj1Set = [$obj1Set  get exportedBy]	;# obj1Set = modules in obj1

    set obj2Set [$mxrDoc setCreate]
    $obj2Set = [$obj2Doc instances Symbol]	;# obj2Set = symbols in obj2
    $obj2Set = [$obj2Set  get exportedBy]	;# obj2Set = modules in obj2

    puts "modules in $obj1 and not in $obj2 = [$obj1Set - [$obj2Set instances]]"
    puts "modules in $obj2 and not in $obj1 = [$obj2Set - [$obj1Set instances]]"

    $obj1Set	delete
    $obj2Set	delete
    $obj1Doc	delete
    $obj2Doc	delete
    $mxrDoc	delete
}

###############################################################################
# 
# mxrSubtreeTestSyn - run a few examples of mxrSubtreeTest 
#

proc mxrSubtreeTestSyn {} {
    
#    set archList "ARM7TDMI R3000 I960JX"
    set archList "R3000"
    foreach arch $archList {
	puts "Start mxrSubtreeTest for $arch"
	set d [mxrDataDocCreate tc_${arch}gnu [wtxPath]/target/lib/lib${arch}gnuvx.a]
	set modList [$d instances Module]
	foreach md $modList {
	    mxrSubtreeTest $md ${arch}
	}
	$d delete
	
    }

}

###############################################################################
#
# mxrSupertreeTestSyn - run a few examples of mxrSupertreeTest
#

proc mxrSupertreeTestSyn {} {
    set archList "R3000"
    foreach arch $archList {
	puts "Start mxrSupertreeTest for $arch"
	set d [mxrDataDocCreate tc_${arch}gnu [wtxPath]/target/lib/lib${arch}gnuvx.a]
	set modList [$d instances Module]
	foreach md $modList {
	    mxrSupertreeTest $md ${arch}
	}
	$d delete
    }
}

###############################################################################
#
# componentSizeTestSyn - run componentSizeTest for all architectures
#

proc componentSizeTestSyn {} {
    set bspList "mv177 mv2604"
    set cmpList1 "INCLUDE_KERNEL"
    set cmpList2 "FOLDER_KERNEL FOLDER_NETWORK"
    set cmpList3 "FOLDER_KERNEL FOLDER_NETWORK FOLDER_TOOLS"
    foreach bsp $bspList {
	cmpProjCreate $bsp
	set hProj [cmpProjHandleGet]
	set mxrDoc [mxrDocCreate $hProj]
	set cxrDoc [cxrDocCreate $hProj]
	componentSizeTest $cmpList1 $mxrDoc $cxrDoc
	componentSizeTest $cmpList2 $mxrDoc $cxrDoc
	componentSizeTest $cmpList3 $mxrDoc $cxrDoc

	    }
    $mxrDoc delete
    $cxrDoc delete
}

set hProj [cmpProjHandleGet]
if {"$cmpTgtsvr" != ""} {
    catch "wtxToolAttach $cmpTgtsvr"
}

