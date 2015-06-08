h23965
s 00000/00000/00000
d R 1.2 01/11/19 19:02:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_ipln.F
e
s 00212/00000/00000
d D 1.1 01/11/19 19:02:38 boiarino 1 0
c date and time created 01/11/19 19:02:38 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_ipln(isec,vect,vout,ip, sint,xint,iflag)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Intersection of a Track with a Plane.
C-
C-      Calculates intersection of track  with a plane situated at     
C-      the distance 'd' from the center of coordinates. The vector    
C-      'ortvec' is the unit length vector directed from the center    
C-      of coordinates to the direction of the plane and orthogonal    
C-      to it.  The track is approximated by a cubic in the track      
C-      length.     To improve stability, the coordinate system        
C-      is shifted. Normally the Newton method is used to find the     
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
C-      IFLAG     =1 if track intersects the surface, =0 if not        
C-
C-   Controls:
C-      EPSI     (internal variable) - accuracy in the track length with
C-               which the intersection is found. The accuracy is estimated
C-               as the difference in the track length between two last
C-               iterations for Newton method (actual error in the value is
C-               expected to be much smaller). The Half-Interval Division
C-               method uses this value as a limit to the accuracy in the
C-               direction orthogonal to the plane.
C-               Actual error in this method is expected to be of the order
C-               of EPSI.
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
C-   Created : by Pavel Degtyarenko (September 1992) based on INT_CYL.
C-            
C-   Authors of INT_CYL: R.Brun and J.Dumont from an original routine by
C-                       H. Boerner, KEK October 1982.
C-                       Modified: S. Laubach August,1990 and by
C-                                 B. Niczyporuk October, 1990
C-                                 B. Niczyporuk December,1996
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
      REAL sint, vect(7),vout(7),xint(7)
C
C Local variables
      INTEGER i, maxhit
      REAL x1(3),x2(3), a(4),b(4),c(4), ortvec(3)
      REAL drctn,xshft,yshft,zshft,sshft,shiftx,shifty,shiftz
      REAL d,r1,r2,rprim0,rprim1,s,s1,x,y,z,rn,dr,dinter,epsi
C
      DATA maxhit /100  /
      DATA epsi   /0.002/
C
C
      iflag  = 1
C
      IF(sda_pln(8,ip,isec).LE.0.) THEN
        iflag  = 0
        GO TO 90
      ENDIF
C
      ortvec(1) = sda_pln(4,ip,isec)
      ortvec(2) = sda_pln(5,ip,isec)
      ortvec(3) = sda_pln(6,ip,isec)
      d         = sda_pln(8,ip,isec)
C
C Translate to offset axes
      DO i = 1,3
        x1(i) = vect(i) - sda_pln(i,ip,isec)
        x2(i) = vout(i) - sda_pln(i,ip,isec)
      ENDDO
C
C The distances of the 1-st and 2-nd point to the plane parallel to
      r1 = x1(1)*ortvec(1) + x1(2)*ortvec(2) + x1(3)*ortvec(3)
      r2 = x2(1)*ortvec(1) + x2(2)*ortvec(2) + x2(3)*ortvec(3)
C
C TRACK CROSSING THE PLANE FROM INSIDE OR OUTSIDE ?
C
      IF (r2.GT.r1) THEN
        IF ((d.LT.r1).OR.(d.GT.r2)) THEN
          iflag = 0
          GO TO 90
        ENDIF
        drctn = 1.
      ELSE
        IF ((d.LT.r2).OR.(d.GT.r1)) THEN
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
C THE INTERSECTION OF TRACK AND PLANE.
C START AT S=0. - MIDDLE OF THE SHIFTED INTERVAL.
C First try to use Newton method: search for zero of the
C function f(s): s   = s - f(s )/f'(s )
C                 i+1   i     i      i
C
      rprim0 = a(2)*ortvec(1) + b(2)*ortvec(2) + c(2)*ortvec(3)
      IF(rprim0.EQ.0.) GO TO 14
      s1 = ( d-(shiftx+a(1))*ortvec(1)
     &        -(shifty+b(1))*ortvec(2)
     &        -(shiftz+c(1))*ortvec(3) )/rprim0
C
      DO i=1,maxhit
        rprim1 = ( ortvec(1)*(a(2)+s1*(2.*a(3)+3.*a(4)*s1)) +
     &             ortvec(2)*(b(2)+s1*(2.*b(3)+3.*b(4)*s1)) +
     &             ortvec(3)*(c(2)+s1*(2.*c(3)+3.*c(4)*s1))  )
        IF(rprim1.EQ.0.) GO TO 14
        s = s1 + (d -
     &    ( (shiftx+a(1)+s1*(a(2)+s1*(a(3)+s1*a(4))))*ortvec(1)+
     &      (shifty+b(1)+s1*(b(2)+s1*(b(3)+s1*b(4))))*ortvec(2)+
     &      (shiftz+c(1)+s1*(c(2)+s1*(c(3)+s1*c(4))))*ortvec(3) ))
     &      /rprim1
        IF(abs(s).GT.sint*0.5) goto 14
        IF (ABS(s-s1).LT.epsi) GO TO 20
        s1 = s
      ENDDO
C
C Good Old Iteration Process
C
   14 CONTINUE
C
      dinter = sint * 0.5
      s      = 0.
C
      DO i=1,maxhit
        x = shiftx + a(1) + s*(a(2) + s*(a(3) + s*a(4)))
        y = shifty + b(1) + s*(b(2) + s*(b(3) + s*b(4)))
        z = shiftz + c(1) + s*(c(2) + s*(c(3) + s*c(4)))
        rn = x*ortvec(1) + y*ortvec(2) + z*ortvec(3)
        dr = (d - rn) * drctn
        IF (ABS(dr).LT.epsi) GOTO 20
        dinter = dinter * 0.5
        IF (dr.LT.0.) s = s - dinter
        IF (dr.GE.0.) s = s + dinter
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
E 1
