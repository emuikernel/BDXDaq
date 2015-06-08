C======================================================================
      SUBROUTINE sim_tobos
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Writes the digitalization results (simulation)
C-                          into the BOS banks.
C-
C-    Inputs  : xx_digi(3,ih,il,isec)
C-    Outputs : BOS banks in IW array
C-    Controls:
C-
C-    Library belongs: libsim.a
C-
C-    Calls:  
C-
C-    Created     6-24-1994   Bogdan Niczyporuk
C-
C-
C-    Called by SIM_MAIN in libsim.a
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdabos.inc"
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdadigi.inc"
C
C
C External functions
      INTEGER ISHFT, ETNCREATE, iucomp
C
C Local variables
      INTEGER*2 I16(2000)
      INTEGER*4 I32(1000)
      EQUIVALENCE ( I16(1),I32(1) )
C
      INTEGER uout, ierr, ifsimo
      INTEGER ind, i, il, nl, id, ih, nh, nd, it, isec, nsec, ifirst
      INTEGER dtime
C
      DATA nl/36/, nsec/6/, ifirst/1/
C
      IF(ifirst.EQ.1) THEN
        ifsimo = 0
        IF(iucomp(4HWSIM,levb,10).NE.0) ifsimo = 1
        CALL ETNFORMAT(IW,'HEAD','I')
        CALL ETNFORMAT(IW,'DC0 ','B16')
        CALL ETNFORMAT(IW,'CC  ','B16')
        CALL ETNFORMAT(IW,'SC  ','B16')
        CALL ETNFORMAT(IW,'EC  ','B16')
        CALL ETNFORMAT(IW,'MCIN','F')
        if (zbeam(1).EQ.0.0) then
           CALL ETNFORMAT(IW,'ST  ','B16')
           CALL ETNFORMAT(IW,'TAGT','B16')
           CALL ETNFORMAT(IW,'TAGE','B16')
           CALL ETNFORMAT(IW,'CALL','B16')
        endif
        ifirst = 0
      ENDIF
C
C Drop LIST and do garbage collection for previous event
C 
      CALL ETLDROP(IW,'E')
      CALL ETNGARB(IW)
C
C Store Info in HEAD bank (Begin of RUN)
C
      IF(ievt.EQ.1) THEN
C Prestart Event
        CALL ETLCTL(IW,'E=','HEAD')
        ind = ETNCREATE(IW,'HEAD',0, 8,1)
        CALL GetUnixTime(dtime)
C
        IW(ind+1) = 1        ! Version of Event Format
        IW(ind+2) = ltrig(1) ! Run Number
        IW(ind+3) = 0        ! Event Number
        IW(ind+4) = dtime    ! Unix Time
        IW(ind+5) = -1       ! Event Type (DATA =0, SDA MC =-1, Geant MC =-2)
        IW(ind+6) = 0        ! ROC Status
        IW(ind+7) = 17       ! Event Class
        IW(ind+8) = 0        ! Event Weight
        IF(ifsimo.EQ.1) THEN
          uout = 3
          CALL FWBOS(IW,uout,'E',ierr)
        ENDIF
        CALL ETLDROP(IW,'E')
C Go Event
        CALL ETLCTL(IW,'E=','HEAD')
        ind = ETNCREATE(IW,'HEAD',0, 8,1)
        CALL GetUnixTime(dtime)
C
        IW(ind+1) = 1        ! Version of Event Format
        IW(ind+2) = ltrig(1) ! Run Number
        IW(ind+3) = 0        ! Event Number
        IW(ind+4) = dtime    ! Unix Time
        IW(ind+5) = -1       ! Event Type (DATA =0, SDA MC =-1, Geant MC =-2)
        IW(ind+6) = 0        ! ROC Status
        IW(ind+7) = 18       ! Event Class
        IW(ind+8) = 0        ! Event Weight
        IF(ifsimo.EQ.1) THEN
          uout = 3
          CALL FWBOS(IW,uout,'E',ierr)
        ENDIF
        CALL ETLDROP(IW,'E')
      ENDIF
C
C Store Info in HEAD bank (with Events)
C
      CALL ETLCTL(IW,'E=','HEAD')
      ind = ETNCREATE(IW,'HEAD',0, 8,1)
      CALL GetUnixTime(dtime)
C
      IW(ind+1) = 1        ! Version of Event Format
      IW(ind+2) = ltrig(1) ! Run Number
      IW(ind+3) = ievt     ! Event Number
      IW(ind+4) = dtime    ! Unix Time
      IW(ind+5) = -1       ! Event Type (DATA =0, SDA MC =-1, Geant MC =-2)
      IW(ind+6) = 0        ! ROC Status
      IW(ind+7) = 11       ! Event Class
      IW(ind+8) = 0        ! Event Weight
C
C Store DC info
C 
      DO 10 isec = 1,nsec
        id = 0
        DO 9 il = 1,nl
          IF(il.EQ.5 .OR. il.EQ.6) GO TO 9
          nh = dc_ndig(il,isec)
C Check if there are hits in a layer "il"
          IF(nh.EQ.0) GO TO 9
          DO ih = 1,nh
            I16(id+1) = dc_digi(1,ih,il,isec) + ISHFT(il,8)
            I16(id+2) = dc_digi(2,ih,il,isec)  
            id  = id + 2
          ENDDO
 9      CONTINUE
        IF(id.LT.2) GO TO 10
        nd = id/2
        nh = nd
C Create bank
        CALL ETLCTL(IW,'E+','DC0 ')
        ind = ETNCREATE(IW,'DC0 ',isec, 2,nh)
        IF(ind.EQ.0) GO TO 10
C Store info in bank
        DO ih = 1,nd
          IW(ind+ih) = I32(ih)
        ENDDO
 10   CONTINUE
C
C Store CC info
C 
      DO 20 isec = 1,nsec
        id = 0
        nh = cc_ndig(1,isec)
C Check if there are hits in a PT "il"
        IF(nh.EQ.0) GO TO 20
        DO ih = 1,nh
          I16(id+1) = cc_digi(1,ih,1,isec)
          I16(id+2) = cc_digi(2,ih,1,isec)  
          I16(id+3) = cc_digi(3,ih,1,isec)  
          id  = id + 3
        ENDDO
        IF(id.LT.3) GO TO 20
        nd = (id + 1)/2
C Create bank
        CALL ETLCTL(IW,'E+','CC  ')
        ind = ETNCREATE(IW,'CC  ',isec, 3,nh)
        IF(ind.EQ.0) GO TO 20
C Store info in bank
        DO id = 1,nd
          IW(ind+id) = I32(id)
        ENDDO
 20   CONTINUE
C
C Store SC info
C 
      DO 30 isec = 1,nsec
        id = 0
        nh = sc_ndig(1,isec)
C Check if there are hits in a slab
        IF(nh.EQ.0) GO TO 30
        DO ih = 1,nh
          I16(id+1) = sc_digi(1,ih,1,isec)
          I16(id+2) = sc_digi(2,ih,1,isec)  
          I16(id+3) = sc_digi(3,ih,1,isec)  
          I16(id+4) = sc_digi(4,ih,1,isec)  
          I16(id+5) = sc_digi(5,ih,1,isec)  
          id  = id + 5
        ENDDO
        IF(id.LT.5) GO TO 30
        nd = (id + 1)/2
C Create bank
        CALL ETLCTL(IW,'E+','SC  ')
        ind = ETNCREATE(IW,'SC  ',isec, 5,nh)
        IF(ind.EQ.0) GO TO 30
C Store info in bank
        DO id = 1,nd
          IW(ind+id) = I32(id)
        ENDDO
 30   CONTINUE
C
C Store EC info
C 
      DO 40 isec = 1,nsec
        id = 0
        DO 39 il = 1,6             ! Loop over the u1,v1,w1,u2,v2 and w2 layers
          nh = ec_ndig(il,isec)
C Check if there are hits in a layer "il"
          IF(nh.EQ.0) GO TO 39
          DO ih = 1,nh
            I16(id+1) = ec_digi(1,ih,il,isec) + ISHFT(il,8)
            I16(id+2) = ec_digi(2,ih,il,isec)  
            I16(id+3) = ec_digi(3,ih,il,isec)  
            id  = id + 3
          ENDDO
 39     CONTINUE
        IF(id.LT.3) GO TO 40
        nd = (id + 1)/2
        nh = id/3
C Create bank
        CALL ETLCTL(IW,'E+','EC  ')
        ind = ETNCREATE(IW,'EC  ',isec, 3,nh)
        IF(ind.EQ.0) GO TO 40
C Store info in bank
        DO id = 1,nd
          IW(ind+id) = I32(id)
        ENDDO
 40   CONTINUE
C
C Store ST info
C 
      id = 0
C Check if there are hits there
      IF(st_ndig.EQ.0) GO TO 50
      DO ih = 1,st_ndig
        IF(st_digi(1,ih).GT.0) THEN
          I16(id+1) = st_digi(1,ih)
          I16(id+2) = st_digi(2,ih)  
          I16(id+3) = st_digi(3,ih)  
          id  = id + 3
        ENDIF
      ENDDO
      IF(id.LT.3) GO TO 50
      nd = (id + 1)/2
      nh = id/3
C Create bank
      CALL ETLCTL(IW,'E+','ST  ')
C                  NAME NR Nc Nr
      ind = ETNCREATE(IW,'ST  ',0, 3,nh)
      IF(ind.EQ.0) GO TO 50
C Store info in bank
      DO id = 1,nd
        IW(ind+id) = I32(id)
      ENDDO
 50   CONTINUE
C
C store TAGE, TAGT info
C 
      IF(tge_ndig.GT.0) THEN
         id = 0
         DO ih = 1,tge_ndig
            I16(id+1) = tge_digi(1,ih)
            I16(id+2) = tge_digi(2,ih)
            id = id +2
         ENDDO
C create TAGE bank and store digis
         ind = ETNCREATE(IW,'TAGE', 0, 2, tge_ndig)
         IF(ind.GT.0) THEN
            DO id = 1,tge_ndig
               IW(ind+id) = I32(id)
            ENDDO
            CALL ETLCTL(IW,'E+','TAGE')
         ENDIF
      ENDIF
C  
      IF(tgt_ndig.EQ.0) GO TO 60
      id = 0
      DO ih=1,tgt_ndig
         I16(id+1) = tgt_digi(1,ih)
         I16(id+2) = tgt_digi(2,ih)
         I16(id+3) = tgt_digi(3,ih)
         id = id +3
      ENDDO
      nd = (id + 1)/2
      nh = id/3
C Create TAGT bank and store digis
      ind = ETNCREATE(IW,'TAGT', 0, 3,nh)
      IF(ind.GT.0) THEN
         DO id = 1,nd
            IW(ind+id) = I32(id)
         ENDDO
         CALL ETLCTL(IW,'E+','TAGT')
      ENDIF

 60   CONTINUE
C
C Store CALL info (beam bunch markers)
C 
      id = 0
C Check if there are hits there
      IF(bm_ndig.EQ.0) GO TO 70
      DO ih = 1,bm_ndig
        IF(bm_digi(1,ih).GT.0) THEN
          I16(id+1) = bm_digi(1,ih)
          I16(id+2) = bm_digi(2,ih)  
          I16(id+3) = bm_digi(3,ih)  
          id  = id + 3
        ENDIF
      ENDDO
      IF(id.LT.3) GO TO 70
      nd = (id + 1)/2
      nh = id/3
C Create bank
      CALL ETLCTL(IW,'E+','CALL')
C                  NAME NR Nc Nr
      ind = ETNCREATE(IW,'CALL',0, 3,nh)
      IF(ind.EQ.0) GO TO 70
C Store info in bank
      DO id = 1,nd
        IW(ind+id) = I32(id)
      ENDDO
 70   CONTINUE
C
C Store "ev_in(9,it)" (track parameters) in MCIN, MCVX, MCTK banks
C
      nd = 9*ntrack
      CALL ETLCTL(IW,'E+','MCIN')
      ind = ETNCREATE(IW,'MCIN',0, 9,ntrack)
      DO it = 1,ntrack
        id = (it - 1)*9
        DO i = 1,9
          RW(ind+id+i) = ev_in(i,it)
        ENDDO
      ENDDO       
C
C Write BOS or FPACK banks onto Disk (tape)
C
      IF(ifsimo.EQ.1) THEN
        uout = 3
        CALL FWBOS(IW,uout,'E',ierr)
C Write HEAD bank after last Event (End of RUN)
        IF(ievt.EQ.Nevt) THEN
          CALL ETLDROP(IW,'E')
          CALL ETLCTL(IW,'E=','HEAD')
          ind = ETNCREATE(IW,'HEAD',0, 8,1)
          CALL GetUnixTime(dtime)
C
          IW(ind+1) = 1        ! Version of Event Format
          IW(ind+2) = ltrig(1) ! Run Number
          IW(ind+3) = Nevt+1   ! Event Number
          IW(ind+4) = dtime    ! Unix Time
          IW(ind+5) = -1       ! Event Type (DATA =0, SDA MC =-1, Geant MC =-2)
          IW(ind+6) = 0        ! ROC Status
          IW(ind+7) = 20       ! Event Class
          IW(ind+8) = 0        ! Event Weight
          CALL FWBOS(IW,uout,'E',ierr)
        ENDIF
      ENDIF
C
 999  CONTINUE
      RETURN
      END
C======================================================================
