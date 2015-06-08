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
