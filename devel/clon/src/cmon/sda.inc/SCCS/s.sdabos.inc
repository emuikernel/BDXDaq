h38053
s 00000/00000/00000
d R 1.2 01/11/19 19:00:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdabos.inc
e
s 00012/00000/00000
d D 1.1 01/11/19 19:00:44 boiarino 1 0
c date and time created 01/11/19 19:00:44 by boiarino
e
u
U
f e 0
t
T
I 1
C Basic BOS array include file.
C                               Bogdan Niczyporuk, June 1994
C
      INTEGER IPAD, IW, ndimb
      PARAMETER (ndimb = 700000)
C
      COMMON/BCS/ IPAD(5), IW(ndimb)
      REAL RW(ndimb)
      EQUIVALENCE (IW(1),RW(1))
C
      save /BCS/
C
E 1
