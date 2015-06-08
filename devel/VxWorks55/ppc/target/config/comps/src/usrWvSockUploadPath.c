/* usrWvSockUploadPath.c - configuration file for Windview Socket upload path */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,30sep98,pr   added changes as in usrWvFileUploadPath.c
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

VOID usrWvSockUploadPathInit ()
    {
    _func_wvOnUploadPathCreate =  (FUNCPTR) sockUploadPathCreate;
    _func_wvOnUploadPathClose = (FUNCPTR) sockUploadPathClose; 
    wvArgHtons = 1;
    wvLogMsgString1 = "ip=";
    wvLogMsgString2 = "port=";

    sockUploadPathLibInit ();
    }
