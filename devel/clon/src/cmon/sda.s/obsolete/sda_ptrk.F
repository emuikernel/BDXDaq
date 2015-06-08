C======================================================================
      SUBROUTINE sda_ptrk(ilnk,isec)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This routine gives access to a track info
C-                         for SIM, PR and FF of trajectory fitting.
C-                         
C-   Inputs  :
C-   Outputs :
C-   Controls: Level = 2  Pattern Recognition Fit (no timing)
C-             Level = 4  Final Fit (with timing information)
C-             ilnk       Track number candidate (see PR)          
C-             isec       Current Sector number          
C-
C-   Library belongs: libsda.a
C-
C-   Created   1-SEP-1995   Bogdan Niczyporuk
C-
C-   Called by ana_prfit & ana_fit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdageom.inc"
#include "sdaanal.inc"
C
C Subroutine parameters
      INTEGER Level, ilnk, isec
C
C Local variables
      INTEGER i, il, ip, iw, iq, isl, ifirst
      CHARACTER*2 DET(npln)
      DATA ifirst /1/
C
C
      IF(ifirst.EQ.1) THEN
        DET(1) = 'VT'
        DO ip = nst_min,nst_max
          DET(ip) = 'ST'
        ENDDO
        DO ip = ndc_min,ndc_max
          DET(ip) = 'DC'
        ENDDO
        DET(ncc_min) = 'CC'
        DO ip = nsc_min,nsc_max
          DET(ip) = 'SC'
        ENDDO
        DO ip = nec_min,nec_max
          DET(ip) = 'EC'
        ENDDO
        ifirst = 0
      ENDIF
C
      Level = trkp(11,1,ilnk)
      WRITE(6,101) Level,isec,ilnk,trkp(12,1,ilnk)
      IF(Level.EQ.2) THEN
        WRITE(6,104) (lnk_clust(i,ilnk),i=1,6)
        WRITE(6,105) ( lnk_segm(i,ilnk),i=1,6)
      ENDIF
      IF(Level.EQ.4) THEN
        WRITE(6,104) (lnk_clust(i,ilnk),i=1,6)
        WRITE(6,105) (dca_segm(i,isec,ilnk),i=1,6)
      ENDIF
      WRITE(6,102) 
      DO 10 il = 1,npl_dc
        iw = trk( 1,il,ilnk)
        isl= trk(11,il,ilnk)
        IF(il.EQ.5 .OR. il.EQ.6) GO TO 10
        IF(MOD(il,6).EQ.1 .AND. il.GT.1) WRITE(6,*)
        WRITE(6,103) il,iw,(trk(i,il,ilnk),i=2,10),isl,trk(12,il,ilnk)
 10   CONTINUE
C 
      WRITE(6,201)
      DO 20 ip = 1,npln
        IF(ip.GT.3 .AND. ip.LE.39) GO TO 20
c*      il = ip - nst_max
        iq = trkp( 9,ip,ilnk)
        isl= trkp(11,ip,ilnk)
c*      IF(il.EQ.5 .OR. il.EQ.6) GO TO 20
        WRITE(6,202) DET(ip),ip,(trkp(i,ip,ilnk),i=1,8)
     1              ,iq,trkp(10,ip,ilnk),isl                      
 20   CONTINUE
C
 999  CONTINUE
C
 101  FORMAT(//' LEVEL=',I2,'  Sector=',I2,'  itrk=',I2
     1     ,'  CHISQ=',E10.3)       
 102  FORMAT(' il   iw    tdc     Td     Dm   SIGm   beta     Dt   Tlen
     1   Wlen  alfa S/L Stat')      
 103  FORMAT(I3,I5,2F7.1,2F7.3,F7.4,F7.3,2F7.1,F8.4,I3,F4.1)
 104  FORMAT(' Linked Clusters in SLs =',6I4) 
 105  FORMAT(' Linked Segments in SLs =',6I4) 
 201  FORMAT(/4X,'      X        Y        Z     Cx     Cy     Cz      p
     1    beta   Q  Tlen S/L')
 202  FORMAT(1X,A2,I2,3F9.3,3F7.4,1X,F5.3,1X,F6.4,I3,F7.2,I3)
C
      RETURN
      END
