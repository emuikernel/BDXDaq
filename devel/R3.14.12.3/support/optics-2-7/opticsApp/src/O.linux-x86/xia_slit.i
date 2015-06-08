# 1 "../xia_slit.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../xia_slit.st"
# 79 "../xia_slit.st"
program xia_slit("name=hsc1,P=pfx:,HSC=hsc1:,S=pfx:serial9")


option +d;
option -c;
option +r;
option -w;


%%#include <stdio.h>
%%#include <math.h>
%%#include <string.h>
%% #include <epicsThread.h>
# 1 "../seqPVmacros.h" 1
# 93 "../xia_slit.st" 2
# 180 "../xia_slit.st"
string s_ainp; assign s_ainp to "{S}.AINP" ; monitor s_ainp; evflag s_ainp_mon; sync s_ainp s_ainp_mon;
string s_tinp; assign s_tinp to "{S}.TINP" ; monitor s_tinp;
string s_aout; assign s_aout to "{S}.AOUT" ;
string s_ieos; assign s_ieos to "{S}.IEOS" ;
string s_oeos; assign s_oeos to "{S}.OEOS" ;

double s_tmot; assign s_tmot to "{S}.TMOT" ; monitor s_tmot;

short s_ofmt; assign s_ofmt to "{S}.OFMT" ;
short s_ifmt; assign s_ifmt to "{S}.IFMT" ;
short s_port; assign s_port to "{S}.PORT" ;
short s_tmod; assign s_tmod to "{S}.TMOD" ;
short s_scan; assign s_scan to "{S}.SCAN" ;
short s_nord; assign s_nord to "{S}.NORD" ; monitor s_nord;
short s_nrrd; assign s_nrrd to "{S}.NRRD" ;

short s_proc; assign s_proc to "{S}.PROC" ;

int s_sevr; assign s_sevr to "{S}.SEVR" ; monitor s_sevr;
int s_optioniv; assign s_optioniv to "{S}.OPTIONIV" ;

short s_baud; assign s_baud to "{S}.BAUD" ;
short s_dbit; assign s_dbit to "{S}.DBIT" ;
short s_sbit; assign s_sbit to "{S}.SBIT" ;
short s_prty; assign s_prty to "{S}.PRTY" ;
short s_mctl; assign s_mctl to "{S}.MCTL" ;
short s_fctl; assign s_fctl to "{S}.FCTL" ;

int debug_flag; assign debug_flag to "{P}{HSC}debug" ; monitor debug_flag;





string port; assign port to "{P}{HSC}port" ;
string hID; assign hID to "{P}{HSC}hID" ; monitor hID; evflag hID_mon; sync hID hID_mon;
string vID; assign vID to "{P}{HSC}vID" ; monitor vID; evflag vID_mon; sync vID vID_mon;
short hOrient; assign hOrient to "{P}{HSC}hOrient" ; monitor hOrient; evflag hOrient_mon; sync hOrient hOrient_mon;
short vOrient; assign vOrient to "{P}{HSC}vOrient" ; monitor vOrient; evflag vOrient_mon; sync vOrient vOrient_mon;

short hBusy; assign hBusy to "{P}{HSC}hBusy" ;
short h_Busy; assign h_Busy to "{P}{HSC}h_Busy" ; monitor h_Busy;
short h_BusyOff; assign h_BusyOff to "{P}{HSC}h_BusyOff.PROC" ;
short h_BusyDisable; assign h_BusyDisable to "{P}{HSC}h_BusyDisable" ;

short vBusy; assign vBusy to "{P}{HSC}vBusy" ;
short v_Busy; assign v_Busy to "{P}{HSC}v_Busy" ; monitor v_Busy;
short v_BusyOff; assign v_BusyOff to "{P}{HSC}v_BusyOff.PROC" ;
short v_BusyDisable; assign v_BusyDisable to "{P}{HSC}v_BusyDisable" ;

short stop; assign stop to "{P}{HSC}stop" ; monitor stop; evflag stop_mon; sync stop stop_mon;
short locate; assign locate to "{P}{HSC}locate" ; monitor locate;
short init; assign init to "{P}{HSC}init" ; monitor init;
short enable; assign enable to "{P}{HSC}enable" ; monitor enable;
short calibrate; assign calibrate to "{P}{HSC}calib" ; monitor calibrate;


int hOuterLimit; assign hOuterLimit to "{P}{HSC}hOuterLimitLI" ; monitor hOuterLimit;
int vOuterLimit; assign vOuterLimit to "{P}{HSC}vOuterLimitLI" ; monitor vOuterLimit;


int hOrigin; assign hOrigin to "{P}{HSC}hOriginLI" ; monitor hOrigin;
int vOrigin; assign vOrigin to "{P}{HSC}vOriginLI" ; monitor vOrigin;


int hStepDelay; assign hStepDelay to "{P}{HSC}hStepDelayLI" ; monitor hStepDelay;
int vStepDelay; assign vStepDelay to "{P}{HSC}vStepDelayLI" ; monitor vStepDelay;


int hGearBacklash; assign hGearBacklash to "{P}{HSC}hGearBacklashLI" ; monitor hGearBacklash;
int vGearBacklash; assign vGearBacklash to "{P}{HSC}vGearBacklashLI" ; monitor vGearBacklash;


int hStPwr; assign hStPwr to "{P}{HSC}hStatPwrMI" ; monitor hStPwr;
int hStLmts; assign hStLmts to "{P}{HSC}hStatLmtsBI" ; monitor hStLmts;
int hStBan; assign hStBan to "{P}{HSC}hStatBanBI" ; monitor hStBan;
int hStEcho; assign hStEcho to "{P}{HSC}hStatEchoBI" ; monitor hStEcho;
int hStLock; assign hStLock to "{P}{HSC}hStatLockBI" ; monitor hStLock;
int hStAlias; assign hStAlias to "{P}{HSC}hStatAliasBI" ; monitor hStAlias;
int hStText; assign hStText to "{P}{HSC}hStatTextBI" ; monitor hStText;

int vStPwr; assign vStPwr to "{P}{HSC}vStatPwrMI" ; monitor vStPwr;
int vStLmts; assign vStLmts to "{P}{HSC}vStatLmtsBI" ; monitor vStLmts;
int vStBan; assign vStBan to "{P}{HSC}vStatBanBI" ; monitor vStBan;
int vStEcho; assign vStEcho to "{P}{HSC}vStatEchoBI" ; monitor vStEcho;
int vStLock; assign vStLock to "{P}{HSC}vStatLockBI" ; monitor vStLock;
int vStAlias; assign vStAlias to "{P}{HSC}vStatAliasBI" ; monitor vStAlias;
int vStText; assign vStText to "{P}{HSC}vStatTextBI" ; monitor vStText;
# 280 "../xia_slit.st"
double t; assign t to "{P}{HSC}t" ; monitor t;
double t_event; assign t_event to "{P}{HSC}t_event" ; monitor t_event; evflag t_event_mon; sync t_event t_event_mon;
double t_hi; assign t_hi to "{P}{HSC}t.HOPR" ; monitor t_hi;
double t_lo; assign t_lo to "{P}{HSC}t.LOPR" ; monitor t_lo;
double t_RB; assign t_RB to "{P}{HSC}tRB" ; monitor t_RB;
double load_t; assign load_t to "{P}{HSC}load_t.B" ;

double l; assign l to "{P}{HSC}l" ; monitor l;
double l_event; assign l_event to "{P}{HSC}l_event" ; monitor l_event; evflag l_event_mon; sync l_event l_event_mon;
double l_hi; assign l_hi to "{P}{HSC}l.HOPR" ; monitor l_hi;
double l_lo; assign l_lo to "{P}{HSC}l.LOPR" ; monitor l_lo;
double l_RB; assign l_RB to "{P}{HSC}lRB" ; monitor l_RB;
double load_l; assign load_l to "{P}{HSC}load_l.B" ;

double b; assign b to "{P}{HSC}b" ; monitor b;
double b_event; assign b_event to "{P}{HSC}b_event" ; monitor b_event; evflag b_event_mon; sync b_event b_event_mon;
double b_hi; assign b_hi to "{P}{HSC}b.HOPR" ; monitor b_hi;
double b_lo; assign b_lo to "{P}{HSC}b.LOPR" ; monitor b_lo;
double b_RB; assign b_RB to "{P}{HSC}bRB" ; monitor b_RB;
double load_b; assign load_b to "{P}{HSC}load_b.B" ;

double r; assign r to "{P}{HSC}r" ; monitor r;
double r_event; assign r_event to "{P}{HSC}r_event" ; monitor r_event; evflag r_event_mon; sync r_event r_event_mon;
double r_hi; assign r_hi to "{P}{HSC}r.HOPR" ; monitor r_hi;
double r_lo; assign r_lo to "{P}{HSC}r.LOPR" ; monitor r_lo;
double r_RB; assign r_RB to "{P}{HSC}rRB" ; monitor r_RB;
double load_r; assign load_r to "{P}{HSC}load_r.B" ;

double h0; assign h0 to "{P}{HSC}h0" ; monitor h0;
double h0_event; assign h0_event to "{P}{HSC}h0_event" ; monitor h0_event; evflag h0_event_mon; sync h0_event h0_event_mon;
double h0_hi; assign h0_hi to "{P}{HSC}h0.HOPR" ; monitor h0_hi;
double h0_lo; assign h0_lo to "{P}{HSC}h0.LOPR" ; monitor h0_lo;
double h0_RB; assign h0_RB to "{P}{HSC}h0RB" ; monitor h0_RB;
double load_h0; assign load_h0 to "{P}{HSC}load_h0.B" ;

double v0; assign v0 to "{P}{HSC}v0" ; monitor v0;
double v0_event; assign v0_event to "{P}{HSC}v0_event" ; monitor v0_event; evflag v0_event_mon; sync v0_event v0_event_mon;
double v0_hi; assign v0_hi to "{P}{HSC}v0.HOPR" ; monitor v0_hi;
double v0_lo; assign v0_lo to "{P}{HSC}v0.LOPR" ; monitor v0_lo;
double v0_RB; assign v0_RB to "{P}{HSC}v0RB" ; monitor v0_RB;
double load_v0; assign load_v0 to "{P}{HSC}load_v0.B" ;

double width; assign width to "{P}{HSC}width" ; monitor width;
double width_event; assign width_event to "{P}{HSC}width_event" ; monitor width_event; evflag width_event_mon; sync width_event width_event_mon;
double width_hi; assign width_hi to "{P}{HSC}width.HOPR" ; monitor width_hi;
double width_lo; assign width_lo to "{P}{HSC}width.LOPR" ; monitor width_lo;
double width_RB; assign width_RB to "{P}{HSC}widthRB" ; monitor width_RB;
double load_width; assign load_width to "{P}{HSC}load_width.B" ;

double height; assign height to "{P}{HSC}height" ; monitor height;
double height_event; assign height_event to "{P}{HSC}height_event" ; monitor height_event; evflag height_event_mon; sync height_event height_event_mon;
double height_hi; assign height_hi to "{P}{HSC}height.HOPR" ; monitor height_hi;
double height_lo; assign height_lo to "{P}{HSC}height.LOPR" ; monitor height_lo;
double height_RB; assign height_RB to "{P}{HSC}heightRB" ; monitor height_RB;
double load_height; assign load_height to "{P}{HSC}load_height.B" ;

int error; assign error to "{P}{HSC}error" ;
string errMsg; assign errMsg to "{P}{HSC}errMsg" ;





char myChar;
char* SNLtaskName;
double dScratch;
long numWords;
string word1;
string word2;
string word3;
string word4;
string word5;
string word6;
int hCSW;
int vCSW;
int h_isMoving;
int v_isMoving;
long pos_a;
long pos_b;
double pos_ag;
double pos_bg;
long l_raw;
long r_raw;
long t_raw;
long b_raw;
string id;
short hasID;
string hscErrors[14];
evflag start_H_move;
evflag start_V_move;
evflag wait_H_input;
evflag wait_V_input;
evflag new_H_RB;
evflag new_V_RB;
evflag move_h;
evflag move_v;
double t_old;
double l_old;
double b_old;
double r_old;
double width_old;
double height_old;
double h0_old;
double v0_old;
int i;
int delta;
int init_delay;
int ainp_fifo_write;
int ainp_fifo_read;
string ainp_fifo[40];
string scratch;


int hsc_timeout;
string hsc_command;
string hsc_response;
# 567 "../xia_slit.st"
ss xiahsc {

  state startup {
    when () {
      pvGet (s_ainp);
      pvGet (s_aout);
      { init = ( 1 ); pvPut(init,SYNC); };
      init_delay = 1.0;




      strcpy (port, macValueGet("S"));
      pvPut(port);

      strcpy (hscErrors[0], "Missing Command");
      strcpy (hscErrors[1], "Unrecognized Command");
      strcpy (hscErrors[2], "Input Buffer Overflow");
      strcpy (hscErrors[3], "No new Alias Given");
      strcpy (hscErrors[4], "Alias too long");
      strcpy (hscErrors[5], "Invalid Field Parameter");
      strcpy (hscErrors[6], "Value Out of Range");
      strcpy (hscErrors[7], "Parameter is read-only");
      strcpy (hscErrors[8], "Invalid/Missing Argument");
      strcpy (hscErrors[9], "No Movement Required");
      strcpy (hscErrors[10], "Uncalibrated: no motion allowed");
      strcpy (hscErrors[11], "Motion out of range");
      strcpy (hscErrors[12], "Invalid/missing direction character");
      strcpy (hscErrors[13], "Invalid Motor Specified");

      pvGet (hID,SYNC);
      pvGet (vID,SYNC);





      t_old = t; b_old = b;
      l_old = l; r_old = r;
      h0_old = h0; width_old = width;
      v0_old = v0; height_old = height;


      hOrigin = vOrigin = 400;
      hOuterLimit = vOuterLimit = 4400;

      { enable = ( 1 ); pvPut(enable,SYNC); };
      { s_tmot = ( .250 ); pvPut(s_tmot,SYNC); };

    } state init
  }



  state disable {
    when (enable) {
      init_delay = 1.0;
      { init = ( 1 ); pvPut(init,SYNC); };
    } state init
  }




  state comm_error {
    when (init || !s_sevr || delay((10*60.0))) {
      if (debug_flag >= 20) { printf("(%s,%d): ","../xia_slit.st",633); printf("comm_error: init=%d  s_sevr=%d",init,s_sevr); printf("\n"); }
      init_delay = 1.0;
      s_sevr = 0;
      { init = ( 1 ); pvPut(init,SYNC); };
    } state init
  }


  state init {
    when (!enable) {} state disable
    when ((efTest (hID_mon) || efTest (vID_mon))) {





      strcpy (port, macValueGet("S"));
      pvPut(port);



      init_delay = 0;
      efClear (hID_mon);
      efClear (vID_mon);
      efClear (hOrient_mon);
      efClear (vOrient_mon);
    } state init
    when (init && delay(init_delay)) {



      error = 0;
      if (!strcmp (hID, vID)) {
        { strcpy(errMsg,"H & V IDs must be different"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
        error++;
      }

      if (sscanf (hID, "XIAHSC-%c-%ld%s", &myChar, &numWords, scratch) != 2) {
        if (sscanf (hID, "%c-%ld%s", &myChar, &numWords, scratch) != 2) {
          if (sscanf (hID, "%c%ld%s", &myChar, &numWords, scratch) != 2) {
            { strcpy(errMsg,"H ID not a valid HSC ID"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
            error++;
          }
        }
      }
      if (sscanf (vID, "XIAHSC-%c-%ld%s", &myChar, &numWords, scratch) != 2) {
        if (sscanf (vID, "%c-%ld%s", &myChar, &numWords, scratch) != 2) {
          if (sscanf (vID, "%c%ld%s", &myChar, &numWords, scratch) != 2) {
            { strcpy(errMsg,"V ID not a valid HSC ID"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
            error++;
          }
        }
      }
      if (!strlen (hID)) {
        { strcpy(errMsg,"define H ID string"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
        error++;
      }
      if (!strlen (vID)) {
        { strcpy(errMsg,"define V ID string"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
        error++;
      }



      if (error) {
        { error = ( (17) ); pvPut(error,SYNC); };
      } else {
        { init = ( 0 ); pvPut(init,SYNC); };
        { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
      }
      pvPut(error);
      efClear (hID_mon);
      efClear (vID_mon);

      init_delay = 30;
    } state init
    when (!init && delay(0.1)) {
      { hBusy = ( (0) ); pvPut(hBusy,SYNC); };
      { vBusy = ( (0) ); pvPut(vBusy,SYNC); };
      { error = ( (0) ); pvPut(error,SYNC); };
      { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
      { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };



      { pvGet(s_optioniv); if( s_optioniv==(1) ) { { s_baud = ( (6) ); pvPut(s_baud,SYNC); }; { s_dbit = ( (4) ); pvPut(s_dbit,SYNC); }; { s_sbit = ( (1) ); pvPut(s_sbit,SYNC); }; { s_prty = ( (1) ); pvPut(s_prty,SYNC); }; { s_fctl = ( (1) ); pvPut(s_fctl,SYNC); }; } };



      { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); };
      { s_ifmt = ( 0 ); pvPut(s_ifmt,SYNC); };
      { s_nrrd = ( 0 ); pvPut(s_nrrd,SYNC); };
      { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); };
      { s_ofmt = ( 0 ); pvPut(s_ofmt,SYNC); };
      { s_tmod = ( 0 ); pvPut(s_tmod,SYNC); };
      { s_scan = ( (0) ); pvPut(s_scan,SYNC); };
      { s_tmot = ( .250 ); pvPut(s_tmot,SYNC); };

      efClear (s_ainp_mon);

      efClear (wait_H_input);
      efClear (wait_V_input);

      efClear (move_h);
      efClear (move_v);
      efClear (new_H_RB);
      efClear (new_V_RB);

      { stop = ( (0) ); pvPut(stop,SYNC); };
      epicsThreadSleep(.02);
      efClear (stop_mon);

      efClear (t_event_mon); efClear (l_event_mon);
      efClear (b_event_mon); efClear (r_event_mon);
      efClear (h0_event_mon); efClear (width_event_mon);
      efClear (v0_event_mon); efClear (height_event_mon);
      { calibrate = ( (0) ); pvPut(calibrate,SYNC); };

      efClear (hID_mon);
      efClear (vID_mon);
      efClear (hOrient_mon);
      efClear (vOrient_mon);





      { sprintf(hsc_command, "!%s %s","ALL",("K")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 760, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
      epicsThreadSleep(.1);
      { sprintf(hsc_command, "!%s %s","ALL",("P")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 762, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
      epicsThreadSleep(.1);
      ainp_fifo_read = 0;
      ainp_fifo_write = 0;
      while (!s_sevr) {
        { { s_tmod = ( (2) ); pvPut(s_tmod,SYNC); }; efClear(s_ainp_mon); { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; for (i = 0; i <= (30); i++) { if (efTest(s_ainp_mon)) { if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 767, SNLtaskName, 10);; printf("%s\n", "READ_HSC: got a message."); epicsThreadSleep(0.01); }; break; } epicsThreadSleep(0.02); } };
        { if (efTest(s_ainp_mon)) { strcpy(ainp_fifo[ainp_fifo_read++], s_ainp); { if ((ainp_fifo_read) >= 40) (ainp_fifo_read) = 0; }; delta = ainp_fifo_read-ainp_fifo_write; if (delta < 0) delta += 40; if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",768); printf("FIFO_IN: adding msg %d:\n ->'%s'",delta,s_ainp); printf("\n"); }; } };
      }
    } state init_limits
  }

  state init_limits {
    when () {






      error = 0;
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",hID,("R 5")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",782); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if (sscanf(hsc_response,"%*s %*s %d",&hStepDelay) == 1) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",782); printf("%s step delay %d",hID,hStepDelay); printf("\n"); }; pvPut(hStepDelay,SYNC); } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 782, SNLtaskName, 1);; printf("%s step delay format error", hID); printf("\n"); epicsThreadSleep(0.01); }; } };
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",hID,("R 6")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",783); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if (sscanf(hsc_response,"%*s %*s %d",&hGearBacklash) == 1) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",783); printf("%s backlash %d",hID,hGearBacklash); printf("\n"); }; pvPut(hGearBacklash,SYNC); } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 783, SNLtaskName, 1);; printf("%s backlash format error", hID); printf("\n"); epicsThreadSleep(0.01); }; } };
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",hID,("R 7")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",784); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if( sscanf(hsc_response,"%*s %*s %d",&hCSW) == 1 ) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",784); printf("%s cntrl word data %d",hID,hCSW); printf("\n"); }; { hStPwr = ( ((0x03) & hCSW) ); pvPut(hStPwr,SYNC); }; { hStLmts = ( (((0x04) & hCSW)?1:0) ); pvPut(hStLmts,SYNC); }; { hStBan = ( (((0x08) & hCSW)?1:0) ); pvPut(hStBan,SYNC); }; { hStEcho = ( (((0x10) & hCSW)?1:0) ); pvPut(hStEcho,SYNC); }; { hStLock = ( (((0x20) & hCSW)?1:0) ); pvPut(hStLock,SYNC); }; { hStAlias = ( (((0x40) & hCSW)?1:0) ); pvPut(hStAlias,SYNC); }; { hStText = ( (((0x80) & hCSW)?1:0) ); pvPut(hStText,SYNC); }; } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 784, SNLtaskName, 1);; printf("%s cntrl word format error", hID); printf("\n"); epicsThreadSleep(0.01); }; } };

      { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",hID,("R 1")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 786, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 786, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",786); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &hOuterLimit) == 1)
      {
        pvPut(hOuterLimit,SYNC);
      }
      else
      {
        error++;
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 794, SNLtaskName, 5);; printf("%s\n", "Could not read horizontal outer limit"); epicsThreadSleep(0.01); };
      }
      { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",hID,("R 2")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 796, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 796, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",796); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &hOrigin) == 1)
      {
        pvPut(hOrigin,SYNC);
      }
      else
      {
        error++;
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 804, SNLtaskName, 5);; printf("%s\n", "Could not read horizontal origin position"); epicsThreadSleep(0.01); };
      }
      if (!error) {
        pos_ag = (((0)-(hOrigin))/((400.0)) );
        pos_bg = (((hOuterLimit)-(hOrigin))/((400.0)) );
        { l_lo = ( pos_ag ); pvPut(l_lo,SYNC); }; { l_hi = ( pos_bg ); pvPut(l_hi,SYNC); };
        { r_lo = ( pos_ag ); pvPut(r_lo,SYNC); }; { r_hi = ( pos_bg ); pvPut(r_hi,SYNC); };







        {if ((l_lo)>(r_lo)) {(dScratch)=(l_lo);} else {(dScratch)=(r_lo);}};
        {if ((dScratch)>(0.0)) {(width_lo)=(dScratch);} else {(width_lo)=(0.0);}}; pvPut(width_lo);
        { width_hi = ( l_hi + r_hi ); pvPut(width_hi,SYNC); };
        { h0_hi = ( (r_hi - l_lo)/2 ); pvPut(h0_hi,SYNC); };
        { h0_lo = ( (r_lo - l_hi)/2 ); pvPut(h0_lo,SYNC); };
      }

      error = 0;
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",vID,("R 7")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",826); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if( sscanf(hsc_response,"%*s %*s %d",&vCSW) == 1 ) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",826); printf("%s cntrl word data %d",vID,vCSW); printf("\n"); }; { vStPwr = ( ((0x03) & vCSW) ); pvPut(vStPwr,SYNC); }; { vStLmts = ( (((0x04) & vCSW)?1:0) ); pvPut(vStLmts,SYNC); }; { vStBan = ( (((0x08) & vCSW)?1:0) ); pvPut(vStBan,SYNC); }; { vStEcho = ( (((0x10) & vCSW)?1:0) ); pvPut(vStEcho,SYNC); }; { vStLock = ( (((0x20) & vCSW)?1:0) ); pvPut(vStLock,SYNC); }; { vStAlias = ( (((0x40) & vCSW)?1:0) ); pvPut(vStAlias,SYNC); }; { vStText = ( (((0x80) & vCSW)?1:0) ); pvPut(vStText,SYNC); }; } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 826, SNLtaskName, 1);; printf("%s cntrl word format error", vID); printf("\n"); epicsThreadSleep(0.01); }; } };
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",vID,("R 6")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",827); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if (sscanf(hsc_response,"%*s %*s %d",&vGearBacklash) == 1) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",827); printf("%s backlash %d",vID,vGearBacklash); printf("\n"); }; pvPut(vGearBacklash,SYNC); } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 827, SNLtaskName, 1);; printf("%s backlash format error", vID); printf("\n"); epicsThreadSleep(0.01); }; } };
      { { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",vID,("R 5")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",828); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; }; if (sscanf(hsc_response,"%*s %*s %d",&vStepDelay) == 1) { if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",828); printf("%s step delay %d",vID,vStepDelay); printf("\n"); }; pvPut(vStepDelay,SYNC); } else { ++error; if (debug_flag >= 1) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 828, SNLtaskName, 1);; printf("%s step delay format error", vID); printf("\n"); epicsThreadSleep(0.01); }; } };

      { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",vID,("R 1")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 830, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 830, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",830); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &vOuterLimit) == 1)
      {
        pvPut(vOuterLimit,SYNC);
      }
      else
      {
        error++;
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 838, SNLtaskName, 5);; printf("%s\n", "Could not read vertical outer limit"); epicsThreadSleep(0.01); };
      }

      { efClear(s_ainp_mon); hsc_timeout = 1; { sprintf(hsc_command, "!%s %s",vID,("R 2")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 841, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (0) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }; { for (i = (30); i; i--) { if (efTest(s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(0.02); } if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 841, SNLtaskName, 10);; printf("\nWAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>", s_tinp); printf("\n"); epicsThreadSleep(0.01); }; } else { strcpy(hsc_response, s_ainp); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",841); printf("\nWAIT_HSC_RESPONSE: <%s> -> NORD=%d  TINP=%s",hsc_command,s_nord,s_tinp); printf("\n"); }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &vOrigin) == 1)
      {
        pvPut(vOrigin,SYNC);
      }
      else
      {
        error++;
        if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 849, SNLtaskName, 5);; printf("%s\n", "Could not read vertical origin position"); epicsThreadSleep(0.01); };
      }
      if (!error) {
        pos_ag = (((0)-(vOrigin))/((400.0)) );
        pos_bg = (((vOuterLimit)-(vOrigin))/((400.0)) );
        { t_lo = ( pos_ag ); pvPut(t_lo,SYNC); }; { t_hi = ( pos_bg ); pvPut(t_hi,SYNC); };
        { b_lo = ( pos_ag ); pvPut(b_lo,SYNC); }; { b_hi = ( pos_bg ); pvPut(b_hi,SYNC); };







        {if ((t_lo)>(b_lo)) {(dScratch)=(t_lo);} else {(dScratch)=(b_lo);}};
        {if ((dScratch)>(0.0)) {(height_lo)=(dScratch);} else {(height_lo)=(0.0);}}; pvPut(height_lo);
        { height_hi = ( t_hi + b_hi ); pvPut(height_hi,SYNC); };

        { v0_hi = ( (t_hi - b_lo)/2 ); pvPut(v0_hi,SYNC); };
        { v0_lo = ( (t_lo - b_hi)/2 ); pvPut(v0_lo,SYNC); };
      }
      { locate = ( 1 ); pvPut(locate,SYNC); };

    } state idle
  }



  state idle {




    when (!enable) { } state disable




    when (init) {
      init_delay = 1; { init = ( 1 ); pvPut(init,SYNC); }; { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); }; { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
    } state init




    when (efTest (stop_mon) && stop) {
      { sprintf(hsc_command, "!%s %s","ALL",("K")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 895, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
      epicsThreadSleep(.1);
      efClear (stop_mon);
    } state idle




    when ( efTest (l_event_mon) ) {
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 904, SNLtaskName, 2);; printf("efTest(l_event_mon) -> premove, l=%f", l); printf("\n"); epicsThreadSleep(0.01); };
    } state premove

    when ( efTest (l_event_mon) || efTest (r_event_mon)
    || efTest (t_event_mon) || efTest (b_event_mon)
    || efTest (h0_event_mon) || efTest (height_event_mon)
    || efTest (v0_event_mon) || efTest (width_event_mon) ) {
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 911, SNLtaskName, 2);; printf("%s\n", "efTest() -> premove"); epicsThreadSleep(0.01); };
    } state premove




    when (efTest (start_H_move)) {
      if (hBusy) {
        if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 919, SNLtaskName, 2);; printf("%s\n", "efTest(start_H_move) and hBusy"); epicsThreadSleep(0.01); };
        { sprintf(hsc_command, "!%s %s",hID,("K")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 920, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
        epicsThreadSleep(.5);
      } else {
        { hBusy = ( (1) ); pvPut(hBusy,SYNC); };
      }
      if (hOrient == (0)) {
        pos_ag = l; pos_bg = r;
      } else {
        pos_ag = r; pos_bg = l;
      }
      h_isMoving = 1;
      efSet (wait_H_input);
      efClear (start_H_move);

      while (!s_sevr) {
        { { s_tmod = ( (2) ); pvPut(s_tmod,SYNC); }; efClear(s_ainp_mon); { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; for (i = 0; i <= (30); i++) { if (efTest(s_ainp_mon)) { if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 935, SNLtaskName, 10);; printf("%s\n", "READ_HSC: got a message."); epicsThreadSleep(0.01); }; break; } epicsThreadSleep(0.02); } };

        sscanf(s_ainp, "%%%s", id);
        if (!strcmp(id, hID)) {
          if (debug_flag >= 2) { printf("(%s,%d): ","../xia_slit.st",939); printf("discarding hID message '%s' id='%s'\n",s_tinp,id); printf("\n"); };
        } else {
          { if (efTest(s_ainp_mon)) { strcpy(ainp_fifo[ainp_fifo_read++], s_ainp); { if ((ainp_fifo_read) >= 40) (ainp_fifo_read) = 0; }; delta = ainp_fifo_read-ainp_fifo_write; if (delta < 0) delta += 40; if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",941); printf("FIFO_IN: adding msg %d:\n ->'%s'",delta,s_ainp); printf("\n"); }; } };
        }
      }
      pos_a = ((pos_ag)*((400.0))+(0.5)+(hOrigin));
      pos_b = ((pos_bg)*((400.0))+(0.5)+(hOrigin));
      sprintf (scratch, "M %ld %ld", pos_a, pos_b);
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 947, SNLtaskName, 2);; printf("%s\n", "start_H_move: writing to motor"); epicsThreadSleep(0.01); };
      { sprintf(hsc_command, "!%s %s",hID,scratch); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 948, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
    } state idle

    when (efTest (start_V_move)) {
      if (vBusy) {
        { sprintf(hsc_command, "!%s %s",vID,("K")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 953, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
        epicsThreadSleep(.5);
      } else {
        { vBusy = ( (1) ); pvPut(vBusy,SYNC); };
      }
      if (vOrient == (0)) {
        pos_ag = t; pos_bg = b;
      } else {
        pos_ag = b; pos_bg = t;
      }
      v_isMoving = 1;
      efSet (wait_V_input);
      efClear (start_V_move);

      while (!s_sevr) {
        { { s_tmod = ( (2) ); pvPut(s_tmod,SYNC); }; efClear(s_ainp_mon); { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; for (i = 0; i <= (30); i++) { if (efTest(s_ainp_mon)) { if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 968, SNLtaskName, 10);; printf("%s\n", "READ_HSC: got a message."); epicsThreadSleep(0.01); }; break; } epicsThreadSleep(0.02); } };

        sscanf(s_ainp, "%%%s", id);
        if (!strcmp(id, vID)) {
          if (debug_flag >= 2) { printf("(%s,%d): ","../xia_slit.st",972); printf("discarding vID message '%s' id='%s'\n",s_tinp,id); printf("\n"); };
        } else {
          { if (efTest(s_ainp_mon)) { strcpy(ainp_fifo[ainp_fifo_read++], s_ainp); { if ((ainp_fifo_read) >= 40) (ainp_fifo_read) = 0; }; delta = ainp_fifo_read-ainp_fifo_write; if (delta < 0) delta += 40; if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",974); printf("FIFO_IN: adding msg %d:\n ->'%s'",delta,s_ainp); printf("\n"); }; } };
        }
      }
      pos_a = ((pos_ag)*((400.0))+(0.5)+(vOrigin));
      pos_b = ((pos_bg)*((400.0))+(0.5)+(vOrigin));
      sprintf (scratch, "M %ld %ld", pos_a, pos_b);
      { sprintf(hsc_command, "!%s %s",vID,scratch); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 980, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
    } state idle




    when (locate) {




      strcpy (port, macValueGet("S"));
      pvPut(port);
      { locate = ( 0 ); pvPut(locate,SYNC); };
      { sprintf(hsc_command, "!%s %s","ALL",("P")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 994, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; };
      epicsThreadSleep(.1);
      while (!s_sevr) {
        if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 997, SNLtaskName, 10);; printf("%s\n", "locate: calling READ_HSC"); epicsThreadSleep(0.01); };
        { { s_tmod = ( (2) ); pvPut(s_tmod,SYNC); }; efClear(s_ainp_mon); { s_proc = ( 1 ); pvPut(s_proc,SYNC); }; for (i = 0; i <= (30); i++) { if (efTest(s_ainp_mon)) { if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 998, SNLtaskName, 10);; printf("%s\n", "READ_HSC: got a message."); epicsThreadSleep(0.01); }; break; } epicsThreadSleep(0.02); } };
        if (!s_sevr) { if (efTest(s_ainp_mon)) { strcpy(ainp_fifo[ainp_fifo_read++], s_ainp); { if ((ainp_fifo_read) >= 40) (ainp_fifo_read) = 0; }; delta = ainp_fifo_read-ainp_fifo_write; if (delta < 0) delta += 40; if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",999); printf("FIFO_IN: adding msg %d:\n ->'%s'",delta,s_ainp); printf("\n"); }; } };
      }
      if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1001, SNLtaskName, 10);; printf("%s\n", "locate: s_sevr = TRUE"); epicsThreadSleep(0.01); };
    } state idle

    when (calibrate) {





      { sprintf(hsc_command, "!%s %s","ALL",("0 I")); if (debug_flag >= 10) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1010, SNLtaskName, 10);; printf("WRITE_HSC: <%s>", hsc_command); printf("\n"); epicsThreadSleep(0.01); }; { s_scan = ( (0) ); pvPut(s_scan,SYNC); }; { s_tmod = ( (1) ); pvPut(s_tmod,SYNC); }; { strcpy(s_ieos,("\r\n")); pvPut(s_ieos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_oeos,("\r")); pvPut(s_oeos,SYNC); epicsThreadSleep(0.01); }; { strcpy(s_aout,hsc_command); pvPut(s_aout,SYNC); epicsThreadSleep(0.01); }; }
      epicsThreadSleep(.1);
      { calibrate = ( (0) ); pvPut(calibrate,SYNC); };
      { locate = ( 1 ); pvPut(locate,SYNC); };
    } state idle





    when (ainp_fifo_read != ainp_fifo_write) {
      { delta = ainp_fifo_read-ainp_fifo_write; if (delta < 0) delta += 40; strcpy(scratch, ainp_fifo[ainp_fifo_write++]); if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",1021); printf("FIFO_OUT: got message %d:\n ->'%s'",delta,scratch); printf("\n"); }; { if ((ainp_fifo_write) >= 40) (ainp_fifo_write) = 0; }; };
      id[0] = word1[0] = word2[0] = word3[0] = word4[0] =
        word5[0] = word6[0] = 0;
      numWords = sscanf (scratch, "%s %s %s %s %s %s",
          word1, word2, word3, word4, word5, word6);
      hasID = sscanf (word1, "%%%s", id);
      if (debug_flag >= 5 && (numWords>0)) {
        printf("xia_slit:fifo_read: (%ld words)...\n'%s'\n", numWords, scratch);
      }
    } state process_response




    when (efTest (new_H_RB)) {
      efClear (new_H_RB);




      if (debug_flag >= 2) { printf("(%s,%d): ","../xia_slit.st",1041); printf("efTest(new_H_RB)==1; h_Busy|hBusy=%d, l=%f,l_RB=%f",h_Busy|hBusy,l,l_RB); printf("\n"); };
      if (!hBusy && !h_Busy) {
        if ((fabs((l)-(l_RB))>((1.e-9)))) { load_l = ( l_RB ); pvPut(load_l,SYNC); };
        if ((fabs((r)-(r_RB))>((1.e-9)))) { load_r = ( r_RB ); pvPut(load_r,SYNC); };
        if ((fabs((h0)-(h0_RB))>((1.e-9)))) { load_h0 = ( h0_RB ); pvPut(load_h0,SYNC); };
        if ((fabs((width)-(width_RB))>((1.e-9)))) { load_width = ( width_RB ); pvPut(load_width,SYNC); };
        efClear (wait_H_input);
      }
    } state idle




    when (efTest (new_V_RB)) {
      efClear (new_V_RB);




      if (!vBusy && !v_Busy) {
        if ((fabs((t)-(t_RB))>((1.e-9)))) { load_t = ( t_RB ); pvPut(load_t,SYNC); };
        if ((fabs((b)-(b_RB))>((1.e-9)))) { load_b = ( b_RB ); pvPut(load_b,SYNC); };
        if ((fabs((v0)-(v0_RB))>((1.e-9)))) { load_v0 = ( v0_RB ); pvPut(load_v0,SYNC); };
        if ((fabs((height)-(height_RB))>((1.e-9)))) { load_height = ( height_RB ); pvPut(load_height,SYNC); };
        efClear (wait_V_input);
      }
    } state idle




    when (stop && !efTest (wait_H_input) && !efTest (wait_V_input)) {
      { stop = ( (0) ); pvPut(stop,SYNC); };
      efClear (stop_mon);
    } state idle




    when ((hBusy || vBusy) && delay ((0.2))) {
      { locate = ( 1 ); pvPut(locate,SYNC); };
    } state idle
    when (delay ((30))) {
      { locate = ( 1 ); pvPut(locate,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1085, SNLtaskName, 2);; printf("%s\n", "IDLE POLL"); epicsThreadSleep(0.01); };
    } state idle
  }



  state premove {



    when (efTest (l_event_mon) && (l == l_old)) {
      efClear (l_event_mon);
      { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1098, SNLtaskName, 2);; printf("%s\n", "l==l_old: ->h_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (r_event_mon) && (r == r_old)) {
      efClear (r_event_mon);
      { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1103, SNLtaskName, 2);; printf("%s\n", "r==r_old: ->h_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (t_event_mon) && (t == t_old)) {
      efClear (t_event_mon);
      { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1108, SNLtaskName, 2);; printf("%s\n", "t==t_old: ->v_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (b_event_mon) && (b == b_old)) {
      efClear (b_event_mon);
      { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1113, SNLtaskName, 2);; printf("%s\n", "b==b_old: ->v_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (h0_event_mon) && (h0 == h0_old)) {
      efClear (h0_event_mon);
      { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1118, SNLtaskName, 2);; printf("%s\n", "h0==h0_old: ->h_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (v0_event_mon) && (v0 == v0_old)) {
      efClear (v0_event_mon);
      { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1123, SNLtaskName, 2);; printf("%s\n", "v0==v0_old: ->v_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (width_event_mon) && (width == width_old)) {
      efClear (width_event_mon);
      { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1128, SNLtaskName, 2);; printf("%s\n", "width==width_old: ->h_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when (efTest (height_event_mon) && (height == height_old)) {
      efClear (height_event_mon);
      { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
      if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1133, SNLtaskName, 2);; printf("%s\n", "height==height_old: ->v_BusyOff"); epicsThreadSleep(0.01); };
    } state idle
    when () {



      if (efTest (l_event_mon) || efTest (r_event_mon) ||
          efTest (h0_event_mon) || efTest (width_event_mon)) {

        if (efTest (l_event_mon) || efTest (r_event_mon)) {
          width = r + l;
          h0 = (r - l) / 2;
        } else {
          l = width / 2 - h0;
          r = width / 2 + h0;
        }

        error = 0;
        if (!(((l_lo)<=(l))&&((l)<=(l_hi)))) { l = l_old; error++; }
        if (!(((r_lo)<=(r))&&((r)<=(r_hi)))) { r = r_old; error++; }
        if (!(((h0_lo)<=(h0))&&((h0)<=(h0_hi)))) { h0 = h0_old; error++; }
        if (!(((width_lo)<=(width))&&((width)<=(width_hi)))) {
           width = width_old; error++; }
        if (error) {

          if (efTest (l_event_mon)) { load_l = ( l ); pvPut(load_l,SYNC); };
          if (efTest (r_event_mon)) { load_r = ( r ); pvPut(load_r,SYNC); };
          if (efTest (h0_event_mon)) { load_h0 = ( h0 ); pvPut(load_h0,SYNC); };
          if (efTest (width_event_mon)) { load_width = ( width ); pvPut(load_width,SYNC); };
          error = (15);
          { strcpy(errMsg,"H soft limits exceeded"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
          { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
        } else {
          { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };

          efSet (start_H_move);
          if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1169, SNLtaskName, 2);; printf("%s\n", "premove: efSet(start_H_move)"); epicsThreadSleep(0.01); };
          if (efTest (l_event_mon) || efTest (r_event_mon)) {
            { load_h0 = ( h0 ); pvPut(load_h0,SYNC); }; { load_width = ( width ); pvPut(load_width,SYNC); };
          } else {
            { load_l = ( l ); pvPut(load_l,SYNC); }; { load_r = ( r ); pvPut(load_r,SYNC); };
          }
        }
        pvPut(error);
        efClear(l_event_mon); efClear(r_event_mon);
        efClear(h0_event_mon); efClear(width_event_mon);
      }



      if (efTest (t_event_mon) || efTest (b_event_mon)
      || efTest (v0_event_mon) || efTest (height_event_mon)) {

        if (efTest (t_event_mon) || efTest (b_event_mon)) {
          height = t + b;
          v0 = (t - b) / 2;
        } else {
          t = height / 2 + v0;
          b = height / 2 - v0;
        }

        error = 0;
        if (!(((t_lo)<=(t))&&((t)<=(t_hi)))) { t = t_old; error++; }
        if (!(((b_lo)<=(b))&&((b)<=(b_hi)))) { b = b_old; error++; }
        if (!(((v0_lo)<=(v0))&&((v0)<=(v0_hi)))) { v0 = v0_old; error++; }
        if (!(((height_lo)<=(height))&&((height)<=(height_hi)))) {
           height = height_old; error++; }
        if (error) {

          if (efTest (t_event_mon)) { load_t = ( t ); pvPut(load_t,SYNC); };
          if (efTest (b_event_mon)) { load_b = ( b ); pvPut(load_b,SYNC); };
          if (efTest (v0_event_mon)) { load_v0 = ( v0 ); pvPut(load_v0,SYNC); };
          if (efTest (height_event_mon)) { load_height = ( height ); pvPut(load_height,SYNC); };
          error = (15);
          { strcpy(errMsg,"V soft limits exceeded"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
          { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
        } else {
          { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };

          efSet (start_V_move);
          if (efTest (t_event_mon) || efTest (b_event_mon)) {
            { load_v0 = ( v0 ); pvPut(load_v0,SYNC); }; { load_height = ( height ); pvPut(load_height,SYNC); };
          } else {
            { load_t = ( t ); pvPut(load_t,SYNC); }; { load_b = ( b ); pvPut(load_b,SYNC); };
          }
        }
        pvPut(error);
        efClear(t_event_mon); efClear(b_event_mon);
        efClear(v0_event_mon); efClear(height_event_mon);
      }
    } state idle
  }



  state process_response {
    entry {
      if (debug_flag >= 5) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1230, SNLtaskName, 5);; printf("process_response:entry numWords=%ld", numWords); printf("\n"); epicsThreadSleep(0.01); };
    }



    when (numWords == -1) {




    } state idle
    when ( (numWords == 2) && (!strcmp(word2, "OK;")) ) {

    } state idle
    when ( (numWords >= 2) && (!strcmp(word2, "ERROR;")) ) {
      if (numWords == 2) {
        { error = ( (16) ); pvPut(error,SYNC); };
        sprintf (errMsg, "%s: unknown error", id);
        pvPut(errMsg);
      } else {
        sscanf (word3, "%d", &error);
        pvPut(error);
        { strcpy(errMsg,hscErrors[error]); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
      }
      if (!strcmp(word1, hID)) {
        if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1255, SNLtaskName, 2);; printf("'%s' reported an error: ->h_BusyOff", hID); printf("\n"); epicsThreadSleep(0.01); };
        { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
        h_isMoving = 0;
      } else if (!strcmp(word1, vID)) {
        if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1259, SNLtaskName, 2);; printf("'%s' reported an error: ->v_BusyOff", vID); printf("\n"); epicsThreadSleep(0.01); };
        { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
        v_isMoving = 0;
      }
    } state idle



    when ( (numWords == 2) && (!strcmp(word2, "BUSY;")) ) {

      if (!strcmp(id, hID)) {
        { hBusy = ( (1) ); pvPut(hBusy,SYNC); };
      } else {
        { vBusy = ( (1) ); pvPut(vBusy,SYNC); };
      }
      if (error) {
        { error = ( 0 ); pvPut(error,SYNC); };
        { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
      }
    } state idle




    when ( (numWords == 4) && (!strcmp(word4, "DONE;")) ) {

      sscanf (word2, "%ld", &pos_a);
      sscanf (word3, "%ld", &pos_b);
      if (error) {
        { error = ( 0 ); pvPut(error,SYNC); };
        { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
      }

      if (!strcmp(id, hID)) {
        if (debug_flag >= 5) { printf("(%s,%d): ","../xia_slit.st",1293); printf("hDONE: pos_a = %ld, pos_b = %ld",pos_a,pos_b); printf("\n"); };
        pos_ag = (((pos_a)-(hOrigin))/((400.0)) );
        pos_bg = (((pos_b)-(hOrigin))/((400.0)) );
        h_isMoving = 0;
        l_raw = ((l)*((400.0))+(0.5)+(hOrigin));
        r_raw = ((r)*((400.0))+(0.5)+(hOrigin));
        if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",1299); printf("hRaw; l=%ld, r=%ld",l_raw,r_raw); printf("\n"); };
        if (hOrient == (0)) {
          if ((pos_a == l_raw) && (pos_b == r_raw)) { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
        } else {
          if ((pos_a == r_raw) && (pos_b == l_raw)) { h_BusyOff = ( 0 ); pvPut(h_BusyOff,SYNC); };
        }
      } else if (!strcmp(id, vID)){
        if (debug_flag >= 5) { printf("(%s,%d): ","../xia_slit.st",1306); printf("vDONE: pos_a = %ld, pos_b = %ld",pos_a,pos_b); printf("\n"); };
        pos_ag = (((pos_a)-(vOrigin))/((400.0)) );
        pos_bg = (((pos_b)-(vOrigin))/((400.0)) );
        v_isMoving = 0;
        t_raw = ((t)*((400.0))+(0.5)+(vOrigin));
        b_raw = ((b)*((400.0))+(0.5)+(vOrigin));
        if (debug_flag >= 10) { printf("(%s,%d): ","../xia_slit.st",1312); printf("vRaw; t=%ld, b=%ld",t_raw,b_raw); printf("\n"); };
        if (vOrient == (0)) {
          if ((pos_a == t_raw) && (pos_b == b_raw)) { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
        } else {
          if ((pos_a == b_raw) && (pos_b == t_raw)) { v_BusyOff = ( 0 ); pvPut(v_BusyOff,SYNC); };
        }
      }
    } state update_positions

    when ( (numWords == 5) && (!strcmp(word2, "OK")) ) {

      sscanf (word3, "%ld", &pos_a);
      sscanf (word4, "%ld", &pos_b);
      if (error) {
        { error = ( 0 ); pvPut(error,SYNC); };
        { strcpy(errMsg,"no error"); pvPut(errMsg,SYNC); epicsThreadSleep(0.01); };
      }
      if (!strcmp(id, hID)) {
        pos_ag = (((pos_a)-(hOrigin))/((400.0)) );
        pos_bg = (((pos_b)-(hOrigin))/((400.0)) );
      } else {
        pos_ag = (((pos_a)-(vOrigin))/((400.0)) );
        pos_bg = (((pos_b)-(vOrigin))/((400.0)) );
      }
    } state update_positions

    when () {

      printf("xia_slit: did not understand reply: '%s' '%s' '%s' '%s' '%s' '%s'\n",
          word1, word2, word3, word4, word5, word6);
      epicsThreadSleep(.1);
    } state idle
  }



  state update_positions {
    when () {

      if (!strcmp(id, hID)) {
        if (hOrient == (0)) {
          { l_RB = ( pos_ag ); pvPut(l_RB,SYNC); };
          { r_RB = ( pos_bg ); pvPut(r_RB,SYNC); };
        } else {
          { r_RB = ( pos_ag ); pvPut(r_RB,SYNC); };
          { l_RB = ( pos_bg ); pvPut(l_RB,SYNC); };
        }
        { width_RB = ( l_RB+r_RB ); pvPut(width_RB,SYNC); };
        { h0_RB = ( (r_RB-l_RB)/2 ); pvPut(h0_RB,SYNC); };
        if (debug_flag >= 2) { printf("<%s,%d,%s,%d> ", "../xia_slit.st", 1361, SNLtaskName, 2);; printf("%s\n", "update_positions: efSet(new_H_RB)"); epicsThreadSleep(0.01); };
        efSet (new_H_RB);
        { hBusy = ( (0) ); pvPut(hBusy,SYNC); };




        l_old = l_RB; r_old = r_RB;
        h0_old = h0_RB; width_old = width_RB;
      }

      if (!strcmp(id, vID)) {
        if (vOrient == (0)) {
          { t_RB = ( pos_ag ); pvPut(t_RB,SYNC); };
          { b_RB = ( pos_bg ); pvPut(b_RB,SYNC); };
        } else {
          { b_RB = ( pos_ag ); pvPut(b_RB,SYNC); };
          { t_RB = ( pos_bg ); pvPut(t_RB,SYNC); };
        }
        { height_RB = ( t_RB+b_RB ); pvPut(height_RB,SYNC); };
        { v0_RB = ( (t_RB-b_RB)/2 ); pvPut(v0_RB,SYNC); };
        efSet (new_V_RB);
        { vBusy = ( (0) ); pvPut(vBusy,SYNC); };




        t_old = t_RB; b_old = b_RB;
        v0_old = v0_RB; height_old = height_RB;
      }
    } state idle
  }

}
