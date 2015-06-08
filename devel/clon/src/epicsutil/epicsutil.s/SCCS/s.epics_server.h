h40996
s 00001/00001/00106
d D 1.3 06/10/17 16:05:49 boiarino 4 3
c EPICS3.14-related fixes
c 
e
s 00005/00002/00102
d D 1.2 03/07/10 16:40:52 wolin 3 1
c Working
e
s 00000/00000/00000
d R 1.2 03/07/10 13:26:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 epicsutil/s/epics_server.h
e
s 00104/00000/00000
d D 1.1 03/07/10 13:26:15 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  epics_server.h

//  epics server class defs

//  E.Wolin, 3-Jul-03


I 3
#include <gddApps.h>
#include <gddAppFuncTable.h>
E 3


I 3
void setDebug(int val);


E 3
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
D 3
  
E 3
  void fillPV(int i);
  void fillPV(uint ui);
  void fillPV(double d);
  void setAlarm();
  casChannel *createChannel(const casCtx &ctx,const char * const pUserName, 
			    const char * const pHostName);
D 3

E 3
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
D 4
    caStatus write(const casCtx &ctx, gdd &value);
E 4
I 4
    caStatus write(const casCtx &ctx, const gdd &value);
E 4
    epicsShareFunc const char *getName() const;
    ~myAttrPV();
    
  };
  

//-------------------------------------------------------------------------
E 1
