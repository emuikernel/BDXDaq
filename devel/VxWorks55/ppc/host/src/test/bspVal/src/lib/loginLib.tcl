# loginLib.tcl - validation suite login library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,14aug97,db  added flush of serial channel before sending command.
# 01a,22jun97,db  written.
#
#
# DESCRIPTION
# This tcl library consists of procedures used to test a login session onto
# VxWorks opened by rlogin.
#
# The function testLogin() makes sure the target shell is suspended by the rlogin
# session.  The rlogin session is checked by calling several shell
# commands such as version() and printLogo(). The function logout() is called
# to end the login session. The test verifies that the shell is returned to 
# the target's console.
#*/

################################################################################
#
# testLogin - test rlogin or telnet session onto VxWorks
#
# This routine makes sure the target shell is suspended by the rlogin
# or telnet session.  The rlogin session is checked by calling the functions
# version() and printLogo(). Logout() is called to end the session. 
# The end of the login session is checked by verifying that the shell is
# returned to the target's console.
#
# SYNOPSIS:
#   testLogin fromFd toFd toAdrs toName 
#
# PARAMETERS:
#   fromFd: serial port handle of target from where to perform the rlogin      
#   toFd:   serial port handle of target to rlogin to 
#   toAdrs: IP address of target to rlogin to
#   toName: name of target to rlogin to
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc testLogin {fromFd toFd toAdrs toName} {

    # set size of buffer used in reading characters from serial port
    # and timeout.

    set buffSize 2048
    set readTMO	 10
    
    # send rlogin command 

    send_user "Destination IP address=$toAdrs" 0 
    serChanFlush $fromFd
    serChanFlush $toFd
    serChanSend $fromFd "rlogin\ \"$toAdrs\"\n"

    # first test: check for IN USE message on target's shell

    set ret [serChanRecv $toFd $buffSize $readTMO "USE" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]}  {
        set passFail 0
    } else  {
        set passFail 1
    }

    # return results of test 1

    testResult $passFail [format "IN USE message when rlogin to %s" $toName]

    # test number 2: try to reach target shell(handle login and password)

    # check login worked onto VxWorks
    # the first read on the serial channel returns the "rlogin "$toAdrs" 
    # message. The second read attempts to get the login prompt or the
    # shell prompt.

    set ret [serChanRecv $fromFd $buffSize $readTMO "->" 1] ;#no extra line
    set ret [serChanRecv $fromFd $buffSize $readTMO "->" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]}  {
       if {[string match *login:* $ret]} {
       	   serChanSend $fromFd "target\n"
       	   set ret [serChanRecv $fromFd $buffSize $readTMO "word:" 1]
    	   send_user [trimSerOutput $ret] 0
       	   if {[string match timeoutError* $ret] || [string match Error* $ret]} {
       	   	set passFail 0
   	   } else {
   	   	serChanSend $fromFd "password\n"
   	   	set ret [serChanRecv $fromFd $buffSize $readTMO "->" 1]
                send_user [trimSerOutput $ret] 0
       	        if {[string match timeoutError* $ret] || \
       	            [string match Error* $ret]} {
       	            set passFail 0
      		} else {
      		    set passFail 1
	        }
	   }
       } else {
       	   set passFail 0
       }
    } else {
        set passFail 1
    }

    # sleep for 5 seconds

    msleep 5000

    # check that VxWorks shell was reached by calling version()

    serChanSend $fromFd "version\n"
    set ret [serChanRecv $fromFd $buffSize $readTMO "$toAdrs" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]} {
        set passFail 0
    } else  {
        set passFail 1
    }

    set inMsg [format "reach shell of %s" $toName]

    # return results for test 2

    testResult $passFail $inMsg

    # test number 3: exercise the pty by calling printLogo() 

    serChanSend $fromFd "printLogo\n"
    set ptMsg [format "rlogin pty usage on %s" $toName]
    set ret [serChanRecv $fromFd $buffSize $readTMO "Development" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]} {
        set passFail 0
    } else  {
        set passFail 1
    }

    # return result for test 3

    testResult $passFail $ptMsg

    #test 4: logout of VxWorks

    serChanSend $fromFd "logout\n"

    # look for part of "Closed connection" or "Connection closed"

    set ret [serChanRecv $fromFd $buffSize $readTMO "lose" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]} {
        set passFail 0
    } else  {
        set passFail 1
    }
    set outMsg [format "logout from %s" $toName]

    if {$passFail == 0} {
    	testResult $passFail $outMsg
    	return
    }

    # check that shell returned to target's console

    msleep 2000

    serChanFlush $toFd
    serChanSend $toFd "version\n"
    set ret [serChanRecv $toFd $buffSize $readTMO "->" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]} {
        set passFail 0
    } elseif {[string match *$toAdrs* $ret]} {
        set passFail 1
    } else {
    	set passFail 0
    }

    # return results for test 4  

    testResult $passFail $outMsg
 }
