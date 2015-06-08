     This directory includes a set of scripts which will add event points
to the network stack for use with the WindView visualization tool. Each
script is executed from the Tornado Shell. Executing the commands in 
the WvNetSetup.scp file alters the options of the network task to enable
the use of event points. Similarly, executing the WvNetCleanup.scp file will
restore the original settings.
     The remaining script files will set event points for particular 
components of the network stack as follows:

		Script File			Component
		-----------			---------
		MuxEventSet.scp			MUX interface
                RouteEventSet.scp		Routing library
                NetBufEventSet.scp		Network pool manager
		IpEventSet.scp			IP network protocol
		UdpEventSet.scp			UDP transport protocol
		TcpEventSet.scp			TCP transport protocol
		BsdSockEventSet.scp		BSD sockets API

     Each of these scripts should only be executed if the corresponding
stack component is actually present. In addition, the IpEventSet.scp file
contains commands to add event points for three optional routines in the 
ICMP and IGMP network protocols (icmpErrorHook, igmpJoinGrpHook, and
igmpLeaveGrpHook). If any of those routines are installed, the commment mark
preceding the associated command may be removed to allow recording of the
event. 
     The events in these scripts are assigned the first 51 user event
identifiers (user events 0 - 50, inclusive). The eventlist.text file
lists the event identifiers assigned to each of the monitored functions
in the network stack. A similar listing is contained in the eventlist.ps
file, which includes the user-defined icons for each event. These icons
can be made available to WindView either by copying the files to the 
WindView resources directory or by setting the WV_USER_ICONS environment 
variable appropriately. For complete instructions on using these icons, see 
the WindView User's Guide.
