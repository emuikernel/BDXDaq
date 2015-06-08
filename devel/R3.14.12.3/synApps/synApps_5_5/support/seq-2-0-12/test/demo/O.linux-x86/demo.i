# 1 "../demo.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../demo.st"
# 33 "../demo.st"
program demo( "pvsys=ca" ",debug=1" )


option +r;


long light;
assign light to "demo:light";

double lightOn;
assign lightOn to "demo:lightOn";
monitor lightOn;

double lightOff;
assign lightOff to "demo:lightOff";
monitor lightOff;


double voltage;
assign voltage to "demo:voltage";
monitor voltage;

double loLimit;
assign loLimit to "demo:loLimit";
monitor loLimit;
evflag loFlag;
sync loLimit to loFlag;

double hiLimit;
assign hiLimit to "demo:hiLimit";
monitor hiLimit;
evflag hiFlag;
sync hiLimit to hiFlag;
# 88 "../demo.st"
entry {




}


ss light {
    state START {
 when () {
     light = 0;
     pvPut( light );
 } state LIGHT_OFF
    }

    state LIGHT_OFF {
 when ( voltage > lightOn ) {
     light = 1;
     pvPut( light );
     seqLog( "light_off -> light_on\n" );
 } state LIGHT_ON
    }

    state LIGHT_ON {
 when ( voltage < lightOff ) {
     light = 0;
     pvPut( light );
     seqLog( "light_on -> light_off\n" );
 } state LIGHT_OFF
    }
}


ss ramp {
    state START {
 when () {
     voltage = loLimit;
     pvPut( voltage );
     seqLog("start -> ramp_up\n");
 } state RAMP_UP
    }

    state RAMP_UP {
 when ( voltage > hiLimit ) {
     seqLog("ramp_up -> ramp_down\n");
 } state RAMP_DOWN

 when ( delay( 0.1 ) ) {
     voltage += 0.1;
     if ( pvPut( voltage ) < 0 )
  seqLog( "pvPut( voltage ) failed\n" );

 } state RAMP_UP
    }

    state RAMP_DOWN {
 when ( voltage < loLimit ) {
     seqLog("ramp_down -> ramp_up\n");
 } state RAMP_UP

 when ( delay( 0.1 ) ) {
     voltage -= 0.1;
     if ( pvPut( voltage ) < 0 )
  seqLog( "pvPut( voltage ) failed\n" );

 } state RAMP_DOWN
    }
}


ss limit {
    state START {
 when ( efTestAndClear( loFlag ) && loLimit > hiLimit ) {
     hiLimit = loLimit;
     pvPut( hiLimit );
 } state START

 when ( efTestAndClear( hiFlag ) && hiLimit < loLimit ) {
     loLimit = hiLimit;
     pvPut( loLimit );
 } state START
    }
}
