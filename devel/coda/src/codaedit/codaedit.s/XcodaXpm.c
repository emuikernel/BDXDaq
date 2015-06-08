/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	Create a pixmap from standard xpm file 
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: XcodaXpm.c,v $
 *   Revision 1.3  1997/11/06 20:02:17  rwm
 *   Xpm fixes.
 *     xpm.h should be in a X11 directory hopefully in /usr/local/include/X11.
 *     Fixed the include and library linking.
 *
 *   Revision 1.2  1997/10/13 15:24:44  heyes
 *   embedded windows
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:20  chen
 *   coda source
 *
 *	  
 */
#include <X11/Intrinsic.h>
#include <X11/xpm.h>
#include <Xm/Xm.h>

#if defined (__STDC__)
Pixmap XcodaCreatePixmapFromXpm(Widget parent,
				char** data,
				int    type)
#else
Pixmap XcodaCreatePixmapFromXpm(parent,data,type)
     Widget parent;
     char   **data;
     int    type;
#endif
{
  Display        *dpy = XtDisplay(parent);
  Window         win = XDefaultRootWindow(dpy);
  Screen         *scr = XDefaultScreenOfDisplay(dpy);
  int            depth = DefaultDepthOfScreen(scr);
  Colormap       cmap = DefaultColormapOfScreen(scr);
  XpmAttributes  attr;
  unsigned long  valuemask = 0;
  int            err;
  unsigned long  pixmap_ret,pixmap_mask;
  XpmColorSymbol col_symbol[1];
  Arg            arg[5];
  int            ac = 0;
  Pixel          parent_bg;

  if(type){ /* normal background for pixmap */
    XtSetArg (arg[ac], XmNbackground, &parent_bg); ac++;
    XtGetValues (parent, arg, ac);
    ac = 0;
  }
  else{  /* inverted or highlighted pixmap */
    XtSetArg (arg[ac], XmNforeground, &parent_bg); ac++;
    XtGetValues (parent, arg, ac);
    ac = 0;
  }    
  col_symbol[0].name = (char *)NULL;
  col_symbol[0].value = (char *)malloc((strlen("LightBlue")+1)*sizeof(char));
  strcpy(col_symbol[0].value,"LightBlue");
  col_symbol[0].pixel = parent_bg;
  
  attr.colormap = cmap;
  attr.depth = depth;
  attr.colorsymbols = col_symbol;
  attr.valuemask = valuemask;
  attr.numsymbols = 1;
  attr.closeness = 65536;
  
  attr.valuemask |= XpmReturnPixels;
  attr.valuemask |= XpmColormap;
  attr.valuemask |= XpmColorSymbols;
  attr.valuemask |= XpmDepth;
  attr.valuemask |= XpmCloseness;
  
  err = XpmCreatePixmapFromData(dpy,win,data,&pixmap_ret,&pixmap_mask,
				&attr);
  free (col_symbol[0].value);
  if(err != XpmSuccess){
    pixmap_ret = NULL;
    return;
  }
  else
    return pixmap_ret;
}
