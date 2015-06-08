h28658
s 00062/00001/01174
d D 1.17 08/11/15 09:37:17 puneetk 17 16
c created ecp_tof1_ec1.lut and ecp_tof2_ec1.lut
c 
e
s 00014/00000/01161
d D 1.16 08/11/14 17:39:06 puneetk 16 15
c created ecp_tof2.lut
c 
e
s 00017/00000/01144
d D 1.15 08/11/12 16:45:19 puneetk 15 14
c created trig_ic1_ecc lut
c 
e
s 00061/00000/01083
d D 1.14 08/11/12 09:29:39 puneetk 14 13
c created trig_ecc_ecp, trig_ecc_ece, trig_ecc_ecp_ece and trig_ic1_ecc_ecp luts
c 
e
s 00016/00000/01067
d D 1.13 08/11/05 15:27:47 puneetk 13 12
c created a new table trig_icv1.lut
c 
e
s 00007/00007/01060
d D 1.12 08/11/05 15:07:36 puneetk 12 11
c changed ecc1 to ecc_ecc1.lut
c 
e
s 00015/00003/01052
d D 1.11 08/11/05 14:55:22 puneetk 11 10
c created new table for trig ece and ecp
c 
e
s 00004/00000/01051
d D 1.10 08/11/05 14:23:34 puneetk 10 9
c updated for ecx_ec3 luts
c 
e
s 00007/00007/01044
d D 1.9 08/11/05 13:42:43 puneetk 9 8
c fixed bug with ectof2_ec1
c 
e
s 00002/00002/01049
d D 1.8 08/10/26 18:27:21 puneetk 8 7
c changed name of ECP_TOFTrig to ECP_TOF1Trig
c 
e
s 00080/00000/00971
d D 1.7 08/10/24 10:50:59 puneetk 7 6
c added ecp x tof single bit luts for 6 sectors
c 
e
s 00010/00000/00961
d D 1.6 08/10/23 11:19:37 boiarino 6 5
c add ecp_tof
c 
e
s 00083/00000/00878
d D 1.5 08/10/23 08:54:39 puneetk 5 4
c *** empty log message ***
e
s 00057/00001/00821
d D 1.4 08/10/22 14:24:55 puneetk 4 3
c added trig_ic1, trig_ic3, trig_ic4, and trig_ic5 luts
c 
e
s 00049/00000/00773
d D 1.3 08/10/22 11:59:19 puneetk 3 2
c added ecx_ectof2_ec1.lut
c 
e
s 00056/00000/00717
d D 1.2 08/10/22 11:20:18 boiarino 2 1
c *** empty log message ***
e
s 00717/00000/00000
d D 1.1 08/10/20 09:30:06 boiarino 1 0
c date and time created 08/10/20 09:30:06 by boiarino
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

using namespace std;

// SecNum Ranges from 1 to 6, indicating the sector number

/////////////////////////////////////////////
// HallBFlexTriggerIC LUT Address Bit Maps //
/////////////////////////////////////////////

// Use for ECP & ECP LUT
#define GET_TOF(SecNum)		(Address & (1<<(6+SecNum-1)))

// Use for ECP LUT
#define GET_ECP(SecNum)		(Address & (1<<(SecNum-1)))

// Use for ECE LUT
#define GET_ECE(SecNum)		(Address & (1<<(SecNum-1)))

// Use for ECC LUT
#define GET_ECC(SecNum)		(Address & (1<<(SecNum-1)))

// Use for TRIG LUT
#define GET_ICHODO_CNT		(Address & 0x7)
#define GET_ICTOTAL_CNT		((Address >> 3) & 0x7)
#define GET_ICVETO_CNT		((Address >> 6) & 0x7)
#define GET_TRIG_ECP		(Address & 0x200)
#define GET_TRIG_ECE		(Address & 0x400)
#define GET_TRIG_ECC		(Address & 0x800)

// level 3 ////////////////////////////////////////////////////////////////////

int Multiplicity(int n1, int n2, int n3, int n4, int n5, int n6) {
  int m = 0;

  if (n1) m++;
  if (n2) m++;
  if (n3) m++;
  if (n4) m++;
  if (n5) m++;
  if (n6) m++;

  return m;
}
I 17

int TOFMultiplicity(int Address) {
  return Multiplicity(GET_TOF(1), GET_TOF(2), GET_TOF(3), GET_TOF(4), GET_TOF(5), GET_TOF(6));
}

int ECPMultiplicity(int Address) {
  return Multiplicity(GET_ECP(1), GET_ECP(2), GET_ECP(3), GET_ECP(4), GET_ECP(5), GET_ECP(6));
}

// precondition: the address is viable and ECP and TOF have multiplicity 1
// postcondition: the sector coincidence is checked between TOF and ECP
int IsTOFECPSameSector(int Address) {
  if ((GET_ECP(1) && GET_TOF(1)) || (GET_ECP(2) && GET_TOF(2)) ||
      (GET_ECP(3) && GET_TOF(3)) || (GET_ECP(4) && GET_TOF(4)) ||
      (GET_ECP(5) && GET_TOF(5)) || (GET_ECP(6) && GET_TOF(6))) {
    return 1;
  } 
  return 0;
}

E 17
/*
int ECC_TestTrig(int Address) {
  // Build a trigger when 2 sector ECC signals are true
  if (Multiplicity(GET_ECC(1), GET_ECC(2), GET_ECC(3), GET_ECC(4), GET_ECC(5), 
      GET_ECC(6)) == 2)
    return 1;

  return 0;
}

int ECE_TestTrig(int Address) {
  // Build a trigger when 2 sectors (ECE and TOF) signals are true
  if (Multiplicity(GET_ECE(1) && GET_TOF(1), GET_ECE(2) && GET_TOF(2), 
                   GET_ECE(3) && GET_TOF(3), GET_ECE(4) && GET_TOF(4), 
                   GET_ECE(5) && GET_TOF(5), GET_ECE(6) && GET_TOF(6)) == 2)
    return 1;

  return 0;
}

int ECP_TestTrig(int Address) {
  // Build a trigger when 2 sectors (ECP and TOF) signals are true
  if (Multiplicity(GET_ECE(1) && GET_TOF(1), GET_ECE(2) && GET_TOF(2), 
                   GET_ECE(3) && GET_TOF(3), GET_ECE(4) && GET_TOF(4), 
                   GET_ECE(5) && GET_TOF(5), GET_ECE(6) && GET_TOF(6)) == 2)
    return 1;

  // or Build a trigger when 3 sectors TOF signals are true
  if (Multiplicity(GET_TOF(1), GET_TOF(2), GET_TOF(3), GET_TOF(4), GET_TOF(5), 
                   GET_TOF(6)) == 3)
    return 1;

  return 0;
}

int TRIG_TestTrig(int Address) {
  // Build a trigger when there are 2 or more total clusters in the IC and 
  // when ECP, ECE, or ECE LUT is true
  if ((GET_ICTOTAL_CNT >= 2) && (GET_TRIG_ECP || GET_TRIG_ECE || GET_TRIG_ECC))
    return 1;

  return 0;
}
*/

// level 2 ///////////////////////////////////////////////////////////////////

//////////// ones and zeroes ///////////////////////////////////

// precondition: the address is a viable one
// postcondition: the address is used to obtain the value for current trigger
int OneTrig(int Address) {
  return 1;
}

// precondition: the address is a viable one
// postcondition: the address is used to obtain the value for current trigger
int ZeroTrig(int Address) {
  return 0;
}

I 2

E 2
/////////// ECC >= 1 /////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the ecc multiplicity of greater than or equal to 1 is 
//    checked and its result is returned
int ECCMult1Trig(int Address) {
  // if at least one ECC sector is triggered then return 1
  if (Multiplicity(GET_ECC(1), GET_ECC(2), GET_ECC(3), GET_ECC(4), GET_ECC(5),
		   GET_ECC(6)) >= 1) {
    return 1;
  }
  return 0;
}

I 2

E 2
////////// EC >= 1 ///////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE is checked for >= 1
int ECE_EC1Trig(int Address) {
  // if at least one ECE sector is triggered then return 1
  if (Multiplicity(GET_ECE(1), GET_ECE(2), GET_ECE(3), GET_ECE(4), GET_ECE(5),
		   GET_ECE(6)) >= 1) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP is checked for >= 1
int ECP_EC1Trig(int Address) {
  // if at least one ECP sector is triggered then return 1
  if (Multiplicity(GET_ECP(1), GET_ECP(2), GET_ECP(3), GET_ECP(4), GET_ECP(5),
		   GET_ECP(6)) >= 1) {
    return 1;
  }
  return 0;
}

/////////// EC >= 2 //////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE is checked for >= 2
int ECE_EC2Trig(int Address) {
  // if at least two ECE sectors are triggered then return 1
  if (Multiplicity(GET_ECE(1), GET_ECE(2), GET_ECE(3), GET_ECE(4), GET_ECE(5),
		   GET_ECE(6)) >= 2) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP is checked for >= 2
int ECP_EC2Trig(int Address) {
  // if at least two ECP sectors are triggered then return 1
  if (Multiplicity(GET_ECP(1), GET_ECP(2), GET_ECP(3), GET_ECP(4), GET_ECP(5),
		   GET_ECP(6)) >= 2) {
    return 1;
  }
  return 0;
}

////////////// EC >= 3 ///////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE is checked for >= 3
int ECE_EC3Trig(int Address) {
  // if at least three ECE sectors are triggered then return 1
  if (Multiplicity(GET_ECE(1), GET_ECE(2), GET_ECE(3), GET_ECE(4), GET_ECE(5),
		   GET_ECE(6)) >= 3) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP is checked for >= 3
int ECP_EC3Trig(int Address) {
  // if at least three ECP sectors are triggered then return 1
  if (Multiplicity(GET_ECP(1), GET_ECP(2), GET_ECP(3), GET_ECP(4), GET_ECP(5),
		   GET_ECP(6)) >= 3) {
    return 1;
  }
  return 0;
}

/////////////// TOF >= 3 /////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of TOF is checked for >= 3
int ECE_TOF3Trig(int Address) {
  // trigger at least 3 sectors in TOF
  if (Multiplicity(GET_TOF(1), GET_TOF(2), GET_TOF(3), GET_TOF(4), GET_TOF(5),
                   GET_TOF(6)) >= 3) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of TOF is checked for >= 3
int ECP_TOF3Trig(int Address) {
  // trigger at least 3 sectors in TOF
  if (Multiplicity(GET_TOF(1), GET_TOF(2), GET_TOF(3), GET_TOF(4), GET_TOF(5),
                   GET_TOF(6)) >= 3) {
    return 1;
  }
  return 0;
}

I 17
///////////// TOF >= 2 ////////////////////////////////////////////////////////

E 17
I 16
// precondition: the address is viable
// postcondition: the multiplicity of TOF is checked for >= 2
int ECP_TOF2(int Address) {
  // trigger at least 2 sectors in TOF
  if (Multiplicity(GET_TOF(1), GET_TOF(2), GET_TOF(3), GET_TOF(4), GET_TOF(5),
                   GET_TOF(6)) >= 2) {
    return 1;
  }
  return 0;
}
E 16
I 2

I 17
////////////// TOF >= 1 x ECP >= 1 (different sectors) ////////////////////////
E 17

I 17
// precondition: the address is viable
// postcondition: the TOF >= 1 and ECP >= 1 for different sectors
int ECP_TOF1_EC1(int Address) {
  // if we have only one sector hit in TOF and ECP and the sectors are not same
  // or if we have total sector hits of more than 2 for both TOF and ECP and 
  // at least one sector in TOF and at least one sector in ECP is hit
  if (((TOFMultiplicity(Address) == 1) && (ECPMultiplicity(Address) == 1) && 
      (IsTOFECPSameSector(Address) == 0)) || 
      ((TOFMultiplicity(Address) + ECPMultiplicity(Address) > 2) && 
       (TOFMultiplicity(Address) > 0) && (ECPMultiplicity(Address) > 0))) {
    return 1;
  }
  return 0;
}

////////////// TOF >= 2 x ECP >= 1 (different sectors) ////////////////////////

// precondition: the address is viable
// postcondition: the TOF >= 2 and ECP >= 1 for different sectors
int ECP_TOF2_EC1(int Address) {
  // if we have only two sector hit in TOF and one in ECP and the sectors are 
  // not same
  // or if we have total sector hits of more than 3 for both TOF and ECP and 
  // at least two sectors in TOF and at least one sector in ECP is hit
  if (((TOFMultiplicity(Address) == 2) && (ECPMultiplicity(Address) == 1) && 
      (IsTOFECPSameSector(Address) == 0)) || 
      ((TOFMultiplicity(Address) + ECPMultiplicity(Address) > 3) && 
       (TOFMultiplicity(Address) > 1) && (ECPMultiplicity(Address) > 0))) {
    return 1;
  }
  return 0;
}

E 17
I 6
/////////////// TOF ANY Sector /////////////////////////////////////////////
D 8
int ECP_TOFTrig(int Address) {
E 8
I 8
int ECP_TOF1Trig(int Address) {
E 8
  // trigger any sector in TOF
  if( GET_TOF(1) || GET_TOF(2) || GET_TOF(3) || GET_TOF(4) || GET_TOF(5) || GET_TOF(6) ) {
    return 1;
  }
  return 0;
}

E 6
/////////////// TOF 2 Sector 60 degrees /////////////////////////////////////////////
int ECP_TOF60Trig(int Address) {
  // trigger 2 60deg sectors in TOF
  if( (GET_TOF(1) && GET_TOF(3)) || (GET_TOF(1) && GET_TOF(5)) || (GET_TOF(3) && GET_TOF(5)) ||
      (GET_TOF(2) && GET_TOF(4)) || (GET_TOF(2) && GET_TOF(6)) || (GET_TOF(4) && GET_TOF(6)) ) {
    return 1;
  }
  return 0;
}

/////////////// TOF 2 opposite Sectors /////////////////////////////////////////////
int ECP_TOF180Trig(int Address) {
  // trigger 2 opposite sectors in TOF
  if( (GET_TOF(1) && GET_TOF(4)) || (GET_TOF(2) && GET_TOF(5)) || (GET_TOF(3) && GET_TOF(6)) ) {
    return 1;
  }
  return 0;
}



E 2
/////////////// EC x TOF >= 1 ////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 1
int ECE_ECTOF1Trig(int Address) {
  // if at least one sector with ECE and TOF is triggered
  if (Multiplicity(GET_ECE(1) && GET_TOF(1), GET_ECE(2) && GET_TOF(2),
		   GET_ECE(3) && GET_TOF(3), GET_ECE(4) && GET_TOF(4),
		   GET_ECE(5) && GET_TOF(5), GET_ECE(6) && GET_TOF(6)) >= 1) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 1
int ECP_ECTOF1Trig(int Address) {
  // if at least one sector with ECP and TOF is triggered
  if (Multiplicity(GET_ECP(1) && GET_TOF(1), GET_ECP(2) && GET_TOF(2),
		   GET_ECP(3) && GET_TOF(3), GET_ECP(4) && GET_TOF(4),
		   GET_ECP(5) && GET_TOF(5), GET_ECP(6) && GET_TOF(6)) >= 1) {
    return 1;
  }
  return 0;
}

///////////////// EC x TOF >= 2 /////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 2
int ECE_ECTOF2Trig(int Address) {
  // if at least two sectors with ECE and TOF is triggered
  if (Multiplicity(GET_ECE(1) && GET_TOF(1), GET_ECE(2) && GET_TOF(2),
		   GET_ECE(3) && GET_TOF(3), GET_ECE(4) && GET_TOF(4),
		   GET_ECE(5) && GET_TOF(5), GET_ECE(6) && GET_TOF(6)) >= 2) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 2
int ECP_ECTOF2Trig(int Address) {
  // if at least two sectors with ECP and TOF is triggered
  if (Multiplicity(GET_ECP(1) && GET_TOF(1), GET_ECP(2) && GET_TOF(2),
		   GET_ECP(3) && GET_TOF(3), GET_ECP(4) && GET_TOF(4),
		   GET_ECP(5) && GET_TOF(5), GET_ECP(6) && GET_TOF(6)) >= 2) {
    return 1;
  }
  return 0;
}

/////////////////// EC x TOF >= 3 /////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 3
int ECE_ECTOF3Trig(int Address) {
  // if at least three sectors with ECE and TOF is triggered
  if (Multiplicity(GET_ECE(1) && GET_TOF(1), GET_ECE(2) && GET_TOF(2),
		   GET_ECE(3) && GET_TOF(3), GET_ECE(4) && GET_TOF(4),
		   GET_ECE(5) && GET_TOF(5), GET_ECE(6) && GET_TOF(6)) >= 3) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 3
int ECP_ECTOF3Trig(int Address) {
  // if at least three sectors with ECP and TOF is triggered
  if (Multiplicity(GET_ECP(1) && GET_TOF(1), GET_ECP(2) && GET_TOF(2),
		   GET_ECP(3) && GET_TOF(3), GET_ECP(4) && GET_TOF(4),
		   GET_ECP(5) && GET_TOF(5), GET_ECP(6) && GET_TOF(6)) >= 3) {
    return 1;
  }
  return 0;
}

////////////////// EC x !TOF >= 1 ////////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x !TOF is checked for >= 1
int ECE_ECNoTOF1Trig(int Address) {
  // if at least one sectors with ECE and !TOF is triggered
  if (Multiplicity(GET_ECE(1) && !GET_TOF(1), GET_ECE(2) && !GET_TOF(2),
		   GET_ECE(3) && !GET_TOF(3), GET_ECE(4) && !GET_TOF(4),
		   GET_ECE(5) && !GET_TOF(5), GET_ECE(6) && !GET_TOF(6)) >= 1){
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x !TOF is checked for >= 1
int ECP_ECNoTOF1Trig(int Address) {
  // if at least one sectors with ECP and !TOF is triggered
  if (Multiplicity(GET_ECP(1) && !GET_TOF(1), GET_ECP(2) && !GET_TOF(2),
		   GET_ECP(3) && !GET_TOF(3), GET_ECP(4) && !GET_TOF(4),
		   GET_ECP(5) && !GET_TOF(5), GET_ECP(6) && !GET_TOF(6)) >= 1){
    return 1;
  }
  return 0;
}

////////////////// EC x !TOF >= 2 ///////////////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x !TOF is checked for >= 2
int ECE_ECNoTOF2Trig(int Address) {
  // if at least two sectors with ECE and !TOF is triggered
  if (Multiplicity(GET_ECE(1) && !GET_TOF(1), GET_ECE(2) && !GET_TOF(2),
		   GET_ECE(3) && !GET_TOF(3), GET_ECE(4) && !GET_TOF(4),
		   GET_ECE(5) && !GET_TOF(5), GET_ECE(6) && !GET_TOF(6)) >= 2){
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x !TOF is checked for >= 2
int ECP_ECNoTOF2Trig(int Address) {
  // if at least two sectors with ECP and !TOF is triggered
  if (Multiplicity(GET_ECP(1) && !GET_TOF(1), GET_ECP(2) && !GET_TOF(2),
		   GET_ECP(3) && !GET_TOF(3), GET_ECP(4) && !GET_TOF(4),
		   GET_ECP(5) && !GET_TOF(5), GET_ECP(6) && !GET_TOF(6)) >= 2){
    return 1;
  }
  return 0;
}

//////////////////// EC x TOF >= 1 && EC >= 2 /////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 1 and ECE
//    >= 2 is checked
int ECE_ECTOF1_EC1Trig(int Address) {
  // if at least one sector with ECE and TOF is triggered and at least two
  // sectors with ECE are triggered
  if (ECE_ECTOF1Trig(Address) && ECE_EC2Trig(Address)) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 1 and ECP
//    >= 2 is checked
int ECP_ECTOF1_EC1Trig(int Address) {
  // if at least one sector with ECP and TOF is triggered and at least two
  // sectors with ECP are triggered
  if (ECP_ECTOF1Trig(Address) && ECP_EC2Trig(Address)) {
    return 1;
  }
  return 0;
}

I 3
D 9
//////////////////// EC x TOF >= 1 && EC >= 2 /////////////////////////////
E 9
I 9
//////////////////// EC x TOF >= 2 && EC >= 3 /////////////////////////////
E 9

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 2 and ECE
D 9
//    >= 2 is checked
E 9
I 9
//    >= 3 is checked
E 9
int ECE_ECTOF2_EC1Trig(int Address) {
D 9
  // if at least two sectors with ECE and TOF is triggered and at least two
E 9
I 9
  // if at least two sectors with ECE and TOF is triggered and at least three
E 9
  // sectors with ECE are triggered
D 9
  if (ECE_ECTOF2Trig(Address) && ECE_EC2Trig(Address)) {
E 9
I 9
  if (ECE_ECTOF2Trig(Address) && ECE_EC3Trig(Address)) {
E 9
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 2 and ECP
D 9
//    >= 2 is checked
E 9
I 9
//    >= 3 is checked
E 9
int ECP_ECTOF2_EC1Trig(int Address) {
D 9
  // if at least two sectors with ECP and TOF is triggered and at least two
E 9
I 9
  // if at least two sectors with ECP and TOF is triggered and at least three
E 9
  // sectors with ECP are triggered
D 9
  if (ECP_ECTOF2Trig(Address) && ECP_EC2Trig(Address)) {
E 9
I 9
  if (ECP_ECTOF2Trig(Address) && ECP_EC3Trig(Address)) {
E 9
    return 1;
  }
  return 0;
}


E 3
/////////////////// EC x TOF >= 1 && EC >= 3 //////////////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 1 and ECE
//    >= 3 is checked
int ECE_ECTOF1_EC2Trig(int Address) {
  // if at least one sector with ECE and TOF is triggered and at least three
  // sectors with ECE are triggered
  if (ECE_ECTOF1Trig(Address) && ECE_EC3Trig(Address)) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 1 and ECP
//    >= 3 is checked
int ECP_ECTOF1_EC2Trig(int Address) {
  // if at least one sector with ECP and TOF is triggered and at least three
  // sectors with ECP are triggered
  if (ECP_ECTOF1Trig(Address) && ECP_EC3Trig(Address)) {
    return 1;
  }
  return 0;
}

/////////////////// EC x TOF >= 1 && EC x !TOF >= 1 ///////////////////////

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 1 and
//    ECE x !TOF is checked for >= 1
int ECE_ECTOF1_ECNoTOF1Trig(int Address) {
  // if at least one sector with ECE and TOF is triggered and at least one
  // sector with ECE and !TOF is triggered
  if (ECE_ECTOF1Trig(Address) && ECE_ECNoTOF1Trig(Address)) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 1 and
//    ECP x !TOF is checked for >= 1
int ECP_ECTOF1_ECNoTOF1Trig(int Address) {
  // if at least one sector with ECP and TOF is triggered and at least one
  // sector with ECP and !TOF is triggered
  if (ECP_ECTOF1Trig(Address) && ECP_ECNoTOF1Trig(Address)) {
    return 1;
  }
  return 0;
}

/////////////////// EC x TOF >= 1 && EC x !TOF >= 2 ////////////////////// 

// precondition: the address is viable
// postcondition: the multiplicity of ECE x TOF is checked for >= 1 and
//    ECE x !TOF is checked for >= 2
int ECE_ECTOF1_ECNoTOF2Trig(int Address) {
  // if at least one sector with ECE and TOF is triggered and at least two
  // sectors with ECE and !TOF is triggered
  if (ECE_ECTOF1Trig(Address) && ECE_ECNoTOF2Trig(Address)) {
    return 1;
  }
  return 0;
}

// precondition: the address is viable
// postcondition: the multiplicity of ECP x TOF is checked for >= 1 and
//    ECP x !TOF is checked for >= 2
int ECP_ECTOF1_ECNoTOF2Trig(int Address) {
  // if at least one sector with ECP and TOF is triggered and at least two
  // sectors with ECP and !TOF is triggered
  if (ECP_ECTOF1Trig(Address) && ECP_ECNoTOF2Trig(Address)) {
    return 1;
  }
  return 0;
}

I 4
//////////// Trigger IC >= 1 //////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the number of cluster hits in ic >= 1 is checked
int TrigIC1(int Address) {
  // if at least one clusters in IC are triggered
  if (GET_ICTOTAL_CNT >= 1) {
    return 1;
  }
  return 0;
}

E 4
//////////// Trigger IC >= 2 //////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the number of cluster hits in ic >= 2 is checked
int TrigIC2(int Address) {
  // if at least two clusters in IC are triggered
  if (GET_ICTOTAL_CNT >= 2) {
    return 1;
  }
  return 0;
}

I 4
//////////// Trigger IC >= 3 //////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the number of cluster hits in ic >= 3 is checked
int TrigIC3(int Address) {
  // if at least three clusters in IC are triggered
  if (GET_ICTOTAL_CNT >= 3) {
    return 1;
  }
  return 0;
}

//////////// Trigger IC >= 4 //////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the number of cluster hits in ic >= 4 is checked
int TrigIC4(int Address) {
  // if at least four clusters in IC are triggered
  if (GET_ICTOTAL_CNT >= 4) {
    return 1;
  }
  return 0;
}

//////////// Trigger IC >= 5 //////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the number of cluster hits in ic >= 5 is checked
int TrigIC5(int Address) {
  // if at least five clusters in IC are triggered
  if (GET_ICTOTAL_CNT >= 5) {
    return 1;
  }
  return 0;
}

E 4
//////////// Trigger IC hodoscope >= 1 ///////////////////////////////////

// precondition: the address is viable
// postcondition: the ic hodoscope >= 1 is triggered 
int TrigICe1(int Address) {
  // IC hodoscrope trigger of at least one hit is checked
  if (GET_ICHODO_CNT >= 1) {
    return 1;
  }
  return 0;
}

///////////// Trigger ECP ///////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the trig ecp is checked
int TrigECP(int Address) {
  // something interesting is going on in ECP
  if (GET_TRIG_ECP > 0) {
    return 1;
  }
  return 0;
}


I 2
///////////// Trigger ECE ///////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the trig ece is checked
int TrigECE(int Address) {
  // something interesting is going on in ECE
  if (GET_TRIG_ECE > 0) {
    return 1;
  }
  return 0;
}


E 2
////////////// Trigger ECC ///////////////////////////////////////////////

// precondition: the address is viable
// postcondition: accesses the trigger for ECC and returns result if > 0
int TrigECC(int Address) {
  // build trigger for ECC >= 1 for current address 
  if (GET_TRIG_ECC > 0) {
    return 1;
  }
  return 0;
}

////////////// Trigger ICe >= 1 x ECP //////////////////////////////////////

// precondition: the address is viable
// postcondition: the ICe >= 1 and ECP is checked
int TrigICe1ECP(int Address) {
  // check if at least one cluster is triggered in IC hodoscope and we have
  // something going on in ECP
  if ((GET_ICHODO_CNT >= 1) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

////////////// Trigger IC >= 1 x ECP ///////////////////////////////////////

// precondition: the address is viable
// postcondition: the IC >= 1 and ECP is checked
int TrigIC1ECP(int Address) {
  // check if at least one cluster is triggered in IC total and we have
  // something going on in ECP
  if ((GET_ICTOTAL_CNT >= 1) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

////////////// Trigger IC >= 2 x ECP ///////////////////////////////////////

// precondition: the address is viable
// postcondition: the IC >= 2 and ECP is checked
int TrigIC2ECP(int Address) {
  // check if at least two clusters are triggered in IC total and we have
  // something going on in ECP
  if ((GET_ICTOTAL_CNT >= 2) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

I 3
////////////// Trigger IC >= 2 x ECE ///////////////////////////////////////

// precondition: the address is viable
// postcondition: the IC >= 2 and ECE is checked
int TrigIC2ECE(int Address) {
  // check if at least two clusters are triggered in IC total and we have
  // something going on in ECE
  if ((GET_ICTOTAL_CNT >= 2) && (GET_TRIG_ECE > 0)) {
    return 1;
  }
  return 0;
}


E 3
///////////// Trigger ICv >= 1 x ECP ///////////////////////////////////////

// precondition: the addrses is viable
// postcondition: the ICv >= 1 and ECP is checked
int TrigICv1ECP(int Address) {
  // check if at least one cluster is triggered in IC cluster with hodo veto
  // and we have something going on in ECP
  if ((GET_ICVETO_CNT >= 1) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

////////////// Trigger IC >= 1 x ECE ///////////////////////////////////////

// precondition: the address is viable
// postcondition: the IC >= 1 and ECE is checked
int TrigIC1ECE(int Address) {
  // check if at least one cluster is triggered in IC total and we have
  // something going on in ECE
  if ((GET_ICTOTAL_CNT >= 1) && (GET_TRIG_ECE > 0)) {
    return 1;
  }
  return 0;
}

I 11
/////////////// Trigger ECE and ECP /////////////////////////////////////////

// precondition: the address is viable
D 12
// postcondition: if ECE and ECP are active
int TrigECEECP(int Address) {
  // if something is going on in ECE and ECP then return 1
  if ((GET_TRIG_ECE > 0) && (GET_TRIG_ECP > 0)) {
E 12
I 12
// postcondition: if ECP and ECE are active
int TrigECPECE(int Address) {
  // if something is going on in ECP and ECE then return 1
  if ((GET_TRIG_ECP > 0) && (GET_TRIG_ECE > 0)) {
E 12
    return 1;
  } 
  return 0;
}

I 13
/////////////// Trigger ICv1 /////////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the IC veto count is checked for at least one 
int TrigICV1(int Address) {
  // if at least one IC veto count is hit then return 1
  if (GET_ICVETO_CNT > 0) {
    return 1;
  }
  return 0;
}

I 14
/////////////// Trigger ECC x ECP ////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the ECC and ECP are checked for activity
int TrigECCECP(int Address) {
  // if there is something going on in ECC and ECP then return 1
  if ((GET_TRIG_ECC > 0) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

/////////////// Trigger ECC x ECE ////////////////////////////////////////////

// precondition: the address is viable
// postcondition: the ECC and ECE are checked for activity
int TrigECCECE(int Address) {
  // if there is something going on in ECC and ECE then return 1
  if ((GET_TRIG_ECC > 0) && (GET_TRIG_ECE > 0)) {
    return 1;
  }
  return 0;
}

/////////////// Trigger ECC x ECP x ECE //////////////////////////////////////

// precondition: the address is viable
// postcondition: the ECC, ECP and ECE are checked for activity
int TrigECCECPECE(int Address) {
  // if there is something going on in ECC, ECP and ECE then return 1
  if ((GET_TRIG_ECC > 0) && (GET_TRIG_ECP > 0) && (GET_TRIG_ECE > 0)) {
    return 1;
  }
  return 0;
}

////////////// Trigger IC >= 1 x ECC x ECP ///////////////////////////////////

// precondition: the address is viable
// postcondition: the IC >= 1 and ECC and ECP is checked
int TrigIC1ECCECP(int Address) {
  // check if at least one cluster is triggered in IC total and we have
  // something going on in ECC and ECP
  if ((GET_ICTOTAL_CNT >= 1) && (GET_TRIG_ECC > 0) && (GET_TRIG_ECP > 0)) {
    return 1;
  }
  return 0;
}

I 15
////////////// Trigger IC >= 1 x ECC /////////////////////////////////////////
E 15

I 15
// precondition: the address is viable
// postcondition: the IC >= 1 and ECCis checked
int TrigIC1ECC(int Address) {
  // check if at least one cluster is triggered in IC total and we have
  // something going on in ECC
  if ((GET_ICTOTAL_CNT >= 1) && (GET_TRIG_ECC > 0)) {
    return 1;
  }
  return 0;
}


E 15
E 14
E 13
E 11
I 5
////////////// Single Bit ECP sector 1 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 1 of ECP is checked
int ECP_EC1_BIT1_Trig(int Address) {
 // if ECP sector 1 is triggered then return 1
  if (GET_ECP(1)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP sector 2 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 2 of ECP is checked
int ECP_EC1_BIT2_Trig(int Address) {
 // if ECP sector 2 is triggered then return 1
  if (GET_ECP(2)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP sector 3 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 3 of ECP is checked
int ECP_EC1_BIT3_Trig(int Address) {
 // if ECP sector 3 is triggered then return 1
  if (GET_ECP(3)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP sector 4 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 4 of ECP is checked
int ECP_EC1_BIT4_Trig(int Address) {
 // if ECP sector 4 is triggered then return 1
  if (GET_ECP(4)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP sector 5 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 5 of ECP is checked
int ECP_EC1_BIT5_Trig(int Address) {
 // if ECP sector 5 is triggered then return 1
  if (GET_ECP(5)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP sector 6 //////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 6 of ECP is checked
int ECP_EC1_BIT6_Trig(int Address) {
 // if ECP sector 6 is triggered then return 1
  if (GET_ECP(6)) {
    return 1;
  }
  return 0;
}

I 7
////////////// Single Bit ECP TOF sector 1 ////////////////////////////////////
E 7

I 7
// precondition: the address is viable
// postcondition: the sector 1 of ECP and TOF is checked
int ECP_ECTOF1_BIT1_Trig(int Address) {
 // if ECP x TOF sector 1 is triggered then return 1
  if (GET_ECP(1) && GET_TOF(1)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP TOF sector 2 ////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 2 of ECP and TOF is checked
int ECP_ECTOF1_BIT2_Trig(int Address) {
 // if ECP x TOF sector 2 is triggered then return 1
  if (GET_ECP(2) && GET_TOF(2)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP TOF sector 3 ////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 3 of ECP  and TOF is checked
int ECP_ECTOF1_BIT3_Trig(int Address) {
 // if ECP x TOF sector 3 is triggered then return 1
  if (GET_ECP(3) && GET_TOF(3)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP TOF sector 4 ////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 4 of ECP and TOF is checked
int ECP_ECTOF1_BIT4_Trig(int Address) {
 // if ECP x TOF sector 4 is triggered then return 1
  if (GET_ECP(4) && GET_TOF(4)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP TOF sector 5 ////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 5 of ECP and TOF is checked
int ECP_ECTOF1_BIT5_Trig(int Address) {
 // if ECP x TOFsector 5 is triggered then return 1
  if (GET_ECP(5) && GET_TOF(5)) {
    return 1;
  }
  return 0;
}

////////////// Single Bit ECP TOF sector 6 ////////////////////////////////////

// precondition: the address is viable
// postcondition: the sector 6 of ECP and TOF is checked
int ECP_ECTOF1_BIT6_Trig(int Address) {
 // if ECP x TOF sector 6 is triggered then return 1
  if (GET_ECP(6) && GET_TOF(6)) {
    return 1;
  }
  return 0;
}


E 7
E 5
// level 1 ///////////////////////////////////////////////////////////////////

// precondition: the filename and numbits are viable. The trig_func is a 
//    viable predefined function. The function name is the string name for
//    the trig_func 
// postcondition: the lookup table file is written with appropriate
//    addresses and values for the different modules
void BuildLUT(const char *filename, int numbits, int trig_func(int), 
	      const char *funcname) {
  // local variables
  FILE *f = fopen(filename, "wt");
  int Address;
  char exename[256];

  // statements
  printf("Building LUT: %s...", filename);
  // check file 
  if (!f) {
    printf("Error opening file: %s\n", filename);
    return;
  }
  // print default header for file with name and time stamp
  fprintf(f, "# File: %s\n", filename);
  fprintf(f, "# Last Build Date & Time: %s %s\n", __DATE__, __TIME__);
  // loop through addresses and setup lookup table for max num bits specified
  for (Address = 0; Address < numbits; Address++) {
    fprintf(f, "%d\t#%02X\n", ((*trig_func)(Address)), Address);
  }
  // close file and issue status
  fclose(f);

  // add the function definition to the file header
  sprintf(exename, "addfunction.pl %s %s", funcname, filename);
  system(exename);


  printf("complete.\n");
}

// level 0 functions //////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
  // local variables max number of bits for 6:1 and 12:1 luts
  int MAX_6_BITS = 64;
  int MAX_12_BITS = 4096;

I 2
D 11



E 11
E 2
  // Build ones for 6:1 lut (ECC)
  BuildLUT("6_one.lut", MAX_6_BITS, OneTrig, "OneTrig");
  // Build zeroes for 6:1 lut (ECC)
  BuildLUT("6_zero.lut", MAX_6_BITS, ZeroTrig, "ZeroTrig");

  // Build ones for 12:1 lut (ECP, ECE, and TRIG)
  BuildLUT("12_one.lut", MAX_12_BITS, OneTrig, "OneTrig");
  // Build zeroes for 12:1 lut (ECP, ECE, and TRIG)
  BuildLUT("12_zero.lut", MAX_12_BITS, ZeroTrig, "ZeroTrig");

  // Build ecc with multiplicity >= 1 (electron)
D 12
  BuildLUT("ecc1.lut", MAX_6_BITS, ECCMult1Trig, "ECCMult1Trig");
E 12
I 12
  BuildLUT("ecc_ecc1.lut", MAX_6_BITS, ECCMult1Trig, "ECCMult1Trig");
E 12

  // Build ec E and P with multiplicity >= 1
  BuildLUT("ece_ec1.lut", MAX_12_BITS, ECE_EC1Trig, "ECE_EC1Trig");
  BuildLUT("ecp_ec1.lut", MAX_12_BITS, ECP_EC1Trig, "ECP_EC1Trig");

  // Build ec E and P with multiplicity >= 2
  BuildLUT("ece_ec2.lut", MAX_12_BITS, ECE_EC2Trig,"ECE_EC2Trig");
  BuildLUT("ecp_ec2.lut", MAX_12_BITS, ECP_EC2Trig,"ECP_EC2Trig");

I 10
  // Build ec E and P with multiplicity >= 3
  BuildLUT("ece_ec3.lut", MAX_12_BITS, ECE_EC3Trig,"ECE_EC3Trig");
  BuildLUT("ecp_ec3.lut", MAX_12_BITS, ECP_EC3Trig,"ECP_EC3Trig");

E 10
  // Build ec E and P with tof multiplicity >= 1
  BuildLUT("ece_ectof1.lut", MAX_12_BITS, ECE_ECTOF1Trig, "ECE_ECTOF1Trig");
  BuildLUT("ecp_ectof1.lut", MAX_12_BITS, ECP_ECTOF1Trig, "ECP_ECTOF1Trig");

  // Build ec E and P with tof multiplicity >= 2
  BuildLUT("ece_ectof2.lut", MAX_12_BITS, ECE_ECTOF2Trig, "ECE_ECTOF2Trig");
  BuildLUT("ecp_ectof2.lut", MAX_12_BITS, ECP_ECTOF2Trig, "ECP_ECTOF2Trig");

  // Build ec E and P with tof multiplicity >= 3
  BuildLUT("ece_ectof3.lut", MAX_12_BITS, ECE_ECTOF3Trig, "ECE_ECTOF3Trig");
  BuildLUT("ecp_ectof3.lut", MAX_12_BITS, ECP_ECTOF3Trig, "ECP_ECTOF3Trig");
 
  // Build ec E and P with no tof multiplicity >= 1
  BuildLUT("ece_ecnotof1.lut", MAX_12_BITS, ECE_ECNoTOF1Trig, "ECE_ECNoTOF1Trig");
  BuildLUT("ecp_ecnotof1.lut", MAX_12_BITS, ECP_ECNoTOF1Trig, "ECP_ECNoTOF1Trig");

  // Build ec E and P with no tof multiplicity >= 2
  BuildLUT("ece_ecnotof2.lut", MAX_12_BITS, ECE_ECNoTOF2Trig, "ECE_ECNoTOF2Trig");
  BuildLUT("ecp_ecnotof2.lut", MAX_12_BITS, ECP_ECNoTOF2Trig, "ECP_ECNoTOF2Trig");

  // Build ec E and P with tof mult >= 1 and ec E and P mult >= 2
  BuildLUT("ece_ectof1_ec1.lut", MAX_12_BITS, ECE_ECTOF1_EC1Trig, "ECE_ECTOF1_EC1Trig");
  BuildLUT("ecp_ectof1_ec1.lut", MAX_12_BITS, ECP_ECTOF1_EC1Trig, "ECP_ECTOF1_EC1Trig");

I 3
  // Build ec E and P with tof mult >= 2 and ec E and P mult >= 2
  BuildLUT("ece_ectof2_ec1.lut", MAX_12_BITS, ECE_ECTOF2_EC1Trig, "ECE_ECTOF2_EC1Trig");
  BuildLUT("ecp_ectof2_ec1.lut", MAX_12_BITS, ECP_ECTOF2_EC1Trig, "ECP_ECTOF2_EC1Trig");


E 3
  // Build ec E and P with tof mult >= 1 and ec E and P mult >= 3
  BuildLUT("ece_ectof1_ec2.lut", MAX_12_BITS, ECE_ECTOF1_EC2Trig, "ECE_ECTOF1_EC2Trig");
  BuildLUT("ecp_ectof1_ec2.lut", MAX_12_BITS, ECP_ECTOF1_EC2Trig, "ECP_ECTOF1_EC2Trig");

  // Build ec E and P with tof mult >= 1 and ec E and P with no tof mult >= 1
  BuildLUT("ece_ectof1_ecnotof1.lut", MAX_12_BITS, ECE_ECTOF1_ECNoTOF1Trig, "ECE_ECTOF1_ECNoTOF1Trig");
  BuildLUT("ecp_ectof1_ecnotof1.lut", MAX_12_BITS, ECP_ECTOF1_ECNoTOF1Trig, "ECP_ECTOF1_ECNoTOF1Trig");

  // Build ec E and P with tof mult >= 1 and ec E and P with no tof mult >= 2
  BuildLUT("ece_ectof1_ecnotof2.lut", MAX_12_BITS, ECE_ECTOF1_ECNoTOF2Trig, "ECE_ECTOF1_ECNoTOF2Trig");
  BuildLUT("ecp_ectof1_ecnotof2.lut", MAX_12_BITS, ECP_ECTOF1_ECNoTOF2Trig, "ECP_ECTOF1_ECNoTOF2Trig");

I 2

  ///////////////////////////////////////////
  // calibration tables

  // Build TOF60 and TOF180
I 6
D 8
  BuildLUT("ecp_tof.lut", MAX_12_BITS, ECP_TOFTrig, "ECP_TOFTrig");
E 8
I 8
  BuildLUT("ecp_tof1.lut", MAX_12_BITS, ECP_TOF1Trig, "ECP_TOF1Trig");
E 8
E 6
  BuildLUT("ecp_tof60.lut", MAX_12_BITS, ECP_TOF60Trig, "ECP_TOF60Trig");
  BuildLUT("ecp_tof180.lut", MAX_12_BITS, ECP_TOF180Trig, "ECP_TOF180Trig");





E 2
  //////////////// main trig tables ////////////////////////////////////////
  
  cout << "\n-------------------------------\nMain Trigger tables\n\n";

  ////////// electron trigger //////////////
  // Build ecc trigger
  BuildLUT("trig_ecc.lut", MAX_12_BITS, TrigECC, "TrigECC");
  
  ///////// IC tests //////////////
I 4
  // Build IC table with 1 or more clusters
  BuildLUT("trig_ic1.lut", MAX_12_BITS, TrigIC1, "TrigIC1");
E 4
  // Build IC table with 2 or more clusters
D 4
  BuildLUT("trig_ic2.lut", MAX_12_BITS, TrigIC2, "TrigIC2");  
E 4
I 4
  BuildLUT("trig_ic2.lut", MAX_12_BITS, TrigIC2, "TrigIC2");
  // Build IC table with 2 or more clusters
  BuildLUT("trig_ic3.lut", MAX_12_BITS, TrigIC3, "TrigIC3");
  // Build IC table with 2 or more clusters
  BuildLUT("trig_ic4.lut", MAX_12_BITS, TrigIC4, "TrigIC4");    
  // Build IC table with 2 or more clusters
  BuildLUT("trig_ic5.lut", MAX_12_BITS, TrigIC5, "TrigIC5");
E 4
  // Build IC table with at least one IC hodoscope cluster
  BuildLUT("trig_ice1.lut", MAX_12_BITS, TrigICe1, "TrigICe1");

  //////// Low Q2 /////////////////
  // Build ICe >= 1 and ECP trigger
  BuildLUT("trig_ice1_ecp.lut", MAX_12_BITS, TrigICe1ECP, "TrigICe1ECP");
  
  ///////// Untagged //////////////
  // Build TOF >= 3 
  BuildLUT("ecp_tof3.lut", MAX_12_BITS, ECP_TOF3Trig, "ECP_TOF3Trig");
  BuildLUT("trig_ecp.lut", MAX_12_BITS, TrigECP, "TrigECP");
I 16

D 17
  // Built TOF >= 2
E 17
I 17
  // Build TOF >= 2
E 17
  BuildLUT("ecp_tof2.lut", MAX_12_BITS, ECP_TOF2, "ECP_TOF2");

I 17
  // Build TOF >= 1 and ECP >= 1 for different sectors of TOF and ECP
  BuildLUT("ecp_tof1_ec1.lut", MAX_12_BITS, ECP_TOF1_EC1, "ECP_TOF1_EC1");

  // Build TOF >= 2 and ECP >= 1 for different sectors of TOF and ECP
  BuildLUT("ecp_tof2_ec1.lut", MAX_12_BITS, ECP_TOF2_EC1, "ECP_TOF2_EC1");
E 17
E 16
  
  // Build ECP x TOF >= 2
  // need to use already made ecp_ectof2.lut and trig_ecp.lut

  // Build (ECPTOF) x IC x ECP
  // use already made ecp_ectof1_ec1.lut
  BuildLUT("trig_ic1_ecp.lut", MAX_12_BITS, TrigIC1ECP, "TrigIC1ECP");
  
  ///////// EG 6 trigger //////////////
  // Build ICv x ECP2
  BuildLUT("trig_icv1_ecp.lut", MAX_12_BITS, TrigICv1ECP, "TrigICv1ECP");
 
  // Build IC2 x ECP
  BuildLUT("trig_ic2_ecp.lut", MAX_12_BITS, TrigIC2ECP, "TrigIC2ECP");

I 3
  // Build IC2 X ECE
  BuildLUT("trig_ic2_ece.lut", MAX_12_BITS, TrigIC2ECE, "TrigIC2ECE");

E 3
  // Build IC x ECE
  BuildLUT("trig_ic1_ece.lut", MAX_12_BITS, TrigIC1ECE, "TrigIC1ECE");

I 2
  // Build ECE - for some calibration configs
  BuildLUT("trig_ece.lut", MAX_12_BITS, TrigECE, "TrigECE");

I 11
D 12
  // Build ECE and ECP 
  BuildLUT("trig_ece_ecp.lut", MAX_12_BITS, TrigECEECP, "TrigECEECP");
E 12
I 12
  // Build ECP and ECE 
  BuildLUT("trig_ecp_ece.lut", MAX_12_BITS, TrigECPECE, "TrigECPECE");
E 12

I 13
  // Build IC veto trigger
  BuildLUT("trig_icv1.lut", MAX_12_BITS, TrigICV1, "TrigICV1");

I 14
  // Build ECC and ECP
  BuildLUT("trig_ecc_ecp.lut", MAX_12_BITS, TrigECCECP, "TrigECCECP");
E 14

I 14
  // Build ECC and ECE
  BuildLUT("trig_ecc_ece.lut", MAX_12_BITS, TrigECCECE, "TrigECCECE");
  
  // Build ECC and ECP and ECE
  BuildLUT("trig_ecc_ecp_ece.lut", MAX_12_BITS, TrigECCECPECE,"TrigECCECPECE");

  // Build IC >= 1 and ECC and ECP
  BuildLUT("trig_ic1_ecc_ecp.lut", MAX_12_BITS, TrigIC1ECCECP,"TrigIC1ECCECP");

I 15
  // Build IC >= 1 and ECC
  BuildLUT("trig_ic1_ecc.lut", MAX_12_BITS, TrigIC1ECC,"TrigIC1ECC");


E 15
E 14
E 13
E 11
E 2
  // Build IC x ECPnoTOF
  // use already made ecp_ecnotof1.lut and trig_ic1_ecp.lut

  // Build IC2 x ECPnoTOF
  // use already made ecp_ecnotof1.lut and trig_ic2_ecp.lut
I 5

  // Build single bit LUTs
  BuildLUT("ecp_ec1_bit1.lut", MAX_12_BITS, ECP_EC1_BIT1_Trig, "ECP_EC1_BIT1_Trig");
  BuildLUT("ecp_ec1_bit2.lut", MAX_12_BITS, ECP_EC1_BIT2_Trig, "ECP_EC1_BIT2_Trig");
  BuildLUT("ecp_ec1_bit3.lut", MAX_12_BITS, ECP_EC1_BIT3_Trig, "ECP_EC1_BIT3_Trig");
  BuildLUT("ecp_ec1_bit4.lut", MAX_12_BITS, ECP_EC1_BIT4_Trig, "ECP_EC1_BIT4_Trig");
  BuildLUT("ecp_ec1_bit5.lut", MAX_12_BITS, ECP_EC1_BIT5_Trig, "ECP_EC1_BIT5_Trig");
  BuildLUT("ecp_ec1_bit6.lut", MAX_12_BITS, ECP_EC1_BIT6_Trig, "ECP_EC1_BIT6_Trig");

I 7
  // Build single bit LUTs with tof
  BuildLUT("ecp_ectof1_bit1.lut", MAX_12_BITS, ECP_ECTOF1_BIT1_Trig, "ECP_ECTOF1_BIT1_Trig");
  BuildLUT("ecp_ectof1_bit2.lut", MAX_12_BITS, ECP_ECTOF1_BIT2_Trig, "ECP_ECTOF1_BIT2_Trig");
  BuildLUT("ecp_ectof1_bit3.lut", MAX_12_BITS, ECP_ECTOF1_BIT3_Trig, "ECP_ECTOF1_BIT3_Trig");
  BuildLUT("ecp_ectof1_bit4.lut", MAX_12_BITS, ECP_ECTOF1_BIT4_Trig, "ECP_ECTOF1_BIT4_Trig");
  BuildLUT("ecp_ectof1_bit5.lut", MAX_12_BITS, ECP_ECTOF1_BIT5_Trig, "ECP_ECTOF1_BIT5_Trig");
  BuildLUT("ecp_ectof1_bit6.lut", MAX_12_BITS, ECP_ECTOF1_BIT6_Trig, "ECP_ECTOF1_BIT6_Trig");
E 7

I 7

E 7
E 5
  
  return 0;
}

E 1