# 1 "../Io.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../Io.st"
program Io ("P=7idd:Io:,MONO=7id:HHLM:energy_get,VSC=7idd:scaler1")
# 18 "../Io.st"
%%#include "epicsThread.h"
# 52 "../Io.st"
double Emono; assign Emono to "{MONO}"; monitor Emono; evflag Emono_mon; sync Emono Emono_mon;

short cnt; assign cnt to "{VSC}.CNT"; monitor cnt; evflag cnt_mon; sync cnt cnt_mon;
double clock_rate; assign clock_rate to "{VSC}.FREQ"; monitor clock_rate;
double ticks; assign ticks to "{VSC}.S1"; monitor ticks; evflag ticks_mon; sync ticks ticks_mon;
double s2; assign s2 to "{VSC}.S2"; monitor s2;
double s3; assign s3 to "{VSC}.S3"; monitor s3;
double s4; assign s4 to "{VSC}.S4"; monitor s4;
double s5; assign s5 to "{VSC}.S5"; monitor s5;
double s6; assign s6 to "{VSC}.S6"; monitor s6;
double s7; assign s7 to "{VSC}.S7"; monitor s7;
double s8; assign s8 to "{VSC}.S8"; monitor s8;
double s9; assign s9 to "{VSC}.S9"; monitor s9;
double s10; assign s10 to "{VSC}.S10"; monitor s10;
double s11; assign s11 to "{VSC}.S11"; monitor s11;
double s12; assign s12 to "{VSC}.S12"; monitor s12;
double s13; assign s13 to "{VSC}.S13"; monitor s13;
double s14; assign s14 to "{VSC}.S14"; monitor s14;
double s15; assign s15 to "{VSC}.S15"; monitor s15;

string nm2; assign nm2 to "{VSC}.NM2";
string nm3; assign nm3 to "{VSC}.NM3";
string nm4; assign nm4 to "{VSC}.NM4";
string nm5; assign nm5 to "{VSC}.NM5";
string nm6; assign nm6 to "{VSC}.NM6";
string nm7; assign nm7 to "{VSC}.NM7";
string nm8; assign nm8 to "{VSC}.NM8";
string nm9; assign nm9 to "{VSC}.NM9";
string nm10; assign nm10 to "{VSC}.NM10";
string nm11; assign nm11 to "{VSC}.NM11";
string nm12; assign nm12 to "{VSC}.NM12";
string nm13; assign nm13 to "{VSC}.NM13";
string nm14; assign nm14 to "{VSC}.NM14";
string nm15; assign nm15 to "{VSC}.NM15";

double flux; assign flux to "{P}flux";
double ionPhotons; assign ionPhotons to "{P}ionPhotons";
double ionAbs; assign ionAbs to "{P}ionAbs";
double detector; assign detector to "{P}detector";

short IoDebug; assign IoDebug to "{P}debug"; monitor IoDebug; evflag IoDebug_mon; sync IoDebug IoDebug_mon;
short icChannel; assign icChannel to "{P}scaler"; monitor icChannel; evflag icChannel_mon; sync icChannel icChannel_mon;
string icName; assign icName to "{P}scaler.DESC";
double E_using; assign E_using to "{P}E_using"; monitor E_using; evflag E_using_mon; sync E_using E_using_mon;
double VperA; assign VperA to "{P}VperA"; monitor VperA; evflag VperA_mon; sync VperA VperA_mon;
double v2f; assign v2f to "{P}v2f"; monitor v2f; evflag v2f_mon; sync v2f v2f_mon;
double xN2; assign xN2 to "{P}xN2"; monitor xN2; evflag xN2_mon; sync xN2 xN2_mon;
double xAr; assign xAr to "{P}xAr"; monitor xAr; evflag xAr_mon; sync xAr xAr_mon;
double xHe; assign xHe to "{P}xHe"; monitor xHe; evflag xHe_mon; sync xHe xHe_mon;
double xAir; assign xAir to "{P}xAir"; monitor xAir; evflag xAir_mon; sync xAir xAir_mon;
double xCO2; assign xCO2 to "{P}xCO2"; monitor xCO2; evflag xCO2_mon; sync xCO2 xCO2_mon;
double activeLen; assign activeLen to "{P}activeLen"; monitor activeLen; evflag activeLen_mon; sync activeLen activeLen_mon;
double deadFront; assign deadFront to "{P}deadFront"; monitor deadFront; evflag deadFront_mon; sync deadFront deadFront_mon;
double deadRear; assign deadRear to "{P}deadRear"; monitor deadRear; evflag deadRear_mon; sync deadRear deadRear_mon;
double kapton1; assign kapton1 to "{P}kapton1"; monitor kapton1; evflag kapton1_mon; sync kapton1 kapton1_mon;
double kapton2; assign kapton2 to "{P}kapton2"; monitor kapton2; evflag kapton2_mon; sync kapton2 kapton2_mon;
double HePath; assign HePath to "{P}HePath"; monitor HePath; evflag HePath_mon; sync HePath HePath_mon;
double airPath; assign airPath to "{P}airPath"; monitor airPath; evflag airPath_mon; sync airPath airPath_mon;
double Be; assign Be to "{P}Be"; monitor Be; evflag Be_mon; sync Be Be_mon;
double dEff; assign dEff to "{P}efficiency"; monitor dEff; evflag dEff_mon; sync dEff dEff_mon;
short ArPcntr; assign ArPcntr to "{P}ArPcntr"; monitor ArPcntr; evflag ArPcntr_mon; sync ArPcntr ArPcntr_mon;

char new_msg[256];
double updateRate;
char *SNLtaskName;
short Io_debug;

%% #include <stdlib.h>
%% #include <math.h>

%% static void EvalFlux(double aln, double dln1, double dln2, double dHe, double dAir, double dBe, double energy);
%% static double photon(double cps, double work, double VtoF, double Volt_per_A, double aln, double deadL, double Kapton, int gasId, double energy);
%% static double absorb(int id, double energy);
%% static double absH(double energy);
%% static double absHe(double energy);
%% static double absBe(double energy);
%% static double absC(double energy);
%% static double absN(double energy);
%% static double absO(double energy);
%% static double absAr(double energy);
%% static double absArPhoto(double energy);
# 145 "../Io.st"
ss ionChamber {

 state init {
  when () {
   SNLtaskName = macValueGet("name");
   Io_debug=0;
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 151, SNLtaskName, "init start"); epicsThreadSleep(0.1); };
   IoDebug=Io_debug;
   efClear(IoDebug_mon);
   updateRate = 10.0;
   pvGet(Emono);
   pvGet(VperA);
   pvGet(clock_rate);
   pvGet(ticks);
   pvGet(s2);
   pvGet(s3);
   pvGet(s4);
   pvGet(s5);
   pvGet(s6);
   pvGet(s7);
   pvGet(s8);
   pvGet(s9);
   pvGet(s10);
   pvGet(s11);
   pvGet(s12);
   pvGet(s13);
   pvGet(s14);
   pvGet(s15);

   {icChannel = (2); pvPut(icChannel);}
   {VperA = (100000000.); pvPut(VperA);}
   {v2f = (100000.); pvPut(v2f);}
   {xAir = (1.); pvPut(xAir);}
   {xN2 = (0.); pvPut(xN2);}
   {xAr = (0.); pvPut(xAr);}
   {xHe = (0.); pvPut(xHe);}
   {xCO2 = (0.); pvPut(xCO2);}
   {activeLen = (60.); pvPut(activeLen);}
   {deadFront = (17.5); pvPut(deadFront);}
   {deadRear = (17.5); pvPut(deadRear);}
   {kapton1 = (.001); pvPut(kapton1);}
   {kapton2 = (.001); pvPut(kapton2);}
   {HePath = (0.); pvPut(HePath);}
   {airPath = (0.); pvPut(airPath);}
   {Be = (0.); pvPut(Be);}
   {dEff = (1.); pvPut(dEff);}
   {ArPcntr = (0); pvPut(ArPcntr);}
   {flux = (0.); pvPut(flux);}
   {ionPhotons = (0.); pvPut(ionPhotons);}
   {ionAbs = (1.); pvPut(ionAbs);}
   {detector = (0.); pvPut(detector);}

   {E_using = (Emono); pvPut(E_using);};
   efClear(Emono_mon);
   efClear(E_using_mon);
   efClear(VperA_mon);
   efClear(cnt_mon);
   efClear(ticks_mon);
   efClear(icChannel_mon);
   efClear(v2f_mon);
   efClear(xN2_mon);
   efClear(xAr_mon);
   efClear(xHe_mon);
   efClear(xAir_mon);
   efClear(xCO2_mon);
   efClear(activeLen_mon);
   efClear(deadFront_mon);
   efClear(deadRear_mon);
   efClear(kapton1_mon);
   efClear(kapton2_mon);
   efClear(HePath_mon);
   efClear(airPath_mon);
   efClear(Be_mon);
   efClear(dEff_mon);
   efClear(ArPcntr_mon);

   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 221, SNLtaskName, "init complete"); epicsThreadSleep(0.1); };
  } state UpdateDisplay
 }


 state idle {
  when (efTestAndClear(IoDebug_mon)) {
   pvGet(IoDebug);
   Io_debug = IoDebug;
   efClear(IoDebug_mon);
   sprintf(new_msg, "changed debug flag to %d", Io_debug);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 232, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(E_using_mon)) {
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 236, SNLtaskName, "monochromator energy (pretend) changed"); epicsThreadSleep(0.1); };
   pvGet(E_using);
   efClear(E_using_mon);
  } state UpdateDisplay

  when (efTestAndClear(Emono_mon)) {
   if (Io_debug >= 2) { printf("<%s,%d,%s> %s\n", "../Io.st", 242, SNLtaskName, "monochromator (actual) changed"); epicsThreadSleep(0.1); };
   pvGet(Emono);
   {E_using = (Emono); pvPut(E_using);};
   efClear(E_using_mon);
   efClear(Emono_mon);
  } state UpdateDisplay

  when (efTestAndClear(VperA_mon)) {
   pvGet(VperA);
   efClear(VperA_mon);
   sprintf(new_msg, "changed, Current gain to =%g", VperA);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 253, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(ArPcntr_mon)) {
   pvGet(ArPcntr);
   efClear(ArPcntr_mon);
   if (ArPcntr) {
    dEff = 1. - exp( -(4*2.54) * absArPhoto(E_using) );
    dEff *= exp( -(.005*2.54) * absorb(5,E_using) );
    pvPut(dEff);
    efClear(dEff_mon);
   }
   sprintf(new_msg, "changed, ArPcntr=%d,  set dEff = %g", ArPcntr,dEff);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 266, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(dEff_mon)) {
   pvGet(dEff);
   {ArPcntr = (0); pvPut(ArPcntr);}
   efClear(ArPcntr_mon);
   efClear(dEff_mon);
   sprintf(new_msg, "changed, dEff=%g", dEff);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 275, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(icChannel_mon) || efTestAndClear(v2f_mon)) {
   pvGet(icChannel);
   pvGet(v2f);
   efClear(icChannel_mon);
   efClear(v2f_mon);
   sprintf(new_msg, "changed, icChannel=%d, v2f=%g", icChannel,v2f);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 284, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(xN2_mon) || efTestAndClear(xHe_mon) || efTestAndClear(xAr_mon) || efTestAndClear(xAir_mon) || efTestAndClear(xCO2_mon)) {
   pvGet(xN2);
   pvGet(xHe);
   pvGet(xAr);
   pvGet(xAir);
   pvGet(xCO2);
   efClear(xN2_mon);
   efClear(xHe_mon);
   efClear(xAr_mon);
   efClear(xAir_mon);
   efClear(xCO2_mon);
   sprintf(new_msg, "changed, xN2=%g, xAr=%g, xHe=%g, xAir=%g, xCO2=%g", xN2,xAr,xHe,xAir,xCO2);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 299, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(activeLen_mon) || efTestAndClear(deadFront_mon) || efTestAndClear(deadRear_mon)) {
   pvGet(activeLen);
   pvGet(deadFront);
   pvGet(deadRear);
   efClear(activeLen_mon);
   efClear(deadFront_mon);
   efClear(deadRear_mon);
   sprintf(new_msg, "changed, activeLen=%g, deadFront=%g, deadRear=%g", activeLen,deadFront,deadRear);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 310, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(HePath_mon) || efTestAndClear(airPath_mon)) {
   pvGet(HePath);
   pvGet(airPath);
   efClear(HePath_mon);
   efClear(airPath_mon);
   sprintf(new_msg, "changed, HePath=%g, airPath=%g", HePath,airPath);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 319, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(kapton1_mon) || efTestAndClear(kapton2_mon) || efTestAndClear(Be_mon)) {
   pvGet(kapton1);
   pvGet(kapton2);
   pvGet(Be);
   efClear(kapton1_mon);
   efClear(kapton2_mon);
   efClear(Be_mon);
   sprintf(new_msg, "changed, kapton1=%g, kapton2=%g, Be=%g", kapton1,kapton2,Be);
   if (Io_debug >= 1) { printf("<%s,%d,%s> %s\n", "../Io.st", 330, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (efTestAndClear(cnt_mon) || efTestAndClear(ticks_mon)) {
   efClear(cnt_mon);
   efClear(ticks_mon);
   sprintf(new_msg, "counter cycled");
   if (Io_debug >= 2) { printf("<%s,%d,%s> %s\n", "../Io.st", 337, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state UpdateDisplay

  when (delay(updateRate)) {
   if (Io_debug >= 3) { printf("<%s,%d,%s> %s\n", "../Io.st", 341, SNLtaskName, "periodic update"); epicsThreadSleep(0.1); };
  } state UpdateDisplay
 }


 state UpdateDisplay {
  when() {
   pvGet(E_using);
   efClear(E_using_mon);
   EvalFlux(activeLen,deadFront,deadRear,HePath,airPath,Be,E_using);
   if (icChannel==2) { pvGet(nm2); {sprintf(icName, nm2); pvPut(icName);} }
   if (icChannel==3) { pvGet(nm3); {sprintf(icName, nm3); pvPut(icName);} }
   if (icChannel==4) { pvGet(nm4); {sprintf(icName, nm4); pvPut(icName);} }
   if (icChannel==5) { pvGet(nm5); {sprintf(icName, nm5); pvPut(icName);} }
   if (icChannel==6) { pvGet(nm6); {sprintf(icName, nm6); pvPut(icName);} }
   if (icChannel==7) { pvGet(nm7); {sprintf(icName, nm7); pvPut(icName);} }
   if (icChannel==8) { pvGet(nm8); {sprintf(icName, nm8); pvPut(icName);} }
   if (icChannel==9) { pvGet(nm9); {sprintf(icName, nm9); pvPut(icName);} }
   if (icChannel==10) { pvGet(nm10); {sprintf(icName, nm10); pvPut(icName);} }
   if (icChannel==11) { pvGet(nm11); {sprintf(icName, nm11); pvPut(icName);} }
   if (icChannel==12) { pvGet(nm12); {sprintf(icName, nm12); pvPut(icName);} }
   if (icChannel==13) { pvGet(nm13); {sprintf(icName, nm13); pvPut(icName);} }
   if (icChannel==14) { pvGet(nm14); {sprintf(icName, nm14); pvPut(icName);} }
   if (icChannel==15) { pvGet(nm15); {sprintf(icName, nm15); pvPut(icName);} }
   pvPut(flux);
   pvPut(ionPhotons);
   pvPut(ionAbs);
   pvPut(detector);
   sprintf(new_msg, "   in Updatedisplay, flux = %g,   detector = %g", flux,detector);
   if (Io_debug >= 3) { printf("<%s,%d,%s> %s\n", "../Io.st", 370, SNLtaskName, new_msg); epicsThreadSleep(0.1); };
  } state idle
 }

}





%{


void EvalFlux(
double aln,
double dln1,
double dln2,
double dHe,
double dAir,
double dBe,
double energy)
{

 double AirAbs;
 double HeAbs;
 double KapAbs;
 double BeAbs;
 double fillAbsorb;
 double frontAbs;
 double cps;


 aln = aln / 10.;
 dln1 = dln1 / 10.;
 dln2 = dln2 / 10.;
 dHe = dHe / 10.;
 dAir = dAir / 10.;
 dBe = dBe * 2.54;

 switch (icChannel) {
 case 2: cps = s2; break;
 case 3: cps = s3; break;
 case 4: cps = s4; break;
 case 5: cps = s5; break;
 case 6: cps = s6; break;
 case 7: cps = s7; break;
 case 8: cps = s8; break;
 case 9: cps = s9; break;
 case 10: cps = s10; break;
 case 11: cps = s11; break;
 case 12: cps = s12; break;
 case 13: cps = s13; break;
 case 14: cps = s14; break;
 case 15: cps = s15; break;
 default:
  cps = 0;
 }
 cps /= (ticks/clock_rate);


 flux = 0.;
 flux += xHe * photon(cps,29.6, v2f,VperA,aln,dln1,kapton1,1,energy);
 flux += xN2 * photon(cps,36.3, v2f,VperA,aln,dln1,kapton1,2,energy);
 flux += xAr * photon(cps,24.4, v2f,VperA,aln,dln1,kapton1,3,energy);
 flux += xAir* photon(cps,35.36,v2f,VperA,aln,dln1,kapton1,4,energy);
 flux += xCO2* photon(cps,35.36,v2f,VperA,aln,dln1,kapton1,7,energy);


 AirAbs = exp(-dAir*absorb(4,energy));
 HeAbs = exp(-dHe * absorb(1,energy) );
 KapAbs = exp(-(kapton2+kapton1)*2.54 * absorb(6,energy) );
 BeAbs = exp(-dBe * absorb(5,energy) );
 fillAbsorb = absorb(1,energy)*xHe + absorb(2,energy)*xN2 + absorb(3,energy)*xAr + absorb(4,energy)*xAir + absorb(7,energy)*xCO2;

 ionAbs = exp(-(dln1+dln2+aln) * fillAbsorb);
 detector = flux * AirAbs * HeAbs * KapAbs * BeAbs * ionAbs * dEff;
 frontAbs = exp( -2.54*kapton1*absorb(6,energy) - dln1*fillAbsorb );
 ionPhotons = (1.- exp(-aln * fillAbsorb)) * frontAbs * flux;
}


double photon(
double cps,
double work,
double VtoF,
double Volt_per_A,
double aln,
double deadL,
double Kapton,
int gasId,
double energy)
{


 double convHe = 6.647;
 double rhoHe = 0.0001785;
 double convN = 23.26;
 double rhoN = 0.00125;
 double convAr = 66.32;
 double rhoAr = 0.001784;
 double edgeAr = 3.202;
 double sum,E1,E2,E3;
 double part1,part2,part3;
 double photo;

 static double aHe[4] = { 6.06488, -3.29055, -0.107256, 0.0144465 };
 static double aN[4] = { 11.2765, -2.65400, -0.200445, 0.0200765 };
 static double aAr1[4] = { 13.9491, -1.82276, -0.328827, 0.0274382 };
 static double aAr2[4] = { 12.2960, -2.63279, -0.073660, 0.0 };
 static double aO[4] = { 11.7130, -2.57229, -0.205893, 0.0199244 };
 static double aC[4] = { 10.6879, -2.71400, -0.200530, 0.0207248 };

 E1 = log(energy);
 E2 = E1 * E1;
 E3 = E2 * E1;
 photo = 0;


 if (gasId == 1) {
  sum = exp(aHe[0] + aHe[1]*E1 + aHe[2]*E2 + aHe[3]*E3);
  photo = sum * rhoHe / convHe;
 }
 else if (gasId == 2) {
  sum = exp(aN[0] + aN[1]*E1 + aN[2]*E2 + aN[3]*E3);
  photo = sum * rhoN / convN;
 }
 else if (gasId == 3) {
  if (E1 > edgeAr) sum = exp(aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
  else sum = exp(aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);
  photo = sum * rhoAr / convAr;
 }
 else if (gasId == 4) {
  sum = exp(aN[0] + aN[1]*E1 + aN[2]*E2 + aN[3]*E3);
  photo = sum * 0.000922 / convN;
  sum = exp(aO[0] + aO[1]*E1 + aO[2]*E2 + aO[3]*E3);
  photo += sum * 0.000266 / convN;
  if (E1 > edgeAr) sum = exp(aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
  else sum = exp(aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);
  photo += sum * 1.66E-5 / convAr;
 }
 else if (gasId == 7) {
  sum = exp(aC[0] + aC[1]*E1 + aC[2]*E2 + aC[3]*E3);
  photo = sum * 0.0005396 / convN;
  sum = exp(aO[0] + aO[1]*E1 + aO[2]*E2 + aO[3]*E3);
  photo += sum * 0.0014374 / convN;
 }


 part1 = cps * work / (1.602E-19 * VtoF * Volt_per_A * energy * 1000.);
 part2 = 1 - exp(-photo * aln);
 part3 = exp(absorb(gasId,energy) * deadL) * exp(absorb(6,energy) * Kapton * 2.54);

 return (part1 * part3 / part2);
}


double absorb(
int id,
double energy)
{
# 540 "../Io.st"
 double rhoH;
 double rhoHe;
 double rhoBe;
 double rhoC;
 double rhoN;
 double rhoO;
 double rhoAr;

 switch (id) {
 case 1:
  rhoHe = 0.0001785;
  return(absHe(energy)*rhoHe);
 case 5:
  rhoBe = 1.848;
  return(absBe(energy)*rhoBe);
 case 2:
  rhoN = 0.00125;
  return(absN(energy)*rhoN);
 case 4:
  rhoN = 0.000922;
  rhoO = 0.000266;
  rhoAr = 1.66E-5;
  return(absN(energy)*rhoN + absO(energy)*rhoO + absAr(energy)*rhoAr);
 case 3:
  rhoAr = 0.001784;
  return(absAr(energy)*rhoAr);
 case 6:
  rhoC = 0.981;
  rhoH = 0.037;
  rhoO = 0.297;
  rhoN = 0.105;
  return(absC(energy)*rhoC + absH(energy)*rhoH + absO(energy)*rhoO + absN(energy)*rhoN);
 case 7:
  rhoC = 0.0005396;
  rhoO = 0.0014374;
  return(absC(energy)*rhoC + absO(energy)*rhoO);
 }
 return 0;
}





double absH(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 2.44964E+00,-3.34953E+00,-4.71370E-02, 7.09962E-03 };
 static double b[4] = {-1.19075E-01,-9.37086E-01,-2.00538E-01, 1.06587E-02 };
 static double c[4] = {-2.15772E+00, 1.32685E+00,-3.05620E-01, 1.85025E-02 };


 conv = 1.674;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return( (photo+coherent+compton)/conv);
}





double absHe(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 6.06488E+00,-3.29055E+00,-1.07256E-01, 1.44465E-02 };
 static double b[4] = { 1.04768E+00,-8.51805E-02,-4.03527E-01, 2.69398E-02 };
 static double c[4] = {-2.56357E+00, 2.02536E+00,-4.48710E-01, 2.79691E-02 };


 conv = 6.647;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absBe(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 9.04511E+00,-2.83487E+00,-2.10021E-01, 2.29526E-02 };
 static double b[4] = { 2.00860E+00,-4.61920E-02,-3.37018E-01, 1.86939E-02 };
 static double c[4] = {-6.90079E-01, 9.46448E-01,-1.71142E-01, 6.51413E-03 };


 conv = 14.96;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absC(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.06879E+01,-2.71400E+00,-2.00530E-01, 2.07248E-02 };
 static double b[4] = { 3.10861E+00,-2.60580E-01,-2.71974E-01, 1.35181E-02 };
 static double c[4] = {-9.82878E-01, 1.46693E+00,-2.93743E-01, 1.56005E-02 };


 conv = 19.94;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absN(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.12765E+01,-2.65400E+00,-2.00445E-01, 2.00765E-02 };
 static double b[4] = { 3.47760E+00,-2.15762E-01,-2.88874E-01, 1.51312E-02 };
 static double c[4] = {-1.23693E+00, 1.74510E+00,-3.54660E-01, 1.98705E-02 };


 conv = 23.26;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absO(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 static double a[4] = { 1.17130E+01,-2.57229E+00,-2.05893E-01, 1.99244E-02 };
 static double b[4] = { 3.77239E+00,-1.48539E-01,-3.07124E-01, 1.67303E-02 };
 static double c[4] = {-1.73679E+00, 2.17686E+00,-4.49050E-01, 2.64733E-02 };


 conv = 26.57;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 photo = exp( a[0] + a[1]*E1 + a[2]*E2 + a[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}





double absAr(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double coherent;
 double compton;
 double edge;
 static double a1[4] = { 1.39491E+01,-1.82276E+00,-3.28827E-01, 2.74382E-02 };
 static double a2[4] = { 1.22960E+01,-2.63279E+00,-7.36600E-02, 0.};
 static double b[4] = { 5.21079E+00, 1.35618E-01,-3.47214E-01, 1.84333E-02 };
 static double c[4] = {-6.82105E-01, 1.74279E+00,-3.17646E-01, 1.56467E-02 };


 conv = 66.32;
 edge = 3.202;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 if (energy>edge) photo = exp( a1[0] + a1[1]*E1 + a1[2]*E2 + a1[3]*E3);
 else photo = exp( a2[0] + a2[1]*E1 + a2[2]*E2 + a2[3]*E3);
 coherent = exp( b[0] + b[1]*E1 + b[2]*E2 + b[3]*E3);
 compton = exp( c[0] + c[1]*E1 + c[2]*E2 + c[3]*E3);
 return((photo+coherent+compton)/conv);
}


double absArPhoto(
double energy)
{
 double conv;
 double E1,E2,E3;
 double photo;
 double rho;
 double edge;
 static double aAr1[4] = {13.9491, -1.82276, -0.328827, 0.0274382 };
 static double aAr2[4] = {12.2960, -2.63279, -0.073660, 0.0 };

 conv = 66.32;
 rho = 0.001784;
 edge = 3.202;

 E1 = log(energy);
 E2 = E1*E1;
 E3 = E2*E1;

 if (energy>edge) photo = exp( aAr1[0] + aAr1[1]*E1 + aAr1[2]*E2 + aAr1[3]*E3);
 else photo = exp( aAr2[0] + aAr2[1]*E1 + aAr2[2]*E2 + aAr2[3]*E3);

 return (photo / conv * rho);
}


}%
