//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Constant definitions for CODA Svc
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqConst.h,v $
//   Revision 1.2  1997/09/16 12:28:43  heyes
//   make comms async
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_DAQ_CONST_H
#define _CODA_DAQ_CONST_H

#include <codaConst.h>

#define CODA_NON_EXISTENCE       -1

#define CODA_LOW_STATE           0

#define CODA_DORMANT             0
#define CODA_BOOTING             1
#define CODA_BOOTED              2
#define CODA_CONFIGURING         3
#define CODA_CONFIGURED          4
#define CODA_DOWNLOADING         5
#define CODA_DOWNLOADED          6
#define CODA_PRESTARTING         7
#define CODA_PAUSED              8
#define CODA_PAUSING             9
#define CODA_ACTIVATING          10
#define CODA_ACTIVE              11
#define CODA_ENDING              12
#define CODA_VERIFYING           13
#define CODA_VERIFIED            14
#define CODA_TERMINATING         15
#define CODA_PRESTARTED          16
#define CODA_RESETTING           17
#define CODA_DISCONNECTED        18
#define CODA_CONNECTING          19
#define CODA_BUSY                20
#define CODA_HIGH_STATE          20

// daq actions
#define CODA_CONNECT_ACTION      0
#define CODA_BOOT_ACTION         1
#define CODA_CONFIGURE_ACTION    2
#define CODA_DOWNLOAD_ACTION     3
#define CODA_PRESTART_ACTION     4
#define CODA_RESET_ACTION        5
#define CODA_END_ACTION          6
#define CODA_PAUSE_ACTION        7
#define CODA_GO_ACTION           8
#define CODA_VERIFY_ACTION       9
#define CODA_ABORT_ACTION        10
#define CODA_TERMINATE_ACTION    11
#define CODA_NUM_ACTIONS         12

// constant strings of class names
#define CODA_ANALYSIS            "analysis"
#define CODA_ANA_CLASS           "ANA"
#define CODA_EVENT_BUILDER       "event_builder"
#define CODA_EB_CLASS            "EB"
#define CODA_EVENT_RECORDER      "event_recorder"
#define CODA_ER_CLASS            "ER"
#define CODA_LOG                 "console_log"
#define CODA_LOG_CLASS           "LOG"
#define CODA_READOUT             "readout_controller"
#define CODA_ROC_CLASS           "ROC"
#define CODA_RUNCONTROL          "run_control"
#define CODA_RC_CLASS            "RC"
#define CODA_USER_SCRIPT         "user_script"
#define CODA_SCRIPT_CLASS        "USER_SCRIPT"
#define CODA_TRIGGER_SUPVSOR     "trigger_supervisor"
#define CODA_TS_CLASS            "TS"
#define CODA_USER_COMP           "user_component"
#define CODA_UC_CLASS            "UC"

// constants associated with files
#define CODA_RUNENV              "$RCDATABASE"
#define CODA_DATABASE            "rcDatabase"
#define CODA_EXPERIMENT          "rcExperiment"
#define CODA_NETWORK             "rcNetwork"
#define CODA_RUN_NUMBER          "rcRunNumber"
#define CODA_RUN_PRIORITY        "rcPriority"
#define CODA_RUN_TYPES           "rcRunTypes"
#define CODA_CONFIG              ".config"
#define CODA_OPTIONS             ".options"

#endif

