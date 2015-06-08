c
c TPE include file
c
      integer MAXADC, MAXTDC
      integer MAXX, MAXY
      parameter (MAXADC=31)
      parameter (MAXTDC=31)
      parameter (MAXX=6)
      parameter (MAXY=6)
c
c pedestals
c
      integer tpeped(MAXADC)
c
c data
c
      integer nadchits, idadc(MAXADC), iadc(MAXADC)
      integer ntdchits, idtdc(MAXTDC), itdc(MAXTDC)
c
c common block
c
      common/TPEnt/tpeped,nadchits,idadc, iadc,ntdchits,idtdc,itdc
      