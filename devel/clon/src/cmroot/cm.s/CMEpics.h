#ifndef CMEpics_H
#define CMEpics_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMEpics                                                              //
//                                                                      //
// Epics for Moller Polarimeter                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TPad;
class TArc;
class CMDisplay;

class CMEpics : public TObject {

private:
   Int_t             nev;                //Event number
   TPad             *m_Pad;              //Pad to draw status
   TObject          *m_Epics;            //Pointer to ...
   Option_t         *tmpfile;            //Temporary file name
   TArc             *m_PMTL;             //Pointer to arc showing ON/OFF status Left PMT
   TArc             *m_PMTR;             //Pointer to arc showing ON/OFF status Right PMT
   TArc             *m_QUAD1;            //Pointer to arc showing ON/OFF status Quad 1
   TArc             *m_QUAD2;            //Pointer to arc showing ON/OFF status Quad 2
   TArc             *m_MAGN;             //Pointer to arc showing ON/OFF status Magnet
   TArc             *m_TARGL;            //Pointer to arc showing ON/OFF status Left Target
   TArc             *m_TARGR;            //Pointer to arc showing ON/OFF status Right Target
   TArc             *m_ALL;              //Pointer to arc showing ON/OFF status ALL
   TText            *tt[7];

   Float_t           pmtl_nom;           //Nominal value for Left PMT
   Float_t           pmtl_mes;           //Measured value for Left PMT
   Float_t           pmtl_dev;           //

   Float_t           pmtr_nom;           //Nominal value for Right PMT
   Float_t           pmtr_mes;           //Measured value for Right PMT
   Float_t           pmtr_dev;           //

   Float_t           beam_energy;          // beam energy

   Float_t           quad1_nom;          //
   Float_t           quad1_mes;          //
   Float_t           quad1_dev;          //

   Float_t           quad2_nom;          //
   Float_t           quad2_mes;          //
   Float_t           quad2_dev;          //

   Float_t           magn_nom;           //
   Float_t           magn_mes;           //
   Float_t           magn_dev;           //

   Float_t           targl_nom;          //
   Float_t           targl_mes;          //
   Float_t           targl_dev;          //

   Float_t           targr_nom;          //
   Float_t           targr_mes;          //
   Float_t           targr_dev;          //

public:
                     CMEpics(Option_t *option="zzz_epics_tmp.tmp");
   virtual          ~CMEpics() {;}
   virtual void      DisplayStatus();
   virtual void      SetValue(Option_t *option="", Float_t value=0.0);
   virtual Float_t   GetValue(Option_t *option="");
   virtual void      Status();
   virtual void      GetQuadCurrents();
   virtual void      Reset();

   TObject          *Epics() {return m_Epics;}

   ClassDef(CMEpics, 0)   //Epics for Moller Polarimeter
};

#endif
