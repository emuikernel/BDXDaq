h40831
s 00000/00000/00000
d R 1.2 02/08/25 23:21:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_dlogtext.c
e
s 00127/00000/00000
d D 1.1 02/08/25 23:21:08 boiarino 1 0
c date and time created 02/08/25 23:21:08 by boiarino
e
u
U
f e 0
t
T
I 1
/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"


/*------ Hv_AddLineToScrolledText -----------*/

void  Hv_AddLineToScrolledText(Hv_Widget        w,
			       Hv_TextPosition *pos,
			       char            *line)

{
  XmTextInsert(w, *pos, line);
  *pos += strlen(line);
  XmTextSetInsertionPosition(w, *pos);
  XmUpdateDisplay(w);

  if (*pos > Hv_MAX_TEXTSIZE)
    Hv_DeleteScrolledTextLines(w, 1 + (*pos - Hv_MAX_TEXTSIZE)/100, pos);

}

/*--------- Hv_AppendLineToScrolledText --------*/

void Hv_AppendLineToScrolledText(Hv_Widget     w,
				 char          *line)

/* add a line to bottom of scrolled text widget */
{
  Hv_TextPosition    pos;

  Hv_JumpToBottomOfScrolledText(w, &pos);
  XmTextSetInsertionPosition(w, pos);
  XmTextInsert(w, pos, line);
  Hv_JumpToBottomOfScrolledText(w, &pos);
  XmUpdateDisplay(w);

  if (pos > Hv_MAX_TEXTSIZE)
    Hv_DeleteScrolledTextLines(w, 1 + (pos - Hv_MAX_TEXTSIZE)/100, &pos);


}


/*--------- Hv_JumpToBottomOfScrolledText ----------*/

void Hv_JumpToBottomOfScrolledText(Hv_Widget        w,
				   Hv_TextPosition *pos)

/* place cursor at end of buffer and set pos accoringly */


{
  *pos = XmTextGetLastPosition(w);
  XmTextSetInsertionPosition(w, *pos);
}


/*-------- Hv_DeleteLinesFromTop --------*/

void  Hv_DeleteLinesFromTop(Hv_Widget        w,
			    int              nlines)

{
  Hv_TextPosition    pos;

  pos = XmTextGetLastPosition(w);
  Hv_DeleteScrolledTextLines(w, nlines, &pos);
}

/*----------- Hv_DeleteScrolledTextLines ---------*/

void  Hv_DeleteScrolledTextLines(Hv_Widget        w,
				 int              nlines,
				 Hv_TextPosition *endpos)

/* delete n lines from text, presumably from a text widget.
   if nlines is < 0, all text up to endpos is deleted */

{
  char  *buffer;
  char  *newstr;
  int   i;

  buffer = XmTextGetString(w);
  newstr = buffer;

  if (nlines < 0)
    newstr += *endpos;
  else {
    i = 0;
    while ((i < nlines) && (newstr != NULL)) {
      newstr = strstr(newstr, "\n") + 1;
      i++;
    }
  }  
  XmTextSetString(w, newstr);

  *endpos = XmTextGetLastPosition(w);

/*  *endpos -= (newstr - buffer); */

  XtFree(buffer);

}

/*-------- Hv_ClearScrolledText ------*/

void Hv_ClearScrolledText(Hv_Widget        w,
			  Hv_TextPosition *pos)

{
  Hv_DeleteScrolledTextLines(w, -1, pos);
  XmTextShowPosition(w, *pos);
}


E 1
