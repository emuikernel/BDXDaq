/*
  ----------------------------------------------------
  -							
  -  File:    feedback.c	
  -							
  -  Summary:						
  -           sets up ced's feedback
  -					       
  -                     				
  -  Revision history:				
  -                     				
  -  Date       Programmer     Comments		
  ------------------------------------------------------
  -  10/19/94	DPH		Initial Version	
  -                                              	
  ------------------------------------------------------
  */

#include "ced.h"
#include "feedback.h"


/* -------- local prototypes --------*/



static void EventFeedback(Hv_View   View);

char  *nulltext = "------";


/*-------- InitFeedback -----------*/

void InitFeedback() {
    AllDCInitSharedFeedback();
    CalorimeterInitSharedFeedback();
    ScintViewInitSharedFeedback();
    SectorViewInitSharedFeedback();
}


/*-------- InitSharedFeedback --------------*/

void InitSharedFeedback(SharedFeedback *sf,
			short tag,
			char  *label1,
			char  *label2) {

    sf->currentslot = -1;
    sf->tag = tag;
    Hv_InitCharStr(&(sf->label[0]), label1);
    Hv_InitCharStr(&(sf->label[1]), label2);
}


/* ----- UpdateEventFeedback -----*/

void  UpdateEventFeedback(void)
    
{
    Hv_View    temp;
    Hv_Region  region;
    
    for (temp = Hv_views.first; temp != NULL;  temp = temp->next) 
	if (Hv_ViewIsVisible(temp)) {
	    region = Hv_GetViewRegion(temp);
	    Hv_SetClippingRegion(region);
	    EventFeedback(temp);
	    Hv_DestroyRegion(region);
	}
}

/* ------ ViewFeedback ---------- */

void ViewFeedback(Hv_View  View,
		  Hv_Point pp)
    
{
    
  if (!Hv_ViewIsVisible(View))
    return;

  EventFeedback(View);

    if (View->tag == ALLDC)
	AllDCFeedback(View, pp);
    
    else if (View->tag == SCINT)
	ScintViewFeedback(View, pp);
    
    else if (View->tag == SECTOR)
	SectorViewFeedback(View, pp);

    else if (View->tag == CALORIMETER)
	CalorimeterViewFeedback(View, pp);

}


/*--------SuperLayerNameFeedback -------*/

void SuperLayerNameFeedback(Hv_View  View,
			    short    superlayer,
			    char    *text)
{
    
    if (superlayer == 0)
	sprintf(text, "1 (region 1 stereo)");
    else if (superlayer == 1)
	sprintf(text, "2 (region 1 axial)");
    else if (superlayer == 2)
	sprintf(text, "3 (region 2 axial)");
    else if (superlayer == 3)
	sprintf(text, "4 (region 2 stereo)");
    else if (superlayer == 4)
	sprintf(text, "5 (region 3 axial)");
    else if (superlayer == 5)
	sprintf(text, "6 (region 3 stereo)");
    else
	strcpy(text, " ");
    
    Hv_ChangeFeedback(View, WHATSUPERLAYER, text);
}


/* ------- EventFeedback ---------*/

static void
EventFeedback(Hv_View View)    
{
  char text[60], *tstr, test[80], small[10], evstr[40];
  int i, tmp, trigpattern;

  if(bosTGBI == NULL)
  {
    sprintf(text, "No Triggers");
  }
  else
  {
    trigpattern = (bosTGBI->res1)&0xFFF;
    /*printf("EventFeedback: trigpattern = 0x%08x\n",trigpattern);*/
    test[0] = '\0';
    for(i=0; i<12; i++)
    {
      tmp = 1;
      if(trigpattern & (tmp<<i))
      {
        sprintf(small,"%d ",i+1);
        strcat(test,small);
      }
    }

    if(triggerFilter == 0)
    {
	  strcat(test,"[No Filtering]");
    }
    else
    {
	  strcat(test,"[Filter:");
	  for(i=0; i<16; i++)
      {
	    if(Hv_CheckBit(triggerFilter, triggerBits[i]))
        {
		  sprintf(small," %d",i+1);
		  strcat(test,small);
	    }
      }
	  strcat(test,"]");
    }

    sprintf(text,"%s",test);
  }

  Hv_ChangeFeedback(View, TRIGGERFILTER, text);

  if(bosHEAD == NULL)
  {
	StandardChangeFeedback(View, RUNNUMBER, NULL);
	StandardChangeFeedback(View, EVSOURCE, NULL);
	StandardChangeFeedback(View, EVENT,    NULL);
  }
  else
  {
	sprintf(text, "%d", bosHEAD->RunNumber);
	StandardChangeFeedback(View, RUNNUMBER, text);

	EventTypeString(bosHEAD->EventType, evstr);
	sprintf(text, "%-d [%s]", bosHEAD->EventNumber, evstr);
	StandardChangeFeedback(View, EVENT, text);
	
	if (eventsource == CED_FROMFILE) {
	    if (bosFileName != NULL) {
		tstr = bosFileName;
		Hv_StripLeadingDirectory(&tstr);
		sprintf(text, "%s", tstr);
	    }
	    else
		sprintf(text, "File");
	}
	else if (eventsource == CED_FROMET) {
	    sprintf(text, "ET: %s", ETSTATION);
	}

	else 
	    sprintf(text, "??");
	StandardChangeFeedback(View, EVSOURCE, text);
  }
}


/*--------- StandardChangeFeedback ------*/

void StandardChangeFeedback(Hv_View  View,
			    short   tag,
			    char   *text)
{
    if (text == NULL)
	Hv_ChangeFeedback(View, tag, " ");
    else
	Hv_ChangeFeedback(View, tag, text);
}

/*--------- StandardFloatChangeFeedback ------*/

void StandardFloatChangeFeedback(Hv_View  View,
				 short   tag,
				 float   val,
				 char   *unitstr)
{
    char    text[40];
    
    if (fabs(val) > 1.0e30) {
	Hv_ChangeFeedback(View, tag, " ");
	return;
    }
    
    if (unitstr == NULL)
	sprintf(text, "%-7.2f", val);
    else
	sprintf(text, "%-7.2f %s", val, unitstr);
    
    Hv_ChangeFeedback(View, tag, text);
}

/*--------- StandardIntChangeFeedback ------*/

void StandardIntChangeFeedback(Hv_View  View,
			       short   tag,
			       int    val,
			       char   *unitstr)
    
{
    char    text[40];
    
    if (unitstr == NULL)
	sprintf(text, "%-d", val);
    else
	sprintf(text, "%-d %s", val, unitstr);
    
    Hv_ChangeFeedback(View, tag, text);
}


/*--------- PosDefIntChangeFeedback ------*/

void PosDefIntChangeFeedback(Hv_View  View,
			     short   tag,
			     int    val)
{
    char    text[20];
    
    if (val >= 0) {
	sprintf(text, "%-d", val);
	Hv_ChangeFeedback(View, tag, text);
    }
    else
	Hv_ChangeFeedback(View, tag, " ");
  
}


/*--------- PosIntChangeFeedback ------*/

void PosIntChangeFeedback(Hv_View  View,
			  short   tag,
			  int    val)
{
    char    text[20];
    
    if (val > 0) {
	sprintf(text, "%-d", val);
	Hv_ChangeFeedback(View, tag, text);
    }
    else
	Hv_ChangeFeedback(View, tag, " ");
  
}

/*------------ ClearFeedback ------------*/

void ClearFeedback(Hv_View View,
		   int opt) {

    switch (opt) {
    case CLEAR_TRACK:
	Hv_ChangeFeedback(View, TRACKID, " ");
	Hv_ChangeFeedback(View, CHI2, " ");
	Hv_ChangeFeedback(View, MOMENTUM, " ");
	Hv_ChangeFeedback(View, VERTEX, " ");
	Hv_ChangeFeedback(View, DIRCOSINE, " ");
	Hv_ChangeFeedback(View, CHARGE, " ");
	break;

    case CLEAR_DC:
	Hv_ChangeFeedback(View, WHATSUPERLAYER, " ");
	Hv_ChangeFeedback(View, WHATLAYER, " ");
	Hv_ChangeFeedback(View, WHATWIRE, " ");
	Hv_ChangeFeedback(View, OCCUPANCY, " ");
	break;

    case CLEAR_ALLDC:
	ClearFeedback(View, CLEAR_DC);
	Hv_ChangeFeedback(View, WHATSECTOR, " ");
	Hv_ChangeFeedback(View, ZPOSITION, " ");
	Hv_ChangeFeedback(View, XPOSITION,  " ");
	Hv_ChangeFeedback(View, DISTANCE,  " ");
	Hv_ChangeFeedback(View, ANGLE,   " ");
	Hv_ChangeFeedback(View, ADCL,   " ");
	Hv_ChangeFeedback(View, ADCR,   " ");
	Hv_ChangeFeedback(View, TDCL,   " ");
	Hv_ChangeFeedback(View, TDCR,   " ");
	break;

    case CLEAR_EC:
	Hv_ChangeFeedback(View, IJCOORD,   " ");
	Hv_ChangeFeedback(View, PIXID,     " ");
	Hv_ChangeFeedback(View, PIXENERGY, " ");
	Hv_ChangeFeedback(View, ZPOSITION, " ");
	Hv_ChangeFeedback(View, XPOSITION, " ");
	Hv_ChangeFeedback(View, YPOSITION, " ");
	Hv_ChangeFeedback(View, DISTANCE,  " ");
	Hv_ChangeFeedback(View, ANGLE,     " ");
	Hv_ChangeFeedback(View, PHI,       " ");
	break;

    case CLEAR_LAC:
	Hv_ChangeFeedback(View, WHATSECTOR, " ");
	Hv_ChangeFeedback(View, WHATXY, " ");
	Hv_ChangeFeedback(View, XTDCL,   " ");
	Hv_ChangeFeedback(View, XADCL,   " ");
	Hv_ChangeFeedback(View, YTDCL,   " ");
	Hv_ChangeFeedback(View, YADCL,   " ");
	Hv_ChangeFeedback(View, XTDCR,   " ");
	Hv_ChangeFeedback(View, XADCR,   " ");
	Hv_ChangeFeedback(View, YTDCR,   " ");
	Hv_ChangeFeedback(View, YADCR,   " ");
	break;


    case CLEAR_SCINT:
	Hv_ChangeFeedback(View, WHATSCINT, " ");
	Hv_ChangeFeedback(View, ZPOSITION, " ");
	Hv_ChangeFeedback(View, XPOSITION, " ");
	Hv_ChangeFeedback(View, YPOSITION, " ");
	Hv_ChangeFeedback(View, DISTANCE,  " ");
	Hv_ChangeFeedback(View, ANGLE,     " ");
	Hv_ChangeFeedback(View, PHI,       " ");
	Hv_ChangeFeedback(View, TDCL,   " ");
	Hv_ChangeFeedback(View, ADCL,   " ");
	Hv_ChangeFeedback(View, TDCR,   " ");
	Hv_ChangeFeedback(View, ADCR,   " ");
	break;

    case CLEAR_START:
	Hv_ChangeFeedback(View, WHATSTARTCNT, " ");
	Hv_ChangeFeedback(View, TDCL,   " ");
	Hv_ChangeFeedback(View, ADCL,   " ");
	break;

    }

}


/*------ ChangeSharedFeedback -----------*/

void ChangeSharedFeedback(Hv_View         View,
			  SharedFeedback *sf,
			  short           slot,
			  char           *text) {
    
/* slot  [0..1] */

    if ((slot < 0) || (slot > 1))
	return;

    if (slot != sf->currentslot) {
	if (sf->label[slot] == NULL)
	    Hv_ChangeFeedbackLabel(View, sf->tag, nulltext);
	else
	    Hv_ChangeFeedbackLabel(View, sf->tag, sf->label[slot]);
	sf->currentslot = slot;
    }

    Hv_ChangeFeedback(View, sf->tag, text);

}

/*------ PosDefChangeSharedFeedback -----------*/

void PosDefChangeSharedFeedback(Hv_View         View,
				SharedFeedback *sf,
				short           slot,
				short           val) {

/* slot  [0..1] */

    if ((slot < 0) || (slot > 1))
	return;

    if (slot != sf->currentslot) {
	if (sf->label[slot] == NULL)
	    Hv_ChangeFeedbackLabel(View, sf->tag, nulltext);
	else
	    Hv_ChangeFeedbackLabel(View, sf->tag, sf->label[slot]);
	sf->currentslot = slot;
    }

    PosDefIntChangeFeedback(View, sf->tag, val);

}





