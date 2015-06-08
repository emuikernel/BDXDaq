
Component INCLUDE_LOADER {
        SYNOPSIS        ELF loader
        MODULES         loadLib.o loadElfLib.o
        INIT_RTN        loadElfInit ();
        HDR_FILES       loadElfLib.h
}

Component INCLUDE_EXC_EXTENDED_VECTORS {
	NAME		Allow 32-bit branches to handlers
        SYNOPSIS        Extended 32-bit vector branches to handlers
	_CHILDREN	FOLDER_KERNEL
	HDR_FILES	private/excLibP.h
	CFG_PARAMS	EXC_EXTENDED_VECTORS_ENABLED
        INIT_RTN        excExtendedVectors = EXC_EXTENDED_VECTORS_ENABLED;
	_INIT_ORDER	usrInit
        INIT_BEFORE     INCLUDE_EXC_HANDLING
}

#ifdef	PPC403
Folder FOLDER_MMU {
        NAME            MMU
        CHILDREN        INCLUDE_MMU_NONE 
        DEFAULTS        INCLUDE_MMU_NONE
}

Component INCLUDE_MMU_NONE {
	NAME 		MMU None
	SYNOPSIS	MMU not available for 403
}
#else	/* PPC403 */
Selection SELECT_MMU {
	NAME		MMU Mode
	SYNOPSIS	Select MMU configuration
	COUNT		1-1
	CHILDREN	INCLUDE_MMU_FULL INCLUDE_MMU_BASIC INCLUDE_MMU_MPU
	DEFAULTS	INCLUDE_MMU_BASIC
}
#endif	/* PPC403 */

Component INCLUDE_CACHE_SUPPORT {
	NAME		cache support
	SYNOPSIS	include cache support
	INIT_RTN	cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE);
	MODULES		cacheLib.o 
	CFG_PARAMS	USER_I_CACHE_MODE USER_D_CACHE_MODE
#if ( !defined(PPC403) && !defined(PPC405) )
	REQUIRES	SELECT_MMU
#endif
	HDR_FILES	cacheLib.h 
}

Parameter EXC_EXTENDED_VECTORS_ENABLED {
	NAME            Enables Extended 32-bit exception vector 
	TYPE		bool
	DEFAULT 	TRUE
}

#ifndef	PPC403
Parameter USER_I_MMU_ENABLE {
        NAME    	enable instruction MMU
        TYPE    	exists
#if ( defined(PPC405) || defined(PPC440) )
        DEFAULT 	FALSE
#else
        DEFAULT 	TRUE
#endif
	_CFG_PARAMS	INCLUDE_MMU_FULL	\
			INCLUDE_MMU_BASIC
}

Parameter USER_D_MMU_ENABLE {
        NAME    	enable data MMU
        TYPE    	exists
        DEFAULT 	TRUE
	_CFG_PARAMS	INCLUDE_MMU_FULL	\
			INCLUDE_MMU_BASIC
}
#endif	/* PPC403 */

Parameter STACK_SAVE {
	NAME		Stack size
	SYNOPSIS	Amount of stack to reserve (bytes) for rom code
	DEFAULT		0x1000
}

Parameter INT_LOCK_LEVEL {
	NAME		INT lock level
	SYNOPSIS	Interrupt lock level
	DEFAULT		0x0
}

Parameter ROOT_STACK_SIZE {
	NAME		Root stack size
	SYNOPSIS	Root task stack size (bytes)
	DEFAULT		24000
}

Parameter SHELL_STACK_SIZE {
	NAME		Shell stack size
	SYNOPSIS	Shell stack size (bytes)
	DEFAULT		20000
}

Parameter WDB_STACK_SIZE {
	NAME		WDB Stack size
	SYNOPSIS	WDB Stack size (bytes)
	DEFAULT		0x2000
}

Parameter ISR_STACK_SIZE {
	NAME		ISR stack size
	SYNOPSIS	ISR Stack size (bytes)
	DEFAULT		5000
}

Parameter VEC_BASE_ADRS {
	NAME		Vector base address
	SYNOPSIS	Vector base address
	DEFAULT		((char *) 0x0 )
}

Parameter ROM_WARM_ADRS {
	NAME		ROM warm boot address
	SYNOPSIS	ROM warm boot address
	DEFAULT		(ROM_TEXT_ADRS + 4)
}

#ifdef	PPC604
Component INCLUDE_ALTIVEC {
      NAME            AltiVec runtime support
      SYNOPSIS        AltiVec register save/restore routines
      _CHILDREN       FOLDER_PERIPHERALS
      INIT_RTN        usrAltivecInit();
      _INIT_ORDER     usrKernelExtraInit
      MODULES         altivecALib.o \
                      altivecArchLib.o \
                      altivecLib.o
      HDR_FILES       altivecLib.h
      CONFIGLETTES    SaveRestVR.c usrAltivec.c
}

Component INCLUDE_ALTIVEC_SHOW {
	NAME		AltiVec show routine
	SYNOPSIS	task AltiVec registers
	_CHILDREN	FOLDER_SHOW_ROUTINES
	MODULES		altivecShow.o
	INIT_RTN	altivecShowInit ();
	_INIT_ORDER	usrShowInit
	HDR_FILES	altivecLib.h
	INCLUDE_WHEN	INCLUDE_ALTIVEC
	REQUIRES	INCLUDE_ALTIVEC
}

Component INCLUDE_WDB_ALTIVEC {
	NAME		WDB AltiVec support
	SYNOPSIS	Initialize WDB AltiVec debug support
	CONFIGLETTES	usrWdbAltivec.c
	_CHILDREN	FOLDER_WDB_OPTIONS
	MODULES		wdbAltivecLib.o
	INIT_RTN	wdbAltivecInit ();
	_INIT_ORDER	usrWdbInit
	INIT_BEFORE	INCLUDE_WDB_BANNER
	INCLUDE_WHEN	INCLUDE_WDB INCLUDE_ALTIVEC
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB INCLUDE_ALTIVEC
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_TASK_ALTIVEC {
	NAME		WDB task AltiVec support
	SYNOPSIS	task mode AltiVec debug support
	_CHILDREN	FOLDER_WDB_OPTIONS
	CONFIGLETTES	usrWdbTaskAltivec.c
	MODULES		wdbAltivecLib.o
	INIT_RTN	wdbTaskAltivecInit ();
	_INIT_ORDER	usrWdbInit
	INIT_BEFORE	INCLUDE_WDB_BANNER
	INCLUDE_WHEN	INCLUDE_WDB_TASK INCLUDE_ALTIVEC
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB_TASK INCLUDE_WDB_ALTIVEC
	HELP		tgtsvr WDB
}

Parameter WDB_TASK_OPTIONS {
	NAME            WDB task options
	DEFAULT         VX_UNBREAKABLE | VX_FP_TASK | VX_ALTIVEC_TASK
}

Parameter WDB_SPAWN_OPTS {
	NAME            default options for spawned tasks
	TYPE            uint
	DEFAULT         VX_FP_TASK | VX_ALTIVEC_TASK
}
#endif	/* PPC604 */
