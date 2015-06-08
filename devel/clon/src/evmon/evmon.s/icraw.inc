c Include file for raw hit information, ADCs and TDC
      Integer MaxX,MaxY,MaxRaw
      PARAMETER (MaxRaw=1000)
      PARAMETER (MaxX=23)
      PARAMETER (MaxY=23)
      Integer nmod,ntdc(MaxRaw)
      Integer Istat(MaxRaw), ADC(MaxRaw), TDC(MaxRaw,8),
     +     ixm(MaxRaw),iym(MaxRaw),ixo(MaxRaw),iyo(MaxRaw)
      Common/icraw/nmod,ntdc,istat,adc,tdc,ixm,iym,ixo,iyo
c
