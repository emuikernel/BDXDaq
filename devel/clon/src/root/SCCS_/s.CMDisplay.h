h07576
s 00012/00001/00094
d D 1.2 03/04/17 17:00:53 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMDisplay.h
e
s 00095/00000/00000
d D 1.1 01/11/19 16:44:43 boiarino 1 0
c date and time created 01/11/19 16:44:43 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef CMDisplay_H
#define CMDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMDisplay                                                            //
//                                                                      //
// Utility class to display Moller Polarimeter GUI                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef CMVirtualDisplay_H
#include "CMVirtualDisplay.h"
#endif

class TDiamond;
class TCanvas;
class TPad;
class TArc;
class TTUBE;
class TBRIK;
class TNode;
class TPolyLine3D;
class TList;
class CMDaq;
class CMEpics;
I 3
class CMInputDialog;
E 3

class CMDisplay : public CMVirtualDisplay {

private:

  char              dtext[10];
  TDiamond         *diamond;

  Bool_t            m_DrawAllViews;         //Flag True if AllViews selected
  Bool_t            m_DrawParticles;        //Flag True if particles to be drawn
  Bool_t            m_DrawClusters;         //Flag True if clusters to be drawn
  Float_t           m_PTcut;                //PT cut to display objects
  Float_t           m_PTcutEGMUNU;          //PT cut for Electrons, Gammas, MUons, Neutrinos
  Float_t           m_Rin;                  //Inner ATLAS radius
  Float_t           m_Rout;                 //Outer ATLAS radius
  Float_t           m_Zin;                  //Inner ATLAS length along Z
  Float_t           m_Zout;                 //Outer ATLAS length along Z
  Float_t           m_Theta;                //Viewing angle theta
  Float_t           m_Phi;                  //Viewing angle phi
  TCanvas          *m_Canvas;               //Pointer to the display canvas
  TPad             *m_StatPad;              //Pointer to the status pad 
  TPad             *m_Buttons;              //Pointer to the buttons pad
  TPad             *m_Pad;                  //Pointer to the event display main pad
  TBRIK            *m_Brik;                 //Main shape
  TTUBE            *m_Tubin;                //Inner tube
  TTUBE            *m_Tubout;               //outer tube
  TNode            *m_Nodin;                //Node for detector outline
  TNode            *m_Nodsda;               //Node for SDA outline
  TList            *m_Fruits;               //List for fruits
  CMDaq            *m_Daq;                  //Pointer to Daq
  CMEpics          *m_Epics;                //Pointer to Epics
  CMVirtualDisplay *m_Display;              //Pointer to Event display object
D 3
   
E 3
I 3

E 3
public:
                    CMDisplay();
                    CMDisplay(const char *title);
  virtual          ~CMDisplay();
  CMVirtualDisplay *Display() {return m_Display;}
  virtual void      Clear(Option_t *option="");
  virtual void      DisplayButtons();
  virtual void      VoltageOn();
  virtual void      VoltageOff();
  virtual void      MoveTarget(int pos);
  virtual void      Download();
  virtual void      Prestart();
  virtual void      Pause();
  virtual void      Resume();
  virtual void      StartLoop(Long_t timeout);
  virtual void      StopLoop();
  virtual void      Event();
I 3
  virtual void      FileSelect();
E 3
  virtual void      ChangeClient();
I 3
  virtual void      SetTitle(const char *datasrc);
E 3

I 3
  // input dialog methods
  virtual char     *OpenFileDialog();
  virtual char     *SaveFileDialog();
  virtual char     *GetStringDialog(const char *prompt, const char *defval);
  virtual Int_t     GetIntegerDialog(const char *prompt, Int_t defval);
  virtual Float_t   GetFloatDialog(const char *prompt, Float_t defval);


E 3
  virtual Bool_t    AllViews() {return m_DrawAllViews;}
  Bool_t            DrawClusters() {return m_DrawClusters;}
  Bool_t            DrawParticles() {return m_DrawParticles;}
  TNode            *Nodin() {return m_Nodin;}
  TTUBE            *Tubin() {return m_Tubin;}
  TNode            *Nodsda() {printf("!!!\n"); return m_Nodsda;}
  TBRIK            *Brik() {return m_Brik;}
  TPad             *Pad() {return m_Pad;}
  TPad             *StatPad() {return m_StatPad;}
  virtual void      SetDaq(CMDaq *Daq=(CMDaq *)0) {m_Daq = (CMDaq *)Daq;}
  virtual void      SetEpics(CMEpics *Epics=(CMEpics *)0) {m_Epics = (CMEpics *)Epics;}

  ClassDef(CMDisplay, 0)   //Utility class to display Moller Polarimeter GUI
};


#endif
E 1
