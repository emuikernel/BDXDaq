//  epics_server.cc

// pv classes for creating a PCAS


// notes:

//   PCAS probably not thread safe so need myUpdate flags

//   creates attrPV's (on the fly) to handle r/w to (case insensitive) fields:
//       HIHI,HIGH,LOLO,LOW,HOPR,LOPR,DRVH,DRVL,ALRM,STAT,SEVR,PREC
//       (n.b. VAL maps to regular data pv)

//   write only allowed to following fields:
//       ALRM,HIHI,LOLO,HIGH,LOW,HOPR,LOPR,DRVH,DRVL

//   all alarm limits are integers, set ALRM to 0(1) to turn alarms off(on)

//   have to set EPICS_CAS_INTF_ADDR_LIST to limit server to single network interface


// to do:


//  ejw, 3-jul-2003



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for ca
#include <casdef.h>
#include <alarm.h>
#include <gdd.h>


// for epics_server
#include <epics_server.h>


// misc
#include <iostream.h>
#include <iomanip.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )

//  misc variables
static int debug = 0;
static char temp[4096];
static int epicsToLocalTime  = 20*(365*24*60*60) + 5*(24*60*60) - (60*60); //?daylight savings?


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//  channel objects for setting read/write access for all data channels
class myChannel : public casChannel {

public:

  myChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
  }


//---------------------------------------------------


  /*sergey aitBool*/bool readAccess() const {
    return aitTrue;
  }


//---------------------------------------------------


  /*sergey aitBool*/bool writeAccess() const {
    return aitFalse;
  }

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


  myPV::myPV(const char *name, aitEnum type, const char *units, int alarm, int hihi, int lolo, 
	     int high, int low, int hopr, int lopr, int drvh, int drvl, int prec) {

    myName=strdup(name);
    myType=type;
    myUnits=strdup(units);
    myAlarm=alarm;
    myHIHI=hihi;
    myLOLO=lolo;
    myHIGH=high;
    myLOW=low;
    myHOPR=hopr;
    myLOPR=lopr;
    myDRVH=drvh;
    myDRVL=drvl;

    myTime=0;
    myIValue=0;
    myUIValue=0;
    myDValue=0.0;
    myStat=epicsAlarmNone;
    mySevr=epicsSevNone;
    myMonitor=0;
    myUpdate=0;

    initFT();
    if(debug!=0)cout << "myPV constructor for " << myName << endl;
  }
  

//---------------------------------------------------


    void myPV::fillPV(int i) {

	if(i!=myIValue) {
	    myUpdate=1;
	    myIValue=i;
	    myTime=time(NULL)-epicsToLocalTime;
	}
	setAlarm();
    }


//------------------------------------------------------------------


    void myPV::fillPV(uint ui) {

	if(ui!=myUIValue) {
	    myUpdate=1;
	    myUIValue=ui;
	    myTime=time(NULL)-epicsToLocalTime;
	}
	setAlarm();
    }


//------------------------------------------------------------------


    void myPV::fillPV(double d) {

	if(d!=myDValue) {
	    myUpdate=1;
	    myDValue=d;
	    myTime=time(NULL)-epicsToLocalTime;
	}
	setAlarm();
    }


//------------------------------------------------------------------


    void myPV::setAlarm() {

	int ival;
	int oldStat = myStat;
	int oldSevr = mySevr;
	

	if(myType==aitEnumInt32) {
	    ival=myIValue;
	    
	} else if (myType==aitEnumUint32) {
	    ival=(int)myUIValue;
	    
	} else if (myType==aitEnumFloat64) {
	    ival=(int)myDValue;
	    
	} else {
	    cerr << "setalarm...unknown ait type for " << myName << endl;
	    return;
	}
	
	
	if(myAlarm!=0) {
	    if(ival>=myHIHI) {
		myStat=epicsAlarmHiHi;
		mySevr=epicsSevMajor;
	    } else if (ival<=myLOLO) {
		myStat=epicsAlarmLoLo;
		mySevr=epicsSevMajor;
	    } else if (ival>=myHIGH) {
		myStat=epicsAlarmHigh;
		mySevr=epicsSevMinor;
	    } else if (ival<=myLOW) {
		myStat=epicsAlarmLow;
		mySevr=epicsSevMinor;
	    } else {
		myStat=epicsAlarmNone;
		mySevr=epicsSevNone;
	    }

	} else {
	    myStat=epicsAlarmNone;
	    mySevr=epicsSevNone;
	}
	
	
	// force update if alarm state changed
	if((oldStat!=myStat)||(oldSevr!=mySevr))myUpdate=1;
    }


//------------------------------------------------------------------


  casChannel *myPV::createChannel(const casCtx &ctx,
			    const char * const pUserName, 
			    const char * const pHostName) {
    return new myChannel(ctx);
  }


//---------------------------------------------------


  caStatus myPV::read(const casCtx &ctx, gdd &prototype) {
    return(ft.read(*this, prototype));
}


//---------------------------------------------------


  void myPV::initFT() {
  
    if(ft_is_initialized!=0)return;
    ft_is_initialized=1;

    ft.installReadFunc("value",    	    &myPV::getVAL);
    ft.installReadFunc("status",   	    &myPV::getSTAT);
    ft.installReadFunc("severity", 	    &myPV::getSEVR);
    ft.installReadFunc("graphicHigh", 	    &myPV::getHOPR);
    ft.installReadFunc("graphicLow",  	    &myPV::getLOPR);
    ft.installReadFunc("controlHigh", 	    &myPV::getDRVH);
    ft.installReadFunc("controlLow",  	    &myPV::getDRVL);
    ft.installReadFunc("alarmHigh",   	    &myPV::getHIHI);
    ft.installReadFunc("alarmLow",    	    &myPV::getLOLO);
    ft.installReadFunc("alarmHighWarning",  &myPV::getHIGH);
    ft.installReadFunc("alarmLowWarning",   &myPV::getLOW);
    ft.installReadFunc("units",       	    &myPV::getUNITS);
    ft.installReadFunc("precision",   	    &myPV::getPREC);
    ft.installReadFunc("enums",       	    &myPV::getENUM);
}


//--------------------------------------------------


  aitEnum myPV::bestExternalType() const {
    return(myType);
}


//---------------------------------------------------


  gddAppFuncTableStatus myPV::getUNITS(gdd &value) {
    if(debug!=0) cout << "...myPV getUNITS for " << myName << endl;
    value.putConvert(myUnits);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getVAL(gdd &value) {
    if(debug!=0) cout << "...myPV getVAL for " << myName << endl;

    if(myType==aitEnumInt32) {
      value.putConvert(myIValue);

    } else if (myType==aitEnumUint32) {
      value.putConvert(myUIValue);

    } else if (myType==aitEnumFloat64) {
      value.putConvert(myDValue);

    } else {
      cerr << "Unknown ait type " << myType << endl;
      value.putConvert(0);
    }

    value.setStat(myStat);
    value.setSevr(mySevr);

    struct timespec t;
    t.tv_sec = myTime;
    t.tv_nsec=0;
    value.setTimeStamp(&t);

    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getSTAT(gdd &value) {
    if(debug!=0) cout << "...myPV getSTAT for " << myName << endl;
    value.putConvert(myStat);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getSEVR(gdd &value) {
    if(debug!=0) cout << "...myPV getSEVR for " << myName << endl;
    value.putConvert(mySevr);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getHIHI(gdd &value) {
    if(debug!=0) cout << "...myPV getHIHI for " << myName << endl;
    value.putConvert(myHIHI);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getLOLO(gdd &value) {
    if(debug!=0) cout << "...myPV getLOLO for " << myName << endl;
    value.putConvert(myLOLO);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getHOPR(gdd &value) {
    if(debug!=0) cout << "...myPV getHOPR for " << myName << endl;
    value.putConvert(myHOPR);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getLOPR(gdd &value) {
    if(debug!=0) cout << "...myPV getLOPR for " << myName << endl;
    value.putConvert(myLOPR);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getDRVH(gdd &value) {
    if(debug!=0) cout << "...myPV getDRVH for " << myName << endl;
    value.putConvert(myDRVH);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getDRVL(gdd &value) {
    if(debug!=0) cout << "...myPV getDRVL for " << myName << endl;
    value.putConvert(myDRVL);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getHIGH(gdd &value) {
    if(debug!=0) cout << "...myPV getHIGH for " << myName << endl;
    value.putConvert(myHIGH);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getLOW(gdd &value) {
    if(debug!=0) cout << "...myPV getLOW for " << myName << endl;
    value.putConvert(myLOW);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getPREC(gdd &value) {
    if(debug!=0) cout << "...myPV getPREC for " << myName << endl;
    value.putConvert(myPREC);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus myPV::getENUM(gdd &value) {
    if(debug!=0) cout << "...myPV getNUM for " << myName << endl;
    value.putConvert(0);
    return S_casApp_success;
  }
  

//---------------------------------------------------


  caStatus myPV::interestRegister() {
    if(debug!=0) cout << "...myPV interestRegister for " << myName << endl;
    myMonitor=1;
    return S_casApp_success;
  }


//---------------------------------------------------


  void myPV::interestDelete() {
    if(debug!=0) cout << "...myPV interestDelete for " << myName << endl;
    myMonitor=0;
  }


//---------------------------------------------------


  epicsShareFunc const char *myPV::getName() const {
    return(myName);
  }
  

//---------------------------------------------------


  void myPV::destroy() {
    if(debug!=0)cout << "myPV destroy for " << myName << endl;
  }
    

//---------------------------------------------------


  myPV::~myPV() {
    if(debug!=0)cout << "myPV destructor for " << myName << endl;
  }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


//  channel objects for setting read/write access for all attribute channels
class myAttrChannel : public casChannel {

public:

  myAttrChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
  }


//---------------------------------------------------


  /*sergey aitBool*/bool readAccess() const {
    return aitTrue;
  }


//---------------------------------------------------


  /*sergey aitBool*/bool writeAccess() const {
    return aitTrue;
  }

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


  myAttrPV::myAttrPV(myPV *ptr, const char* attr, int len) {
      myPtr=ptr;
      strncpy(myAttr,"    ",4);
      strncpy(myAttr,attr,MIN(4,len));
      if(debug!=0)cout << "myAttrPV constructor for " << myPtr->myName 
		       << ", attr: " << myAttr << endl;
  }
  

//---------------------------------------------------


  casChannel *myAttrPV::createChannel(const casCtx &ctx,
				      const char * const pUserName, 
				      const char * const pHostName) {
    return new myAttrChannel(ctx);
  }


//---------------------------------------------------


  aitEnum myAttrPV::bestExternalType() const {
    return(aitEnumInt32);
}


//---------------------------------------------------


  caStatus myAttrPV::read(const casCtx &ctx, gdd &value) {
      if(debug!=0)cout << "...myAttrPV read for " << myPtr->myName << ", attr: " << myAttr << endl;


    if(strncasecmp(myAttr,"ALRM",4)==0) {
      value.putConvert(myPtr->myAlarm);
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
      value.putConvert(myPtr->myHIHI);
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
      value.putConvert(myPtr->myLOLO);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"HIGH",4)==0) {
      value.putConvert(myPtr->myHIGH);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"LOW ",4)==0) {
      value.putConvert(myPtr->myLOW);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"HOPR",4)==0) {
      value.putConvert(myPtr->myHOPR);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"LOPR",4)==0) {
      value.putConvert(myPtr->myLOPR);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"DRVH",4)==0) {
      value.putConvert(myPtr->myDRVH);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"DRVL",4)==0) {
      value.putConvert(myPtr->myDRVL);
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"STAT",4)==0) {
      value.putConvert(myPtr->myStat);
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"SEVR",4)==0) {
      value.putConvert(myPtr->mySevr);
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"PREC",4)==0) {
      value.putConvert(myPtr->myPREC);
      return(S_casApp_success);

    } else {
      cerr << "No read support for attribute: \"" << myAttr << "\"" << endl;
      return(S_casApp_noSupport);
    }

  }
  

//---------------------------------------------------


  caStatus myAttrPV::write(const casCtx &ctx, const gdd &value) {
    if(debug!=0)cout << "...myAttrPV write for " << myPtr->myName 
		     << ",  attr: " << myAttr << ",  value: " << (int)value << endl;


    if(strncasecmp(myAttr,"ALRM",4)==0) {
	myPtr->myAlarm=(int)value;

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
	myPtr->myHIHI=(int)value;

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
	myPtr->myLOLO=(int)value;
      
    } else if (strncasecmp(myAttr,"HIGH",4)==0) {
	myPtr->myHIGH=(int)value;
      
    } else if (strncasecmp(myAttr,"LOW ",4)==0) {
	myPtr->myLOW=(int)value;

    } else if (strncasecmp(myAttr,"HOPR",4)==0) {
	myPtr->myHOPR=(int)value;

    } else if (strncasecmp(myAttr,"LOPR",4)==0) {
	myPtr->myLOPR=(int)value;

    } else if (strncasecmp(myAttr,"DRVH",4)==0) {
	myPtr->myDRVH=(int)value;

    } else if (strncasecmp(myAttr,"DRVL",4)==0) {
	myPtr->myDRVL=(int)value;

    } else if (strncasecmp(myAttr,"PREC",4)==0) {
	myPtr->myPREC=(int)value;

    } else {
	cerr << "No write support for attr: \"" << myAttr << "\"" << endl;
	return S_casApp_noSupport;
    }

    myPtr->setAlarm();

    return S_casApp_success;
  }
  

//---------------------------------------------------


  epicsShareFunc const char *myAttrPV::getName() const {
    sprintf(temp,"%s.%4s",myPtr->myName,myAttr);
    return(temp);
  }


//---------------------------------------------------


  myAttrPV::~myAttrPV() {
    if(debug!=0)cout << "myAttrPV destructor" << endl;
  }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//  must declare all static vars outside of class definition to allocate storage
int myPV::ft_is_initialized=0;
gddAppFuncTable<myPV> myPV::ft;


//---------------------------------------------------------------------------


void setDebug(int val) {
  debug=val;
}


//---------------------------------------------------------------------------
