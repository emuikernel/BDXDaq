h13061
s 00000/00000/00000
d R 1.2 01/11/19 19:02:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_ec1id.F
e
s 00038/00000/00000
d D 1.1 01/11/19 19:02:33 boiarino 1 0
c date and time created 01/11/19 19:02:33 by boiarino
e
u
U
f e 0
t
T
I 1
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
E 1
