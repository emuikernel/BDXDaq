h29185
s 00000/00000/00000
d R 1.2 01/11/19 19:01:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/ana_docam1.F
e
s 00183/00000/00000
d D 1.1 01/11/19 19:01:58 boiarino 1 0
c date and time created 01/11/19 19:01:58 by boiarino
e
u
U
f e 0
t
T
I 1
      SUBROUTINE ana_docam1(is,itr, digi, trk,trkp,itr_sect)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates Distance to the hit wire
C-
C-
C-   Input   :  is          - SuperLayer number
C-              itr         - Track candidate number 
C-              digi(1,la)  - Wire number "iw"
C-              digi(2,la)  - Raw TDC [counts]
C-              digi(3,la)  - Drift Time Td [ns] to the hit wire# "iw"
C-
C-   Outputs :  digi(4,la)  - Drift Distance (measured) Dm [cm]
C-              digi(5,la)  - Sigma of Drift Distance [cm]
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   27-FEB-1996  Bogdan Niczyporuk
C-
C-   Called by ana_dcam
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
#include "sdakeys.inc"
#include "sdacalib.inc"
C
      INTEGER nlay
      PARAMETER (nlay = 6)       ! Maximum layers in SuperLayer
C
C Subroutine parameters
      INTEGER is, itr, itr_sect(ntrmx)
      REAL digi(5,nlay), trk(12,36,ntrmx), trkp(12,nplane,ntrmx)
C
C Local variables
      INTEGER il, la, ig, il1
      REAL slope, xdrift
C
      real  xyz(3), B(3), Babs, beta, alfa
      INTEGER IUCOMP, ifirst, iw
      DATA ifirst /1/
     
      if(ifirst.EQ.1) then
         ifirst=0
         if(IUCOMP('XVST',lcali,10).GT.0) then
c            print *,'ana_docam',ltrig(1),ievt
            call dc_xvst_init(ltrig(1))
c            call dc_fill_t2x_table
            ifirst=-1
         endif
      endif

      digi(3,la)=digi(3,la)+zkine(13+(is-1)/2)
      if(ifirst.LT.0) goto 1000
C
C Here is the option of constant velocity in a drift cell (the old way)
C
      IF(zderr(9) .EQ. 1.) THEN
        DO 5 la = 1,nlay
          digi(4,la) = -9.999
          digi(5,la) = zderr(is+1)
          IF(digi(1,la).LE.0.) GO TO 5
          digi(4,la) = digi(3,la)*vgas
 5      CONTINUE
        GO TO 999
      ENDIF
C
C Convert the Drift Time into the Drift Distance:  Dm = f(Td)
C
      DO 10 la = 1,nlay
        il = (is-1)*6 + la
        digi(4,la) = -9.999
C Discard bad or nonexisting wires
        IF(digi(1,la).LE.0.) GO TO 10
C Check Drift Time range: {0 - Tmax}
        IF(digi(3,la).LE.-30. .OR. digi(3,la).GE.td_h(2,il)*1.2) THEN
          digi(1,la) = -digi(1,la)
          digi(4,la) = -8.888
          GO TO 10
        ENDIF
        IF(digi(3,la).GT.-30. .AND. digi(3,la).LE.0.) THEN
          xdrift = -digi(3,la)
c          digi(3,la) = xdrift
        ELSEIF(digi(3,la).GE.td_h(2,il)) THEN
          xdrift = td_h(2,il)*0.995
        ELSE
          xdrift = digi(3,la)
        ENDIF
C Get Drift Distance       
        il1=il
        if(td_h(1,is*6).eq.0) then
          if(digi(1,la).gt.60) then 
            il1=(is-1)*6+4
          else
            il1=(is-1)*6+3
          endif
        endif
        ig = xdrift/(td_f(1,2,il1)-td_f(1,1,il1)) + 1.
        slope = (td_f(2,ig+1,il1) - td_f(2,ig,il1))
     1         /(td_f(1,ig+1,il1) - td_f(1,ig,il1))
        digi(4,la)=td_f(2,ig,il1)+
     2             slope*(digi(3,la)-td_f(1,ig,il1)+td_f(1,1,il1))
        if(digi(4,la).lt.0) digi(4,la)=0.
C Get Drift Distance error
        digi(5,la) = zderr(is+1)
 10   CONTINUE
C
  999 RETURN

 1000 continue
      Babs=0.
      B(2)=0.
      call ana_betaw(itr,beta,ig)
      alfa=trk(10,is*6-3,itr)
      DO la = 1,nlay
        il = (is-1)*6 + la
        iw = digi(1,la)
        if(is.eq.3 .or. is.eq.4) then
          if(mgtyp.eq.5) then
            B(2)=0.0
            Babs=0.0
          else
            xyz(1) = trkp(2,il+3,itr)
            xyz(2) = trkp(3,il+3,itr)
            xyz(3) = trkp(1,il+3,itr)
            call sda_snake(xyz,B)
            Babs=SQRT(B(1)*B(1)+B(2)*B(2)+B(3)*B(3))
          endif
        endif
        digi(4,la) = -9.999
        digi(5,la) = 100.
C Discard bad or nonexisting wires
        IF(iw.GT.0) then
c          print *,' sl',is,il,iw,trkp(9,1,itr),beta,alfa,babs,b
          call dc_time_to_dist(itr_sect(itr),il,iw,beta,alfa,
     &              digi(3,la),Babs,B(2),digi(4,la),digi(5,la))
c          print *,' d',il,(digi(ig,la),ig=1,5)
          digi(5,la)=digi(5,la)+zkine(16+(is-1)/2)
        endif
      enddo
      END

C======================================================================
c---------------------------
      subroutine rernev(irn,ev,irec)

      implicit none

#include "sdakeys.inc"
      integer irn,ev,irec

      irec=0
      irn=Run
      ev=ievt
      end

      subroutine recmes(rname,clev,rmess)
      character*(*) rname,clev,rmess
      if(clev(1:1).NE.'I') then
         print *,rmess
      endif
      end

      subroutine revinm(cevar,cfil,cfilout)
      character*(*) cevar,cfil,cfilout
      call getenv(cevar,cfilout)
      strlen=LNBLNK(cfilout)
      if(strlen.gt.0) then
         cfilout=cfilout(1:strlen)//'/'//cfil
      else
         print *,' env. var not defined',cevar
         cfilout=cfil
      endif
      print *,cfilout
      end
E 1
