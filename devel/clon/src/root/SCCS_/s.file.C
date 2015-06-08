h32329
s 00002/00002/00097
d D 1.4 02/05/02 14:50:44 boiarino 5 4
c ...
c 
e
s 00001/00000/00098
d D 1.3 02/05/01 18:30:12 boiarino 4 3
c add libcodatt.so
c 
e
s 00007/00005/00091
d D 1.2 02/03/26 15:19:22 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 02/01/08 12:53:52 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/file.C
e
s 00096/00000/00000
d D 1.1 02/01/08 12:53:51 boiarino 1 0
c 
e
u
U
f b 
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
  gSystem->Load("/u/home/boiarino/clasrun/Map/s/libMap.so"); // Map library
  gSystem->Load("../cmon/SunOS/lib/libCmon.so"); // SDA library

  // to make FORTRAN happy
  gSystem->Load("/opt/SUNWspro/lib/libF77.so"); // to recognize  __s_wsFe_nv
  gSystem->Load("/opt/SUNWspro/lib/libM77.so"); // to recognize  __r_atn2
  gSystem->Load("/opt/SUNWspro/lib/libsunmath.so"); // to recognize  __fp_class

  gSystem->Load("../bosio/SunOS/lib/libBosio.so"); // BOSIO library
I 4
  gSystem->Load("../codatt/libcodatt.so"); // TT library
E 4

  //gSystem->Load("../../../et/src/libet.so"); // ET
  //gSystem->Load("$CODA/source/et/src/libet.so"); // ET
  gSystem->Load("/usr/local/coda/2.2.1r2/source/et/src/libet.so"); // ET

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

D 3
  //ptr2 = new CMDaq("/work/clas/disk1/boiarino/clas_030475.A00");
E 3
I 3
  //ptr2 = new CMDaq("/work/clas/disk1/boiarino/clas_031571.A07");
E 3
  //ptr2 = new CMDaq("/w/cache1101/a/mss/clas/e1d/data/clas_023500.A00");
D 3
  //ptr2 = new CMDaq("/w/cache1101/a/mss/clas/g6c/data/clas_029950.A00");
  ptr2 = new CMDaq("ET");
E 3
I 3
  //ptr2 = new CMDaq("/w/cache1101/a/mss/clas/e1-6a/data/clas_030866.A00-A07");
D 5
  ptr2 = new CMDaq("/w/cache1101/a/mss/clas/e6a/data/clas_031571.A00");
  //ptr2 = new CMDaq("/raid/stage_in/clas_031584.A00-A07");
E 5
I 5
//ptr2 = new CMDaq("/w/cache1101/a/mss/clas/e6a/data/clas_031571.A00");
ptr2 = new CMDaq("./test.dat");
E 5
  //ptr2 = new CMDaq("ET");
E 3


  ///////////////////////
  // create polar object
  ///////////////////////

  //ptr3 = new CMPolar("TTTTT");


  ///////////////////////
  // create epics object
  ///////////////////////

  //ptr4 = new CMEpics("./epics.tmp");


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

D 3
  HistPtr = new CMHist("/usr/local/clas/parms/cmon/root/hist1.conf");
  //HistPtr = new CMHist("./hist1.conf");
E 3
I 3
  //HistPtr = new CMHist("/usr/local/clas/parms/cmon/root/hist1.conf");
  HistPtr = new CMHist("./hist1.conf");
E 3


  ////////////////////////////////////////////////////////////////////
  // create Calib object; pointer 'CalibPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMCalib.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  //CalibPtr = new CMCalib("/usr/local/clas/parms/cmon/tglib/TAG_CALIB.dat");
}









E 1
