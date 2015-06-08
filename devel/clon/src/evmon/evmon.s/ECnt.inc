      integer maxEC
      parameter (maxEC=1296)
      integer nEC
      integer secEC
      integer layerEC
      integer stripEC
      integer TDCEC
      integer ADCEC
*
      common /ECnt/ nEC
     &,secEC(maxEC)
     &,layerEC(maxEC)
     &,stripEC(maxEC)
     &,TDCEC(maxEC)
     &,ADCEC(maxEC)
