h39762
s 00000/00000/00000
d R 1.2 01/11/19 19:02:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_ecid.F
e
s 00332/00000/00000
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
      SUBROUTINE ana_ecid(itr, iflg)
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
C-   Created   30-SEP-1996  Bogdan Niczyporuk
C-
C-   Called by ana_finde
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
#include "sdaanal.inc"
#include "sdacalib.inc"
C
C External
      INTEGER iucomp
C
C Subroutine parameters
      INTEGER itr, iflg
C
C Local variables
      INTEGER i,j, ih, ic, nc, nh, il, ip, isec, isl, ifirst, NoShSim
      INTEGER su,sv,sw,Vtrk, du,dv,dw,dif, slab_dif, Ucl,Vcl,Wcl
      INTEGER digi(36,6), Evis(36,3), Etot, Acl_thr, nsmin, nsmax
      INTEGER fslab,nslab, p1s,p2s, sumE, Epeak, ncl(3), Eclust(5,18,3)
      INTEGER UVWdif(18,3), suvw(3)              
      REAL Et, Erat, Evis_cut, p
C
      EQUIVALENCE (suvw(1),su)
C
      DATA ifirst   /1/
      DATA Acl_thr  /10/   ! visible energy threshold [MeV]
      DATA slab_dif /3/    ! Difference between SLABclust and SLABtrk
      DATA Evis_cut /0.10/ ! Cut on Evis/Ptrk
C 
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        ip     = nec_min
        NoShSim = 0
        IF(iucomp(4HSIM ,levb,10).NE.0 .AND. zcut(5).EQ.0.) NoShSim = 1          
      ENDIF
C
C
C Clear arrays
C
      iflg = 2
      DO il = 1,6
        DO isl = 1,36
          digi(isl,il) = 0
        ENDDO
      ENDDO
C
C Check boundaries of EC triangle at distance of "d+dd" from origin
C
      IF(lanal(1).LT.2) THEN
        isec   = ev_out(11,itr)/10.
        sw = ev_out(36,itr)/10000.
        sv =(ev_out(36,itr) - sw*10000.)/100.
        su = ev_out(36,itr) - sw*10000. - sv*100.
      ELSE
        isec = itr_sect(itr)
        su = itr_ecid(1,itr)
        sv = itr_ecid(2,itr)
        sw = itr_ecid(3,itr)
      ENDIF
      IF(su*sv*sw .EQ. 0) GOTO 999
C
C The branch for Acceptance calculation without Shower simulation
      IF(NoShSim.EQ.1) THEN
        IF(su.LT.2 .OR. su.GT.35) GOTO 999
        IF(sv.LT.2 .OR. sv.GT.35) GOTO 999
        IF(sw.LT.2 .OR. sw.GT.35) GOTO 999
        iflg = 1
        p = trkp(7,ip,itr)
        IF(lanal(1).LT.2) p = ev_out(7,itr)
        Et = 1000.*p*0.2/3.
C Store dummy info so that analysis will not corrupt
        DO il = 1,3
          e_clust(1,il) = suvw(il)
          e_clust(2,il) = 1
          e_clust(3,il) = suvw(il)
          e_clust(4,il) = 0
          e_clust(5,il) = Et
          e_clust(6,il) = 0
        ENDDO
      ENDIF
C
      iflg = 1
C
C Unpack the "ec_digi" array
C 
      DO 10 il = 1,6
        nh = ec_ndig(il,isec)
        IF(nh.EQ.0) GOTO 10
        DO ih = 1,nh
          isl  = ec_digi(1,ih,il,isec)
          digi(isl,il) = (ec_digi(3,ih,il,isec) -cal_ec(6,isl,il,isec))
     1                   /cal_ec(3,isl,il,isec)
          IF(digi(isl,il).LT.0) digi(isl,il) = 0
        ENDDO
 10   CONTINUE
C
C Sum energy (forward + backward)
      Etot = 0
      DO il = 1,3
        DO isl = 1,36
          Evis(isl,il) = digi(isl,il) + digi(isl,il+3)
          Etot = Etot + Evis(isl,il)
        ENDDO
      ENDDO
C
C Make a cut on the total visible energy in that sector
      IF(mgtyp.EQ.5 .OR. ABS(zmagn(2)).LT.0.01) THEN
        p = 1.0
      ELSE
        p = trkp(7,ip,itr)
        IF(lanal(1).LT.2) p = ev_out(7,itr)
      ENDIF
C
C Find energy clusters in three views {u,v,w}
C
      DO il = 1,3
        nc    = 0
        fslab = 0
        nslab = 0
        p1s  = 0
        p2s  = 0
        sumE  = 0
        DO isl = 1,36
           IF(Evis(isl,il).GT.0) THEN
             IF(fslab.EQ.0) THEN
               fslab = isl
               p1s  = isl
               Epeak = Evis(isl,il)
             ENDIF
             nslab = nslab + 1
             IF(Evis(isl,il).GT.Epeak) THEN
               p1s  = isl
               Epeak = Evis(isl,il)
             ENDIF 
             sumE = sumE + Evis(isl,il)
             IF(isl.EQ.36 .OR. Evis(isl+1,il).EQ.0) THEN
               nc = nc + 1
               Eclust(1,nc,il) = fslab
               Eclust(2,nc,il) = nslab
               Eclust(3,nc,il) = p1s
               Eclust(4,nc,il) = 0
               Eclust(5,nc,il) = sumE
               fslab = 0
               nslab = 0
               p1s  = 0
               sumE  = 0
             ENDIF               
           ENDIF
        ENDDO
        ncl(il) = nc
C Now search for 2-nd peak (p2s)
        DO 12 i = 1,ncl(il)
           p1s  = Eclust(3,i,il)
           nsmin = Eclust(1,i,il)
           nsmax = nsmin + Eclust(2,i,il) - 1
           Epeak = 0
           DO 11 isl = nsmin,nsmax
             IF(IABS(isl-p1s).LT.2) GO TO 11
             IF(Evis(isl,il).GT.Epeak) THEN
               Epeak = Evis(isl,il)
               p2s  = isl
             ENDIF
 11        CONTINUE
           IF(p2s.LT.2 .OR. p2s.GT.35) THEN
             p2s = 0
             GO TO 12
           ENDIF
           IF(Evis(p2s-1,il).GT.Epeak.OR.Evis(p2s+1,il).GT.Epeak) THEN                       
             p2s = 0
             GO TO 12
           ENDIF
           Eclust(4,i,il) = p2s                     
           p2s = 0
 12     CONTINUE
      ENDDO                                       ! End DO over layers
      IF(ncl(1)*ncl(2)*ncl(3) .EQ. 0) GO TO 999 
C
C Track matching with clusters
C
C U-view
      du = 99
      Ucl= 0
      DO 20 ic = 1,ncl(1)
        UVWdif(ic,1) = 99
        IF(Eclust(5,ic,1).LT.Acl_thr) GO TO 20
        DO j = 3,4
          IF(Eclust(j,ic,1).GT.1 .AND. Eclust(j,ic,1).LT.36) THEN
            dif = Eclust(j,ic,1) - su
            UVWdif(ic,1) = dif
            IF(IABS(dif).LT.IABS(du)) THEN
              du = dif
              Ucl= ic
            ENDIF
          ENDIF
        ENDDO
 20   CONTINUE
      IF(Ucl.EQ.0) GO TO 999        
      UVWdif(Ucl,1) = du
C V-view
      dv = 99
      Vcl= 0
      DO 30 ic = 1,ncl(2)
        UVWdif(ic,2) = 99
        IF(Eclust(5,ic,2).LT.Acl_thr) GO TO 30
        DO j = 3,4
          IF(Eclust(j,ic,2).GT.1 .AND. Eclust(j,ic,2).LT.36) THEN
            dif = Eclust(j,ic,2) - sv
            UVWdif(ic,2) = dif
            IF(IABS(dif).LT.IABS(dv)) THEN
              dv = dif
              Vcl= ic
            ENDIF
          ENDIF
        ENDDO
 30   CONTINUE        
      IF(Vcl.EQ.0) GO TO 999        
      UVWdif(Vcl,2) = dv
C W-view
      dw = 99
      Wcl= 0
      DO 40 ic = 1,ncl(3)
        UVWdif(ic,3) = 99
        IF(Eclust(5,ic,3).LT.Acl_thr) GO TO 40
        DO j = 3,4
          IF(Eclust(j,ic,3).GT.1 .AND. Eclust(j,ic,3).LT.36) THEN
            dif = Eclust(j,ic,3) - sw
            UVWdif(ic,3) = dif
            IF(IABS(dif).LT.IABS(dw)) THEN
              dw = dif
              Wcl= ic
            ENDIF
          ENDIF
        ENDDO
 40   CONTINUE 
      IF(Wcl.EQ.0) GOTO 999        
      UVWdif(Wcl,3) = dw
C
C Check track-cluster matching
      IF(IABS(du).GT.slab_dif) GOTO 999       
      IF(IABS(dv).GT.slab_dif) GOTO 999       
      IF(IABS(dw).GT.slab_dif) GOTO 999       
C Check energy ratio
      Et  = Eclust(5,Ucl,1) + Eclust(5,Vcl,2) + Eclust(5,Wcl,3)
      Erat= Et/(1000.*p)            
      IF(Erat .LT. Evis_cut) GOTO 999
      iflg = 0
C
C Store the results
C
      DO il =1,3
        IF(il.EQ.1) THEN
          ic = Ucl
          e_clust(6,il) = du
        ENDIF
        IF(il.EQ.2) THEN
          ic = Vcl
          e_clust(6,il) = dv
        ENDIF
        IF(il.EQ.3) THEN
          ic = Wcl
          e_clust(6,il) = dw
        ENDIF
        DO i = 1,5
         e_clust(i,il) = Eclust(i,ic,il)
        ENDDO
      ENDDO
C
 999  CONTINUE
C
C Debugging Printout
C
      IF(lprnt(4).EQ.2) THEN
        WRITE(6,101) isec,itr, Etot
        WRITE(6,102)
        DO 60 isl = 1,36
          WRITE(6,103) isl, Evis(isl,1), Evis(isl,2), Evis(isl,3)
 60     CONTINUE
        DO il = 1,3
          IF(il.EQ.1) THEN
            WRITE(6,*)
            WRITE(6,*) ' U-view     Uf   Nu  Up1  Up2  EcU   Utrk   dU'        
            Vtrk = su
          ENDIF
          IF(il.EQ.2) THEN
            WRITE(6,*)
            WRITE(6,*) ' V-view     Vf   Nv  Vp1  Vp2  EcV   Vtrk   dV'
            Vtrk = sv
          ENDIF
          IF(il.EQ.3) THEN
            WRITE(6,*)
            WRITE(6,*) ' W-view     Wf   Nw  Wp1  Wp2  EcW   Wtrk   dW'
            Vtrk = sw
          ENDIF
          DO ic = 1,ncl(il)
            WRITE(6,104) ic, (Eclust(i,ic,il),i=1,5), Vtrk,UVWdif(ic,il)        
          ENDDO
        ENDDO
 101    FORMAT(/'  isec =',I2,'  itr =',I3,'  Etot =',I5,'  (e ID)')              
 102    FORMAT(/'  isl       U       V       W')
 103    FORMAT(I5,3I8)
 104    FORMAT(I5,5X,5I5,I7,I5)
      ENDIF
C
      RETURN
      END
E 1
