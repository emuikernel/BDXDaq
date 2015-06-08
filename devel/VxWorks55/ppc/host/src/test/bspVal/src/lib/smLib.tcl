# smLib.tcl - VxWorks shared memory library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,11aug97,db  minor fixes.
# 01a,19jun97,db  written.
#
#
# DESCRIPTION
# This is a library of routines used to find and verify shared memory locations
# on backplane configurations.
#
# smAdrsFind() attempts to find an address on a slave board that will access a
# given address on a backplane master shared memory.  The routines 
# sysLocalToBusAdrs() and sysBusToLocalAdrs() (located in sysLib.c in the BSP)
# are used to locate a suitable slave address for the shared memory. 
# 
# The following scheme is used to implement dereferencing in tcl: a memory 
# location is allocated using wtxMemAlign() which returns a pointer. The 
# pointer value is stored in a tcl variable. Two functions in pkLib.c, 
# pkIntGet() and pkIntSet() are used for returning/setting the content of an 
# integer location.
#
# smAdrsCheck() verifies that two given addresses access the same memory 
# location.
#
# smResultsCheck() performs two readings of the content of the counter 
# location and with a one second time interval between the two readings. If 
# the two readings are different, it indicates that the test is successful, 
# otherwise the function exits.
#
#*/

################################################################################
#
# smAdrsFind - find slave's shared memory address
#
# This routine attempts to find a backplane slave's address that will access 
# <masterAdrs> on the master.  Address modifiers for standard, extended, and
# short memory address spaces are tried, in that order.  sysLocalToBusAdrs()
# is called on the master to get the related vme address, then
# sysBusToLocalAdrs() is called on the slave to get the related slave address.
#
# If both routines return valid addresses, the memory address is probed, then
# verified by calling smAdrsCheck(), to make sure it is truly shared.
#
# SYNOPSIS:
#   smAdrsFind masterAdrs masterHd slaveHd 
#
# PARAMETERS:
#   masterAdrs: local address used by master to access shared memory location.      
#   masterHd: master target server handle
#   slaveHd:  slave target server handle
#
# RETURNS: 
#   slave address of shared memory, or (-1) if not found
#
# ERRORS: N/A
#

proc smAdrsFind {masterAdrs masterHd slaveHd}  { 

    # try standard, extended, and short address modifiers (in that order)

    set adrsMods "0x3d 0x0d 0x39 0x09 0x29"
    set found 0

    # allocate memory on master and create tcl variable to hold
    # the returned address

    wtxHandle $masterHd
    set vme [wtxMemAlign 2 4]
    if {$vme == 0} {
    	return -1
    }

    # allocate memory on slave and create tcl variable to hold
    # the returned address

    wtxHandle $slaveHd
    set slave [wtxMemAlign 2 4]
    if {$slave == 0} {
    	return -1
    }

    # try each address modifier till shared address found

    foreach mod $adrsMods {

    	# start by looking at master

	wtxHandle $masterHd
	send_user  "# checking address modifier: $mod\r" 0

    	# find vme address of master address by calling sysLocalToBusAdrs()

	set retcode [wtxMemSet $masterAdrs 4 0]
	wtxFuncCall -int  [symbol sysLocalToBusAdrs] $mod $masterAdrs $vme
	set retA [retcodeGet]
	if {$retA == -1} {
		continue
	}

	# sysLocalToBusAdrs was successful. read the content of the 
	# address pointed to by the variable vme.

	wtxFuncCall -int [symbol pkIntGet] $vme
	set vmeAdrs [retcodeGet]
	if {$vmeAdrs == -1} {
		continue
	}
        send_user "masterAdrs=$masterAdrs vme=$vmeAdrs" 0

    	# select slave target server 

        wtxHandle $slaveHd

    	# find slave address of vme address by calling sysBusToLocalAdrs()
        # first initialize the memory location pointed to by slave.

	set retcode [wtxMemSet $slave 4 0]

	wtxFuncCall -int  [symbol sysBusToLocalAdrs] $mod $vmeAdrs $slave
	set retB [retcodeGet]
	if {$retB == -1} {
		continue
	}
	wtxFuncCall -int [symbol pkIntGet] $slave
	set slaveAdrs [retcodeGet]
	if {$slaveAdrs == -1} {
		continue
	}
	send_user  "slaveAdrs=$slaveAdrs" 0

	# probe slave address to make sure it is valid memory

	wtxFuncCall -int [symbol pkTestRamByte] $slaveAdrs
	set retC [retcodeGet]
        if {$retC != 0} {
        	continue
        }

	# check that location is indeed shared memory

        if {![smAdrsCheck $masterAdrs $masterHd $slaveAdrs $slaveHd]} {
        	continue
        }

	set found 1
	break
    }

    # free allocated memory on slave 

    wtxHandle $slaveHd
    wtxMemFree $slave

    # free allocated memory on master

    wtxHandle $masterHd
    wtxMemFree $vme

    # return slave address if found

    if {$found == 1} {
        return $slaveAdrs
    } else {
	return -1
    }
}

################################################################################
#
# smAdrsCheck - check that memory is shared between master and slave 
#
# This routine will check that <masterAdrs> and <slaveAdrs> both refer to the
# same memory location. 
#
# The master writes a one to the address location. Then the slave reads the 
# content of that location into the variable <slaveValue> using the 
# corresponding vme address and increments the content of the memory location.
# Finally, the master reads the memory location. 
#
# The test passes if the values read by the master and the slave are 2 and 1
# respectively.
#
# SYNOPSIS: 
#   smAdrsCheck masterAdrs masterHd slaveAdrs slaveHd
#
# PARAMETERS:
#   masterAdrs: local address used by master to access shared memory location
#   masterHd: master target server handle
#   slaveAdrs: local address used by slave to access shared memory location
#   slaveHd: slave target server handle
#
# RETURNS: 
#   1 if memory is shared, 0 if not
#
# ERRORS: N/A
#

proc smAdrsCheck {masterAdrs masterHd slaveAdrs slaveHd} { 

    # clear the shared memory location on the slave side

    wtxHandle $slaveHd
    wtxFuncCall -int [symbol pkIntSet] $slaveAdrs 0
    retcodeGet

    # write a 1 to the master location

    wtxHandle $masterHd
    wtxFuncCall -int [symbol pkIntSet] $masterAdrs 1
    retcodeGet

    # on the slave, read the content of the shared memory location
    # and save it in the variable slaveValue. Then write a 2 to the 
    # shared memory location. 

    wtxHandle $slaveHd
    wtxFuncCall -int [symbol pkIntGet] $slaveAdrs
    set slaveValue [retcodeGet]

    wtxFuncCall -int [symbol pkIntSet] $slaveAdrs 2
    retcodeGet

    # from the master, read the shared memory location that was just
    # affected by the slave.

    wtxHandle $masterHd
    wtxFuncCall -int [symbol pkIntGet] $masterAdrs
    set masterValue [retcodeGet]

    # return result

    if {($masterValue == 2) && ($slaveValue == 1)}  {
        return 1
    } else { 
        return 0
    }
}

################################################################################
#
# smResultsCheck - checks results of busTas test
#
# This procedure checks if the counter on the master is still incrementing. If
# it is then the test is successful and the procedure displays the PASS message
# If the test fails the procedure exits after displaying the FAIL message
# and killing the busTas test tasks on both the master and the slave.
#
# SYNOPSIS:
#   smResultsCheck masterHd slaveHd masterCountAdrs 
#                  masterTid slaveTid testMessage
# 
# PARAMETERS:
#   masterHd: master target server handle
#   slaveHd: slave target server handle
#   masterCountAdrs: local address used by master to access counter in
#	             shared memory.
#   masterTid: ID of busTas test task running on master
#   slaveTid: ID of busTas test task running on slave
#   testMessage: message to be added to "PASS"("FAIL") before sent to logfile
#
# RETURNS: 
#   N/A (This procedure does not return if test fails)
# 
# ERRORS:
#  "TAS mechanism not working properly?"
#
proc smResultsCheck {masterHd slaveHd masterCountAdrs \
	 	     masterTid slaveTid testMessage} {

    # select master target server and read the content of the counter
    # in shared memory.

    wtxHandle $masterHd
    wtxFuncCall -int [symbol pkIntGet] $masterCountAdrs
    set count1 [retcodeGet]

    msleep 1000

    # do a second reading of the counter one second later

    wtxFuncCall -int [symbol pkIntGet] $masterCountAdrs
    set count2 [retcodeGet]

    send_user  "$testMessage: count1=$count1 count2=$count2" 0

    if {$count1 != $count2} {
  	testResult 1 $testMessage 
    } else {

        # kill test task on master

  	wtxContextKill CONTEXT_TASK $masterTid

  	# select slave target server and kill test task.

  	wtxHandle $slaveHd 
  	wtxContextKill CONTEXT_TASK $slaveTid

	# call wtxToolDetach to disconnect from the two target
	# servers. 

  	wtxToolDetach $masterHd
  	wtxToolDetach $slaveHd

        testResult 0 $testMessage
  	fatalError "TAS mechanism not working properly?"
    }
}

