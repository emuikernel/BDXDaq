h16767
s 00000/00000/00000
d R 1.2 01/11/19 19:06:33 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/obsolete/dcevin.c
e
s 00396/00000/00000
d D 1.1 01/11/19 19:06:32 boiarino 1 0
c date and time created 01/11/19 19:06:32 by boiarino
e
u
U
f e 0
t
T
I 1
/*
    Purpose and Methods : Copies the BOS banks into "sdadigi" common
                          and prints Raw Event Buffer on request.

    Inputs  :
    Outputs :
    Controls:

    Library belongs: libsim.a

    Calls: None 

    Created     6-24-1994   Bogdan Niczyporuk
    C remake by Sergey Boyarinov

    Called by sda_anal
*/


#include <stdio.h>
#include "dclib.h"
#include "sdakeys.h"
#include "sdaevgen.h"
#include "sdageom.h"
#include "sdadigi.h"


void
dcevin_(int *jw, int *ntr_out, float ev_out[ntrmx][36],
                                 int *ntagh, float tag_res[10][7])
{
  dcevin(jw, ntr_out, ev_out, ntagh, tag_res);
  return;
}

void
dcevin(int *jw, int *ntr_out, float ev_out[ntrmx][36],
                                int *ntagh, float tag_res[10][7])
{
  int ind,i,j,il,is,ih,nd,nd2,isec,id,it,minw,maxw,j16,itdc,nam_tr,nam_dst,nam_tag;
  float *rw;
  int i32[7000];
  short *i16;

  static int nl = 36, npt = 36, nstr = 36;
  static int Ncol = 36;                 /* Nitems/track in an event */
  static int Ncoltg = 7;                /* Nitems/track in tagger hit */
  static short i8R = 255, i8L = 32512, dcid;
  /* Gates for Sep. Runs:     .5 mics      1 mics      2 mics */
  static int min_tdc[6] = { 1000, 1000, 2000, 2000, 2000, 2000 };
  static int max_tdc[6] = { 2500, 2500, 5000, 5000, 7000, 7000 };
  /* Exchanged signal cables in DC Sector1 Region2: dctt(n) <-> dctt(16+n) */
  static short dctt[32] = { 3441, 3442, 3443, 3697, 3698, 3953, 3954, 3955,
                            4209, 4210, 4465, 4466, 4467, 4721, 4722, 4723,
                            4981, 4982, 4983, 5237, 5238, 5493, 5494, 5495,
                            5749, 5750, 6005, 6006, 6007, 6261, 6262, 6263 };

  rw = (float *) jw;
  i16 = (short *) i32;

  /* Clear arrays needed for digitalization */

  for(isec=1; isec<=6; isec++)
  {
    for(il=1; il<=nl; il++) sdadigi_.dc_ndig[isec-1][il-1] = 0;
    il = 1;
    sdadigi_.cc_ndig[isec-1][il-1] = 0;
    sdadigi_.sc_ndig[isec-1][il-1] = 0;
    for(il=1; il<=4; il++) sdadigi_.ec1_ndig[isec-1][il-1] = 0;
  }

  /* At first entry define name-indices */

  nam_tr  = etNamind(jw,"MCIN");
  nam_dst = etNamind(jw,"EVTB");
  nam_tag = etNamind(jw,"TGR ");

  /* Loop over simulated tracks to store info in "ev_in(9,it)" array */

  if(nam_tr == 0) goto a1;
  ind = nam_tr + 1;
  ind = etNnext(jw,ind);
  if(ind == 0) goto a1;
  nd = etNdata(jw,ind);
  if(nd == 0) goto a1;
  sdaevgen_.ntrack = nd / 9;
  for(it=0; it<sdaevgen_.ntrack; it++)
  {
    j = it * 9;
    for(i=0; i<9; i++) sdaevgen_.ev_in[it][i] = rw[ind+j+i];
    dcrotds_(&sdaevgen_.ev_in[it][0],&sdaevgen_.evin[it][0],&isec);
    sdaevgen_.itrin_sec[it] = isec;
    /* Get vertex point at the (Z,X)-plane (SDA Coordinate System) */
    if(sdakeys_.zmagn[3] == 0.)
    {
      sdaevgen_.evin[it][0] -= sdaevgen_.evin[it][1]*sdaevgen_.evin[it][3]/sdaevgen_.evin[it][4];
      sdaevgen_.evin[it][2] -= sdaevgen_.evin[it][1]*sdaevgen_.evin[it][5]/sdaevgen_.evin[it][4];
      sdaevgen_.evin[it][1]  = 0.0;
    }
  }
a1:

  /* Loop over reconstructed tracks to store info in "ev_out(Ncol,it)" array */

  if(sdakeys_.lanal[0] != 0) goto a5;
  if(nam_dst == 0) goto a2;
  ind = nam_dst+1;
  ind = etNnext(jw,ind);
  if(ind == 0) goto a2;
  nd = etNdata(jw,ind);
  if(nd < Ncol) goto a2;
  *ntr_out = nd / Ncol;
  for(it=0; it < *ntr_out; it++)
  {
    j = it * Ncol;
    for(i=0; i<Ncol; i++) ev_out[it][i] = rw[ind+j+i];
  }
a2:

  /* Reconstracted tagger hits */
  if(nam_tag == 0) goto a3;
  ind = nam_tag + 1;
  ind = etNnext(jw,ind);
  if(ind == 0) goto a3;
  nd = etNdata(jw,ind);
  if(nd < Ncoltg) goto a3;
  *ntagh = nd/Ncoltg;
  for(it=0; it<*ntagh; it++)
  {
    j = it*Ncoltg;
    for(i=0; i<Ncoltg; i++) tag_res[it][i] = rw[ind+j+i];
  }
a3:

  if(sdakeys_.lprnt[6] == 1 && *ntr_out > 0)
          dcpdst(*ntr_out,ev_out,*ntagh,tag_res);
a5:

  /* Store BM info in "bm_digi" array */
  sdadigi_.bm_ndig = 0;
  for(ih=0; ih<7; ih++)
  {
    sdadigi_.bm_digi[ih][0] = 0;
    sdadigi_.bm_digi[ih][1] = 0;
    sdadigi_.bm_digi[ih][2] = 0;
  }
  for(isec=0; isec<=0; isec++)
  {
    if((ind=etNlink(jw,"CALL",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "bm_digi( , )" */
    sdadigi_.bm_ndig = 7;
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=3)
    {
      ih = i16[i-1];
      if(ih > 0 && ih <= 7)
      {
        sdadigi_.bm_digi[ih-1][0] = i16[i-1];
        sdadigi_.bm_digi[ih-1][1] = i16[i+1-1];
        sdadigi_.bm_digi[ih-1][2] = i16[i+2-1];
      }
    }
  }


  /* Store ST info in "st_digi" array */

  sdadigi_.st_ndig  = 0;
  for(ih=1; ih<=6; ih++)
  {
    sdadigi_.st_digi[ih-1][0] = 0;
    sdadigi_.st_digi[ih-1][1] = 0;
    sdadigi_.st_digi[ih-1][2] = 0;
  }
  for(isec=0; isec<=0; isec++)
  {
    if((ind=etNlink(jw,"ST  ",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "st_digi( , )" */
    sdadigi_.st_ndig = 6;
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=3)
    {
      ih = i16[i-1];
      if(ih > 0)
      {
        sdadigi_.st_digi[ih-1][0] = i16[i-1];
        sdadigi_.st_digi[ih-1][1] = i16[i+1-1];
        sdadigi_.st_digi[ih-1][2] = i16[i+2-1];
      }
    }
  }


  /* Loop over sectors to store info in "dc_digi" array */

  for(isec=1; isec<=6; isec++)
  {
    ind = etNlink(jw,"DC0 ",isec);
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    if(nd > 6528) printf(" EVIN: Nw/sect > 6528 and is %d\n",nd);
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "dc_digi( , , , )" */
    nd2 = 2*nd;
    for(i=1; i<=nd2; i+=2)
    {
      id = i16[i-1] & i8R;
      j16 = i16[i-1] & i8L;
      il = j16 >> 8;

      is   = (il - 1)/6 + 1;
      itdc = i16[i+1-1];
      if(il < 1 || il > nl) continue;
      minw = sdageom_.pln[isec-1][il+3-1][14];
      maxw = sdageom_.pln[isec-1][il+3-1][15];
      if(id < minw || id > maxw)
      {
        printf(" EVIN: Wire# out of range\n");
        continue;
      }

      /* Exchanged signal cable in Sector1 Region2 Wires 113-119 (F.K.) */
      if(isec == 1 && (is+1)/2 == 2 && id>112 && id<120)
      {
        dcid = i16[i-1];
        for(j=1; j<=32; j++)
        {
          if(dcid == dctt[j-1])
          {
            if(j > 16)
              dcid = dctt[j-16-1];
            else
              dcid = dctt[j+16-1];
            break;
          }
        }
        id = dcid & i8R;
        j16 = dcid & i8L;
        il = j16 >> 8;
        is  = (il + 5)/6;
      }

      if(itdc <= min_tdc[is-1] || itdc > max_tdc[is-1]) continue;
      if(sdadigi_.dc_ndig[isec-1][il-1] >= 192) continue;
      sdadigi_.dc_ndig[isec-1][il-1]++;
      ih = sdadigi_.dc_ndig[isec-1][il-1];
      sdadigi_.dc_digi[isec-1][il-1][ih-1][0] = id;
      sdadigi_.dc_digi[isec-1][il-1][ih-1][1] = itdc;
    }
  }


  /* Loop over sectors to store info in "cc_digi" array */

  for(isec=1; isec<=6; isec++)
  {
    ind = etNlink(jw,"CC  ",isec);
    nd  = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "cc_digi( , , , )" */
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=3)
    {
      id = i16[i-1];
      if(id < 1 || id > 36) continue;
      il = 1;
      if(sdadigi_.cc_ndig[isec-1][il-1] >= npt) continue;
      sdadigi_.cc_ndig[isec-1][il-1]++;
      ih = sdadigi_.cc_ndig[isec-1][il-1];
      sdadigi_.cc_digi[isec-1][il-1][ih-1][0] = i16[i-1];
      sdadigi_.cc_digi[isec-1][il-1][ih-1][1] = i16[i+1-1];
      sdadigi_.cc_digi[isec-1][il-1][ih-1][2] = i16[i+2-1];
    }
  }


  /* Loop over sectors to store info in "sc_digi" array */

  for(isec=1; isec<=6; isec++)
  {
    if((ind=etNlink(jw,"SC  ",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=5)
    {
      id = i16[i-1];
      if(id < 1 || id > 48) continue;
      il = 1;
      if(sdadigi_.sc_ndig[isec-1][il-1] >= 48) continue;
      sdadigi_.sc_ndig[isec-1][il-1]++;
      ih = sdadigi_.sc_ndig[isec-1][il-1];
      sdadigi_.sc_digi[isec-1][il-1][ih-1][0] = i16[i-1];
      sdadigi_.sc_digi[isec-1][il-1][ih-1][1] = i16[i+1-1];
      sdadigi_.sc_digi[isec-1][il-1][ih-1][2] = i16[i+2-1];
      sdadigi_.sc_digi[isec-1][il-1][ih-1][3] = i16[i+3-1];
      sdadigi_.sc_digi[isec-1][il-1][ih-1][4] = i16[i+4-1];
    }
  }


  /* Loop over sectors to store info in "ec1_digi" array */

  for(isec=1; isec<=2; isec++)
  {
    if((ind=etNlink(jw,"EC1 ",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "ec1_digi( , , , )" */
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=5)
    {
      id = i16[i-1] & i8R;
      if(id < 1 || id > 40) continue;
      j16 = i16[i-1] & i8L;
      il = j16 >> 8;
      if(il < 1 || il > 4) continue;
      if(sdadigi_.ec1_ndig[isec-1][il-1] >= 40) continue;
      if(il == 1 || il == 3)
      {
        if(sdadigi_.ec1_ndig[isec-1][il-1] >= 24) continue;
        if(id < 1 || id > 24) continue;
      }
      sdadigi_.ec1_ndig[isec-1][il-1] ++;
      ih = sdadigi_.ec1_ndig[isec-1][il-1];
      sdadigi_.ec1_digi[isec-1][il-1][ih-1][0] = id;  
      sdadigi_.ec1_digi[isec-1][il-1][ih-1][0] = i16[i+1-1];  
      sdadigi_.ec1_digi[isec-1][il-1][ih-1][0] = i16[i+2-1];
      sdadigi_.ec1_digi[isec-1][il-1][ih-1][0] = i16[i+3-1];
      sdadigi_.ec1_digi[isec-1][il-1][ih-1][0] = i16[i+4-1];
    }
  }


  /* Loop over sectors to store info in "tgt_digi" array */

  sdadigi_.tgt_ndig = 0;
  for(isec=1; isec<=1; isec++)
  {
    if((ind=etNlink(jw,"TAGT",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "tgt_digi( , )" */
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=3)
    {
      id = i16[i-1];
      if(id < 1 || id > 61) break;
      if(sdadigi_.tgt_ndig >= 62) break;
      sdadigi_.tgt_ndig ++;
      ih = sdadigi_.tgt_ndig;
      sdadigi_.tgt_digi[ih-1][0] = i16[i-1];
      sdadigi_.tgt_digi[ih-1][1] = i16[i+1-1];
      sdadigi_.tgt_digi[ih-1][2] = i16[i+2-1];
    }
  }


  /* Loop over sectors to store info in "tge_digi" array */

  sdadigi_.tge_ndig = 0;
  for(isec=1; isec<=1; isec++)
  {
    if((ind=etNlink(jw,"TAGE",isec)) < 0) continue;
    nd = etNdata(jw,ind);
    if(nd == 0) continue;
    for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1];
    /* Unpack and store in "tge_digi( , )" */
    nd2 = 2*nd - 1;
    for(i=1; i<=nd2; i+=2)
    {
      id = i16[i-1];
      if(id < 1 || id > 384) break;
      if(sdadigi_.tge_ndig >= 384) break;
      sdadigi_.tge_ndig ++;
      ih = sdadigi_.tge_ndig;
      sdadigi_.tge_digi[ih-1][0] = i16[i-1];
      sdadigi_.tge_digi[ih-1][1] = i16[i+1-1];
    }
  }

  /* Print EVB */
  if(sdakeys_.lprnt[1] > 0) sim_pevb_(jw);

  return;
}
E 1
