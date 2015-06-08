C======================================================================
      SUBROUTINE sda_pdst
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This routine gives access to a event bank 'EVTB'
C-                         for Level = 2 or 4
C-                         
C-   Inputs  : ev_out(36,itr)
C-   Outputs : printout
C-   Controls: none
C-
C-   Library belongs: libsda.a
C-
C-   Created   18-SEP-1997   Bogdan Niczyporuk
C-
C-   Called by ana_prfit & ana_fit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------           
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaanal.inc"
C
C Subroutine parameters
C
C Local variables
      INTEGER i, itr, ntr, nitem, nitemt, ih, nhit
      PARAMETER (nitem = 36)
      PARAMETER (nitemt=  7)
      CHARACTER*5 des(nitem), dest(nitemt)
C
      DATA des/'trk0 ','Yv   ','Zv   ','CXv  ','CYv  ','CZv  ','p    ',
     1         'beta ','Q    ','ilink','Sec/L','Chi2 ','Xcc  ','Ycc  ',          
     2         'Zcc  ','CXcc ','CYcc ','CZcc ','Scc  ','IDcc ','Xsc  ',
     3         'Ysc  ','Zsc  ','CXsc ','CYsc ','CZsc ','Ssc  ','IDsc ',
     4         'Xec  ','Yec  ','Zec  ','CXec ','CYec ','CZec ','Sec  ',
     5         'IDuvw'/
C
      DATA dest/'IdE  ','IdT  ','EnE  ','EnT  ','TimE ','TimT','TimTc'/
C
C
      WRITE(6,100) ltrig(1), ievt, ntr_out
      ntr = ntr_out
      IF(ntr_out.GT.6) ntr = 6
C
      WRITE(6,101)
      DO i = 1,nitem
        IF(i.EQ.36) THEN
          WRITE(6,102) i,des(i),(ev_out(i,itr),itr=1,ntr)
        ELSE
          WRITE(6,103) i,des(i),(ev_out(i,itr),itr=1,ntr)
        ENDIF
      ENDDO
C
      IF(zbeam(1).EQ.0.) THEN
        nhit = ntagh
        IF(ntagh.GT.6) nhit = 6
        WRITE(6,*)
        WRITE(6,201)
        DO i = 1,nitemt
          WRITE(6,202) i,dest(i),(tag_res(i,ih),ih=1,nhit)
        ENDDO
      ENDIF
C
      RETURN
 100  FORMAT(/'  DST:[EVTB bank]  Run =',I6,'  Evt =',I8,'  Ntrk =',I2)                  
 101  FORMAT('  No item     track1    track2    track3    track4    tra          
     1ck5    track6')
 102  FORMAT(I4,1X,A5,6(F10.1))
 103  FORMAT(I4,1X,A5,6(F10.4))
 201  FORMAT('  No item     TGhit1    TGhit2    TGhit3    TGhit4    TGh          
     1it5    TGhit6')
 202  FORMAT(I4,1X,A5,6(F10.4))
      END
