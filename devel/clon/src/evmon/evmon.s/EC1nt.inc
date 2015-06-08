      integer maxEC1
      parameter (maxEC1=384)
      integer nEC1
      integer secEC1
      integer layerEC1
      integer stripEC1
      integer TDCLEC1
      integer ADCLEC1
      integer TDCREC1
      integer ADCREC1
*
      common /EC1nt/ nEC1
     &,secEC1(maxEC1)
     &,layerEC1(maxEC1)
     &,stripEC1(maxEC1)
     &,TDCLEC1(maxEC1)
     &,ADCLEC1(maxEC1)
     &,TDCREC1(maxEC1)
     &,ADCREC1(maxEC1)
