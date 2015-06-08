/* limits.h - limits header file */

/* Copyright 1991-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,07dec01,sn   use toolchain specific limits.h
01g,01may94,kdl	 added missing mod hist for 01f.
01f,06dec93,dvs	 added POSIX AIO defines.
01e,24sep92,smb  removed POSIX ifdef.
01d,22sep92,rrr  added support for c++
01c,04jul92,jcf  cleaned up.
01b,03jul92,smb  merged ANSI limits
01a,29jul91,rrr  written.
*/

#ifndef __INClimitsh
#define __INClimitsh

#ifdef __cplusplus
extern "C" {
#endif

#include "types/vxANSI.h"

/* include the toolchain's notion of limits.h */
#ifndef TOOL_FAMILY
#   define TOOL_FAMILY gnu
#endif

#define _TOOL_HDR_STRINGIFY(x)  #x
#define _TOOL_HDR(tc, file) _TOOL_HDR_STRINGIFY(tool/tc/file)
#define _TOOL_HEADER(file) _TOOL_HDR(TOOL_FAMILY,file)

#include _TOOL_HEADER(limits.h)

#define NAME_MAX        _PARM_NAME_MAX	/* max filename length excluding EOS */
#define PATH_MAX        _PARM_PATH_MAX	/* max pathname length excluding EOS */

#define NGROUPS_MAX					/* POSIX extensions */
#define SSIZE_MAX
#define DATAKEYS_MAX		_PARM_DATAKEYS_MAX
#define AIO_LISTIO_MAX		10	/* needs _PARM in types/vxParams.h */
#define AIO_PRIO_DELTA_MAX	254	/* needs _PARM in types/vxParams.h */

#define _POSIX_ARG_MAX		4096
#define _POSIX_CHILD_MAX	6
#define _POSIX_LINK_MAX		8
#define _POSIX_MAX_CANON	255
#define _POSIX_MAX_INPUT	255
#define _POSIX_NAME_MAX		14
#define _POSIX_NGROUPS_MAX	0
#define _POSIX_OPEN_MAX		16
#define _POSIX_PATH_MAX		255
#define _POSIX_PIPE_BUF		512
#define _POSIX_SSIZE_MAX	32767
#define _POSIX_STREAM_MAX	8
#define _POSIX_TZNAME_MAX	3
#define _POSIX_DATAKEYS_MAX	16

#define _POSIX_AIO_LISTIO_MAX	2
#define _POSIX_AIO_MAX		1

#ifdef __cplusplus
}
#endif

#endif /* __INClimitsh */
