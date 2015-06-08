/***************************************************************************/
/*                                                                         */
/*  Filename: sis3320_adc_test1_raw.c                                      */
/*                                                                         */
/*  Funktion: ADC Display routines                                         */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 25.06.2005                                       */
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
#include <analysis.h>
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


//--------------------------------------------------------------------------- 
// Local DEFINES (to be put into Include File)                                                              
//--------------------------------------------------------------------------- 





 
//--------------------------------------------------------------------------- 
// Root Menu Call Back
//--------------------------------------------------------------------------- 





// unsigned int  gl_uint_Test1DisplayRawFlag     ;

int DisableZoomCursorsRaw (void)
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;
   	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_X_POSITION, 0.0);
   	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_Y_POSITION, 0.0);

	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_XY_ZOOM_IN_RAW, ATTR_DIMMED, 1);
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_X_ZOOM_IN_RAW, ATTR_DIMMED, 1);
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_Y_ZOOM_IN_RAW, ATTR_DIMMED, 1);
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, ATTR_NUM_CURSORS, 0);

//	xmin =  (double) gl_unit_defaultGraphRawXmin ;
//	xmax =  (double) gl_unit_defaultGraphRawXmax ;
//	ymin =  (double) gl_unit_defaultGraphRawYmin ;
//	ymax =  (double) gl_unit_defaultGraphRawYmax ;
//	SetAxisScalingMode  (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, xmin, xmax)  ;
//	SetAxisScalingMode  (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, ymin, ymax)  ;
	return 0;
}




int EnableZoomCursorsRaw (void)
{
    int tempX, tempY;
    double x, y;
    double xmin;
    double xmax;
    double ymin;
    double ymax;

	GetAxisScalingMode  (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, &xmin, &xmax)  ;
	GetAxisScalingMode  (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, &ymin, &ymax)  ;
	
	
	tempX = (int) ((xmax - xmin) / 4) ;
	tempY = (int) ((ymax - ymin) / 4) ;

	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, ATTR_NUM_CURSORS, 3);

	// cursor 1
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CURSOR_ENABLED, 1); // enable cursor
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CURSOR_COLOR, VAL_DK_BLUE); 

	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CROSS_HAIR_STYLE, VAL_LONG_CROSS); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CURSOR_MODE, VAL_FREE_FORM); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CURSOR_YAXIS, VAL_LEFT_YAXIS); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, ATTR_CURSOR_POINT_STYLE, VAL_EMPTY_CIRCLE); //  

	if (tempX == 0) x = xmin ;
	          else  x = xmin + (double) (tempX )  ;
	if (tempY == 0) y = ymin ;
	          else  y = ymin + (double) (tempY * 3)  ;

   	SetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, x, y);


	
	// cursor 2
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CURSOR_ENABLED, 1); // enable cursor
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CURSOR_COLOR, VAL_DK_BLUE); 

	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CROSS_HAIR_STYLE, VAL_LONG_CROSS); //   
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CURSOR_MODE, VAL_FREE_FORM); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CURSOR_YAXIS, VAL_LEFT_YAXIS); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, ATTR_CURSOR_POINT_STYLE, VAL_EMPTY_CIRCLE); //  

	if (tempX == 0) x = xmin ;
	          else  x = xmin + (double) (tempX * 3)  ;
	if (tempY == 0) y = ymin ;
	          else  y = ymin + (double) (tempY)  ;
   	SetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, x, y);



	// cursor 3
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CURSOR_ENABLED, 1); // enable cursor
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CURSOR_COLOR, VAL_BLACK); 

	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CROSS_HAIR_STYLE, VAL_SHORT_CROSS); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CURSOR_MODE, VAL_SNAP_TO_POINT); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CURSOR_YAXIS, VAL_LEFT_YAXIS); //  
	SetCursorAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, ATTR_CURSOR_POINT_STYLE, VAL_DOTTED_EMPTY_CIRCLE); //  

	if (tempX == 0) x = xmin ;
	          else  x = xmin + (double) (tempX * 2)  ;
	if (tempY == 0) y = ymin ;
	          else  y = ymin + (double) (tempY * 2)  ;
   	SetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, x, y);


	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_XY_ZOOM_IN_RAW, ATTR_DIMMED, 0);
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_X_ZOOM_IN_RAW, ATTR_DIMMED, 0);
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_Y_ZOOM_IN_RAW, ATTR_DIMMED, 0);
	return 0;																					 

}
  




int openPanelTest1Raw(void) 
{
	

	gl_uint_Test1RawAdcShowChoose = 0 ;
  	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_RING_ADC_CHOOSE, gl_uint_Test1RawAdcShowChoose);

	
	SetPanelPos(Panels[TEST1_RAW],DEFINE_PANEL_TEST1_RAW_POS_Y,DEFINE_PANEL_TEST1_RAW_POS_X)  ;   // y, x
	DisplayPanel (Panels[TEST1_RAW]);

	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, ATTR_CTRL_MODE, VAL_INDICATOR); // only indicator 
	SetCtrlAttribute (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0)  ;   // disable  
	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_CHECKBOX_ZOOM,0);                               // disable   flag
	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_CHECKBOX_CVI_ZOOM,0);                            // disable CVI flag
	DisableZoomCursorsRaw() ;

   	DeleteGraphPlot (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
	return 0;
}
		  






// Call Backs

int CVICALLBACK CB_ClosePanelTest1Raw (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			gl_uint_Test1DisplayRawFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_RAW,0x0);  
            HidePanel(Panels[TEST1_RAW]); //Clear Panel 
			break;
		}
	return 0;
}






 
int CVICALLBACK CB_RawChooseAdcShow (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
 		  GetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_RING_ADC_CHOOSE,&gl_uint_Test1RawAdcShowChoose);
		//  displayRawData () ;
		  break;
	}
	return 0;
} // end function CB_ChooseAdcShow ()



 

int CVICALLBACK CB_RawModuleNumber (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr,i;
	switch (event)
	{
		case EVENT_COMMIT:
		// Den Access-Mode einlesen
 		//  CviErr = GetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_MODULE_NUMBER,&gl_unit_RawModuleNumber);
		//  displayRawData () ;
		  break;
	}
	return 0;
} // end function 



 		  
 		  
 		  


int CVICALLBACK CB_CheckRaw (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int test ;
	int check_on ;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
			  case T1_DIAGRAW_CHECKBOX_ZOOM:
				GetCtrlVal (Panels[TEST1_RAW],T1_DIAGRAW_CHECKBOX_ZOOM,&check_on);  //Show in panel Configuration
				if (check_on == 0) { //off
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_CTRL_MODE, VAL_INDICATOR); // only indicator 
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0);   // disable  
					SetCtrlVal (Panels[TEST1_RAW],T1_DIAGRAW_CHECKBOX_CVI_ZOOM,0);                            // disable CVI flag
					DisableZoomCursorsRaw() ;
				  }
				  else { // on
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_CTRL_MODE, VAL_HOT);     // and hot 
					EnableZoomCursorsRaw() ;
				  }
				break ;			

			  case T1_DIAGRAW_CHECKBOX_CVI_ZOOM:
				GetCtrlVal (Panels[TEST1_RAW],T1_DIAGRAW_CHECKBOX_CVI_ZOOM,&check_on);  //Show in panel Configuration
				if (check_on == 0) { //off
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0);  // disable  
				  }
				  else { // on
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 1);  // enable  
				    SetCtrlAttribute (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, ATTR_CTRL_MODE, VAL_HOT);     // and hot 
					SetCtrlVal (Panels[TEST1_RAW],T1_DIAGRAW_CHECKBOX_ZOOM,1);  //and enable general zoom
					EnableZoomCursorsRaw() ;
				  }
				break ;			
			} // end switch (control)

			break;
		}
#ifdef raus
#endif
	return 0;
}



int CVICALLBACK CB_MoveCursor_Raw (int panel, int control, int event,
                              void *callbackData, int eventData1,  int eventData2)
{
    double x;
    double x1, x2;
    double y;

    switch (event) {
        case EVENT_COMMIT:
        	GetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 3, &x, &y);
        	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_X_POSITION, x);
        	SetCtrlVal (Panels[TEST1_RAW], T1_DIAGRAW_Y_POSITION, y);

        	GetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, &x1, &y);
        	GetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, &x2, &y);

            break;

        case EVENT_RIGHT_CLICK:
            break;
        }
    return 0;
}







	  
int CVICALLBACK CB_ZoomIn (int panel, int control, int event, void *callbackData,
                        int eventData1, int eventData2)
{
    double x1;
    double x2;
    double y1;
    double y2;
    double temp;
	int i ;
    
    if (event == EVENT_COMMIT)
        {
        
        /* Get the current position of both cursors.  Notive that the        */
        /* position is relative to the Graph's coordinate system (your plots)*/
        GetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 1, &x1, &y1);
        GetGraphCursor (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, 2, &x2, &y2);
        
        /* Get positive range values, and rescale the Graph */
        if (x1 > x2)
            {
            temp = x1;
            x1 = x2;
            x2 = temp;
            }
        if (y1 > y2)
            {
            temp = y1;
            y1 = y2;
            y2 = temp;
            }

		switch (control)
		{
		  case T1_DIAGRAW_XY_ZOOM_IN_RAW:
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, x1, x2)  ;
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, y1, y2)  ;
			break;																						  
		  case T1_DIAGRAW_X_ZOOM_IN_RAW:
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, x1, x2)  ;
			break;
		  case T1_DIAGRAW_Y_ZOOM_IN_RAW:
			SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, y1, y2)  ;
			break;
		} // end switch (control)


				
        }
    return 0;
}
	  
	  
	  
	

int CVICALLBACK CB_Expand_Raw (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;

  	unsigned int gl_unit_defaultGraphRawXmin;
  	unsigned int gl_unit_defaultGraphRawXmax;
  	unsigned int gl_unit_defaultGraphRawYmin;
  	unsigned int gl_unit_defaultGraphRawYmax;

	gl_unit_defaultGraphRawXmin = 0x0 ;
	gl_unit_defaultGraphRawXmax = gl_adcEventSampleReadoutLength-1;
	gl_unit_defaultGraphRawYmin = 0x0 ;
	gl_unit_defaultGraphRawYmax = 4095 ;


	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
			  case T1_DIAGRAW_XY_EXPAND_RAW:
				xmin =  (double) gl_unit_defaultGraphRawXmin ;
				xmax =  (double) gl_unit_defaultGraphRawXmax ;
				ymin =  (double) gl_unit_defaultGraphRawYmin ;
				ymax =  (double) gl_unit_defaultGraphRawYmax ;
				SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, xmin, xmax)  ;
				SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, ymin, ymax)  ;
				break;																						  
			  case T1_DIAGRAW_X_EXPAND_RAW:
				xmin =  (double) gl_unit_defaultGraphRawXmin ;
				xmax =  (double) gl_unit_defaultGraphRawXmax ;
				SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_XAXIS, VAL_MANUAL, xmin, xmax)  ;
				break;
			  case T1_DIAGRAW_Y_EXPAND_RAW:
				ymin =  (double) gl_unit_defaultGraphRawYmin ;
				ymax =  (double) gl_unit_defaultGraphRawYmax ;
				SetAxisScalingMode  (Panels[TEST1_RAW],T1_DIAGRAW_GRAPH_RAW, VAL_LEFT_YAXIS, VAL_MANUAL, ymin, ymax)  ;
				break;
			} // end switch (control)

			break;
		}
#ifdef raus
#endif
	return 0;
}

			  
    

 
  
  
  
  
  
 
int displayRawData (unsigned int plot_size)
{
   int  event_i, module_i, channel_i   ;
   int  index   ;
//   unsigned int plot_size ;
//   plot_size = gl_EventPageWrapLength   ;
   
   
       	DeleteGraphPlot (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);



      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 1)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc1_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 2)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc2_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 3)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc3_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_GREEN);
		}
      	
      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 4)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc4_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_CYAN);
		}


      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 5)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc5_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_RED);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 6)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc6_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_BLUE);
		}

      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 7)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc7_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_GREEN);
		}
      	
      	if ((gl_uint_Test1RawAdcShowChoose == 0) || (gl_uint_Test1RawAdcShowChoose == 8)) {	
  			PlotY (Panels[TEST1_RAW], T1_DIAGRAW_GRAPH_RAW, &gl_uint_adc8_data_array[0], 
  			       plot_size, 
			       VAL_UNSIGNED_SHORT_INTEGER, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_CYAN);
		}



	return 0;
}





			  
























/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*      Histogram                                                              */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/

// Call Backs

int CVICALLBACK CB_ClosePanelTest1Histo (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			gl_uint_Test1DisplayHistoFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_HISTO,0x0);  
            HidePanel(Panels[TEST1_HISTO]); //Clear Panel 
			break;
		}
	return 0;
}



int openPanelTest1Histo(void) 
{
	unsigned int xmin_value, xmax_value;	
	unsigned int ymin_value, ymax_value;	

	xmin_value  = 0 ;
	xmax_value  = 4095 ;
	ymin_value  = 0 ;
	ymax_value  = 2047 ;
	
	SetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_XMIN_SCALE, xmin_value);
	SetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_XMAX_SCALE, xmax_value);
	SetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_YMIN_SCALE, ymin_value);
	SetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_YMAX_SCALE, ymax_value);
	SetAxisScalingMode  (Panels[TEST1_HISTO],T1_DIAGHIS_HISTOGRAPH, VAL_XAXIS, VAL_MANUAL, xmin_value,xmax_value)  ;
	SetAxisScalingMode  (Panels[TEST1_HISTO],T1_DIAGHIS_HISTOGRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, ymin_value,ymax_value)  ;

	gl_uint_Test1HistoAdcShowChoose = 0 ;
	SetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_RING_MODE_3,gl_uint_Test1HistoAdcShowChoose);



	
	SetPanelPos(Panels[TEST1_HISTO],DEFINE_PANEL_TEST1_HISTO_POS_Y,DEFINE_PANEL_TEST1_HISTO_POS_X)  ;   // y, x
	DisplayPanel (Panels[TEST1_HISTO]);

 #ifdef raus
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_CTRL_MODE, VAL_INDICATOR); // only indicator 
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0)  ;   // disable  
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_ZOOM,0);                               // disable   flag
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_CVI_ZOOM,0);                            // disable CVI flag
	DisableZoomCursorsRaw() ;

	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_FROM, 0.0);
    SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_TO, 0.0);

 	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_W_LED, OFF);
   	DeleteGraphPlot (Panels[TEST1_RAW], DIAGPANEL3_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
#endif
	return 0;
}











/* --------------------------------------------------------------------------
   CB-Funktion zum einlesen des Zugriffs-Modes
-------------------------------------------------------------------------- */
int CVICALLBACK CB_ChooseHistoShow (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	int CviErr;
	
	switch (event)
	{
		case EVENT_COMMIT:
		// Den Access-Mode einlesen
 		  GetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_RING_MODE_3,&gl_uint_Test1HistoAdcShowChoose);
	      displayAdcHistograph () ; 
		  break;
	}

	return 0;
} // end function CB_ChooseHistoShow ()





 /* --------------------------------------------------------------------------
   CB-Funktion zum 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_SetHistoXYScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr;
	unsigned int xmin_value, xmax_value;	
	unsigned int ymin_value, ymax_value;	
	switch (event)
	{
		case EVENT_COMMIT:


			GetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_XMIN_SCALE, &xmin_value);
			GetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_XMAX_SCALE, &xmax_value);
			GetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_YMIN_SCALE, &ymin_value);
			GetCtrlVal (Panels[TEST1_HISTO],T1_DIAGHIS_HISTO_YMAX_SCALE, &ymax_value);
			SetAxisScalingMode  (Panels[TEST1_HISTO],T1_DIAGHIS_HISTOGRAPH, VAL_XAXIS, VAL_MANUAL,
			                     xmin_value,xmax_value)  ;
			SetAxisScalingMode  (Panels[TEST1_HISTO],T1_DIAGHIS_HISTOGRAPH, VAL_LEFT_YAXIS, VAL_MANUAL,
			                     ymin_value,ymax_value)  ;

  			
  			

			break;
		
	}
	return 0;
} // end function  ()





/* --------------------------------------------------------------------------
   Funktion zum 
-------------------------------------------------------------------------- */

#define HISTO_WIDTH	  0x1000

int displayAdcHistograph (void)
{
	unsigned int i;
	unsigned int value1,value2  ;
	double mean_adc, std_dev_adc ;

	int CviErr;
	unsigned short *adc_pointer ;
			
	    DeleteGraphPlot (Panels[TEST1_HISTO], T1_DIAGHIS_HISTOGRAPH, -1, 1);

  
//histogram
// clear Histogram   

  		if(gl_uint_Test1AutoClearHistogramFlag == 1) { 			
   			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintHistoADC_1_array[i] =  0x0 ;	    
				gl_uintHistoADC_2_array[i] =  0x0 ;	    
				gl_uintHistoADC_3_array[i] =  0x0 ;	    
				gl_uintHistoADC_4_array[i] =  0x0 ;	    
				gl_uintHistoADC_5_array[i] =  0x0 ;	    
				gl_uintHistoADC_6_array[i] =  0x0 ;	    
				gl_uintHistoADC_7_array[i] =  0x0 ;	    
				gl_uintHistoADC_8_array[i] =  0x0 ;	    
			}

		}

//Histogram ADC1
			adc_pointer = (short int* ) (gl_uint_adc1_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_1_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC1, mean_adc);
			SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC1, std_dev_adc);


//Histogram ADC2
			adc_pointer = (short int* ) (gl_uint_adc2_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_2_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC2, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC2, std_dev_adc);

//Histogram ADC3
			adc_pointer = (short int* ) (gl_uint_adc3_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_3_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC3, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC3, std_dev_adc);

//Histogram ADC4
			adc_pointer = (short int* ) (gl_uint_adc4_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_4_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC4, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC4, std_dev_adc);

//Histogram ADC5
			adc_pointer = (short int* ) (gl_uint_adc5_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_5_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC5, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC5, std_dev_adc);

//Histogram ADC6
			adc_pointer = (short int* ) (gl_uint_adc6_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_6_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC6, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC6, std_dev_adc);

//Histogram ADC7
			adc_pointer = (short int* ) (gl_uint_adc7_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_7_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC7, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC7, std_dev_adc);

//Histogram ADC8
			adc_pointer = (short int* ) (gl_uint_adc8_data_array)   ;
  			for (i=0; i<2048; i++) {
			  value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
			  gl_uintHistoADC_8_array[value1] ++ ;
			  gl_doubleTemp_array[i] = (value1) ;	// 256KBytes, 128 Ksamples   
			}

			StdDev (gl_doubleTemp_array, 2048, &mean_adc, &std_dev_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_MEAN_ADC8, mean_adc);
			CviErr = SetCtrlVal (Panels[TEST1_HISTO], T1_DIAGHIS_STD_DEV_ADC8, std_dev_adc);

	
 	switch (gl_uint_Test1HistoAdcShowChoose)				
	{
		case  0: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_1_array[i] ;	    
			}
			break;
		case  1: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_2_array[i] ;	    
			}
			break;
		case  2: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_3_array[i] ;	    
			}
			break;
		case  3: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_4_array[i] ;	    
			}
			break;
		case  4: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_5_array[i] ;	    
			}
			break;
		case  5: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_6_array[i] ;	    
			}
			break;
		case  6: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_7_array[i] ;	    
			}
			break;
		case  7: // 
  			for (i=0; i<HISTO_WIDTH; i++)
			{
				gl_uintGraph_array[i] =  gl_uintHistoADC_8_array[i] ;	    
			}
			break;
	} // end switch (uintHistoShowChoose)			


	PlotY (Panels[TEST1_HISTO], T1_DIAGHIS_HISTOGRAPH, gl_uintGraph_array, HISTO_WIDTH,
	VAL_UNSIGNED_INTEGER, VAL_VERTICAL_BAR, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
	
			
 	return 0x0 ;

	
} // end function  ()
















/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*      FFT                                                                    */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/

// Call Backs

int CVICALLBACK CB_ClosePanelTest1FFT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			gl_uint_Test1DisplayFftFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_FFT,0x0);  
            HidePanel(Panels[TEST1_FFT]); //Clear Panel 
			break;
		}
	return 0;
}




int openPanelTest1FFT(void) 
{
	unsigned int xmin_value, xmax_value;	
	unsigned int ymin_value, ymax_value;	

	xmin_value  = 0 ;
	xmax_value  = 50000 ;
	ymin_value  = 0 ;
	ymax_value  = 4095 ;
	
	GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_XMIN_SCALE, &xmin_value);
	GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_XMAX_SCALE, &xmax_value);
	GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_YMIN_SCALE, &ymin_value);
	GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_YMAX_SCALE, &ymax_value);
	SetAxisScalingMode  (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, VAL_XAXIS, VAL_MANUAL,
	                     xmin_value,xmax_value)  ;
	SetAxisScalingMode  (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, VAL_LEFT_YAXIS, VAL_MANUAL,
	                     ymin_value,ymax_value)  ;

	gl_uint_Test1FftAdcShowChoose = 0 ;
 	SetCtrlVal (Panels[TEST1_FFT],T1_DIAGFFT_RING_MODE_3,gl_uint_Test1FftAdcShowChoose);

	
	SetPanelPos(Panels[TEST1_FFT],DEFINE_PANEL_TEST1_FFT_POS_Y,DEFINE_PANEL_TEST1_FFT_POS_X)  ;   // y, x
	DisplayPanel (Panels[TEST1_FFT]);

 #ifdef raus
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_CTRL_MODE, VAL_INDICATOR); // only indicator 
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0)  ;   // disable  
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_ZOOM,0);                               // disable   flag
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_CVI_ZOOM,0);                            // disable CVI flag
	DisableZoomCursorsRaw() ;

	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_FROM, 0.0);
    SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_TO, 0.0);

 	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_W_LED, OFF);
   	DeleteGraphPlot (Panels[TEST1_RAW], DIAGPANEL3_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
#endif
	return 0;
}



extern unsigned int  gl_uint_Test1FftAdcShowChoose     ;


int CVICALLBACK CB_ChooseFftShow (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr;
	switch (event)  {
		case EVENT_COMMIT:
		// Den Access-Mode einlesen
 		  GetCtrlVal (Panels[TEST1_FFT],T1_DIAGFFT_RING_MODE_3,&gl_uint_Test1FftAdcShowChoose);
	      displayAdcFFTgraph () ; 
		  break;
	}

	return 0;
} // end function CB_ChooseHistoShow ()

 

/* --------------------------------------------------------------------------
   CB-Funktion zum 
-------------------------------------------------------------------------- */
int CVICALLBACK CB_SetFftXYScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr;
	unsigned int xmin_value, xmax_value;	
	unsigned int ymin_value, ymax_value;	
	switch (event)
	{
		case EVENT_COMMIT:

			 
			GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_XMIN_SCALE, &xmin_value);
			GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_XMAX_SCALE, &xmax_value);
			GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_YMIN_SCALE, &ymin_value);
			GetCtrlVal (Panels[TEST1_FFT], T1_DIAGFFT_FFT_YMAX_SCALE, &ymax_value);
			SetAxisScalingMode  (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, VAL_XAXIS, VAL_MANUAL,
			                     xmin_value,xmax_value)  ;
			SetAxisScalingMode  (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, VAL_LEFT_YAXIS, VAL_MANUAL,
			                     ymin_value,ymax_value)  ;

  			
  			

			break;
		
	}
	return 0;
} // end function  ()




/* --------------------------------------------------------------------------
   Funktion zum 
-------------------------------------------------------------------------- */
int displayAdcFFTgraph (void)
{
#define WIDTH 20   
#define NB_PTS      2048
#define FREQ_200MHZ      200000.0
#define FREQ_100MHZ      100000.0
#define FREQ_50MHZ        50000.0

	unsigned int nof_points;
	unsigned int sample_freq;
	
	unsigned int i, j, k, max_ind;
//	unsigned int value1,value2  ;
//	double mean_adc, std_dev_adc ;


	static double SignalArray[NB_PTS];
	static double imagArray[NB_PTS];
	static double spectrum[NB_PTS];
    int     Win, Signal, Scaling, Display;    
    double  df, Freq_Peak, Power_Peak,  Freq, Amp, max; 
    char    Unit[20]="V";        
    WindowConst WinConst;    
    static double phase=0.0;
    static double Converted_Spec[NB_PTS/2];    
    static double Auto_Spec[NB_PTS/2]; 
    static double Test_Spec[NB_PTS/2]; 

	double sample_time ;

 	int	int_value ;
	int	sum_value  ;
	int CviErr;

 	double rmsSignal, rmsNoise, peakAmp, peakFreq, snrAmp, eNoBits ;
	double tempSum, tempMax  ;
	int  tempIndex  ;

	unsigned short *adc_pointer ;
	unsigned int value1,value2  ;

	nof_points =  NB_PTS ;
	sample_freq = FREQ_200MHZ ;


//set Clock source
 	switch (gl_uint_Test1ClockMode)				
	{
	  case  0: //  
		sample_freq = FREQ_200MHZ ;
		break;
	  case  1: // intern 100 MHz
		sample_freq = FREQ_100MHZ ;
		break;
	  case  2: // intern 50 MHz
		sample_freq = FREQ_50MHZ ;
		break;
	} // end switch (uintClockMode)			



//FFT
	DeleteGraphPlot (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, -1, 1);
 	
 	switch (gl_uint_Test1FftAdcShowChoose)				
	{
		case  0: // 
			adc_pointer = (short int* ) (gl_uint_adc1_data_array)   ;
			break;
		case  1: // 
			adc_pointer = (short int* ) (gl_uint_adc2_data_array)   ;
			break;
		case  2: // 
			adc_pointer = (short int* ) (gl_uint_adc3_data_array)   ;
			break;
		case  3: // 
			adc_pointer = (short int* ) (gl_uint_adc4_data_array)   ;
			break;
		case  4: // 
			adc_pointer = (short int* ) (gl_uint_adc5_data_array)   ;
			break;
		case  5: // 
			adc_pointer = (short int* ) (gl_uint_adc6_data_array)   ;
			break;
		case  6: // 
			adc_pointer = (short int* ) (gl_uint_adc7_data_array)   ;
			break;
		case  7: // 
			adc_pointer = (short int* ) (gl_uint_adc8_data_array)   ;
			break;
	} // end switch (uintFftShowChoose)			


 	
 	
  			
	sum_value = 0 ;
	for (i=0; i<NB_PTS; i++) {
		value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
    	sum_value = sum_value +  value1 ;
	}

	sum_value =  ((sum_value   /  NB_PTS) );

  	for (i=0; i<NB_PTS; i++) {
		value1 =  (unsigned int)(adc_pointer[i] & 0x3FFF) ;
		int_value =  value1 ;
		SignalArray[i] =  ((int_value) - sum_value)  ;	    
	}

			
        
        
        GetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_WINDOW, &Win);
        ScaledWindow (SignalArray, NB_PTS, Win, &WinConst);

		sample_time = (double)(1.0/sample_freq) ;
        AutoPowerSpectrum (SignalArray, NB_PTS, sample_time, Auto_Spec, &df);
//        AutoPowerSpectrum (SignalArray, NB_PTS, 1/FREQ, Auto_Spec, &df);
//        suche Maximum
		max = -1; max_ind=0;
		for (i=0;i<NB_PTS/2; i++)
		 if  (Auto_Spec[i]>max) { max =Auto_Spec[i];max_ind = i; }
		 max = sqrt(max); max=max*1.1;
		for (i=0;i<NB_PTS/2; i++) {
		   Test_Spec[i] = 20.0 * log10 (sqrt(Auto_Spec[i])/max);
		}
		 

        /* Get cursor position                              */
        /* Calculate the nearest peak frequency and power   */
        GetGraphCursor (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, 1, &Freq, &Amp);
        PowerFrequencyEstimate (Auto_Spec, NB_PTS/2, Freq, WinConst, df, 7, &Freq_Peak, &Power_Peak);
        SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_POWER_PEAK, Power_Peak);
        SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_PEAK_FREQ, Freq_Peak);

        /* Get the scale information                                */
        /* Convert the spectrum and display the corresponding units */
        GetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_SCALING, &Scaling);
        GetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_DISPLAY, &Display);
        SpectrumUnitConversion (Auto_Spec, NB_PTS/2, 0, Scaling, Display, df, WinConst, Converted_Spec, Unit);
        SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_UNIT,Unit);

        /* Display the spectrum */
        DeleteGraphPlot (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, -1, VAL_DELAYED_DRAW);
        PlotWaveform (Panels[TEST1_FFT], T1_DIAGFFT_FFTGRAPH, Converted_Spec, NB_PTS/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,
                                    VAL_FAT_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_GREEN);
        DeleteGraphPlot (Panels[TEST1_FFT], T1_DIAGFFT_DBGRAPH, -1, VAL_DELAYED_DRAW);
        PlotWaveform (Panels[TEST1_FFT], T1_DIAGFFT_DBGRAPH, Test_Spec, NB_PTS/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
                                    VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_BLUE);

	


//********************************Auswertung des Spektrum, roe 10.5.99****************************/

  

	for (i=0; i<NB_PTS; i++)  {  /*Setze Imaginaer Teil auf Null, Umspeichern tempArray*/
           imagArray[i]=0; spectrum[i]=SignalArray[i];
	}

	FFT (spectrum, imagArray, NB_PTS);
   /*jetzt das Amplituden Spektrum Berechnen*/
//	for (i=0; i<(NB_PTS/2); i++)  {  
//                spectrum[i]= sqrt(spectrum[i]*spectrum[i]+imagArray[i]*imagArray[i])/(NB_PTS/2);
	for (i=0; i<(NB_PTS/2); i++)  {  
                spectrum[i]= Auto_Spec[i];
     }

  

  /*jetzt noch Maximum und Amplitudenwert suchen und SNR berechnen*/
  			 tempSum=0.0;
  			 peakAmp=0.0; 
  			 tempMax=0.0;
  			 tempIndex=0;
    		 for (i=0; i<(NB_PTS/2); i++)  {  
                 if (spectrum[i]>tempMax) {
                 tempMax=spectrum[i];
                 tempIndex=i;
                 }	   /*end if*/
                 tempSum=tempSum + spectrum[i];
             }   /*end for*/

			
//		 	peakFreq=(tempIndex * 12500 ) / 512 ;  /*  */
		 	peakFreq=(tempIndex * sample_freq ) / NB_PTS ;  /*  */
        	SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_PEAK_FREQ_2, peakFreq);

/* Errechne RMS es Peaks mit Schranke +/- WIDTH	   */
 			 rmsSignal= 0.0;
 			 rmsNoise = 0.0;
			 /**************IM FOLGENDEN TEIL fehlen noch Abfragen fuer Wertebreiche der Indizes am Rand...****/
			 /*Abfragen, j<Null fehlt !!!*/
 			 for (i=tempIndex-WIDTH; i<(tempIndex+WIDTH+1); i++) 
 			  {
// 			    if ((i>0) && (i<NB_PTS/2)) rmsSignal=rmsSignal+(spectrum[i] * spectrum[i]);
 			    if ((i>0) && (i<NB_PTS/2)) rmsSignal=rmsSignal+ sqrt(spectrum[i])  ;
 			  }
//  			 rmsSignal=sqrt(rmsSignal);
 			 peakAmp = rmsSignal ; 
			 /*Wertebereich abfragen fehlt */
// 			 for (i=2; i<(tempIndex-WIDTH); i++) rmsNoise=rmsNoise+(spectrum[i] * spectrum[i]);
			 if ((tempIndex-WIDTH) > 6 )
 			         for (i=5; i<(tempIndex-WIDTH); i++) rmsNoise=rmsNoise+ sqrt(spectrum[i])  ;
			 /* starte von 5, um den Gleichspannungsanteil rauszunehmen*/
			 /*Wertebereich abfragen fehlt noch*/
// 			 for (i=tempIndex+WIDTH+1; i<(NB_PTS/2); i++) rmsNoise=rmsNoise+(spectrum[i] * spectrum[i]);
 			 for (i=tempIndex+WIDTH+1; i<(NB_PTS/2); i++) rmsNoise=rmsNoise+ sqrt(spectrum[i]) ;

//  			 rmsNoise=sqrt(rmsNoise);
 			 if ((rmsNoise > 1 ) && (rmsSignal>0))
 			  {
 			 	snrAmp=20.0 * log (rmsSignal/rmsNoise);
 			 	eNoBits = (snrAmp-1.76)/6.02;
  			  }
 			  else
 			  {
 			    snrAmp= 0.0 ;
 			 	eNoBits = 0.0;
               }
                
        	SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_PEAK_FREQ_3, snrAmp);
        	SetCtrlVal(Panels[TEST1_FFT], T1_DIAGFFT_PEAK_FREQ_4, eNoBits);

			

#ifdef raus

#endif			
 	return 0x0 ;

	
} // end function  ()






/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*      LongTerm Panel                                                         */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/

// Call Backs

int CVICALLBACK CB_ClosePanelTest1LongTerm (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			gl_uint_Test1DisplayLongTermFlag = 0 ;
			SetCtrlVal (Panels[TEST1_MENUE], TEST1_MENU_CHECKBOX_DISPL_LONG_T,0x0);  
            HidePanel(Panels[TEST1_LTERM]); //Clear Panel 
			break;
		}
	return 0;
}




int openPanelTest1LongTerm(void) 
{
	
	SetPanelPos(Panels[TEST1_LTERM],DEFINE_PANEL_TEST1_LTERM_POS_Y,DEFINE_PANEL_TEST1_LTERM_POS_X)  ;   // y, x
	DisplayPanel (Panels[TEST1_LTERM]);

 #ifdef raus
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_CTRL_MODE, VAL_INDICATOR); // only indicator 
	SetCtrlAttribute (Panels[DIAG_RAW],DIAGPANEL3_GRAPH_RAW, ATTR_ENABLE_ZOOM_AND_PAN, 0)  ;   // disable  
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_ZOOM,0);                               // disable   flag
	SetCtrlVal (Panels[DIAG_RAW],DIAGPANEL3_CHECKBOX_CVI_ZOOM,0);                            // disable CVI flag
	DisableZoomCursorsRaw() ;

	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_FROM, 0.0);
    SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_X_POSITION_FIT_TO, 0.0);

 	SetCtrlVal (Panels[DIAG_RAW], DIAGPANEL3_W_LED, OFF);
   	DeleteGraphPlot (Panels[TEST1_RAW], DIAGPANEL3_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
#endif
	return 0;
}

