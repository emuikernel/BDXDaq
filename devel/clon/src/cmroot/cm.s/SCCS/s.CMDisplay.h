h51645
s 00105/00000/00000
d D 1.1 06/10/19 15:41:11 boiarino 1 0
c date and time created 06/10/19 15:41:11 by boiarino
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
class CMInputDialog;

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

public:
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
  virtual void      FileSelect();
  virtual void      ChangeClient();
  virtual void      SetTitle(const char *datasrc);

  // input dialog methods
  virtual char     *OpenFileDialog();
  virtual char     *SaveFileDialog();
  virtual char     *GetStringDialog(const char *prompt, const char *defval);
  virtual Int_t     GetIntegerDialog(const char *prompt, Int_t defval);
  virtual Float_t   GetFloatDialog(const char *prompt, Float_t defval);


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
