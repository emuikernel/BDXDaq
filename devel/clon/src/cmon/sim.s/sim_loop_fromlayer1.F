C
C road simulation loop - Sergey Boyarinov
C
      SUBROUTINE SIM_LOOP_fromlayer1(IW,ifevb,inter,Nevents,iwmin,iwmax,pmin,pmax,
     &           charge,thetamin,thetamax)
C
      IMPLICIT NONE
C
#include "sdageom.inc"
#include "sdaevgen.inc"
C
C Parameters
      INTEGER IW(1000),iwmin,iwmax,ifevb,inter,Nevents,ievent
      real pmin,pmax,charge,thetamin,thetamax
C
C Local variables
      integer iw_, ip, ipmax, itmp, ipd, getimom
      real p_, x_, y_, z_, theta_, phi_
      real x1,x2,z1,z2,a,b,getmom
      real ymin,ymax,zmin,zmax
      real phimin,phimax
      real dz,dtheta,theta1
C
      INTEGER i,iwl3,iwthe(6),iwtheold(6),icomp6,ifail,ifirst
      INTEGER iwphi(6),iwphiold(6),iwy1(6),iwy2(6),iwp1(6),iwp2(6)
      INTEGER iwz1(6),iwz2(6)
      REAL p,theta,y,th, ph,pi,degrad,raddeg,dy,dphi
C
      DATA dy/3.0/ ! 3.0 ! along wire resolution of the pattern recognition is about 3.7 cm
      DATA dphi/5.0/ ! 5.0 ! phi resolution of the pattern recognition is about 7.6 degree
      DATA ifirst /1/
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        pi     = ACOS(-1.)
        degrad = pi/180.
        raddeg = 180./pi
      ENDIF

C
ccccc      call dchbook ! ERROR, dchbook has a parameter !!!!!!!!

C from sda_run: do we need it ?
      CALL dcbrun(IW)

C just in case
      CALL ETLDROP(IW,'E')
      CALL ETNGARB(IW)

ccc      CALL timex(time(9)) ! Get initial time
C*********************************************************************
C tracks starts from layer 1
C*********************************************************************

C==============================================================================
C
C                 DC Wire position, Wire Direction & WireLen for layers: 1 - 36
C dc_wpmid(1,iw,il,isec) | along beam     |
C dc_wpmid(2,iw,il,isec) | vertical       |-> Wire position in the middle plane
C dc_wpmid(3,iw,il,isec) | along wire (0) |
C dc_wlen(1,iw,il,isec)  --> Half wire length (+z or +y AMPL,Reg.1 opposit)
C
C==============================================================================

c take IWmin from the parameter list
        iw_ = iwmin - 1

cccccccccccccccccccccccc
c loop over wire number
cccccccccccccccccccccccc

        do while(iw_.LT.iwmax)
          iw_ = iw_ + 1
          print *,'wire=',iw_
c coordinate along the beam
          z2 = dc_wpmid(1,iw_  ,1,1)-0.01
          z1 = dc_wpmid(1,iw_+1,1,1)-0.01
c vertical coordinate
          x2 = dc_wpmid(2,iw_  ,1,1)-0.01
          x1 = dc_wpmid(2,iw_+1,1,1)-0.01
c step for Z coordinate
          a = (x2-x1)/(z2-z1)
          b = x1-a*z1
          dz = (z2-z1)*0.34 ! less then cell
          print *,'z1,z2,x1,x2,a,b,dz=',z1,z2,x1,x2,a,b,dz
c take Pmin for the parameter list 
          ip = getimom(pmin) - 1
          ipmax = getimom(pmax) - 1
          if(ip.LT.0) ip = 0
          if(ipmax.GT.255) ipmax = 255
          if(ip.GT.255) then
            print *,'Pmin too large (',getmom(ip),') - exit'
            print *,'possible range: Pmin=',getmom(0),' Pmax=',getmom(255)
            stop
          endif
          print *,'ipmin=',ip,'  ipmax=',ipmax

ccccccccccccccccccccc
c loop over momentum
ccccccccccccccccccccc

          do while(ip.LE.ipmax)
            p = getmom(ip)
            p_=p
            print *,'current p=',p_
c set 4 other parameters to be used in momentum step calculations
            z_=z1
            y_=0.
            theta_=0.
            phi_ = 0.
c calculate starting theta
  301       continue
#include "macro1.inc"
            call sim_main(ifail)
            if(ifail.GT.0) then
              theta_ = theta_ + 1.
              if(theta_.GT.140.) then
                print *,'error 300-1'
                goto 300
              endif
              goto 301
            endif
            CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwp1)
            itmp = iwp1(1)*iwp1(2)*iwp1(3)*iwp1(4)*iwp1(5)*iwp1(6)
            if(itmp.EQ.0) then
              theta_ = theta_ + 1.
                print *,'error 300-2, iwp1=',iwp1
              if(theta_.GT.140.) goto 300
              goto 301
            endif
            theta1=theta_ ! save theta
            print *,'iwp1=',iwp1
            print *,'adjusting momentum step: starting theta=',theta1
c try next momenta ...
            i=0
            ipd = 1000
            do while(1.EQ.1)
              i=i+1
              if(ip+i.GT.255) then
                i = 255 - ip
                goto 310
              endif
              p_=getmom(ip+i)
              print *,'trying p=',p_

c loop over theta to get next momentum
C SDELAT' ZAVISIMOST' DP OT THETA ???

              theta_ = theta1
              do while(theta_.LT.thetamax)
#include "macro1.inc"
                call sim_main(ifail) ! will work at least once, we tested it above
                if(ifail.GT.0) goto 305
                CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwp2)
                itmp = iwp2(1)*iwp2(2)*iwp2(3)*iwp2(4)*iwp2(5)*iwp2(6)
                if(itmp.EQ.0) goto 305
c                print *,'iwp2=',iwp2
                if(iwp1(1).NE.iwp2(1).OR.iwp1(2).NE.iwp2(2).OR.iwp1(3).NE.iwp2(3).OR.
     &          iwp1(4).NE.iwp2(4).OR.iwp1(5).NE.iwp2(5).OR.iwp1(6).NE.iwp2(6)) then
                  ipd = MIN(ipd,i)
                endif
                theta_ = theta_ + 1.
              enddo
 305          continue
              if(ipd.LT.1000) goto 310
            enddo
c if we are here something wrong
 300        continue
            print *,'sda_run: ERROR 300 - goto next iw'
            goto 400 ! stop
c got next momentum value; will be used for the next loop over momentum
 310        continue
            ip = ip + ipd ! next momentum value
            print 1000, p
            print *,'next p will be ',getmom(ip)
            p_ = p

cccccccccccccccccccccccccccccccccc
c loop over x-z plane coordinates
cccccccccccccccccccccccccccccccccc

            z_ = z1 - dz
            do while(z_.LT.(z2-dz))
              z_ = z_ + dz
              x_ = a*z_ + b
*              print *,'loop over x-z: x=',x_,'   z=',z_,'   z2=',z2

c get 'ymin' and 'ymax'
              if(dc_wlen(1,iw_,1,1).LT.1.) then ! less then 1cm
                dy = dc_wlen(1,iw_,1,1) - 0.1
                ymax = dc_wlen(1,iw_,1,1)
              else
                ymax = dc_wlen(1,iw_,1,1) - 1. ! step 1cm from the end of wire
              endif
              ymin = -ymax
*              print *,'ymin=',ymin,'   ymax=',ymax

c set initial dtheta step; will be adjusted on flight
              dtheta = 0.4 ! 0.4 ! theta resolution of the pattern recognition is about 2 degree
              theta = theta1 ! we already found theta min, it was saved as 'theta1'
              theta = theta - dtheta
              if(thetamax .GT. 140.) thetamax = 140.
              print *,'thetamax=',thetamax,' thetamin=',thetamin,' theta=',theta,' dtheta=',dtheta
              iwtheold(1)=-1

cccccccccccccccccc
c loop over theta
cccccccccccccccccc

              do while(theta.LT.thetamax)
                theta = theta + dtheta
                theta_ = theta

c get iwthe()
                y_=0.
                phi_ = 0.
#include "macro1.inc"
                call sim_main(ifail)
                if(ifail.GT.0) then
                  iwtheold(1)=-1
c      print *,'111'
c      stop
                  goto 3000
                endif
                CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwthe)
                itmp = iwthe(1)*iwthe(2)*iwthe(3)*iwthe(4)*iwthe(5)*iwthe(6)
                if(itmp.EQ.0) then
                  iwtheold(1)=-1
c      print *,'222'
c      stop
                  goto 3000
                endif

C- STEP CORRECTION (theta) ------------------------------------------
                if(iwtheold(1).ne.-1) then

                  if(icomp6(iwthe,iwtheold).eq.1) then ! decrease step
*                    print *,'decrease theta: road: ',iwthe,'  phi=',phi_,'  theta=',theta
                    theta = theta - dtheta ! step back
                    dtheta = dtheta * 0.9 ! decrease step size
*                    print *,'theta step decreased=',dtheta
                    if(dtheta.LT.0.01) then
                      print *,'sda_run: ERROR: dtheta too small - stop.'
                      stop
                    endif
                    goto 2000 ! skip PHI scan, try new THETA
                  else if(icomp6(iwthe,iwtheold).eq.-1) then ! increase step
                    dtheta = dtheta*1.1
*                    print *,'theta step increased=',dtheta
                    do i=1,6
                      iwtheold(i)=iwthe(i)
                    enddo
                  else if(icomp6(iwthe,iwtheold).eq.0) then ! go head
*                    print *,'road: ',iwthe,'  phi0=',phi_,'  theta=',theta
                    do i=1,6
                      iwtheold(i)=iwthe(i)
                    enddo
                  endif

                else
*                  print *,'road: iwthe=',iwthe
                  do i=1,6
                    iwtheold(i)=iwthe(i)
                  enddo
                endif
C- STEP CORRECTION (theta) ------------------------------------------

c choose step for 'y' and calculate phimin and phimax
                phi_ = -0.
                y_=ymin
*                print *,'ymin=',ymin
#include "macro1.inc"
                call sim_main(ifail)
                if(ifail.GT.0) then
                  print *,'1: theta_=',theta_
                  goto 200
                endif
                CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwy1)
                itmp = iwy1(1)*iwy1(2)*iwy1(3)*iwy1(4)*iwy1(5)*iwy1(6)
                if(itmp.EQ.0) then
                  print *,'2->',iwy1
                  print *,'2: theta_=',theta_
                  goto 200
                endif
*                print *,'ymin->',iwy1

                phi_ = 0.
                y_=ymax
*                print *,'ymax=',ymax
#include "macro1.inc"
                call sim_main(ifail)
                if(ifail.GT.0) then
                  print *,'3: theta_=',theta_
                  goto 200
                endif
                CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwy2)
                itmp = iwy2(1)*iwy2(2)*iwy2(3)*iwy2(4)*iwy2(5)*iwy2(6)
                if(itmp.EQ.0) then
                  print *,'4->',iwy2
                  print *,'4: theta_=',theta_
                  goto 200
                endif
*                print *,'ymax->',iwy2
                itmp = IABS(iwy1(1)-iwy2(1))
                do i=2,6
                  if(itmp.LT.IABS(iwy1(i)-iwy2(i))) itmp=IABS(iwy1(i)-iwy2(i))
                enddo
                dy=ymax*2./float(itmp+3)
                y = ymin - dy
*                print *,'itmp=',itmp,'  ymin=',ymin,'  ymax=',ymax,'  dy=',dy
                goto 210

 200            continue
                print *,'sda_run: ERROR 200 - stop'
                goto 500 ! stop

 210            continue


cccccccccccccccccccccccccccccccccc
c loop over coordinate along wire
cccccccccccccccccccccccccccccccccc

                do while(y.LT.(ymax-dy))
                  y = y + dy
                  y_ = y
*                  print *,'y=',y,' (half len=',dc_wlen(1,iw_,1,1),')'

c set phi parameters
                  phi_ = -dphi
                  phimax = 30.
c                  do i=1,6
c                    iwphiold(i)=iwy1(i)
c                  enddo
                  iwphiold(1)=-1

cccccccccccccccccc
c loop over phi-1
cccccccccccccccccc

                  do while(phi_.LT.phimax)
                    phi_ = phi_ + dphi
#include "macro1.inc"
                    CALL sim_main(ifail)
                    if(ifail.GT.0) then
*                      print *,'11'
                      goto 2000
                    endif
                    CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwphi)
                    itmp = iwphi(1)*iwphi(2)*iwphi(3)*iwphi(4)*iwphi(5)*iwphi(6)
                    if(itmp.EQ.0) then
*                      print *,'22'
                      goto 1500
                    endif
C- STEP CORRECTION ------------------------------------------

                    if(iwphiold(1).ne.-1) then
                    if(icomp6(iwphi,iwphiold).eq.1) then ! decrease step
                      phi_ = phi_ - dphi ! step back
                      dphi = dphi * 0.5 ! decrease step size
*                      print *,'1: step decreased(PHI)=',dphi,' because of ',iwphi,'!=',iwphiold
                      if(dphi.LT.0.01) then
                        print *,'1: sda_run: ERROR: dphi too small(1) - stop.'
                        stop
                        goto 1500
                      endif
                      goto 1400 ! try new PHI
                    else if(icomp6(iwphi,iwphiold).eq.-1) then ! increase step
                      dphi = dphi*1.5
                      if(dphi.GT.9.) dphi=9.
*                      print *,'1: step increased(PHI)=',dphi,' because of ',iwphi,'==',iwphiold
                    else if(icomp6(iwphi,iwphiold).eq.0) then ! go head
*                      print *,'1: road: ',iwphi,'  phi1=',phi_,'  theta=',theta_
                      do i=1,6
                        iwphiold(i)=iwphi(i)
                      enddo
                    endif
                    else
*                      print *,'road: iwphi=',iwphi
                      do i=1,6
                        iwphiold(i)=iwphi(i)
                      enddo
                    endif

C- STEP CORRECTION ------------------------------------------
                    IF(ievent.GE.Nevents) goto 100
 1400               continue
                  enddo
 1500             continue
cccccccccccccccccc
c loop over phi-2
cccccccccccccccccc

                  phi_ = dphi
                  phimin = -30.
c                  do i=1,6
c                    iwphiold(i)=iwy1(i)
c                  enddo
                  iwphiold(1)=-1
*                  print *,'2: road0: ',iwphiold,'  phi2=',phi_,'  theta=',theta_

                  do while(phi_.GT.phimin)
                    phi_ = phi_ - dphi
#include "macro1.inc"
                    CALL sim_main(ifail)
                    if(ifail.GT.0) goto 2000
                    CALL sim_anal(iw,iw,ifevb,inter,ievent,iwl3,iwphi)
                    itmp = iwphi(1)*iwphi(2)*iwphi(3)*iwphi(4)*iwphi(5)*iwphi(6)
*                    print *,'2: road1: ',iwphi,'  phi2=',phi_,'  theta=',theta_
                    if(itmp.EQ.0) goto 2000
C- STEP CORRECTION ------------------------------------------
                    if(iwphiold(1).ne.-1) then
                    if(icomp6(iwphi,iwphiold).eq.1) then ! decrease step
*                    print *,'2: road2: ',iwphi,'  phi2=',phi_,'  theta=',theta_
                      phi_ = phi_ + dphi ! step back
                      dphi = dphi * 0.5 ! decrease step size
*                      print *,'2: step decreased(PHI)=',dphi,' because of ',iwphi,'!=',iwphiold
                      if(dphi.LT.0.0001) then
                        print *,'sda_run: ERROR: dphi too small(2) - stop.'
                        stop
                        goto 2000
                      endif
                      goto 1900 ! try new PHI
                    else if(icomp6(iwphi,iwphiold).eq.-1) then ! increase step
                      dphi = dphi*1.5
                      if(dphi.GT.9.) dphi=9.
*                      print *,'2: step increased(PHI)=',dphi,' because of ',iwphi,'==',iwphiold
                    else if(icomp6(iwphi,iwphiold).eq.0) then ! go head
*                      print *,'2: road: ',iwphi,'  phi2=',phi_,'  theta=',theta_
                      do i=1,6
                        iwphiold(i)=iwphi(i)
                      enddo
                    endif
                    else
*                      print *,'2: road: iwphi=',iwphi
                      do i=1,6
                        iwphiold(i)=iwphi(i)
                      enddo
                    endif
C- STEP CORRECTION ------------------------------------------
                    IF(ievent.GE.Nevents) goto 100
 1900               continue
                  enddo



ccccccccccccccccccc
c end of all loops
ccccccccccccccccccc


                enddo   ! y
 2000           continue
 3000           continue
              enddo       ! theta
 500          continue
            enddo     ! z
          enddo       ! p
 400      continue
        enddo         ! iw

C
C *** End of loop over the events ***
C
 100  CONTINUE
C
 1000 format(1X,'mom=',F7.4,' GeV/c')
 1001 format(1x,'track:  p=',f6.3,' GeV/c  iw=',i3,'  coord=',3f7.2,
     &          '  theta=',f6.2,'  phi=',f6.2)  
C
      RETURN
      END
C======================================================================
      INTEGER FUNCTION icomp6(iwc,iwcold)
C
C return: -1 if all 6 numbers the same
C          1 if at least one number differ more then 1
C          0 otherwise
C
      implicit none
      integer iwc(6),iwcold(6)
      if(iwc(1).eq.iwcold(1).and.iwc(2).eq.iwcold(2).and.iwc(3).eq.iwcold(3).and.
     &   iwc(4).eq.iwcold(4).and.iwc(5).eq.iwcold(5).and.iwc(6).eq.iwcold(6)) then
        icomp6=-1
      else if(IABS(iwc(1)-iwcold(1)).gt.1.or.IABS(iwc(2)-iwcold(2)).gt.1.or.
     &        IABS(iwc(3)-iwcold(3)).gt.1.or.IABS(iwc(4)-iwcold(4)).gt.1.or.
     &        IABS(iwc(5)-iwcold(5)).gt.1.or.IABS(iwc(6)-iwcold(6)).gt.1) then
        icomp6=1
      else
        icomp6=0
      endif
      return
      end

