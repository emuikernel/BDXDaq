#ifndef __HVPOINTERH__
#define __HVPOINTERH__

void Hv_LaunchMagnify(Hv_Point *);

extern void            Hv_ButtonPress(Hv_Widget,
				      Hv_Pointer,
				      Hv_XEvent *);

extern void            Hv_PointerMotion(Hv_Widget,
					Hv_Pointer,
					Hv_XEvent *);

extern void            Hv_ButtonRelease(Hv_Widget,
					Hv_Pointer,
					Hv_XEvent   *);


#endif

