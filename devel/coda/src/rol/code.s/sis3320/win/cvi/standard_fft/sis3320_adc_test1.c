/***************************************************************************/
/*                                                                         */
/*  Filename: sis3320_adc_test1.c                                          */
/*                                                                         */
/*  Funktion: SIS3320 ADC  Test                                            */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 25.04.2005                                       */
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

//
//--------------------------------------------------------------------------- 
// Include files                                                              
//--------------------------------------------------------------------------- 

#include <windows.h>
#include <formatio.h>
#include <cvirte.h>     
#include <userint.h>
#include <stdio.h> 
#include <math.h> 
#include <utility.h> 
#include <ansi_c.h>
#include <stdio.h>

#include "sis3320_main_uir.h"
#include "sis3320_global.h" 	// Globale Defines fuers Programm

 
#include "vme_interface_defines.h"


#include "sis3320.h"




//--------------------------------------------------------------------------- 
// Local DEFINES (to be put into Include File)                                                              
//--------------------------------------------------------------------------- 




//int ConfigurationSetupAdc (void) ;
int sis3320_write_dac_offset(unsigned int module_addr, unsigned int* offset_value_array)  ;

int displayRunEventInformations (void) ;
int RunTest1EventAquisition (void)   ;

int Get_ADC_values_of_DAC_Ramp (void) ;
int Display_ADC_values_of_DAC_Ramp (unsigned int plot_size)  ;
 
//--------------------------------------------------------------------------- 
// Root Menu Call Back
//--------------------------------------------------------------------------- 



void CVICALLBACK CB_OpenPanelAdcTest1Menue (int menuBar, int menuItem, void *callbackData,
        int panel)
{
	int i ;


			gl_uint_Test1AdcGain[0] = 0 ;
			gl_uint_Test1AdcGain[1] = 0 ;
			gl_uint_Test1AdcGain[2] = 0 ;
			gl_uint_Test1AdcGain[3] = 0 ;
			gl_uint_Test1AdcGain[4] = 0 ;
			gl_uint_Test1AdcGain[5] = 0 ;
			gl_uint_Test1AdcGain[6] = 0 ;
			gl_uint_Test1AdcGain[7] = 0 ;
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_1, gl_uint_Test1AdcGain[0]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_2, gl_uint_Test1AdcGain[1]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_3, gl_uint_Test1AdcGain[2]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_4, gl_uint_Test1AdcGain[3]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_5, gl_uint_Test1AdcGain[4]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_6, gl_uint_Test1AdcGain[5]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_7, gl_uint_Test1AdcGain[6]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_8, gl_uint_Test1AdcGain[7]);

			
			gl_uint_Test1AdcOffset[0] = 38144 ;
			gl_uint_Test1AdcOffset[1] = 38144 ;
			gl_uint_Test1AdcOffset[2] = 38144 ;
			gl_uint_Test1AdcOffset[3] = 38144 ;
			gl_uint_Test1AdcOffset[4] = 38144 ;
			gl_uint_Test1AdcOffset[5] = 38144 ;
			gl_uint_Test1AdcOffset[6] = 38144 ;
			gl_uint_Test1AdcOffset[7] = 38144 ;
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_1, gl_uint_Test1AdcOffset[0]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_2, gl_uint_Test1AdcOffset[1]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_3, gl_uint_Test1AdcOffset[2]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_4, gl_uint_Test1AdcOffset[3]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_5, gl_uint_Test1AdcOffset[4]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_6, gl_uint_Test1AdcOffset[5]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_7, gl_uint_Test1AdcOffset[6]);
		    SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_8, gl_uint_Test1AdcOffset[7]);

			
			gl_uint_Test1ClockMode = 0 ;
 		  	SetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_MODE_CLK,gl_uint_Test1ClockMode);

			
			gl_uint_Test1InternalTriggerEnableFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_INTERNAL_STO, gl_uint_Test1InternalTriggerEnableFlag);  

			gl_uint_Test1LemoInStartStopEnableFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_LEMO_IN_SS,gl_uint_Test1LemoInStartStopEnableFlag);  
	
			gl_uint_Test1AutostartEnableFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_AUTOSTART, gl_uint_Test1AutostartEnableFlag);  
			


			

			gl_uint_Test1DisplayRawFlag = 1 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_RAW,gl_uint_Test1DisplayRawFlag);  

			if (gl_uint_Test1DisplayRawFlag == 0) { //off
            	HidePanel(Panels[TEST1_RAW]); //Clear Panel 
			  }
			else { // on
				openPanelTest1Raw() ;
			}


			gl_uint_Test1DisplayHistoFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_HISTO,gl_uint_Test1DisplayHistoFlag);  
       		HidePanel(Panels[TEST1_HISTO]); //Clear Panel 

			gl_uint_Test1DisplayFftFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_FFT,gl_uint_Test1DisplayFftFlag);  
       		HidePanel(Panels[TEST1_FFT]); //Clear Panel 

			gl_uint_Test1DisplayLongTermFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_LONG_T,gl_uint_Test1DisplayLongTermFlag);  
       		HidePanel(Panels[TEST1_LTERM]); //Clear Panel 

			gl_uint_Test1AutoClearHistogramFlag = 1 ;
			SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_CHECKBOX_CLR_HISTO_FL,gl_uint_Test1AutoClearHistogramFlag);  


			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_START_DELAY, 0x0) ;
	 		SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_STOP_DELAY, 0x0)  ;


			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_STOP_LENGTH,1);  
	 		SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, 0x400) ;



			gl_uint_Test1StartDelay = 0x0 ;
 			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_START_DELAY, gl_uint_Test1StartDelay) ;
			gl_uint_Test1StopDelay = 0x200 ;
	 		SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_STOP_DELAY, gl_uint_Test1StopDelay)  ;


			gl_uint_Test1SampleCountStopFlag = 1 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_STOP_LENGTH,gl_uint_Test1SampleCountStopFlag);  

			gl_uint_Test1MaxSampleCount = 0x400 ;
	 		SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, gl_uint_Test1MaxSampleCount) ;
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, ATTR_DIMMED, 0);


			gl_uint_Test1PageWrapFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_WRAP_PAGE,gl_uint_Test1PageWrapFlag);  

			gl_PageWrapMode        = 0x7 ;	   // 
			gl_EventPageWrapLength = 0x400 ;   // 1 K
 		  	SetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_WRAP_PAGE,gl_PageWrapMode); //  
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_RING_WRAP_PAGE, ATTR_DIMMED, 1);


			if (gl_uint_Test1SampleCountStopFlag == 1) {
				gl_adcEventSampleReadoutLength = gl_uint_Test1MaxSampleCount ;	   // 
			}
			else {
				gl_adcEventSampleReadoutLength = gl_EventPageWrapLength ;	   // 
			}



			gl_InputTestMode        = 0x0 ;	   // 
 		  	SetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_TEST_MODE, gl_InputTestMode);
	    	
	    										   
	    	
	    	
	    	
	    	SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PEAK, 0x10);
	    	SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_GAP, 0x10);
	    	SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PULSE_LEN, 0x10);
	    	SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_THRESHOLD, 0xFF00);
	  		SetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_TRIGGER,1); // LT


			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_DIMMED, 1);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_VISIBLE, 1);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STOPBUTTOM, ATTR_VISIBLE, 0);
			gl_uint_Test1RunStopStatusFlag = 0 ;

 	        

 	        
 	        
 	        
 	        
   
 	        SetPanelPos(Panels[TEST1_MENUE],DEFINE_PANEL_TEST1_MENU_CTRL_POS_Y,DEFINE_PANEL_TEST1_MENU_CTRL_POS_X)  ;   // y, x

		

		
		//	SetCtrlVal (Panels[TEST1_MENUE], DIR_MENU_GET_AUDIO_FREQ, 1000);


			
			DisplayPanel (Panels[TEST1_MENUE]);



	return ;
}

   

/* close Panels */

int CVICALLBACK CB_ClosePanelAdcTest1Menue (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			gl_uint_Test1DisplayRawFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_RAW,0x0);  

			gl_uint_Test1DisplayHistoFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_HISTO,0x0);  

			gl_uint_Test1DisplayFftFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_FFT,0x0);  

       		HidePanel(Panels[TEST1_RAW]); //Clear Panel 
       		HidePanel(Panels[TEST1_HISTO]); //Clear Panel 
       		HidePanel(Panels[TEST1_FFT]); //Clear Panel 
            
            HidePanel(Panels[TEST1_MENUE]); //Clear Panel 
			break;
		}
	return 0;
}





 int CVICALLBACK CB_CheckBoxTest1Display (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int check_on ;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{

//	GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_STOP_LENGTH,&gl_uint_Test1DisplayRawFlag);  
//	GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_WRAP_PAGE,&gl_uint_Test1DisplayRawFlag);  
		  // raw
			  case TEST1_MENU_CHECKBOX_DISPL_RAW:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_RAW,&gl_uint_Test1DisplayRawFlag);  
				if (gl_uint_Test1DisplayRawFlag == 0) { //off
            		HidePanel(Panels[TEST1_RAW]); //Clear Panel 
				  }
				  else { // on
					openPanelTest1Raw() ;
				  }
				break ;			


		  // Histogram
			  case TEST1_MENU_CHECKBOX_DISPL_HISTO:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_HISTO,&gl_uint_Test1DisplayHistoFlag);  
				if (gl_uint_Test1DisplayHistoFlag == 0) { //off
            		HidePanel(Panels[TEST1_HISTO]); //Clear Panel 
				  }
				  else { // on
					openPanelTest1Histo() ;
				  }
				break ;			

		  // FFT 
			  case TEST1_MENU_CHECKBOX_DISPL_FFT:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_FFT,&gl_uint_Test1DisplayFftFlag);  
				if (gl_uint_Test1DisplayFftFlag == 0) { //off
            		HidePanel(Panels[TEST1_FFT]); //Clear Panel 
				  }
				  else { // on
					openPanelTest1FFT() ;
				  }
				break ;			

		  // Long_term 
			  case TEST1_MENU_CHECKBOX_DISPL_LONG_T:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_LONG_T,&gl_uint_Test1DisplayLongTermFlag);  
				if (gl_uint_Test1DisplayLongTermFlag == 0) { //off
            		HidePanel(Panels[TEST1_LTERM]); //Clear Panel 
				  }
				  else { // on
					openPanelTest1LongTerm() ;
				  }
				break ;			

			} // end switch (control)

			break;
		}
	return 0;
}
								 



  

int CVICALLBACK CB_CheckBoxTest1Sample (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{

		case EVENT_COMMIT:
  			switch (control)
			{

			  case TEST1_MENU_CHECKBOX_INTERNAL_STO:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_INTERNAL_STO,&gl_uint_Test1InternalTriggerEnableFlag);  
				break ;			
			  case TEST1_MENU_CHECKBOX_LEMO_IN_SS:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_LEMO_IN_SS,&gl_uint_Test1LemoInStartStopEnableFlag);  
				break ;			
			  case TEST1_MENU_CHECKBOX_AUTOSTART:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_AUTOSTART,&gl_uint_Test1AutostartEnableFlag);  
				break ;			

		  
		  
		  //  
			  case TEST1_MENU_CHECKBOX_STOP_LENGTH:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_STOP_LENGTH,&gl_uint_Test1SampleCountStopFlag);  
				if (gl_uint_Test1SampleCountStopFlag == 0) { //off
					SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, ATTR_DIMMED, 1);
				  }
				  else { // on
					SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, ATTR_DIMMED, 0);
				  }
				break ;			

				case TEST1_MENU_CHECKBOX_WRAP_PAGE:
				GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_WRAP_PAGE,&gl_uint_Test1PageWrapFlag);  
				if (gl_uint_Test1PageWrapFlag == 0) { //off
					SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_RING_WRAP_PAGE, ATTR_DIMMED, 1);
				  }
				  else { // on
					SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_RING_WRAP_PAGE, ATTR_DIMMED, 0);
				  }
				break ;			
} // end switch (control)

			break;
		}
	return 0;
}

#ifdef raus
#endif









/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */

int CVICALLBACK CB_ADC_Sampling_Parameter (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case TEST1_MENU_ADC_START_DELAY:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_START_DELAY, &gl_uint_Test1StartDelay);
					break;
				case TEST1_MENU_ADC_STOP_DELAY:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_STOP_DELAY, &gl_uint_Test1StopDelay);
					break;
				case TEST1_MENU_ADC_EVENT_LENGTH:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_EVENT_LENGTH, &gl_uint_Test1MaxSampleCount);
					break;
			} // end switch (control)

			break;
		}
	return 0;
}

/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */

int CVICALLBACK CB_ADC_Trigger_Parameter (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int temp;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case TEST1_MENU_TRIGGER_PEAK:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PEAK, &temp);
					break;
				case TEST1_MENU_TRIGGER_GAP:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_GAP, &temp);
					break;
				case TEST1_MENU_TRIGGER_PULSE_LEN:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PULSE_LEN, &temp);
					break;
				case TEST1_MENU_TRIGGER_THRESHOLD:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_THRESHOLD, &temp);
					break;
			} // end switch (control)

			break;
		}
	return 0;
}
  

/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_TriggerMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int temp;
	switch (event)
	{
		case EVENT_COMMIT:
 		  	GetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_TRIGGER,&temp);
		//	SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_DIMMED, 1);
		  break;
	}
	return 0;
} // end function  ()





/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */

int CVICALLBACK CB_InputTestMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int temp;
	switch (event)
		{
		case EVENT_COMMIT:
 		  	GetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_TEST_MODE,&temp);
			switch (temp) {
		  		case  0: //  
					gl_InputTestMode        = 0x0 ;	   // 
 					break;
		  		case  1: //  
					gl_InputTestMode        = 0x10000 ;	   // 
 					break;

			} // end switch 		
			break;
		}
	return 0;
}


/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_PageWrapMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int temp;
	switch (event)
	{
		case EVENT_COMMIT:
 		  	GetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_WRAP_PAGE,&temp);
			switch (temp) {
		  		case  0: //  
					gl_PageWrapMode        = 0x0 ;	   // 
					gl_EventPageWrapLength = 0x1000000 ;   // 16 M
 					break;
		  		case  1: //  
					gl_PageWrapMode        = 0x1 ;	   // 
					gl_EventPageWrapLength = 0x400000 ;   // 4 M
 					break;
		  		case  2: //  
					gl_PageWrapMode        = 0x2 ;	   // 
					gl_EventPageWrapLength = 0x100000 ;   // 1 M
 					break;
		  		case  3: //  
					gl_PageWrapMode        = 0x3 ;	   // 
					gl_EventPageWrapLength = 0x40000 ;   // 256 K
 					break;
		  		case  4: //  
					gl_PageWrapMode        = 0x4 ;	   // 
					gl_EventPageWrapLength = 0x10000 ;   // 64 K
 					break;
		  		case  5: //  
					gl_PageWrapMode        = 0x5 ;	   // 
					gl_EventPageWrapLength = 0x4000 ;   // 16 K
 					break;
		  		case  6: //  
					gl_PageWrapMode        = 0x6 ;	   // 
					gl_EventPageWrapLength = 0x1000 ;   // 4 K
 					break;
		  		case  7: //  
					gl_PageWrapMode        = 0x7 ;	   // 
					gl_EventPageWrapLength = 0x400 ;   // 1 K
 					break;
		  		case  8: //  
					gl_PageWrapMode        = 0x8 ;	   // 512
					gl_EventPageWrapLength = 0x200 ;   // 
 					break;
		  		case  9: //  
					gl_PageWrapMode        = 0x9 ;	   // 256
					gl_EventPageWrapLength = 0x100 ;   // 
 					break;
		  		case  10: //  
					gl_PageWrapMode        = 0xA ;	   // 128
					gl_EventPageWrapLength = 0x80 ;	   // 
 					break;
		  		case  11: //  
					gl_PageWrapMode        = 0xB ;	   //  64
					gl_EventPageWrapLength = 0x40 ;	   // 
 					break;
			} // end switch 		

		//	SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_DIMMED, 1);
		  break;
	}
	return 0;
} // end function  ()







/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */

int CVICALLBACK CB_ADC_Gain (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case TEST1_MENU_ADC_GAIN_1:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_1, &gl_uint_Test1AdcGain[0]);
					break;
				case TEST1_MENU_ADC_GAIN_2:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_2, &gl_uint_Test1AdcGain[1]);
					break;
				case TEST1_MENU_ADC_GAIN_3:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_3, &gl_uint_Test1AdcGain[2]);
					break;
				case TEST1_MENU_ADC_GAIN_4:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_4, &gl_uint_Test1AdcGain[3]);
					break;
				case TEST1_MENU_ADC_GAIN_5:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_5, &gl_uint_Test1AdcGain[4]);
					break;
				case TEST1_MENU_ADC_GAIN_6:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_6, &gl_uint_Test1AdcGain[5]);
					break;
				case TEST1_MENU_ADC_GAIN_7:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_7, &gl_uint_Test1AdcGain[6]);
					break;
				case TEST1_MENU_ADC_GAIN_8:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_GAIN_8, &gl_uint_Test1AdcGain[7]);
					break;
			} // end switch (control)

			break;
		}
	return 0;
}


int CVICALLBACK CB_ADC_Offset (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case TEST1_MENU_ADC_OFFSET_1:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_1, &gl_uint_Test1AdcOffset[0]);
					break;
				case TEST1_MENU_ADC_OFFSET_2:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_2, &gl_uint_Test1AdcOffset[1]);
					break;
				case TEST1_MENU_ADC_OFFSET_3:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_3, &gl_uint_Test1AdcOffset[2]);
					break;
				case TEST1_MENU_ADC_OFFSET_4:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_4, &gl_uint_Test1AdcOffset[3]);
					break;
				case TEST1_MENU_ADC_OFFSET_5:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_5, &gl_uint_Test1AdcOffset[4]);
					break;
				case TEST1_MENU_ADC_OFFSET_6:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_6, &gl_uint_Test1AdcOffset[5]);
					break;
				case TEST1_MENU_ADC_OFFSET_7:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_7, &gl_uint_Test1AdcOffset[6]);
					break;
				case TEST1_MENU_ADC_OFFSET_8:
				    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_ADC_OFFSET_8, &gl_uint_Test1AdcOffset[7]);
					break;
			} // end switch (control)


			break;
		}
	return 0;
}


/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_ClockMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		// Den Clock-Mode einlesen
 		  	GetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_MODE_CLK,&gl_uint_Test1ClockMode);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_DIMMED, 1);
		  break;
	}
	return 0;
} // end function  ()





 int CVICALLBACK CB_CheckBoxHisto (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int check_on ;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{

		  // raw

		  // Clear Histo,m 
			  case T1_DIAGHIS_CHECKBOX_CLR_HISTO_FL:
				GetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_CHECKBOX_CLR_HISTO_FL,&gl_uint_Test1AutoClearHistogramFlag);  
				break ;			


			} // end switch (control)

			break;
		}
	return 0;
}
								 

/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */


int CVICALLBACK CB_Key_Commands (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int i, error, temp;
	unsigned int return_code;
	unsigned int data, addr;

	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
			  case TEST1_MENU_KEY_START:

				addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_START  ;
				if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
					sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 					return -1;
				}
				break ;			
			  case TEST1_MENU_KEY_STOP:

				addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_STOP  ;
				if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
					sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 					return -1;
				}
				break ;			


			} // end switch (control)


			break;
		}
	return 0;
}






/* --------------------------------------------------------------------------
   CB-Funktion 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_ConfigAdc (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int error;
	unsigned int addr;
	switch (event)
	{
		case EVENT_COMMIT:
 			ConfigurationSetupAdc ()    ;
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_DIMMED, 0);
		break;
	}
	return 0;
} // end function  ()



/* --------------------------------------------------------------------------
   SIS3320 Configuration 
-------------------------------------------------------------------------- */
int ConfigurationSetupAdc (void)
{
	unsigned int i, error, temp;
	unsigned int return_code;
	unsigned int data, addr;




        /*  Reset SIS330x  */
		addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_RESET   ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}



//set Clock source
 			switch (gl_uint_Test1ClockMode)				
			{
			  case  0: //  
				data = SIS3320_ACQ_SET_CLOCK_TO_200MHZ ;	   // 
 				break;
			  case  1: // intern 100 MHz
				data = SIS3320_ACQ_SET_CLOCK_TO_100MHZ ;	   // 
 				break;
			  case  2: // intern 50 MHz
				data = SIS3320_ACQ_SET_CLOCK_TO_50MHZ ;	   // 
 				break;
			  case  6: // intern 25 MHz
				data = SIS3320_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN ;	   // 
 				break;
			  case  7: //  
				data = SIS3320_ACQ_SET_CLOCK_TO_P2_CLOCK_IN;	   // 
 				break;
			} // end switch (uintClockMode)			



		
//		data =    data 
//		       +  SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER ;
		

 		if (gl_uint_Test1LemoInStartStopEnableFlag == 1) { 
 			data = data + SIS3320_ACQ_ENABLE_LEMO_START_STOP ; 
 		}
		else {
 			data = data + SIS3320_ACQ_DISABLE_LEMO_START_STOP ; 
 		}

 		if (gl_uint_Test1InternalTriggerEnableFlag == 1) { 
 			data = data + SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER ; 
 		}
		else {
 			data = data + SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER ; 
 		}

 		if (gl_uint_Test1AutostartEnableFlag == 1) { 
 			data = data + SIS3320_ACQ_ENABLE_AUTOSTART ; 
 		}
		else {
 			data = data + SIS3320_ACQ_DISABLE_AUTOSTART ; 
 		}

		
		
		addr = gl_uint_ModAddrConf[0] + SIS3320_ACQUISTION_CONTROL  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}











		// Start Delay
		addr = gl_uint_ModAddrConf[0] + SIS3320_START_DELAY  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,gl_uint_Test1StartDelay )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		// Stop Delay
		addr = gl_uint_ModAddrConf[0] + SIS3320_STOP_DELAY  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,gl_uint_Test1StopDelay )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}



		// Event Length
	 	data = gl_uint_Test1MaxSampleCount - 1 ;
		addr = gl_uint_ModAddrConf[0] + SIS3320_SAMPLE_LENGTH_ALL_ADC  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		// sample start Address
		data = 0 ;
		addr = gl_uint_ModAddrConf[0] + SIS3320_SAMPLE_START_ADDRESS_ALL_ADC  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}








		// Event Config 
		data = 0 ;
		if (gl_uint_Test1SampleCountStopFlag == 1) { data = data + 0x20 ; }
 		if (gl_uint_Test1PageWrapFlag == 1) { data = data + 0x10 ; }
		data = data + (gl_PageWrapMode & 0xf);
		
		addr = gl_uint_ModAddrConf[0] + SIS3320_EVENT_CONFIG_ALL_ADC  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		if (gl_uint_Test1SampleCountStopFlag == 1) {
			gl_adcEventSampleReadoutLength = gl_uint_Test1MaxSampleCount ;	   // 
		}
		else {
			gl_adcEventSampleReadoutLength = gl_EventPageWrapLength ;	   // 
		}


		// Input Test Mode
		data = gl_InputTestMode;
		addr = gl_uint_ModAddrConf[0] + SIS3320_ADC_INPUT_MODE_ALL_ADC  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}






		// Trigger Setup
	    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PEAK, &temp);
		data = temp & 0x1f ;
		
	    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_GAP, &temp);
		data = data + ( (temp & 0x1f) << 8 )  ;
		
	    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_PULSE_LEN, &temp);
		data = data + ( (temp & 0xff) << 16 )  ;

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC1  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC2  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC3  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC4  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC5  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC6  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC7  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_SETUP_ADC8  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
		
		
		// Trigger Threshold
	    GetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_TRIGGER_THRESHOLD, &temp);
		data = temp & 0x1ffff ;

	  	GetCtrlVal (Panels[TEST1_MENUE],TEST1_MENU_RING_TRIGGER,&temp);
 		data = data + ( (temp & 0x3) << 24) ;

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC1  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC2  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC3  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC4  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC5  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC6  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC7  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_TRIGGER_THRESHOLD_ADC8  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}



		data =    ( (gl_uint_Test1AdcGain[0] & 0x1) )
		 		+ ( (gl_uint_Test1AdcGain[1] & 0x1) << 1 )
		 		+ ( (gl_uint_Test1AdcGain[2] & 0x1) << 2 )
		 		+ ( (gl_uint_Test1AdcGain[3] & 0x1) << 3 )
		 		+ ( (gl_uint_Test1AdcGain[4] & 0x1) << 4 )
		 		+ ( (gl_uint_Test1AdcGain[5] & 0x1) << 5 )
		 		+ ( (gl_uint_Test1AdcGain[6] & 0x1) << 6 )
		 		+ ( (gl_uint_Test1AdcGain[7] & 0x1) << 7 )   ;
		
		addr = gl_uint_ModAddrConf[0] + SIS3320_ADC_GAIN_CONTROL  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0]  ;
		if ((error = sis3320_write_dac_offset(addr ,gl_uint_Test1AdcOffset )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis3320_write_dac_offset"); 
 			return -1;
		}


 	
 	
 	
 	
 	return 0x0 ;

	
} // end function  ()






/*******************************************************************************************************/

/* --------------------------------------------------------------------------
   SIS9300 Offset 
   module_addr			    vme module base address
   offset Value_array		DAC offset value (16 bit)
-------------------------------------------------------------------------- */
int sis3320_write_dac_offset(unsigned int module_addr, unsigned int *offset_value_array)
{
	unsigned int i, error, temp;
	unsigned int return_code;
	unsigned int data, addr, base_addr;
	unsigned int max_timeout, timeout_cnt;


	for (i=0;i<8;i++) {	

		data =  offset_value_array[7-i] ;
		addr = gl_uint_ModAddrConf[0] + SIS3320_DAC_DATA  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		data =  1 ; // write to DAC Register
		addr = gl_uint_ModAddrConf[0] + SIS3320_DAC_CONTROL_STATUS  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		max_timeout = 5000 ;
		timeout_cnt = 0 ;
		addr = gl_uint_ModAddrConf[0] + SIS3320_DAC_CONTROL_STATUS  ;
		do {
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4, &data)) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Read"); 
				return -1;
			}
			timeout_cnt++;
		} while ( ((data & 0x8000) == 0x8000) && (timeout_cnt <  max_timeout) )    ;

		if (timeout_cnt >=  max_timeout) {
			return -2 ;
		}


		
		data =  2 ; // Load DACs 
		addr = gl_uint_ModAddrConf[0] + SIS3320_DAC_CONTROL_STATUS  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
			return -1;
		}
		timeout_cnt = 0 ;
		addr = gl_uint_ModAddrConf[0] + SIS3320_DAC_CONTROL_STATUS  ;
		do {
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4, &data)) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Read"); 
				return -1;
			}
			timeout_cnt++;
		} while ( ((data & 0x8000) == 0x8000) && (timeout_cnt <  max_timeout) )    ;

		if (timeout_cnt >=  max_timeout) {
			return -3 ;
		}


	}



   
	return 0x0 ;
																																 
} // end function  ()






/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/







int CVICALLBACK CB_StartAcq (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr, i,j;
	int status;
    unsigned short data[1024] ;
	switch (event)
		{
		case EVENT_COMMIT:


			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_VISIBLE, 0);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STOPBUTTOM, ATTR_VISIBLE, 1);
			gl_uint_Test1RunStopStatusFlag = 1 ;

			gl_unit_Test1RunOneSecondCounter = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_RUN_EVENT_SECOND, gl_unit_Test1RunOneSecondCounter);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_ONE_SECOND_TICK, ATTR_ENABLED, ON);

			RunTest1EventAquisition() ;    



			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_ONE_SECOND_TICK, ATTR_ENABLED, OFF);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_VISIBLE, 1);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STOPBUTTOM, ATTR_VISIBLE, 0);
			gl_uint_Test1RunStopStatusFlag = 0 ;

#ifdef raus
			gl_uint_WaveformArrayLength =    gl_uint_Memory_Data_Format_EventSize / 2 ;
			gl_uint_ModuleArrayLength   =    (gl_uint_WaveformArrayLength * 8 ) + 3   ;
			gl_uint_EventArrayLength    =    gl_uint_Memory_Data_Format_NofModules * gl_uint_ModuleArrayLength ;

			gl_uint_EventPageAddrMask   =    gl_uint_Memory_Data_Format_EventSize - 1    ;

		  	SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_CHECKBOX_WR_DATA, ATTR_DIMMED, 1);
		  	if (gl_uint_RunCheckWrDataToFileFlag == 1)  {  // on
   	 			OpenEventDataFile()  ;    
	   	   	}
			


			gl_uint_system_status = SYSTEM_STATUS_MODULES_RUNNING ;
            CheckSystemStatus () ;


			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STARTBUTTOM, ATTR_VISIBLE, 0);
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STOPBUTTOM, ATTR_VISIBLE, 1);
			gl_uint_RunStopStatusFlag = 1 ;

//            DisableTaskSwitching();
//             SetPanelAttribute (Panels[RUN_CONTROL], ATTR_WINDOW_ZOOM, VAL_MAXIMIZE);

		
			// start one second timer
			gl_uint_RunEventOldCounter  = 0 ;
   			gl_uint_RunEventCounter = 0 ;
			gl_uint_DataEvent_LWordCounter = 0 ;
			SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_RATE, 0.0);
			SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_RATE_A, 0.0);
			SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_MBYTE, 0.0);
			SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_NUMBER, gl_uint_RunEventCounter);
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_ONE_SECOND_TICK, ATTR_ENABLED, ON);

 
		// start
			if (gl_uint_Memory_Data_Format_Mode == MEMORY_DATA_FORMAT_MODE_SINGLE_EVENT)  { /* Single Event ; Single Bank */
				SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_EVENT_NUMBER,1);			   // always Event 1
				SetCtrlAttribute (Panels[DIAG_RAW], DIAGPANEL3_EVENT_NUMBER, ATTR_DIMMED, 1);
				RunSingleEventAquisition() ;    
			 }
			 else {  /* Multi Event ; Switch Bank ; Autostart ; MEMORY_DATA_FORMAT_MODE_MULTI_EVENT */
				SetCtrlAttribute (Panels[DIAG_RAW], DIAGPANEL3_EVENT_NUMBER, ATTR_DIMMED, 0);
         		RunMultiEventSwitchBankAquisition() ;  
			 } 
		   
		  // disable second timer
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_ONE_SECOND_TICK, ATTR_ENABLED, OFF);
		  // cloese data file
		    if (gl_uint_RunCheckWrDataToFileFlag == 1)  {  // on
   	 		  CloseEventDataFile()  ;    
	   	     }

		    AbortAutoBankSwitchAquistion () ;
            SetUsrLedsOff () ;

		  // display Event Informaton
			displayRunEventInformations();
		  // disable stop buttom / enable start button (neccessary if stoped with MaxEventNumber 
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STARTBUTTOM, ATTR_VISIBLE, 1);
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STOPBUTTOM, ATTR_VISIBLE, 0);
			gl_uint_RunStopStatusFlag = 0 ;

		  	SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_CHECKBOX_WR_DATA, ATTR_DIMMED, 0);
			gl_uint_system_status = SYSTEM_STATUS_MODULES_CONFIGURED ;
            CheckSystemStatus () ;
#endif
			break;
		}
//            EnableTaskSwitching();
//            SetPanelAttribute (Panels[RUN_CONTROL], ATTR_WINDOW_ZOOM, VAL_NO_ZOOM);
 	return 0;
}




int CVICALLBACK CB_StopAcq (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr, i;
	int status;
	switch (event)
		{
		case EVENT_COMMIT:

			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STARTBUTTOM, ATTR_VISIBLE, 1);
			SetCtrlAttribute (Panels[TEST1_MENUE], TEST1_MENU_STOPBUTTOM, ATTR_VISIBLE, 0);
			gl_uint_Test1RunStopStatusFlag = 0 ;


#ifdef raus
//	if ((MenuBars[MAIN_MENU_BAR] = LoadMenuBar (Panels[MAIN], "sis3300_greta_uir.uir", MENU)) < 0) return -1;
// 	LoadMenuBar (Panels[MAIN], "sis3300_greta_uir.uir", MENU);
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STARTBUTTOM, ATTR_VISIBLE, 1);
			SetCtrlAttribute (Panels[RUN_CONTROL], RUN_CTRL_STOPBUTTOM, ATTR_VISIBLE, 0);
			gl_uint_RunStopStatusFlag = 0 ;
	 // wait for stop !!!
			gl_uint_system_status = SYSTEM_STATUS_MODULES_CONFIGURED ;  // prepared for stoping !!

            CheckSystemStatus () ;
#endif
			break;
		}
	return 0;
}
		  













int RunTest1EventAquisition (void)
{
//   int  event_i, module_i  ;
   int i ;
   int error  ;
   unsigned int  vme_am_mode, temp, return_code ;
   unsigned int  addr, data, data_rd ;
   unsigned int  dma_request_no_of_words, dma_got_no_of_words, pollcount ;
   unsigned int  raw_plot_size ;
   unsigned int  wrap_read_addr_offset, wrap_read_addr_mask, wrap_sample_offset ;
	
	unsigned short *adc1_pointer ;
	unsigned short *adc2_pointer ;
	unsigned short *adc3_pointer ;
	unsigned short *adc4_pointer ;
	unsigned short *adc5_pointer ;
	unsigned short *adc6_pointer ;
	unsigned short *adc7_pointer ;
	unsigned short *adc8_pointer ;
		   

// Readout Loop  */

	ConfigurationSetupAdc ();

	SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, 0x0, gl_adcEventSampleReadoutLength-1)  ;
	SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 4095)  ;

   
   vme_am_mode = 0x8 ; // MBLT64
   vme_am_mode = 0x9 ; // D32
   vme_am_mode = 0xF ; // BLT32
   do {

	  // start   Test sample
		
		addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_ARM  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_START  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}


	    pollcount=0;
		addr = gl_uint_ModAddrConf[0] + SIS3320_ACQUISTION_CONTROL  ;
		do {
     	 	pollcount++;
     	 	error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;
			if (error != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Read"); 
 				return -1;
			}
      		if (pollcount==100) {
       			ProcessSystemEvents ();    /* danach mus status  abgefragt werden */
       			pollcount=0;
      		 }

		
		}  while ( ( (data_rd & 0x10000 ) == 0x10000 ) && (gl_uint_Test1RunStopStatusFlag == 1) ); /*  */
		


		dma_request_no_of_words = gl_adcEventSampleReadoutLength/2  ;
		raw_plot_size  = gl_adcEventSampleReadoutLength ;


		if (gl_uint_Test1SampleCountStopFlag == 1) {  // Count Stop Mode
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, vme_am_mode , 4, 0, gl_uint_adc1_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC2_OFFSET, vme_am_mode , 4, 0, gl_uint_adc2_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC2_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC3_OFFSET, vme_am_mode , 4, 0, gl_uint_adc3_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC3_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC4_OFFSET, vme_am_mode , 4, 0, gl_uint_adc4_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC4_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC5_OFFSET, vme_am_mode , 4, 0, gl_uint_adc5_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC5_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC6_OFFSET, vme_am_mode , 4, 0, gl_uint_adc6_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC6_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC7_OFFSET, vme_am_mode , 4, 0, gl_uint_adc7_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC7_OFFSET, "sis1100w_Vme_Dma_Read");
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC8_OFFSET, vme_am_mode , 4, 0, gl_uint_adc8_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC8_OFFSET, "sis1100w_Vme_Dma_Read");

		
		}
		else {   // Wrap Mode

     	 // ADC1	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC1  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "ADC1"); 

			wrap_sample_offset =  ((data_rd  ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;

#ifdef raus			
			printf("\n");
			printf("dma_request_no_of_words    = 0x%08x     \n",dma_request_no_of_words);
			printf("data_rd                    = 0x%08x     \n",data_rd);
			printf("wrap_sample_offset         = 0x%08x     \n",wrap_sample_offset);
			printf("wrap_read_addr_offset      = 0x%08x     \n",wrap_read_addr_offset);
			printf("wrap_read_addr_mask        = 0x%08x     \n",wrap_read_addr_mask);
       			ProcessSystemEvents ();    /* danach mus status  abgefragt werden */
#endif  			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc1_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc1_data_array[i] =   gl_uint_adc1_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc1_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc1_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc1_data_array[i] =   gl_uint_adc1_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc1_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}
			
     	 // ADC2	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC2_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC2_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC2  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "ADC2"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc2_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc2_data_array[i] =   gl_uint_adc2_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc2_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc2_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc2_data_array[i] =   gl_uint_adc2_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc2_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
     	 // ADC3	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC3_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC3_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC3  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc3_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc3_data_array[i] =   gl_uint_adc3_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc3_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc3_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc3_data_array[i] =   gl_uint_adc3_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc3_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
     	 // ADC4	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC4_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC4_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC4  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc4_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc4_data_array[i] =   gl_uint_adc4_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc4_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc4_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc4_data_array[i] =   gl_uint_adc4_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc4_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
     	 // ADC5	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC5_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC5_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC5  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc5_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc5_data_array[i] =   gl_uint_adc5_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc5_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc5_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc5_data_array[i] =   gl_uint_adc5_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc5_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
     	 // ADC6	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC6_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC6_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC6  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc6_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc6_data_array[i] =   gl_uint_adc6_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc6_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc6_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc6_data_array[i] =   gl_uint_adc6_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc6_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
		
     	 // ADC7	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC7_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC7_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC7  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc7_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc7_data_array[i] =   gl_uint_adc7_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc7_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc7_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc7_data_array[i] =   gl_uint_adc7_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc7_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}

		
		
		
     	 // ADC8	
     		return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC8_OFFSET, vme_am_mode , 4, 0, gl_dma_rd_buffer, dma_request_no_of_words, &dma_got_no_of_words) ;
			if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC8_OFFSET, "sis1100w_Vme_Dma_Read");
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC8  ;
			sis1100w_Vme_Single_Read(&gl_sis1100_device, addr,0x9,4, &data_rd) ;

			//sisVME_ErrorHandling (data_rd, data_rd, "data_rd"); 

			wrap_sample_offset =  ((data_rd ) & 0x3); 
			wrap_read_addr_offset =  (data_rd & 0xfffffffc)/2 ;
			wrap_read_addr_mask   =  dma_request_no_of_words - 1  ;
			
			if(wrap_sample_offset == 3) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc8_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i - 1) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc8_data_array[i] =   gl_uint_adc8_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i ) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}

			if(wrap_sample_offset == 0) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc8_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask] ) & 0x0fff0fff;
				}
			}

			if(wrap_sample_offset == 1) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc8_data_array[i] =   ((gl_dma_rd_buffer[(wrap_read_addr_offset + i) & wrap_read_addr_mask]) >> 16) & 0x0fff ;
			   		gl_uint_adc8_data_array[i] =   gl_uint_adc8_data_array[i] 
			   		                             + (((gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) << 16) & 0x0fff0000);
				}
			}
			if(wrap_sample_offset == 2) {
				for (i=0;i<dma_request_no_of_words;i++) {
			   		gl_uint_adc8_data_array[i] = (gl_dma_rd_buffer[(wrap_read_addr_offset + i + 1) & wrap_read_addr_mask]) & 0x0fff0fff ;
				}
			}
		
		
		
		
		}



	  if (gl_uint_Test1DisplayRawFlag == 1) {   ; // on
	  	displayRawData(raw_plot_size)  ;
	  }

	  if (gl_uint_Test1DisplayHistoFlag == 1) {   ; // on
	      displayAdcHistograph () ; 
	  }

	  if (gl_uint_Test1DisplayFftFlag == 1) {   ; // on
	      displayAdcFFTgraph () ; 
	  }





    ProcessSystemEvents ();  
   }while (    (gl_uint_Test1RunStopStatusFlag == 1)  );
//            && ((gl_uint_RunEventCounter < gl_uint_RunMaxEventCounter ) || (gl_uint_RunCheckStopEventsFlag == 0) )
//            && ((gl_unit_RunOneSecondCounter < gl_uint_RunMaxSecondsCounter ) || (gl_uint_RunCheckStopTimeFlag == 0) ) ) ; 
	
   return 0;
} // end function  ()






int CVICALLBACK CB_ReadDisplayAdc (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
   int error  ;
   unsigned int  vme_am_mode, temp, return_code ;
   unsigned int  addr, data, data_rd ;
   unsigned int  dma_request_no_of_words, dma_got_no_of_words, pollcount ;
   unsigned int  raw_plot_size ;
	switch (event)
		{
		case EVENT_COMMIT:

   vme_am_mode = 0x8 ; // MBLT64
   vme_am_mode = 0x9 ; // D32
   vme_am_mode = 0xF ; // BLT32
		dma_request_no_of_words = gl_adcEventSampleReadoutLength/2  ;
		raw_plot_size  = gl_adcEventSampleReadoutLength ;
     	return_code =   sis1100w_Vme_Dma_Read(&gl_sis1100_device, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, vme_am_mode , 4, 0, gl_uint_adc1_data_array, dma_request_no_of_words, &dma_got_no_of_words) ;
		if (return_code != 0x0 ) sisVME_ErrorHandling (return_code, gl_uint_ModAddrConf[0] + SIS3320_ADC1_OFFSET, "sis1100w_Vme_Dma_Read");
	  	displayRawData(raw_plot_size)  ;

			break;
		}
	return 0;
}
































int CVICALLBACK CB_RunSecondTick (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
//	float  eventRate,  eventRateAverage, fileMByteSize ;
	switch (event)
		{
		case EVENT_TIMER_TICK:
			gl_unit_Test1RunOneSecondCounter ++   ;
			displayRunEventInformations();

   		//	gl_uint_RunEventOldCounter = gl_uint_RunEventCounter ;

			break;
		}
	return 0;
}


int displayRunEventInformations (void)
{   
	unsigned int temp;
//	float  eventRate,  eventRateAverage, fileMByteSize ;

			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_RUN_EVENT_SECOND, gl_unit_Test1RunOneSecondCounter);


#ifdef raus
		temp = gl_uint_RunEventCounter ;
		eventRate = (float) (temp) - (float)(gl_uint_RunEventOldCounter)	   ;
		gl_uint_RunEventOldCounter = temp ;
		if (gl_unit_RunOneSecondCounter == 0) {
		 	eventRateAverage = eventRate ;
		  }
		 else {
			eventRateAverage = (float) (gl_uint_RunEventCounter) / (float)( gl_unit_RunOneSecondCounter )   ;
		  }
		fileMByteSize =   ((((float) (gl_uint_DataEvent_LWordCounter))  * 4.0) /  1048576.0 ) ;  // 1MByte
//		fileMByteSize =   (((float) (gl_uint_DataEvent_LWordCounter))  / 250000.0 ) ;  // * 4 / 1000

		SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_NUMBER, gl_uint_RunEventCounter);
		SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_RATE, eventRate);
		SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_RATE_A, eventRateAverage);
		SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_SECOND, gl_unit_RunOneSecondCounter);
		SetCtrlVal (Panels[RUN_CONTROL], RUN_CTRL_RUN_EVENT_MBYTE, fileMByteSize);
#endif
  return 0;
}





int CVICALLBACK CB_TestDAC (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
 			Get_ADC_values_of_DAC_Ramp ()    ;
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, 0x0, 0x10000-1)  ;
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 4095)  ;
			Display_ADC_values_of_DAC_Ramp (0x10000) ;
			break;
		}
	return 0;
}


/* --------------------------------------------------------------------------
   SIS3320 Configuration 
-------------------------------------------------------------------------- */
int Get_ADC_values_of_DAC_Ramp (void)
{
	unsigned int i, i_dac_offset, error, temp;
	unsigned int return_code;
	unsigned int data, addr;
	unsigned short short_dummy;
        
        /*  Reset SIS330x  */
		addr = gl_uint_ModAddrConf[0] + SIS3320_KEY_RESET   ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,0x0 )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

		/* *set ADC Clock */
		data = SIS3320_ACQ_SET_CLOCK_TO_200MHZ ;	   // 
		addr = gl_uint_ModAddrConf[0] + SIS3320_ACQUISTION_CONTROL  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}

 	
		data = 0 ;	   // 
		addr = gl_uint_ModAddrConf[0] + SIS3320_ADC_GAIN_CONTROL  ;
		if ((error = sis1100w_Vme_Single_Write(&gl_sis1100_device, addr ,0x9,4,data )) != 0) { 
			sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 			return -1;
		}
 	

		for (i_dac_offset=0;i_dac_offset<0x10000;i_dac_offset++){
		
			// set DAC offset
			for (i=0;i<8;i++){
				gl_uint_Test1AdcOffset[i] = i_dac_offset ;
			}
			addr = gl_uint_ModAddrConf[0]  ;
			if ((error = sis3320_write_dac_offset(addr ,gl_uint_Test1AdcOffset )) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis3320_write_dac_offset"); 
 				return -1;
			}


			// ADC 1 + 2
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_VALUE_ADC12 ;
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4,&data )) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 				return -1;
			}
			short_dummy = (unsigned short) (data >> 16) ;
			gl_ushortDAC_Test_ADC_1_array[i_dac_offset] = short_dummy ;
			short_dummy = (unsigned short) data ;
			gl_ushortDAC_Test_ADC_2_array[i_dac_offset] = short_dummy ;
					
			// ADC 3 + 4
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_VALUE_ADC34 ;
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4,&data )) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 				return -1;
			}
			short_dummy = (unsigned short) (data >> 16) ;
			gl_ushortDAC_Test_ADC_3_array[i_dac_offset] = short_dummy ;
			short_dummy = (unsigned short) data ;
			gl_ushortDAC_Test_ADC_4_array[i_dac_offset] = short_dummy ;
					
			// ADC 5 + 6
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_VALUE_ADC34 ;
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4,&data )) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 				return -1;
			}
			short_dummy = (unsigned short) (data >> 16) ;
			gl_ushortDAC_Test_ADC_5_array[i_dac_offset] = short_dummy ;
			short_dummy = (unsigned short) data ;
			gl_ushortDAC_Test_ADC_6_array[i_dac_offset] = short_dummy ;
					
			// ADC 7 + 8
			addr = gl_uint_ModAddrConf[0] + SIS3320_ACTUAL_SAMPLE_VALUE_ADC34 ;
			if ((error = sis1100w_Vme_Single_Read(&gl_sis1100_device, addr ,0x9,4,&data )) != 0) { 
				sisVME_ErrorHandling (error, addr, "sis1100w_Vme_Single_Write"); 
 				return -1;
			}
			short_dummy = (unsigned short) (data >> 16) ;
			gl_ushortDAC_Test_ADC_7_array[i_dac_offset] = short_dummy ;
			short_dummy = (unsigned short) data ;
			gl_ushortDAC_Test_ADC_8_array[i_dac_offset] = short_dummy ;
					
	
		
		
		} // i_dac_offset

 	
 	
 	
 	return 0x0 ;

} // end function  ()





int Display_ADC_values_of_DAC_Ramp (unsigned int plot_size)
{
   int  event_i, module_i, channel_i   ;
   int  index   ;
//   unsigned int plot_size ;
//   plot_size = gl_EventPageWrapLength   ;
   
   
       	DeleteGraphPlot (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);



      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 1)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_1_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 2)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_2_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 3)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_3_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_GREEN);
		}
      	
      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 4)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_4_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_CYAN);
		}


      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 5)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_5_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_RED);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 6)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_6_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_BLUE);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 7)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_7_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_GREEN);
		}
      	
      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 8)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_ushortDAC_Test_ADC_8_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_CYAN);
		}


	return 0;
}










