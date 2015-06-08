//  scaler_file_analysis

//  analyzes scaler file data

//  E.Wolin, 5-oct-98


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


// system stuff
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


// program variables
static int run;
static int debug              = 0;
static char *scaler_file      = (char*)"/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
static int clock_rate         = 100000;
static char filename[256];
static char line[256];


// scaler file variables
static unsigned long rcst30[15];
static unsigned long s1st00[35];
static unsigned long trgs00[5][16];
static unsigned long trgd00[5][16];




// prototypes
void read_scaler_file(ifstream &file);
void analyze_scalers(void);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
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
  if(!scal.is_open()) {
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
    j=0; ind=0; for(i=16; i<24; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=24; i<32; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
    get_next_line(file,line,sizeof(line));
    j=0; ind=0; for(i=32; i<35; i++) {j+=ind; sscanf(&line[j],"%lu %n",&s1st00[i],&ind);}
  }


  // read trigger scalers
  if(find_tag_line(file,"*TRGS00*",line,sizeof(line))==0) {
    for(k=0; k<5; k++) {
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgs00[k][i]),&ind);}
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=8; i<16; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgs00[k][i]),&ind);}
    }
  }
  if(find_tag_line(file,"*TRGD00*",line,sizeof(line))==0) {
    for(k=0; k<5; k++) {
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=0; i<8; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgd00[k][i]),&ind);}
      get_next_line(file,line,sizeof(line));
      j=0; ind=0; for(i=8; i<16; i++) {j+=ind; sscanf(&line[j],"%lu %n",&(trgd00[k][i]),&ind);}
    }
  }  


}


//------------------------------------------------------------------


void analyze_scalers(void) {

  int i;
  int tbits_on,s1bits_on;

  unsigned long rc_tot  = rcst30[6];
  unsigned long rc_phys = rcst30[7];
  unsigned long rc_scal = rcst30[8];
  unsigned long rc_ill  = rcst30[12];
  unsigned long rc_psyn = rcst30[14];
  
  unsigned long s1_phys       = s1st00[12];
  unsigned long s1l1_zero     = s1st00[13];
  unsigned long s1l1_empty    = s1st00[14];
  unsigned long s1l1_notempty = s1st00[15];
  unsigned long s1l1_ok       = s1st00[16];
  unsigned long s1_sum        = 0;

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

  unsigned long trgs_tot       	 = trgs00[3][12];
  unsigned long trgs_or_l1_live	 = trgs00[2][2];
  unsigned long trgs_l1accept  	 = trgs00[3][13];
  unsigned long trgs_l2fail    	 = trgs00[4][0];
  unsigned long trgs_l2pass    	 = trgs00[4][1];
  unsigned long trgs_l2start   	 = trgs00[4][2];
  unsigned long trgs_l2clear   	 = trgs00[4][3];
  unsigned long trgs_l2accept  	 = trgs00[4][4];
  unsigned long trgs_l3accept  	 = trgs00[4][5];
  unsigned long trgs_sum = 0;

  double deltat                  = (double)trgs00[0][0]/clock_rate;
  double trgd_tot       	 = (double)trgs00[3][12]/deltat;
  double trgd_l1accept  	 = (double)trgs00[3][13]/deltat;
  double trgd_l2fail    	 = (double)trgs00[4][0]/deltat;
  double trgd_l2pass    	 = (double)trgs00[4][1]/deltat;
  double trgd_l2start   	 = (double)trgs00[4][2]/deltat;
  double trgd_l2clear   	 = (double)trgs00[4][3]/deltat;
  double trgd_l2accept  	 = (double)trgs00[4][4]/deltat;
  double trgd_l3accept  	 = (double)trgs00[4][5]/deltat;

  double live_clock   = 0.;
  double live_fcup    = 0.;
  double live_trig    = 0.;
  double live_or      = 0.;
  double live_random  = 0.;

  

  cout << "\n\nScaler file analysis for run " << run << endl;


  // dump rcst
  cout << "\n\nROC 30\n------" << endl << endl;
  cout << "total trigs is:       " << setw(8) << rc_tot << endl;
  cout << "illegal trig is:      " << setw(8) << rc_ill << endl;
  cout << "scaler events is:     " << setw(8) << rc_scal << endl;
  cout << "phys trig is:         " << setw(8) << rc_phys << endl;
  cout << "phys sync is:         " << setw(8) << rc_psyn << endl;
  cout << "phys + psyn is:       " << setw(8) << rc_phys+rc_psyn << endl;
  cout << "tot-scal-illegal is:  " << setw(8) << rc_tot-rc_scal-rc_ill << endl;
  if((rc_tot-rc_scal-rc_ill)!=(rc_phys+rc_psyn)) {
    cout << " *** ROC tot-scal-ill != phys+psync ***" << endl;
  }
  if((rc_phys+rc_psyn)!=s1_phys) {
    cout << " *** ROC phys+psync != s1 phys+psync ***" << endl;
  }
  

  // dump s1st
  s1bits_on=0;
  for(i=0; i<12; i++) {s1_sum+=s1st00[i]; s1bits_on+=(s1st00[i]!=0);}
  cout << "\n\nS1ST\n----" << endl << endl;
  cout << "s1 bits on is:        " << setw(8) << s1bits_on << endl;
  cout << "phys + psync is:      " << setw(8) << s1_phys << endl;
  cout << "l1 latch zero is:     " << setw(8) << s1l1_zero << "      ";
  cout << "l2 latch zero is:     " << setw(8) << s1l2_zero << endl;
  cout << "l1 latch empty is:    " << setw(8) << s1l1_empty << "      ";
  cout << "l2 latch empty is:    " << setw(8) << s1l2_empty << endl;
  cout << "l1 latch not empty:   " << setw(8) << s1l1_notempty << "      ";
  cout << "l2 latch not empty:   " << setw(8) << s1l2_notempty << endl;
  cout << "l1 latch ok is:       " << setw(8) << s1l1_ok << "      ";
  cout << "l2 latch ok is:       " << setw(8) << s1l2_ok << endl;
  cout << "s1 sum is:            " << setw(8) << s1_sum << "      ";
  cout << "l2 pass is:           " << setw(8) << s1l2_pass << endl;
  cout << "roc 13 is:            " << setw(8) << s113_count << "      ";
  cout << "l2 latefail is:       " << setw(8) << s1l2_latefail << endl;
  cout << "roc 15 is:            " << setw(8) << s115_count << endl;
  cout << "13 l1 zero is:        " << setw(8) << s113_l1_zero_count << "      ";
  cout << "13 l2 zero is:        " << setw(8) << s113_l2_zero_count << endl;
  cout << "l1l2 zero is:         " << setw(8) << s1l1l2_zero_count << endl;
  cout << "13 l1l2 zero is:      " << setw(8) << s113_l1l2_zero_count << endl;

  // s1st internal checks
  if(s1_phys!=(s1l1_ok+s1l1_zero+s1l1_empty)) {
    cout << " *** interrupts not equal ok+zero+empty ***" << endl;
  }
  if((s1bits_on==1)&&((s1_sum+s1l1_zero)!=s1_phys)) {
    cout << " *** s1 trigger bit sum+zero != s1 phys for single trigger ***" << endl;
  }
  if((s1l2_pass+s1l2_latefail)!=s1_phys) {
    cout << " *** s1 pass+latefail != s1 phys count ***" << endl;
  }
  if((s1bits_on==1)&&(s1_sum!=s1l1_ok)) {
    cout << " *** s1 trigger bit sum != s1 l1 ok for single trigger ***" << endl;
  }
  

  // dump trgs
  tbits_on=0;
  for(i=0; i<12; i++) {trgs_sum+=trgs00[3][i]; tbits_on+=(trgs00[3][i]!=0);}
  cout << "\n\nTRGS\n----" << endl << endl;
  cout << "bits on is:           " << setw(8) << tbits_on << endl;
  cout << "or_level1 is:         " << setw(8) << trgs_tot
       << "   (" << trgd_tot << ")" << endl;
  cout << "or_level1 live is:    " << setw(8) << trgs_or_l1_live << endl;
  cout << "l1 sum is:            " << setw(8) << trgs_sum << endl;
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

  // trgs internal checks
  if((trgs_sum!=trgs_tot)&&(tbits_on==1)) {
    cout << " *** sum of l1 bits != or_level1 for single trigger *** " << endl;
  }
  if((trgs_l2fail+trgs_l2pass)!=trgs_l2start) {
    cout << " *** trgs l2pass+l2fail != trgs l2start *** " << endl;
  }


  // compare trgs to s1st
  if(trgs_l2fail!=trgs_l2clear+s1l2_latefail) {
    cout << " *** trgs l2fail != (l2clear+latefail) *** " << endl;
  }
  if((trgs_l2pass+s1l2_latefail)!=s1_phys) {
    cout << " *** trgs (l2pass+latefail) doesn't agree with s1 interrupts *** " << endl;
  }
  if((trgs_l2pass+s1l2_latefail)!=trgs_l2accept) {
    cout << " *** trgs (l2pass+latefail) doesn't agree with l2 accepts *** " << endl;
  }
  if(trgs_l2accept!=trgs_l3accept) {
    cout << " *** trgs l2 accept doesn't agree with l3 accepts *** " << endl;
  }
  if(tbits_on!=s1bits_on) {
    cout << " *** trgs bits on != s1 bits on *** " << endl;
  }


  // livetime
  cout << "\n\nLivetime\n--------" << endl << endl;
  live_trig=(double)trgs_l1accept/trgs_tot;
  live_clock=(double)trgs00[2][0]/trgs00[1][0];
  live_random=(double)trgs00[2][3]/trgs00[1][3];
  if(trgs00[1][1]>0) live_fcup=(double)trgs00[2][1]/trgs00[1][1];
  live_or=(double)trgs_or_l1_live/trgs_tot;
  cout << "live trig is:         " << live_trig  
       << "   (" << trgs_l1accept << "/" << trgs_tot << ")" << endl;
  cout << "live clock is:        " << live_clock 
       <<  "   (" << trgs00[2][0] << "/" << trgs00[1][0] << ")" << endl;
  cout << "live fcup is:         " << live_fcup 
       <<  "   (" << trgs00[2][1]  << "/" << trgs00[1][1] << ")" << endl;
  cout << "live or is:           " << live_or
       <<  "   (" << trgs_or_l1_live  << "/" << trgs_tot << ")" << endl;
  cout << "live random is:       " << live_random
       <<  "   (" << trgs00[2][3]  << "/" << trgs00[1][3] << ")" << endl;


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
