#define PCI_VME_INTERFACE


#define _DEBUG
#define PLX_9054
#define PCI_CODE
#define LITTLE_ENDIAN


#ifdef PCI_VME_INTERFACE

  #include "C:\sis1100_3100_win2k\win2000_driver\sis1100_win2k_utils\PLX\PlxApi.h"
  #include "C:\sis1100_3100_win2k\win2000_driver\sis1100_win2k_utils\PLX\Reg9054.h"

  #include "c:\sis1100_3100_win2k\win2000_driver\sis1100_win2k_utils\sis1100_lib\sis1100w.h"		// Header sis1100w.dll
  #include "c:\sis1100_3100_win2k\win2000_driver\sis1100_win2k_utils\sis1100_lib\sis3100_vme_calls.h"



#else 
  #include "..\temp\PlxApi.h"
  #include "..\temp\Reg9054.h"		// Header sis1100w.dll
  #include "..\temp\sis1100w.h"		// Header sis1100w.dll
  #include "..\temp\sis3100_vme_calls.h"

#endif
  extern struct SIS1100_Device_Struct  gl_sis1100_device ;
