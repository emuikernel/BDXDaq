h48579
s 00010/00000/00165
d D 1.3 06/09/01 00:35:47 boiarino 4 3
c dummy VXWORKS
c 
e
s 00018/00019/00147
d D 1.2 02/08/25 19:42:00 boiarino 3 1
c big cleanup (globals, locals, prototypes)
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/feedback.c
e
s 00166/00000/00000
d D 1.1 02/03/29 12:13:00 boiarino 1 0
c date and time created 02/03/29 12:13:00 by boiarino
e
u
U
f e 0
t
T
I 1

I 4
#ifndef VXWORKS

E 4
D 3
#include  "Hv.h"
#include  "simple.h"
E 3
I 3
/* feedback.c */
E 3

I 3
#include <stdio.h>

#include "Hv.h"
E 3
#include "ced.h"
D 3
#include "bos.h"
#include "tgeometry.h"
#include "scintlook.h"
E 3
I 3
#include "tigris.h"
E 3

D 3
static int ScintFeedback();
E 3
I 3
int bits4FB[6][2][96]; /* global */
E 3

D 3
extern     BitsLookup       bitslookup [48] ; 
int        bits4FB [6][2][96];
E 3
I 3
extern Scint_Package Scintillators[6]; /* defined in ced_geometry.c */
extern BitsLookup bitslookup[48]; /* defined in scintillator.c */
E 3

D 3
/* ------ simpleFeedback ---------- */
E 3
I 3
/* local prototypes */
E 3

D 3
void scintViewFeedback(View, pp)
E 3
I 3
static int ScintFeedback(Hv_View View, Hv_Point pp, int *scintNum,
                         int *scintData);
E 3

D 3
Hv_View         View;
Hv_Point        pp;        /*mouse location*/

E 3
I 3
/* ------ simpleFeedback ---------- */
/* pp - mouse location */
void
scintViewFeedback(Hv_View View, Hv_Point pp)
E 3
{
  ViewData         vdata ;
  float            worldx, worldy;
  char             sc_text[20] ;
  int              scint=0, sN, sD, eV=0;
  static int       rS1, rS2, hereBefore=0;

  vdata = (ViewData) (View->data) ;

  Hv_LocalToWorld(View, &worldx, &worldy, pp.x, pp.y);

  scint = ScintFeedback (View, pp, &sN, &sD) ;

  if ((rS1 > 0) &&(rS1 < 95))
      bits4FB [vdata->sectorNumber][vdata->sglphase][rS1] = 0;
  if ((rS2 > 0) &&(rS2 < 95))
      bits4FB [vdata->sectorNumber][vdata->sglphase][rS2] = 0;
  if (scint != 0) {
    if (sN > 16)
      eV = 3 ;
    else
      eV = 0;
    hereBefore = 1;
    sprintf (sc_text, "SD %d  Scint %d", ((sD+1)+eV), sN);
    if (bitslookup[(sN-1)].b.howmany == 0) {
      rS1 = (sN-1)+(sD*16) ;
      rS2 = rS1;
      if ((rS1 > 0) &&(rS1 < 95))
	  bits4FB [vdata->sectorNumber][vdata->sglphase][rS1] = 1;      
    }
    else
      if (bitslookup[(sN-1)].b.howmany == 1) {
	rS1 = bitslookup[(sN-1)].b.litebitbut[0]+(sD*16) ;
	rS2 = rS1;

	if ((rS1 > 0) &&(rS1 < 95))
	    bits4FB [vdata->sectorNumber][vdata->sglphase][rS1] = 1;      
      }
      else {
	rS1 = bitslookup[(sN-1)].b.litebitbut[0]+(sD*16);
	if (sN < 48)
	  rS2 = rS1+1; 
	else            /* don't want to go past 48 */
	  rS2 = rS1;

	if ((rS1 > 0) &&(rS1 < 95))
	    bits4FB [vdata->sectorNumber][vdata->sglphase][rS1] = 1;
	if ((rS2 > 0) &&(rS2 < 95))
	    bits4FB [vdata->sectorNumber][vdata->sglphase][rS2] = 1;
      }
    Hv_DrawItem (vdata->ScintDFront16bits, NULL);
    Hv_DrawItem (vdata->ScintDBack32bits, NULL);
    Hv_ChangeFeedback(View, SIMPLE_FB_SCINT, sc_text);
  } else {
    sprintf (sc_text, " ") ;
    Hv_ChangeFeedback(View, SIMPLE_FB_SCINT, sc_text);
    if (hereBefore == 1) {
      hereBefore = 0;
      Hv_DrawItem (vdata->ScintDFront16bits, NULL);
      Hv_DrawItem (vdata->ScintDBack32bits, NULL);
    }
  }

}

/* ------- ScintFeedback ---- */
D 3

static int ScintFeedback(View, pp, scintNum, scintData)

Hv_View             View;
Hv_Point            pp;        /*mouse location*/
int                 *scintNum, *scintData ;
E 3
I 3
/* pp - mouse location */
static int
ScintFeedback(Hv_View View, Hv_Point pp, int *scintNum, int *scintData)
E 3
{
  ViewData    viewdata;
  Hv_Item     temp;
  short       sect;
  short       plane;
  short       i, minid, maxid, j ;
  Hv_Point    poly[4];
  Hv_FPoint   fp1, fp2, jp1, jp2, jp3, jp4;
  Hv_Point    pt[4] ;


  viewdata = GetViewData(View);

  for (j=0; j<4; j++) {
    temp = viewdata->ArrayOScintillators[j];
    if (Hv_PointInRegion(pp, temp->region)) {

/* now we know that we are in a plane. we need to determine which slab */

      plane = temp->user1;
      sect  = temp->user2;
      ScintSlabLimits(plane, &minid, &maxid);
      
      for (i = minid; i <= maxid; i++) {
	GetSlabPolygon(View, sect, i, poly);
	if (Hv_PointInPolygon(pp, poly, 4)) {
          *scintNum = i+1 ;
/* in a scint now compute which piece */
          fp1 = Scintillators[sect].scints[minid].P[2];
          fp2 = Scintillators[sect].scints[minid].P[1];
          jp3.h = fp2.h + (0.3)*(fp1.h - fp2.h) ;
          jp3.v = fp2.v + (0.3)*(fp1.v - fp2.v) ;
    
          jp1 = Scintillators[sect].scints[maxid].P[3];
          jp2 = Scintillators[sect].scints[maxid].P[0];
          jp4.h = jp2.h + (0.3)*(jp1.h - jp2.h) ;
          jp4.v = jp2.v + (0.3)*(jp1.v - jp2.v) ;
  
          Hv_WorldToLocal(View, jp3.h, jp3.v, &pt[2].x, &pt[2].y);
          Hv_WorldToLocal(View, jp4.h, jp4.v, &pt[3].x, &pt[3].y);
          Hv_WorldToLocal(View, jp2.h, jp2.v, &pt[0].x, &pt[0].y); 
          Hv_WorldToLocal(View, fp2.h, fp2.v, &pt[1].x, &pt[1].y); 
          if (Hv_PointInPolygon (pp, pt, 4)) {
            *scintData = 0 ;
            return True ;
          }
          fp1 = Scintillators[sect].scints[minid].P[2];
          fp2 = Scintillators[sect].scints[minid].P[1];
          jp3.h = fp2.h + (0.67)*(fp1.h - fp2.h) ;
          jp3.v = fp2.v + (0.67)*(fp1.v - fp2.v) ;
    
          jp1 = Scintillators[sect].scints[maxid].P[3];
          jp2 = Scintillators[sect].scints[maxid].P[0];
          jp4.h = jp2.h + (0.67)*(jp1.h - jp2.h) ;
          jp4.v = jp2.v + (0.67)*(jp1.v - jp2.v) ;
  
          Hv_WorldToLocal(View, jp3.h, jp3.v, &pt[2].x, &pt[2].y);
          Hv_WorldToLocal(View, jp4.h, jp4.v, &pt[3].x, &pt[3].y);
          Hv_WorldToLocal(View, jp1.h, jp1.v, &pt[0].x, &pt[0].y); 
          Hv_WorldToLocal(View, fp1.h, fp1.v, &pt[1].x, &pt[1].y); 
          if (Hv_PointInPolygon (pp, pt, 4)) {
            *scintData = 2 ;
          } else
            *scintData = 1 ;

	  return True ;
	}
      }
    }
  }  
  return 0;
}

I 4
#else /* UNIX only */

void
feedback_dummy()
{
}

#endif
E 4
E 1
