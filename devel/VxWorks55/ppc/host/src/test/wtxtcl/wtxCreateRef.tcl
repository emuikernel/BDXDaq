# wtxCreateRef.tcl - Tcl script, generate object module reference (.ref)
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01i,27may98,p_b  WTX 2.0 new changes
# 01h,28jan98,p_b  added tclPathGet, replace "exec mv f1 f2" by
#                  "file copy f1 f2" + "file delete f1"
# 01g,14jun95,f_v  changed wtxToolAttach name parameter
# 01f,24may95,f_v  redirect stderr and update kill and attach management
#		   added timeout for wtxObjModuleLoad
# 01e,22may95,jcf  name revision.
# 01d,12may95,f_v  safe use of unix command 
# 01c,11may95,f_v  now messages are printed with env variable
# 01b,12apr95,f_v  launch loopBack mode automaticly, made more robust
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

proc wtxCreateRef {tgtSvrName {args 0}} { 


    global env
    global USE_TSFORLOAD
    global wtxCmdTimeout
    set wtxCmdTimeout(wtxObjModuleLoad) 100


    wtxToolAttach $tgtSvrName wtxtest
    
    set tgtSvrName2 testLoop[pid]
    set type test

    
    set nameFile [tclPathGet]memAdrs.tcl

    if {[file exists $nameFile] != 1 } {
    		error " ERROR : $nameFile doesn't exists"
        }

    source $nameFile

    # get cpu tool, and adress of text,data,bss
    set info [memAdrs adrList]
    
    set cpu  [lindex $info 0]
    set tool [lindex $info 1]
    set cpuNb [lindex $info 2]

    set refpath $env(WIND_BASE)/target/lib/obj$cpu$tool$type
    puts stdout "Creation in progress..."

    # launch tgtsvr in loopBack mode by a subprocess
    exec tgtsvr $tgtSvrName2 -B loopback -c $refpath/vxWorks \
    -cpu $cpuNb 2>/dev/null &

    # made a tempo 
    after 2000

    # attach tool to loopBack target server
    wtxToolAttach $tgtSvrName2 wtxtest

    # list of object modules - new module should be added at this list
    if { $args == 0 || $args == {}} {
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
    # and move it in proper directory

    foreach i $list {

	# check if file exists in right directory
	if {[file exists $refpath/$i.o] == 1 } {

	    # load object module
	    catch {wtxObjModuleLoad $USE_TSFORLOAD file $adrList(text)\
	    $adrList(data) $adrList(bss) $refpath/$i.o } tId

	    if {[file exists $refpath/$i.ref] == 1 } {
		file copy $refpath/$i.ref $refpath/$i.ref.bak
		file delete $refpath/$i.ref
		puts stdout "File [format "%17s" $i.ref] has been renamed in\
			    [format "%20s" $i.ref.bak]"
		}
		
	    # change args and place file in right directory
	    if {[file exists $i.rms]} {
		if {[file writable $refpath]} {
		    file copy $i.rms $refpath/$i.ref
		    file delete $i.rms
		} {
		    puts stdout  "\nERROR : You can't write in\
				$refpath directory\n"
		    break
		}
	    } {
	    puts stdout  "\nERROR : You can't write in\
			current directory\n"
	    break
	    }

		    


	    # clean up
	    set tId [lindex $tId 0]
	    wtxObjModuleUnload $tId

	    after 1000

	} else {
	    puts stdout  "\nERROR : $refpath/$i.o\n\t doesn't exist\n"
	}
    }

     puts stdout "\n.ref files should be created in\n\
		\t~$env(WIND_BASE)/target/lib/obj$cpu$tool$type directory"

     # kill target server in loopBack mode
     wtxTsKill DESTROY
     after 2000
     # detach from loopBack mode
     wtxToolDetach
     # detach from normal mode
     wtxToolDetach
}
