h18572
s 00000/00000/00000
d R 1.2 01/11/19 19:02:33 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_dcam.F
e
s 00236/00000/00000
d D 1.1 01/11/19 19:02:32 boiarino 1 0
c date and time created 01/11/19 19:02:32 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE ana_dcam(ana,ifail)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Main goal is to derive "dcam" for each hit
C-                         of the candidate track. 
C-
C-
C-   Inputs  : include files
C-
C-   Controls: ifail
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : dcscid, ana_beta, ana_dtime,
C-                        dcdocam, dclramb
C-
C-   Created   JUN-12-1992  Bogdan Niczyporuk
C-   Modified  FEB-28-1996  Bogdan Niczyporuk
C-
C-   Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaanal.inc"
c just to get npl_sc
#include "sdageom.inc"
C
C External
C
C Subroutine parameters
      INTEGER ifail
      integer ana(1000000)
C
      INTEGER nlay
      PARAMETER (nlay = 6)
C
C Local variables
      INTEGER i,k, il, iflg, ilmin,is,icl,isg,ilnk,isgc,isgb, la,lab,lamin
      INTEGER improve, nsgc, n_seg,n_trk, isec,Level, itr_new
C
      REAL chibest,alfa, beta, dat0,dat0B, slopD,slopDbest,chib,cut
      REAL digi(5,nlay), digib(5,nlay)
      real x0(npl_sc)
c
      real IntSg(6,ntrmx)         ! was in sdaanal.inc, but nobody use it
      integer dca_segm(6,6,ntrmx) ! was in sdaanal.inc, but nobody use it
C dca_segm(isl,isec,itr)  - Track Segment No with the resolved LR-ambiguity
C                           in the SuperLayer "isl" and Sector "isec" for "itr" 
C  
C    ===> e.g. [dca_segm (isl,isec,itr), itr = 1,ntr_link] <===
C
      INTEGER nsuper, NlinSL(6)
C
      DATA nsuper /6/
      DATA NlinSL /4, 6, 6, 6, 6, 6/
C
C
      Level = 3
      n_trk = 0
      ifail = 0
      cut = zcut(6) + 3.
C
C Loop over linked track candidates
      IF(ntr_link.EQ.0) GO TO 999
C
C Loop over Track Candidates (linked Clusters "ntr_link")
C
      DO 100 ilnk = 1,ntr_link
C
C Check if fit was successful
        IF(itr_level(ilnk).NE.2) GO TO 100
        isec = itr_sect(ilnk)
        IF(lprnt(5).EQ.1) WRITE(6,101) Level,isec,ilnk
C
C Find SC slab "id" for track candidate "ilnk" other then electron
        IF(ilnk.NE.itrk0) THEN
c          CALL ana_scid(ilnk, iflg)
          do i=1,npl_sc
             x0(i)=trkp(1,nsc_min-1+i,ilnk)
          enddo
          CALL dcscid(isec, itr_scid(1,ilnk), x0, iflg)
          IF(iflg.GT.0) GOTO 100
        ENDIF
C
C Determine the velocity "beta" from Time of Flight 

cc        CALL ana_beta(ilnk,itrk0,itr_sect,itr_scid,beta0,trkp,ifsim,beta,iflg)
        CALL dcbeta(ana,ilnk,ifsim,beta,iflg)

        IF(iflg.GT.0) GOTO 100
C
C Resolve the L-R ambiguity
        n_seg   = 0
        itr_new = 1
        DO 60 is = 1,nsuper
          IF(lprnt(5).EQ.1 .AND. is.EQ.1) WRITE(6,102)
          chibest = 1.E+8
          isgb    = 0
          improve = 0
          ilmin = (is-1)*nlay
          alfa = trk(10,ilmin+3,ilnk)
          icl = lnk_clust(is,ilnk)
          nsgc = nsegmc(icl,is,isec)
C
C Loop over segments in a linked cluster
C
          DO 50 isgc = 1,nsgc
            isg = clust(isgc,icl,is,isec)
C Store {iw,tdc} in "digi" array
            k = 1
            DO la = 1,nlay
              digi(1,la) = segm(k,  isg,is,isec)     ! wire id         
              digi(2,la) = segm(k+1,isg,is,isec)     ! raw tdc [cnts]
              k = k + 2
            ENDDO
C
C Get drift time

cc            CALL ana_dtime(itr_sect(ilnk),is,ilnk,itr_new,
cc     & itr_scid(1,ilnk),trk(1,1,ilnk),trkp(1,1,ilnk),beta,ifsim, digi)
            call dcdtime(ana, is, ilnk, itr_new, beta, digi);

C
C Get measured distance of closest approach
            CALL dcdocam(is,ilnk, digi) ! SDA version

***            CALL ana_docam1(is,ilnk, digi) ! CLAS OFFLINE version
C
C Resolve L-R ambiguity
            CALL dclramb(isec,is,digi,alfa, chib,slopD,dat0)
C
            IF(chib.LT.chibest) THEN
              chibest   = chib
              slopDbest = slopD
              dat0B = dat0
              lamin = 0
              DO la = 1,nlay
                digib(1,la) = digi(1,la)
                digib(2,la) = digi(2,la)
                digib(3,la) = digi(3,la)
                digib(4,la) = digi(4,la)
                digib(5,la) = digi(5,la)
                IF(digib(1,la).GT.0.) lamin = lamin + 1
              ENDDO
              isgb = isg
C Early termination (we prefer segments with maximum No of hits)
              IF(lamin.EQ.NLinSL(is) .AND. chibest.LE.zcut(6)) THEN
                dca_segm(is,isec,ilnk) = isgb
                GO TO 52 
              ENDIF  
            ENDIF                ! IF(chib.LT.chibest)
 50       CONTINUE               ! loop over segments in a SuperLayer
          dca_segm(is,isec,ilnk) = isgb
C
C Improve LR_amb by allowing to disable one layer in a track segment
          IF(chibest.GT.cut .AND. lamin.GT.3) THEN
            lab = 0
            DO 51 la = 1,nlay
              IF(digib(1,la).LT.0.) GO TO 51
              digib(1,la) = - digib(1,la)              ! disable the layer temp
              CALL dclramb(isec,is,digib,alfa, chib,slopD,dat0)        
              IF(chib.LT.chibest) THEN
                chibest   = chib
                slopDbest = slopD
                dat0B = dat0
                lab = la
              ENDIF
              digib(1,la) = ABS(digib(1,la))            ! re-enable the layer
 51         CONTINUE
            IF(lab.GT.0) THEN
              digib(1,lab) = -digib(1,lab)
              digib(4,lab) = -7.777
            ENDIF   
            improve = lab
          ENDIF                                         ! IF(chibest.GT.zcut(6))
C
 52       CONTINUE
          IF(lprnt(5).EQ.1) THEN
            WRITE(6,103) is,icl,isgb,(digib(1,i),i=1,6),improve
     1                  ,slopDbest,(digib(4,i),i=1,6),chibest
          ENDIF
C
C Check if L-R Ambiguity was successfully resolved
          IF(chibest.GT.20.) chibest = 19.5
          CALL HFILL(900+is, chibest,0., 1.)
          IF(chibest.GT.cut) GO TO 60
          n_seg = n_seg + 1
C
C Store info in "trk( , , )" array needed for final fit (with DT)
C
          IntSg(is,ilnk) = dat0B
          DO la = 1,nlay
            il = la + ilmin
            trk(1,il,ilnk) = digib(1,la)                 ! iw
            trk(2,il,ilnk) = digib(2,la)                 ! Raw tdc [cnts]]
            trk(3,il,ilnk) = digib(3,la)                 ! Td [ns]
            trk(4,il,ilnk) = digib(4,la)                 ! Dm [cm]
            trk(5,il,ilnk) = digib(5,la)                 ! SIGd [cm]
            IF(itrk0.EQ.ilnk) THEN
              trk(6,il,ilnk) = beta0                     ! Velocity = beta0 [c]
            ELSE
              trk(6,il,ilnk) = beta                      ! Velocity = beta  [c]
            ENDIF
            trk(11,il,ilnk) = 10*isec + Level            ! Analysis Level & Sect
            trk(12,il,ilnk) = 0.                         ! Status OK
            IF(trk(1,il,ilnk).LE.0.) THEN                ! Status notOK
              IF(trk(4,il,ilnk).EQ.-9.999) trk(12,il,ilnk) = 1.
              IF(trk(4,il,ilnk).EQ.-8.888) trk(12,il,ilnk) = 2.
              IF(trk(4,il,ilnk).EQ.-7.777) trk(12,il,ilnk) = 3.
            ENDIF
          ENDDO
 60     CONTINUE                      ! End of loop over the superlayers
C
C Count tracks with resolved L-R Ambiguity in six SuperLayers
        IF(n_seg.EQ.zcut(3)) THEN
          n_trk = n_trk + 1
          itr_level(ilnk) = Level
          itr_sect(ilnk)  = isec
        ENDIF
  100 CONTINUE                        ! End of loop over linked clusters
      ifail = n_trk
C
 999  CONTINUE
C
 101  FORMAT(//' LEVEL=',I2,'  Sector=',I2,'  itrk=',I2,'  (L/R Ambig)')        
 102  FORMAT(' SL CL isg    La1    La2    La3    La4    La5    La6  Chi2                                                        
     1       impr  SlopD')    
 103  FORMAT(/2I3,I4,6F7.0,12X,I5,F7.3/10X,6F7.3,E12.5)
      RETURN
      END
E 1
