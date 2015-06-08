      integer
     &  ID_pc 
     & ,TDC_pc
     & ,ADCMN_pc
     & ,ADCLT_pc
     & ,ADCRB_pc
     & ,ADCLB_pc
     & ,ADCRT_pc
     & ,ADCVE_pc
     & ,nDSPS
     & ,ID_ps  
     & ,TDC_ps
     & ,ADC_ps
     & ,ID_tc
     & ,TDCTA_tc 
     & ,ADCLT_tc  
     & ,ADCRT_tc  
     & ,ADCLB_tc  
     & ,ADCRB_tc  
     & ,ADCS1_tc
     & ,ADCS2_tc
     & ,ADCS3_tc
     & ,nTAGE0
     & ,ID_tage 
     & ,TDC_tage
     & ,nTAGE1
Csergey: twice     & ,ID_tage 
Csergey: twice     & ,TDC_tage
     & ,nTAGT 
     & ,ID_tagt   
     & ,TDCL_tagt    
     & ,TDCR_tagt
     & ,nST
     & ,ID_st  
     & ,TDC_st 
     & ,ADC_st 
     & ,NT_ID
     & ,NT_TDC
     & ,NT_N
     & ,E_N
     & ,E_ID
     & ,E_TDC
     & ,TGBI_latch
     & ,E_N_NEW
     & ,E_ID_NEW
     & ,E_TDC_NEW
     & ,ID_tatl
     & ,ID_tatr
     & ,TDC_tatl
     & ,TDC_tatr
     & ,nTATL
     & ,nTATR
     & ,nSTN0
     & ,nSTN1
     & ,TDC_stn0
     & ,ADC_stn1
     & ,ID_stn0
     & ,ID_stn1
     & ,nMOR
     & ,TDC_MOR(256)

*
      common /PHOTONnt/ 
     &  ID_pc
     & ,TDC_pc
     & ,ADCMN_pc
     & ,ADCLT_pc
     & ,ADCRB_pc
     & ,ADCLB_pc
     & ,ADCRT_pc
     & ,ADCVE_pc
     & ,nDSPS
     & ,ID_ps  (8)
     & ,TDC_ps (8)
     & ,ADC_ps (8)
     & ,ID_tc
     & ,TDCTA_tc 
     & ,ADCLT_tc  
     & ,ADCRT_tc  
     & ,ADCLB_tc  
     & ,ADCRB_tc  
     & ,ADCS1_tc
     & ,ADCS2_tc
     & ,ADCS3_tc
     & ,nTAGE0
     & ,ID_tage (9216) 
     & ,TDC_tage(9216)
     & ,nTAGT    
     & ,ID_tagt  (61) 
     & ,TDCL_tagt(61)    
     & ,TDCR_tagt(61)
     & ,nST
     & ,ID_st  (6)
     & ,TDC_st (6)
     & ,ADC_st (6)
     & ,NT_ID(976)
     & ,NT_TDC(976)
     & ,NT_N
     & ,E_N
     & ,E_ID(6144)
     & ,E_TDC(6144)
     & ,TGBI_latch
     & ,nTAGE1
     & ,E_N_NEW
     & ,E_ID_NEW(6144)
     & ,E_TDC_NEW(6144)
     & ,nTATL
     & ,ID_tatl(2048)
     & ,TDC_tatl(2048)
     & ,nTATR
     & ,ID_tatr(2048)
     & ,TDC_tatr(2048)
     & ,nSTN0
     & ,nSTN1
     & ,ID_stn0(2048)
     & ,ID_stn1(26)
     & ,TDC_stn0(2048)
     & ,ADC_stn1(26)
     & ,nMOR
     & ,TDC_MOR(256)
     & ,nCTOF1
     & ,nCTOF2
     & ,TDC_CTOF1(16)
     & ,TDC_CTOF2(16)
     & ,ADC_CTOF1
     & ,ADC_CTOF2
     

	integer nCTOF1, nCTOF2, TDC_CTOF1, TDC_CTOF2, ADC_CTOF1, ADC_CTOF2

	integer tac_ped_lt,tac_ped_rt,tac_ped_lb,tac_ped_rb,tac_ped_sum1,tac_ped_sum2,tac_ped_sum3,
     +	pc_ped_ve,pc_ped_mn,pc_ped_lt,pc_ped_rb,pc_ped_lb,pc_ped_rt,
     +	ps_ped

     	real tac_tdc_w,pc_TDC_w,ps_tdc_w,
     +	posTACpeak,posPCpeak,posPSpeak,
     +  tac_sca_lt,tac_sca_rt,tac_sca_lb,tac_sca_rb,tac_sca_sum1,tac_sca_sum2,tac_sca_sum3,
     +  timwk1,timwk2,timwk3,PSthreshold
	integer ET_COIN(384,2)
	common/TagDSDCal/tac_ped_lt,tac_ped_rt,tac_ped_lb,tac_ped_rb,
     +	tac_ped_sum1,tac_ped_sum2,tac_ped_sum3,
     +  tac_sca_lt,tac_sca_rt,tac_sca_lb,tac_sca_rb,
     +  tac_sca_sum1,tac_sca_sum2,tac_sca_sum3,
     +	tac_tdc_w,posTACpeak,
     +	pc_ped_ve,pc_ped_mn,pc_ped_lt,pc_ped_rb,pc_ped_lb,pc_ped_rt,pc_TDC_w,posPCpeak,
     +	ps_ped(8),ps_tdc_w(8),posPSpeak(8),
     +  timwk1,timwk2,timwk3,PSthreshold,
     +  ET_COIN  

	real tage1, tage0, tage2

	integer PSFT_hits,PSBT_hits,PSF_scaler,PSB_scaler,
     +  PSF_TDC, PSB_TDC, PSF_ADC, PSB_ADC,
     +  PS_logic_hits, PS_logic_TDC,
     +  CUA_hits, CUA_scaler, CUA_TDC, CUA_ADC
	
	common/PAIRSPEC/PSFT_hits(16),PSBT_hits(16),PSF_scaler(16),PSB_scaler(16),
     +  PSF_TDC(16,16), PSB_TDC(16,16), PSF_ADC(16), PSB_ADC(16),
     +  PS_logic_hits(16), PS_logic_TDC(16,16),
     +  CUA_hits(4), CUA_scaler(4), CUA_TDC(16,4), CUA_ADC(4)

	integer firsttime

	common/TAGEHITS/ firsttime,TAGE1(200), TAGE0(384), TAGE2(32)



