/***************************************************************************/
/*                                                                         */
/*  Filename: sis3320_configuration.c                                      */
/*                                                                         */
/*  Funktion: configuration                                                */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 20.05.2005                                       */
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

#include "sis3320.h" 	 

#include "vme_interface_defines.h"


int SystemConfiguration (void) ;
int SetDefaultConfiguration (void)	  ;
int WriteParameterToInputBoxes (void) ;
int CopyModuleAddressesToTable (void)	  ;
int module_led_test (void)	   ;



//--------------------------------------------------------------------------- 
// Root Menu Call Back
//--------------------------------------------------------------------------- 


void CVICALLBACK CB_MenuConf (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        
        case MENU_CONFIGURATION_LOAD : {
            LoadConfiguration ()  ;
            }
            break;
        case MENU_CONFIGURATION_SAVE : {
            SaveConfiguration (gl_char_FileNameConf)  ;
            }
            break;
        case MENU_CONFIGURATION_SAVEAS : {
            SaveConfigurationAs ()  ;
            }
            break;


        
        case MENU_CONFIGURATION_MODULES : {
   	        SetPanelPos(Panels[MODULE_PARA_CONF],DEFINE_PANEL_MODULE_CONF_POS_Y,DEFINE_PANEL_MODULE_CONF_POS_X)  ;   // y, x
			DisplayPanel (Panels[MODULE_PARA_CONF]);
            }
            break;
            
            
        }    // end switch menu item

	return ;
}



 

void CVICALLBACK CB_Menu_ModuleLedTest (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        case MENUCONFMO_TESTS_LEDTEST : {
		           module_led_test () ;
           }  
           break;
     }  // switch
return;
}



/* close Panels */
void CVICALLBACK CB_Menu_ModuleConfiguration (int menuBar, int menuItem, void *callbackData,
        int panel)
{
    switch (menuItem) {
        case MENUCONFMO_PANEL_CLOSE : {
            HidePanel(Panels[MODULE_PARA_CONF]); //Clear Panel 
           }  
           break;
     }  // switch
return;
}



/************************************************************************************************************/





int CVICALLBACK CB_ModuleEnableConf (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int CviErr;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case MOD_CONF_MODULE_ENABLE_1:
				    CviErr = GetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ENABLE_1, &gl_uint_ModEnableConf[0]);
					break;
				case MOD_CONF_MODULE_ENABLE_2:
				    CviErr = GetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ENABLE_2, &gl_uint_ModEnableConf[1]);
					break;
			} // end switch (control)

            CopyModuleAddressesToTable();
            CheckSystemStatus() ;  
			break;
		}
	return 0;
}




int CVICALLBACK CB_ModuleAddrConf (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int error=0;
	unsigned int data_rd;
	int CviErr;
	switch (event)
		{
		case EVENT_COMMIT:
  			switch (control)
			{
				case MOD_CONF_MODULE_ADDR_1:
				    CviErr = GetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ADDR_1, &gl_uint_ModAddrConf[0]);
					break;
				case MOD_CONF_MODULE_ADDR_2:
				    CviErr = GetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ADDR_2, &gl_uint_ModAddrConf[1]);
					break;
			} // end switch (control)

            CopyModuleAddressesToTable();
		    CheckSystemStatus() ;  

			break;
		}
	return 0;
}



/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/


int SaveConfigurationAs (void)
{   
int i, temp, status, select;
char tempFile[MAX_FILENAME_LEN];
int tempErr, dummy, okSema;

unsigned int xaddress ;



   GetProjectDir (gl_char_FilePathConf);
   select = FileSelectPopup (gl_char_FilePathConf, NORMAL_CONFIGURATION_FILE, "", 
											  "Save Configuration File", 
											  VAL_SAVE_BUTTON, 0, 1, 1, 1, 
											  gl_char_FileNameConf);

   if (select == VAL_EXISTING_FILE_SELECTED) {
      SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, OFF);
      SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, gl_char_FileNameConf);
      tempErr = SaveConfiguration (gl_char_FileNameConf)  ;

	  if (tempErr != FILE_ACCESS_OK) {
         SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, ON);
         SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, "error writing file ");
	  }
    }

  return 0;
}



/*---------------------------------------------------------------------------
Get all Configuration parameter, store to variables and save into file     
---------------------------------------------------------------------------*/

int SaveConfiguration (char *path_file)
{
    int i, temp, status, okSema;
    //write actual values to file (name defined in opm_defines as constant
   int imod, iadc;
    
unsigned int xaddress ;
	FILE *used_file ;
    char line_in[128];
    char line_out[128];

   char charLoaderFile_1[1024];	// Name und Pfad der Loader-Datei
	char path[1024];
	int  select;
	static char *timeString, *dateString;

    
    used_file=fopen(path_file, "w");
//    used_file=fopen(path_file, "w");
    if (used_file==NULL) return -1  ; // FILE_OPEN_ERROR_CONFIGURATION;

	timeString=TimeStr ();	  //get system time pointer
	dateString=DateStr ();	  // get system date pointer

    fprintf(used_file,"%s\n", STRING_CONFIGURATION_FILE);
	CopyString (line_out, 0, STRING_DATE, 0, strlen(STRING_DATE));
	CopyString (line_out, strlen(line_out), dateString, 0, 10);
	CopyString (line_out, strlen(line_out), STRING_TIME, 0, strlen(STRING_TIME));
	CopyString (line_out, strlen(line_out), timeString, 0, 8);

    fprintf(used_file,"%s \n", line_out);
    fprintf(used_file,"%s \n", STRING_BLANK);

    fprintf(used_file,"%s \n", STRING_MODULES);
    for (i=0;i<MAX_NO_OF_MODULES;i++) 
        fprintf(used_file,"%8x %8x \n",gl_uint_ModEnableConf[i], gl_uint_ModAddrConf[i]);
    fprintf(used_file,"%s \n", STRING_BLANK);

    fprintf(used_file,"%s\n", STRING_BLANK);
    fprintf(used_file,"%s\n", STRING_END_OF_FILE);

    fclose(used_file);
	return 0 ;
}


/********************************************************************************************************/


int FirstReadConfiguration (void)
{   
int i, select;
char tempFile[MAX_FILENAME_LEN];
int tempErr ;

   GetProjectDir (gl_char_FilePathConf);
   select = FileSelectPopup (gl_char_FilePathConf, NORMAL_CONFIGURATION_FILE, "", 
											  "Load Configuration File", 
											  VAL_LOAD_BUTTON, 0, 1, 1, 0, 
											  gl_char_FileNameConf);

   if (select == VAL_EXISTING_FILE_SELECTED) {
      SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, OFF);
      SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, gl_char_FileNameConf);
      tempErr = ReadConfiguration (gl_char_FileNameConf)  ;

	  if (tempErr != FILE_ACCESS_OK) {
	     SetDefaultConfiguration() ;
         SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, ON);
         SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, "not valid configuration file (default configuration)");
	  }
    }
    else { // VAL_NO_FILE_SELECTED; file not valid 
	   SetDefaultConfiguration() ;
       SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, ON);
       SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, "no configuration file is used (default configuration)");
    } 
	// Modules
    WriteParameterToInputBoxes();
    CopyModuleAddressesToTable();
    CheckSystemStatus() ;  
  return 0;
}
 


/********************************************************************************************************/



int LoadConfiguration (void)
{   
int i, select;
char tempFile[MAX_FILENAME_LEN];
int tempErr ;


   GetProjectDir (gl_char_FilePathConf);
   select = FileSelectPopup (gl_char_FilePathConf, "*.ini", "", 
											  "Load Configuration File", 
											  VAL_LOAD_BUTTON, 0, 1, 1, 0, 
											  gl_char_FileNameConf);

   if (select == VAL_EXISTING_FILE_SELECTED) {
      SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, OFF);
      SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, gl_char_FileNameConf);
      tempErr = ReadConfiguration (gl_char_FileNameConf)  ;

	  if (tempErr != FILE_ACCESS_OK) {
	     SetDefaultConfiguration() ;
         SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION_SAVE, ATTR_DIMMED, ON);
         SetCtrlVal (Panels[MAIN], PANEL_STRING_CONFFILE_NAME, "not valid configuration file (default configuration)");
	  }
	// Modules
    WriteParameterToInputBoxes();
    CopyModuleAddressesToTable();
    CheckSystemStatus() ;  
    }
  return 0;
}
 
 

/*---------------------------------------------------------------------------
Read configuration parameter from file     
---------------------------------------------------------------------------*/


int ReadConfiguration (char *path_file)
{   
int i, temp, status;
char tempFile[MAX_FILENAME_LEN];
int tempErr, dummy, okSema;
int comp_err ;
   int imod, iadc;
	FILE *used_file ;

char temp_char[MAX_FILENAME_LEN]  ;

unsigned int xaddress ;

    used_file=fopen(path_file, "r");
	temp = errno ;
    if (used_file==NULL)  return FILE_ACCESS_NOT_OK;
    fgets (temp_char, MAX_FILENAME_LEN, used_file);  /* get header */
    fgets (temp_char, MAX_FILENAME_LEN, used_file);  /* get get data and time */


 // get Modules
    fscanf(used_file,"%s\n",temp_char);	      // get KEY-STRING 	 
    comp_err = strcmp (temp_char, STRING_MODULES); // check KEY-STRING
	if(comp_err != 0)  {
       fclose(used_file);
	   return FILE_ACCESS_NOT_OK;   
	}
    for (i=0;i<MAX_NO_OF_MODULES;i++) {   
      fscanf(used_file,"%8x %8x \n",&gl_uint_ModEnableConf[i], &gl_uint_ModAddrConf[i]);   // read  Modules
	 } 

    fclose(used_file);
   
  return FILE_ACCESS_OK;
}




/********************************************************************************************************/


/* functions */

int SetDefaultConfiguration (void)
{   
int imod, iadc;

  // Modules
   for (imod=0;imod<MAX_NO_OF_MODULES;imod++)  gl_uint_ModEnableConf[imod] = 0 ;
   for (imod=0;imod<MAX_NO_OF_MODULES;imod++)
      gl_uint_ModAddrConf[imod]   = MODULE_CONF_DEFAULT_STARTADDRESS + (imod * MODULE_CONF_DEFAULT_ADDRESSINC) ;

  return 0;
}





/********************************************************************************************************/

int WriteParameterToInputBoxes (void)
{   
int CviErr;

  // Modules
   CviErr = SetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ENABLE_1, gl_uint_ModEnableConf[0]);
   CviErr = SetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ENABLE_2, gl_uint_ModEnableConf[1]);

   CviErr = SetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ADDR_1, gl_uint_ModAddrConf[0]);
   CviErr = SetCtrlVal (Panels[MODULE_PARA_CONF], MOD_CONF_MODULE_ADDR_2, gl_uint_ModAddrConf[1]);

  return 0;
}
    


/********************************************************************************************************/

int CopyModuleAddressesToTable (void)
{
	
   int CviErr;
   int i ;
   int no_of_enabled_modules ;
   int error ;
   unsigned int data_rd;

   no_of_enabled_modules = 0 ;
   gl_uint_system_status =  SYSTEM_STATUS_MODULES_READY ;

   for (i=0;i<MAX_NO_OF_MODULES;i++) {

     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 2),ATTR_CELL_DIMMED, OFF);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 3),ATTR_CELL_DIMMED, OFF);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 4),ATTR_CELL_DIMMED, OFF);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 1),ATTR_TEXT_BGCOLOR, VAL_WHITE);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 2),ATTR_TEXT_BGCOLOR, VAL_WHITE);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 3),ATTR_TEXT_BGCOLOR, VAL_WHITE);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 4),ATTR_TEXT_BGCOLOR, VAL_WHITE);
     SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 5),ATTR_TEXT_BGCOLOR, VAL_WHITE);

     SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 1),gl_uint_ModEnableConf[i]);
     SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 2),gl_uint_ModAddrConf[i]);

	 // check if disabled/enabled
     if  (gl_uint_ModEnableConf[i] == 0) {  // disabled
//		gl_uint_module_status = MODULE_STATUS_NOT_VALID ; 
        SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 3),0x0);
        SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 4),0x0);
        SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 2),ATTR_CELL_DIMMED, ON);
        SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 3),ATTR_CELL_DIMMED, ON);
        SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 4),ATTR_CELL_DIMMED, ON);
      }
      else {   // enabled
        error = sis1100w_Vme_Single_Read(&gl_sis1100_device, gl_uint_ModAddrConf[i] + SIS3320_MODID,0x9,4, &data_rd) ;
        if (error != 0x0) { // vme error
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 3),0xEEEE);
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 4),0xEEEE);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 1),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 2),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 3),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 4),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 5),ATTR_TEXT_BGCOLOR, VAL_RED);
		   gl_uint_system_status =  SYSTEM_STATUS_MODULES_NOT_READY ; 
	     }
         else  {  // vme OK
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 3),(data_rd >> 16) & 0xffff);
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 4),(data_rd ) & 0xffff);

           sis1100w_Vme_Single_Read(&gl_sis1100_device, gl_uint_ModAddrConf[i] + 0x1000000,0x9,4, &data_rd) ;
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 5),(data_rd ) );
           sis1100w_Vme_Single_Read(&gl_sis1100_device, gl_uint_ModAddrConf[i] + 0x1800000,0x9,4, &data_rd) ;
           SetTableCellVal (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (1+i, 6),(data_rd ) );

		 // hier koennte noch ID check rein
           no_of_enabled_modules++;
         }  // VME OK
      } // disabled/enabled
   } /* for loop */

   
   /* if no module is disabled then */
   if (no_of_enabled_modules == 0)   gl_uint_system_status =  SYSTEM_STATUS_MODULES_NOT_READY ;
   
   return 0;
} // end function  ()






/********************************************************************************************************/

int module_led_test (void)
{
	
   int CviErr;
   int i ;
   int no_of_enabled_modules ;
   int error ;
   unsigned int data_rd;
   int idelay, times, temp = 0 ;

   for (times=0;times<10;times++) {
   for (i=0;i<MAX_NO_OF_MODULES;i++) {
     if  (gl_uint_ModEnableConf[i] != 0) {  // enabled
        error = sis1100w_Vme_Single_Write(&gl_sis1100_device, gl_uint_ModAddrConf[i] + SIS3320_CONTROL_STATUS,0x9,4,0x1) ;
        if (error != 0x0) { // vme error
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 1),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 2),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 3),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 4),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 5),ATTR_TEXT_BGCOLOR, VAL_RED);
	     }
   for (idelay=0;idelay<0x100000;idelay++) temp++ ;
      } // disabled/enabled

   } /* for loop */
   i=0;
   for (idelay=0;idelay<0x100000;idelay++) i++ ;
   //delay
   for (i=0;i<MAX_NO_OF_MODULES;i++) {
     if  (gl_uint_ModEnableConf[i] != 0) {  // enabled
        error = sis1100w_Vme_Single_Write(&gl_sis1100_device, gl_uint_ModAddrConf[i] + SIS3320_CONTROL_STATUS,0x9,4,0x10000 ) ;
        if (error != 0x0) { // vme error
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 1),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 2),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 3),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 4),ATTR_TEXT_BGCOLOR, VAL_RED);
           SetTableCellAttribute (Panels[MAIN], PANEL_MODULE_TABLE,  MakePoint (i+1, 5),ATTR_TEXT_BGCOLOR, VAL_RED);
	     }
   for (idelay=0;idelay<0x100000;idelay++) temp++ ; ;

      } // disabled/enabled

   } /* for loop */

   } // times
   
   /* if no module is disabled then */
//   if (no_of_enabled_modules == 0)   gl_uint_module_status = MODULE_STATUS_NOT_VALID ;
   
   return 0;
} // end function  ()





















/********************************************************************************************************/






int CheckSystemStatus (void)
{
   int cvi_err ;
   // Modules not ready 
   if (gl_uint_system_status == SYSTEM_STATUS_MODULES_NOT_READY) {
  	 SetCtrlAttribute (Panels[MAIN], PANEL_SYSTEM_LED, ATTR_ON_COLOR, VAL_RED);
     MenuBars[MAIN_MENU_BAR] = LoadMenuBar (Panels[MAIN], "sis3320_main_uir.uir", MENU) ;
	 MenuBars[CONFMODULE_MENU_BAR] = LoadMenuBar (Panels[MODULE_PARA_CONF], "sis3320_main_uir.uir", MENUCONFMO) ;
     SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION, ATTR_DIMMED, OFF);
     return 0 ;
   }


    // Modules ready but not configrued 
   if (gl_uint_system_status == SYSTEM_STATUS_MODULES_READY) {
  	 SetCtrlAttribute (Panels[MAIN], PANEL_SYSTEM_LED, ATTR_ON_COLOR, VAL_YELLOW);
     MenuBars[MAIN_MENU_BAR] = LoadMenuBar (Panels[MAIN], "sis3320_main_uir.uir", MENU) ;
	 MenuBars[CONFMODULE_MENU_BAR] = LoadMenuBar (Panels[MODULE_PARA_CONF], "sis3320_main_uir.uir", MENUCONFMO) ;
     SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION, ATTR_DIMMED, OFF);
     return 0 ;
   }

    // Modules configrued 
   if (gl_uint_system_status == SYSTEM_STATUS_MODULES_CONFIGURED) {
  	 SetCtrlAttribute (Panels[MAIN], PANEL_SYSTEM_LED, ATTR_ON_COLOR, VAL_GREEN);

     MenuBars[MAIN_MENU_BAR] = LoadMenuBar (Panels[MAIN], "sis3320_main_uir.uir", MENU) ;
	 MenuBars[CONFMODULE_MENU_BAR] = LoadMenuBar (Panels[MODULE_PARA_CONF], "sis3320_main_uir.uir", MENUCONFMO) ;

     SetMenuBarAttribute  (MenuBars[MAIN_MENU_BAR], MENU_CONFIGURATION, ATTR_DIMMED, OFF);
     return 0 ;
   }

    // Modules running 
   if (gl_uint_system_status == SYSTEM_STATUS_MODULES_RUNNING) {
  	 SetCtrlAttribute (Panels[MAIN], PANEL_SYSTEM_LED, ATTR_ON_COLOR, VAL_BLUE);

	  DiscardMenuBar(MenuBars[MAIN_MENU_BAR]);
	  DiscardMenuBar(MenuBars[CONFMODULE_MENU_BAR]);
//	  DiscardMenuBar(MenuBars[CONFSAMPLE_MENU_BAR]);
//	  DiscardMenuBar(MenuBars[CONFTRIG_MENU_BAR]);
//	  DiscardMenuBar(MenuBars[CONFDECAY_MENU_BAR]);
     return 0 ;
   }


   return 0;
} // end function  ()


