/* distObjTypeP.h - distributed objects type header file */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21may98,drm  written.
*/

#ifndef __INCdistObjTypePh
#define __INCdistObjTypePh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

/* types of objects within VxWorks: standard, shared memory and distributed */
#define VX_TYPE_STD_OBJ				0x0L	/* local, non shared objects */
#define VX_TYPE_SM_OBJ				0x1L	/* shared memory objects */
#define VX_TYPE_DIST_OBJ			0x3L	/* distributed objects */

#define VX_TYPE_OBJ_MASK			0x3L	/* two bits to distinguish type */

#define ID_IS_DISTRIBUTED(msgQId) \
	((((uint32_t) (msgQId)) & VX_TYPE_OBJ_MASK) == VX_TYPE_DIST_OBJ)
                                           /* determine if obj is distributed */

/* status codes */
#define S_distLib_NOT_INITIALIZED		(M_distLib | 1) /* error code */
#define S_distLib_NO_OBJECT_DESTROY		(M_distLib | 2) /* error code */
#define S_distLib_UNREACHABLE			(M_distLib | 3) /* error code */
#define S_distLib_UNKNOWN_REQUEST		(M_distLib | 4) /* error code */
#define S_distLib_OBJ_ID_ERROR			(M_distLib | 5) /* error code */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __INCdistObjTypePh */

