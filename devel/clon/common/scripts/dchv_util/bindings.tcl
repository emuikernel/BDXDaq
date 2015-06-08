#TYPE0 BINDINGS
proc type0_bind {} {
global env clonbin
source $clonbin/dchv_util/global.h


$c bind $type0_tagmas(1) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(1)
$c create text 400 8.2c -text "$type0_tagmas(1)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(1) <Any-Leave> {$c itemconfigure current -fill $coltype0_fa
set det vhg
$c delete item
}

$c bind $type0_tagmas(2) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(2)
$c create text 400 8.2c -text "$type0_tagmas(2)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(2) <Any-Leave> {$c itemconfigure current -fill $coltype0_fs
set det vhg
$c delete item
}

$c bind $type0_tagmas(3) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(3)
$c create text 400 8.2c -text "$type0_tagmas(3)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(3) <Any-Leave> {$c itemconfigure current -fill $coltype0_sa
set det vhg
$c delete item
}

$c bind $type0_tagmas(4) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(4)
$c create text 400 8.2c -text "$type0_tagmas(4)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(4) <Any-Leave> {$c itemconfigure current -fill $coltype0_ga
set det vhg
$c delete item
}

$c bind $type0_tagmas(5) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(5)
$c create text 400 8.2c -text "$type0_tagmas(5)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(5) <Any-Leave> {$c itemconfigure current -fill $coltype0_ss
set det vhg
$c delete item
}

$c bind $type0_tagmas(6) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(6)
$c create text 400 8.2c -text "$type0_tagmas(6)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(6) <Any-Leave> {$c itemconfigure current -fill $coltype0_gs
set det vhg
$c delete item
}
$c bind $type0_tagmas(7) <Any-Enter> {$c itemconfigure current -fill SeaGreen1
set det $type0_tagmas(7)
$c create text 400 8.2c -text "$type0_tagmas(7)" -fill navyblue -anchor n -tags item
}
$c bind $type0_tagmas(7) <Any-Leave> {$c itemconfigure current -fill $coltype0_gs
set det vhg
$c delete item
}
}
proc type0_unbind {} {
global env clonbin
source $clonbin/dchv_util/global.h

for {set i 1} {$i<=7} {incr i } {
$c bind $type0_tagmas($i) <Any-Enter> " "
$c bind $type0_tagmas($i) <Any-Leave> " "
}
}



#TYPE1 BINDINGS
proc type1_bind {} {
global env clonbin
source $clonbin/dchv_util/global.h

$c bind $type1_tagmas(1) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type1_tagmas(1)
$c create text 400 8.2c -text "$type1_tagmas(1)" -fill navyblue -anchor n -tags item
}
$c bind $type1_tagmas(1) <Any-Leave> {$c itemconfigure current -outline $coltype1_r3
set det vhg
$c delete item
}

$c bind $type1_tagmas(2) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type1_tagmas(2)
$c create text 400 8.2c -text "$type1_tagmas(2)" -fill navyblue -anchor n -tags item
}
$c bind $type1_tagmas(2) <Any-Leave> {$c itemconfigure current -outline $coltype1_r2
set det vhg
$c delete item
}

$c bind $type1_tagmas(3) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type1_tagmas(3)
$c create text 400 8.2c -text "$type1_tagmas(3)" -fill navyblue -anchor n -tags item
}
$c bind $type1_tagmas(3) <Any-Leave> {$c itemconfigure current -outline $coltype1_r1
set det vhg
$c delete item
}
}
proc type1_unbind {} {
global env clonbin
source $clonbin/dchv_util/global.h

for {set i 1} {$i<=3} {incr i } {
$c bind $type1_tagmas($i) <Any-Enter> " "
$c bind $type1_tagmas($i) <Any-Leave> " "
}
}



#TYPE2 BINDINGS
proc type2_bind {} {
global env clonbin
source $clonbin/dchv_util/global.h

$c bind $type2_tagmas(1) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(1)
$c create text 400 8.2c -text "$type2_tagmas(1)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(1) <Any-Leave> {$c itemconfigure current -outline $coltype2_r3i
set det vhg
$c delete item
}

$c bind $type2_tagmas(2) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(2)
$c create text 400 8.2c -text "$type2_tagmas(2)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(2) <Any-Leave> {$c itemconfigure current -outline $coltype2_r3f
set det vhg
$c delete item
}

$c bind $type2_tagmas(3) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(3)
$c create text 400 8.2c -text "$type2_tagmas(3)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(3) <Any-Leave> {$c itemconfigure current -outline $coltype2_r3b
set det vhg
$c delete item
}

$c bind $type2_tagmas(4) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(4)
$c create text 400 8.2c -text "$type2_tagmas(4)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(4) <Any-Leave> {$c itemconfigure current -outline $coltype2_r2i
set det vhg
$c delete item
}

$c bind $type2_tagmas(5) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(5)
$c create text 400 8.2c -text "$type2_tagmas(5)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(5) <Any-Leave> {$c itemconfigure current -outline $coltype2_r2f
set det vhg
$c delete item
}

$c bind $type2_tagmas(6) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(6)
$c create text 400 8.2c -text "$type2_tagmas(6)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(6) <Any-Leave> {$c itemconfigure current -outline $coltype2_r2b
set det vhg
$c delete item
}

$c bind $type2_tagmas(7) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(7)
$c create text 400 8.2c -text "$type2_tagmas(7)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(7) <Any-Leave> {$c itemconfigure current -outline $coltype2_r1i
set det vhg
$c delete item
}

$c bind $type2_tagmas(8) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(8)
$c create text 400 8.2c -text "$type2_tagmas(8)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(8) <Any-Leave> {$c itemconfigure current -outline $coltype2_r1f
set det vhg
$c delete item
}

$c bind $type2_tagmas(9) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type2_tagmas(9)
$c create text 400 8.2c -text "$type2_tagmas(9)" -fill navyblue -anchor n -tags item
}
$c bind $type2_tagmas(9) <Any-Leave> {$c itemconfigure current -outline $coltype2_r1b
set det vhg
$c delete item
}
}
proc type2_unbind {} {
global env clonbin
source $clonbin/dchv_util/global.h

for {set i 1} {$i<=9} {incr i } {
$c bind $type2_tagmas($i) <Any-Enter> " "
$c bind $type2_tagmas($i) <Any-Leave> " "
}
}


#TYPE3 BINDINGS
proc type3_bind {} {
global env clonbin
source $clonbin/dchv_util/global.h

$c bind $type3_tagmas(1) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(1)
$c create text 400 8.2c -text "$type3_tagmas(1)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(1) <Any-Leave> {$c itemconfigure current -outline $coltype3_r31
set det vhg
$c delete item
}

$c bind $type3_tagmas(2) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(2)
$c create text 400 8.2c -text "$type3_tagmas(2)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(2) <Any-Leave> {$c itemconfigure current -outline $coltype3_r32
set det vhg
$c delete item
}

$c bind $type3_tagmas(3) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(3)
$c create text 400 8.2c -text "$type3_tagmas(3)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(3) <Any-Leave> {$c itemconfigure current -outline $coltype3_r33
set det vhg
$c delete item
}

$c bind $type3_tagmas(4) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(4)
$c create text 400 8.2c -text "$type3_tagmas(4)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(4) <Any-Leave> {$c itemconfigure current -outline $coltype3_r34
set det vhg
$c delete item
}

$c bind $type3_tagmas(5) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(5)
$c create text 400 8.2c -text "$type3_tagmas(5)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(5) <Any-Leave> {$c itemconfigure current -outline $coltype3_r35
set det vhg
$c delete item
}

$c bind $type3_tagmas(6) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(6)
$c create text 400 8.2c -text "$type3_tagmas(6)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(6) <Any-Leave> {$c itemconfigure current -outline $coltype3_r36
set det vhg
$c delete item
}

$c bind $type3_tagmas(7) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(7)
$c create text 400 8.2c -text "$type3_tagmas(7)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(7) <Any-Leave> {$c itemconfigure current -outline $coltype3_r21
set det vhg
$c delete item
}

$c bind $type3_tagmas(8) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(8)
$c create text 400 8.2c -text "$type3_tagmas(8)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(8) <Any-Leave> {$c itemconfigure current -outline $coltype3_r22
set det vhg
$c delete item
}

$c bind $type3_tagmas(9) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(9)
$c create text 400 8.2c -text "$type3_tagmas(9)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(9) <Any-Leave> {$c itemconfigure current -outline $coltype3_r23
set det vhg
$c delete item
}

$c bind $type3_tagmas(10) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(10)
$c create text 400 8.2c -text "$type3_tagmas(10)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(10) <Any-Leave> {$c itemconfigure current -outline $coltype3_r24
set det vhg
$c delete item
}

$c bind $type3_tagmas(11) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(11)
$c create text 400 8.2c -text "$type3_tagmas(11)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(11) <Any-Leave> {$c itemconfigure current -outline $coltype3_r25
set det vhg
$c delete item
}

$c bind $type3_tagmas(12) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(12)
$c create text 400 8.2c -text "$type3_tagmas(12)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(12) <Any-Leave> {$c itemconfigure current -outline $coltype3_r26
set det vhg
$c delete item
}

$c bind $type3_tagmas(13) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(13)
$c create text 400 8.2c -text "$type3_tagmas(13)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(13) <Any-Leave> {$c itemconfigure current -outline $coltype3_r11
set det vhg
$c delete item
}

$c bind $type3_tagmas(14) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(14)
$c create text 400 8.2c -text "$type3_tagmas(14)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(14) <Any-Leave> {$c itemconfigure current -outline $coltype3_r12
set det vhg
$c delete item
}

$c bind $type3_tagmas(15) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(15)
$c create text 400 8.2c -text "$type3_tagmas(15)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(15) <Any-Leave> {$c itemconfigure current -outline $coltype3_r13
set det vhg
$c delete item
}

$c bind $type3_tagmas(16) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(16)
$c create text 400 8.2c -text "$type3_tagmas(16)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(16) <Any-Leave> {$c itemconfigure current -outline $coltype3_r14
set det vhg
$c delete item
}

$c bind $type3_tagmas(17) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(17)
$c create text 400 8.2c -text "$type3_tagmas(17)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(17) <Any-Leave> {$c itemconfigure current -outline $coltype3_r15
set det vhg
$c delete item
}

$c bind $type3_tagmas(18) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type3_tagmas(18)
$c create text 400 8.2c -text "$type3_tagmas(18)" -fill navyblue -anchor n -tags item
}
$c bind $type3_tagmas(18) <Any-Leave> {$c itemconfigure current -outline $coltype3_r16
set det vhg
$c delete item
}
}
proc type3_unbind {} {
global env clonbin
source $clonbin/dchv_util/global.h

for {set i 1} {$i<=18} {incr i } {
$c bind $type3_tagmas($i) <Any-Enter> " "
$c bind $type3_tagmas($i) <Any-Leave> " "
}
}


#TYPE4 BINDINGS
proc type4_bind {} {
global env clonbin
source $clonbin/dchv_util/global.h

$c bind $type4_tagmas(1) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(1)
$c create text 400 8.2c -text "$type4_tagmas(1)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(1) <Any-Leave> {$c itemconfigure current -outline $coltype4_r11f
set det vhg
$c delete item
}

$c bind $type4_tagmas(2) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(2)
$c create text 400 8.2c -text "$type4_tagmas(2)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(2) <Any-Leave> {$c itemconfigure current -outline $coltype4_r11i
set det vhg
$c delete item
}

$c bind $type4_tagmas(3) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(3)
$c create text 400 8.2c -text "$type4_tagmas(3)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(3) <Any-Leave> {$c itemconfigure current -outline $coltype4_r11b
set det vhg
$c delete item
}

$c bind $type4_tagmas(4) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(4)
$c create text 400 8.2c -text "$type4_tagmas(4)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(4) <Any-Leave> {$c itemconfigure current -outline $coltype4_r12f
set det vhg
$c delete item
}

$c bind $type4_tagmas(5) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(5)
$c create text 400 8.2c -text "$type4_tagmas(5)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(5) <Any-Leave> {$c itemconfigure current -outline $coltype4_r12i
set det vhg
$c delete item
}

$c bind $type4_tagmas(6) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(6)
$c create text 400 8.2c -text "$type4_tagmas(6)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(6) <Any-Leave> {$c itemconfigure current -outline $coltype4_r12b
set det vhg
$c delete item
}

$c bind $type4_tagmas(7) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(7)
$c create text 400 8.2c -text "$type4_tagmas(7)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(7) <Any-Leave> {$c itemconfigure current -outline $coltype4_r13f
set det vhg
$c delete item
}

$c bind $type4_tagmas(8) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(8)
$c create text 400 8.2c -text "$type4_tagmas(8)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(8) <Any-Leave> {$c itemconfigure current -outline $coltype4_r13i
set det vhg
$c delete item
}

$c bind $type4_tagmas(9) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(9)
$c create text 400 8.2c -text "$type4_tagmas(9)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(9) <Any-Leave> {$c itemconfigure current -outline $coltype4_r13b
set det vhg
$c delete item
}

$c bind $type4_tagmas(10) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(10)
$c create text 400 8.2c -text "$type4_tagmas(10)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(10) <Any-Leave> {$c itemconfigure current -outline $coltype4_r14f
set det vhg
$c delete item
}

$c bind $type4_tagmas(11) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(11)
$c create text 400 8.2c -text "$type4_tagmas(11)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(11) <Any-Leave> {$c itemconfigure current -outline $coltype4_r14i
set det vhg
$c delete item
}

$c bind $type4_tagmas(12) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(12)
$c create text 400 8.2c -text "$type4_tagmas(12)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(12) <Any-Leave> {$c itemconfigure current -outline $coltype4_r14b
set det vhg
$c delete item
}

$c bind $type4_tagmas(13) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(13)
$c create text 400 8.2c -text "$type4_tagmas(13)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(13) <Any-Leave> {$c itemconfigure current -outline $coltype4_r15f
set det vhg
$c delete item
}

$c bind $type4_tagmas(14) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(14)
$c create text 400 8.2c -text "$type4_tagmas(14)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(14) <Any-Leave> {$c itemconfigure current -outline $coltype4_r15i
set det vhg
$c delete item
}

$c bind $type4_tagmas(15) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(15)
$c create text 400 8.2c -text "$type4_tagmas(15)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(15) <Any-Leave> {$c itemconfigure current -outline $coltype4_r15b
set det vhg
$c delete item
}

$c bind $type4_tagmas(16) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(16)
$c create text 400 8.2c -text "$type4_tagmas(16)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(16) <Any-Leave> {$c itemconfigure current -outline $coltype4_r16f
set det vhg
$c delete item
}

$c bind $type4_tagmas(17) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(17)
$c create text 400 8.2c -text "$type4_tagmas(17)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(17) <Any-Leave> {$c itemconfigure current -outline $coltype4_r16i
set det vhg
$c delete item
}

$c bind $type4_tagmas(18) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(18)
$c create text 400 8.2c -text "$type4_tagmas(18)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(18) <Any-Leave> {$c itemconfigure current -outline $coltype4_r16b
set det vhg
$c delete item
}

$c bind $type4_tagmas(19) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(19)
$c create text 400 8.2c -text "$type4_tagmas(19)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(19) <Any-Leave> {$c itemconfigure current -outline $coltype4_r21f
set det vhg
$c delete item
}

$c bind $type4_tagmas(20) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(20)
$c create text 400 8.2c -text "$type4_tagmas(20)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(20) <Any-Leave> {$c itemconfigure current -outline $coltype4_r21i
set det vhg
$c delete item
}

$c bind $type4_tagmas(21) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(21)
$c create text 400 8.2c -text "$type4_tagmas(21)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(21) <Any-Leave> {$c itemconfigure current -outline $coltype4_r21b
set det vhg
$c delete item
}

$c bind $type4_tagmas(22) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(22)
$c create text 400 8.2c -text "$type4_tagmas(22)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(22) <Any-Leave> {$c itemconfigure current -outline $coltype4_r22f
set det vhg
$c delete item
}

$c bind $type4_tagmas(23) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(23)
$c create text 400 8.2c -text "$type4_tagmas(23)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(23) <Any-Leave> {$c itemconfigure current -outline $coltype4_r22i
set det vhg
$c delete item
}

$c bind $type4_tagmas(24) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(24)
$c create text 400 8.2c -text "$type4_tagmas(24)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(24) <Any-Leave> {$c itemconfigure current -outline $coltype4_r22b
set det vhg
$c delete item
}

$c bind $type4_tagmas(25) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(25)
$c create text 400 8.2c -text "$type4_tagmas(25)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(25) <Any-Leave> {$c itemconfigure current -outline $coltype4_r23f
set det vhg
$c delete item
}

$c bind $type4_tagmas(26) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(26)
$c create text 400 8.2c -text "$type4_tagmas(26)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(26) <Any-Leave> {$c itemconfigure current -outline $coltype4_r23i
set det vhg
$c delete item
}

$c bind $type4_tagmas(27) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(27)
$c create text 400 8.2c -text "$type4_tagmas(27)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(27) <Any-Leave> {$c itemconfigure current -outline $coltype4_r23b
set det vhg
$c delete item
}

$c bind $type4_tagmas(28) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(28)
$c create text 400 8.2c -text "$type4_tagmas(28)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(28) <Any-Leave> {$c itemconfigure current -outline $coltype4_r24f
set det vhg
$c delete item
}

$c bind $type4_tagmas(29) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(29)
$c create text 400 8.2c -text "$type4_tagmas(29)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(29) <Any-Leave> {$c itemconfigure current -outline $coltype4_r24i
set det vhg
$c delete item
}

$c bind $type4_tagmas(30) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(30)
$c create text 400 8.2c -text "$type4_tagmas(30)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(30) <Any-Leave> {$c itemconfigure current -outline $coltype4_r24b
set det vhg
$c delete item
}

$c bind $type4_tagmas(31) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(31)
$c create text 400 8.2c -text "$type4_tagmas(31)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(31) <Any-Leave> {$c itemconfigure current -outline $coltype4_r25f
set det vhg
$c delete item
}

$c bind $type4_tagmas(32) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(32)
$c create text 400 8.2c -text "$type4_tagmas(32)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(32) <Any-Leave> {$c itemconfigure current -outline $coltype4_r25i
set det vhg
$c delete item
}

$c bind $type4_tagmas(33) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(33)
$c create text 400 8.2c -text "$type4_tagmas(33)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(33) <Any-Leave> {$c itemconfigure current -outline $coltype4_r25b
set det vhg
$c delete item
}

$c bind $type4_tagmas(34) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(34)
$c create text 400 8.2c -text "$type4_tagmas(34)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(34) <Any-Leave> {$c itemconfigure current -outline $coltype4_r26f
set det vhg
$c delete item
}

$c bind $type4_tagmas(35) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(35)
$c create text 400 8.2c -text "$type4_tagmas(35)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(35) <Any-Leave> {$c itemconfigure current -outline $coltype4_r26i
set det vhg
$c delete item
}

$c bind $type4_tagmas(36) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(36)
$c create text 400 8.2c -text "$type4_tagmas(36)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(36) <Any-Leave> {$c itemconfigure current -outline $coltype4_r26b
set det vhg
$c delete item
}

$c bind $type4_tagmas(37) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(37)
$c create text 400 8.2c -text "$type4_tagmas(37)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(37) <Any-Leave> {$c itemconfigure current -outline $coltype4_r31f
set det vhg
$c delete item
}

$c bind $type4_tagmas(38) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(38)
$c create text 400 8.2c -text "$type4_tagmas(38)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(38) <Any-Leave> {$c itemconfigure current -outline $coltype4_r31i
set det vhg
$c delete item
}

$c bind $type4_tagmas(39) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(39)
$c create text 400 8.2c -text "$type4_tagmas(39)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(39) <Any-Leave> {$c itemconfigure current -outline $coltype4_r31b
set det vhg
$c delete item
}

$c bind $type4_tagmas(40) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(40)
$c create text 400 8.2c -text "$type4_tagmas(40)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(40) <Any-Leave> {$c itemconfigure current -outline $coltype4_r32f
set det vhg
$c delete item
}

$c bind $type4_tagmas(41) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(41)
$c create text 400 8.2c -text "$type4_tagmas(41)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(41) <Any-Leave> {$c itemconfigure current -outline $coltype4_r32i
set det vhg
$c delete item
}

$c bind $type4_tagmas(42) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(42)
$c create text 400 8.2c -text "$type4_tagmas(42)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(42) <Any-Leave> {$c itemconfigure current -outline $coltype4_r32b
set det vhg
$c delete item
}

$c bind $type4_tagmas(43) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(43)
$c create text 400 8.2c -text "$type4_tagmas(43)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(43) <Any-Leave> {$c itemconfigure current -outline $coltype4_r33f
set det vhg
$c delete item
}

$c bind $type4_tagmas(44) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(44)
$c create text 400 8.2c -text "$type4_tagmas(44)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(44) <Any-Leave> {$c itemconfigure current -outline $coltype4_r33i
set det vhg
$c delete item
}

$c bind $type4_tagmas(45) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(45)
$c create text 400 8.2c -text "$type4_tagmas(45)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(45) <Any-Leave> {$c itemconfigure current -outline $coltype4_r33b
set det vhg
$c delete item
}

$c bind $type4_tagmas(46) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(46)
$c create text 400 8.2c -text "$type4_tagmas(46)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(46) <Any-Leave> {$c itemconfigure current -outline $coltype4_r34f
set det vhg
$c delete item
}

$c bind $type4_tagmas(47) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(47)
$c create text 400 8.2c -text "$type4_tagmas(47)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(47) <Any-Leave> {$c itemconfigure current -outline $coltype4_r34i
set det vhg
$c delete item
}

$c bind $type4_tagmas(48) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(48)
$c create text 400 8.2c -text "$type4_tagmas(48)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(48) <Any-Leave> {$c itemconfigure current -outline $coltype4_r34b
set det vhg
$c delete item
}

$c bind $type4_tagmas(49) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(49)
$c create text 400 8.2c -text "$type4_tagmas(49)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(49) <Any-Leave> {$c itemconfigure current -outline $coltype4_r35f
set det vhg
$c delete item
}

$c bind $type4_tagmas(50) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(50)
$c create text 400 8.2c -text "$type4_tagmas(50)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(50) <Any-Leave> {$c itemconfigure current -outline $coltype4_r35i
set det vhg
$c delete item
}

$c bind $type4_tagmas(51) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(51)
$c create text 400 8.2c -text "$type4_tagmas(51)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(51) <Any-Leave> {$c itemconfigure current -outline $coltype4_r35b
set det vhg
$c delete item
}

$c bind $type4_tagmas(52) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(52)
$c create text 400 8.2c -text "$type4_tagmas(52)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(52) <Any-Leave> {$c itemconfigure current -outline $coltype4_r36f
set det vhg
$c delete item
}

$c bind $type4_tagmas(53) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(53)
$c create text 400 8.2c -text "$type4_tagmas(53)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(53) <Any-Leave> {$c itemconfigure current -outline $coltype4_r36i
set det vhg
$c delete item
}

$c bind $type4_tagmas(54) <Any-Enter> {$c itemconfigure current -outline SeaGreen1
set det $type4_tagmas(54)
$c create text 400 8.2c -text "$type4_tagmas(54)" -fill navyblue -anchor n -tags item
}
$c bind $type4_tagmas(54) <Any-Leave> {$c itemconfigure current -outline $coltype4_r36b
set det vhg
$c delete item
}

}

proc type4_unbind {} {
global env clonbin
source $clonbin/dchv_util/global.h

for {set i 1} {$i<=54} {incr i } {
$c bind $type4_tagmas($i) <Any-Enter> " "
$c bind $type4_tagmas($i) <Any-Leave> " "
}
}

type0_bind
type1_bind
type2_bind
type3_bind
type4_bind


for {set i 1} {$i<=7} {incr i } {
$c bind $type0_tagmas($i) <1> {
type0_unbind
type1_unbind
type2_unbind
type3_unbind
type4_unbind
}
$c bind $type0_tagmas($i) <3> {
type0_bind
type1_bind
type2_bind
type3_bind
type4_bind
}
}
for {set i 1} {$i<=3} {incr i } {
$c bind $type1_tagmas($i) <1> {
type0_unbind
type1_unbind
type2_unbind
type3_unbind
type4_unbind
}
$c bind $type1_tagmas($i) <3> {
type0_bind
type1_bind
type2_bind
type3_bind
type4_bind
}
}
for {set i 1} {$i<=9} {incr i } {
$c bind $type2_tagmas($i) <1> {
type0_unbind
type1_unbind
type2_unbind
type3_unbind
type4_unbind
}
$c bind $type2_tagmas($i) <3> {
type0_bind
type1_bind
type2_bind
type3_bind
type4_bind
}
}
for {set i 1} {$i<=18} {incr i } {
$c bind $type3_tagmas($i) <1> {
type0_unbind
type1_unbind
type2_unbind
type3_unbind
type4_unbind
}
$c bind $type3_tagmas($i) <3> {
type0_bind
type1_bind
type2_bind
type3_bind
type4_bind
}
}
for {set i 1} {$i<=54} {incr i } {
$c bind $type4_tagmas($i) <1> {
type0_unbind
type1_unbind
type2_unbind
type3_unbind
type4_unbind
}
$c bind $type4_tagmas($i) <3> {
type0_bind
type1_bind
type2_bind
type3_bind
type4_bind
}
}

