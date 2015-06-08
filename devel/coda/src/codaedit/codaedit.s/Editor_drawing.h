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
 *	CODA Editor drawing routines header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_drawing.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:04  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_DRAWING_H
#define _EDITOR_DRAWING_H

#if defined (__STDC__)
extern void draw_original(drawComp* draw_comp, 
			  Display* dpy, 
			  Drawable win, 
			  int x, int y, 
			  Dimension wd, 
			  Dimension ht, 
			  int r, int c);
extern void draw_rubber_comp(drawComp* draw_comp, 
			     Display* dpy, 
			     Drawable win, 
			     int x, int y, 
			     Dimension wd, 
			     Dimension ht);
extern void draw_high_light(drawComp* p, 
			    Display* dpy, 
			    Drawable win);
extern void draw_unhigh_light (drawComp* p,
			       Display* dpy,
			       Drawable win);
extern void erase_comp(drawComp* p, 
		       Display* dpy, 
		       Drawable win);
extern void draw_comp_name(drawComp* comp, 
			   Display *dpy,
			   Drawable win);
extern void erase_comp_name(drawComp* comp, 
			    Display *dpy, 
			    Drawable win);
extern void move_comp_name(drawComp* comp, 
			   Display *dpy, 
			   Drawable win, 
			   int x, int y,
			   Dimension wd, 
			   Dimension ht);
extern void draw_host_name(drawComp* comp, 
			   Display* dpy, 
			   Drawable win);
extern void erase_host_name(drawComp* comp, 
			    Display* dpy, 
			    Drawable win);
extern void move_host_name(drawComp* comp, 
			   Display* dpy, 
			   Drawable win, 
			   int x, int y, 
			   Dimension wd, 
			   Dimension ht);
extern void arm_ip_port (ipPort* port, 
			 Display* dpy, 
			 Drawable win);
extern void high_light_input_port(ipPort* port, 
				  Display* dpy, 
				  Drawable win);
extern void high_light_output_port(ipPort* port, 
				   Display* dpy, 
				   Drawable win);
extern void disarm_ip_port(ipPort* port, 
			   Display* dpy, 
			   Drawable win);
extern void draw_port_name(ipPort* port, 
			   Display* dpy, 
			   Drawable win);
extern void erase_port_name(ipPort* port, 
			    Display* dpy, 
			    Drawable win);
extern void draw_rubber_name(ipPort* port, 
			     Display* dpy, 
			     Drawable win, 
			     int x, int y, 
			     Dimension wd, 
			     Dimension ht);
extern void draw_original_arc(Arc* arc, 
			      Display* dpy, 
			      Drawable win);
extern void draw_rubber_arc(Arc* arc, 
			    Display* dpy, 
			    Drawable win,
			    int x, int y);
extern void draw_high_light_arc(Arc* arc, 
				Display* dpy, 
				Drawable win);
extern void erase_arc (Arc* arc, Display* dpy, Drawable win);
#else
extern void draw_original ();
extern void draw_rubber_comp ();
extern void draw_high_light ();
extern void draw_unhigh_light ();
extern void erase_comp ();
extern void draw_comp_name ();
extern void erase_comp_name ();
extern void move_comp_name ();
extern void draw_host_name ();
extern void erase_host_name ();
extern void move_host_name ();
extern void arm_ip_port ();
extern void high_light_input_port ();
extern void high_light_output_port ();
extern void disarm_ip_port ();
extern void draw_port_name ();
extern void erase_port_name ();
extern void draw_rubber_name ();
extern void draw_original_arc ();
extern void draw_rubber_arc ();
extern void draw_high_light_arc ();
extern void erase_arc ();
#endif

#endif











