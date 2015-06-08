/*
Copyright 1998-2002 Wind River Systems, Inc.


modification history
--------------------
03s,31jan03,tor  Mount filesystems at boot time (spr #69254)
03r,08nov02,dbt  Improved dependencies between WDB components (SPR #79259)
03q,22jun02,gls  reversed order of INCLUDE_MEM_MGR_FULL and INCLUDE_MEM_MGR_BASIC in usrRoot() (SPR #78999)
03p,29may02,dat  Fix header file for SELECT, 78096
03o,15may02,j_s  make INCLUDE_SPY require INCLUDE_AUX_CLK
03n,08may02,sn   SPR 76322 - pull in demangler if C++ and the symbol table are included
03m,27mar02,jkf  SPR#74716, moved INCLUDE_DISK_UTIL from DOSFS into IO folder
03l,19mar02,sn   SPR 71699 - remove cplusLoad.o from INCLUDE_CPLUS_DEMANGLER
		 (cplusLoad.o and cplusUsr.o are part of the targe loader)
03k,19mar02,j_s  Delist bufLib.o as a module of INCLUDE_WDB, add INCLUDE_BUF_MGR
		 to REQUIRES of INCLUDE_WDB (SPR #73477)
03j,13mar02,rbl  fixing bug where EXC_SHOW component appeared in two folders
03i,12mar02,jkf  SPR#74178, adding CDROMFS to iosextra init group.
03h,11mar02,sn   Wind Foundation classes no longer supported (SPR #73967)
03g,06mar02,sn   Removed INCLUDE_GNU_64BIT (obsoleted by INCLUDE_GNU_INTRINSICS)
03f,04mar02,j_s  fix the typo 'SELECT_SYMTBL_INIT', should be
                 'SELECT_SYM_TBL_INIT'
03e,05feb02,sn   Added SELECT_COMPILER_INTRINSICS
03d,29jan02,g_h  Add INCLUDE_WDB_COMM_VTMD to SELECT_WDB_COMM_TYPE
03c,28jan02,sn   added (); to cplusCtorsLink; added INCLUDE_CTORS_DTORS to InitGroup
03b,25jan02,zl   removed cplusGlob.o from INCLUDE_CTORS_DTORS
03a,21jan02,sn   added INCLUDE_CTORS_DTORS
02z,09dec01,jac  added INCLUDE_CDROMFS component
02y,30nov01,sbs  Synopsis to NUM_FILES about FD_SETSIZE (SPR 9377)
02x,20nov01,bwa  Changed INCLUDE_EVENTS to INCLUDE_VXEVENTS
02w,12nov01,mas  made new components for SM_OBJ, SM_NET, and SM_COMMON
02v,05nov01,gls  added INCLUDE_POSIX_PTHREADS
02u,06nov01,sn   moved toolchain specific C++ stuff to toolchain subdirectories
02t,31oct01,tcr  add Windview network instrumentation component
02s,22oct01,sn   Added catch-all compiler intrinsics components
02r,11oct01,bwa  Corrected MODULE field for VxWorks events.
02q,11oct01,mas  cfg params: added new ones to, and deleted duplicates from,
		 INCLUDE_SM_OBJ (SPR 4547, 62128)
02p,10oct01,fmk  add configlette to INCLUDE_CPLUS and change name of init
		 routine to match
02o,09oct01,bwa  Added VxWorks events module
02n,26sep01,jws  changed default for SM_OBJ_MAX_TRIES to 5000 (SPR 68418)
02m,20sep01,aeg  added the INCLUDE_SELECT_SUPPORT component.
02l,13sep01,pcm  added INCLUDE_EXC_SHOW (SPR 7333), INCLUDE_LSTLIB (SPR 20698)
02k,09may01,ros  fix codetest
02h,17may01,rec  SPR 63492 - cache not enabled from project
02j,10jan01,sn   Specify configlettes for toolchain related/C++ components
		 Changed C++ stdlib defaults to exclude complex and include STL
		 extract core iostreams modules from full iostreams
02i,05dec00,sn   Fix definition of INCLUDE_CPLUS_STL
02h,11oct00,sn   Use modules instead of linksyms for the C++ comps
02k,01dec00,cmf  changed name of INCLUDE_PCI_SHOW to INCLUDE_PCI_CFGSHOW to
                 match T3 naming
02j,24aug00,pai  corrected INCLUDE_PCI_SHOW component fix (SPR #27759).
02i,19may00,pai  added INCLUDE_PCI_SHOW component (SPR #27759).
02h,11oct99,pai  changed software and hardware floating point init order (SPR
                 28624).
02l,16nov00,zl   added DSP related components.
02k,18oct00,rsh  remove obsolete ctOperateroNew.o reference
02j,27jul00,rsh  fix up MODULES define for CodeTEST component to reflect that
                 fact that we are now archiving in the individual .o's instead
                 of a single relatively linked .o (.a)
02i,14jun00,mnd  Changed INCLUDE_CODETEST MODULES from ctLib to libctSwic.a.
02h,20apr00,mnd  Changed CodeTEST include component to ctLib. Removed older
                 modules cttXdr.o, cttOperatorNew.o, cttTargetLib.o, no longer
                 needed. Updating the file for CodeTEST 2.11 integration with
                 Tornado 2.0.
02g,01apr99,cjs  Correct symbol table initialization logic so that removing
                 the symtab init component removes the selection as well
02f,24mar99,cjs  broke apart symbol table initialization so that symbol
				 table can be included w/o initialization 
02e,17mar99,pr   added ataShow routines (SPR #25777)
02d,10mar99,pr   added trgShow and rBuffShow components (SPR #25417)
02c,26feb99,cjs  Fix defaults for SELECT_WDB_COMM_TYPE, SELECT_WDB_MODE 
02b,23feb99,cth  reordering init order in usrIosExtraInit (SPR 24297)
		 added INCLUDE_MEM_SHOW component (SPR 24371)
		 removed extra PROTOTYPE from usrInit
02a,19jan99,pr   deleted INCLUDE_WHEN entry in INCLUDE_SEQ_TIMESTAMP (SPR 23630)
01z,15jan99,dbt  added some help references for WDB components.
01y,24nov98,pr   added dependencies between INCLUDE_WVUPLOAD_TSFSSOCK and 
		 INCLUDE_WDB_TSFS
01x,19nov98,pr   added INCLUDE_WVUPLOAD_TSFSSOCK to windview upload defaults
01w,04nov98,ms   changed some LINK_SYMS to LINK_DATASYMS so SIMHP works.
01v,21oct98,ms   added INCLUDE_CODETEST component
01u,20oct98,ms   added INCLUDE_WHEN for SEQ_TIMESTAMP and RBUFF components
01t,11oct98,ms   cplusUsr.o no longer part of INCLUDE_CPLUS_MANGLER
01s,30sep98,pr   added TRIGGERING component descriptor
01r,30sep98,ms   folded in pr's WINDVIEW component descriptors
01q,15sep98,ms   fixed errors uncovered by new cxrDocValidate
01p,14sep98,ms   removed INCLUDE_EXC_SHOW (it's now part of EXC_HANDLING)
01o,08sep98,sn   removed TRAP_DEBUG parameter
01n,02sep98,sn   Fixed up C++ dependencies
01m,19aug98,ms   Fixed INCLUDE_CPLUS_STRING - append "_" to _cstringi_o
01l,19aug98,sut  Moved BSP components to 00bsp.cdf
01k,19aug98,jmp  fixed typo in INCLUDE_WDB_VIO_LIB SYNOPSIS.
01j,23aug98,ms   INCLUDE_SHELL now REQUIRES SELECT_SYMTBL_INIT.
01i,23aug98,sut  changed some BSP components
01h,14aug98,ms   added proper DEFAULTS to most folders
01g,12aug98,ms   fixed initialization order of WDB agent
01f,07aug98,cjs  fixed a spelling mistake in a component synopsis 
01e,03jun98,ms   lots of cleanup.
01d,03jun98,sn   updated C++ components
01c,03jun98,dbt  completed WDB components
01b,21may98,ms	 SYM_TBL_INIT and SYM_TBL are now only under one folder
01a,09apr98,ms   written


DESCRIPTION
  This file contains descriptions for some VxWorks components.
The configuration tool searches for .cdf files in four
places; here, arch/<arch>, config/<bsp>, and the project directory.
This file describes "generic" components which are available
to all CPU architectures and BSPs.
If a new component is released, a new .cdf file should be deposited
in the appropriate directory, rather than modifying existing files.

XXX - to do
* add in windview
* "Precious" Components (like the kernel!) that can't be deleted.
* Folder-precious components (like wdbCore), that can't be deleted
*   without deleting the whole folder.
*/

Component INCLUDE_SYS_START {
	NAME		system startup code
	SYNOPSIS	clear BSS and set up the vector table base address.
	INIT_RTN	sysStart (startType);
	CONFIGLETTES	usrStartup.c
	CFG_PARAMS	CLEAR_BSS
	HDR_FILES	intLib.h string.h
}

Component INCLUDE_SYSHW_INIT {
	NAME		BSP hardware initialization
	SYNOPSIS	call the BSPs sysHwInit routine during system startup
	INIT_RTN	sysHwInit ();
	HDR_FILES	sysLib.h
}

Folder	FOLDER_ROOT {
	NAME		all components
	CHILDREN	FOLDER_APPLICATION	\
			FOLDER_TOOLS		\
			FOLDER_NETWORK		\
			FOLDER_CPLUS		\
			FOLDER_OS		\
			FOLDER_OBSOLETE		\
			FOLDER_HARDWARE
	DEFAULTS	FOLDER_TOOLS			\
			FOLDER_NETWORK			\
			FOLDER_OS FOLDER_HARDWARE
}


Folder FOLDER_OS {
	NAME		operating system components
	CHILDREN	FOLDER_IO_SYSTEM	\
			FOLDER_KERNEL		\
			FOLDER_ANSI		\
			FOLDER_POSIX		\
			FOLDER_UTILITIES
	DEFAULTS	FOLDER_IO_SYSTEM FOLDER_KERNEL FOLDER_ANSI \
			FOLDER_UTILITIES
}

Folder	FOLDER_OBSOLETE {
	NAME		obsolete components
	SYNOPSIS	will be removed next release
	CHILDREN	INCLUDE_TYCODRV_5_2
}

Component INCLUDE_TYCODRV_5_2 {
	NAME		5.2 serial drivers
	SYNOPSIS	replaced by sio drivers
	CONFIGLETTES	usrSerialOld.c
	INIT_RTN	usrSerialOldInit ();
	EXCLUDES	INCLUDE_SIO
	CFG_PARAMS	NUM_TTY			\
			CONSOLE_TTY		\
			CONSOLE_BAUD_RATE
	HDR_FILES	sysLib.h stdio.h ioLib.h
}

Folder	FOLDER_ANSI {
	NAME		ANSI C components (libc)
	SYNOPSIS	ANSI libraries
	CHILDREN	INCLUDE_ANSI_ASSERT	\
			INCLUDE_ANSI_CTYPE	\
			INCLUDE_ANSI_LOCALE	\
			INCLUDE_ANSI_MATH	\
			INCLUDE_ANSI_STDIO	\
			INCLUDE_ANSI_STDLIB	\
			INCLUDE_ANSI_STRING	\
			INCLUDE_ANSI_TIME	\
			INCLUDE_ANSI_STDIO_EXTRA
	DEFAULTS	INCLUDE_ANSI_ASSERT INCLUDE_ANSI_CTYPE \
			INCLUDE_ANSI_MATH INCLUDE_ANSI_STDIO \
			INCLUDE_ANSI_STDLIB INCLUDE_ANSI_STRING \
			INCLUDE_ANSI_TIME
}

Component	INCLUDE_ANSI_STDIO_EXTRA {
	NAME		ANSI stdio extensions
	SYNOPSIS	WRS routines getw, putw, and setbuffer
	LINK_SYMS	getw putw setbuffer
}

Component INCLUDE_ANSI_ASSERT {
	NAME		ANSI assert
	LINK_SYMS	__assert
	HELP		ansiAssert
}

Component INCLUDE_ANSI_CTYPE {
	NAME		ANSI ctype
	LINK_SYMS	isalnum isalpha iscntrl isdigit isgraph islower \
			isprint ispunct isspace isupper isxdigit tolower \
			toupper
	HELP		ansiCtype
}

Component INCLUDE_ANSI_LOCALE {
	NAME		ANSI locale
	SYNOPSIS	forced inclusion of isdigit, isupper, tolower, etc
	LINK_SYMS	localeconv setlocale
	HELP		ansiLocale
}

Component INCLUDE_ANSI_MATH {
	NAME		ANSI math
	LINK_SYMS	acos asin atan atan2 ceil cos cosh exp fabs floor \
			fmod frexp ldexp log log10 modf pow sin sinh \
			sqrt tan tanh
	HELP		ansiMath
}

Component INCLUDE_ANSI_STDIO {
	NAME		ANSI stdio
	LINK_SYMS	clearerr fclose feof ferror fflush fgetc fgetpos \
			fgets fopen fprintf fputc fputs fread freopen fscanf \
			fseek fsetpos ftell fwrite getc getchar gets perror \
			putc putchar puts rewind scanf setbuf setvbuf tmpfile \
			tmpnam ungetc vfprintf fdopen fileno 
	HELP		ansiStdio
}

Component INCLUDE_ANSI_STDLIB {
	NAME		ANSI stdlib
	LINK_SYMS	abort abs atexit atof atoi atol bsearch calloc div \
			exit free getenv labs ldiv malloc mblen qsort rand \
			realloc strtod strtol strtoul system 
	HELP		ansiStdlib
}

Component INCLUDE_ANSI_STRING {
	NAME		ANSI string
	LINK_SYMS	memchr memcmp memcpy memset memmove strcat strchr \
			strcmp strcoll strcpy strcspn strerror strlen \
			strncat strncmp strncpy strpbrk strrchr strspn \
			strstr strtok strtok_r strxfrm 
	HELP		ansiString
}

Component INCLUDE_ANSI_TIME {
	NAME		ANSI time
	LINK_SYMS	asctime clock ctime difftime gmtime localtime \
			mktime strftime time
	HELP		ansiTime
}

Folder	FOLDER_CPLUS {
	NAME		C++ components
	SYNOPSIS	C++ class libraries
	CHILDREN	INCLUDE_CTORS_DTORS 	\
                        INCLUDE_CPLUS		\
			INCLUDE_CPLUS_LANG	\
			FOLDER_CPLUS_STDLIB
	DEFAULTS	INCLUDE_CPLUS \
			INCLUDE_CPLUS_LANG
}

Selection SELECT_COMPILER_INTRINSICS {
	NAME		Compiler support routines
	SYNOPSIS        Compiler support routines needed to support dynamic module download
	COUNT		1-
	CHILDREN	INCLUDE_GNU_INTRINSICS \
			INCLUDE_DIAB_INTRINSICS \
			INCLUDE_NO_INTRINSICS
}

Component INCLUDE_GNU_INTRINSICS {
	NAME		GNU compiler support routines
	SYNOPSIS	Compiler support routines that may be needed by dynamically downloaded GNU C code
        CONFIGLETTES    intrinsics.c
}

Component INCLUDE_DIAB_INTRINSICS {
	NAME		Diab compiler support routines
	SYNOPSIS	Compiler support routines that may be needed by dynamically downloaded Diab C code
        CONFIGLETTES    intrinsics.c
}

Component INCLUDE_NO_INTRINSICS {
	NAME		No intrinsics support
	SYNOPSIS	Only include compiler support routines that are required to build kernel
	EXCLUDES	INCLUDE_GNU_INTRINSICS INCLUDE_DIAB_INTRINSICS
}

Component INCLUDE_CPLUS {
	NAME		C++ core runtime
	SYNOPSIS        Basic support for C++ applications
	INIT_RTN	usrCplusLibInit ();
	HDR_FILES	private/cplusLibP.h 
	MODULES		cplusCore.o \
			cplusInit.o \
			cplusLib.o
	CONFIGLETTES	usrCplus.c
        REQUIRES        INCLUDE_CTORS_DTORS
}

Component INCLUDE_CPLUS_LANG {
	NAME		C++ compiler support routines
	SYNOPSIS	Support for exception handling, new, delete etc.
}

Component INCLUDE_CTORS_DTORS {
	NAME		run static initializers
        SYNOPSIS        run compiler generated initialization functions at system startup
	INIT_RTN	cplusCtorsLink ();
	HDR_FILES	cplusLib.h 
	MODULES		cplusXtors.o
}

Folder FOLDER_CPLUS_STDLIB {
	NAME		standard library
	SYNOPSIS	C++ Standard Library
	CHILDREN	INCLUDE_CPLUS_STRING		\
			INCLUDE_CPLUS_COMPLEX		\
			INCLUDE_CPLUS_STL		\
			INCLUDE_CPLUS_IOSTREAMS		\
			INCLUDE_CPLUS_IOSTREAMS_FULL	\
			INCLUDE_CPLUS_STRING_IO		\
			INCLUDE_CPLUS_COMPLEX_IO
	DEFAULTS	INCLUDE_CPLUS_IOSTREAMS INCLUDE_CPLUS_STRING \
			INCLUDE_CPLUS_STL
}

Folder	FOLDER_POSIX {
	NAME		POSIX components
	CHILDREN	INCLUDE_POSIX_AIO	\
			INCLUDE_POSIX_AIO_SYSDRV \
			INCLUDE_POSIX_FTRUNC	\
			INCLUDE_POSIX_MEM	\
			INCLUDE_POSIX_MQ	\
			INCLUDE_POSIX_PTHREADS  \
			INCLUDE_POSIX_SCHED	\
			INCLUDE_POSIX_SEM	\
			INCLUDE_POSIX_SIGNALS	\
			INCLUDE_POSIX_TIMERS	\
			INCLUDE_POSIX_CLOCKS
	DEFAULTS	INCLUDE_POSIX_AIO INCLUDE_POSIX_AIO_SYSDRV \
			INCLUDE_POSIX_FTRUNC INCLUDE_POSIX_MQ \
			INCLUDE_POSIX_SEM INCLUDE_POSIX_SIGNALS \
			INCLUDE_POSIX_TIMERS INCLUDE_POSIX_CLOCKS \
			INCLUDE_POSIX_PTHREADS

}

Component INCLUDE_POSIX_AIO {
	NAME		POSIX asynchronous IO
	MODULES		aioPxLib.o
	INIT_RTN	aioPxLibInit (MAX_LIO_CALLS);
	CFG_PARAMS	MAX_LIO_CALLS
	HDR_FILES	aio.h
}

Component INCLUDE_POSIX_AIO_SYSDRV {
	NAME		POSIX AIO driver
	SYNOPSIS	provides AIO functionality to non-AIO drivers
	MODULES		aioSysDrv.o
	INIT_RTN	aioSysInit (MAX_AIO_SYS_TASKS, \
				AIO_TASK_PRIORITY, AIO_TASK_STACK_SIZE);
	CFG_PARAMS	MAX_AIO_SYS_TASKS \
			AIO_TASK_PRIORITY \
			AIO_TASK_STACK_SIZE
	HDR_FILES	aioSysDrv.h
}

Component INCLUDE_POSIX_FTRUNC {
	NAME		POSIX ftruncate
	MODULES		ftruncate.o
	LINK_SYMS	ftruncate
	HDR_FILES	unistd.h
}

Component INCLUDE_POSIX_MEM {
	NAME		POSIX mman
	SYNOPSIS	null-wrappers for POSIX memory paging routines
	MODULES		mmanPxLib.o
	LINK_SYMS	mlockall
	HDR_FILES	sys/mman.h
}

Component INCLUDE_POSIX_MQ {
	NAME		POSIX message queues
	MODULES		mqPxLib.o
	INIT_RTN	mqPxLibInit (MQ_HASH_SIZE);
	CFG_PARAMS	MQ_HASH_SIZE
	HDR_FILES	private/mqPxLibP.h
}

Component INCLUDE_POSIX_PTHREADS {
	NAME		POSIX threads
	MODULES		pthreadLib.o _pthreadLib.o
	INIT_RTN	pthreadLibInit ();	
	HDR_FILES	pthread.h
}

Component INCLUDE_POSIX_SCHED {
	NAME		POSIX scheduler
	MODULES		schedPxLib.o
	LINK_SYMS	sched_setparam
	HDR_FILES	sched.h
}

Component INCLUDE_POSIX_SEM {
	NAME		POSIX semaphores
	MODULES		semPxLib.o
	INIT_RTN	semPxLibInit ();
	HDR_FILES	classLib.h private/semPxLibP.h
}

Component INCLUDE_POSIX_SIGNALS {
	NAME		POSIX signals
	SYNOPSIS	queued signals
	INIT_RTN	sigqueueInit (NUM_SIGNAL_QUEUES);
	CFG_PARAMS	NUM_SIGNAL_QUEUES
	HDR_FILES	private/sigLibP.h
}

Component INCLUDE_POSIX_TIMERS {
	NAME		POSIX timers
	MODULES		timerLib.o
	LINK_SYMS	nanosleep
	HDR_FILES	time.h
}

Component INCLUDE_POSIX_CLOCKS {
	NAME		POSIX clocks
	MODULES		clockLib.o
	LINK_SYMS	clockLibInit
}

Folder FOLDER_SHOW_ROUTINES {
	NAME		show routines
	SYNOPSIS	enable display of components states
	CHILDREN	INCLUDE_CLASS_SHOW	\
			INCLUDE_MEM_SHOW	\
			INCLUDE_MMU_FULL_SHOW	\
			INCLUDE_MSG_Q_SHOW	\
			INCLUDE_POSIX_AIO_SHOW	\
			INCLUDE_POSIX_MQ_SHOW	\
			INCLUDE_POSIX_SEM_SHOW	\
			INCLUDE_SEM_SHOW	\
			INCLUDE_STDIO_SHOW	\
			INCLUDE_SYM_TBL_SHOW	\
			INCLUDE_TASK_HOOKS_SHOW	\
			INCLUDE_TASK_SHOW	\
			INCLUDE_WATCHDOGS_SHOW	\
			INCLUDE_TRIGGER_SHOW	\
			INCLUDE_RBUFF_SHOW	\
			INCLUDE_ATA_SHOW        \
			INCLUDE_PCI_CFGSHOW	\
			INCLUDE_HW_FP_SHOW	\
			INCLUDE_DSP_SHOW	\
			INCLUDE_EXC_SHOW
	DEFAULTS	INCLUDE_CLASS_SHOW INCLUDE_TASK_SHOW
}

Component INCLUDE_ATA_SHOW {
	NAME		ata show routine
	SYNOPSIS	ata information
	MODULES		ataShow.o
	INIT_RTN	ataShowInit ();
	HDR_FILES	drv/hdisk/ataDrv.h drv/pcmcia/pccardLib.h 
}

Component INCLUDE_PCI_CFGSHOW {
	NAME		pci show routines
	SYNOPSIS	pci information from pciConfigShow routines
	CONFIGLETTES	../../../src/drv/pci/pciConfigShow.c
	INIT_RTN	pciConfigShowInit ();
	HDR_FILES	drv/pci/pciConfigLib.h drv/pci/pciConfigShow.h \
			drv/pci/pciHeaderDefs.h
}

Component INCLUDE_DSP_SHOW {
	NAME		DSP show routine
	SYNOPSIS	task DSP registers
	MODULES		dspShow.o
	INIT_RTN	dspShowInit ();
	HDR_FILES	dspLib.h
}

Component INCLUDE_HW_FP_SHOW {
	NAME		floating point show routine
	SYNOPSIS	task floating point registers
	MODULES		fppShow.o
	INIT_RTN	fppShowInit ();
	HDR_FILES	fppLib.h
}

Component INCLUDE_MSG_Q_SHOW {
	NAME		message queue show routine
	MODULES		msgQShow.o
	INIT_RTN	msgQShowInit ();
}

Component INCLUDE_SEM_SHOW {
	NAME		semaphore show routine
	MODULES		semShow.o
	INIT_RTN	semShowInit ();
}

Component INCLUDE_STDIO_SHOW {
	NAME		stdio show routine
	MODULES		stdioShow.o
	INIT_RTN	stdioShowInit ();
	HDR_FILES	stdio.h
}

Component INCLUDE_SYM_TBL_SHOW {
	NAME		symbol table show routine
	MODULES		symShow.o
	INIT_RTN	symShowInit ();
}

Component INCLUDE_TASK_HOOKS_SHOW {
	NAME		task hook show routine
	MODULES		taskHookShow.o
	INIT_RTN	taskHookShowInit ();
	HDR_FILES	taskHookLib.h
}

Component INCLUDE_WATCHDOGS_SHOW {
	NAME		watchdog timer show routine
	MODULES		wdShow.o
	INIT_RTN	wdShowInit ();
}

Component INCLUDE_TASK_SHOW {
	NAME		task show routine
	MODULES		taskShow.o
	INIT_RTN	taskShowInit ();
}

Component INCLUDE_MEM_SHOW {
	NAME		memory show routine
	MODULES		memShow.o
	INIT_RTN	memShowInit ();
}

Component INCLUDE_CLASS_SHOW {
	NAME		class show routine
	MODULES		classShow.o
	INIT_RTN	classShowInit ();
}

Component INCLUDE_POSIX_AIO_SHOW {
	NAME		asynchronous IO show routine
	MODULES		aioPxShow.o
	LINK_SYMS	aioShow
}

Component INCLUDE_POSIX_MQ_SHOW {
	NAME		POSIX message queue show routine
	MODULES		mqPxShow.o
	INIT_RTN	mqPxShowInit ();
	HDR_FILES	mqPxShow.h
}

Component INCLUDE_POSIX_SEM_SHOW {
	NAME		POSIX semaphore show routine
	MODULES		semPxShow.o
	INIT_RTN	semPxShowInit ();
	HDR_FILES	semPxShow.h
}

Component INCLUDE_TRIGGER_SHOW {
	NAME		trigger show routine
	MODULES		trgShow.o
	INIT_RTN	trgShowInit ();
        HDR_FILES       trgLib.h private/trgLibP.h
	REQUIRES	INCLUDE_TRIGGERING
}

Component INCLUDE_RBUFF_SHOW {
	NAME		rBuff show routine
	MODULES		rBuffShow.o
	INIT_RTN	rBuffShowInit ();
        HDR_FILES       rBuffLib.h
	REQUIRES	INCLUDE_RBUFF
}

Folder	FOLDER_TOOLS {
	NAME		development tool components
	SYNOPSIS	development tools
	CHILDREN	FOLDER_WDB		\
			FOLDER_SHELL		\
			FOLDER_LOADER		\
			FOLDER_SYMTBL		\
			FOLDER_SHOW_ROUTINES	\
			INCLUDE_SPY		\
			INCLUDE_TIMEX		\
			INCLUDE_TRIGGERING	\
			INCLUDE_CODETEST	\
			FOLDER_WINDVIEW		\
                        SELECT_COMPILER_INTRINSICS
	DEFAULTS	FOLDER_WDB
}

Component INCLUDE_CODETEST {
	NAME		CodeTEST run-time support
	SYNOPSIS	CodeTEST target utilities library
	MODULES		ctAmcPrintf.o ctCallPair.o ctMemLib.o \
			ctPerfAn.o ctServer.o \
			ctTrace.o ctUtils.o ctVxWorks.o ctXdr.o \
			ctTargetLib.o ctThread.o ctTimestamp.o \
			ctversion.o taskname.o
/*
Note: ct_mem.g++2.7.o cannot be listed, though it must be in the archive, since,
apparently, the Project facility parser can't see it, presumably due to the "++".
*/
	REQUIRES	INCLUDE_CPLUS
}

Folder FOLDER_SHELL {
	NAME		target shell components
	CHILDREN	INCLUDE_DEBUG		\
			INCLUDE_SHELL_BANNER	\
			INCLUDE_STARTUP_SCRIPT	\
			INCLUDE_SHELL
	DEFAULTS	INCLUDE_DEBUG		\
			INCLUDE_SHELL_BANNER	\
			INCLUDE_SHELL
}

Component INCLUDE_DEBUG {
	NAME		target debugging
	SYNOPSIS	breakpoints and stack tracer on target. Not needed for remote debugging with tornado.
	CONFIGLETTES	usrBreakpoint.c
	MODULES		dbgLib.o
	INIT_RTN	dbgInit ();
	HDR_FILES	dbgLib.h vxLib.h cacheLib.h
}

Component INCLUDE_SHELL_BANNER {
	NAME		shell banner
	SYNOPSIS	display the WRS banner on startup
	CONFIGLETTES	usrBanner.c
	INIT_RTN	usrBanner ();
	REQUIRES	INCLUDE_SHELL
	HDR_FILES	sysLib.h stdio.h shellLib.h usrLib.h
}

Component INCLUDE_STARTUP_SCRIPT {
	NAME		shell startup script
	CONFIGLETTES	usrScript.c
	INIT_RTN	usrStartupScript (sysBootParams.startupScript);
	HDR_FILES	sysLib.h stdio.h shellLib.h usrLib.h ioLib.h
	HELP		shellLib
}

Component INCLUDE_SHELL {
	NAME		target shell
	MODULES		shellLib.o
	INIT_RTN	shellInit (SHELL_STACK_SIZE, TRUE);
	HDR_FILES	shellLib.h
	CFG_PARAMS	SHELL_STACK_SIZE
}

Component INCLUDE_SPY {
	NAME		spy
	SYNOPSIS	utility to measure per-task CPU utilization
	MODULES		spyLib.o
	INIT_RTN	spyLibInit ();
	REQUIRES	INCLUDE_AUX_CLK
	HDR_FILES	spyLib.h
}

Component INCLUDE_TIMEX {
	NAME		timex
	SYNOPSIS	utility to measure function execution time
	MODULES		timexLib.o
	INIT_RTN	timexInit ();
	HDR_FILES	timexLib.h
}

Folder FOLDER_SYMTBL {
	NAME		symbol table components
	CHILDREN	INCLUDE_STAT_SYM_TBL	\
			INCLUDE_SYM_TBL_SYNC	\
			INCLUDE_CPLUS_DEMANGLER	\
			INCLUDE_SYM_TBL \
			FOLDER_SYM_TBL_INIT
	DEFAULTS	INCLUDE_SYM_TBL \
				INCLUDE_SYM_TBL_INIT
}

Folder FOLDER_SYM_TBL_INIT {
	NAME symbol table initialization components
	CHILDREN 	INCLUDE_SYM_TBL_INIT \
			SELECT_SYM_TBL_INIT
	DEFAULTS	INCLUDE_SYM_TBL \
			INCLUDE_SYM_TBL_INIT
}

Component INCLUDE_SYM_TBL_INIT {
	NAME 		initialize symbol table
	REQUIRES	INCLUDE_SYM_TBL \
			SELECT_SYM_TBL_INIT
}

Component INCLUDE_SYM_TBL {
	NAME		symbol table
	MODULES		symLib.o
	INIT_RTN	symLibInit ();
	HDR_FILES	symLib.h
}

Selection SELECT_SYM_TBL_INIT {
	NAME		select symbol table initialization
	SYNOPSIS	method for initializing the system symbol table
	COUNT		1-1
	CHILDREN	INCLUDE_STANDALONE_SYM_TBL \
			INCLUDE_NET_SYM_TBL
	DEFAULTS	INCLUDE_STANDALONE_SYM_TBL
}

Component INCLUDE_STANDALONE_SYM_TBL {
	NAME		built-in symbol table
	SYNOPSIS	prefered method if not booting from the network.
	CONFIGLETTES	usrStandalone.c
	INIT_RTN	usrStandaloneInit ();
	CFG_PARAMS	SYM_TBL_HASH_SIZE_LOG2
	HDR_FILES	sysLib.h symLib.h stdio.h sysSymTbl.h
	HELP		loadLib symLib
	REQUIRES	INCLUDE_SYM_TBL \
			INCLUDE_SYM_TBL_INIT
}

Component INCLUDE_NET_SYM_TBL {
	NAME		downloaded symbol table
	SYNOPSIS	prefered method if booting from the network.
	CONFIGLETTES	usrLoadSym.c
	INIT_RTN	usrLoadSyms ();
	CFG_PARAMS	SYM_TBL_HASH_SIZE_LOG2
	HDR_FILES	loadLib.h string.h sysLib.h sysSymTbl.h symLib.h \
			stdio.h fcntl.h
	REQUIRES	INCLUDE_LOADER \
			INCLUDE_BOOT_LINE_INIT \
			INCLUDE_SYM_TBL \
			INCLUDE_SYM_TBL_INIT
	HELP		loadLib symLib
}

Component INCLUDE_SYM_TBL_SYNC {
	NAME		synchronize host and target symbol tables
	SYNOPSIS	keep tgtsvr and target symbols tables synchronized.
	MODULES		symSyncLib.o
	INIT_RTN	symSyncLibInit ();
	REQUIRES	INCLUDE_SYM_TBL_INIT
	INIT_AFTER	INCLUDE_SYM_TBL_INIT
	HDR_FILES	symSyncLib.h
	HELP		loadLib symLib tgtsvr
}

Component INCLUDE_STAT_SYM_TBL {
	NAME		error status table
	SYNOPSIS	table of error strings for perror()
	CONFIGLETTES	usrStatTbl.c
	MODULES		statTbl.o
	INIT_RTN	usrStatTblInit ();
	CFG_PARAMS	STAT_TBL_HASH_SIZE_LOG2
	HDR_FILES	sysLib.h symLib.h stdio.h sysSymTbl.h
	HELP		errnoLib perror
}

Folder FOLDER_LOADER {
	NAME		loader components
	CHILDREN	INCLUDE_UNLOADER \
			INCLUDE_LOADER	\
			INCLUDE_MODULE_MANAGER
	DEFAULTS	INCLUDE_LOADER
}

Component INCLUDE_CPLUS_DEMANGLER {
	NAME		C++ symbol demangler
	SYNOPSIS	support library for target shell and loader: provides human readable forms of C++ identifiers
	INIT_RTN	cplusDemanglerInit ();
	MODULES		cplusDem.o
	INCLUDE_WHEN	INCLUDE_CPLUS INCLUDE_SYM_TBL
}

Component INCLUDE_MODULE_MANAGER {
	NAME		module manager
	SYNOPSIS	support library for the target-based loader.
	MODULES		moduleLib.o
	INIT_RTN	moduleLibInit ();
	HDR_FILES	moduleLib.h
}

/*
 * arch/<*>/arch.cdf contains the full definition for INCLUDE_LOADER.
 * It extends the skeletal definition provided here.
 */

Component INCLUDE_LOADER {
	NAME		target loader
	MODULES		loadLib.o
	REQUIRES	SELECT_SYM_TBL_INIT \
			SELECT_COMPILER_INTRINSICS
}

Component INCLUDE_UNLOADER {
	NAME		target unloader
	LINK_SYMS	unld
	MODULES		unldLib.o
}

Folder FOLDER_WDB {
	NAME		WDB agent components
	SYNOPSIS	software agent for tornado tools connections
	CHILDREN	FOLDER_WDB_OPTIONS	\
			SELECT_WDB_COMM_TYPE 	\
			SELECT_WDB_MODE
	DEFAULTS	FOLDER_WDB_OPTIONS SELECT_WDB_COMM_TYPE \
			SELECT_WDB_MODE
	HELP		tgtsvr WDB
}

Folder FOLDER_WDB_OPTIONS {
	NAME		WDB agent services
	SYNOPSIS	optional services for the WDB target agent
	CHILDREN	INCLUDE_WDB_BANNER	\
			INCLUDE_WDB_BP		\
			INCLUDE_WDB_CTXT	\
			INCLUDE_WDB_DIRECT_CALL	\
			INCLUDE_WDB_EVENTS	\
			INCLUDE_WDB_EXC_NOTIFY	\
			INCLUDE_WDB_EXIT_NOTIFY	\
			INCLUDE_WDB_FUNC_CALL	\
			INCLUDE_WDB_GOPHER	\
			INCLUDE_WDB_MEM		\
			INCLUDE_WDB_REG		\
			INCLUDE_WDB_VIO		\
			INCLUDE_WDB_VIO_LIB	\
			INCLUDE_WDB_EVENTPOINTS \
			INCLUDE_WDB_START_NOTIFY \
			INCLUDE_WDB_USER_EVENT	\
			INCLUDE_WDB_TASK_BP 	\
			INCLUDE_WDB_HW_FP	\
			INCLUDE_WDB_TASK_HW_FP	\
			INCLUDE_WDB_SYS_HW_FP	\
			INCLUDE_WDB_DSP		\
			INCLUDE_WDB_TASK_DSP	\
			INCLUDE_WDB_SYS_DSP	\
			INCLUDE_WDB_TSFS	\
			INCLUDE_WDB
	DEFAULTS	INCLUDE_WDB_BANNER	\
			INCLUDE_WDB_BP		\
			INCLUDE_WDB_CTXT	\
			INCLUDE_WDB_DIRECT_CALL	\
			INCLUDE_WDB_EVENTS	\
			INCLUDE_WDB_EXC_NOTIFY	\
			INCLUDE_WDB_EXIT_NOTIFY	\
			INCLUDE_WDB_FUNC_CALL	\
			INCLUDE_WDB_GOPHER	\
			INCLUDE_WDB_MEM		\
			INCLUDE_WDB_REG		\
			INCLUDE_WDB_VIO		\
			INCLUDE_WDB_VIO_LIB	\
			INCLUDE_WDB_EVENTPOINTS \
			INCLUDE_WDB_START_NOTIFY \
			INCLUDE_WDB_USER_EVENT	\
			INCLUDE_WDB
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_EVENTPOINTS {
	NAME		WDB eventpoints
	SYNOPSIS	support library for breakpoints and other asynchonous events.
	MODULES		wdbEvtptLib.o
	INIT_RTN	wdbEvtptLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_START_NOTIFY {
	NAME		WDB task creation
	SYNOPSIS	ability to notify the host when a task starts
	MODULES		wdbCtxStartLib.o
	INIT_RTN	wdbCtxStartLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_USER_EVENT {
	NAME		WDB user event
	SYNOPSIS	ability to send user events to the host
	MODULES		wdbUserEvtLib.o
	INIT_RTN	wdbUserEvtLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Selection SELECT_WDB_COMM_TYPE {
	NAME		select WDB connection
	COUNT		1-1
	CHILDREN	INCLUDE_WDB_COMM_SERIAL		\
			INCLUDE_WDB_COMM_TYCODRV_5_2	\
			INCLUDE_WDB_COMM_NETWORK	\
			INCLUDE_WDB_COMM_NETROM         \
			INCLUDE_WDB_COMM_VTMD           \
			INCLUDE_WDB_COMM_END		\
			INCLUDE_WDB_COMM_CUSTOM
	DEFAULTS	INCLUDE_WDB_COMM_NETWORK
	HELP		tgtsvr WDB
}

Selection SELECT_WDB_MODE {
	NAME		select WDB mode
	COUNT		1-2
	CHILDREN	INCLUDE_WDB_TASK	\
			INCLUDE_WDB_SYS
	DEFAULTS	INCLUDE_WDB_TASK
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB {
	NAME		WDB agent
	SYNOPSIS	software agent to support the tornado tools
	MODULES		wdbLib.o	\
			wdbSvcLib.o	\
			wdbCnctLib.o	\
			wdbMemCoreLib.o	\
			wdbRpcLib.o	
	CONFIGLETTES	usrWdbCore.c
	INIT_RTN	wdbConfig ();
	REQUIRES	SELECT_WDB_COMM_TYPE SELECT_WDB_MODE \
			SELECT_COMPILER_INTRINSICS INCLUDE_BUF_MGR
	CFG_PARAMS	WDB_STACK_SIZE
	HDR_FILES	wdb/wdbLib.h wdb/wdbLibP.h private/funcBindP.h \
			wdb/wdbUdpLib.h sysLib.h excLib.h string.h \
			bufLib.h intLib.h rebootLib.h version.h \
			taskHookLib.h vxLib.h private/taskLibP.h \
			private/kernelLibP.h private/funcBindP.h \
			wdb/wdb.h wdb/wdbBpLib.h wdb/wdbSvcLib.h \
			wdb/wdbRtIfLib.h wdb/wdbCommIfLib.h \
			wdb/wdbMbufLib.h wdb/wdbRpcLib.h cacheLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_BANNER {
	NAME		WDB banner
	SYNOPSIS	print banner to console after the agent is initialized
	CONFIGLETTES	usrWdbBanner.c
	INIT_RTN	usrWdbBanner ();
	LINK_SYMS	printf
	HDR_FILES	version.h sysLib.h wdb/wdbLibP.h stdio.h
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_MEM {
	NAME		WDB memory access
	SYNOPSIS	read/write target memory
	MODULES		wdbMemLib.o
	INIT_RTN	wdbMemLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_REG {
	NAME		WDB register access
	SYNOPSIS	get/set registers
	MODULES		wdbRegLib.o
	INIT_RTN	wdbRegsLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_DIRECT_CALL {
	NAME		WDB callouts
	SYNOPSIS	call arbitrary functions directly from WDB
	MODULES		wdbDirectCallLib.o
	INIT_RTN	wdbDirectCallLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_GOPHER {
	NAME		WDB gopher
	SYNOPSIS	information gathering language used by many tools
	MODULES		wdbGopherLib.o
	INIT_RTN	wdbGopherLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_CTXT {
	NAME		WDB tasks
	SYNOPSIS	create/delete/manipulate tasks
	MODULES		wdbCtxLib.o
	INIT_RTN	wdbCtxLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_EVENTS {
	NAME		WDB events
	SYNOPSIS	asynchronous event handling needed for breakpoints etc.
	MODULES		wdbEvtLib.o
	INIT_RTN	wdbEventLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_EXIT_NOTIFY {
	NAME		WDB task exit notification
	SYNOPSIS	ability to notify the host when a task exits
	MODULES		wdbCtxExitLib.o
	INIT_RTN	wdbCtxExitLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_EXC_NOTIFY {
	NAME		WDB exception notification
	SYNOPSIS	notify the host when an exception occurs
	MODULES		wdbExcLib.o
	INIT_RTN	wdbExcLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_FUNC_CALL {
	NAME		WDB call functions
	SYNOPSIS	asynchronous function calls
	MODULES		wdbCallLib.o
	INIT_RTN	wdbFuncCallLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_VIO_LIB {
	NAME		WDB virtual I/O library
	SYNOPSIS	low-level virtual I/O handling
	MODULES		wdbVioLib.o
	INIT_RTN	wdbVioLibInit ();
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h wdb/wdbVioLib.h
	HELP		tgtsvr wdbVioDrv WDB
}

Component INCLUDE_WDB_VIO {
	NAME		VIO driver
	SYNOPSIS	vxWorks driver for accessing virtual I/O
	MODULES		wdbVioDrv.o
	INIT_RTN	wdbVioDrv ("/vio");
	HDR_FILES	drv/wdb/wdbVioDrv.h
	MACRO_NEST	INCLUDE_WDB
	HELP		tgtsvr  WDB
}

Component INCLUDE_WDB_BP {
	NAME		WDB breakpoints
	SYNOPSIS	core breakpoint library
	CONFIGLETTES	usrWdbBp.c usrBreakpoint.c
	MODULES		wdbBpLib.o
	INIT_RTN	usrWdbBp ();
	CFG_PARAMS	WDB_BP_MAX
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	vxLib.h cacheLib.h wdb/wdb.h wdb/wdbLib.h \
			wdb/wdbLibP.h wdb/wdbSvcLib.h wdb/wdbBpLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_TASK_BP {
	NAME		WDB task breakpoints
	SYNOPSIS	task-mode breakpoint library
	MODULES		wdbTaskBpLib.o
	INIT_RTN	wdbTaskBpLibInit ();
	INCLUDE_WHEN	INCLUDE_WDB_TASK INCLUDE_WDB_BP
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbLib.h
	REQUIRES	INCLUDE_WDB_TASK INCLUDE_WDB_BP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_DSP {
	NAME		WDB DSP support
	CONFIGLETTES	usrWdbDsp.c
	INIT_RTN	wdbDspInit ();
	INCLUDE_WHEN	INCLUDE_WDB INCLUDE_DSP
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB INCLUDE_DSP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_TASK_DSP {
	NAME		WDB task DSP support
	SYNOPSIS	task mode DSP debug support
	CONFIGLETTES	usrWdbTaskDsp.c
	INIT_RTN	wdbTaskDspInit ();
	INCLUDE_WHEN	INCLUDE_WDB_TASK INCLUDE_DSP
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB_TASK INCLUDE_DSP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_SYS_DSP {
	NAME		WDB system agent DSP support
	SYNOPSIS	system mode mode DSP debug support
	CONFIGLETTES	usrWdbSysDsp.c
	MODULES		wdbDspLib.o
	INIT_RTN	wdbSysDspInit ();
	INCLUDE_WHEN	INCLUDE_WDB_SYS	INCLUDE_DSP
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h \
			wdb/wdbRegs.h
	REQUIRES	INCLUDE_WDB_SYS INCLUDE_DSP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_HW_FP {
	NAME		WDB hardware fpp support
	CONFIGLETTES	usrWdbFpp.c
	INIT_RTN	wdbFppInit ();
	INCLUDE_WHEN	INCLUDE_WDB INCLUDE_HW_FP
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB INCLUDE_HW_FP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_TASK_HW_FP {
	NAME		WDB task hardware fpp support
	SYNOPSIS	task mode fpp debug support
	CONFIGLETTES	usrWdbTaskFpp.c
	INIT_RTN	wdbTaskFppInit ();
	INCLUDE_WHEN	INCLUDE_WDB_TASK INCLUDE_HW_FP
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_WDB_TASK INCLUDE_HW_FP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_SYS_HW_FP {
	NAME		WDB system agent hardware fpp support
	SYNOPSIS	system mode mode fpp debug support
	CONFIGLETTES	usrWdbSysFpp.c
	MODULES		wdbFpLib.o
	INIT_RTN	wdbSysFppInit ();
	INCLUDE_WHEN	INCLUDE_WDB_SYS	INCLUDE_HW_FP
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	string.h wdb/wdb.h wdb/wdbLib.h wdb/wdbLibP.h \
			wdb/wdbRegs.h
	REQUIRES	INCLUDE_WDB_SYS INCLUDE_HW_FP
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_SYS {
	NAME		WDB system debugging
	SYNOPSIS	A breakpoint stops the entire operating system.
	CONFIGLETTES	usrWdbSys.c
	INIT_RTN	wdbSysModeInit ();
	CFG_PARAMS	WDB_SPAWN_PRI		\
			WDB_SPAWN_OPTS
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	wdb/wdb.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_TASK {
	NAME		WDB task debugging
	SYNOPSIS	A breakpoint stops one task, while others keep running.
	CONFIGLETTES	usrWdbTask.c
	INIT_RTN	wdbTaskModeInit ();
	CFG_PARAMS	WDB_TASK_PRIORITY	\
			WDB_TASK_OPTIONS	\
			WDB_RESTART_TIME	\
			WDB_MAX_RESTARTS
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	cacheLib.h excLib.h string.h bufLib.h sioLib.h \
			sysLib.h ioLib.h taskHookLib.h private/taskLibP.h \
			private/kernelLibP.h private/vmLibP.h \
			private/funcBindP.h wdb/wdb.h wdb/wdbLib.h \
			wdb/wdbLibP.h wdb/wdbBpLib.h \
			wdb/wdbSvcLib.h wdb/wdbRtIfLib.h \
			wdb/wdbCommIfLib.h wdb/wdbMbufLib.h \
			wdb/wdbRpcLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_SERIAL {
	NAME		WDB serial connection
	SYNOPSIS	serial line connection
	CONFIGLETTES	wdbSerial.c
	MODULES		wdbSlipPktDrv.o
	CFG_PARAMS	WDB_TTY_CHANNEL		\
			WDB_TTY_BAUD		\
			INCLUDE_WDB_TTY_TEST	\
			WDB_TTY_ECHO		\
			WDB_MTU
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	private/funcBindP.h wdb/wdbLib.h wdb/wdbUdpLib.h \
			drv/wdb/wdbSlipPktDrv.h wdb/wdbLibP.h sysLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_TYCODRV_5_2 {
	NAME		WDB tyCoDrv connection
	SYNOPSIS	connect over a serial line in a version 1.0 BSP
	CONFIGLETTES	wdbTyco.c
	MODULES		wdbTyCoDrv.o
	EXCLUDES	INCLUDE_WDB_SYS
	CFG_PARAMS	WDB_TTY_DEV_NAME	\
			WDB_TTY_BAUD		\
			INCLUDE_WDB_TTY_TEST	\
			WDB_MTU
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpLib.h drv/wdb/wdbSlipPktDrv.h \
			wdb/wdbLib.h wdb/wdbTyCoDrv.h wdb/wdbLibP.h \
			wdb/wdbCommIfLib.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_NETWORK {
	NAME		WDB network connection
	SYNOPSIS	UDP/IP connection - only supports task mode debugging
	CONFIGLETTES	wdbNetwork.c
	MODULES		wdbUdpSockLib.o
	EXCLUDES	INCLUDE_WDB_SYS
	CFG_PARAMS	WDB_MTU
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpSockLib.h wdb/wdbLibP.h
	REQUIRES	INCLUDE_LOOPBACK INCLUDE_WDB
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_NETROM {
	NAME		WDB netROM connection
	SYNOPSIS	NETROM ROM-emulator connection
	CONFIGLETTES	wdbNetrom.c
	CFG_PARAMS	WDB_MTU			\
			WDB_NETROM_TYPE		\
			WDB_NETROM_WIDTH	\
			WDB_NETROM_INDEX	\
			WDB_NETROM_NUM_ACCESS	\
			WDB_NETROM_ROMSIZE	\
			WDB_NETROM_POLL_DELAY
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpLib.h wdb/wdbLib.h wdb/wdbLibP.h \
			drv/wdb/wdbNetromPktDrv.h
	HELP		tgtsvr wdbNetromPktDrv WDB
}

Component INCLUDE_WDB_COMM_END {
	NAME		WDB END driver connection
	SYNOPSIS	supports both task and system mode debugging
	CONFIGLETTES	wdbEnd.c
	MODULES		wdbEndPktDrv.o
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpLib.h wdb/wdbLib.h \
			wdb/wdbLibP.h drv/wdb/wdbEndPktDrv.h \
			private/funcBindP.h end.h muxLib.h
	REQUIRES	INCLUDE_END INCLUDE_WDB
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_PIPE {
	NAME		WDB simulator pipe connection
	SYNOPSIS	default connection for the simulators
	CONFIGLETTES	wdbPipe.c
	MODULES		wdbPipePktDrv.o
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpLib.h wdb/wdbLib.h wdb/wdbLibP.h \
			drv/wdb/wdbPipePktDrv.h
	HELP		tgtsvr WDB
}

Component INCLUDE_WDB_COMM_CUSTOM {
	NAME		WDB user-defined connection
	SYNOPSIS	callout to user-defined wdbCommDevInit
	MACRO_NEST	INCLUDE_WDB
	REQUIRES	INCLUDE_WDB
	HELP		tgtsvr WDB
}

Folder	FOLDER_APPLICATION {
	NAME		application components
	CHILDREN	INCLUDE_USER_APPL
	DEFAULTS	INCLUDE_USER_APPL
}

Component INCLUDE_USER_APPL {
	NAME		application initialization
	SYNOPSIS	call usrAppInit() (in your usrAppInit.c project file) after startup.
	INIT_RTN	usrAppInit ();
}

Folder	FOLDER_KERNEL {
	NAME		kernel components
	CHILDREN	INCLUDE_KERNEL		\
			INCLUDE_BOOT_LINE_INIT	\
			INCLUDE_SYS_START	\
			INCLUDE_SYSHW_INIT	\
			INCLUDE_EXC_HANDLING	\
			INCLUDE_MEM_MGR_BASIC	\
			INCLUDE_MEM_MGR_FULL	\
			INCLUDE_VXEVENTS	\
			INCLUDE_SEM_BINARY	\
			INCLUDE_SEM_MUTEX	\
			INCLUDE_SEM_COUNTING	\
			INCLUDE_SIGNALS		\
			INCLUDE_MSG_Q		\
			INCLUDE_WATCHDOGS	\
			INCLUDE_TASK_HOOKS	\
			INCLUDE_TASK_VARS	\
			INCLUDE_ENV_VARS	\
			INCLUDE_EXC_TASK
	DEFAULTS	INCLUDE_KERNEL INCLUDE_SYS_START INCLUDE_SYSHW_INIT \
			INCLUDE_EXC_HANDLING \
			INCLUDE_MEM_MGR_FULL INCLUDE_VXEVENTS \
			INCLUDE_SEM_BINARY \
			INCLUDE_SEM_MUTEX INCLUDE_SEM_COUNTING \
			INCLUDE_SIGNALS INCLUDE_MSG_Q INCLUDE_WATCHDOGS \
			INCLUDE_TASK_HOOKS INCLUDE_TASK_VARS \
			INCLUDE_ENV_VARS INCLUDE_EXC_TASK
}

Component INCLUDE_KERNEL {
	NAME		kernel
	SYNOPSIS	context switch and interrupt handling. DO NOT REMOVE.
	MODULES		kernelLib.o taskLib.o intLib.o
	CONFIGLETTES	usrKernel.c
	INIT_RTN	usrKernelInit ();
	CFG_PARAMS	INCLUDE_CONSTANT_RDY_Q	\
			ROOT_STACK_SIZE		\
			ISR_STACK_SIZE		\
			INT_LOCK_LEVEL
	REQUIRES	INCLUDE_MEMORY_CONFIG
	HDR_FILES	sysLib.h private/kernelLibP.h private/workQLibP.h \
			qPriBMapLib.h taskLib.h usrConfig.h
}

Component INCLUDE_ENV_VARS {
	NAME		environment variables
	MODULES		envLib.o
	INIT_RTN	envLibInit (ENV_VAR_USE_HOOKS);
	CFG_PARAMS	ENV_VAR_USE_HOOKS
	HDR_FILES	envLib.h
}

Component INCLUDE_EXC_TASK {
	NAME		exception task
	SYNOPSIS	miscellaneous support task
	MODULES		excLib.o
	INIT_RTN	excInit ();
	HDR_FILES	excLib.h
}

Component INCLUDE_MEM_MGR_FULL {
	NAME		full featured memory allocator
	MODULES		memLib.o
	INIT_RTN	memInit (pMemPoolStart, memPoolSize);
}

Component INCLUDE_VXEVENTS {
	NAME		VxWorks events
	MODULES		eventLib.o \
			semEvLib.o \
			msgQEvLib.o
	INIT_RTN	eventLibInit ();
	HDR_FILES	eventLib.h
}

Component INCLUDE_MSG_Q {
	NAME		message queues
	MODULES		msgQLib.o
	INIT_RTN	msgQLibInit ();
	HDR_FILES	msgQLib.h
}

Component INCLUDE_SEM_BINARY {
	NAME		binary semaphores
	MODULES		semLib.o semBLib.o
	INIT_RTN	semBLibInit ();
	HDR_FILES	semLib.h
}

Component INCLUDE_SEM_MUTEX {
	NAME		mutex semaphores
	MODULES		semMLib.o
	INIT_RTN	semMLibInit ();
	HDR_FILES	semLib.h
}

Component INCLUDE_SEM_COUNTING {
	NAME		counting semaphores
	MODULES		semCLib.o
	INIT_RTN	semCLibInit ();
	HDR_FILES	semLib.h
}

Component INCLUDE_TASK_HOOKS {
	NAME		task hooks
	SYNOPSIS	user callouts on task creation/deletion/context switch
	MODULES		taskHookLib.o
	INIT_RTN	taskHookInit ();
	HDR_FILES	taskHookLib.h
}

Component INCLUDE_TASK_VARS {
	NAME		task variables
	SYNOPSIS	allow global variables to be made private to a task
	MODULES		taskVarLib.o
	LINK_SYMS	taskVarInit
	HDR_FILES	taskVarLib.h
}

Component INCLUDE_WATCHDOGS {
	NAME		watchdog timers
	MODULES		wdLib.o
	INIT_RTN	wdLibInit ();
	HDR_FILES	wdLib.h
}

Component INCLUDE_MEM_MGR_BASIC {
	NAME		minimal memory allocator
	SYNOPSIS	core memory partition manager
	MODULES		memPartLib.o
	INIT_RTN	memPartLibInit (pMemPoolStart, memPoolSize);
	HDR_FILES	memLib.h
}

Component INCLUDE_SIGNALS {
	NAME		signals
	MODULES		sigLib.o
	INIT_RTN	sigInit ();
	HDR_FILES	sigLib.h
}

Folder	FOLDER_IO_SYSTEM {
	NAME		IO system components
	CHILDREN	INCLUDE_IO_SYSTEM	\
			INCLUDE_STDIO		\
			INCLUDE_FLOATING_POINT	\
			INCLUDE_FORMATTED_IO	\
			INCLUDE_LOGGING		\
			INCLUDE_PIPES		\
			INCLUDE_SELECT		\
			INCLUDE_SELECT_SUPPORT	\
			INCLUDE_CDROMFS		\
			INCLUDE_DOSFS		\
			INCLUDE_DISK_UTIL	\
			INCLUDE_RT11FS		\
			INCLUDE_RAWFS		\
			INCLUDE_RAMDRV		\
			INCLUDE_TTY_DEV
	DEFAULTS	INCLUDE_IO_SYSTEM INCLUDE_STDIO \
			INCLUDE_FORMATTED_IO INCLUDE_LOGGING INCLUDE_PIPES \
			INCLUDE_SELECT INCLUDE_SELECT_SUPPORT INCLUDE_TTY_DEV
}

Component INCLUDE_TTY_DEV {
	NAME		terminal driver
	MODULES		ttyDrv.o
	INIT_RTN	ttyDrv ();
	HDR_FILES	ttyLib.h
}

Component INCLUDE_DOSFS {
	NAME		DOS filesystem
	MODULES		dosFsLib.o
	INIT_RTN	dosFsInit (NUM_DOSFS_FILES);
	CFG_PARAMS	NUM_DOSFS_FILES
	HDR_FILES	dosFsLib.h
}

Component INCLUDE_CDROMFS {
	NAME		CDROM (ISO9960 High Sierra) filesystem
	MODULES		cdromFsLib.o
	INIT_RTN	cdromFsInit();
	HDR_FILES	cdromFsLib.h
}

Component INCLUDE_FLOATING_POINT {
	NAME		fpp formatting for printf
	SYNOPSIS	allow printf and others to format floats correctly
	MODULES		floatLib.o
	INIT_RTN	floatInit ();
	HDR_FILES	math.h
}

Component INCLUDE_FORMATTED_IO {
	NAME		formatted IO
	SYNOPSIS	formatting for printf, scanf, etc.
	MODULES		fioLib.o
	INIT_RTN	fioLibInit ();
	HDR_FILES	fioLib.h
}

Component INCLUDE_IO_SYSTEM {
	NAME		IO system
	MODULES		iosLib.o
	INIT_RTN	iosInit (NUM_DRIVERS, NUM_FILES, "/null");
	CFG_PARAMS	NUM_DRIVERS NUM_FILES
	HDR_FILES	iosLib.h
}

Component INCLUDE_LOGGING {
	NAME		message logging
	MODULES		logLib.o
	INIT_RTN	logInit (consoleFd, MAX_LOG_MSGS);
	CFG_PARAMS	MAX_LOG_MSGS
	HDR_FILES	logLib.h
}

Component INCLUDE_PIPES {
	NAME		pipes
	MODULES		pipeDrv.o
	INIT_RTN	pipeDrv ();
	HDR_FILES	pipeDrv.h
}

Component INCLUDE_RAMDRV {
	NAME		RAM disk driver
	SYNOPSIS	allows a filesystem to be put on top of RAM
	MODULES		ramDrv.o
	INIT_RTN	ramDrv ();
	HDR_FILES	ramDrv.h
}

Component INCLUDE_RAWFS {
	NAME		raw filesystem
	MODULES		rawFsLib.o
	INIT_RTN	rawFsInit (NUM_RAWFS_FILES);
	CFG_PARAMS	NUM_RAWFS_FILES
	HDR_FILES	rawFsLib.h
}

Component INCLUDE_RT11FS {
	NAME		RT11 filesystem
	MODULES		rt11FsLib.o
	INIT_RTN	rt11FsInit (NUM_RT11FS_FILES);
	CFG_PARAMS	NUM_RT11FS_FILES
	HDR_FILES	rt11FsLib.h
}

/* Select is really one component with two different init routines */

Component INCLUDE_SELECT {
	NAME		select
	MODULES		selectLib.o
	INIT_RTN	selectInit (NUM_FILES);
	HDR_FILES	selectLib.h
	REQUIRES	INCLUDE_SELECT_SUPPORT
	HELP		selectLib
}

/*
 * This is the second init routine for select. This is because
 * the select delete hook must be called before the RPC delete hook when
 * a task is deleted.
 * The task create hook is installed as part of selectInit() above.
 */

Component INCLUDE_SELECT_SUPPORT {
	NAME		install select task delete hook
	INIT_RTN	selTaskDeleteHookAdd ();
	REQUIRES	INCLUDE_SELECT
	HDR_FILES	selectLib.h
}

Component INCLUDE_STDIO {
	NAME		stdio
	SYNOPSIS	buffered IO library
	MODULES		stdioLib.o
	INIT_RTN	stdioInit ();
	HDR_FILES	stdio.h
}

Folder	FOLDER_UTILITIES {
	NAME		utility components
	CHILDREN	INCLUDE_HASH INCLUDE_BUF_MGR INCLUDE_DLL \
			INCLUDE_RNG_BUF INCLUDE_LSTLIB
}

Component INCLUDE_DISK_UTIL
        {
        NAME            File System and Disk Utilities
        SYNOPSIS        Target-resident File System utilities (cd, copy, ll, mkdir, etc)
        MODULES         usrFsLib.o
        HDR_FILES       usrLib.h
	LINK_SYMS	ioHelp
        }

Component INCLUDE_HASH {
	NAME		hash library
	MODULES		hashLib.o
	INIT_RTN	hashLibInit ();
	HDR_FILES	hashLib.h
}

Component INCLUDE_DLL {
	NAME		doubly linked lists
	MODULES		dllLib.o
	LINK_SYMS	dllInit
}

Component INCLUDE_LSTLIB {
	NAME		linked list library
	MODULES		lstLib.o
	LINK_SYMS	lstInit
	HDR_FILES	lstLib.h
}

Component INCLUDE_RNG_BUF {
	NAME		ring buffers
	MODULES		rngLib.o
	LINK_SYMS	rngCreate
}

Component INCLUDE_BUF_MGR {
	NAME		buffer manager
	MODULES		bufLib.o
	LINK_SYMS	bufPoolInit
}


Component INCLUDE_EXC_HANDLING {
	NAME		exception handling
	MODULES		excArchLib.o
	INIT_RTN	excVecInit ();
	HDR_FILES	excLib.h
}

Component INCLUDE_EXC_SHOW {
	NAME		exception show routines
	MODULES		excArchShow.o
	INIT_RTN	excShowInit ();
	HDR_FILES	excLib.h
}

Component INCLUDE_MMU_FULL_SHOW {
	NAME		MMU show routine
	MODULES		vmShow.o
	INIT_RTN	vmShowInit ();
	HDR_FILES	vmLib.h
}

Component INCLUDE_BOOT_LINE_INIT
        {
        NAME            read the bootline
        SYNOPSIS        parse some boot device configuration info 
        CONFIGLETTES    net/usrBootLine.c
        INIT_RTN        usrBootLineParse (BOOT_LINE_ADRS);
	HDR_FILES	sysLib.h private/funcBindP.h
        }

Folder	FOLDER_WINDVIEW {
	NAME		Windview components
	CHILDREN	INCLUDE_WINDVIEW 	\
			INCLUDE_WINDVIEW_CLASS 	\
                        INCLUDE_WVNET           \
			SELECT_TIMESTAMP 	\
                        SELECT_WVUPLOAD 	\
                        SELECT_WV_BUFFER
	DEFAULTS	INCLUDE_WINDVIEW 	\
			INCLUDE_WINDVIEW_CLASS 	\
			SELECT_TIMESTAMP 	\
                        SELECT_WV_BUFFER	\
			SELECT_WVUPLOAD	
}

Selection	SELECT_WVUPLOAD {
	NAME		upload path(s)
	SYNOPSIS	choose among TSFS, NFS and/or TCP/IP
	COUNT		1-
	CHILDREN	INCLUDE_WVUPLOAD_FILE	\
			INCLUDE_WVUPLOAD_SOCK	\
			INCLUDE_WVUPLOAD_TSFSSOCK 
	DEFAULTS	INCLUDE_WVUPLOAD_FILE   \
	        	INCLUDE_WVUPLOAD_TSFSSOCK 
}

Component INCLUDE_WVUPLOAD_FILE {
	NAME		file upload path initialization 
	SYNOPSIS	initialize path for the upload to file 
	CONFIGLETTES	usrWvFileUploadPath.c
	MODULES		wvFileUploadPathLib.o 
	INIT_RTN	usrWvFileUploadPathInit ();
	HDR_FILES	private/wvUploadPathP.h private/wvFileUploadPathLibP.h
	REQUIRES	INCLUDE_WINDVIEW
}

Component INCLUDE_WVUPLOAD_SOCK {
	NAME		TCP/IP socket upload path initialization 
	SYNOPSIS	initialize path for the upload through TCP/IP sockets 
	CONFIGLETTES	usrWvSockUploadPath.c
	MODULES		wvSockUploadPathLib.o 
	INIT_RTN	usrWvSockUploadPathInit ();
	HDR_FILES	private/wvUploadPathP.h private/wvSockUploadPathLibP.h
	REQUIRES	INCLUDE_WINDVIEW
}

Component INCLUDE_WVUPLOAD_TSFSSOCK {
	NAME		TSFS upload path initialization 
	SYNOPSIS	initialize path for the upload through TSFS socket 
	CONFIGLETTES	usrWvTsfsUploadPath.c
	MODULES		wvTsfsUploadPathLib.o 
	INIT_RTN	usrWvTsfsUploadPathInit ();
	HDR_FILES	private/wvUploadPathP.h private/wvTsfsUploadPathLibP.h
	REQUIRES	INCLUDE_WINDVIEW INCLUDE_WDB_TSFS
	HELP		tgtsvr wdbTsfsDrvDrv WDB
}

Component INCLUDE_WDB_TSFS {
	NAME		WDB target server file system
	SYNOPSIS	virtual file system based on the WDB agent
	MODULES		wdbTsfsDrv.o
	INIT_RTN	wdbTsfsDrv ("/tgtsvr");
	HDR_FILES	private/wvUploadPathP.h private/wvTsfsUploadPathLibP.h wdb/wdbVioLib.h
	INCLUDE_WHEN	INCLUDE_WVUPLOAD_TSFSSOCK
	HELP		tgtsvr wdbTsfsDrvDrv WDB
}

Selection	SELECT_TIMESTAMP {
	NAME		select timestamping
	COUNT		1-1
	CHILDREN	INCLUDE_SYS_TIMESTAMP	\
			INCLUDE_USER_TIMESTAMP	\
			INCLUDE_SEQ_TIMESTAMP
	DEFAULTS	INCLUDE_SEQ_TIMESTAMP
}

Component INCLUDE_SYS_TIMESTAMP {
	NAME		system-defined timestamping
	SYNOPSIS	bsp specific timestamp routines 
	INIT_RTN	wvTmrRegister ((UINTFUNCPTR) sysTimestamp,	\
				(UINTFUNCPTR) sysTimestampLock,	        \
				(FUNCPTR)     sysTimestampEnable,	\
				(FUNCPTR)     sysTimestampDisable,	\
				(FUNCPTR)     sysTimestampConnect,	\
				(UINTFUNCPTR) sysTimestampPeriod,	\
				(UINTFUNCPTR) sysTimestampFreq);
	HDR_FILES	wvTmrLib.h sysLib.h
	REQUIRES	INCLUDE_TIMESTAMP INCLUDE_WINDVIEW
}

Component INCLUDE_USER_TIMESTAMP {
	NAME		user-defined timestamping
	SYNOPSIS	user-defined timestamp routines
	INIT_RTN	wvTmrRegister ((UINTFUNCPTR) USER_TIMESTAMP,	\
				(UINTFUNCPTR) USER_TIMESTAMPLOCK,	\
				(FUNCPTR)     USER_TIMEENABLE,		\
				(FUNCPTR)     USER_TIMEDISABLE,		\
				(FUNCPTR)     USER_TIMECONNECT,		\
				(UINTFUNCPTR) USER_TIMEPERIOD,		\
				(UINTFUNCPTR) USER_TIMEFREQ);
	CFG_PARAMS	USER_TIMESTAMP		\
			USER_TIMESTAMPLOCK	\
			USER_TIMEENABLE		\
			USER_TIMEDISABLE	\
			USER_TIMECONNECT	\
			USER_TIMEPERIOD		\
			USER_TIMEFREQ
	HDR_FILES	wvTmrLib.h
	REQUIRES	INCLUDE_WINDVIEW
}

Component INCLUDE_SEQ_TIMESTAMP {
	NAME		sequential timestamping
	SYNOPSIS	no timestamping
	MODULES		seqDrv.o
	INIT_RTN	wvTmrRegister ((UINTFUNCPTR) seqStamp,	\
				(UINTFUNCPTR) seqStampLock,	\
				(FUNCPTR)     seqEnable,	\
				(FUNCPTR)     seqDisable,	\
				(FUNCPTR)     seqConnect,	\
				(UINTFUNCPTR) seqPeriod,	\
				(UINTFUNCPTR) seqFreq);
	HDR_FILES	wvTmrLib.h private/seqDrvP.h
	REQUIRES	INCLUDE_WINDVIEW
}

Component INCLUDE_WINDVIEW {
	NAME		windview library
	SYNOPSIS	initialize and control event logging
	CONFIGLETTES	usrWindview.c
	MODULES		wvLib.o evtLogLib.o 
	INIT_RTN	windviewConfig ();
	CFG_PARAMS	WV_DEFAULT_BUF_MIN	\
			WV_DEFAULT_BUF_MAX	\
			WV_DEFAULT_BUF_SIZE	\
			WV_DEFAULT_BUF_THRESH	\
			WV_DEFAULT_BUF_OPTIONS	
	HDR_FILES	wvLib.h rBuffLib.h
	REQUIRES	SELECT_TIMESTAMP SELECT_WVUPLOAD SELECT_WV_BUFFER
}

Selection SELECT_WV_BUFFER {
	NAME		windview buffer management library
	COUNT		1-1
	CHILDREN	INCLUDE_RBUFF INCLUDE_WV_BUFF_USER
	DEFAULTS	INCLUDE_RBUFF
}

Component INCLUDE_WINDVIEW_CLASS {
	NAME		windview class instrumentation
	SYNOPSIS	low-level kernel instrumentation needed by windview
	INIT_RTN	wvLibInit ();
	MODULES		wvLib.o
}

Component INCLUDE_RBUFF {
	NAME		rBuff library
	SYNOPSIS	windview 2.0 ring of buffers for event logging 
	MODULES		rBuffLib.o 
	INIT_RTN	rBuffLibInit ();
	HDR_FILES	rBuffLib.h
	INCLUDE_WHEN	INCLUDE_WINDVIEW
}

Component INCLUDE_WV_BUFF_USER {
	NAME		user-defined windview buffer manager
	SYNOPSIS	user defined library for managing windview buffers
	INIT_RTN	USER_WV_BUFF_INIT
	CFG_PARAMS	USER_WV_BUFF_INIT
}

Component INCLUDE_WVNET {
        NAME            network instrumentation
        SYNOPSIS        WindView instrumentation for network
        MODULES         wvNetLib.o
        INIT_RTN        wvNetInit ();
        HDR_FILES       wvNetLib.h
}

Component INCLUDE_TRIGGERING {
        NAME            triggering library
        SYNOPSIS        triggering for system and user events
        MODULES         trgLib.o
        INIT_RTN        trgInit ();
        HDR_FILES       trgLib.h private/trgLibP.h
}

/* INITIALIZATION ORDER */

InitGroup usrInit {
	PROTOTYPE	void usrInit (int startType)
	SYNOPSIS	pre-kernel initialization
	INIT_ORDER	INCLUDE_SYS_START	\
			INCLUDE_CACHE_SUPPORT	\
			INCLUDE_EXC_HANDLING	\
			INCLUDE_SYSHW_INIT	\
			INCLUDE_CACHE_ENABLE	\
			INCLUDE_WINDVIEW_CLASS	\
			INCLUDE_KERNEL
}

InitGroup usrToolsInit {
	INIT_RTN	usrToolsInit ();
	SYNOPSIS	software development tools
	INIT_ORDER	INCLUDE_SPY INCLUDE_TIMEX \
			INCLUDE_MODULE_MANAGER	\
			INCLUDE_LOADER \
			INCLUDE_NET_SYM_TBL \
			INCLUDE_STANDALONE_SYM_TBL \
			INCLUDE_STAT_SYM_TBL \
			INCLUDE_TRIGGERING	\
			usrWdbInit usrShellInit \
			usrWindviewInit		\
			usrShowInit
}

InitGroup usrWindviewInit {
	INIT_RTN	usrWindviewInit ();
	INIT_ORDER	INCLUDE_WINDVIEW	\
			INCLUDE_SYS_TIMESTAMP	\
			INCLUDE_USER_TIMESTAMP	\
			INCLUDE_SEQ_TIMESTAMP	\
			INCLUDE_RBUFF		\
			INCLUDE_WV_BUFF_USER	\
			INCLUDE_WDB_TSFS 	\
			INCLUDE_WVUPLOAD_SOCK	\
			INCLUDE_WVUPLOAD_TSFSSOCK\
			INCLUDE_WVUPLOAD_FILE   \
                        INCLUDE_WVNET
}

InitGroup usrWdbInit {
	INIT_RTN	usrWdbInit ();
	SYNOPSIS	the WDB target agent
	INIT_ORDER	INCLUDE_WDB		\
			INCLUDE_WDB_MEM		\
			INCLUDE_WDB_SYS		\
			INCLUDE_WDB_TASK	\
			INCLUDE_WDB_EVENTS	\
			INCLUDE_WDB_EVENTPOINTS	\
			INCLUDE_WDB_DIRECT_CALL	\
			INCLUDE_WDB_CTXT	\
			INCLUDE_WDB_REG		\
			INCLUDE_WDB_GOPHER	\
			INCLUDE_WDB_EXIT_NOTIFY	\
			INCLUDE_WDB_EXC_NOTIFY	\
			INCLUDE_WDB_FUNC_CALL	\
			INCLUDE_WDB_VIO_LIB	\
			INCLUDE_WDB_VIO		\
			INCLUDE_WDB_BP		\
			INCLUDE_WDB_TASK_BP	\
			INCLUDE_WDB_START_NOTIFY \
			INCLUDE_WDB_USER_EVENT	\
			INCLUDE_WDB_HW_FP	\
			INCLUDE_WDB_TASK_HW_FP	\
			INCLUDE_WDB_SYS_HW_FP	\
			INCLUDE_WDB_DSP		\
			INCLUDE_WDB_TASK_DSP	\
			INCLUDE_WDB_SYS_DSP	\
			INCLUDE_WDB_BANNER	\
			INCLUDE_SYM_TBL_SYNC
}

InitGroup usrShellInit {
	INIT_RTN	usrShellInit ();
	SYNOPSIS	the target shell
	INIT_ORDER	INCLUDE_DEBUG \
			INCLUDE_SHELL_BANNER \
			INCLUDE_STARTUP_SCRIPT \
			INCLUDE_SHELL
}

InitGroup usrShowInit {
	INIT_RTN	usrShowInit ();
	SYNOPSIS	enable object show routines
	INIT_ORDER	INCLUDE_TASK_SHOW \
			INCLUDE_CLASS_SHOW \
			INCLUDE_MEM_SHOW \
			INCLUDE_TASK_HOOKS_SHOW \
			INCLUDE_SEM_SHOW \
			INCLUDE_MSG_Q_SHOW \
			INCLUDE_WATCHDOGS_SHOW \
			INCLUDE_SYM_TBL_SHOW \
			INCLUDE_MMU_FULL_SHOW \
			INCLUDE_POSIX_MQ_SHOW \
			INCLUDE_POSIX_SEM_SHOW \
			INCLUDE_HW_FP_SHOW \
			INCLUDE_DSP_SHOW \
			INCLUDE_ATA_SHOW \
			INCLUDE_TRIGGER_SHOW \
			INCLUDE_RBUFF_SHOW \
			INCLUDE_STDIO_SHOW
}

InitGroup usrKernelCoreInit {
	INIT_RTN	usrKernelCoreInit ();
	SYNOPSIS	core kernel facilities
	INIT_ORDER	INCLUDE_VXEVENTS		\
			INCLUDE_SEM_BINARY 	\
			INCLUDE_SEM_MUTEX 	\
			INCLUDE_SEM_COUNTING 	\
			INCLUDE_MSG_Q 		\
			INCLUDE_WATCHDOGS 	\
			INCLUDE_TASK_HOOKS
}

InitGroup usrKernelExtraInit {
	INIT_RTN	usrKernelExtraInit ();
	SYNOPSIS	extended kernel facilities
	INIT_ORDER	INCLUDE_HASH		\
			INCLUDE_SYM_TBL 	\
			INCLUDE_ENV_VARS 	\
			INCLUDE_SIGNALS 	\
			INCLUDE_POSIX_AIO 	\
			INCLUDE_POSIX_AIO_SYSDRV  \
			INCLUDE_POSIX_MQ 	\
			INCLUDE_POSIX_PTHREADS 	\
			INCLUDE_POSIX_SEM 	\
			INCLUDE_POSIX_SIGNALS 	\
			INCLUDE_PROTECT_TEXT 	\
			INCLUDE_PROTECT_VEC_TABLE 
}

InitGroup usrIosCoreInit {
	INIT_RTN	usrIosCoreInit ();
	SYNOPSIS	core I/O system
	INIT_ORDER	INCLUDE_SW_FP \
			INCLUDE_HW_FP \
			INCLUDE_DSP \
			INCLUDE_BOOT_LINE_INIT \
			INCLUDE_IO_SYSTEM \
			INCLUDE_TTY_DEV \
			INCLUDE_TYCODRV_5_2 \
			INCLUDE_SIO \
			INCLUDE_PC_CONSOLE
	}

InitGroup usrIosExtraInit {
	INIT_RTN	usrIosExtraInit ();
	SYNOPSIS	extended I/O system
	INIT_ORDER	INCLUDE_EXC_SHOW \
			INCLUDE_EXC_TASK \
			INCLUDE_LOGGING \
			INCLUDE_PIPES \
			INCLUDE_STDIO \
			INCLUDE_FORMATTED_IO \
			INCLUDE_FLOATING_POINT \
			INCLUDE_CDROMFS \
			INCLUDE_DOSFS \
			INCLUDE_RAWFS \
			INCLUDE_RT11FS \
			INCLUDE_RAMDRV \
			INCLUDE_SCSI \
			INCLUDE_FD \
			INCLUDE_IDE \
			INCLUDE_ATA \
			INCLUDE_LPT \
			INCLUDE_PCMCIA \
			INCLUDE_TFFS \
			INCLUDE_TFFS_DOSFS
}



InitGroup usrRoot {
	PROTOTYPE	void usrRoot (char *pMemPoolStart, unsigned memPoolSize)
	SYNOPSIS	entry point for post-kernel initialization
	INIT_ORDER	usrKernelCoreInit	\
			INCLUDE_MEM_MGR_FULL	\
			INCLUDE_MEM_MGR_BASIC	\
			INCLUDE_MMU_BASIC 	\
			INCLUDE_MMU_FULL	\
			INCLUDE_MMU_MPU		\
			INCLUDE_SYSCLK_INIT	\
			INCLUDE_SELECT		\
			usrIosCoreInit		\
			usrKernelExtraInit	\
			usrIosExtraInit		\
			usrNetworkInit		\
			INCLUDE_SELECT_SUPPORT	\
			usrToolsInit		\
                        INCLUDE_CTORS_DTORS     \
			INCLUDE_CPLUS		\
			INCLUDE_CPLUS_DEMANGLER	\
			INCLUDE_HTTP		\
			INCLUDE_USER_APPL
}


Parameter INT_LOCK_LEVEL {
	NAME		INT lock level
	SYNOPSIS	Interrupt lock level
}

Parameter ROOT_STACK_SIZE {
	NAME		root stack size
	SYNOPSIS	Root task stack size (bytes)
}

Parameter SHELL_STACK_SIZE {
	NAME		shell stack size
	SYNOPSIS	Shell stack size (bytes)
}

Parameter WDB_STACK_SIZE {
	NAME		WDB Stack size
	SYNOPSIS	WDB Stack size (bytes)
}

Parameter ISR_STACK_SIZE {
	NAME		ISR stack size
	SYNOPSIS	ISR Stack size (bytes)
}

Parameter USER_I_MMU_ENABLE {
	NAME		instruction MMU
	SYNOPSIS	Enable instuction MMU
	TYPE		exists
	DEFAULT		TRUE
}

Parameter USER_D_MMU_ENABLE {
	NAME		data MMU
	SYNOPSIS	Enable data MMU
	TYPE		exists
	DEFAULT		TRUE
}



Parameter NUM_RT11FS_FILES {
	NAME		max # open RT11FS files
	TYPE		uint
	DEFAULT		5
}

Parameter NUM_RAWFS_FILES {
	NAME		max # open RAWFS files
	TYPE		uint
	DEFAULT		5
}

Parameter NUM_DOSFS_FILES {
	NAME		max # open DOSFS files
	TYPE		uint
	DEFAULT		20
}

Parameter MAX_LOG_MSGS {
	NAME		max # queued messages
	TYPE		uint
	DEFAULT		50
}

Parameter NUM_DRIVERS {
	NAME		max # drivers
	TYPE		uint
	DEFAULT		20
}

Parameter NUM_FILES {
	SYNOPSIS	Limit to FD_SETSIZE (2048) when using selectLib
	NAME		max # open files
	TYPE		uint
	DEFAULT		50
}

Parameter CLEAR_BSS {
	NAME		clear BSS at system startup (recomended)
	TYPE		exists
	DEFAULT		TRUE
}


Parameter MAX_LIO_CALLS {
	NAME		max outstanding lio calls, 0=default
	TYPE		uint
	DEFAULT		0
}

Parameter MAX_AIO_SYS_TASKS {
	NAME		max aio system tasks, 0 = default
	TYPE		uint
	DEFAULT		0
}

Parameter AIO_TASK_PRIORITY {
	NAME		aio system tasks priority, 0 = default
	TYPE		uint
	DEFAULT		0
}

Parameter AIO_TASK_STACK_SIZE {
	NAME		aio tasks stack size, 0 = default
	TYPE		uint
	DEFAULT		0
}

Parameter MQ_HASH_SIZE {
	NAME		POSIX message queue hash table size, 0 = default
	TYPE		uint
	DEFAULT		0
}

Parameter NUM_SIGNAL_QUEUES {
	NAME		POSIX queued signal count
	TYPE		uint
	DEFAULT		16
}

Parameter LOGIN_USER_NAME {
	NAME		rlogin/telnet user name
	DEFAULT		"target"
}

Parameter LOGIN_PASSWORD {
	NAME		rlogin/telnet encryted password
	DEFAULT		"bReb99RRed"
}

Parameter SYM_TBL_HASH_SIZE_LOG2 {
	NAME		symbol table hash size log 2
	TYPE		uint
	DEFAULT		8
}

Parameter WDB_TASK_PRIORITY {
	NAME		WDB task priority
	TYPE		uint
	DEFAULT		3
}

Parameter WDB_TASK_OPTIONS {
	NAME		WDB task options
	DEFAULT		VX_UNBREAKABLE | VX_FP_TASK
}

Parameter WDB_RESTART_TIME {
	NAME		delay before error-restarting agent
	TYPE		uint
	DEFAULT		10
}

Parameter WDB_MAX_RESTARTS {
	NAME		max # agent restarts on error
	TYPE		uint
	DEFAULT		5
}

Parameter WDB_SPAWN_PRI {
	NAME		default stack size for spawned tasks
	TYPE		uint
	DEFAULT		100
}

Parameter WDB_SPAWN_OPTS {
	NAME		default options for spawned tasks
	TYPE		uint
	DEFAULT		VX_FP_TASK
}

Parameter WDB_BP_MAX {
	NAME		max # of break points
	TYPE		uint
	DEFAULT		50
}

Parameter STAT_TBL_HASH_SIZE_LOG2 {
	NAME		error table hash size log 2
	TYPE		uint
	DEFAULT		6
}

Parameter INCLUDE_CONSTANT_RDY_Q {
	NAME		constant-time kernel ready queue
	TYPE		exists
	DEFAULT		TRUE
}

Parameter INCLUDE_WDB_TTY_TEST {
	SYNOPSIS	print WDB READY out the serial port on startup.
	TYPE		exists
	DEFAULT		TRUE
}

Parameter WDB_TTY_ECHO {
	NAME		stop boot to echo characters in INCLUDE_WDB_TTY_TEST
	TYPE		bool
	DEFAULT		FALSE
}

Parameter WDB_TTY_DEV_NAME {
	NAME		WDB tty device name
	DEFAULT		/tyCo/1
}

Parameter WDB_TTY_BAUD {
	NAME		baud rate for WDB serial channel
	TYPE		uint
	DEFAULT		9600
}

Parameter WDB_TTY_CHANNEL {
	NAME		serial channel for WDB connection
	TYPE		uint
	DEFAULT		1
}

Parameter WDB_MTU {
	NAME		MTU for WDB communication channel
	TYPE		uint
	DEFAULT		1500
}

Parameter ENV_VAR_USE_HOOKS {
	NAME		install environment variable task create/delete hooks
	TYPE		bool
	DEFAULT		TRUE
}


Parameter WDB_NETROM_INDEX {
	DEFAULT		0
	TYPE		uint
}

Parameter WDB_NETROM_NUM_ACCESS {
	SYNOPSIS	number of accesses to pod zero per byte read
	DEFAULT		1
	TYPE		uint
}

Parameter WDB_NETROM_POLL_DELAY {
	SYNOPSIS	clock tick interval to poll for data
	DEFAULT		2
	TYPE		uint
}

Parameter WDB_NETROM_ROMSIZE {
	SYNOPSIS	size of one ROM - divide by N if board has N ROMS
	DEFAULT		ROM_SIZE
}

Parameter WDB_NETROM_TYPE {
	SYNOPSIS	default is new 500 series
	DEFAULT		500
	TYPE		uint
}

Parameter WDB_NETROM_WIDTH {
	SYNOPSIS	not used for 500 series netroms
	DEFAULT		1
	TYPE		uint
}

Parameter USER_WV_BUFF_INIT {
	NAME		routine to initialize user-defined buffer manager
}

Parameter USER_TIMESTAMP {
	NAME	        Timestamp routine
	SYNOPSIS	user defined timestamp routine address
}

Parameter USER_TIMESTAMPLOCK {
	NAME	        Timestamp with intLock 
	SYNOPSIS	same as Timestamp but with interrupts locked
}

Parameter USER_TIMEENABLE {
	NAME	        enable Timestamp routine
	SYNOPSIS	enable a timestamp timer interrupt
}

Parameter USER_TIMEDISABLE {
	NAME	        disable Timestamp routine
	SYNOPSIS	disable a timestamp timer interrupt
}

Parameter USER_TIMECONNECT {
	NAME	        connect Timestamp routine to interrupt
	SYNOPSIS	connect a user routine to a timestamp timer interrupt
}

Parameter USER_TIMEPERIOD {
	NAME	        get the period of timestamp 
	SYNOPSIS	get the period of a timestamp timer
}

Parameter USER_TIMEFREQ {
	NAME	        get frequency of imestamp 
	SYNOPSIS	get a timestamp timer clock frequency
}

Parameter WV_DEFAULT_BUF_MIN {
	NAME		min number of buffers 
	TYPE		int
	DEFAULT		4
}

Parameter WV_DEFAULT_BUF_MAX {
	NAME		max number of buffers 
	TYPE		int
	DEFAULT		10
}

Parameter WV_DEFAULT_BUF_SIZE {
	NAME		size of each buffer 
	TYPE		int
	DEFAULT		0x8000
}

Parameter WV_DEFAULT_BUF_THRESH {
	NAME		buffer threshold
	TYPE		int
	DEFAULT		0x4000
}

Parameter WV_DEFAULT_BUF_OPTIONS {
	NAME		buffer options
	TYPE		int
	DEFAULT		0x0
}


