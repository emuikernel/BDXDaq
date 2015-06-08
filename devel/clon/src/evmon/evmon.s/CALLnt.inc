      integer maxCALL
      integer maxRFT
      parameter (maxCALL=10)
      parameter (maxRFT=1000)
      integer nCALL
      integer nRFT
      integer idCALL(maxCALL)
      integer tdcCALL(maxCALL)
      integer adcCALL(maxCALL)
      integer idRFT(maxRFT)
      integer tdcRFT(maxRFT)
      common /CALLnt/ nCALL
     &,idCALL
     &,tdcCALL
     &,adcCALL
      common /RFTnt/ nRFT
     &,idRFT
     &,tdcRFT
      