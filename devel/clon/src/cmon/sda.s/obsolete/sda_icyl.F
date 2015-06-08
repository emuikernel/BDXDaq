C======================================================================
      SUBROUTINE sda_icyl(isec,vect,vout,ip, sint,xint,iflag)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Intersection of a Track with a Cylinder.
C-
C-      Calculates intersection of track  with cylindrical surface     
C-      of radius R.  The track is approximated by a cubic in the      
C-      track length.  To improve stability, the coordinate system     
C-      is shifted. First the Newton method (by P.D.) is used to find the     
C-      intersection. Derivatives are calculated analytically using    
C-      the parameters of cubic approximation. For input parameters    
C-      given correctly and for the track not too steep to the surface 
C-      the Newton method converges in 1-2 iterations and gives        
C-      ultimate accuracy. For the reasons of safety of the procedure  
C-      the old Half-Interval Division method is used if the first     
C-      method is failed.
C-
C-
C-   Inputs  :
C-      isec      current sector number
C-      VECT(7)   x,y,z,px/p,py/p,pz/p,p at the beginning of step      
C-      VOUT(7)   x,y,z,px/p,py/p,pz/p,p at the end of step            
C-      IP        plane number                               
C-      SINT      step size (The value of SINT is changed!!!)          
C-
C-   Outputs :
C-      SINT      step size from the beginning of the step to the      
C-                intersection point                                   
C-      XINT(7)   x,y,z,px/p,py/p,pz/p,p at the intersection point     
C-                if p at the beginning is different from p at the end 
C-                of the step then p at the intersection is the linear 
C-                interpolation between the input values               
C-      IFLAG     =1 if track intersects cylinder, =0 if not           
C-
C-   Controls:
C-      EPSI     (internal variable) - accuracy in the track length with
C-               which the intersection is found. The accuracy is estimated
C-               as the difference in the track length between two last
C-               iterations for Newton method (actual error in the value is
C-               expected to be much smaller). The Half-Interval Division
C-               method uses this value as a limit to the accuracy in the
C-               direction orthogonal to the surface of the cylinder.
C-               Actual error in this method is expected to be of the order
C-               of 0.5*EPSI/R.
C-               Default value: 20 microns.
C-
C-      MAXHIT   (internal variable) - limit to the number of iterations
C-               for the fitting procedures. Normally number of iterations
C-               doesn't exceed 3-4 for Newton method and 10-12 for
C-               Half-Interval Division method. Default value: 100
C-
C-   Library belongs : libsda.a
C-
C-   Calls : sda_gcubs
C-
C-   Authors : R.Brun and J.Dumont from an original routine by
C-             H. Boerner, KEK October 1982.
C-
C-   Modified: S. Laubach August,1990 and by B. Niczyporuk October,1990
C-   Modified: Pavel Degtyarenko added the Newton Method September 1992.
C-            
C-
C-   Called by sda_swim
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include"sdageom.inc"
C
C Subroutine parameters
      INTEGER isec, ip, iflag
      REAL sint, vect(7), vout(7) ,xint(7)
C
C Local variables
      INTEGER i, maxhit
      REAL x1(3),x2(3), a(4),b(4),c(4)
      REAL r2,r12,r22,drctn,xshft,yshft,zshft,sshft,shiftx,shifty,shiftz
      REAL ash1,bsh1,rprim0,rprim1,s,s1,x,y,rn2,dr2,dinter,epsi
C
      DATA maxhit /100  /
      DATA epsi   /0.002/
C
C
      iflag  = 1
C
      IF(sda_pln(7,ip,isec).LE.0.) THEN
        iflag  = 0
        GO TO 90
      ENDIF
C
C Translate to offset axes
C
      DO i = 1,3
        x1(i) = vect(i) - sda_pln(i,ip,isec)
        x2(i) = vout(i) - sda_pln(i,ip,isec)
      ENDDO
C
      r12   = x1(1)*x1(1) + x1(2)*x1(2)
      r22   = x2(1)*x2(1) + x2(2)*x2(2)
      r2    = sda_pln(7,ip,isec)*sda_pln(7,ip,isec)
      dr2   = r22 - r2
C
C TRACK CROSSING THE CYLINDER FROM INSIDE OR OUTSIDE ?
C
      IF (r22.GT.r12) THEN
        IF ((r2.LT.r12).OR.(r2.GT.r22)) THEN
          iflag = 0
          GO TO 90
        ENDIF
        drctn = 1.
      ELSE
        IF ((r2.LT.r22).OR.(r2.GT.r12)) THEN
          iflag = 0
          GO TO 90
        ENDIF
        drctn = -1.
      ENDIF

C
C SHIFT COORDINATE SYSTEM SUCH THAT CENTER OF GRAVITY=0
C
      shiftx = (x1(1) + x2(1)) * 0.5
      shifty = (x1(2) + x2(2)) * 0.5
      shiftz = (x1(3) + x2(3)) * 0.5
C
C ONLY ONE VALUE NECESSARY SINCE X1= -X2 ETC.
C
      xshft  = x1(1) - shiftx
      yshft  = x1(2) - shifty
      zshft  = x1(3) - shiftz
      sshft  = -sint * 0.5
C
C PARAMETRIZE THE TRACK BY A CUBIC THROUGH X1,X2
C
      CALL dcgcubs(sshft,xshft,vect(4),vout(4),a)
      CALL dcgcubs(sshft,yshft,vect(5),vout(5),b)
      CALL dcgcubs(sshft,zshft,vect(6),vout(6),c)
C
C ITERATE TO FIND THE TRACK LENGTH CORRESPONDING TO
C THE INTERSECTION OF TRACK AND CYLINDER.
C START AT S=0. - MIDDLE OF THE SHIFTED INTERVAL.
C First try to use Newton method: search for zero of the
C function f(s): s   = s - f(s )/f'(s )
C                 i+1   i     i      i
C
      ash1 = shiftx + a(1)
      bsh1 = shifty + b(1)
      rprim0 = 2.*(ash1*a(2) + bsh1*b(2))
      IF(rprim0.EQ.0.) GOTO 14
      s1 = (r2-ash1*ash1-bsh1*bsh1)/rprim0
C
      DO i=1,maxhit
        rprim1 = 2.*( ash1*a(2)         +   bsh1*b(2)            +
     &    s1*(2.*ash1*a(3)+a(2)*a(2)    +2.*bsh1*b(3)+b(2)*b(2)  +
     &    s1*(3.*(ash1*a(4)+a(2)*a(3)   +   bsh1*b(4)+b(2)*b(3)) +
     &    s1*(2.*(2.*a(2)*a(4)+a(3)*a(3)+2.*b(2)*b(4)+b(3)*b(3)) +
     &    s1*(5.*(a(3)*a(4)             +   b(3)*b(4)          ) +
     &    s1*(3.*(a(4)*a(4)             +   b(4)*b(4)          )
     &    ))))))
        IF(rprim1.EQ.0.) GOTO 14
        s = s1 + (r2-(ash1 + s1*(a(2) + s1*(a(3) + s1*a(4))))**2-
     &               (bsh1 + s1*(b(2) + s1*(b(3) + s1*b(4))))**2 )
     &               /rprim1
        IF(abs(s).GT.sint*0.5) goto 14
        IF (ABS(s-s1).LT.epsi) GO TO 20
        s1 = s
      ENDDO
C
C Good Old Iteration Process ---
C
   14 CONTINUE
C
      dinter = sint * 0.5
      s      = 0.
C
      DO i=1,maxhit
        x = shiftx + a(1) + s*(a(2) + s*(a(3) + s*a(4)))
        y = shifty + b(1) + s*(b(2) + s*(b(3) + s*b(4)))
        rn2 = x*x + y*y
        dr2 = (r2 - rn2) * drctn
        IF (ABS(dr2).LT.epsi) GO TO 20
        dinter = dinter * 0.5
        IF (dr2.LT.0.) s = s - dinter
        IF (dr2.GE.0.) s = s + dinter
      ENDDO
C
C COMPUTE INTERSECTION IN Offset COORDINATES
C
   20 CONTINUE
      xint(1) = shiftx+a(1) +s*(a(2)+s*(a(3)+s*a(4)))+sda_pln(1,ip,isec)          
      xint(2) = shifty+b(1) +s*(b(2)+s*(b(3)+s*b(4)))+sda_pln(2,ip,isec)
      xint(3) = shiftz+c(1) +s*(c(2)+s*(c(3)+s*c(4)))+sda_pln(3,ip,isec)
      xint(4) = a(2) + s*(2.*a(3) + 3.*s*a(4))
      xint(5) = b(2) + s*(2.*b(3) + 3.*s*b(4))
      xint(6) = c(2) + s*(2.*c(3) + 3.*s*c(4))
      xint(7) = 0.5*( vect(7) + vout(7) )
      CALL VUNIT(xint(4),xint(4),3)
      sint = sint*0.5 + s
C
   90 CONTINUE
      RETURN
      END
