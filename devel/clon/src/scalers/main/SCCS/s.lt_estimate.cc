h22684
s 00001/00001/00258
d D 1.5 01/01/03 10:51:08 wolin 6 5
c is_open()
e
s 00002/00002/00257
d D 1.4 00/10/26 16:51:24 wolin 5 4
c const
e
s 00003/00003/00256
d D 1.3 00/08/10 16:39:17 gurjyan 4 3
c *** empty log message ***
e
s 00000/00000/00259
d D 1.2 00/04/25 13:46:30 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/lt_estimate.cc
e
s 00259/00000/00000
d D 1.1 00/04/25 13:44:51 clasrun 1 0
c 
e
u
U
f b 
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
#include <iomanip.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


// program variables
static int run, conversion, helicity;
static int debug              = 0;
D 5
static char *scaler_file      = "/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
E 5
I 5
static char *scaler_file      = (char*)"/usr/local/clas/parms/scalers/archive/scalers_clasprod_%06d.txt";
E 5
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
D 5
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen);
E 5
I 5
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
E 5
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
D 6
  if(scal.bad()) {
E 6
I 6
  if(!scal.is_open()) {
E 6
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

  double deltat                  = (double)trgs00[0][0]/10000;
  double trgd_tot       	 = (double)trgs00[3][12]/deltat;
  double trgd_l1accept  	 = (double)trgs00[3][13]/deltat;
  double trgd_l2fail    	 = (double)trgs00[4][0]/deltat;
  double trgd_l2pass    	 = (double)trgs00[4][1]/deltat;
  double trgd_l2start   	 = (double)trgs00[4][2]/deltat;
  double trgd_l2clear   	 = (double)trgs00[4][3]/deltat;
  double trgd_l2accept  	 = (double)trgs00[4][4]/deltat;
  double trgd_l3accept  	 = (double)trgs00[4][5]/deltat;

  double live_clock = 0.;
  double live_fcup  = 0.;
  double live_trig  = 0.;
  double live_or    = 0.;

double est_lt = 100.0 - (((trgd_l1accept * conversion)/10000.0) + ((trgd_l2clear * 7)/10000.0) + ((helicity * 500)/10000.0) );  
D 4
  cout << "\nLive time for run " << run << endl;
E 4
I 4
 cout << "\nLive time for run " << run << endl << endl;
E 4

D 4
  cout << "Input trigger rate is      " << trgd_tot << " Hz" << endl;   
E 4
I 4
  cout << "Input trigger rate is  " << trgd_tot << " Hz" << endl;   
E 4
  cout << "Accepted event rate is " << trgd_l1accept << " Hz" << endl;   
  cout << "Rejected event rate is " << trgd_l2clear << " Hz" << endl << endl;   

if ( trgd_tot >=3500 )
  cout << "DAQ processes can't be explained by the theoretical pipeline model" << endl << endl; 

  // dump rcst
  // livetime
  cout << "-------------------------" << endl << endl;
  live_trig=(double)trgs_l1accept/trgs_tot;
  live_clock=(double)trgs00[2][0]/trgs00[1][0];
  if(trgs00[1][1]>0) live_fcup=(double)trgs00[2][1]/trgs00[1][1];
  live_or=(double)trgs_or_l1_live/trgs_tot;
  cout << "live trig is:               " << live_trig * 100  << endl;
  cout << "live clock is:              " << live_clock * 100 << endl;
  cout << "live fcup is:               " << live_fcup * 100 << endl;
  cout << "Estimated live-time is:     " << est_lt << endl;         
  cout << endl << endl;
  return;
}


//------------------------------------------------------------------



void decode_command_line(int argc, char **argv){

  int i=1;

  while(i<argc) {

    if(strcasecmp(argv[i],"-h")==0) {
      cout << "\n\n  lt_estimate [-debug] run_number (conversion time in micro sec.) ( helicity veto rate in Hz)" << endl << endl;
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
  run=atoi(argv[1]);
  conversion = atoi(argv[2]);
  helicity = atoi(argv[3]);
D 4
  cout << "\n For livetime estimation following parameters were used: " << endl;
E 4
I 4
  cout << "\nFor livetime estimation following parameters were used: " << endl;
E 4
  cout << "FE conversion time =  " << conversion << " micro sec."<< endl;
  cout << "Helicity veto rate =  " << helicity << " Hz"<< endl;

  return;
}

//------------------------------------------------------------------
E 1
