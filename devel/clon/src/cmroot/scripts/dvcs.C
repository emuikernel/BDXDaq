{
  gROOT->Reset();


  ///////////////////////////
  // Load libraries we need
  ///////////////////////////

  // fast reconstruction
//gSystem->Load("/u/home/boiarino/clasrun/Map/s/libMap.so"); // Map library
//gSystem->Load("../cmon/SunOS/lib/libcmon.so"); // SDA library

  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libbosio.so"); // BOSIO library
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libcodatt.so"); // TT library
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libet.so"); // ET

// Solaris
  gSystem->Load("/usr/lib/librt.so");
  gSystem->Load("/usr/lib/libpthread.so");
  gSystem->Load("/usr/lib/libresolv.so"); // for inet_aton

//Linux
//do not need any extra libraries ???

  // CLAS Monitor ROOT library
  gSystem->Load("./$OSTYPE_MACHINE/lib/libcmroot.so");
//gSystem->Load("$CLON/$OSTYPE_MACHINE/lib/libcmroot.so");


  ////////////////////////////////////////////////////////////////////
  // create display object; pointer 'DisplayPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMDisplay.cxx file !!!
  ////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////  DisplayPtr = new CMDisplay("CLAS Online Monitor");


  /////////////////////////////////////////////////////////////////////////
  // create DAQ object ( do not forget to switch between ET and file !!! )
  /////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////  ptr2 = new CMDaq("/work/boiarino/clas_030735.A00",DisplayPtr);
//ptr2 = new CMDaq("/tmp/et_sys_clasprod",DisplayPtr);


  ///////////////////////
  // create polar object
  ///////////////////////

  //ptr3 = new CMPolar("TTTTT",DisplayPtr);


  ///////////////////////
  // create EPICS object
  ///////////////////////

  //EpicsPtr = new CMEpics("./epics.tmp");


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

  //HistPtr = new CMHist("$CLON_PARMS/cmon/root/hist1.conf",DisplayPtr);
/////////////////////////  HistPtr = new CMHist("/usr/local/clas/parms/cmon/root/hist1.conf",DisplayPtr);

  //HistPtr = new CMHist("./scripts/hist1.conf",DisplayPtr);


  ////////////////////////////////////////////////////////////////////
  // create Calib object; pointer 'CalibPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMCalib.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  //CalibPtr = new CMCalib("/usr/local/clas/parms/cmon/tglib/TAG_CALIB.dat");




  ////////////////////////////////////////////////////////////////////
  // DVCS IC calorimeter GUI
  ////////////////////////////////////////////////////////////////////

  pDVCSGUI = new DVCSGUI(gClient->GetRoot(), 400, 200, "dvcstrig.dat");
  while(1)
  {
    gSystem->ProcessEvents();
    gSystem->Sleep(1);
  }




}









