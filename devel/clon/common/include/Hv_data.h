/*
 *  Hv_data.h is the header containing data structure definitions
 *  
 *  The Hv library was developed at CEBAF under contract to the
 *  Department of Energy and is the property of they U.S. Government.
 *  
 *  It may be used to develop commercial software, but the Hv
 *  source code and/or compiled modules/libraries may not be sold.
 *  
 *  Questions or comments should be directed to heddle@cebaf.gov
 */

#ifndef __HVDATAH__
#define __HVDATAH__

#ifdef Hv_USEOPENGL
#ifndef WIN32
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>
#else
#endif
#endif


/* -------------------------------- TYPEDEFS -------------------------- */


typedef struct hvfpoint {
    float	  h;	/* horizontal coordinate (in world coordinates) */
    float	  v;	/* vertical coordinate (in world coordinates) */
}  Hv_FPoint;


/*----------  RECTANGLES ---------*/

/* an Hv_Rect contains redundant info, but its useful to hold info
   for our canvas so that it needn't be recalced each time we
   transform from world to local coordinates */

typedef struct hvrect {
    short		left;		/* Left pixel coordinate */
    short		top;		/* Top pixel coordinate */
    short		right;		/* Right pixel coordinate */
    short		bottom;		/* Bottom pixel coordinate */
    short	        width;		/* right - left */
    short	        height;		/* bottom - top */
}  Hv_Rect;

/* an Hv_FRect contains redundant info, but its useful to hold info
   for our plot so that it needn't be recalced each time we
   transform from world to local coordinates
   
   IMPORTANT: local pixel cs has ymin at top --- real world cs has ymin at bottom */

typedef struct hvfrect
{
    float	xmin;		/* xmin */
    float	ymin;		/* ymax */
    float	xmax;		/* xmax */
    float	ymax;		/* ymin */
    float	width;		/* xmax - xmin */
    float	height;		/* ymax - ymin */
}  Hv_FRect;

/*-------- TEXT RELATED ---------*/

/* some items have associated text -- here is structure to hold text */

typedef  struct  hvstring
{
    short         font;		/* font */
    short         strcol;         /* string color (the text) */
    short         fillcol;        /* fillcolor */
    short         offset;         /* for "scrolling" */
    char          *text;		/* for items using text */
    Boolean       horizontal;	/* orientation flag */
}  Hv_String;



/*--------- FEEDBACK ---------*/

/* feedback data used for FEEDBACK_ITEMs */

typedef struct hvfeedback *Hv_FeedbackPtr;

typedef struct hvfeedback
{ 
    short            tag;            /* tag for id-ing feedback */
    short            type;           /* 0 for normal text, 1 for separator */
    Hv_String       *label;          /* text string */
    Hv_String       *data;           /* data text string */
    Hv_Rect         *labelrect;      /* rectangles surrounding text RELATIVE TO FEEDBACK_ITEM AREA*/
    Hv_Rect         *datarect;       /* rectangles surrounding data text RELATIVE TO FEEDBACK_ITEM AREA*/
    Hv_FeedbackPtr  next;            /* to make linked list */
} Hv_Feedback;

typedef struct hvcolorscheme
{
    short       fillcolor;           /* Interior area fill color*/
    short       topcolor;            /* Color for left top sides */
    short       bottomcolor;         /* Color for right bottom sides */
    short       lighttrimcolor;      /* Trim color for TopColor */
    short       darktrimcolor;       /* Trim color for BottomColor*/
    short       hotcolor;            /* for fill when selected, etc */
    short       borderwidth;         /* Pixel Width of border*/
    short       framewidth;          /* Pixel Width of Frame (3D effect part)*/
} Hv_ColorScheme;

/* pointers to functions */

typedef void     (*Hv_FunctionPtr)();      /*pointer to function returning a void */
typedef Boolean  (*Hv_CheckProc)();        /*pointer to function returning a Boolean */


/* an attribute might be a short, int, long,  float or pointer */

typedef union {
    double           f;
    char             c;
    short            s;
    int              i;
    long             l;
    void            *v;
    Hv_FunctionPtr   fp;
} Hv_AttributeValue;

typedef Hv_AttributeValue Hv_AttributeArray[Hv_NUMATTRIBUTES];


/*--------- next structure is used in dialogs with timers ------*/

typedef struct hvtimerdlogdata  *Hv_TimerDialogData;

typedef struct hvtimerdlogdata {
    Hv_Widget          dialog;
    Hv_FunctionPtr     callback;
    unsigned long      interval;
    Hv_Pointer         userptr;
    Hv_IntervalId      waitid;
    Hv_Widget          label;
    char              *updatetext;
    short              font;
} Hv_TimerDialogDataRec;


/*********** AUXILLARY POINTERS & LINKED LISTS **********/

typedef struct item		  *Hv_Item;       /*defined below*/
typedef struct view	          *Hv_View;       /*defined below*/

/* ----------- some special item auxillary pointers */


/* for button groups */

typedef struct hvbuttongroupdata
{
    int             nbutton;      /* number of buttons  Hv_NUMBUTTONS*/
    int             orientation;  /* horizontal or vertical Hv_ORIENTATION*/
    Boolean         pad;          /* if true, all buttons have same width Hv_PADBUTTON */
    int             gap;          /* extra gap between buttons Hv_GAP*/
    int             alignment;    /* left or center vertical button groups Hv_ALIGNMENT*/
    short           font;         /* label font Hv_FONT*/
    short           fg;           /* foreground color  Hv_COLOR*/
    short           hotcolor;     /* hot color during tracking Hv_HOTCOLOR*/
    Hv_Rect         boundary;     /* boundary of buttons, not same as items area */
    char          **label;        /* labels Hv_STRINGARRAY*/
    Hv_Rect        *rect;         /* boundary rects*/
    Boolean        *enabled;      /* can that button be selected*/
    int            *state;        /* button state (off or on)*/
    Hv_FunctionPtr *callback;     /* single click callbacks Hv_CALLBACKARRAY*/
    int             hotindex;     /* index [0,1,...] of current "hot" button*/
} Hv_ButtonGroupData;

typedef struct hvbuttongroupcallbackdata
{
    int            pos;           /* index (1,2,..) of the button that was hit */
    char           *label;        /* label of the button that was hit */
} Hv_ButtonGroupCallbackData;

/* Hv_Events take the place of x events */

typedef  struct  hvevent *Hv_Event;

typedef  struct  hvevent

{
    Hv_View         view;          /* view where click occurred */
    Hv_Item         item;          /* item where click occurred (or null) */
    Hv_Point        where;         /* location of click */
    int             modifiers;     /* modifiers */
    int             clicktype;     /* 1 for single, 2 for double */
    void           *callbackdata;  /* e.g. buttongroupcallbackdata */    
} Hv_EventData;


typedef unsigned char byte;


/* info structure filled in by the LoadXXX() image reading routines */

/* for holding pictures */

typedef struct {
  byte *pic;                  /* image data */
  int   w, h;                 /* pic size */
  int   x, y;                 /* cropping offsets */
  int   cw, ch;               /* cropped width and height */
  int   sw, sh;               /* scaleing width and height */
  int   type;                 /* PIC8 or PIC24 */
  byte  r[256],g[256],b[256]; /* colormap for PIC8 */
  int   normw, normh;         /* 'normal size' */
  unsigned long *mycolors;
  Hv_Image      *image;
} Hv_PicInfo;


/* for world images */

typedef struct hvworldimagedata {
    Boolean      frame;
    short        thickness;
    Hv_PicInfo   *pinfo;
    int          user1, user2;

/* these are used for map views  */

    float        latmin, latmax, longmin, longmax;

/* for offscreen storage */

    Hv_Pixmap    background;

} Hv_WorldImageData;
/* for world images */

typedef struct hvworldrectdata
{
    Boolean      frame;
    Boolean      grid;
    short        nrows;
    short        ncols;
    short        fillcolor;
    short        thickness;
    short        style;         /* line style */
} Hv_WorldRectData;

/* world ovals have same datastructure */

typedef  Hv_WorldRectData    Hv_WorldOvalData;

/* for world lines */

typedef struct hvworldlinedata
{
    short        start;  /* [0,1,2,3] for [upperleft, upperright, lowerright, lowerleft]*/
    short        stop;
    short        thickness;
    short        style;
    short        arrowstyle; /* 0 for no arrows */
    short        arrowcolor;
    
/* multiplexing avoids repeated links */
    
    Boolean      multiplexed;
    int          multiplexbits;
    short        multiplexcolors[5];
    
} Hv_WorldLineData;

/* for world polygon */

typedef struct hvworldpolygondata
{
    short        user1;         /* whatever */
    short        thickness;     /* line thickness */
    short        style;         /* line style */
    short        arrowstyle;    /* 0 for no arrows (for open polys) */
    short        fillcolor;     /* fill color NOTE: LINE COLOR IN Item->color*/
    short        symbolcolor;   /* color of vertex symbol */
    short        numpnts;       /* number of points in the polygon */
    short        nrows;         /* for grid overlay */
    short        ncols;         /* for grid overlay */
    short        symbol;        /* symbol for vertices (square, circ, diamond, cross) */
    short        symbolsize;    /* size of vertex symbol*/
    Boolean      grid;          /* draw grid overlay? */
    Boolean      closed;        /* open or clodes polygon */
    Boolean      showpoints;    /* show symbols on vertices */
    Boolean      showlines;     /* draw lines between vertices */
    Hv_FPoint    *fpts;         /* world points */
    Hv_Point     *poly;         /* corresponding pixel points */
    void         *userdata;     /* whatever */
    Boolean      special4pt;    /* special 4 point poly */

    void         *map;          /* for Hv_Map views only */
    Hv_FPoint    basepoint;     /* rotation point */
    float        basepointrad;  /* radius of rotation handle */
} Hv_WorldPolygonData;


/* for world wedge */

typedef struct hvworldwedgedata
{
    short        user1;         /* whatever */
    short        thickness;     /* line thickness */
    short        style;         /* line style */
    short        fillcolor;     /* fill color NOTE: LINE COLOR IN Item->color*/
    Hv_FPoint    basepoint;     /* center point */
    float        azim1;         /* start azimuth */
    float        azim2;         /* end azimuth */
    float        radius;        /* you guessed it*/
    float        innerrad;      /* inner radius */
    int          direction;     /* cw or ccw */
    void         *map;          /* for map views only */
    void         *userdata;     /* whatever */
    int          mode;          /* wedge or ellipse */
    float        xrad;          /* for ellipse */
    float        yrad;          /* for ellipse */

} Hv_WorldWedgeData;


/* for rainbowscales */

typedef struct hvrainbowdata
{
    short            style;               /* line style */
    short            valmin;              /* min allowed value */
    short            valmax;              /* max allowed value */
    short            mincolor;            /* color corresponding to valmin */
    short            maxcolor;            /* color corresponding to valmax */
    short            colorwidth;          /* pixel width of each color strip */
    short            orientation;         /* horizontal (def) or vertical */
    short            numlab;              /* how many value labels */
    short            precision;           /* so that labels can be written as floats */
    short            dx;                  /* if this rainbow is used for a "therma" plot */
    short            dy;                  /* if this rainbow is used for a "therma" plot */
    Boolean          limitededit;         /* if true, edit limit values only */
    float            scalefactor;         /* label valuse can be scaled */
    Hv_Item	     titlestr;		  /* title string stuff */
    short            titlefont;           /* used for title */
    short            labelfont;           /* used for label */
    Hv_Item          labels[Hv_MAXRAINBOWLABEL];  /* the labels */
} Hv_RainbowData;


/* for LEDBars */

typedef struct hvleddata
{
    short  numled;
    short  orientation;
    short  oncolor;
    short  offcolor;
    short  neutralcolor;
    short  textcolor;
    short  font;
    char   **labels;
    short  *states;
} Hv_LEDData;

/* for fractiondone items */

typedef struct hvfractiondonedata
{
    short  topcolor;
    short  bottomcolor;
    short  fillcolor;
    float  fraction;
} Hv_FractionDoneData;

/* for sliders */

typedef struct hvsliderdata
{
    short            valmin;              /* min allowed value */
    short            valmax;              /* max allowed value */
    short            current;             /* current value */
    short            orientation;         /* horizontal (def) or vertical */
    Hv_Item	   valstr;		/* current value string stuff */
    Hv_Item	   titlestr;		/* title string stuff */
    Hv_Item          thumb;               /* sliding part*/
} Hv_SliderData;


/* for wheels */

typedef struct hvwheeldata
{
    short            valmin;              /* min allowed value */
    short            valmax;              /* max allowed value */
    short            current;             /* current value */
    short            orientation;         /* horizontal (def) or vertical */
    Hv_Item	   valstr;		/* current value string stuff */
    Hv_Item	   titlestr;		/* title string stuff */
} Hv_WheelData;

/* for separators */

typedef struct hvseparatordata
{
    short            topcolor;            /* color of top or left line [black] */
    short            bottomcolor;         /* color of bot or right line [white] */
    short            orientation;         /* [horizontal] or vertical */
    short            gap;                 /* when resizing against parent [4] */
} Hv_SeparatorData;

typedef struct hv_colorbuttondata

{
    short     fillcolor;
    Hv_Item   text;
    Boolean   allownocolor;
} Hv_ColorButtonData;

/*-------  SCROLL -------*/

/* here is the structure that contains the relevant scroll info*/

typedef struct hvscrollinfo *Hv_ScrollInfoPtr;

typedef struct hvscrollinfo
{
    Boolean         active;             /* indicates whether the scroll bar is active */
    float           minval;             /* value corresponding to min settine of scroll bar*/
    float           maxval;             /* value corresponding to max setting of scroll bar*/
    float           val;                /* value corresponding to present thumb location */
    short           arrow_step;         /* pixel step when arrow is clicked*/
    short           slide_step;         /* pixel step when slide region is clicked*/
} Hv_ScrollInfo;

/* here is the structure of the listhead "bookkeeeper" for lists of Hv_Items */

typedef struct hvitemlistdata        *Hv_ItemList;

typedef struct hvitemlistdata
{
    Hv_Item	first;		/* first item added to linked list */
    Hv_Item	last;		/* last item added to linked list */
} Hv_ItemListData;

/* here is the structure of the listhead "bookkeeeper" for lists of Views */

typedef struct view_listhead
{
    Hv_View	first;		/* first view added to linked list */
    Hv_View	last;		/* last  view added to linked list */
} View_ListHead;


typedef struct hvinterval
{
    unsigned long    interval;            /* used in timed redraws */
    Hv_IntervalId    intervalid;          /* used in timed redraws */
    unsigned long    timedredrawcount;    /* used in timed redraws */
} Hv_Interval;


/* connections "connect" items with a line */

typedef struct hvconnectiondata  *Hv_ConnectionDataPtr;

/* first part of structure MUST BE SAME AS WORLDLINE */

typedef struct hvconnectiondata
{
    short        start;  /* [0,1,2,3] for [upperleft, upperright, lowerright, lowerleft]*/
    short        stop;
    short        thickness;
    short        style;
    short        arrowstyle; /* 0 for no arrows */
    short        arrowcolor;
    
/* multiplexing avoids repeated links */
    
    Boolean      multiplexed;
    int          multiplexbits;
    short        multiplexcolors[5];
    
    
    Hv_Item      item1, item2;   /* two connected items */
    Boolean      active;
    
    Hv_CheckProc checkproc;  /* user supplied to check for valid connection */
} Hv_ConnectionData;

typedef struct hvconnectionnode *Hv_ConnectionNodePtr;

typedef struct hvconnectionnode
{
    Hv_ConnectionNodePtr  next;    /* next connection for this item */
    Hv_Item               item;    /* actual item (i.e. the line) */
} Hv_ConnectionNode;



/*--------  VIEWS -------*/

/* here is the structure for a view, which is basically a macintosh like
   window on our main Motif window. */

typedef struct view {
  Boolean           enabled;              /* if TRUE, then can "click" items on view*/
  short             tag;                  /* for IDing  this view*/
  short             reserved;             /* not presently used */
  short             minwidth;             /* minimum pixel width  of this view */
  short             minheight;            /* minimum pixel height of this view */
  
  int               modifiers;            /* to remember modifier clicks */
  int		    drawcontrol;	  /* holds drawing options (bitwise) */
  int               userdrawcontrol;      /* another one for user defined options */
  int               popupcontrol;         /* holds which popup items are active */
    
  Hv_FRect  	   *world;	          /* world coordinate system*/
  Hv_FRect         *defaultworld;         /* default world coordinate system */
  Hv_FRect         *lastworld;            /*  so that zooms can be undone */
  Hv_Rect	   *local;	          /* local pixel coordinate system*/
  Hv_Rect          *previouslocal;        /* holds previous local system -- so can toggle with full screen */
    
  Hv_Rect          *magrect;              /* used for pointer magnification */
  Hv_FRect         *magworld;             /* used for pointer magnification */
  Hv_Pixmap         magpixmap;            /* for saving what is under magrect */

  Hv_Rect          *hotrect;              /* holds the "hot" for zooming and scrolling*/
  Hv_Item           containeritem;        /* holds a pointer to the container data */
  Hv_Item           feedbackitem;         /* holds a pointer to the feedback data */
  Hv_Item           hotrectborderitem;    /* holds a pointer to the feedback data */
  Hv_Item           brickwallitem;        /* unmovable, and no object may move in front of it (like a map) */
    
/* auxillary functions */
    
  Hv_FunctionPtr    customize;            /* function for user defined customization via popup menu */
  Hv_FunctionPtr    initialize;           /* setup function for initialization and creating item lists */
  Hv_FunctionPtr    feedback;             /* function called to update feedback */
  Hv_FunctionPtr    userdraw;             /* specialized drawing AFTER item list is drawn */
  Hv_FunctionPtr    destroy;              /* to destroy extra data */
  
  float             minzoomw, minzoomh;   /* cannot zoom to a world w or h smaller than this */
  float             maxzoomw, maxzoomh;   /* cannot zoom to a world w or h bigger  than this */
  Hv_ItemList       items;                /* listhead for the linked list of item's controlled by this view */
  
  Hv_Widget         menu;
  void             *data;                 /*for any purpose*/
  
  Hv_ScrollInfoPtr  hscroll;              /*for horizontal scrolling*/
  Hv_ScrollInfoPtr  vscroll;              /*for vertical scrolling*/
  
  Hv_View           next;                 /*next view in linked list*/
  Hv_View           prev;                 /*previous view in linked list*/
  
/* connection stuff */
  
  Boolean           editconnections;      /*permit individual editing*/
  Boolean           drawconnections;
  Boolean           drawarrowheads;
  Hv_ConnectionNodePtr  connections;      /*list of items connected to each other*/
    
/* simulation stuff */
    
  Hv_FunctionPtr    simproc;              /*function called when sim timer times out*/
  unsigned long     siminterval;          /*interval until simproc is called again*/
  Hv_IntervalId     simintervalid;        /*id of the timer event*/
  void             *mapdata;              /* will point to Hv_ViewMapDataRec if view has maps */
    
  Hv_Pixmap         background;           /* for saving the background if requested */
  
  Hv_FunctionPtr    animationproc;        /* used in conjunction with savebg for anim*/
  
/* related to undo */
    
  Hv_Item          lastdragitem;
  Hv_Item          lastrotateitem;
  
  Hv_Point         lastdragdelta;
  float            lastazimuth;
    
/* off screen user draw */
    
  Hv_FunctionPtr   offscreenuserdraw;
  
  Hv_FRect         *previousworld;       /* added for better restorations from "flip" views */
    
/* new variables related to the new munubutton drawing tools */
    
  short            hotfont;
  short            hotfontfamily, hotfontsize, hotfontstyle;
  short            hotfillcolor;
  short            hotbordercolor;
  short            hotshape;
  short            hotpattern;
  short            hotlinewidth;
  short            hotlinestyle;
  short            hotarrowstyle;
  
/* for motionless feedback */

  Hv_IntervalId     motionlessfbintervalid;
  unsigned long     motionlessfbinterval;

/*for views that are dialogs */

  int                 answer;
  Hv_Item             deleteitem;  /*item scheduled for deletion*/
  Hv_Item             closeout;    /* closeout button group */
  int                 ok, cancel, apply, del; /*positions in BG */
  Hv_TimerDialogData  timerdata;   /*for self destruct dialogs */
  
/*a frame for postential use in a 3D view*/
  
  Hv_Widget           frame3D;
#ifdef Hv_USEOPENGL
  Hv_FunctionPtr      InputRoutine3d;
  Hv_FunctionPtr      DrawRoutine3d;
  Hv_FunctionPtr      CleanUp3d;
  Hv_Pixmap           Pix3D;
  Hv_Pointer          userptr3D;
#ifndef WIN32
  GLXContext          glxContext;
#else
  HGLRC               glxContext;
  HDC                 hdc;
  HWND				  hwnd;
#endif
 
#endif

  int                 memoryhdc;  /* used in Win32 */
}  ViewRecord;


/*-------  ITEMS -------*/

/* Here is the structure for an item.
   The items drawn will be stored as a doubly linked list.
   New items are added at the end. */

typedef struct item
{
    Hv_View          view;                /* view owner of this item */
    short	     type;	          /* i.e., rectangle, coil, line, etc */
    short            tag;                 /* user sets this tag to quickly find the item */
    short            color;               /* use this for simple one color items */
    short            state;               /* for buttons, especially*/
    short            resizepolicy;        /* how a resize affects this item */
    short            domain;              /* where this items lives (hotrect or not hotrect */
    
    int		     drawcontrol;	  /* holds drawing options (bitwise) */
    int              user1;               /* for any purpose*/
    int              user2;               /* for any purpose*/
    void *           userptr;             /* for any purpose*/  
    
    float            azimuth;             /* relevant for rotatable items */
    
    
    Hv_Interval     *drawinterval;        /*used in timed redraw */
    Hv_Rect	    *area;	        /* pixel coordinates of item's bounding rect */
    Hv_FRect        *worldarea;           /* world rect corresponding to area for HOTRECT items */
    Hv_Rect         *rotaterect;          /* used to grab item for rotation*/
    Hv_Point        *rotaterp;            /* rotate ref point */

    Hv_Region        region;              /* used for complicated items -- often NULL */
    Hv_String	    *str;		        /* pointer to string stuff, if needed */
    void	    *data;	        /* generic pointer for private item specific data*/
    char            *balloon;             /* points to balloon help text */
    Hv_Item	     next;		/* for making linked list */
    Hv_Item	     prev;		/* for making linked list */
    Hv_ItemList	     children;	        /* an internal list of items contained in this one! */
    
    Hv_FunctionPtr   standarddraw;        /* standard drawing routine */
    Hv_FunctionPtr   singleclick;         /* single click callback */
    Hv_FunctionPtr   doubleclick;         /* double click callback */
    Hv_FunctionPtr   userdraw;            /* specialized drawing */
    Hv_FunctionPtr   initialize;          /* specialized initialization */
    Hv_FunctionPtr   customize;           /* specialized customization */
    Hv_FunctionPtr   destroy;             /* specialized destruction */
    Hv_FunctionPtr   afterdrag;           /* called after a drag */
    Hv_FunctionPtr   afteroffset;         /* called after an offset */
    Hv_FunctionPtr   afterrotate;         /* called after a rotate */

    Hv_FunctionPtr   fixregion;           /* called to fix region */
    Hv_FunctionPtr   checkenlarge;        /*check for item enlargement*/
    
    Hv_ColorScheme   *scheme;             /* 3D color scheme for itemss that need it*/
    Hv_Item          parent;              /* if this is part of a sublist, this may be useful*/
    
    short            pattern;             /* background pattern */
    void *           userptr2;            /* for any purpose*/  

/* an extra FPoint that can be used for precise locking,
   especially useful for storing a latitude and longitude
   for map views */

    Hv_FPoint       *base;
    
}  Hv_ItemData;



/* data structure used by Hv_MENUBUTONITEMs */

typedef struct menubuttonitem  *Hv_MenuButtonData;

typedef struct menubuttonitem {
    
/* the current field is either the current item
   1..N for a text based menu or 256*row plus
   col for an icon based, where row and col
   are 1.. and represent the current choice. */
    
    short           current;                /* current choice */
    short           style;                  /* text or icon based */
    short           armcolor;               /* background of entry display */
    short           fcolor;                 /* text and icon background color */
    short           ccolor;                 /* background color of current choice*/
    Hv_Rect         display;                /* the rect that will hold the choices */

/* for a text based, numcol = 1 and numrow is
   the number of choices. For icon based, the
   icons are arranged in an numrowxnumcol
   array */

    short           numrow, numcol;      
    
/* for icon based, the size of the icons is needed */
    
    short           iconwidth, iconheight;
    
/* the ChoiceCB MUST be provided by the user.
   Its protocol is ChoiceCB(Hv_Item Item, short choice)
   where choice is composite for icon menus as described
   above */
    
    
    Hv_FunctionPtr  ChoiceCB;               /*called when menu selected */
    
/* AlternateCB called when user selects "button" part of
   icon button */
    
    Hv_FunctionPtr  AlternateCB;
    
/* the icondrawer is need, obviously, only for iconbased */
    
    Hv_FunctionPtr  IconDraw;
    
/* the following are used only for text menus */
    
    Hv_String       *title;                 /*title for text based*/
    Hv_String       **entries;              /*labels for text based*/
    short           *estyles;               /*style of each entry (future use)*/
    
    
/* some buttons, for example shoosing the default
   background, do not need an active area since
   both the active area and the drawing area
   would have the same callback */
    
    Boolean         hasactivearea;
    
/* if tandem is true, the Altername callback
   will be called after the MenuCB */
    
    Boolean         tandem;
    
/* generic user data */
    
    void            *data;                  /*user data*/
} Hv_MenuButtonDataRec;


/* ballondata used for Hv's single balloon, which is shared among all items
   using a baloon (since only one can be displayed at a time) */

typedef struct balloondata
{
    Hv_Point   pp;         /* point from where balloon emerges */         
    int        numline;    /* number of lines of balloon text */
    Hv_Region  region;     /* balloons region */
    char       *text;      /* balloon text (never malloced, just redirected to an items balloon field */
} BalloonData;


/* keep a master linked list of dialogs */

typedef struct hvdlogdata  *Hv_DialogData;

typedef struct hvdlogdata 
{
    Hv_DialogData       next;
    Hv_Widget           dialog;
    int                 answer;
    void               *data;
    Boolean             opened;
    Hv_Widget           ok, cancel, apply, del;
    int                 user1, user2;
    Hv_TimerDialogData  timerdata;
    Hv_Item             deleteitem;  /*item scheduled for deletion*/
} Hv_DialogDataRec;

/* used for sets of radio buttons in dialogs
   NOTHING TO DO WITH THE Hv_CHOICEITEM !! */

typedef struct hvradiolist  *Hv_RadioListPtr;

typedef struct hvradiolist
{
    Hv_Widget            radio;
    Hv_RadioListPtr      next;
    void                *data;
    short                pos;
} Hv_RadioList;

typedef struct  hvradiohead  *Hv_RadioHeadPtr;

typedef struct  hvradiohead
{
    Hv_Widget        activewidget;
    short            activepos; /* not a C INDEX ! */
    Hv_RadioListPtr  head;
} Hv_RadioHead;

/* used for option buttons */

typedef struct hvoptionbuttonrec  *Hv_OptionButtonPtr;

typedef struct hvoptionbuttonrec
{
    short     armcolor;
    Boolean   *control;
} Hv_OptionButtonRec;


/* this one is used for the option button column 
   convenience creation */

typedef struct hvoptionbuttondata  *Hv_OptionButtonDataPtr;

typedef struct hvoptionbuttondata
{
  char             *label;
  Hv_FunctionPtr    callback;
  Boolean          *control;
  Boolean          enabled;
  Boolean          redraw;
  Hv_Item          *item;
} Hv_OptionButtonData;

#endif
