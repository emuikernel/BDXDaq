h23062
s 00013/00006/01192
d D 1.4 07/11/12 16:41:19 heddle 5 4
c new start counter
e
s 00006/00001/01192
d D 1.3 07/11/03 22:49:30 boiarino 4 3
c fix prototype StartCounterSubPoly()
c 
e
s 00001/00000/01192
d D 1.2 07/10/26 10:02:45 heddle 3 1
c added more scints
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:25 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/proto.h
e
s 01192/00000/00000
d D 1.1 02/09/09 16:27:24 boiarino 1 0
c date and time created 02/09/09 16:27:24 by boiarino
e
u
U
f e 0
t
T
I 1
/*
---------------------------------------------------------------------------
-
-   	File:    proto.h
-
-	prototypes header file for ced version 2.0
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  10/19/94	DPH		Initial Version					
-------------------------------------------------------------------------------
*/

#ifndef __CEDPROTOH
#define __CEDPROTOH


/*------ anchor.c ------*/

extern void AnchorButtonCallback(Hv_Event  hvevent);

extern void DrawAnchorButton(Hv_Item    Item,
			     Hv_Region   region);

/*------ alldc.c --------*/

extern short   GetFirstWireID(short sect,
			      short layer);


extern void  InitDCData();

extern void  EditDCData(void);


extern Boolean GoodDCTDCValue(short rgn,         /*0..2*/
			      short tdc);
 
extern void GetDCTDCValue(short sect,        /* 0..5 */
			  short rgn,         /*0..2*/
			  short supl,        /* 0..1 */
			  short layer,       /* 0..5 */
			  short wire,        /* 0..191*/
			  float *occupancy,  /*occupancy of that superlayer*/
			  int   *nhit,       /*hits in that superlayer*/
			  short *tdc);
 
extern void GetDCAccumValue(short sect,        /* 0..5 */
			    short rgn,         /*0..2*/
			    short supl,        /* 0..1 */
			    short layer,       /* 0..5 */
			    short wire,        /* 0..191*/
			    int  *acccnt,
			    int  *avgtdc,
			    float *avgocc);


extern void DrawAllDCDeadWires(Hv_View   View,
			       short     sect,
			       Hv_Region region);

extern void DrawAllDCHits(Hv_View     View,
			  short       sect,
			  int         numhits,
			  char       *chits,
			  size_t      size,
			  Hv_Region   region);

extern void DrawAllDC1Hits(Hv_View     View,
			   short       sect,
			   int         numhits,
			   DC1DataPtr  hits,
			   Hv_Region   region);

 
extern void DrawAllDC(Hv_Item   Item,
		      Hv_Region region);


extern void  SectorGridSectorRect(Hv_View View,
				  short   sect,
				  Hv_Rect *r);

extern void  AllDCSuperLayerRect(Hv_View View,
				 short   sect,
				 short   rgn,
				 short   supl,
				 Hv_Rect *suplrect);

extern void  AllDCWireRect(short   sect,
			   short   rgn,
			   short   supl,
			   short   layer,
			   short   wire,
			   Hv_Rect *sr,
			   Hv_Rect *layerrect,
			   Hv_Rect *wirerect);

extern void  AllDCLayerRect(short   sect,
			    short   rgn,
			    short   supl,
			    short   layer,
			    Hv_Rect *suplrect,
			    Hv_Rect *layerrect);

extern Hv_Item MallocAllDC(Hv_View View,
			   short   sect,
			   short   superlayer);

extern void  AllDCScintSlabRect(Hv_View View,
				short   sect,
				short   scint,
				Hv_Rect *scintrect,
				Hv_Rect *slabrect);


extern void  GetAllDCCoordinates(Hv_View   View,
				 Hv_Point  pp, 
				 short    *sect, 
				 float    *r,
				 float    *theta,
				 float    *z,
				 float    *x,
				 short    *supl,
				 short    *lay,
				 short    *wire,
				 short    *scint);

extern Hv_Item MallocSectorGrid(Hv_View View);

extern Hv_Item MallocAllDCScint(Hv_View View,
				short   sect);

/*--------- alldcview.c -----------*/

extern void AllDCInitSharedFeedback();

extern void AllDCViewSetup(Hv_View View);

extern void AllDCFeedback(Hv_View View,
			  Hv_Point  pp);


/*----- beamline.c ---------*/

extern Hv_Item MallocBeamlineItem(Hv_View   View);

/*------ bitops.c -----------*/

extern void BinaryPrint(unsigned long  k,
                        unsigned long *ktest);

extern int BitCount(unsigned x);

extern unsigned long  CompositeLong(unsigned short high,
				    unsigned short low);

extern unsigned short  CompositeShort(unsigned char high,
				      unsigned char low);

extern unsigned short  LowWord(unsigned long lw);

extern unsigned short  HighWord(unsigned long lw);

extern unsigned char  LowByte(unsigned short sw);

extern unsigned char  HighByte(unsigned short sw);

void BreakShort(unsigned short sw,
		unsigned char *low,
		unsigned char *high);

extern void BreakLongWord(unsigned int     lw,
			  unsigned short  *high,
			  unsigned short  *low);

extern void  LongWordBytes(unsigned long   lw,
			   unsigned char   *b3,
			   unsigned char   *b2,
			   unsigned char   *b1,
			   unsigned char   *b0);

/*------- calview.c ---------*/

extern void CalorimeterViewSetup(Hv_View View);

extern void CalorimeterViewFeedback(Hv_View  View,
				    Hv_Point pp);

extern void CalorimeterInitSharedFeedback();

/*------ cerenkov.c -----*/

extern void DrawCerenkovHits(Hv_View  View,
			     short   sect,
			     Hv_Region region);

extern Hv_Item MallocCerenkov(Hv_View  View,
			      int    sect,
			      short   ID);

extern void SetCerenkovGeometrySector(Hv_View  View);


extern void GetCerenkovPolygon(Hv_View View,
			       short  sect,
			       short  ID,
			       Hv_Point *poly);

extern void GetCerenkovInnerPolygon(Hv_View View,
				    short  sect,
				    short  ID,
				    Hv_Point *poly);

extern void GetCerenkovHit(short   sect,
			   short   id,
			   short   *tdcl,
			   short   *adcl);

/*------ dc.c -----------*/

extern float SuperlayerOccupancy(short sect,
				 short rgn,
				 short supl,
				 int   *nhit);


extern void    ReadDeadWireFile();

extern Boolean checkDCData(short sect,
			   DC0DataPtr hits);

extern Boolean checkDC1Data(short sect,
			    DC1DataPtr hits); 

extern void SenseWirePosition(float   phi,
			      short   sect,
			      short   rgn,
			      short   suplay,
			      short   layer,
			      short   wire,
			      float   *fz,
			      float   *fx);

void FieldWirePosition(float      phi,
		       short      sect,
		       short      rgn,
		       short      suplay,
		       short      layer,
		       short      sensewire,
		       Hv_FPoint *fpts,
		       short      control);

void GetDCCellPolygon(Hv_View    View,
		      short      sect,
		      short      rgn,
		      short      suplay,
		      short      layer,
		      short      wire,
		      Hv_Point  *pp,
		      Hv_FPoint *fpts);

extern Boolean  IsDead(short sect,
		       short rgn,
		       short supl,
		       short lay,
		       short wire);

extern void DrawDeadWires(Hv_View   View,
			  short     sect,
			  Hv_Region region);

extern void DrawDC0Hits(Hv_View   View,
			short     sect,
			Hv_Region region);

extern void DrawDC1Hits(Hv_View   View,
			short     sect,
			Hv_Region region);

extern void DrawSEGHits(Hv_View   View,
			short     sect,
			Hv_Region region);

extern Hv_Item MallocDC(Hv_View  View,
			int      sect,
			short    superlayer);

extern void SetDCGeometrySector(Hv_View  View);

extern void GetDCHit(Hv_Item   Item,
		     Hv_Point  pp,
		     short     sect,
		     short     superlayer,
		     float    *occupancy,
		     int      *nhit,
		     short    *layer,
		     short    *wire,
		     short    *tdcl);


/*------ dlogs.c ------*/

extern void EventFilterDialog();

extern void DoAboutDlog(void);

extern Hv_Widget StandardInfoPair(Hv_Widget  parent,
				  char      *label);

extern void GetNewView(void);

/*------- draw.c ----------*/

extern void  DumpCRects(short       color,
			Hv_Point    *xp,
			short       occurs[],
			XRectangle  *rect);

extern void  DumpCRectangles(short       color,
			     short       occurs[],
			     XRectangle  *rect);
     

/*------ ec1.c -------*/

extern void EC1SectorViewLongStripPoly(Hv_Item   Item,
				       int       sect,
				       int       layer,
				       int       id,
				       Hv_Point  *poly);

extern void EC1SectorViewShortStripPoly(Hv_Item   Item,
					int       sect,
					int       layer,
					int       id,
					Hv_Point  *poly);

extern void EC1LayerPolygon(Hv_Item   Item,
			    int       sect,
			    int       layer,
			    Hv_Point  *poly);

extern void GetSectorViewEC1FPolygon(short     sect,
				     Hv_FPoint *fpts);
 


extern Hv_Item MallocEC1Shower(Hv_View  View,
			       int      sect);

extern Boolean checkEC1Data(short sect,
			   ECDataPtr hits);

extern void GetSectorViewEC1Polygon(Hv_View   View,
				    short     sect,
				    Hv_Point *poly);


void GetSectViewEC1Hit(short    sect,
		       short    layer,
		       short    strip,
		       short   *tdcL,
		       short   *adcL,
		       short   *tdcR,
		       short   *adcR);

extern void DrawEC1Hits(Hv_View   View,
			short     sect,
			Hv_Region region);

/*------ et.c -------------*/

extern int init_et_(char *session,
		    char *station,
		    int  qsize,
		    int  prescale);

extern int get_et_event_(int timeout);

extern void connect_et();

extern int close_et_();

/*------ evdump.c ----------*/

extern void DumpECPIBank(PiDataPtr pi,
			 int num);


extern void DumpMCINBank(MCINDataPtr mcin,
			 int     num);

extern void DumpHBTRBank(HBTRDataPtr hbtr,
			 int         num);

extern void DumpHBLABank(HBLADataPtr hbla,
			 int         num);

extern void DumpHDPLBank(HDPLDataPtr hdpl,
			 int         num);

extern void DumpTBTRBank(TBTRDataPtr tbtr,
			 int         num);

extern void DumpPARTBank(PARTDataPtr part,
			 int         num);

extern void DumpTBLABank(TBLADataPtr tbla,
			 int         num);

extern void DumpTDPLBank(TDPLDataPtr tdpl,
			 int         num);

D 5
extern void DumpSTBank(STDataPtr st,
E 5
I 5
extern void DumpSTN0Bank(STN0DataPtr st,
E 5
		       int     num);

I 5
extern void DumpSTN1Bank(STN1DataPtr st,
		       int     num);

E 5
extern void DumpTAGEBank(TAGEDataPtr tage,
			 int     num);

extern void DumpTAGTBank(TAGTDataPtr tagt,
			 int     num);

extern void DumpMCVXBank(MCVXDataPtr mcvx,
			 int     num);

extern void DumpMCTKBank(MCTKDataPtr mctk,
			 int     num);

extern void DumpHEADBank(HEADDataPtr   head);

extern void DumpDC0Bank(DC0DataPtr  hits,
			int     num);

extern void DumpDC1Bank(DC1DataPtr  hits,
			int     num);

extern void DumpSEGBank(SEGDataPtr  hits,
			int     num);

extern void DumpSCBank(SCDataPtr  hits,
		       int     num);

extern void DumpCCBank(NormalDataPtr  hits,
		       int       num);

extern void DumpCC1Bank(NormalDataPtr  hits,
			int       num);

extern void DumpECBank(ECDataPtr  hits,
		       int     num);

extern void DumpEC1Bank(EC1DataPtr  hits,
			int     num);


extern Boolean ECBankCB(Hv_Widget w);

extern Boolean DC0BankCB(Hv_Widget   w);

extern Boolean DC1BankCB(Hv_Widget   w);

extern Boolean SEGBankCB(Hv_Widget   w);

extern Boolean SCBankCB(Hv_Widget   w);

extern Boolean EC1BankCB(Hv_Widget   w);

extern Boolean CCBankCB(Hv_Widget   w);

extern Boolean CC1BankCB(Hv_Widget   w);

extern Boolean HBLABankCB(Hv_Widget   w);

extern Boolean HDPLBankCB(Hv_Widget   w);

extern Boolean TBLABankCB(Hv_Widget   w);

extern Boolean TDPLBankCB(Hv_Widget   w);

/*----- event.c --------*/

extern void CloseOutputFile();

extern void OpenOutputFile(void);

extern void WriteEvent(void);

extern void SetEventFileFilter(void);

extern void SetEventBad(int code, int flag);

extern void StartEventTimer();

extern void StopEventTimer();

extern void ZeroEventHistos(int flag);

extern void AccumulateDialog(Hv_Widget w);

extern void AccumulateEvents(int n, int nmax);

extern void EventButtonSelectability();

extern void InitBOS(void);

extern int  GetNumRows(int *);

extern void EventControl(Hv_Widget   w);

extern void NextEvent(void);

extern int  bosBankSize(int index);

extern int  bosBankNumber(int index);

extern void PrevEvent(void);

extern void FirstEvent(void);

extern void SetEventTimeInterval(void);

extern void GotoEvent(void);

extern void  EraseEvent(void);

extern void  EraseViewEvent(Hv_View  View);

extern void  DrawEvent(void);

extern void DrawViewEvent(Hv_View  View,
			  Hv_Region region);

extern void OpenEventFile(char *fn);

extern void OpenEventCB(void);

extern void UpdateEventDialog(void);

extern short MostDataSector(void);

extern short LargestECSumSector(void);

extern void FixMostDataViews(void);

extern void FixLargestECSumViews(void);

extern void bosGetBankIndices(void);

extern void EventTypeString(int  evtype,
			    char  *line);

extern int EventSize(void);

extern  short   CountToColor(int count,
			     Hv_RainbowData *data,
			     int algorithm);


/*------- evstore.c -----------*/

extern void EventStoreConnect(void);


/*-------- fannedscint.c ----------*/

extern	void GetScintViewHit(short sect, 
			     short scint,
			     short *tdcl,
			     short *adcl, 
			     short *tdcr,
			     short *adcr);

extern void DrawScintViewHits(Hv_View   View,
			      short     sect,
			      Hv_Region region);


extern void GetScintCoordinates(Hv_View  View,
				Hv_Point pp,
				short    sect,
				short    *id,
				float    *x,
				float    *y,
				float    *z,
				float    *r,
				float    *theta,
				float    *phi);

extern Hv_Item MallocFannedScint(Hv_View  View,
				 short    sect,
				 Hv_Item  parent);


extern void DrawStartHits(Hv_View   View,
			  Hv_Region region);

extern Hv_Item MallocStart(Hv_View  View);

extern void GetStartHit(short   sect,
			short   tcount,
			short   *tdc,
			short   *adc);

/*-------- fannedshower.c ---------*/

extern void DrawPixel(Hv_View  View,
		      short   sect,
		      short   u,
		      short   v,
		      short   w,
		      short   color,
		      Hv_Region region);


extern float ijToEnergy(float  i,
			float  j,
			int    num,
			PiDataPtr *localhits);

extern void ECPIHitsHere(short sect, short plane,
			 int        *num,
			 PiDataPtr  **localhits);


extern void GetLAECXY(Hv_View  View,
		      Hv_Point pp,
		      short    sect,
		      short    *xstrip,
		      short    *ystrip);

extern void GetShowerUVW(Hv_View  View,
			 Hv_Point pp,
			 short    sect,
			 short    *u,
			 short    *v,
			 short    *w,
			 float    *z,
			 float    *x,
			 float    *y,
			 float    *rad,
			 float    *theta,
			 float    *phi,
			 ijPoint  *rij);

extern void FrameECStrips(Hv_View  View,
			  short    sect,
			  short    type,
			  short    color);

extern Hv_Item MallocForwardEC(Hv_View  View,
			      short   sect,
			      Hv_Item  parent);

extern Hv_Item MallocLAEC(Hv_View  View,
			  short   sect,
			  Hv_Item  parent);

extern void GetShowerUVWHit(short  sect,
			    short  plane,
			    short  u,
			    short  v,
			    short  w,
			    short  *utdc,
			    short  *vtdc,
			    short  *wtdc,
			    short  *uadc,
			    short  *vadc,
			    short  *wadc);

extern void GetLAECHit(short  sect,
		       short  plane,
		       short  xstrip,
		       short  ystrip,
		       short  *xtdcl,
		       short  *xadcl,
		       short  *xtdcr,
		       short  *xadcr,
		       short  *ytdcl,
		       short  *yadcl,
		       short  *ytdcr,
		       short  *yadcr);

extern void DrawFannedShowerHits(Hv_View   View,
				 short     sect,
				 Hv_Region region);

extern void GetShowerHistoHit(Hv_View   View,
			      Hv_Point  pp,
			      short     sect,
			      short     *u,
			      short     *v,
			      short     *w,
			      short     *tdc,
			      short     *adc);


extern void GetLAECHistoHit(Hv_View   View,
			    Hv_Point  pp,
			    short     sect,
			    short     *xstrip,
			    short     *ystrip,
			    short     *tdc,
			    short     *adc,
			    Boolean   *left);


/*------- feedback.c ---------------*/


extern void SuperLayerNameFeedback(Hv_View  View,
				    short    superlayer,
				    char    *text);


extern void InitSharedFeedback(SharedFeedback *sf,
			       short tag,
			       char  *label1,
			       char  *label2);

extern void InitFeedback();

extern void UpdateEventFeedback(void);

extern void ViewFeedback(Hv_View       View,
			 Hv_Point      pp);

extern void StandardChangeFeedback(Hv_View  View,
				   short   tag,
				   char   *text);

extern void StandardFloatChangeFeedback(Hv_View  View,
					short   tag,
					float   val,
					char   *unitstr);

extern void StandardIntChangeFeedback(Hv_View  View,
				      short   tag,
				      int    val,
				      char   *unitstr);

extern void PosDefIntChangeFeedback(Hv_View  View,
				    short   tag,
				    int    val);

extern void PosIntChangeFeedback(Hv_View  View,
				 short   tag,
				 int    val);

extern void ChangeSharedFeedback(Hv_View         View,
				 SharedFeedback *sf,
				 short           slot,
				 char           *text);

extern void PosDefChangeSharedFeedback(Hv_View         View,
				       SharedFeedback *sf,
				       short           slot,
				       short           val);


extern void ClearFeedback(Hv_View View,
			  int opt);

extern  Boolean BField(float   x,
		       float   y,
		       float   z,
		       fvect   B,
		       float  *Bmag);

extern  void InitField(void);

extern  void ReadField(void);

extern  Hv_Item MallocFieldmapItem(Hv_View   View);

/*------ init.c ------*/

extern  void          Init(void);

/*----- library.c ------*/

extern void           ced_plot_(int *intervel);

extern void           ced_refresh_(void);

/*------ math.c -------------*/

extern void          StopWait(void);

extern void          StartWait(void);

extern void  scalevect(fvect   R,
		       float   sf);

extern float fvectmag(fvect   fv);

extern float Cart3DLength(Cart3D  *R);

extern float PixelToPixelWorldDistance(Hv_View   View,
				       short    x1,
				       short    y1,
				       short    x2,
				       short    y2);

extern float Distance3D(float  x1,
			float  y1,
			float  z1,
			float  x2,
			float  y2,
			float  z2);

extern void  ScreenZXtoCLASXYZ(Hv_View  View,
			       float   fz,
			       float   fx,
			       float   *phi,
			       float   *x,
			       float   *y,
			       float   *z) ;

extern void  MidplanePolyToProjectedPoly(Hv_View   View,
					 Hv_FPoint *fpts,
					 short     np);

extern float MidplaneXtoProjectedX(Hv_View   View,
				   float     x);

extern int   XYtoSector(float x,
			float y);

extern void  CLASXYZtoSectorXYZ(int   sect,
				float x,
				float y,
				float z,
				float *sx,
				float *sy,
				float *sz);

extern void  ScreenZXtoCLASCoord(Hv_View  View,
			 	 float   fz,
				 float   fx,
				 float   *phi,
				 float   *x,
				 float   *y,
				 float   *z,
				 float   *r,
				 float   *theta);

extern void  LocaltoCLASXYZ(Hv_View  View,
			    short    pixx,
			    short    pixy,
			    float    *phi,
			    float    *x,
			    float    *y,
			    float    *z);

extern void  CEDxyToCLASxy(float  xced,
			   float  yced,
			   float  *x,
			   float  *y,
			   int    sect);

extern void  LocaltoCLASCoord(Hv_View  View,
			      short    pixx,
			      short    pixy,
			      float    *phi,
			      float    *x,
			      float    *y,
			      float    *z,
			      float    *r,
			      float    *theta) ;

extern void CheckZeroTolerance(float   *v,
			       float   eps);


/*------ menus.c ------*/

extern void EditDetectorColors();

extern void MakeMenus(void);

extern void CustomizeQuickZoom(Hv_View    View);

extern void DoQuickZoom(Hv_Widget   w,
			Hv_Pointer  clientdata);


/*------ message.c --------*/

extern int MultilineQuestion(char **lines,
			     int  flags[],
			     int  nline);

extern void BroadcastMessage(char *str);

extern void StandardLogMessage(char  *prompt,
				char  *message);

extern void UpdateMessageBox(Hv_View  View,
			     char    *str);

extern void LogMessage(char     *message,
		       int      type,
		       Boolean  timestamp,
		       Boolean  username,
		       Boolean  echo,
		       Boolean  echotohv);

/*------ removenoise.c -----*/

extern void  SetBitHugeWord(HugeWordPtr hw,
			    int n);

extern Boolean  CheckBitHugeWord(HugeWordPtr hw,
				 int n);

extern  HugeWordPtr    mallocHugeWord(int numbits);

extern  void           ClearHugeWord(HugeWordPtr  hw);

extern  void           RemoveDCNoise(Hv_View     view,
				     int         sect);

extern  void           PrintHugeWord(HugeWordPtr  hw);

/*------ scintillator.c -----*/

extern void    ReadDeadTOFFile();

extern Boolean checkSCData(short sect,
			   SCDataPtr hits);


extern void DrawScintHits(Hv_View   View,
			  short     sect,
			  Hv_Region region);

extern Hv_Item MallocScintillator(Hv_View  View,
				  int      sect,
				  short    plane);

extern void SetScintGeometrySector(Hv_View  View);


extern void GetSlabPolygon(Hv_View   View,
			   short     sect,
			   short     slab,
			   Hv_Point *poly);

extern void GetScintHit(short   sect,
			short   scint,
			short   *tdcl,
			short   *adcl,
			short   *tdcr,
			short   *adcr);

extern void ScintSlabLimits(short  ID,
I 3
			    short  sect,
E 3
			    short  *minid,
			    short  *maxid);

extern int PanelFromID(int  ID);

/*------- scintview.c ---------*/

I 4
D 5
/*sergey
E 5
I 5
/*--------- splitStartCounterPoly1 ----------*/

extern void splitStartCounterPoly1(Hv_Point *poly1,
				   Hv_Point poly[4][3]);

extern void splitStartCounterPoly2(Hv_Point *poly2,
				   Hv_Point poly[4][4]);


E 5
E 4
extern void StartCounterSubPoly(Hv_View   View,
D 5
				short     sect,
D 4
				Hv_Point  *poly1,
E 4
I 4
				Hv_Point  **polys);
*/
extern void StartCounterSubPoly(Hv_View   View,
E 5
			    short     sect,
			    Hv_Point  *poly1,
E 4
				Hv_Point  *poly2);

extern void ScintViewSetup(Hv_View View);

extern void ScintViewFeedback(Hv_View  View,
				    Hv_Point pp);

extern void ScintViewInitSharedFeedback();


/*------ sectorhex.c --------*/

extern void      DrawSectorHexItem(Hv_Item   Item,
				   Hv_Region region);


extern Hv_Item        MallocSectorHex(Hv_View View);

/*------- sectorview.c ---------*/

extern void SectorViewSetup(Hv_View View);

extern void SectorViewFeedback(Hv_View  View,
				    Hv_Point pp);

extern void SectorViewInitSharedFeedback();

extern void AddDCColorButtons(Hv_View View,
			      short   top,
			      Hv_Item box);

extern void  ColorButtonCallback(Hv_Event    hvevent );

extern Hv_Item StandardColorButton(Hv_View    View,
				   Hv_Item   parent,
				   char     *label,
				   short     color,
				   int       pos,
				   short     gap,
				   Boolean   allownocol);

extern  Hv_Item  DC1Controls(Hv_View View,
			     short left,
			     short top,
			     short width,
			     short font);


/*------ setup.c ------*/

extern Hv_Item  StandardSeparator(Hv_View View,
				  Hv_Item parent,
				  int     gap);

extern void AlgorithmCB(Hv_Event hvevent);


extern void AddECItems(Hv_View View,
		       Hv_Item parent);

extern void AddScintillatorItems(Hv_View View,
				 Hv_Item  parent);

extern void AddCerenkovItems(Hv_View View);

extern void AddDCItems(Hv_View View);


extern  Hv_Item       StandardOptionButtonBox(Hv_View View,
					      short w);



extern  Hv_Item       StandardOptionButton(Hv_View View,
					   Hv_Item parent,
					   Boolean *control,
					   char *label,
					   short gap,
					   short font);


extern  void          OptionButtonCallback(Hv_Event hvevent);

extern  void          ShowWhatCB(Hv_Event hvevent);

extern  Hv_View       NewView(short tag);

extern  void          SetSelectability(Hv_View View);

extern  ViewData      GetViewData(Hv_View View);

extern  void          UpdateViewTitle(Hv_View View);


/*------ shower.c -----*/

extern int TotalECADCSum(short sect);

extern Boolean checkECData(short sect,
			   ECDataPtr hits);


extern void GetStripTypekValue(short      type,
			       short      plane,
			       float      *k,
			       float      *dk);


extern void  ijktofp(Shower_Package *sp,
		     short         sect,
		     float         i,
		     float         j,
		     float         k,
		     Hv_FPoint     *fp);


extern void GetSectorViewECPolygon(Hv_View   View,
				   short     sect,
				   short     plane,
				   Hv_Point *poly);


extern void GetSectViewShowerHit(Hv_Item  Item,
				 Hv_Point pp,
				 short    sect,
				 short    plane,
				 short   *type,
				 short   *ID,
				 short   *tdc,
				 short   *adc);

extern void DrawShowerHits(Hv_View   View,
			   short     sect,
			   Hv_Region region);

extern Hv_Item MallocShower(Hv_View  View,
			    int      sect,
			    short    ID,
			    Hv_Item  parent);

extern void SetShowerGeometrySector(Hv_View  View);

extern short  PixelFromUVW(short u,
			   short v,
			   short w);

extern void  UVWFromPixel(short *u,
			  short *v,
			  short *w,
			  short pixel);


/*------ tagger.c --------*/

extern void  GetTaggerCoordinates(Hv_View   View,
				  Hv_Point  pp, 
				  short     *ec,
				  short     *tc);

extern void DrawTaggerHits(Hv_View   View,
			   Hv_Region region);

extern Hv_Item MallocTagger(Hv_View  View);

extern void GetTaggerHit(short   ecount,
			 short   tcount,
			 short   *etdc,
			 short   *ttdcl,
			 short   *ttdcr);

extern void GetTaggerERect(Hv_View  View,
			    int       id,
			    Hv_Rect  *rect);

extern void GetTaggerTRect(Hv_View View,
			    int      id,
			    Hv_Rect *rect);


extern void GetTaggerMainERect(Hv_View  View,
				    Hv_Rect *rect);

extern void GetTaggerMainTRect(Hv_View View,
				    Hv_Rect *rect);


/*------ tile.c -------*/

extern void           DrawTile(void);

/*------- track.c --------*/

extern int           GetHBTrackSector(int trackid);

extern int           GetTBTrackSector(int trackid);

extern int           GetNumHBTracks();

extern int           GetNumTBTracks();

extern void          DrawHBTrackBanks(Hv_View         View,
				    int             sect,
				    Hv_Region       cliprgn);
    
extern void          DrawTBTrackBanks(Hv_View         View,
				      int             sect,
				      Hv_Region       cliprgn);
    

extern void           GetHBTrackPoly(Hv_View      View,
				     int          trackid,
				     Hv_Point     **xp,
				     int          *np,
				     HBTRDataPtr  *hbtr);

extern void           GetTBTrackPoly(Hv_View      View,
				     int          trackid,
				     Hv_Point     **xp,
				     int          *np,
				     TBTRDataPtr  *tbtr);
    
#endif
E 1
