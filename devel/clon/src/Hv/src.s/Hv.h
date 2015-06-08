#ifndef __HVH__
#define __HVH__


/**
 * <EM>Main header file for the Hv Library.</EM>
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
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a>  <HR>
 */


#ifdef WIN32
#define  STRICT
#include <windows.h>
#include <direct.h>

#endif

/*  Standard C headers  */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>




/* X & Motif Includes */

#include "Xm/Xm.h"

#include "Xm/BulletinB.h"
#include "Xm/CascadeB.h"
#include "Xm/CascadeBG.h"
#include "Xm/DrawingA.h"
#include "Xm/Frame.h"
#include "Xm/Form.h"
#include "Xm/Label.h"
#include "Xm/LabelG.h"
#include "Xm/List.h"
#include "Xm/MessageB.h"
#include "Xm/PushB.h"
#include "Xm/PushBG.h"
#include "Xm/RowColumn.h"
#include "Xm/Scale.h"
#include "Xm/SeparatoG.h"
#include "Xm/Separator.h"
#include "Xm/Text.h"
#include "Xm/TextF.h"
#include "Xm/ToggleB.h"
#include "Xm/ToggleBG.h"
#include "Xm/DialogS.h"

#ifndef _AIX
#ifndef sparc
#if XtSpecificationRelease > 4
#include "Xm/RepType.h"
#endif
#endif
#endif
#endif


#include <stdarg.h>  /* for variable length argument lists */


#ifdef __sgi
#include <malloc.h>
#endif




/* Hv defines */


/* points in/out of rectangles */

#define Hv_RECTANGLEOUT         0
#define Hv_RECTANGLEIN          1


/* shapes for the drawing tools */

#define Hv_LINESHAPE             1
#define Hv_RECTSHAPE             2
#define Hv_OVALSHAPE             3
#define Hv_OPENPOLYSHAPE         4
#define Hv_CLOSEDPOLYSHAPE       5

/* directions */

#define Hv_CLOCKWISE             0
#define Hv_COUNTERCLOCKWISE      1

/* distance units for maps */

#define Hv_KILOMETERS            1
#define Hv_NAUTICALMILES         2
#define Hv_MILES                 3

/* arrows */

#define Hv_NOARROW               0
#define Hv_ARROWATEND            1
#define Hv_ARROWATSTART          2
#define Hv_ARROWATBOTH           3

/* Hv_MENUBUTTONITEM styles */

#define Hv_TEXTBASED             1
#define Hv_ICONBASED             2

/* text based Hv_MENUBUTTONITEM behaviors */

#define Hv_OPTIONBEHAVIOR        1    /*choice one of N*/
#define Hv_GENERALBEHAVIOR       2    /*regular pulldown */


/* # points in world wedge drawing */

#define  Hv_NUMWEDGEPOINTS   260
#define  Hv_NUMWEDGESTRAIGHT 80    /*fraction of above used for legs */
#define  Hv_WEDGEMODE        1
#define  Hv_ELLIPSEMODE      2

/* scroll policies */

#define Hv_SCROLLNONE        1      
#define Hv_SCROLLHORIZONTAL  2    
#define Hv_SCROLLVERTICAL    3  
#define Hv_SCROLLBOTH        4

/* WorldLine drawing policies */

#define Hv_NORESTRICTION        1
#define Hv_ONEPOINTINHOTRECT    2
#define Hv_BOTHPOINTSINHOTRECT  3 

#define Hv_LIMITEDEDIT          1001    /* allows some limited item editing (eg rainbows)*/
#define Hv_WELCOMEVIEW         -1001    /* welcome view */
#define Hv_XYPLOTVIEW          -1002    /* xyplot */
#define Hv_VIRTUALVIEW         -1003    /* virtual desktop selector*/
#define Hv_DIALOGVIEW          -1004    /* views that are dialogs */ 

#define Hv_MAXRAINBOWLABEL      8       /* max labels on a rainbow scale */
#define Hv_COLORS              176      /* number of colors Hv will try to allocate (standard)*/
#define Hv_MAXBALLOONSTR        15      /* max number of lines in a baloon help message */
#define Hv_MAXBALLOONSTRLEN     35      /* max number of chars on each line */

#define Hv_MAXCHOICE            60      /* max choice buttons in a single set*/
#define Hv_MAXRADIOBUTTONS      Hv_MAXCHOICE /*backward compatibility*/


#define Hv_MAXAUTOFBENTRIES     Hv_MAXCHOICE   /* max num feedback entries via attributes */
#define Hv_WELCOMERAINBOWTAG   -2001    /* tag of rainbow in welcome view */
#define Hv_VERSIONSTRINGTAG    -2002    /* tag of version string */

#define Hv_MINVIEWMOVE         4        /* Minimum view move */
#define Hv_MINTHUMBWIDTH       30       /* minimum width and height of a slider thumb */

#define Hv_MAX_TEXTSIZE        500000   /* max text size in a scrolled text widget */

/* special tag for separators */

#define Hv_SEPARATORTAG       -32766    /* special tag for feedback separators */

/* special tag for menubuttoitem text */

#define Hv_MENUBUTTONCHOICE   1

/* Hv_Event Modifiers */

#define	Hv_SHIFT	        01	/* Bit 0  -> Shift was pressed */
#define Hv_CONTROL              02      /* Bit 1  -> Control was pressed */
#define Hv_MOD1                 04      /* Bit 2  -> Mod1 (Alt) was pressed" */

/* Event Masks */

#define Hv_BUTTONPRESSMASK      01
#define Hv_BUTTONRELEASEMASK    02
#define Hv_BUTTONMOTIONMASK     04
#define Hv_BUTTON1MOTIONMASK   010

/* feedback types */

#define Hv_FEEDBACKTEXT         0
#define Hv_FEEDBACKSEPARATOR    1

/* symbols */

#define Hv_NOSYMBOL             0
#define Hv_RECTSYMBOL           1
#define Hv_CIRCLESYMBOL         2
#define Hv_DIAMONDSYMBOL        3
#define Hv_CROSSSYMBOL          4
#define Hv_TRIANGLEUPSYMBOL     5
#define Hv_TRIANGLEDOWNSYMBOL   6
#define Hv_BLOCKISYMBOL         7

#define Hv_POINTSYMBOL          8
#define Hv_BIGPOINTSYMBOL       9
#define Hv_NOSYMBOLS            10

#define Hv_BLOCKHSYMBOL         11
#define Hv_UPARROWSYMBOL        12
#define Hv_DOWNARROWSYMBOL      13  
#define Hv_LEFTARROWSYMBOL      14  
#define Hv_RIGHTARROWSYMBOL     15   

#define Hv_HEXAGONSYMBOL        16
#define Hv_FOCUSSYMBOL          17
#define Hv_DEFOCUSSYMBOL        18
#define Hv_XSYMBOL              19

#define Hv_USERSYMBOL           1000

/* stipples (patterns) */

#define Hv_FILLSOLIDPAT          0
#define Hv_80PERCENTPAT          1
#define Hv_50PERCENTPAT          2
#define Hv_SCROLLPAT             Hv_50PERCENTPAT
#define Hv_20PERCENTPAT          3
#define Hv_HATCHPAT              4
#define Hv_DIAGONAL1PAT          5
#define Hv_DIAGONAL2PAT          6
#define Hv_HSTRIPEPAT            7
#define Hv_VSTRIPEPAT            8
#define Hv_SCALEPAT              9
#define Hv_STARPAT               10
#define Hv_HOLLOWPAT             11
#define Hv_NUMPATTERNS           12

/* line styles */

#define Hv_SOLIDLINE             0
#define Hv_DASHED                1
#define Hv_LONGDASHED            2
#define Hv_DOTDASH               3

/* parts of a rectangle */

#define Hv_UPPERLEFT            0
#define Hv_MIDDLETOP            1
#define Hv_UPPERRIGHT           2
#define Hv_MIDDLELEFT           3
#define Hv_MIDDLE               4
#define Hv_MIDDLERIGHT          5
#define Hv_LOWERLEFT            6
#define Hv_MIDDLEBOTTOM         7
#define Hv_LOWERRIGHT           8

/* text entry modes */

#define Hv_SINGLELINEEDIT     1
#define Hv_MULTILINEEDIT      2

/* positioning child text items */

#define Hv_POSITIONBELOW      0
#define Hv_POSITIONABOVE      1
#define Hv_POSITIONLEFT       2
#define Hv_POSITIONRIGHT      3
#define Hv_POSITIONCENTER     4

/* Postscript defines */

#define Hv_PSPOSTSCRIPT       1          /* normal postcript file type */
#define Hv_PSEPS              2          /* encapsulated postscript file */

#define Hv_PSLANDSCAPE        1
#define Hv_PSPORTRAIT         2

#define Hv_PSLEFTMARGIN       0.4        /*in inches */
#define Hv_PSTOPMARGIN        0.4        /*in inches */
#define Hv_PSRIGHTMARGIN      0.4        /*in inches */
#define Hv_PSBOTTOMMARGIN     0.4        /*in inches */
#define Hv_PSPIXELDENSITY     600        /*pixels per inch on virtual ps screen */

#define Hv_PSHORIZONTAL       0           /*horizontal text*/
#define Hv_PSVERTICAL         1           /*vertical text*/

#define Hv_PSCAPBUTT	      0           /*controls end of line*/
#define Hv_PSCAPROUND	      1           /*controls end of line*/
#define	Hv_PSCAPPROJECTING    2           /*controls end of line*/

#define	Hv_PSJOINMITER        0           /*controls joining of lines*/
#define Hv_PSJOINROUND	      1           /*controls joining of lines*/
#define Hv_PSJOINBEVEL	      2           /*controls joining of lines*/

/* scroll arrows */

#define Hv_LEFTARROW     01
#define Hv_RIGHTARROW    02
#define Hv_UPARROW       04
#define Hv_DOWNARROW     010

/* Item Types */

#define Hv_USERITEM              1001   /* user defined item */
#define	Hv_RAINBOWITEM		 1002   /* Rainbow strip for "thermal" plots */
#define Hv_SMALLICONITEM         1003   /* small draggable icons */
#define Hv_CONTAINERITEM         1004   /* View container */
#define Hv_FEEDBACKITEM          1005   /* For feedback text */
#define Hv_BUTTONITEM            1006   /* Button with text */
#define Hv_HOTRECTBORDERITEM     1007   /* sculptured frame around the hotrect */

#define Hv_CHOICESETITEM         1008   /* sculptured frame about a set of choice buttons */
#define Hv_RADIOCONTAINERITEM    Hv_CHOICESETITEM /*backward compatibility */

#define Hv_SLIDERITEM            1009   /* slider control */
#define Hv_SLIDERTHUMBITEM       1010   /* thumb within slider control*/
#define Hv_MAPITEM               1011   /* mercator maps */
#define Hv_TEXTITEM		 1012   /* usual string item*/
#define Hv_CLOCKITEM             1013   /* clock item */
#define Hv_COLORBUTTONITEM       1014   /* color button */
#define Hv_WORLDRECTITEM         1015   /* rectangle/grid based on world coordinates */
#define Hv_WORLDLINEITEM         1016   /* line based on world coordinates */
#define Hv_WORLDPOLYGONITEM      1017   /* polygon based on world coordinates */
#define Hv_WORLDOVALITEM         1018   /* oval based on world coordinates */

#define Hv_CHOICEITEM            1019   /* choice button (sits in a Hv_CHOICESETITEM) */
#define Hv_RADIOBUTTONITEM       Hv_CHOICEITEM /*backward compatibility */

#define Hv_BOXITEM               1020   /* simple 3D parent box */ 
#define Hv_OPTIONBUTTONITEM      1021   /* simple option button (toggle) */
#define Hv_WHEELITEM             1022   /* thumb wheel */
#define Hv_SEPARATORITEM         1023   /* typically the child of a box */
#define Hv_CONNECTIONITEM        1024   /* for connecting hotrect items */
#define Hv_PLOTITEM              1025   /* xy plots */
#define Hv_TEXTENTRYITEM         1026   /* JAM's in-place text editor */
#define Hv_FRACTIONDONEITEM      1027   /* usual amount to go item */
#define Hv_LEDBARITEM            1028   /* LED status bar */
#define Hv_GLOBEITEM             1029   /* used in orthographic map projection */
#define Hv_MENUBUTTONITEM        1039   /* on view popup menu */

#define Hv_WORLDWEDGEITEM        1040   /* world wedge */
#define Hv_WORLDIMAGEITEM        1041   /* world image */

#define Hv_BUTTONGROUPITEM       1042   /* netscape like button group */

/* Menu Types */

#define Hv_PULLDOWNMENUTYPE      Hv_USERITEM    /* NOTE: keep this the same as Hv_USERITEM */
#define Hv_SUBMENUTYPE           1002
#define Hv_POPUPMENUTYPE         1003
#define Hv_HELPMENUTYPE          1004

/* Menu Item Types */

#define Hv_STANDARDMENUITEM      Hv_USERITEM    /* NOTE: keep this the same as Hv_USERITEM */
#define Hv_TOGGLEMENUITEM        1002
#define Hv_SEPARATOR             2003
#define Hv_SUBMENUITEM           1004

/* dialog item types */

#define Hv_BUTTONDIALOGITEM         Hv_USERITEM    /* NOTE: keep this the same as Hv_USERITEM */
#define Hv_COLORSELECTORDIALOGITEM  1002

#define Hv_SEPARATORDIALOGITEM      Hv_SEPARATOR

#define Hv_LISTDIALOGITEM           1004
#define Hv_ROWCOLUMNDIALOGITEM      1005
#define Hv_SCALEDIALOGITEM          1006
#define Hv_RADIODIALOGITEM          1007
#define Hv_TOGGLEDIALOGITEM         1008
#define Hv_TEXTDIALOGITEM           1009
#define Hv_CLOSEOUTDIALOGITEM       1010
#define Hv_SCROLLEDTEXTDIALOGITEM   1011
#define Hv_LABELDIALOGITEM          1012
#define Hv_FILESELECTORDIALOGITEM   1013  /* Dat Cao of Sparta, Inc's widget */
#define Hv_ACTIONAREADIALOGITEM     1014  /* upgrade of Hv_CLOSEOUTDIALOGITEM */
#define Hv_DIALOG                   1015  /* Dialog Box widget type */
#define Hv_THINMENUOPTION           1016  
#define Hv_SCROLLBARDIALOGITEM      1017
/* Dialog types */

#define Hv_MODAL                 Hv_USERITEM    /* NOTE: keep this the same as Hv_USERITEM */
#define Hv_MODELESS              1002

/* draw control bits  for VIEWS */

#define	Hv_SQUARE		   01	/* Bit 0  -> Force Square Aspect */
#define Hv_ACTIVE                  02   /* Bit 1  -> View is active (vs. hidden) */
#define Hv_ZOOMABLE                04   /* Bit 2  -> View can be "zoomed" */

#define Hv_ENLARGEABLE            010   /* Bit 3  -> View can be "enlarged" */
#define Hv_ENLARGABLE            Hv_ENLARGEABLE   /* because spelling was debated */

#define Hv_SCROLLABLE                 020   /* Bit 4  -> View can be "Scrolled" */
#define Hv_ZOOMONRESIZE               040   /* Bit 5  -> when view is resized, zoom world system commenurately */
#define Hv_NOHOTRECTFILL             0100   /* Bit 6  -> hot rect is not filled (user must fill e.g. maps) */
#define Hv_SAVEBACKGROUND            0200   /* Bit 7  -> try to save background for faster updates */
#define Hv_DIRTY                     0400   /* Bit 8  -> used with SAVEBACKGROUND to indicate need for update */
#define Hv_FIXEDWORLD               01000   /* Bit 9  -> rarely used means world can never change */
#define Hv_DRAWINGTOOLCOPYRELATIVE  02000   /* Bit 10 -> view to view drawing item copies are relative */
#define Hv_3DVIEW                   04000   /* Bit 11-> This is a 3D View */
#define Hv_MAGNIFY                 010000   /* Bit 12-> pointer mag on */

/* define some useful combinations of draw control bits */

#define Hv_STANDARDDRAWCONTROL    Hv_ZOOMABLE + Hv_ENLARGEABLE + Hv_SCROLLABLE + Hv_SQUARE + Hv_ZOOMONRESIZE

/* resize policys for Hv_RESIZEPOLICY attribute */

#define	Hv_DONOTHINGONRESIZE	    1      /* item stays in same place */
#define Hv_OFFSETONRESIZE 	    2	   /* item offsets by same amount as view */
#define Hv_RESIZEONRESIZE 	    3	   /* item resizes by same amount as children */
#define Hv_RECENTERONRESIZE         4      /* item recenters on resize */
#define Hv_SHIFTXONRESIZE           5      /* only change x coordinate on resize */
#define Hv_SIZEXOFFYONRESIZE        6      /* grow wrt to x but offset wrt y (feedback items) */
#define Hv_SHIFTYONRESIZE           7      /* only change y coordinate on resize */
#define Hv_GROWXONRESIZE            8      /* only expand coordinate on resize */
#define Hv_GROWYONRESIZE            9      /* only expand y coordinate on resize */

/* string extra is for extra space margin in text item areas */

#define Hv_STRINGEXTRA              2
#define Hv_TEXTENTRYEXTRA           4

/* item domain possibilities */

#define Hv_INSIDEHOTRECT                  1  /* a hotrect item */
#define Hv_OUTSIDEHOTRECT                 2  /* guess */

/* draw control bits for Hv_Items */

#define Hv_FRAMEAREA                    01        /* Bit 0  -> frame the area */
#define Hv_HASBALLOON                   02        /* Bit 1  -> item will use balloon help */

/* BIT 2 IS Hv_ZOOMABLE    -- ALREADY DEFINED AS A VIEW DRAW CONTROL -- USE FOR ITEMS TOO */
/* BIT 3 IS Hv_ENLARGEABLE -- ALREADY DEFINED AS A VIEW DRAW CONTROL -- USE FOR ITEMS TOO */

#define Hv_DRAGGABLE                   020        /* Bit 4  -> item is draggable */
#define Hv_DRAGABLE            Hv_DRAGGABLE       /* alternate spelling */
#define Hv_FEEDBACKWHENDRAGGED         040        /* Bit 5  -> feedback when dragged */
#define Hv_FANCYDRAG                  0100        /* Bit 6  -> fancy redrawing while dragging */
#define Hv_ROTATABLE                  0200        /* Bit 7  -> item is rotatable */
#define Hv_NOPOINTERCHECK             0400        /* Bit 8  -> pointer has no need to check this item */
#define Hv_CHECKAREAONLY             01000        /* Bit 9  -> only check area (not region) for selection */
#define Hv_NOUPDATEAFTERDRAG         02000        /* Bit 10 -> item is not redrawn after drag */
#define Hv_REDRAWHOTRECTWHENCHANGED  04000        /* Bit 11 -> if item (e.g. slider) changes, redraw hotrect*/
#define Hv_ALWAYSREDRAW             010000        /* Bit 12 -> items (typically backgrounds) that are always redrawn */
#define Hv_HIGHLIGHTITEM            020000        /* Bit 13 -> call special draw routine (to show it is being edited) */
#define Hv_CONFINETOPARENT          040000        /* Bit 14 -> cannot drag outside of parent */
#define Hv_DONTDRAW                0100000        /* Bit 15 -> dont draw me! */
#define Hv_DONTAUTOSIZE            0200000        /* Bit 16 -> dont autosize text item */
#define Hv_CONFINETOGRANDPARENT    0400000        /* Bit 17 -> cannot drag outside of grand parent */
#define Hv_DONTUPDATECONNECTIONS  01000000        /* Bit 18 -> do not update connections after a drag */
#define Hv_JUSTMEONTIMER          02000000        /* Bit 19 -> just draw this item on a timed redraw */
#define Hv_DIRTYWHENCHANGED       04000000        /* Bit 20 -> if item (e.g. slider) changes, set view dirty*/


/* next bit is used for different purposes for different items */

#define Hv_ROUNDED               010000000        /* Bit 21 -> rounded shape (for buttons) */

#define Hv_TREATASMAP            Hv_ROUNDED       /* Bit 21 used by wpoly */


#define Hv_ALIGNLEFT             Hv_ROUNDED       /* Bit 21 -> text in box item is left aligned */

#define Hv_INBACKGROUND          020000000        /* Bit 22 -> included in saved background */      
#define Hv_WPOLYBASED            040000000        /* Bit 23 -> based on a world polygon */
#define Hv_STRICTHRCONFINE      0100000000        /* Bit 24 -> strictly confined to hotrect */
#define Hv_CLIPPARENT           0200000000        /* Bit 25 -> never draw outside parent */
#define Hv_CHECKAREAOL1ST       0400000000        /* Bit 26 -> complex objects, check area overlap b4 reg */
#define Hv_LIGHTWEIGHT         01000000000        /* Bit 27 -> lightweight versions */
#define Hv_LOCKED              02000000000        /* Bit 28 -> item is locked and cannot be deleted */
#define Hv_OPTIONALDRAW        Hv_LIGHTWEIGHT     /* piggy back a synonym */
#define Hv_GHOSTED             04000000000        /* Bit 29 -> ghosted (for text)*/
#define Hv_COLLAPSABLE         010000000000       /* Bit 30 -> collapsable boxes*/

/* NOTE CANNOT ADD ANY MORE BITS MUST CAREFULLY PIGGYBACK */



/* numerical constants */

#ifdef WIN32
/* */
#define Hv_PI             (float)3.1415926535         /* take a wild guess */
#define Hv_TWOPI          (float)6.2831853
#define Hv_PIOVER2        (float)1.5707963
#define Hv_PIOVER3        (float)1.0471976
#define Hv_PIOVER4        (float)0.7853982
#define Hv_THREEPIOVER4   (float)2.3561945
#define Hv_THREEPIOVER2   (float)4.7123890
#else
#define Hv_PI             3.1415926535         /* take a wild guess */
#define Hv_TWOPI          6.2831853
#define Hv_PIOVER2        1.5707963
#define Hv_PIOVER3        1.0471976
#define Hv_PIOVER4        0.7853982
#define Hv_THREEPIOVER4   2.3561945
#define Hv_THREEPIOVER2   4.7123890
#endif

/* returns from mouse clicks -- indicating different
   parts of the view */

#define Hv_INGROWBOX            1            /* indicates a mouse selection in the grow box of a view */
#define Hv_INDRAGREGION         2            /* indicates a mouse selection in the drag region of a view */
#define Hv_INCLOSEBOX           3            /* indicates a mouse selection in the close region of a view */
#define Hv_INBIGBOX             4            /* indicates a mouse selection in the enlarge region of a view */
#define Hv_INHORIZONTALTHUMB    5            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INVERTICALTHUMB      6            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INLEFTARROW          7            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INRIGHTARROW         8            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INUPARROW            9            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INDOWNARROW         10            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INLEFTSLIDE         11            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INRIGHTSLIDE        12            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INUPSLIDE           13            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INDOWNSLIDE         14            /* indicates a mouse selection in that part of a scroll bar */
#define Hv_INHOTRECT           15            /* indicates a mouse selection in the view's hot_rect */


#define Hv_DEFAULT      -1    /*signal for default anything -- font etc*/
#define Hv_NONE  	-1    /* None for anything (eg menu accelerator) */

#define Hv_NO           0
#define Hv_YES          1

/* dialog closeouts (bitwise) aka "action" buttons */

#define Hv_OKBUTTON       01
#define Hv_CANCELBUTTON   02          
#define Hv_APPLYBUTTON    04         
#define Hv_DELETEBUTTON  010
#define Hv_DONEBUTTON    020

#define Hv_ALLACTIONS Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON + Hv_DELETEBUTTON

/* dialog answers */

#define Hv_DELETE      -1
#define Hv_CANCEL       0
#define Hv_OK           1
#define Hv_DONE         2
#define Hv_APPLY        3
#define Hv_RUNNING      4

#define Hv_ALT          0     /* Alt (Compose) key accelerator */
#define Hv_CTRL  	1     /* Control key accelerator */

#define Hv_LINESIZE    1024


#define Hv_FOUNDEOF         -1        /* found the end of the file */
#define Hv_FOUNDSTARTREC    -2        /* found the start of a record */
#define Hv_FOUNDENDREC      -3        /* found the end of a record */


#define Hv_OFF                0
#define Hv_ON                 1
#define Hv_NEUTRAL            -1

/* tools */

#define Hv_CAMERATOOL         01
#define Hv_RECTTOOL           02
#define Hv_POLYGONTOOL        04  
#define Hv_TEXTTOOL          010
#define Hv_OVALTOOL          020  
#define Hv_LINETOOL          040  
#define Hv_WEDGETOOL        0100
#define Hv_ALLTOOLS         32767   

/* drawing tool tag added in 10041 to items created
   via the drawing tools */

#define Hv_DRAWINGTOOLTAG   -32133  

/* popup2 and Action menu items */

#define Hv_REFRESHSCREEN           1000
#define Hv_PRINTSCREEN             1001
#define Hv_OPENCONFIGURATION       1003
#define Hv_SAVECONFIGURATION       1004
#define Hv_CHANGEBACKGROUND        1005
#define Hv_QUIT                    1007

/* color menu */

#define Hv_BACKGROUNDCOLOR         1000

/* help menu */

#define Hv_STANDARDHELP            1000
#define Hv_BALLOONHELP             1001

/* Popup Menu */

#define Hv_REFRESHVIEW           01       /* refresh the view */
#define Hv_CUSTOMIZEVIEW         02       /* customize the view */
#define Hv_SENDBACKVIEW          04       /* send to bak of stack */
#define Hv_DELETEVIEW           010       /* delete the view */
#define Hv_PRINTVIEW            020       /* print view to ps file */
#define Hv_SEPVIEW              040       /* separator item */
#define Hv_UNDOVIEW            0100       /* undo menu */
#define Hv_QUICKZOOM           0200       /* quick zoom menu*/
#define Hv_65PERCENTVIEW       0400       /* sixty five percent zoom item */
#define Hv_85PERCENTVIEW      01000       /* eighty five percent zoom item */
#define Hv_110PERCENTVIEW     02000       /* one hundred ten percent zoom item */
#define Hv_125PERCENTVIEW     04000       /* one hundred twenty five percent zoom item */
#define Hv_VARZOOMVIEW       010000       /* variable zoom item */
#define Hv_DEFAULTVIEW       020000       /* restore default world system*/
#define Hv_UNDOZOOM          040000       /* undo last zoom */
#define Hv_SEPVIEW2         0100000       /* another separator item */
#define Hv_ZOOMRESIZETOGGLE 0200000       /* zoom when resizing? */
#define Hv_NUMPOPUP             17

#define Hv_PERCENTVIEWS         Hv_65PERCENTVIEW + Hv_85PERCENTVIEW + Hv_110PERCENTVIEW + Hv_125PERCENTVIEW
#define Hv_ZOOMRELATED          Hv_QUICKZOOM + Hv_VARZOOMVIEW + Hv_DEFAULTVIEW + Hv_ZOOMRESIZETOGGLE
#define Hv_MINPOPUPCONTROL      Hv_REFRESHVIEW + Hv_SENDBACKVIEW + Hv_PRINTVIEW + Hv_UNDOVIEW
#define Hv_STANDARDPOPUPCONTROL Hv_MINPOPUPCONTROL + Hv_CUSTOMIZEVIEW + Hv_PERCENTVIEWS + Hv_ZOOMRELATED

/* Quick Zoom SubMenu */

#define Hv_NUMQUICKZOOM          12        /*number of aviable quickzoom entries */

#define  Hv_NUMSEARCHPATH        50        /* max number of paths searched when an open is attempted */

/*********** Inclusion of other headers ************/

#include "Hv_attributes.h" /* attribute MACRO defintions */
#include "Hv_gateway.h"    /* data structure defs */
#include "Hv_private.h"    /* private declarations */
#include "Hv_fonts.h"      /* font related */

/*------- global variables --------*/

/* use to disable magnification stuff */

extern Boolean     Hv_allowMag;

/* used to control amount of magnification */

extern int         Hv_magfact;

/* used to draw NonBackground items into the background pixmap */

extern Boolean      Hv_drawNBItemsToo;

/* used to globally disable continuous zoom */

extern Boolean      Hv_allowContinuousZoom;

/* used by windows apps */

extern int Hv_iccmdShow;

/* these used to be macros, but for consistency changed to vars.
   they still masquerade as macros */

extern float Hv_RADTODEG;
extern float Hv_DEGTORAD;


/* file separator variables */

extern char *Hv_FileSeparatorString;
extern char  Hv_FileSeparator;

/* path separator variables */

extern char *Hv_PathSeparatorString;
extern char  Hv_PathSeparator;


/* some strings used in callbacks */

extern  char           *Hv_inputCallback;
extern  char           *Hv_activateCallback;
extern  char           *Hv_valueChangedCallback;
extern  char           *Hv_okCallback;
extern  char           *Hv_cancelCallback;
extern  char           *Hv_multipleSelectionCallback;
extern  char           *Hv_singleSelectionCallback;
extern  char           *Hv_thinMenuCallback;

extern Hv_View         Hv_modalView;        /*if not NULL, modal view up*/

extern int             Hv_units;            /* of use in map views */

extern char          **Hv_configuration;    /* null terminated string array holding current config file */

extern Hv_FunctionPtr  Hv_AlternateEventHandler; /* alternate event handler */

extern Hv_FunctionPtr  Hv_UserReadConfiguration;   /* if !null, config file (if found) is passed here */
extern Hv_FunctionPtr  Hv_UserWriteConfiguration;   /* if !null, app can write some to config file */

extern Hv_FunctionPtr  Hv_CreateDrawingItemCB;  /* if !null, called after drawing tool created an item */
extern Hv_FunctionPtr  Hv_UserDrawSymbolsOnPoints;
extern Boolean         Hv_dontSendBack;   /*prevent view from being sent back on shift click*/
extern Boolean         Hv_quoteDelimitsSegment;
extern short           Hv_lastDraggedVertex;  /*related to changing world poly */
extern short           Hv_simpleTextBg;
extern short           Hv_simpleTextFg;
extern short           Hv_simpleTextBorder;

extern Hv_Widget       Hv_lastLabel;
extern Boolean         Hv_drawingOffScreen;

extern char            *Hv_cachedDirectory;
extern char            *Hv_cachedDirectory1; /* for double file selector */
extern char            *Hv_cachedDirectory2; /* for double file selector */

extern Boolean         Hv_feedbackWhileSelecting;
extern Boolean         Hv_drawingOn;
extern int             Hv_worldLinePolicy;
extern Hv_Item         Hv_printThisPlotOnly;
extern Boolean         Hv_useDrawImage;
extern Boolean         Hv_trapCommandArgs;
extern char           *Hv_leftOverArgs;

/* try another approach, namely do NOT concatenate
   left over args for easier processing */

extern int             Hv_argc;
extern char           **Hv_argv;

extern short           Hv_fancySymbolLite;
extern int             Hv_screenResolution;               /* pixels per inch*/
extern Boolean         Hv_printAllColorsBlack;
extern float           Hv_sciNotHiLimit;
extern float           Hv_sciNotLowLimit;
extern Boolean         Hv_firstExposure;
extern Boolean         Hv_simpleSymbols;
extern int             Hv_version;    /*100 * version*/

extern Hv_Item         Hv_activeTextItem;

extern int             Hv_theEnlargedPolyNp;
extern Hv_Point        *Hv_theEnlargedPoly;         /* poly being enlarged */
extern Hv_FunctionPtr  Hv_UserLineEnlargeCheck;     /* called when enlarging poly*/
extern Hv_FunctionPtr  Hv_FinalUserInit;
extern Hv_FunctionPtr  Hv_DuringRectDrag;
extern Hv_FunctionPtr  Hv_DanglingTextItemCallback;
extern Hv_FunctionPtr  Hv_userItemName;
extern Hv_FunctionPtr  Hv_HighlightDraw;
extern Hv_FunctionPtr  Hv_FunctionKeyCallback[9];
extern Hv_Widget       Hv_quitItem;
extern Hv_Widget       Hv_quitPopupItem;
extern Boolean         Hv_handlingExposure;

/* various objects to be excluded from exposure updating */

extern Hv_Item         Hv_excludeFromExposure;

extern unsigned int    Hv_tileX, Hv_tileY;
extern Hv_FunctionPtr  Hv_TileDraw;
extern Boolean         Hv_fKeysActive;         /* if true, fkeys provide diagnostic messages */
extern Boolean         Hv_offsetDueToScroll;   /* indicates a call to OffSetItem is due to a scroll, not moving a view */
extern short           Hv_scrollDH;            /* used as hint that redraw is result of scroll */
extern short           Hv_scrollDV;            /* used as hint that redraw is result of scroll */
extern Boolean         Hv_offsetDueToDrag;     /* indicates a call to OffSetItem is due to a drag, not moving a view */ 


extern Hv_Visual      *Hv_visual;              /* default visual */
extern Hv_Display     *Hv_display;	       /* what display (i.e. cpu) is used */
extern Hv_Window       Hv_mainWindow;          /* main application window */
extern Hv_Cursor       Hv_defaultCursor;       /* default cursor */
extern Hv_Cursor       Hv_crossCursor;         /* crosshair cursor */
extern Hv_Cursor       Hv_waitCursor;          /* wait cursor */
extern Hv_Cursor       Hv_dragCursor;          /* for dragging a view*/
extern Hv_Cursor       Hv_bottomRightCursor;   /* for resizing a view*/
extern Hv_Cursor       Hv_dotboxCursor;        /* a box with a dot in the middle */
extern char           *Hv_hostName;            /* e.g. "clas01" */
extern char           *Hv_userName;            /* e.g. "bogdan" */
extern char           *Hv_osName;              /* e.g. "Minux"  */

/* function pointers for specialized use responses following
   some action */

extern Hv_FunctionPtr  Hv_CustomizeQuickZoom;  /*user provided to modify quickzoom labels */
extern Hv_FunctionPtr  Hv_QuickZoom;           /*should be redirected to app specific*/
extern Hv_FunctionPtr  Hv_UserZoom;            /*can be used for specialized action AFTER standard zoom*/
extern Hv_FunctionPtr  Hv_UserMove;            /*can be used for specialized action AFTER standard move*/
extern Hv_FunctionPtr  Hv_UserResize;          /*can be used for specialized action AFTER standard resize*/
extern Hv_FunctionPtr  Hv_ClickIntercept;      /*used to intercept click */
extern Boolean         Hv_ignoreClickIntercept; /*used to ignore above */

extern Hv_FunctionPtr  Hv_UserDefaultWorld;    /*to intercept standard default world */

extern Hv_FunctionPtr  Hv_AfterItemRotate;     /*called after rotating items*/
extern Hv_FunctionPtr  Hv_AfterTextEdit;       /*called after a text edit*/

extern Boolean         Hv_dontZoom;            /*if true, dont zoom after selectrect but call Hv_InsteadOfZoom*/
extern Boolean         Hv_timedRedraw;         /*if true, drawing is occuring because of a timed redraw */

/* Pointer Stuff */

extern Hv_Item         Hv_hotItem;        /* Item selected by button press */
extern Hv_View         Hv_hotView;        /* view selected by button press */

extern Boolean         Hv_completeFeedback;    /*set to false for faster feedback */
extern Boolean         Hv_extraFastPlease;     /*a signal (e.g. when dragging an item) to draw as fast as poss*/  

extern  Boolean        Hv_inPostscriptMode;          /* True only during postscript drawing */
extern  short          Hv_numColors;
extern unsigned long   Hv_colors[Hv_COLORS];      /* colors (pixel vals into the colormap)*/
extern  Boolean        Hv_readOnlyColors;

extern  short          Hv_globalFeedbackLabelColor;
extern  short          Hv_globalFeedbackDataFont;

/* define some constant colors-- these correspond to indices
   in the colors array, i.e. colors[Hv_green] means that
   colors[Hv_green].pixel is the corresponding pixel in
   the color map */

extern short   	Hv_red,           Hv_blue,        Hv_green,         Hv_yellow,       Hv_olive,
                Hv_slateBlue,     Hv_orange,      Hv_purple,        Hv_aquaMarine,   Hv_peru,
                Hv_violet,        Hv_brown,       Hv_darkSeaGreen,  Hv_mistyRose,    Hv_peachPuff,
                Hv_lavender,      Hv_powderBlue,  Hv_forestGreen,   Hv_aliceBlue,    Hv_rosyBrown,
                Hv_sandyBrown,
                Hv_gray0,         Hv_gray1,       Hv_gray2,         Hv_gray3,        Hv_gray4,
                Hv_gray5,         Hv_gray6,       Hv_gray7,         Hv_gray8,        Hv_gray9,
                Hv_gray10,        Hv_gray11,      Hv_gray12,        Hv_gray13,       Hv_gray14,
                Hv_gray15,        Hv_black,       Hv_white,         Hv_darkGreen,    Hv_honeydew,
                Hv_gray16,        Hv_coral,       Hv_maroon,        Hv_skyBlue,      Hv_darkBlue,
                Hv_steelBlue,     Hv_tan,         Hv_wheat,         Hv_highlight,    Hv_lightSeaGreen,
                Hv_tan3,          Hv_chocolate,   Hv_lawnGreen,     Hv_seaGreen,     Hv_mediumForestGreen,        
                Hv_cadetBlue,     Hv_royalBlue,   Hv_dodgerBlue,    Hv_navyBlue,     Hv_cornFlowerBlue, 
                Hv_canvasColor;


/* the "Hv_hot" variables are used to communicate choices
   made from an option  menu popup on a dialog */

extern short            Hv_hotFont;       /* used for selecting fonts from a menu */
extern short            Hv_hotFontFamily; /* used for selecting fonts from a menu */
extern short            Hv_hotFontSize;   /* used for selecting fonts from a menu */
extern short            Hv_hotFontStyle;  /* used for selecting fonts from a menu */
extern short            Hv_hotSymbol;     /* used for selecting symbols from a menu */
extern short            Hv_hotStyle;      /* used for selecting a line style from a menu */
extern short            Hv_hotArrowStyle; /* used for selecting an arrow style from a menu */

/* Global graphics variables */


extern View_ListHead   Hv_views;	      /* bookkeeper for our view list */

extern Hv_View         Hv_welcomeView;
extern Hv_View         Hv_virtualView;

/* standard menus */

extern Hv_Widget          Hv_helpMenu;
extern Hv_Widget          Hv_actionMenu;
extern Hv_Widget          Hv_viewMenu;
extern Hv_Widget          Hv_quickZoomSubmenu;
extern Hv_Widget          Hv_popupItems[Hv_NUMPOPUP]; 
extern Hv_Widget          Hv_quickZoomItems[Hv_NUMQUICKZOOM]; 

/* user menus */

extern Boolean            Hv_userMenu;


/*-------- Hv_attributes.c ----------*/

extern void Hv_VaGetItemAttributes(Hv_Item Item,
				   ...);

extern void Hv_VaGetViewAttributes(Hv_View View,
				   ...);


/*-------- Hv_box.c --------*/

#ifdef __cplusplus
extern "C"
{
#endif

extern void            Hv_DrawBoxItem(Hv_Item,
				      Hv_Region);

extern void            Hv_UpdateBoxItem(Hv_Item,
					char *);


/*-------- Hv_button.c ------*/

extern void            Hv_DrawButtonItem(Hv_Item,
					 Hv_Region);

extern void            Hv_ButtonDrawingArea(Hv_Item,
					    Hv_Rect *);

extern void            Hv_ToggleButton(Hv_Item);

extern void            Hv_ButtonSingleClick(Hv_Event);

extern short           Hv_GetButtonState(Hv_Item);

extern void            Hv_SetButtonState(Hv_Item,
					 short);

/*-------- Hv_buttongroup.c ------*/

extern Hv_ButtonGroupData *Hv_ButtonGroupGetData(Hv_Item Item);

extern void            Hv_ButtonGroupPosSetEnabled(Hv_Item  Item,
						   int      pos,
						   Boolean  enab,
						   Boolean  redraw);

extern int             Hv_ButtonGroupLabelToPos(Hv_Item Item,
						char    *label);

extern void            Hv_ButtonGroupPosSetState(Hv_Item  Item,
						 int      pos,
						 int      state,
						 Boolean  redraw);

extern void            Hv_ButtonGroupToggleState(Hv_Item  Item,
						 int      pos);

extern void            Hv_ButtonGroupChangeLabel(Hv_Item Item,
						 int     pos,
						 char   *label);

extern Hv_ButtonGroupCallbackData *Hv_ButtonGroupGetCallbackData(Hv_Event hvevent);

/*-------- Hv_callback.c -------*/

extern void            Hv_ResetActiveView(Hv_Event,
					  Hv_View *,
					  Hv_Item *,
					  Hv_Cursor);

extern void            Hv_StandardFirstEntry(Hv_Event,
					     Hv_FunctionPtr,
					     Hv_View *,
					     Hv_Item *,
					     Hv_Cursor *);


/*-------- Hv_choice.c --------*/

extern short           Hv_HandleChoice(Hv_Item);

extern void            Hv_PosSetChoice(short       pos,
				       Hv_Item     choiceset);

extern void            Hv_PosEnableChoice(short       pos,
					  Hv_Item     choiceset,
					  Boolean     enabled);

/************* BACKWARD COMPATIBILITY WRAPPERS ************/
extern short           Hv_HandleRadioButton(Hv_Item);

extern void            Hv_PosSetActiveRadio(short       pos,
					    Hv_Item     radiocont);
/***********************************************************/


/*------- Hv_colorbutton.c -------*/

extern void            Hv_DrawColorButtonItem(Hv_Item,
					      Hv_Region);

extern void            Hv_ChangeColorButtonText(Hv_Item,
						char *,
						Boolean);

extern short           Hv_GetColorButtonColor(Hv_Item);

extern void            Hv_EditColorButtonItem(Hv_Event);

extern void            Hv_ToggleColorButtonItem(Hv_Event);

/*-------- Hv_compound.c ------------*/

extern void	      Hv_CompoundStringDimension(Hv_String  *str,
						 short     *w,
						 short     *h);

extern void           Hv_CompoundStringArea(Hv_String *str,
					    short     x,
					    short     y,
					    Hv_Rect   *area,
					    short     extra); 

extern void           Hv_CompoundStringXYFromArea(Hv_String *str,
						  short     *x,
						  short     *y,
						  Hv_Rect   *area,
						  short     extra);
    

/*-------- Hv_connect.c ----------*/

extern void            Hv_ToggleConnection(Hv_Item);

extern void            Hv_DisableConnection(Hv_Item);

extern void            Hv_EnableConnection(Hv_Item);

extern Hv_Item         Hv_AddConnection(Hv_Item,
					Hv_Item,
					short,
					short,
					short,
					short,
					short,
					short,
					Boolean,
					Hv_CheckProc,
					Boolean);

extern Hv_Item         Hv_MultiplexConnection(Hv_Item,
					      Hv_Item,
					      short,
					      short,
					      short,
					      short,
					      short,
					      short,
					      short,
					      Boolean,
					      Hv_CheckProc);

extern void            Hv_UpdateConnections(Hv_View,
					    Hv_Item,
					    Boolean);

extern void            Hv_RemoveConnections(Hv_Item,
					    short,
					    Boolean,
					    Boolean,
					    Boolean);

extern void            Hv_RemoveConnection(Hv_Item,
					   Hv_Item,
					   short,
					   Boolean);

extern void            Hv_RemoveViewConnections(Hv_View,
						Boolean,
						short);

extern Hv_Region       Hv_ConnectionRegion(Hv_Item);

extern Hv_Item         Hv_DuplicateConnection(Hv_Item,
					      Hv_Item,
					      short);

extern void            Hv_ChangeConnectionStyles(Hv_View,
						 Hv_Item,
						 short,
						 short,
						 short,
						 short,
						 short,
						 short);


extern Hv_Widget       Hv_CreateOptionMenu(Hv_Widget      rc,
					   short          font,
					   char          *title,
					   int            numitems,
					   char          *items[],
					   Hv_FunctionPtr CB);




/*------- Hv_dlogmenus.c -----------*/

extern Hv_Widget       Hv_CreateAxesTypePopup(Hv_Widget  rc,
					      short font);

extern Hv_Widget       Hv_CreatePlotTypePopup(Hv_Widget  rc,
					      short font);

extern Hv_Widget       Hv_CreateConnectionTypePopup(Hv_Widget  rc,
						    short font);

extern Hv_Widget       Hv_CreateFitTypePopup(Hv_Widget  rc,
					     short font);

extern Hv_Widget       Hv_CreateSymbolPopup(Hv_Widget rc,
					    short  font);

extern Hv_Widget       Hv_CreateFontFamilyPopup(Hv_Widget rc,
					    short  font);

extern Hv_Widget       Hv_CreateFontSizePopup(Hv_Widget rc,
					    short  font);

extern Hv_Widget       Hv_CreateFontStylePopup(Hv_Widget rc,
					    short  font);

extern Hv_Widget       Hv_CreateLineStylePopup(Hv_Widget rc,
					       short  font);

extern Hv_Widget       Hv_CreateArrowStylePopup(Hv_Widget rc,
						short  font);


/*--------- Hv_dlogs.c ---------*/

extern Hv_Widget       Hv_CreateManagedPushButton(Hv_Widget       parent,
						  char           *labelstr,
						  short           font,
						  Hv_CallbackProc callback,
						  Hv_Pointer      data);

extern void            Hv_VaCreateDialog(Hv_Widget *,
					 ...);

extern Hv_Widget       Hv_VaCreateDialogItem(Hv_Widget,
					     ...);

extern void            Hv_SelfDestructInformation(char           **lines,
						  int              flags[],
						  int              nline,
						  int              duration,
						  char            *title,
						  char            *donestring,
						  Hv_FunctionPtr   callback);


extern void            Hv_TimedInformation(char *,
					   int duration);

extern int             Hv_ModifyColor(Hv_Widget,
				      short *,
				      char *,
				      Boolean);

extern int             Hv_ModifyPattern(Hv_Widget,
					short *,
					char *);

extern void            Hv_StopDialogTimer(Hv_Widget dialog);

extern int  Hv_DoDialogWithTimer(Hv_Widget      dialog,
				 Hv_Widget      def_btn,
				 Hv_FunctionPtr tfunc,
				 Hv_Pointer     ptr,
				 unsigned long  ms_interval,
				 Hv_Widget      updatelabel,
				 short          updatelabelfont);

extern void            Hv_OpenDialog(Hv_Widget,
				     Hv_Widget);

extern void            Hv_CloseDialog(Hv_Widget);

extern int             Hv_Wait(int);


/*--------- Hv_dlogsupport.c -------*/

extern void            Hv_SetScaleMinMax(Hv_Widget   w,
					 int      min,
					 int      max);

extern void            Hv_GetOptionMenuButtons(Hv_Widget       optmenu,
					       Hv_WidgetList   *buttons,
					       int             *num_buttons);

extern void            Hv_SetOptionMenu(Hv_Widget     optmenu,
					Hv_WidgetList buttons,
					int           whichone);

extern short           Hv_PosGetOptionMenu(Hv_Widget     optmenu,
					   Hv_WidgetList buttons,
					   int           num_buttons);


extern void            Hv_VariableZoom(Hv_View);

extern Hv_Widget       Hv_GetDialogActionButton(Hv_Widget,
						int);

extern int             Hv_GetDialogAnswer(Hv_Widget);

extern void            Hv_NotYet(void);

extern void            Hv_SetDeleteItem(Hv_Widget,
					Hv_Item);

extern Hv_Widget       Hv_SimpleTextEdit(Hv_Widget,
					 char *,
					 char *,
					 short);

extern Hv_Widget       Hv_SimpleDialogSeparator(Hv_Widget);

extern Hv_Widget       Hv_SimpleColorLabel(Hv_Widget,
					   char *,
					   Hv_FunctionPtr);

extern Hv_Widget       Hv_SimplePatternLabel(Hv_Widget,
					     char *,
					     Hv_FunctionPtr);

extern Hv_Widget       Hv_SimpleToggleButton(Hv_Widget,
					     char *);

extern void            Hv_ChangeLabelColors(Hv_Widget,
					    short,
					    short);


extern void  	       Hv_SetToggleButton(Hv_Widget,
					  Boolean);

extern void            Hv_SetFloatText(Hv_Widget,
				       float,
				       int);

extern void	       Hv_SetIntText(Hv_Widget,
				     int);

extern float	       Hv_GetFloatText(Hv_Widget);

extern int	       Hv_GetIntText(Hv_Widget);




/*------- Hv_draw.c ------*/

extern void            Hv_FrameWedge(short xc,
				     short yc,
				     short rad,
				     int   start,
				     int   del,
				     short color);

extern void            Hv_FrameCircle(short xc,
				      short yc,
				      short rad,
				      short color);


extern void            Hv_FillCircle(short    xc,
				     short    yc,
				     short    rad,
				     Boolean  Frame,
				     short    fillcolor,
				     short    framecolor);

extern void            Hv_FrameOval(Hv_Rect *rect,
				    short    color);

extern void 	       Hv_FillOval(Hv_Rect *rect,
				   Boolean  frame,
				   short    fillcolor,
				   short    framecolor);

extern void Hv_DrawGhostedText(int x,
			       int y,
			       Hv_String *str,
			       short     lightcolor,
			       short     darkcolor);

extern void            Hv_DrawSymbolsOnPoints(Hv_Point *,
					      int,
					      short,
					      short,
					      short);


extern void            Hv_DrawWorldCircle(Hv_View,
					  float,
					  float,
					  float,
					  Boolean,
					  short,
					  short);

extern void            Hv_FrameRect(Hv_Rect *rect,
				    short    color);

extern void 	       Hv_FillRect(Hv_Rect *rect,
				   short    color);


extern void 	       Hv_FillPatternRect(Hv_Rect *,
					  short,
					  short);

extern void            Hv_Simple3DRect(Hv_Rect *,
				       Boolean,
				       short);

extern void            Hv_Simple3DCircle(short     xc,
					 short     yc,
					 short     rad,
					 short     fillcolor,
					 Boolean   out);

extern void            Hv_Simple3DDiamond(short,
					  short,
					  short,
					  short,
					  Boolean);

extern void            Hv_BlockISymbol(short,
				       short,
				       short,
				       short,
				       short);

extern void            Hv_BlockHSymbol(short,
				       short,
				       short,
				       short,
				       short);


extern void            Hv_FramePolygon(Hv_Point *,
				       short,
				       short);

extern void 	       Hv_EraseRect(Hv_Rect *);

extern void            Hv_Draw3DRect(Hv_Rect *,
				     Hv_ColorScheme *);

extern void            Hv_Draw3DCircle(short,
				       short,
				       int,
				       Hv_ColorScheme *);

extern void Hv_DrawGCLine(Hv_View View,
			  float   lat1,
			  float   long1,
			  float   lat2,
			  float   long2,
			  int     np,
			  short   color);



extern void 	       Hv_DrawItem(Hv_Item,
				   Hv_Region);

/*-------- Hv_drawsupport.c -------*/


extern void            Hv_WorldPointToLocalPoint(Hv_View,
						 Hv_FPoint *,
						 Hv_Point *);

extern void            Hv_WorldPoint2ToLocalRect(Hv_View,
						 Hv_FPoint *,
						 Hv_FPoint *,
						 Hv_Rect *,
						 short);

extern void            Hv_ClipRect(Hv_Rect *);


extern void            Hv_RestoreClipRegion(Hv_Region);

extern void 	       Hv_FullClip(void);

extern void            Hv_SetSegment(Hv_Segment *,
				     short,
				     short,
				     short,
				     short);

extern void            Hv_RotateFPoints(float,
					Hv_FPoint *,
					short);

extern void            Hv_RotateFPointsAboutPoint(float,
						  Hv_FPoint *,
						  short,
						  float,
						  float);

extern void            Hv_RotateFPoint(float,
				       Hv_FPoint *);

extern Boolean         Hv_PolygonIntersectsPolygon(Hv_Point *,
						   Hv_Point *,
						   short,
						   short);

extern Boolean         Hv_PolygonIntersectsRect(Hv_Point *,
						short,
						Hv_Rect *);

extern Boolean         Hv_RectIntersectsRect(Hv_Rect *,
					     Hv_Rect *);

extern void	       Hv_PolygonEnclosingRect(Hv_Rect *,
					       Hv_Point *,
					       int);

extern void            Hv_OffsetPolygon(Hv_Point *,
					int,
					short,
					short);

extern void            Hv_CreatePolygonFromRect(Hv_Point *,
						Hv_Rect  *);

extern Boolean         Hv_FPointInFPolygon(Hv_FPoint  fp,
					   Hv_FPoint  *fpoly,
					   int        npoly);

extern Boolean         Hv_PointInPolygon(Hv_Point,
					 Hv_Point *,
					 int);

extern void	       Hv_SetPoint(Hv_Point *,
				   short,
				   short);

extern void	       Hv_SetFPoint(Hv_FPoint *,
				    float,
				    float);

extern void            Hv_InvertColorScheme(Hv_ColorScheme  *);

extern void            Hv_ClipItem(Hv_Item,
				   short,
				   short);

extern Hv_Region       Hv_ItemDrawingRegion(Hv_Item,
					    short,
					    short);


/*--------- Hv_drawtools.c ---------*/

extern void   Hv_AddDrawingTools(Hv_View  View,
				 short    left,
				 short    top,
				 short    orientation,
				 short    resizepolicy,
				 short    fillcolor,
				 Boolean  cameratoo,
				 Hv_Item *first,
				 Hv_Item *last);

/*--------- Hv_feedback.c ---------*/

extern void            Hv_UpdateFeedbackText(Hv_Item);

extern void            Hv_ChangeFeedbackLabel(Hv_View View,
					      short tag,
					      char *label);

extern void            Hv_RefreshFeedbackRegion(Hv_Item,
						Hv_Rect *);

extern void            Hv_UpdateFeedbackArea(Hv_Item);

extern void            Hv_DeleteFeedbackEntry(Hv_Item,
					      short);

extern void            Hv_DestroyFeedbackItem(Hv_Item);

extern void            Hv_ChangeFeedback(Hv_View,
					 short,
					 char *);

extern void            Hv_ModifyFeedback(Hv_View View,
					 short tag,
					 char *label,
					 char *data);

extern void            Hv_AddFeedbackEntry(Hv_Item,
					   short,
					   char *,
					   short,
					   short);

extern void            Hv_AddFeedbackSeparator(Hv_Item);


/*-------- Hv_fractdoneitem.c --------*/

extern void            Hv_FractionDoneUpdate(Hv_Item,
					     float);

/*--------- Hv_init.c ---------*/


extern void            Hv_VaInitialize(unsigned int,
				       char **,
				       ...);

/*--------- Hv_io.c ---------*/

extern char           *Hv_NextTextSegment(char **);

extern int             Hv_CountSegments(char *);



extern void            Hv_AddPath(char *);

extern void            Hv_NextRecordSize(FILE *,
					 char *,
					 char *,
					 int *,
					 int *);

extern char           *Hv_PrependHostToFilename(char *);

extern FILE           *Hv_LookForNamedFile(char *,
					   char **,
					   char *);

extern char           *Hv_FindFileWithExtension(char *,
						char **,
						char *prompt);

extern Boolean         Hv_NotAComment(char *);

extern Boolean         Hv_IsAComment(char *);

extern char           *Hv_GetNextNonComment(FILE *,
					    char *);

extern char           *Hv_FindRecord(FILE *,
				     char *,
				     Boolean,
				     char *);

extern int             Hv_ReadNextLine(FILE *,
				       char *,
				       char *,
				       char *);

extern Boolean         Hv_ReportDatabaseSearch(FILE *,
					       char *,
					       Boolean);

extern void            Hv_RecordLineCount(FILE *,
					  char *,
					  char *,
					  int *,
					  int *);

/*---------- Hv_items.c ----------*/

extern void           Hv_LockItem(Hv_Item Item);

extern void           Hv_UnlockItem(Hv_Item Item);

extern Boolean        Hv_IsItemLocked(Hv_Item Item);

extern Hv_Item        Hv_VaCreateItem(Hv_View, ...);

extern void           Hv_SetItemToDraw(Hv_Item  Item);

extern void           Hv_SetItemNotToDraw(Hv_Item  Item);

extern Boolean        Hv_CheckItemToDraw(Hv_Item  Item);

extern void           Hv_SetItemState(Hv_Item   Item,
				      short     state);

extern short          Hv_GetItemState(Hv_Item   Item);

extern void           Hv_SetItemDrawControlBit(Hv_Item  Item,
					       int      bit);

extern Boolean        Hv_CheckItemDrawControlBit(Hv_Item  Item,
						 int      bit);

extern void           Hv_ClearItemDrawControlBit(Hv_Item  Item,
						 int      bit);

extern void           Hv_HotRectItemsBoundary(Hv_ItemList  items,
					      Hv_Rect      *br);

extern Hv_Item        Hv_ItemMatch(Hv_ItemList  items,
				   short        type,
				   short        tag,
				   short        domain);

extern Hv_Item        Hv_PointInItemMatch(Hv_ItemList  items,
					  short        type,
					  short        tag,
					  short        domain,
					  Hv_Point     pp);

extern Boolean        Hv_PointInItem(Hv_Point  pp,
				     Hv_Item   Item);

extern void           Hv_RedrawItem(Hv_Item   Item);

extern char          *Hv_GetItemText(Hv_Item   Item);

extern void           Hv_SetItemText(Hv_Item   Item,
				     char    *text);

extern void           Hv_SetItemTimedRedraw(Hv_Item        Item,
					    unsigned long  interval);

extern void           Hv_RemoveItemTimedRedraw(Hv_Item  Item);

extern Hv_Region      Hv_GetItemDrawingRegion(Hv_Item    Item,
					      short      dh,
					      short      dv);

extern Hv_Region      Hv_GetMinimumItemRegion(Hv_Item  Item);

extern void           Hv_SetItemArea(Hv_Item  Item,
				     short    left,
				     short    top,
				     short    width,
				     short    height);

extern Hv_Rect       *Hv_GetItemArea(Hv_Item  Item);

extern void           Hv_GetTotalItemArea(Hv_Item  Item,
					  Hv_Rect  *rect);

extern void           Hv_SetItemRegion(Hv_Item   Item,
				       Hv_Region region);

extern Hv_Region      Hv_GetItemRegion(Hv_Item  Item);

extern void           Hv_SetItemString(Hv_Item   Item,
				       Hv_String *str);

extern Hv_String     *Hv_GetItemString(Hv_Item  Item);

extern void           Hv_SetItemData(Hv_Item  Item,
				     void    *data);

extern void          *Hv_GetItemData(Hv_Item    Item);

extern void           Hv_SetItemColor(Hv_Item  Item,
				      short    color);

extern short          Hv_GetItemColor(Hv_Item    Item);

extern void           Hv_SetItemTag(Hv_Item  Item,
				    short    tag);

extern short          Hv_GetItemTag(Hv_Item    Item);

extern Hv_View        Hv_GetItemView(Hv_Item    Item);


extern void           Hv_MoveItemBehindItem(Hv_Item moveItem,
					    Hv_Item stayItem);

extern void           Hv_SendItemToBack(Hv_Item  Item);

extern void           Hv_SendItemToFront(Hv_Item  Item);

extern void           Hv_DumpItemList(Hv_ItemList  items,
				      FILE        *fp,
				      int          nspace);

extern void           Hv_DeleteItem(Hv_Item  Item);

extern void           Hv_DestroyItem(Hv_Item    Item);

extern void           Hv_EnableItem(Hv_Item Item);

extern void           Hv_DisableItem(Hv_Item Item);

extern void           Hv_EnableAllItems(Hv_View   View);

extern void           Hv_DisableAllItems(Hv_View   View);

extern void           Hv_SetItemSensitivity(Hv_Item  Item,
					    Boolean  val,
					    Boolean  Redraw);

extern Boolean        Hv_ItemIsEnabled(Hv_Item Item);

extern void           Hv_KillItemList(Hv_ItemList   items);

extern void           Hv_OffsetItem(Hv_Item    Item,
				    short      dh,
				    short      dv,
				    Boolean    children);

extern void           Hv_OffsetItemList(Hv_ItemList   items,
					short         dh,
					short         dv);

extern void           Hv_OffsetHotRectItems(Hv_ItemList   items,
					    short        dh,
					    short        dv);

extern void           Hv_ResizeItem (Hv_Item     Item,
				     short       dh,
				     short       dv,
				     Boolean     children);

extern void           Hv_ResizeItemList(Hv_ItemList   items,
					short         dh,
					short         dv);


extern void            Hv_GetItemCenter(Hv_Item   Item,
					short    *xc,
					short    *yc);

extern void	       Hv_DrawItemList(Hv_ItemList	items,
				       Hv_Region	region);

extern void            Hv_UpdateItem(Hv_Item   Item);

extern Hv_Item         Hv_GetItemFromTag(Hv_ItemList  items,
					 short        tag);

extern Hv_Region       Hv_CopyItemRegion(Hv_Item  Item,
					 Boolean  IncludeChildren);

extern void            Hv_AddItemToList(Hv_ItemList	items,
					Hv_Item    	Item);


extern void            Hv_MaybeDrawItem(Hv_Item 	Item,
					Hv_Region	region);

extern void            Hv_FixItemWorldArea(Hv_Item  Item,
					   Boolean  children);

/*-------- Hv_led.c --------*/

extern void            Hv_UpdateLED(Hv_Item,
				    int,
				    int);




/*-------- Hv_math.c -------*/

extern float         Hv_Distance(Hv_FPoint f1,
				 Hv_FPoint f2);

extern short         Hv_InRange(float v);

extern void          Hv_WorldToLocal(Hv_View     View,
				     float     fx,
				     float     fy,
				     short     *x,
				     short     *y);

extern void           Hv_LocalToWorld(Hv_View     View,
				      float     *fx,
				      float     *fy,
				      short     x,
				      short     y);

extern int             Hv_RandomInt(int   *seed,
				    int   maxval);

extern void            Hv_IndexSort(int     n,
				    float  *arr,
				    int    *indx);

/*--------- Hv_menubuttons.c -------*/

extern void            Hv_RemoveMenuButtonEntry(Hv_Item  Item,
						int      pos);

extern void            Hv_AddMenuButtonEntry(Hv_Item   Item,
					     char     *label,
					     short     font,
					     short     color,
					     short     style);

extern void            Hv_GetMenuButtonChoiceRect(Hv_Item  Item,
						  int      choice,
						  Hv_Rect  *rect);

extern short           Hv_GetCurrentMenuButtonChoice(Hv_Item Item);

extern void            Hv_SetCurrentMenuButtonChoice(Hv_Item Item,
						     short choice);

Hv_MenuButtonData      Hv_GetMenuButtonData(Hv_Item Item);

/*--------- Hv_menus.c ---------*/

extern void            Hv_DoPopup2AndActionMenu(Hv_Widget	w,
						Hv_Pointer	data);

extern void            Hv_VaCreateMenu(Hv_Widget *menu,
				       ...);

extern Hv_Widget       Hv_VaCreateMenuItem(Hv_Widget menu,
					   ...);

extern void            Hv_SetQuickZoomLabel(short   item,
					    char   *str);

extern void            Hv_ViewPopup(Hv_View    View,
				    Hv_XEvent  *event);

/*-------- Hv_offscreen.c -----*/

extern void            Hv_SetViewClean(Hv_View  View);

extern Boolean         Hv_ViewSavesBackground(Hv_View  View);

extern Boolean         Hv_IsViewDirty(Hv_View  View);

extern void            Hv_SetViewDirty(Hv_View  View);





/*-------- Hv_optionbutton.c --------*/

extern Hv_OptionButtonData *Hv_MallocOptionButtonData();

extern void  Hv_FreeOptionButtonData(Hv_OptionButtonData *obd);

extern void  Hv_SetOptionButtonData(Hv_OptionButtonData *obd,
				    char           *label,
				    Hv_FunctionPtr  callback,
				    Hv_Item        *item,
				    Boolean        *control,
				    Boolean        enabled,
				    Boolean        redraw);

extern Hv_Item Hv_AddOptionButtonColumn(Hv_Item             parent,
					short               rel_x,
					short               rel_y,
					short               armcolor,
					short               font,
					short               gap,
					Hv_OptionButtonData *obd,
					short               *w,
					short               *h);

extern Hv_Item         Hv_StandardOptionButton(Hv_View  View,
					       Hv_Item  parent,
					       Hv_Item  pitem, 
					       Boolean  *control,
					       char   *label,
					       int    drawcontrol);


extern void            Hv_OptionButtonCallback(Hv_Event  hvevent);

extern Boolean         Hv_GetOptionButton(Hv_Item   Item);

extern void            Hv_SetOptionButton(Hv_Item   Item,
					  Boolean   control);

/*--------- Hv_radiodlogs.c ---------*/

extern void            Hv_RadioListSensitivity(Hv_RadioHeadPtr   radiohead,
					       short        pos,
					       Boolean       val);

extern void Hv_StandardRadioButtonCallback(Hv_Widget w,
					   Hv_Pointer client_data);

extern void            Hv_SetActiveRadioButton(Hv_Widget      nowon,
					       Hv_RadioHeadPtr   radiohead);


extern void            Hv_PosSetActiveRadioButton(short       pos,
						  Hv_RadioHeadPtr  radiohead);


extern Hv_RadioListPtr Hv_GetActiveRadioButton(Hv_RadioHeadPtr radiohead);

extern void           *Hv_GetActiveRadioButtonData(Hv_RadioHeadPtr  radiohead);

extern short           Hv_PosGetActiveRadioButton(Hv_RadioHeadPtr radiohead);

/*------- Hv_rectsupport.c -------*/

extern short           Hv_RectRight(Hv_Rect  *r);

extern short           Hv_RectBottom(Hv_Rect  *r);


extern void            Hv_GetRectCenter(Hv_Rect  *r,
					short   *xc,
					short   *yc);

extern void            Hv_GetFRectCenter(Hv_FRect  *r,
					 float    *xc,
					 float    *yc);

extern void            Hv_FixRectWH(Hv_Rect *theSR);

extern void            Hv_FixRectRB(Hv_Rect *theSR);

extern void            Hv_FixFRectWH(Hv_FRect  *thefSR);

extern void            Hv_InitRect(Hv_Rect *theSR);

extern void            Hv_CopyRect (Hv_Rect *dest,
				    Hv_Rect *src);

extern void            Hv_CopyFRect (Hv_FRect  *dest,
				     Hv_FRect  *src);


extern void            Hv_GetRectLTRB(Hv_Rect  *rect,
				      short   *l,
				      short   *t,
				      short   *r,
				      short   *b);

extern void            Hv_SetRect(Hv_Rect    *srect,
				  short      L,
				  short      T,
				  short      W,
				  short      H);  

extern void            Hv_SetFRect(Hv_FRect  *frect,
				    float      xmin,
				    float      xmax,
				    float      ymin,
				    float      ymax);

extern void            Hv_ResizeRect (Hv_Rect     *rect,
				      short      dh,
				      short      dv);

extern void            Hv_OffsetRect (Hv_Rect  *rect,
				      short   dh,
				      short   dv);

extern void            Hv_PrintRect(FILE *file,
				    Hv_Rect *rect,
				    char * message);

extern Boolean         Hv_PointInRect(Hv_Point   pp,
				      Hv_Rect   *rect);

extern Boolean         Hv_FPointInFRect(Hv_FPoint	pp,
					 Hv_FRect    *rect);

extern void            Hv_ShrinkRect(Hv_Rect    *r,
				     short     dh,
				     short     dv);

/*---------- Hv_region.c ---------*/

extern void            Hv_PrintRegionBounds(FILE     *file,
					    Hv_Region region,
					    char     *message);

extern void            Hv_EraseRegion (Hv_Region region);

extern void            Hv_FillRegion(Hv_Region  region,
				     short      color,
				     Hv_Region  clipregion);

extern Hv_Region       Hv_CreateRegionFromLine(short  x1,
					       short  y1,
					       short  x2,
					       short  y2,
					       short  slop);

extern Hv_Region       Hv_CreateRegionFromLines(Hv_Point   *pp,
						short       nlines,
						short       slop);

extern void            Hv_KillRegion(Hv_Region  *region);

extern void            Hv_RemoveRectFromRegion(Hv_Region  *reg,
					       Hv_Rect   *rect);

extern Hv_Region       Hv_SubtractRectFromRegion(Hv_Region reg,
						 Hv_Rect  *rect);

extern void            Hv_IntersectRegionWithRegion(Hv_Region *thereg,
						    Hv_Region ireg);

extern void            Hv_CopyRegion(Hv_Region *dreg,
				     Hv_Region sreg);

extern void            Hv_AddRegionToRegion(Hv_Region  *mainreg,
					    Hv_Region  addreg);

extern void            Hv_SubtractRegionFromRegion(Hv_Region  *mainreg,
						   Hv_Region  subreg);

extern void            Hv_UnionPolygonWithRegion(Hv_Point   *poly,
						 int     np,
						 Hv_Region  *region);

extern void            Hv_UnionRectWithRegion(Hv_Rect   *sr,
					       Hv_Region   region);

extern Hv_Region       Hv_RectRegion(Hv_Rect    *rect);


/*-------- Hv_slider.c --------*/

extern void            Hv_SetSliderCurrentValue(Hv_Item  slider,
						short   val,
						Boolean  redraw);

extern short           Hv_GetSliderCurrentValue(Hv_Item  slider);


/*-------- Hv_separator.c --------*/

extern void            Hv_CenterSeparator(Hv_Item  Item);

extern void            Hv_SeparatorResizeToParent(Hv_Item  Item);

/*------- Hv_standard.c -----------*/

extern Hv_Widget       Hv_StandardFileSelectorItem(Hv_Widget parent,
						   short     width,
						   char      *mask);
    
extern void            Hv_StandardRedoItem(Hv_Item  Item);

extern Hv_Widget       Hv_StandardLabel(Hv_Widget  rc,
					char     *label,
					int     type);

extern Hv_Widget       Hv_StandardClearListButton(Hv_Widget  parent,
						  Hv_Widget  list,
						  char    *label,
						  short    font);

extern Hv_Widget       Hv_StandardDeleteButton(Hv_Widget  parent);

extern Hv_Widget       Hv_StandardButton(Hv_Widget    parent,
					 char      *label,
					 Hv_FunctionPtr CB);

extern Hv_Widget       Hv_StandardScale(Hv_Widget    parent,
					int       min,
					int       max,
					int       current,
					Hv_FunctionPtr CB);

extern Hv_Widget       Hv_StandardTextField(Hv_Widget   parent);

extern Hv_Widget       Hv_StandardToggleButton(Hv_Widget    parent,
					       char      *label,
					       Hv_FunctionPtr  CB);

extern Hv_Widget       Hv_StandardDoneButton(Hv_Widget  parent,
					     char    *donestr);

extern Hv_Widget       Hv_StandardActionButtons(Hv_Widget  parent,
						short    spacing,
						int     whichones);

extern Hv_Widget       Hv_StandardActionArea(Hv_Widget  parent,
					     int     whichones);

extern Hv_Widget       Hv_DialogTable(Hv_Widget  parent,
				      short    numrow,
				      short    spacing);

extern Hv_Widget       Hv_DialogRow(Hv_Widget  parent,
				    short    spacing);

extern Hv_Widget       Hv_DialogColumn(Hv_Widget  parent,
				       short    spacing);

extern Hv_Widget       Hv_TightDialogTable(Hv_Widget  parent,
					   short    numrow,
					   short    spacing);

extern Hv_Widget       Hv_TightDialogRow(Hv_Widget  parent,
					 short    spacing);

extern Hv_Widget       Hv_TightDialogColumn(Hv_Widget  parent,
				       short    spacing);

extern void            Hv_StandardTools(Hv_View,
					short,
					short,
					short,
					short,
					int,
					Hv_Item *,
					Hv_Item *);

/*-------- Hv_textitem.c --------*/

extern void           Hv_TextItemAfterDrag(Hv_Item  Item,
					   short   dh,
					   short   dv);

extern void            Hv_TextItemAfterOffset(Hv_Item  Item,
					      short   dh,
					      short   dv);


extern void            Hv_SizeTextItem(Hv_Item    Item,
				       short     x,
				       short     y);

extern void            Hv_ConcatTextToTextItem(Hv_Item   Item,
					       char     *concattext);

extern void            Hv_ChangeTextItemText(Hv_Item  Item,
					     char    *str,
					     Boolean  redraw);


/*--------- Hv_utilities.c -------*/

extern  char         *Hv_ReplaceSubstring(char *s,
					  char *substr,
					  char *newss);

extern  void          Hv_SetUnits(int units);

extern  short         Hv_ColorFromText(char *line);

extern  short         Hv_PatternFromText(char *line);

extern  short         Hv_SymbolFromText(char *line);

extern Hv_Point      *Hv_NewPoints(int n);

extern Hv_FPoint     *Hv_NewFPoints(int n);

extern Hv_Rect       *Hv_NewRect(void);

extern Hv_FRect      *Hv_NewFRect(void);

extern void           Hv_InitCharStr(char  **dst,
				     char  *sst);

extern void           Hv_DisableWidget(Hv_Widget   w);

extern void           Hv_EnableWidget(Hv_Widget   w);

extern char          *Hv_GetHvTime(void);

extern void          *Hv_Malloc(size_t  size);

extern void           Hv_Free(void  *ptr);

extern void           Hv_CopyString(Hv_String **dstr,
				    Hv_String *sstr);

extern Hv_String            *Hv_CreateString(char  *text);

extern void           Hv_DestroyString(Hv_String   *str);

extern Boolean        Hv_Contained(char *str,
				   char *substr);

extern void	      Hv_StringDimension(Hv_String  *str,
					 short     *w,
					 short     *h);

extern void           Hv_CharDimension(char  *text,
				       short  font,
				       short  *w,
				       short  *h);

extern void	      Hv_NumberToString(char	*text,
					float	v,
					short	p,
					float  eps);

extern  int Hv_StrLen(char *);

/*--------- Hv_views.c ---------*/

extern void Hv_MagnifyView(Hv_View,
			   Hv_Point *);

extern short   Hv_GetViewBackgroundColor(Hv_View);

extern void    Hv_SetViewBackgroundColor(Hv_View,
					 short);

extern Boolean Hv_ItemExistsInView(Hv_View   View,
			    Hv_Item   Item);

extern Boolean Hv_IsHotRectOccluded(Hv_View View);

extern Boolean  Hv_ViewIs3D(Hv_View  View);

extern Boolean Hv_CheckPercentCanZoom(Hv_View View,
				      float   amount);

extern Boolean Hv_CheckCanZoom(Hv_View   View,
			       Hv_Rect  *rect,
			       Boolean   popdialog);

extern Boolean        Hv_IsMapView(Hv_View View);

extern void	      Hv_DrawHotRectItems(Hv_View     View,
					  Hv_Region	region);


extern void          *Hv_GetViewData(Hv_View View);

extern Hv_Rect       *Hv_GetViewHotRect(Hv_View View);

extern void           Hv_VaCreateView(Hv_View *View,
				      ...);

extern void           Hv_SetViewPopupControl(Hv_View   View,
					     int     bits);

extern void           Hv_SetViewMinimumSize(Hv_View  View,
					    short   minw,
					    short   minh);

extern void           Hv_StopSimulation(Hv_View  View);

extern void           Hv_ChangeSimulationInterval(Hv_View View,
						  unsigned long interval);

extern void           Hv_StartSimulation(Hv_View     View,
					 Hv_FunctionPtr  proc,
					 unsigned long  interval);

extern short          Hv_GetViewTag(Hv_View  View);

extern Hv_ItemList    Hv_GetViewItemList(Hv_View View);

extern Boolean        Hv_ViewIsActive(Hv_View  View);

extern Boolean        Hv_ViewIsVisible(Hv_View  View);

extern void           Hv_SetViewBrickWallItem(Hv_Item   Item);

extern Hv_Region      Hv_GetViewRegion(Hv_View  View);

extern void           Hv_DrawView(Hv_View     View,
				  Hv_Region    region);


extern void	      Hv_DrawControlItems(Hv_View               View,
					  Hv_Region	region);

extern void           Hv_DrawViewHotRect(Hv_View  View);

extern void           Hv_RedrawViewHotRect(Hv_View  View,
					   short   type,
					   Hv_Region cregion);

extern void	      Hv_DrawNonBackgroundItems(Hv_View     View,
						Hv_Region   region);

extern Hv_Region      Hv_HotRectRegion(Hv_View   View);

extern Hv_Region      Hv_ClipHotRect(Hv_View   View,
				     short    shrink);

extern Hv_Region      Hv_ClipView(Hv_View  View);

extern void           Hv_FreeView(Hv_View    View);

extern void           Hv_EnableView(Hv_View  View);

extern void           Hv_DisableView(Hv_View  View);

extern char          *Hv_GetViewTitle(Hv_View  View);


extern void           Hv_SetViewTitle(Hv_View   View,
				      char    *title);

extern void           Hv_FitFeedback(Hv_View  View,
				     short   margin);

extern void           Hv_ChangeViewLocal(Hv_View  View,
					 Hv_Rect  *newlocal);
/*-------- Hv_wheel.c --------*/

extern void           Hv_GetWheelValues(Hv_Item   wheel,
					short    *minval,
					short    *maxval,
					short    *val);

extern short          Hv_GetWheelCurrentValue(Hv_Item  wheel);

extern void           Hv_SetWheelValues(Hv_Item  wheel,
					short   vmin,
					short   vmax,
					short   val,
					Boolean  redraw);

extern void           Hv_SetWheelCurrentValue(Hv_Item  wheel,
					      short   val,
					      Boolean  redraw);


/*-------- Hv_worldimage.c ----------*/

extern void           Hv_LatLongRectToLocalRect(Hv_View View,
				      float   latmin,
				      float   latmax,
				      float   longmin,
				      float   longmax,
				      Hv_Rect *rect);

extern void           Hv_EditWorldImageItem(Hv_Event   hvevent);

extern void           Hv_CheckWorldImageEnlarge(Hv_Item    Item,
						Hv_Point   StartPP,
						Boolean    ShowCorner,
						Boolean   *enlarged);


/*-------- Hv_worldline.c ----------*/

extern void           Hv_EditWorldLineItem(Hv_Event   hvevent);

extern void           Hv_DrawWorldLineItem(Hv_Item    Item,
					   Hv_Region   region);

extern void           Hv_FixLineRegion(Hv_Item  Item);

extern void           Hv_CheckWorldLineEnlarge(Hv_Item  Item,
					       Hv_Point  pp,
					       Boolean  Shifted,
					       Boolean  *enlarged);

/*-------- Hv_worldpolygon.c -----*/

extern void Hv_DrawWorldPolygonRotator(Hv_Item,
								short,
								short,
								short,
								Boolean);


extern Boolean Hv_TreatAsMap(Hv_Item item);

extern void Hv_PolygonAfterRotate(Hv_Item Item);

extern float Hv_WorldPolygonArea(Hv_Item Item);

extern void           Hv_EditWorldPolygonItem(Hv_Event   hvevent);

extern void           Hv_PolygonAfterDrag(Hv_Item  Item,
					  short   dh,
					  short   dv);

extern void           Hv_PolygonAfterOffset(Hv_Item  Item,
					    short   dh,
					    short   dv);

extern void            Hv_OffsetWorldPolygon(Hv_Item   Item,
					     float    dx,
					     float    dy);

extern void            Hv_WorldPolygonToLocalPolygon(Hv_View   View,
						     short    npts,
						     Hv_Point  *poly,
						     Hv_FPoint *wpoly);

extern void            Hv_LatLongPolygonToLocalPolygon(Hv_View   View,
						       short    npts,
						       Hv_Point  *poly,
						       Hv_FPoint *wpoly);

extern void           Hv_LocalPolygonToWorldPolygon(Hv_View   View,
						    short    npts,
						    Hv_Point  *poly,
						    Hv_FPoint *wpoly);

extern void           Hv_LocalPolygonToLatLongPolygon(Hv_View   View,
						      short    npts,
						      Hv_Point  *poly,
						      Hv_FPoint *wpoly);

extern void           Hv_DrawWorldPolygonItem(Hv_Item    Item,
					      Hv_Region   region);

extern void           Hv_FixPolygonRegion(Hv_Item  Item);

extern void           Hv_CheckWorldPolygonEnlarge(Hv_Item    Item,
						  Hv_Point   StartPP,
						  Boolean    Shifted,
						  Boolean   *enlarged);

extern void           Hv_DumpPolygon(Hv_Item   Item,
				     char * message);

/*-------- Hv_worldrect.c ----------*/


extern void           Hv_EditWorldRectItem(Hv_Event   hvevent);

extern void           Hv_DrawWorldRectItem(Hv_Item    Item,
					   Hv_Region   region);

extern void           Hv_WorldRectToLocalRect(Hv_View   View,
					      Hv_Rect   *area,
					      Hv_FRect  *wr);

extern void           Hv_LocalRectToWorldRect(Hv_View   View,
					      Hv_Rect   *area,
					      Hv_FRect  *wr);


extern void           Hv_CheckWorldRectEnlarge(Hv_Item    Item,
					       Hv_Point   StartPP,
					       Boolean    ShowCorner,
					       Boolean   *enlarged);


/*-------- Hv_worldwedge.c -----*/

extern void Hv_WedgeAfterRotate(Hv_Item Item);

extern Hv_Item Hv_CreateMapEllipseWithParent(Hv_View View,
					     Hv_Item parent,
					     float   latitude,
					     float   longitude,
					     float   xrad,
					     float   yrad,
					     float   azimuth);

extern Hv_Item Hv_CreateMapEllipse(Hv_View View,
				   float   latitude,
				   float   longitude,
				   float   xrad,
				   float   yrad,
				   float   azimuth);

extern void           Hv_GetWedgePoly(Hv_Item    Item,
				      Hv_Point   **poly,
				      short      *np,
				      Hv_Point   *rrp);


extern void           Hv_EditWorldWedgeItem(Hv_Event   hvevent);


extern void           Hv_DrawWorldWedgeItem(Hv_Item    Item,
					    Hv_Region   region);

extern void           Hv_WedgeAfterDrag(Hv_Item  Item,
					short   dh,
					short   dv);

extern void           Hv_WedgeAfterOffset(Hv_Item  Item,
					  short   dh,
					  short   dv);

extern void           Hv_FixWedgeRegion(Hv_Item  Item);



#ifdef __cplusplus
}
#endif

#endif

