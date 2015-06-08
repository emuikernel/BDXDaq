Component INCLUDE_LOADER {
        SYNOPSIS        SOM loader
        MODULES         loadLib.o loadSomCoffLib.o
        INIT_RTN        loadSomCoffInit ();
        HDR_FILES       loadSomCoffLib.h
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

