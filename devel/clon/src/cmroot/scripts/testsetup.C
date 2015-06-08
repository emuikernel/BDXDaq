{
  gROOT->Reset();


  ///////////////////////////
  // Load libraries we need
  ///////////////////////////

  // fast reconstruction
//gSystem->Load("/u/home/boiarino/clasrun/Map/s/libMap.so"); // Map library
//gSystem->Load("../cmon/SunOS/lib/libCmon.so"); // SDA library

  // to make FORTRAN happy
//gSystem->Load("/opt/SUNWspro/lib/libF77.so"); // to recognize  __s_wsFe_nv
//gSystem->Load("/opt/SUNWspro/lib/libM77.so"); // to recognize  __r_atn2
//gSystem->Load("/opt/SUNWspro/lib/libsunmath.so"); // to recognize  __fp_class

  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libbosio.so"); // BOSIO library
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libcodatt.so"); // TT library

  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libet.so"); // ET
  gSystem->Load("/usr/lib/librt.so");
  gSystem->Load("/usr/lib/libpthread.so");
  gSystem->Load("/usr/lib/libresolv.so"); // for inet_aton

  // CLAS Monitor ROOT library
  //gSystem->Load("$CLON/$OSTYPE_MACHINE/lib/libcmroot.so");
  gSystem->Load("./$OSTYPE_MACHINE/lib/libcmroot.so");


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
  ptr2 = new CMDaq("/tmp/et_sys_clastest");


  ////////////////////////////////////////////////////////////////////
  // create TestSetup object; pointer 'TestSetupPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMTestSetup.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  TestSetupPtr = new CMTestSetup("./hist222.conf");

}





