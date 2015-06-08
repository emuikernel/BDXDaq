      integer maxECT
      parameter (maxECT=1296)
      integer nECT
      integer secECT
      integer layerECT
      integer stripECT
      integer TDCECT

*
      common /ECTnt/ nECT
     &,secECT(maxECT)
     &,layerECT(maxECT)
     &,stripECT(maxECT)
     &,TDCECT(maxECT)

