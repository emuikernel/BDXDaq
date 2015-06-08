/***************************************************************************/
/*                                                                         */
/*  Filename: sis3320_global.h                                             */
/*                                                                         */
/*  Funktion: definition of globals                                        */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 07.05.2005                                       */
/*  last modification:    30.09.2005                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  SIS  Struck Innovative Systeme GmbH                                    */
/*                                                                         */
/*  Harksheider Str. 102A                                                  */
/*  22399 Hamburg                                                          */
/*                                                                         */
/*  Tel. +49 (0)40 60 87 305 0                                             */
/*  Fax  +49 (0)40 60 87 305 20                                            */
/*                                                                         */
/*  http://www.struck.de                                                   */
/*                                                                         */
/*  © 2005                                                                 */
/*                                                                         */
/***************************************************************************/

// --- Programm globale Konstanten ------------------------------------------

#define OFF					 0
#define ON					 1

// Panel Open Positions

#define DEFINE_PANEL_MODULE_CONF_POS_Y 			400
#define DEFINE_PANEL_MODULE_CONF_POS_X 			 50


#define DEFINE_PANEL_TEST1_MENU_CTRL_POS_Y 		40
#define DEFINE_PANEL_TEST1_MENU_CTRL_POS_X 		10

#define DEFINE_PANEL_TEST1_RAW_POS_Y 			380
#define DEFINE_PANEL_TEST1_RAW_POS_X 			10

#define DEFINE_PANEL_TEST1_HISTO_POS_Y 			380
#define DEFINE_PANEL_TEST1_HISTO_POS_X 			700

#define DEFINE_PANEL_TEST1_FFT_POS_Y 			420
#define DEFINE_PANEL_TEST1_FFT_POS_X 			100

#define DEFINE_PANEL_TEST1_LTERM_POS_Y 			520
#define DEFINE_PANEL_TEST1_LTERM_POS_X 			100




#define SYSTEM_STATUS_NULL        		 	0
#define SYSTEM_STATUS_MODULES_NOT_READY 	1		 // red
#define SYSTEM_STATUS_MODULES_READY			2		 // yellow
#define SYSTEM_STATUS_MODULES_CONFIGURED	3		 // green
#define SYSTEM_STATUS_MODULES_RUNNING		4		 // blue




#define MAX_NO_OF_MODULES    	2
#define INDEX_MODULE1 			0
#define INDEX_MODULE2 			1



#define MAX_PATHNAME_LENGTH 	1024
#define MAX_FILENAME_LENGTH 	1024  //including Pathname



#define NORMAL_CONFIGURATION_FILE  		"sis3320_gui_config.ini"   


#define STRING_CONFIGURATION_FILE		"SIS3320 CONFIGURATION File"   
#define STRING_DATE  					"Date: "   
#define STRING_TIME  					"   Time: "   
#define STRING_MODULES  				"MODULES"   
#define STRING_BLANK				" "   
#define STRING_END_OF_FILE			"END_OF_FILE"   

#define FILE_ACCESS_OK  			0   
#define FILE_ACCESS_NOT_OK  		-1   

#define STATUS_YES  	  			0   
#define STATUS_NO			  		-1   

#define PRINT_DATE_YES 	  			0   
#define PRINT_DATE_NO 	  			-1   


// default configuration values
#define MODULE_CONF_DEFAULT_STARTADDRESS   	0x30000000 
#define MODULE_CONF_DEFAULT_ADDRESSINC   	0x1000000 







#define NO_OF_PANEL			 	26	  // Anzahl der Fenster im UIR-File

#define	MAIN				 	0	  // Haupt-Fenster


#define MODULE_PARA_CONF	 	1
#define MODULE_MESS_SYSTEM	 	2
#define MODULE_MESS_ERROR	 	3

  
#define TEST1_MENUE       		4	  // Fenster fuer die Funktionskarten
#define TEST1_RAW       		5	  // Fenster fuer die Funktionskarten
#define TEST1_HISTO       		6	  // Fenster fuer die Funktionskarten
#define TEST1_FFT       		7	  // Fenster fuer die Funktionskarten
#define TEST1_LTERM       		8	  // Fenster fuer die Funktionskarten




#define MESSAGE_PANEL	 	 	26	  // Nachrichten-Fenster

// Menubars
#define NO_OF_MENUBARS	     20	  // Anzahl der Fenster im UIR-File

#define MAIN_MENU_BAR              0
#define CONFMODULE_MENU_BAR		   1


// --- Programm globale Variablen -------------------------------------------


extern int Panels[NO_OF_PANEL];		// Feld fuer die einzelnen PanelHandles
extern int MenuBars[NO_OF_MENUBARS];		// Feld fuer die einzelnen PanelHandles




extern char gl_char_FilePathConf[MAX_PATHNAME_LENGTH];
extern char gl_char_FileNameConf[MAX_FILENAME_LENGTH];	 

extern char gl_char_FilePathData[MAX_PATHNAME_LENGTH];
extern char gl_char_FileNameData[MAX_FILENAME_LENGTH];	 
extern char gl_char_FileNameLoaderTS[MAX_FILENAME_LENGTH];	 



// run globals
extern unsigned int gl_uint_ModConfIndexRun[MAX_NO_OF_MODULES]  ;
extern unsigned int gl_uint_ModAddrRun[MAX_NO_OF_MODULES]  ;
extern unsigned int gl_uint_NoOfModulesRun ;


// configuration globals 
extern unsigned int gl_uint_system_status ;   

extern unsigned int gl_uint_ModEnableConf[MAX_NO_OF_MODULES]   ;
extern unsigned int gl_uint_ModAddrConf[MAX_NO_OF_MODULES]		;


				   

// ADC Test1 

extern unsigned int  gl_uint_Test1DisplayRawFlag     ;
extern unsigned int  gl_uint_Test1DisplayHistoFlag     ;
extern unsigned int  gl_uint_Test1DisplayFftFlag     ;
extern unsigned int  gl_uint_Test1DisplayLongTermFlag     ;
extern unsigned int  gl_uint_Test1AutoClearHistogramFlag     ;

extern unsigned int  gl_unit_Test1RunOneSecondCounter     ;
extern unsigned int  gl_uint_Test1RunStopStatusFlag     ;

extern unsigned int  gl_uint_Test1RawAdcShowChoose     ;
extern unsigned int  gl_uint_Test1HistoAdcShowChoose     ;
extern unsigned int  gl_uint_Test1FftAdcShowChoose     ;


extern unsigned int  gl_uint_Test1ClockMode     ;
extern unsigned int  gl_uint_Test1AdcGain[8]     ;
extern unsigned int  gl_uint_Test1AdcOffset[8]     ;

extern unsigned int  gl_uint_Test1InternalTriggerEnableFlag     ;
extern unsigned int  gl_uint_Test1LemoInStartStopEnableFlag     ;
extern unsigned int  gl_uint_Test1AutostartEnableFlag     ;

extern unsigned int  gl_uint_Test1StartDelay     ;
extern unsigned int  gl_uint_Test1StopDelay     ;
extern unsigned int  gl_uint_Test1MaxSampleCount     ;
extern unsigned int  gl_uint_Test1SampleCountStopFlag     ;
extern unsigned int  gl_uint_Test1PageWrapFlag     ;


extern unsigned int  gl_PageWrapMode     ;
extern unsigned int  gl_EventPageWrapLength     ;

extern unsigned int  gl_adcEventSampleReadoutLength     ;

extern unsigned int  gl_InputTestMode     ;









extern double gl_doubleTemp_array[0x20000]  ;	// 256KBytes, 128 Ksamples   

extern unsigned int gl_uintGraph_array[0x4000]  ;	// 14 bit   

extern unsigned int gl_uintHistoADC_1_array[0x4000]  ;	// 14 bit   
extern unsigned int gl_uintHistoADC_2_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_3_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_4_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_5_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_6_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_7_array[0x4000]  ;	//     
extern unsigned int gl_uintHistoADC_8_array[0x4000]  ;	//     

extern unsigned int gl_dma_rd_buffer[0x200000] ; //	 2 MLWorte  ; 4 MSample ;  8 Mbyte MByte
extern unsigned int gl_uint_adc1_data_array[0x200000] ; // 2 MLWorte  ; 4 MSample ;  8 Mbyte MByte    
extern unsigned int gl_uint_adc2_data_array[0x200000] ;
extern unsigned int gl_uint_adc3_data_array[0x200000] ;
extern unsigned int gl_uint_adc4_data_array[0x200000] ;
extern unsigned int gl_uint_adc5_data_array[0x200000] ;
extern unsigned int gl_uint_adc6_data_array[0x200000] ;
extern unsigned int gl_uint_adc7_data_array[0x200000] ;
extern unsigned int gl_uint_adc8_data_array[0x200000] ;



extern unsigned short gl_ushortDAC_Test_ADC_1_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_2_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_3_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_4_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_5_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_6_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_7_array[0x10000]  ;	// 16 bit   
extern unsigned short gl_ushortDAC_Test_ADC_8_array[0x10000]  ;	// 16 bit   


// --- Prototyps     -----------------------------


int FirstReadConfiguration (void)  ;
int LoadConfiguration (void) ;
int ReadConfiguration (char *path_file) ;
int SaveConfigurationAs (void)	  ;
int SaveConfiguration (char *path_file) ;

int distribute_module_addresses (void)  ;

int CheckSystemStatus (void)  ;


int openPanelTest1Raw(void) ;
int openPanelTest1FFT(void)  ;
int openPanelTest1Histo(void); 
int openPanelTest1LongTerm(void); 

int displayRawData (unsigned int plot_size) ;
int displayAdcHistograph (void) ;
int displayAdcFFTgraph (void) ;

int ConfigurationSetupAdc (void) ;


// --- Prototypen -----------------------------------------------------------
int sisVME_ErrorHandling (unsigned int prot_error, unsigned int vme_addr, char *err_messages)  ;
int write_system_messages (char *system_messages,  int write_datetime)   ;


