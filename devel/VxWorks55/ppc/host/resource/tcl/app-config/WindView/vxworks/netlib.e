#
# netlib.e - WindView event table for networking subsystem - note that this is
# executable TCL code, although the definition of wvEvent in
# database.tcl is required before this file will execute correctly.
#
# Copyright 1995 - 2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01e,11mar02,tcr  Change range for help ids
# 01d,11dec01,tcr  Correct description for WV_NETEVENT_IPIN_FRAGDROP
#                  Fix up help ids
# 01c,01nov01,rae  Corrected event 20039 (SPR # 71284)
# 01b,22oct01,cpd  imported from T3
# 01a,12jan00,spm  written
#

#
# The format is:-
#
#  wvEvent EVENT_TITLE eventId|eventIdRange eventFlags eventParams
#
# where eventId is a single numeric ID (matching that in eventP.h),
# eventIdRange is the lower and upper event-IDs separated by a dash,
# eventParams is a Tcl-list, where each element is a
# <type name> pair (type is UINT32 or STRING and name is the
# parameter's textual name), and eventFlags is a Tcl-list of zero or 
# more option-selections, which can be:-
#
#  -name=niceName 
#  -notimestamp
#  -class=className
#  -helpid=helpid as integer
#  -trigger=true (if the event can fire a trigger)
#
# and the result is processed as an event-definition.
#

# The following events are defined in if.c

wvEvent WV_NETEVENT_IFINIT_START 20000 {
    -name=ifinit-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20000
} {
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFRESET_START 20001 {
    -name=ifreset-start
    -class=auxnet
    -helpid=20001
} {
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFATTACH_START 20002 {
    -name=if_attach-start
    -class=auxnet
    -icon=wvnet-attach
    -helpid=20002
} {
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFDETACH_START 20003 {
    -name=if_dettach-start
    -class=auxnet
    -icon=wvnet-detach
    -helpid=20003
} {
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFAFREE_PANIC 20004 {
    -name=ifafree-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20004
} {
    UINT32 wvNetEventId 
}

#
# The following event is currently unused. The link_rtrequest() routine
# is the default handler for altering any link-level information associated
# with routes. It is always replaced with arp_rtrequest for Ethernet devices,
# so it is never called.
#
# wvEvent WV_NETEVENT_LINKRTREQ_FAIL 20005 {
#    -name=link_rtrequest-fail 
#    -class=auxnet 
#    -icon=wvnet-error
#} {
#     UINT32 wvNetEventId 
#}

wvEvent WV_NETEVENT_IFDOWN_START 20006 {
    -name=if_down-start
    -class=auxnet
    -helpid=20005
} {
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFUP_START 20007 {
    -name=if_up-start
    -class=auxnet
    -helpid=20006
} {
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFWATCHDOG 20008 {
    -name=if_slowtimo-ifWdStart
    -class=auxnet
    -helpid=20007
} {
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFIOCTL_START 20009 {
    -name=ifioctl-start
    -class=auxnet
    -icon=ifctl-start
    -helpid=20008
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFIOCTL_BADIFNAME 20010 {
    -name=ifioctl-badifname
    -class=auxnet
    -icon=wvnet-error
    -helpid=20009
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFIOCTL_NOPROTO 20011 {
    -name=ifioctl-noproto
    -class=auxnet
    -icon=wvnet-error
    -helpid=20010
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFPROMISC_START 20012 {
    -name=ifpromisc-start
    -class=auxnet
    -icon=ifctl-start
    -helpid=20011
} {
    UINT32 pswitch
    UINT32 pIf
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IFPROMISC_FAIL 20013 {
    -name=ifpromisc-ifdown
    -class=auxnet
    -icon=wvnet-error
    -helpid=20012
} {
    UINT32 pswitch
    UINT32 pIf
    UINT32 wvNetEventId 
}

# The following events are defined in if_ether.c

wvEvent WV_NETEVENT_ARPTIMER_FREE 20014 {
    -name=arptimer-free
    -class=auxnet
    -helpid=20013
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPRTREQ_START 20015 {
    -name=arp_rtrequest-start
    -class=auxnet
    -icon=arpctl-start
    -helpid=20014
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPRTREQ_BADGATE 20016 {
    -name=arp_rtrequest-badgate
    -class=auxnet
    -icon=wvnet-error
    -helpid=20015
} {
    UINT32 gateLength
    UINT32 gateFamily
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPRTREQ_FAIL 20017 {
    -name=arp_rtrequest-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20016
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPREQ_SEND 20018 {
    -name=arprequest-send
    -class=auxnet
    -helpid=20017
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPREQ_FAIL 20019 {
    -name=arprequest-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20018
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPRESOLV_START 20020 {
    -name=arpresolve-start
    -class=auxnet
    -helpid=20019
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPLOOK_FAIL 20021 {
    -name=arpresolve-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20020
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPINTR_START 20022 {
    -name=arpintr-start
    -class=auxnet
    -helpid=20021
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPINTR_FAIL 20023 {
    -name=arpintr-panic
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20022
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPIN_START 20024 {
    -name=in_arpinput-start
    -class=auxnet
    -helpid=20023
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPIN_BADADDR 20025 {
    -name=in_arpinput-etherbcast
    -class=auxnet
    -icon=arpinput-error
    -helpid=20024
} {
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIN_BADADDR2 20026 {
    -name=in_arpinput-dupaddr
    -class=auxnet
    -icon=arpinput-error
    -helpid=20025
} {
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIN_BADADDR3 20027 {
    -name=in_arpinput-newetheraddr
    -class=auxnet
    -helpid=20026
} {
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIN_DELAYEDSEND 20028 {
    -name=in_arpinput-delayedsend
    -class=corenet
    -helpid=20027
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIN_REPLYSEND 20029 {
    -name=in_arpinput-replysend
    -class=auxnet
    -helpid=20028
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPTFREE_PANIC 20030 {
    -name=arptfree-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20029
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ARPIOCTL_NOTSUPP 20031 {
    -name=arpioctl-badreq
    -class=auxnet
    -icon=wvnet-error
    -helpid=20030
} {
    UINT32 reqHwFamily
    UINT32 reqProtoFamily
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIOCTL_START 20032 {
    -name=arpioctl-start
    -class=auxnet
    -icon=arpctl-start
    -helpid=20031
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ARPIOCTL_SEARCHFAIL 20033 {
    -name=arpioctl-noentry
    -class=auxnet
    -icon=wvnet-error
    -helpid=20032
} {
    UINT32 ipAddr
    UINT32 request
    UINT32 wvNetEventId
}

# The following events are defined in if_subr.c

wvEvent WV_NETEVENT_ETHEROUT_IFDOWN 20034 {
    -name=ether_output-ifdown
    -class=corenet
    -icon=ether_output-error
    -helpid=20033
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHEROUT_NOROUTE 20035 {
    -name=ether_output-noroute
    -class=corenet
    -icon=ether_output-error
    -helpid=20034
} {
    UINT32 ipAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHEROUT_RTREJECT 20036 {
    -name=ether_output-rtreject
    -class=corenet
    -icon=ether_output-error
    -helpid=20035
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ETHEROUT_NOBUFS 20037 {
    -name=ether_output-nomem
    -class=corenet
    -icon=wvnet-nomem
    -helpid=20036
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHEROUT_AFNOTSUPP 20038 {
    -name=ether_output-afnotsupp
    -class=corenet
    -icon=ether_output-error
    -helpid=20037
} {
    UINT32 addrFamily
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHEROUT_FINISH 20039 {
    -name=ether_output-send
    -class=corenet
    -helpid=20038
} {
    UINT32 error
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHERIN_START 20040 {
    -name=ether_input-start
    -class=corenet
    -helpid=20039
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHERIFATTACH_START 20041 {
    -name=ether_ifattach-start
    -class=auxnet
    -icon=wvnet-attach
    -helpid=20040
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

#
# This ether_addmulti event defined in the if_subr.c file is currently
# unused. The ether_addmulti() routine adds multicast address for BSD
# Ethernet drivers, but those VxWorks drivers do not support multicasting.
#
# wvEvent WV_NETEVENT_ADDMULT_START 20042 {
#     -name=ether_addmulti-start
#     -class=auxnet
#     -icon=etherctl-start
# } {
#     UINT32 pIf
#     UINT32 wvNetEventId
# }

wvEvent WV_NETEVENT_ADDMULT_AFNOTSUPP 20043 {
    -name=ether_addmulti-afnotsupp
    -class=auxnet
    -icon=wvnet-error
    -helpid=20041
} {
    UINT32 addrFamily
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ADDMULT_BADADDR 20044 {
    -name=ether_addmulti-badaddr
    -class=auxnet
    -icon=wvnet-error
    -helpid=20042
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ADDMULT_NOBUFS 20045 {
    -name=ether_addmulti-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20043
} {
    UINT32 wvNetEventId
}

#
# This ether_delmulti event defined in the if_subr.c file is currently
# unused. The ether_delmulti() routine removes multicast address for BSD
# Ethernet drivers, but those VxWorks drivers do not support multicasting.
#
# wvEvent WV_NETEVENT_DELMULT_START 20046 {
#    -name=ether_delmulti-start    -class=auxnet    -icon=etherctl-start} {
#     UINT32 pIf
#     UINT32 wvNetEventId
# }

wvEvent WV_NETEVENT_DELMULT_AFNOTSUPP 20047 {
    -name=ether_delmulti-afnotsupp
    -class=auxnet
    -icon=wvnet-error
    -helpid=20044
} {
    UINT32 addrFamily
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_DELMULT_BADADDR 20048 {
    -name=ether_delmulti-searchfail
    -class=auxnet
    -icon=wvnet-error
    -helpid=20045
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ETHERATTACH_START 20049 {
    -name=ether_attach-start
    -class=auxnet
    -icon=wvnet-attach
    -helpid=20046
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TYPEADD_FAIL 20050 {
    -name=netTypeAdd-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20047
} {
    UINT32 etherType
    UINT32 wvNetEventId
}

# The following events are defined in igmp.c

wvEvent WV_NETEVENT_IGMPINIT_START 20051 {
    -name=igmp_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20048
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IGMPIN_SHORTMSG 20052 {
    -name=igmp_input-shortmsg
    -class=corenet
    -icon=igmp_input-error
    -helpid=20049
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IGMPIN_BADSUM 20053 {
    -name=igmp_input-badsum
    -class=corenet
    -icon=igmp_input-error
    -helpid=20050
} {
     UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IGMPIN_BADADDR 20054 {
    -name=igmp_input-badaddr
    -class=corenet
    -icon=igmp_input-error
    -helpid=20051
} {
    UINT32 igmpAddr
    UINT32 ipAddr
    UINT32 igmpType
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IGMPIN_FINISH 20055 {
    -name=igmp_input-recv
    -class=corenet
    -helpid=20052
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IGMPJOIN_START 20056 {
    -name=igmp_joingroup-start
    -class=auxnet
    -icon=igmpctl-start
    -helpid=20053
} {
    UINT32 mcastAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IGMPLEAVE_START 20057 {
    -name=igmp_leavegroup-start
    -class=auxnet
    -icon=igmpctl-start
    -helpid=20054
} {
    UINT32 mcastAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IGMPFASTTIMER_START 20058 {
    -name=igmp_fasttimo-start
    -class=auxnet
    -helpid=20055
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IGMPSENDREP_NOBUFS 20059 {
    -name=igmp_sendreport-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20056
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IGMPSENDREP_FINISH 20060 {
    -name=igmp_sendreport-send
    -class=corenet
    -helpid=20057
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
} 

# The following events are defined in in.c

wvEvent WV_NETEVENT_INCTRL_START 20061 {
    -name=in_control-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20058
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INCTRL_SEARCHFAIL 20062 {
    -name=in_control-noifdata
    -class=auxnet
    -icon=wvnet-error
    -helpid=20059
} {
    UINT32 pIf
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INCTRL_BADSOCK 20063 {
    -name=in_control-notpriv
    -class=auxnet
    -icon=wvnet-error
    -helpid=20060
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INCTRL_PANIC 20064 {
    -name=in_control-panic-noifp
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20061
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INCTRL_NOBUFS 20065 {
    -name=in_control-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20062
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INCTRL_BADFLAGS 20066 {
    -name=in_control-badflags
    -class=auxnet
    -icon=wvnet-error
    -helpid=20063
} {
    UINT32 pIf
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INROUTEDEL_START 20067 {
    -name=in_ifscrub-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20064
} {
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INIFINIT_START 20068 {
    -name=in_ifinit-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20065
} {
    UINT32 ipAddr
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INIFADDRDEL_START 20069 {
    -name=in_ifaddr_remove-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20066
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_INADDMULT_START 20070 {
    -name=in_addmulti-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20067
} {
    UINT32 mcastAddr
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INADDMULT_NOBUFS 20071 {
    -name=in_addmulti-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20068
} {
    UINT32 mcastAddr
    UINT32 pIf
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_INDELMULT_START 20072 {
    -name=in_delmulti-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20069
} {
    UINT32 wvNetEventId }

# The following events are defined in in_pcb.c

wvEvent WV_NETEVENT_PCBALLOC_NOBUFS 20073 {
    -name=in_pcballoc-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20070
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBBIND_NOADDR 20074 {
    -name=in_pcbbind-noaddr
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20071
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBBIND_BADSOCK 20075 {
    -name=in_pcbbind-badsock
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20072
} {
    UINT32 localPort
    UINT32 localAddr
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBBIND_BADADDRLEN 20076 {
    -name=in_pcbbind-badaddrlen
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20073
} {
    UINT32 addrLen
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBBIND_BADADDR 20077 {
    -name=in_pcbbind-badaddr
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20074
} {
    UINT32 localAddr
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBBIND_BADPORT 20078 {
    -name=in_pcbbind-badport
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20075
} {
    UINT32 localPort
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBLADDR_BADADDR 20079 {
    -name=in_pcbladdr-badaddr
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20076
} {
    UINT32 addrPort
    UINT32 addrFam
    UINT32 addrLen
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBLADDR_BADIF 20080 {
    -name=in_pcbladdr-badif
    -class=auxnet
    -icon=pcbbind-error
    -helpid=20077
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBCONNECT_BADADDR 20081 {
    -name=in_pcbconnect-badaddr
    -class=auxnet
    -helpid=20078
} {
    UINT32 localPort
    UINT32 localAddr
    UINT32 dstAddr
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPTIMER_ROUTEDROP 20082 {
    -name=in_losing-start
    -class=corenet
    -helpid=20079
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_PCBBIND_START 20083 {
    -name=in_pcbbind-start
    -class=auxnet
    -helpid=20080
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBCONNECT_START 20084 {
    -name=in_pcbconnect-start
    -class=auxnet
    -helpid=20081
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBDISCONN_START 20085 {
    -name=in_pcbdisconnect-start
    -class=auxnet
    -helpid=20082
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_PCBNOTIFY_START 20086 {
    -name=in_pcbnotify-start
    -class=auxnet
    -helpid=20083
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 command
    UINT32 wvNetEventId
}

# The following events are defined in ip_icmp.c

wvEvent WV_NETEVENT_ICMPERR_START 20087 {
    -name=icmp_error-start
    -class=corenet
    -helpid=20084
} {
    UINT32 errCode
    UINT32 errType
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ICMPERR_PANIC 20088 {
    -name=icmp_error-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20085
} {
    UINT32 errType
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ICMPIN_SHORTMSG 20089 {
    -name=icmp_input-shortmsg
    -class=corenet
    -icon=icmp_input-error
    -helpid=20086
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ICMPIN_BADSUM 20090 {
    -name=icmp_input-badsum
    -class=corenet
    -icon=icmp_input-error
    -helpid=20087
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ICMPIN_BADLEN 20091 {
    -name=icmp_input-badlen
    -class=corenet
    -icon=icmp_input-error
    -helpid=20088
} {
    UINT32 ipHdrLen
    UINT32 msgLen
    UINT32 wvNetEventId
}

#
# These two icmp_sysctl events defined in the ip_icmp.c file are currently
# unused. The icmp_sysctl() routine implements the (Unix) sysctl command
# (which is not provided with VxWorks) for the ICMP protocol.
#
# wvEvent WV_NETEVENT_ICMPCTRL_BADLEN 20092 {
#    -name=icmp_sysctl-badlen
#     -class=auxnet
#     -icon=wvnet-error
# } {
#     UINT32 namelen
#     UINT32 wvNetEventId
# }
#
# wvEvent WV_NETEVENT_ICMPCTRL_BADCMD 20093 {
#    -name=icmp_sysctl-badcmd
#     -class=auxnet
#     -icon=wvnet-error
# } {
#     UINT32 ctrlType
#     UINT32 wvNetEventId
#}

wvEvent WV_NETEVENT_ICMPIN_START 20094 {
    -name=icmp_input-start
    -class=corenet
    -helpid=20089
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_ICMPREFLECT_START 20095 {
    -name=icmp_reflect-start
    -class=corenet
    -helpid=20090
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ICMPSEND_START 20096 {
    -name=icmp_send-start
    -class=corenet
    -helpid=20091
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

#
# This icmp_sysctl event defined in the ip_icmp.c file is currently
# unused. The icmp_sysctl() routine implements the (Unix) sysctl command
# (which is not provided with VxWorks) for the ICMP protocol.
#
# wvEvent WV_NETEVENT_ICMPCTRL_START 20097 {
#    -name=icmp_sysctl-start
#     -class=auxnet
# } {
#     UINT32 ctrlType
#     UINT32 wvNetEventId
# }

# The following events are defined in ip_input.c

wvEvent WV_NETEVENT_IPINIT_PANIC 20098 {
    -name=ip_init-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20092
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPIN_PANIC 20099 {
    -name=ipintr-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20093
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPIN_SHORTMSG 20100 {
    -name=ipintr-shortmsg
    -class=corenet
    -icon=ip_input-error
    -helpid=20094
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPIN_BADVERS 20101 {
    -name=ipintr-badvers
    -class=corenet
    -icon=ip_input-error
    -helpid=20095
} {
    UINT32 version
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPIN_BADHLEN 20102 {
    -name=ipintr-badhlen
    -class=corenet
    -icon=ip_input-error
    -helpid=20096
} {
    UINT32 hdrLength
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPIN_BADSUM 20103 {
    -name=ipintr-badsum
    -class=corenet
    -icon=ip_input-error
    -helpid=20097
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPIN_BADLEN 20104 {
    -name=ipintr-badlen
    -class=corenet
    -icon=ip_input-error
    -helpid=20098
} {
    UINT32 hdrLength
    UINT32 ipLength
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPIN_BADMBLK 20105 {
    -name=ipintr-badmblk
    -class=corenet
    -icon=ip_input-error
    -helpid=20099
} {
    UINT32 ipLength
    UINT32 mblkLength
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPIN_BADOPT 20106 {
    -name=ip_dooptions-badopt
    -class=corenet
    -icon=ip_input-error
    -helpid=20100
} {
    UINT32 errCode
    UINT32 errType
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPFWD_BADADDR 20107 {
    -name=ip_forward-badaddr
    -class=corenet
    -icon=ip_output-error
    -helpid=20101
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

#
# These two ip_sysctl events defined in the ip_input.c file are currently
# unused. The ip_sysctl() routine implements the (Unix) sysctl command
# (which is not provided with VxWorks) for the IP protocol.
#
# wvEvent WV_NETEVENT_IPCTRL_BADCMDLEN 20108 {
#     -name=ip_sysctl-badlen
#     -class=auxnet
#     -icon=wvnet-error
# } {
#     UINT32 namelen
#     UINT32 wvNetEventId
# }
#
# wvEvent WV_NETEVENT_IPCTRL_BADCMD 20109 {
#     -name=ip_sysctl-badcmd
#     -class=auxnet
#     -icon=wvnet-error
# } {
#     UINT32 ctrlType
#     UINT32 wvNetEventId
# }

wvEvent WV_NETEVENT_IPIN_FRAGDROP 20110 {
    -name=ipReAssemble-fragdrop
    -class=corenet
    -icon=ip-fragdrop
    -helpid=20102
} {
    UINT32 ipHdrFrag
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPIN_FINISH 20111 {
    -name=ipintr-recv
    -class=corenet
    -icon=ip_input-start
    -helpid=20103
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPFWD_STATUS 20112 {
    -name=ip_forward-result
    -class=corenet
    -helpid=20104
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 result
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPIN_GOODMBLK 20113 {
    -name=ipintr-newpkt
    -class=corenet
    -helpid=20105
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPFRAGFREE_START 20114 {
    -name=ip_freef-start
    -class=auxnet
    -helpid=20106
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPTIMER_FRAGFREE 20115 {
    -name=ip_slowtimo-start
    -class=auxnet
    -helpid=20107
} {
    UINT32 wvNetEventId }

#
# This ip_sysctl event defined in the ip_input.c file is currently
# unused. The ip_sysctl() routine implements the (Unix) sysctl command
# (which is not provided with VxWorks) for the IP protocol.
#
# wvEvent WV_NETEVENT_IPCTRL_START 20116 {
#    -name=ip_sysctl-start
#     -class=auxnet
#     -icon=ipctl-start
# } {
#     UINT32 ctrlType
#     UINT32 wvNetEventId 
# }

wvEvent WV_NETEVENT_IPINIT_START 20117 {
    -name=ip_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20108
} {
    UINT32 wvNetEventId }

# The following events are defined in ip_mroute.c

wvEvent WV_NETEVENT_ADDMRT_NOBUFS 20118 {
    -name=add_mrt-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20109
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPMRT_NOTINIT 20119 {
    -name=ip_mrouter_cmd-notinit
    -class=auxnet
    -icon=wvnet-error
    -helpid=20110
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPMRT_BADCMD 20120 {
    -name=ip_mrouter_cmd-badcmd
    -class=auxnet
    -icon=wvnet-error
    -helpid=20111
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ADDVIF_BADINDEX 20121 {
    -name=add_vif-badindex
    -class=auxnet
    -icon=wvnet-error
    -helpid=20112
} {
    UINT32 maxIndex
    UINT32 index
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ADDVIF_BADENTRY 20122 {
    -name=add_vif-badentry
    -class=auxnet
    -icon=wvnet-error
    -helpid=20113
} {
    UINT32 localAddr
    UINT32 index
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_ADDVIF_SEARCHFAIL 20123 {
    -name=add_vif-badaddr
    -class=auxnet
    -icon=wvnet-error
    -helpid=20114
} {
    UINT32 localAddr
    UINT32 index
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ADDVIF_BADFLAGS 20124 {
    -name=add_vif-badif
    -class=auxnet
    -icon=wvnet-error
    -helpid=20115
} {
    UINT32 pIf
    UINT32 index
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_DELVIF_BADINDEX 20125 {
    -name=del_vif-badindex
    -class=auxnet
    -icon=wvnet-error
    -helpid=20116
} {
    UINT32 maxIndex
    UINT32 index
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_DELVIF_BADENTRY 20126 {
    -name=del_vif-badentry
    -class=auxnet
    -icon=wvnet-error
    -helpid=20117
} {
    UINT32 index
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TUNNEL_STATUS 20127 {
    -name=tunnel_send-result
    -class=corenet
    -helpid=20118
} {
    UINT32 dstAddr
    UINT32 remoteAddr
    UINT32 localAddr
    UINT32 result
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_PHYINT_STATUS 20128 {
    -name=phyint_send-result
    -class=corenet
    -helpid=20119
} {
    UINT32 pIf
    UINT32 result
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPMRT_START 20129 {
    -name=ip_mrouter_cmd-start
    -class=auxnet
    -helpid=20120
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

# The following events are defined in ip_output.c

wvEvent WV_NETEVENT_IPOUT_BADADDR 20130 {
    -name=ip_output-badaddr
    -class=corenet
    -icon=ip_output-error
    -helpid=20121
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPOUT_BADFLAGS 20131 {
    -name=ip_output-badaddrtype
    -class=corenet
    -icon=ip_output-error
    -helpid=20122
} {
    UINT32 pIf
    UINT32 dstAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPOUT_CANTFRAG 20132 {
    -name=ip_output-badsize
    -class=corenet
    -icon=ip_output-error
    -helpid=20123
} {
    UINT32 pIf
    UINT32 dstAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPOUT_SHORTMSG 20133 {
    -name=ip_output-shortmsg
    -class=corenet
    -icon=ip_output-error
    -helpid=20124
} {
    UINT32 pIf
    UINT32 dstAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPOUTCTRL_BADCMD 20134 {
    -name=ip_ctloutput-badcmd
    -class=auxnet
    -icon=wvnet-error
    -helpid=20125
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPOUTCTRL_START 20135 {
    -name=ip_ctloutput-start
    -class=auxnet
    -icon=ipctl-start
    -helpid=20126
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPMLOOP_START 20136 {
    -name=ip_mloopback-start
    -class=auxnet
    -icon=ip_input-start
    -helpid=20127
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_IPOUT_CACHEMISS 20294 {
    -name=ip_output-cachemiss
    -class=corenet
    -helpid=20128
} {
    UINT32 routeAddr
    UINT32 dstAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_IPOUT_FINISH 20295 {
    -name=ip_output-result
    -class=corenet
    -helpid=20129
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 pIf
    UINT32 result
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPOUT_SENDALL 20296 {
    -name=ip_output-sendall
    -class=corenet
    -helpid=20130
} {
    UINT32 mtuLen
    UINT32 pktLen
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_IPOUT_SENDFRAG 20297 {
    -name=ip_output-sendfrag
    -class=corenet
    -helpid=20131
} {
    UINT32 mtuLen
    UINT32 hdrLen
    UINT32 pktLen
    UINT32 dstAddr
    UINT32 wvNetEventId
}

# The following events are defined in radix.c

wvEvent WV_NETEVENT_RNINIT_PANIC 20137 {
    -name=rn_init-panic-nomem
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20132
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNADD_BADMASK 20138 {
    -name=rn_addmask-dupmask
    -class=auxnet
    -icon=wvnet-error
    -helpid=20133
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNADD_NOMASK 20139 {
    -name=rn_new_radix_mask-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20134
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNADD_BADROUTE 20140 {
    -name=rn_addroute-badroute
    -class=auxnet
    -icon=wvnet-error
    -helpid=20135
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNDEL_BADREFCNT 20141 {
    -name=rn_delete-badrefcnt
    -class=auxnet
    -icon=wvnet-error
    -helpid=20136
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNDEL_BADMASK 20142 {
    -name=rn_delete-badmask
    -class=auxnet
    -icon=wvnet-error
    -helpid=20137
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNDEL_SEARCHFAIL 20143 {
    -name=rn_delete-noroute
    -class=auxnet
    -icon=wvnet-error
    -helpid=20138
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNDEL_KEYSEARCHFAIL 20144 {
    -name=rn_delete-noroutekey
    -class=auxnet
    -icon=wvnet-error
    -helpid=20139
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RNDEL_EXTRAMASK 20145 {
    -name=rn_delete-extramask
    -class=auxnet
    -icon=wvnet-error
    -helpid=20140
} {
    UINT32 wvNetEventId }

# The following events are defined in raw_cb.c
#
#   The first three events are currently unused.
#   The raw_attach() routine is only used when creating a routing
#   socket, which already tests for the memory allocation failure,
#   so the first event can never occur.
#
#   The second event is also meaningless since the soreserve() routine
#   failure which triggers it does not actually attempt to allocate
#   memory. It only enforces certain limits on the requested size.
#
#   The third event is redundant because the PRU_BIND case
#   statement in the raw_usrreq() routine is currently ignored.
#
# wvEvent WV_NETEVENT_RAWATTACH_NOPCBMEM 20146 {
#    -name=raw_attach-nopcbmem
#     -class=corenet
#     -icon=wvnet-nomem
# } {
#     UINT32 sockNum
#     UINT32 wvNetEventId
# }
#
# wvEvent WV_NETEVENT_RAWATTACH_NOSOCKBUFMEM 20147 {
#    -name=raw_attach-nosockbufmem
#     -class=corenet
#     -icon=wvnet-nomem
# } {
#     UINT32 sockNum
#     UINT32 wvNetEventId
# }
#
# wvEvent WV_NETEVENT_RAWBIND_NOIF 20148 {
#    -name=raw_bind-noif
#     -class=corenet
#     -icon=wvnet-error
# } {
#     UINT32 sockNum
#     UINT32 wvNetEventId
# }

wvEvent WV_NETEVENT_RAWATTACH_START 20149 {
    -name=raw_attach-start
    -class=corenet
    -icon=wvnet-attach
    -helpid=20141
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RAWDETACH_START 20150 {
    -name=raw_detach-start
    -class=corenet
    -icon=wvnet-detach
    -helpid=20142
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

#
# This raw_bind event defined in the raw_cb.c file is currently unused.
# The PRU_BIND option is currently not supported for routing sockets.
#
# wvEvent WV_NETEVENT_RAWBIND_START 20151 {
#    -name=raw_bind-start
#     -class=corenet
# } {
#     UINT32 sockNum
#     UINT32 wvNetEventId
# }

# The following events are defined in raw_ip.c

wvEvent WV_NETEVENT_RAWIPREQ_PANIC 20152 {
    -name=rip_usrreq-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20143
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWCTLOUT_BADLEVEL 20153 {
    -name=rip_ctloutput-badlevel
    -class=auxnet
    -icon=wvnet-error
    -helpid=20144
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWCTLOUT_BADMEM 20154 {
    -name=rip_ctloutput-badmem
    -class=auxnet
    -icon=wvnet-error
    -helpid=20145
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPREQ_FAIL 20155 {
    -name=rip_usrreq-fail
    -class=corenet
    -icon=wvnet-error
    -helpid=20146
} {
    UINT32 error
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPIN_NOSOCK 20156 {
    -name=rip_input-nosock
    -class=corenet
    -helpid=20147
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPIN_LOST 20157 {
    -name=rip_input-fullbuf
    -class=corenet
    -helpid=20148
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPIN_START 20158 {
    -name=rip_input-start
    -class=corenet
    -helpid=20149
} {
     UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RAWIPOUT_FINISH 20159 {
    -name=rip_output-result
    -class=corenet
    -helpid=20150
} {
    UINT32 dstAddr
    UINT32 srcAddr
    UINT32 result
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWCTLOUT_START 20160 {
    -name=rip_ctloutput-start
    -class=auxnet
    -icon=rawctl-start
    -helpid=20151
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPREQ_START 20161 {
    -name=rip_usrreq-start
    -class=corenet
    -icon=rawctl-start
    -helpid=20152
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIPINIT_START 20162 {
    -name=rip_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20153
} {
    UINT32 wvNetEventId }

# The following events are defined in raw_usrreq.c

wvEvent WV_NETEVENT_RAWREQ_PANIC 20163 {
    -name=raw_usrreq-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20154
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWREQ_FAIL 20164 {
    -name=raw_usrreq-fail
    -class=corenet
    -icon=wvnet-error
    -helpid=20155
} {
    UINT32 error
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIN_LOST 20165 {
    -name=raw_input-fullbuf
    -class=corenet
    -helpid=20156
} {
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWIN_START 20166 {
    -name=raw_input-start
    -class=corenet
    -helpid=20157
} {
    UINT32 protocol
    UINT32 family
    UINT32 wvNetEventId 
}

#
# The following event in raw_usrreq.c is currently unused. The
# protocol-specific control input routines are called when an interface
# shuts down or an ICMP redirect changes the routing table. Neither situation
# sends any messages to raw sockets in the PF_ROUTE domain, so there is no
# point in reporting this event. The current implementation of the
# raw_ctlinput() probably does nothing in response to those situations since
# routing protocols are capable of detecting the conditions independently. 
#
# wvEvent WV_NETEVENT_RAWCTLIN_START 20167 {
#     -name=raw_ctlinput-start
#     -class=auxnet
#     -icon=rtectl-start
# } {
#     UINT32 request
#     UINT32 wvNetEventId 
# }

wvEvent WV_NETEVENT_RAWREQ_START 20168 {
    -name=raw_usrreq-start
    -class=corenet
    -icon=rtectl-start
    -helpid=20158
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RAWINIT_START 20169 {
    -name=raw_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20159
} {
    UINT32 wvNetEventId }

# The following events are defined in route.c

wvEvent WV_NETEVENT_RTFREE_PANIC 20170 {
    -name=rtfree-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20160
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RTREQ_PANIC 20171 {
    -name=rtrequest-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20161
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RTSET_NOBUFS 20172 {
    -name=rt_setgate-nomem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20162
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RTFREE_BADREFCNT 20173 {
    -name=rtfree-badrefcnt
    -class=auxnet
    -icon=wvnet-error
    -helpid=20163
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RTALLOC_SEARCHFAIL 20174 {
    -name=rtalloc1-miss
    -class=auxnet
    -helpid=20164
} {
    UINT32 dstAddr
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_RTRESET_BADDEST 20175 {
    -name=rtredirect-error
    -class=auxnet
    -helpid=20165
} {
    UINT32 error
    UINT32 gateAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTREQ_BADROUTE 20176 {
    -name=rtrequest-error
    -class=auxnet
    -icon=wvnet-error
    -helpid=20166
} {
    UINT32 netmask
    UINT32 gateAddr
    UINT32 dstAddr
    UINT32 error
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTIOCTL_BADREQ 20177 {
    -name=rtioctl-badreq
    -class=auxnet
    -icon=wvnet-error
    -helpid=20167
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTALLOC_CLONE 20178 {
    -name=rtalloc1-clone
    -class=auxnet
    -helpid=20168
} {
    UINT32 dstAddr
    UINT32 baseAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTALLOC_SEARCHGOOD 20179 {
    -name=rtalloc1-match
    -class=auxnet
    -helpid=20169
} {
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTFREE_START 20180 {
    -name=rtfree-start
    -class=auxnet
    -helpid=20170
} {
    UINT32 dstAddr
    UINT32 refcnt
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTIOCTL_START 20181 {
    -name=rtioctl-start
    -class=auxnet
    -icon=rtectl-start
    -helpid=20171
} {
    UINT32 request
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ROUTEINIT_START 20182 {
    -name=route_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20172
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTRESET_STATUS 20183 {
    -name=rtredirect-result
    -class=auxnet
    -helpid=20173
} {
    UINT32 netmask
    UINT32 gateAddr
    UINT32 dstAddr
    UINT32 result
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTINIT_START 20184 {
    -name=rtinit-start
    -class=auxnet
    -helpid=20174
} {
    UINT32 dstAddr
    UINT32 command
    UINT32 wvNetEventId
}

# The following events are defined in rtsock.c

wvEvent WV_NETEVENT_RTOUT_BADMSG 20185 {
    -name=route_output-nobufs
    -class=auxnet
    -icon=route_output-error
    -helpid=20175
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTOUT_PANIC 20186 {
    -name=route_output-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20176
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_COPYDATA_PANIC 20187 {
    -name=m_copydata-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20177
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_RTOUTPUT_FAIL 20188 {
    -name=route_output-error
    -class=auxnet
    -helpid=20178
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_RTOUTPUT_START 20189 {
    -name=route_output-start
    -class=auxnet
    -helpid=20179
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_ROUTEREQ_START 20190 {
    -name=route_usrreq-start
    -class=auxnet
    -icon=rtectl-start
    -helpid=20180
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId
}

#
# The sysctl_rtable() routine defined in the rtsock.c file implements the
# (Unix) sysctl command (which is not provided with VxWorks) for the raw
# sockets routing protocol. Unlike all the other sysctl() support routines,
# this one is actually used. The RIP implementation calls it during startup
# as a legacy of the port. However, that single occurrence of a routine
# which is not available to the customer is too minor to generate this event.
#
# wvEvent WV_NETEVENT_ROUTEWALK_START 20191 {
#    -name=sysctl_rtable-start
#    -class=auxnet
#    -icon=rtectl-start
#} {
#     UINT32 ctrlType
#     UINT32 wvNetEventId
#}

wvEvent WV_NETEVENT_RTSOCKINIT_START 20192 {
    -name=rtSockInit-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20181
} {
    UINT32 wvNetEventId
}

# The following events are defined in sys_socket.c

wvEvent WV_NETEVENT_SOCKIOCTL_START 20193 {
    -name=soo_ioctl-start
    -class=auxnet
    -helpid=20182
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCKSELECT_START 20194 {
    -name=soo_select-start
    -class=auxnet
    -helpid=20183
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCKUNSELECT_START 20195 {
    -name=soo_unselect-start
    -class=auxnet
    -helpid=20184
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

# The following events are defined in tcp_input.c

wvEvent WV_NETEVENT_TCPOOB_PANIC 20196 {
    -name=tcp_pulloutofband-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20185
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_BADSUM 20197 {
    -name=tcp_input-badsum
    -class=corenet
    -icon=tcp_input-error
    -helpid=20186
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_BADSEG 20198 {
    -name=tcp_input-badhlen
    -class=corenet
    -icon=tcp_input-error
    -helpid=20187
} {
    UINT32 hdrLen
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_SEARCHFAIL 20199 {
    -name=tcp_input-nopcb
    -class=corenet
    -icon=tcp_input-error
    -helpid=20188
} {
    UINT32 dstPort
    UINT32 dstAddr
    UINT32 srcPort
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_BADSYNC 20200 {
    -name=tcp_input-badsync
    -class=corenet
    -icon=tcp_input-error
    -helpid=20189
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_SENDFAIL 20201 {
    -name=tcp_input-rexmt
    -class=corenet
    -helpid=20190
} {
    UINT32 numAcks
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_START 20202 {
    -name=tcp_input-start
    -class=corenet
    -helpid=20191
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_DUPSEG 20203 {
    -name=tcp_reass-segdrop
    -class=corenet
    -helpid=20192
} {
    UINT32 seqlen
    UINT32 seqno
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_PUREACK 20204 {
    -name=tcp_input-pureack
    -class=corenet
    -helpid=20193
} {
    UINT32 ackEnd
    UINT32 ackStart
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_PUREDATA 20205 {
    -name=tcp_input-puredata
    -class=corenet
    -helpid=20194
} {
    UINT32 datalen
    UINT32 seqno
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_ACCEPT 20206 {
    -name=tcp_input-accept
    -class=auxnet
    -helpid=20195
} {
    UINT32 recvseq
    UINT32 sendseq
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_CONNECT 20207 {
    -name=tcp_input-connect
    -class=auxnet
    -helpid=20196
} {
    UINT32 recvseq
    UINT32 sendseq
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_DATACLOSED 20208 {
    -name=tcp_input-dataclosed
    -class=corenet
    -helpid=20197
} {
    UINT32 datalen
    UINT32 seqno
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_RESET 20209 {
    -name=tcp_input-reset
    -class=corenet
    -helpid=20198
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_SENDCLOSED 20210 {
    -name=tcp_input-sendclosed
    -class=auxnet
    -helpid=20199
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_CLOSEWAIT 20211 {
    -name=tcp_input-closewait
    -class=auxnet
    -helpid=20200
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_RECVCLOSED 20212 {
    -name=tcp_input-recvclosed
    -class=auxnet
    -helpid=20201
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_CLOSING 20213 {
    -name=tcp_input-closing
    -class=auxnet
    -helpid=20202
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_CLOSE_RESTART 20214 {
    -name=tcp_input-close-restart
    -class=auxnet
    -icon=tcp_timeout
    -helpid=20203
} {
    UINT32 lostFlag
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_DUPDATA 20215 {
    -name=tcp_reass-dupdata
    -class=corenet
    -helpid=20204
} {
    UINT32 seqlen
    UINT32 seqno
    UINT32 startseq
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPIN_SHORTSEG 20216 {
    -name=tcp_input-shortseg
    -class=corenet
    -icon=tcp_input-error
    -helpid=20205
} {
    UINT32 wvNetEventId
}

# The following events are defined in tcp_output.c

wvEvent WV_NETEVENT_TCPOUT_HDRPANIC 20217 {
    -name=tcp_output-panic-hdrsize
    -class=corenet
    -icon=wvnet-panic
    -helpid=20206
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPOUT_PANIC 20218 {
    -name=tcp_output-panic-notemplate
    -class=corenet
    -icon=wvnet-panic
    -helpid=20207
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPOUT_FINISH 20219 {
    -name=tcp_output-result
    -class=corenet
    -helpid=20208
} {
    UINT32 result
    UINT32 sockNum
    UINT32 wvNetEventId
}

# The following events are defined in tcp_subr.c

wvEvent WV_NETEVENT_TCPINIT_HDRPANIC 20220 {
    -name=tcp_init-panic-hdrsize
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20209
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPRESPOND_START 20221 {
    -name=tcp_respond-start
    -class=corenet
    -helpid=20210
} {
    UINT32 flags
    UINT32 seqNum
    UINT32 ackNum
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPDROP_START 20222 {
    -name=tcp_drop-start
    -class=auxnet
    -helpid=20211
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPCLOSE_START 20223 {
    -name=tcp_close-start
    -class=auxnet
    -helpid=20212
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPNOTIFY_IGNORE 20224 {
    -name=tcp_notify-ignore
    -class=auxnet
    -icon=tcp_notify
    -helpid=20213
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPNOTIFY_KILL 20225 {
    -name=tcp_notify-harderror
    -class=auxnet
    -icon=tcp_notify
    -helpid=20214
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPNOTIFY_ERROR 20226 {
    -name=tcp_notify-softerror
    -class=auxnet
    -icon=tcp_notify
    -helpid=20215
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_TCPCTLIN_START 20227 {
    -name=tcp_ctlinput-start
    -class=auxnet
    -icon=tcpctl-start
    -helpid=20216
} {
    UINT32 command
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPINIT_START 20228 {
    -name=tcp_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20217
} {
    UINT32 wvNetEventId }

# The following events are defined in tcp_timer.c

wvEvent WV_NETEVENT_TCPFASTTIMER_START 20229 {
    -name=tcp_fasttimo-start
    -class=auxnet
    -icon=tcp_timeout
    -helpid=20218
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_TCPSLOWTIMER_START 20230 {
    -name=tcp_slowtimo-start
    -class=auxnet
    -icon=tcp_timeout
    -helpid=20219
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_TCPTIMER_START 20231 {
    -name=tcp_timers-start
    -class=auxnet
    -icon=tcp_timeout
    -helpid=20220
} {
    UINT32 timer
    UINT32 sockNum
    UINT32 wvNetEventId 
}

# The following events are defined in tcp_usrreq.c

wvEvent WV_NETEVENT_TCPREQ_PANIC 20232 {
    -name=tcp_usrreq-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20221
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPCTLOUT_NOPCB 20233 {
    -name=tcp_ctloutput-nopcb
    -class=auxnet
    -helpid=20222
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPATTACH_NOBUFS 20234 {
    -name=tcp_attach-nopcbmem
    -class=auxnet
    -icon=wvnet-nomem
    -helpid=20223
} {
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPREQ_BADMEM 20235 {
    -name=tcp_usrreq-badmem
    -class=auxnet
    -icon=wvnet-error
    -helpid=20224
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPREQ_NOPCB 20236 {
    -name=tcp_usrreq-nopcb
    -class=auxnet
    -helpid=20225
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPREQ_FAIL 20237 {
    -name=tcp_usrreq-fail
    -class=auxnet
    -icon=wvnet-error
    -helpid=20226
} {
    UINT32 error
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPCTLOUT_START 20238 {
    -name=tcp_ctloutput-start
    -class=auxnet
    -icon=tcpctl-start
    -helpid=20227
} {
    UINT32 optname
    UINT32 level
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPREQ_START 20239 {
    -name=tcp_usrreq-start
    -class=auxnet
    -icon=tcpctl-start
    -helpid=20228
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPATTACH_START 20240 {
    -name=tcp_attach-start
    -class=auxnet
    -icon=wvnet-attach
    -helpid=20229
} {
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_TCPDISCONN_START 20241 {
    -name=tcp_disconnect-start
    -class=auxnet
    -icon=wvnet-detach
    -helpid=20230
} {
    UINT32 sockNum
    UINT32 wvNetEventId 
}

# The following events are defined in udp_usrreq.c

wvEvent WV_NETEVENT_UDPREQ_PANIC 20242 {
    -name=udp_usrreq-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20231
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_UDPOUT_FAIL 20243 {
    -name=udp_output-error
    -class=corenet
    -helpid=20232
} {
    UINT32 result
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_UDPREQ_NOPCB 20244 {
    -name=udp_usrreq-nopcb
    -class=auxnet
    -helpid=20233
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_UDPIN_SHORTPKTHDR 20245 {
    -name=udp_input-shorthdr
    -class=corenet
    -icon=udp_input-error
    -helpid=20234
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_UDPIN_BADLEN 20246 {
    -name=udp_input-shortpkt
    -class=corenet
    -icon=udp_input-error
    -helpid=20235
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_UDPIN_BADSUM 20247 {
    -name=udp_input-badsum
    -class=corenet
    -icon=udp_input-error
    -helpid=20236
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_UDPIN_FULLSOCK 20248 {
    -name=udp_input-fullbuf
    -class=corenet
    -helpid=20237
} {
    UINT32 sockNum
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_UDPIN_SEARCHFAIL 20249 {
    -name=udp_input-nopcb
    -class=corenet
    -icon=udp_input-error
    -helpid=20238
} {
    UINT32 dstPort
    UINT32 dstAddr
    UINT32 srcPort
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPIN_NOPORT 20250 {
    -name=udp_input-noport
    -class=corenet
    -icon=udp_input-error
    -helpid=20239
} {
    UINT32 dstPort
    UINT32 dstAddr
    UINT32 srcPort
    UINT32 srcAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPIN_START 20251 {
    -name=udp_input-start
    -class=corenet
    -helpid=20240
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPOUT_FINISH 20252 {
    -name=udp_output-result
    -class=corenet
    -helpid=20241
} {
    UINT32 dstPort
    UINT32 srcPort
    UINT32 result
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPNOTIFY_START 20253 {
    -name=udp_notify-error
    -class=auxnet
    -helpid=20242
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPCTLIN_START 20254 {
    -name=udp_ctlinput-start
    -class=auxnet
    -icon=udpctl-start
    -helpid=20243
} {
    UINT32 command
    UINT32 wvNetEventId 
}

wvEvent WV_NETEVENT_UDPREQ_START 20255 {
    -name=udp_usrreq-start
    -class=auxnet
    -icon=udpctl-start
    -helpid=20244
} {
    UINT32 request
    UINT32 sockNum
    UINT32 wvNetEventId 
}

#
# This udp_sysctl event defined in the udp_usrreq.c file is currently
# unused. The udp_sysctl() routine implements the (Unix) sysctl command
# (which is not provided with VxWorks) for the UDP protocol.
#
# wvEvent WV_NETEVENT_UDPCTRL_START 20256 {
#    -name=udp_sysctl-start 
#    -class=auxnet 
#    -icon=udpctl-start
#} {
#    UINT32 ctrlType
#    UINT32 wvNetEventId
#}

wvEvent WV_NETEVENT_UDPINIT_START 20257 {
    -name=udp_init-start
    -class=auxnet
    -icon=wvnet-start
    -helpid=20245
} {
    UINT32 wvNetEventId }

wvEvent WV_NETEVENT_UDPIN_CACHEMISS 20258 {
    -name=udp_input-cachemiss
    -class=corenet
    -helpid=20246
} {
    UINT32 srcPort
    UINT32 srcAddr
    UINT32 dstPort
    UINT32 dstAddr
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_UDPIN_PCBGOOD 20293 {
    -name=udp_input-newdata
    -class=corenet
    -helpid=20247
} {
    UINT32 srcPort
    UINT32 srcAddr
    UINT32 dstPort
    UINT32 dstAddr
    UINT32 wvNetEventId
}

# The following events are defined in uipc_mbuf.c

wvEvent WV_NETEVENT_POOLINIT_PANIC 20259 {
    -name=_netStackPoolInit-panic-nomem
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20248
} {
    UINT32 memSize
    UINT32 pNetPool
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_MEMINIT_PANIC 20260 {
    -name=mbinit-panic-error
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20249
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_DEVMEM_PANIC 20261 {
    -name=m_devget-panic-badwidth
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20250
} {
    UINT32 memWidth
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_MEMCOPY_PANIC 20262 {
    -name=m_copym-panic-error
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20251
} {
    UINT32 pData
    UINT32 dataLen
    UINT32 dataOffset
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_MEMFREE_PANIC 20263 {
    -name=m_free-panic-error
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20252
} {
    UINT32 pMblk
    UINT32 wvNetEventId
}

# The following events are defined in uipc_sock.c

wvEvent WV_NETEVENT_SOFREE_PANIC 20264 {
    -name=sofree-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20253
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCLOSE_PANIC 20265 {
    -name=soclose-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20254
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOACCEPT_PANIC 20266 {
    -name=soaccept-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20255
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOSEND_FAIL 20267 {
    -name=sosend-error
    -class=corenet
    -helpid=20256
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SORECV_FAIL 20268 {
    -name=soreceive-error
    -class=corenet
    -helpid=20257
} {
    UINT32 error
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCREATE_SEARCHFAIL 20269 {
    -name=socreate-nosocktype
    -class=auxnet
    -icon=wvnet-error
    -helpid=20258
} {
    UINT32 type
    UINT32 protocol
    UINT32 domain
    UINT32 wvNetEventId
}

# The following event is currently unused. The switchboard search routines
# within socreate() will always return an entry which matches the specified
# socket type.
#
# wvEvent WV_NETEVENT_SOCREATE_BADTYPE 20270 {
#    -name=socreate-badtype 
#    -class=auxnet 
#    -icon=wvnet-error
#} {
#     UINT32 sockType
#     UINT32 protocolType
#     UINT32 wvNetEventId
# }

wvEvent WV_NETEVENT_SOCONNECT_BADSOCK 20271 {
    -name=soconnect-denied
    -class=auxnet
    -helpid=20259
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SODISCONN_STATUS 20272 {
    -name=sodisconnect-result
    -class=auxnet
    -helpid=20260
} {
    UINT32 result
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCLOSE_WAIT 20273 {
    -name=soclose-sleep
    -class=auxnet
    -helpid=20261
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCREATE_START 20274 {
    -name=socreate-start
    -class=auxnet
    -helpid=20262
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOBIND_START 20275 {
    -name=sobind-start
    -class=auxnet
    -helpid=20263
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOLISTEN_START 20276 {
    -name=solisten-start
    -class=auxnet
    -helpid=20264
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOFREE_START 20277 {
    -name=sofree-start
    -class=auxnet
    -helpid=20265
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCLOSE_START 20278 {
    -name=soclose-start
    -class=auxnet
    -helpid=20266
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOABORT_START 20279 {
    -name=soabort-start
    -class=auxnet
    -helpid=20267
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOACCEPT_START 20280 {
    -name=soaccept-start
    -class=auxnet
    -helpid=20268
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOCONNECT_START 20281 {
    -name=soconnect-start
    -class=auxnet
    -helpid=20269
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

#
# The following event is currently unused. The soconnect2() routine
# is apparently designed to establish a pair of connections between
# two hosts (possibly for half-duplex systems?). It is currently
# unavailable to users and is never called.
#
# wvEvent WV_NETEVENT_SOCONNECT2_START 20282 {
#    -name=soconnect2-start 
#    -class=auxnet
#} {
#    UINT32 dstSockNum
#    UINT32 srcSockNum
#    UINT32 wvNetEventId
#}

wvEvent WV_NETEVENT_SOSEND_FINISH 20283 {
    -name=sosend-finish
    -class=corenet
    -helpid=20270
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SORECV_FINISH 20284 {
    -name=soreceive-finish
    -class=corenet
    -helpid=20271
} {
    UINT32 sockNum
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SOSHUTDOWN_START 20285 {
    -name=soshutdown-start
    -class=auxnet
    -helpid=20272
} {
    UINT32 shutdownType
    UINT32 sockNum
    UINT32 wvNetEventId
}

# The following events are defined in uipc_sock.c

wvEvent WV_NETEVENT_SBAPPENDADDR_PANIC 20286 {
    -name=sbappendaddr-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20273
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SBAPPENDCTRL_PANIC 20287 {
    -name=sbappendcontrol-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20274
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SBFLUSH_PANIC 20288 {
    -name=sbflush-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20275
} {
    UINT32 sbLockFlag
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SBDROP_PANIC 20289 {
    -name=sbdrop-panic
    -class=auxnet
    -icon=wvnet-panic
    -helpid=20276
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SBWAIT_SLEEP 20290 {
    -name=sbwait-sleep
    -class=corenet
    -helpid=20277
} {
    UINT32 wvNetEventId
}

wvEvent WV_NETEVENT_SBWAKEUP_START 20291 {
    -name=sbwakeup-start
    -class=corenet
    -helpid=20278
} {
    UINT32 wakeupType
    UINT32 sockNum
    UINT32 wvNetEventId
}

# The following events are defined in unixLib.c

wvEvent WV_NETEVENT_HASHINIT_PANIC 20292 {
    -name=hashinit-panic
    -class=corenet
    -icon=wvnet-panic
    -helpid=20279
} {
    UINT32 hashType
    UINT32 wvNetEventId
}

#
# end of VxWorks networking events
#
