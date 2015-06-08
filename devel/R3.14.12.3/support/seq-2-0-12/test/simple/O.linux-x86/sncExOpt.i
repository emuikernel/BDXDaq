# 1 "../sncExOpt.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../sncExOpt.st"
program snctest
float v;
assign v to "{user}:xxxExample";
monitor v;

ss ss1
{
 state low
 {
            option +t;
     entry
     {
  printf("Will do this on entry");
  printf("Another thing to do on entry");
     }
     when(v>5.0)
     {
  printf("now changing to high\n");
     } state high
     when(delay(.1))
     {

     } state low
 }
 state high
 {
            option -t;
     when(v<=5.0)
     {
  printf("changing to low\n");
     } state low
     when(delay(.1))
     {



     } state high
 }
}
