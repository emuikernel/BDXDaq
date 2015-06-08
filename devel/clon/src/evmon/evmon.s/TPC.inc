      integer max_tpc
      parameter (max_tpc=60000)
      integer TPC_ID                !  the address of the hit detector element
      parameter ( TPC_ID = 1 )
      integer TPC_TDC               !  tdc information (channels)
      parameter ( TpC_TDC = 2 )
      integer TPC_ADC               !  adc information (channels)
      parameter ( TPC_ADC = 3 )
      integer itpc_ok
      integer nTPC,idTPC,secTPC,tdcTPC,adcTPC,sideTPC,padTPC,rowTPC,colTPC
      common/TPC/nTPC,itpc_ok
     &          ,idTPC(max_tpc),secTPC(max_tpc),tdcTPC(max_tpc),adcTPC(max_tpc)
     &          ,sideTPC(max_tpc),padTPC(max_tpc),rowTPC(max_tpc),colTPC(max_tpc)

