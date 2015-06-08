h46431
s 00000/00000/00000
d R 1.2 01/11/19 19:02:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_lramb.F
e
s 00326/00000/00000
d D 1.1 01/11/19 19:02:34 boiarino 1 0
c date and time created 01/11/19 19:02:34 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE ana_lramb(isec,is,digi,psi, chib,slopD,dat0B)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Resolves Left-Right AMBiguities for track
C-                         segment in a SuperLayer
C-
C-   Input:    isec       - sector number
C-             is         - superlaler number
C-             psi        - projection track angle w.r. to R (or normal to SL)
C-             digi       - hit info {iw,tdc,Td,-/+Dm,SigDm} for a SL
C-
C-   Outputs : chib       - best chi2 from all 4 solutions.
C-             slopD      - slope difference between track segment & PR fit
C-             dat0B      - strait line intercept at layer#1
C-
C-   Controls:
C-
C-
C-   Library belongs    : libana.a
C-
C-   Calls              : none
C-
C-   Created   26-OCT-1990   Bogdan Niczyporuk
C-   Modified  29-JULY-1992  Billy Leung
C-   Modified  23-FEB-1996   Bogdan Niczyporuk
C-
C-   Called by ana_dcam
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
C
      INTEGER nlay
      PARAMETER (nlay = 6)
C
C External functions
      INTEGER IBSET
      LOGICAL BTEST
C
C Subroutine parameters
      INTEGER isec, is
      REAL digi(5,nlay), psi, chib, slopD 
C
C Local variables
      INTEGER i, il,il3, istart, J1,J2,JW, J1old(nlay)
      INTEGER max_miss, nmiss, nw0, ilmin, NLinSL(6)        
      INTEGER LLL,LLR,LRL,LRR, LRB
C
      REAL xr(nlay), phw(nlay),dlw(nlay), wstg(nlay), w, Scut, SLcut 
      REAL dw1LL, dw2LL, dw1LR, dw2LR, dw1RL, dw2RL, dw1RR, dw2RR
      REAL fw0, fw1, fw2, fdet
      REAL chiLL, chiLR, chiRL, chiRR
      REAL dat0LL, dat0LR, dat0RL, dat0RR
      REAL drawLL, drawLR, drawRL, drawRR
      REAL dsegLL, dsegLR, dsegRL, dsegRR
      REAL predLL, predLR, predRL, predRR
      REAL sgnLL, sgnLR, sgnRL, sgnRR
      REAL slopLL, slopLR, slopRL, slopRR, slopTF, slopB,dat0B
      REAL cpsi, df, deti, ddist(nlay), derr(nlay), deltaR 
C
      LOGICAL miss(nlay)       ! inefficiency of any cause
C
      INTEGER min_nlay
      PARAMETER (min_nlay = 3) ! min. number of layers needed
      DATA NLinSL / 4, 6, 6, 6, 6, 6/
      DATA SLcut /0.10/
C
C
C LR_AMB begins
C
C Maximum inefficiency (in terms of #layer) possible.
      max_miss =  nlay - min_nlay
C Initialize chi-square best and second best to huge values.
      chib = 1000.
C When "istart" set to non-zero value will skip re-doing with
C different starting layers (see last IF block) even if bad chi2 results.
C
      istart = 1                              ! istart can be set to 2
C
      cpsi = COS(psi)
      slopTF = TAN(psi)
      Scut = SLcut*SQRT( FLOAT(nlay)/FLOAT(NLinSL(is)) )
C
C Local coordinate system where "xr" is X-axis along R of SL (or normal to SL)
C and "wstg", Y-axis along the layer (arc).
      ilmin = (is-1)*nlay + nst_max
      il3 = ilmin + 3
      deltaR = sda_pln(7,il3,isec) - sda_pln(7,il3-1,isec)
      DO i = 1,nlay
        il = ilmin + i
        w = ABS(digi(1,i))
        IF(w.GT.sda_pln(17,il,isec)) THEN
          phw(i) = sda_pln(12,il,isec) + (sda_pln(17,il,isec) - 1.)
     1            *sda_pln(13,il,isec)        
          dlw(i) = (w - sda_pln(17,il,isec))*2.*sda_pln(19,il3,isec)        
        ELSE
          phw(i) = sda_pln(12,il,isec) + (w - 1.)*sda_pln(13,il,isec)
          dlw(i) = 0.     
        ENDIF
        xr(i)   = FLOAT(i - 1)*deltaR
        wstg(i) = (phw(i) - phw(1))*sda_pln(7,il,isec) + dlw(i) - dlw(1)
        ddist(i)= ABS(digi(4,i))/cpsi
        derr(i) = digi(5,i)/cpsi
      ENDDO
C
C Remember the old starting layers, initially none.
      DO i = 1,nlay
        miss(i) = .FALSE.
        J1old(i) = 0
      ENDDO
C
   30 CONTINUE
C
C Find first 2 starting layers, total the no. of inefficient layers
C if no. of required layers met, J1 will be the first; J2, the second.
      nmiss = 0
      J1 = 0
      J2 = 0
      DO il = 1,nlay
        IF (digi(1,il).LE.0.) THEN   ! -ve no. mean inefficiency
          nmiss = nmiss + 1
          miss(il) = .TRUE.
        ELSE IF (J1old(il).NE.1) THEN
          IF (J1.EQ.0) THEN          ! Test if J1 is picked
            J1 = il
          ELSE
            IF (J2.EQ.0) THEN        ! Test if J2 is picked
              J2 = il
            ENDIF
          ENDIF
        ENDIF
      ENDDO
C
C Too many inefficient layers, leave subroutine
      IF ((nmiss.GT.max_miss) .OR. (J1.EQ.0) .OR. (J2.EQ.0)) THEN
        GO TO 999
      ENDIF
C
C Initialize four starting solutions (first 2 layers)
      nw0 = 2
      fw0 = nw0
      fw1 = xr(J1) + xr(J2)               ! Sum of the x's
      fw2 = xr(J1)*xr(J1) + xr(J2)*xr(J2) ! Sum of the x squares
C
      dw1LL = -ddist(J1)+wstg(J1)           -ddist(J2)+wstg(J2)
      dw2LL =(-ddist(J1)+wstg(J1))*xr(J1) +(-ddist(J2)+wstg(J2))*xr(J2)
      dw1LR = -ddist(J1)+wstg(J1)           +ddist(J2)+wstg(J2)
      dw2LR =(-ddist(J1)+wstg(J1))*xr(J1) +( ddist(J2)+wstg(J2))*xr(J2)
      dw1RL =  ddist(J1)+wstg(J1)           -ddist(J2)+wstg(J2)
      dw2RL =( ddist(J1)+wstg(J1))*xr(J1) +(-ddist(J2)+wstg(J2))*xr(J2)
      dw1RR =  ddist(J1)+wstg(J1)           +ddist(J2)+wstg(J2)
      dw2RR =( ddist(J1)+wstg(J1))*xr(J1) +( ddist(J2)+wstg(J2))*xr(J2)
C
C We have two hits to get started, solve lin. regress.
      fdet = fw0*fw2-fw1*fw1
      IF(fdet.LE.0.) GO TO 999
      deti = 1./fdet
      dat0LL = ( dw1LL*fw2 - dw2LL*fw1)*deti
      slopLL = (-dw1LL*fw1 + dw2LL*fw0)*deti
      dat0LR = ( dw1LR*fw2 - dw2LR*fw1)*deti
      slopLR = (-dw1LR*fw1 + dw2LR*fw0)*deti
      dat0RL = ( dw1RL*fw2 - dw2RL*fw1)*deti
      slopRL = (-dw1RL*fw1 + dw2RL*fw0)*deti
      dat0RR = ( dw1RR*fw2 - dw2RR*fw1)*deti
      slopRR = (-dw1RR*fw1 + dw2RR*fw0)*deti
C
C Loop over remaining wires, picking up hits for segment.
      DO 100 JW = 1,nlay
        IF(digi(1,JW).LE.0.) GO TO 100
        IF(JW.EQ.J1 .OR. JW.EQ.J2) GO TO 100
C
C Determine predicted distance, using dat0 and slop.
        predLL = dat0LL + slopLL*xr(JW) - wstg(JW)
        sgnLL  = SIGN(1.,predLL)
        predLR = dat0LR + slopLR*xr(JW) - wstg(JW)
        sgnLR  = SIGN(1.,predLR)
        predRL = dat0RL + slopRL*xr(JW) - wstg(JW)
        sgnRL  = SIGN(1.,predRL)
        predRR = dat0RR + slopRR*xr(JW) - wstg(JW)
        sgnRR  = SIGN(1.,predRR)
C
        nw0  = nw0 + 1
        fw0  = nw0
        fw1  = fw1 + xr(JW)          ! Sum of the x's
        fw2  = fw2 + xr(JW)*xr(JW)   ! Sum of the x squares
C
        dsegLL = sgnLL*ddist(JW) + wstg(JW)
        dw1LL  = dw1LL + dsegLL
        dw2LL  = dw2LL + dsegLL*xr(JW)
        dsegLR = sgnLR*ddist(JW) + wstg(JW)
        dw1LR  = dw1LR + dsegLR
        dw2LR  = dw2LR + dsegLR*xr(JW)
        dsegRL = sgnRL*ddist(JW) + wstg(JW)
        dw1RL  = dw1RL + dsegRL
        dw2RL  = dw2RL + dsegRL*xr(JW)
        dsegRR = sgnRR*ddist(JW) + wstg(JW)
        dw1RR  = dw1RR + dsegRR
        dw2RR  = dw2RR + dsegRR*xr(JW)
C
C If we have more than one hit in seg, solve lin. regress.
        fdet = fw0*fw2 - fw1*fw1
        IF(fdet.LE.0.) GO TO 100       ! avoid divding by delta=0
        deti = 1./fdet
        dat0LL = ( dw1LL*fw2 - dw2LL*fw1)*deti
        slopLL = (-dw1LL*fw1 + dw2LL*fw0)*deti
        dat0LR = ( dw1LR*fw2 - dw2LR*fw1)*deti
        slopLR = (-dw1LR*fw1 + dw2LR*fw0)*deti
        dat0RL = ( dw1RL*fw2 - dw2RL*fw1)*deti
        slopRL = (-dw1RL*fw1 + dw2RL*fw0)*deti
        dat0RR = ( dw1RR*fw2 - dw2RR*fw1)*deti
        slopRR = (-dw1RR*fw1 + dw2RR*fw0)*deti
  100 CONTINUE
C
C Check if min. required layers is met
      IF (nw0.LT.min_nlay) GO TO 999
C
C Get left/right ambig resol for the four solutions.
      chiLL = 0.
      chiLR = 0.
      chiRL = 0.
      chiRR = 0.
      LLL  = 0
      LLR  = 0
      LRL  = 0
      LRR  = 0
C
C "Lxx" has its first 6 bits set to one if the xx solution
C ends up on the RIGHT-hand side of the hit wire for each of the 6 layers.
      DO JW=1,nlay
        IF(.NOT.miss(JW)) THEN
          predLL = dat0LL + slopLL*xr(JW) - wstg(JW)
          IF(predLL.GT.0.) LLL = IBSET(LLL,JW-1)
          drawLL = SIGN(ddist(JW),predLL)
          chiLL = chiLL + ((drawLL - predLL)/derr(JW))**2
C
          predLR = dat0LR + slopLR*xr(JW) - wstg(JW)
          IF(predLR.GT.0.) LLR = IBSET(LLR,JW-1)
          drawLR = SIGN(ddist(JW),predLR)
          chiLR = chiLR + ((drawLR - predLR)/derr(JW))**2
C
          predRL = dat0RL + slopRL*xr(JW) - wstg(JW)
          IF(predRL.GT.0.) LRL = IBSET(LRL,JW-1)
          drawRL = SIGN(ddist(JW),predRL)
          chiRL = chiRL + ((drawRL - predRL)/derr(JW))**2
C
          predRR = dat0RR + slopRR*xr(JW) - wstg(JW)
          IF(predRR.GT.0.) LRR = IBSET(LRR,JW-1)
          drawRR = SIGN(ddist(JW),predRR)
          chiRR = chiRR + ((drawRR - predRR)/derr(JW))**2
        ENDIF
      ENDDO
C
C Normalize chi-square's by dividing by number of degrees of
C freedom for all four solutions, and correcting for the slope.
      df = nw0 - 2
      chiLL = chiLL/df
      chiLR = chiLR/df
      chiRL = chiRL/df
      chiRR = chiRR/df
C
C Record the slope differece with the best solution
      IF (ABS(slopLL-slopTF).GT.Scut) chiLL = 100000.+chiLL
      IF (ABS(slopLR-slopTF).GT.Scut) chiLR = 100000.+chiLR
      IF (ABS(slopRL-slopTF).GT.Scut) chiRL = 100000.+chiRL
      IF (ABS(slopRR-slopTF).GT.Scut) chiRR = 100000.+chiRR
C
C Pick the solution with the least chi-square
      chib = MIN(chiLL,chiLR,chiRL,chiRR)
****      print *,'lramb: sec,is=',isec,is,' hi2=',chiLL,chiLR,chiRL,chiRR,'->',chib
C
C Save the set bits for the best solution and then pick the
C solution with the second best chi-square.
      IF(ABS(chib - chiLL).LT.0.00001) THEN
        LRB   = LLL
        slopB = slopLL
        dat0B = dat0LL*cpsi
      ELSE IF(ABS(chib - chiLR).LT.0.00001) THEN
        LRB   = LLR
        slopB = slopLR
        dat0B = dat0LR*cpsi
      ELSE IF(ABS(chib - chiRL).LT.0.00001) THEN
        LRB   = LRL
        slopB = slopRL
        dat0B = dat0RL*cpsi
      ELSE IF(ABS(chib - chiRR).LT.0.00001) THEN
        LRB   = LRR
        slopB = slopRR
        dat0B = dat0RR*cpsi
      ELSE
      ENDIF
      slopD = slopB - slopTF
C
C Mark the sign to indicate LR, "-" => Left of the wire.
      DO i=1,nlay
        IF (.NOT.miss(i)) THEN
          IF( BTEST(LRB,i-1) )THEN
            digi(4,i) =  ABS(digi(4,i))
          ELSE
            digi(4,i) = -ABS(digi(4,i))
          ENDIF
        ENDIF
      ENDDO
C
C If chi-square > 7.0 (CL < 1%), re-do LR_amb with 2 diff. starting layers
      IF(chib.LE.zcut(6)) GO TO 999
C
C The layers J1 & J2 failed, get started with other two layers,
      IF(istart.GT.0) THEN
        J1old(J1) = 1
        J1old(J2) = 1
C Reset "istart" only to re-do once with another 2 starting layers.
        istart = istart - 1
        DO il = 1, nlay
          miss(il) = .FALSE.
        ENDDO
        GO TO 30
      ENDIF
C
  999 RETURN
      END
E 1
