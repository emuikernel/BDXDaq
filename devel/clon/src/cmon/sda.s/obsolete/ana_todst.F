C======================================================================
      SUBROUTINE ana_todst(iw, rw, ntr_out, ev_out, ntagh, tag_res)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Writes the reconstructed event into DST. 
C-                         
C-
C-   Inputs  :  ...
C-
C-   Outputs :  DST (or MiniDST)
C-
C-   Controls:
C-
C-   Library belongs: libana.a
C-
C-   Calls: BOS routines
C-
C-   Created   AUG-14-1997  Bogdan Niczyporuk
C-
C-   Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
#include "sdakeys.inc"
C
      INTEGER iw(1000),ntr_out,ntagh
      REAL rw(1000),ev_out(36,ntrmx),tag_res(7,10)
C
C External
      INTEGER ETNCREATE
C
C Local variables
      INTEGER ifirst, i, it, id, ih, iret 
      INTEGER  inde, indt, NcolEVT, NcolTGR
      DATA ifirst /1/
      DATA NcolEVT /40/
      DATA NcolTGR /7/
C
C
      IF(ifirst.EQ.1) THEN
        CALL ETNFORMAT(IW,'EVTB','F')
        CALL ETNFORMAT(IW,'TGR ','F')
        CALL ETNFORMAT(IW,'TRAK','F')
        ifirst = 0
      ENDIF
C
c      print *,'=========================================='
c      call bprnt(iw,'E')
c      print *,'=========================================='

cc      CALL ETLCTL(IW,'E=','HEADCALLDC0 CC  SC  EC  EC1 EVTB')
cc      IF(zbeam(1).EQ.0.) CALL ETLCTL(IW,'E+','ST  TAGTTAGETGR ')
cc      CALL ETLCTL(IW,'E+','PATH')
      CALL ETLCTL(IW,'E=','HEADTGBITBTRTRAK')
C          
C
C Create event bank
      inde = ETNCREATE(IW,'EVTB',0, NcolEVT,ntr_out)
      DO it = 1,ntr_out
        id = (it - 1)*NcolEVT
        DO i = 1,NcolEVT
          RW(inde+id+i) = ev_out(i,it)
        ENDDO
      ENDDO       
C
C Create tagger bank
      IF(ntagh.GT.0) THEN
        indt = ETNCREATE(IW,'TGR ',0, NcolTGR,ntagh)
        DO ih = 1,ntagh
          id = (ih - 1)*NcolTGR
          DO i = 1,NcolTGR
            RW(indt+id+i) = tag_res(i,ih)
          ENDDO
        ENDDO
      ENDIF       
C
      CALL FWBOS(IW,2,'E',iret)
      CALL ETLDROP(IW,'E')
      CALL ETNGARB(IW)
C
C
      RETURN
      END
