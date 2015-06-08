/*
  ----------------------------------------------------
  -							
  -  File:    calview.c	
  -							
  -  Summary:						
  -           for ascii dumps of banks into the event control
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ------------------------------------------------------
  -  10/2/97	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
  */


#include "ced.h"
#include "bosced.h"

char *sep = "---\n";

/*------- DC0BankCB --------*/

Boolean DC0BankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evdc0[i]) {
	    sprintf(line, "DC0 Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexDC0[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpDC0Bank(bosDC0[i], bosNumDC0[i]);
	    return True;
	}
    return False;
}


/*------- DC1BankCB --------*/

Boolean DC1BankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evdc1[i]) {
	    sprintf(line, "DC1 Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexDC1[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpDC1Bank(bosDC1[i], bosNumDC1[i]);
	    return True;
	}
    return False;
}


/*------- SEGBankCB --------*/

Boolean SEGBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evseg[i]) {
	    sprintf(line, "SEG Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexSEG[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpSEGBank(bosSEG[i], bosNumSEG[i]);
	    return True;
	}
    return False;
}


/*------- SCBankCB --------*/

Boolean SCBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evsc[i]) {
	    sprintf(line, "Scintillator Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexSC[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpSCBank(bosSC[i], bosNumSC[i]);
	    return True;
	}
    return False;
}

/*------- ECBankCB --------*/

Boolean ECBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evec[i]) {
	    sprintf(line, "Forward Angle ECal Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexEC[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpECBank(bosEC[i], bosNumEC[i]);
	    return True;
	}
    return False;
}

/*------- DumpECPIBank --------*/

void DumpECPIBank(PiDataPtr pi, int num)
    
    
{
    char    line[EV_LINE_SIZE];
    int     j, lay, nh, hid;
    char   *header = " sect\t lay \t nhit\t hit \t iloc\t jloc \t  di \t  dj \t  R  \t  E  \n";

    if ((pi == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- ECPI bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);

    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	lay = pi->layer - 9;
	if ((lay < 0) || (lay > 2))
	    lay = 3;

	nh = pi->hitid & 0xFFFF;
	hid = pi->hitid >> 16;

	sprintf(line, "%3d\t%s\t%3d\t%3d\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\n",
		pi->id, eclayernames[lay], nh, hid, pi->iloc,
		pi->jloc, pi->diloc, pi->djloc, pi->r, pi->e);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	pi++;
    }
    
}

/*------- EC1BankCB --------*/

Boolean EC1BankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evec1[i]) {
	    sprintf(line, "Large Angle Ecal Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexEC1[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpEC1Bank(bosEC1[i], bosNumEC1[i]);
	    return True;
	}
    return False;
}

/*------- CCBankCB --------*/

Boolean CCBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evcc[i]) {
	    sprintf(line, "Forward Angle CC Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexCC[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpCCBank(bosCC[i], bosNumCC[i]);
	    return True;
	}
    return False;
}

/*------- CC1BankCB --------*/

Boolean CC1BankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evcc1[i]) {
	    sprintf(line, "Large Angle CC Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexCC1[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpCC1Bank(bosCC1[i], bosNumCC1[i]);
	    return True;
	}
    return False;
}

/*------- HBLABankCB --------*/

Boolean HBLABankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evhbla[i]) {
	    sprintf(line, "HBLA Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexHBLA[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpHBLABank(bosHBLA[i], bosNumHBLA[i]);
	    return True;
	}
    return False;
}


/*------- HDPLBankCB --------*/

Boolean HDPLBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evhdpl[i]) {
	    sprintf(line, "HDPL Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexHDPL[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpHDPLBank(bosHDPL[i], bosNumHDPL[i]);
	    return True;
	}
    return False;
}


/*------- TBLABankCB --------*/

Boolean TBLABankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evtbla[i]) {
	    sprintf(line, "TBLA Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexTBLA[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpTBLABank(bosTBLA[i], bosNumTBLA[i]);
	    return True;
	}
    return False;
}


/*------- TDPLBankCB --------*/

Boolean TDPLBankCB(Hv_Widget w)
    
{
    char    line[EV_LINE_SIZE];
    int     i;
    
    Hv_ClearScrolledText(evtext, &evpos);
    
    for (i = 0; i < 6; i++)
	if (w == evtdpl[i]) {
	    sprintf(line, "TDPL Data for sector %1d  (%-d bytes)\n", i+1,
		    bosBankSize(bosIndexTDPL[i]));
	    Hv_AddLineToScrolledText(evtext, &evpos, line);
	    DumpTDPLBank(bosTDPL[i], bosNumTDPL[i]);
	    return True;
	}
    return False;
}



/*----- DumpHBLAbank -------*/

void DumpHBLABank(HBLADataPtr hbla,
		  int         num)
{


    int  j;
    char *header = "track \tplane \t  x  \t  y  \t  z  \t  cx \t  cy \t  cz \t tlen \tdc1   \tstat  \twire  \tdtime \talpha \twlen  \tsgdoca\tfitdoca\n";
    char line[EV_LINE_SIZE];
    int track, plane;

    
    if ((hbla == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- HBLA bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {

	if ((j > 0) && ((j % 34) == 0)) 
	    Hv_AddLineToScrolledText(evtext, &evpos, sep);

	plane = hbla->trk_pln % 100;
	track = hbla->trk_pln / 100;

	sprintf(line, "  %-3d \t  %-3d \t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%-6d\t%-6d\t%-6d\t%6.1f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
		track, plane,
		hbla->x,  hbla->y,  hbla->z,
		hbla->cx, hbla->cy, hbla->cz,
		hbla->tlen, hbla->dc1, hbla->stat,
		hbla->wire, hbla->dtime, hbla->alpha,
		hbla->wlen, hbla->sgdoca, hbla->fitdoca);

	Hv_AddLineToScrolledText(evtext, &evpos, line);

	hbla++;
    }
}

/*----- DumpHDPLbank -------*/

void DumpHDPLBank(HDPLDataPtr hdpl,
		  int         num)
{
    int  j;

    char *header = "track \tplane \t  x  \t  y  \t  z  \t  cx \t  cy \t  cz \t tlen \n";
    char line[EV_LINE_SIZE];
    int track, plane;
    
    if ((hdpl == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- HDPL bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	if ((j > 0) && ((j % 10) == 0)) 
	    Hv_AddLineToScrolledText(evtext, &evpos, sep);

	plane = hdpl->trk_pln % 100;
	track = hdpl->trk_pln / 100;


	sprintf(line, "  %-3d \t  %-3d \t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
		track, plane,
		hdpl->x,  hdpl->y,  hdpl->z,
		hdpl->cx, hdpl->cy, hdpl->cz,
		hdpl->tlen);
	Hv_AddLineToScrolledText(evtext, &evpos, line);

	hdpl++;
    }
}



/*----- DumpTBLAbank -------*/

void DumpTBLABank(TBLADataPtr tbla,
		  int         num)
{


    int  j;
    char *header = "track \tplane \t  x  \t  y  \t  z  \t  Bx \t  By \t  Bz \t tlen \tdc1   \tstat  \twire  \tdtime \talpha \twlen  \tsgdoca\tfitdoca\tcaldoca\n";
    char line[EV_LINE_SIZE];
    int track, plane;

    
    if ((tbla == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- TBLA bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {

	if ((j > 0) && ((j % 34) == 0)) 
	    Hv_AddLineToScrolledText(evtext, &evpos, sep);

	plane = tbla->trk_pln % 100;
	track = tbla->trk_pln / 100;

	sprintf(line, "  %-3d \t  %-3d \t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%-6d\t%-6d\t%-6d\t%6.1f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
		track, plane,
		tbla->x,  tbla->y,  tbla->z,
		tbla->Bx, tbla->By, tbla->Bz,
		tbla->tlen, tbla->dc1, tbla->stat,
		tbla->wire, tbla->dtime, tbla->alpha,
		tbla->wlen, tbla->sgdoca, tbla->fitdoca, tbla->calcdoca);

	Hv_AddLineToScrolledText(evtext, &evpos, line);

	tbla++;
    }
}

/*----- DumpTDPLbank -------*/

void DumpTDPLBank(TDPLDataPtr tdpl,
		  int         num)
{
    int  j;

    char *header = "track \tplane \t  x  \t  y  \t  z  \t  cx \t  cy \t  cz \t tlen \n";
    char line[EV_LINE_SIZE];
    int track, plane;
    
    if ((tdpl == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- TDPL bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	if ((j > 0) && ((j % 10) == 0)) 
	    Hv_AddLineToScrolledText(evtext, &evpos, sep);

	plane = tdpl->trk_pln % 100;
	track = tdpl->trk_pln / 100;


	sprintf(line, "  %-3d \t  %-3d \t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
		track, plane,
		tdpl->x,  tdpl->y,  tdpl->z,
		tdpl->cx, tdpl->cy, tdpl->cz,
		tdpl->tlen);
	Hv_AddLineToScrolledText(evtext, &evpos, line);

	tdpl++;
    }
}


/*----- DumpHBTRbank -------*/

void DumpHBTRBank(HBTRDataPtr hbtr,
		  int         num)
{
    int  j;
    char *header = "  x  \t  y  \t  z  \t  px \t  py \t  pz \t  q  \t chi2 \t sect \ttrack\n";
    char line[EV_LINE_SIZE];
    int sect, track;

    if ((hbtr == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- HBTR bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	
	track = hbtr->itr_sec % 100;
	sect  = hbtr->itr_sec / 100;

	sprintf(line, " %6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t  %-4.0f\t%6.3f\t  %-4d\t%-d\n",
		hbtr->x, hbtr->y, hbtr->z,
		hbtr->px, hbtr->py, hbtr->pz,
		hbtr->q, hbtr->chi2, sect, track);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hbtr++;
    }
}

/*----- Dumppartbank -------*/


extern void DumpPARTBank(PARTDataPtr part,
			 int         num) {
    int  j;
    char *header = "  x  \t  y  \t  z  \t  E  \t  px \t  py \t  pz \t q \ttrk\t   qpid   \t qtrk \tflags\n";
    char line[EV_LINE_SIZE];

    if ((part == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- PART bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, " %6.3f%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t %-3.0f\t %-3d\t%10.3f\t%6.3f\t%-d\n",
		part->x, part->y, part->z,
		part->E,
		part->px, part->py, part->pz,
		part->q, part->trkid, part->qpid, part->qtrk, part->flags);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	part++;
    }
}


/*----- DumpTBTRbank -------*/

void DumpTBTRBank(TBTRDataPtr tbtr,
		  int         num)
{
    int  j;
    char *header = "  x  \t  y  \t  z  \t  px \t  py \t  pz \t  q  \t chi2 \thbtr#\t sect \ttrack\n";
    char line[EV_LINE_SIZE];
    int sect, track;

    if ((tbtr == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- TBTR bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	
	track = tbtr->itr_sec % 100;
	sect  = tbtr->itr_sec / 100;

	sprintf(line, " %6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t  %-4.0f\t%6.3f\t  %-4d\t%-4d\t%-d\n",
		tbtr->x, tbtr->y, tbtr->z,
		tbtr->px, tbtr->py, tbtr->pz,
		tbtr->q, tbtr->chi2, tbtr->itr_hbt, sect, track);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	tbtr++;
    }
}

/*----- DumpMCINbank -------*/

void DumpMCINBank(MCINDataPtr mcin,
			 int         num)
{
    int  j, k, ioflag;
    char *header = " Xo  \t Yo  \t Zo  \t Px  \t Py  \t Pz  \tP[GeV]\tM[Mev]\t  Q   \tParticle\n";
    char pname[12];
    char line[EV_LINE_SIZE];
    
    if ((mcin == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- MCIN bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
#ifdef WITH_OFFLINE
	ioflag = jpname_(&(mcin->LundID), pname);
#else
    ioflag = 0;
#endif
	if (ioflag == 0) {
	    for (k = 11; k > 0; k--)
		if (pname[k] != ' ')
		    break;
		else
		    pname[k] = '\0';
	}
	else
	    strcpy(pname, "??");
	sprintf(line, "%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%-6.3f\t%-6.1f\t%3.0f   \t%s\n",
		mcin->Xo, mcin->Yo, mcin->Zo, mcin->Px, mcin->Py, mcin->Pz, mcin->P, 1000.0*mcin->M, mcin->Q, pname);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	mcin++;
    }
    
}


/*----- DumpMCVXbank -------*/

void DumpMCVXBank(MCVXDataPtr mcvx,
			 int         num)
{
    int  j;
    char *header = " X   \t Y   \t Z   \t tof \tvtxflag\n";
    char line[EV_LINE_SIZE];
    
    if ((mcvx == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- MCVX bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, "%6.3f\t%6.3f\t%6.3f\t%6.3f\t%d\n",
		mcvx->X, mcvx->Y, mcvx->Z, mcvx->tof, mcvx->vtx_flag);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	mcvx++;
    }
    
}


/*----- DumpMCTKbank -------*/

void DumpMCTKBank(MCTKDataPtr mctk,
			 int         num)
{
    int  j, k, ioflag;
    char *header = " Cx  \t Cy  \t Cz  \tP[Gev]\tM[Mev]\t  Q  \ttflag \tb_vtx \te_vtx \tparent\tParticle\n";
    char pname[12];
    char line[EV_LINE_SIZE];
    
    if ((mctk == NULL) || (num == 0))
	return;
    
    sprintf(line, "---- MCTK bank ----  count: %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	
#ifdef WITH_OFFLINE
	ioflag = jpname_(&(mctk->LundID), pname);
#else
    ioflag = 0;
#endif	
	if (ioflag == 0) {
	    for (k = 11; k > 0; k--)
		if (pname[k] != ' ')
		    break;
		else
		    pname[k] = '\0';
	}
	else
	    strcpy(pname, "??");
	sprintf(line, "%6.3f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%3.0f   \t%-6d\t%-6d\t%-6d\t%-6d\t%s\n",
		mctk->Cx, mctk->Cy, mctk->Cz,
		mctk->P,  1000.0*mctk->M,  mctk->Q,
		mctk->track_flag, mctk->beg_vtx, mctk->end_vtx, mctk->parent, pname);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	mctk++;
    }
    
}


/*------- DumpHEADBank --------*/

void DumpHEADBank(HEADDataPtr head)
    
{
    char line[EV_LINE_SIZE];
    char evstr[40];
    int  i;
    int  *bs;
    
    if (head == NULL)
	return;
    
    sprintf(line, "\n-------  HEAD bank --------\n");
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    sprintf(line, "\tVersion Number\t%d\n", head->VersionNumber);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    sprintf(line, "\t    Run Number\t%d\n", head->RunNumber);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    sprintf(line, "\t  Event Number\t%d\n", head->EventNumber);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
/*  sprintf(line, "\t    Event Time\t%d\n", head->EventTime); */
    sprintf(line, "\t    Event Time\t%s\n", ctime((time_t *)(&(head->EventTime))));
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    EventTypeString(head->EventType, evstr);
    sprintf(line, "\t    Event Type\t%s\n", evstr);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    sprintf(line, "\t    ROC Status\t%d\n", head->ROCStatus);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    sprintf(line, "\t   Event Class\t%d\n", head->EventClass);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    sprintf(line, "\t  Event Weight\t%d\n", head->EventWeight);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    sprintf(line, "\nData per sector (Bytes)\n");
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    sprintf(line, "\t1  \t2  \t3  \t4  \t5  \t6\n");
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
    bs = bosSectorDataSize;
    for (i = 0; i < 6; i++) {
	sprintf(line, "\t%-d", bs[i]);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
    }
    sprintf(line, "\n---------------------------\n");
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    
}


/*----- DumpDC0Bank -------*/

void DumpDC0Bank(DC0DataPtr  hits,
			int     num)
    
{
    int  j;
    unsigned char layer, wire;
    char *header = "\t layer\t  wire\t   tdc\n\t-----------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "\tDC Number of DC0 Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	BreakShort(hits->id, &wire, &layer);
	sprintf(line, "\t%6d\t%6d\t%6d\n", layer, wire, hits->tdc);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}


/*----- DumpDC1Bank -------*/

void DumpDC1Bank(DC1DataPtr  hits,
		 int     num)
    
{
    int  j;
    unsigned char layer, wire;
    char *header = "\t layer\t  wire\t   time\n\t-----------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "\tDC Number of DC1 Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	BreakShort((short)(hits->id), &wire, &layer);
	sprintf(line, "\t%6d\t%6d\t%f\n", layer, wire, hits->time);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}


/*----- DumpSEGBank -------*/

void DumpSEGBank(SEGDataPtr  hits,
			int     num)
    
{
    int  j;
    char *header = "\t  supl\t  seg\n\t-----------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "\tNumber of Segments = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, "\t%6d\t%6d\n", hits->superlayer, hits->segid);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}

/*----- DumpSCBank -------*/

void DumpSCBank(SCDataPtr  hits,
		       int     num)
    
{
    int  j;
    char *header = "\t    id\t tdc L\t adc L\t tdc R\t adc R\n\t---------------------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "\tSC Number of Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line,  "\t%6d\t%6d\t%6d\t%6d\t%6d\n", hits->id, hits->tdcleft,
		hits->adcleft, hits->tdcright, hits->adcright);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}


/*----- DumpCCBank -------*/

void DumpCCBank(NormalDataPtr  hits,
		       int       num)
    
{
    int  j;
    char *header = "\t    ID\t   tdc\t   adc\n\t------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "CC Number of Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, "\t%6d\t%6d\t%6d\n", hits->id, hits->tdc, hits->adc);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}

/*----- DumpCC1Bank -------*/

void DumpCC1Bank(NormalDataPtr  hits,
			int       num)
    
{
    int  j;
    char *header = "\t    ID\t   tdc\t   adc\n\t------------------------\n";
    char line[EV_LINE_SIZE];
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "CC1 Number of Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, "\t%6d\t%6d\t%6d\n", hits->id, hits->tdc, hits->adc);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}

/*----- DumpSTN0Bank -------*/

void DumpSTN0Bank(STN0DataPtr  hits,
		  int          num) {

  int            j;
  char          *header = "\t sector\t  tdc\n\t-----------------------------\n";
  char           line[EV_LINE_SIZE];
  
  if ((hits == NULL) || (num == 0)) {
    return;
  }
  
  sprintf(line, "Start Counter Number of TDCs = %d\n", num);
  Hv_AddLineToScrolledText(evtext, &evpos, line);
  Hv_AddLineToScrolledText(evtext, &evpos, header);
  
  for (j = 0; j < num; j++) {
    sprintf(line, "\t%6d\t%6d\t%6d\n", hits->id, hits->tdc);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    hits++;
  }
  

}

/*----- DumpSTN1Bank -------*/

void DumpSTN1Bank(STN1DataPtr  hits,
		  int          num) {
  int            j;
  char          *header = "\t sector\t  adc\n\t-----------------------------\n";
  char           line[EV_LINE_SIZE];
  
  if ((hits == NULL) || (num == 0)) {
    return;
  }
  
  sprintf(line, "Start Counter Number of ADCs = %d\n", num);
  Hv_AddLineToScrolledText(evtext, &evpos, line);
  Hv_AddLineToScrolledText(evtext, &evpos, header);
  
  for (j = 0; j < num; j++) {
    sprintf(line, "\t%6d\t%6d\t%6d\n", hits->id, hits->adc);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    hits++;
  }
  
}


/*----- DumpTAGEBank -------*/

void DumpTAGEBank(TAGEDataPtr  hits,
		  int          num)
    
{
    int            j;
    char          *header = "\t   ID  \t tdc\n\t-----------------------------\n";
    char           line[EV_LINE_SIZE];

    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "SNumber of Tagger E Counter Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	sprintf(line, "\t%6d\t%6d\n", hits->id, hits->tdc);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
    
}


/*----- DumpTAGTBank -------*/

void DumpTAGTBank(TAGTDataPtr  hits,
		  int          num)
    
{
    int            j;
    char          *header = "\t   ID  \ttdc_L\tdc_R\n\t-----------------------------\n";
    char           line[EV_LINE_SIZE];

    unsigned char  id, dummy;

fprintf(stderr, "in dumptagt hits = %p num = %d\n", hits, num);
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "Number of Tagger T Counter Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	BreakShort(hits->id, &id, &dummy);
	sprintf(line, "\t%6d\t%6d\t%6d\n", id, hits->tdcleft, hits->tdcright);
	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
}

/*----- DumpECBank -------*/

void
DumpECBank(ECDataPtr hits, int num)    
{
  int j;
  unsigned char layer, strip;
  char layernames[7][10];
  static Boolean firsttime = True;
  char *header = "\t plane\t  strip\t   tdc\t   adc\n\t-----------------------------\n";
  char line[EV_LINE_SIZE];
    
  if(firsttime)
  {
    strcpy(layernames[0], "???");
    strcpy(layernames[1], "u-in");
    strcpy(layernames[2], "v-in");
    strcpy(layernames[3], "w-in");
    strcpy(layernames[4], "u-out");
    strcpy(layernames[5], "v-out");
    strcpy(layernames[6], "w-out");
    firsttime = False;
  }

  if((hits == NULL) || (num == 0)) return;

  sprintf(line, "EC Number of Hits = %d\n", num);
  Hv_AddLineToScrolledText(evtext, &evpos, line);
  Hv_AddLineToScrolledText(evtext, &evpos, header);

  for(j=0; j<num; j++)
  {
    BreakShort(hits->id, &strip, &layer);

    if((layer > 6) || (layer < 1))
      sprintf(line, "\t%s\t%6d\t%6d\t%6d\n", layernames[0], strip, hits->tdc, hits->adc);
    else
      sprintf(line, "\t%s\t%6d\t%6d\t%6d\n", layernames[layer], strip, hits->tdc, hits->adc);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    hits++;
  }
    
}


/*----- DumpEC1Bank -------*/

void DumpEC1Bank(EC1DataPtr  hits,
			int     num)
    
{
    char *lnames[] = {"1 (Xin) ",
			     "2 (Yin) ",
			     "3 (Xout)",
			     "4 (Yout)"};

    unsigned char  strip, layer;
    int            j;
    char          *header = "\t layer   strip\t tdc L\t adc L\t tdc R\t adc R\n\t----------------------------------------------\n";
    char           line[EV_LINE_SIZE];
    
    
    if ((hits == NULL) || (num == 0))
	return;
    
    sprintf(line, "EC1 Number of Hits = %d\n", num);
    Hv_AddLineToScrolledText(evtext, &evpos, line);
    Hv_AddLineToScrolledText(evtext, &evpos, header);
    
    for (j = 0; j < num; j++) {
	BreakShort(hits->id, &strip, &layer);
	
	if ((layer > 4) || (layer < 1))
	    sprintf(line, "\t???\t%6d\t%6d\t%6d\t%6d\t%6d\n", strip, 
		    hits->tdcleft, hits->adcleft,
		    hits->tdcright, hits->adcright);
	else
	    sprintf(line, "\t%s %6d\t%6d\t%6d\t%6d\t%6d\n", lnames[layer-1], strip, 
		    hits->tdcleft, hits->adcleft,
		    hits->tdcright, hits->adcright); 
/*	    sprintf(line, "\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\n", layer, strip, 
		    hits->tdcleft, hits->adcleft,
		    hits->tdcright, hits->adcright); */

	Hv_AddLineToScrolledText(evtext, &evpos, line);
	hits++;
    }
    
}


