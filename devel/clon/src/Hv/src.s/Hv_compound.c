/**
 * <EM>Routines for dealing with Hv_Strings with
 * TeX like control sequences.</EM>
 * <P> <B>
 * In Hv, compound strings have (unfortunately) two meanings. One
 * is an <code>Hv_String</code> with embedded Tex-like control
 * codes. Another is a true compound string returned by the
 * native windowing system, such as a Motif string. The latter
 * are type <CODE>Hv_CompoundString</CODE> and should. for the
 * most part, only be used internally.
 * </B>
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


short     dely;

/*------ local prototypes -------*/

static short  Hv_StringBaseline(char  *text,
				short  font);

/*------- Hv_Baseline ---------*/

static short  Hv_StringBaseline(char  *text,
				short  font)
    
{
    Hv_CompoundString  cs;
    short              bl;
    
    if (text == NULL) {
	Hv_Println("Hv benign warning: tried to baseline a NULL string.");
	return 0;
    }

    cs = Hv_CreateCompoundString(text, font);
    bl = Hv_CompoundStringBaseline(cs);
    Hv_CompoundStringFree(cs);  /* free the motif string structure */
    return bl;
}

/**
 * Hv_CompoundStringProcess
 * @purpose  Process a Compound String.
 * @param str    The Hv_String to draw.
 * @param x      Left of String.
 * @param y      Top of String.
 * @param w      Returns width.
 * @param h      Returns width.
 * @param option  If 0, Draws string at x, y and ignores w, h, area,
 * If 1, Finds w & h of string
 */

void Hv_CompoundStringProcess(Hv_String    *str,
			      short        x,
			      short        y,
			      short        *w,
			      short        *h,
				  int          option) {
    
    
/*--------------------------------------------------
  Does bunches of stuff for compound strings:
  
  option = 0   Draws cs at x,y ignores w,h,area
  option = 1   Finds w & h of cs, ignores x & y & area
  
  text in the compound hvstr includes latex like special characters.
  Presently recognized are:
  
  \n     newline
  \\     the backslash itself
  \^     superstring (raise baseline)
  \_     substring (lower baseline)
  \g     use symbol font
  \r     use roman font
  \b     start boldface
  \p     start plain face
  \+     increase font size
  \-     lower font size
  \d     return to default settings
  \s     small space
  \S     big space
  \c     center (relevant for multiline text )
  \l     left justify (default. only use to undo \c)
  
  NOTE: the "default" style is the one specified in the hvstr.
  all \{}'s are departures from that style
  
  ---------------------------------------------------*/
    
    
    
    Hv_String      *next;
    short          xt, yt, ymin, ymax;
    char           *deftext;
    char           *t;
    int            len, contchar;
    short          sw, sh, ww, hh;
    short          defbase, newbase;
    short          extra = 0;
    short          linespace;
    char           *teststring = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
    short          dummy, th, newth;
    short          oldfont;
    Boolean        psvert = False;
    Boolean        center = False;
    short          centdel;      /* extra space to center */
    Boolean        psnomoveto  = False;    /* used for coumpound strings in ps */
    Boolean        psmovedonce = False;    /* used for coumpound strings in ps */
    
    Boolean        DoDraw;
    Boolean        GetDim;


    defbase = 0;

/* work with a copy */
    
    if ((str == NULL) || (str->text == NULL) || ((len = strlen(str->text)) < 1))
	return;
    
/* if I have a strange option return */

    DoDraw = (option == 0);
    GetDim = (option == 1);
    
    if (!(DoDraw || GetDim))
	return;
    
/* work with a copy */
    
    Hv_CopyString(&next, str);

    deftext = next->text;
    t = deftext;
    dely = 0;
    
/* get the height dimension for the default font */

    Hv_CharDimension(teststring, str->font, &dummy, &th);
    linespace = (short)(1.25*((float)th));
    
    
    if (DoDraw) { /* draw */
	Hv_CompoundStringDimension(str, &sw, &sh);
	
/* psvert is "ps vertical" */
	
	psvert = (Hv_inPostscriptMode && !(str->horizontal));
/*	if (psvert)
	    y += sw;*/
	
	xt = x;
	yt = y;
    }
    
    else if (option == 1) { /* dimension */
	*w = 0;
	*h = 0;
	xt = 0;
	yt = 0;
	ymin = 0;
	ymax = th;
	*h = th;
    }
    
    
    while (((t - deftext) < len) && (contchar = Hv_ParseString(t)) >= 0) {
	
/* note: the control character return should be applied to the NEXT segment */
	
/* note: the control characters 'c' and 'l' do not affect size */
	
	next->text = t;
	
	Hv_StringDimension(next, &ww, &hh);
	newbase = Hv_StringBaseline(next->text, next->font);

	if (defbase == 0)
	    defbase = newbase;
	
	if (DoDraw) {  /* draw */
	    
/* center the string? */
	    
	    if (center)
		centdel = (sw - ww)/2;
	    else
		centdel = 0;
	    
	    
/* note the ugly trick in ps mode -- since we cannot use the gaps generrated
   by the x strings to accurately space ourselves in ps coordinates we simply
   avoid the "MoveTo" part of Hv_DrawString by passing the huge negative
   numbers */
	    
	    if (Hv_inPostscriptMode)
		Hv_psnomoveto = (psnomoveto && psmovedonce);

	    
	    if (psvert) {
		yt -= ww;
		Hv_SimpleDrawString((short)(xt + (th - hh) + extra), (short)(yt + centdel), next);
	    }
	    else {
		Hv_SimpleDrawString((short)(xt + centdel), (short)(yt + (defbase - newbase) + extra), next);
		xt += ww;
	    }
	    
	    
/* as long as we have written any kind of string, we have moved */
	    
	    if ((next != NULL) && (next->text) && (strlen(next->text) > 0))
		psmovedonce = True;
	    
	    psnomoveto = False;
	    
	}  /* end DoDraw block */
	
	
	else if (GetDim) { /* dimension */
	    xt += ww;
	    *w = Hv_sMax(*w, xt);
	    if ((next != NULL) && (next->text) && (strlen(next->text) > 0)) {
		Hv_CharDimension(next->text, next->font, &dummy, &newth);
/*		ymin = Hv_sMin(ymin, yt + th - newth); */
		ymin = Hv_sMin(ymin, (short)(yt + defbase - newbase));
	    }
	}  /* end GetDim block */
	
	switch ((char)(contchar)) {
	case 'n':
	    
	    if (DoDraw) {
		if (psvert) {
		    yt = y;
		    xt += linespace;
		}
		else {
		    yt += linespace;
		    xt = x;
		}
	    }
	    else if (GetDim) {
		yt += linespace;
		ymax = Hv_sMax(ymax, (short)(yt + th));
		xt = 0;
	    }
	    break;
	    
	case 'c':
	    if (DoDraw)
		center = True;
	    break;
	    
	case 'l':
	    if (DoDraw)
		center = False;
	    break;

	    
	case 'g':
	    next->font = Hv_CorrespondingFont(next->font, Hv_GREEKFONT);
	    
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	case 'd':
	    next->font = str->font;
	    
	    if (DoDraw) {
		psnomoveto = True;
		Hv_psrely = extra;
	    }
	    extra = 0;
	    break;
	    
	case '^':
	    extra -= 3;
	    
	    if (GetDim)
		ymin = Hv_sMin(ymin, (short)(yt + extra));

	    else if (DoDraw) {
		psnomoveto = True;
		Hv_psrely = 3;
	    }

	    break;
	    
	case '_':
	    extra += 3;
	    
	    if (GetDim)
		ymax = Hv_sMax(ymax, (short)(yt + th + extra));

	    else if (DoDraw) {
		psnomoveto = True;
		Hv_psrely = -3;
	    }
	    break;
	    
	case 'r':
	    next->font = str->font;
	    next->font = Hv_CorrespondingFont(next->font, Hv_ROMANFONT);
	    
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	case  'h':
	    next->font = str->font;
	    next->font = Hv_CorrespondingFont(next->font, Hv_HELVETICAFONT);
	    
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	case 's':
	    if (psvert)
		yt -= 2;
	    else
		xt += 2;
	    break;
	    
	case 'S':
	    if (psvert)
		yt -= 10;
	    else
		xt += 10;
	    break;
	    
	case 'b':
	    next->font = Hv_CorrespondingFont(next->font, Hv_BOLDFONT);
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	case '+':
	    oldfont = next->font;
	    next->font = Hv_CorrespondingFont(next->font, Hv_NEXTBIGGERFONT);
	    if (DoDraw)
		psnomoveto = True;
	    
	    break;
	    
	case '-':
	    oldfont = next->font;
	    next->font = Hv_CorrespondingFont(next->font, Hv_NEXTSMALLERFONT);
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	case 'p':
	    next->font = Hv_CorrespondingFont(next->font, Hv_PLAINFONT);
	    if (DoDraw)
		psnomoveto = True;
	    break;
	    
	default:
	    break;
	}
	
	
	t = t + strlen(t) + 1;
	if (contchar > 0)
	    t++;
    }
    
    next->text = deftext;
    Hv_DestroyString(next);

    if (DoDraw) {
	Hv_psnomoveto = False;
	Hv_psrely = 0;
    }
    else if (GetDim) {
	*h = ymax - ymin + 1;
	if (ymin < 0)
	    dely = -ymin;

    }
}


/**
 * Hv_DrawCompoundString
 * @purpose  Draw a Compound String.
 * @param x      Left of String.
 * @param y      Top of String.
 * @param str    The Hv_String to draw.
 */

void Hv_DrawCompoundString(short x,
			   short y,
			   Hv_String *str) {
    
    short     dummy1, dummy2;

    Hv_CompoundStringProcess(str, x, y, &dummy1, &dummy2, 0);
}



/* ------- Hv_CompoundStringDimension ------------- */

void	Hv_CompoundStringDimension(Hv_String    *str,
				   short     *w,
				   short     *h)
    
/* calculate the pixel width and height of the string,
   given its specified text and font */
    
{
    Hv_CompoundStringProcess(str, 0, 0, w, h, 1);
}


/*-------- Hv_CompoundStringArea ----------*/

void Hv_CompoundStringArea(Hv_String *str,
			   short     x,
			   short     y,
			   Hv_Rect   *area,
			   short     extra) 
    
{
    short  w, h;
    short  e2;

    Hv_CompoundStringDimension(str, &w, &h);
    e2 = 2*extra;
    w += e2;
    h += e2;

    if (str->horizontal) {
	area->left = x - extra;  
	area->top =  y - e2 - dely;
	area->width  = w;
	area->height = h;
    }
    else {
	area->left = x - e2 - dely;  
	area->bottom =  y + extra;
	area->width  = h;
	area->height = w;
	area->top = area->bottom - area->height;
    }

    Hv_FixRectRB(area);
}


/*-------- Hv_CompoundStringXYFromArea ----------*/

void Hv_CompoundStringXYFromArea(Hv_String *str,
				 short     *x,
				 short     *y,
				 Hv_Rect   *area,
				 short     extra) 
    
{
    short  w, h;
    short  e2;

    e2 = 2*extra;
    
    Hv_CompoundStringDimension(str, &w, &h);

    if (str->horizontal) {
	*x = area->left + extra;
	*y = area->top + e2 + dely;
    }
    else {
	*x = area->left + e2 + dely;
	*y = area->bottom - extra;
    }
}


