# wtxCreateRms.tcl - Tcl script, generate object module signature (.rms)
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01f,27may98,p_b  WTX 2.0 new changes
# 01e,28jan98,p_b  added tclPathGet
# 01d,14jun95,f_v  added wtxtest name to wtxToolAttach
# 01c,22may95,jcf  name revision.
# 01b,12apr95,f_v  made more robust, Launch loopBack automaticly
# 01a,02mar95,f_v  written.
#
#
#
# DESCRIPTION
#
#    This routine create object module reference and place it in
#    ~/wpwr/target/lib/obj<CPU><TOOL>test 
#
#    If you don't pass name as parameter wtxCreateRef make each reference, or
#    if you wan't just change one or more files, you must specifie these
#    files as parameters
#

proc wtxCreateRms { tgtSvrName {args ""} } { 

    global env
    global USE_TSFORLOAD
    set flag 0

    wtxToolAttach $tgtSvrName wtxtest

    set nameFile [tclPathGet]memAdrs.tcl

    if {[file exists $nameFile] != 1 } {
    		error " ERROR : $nameFile doesn't exists"
        }

    source $nameFile

    # get cpu tool, and adress of text,data,bss
    set info [memAdrs adrList]
    
    set cpu   [lindex $info 0]
    set tool  [lindex $info 1]
    set cpuNb [lindex $info 2]

    set type test
    set pt "..."

    puts stdout "Test in progress" 

    set refpath \
	$env(WIND_BASE)/target/lib/obj$cpu$tool$type

    set tgtSvrName2 testLoop[pid]
    exec tgtsvr $tgtSvrName2 -B loopback -c $refpath/vxWorks -cpu $cpuNb &

    after 2000

    wtxToolDetach

    wtxToolAttach $tgtSvrName2 wtxtest

    # list of object modules - new module should be added at this list
    if { $args == "" } {
	   set list [glob $refpath/*.o]
      } else { 
	set list $args
      }

    # get file name without extension from list
    for { set i 0 } { $i < [llength $list] } { incr i } {
	set list [lreplace $list $i $i [file root [file tail\
			[lindex $list $i]]]]
       }

    # Load a module which generate a .rms file in current directory
    # and compare it to reference

    foreach i $list {

	# check if file exists in right directory
	if {[file exists $refpath/$i.o] == 1 } {

	# load object module
	catch {wtxObjModuleLoad $USE_TSFORLOAD file $adrList(text)\
		$adrList(data) $adrList(bss) $refpath/$i.o } tId

	# make difference between new file and reference file
	catch {exec diff $i.rms $refpath/$i.ref} diff

	if { $diff != ""} {
	   puts stdout "There is a difference between $i.ref and $i.rms files"
	   set flag 1
	   }

	# clean up
	file delete $i.rms
	set tId [lindex $tId 0]
	wtxObjModuleUnload $tId

	} else {
	    puts stdout  "\nERROR : $refpath/$i.o\n\t doesn't exist\n"
	}
     }

     wtxTsKill
     after 2000
     wtxToolDetach

     if { $flag == 1 } {
     puts stdout "\nThere isn't any difference between others\
			.ref and .rms files\n"
     } {
     puts stdout "\nThere isn't any difference between .ref and .rms files\n"
     }
}
