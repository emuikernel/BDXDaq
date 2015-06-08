h20551
s 00011/00001/00424
d D 1.2 03/11/27 21:03:33 boiarino 3 1
c .
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMEpics.cxx
e
s 00425/00000/00000
d D 1.1 01/11/19 16:44:43 boiarino 1 0
c date and time created 01/11/19 16:44:43 by boiarino
e
u
U
f e 0
t
T
I 1

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMEpics                                                              //
//                                                                      //
// Epics for Moller Polarimeter                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include <TROOT.h>
#include <TText.h>
#include <TPad.h>
#include <TArc.h>

#include "CMEpics.h"

#include "CMDisplay.h" // to get gCMDisplay
extern CMDisplay *gCMDisplay;

ClassImp(CMEpics)

Int_t mUPDATE = 2000;

Float_t dxtr = 0.15; // from CMDisplay.cxx //  | could we get it
Float_t dytr = 0.45; // from CMDisplay.cxx //  |   from existing Pad ???
Float_t xt   = 0.4*dxtr;
Float_t xxt  = 0.5*dxtr;
Float_t yt   = 0.88*dytr;
Float_t dyt  = 0.12*dytr;
Float_t ddyt = 0.4*dyt;
Float_t xarc = 0.8*dxtr;
Float_t rarc = 0.15*dyt;
TText *t = new TText();

#define ABS(x) ((x) < 0 ? -(x) : (x))

//_____________________________________________________________________________
CMEpics::CMEpics(Option_t *fname) : TObject()
{
// Create a epics object.

I 3
   tmpfile = fname;

/* comment out graphics

E 3
   m_Pad = gCMDisplay->StatPad(); // Pad to draw everything
printf("epics: m_pad=0x%08x\n",m_Pad);
   gCMDisplay->SetEpics(this);    // register myself into gCMDisplay object
D 3
   tmpfile = fname;
E 3


   // Create status view pad

   m_Pad->Range(0,0,dxtr,dytr);
   m_Pad->Draw();
   m_Pad->cd();
   m_Pad->SetFillColor(22);
   m_Pad->SetBorderSize(2);

   t->SetTextFont(61);
   t->SetTextSize(0.14);
   t->SetTextAlign(22);
   t->DrawText(0.5*dxtr, 0.96*dytr,"NOM        MEAS");
   t->SetTextSize(0.1);
   t->SetTextAlign(22);


   t->DrawText(xt,yt,      "Left PMT   ");
   m_PMTL  = new TArc(xarc,yt,      rarc); m_PMTL->Draw();

   t->DrawText(xt,yt-dyt,  "Right PMT  ");
   m_PMTR  = new TArc(xarc,yt-dyt,  rarc); m_PMTR->Draw();

   t->DrawText(xt,yt-2*dyt,"Quad 1      ");
   m_QUAD1 = new TArc(xarc,yt-2*dyt,rarc); m_QUAD1->Draw();

   t->DrawText(xt,yt-3*dyt,"Quad 2      ");
   m_QUAD2 = new TArc(xarc,yt-3*dyt,rarc); m_QUAD2->Draw();

   t->DrawText(xt,yt-4*dyt,"Magnet      ");
   m_MAGN  = new TArc(xarc,yt-4*dyt,rarc); m_MAGN->Draw();

   t->DrawText(xt,yt-5*dyt,"Left Target ");
   m_TARGL = new TArc(xarc,yt-5*dyt,rarc); m_TARGL->Draw();

   t->DrawText(xt,yt-6*dyt,"Right Target");
   m_TARGR = new TArc(xarc,yt-6*dyt,rarc); m_TARGR->Draw();

   t->SetTextSize(0.12);
   t->DrawText(xt,yt-7*dyt,"Status -> ");
   t->SetTextSize(0.1);
   m_ALL   = new TArc(xarc,yt-7*dyt,rarc); m_ALL->Draw();


   AppendPad(); // append display object as last object to force selection

   // Set default values

   nev = 0;
   Int_t j;
   for(j=0; j<7; j++) tt[j] = (TText *)0;

   pmtl_nom = -1550.0;           //Nominal value for Left PMT (V)
   pmtl_dev = 5.0;

   pmtr_nom = -1410.0;           //Nominal value for Right PMT (V)
   pmtr_dev = 5.0;

   // set nominal quad currents (A)using beam energy 
   // E=2.45 GeV => quad1=164.1 A and quad2=1083 A

   //   GetQuadCurrents();
   //   fprintf(stderr,"quad1=%f\t quad2=%f\n",quad1_nom,quad2_nom);
   //   quad1_nom = 164.1;            Nominal value for upstream quad (A)
   quad1_dev = 50.0;

   //   quad2_nom = 1083.0;           Nominal value for dnstream quad (A)
   quad2_dev = 50.0;

   magn_nom = 4.0;
   targl_nom = 5.45;
   targr_nom = 0.60;
I 3

*/

E 3
}

//_____________________________________________________________________________
void CMEpics::DisplayStatus()
{
// Display current status parameters

I 3
/* comment out graphics

E 3
   m_Pad->cd();

   char *str = "                          ";
   Int_t j;
   //   fprintf(stderr,"called DisplayStatus\n \t quad 1 =%g (nom)=%g (meas)\n",quad1_nom,quad1_mes);
   pmtl_mes  = GetValue("B_hv_BM_MOLLER_L.MVI");
     pmtr_mes  = GetValue("B_hv_BM_MOLLER_R.MVI");
   for(j=0; j<7; j++) if(tt[j]) delete tt[j];

   sprintf(str,"%8.2f      %8.2f",pmtl_nom,pmtl_mes);
   tt[0] = new TText(xxt, yt-ddyt, str);

   sprintf(str,"%8.2f      %8.2f",pmtr_nom,pmtr_mes);
   tt[1] = new TText(xxt, yt-ddyt-dyt, str);

   sprintf(str,"%8.2f      %8.2f",quad1_nom,quad1_mes);
   tt[2] = new TText(xxt, yt-ddyt-2*dyt, str);

   sprintf(str,"%8.2f      %8.2f",quad2_nom,quad2_mes);
   tt[3] = new TText(xxt, yt-ddyt-3*dyt, str);

   sprintf(str,"%8.2f      %8.2f",magn_nom,magn_mes);
   tt[4] = new TText(xxt, yt-ddyt-4*dyt, str);

   sprintf(str,"%8.2f      %8.2f",targl_nom,targl_mes);
   tt[5] = new TText(xxt, yt-ddyt-5*dyt, str);

   sprintf(str,"%8.2f      %8.2f",targr_nom,targr_mes);
   tt[6] = new TText(xxt, yt-ddyt-6*dyt, str);

   for(j=0; j<7; j++)
   {
     tt[j]->SetTextSize(0.1);
     tt[j]->SetTextAlign(22);
     tt[j]->Draw();
   }

   Int_t good;
   good = 1;

   if(ABS(pmtl_mes - pmtl_nom) < pmtl_dev)
   {
     m_PMTL->SetFillColor(kGreen);
   }
   else
   {
     good = 0;
     m_PMTL->SetFillColor(kRed);
   }

   if(ABS(pmtr_mes - pmtr_nom) < pmtr_dev)
   {
     m_PMTR->SetFillColor(kGreen);
   }
   else
   {
     good = 0;
     m_PMTR->SetFillColor(kRed);
   }

   if(ABS(quad1_mes - quad1_nom) < quad1_dev)
   {
     m_QUAD1->SetFillColor(kGreen);
   }
   else
   {
     good = 0;
     m_QUAD1->SetFillColor(kRed);
   }

   if(ABS(quad2_mes - quad2_nom) < quad2_dev)
   {
     m_QUAD2->SetFillColor(kGreen);
   }
   else
   {
     good = 0;
     m_QUAD2->SetFillColor(kRed);
   }

   m_MAGN->SetFillColor(kYellow);
   m_TARGL->SetFillColor(kYellow);
   m_TARGR->SetFillColor(kYellow);

   if(good) m_ALL->SetFillColor(kGreen); else m_ALL->SetFillColor(kRed);

   m_Pad->Modified();
I 3
*/

E 3
}

//_____________________________________________________________________________
void CMEpics::Status()
{
// Check current status

   nev++;
   if(!(nev%mUPDATE))
   {
     pmtl_mes  = GetValue("B_hv_BM_MOLLER_L.MVI");
     pmtr_mes  = GetValue("B_hv_BM_MOLLER_R.MVI");
     quad1_mes = GetValue("hallb_sf_xy560_0_14");
     quad2_mes = GetValue("hallb_sf_xy560_0_18");
     magn_mes  = GetValue("hallb_sf_xy560_0_19"); /* -/+ 3.6-4.1 A */
     targl_mes = GetValue("moeller_target.RBV"); /* 5.45 */
     targr_mes = GetValue("moeller_target.RBV"); /* 0.6 */
     /* empty - 3.03 */

     DisplayStatus();
   }
}

//_____________________________________________________________________________
void CMEpics::Reset()
{
// Reset everything

  //Int_t j;
  //for(j=0; j<7; j++) if(tt[j]) {delete tt[j]; tt[j] = (TText *)0;}
}

//_____________________________________________________________________________
void CMEpics::SetValue(Option_t *name, Float_t value)
{
// Set one value

  // names for setting
  // 
  // pmtl_nom   B_hv_BM_MOLLER_L.DVO
  // pmtr_nom   B_hv_BM_MOLLER_R.DVO
  // quad1_nom  DYNABSETI
  // quad2_nom  DYNACSETI
  // magn_nom   kepco_command.VAL    -1  0  1
  // targl_nom  moller_target_command.VAL        left-11 out-12 right-13 calibrate-1
  // check moller target calibration moller_target_calib 0 => not calibrated 1=> calibrated

   char str[100], *strval = "                    ";

   //   strcpy(str,"put_epics ");
   // use remsh to set EPICS values on clas10
   strcpy(str,"rsh clas10.cebaf.gov /apps/epics/R3.12.2patch1/extensions/bin/hp700/caput ");
   strcat(str,name);
   sprintf(strval," %5.2f",value);
   strcat(str,strval);
   printf("%s<-\n",str);
   system(str);
}

//_____________________________________________________________________________
Float_t CMEpics::GetValue(Option_t *name)
{
// Get one value

   Float_t value;
   char str1[100], str2[100];
   FILE *fd;

   strcpy(str1,"rm -f ");
   strcat(str1,tmpfile);
   //printf("str1->%s<-\n",str1);

   strcpy(str2,"get_epics ");
   strcat(str2,name);
   strcat(str2," > ");
   strcat(str2,tmpfile);
   //printf("str2->%s<-\n",str2);

   system(str1);
   system(str2);
   if(fd = fopen(tmpfile, "r"))
   {
     fscanf(fd,"%f",&value);
     fclose(fd);
   }
   else
   {
     printf("CMEpics::GetValue(): can not open temporary file >%s<\n",tmpfile);
   }
   system(str1);

   printf("value=%f\n",value);  

   return(value);
}

/*  magfield.f
   magnetic field settings for Hall B moller polarimeter
     values ala chip
  lhk 3/98 
*/

void CMEpics::GetQuadCurrents()
{

  float q1m0, q1m1, q1m2, q1m3;  /* quad 1 terms           */
  float q2m0, q2m1, q2m2;        /* quad 2 terms           */
  float gvc1, gvc2, gvc3, gvc4, gvc5;
                                 /* grad to curr terms     */
  float qradius;                /* quad radius (in)       */
  float q1fld;                   /* quad 1 field (kG)      */
  float q1grad;                  /* quad 1 gradient (G/in) */
  Float_t q1curr;                  /* quad 1 current (A)     */
  float q2fld;                   /* quad 2 field (kG)      */
  float q2grad;                  /* quad 2 gradient (G/in) */
  Float_t q2curr;                  /* quad 2 current (A)     */
  float energy;                  /* beam energy (GeV)      */

  q1m0 = -0.50868102221;
  q1m1 = -0.99269187356;
  q1m2 =  0.68615481813;
  q1m3 = -0.037955948119;

  q2m0 =  0.10761692242;
  q2m1 =  1.5485463189;
  q2m2 =  0.028639510611;

  gvc1 =  1.000E-02;
  gvc2 =  4.436E-01;
  gvc3 =  3.224E-05;
  gvc4 = -3.202E-08;
  gvc5 =  9.931E-12;

  qradius = 5.0675;
  /*
    Results from gradient (Gauss/in) to current (A) fit (P(4))

    lambda :  1.000E+12 chi square (red):  1.667E-01

    Parameter  1 =  1.000E-02  error : 5.000E-05
    Parameter  2 =  4.436E-01  error : 7.860E-03
    Parameter  3 =  3.224E-05  error : 2.011E-05
    Parameter  4 = -3.202E-08  error : 1.501E-08
    Parameter  5 =  9.931E-12  error : 3.382E-12

    Covariance Matrix :
    2.500E-09  -1.016E-11   1.335E-14  -6.913E-18   1.222E-21
    -1.016E-11   6.178E-05  -1.538E-07   1.096E-10  -2.351E-14
    1.335E-14  -1.538E-07   4.045E-10  -2.980E-13   6.534E-17
    -6.913E-18   1.096E-10  -2.980E-13   2.253E-16  -5.037E-20
    1.222E-21  -2.351E-14   6.534E-17  -5.037E-20   1.144E-23


  */

  energy=GetValue("MBSY2C_energy")/1000;
  fprintf(stderr,"energy=%f\n",energy);
  if(energy<0.7)
    {
      printf(" \n\n ** Could not read Beam energy from EPICS! \n\n ");
      printf(" Enter Beam Energy: ");
      scanf("%f", &energy);
      fprintf(stderr,"\n Problem reading energy via EPICS \n \n \t ** setting quads to nominal values consistent with 2.45 GeV\n\n\n");
      energy=2.45; // set default energy to 2.45 GeV
    }
  q1fld = q1m0 + (q1m1 * energy) + (q1m2 * energy * energy)
    + (q1m3 * energy * energy * energy);
  q2fld = q2m0 + (q2m1 * energy) + (q2m2 * energy * energy);

  q1grad = q1fld * 1000. / qradius;
  q2grad = q2fld * 1000. / qradius;

  q1curr =   gvc1 
           + gvc2 * q1grad 
           + gvc3 * q1grad * q1grad
           + gvc4 * q1grad * q1grad * q1grad 
           + gvc5 * q1grad * q1grad * q1grad * q1grad;
  q2curr =   gvc1 
           + gvc2 * q2grad 
           + gvc3 * q2grad * q2grad
           + gvc4 * q2grad * q2grad * q2grad 
           + gvc5 * q2grad * q2grad * q2grad * q2fld;

  q1curr = q1curr * 3.;    /* quads in parallel */
  q2curr = q2curr * 3.;

  /*
  printf("            Field (kG)   Gradient (G/in)   Current (A)\n");
 
  printf("   Quad 1   %f       %8.3f        %8.3f \n", q1fld, q1grad, q1curr);
  printf("   Quad 2   %f       %8.3f        %8.3f \n", q2fld, q2grad, q2curr);
  fprintf(stderr,"Subroutine QuadSet => quad1=%g\tquad2=%g\n",quad1_nom,quad2_nom);
  fprintf(stderr,"Subroutine QuadSet => quad1=%g\tquad2=%g\n",q1curr,q2curr);
  */
  if(energy) 
    {
      fprintf(stderr,"Subroutine QuadSet => quad1=%g\tquad2=%g\n",q1curr,q2curr);
      quad1_nom=q1curr;
      quad2_nom=q2curr;
      if(quad1_nom < 0)
	quad1_nom=quad1_nom*(-1.0);	
      SetValue("DYNABSETI",quad1_nom);
      SetValue("DYNACSETI",quad2_nom);
      
    }
}




E 1
