# 1 "../sncEntryOpte.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../sncEntryOpte.st"
program sncentryopte




%%#include "epicsThread.h"

float v;

ss ss1
{
 state low
 {
     option -e;
     entry
     {
  printf("Pause on each entry to low, including 'iterations'\n");
  epicsThreadSleep(1.0);
     }
     when(v>5.0)
     {
  printf("low, v = %g, now changing to high\n",v);
     } state high
     when(delay(2.0))
     {
  printf("low, delay timeout, incr v and now reenter low\n");
  v = v+1.0;
  printf("v = %g\n",v);
     } state low
     exit
     {
  printf("low, print this on exit of low\n");
     }

 }

 state high
 {
     entry { printf("\nentered high\n"); }
     when(v==0.0)
     {
  printf("changing to low\n");
     } state low
     when(delay(3.0))
     {
  printf("high, delay 3.0 timeout, resetting v\n");
  v = 0;
     } state high
 }
}
