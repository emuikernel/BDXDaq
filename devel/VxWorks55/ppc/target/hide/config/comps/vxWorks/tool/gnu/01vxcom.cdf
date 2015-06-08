/* 01vxcom.cdf - VxCOM Component description for T2.x */

/* Copyright (c) 1999-2001 Wind River systems, Inc. */

/*
modification history
--------------------
01e,13mar02,nel  SPR#74150. Removed unrequired INCLUDE_COM_AUTOMATION.
		 SPR#73973. Remove invalid reference to INCLUDE_COM_CORE_KERNEL.
01d,18sep01,nel  Add DebugHooks.o into base COM INCLUDE.
01c,17sep01,nel  make layout the same as AE
01b,20jul01,dbs  fix cdf syntax errors
01a,28jun01,dbs  written
*/

Folder FOLDER_COM {
	NAME		VxCOM (Component Object Model)
	SYNOPSIS	COM (Component Object Model) Support
	_CHILDREN	FOLDER_ROOT
	CHILDREN	FOLDER_COM_CORE \
			FOLDER_COM_CPLUS
}

/* COM Core Support */

Folder FOLDER_COM_CORE {
	NAME		COM Core
	SYNOPSIS	COM (Component Object Model) Core Support
	CHILDREN	INCLUDE_COM_CORE \
			INCLUDE_COM_SHOW \
			INCLUDE_COM_NTP_TIME
	DEFAULTS	INCLUDE_COM_CORE
}

Component INCLUDE_COM_CORE {
	NAME		COM Core 
	SYNOPSIS	COM (Component Object Model) Core API
        PROTOTYPE	void comCoreLibInit (void);
	INIT_RTN	comCoreLibInit (); 
	_INIT_ORDER	usrRoot
	MODULES		comCoreLib.o \
			comRegistry.o \
			comCoreTypes_i.o \
			comSysLib.o 
	INCLUDE_WHEN	INCLUDE_COM
}

Component INCLUDE_COM_SHOW {
        NAME            COM show routines
        SYNOPSIS        enable display of COM registry, etc.
        PROTOTYPE	void comShowInit (void);
        INIT_RTN        comShowInit ();
        _INIT_ORDER     usrShowInit
        MODULES         comShow.o
}

Component INCLUDE_COM_NTP_TIME {
        NAME            COM NTP Time support
        SYNOPSIS        Use NTP to provide an adjustment to the system clock.
        PROTOTYPE	void comSysNtpInit (int, char *, unsigned init);
        INIT_RTN        comSysNtpInit (VXCOM_NTP_BROADCAST, VXCOM_NTP_SERVER, VXCOM_NTP_TIMEOUT);
        _INIT_ORDER     usrRoot
	CFG_PARAMS	VXCOM_NTP_BROADCAST \
			VXCOM_NTP_SERVER \
			VXCOM_NTP_TIMEOUT
        MODULES         comSysNtp.o
}

Parameter VXCOM_NTP_BROADCAST {
        NAME            NTP Broadcast
        SYNOPSIS        Use the NTP broadcast to get NTP server time
        TYPE            bool
        DEFAULT         FALSE
}

Parameter VXCOM_NTP_SERVER {
        NAME            NTP Server
        SYNOPSIS        NTP Server to use or ignored if broadcast mode is set.
        TYPE            string
        DEFAULT         "ntp"
}

Parameter VXCOM_NTP_TIMEOUT {
	NAME		NTP Timeout
	SYNOPSIS	NTP Timeout used when trying to retrieve time.
	TYPE		uint
	DEFAULT		5
}

/* Enhanced COM Support */

Folder FOLDER_COM_CPLUS {
	NAME		Enhanced COM Support
	SYNOPSIS	Enhanced COM (Component Object Model) Support
	CHILDREN	INCLUDE_COM 
	DEFAULTS	INCLUDE_COM
}

Component INCLUDE_COM {
	NAME		Enhanced COM Support
	SYNOPSIS	Enhanced COM (Component Object Model) Support
        PROTOTYPE	void comLibInit (void);
	INIT_RTN	comLibInit (); 
        _INIT_ORDER     usrRoot
	MODULES		comLib.o \
			comMisc.o \
			DebugHooks.o \
			TaskAllocator.o \
			MemoryStream.o \
			vxStream_i.o \
			vxidl_i.o \
			comAutomation_i.o
}
