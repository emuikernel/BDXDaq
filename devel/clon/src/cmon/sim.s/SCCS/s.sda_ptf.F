h55489
s 00000/00000/00000
d R 1.2 01/11/19 19:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sda_ptf.F
e
s 00362/00000/00000
d D 1.1 01/11/19 19:05:13 boiarino 1 0
c date and time created 01/11/19 19:05:13 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_ptf(X,B)
C----------------------------------------------------------------------
C
C     Purpose and Methods: Calculate magnetic field of Helmholtz coils
C
C     Library belongs: libsda.a
C
C     Called by: sda_minit
C
C     Author    U. Hartfiel     (1982)
C     Modified: M.Guckes    July 1988
C               B.Niczyporuk Sep.1998 (Real magnet geometry utilized + ...)
C
C--  COIL PARAMETERS
C--  ===============
C
C--  N       Number of coils
C--  H0      Field in the origin
C--  RJ      Current density (to calculate the magnetic field)
C--  RI      Inner coil radius
C--  RA      Outer coil radius
C
C--  ZAU     Axial distance of the gap to R-axis at RI
C--  ZAO     Axial distance of the gap to R-axis at RA
C--  ZBU     Axial distance of the outer edge to R-axis at RI
C--  ZBO     Axial distance of the outer edge to R-axis at RA
C
C          
C          A R
C          I
C          I               ZA0      ZBO 
C       RA +	             ________
C          I                | 	     |
C          I		        |	     | 
C          I        	    |	     |
C       RI +                |________|
C          I      ____     ZAU      ZBU 
C          I     |    |         
C          I     |____|
C          I      
C       ---0-----+----+-----+--------+---->  Z
C          I 
C
C
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine variables
      REAL X(3), B(3)
C
C Local variables
C
C Number of loops
      INTEGER i, NLOOP
      PARAMETER (NLOOP = 8)
C
      REAL RI(NLOOP), ZAU(NLOOP), ZBU(NLOOP), RA(NLOOP),
     1     ZAO(NLOOP), ZBO(NLOOP), RJ(NLOOP), RJQ1(NLOOP), TURNS(NLOOP)
C
      REAL H0, S5,S6, D5, R, Z, CURm
C
      LOGICAL LFIRST
C
C Max field [KG]
      DATA H0 /51.0/
C
C Maximum Current [A]
c*    DATA CURm/123.646/
      DATA CURm/124.000/
C
C Oxford coils geometry (Oxford mail of Sep.10,1998)
      DATA RI /11.70,11.76,  13.44,13.385, 16.60, 16.60, 19.488,19.446/
      DATA RA /13.40,13.345, 15.17,15.10,  19.20, 19.185,22.00, 21.920/
C
      DATA ZAU/ 5.20,-8.20,   5.20,-8.20,   7.50,-12.20,  7.50,-12.20/
      DATA ZBU/ 8.20,-5.20,   8.20,-5.20,  12.20, -7.50, 12.20, -7.50/
C
      DATA ZAO/ 5.20,-8.20,   5.20,-8.20,   7.50,-12.20,  7.50,-12.20/
      DATA ZBO/ 8.20,-5.20,   8.20,-5.20,  12.20, -7.50, 12.20, -7.50/
C
C Number of turns per coil
      DATA TURNS/869.0,858.0,1288.0,1288.0,2918.0,2919.0,2900.0,2900.0/
C                    
      DATA    LFIRST /.TRUE./
C
      IF (LFIRST) THEN
         LFIRST = .FALSE.
C
C Calculate Current Density [A/CM**2] for each coil
         DO i = 1,NLOOP
           RJ(i) = TURNS(i)*CURm/( (RA(i) - RI(i))*(ZBU(i) - ZAU(i)) )
         ENDDO
C
C  Integration over D(I) and all coils
         CALL DUSP(NLOOP,0.,0.,RJ,RI,ZAU,ZBU,RA,ZAO,ZBO,H0,S5,S6,D5)
         DO i =1,NLOOP
           RJQ1(i) = RJ(i)*(H0/S6)
	     ENDDO
	     WRITE(6,101) RJ,RJQ1
 101     FORMAT(' RJ  =',8F9.2/' RJQ1=',8F9.2)
         WRITE(6,102) H0,S5,S6,D5
 102     FORMAT(' H0,S5,S6,D5 =',4F10.4)  
      ENDIF
C
C
      R = SQRT(X(1)**2+X(2)**2)
      Z = X(3)
C
      CALL DUSP(NLOOP,R,Z,RJ,RI,ZAU,ZBU,RA,ZAO,ZBO,H0,S5,S6,D5)
C
      IF (R.EQ.0) R=1.E-5
      B(1) = X(1)*(S5/R)
      B(2) = X(2)*(S5/R)
      B(3) = S6
C
      RETURN
      END
C======================================================================
      SUBROUTINE DUSP(NLOOP,R,Z,RJ,RI,ZAU,ZBU,RA,ZAO,ZBO,H0,S5,S6,D5)
C----------------------------------------------------------------------
C
C     Purpose and Methods: Magnetic field calculation
C                          Integration over D(I) and all coils
C
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine variables
      INTEGER NLOOP
      REAL R, Z, H0, S5, S6, D5
      REAL RI(NLOOP), ZAU(NLOOP), ZBU(NLOOP), RA(NLOOP),
     1     ZAO(NLOOP), ZBO(NLOOP), RJ(NLOOP)
C
C Local variables
      INTEGER I1, K, N1,N2
      REAL RRN1, T1, S1,S2,S3,S4, R2 
C
      R2 = R*R
      S5 = 0.
      S6 = 0.
C
      DO 30 K=1,NLOOP
        IF (R.GE.RI(K).AND.R.LE.RA(K).AND.ABS(Z).LE.ABS(ZBO(K))) THEN
           N1 = 100
           RRN1= 0.01
        ELSE
           N1 = 16
           RRN1= 1./16.
        END IF
        N2 = N1
        T1 = (RA(K)-RI(K))*RRN1
        S3 = 0.
        S4 = 0.
C
        DO 311 I1=0,N1,N1
C--  Integration over variable length
          CALL INULI(R,R2,Z,RI(K),FLOAT(I1),RRN1,N2,T1,ZAU(K),ZBU(K),
     1         ZAO(K),ZBO(K),S1,S2)
          S3 = S3+S1
          S4 = S4+S2
 311    CONTINUE
C
        DO 32 I1=1,(N1-1),2
          CALL INULI(R,R2,Z,RI(K),FLOAT(I1),RRN1,N2,T1,ZAU(K),ZBU(K),
     1         ZAO(K),ZBO(K),S1,S2)
          S3 = S3+4.*S1
          S4 = S4+4.*S2
 32     CONTINUE
C
        DO 33 I1=2,(N1-2),2
          CALL INULI(R,R2,Z,RI(K),FLOAT(I1),RRN1,N2,T1,ZAU(K),ZBU(K),
     1         ZAO(K),ZBO(K),S1,S2)
          S3 = S3+2.*S1
          S4 = S4+2.*S2
 33     CONTINUE
C
        S3 = S3*(T1*0.33333333)
        S4 = S4*(T1*0.33333333)
        S5 = S5+S3*RJ(K)*0.0001
        S6 = S6+S4*RJ(K)*0.0001
 30   CONTINUE
C
      D5 = (S6-H0)/H0
C
      RETURN
      END
C======================================================================
      SUBROUTINE INULI(R,R2,Z,RIK,RI1,RRN1,N2,T1,ZAUK,ZBUK,ZAOK,ZBOK,              
     1                 S1,S2)
C----------------------------------------------------------------------
C
C     Purpose and Methods: Integration over variable length
C
C
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine variables
      INTEGER N2
      REAL R, R2, Z, RIK, RI1, RRN1 ,T1, ZAUK,ZBUK,ZAOK,ZBOK, S1,S2
C
C Local varibles
      INTEGER I2  
      REAL A,A2, B1,B2, GIA,GIB, T2,T3
C
C
      A = RIK+T1*RI1
      A2 = A*A
      GIA = ZAUK+(ZAOK-ZAUK)*RRN1*RI1
      GIB = ZBUK+(ZBOK-ZBUK)*RRN1*RI1
      T2 = (GIB-GIA)/FLOAT(N2)
      S1 = 0.
      S2 = 0.
C
      DO 40 I2=0,N2,N2
C--  Calculation of BR and BZ
        CALL BRUBZ(A,A2,R,R2,Z,FLOAT(I2),GIA,T2,B1,B2)
        S1 = S1+B1
        S2 = S2+B2
 40   CONTINUE
C
      DO 41 I2=1,(N2-1),2
        CALL BRUBZ(A,A2,R,R2,Z,FLOAT(I2),GIA,T2,B1,B2)
        S1 = S1+4.*B1
        S2 = S2+4.*B2
 41   CONTINUE
C
      DO 42 I2=2,(N2-2),2
        CALL BRUBZ(A,A2,R,R2,Z,FLOAT(I2),GIA,T2,B1,B2)
        S1 = S1+2.*B1
        S2 = S2+2.*B2
 42   CONTINUE
C
      T3 = ABS(T2)
      S1 = S1*(T3*0.33333333)
      S2 = S2*(T3*0.33333333)
C
      RETURN
      END
C======================================================================
      SUBROUTINE BRUBZ(A,A2,R,R2,Z,RI2,GIA,T2,B1,B2)
C----------------------------------------------------------------------
C
C     Purpose and Methods: Calculation of BR and BZ
C
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine variables
      REAL A, A2, R, R2, Z, RI2, GIA, T2, B1, B2
C
C Local varibles 
      REAL E1,E2, U0,U1,U2, Z1,Z2, AK2
C
C
      Z1 = Z-(GIA+T2*RI2)
      Z2 = Z1*Z1
C
 51   CONTINUE
        U1 = (A+R)*(A+R)+Z2
        U0 = SQRT(U1)
        U2 = (A-R)*(A-R)+Z2
        IF (U2.LE.0.000064) THEN
           A = A+0.01
           A2 = A*A
           GO TO 51
        END IF
C
      AK2 = 4.*A*R/U1
      IF (AK2.GT.0.9999999) AK2=0.9999999
C--  Calculation of elliptic integrals
      CALL ELLIPT(AK2, E1, E2)
      IF (R.GE.0.000001 .AND. ABS(Z1).GE.0.000001) THEN
         B1 = 2.*(E2*(A2+R2+Z2)/U2-E1)*Z1/(R*U0)
      ELSE
         B1 = 0.
      END IF
      B2 = 2.*(E2*(A2-R2-Z2)/U2+E1)/U0
C
      RETURN
      END
C======================================================================
      SUBROUTINE ELLIPT(C8,E1,E2)
C----------------------------------------------------------------------
C
C     Purpose and Methods: Calculation of elliptic integrals
C
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine variables
      REAL C8, E1, E2
C
C Local varibles
      INTEGER I9
      REAL A8,A9, B8,B9, C9, PIBY2 
C
      LOGICAL LFIRST
C
      DATA    LFIRST /.TRUE./
C
C
      IF (LFIRST) THEN
         LFIRST = .FALSE.
         PIBY2 = ACOS(-1.)/2.
      ENDIF
C
      A8 = 1.
      I9 = 2
      B8 = SQRT(1.-C8)
C
 60   CONTINUE
        A9 = (A8+B8)*0.5
        B9 = SQRT(A8*B8)
        C9 = (A8-B8)*0.5
        C8 = C8+I9*C9*C9
        I9 = I9+I9
        A8 = A9
        B8 = B9
      IF (ABS(C9).GT.0.000001) GO TO 60
C
      E1 = PIBY2/A8
      E2 = (1.-C8*.5)*E1
C
      RETURN
      END




















E 1
