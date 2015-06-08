C======================================================================
      SUBROUTINE sim_ccdigi
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Digitalization of hits recorded in CC planes.
C-
C-   Inputs  : Swimming results & CC Geometry
C-   Outputs : cc_digi & cc_ndig arrays
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: none
C-
C-   Created   25-FEB-1997   Bogdan Niczyporuk
C-
C-    Called by sim_main
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaevgen.inc"
#include "sdageom.inc"
C
C Local variables
C
      INTEGER it, iacc, isec
      REAL Yright, Xleft, tn30, PHT, hcclen
C
      DATA tn30 /0.57735 /                        ! TAN(30deg)
      DATA PHT  /30.0    /                        ! Space for Photo Tube
C
C
C Loop over all track in a simulated event
      DO 200 it = 1,ntrack
        IF(evin(9,it).EQ.0.) GO TO 200            ! No DIGI for Q = 0
        isec = itrin_sec(it)
        iacc = 0
C Check CC-acceptance for all charged particles
        IF(cc_trps(1,1,it) .GT. 999.) GO TO 10
        Yright = sda_pln(13,ncc_min,isec) + 7.0
        IF(cc_trps(2,1,it) .LE. Yright) GO TO 10        
        Xleft  = sda_pln(14,ncc_min,isec) + 3.0
        IF(cc_trps(1,1,it) .LE. Xleft ) GO TO 10
        hcclen = ABS(cc_trps(2,1,it)*tn30) - PHT
        IF(ABS(sc_trps(3,1,it)). GE. hcclen) GO TO 10
        iacc = 1
 10     CONTINUE
        itrin_acc(3,it) = iacc
 200  CONTINUE                                    ! End of loop over all tracks 
C
 999  CONTINUE
      RETURN
      END
