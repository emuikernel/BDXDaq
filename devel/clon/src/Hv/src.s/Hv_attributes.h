/**
 * <EM> Header containing the macro definitions of attributes. </EM>
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

#ifndef __HVATTRIBH__
#define __HVATTRIBH__


/* ATTRIBUTES --- fields of items and views and dialog widgets
   that can be set in the creation via the variable length argument list

   NOTE: some attributes share the same slot (synonyms) since a) they are the
   same type b) They can have the same default value and c) They are
   never meaningful at the same time.

   NEVER NEVER make an attribute for 0, that is reseverd for the
   NULL termination !!\
*/

#define Hv_DRAWCONTROL         1         /*bitwise drawing control*/

#define Hv_STATE               2         /*e.g. on or off*/
#define Hv_NOWON               Hv_STATE  /*whch radio button is on*/
#define Hv_SHOW                Hv_STATE

#define Hv_TAG                 3         /*a user provided ID */
#define Hv_USER1               4         /*user provided data (int) */
#define Hv_USER2               5         /*user provided data (int) */

#define Hv_SINGLECLICK         6         /*callback for a single click */
#define Hv_CALLBACK            Hv_SINGLECLICK         /*better name for menus*/

#define Hv_DOUBLECLICK         7         /*callback for a double click */
#define Hv_USERDRAW            8         /*user provided drawing routine */
#define Hv_INITIALIZE          9         /*user provided initialization routine */
#define Hv_CUSTOMIZE           10        /*user provided customization */
#define Hv_DESTROY             11        /*user provided destruction routine */
#define Hv_FEEDBACK            12        /*user provided feedback function */
#define Hv_PARENT              13        /*parent of this item */
#define Hv_USERDRAWCONTROL     14        /*bitwise drawing control available for user flags */
#define Hv_POPUPCONTROL        15        /*bitwise popup menu control */
#define Hv_MINWIDTH            16        /*min view pixel width */
#define Hv_MINHEIGHT           17        /*max view pixel width */
#define Hv_MINZOOMWIDTH        18        /*min zoom world width */
#define Hv_MAXZOOMWIDTH        19        /*max zoom world width */
#define Hv_MINZOOMHEIGHT       20        /*min zoom world height */
#define Hv_MAXZOOMHEIGHT       21        /*max zoom world height */
#define Hv_LEFT                22        /*item area left */
#define Hv_TOP                 23        /*item area top */
#define Hv_WIDTH               24        /*item area width */
#define Hv_RADIUS              Hv_WIDTH  /*for round buttons*/

#define Hv_HEIGHT              25        /*item area height */
#define Hv_BALLOON             26        /*ballon text */

#define Hv_TEXT                27        /*text */
#define Hv_LIST                Hv_TEXT
#define Hv_RADIOLIST           Hv_TEXT
#define Hv_FILENAME            Hv_TEXT

#define Hv_FONT                28        /*font */

#define Hv_COLOR               29        /*item color */
#define Hv_FOREGROUND          Hv_COLOR

#define Hv_TEXTCOLOR           30        /*text color */
#define Hv_CHILDPLACEMENT      31        /*how to place a child */
#define Hv_REDRAWTIME          32        /*time to forced redraw (for animation -- items only)*/

#define Hv_XMIN                33        /*view world system minimum x */ 
#define Hv_XMAX                34        /*view world system maximum x */ 
#define Hv_YMIN                35        /*view world system minimum y */ 
#define Hv_YMAX                36        /*view world system maximim y */ 

#define Hv_STARTX              Hv_XMIN
#define Hv_STOPX               Hv_XMAX
#define Hv_STARTY              Hv_YMIN
#define Hv_STOPY               Hv_YMAX

#define Hv_HOTRECTHMARGIN      37        /*horizontal spacer about hotrect */
#define Hv_HOTRECTVMARGIN      38        /*vertical spacer about hotrect */
#define Hv_HOTRECTWIDTH        39        /*hotrect width */
#define Hv_HOTRECTHEIGHT       40        /*hotrect height */
#define Hv_RELATIVEPLACEMENT   41        /*positioning */

#define Hv_PLACEMENTGAP        42        /*gap used in relative placement */
#define Hv_SPACING             Hv_PLACEMENTGAP

#define Hv_PLACEMENTITEM       43        /*if other than prev is used for relative placement*/

#define Hv_TITLE               44        /*title of a view */

#define Hv_RESIZEPOLICY        45        /*resize policy for an item*/
#define Hv_TYPE                46        /*type of item*/
#define Hv_DOMAIN              47        /*where the item lives*/
#define Hv_ACCELERATORTYPE     48        /*menu accelerators*/
#define Hv_ACCELERATORCHAR     49        /*accelerator character*/

#define Hv_ID                  50        /*for menus*/
#define Hv_SUBMENU             51        /*for attaching a submenu*/
#define Hv_ORIENTATION         52        /*for rowcols in dialogs*/
#define Hv_EDITABLE            53        /*for text fields in dialogs*/
#define Hv_ADDHOTRECT          Hv_EDITABLE      /*for views*/
#define Hv_PADBUTTON           Hv_EDITABLE      /*for button groups*/

#define Hv_DIALOGCLOSEOUT      54        /*ok, cancel, apply*/

#define Hv_DEFAULTTEXT         55        /*for editable strings*/

#define Hv_PACKING             56        /*dialog item packing*/
#define Hv_ALIGNMENT           57        /*dialog item alignment*/

#define Hv_NUMROWS             58        /*number of rows in a horizontal row col*/
#define Hv_NUMCOLUMNS          59        /*num of columns in a vertical rowcol*/
#define Hv_PIXELDY             Hv_NUMROWS
#define Hv_PIXELDX             Hv_NUMCOLUMNS

#define Hv_BORDERWIDTH         60        /*around labels in dlogs*/
#define Hv_BORDERCOLOR         61        /*color of border around labels in dlogs*/

#define Hv_NUMITEMS            62        /*in a scrollable list*/
#define Hv_NUMPOINTS           Hv_NUMITEMS
#define Hv_NUMBUTTONS          Hv_NUMITEMS

#define Hv_NUMVISIBLEITEMS     63        /*in a scrollable list*/

#define Hv_MINVALUE            64        /*for scales*/
#define Hv_MAXVALUE            65        /*for scales*/
#define Hv_CURRENTVALUE        66        /*for scales*/

#define Hv_DATA                67        /*generic Pointer data for user data*/ 

#define Hv_FILLCOLOR           68        /*for text background, mostly*/
#define Hv_BACKGROUND          Hv_FILLCOLOR
#define Hv_HOTRECTCOLOR        Hv_FILLCOLOR
#define Hv_OFFCOLOR            Hv_FILLCOLOR

#define Hv_AFTERDRAG           69        /*function called after a drag*/
#define Hv_FIXREGION           70        /*function called to fix item's region*/
#define Hv_CHECKENLARGE        71        /*check for item enlargement*/
#define Hv_AFTEROFFSET         72        /*function called after item is offset*/
#define Hv_OPTION              73        /*an optional list of options (e.g. set of radio buttons)*/
#define Hv_DRAGCALLBACK        74        /*optional function called when held down*/
#define Hv_CHECKPROC           Hv_DRAGCALLBACK

#define Hv_FEEDBACKDATACOLS    75        /*number of coulums to allow for feedback data string*/
#define Hv_ARMCOLOR            76        /*for option buttons*/
#define Hv_ONCOLOR             Hv_ARMCOLOR
#define Hv_HOTCOLOR            Hv_ARMCOLOR

#define Hv_CONTROLVAR          77        /*boolean associated with a option button*/
#define Hv_CHILDDH             78        /*horizontal offset for a child*/
#define Hv_CHILDDV             79        /*vertical offset fro a child*/
#define Hv_AUTOSIZE            80        /*for relevant objects, tries to auto size*/

#define Hv_GRANDCHILDPLACEMENT 81        /*how to place a grand child */
#define Hv_PRECISION           82        /*precision when writing labels*/
#define Hv_MINCOLOR            83        /*for rainbows*/
#define Hv_TOPCOLOR            Hv_MINCOLOR
#define Hv_MAXCOLOR            84        /*for rainbows*/
#define Hv_ALLOWNOCOLOR        Hv_MAXCOLOR
#define Hv_BOTTOMCOLOR         Hv_MAXCOLOR
#define Hv_NUMLABELS           85        /*how many labels to write*/
#define Hv_SCALEFACTOR         86        /*to change units for labels*/
#define Hv_COLORWIDTH          87        /*for rainbows*/
#define Hv_LISTSELECTION       88
#define Hv_TEXTENTRYMODE       89
#define Hv_FRAMEWIDTH          90
#define Hv_LISTRESIZEPOLICY    91

#define Hv_XLABEL              92
#define Hv_MASK                Hv_XLABEL


#define Hv_YLABEL              93

#define Hv_DRAWATCREATION      94
#define Hv_OPENATCREATION      Hv_DRAWATCREATION
#define Hv_TRAPCOMMANDARGS     Hv_DRAWATCREATION

#define Hv_REASON              95

/*for simulations*/

#define Hv_SIMPROC             96
#define Hv_USERDATA            Hv_SIMPROC
#define Hv_SIMINTERVAL         97

#define Hv_FEEDBACKENTRY       98
#define Hv_MENUBUTTONENTRY     Hv_FEEDBACKENTRY

#define Hv_FEEDBACKMARGIN      99
#define Hv_ANIMATIONPROC       100
#define Hv_PATTERN             101
#define Hv_NEUTRALCOLOR        Hv_PATTERN
#define Hv_MBCOLOR             Hv_PATTERN

#define Hv_OFFSCREENUSERDRAW   102

#define Hv_HOTRECTLMARGIN      103
#define Hv_HOTRECTTMARGIN      104
#define Hv_HOTRECTRMARGIN      105
#define Hv_HOTRECTBMARGIN      106
#define Hv_VALUECHANGED        107

/*for virtual views*/

#define Hv_USEVIRTUALVIEW      108
#define Hv_VIRTUALWIDTH        109
#define Hv_VIRTUALHEIGHT       110
#define Hv_VIRTUALVIEWHRW      111        /*pixel width of hr on virtual view*/
#define Hv_VIRTUALVIEWHRFILL   112        /*fill color of VV hotrect*/
#define Hv_VIRTUALVIEWFBFILL   113        /*fill color of VV feedback*/
#define Hv_VIRTUALVIEWFBFONT   114        /*font used in VV feedback*/
#define Hv_VIRTUALVIEWFBTEXTCOLOR Hv_TEXTCOLOR
#define Hv_VIRTUALVIEWLMARGIN  115
#define Hv_VIRTUALVIEWTMARGIN  116
#define Hv_VIRTUALVIEWRMARGIN  117
#define Hv_VIRTUALVIEWBMARGIN  118
#define Hv_VIRTUALVIEWFBDATACOLS 119

#define Hv_USEWELCOMEVIEW      120

#define Hv_TITLEPLACEMENT      121

/* added for Hv_MENUBUTTONITEMs */

#define Hv_MENUBUTTONSTYLE     122
#define Hv_MENUBUTTONBEHAVIOR  123
#define Hv_ICONWIDTH           124       /* the icon size when menu is popped*/
#define Hv_ICONHEIGHT          125 
#define Hv_MBICONWIDTH         126       /* the drawing area on the button */
#define Hv_MBICONHEIGHT        127
#define Hv_CURRENTCHOICE       128
#define Hv_ICONDRAW            129
#define Hv_MENUCHOICECB        130
#define Hv_MBNOACTIVEAREA      131       /* if true, no active area */
#define Hv_MBTANDEM            132       /* for tandem callbacks */
#define Hv_MBCHOICECOLOR       133       /* fill color for present choice */

#define Hv_MAINWINDOWCOLOR     134       /*WARNING: this will be a POINTER because color unknown at init*/

/* two attribute for "motionless" feedback updates */

#define Hv_MOTIONLESSFB         134      /* boolean specifying if it should be used */
#define Hv_MOTIONLESSFBINTERVAL 135      /* update interval in msec */

#define Hv_MENUTOGGLETYPE      136
#define Hv_USEEXTENDEDCOLORS   137       /* deprecated, no choice any more */
#define Hv_USERPTR             138       /* user pointer to anything */
#define Hv_FONT2               139       /* second font spec */
#define Hv_GAP                 140       /* used to piggy back placement gap */

#define Hv_VIEWFINALINIT       141       /* useful for pre-zooming views */
#define Hv_MARGIN              142       /* pre 1.0049: piggy backed on Hv_GAP */
#define Hv_AFTERROTATE         143       /*function called after a rotate*/

#define Hv_WMIGNORECLOSE       144      /*igore "close" from Motif menu */
#define Hv_IMAGE               145      /* an XImage */
#define Hv_PICINFO             146      /* a PICINFO */
#define Hv_INITFROMFILE        147      /* tell initialization to read a file*/

#define Hv_LATMIN              148
#define Hv_LONGMIN             149
#define Hv_LATMAX              150
#define Hv_LONGMAX             151
#define Hv_LATITUDE            Hv_LATMIN
#define Hv_LONGITUDE           Hv_LONGMIN

#define Hv_ALTERNATEVIEWMENU   152       /* to place views on other menus */

#define Hv_STRINGARRAY         153
#define Hv_CALLBACKARRAY       154

#define Hv_HGAP                155
#define Hv_VGAP                156

#define Hv_SHOWVALUE           157       /* for sliders and wheels */

#define Hv_VERSIONNUMBER       158       /* 100 x version number */
#define Hv_LABEL               159       /* for mutitudinous labels */
#define Hv_WELCOMETEXT         160       /* shows up in welcome view */
#define Hv_NORAINBOW           161       /* rainbow on welcome view? */

#define Hv_WORDWRAP            162       /* word arap in dialog text area */

#define Hv_SCROLLPOLICY        163
#define Hv_NUMATTRIBUTES       164       /*total number of recognized attributes*/


#endif





