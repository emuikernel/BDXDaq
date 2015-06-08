#!/bin/sh
  n=0

while [ 1 ]
do
    let n=$n+1
    echo $n
    caput "TAMA1:SOL:1" $n
    sleep 1
done
