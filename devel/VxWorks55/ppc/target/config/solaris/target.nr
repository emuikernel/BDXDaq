'\" t
.so wrs.an
.\" solaris/target.nr - Solaris 2.x target-specific documentation
.\"
.\" Copyright 1996-2002 Wind River Systems, Inc.
.\"
.\" modification history
.\" --------------------
.\" 01f,02jan02,hbh  Fixed simulator option display.
.\" 01e,12nov01,jmp  updated networking information.
.\" 01d,27aug01,dgp  change manual pages to reference entries per SPR 23698
.\" 01c,15apr99,p_m  Told the world that this is Solaris 2.x simulator.
.\" 01b,28jun96,ism  minor update
.\" 01a,03apr96,ism  written.
.\"
.\"
.TH "solaris" T "VxSim for Solaris 2.x" "Rev: 02 Jan 02" "VXWORKS REFERENCE MANUAL"

.SH "NAME"
.aX "VxSim for Solaris 2.x"

.SH "INTRODUCTION"
This reference entry provides target-specific information necessary to run
the VxWorks simulator on Solaris.

.SH "BOOT ROMS"
Not applicable.
.PP
To load the VxWorks simulator simply type "vxWorks" to the UNIX shell.
For further information, follow the instructions in the
.I "VxSim"
chapter of the
.I "Tornado User's Guide"
and also see the
.I "Getting Started"
chapter of the
.I "VxWorks Programmer's Guide."

.SH "JUMPERS"
Not applicable.

.SH "DEVICES"
A sio driver is provided for standard input and output.
UNIX job control characters are not disabled even though
the I/O is in raw mode.  Trapping of ^C and other
special characters is UNIX-shell specific and not
according to usual VxWorks tyLib conventions.
.PP
A pseudo-DOS disk may be created by adding INCLUDE_DOS_DISK in config.h.
By default, a 1.4 Meg file/disk is created in "/tmp/<progname><Procnum>.dos".

.PP
FULL SIMULATOR ONLY
A network interface called PPP (Point to Point Protocol) allows this UNIX
process to connect to other UNIX processes on the same host using
a unique IP address (usually 192.168.255.\<ProcessorNumber + 1\>). The simulator
does not need networking to load files or debug applications.
Prior to Tornado 2.2, ULIP was provided as the default network interface for
VxSim on Solaris host. Only PPP is supported on Tornado 2.2. 

`Loading PPP on a Solaris 2.7 or 2.8 Host'

First, use the command pkginfo to check whether the following packages are installed on your host:

\cs
    SUNWapppr	PPP/IP Asynchronous PPP daemon configuration files
    SUNWapppu	PPP/IP Asynchronous PPP daemon and PPP login service
    SUNWpppk	PPP/IP and IPdialup Device Drivers
    SUNWpppkx	PPP/IP and IPdialup Device Drivers (64-bit)
    SUNWbnur	Networking UUCP Utilities, (Root)
    SUNWbnuu	Networking UUCP Utilities, (Usr)
\ce

For example:

\cs
    % pkginfo | egrep  'ppp|bnu'
    system SUNWapppr	PPP/IP Asynchronous PPP daemon configuration files
    system SUNWapppu	PPP/IP Asynchronous PPP daemon and PPP login service
    system SUNWpppk	PPP/IP and IPdialup Device Drivers
    system SUNWpppkx	PPP/IP and IPdialup Device Drivers
    system SUNWbnur	Networking UUCP Utilities, (Root)
    system SUNWbnuu	Networking UUCP Utilities, (Usr)
\ce

The SUNWpppkx package should only be installed on Solaris hosts running in 64-bit mode. Use the isainfo -b command to determine the mode in which your Solaris 2.7 or 2.8 host runs. This command returns 32 or 64. If the packages are not already installed, mount the Solaris installation disk and change your working directory to the location of these packages (for example, on a Solaris 2.7 CD-ROM, they can be found in /cdrom/sol7_599_sparc_sun_srvr/s0/Solaris_2.7/Product) and install them using the following commands:

For a Solaris host running in 32 bits:

\cs
    % isainfo -b
    32
    % su root
    Password:
    # pkgadd -d  `pwd` SUNWbnur SUNWbnuu SUNWpppk SUNWapppr SUNWapppu
\ce

For a Solaris host running in 64 bits:

\cs
    % isainfo -b
    64
    % su root
    Password:
    # pkgadd -d  `pwd`  SUNWbnur SUNWbnuu SUNWpppk SUNWpppkx SUNWapppr SUNWappp
\ce

Next, as root, copy <installDir>/target/config/solaris/asppp.cf to the /etc directory, as follows (replace the leading path segment <installDir>/target/ with the path to your installed Tornado tree):

\cs
    # cp <installDir>/target/config/solaris/asppp.cf /etc
\ce

`CAUTION:' If you already have aspppd running, stop it with asppp stop before proceeding.

Finally, start the PPP daemon aspppd by typing the following as root:

\cs
    # /etc/init.d/asppp start
\ce

The PPP driver is now installed and running on your Solaris system, and will be restarted automatically when Solaris reboots.

The PPP configuration assigns IP addresses 192.168.255.1 through 192.168.255.16 to sixteen devices, and associates with them the peer system names vxsim0 through vxsim15, respectively as configured in asppp.cf.

If those IP addresses are not suitable, you can update them in asppp.cf, and modify VXSIM_IP_ADDR parameter from the workspace (the default is "192.168.255.%d"). This parameter belongs to "hardware>BSP configuration variants" component. You may want to directly change that parameter in bsp.cdf and config.h bsp files. Finally, copy the asppp.cf file as explained above and rebuild your project or your bsp.

You can also use the following commands to start or stop the Solaris PPP driver after the driver has been installed (you must have root privileges):

\cs
    # /etc/init.d/asppp start
    # /etc/init.d/asppp stop
\ce

If you get the following error message at simulator startup, you need to check access rights to the file /tmp/.asppp.fifo:

\cs
	Can't open /tmp/.asppp.fifo
	solaris_ppp_init failed
	Attaching network interface ppp0...
	ppp0: ppp 2.1.2 started by 
	ppp0: Connect: ppp0 <--> /tyCo/1
	ppp0: timeout: could not establish link with peer.
	Unable to initialize PPP connection.
\ce

Whereas, the following output requires a different response:

\cs
	% ls -l /tmp/.asppp.fifo
	prw------- 1 root other 0 Oct 18 17:04 /tmp/.asppp.fifo|
\ce

In this case, as root, add the following command to the /etc/init.d/asppp file:

\cs
	% chmod a+rw /tmp/.asppp.fifo
\ce

Add the preceding command line at the end of the following code:

\cs
	% su root
	password:
	# vi /etc/init.d/asppp
	
	# Start the aspppd daemon
	/usr/sbin/aspppd -d 1 ||
	echo "aspppd not started, see /var/adm/log/asppp.log"
\ce

Save the file and restart PPP.

`WARNING:' The above problem is likely to occur on Solaris 2.8 hosts only.

`CAUTION:' If you want to use Tornado 2.0 and Tornado 2.2 Solaris simulators on the same machine, you should assign similar IP addresses for both simulators, that is, those declared in the asppp.cf file. For Tornado 2.2, default addresses are of the form 192.168.255.\<ProcessorNumber + 1\>, whereas, for Tornado 2.0, they are of the form 127.0.1.\<ProcessorNumber\>.

In order to use the two simulators simultaneously, you must modify either the Tornado 2.2 IP addresses or the Tornado 2.0 IP addresses (the better solution). For Tornado 2.0, the following modifications are required:

In the file sysLib.c (in the directory <installDir>/target/config/solaris), modify the values of the global variables vxsim_gateway and vxsim_ip_addr. Change from:

\cs
	...
	#elif defined(INCLUDE_PPP)
	char *vxsim_bootdev = "ppp";
	char *vxsim_gateway = "g=127.0.1.254";
	#else
	...
	char *vxsim_ip_addr = "127.0.1.%d";
\ce
To:
\cs
	char *vxsim_gateway = "g=192.168.255.254";
	char *vxsim_ip_addr = "192.168.255.%d";
\ce

Find the following line in the sysHwInit2( ) routine:

\cs
	sprintf (target_ip, vxsim_ip_addr, sysProcNumGet ());
\ce

Replace it with:

\cs
	sprintf (target_ip, vxsim_ip_addr, (sysProcNumGet () + 1));
\ce

Then rebuild your system.


`Loading PPP on a Solaris 2.9 Host'

First, use the command pkginfo to check whether the following packages are installed on your host:

\cs
    SUNWpppd	Solaris PPP Device Drivers
    SUNWpppdr	Solaris PPP configuration files
    SUNWpppdu	Solaris PPP daemon and utilities
    SUNWpppdt	Solaris PPP Tunneling
    SUNWpppdx	Solaris PPP Device Drivers (64-bit)
    SUNWpppg	GNU utilities for PPP
    SUNWbnur	Networking UUCP Utilities, (Root)
    SUNWbnuu	Networking UUCP Utilities, (Usr)
\ce

For example:

\cs
    % pkginfo | egrep  'ppp|bnu'
    system      SUNWbnur       Networking UUCP Utilities, (Root)
    system      SUNWbnuu       Networking UUCP Utilities, (Usr)
    system      SUNWpppd       Solaris PPP Device Drivers
    system      SUNWpppdr      Solaris PPP configuration files
    system      SUNWpppdt      Solaris PPP Tunneling
    system      SUNWpppdu      Solaris PPP daemon and utilities
    system      SUNWpppdx      Solaris PPP Device Drivers (64-bit)
    system      SUNWpppg       GNU utilities for PPP
\ce

The SUNWpppdx package should only be installed on Solaris hosts running in 64-bit mode. Use the isainfo -b command to determine the mode in which your Solaris 2.9 host runs. This command returns 32 or 64. If the packages are not already installed, mount the Solaris installation disk and change your working directory to the location of these packages and install them using the following commands:

For a Solaris host running in 32 bits:

\cs
    % isainfo -b
    32
    % su root
    Password:
    # pkgadd -d  `pwd` SUNWbnur SUNWbnuu SUNWpppd SUNWpppdr SUNWpppdt SUNWpppdu SUNWpppg
\ce

For a Solaris host running in 64 bits:

\cs
    % isainfo -b
    64
    % su root
    Password:
    # pkgadd -d  `pwd` SUNWbnur SUNWbnuu SUNWpppd SUNWpppdr SUNWpppdt SUNWpppdu SUNWpppdx SUNWpppg
\ce

Next, still as root, copy <installDir>/target/config/solaris/vxsimppp to the /etc/init.d directory, as follows (replace the leading path segment <installDir>/target/ with the path to your installed Tornado tree). If you want PPP daemons to be automatically started during Solaris startup, then you need to create a link in /etc/rc2.d:

\cs
    # cp <installDir>/target/config/solaris/vxsimppp /etc/init.d
    # ln -s /etc/init.d/vxsimppp /etc/rc2.d/S80vxsimppp
\ce

Finally, start the PPP daemons pppd by typing the following as root:

\cs
    # /etc/rc.d/init.d/vxsimppp start
\ce

The following table describe the configurable parameters of that script. The default values will usually be appropriate, but make sure there is no conflict with other applications.

\ts
Parameters                  | Default       | Description
----------------------------+---------------+------------------------------------
PPP_MASTER_PSEUDO_TTY_PATH  | "/dev/ttyr%x" | PPP master pseudo terminal path. This value must be coherent with the slave pseudo terminal path defined in the project facilty (parameter PPP_PSEUDO_TTY_PATH of INCLUDE_SOLARIS_NET_CONFIG component). By default the master is "/dev/ttyr%x" and slave is "/dev/ptyr%x".
ENABLE_REMOTE_ACCESS        | 1 (true)      | To enable remote access of VxSim targets, IP fowarding must be enabled. This is done by setting the solaris kernel parameter ip_forwarding to true using ndd command. To disable this facility reset ENABLE_REMOTE_ACCESS to 0.
FIRST_VXSIM                 | 0             | This defines the simulator IP address range, so by default:
LAST_VXSIM                  | 15            | vxsim0	192.168.255.1
                            |               | vxsim1	192.168.255.2
                            |               | ...	...
                            |               | vxsim15	192.168.255.16
DEBUG                       | 0 (false)     | Enables PPP connection debugging facilities. If this option is given, pppd will log the contents of all control packets sent or received in a readable form. The packets are logged through syslog with facility daemon and level debug. This information can be directed to a file by setting up /etc/syslog.conf appropriately (see syslog.conf manual).
\te

The PPP daemons are now installed and running on your Solaris system, and will be restarted automatically when Solaris reboots.

The PPP configuration assigns IP addresses 192.168.255.1 through 192.168.255.16 to sixteen devices. You may want to match these IP addresses with host names in /etc/hosts: for example vxsim vxsim0 through vxsim15.

If those IP addresses are not suitable, you can modify VXSIM_IP_ADDR parameter from the workspace (the default is "192.168.255.%d"). This parameter belongs to "hardware>BSP configuration variants" component. You may want to directly change that parameter in bsp.cdf and config.h bsp files. Finally, rebuild your project or your bsp.

You can also use the following commands to start or stop the Solaris PPP driver after the driver has been installed (you must have root privileges):

\cs
    # /etc/init.d/vxsimppp start
    # /etc/init.d/vxsimppp stop
\ce

For further information, follow the instructions in the
.I "Configuring the VxSim Full Simulator"
sub-chapter of the
.I "VxSim"
chapter of the
.I "Tornado User's Guide".

.SH "SPECIAL CONSIDERATIONS"
This simulator is provided for Solaris version 2.7 and upper.
.PP
The system memory is allocated with u_malloc() (UNIX malloc(3)) based on the
LOCAL_MEM_SIZE parameter in config.h.  A minimum of 3 Megabytes is required.
.PP
A ^X causes reboot() to be called, resetting the network driver before
calling sysToMonitor().  Memory is effectively cleared during
a reboot via u_execvp() from within sysToMonitor().
.PP
To exit the simulator, type ^\ (quit) or reboot() with BOOT_NO_AUTOBOOT(1),
which returns to your UNIX shell.  UNIX job control is also supported.
.PP
VxSim can use any VxWorks file system. The default file system is the 
pass-through file system, passFs, which is unique to VxSim. For further
information, please refer to
.I "File Systems"
sub-chapter of the
.I "Integrated Simulator"
chapter of the 
.I "Tornado User's Guide"

.PP
Options may be passed to VxWorks from the command line:
.PP
.nf
    -p <proc num>  (set processor number)
    -r <ram bytes> (set memory size)
.fi

.SH "SEE ALSO"
.pG "Getting Started, Configuration"
and
.I "VxSim"
chapter of the
.I "Tornado User's Guide".
