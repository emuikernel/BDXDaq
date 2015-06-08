# 1 "../sncEntryVar.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../sncEntryVar.st"
program sncentryvar





float v;
assign v to "CD01:FD:PERIOD";
monitor v;

ss ss1
{
 state low
 {
     entry
     {
  v = 20.0;
                pvPut(v);
  printf("Init v = %g on first entry to low\n",v);
     }
     when(v<11.0)
     {

  printf("low, v = %g, now changing to high\n",v);
     } state high
     when(v>80)
     {
              v = 20;
              pvPut(v);
     } state low
     when(delay(2.0))
     {
  printf("low, delay timeout, incr v and now reenter low\n");
  v = v+1.0;
                pvPut(v);
  printf("v = %g\n",v);
     } state low
     exit
     {
  printf("low, print this on exit of low\n");
     }

 }

 state high
 {
     entry { printf("\nEntered high\n"); }
     when(v>=20.0)
     {
  printf("high, v = %g changing to low\n", v);
     } state low
     when(delay(3.0))
     {
  v = 20;
                pvPut(v);
  printf("high, delay timeout, resetting v = %g\n",v);
     } state high
 }
}
