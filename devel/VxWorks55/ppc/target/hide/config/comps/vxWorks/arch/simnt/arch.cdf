/*
XXX cym KERNEL is overridden here to reflect the fact that simLib.o calls the
wdbDbg functions directly, instead of through function pointers.  This will
be fixed and removed after Beta.
*/

Component INCLUDE_KERNEL {
        NAME            kernel
        SYNOPSIS        context switch and interrupt handling. DO NOT REMOVE.
        MODULES         kernelLib.o taskLib.o intLib.o
        CONFIGLETTES    usrKernel.c
        INIT_RTN        usrKernelInit ();
        CFG_PARAMS      INCLUDE_CONSTANT_RDY_Q  \
                        ROOT_STACK_SIZE         \
                        ISR_STACK_SIZE          \
                        INT_LOCK_LEVEL
        REQUIRES        INCLUDE_MEMORY_CONFIG INCLUDE_WDB_BP
        HDR_FILES       sysLib.h private/kernelLibP.h private/workQLibP.h \
                        qPriBMapLib.h taskLib.h usrConfig.h
}

Component INCLUDE_LOADER {
        SYNOPSIS        PECOFF loader
        MODULES         loadLib.o loadPecoffLib.o
        INIT_RTN        loadPecoffInit ();
        HDR_FILES       loadPecoffLib.h
}

Parameter INT_LOCK_LEVEL {
	NAME		INT lock level
	SYNOPSIS	Interrupt lock level
	DEFAULT		0x01
}

Parameter ROOT_STACK_SIZE {
	NAME		Root stack size
	SYNOPSIS	Root task stack size (bytes)
	DEFAULT		20000
}

Parameter SHELL_STACK_SIZE {
	NAME		Shell stack size
	SYNOPSIS	Shell stack size (bytes)
	DEFAULT		50000
}

Parameter WDB_STACK_SIZE {
	NAME		WDB Stack size
	SYNOPSIS	WDB Stack size (bytes)
	DEFAULT		0x2000
}

Parameter ISR_STACK_SIZE {
	NAME		ISR stack size
	SYNOPSIS	ISR Stack size (bytes)
	DEFAULT		50000
}

Parameter VEC_BASE_ADRS {
	NAME		Vector base address
	SYNOPSIS	Vector base address
	DEFAULT		0
}

Parameter ROM_WARM_ADRS {
	NAME		ROM warm boot address
	SYNOPSIS	ROM warm boot address
	DEFAULT		(0x0)
}

