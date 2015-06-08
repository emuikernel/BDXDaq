C======================================================================
      SUBROUTINE sim_pevb(iw)
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Prints Raw Event Buffer (BOS) from "sdadigi"
C-
C-    Inputs  :
C-    Outputs :
C-    Controls:
C-
C-    Library belongs: libsim.a
C-
C-    Calls: None 
C-
C-    Created     8-24-1995   Bogdan Niczyporuk
C-
C-
C-    Called by sim_main, sda_evin 
C-
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdadigi.inc"
C
      INTEGER iw(1000)
C
C External function
      INTEGER ETNLINK
C
C Local variables
      INTEGER i, ind, il, ih, isec, it, nh, nhp, iprnt        
      INTEGER ifirst, nsect, nl
      INTEGER RunNo,EvtNo,EvtType,EvtClas, iq
     1       ,nhdc, nhcc, nhsc, nhec, nhec1, nhsum                        
      CHARACTER*2 namep(6), namep1(4) 
      CHARACTER*24 dtimes 
C
      DATA ifirst/1/, nsect/6/, nl/36/
C
      IF(ifirst.EQ.1) THEN
        IF(ltrig(4).NE.0) nsect = ltrig(4)
        namep(1) = 'u1'
        namep(2) = 'v1'
        namep(3) = 'w1'
        namep(4) = 'u2'
        namep(5) = 'v2'
        namep(6) = 'w2'
         namep1(1) = 'x1'
         namep1(2) = 'z1'
         namep1(3) = 'x2'
         namep1(4) = 'z2'
        ifirst = 0
      ENDIF
C
C Debug printout
C
C Print info from HEAD bank 
      IF(lprnt(2).LT.0) THEN
        ind = ETNLINK(IW,'HEAD',0)
        RunNo   = IW(ind+2)
        IF(RunNo.EQ.0) RunNo = ltrig(1)
        EvtNo   = IW(ind+3)
        EvtType = IW(ind+5)
        EvtClas = IW(ind+7)
        CALL GetASCIITime(IW(ind+4),dtimes)
        iprnt    = -lprnt(2)
        IF(MOD(EvtNo,iprnt).EQ.0) THEN
          WRITE(6,1001) RunNo,EvtNo,EvtType,EvtClas,dtimes
        ENDIF
      ENDIF
      IF(lprnt(2).NE.1) GO TO 999
C
C MCIN dump
      IF(ntrack.GT.0) THEN
        WRITE(6,5001)
        WRITE(6,5002)
        DO it = 1,ntrack
          iq = ev_in(9,it)
          WRITE(6,1002) it,(ev_in(i,it),i=1,8),iq
        ENDDO
      ENDIF
C
C ST dump
      IF(st_ndig.GT.0) THEN
        WRITE(6,6001)
        nh = 0
        DO ih = 1,st_ndig
          IF(st_digi(1,ih).GT.0) THEN
            nh = nh + 1
            WRITE(6,6002) nh,st_digi(1,ih),st_digi(2,ih),st_digi(3,ih)
          ENDIF
        ENDDO
      ENDIF
C
C BM dump
      IF(bm_ndig.GT.0) THEN
        WRITE(6,6003)
        nh = 0
        DO ih = 1,bm_ndig
          IF(bm_digi(1,ih).GT.0) THEN
            nh = nh + 1
            WRITE(6,6004) nh,bm_digi(1,ih),bm_digi(2,ih),bm_digi(3,ih)
          ENDIF
        ENDDO
      ENDIF
C
C TAGT dump
      IF(tgt_ndig.GT.0) THEN
        WRITE(6,6005)
        DO ih = 1,tgt_ndig
          WRITE(6,6006) ih,tgt_digi(1,ih),tgt_digi(2,ih),tgt_digi(3,ih)         
        ENDDO
      ENDIF
C
C TAGE dump
      IF(tge_ndig.GT.0) THEN
        WRITE(6,6007)
        DO ih = 1,tge_ndig
          WRITE(6,6008) ih,tge_digi(1,ih),tge_digi(2,ih)         
        ENDDO
      ENDIF
C
C Loop over Sectors
      DO isec = 1,nsect
        nhdc = 0
        DO il = 1,nl
          nhdc = nhdc + dc_ndig(il,isec)
        ENDDO
        nhcc = cc_ndig(1,isec)
        nhsc = sc_ndig(1,isec)
        nhec = 0
        DO il = 1,6
          nhec = nhec + ec_ndig(il,isec)
        ENDDO
        nhec1 = 0
        IF(isec.LT.3) THEN
          DO il = 1,4
            nhec1 = nhec1 + ec1_ndig(il,isec)
          ENDDO
        ENDIF
        nhsum = nhdc + nhcc + nhsc + nhec + nhec1
        IF(nhsum.GT.0) WRITE(6,1003) isec
C
C DC dump
        IF(nhdc.GT.0) THEN
          WRITE(6,1005)
          DO 100 il = 1,nl
            IF(lanal(2).NE.1.AND.lanal(2).NE.2) THEN
              IF(il.EQ.5 .OR. il.EQ.6) GO TO 100
            ENDIF
            nh = dc_ndig(il,isec)
            IF(nh.EQ.0) GO TO 100
            nhp = nh
            IF(nh.GT.6) nhp = 6
            IF(MOD(il,6).EQ.1 .AND. il.GT.1) WRITE(6,*)
            WRITE(6,1004) il,nh,(dc_digi(1,ih,il,isec)
     1                       ,dc_digi(2,ih,il,isec),ih=1,nhp)
 100      CONTINUE
        ENDIF
C
C CC dump
        IF(nhcc.GT.0) THEN
          WRITE(6,2001)
          nh = cc_ndig(1,isec)
          DO ih = 1,nh
            WRITE(6,2002) ih,cc_digi(1,ih,1,isec),cc_digi(2,ih,1,isec)              
     1                      ,cc_digi(3,ih,1,isec)
          ENDDO
        ENDIF
C
C SC dump
        IF(nhsc.GT.0) THEN
        WRITE(6,3001)
        nh = sc_ndig(1,isec)
        DO ih = 1,nh
         WRITE(6,3002) ih,sc_digi(1,ih,1,isec),sc_digi(2,ih,1,isec)       
     1   ,sc_digi(3,ih,1,isec),sc_digi(4,ih,1,isec),sc_digi(5,ih,1,isec)                               
        ENDDO
        ENDIF
C
C EC dump
        IF(nhec.GT.0) THEN
          WRITE(6,4001)
          DO 400 il = 1,6
           nh = ec_ndig(il,isec)
           IF(nh.EQ.0) GO TO 400
           nhp = nh
           IF(nh.GT.4) nhp = 4
           WRITE(6,4002) namep(il),nh, (ec_digi(1,ih,il,isec) 
     1            ,ec_digi(2,ih,il,isec),ec_digi(3,ih,il,isec),ih=1,nhp)                          
 400      CONTINUE
        ENDIF
C
C EC1 dump
        IF(nhec1.GT.0) THEN
          WRITE(6,4011)
          DO 500 il = 1,4
           nh = ec1_ndig(il,isec)
           IF(nh.EQ.0) GO TO 500
           nhp = nh
           IF(nh.GT.4) nhp = 4
           WRITE(6,4012) namep1(il),nh,(ec1_digi(1,ih,il,isec) 
     1          ,ec1_digi(2,ih,il,isec),ec1_digi(3,ih,il,isec)                                                
     2          ,ec1_digi(4,ih,il,isec),ec1_digi(5,ih,il,isec),ih=1,nhp)                                                
 500      CONTINUE
        ENDIF
      ENDDO    ! End of loop over Sectors
C
 999  CONTINUE
C
 1001 FORMAT(//' LEVEL= 0 (EVB)'/
     1     ' RunNo=',I6,'   EvtNo=',I7,'   Type/Clas=',I3,'/',I2,3X,A24)                                                                                                           
 1002 FORMAT(3X,I3,3F8.3,1X,3F8.4,1X,F8.3,1X,F8.5,I4)
 1003 FORMAT(/' *** Sector =',I2,' *** ')                            
 1004 FORMAT(2I3,6(1X,I5,I6))
 1005 FORMAT(/' DC Raw Buffer'/' il nh   (id  tdc)...')
 2001 FORMAT(/' CC Raw Buffer'/'    ih    id  tdc  adc')
 2002 FORMAT(3X,I3,1X,3I5)
 3001 FORMAT(/' SC Raw Buffer'/'    ih    id tdcL adcL tdcR adcR')
 3002 FORMAT(3X,I3,1X,5I5)
 4001 FORMAT(/' EC Raw Buffer'/' il nh   (id  tdc  adc)...')
 4002 FORMAT(1X,A2,I3,4(1X,3I5))
 4011 FORMAT(/' EC1 Raw Buffer'/' il nh   (id tdcL adcL tdcR adcR)...')
 4012 FORMAT(1X,A2,I3,4(1X,5I5))
 5001 FORMAT(/' MCIN Event')
 5002 FORMAT(4X,'it    Xv      Yv      Zv      Cx      Cy      Cz                                    
     1 p      mass     Q')
 6001 FORMAT(/' ST Raw Buffer'/'    ih    id  tdc  adc')
 6002 FORMAT(3X,I3,1X,3I5)
 6003 FORMAT(/' BM Raw Buffer'/'    ih    id  tdc  adc')
 6004 FORMAT(3X,I3,1X,3I5)
 6005 FORMAT(/' TAGT Raw Buffer'/'    ih    id tdcL tdcR')
 6006 FORMAT(3X,I3,1X,3I5)
 6007 FORMAT(/' TAGE Raw Buffer'/'    ih    id  tdc')
 6008 FORMAT(3X,I3,1X,2I5)
C
      RETURN
      END
