C Second BOS array include file
C
      INTEGER JPAD, JW, jndimb
      PARAMETER (jndimb = 700000)
C
      COMMON/WCS/ JPAD(5), JW(jndimb)
      REAL RJW(jndimb)
      EQUIVALENCE (JW(1),RJW(1))
C
      save /WCS/
C
