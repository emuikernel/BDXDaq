c     SCnt.inc 
      integer maxSC
      integer maxSCT
      parameter (maxSC=342)
      parameter (maxSCT=1200)

      integer nSC
      integer secSC
      integer idSC
      integer TDCLSC
      integer ADCLSC
      integer TDCRSC
      integer ADCRSC

      common /SCnt/ nSC
     &,secSC(maxSC)
     &,idSC(maxSC)
     &,TDCLSC(maxSC)
     &,ADCLSC(maxSC)
     &,TDCRSC(maxSC)
     &,ADCRSC(maxSC)

c pipeline TDCs from SCT bank
      integer nSCT
      integer idSCT
      integer secSCT
      integer TDCSCT
      common/SCTnt/ nSCT
     &,idSCT(maxSCT)
     &,secSCT(maxSCT)
     &,TDCSCT(maxSCT)