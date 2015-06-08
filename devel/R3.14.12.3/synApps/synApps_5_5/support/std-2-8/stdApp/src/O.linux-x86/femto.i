# 1 "../femto.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../femto.st"
# 15 "../femto.st"
program femto("name=femto,P=,H=,F=,G1=,G2=,G3=,NO=")

option +d;
option -c;
option +r;
option -w;

%%#include <stdio.h>
%%#include <math.h>
# 1 "../seqPVmacros.h" 1
# 25 "../femto.st" 2





int t0; assign t0 to "{G1}" ; monitor t0; evflag t0_mon; sync t0 t0_mon;
int t1; assign t1 to "{G2}" ; monitor t1; evflag t1_mon; sync t1 t1_mon;
int t2; assign t2 to "{G3}" ; monitor t2; evflag t2_mon; sync t2 t2_mon;
int tx; assign tx to "{NO}" ; monitor tx; evflag tx_mon; sync tx tx_mon;
double gain; assign gain to "{P}{H}{F}gain" ;
int gainidx; assign gainidx to "{P}{H}{F}gainidx" ; monitor gainidx; evflag gainidx_mon; sync gainidx gainidx_mon;
int debug_flag; assign debug_flag to "{P}{H}{F}debug" ; monitor debug_flag; evflag debug_flag_mon; sync debug_flag debug_flag_mon;

int curgain;
char msg[256];
char* SNLtaskName;

%%static int powers[] = {5,6,7,8,9,10,11,0,3,4,5,6,7,8,9,0};

ss femto
{

    state init
    {
        when()
        {
            SNLtaskName = macValueGet("name");

            pvGet(t0,SYNC);
            pvGet(t1,SYNC);
            pvGet(t2,SYNC);
            pvGet(tx,SYNC);

            if( (tx+t2+t1+t0) == 0 )
            {
                gainidx = 8;
                if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 61, SNLtaskName, 2);; printf("%s\n", "gainidx = default (1e3 since all bits were OFF)"); epicsThreadSleep(0.01); };
            }
            else
            {
               gainidx = (tx << 3) | (t2 << 2) | (t1 << 1) | t0;
               sprintf(msg,"gainidx = %d",gainidx);
               if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 67, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };
            }

            if( gainidx >= 15 || gainidx == 7 )
            {
                gainidx = 6;
                if (debug_flag >= 4) { printf("<%s,%d,%s,%d> ", "../femto.st", 73, SNLtaskName, 4);; printf("%s\n", "resetting gain to 1e11"); epicsThreadSleep(0.01); };
            }

            curgain = -1;
            { gainidx = ( gainidx ); pvPut(gainidx,SYNC); };
            { gain = ( pow(10,powers[gainidx]) ); pvPut(gain,SYNC); };
            sprintf(msg,"gainidx = %d",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 80, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };

            efClear(t0_mon);
            efClear(t1_mon);
            efClear(t2_mon);
            efClear(tx_mon);
            efClear(gainidx_mon);
            efClear(debug_flag_mon);

            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 89, SNLtaskName, 2);; printf("%s\n", "init complete"); epicsThreadSleep(0.01); };
        } state changeGain

    }

    state updateGain
    {
        when()
        {
            gainidx = (tx << 3) | (t2 << 2) | (t1 << 1) | t0;
            sprintf(msg,"gainidx = %d",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 100, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };

            if( gainidx >= 15 || gainidx == 7 )
            {
                gainidx = 6;
                if (debug_flag >= 4) { printf("<%s,%d,%s,%d> ", "../femto.st", 105, SNLtaskName, 4);; printf("%s\n", "resetting gain to 1e11"); epicsThreadSleep(0.01); };
            }

            curgain = gainidx;
            { gainidx = ( gainidx ); pvPut(gainidx,SYNC); };
            { gain = ( pow(10,powers[gainidx]) ); pvPut(gain,SYNC); };
            sprintf(msg,"gainidx = %d",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 112, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };

        } state idle

    }

    state changeGain
    {
        when( curgain == gainidx )
        {
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 122, SNLtaskName, 2);; printf("%s\n", "No gain change required"); epicsThreadSleep(0.01); };
        } state idle

        when( gainidx < 0 )
        {
            { gainidx = ( curgain ); pvPut(gainidx,SYNC); };
            { gain = ( pow(10,powers[curgain]) ); pvPut(gain,SYNC); };
            sprintf(msg,"Requested gain (%d) too low!",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 130, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };
        } state idle

        when( gainidx > 15 )
        {
            { gainidx = ( curgain ); pvPut(gainidx,SYNC); };
            { gain = ( pow(10,powers[curgain]) ); pvPut(gain,SYNC); };
            sprintf(msg,"Requested gain (%d) too high!",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 138, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };
        } state idle

        when( gainidx == 7 || gainidx == 15 )
        {
            { gainidx = ( curgain ); pvPut(gainidx,SYNC); };
            { gain = ( pow(10,powers[curgain]) ); pvPut(gain,SYNC); };
            sprintf(msg,"Requested gain (%d) invalid!",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 146, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };
        } state idle

        when()
        {
            sprintf(msg,"gain %d selected",gainidx);
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 152, SNLtaskName, 2);; printf("%s\n", msg); epicsThreadSleep(0.01); };


            if( gainidx == 0 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 1 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 2 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 3 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 4 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 5 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 6 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 0 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 8 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 9 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 10 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 11 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 0 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 12 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 13 )
            {
                { t0 = ( 1 ); pvPut(t0,SYNC); };
                { t1 = ( 0 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }


            if( gainidx == 14 )
            {
                { t0 = ( 0 ); pvPut(t0,SYNC); };
                { t1 = ( 1 ); pvPut(t1,SYNC); };
                { t2 = ( 1 ); pvPut(t2,SYNC); };
                { tx = ( 1 ); pvPut(tx,SYNC); };
            }

            curgain = gainidx;
            { gain = ( pow(10,powers[gainidx]) ); pvPut(gain,SYNC); };

            efClear(t0_mon);
            efClear(t1_mon);
            efClear(t2_mon);
            efClear(tx_mon);
            efClear(gainidx_mon);

        } state idle

    }

    state idle
    {
        when( efTestAndClear(debug_flag_mon) )
        {
            sprintf(msg,"Debug level changed to %d",debug_flag);
            if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../femto.st", 298, SNLtaskName, 1);; printf("%s\n", msg); epicsThreadSleep(0.01); };
        } state idle

        when( efTestAndClear(tx_mon) || efTestAndClear(t2_mon) ||
              efTestAndClear(t1_mon) || efTestAndClear(t0_mon)
            )
        {
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 305, SNLtaskName, 2);; printf("%s\n", "bits changed, updating"); epicsThreadSleep(0.01); };
        } state updateGain

        when( efTestAndClear(gainidx_mon) )
        {
            if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../femto.st", 310, SNLtaskName, 2);; printf("%s\n", "gain change"); epicsThreadSleep(0.01); };
        } state changeGain

        when( delay(.1) ) {} state idle
    }

}
