/*
  ----------------------------------------------------
  -							
  -  File:    track.c				
  -							
  -  Summary:						
  -           Track Drawing
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ------------------------------------------------------
  -  11/19/96	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
*/

#include "ced.h"

/* ------ local prototypes -------*/

static void DrawDOCA(Hv_View   View,
		     Hv_Region region,
		     int       sect,
		     int       layer,
		     int       wire,
		     float     doca);
		     

static void GetTBDOCA(short       *wires,
		      float       *doca,
		      int         trackid);

static HBTRDataPtr  GetHBTR(int trackid);
static HBLADataPtr  GetHBLA(int trackid);
static HDPLDataPtr  GetHDPL(int trackid);

static TBTRDataPtr  GetTBTR(int trackid);
static TBLADataPtr  GetTBLA(int trackid);
static TDPLDataPtr  GetTDPL(int trackid);

static void  SetPoint(Hv_View   View,
		      Hv_Point *xp,
		      int       sect,
		      float     phi,
		      float     z,
		      float     x,
		      int       n);


/*-------- GetNumHBTracks --------*/

int  GetNumHBTracks() {

    int numhbla = 0;
    int numhdpl = 0;
    int i;

    for (i = 0; i < 6; i++) {
	numhbla += bosNumHBLA[i] / 34;  /* division for rows */
	numhdpl += bosNumHDPL[i] / 10;  /* division for rows */
    }

    if (numhbla != numhdpl) {
	fprintf(stderr, "WARNING: number of tracks in HBLA and HDPL differ. Using max.\n");
	return Hv_iMax(numhbla, numhdpl);
    }
    else {
	return numhbla;
    }
}


/*-------- GetNumTBTracks --------*/

int  GetNumTBTracks() {

    int numtbla = 0;
    int numtdpl = 0;
    int i;

    for (i = 0; i < 6; i++) {
	numtbla += bosNumTBLA[i] / 34;  /* division for rows */
	numtdpl += bosNumTDPL[i] / 10;  /* division for rows */
    }

    if (numtbla != numtdpl) {
/*	fprintf(stderr, "WARNING: number of tracks in TBLA and TDPL differ. Using max.\n");*/
	return Hv_iMax(numtbla, numtdpl);
    }
    else {
	return numtbla;
    }
}


/*-------- GetHBTrackSector --------*/

int  GetHBTrackSector(int trackid) {

/*
 * RETURNS C INDEX [0..5] or -1 for error
 */


    HBLADataPtr  hbla;
    int          i;
    int          sect;


/*
 * loop through all the HBLAs and return sect for the 
 * one with matching ID, else return -1
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexHBLA[sect] > 0) {
	    
	    hbla = bosHBLA[sect];
	    
	    for (i = 0; i < (bosNumHBLA[sect] / 34); i++) {
		if ((hbla->trk_pln / 100) == trackid)
		    return sect;
		
		hbla += 34; /* 34 rows */
	    }
	}
    }

    fprintf(stderr, "WARNING: did NOT find HBLA for trackid: %d\n", trackid);

    return -1;
}


/*-------- GetTBTrackSector --------*/

int  GetTBTrackSector(int trackid) {

/*
 * RETURNS C INDEX [0..5] or -1 for error
 */


    TBLADataPtr  tbla;
    TDPLDataPtr  tdpl;
    int          i;
    int          sect;

/*
 * loop through all the TBPLs and return sect for the 
 * one with matching ID, else return -1
 */

    for (sect = 0; sect < 6; sect++) {
	if (bosIndexTDPL[sect] > 0) {
	    
	    tdpl = bosTDPL[sect];
	    
	    for (i = 0; i < (bosNumTDPL[sect] / 10); i++) {
		if ((tdpl->trk_pln / 100) == trackid)
		    return sect;
		
		tdpl += 10; /* 10 rows */
	    }
	}
    }

/*
 * loop through all the TBLAs and return sect for the 
 * one with matching ID, else return -1
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexTBLA[sect] > 0) {
	    
	    tbla = bosTBLA[sect];
	    
	    for (i = 0; i < (bosNumTBLA[sect] / 34); i++) {
		if ((tbla->trk_pln / 100) == trackid)
		    return sect;
		
		tbla += 34; /* 34 rows */
	    }
	}
    }

    fprintf(stderr, "WARNING: did NOT find TBLA or TDPL for trackid: %d\n", trackid);

    return -1;
}


/*--------- GetHBTR --------------*/

static HBTRDataPtr  GetHBTR(int trackid)
{

    HBTRDataPtr  hbtr;
    int          i;
    int          sect;
    int          trackinsect;
    HDPLDataPtr  hdpl;

/*
 * trackid is just a pointer to the correct
 * hbtrk, it should NOT be used to match the track part
 * of itr_sec
 */

    if ((bosIndexHBTR > 0) && (trackid > 0) && (bosNumHBTR >= trackid)) {
	return bosHBTR + (trackid-1);
    }

    fprintf(stderr, "WARNING: did NOT find HBTR for trackid: %d\n", trackid);
    return NULL;
}


/*--------- GetTBTR --------------*/

static TBTRDataPtr  GetTBTR(int trackid)
{
    TDPLDataPtr  tdpl;

/*
 * trackid is just a pointer to the correct
 * tbtrk, it should NOT be used to match the track part
 * of itr_sec
 */

    if ((bosIndexTBTR > 0) && (trackid > 0) && (bosNumTBTR >= trackid)) {
	return bosTBTR + (trackid-1);
    }

    fprintf(stderr, "WARNING: did NOT find TBTR for trackid: %d\n", trackid);
    return NULL;
}


/*--------- GetHBLA --------------*/

static HBLADataPtr  GetHBLA(int trackid)
{

    HBLADataPtr  hbla;
    int          i;
    int          sect;

/*
 * loop through all the HBLAs and return the 
 * one with matching ID, else return NULL
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexHBLA[sect] > 0) {
	    
	    hbla = bosHBLA[sect];
	    
	    for (i = 0; i < (bosNumHBLA[sect] / 34); i++) {
		if ((hbla->trk_pln / 100) == trackid)
		    return hbla;
		
		hbla += 34; /* 34 rows */
	    }
	}
    }

    fprintf(stderr, "WARNING: did NOT find HBLA for trackid: %d\n", trackid);
    return NULL;
}


/*--------- GetHDPL --------------*/

static HDPLDataPtr  GetHDPL(int trackid)
{

    HDPLDataPtr  hdpl;
    int          i;
    int          sect;

/*
 * loop through all the HDPLs and return the 
 * one with matching ID, else return NULL
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexHDPL[sect] > 0) {
	    
	    hdpl = bosHDPL[sect];
	    
	    for (i = 0; i < (bosNumHDPL[sect] / 10); i++) {
		if ((hdpl->trk_pln / 100) == trackid)
		    return hdpl;
		
		hdpl += 10;  /* 10 rows */
	    }
	}
    }

    fprintf(stderr, "WARNING: did NOT find HDPL for trackid: %d\n", trackid);
    return NULL;
}


/*--------- GetTBLA --------------*/

static TBLADataPtr  GetTBLA(int trackid)
{

    TBLADataPtr  tbla;
    int          i;
    int          sect;

/*
 * loop through all the TBLAs and return the 
 * one with matching ID, else return NULL
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexTBLA[sect] > 0) {
	    
	    tbla = bosTBLA[sect];
	    
	    for (i = 0; i < (bosNumTBLA[sect] / 34); i++) {
		if ((tbla->trk_pln / 100) == trackid)
		    return tbla;
		
		tbla += 34; /* 34 rows */
	    }
	}
    }

    return NULL;
}


/*--------- GetTDPL --------------*/

static TDPLDataPtr  GetTDPL(int trackid)
{

    TDPLDataPtr  tdpl;
    int          i;
    int          sect;

/*
 * loop through all the TDPLs and return the 
 * one with matching ID, else return NULL
 */


    for (sect = 0; sect < 6; sect++) {
	if (bosIndexTDPL[sect] > 0) {
	    
	    tdpl = bosTDPL[sect];
	    
	    for (i = 0; i < (bosNumTDPL[sect] / 10); i++) {
		if ((tdpl->trk_pln / 100) == trackid)
		    return tdpl;
		
		tdpl += 10;  /* 10 rows */
	    }
	}
    }

    return NULL;
}


/*--------- DrawHBTrackBanks ----------*/

void DrawHBTrackBanks(Hv_View         View,
		      int             sect,
		      Hv_Region       cliprgn)

/*
 * sect is a C Index [0..5]
 */
    
{
    int          j, k;
    ViewData     viewdata;
    Hv_Point     *xp;
    int          np;
    int          trackid;
    HBTRDataPtr  hbtr;
    int          charge = 99;
    short        sym;

    viewdata = GetViewData(View);

    if (!viewdata->displayHBT)
	return;
    
/* do not show tracks in "accumulate" mode */
    
    if (viewdata->showwhat != SINGLEEVENT)
	return;

    
    for (j = 0; j < GetNumHBTracks(); j++) {
	trackid = j+1;
	
	switch (View->tag) {
	    
	case SECTOR:

	    if (sect != GetHBTrackSector(trackid))
		break;

	    GetHBTrackPoly(View, trackid, &xp, &np, &hbtr);



	    if ((xp == NULL) || (np < 2))
		break;


/* finally ready to draw the track! */

	    charge = 99;

	    if (hbtr != NULL)
		charge = (int)(hbtr->q);

	    if (charge == 99) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
		sym = Hv_DIAMONDSYMBOL;
	    }

	    else if (charge == 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
		sym = Hv_XSYMBOL;
	    }

	    else if (charge < 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
		sym = Hv_XSYMBOL;
	    }

	    else if (charge > 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
		sym = Hv_XSYMBOL;
	    }



	    for (k = 0; k < (np-1); k++)
		Hv_DrawLine(xp[k].x, xp[k].y, xp[k+1].x, xp[k+1].y, viewdata->hb_color);

/* symbols on the points ? */

	    Hv_DrawSymbolsOnPoints(xp, np, 5, Hv_black, sym);


	    Hv_Free(xp);
	    break;
	    
	    
	case ALLDC:
	    break;
	}
    }

    Hv_SetLineStyle(0,Hv_SOLIDLINE);

}



/*--------- DrawTBTrackBanks ----------*/

void DrawTBTrackBanks(Hv_View         View,
		      int             sect,
		      Hv_Region       cliprgn) {
    int          j, k;
    ViewData     viewdata;
    Hv_Point     *xp;
    int          np;
    int          trackid;
    TBTRDataPtr  tbtr;
    int          charge = 99;
    short        sym;
    short        wires[34];
    float        doca[34];
    int          layer;

    viewdata = GetViewData(View);

    if (!viewdata->displayTBT)
	return;
    
/* do not show tracks in "accumulate" mode */
    
    if (viewdata->showwhat != SINGLEEVENT)
	return;

    
    for (j = 0; j < GetNumTBTracks(); j++) {
	trackid = j+1;
	
	switch (View->tag) {
	    
	case SECTOR:

	    if (sect != GetTBTrackSector(trackid))
		break;

	    GetTBTrackPoly(View, trackid, &xp, &np, &tbtr);



	    if ((xp == NULL) || (np < 2))
		break;


/* finally ready to draw the track! */

	    charge = 99;

	    sym = Hv_XSYMBOL;

	    if (tbtr != NULL)
		charge = (int)(tbtr->q);

	    if (charge == 99) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
		sym = Hv_DIAMONDSYMBOL;
	    }

	    else if (charge == 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
	    }

	    else if (charge < 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
	    }

	    else if (charge > 0) {
		Hv_SetLineStyle(0,Hv_SOLIDLINE);
	    }



	    for (k = 0; k < (np-1); k++)
		Hv_DrawLine(xp[k].x, xp[k].y, xp[k+1].x, xp[k+1].y, viewdata->tb_color);

/* symbols on the points */


	    if (viewdata->displayDCA) {
		GetTBDOCA(wires, doca, trackid);
		if (wires[0] > -2) {
		    for (k = 0; k < 34; k++) { 
			if (k < 4)
			    layer = k;
			else
			    layer = k + 2;

			if (wires[k] >= 0)
			    DrawDOCA(View, cliprgn, sect, layer, wires[k], doca[k]); 
			
		    }
		    
		}
	    }

	    else
		Hv_DrawSymbolsOnPoints(xp, np, 5, Hv_black, sym);
	    
	    Hv_Free(xp);
	    break;
	    
	    
	case ALLDC:
	    break;
	}
    }

    Hv_SetLineStyle(0,Hv_SOLIDLINE);
}

/* ------- DrawDOCA --------*/

static void DrawDOCA(Hv_View   View,
		     Hv_Region region,
		     int       sect,
		     int       layer,
		     int       wire,
		     float     doca) {

/* all indices (sect, layer, wire) are already C indices */

    short            rgn, lay, supl;
    ViewData         viewdata = GetViewData(View);
    float            phi, fx, fz;

    if (wire < 0)
	return;

    rgn = layer / 12;
    supl = (layer % 12) / 6;
    lay = layer % 6;
    phi = Hv_DEGTORAD*viewdata->dphi;
    
	
    SenseWirePosition(phi, sect, rgn, supl, lay, wire, &fz, &fx);

    Hv_DrawWorldCircle(View, fz, fx, fabs(doca), True,
		       Hv_black, dcdcacolor);  

}

		     

/*--------- GetTBDOCA ---------*/

static void GetTBDOCA(short       *wires,
		      float       *doca,
		      int         trackid) {

    int          i;
    TBLADataPtr  tbla;
    int          sect;
    int          wire;

    tbla = GetTBLA(trackid);

    if (tbla == NULL) {
	wires[0] = -2;
	return;
    }

    sect = GetTBTrackSector(trackid);

    if (sect < 0) {
	wires[0] = -2;
	return;
    }

    for (i = 0; i < 34; i++) {
	wire = Hv_iMax(0, tbla[i].wire);
	wires[i] = wire - 1;
	doca[i]  = tbla[i].calcdoca;
    }


}

		      

/*---------- GetHBTrackPoly ----------*/

void GetHBTrackPoly(Hv_View      View,
		    int          trackid,
		    Hv_Point     **xp,
		    int          *np,
		    HBTRDataPtr  *hbtr)
    
/* "trackid" specifies which track 
   "hbtr" is the matching HBTR record, if any.

   note that trackid in HBTR is different than trackid
   in the hit banks. 
*/
    
{
    HBLADataPtr hbla;
    HDPLDataPtr hdpl;
    int         j;
    int         sect;
    float       phi;
    ViewData    viewdata;

    *np = 0;
    *xp = NULL;

    *hbtr = GetHBTR(trackid);
    hbla = GetHBLA(trackid);
    hdpl = GetHDPL(trackid);

    if ((hbla == NULL) && (hdpl == NULL))
	return;


    sect = GetHBTrackSector(trackid);

    if (sect < 0) {
	return;
    }

    viewdata = GetViewData(View);
    phi = Hv_DEGTORAD*viewdata->dphi;

    *xp = Hv_NewPoints(44);

/* fprintf(stderr, "get TrackPoly for sector: %d\n", sect+1); */

    if (hdpl != NULL) {
	for (j = 0; j < 3; j++) {
	    if (fabs(hdpl[j].x) < 999.0) {
		SetPoint(View, *xp, sect, phi, hdpl[j].z, hdpl[j].x, *np); 
		*np += 1;
	    }
	}

	for (j = 0; j < 34; j++) {
	    if (fabs(hbla[j].x) < 999.0) {
		SetPoint(View, *xp, sect, phi, hbla[j].z, hbla[j].x, *np); 
		*np += 1;
	    }
	}

	for (j = 3; j < 10; j++) {
	    if (fabs(hdpl[j].x) < 999.0) {
		SetPoint(View, *xp, sect, phi, hdpl[j].z, hdpl[j].x, *np); 
		*np += 1;
	    }
	}

    }


/* if not a good poly, clean up here */

    if (*np < 2) {
	Hv_Free(*xp);
	*xp = NULL;
	*np = 0;
    }
}

/*---------- GetTBTrackPoly ----------*/

void GetTBTrackPoly(Hv_View      View,
		    int          trackid,
		    Hv_Point     **xp,
		    int          *np,
		    TBTRDataPtr  *tbtr)
    
/* "trackid" specifies which track 
   "tbtr" is the matching TBTR record, if any.

   note that trackid in TBTR is different than trackid
   in the hit banks. 
*/
    
{
    TBLADataPtr tbla;
    TDPLDataPtr tdpl;
    int         j;
    int         sect;
    float       phi;
    ViewData    viewdata;

    *np = 0;
    *xp = NULL;

    *tbtr = GetTBTR(trackid);
    tbla = GetTBLA(trackid);
    tdpl = GetTDPL(trackid);

    if ((tbla == NULL) && (tdpl == NULL))
	return;


    sect = GetTBTrackSector(trackid);

    if (sect < 0) {
	return;
    }

    viewdata = GetViewData(View);
    phi = Hv_DEGTORAD*viewdata->dphi;

    *xp = Hv_NewPoints(44);

/* fprintf(stderr, "get TrackPoly for sector: %d\n", sect+1); */

    if (tbla != NULL) {
	for (j = 0; j < 34; j++) {
	    if (fabs(tbla[j].x) < 999.0) {
		SetPoint(View, *xp, sect, phi, tbla[j].z, tbla[j].x, *np); 
		*np += 1;
	    }
	}
    }

    else if (tdpl != NULL) {
	for (j = 0; j < 10; j++) {
	    if (fabs(tdpl[j].x) < 999.0) {
		SetPoint(View, *xp, sect, phi, tdpl[j].z, tdpl[j].x, *np); 
		*np += 1;
	    }
	}
    }

/* if not a good poly, clean up here */

    if (*np < 2) {
	Hv_Free(*xp);
	*xp = NULL;
	*np = 0;
    }
}

/*------ SetPoint ---------*/

static void  SetPoint(Hv_View   View,
		      Hv_Point *xp,
		      int       sect,
		      float     phi,
		      float     z,
		      float     x,
		      int       n) {

    short xs, ys;

    if (sect >= 3)
	x = -x;

    if (fabs(phi) > 0.01)
	x  =x/cos(phi);

    Hv_WorldToLocal(View, z, x, &xs, &ys);
    xp[n].x = xs;
    xp[n].y = ys;

}






