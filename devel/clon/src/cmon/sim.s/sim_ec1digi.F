C======================================================================
      SUBROUTINE sim_ec1digi
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Digitalization of hits recorded in EC1 planes for
C-                          photons and electrons (steering routine).
C-                          Actual Digitalization is donne in "elss_access".
C-
C-   Inputs  : Swimming results
C-   Outputs : ec1digi & ec1_ndig arrays in "sdadigi.inc"
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: ???
C-
C-   Created  26-MAY-1998   Bogdan Niczyporuk
C-
C-    Called by sim_main
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdacalib.inc"
C
C Local variables
C
C
 999  CONTINUE
      RETURN
      END
