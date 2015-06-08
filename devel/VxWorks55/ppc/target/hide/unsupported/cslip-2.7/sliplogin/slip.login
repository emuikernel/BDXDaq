#!/bin/sh -x
# @(#) $Header: slip.login,v 1.16 93/02/11 20:30:54 torek Exp $ (LBL)
#
# generic login file for a slip line.  sliplogin invokes this with
# the parameters:
#      1        2         3        4          5         6     7-n
#   slipunit ttyspeed loginname local-addr remote-addr mask opt-args

prog=$0
PATH=/etc:/usr/etc:/sbin:/usr/sbin:/bin:$PATH ; export PATH
U=$1
eval L=$4
R=$5
#FC=mdmbuf		# SunOS 3.5 hack
FC=crtscts
stty $FC > /dev/tty
ifconfig sl$U $L $R netmask $6 || exit 1
route set $R $L mtu 552 pipesize 2048 rtt 5 || exit 2
n=$#
while [ $n -gt 6 ]; do
	n=`expr $n - 1`
	case $7 in
	normal)		ifconfig sl$U -link0 -link1 ;;
	compress)	ifconfig sl$U link0 ;;
	auto-comp)	ifconfig sl$U link1 ;;
	no-icmp)	ifconfig sl$U link2 ;;
	proxy-arp)	E=`myetheraddr $R` || exit 3
			arp -s $R $E pub || exit 4 ;;
	proxy-arp-addr)	if [ $n -le 6 ]; then
				echo $prog":" missing argument to proxy-arp-addr
				exit 5
			fi
			n=`expr $n - 1`
			shift
			E=`myetheraddr $7` || exit 6
			route -n add $7 $R 1 || exit 7
			route -n delete $7 $7 # delete possible dynamic route
			arp -s $7 $E pub || exit 8 ;;
	netroute)	if [ $n -le 6 ]; then
				echo $prog":" missing argument to netroute
				exit 9
			fi
			n=`expr $n - 1`
			shift
			route -n delete net $7 $R
			route -n add net $7 $R 1 || exit 10
			route set net $7 $R mtu 552 pipesize 2048 rtt 5 ;;
	default)	route -n delete default $R
			route -n add default $R 1 || exit 11
			route set default $R mtu 552 pipesize 2048 rtt 5 ;;
	hwfc)		stty $FC > /dev/tty ;;
	no-hwfc)	stty -$FC > /dev/tty ;;
	*)		echo $prog":" $7 unknown option
			exit 12 ;;
	esac
	shift
done
exit 0
