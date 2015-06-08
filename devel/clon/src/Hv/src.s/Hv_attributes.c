/**
 * <EM>Functions dealing with attributes for items, views, and dialogs.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


#include "Hv.h"	  /* contains all necessary include files */

/* ------- local prototypes ------- */

static void   Hv_SetDefaultAttributes(Hv_AttributeArray);

/*-------- Hv_VaGetItemAttributes -----------*/

extern void Hv_VaGetItemAttributes(Hv_Item Item,
				   ...)

/****************************************
  This routine cleans up the mess created 
  by many "Hv_Get" routines, which will
  still work but will not be supported
  or documented.
***************************************/

{
  int              argflag = -1;
  char            *errstr;
  int              prevargflag;
  void            *ptr;
  va_list          ap;

  va_start(ap, Item);

/* now get the requests */

  while (argflag != 0) {
    prevargflag = argflag;
    argflag = va_arg(ap, int);  /* this is the attribute */

/* done processing when argflag == 0 */

    if (argflag == 0)
      return;

    ptr = va_arg(ap, void *);   /* pointer to be "stuffed" */


    if ((argflag < 0) || (argflag >= Hv_NUMATTRIBUTES)) {
      errstr = (char *)Hv_Malloc(80);
      sprintf(errstr, "Illegal attribute %d passed to Hv_VaGetItemAttributes.", argflag);
      Hv_Warning(errstr);
      Hv_Free(errstr);
      Hv_Println("Illegal attribute %d passed to Hv_VaGetItemAttributes", argflag);
      Hv_Println("Previously processed attribute %d", prevargflag);
    }

    else { /* proper pointer */
      switch (argflag) {

      case Hv_USER1:
	*((short *)ptr) = Item->user1;
      break;
      
      case Hv_USER2:
	*((short *)ptr) = Item->user2;
      break;

      case Hv_STATE:
	*((short *)ptr) = Item->state;
      break;
      
      case Hv_COLOR:
	*((short *)ptr) = Item->color;
      break;

      case Hv_TAG:
	*((short *)ptr) = Item->tag;
      break;
      
      case Hv_LEFT:
	*((short *)ptr) = Item->area->left;
      break;
      
      case Hv_TOP:
	*((short *)ptr) = Item->area->top;
      break;
      
      case Hv_WIDTH:
	*((short *)ptr) = Item->area->width;
      break;
      
      case Hv_HEIGHT:
	*((short *)ptr) = Item->area->height;
      break;
      
      default:
	Hv_Println("Hv WARNING: attribute [%d] not supported by Hv_VaGetItemAttributes.", argflag);
	Hv_Println("The return pointer should be assumed to contain garbage.");
	break;
      }  /* end switch */
      
    }

  }  /* end while argflag != 0*/
  va_end(ap);
}


/*--------- Hv_VaGetViewAttributes ------------*/

extern void Hv_VaGetViewAttributes(Hv_View View,
				   ...)
/****************************************
  This routine cleans up the mess created 
  by many "Hv_Get" routines, which will
  still work but will not be supported
  or documented.
***************************************/


{
}




/* ------- Hv_GetAttributes -------*/

void Hv_GetAttributes(va_list ap,
		      Hv_AttributeArray attributes,
		      char **optlist,
		      int *tags,
		      int *fonts,
		      int *colors,
		      short *numopt)
{
  int              argflag = -1;
  char             *errstr;
  char             *tstr;
  int              dummy;
  int              prevargflag;

  Hv_SetDefaultAttributes(attributes);

  if (numopt != NULL)
    *numopt = 0;

/* now get the attributes */

  while (argflag != 0) {
    prevargflag = argflag;
    argflag = va_arg(ap, int);

    if ((argflag < 0) || (argflag >= Hv_NUMATTRIBUTES)) {
      errstr = (char *)Hv_Malloc(80);
      sprintf(errstr, "Illegal attribute %d passed to Hv_GetAttributes.", argflag);
      Hv_Warning(errstr);
      Hv_Free(errstr);
      Hv_Println("Illegal attribute %d passed to Hv_GetAttributes", argflag);
      Hv_Println("Previously processed attribute %d", prevargflag);
    }

    else if (argflag > 0) {

/* group according to type */

      if ((argflag == Hv_DRAWCONTROL)
	  || (argflag == Hv_USERDRAWCONTROL)
	  || (argflag == Hv_POPUPCONTROL)
	  || (argflag == Hv_MINVALUE)
	  || (argflag == Hv_MAXVALUE)
	  || (argflag == Hv_CURRENTVALUE)
	  || (argflag == Hv_USER1)
	  || (argflag == Hv_USER2)
	  || (argflag == Hv_EDITABLE)
	  || (argflag == Hv_WORDWRAP)
	  || (argflag == Hv_SHOWVALUE)
	  || (argflag == Hv_INITFROMFILE)
	  || (argflag == Hv_USEEXTENDEDCOLORS)
	  || (argflag == Hv_MOTIONLESSFB)
	  || (argflag == Hv_MBNOACTIVEAREA)
	  || (argflag == Hv_MBTANDEM)
	  || (argflag == Hv_SIMINTERVAL)
	  || (argflag == Hv_MOTIONLESSFBINTERVAL)
	  || (argflag == Hv_DIALOGCLOSEOUT)
	  || (argflag == Hv_CHILDPLACEMENT)
	  || (argflag == Hv_TITLEPLACEMENT)
	  || (argflag == Hv_GRANDCHILDPLACEMENT)
	  || (argflag == Hv_RELATIVEPLACEMENT)) {
	attributes[argflag].i = va_arg(ap, int);
	
}
      else if ((argflag == Hv_STATE)
	       || (argflag == Hv_USEWELCOMEVIEW)
	       || (argflag == Hv_USEVIRTUALVIEW)
	       || (argflag == Hv_VIRTUALVIEWHRW)
	       || (argflag == Hv_VIRTUALWIDTH)
	       || (argflag == Hv_VIRTUALHEIGHT)
	       || (argflag == Hv_DRAWATCREATION)
	       || (argflag == Hv_ACCELERATORTYPE)
	       || (argflag == Hv_TAG)
	       || (argflag == Hv_REASON)
	       || (argflag == Hv_FRAMEWIDTH)
	       || (argflag == Hv_MINWIDTH)
	       || (argflag == Hv_MINHEIGHT)
	       || (argflag == Hv_LEFT)
	       || (argflag == Hv_TOP)
	       || (argflag == Hv_WIDTH)
	       || (argflag == Hv_HEIGHT)
	       || (argflag == Hv_TEXTENTRYMODE)
	       || (argflag == Hv_FONT)
	       || (argflag == Hv_FONT2)
	       || (argflag == Hv_VIRTUALVIEWFBFONT)
	       || (argflag == Hv_NUMROWS)
	       || (argflag == Hv_NUMCOLUMNS)
	       || (argflag == Hv_NUMLABELS)
	       || (argflag == Hv_NUMITEMS)
	       || (argflag == Hv_PRECISION)
	       || (argflag == Hv_NUMVISIBLEITEMS)
	       || (argflag == Hv_BORDERWIDTH)
	       || (argflag == Hv_BORDERCOLOR)
	       || (argflag == Hv_PACKING)
	       || (argflag == Hv_ALIGNMENT)
	       || (argflag == Hv_COLORWIDTH)
	       || (argflag == Hv_COLOR)
	       || (argflag == Hv_PATTERN)
	       || (argflag == Hv_VERSIONNUMBER)
	       || (argflag == Hv_MINCOLOR)
	       || (argflag == Hv_MAXCOLOR)
	       || (argflag == Hv_TEXTCOLOR)
	       || (argflag == Hv_ARMCOLOR)
	       || (argflag == Hv_MBCHOICECOLOR)
	       || (argflag == Hv_FILLCOLOR)
	       || (argflag == Hv_MENUTOGGLETYPE)
	       || (argflag == Hv_VIRTUALVIEWHRFILL)
	       || (argflag == Hv_VIRTUALVIEWFBFILL)
	       || (argflag == Hv_RESIZEPOLICY)
	       || (argflag == Hv_TYPE)
	       || (argflag == Hv_AUTOSIZE)
	       || (argflag == Hv_NORAINBOW)
	       || (argflag == Hv_DOMAIN)
	       || (argflag == Hv_ORIENTATION)
	       || (argflag == Hv_FEEDBACKDATACOLS)
	       || (argflag == Hv_VIRTUALVIEWFBDATACOLS)
	       || (argflag == Hv_HOTRECTHMARGIN)
	       || (argflag == Hv_HOTRECTVMARGIN)
	       || (argflag == Hv_HOTRECTLMARGIN)
	       || (argflag == Hv_HOTRECTTMARGIN)
	       || (argflag == Hv_HOTRECTRMARGIN)
	       || (argflag == Hv_HOTRECTBMARGIN)
	       || (argflag == Hv_VIRTUALVIEWLMARGIN)
	       || (argflag == Hv_VIRTUALVIEWTMARGIN)
	       || (argflag == Hv_VIRTUALVIEWRMARGIN)
	       || (argflag == Hv_VIRTUALVIEWBMARGIN)
	       || (argflag == Hv_HOTRECTWIDTH)
	       || (argflag == Hv_HOTRECTHEIGHT)
	       || (argflag == Hv_CHILDDV)
	       || (argflag == Hv_CHILDDH)
	       || (argflag == Hv_CURRENTCHOICE)
	       || (argflag == Hv_SCROLLPOLICY)
	       || (argflag == Hv_MENUBUTTONSTYLE)
	       || (argflag == Hv_MENUBUTTONBEHAVIOR)
	       || (argflag == Hv_ICONWIDTH)
	       || (argflag == Hv_ICONHEIGHT)
	       || (argflag == Hv_MBICONWIDTH)
	       || (argflag == Hv_MBICONHEIGHT)
	       || (argflag == Hv_MENUBUTTONBEHAVIOR)
	       || (argflag == Hv_FEEDBACKMARGIN)
	       || (argflag == Hv_GAP)
	       || (argflag == Hv_HGAP)
	       || (argflag == Hv_VGAP)
	       || (argflag == Hv_MARGIN)
	       || (argflag == Hv_PLACEMENTGAP)) {

	attributes[argflag].s = (short)va_arg(ap, int);
      }

      else if ((argflag == Hv_ID)
	       || (argflag == Hv_REDRAWTIME))
	attributes[argflag].l = va_arg(ap, long);

      else if ((argflag == Hv_ACCELERATORCHAR) ||
	       (argflag == Hv_LISTSELECTION) ||
	       (argflag == Hv_LISTRESIZEPOLICY))

	attributes[argflag].c = (char)va_arg(ap, int);


      else if ((argflag == Hv_OPTION)) {
	if ((optlist != NULL) && (numopt != NULL))
	  if (*numopt < Hv_MAXCHOICE) {
	    tstr = va_arg(ap, char *);
	    Hv_InitCharStr(&(optlist[*numopt]), tstr);
	    (*numopt)++;
	  }
      }

/* feedback entries may be passed via groups of four values:

       (tag, font, color, text)
       
   in order!  */

      else if ((argflag == Hv_FEEDBACKENTRY)) {
	if ((optlist != NULL) &&
	    (tags != NULL) &&
	    (fonts != NULL) &&
	    (colors != NULL) &&
	    (numopt != NULL))

	  if (*numopt < Hv_MAXAUTOFBENTRIES) {
	    dummy = va_arg(ap, int);
	    tags[*numopt] = dummy;
	    if (dummy == Hv_SEPARATORTAG)
	      optlist[*numopt] = NULL;
	    else {
	      dummy = va_arg(ap, int);
	      fonts[*numopt] = dummy;
	      dummy = va_arg(ap, int);
	      colors[*numopt] = dummy;
	      tstr = va_arg(ap, char *);
	      Hv_InitCharStr(&(optlist[*numopt]), tstr);
	    }
	    (*numopt)++;
	  }
      }
      
      else if ((argflag == Hv_MINZOOMWIDTH)
	       || (argflag == Hv_MAXZOOMWIDTH)
	       || (argflag == Hv_MINZOOMHEIGHT)
	       || (argflag == Hv_MAXZOOMHEIGHT)
	       || (argflag == Hv_SCALEFACTOR)
	       || (argflag == Hv_XMIN)
	       || (argflag == Hv_XMAX)
	       || (argflag == Hv_YMIN)
	       || (argflag == Hv_YMAX)
	       || (argflag == Hv_LATMIN)
	       || (argflag == Hv_LATMAX)
	       || (argflag == Hv_LONGMIN)
	       || (argflag == Hv_LONGMAX)) {
	attributes[argflag].f = (float)(va_arg(ap, double));
      }


/* function pointers */


      else if ((argflag == Hv_USERDRAW)
	       || (argflag == Hv_SIMPROC)
	       || (argflag == Hv_INITIALIZE)
	       || (argflag == Hv_VIEWFINALINIT)
	       || (argflag == Hv_DESTROY)
	       || (argflag == Hv_ICONDRAW)
	       || (argflag == Hv_CUSTOMIZE)
	       || (argflag == Hv_FEEDBACK)
	       || (argflag == Hv_OFFSCREENUSERDRAW)
	       || (argflag == Hv_VALUECHANGED)
	       || (argflag == Hv_CALLBACK)
	       || (argflag == Hv_MENUCHOICECB)
	       || (argflag == Hv_DOUBLECLICK)) {
	  attributes[argflag].fp = (Hv_FunctionPtr)(va_arg(ap, Hv_FunctionPtr));
      }

      else  /* only thing left are the pointers */
	attributes[argflag].v = va_arg(ap, void *);

    } /* end argflag > 0 */

  }
  va_end(ap);
}


/* ------- Hv_SetDefaultAttributes -------*/

static void Hv_SetDefaultAttributes(Hv_AttributeArray attributes)

{
  attributes[Hv_ACCELERATORCHAR].c  = ' ';
  attributes[Hv_LISTSELECTION].c = (char)Hv_SINGLELISTSELECT;
  attributes[Hv_LISTRESIZEPOLICY].c = (char)Hv_VARIABLE;

  attributes[Hv_SIMINTERVAL].i     = 1000;    /* millisec */
  attributes[Hv_MOTIONLESSFBINTERVAL].i     = 1000;    /* millisec */

  attributes[Hv_DRAWCONTROL].i     = 0;
  attributes[Hv_USERDRAWCONTROL].i = 0;
  attributes[Hv_POPUPCONTROL].i    = Hv_STANDARDPOPUPCONTROL;
  attributes[Hv_MINVALUE].i        = 0;
  attributes[Hv_MAXVALUE].i        = 100;
  attributes[Hv_CURRENTVALUE].i    = 50;
  attributes[Hv_USER1].i           = 0;
  attributes[Hv_USER2].i           = 0;
  attributes[Hv_CHILDPLACEMENT].i  = Hv_DEFAULT;
  attributes[Hv_TITLEPLACEMENT].i  = Hv_POSITIONABOVE;
  attributes[Hv_GRANDCHILDPLACEMENT].i  = Hv_DEFAULT;
  attributes[Hv_RELATIVEPLACEMENT].i = Hv_DEFAULT;
  attributes[Hv_EDITABLE].i        = 1; 
  attributes[Hv_WORDWRAP].i        = 0; 
  attributes[Hv_SHOWVALUE].i       = 1; 
  attributes[Hv_INITFROMFILE].i    = 0; 
  attributes[Hv_WMIGNORECLOSE].i   = 0; 
  attributes[Hv_USEEXTENDEDCOLORS].i = 1; 
  attributes[Hv_MOTIONLESSFB].i    = 0; 
  attributes[Hv_MBNOACTIVEAREA].i  = 0; 
  attributes[Hv_MBTANDEM].i        = 0; 
  attributes[Hv_DIALOGCLOSEOUT].i  = Hv_OKBUTTON + Hv_CANCELBUTTON; 

  attributes[Hv_ACCELERATORTYPE].s = Hv_ALT;
  attributes[Hv_TEXTENTRYMODE].s   = Hv_SINGLELINEEDIT;
  attributes[Hv_STATE].s           = Hv_OFF;
  attributes[Hv_ICONWIDTH].s       = 34;
  attributes[Hv_ICONHEIGHT].s      = 28;
  attributes[Hv_MBICONWIDTH].s     = attributes[Hv_ICONWIDTH].s;
  attributes[Hv_MBICONHEIGHT].s    = attributes[Hv_ICONHEIGHT].s;
  attributes[Hv_CURRENTCHOICE].s   = 1;
  attributes[Hv_SCROLLPOLICY].s    = Hv_SCROLLVERTICAL;
  attributes[Hv_MENUBUTTONSTYLE].s = Hv_TEXTBASED;
  attributes[Hv_MENUBUTTONBEHAVIOR].s = Hv_OPTIONBEHAVIOR;
  attributes[Hv_USEVIRTUALVIEW].s  = 0;
  attributes[Hv_USEWELCOMEVIEW].s  = 0;
  attributes[Hv_VIRTUALVIEWHRW].s  = 140;
  attributes[Hv_VIRTUALWIDTH].s    = 5000;
  attributes[Hv_VIRTUALHEIGHT].s   = 4000;
  attributes[Hv_DRAWATCREATION].s  = 0;
  attributes[Hv_FRAMEWIDTH].s      = 2;
  attributes[Hv_TAG].s             = -32767;
  attributes[Hv_REASON].s          = -1;
  attributes[Hv_MINWIDTH].s        = 400;
  attributes[Hv_MINHEIGHT].s       = 300;
  attributes[Hv_LEFT].s            = -32766;
  attributes[Hv_TOP].s             = -32766;
  attributes[Hv_CHILDDH].s         = -32766;
  attributes[Hv_CHILDDV].s         = -32766;
  attributes[Hv_WIDTH].s           = 0;
  attributes[Hv_HEIGHT].s          = 0;
  attributes[Hv_FONT].s            = Hv_fixed2;
  attributes[Hv_FONT2].s           = Hv_fixed2;
  attributes[Hv_MINCOLOR].s        = Hv_red;
  attributes[Hv_MAXCOLOR].s        = Hv_white;
  attributes[Hv_COLORWIDTH].s      = 2;
  attributes[Hv_COLOR].s           = Hv_black;
  attributes[Hv_PATTERN].s         = Hv_DEFAULT;
  attributes[Hv_VERSIONNUMBER].s   = 0;
  attributes[Hv_TEXTCOLOR].s       = Hv_DEFAULT;
  attributes[Hv_ARMCOLOR].s        = Hv_DEFAULT;
  attributes[Hv_MBCHOICECOLOR].s   = Hv_gray11;
  attributes[Hv_FILLCOLOR].s       = Hv_DEFAULT;

  attributes[Hv_MENUTOGGLETYPE].s   = (short)Hv_N_OF_MANY;
  
  attributes[Hv_VIRTUALVIEWHRFILL].s = Hv_DEFAULT;
  attributes[Hv_VIRTUALVIEWFBFILL].s = Hv_DEFAULT;
  attributes[Hv_VIRTUALVIEWFBFONT].s = Hv_helveticaBold11;

  attributes[Hv_BORDERCOLOR].s     = Hv_gray5;
  attributes[Hv_BORDERWIDTH].s     = 0;

  attributes[Hv_AUTOSIZE].s        = 0;
  attributes[Hv_NORAINBOW].s       = 0;
  attributes[Hv_RESIZEPOLICY].s    = Hv_DEFAULT;
  attributes[Hv_TYPE].s            = Hv_USERITEM;
  attributes[Hv_DOMAIN].s          = Hv_DEFAULT;
  attributes[Hv_FEEDBACKMARGIN].s  = -1;
  attributes[Hv_HOTRECTHMARGIN].s  = -1;
  attributes[Hv_HOTRECTVMARGIN].s  = -1;

  attributes[Hv_HOTRECTLMARGIN].s  = 15;
  attributes[Hv_HOTRECTTMARGIN].s  = 15;
  attributes[Hv_HOTRECTRMARGIN].s  = 15;
  attributes[Hv_HOTRECTBMARGIN].s  = 15;

  attributes[Hv_VIRTUALVIEWLMARGIN].s  = 4;
  attributes[Hv_VIRTUALVIEWTMARGIN].s  = 30;
  attributes[Hv_VIRTUALVIEWRMARGIN].s  = 4;
  attributes[Hv_VIRTUALVIEWBMARGIN].s  = 4;

  attributes[Hv_HOTRECTWIDTH].s    = 400;
  attributes[Hv_HOTRECTHEIGHT].s   = 400;
  attributes[Hv_MARGIN].s          = 1;
  attributes[Hv_GAP].s             = 1;
  attributes[Hv_HGAP].s            = 1;
  attributes[Hv_VGAP].s            = 1;
  attributes[Hv_PLACEMENTGAP].s    = 1;
  attributes[Hv_ORIENTATION].s     = Hv_HORIZONTAL;
  attributes[Hv_NUMROWS].s         = 1;
  attributes[Hv_NUMCOLUMNS].s      = 1;
  attributes[Hv_NUMLABELS].s       = 2;
  attributes[Hv_PRECISION].s       = 0;
  attributes[Hv_NUMITEMS].s        = 6;
  attributes[Hv_NUMVISIBLEITEMS].s = 6;
  attributes[Hv_PACKING].s         = Hv_TIGHT;
  attributes[Hv_ALIGNMENT].s       = Hv_BEGINNING;
  attributes[Hv_FEEDBACKDATACOLS].s = 15;
  attributes[Hv_VIRTUALVIEWFBDATACOLS].s = 25;

/* NOTE this color is a pointer since it is
   used at init when colors are as yet undefined
   should be something like:

       Hv_MAINWINDOWCOLOR,     &Hv_powderBlue

*/

  attributes[Hv_MAINWINDOWCOLOR].v  = NULL; 

  attributes[Hv_ALTERNATEVIEWMENU].v = NULL;
  attributes[Hv_MENUCHOICECB].v    = NULL;
  attributes[Hv_ICONDRAW].v        = NULL;
  attributes[Hv_ANIMATIONPROC].v   = NULL;
  attributes[Hv_SIMPROC].v         = NULL;
  attributes[Hv_DATA].v            = NULL;
  attributes[Hv_SINGLECLICK].v     = NULL;
  attributes[Hv_DOUBLECLICK].v     = NULL;
  attributes[Hv_DRAGCALLBACK].v    = NULL;

  attributes[Hv_STRINGARRAY].v    = NULL;
  attributes[Hv_CALLBACKARRAY].v    = NULL;

  attributes[Hv_USERDRAW].v        = NULL;
  attributes[Hv_OFFSCREENUSERDRAW].v  = NULL;
  attributes[Hv_INITIALIZE].v      = NULL;
  attributes[Hv_VIEWFINALINIT].v   = NULL;
  attributes[Hv_USERPTR].v         = NULL;
  attributes[Hv_CUSTOMIZE].v       = NULL;
  attributes[Hv_AFTERDRAG].v       = NULL;
  attributes[Hv_AFTERROTATE].v     = NULL;
  attributes[Hv_AFTEROFFSET].v     = NULL;
  attributes[Hv_FIXREGION].v       = NULL;
  attributes[Hv_CHECKENLARGE].v    = NULL;
  attributes[Hv_DESTROY].v         = NULL;
  attributes[Hv_FEEDBACK].v        = NULL;
  attributes[Hv_PARENT].v          = NULL;
  attributes[Hv_SUBMENU].v         = NULL;
  attributes[Hv_BALLOON].v         = NULL;
  attributes[Hv_TEXT].v            = NULL;
  attributes[Hv_XLABEL].v          = NULL;
  attributes[Hv_YLABEL].v          = NULL;
  attributes[Hv_CONTROLVAR].v      = NULL;
  attributes[Hv_DEFAULTTEXT].v     = NULL;
  attributes[Hv_PLACEMENTITEM].v   = NULL;
  attributes[Hv_TITLE].v           = NULL;
  attributes[Hv_LABEL].v           = NULL;
  attributes[Hv_WELCOMETEXT].v     = NULL;
  attributes[Hv_VALUECHANGED].v    = NULL;
  attributes[Hv_IMAGE].v    = NULL;
  attributes[Hv_PICINFO].v    = NULL;

  attributes[Hv_MINZOOMWIDTH].f    = -1.0;
  attributes[Hv_MAXZOOMWIDTH].f    = -1.0;
  attributes[Hv_MINZOOMHEIGHT].f   = -1.0;
  attributes[Hv_MAXZOOMHEIGHT].f   = -1.0;
  attributes[Hv_SCALEFACTOR].f     = 1.0;
  attributes[Hv_LATMIN].f          = 0.0;
  attributes[Hv_LATMAX].f          = 0.0;
  attributes[Hv_LONGMIN].f         = 0.0;
  attributes[Hv_LONGMAX].f         = 0.0;
  attributes[Hv_XMIN].f            = 0.0;
  attributes[Hv_XMAX].f            = 1.0;
  attributes[Hv_YMIN].f            = 0.0;
  attributes[Hv_YMAX].f            = 1.0;

  attributes[Hv_REDRAWTIME].l      = 0;
  attributes[Hv_ID].l              = Hv_DEFAULT;
}
