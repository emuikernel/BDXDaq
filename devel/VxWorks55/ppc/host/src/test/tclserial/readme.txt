Readme.txt for Tcl Serial Interface routines.

There are now Tcl packages that can be loaded to allow access to the serial
ports on all the hosts that WRS supports.  The two packages, one for Unix
hosts and one for Windows machines allows a user to open, close, flush, 
send, and receive data to and from the machine's serial port.

The Tcl interface routines are:

int tclSerOpen {deviceName baudRate dataBits stopBits parity)
	deviceName is a character string "ttya", "com2", etc for the host port
	baudRate is a valid baud rate 9600, 19200, 38400, etc.
	dataBits is one of 5, 6, 7, or 8
	stopBits is one of 1, or 2 (or for Windows anything else for 1.5).
	parity is a string, one of "none", "even", "odd", "mark" (windows only)

  Opens the named serial device for reading and writing and returns a file 
  descriptor or handle (windows) to be used with other functions.  
  Upon failure returns -1.
 
int tclSerClose {fd}
	fd is value returned by tclSerOpen proc.

  Closes the port or device represented by fd.  Returns 0 upon success, -1
  on error.
  
int tclSerFlush {fd}
	fd is value returned by tclSerOpen proc.

  Flushes the input buffer and any input not yet read.  Returns 0 upon success,
  -1 on failure.

int tclSerSend {fd commandString commandLength}
	fd is value returned by tclSerOpen proc.

  Sends the command string to the port or device named by fd.  Returns 0 upon
  success, or -1 on failure.

int tclSerReceive {fd receiveBufSize timeOutValue endPattern}
	fd is value returned by tclSerOpen proc.

  Receives any and all input from the device represented by fd until:

  1) the timeout value is exceeded, upon which the return value is 0
  2) the input buffer has been overrun (receiveBufSize too small) which returns
     -1
  3) the endPattern has been received, which returns the number of bytes
     received.


  In all cases, the returned value is a Tcl list of two items consisting of:
  {return_code buffer_contents}.  The return code as described above is
  either a 0 (timeout occurred), -1 (receive buffer too small), or number of
  bytes received upon success.  The buffer_contents are the characters received
  until the read was stopped.
 

Examples of usage:

Note: The "0" you see after some of these commands shows the Tcl
result of the command execution.

To load the Tcl Serial Package:

set ext [info sharedlibextension]
set bkDir $env(WIND_BASE)/host/$env(WIND_HOST_TYPE)/lib/backend
load [file join $bkDir pkgtclserial$ext]

The three lines above automatically determines the shared library extension,
and loads the tcl serial package.

To open a device:

set fd [tclSerOpen com2 9600 8 1 none]
0

To flush the device:

tclSerFlush $fd
0

To set a string buffer with some characters and send it to the device:

The following example Tcl scripts send a command to the built-in shell or 
to the bootrom user interface.

Sending the "i" comamnd to the shell.

set command "i\n"
tclSerSend $fd $command [string length $command]
catch {tclSerReceive $fd 1048 45 "->"} results
puts $results

The output of the "puts $results" command yields:

753 {i

  NAME        ENTRY       TID    PRI   STATUS      PC       SP     ERRNO  DELAY
---------- ------------ -------- --- ---------- -------- -------- ------- -----
tExcTask   excTask      801f9c30   0 PEND       8007dbf8 801f9b68       0     0
tLogTask   logTask      801f7110   0 PEND       8007dbf8 801f7050       0     0
tShell     shell        801e7f60   1 READY      80060f50 801e7bc0       0     0
tRlogind   rlogind      801ef5f0   2 PEND       800631f0 801ef258       0     0
tTelnetd   telnetd      801ed670   2 PEND       800631f0 801ed538       0     0
tNetTask   netTask      801f2b60  50 PEND       800631f0 801f2ae0       0     0
tFtpdTask  ftpdTask     801ec0d0  55 PEND       800631f0 801ebfa8       0     0
value = 0 = 0x0
->}

Where 753 is the number of bytes received (a successful receive of the "->" 
string and the rest is the contents of the receive buffer bracked by '{', '}'.

Sending the "reboot" command to the shell.

set command "reboot\n"
tclSerSend $fd $command [string length $command]
catch {tclSerReceive $fd 9182 65 "->"} results
puts $results

The output of the "puts $results" command yields:

2089 { reboot


Press any key to stop auto-boot...
 0
auto-booting...


boot device          : sn
processor number     : 0
host name            : meuse
file name            : /view/will_101/vobs/wpwr/target/config/idts381/vxWorks
inet on ethernet (e) : 147.11.44.169:ffffff00
host inet (h)        : 147.11.44.7
user (u)             : will
flags (f)            : 0x0
target name (tn)     : t44-169

Attaching network interface sn0... done.
Attaching network interface lo0... done.
Loading... 573912
Starting at 0x80010000...

Attaching network interface sn0... done.
Attaching network interface lo0... done.
NFS client support not included.
Loading symbol table from meuse:/view/will_101/vobs/wpwr/target/config/idts381/v
xWorks.sym ...done


 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
      ]]]]]]]]]]]  ]]]]     ]]]]]]]]]]       ]]              ]]]]         (R)
 ]     ]]]]]]]]]  ]]]]]]     ]]]]]]]]       ]]               ]]]]
 ]]     ]]]]]]]  ]]]]]]]]     ]]]]]] ]     ]]                ]]]]
 ]]]     ]]]]] ]    ]]]  ]     ]]]] ]]]   ]]]]]]]]]  ]]]] ]] ]]]]  ]]   ]]]]]
 ]]]]     ]]]  ]]    ]  ]]]     ]] ]]]]] ]]]]]]   ]] ]]]]]]] ]]]] ]]   ]]]]
 ]]]]]     ]  ]]]]     ]]]]]      ]]]]]]]] ]]]]   ]] ]]]]    ]]]]]]]    ]]]]
 ]]]]]]      ]]]]]     ]]]]]]    ]  ]]]]]  ]]]]   ]] ]]]]    ]]]]]]]]    ]]]]
 ]]]]]]]    ]]]]]  ]    ]]]]]]  ]    ]]]   ]]]]   ]] ]]]]    ]]]] ]]]]    ]]]]
 ]]]]]]]]  ]]]]]  ]]]    ]]]]]]]      ]     ]]]]]]]  ]]]]    ]]]]  ]]]] ]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]       Development System
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]       VxWorks version 5.3.1
 ]]]]]]]]]]]]]]]]]]]]]]]]]]       KERNEL: WIND version 2.5
 ]]]]]]]]]]]]]]]]]]]]]]]]]       Copyright Wind River Systems, Inc., 1984-1997

                               CPU: IDT S381.  Processor #0.
                              Memory Size: 0x200000.  BSP version 1.1/1.

->}


Sending the ^x command to the bootrom.

set command "\n"
tclSerSend $fd $command [string length $command]
catch {tclSerReceive $fd 2096 65 "->"} results
puts $results

The output of the "puts $results command" yields:

2081 {


Press any key to stop auto-boot...
 0
auto-booting...


boot device          : sn
processor number     : 0
host name            : meuse
file name            : /view/will_101/vobs/wpwr/target/config/idts381/vxWorks
inet on ethernet (e) : 147.11.44.169:ffffff00
host inet (h)        : 147.11.44.7
user (u)             : will
flags (f)            : 0x0
target name (tn)     : t44-169

Attaching network interface sn0... done.
Attaching network interface lo0... done.
Loading... 573912
Starting at 0x80010000...

Attaching network interface sn0... done.
Attaching network interface lo0... done.
NFS client support not included.
Loading symbol table from meuse:/view/will_101/vobs/wpwr/target/config/idts381/v
xWorks.sym ...done


 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
      ]]]]]]]]]]]  ]]]]     ]]]]]]]]]]       ]]              ]]]]         (R)
 ]     ]]]]]]]]]  ]]]]]]     ]]]]]]]]       ]]               ]]]]
 ]]     ]]]]]]]  ]]]]]]]]     ]]]]]] ]     ]]                ]]]]
 ]]]     ]]]]] ]    ]]]  ]     ]]]] ]]]   ]]]]]]]]]  ]]]] ]] ]]]]  ]]   ]]]]]
 ]]]]     ]]]  ]]    ]  ]]]     ]] ]]]]] ]]]]]]   ]] ]]]]]]] ]]]] ]]   ]]]]
 ]]]]]     ]  ]]]]     ]]]]]      ]]]]]]]] ]]]]   ]] ]]]]    ]]]]]]]    ]]]]
 ]]]]]]      ]]]]]     ]]]]]]    ]  ]]]]]  ]]]]   ]] ]]]]    ]]]]]]]]    ]]]]
 ]]]]]]]    ]]]]]  ]    ]]]]]]  ]    ]]]   ]]]]   ]] ]]]]    ]]]] ]]]]    ]]]]
 ]]]]]]]]  ]]]]]  ]]]    ]]]]]]]      ]     ]]]]]]]  ]]]]    ]]]]  ]]]] ]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]       Development System
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]]
 ]]]]]]]]]]]]]]]]]]]]]]]]]]]       VxWorks version 5.3.1
 ]]]]]]]]]]]]]]]]]]]]]]]]]]       KERNEL: WIND version 2.5
 ]]]]]]]]]]]]]]]]]]]]]]]]]       Copyright Wind River Systems, Inc., 1984-1997

                               CPU: IDT S381.  Processor #0.
                              Memory Size: 0x200000.  BSP version 1.1/1.

->}

Closing the device:

tclSerClose $fd
0

If a command was entered and the timeout value was too short this example
shows what happens.  We invoke the reboot command, but pass a timeout value of
only 5 seconds.

set command "reboot\n"
tclSerSend $fd $command [string length $command]
catch {tclSerReceive $fd 9182 5 "->"} results
puts $results

0 {reboot
}

Note that the contents of results contains { 0 {reboot} ).  The 0 is the return
value indicating a timeout has occurred, and the <reboot> is the only results
collected prior to the timeout.


Debug Mode:

To turn on an internal debugging flag so that the drivers show the characters
sent and received on the fly, set the linked Tcl variable debug_on to 1:

set debug_on 1

The above tcl command will cause the drivers to display the parameters passed 
to the Tcl interface routines as well as the characters sent and received.  
The characters sent are show in hexadecimal to show unprintable characters as 
well.  Characters received are always shown in ASCII.

Using the following tcl script:

  set command "i\n"
  tclSerSend $fd $command [string length $command]
  catch {tclSerReceive $fd 1048 45 "->"} results
  puts $results

that was stored in a file tcl_i, and using wtxtcl which already loaded the
Tcl serial package and opened a port, we get the following result if the
debug_on Tcl variable is set to 1:

% source tcl_i
arg[0] = tclSerSend arg[1] = 84 arg[2] = i
 arg[3] = 2

win32_write: length = 2
69 0a

arg[0] = tclSerReceive arg[1] = 84 arg[2] = 1048 arg[3] = 45 arg[4] = ->
 i

  NAME        ENTRY       TID    PRI   STATUS      PC       SP     ERRNO  DELAY
---------- ------------ -------- --- ---------- -------- -------- ------- -----
tExcTask   excTask      801f9c30   0 PEND       8007dbf8 801f9b68       0     0
tLogTask   logTask      801f7110   0 PEND       8007dbf8 801f7050       0     0
tShell     shell        801e7ab0   1 READY      80060f50 801e7710       0     0
tRlogind   rlogind      801ef5f0   2 PEND       800631f0 801ef258       0     0
tTelnetd   telnetd      801ed670   2 PEND       800631f0 801ed538       0     0
tNetTask   netTask      801f2b60  50 PEND       800631f0 801f2ae0       0     0
tFtpdTask  ftpdTask     801ec0d0  55 PEND       800631f0 801ebfa8       0     0
value = 0 = 0x0
->
754 { i

  NAME        ENTRY       TID    PRI   STATUS      PC       SP     ERRNO  DELAY
---------- ------------ -------- --- ---------- -------- -------- ------- -----
tExcTask   excTask      801f9c30   0 PEND       8007dbf8 801f9b68       0     0
tLogTask   logTask      801f7110   0 PEND       8007dbf8 801f7050       0     0
tShell     shell        801e7ab0   1 READY      80060f50 801e7710       0     0
tRlogind   rlogind      801ef5f0   2 PEND       800631f0 801ef258       0     0
tTelnetd   telnetd      801ed670   2 PEND       800631f0 801ed538       0     0
tNetTask   netTask      801f2b60  50 PEND       800631f0 801f2ae0       0     0
tFtpdTask  ftpdTask     801ec0d0  55 PEND       800631f0 801ebfa8       0     0
value = 0 = 0x0
->}
%
