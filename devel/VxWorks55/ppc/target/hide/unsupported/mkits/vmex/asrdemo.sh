#!/bin/sh

usage="usage: asrdemo.sh <tgtsvr>"

if [ $# -ne 1 ]
then
   echo "$usage"
   exit
fi

wtxtcl << PIGGY
dialog -resource asrdemo -geometry 500x320 $1
PIGGY
