/*
Copyright 1998-2002 Wind River Systems, Inc.


modification history
--------------------
01b,11mar02,mas  added default values to SM params (SPR 73371)
01a,12nov01,mas  written based on 00vxWorks.cdf: 02v,05nov01,gls


DESCRIPTION
  This file contains the description for the VxWorks shared memory component.
*/


Component INCLUDE_SM_COMMON {
	NAME		basic shared memory support and parameters
	MODULES		smLib.o \
			smUtilLib.o
	INIT_RTN	smLibInit (NULL, SM_OBJ_MEM_SIZE, SM_MEM_SIZE);
	_INIT_ORDER	usrKernelExtraInit
	INIT_BEFORE	INCLUDE_PROTECT_TEXT
	_CHILDREN	FOLDER_MEMORY
	REQUIRES	INCLUDE_MEMORY_CONFIG
	CFG_PARAMS	SM_MASTER \
			SM_CPUS_MAX \
			SM_OFF_BOARD \
			SM_ANCHOR_OFFSET \
			SM_ANCHOR_ADRS \
			SM_INT_TYPE \
			SM_INT_ARG1 \
			SM_INT_ARG2 \
			SM_INT_ARG3 \
			SM_MAX_WAIT \
			SM_MEM_ADRS \
			SM_MEM_SIZE \
			SM_TAS_TYPE \
			SM_OBJ_MEM_SIZE
	HDR_FILES	smLib.h \
			smUtilLib.h
}


Parameter SM_MEM_ADRS {
	NAME		shared memory address, NONE = allocate local memory
	DEFAULT		NONE
}

Parameter SM_MEM_SIZE {
	NAME		shared memory size
	TYPE		uint
	DEFAULT		0
}

Parameter SM_INT_TYPE {
	NAME		shared memory interrupt type
	TYPE		uint
	DEFAULT		SM_INT_NONE
}

Parameter SM_INT_ARG1 {
	NAME		shared memory interrupt type - argument 1
	TYPE		uint
	DEFAULT		0
}

Parameter SM_INT_ARG2 {
	NAME		shared memory interrupt type - argument 2
	TYPE		uint
	DEFAULT		0
}

Parameter SM_INT_ARG3 {
	NAME		shared memory interrupt type - argument 3
	TYPE		uint
	DEFAULT		0
}

Parameter SM_OFF_BOARD {
	NAME		is the shared memory off board?
	TYPE		bool
	DEFAULT		TRUE
}

Parameter SM_MASTER {
	NAME		shared memory master CPU number
	TYPE		uint
	DEFAULT		0
}

Parameter SM_OBJ_MEM_SIZE {
	NAME		shared memory object pool size
	TYPE		uint
	DEFAULT		0
}

Parameter SM_TAS_TYPE {
	NAME		shared memory test-and-set type
	DEFAULT		SM_TAS_HARD
}

Parameter SM_MAX_WAIT {
	NAME		max period in ticks to wait for master to boot
	TYPE		uint
	DEFAULT		3000
}

Parameter SM_CPUS_MAX {
	NAME		max # of cpus for shared network, 0 = DEFAULT_CPUS_MAX
	TYPE		uint
	DEFAULT		0
}

Parameter SM_ANCHOR_ADRS {
	NAME		shared memory anchor address
	DEFAULT		((char *) (LOCAL_MEM_LOCAL_ADRS+SM_ANCHOR_OFFSET))
}

Parameter SM_ANCHOR_OFFSET {
	NAME		shared memory anchor offset from start of phys memory
	TYPE		uint
	DEFAULT		0x600
}

