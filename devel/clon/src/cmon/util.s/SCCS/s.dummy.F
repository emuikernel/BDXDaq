h10564
s 00000/00000/00000
d R 1.2 01/11/19 19:12:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/dummy.F
e
s 00047/00000/00000
d D 1.1 01/11/19 19:12:17 boiarino 1 0
c date and time created 01/11/19 19:12:17 by boiarino
e
u
U
f e 0
t
T
I 1
c
c dummy.F
c

      FUNCTION ICFIND (CHI, CHV, JLP, JRP)
C-    Find first occurrence of CHI(1:1) in CHV(JL:JR)
      DIMENSION    JLP(9), JRP(9)
      CHARACTER    CHV*(*), CHI*1
      JJ = JLP(1)
      JR = JRP(1)
   11 IF (JJ.GT.JR)          GO TO 19
      IF (CHV(JJ:JJ).NE.CHI)  THEN
          JJ = JJ + 1
          GO TO 11
        ENDIF
      ICFIND = JJ
      RETURN
   19 continue
      ICFIND = JR + 1
      RETURN
      END


      SUBROUTINE GETENVF (NAME, TEXT)
C Fortran interface routine to getenv
      COMMON/SLATE/ islate(40)
      CHARACTER*(*)  NAME, TEXT
      LGN = LNBLNKL(NAME)
      NTX = LEN(TEXT)
      CALL GETENI (NAME,TEXT,NTX,LGN)
      islate(1) = NTX
      RETURN
      END

      FUNCTION LNBLNKL(CHV)
C-    Find last non-blank character in CHV
      CHARACTER    CHV*(*)
      N = LEN(CHV)
      DO 17  JJ= N,1,-1
      IF (CHV(JJ:JJ).NE.' ') GO TO 99
   17 CONTINUE
      JJ = 0
   99 LNBLNKL = JJ
      RETURN
      END


E 1
