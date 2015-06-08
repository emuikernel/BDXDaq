test()
{
  //gROOT->Reset();

  //gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libbosio.so"); // BOSIO library
  //gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libcodatt.so"); // TT library
  //gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libet.so"); // ET
  //gSystem->Load("/usr/lib/librt.so");
  //gSystem->Load("/usr/lib/libpthread.so");
  //gSystem->Load("./$OSTYPE_MACHINE/lib/libcmroot.so");


  ////////////////////////////////////////////////////////////////////
  // create display object; pointer 'DisplayPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMDisplay.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  DisplayPtr = new CMDisplay("CLAS Online Monitor");


  /////////////////////////////////////////////////////////////////////////
  // create DAQ object ( do not forget to switch between ET and file !!! )
  /////////////////////////////////////////////////////////////////////////

  //ptr2 = new CMDaq("/work/clas/disk1/boiarino/clas_030475.A00",DisplayPtr);
  ptr2 = new CMDaq("/tmp/et_sys_clastest",DisplayPtr);



  ////////////////////////////////////////////////////////////////////
  // create Hist object; pointer 'HistPtr' will be used for the
  // botton handling operations; do not change it, that name is coded
  // in CMHist.cxx file !!!
  ////////////////////////////////////////////////////////////////////

  HistPtr = new CMHist("./hist1.conf",DisplayPtr);

}









