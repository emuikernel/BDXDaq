//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Constants used in the CG library
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgConst.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef CONST_T
#define CONST_T

#define CG_ALIGNMENT_BEGINNING  0x1000
#define CG_ALIGNMENT_CENTER     0x1001
#define CG_ALIGNMENT_END        0x1002

#define CG_ALIGNMENT_TOP        0x2000
#define CG_ALIGNMENT_BOTTOM     0x2001

/* Rotated string alignment */
#define CG_NONE                    0
#define CG_TLEFT                   1
#define CG_TCENTRE                 2
#define CG_TRIGHT                  3
#define CG_MLEFT                   4
#define CG_MCENTRE                 5
#define CG_MRIGHT                  6
#define CG_BLEFT                   7
#define CG_BCENTRE                 8
#define CG_BRIGHT                  9

#define PI       3.141592653589793

#define PIHALF   1.570796326794896

#define TWOPI    6.283185307179586

#define RAD2DEG 57.295779513082323        // radians to degrees factor

#define DEG2RAD  0.017453292519943        // degrees to radians factor

#define HSR2     0.707106781186547               // half times sqrt(2)
#endif
