h55772
s 00000/00000/00000
d R 1.2 01/11/19 19:02:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_dsect.F
e
s 00637/00000/00000
d D 1.1 01/11/19 19:02:36 boiarino 1 0
c date and time created 01/11/19 19:02:36 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_dsect(isec,sxy,x0,y0,
     &                     ntr_link,itr_sect,segm,lnk_segm)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : draw sector gometry, tracks and hits (dirty stuff)
C-
C-   Inputs  : isec -sector number
C-             sxy     -scale factor
C-             x0,y0   -position in HIGZ window of size 20x20 cm2
C-   Output  :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created   2-OCT-1991   Billy K. Leung
C-   Modified 15-DEC-1993   Bogdan B. Niczyporuk (for new vs. of SDA)
C-
C-
C-   Called by SDA_ENTER
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdadraw.inc"
C
C Subroutine parameters
      INTEGER isec,ntr_link,itr_sect(ntrmx),lnk_segm(6,ntrmx),
     &        segm(12,nsgmx,6,6)
      REAL sxy,x0,y0
C
C Local variables
      INTEGER nsup, nlay, max_nwire, max_nhit
      PARAMETER (nsup = 6, nlay = 6, max_nwire = 194, max_nhit = 36*192)
C
      INTEGER i,j,k, il,ip,iw,it,isc,ind,ig, nw,lmax,nwire,nwmin,nwmax
      INTEGER nplan,j_plane, nstepm, npoint, nhit, indb, ind_sc(2,48)
      INTEGER ifirst, ibgrd,ired,igreen,iblue,ipurp,ilblue, nslab(6) 
C
      REAL xmax, ymax, deltar,guardr, phi, degrad
      REAL phi_plane, x_plane, y_plane, delta_pl, xlast,ylast, xfirst
      REAL yfirst, phimin,phimax, phi25,d_out,dphi,Ndiv, phiort
      REAL x,y, x1,y1,x2,y2,x3,y3,x4,y4, dist,din,dout,d20, x00(3)
      REAL f_s, fox, rad, angle,angle1,angle2, sas,sac
C 
      REAL x_hit(mxstep), y_hit(mxstep)
      REAL x_bgrd(max_nhit), y_bgrd(max_nhit)
      REAL gwire_x(max_nwire), gwire_y(max_nwire)
C
      CHARACTER*2 legend
C
      EQUIVALENCE (sda_wc(2), xmax)
      EQUIVALENCE (sda_wc(4), ymax)
C
      DATA ifirst /1/
C
C
      IF(ifirst.EQ.1) THEN
        nslab(1) = sda_pln(18,nsc_min,isec)
        nslab(2) = sda_pln(18,nsc_min+1,isec)
        nslab(3) = sda_pln(18,nsc_min+2,isec)
        nslab(4) = sda_pln(19,nsc_min+2,isec)
        nslab(5) = sda_pln(18,nsc_min+3,isec)
        nslab(6) = sda_pln(19,nsc_min+3,isec)
        isc = 0
        DO ip = 1,6
          DO i = 1,nslab(ip)
            isc = isc + 1
            ind_sc(1,isc) = i
            ind_sc(2,isc) = ip
          ENDDO
        ENDDO
        ibgrd  = 1
        ired   = 2
        igreen = 3
        iblue  = 4
        ipurp  = 6
        ilblue = 7
        degrad = ACOS(-1.)/180.
        ifirst = 0
      ENDIF
C
C Set up a view box for the convenience of hard_copy output
      CALL ISPMCI(ibgrd)
      CALL ISPLCI(ibgrd)
      IF(ABS(sxy).NE.1.) THEN
        CALL IGBOX(0.,xmax,0.05,0.77*ymax)
        WRITE(legend(1:2),'(I2)') isec
        CALL IGTEXT(xmax*.05,ymax*.75,
     1    'SDA Program   Middle Plane View of the CLAS Detector'
     2    ,ymax/100.,0.,'L')
C
C X-/Y- axes for the mid-plane
        CALL IGAXIS(x0,16.,y0,y0,0.,1.,0,'AU')
        CALL IGTEXT(16.2,y0,'Xs  (beam)',ymax/50.,0,'L')
        CALL IGAXIS(x0,x0,y0,12.,0.,1.,0,'AU')
        CALL IGTEXT(x0,12.2,'Ys',ymax/50.,0.,'L')
      ENDIF
C
C
C Draw geometry of ST (Start Counter)
C
      IF(zbeam(1).EQ.0.) THEN
       x_hit(1) = sda_pln(14,2,isec)*ABS(sxy)+x0                       
       y_hit(1) = sda_pln(15,2,isec)*sxy + y0
       x_hit(2) = sda_pln(12,2,isec)*ABS(sxy)+x0
       y_hit(2) = sda_pln(13,2,isec)*sxy + y0
       x_hit(3) = sda_pln(12,3,isec)*ABS(sxy) + x0
       y_hit(3) = sda_pln(13,3,isec)*sxy + y0
       CALL IPL(3,x_hit,y_hit)
      ENDIF 
C 
C Draw geometry of DC 
C
      ind = 0
      DO i = 1,nsup
        il = (i-1)*nlay + 1 + nst_max
        deltaR = 0.866025*sda_pln(7,il+2,isec)*sda_pln(13,il+2,isec)
        guardr = sda_pln(7,il,isec) - deltaR
        IF (i.LT.3) THEN
          j = 0
          phi_plane = 0.
          DO WHILE (phi_plane.LT.90.*degrad)
            j = j + 1
            phi_plane = sda_pln(12,il,isec) + (j-1)*sda_pln(13,il,isec)
          ENDDO
          j_plane = j
          x_plane = guardr*COS(phi_plane) + sda_pln(1,il,isec)
          y_plane = guardr + sda_pln(2,il,isec)
          delta_pl= guardr*sda_pln(13,il,isec)
          nwire = sda_pln(14,il,isec)
          nwmin = sda_pln(15,il,isec)
          nwmax = sda_pln(14,il,isec)
          nw = 0
          DO j = 1,nwire
           IF(j.GE.nwmin .AND. j.LE.nwmax) THEN
             nw = nw + 1
             phi = sda_pln(12,il,isec) + (j-1)*sda_pln(13,il,isec)
             IF(phi.LT.90.*degrad) then
               gwire_x(nw) = guardr*COS(phi) + sda_pln(1,il,isec)
               gwire_y(nw) = guardr*SIN(phi) + sda_pln(2,il,isec)
             ELSE
               gwire_x(nw) = x_plane - (j - j_plane)*delta_pl
               gwire_y(nw) = guardr + sda_pln(2,il,isec)
             ENDIF
             gwire_x(nw) = gwire_x(nw)*ABS(sxy) + x0
             gwire_y(nw) = gwire_y(nw)*sxy + y0
           ENDIF
          ENDDO
          CALL IPL(nw, gwire_x,gwire_y)
          xlast = gwire_x(nw)
          ylast = gwire_y(nw)
          xfirst = gwire_x(1)
          yfirst = gwire_y(1)
          lmax = 5
          IF(i.EQ.1) lmax = 3
          guardr = sda_pln(7,il+lmax,isec) + deltaR
          nwmin = sda_pln(15,il+lmax,isec)
          nwmax = sda_pln(14,il+lmax,isec)
          nw = 0
          DO j = 1,nwire
           IF(j.GE.nwmin .AND. j.LE.nwmax) THEN
             nw = nw + 1
             phi=sda_pln(12,il+lmax,isec)+(j-1)*sda_pln(13,il+lmax,isec)
             IF(phi.LT.90.*degrad) then
               gwire_x(nw) = guardr*COS(phi) + sda_pln(1,il,isec)
               gwire_y(nw) = guardr*SIN(phi) + sda_pln(2,il,isec)
             ELSE
               gwire_x(nw) = x_plane - (j - j_plane)*delta_pl
               gwire_y(nw) = guardr + sda_pln(2,il,isec)
             ENDIF
             gwire_x(nw) = gwire_x(nw)*ABS(sxy) + x0
             gwire_y(nw) = gwire_y(nw)*sxy + y0
           ENDIF
          ENDDO
          CALL IPL(nw, gwire_x,gwire_y)
          gwire_x(2) = xfirst
          gwire_y(2) = yfirst
          CALL IPL(2, gwire_x,gwire_y)
          gwire_x(1) = gwire_x(nw)
          gwire_y(1) = gwire_y(nw)
          gwire_x(2) = xlast
          gwire_y(2) = ylast
          CALL IPL(2, gwire_x,gwire_y)
        ELSE
          DO j = 1,max_nwire
            phi = sda_pln(12,il,isec) + (j-1)*sda_pln(13,il,isec)
            gwire_x(j) = guardr*COS(phi) + sda_pln(1,il,isec)
            gwire_y(j) = guardr*SIN(phi) + sda_pln(2,il,isec)
            gwire_x(j) = gwire_x(j)*ABS(sxy) + x0
            gwire_y(j) = gwire_y(j)*sxy + y0
          ENDDO
          CALL IPL(max_nwire,gwire_x,gwire_y)
          xlast = gwire_x(max_nwire)
          ylast = gwire_y(max_nwire)
          xfirst = gwire_x(1)
          yfirst = gwire_y(1)
          guardr = sda_pln(7,il+lmax,isec) + deltaR
          DO j = 1,max_nwire
            phi=sda_pln(12,il+lmax,isec)+(j-1)*sda_pln(13,il+lmax,isec)
            gwire_x(j) = guardr*COS(phi) + sda_pln(1,il+lmax,isec)
            gwire_y(j) = guardr*SIN(phi) + sda_pln(2,il+lmax,isec)
            gwire_x(j) = gwire_x(j)*ABS(sxy) + x0
            gwire_y(j) = gwire_y(j)*sxy + y0
          ENDDO
          CALL IPL(max_nwire,gwire_x,gwire_y)
          gwire_x(2) = xfirst
          gwire_y(2) = yfirst
          CALL IPL(2,gwire_x,gwire_y)
          gwire_x(1) = gwire_x(max_nwire)
          gwire_y(1) = gwire_y(max_nwire)
          gwire_x(2) = xlast
          gwire_y(2) = ylast
          CALL IPL(2,gwire_x,gwire_y)
        ENDIF
      ENDDO
C
C Draw geometry of CC
C
      d_out  = sda_pln(8,ncc_min,isec)
      phi25  = ATAN2(sda_pln(5,ncc_min,isec),sda_pln(4,ncc_min,isec))
      phimin =  7.*degrad       
      phimax = 45.*degrad
      Ndiv   = 100.
C  
      dphi = (sda_pln(17,ncc_min,isec)-sda_pln(16,ncc_min,isec))/Ndiv 
      phi  = sda_pln(16,ncc_min,isec)
      ind  = 0
      k    = Ndiv + 1.
      DO i = 1,k
        ind  = ind + 1
        x = sda_pln(7,ncc_min,isec)*COS(phi) + sda_pln(1,ncc_min,isec)
        y = sda_pln(7,ncc_min,isec)*SIN(phi) + sda_pln(2,ncc_min,isec)
        x_hit(ind) = x*ABS(sxy) + x0
        y_hit(ind) = y*sxy      + y0
        phi = phi + dphi
      ENDDO
      CALL ISPLCI(ibgrd)
      CALL IPL(ind,x_hit,y_hit)
C
      x1  = x_hit(1)
      y1  = y_hit(1) 
      x2  = x_hit(ind)
      y2  = y_hit(ind) 
      d20 = d_out/COS(phi25-phimin)
      x3  = (d20*COS(phimin) + sda_pln(1,nec_min,isec))*ABS(sxy) + x0
      y3  = (d20*SIN(phimin) + sda_pln(2,nec_min,isec))*sxy + y0
      d20 = d_out/COS(phimax-phi25)
      x4  = (d20*COS(phimax) + sda_pln(1,nec_min,isec))*ABS(sxy) + x0
      y4  = (d20*SIN(phimax) + sda_pln(2,nec_min,isec))*sxy + y0
       x_hit(1) = x3
       x_hit(2) = x4
       y_hit(1) = y3
       y_hit(2) = y4
       CALL IPL(2,x_hit,y_hit) 
      x_hit(1) = x1
      x_hit(2) = x3
      y_hit(1) = y1
      y_hit(2) = y3
      CALL IPL(2,x_hit,y_hit) 
       x_hit(1) = x2
       x_hit(2) = x4
       y_hit(1) = y2
       y_hit(2) = y4
       CALL IPL(2,x_hit,y_hit)
C
C Draw geometry of SC
C
      DO isc = 1,6
        k = nslab(isc) + 1
        x_hit(1) = sc_grid(1,1,isc,isec)*ABS(sxy) + x0
        x_hit(2) = sc_grid(1,k,isc,isec)*ABS(sxy) + x0
        y_hit(1) = sc_grid(2,1,isc,isec)*sxy + y0
        y_hit(2) = sc_grid(2,k,isc,isec)*sxy + y0
        CALL IPL(2,x_hit,y_hit) 
        x_hit(1) = sc_grid(3,1,isc,isec)*ABS(sxy) + x0
        x_hit(2) = sc_grid(3,k,isc,isec)*ABS(sxy) + x0
        y_hit(1) = sc_grid(4,1,isc,isec)*sxy + y0
        y_hit(2) = sc_grid(4,k,isc,isec)*sxy + y0
        CALL IPL(2,x_hit,y_hit) 
C Draw division between slabs
        DO i = 1,k
          x_hit(1) = sc_grid(1,i,isc,isec)*ABS(sxy) + x0
          x_hit(2) = sc_grid(3,i,isc,isec)*ABS(sxy) + x0
          y_hit(1) = sc_grid(2,i,isc,isec)*sxy + y0
          y_hit(2) = sc_grid(4,i,isc,isec)*sxy + y0
          CALL IPL(2,x_hit,y_hit) 
        ENDDO 
      ENDDO
C
C Draw geometry of EC 
C
      DO ip = nec_min, nec_max
C Offset center
        x00(1) = sda_pln(1,ip,isec)  
        x00(2) = sda_pln(2,ip,isec)  
        x00(3) = sda_pln(2,ip,isec)
C
        din    = sda_pln(8,ip,isec)
        dout   = din + (sda_pln(19,ip,isec)
     1          +sda_pln(20,ip,isec))*sda_pln(16,ip,isec)              
        phiort = ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec))
        IF(ip.EQ.nec_min) THEN
          phimin = phiort - 20.*degrad
          phimax = phiort + 20.*degrad
        ELSE
          phimin = ATAN2(sda_pln(13,ip,isec),sda_pln(12,ip,isec))
          phimax = ATAN2(sda_pln(15,ip,isec),sda_pln(14,ip,isec))
        ENDIF
         dphi = ABS(phimin - phiort)
         x1 = (din *(COS(phimin)/COS(dphi)) + x00(1))*ABS(sxy) + x0
         y1 = (din *(SIN(phimin)/COS(dphi)) + x00(2))*    sxy  + y0
         x3 = (dout*(COS(phimin)/COS(dphi)) + x00(1))*ABS(sxy) + x0
         y3 = (dout*(SIN(phimin)/COS(dphi)) + x00(2))*    sxy  + y0
        dphi = ABS(phimax - phiort)
        x2 = (din *(COS(phimax)/COS(dphi)) + x00(1))*ABS(sxy) + x0
        y2 = (din *(SIN(phimax)/COS(dphi)) + x00(2))*    sxy  + y0
        x4 = (dout*(COS(phimax)/COS(dphi)) + x00(1))*ABS(sxy) + x0
        y4 = (dout*(SIN(phimax)/COS(dphi)) + x00(2))*    sxy  + y0
        IF(ip.EQ.nec_max .AND. isec.GT.2) GO TO 10       
          x_hit(1) = x1
          x_hit(2) = x2
          y_hit(1) = y1
          y_hit(2) = y2
          CALL IPL(2,x_hit,y_hit) 
          x_hit(1) = x3
          x_hit(2) = x4
          y_hit(1) = y3
          y_hit(2) = y4
          CALL IPL(2,x_hit,y_hit) 
          x_hit(1) = x1
          x_hit(2) = x3
          y_hit(1) = y1
          y_hit(2) = y3
          CALL IPL(2,x_hit,y_hit) 
          x_hit(1) = x2
          x_hit(2) = x4
          y_hit(1) = y2
          y_hit(2) = y4
          CALL IPL(2,x_hit,y_hit)
 10     CONTINUE
      ENDDO
C 
C Plot hits in DC (digitalization)
C
      indb = 0
      DO 11 i = 1,npl_dc
        nhit = dc_ndig(i,isec)
        IF(nhit.EQ.0) GO TO 11
        DO j = 1,nhit
          iw = dc_digi(1,j,i,isec)
          IF(indb.LT.max_nhit) THEN
            indb = indb + 1
            x_bgrd(indb) = dc_wpmid(1,iw,i,isec)*ABS(sxy) + x0
            y_bgrd(indb) = dc_wpmid(2,iw,i,isec)*sxy + y0
          ENDIF
        ENDDO
 11   CONTINUE
      IF (indb.GT.0) THEN
        CALL ISPMCI(ired)
        CALL ISMK(5)
        CALL IPM(indb,x_bgrd,y_bgrd)
      ENDIF





C
C Draw pattern recognition results
C
      do i = 1,ntr_link
        if(itr_sect(i).EQ.isec) then
c          print *,'isec=',isec
c          print *,'link=',lnk_segm(1,i),lnk_segm(2,i),lnk_segm(3,i),
c     &                    lnk_segm(4,i),lnk_segm(5,i),lnk_segm(6,i)
c          print *,'clust=',lnk_clust(1,i),lnk_clust(2,i),lnk_clust(3,i),
c     &                     lnk_clust(4,i),lnk_clust(5,i),lnk_clust(6,i)
c          print *,'segm=',segm(3,lnk_segm(1,i),1,isec),
c     &                    segm(3,lnk_segm(2,i),2,isec),
c     &                    segm(3,lnk_segm(3,i),3,isec),
c     &                    segm(3,lnk_segm(4,i),4,isec),
c     &                    segm(3,lnk_segm(5,i),5,isec),
c     &                    segm(3,lnk_segm(6,i),6,isec)

            x_hit(1)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(1,i),1,isec)),2,isec)*ABS(sxy)+x0
            x_hit(2)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(2,i),2,isec)),8,isec)*ABS(sxy)+x0
            x_hit(3)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(3,i),3,isec)),14,isec)*ABS(sxy)+x0
            x_hit(4)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(4,i),4,isec)),20,isec)*ABS(sxy)+x0
            x_hit(5)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(5,i),5,isec)),26,isec)*ABS(sxy)+x0
            x_hit(6)=
     & dc_wpmid(1,ABS(segm(3,lnk_segm(6,i),6,isec)),32,isec)*ABS(sxy)+x0

            y_hit(1)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(1,i),1,isec)),2,isec)*sxy+y0
            y_hit(2)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(2,i),2,isec)),8,isec)*sxy+y0
            y_hit(3)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(3,i),3,isec)),14,isec)*sxy+y0
            y_hit(4)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(4,i),4,isec)),20,isec)*sxy+y0
            y_hit(5)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(5,i),5,isec)),26,isec)*sxy+y0
            y_hit(6)=
     & dc_wpmid(2,ABS(segm(3,lnk_segm(6,i),6,isec)),32,isec)*sxy+y0

c      print *,'x_hit=',
c     &x_hit(1),x_hit(2),x_hit(3),x_hit(4),x_hit(5),x_hit(6)
c      print *,'y_hit=',
c     &y_hit(1),y_hit(2),y_hit(3),y_hit(4),y_hit(5),y_hit(6)

          call isplci(igreen)
          call ipl(6,x_hit,y_hit)

        endif
      enddo







C 
C Plot hits in SC (digitalization)
C
      CALL ISPLCI(ipurp)
      nhit = sc_ndig(1,isec)
      IF(nhit.EQ.0) GO TO 13
      DO 12 j = 1,nhit
        IF(sc_digi(2,j,1,isec).EQ.0.AND.sc_digi(4,j,1,isec).EQ.0) GOTO 12
        isc = sc_digi(1,j,1,isec)
        IF(isc.LT.1 .OR. isc.GT.48) GO TO 12
        ig = ind_sc(1,isc)
        ip = ind_sc(2,isc)
        x_hit(1) = sc_grid(1,ig  ,ip,isec)*ABS(sxy) + x0
        x_hit(2) = sc_grid(3,ig+1,ip,isec)*ABS(sxy) + x0
        y_hit(1) = sc_grid(2,ig  ,ip,isec)*sxy + y0
        y_hit(2) = sc_grid(4,ig+1,ip,isec)*sxy + y0
        CALL IPL(2,x_hit,y_hit) 
        x_hit(1) = sc_grid(3,ig  ,ip,isec)*ABS(sxy) + x0
        x_hit(2) = sc_grid(1,ig+1,ip,isec)*ABS(sxy) + x0
        y_hit(1) = sc_grid(4,ig  ,ip,isec)*sxy + y0
        y_hit(2) = sc_grid(2,ig+1,ip,isec)*sxy + y0
        CALL IPL(2,x_hit,y_hit) 
 12   CONTINUE
 13   CONTINUE
C
C Draw HBT trajectories (swimming)
C
c      print *,'ntrdraw0=',ntrdraw0
      DO it = 1,ntrdraw0
c      print *,'->',it,isec,draw0_isec(it)
        IF(isec.EQ.draw0_isec(it)) THEN
          nstepm = draw0_nstep(it)
          DO i = 1,nstepm
            npoint = i
            x_hit(npoint) = draw0_xyz(1,i,it)*ABS(sxy) +x0
            y_hit(npoint) = draw0_xyz(2,i,it)*sxy +y0
          ENDDO
          CALL ISPLCI(ired)
          CALL IPL(npoint,x_hit,y_hit)
          IF(nstepm.GE.1) THEN
            WRITE(legend(1:2),FMT='(I2)') it
            CALL IGTEXT(x_hit(nstepm*3/4),y_hit(nstepm*3/4),legend,
     1         20.*ABS(sxy),0.,'L')
          ENDIF
C
C Plot hits in 2xST, CC, 4xSC and 2xEC (total 9 planes)
c          Nplan = 9
c          ind = 0
c          DO 21 ip = 1,Nplan
c            IF(ip.EQ.Nplan .AND. isec.GT.2) GO TO 21       
c            IF(ip.LE.2 .AND. zbeam(1).NE.0.) GO TO 21       
c            IF(ip.GE.4 .AND. ip.LE.7) GO TO 21
c            IF(draw0_hit(2,ip,it).GT.999.) GO TO 21
c            IF(ip.LE.2) THEN
c              k = ip + 1
c            ELSE 
c              k = ip + 37          
c            ENDIF
c            ind = ind + 1
c            x_hit(ind) = draw0_hit(1,ip,it)*ABS(sxy) + x0
c            y_hit(ind) = draw0_hit(2,ip,it)*sxy      + y0
c 21      CONTINUE
c          IF(ind.GT.0) THEN
c            CALL ISPMCI(ipurp)
c            CALL ISMK(2)
c            CALL IPM(ind,x_hit,y_hit)
c          ENDIF
        ENDIF  ! IF(isec ...
      ENDDO    ! EndDo over ntrdraw0
C
C Draw final trajectories (swimming)
C
      DO it = 1,ntrdraw
        IF(isec.EQ.draw_isec(it)) THEN
          nstepm = draw_nstep(it)
          DO i = 1,nstepm
            npoint = i
            x_hit(npoint) = draw_xyz(1,i,it)*ABS(sxy) +x0
            y_hit(npoint) = draw_xyz(2,i,it)*sxy +y0
          ENDDO
          CALL ISPLCI(iblue)
          CALL IPL(npoint,x_hit,y_hit)
          IF(nstepm.GE.1) THEN
            WRITE(legend(1:2),FMT='(I2)') it
            CALL IGTEXT(x_hit(nstepm*3/4),y_hit(nstepm*3/4),legend,
     1         20.*ABS(sxy),0.,'L')
          ENDIF
C
C Plot hits in 2xST, CC, 4xSC and 2xEC (total 9 planes)
          Nplan = 9
          ind = 0
          DO 20 ip = 1,Nplan
            IF(ip.EQ.Nplan .AND. isec.GT.2) GO TO 20       
            IF(ip.LE.2 .AND. zbeam(1).NE.0.) GO TO 20       
            IF(ip.GE.4 .AND. ip.LE.7) GO TO 20
            IF(draw_hit(2,ip,it).GT.999.) GO TO 20
            IF(ip.LE.2) THEN
              k = ip + 1
            ELSE 
              k = ip + 37          
            ENDIF
            ind = ind + 1
            x_hit(ind) = draw_hit(1,ip,it)*ABS(sxy) + x0
            y_hit(ind) = draw_hit(2,ip,it)*sxy      + y0
 20       CONTINUE
          IF(ind.GT.0) THEN
            CALL ISPMCI(ipurp)
            CALL ISMK(2)
            CALL IPM(ind,x_hit,y_hit)
          ENDIF
C
        ENDIF  ! IF(isec ...
      ENDDO    ! EndDo over ntrdraw
C
C Draw Front View for the sector
C
      IF(ABS(sxy).NE.1.) THEN  ! Not drawing Front View for sxy=1.
        CALL ISPMCI(ibgrd)
        CALL ISPLCI(ibgrd)
        f_s    = 0.3                   ! Additional scale factor
        fox    = -300.*ABS(sxy)        ! X offset (no Y offset)
        dist   = 300.                  ! Dist from beam to the outline
C
        rad    = dist/cos(30.*degrad)
        angle  = (isec-1)*60.*degrad
        angle1 = ((isec-1)*60. - 30.)*degrad
        angle2 = ((isec-1)*60. + 30.)*degrad
        sas    = sin((isec-1)*60.*degrad)
        sac    = cos((isec-1)*60.*degrad)
        x_hit(1) = x0 + fox
        y_hit(1) = y0
        x_hit(2) = rad*cos(angle1)*f_s*ABS(sxy) + x0 + fox
        y_hit(2) = rad*sin(angle1)*f_s*ABS(sxy) + y0
        x_hit(3) = rad*cos(angle2)*f_s*ABS(sxy) + x0 + fox
        y_hit(3) = rad*sin(angle2)*f_s*ABS(sxy) + y0
        x_hit(4) = x0 + fox
        y_hit(4) = y0
        CALL IPL(4,x_hit,y_hit)
        x_hit(1) = 1.15*rad*cos(angle)*f_s*ABS(sxy) + x0 + fox
        y_hit(1) = 1.15*rad*sin(angle)*f_s*ABS(sxy) + y0
        WRITE(legend(1:2),FMT='(I2)') isec
        CALL IGTEXT(x_hit(1),y_hit(1),legend,
     1               30.*ABS(sxy),0.,'C')
C
C Draw final trajectories (FRONT VIEW)
C
        CALL ISPMCI(ilblue)
        CALL ISPLCI(ilblue)
        DO it = 1,ntrdraw
          IF(isec.EQ.draw_isec(it)) THEN
            nstepm = draw_nstep(it)
            DO i = 1,nstepm
              npoint = i
              x_hit(npoint) = draw_xyz(2,i,it)*sac -
     1                         draw_xyz(3,i,it)*sas
              y_hit(npoint) = draw_xyz(2,i,it)*sas +
     1                         draw_xyz(3,i,it)*sac
              x_hit(npoint) = x_hit(npoint)*f_s*ABS(sxy) + x0 + fox
              y_hit(npoint) = y_hit(npoint)*f_s*ABS(sxy) + y0
            ENDDO
            CALL IPL(npoint,x_hit,y_hit)
            IF(nstepm.GE.1) THEN
              WRITE(legend(1:2),FMT='(I2)') it
              CALL IGTEXT(x_hit(nstepm),y_hit(nstepm),legend,
     1          20.*ABS(sxy),0.,'L')
            ENDIF
          ENDIF
        ENDDO
      ENDIF
C
      CALL ISPMCI(ibgrd)
      CALL ISPLCI(ibgrd)
C
C Draw a scale factor equivalent to 100cm on the bottom left corner.
C
      x1 = 1.
      x2 = x1 + ABS(sxy)*100.
      IF(ABS(sxy).NE.1.) THEN
        CALL IGSET('TMSI',ymax/140)
        CALL IGAXIS(x1,x2,.2,.2,0.,100.,502,'US')
        CALL IGTEXT(x2,.4,'100 cm',ymax/140.,0.,'L')
      ENDIF
      IF(ABS(sxy).EQ.1.) THEN
        x1 = 300. + x0
        x2 = x1 + 100. + x0
        CALL IGSET('TMSI',20.)
        CALL IGAXIS(x1,x2,-550.+y0,-550.+y0,0.,100.,502,'US')
        CALL IGTEXT(x2,-530.+y0,'100 cm',20.,0.,'L')
C  X-/Y- axes 
        WRITE(legend(1:2),FMT='(I2)') isec
        CALL IGAXIS(x0+400.,x0+550.,y0,y0,0.,1.,0,'AU')
        CALL IGTEXT(x0+550.,y0-40.,'Z(X?S!)',30.,0.,'L')
        CALL IGAXIS(x0,x0,y0+sxy*400.,y0+sxy*550.,0.,1.,0,'AU')
        CALL IGTEXT(x0+15.,y0+sxy*550.,'Y?SECTOR '//legend//'!',
     1    30.,0.,'L')
      ENDIF
C
  999 CONTINUE
      RETURN
      END
E 1
