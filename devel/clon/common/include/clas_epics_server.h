//  clas_epics_server.h

//  contains epics channel names, types, units, etc.

//  field names case-insensitive

//  read only to fields:  
//       STAT,SEVR,VAL

//  read/write allowed to fields:
//       ALRM,HIHI,LOLO,HIGH,LOW,HOPR,LOPR,DRVH,DRVL

//   all alarm limits are integers, set ALRM to 0(1) to turn alarms off(on)

//  E.Wolin, 2-jul-03


// channel names
static const char *pvNames[] = {
  "hallb_run_number", 	 "hallb_event_count",  "hallb_event_rate",    "hallb_livetime",
  "hallb_ts_csr",     	 "hallb_go_bit",       "hallb_trig_enable",   "hallb_roc_enable",
  "hallb_prescale_1", 	 "hallb_prescale_2",   "hallb_prescale_3",    "hallb_prescale_4",
  "hallb_prescale_5", 	 "hallb_prescale_6",   "hallb_prescale_7",    "hallb_prescale_8",
  "hallb_ec_inner_hi",	 "hallb_ec_outer_hi",  "hallb_ec_total_hi",   "hallb_ec_inner_lo",
  "hallb_ec_outer_lo",	 "hallb_ec_total_lo", 
  "hallb_cc_hi",      	 "hallb_cc_lo",        "hallb_sc_hi",         "hallb_sc_lo",
  "hallb_ec_tdc_thresh", "hallb_ec_tdc_width", "hallb_cc_tdc_thresh", "hallb_cc_tdc_width",
  "hallb_sc_tdc_thresh", "hallb_sc_tdc_width",
  "hallb_data_rate",
};


//  channel types
static aitEnum pvTypes[sizeof(pvNames)/sizeof(char*)] = {
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumUint32,  aitEnumUint32,  aitEnumUint32,  aitEnumUint32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32, 	    
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,
  aitEnumInt32,
};


// pv units
static const char *pvUnits[sizeof(pvNames)/sizeof(char*)] = {
  "run",    "events",   "evt/sec",  "percent",
  "hex",    "off/on",   "hex", 	    "hex",
  "trig",   "trig",	"trig",	    "trig",
  "trig",   "trig",	"trig",	    "trig",
  "count",  "count",    "count",    "count",
  "count",  "count",
  "count",  "count",	"count",    "count",
  "count",  "count",	"count",    "count",
  "count",  "count",
  "MB/sec",
};


//  PREC field
static int pvPREC[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};



//  values below archived to file and read in at program start


//  ALRM field, non-zero means alarms enabled
static int pvAlarm[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  HIHI field
static int pvHIHI[sizeof(pvNames)/sizeof(char*)] = {
    0,100,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  LOLO field
static int pvLOLO[sizeof(pvNames)/sizeof(char*)] = {
    0,5,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  HIGH field
static int pvHIGH[sizeof(pvNames)/sizeof(char*)] = {
    0,50,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  LOW field
static int pvLOW[sizeof(pvNames)/sizeof(char*)] = {
    0,10,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  HOPR field
static int pvHOPR[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,100,0,1,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  LOPR field
static int pvLOPR[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  DRVH field
static int pvDRVH[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//  DRVL field
static int pvDRVL[sizeof(pvNames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,
};


//------------------------------------------------------------------------------