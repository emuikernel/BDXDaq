/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	CODA Editor database/graph converter header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_converter.h,v $
 *   Revision 1.3  1997/06/20 16:59:51  heyes
 *   clean up GUI!
 *
 *   Revision 1.2  1996/11/13 14:55:43  chen
 *   allow different readout list in different configuration
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:30  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_CONVERTER_H
#define _EDITOR_CONVERTER_H

#if defined (__STDC__)
extern IoId       *newIoId (void);
extern void       freeIoId (IoId* id);
extern ConfigInfo *newConfigInfo (void);
extern void       setConfigInfoName (ConfigInfo* cinfo, char* name);
extern void       setConfigInfoCode (ConfigInfo* cinfo, char* code);
extern void       setConfigInfoInputs (ConfigInfo* cinfo, char* buffer);
extern void       setConfigInfoOutputs (ConfigInfo* cinfo, char* buffer);
extern void       setConfigInfoPosition (ConfigInfo* cinfo, int row, int col);
extern void       addScriptToConfigInfo (ConfigInfo* cinfo, char* state,
					 char* script);
extern int        matchConfigInfo (ConfigInfo* cinfo, char* name);
extern void       freeConfigInfo (ConfigInfo* cinfo);
extern rcNetComp* newRcNetComp (void);
extern void       freeRcNetComp (rcNetComp* comp);
extern void       setRcNetComp (rcNetComp* comp, 
				char* name, 
				int id, 
				char* cmd, 
				char* comp_type, 
				char* host);
extern void       XcodaEditorWriteToConfig(char* config_name, 
					   XcodaEditorGraph* graph);
extern void       XcodaEditorInsertAllComps (XcodaEditorGraph* graph);
extern int        getConfigurationInfo (char* config, 
					ConfigInfo** cinfo, int* num);
extern int        constructRcnetComps(rcNetComp** comps, int* num);
extern int        constructRcnetCompsWithConfig (char* config, 
						 rcNetComp** comps, int* num,
						 ConfigInfo** cinfos,
						 int* num_cinfo);
extern void       XcodaEditorConstructGraphFromConfig(XcodaEditorGraph* graph, 
						      rcNetComp** daq_list, 
						      int num_daqs, 
						      ConfigInfo** cinfo, 
						      int num_cinfos);
extern int        compNotUsed (char *comp_name);
#else
extern IoId       *newIoId ();
extern void       freeIoId ();
extern ConfigInfo *newConfigInfo ();
extern void       setConfigInfoName ();
extern void       setConfigInfoCode ();
extern void       setConfigInfoInputs ();
extern void       setConfigInfoOutputs ();
extern void       setConfigInfoPosition ();
extern void       addScriptToConfigInfo ();
extern int        matchConfigInfo ();
extern void       freeConfigInfo ();
extern rcNetComp* newRcNetComp ();
extern void       freeRcNetComp ();
extern void       setRcNetComp ();
extern void       XcodaEditorWriteToConfig ();
extern void       XcodaEditorInsertAllComps ();
extern int        getConfigurationInfo ();
extern int        constructRcnetComps ();
extern int        constructRcnetCompsWithConfig ();
extern void       XcodaEditorConstructGraphFromConfig();
extern int        compNotUsed ();
#endif

extern int        outputFileNum;
extern int        codaFileNum;
extern int        debuggerNum;
extern int        ddNum;

#endif
