      integer maxCC
      parameter (maxCC=216)
      integer nCC
      integer secCC
      integer idCC
      integer TDCCC
      integer ADCCC
      integer TDCCC_NEW
      integer N_TDC
      integer L1
*
      common /CCnt/ nCC,L1
     &,secCC(maxCC)
     &,idCC(maxCC)
     &,TDCCC(maxCC)
     &,ADCCC(maxCC)
     &,TDCCC_NEW(maxCC)
     &,N_TDC(maxCC)
