# 1 "../xiahsc.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../xiahsc.st"
program xiahsc ("name=hsc1, S=ito:serial:b0, P=ito:, HSC=hsc1:")

option +r;
option +d;
# 86 "../xiahsc.st"
string s_ainp; assign s_ainp to "{S}.AINP"; monitor s_ainp; evflag s_ainp_mon; sync s_ainp s_ainp_mon;
string s_aout; assign s_aout to "{S}.AOUT";
short s_baud; assign s_baud to "{S}.BAUD";
short s_dbit; assign s_dbit to "{S}.DBIT";
short s_fctl; assign s_fctl to "{S}.FCTL";

string s_ieos; assign s_ieos to "{S}.IEOS";
short s_ifmt; assign s_ifmt to "{S}.IFMT";
short s_nord; assign s_nord to "{S}.NORD"; monitor s_nord;
short s_nrrd; assign s_nrrd to "{S}.NRRD";

string s_oeos; assign s_oeos to "{S}.OEOS";
short s_ofmt; assign s_ofmt to "{S}.OFMT";
short s_proc; assign s_proc to "{S}.PROC"; monitor s_proc;
short s_prty; assign s_prty to "{S}.PRTY";
short s_sbit; assign s_sbit to "{S}.SBIT";
short s_scan; assign s_scan to "{S}.SCAN";
int s_sevr; assign s_sevr to "{S}.SEVR"; monitor s_sevr;
short s_tmod; assign s_tmod to "{S}.TMOD";

double s_tmot; assign s_tmot to "{S}.TMOT"; monitor s_tmot;





string port; assign port to "{P}{HSC}port";
string hID; assign hID to "{P}{HSC}hID"; monitor hID; evflag hID_mon; sync hID hID_mon;
string vID; assign vID to "{P}{HSC}vID"; monitor vID; evflag vID_mon; sync vID vID_mon;
short hOrient; assign hOrient to "{P}{HSC}hOrient"; monitor hOrient; evflag hOrient_mon; sync hOrient hOrient_mon;
short vOrient; assign vOrient to "{P}{HSC}vOrient"; monitor vOrient; evflag vOrient_mon; sync vOrient vOrient_mon;
short hBusy; assign hBusy to "{P}{HSC}hBusy";
short vBusy; assign vBusy to "{P}{HSC}vBusy";
short stop; assign stop to "{P}{HSC}stop"; monitor stop; evflag stop_mon; sync stop stop_mon;
short locate; assign locate to "{P}{HSC}locate"; monitor locate;
short init; assign init to "{P}{HSC}init"; monitor init;
short enable; assign enable to "{P}{HSC}enable"; monitor enable;
short calibrate; assign calibrate to "{P}{HSC}calib"; monitor calibrate;
double t; assign t to "{P}{HSC}t"; monitor t; evflag t_mon; sync t t_mon;
double t_hi; assign t_hi to "{P}{HSC}t.HOPR"; monitor t_hi;
double t_lo; assign t_lo to "{P}{HSC}t.LOPR"; monitor t_lo;
double t_RB; assign t_RB to "{P}{HSC}tRB"; monitor t_RB;
double l; assign l to "{P}{HSC}l"; monitor l; evflag l_mon; sync l l_mon;
double l_hi; assign l_hi to "{P}{HSC}l.HOPR"; monitor l_hi;
double l_lo; assign l_lo to "{P}{HSC}l.LOPR"; monitor l_lo;
double l_RB; assign l_RB to "{P}{HSC}lRB"; monitor l_RB;
double b; assign b to "{P}{HSC}b"; monitor b; evflag b_mon; sync b b_mon;
double b_hi; assign b_hi to "{P}{HSC}b.HOPR"; monitor b_hi;
double b_lo; assign b_lo to "{P}{HSC}b.LOPR"; monitor b_lo;
double b_RB; assign b_RB to "{P}{HSC}bRB"; monitor b_RB;
double r; assign r to "{P}{HSC}r"; monitor r; evflag r_mon; sync r r_mon;
double r_hi; assign r_hi to "{P}{HSC}r.HOPR"; monitor r_hi;
double r_lo; assign r_lo to "{P}{HSC}r.LOPR"; monitor r_lo;
double r_RB; assign r_RB to "{P}{HSC}rRB"; monitor r_RB;
double h0; assign h0 to "{P}{HSC}h0"; monitor h0; evflag h0_mon; sync h0 h0_mon;
double h0_hi; assign h0_hi to "{P}{HSC}h0.HOPR"; monitor h0_hi;
double h0_lo; assign h0_lo to "{P}{HSC}h0.LOPR"; monitor h0_lo;
double h0_RB; assign h0_RB to "{P}{HSC}h0RB"; monitor h0_RB;
double v0; assign v0 to "{P}{HSC}v0"; monitor v0; evflag v0_mon; sync v0 v0_mon;
double v0_hi; assign v0_hi to "{P}{HSC}v0.HOPR"; monitor v0_hi;
double v0_lo; assign v0_lo to "{P}{HSC}v0.LOPR"; monitor v0_lo;
double v0_RB; assign v0_RB to "{P}{HSC}v0RB"; monitor v0_RB;
double width; assign width to "{P}{HSC}width"; monitor width; evflag width_mon; sync width width_mon;
double width_hi; assign width_hi to "{P}{HSC}width.HOPR"; monitor width_hi;
double width_lo; assign width_lo to "{P}{HSC}width.LOPR"; monitor width_lo;
double width_RB; assign width_RB to "{P}{HSC}widthRB"; monitor width_RB;
double height; assign height to "{P}{HSC}height"; monitor height; evflag height_mon; sync height height_mon;
double height_hi; assign height_hi to "{P}{HSC}height.HOPR"; monitor height_hi;
double height_lo; assign height_lo to "{P}{HSC}height.LOPR"; monitor height_lo;
double height_RB; assign height_RB to "{P}{HSC}heightRB"; monitor height_RB;
int error; assign error to "{P}{HSC}error";
string errMsg; assign errMsg to "{P}{HSC}errMsg";

int xiahscDebug; assign xiahscDebug to "{P}{HSC}debug"; monitor xiahscDebug;




char scratch[256];
char myChar;
double dScratch;
long numWords;
string word1; string word2; string word3;
string word4; string word5; string word6;
long pos_a; long pos_b;
double pos_ag; double pos_bg;
string id; short hasID;
string hscErrors[14];
evflag start_H_move; evflag start_V_move;
evflag wait_H_input; evflag wait_V_input;
evflag new_H_RB; evflag new_V_RB;
evflag move_h; evflag move_v;
double t_old; double l_old;
double b_old; double r_old;
double width_old; double height_old;
double h0_old; double v0_old;
int init_delay;
int i;
int count;
int hOrigin; int vOrigin;
int hOuterLimit; int vOuterLimit;
string hsc_command;
string hsc_response;
int hsc_timeout;


%% #include <math.h>
%% #include <string.h>
%% #include <epicsThread.h>
# 329 "../xiahsc.st"
ss xiahsc {

  state startup {
    when () {
      xiahscDebug = 0;
      pvGet (s_ainp);
      pvGet (s_aout);
      {init = (1); pvPut(init);};
      init_delay = 1.0;




      strcpy (port, macValueGet("S"));
      pvPut (port);

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





      t_old = t; b_old = b;
      l_old = l; r_old = r;
      h0_old = h0; width_old = width;
      v0_old = v0; height_old = height;


      hOrigin = vOrigin = 400;
      hOuterLimit = vOuterLimit = 4400;

      {enable = (1); pvPut(enable);};

      {s_tmot = (.250); pvPut(s_tmot);};

    } state init
  }



  state disable {
    when (enable) {
      init_delay = 1.0;
      {init = (1); pvPut(init);};
    } state init
  }




  state comm_error {
    when (init || !s_sevr || delay((10 * 60.0))) {
      if (xiahscDebug >= 20) { printf("(%s,%d): ", "../xiahsc.st", 394); printf("comm_error: init=%d  s_sevr=%d",init,s_sevr); printf("\n"); }
      init_delay = 1.0;
      s_sevr = 0;
      {init = (1); pvPut(init);};
    } state init
  }


  state init {
    when (!enable) {} state disable
    when ( efTest (hID_mon) || efTest (vID_mon) ) {
# 417 "../xiahsc.st"
      strcpy (port, macValueGet("S"));
      pvPut (port);



      init_delay = 0;
      init = 1;
      efClear (hID_mon);
      efClear (vID_mon);
      efClear (hOrient_mon);
      efClear (vOrient_mon);
    } state init
    when (init && delay(init_delay)) {



      error = 0;
      if (!strcmp (hID, vID)) {
        {strcpy(errMsg, "H & V IDs must be different"); pvPut(errMsg);};
        error++;
      }
      if (sscanf (hID, "XIAHSC-%c-%ld%s", &myChar, &numWords, scratch) != 2) {
        if (sscanf (hID, "%c-%ld%s", &myChar, &numWords, scratch) != 2) {
   {strcpy(errMsg, "H ID not a valid HSC ID"); pvPut(errMsg);};
          error++;
        }
      }
      if (sscanf (vID, "XIAHSC-%c-%ld%s", &myChar, &numWords, scratch) != 2) {
        if (sscanf (vID, "%c-%ld%s", &myChar, &numWords, scratch) != 2) {
          {strcpy(errMsg, "V ID not a valid HSC ID"); pvPut(errMsg);};
          error++;
        }
      }
      if (!strlen (hID)) {
        {strcpy(errMsg, "define H ID string"); pvPut(errMsg);};
        error++;
      }
      if (!strlen (vID)) {
        {strcpy(errMsg, "define V ID string"); pvPut(errMsg);};
        error++;
      }



      if (error) {
        {error = (17); pvPut(error);};
      } else {
        {init = (0); pvPut(init);};
        {strcpy(errMsg, "no error"); pvPut(errMsg);};
      }
      pvPut (error);
      efClear (hID_mon);
      efClear (vID_mon);

      init_delay = 30;
    } state init
    when (!init && delay(0.1)) {
      {hBusy = (0); pvPut(hBusy);};
      {vBusy = (0); pvPut(vBusy);};
      {error = (0); pvPut(error);};




      { {s_baud = (6); pvPut(s_baud);}; {s_dbit = (4); pvPut(s_dbit);}; {s_sbit = (1); pvPut(s_sbit);}; {s_prty = (1); pvPut(s_prty);}; {s_fctl = (1); pvPut(s_fctl);}; };




      {strcpy(s_ieos, "\n"); pvPut(s_ieos);};
      {s_ifmt = (0); pvPut(s_ifmt);};
      {s_nrrd = (0); pvPut(s_nrrd);};

      {strcpy(s_oeos, "\r"); pvPut(s_oeos);};
      {s_ofmt = (0); pvPut(s_ofmt);};
      {s_tmod = (0); pvPut(s_tmod);};
      {s_scan = (0); pvPut(s_scan);};

      {s_tmot = (.250); pvPut(s_tmot);};

      efClear (s_ainp_mon);

      efClear (wait_H_input);
      efClear (wait_V_input);

      efClear (move_h);
      efClear (move_v);
      efClear (new_H_RB);
      efClear (new_V_RB);

      {stop = (0); pvPut(stop);};
      epicsThreadSleep(.02);
      efClear (stop_mon);

      efClear (t_mon); efClear (l_mon);
      efClear (b_mon); efClear (r_mon);
      efClear (h0_mon); efClear (width_mon);
      efClear (v0_mon); efClear (height_mon);
      {calibrate = (0); pvPut(calibrate);};

      efClear (hID_mon);
      efClear (vID_mon);
      efClear (hOrient_mon);
      efClear (vOrient_mon);





      { if (!s_sevr) { sprintf (hsc_command, "!%s %s", "ALL", "K"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 526); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } };
      epicsThreadSleep(.1);
# 536 "../xiahsc.st"
      error = 0;
      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", hID, "R 1"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 537); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 537); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 537); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &hOuterLimit) != 1) {
  error++;
 if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 540); printf("Could not read horizontal outer limit"); printf("\n"); };
      }
      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", hID, "R 2"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 542); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 542); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 542); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &hOrigin) != 1) {
  error++;
 if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 545); printf("Could not read horizontal origin position"); printf("\n"); };
      }

      if (!error) {
  pos_ag = ( 1.0*(0 -hOrigin)/400 );
  pos_bg = ( 1.0*(hOuterLimit-hOrigin)/400 );
  {l_lo = (pos_ag); pvPut(l_lo);}; {l_hi = (pos_bg); pvPut(l_hi);};
  {r_lo = (pos_ag); pvPut(r_lo);}; {r_hi = (pos_bg); pvPut(r_hi);};







 {if ((l_lo)>(r_lo)) {dScratch=(l_lo);} else {dScratch=(r_lo);}};
 {if ((dScratch)>(0.0)) {width_lo=(dScratch);} else {width_lo=(0.0);}}; pvPut (width_lo);
 {width_hi = (l_hi + r_hi); pvPut(width_hi);};




 {h0_hi = ((r_hi - l_lo)/2); pvPut(h0_hi);};
 {h0_lo = ((r_lo - l_hi)/2); pvPut(h0_lo);};
      }

      error = 0;
      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", vID, "R 1"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 572); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 572); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 572); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &vOuterLimit)!= 1) {
        error++;
 if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 575); printf("Could not read vertical outer limit"); printf("\n"); };
      }
      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", vID, "R 2"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 577); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 577); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 577); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %d", &vOrigin)!= 1) {
        error++;
 if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 580); printf("Could not read vertical origin position"); printf("\n"); };
      }
      if (!error) {
        pos_ag = ( 1.0*(0 -vOrigin)/400 );
        pos_bg = ( 1.0*(vOuterLimit-vOrigin)/400 );
        {t_lo = (pos_ag); pvPut(t_lo);}; {t_hi = (pos_bg); pvPut(t_hi);};
        {b_lo = (pos_ag); pvPut(b_lo);}; {b_hi = (pos_bg); pvPut(b_hi);};







 {if ((t_lo)>(b_lo)) {dScratch=(t_lo);} else {dScratch=(b_lo);}};
 {if ((dScratch)>(0.0)) {height_lo=(dScratch);} else {height_lo=(0.0);}}; pvPut (height_lo);
 {height_hi = (t_hi + b_hi); pvPut(height_hi);};




 {v0_hi = ((t_hi - b_lo)/2); pvPut(v0_hi);};
 {v0_lo = ((t_lo - b_hi)/2); pvPut(v0_lo);};
      }
      {locate = (1); pvPut(locate);};

    } state idle
  }


  state idle {

    when (!enable) {} state disable
    when (s_sevr) {
      {error = (18); pvPut(error);};
      sprintf (scratch, "communications error: %d", s_sevr);
      {strcpy(errMsg, scratch); pvPut(errMsg);};
    } state comm_error
    when (efTest (hID_mon) || efTest (vID_mon)) {} state init
    when (efTest (hOrient_mon) || efTest (vOrient_mon)) {} state init


    when (init) { init_delay = 1; {init = (1); pvPut(init);}; } state init

    when (efTest (stop_mon) && stop) {





      { if (!s_sevr) { sprintf (hsc_command, "!%s %s", "ALL", "K"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 630); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }
      epicsThreadSleep(.1);
      efClear (stop_mon);
      {locate = (1); pvPut(locate);};
    } state idle




    when ( efTest (l_mon) || efTest (r_mon)
      || efTest (t_mon) || efTest (b_mon)
      || efTest (h0_mon) || efTest (height_mon)
      || efTest (v0_mon) || efTest (width_mon) ) {
    } state premove





    when ( (efTest (start_H_move) || efTest (start_V_move))
    && delay (0.1)) {
      if (efTest (start_H_move)) {
        if (hBusy) {
          { if (!s_sevr) { sprintf (hsc_command, "!%s %s", hID, "K"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 653); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } };
          epicsThreadSleep(.1);
        } else {
          {hBusy = (1); pvPut(hBusy);};
        }
        if (hOrient == 0) {
          pos_ag = l; pos_bg = r;
        } else {
          pos_ag = r; pos_bg = l;
        }
        strcpy (word1, hID);
        efSet (wait_H_input);
        efClear (start_H_move);



        pos_a = ( (pos_ag) * 400 + 0.5 + hOrigin );
        pos_b = ( (pos_bg) * 400 + 0.5 + hOrigin );
      }
      if (efTest (start_V_move)) {
        if (vBusy) {
          { if (!s_sevr) { sprintf (hsc_command, "!%s %s", vID, "K"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 674); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } };
          epicsThreadSleep(.1);
        } else {
          {vBusy = (1); pvPut(vBusy);};
        }
        if (vOrient == 0) {
          pos_ag = t; pos_bg = b;
        } else {
          pos_ag = b; pos_bg = t;
        }
        strcpy (word1, vID);
        efSet (wait_V_input);
        efClear (start_V_move);



        pos_a = ( (pos_ag) * 400 + 0.5 + vOrigin );
        pos_b = ( (pos_bg) * 400 + 0.5 + vOrigin );
      }
      sprintf (scratch, "M %ld %ld", pos_a, pos_b);
      { if (!s_sevr) { sprintf (hsc_command, "!%s %s", word1, scratch); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 694); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } };
      {locate = (1); pvPut(locate);};
    } state idle

    when (locate) {
# 707 "../xiahsc.st"
      strcpy (port, macValueGet("S"));
      pvPut (port);
      {locate = (0); pvPut(locate);};
    } state get_readback_positions

    when ( calibrate ) {





      { if (!s_sevr) { sprintf (hsc_command, "!%s %s", "ALL", "0 I"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 718); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (1); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }
      epicsThreadSleep(.1);
      {calibrate = (0); pvPut(calibrate);};
      {locate = (1); pvPut(locate);};
    } state idle

    when (efTest (new_H_RB)) {
      efClear (new_H_RB);




      if (!hBusy) {
        if (l != l_RB) {l = (l_RB); pvPut(l);};
        if (r != r_RB) {r = (r_RB); pvPut(r);};
        if (h0 != h0_RB) {h0 = (h0_RB); pvPut(h0);};
        if (width != width_RB) {width = (width_RB); pvPut(width);};
        epicsThreadSleep(.02);
        efClear (l_mon); efClear (r_mon);
        efClear (h0_mon); efClear (width_mon);
        efClear (wait_H_input);
      }
    } state idle

    when (efTest (new_V_RB)) {
      efClear (new_V_RB);




      if (!vBusy) {
        if (t != t_RB) {t = (t_RB); pvPut(t);};
        if (b != b_RB) {b = (b_RB); pvPut(b);};
        if (v0 != v0_RB) {v0 = (v0_RB); pvPut(v0);};
        if (height != height_RB) {height = (height_RB); pvPut(height);};
        epicsThreadSleep(.02);
        efClear (t_mon); efClear (b_mon);
        efClear (v0_mon); efClear (height_mon);
        efClear (wait_V_input);
      }
    } state idle

    when ((hBusy || vBusy) && delay (0.25)) {
      {locate = (1); pvPut(locate);};
    } state idle
    when (delay (5.0)) {
      {locate = (1); pvPut(locate);};
    } state idle

  }


  state get_readback_positions {
    when (!enable) {} state disable
    when ( ) {




      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", hID, "P"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 777); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 777); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 777); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %ld %ld", &pos_a, &pos_b) == 2) {
        pos_ag = ( 1.0*(pos_a-hOrigin)/400 );
        pos_bg = ( 1.0*(pos_b-hOrigin)/400 );
        if (hOrient == 0) {
          if (pos_ag != l_RB) { {l_RB = (pos_ag); pvPut(l_RB);} };
          if (pos_bg != r_RB) { {r_RB = (pos_bg); pvPut(r_RB);} };
        } else {
          if (pos_ag != r_RB) { {r_RB = (pos_ag); pvPut(r_RB);} };
          if (pos_bg != l_RB) { {l_RB = (pos_bg); pvPut(l_RB);} };
        }
 if (hBusy) {hBusy = (0); pvPut(hBusy);};



        {width_RB = (l_RB+r_RB); pvPut(width_RB);};
        {h0_RB = ((r_RB-l_RB)/2); pvPut(h0_RB);};
        efSet (new_H_RB);




        l_old = l_RB; r_old = r_RB;
        h0_old = h0_RB; width_old = width_RB;
      } else
        if (sscanf (hsc_response, "%*s %*s %s", scratch) == 1) {
          if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 803); printf("axis=%s, command=<%s>, response=%s","H",hsc_command,hsc_response); printf("\n"); };
   if (!strcmp (scratch, "BUSY;")) {
     if (!hBusy) {hBusy = (1); pvPut(hBusy);};
   } else {
            if (xiahscDebug >= 20) { printf("(%s,%d): ", "../xiahsc.st", 807); printf("could not understand %s",hsc_response); printf("\n"); };
   }
 }





      { efClear (s_ainp_mon); hsc_timeout = 1; { if (!s_sevr) { sprintf (hsc_command, "!%s %s", vID, "P"); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 815); printf("WRITE_HSC: <%s>",hsc_command); printf("\n"); }; {s_scan = (0); pvPut(s_scan);}; {s_tmod = (0); pvPut(s_tmod);}; {strcpy(s_ieos, "\n"); pvPut(s_ieos);}; {strcpy(s_oeos, "\r"); pvPut(s_oeos);}; {strcpy(s_aout, hsc_command); pvPut(s_aout);}; } }; { if (!s_sevr) { for (i = 15; i; i--) { if (efTest (s_ainp_mon)) { hsc_timeout = 0; break; } epicsThreadSleep(.02); }; if (hsc_timeout == 1) { strcpy(hsc_response, ""); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 815); printf("WAIT_HSC_RESPONSE: <%s> -> <<< TIMEOUT >>>",hsc_command); printf("\n"); }; } else { strcpy(hsc_response, s_ainp); if (xiahscDebug >= 10) { printf("(%s,%d): ", "../xiahsc.st", 815); printf("WAIT_HSC_RESPONSE: <%s> -> NORD=%d  AINP=%s",hsc_command,s_nord,hsc_response); printf("\n"); }; }; } }; };
      if (sscanf (hsc_response, "%*s %*s %ld %ld", &pos_a, &pos_b) == 2) {
        pos_ag = ( 1.0*(pos_a-vOrigin)/400 );
        pos_bg = ( 1.0*(pos_b-vOrigin)/400 );
        if (vOrient == 0) {
          if (pos_ag != t_RB) { {t_RB = (pos_ag); pvPut(t_RB);} };
          if (pos_bg != b_RB) { {b_RB = (pos_bg); pvPut(b_RB);} };
        } else {
          if (pos_ag != b_RB) { {b_RB = (pos_ag); pvPut(b_RB);} };
          if (pos_bg != t_RB) { {t_RB = (pos_bg); pvPut(t_RB);} };
        }
 if (vBusy) {vBusy = (0); pvPut(vBusy);};



        {height_RB = (t_RB+b_RB); pvPut(height_RB);};
        {v0_RB = ((t_RB-b_RB)/2); pvPut(v0_RB);};
        efSet (new_V_RB);




        t_old = t_RB; b_old = b_RB;
        v0_old = v0_RB; height_old = height_RB;
      } else
        if (sscanf (hsc_response, "%*s %*s %s", scratch) == 1) {
          if (xiahscDebug >= 5) { printf("(%s,%d): ", "../xiahsc.st", 841); printf("axis=%s, command=<%s>, response=%s","V",hsc_command,hsc_response); printf("\n"); };
   if (!strcmp (scratch, "BUSY;")) {
     if (!vBusy) {vBusy = (1); pvPut(vBusy);};
   } else {
            if (xiahscDebug >= 20) { printf("(%s,%d): ", "../xiahsc.st", 845); printf("could not understand %s",hsc_response); printf("\n"); };
   }
 }

    } state idle
  }


  state premove {



    when (efTest (l_mon) && (l == l_old)) { efClear (l_mon); } state idle
    when (efTest (r_mon) && (r == r_old)) { efClear (r_mon); } state idle
    when (efTest (t_mon) && (t == t_old)) { efClear (t_mon); } state idle
    when (efTest (b_mon) && (b == b_old)) { efClear (b_mon); } state idle
    when (efTest (h0_mon) && (h0 == h0_old)) { efClear (h0_mon); } state idle
    when (efTest (v0_mon) && (v0 == v0_old)) { efClear (v0_mon); } state idle
    when (efTest (width_mon) && (width == width_old)) {
       efClear (width_mon); } state idle
    when (efTest (height_mon) && (height == height_old)) {
       efClear (height_mon); } state idle
    when () {



      if (efTest (l_mon) || efTest (r_mon)
      || efTest (h0_mon) || efTest (width_mon)) {

        if (efTest (l_mon) || efTest (r_mon)) {
          width = r + l;
          h0 = (r - l) / 2;
        } else {
          l = width / 2 - h0;
          r = width / 2 + h0;
        }

        error = 0;
        if (!( (l_lo <= l) && (l <= l_hi) )) { l = l_old; error++; }
        if (!( (r_lo <= r) && (r <= r_hi) )) { r = r_old; error++; }
        if (!( (h0_lo <= h0) && (h0 <= h0_hi) )) { h0 = h0_old; error++; }
        if (!( (width_lo <= width) && (width <= width_hi) )) {
           width = width_old; error++; }
        if (error) {

          if (efTest (l_mon)) pvPut (l);
          if (efTest (r_mon)) pvPut (r);
          if (efTest (h0_mon)) pvPut (h0);
          if (efTest (width_mon)) pvPut (width);
          error = 15;
          {strcpy(errMsg, "H soft limits exceeded"); pvPut(errMsg);};
        } else {
          {strcpy(errMsg, "no error"); pvPut(errMsg);};

          efSet (start_H_move);
          if (efTest (l_mon) || efTest (r_mon)) {
            pvPut (h0); pvPut (width);
          } else {
            pvPut (l); pvPut (r);
          }
        }
        epicsThreadSleep(.02);
        efClear(l_mon); efClear(r_mon);
        efClear(h0_mon); efClear(width_mon);
        pvPut (error);
      }



      if (efTest (t_mon) || efTest (b_mon)
      || efTest (v0_mon) || efTest (height_mon)) {

        if (efTest (t_mon) || efTest (b_mon)) {
          height = t + b;
          v0 = (t - b) / 2;
        } else {
          t = height / 2 + v0;
          b = height / 2 - v0;
        }

        error = 0;
        if (!( (t_lo <= t) && (t <= t_hi) )) { t = t_old; error++; }
        if (!( (b_lo <= b) && (b <= b_hi) )) { b = b_old; error++; }
        if (!( (v0_lo <= v0) && (v0 <= v0_hi) )) { v0 = v0_old; error++; }
        if (!( (height_lo <= height) && (height <= height_hi) )) {
           height = height_old; error++; }
        if (error) {

          if (efTest (t_mon)) pvPut (t);
          if (efTest (b_mon)) pvPut (b);
          if (efTest (v0_mon)) pvPut (v0);
          if (efTest (height_mon)) pvPut (height);
          error = 15;
          {strcpy(errMsg, "V soft limits exceeded"); pvPut(errMsg);};
        } else {

          efSet (start_V_move);
          if (efTest (t_mon) || efTest (b_mon)) {
            pvPut (v0); pvPut (height);
          } else {
            pvPut (t); pvPut (b);
          }
        }
        epicsThreadSleep(.02);
        efClear(t_mon); efClear(b_mon);
        efClear(v0_mon); efClear(height_mon);
        pvPut (error);
      }
    } state idle
  }

}
