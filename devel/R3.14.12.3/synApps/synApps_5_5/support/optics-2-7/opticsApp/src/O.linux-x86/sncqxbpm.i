# 1 "../sncqxbpm.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../sncqxbpm.st"
program sncqxbpm ("name=qxbpm, P=iad:xbpm:, S=lax:serialOI:a4")

option +d;

# 1 "../seqPVmacros.h" 1
# 6 "../sncqxbpm.st" 2
# 165 "../sncqxbpm.st"
string s_ainp; assign s_ainp to "{S}.AINP" ; monitor s_ainp; evflag s_ainp_mon; sync s_ainp s_ainp_mon;
string s_aout; assign s_aout to "{S}.AOUT" ;
short s_baud; assign s_baud to "{S}.BAUD" ;
short s_dbit; assign s_dbit to "{S}.DBIT" ;
short s_fctl; assign s_fctl to "{S}.FCTL" ;
string s_idel; assign s_idel to "{S}.IEOS" ;
short s_ifmt; assign s_ifmt to "{S}.IFMT" ;
short s_nord; assign s_nord to "{S}.NORD" ; monitor s_nord;
short s_nrrd; assign s_nrrd to "{S}.NRRD" ;
string s_odel; assign s_odel to "{S}.OEOS" ;
short s_ofmt; assign s_ofmt to "{S}.OFMT" ;
short s_proc; assign s_proc to "{S}.PROC" ; monitor s_proc;
short s_prty; assign s_prty to "{S}.PRTY" ;
short s_sbit; assign s_sbit to "{S}.SBIT" ;
short s_scan; assign s_scan to "{S}.SCAN" ;
int s_sevr; assign s_sevr to "{S}.SEVR" ; monitor s_sevr;
short s_tmod; assign s_tmod to "{S}.TMOD" ;
double s_tmot; assign s_tmot to "{S}.TMOT" ; monitor s_tmot;







string port; assign port to "{P}port" ;
int bpm_address; assign bpm_address to "{P}address" ; monitor bpm_address;
short init; assign init to "{P}init" ; monitor init;
short enable; assign enable to "{P}enable" ; monitor enable;
int error; assign error to "{P}error" ;
string errMsg; assign errMsg to "{P}errMsg" ;
int debug_flag; assign debug_flag to "{P}debug" ; monitor debug_flag;
double pos_x; assign pos_x to "{P}pos:x" ;
double pos_y; assign pos_y to "{P}pos:y" ;
double current_total; assign current_total to "{P}current:total" ;
double current_a; assign current_a to "{P}current:a" ;
double current_b; assign current_b to "{P}current:b" ;
double current_c; assign current_c to "{P}current:c" ;
double current_d; assign current_d to "{P}current:d" ;
unsigned long raw_a; assign raw_a to "{P}current:a:raw" ;
unsigned long raw_b; assign raw_b to "{P}current:b:raw" ;
unsigned long raw_c; assign raw_c to "{P}current:c:raw" ;
unsigned long raw_d; assign raw_d to "{P}current:d:raw" ;
long current_low_raw; assign current_low_raw to "{P}current:low:raw" ; monitor current_low_raw;
short current_low; assign current_low to "{P}current:low" ;
short current_ok; assign current_ok to "{P}current:ok" ;
short signal_mode; assign signal_mode to "{P}mode" ; monitor signal_mode; evflag signal_mode_mon; sync signal_mode signal_mode_mon;
short buflen; assign buflen to "{P}buflen" ; monitor buflen; evflag buflen_mon; sync buflen buflen_mon;
short buflen_lo; assign buflen_lo to "{P}buflen.LOPR" ; monitor buflen_lo;
short buflen_hi; assign buflen_hi to "{P}buflen.HOPR" ; monitor buflen_hi;
int gain; assign gain to "{P}gain" ; monitor gain; evflag gain_mon; sync gain gain_mon;
double period_s; assign period_s to "{P}period" ; monitor period_s; evflag period_s_mon; sync period_s period_s_mon;
double period_s_LOPR; assign period_s_LOPR to "{P}period.LOPR" ; monitor period_s_LOPR;
double period_s_HOPR; assign period_s_HOPR to "{P}period.HOPR" ; monitor period_s_HOPR;

double gx; assign gx to "{P}GX" ; monitor gx;
double gy; assign gy to "{P}GY" ; monitor gy;

short set_offsets; assign set_offsets to "{P}set_offsets" ; monitor set_offsets;
short set_defaults; assign set_defaults to "{P}set_defaults" ; monitor set_defaults;
double settling; assign settling to "{P}settling" ; monitor settling;
# 235 "../sncqxbpm.st"
string gainStr [ 6 ]; assign gainStr to { "{P}gain.ZRST", "{P}gain.ONST", "{P}gain.TWST", "{P}gain.THST", "{P}gain.FRST", "{P}gain.FVST" } ; monitor gainStr;
string signal_modeStr [ 3 ]; assign signal_modeStr to { "{P}mode.ZRST", "{P}mode.ONST", "{P}mode.TWST" } ; monitor signal_modeStr;
# 257 "../sncqxbpm.st"
double gainTrim [ 24 ]; assign gainTrim to { "{P}r1:A1", "{P}r1:B1", "{P}r1:C1", "{P}r1:D1", "{P}r2:A1", "{P}r2:B1", "{P}r2:C1", "{P}r2:D1", "{P}r3:A1", "{P}r3:B1", "{P}r3:C1", "{P}r3:D1", "{P}r4:A1", "{P}r4:B1", "{P}r4:C1", "{P}r4:D1", "{P}r5:A1", "{P}r5:B1", "{P}r5:C1", "{P}r5:D1", "{P}r6:A1", "{P}r6:B1", "{P}r6:C1", "{P}r6:D1" } ; monitor gainTrim;
long offset [ 24 ]; assign offset to { "{P}r1:A2", "{P}r1:B2", "{P}r1:C2", "{P}r1:D2", "{P}r2:A2", "{P}r2:B2", "{P}r2:C2", "{P}r2:D2", "{P}r3:A2", "{P}r3:B2", "{P}r3:C2", "{P}r3:D2", "{P}r4:A2", "{P}r4:B2", "{P}r4:C2", "{P}r4:D2", "{P}r5:A2", "{P}r5:B2", "{P}r5:C2", "{P}r5:D2", "{P}r6:A2", "{P}r6:B2", "{P}r6:C2", "{P}r6:D2" } ; monitor offset;






char scratch[256];
int i;
string bpm_command;
string bpm_response;
string SNLtaskName;
int retry;
double updateDelay;
evflag started;
int oldGain;
int newGain;
int channel;
double factor;
long long_internal;
string debug_str;

%% #include <math.h>
%% #include <string.h>
%% #include <epicsThread.h>
# 293 "../sncqxbpm.st"
ss sncqxbpm {

  state startup {
    when () {
      efClear(started);

      pvGet (s_ainp);
      pvGet (s_aout);
      { init = ( 1 ); pvPut(init,SYNC); };
      { period_s_LOPR = ( 0.05 ); pvPut(period_s_LOPR,SYNC); };




      strcpy (SNLtaskName, macValueGet("name"));
      strcpy (port, macValueGet("S"));
      pvPut (port);
      { current_low = ( 1 ); pvPut(current_low,SYNC); };
      { current_ok = ( 0 ); pvPut(current_ok,SYNC); };
      { set_offsets = ( 0 ); pvPut(set_offsets,SYNC); };
      updateDelay = 3;
      efSet(started);
    } state idle
  }


  state init {
    when (!enable) {
      if (debug_flag >= 0) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 321, SNLtaskName, 0);; printf("%s\n", "software disabled"); epicsThreadSleep(0.01); };
    } state disable
    when (init) {



      { init = ( 0 ); pvPut(init,SYNC); };



      { s_baud = ( 7 ); pvPut(s_baud,SYNC); }; {epicsThreadSleep(2/60.);};
      { s_dbit = ( 4 ); pvPut(s_dbit,SYNC); }; {epicsThreadSleep(2/60.);};
      { s_prty = ( 1 ); pvPut(s_prty,SYNC); }; {epicsThreadSleep(2/60.);};
      { s_sbit = ( 1 ); pvPut(s_sbit,SYNC); }; {epicsThreadSleep(2/60.);};
      { s_fctl = ( 1 ); pvPut(s_fctl,SYNC); }; {epicsThreadSleep(2/60.);};
      { s_tmot = ( 0.250 ); pvPut(s_tmot,SYNC); };
      { s_ofmt = ( 0 ); pvPut(s_ofmt,SYNC); };
      { s_ifmt = ( 0 ); pvPut(s_ifmt,SYNC); };
      { strcpy(s_odel,"\n"); pvPut(s_odel,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(2/60.);};
      { strcpy(s_idel,"\n"); pvPut(s_idel,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(2/60.);};
      { s_nrrd = ( 0 ); pvPut(s_nrrd,SYNC); };
      { s_scan = ( 0 ); pvPut(s_scan,SYNC); };
      efClear (s_ainp_mon);
# 357 "../sncqxbpm.st"
      { sprintf(bpm_command, "*RST%d", bpm_address, 0, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 357, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 1) { s_tmod = ( 1 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (1 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 357, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };
      {epicsThreadSleep(6/60.);};
      efSet(gain_mon);
      efSet(signal_mode_mon);
      efSet(period_s_mon);






    } state idle
  }






  state disable {
    when (enable) {
      { init = ( 1 ); pvPut(init,SYNC); };
      if (debug_flag >= 0) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 379, SNLtaskName, 0);; printf("%s\n", "software re-enabled"); epicsThreadSleep(0.01); };
    } state idle
  }




  state comm_error {
    when (init || !s_sevr || delay((0.5 * 60.0))) {
      sprintf (debug_str, "comm_error: init=%d  s_sevr=%d", init, s_sevr);
      if (debug_flag >= 20) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 389, SNLtaskName, 20);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
      s_sevr = 0;
      { init = ( 1 ); pvPut(init,SYNC); };
    } state init
  }


  state idle {


    when (!enable) {} state disable

    when (s_sevr) {
      { error = ( 2 ); pvPut(error,SYNC); };
      sprintf (scratch, "communications error: %d", s_sevr);
      { strcpy(errMsg,scratch); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
    } state comm_error

    when (init) {
      if (debug_flag >= 20) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 408, SNLtaskName, 20);; printf("%s\n", "user re-initialized"); epicsThreadSleep(0.01); };
    } state init


    when (efTestAndClear(gain_mon)) {
      sprintf (debug_str, "gain changed to %s (%d)", gainStr[gain],gain);
      if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 414, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
      { sprintf(bpm_command, ":CONF%d:CURR:RANG %d", bpm_address, gain+1, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 415, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 1) { s_tmod = ( 1 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (1 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 415, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };




      updateDelay = 3;
      updateDelay = 0.01;
    } state idle


    when (efTestAndClear(signal_mode_mon) || efTestAndClear(buflen_mon)) {

      if (buflen < buflen_lo) { buflen = ( buflen_lo ); pvPut(buflen,SYNC); };
      if (buflen > buflen_hi) { buflen = ( buflen_hi ); pvPut(buflen,SYNC); };
      {epicsThreadSleep(2/60.);};
      efClear(buflen_mon);
      if (signal_mode == 0) {
        sprintf (debug_str, "gain changed to %s (%d)",
          signal_modeStr[signal_mode], signal_mode);
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 434, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
        { sprintf(bpm_command, ":CONF%d:SINGLE", bpm_address, 0, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 435, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 1) { s_tmod = ( 1 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (1 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 435, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };
      }
      if (signal_mode == 1) {
        sprintf (debug_str, "gain changed to %s (%d) with buffer length = %d",
          signal_modeStr[signal_mode], signal_mode, buflen);
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 440, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
        { sprintf(bpm_command, ":CONF%d:AVGCURR %d", bpm_address, buflen, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 441, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 1) { s_tmod = ( 1 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (1 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 441, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };
      }
      if (signal_mode == 2) {
        sprintf (debug_str, "gain changed to %s (%d) with buffer length = %d",
          signal_modeStr[signal_mode], signal_mode, buflen);
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 446, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
        { sprintf(bpm_command, ":CONF%d:WDWCURR %d", bpm_address, buflen, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 1, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 447, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 1) { s_tmod = ( 1 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (1 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 447, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };
      }
    } state idle

    when (efTestAndClear(period_s_mon)) {
      if (period_s < period_s_LOPR) { period_s = ( period_s_LOPR ); pvPut(period_s,SYNC); };
      if (period_s > period_s_HOPR) { period_s = ( period_s_HOPR ); pvPut(period_s,SYNC); };
      sprintf (debug_str, "checking update period: %g s", period_s);
      if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 455, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
    } state idle


    when (delay(period_s - 0.05 + updateDelay)) {
      { sprintf(bpm_command, ":READ%d:CURRALL?", bpm_address, 0, 0, 0); sprintf(debug_str, "BPM_COMM: <%s> (%d) <-- <%s>", s_aout, 0, bpm_command); if (debug_flag >= 15) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 460, SNLtaskName, 15);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; if (s_tmod != 0) { s_tmod = ( 0 ); pvPut(s_tmod,SYNC); }; {epicsThreadSleep(1/60.);}; if (0 == 1) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } } else { strcpy (bpm_response, ""); for (retry = 0; retry < 3; retry++) { if (strcmp(s_aout, bpm_command)) { { strcpy(s_aout,bpm_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; {epicsThreadSleep(1/60.);}; } else { { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; {epicsThreadSleep(1/60.);}; } for (i = 0; i < 0.25; i++) { {epicsThreadSleep(1/60.);}; if (efTest(s_ainp_mon)) break; } if (!s_sevr && efTest(s_ainp_mon)) { sprintf(debug_str, "BPM_COMM: <%s> ==> <%s>", s_aout,s_ainp); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 460, SNLtaskName, 10);; printf("%s\n", debug_str); epicsThreadSleep(0.01); }; strcpy (bpm_response, s_ainp+1); break; } } } };
      sprintf (debug_str, "serial.SEVR: %d", s_sevr);
      if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 462, SNLtaskName, 5);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
      if (!s_sevr) {
        sscanf(bpm_response, "%lu%lu%lu%lu", &raw_a, &raw_b, &raw_c, &raw_d);
 pvPut (raw_a);
 pvPut (raw_b);
 pvPut (raw_c);
 pvPut (raw_d);
        sprintf (debug_str, "bpm_command=<%s>  bpm_response=<%s>",
          bpm_command, bpm_response);
  if (debug_flag >= 12) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 471, SNLtaskName, 12);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
        sprintf (debug_str, "raw: a=%ld  b=%ld  c=%ld  d=%ld",
          raw_a, raw_b, raw_c, raw_d);
  if (debug_flag >= 12) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 474, SNLtaskName, 12);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
        { current_a = ( long_internal = raw_a - offset[((gain)*4 + (1) - 1)], gainTrim[((gain)*4 + (1) - 1)]*( long_internal ) ); pvPut(current_a,SYNC); };
        { current_b = ( long_internal = raw_b - offset[((gain)*4 + (2) - 1)], gainTrim[((gain)*4 + (2) - 1)]*( long_internal ) ); pvPut(current_b,SYNC); };
        { current_c = ( long_internal = raw_c - offset[((gain)*4 + (3) - 1)], gainTrim[((gain)*4 + (3) - 1)]*( long_internal ) ); pvPut(current_c,SYNC); };
        { current_d = ( long_internal = raw_d - offset[((gain)*4 + (4) - 1)], gainTrim[((gain)*4 + (4) - 1)]*( long_internal ) ); pvPut(current_d,SYNC); };
        { current_total = ( current_a+current_b+current_c+current_d ); pvPut(current_total,SYNC); };






        { pos_x = ( gx*(current_b - current_d)/(current_b + current_d) ); pvPut(pos_x,SYNC); };
        { pos_y = ( gy*(current_a - current_c)/(current_a + current_c) ); pvPut(pos_y,SYNC); };
        { current_low = ( (raw_a<current_low_raw) && (raw_b<current_low_raw) && (raw_c<current_low_raw) && (raw_d<current_low_raw) ); pvPut(current_low,SYNC); };






        { current_ok = ( (raw_a>=current_low_raw) && (raw_b>=current_low_raw) && (raw_c>=current_low_raw) && (raw_d>=current_low_raw) ); pvPut(current_ok,SYNC); };







      }
    } state idle

  }

}





ss sncqxbpm_tools {

  state startup {
    when (efTest(started)) {
    } state idle
  }

  state idle {

    when (set_offsets) {
      oldGain = gain;
      {epicsThreadSleep(60/60.);};
      for (newGain = 0; newGain < 6; newGain++) {
  if (!set_offsets) break;
 { gain = ( newGain ); pvPut(gain,SYNC); };
  if (!set_offsets) break;
 epicsThreadSleep(settling);
  if (!set_offsets) break;
  sprintf (debug_str, "new gain=%d", gain);
  if (debug_flag >= 18) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 533, SNLtaskName, 18);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
 { offset[((gain)*4 + (1) - 1)] = ( raw_a ); pvPut(offset[((gain)*4 + (1) - 1)],SYNC); };
 { offset[((gain)*4 + (2) - 1)] = ( raw_b ); pvPut(offset[((gain)*4 + (2) - 1)],SYNC); };
 { offset[((gain)*4 + (3) - 1)] = ( raw_c ); pvPut(offset[((gain)*4 + (3) - 1)],SYNC); };
 { offset[((gain)*4 + (4) - 1)] = ( raw_d ); pvPut(offset[((gain)*4 + (4) - 1)],SYNC); };
 for (channel=1; channel <= 4; channel++) {
    sprintf (debug_str,
      "  offset[%d]=%ld",
      channel,offset[((gain)*4 + (channel) - 1)]);
    if (debug_flag >= 19) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 542, SNLtaskName, 19);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
 }
      }
      { gain = ( oldGain ); pvPut(gain,SYNC); };
      { set_offsets = ( 0 ); pvPut(set_offsets,SYNC); };
      {epicsThreadSleep(2/60.);};
      sprintf (debug_str, "restored to previous gain #%d", gain);
      if (debug_flag >= 18) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 549, SNLtaskName, 18);; printf("%s\n", debug_str); epicsThreadSleep(0.01); };
    } state idle

    when (set_defaults) {
      if (debug_flag >= 18) { printf("<%s,%d,%s,%d> ", "../sncqxbpm.st", 553, SNLtaskName, 18);; printf("%s\n", "restore default calibrations"); epicsThreadSleep(0.01); };
      { signal_mode = ( 2 ); pvPut(signal_mode,SYNC); };
      { buflen = ( 30 ); pvPut(buflen,SYNC); };
      { current_low_raw = ( 1000 ); pvPut(current_low_raw,SYNC); };
      { period_s = ( 0.1 ); pvPut(period_s,SYNC); };
      { gx = ( 4.5 ); pvPut(gx,SYNC); };
      { gy = ( 4.5 ); pvPut(gy,SYNC); };
      { settling = ( 2.5 ); pvPut(settling,SYNC); };
      for (newGain = 0; newGain < 6; newGain++) {
 if (newGain == 0) {factor = 350e-9;}
 if (newGain == 1) {factor = 700e-9;}
 if (newGain == 2) {factor = 1400e-9;}
 if (newGain == 3) {factor = 7e-6;}
 if (newGain == 4) {factor = 70e-6;}
 if (newGain == 5) {factor = 700e-6;}
 factor /= 10;
 factor /= 1e5;
 for (channel=1; channel <= 4; channel++) {
   { gainTrim[((newGain)*4 + (channel) - 1)] = ( factor ); pvPut(gainTrim[((newGain)*4 + (channel) - 1)],SYNC); };
 }
      }
      { set_defaults = ( 0 ); pvPut(set_defaults,SYNC); };
      {epicsThreadSleep(2/60.);};
    } state idle

  }

}
