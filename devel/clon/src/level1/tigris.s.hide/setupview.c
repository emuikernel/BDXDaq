
#ifndef VXWORKS

/* setupview.c */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"


/* ----------------- DoTheBigViewSetup -------------------- */

void
DoBigViewSetup(Hv_View View, Hv_Rect *hotrect)
{
  Hv_Item      sebox, bbox, it ;
  Hv_Item      ts1 ;
  int          vs, i ;
  ViewData     vdata;

  vdata = GetViewData (View) ;

  /*
    draw the bounding box for the radio button
    draw the text the will appear
    and finally create bounding box to put all of Dave Damn bits in
    */
  sebox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_BALLOON,           (char *)"Select Which Memory Phase",
                         Hv_LEFT,              hotrect->right + 15,
                         Hv_TOP,               (hotrect->top-5),
                         Hv_WIDTH,             (View->local->right - hotrect->right - 15),
                         Hv_HEIGHT,            67,
                         NULL) ;

  ts1 = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_RADIOCONTAINERITEM,
                         Hv_PARENT,            sebox,
                         Hv_LEFT,              sebox->area->left+15,
                         Hv_TOP,               sebox->area->top+8,
                         Hv_FILLCOLOR,         Hv_white,
                         Hv_OPTION,            (char *)" SE 1-6",
                         Hv_OPTION,            (char *)" SE 7-12",
                         Hv_SINGLECLICK,       EVSERadioClick,
                         NULL) ;


  bbox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_BALLOON,           (char *)"Bit representation of the scintillators.",
                         Hv_LEFT,              hotrect->right+7,
                         Hv_TOP,               hotrect->top+70,
                         Hv_WIDTH,             (View->local->right - hotrect->right - 15),
                         Hv_HEIGHT,            330,
                         NULL) ;

  /*
    1st the numbers...

    This is the top set of numbers from 2 by 2 till 16
    */
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+149,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"2",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+177,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"4",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+203,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"6",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+232,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"8",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+254,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"10",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+283,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"12",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+310,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"14",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+343,
                         Hv_TOP,               bbox->area->top+7,
                         Hv_TEXT,              (char *)"16",
                         NULL) ;

/* then a little bit of text. */
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+10,
                         Hv_TOP,               bbox->area->top+73,
                         Hv_TEXT,              (char *)"Scint Data",
                         NULL) ;

/* now a SD label */
  
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+19,
                         Hv_TEXT,              (char *)"SD1",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+37,
                         Hv_TEXT,              (char *)"SD2",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+55,
                         Hv_TEXT,              (char *)"SD3",
                         NULL) ;

/* 
 * top set of (3*16) doughty bit "buttons" 
 * this hold all of the data needed to draw this item
 * the first item represents the first 3 x 16 scint data bits.
 * range : bit 0 - bit 48
 * rbase is the offset into the color table for the forward scintillator colors.
 * vbase again a color lookup index.
 *
*/
  vdata->bbs1 = malloc (sizeof(BitBoxStruct));
  vdata->bbs1->nhordivs = 15;
  vdata->bbs1->nvertdivs  = 2;               /* make height multiple of 2 */
  vdata->bbs1->rangel = 0;
  vdata->bbs1->rangeh = 48;
  vdata->bbs1->rbase = 0;
  vdata->bbs1->vbase = 0;
  vdata->bbs1->color     = Hv_gray15;
  vdata->ScintDFront16bits = Hv_VaCreateItem (View,                 
			Hv_TYPE,               Hv_USERITEM,
			Hv_USERDRAW,           DrawSDbits,
			Hv_PARENT,             bbox,
			Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
			Hv_LEFT,               bbox->area->left+128,
			Hv_TOP,                bbox->area->top+20,
			Hv_WIDTH,              (bbox->area->width-148),
			Hv_HEIGHT,             48,
			Hv_DATA,               vdata->bbs1,
			NULL);

  vdata->ScintDFront16bits->singleclick = bbSDclick;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+143,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"18",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+171,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"23",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+201,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"27",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+228,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"31",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
			 Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+255,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"35",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+283,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"39",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+310,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"43",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+339,
                         Hv_TOP,               bbox->area->top+75,
                         Hv_TEXT,              (char *)"48",
                         NULL) ;

 /* 
  * bottom set of (3*16) doughty bit "buttons" 
  * this hold all of the data needed to draw this item
  * the first item represents the last 3 x 16 scint data bits.
  * range : bit 49 - bit 96
  * rbase is the offset into the color table for the rearward scintillator colors.
  * vbase again a color lookup index.
  */
  
  vdata->bbs2 = malloc (sizeof (BitBoxStruct));
  vdata->bbs2->nhordivs = 15;
  vdata->bbs2->nvertdivs  = 2;
  vdata->bbs2->rangel = 48;
  vdata->bbs2->rangeh = 96;
  vdata->bbs2->rbase = 17;
  vdata->bbs2->vbase = 3;
  vdata->bbs2->color     = Hv_gray10;
  vdata->ScintDBack32bits = Hv_VaCreateItem (View, 
			Hv_TYPE,               Hv_USERITEM,
			Hv_USERDRAW,           DrawSDbits,
			Hv_PARENT,             bbox,
			Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
			Hv_PLACEMENTITEM,      vdata->ScintDFront16bits,
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       20,
			Hv_WIDTH,              (bbox->area->width-148),
			Hv_HEIGHT,             48,
			Hv_DATA,               vdata->bbs2,
			NULL);

  vdata->ScintDBack32bits->singleclick = bbSDclick;
/* now a SD label */
  
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+87,
                         Hv_TEXT,              (char *)"SD4",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+105,
                         Hv_TEXT,              (char *)"SD5",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+123,
                         Hv_TEXT,              (char *)"SD6",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+149,
			 Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"2",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+175,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"4",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+205,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"6",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+232,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"8",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+255,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"10",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+282,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"12",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+310,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"14",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+339,
                         Hv_TOP,               bbox->area->top+140,
                         Hv_TEXT,              (char *)"16",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+152,
                         Hv_TEXT,              (char *)"SP1",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+166,
                         Hv_TEXT,              (char *)"SP2",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+182,
                         Hv_TEXT,              (char *)"SP3",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+196,
                         Hv_TEXT,              (char *)"SP4",
                         NULL) ;

 /* 
  * spare data bits for Dave's bits
  * this holds all of the data needed to draw this item
  * the item represents the 4 x 16 spare data bits.
  * range : bit 0 - bit 64
  * rbase is the offset into the color table for the spare data bits
  * vbase again a color lookup index.
  */

  vdata->bbs3 = malloc (sizeof (BitBoxStruct));
  vdata->bbs3->nhordivs = 15;
  vdata->bbs3->nvertdivs  = 3;
  vdata->bbs3->rangel = 0;
  vdata->bbs3->rangeh = 64;        /* these are not mapped to any functions so I set 'em = 0 */
  vdata->bbs3->rbase = 0;
  vdata->bbs3->vbase = 3;
  vdata->bbs3->color   = Hv_cadetBlue;
  vdata->SpareDataBits = Hv_VaCreateItem (View, 
					   Hv_TYPE,               Hv_USERITEM,
					   Hv_USERDRAW,           DrawSPbits,
					   Hv_PARENT,             bbox,
					   Hv_PLACEMENTITEM,      vdata->ScintDBack32bits,
					   Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
					   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
					   Hv_PLACEMENTGAP,       17,
					   Hv_WIDTH,              (bbox->area->width-148),
					   Hv_HEIGHT,             56,
					   Hv_DATA,               vdata->bbs3,
					   NULL);

  vdata->SpareDataBits->singleclick = bbSPclick;
 /* 
  * Extra data bits for Dave's bits
  * this hold all of the data needed to draw this item
  * the item represents the 6 x 16 extra data bits.
  * range : bit 0 - bit 96
  * rbase is the offset into the color table for the extra data bits
  * vbase again a color lookup index.
  */

  vdata->bbs4 = malloc (sizeof (BitBoxStruct));
  vdata->bbs4->nhordivs = 15;
  vdata->bbs4->nvertdivs  = 5;
  vdata->bbs4->rangel = 0;
  vdata->bbs4->rangeh = 96;        /* these are not mapped to any functions so I set 'em = 0 */
  vdata->bbs4->rbase = 0;
  vdata->bbs4->vbase = 0;
  vdata->bbs4->color     = Hv_dodgerBlue;
  vdata->ExtraDataBits = Hv_VaCreateItem (View, 
			Hv_TYPE,               Hv_USERITEM,
			Hv_USERDRAW,           DrawEDbits,
			Hv_PARENT,             bbox,
			Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
					  /*			Hv_PLACEMENTITEM,      it, */
			Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			Hv_PLACEMENTGAP,       20,			
			Hv_WIDTH,              (bbox->area->width-148),
			Hv_HEIGHT,             96,
			Hv_DATA,               vdata->bbs4,
			NULL);

  vdata->ExtraDataBits->singleclick = bbEDclick;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+10,
                         Hv_TOP,               bbox->area->top+174,
                         Hv_TEXT,              (char *)"Spare Data",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+10,
                         Hv_TOP,               bbox->area->top+272,
                         Hv_TEXT,              (char *)"Extra Data",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+231,
                         Hv_TEXT,              (char *)"FC1",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+247,
                         Hv_TEXT,              (char *)"FC2",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+263,
                         Hv_TEXT,              (char *)"FC3",
                         NULL) ;

  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+278,
                         Hv_TEXT,              (char *)"CC1",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+295,
                         Hv_TEXT,              (char *)"CC2",
                         NULL) ;
  ts1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            bbox,
                         Hv_LEFT,              bbox->area->left+103,
                         Hv_TOP,               bbox->area->top+311,
                         Hv_TEXT,              (char *)"CC3",
                         NULL) ;
}

#else /* UNIX only */

void
setupview_dummy()
{
}

#endif
