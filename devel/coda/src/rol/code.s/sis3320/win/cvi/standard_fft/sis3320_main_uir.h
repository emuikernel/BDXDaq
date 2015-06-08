/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2005. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  DEU_GRAPH                       1
#define  DEU_GRAPH_TIMEGRAPH             2
#define  DEU_GRAPH_FREQGRAPH             3
#define  DEU_GRAPH_PEAK_AMPL             4
#define  DEU_GRAPH_PEAK_FREQ             5
#define  DEU_GRAPH_GET_AUDIO_CMC_NO      6       /* callback function: CB_Deu_Audio_CmcChNumber */
#define  DEU_GRAPH_GET_AUDIO_CH_NO       7       /* callback function: CB_Deu_Audio_CmcChNumber */

#define  DEU_MENU                        2
#define  DEU_MENU_READLOOP               2       /* callback function: CB_DeuTopMenu_read_loop */
#define  DEU_MENU_LOOP_TICK              3       /* callback function: CB_ReadLoopDeuTopTimer */
#define  DEU_MENU_GET_OUT_HIGHSPEED_7    4       /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_6    5       /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_5    6       /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_4    7       /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_SPY_AUDIO          8       /* callback function: CB_Deu_Write_SpyAudio */
#define  DEU_MENU_CHECKBOX_DISPL_GRAPH   9       /* callback function: CB_DeuGraphCheckBox */
#define  DEU_MENU_GET_OUT_HIGHSPEED_3    10      /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_2    11      /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_1    12      /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_HIGHSPEED_0    13      /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_DISCRETE       14      /* callback function: CB_Deu_Write_DiscreteHighSpeed */
#define  DEU_MENU_GET_OUT_PORT_NO        15      /* callback function: CB_Deu_Write_DeuPortNumber */
#define  DEU_MENU_GET_IN_PORT_NO         16      /* callback function: CB_Deu_Read_DeuPortNumber */
#define  DEU_MENU_GET_OUT_CMC_NO         17      /* callback function: CB_Deu_Write_DeuPortNumber */
#define  DEU_MENU_GET_OUT_DEU_NO         18      /* callback function: CB_Deu_Write_DeuPortNumber */
#define  DEU_MENU_GET_IN_CMC_NO          19      /* callback function: CB_Deu_Read_DeuPortNumber */
#define  DEU_MENU_GET_IN_DEU_NO          20      /* callback function: CB_Deu_Read_DeuPortNumber */
#define  DEU_MENU_CLOSE                  21      /* callback function: CB_ClosePanelDeuTop */
#define  DEU_MENU_CURSORMSG_53           22
#define  DEU_MENU_CURSORMSG_68           23
#define  DEU_MENU_CURSORMSG_49           24
#define  DEU_MENU_CURSORMSG_56           25
#define  DEU_MENU_CURSORMSG_57           26
#define  DEU_MENU_DECORATION_14          27
#define  DEU_MENU_DECORATION_9           28
#define  DEU_MENU_DECORATION_10          29
#define  DEU_MENU_DECORATION_12          30
#define  DEU_MENU_DECORATION_8           31
#define  DEU_MENU_DECORATION_13          32
#define  DEU_MENU_DECORATION_7           33
#define  DEU_MENU_CURSORMSG_58           34
#define  DEU_MENU_CHECKBOX_TRANS_ENABLE  35      /* callback function: CB_CheckBoxDeuTransmit */
#define  DEU_MENU_CURSORMSG_59           36
#define  DEU_MENU_CURSORMSG_60           37
#define  DEU_MENU_CURSORMSG_61           38
#define  DEU_MENU_CURSORMSG_69           39
#define  DEU_MENU_CURSORMSG_70           40
#define  DEU_MENU_CURSORMSG_71           41
#define  DEU_MENU_CURSORMSG_72           42
#define  DEU_MENU_CURSORMSG_73           43
#define  DEU_MENU_CURSORMSG_62           44
#define  DEU_MENU_CURSORMSG_63           45
#define  DEU_MENU_CURSORMSG_66           46
#define  DEU_MENU_CURSORMSG_64           47
#define  DEU_MENU_CURSORMSG_50           48
#define  DEU_MENU_CURSORMSG_65           49
#define  DEU_MENU_CURSORMSG_48           50
#define  DEU_MENU_CURSORMSG_52           51
#define  DEU_MENU_CURSORMSG_33           52
#define  DEU_MENU_CURSORMSG_45           53
#define  DEU_MENU_CURSORMSG_47           54
#define  DEU_MENU_FRAME_SYNCH_ERROR_CNT  55
#define  DEU_MENU_SIGNAL_LOST_ERROR_CNT  56
#define  DEU_MENU_CRC_ERROR_CNT          57
#define  DEU_MENU_CURSORMSG_74           58
#define  DEU_MENU_HEADER_LOST_CNT        59
#define  DEU_MENU_FRAME_TIMEOUT_CNT      60
#define  DEU_MENU_FRAME_DEU_ERROR_CNT_B  61
#define  DEU_MENU_FRAME_DEU_ERROR_CNT_A  62
#define  DEU_MENU_SOFT_VERSION           63
#define  DEU_MENU_HIGHSPEED_DIR_IN_1B    64
#define  DEU_MENU_HIGHSPEED_DIR_IN_0B    65
#define  DEU_MENU_FRAME_DIR_ERROR_CNT_B  66
#define  DEU_MENU_DISCRETE_DIR_IN_1B     67
#define  DEU_MENU_SUPERFRAME_CNT_B       68
#define  DEU_MENU_FRAME_DIR_ERROR_CNT_A  69
#define  DEU_MENU_DISCRETE_DIR_IN_0B     70
#define  DEU_MENU_SUPERFRAME_CNT_A       71
#define  DEU_MENU_CURSORMSG_75           72
#define  DEU_MENU_CURSORMSG_67           73
#define  DEU_MENU_CURSORMSG_44           74
#define  DEU_MENU_CURSORMSG_46           75
#define  DEU_MENU_CURSORMSG_51           76
#define  DEU_MENU_CURSORMSG_31           77
#define  DEU_MENU_LOOP_TIME              78      /* callback function: CB_DeuTopLoopTime */
#define  DEU_MENU_HIGHSPEED_DIR_IN_1A    79
#define  DEU_MENU_HIGHSPEED_DIR_IN_0A    80
#define  DEU_MENU_DISCRETE_DIR_IN_1A     81
#define  DEU_MENU_DISCRETE_DEU_IN        82
#define  DEU_MENU_ADDR_DIR_IN_B          83
#define  DEU_MENU_DISCRETE_DIR_IN_0A     84
#define  DEU_MENU_HIGHSPEED_DEU_IN_7     85
#define  DEU_MENU_HIGHSPEED_DEU_IN_6     86
#define  DEU_MENU_HIGHSPEED_DEU_IN_5     87
#define  DEU_MENU_HIGHSPEED_DEU_IN_4     88
#define  DEU_MENU_HIGHSPEED_DEU_IN_3     89
#define  DEU_MENU_HIGHSPEED_DEU_IN_2     90
#define  DEU_MENU_HIGHSPEED_DEU_IN_1     91
#define  DEU_MENU_HIGHSPEED_DEU_IN_0     92
#define  DEU_MENU_IDENT_DEU_IN           93
#define  DEU_MENU_ADDR_DIR_IN_A          94
#define  DEU_MENU_IDENT_DEU_OUT          95

#define  DIR_MENU                        3
#define  DIR_MENU_READLOOP               2       /* callback function: CB_DirMenu_read_loop */
#define  DIR_MENU_LOOP_TICK              3       /* callback function: CB_ReadLoopTimer */
#define  DIR_MENU_GET_OUT_DISCRETE_1     4       /* callback function: CB_Dir_Write_DiscreteHighSpeed */
#define  DIR_MENU_GET_OUT_HIGHSPEED_1    5       /* callback function: CB_Dir_Write_DiscreteHighSpeed */
#define  DIR_MENU_GET_OUT_HIGHSPEED_0    6       /* callback function: CB_Dir_Write_DiscreteHighSpeed */
#define  DIR_MENU_GET_OUT_DISCRETE_0     7       /* callback function: CB_Dir_Write_DiscreteHighSpeed */
#define  DIR_MENU_GET_OUT_CMC_NO         8       /* callback function: CB_Dir_Write_DeuPortNumber */
#define  DIR_MENU_GET_OUT_PORT_NO        9       /* callback function: CB_Dir_Write_DeuPortNumber */
#define  DIR_MENU_GET_IN_PORT_NO         10      /* callback function: CB_Dir_Read_PortNumber */
#define  DIR_MENU_GET_AUDIO_FREQ         11      /* callback function: CB_Dir_Write_AudioFreq */
#define  DIR_MENU_GET_SPY_AUDIO          12      /* callback function: CB_Dir_Write_SpyAudio */
#define  DIR_MENU_GET_OUT_DEU_NO         13      /* callback function: CB_Dir_Write_DeuPortNumber */
#define  DIR_MENU_CURSORMSG_51           14
#define  DIR_MENU_CURSORMSG_54           15
#define  DIR_MENU_CURSORMSG_55           16
#define  DIR_MENU_CURSORMSG_56           17
#define  DIR_MENU_CURSORMSG_57           18
#define  DIR_MENU_GET_IN_DEU_NO          19      /* callback function: CB_Dir_Read_DeuNumber */
#define  DIR_MENU_CLOSE                  20      /* callback function: CB_ClosePanelDirector */
#define  DIR_MENU_CURSORMSG_32           21
#define  DIR_MENU_CURSORMSG_58           22
#define  DIR_MENU_CURSORMSG_59           23
#define  DIR_MENU_HIGHSPEED_IN_7A        24
#define  DIR_MENU_CURSORMSG_60           25
#define  DIR_MENU_HIGHSPEED_IN_6A        26
#define  DIR_MENU_CURSORMSG_61           27
#define  DIR_MENU_HIGHSPEED_IN_5A        28
#define  DIR_MENU_CURSORMSG_62           29
#define  DIR_MENU_HIGHSPEED_IN_4A        30
#define  DIR_MENU_CURSORMSG_41           31
#define  DIR_MENU_CURSORMSG_40           32
#define  DIR_MENU_CURSORMSG_39           33
#define  DIR_MENU_CURSORMSG_38           34
#define  DIR_MENU_DECORATION_8           35
#define  DIR_MENU_DECORATION_7           36
#define  DIR_MENU_CURSORMSG_63           37
#define  DIR_MENU_HIGHSPEED_IN_3A        38
#define  DIR_MENU_DECORATION_10          39
#define  DIR_MENU_DECORATION_9           40
#define  DIR_MENU_DECORATION_6           41
#define  DIR_MENU_HIGHSPEED_IN_2A        42
#define  DIR_MENU_DISCRETE_IN_A          43
#define  DIR_MENU_HIGHSPEED_IN_1A        44
#define  DIR_MENU_HIGHSPEED_IN_0A        45
#define  DIR_MENU_SIGNAL_LOST_ERROR_C_A  46
#define  DIR_MENU_CRC_ERROR_CNT_A        47
#define  DIR_MENU_HEADER_LOST_CNT_A      48
#define  DIR_MENU_FRAME_TIMEOUT_CNT_A    49
#define  DIR_MENU_FRAME_ERROR_CNT_A      50
#define  DIR_MENU_SUPERFRAME_CNT_A       51
#define  DIR_MENU_IDENT_IN_A             52
#define  DIR_MENU_CURSORMSG_37           53
#define  DIR_MENU_RINGSLIDE_AUDIO        54      /* callback function: CB_DirAudioOutSignalType */
#define  DIR_MENU_CURSORMSG_36           55
#define  DIR_MENU_HIGHSPEED_IN_7B        56
#define  DIR_MENU_CURSORMSG_35           57
#define  DIR_MENU_HIGHSPEED_IN_6B        58
#define  DIR_MENU_SOFT_VERSION           59
#define  DIR_MENU_CURSORMSG_34           60
#define  DIR_MENU_HIGHSPEED_IN_5B        61
#define  DIR_MENU_CURSORMSG_33           62
#define  DIR_MENU_HIGHSPEED_IN_4B        63
#define  DIR_MENU_CURSORMSG_45           64
#define  DIR_MENU_CURSORMSG_65           65
#define  DIR_MENU_CURSORMSG_64           66
#define  DIR_MENU_CURSORMSG_44           67
#define  DIR_MENU_CURSORMSG_50           68
#define  DIR_MENU_CURSORMSG_49           69
#define  DIR_MENU_CURSORMSG_48           70
#define  DIR_MENU_CURSORMSG_47           71
#define  DIR_MENU_CURSORMSG_46           72
#define  DIR_MENU_CURSORMSG_31           73
#define  DIR_MENU_HIGHSPEED_IN_3B        74
#define  DIR_MENU_LOOP_TIME              75      /* callback function: CB_DirLoopTime */
#define  DIR_MENU_HIGHSPEED_IN_2B        76
#define  DIR_MENU_DISCRETE_IN_B          77
#define  DIR_MENU_HIGHSPEED_IN_1B        78
#define  DIR_MENU_HIGHSPEED_IN_0B        79
#define  DIR_MENU_SIGNAL_LOST_ERROR_C_B  80
#define  DIR_MENU_CRC_ERROR_CNT_B        81
#define  DIR_MENU_HEADER_LOST_CNT_B      82
#define  DIR_MENU_FRAME_TIMEOUT_CNT_B    83
#define  DIR_MENU_FRAME_ERROR_CNT_B      84
#define  DIR_MENU_SUPERFRAME_CNT_B       85
#define  DIR_MENU_ADDR_OUT               86
#define  DIR_MENU_IDENT_IN_B             87

#define  MESS_ERR                        4
#define  MESS_ERR_TEXTBOX                2

#define  MESS_SYS                        5
#define  MESS_SYS_TEXTBOX                2

#define  MOD_CONF                        6
#define  MOD_CONF_MODULE_ADDR_2          2       /* callback function: CB_ModuleAddrConf */
#define  MOD_CONF_CURSORMSG_2            3
#define  MOD_CONF_DECORATION_17          4
#define  MOD_CONF_MODULE_ADDR_1          5       /* callback function: CB_ModuleAddrConf */
#define  MOD_CONF_CURSORMSG_1            6
#define  MOD_CONF_MODULE_ENABLE_2        7       /* callback function: CB_ModuleEnableConf */
#define  MOD_CONF_CURSORMSG_21           8
#define  MOD_CONF_CURSORMSG_20           9
#define  MOD_CONF_MODULE_ENABLE_1        10      /* callback function: CB_ModuleEnableConf */

#define  PANEL                           7
#define  PANEL_STRING_CONFFILE_NAME      2
#define  PANEL_MODULE_TABLE              3
#define  PANEL_DECORATION_18             4
#define  PANEL_SYSTEM_LED                5       /* callback function: CB_SystemStatusLed */

#define  T1_DIAGFFT                      8
#define  T1_DIAGFFT_CLOSE                2       /* callback function: CB_ClosePanelTest1FFT */
#define  T1_DIAGFFT_RING_MODE_3          3       /* callback function: CB_ChooseFftShow */
#define  T1_DIAGFFT_WINDOW               4
#define  T1_DIAGFFT_SCALING              5
#define  T1_DIAGFFT_PEAK_FREQ_4          6
#define  T1_DIAGFFT_PEAK_FREQ_3          7
#define  T1_DIAGFFT_PEAK_FREQ_2          8
#define  T1_DIAGFFT_PEAK_FREQ            9
#define  T1_DIAGFFT_FFT_XMAX_SCALE       10      /* callback function: CB_SetFftXYScale */
#define  T1_DIAGFFT_DISPLAY              11
#define  T1_DIAGFFT_FFT_XMIN_SCALE       12      /* callback function: CB_SetFftXYScale */
#define  T1_DIAGFFT_POWER_PEAK           13
#define  T1_DIAGFFT_FFT_YMAX_SCALE       14      /* callback function: CB_SetFftXYScale */
#define  T1_DIAGFFT_DBGRAPH              15
#define  T1_DIAGFFT_FFTGRAPH             16
#define  T1_DIAGFFT_FFT_YMIN_SCALE       17      /* callback function: CB_SetFftXYScale */
#define  T1_DIAGFFT_TEXTMSG_7            18
#define  T1_DIAGFFT_TEXTMSG_6            19
#define  T1_DIAGFFT_TEXTMSG_4            20
#define  T1_DIAGFFT_UNIT                 21
#define  T1_DIAGFFT_UNIT_2               22

#define  T1_DIAGHIS                      9
#define  T1_DIAGHIS_CLOSE                2       /* callback function: CB_ClosePanelTest1Histo */
#define  T1_DIAGHIS_RING_MODE_3          3       /* callback function: CB_ChooseHistoShow */
#define  T1_DIAGHIS_HISTOGRAPH           4
#define  T1_DIAGHIS_CHECKBOX_CLR_HISTO_FL 5      /* callback function: CB_CheckBoxHisto */
#define  T1_DIAGHIS_HISTO_XMAX_SCALE     6       /* callback function: CB_SetHistoXYScale */
#define  T1_DIAGHIS_HISTO_XMIN_SCALE     7       /* callback function: CB_SetHistoXYScale */
#define  T1_DIAGHIS_HISTO_YMAX_SCALE     8       /* callback function: CB_SetHistoXYScale */
#define  T1_DIAGHIS_HISTO_YMIN_SCALE     9       /* callback function: CB_SetHistoXYScale */
#define  T1_DIAGHIS_STD_DEV_ADC1         10
#define  T1_DIAGHIS_STD_DEV_ADC2         11
#define  T1_DIAGHIS_STD_DEV_ADC3         12
#define  T1_DIAGHIS_MEAN_ADC1            13
#define  T1_DIAGHIS_STD_DEV_ADC4         14
#define  T1_DIAGHIS_MEAN_ADC2            15
#define  T1_DIAGHIS_STD_DEV_ADC5         16
#define  T1_DIAGHIS_MEAN_ADC3            17
#define  T1_DIAGHIS_STD_DEV_ADC6         18
#define  T1_DIAGHIS_MEAN_ADC4            19
#define  T1_DIAGHIS_STD_DEV_ADC7         20
#define  T1_DIAGHIS_MEAN_ADC5            21
#define  T1_DIAGHIS_STD_DEV_ADC8         22
#define  T1_DIAGHIS_MEAN_ADC6            23
#define  T1_DIAGHIS_MEAN_ADC7            24
#define  T1_DIAGHIS_MEAN_ADC8            25
#define  T1_DIAGHIS_MEAN                 26
#define  T1_DIAGHIS_MEAN_2               27
#define  T1_DIAGHIS_StdDev               28

#define  T1_DIAGLON                      10
#define  T1_DIAGLON_CLOSE                2       /* callback function: CB_ClosePanelTest1LongTerm */

#define  T1_DIAGRAW                      11
#define  T1_DIAGRAW_RING_ADC_CHOOSE      2       /* callback function: CB_RawChooseAdcShow */
#define  T1_DIAGRAW_CLOSE                3       /* callback function: CB_ClosePanelTest1Raw */
#define  T1_DIAGRAW_Y_ZOOM_IN_RAW        4       /* callback function: CB_ZoomIn */
#define  T1_DIAGRAW_X_ZOOM_IN_RAW        5       /* callback function: CB_ZoomIn */
#define  T1_DIAGRAW_XY_ZOOM_IN_RAW       6       /* callback function: CB_ZoomIn */
#define  T1_DIAGRAW_XY_EXPAND_RAW        7       /* callback function: CB_Expand_Raw */
#define  T1_DIAGRAW_Y_EXPAND_RAW         8       /* callback function: CB_Expand_Raw */
#define  T1_DIAGRAW_MODULE_NUMBER        9       /* callback function: CB_RawModuleNumber */
#define  T1_DIAGRAW_X_EXPAND_RAW         10      /* callback function: CB_Expand_Raw */
#define  T1_DIAGRAW_Y_POSITION           11
#define  T1_DIAGRAW_X_POSITION           12
#define  T1_DIAGRAW_CHECKBOX_ZOOM        13      /* callback function: CB_CheckRaw */
#define  T1_DIAGRAW_CHECKBOX_CVI_ZOOM    14      /* callback function: CB_CheckRaw */
#define  T1_DIAGRAW_GRAPH_RAW            15      /* callback function: CB_MoveCursor_Raw */
#define  T1_DIAGRAW_CURSORMSG_21         16

#define  TEST1_MENU                      12
#define  TEST1_MENU_STOPBUTTOM           2       /* callback function: CB_StopAcq */
#define  TEST1_MENU_STARTBUTTOM          3       /* callback function: CB_StartAcq */
#define  TEST1_MENU_CLOSE                4       /* callback function: CB_ClosePanelAdcTest1Menue */
#define  TEST1_MENU_CHECKBOX_DISPL_LONG_T 5      /* callback function: CB_CheckBoxTest1Display */
#define  TEST1_MENU_CHECKBOX_DISPL_FFT   6       /* callback function: CB_CheckBoxTest1Display */
#define  TEST1_MENU_CHECKBOX_DISPL_HISTO 7       /* callback function: CB_CheckBoxTest1Display */
#define  TEST1_MENU_CHECKBOX_AUTOSTART   8       /* callback function: CB_CheckBoxTest1Sample */
#define  TEST1_MENU_CHECKBOX_LEMO_IN_SS  9       /* callback function: CB_CheckBoxTest1Sample */
#define  TEST1_MENU_CHECKBOX_INTERNAL_STO 10     /* callback function: CB_CheckBoxTest1Sample */
#define  TEST1_MENU_CHECKBOX_STOP_LENGTH 11      /* callback function: CB_CheckBoxTest1Sample */
#define  TEST1_MENU_CHECKBOX_WRAP_PAGE   12      /* callback function: CB_CheckBoxTest1Sample */
#define  TEST1_MENU_CHECKBOX_DISPL_RAW   13      /* callback function: CB_CheckBoxTest1Display */
#define  TEST1_MENU_RING_TEST_MODE       14      /* callback function: CB_InputTestMode */
#define  TEST1_MENU_RING_WRAP_PAGE       15      /* callback function: CB_PageWrapMode */
#define  TEST1_MENU_RING_TRIGGER         16      /* callback function: CB_TriggerMode */
#define  TEST1_MENU_RING_MODE_CLK        17      /* callback function: CB_ClockMode */
#define  TEST1_MENU_KEY_STOP             18      /* callback function: CB_Key_Commands */
#define  TEST1_MENU_KEY_START            19      /* callback function: CB_Key_Commands */
#define  TEST1_MENU_READ_ADC             20      /* callback function: CB_ReadDisplayAdc */
#define  TEST1_MENU_DAC_TEST             21      /* callback function: CB_TestDAC */
#define  TEST1_MENU_CONFIG_ADC           22      /* callback function: CB_ConfigAdc */
#define  TEST1_MENU_RUN_EVENT_SECOND     23
#define  TEST1_MENU_ONE_SECOND_TICK      24      /* callback function: CB_RunSecondTick */
#define  TEST1_MENU_ADC_OFFSET_1         25      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_2         26      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_3         27      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_4         28      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_5         29      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_6         30      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_OFFSET_7         31      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_TRIGGER_THRESHOLD    32      /* callback function: CB_ADC_Trigger_Parameter */
#define  TEST1_MENU_TRIGGER_PULSE_LEN    33      /* callback function: CB_ADC_Trigger_Parameter */
#define  TEST1_MENU_TRIGGER_GAP          34      /* callback function: CB_ADC_Trigger_Parameter */
#define  TEST1_MENU_TRIGGER_PEAK         35      /* callback function: CB_ADC_Trigger_Parameter */
#define  TEST1_MENU_ADC_EVENT_LENGTH     36      /* callback function: CB_ADC_Sampling_Parameter */
#define  TEST1_MENU_ADC_STOP_DELAY       37      /* callback function: CB_ADC_Sampling_Parameter */
#define  TEST1_MENU_ADC_START_DELAY      38      /* callback function: CB_ADC_Sampling_Parameter */
#define  TEST1_MENU_ADC_OFFSET_8         39      /* callback function: CB_ADC_Offset */
#define  TEST1_MENU_ADC_GAIN_8           40      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_7           41      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_6           42      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_CURSORMSG_8          43
#define  TEST1_MENU_CURSORMSG_7          44
#define  TEST1_MENU_CURSORMSG_6          45
#define  TEST1_MENU_CURSORMSG_5          46
#define  TEST1_MENU_CURSORMSG_4          47
#define  TEST1_MENU_CURSORMSG_3          48
#define  TEST1_MENU_CURSORMSG_2          49
#define  TEST1_MENU_CURSORMSG_1          50
#define  TEST1_MENU_ADC_GAIN_5           51      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_4           52      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_3           53      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_2           54      /* callback function: CB_ADC_Gain */
#define  TEST1_MENU_ADC_GAIN_1           55      /* callback function: CB_ADC_Gain */

#define  TEST2_MENU                      13
#define  TEST2_MENU_CLOSE                2       /* callback function: CB_ClosePanelAdcTest2Menue */
#define  TEST2_MENU_GRAPH_FIR_E          3
#define  TEST2_MENU_ENERGY_HISTOGRAPH    4
#define  TEST2_MENU_GRAPH_FIR_T          5
#define  TEST2_MENU_GRAPH_RAW            6
#define  TEST2_MENU_STOPBUTTOM           7       /* callback function: CB_Test2_StopAdc */
#define  TEST2_MENU_CHECKBOX_SHOW_GRAPH  8       /* callback function: CB_CheckBoxTest2 */
#define  TEST2_MENU_CHECKBOX_LOOP_ENABLE 9       /* callback function: CB_CheckBoxTest2 */
#define  TEST2_MENU_ENERGY_XMAX_SCALE    10      /* callback function: CB_SetEnergyXScale */
#define  TEST2_MENU_FIR_XMAX_SCALE       11      /* callback function: CB_SetFirXYScale */
#define  TEST2_MENU_CHECKBOX_TRIGGER_GT  12      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_PEAKMSE              13
#define  TEST2_MENU_PEAKINT              14
#define  TEST2_MENU_PEAKWIDTH            15
#define  TEST2_MENU_PEAKPOS              16
#define  TEST2_MENU_CHECKBOX_TRIGGER_LT  17      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_ENERGY_XMIN_SCALE    18      /* callback function: CB_SetEnergyXScale */
#define  TEST2_MENU_FIR_XMIN_SCALE       19      /* callback function: CB_SetFirXYScale */
#define  TEST2_MENU_CONFIGBUTTOM         20      /* callback function: CB_Test2_ConfigAdc */
#define  TEST2_MENU_GAUSFIT_BUTTOM       21      /* callback function: CB_Test2EnergyGaussFit */
#define  TEST2_MENU_CLREAR_BUTTOM        22      /* callback function: CB_Test2_ClearHistoAdc */
#define  TEST2_MENU_STARTBUTTOM          23      /* callback function: CB_Test2_StartAdc */
#define  TEST2_MENU_GET_IN_ENER_LENGTH   24      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_GET_IN_ENER_GAP      25      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_GET_IN_TRIG_THRESHOLD 26     /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_GET_IN_TRIG_LENGTH   27      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_GET_IN_TRIG_GAP      28      /* callback function: CB_Test2_get_G_and_P */
#define  TEST2_MENU_ENERGY_DISPLAY3      29
#define  TEST2_MENU_ENERGY_DISPLAY2      30
#define  TEST2_MENU_TRIGGER_COUNTER      31
#define  TEST2_MENU_ENERGY_DISPLAY1      32


     /* Menu Bars, Menus, and Menu Items: */

#define  MENU                            1
#define  MENU_FILE                       2
#define  MENU_FILE_QUIT                  3       /* callback function: CB_MenuFile */
#define  MENU_VIEW                       4
#define  MENU_VIEW_MESSAGES              5
#define  MENU_VIEW_MESSAGES_SUBMENU      6
#define  MENU_VIEW_MESSAGES_SYSTEM       7       /* callback function: CB_MenuMessages */
#define  MENU_VIEW_MESSAGES_ERROR        8       /* callback function: CB_MenuMessages */
#define  MENU_CONFIGURATION              9
#define  MENU_CONFIGURATION_LOAD         10      /* callback function: CB_MenuConf */
#define  MENU_CONFIGURATION_SEPARATOR_3  11
#define  MENU_CONFIGURATION_SAVE         12      /* callback function: CB_MenuConf */
#define  MENU_CONFIGURATION_SAVEAS       13      /* callback function: CB_MenuConf */
#define  MENU_CONFIGURATION_SEPARATOR_2  14
#define  MENU_CONFIGURATION_MODULES      15      /* callback function: CB_MenuConf */
#define  MENU_CONFIGURATION_SEPARATOR_4  16
#define  MENU_TEST_MENUS                 17
#define  MENU_TEST_MENUS_OPENPANEL_TEST1 18      /* callback function: CB_OpenPanelAdcTest1Menue */
#define  MENU_TEST_MENUS_SEPARATOR_4     19

#define  MENUBAR                         2

#define  MENUCONEN                       3
#define  MENUCONEN_PANEL                 2
#define  MENUCONEN_PANEL_CLOSE           3       /* callback function: CB_Menu_DecayTimeConfiguration */

#define  MENUCONFMO                      4
#define  MENUCONFMO_PANEL                2
#define  MENUCONFMO_PANEL_CLOSE          3       /* callback function: CB_Menu_ModuleConfiguration */
#define  MENUCONFMO_TESTS                4
#define  MENUCONFMO_TESTS_LEDTEST        5       /* callback function: CB_Menu_ModuleLedTest */

#define  MENUCONFSA                      5
#define  MENUCONFSA_PANEL                2
#define  MENUCONFSA_PANEL_CLOSE          3       /* callback function: CB_Menu_ComParamterConfiguration */

#define  MENUCONFTR                      6
#define  MENUCONFTR_PANEL                2
#define  MENUCONFTR_PANEL_CLOSE          3       /* callback function: CB_Menu_TriggerConfiguration */
#define  MENUCONFTR_PANEL_SEPARATOR      4
#define  MENUCONFTR_GET                  5
#define  MENUCONFTR_GET_MODULE1          6       /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE2          7       /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE3          8       /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE4          9       /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE5          10      /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE6          11      /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE7          12      /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_MODULE8          13      /* callback function: CB_GetTrigValueToTable */
#define  MENUCONFTR_GET_SEPARATOR_2      14
#define  MENUCONFTR_PUT                  15
#define  MENUCONFTR_PUT_MODULE1          16      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE2          17      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE3          18      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE4          19      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE5          20      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE6          21      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE7          22      /* callback function: CB_PutTrigValueToTable */
#define  MENUCONFTR_PUT_MODULE8          23      /* callback function: CB_PutTrigValueToTable */

#define  MENUCONFyy                      7
#define  MENUCONFyy_PANEL                2
#define  MENUCONFyy_PANEL_CLOSE          3       /* callback function: CB_Menu_DecayTimeConfiguration */

#define  MENUCTRLOF                      8
#define  MENUCTRLOF_PANEL                2
#define  MENUCTRLOF_PANEL_CLOSE          3       /* callback function: CB_MenuOfflineControl */
#define  MENUCTRLOF_FILE                 4
#define  MENUCTRLOF_FILE_DEFINE_FILENAME 5       /* callback function: CB_MenuOfflineControl */

#define  MENUCTRLRU                      9
#define  MENUCTRLRU_PANEL                2
#define  MENUCTRLRU_PANEL_CLOSE          3       /* callback function: CB_MenuRunControl */
#define  MENUCTRLRU_FILE                 4
#define  MENUCTRLRU_FILE_CHANGE_FILENAME 5       /* callback function: CB_MenuRunControl */

#define  MENUDIAG                        10
#define  MENUDIAG_PANEL                  2
#define  MENUDIAG_PANEL_CLOSE            3       /* callback function: CB_MenuDiagnostic */

#define  MENUDIAGRA                      11
#define  MENUDIAGRA_PANEL                2
#define  MENUDIAGRA_PANEL_CLOSE          3       /* callback function: CB_MenuDiagnosticRaw */

#define  MENUDIAGxx                      12
#define  MENUDIAGxx_PANEL                2
#define  MENUDIAGxx_PANEL_CLOSE          3       /* callback function: CB_MenuDiagnosticEnergy */

#define  MENUMESERR                      13
#define  MENUMESERR_CLOSE                2       /* callback function: CB_Menu_MessagesError */
#define  MENUMESERR_CLEAR                3       /* callback function: CB_Menu_MessagesError */

#define  MENUMESSYS                      14
#define  MENUMESSYS_CLOSE                2       /* callback function: CB_Menu_MessagesSystem */
#define  MENUMESSYS_CLEAR                3       /* callback function: CB_Menu_MessagesSystem */
#define  MENUMESSYS_TEST1                4       /* callback function: CB_Menu_MessagesSystem */

#define  MENUTEMP                        15
#define  MENUTEMP_FILE                   2
#define  MENUTEMP_FILE_QUIT              3       /* callback function: CB_MenuFile */

#define  Panel                           16


     /* Callback Prototypes: */ 

int  CVICALLBACK CB_ADC_Gain(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ADC_Offset(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ADC_Sampling_Parameter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ADC_Trigger_Parameter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckBoxDeuTransmit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckBoxHisto(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckBoxTest1Display(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckBoxTest1Sample(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckBoxTest2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_CheckRaw(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ChooseFftShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ChooseHistoShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClockMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelAdcTest1Menue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelAdcTest2Menue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelDeuTop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelDirector(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelTest1FFT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelTest1Histo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelTest1LongTerm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ClosePanelTest1Raw(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ConfigAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Deu_Audio_CmcChNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Deu_Read_DeuPortNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Deu_Write_DeuPortNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Deu_Write_DiscreteHighSpeed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Deu_Write_SpyAudio(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DeuGraphCheckBox(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DeuTopLoopTime(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DeuTopMenu_read_loop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Read_DeuNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Read_PortNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Write_AudioFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Write_DeuPortNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Write_DiscreteHighSpeed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Dir_Write_SpyAudio(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DirAudioOutSignalType(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DirLoopTime(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_DirMenu_read_loop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Expand_Raw(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CB_GetTrigValueToTable(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CB_InputTestMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Key_Commands(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CB_Menu_ComParamterConfiguration(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_DecayTimeConfiguration(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_MessagesError(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_MessagesSystem(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_ModuleConfiguration(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_ModuleLedTest(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_Menu_TriggerConfiguration(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuConf(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuDiagnostic(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuDiagnosticEnergy(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuDiagnosticRaw(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuFile(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuMessages(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuOfflineControl(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CB_MenuRunControl(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CB_ModuleAddrConf(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ModuleEnableConf(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_MoveCursor_Raw(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CB_OpenPanelAdcTest1Menue(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CB_PageWrapMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CB_PutTrigValueToTable(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CB_RawChooseAdcShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_RawModuleNumber(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ReadDisplayAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ReadLoopDeuTopTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ReadLoopTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_RunSecondTick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SetEnergyXScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SetFftXYScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SetFirXYScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SetHistoXYScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_StartAcq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_StopAcq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SystemStatusLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2_ClearHistoAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2_ConfigAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2_get_G_and_P(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2_StartAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2_StopAdc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_Test2EnergyGaussFit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_TestDAC(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_TriggerMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_ZoomIn(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
