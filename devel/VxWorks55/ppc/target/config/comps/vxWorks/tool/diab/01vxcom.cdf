/* 01vxcom.cdf - VxCOM Component description for T2.x */

/* Copyright (c) 1999-2001 Wind River systems, Inc. */

/*
modification history
--------------------
01b,13mar02,nel  SPR#73973. Remove invalid reference to
                 INCLUDE_COM_CORE_KERNEL.
01a,17dec01,nel  written
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
			comSysLib.o \
			vxidl_i.o \
			vxStream_i.o
	INCLUDE_WHEN	INCLUDE_COM
}

Component INCLUDE_COM_SHOW {
        NAME            COM show routines
        SYNOPSIS        enable display of COM registry, etc.
        PROTOTYPE	int comShowInit (void); \
			int include_vxcom_comShow (void);
        INIT_RTN        comShowInit (); \
			include_vxcom_comShow ();
        _INIT_ORDER     usrShowInit
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
        PROTOTYPE	void comLibInit (void); \
			int include_vxcom_comLib (void); \
			int include_vxcom_comMisc (void); \
			int include_vxcom_TaskAllocator (void); \
			int include_vxcom_MemoryStream (void);
	INIT_RTN	comLibInit (); \
			include_vxcom_comLib (); \
			include_vxcom_comMisc (); \
			include_vxcom_TaskAllocator (); \
			include_vxcom_MemoryStream ();
        _INIT_ORDER     usrRoot
	MODULES		DebugHooks.o
}

