h21066
s 00002/00001/00096
d D 1.7 05/02/25 14:59:22 boiarino 8 7
c *** empty log message ***
e
s 00003/00003/00094
d D 1.6 03/12/16 22:26:03 boiarino 7 6
c .
e
s 00004/00004/00093
d D 1.5 03/04/17 17:02:04 boiarino 6 5
c *** empty log message ***
e
s 00001/00000/00096
d D 1.4 02/05/01 18:37:24 boiarino 5 4
c add libcodatt.so
c 
e
s 00002/00002/00094
d D 1.3 02/03/26 15:19:07 boiarino 4 3
c *** empty log message ***
e
s 00005/00005/00091
d D 1.2 01/11/28 23:02:59 boiarino 3 1
c minor
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/cmon.C
e
s 00096/00000/00000
d D 1.1 01/11/19 16:44:48 boiarino 1 0
c date and time created 01/11/19 16:44:48 by boiarino
e
u
U
f e 0
t
T
I 1
{
  gROOT->Reset();


  ///////////////////////////
  // Load libraries we need
  ///////////////////////////

  // fast reconstruction
D 3
  gSystem->Load("../../../Map/s/libMap.so"); // Map library
  gSystem->Load("../SunOS/lib/libCmon.so"); // SDA library
E 3
I 3
  gSystem->Load("/u/home/boiarino/clasrun/Map/s/libMap.so"); // Map library
  gSystem->Load("../cmon/SunOS/lib/libCmon.so"); // SDA library
E 3

  // to make FORTRAN happy
  gSystem->Load("/opt/SUNWspro/lib/libF77.so"); // to recognize  __s_wsFe_nv
  gSystem->Load("/opt/SUNWspro/lib/libM77.so"); // to recognize  __r_atn2
  gSystem->Load("/opt/SUNWspro/lib/libsunmath.so"); // to recognize  __fp_class

D 3
  gSystem->Load("../../bosio/SunOS/lib/libBosio.so"); // BOSIO library
E 3
I 3
  gSystem->Load("../bosio/SunOS/lib/libBosio.so"); // BOSIO library
I 5
D 6
  gSystem->Load("../codatt/libcodatt.so"); // TT library
E 6
I 6
  gSystem->Load("../codatt/libCodatt.so"); // TT library
E 6
E 5
E 3

D 3
  gSystem->Load("../../../et/src/libet.so"); // ET
E 3
I 3
  //gSystem->Load("../../../et/src/libet.so"); // ET
E 3
D 6
  //gSystem->Load("$CODA/source/et/src/libet.so"); // ET
D 3
  //gSystem->Load("/usr/local/coda/2.2.1/source/et/src/libet.so"); // ET
E 3
I 3
  gSystem->Load("/usr/local/coda/2.2.1r2/source/et/src/libet.so"); // ET
E 6
I 6
  //gSystem->Load("/usr/local/coda/2.2.1r2/source/et/src/libet.so"); // ET
  gSystem->Load("/usr/local/coda/2.2.1r2/SunOS/lib/libet.so"); // ET
E 6
E 3

  // CLAS Monitor ROOT library
  gSystem->Load("./libCMRoot.so");


  ////////////////////////////////////////////////////////////////////
  // create display object; pointer 'DisplayPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMDisplay.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  DisplayPtr = new CMDisplay("CLAS Online Monitor");


  /////////////////////////////////////////////////////////////////////////
  // create DAQ object ( do not forget to switch between ET and file !!! )
  /////////////////////////////////////////////////////////////////////////

  //ptr2 = new CMDaq("/work/clas/disk1/boiarino/clas_030475.A00");
  //ptr2 = new CMDaq("/w/cache1101/a/mss/clas/e1d/data/clas_023500.A00");
  //ptr2 = new CMDaq("/w/cache1101/a/mss/clas/g6c/data/clas_029950.A00");
D 6
  ptr2 = new CMDaq("ET");
E 6
I 6
D 7
  ptr2 = new CMDaq("/tmp/et_sys_clasprod");
E 7
I 7
D 8
  ptr2 = new CMDaq("/tmp/et_sys_sertest");
E 8
I 8
//ptr2 = new CMDaq("/tmp/et_sys_clastest");
  ptr2 = new CMDaq("/tmp/et_sys_clasprod");
E 8
E 7
E 6


  ///////////////////////
  // create polar object
  ///////////////////////

  //ptr3 = new CMPolar("TTTTT");


  ///////////////////////
D 7
  // create epics object
E 7
I 7
  // create EPICS object
E 7
  ///////////////////////

D 7
  //ptr4 = new CMEpics("./epics.tmp");
E 7
I 7
  EpicsPtr = new CMEpics("./epics.tmp");
E 7


  ////////////////////////////////////////////////////////////////////
  // create SDA object; pointer 'SdaPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMSda.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  //SdaPtr = new CMSda("./sda.config");


  ////////////////////////////////////////////////////////////////////
  // create Hist object; pointer 'HistPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMHist.cxx file !!!
  ////////////////////////////////////////////////////////////////////

D 4
  HistPtr = new CMHist("/usr/local/clas/parms/cmon/root/hist1.conf");
  //HistPtr = new CMHist("./hist1.conf");
E 4
I 4
  //HistPtr = new CMHist("/usr/local/clas/parms/cmon/root/hist1.conf");
  HistPtr = new CMHist("./hist1.conf");
E 4


  ////////////////////////////////////////////////////////////////////
  // create Calib object; pointer 'CalibPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMCalib.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  //CalibPtr = new CMCalib("/usr/local/clas/parms/cmon/tglib/TAG_CALIB.dat");
}









E 1
