      integer max_rast
      parameter (max_rast=2)
      integer n_rast
      integer irast_ok
      integer id_rast
      integer TDC_rast
      integer ADC_rast
*
      common /rast/ n_rast, irast_ok
     &,id_rast(max_rast)
     &,TDC_rast(max_rast)
     &,ADC_rast(max_rast)
