c
c IC include file
c

      integer MAXADC, MAXTDC
      parameter (MAXADC=424)
      parameter (MAXTDC=2048)

c
c pedestals
c
      integer icped(MAXADC)
c
c data
c
      integer nadchits,ixadc(MAXADC),iyadc(MAXADC),iadc(MAXADC)
      integer ntdchits,ixtdc(MAXTDC),iytdc(MAXTDC),itdc(MAXTDC)
c
c common block
c
      common /IcCal/icped,nadchits,ixadc,iyadc,iadc,ntdchits,ixtdc,iytdc,itdc
