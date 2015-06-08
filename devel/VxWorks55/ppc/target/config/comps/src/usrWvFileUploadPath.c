/* usrWvFileUploadPath.c - configuration file for Windview File upload path */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,13may98,pr   written
*/

/*
DESCRIPTION
*/

extern FUNCPTR _func_wvOnUploadPathCreate;
extern FUNCPTR _func_wvOnUploadPathClose;
extern int wvArgHtons;
extern char * wvLogMsgString1;
extern char * wvLogMsgString2;

VOID usrWvFileUploadPathInit ()
    {

    _func_wvOnUploadPathCreate = (FUNCPTR) fileUploadPathCreate;
    _func_wvOnUploadPathClose = (FUNCPTR) fileUploadPathClose; 
    wvArgHtons = 0;
    wvLogMsgString1 = "fname=";
    wvLogMsgString2 = "flag=";

    fileUploadPathLibInit ();
    }
