/*
  ----------------------------------------------------
  -							
  -  File:    event.c	
  -							
  -  Summary:						
  -           controls bos events
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ----------------------------------------------------
  -  10/19/94	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
  */

#include "ced.h"
#include <unistd.h>

#ifndef NO_ET
#include "et.h"
#endif

/*
  #include "bos.h"
  #include "bosio.h"
*/


extern void bdrop_();
extern void fparm_();
extern void fseqr_();
extern void fwbos_();
extern void frbos_();
extern void bos_();
extern int  mlink_();
extern void bgarb_();
extern void fermes_();


/*------ local prototypes ----------*/

static void initAccumWrite();

static void cleanupAccumWrite();

static Boolean writeAccEvents();

static Boolean accPhysicsOnly();

static void EventPrintName();

static int fparm_c(char *mess);

static void NextMatchingEvent();

static Boolean ReadNextEventFromFile();

static void ConvertCB();

static void InitBitData();

static void ZeroBitData();

static void SetBitData();

static void EventTimeOut(void);

static void AccumulateTimerCB(Hv_TimerDialogData tdlogdata);

static void ZeroCB(Hv_Widget w);

static void BookEvent();

static void AccumulateCB(Hv_Widget w);

static void bosDropBanks();

static void ReadyEvent();

static void CloseEventControlDialog(Hv_Widget w);

static void GotoEventNumber(int num);

static void ScaleCB(Hv_Widget   w);

static void BankCB(Hv_Widget   w);


static void bosZeroBankIndices(void);

static Boolean SomeSectorHasData(int *Idx);

static void bosOpen(char  *dataname, int unitnum, char *filename, int option);

static void  bosClose(char  *dataname );

static void  bosRewind(char *dataname );

static int  bosCountEvents(char *dataname,
			   int   option);

static void  LabelSensitivity(Hv_Widget   label,
			      Boolean    sens);

static void UpdateOutputFileLabel();


/*------ global variables ----------*/

Boolean                 nextfailed;
int                     accumulateCount;
int                     accumulateMax;
Boolean                 nextisactive;

Boolean                 goodWriteOpen = False;

char                   *etsession = "clasprod";


Boolean                 etisok = False;

static Hv_Widget        filelabel = NULL;

static Hv_Widget        lastbank = NULL;
static Hv_Widget        evdialog = NULL;
static Hv_Widget        evnumber, evtype, evsize, evcount;
static Hv_Widget        evscale, evnext, evprev, evgoto, evopen, evconvert;
static Hv_Widget        evhead, evtage, evtagt, evmcin, evstn0, evstn1, evmcvx, evmctk, evecpi;  

static Hv_Widget        evhbtr;  /* for hit based tracks */
static Hv_Widget        evtbtr;  /* for time based tracks */
static Hv_Widget        evpart;  /* for reconstructed 4 vectors */

static Hv_Widget        writeAccEvTB = NULL;
static Hv_Widget        physicsOnly  = NULL;

static Boolean          etReadFailure = True;

Hv_TextPosition   evpos = 0;
Hv_Widget         evtext;

Hv_Widget         evdc0[6];
Hv_Widget         evdc1[6];

Hv_Widget         evseg[6];
Hv_Widget         evsc[6];
Hv_Widget         evcc[6];
Hv_Widget         evcc1[6];
Hv_Widget         evec[6];
Hv_Widget         evec1[6];

Hv_Widget         evhbla[6];  /* hit based layer banks */
Hv_Widget         evhdpl[6];  /* hit based non dc planes */

Hv_Widget         evtbla[6];  /* time based layer banks */
Hv_Widget         evtdpl[6];  /* time based non dc planes */

char             *eclayernames[4];

static Hv_Widget        dc0lab, dc1lab, seglab;
static Hv_Widget        sclab, cclab, cc1lab, eclab, ec1lab;
static Hv_Widget        hblalab, hdpllab, tblalab, tdpllab;

Hv_Widget               evnextmenu;
Hv_Widget               evprevmenu;
Hv_Widget               evgotomenu;
Hv_Widget               evwritemenu;
Hv_Widget               evclosemenu;
Hv_Widget               evoutputmenu;
Hv_Widget               evopenmenu;

Hv_Widget               evintervalmenu;
Hv_Widget               evfiltermenu;

/* this is a counter of events, not necessarily the same as the event number */
/* eventCount NOT used for events from ET */

int    eventCount;      

/* more global variables -- declared in bosced.h */

char     *bosDataName = "BOSINPUT";          /* data file access name for FPACK */
char     *bosFileName = NULL;

/* variables added for output */

char     *bosOutDataName = "BOSOUTPUT";     /* data file access name for FPACK */
char     *bosOutFileName = NULL;

int       bosUnit;                           /* FORTRAN input io unit number */
int       bosOutUnit;                        /* FORTRAN output io unit number */

int       bosNdim;                           /* array size for bcs */
int       bosWdim;                           /* array size for wcs */

int       bosNumEvents;                      /* number of events */
BOSBank   bcs_;                              /* the main bos bank (common) */
BOSBank   wcs_;                              /* additional bos bank (common) */

/* named bank indices */

int     bosIndexDC0[6];
int     bosIndexDC1[6];


/* geometry banks */

int     bosIndexDCGM[6];  /* DCGM geo bank */
int     bosIndexDCGW[6];  /* DCGW geo bank */

int     bosIndexSEG[6];
int     bosIndexCC[6];
int     bosIndexCC1[6];
int     bosIndexSC[6];
int     bosIndexEC[6];
int     bosIndexEC1[6];
int     bosIndexECPI;
int     bosIndexTAGE;
int     bosIndexTAGT;
int     bosIndexHEAD;
int     bosIndexTGBI;
int     bosIndexSTN0;
int     bosIndexSTN1;
int     bosIndexMCIN;
int     bosIndexMCVX;
int     bosIndexMCTK;

/* track related */

int     bosIndexHBLA[6]; /* hit based layers */
int     bosIndexHDPL[6]; /* hit based non dc planes */

int     bosIndexTBLA[6]; /* time based layers */
int     bosIndexTDPL[6]; /* time based non dc planes */

int     bosIndexHBTR;    /* hit based tracking result */
int     bosIndexTBTR;    /* time based tracking result */
int     bosIndexPART;    /* particle id bank (4 vector) */

/* the names of the recognized named banks */

char   *bosBankNameDC0  = "DC0 ";
char   *bosBankNameDC1  = "DC1 ";
char   *bosBankNameSEG  = "SEG ";
char   *bosBankNameCC   = "CC  ";
char   *bosBankNameCC1  = "CC1 ";
char   *bosBankNameSC   = "SC  ";
char   *bosBankNameEC   = "EC  ";
char   *bosBankNameEC1  = "EC1 ";
char   *bosBankNameECPI = "ECPI";

char   *bosBankNameDCGM = "DCGM";
char   *bosBankNameDCGW = "DCGW";

char   *bosBankNameTAGT = "TAGT";
char   *bosBankNameTAGE = "TAGE";
char   *bosBankNameHEAD = "HEAD";
char   *bosBankNameTGBI = "TGBI";
char   *bosBankNameST   = "ST  ";
char   *bosBankNameSTN0 = "STN0";
char   *bosBankNameSTN1 = "STN1";
char   *bosBankNameMCIN = "MCIN";
char   *bosBankNameMCVX = "MCVX";
char   *bosBankNameMCTK = "MCTK";

char   *bosBankNameHBLA = "HBLA";
char   *bosBankNameHDPL = "HDPL";

char   *bosBankNameTBLA = "TBLA";
char   *bosBankNameTDPL = "TDPL";

char   *bosBankNameHBTR = "HBTR";    /* hit based tracking result */
char   *bosBankNameTBTR = "TBTR";    /* time based tracking result */
char   *bosBankNamePART = "PART";    /* 4 vectors */

Boolean bosSectorHasData[6];
int     bosSectorDataSize[6];

/* data pointers */

DC0DataPtr     bosDC0[6];
DC1DataPtr     bosDC1[6];
SEGDataPtr     bosSEG[6];
NormalDataPtr  bosCC[6];
NormalDataPtr  bosCC1[6];
SCDataPtr      bosSC[6];
ECDataPtr      bosEC[6];
EC1DataPtr     bosEC1[6];
PiDataPtr      bosECPI;
TAGEDataPtr    bosTAGE;
TAGTDataPtr    bosTAGT;
HEADDataPtr    bosHEAD;
TGBIDataPtr    bosTGBI;
STN0DataPtr    bosSTN0;
STN1DataPtr    bosSTN1;
MCINDataPtr    bosMCIN;
MCVXDataPtr    bosMCVX;
MCTKDataPtr    bosMCTK;

/* geo banks */

DCGMDataPtr     bosDCGM[6];
DCGWDataPtr     bosDCGW[6];

/* for DC Noise removal test */

DCBitData       dcbitdata[6][3][2];  /* sect, rgn, supl */
DCBitData       cleandcbitdata[6][3][2];  /* sect, rgn, supl */


/* data pointers for tracks */

HBLADataPtr    bosHBLA[6];  /* hit based layers */
HDPLDataPtr    bosHDPL[6];  /* hit based non DC planes */
HBTRDataPtr    bosHBTR;     /* hit based tracking result */


TBLADataPtr    bosTBLA[6];  /* time based layers */
TDPLDataPtr    bosTDPL[6];  /* time based non DC planes */
TBTRDataPtr    bosTBTR;     /* time based tracking result */
PARTDataPtr    bosPART;     /* four vectors */

int            bosNumDC0[6];
int            bosNumDC1[6];
int            bosNumSEG[6];
int            bosNumCC[6];
int            bosNumCC1[6];
int            bosNumSC[6];
int            bosNumEC[6];
int            bosNumEC1[6];
int            bosNumECPI;
int            bosNumTAGT;
int            bosNumTAGE;
int            bosNumSTN0;
int            bosNumSTN1;
int            bosNumMCIN;
int            bosNumMCVX;
int            bosNumMCTK;

/* used for tracks */

int            bosNumHBLA[6]; /* hit based layers */
int            bosNumHDPL[6]; /* hit based non DC planes */
int            bosNumHBTR;    /* hit based tracking result */
int            bosNumPART;    /* four vectors */

int            bosNumTBLA[6]; /* time based layers */
int            bosNumTDPL[6]; /* time based non DC planes */
int            bosNumTBTR;    /* time based tracking result */

long           accumEventCount;
int            triggerFilter;
int            triggerMaxTries;  /* number times will try to find a trigger */
int            triggerBits[] = {   01,      02,      04,     010,
				   020,     040,     0100,   0200,
				   0400,    01000,   02000,  04000,
				   010000,  020000,  040000, 0100000};

AccumulatedDC   accdc[6][3]; 
AccumulatedSC   accsc[6]; 
AccumulatedEC   accec[6]; 
AccumulatedEC1  accec1[6];

ComponentStatusPtr dcstatus[6][3][2];  /* used to store dead wires */


static Hv_Widget ntoacc = NULL;
static Boolean   drawNextEvent = True;
static Boolean   stopAccumulating = False;
static Hv_Widget accdialog = NULL;

/*------ InitBOS -------*/

void
InitBOS(void)    
{
  int i, j, k;
  
  /* initialize bos indices and data pointers*/
  
  bosZeroBankIndices();
  
  /* use unit 1 to read bos banks */
  
  bosUnit = 1;
  bosNdim = NDIM;
  bosWdim = WDIM;
  
  bosNumEvents = 0;
  eventCount = 0;
  triggerFilter = 0;  /* 0 means accept any event */
  triggerMaxTries = 100;
  
  /* use unit 7 to write events */
  
  bosOutUnit = 7;
  
  bos_(bcs_.iw, &bosNdim);     /* initialize the main bos common */
  bcs_.iw[5] = 0;              /* suppress printing -- NOTE C INDEX EFFECT */
  
  bos_(wcs_.iw, &bosWdim);     /* initialize the additional bos common */
  wcs_.iw[5] = 0;              /* suppress printing -- NOTE C INDEX EFFECT */
  
  /* initialize the bit data used in noise reduction */
  
  InitBitData();
  
  /* event file filter */
  
#ifdef OFFLINE
  Hv_InitCharStr(&eventFileFilter, "*.*00");
#else
  Hv_InitCharStr(&eventFileFilter, "/raid/stage_in/*.*00");
#endif
  
  /* zero the histos */
  
  ZeroEventHistos(0);
  
  /* initialize the wire status */
  
  for (i= 0; i < 6; i++)
    for (j = 0; j < 3; j++)
      for (k = 0; k < 2; k++)
	dcstatus[i][j][k] = NULL;
  
  /* get any and all name indices, which do
     not change from event to event */
  
  Hv_InitCharStr(&(eclayernames[0]), "both");
  Hv_InitCharStr(&(eclayernames[1]), "inner");
  Hv_InitCharStr(&(eclayernames[2]), "outer");
  Hv_InitCharStr(&(eclayernames[3]), "unkn");
  
  
  /* BOS geometry banks */
  
  ReInitGeometry();
  
}

#define BUTTONGAP   0
#define BUTTONGAP2  0





/* Sergey: following 3 routines for noise reduction;
   commented out for a while, will use our version in future */

/*--------- InitBitData ----------------*/

static void InitBitData() {
  
  
  /* sergey: DC noise removel ONLY */
  /*sginit();*/
  
#ifdef BLABLA
  /*sergey: FULL pattern recognition (calls sginit() inside)*/
  trconfig();   /* read 'cmon.config' file */
  /*l3init(9999);*/ /* level 3 trigger initialization */
  prinit_();    /* pattern recognition initialization */
#endif
  
  /*
    
  int sect, rgn, supl;
  int numlay, numwire;
  int i;
  static Boolean  called = False;
  
  if (called) {
  fprintf(stderr, "WARNING InitBitData called more than once.\n");
  return;
  }
  
  
  for (sect = 0; sect < 6; sect++)
  for (rgn = 0; rgn < 3; rgn++)
  for (supl = 0; supl < 2; supl++) {
  numlay = DriftChambers[sect].regions[rgn].superlayers[supl].numlayers;
  numwire = DCLastWire(sect, rgn, supl, 0) + 1;
  
  for (i = 0; i < 6; i++) {
  if (i < numlay) {
  dcbitdata[sect][rgn][supl].data[i] = mallocHugeWord(numwire);
  cleandcbitdata[sect][rgn][supl].data[i] = mallocHugeWord(numwire);
  }
  else {
  dcbitdata[sect][rgn][supl].data[i] = NULL;
  cleandcbitdata[sect][rgn][supl].data[i] = NULL;
  }
  }
  
  dcbitdata[sect][rgn][supl].nlayer = numlay;
  dcbitdata[sect][rgn][supl].nwire = numwire;
  cleandcbitdata[sect][rgn][supl].nlayer = numlay;
  cleandcbitdata[sect][rgn][supl].nwire = numwire;
  
  }
  
  ZeroBitData();
  called = True;
  */
}

/*--------- ZeroBitData ----------------*/

static void ZeroBitData() {
  /*
    int sect, rgn, supl;
    int i;
    
    for (sect = 0; sect < 6; sect++)
    for (rgn = 0; rgn < 3; rgn++)
    for (supl = 0; supl < 2; supl++) {
    
    for (i = 0; i < 6; i++) {
    if (i < dcbitdata[sect][rgn][supl].nlayer) {
    ClearHugeWord(dcbitdata[sect][rgn][supl].data[i]);
    ClearHugeWord(cleandcbitdata[sect][rgn][supl].data[i]);
    }
    else {
    dcbitdata[sect][rgn][supl].data[i] = NULL;
    cleandcbitdata[sect][rgn][supl].data[i] = NULL;
    }
    }
    
    }
  */
}

/*------------ SetBitData -------------*/

static void SetBitData() {
  /* This procedure converts dc hits into bit hits for
     use by the noise reduction algorithm */
  
  /*
    
  int sect,  rgn, supl, lay;
  DC0DataPtr hits0;
  int        num;
  int        i;
  unsigned char layer, wire;
  
  ZeroBitData();
  
  for (sect = 0; sect < 6; sect++) {
  hits0 = bosDC0[sect];
  
  if (hits0 != NULL) {
  num = bosNumDC0[sect];
  for (i = 0; i < num; i++) {
  if (checkDCData(sect, hits0)) {
  BreakShort(hits0->id, &wire, &layer);    
  wire--;
  layer--;
  rgn = layer / 12;
  supl = (layer % 12) / 6;
  lay = layer % 6;
  SetBitHugeWord(dcbitdata[sect][rgn][supl].data[lay], wire);
  }
  hits0++;
  }
  }
  
  }
  */
}


/*------------- ZeroEventHistos ---------------*/

void ZeroEventHistos(int flag) {
     
  /* zeros the event histos */
     
  int    scint, sect, reg, layer0, wire;
  int    plane, strip, type;
  int    supl;
  int    layer, numlay, numwire;
  
  
  fprintf(stderr, "start zero histos...");
  
  accumEventCount = 0;
  
  
  /* first DC */
  
  for (sect = 0; sect < 6; sect++) {
    for (reg = 0; reg < 3; reg++) {
      accdc[sect][reg].maxhits = 0;
      accdc[sect][reg].hotid = 0;
      
      accdc[sect][reg].scale = 1;
      
      for (supl = 0; supl < 2; supl++) {
	layer0 = 6*supl;
	
	/* occupancy sum stored on a superlayer basis */
	
	accdc[sect][reg].occsum[supl] = 0.0;
	
	numlay = DCNumLayers(sect, reg, supl);
	for (layer = 0; layer < numlay; layer++) {
	  numwire = DCNumWires(sect, reg, supl, layer);
	  for (wire = 0; wire < numwire; wire++) {
	    accdc[sect][reg].counts[layer0 + layer][wire] = 0;
	    accdc[sect][reg].tdcsum[layer0 + layer][wire] = 0.0;
	  }
	} 
      }
      
    }
  }
  
  /* now SC */
  
  for (sect = 0; sect < 6; sect++) {
    accsc[sect].maxhits = 0;
    accsc[sect].scale = 1;
    for (scint = 0; scint < NUM_SCINT; scint++) {
      accsc[sect].counts[scint] = 0;
    }
  }
  
  /* now EC  */
  
  for (sect = 0; sect < 6; sect++) {
    accec[sect].maxhits = 0;
    accec[sect].scale = 1;
    for (plane = 0; plane < 2; plane++) {
      for (type = 0; type < 3; type++) {
	for (strip = 0; strip < 36; strip++) {
	  accec[sect].counts[plane][type][strip] = 0;
	}
      }
    }
  }
  
  
  /* now EC1 (Italian LAC) (actually x only goes to 24) */
  
  for (sect = 0; sect < 6; sect++) {
    accec1[sect].maxhits = 0;
    accec1[sect].scale = 1;
    for (plane = 0; plane < 2; plane++) {
      for (type = 0; type < 2; type++)
	for (strip = 0; strip < 40; strip++)
	  accec1[sect].counts[plane][type][strip] = 0;
      
    }
  }
  
  
  fprintf(stderr, "done\n");
}


/*------------- AccumulateDialog -------------*/
/* dialog that does the event accumulation */

void AccumulateDialog(Hv_Widget w) {    

  Hv_Widget    rc, rowcol, dummy;
  static Hv_Widget label;
  
  if(!accdialog) {
    
    Hv_VaCreateDialog(&accdialog, Hv_TITLE, " Accumulate Events ", NULL);
    rowcol = Hv_DialogColumn(accdialog, 6);
    
    label = Hv_StandardLabel(rowcol, "             ", 0);
    writeAccEvTB = Hv_SimpleToggleButton(rowcol, "Write accumulated Events to a file");
    physicsOnly  = Hv_SimpleToggleButton(rowcol, "Accumulate only Physics Events");
    Hv_SetToggleButton(writeAccEvTB, False);
    Hv_SetToggleButton(physicsOnly, True);
    dummy = Hv_SimpleDialogSeparator(rowcol);
    
    rc = Hv_DialogRow(rowcol, 6);
    ntoacc = Hv_SimpleTextEdit(rc, " Number ", "10000", 4);
    dummy = Hv_StandardButton(rc, "Accumulate", AccumulateCB);
    /* close out */
    dummy = Hv_SimpleDialogSeparator(rowcol);
    rc = Hv_DialogRow(rowcol, 50);
    dummy = Hv_StandardButton(rc,     " Zero Histos ", ZeroCB);
    
    dummy = Hv_StandardDoneButton(rc, " Close/Stop ");
  }

  /* the dialog has been created */
  
  accumulateCount = 0;
  stopAccumulating = True;
  Hv_ChangeLabel(label, "             ", Hv_fixed2);  
  Hv_DoDialogWithTimer(accdialog, NULL, AccumulateTimerCB, NULL, 1000, label,
                       Hv_fixed2);
}


/*------------- ZeroCB ---------------*/

static void ZeroCB(Hv_Widget w) {
  if(Hv_Question("Zero all accumulated data?")) ZeroEventHistos(0);
}

/*-------- initAccumWrite ----*/

static void initAccumWrite() {
  //write events?
  
  if (writeAccEvents()) {
    cleanupAccumWrite();
    OpenOutputFile();
  }
  
}

static void cleanupAccumWrite() {
  if (writeAccEvents()) {
    if (bosOutFileName != NULL) {
      //if we have been writing out accumulated events, close output file
      CloseOutputFile();
      Hv_Free(bosOutFileName);
      bosOutFileName = NULL;
    }
  }
}


/*----------- AcuumulateTimerCB ----------*/
/* do not do anything if not accumulating */

static void AccumulateTimerCB(Hv_TimerDialogData tdlogdata) {    
  if(stopAccumulating) {
    cleanupAccumWrite();
    return;
  }
  
  
  AccumulateEvents(50, accumulateMax);

  stopAccumulating = (accumulateCount >= (accumulateMax-1));
  if(stopAccumulating) {  /* done ? */
    StopWait();
    Hv_SysBeep();
    /* make sure Hv_DialogTimerCB does not set another timer */
    tdlogdata->waitid = 0;
    Hv_CloseDialog(accdialog);
    cleanupAccumWrite();
    return;
  }
  
  /* update the label */
  Hv_Free(tdlogdata->updatetext);
  tdlogdata->updatetext = (char *)Hv_Malloc(20);
  sprintf(tdlogdata->updatetext, "%d", accumulateCount);
}



/*-------------- AccumulateCB ----------------*/
/* this is the call back for the "accumulate" button
   on the accumulate dialog */

static void AccumulateCB(Hv_Widget w) {
  int nmax;
  
  initAccumWrite(); //write to a file?
  
  /* get the number to accumulate */
  accumulateMax = Hv_GetIntText(ntoacc);
  
  /* if from a file, don't ask for too many! */
  if(precountEvents) {
    if(eventsource == CED_FROMFILE) {
      nmax = (bosNumEvents - eventCount);
      if(accumulateMax > nmax) {
        fprintf(stderr, "Can only accumulate %d more events from current file.\n", nmax);
        accumulateMax = nmax;
      }
    }
  }
  
  stopAccumulating = False;
  accumulateCount = 0;
  StartWait(); /* the wait cursor */
}


/*-------------- AccumulateEvents -----------*/
/* This is used to accumulate events into the histo
   data structures. It calls next event n times, but only
   displays the last one */

void AccumulateEvents(int n, int nmax)  {
  int imax;
  int etype;
  Boolean onemore;
  
  if(n < 1) {
    return;
  }
  
  drawNextEvent = False;
  onemore = False;
  
  imax = accumulateCount + n;
  
  if(imax >= (nmax - 1)) {
    onemore = True;
    imax = nmax - 1;
  }
  
  
  while(accumulateCount < imax) {
    printf("accumulateCount=%d\n",accumulateCount);
    etype = -1;
    NextEvent();
    if(bosHEAD != NULL) {
      etype = bosHEAD->EventType;
    }
    
    
    if(!nextfailed) {    
      if(stopAccumulating) {
	fprintf(stderr, "Stopped accumulating\n");
	DrawEvent();
	UpdateEventDialog();
	return;
      }
      if (!accPhysicsOnly() || (etype > 0) && (etype < 16)) {
	
	
	if (writeAccEvents()) {
	  WriteEvent();
	}
	
	accumulateCount++;
      }
      else {
	fprintf(stderr, "Skipped non-physics event type: %d\n", etype);
      }
    } // !nextfailed
  }
  
  drawNextEvent = True;
  
  /* the last time through we need one more (which gets drawn) */
  
  if(onemore) {
    nextfailed = True;
    while(nextfailed) {
      NextEvent();
    }
    accumulateCount++;
  }
  
}

/*
 * check to see whether we should write accumulated event
 */

static Boolean writeAccEvents() {
  return (writeAccEvTB != NULL) && (Hv_GetToggleButton(writeAccEvTB));
}

/*
 * check to see whether we should we accumulate physics only events
 */

static Boolean accPhysicsOnly() {
  return (physicsOnly != NULL) && (Hv_GetToggleButton(physicsOnly));
}

/*------------ CountToColor ------------*/
/****************************************
  
  Used for "hit maps"
  
  Coverts the count to a color, from mincolor
  for zero to Hv_red for max.The flag algorithm
  has the options:
  
  NOTE: you might want to only call this if count > 0,
  and use Hv_black for 0
  
  1:   linear
  2:   square root (grows to Hv_red faster,
  so that low counts stand out more)
  3:   exponential
  
  
*******************************************/

short
CountToColor(int count, Hv_RainbowData *data, int algorithm)    
{
  short  color;
  float  fcount;
  static float  del, fdel, ratio;
  
  
  del = (float)(data->maxcolor - data->mincolor);
  fcount = (float)count;
  fdel = (float)(data->valmax - data->valmin);
  
  if (fdel < 1.0e-10)
    return data->mincolor;
  
  ratio = (fcount - (float)(data->valmin)) / fdel;
  
  switch (algorithm) {
  case 1:
    color = data->mincolor +  (short)(del * ratio);
    break;
    
  case 2:
    color = data->mincolor +  (short)(del * sqrt(ratio));
    break;
    
  case 3:
    color = data->mincolor + (short)(del * 1.5819*(1.0 - exp(-ratio)));
    break;
    
  default:
    color =  Hv_gray12;
  }
  
  
  return color;
  
}

/*------- EventControl --------*/

void EventControl(Hv_Widget w)
     
     /* handle the event controller.. the complicated
	ber-like event controller  */
     
{
  static Hv_Widget        close;
  Hv_Widget               dummy, rowcol, rc, rcmain, rc1, rc2;
  char                    tstr[20];
  int                     i;
  
  if (!evdialog) {
    Hv_VaCreateDialog(&evdialog,
		      Hv_TYPE,      Hv_MODELESS,
		      Hv_TITLE,     " Event Control ",
		      NULL);
    
    rowcol = Hv_TightDialogColumn(evdialog, 0);
    
    
    /* first create the informational fields */
    
    rc = Hv_DialogTable(rowcol, 2, 2);
    evnumber  = StandardInfoPair(rc, "     Number"); 
    evtype    = StandardInfoPair(rc, "     Type"); 
    evsize    = StandardInfoPair(rc, "     Size"); 
    evcount   = StandardInfoPair(rc, "     Count"); 
    
    filelabel = Hv_StandardLabel(rowcol, " ", -1);
    
    /* event scale */
    
    dummy = Hv_SimpleDialogSeparator(rowcol);
    evscale = Hv_StandardScale(rowcol, 0, 100, 0, ScaleCB);
    dummy = Hv_SimpleDialogSeparator(rowcol);
    
    /* split the dialog down the middle. Left half raw bnks, right half analyzed */
    
    rcmain = Hv_TightDialogRow(rowcol, 10);
    rc1 = Hv_TightDialogColumn(rcmain, 0);
    rc2 = Hv_TightDialogColumn(rcmain, 0);
    
    /* bank buttons */
    
    dummy = Hv_StandardLabel(rc1, "Raw Banks", 0);
    
    /* once only banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP+2);
    evhead = Hv_StandardButton(rc, "HEAD", BankCB);
    evtage = Hv_StandardButton(rc, "TAGE", BankCB);
    evtagt = Hv_StandardButton(rc, "TAGT", BankCB);
    evstn0 = Hv_StandardButton(rc, "STN0", BankCB);
    evstn1 = Hv_StandardButton(rc, "STN1", BankCB);
    evmcin = Hv_StandardButton(rc, "MCIN", BankCB);
    evmcvx = Hv_StandardButton(rc, "MCVX", BankCB);
    evmctk = Hv_StandardButton(rc, "MCTK", BankCB);
    
    /* drift chamber banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    dc0lab = Hv_StandardLabel(rc, "DC0", 0);
    
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "DC0 %1d", i+1);
      evdc0[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    
    /* DC1 drift chamber banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    dc1lab = Hv_StandardLabel(rc, "DC1", 0);
    
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "DC1 %1d", i+1);
      evdc1[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    
    /* Level 2 segment banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    seglab = Hv_StandardLabel(rc, "SEG", 0);
    
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "SEG %1d", i+1);
      evseg[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* scintillator banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    sclab = Hv_StandardLabel(rc, "SC", 0);
    
    for (i = 0; i < 6; i++) {
      sprintf(tstr, " SC %1d", i+1);
      evsc[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* forward ec banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    eclab = Hv_StandardLabel(rc, "EC", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, " EC %1d", i+1);
      evec[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* large angle ec1 banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    ec1lab = Hv_StandardLabel(rc, "EC1", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "EC* %1d", i+1);
      evec1[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* forward cc banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    cclab = Hv_StandardLabel(rc, "CC", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, " CC %1d", i+1);
      evcc[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* large angle cc1 banks */
    
    rc = Hv_DialogRow(rc1, BUTTONGAP);
    cc1lab = Hv_StandardLabel(rc, "CC1", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "CC* %1d", i+1);
      evcc1[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* analyzed banks */
    
    dummy = Hv_StandardLabel(rc2, "Analyzed Banks", 0);
    
    /* some once only analyzed banks */
    
    rc = Hv_DialogRow(rc2, BUTTONGAP);
    evhbtr = Hv_StandardButton(rc, "HBTR", BankCB);
    evtbtr = Hv_StandardButton(rc, "TBTR", BankCB);
    evpart = Hv_StandardButton(rc, "PART", BankCB);
    evecpi = Hv_StandardButton(rc, "ECPI", BankCB);
    
    /* hbla banks (hit based) */
    
    rc = Hv_DialogRow(rc2, BUTTONGAP2);
    hblalab = Hv_StandardLabel(rc, "HBLA", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "HBLA %1d", i+1);
      evhbla[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* hdpl banks (hit based non DC) */
    
    rc = Hv_DialogRow(rc2, BUTTONGAP2);
    hdpllab = Hv_StandardLabel(rc, "HDPL", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "HDPL %1d", i+1);
      evhdpl[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* tbla banks (time based) */
    
    rc = Hv_DialogRow(rc2, BUTTONGAP2);
    tblalab = Hv_StandardLabel(rc, "TBLA", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "TBLA %1d", i+1);
      evtbla[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    /* tdpl banks (time based non DC) */
    
    rc = Hv_DialogRow(rc2, BUTTONGAP2);
    tdpllab = Hv_StandardLabel(rc, "TDPL", 0);
    for (i = 0; i < 6; i++) {
      sprintf(tstr, "TDPL %1d", i+1);
      evtdpl[i]  = Hv_StandardButton(rc, tstr, BankCB);
    }
    
    
    /* scrolled text for the dump */
    
    dummy = Hv_SimpleDialogSeparator(rowcol);
    
    evtext = Hv_VaCreateDialogItem(rowcol,
				   Hv_TYPE,        Hv_SCROLLEDTEXTDIALOGITEM,
				   Hv_NUMROWS,     18,
				   Hv_BACKGROUND,  Hv_aliceBlue,
				   Hv_TEXTCOLOR,   Hv_black,
				   Hv_NUMCOLUMNS,  101,
				   NULL);
    
    /* now the buttons */
    
    dummy = Hv_SimpleDialogSeparator(rowcol);
    rc = Hv_DialogRow(rowcol, 10);
    
    evnext = Hv_StandardButton(rc, " Next  ", NextEvent);
    
    evprev    = Hv_StandardButton(rc, " Prev  ",  PrevEvent);
    evgoto    = Hv_StandardButton(rc, " Goto  ",  GotoEvent);
    
    evopen    = Hv_StandardButton(rc, " Open  ",  OpenEventCB);
    evconvert = Hv_StandardButton(rc, "Convert",  ConvertCB);
    
    Hv_SetSensitivity(evconvert,  False);
    close     = Hv_StandardButton(rc, " Close ", CloseEventControlDialog);
  }
  
  UpdateEventDialog();
  Hv_OpenDialog(evdialog, NULL);
}

#undef BUTTONGAP

/*------ CloseEventControlDialog --------*/

static void CloseEventControlDialog(Hv_Widget w)
     
{
  Hv_CloseDialog(evdialog);
}


/*-------- NextEvent ---------*/

void NextEvent(void)   {
  int et_ret;

  /*printf("NextEvent() reached\n");*/
  
  /* if we are looking for specific bits, call a different routine */
  /*sergey if(triggerFilter != 0)*/
  {
    NextMatchingEvent();
    return;
  }  

  /* assume success */
  nextfailed = False;
  
  if (drawNextEvent) {
    EraseEvent();
  }
  
  if (eventsource == CED_FROMET) {
    bosDropBanks();

    et_ret = get_et_event_(0);

    etReadFailure = (et_ret >= 0);

    if (etReadFailure) {
      fprintf(stderr, "ET GetEvent failed. get_et_event returned: %d\n", et_ret);
      nextfailed = True;
    }
    else {
      nextfailed = False;
      ReadyEvent();
    }
  }//end from ET
  
  else { /* from file */
    if(!ReadNextEventFromFile()) {
      stopAccumulating = True;
      Hv_SysBeep();
      return;
    }
    
    eventCount++;
    ReadyEvent();
  }
  
}


/*----------- NextMatchingEvent ---------*/
/*
 *   Here we scan for events with a type
 *   that matches one of the trigger bits
 *   set
 */

static void NextMatchingEvent() {
  int etype, trigpattern, count, ef;
  Boolean match;
  
  printf("NextMatchingEvent() reached\n");
  
  nextfailed = False;
  
  if(drawNextEvent) {
    EraseEvent();
  }
  
  /* counts used to count attempts */
  count = 0;
  
  if(eventsource == CED_FROMET) {

    bosDropBanks();
    if(etisok) {
      do {
	count++;
	if(count > triggerMaxTries) {
	  nextfailed = True;
	  return;
	}
	ef = get_et_event_(0); 
	if(ef >= 0) {
	  nextfailed = True;
	  if(stopAccumulating)
	    fprintf(stderr, "Problem with get_et_event_\n"); 
	  return;
	}
	      
	bosGetBankIndices();
	      
	etype = -1;
	trigpattern = 0;
	if(bosHEAD != NULL) etype = bosHEAD->EventType;
	if(bosTGBI != NULL) trigpattern = (bosTGBI->res1)&0xFFF;
	
	/*sergey match = ((etype > 0) && (etype < 16) && */
	match = ((etype > 0) && (etype < 15) && (trigpattern != 0) &&
		 Hv_CheckBit(triggerFilter, trigpattern));
	
      } while (!match);
    }
  } /* end from et */  
  else { /* from file */
    do {
      count++;
      
      if(count > triggerMaxTries) {
	nextfailed = True;
	stopAccumulating = True;
	Hv_SysBeep();
	DrawEvent();  /* cause it was erased */
	return;
      }
      
      if(!ReadNextEventFromFile()) {
	stopAccumulating = True;
	Hv_SysBeep();
	DrawEvent();  /* cause it was erased */
	return;
      }
      
      bosGetBankIndices();
      
      etype = -1;
      trigpattern = 0;
      if(bosHEAD != NULL) etype = bosHEAD->EventType;
      if(bosTGBI != NULL) trigpattern = (bosTGBI->res1)&0xFFF;
      
      match = ((etype > 0) && (etype < 16) &&
	       Hv_CheckBit(triggerFilter, trigpattern));
      
      /*printf("match: triggerFilter=0x%08x trigpattern=0x%08x match=%d\n",
	triggerFilter, trigpattern, match);*/
      
    } while (!match);
    
    eventCount++;
  } //end from file
  
  ReadyEvent();
}


/*------- ReadyEvent -------*/

static void ReadyEvent()  {
  bosGetBankIndices();
  
  /* look for bad events */
  
  eventBadCode = -1;
  eventBadFlag = -1;
  eventBadFormat = False;
  
  BookEvent();
  
  SetBitData();
  
  if(drawNextEvent) {
    UpdateEventFeedback();
    UpdateEventDialog();
    FixMostDataViews();
    FixLargestECSumViews();
    DrawEvent();
    EventPrintName();
  }
  
}

/*----- SetEventBad -------*/

extern void SetEventBad(int code, int flag)
     
{
  eventBadFormat = True;
  eventBadCode = code;
  eventBadFlag = flag;
}




/*-------- PrevEvent ---------*/

void PrevEvent(void)
     
{
  int    i;
  
  if (eventsource == CED_FROMET) {
  }
  
  else { /* from file */
    eventCount -= 2;
    EraseEvent();
    bosRewind(bosDataName);
    
    for (i = 0; i < eventCount; i++)
      ReadNextEventFromFile();
    
    NextEvent();
  }
}

/* -------- ReadNextEventFromFile ------*/

static Boolean
ReadNextEventFromFile()
{
  int iret;
  
  bosDropBanks();
  frbos_(bcs_.iw, &bosUnit, "E", &iret, 1);
  
  return(iret == 0);
}


/* ----------- GotoEvent -------------*/

void GotoEvent(void)
     
{
  static  Widget   dialog = NULL;
  static  Widget   gtevnum;
  Widget           rowcol, rc, dummy;
  int              num;      
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Goto Event", NULL);
    
    rowcol = Hv_DialogColumn(dialog, 8);
    rc = Hv_DialogRow(rowcol, 10);
    gtevnum = Hv_SimpleTextEdit(rc, "Event Number ", NULL, 4);
    
    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }
  
  /* the dialog has been created */
  
  Hv_SetIntText(gtevnum, Hv_iMax(1, eventCount));
  
  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    num = Hv_GetIntText(gtevnum);
    
    if (num > 0) {
      if (!precountEvents || (num <= bosNumEvents))
	GotoEventNumber(num);
    }
    
  }
}


/* ----------- SetEventTimeInterval -------------*/

void SetEventTimeInterval(void)
     
     /* time interval used when events are "on timer" */
     
{
  static  Widget   dialog = NULL;
  static  Widget   gtevtime;
  Widget           rowcol, rc, dummy;
  int              num;      
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Event Time Interval", NULL);
    
    rowcol = Hv_DialogColumn(dialog, 8);
    rc = Hv_DialogRow(rowcol, 10);
    gtevtime = Hv_SimpleTextEdit(rc, "Interval (sec) ", NULL, 4);
    
    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }
  
  /* the dialog has been created */
  
  Hv_SetIntText(gtevtime, Hv_iMax(1, eventtime));
  
  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    num = Hv_GetIntText(gtevtime);
    if (num > 0) {
      eventtime = num;
      EventButtonSelectability();
      if (eventwhen == CED_ONTIMER) {
	
	/*
	 * 2.025 Mod  commented out old method. Now,
	 * if there was a time do NOTHING because
	 * the timer timeout will pick up the new
	 * time. If there was not a timer, start one.
	 */
	
	/*		StopEventTimer();
			StartEventTimer(); */
	
	if (eventinterval == 0)
	  StartEventTimer();
	
	
      }
    }
  }
}


/* ----------- SetEventFileFilter -------------*/

void SetEventFileFilter(void)
     
     /* time interval used when events are "on timer" */
     
{
  static  Widget   dialog = NULL;
  static  Widget   gtfilter;
  Widget           rowcol, rc, dummy;
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Event File Extension", NULL);
    
    rowcol = Hv_DialogColumn(dialog, 8);
    rc = Hv_DialogRow(rowcol, 10);
    gtfilter = Hv_SimpleTextEdit(rc, "extension ", NULL, 4);
    
    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }
  
  /* the dialog has been created */
  
  Hv_SetString(gtfilter, eventFileFilter);
  
  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    Hv_Free(eventFileFilter);
    eventFileFilter = Hv_GetString(gtfilter);
  }
}


/*-------- EraseEvent --------*/

void   EraseEvent()
     
     /* erase from all views */
     
{
  Hv_View    temp;
  
  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next)  
    EraseViewEvent(temp);
}

/*------ EraseViewEvent ----*/

void EraseViewEvent(Hv_View  View)
     
     /* erases by restoring background */
     
{
  Hv_Region  clipreg = NULL;
  
  clipreg = Hv_ClipHotRect(View, 1);
  Hv_RestoreViewBackground(View);
  Hv_DrawNonBackgroundItems(View, clipreg);
  
  Hv_DestroyRegion(clipreg);
}


/*------ DrawEvent ---------*/

void   DrawEvent(void)
     
{
  Hv_View   temp;
  
  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next)  
    if (Hv_ViewIsVisible(temp))
      DrawViewEvent(temp, NULL);
}

/*------- DrawViewEvent --------*/

void DrawViewEvent(Hv_View   View,
		   Hv_Region region) {
  short     sect;
  ViewData  viewdata;
  Hv_Region hreg;
  Hv_Region clipreg = NULL;
  
  viewdata = GetViewData(View);
  if (!(viewdata->displayEvent))
    return;
  
  /* If there is no event data, no need to hang around */
  
  if ((EventSize() <= 0) && (viewdata->showwhat == SINGLEEVENT))
    return;
  
  hreg = Hv_ClipHotRect(View, 1);
  
  clipreg = Hv_IntersectRegion(hreg, region);
  Hv_SetClippingRegion(clipreg);
  
  if (viewdata->TaggerItem != NULL) {
    
  }
  
  
  
  Hv_DestroyRegion(hreg);
  
  switch (View->tag) {
  case SECTOR:
    sect = GoodSector(View);
    
    DrawTaggerHits(View, clipreg);
    if ((viewdata->displayTagger) && (viewdata->TaggerItem != NULL)) {
      Hv_RemoveRectFromRegion(&clipreg, viewdata->TaggerItem->area);
      Hv_SetClippingRegion(clipreg);
    }
    
    
    /* sergey: FULL pattern recognition*/
    if(reducenoise)
      {
	int ifail;
	if(viewdata->suppressNoise)
	  {
	    /*printf("calls prlib_()\n");*/
	    /*l3lib(bcs_.iw,&ifail);*/
	    /*printf("l3lib() returns %d\n",ifail);*/
	    prlib_(bcs_.iw,&ifail);
	    /*printf("prlib_() returns %d\n",ifail);*/
	    /*
	      pr_makeDC1(bcs_.iw);
	    */
	  }
      }
    
    
    
    /* 'Sector' view */
    /* Sergey: DC noise removal ONLY
       if(reducenoise)
       {
       if(viewdata->suppressNoise)
       {
       int layershifts[6] = {0, 2, 3, 3, 4, 4};
       int nsl = 5;
       int isec;
       for(isec=1; isec<=6; isec++)
       {
       sgremovenoise(bcs_.iw, isec, nsl, layershifts);
       }
       }
       }
    */
    
    
    
    /* go package by package */
    
    DrawScintHits(View, sect,   clipreg);
    DrawScintHits(View, sect+3, clipreg);
    
    DrawCerenkovHits(View, sect,   clipreg);
    DrawCerenkovHits(View, sect+3, clipreg);
    
    DrawShowerHits(View, sect,   clipreg);
    DrawShowerHits(View, sect+3, clipreg);
    
    if (sect < 2)
      DrawEC1Hits(View, sect,   clipreg);
    
    /* the DC0 hits */
    
    DrawDC0Hits(View, sect,   clipreg);
    DrawDC0Hits(View, sect+3,   clipreg);
    
    /* the DC1 hits */
    
    DrawDC1Hits(View, sect,   clipreg);
    DrawDC1Hits(View, sect+3,   clipreg);
    
    /* level 2 trigger segments */
    
    DrawSEGHits(View, sect,   clipreg);
    DrawSEGHits(View, sect+3, clipreg);
    
    /* dead wires */
    
    DrawDeadWires(View, sect,   clipreg);
    DrawDeadWires(View, sect+3, clipreg);
    
    /*  hit based tracks */
    
    DrawHBTrackBanks(View, sect, clipreg);
    DrawHBTrackBanks(View, sect+3, clipreg);
    
    /*  time based tracks */
    
    DrawTBTrackBanks(View, sect, clipreg);
    DrawTBTrackBanks(View, sect+3, clipreg);
    
    break;
    
  case SCINT:
    DrawStartHits(View, clipreg);
    for (sect = 0; sect < 6; sect++)
      DrawScintViewHits(View, sect,   clipreg);
    break;
    
    
  case ALLDC:
    
    
    
    /* Sergey: FULL pattern recognition */
    if(reducenoise)
      {
        int ifail;
        if(viewdata->suppressNoise)
	  {
	    printf("calls prlib_()\n");
	    /*l3lib(bcs_.iw,&ifail);*/
	    printf("l3lib() returns %d\n",ifail);
	    prlib_(bcs_.iw,&ifail);
	    printf("prlib_() returns %d\n",ifail);
	    /*
	      pr_makeDC1(bcs_.iw);
	    */
	  }
      }
    
    
    
    for (sect = 0; sect < 6; sect++)
      {
	/* 'Drift Chambers' view */
	
	
        /* Sergey: DC noise removal ONLY 
	   if(reducenoise)
	   {
	   if(viewdata->suppressNoise)
	   {
	   int layershifts[6] = {0, 2, 3, 3, 4, 4};
	   int nsl = 5;
	   int isec = sect + 1;
	   sgremovenoise(bcs_.iw, isec, nsl, layershifts);
	   }
	   }	    
        */
	
	/* go package by package */
	
	DrawScintHits(View, sect,   clipreg);
	
	/* the DC0 hits */
	
	DrawDC0Hits(View, sect,   clipreg);
	
	/* the DC1 hits */
	
	DrawDC1Hits(View, sect,   clipreg);
	
	/* level 2 trigger segments */
	
	DrawSEGHits(View, sect,   clipreg);
	
	/* dead wires */
	
	DrawAllDCDeadWires(View, sect,   clipreg);
	
	/* the hit based track banks */
	
	DrawHBTrackBanks(View, sect, clipreg);
	
	/* the time based track banks */
	
	DrawTBTrackBanks(View, sect, clipreg);
	
      }
    break;
    
  case CALORIMETER:
    for (sect = 0; sect < 6; sect++)
      DrawFannedShowerHits(View, sect, clipreg);
    
    break;
    
  }  /* end of switch on tag */
  
  
  Hv_DestroyRegion(clipreg);
  Hv_SetClippingRegion(region);
}

/*-------- OpenEventFile ---------*/

void OpenEventFile(char *fn) {
  
  char *text;
  
  if ((fn == NULL) || (strlen(fn) < 1)) {
    fprintf(stderr, "bad file name \n");
    Hv_Warning("No event file name given.");
    return;
  }
  
  if (!Hv_FileExists(fn)) {
    fprintf(stderr, "event file does not exist\n");
    text = (char *)Hv_Malloc(strlen(fn) + 30);
    sprintf(text, "Could not open file %s", fn);
    Hv_Warning(text);
    Hv_Free(text);
    return;
  }
  
  Hv_Free(bosFileName);
  Hv_InitCharStr(&bosFileName, fn);
  
  bosClose(bosDataName);
  
  bosNumEvents = 0;
  
  /* every open causes eventCount to be reset */
  
  eventCount = 0;
  
  bosOpen(bosDataName, bosUnit, bosFileName, 0);    /* open the event file */
  
  if (precountEvents)
    {
      bosNumEvents = bosCountEvents(bosDataName, 0);
      fprintf(stderr, "Number of events in file [%s] was %d\n", bosFileName, bosNumEvents);
      if (bosNumEvents > 0)
	NextEvent();
      else 
	UpdateEventDialog();
    }
  else
    NextEvent();
  
  fprintf(stderr, "file opened\n");
}


/*------ CloseOutputFile --------*/

void CloseOutputFile() {
  
  int   iret;
  char *tstr;
  char  mess[256];
  
  fwbos_(bcs_.iw, &bosOutUnit, "0", &iret, 1);
  fprintf(stderr, "return from last buffer write: %d\n", iret);
  
  /*close file*/
  
  sprintf(mess,"CLOSE %s UNIT=%d", bosOutDataName, bosOutUnit);
  fparm_c(mess);
  
  /*    bosClose(bosOutDataName); */ /* close old one */
  
  if (bosOutFileName != NULL) {
    tstr = (char *)Hv_Malloc(strlen(bosOutFileName) + 100);
    sprintf(tstr, "Closed %s", bosOutFileName);
    BroadcastMessage(tstr);
    Hv_Free(bosOutFileName);
    bosOutFileName = NULL;
  }
  
  
  UpdateOutputFileLabel();
  EventButtonSelectability();
}


/*--------- UpdateOutputFileLabel --------*/

static void UpdateOutputFileLabel() {
  
  char  *tstr;
  
  if (filelabel == NULL) {
    return;
  }
  
  if (bosOutFileName == NULL) {
    Hv_ChangeLabel(filelabel, "(no output file)", Hv_fixed2);
  }
  else {
    tstr = (char *)Hv_Malloc(strlen(bosOutFileName) + 100);
    sprintf(tstr, "Output File:  %s", bosOutFileName);
    Hv_ChangeLabel(filelabel, tstr, Hv_fixed2);
    Hv_Free(tstr);
  }
}

/*-------- OpenOutputFile ---------*/

void OpenOutputFile(void) {
  
  /* this is the "callback" for opening a file for dumping events*/
  
  char  *tstr;
  char  *fn;
  
  fn = Hv_FileSelect("Select an OUTPUT Event File",
		     eventFileFilter,
		     "");
  
  Hv_ReplaceNewLine(fn);
  
  if ((fn == NULL) || (strlen(fn) < 1)) {
    Hv_Warning("No file selected... Nothing has changed.");
    return;
  }
  
  /* ok, we gots a good one */
  
  bosClose(bosOutDataName); /* close old one */
  
  bosOpen(bosOutDataName, bosOutUnit, fn, 1);    /* open the event file */
  
  if (!goodWriteOpen) {
    UpdateOutputFileLabel();
    return;   /* cave, leaving us in whatever state we were in */
  }
  
  /* OK we seem to have a file ready for writing */
  
  Hv_Free(bosOutFileName);
  
  Hv_InitCharStr(&bosOutFileName, fn);
  
  tstr = (char *)Hv_Malloc(strlen(bosOutFileName) + 100);
  sprintf(tstr, "Output file: %s", bosOutFileName);
  
  BroadcastMessage(tstr);
  
  Hv_Free(fn);
  UpdateOutputFileLabel();
  EventButtonSelectability();
}


/*-------- WriteEvent ---------*/

void WriteEvent(void)
     
     /* this is the "callback" for writing an event to a file */
     
{
  char   *tstr;
  int     iret;
  int     evn = -1;
  char   *t2;
  char   *moron;
  
  fwbos_(bcs_.iw, &bosOutUnit, "E", &iret, 1);
  
  
  if (bosHEAD != NULL)
    evn = bosHEAD->EventNumber;
  
  if (bosOutFileName != NULL) {
    Hv_InitCharStr(&t2, bosOutFileName);
    moron = t2;
    Hv_StripLeadingDirectory(&moron);
    tstr = (char *)Hv_Malloc(80 + strlen(bosOutFileName));
    sprintf(tstr, "Wrote event %d to %s (iret: %d)", evn, moron, iret);
    Hv_Free(t2);
  }
  else {
    tstr = (char *)Hv_Malloc(80);
    sprintf(tstr, "Wrote event %d to ?? (iret: %d)", evn, iret);
  }
  
  fprintf(stderr, "%s\n", tstr);
  
  BroadcastMessage(tstr);
  Hv_Free(tstr);
  
}

/*-------- ConvertCB ---------*/

static void ConvertCB(void)
     
     /* this is the "callback" for converting a file to noiseless */
     
{
  char  *text = NULL;
  char  *nnfn = NULL;
  char  *deffn = NULL;
  char  *temp;
  
  int   saveevnum;
  
  
  if (bosFileName == NULL)
    return;
  
  
  text = (char *)Hv_Malloc(40 + strlen(bosFileName));
  sprintf(text, "Remove DC noise from file %s?", bosFileName);
  
  if (Hv_Question(text)) {
    
    /* get the output file */
    
    deffn = (char *)(Hv_Malloc(strlen(bosFileName) + 10));
    strcpy(deffn, bosFileName);
    
    Hv_RemoveFileExtension(deffn);
    strcat(deffn, ".nonoise");
    
    temp = deffn;
    
    Hv_StripLeadingDirectory(&temp);
    
    nnfn = Hv_FileSelect("Output File",
			 "*.nonoise",
			 temp);
    
    
    if (nnfn == NULL) {
      fprintf(stderr, "No event file opened.\n");
      return;
    }
    
    /* check file exists */
    
    if (Hv_FileExists(nnfn)) {
      
      Hv_Free(deffn);  /* reuse */
      deffn = (char *)(Hv_Malloc(strlen(nnfn) + 30));
      sprintf(deffn, "Overwrite existing %s?", nnfn);
      
      if (!Hv_Question(deffn)) {
	Hv_Free(deffn);
	Hv_Free(nnfn);
	return;
      }
      
    }
    
    
    /* ready to open */
    
    
    /* cache the present event number so that we can return */
    
    saveevnum = eventCount;
    
    /* now return to the event we were on */
    
    GotoEventNumber(saveevnum);
    
  }
  
  Hv_Free(nnfn);
  Hv_Free(deffn);
  Hv_Free(text);
}

/*-------- OpenEventCB ---------*/

void OpenEventCB(void)
     
     /******** this is the "callback" **********/
     
{
  char  *fn;
  char  *defdir;
  
  static Boolean  firsttime = True;
  
  if (firsttime) {
    firsttime = False;
    defdir = (char *)getenv("CEDDATADIR");
    if (defdir != NULL) {
      fprintf(stderr, "Looking for event files in [%s]\n", defdir);
      
      if (Hv_cachedDirectory != NULL) {
	Hv_Free(Hv_cachedDirectory);
      }
      Hv_InitCharStr(&Hv_cachedDirectory, defdir);
    }
    else {
      fprintf(stderr, "NULL CEDDATADIR environment variable\n");
    }
    
  }
  
  
  fn = Hv_FileSelect("Select an Event File",
		     eventFileFilter,
		     " ");
  
  Hv_ReplaceNewLine(fn);
  
  if (fn == NULL) {
    fprintf(stderr, "No event file opened.\n");
    return;
  }
  
  OpenEventFile(fn);
  Hv_Free(fn);
}

/*------ UpdateEventDialog -------*/

void UpdateEventDialog(void)
     
{
  char    text[60];
  int     esize;
  
  EventButtonSelectability();
  
  if (evdialog == NULL)
    return;
  
  /* first the number of events */
  
  if (!precountEvents) 
    sprintf(text, "N/A");
  else
    sprintf(text, "%d", bosNumEvents);
  
  Hv_SetString(evcount, text);
  
  esize = EventSize();
  
  if (esize > 0) {
    sprintf(text, "%d bytes", esize);
    Hv_SetString(evsize, text);
  }
  else
    Hv_SetString(evsize, " ");
  
  if (bosHEAD != NULL) {
    sprintf(text, "%d", bosHEAD->EventNumber);
    Hv_SetString(evnumber, text);
    
    EventTypeString(bosHEAD->EventType, text);
    Hv_SetString(evtype, text);
    
  }
  else {
    Hv_SetString(evnumber, " ");
    Hv_SetString(evtype, " ");
  }
  
  
  if (precountEvents)
    Hv_SetScaleMinMax(evscale, 0, Hv_iMax(2, bosNumEvents-1));
  else
    Hv_SetScaleMinMax(evscale, 0, Hv_iMax(2, eventCount - 1));
  
  Hv_SetScaleValue(evscale, Hv_iMax(0, eventCount-1));
  
  
  if ((EventSize() > 0) && (evdialog != NULL)) {
    if (lastbank == NULL)
      BankCB(evhead);
    else
      BankCB(lastbank);
  }
  
  /* output file label */
  
  UpdateOutputFileLabel();
}


/*------- LargetsECSumSector -------*/

short LargestECSumSector(void)
     
     /* returns sect with largest ec adc sum, NOT AS
	A CINDEX. If no event, returns -1 */
     
{
  
  short  i, largest;
  int    sum, newsum;
  
  if (EventSize() <= 0)
    return -1;
  
  
  largest = 0;
  sum = -1;
  
  
  for (i = 0; i < 6; i++) {
    newsum = TotalECADCSum(i);
    if (newsum > sum) {
      sum = newsum;
      largest = i;
    }
    
  }
  
  /* so far mostds is a C index */
  
  largest = largest % 3;  /* only return the "top" sector */
  return largest + 1;    /* convert to NON C index */
}


/*------- MostDataSector -------*/

short MostDataSector(void)
     
     /* returns sect with most data, NOT AS
	A CINDEX. If no event, returns -1 */
     
{
  short  i, mostds;
  
  if (EventSize() <= 0)
    return -1;
  
  mostds = 0;
  for (i = 1; i < 6; i++) {
    if (bosSectorDataSize[i] > bosSectorDataSize[mostds])
      mostds = i;
  }
  
  /* so far mostds is a C index */
  
  mostds = mostds % 3;  /* only return the "top" sector */
  return mostds + 1;    /* convert to NON C index */
}

/*------ FixLargestECSumViews -------*/

void FixLargestECSumViews(void) {
  
}



/*------ FixMostDataViews -------*/

void FixMostDataViews(void)
     
{
  short      mostds;
  ViewData   viewdata;
  Hv_View    temp;
  short      oldvs, newvs;
  Hv_FRect   world;
  
  mostds = -MostDataSector();
  
  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) 
    if (temp->tag == SECTOR) {
      viewdata = GetViewData(temp);
      oldvs = viewdata->visiblesectors;
      
      /* if this is a most data sector view (flagged by a negative
	 vs) AND the most data sector has changed, then we need
	 to fix the title */
      
      if ((oldvs < 0) && (oldvs != mostds)) {
	viewdata->visiblesectors = mostds;
	newvs = abs(mostds) - 1;
	
	viewdata->phi = Hv_DEGTORAD*(60.0*newvs + (float)(viewdata->dphi));
	SetSelectability(temp);
	UpdateViewTitle(temp);
	SwitchSectorGeometry(temp);
	
	/* switch the "worlds" */
	
	oldvs = abs(oldvs) - 1;
	
	Hv_LocalRectToWorldRect(temp, temp->hotrect, &(viewdata->worlds[oldvs]));
	world = viewdata->worlds[newvs];
	Hv_QuickZoomView(temp, world.xmin, world.xmax, world.ymin, world.ymax);
      }
    }
  
  
}


/*------ bosGetBankIndices ------*/

void  bosGetBankIndices(void) {
  int          i, j, k;
  int          banksize;
  SCDataPtr    hits; /* fix for SC ids for new indices up to 57 */
  
  bosZeroBankIndices();
  
  for (i = 0; i < 6; i++) {
    j = i + 1;
    bosIndexDC0[i] = mlink_(bcs_.iw,bosBankNameDC0, &j, 4);
    bosIndexDC1[i] = mlink_(bcs_.iw,bosBankNameDC1, &j, 4);
    bosIndexSEG[i] = mlink_(bcs_.iw,bosBankNameSEG, &j, 4);
    bosIndexCC[i]  = mlink_(bcs_.iw,bosBankNameCC,  &j, 4);
    bosIndexCC1[i] = mlink_(bcs_.iw,bosBankNameCC1, &j, 4);
    bosIndexSC[i]  = mlink_(bcs_.iw,bosBankNameSC,  &j, 4);
    bosIndexEC[i]  = mlink_(bcs_.iw,bosBankNameEC,  &j, 4);
    bosIndexEC1[i] = mlink_(bcs_.iw,bosBankNameEC1, &j, 4);
    
    /* hit based banks */
    
    bosIndexHBLA[i] = mlink_(bcs_.iw,bosBankNameHBLA, &j, 4);
    bosIndexHDPL[i] = mlink_(bcs_.iw,bosBankNameHDPL, &j, 4);
    
    /* time based banks */
    
    bosIndexTBLA[i] = mlink_(bcs_.iw,bosBankNameTBLA, &j, 4);
    bosIndexTDPL[i] = mlink_(bcs_.iw,bosBankNameTDPL, &j, 4);
    
    bosSectorHasData[i] = ((bosIndexDC0[i] + bosIndexDC1[i] +
			    bosIndexSEG[i] + 
			    bosIndexCC[i]  + bosIndexCC1[i] + 
			    bosIndexSC[i]  + bosIndexEC[i]  +
			    bosIndexEC1[i] + bosIndexHBLA[i] +
			    bosIndexHDPL[i] + bosIndexTBLA[i] +
			    bosIndexTDPL[i]) > 0);
    
    
    bosSectorDataSize[i] = 0;
    
    if (bosSectorHasData[i]) {
      
      /* Drift chamber (DC1) data? */
      
      if (bosIndexDC1[i] != 0) {
	banksize = bosBankSize(bosIndexDC1[i]);
	bosNumDC1[i] = banksize/sizeof(DC1Data);
	bosDC1[i]    = (DC1DataPtr)(&(bcs_.iw[bosIndexDC1[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /* NEW Drift chamber (DC0) data? */
      
      if (bosIndexDC0[i] != 0) {
	banksize = bosBankSize(bosIndexDC0[i]);
	bosNumDC0[i] = banksize/sizeof(DC0Data);
	bosDC0[i]    = (DC0DataPtr)(&(bcs_.iw[bosIndexDC0[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      
      /* Level 2 (SEG) data? */
      
      if (bosIndexSEG[i] != 0) {
	banksize = bosBankSize(bosIndexSEG[i]);
	bosNumSEG[i] = banksize/sizeof(SEGData);
	bosSEG[i]    = (SEGDataPtr)(&(bcs_.iw[bosIndexSEG[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /*  scintillators data? */
      
      if (bosIndexSC[i] != 0) {
	banksize = bosBankSize(bosIndexSC[i]);
	bosNumSC[i] = banksize/sizeof(SCData);
	bosSC[i]    = (SCDataPtr)(&(bcs_.iw[bosIndexSC[i]]));
	
	bosSectorDataSize[i] += banksize;
	
	/* fix indices */
	
	hits = bosSC[i];
	for (k = 0; k < bosNumSC[i]; k++) {
	  /* remove high byte so new data (index up to 57) works */
	  hits->id = LowByte(hits->id);
	  hits++;
	}
      }
      
      /* forward angle cerenkov data? */
      
      if (bosIndexCC[i] != 0) {
	banksize = bosBankSize(bosIndexCC[i]);
	bosNumCC[i] = banksize/sizeof(NormalData);
	bosCC[i]    = (NormalDataPtr)(&(bcs_.iw[bosIndexCC[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /* back angle cerenkov data? */
      
      if (bosIndexCC1[i] != 0) {
	banksize = bosBankSize(bosIndexCC1[i]);
	bosNumCC1[i] = banksize/sizeof(NormalData);
	bosCC1[i]    = (NormalDataPtr)(&(bcs_.iw[bosIndexCC1[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /* forward angle EC data? */
      
      if (bosIndexEC[i] != 0) {
	banksize = bosBankSize(bosIndexEC[i]);
	bosNumEC[i] = banksize/sizeof(ECData);
	bosEC[i]    = (ECDataPtr)(&(bcs_.iw[bosIndexEC[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /* back angle EC data? */
      
      if (bosIndexEC1[i] != 0) {
	banksize = bosBankSize(bosIndexEC1[i]);
	bosNumEC1[i] = banksize/sizeof(EC1Data);
	bosEC1[i]    = (EC1DataPtr)(&(bcs_.iw[bosIndexEC1[i]]));
	bosSectorDataSize[i] += banksize;
      }
      
      /* HBLA Data? */
      
      if (bosIndexHBLA[i] != 0) {
	banksize = bosBankSize(bosIndexHBLA[i]);
	bosNumHBLA[i] = banksize/sizeof(HBLAData);
	bosHBLA[i]    = (HBLADataPtr)(&(bcs_.iw[bosIndexHBLA[i]]));
      }
      
      /* HDPL Data? */
      
      if (bosIndexHDPL[i] != 0) {
	banksize = bosBankSize(bosIndexHDPL[i]);
	bosNumHDPL[i] = banksize/sizeof(HDPLData);
	bosHDPL[i]    = (HDPLDataPtr)(&(bcs_.iw[bosIndexHDPL[i]]));
      }
      
      
      /* TBLA Data? */
      
      if (bosIndexTBLA[i] != 0) {
	banksize = bosBankSize(bosIndexTBLA[i]);
	bosNumTBLA[i] = banksize/sizeof(TBLAData);
	bosTBLA[i]    = (TBLADataPtr)(&(bcs_.iw[bosIndexTBLA[i]]));
      }
      
      /* TDPL Data? */
      
      if (bosIndexTDPL[i] != 0) {
	banksize = bosBankSize(bosIndexTDPL[i]);
	bosNumTDPL[i] = banksize/sizeof(TDPLData);
	bosTDPL[i]    = (TDPLDataPtr)(&(bcs_.iw[bosIndexTDPL[i]]));
      }
      
      
    } /* end sector has data */
    
  }
  
  /* Stepan's new EC recon bank? */
  
  j = 0;
  bosIndexECPI  = mlink_(bcs_.iw,bosBankNameECPI,  &j, 4);
  if (bosIndexECPI != 0) {
    banksize = bosBankSize(bosIndexECPI);
    bosNumECPI = banksize/sizeof(PiData);
    bosECPI = (PiDataPtr)(&(bcs_.iw[bosIndexECPI]));
  }
  
  /* new start counter banks? */
  
  
  j = 0;
  bosIndexSTN0 = mlink_(bcs_.iw,bosBankNameSTN0, &j, 4);
  
  if (bosIndexSTN0 != 0) {
    banksize = bosBankSize(bosIndexSTN0);
    bosNumSTN0 = banksize/sizeof(STN0Data);
    bosSTN0 = (STN0DataPtr)(&(bcs_.iw[bosIndexSTN0]));
  }
  else {
    j = 1;
    bosIndexSTN0 = mlink_(bcs_.iw,bosBankNameSTN0, &j, 4);
    
    if (bosIndexSTN0 != 0) {
      banksize = bosBankSize(bosIndexSTN0);
      bosNumSTN0 = banksize/sizeof(STN0Data);
      bosSTN0 = (STN0DataPtr)(&(bcs_.iw[bosIndexSTN0]));
    }
  }
  
  
  j = 0;
  bosIndexSTN1 = mlink_(bcs_.iw,bosBankNameSTN1, &j, 4);
  
  if (bosIndexSTN1 != 0) {
    banksize = bosBankSize(bosIndexSTN1);
    bosNumSTN1 = banksize/sizeof(STN1Data);
    bosSTN1 = (STN1DataPtr)(&(bcs_.iw[bosIndexSTN1]));
  }
  else {
    j = 1;
    bosIndexSTN1 = mlink_(bcs_.iw,bosBankNameSTN1, &j, 4);
    
    if (bosIndexSTN1 != 0) {
      banksize = bosBankSize(bosIndexSTN1);
      bosNumSTN1 = banksize/sizeof(STN1Data);
      bosSTN1 = (STN1DataPtr)(&(bcs_.iw[bosIndexSTN1]));
    }
  }
  
  
  /* tagger E  data ? */
  
  j = 0;
  bosIndexTAGE = mlink_(bcs_.iw,bosBankNameTAGE, &j, 4);
  if (bosIndexTAGE != 0) {
    banksize = bosBankSize(bosIndexTAGE);
    bosNumTAGE = banksize/sizeof(TAGEData);
    bosTAGE = (TAGEDataPtr)(&(bcs_.iw[bosIndexTAGE]));
  }
  else {
    j = 1;
    bosIndexTAGE = mlink_(bcs_.iw,bosBankNameTAGE, &j, 4);
    if (bosIndexTAGE != 0) {
      banksize = bosBankSize(bosIndexTAGE);
      bosNumTAGE = banksize/sizeof(TAGEData);
      bosTAGE = (TAGEDataPtr)(&(bcs_.iw[bosIndexTAGE]));
    }
  }
  
  
  /* tagger E  data ? */
  
  j = 0;
  bosIndexTAGT = mlink_(bcs_.iw,bosBankNameTAGT, &j, 4);
  if (bosIndexTAGT != 0) {
    banksize = bosBankSize(bosIndexTAGT);
    bosNumTAGT = banksize/sizeof(TAGTData);
    bosTAGT = (TAGTDataPtr)(&(bcs_.iw[bosIndexTAGT]));
  }
  else {
    j = 1;
    bosIndexTAGT = mlink_(bcs_.iw,bosBankNameTAGT, &j, 4);
    if (bosIndexTAGT != 0) {
      banksize = bosBankSize(bosIndexTAGT);
      bosNumTAGT = banksize/sizeof(TAGTData);
      bosTAGT = (TAGTDataPtr)(&(bcs_.iw[bosIndexTAGT]));
    }
  }
  
  
  /* header bank? */
  
  j = 0;
  bosIndexHEAD = mlink_(bcs_.iw,bosBankNameHEAD, &j, 4);
  if (bosIndexHEAD != 0)
    {
      bosHEAD = (HEADDataPtr)(&(bcs_.iw[bosIndexHEAD]));
      
      /* re init geometry? */
      
      if(bosHEAD != NULL)
	{
	  if (runnumber != bosHEAD->RunNumber)
	    {
	      runnumber = bosHEAD->RunNumber;
	      ReInitGeometry();
	    }
	}
    }
  else
    {
      /* if no head bank, see if it is an old style event */
      j = 0;
      bosIndexHEAD = mlink_(bcs_.iw,bosBankNameHEAD, &j, 4);
      if (bosIndexHEAD != 0)
	bosHEAD = (HEADDataPtr)(&(bcs_.iw[bosIndexHEAD]));
    }
  
  /* TGBI bank? */
  
  j = 0;
  bosIndexTGBI = mlink_(bcs_.iw,bosBankNameTGBI, &j, 4);
  if (bosIndexTGBI != 0)
    bosTGBI = (TGBIDataPtr)(&(bcs_.iw[bosIndexTGBI]));
  
  /* Hit based track parameter banks ? */
  
  j = 0;
  bosIndexHBTR = mlink_(bcs_.iw,bosBankNameHBTR, &j, 4);
  
  if (bosIndexHBTR != 0) {
    banksize = bosBankSize(bosIndexHBTR);
    bosNumHBTR = banksize/sizeof(HBTRData); 
    bosHBTR = (HBTRDataPtr)(&(bcs_.iw[bosIndexHBTR]));
  }
  
  
  /* Time based track parameter banks ? */
  
  j = 0;
  bosIndexTBTR = mlink_(bcs_.iw,bosBankNameTBTR, &j, 4);
  
  if (bosIndexTBTR != 0) {
    banksize = bosBankSize(bosIndexTBTR);
    bosNumTBTR = banksize/sizeof(TBTRData); 
    bosTBTR = (TBTRDataPtr)(&(bcs_.iw[bosIndexTBTR]));
  }
  
  
  /* part banks ? */
  
  j = 1;
  bosIndexPART = mlink_(bcs_.iw,bosBankNamePART, &j, 4);
  
  if (bosIndexPART != 0) {
    banksize = bosBankSize(bosIndexPART);
    bosNumPART = banksize/sizeof(PARTData); 
    bosPART = (PARTDataPtr)(&(bcs_.iw[bosIndexPART]));
  }
  
  
  /* Monte Carlo Input Data Banks? */
  
  j = 0;
  bosIndexMCIN = mlink_(bcs_.iw,bosBankNameMCIN, &j, 4);
  
  if (bosIndexMCIN != 0) {
    banksize = bosBankSize(bosIndexMCIN);
    bosNumMCIN = banksize/sizeof(MCINData);
    bosMCIN = (MCINDataPtr)(&(bcs_.iw[bosIndexMCIN]));
  }
  
  j = 0;
  bosIndexMCVX = mlink_(bcs_.iw,bosBankNameMCVX, &j, 4);
  
  if (bosIndexMCVX != 0) {
    banksize = bosBankSize(bosIndexMCVX);
    bosNumMCVX = banksize/sizeof(MCVXData);
    bosMCVX = (MCVXDataPtr)(&(bcs_.iw[bosIndexMCVX]));
  }
  
  j = 0;
  bosIndexMCTK = mlink_(bcs_.iw,bosBankNameMCTK, &j, 4);
  
  if (bosIndexMCTK != 0) {
    banksize = bosBankSize(bosIndexMCTK);
    bosNumMCTK = banksize/sizeof(MCTKData);
    bosMCTK = (MCTKDataPtr)(&(bcs_.iw[bosIndexMCTK]));
  }
}


/*------- EventTypeString ------*/

void EventTypeString(int  evtype,
		     char  *line) {
  if (evtype == -3)
    sprintf(line, "CLASSim");
  else if (evtype == -2)
    sprintf(line, "GEANT");
  else if (evtype == -1)
    sprintf(line, "SDA");
  
  else if (evtype == 16)
    sprintf(line, "Sync Event");
  else if (evtype == 17)
    sprintf(line, "Prestart Event");
  else if (evtype == 18)
    sprintf(line, "Go Event");
  else if (evtype == 19)
    sprintf(line, "Pause");
  else if (evtype == 20)
    sprintf(line, "End");
  else if ((evtype > 0) && (evtype < 16))
    sprintf(line, "Phys Type %d", evtype);
  else
    sprintf(line, "%d", evtype);
}


/*-------- EventSize -------*/

int EventSize(void)
     
     /* return size of event in bytes */
     
{
  int    esize = 0;
  int    i;
  
  esize += bosBankSize(bosIndexHEAD);
  esize += bosBankSize(bosIndexTAGE);
  esize += bosBankSize(bosIndexTAGT);
  esize += bosBankSize(bosIndexSTN0);
  esize += bosBankSize(bosIndexSTN1);
  esize += bosBankSize(bosIndexMCIN);
  esize += bosBankSize(bosIndexMCVX);
  esize += bosBankSize(bosIndexMCTK);
  esize += bosBankSize(bosIndexECPI);
  
  for (i = 0; i < 6; i++) {
    esize += bosBankSize(bosIndexDC0[i]);
    esize += bosBankSize(bosIndexDC1[i]);
    esize += bosBankSize(bosIndexSEG[i]);
    esize += bosBankSize(bosIndexSC[i]);
    esize += bosBankSize(bosIndexEC[i]);
    esize += bosBankSize(bosIndexEC1[i]);
    esize += bosBankSize(bosIndexEC[i]);
    esize += bosBankSize(bosIndexCC1[i]);
  }
  
  return  esize;
}


/*-------- GotoEventNumber ---------*/

static void GotoEventNumber(int num)
     
{
  int  i;
  
  if (num <= 0)
    return;
  
  if (precountEvents && (num > bosNumEvents)) 
    return;
  
  
  /* erase old event before the bos banks are changed */
  
  EraseEvent();
  
  
  eventCount = num; 
  
  bosRewind(bosDataName);
  
  for (i = 0; i < num; i++)
    {
      if(!ReadNextEventFromFile()) break;
    }
  
  NextEvent();
}

/*------- ScaleCB --------*/

static void ScaleCB(Hv_Widget w)
     
{
  GotoEventNumber(Hv_GetScaleValue(w));
}

/*------- BankCB --------*/

static void BankCB(Hv_Widget w)
     
{
  Boolean  found = False;
  
  Hv_ClearScrolledText(evtext, &evpos);
  
  lastbank = w;
  
  if (w == evhead)
    DumpHEADBank(bosHEAD);
  
  else if (w == evtage) {
    DumpTAGEBank(bosTAGE, bosNumTAGE);
  }
  
  else if (w == evtagt) {
    DumpTAGTBank(bosTAGT, bosNumTAGT);
  }
  
  else if (w == evhbtr)
    DumpHBTRBank(bosHBTR, bosNumHBTR);
  
  else if (w == evtbtr)
    DumpTBTRBank(bosTBTR, bosNumTBTR);
  
  else if (w == evpart)
    DumpPARTBank(bosPART, bosNumPART);
  
  else if (w == evstn0){
    DumpSTN0Bank(bosSTN0, bosNumSTN0);
  }
  
  else if (w == evstn1){
    DumpSTN1Bank(bosSTN1, bosNumSTN1);
  }
  
  else if (w == evmcin)
    DumpMCINBank(bosMCIN, bosNumMCIN);
  
  else if (w == evmcvx)
    DumpMCVXBank(bosMCVX, bosNumMCVX);
  
  else if (w == evmctk)
    DumpMCTKBank(bosMCTK, bosNumMCTK);
  
  else if (w == evecpi)
    DumpECPIBank(bosECPI, bosNumECPI);
  
  else {
    found = DC0BankCB(w);
    if (!found)
      found = DC1BankCB(w);
    if (!found)
      found = SEGBankCB(w);
    if (!found)
      found = SCBankCB(w);
    if (!found)
      found = ECBankCB(w);
    if (!found)
      found = EC1BankCB(w);
    if (!found)
      found = CCBankCB(w);
    if (!found)
      found = CC1BankCB(w);
    if (!found)
      found = HBLABankCB(w);
    if (!found)
      found = HDPLBankCB(w);
    if (!found)
      found = TBLABankCB(w);
    if (!found)
      found = TDPLBankCB(w);
  }
}

/*----- bosZeroBankIndices -------*/

static void bosZeroBankIndices(void)
     
     /* zeroes the global indices AND NULLs data pointers.
	The pointers  do not have to be freed because they
	are never alloced -- they simply are cast to
	various places in the bos buffer */
     
{
  int    i;
  
  for (i = 0; i < 6; i++) {
    
    /* zero the indices */
    
    bosIndexDC0[i] = 0;
    bosIndexDC1[i] = 0;
    bosIndexSEG[i] = 0;
    bosIndexCC[i]  = 0;
    bosIndexCC1[i] = 0;
    bosIndexSC[i]  = 0;
    bosIndexEC[i]  = 0;
    bosIndexEC1[i] = 0;
    
    bosIndexHBLA[i] = 0;
    bosIndexHDPL[i] = 0;
    bosIndexTBLA[i] = 0;
    bosIndexTDPL[i] = 0;
    
    /* NULL the data pointers */
    
    bosDC0[i] = NULL;
    bosDC1[i] = NULL;
    bosSEG[i] = NULL;
    bosCC[i]  = NULL;
    bosCC1[i] = NULL;
    bosSC[i]  = NULL;
    bosEC[i]  = NULL;
    bosEC1[i] = NULL;
    
    bosHBLA[i] = NULL;
    bosHDPL[i] = NULL;
    bosTBLA[i] = NULL;
    bosTDPL[i] = NULL;
    
    /* zero the hit counters */
    
    bosNumDC0[i] = 0;
    bosNumDC1[i] = 0;
    bosNumSEG[i] = 0;
    bosNumCC[i]  = 0;
    bosNumCC1[i] = 0;
    bosNumSC[i]  = 0;
    bosNumEC[i]  = 0;
    bosNumEC1[i] = 0;
    
    bosNumHBLA[i] = 0;
    bosNumHDPL[i] = 0;
    bosNumTBLA[i] = 0;
    bosNumTDPL[i] = 0;
    
    bosSectorHasData[i] = False;
  }
  
  /* similarly for one only banks */
  
  bosIndexTAGT = 0;
  bosIndexTAGE = 0;
  bosIndexHEAD = 0;
  bosIndexSTN0 = 0;
  bosIndexSTN1 = 0;
  bosIndexMCIN = 0;
  bosIndexMCVX = 0;
  bosIndexMCTK = 0;
  bosIndexECPI = 0;
  
  /* null the  hit pointers */
  
  bosTAGE     = NULL;
  bosTAGT     = NULL;
  bosHEAD     = NULL;
  bosMCIN     = NULL;
  bosMCVX     = NULL;
  bosMCTK     = NULL;
  bosECPI     = NULL;
  
  
  bosNumECPI = 0;
  bosNumTAGT = 0;
  bosNumTAGE = 0;
  bosNumSTN0 = 0;
  bosNumSTN1 = 0;
  bosNumMCIN = 0;
  bosNumMCVX = 0;
  bosNumMCTK = 0;
  
  /* for tracks */
  
  bosIndexHBTR = 0;
  bosHBTR = NULL;
  bosNumHBTR = 0;
  
  bosIndexTBTR = 0;
  bosTBTR = NULL;
  bosNumTBTR = 0;
  
  bosIndexPART = 0;
  bosPART = NULL;
  bosNumPART = 0;
  
}

/*-------- StartEventTimer --------------*/

void StartEventTimer()
     
{
  
  StopEventTimer();
  eventinterval = XtAppAddTimeOut(Hv_appContext,
				  eventtime*1000,
				  (XtTimerCallbackProc)EventTimeOut,
				  NULL); 
}

/*------ EventTimeOut ----------*/

static void EventTimeOut(void) {
  
  
  if (!nextisactive)
    fprintf(stderr, "(EventTimeOut) No source of events\n");
  else
    NextEvent();
  
  eventinterval = XtAppAddTimeOut(Hv_appContext,
				  eventtime*1000,
				  (XtTimerCallbackProc)EventTimeOut,
				  NULL); 
  
}

/*-------- StopEventTimer ---------------*/

void StopEventTimer()
     
{
  if (eventinterval != 0) {
    XtRemoveTimeOut(eventinterval);
    eventinterval = 0;
  }
}

/*-------- EventButtonSelectability ----------*/

void  EventButtonSelectability(void)
     
     /* set the buttons on the event controller to their correct selectability */
     
{
  int      i;
  Boolean  fileok, fileopen;
  char     text[30];
  
  nextisactive = False;
  
  fileopen = (bosFileName != NULL);
  
  if (precountEvents)
    fileok = fileopen && (bosNumEvents > 1) && (eventCount < bosNumEvents);
  else
    fileok = fileopen;
  
  XtVaSetValues(evfromfile,   XmNset, eventsource == CED_FROMFILE, NULL);
  XtVaSetValues(evfromet,     XmNset, eventsource == CED_FROMET, NULL);
  
  XtVaSetValues(evondemand,    XmNset, eventwhen == CED_ONDEMAND, NULL);
  XtVaSetValues(evontimer,     XmNset, eventwhen == CED_ONTIMER,  NULL);
  sprintf(text, "Event every %d seconds", eventtime);
  Hv_SetMenuItemString(evontimer, text, Hv_DEFAULT);
  
  
  if (eventsource == CED_FROMET) {
    nextisactive = etisok;
    Hv_SetSensitivity(writeAccEvTB, etisok);
    Hv_SetSensitivity(physicsOnly , etisok);
    Hv_SetSensitivity(evaccumulate, etisok);
    Hv_SetSensitivity(evnextmenu,  nextisactive);
    Hv_SetSensitivity(evprevmenu,  False);
    Hv_SetSensitivity(evgotomenu,  False);
  }
  
  
  else {
    nextisactive = fileok;
    Hv_SetSensitivity(writeAccEvTB,  fileok);
    Hv_SetSensitivity(physicsOnly,   fileok);
    Hv_SetSensitivity(evaccumulate,  fileok);
    Hv_SetSensitivity(evnextmenu,    nextisactive);
    Hv_SetSensitivity(evprevmenu,   fileok);
    
    if (precountEvents)
      Hv_SetSensitivity(evgotomenu, (bosNumEvents > 1));
    else
      Hv_SetSensitivity(evgotomenu,  True);
  }
  
  
  /* can we write? */
  
  /*    fprintf(stderr, "bof = %p    bih = %d\n", bosOutFileName, bosIndexHEAD); */
  
  Hv_SetSensitivity(evwritemenu,
		    (bosOutFileName != NULL) && (bosIndexHEAD > 0));
  
  Hv_SetSensitivity(evclosemenu, (bosOutFileName != NULL));
  
  /******** now for the event control dialog **********/
  
  if (evdialog == NULL)
    return;
  
  Hv_SetSensitivity(evnext,  nextisactive);
  
  if (eventsource == CED_FROMET) {
    Hv_SetSensitivity(evscale, False);
    Hv_SetSensitivity(evprev,  False);
    Hv_SetSensitivity(evgoto,  False);
    Hv_SetSensitivity(evopen,  False);
    Hv_SetSensitivity(evconvert,  False);
  }
  else {
    Hv_SetSensitivity(evprev,  fileok);
    
    /* disable convert for this release */
    /*	Hv_SetSensitivity(evconvert,  fileopen); */
    
    Hv_SetSensitivity(evconvert,  False);
    
    
    if (precountEvents) {
      Hv_SetSensitivity(evscale, (bosNumEvents > 1));
      Hv_SetSensitivity(evgoto,  (bosNumEvents > 1));
    }
    else {
      Hv_SetSensitivity(evscale, fileok);
      Hv_SetSensitivity(evgoto,  fileok);
    }
    
    Hv_SetSensitivity(evopen,  True);
  }
  
  Hv_SetSensitivity(evecpi, (bosIndexECPI > 0));
  Hv_SetSensitivity(evhead, (bosIndexHEAD > 0));
  Hv_SetSensitivity(evtage, (bosIndexTAGE > 0));
  Hv_SetSensitivity(evtagt, (bosIndexTAGT > 0));
  Hv_SetSensitivity(evstn0, (bosIndexSTN0 > 0));
  Hv_SetSensitivity(evstn1, (bosIndexSTN1 > 0));
  Hv_SetSensitivity(evmcin, (bosIndexMCIN > 0));
  Hv_SetSensitivity(evmcvx, (bosIndexMCVX > 0));
  Hv_SetSensitivity(evmctk, (bosIndexMCTK > 0));
  
  /* track related widgets */
  
  Hv_SetSensitivity(evhbtr,   (bosIndexHBTR > 0));
  Hv_SetSensitivity(evtbtr,   (bosIndexTBTR > 0));
  Hv_SetSensitivity(evpart,   (bosIndexPART > 0));
  
  LabelSensitivity(dc0lab,  SomeSectorHasData(bosIndexDC0));
  LabelSensitivity(dc1lab,  SomeSectorHasData(bosIndexDC1));
  LabelSensitivity(seglab,  SomeSectorHasData(bosIndexSEG));
  LabelSensitivity(sclab,   SomeSectorHasData(bosIndexSC));
  LabelSensitivity(cclab,   SomeSectorHasData(bosIndexCC));
  LabelSensitivity(cc1lab,  SomeSectorHasData(bosIndexCC1));
  LabelSensitivity(eclab,   SomeSectorHasData(bosIndexEC));
  LabelSensitivity(ec1lab,  SomeSectorHasData(bosIndexEC1));
  LabelSensitivity(hblalab, SomeSectorHasData(bosIndexHBLA));
  LabelSensitivity(hdpllab, SomeSectorHasData(bosIndexHDPL));
  LabelSensitivity(tblalab, SomeSectorHasData(bosIndexTBLA));
  LabelSensitivity(tdpllab, SomeSectorHasData(bosIndexTDPL));
  
  for (i = 0; i < 6; i++) {
    Hv_SetSensitivity(evdc0[i],  (bosIndexDC0[i]  > 0));
    Hv_SetSensitivity(evdc1[i],  (bosIndexDC1[i]  > 0));
    Hv_SetSensitivity(evseg[i],  (bosIndexSEG[i]  > 0));
    Hv_SetSensitivity(evsc[i],   (bosIndexSC[i]   > 0));
    Hv_SetSensitivity(evec[i],   (bosIndexEC[i]   > 0));
    Hv_SetSensitivity(evec1[i],  (bosIndexEC1[i]  > 0));
    Hv_SetSensitivity(evcc[i],   (bosIndexCC[i]   > 0));
    Hv_SetSensitivity(evcc1[i],  (bosIndexCC1[i]  > 0));
    Hv_SetSensitivity(evhbla[i], (bosIndexHBLA[i] > 0));
    Hv_SetSensitivity(evhdpl[i], (bosIndexHDPL[i] > 0));
    Hv_SetSensitivity(evtbla[i], (bosIndexTBLA[i] > 0));
    Hv_SetSensitivity(evtdpl[i], (bosIndexTDPL[i] > 0));
  }
}

/*------- SomeSectorHasData ------*/

static Boolean SomeSectorHasData(int *Idx)
     
{
  int   i;
  
  for (i = 0; i < 6; i++)
    if (Idx[i] > 0)
      return True;
  
  return False;
}


/* ------- bosOpen ---------*/

static void bosOpen(char  *dataname,
		    int    unitnum,
		    char  *filename,
		    int    option)
     
     /*  *dataname   name used by subsequent fpack routines to access
	 unitnum    fortran io unit number
	 *filename   actual filename
	 
	 option = 0, open for reading, else write
	 
     */
     
     /* open the file via FPACK */
{
  char command[1024];
  
  
  if (filename == NULL)
    return;
  
  
  if (option == 0) { /*reading */
    sprintf(command, "OPEN %s UNIT=%d FILE=\"%s\" READ STATUS=OLD",
	    dataname, unitnum, filename);
  }
  else { /* writing or appending */
    goodWriteOpen = True;
    
    if (Hv_FileExists(filename)) { /*appending */
      fprintf(stderr, "Appending.\n");
      sprintf(command,
	      "OPEN %s UNIT=%d FILE=\"%s\" ACTION=READWRITE FORM=BINARY STATUS=NEW ACCESS=SEQ RECL=3600", 
	      dataname, unitnum, filename);
    }
    else { /* creating */
      fprintf(stderr, "Creating new file.\n");
      sprintf(command, "OPEN %s UNIT=%d FILE=\"%s\" WRITE STATUS=NEW RECL=3600",
	      dataname, unitnum, filename);
    }
    
  }
  
  
  if (fparm_c(command)) {
    fprintf(stderr, "Successful open.\n");
    if (option != 0)
      goodWriteOpen = True;
  }
  else {
    fprintf(stderr, "Open failed.\n");
    if (option != 0)
      goodWriteOpen = False;
  }
  
}

/* ------- bosClose ---------*/

static void    bosClose(char *dataname)
     
     /* dataname  name used by fpack routines to access file */
     
     /* simply close the file via FPACK */
     
{
  char command[1024];
  
  sprintf(command, "CLOSE %s", dataname);
  
  fparm_c(command);   /*call the FPACK interpreter*/
}


/* ------- bosRewind ---------*/

static void    bosRewind(char *dataname)
     
     /* counts the number of events in a bos file and then rewinds it */
     
{
  char command[1024];
  
  sprintf(command, "REWIND %s", dataname);
  
  fparm_c(command);
  
}


/* ------- bosCountEvents ---------*/
/* counts the number of events in a bos file and
   for (option == 0) rewinds it */

static int
bosCountEvents(char *dataname, int   option)
{
  int num = 0;
  
  bosRewind(dataname);
  
  while(True)
    {
      if(!ReadNextEventFromFile()) break;
      num++;
    }
  
  /* do one more get for the "-1" case -- see FPACK manual (Sergey: ???) */
  
  if(option == 0) bosRewind(dataname);
  
  return(num);
}



/*----- bosBankSize -------*/

int  bosBankSize(int index)
     
     /* returns the size in bytes */
     
{
  size_t  intsize = sizeof(int);  /*int size in bytes*/
  
  if (index <= 0)
    return 0;
  
  return  intsize*bcs_.iw[index-1];
}


/*----- bosBankNumber -------*/

int  bosBankNumber(int index)
     
     /* returns the bank index (keep in mind
	the extra -1 beacuse of "C" */
     
{
  
  if (index <= 0)
    return 0;
  
  return  bcs_.iw[index-3];
}


/*-------- LabellSensitivity --------*/

static void  LabelSensitivity(Hv_Widget   label,
			      Boolean    sens)
     
{
  if (sens)
    Hv_ChangeLabelColors(label, Hv_red, -1);
  else
    Hv_ChangeLabelColors(label, Hv_gray10, -1);
}

/*------- bosDropBanks --------*/

static void bosDropBanks()
     
{
  bdrop_(bcs_.iw, "E", 1);
  bdrop_(bcs_.iw, "R", 1);
  bgarb_(bcs_.iw);
  /*    bosZeroBankIndices(); */
}

/*---------- BookEvent ---------*/

static void BookEvent() {
  int              sect;
  DC0DataPtr       DC0hits;
  SCDataPtr        SChits;
  ECDataPtr        EChits;
  EC1DataPtr       EC1hits;
  
  int              i, nhit;
  unsigned char    layer, wire;
  short            slab, rgn, lay, shortlay, supl;
  unsigned char    strip;
  short            type, plane;
  
  if ((eventsource == CED_FROMET) && etReadFailure) {
    fprintf(stderr, "Skipped BookEvent, bad ET read.\n");
    return;
  }
  
  /* books the event into the "accumulated" structures */
  
  accumEventCount += 1;
  
  
  for (sect = 0; sect < 6; sect++) {
    
    /* book the DC0 banks */
    
    DC0hits = bosDC0[sect];
    
    if (DC0hits != NULL)
      for (i = 0; i < bosNumDC0[sect]; i++) {
	if (checkDCData(sect, DC0hits)) {
	  BreakShort(DC0hits->id, &wire, &layer);
	  
	  /* layer is 1..36 and must be converted */
	  
	  wire--;
	  layer--;
	  rgn = layer / 12;
	  lay = layer % 12;   /* the 0..11 variety */
	  supl = lay / 6;   /* the 0..1 variety*/
	  shortlay = lay % 6; /* the 0..5 variety */
	  
	  
	  if ((wire > DCLastWire(sect, rgn, supl, shortlay))) {
	    fprintf(stderr, "OOPS event: %d sect: %d rgn: %d supl %d lay %d wire id = %d out of %d\n",
		    bosHEAD->EventNumber, sect+1, rgn+1, supl+1, shortlay+1, wire+1,
		    DCLastWire(sect, rgn, supl, shortlay)+1);
	  }
	  else {
	    
	    accdc[sect][rgn].counts[lay][wire] += 1;
	    
	    /* check for new hottest wire */
	    
	    if (accdc[sect][rgn].counts[lay][wire] > accdc[sect][rgn].maxhits) {
	      accdc[sect][rgn].maxhits = accdc[sect][rgn].counts[lay][wire];
	      accdc[sect][rgn].hotid = DC0hits->id;
	    }
	    
	    
	    accdc[sect][rgn].maxhits = Hv_iMax(accdc[sect][rgn].maxhits,
					       accdc[sect][rgn].counts[lay][wire]);
	    
	    accdc[sect][rgn].tdcsum[lay][wire] += (float)(DC0hits->tdc);
	  }
	}
	DC0hits++;
      } /* end DC0 for loop */
    
    /*
     * now get the super layer occupancies
     */
    
    for (rgn = 0; rgn < 3; rgn++)
      for(supl = 0; supl < 2; supl++) {
	accdc[sect][rgn].occsum[supl] += SuperlayerOccupancy(sect, rgn, supl, &nhit);

	if (rgn == 0) {
	  //fprintf(stderr, "[%d, %d, %d]  occ = %f\n",
	  //	  sect+1, rgn+1, supl+1, SuperlayerOccupancy(sect, rgn, supl, &nhit)); 
	  //		fprintf(stderr, "[%d, %d, %d]  occsum = %f\n",
	  //		sect+1, rgn+1, supl+1, accdc[sect][rgn].occsum[supl]); 
	}
      }
    
    
    /* now book the Scintillator Data */
    
    
    SChits = bosSC[sect];
    for (i = 0; i < bosNumSC[sect]; i++) {
      slab = SChits->id - 1; /* convert to C Index */
      
      if ((slab >= 0) && (slab <= (NUM_SCINT-1))) {
	accsc[sect].counts[slab] += 1;
	
	if (accsc[sect].counts[slab] > accsc[sect].maxhits) {
	  accsc[sect].maxhits = accsc[sect].counts[slab];
	}
      }
      
      SChits++;
      
    } /* end scint for loop */
    
    /* now book the ec data */
    
    EChits = bosEC[sect];
    
    for (i = 0; i < bosNumEC[sect]; i++) {
      BreakShort(EChits->id, &strip, &layer);
      layer--;           /* 0..5*/
      plane = layer/3;   /*0..1*/
      type = layer % 3;  /* 0..2  for u, v, w*/
      strip--;           /* 0..25 */
      
      /* look for bad IDS (note strip is 1..36) */
      
      if ((strip < 36) && (layer < 6))
	accec[sect].counts[plane][type][strip] += 1;
      
      EChits++;
    }
    
    
    /* now book the LAC data */
    
    EC1hits = bosEC1[sect];
    
    for (i = 0; i < bosNumEC1[sect]; i++) {
      BreakShort(EC1hits->id, &strip, &layer);
      
      layer--;  /* 0..3 */
      
      plane = layer / 2;
      type  = layer % 2;
      
      strip--;
      
      if ((strip < 40) && (layer < 4))
	accec1[sect].counts[plane][type][strip] += 1;
      
      EC1hits++;
    }
    
    
  } /* END OF SECTOR LOOP */
  
}



/* this should be the standard open and close routine
   returns 0 if failure 1 if success */

static int fparm_c(char *mess)
{
  char msg[73];
  int one = 1;
  
  /*fprintf(stderr, "issuing fpack command:\n%s\n", mess);*/
  
  fparm_(mess,strlen(mess));
  
  fermes_(msg,&one,sizeof(msg));
  msg[sizeof(msg) - 1] = '\0';
  if(msg[0] != ' ') {
    fprintf(stderr, "%s\n",msg);
    return(0);
  }
  return(1);
}

/*----- GetNumRows -------*/

int GetNumRows(int *bosptr) {
  return *(bosptr-5);
}


/*--------- EventPrintName -------*/

static void EventPrintName() {
  
  char *estr;
  
  estr = (char *)Hv_Malloc(256);
  
  if (bosHEAD == NULL) {
    sprintf(estr,  "noevent");
  }
  else {
    sprintf(estr, "Run_%d_Event_%d.ps", bosHEAD->RunNumber, bosHEAD->EventNumber);
  }
  
  
  Hv_Free(Hv_psDefaultFile);
  Hv_InitCharStr(&Hv_psDefaultFile, estr);
  Hv_Free(Hv_psNote);
  Hv_InitCharStr(&Hv_psNote, estr);
  Hv_Free(estr);
}
