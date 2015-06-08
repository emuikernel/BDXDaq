#ifndef   __SIMPLEH
#define   __SIMPLEH

/* define feedback tags */

#define LOCALX        0
#define LOCALY        1
#define WORLDX        2
#define WORLDY        3

extern   void   simpleInit(void);

extern   void   SetupSimpleView(Hv_View);

extern   void   simpleFeedback(Hv_View,
			       Hv_Point);

#endif
