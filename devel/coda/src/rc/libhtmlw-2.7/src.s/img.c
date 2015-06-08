/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

/* Sergey */
#ifdef Linux
#undef X_WCHAR
#define __UNIXOS2__
#endif

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "picread.h"
#include "HTML.h"

ImageInfo *scaleImage(ImageInfo *img_data,char *width,char *height);

/* for selective image loading */
#define blank_width 8
#define blank_height 8
static unsigned char blank_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
static ImageInfo *blank = NULL;

/*******************************/


/* Defined in gui.c */
extern char *cached_url;
extern int srcTrace;
/*
          (dsp, DefaultRootWindow (dsp), \
*/

#if defined(__STDC__) || defined(__sgi)
#define MAKE_IMGINFO_FROM_BITMAP(name) \
  if (!name) \
    { \
      name = (ImageInfo *)malloc (sizeof (ImageInfo)); \
      name->ismap = 0; \
      name->width = name##_width; \
      name->height = name##_height; \
      name->image_data = NULL; \
      /* Bandaid for bug in Eric's code. */ \
      name->internal = 1; \
      name->transparent = 0; \
      name->image = \
        XCreatePixmapFromBitmapData \
          (XtDisplay(swin), XtWindow(swin), \
           name##_bits,  \
           name##_width, name##_height, \
           fg_pixel, bg_pixel, DefaultDepth(dsp, DefaultScreen(dsp))); \
    } \
  if (!name->image) \
    { \
      name->image = \
        XCreatePixmapFromBitmapData \
          (XtDisplay(swin), XtWindow(swin), \
           name##_bits,  \
           name##_width, name##_height, \
           fg_pixel, bg_pixel, DefaultDepth(dsp, DefaultScreen(dsp))); \
    }
#else
#define MAKE_IMGINFO_FROM_BITMAP(name) \
  if (!name) \
    { \
      name = (ImageInfo *)malloc (sizeof (ImageInfo)); \
      name->ismap = 0; \
      name->width = name/**/_width; \
      name->height = name/**/_height; \
      name->image_data = NULL; \
      /* Bandaid for bug in Eric's code. */ \
      name->internal = 1; \
      name->transparent = 0; \
      name->image = \
        XCreatePixmapFromBitmapData \
          (XtDisplay(swin), XtWindow(swin), \
           name/**/_bits,  \
           name/**/_width, name/**/_height, \
           fg_pixel, bg_pixel, DefaultDepth(dsp, DefaultScreen(dsp))); \
    } \
  if (!name->image) \
    { \
      name->image = \
        XCreatePixmapFromBitmapData \
          (XtDisplay(swin), XtWindow(swin), \
           name/**/_bits,  \
           name/**/_width, name/**/_height, \
           fg_pixel, bg_pixel, DefaultDepth(dsp, DefaultScreen(dsp))); \
    }
#endif /* __STDC__ */

#define RETURN_IMGINFO_FROM_BITMAP(x) \
{ \
  MAKE_IMGINFO_FROM_BITMAP(x); \
  if (cci_event) MoCCISendEventOutput(IMAGE_LOADED); \
  return x; \
}

/* ------------------------------------------------------------------------ */


ImageInfo *scaleImage(ImageInfo *img_data,char *width,char *height) {
/*
int widthInt,heightInt;

        if (!width) {
                widthInt=img_data->width;
	}
        else {
                if (strchr(width,'%')) {
                        widthInt=(atoi(width)/100)*img_data->width;
		}
                else {
                        widthInt=atoi(width);
		}
	}

        if (!height) {
                heightInt=img_data->height;
	}
        else {
                heightInt=atoi(height);
	}

        for (i=0; i<256; i++) {
                convertRed[i]=(byte)img_data->reds[i];
                convertGreen[i]=(byte)img_data->greens[i];
                convertBlue[i]=(byte)img_data->blues[i];
	}

        img_data->image_data=SmoothResize(img_data->image_data,0,
                                          img_data->width,img_data->height,
                                          widthInt,heightInt,
                                          convertRed,convertGreen,convertBlue,
                                          newRed,newGreen,newBlue,256);
*/

	return NULL;
}

/* ------------------------------------------------------------------------ */

unsigned char nums[]={
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128
};

/* Cache Load Hack: (BJS)
   char *ir_hack_string, *diskfile, *url;
   sprintf(ir_hack_string,"%s\0%s",diskfile,url);
   ImageResolve(NULL,ir_hack_string,0,NULL,NULL);
   */

/* Width and Height Hack: (SWP)
	Added "width" and "height" to the parameter list. These are used to
	resize the image when we are done decoding. This'll be fun when we
	start processing on a line-by-line basis.
	If NULLs are passed, the width and height are not used.
	*/

static char *my_strrchr(char *a,char b)
{
  char *p;

  p=a+strlen(a);
  
  for (;p != a;p--) 
    {
      if (*p == b) goto found;
    }
  return NULL;
found:
  return p;
}

/* Image resolution function. */
ImageInfo *ImageResolve (Widget w, char *src, int noload, char *wid, char *hei)
{
    Widget swin = w;
    int i, cnt;
    unsigned char *data;
    unsigned char *bg_map;
    unsigned char *bgptr;
    unsigned char *cptr;
    unsigned char *ptr;
    int width, height;
    int Used[256];
    XColor colrs[256];
    ImageInfo *img_data;
/*  char *txt;*/
    int widthbyheight = 0;
    int rc;
    int bg, bg_red, bg_green, bg_blue;
#ifdef HAVE_HDF
    int ishdf = 0;
    char *hdfref = NULL;
#endif

    extern int Vclass;
    static Pixel fg_pixel, bg_pixel;
    static int done_fetch_colors = 0;
    int j,bcnt;

    static int once = 0;
    extern char HTMLdirCache[500];
    static char currFile[500];
    char fileName[255];
    extern char *srtdup();

#ifndef DISABLE_TRACE
    if (srcTrace)
        fprintf (stderr,
                 "[ImageResolve] I'm loading '%s'\n",
                 src);
#endif
    /* Either there is a complete path in ifileName OR ifileName is
       relative to cwd OR ifileName is relative to the last full 
       path we were given.
       */
    if (src[0] != '\0') {
      if (src[0] != '/') {
	if (strncmp(src,"http:/",6) == 0) {
	  if (src[7] == '.')
	    sprintf(fileName,"%s/%s",HTMLdirCache,&src[6]);
	  else 
	    sprintf(fileName,"%s/%s",getenv("HTML_ROOT"),&src[6]);

	} else {
	  sprintf(fileName,"%s/%s",HTMLdirCache,src);
	}
      } else {
	sprintf(fileName,"%s",src);
      }
    }
#ifndef DISABLE_TRACE
    if (srcTrace)
        fprintf (stderr,
                 "[ImageResolve] Demangled path is '%s', noload %d!\n",
                 fileName, noload);
#endif
    
    if (!fileName)
        return NULL;

foundit:
#ifndef DISABLE_TRACE
    if (srcTrace)
        fprintf (stderr, "[ImageResolve] hello!\n");
#endif

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "GOING ON THROUGH...\n");
#endif

  /*
   * No transparent background by default
   */
  bg = -1;
  bg_map = NULL;
  {
    data = ReadBitmap(w,fileName, &width, &height, colrs, &bg);
    
#ifndef DISABLE_TRACE
    if (srcTrace)
      fprintf (stderr,
	       "[ImageResolve] Did ReadBitmap, got 0x%08x\n", data);
#endif
    
    /* if we have a transparent background, prepare for it */
    if ((bg >= 0)&&(data != NULL))
      {
	unsigned long bg_pixel;
	XColor tmpcolr;
	
	/* This code copied from xpmread.c.  I could almost
	 * delete the code from there, but I suppose an XPM
	 * file could pathalogially have multiple transparent
	 * colour indicies. -- GWP
	 */
	XtVaGetValues(swin, XtNbackground, &bg_pixel, NULL);
	tmpcolr.pixel = bg_pixel;
	XQueryColor(XtDisplay(swin),
		    DefaultColormap(XtDisplay(swin), DefaultScreen(XtDisplay(swin))),
		    &tmpcolr);
	
	bg_red = colrs[bg].red = tmpcolr.red;
	bg_green = colrs[bg].green = tmpcolr.green;
	bg_blue = colrs[bg].blue = tmpcolr.blue;
	colrs[bg].flags = DoRed|DoGreen|DoBlue;
	
	bg_map = (unsigned char *)malloc(width * height);
      }
      
	if (data == NULL)
	{
#ifndef DISABLE_TRACE
		if (srcTrace)
			fprintf (stderr, "[ImageResolve] data == NULL; punting...\n");
#endif

		return NULL;
	}

{
int found_bg=0;

	if (data!=NULL) {
		for (i=0; i<width*height; i++) {
			if ((int)(data[i])==bg) {
				found_bg=1;
				break;
			}
		}
		if (!found_bg) {
			bg=(-1);
		}
	}
}

      img_data = (ImageInfo *)malloc(sizeof(ImageInfo));

      img_data->width = width;
      img_data->height = height;
      img_data->image_data = data;
      img_data->image = 0;
      img_data->clip = 0;
      img_data->src = strdup(fileName);
      /* Bandaid for bug afflicting Eric's code, apparently. */
      img_data->internal = 0;
  }

	widthbyheight = img_data->width * img_data->height;

	/* Fill out used array. */
	for (i=0; i < 256; i++) {
		Used[i] = 0;
	}

	cnt = 1;
	bgptr = bg_map;
	cptr = img_data->clip_data;
	ptr = img_data->image_data;

	/*This sets the bg map and also creates bitmap data for the
		clip mask when there is a bg image */
	for (i=0; i<img_data->height; i++) {
		for (j=0,bcnt=0; j<img_data->width; j++) {
			if (Used[(int)*ptr] == 0) {
				Used[(int)*ptr] = cnt;
				cnt++;
			}
			if (bg>=0) {
				if (*ptr == bg) {
					*bgptr = 1;
				}
				else {
					*bgptr = 0;
					/*if (img_data->transparent) {
						*cptr += nums[(bcnt % 8)];
					}*/
				}
				/*if (img_data->transparent &&
				    ((bcnt % 8)==7 ||
				     j==(img_data->width-1))) {
					cptr++;
				}*/
				bgptr++;
				bcnt++;
			}
			ptr++;
		}
	}
	cnt--;

  img_data->num_colors = cnt;

#ifdef HAVE_HDF
  if (!ishdf)
#endif
    {
      img_data->reds = (int *)malloc(sizeof(int) * cnt);
      img_data->greens = (int *)malloc(sizeof(int) * cnt);
      img_data->blues = (int *)malloc(sizeof(int) * cnt);
    }

  /* bg is not set in here if it gets munged by MedCut */
  for (i=0; i < 256; i++)
    {
      int indx;
      
      if (Used[i] != 0)
        {
          indx = Used[i] - 1;
          img_data->reds[indx] = colrs[i].red;
          img_data->greens[indx] = colrs[i].green;
          img_data->blues[indx] = colrs[i].blue;
	  /* squeegee in the background color */
	  if ((bg >= 0)&&(i == bg))
            {
              img_data->reds[indx] = bg_red;
              img_data->greens[indx] = bg_green;
              img_data->blues[indx] = bg_blue;
	      img_data->bg_index=indx;
            }
        }
    }

    /* if MedianCut ate our background, add the new one now. */
    if (bg == 256)
      {
	img_data->reds[cnt - 1] = bg_red;
	img_data->greens[cnt - 1] = bg_green;
	img_data->blues[cnt - 1] = bg_blue;
	img_data->bg_index=(cnt-1);
      }
  
  bgptr = bg_map;
  cptr = img_data->clip_data;
  ptr = img_data->image_data;
  for (i=0; i < widthbyheight; i++)
    {
      *ptr = (unsigned char)(Used[(int)*ptr] - 1);
      /* if MedianCut ate the background, enforce it here */
      if (bg == 256)
        {
          if (*bgptr)
            {
              *ptr = (unsigned char)(cnt - 1);
            }
          bgptr++;
        }
      ptr++;
    }

  /* free the background map if we have one */
  if (bg_map != NULL)
  {
    free (bg_map);
  }

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "[ImageResolve] Doing mo_cache_data on '%s', 0x%08x\n",
             fileName, img_data);
#endif

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "[ImageResolve] Leaving...\n");
#endif



  return img_data;
/*
  if (!wid && !hei) {
	return img_data;
  }

  return(scaleImage(img_data,wid,hei));
*/
}

ImageInfo *DelayedImageResolve (Widget w, char *src)
{
  ImageInfo *img;

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "[DelayedImageResolve] I'm entering, src '%s'\n", src);
#endif

  img = ImageResolve (w, src, 0, NULL, NULL);

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "[DelayedImageResolve] Returning 0x%08x\n", img);
#endif

  return img;
}

int mo_free_image_data (void *ptr)
{
  ImageInfo *img = (ImageInfo *)ptr;
  Widget swin;/*GHGH what to set this to!*/

#ifndef DISABLE_TRACE
  if (srcTrace)
    fprintf (stderr, "[mo_free_image_info] Freeing 0x%08x\n", img);
#endif

  if (!img)
    return -1;/* fail */

  if (img->reds)
    {
      free (img->reds);
      img->reds = NULL;
    }
  if (img->greens)
    {
      free (img->greens);
      img->greens = NULL;
    }
  if (img->blues)
    {
      free (img->blues);
      img->blues = NULL;
    }
  if (img->image_data)
    {
      free (img->image_data);
      img->image_data = NULL;
    }
  if (img->clip_data) {
      free (img->clip_data);
      img->clip_data = NULL;
  }
  if (img->src) {
	free(img->src);
	img->src=NULL;
  }
  if (img->image!=None) {
	XFreePixmap(XtDisplay(swin),img->image);
	img->image=None;
  }
  if (img->clip!=None) {
	XFreePixmap(XtDisplay(swin),img->clip);
	img->clip=None;
  }

  return 0;/* ghgh good */
}
/*
int mo_register_image_resolution_function (mo_window *win)
{
  XmxSetArg (WbNresolveImageFunction, (long)ImageResolve);
  XmxSetArg (WbNresolveDelayedImage, (long)DelayedImageResolve);
  XmxSetValues (win->scrolled_win);
  return mo_succeed;
}
*/
