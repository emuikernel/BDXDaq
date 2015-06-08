h17898
s 00001/00001/00366
d D 1.13 01/10/26 15:26:09 wolin 16 15
c Typo
e
s 00008/00004/00359
d D 1.12 01/10/26 15:25:15 wolin 15 14
c Added live_random
e
s 00001/00001/00362
d D 1.11 01/01/03 10:52:14 wolin 14 13
c is_open()
e
s 00002/00002/00361
d D 1.10 00/11/13 10:56:13 wolin 13 10
i 12
c Accepted child's version in workspace "/usr/local/clas/devel/source".
c 
e
s 00002/00001/00361
d D 1.8.1.2 00/11/13 10:54:18 wolin 12 11
c Added clock_rate=100000
e
s 00002/00002/00360
d D 1.8.1.1 00/10/26 16:54:29 wolin 11 9
c const
e
s 00001/00001/00361
d D 1.9 00/11/09 14:36:31 wolin 10 9
c 100k clock
e
s 00069/00029/00293
d D 1.8 00/07/27 16:53:24 wolin 9 8
c Probably still needs improvements
e
s 00029/00038/00293
d D 1.7 99/10/04 14:31:21 wolin 8 7
c Still improving...
c 
e
s 00089/00028/00242
d D 1.6 99/10/01 15:33:45 wolin 7 6
c Still adding more l2 scalers
c 
e
s 00039/00023/00231
d D 1.5 99/08/16 16:46:35 wolin 6 5
c Added l2 stuff
c 
e
s 00013/00005/00241
d D 1.4 98/10/08 13:05:04 wolin 5 4
c Minor mods
c 
e
s 00002/00001/00244
d D 1.3 98/10/06 15:39:08 wolin 4 3
c Still developing...
c 
e
s 00015/00001/00230
d D 1.2 98/10/06 11:58:32 wolin 3 1
c Added livetime
c 
e
s 00000/00000/00000
d R 1.2 98/10/06 11:46:23 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/scaler_file_analysis.cc
e
s 00231/00000/00000
d D 1.1 98/10/06 11:46:22 wolin 1 0
c 
e
u
U
f e 0
t
T
I 1
//  scaler_file_analysis

//  analyzes scaler file data

//  E.Wolin, 5-oct-98


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


// system stuff
#include <iostream.h>
I 6
#include <iomanip.h>
E 6
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


// program variables
static int run;
static int debug              = 0;
D 9
static char *scaler_file      = "/usr/local/clas/parms/scalers/archive/dd2scaler_clasprod_%06d.txt";
E 9
I 9
D 11
D 13
static char *scaler_file      = "/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
E 13
I 13
static char *scaler_file      = (char*)"/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
E 13
E 11
I 11
static char *scaler_file      = (char*)"/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
I 12
static int clock_rate         = 100000;
E 12
E 11
E 9
static char filename[256];
static char line[256];
D 4
static char junk[256];
E 4


// scaler file variables
static unsigned long rcst30[15];
D 6
static unsigned long s1st00[17];
E 6
I 6
D 7
static unsigned long s1st00[29];
E 6
static unsigned long trgs00[4][16];
E 7
I 7
static unsigned long s1st00[35];
static unsigned long trgs00[5][16];
I 9
static unsigned long trgd00[5][16];
E 9
E 7




// prototypes
void read_scaler_file(ifstream &file);
void analyze_scalers(void);
D 11
D 13
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen);
E 13
I 13
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
E 13
E 11
I 11
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
E 11
int get_next_line(ifstream &file, char buffer[], int buflen);
void decode_command_line(int argc, char **argv);



//-----------------------------------------------------------------------


main(int argc, char **argv) {
  

  // synch with c i/o
  ios::sync_with_stdio();


  // decode command-line arguments
  decode_command_line(argc,argv);


  // get scaler file name
  sprintf(filename,scaler_file,run);


  // open scaler file
  ifstream scal(filename);
D 14
  if(scal.bad()) {
E 14
I 14
  if(!scal.is_open()) {
E 14
    cerr << "Unable to open scaler file " << filename << endl << endl;
    exit(EXIT_FAILURE);
  }


  // read and analyze file
  read_scaler_file(scal);
  analyze_scalers();


  // done
  exit(EXIT_SUCCESS);

}


//------------------------------------------------------------------


void read_scaler_file(ifstream &file) {

  int i,j,k,ind;


  // read scaler1 RCST
  if(find_tag_line(file,"*RCST30*",line,sizeof(line))==0) {
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&rcst30[i],&ind);}
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=8; i<15; i++) {j+=ind; sscanf(&line[j],"%lu %n",&rcst30[i],&ind);}
  }


  // read S1ST
  if(find_tag_line(file,"*S1ST00*",line,sizeof(line))==0) {
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=8; i<16; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
D 6
    j=0; ind=0; for(i=16; i<17; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
E 6
I 6
    j=0; ind=0; for(i=16; i<24; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
D 7
    j=0; ind=0; for(i=24; i<29; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
E 7
I 7
    j=0; ind=0; for(i=24; i<32; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=32; i<35; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
E 7
E 6
  }


  // read trigger scalers
  if(find_tag_line(file,"*TRGS00*",line,sizeof(line))==0) {
D 7
    for(k=0; k<4; k++) {
E 7
I 7
    for(k=0; k<5; k++) {
E 7
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgs00[k][i]),&ind);}
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=8; i<16; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgs00[k][i]),&ind);}
    }
I 9
  }
  if(find_tag_line(file,"*TRGD00*",line,sizeof(line))==0) {
    for(k=0; k<5; k++) {
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgd00[k][i]),&ind);}
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=8; i<16; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgd00[k][i]),&ind);}
    }
E 9
  }  

I 9

E 9
}


//------------------------------------------------------------------


void analyze_scalers(void) {

  int i;
I 5
  int tbits_on,s1bits_on;
E 5

  unsigned long rc_tot  = rcst30[6];
  unsigned long rc_phys = rcst30[7];
  unsigned long rc_scal = rcst30[8];
  unsigned long rc_ill  = rcst30[12];
  unsigned long rc_psyn = rcst30[14];
  
D 7
  unsigned long s1_tot      = s1st00[12];
E 7
I 7
D 8
  unsigned long s1_int      = s1st00[12];
E 7
  unsigned long s1_zero     = s1st00[13];
  unsigned long s1_empty    = s1st00[14];
  unsigned long s1_notempty = s1st00[15];
  unsigned long s1_ok       = s1st00[16];
  unsigned long s1_sum      = 0;
E 8
I 8
  unsigned long s1_phys       = s1st00[12];
  unsigned long s1l1_zero     = s1st00[13];
  unsigned long s1l1_empty    = s1st00[14];
  unsigned long s1l1_notempty = s1st00[15];
  unsigned long s1l1_ok       = s1st00[16];
  unsigned long s1_sum        = 0;
E 8

I 6
D 7
  unsigned long s1l2_pass     = s1st00[23];
  unsigned long s1l2_fail     = s1st00[24];
  unsigned long s1l2_zero     = s1st00[25];
  unsigned long s1l2_empty    = s1st00[26];
  unsigned long s1l2_notempty = s1st00[27];
  unsigned long s1l2_ok       = s1st00[28];

E 6
  unsigned long trgs_tot = trgs00[3][12];
  unsigned long trgs_int = trgs00[3][13];
E 7
I 7
  unsigned long s1l2_pass     	      = s1st00[23];
  unsigned long s1l2_latefail 	      = s1st00[24];
  unsigned long s1l2_zero     	      = s1st00[25];
  unsigned long s1l2_empty    	      = s1st00[26];
  unsigned long s1l2_notempty 	      = s1st00[27];
  unsigned long s1l2_ok       	      = s1st00[28];
  unsigned long s113_count    	      = s1st00[29];
  unsigned long s115_count    	      = s1st00[30];
  unsigned long s1l1l2_zero_count     = s1st00[31];
  unsigned long s113_l1_zero_count    = s1st00[32];
  unsigned long s113_l2_zero_count    = s1st00[33];
  unsigned long s113_l1l2_zero_count  = s1st00[34];

D 9
  unsigned long trgs_tot      = trgs00[3][12];
  unsigned long trgs_int      = trgs00[3][13];
  unsigned long trgs_l2fail   = trgs00[4][0];
  unsigned long trgs_l2pass   = trgs00[4][1];
  unsigned long trgs_l2start  = trgs00[4][2];
  unsigned long trgs_l2clear  = trgs00[4][3];
I 8
  unsigned long trgs_l2accept = trgs00[4][4];
  unsigned long trgs_l3accept = trgs00[4][5];
E 9
I 9
  unsigned long trgs_tot       	 = trgs00[3][12];
  unsigned long trgs_or_l1_live	 = trgs00[2][2];
  unsigned long trgs_l1accept  	 = trgs00[3][13];
  unsigned long trgs_l2fail    	 = trgs00[4][0];
  unsigned long trgs_l2pass    	 = trgs00[4][1];
  unsigned long trgs_l2start   	 = trgs00[4][2];
  unsigned long trgs_l2clear   	 = trgs00[4][3];
  unsigned long trgs_l2accept  	 = trgs00[4][4];
  unsigned long trgs_l3accept  	 = trgs00[4][5];
E 9
E 8
E 7
  unsigned long trgs_sum = 0;

I 9
D 10
D 12
  double deltat                  = (double)trgs00[0][0]/10000;
E 10
I 10
  double deltat                  = (double)trgs00[0][0]/100000;
E 10
E 12
I 12
  double deltat                  = (double)trgs00[0][0]/clock_rate;
E 12
  double trgd_tot       	 = (double)trgs00[3][12]/deltat;
  double trgd_l1accept  	 = (double)trgs00[3][13]/deltat;
  double trgd_l2fail    	 = (double)trgs00[4][0]/deltat;
  double trgd_l2pass    	 = (double)trgs00[4][1]/deltat;
  double trgd_l2start   	 = (double)trgs00[4][2]/deltat;
  double trgd_l2clear   	 = (double)trgs00[4][3]/deltat;
  double trgd_l2accept  	 = (double)trgs00[4][4]/deltat;
  double trgd_l3accept  	 = (double)trgs00[4][5]/deltat;

E 9
I 3
D 15
  double live_clock = 0.;
  double live_fcup  = 0.;
  double live_trig  = 0.;
I 9
  double live_or    = 0.;
E 15
I 15
  double live_clock   = 0.;
  double live_fcup    = 0.;
  double live_trig    = 0.;
  double live_or      = 0.;
  double live_random  = 0.;
E 15
E 9

E 3
  
I 7

E 7
I 4
  cout << "\n\nScaler file analysis for run " << run << endl;
E 4

I 4

E 4
  // dump rcst
  cout << "\n\nROC 30\n------" << endl << endl;
D 6
  cout << "total trigs is:        " << rc_tot << endl;
  cout << "illegal trig is:       " << rc_ill << endl;
  cout << "scal is:               " << rc_scal << endl;
  cout << "phys trig is:          " << rc_phys << endl;
  cout << "phys sync is:          " << rc_psyn << endl;
  cout << "phys + psyn is:        " << rc_phys+rc_psyn << endl;
  cout << "tot-scal-ill is:       " << rc_tot-rc_scal-rc_ill << endl;
E 6
I 6
  cout << "total trigs is:       " << setw(8) << rc_tot << endl;
  cout << "illegal trig is:      " << setw(8) << rc_ill << endl;
D 7
  cout << "scal is:              " << setw(8) << rc_scal << endl;
E 7
I 7
  cout << "scaler events is:     " << setw(8) << rc_scal << endl;
E 7
  cout << "phys trig is:         " << setw(8) << rc_phys << endl;
  cout << "phys sync is:         " << setw(8) << rc_psyn << endl;
  cout << "phys + psyn is:       " << setw(8) << rc_phys+rc_psyn << endl;
D 7
  cout << "tot-scal-ill is:      " << setw(8) << rc_tot-rc_scal-rc_ill << endl;
E 7
I 7
  cout << "tot-scal-illegal is:  " << setw(8) << rc_tot-rc_scal-rc_ill << endl;
E 7
E 6
  if((rc_tot-rc_scal-rc_ill)!=(rc_phys+rc_psyn)) {
D 7
    cout << " *** ROC event stats don't agree *** " << endl;
E 7
I 7
    cout << " *** ROC tot-scal-ill != phys+psync ***" << endl;
  }
D 8
  if((rc_phys+rc_psyn)!=s1_int) {
E 8
I 8
  if((rc_phys+rc_psyn)!=s1_phys) {
E 8
    cout << " *** ROC phys+psync != s1 phys+psync ***" << endl;
E 7
  }
  

  // dump s1st
D 5
  for(i=0; i<12; i++) s1_sum+=s1st00[i];
E 5
I 5
  s1bits_on=0;
  for(i=0; i<12; i++) {s1_sum+=s1st00[i]; s1bits_on+=(s1st00[i]!=0);}
E 5
  cout << "\n\nS1ST\n----" << endl << endl;
I 9
  cout << "s1 bits on is:        " << setw(8) << s1bits_on << endl;
E 9
D 6
  cout << "phys + psyn is:        " << s1_tot << endl;
  cout << "latch zero is:         " << s1_zero << endl;
  cout << "latch empty is:        " << s1_empty << endl;
  cout << "latch not empty is:    " << s1_notempty << endl;
  cout << "latch ok is:           " << s1_ok << endl;
  cout << "s1 sum is:             " << s1_sum << endl;
I 5
  cout << "s1 bits on is:         " << s1bits_on << endl;
E 6
I 6
D 7
  cout << "phys + psyn is:       " << setw(8) << s1_tot << endl;
E 7
I 7
D 8
  cout << "phys + psyn is:       " << setw(8) << s1_int << endl;
E 7
  cout << "l1 latch zero is:     " << setw(8) << s1_zero << "      ";
E 8
I 8
  cout << "phys + psync is:      " << setw(8) << s1_phys << endl;
  cout << "l1 latch zero is:     " << setw(8) << s1l1_zero << "      ";
E 8
  cout << "l2 latch zero is:     " << setw(8) << s1l2_zero << endl;
D 8
  cout << "l1 latch empty is:    " << setw(8) << s1_empty << "      ";
E 8
I 8
  cout << "l1 latch empty is:    " << setw(8) << s1l1_empty << "      ";
E 8
  cout << "l2 latch empty is:    " << setw(8) << s1l2_empty << endl;
D 8
  cout << "l1 latch not empty:   " << setw(8) << s1_notempty << "      ";
E 8
I 8
  cout << "l1 latch not empty:   " << setw(8) << s1l1_notempty << "      ";
E 8
  cout << "l2 latch not empty:   " << setw(8) << s1l2_notempty << endl;
D 8
  cout << "l1 latch ok is:       " << setw(8) << s1_ok << "      ";
E 8
I 8
  cout << "l1 latch ok is:       " << setw(8) << s1l1_ok << "      ";
E 8
  cout << "l2 latch ok is:       " << setw(8) << s1l2_ok << endl;
  cout << "s1 sum is:            " << setw(8) << s1_sum << "      ";
  cout << "l2 pass is:           " << setw(8) << s1l2_pass << endl;
D 9
  cout << "s1 bits on is:        " << setw(8) << s1bits_on << "      ";
E 9
I 9
  cout << "roc 13 is:            " << setw(8) << s113_count << "      ";
E 9
D 7
  cout << "l2 fail is:           " << setw(8) << s1l2_fail << endl;
E 6
E 5
  if(s1_tot!=(s1_ok+s1_zero+s1_empty)) {
    cout << " *** s1 tot not equal ok + zero + empty *** " << endl;
E 7
I 7
  cout << "l2 latefail is:       " << setw(8) << s1l2_latefail << endl;
D 9
  cout << "s1 roc 13 is:         " << setw(8) << s113_count << endl;
  cout << "s1 roc 15 is:         " << setw(8) << s115_count << endl;
  cout << "s1 13 l1 zero is:     " << setw(8) << s113_l1_zero_count << "      ";
  cout << "s1 13 l2 zero is:     " << setw(8) << s113_l2_zero_count << endl;
  cout << "s1 l1l2 zero is:      " << setw(8) << s1l1l2_zero_count << endl;
  cout << "s1 13 l1l2 zero is:   " << setw(8) << s113_l1l2_zero_count << endl;
E 9
I 9
  cout << "roc 15 is:            " << setw(8) << s115_count << endl;
  cout << "13 l1 zero is:        " << setw(8) << s113_l1_zero_count << "      ";
  cout << "13 l2 zero is:        " << setw(8) << s113_l2_zero_count << endl;
  cout << "l1l2 zero is:         " << setw(8) << s1l1l2_zero_count << endl;
  cout << "13 l1l2 zero is:      " << setw(8) << s113_l1l2_zero_count << endl;
E 9

D 8



E 8
  // s1st internal checks
D 8
  if(s1_int!=(s1_ok+s1_zero+s1_empty)) {
E 8
I 8
  if(s1_phys!=(s1l1_ok+s1l1_zero+s1l1_empty)) {
E 8
    cout << " *** interrupts not equal ok+zero+empty ***" << endl;
  }
D 8
  if(s1_sum<s1_int) {
    cout << " *** s1 trigger bit sum less than s1 interrupt count ***" << endl;
E 8
I 8
  if((s1bits_on==1)&&((s1_sum+s1l1_zero)!=s1_phys)) {
    cout << " *** s1 trigger bit sum+zero != s1 phys for single trigger ***" << endl;
E 8
E 7
  }
D 7
  if(s1_sum<s1_tot) {
    cout << " *** s1 sum less than s1 tot *** " << endl;
E 7
I 7
D 8
  if((s1l2_pass+s1l2_latefail)!=s1_int) {
    cout << " *** s1 pass+latefail != s1 interrupt count ***" << endl;
E 8
I 8
  if((s1l2_pass+s1l2_latefail)!=s1_phys) {
    cout << " *** s1 pass+latefail != s1 phys count ***" << endl;
  }
  if((s1bits_on==1)&&(s1_sum!=s1l1_ok)) {
    cout << " *** s1 trigger bit sum != s1 l1 ok for single trigger ***" << endl;
E 8
E 7
  }
I 7
D 8

E 8
E 7
  

  // dump trgs
D 5
  for(i=0; i<12; i++) trgs_sum+=trgs00[3][i];
E 5
I 5
  tbits_on=0;
  for(i=0; i<12; i++) {trgs_sum+=trgs00[3][i]; tbits_on+=(trgs00[3][i]!=0);}
E 5
  cout << "\n\nTRGS\n----" << endl << endl;
D 6
  cout << "raw trigs is:          " << trgs_tot << endl;
D 3
  cout << "interrupts is:         " << trgs_int << endl;
E 3
  cout << "trg sum is:            " << trgs_sum << endl;
I 3
  cout << "interrupts is:         " << trgs_int << endl;
I 5
  cout << "trgs bits on is:       " << tbits_on << endl;
E 6
I 6
D 7
  cout << "raw trigs is:         " << setw(8) << trgs_tot << endl;
  cout << "trg sum is:           " << setw(8) << trgs_sum << endl;
E 7
I 7
D 9
  cout << "or_level1 is:         " << setw(8) << trgs_tot << endl;
E 9
I 9
  cout << "bits on is:           " << setw(8) << tbits_on << endl;
  cout << "or_level1 is:         " << setw(8) << trgs_tot
       << "   (" << trgd_tot << ")" << endl;
  cout << "or_level1 live is:    " << setw(8) << trgs_or_l1_live << endl;
E 9
  cout << "l1 sum is:            " << setw(8) << trgs_sum << endl;
E 7
D 9
  cout << "interrupts is:        " << setw(8) << trgs_int << endl;
  cout << "trgs bits on is:      " << setw(8) << tbits_on << endl;
E 6
E 5
E 3
D 7
  if(trgs_int!=s1_tot) {
    cout << " *** TRGS interrups don't agree with S1ST *** " << endl;
E 7
I 7
  cout << "trgs l2fail:          " << setw(8) << trgs_l2fail << endl;
  cout << "trgs l2pass:          " << setw(8) << trgs_l2pass << endl;
  cout << "trgs l2start:         " << setw(8) << trgs_l2start << endl;
  cout << "trgs l2clear:         " << setw(8) << trgs_l2clear << endl;
I 8
  cout << "trgs l2accept:        " << setw(8) << trgs_l2accept << "      ";
  cout << "trgs l3accept:        " << setw(8) << trgs_l3accept << endl;
E 9
I 9
  cout << "l1accepts is:         " << setw(8) << trgs_l1accept
       << "   (" << trgd_l1accept << ")" << endl;
  cout << "l2start is:           " << setw(8) << trgs_l2start
       << "   (" << trgd_l2start << ")" << endl;
  cout << "l2clear is:           " << setw(8) << trgs_l2clear
       << "   (" << trgd_l2clear << ")" << endl;
  cout << "l2pass is:            " << setw(8) << trgs_l2pass
       << "   (" << trgd_l2pass << ")" << endl;
  cout << "l2fail is:            " << setw(8) << trgs_l2fail
       << "   (" << trgd_l2fail << ")" << endl;
  cout << "l2accept is:          " << setw(8) << trgs_l2accept
       << "   (" << trgd_l2accept << ")" << endl;
  cout << "l3accept is:          " << setw(8) << trgs_l3accept
       << "   (" << trgd_l3accept << ")" << endl;
E 9
E 8

  // trgs internal checks
  if((trgs_sum!=trgs_tot)&&(tbits_on==1)) {
D 8
    cout << " *** sum of l1 bits != or_level1 for single trigger bit on *** " << endl;
E 7
  }
  if(trgs_sum<trgs_tot) {
D 7
    cout << " *** trgs sum less than trgs tot *** " << endl;
E 7
I 7
    cout << " *** sum of l1 bits < or_level1 *** " << endl;
E 8
I 8
    cout << " *** sum of l1 bits != or_level1 for single trigger *** " << endl;
E 8
  }
  if((trgs_l2fail+trgs_l2pass)!=trgs_l2start) {
    cout << " *** trgs l2pass+l2fail != trgs l2start *** " << endl;
E 7
  }
I 7
D 8
  if(trgs_l2fail!=trgs_l2clear) {
    cout << " *** trgs l2fail != l2clear (ok for late fail mode) *** " << endl;
  }
  if(trgs_int!=trgs_l2pass) {
    cout << " *** trgs interrupts != l2pass (ok for late fail mode) *** " << endl;
  }

E 8


  // compare trgs to s1st
D 8
  if(trgs_int!=s1_int) {
E 8
I 8
  if(trgs_l2fail!=trgs_l2clear+s1l2_latefail) {
    cout << " *** trgs l2fail != (l2clear+latefail) *** " << endl;
  }
D 9
  if(trgs_int!=s1_phys) {
E 8
    cout << " *** trgs interrupts don't agree with s1 interrupts *** " << endl;
E 9
I 9
  if((trgs_l2pass+s1l2_latefail)!=s1_phys) {
    cout << " *** trgs (l2pass+latefail) doesn't agree with s1 interrupts *** " << endl;
  }
  if((trgs_l2pass+s1l2_latefail)!=trgs_l2accept) {
    cout << " *** trgs (l2pass+latefail) doesn't agree with l2 accepts *** " << endl;
  }
  if(trgs_l2accept!=trgs_l3accept) {
    cout << " *** trgs l2 accept doesn't agree with l3 accepts *** " << endl;
E 9
  }
  if(tbits_on!=s1bits_on) {
    cout << " *** trgs bits on != s1 bits on *** " << endl;
  }
D 8
  if(trgs_l2pass!=s1_int) {
    cout << " *** trgs l2pass != s1_int (ok for late fail mode) *** " << endl;
  }


E 8
E 7


I 3
  // livetime
  cout << "\n\nLivetime\n--------" << endl << endl;
D 9
  live_trig=(double)trgs_int/trgs_tot;
E 9
I 9
  live_trig=(double)trgs_l1accept/trgs_tot;
E 9
  live_clock=(double)trgs00[2][0]/trgs00[1][0];
I 15
  live_random=(double)trgs00[2][3]/trgs00[1][3];
E 15
  if(trgs00[1][1]>0) live_fcup=(double)trgs00[2][1]/trgs00[1][1];
I 9
  live_or=(double)trgs_or_l1_live/trgs_tot;
E 9
D 5
  cout << "live trig is:          " << live_trig  << endl;
  cout << "live clock is:         " << live_clock << endl;
  cout << "live fcup is:          " << live_fcup << endl;
E 5
I 5
D 6
  cout << "live trig is:          " << live_trig  
E 6
I 6
  cout << "live trig is:         " << live_trig  
E 6
D 9
       << "   (" << trgs_int << "/" << trgs_tot << ")" << endl;
E 9
I 9
       << "   (" << trgs_l1accept << "/" << trgs_tot << ")" << endl;
E 9
D 6
  cout << "live clock is:         " << live_clock 
E 6
I 6
  cout << "live clock is:        " << live_clock 
E 6
       <<  "   (" << trgs00[2][0] << "/" << trgs00[1][0] << ")" << endl;
D 6
  cout << "live fcup is:          " << live_fcup 
E 6
I 6
  cout << "live fcup is:         " << live_fcup 
E 6
       <<  "   (" << trgs00[2][1]  << "/" << trgs00[1][1] << ")" << endl;
I 9
  cout << "live or is:           " << live_or
       <<  "   (" << trgs_or_l1_live  << "/" << trgs_tot << ")" << endl;
I 15
D 16
  cout << "live random is:           " << live_random
E 16
I 16
  cout << "live random is:       " << live_random
E 16
       <<  "   (" << trgs00[2][3]  << "/" << trgs00[1][3] << ")" << endl;
E 15
E 9
E 5


E 3
  cout << endl << endl;
  return;
}


//------------------------------------------------------------------



void decode_command_line(int argc, char **argv){

  int i=1;

  while(i<argc) {

    if(strcasecmp(argv[i],"-h")==0) {
      cout << "\n\n  scaler_file_analysis [-debug] run_number" << endl << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-",1)!=0) {
      break;
    }
  }


  // get run number to analyze
  run=atoi(argv[argc-1]);


  return;
}

//------------------------------------------------------------------
E 1
