/*
 * Copyright (c) 1983 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)vars.c	5.3 (Berkeley) 9/2/88";
#endif /* not lint */

#include "tip.h"

/*
 * Definition of variables
 */
value_t vtable[] = {
	{ "beautify",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "be",		(char *)TRUE },
	{ "baudrate",	NUMBER|IREMOTE|INIT,	(READ<<PUBLIC)|(WRITE<<ROOT),
	  "ba",		(char *)&BR },
	{ "dialtimeout",NUMBER,			(READ<<PUBLIC)|(WRITE<<ROOT),
	  "dial",	(char *)60 },
	{ "eofread",	STRING|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "eofr",	(char *)&IE },
	{ "eofwrite",	STRING|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "eofw",	(char *)&OE },
	{ "eol",	STRING|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  NOSTR,	(char *)&EL },
	{ "escape",	CHAR,			(READ|WRITE)<<PUBLIC,
	  "es",		(char *)'~' },
	{ "exceptions",	STRING|INIT|IREMOTE,	(READ|WRITE)<<PUBLIC,
	  "ex",		(char *)&EX },
	{ "force",	CHAR,			(READ|WRITE)<<PUBLIC,
	  "fo",		(char *)CTRL('p') },
	{ "framesize",	NUMBER|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "fr",		(char *)&FS },
	{ "host",	STRING|IREMOTE|INIT,	READ<<PUBLIC,
	  "ho",		(char *)&HO },
	{ "log",	STRING|INIT,		(READ|WRITE)<<ROOT,
	  NOSTR,	"/usr/adm/aculog" },
	{ "phones",	STRING|INIT|IREMOTE,	READ<<PUBLIC,
	  NOSTR,	(char *)&PH },
	{ "prompt",	CHAR,			(READ|WRITE)<<PUBLIC,
	  "pr",		(char *)'\n' },
	{ "raise",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "ra",		(char *)FALSE },
	{ "raisechar",	CHAR,			(READ|WRITE)<<PUBLIC,
	  "rc",		(char *)CTRL('a') },
	{ "record",	STRING|INIT|IREMOTE,	(READ|WRITE)<<PUBLIC,
	  "rec",	(char *)&RE },
	{ "remote",	STRING|INIT|IREMOTE,	READ<<PUBLIC,
	  NOSTR,	(char *)&RM },
	{ "script",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "sc",		(char *)FALSE },
	{ "tabexpand",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "tab",	(char *)FALSE },
	{ "verbose",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "verb",	(char *)FALSE },
	{ "SHELL",	STRING|ENVIRON|INIT,	(READ|WRITE)<<PUBLIC,
	  NULL,		"/bin/sh" },
	{ "HOME",	STRING|ENVIRON,		(READ|WRITE)<<PUBLIC,
	  NOSTR,	NOSTR },
	{ "echocheck",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "ec",		(char *)FALSE },
	{ "disconnect",	STRING|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "di",		(char *)&DI },
	{ "tandem",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "ta",		(char *)TRUE },
	{ "linedelay",	NUMBER|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "ldelay",	(char *)&DL },
	{ "chardelay",	NUMBER|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "cdelay",	(char *)&CL },
	{ "etimeout",	NUMBER|IREMOTE|INIT,	(READ|WRITE)<<PUBLIC,
	  "et",		(char *)&ET },
	{ "rawftp",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "raw",	(char *)FALSE },
	{ "halfduplex",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "hdx",	(char *)FALSE },
	{ "localecho",	BOOL,			(READ|WRITE)<<PUBLIC,
	  "le",		(char *)FALSE },
	{ "parity",	STRING|INIT|IREMOTE,	(READ|WRITE)<<PUBLIC,
	  "par",	(char *)&PA },
	{ "connectcmd",	STRING|IREMOTE,		READ<<PUBLIC,
	  "cc",		(char *)&CC },
	{ "slipaddr",	STRING|IREMOTE,		READ<<PUBLIC,
	  "sa",		(char *)&SA },
	{ "destaddr",	STRING|IREMOTE,		READ<<PUBLIC,
	  "da",		(char *)&DA },
	{ "slipmask",	STRING|IREMOTE,		READ<<PUBLIC,
	  "sm",		(char *)&SM },
	{ "sliptype",	STRING|IREMOTE,		READ<<PUBLIC,
	  "st",		(char *)&ST },
	{ "loginscript",STRING|IREMOTE,		(READ|WRITE)<<PUBLIC,
	  "ls",		(char *)&LS },
	{ "linesync",   BOOL,			(READ|WRITE)<<PUBLIC,
	  "li",		(char *)FALSE },
	{ "dtrhup",	BOOL,			READ<<PUBLIC,
	  "dt",		(char *)FALSE },
	{ "hwflowctrl",	BOOL,			READ<<PUBLIC,
	  "fc",		(char *)FALSE },
	{ NOSTR, NULL, NULL, NOSTR, NOSTR }
};