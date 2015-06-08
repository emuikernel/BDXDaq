#ifndef __HVDRAGH__
#define __HVDRAGH__

/*
 * various drag policies (bits)
 */

#define Hv_RECTCONFINED     01    /* confine drag rect (instead of just mouse) */ 
#define Hv_NODRAGPOLICY     02    /* usual drag anywhere policy */
#define Hv_NOSELECTPOLICY   Hv_NODRAGPOLICY /* usual arbitrary rubber band policy */
#define Hv_ROTATEPOLICY     04    /* tricks DragRect into rotating */
#define Hv_FIXEDXPOLICY     010   /* Ignore x motion (e.g vertical scroll thumb) */
#define Hv_FIXEDYPOLICY     020   /* Ignore y motion (e.g horizont scroll thumb) */
#define Hv_SHIFTPOLICY      040   /* Shift was pressed when drag started */
#define Hv_SQUAREPOLICY     0100  /* Actually fixed aspect ration policy */
#define Hv_FIXEDASPECTPOLICY      Hv_SQUAREPOLICY
#define Hv_OVALMETHOD       0200  /* Apply oval vs. rectangle methods */

/*
 * data structure that holds info about a drag,
 * so that the callback can be called back with
 * sensible data.
 */

typedef struct hvdragdata  *Hv_DragData;

typedef struct hvdragdata {
  Hv_View         view;       /* view being dragged or dragged on. */
  Hv_Item         item;       /* item being dragged (or NULL) */
  Hv_Point        startpp;    /* Mouse point where drag initiated. */
  Hv_Point        finalpp;    /* Mouse point at release */
  Hv_Rect         startrect;  /* Initial rectangle */
  Hv_Rect         limitrect;  /* Confining rectangle */
  Hv_Rect         finalrect;  /* Final rectangle */
  int             policy;     /* one of the *CONFINED constants */
  Hv_FunctionPtr  callback;   /* callback function */
  Hv_Region       itemregion; /* original item region */
  char            oldstr[80]; /* for showing update while dragging */ 
  char            newstr[80]; /* for showing update while dragging */ 
  Hv_Rect         strrect;    /* for showing update while dragging */ 
  int             vertex;     /* for polygon use */
  float           asprat;     /* for fixed aspect ratio selections */
  Hv_FRect       *tempworld;  /* hold a view's world system */
  Hv_Point       *poly;       /* for selecting polygons */
  short           numpts;     /* also for polygons */
} Hv_DragDataRec;


extern void Hv_LaunchDragRect(Hv_View,
			      Hv_Item,
			      Hv_Point *,
			      Hv_Rect  *,
			      Hv_Rect  *,
			      int,
			      Hv_FunctionPtr);

extern void            Hv_LaunchSelectPolygon(Hv_View,
					      Hv_Item,
					      Hv_Rect *,
					      Hv_Point *,
					      Hv_FunctionPtr);

extern void            Hv_LaunchSelectRect(Hv_View,
					   Hv_Item,
					   Hv_Rect *,
					   Hv_Point *,
					   short,
					   float,
					   Hv_FunctionPtr);

extern void            Hv_LaunchSelectOval(Hv_View,
					   Hv_Item,
					   Hv_Rect *,
					   Hv_Point *,
					   short,
					   Hv_FunctionPtr);

extern void Hv_FreeDragData(Hv_DragData);

extern Hv_DragData Hv_MallocDragData(Hv_View,
				     Hv_Item,
				     Hv_Point *,
				     Hv_Rect *,
				     Hv_Rect *,
				     int,
				     int,
				     Hv_FunctionPtr);


extern void            Hv_ConnectLine(Hv_Rect,
				      Hv_Point,
				      Hv_Point *);

extern void            Hv_CreateLine(Hv_Rect   LimitRect,
				     Hv_Point  StartPP,
				     Hv_Point *StopPP);

extern void            Hv_EnlargeLine(Hv_Rect,
				      Hv_Point,
				      Hv_Point *,
				      short,
				      Hv_Point *);

extern void            Hv_EnlargeRect(Hv_Rect *,
				      Hv_Rect,
				      Hv_Point,
				      Boolean);

extern void            Hv_EnlargeOval(Hv_Rect *,
				      Hv_Rect,
				      Hv_Point,
				      Boolean);


extern void            Hv_SelectWedge(Hv_View View,
				      Hv_Rect    LimitRect,
				      Hv_Point   StartPP,
				      Hv_FPoint  *basepoint,
				      float      *azim1,
				      float      *azim2,
				      float      *radius,
				      int        *direction);

extern void            Hv_LaunchMagnify();

#endif
