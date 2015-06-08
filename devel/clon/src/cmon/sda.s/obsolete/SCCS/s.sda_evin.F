h13879
s 00000/00000/00000
d R 1.2 01/11/19 19:02:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_evin.F
e
s 00481/00000/00000
d D 1.1 01/11/19 19:02:37 boiarino 1 0
c date and time created 01/11/19 19:02:37 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_evin(iw, rw, ntr_out, ev_out, ntagh, tag_res)
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Copies the BOS banks into "sdadigi" common
C-                          and prints Raw Event Buffer on request.
C-
C-    Inputs  :
C-    Outputs :
C-    Controls:
C-
C-    Library belongs: libsim.a
C-
C-    Calls: None 
C-
C-    Created     6-24-1994   Bogdan Niczyporuk
C-
C-
C-    Called by sda_anal
C-
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
C
      INTEGER iw(1000),ntr_out,ntagh
      REAL rw(1000),ev_out(36,ntrmx),tag_res(7,10)
C
C External functions
      INTEGER ETNAMIND, ETNNEXT, ETNNUM, ETNDATA, ISHFT, IAND
C
C Local variables
      INTEGER*2 I16(14000)
      INTEGER*4 I32(7000)
      EQUIVALENCE ( I16(1),I32(1) )
C
      INTEGER ind, i,j, il, is, ih, nd, nd2, isec, id, it, minw,maxw
      INTEGER j16, itdc, min_tdc(6),max_tdc(6)
      INTEGER*2 i8R, i8L
      INTEGER Ncol, Ncoltg, nsect, nl, npt, nstr
      INTEGER nam_st, nam_dc, nam_cc, nam_bm, nam_sc, nam_ec, nam_tr
     1       ,nam_ec1, nam_tgt, nam_tge, nam_dst, nam_tag        
C        
      DATA nsect/6/, nl/36/, npt/36/, nstr/36/
      DATA nam_st/0/, nam_dc/0/, nam_cc/0/, nam_bm/0/
     1    ,nam_sc/0/, nam_ec/0/, nam_tr/0/, nam_dst/0/
     2    ,nam_ec1/0/, nam_tgt/0/, nam_tge/0/, nam_tag/0/
C
      DATA Ncol/36/                     ! Nitems/track in an event
      DATA Ncoltg /7/                   ! Nitems/event in tagger hit
      DATA i8R/255/, i8L/32512/
C
C Gates: Sep97+ Runs  750 ns     1500 ns     2500 ns
      DATA min_tdc /1000, 1000, 2000, 2000, 2000, 2000/
      DATA max_tdc /2500, 2500, 5000, 5000, 7000, 7000/
C
C Exchanged signal cables in DC Sector1 Region2:
C                            dctt(n) <-> dctt(16+n)
      INTEGER*2  dctt(32), dcid
      DATA  dctt /3441,3442,3443,3697,3698,3953,3954,3955,4209,4210,
     1            4465,4466,4467,4721,4722,4723,
     2            4981,4982,4983,5237,5238,5493,5494,5495,5749,5750,            
     3            6005,6006,6007,6261,6262,6263/            
      
C
C
C Clear arrays needed for digitalization
      DO ih = 1,7
        bm_digi(1,ih) = 0
        bm_digi(2,ih) = 0
        bm_digi(3,ih) = 0
      ENDDO
      st_ndig  = 0
      tgt_ndig = 0
      tge_ndig = 0
      DO ih = 1,nsect
        st_digi(1,ih) = 0
        st_digi(2,ih) = 0
        st_digi(3,ih) = 0
      ENDDO
      DO isec = 1,nsect
        DO il = 1,nl
          dc_ndig(il,isec) = 0
        ENDDO
        il = 1
        cc_ndig(il,isec) = 0
        sc_ndig(il,isec) = 0
        DO il = 1,6
          ec_ndig(il,isec) = 0
        ENDDO
        DO il = 1,4
          ec1_ndig(il,isec) = 0
        ENDDO
      ENDDO
C
C At first entry define name-indices
C
      nam_tr  = ETNAMIND(IW,'MCIN')
      nam_bm  = ETNAMIND(IW,'CALL')
      nam_st  = ETNAMIND(IW,'ST  ')
      nam_dc  = ETNAMIND(IW,'DC0 ')
      nam_cc  = ETNAMIND(IW,'CC  ')
      nam_sc  = ETNAMIND(IW,'SC  ')
      nam_ec  = ETNAMIND(IW,'EC  ')
      nam_ec1 = ETNAMIND(IW,'EC1 ')
      nam_tgt = ETNAMIND(IW,'TAGT')
      nam_tge = ETNAMIND(IW,'TAGE')
      nam_dst = ETNAMIND(IW,'EVTB')
      nam_tag = ETNAMIND(IW,'TGR ')
C
C Loop over simulated tracks to store info in "ev_in(9,it)" array
C
      IF(nam_tr .EQ. 0) GO TO 1
      ind = nam_tr + 1
      ind = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 1
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 1
        ntrack = nd/9
        DO it = 1,ntrack
          j = (it - 1)*9
          DO i = 1,9
            ev_in(i,it) = RW(ind+j +i)
          ENDDO
          CALL dcrotds(ev_in(1,it),evin(1,it),isec)
          itrin_sec(it) = isec
C Get vertex point at the (Z,X)-plane (SDA Coordinate System)
          IF(zmagn(4).EQ.0.) THEN
            evin(1,it) = evin(1,it) - evin(2,it)*evin(4,it)/evin(5,it)
            evin(3,it) = evin(3,it) - evin(2,it)*evin(6,it)/evin(5,it)
            evin(2,it) = 0.
          ENDIF
        ENDDO
 1    CONTINUE
C
C Loop over reconstructed tracks to store info in "ev_out(Ncol,it)" array
C
      IF(lanal(1) .NE. 0) GO TO 5
C Reconstructed tracks
      IF(nam_dst  .EQ. 0) GO TO 2
      ind = nam_dst + 1
      ind = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 2
        nd = etndata(IW,ind)
        IF(nd.LT.Ncol) GO TO 2
        ntr_out = nd/Ncol
        DO it = 1,ntr_out
          j = (it - 1)*Ncol
          DO i = 1,Ncol
            ev_out(i,it) = RW(ind+j +i)
c      print *,'F=',RW(ind+j +i),'  I=',IW(ind+j +i)
          ENDDO
        ENDDO
 2    CONTINUE
C Reconstracted tagger hits
      IF(nam_tag  .EQ. 0) GO TO 3
      ind = nam_tag + 1
      ind = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 3
        nd = etndata(IW,ind)
        IF(nd.LT.Ncoltg) GO TO 3
        ntagh = nd/Ncoltg
        DO it = 1,ntagh
          j = (it - 1)*Ncoltg
          DO i = 1,Ncoltg
            tag_res(i,it) = RW(ind+j +i)
          ENDDO
        ENDDO
 3      CONTINUE
        IF(lprnt(7).EQ.1 .AND. ntr_out.GT.0)
     &    CALL dcpdst(ntr_out, ev_out, ntagh, tag_res)
C
 5    CONTINUE
C
C Store BM info in "bm_digi" array
C
      ind = nam_bm + 1
      ind = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 6
        nd = etndata(IW,ind)
        IF(nd.EQ.0)  GO TO 6
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "bm_digi( , )"
        bm_ndig = 7
        nd2 = 2*nd - 1
        DO i = 1,nd2,3
          ih = I16(i)
          IF(ih.GT.0 .AND. ih.LE.7) THEN
            bm_digi(1,ih) = I16(i)  
            bm_digi(2,ih) = I16(i+1)  
            bm_digi(3,ih) = I16(i+2)
          ENDIF
        ENDDO
 6    CONTINUE
C
C Store ST info in "st_digi" array
C
      ind = nam_st + 1
      ind = etnnext(iw,ind)
        IF(ind.EQ.0) GOTO 11
        nd = etndata(IW,ind)

        IF(nd.EQ.0)  GOTO 11
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "st_digi( , )"
        st_ndig = 6
        nd2 = 2*nd - 1
        DO i = 1,nd2,3
          ih = I16(i)
          IF(ih.GT.0) THEN
            st_digi(1,ih) = I16(i)  
            st_digi(2,ih) = I16(i+1)  
            st_digi(3,ih) = I16(i+2)
          ENDIF
        ENDDO
 11   CONTINUE
C
C Loop over sectors to store info in "dc_digi" array
C
      ind = nam_dc + 1
 20   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 22
        isec = etnnum(IW,ind)
        IF(isec.LT.1 .OR. isec.GT.nsect) GO TO 20
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 20
        IF(nd.GT.6528) WRITE(6,*) ' EVIN: Nw/sect > 6528 and is',nd
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "dc_digi( , , , )"
        nd2 = 2*nd
        DO 21 i = 1,nd2,2
          id  = IAND(I16(i),i8R)
          j16 = IAND(I16(i),i8L)
          il  = ISHFT(j16,-8)
          is  = (il - 1)/6 + 1
          itdc= I16(i+1)
          IF(il.LT.1 .OR. il.GT.nl)  GO TO 21
          minw = sda_pln(15,il+3,isec)
          maxw = sda_pln(16,il+3,isec)
          IF(id.LT.minw .OR. id.GT.maxw) GO TO 21
C Exchanged signal cable in Sector1 Region2 Wires 113-119 (F.K.)
          IF(isec.EQ.1 .AND. (is+1)/2.EQ.2 .AND. 
     1                id.GT.112 .AND. id.LT.120) THEN
             dcid = I16(i)
             DO j = 1,32
                IF(dcid .EQ. dctt(j)) THEN
                   IF(j .GT. 16) THEN
                      dcid = dctt(j-16)
                   ELSE
                      dcid = dctt(j+16)
                   ENDIF
                   GO TO 23
                ENDIF
             ENDDO
 23          id  = IAND(dcid,i8R)
             j16 = IAND(dcid,i8L)
             il  = ISHFT(j16,-8)
             is  = (il + 5)/6 
          ENDIF
          IF(itdc.LE.min_tdc(is) .OR. itdc.GT.max_tdc(is)) GO TO 21
          IF(dc_ndig(il,isec).GE.192) GO TO 21
          dc_ndig(il,isec) = dc_ndig(il,isec) + 1
          ih = dc_ndig(il,isec)
          dc_digi(1,ih,il,isec) = id  
          dc_digi(2,ih,il,isec) = itdc  
 21     CONTINUE
C Go to next sector
        GO TO 20
 22   CONTINUE
C
C Loop over sectors to store info in "cc_digi" array
C
      ind = nam_cc + 1
 30   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 32
        isec = etnnum(IW,ind)
        IF(isec.LT.1 .OR. isec.GT.nsect) GO TO 30
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 30
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "cc_digi( , , , )"
        nd2 = 2*nd - 1
        DO 31 i = 1,nd2,3
          id = I16(i)
          IF(id.LT.1 .OR. id.GT.36) GO TO 31
          il = 1
          IF(cc_ndig(il,isec).GE.npt) GO TO 31
          cc_ndig(il,isec) = cc_ndig(il,isec) + 1
          ih = cc_ndig(il,isec)
          cc_digi(1,ih,il,isec) = I16(i) 
          cc_digi(2,ih,il,isec) = I16(i+1) 
          cc_digi(3,ih,il,isec) = I16(i+2)
 31     CONTINUE
C Go to next sector
        GO TO 30
 32   CONTINUE
C
C Loop over sectors to store info in "sc_digi" array
C
      ind = nam_sc + 1
 40   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 42
        isec = etnnum(IW,ind)
        IF(isec.LT.1 .OR. isec.GT.nsect) GO TO 40
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 40
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "sc_digi( , , , )"
        nd2 = 2*nd - 1
        DO 41 i = 1,nd2,5
          id = I16(i)
          IF(id.LT.1 .OR. id.GT.48) GO TO 41
          il = 1
          IF(sc_ndig(il,isec).GE.48) GO TO 41
          sc_ndig(il,isec) = sc_ndig(il,isec) + 1
          ih = sc_ndig(il,isec)
          sc_digi(1,ih,il,isec) = I16(i)  
          sc_digi(2,ih,il,isec) = I16(i+1)  
          sc_digi(3,ih,il,isec) = I16(i+2)
          sc_digi(4,ih,il,isec) = I16(i+3)
          sc_digi(5,ih,il,isec) = I16(i+4)
 41     CONTINUE
C Go to next sector
        GO TO 40
 42   CONTINUE
C
C Loop over sectors to store info in "ec_digi" array
C
      ind = nam_ec + 1
 50   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 52
        isec = etnnum(IW,ind)
        IF(isec.LT.1 .OR. isec.GT.nsect) GO TO 50
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 50
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "ec_digi( , , , )"
        nd2 = 2*nd - 1
        DO 51 i = 1,nd2,3
          id  = IAND(I16(i),i8R)
          IF(id.LT.1 .OR. id.GT.nstr) GO TO 51
          j16 = IAND(I16(i),i8L)
          il  = ISHFT(j16,-8)
          IF(il.LT.1 .OR. il.GT.6) GO TO 51
          IF(ec_ndig(il,isec).GE.nstr) GO TO 51
          ec_ndig(il,isec) = ec_ndig(il,isec) + 1
          ih = ec_ndig(il,isec)
          ec_digi(1,ih,il,isec) = id  
          ec_digi(2,ih,il,isec) = I16(i+1)  
          ec_digi(3,ih,il,isec) = I16(i+2)
 51     CONTINUE
C Go to next sector
        GO TO 50
 52   CONTINUE
C
C Loop over sectors to store info in "ec1_digi" array
C
      ind = nam_ec1 + 1
 60   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 62         
        isec = etnnum(IW,ind)
        IF(isec.LT.1 .OR. isec.GT.2) GO TO 62
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 60
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "ec1_digi( , , , )"
        nd2 = 2*nd - 1
        DO 61 i = 1,nd2,5
          id  = IAND(I16(i),i8R)
          IF(id.LT.1 .OR. id.GT.40) GO TO 61
          j16 = IAND(I16(i),i8L)
          il  = ISHFT(j16,-8)
          IF(il.LT.1 .OR. il.GT.4) GO TO 61
          IF(ec1_ndig(il,isec).GE.40) GO TO 61
          IF(il.EQ.1 .OR. il.EQ.3) THEN
            IF(ec1_ndig(il,isec).GE.24) GO TO 61
            IF(id.LT.1 .OR. id.GT.24) GO TO 61
          ENDIF
          ec1_ndig(il,isec) = ec1_ndig(il,isec) + 1
          ih = ec1_ndig(il,isec)
          ec1_digi(1,ih,il,isec) = id  
          ec1_digi(2,ih,il,isec) = I16(i+1)  
          ec1_digi(3,ih,il,isec) = I16(i+2)
          ec1_digi(4,ih,il,isec) = I16(i+3)
          ec1_digi(5,ih,il,isec) = I16(i+4)
 61     CONTINUE
C Go to next sector
        GO TO 60
 62   CONTINUE
C
C Loop over sectors to store info in "tgt_digi" array
C
      ind = nam_tgt + 1
 70   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 72
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 72
C
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "tgt_digi( , )"
        nd2 = 2*nd - 1
        DO 71 i = 1,nd2,3
          id = I16(i)
          IF(id.LT.1 .OR. id.GT.61) GO TO 71
          IF(tgt_ndig.GE.62) GO TO 71
          tgt_ndig = tgt_ndig + 1
          ih = tgt_ndig
          tgt_digi(1,ih) = I16(i) 
          tgt_digi(2,ih) = I16(i+1) 
          tgt_digi(3,ih) = I16(i+2)
 71     CONTINUE
 72     CONTINUE
C
C Loop over sectors to store info in "tgt_digi" array
C
      ind = nam_tge + 1
 80   CONTINUE
        ind  = etnnext(iw,ind)
        IF(ind.EQ.0) GO TO 82
        nd = etndata(IW,ind)
        IF(nd.EQ.0) GO TO 82
        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO
C Unpack and store in "tge_digi( , )"
        nd2 = 2*nd - 1
        DO 81 i = 1,nd2,2
          id = I16(i)
          IF(id.LT.1 .OR. id.GT.384) GO TO 81
          IF(tge_ndig.GE.384) GO TO 81
          tge_ndig = tge_ndig + 1
          ih = tge_ndig
          tge_digi(1,ih) = I16(i) 
          tge_digi(2,ih) = I16(i+1) 
 81     CONTINUE
 82     CONTINUE
C
C Print EVB
      IF(lprnt(2).NE.0) CALL sda_pevb(iw)
C
 999  CONTINUE
      RETURN
      END

E 1
