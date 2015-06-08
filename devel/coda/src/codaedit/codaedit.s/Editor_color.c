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
 *	 Color utilties for CODA Editor
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_color.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:02  heyes
 *   Imported sources
 *
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <math.h>

/**********************************************************************************
 *                void RGB_TO_HSV(r, g, b, h, s, v)                               *
 * Description:                                                                   *
 *     Standard RGB to HSV Color Model Conversion                                 *
 *********************************************************************************/
#if defined (__STDC__)
void RGB_TO_HSV(int r,int g,int b,
		float* h, float* s,float* v)
#else
void RGB_TO_HSV(r,g,b,h,s,v)
int r,g,b;
float *h,*s,*v;
#endif
{
  float max,min;
  float temp;
  float rr,gg,bb;
  float delta;

  rr = r/65535.0;
  gg = g/65535.0;
  bb = b/65535.0;

  max = rr;
  if(gg >= max)
     max = gg;
  if(bb >= max)
     max = bb;

  min = rr;
  if(gg < min)
     min = gg;
  if(bb < min)
     min = bb;
  

  *v = max;
  if(max != 0)
    {
     *s = (max-min)/max;
   }
  else
     *s = 0.0;

  if(*s == 0.0)
    *h = 400.0;
  else
    {
      delta = max - min;
      if(rr == max)
	*h = (gg-bb)/delta;
      else if(gg == max)
	*h = 2 + (bb - rr)/delta;
      else
        *h = 4 + (rr - gg)/delta;

      *h = *h * 60.0;
      if(*h < 0)
        *h = *h + 360;
    }
}

/**********************************************************************************
 *                void RGB_TO_HSV(r, g, b, h, s, v)                               *
 * Description:                                                                   *
 *     Standard HSV to RGB Color Model Conversion                                 *
 *********************************************************************************/
#if defined (__STDC__)
void HSV_TO_RGB(int* r,int* g,int* b,
		float h, float s, float v)
#else
void HSV_TO_RGB(r,g,b,h,s,v)
int *r,*g,*b;
float h,s,v;
#endif
{
  float rr,gg,bb;
  int i;
  float f,p,q,t;

  if(s == 0)
    {
     if(h == 400.0)
       {
        rr = v;
        gg = v;
        bb = v;
      }
   }
  else
    {
      if(h == 360.0)
         h = 0.0;
      h = h/60;
      i = floor(h);
      f = h -i;
      p = v*(1-s);
      q = v*(1-(s*f));
      t = v*(1-(s*(1-f)));
      switch(i)
	{
	case 0:
          rr = v;
          gg = t;
          bb = p;
          break;
        case 1:
          rr = q;
          gg = v;
          bb = p;
          break;
        case 2:
          rr = p;
          gg = v;
          bb = t;
          break;
        case 3:
          rr = p;
          gg = q;
          bb = v;
          break;
        case 4:
          rr = t;
          gg = p;
          bb = v;
          break;
        case 5:
          rr = v;
          gg = p;
          bb = q;
        default:
          break;
	}
    }
  *r = (int)(rr*65535);
  *g = (int)(gg*65535);;
  *b = (int)(bb*65535);;
}

/*******************************************************************************
 *        void XcodaGetShadowColor(dpy win, col, b_shadow, t_shadow)           *
 * Description:                                                                *
 *     Return top and bottom shadow color of a given color pixel               *
 ******************************************************************************/
#if defined (__STDC__)
void XcodaGetShadowColor(Display* dpy,
			 Drawable win,
			 Pixel bg,
			 Pixel* b_shadow,
			 Pixel* t_shadow)
#else
void XcodaGetShadowColor(dpy,win,bg,b_shadow,t_shadow)
Display *dpy;
Drawable win;
Pixel bg;
Pixel *b_shadow,*t_shadow;
#endif
{
  Colormap colmap;
  Screen   *scr = XDefaultScreenOfDisplay(dpy);
  XColor   bg_color,t_color,b_color;
  int      r,b,g;
  float    h,s,v,th,ts,tv;

  colmap = DefaultColormapOfScreen(scr);
    
  bg_color.pixel = bg;
  XQueryColor(dpy,colmap,&bg_color);

  RGB_TO_HSV(bg_color.red,bg_color.green,bg_color.blue,&h,&s,&v);

/*calculate bottom shadow*/
  if(s == 0.0)
    {
      ts = 0.0;
      th = 400.0; /*undefined*/
      tv = 3*v/4;
    }
  else
    {
      ts = s;
      th = h;
      tv = 3*v/4;
    }

  HSV_TO_RGB(&r,&g,&b,th,ts,tv);
  
  b_color.red = r;
  b_color.green = g;
  b_color.blue = b;

  XAllocColor(dpy,colmap,&b_color);
  *b_shadow = b_color.pixel;

/*calculate top shade*/
  if(s == 0.0)
    {
      ts = 0.0;
      th = 400.0; /*undefined*/
      if (v == 0.0)
	tv = 0.30;
      else{
	if (v*2.0 <= 1.0)
	  tv = v*2.0;
	else
	  tv = 1.0;
      }
    }
  else
    {
      ts = s/4;
      th = h;
      tv = v;
    }

  HSV_TO_RGB(&r,&g,&b,th,ts,tv);

  t_color.red = r;
  t_color.green = g;
  t_color.blue = b;

  XAllocColor(dpy,colmap,&t_color);
  *t_shadow = t_color.pixel;
}

/*****************************************************************************
 *         int XcodaAllocNearestColor (dpy, cmap, color)                     *
 * Description:                                                              *
 *     Allocate a nearest color to a request XColor structure                *
 ****************************************************************************/
#if defined (__STDC__)
int XcodaAllocNearestColor(Display* dpy, 
			   Colormap cmap, 
			   XColor* color)
#else
int XcodaAllocNearestColor(dpy, cmap, color)
Display *dpy;
Colormap cmap;
XColor *color;
#endif
{
  int            i, dc;
  XColor         *ctab;
  int            rd, gd, bd;
  unsigned short ri, gi, bi;
  int            mdist, close, d, status;

  dc = XCellsOfScreen (XDefaultScreenOfDisplay (dpy));
  ctab = (XColor *)malloc (dc*sizeof(XColor));
  if (ctab == NULL){
    fprintf(stderr,"Cannot allocate memory for XColor object\n");
    exit (1);
  }
  for (i = 0; i < dc; i++)
    ctab[i].pixel = (unsigned long)i;

  XQueryColors (dpy, cmap, ctab, dc);

  mdist = 3*dc*dc + 10;
  close = -1;
  ri = (color->red) >> 8;
  gi = (color->green) >> 8;
  bi = (color->blue) >> 8;
  
  for (i = 0; i < dc; i++){
    rd = ri - (ctab[i].red >> 8);
    gd = gi - (ctab[i].green >> 8);
    bd = bi - (ctab[i].blue >> 8);
    
    d = rd*rd + gd*gd + bd*bd;
    if (d < mdist){
      mdist = d;
      close = i;
    }
  }
  if (close < 0){ /* this cannot happen!!! */
    fprintf(stderr,"Bummer!!!!, Fatal Error!!!!\n");
    exit (1);
  }
  if (XAllocColor(dpy, cmap, &ctab[close])){
    color->red = ctab[close].red;
    color->green = ctab[close].green;
    color->blue = ctab[close].blue;
    color->pixel = ctab[close].pixel;
    status = 1;
  }
  else
    status = 0;

  free (ctab);
  return status;
}

/**************************************************************************
 *            Pixel get_pixel (dpy, camp, r, g, b)                        *
 * Description:                                                           *
 *     Get pixel value by specifying a trplet (r, g, b)                   *
 *     Return 0: upon failure                                             *
 *************************************************************************/
#if defined (__STDC__)
Pixel get_pixel(Display* dpy, 
		Colormap cmap, 
		unsigned short r, 
		unsigned short g, 
		unsigned short b)
#else
Pixel get_pixel(dpy, cmap, r, g, b)
Display *dpy;
Colormap cmap;
unsigned short r, g, b;
#endif
{
  XColor color;
  int    status;

  color.red = r;
  color.green = g;
  color.blue = b;
  color.flags = DoRed | DoGreen | DoBlue;

  if(XAllocColor(dpy, cmap, &color))
    return (color.pixel);
  else{
    status = XcodaAllocNearestColor(dpy, cmap, &color);
    if (status){
      return (color.pixel);
    }
    else
      return (0);
  }
}

/************************************************************************
 *     int  XcodaEditorInitColor(Pixel)                                 *
 * Description:                                                         *
 *      Load Color Value from a file                                    *
 *      File Format:                                                    *
 *      background        rgb:rr/gg/bb                                  *
 *      foreground        rgb:rr/gg/bb                                  *
 ***********************************************************************/
#if defined (__STDC__)
int XcodaEditorInitColor (Display* dpy, 
			  Colormap cmap, 
			  Pixel* fg_pixel, 
			  Pixel* bg_pixel)
#else
int XcodaEditorInitColor (dpy, cmap, fg_pixel, bg_pixel)
Display* dpy;
Colormap cmap;
Pixel *fg_pixel, *bg_pixel;
#endif
{
  char *home;
  char fullname[128], temp0[64],temp1[64];
  char line[128];
  FILE *fd;
  extern char *getenv();
  int          r, g, b;
  int          num_lines = 0;
  
  home = getenv ("HOME");
  if (!home)
    return 0;
  strcpy (fullname, home);
  strcat (fullname, "/.netEditor");

  fd = fopen (fullname,"r");
  if (fd == NULL)
    return 0;
  while (!feof(fd)){
    line[0] = '\0';
    fgets (line, sizeof (line), fd);
    if (strstr (line, "background") != 0){
      sscanf (line, "%s %s",temp0,temp1);
      sscanf(temp1,"rgb:%x/%x/%x", &r, &g, &b);
      *bg_pixel = get_pixel (dpy, cmap, (unsigned short)r, 
			     (unsigned short)g, (unsigned short)b);
    }
    else if (strstr (line, "foreground") != 0){
      sscanf (line, "%s %s",temp0,temp1);
      sscanf(temp1,"rgb:%x/%x/%x", &r, &g, &b);  
      *fg_pixel = get_pixel (dpy, cmap, (unsigned short)r, 
			     (unsigned short)g, (unsigned short)b);
    }
  }
  fclose (fd);
  return 1;
}
