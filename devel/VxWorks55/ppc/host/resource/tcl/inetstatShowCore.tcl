# inetstatShowCore.tcl - Implementation of inetstatShow core routines in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the inetstatShow core routines.
#
# RESOURCE FILES
# 


#############################################################################
#
# inpcbInfoGet - get info about an active connection for an IP socket
#
# Information is grabbed from the inpcb structure defined in 
# /netinet/in_pcb.h:
# .CS
#     typedef struct inpcb 
#         {
#         LIST_ENTRY(inpcb) inp_list;
#         LIST_ENTRY(inpcb) inp_hash;
#         struct  inpcbinfo *inp_pcbinfo;
#         struct  in_addr inp_faddr;
#         u_short inp_fport;
#         struct  in_addr inp_laddr;
#         u_short inp_lport;
#         struct  socket *inp_socket;
#         caddr_t inp_ppcb;
#         struct  route inp_route;
#         int     inp_flags;
#         struct  ip inp_ip;
#         struct  mbuf *inp_options;
#         struct  ip_moptions *inp_moptions;
#         };
#
#     #define LIST_ENTRY(type)       \
#         struct                     \
#             {                      \
#             struct type *le_next;  \
#             struct type **le_prev; \
#             }
#
#     typedef struct socket
#         {
#         short       so_type;
#         short       so_options;
#         short       so_linger;
#         short       so_state;
#         caddr_t     so_pcb;
#         struct      protosw *so_proto;
#         int         so_userArg;
#         SEMAPHORE   so_timeoSem;
#         int         so_fd;
#         struct      socket *so_head;
#         struct      socket *so_q0;
#         struct      socket *so_q;
#         short       so_q0len;
#         short       so_qlen;
#         short       so_qlimit;
#         short       so_timeo;
#         u_short     so_error;
#         short       so_pgrp;
#         u_long      so_oobmark;
#         struct      sockbuf
#             {
#             u_long          sb_cc;
#             u_long          sb_hiwat;
#             u_long          sb_mbcnt;
#             u_long          sb_mbmax;
#             u_long          sb_lowat;
#             struct          mbuf *sb_mb;
#             struct          proc *sb_sel;
#             short           sb_timeo;
#             short           sb_flags;
#             SEMAPHORE       sb_Sem;
#             int             sb_want;
#             } so_rcv, so_snd;
#         void        (*so_upcall) (struct socket *so, caddr_t arg, int waitf);
#         caddr_t     so_upcallarg;
#         SEL_WAKEUP_LIST so_selWakeupList;
#         };
# .CE
#
# SYNOPSIS:
#   inpcbInfoGet nodeId 
#
# PARAMETERS:
#   nodeId : id of the connection
#
# RETURNS:
#   a list of 16 items:
#       inp_faddr: (four bytes) foreign internet address, 
#       inp_fport: (two bytes)  foreign port,
#       inp_laddr: (four bytes) local internet address, 
#       inp_lport: (two bytes)  local port, 
#       inp_ppcb : pointer to per-protocol pcb,
#       inp_list.le_next : next PCBs of this proto,
#       inp_socket->so_rcv.sb_cc: Recv-Q,
#       inp_socket->so_snd.sb_cc: Send-Q
#
# ERRORS: N/A
#

proc inpcbInfoGet {nodeId} {

    global cpuFamily
    global __wtxCpuType

    # Get the list from the inpcb structure

    # Because alignment of short int are differents between architecture,
    # we have two differents call to wtxGopherEval

    # Call for 68k architecture

    if {$cpuFamily($__wtxCpuType) == "m68k"} {
	return [wtxGopherEval "$nodeId +20  @b@b@b@b @b@b  @b@b@b@b @b@b +4<*!>\
		 -36   <* !> +32 <*  +80  @ +60  @>"]
    } else {

	# Call for sparc, solaris, i960, ppc, mips, i86 architectures

	return [wtxGopherEval "$nodeId +20  @b@b@b@b @b@b +2  @b@b@b@b @b@b\
				 +6<*!> -40   <* !> +36 <*  +80  @ +60  @>"]
    }
}


#############################################################################
#
# inpcbPrint - display info about an active connection for an IP socket
#
# Display info about an active connection for an IP socket
#
# SYNOPSIS:
#   inpcbPrint name inpcbName
#
# PARAMETERS:
#   name : name of the protocol (TCP or UDP)
#   inpcbName : name of the global variable refering to the first connection
#		(tcpcb or udb)
#
# RETURNS: N/A
#
# ERRORS: 
#   <inpcbName> not found
#

proc inpcbPrint {name inpcbName} {

    # get the value of the symbol if it exists

    set inpcbId [symAddrGet $inpcbName "$inpcbName not found"]
    set nodeId [wtxGopherEval "$inpcbId*!"]

    set isTcp 0
    if {$name == "TCP"} {set isTcp 1}

    while {($nodeId != $inpcbId) && ($nodeId != 0)} {
  	set nodeInfo [inpcbInfoGet $nodeId]

	# display nodeId
	
	puts -nonewline stdout [format "%-8x " $nodeId]

	# display name Recv-Q Send-Q

	puts -nonewline stdout [format "%-5.5s %6d %6d " $name \
		[lindex $nodeInfo 14] [lindex $nodeInfo 15]]
	
	# display local internet address and port

	puts -nonewline stdout [format " %-18.18s" [format "%s.%d" \
			[ifAddrFormat [lrange $nodeInfo 6 9]]\
			 [twoBytesPack [lrange $nodeInfo 10 11]] ]]

	# display foreign internet address and port

	puts -nonewline stdout [format " %-18.18s" [format "%s.%d"\
			 [ifAddrFormat [lrange $nodeInfo 0 3]]\
			[twoBytesPack  [lrange $nodeInfo 4 5]] ]]
	if {$isTcp} {
	    set tcpcb [lindex $nodeInfo 12]
	    set tState [wtxGopherEval "$tcpcb * +8@w"]

	    # test TCP_NSTATES = 11 constante

	    if {($tState < 0) || ($tState >= 11)} {
		puts -nonewline stdout [format " %d" $tState]
	    } else {
		puts -nonewline stdout [format " %s" [lindex {CLOSED \
				LISTEN      \
				SYN_SENT    \
				SYN_RCVD    \
				ESTABLISHED \
				CLOSE_WAIT  \
				FIN_WAIT_1  \
				CLOSING     \
				LAST_ACK    \
				FIN_WAIT_2  \
				TIME_WAIT} $tState]]
	    }
	}
	puts stdout ""

	# get next nodeId

	set nodeId [lindex $nodeInfo 13]
    }
}
