// test drawing panel
#include <stdio.h>
#include <stdlib.h>

#include <cgCircle.h>
#include <cgPolygon.h>
#include <cgLine.h>
#include <cgWinView.h>
#include <cgScene.h>
#include <cgRec.h>
#include <cgFrameRec.h>
#include <cgGauge.h>
#include <cgString.h>
#include <cgXpmImage.h>
#include <cgCircle.h>
#include <cgArc.h>
#include <cgPieArc.h>
#include <cgVmeCrate.h>
#include <cgComputer.h>
#include <cgTapeDrive.h>
#include <cgTrashCan.h>
#include <cg2DRtPlot.h>
#include <cgRotString.h>

#include <cgXdrawing.h>

#include <cgDispDev.h>

#include "RCLogo.xpm"
#include "rcDisc.xpm"

#ifdef _XWIN_24
 unsigned long pt[24] = {
   0xff000000,0x0000f000,0x0000f000,0x00fff000,0x00fff000,0xff00ff00,
   0xff000000,0x00000000,0x0000f000,0x0000f000,0x00fff000,0x00fff000,
   0xff000000,0xff00ff00,0x00000000,0x0000f000,0x0000f000,0x00fff000,
   0xff000000,0x00fff000,0xff00ff00,0x0000f000,0x0000f000,0x00fff000} ;
#else
   unsigned char pt[24] = {2,2,0,0,2,0,
			   2,2,0,0,2,0,
			   1,1,1,1,1,0,
			   3,3,0,0,0,0 } ;
#endif

static cg2DRtPlot* plot = 0;
static cgScene* s = 0;

void drawPanel (cgDispDev& disp)
{
  cgContext *cxt = new cgContext (disp);
  // set foreground red
  cxt->setForeground ("red");

  s = new cgScene (0.0, 40.0, 0.0, 50.0,
		   0.05, 0.95, 0.05, 0.95, disp);

  disp.registerScene (s);

/*  for (int i = 1; i < 10; i++) {
    cgLine line  (1.0, (i)*10.0, 90.0, (i)*10.0);
//    cgLine *line = new cgLine (1.0, (i+1)*9.0, 90.0, (i+1)*9.0);
    line.setCgCxt (*cxt);
//    line->attach (s);
    *s += line;
    line.draw (s);
  }
  
  cgPoint points[10];
  i = 0;
  points[i].setx (10.0);
  points[i++].sety (10.0);

  points[i].setx (10.0);
  points[i++].sety (20.0);

  points[i].setx (20.0);
  points[i++].sety (30.0);

  points[i].setx (40.0);
  points[i++].sety (60.0);

  points[i].setx (50.0);
  points[i++].sety (80.0);

  points[i].setx (120.0);
  points[i++].sety (50.0);

  points[i].setx (60.0);
  points[i++].sety (50.0);

  points[i].setx (40.0);
  points[i++].sety (30.0);

  points[i].setx (30.0);
  points[i++].sety (20.0);

  points[i].setx (20.0);
  points[i++].sety (20.0);

  cgPolygon* cc = new cgPolygon (points, 10);
  cc->setCgCxt (*cxt);
  cgPattern pat (4, 6, pt);

  //  cc.rot (3).hatch (s, 7);
  cc->attach (s);
  cc->draw (s);

  cgRec rec (5.0, 5.0, 100.0, 80.0);
  rec.createCgCxt (disp);
  cgContext* ncxt = rec.cgCxt ();
  ncxt->setForeground ("blue");
  *s += rec;
//  rec.draw (s);

  cgFrameRec frec (60.0, 60.0, 20.0, 20.0, 4);
  frec.createCgCxt (disp);
  *s += frec;
  frec.draw (s);

  cgGauge   *gauge = new cgGauge (10.0, 100.0, 10.0, 30.0);
  gauge->createCgCxt (disp);
  ncxt = gauge->cgCxt ();
//  ncxt->setForeground ("blue");
  gauge->attach (s);
  gauge->draw (s);
//  gauge->setValue (s, 30.0);

  cgString str ("Hello There", 
	"-adobe-helvetica-medium-*-*-*-*-180-*-*-*-*-*-*", 10.0, 40.0);
  str.createCgCxt (disp);
  *s += str;
  str.draw (s);

  cgXpmImage* image = new cgXpmImage ("runcontrol", 60, 10, rcDisc_xpm);
  image->createCgCxt (disp);
  image->init ();
  image->attach (s);
  image->draw (s); 

  cgPoint center (30.0, 30.0);
  cgCircle *circle = new cgCircle (center, 30.0);
  circle->createCgCxt (disp);
  circle->attach (s);
  circle->draw (s); 

  cgVmeCrate *crate = new cgVmeCrate (10.0, 20.0, 20.0, 15);
  crate->createCgCxt (disp);
  crate->setCrateColor ("white");
  crate->setCrateFgColor ("black");
  crate->attach (s);
  crate->draw (s); 

  cgComputer *comp = new cgComputer (20.0, 20.0, 30.0);
  comp->createCgCxt (disp);
  comp->setComputerColor ("white");
  comp->attach (s);
  comp->draw (s); 

  cgPieArc* arc0 = new cgPieArc (30.0, 30.0, 20.0, 0.0, 80.0);
  arc0->createCgCxt (disp);
  arc0->attach (s);
  arc0->solid (s);

  cgTapeDrive drive (30.0, 30.0, 20.0);
  drive.createCgCxt (disp);
  drive.setTapeDriveColor ("white");
  *s += drive;
  drive.draw (s); 


  cgTrashCan* can = new cgTrashCan (30.0, 30.0, 40.0);
  can->createCgCxt (disp);
  can->attach (s);
  can->setCanColor ("White");
  can->setCanFgColor ("black");
  can->open (s);
  can->draw (s); 

  plot = new cg2DRtPlot (0.0, 0.0, 50.0, 40, 20);
  plot->createCgCxt (disp);
  plot->attach (s);
  plot->foregroundColor ("red");
  plot->draw (s); */

  cgRotString* rstr = new cgRotString ("Hello There", 
	"-adobe-helvetica-medium-*-*-*-*-180-*-*-*-*-*-*", 10.0, 40.0,
				       45.0, 2.0);
  rstr->createCgCxt (disp);
  cgContext* ncxt = rstr->cgCxt ();
  ncxt->setForeground ("blue");
  rstr->attach (s);
  rstr->draw (s);
  
  
}

void updateValue (double value)
{
  plot->addPoint (value, s);
}

  
