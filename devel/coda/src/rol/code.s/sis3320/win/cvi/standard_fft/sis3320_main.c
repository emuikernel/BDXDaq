/***************************************************************************/
/*                                                                         */
/*  Filename: sis3320_main.c                                               */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 16.05.2005                                       */
/*  last modification:    29.09.2005                                       */
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


// --- CVI - Header ---------------------------------------------------------
#include <windows.h>
#include <formatio.h>
#include <cvirte.h>
#include <userint.h>
#include <stdio.h>
#include <ansi_c.h>
#include <utility.h>         

// --- Programm - Header ----------------------------------------------------

#include "sis3320_main_uir.h"
#include "sis3320_global.h" 	// Globale Defines fuers Programm

 
/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/

#include "vme_interface_defines.h"


// --- Programm globale Variablen -------------------------------------------
// 

struct SIS1100_Device_Struct  sis1100_device1 ;
struct SIS1100_Device_Struct  gl_sis1100_device ;


char gl_char_FilePathConf[1024];
char gl_char_FileNameConf[1024];	 
char gl_char_FileNameLoaderTS[1024];	 

char gl_char_FilePathData[1024];
char gl_char_FileNameData[1024];	 

// run globals
unsigned int gl_uint_ModConfIndexRun[MAX_NO_OF_MODULES]  ;
unsigned int gl_uint_ModAddrRun[MAX_NO_OF_MODULES]  ;
unsigned int gl_uint_NoOfModulesRun ;

// configuration globals 
unsigned int gl_uint_system_status ;   

unsigned int gl_uint_ModEnableConf[MAX_NO_OF_MODULES]   ;
unsigned int gl_uint_ModAddrConf[MAX_NO_OF_MODULES]		;




unsigned int  gl_uint_Test1DisplayRawFlag     ;
unsigned int  gl_uint_Test1DisplayHistoFlag     ;
unsigned int  gl_uint_Test1DisplayFftFlag     ;
unsigned int  gl_uint_Test1DisplayLongTermFlag     ;
unsigned int  gl_uint_Test1AutoClearHistogramFlag     ;

unsigned int  gl_unit_Test1RunOneSecondCounter     ;
unsigned int  gl_uint_Test1RunStopStatusFlag     ;


unsigned int  gl_uint_Test1RawAdcShowChoose     ;
unsigned int  gl_uint_Test1HistoAdcShowChoose     ;
unsigned int  gl_uint_Test1FftAdcShowChoose     ;


unsigned int  gl_uint_Test1ClockMode     ;
unsigned int  gl_uint_Test1AdcGain[8]     ;
unsigned int  gl_uint_Test1AdcOffset[8]     ;


unsigned int  gl_uint_Test1InternalTriggerEnableFlag     ;
unsigned int  gl_uint_Test1LemoInStartStopEnableFlag     ;
unsigned int  gl_uint_Test1AutostartEnableFlag     ;


unsigned int  gl_uint_Test1StartDelay     ;
unsigned int  gl_uint_Test1StopDelay     ;
unsigned int  gl_uint_Test1MaxSampleCount     ;
unsigned int  gl_uint_Test1SampleCountStopFlag     ;
unsigned int  gl_uint_Test1PageWrapFlag     ;


unsigned int  gl_PageWrapMode     ;
unsigned int  gl_EventPageWrapLength     ;

unsigned int  gl_adcEventSampleReadoutLength     ;

unsigned int  gl_InputTestMode     ;







double gl_doubleTemp_array[0x20000]  ;	// 256KBytes, 128 Ksamples   


unsigned int gl_uintGraph_array[0x4000]  ;	// 14 bit   

unsigned int gl_uintHistoADC_1_array[0x4000]  ;	// 14 bit   
unsigned int gl_uintHistoADC_2_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_3_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_4_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_5_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_6_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_7_array[0x4000]  ;	//     
unsigned int gl_uintHistoADC_8_array[0x4000]  ;	//     





unsigned int gl_dma_rd_buffer[0x200000] ; // 2 MLWorte  ; 4 MSample ;  8 Mbyte MByte

unsigned int gl_uint_adc1_data_array[0x200000] ; // 2 MLWorte  ; 4 MSample ;  8 Mbyte MByte
unsigned int gl_uint_adc2_data_array[0x200000] ;
unsigned int gl_uint_adc3_data_array[0x200000] ;
unsigned int gl_uint_adc4_data_array[0x200000] ;
unsigned int gl_uint_adc5_data_array[0x200000] ;
unsigned int gl_uint_adc6_data_array[0x200000] ;
unsigned int gl_uint_adc7_data_array[0x200000] ;
unsigned int gl_uint_adc8_data_array[0x200000] ;




unsigned short gl_ushortDAC_Test_ADC_1_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_2_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_3_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_4_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_5_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_6_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_7_array[0x10000]  ;	// 16 bit   
unsigned short gl_ushortDAC_Test_ADC_8_array[0x10000]  ;	// 16 bit   





// --- Panel Handle ---------------------------------------------------------
int Panels[NO_OF_PANEL];		// Feld fuer die einzelnen PanelHandles
int MenuBars[NO_OF_MENUBARS];		// Feld fuer die einzelnen PanelHandles

int intShowPanel;							// Zeigt an welches Panel nach
											// dem Message-Panel angezeigt
											// werden soll


// --- Prototypen -----------------------------------------------------------



/* --------------------------------------------------------------------------
   Hauptprogramm
-------------------------------------------------------------------------- */
int main (int argc, char *argv[])
{									 

	int CviErr;
	int error=0;
	int mode_wr, mode_rd;
	int stat;
	unsigned int addr_wr, addr_rd, data_rd;

	int sis_ret ;
	HANDLE hProcess;
	HANDLE hDevice ;
	U32 found_no_sis1100 ;
	U32 use_sis1100_no ;
	U32 sis1100_no  ;
	U32 sis1100_PciBus ;
	U32 sis1100_PciSlot ;
	U32 data ;
	int return_code ;
    int i ;
    char messages_buffer[80] ;           
	int i2, temp ;


	
	if (InitCVIRTE (0, argv, 0) == 0)  return -1;  	// Nicht genug Speicher

	// Das Haupt-Panel laden
	if ((Panels[MAIN] = LoadPanel (0, "sis3320_main_uir.uir", PANEL)) < 0) return -1;

  
	if ((Panels[MODULE_PARA_CONF] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", MOD_CONF)) < 0) return -1;

	if ((Panels[MODULE_MESS_SYSTEM] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", MESS_SYS)) < 0) return -1;
	if ((Panels[MODULE_MESS_ERROR]  = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", MESS_ERR)) < 0) return -1;

				 
	if ((Panels[TEST1_MENUE] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", TEST1_MENU)) < 0) return -1;
	if ((Panels[TEST1_RAW] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", T1_DIAGRAW)) < 0) return -1;
	if ((Panels[TEST1_HISTO] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", T1_DIAGHIS)) < 0) return -1;
	if ((Panels[TEST1_FFT] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", T1_DIAGFFT)) < 0) return -1;
	if ((Panels[TEST1_LTERM] = LoadPanel (Panels[MAIN], "sis3320_main_uir.uir", T1_DIAGLON)) < 0) return -1;


	




// load MenuBars

	if ((MenuBars[MAIN_MENU_BAR] = LoadMenuBar (Panels[MAIN], "sis3320_main_uir.uir", MENU)) < 0) return -1;
							

	gl_uint_Test1DisplayRawFlag = 0 ;
	gl_uint_Test1DisplayHistoFlag = 0 ;
	gl_uint_Test1DisplayFftFlag = 0 ;


#ifdef PCI_VME_INTERFACE


/************************************
*        init sis1100
************************************/

	sis_ret = sis1100w_Find_No_Of_sis1100(&found_no_sis1100) ;

    if (found_no_sis1100 == 0)  exit(-1) ;

    sprintf (messages_buffer, "number of SIS1100 modules = %d \n",found_no_sis1100);
    write_system_messages (messages_buffer, PRINT_DATE_YES)	   ;

	for (i=1;i<=found_no_sis1100;i++) {
	  sis_ret = sis1100w_Get_PciSlot_Information(i, &sis1100_PciBus, &sis1100_PciSlot) ;
	}
	sprintf (messages_buffer, "used SIS1100 is in PCI Slot %d  \n",found_no_sis1100);
    write_system_messages (messages_buffer, PRINT_DATE_NO)	   ;

	use_sis1100_no = found_no_sis1100 ; // use last one which was founded
  //   use_sis1100_no = 1 ; 
    if ((sis1100w_Get_Handle_And_Open(use_sis1100_no, &sis1100_device1))!=0) exit(-1);
    if ((sis1100w_Init(&sis1100_device1, 1 /* enable_logfile */)) !=0) {
    	write_system_messages ("failed to open SIS1100/3100", PRINT_DATE_NO)	   ;
 		//  exit(-1);
 	 }
 	 else {
    	write_system_messages ("open and init SIS1100/3100  OK", PRINT_DATE_NO)	   ;
  	 }

    sis1100w_Init_sis3100(&sis1100_device1, 1 /* enable_logfile */) ; 
    gl_sis1100_device =   sis1100_device1 ;	   // global SIS1100 device handle 

#endif

/************************************
*       end of  init sis1100
************************************/

    SetCtrlVal (Panels[MAIN], PANEL_SYSTEM_LED, ON);
	SetCtrlAttribute (Panels[MAIN], PANEL_SYSTEM_LED, ATTR_ON_COLOR, VAL_RED);


   FirstReadConfiguration();

	// Das Panel anzeigen
	SetPanelPos(Panels[MAIN],30,20)  ;
	DisplayPanel (Panels[MAIN]);
	// Die Prozessprioritaet setzen


	hProcess = GetCurrentProcess();
//	stat = SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
//	stat = SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
	// IDLE_PRIORITY_CLASS
	// NORMAL_PRIORITY_CLASS
	// HIGH_PRIORITY_CLASS
	// REALTIME_PRIORITY_CLASS  
	
	// Das CVI-Userinterface starten
	RunUserInterface ();

	return 0;
} // end function main ()

			  











//--------------------------------------------------------------------------- 
// Root Menu Call Back
//--------------------------------------------------------------------------- 

void CVICALLBACK CB_MenuFile (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        /* Take action depending on which item was selected */
        case MENU_FILE_QUIT :
            if (ConfirmPopup ("Program End", "Sure to quit Application?"))  {  
             SaveConfiguration (gl_char_FileNameConf)  ;
				QuitUserInterface (0);
                return;}
            break;


        }    // end switch menu item
            
             
return;

}


 
/* Messages Windows */

void CVICALLBACK CB_MenuMessages (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        /* Take action depending on which item was selected */

        case MENU_VIEW_MESSAGES_SYSTEM : {
	        SetPanelPos(Panels[MODULE_MESS_SYSTEM],60,400)  ;   // y, x
 			DisplayPanel (Panels[MODULE_MESS_SYSTEM]);
            }
            break;
        case MENU_VIEW_MESSAGES_ERROR : {
	        SetPanelPos(Panels[MODULE_MESS_ERROR],360,400)  ;   
 			DisplayPanel (Panels[MODULE_MESS_ERROR]);
            }
            break;
        }    // end switch menu item
              
return;

}







/* close Panels */
void CVICALLBACK CB_Menu_MessagesSystem (int menuBar, int menuItem, void *callbackData,
        int panel)
{
  int i2, temp ;
    switch (menuItem) {
        case MENUMESSYS_CLOSE : {
            HidePanel(Panels[MODULE_MESS_SYSTEM]); //Clear Panel 
           }  
           break;
        case MENUMESSYS_CLEAR : {
			ResetTextBox (Panels[MODULE_MESS_SYSTEM], MESS_SYS_TEXTBOX, "");
           }  
           break;
        case MENUMESSYS_TEST1 : {

           
           }  
           break;

     }  // switch
return;
}







			  
	
	



/* close Panels */
void CVICALLBACK CB_Menu_MessagesError (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        case MENUMESERR_CLOSE : {
            HidePanel(Panels[MODULE_MESS_ERROR]); //Clear Panel 
           }  
           break;
        case MENUMESERR_CLEAR : {
			ResetTextBox (Panels[MODULE_MESS_ERROR], MESS_ERR_TEXTBOX, "");
           }  
           break;
     }  // switch
return;
}









/* --------------------------------------------------------------------------
   CB-Funktion  
-------------------------------------------------------------------------- */

int CVICALLBACK CB_SystemStatusLed (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    char buffer[2048];
	switch (event)
		{
		case EVENT_COMMIT:
			break;

		case EVENT_RIGHT_CLICK:

			CopyString (buffer, 0,              "grey:   SIS1100/3100 interface not initialized \n", 0,
			                             strlen("grey:   SIS1100/3100 interface not initialized \n"));
			CopyString (buffer, strlen(buffer), "red:    SIS3300 Module status/definition not OK \n", 0,
			                             strlen("red:    SIS3300 Module status/definition not OK \n"));
			CopyString (buffer, strlen(buffer), "yellow: not valid configuration \n", 0,
			                             strlen("yellow: not valid configuration \n"));
			CopyString (buffer, strlen(buffer), "green:  valid configuration / ready for running \n", 0,
			                             strlen("green:  valid configuration / ready for running \n"));
			CopyString (buffer, strlen(buffer), "blue:   running \n", 0,
			                             strlen("blue:   running \n"));

			MessagePopup ("meaning  ",buffer) ;
    		break;
		}
	return 0;
}





/* --------------------------------------------------------------------------
   VME Error Handling
-------------------------------------------------------------------------- */


int sisVME_ErrorHandling (unsigned int prot_error, unsigned int vme_addr, char *err_messages)
{
	
	int CviErr;
//	char error_msg[MAX_PATHNAME_LEN];
	char buffer[128];


	DisplayPanel (Panels[MODULE_MESS_ERROR]);

	if(err_messages) {
	  sprintf (buffer, "%s    ", err_messages);
      SetCtrlVal (Panels[MODULE_MESS_ERROR], MESS_ERR_TEXTBOX, buffer);   
	 }
	sprintf (buffer, "prot_error=%8x at address=%8x  \n", prot_error,vme_addr );
    SetCtrlVal (Panels[MODULE_MESS_ERROR], MESS_ERR_TEXTBOX, buffer);   
	

return 0;
} // end function sisVME_ErrorHandling ()











/* --------------------------------------------------------------------------
   Messages System Handling
-------------------------------------------------------------------------- */

int write_system_messages (char *system_messages, int write_datetime)
{
	int CviErr;
	char buffer[128];
	static char *timeString, *dateString;

	if (write_datetime == PRINT_DATE_YES)  {
	   sprintf (buffer,"\n");
       SetCtrlVal (Panels[MODULE_MESS_SYSTEM], MESS_SYS_TEXTBOX, buffer);   
	   timeString=TimeStr ();	  //get system time pointer
	   dateString=DateStr ();	  // get system date pointer
	   CopyString (buffer, 0, STRING_DATE, 0, strlen(STRING_DATE));
	   CopyString (buffer, strlen(buffer), dateString, 0, 10);
	   CopyString (buffer, strlen(buffer), STRING_TIME, 0, strlen(STRING_TIME));
	   CopyString (buffer, strlen(buffer), timeString, 0, 8);
       SetCtrlVal (Panels[MODULE_MESS_SYSTEM], MESS_SYS_TEXTBOX, buffer);   
	   sprintf (buffer,"\n");
       SetCtrlVal (Panels[MODULE_MESS_SYSTEM], MESS_SYS_TEXTBOX, buffer);   

    }

	sprintf (buffer,"-- %s", system_messages);
	SetCtrlVal (Panels[MODULE_MESS_SYSTEM], MESS_SYS_TEXTBOX, buffer);   

//	if(err_messages) {
//	  sprintf (buffer, "%s    ", err_messages);
//      SetCtrlVal (Panels[MODULE_MESS_ERROR], MESS_ERR_TEXTBOX, buffer);   
//	 }
//	sprintf (buffer, "prot_error=%8x at address=%8x  \n", prot_error,vme_addr );
//    SetCtrlVal (Panels[MODULE_MESS_ERROR], MESS_ERR_TEXTBOX, buffer);   
	

return 0;
} // end function sisVME_ErrorHandling ()










