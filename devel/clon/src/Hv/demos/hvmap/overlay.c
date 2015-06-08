/*
----------------------------------------------------
-							
-  File:    setup.c	
-							
-  Summary:						
-           sets up views
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  08/24/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/


#include "Hv.h"
#include "Hv_maps.h"
#include "maps.h"

static Overlay GetOverlay();

/*-------- ReadOverlay ------*/

void  ReadOverlay(hvevent)

Hv_Event     hvevent;

{
  Hv_View                View = hvevent->view;
  Hv_Item                Item = hvevent->item;
  char                   *fname = NULL;
  FILE                   *fp = NULL;
  char                   *line = NULL;
  Overlay                ovl = NULL;
  int                    n = -1;
  float                  dellat  = 1.0e30;
  float                  dellong = 1.0e30;
  int                    i, nr;

  Hv_ToggleButton(Item);
  fname = Hv_FileSelect("overlay filename", "*.ovl", NULL);


  fp = fopen(fname, "r");


  if (fp != NULL) {
    DestroyOverlay(View);


/* try to read the file. All angles in the file
   are in degrees -- they wil be converted to radians*/

    line = (char *)Hv_Malloc(256*sizeof(char));

    if (Hv_GetNextNonComment(fp,  line))
      sscanf(line, "%d", &n);

    if (Hv_GetNextNonComment(fp,  line))
      nr = sscanf(line, "%f %f", &dellat, &dellong);

    dellat  = Hv_DEGTORAD*fabs(dellat);
    dellat  = Hv_fMax(1.0e-6, dellat);

    if (nr == 1) {
      fprintf(stderr, "Old style format. New style has both dellat and dellong.\n");
      dellong = dellat;
    }
    else {
      dellong = Hv_DEGTORAD*fabs(dellong);
      dellong = Hv_fMax(1.0e-6, dellong);
    }
    
    ovl = MallocOverlay(View, n, dellat, dellong);
    if (ovl != NULL) {

      for (i = 0; i < n; i++)
	if (Hv_GetNextNonComment(fp,  line)) {
	  sscanf(line, "%f %f %d",
		 &(ovl->latitude[i]),
		 &(ovl->longitude[i]),
		 &(ovl->value[i]));
	  
	  ovl->latitude[i]  *= Hv_DEGTORAD;
	  ovl->longitude[i] *= Hv_DEGTORAD;
	  ovl->value[i] = Hv_iMax(0, Hv_iMin(NUMOVERLAY-1, ovl->value[i]));
	}
    }
    Hv_SetViewDirty(View);
    Hv_DrawViewHotRect(View);
    
    Hv_Free(line);
  }

  Hv_Free(fname);
  Hv_ToggleButton(Item);
}


/*--------- MallocOverlay ---------*/

Overlay  MallocOverlay(View, n, dellat, dellong)

Hv_View   View;
int       n;
float     dellat, dellong;    /* RADIANS*/

{
  Overlay    ovl;
  ViewData   viewdata;

  if (View == NULL)
    return NULL;

  if (n < 1) 
    return NULL;

  DestroyOverlay(View);

  ovl = (Overlay)Hv_Malloc(sizeof(OverlayRec));
  ovl->npts = n;
  ovl->dellat  = dellat;
  ovl->dellong = dellong;
  ovl->latitude  = (float *)Hv_Malloc(n*sizeof(float));
  ovl->longitude = (float *)Hv_Malloc(n*sizeof(float));
  ovl->value    = (int *)Hv_Malloc(n*sizeof(int));
  
  viewdata = GetViewData(View);
  viewdata->data = ovl;
  return ovl;
}


/*------- DestroyOverlay ----------*/

void  DestroyOverlay(View)

Hv_View      View;

{
  Overlay       ovl;
  ViewData      viewdata;
  
  ovl = GetOverlay(View);
  if (ovl == NULL)
    return;

  Hv_Free(ovl->latitude);
  Hv_Free(ovl->longitude);
  Hv_Free(ovl->value);
  Hv_Free(ovl);

  viewdata = GetViewData(View);
  viewdata->data = NULL;
}

/*------- DrawOverlay --------*/

void  DrawOverlay(View, region)

Hv_View      View;
Region       region;

{
  Overlay            ovl;
  float              latmin, latmax, longmin, longmax;
  int                i;
  ViewData           viewdata;
  Hv_Region          totrgn = NULL;
  Hv_ViewMapData     mdata;

  ovl = GetOverlay(View);
  if (ovl == NULL)
    return;

  if ((mdata = Hv_GetViewMapData(View)) == NULL)
    return;


  if ((mdata->projection == Hv_ORTHOGRAPHIC) &&
      (mdata->GlobeItem != NULL) &&
      (mdata->GlobeItem->region != NULL)) {
    if (region == NULL)
      Hv_SetClippingRegion(mdata->GlobeItem->region);
    else {
      totrgn = Hv_IntersectRegion(region, mdata->GlobeItem->region);
      Hv_SetClippingRegion(totrgn);
    }
  }


  viewdata = GetViewData(View);
  for (i = 0; i < ovl->npts; i++) {

/* switch to simpler algorithm */

    latmin  = ovl->latitude[i];
    longmin = ovl->longitude[i];

    latmax  = latmin  + ovl->dellat;
    longmax = longmin + ovl->dellong;

    latmax = Hv_fMin(Hv_PIOVER2, latmax);

    Hv_DrawOverlayRect(View,
		    latmin,
		    latmax,
		    longmin,
		    longmax,
		    Hv_GetColorButtonColor(viewdata->ColorButtons[ovl->value[i]]));
  }

  Hv_DrawMapBordersOnly(View, region, True);
  Hv_Flush();
  Hv_DestroyRegion(totrgn);
}

/*------ GetOverlay -------*/

static Overlay GetOverlay(View)

Hv_View    View;

{
  ViewData   viewdata;

  if (View == NULL)
    return NULL;

  viewdata = GetViewData(View);
  return viewdata->data;
}




