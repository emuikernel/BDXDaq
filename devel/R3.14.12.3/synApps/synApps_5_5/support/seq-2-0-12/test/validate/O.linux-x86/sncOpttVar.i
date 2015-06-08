# 1 "../sncOpttVar.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../sncOpttVar.st"
program sncopttvar

option +r;
# 14 "../sncOpttVar.st"
float d;
float v;

ss ss1
{
 state low
 {
       option -t;
     entry
     {
  d = 4.0;
  printf("Entered low, initialize delay to %g sec.\n",d);
     }
     when(d>10.0)
     {
  printf("low, delay = %g, now changing to high\n",d);
     } state high
     when(delay(d))
     {
  printf("low, delay timeout, reenter low\n");
  d += 2.0;
  printf("low, waiting %g secs to iterate\n",d);
     } state low
     exit
     {
  printf("low, print this on exit of low\n");
     }

 }

 state high
 {
     entry {
                 printf("\nEntered high\n");
                 v = 3.0;
            }
     when(v==0)
     {
  printf("changing to low\n");
     } state low
     when(delay(3.0))
     {
  printf("high, delay 3.0 timeout, decr v and re-enter high\n");
  v = v-1.0;
  printf("v = %g\n",v);
     } state high
 }
}
