//  begin_run_event
//
//   Inserts parameter events into special bos/fpack parameter file
//
//  still to do:
//    RUNPARMS
//
//  ejw, 23-apr-98
// sergey 29-oct-2007 migrate from cdev to ca


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// temporary array sizes
#define MAXPRETRIG    1000  
#define MAXHV        10000
#define MAXDISCR      5000
#define MAXL1        50000
#define MAXTRIG      50000
#define MAXPED       50000
#define MAXMLU       50000


// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdarg.h>
#include <unistd.h>

#include "epicsutil.h"

// for bosio
#define NBCS 100000
extern "C" {
#include "bosio.h"
#include "bos.h"
#include "bcs.h"
}


// misc 
static char *msql_database  = (char*)"clasrun";
static char *session        = (char*)"clastest";
static char *clon_parms     = getenv("CLON_PARMS");
static char *file_loc       = (char*)"/raid/stage_in";
static char *trig_name      = (char*)"/trigger/current_trig_config.cfg";
static int debug            = 0;
static int run;
static BOSIO *descriptor;
static char buffer[512];
static char filename[512];
static char temp[4096];
static char temp2[128];
static char temp3[128];


// for epics channels
struct epics_struct {
  const char *name;
  const char *chan;
  const char *get;
};

static char *epics_name[] = {
  "Hall A energy",
  "Hall B energy",
  "Hall C energy",
  "Hall A beam current",
  "Hall B beam current",
  "Hall C beam current",
  "A Laser Duty Factor",
  "B Laser Duty Factor",
  "C Laser Duty Factor",
  //  "Hall A status",
  //  "Hall B status",
  //  "Hall C status",
  "A slit position",
  "B slit position",
  "C slit position",
  "A polarized gun",
  "B polarized gun",
  "C polarized gun",
  "e2_target_a_status",
  "e2_target_b_status",
  "e2_target_c_status",
  "thermionic gun",
  "beam phase", 
  "torus current",
  "mini current setpoint",
  "mini current readback",
  "mini voltage",
  "tagger current setpoint",
  "tagger current readback",
  "tagger voltage",
  "Hall B Helicity",
  "Harp",
  "Scaler clock",
  "Faraday cup",
  "Halo U upstream",
  "Halo D upstream",
  "Halo L upstream",
  "Halo R upstream",
  "Halo U downstream",
  "Halo D downstream",
  "Halo L downstream",
  "Halo R downstream",
  "CC O2 monitor lamp",
  "CC O2 monitor gas",
  "Tagger turbo speed (%)",
  "Moeller vacuum (V)",
  "Upstream beam vacuum (V)",
  "Target vacuum (V)",
  "Faraday cup ion pump (KV)",
  "Faraday cup ion pump (ma)",
  "bpm_1_x",
  "bpm_1_y",
  "bpm_1_i", 
  "bpm_1_x_gain",
  "bpm_1_y_gain",
  "bpm_1_i_gain",
  "bpm_1_x_scale",
  "bpm_1_y_scale",
  "bpm_1_i_scale",
  "bpm_2_x",
  "bpm_2_y",
  "bpm_2_i",
  "bpm_2_i_gain",
  "bpm_2_x_gain",
  "bpm_2_y_gain",
  "bpm_2_x_scale",
  "bpm_2_y_scale",
  "bpm_2_i_scale",
  "bpm_3_x",
  "bpm_3_y",
  "bpm_3_i", 
  "bpm_3_x_gain",
  "bpm_3_y_gain",
  "bpm_3_i_gain",
  "bpm_3_x_scale",
  "bpm_3_y_scale",
  "bpm_3_i_scale",
  "Moller Quad1 current",
  "Moller Quad2 current",
  "Helmholtz Coils current",
  "Moller target position",
  "Raster set energy",
  "Raster set pattern X",
  "Raster set pattern Y",
  "Raster X offset",
  "Raster Y offset",   
  "cryo target pressure",
  "cryo target temperature",
  "cryo target status",
  "torus he buf p (atm)",
  "torus he tank p (atm)",
  "torus cold he t (kelvin)",
  "torus ln2 tank pres (atm)",
  "torus ln2 tank level (%)",
  "torus he tank level (%)",
  "torus svc module vac (-log torr)",
  "cryo target heat exch pressure",
  "cryo target cryostat pressure",
  "cryo target He3 tank pressure",
  "cryo target target pressure",
  "cryo target beam screen temperature",
  "cryo target 20k heater temperature",
  "cryo target 300k heater temperature",
  "cryo target D2 tank pressure",
  "cryo target H2 tank pressure",
  "cryo target He cryostat level",
  "cryo target primary target vac",
  "cryo target primary chamber vac",
  "cryo target secondary chamber vac",
  "cryo target joule-thompson exch",
  "cryo target temperature",
  "cryo target heat exch temperature",
  "cryo target heat exch exhaust temperature",
  "cryo target level low",
  "cryo target level high",
  "cryo target level low display",
  "cryo target level high display",
  "cryo target level high by PT_T",
  "cryo target level low by PT_T",
};

static char *epics_chan[] = {
  "MBSY1C_energy",
  "MBSY2C_energy",
  "MBSY3C_energy",
  "IBC1H03AAVG",
  "IBC2C24AVG",
  "IBC3H00AVG",
  "IGL1I00HALLADF",
  "IGL1I00HALLBDF",
  "IGL1I00HALLCDF",
  //  "PLC_HLA",
  //  "PLC_HLB",
  //  "PLC_HLC",
  "SMRPOSA",
  "SMRPOSB",
  "SMRPOSC",
  "IGL1I00DAC0",
  "IGL1I00DAC2",
  "IGL1I00DAC4",
  "e2_target_a_status",
  "e2_target_b_status",
  "e2_target_c_status",
  "IGT0I00BIASET", 
  "IGL1I00OD16_16.RVAL",
  "torus_current",
  "MTSETI",
  "MTIRBCK",
  "MTVRBCK",
  "TMSETI",
  "TMIRBCK",
  "TMVRBCK",
  "hallb:helicity",
  "harp.DRBV",
  "scalerS1o", 
  "scaler_calc1",
  "scalerS2o",
  "scalerS3o",
  "scalerS4o",
  "scalerS5o",
  "scalerS6o",
  "scalerS7o",
  "scalerS8o",
  "scalerS9o",
  "cc_o2_ref",
  "cc_o2_sig",
  "VMP2C24SPD",
  "VCG2C21",
  "VCG2C24",
  "VCG2H01",
  "VIP2H01DV",
  "VIP2H01DI",
  "IPM2H01.XPOS",
  "IPM2H01.YPOS",
  "IPM2H01",
  "IPM2H01.XKMF",
  "IPM2H01.YKMF",
  "IPM2H01.IKMF",
  "IPM2H01XSENrbstr.STR1",
  "IPM2H01YSENrbstr.STR1",
  "IBC2H01ISENrbstr.STR1",
  "IPM2C24A.XPOS",
  "IPM2C24A.YPOS",
  "IPM2C24A",
  "IPM2C24A.XKMF",
  "IPM2C24A.YKMF",
  "IPM2C24A.IKMF",
  "IPM2C24AXSENrbstr.STR1",
  "IPM2C24AYSENrbstr.STR1",
  "IBC2C24AISENrbstr.STR1",
  "IPM2C21A.XPOS",
  "IPM2C21A.YPOS",
  "IPM2C21A",
  "IPM2C21A.XKMF",
  "IPM2C21A.YKMF",
  "IPM2C21A.IKMF",
  "IPM2C21AXSENrbstr.STR1",
  "IPM2C21AYSENrbstr.STR1",
  "IBC2C21AISENrbstr.STR1",
  "hallb_sf_xy560_0_14",
  "hallb_sf_xy560_0_18",
  "hallb_sf_xy560_0_19",
  "moeller_target.RBV",
  "RASTSETENERGY",
  "RASTSETPATTERNX",
  "RASTSETPATTERNY",
  "RASTSETXOFFSET",
  "RASTSETYOFFSET", 
  "B_cryotarget_pressure",
  "B_cryotarget_temperature",
  "B_cryotarget_status",
  "hallb_sf_xy560_0_6",
  "hallb_sf_xy560_0_7",
  "hallb_sf_xy560_0_8",
  "hallb_sf_xy560_0_9",
  "hallb_sf_xy560_0_10",
  "hallb_sf_xy560_0_11",
  "hallb_sf_xy560_0_12",
  "B_cryotarget_PT_2KH",
  "B_cryotarget_PT_5KH",
  "B_cryotarget_PT_HES",
  "B_cryotarget_PT_T",
  "B_cryotarget_TT_EBP",
  "B_cryotarget_TT_E1",
  "B_cryotarget_TT_ECR",
  "B_cryotarget_PT_D2S",
  "B_cryotarget_PT_H2S",
  "B_cryotarget_LT_CR",
  "B_cryotarget_PT_VT",
  "B_cryotarget_PT_CVP",
  "B_cryotarget_PT_CVS",
  "B_cryotarget_JT_2K",
  "B_cryotarget_TT_T",
  "B_cryotarget_TT_ET",
  "B_cryotarget_TT_ER1",
  "B_cryotarget_LS_TB",
  "B_cryotarget_LS_TH",
  "B_cryotarget_NIV_C_BAS",
  "B_cryotarget_NIV_C_HAUT",
  "B_cryotarget_NIV_BAS",
  "B_cryotarget_NIV_HAUT",
};

static int ncallback = 0;
static float      epics_val[sizeof(epics_chan)/sizeof(char *)];
static int nepics  = sizeof(epics_chan)/sizeof(char *);


// constants for head bank
static int nvers  = 0;
static int type   = 100;
static int rocst  = 0;
static int evcls  = 0;
static int presc  = 0;


// prototypes
void decode_command_line(int argc, char **argv);
void add_tped_event();
void add_cped_event();
void add_spar_event();
void add_pretrig_event();
void add_discr_event();
void add_ts_event();
void add_trig_event();
void add_l1prog_event();
void add_brep_event();
void add_mlu_event();
void add_ttbl_event();
void create_tped1_bank(const char *fname, const char *bankname);
void create_tped2_bank(const char *fname, const char *bankname);
void create_tped6_bank(const char *fname, const char *bankname);
void create_tped7_bank(const char *fname, const char *bankname);
void create_cped_bank(const char *fname, const char *bankname, int banknum);
void create_spar_bank(const char *fname, const char *bankname, int banknum);
void create_pretrig_bank(int ncol, const char *bankname, int banknum, ifstream &file, const char *tag); 
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
void add_bank(const char *bankname, int banknum, const char *format, int ncol, int nrow, int nword, int data[]);
void va_add_bank(const char *bankname, int banknum, const char *format, int ncol, int nrow, int nword, ...);
int str2int(const char *s);
extern "C" {
int get_run_number(const char *msql_database, const char *session);
}


// program start time
static time_t start=time(NULL);


//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int status;
  BOSIOptr ptr;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(session==NULL)session=(char*)"clasprod";


  // get run number
  run=get_run_number(msql_database,session);
  

  // initialize bos
  status=bosInit(bcs_.iw,NBCS);


  // open parms file
  sprintf(filename,"%s/parms_%s_%06d.dat",file_loc,session,run);
  status=bosOpen(filename,(char*)"w",&descriptor);
  int recl=64000;
  bosIoctl(descriptor,(char*)"recl",(void *)&recl);
  if(status!=0) {
    cerr << "?unable to open output file!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // add events to bos file
  add_tped_event();
  add_cped_event();
  add_spar_event();
  add_pretrig_event();
  add_discr_event();
  add_trig_event();
  add_l1prog_event();
  add_ts_event();
  add_brep_event();
  add_mlu_event();
  //   add_ttbl_event();


  // flush and close file
  bosWrite(descriptor,bcs_.iw,(char*)"0");
  bosClose(descriptor);


  // done
  exit(EXIT_SUCCESS);
}

  
//----------------------------------------------------------------


void
add_tped_event()
{
  int ecpe=0,ccpe=0,stpe=0,dpsp=0,scpe=0,ecp1=0,dpcp=0,dtcp=0;


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);
  

  // (I,I,F)
  sprintf(temp,"%s/pedman/Tfiles/ec.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    ecpe=atoi(strrchr(temp2,'_')+1);
    create_tped1_bank(temp,"ECPE");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }

  sprintf(temp,"%s/pedman/Tfiles/cc.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    ccpe=atoi(strrchr(temp2,'_')+1);
    create_tped1_bank(temp,"CCPE");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  sprintf(temp,"%s/pedman/Tfiles/st.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    stpe=atoi(strrchr(temp2,'_')+1);
    create_tped1_bank(temp,"STPE");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  sprintf(temp,"%s/pedman/Tfiles/ps.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    dpsp=atoi(strrchr(temp2,'_')+1);
    create_tped1_bank(temp,"DPSP");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }



  // (I,2(I,F))
  sprintf(temp,"%s/pedman/Tfiles/sc.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    scpe=atoi(strrchr(temp2,'_')+1);
    create_tped2_bank(temp,"SCPE");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  sprintf(temp,"%s/pedman/Tfiles/lac.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    ecp1=atoi(strrchr(temp2,'_')+1);
    create_tped2_bank(temp,"ECP1");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  // (I,6(I,F))
  sprintf(temp,"%s/pedman/Tfiles/pc.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    dpcp=atoi(strrchr(temp2,'_')+1);
    create_tped6_bank(temp,"DPCP");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  // (I,7(I,F))
  sprintf(temp,"%s/pedman/Tfiles/tac.tped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    dtcp=atoi(strrchr(temp2,'_')+1);
    create_tped7_bank(temp,"DTCP");
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }



  // add pedestal run index bank
  va_add_bank("RNPE",0,"(I,A)",2,8,16,
	      ecpe,str2int("ECPE"),
	      ccpe,str2int("CCPE"),
	      stpe,str2int("STPE"),
	      dpsp,str2int("DPSP"),
	      scpe,str2int("SCPE"),
	      ecp1,str2int("ECP1"),
	      dpcp,str2int("DPCP"),
	      dtcp,str2int("DTCP"));
  

  // write and drop banks
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  if(debug==1)cout << "Created tped event" << endl;

  return;
}


//----------------------------------------------------------------


void
create_tped1_bank(const char *fname, const char *bankname)
{
  int nentry,dum;
  int tpeds[500][3];
  char sectors[120];
  int sec;
  char csec[16];


  // create translated ped banks from data in file
  ifstream file(fname);
  if(!file.is_open()) {
    cerr << "?unable to open " << fname << endl;
    return;
  }
    

  // get list of sectors
  if((find_tag_line(file,"*SECTORS*",sectors,sizeof(sectors))!=0) ||
     (get_next_line(file,sectors,sizeof(sectors))!=0)) {
    cerr << "No sector tag for file,bank " << fname << ", " << bankname << endl;
    file.close();
    return;
  }
  

  // loop over each sector
  char *ptr=strtok(sectors," ");  // point to 1st token
  while (ptr!=NULL) {

    sec=atoi(ptr);

    // loop over data
    nentry=0;
    sprintf(csec,"*SECTOR%d*",sec);
    if(find_tag_line(file,csec,buffer,sizeof(buffer))!=0) {
      cerr << "No " << csec << " tag for file,bank " << fname << ", " << bankname << endl;
    } else {
      while (get_next_line(file,buffer,sizeof(buffer))==0) {
	nentry++;
	if(nentry>500) {
	  nentry--;
	  cerr << "?too many tpeds in " << fname << endl;
	  break;
	}
	sscanf(buffer,"%d %d %d %f",
	       &tpeds[nentry-1][0], &dum, &tpeds[nentry-1][1], &tpeds[nentry-1][2]);
      }
      
      // create bos bank
      add_bank(bankname,sec,"(I,I,F)",3,nentry,nentry*3,(int*)tpeds);
      
      if(debug!=0) {
	cout << bankname << " bank number " << sec << ", format I,I,F"
	     << ", ncol 3" << ", nrow " << nentry 
	     << ", buflen " << nentry*3 << endl;
      }      
    }
      
    // update ptr to point to next token
    ptr=strtok(NULL," ");
  }


  // close file
  file.close();


  // done
  return;
}


//----------------------------------------------------------------


void
create_tped2_bank(const char *fname, const char *bankname)
{
  int nentry,dum;
  int tpeds[500][5];
  char sectors[120];
  int sec;
  char csec[16];


  // create translated ped banks from data in file
  ifstream file(fname);
  if(!file.is_open()) {
    cerr << "?unable to open " << fname << endl;
    return;
  }
    

  // get list of sectors
  if((find_tag_line(file,"*SECTORS*",sectors,sizeof(sectors))!=0) ||
     (get_next_line(file,sectors,sizeof(sectors))!=0)) {
    cerr << "No sector tag for file,bank " << fname << ", " << bankname << endl;
    file.close();
    return;
  }
  

  // loop over each sector
  char *ptr=strtok(sectors," ");  // point to 1st token
  while (ptr!=NULL) {

    sec=atoi(ptr);

    // loop over data
    nentry=0;
    sprintf(csec,"*SECTOR%d*",sec);
    if(find_tag_line(file,csec,buffer,sizeof(buffer))!=0) {
      cerr << "No " << csec << " tag for file,bank " << fname << ", " << bankname;
    } else {
      while (get_next_line(file,buffer,sizeof(buffer))==0) {
	nentry++;
	if(nentry>500) {
	  nentry--;
	  cerr << "?too many tpeds in " << fname << endl;
	  break;
	}
	sscanf(buffer,"%d %d %d %f %d %f",
	       &tpeds[nentry-1][0], &dum, &tpeds[nentry-1][1], &tpeds[nentry-1][2],
	       &tpeds[nentry-1][3], &tpeds[nentry-1][4]);
      }
      
      // create bos bank
      add_bank(bankname,sec,"(I,2(I,F))",5,nentry,nentry*5,(int*)tpeds);
      
      if(debug!=0) {
	cout << bankname << " bank number " << sec << ", format I,2(I,F)"
	     << ", ncol 5" << ", nrow " << nentry 
	     << ", buflen " << nentry*5 << endl;
      }      
    }
      
    // update ptr to point to next token
    ptr=strtok(NULL," ");
  }


  // close file
  file.close();


  // done
  return;
}


//----------------------------------------------------------------


void
create_tped6_bank(const char *fname, const char *bankname)
{
  int count;
  int tpeds[13];
  char sectors[120];
  int sec;
  char csec[16];
  int id,dum,ped;
  float sigma;


  // create translated ped banks from data in file
  ifstream file(fname);
  if(!file.is_open()) {
    cerr << "?unable to open " << fname << endl;
    return;
  }
    

  // get list of sectors
  if((find_tag_line(file,"*SECTORS*",sectors,sizeof(sectors))!=0) ||
     (get_next_line(file,sectors,sizeof(sectors))!=0)) {
    cerr << "No sector tag for file,bank " << fname << ", " << bankname << endl;
    file.close();
    return;
  }
  

  // loop over each sector
  char *ptr=strtok(sectors," ");  // point to 1st token
  while (ptr!=NULL) {

    sec=atoi(ptr);

    // loop over data
    sprintf(csec,"*SECTOR%d*",sec);
    if(find_tag_line(file,csec,buffer,sizeof(buffer))!=0) {
      cerr << "No " << csec << " tag for file,bank " << fname << ", " << bankname;
    } else {
      for(count=0; count<6; count++) {
	if(get_next_line(file,buffer,sizeof(buffer))!=0) {
	  cerr << "Not enough data in " << fname << endl;
	  file.close();
	  return;
	}
	sscanf(buffer,"%d %d %d %f",&id,&dum,&ped,&sigma);
	tpeds[0]=id;
	tpeds[2*count+1]=ped;
	tpeds[2*count+2]=*(int*)&sigma;
      }
      
      
      // create bos bank
      add_bank(bankname,sec,"(I,6(I,F))",13,1,13,(int*)tpeds);
      
      if(debug!=0) {
	cout << bankname << " bank number " << sec << ", format I,6(I,F)"
	     << ", ncol 13" << ", nrow 1" << ", buflen 13" << endl;
      }      
    }

    // update ptr to point to next token
    ptr=strtok(NULL," ");
  }


  // close file
  file.close();


  // done
  return;
}


//----------------------------------------------------------------


void
create_tped7_bank(const char *fname, const char *bankname)
{
  int count;
  int tpeds[15];
  char sectors[120];
  int sec;
  char csec[16];
  int id,dum,ped;
  float sigma;


  // create translated ped banks from data in file
  ifstream file(fname);
  if(!file.is_open()) {
    cerr << "?unable to open " << fname << endl;
    return;
  }
    

  // get list of sectors
  if((find_tag_line(file,"*SECTORS*",sectors,sizeof(sectors))!=0) ||
     (get_next_line(file,sectors,sizeof(sectors))!=0)) {
    cerr << "No sector tag for file,bank " << fname << ", " << bankname << endl;
    file.close();
    return;
  }
  

  // loop over each sector
  char *ptr=strtok(sectors," ");  // point to 1st token
  while (ptr!=NULL) {

    sec=atoi(ptr);

    // loop over data
    sprintf(csec,"*SECTOR%d*",sec);
    if(find_tag_line(file,csec,buffer,sizeof(buffer))!=0) {
      cerr << "No " << csec << " tag for file,bank " << fname << ", " << bankname;
    } else {
      for(count=0; count<7; count++) {
	if(get_next_line(file,buffer,sizeof(buffer))!=0) {
	  cerr << "Not enough data in " << fname << endl;
	  file.close();
	  return;
	}
	sscanf(buffer,"%d %d %d %f",&id,&dum,&ped,&sigma);
	tpeds[0]=id;
	tpeds[2*count+1]=ped;
	tpeds[2*count+2]=*(int*)&sigma;
      }
      
      
      // create bos bank
      add_bank(bankname,sec,"(I,7(I,F))",15,1,15,(int*)tpeds);
      
      if(debug!=0) {
	cout << bankname << " bank number " << sec << ", format I,7(I,F)"
	     << ", ncol 15" << ", nrow 1" << ", buflen 15" << endl;
      }      
    }      

    // update ptr to point to next token
    ptr=strtok(NULL," ");
  }


  // close file
  file.close();


  // done
  return;
}


//----------------------------------------------------------------


void
add_cped_event()
{
  int cc=0,sc=0,ec1=0,ec2=0,lac=0;


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);
  

  sprintf(temp,"%s/pedman/archive/cc1.ped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    cc=atoi(strrchr(temp2,'_')+1);
    create_cped_bank(temp,"CPED",12);
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }

 
  sprintf(temp,"%s/pedman/archive/sc1.ped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    sc=atoi(strrchr(temp2,'_')+1);
    create_cped_bank(temp,"CPED",13);
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }

 
  sprintf(temp,"%s/pedman/archive/ec1.ped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    ec1=atoi(strrchr(temp2,'_')+1);
    create_cped_bank(temp,"CPED",14);
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }

 
  sprintf(temp,"%s/pedman/archive/ec2.ped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    ec2=atoi(strrchr(temp2,'_')+1);
    create_cped_bank(temp,"CPED",15);
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }

 
  sprintf(temp,"%s/pedman/archive/lac1.ped",clon_parms);
  if(readlink(temp,temp2,sizeof(temp2))>0) {
    lac=atoi(strrchr(temp2,'_')+1);
    create_cped_bank(temp,"CPED",16);
  } else {
    cerr << "Unable to translate link: " << temp << endl;
  }


  // add pedestal run index bank
  va_add_bank("RNPE",0,"(I,A)",2,5,10,
	       cc,str2int("CC  "),
	       sc,str2int("SC  "),
	      ec1,str2int("EC1 "),
	      ec2,str2int("EC2 "),
	      lac,str2int("LAC "));
  

  // write and drop banks
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  if(debug==1)cout << "Created cped event" << endl;

  return;
}


//----------------------------------------------------------------


void
create_cped_bank(const char *fname, const char *bankname, int banknum)
{
  int nentry=0;
  int cped[1000][5];


  // fill local array with cped data
  ifstream file(fname);
  while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad()) {
    file.getline(buffer,sizeof(buffer));
    if(file.eof()==0) {
      nentry++;
      if(nentry>1000) {
	nentry--;
	cerr << "?too many cpeds in " << fname << endl;
	break;
      }
      sscanf(buffer,"%d %d %d %f %d",
	     &cped[nentry-1][0], &cped[nentry-1][1], &cped[nentry-1][2],
	     &cped[nentry-1][3], &cped[nentry-1][4]);
    }
  }
  file.close();


  // create bos bank
  if(nentry>0) {
    add_bank(bankname,banknum,"(3I,F,I)",5,nentry,nentry*5,(int*)cped);
    if(debug!=0) {
      cout << bankname << " bank number " << banknum << ", format 3I,F,I"
	   << ", ncol 5" << ", nrow " << nentry 
	   << ", buflen " << nentry*5 << endl;
    }
  } else {
    cerr << "No cped data for fname,bank,num: " << fname << ", " << bankname << ", " << banknum << endl;
  }


  // done
  return;
}


//----------------------------------------------------------------


void
add_spar_event()
{
  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);

  
  sprintf(temp,"%s/pedman/spar_archive/cc1_%s_%06d_hard.spar",clon_parms,session,run);
  create_spar_bank(temp,"SPAR",12); 
 				    
  sprintf(temp,"%s/pedman/spar_archive/sc1_%s_%06d_hard.spar",clon_parms,session,run);
  create_spar_bank(temp,"SPAR",13); 
 				    
  sprintf(temp,"%s/pedman/spar_archive/ec1_%s_%06d_hard.spar",clon_parms,session,run);
  create_spar_bank(temp,"SPAR",14); 
 				    
  sprintf(temp,"%s/pedman/spar_archive/ec2_%s_%06d_hard.spar",clon_parms,session,run);
  create_spar_bank(temp,"SPAR",15); 
 				    
  sprintf(temp,"%s/pedman/spar_archive/lac1_%s_%06d_hard.spar",clon_parms,session,run);
  create_spar_bank(temp,"SPAR",16);
 

  // write and drop banks
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  if(debug==1)cout << "Created spar event" << endl;

  return;
}


//----------------------------------------------------------------


void
create_spar_bank(const char *fname, const char *bankname, int banknum)
{
  int nentry=0;
  int spar[1000][4];


  // fill local array with sparsification data
  ifstream file(fname);
  if(!file.is_open()) {
    cerr << "Unable to read spar file " << fname << endl;
    return;
  }
  while (get_next_line(file,buffer,sizeof(buffer))==0) {
    nentry++;
    if(nentry>1000) {
      nentry--;
      cerr << "?too many spars in " << fname << endl;
      break;
    }
    sscanf(buffer,"%d %d %d %d",
	   &spar[nentry-1][0], &spar[nentry-1][1], &spar[nentry-1][2], &spar[nentry-1][3]);
  }
  file.close();


  // create bos bank
  if(nentry>0) {
    add_bank(bankname,banknum,"4I",4,nentry,nentry*4,(int*)spar);
    if(debug!=0) {
      cout << bankname << " bank number " << banknum << ", format 4I"
	   << ", ncol 4" << ", nrow " << nentry 
	   << ", buflen " << nentry*4 << endl;
    }
  } else {
    cerr << "No spar data for fname,bank,num: " << fname << ", " << bankname << ", " << banknum << endl;
  }


  // done
  return;
}


//----------------------------------------------------------------


void
add_ttbl_event()
{
  int i;


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  return;
}


//----------------------------------------------------------------


void
add_pretrig_event()
{
  int i,buflen;
  int time;
  istrstream buf(buffer,sizeof(buffer));


  // open pretrig file
  sprintf(filename,"%s/pretrigger/archive/pretrig_%s_%06d.txt",clon_parms,session,run);
  ifstream file(filename);
  if(!file.is_open()) {
    cerr << "Unable to open pretrig file " << filename << endl;
    return;
  }
  file.setf(ios::skipws);


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);
    

  // get time and fill bank
  find_tag_line(file,"*TIME*",buffer,sizeof(buffer));
  get_next_line(file,buffer,sizeof(buffer));
  buf >> time;
  add_bank("PRTM",0,"I",1,1,1,(int *)&time);
  if(debug!=0) {
    cout << "Pretrig bank PRTM, number 0, format I"
	 << ", ncol 3" << ", nrow 1, buflen 1" << endl;
  }
  

  // get ec thresholds and fill bank
  create_pretrig_bank(6,"ECPT",0,file,"*EC_L1_T*");


  // get ec masks and fill bank
  create_pretrig_bank(6,"ECPM",0,file,"*EC_L1_M*");
  

  // get ec mean thresholds
  create_pretrig_bank(6,"ECMT",0,file,"*EC_Mean*");
  

  // get cc thresholds
  create_pretrig_bank(4,"CCPT",0,file,"*CC_L1_T*");


  // get cc means
  create_pretrig_bank(2,"CCMT",0,file,"*CC_Mean*");

					       
  // get sc widths,thresholds,masks and fill bank
  create_pretrig_bank(3,"SCPT",0,file,"*SC_L1_T*");
					       
					       
  // get sc mean threshold
  create_pretrig_bank(1,"SCMT",0,file,"*SC_MeanThr*");
					       

  // get sc mean width
  create_pretrig_bank(1,"SCMW",0,file,"*SC_MeanWid*");
					       
					       
  // close file
  file.close();
    
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  return;
}


//----------------------------------------------------------------


void
create_pretrig_bank(int ncol, const char *bankname, int num, ifstream &file, const char *tag)
{
  istrstream buf(buffer,sizeof(buffer));
  int npretrig=0;
  int *pretrig  = new int[MAXPRETRIG*ncol];
  int j;
  char fmt[10];

  find_tag_line(file,tag,buffer,sizeof(buffer));
  while((npretrig<MAXPRETRIG)&&get_next_line(file,buffer,sizeof(buffer))==0) {
    npretrig++;
    buf.seekg(0);
    for (j=0; j<ncol; j++) {  buf >> pretrig[(npretrig-1)*ncol+j]; }
  }


  sprintf(fmt,"%dI",ncol);
  add_bank(bankname,num,fmt,ncol,npretrig,ncol*npretrig,pretrig);
  if(debug!=0) {
    cout << "Pretrig bank " << bankname << ", number " << num << ", format " << fmt 
	 << ", ncol " << ncol << ", nrow " << npretrig
	 << ", buflen " << ncol*npretrig << endl;
  }

  delete []pretrig;
  return;
}


//----------------------------------------------------------------


void
add_discr_event()
{
  int i,j,buflen;
  int time,ndisc;
  istrstream buf(buffer,sizeof(buffer));

  int discr[MAXDISCR][5];
  int lacdiscr[MAXDISCR][4];


  // open discr file
  sprintf(filename,"%s/discr/archive/discr_%s_%06d.txt",clon_parms,session,run);
  ifstream file(filename);
  if(!file.is_open()) {
    cerr << "Unable to open discr file " << filename << endl;
    return;
  }
  file.setf(ios::skipws);


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  // get time and fill bank
  if(find_tag_line(file,"*TIME*",buffer,sizeof(buffer))==0) {
    get_next_line(file,buffer,sizeof(buffer));
    buf >> time;
    add_bank("DITM",0,"I",1,1,1,(int *)&time);
    if(debug!=0) {
      cout << "Discr bank DITM, number 1, format I"
	   << ", ncol 1, nrow 1, buflen 1" << endl;
    }
  } else {
    cout << "Unable to find *TIME* in " << filename << endl;
  }
  


  // get ec channel discriminators and fill bank
  ndisc=0;
  if(find_tag_line(file,"*EC*",buffer,sizeof(buffer))==0) {
    while((ndisc<MAXDISCR)&&(get_next_line(file,buffer,sizeof(buffer))==0)) {
      ndisc++;
      buf.seekg(0);
      for (j=0; j<5; j++) {  buf >> discr[ndisc-1][j]; }
    }
    if(ndisc>0)add_bank("ECDI",0,"5I",5,ndisc,5*ndisc,(int *)&discr[0][0]);
    if(debug!=0) {
      cout << "Discr bank ECDI, number 0, format 5I"
	   << ", ncol 5" << ", nrow " << ndisc
	   << ", buflen " << 5*ndisc << endl;
    }
  } else {
    cout << "Unable to find *EC* in " << filename << endl;
  }



  // get cc channel discriminators and fill bank
  ndisc=0;
  if(find_tag_line(file,"*CC*",buffer,sizeof(buffer))==0) {
    while((ndisc<MAXDISCR)&&get_next_line(file,buffer,sizeof(buffer))==0) {
      ndisc++;
      buf.seekg(0);
      for (j=0; j<5; j++) {  buf >> discr[ndisc-1][j]; }
    }
    if(ndisc>0)add_bank("CCDI",0,"5I",5,ndisc,5*ndisc,(int *)&discr[0][0]);
    if(debug!=0) {
      cout << "Discr bank CCDI, number 0, format 5I"
	   << ", ncol 5" << ", nrow " << ndisc
	   << ", buflen " << 5*ndisc << endl;
    }
  } else {
    cout << "Unable to find *CC* in " << filename << endl;
  }



  // get sc channel discriminators and fill bank
  ndisc=0;
  if(find_tag_line(file,"*SC*",buffer,sizeof(buffer))==0) {
    while((ndisc<MAXDISCR)&&get_next_line(file,buffer,sizeof(buffer))==0) {
      ndisc++;
      buf.seekg(0);
      for (j=0; j<5; j++) {  buf >> discr[ndisc-1][j]; }
    }
    if(ndisc>0)add_bank("SCDI",0,"5I",5,ndisc,5*ndisc,(int *)&discr[0][0]);
    if(debug!=0) {
      cout << "Discr bank SCDI, number 0, format 5I"
	   << ", ncol 5" << ", nrow " << ndisc
	   << ", buflen " << 5*ndisc << endl;
    }
  } else {
    cout << "Unable to find *SC* in " << filename << endl;
  }



  // get lac channel discriminators and fill bank
  ndisc=0;
  if(find_tag_line(file,"*LAC_T*",buffer,sizeof(buffer))==0) {
    while((ndisc<MAXDISCR)&&get_next_line(file,buffer,sizeof(buffer))==0) {
      ndisc++;
      buf.seekg(0);
      for (j=0; j<4; j++) {  buf >> lacdiscr[ndisc-1][j]; }
    }
    if(ndisc>0)add_bank("LCDI",0,"4I",4,ndisc,4*ndisc,(int *)&lacdiscr[0][0]);
    if(debug!=0) {
      cout << "Discr bank LCDI, number 0, format 4I"
	   << ", ncol 4" << ", nrow " << ndisc
	   << ", buflen " << 4*ndisc << endl;
    }
  } else {
    cout << "Unable to find *LAC_T* in " << filename << endl;
  }


  // done
  file.close();
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  return;
}


//----------------------------------------------------------------


void
add_ts_event()
{
  int stat1,stat2;
  int i,j,buflen;
  int tsreg[128],tsprog[4096];
  istrstream buf(buffer,sizeof(buffer));


  // open ts file
  sprintf(filename,"%s/ts/current_ts_program.txt",clon_parms);
  ifstream file(filename);
  if(!file.is_open()) {
    cerr << "?unable to open ts program file" << endl;
    return;
  }
    

  // check if program read properly
  stat1=find_tag_line(file,"*TSREG*",buffer,sizeof(buffer));
  stat2=find_tag_line(file,"*TSPROG*",buffer,sizeof(buffer));
  if((stat1!=0)||(stat2!=0)) {
    cerr << "?unable to read ts" << endl;
    file.close();
    return;
  }


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  // read ts registers
  find_tag_line(file,"*TSREG*",buffer,sizeof(buffer));
  for(i=0; i<16; i++) {
    get_next_line(file,buffer,sizeof(buffer));
    buf.seekg(0);
    for (j=0; j<8; j++) buf >> hex >> tsreg[8*i+j];
  }

  // read ts program
  find_tag_line(file,"*TSPROG*",buffer,sizeof(buffer));
  for(i=0; i<512; i++) {
    get_next_line(file,buffer,sizeof(buffer));
    buf.seekg(0);
    for (j=0; j<8; j++) buf >> hex >> tsprog[8*i+j];
  }
  file.close();


  // add ts register bank
  add_bank("TSRG",0,"B32",128,1,128,(int *)&tsreg[0]);
  if(debug!=0) {
    cout << "TS bank TSRG, number 0, format B32"
	 << ", ncol 128, nrow 1, buflen 128" << endl;
  }


  // add ts program bank
  add_bank("TSPR",0,"B32",4096,1,4096,(int *)&tsprog[0]);
  if(debug!=0) {
    cout << "TS bank TSPR, number 0, format B32"
	 << ", ncol 4096, nrow 1, buflen 4096" << endl;
  }
    
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  return;
}


//----------------------------------------------------------------


void
add_trig_event()
{
  int i,buflen;
  char trig_buf[256];
  char trig_prog[MAXTRIG];


  // read trig program into local array
  sprintf(temp,"%s/%s",clon_parms,trig_name);
  trig_prog[0] = (char)NULL;
  ifstream trigprog(temp);
  if(!trigprog.is_open()) {
    cerr << "Unable to read trig program file " << trig_name << endl;
    return;
  }

  while(trigprog.is_open()&&trigprog.good()&&!trigprog.eof()&&!trigprog.fail()&&!trigprog.bad()) {
    trigprog.getline(trig_buf,sizeof(trig_buf));
    strcat(trig_prog,trig_buf);
    strcat(trig_prog,"\n");
  }
  trigprog.close();


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  // add trig program bank
  buflen=(strlen(trig_prog)+1)/4;
  add_bank("TRCF",0,"A",buflen,1,buflen,(int *)&trig_prog[0]);
  if(debug!=0) {
    cout << "TRIG bank TRCF number 0, format A, ncol " << buflen << ", nrow 1"
	 << ", buflen " << buflen << endl;
  }


  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  return;
}


//----------------------------------------------------------------


void
add_l1prog_event()
{
  int i,buflen;
  char l1_name[512];
  char l1_buf[256];
  char l1_prog[MAXL1];


  // get l1 program file name
  sprintf(temp,"%s/ts/Current.Config",clon_parms);
  ifstream l1name(temp);
  if(!l1name.is_open()) {
    cerr << "Unable to read l1 name file " << temp << endl;
    return;
  }
  l1name.getline(l1_name,sizeof(l1_name));
  l1name.close();


  // read l1 program into local array
  l1_prog[0] = (char)NULL;
  ifstream l1prog(l1_name);
  if(!l1prog.is_open()) {
    cerr << "Unable to read l1 program file " << l1_name << endl;
    return;
  }
  while(l1prog.is_open()&&l1prog.good()&&!l1prog.eof()&&!l1prog.fail()&&!l1prog.bad()) {
    l1prog.getline(l1_buf,sizeof(l1_buf));
    strcat(l1_prog,l1_buf);
    strcat(l1_prog,"\n");
  }
  l1prog.close();



  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  // add l1 program bank
  buflen=(strlen(l1_prog)+1)/4;
  add_bank("L1PG",0,"A",buflen,1,buflen,(int *)&l1_prog[0]);
  if(debug!=0) {
    cout << "L1 bank L1PG number 0, format A, ncol " << buflen << ", nrow 1"
	 << ", buflen " << buflen << endl;
  }


  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);


  return;
}


//----------------------------------------------------------------


void
add_brep_event()
{
  int i, result;


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  for(i=0; i<nepics; i++)
  {
    epics_val[i]=-9999.0;
  }
  result = getepics(nepics, epics_chan, epics_val);
  
  /*
  // check if all callbacks received
  if(ncallback<nepics) {
    cerr << "?only received " << ncallback << " callbacks out of " 
	 << nepics << " expected" << endl;
  }    
  */
  
  // create bos bank
  add_bank("BREP",2,"F",nepics,1,nepics,(int *)&epics_val[0]);
  if(debug!=0) {
    cout << "bank BREP, number 0, format F, ncol " << nepics << ", nrow 1, buflen " << nepics << endl;
  }


  // write bank
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);
  return;
}


//---------------------------------------------------------------------------


void
add_mlu_event()
{
  int buflen;


  // open mlu file
  sprintf(temp,"%s/euphrates/current.trg",clon_parms);
  ifstream mlu(temp);
  if(!mlu.is_open()) {
    cerr << "Unable to read mlu file " << temp << endl;
    return;
  }


  // create and fill text string from file
  char *mlu_prog=new char[MAXMLU];
  mlu_prog[0] = (char)NULL;
  while (mlu.eof()==0) {
    mlu.getline(temp,sizeof(temp));
    strcat(mlu_prog,temp);
    strcat(mlu_prog,"\n");
  }
  mlu.close();


  // head bank
  va_add_bank("HEAD",0,"I",8,1,8,nvers,run,0,start,type,rocst,evcls,presc);


  buflen=(strlen(mlu_prog)+1)/4;
  add_bank("MLU ",0,"A",buflen,1,buflen,(int *)&mlu_prog[0]);
  if(debug!=0) {
    cout << "MLU bank MLU  number 0, format A, ncol " << buflen << ", nrow 1"
	 << ", buflen " << buflen << endl;
  }

  // write bank
  bosWrite(descriptor,bcs_.iw,(char*)"E");
  bosLdrop(bcs_.iw,(char*)"E");
  bosNgarbage(bcs_.iw);

  delete mlu_prog;

  return;
}


//----------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  begin_run_event [-m msql_database] [-s session] [-f file_loc] [-debug]\n\n\n";


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0) {
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-f",2)==0) {
      file_loc=strdup(argv[i+1]);
      i=i+2;
    }
    else{
      i=i+1;
    }
  }
}


//---------------------------------------------------------------------


void
va_add_bank(const char *name, int num, const char *format, int ncol, int nrow, int ndata, ...)
{
  int i,ind;
  va_list ap;


  strcpy(temp,"E+");
  strcpy(temp2,name);
  strcpy(temp3,format);

  bosLctl(bcs_.iw,temp,temp2);
  bosNformat(bcs_.iw,temp2,temp3);
  ind=bosNcreate(bcs_.iw,temp2,num,ncol,nrow);

  va_start(ap,ndata);
  for(i=0; i<ndata; i++) bcs_.iw[ind+i]=va_arg(ap,int);

  /*sergey: do you mean 'ap' ??
  va_end(arg);*/
  va_end(ap);

  return;
}


//----------------------------------------------------------------


void
add_bank(const char *name, int num, const char *format, int ncol, int nrow, int ndata, int data[])
{
  int i,ind;

  strcpy(temp,"E+");
  strcpy(temp2,name);
  strcpy(temp3,format);

  bosLctl(bcs_.iw,temp,temp2);
  bosNformat(bcs_.iw,temp2,temp3);
  ind=bosNcreate(bcs_.iw,temp2,num,ncol,nrow);

  for(i=0; i<ndata; i++) bcs_.iw[ind+i]=data[i];

  return;
}


//----------------------------------------------------------------


int
str2int(const char *s)
{
  int i,val=0;

  for(i=0; i<4; i++) {
    val|=s[i]<<((3-i)*8);
  }

  return(val);
}


//----------------------------------------------------------------


