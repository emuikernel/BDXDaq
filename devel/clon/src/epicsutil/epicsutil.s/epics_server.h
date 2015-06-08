//  epics_server.h

//  epics server class defs

//  E.Wolin, 3-Jul-03


#include <gddApps.h>
#include <gddAppFuncTable.h>


void setDebug(int val);


//-------------------------------------------------------------------------


class myPV : public casPV {


private:
  static int ft_is_initialized;
  static gddAppFuncTable<myPV> ft;
  
  
public:
  char *myName;
  aitEnum myType;
  char *myUnits;
  int myAlarm;
  int myHIHI;
  int myLOLO;
  int myHIGH;
  int myLOW;
  int myHOPR;
  int myLOPR;
  int myDRVH;
  int myDRVL;
  int myPREC;
  
  int myIValue;
  unsigned int myUIValue;
  double myDValue;
  time_t myTime;
  int myStat;
  int mySevr;
  int myMonitor;
  int myUpdate;

  
  myPV(const char *name, aitEnum type, const char *units, int alarm, int hihi, int lolo, 
       int high, int low, int hopr, int lopr, int drvh, int drvl, int prec);
  void fillPV(int i);
  void fillPV(uint ui);
  void fillPV(double d);
  void setAlarm();
  casChannel *createChannel(const casCtx &ctx,const char * const pUserName, 
			    const char * const pHostName);
  caStatus read(const casCtx &ctx, gdd &prototype);
  static void initFT();
  aitEnum bestExternalType() const;
  gddAppFuncTableStatus getUNITS(gdd &value);
  gddAppFuncTableStatus getVAL(gdd &value);
  gddAppFuncTableStatus getSTAT(gdd &value);
  gddAppFuncTableStatus getSEVR(gdd &value);
  gddAppFuncTableStatus getHIHI(gdd &value);
  gddAppFuncTableStatus getLOLO(gdd &value);
  gddAppFuncTableStatus getHOPR(gdd &value);
  gddAppFuncTableStatus getLOPR(gdd &value);
  gddAppFuncTableStatus getDRVH(gdd &value);
  gddAppFuncTableStatus getDRVL(gdd &value);
  gddAppFuncTableStatus getHIGH(gdd &value);
  gddAppFuncTableStatus getLOW(gdd &value);
  gddAppFuncTableStatus getPREC(gdd &value);
  gddAppFuncTableStatus getENUM(gdd &value);
  caStatus interestRegister();
  void interestDelete();
  epicsShareFunc const char *getName() const;
  void destroy();
  ~myPV();

};


//-------------------------------------------------------------------------


class myAttrPV : public casPV {


 public:
    myPV *myPtr;
    char myAttr[4];
    
    myAttrPV(myPV *ptr, const char* attr, int len);
    casChannel *createChannel(const casCtx &ctx,const char * const pUserName, 
			      const char * const pHostName);
    aitEnum bestExternalType() const;
    caStatus read(const casCtx &ctx, gdd &value);
    caStatus write(const casCtx &ctx, const gdd &value);
    epicsShareFunc const char *getName() const;
    ~myAttrPV();
    
  };
  

//-------------------------------------------------------------------------
