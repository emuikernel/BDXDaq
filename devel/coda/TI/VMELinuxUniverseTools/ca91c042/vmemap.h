//------------------------------------------------------------------------------  
//title: Tundra Universe PCI-VME Kernel Driver
//version: Linux 1.1
//date: March 1999                                                                
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//platform: Linux 2.4.x
//language: GCC 2.95 and 3.0
//module: ca91c042
//------------------------------------------------------------------------------  
//  Purpose: Provide a Kernel Driver to Linux for the Universe I and II 
//           Universe model number ca91c042
//  Docs:                                  
//    This driver supports both the Universe and Universe II chips                                     
//------------------------------------------------------------------------------  
//-----------------------------------------------------------------------------
// RCS:
// $Id: vmemap.h,v 1.4 2001/10/27 03:50:07 jhuggins Exp $
//
//-----------------------------------------------------------------------------
//       76543210
// 31-24 Reserved
//       76   543     210
// 23-16 VDW Reserved  VAS
//       76  54     321       0
// 15-08 PGM SUPER  Reserved  VCT
//       765432    10
// 07-00 Reserved  LAS

#define VMED8    0x00000000
#define VMED16   0x00400000
#define VMED32   0x00800000
#define VMED64   0x00C00000

#define VMEA16   0x00000000
#define VMEA24   0x00010000
#define VMEA32   0x00020000
#define VMECR    0x00050000
#define VMEUR1   0x00060000
#define VMEUR2   0x00070000

#define VMEDATA  0x00000000
#define VMEPGM   0x00004000
#define VMESUPER 0x00000100

#define DMAREAD  0x00000000
#define DMAWRITE 0x80000000
