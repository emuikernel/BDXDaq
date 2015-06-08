h45259
s 00000/00000/00000
d R 1.2 03/06/20 17:38:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/spars.C
e
s 00055/00000/00000
d D 1.1 03/06/20 17:38:58 boiarino 1 0
c date and time created 03/06/20 17:38:58 by boiarino
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
  gSystem->Load("/u/home/boiarino/clasrun/Map/s/libMap.so"); // Map library
  gSystem->Load("../cmon/SunOS/lib/libCmon.so"); // SDA library

  // to make FORTRAN happy
  gSystem->Load("/opt/SUNWspro/lib/libF77.so"); // to recognize  __s_wsFe_nv
  gSystem->Load("/opt/SUNWspro/lib/libM77.so"); // to recognize  __r_atn2
  gSystem->Load("/opt/SUNWspro/lib/libsunmath.so"); // to recognize  __fp_class

  gSystem->Load("../bosio/SunOS/lib/libBosio.so"); // BOSIO library
  gSystem->Load("../codatt/libCodatt.so"); // TT library

  //gSystem->Load("../../../et/src/libet.so"); // ET
  //gSystem->Load("/usr/local/coda/2.2.1r2/source/et/src/libet.so"); // ET
  gSystem->Load("/usr/local/coda/2.2.1r2/SunOS/lib/libet.so"); // ET

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
  ptr2 = new CMDaq("ET");


  ////////////////////////////////////////////////////////////////////
  // create Spars object; pointer 'SparsPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMSpars.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  SparsPtr = new CMSpars("/usr/local/clas/parms/cmon/tglib/TAG_CALIB.dat");
}
E 1
