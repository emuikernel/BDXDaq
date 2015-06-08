C======================================================================
      SUBROUTINE ana_ec1id(itr, iflg)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Finds energy cluster the track "itr" points to
C-
C-
C-   Input   :  itr     - track candidate number
C-
C-   Outputs :  iflg = 0  means OK
C-              
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   26-MAY-1998  Bogdan Niczyporuk
C-
C-   Called by ???
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdadigi.inc"
#include "sdageom.inc"
#include "sdacalib.inc"
C
C Subroutine parameters
      INTEGER itr, iflg
C
      RETURN
      END
