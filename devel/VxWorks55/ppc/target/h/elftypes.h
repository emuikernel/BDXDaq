/* elftypes.h - typedefs of elf types for vxWorks */

/* Copyright 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,30apr03,jn   derived from Diab elftypes.h header file 
                 /vobs/rome_diab_lib/dlib/include/include.unx/
		 elftypes.h@@/main/1
		 and /.wind_vxw_h/river/target/h/vxWorks.h@@/main/tor2/1 
		 (for elements of Wind River coding standards)
*/

#ifndef __INCelftypesh
#define __INCelftypesh

#ifdef __cplusplus
extern "C" {
#endif

/**************	Exported data, types and macros	*****************/

typedef unsigned long   Elf32_Addr;
typedef unsigned short  Elf32_Half;
typedef unsigned long   Elf32_Off;
typedef long            Elf32_Sword;
typedef unsigned long   Elf32_Word;

#ifdef __cplusplus
}
#endif

#endif /* __INCelftypesh */
