h58804
s 00000/00000/00000
d R 1.2 01/11/19 19:05:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_link.F
e
s 00145/00000/00000
d D 1.1 01/11/19 19:05:15 boiarino 1 0
c date and time created 01/11/19 19:05:15 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_link(isec,vect1,pin,ntof,ecdigi2,ifail,iwl3,istat,
     &                     segm,clust,nclust,nsegmc)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : find possible trajectories by matching
C-                         permutations of found clusters from 6
C-                         super-layers to the link dictionary.
C-
C-   Inputs  : isec  - the sector to be analyzed
C-             pin   - momentum from simulation (GeV/c)
C-             ifail - control flag
C-
C-   Library belongs    : libana.a
C-
C-   Calls              : none
C-
C-   Created   JUNE- 9-1992 Bogdan Niczyporuk
C-   Modified  JUNE-10-1992 Billy Leung
C-   Modified   DEC-29-1998 Serguei Boiarinov
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
C
C Subroutine parameters
      INTEGER isec,ntof,ecdigi2(3),ifail,iwl3,istat(6)
      REAL vect1(6),pin
      integer segm(12,nsgmx,6,6),clust(nsgcmx,nclmx,6,6),
     &                nclust(6,6),nsegmc(nclmx,6,6)
C Local variables
      BYTE t1(4), t2(4)
      INTEGER mtest1, mtest2
      EQUIVALENCE (mtest1,t1(1)), (mtest2,t2(1))
C
      integer m(6),nw(6,6)

      INTEGER k, n              !               wire  cl  sl
      INTEGER is, icl, isg, nw3(nclmx,6), isegm(192,nclmx,6)  
      BYTE iroad(192,nclmx,6),tmp
      INTEGER ih1, ih2, ih3, ih4, ih5, ih6
      INTEGER icl1, icl2, icl3, icl4, icl5, icl6
      INTEGER trk_found, NWL(6)
      REAL moment
      DATA NWL / 130, 142, 192, 192, 192, 192 /
C
C SIM_LINK begins
C
      t1(4) = 0 ! to make sure
      t2(4) = 0 !
C
C Prepares some arrays to spead up an a link procedure
C
      DO is = 1, 6                        ! loop over superlayers
        DO 1 icl = 1, nclust(is,isec)     ! loop over clusters in superlayer
          nw3(icl,is) = 0 ! counter for roads in layer 3 (could be 0 !)
          DO isg = 1, nsegmc(icl,is,isec) ! loop over segments in cluster
            n = clust(isg,icl,is,isec)    ! segment number
            tmp = IABS(segm(5,n,is,isec)) ! segm(5,...) - layer 3 !!!
            DO k = 1, nw3(icl,is)
              IF(iroad(k,icl,is).EQ.tmp) GOTO 5 ! exist
            ENDDO
            nw3(icl,is) = nw3(icl,is) + 1
            isegm(nw3(icl,is),icl,is) = n
            iroad(nw3(icl,is),icl,is) = tmp ! road number in layer 3
 5          CONTINUE
          ENDDO
 1      CONTINUE
      ENDDO
C
C Loops over all combinations of clusters in supelayers
C
C Loop over track roads in clusters
      ifail = 0
      DO icl2 = 1,nclust(2,isec)
        DO icl3 = 1,nclust(3,isec)
          DO icl5 = 1,nclust(5,isec)
            DO ih2 = 1, nw3(icl2,2)
              DO ih3 = 1, nw3(icl3,3)
                DO ih5 = 1, nw3(icl5,5)
                  t1(1)=iroad(ih2,icl2,2)
                  t1(2)=iroad(ih3,icl3,3)
                  t1(3)=iroad(ih5,icl5,5)
                  DO icl1 = 1,nclust(1,isec)
                    DO icl4 = 1,nclust(4,isec)
                      DO icl6 = 1,nclust(6,isec)
                        DO ih1 = 1, nw3(icl1,1)
                          DO ih4 = 1, nw3(icl4,4)
                            DO ih6 = 1, nw3(icl6,6)
                              t2(1)=iroad(ih1,icl1,1)
                              t2(2)=iroad(ih4,icl4,4)
                              t2(3)=iroad(ih6,icl6,6)
C
                              m(1) = isegm(ih1,icl1,1)
                              m(2) = isegm(ih2,icl2,2)
                              m(3) = isegm(ih3,icl3,3)
                              m(4) = isegm(ih4,icl4,4)
                              m(5) = isegm(ih5,icl5,5)
                              m(6) = isegm(ih6,icl6,6)
                              DO is=1,6
                                DO k=1,6
                                  nw(k,is)=segm(k*2-1,m(is),is,isec)
                                ENDDO
                              ENDDO
C
                              CALL prupdatelink(mtest1,mtest2,vect1,pin,
     &                                ntof,ecdigi2,nw,trk_found,moment)

                              IF(trk_found.EQ.1) THEN
                                iwl3 = nw(3,5)
                                istat(1) = nw(3,1)
                                istat(2) = nw(3,2)
                                istat(3) = nw(3,3)
                                istat(4) = nw(3,4)
                                istat(5) = nw(3,5)
                                istat(6) = nw(3,6)
                                ifail    = ifail + 1  ! No of links/sector
                              ENDIF
                            ENDDO
                          ENDDO
                        ENDDO
                      ENDDO
                    ENDDO
                  ENDDO
                ENDDO
              ENDDO
            ENDDO
          ENDDO
        ENDDO
      ENDDO
C
      RETURN
      END






E 1
