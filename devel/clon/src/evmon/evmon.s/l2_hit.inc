c  include file for level2 histograms

c  Stepan Stepanyan, 8-feb-1999


      integer MAXhit
      parameter (MAXhit = 4)
      integer nsl0
      integer nsl1
      integer sl2h
      integer l2sec
      common/l2_hit/nsl0,sl2h(6,6,MAXhit)
      common/l2_res/nsl1,l2sec(9,MAXhit)
      save
      