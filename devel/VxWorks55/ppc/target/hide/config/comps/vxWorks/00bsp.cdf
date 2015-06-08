/* 00bsp.cdf - BSP component description file */

/* Copyright 1984 - 2001 Wind River Systems, Inc.  */


/*
modification history
--------------------
01q,21jun02,jkf  SPR#74185, removed STACK_ADRS and STACK_RESIDENT params 
01p,20jun02,h_k  fixed Component INCLUDE_DSP (SPR #78892)
01o,02apr02,gls  added MMU params to INCLUDE_MMU components (SPR #74158)
01n,05mar02,yp   added CFI drver for AMD devices to TrueFFS
01m,13nov01,nrv  changed TFFS support by creating FOLDER_TFFS
01n,09nov01,brk  spelling mistake (SPR 63637)
01m,09oct01,rae  merge from truestack ver 01n base o1m (SPR #67023)
01l,17may01,rec   SPR 63492 - cache not enabled from project make
01l,16nov00,zl   added DSP component.
01m,07dec00,pai  Merged in WRN fixes for project builds in the 'Barney'
                 release.
01l,16nov00,cmf  added parameters to BSP Memory Configuration Component for
                 SPR 31071
01l,06dec00,spm  renamed usrConfigIf.c for project tool build fix
01k,22apr99,pr	 added dependency on DOSFS for ATA
01j,10mar99,yh	 chg'd name to INCLUDE_USR_ENTRIES (SPR 25331)
01i,26feb99,dat  added FEI driver to list, (SPR 23818)
01h,25feb99,cth  added CONSOLE_BAUD_RATE param to INCLUDE_PC_CONSOLE(SPR 24862)
01g,23feb99,cth  added CONSOLE_BAUD_RATE param to INCLUDE_SIO (SPR 24862)
01f,07jan99,cjs  fixed spelling error in folder description 
01e,15sep98,ms   fixed some errors uncovered by new cxrDocValidate.
01d,19aug98,sut  Moved BSP components from 00vxWorks.cdf
01c,18aug98,ann  Removed LOOPBACK dependancy on BSD
01b,18aug98,sut  fixed the SYSCLK_INIT
01a,23jul98,sut  written
*/

/*
DESCRIPTION
This file contains descriptions for some BSP components.
The configuration tool searches for .cdf files in four
places; here, arch/<arch>, config/<bsp>, and the project directory.
This file describes "generic" components which are available
to all BSPs.
If a new component is released, a new .cdf file should be deposited
in the appropriate directory, rather than modifying existing files.
*/


/* Generic BSP configuration parameters */


Folder	FOLDER_HARDWARE {
	NAME		hardware
	SYNOPSIS	Hardware components
	CHILDREN	FOLDER_MEMORY		\
			FOLDER_BUSES		\
			FOLDER_PERIPHERALS	\
			FOLDER_BSP_CONFIG
}

Folder FOLDER_MEMORY {
	NAME		memory
	SYNOPSIS	Memory components
	CHILDREN	INCLUDE_MEMORY_CONFIG	\
			FOLDER_MMU		\
			INCLUDE_CACHE_SUPPORT	\
			INCLUDE_CACHE_ENABLE
}


Folder FOLDER_MMU {
	NAME		MMU
	CHILDREN	INCLUDE_PROTECT_TEXT		\
			INCLUDE_PROTECT_VEC_TABLE	\
			SELECT_MMU
	DEFAULTS	SELECT_MMU
}


Folder FOLDER_BUSES {
	NAME		buses
	SYNOPSIS	Bus components
	CHILDREN	INCLUDE_PCI_PARAMS	\
			INCLUDE_VME_PARAMS	\
			INCLUDE_PCMCIA		\
			FOLDER_SCSI
}

Folder	FOLDER_SCSI {
	NAME		scsi
	SYNOPSIS	SCSI components
	CHILDREN	INCLUDE_SCSI		\
			SELECT_SCSI_VERSION
}

Folder FOLDER_PERIPHERALS {
	NAME		peripherals
	SYNOPSIS	Peripheral components
	CHILDREN	INCLUDE_LPT	\
			INCLUDE_FD	\
			FOLDER_HD	\
			FOLDER_CLOCK	\
			FOLDER_SERIAL	\
			FOLDER_TFFS	\
			FOLDER_FPP	\
			INCLUDE_DSP
}


Folder	FOLDER_HD {
	NAME		hard disks
	SYNOPSIS	Hard disk components
	CHILDREN	INCLUDE_IDE	\
			INCLUDE_ATA
}


Folder	FOLDER_CLOCK {
	NAME		clocks
	SYNOPSIS	Clock component
	CHILDREN	INCLUDE_SYSCLK_INIT	\
			INCLUDE_AUX_CLK		\
			INCLUDE_TIMESTAMP
}


Folder	FOLDER_SERIAL {
	NAME		serial
	SYNOPSIS	serial
	CHILDREN	INCLUDE_SIO		\
			INCLUDE_PC_CONSOLE

}


Folder  FOLDER_TFFS {
        NAME          TrueFFS
        SYNOPSIS      TFFS components
        CHILDREN      INCLUDE_TFFS \
                      INCLUDE_TFFS_SHOW \
                      FOLDER_TFFS_DRIVERS \
                      FOLDER_TFFS_TL
}


Folder  FOLDER_TFFS_DRIVERS {
        NAME                  TrueFFS drivers
        CHILDREN              INCLUDE_MTD_AMD \
                              INCLUDE_MTD_I28F008 \
                              INCLUDE_MTD_I28F008BAJA \
                              INCLUDE_MTD_I28F016 \
                              INCLUDE_MTD_WAMDMTD \
                              INCLUDE_MTD_CFIAMD \
                              INCLUDE_MTD_CFISCS
}


Folder  FOLDER_TFFS_TL {
        NAME             TrueFFS translation layer
        CHILDREN         INCLUDE_TL_FTL \
                         INCLUDE_TL_SSFDC
}


Folder	FOLDER_BSP_CONFIG {
	NAME		BSP configuration variants
}

Component INCLUDE_MEMORY_CONFIG {
	NAME		BSP Memory Configuration
	SYNOPSIS	Memory configuration parameter for BSP
	CFG_PARAMS      LOCAL_MEM_LOCAL_ADRS	\
			LOCAL_MEM_SIZE		\
			LOCAL_MEM_AUTOSIZE	\
			USER_RESERVED_MEM	\
			NV_RAM_SIZE		\
			NV_BOOT_OFFSET		\
			VEC_BASE_ADRS		\
			EXC_MSG_OFFSET		\
			EXC_MSG_ADRS		\
			BOOT_LINE_SIZE		\
			BOOT_LINE_ADRS		\
			BOOT_LINE_OFFSET	\
			DEFAULT_BOOT_LINE	\
			RESERVED		\
			FREE_RAM_ADRS		\
			ROM_WARM_ADRS		\
			STACK_SAVE		\
			RAM_HIGH_ADRS		\
			RAM_LOW_ADRS		\
			ROM_BASE_ADRS		\
			ROM_TEXT_ADRS		\
			ROM_SIZE
}

Component INCLUDE_CACHE_SUPPORT {
	NAME		cache support
	SYNOPSIS	include cache support
	INIT_RTN	cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE);
	MODULES		cacheLib.o
	CFG_PARAMS	USER_I_CACHE_MODE USER_D_CACHE_MODE
	HDR_FILES	cacheLib.h
}

Component INCLUDE_CACHE_ENABLE {
	NAME		enable caches
	SYNOPSIS	optionally enable caches
	CONFIGLETTES	usrCache.c
	INIT_RTN	usrCacheEnable ();
	CFG_PARAMS	USER_I_CACHE_ENABLE USER_D_CACHE_ENABLE
	REQUIRES	INCLUDE_CACHE_SUPPORT
	INCLUDE_WHEN	INCLUDE_CACHE_SUPPORT
	HDR_FILES	cacheLib.h
}

Component INCLUDE_PROTECT_TEXT {
	NAME		write-protect program text
	INIT_RTN	vmTextProtect ();
	REQUIRES	INCLUDE_MMU_FULL
	HDR_FILES	vmLib.h
}

Component INCLUDE_PROTECT_VEC_TABLE {
	NAME		write-protect vector table
	INIT_RTN	intVecTableWriteProtect ();
	REQUIRES	INCLUDE_MMU_FULL
	HDR_FILES	intLib.h
}

Selection SELECT_MMU {
	NAME		MMU Mode
	SYNOPSIS	Select MMU configuration
	COUNT		1-1
	CHILDREN	INCLUDE_MMU_FULL INCLUDE_MMU_BASIC INCLUDE_MMU_MPU
}

Component INCLUDE_MMU_MPU {
        NAME            MPU support
        SYNOPSIS        MPU component
        CONFIGLETTES    usrMmuInit.c
        MODULES         vmMpuLib.o
        INIT_RTN        usrMmuInit ();
        HDR_FILES       vmLib.h sysLib.h
	CFG_PARAMS	VM_PAGE_SIZE		\
			USER_D_MMU_ENABLE	\
			USER_I_MMU_ENABLE
}

Component INCLUDE_MMU_BASIC {
	NAME		basic MMU support
	SYNOPSIS	basic MMU component
	CONFIGLETTES	usrMmuInit.c
	MODULES		vmBaseLib.o
	INIT_RTN	usrMmuInit ();
	HDR_FILES	rebootLib.h private/vmLibP.h fioLib.h sysLib.h
	CFG_PARAMS	VM_PAGE_SIZE		\
			USER_D_MMU_ENABLE	\
			USER_I_MMU_ENABLE
}

Component INCLUDE_MMU_FULL {
	NAME		full MMU support
	CONFIGLETTES	usrMmuInit.c
	MODULES		vmLib.o
	INIT_RTN	usrMmuInit ();
	HDR_FILES	vmLib.h sysLib.h
	CFG_PARAMS	VM_PAGE_SIZE		\
			USER_D_MMU_ENABLE	\
			USER_I_MMU_ENABLE
}

Component INCLUDE_PCI_PARAMS {
	NAME		PCI configuration
	SYNOPSIS	System assumes BSP supports PCI
	CFG_PARAMS	PCI_MSTR_MEM_SIZE	\
			PCI_MSTR_MEM_BUS	\
			PCI_MSTR_MEM_LOCAL	\
			PCI_MSTR_IO_SIZE	\
			PCI_MSTR_IO_BUS		\
			PCI_MSTR_IO_LOCAL	\
			PCI_MSTR_MEMIO_SIZE	\
			PCI_MSTR_MEMIO_BUS	\
			PCI_MSTR_MEMIO_LOCAL	\
			PCI_SLV_MEM_SIZE	\
			PCI_SLV_MEM_BUS		\
			PCI_SLV_MEM_LOCAL	\
			PCI_SLV_IO_SIZE		\
			PCI_SLV_IO_BUS		\
			PCI_SLV_IO_LOCAL	\
			PCI_SLV_MEMIO_SIZE	\
			PCI_SLV_MEMIO_BUS	\
			PCI_SLV_MEMIO_LOCAL
}


Component INCLUDE_VME_PARAMS {
	NAME		VME configuration
	SYNOPSIS	Systems assumes BSP supports VME
	CFG_PARAMS	VME_A16_MSTR_SIZE	\
			VME_A16_MSTR_BUS	\
			VME_A16_MSTR_LOCAL	\
			VME_A24_MSTR_SIZE	\
			VME_A24_MSTR_BUS	\
			VME_A24_MSTR_LOCAL	\
			VME_A32_MSTR_SIZE	\
			VME_A32_MSTR_BUS	\
			VME_A32_MSTR_LOCAL	\
			VME_A16_SLV_SIZE	\
			VME_A16_SLV_BUS		\
			VME_A16_SLV_LOCAL	\
			VME_A24_SLV_SIZE	\
			VME_A24_SLV_BUS		\
			VME_A24_SLV_LOCAL	\
			VME_A32_SLV_SIZE	\
			VME_A32_SLV_BUS		\
			VME_A32_SLV_LOCAL
}

Component INCLUDE_PCMCIA {
	NAME		PCMCIA
	SYNOPSIS	PCMCIA components
	CONFIGLETTES	usrPcmcia.c
	INIT_RTN	pcmciaInit ();
	HDR_FILES	drv/pcmcia/pcmciaLib.h
}

Component INCLUDE_SCSI {
	NAME		scsi
	SYNOPSIS	SCSI components
	CONFIGLETTES	usrScsi.c
	MODULES		scsiLib.o
	INIT_RTN	usrScsiConfig ();
	HDR_FILES	sysLib.h stdio.h scsiLib.h

}


Selection SELECT_SCSI_VERSION {
	NAME		SCSI version
	COUNT		1-1
	CHILDREN	INCLUDE_SCSI1	\
			INCLUDE_SCSI2
	DEFAULTS	INCLUDE_SCSI2
}


Component INCLUDE_SCSI1 {
	NAME		SCSI 1
	SYNOPSIS	SCSI 1 components	
	MODULES		scsi1Lib.o
}

Component INCLUDE_SCSI2 {
	NAME		SCSI 2
	SYNOPSIS	SCSI 2 components
	MODULES		scsi2Lib.o
	MACRO_NEST	INCLUDE_SCSI
}

Component INCLUDE_LPT {
	NAME		parallel port
	SYNOPSIS	Parallel port components
	MODULES		lptDrv.o
	INIT_RTN	lptDrv (LPT_CHANNELS, &lptResources[0]);
	HDR_FILES	drv/parallel/lptDrv.h
	CFG_PARAMS	LPT_CHANNELS
}

Component INCLUDE_FD {
	NAME		floppy drive (NEC 765)
	SYNOPSIS	NEC 765 floppy drive component
	MODULES		nec765Fd.o
	CONFIGLETTES	usrFd.c
	INIT_RTN	fdDrv (FD_INT_VEC, FD_INT_LVL);
	CFG_PARAMS	FD_INT_VEC FD_INT_LVL
	HDR_FILES	drv/fdisk/nec765Fd.h
}

Component INCLUDE_IDE {
	NAME		IDE hard drive
	SYNOPSIS	IDE hard drive component
	MODULES		ideDrv.o
	INIT_RTN	ideDrv (IDE_INT_VEC, IDE_INT_LVL, IDE_CONFIG);
	CFG_PARAMS	IDE_INT_VEC IDE_INT_LVL IDE_CONFIG
	HDR_FILES	drv/hdisk/ideDrv.h
}

Component INCLUDE_ATA {
	NAME		ATA hard drive
	SYNOPSIS	ATA hard drive component
	MODULES		ataDrv.o
	CONFIGLETTES	usrAta.c
	INIT_RTN	usrAtaInit ();
	HDR_FILES	drv/hdisk/ataDrv.h drv/pcmcia/pccardLib.h \
			stdio.h string.h
	REQUIRES	INCLUDE_DOSFS
}

Component INCLUDE_SYSCLK_INIT {
	NAME		System clock
	SYNOPSIS	System clock component
	CONFIGLETTES	sysClkInit.c
	HDR_FILES	tickLib.h
	INIT_RTN	sysClkInit ();
	CFG_PARAMS	SYS_CLK_RATE		\
			SYS_CLK_RATE_MIN	\
			SYS_CLK_RATE_MAX
}

Component INCLUDE_AUX_CLK {
	NAME		AUX clock
	SYNOPSIS	AUX clock component
	CFG_PARAMS	AUX_CLK_RATE_MIN	\
			AUX_CLK_RATE_MAX
}

Component INCLUDE_TIMESTAMP {
	NAME		high resolution timestamping
	SYNOPSIS	BSP high resolution timestamp driver
	HDR_FILES	drv/timer/timerDev.h
}

Component INCLUDE_SIO {
	NAME		SIO
	SYNOPSIS	SIO component
	CONFIGLETTES	usrSerial.c
	INIT_RTN	usrSerialInit ();
	EXCLUDES	INCLUDE_TYCODRV_5_2
	CFG_PARAMS	NUM_TTY	CONSOLE_TTY CONSOLE_BAUD_RATE
	HDR_FILES	sysLib.h ttyLib.h stdio.h string.h ioLib.h
	REQUIRES	INCLUDE_TTY_DEV
}


Component INCLUDE_PC_CONSOLE {
	NAME		PC console
	SYNOPSIS	PC console component
	CONFIGLETTES	usrPcConsole.c
	INIT_RTN	usrPcConsoleInit ();
	HDR_FILES	sysLib.h drv/serial/pcConsole.h stdio.h
	CFG_PARAMS	N_VIRTUAL_CONSOLES PC_CONSOLE CONSOLE_BAUD_RATE
}

Component INCLUDE_TFFS {
	NAME		TrueFFS flash file system
	INIT_RTN	tffsDrv ();
	MODULES		tffsDrv.o tffsLib.o
	CONFIGLETTES	usrTffs.c
}

Component INCLUDE_DSP {
	NAME		DSP library
	CONFIGLETTES	usrDsp.c
	MODULES		dspLib.o
	INIT_RTN	usrDspInit ();
	HDR_FILES	dspLib.h
}

Folder FOLDER_FPP {
	NAME		floating point libraries
	CHILDREN	INCLUDE_SW_FP INCLUDE_HW_FP
}

Component INCLUDE_SW_FP {
	NAME		software fpp support
	MODULES		mathSoftLib.o
	INIT_RTN	mathSoftInit ();
	HDR_FILES	math.h
}

Component INCLUDE_HW_FP {
	NAME		hardware fpp support
	MODULES		mathHardLib.o
	INIT_RTN	mathHardInit ();
	HDR_FILES	math.h
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

Parameter LOCAL_MEM_LOCAL_ADRS {
	NAME		local memory address
	SYNOPSIS	The start of the on-board memory area
	DEFAULT		0x0
}

Parameter LOCAL_MEM_SIZE {
	NAME		local memory size
	SYNOPSIS	Fixed (static) memory size
	DEFAULT		0x00400000
}

Parameter LOCAL_MEM_AUTOSIZE {
	NAME		local memory Autosize
	SYNOPSIS	Run-time (dynamic) sizing
	TYPE		exists
	DEFAULT		FALSE
}

Parameter USER_RESERVED_MEM {
	NAME		user reserved Memory
	SYNOPSIS	Reserved memory size ( bytes)
	DEFAULT		0x0
}

Parameter NV_RAM_SIZE {
	NAME		NVRAM size
	SYNOPSIS	NVRAM size (bytes)
	DEFAULT		0x100
}

Parameter NV_BOOT_OFFSET {
	NAME		NVRAM boot offset
	SYNOPSIS	NVRAM boot offset
	DEFAULT		0x0
}

Parameter VEC_BASE_ADRS {
	NAME		Vector base address
	SYNOPSIS	Vector base address
	DEFAULT		(LOCAL_MEM_LOCAL_ADRS + 0x0)
}


Parameter EXC_MSG_OFFSET {
	NAME		exception message offset
	SYNOPSIS	Exception message offset
	DEFAULT		0x800
}


Parameter EXC_MSG_ADRS {
	NAME		exception message address
	SYNOPSIS	Exception message address
	DEFAULT		(LOCAL_MEM_LOCAL_ADRS + EXC_MSG_OFFSET)
}


Parameter BOOT_LINE_SIZE {
	NAME		boot line size (bytes)
	SYNOPSIS	Boot line size (bytes)
	DEFAULT		0x100
}


Parameter BOOT_LINE_ADRS {
	NAME		Boot line address
	SYNOPSIS	Boot line address
	DEFAULT		(LOCAL_MEM_LOCAL_ADRS + BOOT_LINE_OFFSET)
}

Parameter BOOT_LINE_OFFSET {
	NAME		boot line offset
	SYNOPSIS	Boot line offset
	DEFAULT		0x700
}

Parameter DEFAULT_BOOT_LINE {
	NAME		default boot line
	SYNOPSIS	Default boot line string
	TYPE		string
	DEFAULT		"ei(0,0)host:/usr/vw/config/bsp/vxWorks \
                         h=90.0.0.3 e=90.0.0.50 u=target"
}


Parameter RESERVED {
	NAME		reserved
	SYNOPSIS	Reserved
	DEFAULT		0x400
}

Parameter FREE_RAM_ADRS {
	NAME		free RAM address
	SYNOPSIS	Free RAM starting address
	DEFAULT		(end)
}

Parameter ROM_WARM_ADRS {
	NAME		ROM warm boot address
	SYNOPSIS	ROM warm boot address
}

Parameter STACK_SAVE {
	NAME		stack size
	SYNOPSIS	Amount of stack to reserve (bytes) for rom code
	DEFAULT		0x40
}

Parameter USER_D_CACHE_MODE {
	NAME		instruction cache mode
	DEFAULT		CACHE_WRITETHROUGH
}

Parameter USER_I_CACHE_MODE {
	NAME		data cache mode
	DEFAULT		CACHE_WRITETHROUGH
}

Parameter USER_I_CACHE_ENABLE {
	NAME		enable instruction cache
	TYPE		exists
	DEFAULT		TRUE
}

Parameter USER_D_CACHE_ENABLE {
	NAME		enable data cache
	TYPE		exists
	DEFAULT		TRUE
}

Parameter PCI_MSTR_MEM_SIZE {
	NAME		PCI Master memory size
	SYNOPSIS	PCI Master window memory size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_MSTR_MEM_BUS {
	NAME		PCI Master memory bus address
	SYNOPSIS	PCI Master window memory bus address
	DEFAULT		(0x0)
}

Parameter PCI_MSTR_MEM_LOCAL {
	NAME		PCI Master memory local address
	SYNOPSIS	PCI Master window memory local address
	DEFAULT		(0x0)
}



Parameter PCI_MSTR_IO_SIZE {
	NAME		PCI Master IO size
	SYNOPSIS	PCI Master window IO size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_MSTR_IO_BUS {
	NAME		PCI Master IO bus address
	SYNOPSIS	PCI Master window IO bus address
	DEFAULT		(0x0)
}

Parameter PCI_MSTR_IO_LOCAL {
	NAME		PCI Master IO local address
	SYNOPSIS	PCI Master window IO local address
	DEFAULT		(0x0)
}


Parameter PCI_MSTR_MEMIO_SIZE {
	NAME		PCI Master non-prefetch memory size
	SYNOPSIS	PCI Master window non-prefetch memory size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_MSTR_MEMIO_BUS {
	NAME		PCI Master non-prefetch memory bus address
	SYNOPSIS	PCI Master window non-prefetch memory bus address
	DEFAULT		(0x0)
}

Parameter PCI_MSTR_MEMIO_LOCAL {
	NAME		PCI Master non-prefetch memory local address
	SYNOPSIS	PCI Master window non-prefetch memory local address
	DEFAULT		(0x0)
}


Parameter PCI_SLV_MEM_SIZE {
	NAME		PCI Slave memory size
	SYNOPSIS	PCI Slave window memory size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_SLV_MEM_BUS {
	NAME		PCI Slave memory bus address
	SYNOPSIS	PCI Slave window memory bus address
	DEFAULT		(0x0)
}


Parameter PCI_SLV_MEM_LOCAL {
	NAME		PCI Slave memory local address
	SYNOPSIS	PCI Slave memory window local address
	DEFAULT		(0x0)
}



Parameter PCI_SLV_IO_SIZE {
	NAME		PCI Slave IO size
	SYNOPSIS	PCI Slave window IO size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_SLV_IO_BUS {
	NAME		PCI Slave IO bus address
	SYNOPSIS	PCI Slave window IO bus address
	DEFAULT		(0x0)
}

Parameter PCI_SLV_IO_LOCAL {
	NAME		PCI Slave IO local address
	SYNOPSIS	PCI Slave window IO local address
	DEFAULT		(0x0)
}


Parameter PCI_SLV_MEMIO_SIZE {
	NAME		PCI Slave non-prefetch memory size
	SYNOPSIS	PCI Slave non-prefetch memory size (bytes)
	DEFAULT		(0x0)
}


Parameter PCI_SLV_MEMIO_BUS {
	NAME		PCI Slave non-prefetch memory bus address
	SYNOPSIS	PCI Slave window non-prefetch memory bus address
	DEFAULT		(0x0)
}

Parameter PCI_SLV_MEMIO_LOCAL {
	NAME		PCI Slave non-prefetch memory local address
	SYNOPSIS	PCI Slave window non-prefetch memory local address
	DEFAULT		(0x0)
}

Parameter VME_A16_MSTR_SIZE {
	NAME		VME A16 Master size
	SYNOPSIS	VME A16 Master window size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A16_MSTR_BUS {
	NAME		VME A16 Master bus address
	SYNOPSIS	VME A16 Master window bus address
	DEFAULT		(0x0)
}

Parameter VME_A16_MSTR_LOCAL {
	NAME		VME A16 Master local address
	SYNOPSIS	VME A16 Master window local address
	DEFAULT		(0x0)
}


Parameter VME_A24_MSTR_SIZE {
	NAME		VME A24 Master size
	SYNOPSIS	VME A24 Master window size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A24_MSTR_BUS {
	NAME		VME A24 Master bus address
	SYNOPSIS	VME A24 Master window bus address
	DEFAULT		(0x0)
}

Parameter VME_A24_MSTR_LOCAL {
	NAME		VME A24 Master local address
	SYNOPSIS	VME A24 Master window local address
	DEFAULT		(0x0)
}

Parameter VME_A32_MSTR_SIZE {
	NAME		VME A32 Master size
	SYNOPSIS	VME A32 Master window size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A32_MSTR_BUS {
	NAME		VME A32 Master bus address
	SYNOPSIS	VME A32 Master window bus address
	DEFAULT		(0x0)
}

Parameter VME_A32_MSTR_LOCAL {
	NAME		VME A32 Master local address
	SYNOPSIS	VME A32 Master window local address
	DEFAULT		(0x0)
}


Parameter VME_A16_SLV_SIZE {
	NAME		VME A16 Slave size
	SYNOPSIS	VME A16 Slave window size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A16_SLV_BUS {
	NAME		VME A16 Slave bus address
	SYNOPSIS	VME A16 Slave window bus address
	DEFAULT		(0x0)
}


Parameter VME_A16_SLV_LOCAL {
	NAME		VME A16 Slave local address
	SYNOPSIS	VME A16 Slave windo local address
	DEFAULT		(0x0)
}


Parameter VME_A24_SLV_SIZE {
	NAME		VME A24 Slave size
	SYNOPSIS	VME A24 Slave window size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A24_SLV_BUS {
	NAME		VME A24 Slave bus address
	SYNOPSIS	VME A24 Slave window bus address
	DEFAULT		(0x0)
}

Parameter VME_A24_SLV_LOCAL {
	NAME		VME A24 Slave local address
	SYNOPSIS	VME A246 Slave window local address
	DEFAULT		(0x0)
}

Parameter VME_A32_SLV_SIZE {
	NAME		VME A32 Slave size
	SYNOPSIS	VME A32 Slave size (bytes)
	DEFAULT		(0x0)
}

Parameter VME_A32_SLV_BUS {
	NAME		VME A32 Slave bus address
	SYNOPSIS	VME A32 Slave window bus address
	DEFAULT		(0x0)
}

Parameter VME_A32_SLV_LOCAL {
	NAME		VME A32 Slave local address
	SYNOPSIS	VME A32 Slave window local address
	DEFAULT		(0x0)
}

Parameter LPT_CHANNELS {
	NAME		number of channels	
	SYNOPSIS	Number of parallel port channels
	TYPE		uint
}

Parameter FD_INT_VEC {
	NAME		interrupt vector
	SYNOPSIS	floppy drive interrupt vector
	TYPE		uint
}

Parameter FD_INT_LVL {
	NAME		interrupt level
	SYNOPSIS	Floppy drive interrupt level
	TYPE		uint
}

Parameter IDE_INT_VEC {
	NAME		interrupt vector
	SYNOPSIS	IDE interrupt vector
	TYPE		uint
}

Parameter IDE_INT_LVL {
	NAME		interrupt level
	SYNOPSIS	IDE interrupt leve
	TYPE		uint
}

Parameter IDE_CONFIG {
	NAME		manual configuration
	SYNOPSIS	Manual configuration
	TYPE		bool
}

Parameter SYS_CLK_RATE_MIN {
	NAME		SYS clock rate min
	SYNOPSIS	Minimum system clock rate
	DEFAULT		(0x1)
}


Parameter SYS_CLK_RATE_MAX {
	NAME		SYS clock rate max
	SYNOPSIS	Maximum system clock rate
	DEFAULT		(1000)
}

Parameter SYS_CLK_RATE {
	NAME		# ticks per second
	TYPE		uint
	DEFAULT		60
}

Parameter AUX_CLK_RATE_MIN {
	NAME		AUX clock rate min
	SYNOPSIS	Minimum AUX clock rate
	DEFAULT		(0x1)
}


Parameter AUX_CLK_RATE_MAX {
	NAME		AUX clock rate max
	SYNOPSIS	Maximum AUX clock rate
	DEFAULT		(1000)
}


Parameter NUM_TTY {
	NAME		number of serial ports
	TYPE		uint
	DEFAULT		2
}

Parameter CONSOLE_TTY {
	NAME		console serial port
	TYPE		uint
	DEFAULT		0
}

Parameter N_VIRTUAL_CONSOLES {
	DEFAULT		1
	TYPE		uint
}

Parameter PC_CONSOLE {
	DEFAULT		0
	TYPE		uint
}


Parameter CONSOLE_BAUD_RATE {
	NAME		baud rate of console port
	TYPE		uint
	DEFAULT		9600
}


Parameter RAM_LOW_ADRS {
	NAME		RAM low Adddress
	SYNOPSIS	The address at which to load VxWorks
	DEFAULT		0x01000
}

Parameter RAM_HIGH_ADRS {
	NAME		RAM high Address
	SYNOPSIS	The destination address used for copying \
			boot ROM image to RAM
	DEFAULT		0x020000
}

Parameter ROM_TEXT_ADRS {
	NAME		ROM text address
	SYNOPSIS	ROM text address
}


Parameter ROM_BASE_ADRS {
	NAME		ROM base address
	SYNOPSIS	ROM base address
}


Parameter ROM_SIZE {
	NAME		ROM size
	SYNOPSIS	ROM size
}



Parameter VM_PAGE_SIZE {
	NAME		VM page szie
	SYNOPSIS	Virtual memory page size (bytes)
	DEFAULT		8192
}


/* BSD network drivers and parameters */

Folder FOLDER_BSD_NET_DRV {
	NAME		BSD Ethernet drivers
	SYNOPSIS	Ethernet drivers using the BSD4.4 Interface
	CHILDREN	INCLUDE_LOOPBACK \
			INCLUDE_DC \
			INCLUDE_EGL \
			INCLUDE_EI \
			INCLUDE_FEI \
			INCLUDE_EX \
			INCLUDE_ENP \
			INCLUDE_IE \
			INCLUDE_ILAC \
			INCLUDE_LN \
			INCLUDE_LNSGI \
			INCLUDE_NIC \
			INCLUDE_NIC_EVB \
			INCLUDE_MED \
			INCLUDE_ELC \
			INCLUDE_ULTRA \
			INCLUDE_EEX \
			INCLUDE_ELT \
			INCLUDE_QU \
			INCLUDE_ENE \
			INCLUDE_ESMC \
			INCLUDE_SN \
			INCLUDE_OLI		\
			INCLUDE_USR_ENTRIES	\
			INCLUDE_IF_USR		\
			INCLUDE_LNEBSA		\
			INCLUDE_FN
	DEFAULTS	INCLUDE_LOOPBACK
}

Component INCLUDE_USR_ENTRIES
	{
	NAME		user-defined BSD4.4 driver entries
	SYNOPSIS	associates driver names with attach routines
	CFG_PARAMS	NETIF_USR_ENTRIES NETIF_USR_DECL
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_IF_USR {
	NAME		user-defined BSD4.4 driver entry
	SYNOPSIS	driver routine from the BSP
	CFG_PARAMS	IF_USR_NAME IF_USR_ATTACH IF_USR_ARG1 IF_USR_ARG2 \
			IF_USR_ARG3 IF_USR_ARG4 IF_USR_ARG5 IF_USR_ARG6 \
			IF_USR_ARG7 IF_USR_ARG8
	REQUIRES	INCLUDE_BSD
}

Component INCLUDE_LNEBSA
	{
	NAME		LN netif driver for EBSA
	SYNOPSIS	BSD 4.4 LN ethernet driver for EBSA
	MODULES		if_lnebsa.o
	CFG_PARAMS	IO_ADRS_LNEBSA INT_VEC_LNEBSA			\
			INT_LVL_LNEBSA LNEBSA_POOL_ADRS LNEBSA_POOL_SIZE \
			LNEBSA_DATA_WIDTH LNEBSA_MODE LNEBSA_DMA_CHAN
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_OLI
	{
	NAME		OLI netif driver
	SYNOPSIS	BSD 4.4 OLI ethernet driver
	MODULES		if_oli.o
	CFG_PARAMS	IO_ADRS_NISA_BASE IO_ADRS_NISA_PCMCIA \
			IO_ADRS_NISA_PCMEM INT_VEC_PCMCIA_A \
			INT_LVL_PCMCIA_A INT_VEC_PCMCIA_B \
			INT_LVL_PCMCIA_B
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_DC
	{
	NAME		DC netif driver
	SYNOPSIS	BSD 4.4 DEC ethernet driver
	MODULES		if_dc.o
	CFG_PARAMS	IO_ADRS_DC INT_VEC_DC INT_LVL_DC \
			DC_POOL_ADRS DC_POOL_SIZE DC_DATA_WIDTH \
			DC_RAM_PCI_ADRS DC_MODE
	REQUIRES	INCLUDE_BSD
	}	
	
Component INCLUDE_EGL
	{
	NAME		EGL netif driver
	SYNOPSIS	BSD 4.4 EGL ethernet driver
	MODULES		if_egl.o
	CFG_PARAMS	IO_ADRS_EGL INT_VEC_EGL INT_LVL_EGL
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_EI
	{ 
	NAME		EI netif driver
	SYNOPSIS	BSD 4.4 Intel i82596 ethernet driver
	MODULES		if_ei.o
	CFG_PARAMS	INT_VEC_EI EI_SYSBUS EI_POOL_ADRS
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_FEI
	{ 
	NAME		FEI netif driver
	SYNOPSIS	BSD 4.4 Intel i82557,558,559 driver
	MODULES		if_fei.o
	CFG_PARAMS	FEI_POOL_ADRS
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_EX
	{ 
	NAME		EX netif driver
	SYNOPSIS	BSD 4.4 EX ethernet driver
	MODULES		if_ex.o
	CFG_PARAMS	IO_ADRS_EX INT_VEC_EX INT_LVL_EX \
				  IO_AM_EX_MASTER IO_AM_EX
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_ENP
	{ 
	NAME		ENP netif driver
	SYNOPSIS	BSD 4.4 ENP ethernet driver
	MODULES		if_enp.o
	CFG_PARAMS	IO_ADRS_ENP INT_VEC_ENP INT_LVL_ENP IO_AM_ENP
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_IE
	{
	NAME		IE netif driver
	SYNOPSIS	BSD 4.4 IE ethernet driver
	MODULES		if_ie.o
	CFG_PARAMS	IO_ADRS_IE INT_VEC_IE INT_LVL_IE
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_ILAC
	{
	NAME		ILAC netif driver
	NAME		ILAC Ethernet
	SYNOPSIS	BSD 4.4 ILAC ethernet driver
	MODULES		if_ilac.o
	CFG_PARAMS	IO_ADRS_ILAC INT_VEC_ILAC
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_LN
	{
	NAME		LN netif driver
	SYNOPSIS	BSD 4.4 AMD Lance ethernet driver
	MODULES		if_ln.o
	CFG_PARAMS	IO_ADRS_LN INT_VEC_LN INT_LVL_LN \
	  LN_POOL_ADRS LN_POOL_SIZE LN_DATA_WIDTH LN_PADDING \
	  LN_RING_BUF_SIZE
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_LNSGI
	{
	NAME		LN netif driver for SGI
	SYNOPSIS	BSD 4.4 SGI specific Lance ethernet driver
	MODULES		if_lnsgi.o
	CFG_PARAMS	IO_ADRS_LNSGI INT_VEC_LNSGI \
	  INT_LVL_LNSGI LNSGI_POOL_ADRS LNSGI_POOL_SIZE \
	  LNSGI_DATA_WIDTH LNSGI_PADDING LNSGI_RING_BUF_SIZE
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_NIC
	{
	NAME		NIC netif driver
	SYNOPSIS	BSD 4.4 NIC ethernet driver
	MODULES		if_nic.o
	CFG_PARAMS	IO_ADRS_NIC INT_VEC_NIC INT_LVL_NIC
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_NIC_EVB
	{
	NAME		NIC EVB netif driver
	SYNOPSIS	BSD 4.4 NIC EVB ethernet driver
	MODULES		if_nicEvb.o
	CFG_PARAMS	IO_ADRS_NIC INT_VEC_NIC INT_LVL_NIC
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_MED
	{
	NAME		MED netif driver
	SYNOPSIS	BSD 4.4 MED ethernet driver
	MODULES		if_med.o
	CFG_PARAMS	IO_ADRS_DBETH INT_VEC_DBETH INT_LVL_DBETH
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_ELC
	{
	NAME		ELC netif driver
	SYNOPSIS	BSD 4.4 ELC ethernet driver
	MODULES		if_elc.o
	CFG_PARAMS	IO_ADRS_ELC INT_VEC_ELC INT_LVL_ELC \
			MEM_ADRS_ELC MEM_SIZE_ELC CONFIG_ELC
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_ULTRA
	{
	NAME		ULTRA netif driver
	SYNOPSIS	BSD 4.4 ULTRA ethernet driver
	MODULES		if_ultra.o
	CFG_PARAMS	IO_ADRS_ULTRA INT_VEC_ULTRA \
				INT_LVL_ULTRA MEM_ADRS_ULTRA \
				MEM_SIZE_ULTRA CONFIG_ULTRA
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_EEX
	{
	NAME		EEX netif driver
	SYNOPSIS	BSD 4.4 EEX ethernet driver
	MODULES		if_eex.o
	CFG_PARAMS	IO_ADRS_EEX INT_VEC_EEX INT_LVL_EEX NTFDS_EEX \
			 CONFIG_EEX
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_ELT
	{
	NAME		ELT netif driver
	SYNOPSIS	BSD 4.4 ELT ethernet driver
	MODULES		if_elt.o
	CFG_PARAMS	IO_ADRS_ELT INT_VEC_ELT INT_LVL_ELT NRF_ELT \
			CONFIG_ELT
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_QU
	{
	NAME		QUICC netif driver
	SYNOPSIS	BSD 4.4 QUICC ethernet driver
	MODULES		if_qu.o
	CFG_PARAMS	IO_ADRS_QU_EN INT_VEC_QU_EN \
				QU_EN_SCC QU_EN_TX_BD QU_EN_RX_BD \
				QU_EN_TX_OFF QU_EN_RX_OFF \
				QU_EN_MEM
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_ENE
	{
	NAME		ENE netif driver
	SYNOPSIS	BSD 4.4 ENE ethernet driver
	MODULES		if_ene.o
	CFG_PARAMS	IO_ADRS_ENE INT_VEC_ENE INT_LVL_ENE
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_ESMC
	{
	NAME		ESMC netif driver
	SYNOPSIS	BSD 4.4 ESMC ethernet driver
	MODULES		if_esmc.o
	CFG_PARAMS	IO_ADRS_ESMC INT_VEC_ESMC \
			INT_LVL_ESMC CONFIG_ESMC RX_MODE_ESMC
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_SN
	{
	NAME		SN netif driver
	SYNOPSIS	BSD 4.4 Sonic ethernet driver
	MODULES		if_sn.o
	CFG_PARAMS	IO_ADRS_SN INT_VEC_SN
	REQUIRES	INCLUDE_BSD
	}

Component  INCLUDE_FN
	{
	NAME		FN netif driver
	SYNOPSIS	BSD 4.4 FN ethernet driver
	MODULES		if_fn.o
	REQUIRES	INCLUDE_BSD
	}

Component INCLUDE_LOOPBACK
	{
	NAME		loopback driver
	SYNOPSIS	loopback interface for routining to localhost
	CONFIGLETTES	net/usrNetLoopbackStart.c 
	INIT_RTN	usrNetLoopbackStart ();
	MODULES		if_loop.o
	REQUIRES        INCLUDE_NETDEV_CONFIG
	}

Parameter CONFIG_EEX {
}

Parameter CONFIG_ELC {
}

Parameter CONFIG_ELT {
}

Parameter CONFIG_ESMC {
}

Parameter CONFIG_ULTRA {
}

Parameter DC_DATA_WIDTH {
}

Parameter DC_MODE {
}

Parameter DC_POOL_ADRS {
}

Parameter DC_POOL_SIZE {
}

Parameter DC_RAM_PCI_ADRS {
}

Parameter EI_POOL_ADRS {
}

Parameter EI_SYSBUS {
}

Parameter INT_LVL_DBETH {
}

Parameter INT_LVL_DC {
}

Parameter INT_LVL_EEX {
}

Parameter INT_LVL_EGL {
}

Parameter INT_LVL_ELC {
}

Parameter INT_LVL_ELT {
}

Parameter INT_LVL_ENE {
}

Parameter INT_LVL_ENP {
}

Parameter INT_LVL_ESMC {
}

Parameter INT_LVL_EX {
}

Parameter INT_LVL_IE {
}

Parameter INT_LVL_LN {
}

Parameter INT_LVL_LNEBSA {
}

Parameter INT_LVL_LNSGI {
}

Parameter INT_LVL_NIC {
}

Parameter INT_LVL_PCMCIA_A {
}

Parameter INT_LVL_PCMCIA_B {
}

Parameter INT_LVL_ULTRA {
}

Parameter INT_VEC_DBETH {
}

Parameter INT_VEC_DC {
}

Parameter INT_VEC_EEX {
}

Parameter INT_VEC_EGL {
}

Parameter INT_VEC_EI {
}

Parameter INT_VEC_ELC {
}

Parameter INT_VEC_ELT {
}

Parameter INT_VEC_ENE {
}

Parameter INT_VEC_ENP {
}

Parameter INT_VEC_ESMC {
}

Parameter INT_VEC_EX {
}

Parameter INT_VEC_IE {
}

Parameter INT_VEC_ILAC {
}

Parameter INT_VEC_LN {
}

Parameter INT_VEC_LNEBSA {
}

Parameter INT_VEC_LNSGI {
}

Parameter INT_VEC_NIC {
}

Parameter INT_VEC_PCMCIA_A {
}

Parameter INT_VEC_PCMCIA_B {
}

Parameter INT_VEC_QU_EN {
}

Parameter INT_VEC_SN {
}

Parameter INT_VEC_ULTRA {
}

Parameter IO_ADRS_DBETH {
}

Parameter IO_ADRS_DC {
}

Parameter IO_ADRS_EEX {
}

Parameter IO_ADRS_EGL {
}

Parameter IO_ADRS_ELC {
}

Parameter IO_ADRS_ELT {
}

Parameter IO_ADRS_ENE {
}

Parameter IO_ADRS_ENP {
}

Parameter IO_ADRS_ESMC {
}

Parameter IO_ADRS_EX {
}

Parameter IO_ADRS_IE {
}

Parameter IO_ADRS_ILAC {
}

Parameter IO_ADRS_LN {
}

Parameter IO_ADRS_LNEBSA {
}

Parameter IO_ADRS_LNSGI {
}

Parameter IO_ADRS_NIC {
}

Parameter IO_ADRS_NISA_BASE {
}

Parameter IO_ADRS_NISA_PCMCIA {
}

Parameter IO_ADRS_NISA_PCMEM {
}

Parameter IO_ADRS_QU_EN {
}

Parameter IO_ADRS_SN {
}

Parameter IO_ADRS_ULTRA {
}

Parameter IO_AM_ENP {
}

Parameter IO_AM_EX {
}

Parameter IO_AM_EX_MASTER {
}

Parameter LNEBSA_DATA_WIDTH {
}

Parameter LNEBSA_DMA_CHAN {
}

Parameter LNEBSA_MODE {
}

Parameter LNEBSA_POOL_ADRS {
}

Parameter LNEBSA_POOL_SIZE {
}

Parameter LNSGI_DATA_WIDTH {
}

Parameter LNSGI_PADDING {
}

Parameter LNSGI_POOL_ADRS {
}

Parameter LNSGI_POOL_SIZE {
}

Parameter LNSGI_RING_BUF_SIZE {
}

Parameter LN_DATA_WIDTH {
}

Parameter LN_PADDING {
}

Parameter LN_POOL_ADRS {
}

Parameter LN_POOL_SIZE {
}

Parameter LN_RING_BUF_SIZE {
}

Parameter MEM_ADRS_ELC {
}

Parameter MEM_ADRS_ULTRA {
}

Parameter MEM_SIZE_ELC {
}

Parameter MEM_SIZE_ULTRA {
}

Parameter NRF_ELT {
}

Parameter NTFDS_EEX {
}

Parameter QU_EN_MEM {
}

Parameter QU_EN_RX_BD {
}

Parameter QU_EN_RX_OFF {
}

Parameter QU_EN_SCC {
}

Parameter QU_EN_TX_BD {
}

Parameter QU_EN_TX_OFF {
}

Parameter RX_MODE_ESMC {
}

Parameter IF_USR_NAME {
}

Parameter IF_USR_ATTACH {
}

Parameter IF_USR_ARG1 {
}

Parameter IF_USR_ARG2 {
}

Parameter IF_USR_ARG3 {
}

Parameter IF_USR_ARG4 {
}

Parameter IF_USR_ARG5 {
}

Parameter IF_USR_ARG6 {
}

Parameter IF_USR_ARG7 {
}

Parameter IF_USR_ARG8 {
}

Parameter FEI_POOL_ADRS {
}

Parameter NETIF_USR_ENTRIES {
}

Parameter NETIF_USR_DECL {
}
