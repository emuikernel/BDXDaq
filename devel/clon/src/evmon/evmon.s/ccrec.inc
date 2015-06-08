
c   ccrec.inc
c   =========
c
c   Extracted values of CC response
c
c   AV  May,14 1997
c
c  
c  Variables contained in CCCALB common block
c  --------------------------------------------
c
c
      integer mccsegm,   ! Max number of CC segments
     &        kcciden,   ! 1000*Sector + Segment (1-36)
     &        kccpmtn,   ! Unique PMT number
     &        kccrunn    ! Calibration run number
      real    xcct2ch,   ! Time to chann. coeff. ( ~ 0.05 )
     &        opheamp,   ! One ph.e. amplitude
     &        dpheamp,   ! One ph.e. amplitude dispersion
     &        ccpedes,   ! Amplitude pedestal
     &        acctime,   ! Additive time constant
     &        dcctime,   ! Additive time constant dispersion
     &        rccsens    ! Normalized PMT sensib. for Cerenkov photons
c
      parameter ( mccsegm = 36 )   !  Number of CC segments
c
c_end_var
c
      common /CCCALB/ kcciden(6,mccsegm),xcct2ch(6,mccsegm),
     & opheamp(6,mccsegm), dpheamp(6,mccsegm), ccpedes(6,mccsegm),
     & acctime(6,mccsegm), dcctime(6,mccsegm), kccpmtn(6,mccsegm),
     & rccsens(6,mccsegm), kccrunn(6,mccsegm)
c    
c =========================================================
