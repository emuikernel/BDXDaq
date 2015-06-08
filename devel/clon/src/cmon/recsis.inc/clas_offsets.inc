
C    Offsets for the BMPR bank follow 

          integer BMPR_Q_TOT               !  Total Charge (Coulomb)
          parameter ( BMPR_Q_TOT = 1 ) 
          integer BMPR_Q_TOT_LV               !  Total charge in Live Time (Coulomb) 
          parameter ( BMPR_Q_TOT_LV = 2 ) 
          integer BMPR_TL_TU               !  ( Time Live ) / ( Time Ungated )
          parameter ( BMPR_TL_TU = 3 ) 
          integer BMPR_CURRENT               !  Current in Ampers
          parameter ( BMPR_CURRENT = 4 ) 

C    Offsets for the BREP bank follow 

          integer BREP_MBSY2C_ENERGY               !  beam energy
          parameter ( BREP_MBSY2C_ENERGY = 1 ) 
          integer BREP_IGT0I00BIASET               !  thermionic gun
          parameter ( BREP_IGT0I00BIASET = 2 ) 
          integer BREP_IGL1I00DAC2               !  B polarized gun
          parameter ( BREP_IGL1I00DAC2 = 3 ) 
          integer BREP_SMRPOSA               !  A slit position
          parameter ( BREP_SMRPOSA = 4 ) 
          integer BREP_SMRPOSB               !  B slit position
          parameter ( BREP_SMRPOSB = 5 ) 
          integer BREP_SMRPOSC               !  C slit position
          parameter ( BREP_SMRPOSC = 6 ) 
          integer BREP_HARP               !  harp
          parameter ( BREP_HARP = 7 ) 
          integer BREP_HALLB_SF_XY560               !  torus current
          parameter ( BREP_HALLB_SF_XY560 = 8 ) 
          integer BREP_MTSETI               !  Mini set current
          parameter ( BREP_MTSETI = 9 ) 
          integer BREP_MTIRBCK               !  Mini current readback
          parameter ( BREP_MTIRBCK = 10 ) 
          integer BREP_MTVRBCK               !  Mini voltage readback
          parameter ( BREP_MTVRBCK = 11 ) 
          integer BREP_TMSETI               !  Tagger set current
          parameter ( BREP_TMSETI = 12 ) 
          integer BREP_TMIRBCK               !  Tagger current readback
          parameter ( BREP_TMIRBCK = 13 ) 
          integer BREP_TMVRBCK               !  Tagger voltage readback
          parameter ( BREP_TMVRBCK = 14 ) 
          integer BREP_CRYO_PRESSURE               !  cryotarget pressure
          parameter ( BREP_CRYO_PRESSURE = 15 ) 
          integer BREP_CRYO_TEMPERATURE               !  cryotarget temperature
          parameter ( BREP_CRYO_TEMPERATURE = 16 ) 
          integer BREP_CRYO_STATUS               !  cryotarget status
          parameter ( BREP_CRYO_STATUS = 17 ) 
          integer BREP_VCG2C24               !  upstream beam vacuum
          parameter ( BREP_VCG2C24 = 18 ) 
          integer BREP_VCG2H01               !  target vacuum
          parameter ( BREP_VCG2H01 = 19 ) 
          integer BREP_SCALERS2O               !  Halo UP upstream
          parameter ( BREP_SCALERS2O = 20 ) 
          integer BREP_SCALERS3O               !  Halo DOWN upstream
          parameter ( BREP_SCALERS3O = 21 ) 
          integer BREP_SCALERS4O               !  Halo LEFT upstream
          parameter ( BREP_SCALERS4O = 22 ) 
          integer BREP_SCALERS5O               !  Halo RIGHT upstream
          parameter ( BREP_SCALERS5O = 23 ) 
          integer BREP_SCALERS6O               !  Halo UP downstream
          parameter ( BREP_SCALERS6O = 24 ) 
          integer BREP_SCALERS7O               !  Halo DOWN downstream
          parameter ( BREP_SCALERS7O = 25 ) 
          integer BREP_SCALERS8O               !  Halo LEFT downstream
          parameter ( BREP_SCALERS8O = 26 ) 
          integer BREP_SCALERS9O               !  Halo RIGHT downstream
          parameter ( BREP_SCALERS9O = 27 ) 
          integer BREP_IPM2H01_XPOS               !  bpm 1 x
          parameter ( BREP_IPM2H01_XPOS = 28 ) 
          integer BREP_IPM2H01_YPOS               !  bpm 1 y
          parameter ( BREP_IPM2H01_YPOS = 29 ) 
          integer BREP_IPM2H01               !  bpm 1 current
          parameter ( BREP_IPM2H01 = 30 ) 
          integer BREP_IPM2C24A_XPOS               !  bpm 2 x
          parameter ( BREP_IPM2C24A_XPOS = 31 ) 
          integer BREP_IPM2C24A_YPOS               !  bpm 2 y
          parameter ( BREP_IPM2C24A_YPOS = 32 ) 
          integer BREP_IPM2C24A               !  bpm 2 current
          parameter ( BREP_IPM2C24A = 33 ) 
          integer BREP_IPM2C22A_XPOS               !  bpm 3 x
          parameter ( BREP_IPM2C22A_XPOS = 34 ) 
          integer BREP_IPM2C22A_YPOS               !  bpm 3 y
          parameter ( BREP_IPM2C22A_YPOS = 35 ) 
          integer BREP_IPM2C22A               !  bpm 3 current
          parameter ( BREP_IPM2C22A = 36 ) 

C    Offsets for the CALL bank follow 

          integer CALL_ID               !  catch-all element(RF,laser diode,etc)
          parameter ( CALL_ID = 1 ) 
          integer CALL_TDC               !  tdc information (channels)
          parameter ( CALL_TDC = 2 ) 
          integer CALL_ADC               !  adc information (channels)
          parameter ( CALL_ADC = 3 ) 

C    Offsets for the CC bank follow 

          integer CC_ID               !  the address of the hit detector element
          parameter ( CC_ID = 1 ) 
          integer CC_TDC               !  tdc information (channels)
          parameter ( CC_TDC = 2 ) 
          integer CC_ADC               !  adc information (channels)
          parameter ( CC_ADC = 3 ) 

C    Offsets for the CC01 bank follow 

          integer CC01_ID               !  the address of the hit detector element
          parameter ( CC01_ID = 1 ) 
          integer CC01_TIME               !  time(ns) 
          parameter ( CC01_TIME = 2 ) 
          integer CC01_N_PE               !  number of photoelectrons
          parameter ( CC01_N_PE = 3 ) 

C    Offsets for the CC1 bank follow 

          integer CC1_ID               !  the address of the hit detector element
          parameter ( CC1_ID = 1 ) 
          integer CC1_TDC               !  tdc information (channels)
          parameter ( CC1_TDC = 2 ) 
          integer CC1_ADC               !  adc information (channels)
          parameter ( CC1_ADC = 3 ) 

C    Offsets for the CCDI bank follow 

          integer CCDI_CRATE               !  CAMAC crate number
          parameter ( CCDI_CRATE = 1 ) 
          integer CCDI_SLOT               !  slot
          parameter ( CCDI_SLOT = 1 ) 
          integer CCDI_MASK               !  mask
          parameter ( CCDI_MASK = 1 ) 
          integer CCDI_THRESHOLD               !  actual threshold value (mV)
          parameter ( CCDI_THRESHOLD = 1 ) 
          integer CCDI_WIDTH               !  actual width value
          parameter ( CCDI_WIDTH = 1 ) 

C    Offsets for the CCH bank follow 

          integer CCH_TKNUM               !  track number, 1000*istak+itra
          parameter ( CCH_TKNUM = 1 ) 
          integer CCH_ID               !  track PDG id
          parameter ( CCH_ID = 2 ) 
          integer CCH_NHITS               !  number of CC hits per track
          parameter ( CCH_NHITS = 3 ) 
          integer CCH_SECTOR               !  sector number of track
          parameter ( CCH_SECTOR = 4 ) 
          integer CCH_SEGMENT               !  cc segment number of track
          parameter ( CCH_SEGMENT = 5 ) 
          integer CCH_PMOM               !  track momentum 
          parameter ( CCH_PMOM = 6 ) 
          integer CCH_XIN               !  x pos track entry into cerenkov
          parameter ( CCH_XIN = 7 ) 
          integer CCH_YIN               !  y pos
          parameter ( CCH_YIN = 8 ) 
          integer CCH_ZIN               !  z pos
          parameter ( CCH_ZIN = 9 ) 
          integer CCH_XOUT               !  x pos track exit from cerenkov
          parameter ( CCH_XOUT = 10 ) 
          integer CCH_YOUT               !  y pos
          parameter ( CCH_YOUT = 11 ) 
          integer CCH_ZOUT               !  z pos
          parameter ( CCH_ZOUT = 12 ) 

C    Offsets for the CCMT bank follow 

          integer CCMT_MEAN_HIGH               !  mean high threshold (mV)
          parameter ( CCMT_MEAN_HIGH = 1 ) 
          integer CCMT_MEAN_LO               !  mean lo threshold (mV)
          parameter ( CCMT_MEAN_LO = 2 ) 

C    Offsets for the CCPB bank follow 

          integer CCPB_SCSGHT               !  100*sector + Cluster # in CCRC
          parameter ( CCPB_SCSGHT = 1 ) 
          integer CCPB_NPHE               !  Number of photo-electrons*10
          parameter ( CCPB_NPHE = 2 ) 
          integer CCPB_TIME               !  Flight time relative to the evnt start time
          parameter ( CCPB_TIME = 3 ) 
          integer CCPB_PATH               !  Path lenght from target (from tracking)
          parameter ( CCPB_PATH = 4 ) 
          integer CCPB_CHI2CC               !  Geometrical matching: angle between CC hit and
          parameter ( CCPB_CHI2CC = 5 ) 
          integer CCPB_STATUS               !  CCsegment*10
          parameter ( CCPB_STATUS = 6 ) 

C    Offsets for the CCPE bank follow 

          integer CCPE_ID               !  the address of the hit detector element
          parameter ( CCPE_ID = 1 ) 
          integer CCPE_MEAN               !  adc pedestal mean value (channel)
          parameter ( CCPE_MEAN = 2 ) 
          integer CCPE_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( CCPE_SIGMA = 3 ) 

C    Offsets for the CCRC bank follow 

          integer CCRC_NRSECT               !  Sector #
          parameter ( CCRC_NRSECT = 1 ) 
          integer CCRC_NRSEGM               !  10 * Mean segment #
          parameter ( CCRC_NRSEGM = 2 ) 
          integer CCRC_NRSEGM_P               !  10 * Max segment # in the cluster
          parameter ( CCRC_NRSEGM_P = 3 ) 
          integer CCRC_NRSEGM_M               !  10 * Min segment # in the cluster
          parameter ( CCRC_NRSEGM_M = 4 ) 
          integer CCRC_NRPHE               !  Number of photoelectrons obtained
          parameter ( CCRC_NRPHE = 5 ) 
          integer CCRC_NRTIME               !  TOF in channels (50psec/channel)
          parameter ( CCRC_NRTIME = 6 ) 
          integer CCRC_NRTHET               !  Estimated angle Theta
          parameter ( CCRC_NRTHET = 7 ) 
          integer CCRC_NRDTHET               !  Estimated error of angle Theta
          parameter ( CCRC_NRDTHET = 8 ) 
          integer CCRC_NRPHY               !  Phy index 
          parameter ( CCRC_NRPHY = 9 ) 
          integer CCRC_NRIEC               !  Estimated i-coordinate in EC 
          parameter ( CCRC_NRIEC = 10 ) 
          integer CCRC_NRDIEC               !  Estimated error of i-coord. in EC
          parameter ( CCRC_NRDIEC = 11 ) 
          integer CCRC_NRSTAT               !  Status  word (yet unclear) 
          parameter ( CCRC_NRSTAT = 12 ) 

C    Offsets for the CCS bank follow 

          integer CCS_CCS1               !  scaler sector 1
          parameter ( CCS_CCS1 = 1 ) 
          integer CCS_CCS2               !  scaler sector 1
          parameter ( CCS_CCS2 = 2 ) 
          integer CCS_CCS3               !  scaler sector 1
          parameter ( CCS_CCS3 = 3 ) 
          integer CCS_CCS4               !  scaler sector 1
          parameter ( CCS_CCS4 = 4 ) 
          integer CCS_CCS5               !  scaler sector 1
          parameter ( CCS_CCS5 = 5 ) 
          integer CCS_CCS6               !  scaler sector 1
          parameter ( CCS_CCS6 = 6 ) 
          integer CCS_CCS7               !  scaler sector 1
          parameter ( CCS_CCS7 = 7 ) 
          integer CCS_CCS8               !  scaler sector 1
          parameter ( CCS_CCS8 = 8 ) 
          integer CCS_CCS9               !  scaler sector 1
          parameter ( CCS_CCS9 = 9 ) 
          integer CCS_CCS10               !  scaler sector 1
          parameter ( CCS_CCS10 = 10 ) 
          integer CCS_CCS11               !  scaler sector 1
          parameter ( CCS_CCS11 = 11 ) 
          integer CCS_CCS12               !  scaler sector 1
          parameter ( CCS_CCS12 = 12 ) 
          integer CCS_CCS13               !  scaler sector 1
          parameter ( CCS_CCS13 = 13 ) 
          integer CCS_CCS14               !  scaler sector 1
          parameter ( CCS_CCS14 = 14 ) 
          integer CCS_CCS15               !  scaler sector 1
          parameter ( CCS_CCS15 = 15 ) 
          integer CCS_CCS16               !  scaler sector 1
          parameter ( CCS_CCS16 = 16 ) 
          integer CCS_CCS17               !  scaler sector 2
          parameter ( CCS_CCS17 = 17 ) 
          integer CCS_CCS18               !  scaler sector 2
          parameter ( CCS_CCS18 = 18 ) 
          integer CCS_CCS19               !  scaler sector 2
          parameter ( CCS_CCS19 = 19 ) 
          integer CCS_CCS20               !  scaler sector 2
          parameter ( CCS_CCS20 = 20 ) 
          integer CCS_CCS21               !  scaler sector 2
          parameter ( CCS_CCS21 = 21 ) 
          integer CCS_CCS22               !  scaler sector 2
          parameter ( CCS_CCS22 = 22 ) 
          integer CCS_CCS23               !  scaler sector 2
          parameter ( CCS_CCS23 = 23 ) 
          integer CCS_CCS24               !  scaler sector 2
          parameter ( CCS_CCS24 = 24 ) 
          integer CCS_CCS25               !  scaler sector 2
          parameter ( CCS_CCS25 = 25 ) 
          integer CCS_CCS26               !  scaler sector 2
          parameter ( CCS_CCS26 = 26 ) 
          integer CCS_CCS27               !  scaler sector 2
          parameter ( CCS_CCS27 = 27 ) 
          integer CCS_CCS28               !  scaler sector 2
          parameter ( CCS_CCS28 = 28 ) 
          integer CCS_CCS29               !  scaler sector 2
          parameter ( CCS_CCS29 = 29 ) 
          integer CCS_CCS30               !  scaler sector 2
          parameter ( CCS_CCS30 = 30 ) 
          integer CCS_CCS31               !  scaler sector 2
          parameter ( CCS_CCS31 = 31 ) 
          integer CCS_CCS32               !  scaler sector 2
          parameter ( CCS_CCS32 = 32 ) 
          integer CCS_CCS33               !  scaler sector 3
          parameter ( CCS_CCS33 = 33 ) 
          integer CCS_CCS34               !  scaler sector 3
          parameter ( CCS_CCS34 = 34 ) 
          integer CCS_CCS35               !  scaler sector 3
          parameter ( CCS_CCS35 = 35 ) 
          integer CCS_CCS36               !  scaler sector 3
          parameter ( CCS_CCS36 = 36 ) 
          integer CCS_CCS37               !  scaler sector 3
          parameter ( CCS_CCS37 = 37 ) 
          integer CCS_CCS38               !  scaler sector 3
          parameter ( CCS_CCS38 = 38 ) 
          integer CCS_CCS39               !  scaler sector 3
          parameter ( CCS_CCS39 = 39 ) 
          integer CCS_CCS40               !  scaler sector 3
          parameter ( CCS_CCS40 = 40 ) 
          integer CCS_CCS41               !  scaler sector 3
          parameter ( CCS_CCS41 = 41 ) 
          integer CCS_CCS42               !  scaler sector 3
          parameter ( CCS_CCS42 = 42 ) 
          integer CCS_CCS43               !  scaler sector 3
          parameter ( CCS_CCS43 = 43 ) 
          integer CCS_CCS44               !  scaler sector 3
          parameter ( CCS_CCS44 = 44 ) 
          integer CCS_CCS45               !  scaler sector 3
          parameter ( CCS_CCS45 = 45 ) 
          integer CCS_CCS46               !  scaler sector 3
          parameter ( CCS_CCS46 = 46 ) 
          integer CCS_CCS47               !  scaler sector 3
          parameter ( CCS_CCS47 = 47 ) 
          integer CCS_CCS48               !  scaler sector 3
          parameter ( CCS_CCS48 = 48 ) 
          integer CCS_CCS49               !  scaler sector 4
          parameter ( CCS_CCS49 = 49 ) 
          integer CCS_CCS50               !  scaler sector 4
          parameter ( CCS_CCS50 = 50 ) 
          integer CCS_CCS51               !  scaler sector 4
          parameter ( CCS_CCS51 = 51 ) 
          integer CCS_CCS52               !  scaler sector 4
          parameter ( CCS_CCS52 = 52 ) 
          integer CCS_CCS53               !  scaler sector 4
          parameter ( CCS_CCS53 = 53 ) 
          integer CCS_CCS54               !  scaler sector 4
          parameter ( CCS_CCS54 = 54 ) 
          integer CCS_CCS55               !  scaler sector 4
          parameter ( CCS_CCS55 = 55 ) 
          integer CCS_CCS56               !  scaler sector 4
          parameter ( CCS_CCS56 = 56 ) 
          integer CCS_CCS57               !  scaler sector 4
          parameter ( CCS_CCS57 = 57 ) 
          integer CCS_CCS58               !  scaler sector 4
          parameter ( CCS_CCS58 = 58 ) 
          integer CCS_CCS59               !  scaler sector 4
          parameter ( CCS_CCS59 = 59 ) 
          integer CCS_CCS60               !  scaler sector 4
          parameter ( CCS_CCS60 = 60 ) 
          integer CCS_CCS61               !  scaler sector 4
          parameter ( CCS_CCS61 = 61 ) 
          integer CCS_CCS62               !  scaler sector 4
          parameter ( CCS_CCS62 = 62 ) 
          integer CCS_CCS63               !  scaler sector 4
          parameter ( CCS_CCS63 = 63 ) 
          integer CCS_CCS64               !  scaler sector 4
          parameter ( CCS_CCS64 = 64 ) 
          integer CCS_CCS65               !  scaler sector 5
          parameter ( CCS_CCS65 = 65 ) 
          integer CCS_CCS66               !  scaler sector 5
          parameter ( CCS_CCS66 = 66 ) 
          integer CCS_CCS67               !  scaler sector 5
          parameter ( CCS_CCS67 = 67 ) 
          integer CCS_CCS68               !  scaler sector 5
          parameter ( CCS_CCS68 = 68 ) 
          integer CCS_CCS69               !  scaler sector 5
          parameter ( CCS_CCS69 = 69 ) 
          integer CCS_CCS70               !  scaler sector 5
          parameter ( CCS_CCS70 = 70 ) 
          integer CCS_CCS71               !  scaler sector 5
          parameter ( CCS_CCS71 = 71 ) 
          integer CCS_CCS72               !  scaler sector 5
          parameter ( CCS_CCS72 = 72 ) 
          integer CCS_CCS73               !  scaler sector 5
          parameter ( CCS_CCS73 = 73 ) 
          integer CCS_CCS74               !  scaler sector 5
          parameter ( CCS_CCS74 = 74 ) 
          integer CCS_CCS75               !  scaler sector 5
          parameter ( CCS_CCS75 = 75 ) 
          integer CCS_CCS76               !  scaler sector 5
          parameter ( CCS_CCS76 = 76 ) 
          integer CCS_CCS77               !  scaler sector 5
          parameter ( CCS_CCS77 = 77 ) 
          integer CCS_CCS78               !  scaler sector 5
          parameter ( CCS_CCS78 = 78 ) 
          integer CCS_CCS79               !  scaler sector 5
          parameter ( CCS_CCS79 = 79 ) 
          integer CCS_CCS80               !  scaler sector 5
          parameter ( CCS_CCS80 = 80 ) 
          integer CCS_CCS81               !  scaler sector 6
          parameter ( CCS_CCS81 = 81 ) 
          integer CCS_CCS82               !  scaler sector 6
          parameter ( CCS_CCS82 = 82 ) 
          integer CCS_CCS83               !  scaler sector 6
          parameter ( CCS_CCS83 = 83 ) 
          integer CCS_CCS84               !  scaler sector 6
          parameter ( CCS_CCS84 = 84 ) 
          integer CCS_CCS85               !  scaler sector 6
          parameter ( CCS_CCS85 = 85 ) 
          integer CCS_CCS86               !  scaler sector 6
          parameter ( CCS_CCS86 = 86 ) 
          integer CCS_CCS87               !  scaler sector 6
          parameter ( CCS_CCS87 = 87 ) 
          integer CCS_CCS88               !  scaler sector 6
          parameter ( CCS_CCS88 = 88 ) 
          integer CCS_CCS89               !  scaler sector 6
          parameter ( CCS_CCS89 = 89 ) 
          integer CCS_CCS90               !  scaler sector 6
          parameter ( CCS_CCS90 = 90 ) 
          integer CCS_CCS91               !  scaler sector 6
          parameter ( CCS_CCS91 = 91 ) 
          integer CCS_CCS92               !  scaler sector 6
          parameter ( CCS_CCS92 = 92 ) 
          integer CCS_CCS93               !  scaler sector 6
          parameter ( CCS_CCS93 = 93 ) 
          integer CCS_CCS94               !  scaler sector 6
          parameter ( CCS_CCS94 = 94 ) 
          integer CCS_CCS95               !  scaler sector 6
          parameter ( CCS_CCS95 = 95 ) 
          integer CCS_CCS96               !  scaler sector 6
          parameter ( CCS_CCS96 = 96 ) 

C    Offsets for the CHI2 bank follow 

          integer CHI2_CHI2               !  overall chi2 for event  
          parameter ( CHI2_CHI2 = 1 ) 
          integer CHI2_CL               !  percentage (confidence level)
          parameter ( CHI2_CL = 2 ) 
          integer CHI2_NDF               !  number of effective degrees of freedom
          parameter ( CHI2_NDF = 3 ) 
          integer CHI2_ITER               ! 
          parameter ( CHI2_ITER = 4 ) 

C    Offsets for the CL01 bank follow 

          integer CL01_AC_AMP               !  60 Hz AC amplitude (pedestal subtracted)
          parameter ( CL01_AC_AMP = 1 ) 
          integer CL01_FC_DIODE_AMP               !  Forward Carriage diode amplitude (ped sub.)
          parameter ( CL01_FC_DIODE_AMP = 2 ) 
          integer CL01_FC_DIODE_T               !  Forward Carriage diode time
          parameter ( CL01_FC_DIODE_T = 3 ) 
          integer CL01_NC_DIODE_AMP               !  North Clamshell diode amplitude (ped sub.)
          parameter ( CL01_NC_DIODE_AMP = 4 ) 
          integer CL01_NC_DIODE_T               !  Forward  diode time
          parameter ( CL01_NC_DIODE_T = 5 ) 
          integer CL01_SC_DIODE_AMP               !  Forward Carriage diode amplitude (ped sub.)
          parameter ( CL01_SC_DIODE_AMP = 6 ) 
          integer CL01_SC_DIODE_T               !  Forward Carriage diode time(ns)
          parameter ( CL01_SC_DIODE_T = 7 ) 
          integer CL01_SF_DIODE_AMP               !  Forward Carriage diode amplitude (ped sub.)
          parameter ( CL01_SF_DIODE_AMP = 8 ) 
          integer CL01_SF_DIODE_T               !  Forward Carriage diode time(ns)
          parameter ( CL01_SF_DIODE_T = 9 ) 
          integer CL01_RF1               !  RF time 1 (ns)
          parameter ( CL01_RF1 = 10 ) 
          integer CL01_RF2               !  RF time 2 (ns)
          parameter ( CL01_RF2 = 11 ) 
          integer CL01_RF               !  GOOD RF time (ns)
          parameter ( CL01_RF = 12 ) 

C    Offsets for the CLST bank follow 

          integer CLST_CLUST               !  bit packed,  see: include/bosddl.h, clasCLST_t
          parameter ( CLST_CLUST = 1 ) 

C    Offsets for the CPED bank follow 

          integer CPED_SLOT               !  ADC slot
          parameter ( CPED_SLOT = 1 ) 
          integer CPED_CHANNEL               !  ADC channel
          parameter ( CPED_CHANNEL = 2 ) 
          integer CPED_MEAN               !  adc pedestal mean value (channel)
          parameter ( CPED_MEAN = 3 ) 
          integer CPED_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( CPED_SIGMA = 4 ) 
          integer CPED_OFFSET               !  offset for sparsification threshold calculation
          parameter ( CPED_OFFSET = 5 ) 

C    Offsets for the DC0 bank follow 

          integer DC0_ID               !  the address of the hit detector element
          parameter ( DC0_ID = 1 ) 
          integer DC0_TDC               !  tdc information (channels)
          parameter ( DC0_TDC = 2 ) 

C    Offsets for the DC1 bank follow 

          integer DC1_ID               !  the address of the hit detector element
          parameter ( DC1_ID = 1 ) 
          integer DC1_TIME               !  time(ns) 
          parameter ( DC1_TIME = 2 ) 

C    Offsets for the DCDW bank follow 

          integer DCDW_ID               !  ID_wire 
          parameter ( DCDW_ID = 1 ) 
          integer DCDW_TIDLY               !  TIme DeLaY (ns) 		 
          parameter ( DCDW_TIDLY = 2 ) 
          integer DCDW_STAT               !  wire status word 
          parameter ( DCDW_STAT = 3 ) 

C    Offsets for the DCGM bank follow 

          integer DCGM_X_CURVE               !  x center of curvature (cm)
          parameter ( DCGM_X_CURVE = 1 ) 
          integer DCGM_Y_CURVE               !  y center of curvature (cm)
          parameter ( DCGM_Y_CURVE = 2 ) 
          integer DCGM_Z_CURVE               !  z center of curvature (cm)
          parameter ( DCGM_Z_CURVE = 3 ) 
          integer DCGM_R_CURVE               !  radius of curvature (cm)
          parameter ( DCGM_R_CURVE = 4 ) 
          integer DCGM_THETA_START               !  angle of first logical wire WRT the center of curvature (rad)
          parameter ( DCGM_THETA_START = 5 ) 
          integer DCGM_D_THETA               !  delta theta between wires WRT center of curvature
          parameter ( DCGM_D_THETA = 6 ) 
          integer DCGM_X_NMID               !  x normal vector to the midplane
          parameter ( DCGM_X_NMID = 7 ) 
          integer DCGM_Y_NMID               !  y normal vector to the midplane
          parameter ( DCGM_Y_NMID = 8 ) 
          integer DCGM_Z_NMID               !  z normal vector to the midplane
          parameter ( DCGM_Z_NMID = 9 ) 
          integer DCGM_THETA_MIN               !  theta of first physical wire (rad)
          parameter ( DCGM_THETA_MIN = 10 ) 
          integer DCGM_THETA_MAX               !  theta of last physical wire (rad)
          parameter ( DCGM_THETA_MAX = 11 ) 
          integer DCGM_MIN_WIRE               !  minimum physical wire number
          parameter ( DCGM_MIN_WIRE = 12 ) 
          integer DCGM_MAX_WIRE               !  maximum physical wire number
          parameter ( DCGM_MAX_WIRE = 13 ) 
          integer DCGM_STEREO               !  approximate stereo angle
          parameter ( DCGM_STEREO = 14 ) 
          integer DCGM_CELL_SIZE               !  cell size (cm)
          parameter ( DCGM_CELL_SIZE = 15 ) 
          integer DCGM_X_NORM               !  x normal vector to the plane(region 1)
          parameter ( DCGM_X_NORM = 16 ) 
          integer DCGM_Y_NORM               !  y normal vector to the plane(region 1)
          parameter ( DCGM_Y_NORM = 17 ) 
          integer DCGM_Z_NORM               !  z normal vector to the plane(region 1)
          parameter ( DCGM_Z_NORM = 18 ) 
          integer DCGM_P_DIST               !  distance of plane to origin(cm) (region 1)
          parameter ( DCGM_P_DIST = 19 ) 
          integer DCGM_P_SEP               !  planar separation(cm) (region 1)
          parameter ( DCGM_P_SEP = 20 ) 
          integer DCGM_MAX_CYLW               !  maximum cylindrical wire
          parameter ( DCGM_MAX_CYLW = 21 ) 

C    Offsets for the DCGW bank follow 

          integer DCGW_X_MID               !  x at midplane of wire(cm)
          parameter ( DCGW_X_MID = 1 ) 
          integer DCGW_Y_MID               !  y at midplane of wire(cm)
          parameter ( DCGW_Y_MID = 2 ) 
          integer DCGW_Z_MID               !  z at midplane of wire(cm)
          parameter ( DCGW_Z_MID = 3 ) 
          integer DCGW_X_DIR               !  x direction cosine along wire (cm)
          parameter ( DCGW_X_DIR = 4 ) 
          integer DCGW_Y_DIR               !  y direction cosine along wire (cm)
          parameter ( DCGW_Y_DIR = 5 ) 
          integer DCGW_Z_DIR               !  z direction cosine along wire (cm)
          parameter ( DCGW_Z_DIR = 6 ) 
          integer DCGW_W_LEN               !  wire length from midplane to amplifier (cm)
          parameter ( DCGW_W_LEN = 7 ) 
          integer DCGW_W_LEN_HV               !  wire length from midplane to HV (cm)
          parameter ( DCGW_W_LEN_HV = 8 ) 

C    Offsets for the DCH bank follow 

          integer DCH_X               !  x at layer center
          parameter ( DCH_X = 1 ) 
          integer DCH_Y               !  y at layer center
          parameter ( DCH_Y = 2 ) 
          integer DCH_Z               !  z at layer center
          parameter ( DCH_Z = 3 ) 
          integer DCH_STEP               !  step size through layer
          parameter ( DCH_STEP = 4 ) 
          integer DCH_DEDX               !  energy deposit in layer
          parameter ( DCH_DEDX = 5 ) 
          integer DCH_PMOM               !  track momentum at layer center 
          parameter ( DCH_PMOM = 6 ) 
          integer DCH_TIME               !  time of hit at layer center
          parameter ( DCH_TIME = 7 ) 
          integer DCH_CX               !  track x dir cosine at layer center
          parameter ( DCH_CX = 8 ) 
          integer DCH_CY               !  track y dir cosine at layer center
          parameter ( DCH_CY = 9 ) 
          integer DCH_CZ               !  track z dir cosine at layer center
          parameter ( DCH_CZ = 10 ) 
          integer DCH_TRACK               !  track number
          parameter ( DCH_TRACK = 11 ) 
          integer DCH_ID               !  track PDG id
          parameter ( DCH_ID = 12 ) 
          integer DCH_LAYER               !  layer number
          parameter ( DCH_LAYER = 13 ) 

C    Offsets for the DCPB bank follow 

          integer DCPB_SCTR               !  100*sector+track_ID in *BTR  
          parameter ( DCPB_SCTR = 1 ) 
          integer DCPB_X_SC               !  x coordinate of track intersection with SC plane 
          parameter ( DCPB_X_SC = 2 ) 
          integer DCPB_Y_SC               !  y coordinate of track intersection with SC plane
          parameter ( DCPB_Y_SC = 3 ) 
          integer DCPB_Z_SC               !  z coordinate of track intersection with SC plane
          parameter ( DCPB_Z_SC = 4 ) 
          integer DCPB_CX_SC               !  X dir cosine at (x_SC,y_SC,z_SC)
          parameter ( DCPB_CX_SC = 5 ) 
          integer DCPB_CY_SC               !  y dir cosine at (x_SC,y_SC,z_SC)
          parameter ( DCPB_CY_SC = 6 ) 
          integer DCPB_CZ_SC               !  z dir cosine at (x_SC,y_SC,z_SC)
          parameter ( DCPB_CZ_SC = 7 ) 
          integer DCPB_X_V               !  vertex X after fiting to the beam position
          parameter ( DCPB_X_V = 8 ) 
          integer DCPB_Y_V               !  vertex Y after fiting to the beam position
          parameter ( DCPB_Y_V = 9 ) 
          integer DCPB_Z_V               !  vertex Z after fiting to the beam position
          parameter ( DCPB_Z_V = 10 ) 
          integer DCPB_R_V               !  distance from production vertex to the bemam. 
          parameter ( DCPB_R_V = 11 ) 
          integer DCPB_CHI2               !  Chisquare of track fitting
          parameter ( DCPB_CHI2 = 12 ) 
          integer DCPB_STATUS               !  Status word
          parameter ( DCPB_STATUS = 13 ) 

C    Offsets for the DCV1 bank follow 

          integer DCV1_TSR1               !  Tsmear 
          parameter ( DCV1_TSR1 = 1 ) 
          integer DCV1_V0R1               !  drift velocity (slope)
          parameter ( DCV1_V0R1 = 2 ) 
          integer DCV1_TMR1               !  Maximum drift time (Tmax)
          parameter ( DCV1_TMR1 = 3 ) 
          integer DCV1_SP1R1               !  spare  
          parameter ( DCV1_SP1R1 = 4 ) 
          integer DCV1_SP2R1               !  spare	 
          parameter ( DCV1_SP2R1 = 5 ) 

C    Offsets for the DCV2 bank follow 

          integer DCV2_TS1R2               !  Tsmear 
          parameter ( DCV2_TS1R2 = 1 ) 
          integer DCV2_V01R2               !  drift velocity (slope)
          parameter ( DCV2_V01R2 = 2 ) 
          integer DCV2_VA1R2               !  drift velocity function parameter
          parameter ( DCV2_VA1R2 = 3 ) 
          integer DCV2_VB1R2               !  drift velocity function parameter  
          parameter ( DCV2_VB1R2 = 4 ) 
          integer DCV2_TM1R2               !  Maximum drift time (Tmax) 		 
          parameter ( DCV2_TM1R2 = 5 ) 
          integer DCV2_TA1R2               !  Tmax function parameter
          parameter ( DCV2_TA1R2 = 6 ) 
          integer DCV2_TB1R2               !  Tmax function parameter
          parameter ( DCV2_TB1R2 = 7 ) 
          integer DCV2_TS2R2               !  Tsmear 
          parameter ( DCV2_TS2R2 = 8 ) 
          integer DCV2_V02R2               !  drift velocity (slope)
          parameter ( DCV2_V02R2 = 9 ) 
          integer DCV2_VA2R2               !  drift velocity function parameter
          parameter ( DCV2_VA2R2 = 10 ) 
          integer DCV2_VB2R2               !  drift velocity function parameter  
          parameter ( DCV2_VB2R2 = 11 ) 
          integer DCV2_TM2R2               !  Maximum drift time (Tmax) 		 
          parameter ( DCV2_TM2R2 = 12 ) 
          integer DCV2_TA2R2               !  Tmax function parameter
          parameter ( DCV2_TA2R2 = 13 ) 
          integer DCV2_TB2R2               !  Tmax function parameter
          parameter ( DCV2_TB2R2 = 14 ) 

C    Offsets for the DCV3 bank follow 

          integer DCV3_TSR3               !  Tsmear 
          parameter ( DCV3_TSR3 = 1 ) 
          integer DCV3_V0R3               !  drift velocity (slope)
          parameter ( DCV3_V0R3 = 2 ) 
          integer DCV3_TMR3               !  Maximum drift time (Tmax)
          parameter ( DCV3_TMR3 = 3 ) 
          integer DCV3_SP1R3               !  spare  
          parameter ( DCV3_SP1R3 = 4 ) 
          integer DCV3_SP2R3               !  spare	 
          parameter ( DCV3_SP2R3 = 5 ) 

C    Offsets for the DDLY bank follow 

          integer DDLY_ID               !  ID_wire 
          parameter ( DDLY_ID = 1 ) 
          integer DDLY_TIDLY               !  TIme DeLaY (ns) 		 
          parameter ( DDLY_TIDLY = 2 ) 
          integer DDLY_STAT               !  wire STATus = 100*C + 10*A + B
          parameter ( DDLY_STAT = 3 ) 

C    Offsets for the DGEO bank follow 

          integer DGEO_ID_SEC               !  ID_sector 
          parameter ( DGEO_ID_SEC = 1 ) 
          integer DGEO_ID_REG               !  ID_region 
          parameter ( DGEO_ID_REG = 2 ) 
          integer DGEO_XPOS               !  x misalignment 		 
          parameter ( DGEO_XPOS = 3 ) 
          integer DGEO_YPOS               !  y misalignment 		 
          parameter ( DGEO_YPOS = 4 ) 
          integer DGEO_ZPOS               !  z misalignment 		 
          parameter ( DGEO_ZPOS = 5 ) 
          integer DGEO_SXPOS               !  sx sine of little x angle 		 
          parameter ( DGEO_SXPOS = 6 ) 
          integer DGEO_SYPOS               !  sy sine of little y angle 
          parameter ( DGEO_SYPOS = 7 ) 
          integer DGEO_SZPOS               !  sz sine of little z angle 
          parameter ( DGEO_SZPOS = 8 ) 

C    Offsets for the DHCL bank follow 

          integer DHCL_SLY               !  superlayer
          parameter ( DHCL_SLY = 1 ) 
          integer DHCL_BTRK               !  track_in_sector# (bit set)
          parameter ( DHCL_BTRK = 2 ) 
          integer DHCL_TRKS1               !  combinations to track segments
          parameter ( DHCL_TRKS1 = 3 ) 
          integer DHCL_TRKS2               !    "             "       "
          parameter ( DHCL_TRKS2 = 4 ) 
          integer DHCL_WIRE1               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE1 = 5 ) 
          integer DHCL_BWIR1               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR1 = 6 ) 
          integer DHCL_WIRE2               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE2 = 7 ) 
          integer DHCL_BWIR2               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR2 = 8 ) 
          integer DHCL_WIRE3               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE3 = 9 ) 
          integer DHCL_BWIR3               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR3 = 10 ) 
          integer DHCL_WIRE4               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE4 = 11 ) 
          integer DHCL_BWIR4               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR4 = 12 ) 
          integer DHCL_WIRE5               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE5 = 13 ) 
          integer DHCL_BWIR5               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR5 = 14 ) 
          integer DHCL_WIRE6               !  1.wire# in this cluster in 1.layer
          parameter ( DHCL_WIRE6 = 15 ) 
          integer DHCL_BWIR6               !  hits in this layer (starting from WIRE1) (bit st)
          parameter ( DHCL_BWIR6 = 16 ) 

C    Offsets for the DITM bank follow 

          integer DITM_TIME               !  time of discriminator calibration
          parameter ( DITM_TIME = 1 ) 

C    Offsets for the DOCA bank follow 

          integer DOCA_ID               !  the address of the hit detector element
          parameter ( DOCA_ID = 1 ) 
          integer DOCA_DOCA               !  doca from GSIM (micron)
          parameter ( DOCA_DOCA = 2 ) 

C    Offsets for the DPCP bank follow 

          integer DPCP_ID               !  the address of the hit detector element
          parameter ( DPCP_ID = 1 ) 
          integer DPCP_MN_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_MN_MEAN = 2 ) 
          integer DPCP_MN_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_MN_SIGMA = 3 ) 
          integer DPCP_LT_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_LT_MEAN = 4 ) 
          integer DPCP_LT_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_LT_SIGMA = 5 ) 
          integer DPCP_RB_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_RB_MEAN = 6 ) 
          integer DPCP_RB_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_RB_SIGMA = 7 ) 
          integer DPCP_LB_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_LB_MEAN = 8 ) 
          integer DPCP_LB_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_LB_SIGMA = 9 ) 
          integer DPCP_RT_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_RT_MEAN = 10 ) 
          integer DPCP_RT_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_RT_SIGMA = 11 ) 
          integer DPCP_VT_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPCP_VT_MEAN = 12 ) 
          integer DPCP_VT_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPCP_VT_SIGMA = 13 ) 

C    Offsets for the DPSP bank follow 

          integer DPSP_ID               !  the address of the hit detector element
          parameter ( DPSP_ID = 1 ) 
          integer DPSP_MEAN               !  adc pedestal mean value (channel)
          parameter ( DPSP_MEAN = 2 ) 
          integer DPSP_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DPSP_SIGMA = 3 ) 

C    Offsets for the DSPC bank follow 

          integer DSPC_PCID               !  Id compelled by DAQ (always 1)
          parameter ( DSPC_PCID = 1 ) 
          integer DSPC_TDCPC               !  tdc information ( scintillator)
          parameter ( DSPC_TDCPC = 2 ) 
          integer DSPC_ADCMN               !  adc information (main)
          parameter ( DSPC_ADCMN = 3 ) 
          integer DSPC_ADCLT               !  adc information (left top)
          parameter ( DSPC_ADCLT = 4 ) 
          integer DSPC_ADCRB               !  adc information (right bottom)
          parameter ( DSPC_ADCRB = 5 ) 
          integer DSPC_ADCLB               !  adc information (left bottom)
          parameter ( DSPC_ADCLB = 6 ) 
          integer DSPC_ADCRT               !  adc information (right top)
          parameter ( DSPC_ADCRT = 7 ) 
          integer DSPC_ADCVE               !  adc information (veto)
          parameter ( DSPC_ADCVE = 8 ) 

C    Offsets for the DSPS bank follow 

          integer DSPS_ID               !  paddle ID (left = 1 to 4)( right = 5 to 8)
          parameter ( DSPS_ID = 1 ) 
          integer DSPS_TDC               !  tdc information
          parameter ( DSPS_TDC = 2 ) 
          integer DSPS_ADC               !  adc information 
          parameter ( DSPS_ADC = 3 ) 

C    Offsets for the DSTC bank follow 

          integer DSTC_TACID               !  ID compelled by DAQ (always 1)
          parameter ( DSTC_TACID = 1 ) 
          integer DSTC_TDCTAC               !  tdc information (on sum)
          parameter ( DSTC_TDCTAC = 2 ) 
          integer DSTC_ADCLT               !  adc information (left top)
          parameter ( DSTC_ADCLT = 3 ) 
          integer DSTC_ADCRT               !  adc information (right top)
          parameter ( DSTC_ADCRT = 4 ) 
          integer DSTC_ADCLB               !  adc information (left bottom)
          parameter ( DSTC_ADCLB = 5 ) 
          integer DSTC_ADCRB               !  adc information (right bottom)
          parameter ( DSTC_ADCRB = 6 ) 
          integer DSTC_ADCSUM1               !  adc information (sum scale1)
          parameter ( DSTC_ADCSUM1 = 7 ) 
          integer DSTC_ADCSUM2               !  adc information (sum scale2)
          parameter ( DSTC_ADCSUM2 = 8 ) 
          integer DSTC_ADCSUM3               !  adc information (sum scale3)
          parameter ( DSTC_ADCSUM3 = 9 ) 

C    Offsets for the DTCP bank follow 

          integer DTCP_ID               !  the address of the hit detector element
          parameter ( DTCP_ID = 1 ) 
          integer DTCP_LT_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_LT_MEAN = 2 ) 
          integer DTCP_LT_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_LT_SIGMA = 3 ) 
          integer DTCP_RT_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_RT_MEAN = 4 ) 
          integer DTCP_RT_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_RT_SIGMA = 5 ) 
          integer DTCP_LB_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_LB_MEAN = 6 ) 
          integer DTCP_LB_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_LB_SIGMA = 7 ) 
          integer DTCP_RB_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_RB_MEAN = 8 ) 
          integer DTCP_RB_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_RB_SIGMA = 9 ) 
          integer DTCP_SUM1_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_SUM1_MEAN = 10 ) 
          integer DTCP_SUM1_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_SUM1_SIGMA = 11 ) 
          integer DTCP_SUM2_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_SUM2_MEAN = 12 ) 
          integer DTCP_SUM2_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_SUM2_SIGMA = 13 ) 
          integer DTCP_SUM3_MEAN               !  adc pedestal mean value (channel)
          parameter ( DTCP_SUM3_MEAN = 14 ) 
          integer DTCP_SUM3_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( DTCP_SUM3_SIGMA = 15 ) 

C    Offsets for the DTRK bank follow 

          integer DTRK_X               ! 
          parameter ( DTRK_X = 1 ) 
          integer DTRK_Y               ! 
          parameter ( DTRK_Y = 2 ) 
          integer DTRK_Z               ! 
          parameter ( DTRK_Z = 3 ) 

C    Offsets for the EC bank follow 

          integer EC_ID               !  the address of the hit detector element
          parameter ( EC_ID = 1 ) 
          integer EC_TDC               !  tdc information (channels)
          parameter ( EC_TDC = 2 ) 
          integer EC_ADC               !  adc information (channels)
          parameter ( EC_ADC = 3 ) 

C    Offsets for the EC01 bank follow 

          integer EC01_ID               !  the address of the hit detector element
          parameter ( EC01_ID = 1 ) 
          integer EC01_TIME               !  time for left paddle(ns) 
          parameter ( EC01_TIME = 2 ) 
          integer EC01_ENERGY               !  energy in left paddle(MeV) 
          parameter ( EC01_ENERGY = 3 ) 

C    Offsets for the EC1 bank follow 

          integer EC1_ID               !  the address of the hit detector element
          parameter ( EC1_ID = 1 ) 
          integer EC1_TDCL               !  tdc information (channels)
          parameter ( EC1_TDCL = 2 ) 
          integer EC1_ADCL               !  adc information (channels)
          parameter ( EC1_ADCL = 3 ) 
          integer EC1_TDCR               !  tdc information (channels)
          parameter ( EC1_TDCR = 4 ) 
          integer EC1_ADCR               !  adc information (channels)
          parameter ( EC1_ADCR = 5 ) 

C    Offsets for the EC1P bank follow 

          integer EC1P_N1X               !  x component of outward normal to plane
          parameter ( EC1P_N1X = 1 ) 
          integer EC1P_N1Y               !  y component of outward normal to plane
          parameter ( EC1P_N1Y = 2 ) 
          integer EC1P_N1Z               !  z component of outward normal to plane
          parameter ( EC1P_N1Z = 3 ) 
          integer EC1P_R1N               !  distanse in cm from origin to plane
          parameter ( EC1P_R1N = 4 ) 

C    Offsets for the EC1R bank follow 

          integer EC1R_E_TOT               !  cluster energy (sum of inner+outer)
          parameter ( EC1R_E_TOT = 1 ) 
          integer EC1R_DE_TOT               !   error on the cluster energy
          parameter ( EC1R_DE_TOT = 2 ) 
          integer EC1R_T_TOT               !  time
          parameter ( EC1R_T_TOT = 3 ) 
          integer EC1R_DT_TOT               !  error on the time
          parameter ( EC1R_DT_TOT = 4 ) 
          integer EC1R_X_M               !  x-in CLAS frame
          parameter ( EC1R_X_M = 5 ) 
          integer EC1R_Y_M               !  y-in CLAS frame
          parameter ( EC1R_Y_M = 6 ) 
          integer EC1R_Z_M               !  z-in CLAS frame
          parameter ( EC1R_Z_M = 7 ) 
          integer EC1R_DX_M               !  error on x
          parameter ( EC1R_DX_M = 8 ) 
          integer EC1R_DY_M               !  error on y
          parameter ( EC1R_DY_M = 9 ) 
          integer EC1R_DZ_M               !  error on z
          parameter ( EC1R_DZ_M = 10 ) 
          integer EC1R_E_IN               !   cluster energy in inner layer
          parameter ( EC1R_E_IN = 11 ) 
          integer EC1R_T_IN               !  time from inner layer
          parameter ( EC1R_T_IN = 12 ) 
          integer EC1R_X_IN               !  lab coordinate , inner layer
          parameter ( EC1R_X_IN = 13 ) 
          integer EC1R_Y_IN               !  lab coordinate , inner layer
          parameter ( EC1R_Y_IN = 14 ) 
          integer EC1R_X_OUT               !  lab coordinate , outer layer
          parameter ( EC1R_X_OUT = 15 ) 
          integer EC1R_Y_OUT               !  lab coordinate , outer layer
          parameter ( EC1R_Y_OUT = 16 ) 
          integer EC1R_X2_IN_L               !  second moment of x inner left
          parameter ( EC1R_X2_IN_L = 17 ) 
          integer EC1R_X2_IN_R               !  second moment of x inner right 
          parameter ( EC1R_X2_IN_R = 18 ) 
          integer EC1R_Y2_IN_L               !  second moment of y inner left
          parameter ( EC1R_Y2_IN_L = 19 ) 
          integer EC1R_Y2_IN_R               !  second moment of y inner right
          parameter ( EC1R_Y2_IN_R = 20 ) 
          integer EC1R_X2_OUT_L               !  second moment of x outer hit left
          parameter ( EC1R_X2_OUT_L = 21 ) 
          integer EC1R_X2_OUT_R               !  second moment of x outer hit right
          parameter ( EC1R_X2_OUT_R = 22 ) 
          integer EC1R_Y2_OUT_L               !  second moment of y outer hit left
          parameter ( EC1R_Y2_OUT_L = 23 ) 
          integer EC1R_Y2_OUT_R               !  second moment of y outer hit right
          parameter ( EC1R_Y2_OUT_R = 24 ) 
          integer EC1R_I_IN               !  cluster center in X inner short layer
          parameter ( EC1R_I_IN = 25 ) 
          integer EC1R_J_IN               !  cluster center in Y inner long layer
          parameter ( EC1R_J_IN = 26 ) 
          integer EC1R_I_OUT               !  cluster center in X outer layers
          parameter ( EC1R_I_OUT = 27 ) 
          integer EC1R_J_OUT               !  cluster center in Y outer layer
          parameter ( EC1R_J_OUT = 28 ) 
          integer EC1R_A_IN_XL               !  energy sum in inner short left pmts
          parameter ( EC1R_A_IN_XL = 29 ) 
          integer EC1R_A_IN_XR               !  energy sum in inner short right pmts
          parameter ( EC1R_A_IN_XR = 30 ) 
          integer EC1R_A_IN_YL               !  energy sum in inner long  left pmts
          parameter ( EC1R_A_IN_YL = 31 ) 
          integer EC1R_A_IN_YR               !  energy sum in inner long  right pmts
          parameter ( EC1R_A_IN_YR = 32 ) 
          integer EC1R_A_OUT_XL               !  energy sum in outer short left pmts
          parameter ( EC1R_A_OUT_XL = 33 ) 
          integer EC1R_A_OUT_XR               !  energy sum in outer short right pmts
          parameter ( EC1R_A_OUT_XR = 34 ) 
          integer EC1R_A_OUT_YL               !  energy sum in outer long left pmts
          parameter ( EC1R_A_OUT_YL = 35 ) 
          integer EC1R_A_OUT_YR               !  energy sum in outer long right pmts
          parameter ( EC1R_A_OUT_YR = 36 ) 
          integer EC1R_T_IN_XS               !  tdc sum in inner short righ+left  for cluster center
          parameter ( EC1R_T_IN_XS = 37 ) 
          integer EC1R_T_IN_XD               !  tdc dif in inner short right-left 
          parameter ( EC1R_T_IN_XD = 38 ) 
          integer EC1R_T_IN_YS               !  tdc sum in inner long  righ+left
          parameter ( EC1R_T_IN_YS = 39 ) 
          integer EC1R_T_IN_YD               !  tdc dif in inner long  right-left
          parameter ( EC1R_T_IN_YD = 40 ) 
          integer EC1R_T_OUT_XS               !  tdc sum in outer short righ+left 
          parameter ( EC1R_T_OUT_XS = 41 ) 
          integer EC1R_T_OUT_XD               !  tdc dif in outer short right-left 
          parameter ( EC1R_T_OUT_XD = 42 ) 
          integer EC1R_T_OUT_YS               !  tdc sum in outer long righ+left
          parameter ( EC1R_T_OUT_YS = 43 ) 
          integer EC1R_T_OUT_YD               !  tdc dif in outer long right-left 
          parameter ( EC1R_T_OUT_YD = 44 ) 
          integer EC1R_IBL               !  LAC  block number
          parameter ( EC1R_IBL = 45 ) 
          integer EC1R_NCLUSTER               !  1000xNclust4+100xNclust3+10xNclust2+Nclust1 
          parameter ( EC1R_NCLUSTER = 46 ) 
          integer EC1R_PMTFIRED               !  Number of fired pmt (more than threshold) 
          parameter ( EC1R_PMTFIRED = 47 ) 
          integer EC1R_Z_IN               !  Z in CLAS frame
          parameter ( EC1R_Z_IN = 48 ) 
          integer EC1R_Z_OUT               !  Z out in CLAS frame
          parameter ( EC1R_Z_OUT = 49 ) 
          integer EC1R_ISTAT               !  status word
          parameter ( EC1R_ISTAT = 50 ) 

C    Offsets for the ECCA bank follow 

          integer ECCA_ID               !  PMT ID
          parameter ( ECCA_ID = 1 ) 
          integer ECCA_APED               !  ADC pedestals (channels)
          parameter ( ECCA_APED = 2 ) 
          integer ECCA_ASIG               !  ADC pedestal variance (channels)
          parameter ( ECCA_ASIG = 3 ) 
          integer ECCA_AMIP               !  ADC calibration from MIP (N.I.M.P/ch.)
          parameter ( ECCA_AMIP = 4 ) 
          integer ECCA_AMIPU               !  aMIP Error
          parameter ( ECCA_AMIPU = 5 ) 
          integer ECCA_ASHR               !  ADC calibration from showers (GeV/ch.)
          parameter ( ECCA_ASHR = 6 ) 
          integer ECCA_ASHRU               !  aSHR Error
          parameter ( ECCA_ASHRU = 7 ) 
          integer ECCA_STAT               !  4 byte status word    
          parameter ( ECCA_STAT = 8 ) 

C    Offsets for the ECCL bank follow 

          integer ECCL_ID               !  PMT ID
          parameter ( ECCL_ID = 1 ) 
          integer ECCL_LDB               !  Stack atten length (database)(cm)
          parameter ( ECCL_LDB = 2 ) 
          integer ECCL_LDBU               !  lDB Error (cm)
          parameter ( ECCL_LDBU = 3 ) 
          integer ECCL_LMIP               !  Stack atten length (MinIonPart)(cm) 
          parameter ( ECCL_LMIP = 4 ) 
          integer ECCL_LMIPU               !  lMIP Error (cm)
          parameter ( ECCL_LMIPU = 5 ) 
          integer ECCL_LSHR               !  Stack atten length (showers)(cm)  
          parameter ( ECCL_LSHR = 6 ) 
          integer ECCL_LSHRU               !  lSHR Error (cm) 
          parameter ( ECCL_LSHRU = 7 ) 
          integer ECCL_STAT               !  4 byte status word    
          parameter ( ECCL_STAT = 8 ) 

C    Offsets for the ECCT bank follow 

          integer ECCT_ID               !  PMT ID
          parameter ( ECCT_ID = 1 ) 
          integer ECCT_TOFF               !  TDC offset (channels)
          parameter ( ECCT_TOFF = 2 ) 
          integer ECCT_TOFFU               !  tOFF Error
          parameter ( ECCT_TOFFU = 3 ) 
          integer ECCT_TGAIN               !  TDC conversion gain (nS/channel)
          parameter ( ECCT_TGAIN = 4 ) 
          integer ECCT_TGAINU               !  tGAIN Error
          parameter ( ECCT_TGAINU = 5 ) 
          integer ECCT_TW0               !  Time walk constant (channels)
          parameter ( ECCT_TW0 = 6 ) 
          integer ECCT_TW0U               !  tWOu Error
          parameter ( ECCT_TW0U = 7 ) 
          integer ECCT_TW1               !  Time walk correction parameter 
          parameter ( ECCT_TW1 = 8 ) 
          integer ECCT_TW1U               !  tW1u Error
          parameter ( ECCT_TW1U = 9 ) 
          integer ECCT_TVEF               !  Effective velocity of light (cm/ns)
          parameter ( ECCT_TVEF = 10 ) 
          integer ECCT_TVEFU               !  tVEFu Error
          parameter ( ECCT_TVEFU = 11 ) 
          integer ECCT_STAT               !  4 byte status word    
          parameter ( ECCT_STAT = 12 ) 

C    Offsets for the ECDI bank follow 

          integer ECDI_CRATE               !  CAMAC crate number
          parameter ( ECDI_CRATE = 1 ) 
          integer ECDI_SLOT               !  slot
          parameter ( ECDI_SLOT = 1 ) 
          integer ECDI_MASK               !  mask
          parameter ( ECDI_MASK = 1 ) 
          integer ECDI_THRESHOLD               !  actual threshold value (mV)
          parameter ( ECDI_THRESHOLD = 1 ) 
          integer ECDI_WIDTH               !  actual width value
          parameter ( ECDI_WIDTH = 1 ) 

C    Offsets for the ECG bank follow 

          integer ECG_L0               !  distance from the target 
          parameter ( ECG_L0 = 1 ) 
          integer ECG_THE0               !  angle between the beam and perpendicular
          parameter ( ECG_THE0 = 2 ) 
          integer ECG_YLOW               !  
          parameter ( ECG_YLOW = 3 ) 
          integer ECG_YHI               !  
          parameter ( ECG_YHI = 4 ) 
          integer ECG_DYLOW               !  
          parameter ( ECG_DYLOW = 5 ) 
          integer ECG_DYHI               !  
          parameter ( ECG_DYHI = 6 ) 
          integer ECG_LRTH               !  thickness of the single layer
          parameter ( ECG_LRTH = 6 ) 
          integer ECG_TANGR               !  
          parameter ( ECG_TANGR = 7 ) 
          integer ECG_SECTOR               !  
          parameter ( ECG_SECTOR = 8 ) 
          integer ECG_PHISEC               !  
          parameter ( ECG_PHISEC = 9 ) 
          integer ECG_X0OFF               !  
          parameter ( ECG_X0OFF = 10 ) 
          integer ECG_Y0OFF               !  
          parameter ( ECG_Y0OFF = 11 ) 
          integer ECG_Z0OFF               !  
          parameter ( ECG_Z0OFF = 12 ) 
          integer ECG_ROTM11               !  
          parameter ( ECG_ROTM11 = 13 ) 
          integer ECG_ROTM12               !  
          parameter ( ECG_ROTM12 = 14 ) 
          integer ECG_ROTM13               !  
          parameter ( ECG_ROTM13 = 15 ) 
          integer ECG_ROTM21               !  
          parameter ( ECG_ROTM21 = 16 ) 
          integer ECG_ROTM22               !  
          parameter ( ECG_ROTM22 = 17 ) 
          integer ECG_ROTM23               !  
          parameter ( ECG_ROTM23 = 18 ) 
          integer ECG_ROTM31               !  
          parameter ( ECG_ROTM31 = 19 ) 
          integer ECG_ROTM32               !  
          parameter ( ECG_ROTM32 = 20 ) 
          integer ECG_ROTM33               !  
          parameter ( ECG_ROTM33 = 21 ) 

C    Offsets for the ECH bank follow 

          integer ECH_X               !  x of hit
          parameter ( ECH_X = 1 ) 
          integer ECH_Y               !  y of hit
          parameter ( ECH_Y = 2 ) 
          integer ECH_Z               !  z of hit
          parameter ( ECH_Z = 3 ) 
          integer ECH_CX               !  track x dir cosine
          parameter ( ECH_CX = 4 ) 
          integer ECH_CY               !  track y dir cosine
          parameter ( ECH_CY = 5 ) 
          integer ECH_CZ               !  track z dir cosine
          parameter ( ECH_CZ = 6 ) 
          integer ECH_PMOM               !  track momentum
          parameter ( ECH_PMOM = 7 ) 
          integer ECH_ID               !  track PDG id
          parameter ( ECH_ID = 8 ) 
          integer ECH_TOF               !  time of flight
          parameter ( ECH_TOF = 9 ) 
          integer ECH_EDEPIN               !  deposit energy (inner part)
          parameter ( ECH_EDEPIN = 10 ) 
          integer ECH_EDEPOUT               !  deposit energy (outer part)
          parameter ( ECH_EDEPOUT = 11 ) 

C    Offsets for the ECHB bank follow 

          integer ECHB_SECT               !  Sector number & Layer number
          parameter ( ECHB_SECT = 1 ) 
          integer ECHB_E__HIT               !  energy found 
          parameter ( ECHB_E__HIT = 2 ) 
          integer ECHB_DE_HIT               !  error on the energy found 
          parameter ( ECHB_DE_HIT = 3 ) 
          integer ECHB_T_HIT               !  time found 
          parameter ( ECHB_T_HIT = 4 ) 
          integer ECHB_DT_HI               !  error time found 
          parameter ( ECHB_DT_HI = 5 ) 
          integer ECHB_I_HIT               !  sector rectangular coordinate 
          parameter ( ECHB_I_HIT = 6 ) 
          integer ECHB_J_HIT               !  sector rectangular coordinate 
          parameter ( ECHB_J_HIT = 7 ) 
          integer ECHB_DI_HIT               !  sector rectangular coordinate error,
          parameter ( ECHB_DI_HIT = 8 ) 
          integer ECHB_DJ_HIT               !  sector rectangular coordinate error,
          parameter ( ECHB_DJ_HIT = 9 ) 
          integer ECHB_X_HIT               !  lab coordinate,
          parameter ( ECHB_X_HIT = 10 ) 
          integer ECHB_Y_HIT               !  lab coordinate,
          parameter ( ECHB_Y_HIT = 11 ) 
          integer ECHB_Z_HIT               !  lab coordinate,
          parameter ( ECHB_Z_HIT = 12 ) 
          integer ECHB_DX_HIT               !  lab coordinate error,
          parameter ( ECHB_DX_HIT = 13 ) 
          integer ECHB_DY_HIT               !  lab coordinate error, 
          parameter ( ECHB_DY_HIT = 14 ) 
          integer ECHB_DZ_HIT               !  lab coordinate error,
          parameter ( ECHB_DZ_HIT = 15 ) 
          integer ECHB_U2_HIT               !  second moment of u  _hit pattern
          parameter ( ECHB_U2_HIT = 16 ) 
          integer ECHB_V2_HIT               !  second moment of v  _hit pattern
          parameter ( ECHB_V2_HIT = 17 ) 
          integer ECHB_W2_HIT               !  second moment of w  _hit pattern
          parameter ( ECHB_W2_HIT = 18 ) 
          integer ECHB_U3_HIT               !  third moment of u  _hit pattern 
          parameter ( ECHB_U3_HIT = 19 ) 
          integer ECHB_V3_HIT               !  third moment of v  _hit pattern 
          parameter ( ECHB_V3_HIT = 20 ) 
          integer ECHB_W3_HIT               !  third moment of w  _hit pattern 
          parameter ( ECHB_W3_HIT = 21 ) 
          integer ECHB_U4_HIT               !  forth moment of u  _hit pattern 
          parameter ( ECHB_U4_HIT = 22 ) 
          integer ECHB_V4_HIT               !  forth moment of v  _hit pattern 
          parameter ( ECHB_V4_HIT = 23 ) 
          integer ECHB_W4_HIT               !  forth moment of w  _hit pattern 
          parameter ( ECHB_W4_HIT = 24 ) 
          integer ECHB_CENTR_U               !  peak position on U axis 
          parameter ( ECHB_CENTR_U = 25 ) 
          integer ECHB_CENTR_V               !  peak position on V axis 
          parameter ( ECHB_CENTR_V = 26 ) 
          integer ECHB_CENTR_W               !  peak position on W axis 
          parameter ( ECHB_CENTR_W = 27 ) 
          integer ECHB_PATH_U               !  path length from hit position to U axis 
          parameter ( ECHB_PATH_U = 28 ) 
          integer ECHB_PATH_V               !  path length from hit position to V axis 
          parameter ( ECHB_PATH_V = 29 ) 
          integer ECHB_PATH_W               !  path length from hit position to W axis 
          parameter ( ECHB_PATH_W = 30 ) 
          integer ECHB_NSTRP_U               !  Number of U strips in the hit
          parameter ( ECHB_NSTRP_U = 31 ) 
          integer ECHB_NSTRP_V               !  Number of V strips in the hit
          parameter ( ECHB_NSTRP_V = 32 ) 
          integer ECHB_NSTRP_W               !  Number of W strips in the hit
          parameter ( ECHB_NSTRP_W = 33 ) 
          integer ECHB_MATCHID1               !  Id of matched hit in the layer1
          parameter ( ECHB_MATCHID1 = 34 ) 
          integer ECHB_CH21               !  Quality measure of matching with layer1
          parameter ( ECHB_CH21 = 35 ) 
          integer ECHB_MATCHID2               !  Id of matched hit in the layer2
          parameter ( ECHB_MATCHID2 = 36 ) 
          integer ECHB_CH22               !  Quality measure of matching with layer2
          parameter ( ECHB_CH22 = 37 ) 
          integer ECHB_ISTAT               !  Number of hits & hit ID
          parameter ( ECHB_ISTAT = 38 ) 

C    Offsets for the ECMT bank follow 

          integer ECMT_IN_HIGH               !  inner high threshold (mV)
          parameter ( ECMT_IN_HIGH = 1 ) 
          integer ECMT_OUT_HIGH               !  outer high threshold (mV)
          parameter ( ECMT_OUT_HIGH = 2 ) 
          integer ECMT_TOTAL_HIGH               !  total high threshold (mV)
          parameter ( ECMT_TOTAL_HIGH = 3 ) 
          integer ECMT_IN_LO               !  inner lo threshold (mV)
          parameter ( ECMT_IN_LO = 4 ) 
          integer ECMT_OUT_LO               !  outer lo threshold (mV)
          parameter ( ECMT_OUT_LO = 5 ) 
          integer ECMT_TOTAL_LO               !  total lo threshold (mV)
          parameter ( ECMT_TOTAL_LO = 6 ) 

C    Offsets for the ECP bank follow 

          integer ECP_N1X               !  x component of outward normal to plane
          parameter ( ECP_N1X = 1 ) 
          integer ECP_N1Y               !  y component of outward normal to plane
          parameter ( ECP_N1Y = 2 ) 
          integer ECP_N1Z               !  z component of outward normal to plane
          parameter ( ECP_N1Z = 3 ) 
          integer ECP_R1N               !  distanse in cm from origin to plane
          parameter ( ECP_R1N = 4 ) 
          integer ECP_PLW               !  whole plane depth
          parameter ( ECP_PLW = 5 ) 
          integer ECP_PLI               !  inner plane depth
          parameter ( ECP_PLI = 6 ) 
          integer ECP_PLO               !  outer plane depth
          parameter ( ECP_PLO = 7 ) 

C    Offsets for the ECP1 bank follow 

          integer ECP1_ID               !  the address of the hit detector element
          parameter ( ECP1_ID = 1 ) 
          integer ECP1_MEAN_LEFT               !  left adc pedestal mean value (channel)
          parameter ( ECP1_MEAN_LEFT = 2 ) 
          integer ECP1_SIGMA_LEFT               !  sigma of the pedestal distribution for left adc (channel)
          parameter ( ECP1_SIGMA_LEFT = 3 ) 
          integer ECP1_MEAN_RIGHT               !  right adc pedestal mean value (channel)
          parameter ( ECP1_MEAN_RIGHT = 4 ) 
          integer ECP1_SIGMA_RIGHT               !  sigma of the pedestal distribution for right adc (channel
          parameter ( ECP1_SIGMA_RIGHT = 5 ) 

C    Offsets for the ECPB bank follow 

          integer ECPB_SCHT               !  100*sector+Whole_Hit_ID in ECHB 
          parameter ( ECPB_SCHT = 1 ) 
          integer ECPB_ETOT               !  Reconstructed total energy
          parameter ( ECPB_ETOT = 2 ) 
          integer ECPB_EIN               !  Inner energy
          parameter ( ECPB_EIN = 3 ) 
          integer ECPB_EOUT               !  Outer energy 
          parameter ( ECPB_EOUT = 4 ) 
          integer ECPB_TIME               !  Flight time relative to the evnt start time
          parameter ( ECPB_TIME = 5 ) 
          integer ECPB_PATH               !  Path lenght from target
          parameter ( ECPB_PATH = 6 ) 
          integer ECPB_X               !  x coordinate of hit 
          parameter ( ECPB_X = 7 ) 
          integer ECPB_Y               !  y coordinate of hit
          parameter ( ECPB_Y = 8 ) 
          integer ECPB_Z               !  z coordinate of hit
          parameter ( ECPB_Z = 9 ) 
          integer ECPB_M2_HIT               !  second moment of _hit pattern
          parameter ( ECPB_M2_HIT = 10 ) 
          integer ECPB_M3_HIT               !  third moment of  _hit pattern 
          parameter ( ECPB_M3_HIT = 11 ) 
          integer ECPB_M4_HIT               !  forth moment of  _hit pattern 
          parameter ( ECPB_M4_HIT = 12 ) 
          integer ECPB_INNSTR               !  10000*UI+100*VI+WI 
          parameter ( ECPB_INNSTR = 13 ) 
          integer ECPB_OUTSTR               !  10000*UO+100*VO+WO 
          parameter ( ECPB_OUTSTR = 14 ) 
          integer ECPB_CHI2EC               !  Quality measure of geometrical matching
          parameter ( ECPB_CHI2EC = 15 ) 
          integer ECPB_STATUS               !  Status word (not implemented yet)
          parameter ( ECPB_STATUS = 16 ) 

C    Offsets for the ECPC bank follow 

          integer ECPC_ID               !  layer(1-3) * 100 + strip ID
          parameter ( ECPC_ID = 1 ) 
          integer ECPC_TDC               !  tdc (channels)
          parameter ( ECPC_TDC = 2 ) 
          integer ECPC_ADC               !  adc - pedestal (channels)
          parameter ( ECPC_ADC = 3 ) 

C    Offsets for the ECPE bank follow 

          integer ECPE_ID               !  the address of the hit detector element
          parameter ( ECPE_ID = 1 ) 
          integer ECPE_MEAN               !  adc pedestal mean value (channel)
          parameter ( ECPE_MEAN = 2 ) 
          integer ECPE_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( ECPE_SIGMA = 3 ) 

C    Offsets for the ECPI bank follow 

          integer ECPI_ID               !  sector number
          parameter ( ECPI_ID = 1 ) 
          integer ECPI_LAYER               !  layer number, note 1-inner, 2-outer, 3-whole
          parameter ( ECPI_LAYER = 2 ) 
          integer ECPI_HITID               !  number of hits (first 16 bits) and hit ID (last 16 bits)
          parameter ( ECPI_HITID = 3 ) 
          integer ECPI_ILOC               !  position of the hit in the local coordinate system
          parameter ( ECPI_ILOC = 4 ) 
          integer ECPI_JLOC               !  position of the hit in the local coordinate system
          parameter ( ECPI_JLOC = 5 ) 
          integer ECPI_DILOC               !  i width of the hit
          parameter ( ECPI_DILOC = 6 ) 
          integer ECPI_DJLOC               !  j width of the hit 
          parameter ( ECPI_DJLOC = 7 ) 
          integer ECPI_R               !  radius of the shower
          parameter ( ECPI_R = 8 ) 
          integer ECPI_E               !  energy
          parameter ( ECPI_E = 9 ) 

C    Offsets for the ECPO bank follow 

          integer ECPO_UID1               !  Pointer to the U-strips 1-18 for hit
          parameter ( ECPO_UID1 = 1 ) 
          integer ECPO_UID2               !  Pointer to the U-strips 19-3 for hit
          parameter ( ECPO_UID2 = 2 ) 
          integer ECPO_VID1               !  Pointer to the V-strips 1-18 for hit
          parameter ( ECPO_VID1 = 3 ) 
          integer ECPO_VID2               !  Pointer to the V-strips 19-3 for hit
          parameter ( ECPO_VID2 = 4 ) 
          integer ECPO_WID1               !  Pointer to the W-strips 1-18 for hit 
          parameter ( ECPO_WID1 = 5 ) 
          integer ECPO_WID2               !  Pointer to the W-strips 19-36 for hit 
          parameter ( ECPO_WID2 = 6 ) 
          integer ECPO_SLH               !  Sector*1000+Layer*100+Hit
          parameter ( ECPO_SLH = 7 ) 

C    Offsets for the ECRB bank follow 

          integer ECRB_E_IN               !  energy found for the inner layer
          parameter ( ECRB_E_IN = 1 ) 
          integer ECRB_E_OUT               !  energy found for the outer layer
          parameter ( ECRB_E_OUT = 2 ) 
          integer ECRB_DE_IN               !  error on the energy found for the inner layer
          parameter ( ECRB_DE_IN = 3 ) 
          integer ECRB_DE_OUT               !  error on the energy found for the outer layer
          parameter ( ECRB_DE_OUT = 4 ) 
          integer ECRB_T_IN               !  time found for the inner layer
          parameter ( ECRB_T_IN = 5 ) 
          integer ECRB_T_OUT               !  time found for the outer layer
          parameter ( ECRB_T_OUT = 6 ) 
          integer ECRB_DT_IN               !  error on the time found for the inner layer
          parameter ( ECRB_DT_IN = 7 ) 
          integer ECRB_DT_OUT               !  error on the time found for the outer layer
          parameter ( ECRB_DT_OUT = 8 ) 
          integer ECRB_I_IN               !  sector rectangular coordinate for the inner layer
          parameter ( ECRB_I_IN = 9 ) 
          integer ECRB_J_IN               !  sector rectangular coordinate for the inner layer
          parameter ( ECRB_J_IN = 10 ) 
          integer ECRB_I_OUT               !  sector rectangular coordinate for the outer layer
          parameter ( ECRB_I_OUT = 11 ) 
          integer ECRB_J_OUT               !  sector rectangular coordinate for the outer layer
          parameter ( ECRB_J_OUT = 12 ) 
          integer ECRB_DI_IN               !  sector rectangular coordinate error, inner layer
          parameter ( ECRB_DI_IN = 13 ) 
          integer ECRB_DJ_IN               !  sector rectangular coordinate error, inner layer
          parameter ( ECRB_DJ_IN = 14 ) 
          integer ECRB_DI_OUT               !  sector rectangular coordinate error, outer layer
          parameter ( ECRB_DI_OUT = 15 ) 
          integer ECRB_DJ_OUT               !  sector rectangular coordinate error, outer layer
          parameter ( ECRB_DJ_OUT = 16 ) 
          integer ECRB_X_IN               !  lab coordinate, inner layer
          parameter ( ECRB_X_IN = 17 ) 
          integer ECRB_Y_IN               !  lab coordinate, inner layer
          parameter ( ECRB_Y_IN = 18 ) 
          integer ECRB_Z_IN               !  lab coordinate, inner layer
          parameter ( ECRB_Z_IN = 19 ) 
          integer ECRB_X_OUT               !  lab coordinate, outer layer
          parameter ( ECRB_X_OUT = 20 ) 
          integer ECRB_Y_OUT               !  lab coordinate, outer layer
          parameter ( ECRB_Y_OUT = 21 ) 
          integer ECRB_Z_OUT               !  lab coordinate, outer layer
          parameter ( ECRB_Z_OUT = 22 ) 
          integer ECRB_DX_IN               !  lab coordinate error, inner layer
          parameter ( ECRB_DX_IN = 23 ) 
          integer ECRB_DY_IN               !  lab coordinate error, inner layer
          parameter ( ECRB_DY_IN = 24 ) 
          integer ECRB_DZ_IN               !  lab coordinate error, inner layer
          parameter ( ECRB_DZ_IN = 25 ) 
          integer ECRB_DX_OUT               !  lab coordinate error, outer layer
          parameter ( ECRB_DX_OUT = 26 ) 
          integer ECRB_DY_OUT               !  lab coordinate error, outer layer
          parameter ( ECRB_DY_OUT = 27 ) 
          integer ECRB_DZ_OUT               !  lab coordinate error, outer layer
          parameter ( ECRB_DZ_OUT = 28 ) 
          integer ECRB_U2_IN               !  second moment of u inner hit pattern
          parameter ( ECRB_U2_IN = 29 ) 
          integer ECRB_V2_IN               !  second moment of v inner hit pattern
          parameter ( ECRB_V2_IN = 30 ) 
          integer ECRB_W2_IN               !  second moment of w inner hit pattern
          parameter ( ECRB_W2_IN = 31 ) 
          integer ECRB_U2_OUT               !  second moment of u outer hit pattern
          parameter ( ECRB_U2_OUT = 32 ) 
          integer ECRB_V2_OUT               !  second moment of v outer hit pattern
          parameter ( ECRB_V2_OUT = 33 ) 
          integer ECRB_W2_OUT               !  second moment of w outer hit pattern
          parameter ( ECRB_W2_OUT = 34 ) 
          integer ECRB_U3_IN               !  third moment of u inner hit pattern 
          parameter ( ECRB_U3_IN = 35 ) 
          integer ECRB_V3_IN               !  third moment of v inner hit pattern 
          parameter ( ECRB_V3_IN = 36 ) 
          integer ECRB_W3_IN               !  third moment of w inner hit pattern 
          parameter ( ECRB_W3_IN = 37 ) 
          integer ECRB_U3_OUT               !  third moment of u outer hit pattern 
          parameter ( ECRB_U3_OUT = 38 ) 
          integer ECRB_V3_OUT               !  third moment of v outer hit pattern 
          parameter ( ECRB_V3_OUT = 39 ) 
          integer ECRB_W3_OUT               !  third moment of w outer hit pattern 
          parameter ( ECRB_W3_OUT = 40 ) 
          integer ECRB_I2               !  second moment of overall shower, sector coordinates
          parameter ( ECRB_I2 = 41 ) 
          integer ECRB_J2               !  second moment of overall shower, sector coordinates
          parameter ( ECRB_J2 = 42 ) 
          integer ECRB_I3               !  third moment of overall shower, sector coordinates
          parameter ( ECRB_I3 = 43 ) 
          integer ECRB_J3               !  third moment of overall shower, sector coordinates
          parameter ( ECRB_J3 = 44 ) 
          integer ECRB_SPARE1               !  spare
          parameter ( ECRB_SPARE1 = 45 ) 
          integer ECRB_SPARE2               !  spare
          parameter ( ECRB_SPARE2 = 46 ) 
          integer ECRB_SPARE3               !  spare
          parameter ( ECRB_SPARE3 = 47 ) 
          integer ECRB_SPARE4               !  spare
          parameter ( ECRB_SPARE4 = 48 ) 
          integer ECRB_ISTAT               !  status word
          parameter ( ECRB_ISTAT = 49 ) 

C    Offsets for the ECS bank follow 

          integer ECS_ECS1               !  scaler sector 1
          parameter ( ECS_ECS1 = 1 ) 
          integer ECS_ECS2               !  scaler sector 1
          parameter ( ECS_ECS2 = 2 ) 
          integer ECS_ECS3               !  scaler sector 1
          parameter ( ECS_ECS3 = 3 ) 
          integer ECS_ECS4               !  scaler sector 1
          parameter ( ECS_ECS4 = 4 ) 
          integer ECS_ECS5               !  scaler sector 1
          parameter ( ECS_ECS5 = 5 ) 
          integer ECS_ECS6               !  scaler sector 1
          parameter ( ECS_ECS6 = 6 ) 
          integer ECS_ECS7               !  scaler sector 1
          parameter ( ECS_ECS7 = 7 ) 
          integer ECS_ECS8               !  scaler sector 1
          parameter ( ECS_ECS8 = 8 ) 
          integer ECS_ECS9               !  scaler sector 1
          parameter ( ECS_ECS9 = 9 ) 
          integer ECS_ECS10               !  scaler sector 1
          parameter ( ECS_ECS10 = 10 ) 
          integer ECS_ECS11               !  scaler sector 1
          parameter ( ECS_ECS11 = 11 ) 
          integer ECS_ECS12               !  scaler sector 1
          parameter ( ECS_ECS12 = 12 ) 
          integer ECS_ECS13               !  scaler sector 1
          parameter ( ECS_ECS13 = 13 ) 
          integer ECS_ECS14               !  scaler sector 1
          parameter ( ECS_ECS14 = 14 ) 
          integer ECS_ECS15               !  scaler sector 1
          parameter ( ECS_ECS15 = 15 ) 
          integer ECS_ECS16               !  scaler sector 1
          parameter ( ECS_ECS16 = 16 ) 
          integer ECS_ECS17               !  scaler sector 2
          parameter ( ECS_ECS17 = 17 ) 
          integer ECS_ECS18               !  scaler sector 2
          parameter ( ECS_ECS18 = 18 ) 
          integer ECS_ECS19               !  scaler sector 2
          parameter ( ECS_ECS19 = 19 ) 
          integer ECS_ECS20               !  scaler sector 2
          parameter ( ECS_ECS20 = 20 ) 
          integer ECS_ECS21               !  scaler sector 2
          parameter ( ECS_ECS21 = 21 ) 
          integer ECS_ECS22               !  scaler sector 2
          parameter ( ECS_ECS22 = 22 ) 
          integer ECS_ECS23               !  scaler sector 2
          parameter ( ECS_ECS23 = 23 ) 
          integer ECS_ECS24               !  scaler sector 2
          parameter ( ECS_ECS24 = 24 ) 
          integer ECS_ECS25               !  scaler sector 2
          parameter ( ECS_ECS25 = 25 ) 
          integer ECS_ECS26               !  scaler sector 2
          parameter ( ECS_ECS26 = 26 ) 
          integer ECS_ECS27               !  scaler sector 2
          parameter ( ECS_ECS27 = 27 ) 
          integer ECS_ECS28               !  scaler sector 2
          parameter ( ECS_ECS28 = 28 ) 
          integer ECS_ECS29               !  scaler sector 2
          parameter ( ECS_ECS29 = 29 ) 
          integer ECS_ECS30               !  scaler sector 2
          parameter ( ECS_ECS30 = 30 ) 
          integer ECS_ECS31               !  scaler sector 2
          parameter ( ECS_ECS31 = 31 ) 
          integer ECS_ECS32               !  scaler sector 2
          parameter ( ECS_ECS32 = 32 ) 
          integer ECS_ECS33               !  scaler sector 3
          parameter ( ECS_ECS33 = 33 ) 
          integer ECS_ECS34               !  scaler sector 3
          parameter ( ECS_ECS34 = 34 ) 
          integer ECS_ECS35               !  scaler sector 3
          parameter ( ECS_ECS35 = 35 ) 
          integer ECS_ECS36               !  scaler sector 3
          parameter ( ECS_ECS36 = 36 ) 
          integer ECS_ECS37               !  scaler sector 3
          parameter ( ECS_ECS37 = 37 ) 
          integer ECS_ECS38               !  scaler sector 3
          parameter ( ECS_ECS38 = 38 ) 
          integer ECS_ECS39               !  scaler sector 3
          parameter ( ECS_ECS39 = 39 ) 
          integer ECS_ECS40               !  scaler sector 3
          parameter ( ECS_ECS40 = 40 ) 
          integer ECS_ECS41               !  scaler sector 3
          parameter ( ECS_ECS41 = 41 ) 
          integer ECS_ECS42               !  scaler sector 3
          parameter ( ECS_ECS42 = 42 ) 
          integer ECS_ECS43               !  scaler sector 3
          parameter ( ECS_ECS43 = 43 ) 
          integer ECS_ECS44               !  scaler sector 3
          parameter ( ECS_ECS44 = 44 ) 
          integer ECS_ECS45               !  scaler sector 3
          parameter ( ECS_ECS45 = 45 ) 
          integer ECS_ECS46               !  scaler sector 3
          parameter ( ECS_ECS46 = 46 ) 
          integer ECS_ECS47               !  scaler sector 3
          parameter ( ECS_ECS47 = 47 ) 
          integer ECS_ECS48               !  scaler sector 3
          parameter ( ECS_ECS48 = 48 ) 
          integer ECS_ECS49               !  scaler sector 4
          parameter ( ECS_ECS49 = 49 ) 
          integer ECS_ECS50               !  scaler sector 4
          parameter ( ECS_ECS50 = 50 ) 
          integer ECS_ECS51               !  scaler sector 4
          parameter ( ECS_ECS51 = 51 ) 
          integer ECS_ECS52               !  scaler sector 4
          parameter ( ECS_ECS52 = 52 ) 
          integer ECS_ECS53               !  scaler sector 4
          parameter ( ECS_ECS53 = 53 ) 
          integer ECS_ECS54               !  scaler sector 4
          parameter ( ECS_ECS54 = 54 ) 
          integer ECS_ECS55               !  scaler sector 4
          parameter ( ECS_ECS55 = 55 ) 
          integer ECS_ECS56               !  scaler sector 4
          parameter ( ECS_ECS56 = 56 ) 
          integer ECS_ECS57               !  scaler sector 4
          parameter ( ECS_ECS57 = 57 ) 
          integer ECS_ECS58               !  scaler sector 4
          parameter ( ECS_ECS58 = 58 ) 
          integer ECS_ECS59               !  scaler sector 4
          parameter ( ECS_ECS59 = 59 ) 
          integer ECS_ECS60               !  scaler sector 4
          parameter ( ECS_ECS60 = 60 ) 
          integer ECS_ECS61               !  scaler sector 4
          parameter ( ECS_ECS61 = 61 ) 
          integer ECS_ECS62               !  scaler sector 4
          parameter ( ECS_ECS62 = 62 ) 
          integer ECS_ECS63               !  scaler sector 4
          parameter ( ECS_ECS63 = 63 ) 
          integer ECS_ECS64               !  scaler sector 4
          parameter ( ECS_ECS64 = 64 ) 
          integer ECS_ECS65               !  scaler sector 5
          parameter ( ECS_ECS65 = 65 ) 
          integer ECS_ECS66               !  scaler sector 5
          parameter ( ECS_ECS66 = 66 ) 
          integer ECS_ECS67               !  scaler sector 5
          parameter ( ECS_ECS67 = 67 ) 
          integer ECS_ECS68               !  scaler sector 5
          parameter ( ECS_ECS68 = 68 ) 
          integer ECS_ECS69               !  scaler sector 5
          parameter ( ECS_ECS69 = 69 ) 
          integer ECS_ECS70               !  scaler sector 5
          parameter ( ECS_ECS70 = 70 ) 
          integer ECS_ECS71               !  scaler sector 5
          parameter ( ECS_ECS71 = 71 ) 
          integer ECS_ECS72               !  scaler sector 5
          parameter ( ECS_ECS72 = 72 ) 
          integer ECS_ECS73               !  scaler sector 5
          parameter ( ECS_ECS73 = 73 ) 
          integer ECS_ECS74               !  scaler sector 5
          parameter ( ECS_ECS74 = 74 ) 
          integer ECS_ECS75               !  scaler sector 5
          parameter ( ECS_ECS75 = 75 ) 
          integer ECS_ECS76               !  scaler sector 5
          parameter ( ECS_ECS76 = 76 ) 
          integer ECS_ECS77               !  scaler sector 5
          parameter ( ECS_ECS77 = 77 ) 
          integer ECS_ECS78               !  scaler sector 5
          parameter ( ECS_ECS78 = 78 ) 
          integer ECS_ECS79               !  scaler sector 5
          parameter ( ECS_ECS79 = 79 ) 
          integer ECS_ECS80               !  scaler sector 5
          parameter ( ECS_ECS80 = 80 ) 
          integer ECS_ECS81               !  scaler sector 6
          parameter ( ECS_ECS81 = 81 ) 
          integer ECS_ECS82               !  scaler sector 6
          parameter ( ECS_ECS82 = 82 ) 
          integer ECS_ECS83               !  scaler sector 6
          parameter ( ECS_ECS83 = 83 ) 
          integer ECS_ECS84               !  scaler sector 6
          parameter ( ECS_ECS84 = 84 ) 
          integer ECS_ECS85               !  scaler sector 6
          parameter ( ECS_ECS85 = 85 ) 
          integer ECS_ECS86               !  scaler sector 6
          parameter ( ECS_ECS86 = 86 ) 
          integer ECS_ECS87               !  scaler sector 6
          parameter ( ECS_ECS87 = 87 ) 
          integer ECS_ECS88               !  scaler sector 6
          parameter ( ECS_ECS88 = 88 ) 
          integer ECS_ECS89               !  scaler sector 6
          parameter ( ECS_ECS89 = 89 ) 
          integer ECS_ECS90               !  scaler sector 6
          parameter ( ECS_ECS90 = 90 ) 
          integer ECS_ECS91               !  scaler sector 6
          parameter ( ECS_ECS91 = 91 ) 
          integer ECS_ECS92               !  scaler sector 6
          parameter ( ECS_ECS92 = 92 ) 
          integer ECS_ECS93               !  scaler sector 6
          parameter ( ECS_ECS93 = 93 ) 
          integer ECS_ECS94               !  scaler sector 6
          parameter ( ECS_ECS94 = 94 ) 
          integer ECS_ECS95               !  scaler sector 6
          parameter ( ECS_ECS95 = 95 ) 
          integer ECS_ECS96               !  scaler sector 6
          parameter ( ECS_ECS96 = 96 ) 

C    Offsets for the EID0 bank follow 

          integer EID0_JEID0SEC               !   Sector # for electron candidate
          parameter ( EID0_JEID0SEC = 1 ) 
          integer EID0_JEID0CC               !   Pointer to the hit in CCRC bank
          parameter ( EID0_JEID0CC = 2 ) 
          integer EID0_JEID0EC               !   Pointer to the hit in ECHB bank
          parameter ( EID0_JEID0EC = 3 ) 
          integer EID0_JEID0SC               !   Pointer to the hit in SCRX bank
          parameter ( EID0_JEID0SC = 4 ) 

C    Offsets for the EPIC bank follow 

          integer EPIC_VALUE               !  value of the epics channel
          parameter ( EPIC_VALUE = 1 ) 
          integer EPIC_CHAR1               !  char[32] name;
          parameter ( EPIC_CHAR1 = 2 ) 
          integer EPIC_CHAR2               !  
          parameter ( EPIC_CHAR2 = 3 ) 
          integer EPIC_CHAR3               !  
          parameter ( EPIC_CHAR3 = 4 ) 
          integer EPIC_CHAR4               !  
          parameter ( EPIC_CHAR4 = 5 ) 
          integer EPIC_CHAR5               !  
          parameter ( EPIC_CHAR5 = 6 ) 
          integer EPIC_CHAR6               !  
          parameter ( EPIC_CHAR6 = 7 ) 
          integer EPIC_CHAR7               !  
          parameter ( EPIC_CHAR7 = 8 ) 
          integer EPIC_CHAR8               !  
          parameter ( EPIC_CHAR8 = 9 ) 

C    Offsets for the EVNT bank follow 

          integer EVNT_ID               !  Particle Data Group ID (from SEB) 
          parameter ( EVNT_ID = 1 ) 
          integer EVNT_PMOM               !  momentum (from tracking)
          parameter ( EVNT_PMOM = 2 ) 
          integer EVNT_MASS               !  mass squared (from SEB =p**2(1.-betta**2)/beta**2) 
          parameter ( EVNT_MASS = 3 ) 
          integer EVNT_CHARGE               !  charge (from tracking)
          parameter ( EVNT_CHARGE = 4 ) 
          integer EVNT_BETTA               !  Particle velocity in the units of c (=R_trk/TOF/c)
          parameter ( EVNT_BETTA = 5 ) 
          integer EVNT_CX               !  x dir cosine at track origin 
          parameter ( EVNT_CX = 6 ) 
          integer EVNT_CY               !  y dir cosine at track origin
          parameter ( EVNT_CY = 7 ) 
          integer EVNT_CZ               !  z dir cosine at track origin
          parameter ( EVNT_CZ = 8 ) 
          integer EVNT_X               !  X coordinate of vertex (cm)
          parameter ( EVNT_X = 9 ) 
          integer EVNT_Y               !  Y coordinate of vertex (cm)
          parameter ( EVNT_Y = 10 ) 
          integer EVNT_Z               !  Z coordinate of vertex (cm)
          parameter ( EVNT_Z = 11 ) 
          integer EVNT_DCSTAT               !  Pointer to DCPB bank (=0 if DC is not involved) 
          parameter ( EVNT_DCSTAT = 12 ) 
          integer EVNT_CCSTAT               !  Pointer to CCPB bank (=0 if CC is not involved) 
          parameter ( EVNT_CCSTAT = 13 ) 
          integer EVNT_SCSTAT               !  Pointer to SCPB bank (=0 if SC is not involved)  
          parameter ( EVNT_SCSTAT = 14 ) 
          integer EVNT_ECSTAT               !  Pointer to ECPB bank (=0 if EC is not involved) 
          parameter ( EVNT_ECSTAT = 15 ) 
          integer EVNT_LCSTAT               !  Pointer to LCPB bank (=0 if LAC is not involved) 
          parameter ( EVNT_LCSTAT = 16 ) 
          integer EVNT_STSTAT               !  Pointer to STPB bank (=0 if ST is not involved) 
          parameter ( EVNT_STSTAT = 17 ) 
          integer EVNT_STATUS               !  Status word (=0 out of time particle)
          parameter ( EVNT_STATUS = 18 ) 

C    Offsets for the FBPM bank follow 

          integer FBPM_ID               !  the address of the hit detector element
          parameter ( FBPM_ID = 1 ) 
          integer FBPM_TDC               !  tdc information (channels)
          parameter ( FBPM_TDC = 2 ) 
          integer FBPM_ADC               !  adc information (channels)
          parameter ( FBPM_ADC = 3 ) 

C    Offsets for the G1SL bank follow 

          integer G1SL_G1T1               !  
          parameter ( G1SL_G1T1 = 1 ) 
          integer G1SL_G1T2               !  
          parameter ( G1SL_G1T2 = 2 ) 
          integer G1SL_G1T3               !  
          parameter ( G1SL_G1T3 = 3 ) 
          integer G1SL_G1T4               !  
          parameter ( G1SL_G1T4 = 4 ) 
          integer G1SL_G1T5               !  
          parameter ( G1SL_G1T5 = 5 ) 
          integer G1SL_G1T6               !  
          parameter ( G1SL_G1T6 = 6 ) 
          integer G1SL_G1T7               !  
          parameter ( G1SL_G1T7 = 7 ) 
          integer G1SL_G1T8               !  
          parameter ( G1SL_G1T8 = 8 ) 
          integer G1SL_G1T9               !  
          parameter ( G1SL_G1T9 = 9 ) 
          integer G1SL_G1T10               !  
          parameter ( G1SL_G1T10 = 10 ) 
          integer G1SL_G1T11               !  
          parameter ( G1SL_G1T11 = 11 ) 
          integer G1SL_G1T12               !  
          parameter ( G1SL_G1T12 = 12 ) 
          integer G1SL_G1T13               !  
          parameter ( G1SL_G1T13 = 13 ) 
          integer G1SL_G1T14               !  
          parameter ( G1SL_G1T14 = 14 ) 
          integer G1SL_G1T15               !  
          parameter ( G1SL_G1T15 = 15 ) 
          integer G1SL_G1T16               !  
          parameter ( G1SL_G1T16 = 16 ) 
          integer G1SL_G1T17               !  
          parameter ( G1SL_G1T17 = 17 ) 
          integer G1SL_G1T18               !  
          parameter ( G1SL_G1T18 = 18 ) 
          integer G1SL_G1T19               !  
          parameter ( G1SL_G1T19 = 19 ) 
          integer G1SL_G1T20               !  
          parameter ( G1SL_G1T20 = 20 ) 
          integer G1SL_G1T21               !  
          parameter ( G1SL_G1T21 = 21 ) 
          integer G1SL_G1T22               !  
          parameter ( G1SL_G1T22 = 22 ) 
          integer G1SL_G1T23               !  
          parameter ( G1SL_G1T23 = 23 ) 
          integer G1SL_G1T24               !  
          parameter ( G1SL_G1T24 = 24 ) 
          integer G1SL_G1T25               !  
          parameter ( G1SL_G1T25 = 25 ) 
          integer G1SL_G1T26               !  
          parameter ( G1SL_G1T26 = 26 ) 
          integer G1SL_G1T27               !  
          parameter ( G1SL_G1T27 = 27 ) 
          integer G1SL_G1T28               !  
          parameter ( G1SL_G1T28 = 28 ) 
          integer G1SL_G1T29               !  
          parameter ( G1SL_G1T29 = 29 ) 
          integer G1SL_G1T30               !  
          parameter ( G1SL_G1T30 = 30 ) 
          integer G1SL_G1T31               !  
          parameter ( G1SL_G1T31 = 31 ) 
          integer G1SL_G1T32               !  
          parameter ( G1SL_G1T32 = 32 ) 
          integer G1SL_G1T33               !  
          parameter ( G1SL_G1T33 = 33 ) 
          integer G1SL_G1T34               !  
          parameter ( G1SL_G1T34 = 34 ) 
          integer G1SL_G1T35               !  
          parameter ( G1SL_G1T35 = 35 ) 
          integer G1SL_G1T36               !  
          parameter ( G1SL_G1T36 = 36 ) 
          integer G1SL_G1T37               !  
          parameter ( G1SL_G1T37 = 37 ) 
          integer G1SL_G1T38               !  
          parameter ( G1SL_G1T38 = 38 ) 
          integer G1SL_G1T39               !  
          parameter ( G1SL_G1T39 = 39 ) 
          integer G1SL_G1T40               !  
          parameter ( G1SL_G1T40 = 40 ) 
          integer G1SL_G1T41               !  
          parameter ( G1SL_G1T41 = 41 ) 
          integer G1SL_G1T42               !  
          parameter ( G1SL_G1T42 = 42 ) 
          integer G1SL_G1T43               !  
          parameter ( G1SL_G1T43 = 43 ) 
          integer G1SL_G1T44               !  
          parameter ( G1SL_G1T44 = 44 ) 
          integer G1SL_G1T45               !  
          parameter ( G1SL_G1T45 = 45 ) 
          integer G1SL_G1T46               !  
          parameter ( G1SL_G1T46 = 46 ) 
          integer G1SL_G1T47               !  
          parameter ( G1SL_G1T47 = 47 ) 
          integer G1SL_G1T48               !  
          parameter ( G1SL_G1T48 = 48 ) 
          integer G1SL_G1T49               !  
          parameter ( G1SL_G1T49 = 49 ) 
          integer G1SL_G1T50               !  
          parameter ( G1SL_G1T50 = 50 ) 
          integer G1SL_G1T51               !  
          parameter ( G1SL_G1T51 = 51 ) 
          integer G1SL_G1T52               !  
          parameter ( G1SL_G1T52 = 52 ) 
          integer G1SL_G1T53               !  
          parameter ( G1SL_G1T53 = 53 ) 
          integer G1SL_G1T54               !  
          parameter ( G1SL_G1T54 = 54 ) 
          integer G1SL_G1T55               !  
          parameter ( G1SL_G1T55 = 55 ) 
          integer G1SL_G1T56               !  
          parameter ( G1SL_G1T56 = 56 ) 
          integer G1SL_G1T57               !  
          parameter ( G1SL_G1T57 = 57 ) 
          integer G1SL_G1T58               !  
          parameter ( G1SL_G1T58 = 58 ) 
          integer G1SL_G1T59               !  
          parameter ( G1SL_G1T59 = 59 ) 
          integer G1SL_G1T60               !  
          parameter ( G1SL_G1T60 = 60 ) 
          integer G1SL_G1T61               !  
          parameter ( G1SL_G1T61 = 61 ) 
          integer G1SL_G1T62               !  
          parameter ( G1SL_G1T62 = 62 ) 
          integer G1SL_G1T63               !  
          parameter ( G1SL_G1T63 = 63 ) 
          integer G1SL_G1T64               !  
          parameter ( G1SL_G1T64 = 64 ) 

C    Offsets for the G2SL bank follow 

          integer G2SL_G2T1               !  
          parameter ( G2SL_G2T1 = 1 ) 
          integer G2SL_G2T2               !  
          parameter ( G2SL_G2T2 = 2 ) 
          integer G2SL_G2T3               !  
          parameter ( G2SL_G2T3 = 3 ) 
          integer G2SL_G2T4               !  
          parameter ( G2SL_G2T4 = 4 ) 
          integer G2SL_G2T5               !  
          parameter ( G2SL_G2T5 = 5 ) 
          integer G2SL_G2T6               !  
          parameter ( G2SL_G2T6 = 6 ) 
          integer G2SL_G2T7               !  
          parameter ( G2SL_G2T7 = 7 ) 
          integer G2SL_G2T8               !  
          parameter ( G2SL_G2T8 = 8 ) 
          integer G2SL_G2T9               !  
          parameter ( G2SL_G2T9 = 9 ) 
          integer G2SL_G2T10               !  
          parameter ( G2SL_G2T10 = 10 ) 
          integer G2SL_G2T11               !  
          parameter ( G2SL_G2T11 = 11 ) 
          integer G2SL_G2T12               !  
          parameter ( G2SL_G2T12 = 12 ) 
          integer G2SL_G2T13               !  
          parameter ( G2SL_G2T13 = 13 ) 
          integer G2SL_G2T14               !  
          parameter ( G2SL_G2T14 = 14 ) 
          integer G2SL_G2T15               !  
          parameter ( G2SL_G2T15 = 15 ) 
          integer G2SL_G2T16               !  
          parameter ( G2SL_G2T16 = 16 ) 
          integer G2SL_G2T17               !  
          parameter ( G2SL_G2T17 = 17 ) 
          integer G2SL_G2T18               !  
          parameter ( G2SL_G2T18 = 18 ) 
          integer G2SL_G2T19               !  
          parameter ( G2SL_G2T19 = 19 ) 
          integer G2SL_G2T20               !  
          parameter ( G2SL_G2T20 = 20 ) 
          integer G2SL_G2T21               !  
          parameter ( G2SL_G2T21 = 21 ) 
          integer G2SL_G2T22               !  
          parameter ( G2SL_G2T22 = 22 ) 
          integer G2SL_G2T23               !  
          parameter ( G2SL_G2T23 = 23 ) 
          integer G2SL_G2T24               !  
          parameter ( G2SL_G2T24 = 24 ) 
          integer G2SL_G2T25               !  
          parameter ( G2SL_G2T25 = 25 ) 
          integer G2SL_G2T26               !  
          parameter ( G2SL_G2T26 = 26 ) 
          integer G2SL_G2T27               !  
          parameter ( G2SL_G2T27 = 27 ) 
          integer G2SL_G2T28               !  
          parameter ( G2SL_G2T28 = 28 ) 
          integer G2SL_G2T29               !  
          parameter ( G2SL_G2T29 = 29 ) 
          integer G2SL_G2T30               !  
          parameter ( G2SL_G2T30 = 30 ) 
          integer G2SL_G2T31               !  
          parameter ( G2SL_G2T31 = 31 ) 
          integer G2SL_G2T32               !  
          parameter ( G2SL_G2T32 = 32 ) 
          integer G2SL_G2T33               !  
          parameter ( G2SL_G2T33 = 33 ) 
          integer G2SL_G2T34               !  
          parameter ( G2SL_G2T34 = 34 ) 
          integer G2SL_G2T35               !  
          parameter ( G2SL_G2T35 = 35 ) 
          integer G2SL_G2T36               !  
          parameter ( G2SL_G2T36 = 36 ) 
          integer G2SL_G2T37               !  
          parameter ( G2SL_G2T37 = 37 ) 
          integer G2SL_G2T38               !  
          parameter ( G2SL_G2T38 = 38 ) 
          integer G2SL_G2T39               !  
          parameter ( G2SL_G2T39 = 39 ) 
          integer G2SL_G2T40               !  
          parameter ( G2SL_G2T40 = 40 ) 
          integer G2SL_G2T41               !  
          parameter ( G2SL_G2T41 = 41 ) 
          integer G2SL_G2T42               !  
          parameter ( G2SL_G2T42 = 42 ) 
          integer G2SL_G2T43               !  
          parameter ( G2SL_G2T43 = 43 ) 
          integer G2SL_G2T44               !  
          parameter ( G2SL_G2T44 = 44 ) 
          integer G2SL_G2T45               !  
          parameter ( G2SL_G2T45 = 45 ) 
          integer G2SL_G2T46               !  
          parameter ( G2SL_G2T46 = 46 ) 
          integer G2SL_G2T47               !  
          parameter ( G2SL_G2T47 = 47 ) 
          integer G2SL_G2T48               !  
          parameter ( G2SL_G2T48 = 48 ) 
          integer G2SL_G2T49               !  
          parameter ( G2SL_G2T49 = 49 ) 
          integer G2SL_G2T50               !  
          parameter ( G2SL_G2T50 = 50 ) 
          integer G2SL_G2T51               !  
          parameter ( G2SL_G2T51 = 51 ) 
          integer G2SL_G2T52               !  
          parameter ( G2SL_G2T52 = 52 ) 
          integer G2SL_G2T53               !  
          parameter ( G2SL_G2T53 = 53 ) 
          integer G2SL_G2T54               !  
          parameter ( G2SL_G2T54 = 54 ) 
          integer G2SL_G2T55               !  
          parameter ( G2SL_G2T55 = 55 ) 
          integer G2SL_G2T56               !  
          parameter ( G2SL_G2T56 = 56 ) 
          integer G2SL_G2T57               !  
          parameter ( G2SL_G2T57 = 57 ) 
          integer G2SL_G2T58               !  
          parameter ( G2SL_G2T58 = 58 ) 
          integer G2SL_G2T59               !  
          parameter ( G2SL_G2T59 = 59 ) 
          integer G2SL_G2T60               !  
          parameter ( G2SL_G2T60 = 60 ) 
          integer G2SL_G2T61               !  
          parameter ( G2SL_G2T61 = 61 ) 
          integer G2SL_G2T62               !  
          parameter ( G2SL_G2T62 = 62 ) 
          integer G2SL_G2T63               !  
          parameter ( G2SL_G2T63 = 63 ) 
          integer G2SL_G2T64               !  
          parameter ( G2SL_G2T64 = 64 ) 

C    Offsets for the G3SL bank follow 

          integer G3SL_G3T1               !  
          parameter ( G3SL_G3T1 = 1 ) 
          integer G3SL_G3T2               !  
          parameter ( G3SL_G3T2 = 2 ) 
          integer G3SL_G3T3               !  
          parameter ( G3SL_G3T3 = 3 ) 
          integer G3SL_G3T4               !  
          parameter ( G3SL_G3T4 = 4 ) 
          integer G3SL_G3T5               !  
          parameter ( G3SL_G3T5 = 5 ) 
          integer G3SL_G3T6               !  
          parameter ( G3SL_G3T6 = 6 ) 
          integer G3SL_G3T7               !  
          parameter ( G3SL_G3T7 = 7 ) 
          integer G3SL_G3T8               !  
          parameter ( G3SL_G3T8 = 8 ) 
          integer G3SL_G3T9               !  
          parameter ( G3SL_G3T9 = 9 ) 
          integer G3SL_G3T10               !  
          parameter ( G3SL_G3T10 = 10 ) 
          integer G3SL_G3T11               !  
          parameter ( G3SL_G3T11 = 11 ) 
          integer G3SL_G3T12               !  
          parameter ( G3SL_G3T12 = 12 ) 
          integer G3SL_G3T13               !  
          parameter ( G3SL_G3T13 = 13 ) 
          integer G3SL_G3T14               !  
          parameter ( G3SL_G3T14 = 14 ) 
          integer G3SL_G3T15               !  
          parameter ( G3SL_G3T15 = 15 ) 
          integer G3SL_G3T16               !  
          parameter ( G3SL_G3T16 = 16 ) 
          integer G3SL_G3T17               !  
          parameter ( G3SL_G3T17 = 17 ) 
          integer G3SL_G3T18               !  
          parameter ( G3SL_G3T18 = 18 ) 
          integer G3SL_G3T19               !  
          parameter ( G3SL_G3T19 = 19 ) 
          integer G3SL_G3T20               !  
          parameter ( G3SL_G3T20 = 20 ) 
          integer G3SL_G3T21               !  
          parameter ( G3SL_G3T21 = 21 ) 
          integer G3SL_G3T22               !  
          parameter ( G3SL_G3T22 = 22 ) 
          integer G3SL_G3T23               !  
          parameter ( G3SL_G3T23 = 23 ) 
          integer G3SL_G3T24               !  
          parameter ( G3SL_G3T24 = 24 ) 
          integer G3SL_G3T25               !  
          parameter ( G3SL_G3T25 = 25 ) 
          integer G3SL_G3T26               !  
          parameter ( G3SL_G3T26 = 26 ) 
          integer G3SL_G3T27               !  
          parameter ( G3SL_G3T27 = 27 ) 
          integer G3SL_G3T28               !  
          parameter ( G3SL_G3T28 = 28 ) 
          integer G3SL_G3T29               !  
          parameter ( G3SL_G3T29 = 29 ) 
          integer G3SL_G3T30               !  
          parameter ( G3SL_G3T30 = 30 ) 
          integer G3SL_G3T31               !  
          parameter ( G3SL_G3T31 = 31 ) 
          integer G3SL_G3T32               !  
          parameter ( G3SL_G3T32 = 32 ) 
          integer G3SL_G3T33               !  
          parameter ( G3SL_G3T33 = 33 ) 
          integer G3SL_G3T34               !  
          parameter ( G3SL_G3T34 = 34 ) 
          integer G3SL_G3T35               !  
          parameter ( G3SL_G3T35 = 35 ) 
          integer G3SL_G3T36               !  
          parameter ( G3SL_G3T36 = 36 ) 
          integer G3SL_G3T37               !  
          parameter ( G3SL_G3T37 = 37 ) 
          integer G3SL_G3T38               !  
          parameter ( G3SL_G3T38 = 38 ) 
          integer G3SL_G3T39               !  
          parameter ( G3SL_G3T39 = 39 ) 
          integer G3SL_G3T40               !  
          parameter ( G3SL_G3T40 = 40 ) 
          integer G3SL_G3T41               !  
          parameter ( G3SL_G3T41 = 41 ) 
          integer G3SL_G3T42               !  
          parameter ( G3SL_G3T42 = 42 ) 
          integer G3SL_G3T43               !  
          parameter ( G3SL_G3T43 = 43 ) 
          integer G3SL_G3T44               !  
          parameter ( G3SL_G3T44 = 44 ) 
          integer G3SL_G3T45               !  
          parameter ( G3SL_G3T45 = 45 ) 
          integer G3SL_G3T46               !  
          parameter ( G3SL_G3T46 = 46 ) 
          integer G3SL_G3T47               !  
          parameter ( G3SL_G3T47 = 47 ) 
          integer G3SL_G3T48               !  
          parameter ( G3SL_G3T48 = 48 ) 
          integer G3SL_G3T49               !  
          parameter ( G3SL_G3T49 = 49 ) 
          integer G3SL_G3T50               !  
          parameter ( G3SL_G3T50 = 50 ) 
          integer G3SL_G3T51               !  
          parameter ( G3SL_G3T51 = 51 ) 
          integer G3SL_G3T52               !  
          parameter ( G3SL_G3T52 = 52 ) 
          integer G3SL_G3T53               !  
          parameter ( G3SL_G3T53 = 53 ) 
          integer G3SL_G3T54               !  
          parameter ( G3SL_G3T54 = 54 ) 
          integer G3SL_G3T55               !  
          parameter ( G3SL_G3T55 = 55 ) 
          integer G3SL_G3T56               !  
          parameter ( G3SL_G3T56 = 56 ) 
          integer G3SL_G3T57               !  
          parameter ( G3SL_G3T57 = 57 ) 
          integer G3SL_G3T58               !  
          parameter ( G3SL_G3T58 = 58 ) 
          integer G3SL_G3T59               !  
          parameter ( G3SL_G3T59 = 59 ) 
          integer G3SL_G3T60               !  
          parameter ( G3SL_G3T60 = 60 ) 
          integer G3SL_G3T61               !  
          parameter ( G3SL_G3T61 = 61 ) 
          integer G3SL_G3T62               !  
          parameter ( G3SL_G3T62 = 62 ) 
          integer G3SL_G3T63               !  
          parameter ( G3SL_G3T63 = 63 ) 
          integer G3SL_G3T64               !  
          parameter ( G3SL_G3T64 = 64 ) 

C    Offsets for the G4SL bank follow 

          integer G4SL_G4T1               !  
          parameter ( G4SL_G4T1 = 1 ) 
          integer G4SL_G4T2               !  
          parameter ( G4SL_G4T2 = 2 ) 
          integer G4SL_G4T3               !  
          parameter ( G4SL_G4T3 = 3 ) 
          integer G4SL_G4T4               !  
          parameter ( G4SL_G4T4 = 4 ) 
          integer G4SL_G4T5               !  
          parameter ( G4SL_G4T5 = 5 ) 
          integer G4SL_G4T6               !  
          parameter ( G4SL_G4T6 = 6 ) 
          integer G4SL_G4T7               !  
          parameter ( G4SL_G4T7 = 7 ) 
          integer G4SL_G4T8               !  
          parameter ( G4SL_G4T8 = 8 ) 
          integer G4SL_G4T9               !  
          parameter ( G4SL_G4T9 = 9 ) 
          integer G4SL_G4T10               !  
          parameter ( G4SL_G4T10 = 10 ) 
          integer G4SL_G4T11               !  
          parameter ( G4SL_G4T11 = 11 ) 
          integer G4SL_G4T12               !  
          parameter ( G4SL_G4T12 = 12 ) 
          integer G4SL_G4T13               !  
          parameter ( G4SL_G4T13 = 13 ) 
          integer G4SL_G4T14               !  
          parameter ( G4SL_G4T14 = 14 ) 
          integer G4SL_G4T15               !  
          parameter ( G4SL_G4T15 = 15 ) 
          integer G4SL_G4T16               !  
          parameter ( G4SL_G4T16 = 16 ) 
          integer G4SL_G4T17               !  
          parameter ( G4SL_G4T17 = 17 ) 
          integer G4SL_G4T18               !  
          parameter ( G4SL_G4T18 = 18 ) 
          integer G4SL_G4T19               !  
          parameter ( G4SL_G4T19 = 19 ) 
          integer G4SL_G4T20               !  
          parameter ( G4SL_G4T20 = 20 ) 
          integer G4SL_G4T21               !  
          parameter ( G4SL_G4T21 = 21 ) 
          integer G4SL_G4T22               !  
          parameter ( G4SL_G4T22 = 22 ) 
          integer G4SL_G4T23               !  
          parameter ( G4SL_G4T23 = 23 ) 
          integer G4SL_G4T24               !  
          parameter ( G4SL_G4T24 = 24 ) 
          integer G4SL_G4T25               !  
          parameter ( G4SL_G4T25 = 25 ) 
          integer G4SL_G4T26               !  
          parameter ( G4SL_G4T26 = 26 ) 
          integer G4SL_G4T27               !  
          parameter ( G4SL_G4T27 = 27 ) 
          integer G4SL_G4T28               !  
          parameter ( G4SL_G4T28 = 28 ) 
          integer G4SL_G4T29               !  
          parameter ( G4SL_G4T29 = 29 ) 
          integer G4SL_G4T30               !  
          parameter ( G4SL_G4T30 = 30 ) 
          integer G4SL_G4T31               !  
          parameter ( G4SL_G4T31 = 31 ) 
          integer G4SL_G4T32               !  
          parameter ( G4SL_G4T32 = 32 ) 
          integer G4SL_G4T33               !  
          parameter ( G4SL_G4T33 = 33 ) 
          integer G4SL_G4T34               !  
          parameter ( G4SL_G4T34 = 34 ) 
          integer G4SL_G4T35               !  
          parameter ( G4SL_G4T35 = 35 ) 
          integer G4SL_G4T36               !  
          parameter ( G4SL_G4T36 = 36 ) 
          integer G4SL_G4T37               !  
          parameter ( G4SL_G4T37 = 37 ) 
          integer G4SL_G4T38               !  
          parameter ( G4SL_G4T38 = 38 ) 
          integer G4SL_G4T39               !  
          parameter ( G4SL_G4T39 = 39 ) 
          integer G4SL_G4T40               !  
          parameter ( G4SL_G4T40 = 40 ) 
          integer G4SL_G4T41               !  
          parameter ( G4SL_G4T41 = 41 ) 
          integer G4SL_G4T42               !  
          parameter ( G4SL_G4T42 = 42 ) 
          integer G4SL_G4T43               !  
          parameter ( G4SL_G4T43 = 43 ) 
          integer G4SL_G4T44               !  
          parameter ( G4SL_G4T44 = 44 ) 
          integer G4SL_G4T45               !  
          parameter ( G4SL_G4T45 = 45 ) 
          integer G4SL_G4T46               !  
          parameter ( G4SL_G4T46 = 46 ) 
          integer G4SL_G4T47               !  
          parameter ( G4SL_G4T47 = 47 ) 
          integer G4SL_G4T48               !  
          parameter ( G4SL_G4T48 = 48 ) 
          integer G4SL_G4T49               !  
          parameter ( G4SL_G4T49 = 49 ) 
          integer G4SL_G4T50               !  
          parameter ( G4SL_G4T50 = 50 ) 
          integer G4SL_G4T51               !  
          parameter ( G4SL_G4T51 = 51 ) 
          integer G4SL_G4T52               !  
          parameter ( G4SL_G4T52 = 52 ) 
          integer G4SL_G4T53               !  
          parameter ( G4SL_G4T53 = 53 ) 
          integer G4SL_G4T54               !  
          parameter ( G4SL_G4T54 = 54 ) 
          integer G4SL_G4T55               !  
          parameter ( G4SL_G4T55 = 55 ) 
          integer G4SL_G4T56               !  
          parameter ( G4SL_G4T56 = 56 ) 
          integer G4SL_G4T57               !  
          parameter ( G4SL_G4T57 = 57 ) 
          integer G4SL_G4T58               !  
          parameter ( G4SL_G4T58 = 58 ) 
          integer G4SL_G4T59               !  
          parameter ( G4SL_G4T59 = 59 ) 
          integer G4SL_G4T60               !  
          parameter ( G4SL_G4T60 = 60 ) 
          integer G4SL_G4T61               !  
          parameter ( G4SL_G4T61 = 61 ) 
          integer G4SL_G4T62               !  
          parameter ( G4SL_G4T62 = 62 ) 
          integer G4SL_G4T63               !  
          parameter ( G4SL_G4T63 = 63 ) 
          integer G4SL_G4T64               !  
          parameter ( G4SL_G4T64 = 64 ) 

C    Offsets for the GPAR bank follow 

          integer GPAR_FVAL               !  float value
          parameter ( GPAR_FVAL = 1 ) 
          integer GPAR_IVAL               !  int value 
          parameter ( GPAR_IVAL = 2 ) 
          integer GPAR_CHAR1               !  char[20] name;
          parameter ( GPAR_CHAR1 = 3 ) 
          integer GPAR_CHAR2               !  
          parameter ( GPAR_CHAR2 = 4 ) 
          integer GPAR_CHAR3               !  
          parameter ( GPAR_CHAR3 = 5 ) 
          integer GPAR_CHAR4               !  
          parameter ( GPAR_CHAR4 = 6 ) 
          integer GPAR_CHAR5               !  
          parameter ( GPAR_CHAR5 = 7 ) 

C    Offsets for the HBER bank follow 

          integer HBER_Q_OVER_P               !  q/p
          parameter ( HBER_Q_OVER_P = 1 ) 
          integer HBER_LAMBDA               !  dip angle (pi/2 - theta)
          parameter ( HBER_LAMBDA = 2 ) 
          integer HBER_PHI               !  phi
          parameter ( HBER_PHI = 3 ) 
          integer HBER_D0               !  min.distance from (x=0,y=0,z=?)   [cm]
          parameter ( HBER_D0 = 4 ) 
          integer HBER_Z0               !  z position of starting point  [cm]
          parameter ( HBER_Z0 = 5 ) 
          integer HBER_C11               !  element C{1,1}
          parameter ( HBER_C11 = 6 ) 
          integer HBER_C12               !  element C{1,2}
          parameter ( HBER_C12 = 7 ) 
          integer HBER_C13               !  element C{1,3}
          parameter ( HBER_C13 = 8 ) 
          integer HBER_C14               !  element C{1,4}
          parameter ( HBER_C14 = 9 ) 
          integer HBER_C15               !  element C{1,5}
          parameter ( HBER_C15 = 10 ) 
          integer HBER_C22               !  element C{2,2}
          parameter ( HBER_C22 = 11 ) 
          integer HBER_C23               !  element C{2,3}
          parameter ( HBER_C23 = 12 ) 
          integer HBER_C24               !  element C{2,4}
          parameter ( HBER_C24 = 13 ) 
          integer HBER_C25               !  element C{2,5}
          parameter ( HBER_C25 = 14 ) 
          integer HBER_C33               !  element C{3,3}
          parameter ( HBER_C33 = 15 ) 
          integer HBER_C34               !  element C{3,4}
          parameter ( HBER_C34 = 16 ) 
          integer HBER_C35               !  element C{3,5}
          parameter ( HBER_C35 = 17 ) 
          integer HBER_C44               !  element C{4,4}
          parameter ( HBER_C44 = 18 ) 
          integer HBER_C45               !  element C{4,5}
          parameter ( HBER_C45 = 19 ) 
          integer HBER_C55               !  element C{5,5}
          parameter ( HBER_C55 = 20 ) 
          integer HBER_CHI2               !  Chisquare for this Track
          parameter ( HBER_CHI2 = 21 ) 
          integer HBER_LAYINFO1               !  layerhit info
          parameter ( HBER_LAYINFO1 = 22 ) 
          integer HBER_LAYINFO2               !  layerhit info&sector&track#in sector
          parameter ( HBER_LAYINFO2 = 23 ) 

C    Offsets for the HBID bank follow 

          integer HBID_TRACK               !  Track Candidate (ptr to HBTR)
          parameter ( HBID_TRACK = 1 ) 
          integer HBID_SEC               !  Sector track is in
          parameter ( HBID_SEC = 2 ) 
          integer HBID_BETA               !  Beta of the track in units of c
          parameter ( HBID_BETA = 3 ) 
          integer HBID_VTIME               !  vertex time of track
          parameter ( HBID_VTIME = 4 ) 
          integer HBID_SC_STAT               !  status of hit matching to SC: see sc.h
          parameter ( HBID_SC_STAT = 5 ) 
          integer HBID_SC_ID               !  Pointer to SCRC bank
          parameter ( HBID_SC_ID = 6 ) 
          integer HBID_SC_TIME               !  SC calibrated time for this track (ns)
          parameter ( HBID_SC_TIME = 7 ) 
          integer HBID_SC_QUAL               !  quality of match for SC
          parameter ( HBID_SC_QUAL = 8 ) 
          integer HBID_SC_VTIME               !  time at vertex for SC(ns)
          parameter ( HBID_SC_VTIME = 9 ) 
          integer HBID_SC_BETA               !  Beta calculated from TOF from SC
          parameter ( HBID_SC_BETA = 10 ) 
          integer HBID_CC_STAT               !  status of hit matching to CC: see sc.h
          parameter ( HBID_CC_STAT = 11 ) 
          integer HBID_CC_ID               !  pointer to CC01 bank
          parameter ( HBID_CC_ID = 12 ) 
          integer HBID_CC_TIME               !  CC calibrated time for this track (ns)
          parameter ( HBID_CC_TIME = 13 ) 
          integer HBID_CC_QUAL               !  quality of match for CC
          parameter ( HBID_CC_QUAL = 14 ) 
          integer HBID_CC_VTIME               !  time at vertex for CC(ns)
          parameter ( HBID_CC_VTIME = 15 ) 
          integer HBID_CC_BETA               !  Beta as calculated by the EC
          parameter ( HBID_CC_BETA = 16 ) 
          integer HBID_EC_STAT               !  status of hit matching to ec: see sc.h
          parameter ( HBID_EC_STAT = 17 ) 
          integer HBID_EC_ID               !  Pointer to ECHB bank
          parameter ( HBID_EC_ID = 18 ) 
          integer HBID_EC_TIME               !  EC calibrated time for this track (ns)
          parameter ( HBID_EC_TIME = 19 ) 
          integer HBID_EC_QUAL               !  EC quality factor
          parameter ( HBID_EC_QUAL = 20 ) 
          integer HBID_EC_VTIME               !  time at vertex for EC(ns)
          parameter ( HBID_EC_VTIME = 21 ) 
          integer HBID_EC_BETA               !  Beta as calculated by the EC
          parameter ( HBID_EC_BETA = 22 ) 
          integer HBID_ST_STAT               !  status of hit matching to ST
          parameter ( HBID_ST_STAT = 23 ) 
          integer HBID_ST_ID               !  Pointer to STR bank
          parameter ( HBID_ST_ID = 24 ) 
          integer HBID_ST_TIME               !  ST calibrated time for this track (ns)
          parameter ( HBID_ST_TIME = 25 ) 
          integer HBID_ST_QUAL               !  ST quality factor
          parameter ( HBID_ST_QUAL = 26 ) 
          integer HBID_ST_VTIME               !  time at vertex for ST(ns)
          parameter ( HBID_ST_VTIME = 27 ) 
          integer HBID_ST_BETA               !  Beta as calculated by the ST
          parameter ( HBID_ST_BETA = 28 ) 
          integer HBID_LAC_STAT               !  status of hit matching to LAC
          parameter ( HBID_LAC_STAT = 29 ) 
          integer HBID_LAC_ID               !  Pointer to EC1R bank
          parameter ( HBID_LAC_ID = 30 ) 
          integer HBID_LAC_TIME               !  LAC calibrated time for this track (ns)
          parameter ( HBID_LAC_TIME = 31 ) 
          integer HBID_LAC_QUAL               !  LAC quality factor
          parameter ( HBID_LAC_QUAL = 32 ) 
          integer HBID_LAC_VTIME               !  time at vertex for LAC(ns)
          parameter ( HBID_LAC_VTIME = 33 ) 
          integer HBID_LAC_BETA               !  Beta as calculated by the LAC
          parameter ( HBID_LAC_BETA = 34 ) 

C    Offsets for the HBLA bank follow 

          integer HBLA_TRK_PLN               !  (track_number) *100 + Trk_plane_number
          parameter ( HBLA_TRK_PLN = 1 ) 
          integer HBLA_X               !  z coord [cm]  for track in this plane
          parameter ( HBLA_X = 2 ) 
          integer HBLA_Y               !  y coord [cm]  for track in this plane
          parameter ( HBLA_Y = 3 ) 
          integer HBLA_Z               !  z coord [cm]  for track in this plane
          parameter ( HBLA_Z = 4 ) 
          integer HBLA_BX               !  B-field in x [kG] at coord.{x,y,z}
          parameter ( HBLA_BX = 5 ) 
          integer HBLA_BY               !  B-field in y [kG] at coord.{x,y,z}
          parameter ( HBLA_BY = 6 ) 
          integer HBLA_BZ               !  B-field in z [kG] at coord.{x,y,z}
          parameter ( HBLA_BZ = 7 ) 
          integer HBLA_TLEN               !  track length [cm] from origin to this plane
          parameter ( HBLA_TLEN = 8 ) 
          integer HBLA_DC1               !  Pointer to DC1 bank
          parameter ( HBLA_DC1 = 9 ) 
          integer HBLA_STAT               !  Status of the hit
          parameter ( HBLA_STAT = 10 ) 
          integer HBLA_WIRE               !  Wire number 
          parameter ( HBLA_WIRE = 11 ) 
          integer HBLA_DTIME               !  drift time [ns]  (not corrected for start ime)
          parameter ( HBLA_DTIME = 12 ) 
          integer HBLA_ALPHA               !  track angle (relative to R of SL) [deg]
          parameter ( HBLA_ALPHA = 13 ) 
          integer HBLA_WLEN               !  Wire length (hit pos. to preamp)  [cm]
          parameter ( HBLA_WLEN = 14 ) 
          integer HBLA_SGDOCA               !  sigma DOCA
          parameter ( HBLA_SGDOCA = 15 ) 
          integer HBLA_FITDOCA               !  Fitted DOCA
          parameter ( HBLA_FITDOCA = 16 ) 

C    Offsets for the HBTB bank follow 

          integer HBTB_TRK               !  Track Candidate 
          parameter ( HBTB_TRK = 1 ) 
          integer HBTB_SLY               !  Superlayer
          parameter ( HBTB_SLY = 2 ) 
          integer HBTB_ICL               !  Cluster# that matched to the track
          parameter ( HBTB_ICL = 3 ) 
          integer HBTB_ISG               !  Segment# in this cluster
          parameter ( HBTB_ISG = 4 ) 
          integer HBTB_ID1               !  Ptr. to DC1 entry  for 1.Layer in SL
          parameter ( HBTB_ID1 = 5 ) 
          integer HBTB_ID2               !  Ptr. to DC1 entry  for 2.Layer in SL
          parameter ( HBTB_ID2 = 6 ) 
          integer HBTB_ID3               !  Ptr. to DC1 entry  for 3.Layer in SL
          parameter ( HBTB_ID3 = 7 ) 
          integer HBTB_ID4               !  Ptr. to DC1 entry  for 4.Layer in SL
          parameter ( HBTB_ID4 = 8 ) 
          integer HBTB_ID5               !  Ptr. to DC1 entry  for 5.Layer in SL
          parameter ( HBTB_ID5 = 9 ) 
          integer HBTB_ID6               !  Ptr. to DC1 entry  for 6.Layer in SL
          parameter ( HBTB_ID6 = 10 ) 

C    Offsets for the HBTR bank follow 

          integer HBTR_X               !  x
          parameter ( HBTR_X = 1 ) 
          integer HBTR_Y               !  y 'Vertex' position {x,y,z}
          parameter ( HBTR_Y = 2 ) 
          integer HBTR_Z               !  z
          parameter ( HBTR_Z = 3 ) 
          integer HBTR_PX               !  Px
          parameter ( HBTR_PX = 4 ) 
          integer HBTR_PY               !  Py  momentum at 'vertex' {Px,Py,Pz}
          parameter ( HBTR_PY = 5 ) 
          integer HBTR_PZ               !  Pz
          parameter ( HBTR_PZ = 6 ) 
          integer HBTR_Q               !  charge   (straight tracks: set q=0.)
          parameter ( HBTR_Q = 7 ) 
          integer HBTR_CHI2               !  Chisquare for this Track
          parameter ( HBTR_CHI2 = 8 ) 
          integer HBTR_ITR_SEC               !  Trackno_in_Sector + Sector*100
          parameter ( HBTR_ITR_SEC = 9 ) 

C    Offsets for the HCAL bank follow 

          integer HCAL_VERSION               !  Version number
          parameter ( HCAL_VERSION = 1 ) 
          integer HCAL_RUN_LOW               !  LOW RUN number		 
          parameter ( HCAL_RUN_LOW = 2 ) 
          integer HCAL_RUN_HIGH               !  HIGH RUN number
          parameter ( HCAL_RUN_HIGH = 3 ) 
          integer HCAL_CATIME               !  CAlibration TIME (unix time = 0 1/1/1970)		
          parameter ( HCAL_CATIME = 4 ) 
          integer HCAL_ROCCA               !  32 bit ReadOut Controller CAlibration status
          parameter ( HCAL_ROCCA = 5 ) 

C    Offsets for the HDPL bank follow 

          integer HDPL_TRK_PLN               !  (track_number) *100 + plane_number
          parameter ( HDPL_TRK_PLN = 1 ) 
          integer HDPL_X               !  vector3_t pos;
          parameter ( HDPL_X = 2 ) 
          integer HDPL_Y               !   (x, y, z coord. for track in on the plane)
          parameter ( HDPL_Y = 3 ) 
          integer HDPL_Z               !  
          parameter ( HDPL_Z = 4 ) 
          integer HDPL_CX               !  vector3_t dir;
          parameter ( HDPL_CX = 5 ) 
          integer HDPL_CY               !   direction cosines (x,y,z) for track at coord.{x,y,z}
          parameter ( HDPL_CY = 6 ) 
          integer HDPL_CZ               !  
          parameter ( HDPL_CZ = 7 ) 
          integer HDPL_TLEN               !  track length [cm] from origin to this plane
          parameter ( HDPL_TLEN = 8 ) 

C    Offsets for the HEAD bank follow 

          integer HEAD_VERSION               !  Version Number
          parameter ( HEAD_VERSION = 1 ) 
          integer HEAD_NRUN               !  Run Number (monotonically increasing)
          parameter ( HEAD_NRUN = 2 ) 
          integer HEAD_NEVENT               !  Event Number (starting with 1 at run begin,
          parameter ( HEAD_NEVENT = 3 ) 
          integer HEAD_TIME               !  Event Time (UNIX time = seconds as of January 1,1970)
          parameter ( HEAD_TIME = 4 ) 
          integer HEAD_TYPE               !  Event Type (Defined by on-line system or MC run:
          parameter ( HEAD_TYPE = 5 ) 
          integer HEAD_ROC               !             = 0        - sync status is OK
          parameter ( HEAD_ROC = 6 ) 
          integer HEAD_EVTCLASS               !  Event Classification from DAQ:
          parameter ( HEAD_EVTCLASS = 7 ) 
          integer HEAD_TRIGBITS               !  Level 1 Trigger Latch Word (16 bits)
          parameter ( HEAD_TRIGBITS = 8 ) 

C    Offsets for the HEVT bank follow 

          integer HEVT_ESTATUS               !  Event Statuse after reconstruction
          parameter ( HEVT_ESTATUS = 1 ) 
          integer HEVT_NRUN               !  Run Number (monotonically increasing)
          parameter ( HEVT_NRUN = 2 ) 
          integer HEVT_NEVENT               !  Event Number in the run NRUN 
          parameter ( HEVT_NEVENT = 3 ) 
          integer HEVT_TYPE               !  Event Type (Data or MC)
          parameter ( HEVT_TYPE = 4 ) 
          integer HEVT_NPGP               !  Number of final reconstructed particles*100 +
          parameter ( HEVT_NPGP = 5 ) 
          integer HEVT_TRGPRS               !  Trigger type*10000 + 
          parameter ( HEVT_TRGPRS = 6 ) 
          integer HEVT_FC               !  Faraday Cup (K)
          parameter ( HEVT_FC = 7 ) 
          integer HEVT_FCG               !  Faraday Cup Gated (K)
          parameter ( HEVT_FCG = 8 ) 
          integer HEVT_TG               !  Clock Gated
          parameter ( HEVT_TG = 9 ) 
          integer HEVT_STT               !  Event Start Time 
          parameter ( HEVT_STT = 10 ) 
          integer HEVT_RF1               !  RF Time 
          parameter ( HEVT_RF1 = 11 ) 
          integer HEVT_RF2               !  RF Time
          parameter ( HEVT_RF2 = 12 ) 
          integer HEVT_CON1               !  Control Rates 
          parameter ( HEVT_CON1 = 13 ) 
          integer HEVT_CON2               !  Control Rates  
          parameter ( HEVT_CON2 = 14 ) 
          integer HEVT_CON3               !  Control Rates 
          parameter ( HEVT_CON3 = 15 ) 
          integer HEVT_PTIME               !  Event Processing Time (UNIX time = seconds)
          parameter ( HEVT_PTIME = 16 ) 

C    Offsets for the HLS bank follow 

          integer HLS_SYNC               !  sync signal 
          parameter ( HLS_SYNC = 1 ) 
          integer HLS_C22AI               !  current amplitude    
          parameter ( HLS_C22AI = 2 ) 
          integer HLS_C22AX               !  X position amplitude  BPM 2C22
          parameter ( HLS_C22AX = 3 ) 
          integer HLS_C22AY               !  Y position amplitude 
          parameter ( HLS_C22AY = 4 ) 
          integer HLS_C24AI               !  current amplitude     
          parameter ( HLS_C24AI = 5 ) 
          integer HLS_C24AX               !  X position amplitude   BPM 2C24
          parameter ( HLS_C24AX = 6 ) 
          integer HLS_C24AY               !  Y position amplitude   
          parameter ( HLS_C24AY = 7 ) 
          integer HLS_FCI               !  Faradday cup current amplitude
          parameter ( HLS_FCI = 8 ) 
          integer HLS_PMT1               !  Lumi monitor on beam pipe
          parameter ( HLS_PMT1 = 9 ) 
          integer HLS_PMT2               !     
          parameter ( HLS_PMT2 = 10 ) 
          integer HLS_PMT3               !     
          parameter ( HLS_PMT3 = 11 ) 
          integer HLS_PMT4               !      
          parameter ( HLS_PMT4 = 12 ) 

C    Offsets for the KFIT bank follow 

          integer KFIT_MOMENTA_F               !  fitted momentum parameter (GeV)   
          parameter ( KFIT_MOMENTA_F = 1 ) 
          integer KFIT_LAMBDA_F               !  fitted dip angle (radians) 
          parameter ( KFIT_LAMBDA_F = 2 ) 
          integer KFIT_PHI_F               !  fitted phi angle (radians)
          parameter ( KFIT_PHI_F = 3 ) 
          integer KFIT_D0_F               !  fitted d0 (cm)
          parameter ( KFIT_D0_F = 4 ) 
          integer KFIT_Z0_F               !  fitted z0 (cm)
          parameter ( KFIT_Z0_F = 5 ) 
          integer KFIT_SIGMA11               !    error element of known
          parameter ( KFIT_SIGMA11 = 6 ) 
          integer KFIT_SIGMA12               !    error element of known
          parameter ( KFIT_SIGMA12 = 7 ) 
          integer KFIT_SIGMA13               !    error element of known
          parameter ( KFIT_SIGMA13 = 8 ) 
          integer KFIT_SIGMA14               !    error element of known
          parameter ( KFIT_SIGMA14 = 9 ) 
          integer KFIT_SIGMA15               !    error element of known
          parameter ( KFIT_SIGMA15 = 10 ) 
          integer KFIT_SIGMA22               !    error element of known   
          parameter ( KFIT_SIGMA22 = 11 ) 
          integer KFIT_SIGMA23               !    error element of known
          parameter ( KFIT_SIGMA23 = 12 ) 
          integer KFIT_SIGMA24               !    error element of known
          parameter ( KFIT_SIGMA24 = 13 ) 
          integer KFIT_SIGMA25               !    error element of known
          parameter ( KFIT_SIGMA25 = 14 ) 
          integer KFIT_SIGMA33               !    error element of known 
          parameter ( KFIT_SIGMA33 = 15 ) 
          integer KFIT_SIGMA34               !    error element of known 
          parameter ( KFIT_SIGMA34 = 16 ) 
          integer KFIT_SIGMA35               !    error element of known 
          parameter ( KFIT_SIGMA35 = 17 ) 
          integer KFIT_SIGMA44               !    error element of known 
          parameter ( KFIT_SIGMA44 = 18 ) 
          integer KFIT_SIGMA45               !    error element of known 
          parameter ( KFIT_SIGMA45 = 19 ) 
          integer KFIT_SIGMA55               !    error element of known 
          parameter ( KFIT_SIGMA55 = 20 ) 
          integer KFIT_COV11               !    error element bewteen known and unknown
          parameter ( KFIT_COV11 = 21 ) 
          integer KFIT_COV12               !    error element bewteen known and unknown
          parameter ( KFIT_COV12 = 22 ) 
          integer KFIT_COV13               !    error element bewteen known and unknown
          parameter ( KFIT_COV13 = 23 ) 
          integer KFIT_COV14               !    error element bewteen known and unknown
          parameter ( KFIT_COV14 = 24 ) 
          integer KFIT_COV15               !    error element bewteen known and unknown
          parameter ( KFIT_COV15 = 25 ) 
          integer KFIT_COV21               !    error element bewteen known and unknown
          parameter ( KFIT_COV21 = 26 ) 
          integer KFIT_COV22               !    error element bewteen known and unknown
          parameter ( KFIT_COV22 = 27 ) 
          integer KFIT_COV23               !    error element bewteen known and unknown
          parameter ( KFIT_COV23 = 28 ) 
          integer KFIT_COV24               !    error element bewteen known and unknown
          parameter ( KFIT_COV24 = 29 ) 
          integer KFIT_COV25               !    error element bewteen known and unknown
          parameter ( KFIT_COV25 = 30 ) 
          integer KFIT_COV31               !    error element bewteen known and unknown
          parameter ( KFIT_COV31 = 31 ) 
          integer KFIT_COV32               !    error element bewteen known and unknown
          parameter ( KFIT_COV32 = 32 ) 
          integer KFIT_COV33               !    error element bewteen known and unknown
          parameter ( KFIT_COV33 = 33 ) 
          integer KFIT_COV34               !    error element bewteen known and unknown
          parameter ( KFIT_COV34 = 34 ) 
          integer KFIT_COV35               !    error element bewteen known and unknown
          parameter ( KFIT_COV35 = 35 ) 
          integer KFIT_COV41               !    error element bewteen known and unknown
          parameter ( KFIT_COV41 = 36 ) 
          integer KFIT_COV42               !    error element bewteen known and unknown
          parameter ( KFIT_COV42 = 37 ) 
          integer KFIT_COV43               !    error element bewteen known and unknown
          parameter ( KFIT_COV43 = 38 ) 
          integer KFIT_COV44               !    error element bewteen known and unknown
          parameter ( KFIT_COV44 = 39 ) 
          integer KFIT_COV45               !    error element bewteen known and unknown
          parameter ( KFIT_COV45 = 40 ) 
          integer KFIT_COV51               !    error element bewteen known and unknown
          parameter ( KFIT_COV51 = 41 ) 
          integer KFIT_COV52               !    error element bewteen known and unknown
          parameter ( KFIT_COV52 = 42 ) 
          integer KFIT_COV53               !    error element bewteen known and unknown
          parameter ( KFIT_COV53 = 43 ) 
          integer KFIT_COV54               !    error element bewteen known and unknown
          parameter ( KFIT_COV54 = 44 ) 
          integer KFIT_COV55               !    error element bewteen known and unknown
          parameter ( KFIT_COV55 = 45 ) 
          integer KFIT_CHI2PIECE               !  track contribution to chi2
          parameter ( KFIT_CHI2PIECE = 46 ) 

C    Offsets for the L1PG bank follow 

          integer L1PG_L1PROG               !  Level 1 program
          parameter ( L1PG_L1PROG = 1 ) 

C    Offsets for the L2H bank follow 

          integer L2H_ID               !  Superlayer + 256*(hit# in TDC)
          parameter ( L2H_ID = 1 ) 
          integer L2H_TDC               !  tdc information (channels)
          parameter ( L2H_TDC = 2 ) 

C    Offsets for the L2S bank follow 

          integer L2S_ID               !  ID+256*hit#
          parameter ( L2S_ID = 1 ) 
          integer L2S_TDC               !  TDC value
          parameter ( L2S_TDC = 2 ) 

C    Offsets for the LASR bank follow 

          integer LASR_ID               !  identifier  0-1099  TOF 1100-10000 EC laser  
          parameter ( LASR_ID = 1 ) 
          integer LASR_STAT1               !  status words different for each id
          parameter ( LASR_STAT1 = 2 ) 
          integer LASR_STAT2               !   "
          parameter ( LASR_STAT2 = 3 ) 
          integer LASR_STAT3               !   "
          parameter ( LASR_STAT3 = 4 ) 

C    Offsets for the LCDI bank follow 

          integer LCDI_CRATE               !  CAMAC crate number
          parameter ( LCDI_CRATE = 1 ) 
          integer LCDI_SLOT               !  slot
          parameter ( LCDI_SLOT = 1 ) 
          integer LCDI_MASK               !  mask
          parameter ( LCDI_MASK = 1 ) 
          integer LCDI_THRESHOLD               !  actual threshold value (mV)
          parameter ( LCDI_THRESHOLD = 1 ) 

C    Offsets for the LCPB bank follow 

          integer LCPB_SCHT               !  100*sector+Hit_ID in EC1R
          parameter ( LCPB_SCHT = 1 ) 
          integer LCPB_ETOT               !  Reconstructed total energy
          parameter ( LCPB_ETOT = 2 ) 
          integer LCPB_TIME               !  Flight time relative to the evnt start time
          parameter ( LCPB_TIME = 3 ) 
          integer LCPB_PATH               !  Path lenght from target
          parameter ( LCPB_PATH = 4 ) 
          integer LCPB_X               !  x coordinate of the hit
          parameter ( LCPB_X = 5 ) 
          integer LCPB_Y               !  y coordinate of the hit
          parameter ( LCPB_Y = 6 ) 
          integer LCPB_Z               !  z coordinate of the hit
          parameter ( LCPB_Z = 7 ) 
          integer LCPB_CHI2LC               !  Quality measure of geometrical matching
          parameter ( LCPB_CHI2LC = 8 ) 
          integer LCPB_STATUS               !  Status word
          parameter ( LCPB_STATUS = 9 ) 
          integer LCPB_EIN               !  Reconstructed energy in the inner part
          parameter ( LCPB_EIN = 10 ) 

C    Offsets for the MCEV bank follow 

          integer MCEV_I1               !  first geant random number seed for event
          parameter ( MCEV_I1 = 1 ) 
          integer MCEV_I2               !  second seed
          parameter ( MCEV_I2 = 2 ) 

C    Offsets for the MCHD bank follow 

          integer MCHD_NRUN               !  Run Number
          parameter ( MCHD_NRUN = 1 ) 
          integer MCHD_NEVENT               !  Event Number
          parameter ( MCHD_NEVENT = 2 ) 
          integer MCHD_TIME               !  Event Time (UNIX time)
          parameter ( MCHD_TIME = 3 ) 
          integer MCHD_TYPE               !  Event Type (MUST be negative)
          parameter ( MCHD_TYPE = 4 ) 
          integer MCHD_REACTYPE               !  optional: user defined reaction type
          parameter ( MCHD_REACTYPE = 5 ) 
          integer MCHD_WEIGHT               !  event weight
          parameter ( MCHD_WEIGHT = 6 ) 
          integer MCHD_W               !  center_of_mass energy (inv.mass of hadronic states)
          parameter ( MCHD_W = 7 ) 
          integer MCHD_Q2               !  Q2 (photon virtuallity)
          parameter ( MCHD_Q2 = 8 ) 
          integer MCHD_E_PHOT               !  energy of (real/virtual) photon
          parameter ( MCHD_E_PHOT = 9 ) 
          integer MCHD_PX_PHOT               !  p_x: momentum (in x) of (real/virtual) photon
          parameter ( MCHD_PX_PHOT = 10 ) 
          integer MCHD_PY_PHOT               !  p_y: momentum (in y) of (real/virtual) photon
          parameter ( MCHD_PY_PHOT = 11 ) 
          integer MCHD_PZ_PHOT               !  p_z: momentum (in z) of (real/virtual) photon
          parameter ( MCHD_PZ_PHOT = 12 ) 
          integer MCHD_E_TARG               !  energy of target particle
          parameter ( MCHD_E_TARG = 13 ) 
          integer MCHD_PX_TARG               !  p_x: momentum (in x) of target part.
          parameter ( MCHD_PX_TARG = 14 ) 
          integer MCHD_PY_TARG               !  p_y: momentum (in y) of target part.
          parameter ( MCHD_PY_TARG = 15 ) 
          integer MCHD_PZ_TARG               !  p_z: momentum (in z) of target part.
          parameter ( MCHD_PZ_TARG = 16 ) 

C    Offsets for the MCTK bank follow 

          integer MCTK_CX               !  x dir cosine at track origin
          parameter ( MCTK_CX = 1 ) 
          integer MCTK_CY               !  y dir cosine
          parameter ( MCTK_CY = 2 ) 
          integer MCTK_CZ               !  z dir cosine
          parameter ( MCTK_CZ = 3 ) 
          integer MCTK_PMOM               !  momentum
          parameter ( MCTK_PMOM = 4 ) 
          integer MCTK_MASS               !  mass
          parameter ( MCTK_MASS = 5 ) 
          integer MCTK_CHARGE               !  charge
          parameter ( MCTK_CHARGE = 6 ) 
          integer MCTK_ID               !  track Particle Data Group id
          parameter ( MCTK_ID = 7 ) 
          integer MCTK_FLAG               !  track flag
          parameter ( MCTK_FLAG = 8 ) 
          integer MCTK_BEG_VTX               !  beginning vertex number 
          parameter ( MCTK_BEG_VTX = 9 ) 
          integer MCTK_END_VTX               !  ending vertex number
          parameter ( MCTK_END_VTX = 10 ) 
          integer MCTK_PARENT               !  parent track
          parameter ( MCTK_PARENT = 11 ) 

C    Offsets for the MCVX bank follow 

          integer MCVX_X               !  x of vertex
          parameter ( MCVX_X = 1 ) 
          integer MCVX_Y               !  y
          parameter ( MCVX_Y = 2 ) 
          integer MCVX_Z               !  z
          parameter ( MCVX_Z = 3 ) 
          integer MCVX_TOF               !  secs of flight
          parameter ( MCVX_TOF = 4 ) 
          integer MCVX_FLAG               !  vertex flag
          parameter ( MCVX_FLAG = 5 ) 

C    Offsets for the MTRK bank follow 

          integer MTRK_SECT               !  Sector Number for track
          parameter ( MTRK_SECT = 1 ) 
          integer MTRK_TRKL1               !  track segment and track cluster for Superlayer 1 (lnk_t)
          parameter ( MTRK_TRKL1 = 2 ) 
          integer MTRK_TRKL2               !  track segment and track cluster for Superlayer 2
          parameter ( MTRK_TRKL2 = 3 ) 
          integer MTRK_TRKL3               !  track segment and track cluster for Superlayer 3
          parameter ( MTRK_TRKL3 = 4 ) 
          integer MTRK_TRKL4               !  track segment and track cluster for Superlayer 4
          parameter ( MTRK_TRKL4 = 5 ) 
          integer MTRK_TRKL5               !  track segment and track cluster for Superlayer 5
          parameter ( MTRK_TRKL5 = 6 ) 
          integer MTRK_TRKL6               !  track segment and track cluster for Superlayer 6
          parameter ( MTRK_TRKL6 = 7 ) 

C    Offsets for the MVRT bank follow 

          integer MVRT_V_ID               !  info about track ids
          parameter ( MVRT_V_ID = 1 ) 
          integer MVRT_NTRK               !  number of tracks used to make vertex
          parameter ( MVRT_NTRK = 2 ) 
          integer MVRT_X               !  x vector3_t vert{x,y,z}
          parameter ( MVRT_X = 3 ) 
          integer MVRT_Y               !  y 
          parameter ( MVRT_Y = 4 ) 
          integer MVRT_Z               !  z
          parameter ( MVRT_Z = 5 ) 
          integer MVRT_CHI2               !  chi2
          parameter ( MVRT_CHI2 = 6 ) 
          integer MVRT_CXX               !  Covariance matrix array element
          parameter ( MVRT_CXX = 7 ) 
          integer MVRT_CXY               !  Covariance matrix array element
          parameter ( MVRT_CXY = 8 ) 
          integer MVRT_CXZ               !  Covariance matrix array element
          parameter ( MVRT_CXZ = 9 ) 
          integer MVRT_CYY               !  Covariance matrix array element
          parameter ( MVRT_CYY = 10 ) 
          integer MVRT_CYZ               !  Covariance matrix array element
          parameter ( MVRT_CYZ = 11 ) 
          integer MVRT_CZZ               !  Covariance matrix array element
          parameter ( MVRT_CZZ = 12 ) 
          integer MVRT_STAT               !  status integer, not used yet
          parameter ( MVRT_STAT = 13 ) 

C    Offsets for the PART bank follow 

          integer PART_PID               !  particle id (GEANT)
          parameter ( PART_PID = 1 ) 
          integer PART_X               !  vector3_t vert; Vertex position {x,y,z} 
          parameter ( PART_X = 2 ) 
          integer PART_Y               !  y
          parameter ( PART_Y = 3 ) 
          integer PART_Z               !  z
          parameter ( PART_Z = 4 ) 
          integer PART_E               !  vector4_t p; Energy
          parameter ( PART_E = 5 ) 
          integer PART_PX               !  momentum {x,y,z}
          parameter ( PART_PX = 6 ) 
          integer PART_PY               !   py
          parameter ( PART_PY = 7 ) 
          integer PART_PZ               !  pz
          parameter ( PART_PZ = 8 ) 
          integer PART_Q               !  charge
          parameter ( PART_Q = 9 ) 
          integer PART_TRKID               !  index to TBID bank, counting from 1
          parameter ( PART_TRKID = 10 ) 
          integer PART_QPID               !  quality factor for the pid
          parameter ( PART_QPID = 11 ) 
          integer PART_QTRK               !  quality factor for the trk
          parameter ( PART_QTRK = 12 ) 
          integer PART_FLAGS               !  set of flags defining track (ie, BEAM)
          parameter ( PART_FLAGS = 13 ) 

C    Offsets for the PCO bank follow 

          integer PCO_TIME               !   TDC time 
          parameter ( PCO_TIME = 1 ) 
          integer PCO_ELT               !  energy deposit PC = left top
          parameter ( PCO_ELT = 2 ) 
          integer PCO_ERB               !  energy deposit PC = right bottom
          parameter ( PCO_ERB = 3 ) 
          integer PCO_ELB               !  energy deposit PC = left bottom 
          parameter ( PCO_ELB = 4 ) 
          integer PCO_ERT               !  energy deposit PC = right top
          parameter ( PCO_ERT = 5 ) 
          integer PCO_EMAIN               !  energy deposit PC = MAIN
          parameter ( PCO_EMAIN = 6 ) 
          integer PCO_EVETO               !  energy deposit PC = veto
          parameter ( PCO_EVETO = 7 ) 
          integer PCO_TID               !  T id of the corresponding T
          parameter ( PCO_TID = 8 ) 

C    Offsets for the PRTM bank follow 

          integer PRTM_TIME               !  time of pretrig calibration
          parameter ( PRTM_TIME = 1 ) 

C    Offsets for the PSO bank follow 

          integer PSO_ID               !  Counter Id ()
          parameter ( PSO_ID = 1 ) 
          integer PSO_TIME               !   TDC time 
          parameter ( PSO_TIME = 2 ) 
          integer PSO_ENER               !  energy deposit
          parameter ( PSO_ENER = 3 ) 
          integer PSO_TID               !  T id of the corresponding T
          parameter ( PSO_TID = 4 ) 

C    Offsets for the PTDB bank follow 

          integer PTDB_PBEAM               !  beam polarization (% x 100)
          parameter ( PTDB_PBEAM = 1 ) 
          integer PTDB_PB_DATE               !  date of measurement
          parameter ( PTDB_PB_DATE = 2 ) 
          integer PTDB_TTYPE               !  Target Type 
          parameter ( PTDB_TTYPE = 3 ) 
          integer PTDB_B_TARG               !  Target holding field (Tesla x100)
          parameter ( PTDB_B_TARG = 4 ) 
          integer PTDB_PTARG               !  Target polarization
          parameter ( PTDB_PTARG = 5 ) 
          integer PTDB_PT_TIME               !  Time measured (according to PC)
          parameter ( PTDB_PT_TIME = 6 ) 
          integer PTDB_HE_LEVEL               !  Liquid Helium Level(% x 100)
          parameter ( PTDB_HE_LEVEL = 7 ) 
          integer PTDB_EIOF               !  EIO microwave tube frequency (MHz)
          parameter ( PTDB_EIOF = 8 ) 
          integer PTDB_TTEMPH               !  Target temp measured by He Cell (K x100)
          parameter ( PTDB_TTEMPH = 9 ) 
          integer PTDB_TTEMPC               !  Target temp measured by Cernox (K x100)
          parameter ( PTDB_TTEMPC = 10 ) 
          integer PTDB_ANEALT               !  Date of last target anealing
          parameter ( PTDB_ANEALT = 11 ) 

C    Offsets for the RCST bank follow 

          integer RCST_DOWN_COUNT               !  download count
          parameter ( RCST_DOWN_COUNT = 1 ) 
          integer RCST_PRESTART_COUNT               !  prestart count
          parameter ( RCST_PRESTART_COUNT = 2 ) 
          integer RCST_GO_COUNT               !  go count
          parameter ( RCST_GO_COUNT = 3 ) 
          integer RCST_TRIG_COUNT               !  total trigger count
          parameter ( RCST_TRIG_COUNT = 4 ) 
          integer RCST_EVENT_COUNT               !  phys event count
          parameter ( RCST_EVENT_COUNT = 5 ) 
          integer RCST_SYNC_COUNT               !  force-sync event count
          parameter ( RCST_SYNC_COUNT = 6 ) 
          integer RCST_RUN_TRIG_COUNT               !  trigger count current run
          parameter ( RCST_RUN_TRIG_COUNT = 7 ) 
          integer RCST_RUN_EVENT_COUNT               !  phys event count current run
          parameter ( RCST_RUN_EVENT_COUNT = 8 ) 
          integer RCST_RUN_SYNC_COUNT               !  force-sync event count current run
          parameter ( RCST_RUN_SYNC_COUNT = 9 ) 
          integer RCST_PAUSE_COUNT               !  coda pause count
          parameter ( RCST_PAUSE_COUNT = 10 ) 
          integer RCST_END_COUNT               !  end count
          parameter ( RCST_END_COUNT = 11 ) 
          integer RCST_ILLEGAL_COUNT               !  illegal count
          parameter ( RCST_ILLEGAL_COUNT = 12 ) 
          integer RCST_RUN_ILLEGAL_COUNT               !  illegal count current run
          parameter ( RCST_RUN_ILLEGAL_COUNT = 13 ) 
          integer RCST_PHYS_SYNC_COUNT               !  physics-sync event count
          parameter ( RCST_PHYS_SYNC_COUNT = 14 ) 
          integer RCST_RUN_PHYS_SYNC_COUNT               !  physics-sync event count current run
          parameter ( RCST_RUN_PHYS_SYNC_COUNT = 15 ) 

C    Offsets for the RF bank follow 

          integer RF_RF               !  Best RF value in ns
          parameter ( RF_RF = 1 ) 
          integer RF_RF1               !  RF1 in ns
          parameter ( RF_RF1 = 2 ) 
          integer RF_RF2               !  RF2 in ns
          parameter ( RF_RF2 = 3 ) 

C    Offsets for the RGLK bank follow 

          integer RGLK_IREGION               !  region
          parameter ( RGLK_IREGION = 1 ) 
          integer RGLK_X               !  X pos. of hit in CLAS (cm)
          parameter ( RGLK_X = 2 ) 
          integer RGLK_Y               !  Y pos. of hit in CLAS (cm)
          parameter ( RGLK_Y = 3 ) 
          integer RGLK_Z               !  Z pos. of hit in CLAS (cm)
          parameter ( RGLK_Z = 4 ) 
          integer RGLK_THETA0               !  polar angle of the link position (deg)
          parameter ( RGLK_THETA0 = 5 ) 
          integer RGLK_PHI0               !  azim. angle of the link position (deg)
          parameter ( RGLK_PHI0 = 6 ) 
          integer RGLK_RTHETA               !  polar angle of the link direction (deg)
          parameter ( RGLK_RTHETA = 7 ) 
          integer RGLK_RPHI               !  azim. angle of the link direction (deg)
          parameter ( RGLK_RPHI = 8 ) 
          integer RGLK_CHI2               !  fit chi2        
          parameter ( RGLK_CHI2 = 9 ) 
          integer RGLK_STATUS               !  MINUIT fit status (from 0=bad to 3=ok)
          parameter ( RGLK_STATUS = 10 ) 

C    Offsets for the RNLG bank follow 

          integer RNLG_SQL               !  Run log entry SQL statement
          parameter ( RNLG_SQL = 1 ) 

C    Offsets for the RNPE bank follow 

          integer RNPE_RUN               !  calibration run number
          parameter ( RNPE_RUN = 1 ) 
          integer RNPE_BANK               !  bank name
          parameter ( RNPE_BANK = 2 ) 

C    Offsets for the RTSL bank follow 

          integer RTSL_RAWT1               !  
          parameter ( RTSL_RAWT1 = 1 ) 
          integer RTSL_RAWT2               !  
          parameter ( RTSL_RAWT2 = 2 ) 
          integer RTSL_RAWT3               !  
          parameter ( RTSL_RAWT3 = 3 ) 
          integer RTSL_RAWT4               !  
          parameter ( RTSL_RAWT4 = 4 ) 
          integer RTSL_RAWT5               !  
          parameter ( RTSL_RAWT5 = 5 ) 
          integer RTSL_RAWT6               !  
          parameter ( RTSL_RAWT6 = 6 ) 
          integer RTSL_RAWT7               !  
          parameter ( RTSL_RAWT7 = 7 ) 
          integer RTSL_RAWT8               !  
          parameter ( RTSL_RAWT8 = 8 ) 
          integer RTSL_RAWT9               !  
          parameter ( RTSL_RAWT9 = 9 ) 
          integer RTSL_RAWT10               !  
          parameter ( RTSL_RAWT10 = 10 ) 
          integer RTSL_RAWT11               !  
          parameter ( RTSL_RAWT11 = 11 ) 
          integer RTSL_RAWT12               !  
          parameter ( RTSL_RAWT12 = 12 ) 
          integer RTSL_RAWT13               !  
          parameter ( RTSL_RAWT13 = 13 ) 
          integer RTSL_RAWT14               !  
          parameter ( RTSL_RAWT14 = 14 ) 
          integer RTSL_RAWT15               !  
          parameter ( RTSL_RAWT15 = 15 ) 
          integer RTSL_RAWT16               !  
          parameter ( RTSL_RAWT16 = 16 ) 
          integer RTSL_RAWT17               !  
          parameter ( RTSL_RAWT17 = 17 ) 
          integer RTSL_RAWT18               !  
          parameter ( RTSL_RAWT18 = 18 ) 
          integer RTSL_RAWT19               !  
          parameter ( RTSL_RAWT19 = 19 ) 
          integer RTSL_RAWT20               !  
          parameter ( RTSL_RAWT20 = 20 ) 
          integer RTSL_RAWT21               !  
          parameter ( RTSL_RAWT21 = 21 ) 
          integer RTSL_RAWT22               !  
          parameter ( RTSL_RAWT22 = 22 ) 
          integer RTSL_RAWT23               !  
          parameter ( RTSL_RAWT23 = 23 ) 
          integer RTSL_RAWT24               !  
          parameter ( RTSL_RAWT24 = 24 ) 
          integer RTSL_RAWT25               !  
          parameter ( RTSL_RAWT25 = 25 ) 
          integer RTSL_RAWT26               !  
          parameter ( RTSL_RAWT26 = 26 ) 
          integer RTSL_RAWT27               !  
          parameter ( RTSL_RAWT27 = 27 ) 
          integer RTSL_RAWT28               !  
          parameter ( RTSL_RAWT28 = 28 ) 
          integer RTSL_RAWT29               !  
          parameter ( RTSL_RAWT29 = 29 ) 
          integer RTSL_RAWT30               !  
          parameter ( RTSL_RAWT30 = 30 ) 
          integer RTSL_RAWT31               !  
          parameter ( RTSL_RAWT31 = 31 ) 
          integer RTSL_RAWT32               !  
          parameter ( RTSL_RAWT32 = 32 ) 
          integer RTSL_RAWT33               !  
          parameter ( RTSL_RAWT33 = 33 ) 
          integer RTSL_RAWT34               !  
          parameter ( RTSL_RAWT34 = 34 ) 
          integer RTSL_RAWT35               !  
          parameter ( RTSL_RAWT35 = 35 ) 
          integer RTSL_RAWT36               !  
          parameter ( RTSL_RAWT36 = 36 ) 
          integer RTSL_RAWT37               !  
          parameter ( RTSL_RAWT37 = 37 ) 
          integer RTSL_RAWT38               !  
          parameter ( RTSL_RAWT38 = 38 ) 
          integer RTSL_RAWT39               !  
          parameter ( RTSL_RAWT39 = 39 ) 
          integer RTSL_RAWT40               !  
          parameter ( RTSL_RAWT40 = 40 ) 
          integer RTSL_RAWT41               !  
          parameter ( RTSL_RAWT41 = 41 ) 
          integer RTSL_RAWT42               !  
          parameter ( RTSL_RAWT42 = 42 ) 
          integer RTSL_RAWT43               !  
          parameter ( RTSL_RAWT43 = 43 ) 
          integer RTSL_RAWT44               !  
          parameter ( RTSL_RAWT44 = 44 ) 
          integer RTSL_RAWT45               !  
          parameter ( RTSL_RAWT45 = 45 ) 
          integer RTSL_RAWT46               !  
          parameter ( RTSL_RAWT46 = 46 ) 
          integer RTSL_RAWT47               !  
          parameter ( RTSL_RAWT47 = 47 ) 
          integer RTSL_RAWT48               !  
          parameter ( RTSL_RAWT48 = 48 ) 
          integer RTSL_RAWT49               !  
          parameter ( RTSL_RAWT49 = 49 ) 
          integer RTSL_RAWT50               !  
          parameter ( RTSL_RAWT50 = 50 ) 
          integer RTSL_RAWT51               !  
          parameter ( RTSL_RAWT51 = 51 ) 
          integer RTSL_RAWT52               !  
          parameter ( RTSL_RAWT52 = 52 ) 
          integer RTSL_RAWT53               !  
          parameter ( RTSL_RAWT53 = 53 ) 
          integer RTSL_RAWT54               !  
          parameter ( RTSL_RAWT54 = 54 ) 
          integer RTSL_RAWT55               !  
          parameter ( RTSL_RAWT55 = 55 ) 
          integer RTSL_RAWT56               !  
          parameter ( RTSL_RAWT56 = 56 ) 
          integer RTSL_RAWT57               !  
          parameter ( RTSL_RAWT57 = 57 ) 
          integer RTSL_RAWT58               !  
          parameter ( RTSL_RAWT58 = 58 ) 
          integer RTSL_RAWT59               !  
          parameter ( RTSL_RAWT59 = 59 ) 
          integer RTSL_RAWT60               !  
          parameter ( RTSL_RAWT60 = 60 ) 
          integer RTSL_RAWT61               !  
          parameter ( RTSL_RAWT61 = 61 ) 
          integer RTSL_RAWT62               !  
          parameter ( RTSL_RAWT62 = 62 ) 
          integer RTSL_RAWT63               !  
          parameter ( RTSL_RAWT63 = 63 ) 
          integer RTSL_RAWT64               !  
          parameter ( RTSL_RAWT64 = 64 ) 

C    Offsets for the RUNC bank follow 

          integer RUNC_RUNNO               !  Run number extracted from map
          parameter ( RUNC_RUNNO = 1 ) 
          integer RUNC_BEAM_E               !  beam energy in MeV
          parameter ( RUNC_BEAM_E = 2 ) 
          integer RUNC_B_FIRST               !  firsttime
          parameter ( RUNC_B_FIRST = 3 ) 
          integer RUNC_Q_LIVE               !  Fcup * live time (1*E-10C)
          parameter ( RUNC_Q_LIVE = 4 ) 
          integer RUNC_QL_FIRST               !  firsttime	
          parameter ( RUNC_QL_FIRST = 5 ) 
          integer RUNC_Q_ALL               !  Fcup (1*E-10C)
          parameter ( RUNC_Q_ALL = 6 ) 
          integer RUNC_QA_FIRST               !  firsttime	
          parameter ( RUNC_QA_FIRST = 5 ) 
          integer RUNC_TOR_CURR               !  Torus Current (A)
          parameter ( RUNC_TOR_CURR = 6 ) 
          integer RUNC_T_FIRST               !  firsttime	
          parameter ( RUNC_T_FIRST = 7 ) 
          integer RUNC_MTOR_CURR               !  Minitorus Current (A)
          parameter ( RUNC_MTOR_CURR = 8 ) 
          integer RUNC_MT_FIRST               !  firsttime	
          parameter ( RUNC_MT_FIRST = 9 ) 
          integer RUNC_TAG_CURR               !  Tagger Current (A)
          parameter ( RUNC_TAG_CURR = 8 ) 
          integer RUNC_TAG_FIRST               !  firsttime	
          parameter ( RUNC_TAG_FIRST = 10 ) 

C    Offsets for the S1ST bank follow 

          integer S1ST_LATCH1_BIT1_COUNT               !  Count trigger bit 1  latched events
          parameter ( S1ST_LATCH1_BIT1_COUNT = 1 ) 
          integer S1ST_LATCH1_BIT2_COUNT               !  Count trigger bit 2  latched events
          parameter ( S1ST_LATCH1_BIT2_COUNT = 2 ) 
          integer S1ST_LATCH1_BIT3_COUNT               !  Count trigger bit 3  latched events
          parameter ( S1ST_LATCH1_BIT3_COUNT = 3 ) 
          integer S1ST_LATCH1_BIT4_COUNT               !  Count trigger bit 4  latched events
          parameter ( S1ST_LATCH1_BIT4_COUNT = 4 ) 
          integer S1ST_LATCH1_BIT5_COUNT               !  Count trigger bit 5  latched events
          parameter ( S1ST_LATCH1_BIT5_COUNT = 5 ) 
          integer S1ST_LATCH1_BIT6_COUNT               !  Count trigger bit 6  latched events
          parameter ( S1ST_LATCH1_BIT6_COUNT = 6 ) 
          integer S1ST_LATCH1_BIT7_COUNT               !  Count trigger bit 7  latched events
          parameter ( S1ST_LATCH1_BIT7_COUNT = 7 ) 
          integer S1ST_LATCH1_BIT8_COUNT               !  Count trigger bit 8  latched events
          parameter ( S1ST_LATCH1_BIT8_COUNT = 8 ) 
          integer S1ST_LATCH1_BIT9_COUNT               !  Count trigger bit 9  latched events
          parameter ( S1ST_LATCH1_BIT9_COUNT = 9 ) 
          integer S1ST_LATCH1_BIT10_COUNT               !  Count trigger bit 10 latched events
          parameter ( S1ST_LATCH1_BIT10_COUNT = 10 ) 
          integer S1ST_LATCH1_BIT11_COUNT               !  Count trigger bit 11 latched events
          parameter ( S1ST_LATCH1_BIT11_COUNT = 11 ) 
          integer S1ST_LATCH1_BIT12_COUNT               !  Count trigger bit 12 latched events
          parameter ( S1ST_LATCH1_BIT12_COUNT = 12 ) 
          integer S1ST_EVENT_COUNT               !  Latched event count this run
          parameter ( S1ST_EVENT_COUNT = 13 ) 
          integer S1ST_LATCH1_ZERO_COUNT               !  Latch1 zero count (illegal)
          parameter ( S1ST_LATCH1_ZERO_COUNT = 14 ) 
          integer S1ST_LATCH1_EMPTY_COUNT               !  Latch1 empty count (illegal)
          parameter ( S1ST_LATCH1_EMPTY_COUNT = 15 ) 
          integer S1ST_LATCH1_NOT_EMPTY_COUNT               !  Latch1 not empty on sync event (illegal)
          parameter ( S1ST_LATCH1_NOT_EMPTY_COUNT = 16 ) 
          integer S1ST_LATCH1_OK_COUNT               !  Latch1 ok
          parameter ( S1ST_LATCH1_OK_COUNT = 17 ) 
          integer S1ST_LEVEL2_SECTOR1               !  Level2 sector1 count
          parameter ( S1ST_LEVEL2_SECTOR1 = 18 ) 
          integer S1ST_LEVEL2_SECTOR2               !  Level2 sector2 count
          parameter ( S1ST_LEVEL2_SECTOR2 = 19 ) 
          integer S1ST_LEVEL2_SECTOR3               !  Level2 sector3 count
          parameter ( S1ST_LEVEL2_SECTOR3 = 20 ) 
          integer S1ST_LEVEL2_SECTOR4               !  Level2 sector4 count
          parameter ( S1ST_LEVEL2_SECTOR4 = 21 ) 
          integer S1ST_LEVEL2_SECTOR5               !  Level2 sector5 count
          parameter ( S1ST_LEVEL2_SECTOR5 = 22 ) 
          integer S1ST_LEVEL2_SECTOR6               !  Level2 sector6 count
          parameter ( S1ST_LEVEL2_SECTOR6 = 23 ) 
          integer S1ST_LEVEL2_PASS               !  Level2 pass count
          parameter ( S1ST_LEVEL2_PASS = 24 ) 
          integer S1ST_LEVEL2_FAIL               !  Level2 fail count
          parameter ( S1ST_LEVEL2_FAIL = 25 ) 
          integer S1ST_LATCH2_ZERO_COUNT               !  Latch2 zero count (illegal)
          parameter ( S1ST_LATCH2_ZERO_COUNT = 26 ) 
          integer S1ST_LATCH2_EMPTY_COUNT               !  Latch2 empty count (illegal)
          parameter ( S1ST_LATCH2_EMPTY_COUNT = 27 ) 
          integer S1ST_LATCH2_NOT_EMPTY_COUNT               !  Latch2 not empty on sync event (illegal)
          parameter ( S1ST_LATCH2_NOT_EMPTY_COUNT = 28 ) 
          integer S1ST_LATCH2_OK_COUNT               !  Latch2 ok
          parameter ( S1ST_LATCH2_OK_COUNT = 29 ) 
          integer S1ST_ROC_13_COUNT               !  Roc code 13 count (zero latch)
          parameter ( S1ST_ROC_13_COUNT = 30 ) 
          integer S1ST_ROC_15_COUNT               !  Roc code 15 count (illegal)
          parameter ( S1ST_ROC_15_COUNT = 31 ) 
          integer S1ST_L1L2_ZERO_COUNT               !  (latch1==0)&&(latch2==0)
          parameter ( S1ST_L1L2_ZERO_COUNT = 32 ) 
          integer S1ST_L1ZERO_13_COUNT               !  (latch1==0)&&(roc_code==13)
          parameter ( S1ST_L1ZERO_13_COUNT = 33 ) 
          integer S1ST_L2ZERO_13_COUNT               !  (latch2==0)&&(roc_code==13)
          parameter ( S1ST_L2ZERO_13_COUNT = 34 ) 
          integer S1ST_L1L2ZERO_13_COUNT               !  (latch1==0)&&(latch2==0)&&(roc_code==13)
          parameter ( S1ST_L1L2ZERO_13_COUNT = 35 ) 

C    Offsets for the SC bank follow 

          integer SC_ID               !  the address of the hit detector element
          parameter ( SC_ID = 1 ) 
          integer SC_TDCL               !  tdc information (channels)
          parameter ( SC_TDCL = 2 ) 
          integer SC_ADCL               !  adc information (channels)
          parameter ( SC_ADCL = 3 ) 
          integer SC_TDCR               !  tdc information (channels)
          parameter ( SC_TDCR = 4 ) 
          integer SC_ADCR               !  adc information (channels)
          parameter ( SC_ADCR = 5 ) 

C    Offsets for the SC1 bank follow 

          integer SC1_ID               !  the address of the hit detector element
          parameter ( SC1_ID = 1 ) 
          integer SC1_TIME_L               !  time for left paddle(ns) 
          parameter ( SC1_TIME_L = 2 ) 
          integer SC1_ENERGY_L               !  energy in left paddle(MeV) 
          parameter ( SC1_ENERGY_L = 3 ) 
          integer SC1_TIME_R               !  time for right paddle(ns) 
          parameter ( SC1_TIME_R = 4 ) 
          integer SC1_ENERGY_R               !  energy in right paddle(MeV) 
          parameter ( SC1_ENERGY_R = 5 ) 
          integer SC1_DTIME_L               !  uncertainty in time for left paddle(ns) 
          parameter ( SC1_DTIME_L = 6 ) 
          integer SC1_DENERGY_L               !  uncertainty in energy in left paddle(MeV) 
          parameter ( SC1_DENERGY_L = 7 ) 
          integer SC1_DTIME_R               !  uncertainty in time for right paddle(ns) 
          parameter ( SC1_DTIME_R = 8 ) 
          integer SC1_DENERGY_R               !  uncertainty in energy in right paddle(MeV) 
          parameter ( SC1_DENERGY_R = 9 ) 

C    Offsets for the SCC bank follow 

          integer SCC_ID               !  paddle id# 
          parameter ( SCC_ID = 1 ) 
          integer SCC_DATE               !  UNIX univiersal time of calibraton (32 bits) 
          parameter ( SCC_DATE = 2 ) 
          integer SCC_VERSION               !  sequential version# of calibration 
          parameter ( SCC_VERSION = 3 ) 
          integer SCC_STATUS               !  4 byte status word (see details) 
          parameter ( SCC_STATUS = 4 ) 
          integer SCC_TD0L               !  (Left) gives 0 time at center for tube (ns) 
          parameter ( SCC_TD0L = 5 ) 
          integer SCC_TD0LU               !  (Left) uncertainty (ns) 
          parameter ( SCC_TD0LU = 6 ) 
          integer SCC_TD0R               !  (Right) gives 0 time at center for tube (ns) 
          parameter ( SCC_TD0R = 7 ) 
          integer SCC_TD0RU               !  (Right) uncertainty (ns) 
          parameter ( SCC_TD0RU = 8 ) 
          integer SCC_TD1L               !  (Left) TDC**1 coefficient (ns/ch) 
          parameter ( SCC_TD1L = 9 ) 
          integer SCC_TD1LU               !  (Left) uncertainty (ns/ch) 
          parameter ( SCC_TD1LU = 10 ) 
          integer SCC_TD1R               !  (Right) TDC**1 coefficient (ns/ch) 
          parameter ( SCC_TD1R = 11 ) 
          integer SCC_TD1RU               !  (Right) uncertainty (ns/ch) 
          parameter ( SCC_TD1RU = 12 ) 
          integer SCC_TD2L               !  (Left) TDC**2 coefficient 
          parameter ( SCC_TD2L = 13 ) 
          integer SCC_TD2LU               !  (Left) uncertainty (ns/ch) 
          parameter ( SCC_TD2LU = 14 ) 
          integer SCC_TD2R               !  (Right)TDC**2 coefficient 
          parameter ( SCC_TD2R = 15 ) 
          integer SCC_TD2RU               !  (Right) uncertainty (ns/ch) 
          parameter ( SCC_TD2RU = 16 ) 
          integer SCC_TW0L               !  (Left) time walk constant (ns) 
          parameter ( SCC_TW0L = 17 ) 
          integer SCC_TW0LU               !  (Left) uncertainty (ns) 
          parameter ( SCC_TW0LU = 18 ) 
          integer SCC_TW0R               !  (Right) time walk constant (ns) 
          parameter ( SCC_TW0R = 19 ) 
          integer SCC_TW0RU               !  (Right) uncertainty (ns) 
          parameter ( SCC_TW0RU = 20 ) 
          integer SCC_TW1L               !  (Left) time walk 1st factor 
          parameter ( SCC_TW1L = 21 ) 
          integer SCC_TW1LU               !  (Left) uncertainty 
          parameter ( SCC_TW1LU = 22 ) 
          integer SCC_TW1R               !  (Right) time walk 1st factor 
          parameter ( SCC_TW1R = 23 ) 
          integer SCC_TW1RU               !  (Right) uncertainty 
          parameter ( SCC_TW1RU = 24 ) 
          integer SCC_TW2L               !  (Left) time walk 2nd factor 
          parameter ( SCC_TW2L = 25 ) 
          integer SCC_TW2LU               !  (Left) uncertainty 
          parameter ( SCC_TW2LU = 26 ) 
          integer SCC_TW2R               !  (Right) time walk 2nd factor 
          parameter ( SCC_TW2R = 27 ) 
          integer SCC_TW2RU               !  (Right) uncertainty 
          parameter ( SCC_TW2RU = 28 ) 
          integer SCC_ADPL               !  (Left) ADC pedestal (ch) 
          parameter ( SCC_ADPL = 29 ) 
          integer SCC_ADPLU               !  (Left) uncertainty 
          parameter ( SCC_ADPLU = 30 ) 
          integer SCC_ADPR               !  (Right) ADC pedestal (ch) 
          parameter ( SCC_ADPR = 31 ) 
          integer SCC_ADPRU               !  (Right) uncertainty 
          parameter ( SCC_ADPRU = 32 ) 
          integer SCC_M0L               !  (Left) nmip adc channel 
          parameter ( SCC_M0L = 33 ) 
          integer SCC_M0LU               !  (Left) uncertainty (ch) 
          parameter ( SCC_M0LU = 34 ) 
          integer SCC_M0R               !  (Right)nmip adc channel 
          parameter ( SCC_M0R = 35 ) 
          integer SCC_M0RU               !  (Right) uncertainty(ch) 
          parameter ( SCC_M0RU = 36 ) 
          integer SCC_VEFL               !  (Left) effective velocity of light (cm/ns) 
          parameter ( SCC_VEFL = 37 ) 
          integer SCC_VEFLU               !  (Left) uncertainty (cm/ns) 
          parameter ( SCC_VEFLU = 38 ) 
          integer SCC_VEFR               !  (Right) effective velocity of light (cm/ns) 
          parameter ( SCC_VEFR = 39 ) 
          integer SCC_VEFRU               !  (Right) uncertainty (cm/ns) 
          parameter ( SCC_VEFRU = 40 ) 
          integer SCC_ATNL               !  (Left) attenuation length (cm) 
          parameter ( SCC_ATNL = 41 ) 
          integer SCC_ATNLU               !  (Left) uncertainty (cm) 
          parameter ( SCC_ATNLU = 42 ) 
          integer SCC_ATNR               !  (Right) attenuation length (cm) 
          parameter ( SCC_ATNR = 43 ) 
          integer SCC_ATNRU               !  (Right) uncertainty (cm) 
          parameter ( SCC_ATNRU = 44 ) 
          integer SCC_TSIG0L               !  (Left) 1st parameter of measured resolution (ns) 
          parameter ( SCC_TSIG0L = 45 ) 
          integer SCC_TSIG0R               !  (Right) 1st parameter of measured resolution (ns) 
          parameter ( SCC_TSIG0R = 46 ) 
          integer SCC_TSIG1L               !  (Left) 2nd parameter of measured resolution 
          parameter ( SCC_TSIG1L = 47 ) 
          integer SCC_TSIG1R               !  (Right) 2nd parameter of measured resolution 
          parameter ( SCC_TSIG1R = 48 ) 

C    Offsets for the SCDI bank follow 

          integer SCDI_CRATE               !  CAMAC crate number
          parameter ( SCDI_CRATE = 1 ) 
          integer SCDI_SLOT               !  slot
          parameter ( SCDI_SLOT = 1 ) 
          integer SCDI_MASK               !  mask
          parameter ( SCDI_MASK = 1 ) 
          integer SCDI_THRESHOLD               !  actual threshold value (mV)
          parameter ( SCDI_THRESHOLD = 1 ) 
          integer SCDI_WIDTH               !  actual width value
          parameter ( SCDI_WIDTH = 1 ) 

C    Offsets for the SCG bank follow 

          integer SCG_ID               !   paddle id#
          parameter ( SCG_ID = 1 ) 
          integer SCG_PANEL               !  panel number - to which plane (panel) the paddle belongs
          parameter ( SCG_PANEL = 2 ) 
          integer SCG_XCCW               !  X center of CCW end of paddle in CLAS system
          parameter ( SCG_XCCW = 3 ) 
          integer SCG_YCCW               !  Y center of CCW end 
          parameter ( SCG_YCCW = 4 ) 
          integer SCG_ZCCW               !  - Z center of CCW end
          parameter ( SCG_ZCCW = 5 ) 
          integer SCG_XCW               !  X center of CW end 
          parameter ( SCG_XCW = 6 ) 
          integer SCG_YCW               !  Y center of CW end
          parameter ( SCG_YCW = 7 ) 
          integer SCG_ZCW               ! - Z center of CW end 
          parameter ( SCG_ZCW = 8 ) 
          integer SCG_WIDTH               !  width (cm) (~|| CLAS theta) 
          parameter ( SCG_WIDTH = 9 ) 
          integer SCG_THICK               !  thickness (cm) (~|| CLAS r) 
          parameter ( SCG_THICK = 10 ) 
          integer SCG_DELTA               !  the difference in length of the higher-theta slab minus the lower-theta slab divided by 4 (cm) 
          parameter ( SCG_DELTA = 11 ) 

C    Offsets for the SCGD bank follow 

          integer SCGD_ID               !  paddle # inside the plane (1-23(max))
          parameter ( SCGD_ID = 1 ) 
          integer SCGD_NORM_Z               !  Z(X') coordinate of unit normal to plane
          parameter ( SCGD_NORM_Z = 2 ) 
          integer SCGD_NORM_X               !  Z(X') coordinate of unit normal to plane
          parameter ( SCGD_NORM_X = 3 ) 
          integer SCGD_NORM_D               !  distance to plane along unit vector
          parameter ( SCGD_NORM_D = 4 ) 
          integer SCGD_ALON_Z               !  half width in X direction (along id's)
          parameter ( SCGD_ALON_Z = 5 ) 
          integer SCGD_ALON_X               !  half length of SC in Y direction
          parameter ( SCGD_ALON_X = 6 ) 
          integer SCGD_BEG_PD               !  begin point of the paddle along id
          parameter ( SCGD_BEG_PD = 7 ) 
          integer SCGD_END_PD               !  end point of the paddle along id
          parameter ( SCGD_END_PD = 8 ) 
          integer SCGD_PDL_SH               !  shift of the paddle in respect to the Mid Plane
          parameter ( SCGD_PDL_SH = 9 ) 
          integer SCGD_R_BEAM               !  distance from the beam to the counte center in M.P.
          parameter ( SCGD_R_BEAM = 10 ) 

C    Offsets for the SCH bank follow 

          integer SCH_X               !  x of hit
          parameter ( SCH_X = 1 ) 
          integer SCH_Y               !  y of hit
          parameter ( SCH_Y = 2 ) 
          integer SCH_Z               !  z of hit
          parameter ( SCH_Z = 3 ) 
          integer SCH_CX               !  track x dir cosine
          parameter ( SCH_CX = 4 ) 
          integer SCH_CY               !  track y dir cosine
          parameter ( SCH_CY = 5 ) 
          integer SCH_CZ               !  track z dir cosine
          parameter ( SCH_CZ = 6 ) 
          integer SCH_PMOM               !  track momentum
          parameter ( SCH_PMOM = 7 ) 
          integer SCH_TRACK               !  track number
          parameter ( SCH_TRACK = 8 ) 
          integer SCH_ID               !  track PDG id
          parameter ( SCH_ID = 9 ) 

C    Offsets for the SCMD bank follow 

          integer SCMD_ID               !  paddle # 
          parameter ( SCMD_ID = 1 ) 
          integer SCMD_STATUS               !  general status information 
          parameter ( SCMD_STATUS = 2 ) 
          integer SCMD_X_NORM               !  X of unit vector normal to paddle
          parameter ( SCMD_X_NORM = 3 ) 
          integer SCMD_Y_NORM               !  Y of unit vector normal to paddle
          parameter ( SCMD_Y_NORM = 4 ) 
          integer SCMD_D_NORM               !  normal distance to paddle plane
          parameter ( SCMD_D_NORM = 5 ) 
          integer SCMD_X_PANEL               !  X of unit vector along the panel
          parameter ( SCMD_X_PANEL = 6 ) 
          integer SCMD_Y_PANEL               !  Y of unit vector along the panel
          parameter ( SCMD_Y_PANEL = 7 ) 
          integer SCMD_B_PAD               !  Begin of paddle along the panel
          parameter ( SCMD_B_PAD = 8 ) 
          integer SCMD_E_PAD               !  End of paddle along the panel
          parameter ( SCMD_E_PAD = 9 ) 
          integer SCMD_TOF               !  time of flight (nS)
          parameter ( SCMD_TOF = 10 ) 
          integer SCMD_EDP               !  energy deposited (GeV) 
          parameter ( SCMD_EDP = 11 ) 
          integer SCMD_P_TIME               !  position i.r.t. mid.plane from time
          parameter ( SCMD_P_TIME = 12 ) 
          integer SCMD_P_ATLN               !  position i.r.t. mid.plane from atten.
          parameter ( SCMD_P_ATLN = 13 ) 

C    Offsets for the SCMT bank follow 

          integer SCMT_MEAN_THR               !  mean threshold (mV)
          parameter ( SCMT_MEAN_THR = 1 ) 

C    Offsets for the SCMW bank follow 

          integer SCMW_MEAN_WIDTH               !  mean threshold width
          parameter ( SCMW_MEAN_WIDTH = 1 ) 

C    Offsets for the SCP bank follow 

          integer SCP_N1X               !  x component of outward normal to panel 1
          parameter ( SCP_N1X = 1 ) 
          integer SCP_N1Y               !  y component of outward normal to panel 1
          parameter ( SCP_N1Y = 2 ) 
          integer SCP_N1Z               !  z component of outward normal to panel 1
          parameter ( SCP_N1Z = 3 ) 
          integer SCP_R1N               !  min.dist.inner(cm)from origin to panel 1
          parameter ( SCP_R1N = 4 ) 
          integer SCP_N2X               !  x component of outward normal to panel 2
          parameter ( SCP_N2X = 5 ) 
          integer SCP_N2Y               !  y component of outward normal to panel 2
          parameter ( SCP_N2Y = 6 ) 
          integer SCP_N2Z               !  z component of outward normal to panel 2
          parameter ( SCP_N2Z = 7 ) 
          integer SCP_R2N               !  min.dist.inner(cm)from origin to panel 2
          parameter ( SCP_R2N = 8 ) 
          integer SCP_N3X               !  x component of outward normal to panel 3
          parameter ( SCP_N3X = 9 ) 
          integer SCP_N3Y               !  y component of outward normal to panel 3
          parameter ( SCP_N3Y = 10 ) 
          integer SCP_N3Z               !  z component of outward normal to panel 3
          parameter ( SCP_N3Z = 11 ) 
          integer SCP_R3N               !  min.dist.inner(cm)from origin to panel 3
          parameter ( SCP_R3N = 12 ) 
          integer SCP_N4X               !  x component of outward normal to panel 4
          parameter ( SCP_N4X = 13 ) 
          integer SCP_N4Y               !  y component of outward normal to panel 4
          parameter ( SCP_N4Y = 14 ) 
          integer SCP_N4Z               !  z component of outward normal to panel 4
          parameter ( SCP_N4Z = 15 ) 
          integer SCP_R4N               !  min.dist.inner(cm)from origin to panel 4
          parameter ( SCP_R4N = 16 ) 

C    Offsets for the SCPB bank follow 

          integer SCPB_SCPDHT               !  10000*sector+100*SC_PD_ID+Hit_ID in SCR 
          parameter ( SCPB_SCPDHT = 1 ) 
          integer SCPB_EDEP               !  Deposited energy (dE/dX)
          parameter ( SCPB_EDEP = 2 ) 
          integer SCPB_TIME               !  Flight time relative to the evnt start time
          parameter ( SCPB_TIME = 3 ) 
          integer SCPB_PATH               !  Path lenght from target
          parameter ( SCPB_PATH = 4 ) 
          integer SCPB_CHI2SC               !  Quality measure of geometrical matching
          parameter ( SCPB_CHI2SC = 5 ) 
          integer SCPB_STATUS               !  Status word (not defined yet)
          parameter ( SCPB_STATUS = 6 ) 

C    Offsets for the SCPE bank follow 

          integer SCPE_ID               !  the address of the hit detector element
          parameter ( SCPE_ID = 1 ) 
          integer SCPE_MEAN_LEFT               !  left adc pedestal mean value (channel)
          parameter ( SCPE_MEAN_LEFT = 2 ) 
          integer SCPE_SIGMA_LEFT               !  sigma of the pedestal distribution for left adc (channel)
          parameter ( SCPE_SIGMA_LEFT = 3 ) 
          integer SCPE_MEAN_RIGHT               !  right adc pedestal mean value (channel)
          parameter ( SCPE_MEAN_RIGHT = 4 ) 
          integer SCPE_SIGMA_RIGHT               !  sigma of the pedestal distribution for right adc (channel
          parameter ( SCPE_SIGMA_RIGHT = 5 ) 

C    Offsets for the SCPS bank follow 

          integer SCPS_ID               !  Plane #(1-6):1-23,24-34,35-39,40-42,43-46,47-48
          parameter ( SCPS_ID = 1 ) 
          integer SCPS_FIRSTPNUM               !  The first paddle number in the plane
          parameter ( SCPS_FIRSTPNUM = 2 ) 
          integer SCPS_LASTPNUM               !  The last paddle number in the plane
          parameter ( SCPS_LASTPNUM = 3 ) 
          integer SCPS_DISTANCE               !  distance to the center of the plane
          parameter ( SCPS_DISTANCE = 4 ) 
          integer SCPS_BEGALONSHFT               !  X-shift of Begin Plane poin along id of SC
          parameter ( SCPS_BEGALONSHFT = 5 ) 
          integer SCPS_ENDALONSHFT               !  X-shift of End of Plane poin along id of SC
          parameter ( SCPS_ENDALONSHFT = 6 ) 
          integer SCPS_LENGTHSHFT               !  Y-shift of Plane SYS along the SC length
          parameter ( SCPS_LENGTHSHFT = 7 ) 
          integer SCPS_ALONGIDX               !  XP.x direction for width in Sector System
          parameter ( SCPS_ALONGIDX = 8 ) 
          integer SCPS_ALONGIDY               !  XP.y direction for width in Sector System
          parameter ( SCPS_ALONGIDY = 9 ) 
          integer SCPS_ALONGIDZ               !  XP.z direction for width in Sector System
          parameter ( SCPS_ALONGIDZ = 10 ) 
          integer SCPS_SLENGTHX               !  YP.x direction for length in Sector System
          parameter ( SCPS_SLENGTHX = 11 ) 
          integer SCPS_SLENGTHY               !  YP.y direction for length in Sector System
          parameter ( SCPS_SLENGTHY = 12 ) 
          integer SCPS_SLENGTHZ               !  YP.z direction for length in Sector System
          parameter ( SCPS_SLENGTHZ = 13 ) 
          integer SCPS_UNORMALX               !  ZP.x direction for thickness in Sector System
          parameter ( SCPS_UNORMALX = 14 ) 
          integer SCPS_UNORMALY               !  ZP.y direction for thickness in Sector System
          parameter ( SCPS_UNORMALY = 15 ) 
          integer SCPS_UNORMALZ               !  ZP.z direction for thickness in Sector System
          parameter ( SCPS_UNORMALZ = 16 ) 

C    Offsets for the SCR bank follow 

          integer SCR_ID               !  paddle id  
          parameter ( SCR_ID = 1 ) 
          integer SCR_ENERGY               !  Energy (MeV) 
          parameter ( SCR_ENERGY = 2 ) 
          integer SCR_TIME               !  time(ns)
          parameter ( SCR_TIME = 3 ) 
          integer SCR_X               !  x position in sector coodinate system 
          parameter ( SCR_X = 4 ) 
          integer SCR_Y               !  y position in sector coodinate system 
          parameter ( SCR_Y = 5 ) 
          integer SCR_Z               !  z position in sector coodinate system 
          parameter ( SCR_Z = 6 ) 
          integer SCR_DX               !  x error in sector coodinate system 
          parameter ( SCR_DX = 7 ) 
          integer SCR_DY               !  y error in sector coodinate system 
          parameter ( SCR_DY = 8 ) 
          integer SCR_DZ               !  z error in sector coodinate system 
          parameter ( SCR_DZ = 9 ) 
          integer SCR_STATUS               !  status word defined in sc.h
          parameter ( SCR_STATUS = 10 ) 
          integer SCR_DENERGY               !  uncertainty in Energy (MeV) 
          parameter ( SCR_DENERGY = 11 ) 
          integer SCR_DTIME               !  uncertainty in time (ns) 
          parameter ( SCR_DTIME = 12 ) 

C    Offsets for the SCRC bank follow 

          integer SCRC_ID               !  cluster id   
          parameter ( SCRC_ID = 1 ) 
          integer SCRC_ENERGY               !  cluster Energy (MeV) 
          parameter ( SCRC_ENERGY = 2 ) 
          integer SCRC_DENERGY               !  error in cluster energy (ns)
          parameter ( SCRC_DENERGY = 3 ) 
          integer SCRC_TIME               !  cluster (energy-weighted) time(ns) 
          parameter ( SCRC_TIME = 4 ) 
          integer SCRC_DTIME               !  error in cluster time (ns)
          parameter ( SCRC_DTIME = 5 ) 
          integer SCRC_X               !  x position in sector coordinate system 
          parameter ( SCRC_X = 6 ) 
          integer SCRC_Y               !  y position in sector coordinate system 
          parameter ( SCRC_Y = 7 ) 
          integer SCRC_Z               !  z position in sector coordinate system 
          parameter ( SCRC_Z = 8 ) 
          integer SCRC_DX               !  x error in sector coordinate system 
          parameter ( SCRC_DX = 9 ) 
          integer SCRC_DY               !  y error in sector coordinate system 
          parameter ( SCRC_DY = 10 ) 
          integer SCRC_DZ               !  z error in sector coordinate system 
          parameter ( SCRC_DZ = 11 ) 
          integer SCRC_STATUS               !  status word defined in sc.h
          parameter ( SCRC_STATUS = 12 ) 

C    Offsets for the SCS bank follow 

          integer SCS_SCS1               !  scaler sector 1
          parameter ( SCS_SCS1 = 1 ) 
          integer SCS_SCS2               !  scaler sector 1
          parameter ( SCS_SCS2 = 2 ) 
          integer SCS_SCS3               !  scaler sector 1
          parameter ( SCS_SCS3 = 3 ) 
          integer SCS_SCS4               !  scaler sector 1
          parameter ( SCS_SCS4 = 4 ) 
          integer SCS_SCS5               !  scaler sector 1
          parameter ( SCS_SCS5 = 5 ) 
          integer SCS_SCS6               !  scaler sector 1
          parameter ( SCS_SCS6 = 6 ) 
          integer SCS_SCS7               !  scaler sector 1
          parameter ( SCS_SCS7 = 7 ) 
          integer SCS_SCS8               !  scaler sector 1
          parameter ( SCS_SCS8 = 8 ) 
          integer SCS_SCS9               !  scaler sector 1
          parameter ( SCS_SCS9 = 9 ) 
          integer SCS_SCS10               !  scaler sector 1
          parameter ( SCS_SCS10 = 10 ) 
          integer SCS_SCS11               !  scaler sector 1
          parameter ( SCS_SCS11 = 11 ) 
          integer SCS_SCS12               !  scaler sector 1
          parameter ( SCS_SCS12 = 12 ) 
          integer SCS_SCS13               !  scaler sector 1
          parameter ( SCS_SCS13 = 13 ) 
          integer SCS_SCS14               !  scaler sector 1
          parameter ( SCS_SCS14 = 14 ) 
          integer SCS_SCS15               !  scaler sector 1
          parameter ( SCS_SCS15 = 15 ) 
          integer SCS_SCS16               !  scaler sector 1
          parameter ( SCS_SCS16 = 16 ) 
          integer SCS_SCS17               !  scaler sector 1
          parameter ( SCS_SCS17 = 17 ) 
          integer SCS_SCS18               !  scaler sector 1
          parameter ( SCS_SCS18 = 18 ) 
          integer SCS_SCS19               !  scaler sector 1
          parameter ( SCS_SCS19 = 19 ) 
          integer SCS_SCS20               !  scaler sector 1
          parameter ( SCS_SCS20 = 20 ) 
          integer SCS_SCS21               !  scaler sector 1
          parameter ( SCS_SCS21 = 21 ) 
          integer SCS_SCS22               !  scaler sector 1
          parameter ( SCS_SCS22 = 22 ) 
          integer SCS_SCS23               !  scaler sector 1
          parameter ( SCS_SCS23 = 23 ) 
          integer SCS_SCS24               !  scaler sector 1
          parameter ( SCS_SCS24 = 24 ) 
          integer SCS_SCS25               !  scaler sector 1
          parameter ( SCS_SCS25 = 25 ) 
          integer SCS_SCS26               !  scaler sector 1
          parameter ( SCS_SCS26 = 26 ) 
          integer SCS_SCS27               !  scaler sector 1
          parameter ( SCS_SCS27 = 27 ) 
          integer SCS_SCS28               !  scaler sector 1
          parameter ( SCS_SCS28 = 28 ) 
          integer SCS_SCS29               !  scaler sector 1
          parameter ( SCS_SCS29 = 29 ) 
          integer SCS_SCS30               !  scaler sector 1
          parameter ( SCS_SCS30 = 30 ) 
          integer SCS_SCS31               !  scaler sector 1
          parameter ( SCS_SCS31 = 31 ) 
          integer SCS_SCS32               !  scaler sector 1
          parameter ( SCS_SCS32 = 32 ) 
          integer SCS_SCS33               !  scaler sector 2
          parameter ( SCS_SCS33 = 33 ) 
          integer SCS_SCS34               !  scaler sector 2
          parameter ( SCS_SCS34 = 34 ) 
          integer SCS_SCS35               !  scaler sector 2
          parameter ( SCS_SCS35 = 35 ) 
          integer SCS_SCS36               !  scaler sector 2
          parameter ( SCS_SCS36 = 36 ) 
          integer SCS_SCS37               !  scaler sector 2
          parameter ( SCS_SCS37 = 37 ) 
          integer SCS_SCS38               !  scaler sector 2
          parameter ( SCS_SCS38 = 38 ) 
          integer SCS_SCS39               !  scaler sector 2
          parameter ( SCS_SCS39 = 39 ) 
          integer SCS_SCS40               !  scaler sector 2
          parameter ( SCS_SCS40 = 40 ) 
          integer SCS_SCS41               !  scaler sector 2
          parameter ( SCS_SCS41 = 41 ) 
          integer SCS_SCS42               !  scaler sector 2
          parameter ( SCS_SCS42 = 42 ) 
          integer SCS_SCS43               !  scaler sector 2
          parameter ( SCS_SCS43 = 43 ) 
          integer SCS_SCS44               !  scaler sector 2
          parameter ( SCS_SCS44 = 44 ) 
          integer SCS_SCS45               !  scaler sector 2
          parameter ( SCS_SCS45 = 45 ) 
          integer SCS_SCS46               !  scaler sector 2
          parameter ( SCS_SCS46 = 46 ) 
          integer SCS_SCS47               !  scaler sector 2
          parameter ( SCS_SCS47 = 47 ) 
          integer SCS_SCS48               !  scaler sector 2
          parameter ( SCS_SCS48 = 48 ) 
          integer SCS_SCS49               !  scaler sector 2
          parameter ( SCS_SCS49 = 49 ) 
          integer SCS_SCS50               !  scaler sector 2
          parameter ( SCS_SCS50 = 50 ) 
          integer SCS_SCS51               !  scaler sector 2
          parameter ( SCS_SCS51 = 51 ) 
          integer SCS_SCS52               !  scaler sector 2
          parameter ( SCS_SCS52 = 52 ) 
          integer SCS_SCS53               !  scaler sector 2
          parameter ( SCS_SCS53 = 53 ) 
          integer SCS_SCS54               !  scaler sector 2
          parameter ( SCS_SCS54 = 54 ) 
          integer SCS_SCS55               !  scaler sector 2
          parameter ( SCS_SCS55 = 55 ) 
          integer SCS_SCS56               !  scaler sector 2
          parameter ( SCS_SCS56 = 56 ) 
          integer SCS_SCS57               !  scaler sector 2
          parameter ( SCS_SCS57 = 57 ) 
          integer SCS_SCS58               !  scaler sector 2
          parameter ( SCS_SCS58 = 58 ) 
          integer SCS_SCS59               !  scaler sector 2
          parameter ( SCS_SCS59 = 59 ) 
          integer SCS_SCS60               !  scaler sector 2
          parameter ( SCS_SCS60 = 60 ) 
          integer SCS_SCS61               !  scaler sector 2
          parameter ( SCS_SCS61 = 61 ) 
          integer SCS_SCS62               !  scaler sector 2
          parameter ( SCS_SCS62 = 62 ) 
          integer SCS_SCS63               !  scaler sector 2
          parameter ( SCS_SCS63 = 63 ) 
          integer SCS_SCS64               !  scaler sector 2
          parameter ( SCS_SCS64 = 64 ) 
          integer SCS_SCS65               !  scaler sector 3
          parameter ( SCS_SCS65 = 65 ) 
          integer SCS_SCS66               !  scaler sector 3
          parameter ( SCS_SCS66 = 66 ) 
          integer SCS_SCS67               !  scaler sector 3
          parameter ( SCS_SCS67 = 67 ) 
          integer SCS_SCS68               !  scaler sector 3
          parameter ( SCS_SCS68 = 68 ) 
          integer SCS_SCS69               !  scaler sector 3
          parameter ( SCS_SCS69 = 69 ) 
          integer SCS_SCS70               !  scaler sector 3
          parameter ( SCS_SCS70 = 70 ) 
          integer SCS_SCS71               !  scaler sector 3
          parameter ( SCS_SCS71 = 71 ) 
          integer SCS_SCS72               !  scaler sector 3
          parameter ( SCS_SCS72 = 72 ) 
          integer SCS_SCS73               !  scaler sector 3
          parameter ( SCS_SCS73 = 73 ) 
          integer SCS_SCS74               !  scaler sector 3
          parameter ( SCS_SCS74 = 74 ) 
          integer SCS_SCS75               !  scaler sector 3
          parameter ( SCS_SCS75 = 75 ) 
          integer SCS_SCS76               !  scaler sector 3
          parameter ( SCS_SCS76 = 76 ) 
          integer SCS_SCS77               !  scaler sector 3
          parameter ( SCS_SCS77 = 77 ) 
          integer SCS_SCS78               !  scaler sector 3
          parameter ( SCS_SCS78 = 78 ) 
          integer SCS_SCS79               !  scaler sector 3
          parameter ( SCS_SCS79 = 79 ) 
          integer SCS_SCS80               !  scaler sector 3
          parameter ( SCS_SCS80 = 80 ) 
          integer SCS_SCS81               !  scaler sector 3
          parameter ( SCS_SCS81 = 81 ) 
          integer SCS_SCS82               !  scaler sector 3
          parameter ( SCS_SCS82 = 82 ) 
          integer SCS_SCS83               !  scaler sector 3
          parameter ( SCS_SCS83 = 83 ) 
          integer SCS_SCS84               !  scaler sector 3
          parameter ( SCS_SCS84 = 84 ) 
          integer SCS_SCS85               !  scaler sector 3
          parameter ( SCS_SCS85 = 85 ) 
          integer SCS_SCS86               !  scaler sector 3
          parameter ( SCS_SCS86 = 86 ) 
          integer SCS_SCS87               !  scaler sector 3
          parameter ( SCS_SCS87 = 87 ) 
          integer SCS_SCS88               !  scaler sector 3
          parameter ( SCS_SCS88 = 88 ) 
          integer SCS_SCS89               !  scaler sector 3
          parameter ( SCS_SCS89 = 89 ) 
          integer SCS_SCS90               !  scaler sector 3
          parameter ( SCS_SCS90 = 90 ) 
          integer SCS_SCS91               !  scaler sector 3
          parameter ( SCS_SCS91 = 91 ) 
          integer SCS_SCS92               !  scaler sector 3
          parameter ( SCS_SCS92 = 92 ) 
          integer SCS_SCS93               !  scaler sector 3
          parameter ( SCS_SCS93 = 93 ) 
          integer SCS_SCS94               !  scaler sector 3
          parameter ( SCS_SCS94 = 94 ) 
          integer SCS_SCS95               !  scaler sector 3
          parameter ( SCS_SCS95 = 95 ) 
          integer SCS_SCS96               !  scaler sector 3
          parameter ( SCS_SCS96 = 96 ) 
          integer SCS_SCS97               !  scaler sector 4
          parameter ( SCS_SCS97 = 97 ) 
          integer SCS_SCS98               !  scaler sector 4
          parameter ( SCS_SCS98 = 98 ) 
          integer SCS_SCS99               !  scaler sector 4
          parameter ( SCS_SCS99 = 99 ) 
          integer SCS_SCS100               !  scaler sector 4
          parameter ( SCS_SCS100 = 100 ) 
          integer SCS_SCS101               !  scaler sector 4
          parameter ( SCS_SCS101 = 101 ) 
          integer SCS_SCS102               !  scaler sector 4
          parameter ( SCS_SCS102 = 102 ) 
          integer SCS_SCS103               !  scaler sector 4
          parameter ( SCS_SCS103 = 103 ) 
          integer SCS_SCS104               !  scaler sector 4
          parameter ( SCS_SCS104 = 104 ) 
          integer SCS_SCS105               !  scaler sector 4
          parameter ( SCS_SCS105 = 105 ) 
          integer SCS_SCS106               !  scaler sector 4
          parameter ( SCS_SCS106 = 106 ) 
          integer SCS_SCS107               !  scaler sector 4
          parameter ( SCS_SCS107 = 107 ) 
          integer SCS_SCS108               !  scaler sector 4
          parameter ( SCS_SCS108 = 108 ) 
          integer SCS_SCS109               !  scaler sector 4
          parameter ( SCS_SCS109 = 109 ) 
          integer SCS_SCS110               !  scaler sector 4
          parameter ( SCS_SCS110 = 110 ) 
          integer SCS_SCS111               !  scaler sector 4
          parameter ( SCS_SCS111 = 111 ) 
          integer SCS_SCS112               !  scaler sector 4
          parameter ( SCS_SCS112 = 112 ) 
          integer SCS_SCS113               !  scaler sector 4
          parameter ( SCS_SCS113 = 113 ) 
          integer SCS_SCS114               !  scaler sector 4
          parameter ( SCS_SCS114 = 114 ) 
          integer SCS_SCS115               !  scaler sector 4
          parameter ( SCS_SCS115 = 115 ) 
          integer SCS_SCS116               !  scaler sector 4
          parameter ( SCS_SCS116 = 116 ) 
          integer SCS_SCS117               !  scaler sector 4
          parameter ( SCS_SCS117 = 117 ) 
          integer SCS_SCS118               !  scaler sector 4
          parameter ( SCS_SCS118 = 118 ) 
          integer SCS_SCS119               !  scaler sector 4
          parameter ( SCS_SCS119 = 119 ) 
          integer SCS_SCS120               !  scaler sector 4
          parameter ( SCS_SCS120 = 120 ) 
          integer SCS_SCS121               !  scaler sector 4
          parameter ( SCS_SCS121 = 121 ) 
          integer SCS_SCS122               !  scaler sector 4
          parameter ( SCS_SCS122 = 122 ) 
          integer SCS_SCS123               !  scaler sector 4
          parameter ( SCS_SCS123 = 123 ) 
          integer SCS_SCS124               !  scaler sector 4
          parameter ( SCS_SCS124 = 124 ) 
          integer SCS_SCS125               !  scaler sector 4
          parameter ( SCS_SCS125 = 125 ) 
          integer SCS_SCS126               !  scaler sector 4
          parameter ( SCS_SCS126 = 126 ) 
          integer SCS_SCS127               !  scaler sector 4
          parameter ( SCS_SCS127 = 127 ) 
          integer SCS_SCS128               !  scaler sector 4
          parameter ( SCS_SCS128 = 128 ) 
          integer SCS_SCS129               !  scaler sector 5
          parameter ( SCS_SCS129 = 129 ) 
          integer SCS_SCS130               !  scaler sector 5
          parameter ( SCS_SCS130 = 130 ) 
          integer SCS_SCS131               !  scaler sector 5
          parameter ( SCS_SCS131 = 131 ) 
          integer SCS_SCS132               !  scaler sector 5
          parameter ( SCS_SCS132 = 132 ) 
          integer SCS_SCS133               !  scaler sector 5
          parameter ( SCS_SCS133 = 133 ) 
          integer SCS_SCS134               !  scaler sector 5
          parameter ( SCS_SCS134 = 134 ) 
          integer SCS_SCS135               !  scaler sector 5
          parameter ( SCS_SCS135 = 135 ) 
          integer SCS_SCS136               !  scaler sector 5
          parameter ( SCS_SCS136 = 136 ) 
          integer SCS_SCS137               !  scaler sector 5
          parameter ( SCS_SCS137 = 137 ) 
          integer SCS_SCS138               !  scaler sector 5
          parameter ( SCS_SCS138 = 138 ) 
          integer SCS_SCS139               !  scaler sector 5
          parameter ( SCS_SCS139 = 139 ) 
          integer SCS_SCS140               !  scaler sector 5
          parameter ( SCS_SCS140 = 140 ) 
          integer SCS_SCS141               !  scaler sector 5
          parameter ( SCS_SCS141 = 141 ) 
          integer SCS_SCS142               !  scaler sector 5
          parameter ( SCS_SCS142 = 142 ) 
          integer SCS_SCS143               !  scaler sector 5
          parameter ( SCS_SCS143 = 143 ) 
          integer SCS_SCS144               !  scaler sector 5
          parameter ( SCS_SCS144 = 144 ) 
          integer SCS_SCS145               !  scaler sector 5
          parameter ( SCS_SCS145 = 145 ) 
          integer SCS_SCS146               !  scaler sector 5
          parameter ( SCS_SCS146 = 146 ) 
          integer SCS_SCS147               !  scaler sector 5
          parameter ( SCS_SCS147 = 147 ) 
          integer SCS_SCS148               !  scaler sector 5
          parameter ( SCS_SCS148 = 148 ) 
          integer SCS_SCS149               !  scaler sector 5
          parameter ( SCS_SCS149 = 149 ) 
          integer SCS_SCS150               !  scaler sector 5
          parameter ( SCS_SCS150 = 150 ) 
          integer SCS_SCS151               !  scaler sector 5
          parameter ( SCS_SCS151 = 151 ) 
          integer SCS_SCS152               !  scaler sector 5
          parameter ( SCS_SCS152 = 152 ) 
          integer SCS_SCS153               !  scaler sector 5
          parameter ( SCS_SCS153 = 153 ) 
          integer SCS_SCS154               !  scaler sector 5
          parameter ( SCS_SCS154 = 154 ) 
          integer SCS_SCS155               !  scaler sector 5
          parameter ( SCS_SCS155 = 155 ) 
          integer SCS_SCS156               !  scaler sector 5
          parameter ( SCS_SCS156 = 156 ) 
          integer SCS_SCS157               !  scaler sector 5
          parameter ( SCS_SCS157 = 157 ) 
          integer SCS_SCS158               !  scaler sector 5
          parameter ( SCS_SCS158 = 158 ) 
          integer SCS_SCS159               !  scaler sector 5
          parameter ( SCS_SCS159 = 159 ) 
          integer SCS_SCS160               !  scaler sector 5
          parameter ( SCS_SCS160 = 160 ) 
          integer SCS_SCS161               !  scaler sector 6
          parameter ( SCS_SCS161 = 161 ) 
          integer SCS_SCS162               !  scaler sector 6
          parameter ( SCS_SCS162 = 162 ) 
          integer SCS_SCS163               !  scaler sector 6
          parameter ( SCS_SCS163 = 163 ) 
          integer SCS_SCS164               !  scaler sector 6
          parameter ( SCS_SCS164 = 164 ) 
          integer SCS_SCS165               !  scaler sector 6
          parameter ( SCS_SCS165 = 165 ) 
          integer SCS_SCS166               !  scaler sector 6
          parameter ( SCS_SCS166 = 166 ) 
          integer SCS_SCS167               !  scaler sector 6
          parameter ( SCS_SCS167 = 167 ) 
          integer SCS_SCS168               !  scaler sector 6
          parameter ( SCS_SCS168 = 168 ) 
          integer SCS_SCS169               !  scaler sector 6
          parameter ( SCS_SCS169 = 169 ) 
          integer SCS_SCS170               !  scaler sector 6
          parameter ( SCS_SCS170 = 170 ) 
          integer SCS_SCS171               !  scaler sector 6
          parameter ( SCS_SCS171 = 171 ) 
          integer SCS_SCS172               !  scaler sector 6
          parameter ( SCS_SCS172 = 172 ) 
          integer SCS_SCS173               !  scaler sector 6
          parameter ( SCS_SCS173 = 173 ) 
          integer SCS_SCS174               !  scaler sector 6
          parameter ( SCS_SCS174 = 174 ) 
          integer SCS_SCS175               !  scaler sector 6
          parameter ( SCS_SCS175 = 175 ) 
          integer SCS_SCS176               !  scaler sector 6
          parameter ( SCS_SCS176 = 176 ) 
          integer SCS_SCS177               !  scaler sector 6
          parameter ( SCS_SCS177 = 177 ) 
          integer SCS_SCS178               !  scaler sector 6
          parameter ( SCS_SCS178 = 178 ) 
          integer SCS_SCS179               !  scaler sector 6
          parameter ( SCS_SCS179 = 179 ) 
          integer SCS_SCS180               !  scaler sector 6
          parameter ( SCS_SCS180 = 180 ) 
          integer SCS_SCS181               !  scaler sector 6
          parameter ( SCS_SCS181 = 181 ) 
          integer SCS_SCS182               !  scaler sector 6
          parameter ( SCS_SCS182 = 182 ) 
          integer SCS_SCS183               !  scaler sector 6
          parameter ( SCS_SCS183 = 183 ) 
          integer SCS_SCS184               !  scaler sector 6
          parameter ( SCS_SCS184 = 184 ) 
          integer SCS_SCS185               !  scaler sector 6
          parameter ( SCS_SCS185 = 185 ) 
          integer SCS_SCS186               !  scaler sector 6
          parameter ( SCS_SCS186 = 186 ) 
          integer SCS_SCS187               !  scaler sector 6
          parameter ( SCS_SCS187 = 187 ) 
          integer SCS_SCS188               !  scaler sector 6
          parameter ( SCS_SCS188 = 188 ) 
          integer SCS_SCS189               !  scaler sector 6
          parameter ( SCS_SCS189 = 189 ) 
          integer SCS_SCS190               !  scaler sector 6
          parameter ( SCS_SCS190 = 190 ) 
          integer SCS_SCS191               !  scaler sector 6
          parameter ( SCS_SCS191 = 191 ) 
          integer SCS_SCS192               !  scaler sector 6
          parameter ( SCS_SCS192 = 192 ) 

C    Offsets for the SGMP bank follow 

          integer SGMP_IS               !  superlayer number of the segment
          parameter ( SGMP_IS = 1 ) 
          integer SGMP_SGM1               !  data structure see: include/bosddl.h, wire_t
          parameter ( SGMP_SGM1 = 2 ) 
          integer SGMP_SGM2               !  wire_t
          parameter ( SGMP_SGM2 = 3 ) 
          integer SGMP_SGM3               !  wire_t
          parameter ( SGMP_SGM3 = 4 ) 
          integer SGMP_SGM4               !  wire_t
          parameter ( SGMP_SGM4 = 5 ) 
          integer SGMP_SGM5               !  wire_t
          parameter ( SGMP_SGM5 = 6 ) 
          integer SGMP_SGM6               !  wire_t
          parameter ( SGMP_SGM6 = 7 ) 

C    Offsets for the SPAR bank follow 

          integer SPAR_SLOT               !  ADC slot
          parameter ( SPAR_SLOT = 1 ) 
          integer SPAR_CHANNEL               !  ADC channel
          parameter ( SPAR_CHANNEL = 2 ) 
          integer SPAR_SPAR               !  sparsification threshold (channel)
          parameter ( SPAR_SPAR = 3 ) 
          integer SPAR_PEDMEAN               !  pedestal mean value measured, using internal 500ns gate (channel)
          parameter ( SPAR_PEDMEAN = 4 ) 

C    Offsets for the SPIN bank follow 

          integer SPIN_XSPIN               !  x component of spin
          parameter ( SPIN_XSPIN = 1 ) 
          integer SPIN_YSPIN               !  y component of spin
          parameter ( SPIN_YSPIN = 2 ) 
          integer SPIN_ZSPIN               !  z component of spin
          parameter ( SPIN_ZSPIN = 3 ) 

C    Offsets for the ST bank follow 

          integer ST_ID               !  the address of the hit detector element
          parameter ( ST_ID = 1 ) 
          integer ST_TDC               !  tdc information (channels)
          parameter ( ST_TDC = 2 ) 
          integer ST_ADC               !  adc information (channels)
          parameter ( ST_ADC = 3 ) 

C    Offsets for the ST1 bank follow 

          integer ST1_ID               !  Pair id
          parameter ( ST1_ID = 1 ) 
          integer ST1_STATUS               !  status word
          parameter ( ST1_STATUS = 2 ) 
          integer ST1_TIME_1               !  Time (ns) for side 1
          parameter ( ST1_TIME_1 = 3 ) 
          integer ST1_ADC_1               !  Pedestal-subtracted adc for side 1
          parameter ( ST1_ADC_1 = 4 ) 
          integer ST1_TIME_2               !  Time (ns) for side 2
          parameter ( ST1_TIME_2 = 5 ) 
          integer ST1_ADC_2               !  Pedestal-subtracted ADC  for side 2
          parameter ( ST1_ADC_2 = 6 ) 

C    Offsets for the STH bank follow 

          integer STH_X               !  x of hit
          parameter ( STH_X = 1 ) 
          integer STH_Y               !  y of hit
          parameter ( STH_Y = 2 ) 
          integer STH_Z               !  z of hit
          parameter ( STH_Z = 3 ) 
          integer STH_CX               !  track x dir cosine
          parameter ( STH_CX = 4 ) 
          integer STH_CY               !  track y dir cosine
          parameter ( STH_CY = 5 ) 
          integer STH_CZ               !  track z dir cosine
          parameter ( STH_CZ = 6 ) 
          integer STH_PMOM               !  track momentum
          parameter ( STH_PMOM = 7 ) 
          integer STH_TRACK               !  track number
          parameter ( STH_TRACK = 8 ) 
          integer STH_ID               !  track PDG id
          parameter ( STH_ID = 9 ) 
          integer STH_TOF               !  flight time
          parameter ( STH_TOF = 10 ) 

C    Offsets for the STPB bank follow 

          integer STPB_STHID               !  100*sector+Hit_ID  
          parameter ( STPB_STHID = 1 ) 
          integer STPB_TIME               !  Flight time relative to the evnt start time
          parameter ( STPB_TIME = 2 ) 
          integer STPB_PATH               !  Path lenght from target
          parameter ( STPB_PATH = 3 ) 
          integer STPB_CHARGE               !  charge (get from tracking)
          parameter ( STPB_CHARGE = 4 ) 
          integer STPB_STATUS               !  Status word (not defined yet)
          parameter ( STPB_STATUS = 5 ) 

C    Offsets for the STPE bank follow 

          integer STPE_ID               !  the address of the hit detector element
          parameter ( STPE_ID = 1 ) 
          integer STPE_MEAN               !  adc pedestal mean value (channel)
          parameter ( STPE_MEAN = 2 ) 
          integer STPE_SIGMA               !  sigma of the pedestal distribution (channel)
          parameter ( STPE_SIGMA = 3 ) 

C    Offsets for the STR bank follow 

          integer STR_ID               !  sector
          parameter ( STR_ID = 1 ) 
          integer STR_TRK_NO               !  Pointer to track in HBTR
          parameter ( STR_TRK_NO = 2 ) 
          integer STR_ST_TIME               !  flight time from ST (ns)
          parameter ( STR_ST_TIME = 3 ) 
          integer STR_ST_L               !  flight path from ST (cm)
          parameter ( STR_ST_L = 4 ) 
          integer STR_ST_POS               !  position within the start counter
          parameter ( STR_ST_POS = 5 ) 
          integer STR_STATUS               !  Status word
          parameter ( STR_STATUS = 6 ) 

C    Offsets for the STS bank follow 

          integer STS_STS1               !  trigger rate sector 1-2
          parameter ( STS_STS1 = 1 ) 
          integer STS_STS2               !  trigger rate sector 3-4
          parameter ( STS_STS2 = 2 ) 
          integer STS_STS3               !  trigger rate sector 5-6
          parameter ( STS_STS3 = 3 ) 
          integer STS_STS4               !  
          parameter ( STS_STS4 = 4 ) 
          integer STS_STS5               !  
          parameter ( STS_STS5 = 5 ) 
          integer STS_STS6               !  
          parameter ( STS_STS6 = 6 ) 
          integer STS_STS7               !  
          parameter ( STS_STS7 = 7 ) 
          integer STS_STS8               !  
          parameter ( STS_STS8 = 8 ) 
          integer STS_STS9               !  
          parameter ( STS_STS9 = 9 ) 
          integer STS_STS10               !  
          parameter ( STS_STS10 = 10 ) 
          integer STS_STS11               !  
          parameter ( STS_STS11 = 11 ) 
          integer STS_STS12               !  
          parameter ( STS_STS12 = 12 ) 
          integer STS_STS13               !  
          parameter ( STS_STS13 = 13 ) 
          integer STS_STS14               !  
          parameter ( STS_STS14 = 14 ) 
          integer STS_STS15               !  
          parameter ( STS_STS15 = 15 ) 
          integer STS_STS16               !  
          parameter ( STS_STS16 = 16 ) 

C    Offsets for the SYNC bank follow 

          integer SYNC_ID               !  slot number
          parameter ( SYNC_ID = 1 ) 
          integer SYNC_TDCL               !  count of missing responses
          parameter ( SYNC_TDCL = 2 ) 
          integer SYNC_ADCL               !  count of extra buffers
          parameter ( SYNC_ADCL = 3 ) 

C    Offsets for the TACO bank follow 

          integer TACO_ID               !  ID  1= TAC, 2 = USLG
          parameter ( TACO_ID = 1 ) 
          integer TACO_TIME               !   TDC time 
          parameter ( TACO_TIME = 2 ) 
          integer TACO_ELT               !  energy deposit TAC = left top
          parameter ( TACO_ELT = 3 ) 
          integer TACO_ERT               !  energy deposit TAC = right top
          parameter ( TACO_ERT = 4 ) 
          integer TACO_ELB               !  energy deposit TAC = left bottom
          parameter ( TACO_ELB = 5 ) 
          integer TACO_ERB               !  energy deposit TAC = right bottom
          parameter ( TACO_ERB = 6 ) 
          integer TACO_ESUM               !  energy deposit TAC = sum1
          parameter ( TACO_ESUM = 7 ) 
          integer TACO_ESUM2               !  energy deposit TAC = sum2
          parameter ( TACO_ESUM2 = 8 ) 
          integer TACO_ESUM3               !  energy deposit TAC = sum3
          parameter ( TACO_ESUM3 = 9 ) 
          integer TACO_TID               !  T id of the corresponding T
          parameter ( TACO_TID = 10 ) 

C    Offsets for the TAGE bank follow 

          integer TAGE_ID               !  the address of the hit detector element
          parameter ( TAGE_ID = 1 ) 
          integer TAGE_TDC               !  tdc information (channels)
          parameter ( TAGE_TDC = 2 ) 

C    Offsets for the TAGI bank follow 

          integer TAGI_IDT               !  T id
          parameter ( TAGI_IDT = 1 ) 
          integer TAGI_TIMEL               !  time information (Left counters channels)
          parameter ( TAGI_TIMEL = 2 ) 
          integer TAGI_TIMER               !  time information (Right counters channels)
          parameter ( TAGI_TIMER = 3 ) 
          integer TAGI_IDE               !  E id
          parameter ( TAGI_IDE = 4 ) 
          integer TAGI_TIMEE               !  time information (E counters)
          parameter ( TAGI_TIMEE = 5 ) 
          integer TAGI_TIMEMEAN               !  time information (left/right mean value | after alignement)
          parameter ( TAGI_TIMEMEAN = 6 ) 
          integer TAGI_TRF               !  time information (mean val - RF | after alignement)
          parameter ( TAGI_TRF = 7 ) 
          integer TAGI_NEXTTIME               !  time difference with the next T when in coincidence | after alignment)
          parameter ( TAGI_NEXTTIME = 8 ) 

C    Offsets for the TAGR bank follow 

          integer TAGR_ERG               !  Energy of the photon in GeV
          parameter ( TAGR_ERG = 1 ) 
          integer TAGR_TTAG               !  Time of the photon has reconstructed in the Tagger
          parameter ( TAGR_TTAG = 2 ) 
          integer TAGR_TPHO               !  Time of the photon after RF correction
          parameter ( TAGR_TPHO = 3 ) 
          integer TAGR_STAT               !  Status ( 7 or 15 are Good) other values have problems (see tag_process_TAGR.F) 
          parameter ( TAGR_STAT = 4 ) 
          integer TAGR_T_ID               !  T counter Id
          parameter ( TAGR_T_ID = 5 ) 
          integer TAGR_E_ID               !  E counter Id
          parameter ( TAGR_E_ID = 6 ) 

C    Offsets for the TAGT bank follow 

          integer TAGT_ID               !  the address of the hit detector element
          parameter ( TAGT_ID = 1 ) 
          integer TAGT_TDCL               !  tdc information (Left counters channels)
          parameter ( TAGT_TDCL = 2 ) 
          integer TAGT_TDCR               !  tdc information (Right counters channels)
          parameter ( TAGT_TDCR = 3 ) 

C    Offsets for the TBER bank follow 

          integer TBER_Q_OVER_P               !  q/p
          parameter ( TBER_Q_OVER_P = 1 ) 
          integer TBER_LAMBDA               !  dip angle (pi/2 - theta)
          parameter ( TBER_LAMBDA = 2 ) 
          integer TBER_PHI               !  phi
          parameter ( TBER_PHI = 3 ) 
          integer TBER_D0               !  min.distance from (x=0,y=0,z=?)  [cm]
          parameter ( TBER_D0 = 4 ) 
          integer TBER_Z0               !  z position of starting point     [cm] 
          parameter ( TBER_Z0 = 5 ) 
          integer TBER_C11               !  element C{1,1}
          parameter ( TBER_C11 = 6 ) 
          integer TBER_C12               !  element C{1,2}
          parameter ( TBER_C12 = 7 ) 
          integer TBER_C13               !  element C{1,3}
          parameter ( TBER_C13 = 8 ) 
          integer TBER_C14               !  element C{1,4}
          parameter ( TBER_C14 = 9 ) 
          integer TBER_C15               !  element C{1,5}
          parameter ( TBER_C15 = 10 ) 
          integer TBER_C22               !  element C{2,2}
          parameter ( TBER_C22 = 11 ) 
          integer TBER_C23               !  element C{2,3}
          parameter ( TBER_C23 = 12 ) 
          integer TBER_C24               !  element C{2,4}
          parameter ( TBER_C24 = 13 ) 
          integer TBER_C25               !  element C{2,5}
          parameter ( TBER_C25 = 14 ) 
          integer TBER_C33               !  element C{3,3}
          parameter ( TBER_C33 = 15 ) 
          integer TBER_C34               !  element C{3,4}
          parameter ( TBER_C34 = 16 ) 
          integer TBER_C35               !  element C{3,5}
          parameter ( TBER_C35 = 17 ) 
          integer TBER_C44               !  element C{4,4}
          parameter ( TBER_C44 = 18 ) 
          integer TBER_C45               !  element C{4,5}
          parameter ( TBER_C45 = 19 ) 
          integer TBER_C55               !  element C{5,5}
          parameter ( TBER_C55 = 20 ) 
          integer TBER_CHI2               !  Chisquare for this Track
          parameter ( TBER_CHI2 = 21 ) 
          integer TBER_LAYINFO1               !  layerhit info
          parameter ( TBER_LAYINFO1 = 22 ) 
          integer TBER_LAYINFO2               !  layerhit info&sector&track#in hber
          parameter ( TBER_LAYINFO2 = 23 ) 

C    Offsets for the TBID bank follow 

          integer TBID_TRACK               !  Track (index to TBTR) or zero if neutral
          parameter ( TBID_TRACK = 1 ) 
          integer TBID_SEC               !  Sector track is in
          parameter ( TBID_SEC = 2 ) 
          integer TBID_BETA               !  Beta of the track in units of c
          parameter ( TBID_BETA = 3 ) 
          integer TBID_VTIME               !  vertex time of track
          parameter ( TBID_VTIME = 4 ) 
          integer TBID_SC_STAT               !  status of hit matching to SC: see sc.h
          parameter ( TBID_SC_STAT = 5 ) 
          integer TBID_SC_ID               !  Pointer to SCRC bank
          parameter ( TBID_SC_ID = 6 ) 
          integer TBID_SC_TIME               !  SC calibrated time for this track (ns)
          parameter ( TBID_SC_TIME = 7 ) 
          integer TBID_SC_QUAL               !  quality of match for SC
          parameter ( TBID_SC_QUAL = 8 ) 
          integer TBID_SC_VTIME               !  time at vertex for SC(ns)
          parameter ( TBID_SC_VTIME = 9 ) 
          integer TBID_SC_BETA               !  Beta calculated from TOF from SC
          parameter ( TBID_SC_BETA = 10 ) 
          integer TBID_CC_STAT               !  status of hit matching to CC: see sc.h
          parameter ( TBID_CC_STAT = 11 ) 
          integer TBID_CC_ID               !  pointer to CC01 bank
          parameter ( TBID_CC_ID = 12 ) 
          integer TBID_CC_TIME               !  CC calibrated time for this track (ns)
          parameter ( TBID_CC_TIME = 13 ) 
          integer TBID_CC_QUAL               !  quality of match for CC
          parameter ( TBID_CC_QUAL = 14 ) 
          integer TBID_CC_VTIME               !  time at vertex for CC(ns)
          parameter ( TBID_CC_VTIME = 15 ) 
          integer TBID_CC_BETA               !  Beta as calculated by the EC
          parameter ( TBID_CC_BETA = 16 ) 
          integer TBID_EC_STAT               !  status of hit matching to ec: see sc.h
          parameter ( TBID_EC_STAT = 17 ) 
          integer TBID_EC_ID               !  Pointer to ECHB bank
          parameter ( TBID_EC_ID = 18 ) 
          integer TBID_EC_TIME               !  EC calibrated time for this track (ns)
          parameter ( TBID_EC_TIME = 19 ) 
          integer TBID_EC_QUAL               !  EC quality factor
          parameter ( TBID_EC_QUAL = 20 ) 
          integer TBID_EC_VTIME               !  time at vertex for EC(ns)
          parameter ( TBID_EC_VTIME = 21 ) 
          integer TBID_EC_BETA               !  Beta as calculated by the EC
          parameter ( TBID_EC_BETA = 22 ) 
          integer TBID_ST_STAT               !  status of hit matching to ST
          parameter ( TBID_ST_STAT = 23 ) 
          integer TBID_ST_ID               !  Pointer to STR bank
          parameter ( TBID_ST_ID = 24 ) 
          integer TBID_ST_TIME               !  ST calibrated time for this track (ns)
          parameter ( TBID_ST_TIME = 25 ) 
          integer TBID_ST_QUAL               !  ST quality factor
          parameter ( TBID_ST_QUAL = 26 ) 
          integer TBID_ST_VTIME               !  time at vertex for ST(ns)
          parameter ( TBID_ST_VTIME = 27 ) 
          integer TBID_ST_BETA               !  Beta as calculated by the ST
          parameter ( TBID_ST_BETA = 28 ) 
          integer TBID_LAC_STAT               !  status of hit matching to LAC
          parameter ( TBID_LAC_STAT = 29 ) 
          integer TBID_LAC_ID               !  Pointer to EC1R bank
          parameter ( TBID_LAC_ID = 30 ) 
          integer TBID_LAC_TIME               !  LAC calibrated time for this track (ns)
          parameter ( TBID_LAC_TIME = 31 ) 
          integer TBID_LAC_QUAL               !  LAC quality factor
          parameter ( TBID_LAC_QUAL = 32 ) 
          integer TBID_LAC_VTIME               !  time at vertex for LAC(ns)
          parameter ( TBID_LAC_VTIME = 33 ) 
          integer TBID_LAC_BETA               !  Beta as calculated by the LAC
          parameter ( TBID_LAC_BETA = 34 ) 

C    Offsets for the TBLA bank follow 

          integer TBLA_TRK_PLN               !  (track_number) *100 + Trk_plane_number
          parameter ( TBLA_TRK_PLN = 1 ) 
          integer TBLA_X               !  z coord [cm]  for track in this plane
          parameter ( TBLA_X = 2 ) 
          integer TBLA_Y               !  y coord [cm]  for track in this plane
          parameter ( TBLA_Y = 3 ) 
          integer TBLA_Z               !  z coord [cm]  for track in this plane
          parameter ( TBLA_Z = 4 ) 
          integer TBLA_BX               !  B-field in x [kG] at coord.{x,y,z}
          parameter ( TBLA_BX = 5 ) 
          integer TBLA_BY               !  B-field in y [kG] at coord.{x,y,z}
          parameter ( TBLA_BY = 6 ) 
          integer TBLA_BZ               !  B-field in z [kG] at coord.{x,y,z}
          parameter ( TBLA_BZ = 7 ) 
          integer TBLA_TLEN               !  track length [cm] from origin to this plane
          parameter ( TBLA_TLEN = 8 ) 
          integer TBLA_DC1               !  Pointer to DC1 bank
          parameter ( TBLA_DC1 = 9 ) 
          integer TBLA_STAT               !  Status of the hit
          parameter ( TBLA_STAT = 10 ) 
          integer TBLA_WIRE               !  Wire number 
          parameter ( TBLA_WIRE = 11 ) 
          integer TBLA_DTIME               !  drift time  [ns]
          parameter ( TBLA_DTIME = 12 ) 
          integer TBLA_ALPHA               !  track angle (relative to R of SL) [deg]
          parameter ( TBLA_ALPHA = 13 ) 
          integer TBLA_WLEN               !  Wire length (hit pos. to preamp)  [cm]
          parameter ( TBLA_WLEN = 14 ) 
          integer TBLA_SGDOCA               !  sigma DOCA  [cm]
          parameter ( TBLA_SGDOCA = 15 ) 
          integer TBLA_FITDOCA               !  Fitted DOCA [cm]
          parameter ( TBLA_FITDOCA = 16 ) 
          integer TBLA_CALCDOCA               !  calculated DOCA (via dtime)  [cm]
          parameter ( TBLA_CALCDOCA = 17 ) 

C    Offsets for the TBTR bank follow 

          integer TBTR_X               !  x
          parameter ( TBTR_X = 1 ) 
          integer TBTR_Y               !  y 'Vertex' position {x,y,z}
          parameter ( TBTR_Y = 2 ) 
          integer TBTR_Z               !  z
          parameter ( TBTR_Z = 3 ) 
          integer TBTR_PX               !  Px
          parameter ( TBTR_PX = 4 ) 
          integer TBTR_PY               !  Py  momentum at 'vertex' {Px,Py,Pz}
          parameter ( TBTR_PY = 5 ) 
          integer TBTR_PZ               !  Pz
          parameter ( TBTR_PZ = 6 ) 
          integer TBTR_Q               !  charge   (straight tracks: set q=0.) 
          parameter ( TBTR_Q = 7 ) 
          integer TBTR_CHI2               !  Chisquare for this Track
          parameter ( TBTR_CHI2 = 8 ) 
          integer TBTR_ITR_SEC               !  Trackno_in_Sector + Sector*100
          parameter ( TBTR_ITR_SEC = 9 ) 
          integer TBTR_ITR_HBT               !  Trackno in HBTR for this track
          parameter ( TBTR_ITR_HBT = 10 ) 

C    Offsets for the TCSB bank follow 

          integer TCSB_XPOS               !  x misalignment of TCS in HCS 		 
          parameter ( TCSB_XPOS = 1 ) 
          integer TCSB_YPOS               !  y misalignment of TCS in HCS	 
          parameter ( TCSB_YPOS = 2 ) 
          integer TCSB_ZPOS               !  z misalignment of TCS in HCS	 
          parameter ( TCSB_ZPOS = 3 ) 
          integer TCSB_SXPOS               !  sx sine of x-axis misorientation of TCS vs HCS		 
          parameter ( TCSB_SXPOS = 4 ) 
          integer TCSB_SYPOS               !  sy sine of y-axis misorientation of TCS vs HCS
          parameter ( TCSB_SYPOS = 5 ) 
          integer TCSB_SZPOS               !  sz sine of z-axis misorientation of TCS vs HCS
          parameter ( TCSB_SZPOS = 6 ) 

C    Offsets for the TDPL bank follow 

          integer TDPL_TRK_PLN               !  (track_number) *100 + plane_number
          parameter ( TDPL_TRK_PLN = 1 ) 
          integer TDPL_X               !  vector3_t pos;
          parameter ( TDPL_X = 2 ) 
          integer TDPL_Y               !   (x, y, z coord. for track in on the plane)
          parameter ( TDPL_Y = 3 ) 
          integer TDPL_Z               !  
          parameter ( TDPL_Z = 4 ) 
          integer TDPL_CX               !  vector3_t dir;
          parameter ( TDPL_CX = 5 ) 
          integer TDPL_CY               !   direction cosines (x,y,z) for track at coord.{x,y,z}
          parameter ( TDPL_CY = 6 ) 
          integer TDPL_CZ               !  
          parameter ( TDPL_CZ = 7 ) 
          integer TDPL_TLEN               !  track length [cm] from origin to this plane
          parameter ( TDPL_TLEN = 8 ) 

C    Offsets for the TGBI bank follow 

          integer TGBI_LATCH1               !  level1 trigger latch word (16 bits)
          parameter ( TGBI_LATCH1 = 1 ) 
          integer TGBI_HELICITY_SCALER               !  helicity interval count
          parameter ( TGBI_HELICITY_SCALER = 2 ) 
          integer TGBI_INTERRUPT_TIME               !  interrupt time from microsec clock
          parameter ( TGBI_INTERRUPT_TIME = 3 ) 
          integer TGBI_LATCH2               !  level2 trigger latch word (16 bits)
          parameter ( TGBI_LATCH2 = 4 ) 
          integer TGBI_LEVEL3               !  level3 trigger word (32 bits)
          parameter ( TGBI_LEVEL3 = 5 ) 

C    Offsets for the TGPB bank follow 

          integer TGPB_POINTER               !  pointer to TAGR  
          parameter ( TGPB_POINTER = 1 ) 
          integer TGPB_TIME               !  starttime_TAG at interaction point(ns)
          parameter ( TGPB_TIME = 2 ) 
          integer TGPB_ENERGY               !  photon energy(GeV)
          parameter ( TGPB_ENERGY = 3 ) 
          integer TGPB_DT               !  starttime_ST - starttime_TAG (ns)
          parameter ( TGPB_DT = 4 ) 

C    Offsets for the TGS bank follow 

          integer TGS_RAWT1               !  
          parameter ( TGS_RAWT1 = 1 ) 
          integer TGS_RAWT2               !  
          parameter ( TGS_RAWT2 = 2 ) 
          integer TGS_RAWT3               !  
          parameter ( TGS_RAWT3 = 3 ) 
          integer TGS_RAWT4               !  
          parameter ( TGS_RAWT4 = 4 ) 
          integer TGS_RAWT5               !  
          parameter ( TGS_RAWT5 = 5 ) 
          integer TGS_RAWT6               !  
          parameter ( TGS_RAWT6 = 6 ) 
          integer TGS_RAWT7               !  
          parameter ( TGS_RAWT7 = 7 ) 
          integer TGS_RAWT8               !  
          parameter ( TGS_RAWT8 = 8 ) 
          integer TGS_RAWT9               !  
          parameter ( TGS_RAWT9 = 9 ) 
          integer TGS_RAWT10               !  
          parameter ( TGS_RAWT10 = 10 ) 
          integer TGS_RAWT11               !  
          parameter ( TGS_RAWT11 = 11 ) 
          integer TGS_RAWT12               !  
          parameter ( TGS_RAWT12 = 12 ) 
          integer TGS_RAWT13               !  
          parameter ( TGS_RAWT13 = 13 ) 
          integer TGS_RAWT14               !  
          parameter ( TGS_RAWT14 = 14 ) 
          integer TGS_RAWT15               !  
          parameter ( TGS_RAWT15 = 15 ) 
          integer TGS_RAWT16               !  
          parameter ( TGS_RAWT16 = 16 ) 
          integer TGS_RAWT17               !  
          parameter ( TGS_RAWT17 = 17 ) 
          integer TGS_RAWT18               !  
          parameter ( TGS_RAWT18 = 18 ) 
          integer TGS_RAWT19               !  
          parameter ( TGS_RAWT19 = 19 ) 
          integer TGS_RAWT20               !  
          parameter ( TGS_RAWT20 = 20 ) 
          integer TGS_RAWT21               !  
          parameter ( TGS_RAWT21 = 21 ) 
          integer TGS_RAWT22               !  
          parameter ( TGS_RAWT22 = 22 ) 
          integer TGS_RAWT23               !  
          parameter ( TGS_RAWT23 = 23 ) 
          integer TGS_RAWT24               !  
          parameter ( TGS_RAWT24 = 24 ) 
          integer TGS_RAWT25               !  
          parameter ( TGS_RAWT25 = 25 ) 
          integer TGS_RAWT26               !  
          parameter ( TGS_RAWT26 = 26 ) 
          integer TGS_RAWT27               !  
          parameter ( TGS_RAWT27 = 27 ) 
          integer TGS_RAWT28               !  
          parameter ( TGS_RAWT28 = 28 ) 
          integer TGS_RAWT29               !  
          parameter ( TGS_RAWT29 = 29 ) 
          integer TGS_RAWT30               !  
          parameter ( TGS_RAWT30 = 30 ) 
          integer TGS_RAWT31               !  
          parameter ( TGS_RAWT31 = 31 ) 
          integer TGS_RAWT32               !  
          parameter ( TGS_RAWT32 = 32 ) 
          integer TGS_RAWT33               !  
          parameter ( TGS_RAWT33 = 33 ) 
          integer TGS_RAWT34               !  
          parameter ( TGS_RAWT34 = 34 ) 
          integer TGS_RAWT35               !  
          parameter ( TGS_RAWT35 = 35 ) 
          integer TGS_RAWT36               !  
          parameter ( TGS_RAWT36 = 36 ) 
          integer TGS_RAWT37               !  
          parameter ( TGS_RAWT37 = 37 ) 
          integer TGS_RAWT38               !  
          parameter ( TGS_RAWT38 = 38 ) 
          integer TGS_RAWT39               !  
          parameter ( TGS_RAWT39 = 39 ) 
          integer TGS_RAWT40               !  
          parameter ( TGS_RAWT40 = 40 ) 
          integer TGS_RAWT41               !  
          parameter ( TGS_RAWT41 = 41 ) 
          integer TGS_RAWT42               !  
          parameter ( TGS_RAWT42 = 42 ) 
          integer TGS_RAWT43               !  
          parameter ( TGS_RAWT43 = 43 ) 
          integer TGS_RAWT44               !  
          parameter ( TGS_RAWT44 = 44 ) 
          integer TGS_RAWT45               !  
          parameter ( TGS_RAWT45 = 45 ) 
          integer TGS_RAWT46               !  
          parameter ( TGS_RAWT46 = 46 ) 
          integer TGS_RAWT47               !  
          parameter ( TGS_RAWT47 = 47 ) 
          integer TGS_RAWT48               !  
          parameter ( TGS_RAWT48 = 48 ) 
          integer TGS_RAWT49               !  
          parameter ( TGS_RAWT49 = 49 ) 
          integer TGS_RAWT50               !  
          parameter ( TGS_RAWT50 = 50 ) 
          integer TGS_RAWT51               !  
          parameter ( TGS_RAWT51 = 51 ) 
          integer TGS_RAWT52               !  
          parameter ( TGS_RAWT52 = 52 ) 
          integer TGS_RAWT53               !  
          parameter ( TGS_RAWT53 = 53 ) 
          integer TGS_RAWT54               !  
          parameter ( TGS_RAWT54 = 54 ) 
          integer TGS_RAWT55               !  
          parameter ( TGS_RAWT55 = 55 ) 
          integer TGS_RAWT56               !  
          parameter ( TGS_RAWT56 = 56 ) 
          integer TGS_RAWT57               !  
          parameter ( TGS_RAWT57 = 57 ) 
          integer TGS_RAWT58               !  
          parameter ( TGS_RAWT58 = 58 ) 
          integer TGS_RAWT59               !  
          parameter ( TGS_RAWT59 = 59 ) 
          integer TGS_RAWT60               !  
          parameter ( TGS_RAWT60 = 60 ) 
          integer TGS_RAWT61               !  
          parameter ( TGS_RAWT61 = 61 ) 
          integer TGS_NOTUSED62               !  
          parameter ( TGS_NOTUSED62 = 62 ) 
          integer TGS_NOTUSED63               !  
          parameter ( TGS_NOTUSED63 = 63 ) 
          integer TGS_NOTUSED64               !  
          parameter ( TGS_NOTUSED64 = 64 ) 
          integer TGS_BNK1T1               ! 
          parameter ( TGS_BNK1T1 = 65 ) 
          integer TGS_BNK1T2               ! 
          parameter ( TGS_BNK1T2 = 66 ) 
          integer TGS_BNK1T3               ! 
          parameter ( TGS_BNK1T3 = 67 ) 
          integer TGS_BNK1T4               ! 
          parameter ( TGS_BNK1T4 = 68 ) 
          integer TGS_BNK1T5               ! 
          parameter ( TGS_BNK1T5 = 69 ) 
          integer TGS_BNK1T6               ! 
          parameter ( TGS_BNK1T6 = 70 ) 
          integer TGS_BNK1T7               ! 
          parameter ( TGS_BNK1T7 = 71 ) 
          integer TGS_BNK1T8               ! 
          parameter ( TGS_BNK1T8 = 72 ) 
          integer TGS_BNK1T9               ! 
          parameter ( TGS_BNK1T9 = 73 ) 
          integer TGS_BNK1T10               ! 
          parameter ( TGS_BNK1T10 = 74 ) 
          integer TGS_BNK1T11               ! 
          parameter ( TGS_BNK1T11 = 75 ) 
          integer TGS_BNK1T12               ! 
          parameter ( TGS_BNK1T12 = 76 ) 
          integer TGS_BNK1T13               ! 
          parameter ( TGS_BNK1T13 = 77 ) 
          integer TGS_BNK1T14               ! 
          parameter ( TGS_BNK1T14 = 78 ) 
          integer TGS_BNK1T15               ! 
          parameter ( TGS_BNK1T15 = 79 ) 
          integer TGS_BNK1T16               ! 
          parameter ( TGS_BNK1T16 = 80 ) 
          integer TGS_BNK1T17               ! 
          parameter ( TGS_BNK1T17 = 81 ) 
          integer TGS_BNK1T18               ! 
          parameter ( TGS_BNK1T18 = 82 ) 
          integer TGS_BNK1T19               ! 
          parameter ( TGS_BNK1T19 = 83 ) 
          integer TGS_BNK1T20               ! 
          parameter ( TGS_BNK1T20 = 84 ) 
          integer TGS_BNK1T21               ! 
          parameter ( TGS_BNK1T21 = 85 ) 
          integer TGS_BNK1T22               ! 
          parameter ( TGS_BNK1T22 = 86 ) 
          integer TGS_BNK1T23               ! 
          parameter ( TGS_BNK1T23 = 87 ) 
          integer TGS_BNK1T24               ! 
          parameter ( TGS_BNK1T24 = 88 ) 
          integer TGS_BNK1T25               ! 
          parameter ( TGS_BNK1T25 = 89 ) 
          integer TGS_BNK1T26               ! 
          parameter ( TGS_BNK1T26 = 90 ) 
          integer TGS_BNK1T27               ! 
          parameter ( TGS_BNK1T27 = 91 ) 
          integer TGS_BNK1T28               ! 
          parameter ( TGS_BNK1T28 = 92 ) 
          integer TGS_BNK1T29               ! 
          parameter ( TGS_BNK1T29 = 93 ) 
          integer TGS_BNK1T30               ! 
          parameter ( TGS_BNK1T30 = 94 ) 
          integer TGS_BNK1T31               ! 
          parameter ( TGS_BNK1T31 = 95 ) 
          integer TGS_BNK1T32               ! 
          parameter ( TGS_BNK1T32 = 96 ) 
          integer TGS_BNK1T33               ! 
          parameter ( TGS_BNK1T33 = 97 ) 
          integer TGS_BNK1T34               ! 
          parameter ( TGS_BNK1T34 = 98 ) 
          integer TGS_BNK1T35               ! 
          parameter ( TGS_BNK1T35 = 99 ) 
          integer TGS_BNK1T36               ! 
          parameter ( TGS_BNK1T36 = 100 ) 
          integer TGS_BNK1T37               ! 
          parameter ( TGS_BNK1T37 = 101 ) 
          integer TGS_BNK1T38               ! 
          parameter ( TGS_BNK1T38 = 102 ) 
          integer TGS_BNK1T39               ! 
          parameter ( TGS_BNK1T39 = 103 ) 
          integer TGS_BNK1T40               ! 
          parameter ( TGS_BNK1T40 = 104 ) 
          integer TGS_BNK1T41               ! 
          parameter ( TGS_BNK1T41 = 105 ) 
          integer TGS_BNK1T42               ! 
          parameter ( TGS_BNK1T42 = 106 ) 
          integer TGS_BNK1T43               ! 
          parameter ( TGS_BNK1T43 = 107 ) 
          integer TGS_BNK1T44               ! 
          parameter ( TGS_BNK1T44 = 108 ) 
          integer TGS_BNK1T45               ! 
          parameter ( TGS_BNK1T45 = 109 ) 
          integer TGS_BNK1T46               ! 
          parameter ( TGS_BNK1T46 = 110 ) 
          integer TGS_BNK1T47               ! 
          parameter ( TGS_BNK1T47 = 111 ) 
          integer TGS_BNK1T48               ! 
          parameter ( TGS_BNK1T48 = 112 ) 
          integer TGS_BNK1T49               ! 
          parameter ( TGS_BNK1T49 = 113 ) 
          integer TGS_BNK1T50               ! 
          parameter ( TGS_BNK1T50 = 114 ) 
          integer TGS_BNK1T51               ! 
          parameter ( TGS_BNK1T51 = 115 ) 
          integer TGS_BNK1T52               ! 
          parameter ( TGS_BNK1T52 = 116 ) 
          integer TGS_BNK1T53               ! 
          parameter ( TGS_BNK1T53 = 117 ) 
          integer TGS_BNK1T54               ! 
          parameter ( TGS_BNK1T54 = 118 ) 
          integer TGS_BNK1T55               ! 
          parameter ( TGS_BNK1T55 = 119 ) 
          integer TGS_BNK1T56               ! 
          parameter ( TGS_BNK1T56 = 120 ) 
          integer TGS_BNK1T57               ! 
          parameter ( TGS_BNK1T57 = 121 ) 
          integer TGS_BNK1T58               ! 
          parameter ( TGS_BNK1T58 = 122 ) 
          integer TGS_BNK1T59               ! 
          parameter ( TGS_BNK1T59 = 123 ) 
          integer TGS_BNK1T60               ! 
          parameter ( TGS_BNK1T60 = 124 ) 
          integer TGS_BNK1T61               ! 
          parameter ( TGS_BNK1T61 = 125 ) 
          integer TGS_NOTUSED126               ! 
          parameter ( TGS_NOTUSED126 = 126 ) 
          integer TGS_NOTUSED127               ! 
          parameter ( TGS_NOTUSED127 = 127 ) 
          integer TGS_NOTUSED128               ! 
          parameter ( TGS_NOTUSED128 = 128 ) 
          integer TGS_BNK2T1               ! 
          parameter ( TGS_BNK2T1 = 129 ) 
          integer TGS_BNK2T2               ! 
          parameter ( TGS_BNK2T2 = 130 ) 
          integer TGS_BNK2T3               ! 
          parameter ( TGS_BNK2T3 = 131 ) 
          integer TGS_BNK2T4               ! 
          parameter ( TGS_BNK2T4 = 132 ) 
          integer TGS_BNK2T5               ! 
          parameter ( TGS_BNK2T5 = 133 ) 
          integer TGS_BNK2T6               ! 
          parameter ( TGS_BNK2T6 = 134 ) 
          integer TGS_BNK2T7               ! 
          parameter ( TGS_BNK2T7 = 135 ) 
          integer TGS_BNK2T8               ! 
          parameter ( TGS_BNK2T8 = 136 ) 
          integer TGS_BNK2T9               ! 
          parameter ( TGS_BNK2T9 = 137 ) 
          integer TGS_BNK2T10               ! 
          parameter ( TGS_BNK2T10 = 138 ) 
          integer TGS_BNK2T11               ! 
          parameter ( TGS_BNK2T11 = 139 ) 
          integer TGS_BNK2T12               ! 
          parameter ( TGS_BNK2T12 = 140 ) 
          integer TGS_BNK2T13               ! 
          parameter ( TGS_BNK2T13 = 141 ) 
          integer TGS_BNK2T14               ! 
          parameter ( TGS_BNK2T14 = 142 ) 
          integer TGS_BNK2T15               ! 
          parameter ( TGS_BNK2T15 = 143 ) 
          integer TGS_BNK2T16               ! 
          parameter ( TGS_BNK2T16 = 144 ) 
          integer TGS_BNK2T17               ! 
          parameter ( TGS_BNK2T17 = 145 ) 
          integer TGS_BNK2T18               ! 
          parameter ( TGS_BNK2T18 = 146 ) 
          integer TGS_BNK2T19               ! 
          parameter ( TGS_BNK2T19 = 147 ) 
          integer TGS_BNK2T20               ! 
          parameter ( TGS_BNK2T20 = 148 ) 
          integer TGS_BNK2T21               ! 
          parameter ( TGS_BNK2T21 = 149 ) 
          integer TGS_BNK2T22               ! 
          parameter ( TGS_BNK2T22 = 150 ) 
          integer TGS_BNK2T23               ! 
          parameter ( TGS_BNK2T23 = 151 ) 
          integer TGS_BNK2T24               ! 
          parameter ( TGS_BNK2T24 = 152 ) 
          integer TGS_BNK2T25               ! 
          parameter ( TGS_BNK2T25 = 153 ) 
          integer TGS_BNK2T26               ! 
          parameter ( TGS_BNK2T26 = 154 ) 
          integer TGS_BNK2T27               ! 
          parameter ( TGS_BNK2T27 = 155 ) 
          integer TGS_BNK2T28               ! 
          parameter ( TGS_BNK2T28 = 156 ) 
          integer TGS_BNK2T29               ! 
          parameter ( TGS_BNK2T29 = 157 ) 
          integer TGS_BNK2T30               ! 
          parameter ( TGS_BNK2T30 = 158 ) 
          integer TGS_BNK2T31               ! 
          parameter ( TGS_BNK2T31 = 159 ) 
          integer TGS_BNK2T32               ! 
          parameter ( TGS_BNK2T32 = 160 ) 
          integer TGS_BNK2T33               ! 
          parameter ( TGS_BNK2T33 = 161 ) 
          integer TGS_BNK2T34               ! 
          parameter ( TGS_BNK2T34 = 162 ) 
          integer TGS_BNK2T35               ! 
          parameter ( TGS_BNK2T35 = 163 ) 
          integer TGS_BNK2T36               ! 
          parameter ( TGS_BNK2T36 = 164 ) 
          integer TGS_BNK2T37               ! 
          parameter ( TGS_BNK2T37 = 165 ) 
          integer TGS_BNK2T38               ! 
          parameter ( TGS_BNK2T38 = 166 ) 
          integer TGS_BNK2T39               ! 
          parameter ( TGS_BNK2T39 = 167 ) 
          integer TGS_BNK2T40               ! 
          parameter ( TGS_BNK2T40 = 168 ) 
          integer TGS_BNK2T41               ! 
          parameter ( TGS_BNK2T41 = 169 ) 
          integer TGS_BNK2T42               ! 
          parameter ( TGS_BNK2T42 = 170 ) 
          integer TGS_BNK2T43               ! 
          parameter ( TGS_BNK2T43 = 171 ) 
          integer TGS_BNK2T44               ! 
          parameter ( TGS_BNK2T44 = 172 ) 
          integer TGS_BNK2T45               ! 
          parameter ( TGS_BNK2T45 = 173 ) 
          integer TGS_BNK2T46               ! 
          parameter ( TGS_BNK2T46 = 174 ) 
          integer TGS_BNK2T47               ! 
          parameter ( TGS_BNK2T47 = 175 ) 
          integer TGS_BNK2T48               ! 
          parameter ( TGS_BNK2T48 = 176 ) 
          integer TGS_BNK2T49               ! 
          parameter ( TGS_BNK2T49 = 177 ) 
          integer TGS_BNK2T50               ! 
          parameter ( TGS_BNK2T50 = 178 ) 
          integer TGS_BNK2T51               ! 
          parameter ( TGS_BNK2T51 = 179 ) 
          integer TGS_BNK2T52               ! 
          parameter ( TGS_BNK2T52 = 180 ) 
          integer TGS_BNK2T53               ! 
          parameter ( TGS_BNK2T53 = 181 ) 
          integer TGS_BNK2T54               ! 
          parameter ( TGS_BNK2T54 = 182 ) 
          integer TGS_BNK2T55               ! 
          parameter ( TGS_BNK2T55 = 183 ) 
          integer TGS_BNK2T56               ! 
          parameter ( TGS_BNK2T56 = 184 ) 
          integer TGS_BNK2T57               ! 
          parameter ( TGS_BNK2T57 = 185 ) 
          integer TGS_BNK2T58               ! 
          parameter ( TGS_BNK2T58 = 186 ) 
          integer TGS_BNK2T59               ! 
          parameter ( TGS_BNK2T59 = 187 ) 
          integer TGS_BNK2T60               ! 
          parameter ( TGS_BNK2T60 = 188 ) 
          integer TGS_BNK2T61               ! 
          parameter ( TGS_BNK2T61 = 189 ) 
          integer TGS_NOTUSED190               ! 
          parameter ( TGS_NOTUSED190 = 190 ) 
          integer TGS_NOTUSED191               ! 
          parameter ( TGS_NOTUSED191 = 191 ) 
          integer TGS_NOTUSED192               ! 
          parameter ( TGS_NOTUSED192 = 192 ) 

C    Offsets for the TLV1 bank follow 

          integer TLV1_ID               !  id = Detector * 256 + channel
          parameter ( TLV1_ID = 1 ) 
          integer TLV1_TIME               !  time(ns) 
          parameter ( TLV1_TIME = 2 ) 

C    Offsets for the TRGS bank follow 

          integer TRGS_CLOCK_UG               !  Ungated Clock
          parameter ( TRGS_CLOCK_UG = 1 ) 
          integer TRGS_FCUP_UG               !  FCUP UNGATED
          parameter ( TRGS_FCUP_UG = 2 ) 
          integer TRGS_MICROSEC               !  microsecond clock (will overflow during normal run)
          parameter ( TRGS_MICROSEC = 3 ) 
          integer TRGS_HEL_SYNC               !  Helicity sync signal
          parameter ( TRGS_HEL_SYNC = 4 ) 
          integer TRGS_MOR_ST               !  MOR.ST
          parameter ( TRGS_MOR_ST = 5 ) 
          integer TRGS_MOR_PC               !  MOR.PC
          parameter ( TRGS_MOR_PC = 6 ) 
          integer TRGS_MOR_PS               !  MOR.PS
          parameter ( TRGS_MOR_PS = 7 ) 
          integer TRGS_MOR_TAC               !  MOR.TAC
          parameter ( TRGS_MOR_TAC = 8 ) 
          integer TRGS_MOR               !  Master OR
          parameter ( TRGS_MOR = 9 ) 
          integer TRGS_PC               !  Pair Counter
          parameter ( TRGS_PC = 10 ) 
          integer TRGS_PS               !  Pair Spectrometer
          parameter ( TRGS_PS = 11 ) 
          integer TRGS_TAC               !  Total Absorption Counter
          parameter ( TRGS_TAC = 12 ) 
          integer TRGS_ST               !  ST
          parameter ( TRGS_ST = 13 ) 
          integer TRGS_NOTUSED14               !  Currently not used
          parameter ( TRGS_NOTUSED14 = 14 ) 
          integer TRGS_CLOCK_UG_2               !  duplicate of channel 1
          parameter ( TRGS_CLOCK_UG_2 = 15 ) 
          integer TRGS_FCUP_UG_2               !  duplicate of channel 2
          parameter ( TRGS_FCUP_UG_2 = 16 ) 
          integer TRGS_CLOCK_G1               !  Clock with run gatei
          parameter ( TRGS_CLOCK_G1 = 17 ) 
          integer TRGS_FCUP_G1               !  FCUP with Run Gate
          parameter ( TRGS_FCUP_G1 = 18 ) 
          integer TRGS_NOTUSED19               !  Currently not used
          parameter ( TRGS_NOTUSED19 = 19 ) 
          integer TRGS_NOTUSED20               !  Currently not used
          parameter ( TRGS_NOTUSED20 = 20 ) 
          integer TRGS_MOR_ST_RG               !  MOR.ST  with run gate
          parameter ( TRGS_MOR_ST_RG = 21 ) 
          integer TRGS_MOR_PC_RG               !  MOR.PC with run gate
          parameter ( TRGS_MOR_PC_RG = 22 ) 
          integer TRGS_MOR_PS_RG               !  MOR.PS with run gate
          parameter ( TRGS_MOR_PS_RG = 23 ) 
          integer TRGS_MOR_TAC_RG               !  MOR.TAC with run gate
          parameter ( TRGS_MOR_TAC_RG = 24 ) 
          integer TRGS_MOR_RG               !  MASTER_OR with run gate
          parameter ( TRGS_MOR_RG = 25 ) 
          integer TRGS_PC_RG               !  PC with run gate
          parameter ( TRGS_PC_RG = 26 ) 
          integer TRGS_PS_RG               !  PS with run gate
          parameter ( TRGS_PS_RG = 27 ) 
          integer TRGS_TAC_RG               !  TAC with run gate
          parameter ( TRGS_TAC_RG = 28 ) 
          integer TRGS_ST_RG               !  ST  with run gate
          parameter ( TRGS_ST_RG = 29 ) 
          integer TRGS_NOTUSED30               !  Currently not used
          parameter ( TRGS_NOTUSED30 = 30 ) 
          integer TRGS_CLOCK_G1_2               !  duplicate of channel 17
          parameter ( TRGS_CLOCK_G1_2 = 31 ) 
          integer TRGS_FCUP_G1_2               !  duplicate of channel 18
          parameter ( TRGS_FCUP_G1_2 = 32 ) 
          integer TRGS_CLOCK_G2               !  CLOCK with Live gate
          parameter ( TRGS_CLOCK_G2 = 33 ) 
          integer TRGS_FCUP_G2               !  FCUP with Live gate
          parameter ( TRGS_FCUP_G2 = 34 ) 
          integer TRGS_TRIG_OR_G2               !  Trigger OR of 1-12 with Live gate
          parameter ( TRGS_TRIG_OR_G2 = 35 ) 
          integer TRGS_NOTUSED36               !  Currently not used
          parameter ( TRGS_NOTUSED36 = 36 ) 
          integer TRGS_NOTUSED37               !  Currently not used
          parameter ( TRGS_NOTUSED37 = 37 ) 
          integer TRGS_NOTUSED38               !  Currently not used
          parameter ( TRGS_NOTUSED38 = 38 ) 
          integer TRGS_NOTUSED39               !  Currently not used
          parameter ( TRGS_NOTUSED39 = 39 ) 
          integer TRGS_NOTUSED40               !  Currently not used
          parameter ( TRGS_NOTUSED40 = 40 ) 
          integer TRGS_MOR_LG               !  MASTER_OR with Live gate
          parameter ( TRGS_MOR_LG = 41 ) 
          integer TRGS_NOTUSED42               !  Currently not used
          parameter ( TRGS_NOTUSED42 = 42 ) 
          integer TRGS_NOTUSED43               !  Currently not used
          parameter ( TRGS_NOTUSED43 = 43 ) 
          integer TRGS_NOTUSED44               !  Currently not used
          parameter ( TRGS_NOTUSED44 = 44 ) 
          integer TRGS_NOTUSED45               !  Currently not used
          parameter ( TRGS_NOTUSED45 = 45 ) 
          integer TRGS_NOTUSED46               !  Currently not used
          parameter ( TRGS_NOTUSED46 = 46 ) 
          integer TRGS_CLOCK_G2_2               !  duplicate of channel 33
          parameter ( TRGS_CLOCK_G2_2 = 47 ) 
          integer TRGS_FCUP_G2_2               !  duplicate of channel 34
          parameter ( TRGS_FCUP_G2_2 = 48 ) 
          integer TRGS_TRIG1_UG               !  Trigger 1 ungated, prescaled
          parameter ( TRGS_TRIG1_UG = 49 ) 
          integer TRGS_TRIG2_UG               !  Trigger 2 ungated, prescaled
          parameter ( TRGS_TRIG2_UG = 50 ) 
          integer TRGS_TRIG3_UG               !  Trigger 3 ungated, prescaled
          parameter ( TRGS_TRIG3_UG = 51 ) 
          integer TRGS_TRIG4_UG               !  Trigger 4 ungated, prescaled
          parameter ( TRGS_TRIG4_UG = 52 ) 
          integer TRGS_TRIG5_UG               !  Trigger 5 ungated, prescaled
          parameter ( TRGS_TRIG5_UG = 53 ) 
          integer TRGS_TRIG6_UG               !  Trigger 6 ungated, prescaled
          parameter ( TRGS_TRIG6_UG = 54 ) 
          integer TRGS_TRIG7_UG               !  Trigger 7 ungated, prescaled
          parameter ( TRGS_TRIG7_UG = 55 ) 
          integer TRGS_TRIG8_UG               !  Trigger 8 ungated, prescaled
          parameter ( TRGS_TRIG8_UG = 56 ) 
          integer TRGS_TRIG9_UG               !  Trigger 9 ungated, prescaled
          parameter ( TRGS_TRIG9_UG = 57 ) 
          integer TRGS_TRIG10_UG               !  Trigger 10 ungated, prescaled
          parameter ( TRGS_TRIG10_UG = 58 ) 
          integer TRGS_TRIG11_UG               !  Trigger 11 ungated, prescaled
          parameter ( TRGS_TRIG11_UG = 59 ) 
          integer TRGS_TRIG12_UG               !  Trigger 12 ungated, prescaled
          parameter ( TRGS_TRIG12_UG = 60 ) 
          integer TRGS_TRIG_OR_UG               !  Trigger OR of 1-12 ungated,
          parameter ( TRGS_TRIG_OR_UG = 61 ) 
          integer TRGS_L1ACCEPT               !  Level 1 accept
          parameter ( TRGS_L1ACCEPT = 62 ) 
          integer TRGS_NOTUSED63               !  Currently not used
          parameter ( TRGS_NOTUSED63 = 63 ) 
          integer TRGS_NOTUSED64               !  Currently not used
          parameter ( TRGS_NOTUSED64 = 64 ) 
          integer TRGS_L2FAIL               !  Level2 fail
          parameter ( TRGS_L2FAIL = 65 ) 
          integer TRGS_L2PASS               !  Level2 pass
          parameter ( TRGS_L2PASS = 66 ) 
          integer TRGS_L2START               !  Level2 start
          parameter ( TRGS_L2START = 67 ) 
          integer TRGS_L2CLEAR               !  Level2 clear
          parameter ( TRGS_L2CLEAR = 68 ) 
          integer TRGS_L2ACCEPT               !  Level2 accept
          parameter ( TRGS_L2ACCEPT = 69 ) 
          integer TRGS_L3ACCEPT               !  Level3 accept
          parameter ( TRGS_L3ACCEPT = 70 ) 
          integer TRGS_NOTUSED71               !  Currently not used
          parameter ( TRGS_NOTUSED71 = 71 ) 
          integer TRGS_NOTUSED72               !  Currently not used
          parameter ( TRGS_NOTUSED72 = 72 ) 
          integer TRGS_L2SEC1_G               !  Level2 sec1 gated
          parameter ( TRGS_L2SEC1_G = 73 ) 
          integer TRGS_L2SEC2_G               !  Level2 sec2 gated
          parameter ( TRGS_L2SEC2_G = 74 ) 
          integer TRGS_L2SEC3_G               !  Level2 sec3 gated
          parameter ( TRGS_L2SEC3_G = 75 ) 
          integer TRGS_L2SEC4_G               !  Level2 sec4 gated
          parameter ( TRGS_L2SEC4_G = 76 ) 
          integer TRGS_L2SEC5_G               !  Level2 sec5 gated
          parameter ( TRGS_L2SEC5_G = 77 ) 
          integer TRGS_L2SEC6_G               !  Level2 sec6 gated
          parameter ( TRGS_L2SEC6_G = 78 ) 
          integer TRGS_L2_OR_G               !  OR level2 gated
          parameter ( TRGS_L2_OR_G = 79 ) 
          integer TRGS_L2_OK_G               !  Level 2 OK gated
          parameter ( TRGS_L2_OK_G = 80 ) 

C    Offsets for the TRKS bank follow 

          integer TRKS_TRK_LEV               !  Tracknr + analysis_level*100
          parameter ( TRKS_TRK_LEV = 1 ) 
          integer TRKS_BETA               !  Beta of the track in unit of c
          parameter ( TRKS_BETA = 2 ) 
          integer TRKS_ST_TIME               !  Start time of the event (track: photon trigg)
          parameter ( TRKS_ST_TIME = 3 ) 
          integer TRKS_CC_TIME               !  CC calibrated time for this track (ns)
          parameter ( TRKS_CC_TIME = 4 ) 
          integer TRKS_SC_TIME               !  SC calibrated time for this track (ns)
          parameter ( TRKS_SC_TIME = 5 ) 
          integer TRKS_EC_TIME               !  EC calibrated time for this track (ns)
          parameter ( TRKS_EC_TIME = 6 ) 
          integer TRKS_ST_ID               !  ST hit id (ptr. to STx: photon trigg)
          parameter ( TRKS_ST_ID = 7 ) 
          integer TRKS_CC_ID               !  CC hit id (ptr. to CCRC)
          parameter ( TRKS_CC_ID = 8 ) 
          integer TRKS_SC_ID               !  SC hit id (ptr. to SCRW)
          parameter ( TRKS_SC_ID = 9 ) 
          integer TRKS_EC_ID               !  EC Cluster-id (ptr. to ECHB)
          parameter ( TRKS_EC_ID = 10 ) 

C    Offsets for the TRL1 bank follow 

          integer TRL1_FIT_FLAGS               !  flags used in track fitting
          parameter ( TRL1_FIT_FLAGS = 1 ) 
          integer TRL1_HITS_HBT               !  hits used in HBT fit
          parameter ( TRL1_HITS_HBT = 2 ) 
          integer TRL1_HITS_TBT               !  hits used in TBT fit
          parameter ( TRL1_HITS_TBT = 3 ) 
          integer TRL1_X               !  x
          parameter ( TRL1_X = 4 ) 
          integer TRL1_Y               !  y   position in first DC layer 
          parameter ( TRL1_Y = 5 ) 
          integer TRL1_Z               !  z
          parameter ( TRL1_Z = 6 ) 
          integer TRL1_CX               !  cx
          parameter ( TRL1_CX = 7 ) 
          integer TRL1_CY               !  cy  dir.cosine at first DC layer
          parameter ( TRL1_CY = 8 ) 
          integer TRL1_CZ               !  cz
          parameter ( TRL1_CZ = 9 ) 
          integer TRL1_TLEN               !  track length to this layer (starting
          parameter ( TRL1_TLEN = 10 ) 

C    Offsets for the TRPB bank follow 

          integer TRPB_HITS_TBT               !  hits used in TBT fit
          parameter ( TRPB_HITS_TBT = 1 ) 
          integer TRPB_X               !  x
          parameter ( TRPB_X = 2 ) 
          integer TRPB_Y               !  y   position in first DC layer 
          parameter ( TRPB_Y = 3 ) 
          integer TRPB_Z               !  z
          parameter ( TRPB_Z = 4 ) 
          integer TRPB_CX               !  cx
          parameter ( TRPB_CX = 5 ) 
          integer TRPB_CY               !  cy  dir.cosine at first DC layer
          parameter ( TRPB_CY = 6 ) 
          integer TRPB_CZ               !  cz
          parameter ( TRPB_CZ = 7 ) 
          integer TRPB_TLEN               !  track length to this layer (starting
          parameter ( TRPB_TLEN = 8 ) 

C    Offsets for the TSPR bank follow 

          integer TSPR_MEM               !  memory value
          parameter ( TSPR_MEM = 1 ) 

C    Offsets for the TSRG bank follow 

          integer TSRG_REG               !  register value
          parameter ( TSRG_REG = 1 ) 

C    Offsets for the UNUS bank follow 

          integer UNUS_NDCUN               !  Number of unused DC tracks
          parameter ( UNUS_NDCUN = 1 ) 
          integer UNUS_IDCUN               !  Trek ID  
          parameter ( UNUS_IDCUN = 2 ) 
          integer UNUS_NSCUN               !  Number of unused SC hits
          parameter ( UNUS_NSCUN = 1 ) 
          integer UNUS_ISCUN               !  SC hit ID
          parameter ( UNUS_ISCUN = 2 ) 
          integer UNUS_NCCUN               !  Number of unused CC hits
          parameter ( UNUS_NCCUN = 1 ) 
          integer UNUS_ICCUN               !  CC hit ID
          parameter ( UNUS_ICCUN = 2 ) 
          integer UNUS_NECUN               !  Number of unused EC hits
          parameter ( UNUS_NECUN = 1 ) 
          integer UNUS_IECUN               !  EC hit ID
          parameter ( UNUS_IECUN = 2 ) 

C    Offsets for the VERT bank follow 

          integer VERT_VERTEX               !  vertex id
          parameter ( VERT_VERTEX = 1 ) 
          integer VERT_TRK1               !  track #1
          parameter ( VERT_TRK1 = 2 ) 
          integer VERT_TRK2               !  track #2
          parameter ( VERT_TRK2 = 3 ) 
          integer VERT_X               !  x vector3_t vert{x,y,z}
          parameter ( VERT_X = 4 ) 
          integer VERT_Y               !  y 
          parameter ( VERT_Y = 5 ) 
          integer VERT_Z               !  z
          parameter ( VERT_Z = 6 ) 
          integer VERT_SEPD               !  seperation distance
          parameter ( VERT_SEPD = 7 ) 
