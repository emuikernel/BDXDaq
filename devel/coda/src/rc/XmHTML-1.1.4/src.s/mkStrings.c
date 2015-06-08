#ifndef production
static char rcsId[]="$Header: /net/mizar/usr/local/source/coda_source/rc/XmHTML-1.1.4/XmHTML-1.1.4/src/mkStrings.c,v 1.1.1.1 1998/08/25 17:48:11 rwm Exp $";
#endif
/*****
* mkStrings.c : create a stringtable & defines from a list of strings
*
* This file Version	$Revision: 1.1.1.1 $
*
* Creation date:		Tue Sep  2 01:13:25 GMT+0100 1997
* Last modification: 	$Date: 1998/08/25 17:48:11 $
* By:					$Author: rwm $
* Current State:		$State: Exp $
*
* Author:				newt
*
* Copyright (C) 1994-1997 by Ripley Software Development 
* All Rights Reserved
*
* This file is part of the XmHTML Widget Library
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* Note: this file is a terrible mess.
*****/
/*****
* ChangeLog 
* $Log: mkStrings.c,v $
* Revision 1.1.1.1  1998/08/25 17:48:11  rwm
*   Initial XmHTML-1.1.4 with RWM fixes.
*
* Revision 1.2  1998/04/04 06:28:14  newt
* XmHTML Beta 1.1.3
*
* Revision 1.1  1997/10/23 00:23:15  newt
* Initial Revision
*
*****/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/*** External Function Prototype Declarations ***/

/*** Public Variable Declarations ***/

/*** Private Datatype Declarations ****/
#define MKSTRINGS_VERSION		"mkStrings Version 1.30"

/* Initial size of output buffer. */
#define MAX_OUTPUT_SIZE			32768

/*** Private Function Prototype Declarations ****/

/*** Private Variable Declarations ***/
/* header for a c source file */
static char *motif_source = {"\n"
"#include <Xm/XmP.h>\n"
"#if XtSpecificationRelease < 6\n"
"#include <X11/IntrinsicP.h>\n"
"#endif\n"
"\n"
"#ifndef _XmConst\n"
"# ifdef __STDC__\n"
"#  define _XmConst const\n"
"# else\n"
"#  define _XmConst\n"
"# endif\n"
"#endif\n\n"
"/*****\n"
"* The missing commas are intentional. Let the compiler\n"
"* concatenate the strings\n"
"*****/\n\n"
"_XmConst char _%s%s[] =\n"};

/* header for a c include file */
static char *motif_header = {"\n"
"_XFUNCPROTOBEGIN\n\n"
"/*****\n"
"* Don't define %s_STRINGDEFINES if you want to save space\n"
"*****/\n"
"\n#ifndef %s_STRINGDEFINES\n"
"# ifndef _XmConst\n"
"#  ifdef __STDC__\n"
"#   define _XmConst const\n"
"#  else\n"
"#   define _XmConst\n"
"#  endif\n"
"# endif\n"
"extern _XmConst char _%s%s[];\n"
"#endif\n\n"
"#ifdef %s_STRINGDEFINES\n\n"};

/* closure for all output types */
static char *closure = {"\n"
"#endif /* %s_STRINGDEFINES */\n"
"\n_XFUNCPROTOEND\n\n"
"/* Don't add anything after this endif! */\n"
"#endif /* __%s_%s_h__ */\n"};

static void
writeMotifStrings(char what, char *app_name)
{
	char name[512];
	int curr_size = 0;

	/* generate a c-source file */
	if(what == 'c')
	{
		printf(motif_source, app_name, "Strings");

		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
				continue;

			name[strlen(name)-1] = '\0';	/* strip newline */
			printf("/* %5i */ \"%s\\0\"\n", curr_size, name);
			curr_size += strlen(name)+1;
		}
		printf("/* ????? */\n\"\\0\";\n");
	}
	else	/* need a header file */
	{
		char msg[1024];
		char *outbuf;
		int bufsize = MAX_OUTPUT_SIZE;

		printf("\n#ifndef __%s_Strings_h__\n", app_name);
		printf("#define __%s_Strings_h__\n", app_name);

		outbuf = (char*)calloc(1, bufsize);

		printf(motif_header, app_name, app_name, app_name, "Strings", app_name);

		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
				continue;

			name[strlen(name)-1] = '\0';	/* strip newline */

			if(isupper(name[0]))
			{
				/* regular defines */
				printf("# ifndef XmC%s\n", name);
				printf("#  define XmC%s \"%s\"\n", name, name);
				printf("# endif\n");
				printf("# ifndef XmR%s\n", name);
				printf("#  define XmR%s \"%s\"\n", name, name);
				printf("# endif\n");

				/* collect string table references */
				sprintf(msg,
					"# ifndef XmC%s\n"
					"#  define XmC%s ((char *)&_%sStrings[%i])\n"
					"# endif\n"
					"# ifndef XmR%s\n"
					"#  define XmR%s ((char *)&_%sStrings[%i])\n"
					"# endif\n",
					name, name, app_name, curr_size, name,
					name, app_name, curr_size);
			}
			else
			{
				/* regular defines */
				printf("# ifndef XmN%s\n", name);
				printf("#  define XmN%s \"%s\"\n", name, name);
				printf("# endif\n");

				/* collect string table references */
				sprintf(msg,
					"# ifndef XmN%s\n"
					"#  define XmN%s ((char *)&_%sStrings[%i])\n"
					"# endif\n",
					name, name, app_name, curr_size);
			}
			curr_size += strlen(name)+1;

			/* resize output buffer if we're about to overflow */
			if(strlen(outbuf) + strlen(msg) >= bufsize)
			{
				bufsize += MAX_OUTPUT_SIZE;
				outbuf = (char*)realloc(outbuf, bufsize);
			}
			strcat(outbuf, msg);
		}
		printf("\n#else\n\n");

		/* flush string table references */
		printf("%s", outbuf);

		/* write closure */
		printf(closure, app_name, app_name, "Strings");

		free(outbuf);
	}
}

static int
StringLength(char *name)
{
	char *chPtr;
	int i = 0;

	/* compute length of string */
	for(chPtr = name, i = 0; *chPtr != '\0'; chPtr++)
	{
		switch(*chPtr)
		{
			/* hmm, a backslash, check if it's used as a delimiter */
			case '\\':
				switch(*(chPtr+1))
				{
					/*****
					* When the next char is any of these two,
					* they take up 2 chars in input but only one
					* in the output. Account for it.
					*****/
					case 'f':	/* formfeed */
					case 'r':	/* carriage return */
					case 'v':	/* vertical tab */
					case 't':	/* horizontal tab */
					case 'n':	/* newline */
					case '"':	/* quote */
					case '\\':	/* backslash */
						chPtr++;
				}
			default:
				i++;
		}
	}
	return(i);
}

static void
writeWarningTable(char what, char *app_name)
{
	char name[512];
	char *chPtr;
	int i, curr_size = 0;

	/* generate a c-source file */
	if(what == 'c')
	{
		printf(motif_source, app_name, "Messages");

		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
			{
				memset(&name, 0, 512);
				continue;
			}

			name[strlen(name)-1] = '\0';	/* strip newline */
			printf("/* %5i */ \"%s\\0\"\n", curr_size, name);

			curr_size += StringLength(name)+1;
			memset(&name, 0, 512);
		}
		printf("/* ????? */\n\"\\0\";\n");
	}
	else	/* need a header file */
	{
		int msg_count = 0;
		char pfx[256];
		char msg[512];
		char *outbuf;
		int bufsize = MAX_OUTPUT_SIZE;

		for(msg_count = 0; msg_count < strlen(app_name); msg_count++)
			pfx[msg_count] = toupper(app_name[msg_count]);
		pfx[msg_count] = '\0';
		msg_count = 0;

		printf("\n#ifndef __%s_Messages_h__\n", app_name);
		printf("#define __%s_Messages_h__\n", app_name);

		printf(motif_header, app_name, app_name, app_name, "Messages",
			app_name);

		outbuf = (char*)calloc(1, bufsize);

		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
			{
				memset(&name, 0, 512);
				continue;
			}

			name[strlen(name)-1] = '\0';	/* strip newline */

			printf("# define %s_MSG_%i \\\n \"%s\"\n", pfx, msg_count, name);
			sprintf(msg, "# define %s_MSG_%i ((char *)&_%sMessages[%i])\n",
				pfx, msg_count, app_name, curr_size);

			curr_size += StringLength(name)+1;

			/* resize output buffer if we're about to overflow */
			if(strlen(outbuf) + strlen(msg) >= bufsize)
			{
				bufsize += MAX_OUTPUT_SIZE;
				outbuf = (char*)realloc(outbuf, bufsize);
			}
			strcat(outbuf, msg);
			msg_count++;
			memset(&name, 0, 512);
		}
		printf("\n#else\n\n");
		printf("%s", outbuf);

		printf(closure, app_name, app_name, "Messages");
		free(outbuf);
	}
}

static void
writeXtStrings(char what, char *app_name)
{
	char name[512];
	int curr_size = 0;

	/* generate a c-source file */
	if(what == 'c')
	{
		printf("\n#if XtSpecificationRelease < 6\n");
		printf("#include <X11/IntrinsicP.h>\n");
		printf("#endif\n\n");

		printf("/*****\n* The missing commas are intentional. Let the "
			"compiler\n* concatenate the strings\n*****/\n\n");
		printf("#ifndef _XtStringDefs_h_Const\n");
		printf("# ifdef __STDC__\n");
		printf("#  define _XtStringDefs_h_Const const\n");
		printf("# else\n");
		printf("#  define _XtStringDefs_h_Const\n");
		printf("# endif\n");
		printf("#endif\n");
		printf("_XtStringDefs_h_Const char _%sStrings[] =\n", app_name);
		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
				continue;

			name[strlen(name)-1] = '\0';	/* strip newline */
			printf("/* %5i */ \"%s\\0\"\n", curr_size, name);
			curr_size += strlen(name)+1;
		}
		printf("/* ????? */\n\"\\0\";\n");
	}
	else	/* need a header file */
	{
		printf("\n#ifndef __%s_Strings_h__\n", app_name);
		printf("#define __%s_Strings_h__\n", app_name);
		printf("\n_XFUNCPROTOBEGIN\n");

		printf("\n/*****\n* Don't define XTSTRINGDEFINES if you want to "
			"save space\n*****/\n", app_name);

		printf("\n#ifndef XTSTRINGDEFINES\n");
		printf("# ifndef _XtStringDefs_h_Const\n");
		printf("#  ifdef __STDC__\n");
		printf("#   define _XtStringDefs_h_Const const\n");
		printf("#  else\n");
		printf("#   define _XtStringDefs_h_Const\n");
		printf("#  endif\n");
		printf("# endif\n");
		printf("extern _XtStringDefs_h_Const char _%sStrings[];\n", app_name);
		printf("#endif\n\n");
		printf("#ifndef _XtStringDefs_h_\n");
		printf("#include <X11/StringDefs.h>\n");
		printf("#endif\n\n");
		
		while((fgets(name, 512, stdin)) != NULL)
		{
			if(name[0] == ';')
				continue;

			name[strlen(name)-1] = '\0';	/* strip newline */

			printf("\n#ifdef XTSTRINGDEFINES\n");
			if(isupper(name[0]))
			{
				printf("# ifndef XtC%s\n", name);
				printf("#  define XtC%s \"%s\"\n", name, name);
				printf("# endif\n");
				printf("# ifndef XtR%s\n", name);
				printf("#  define XtR%s \"%s\"\n", name, name);
				printf("# endif\n");
			}
			else
			{
				printf("# ifndef XtN%s\n", name);
				printf("#  define XtN%s \"%s\"\n", name, name);
				printf("# endif\n");
			}
			printf("#else\n");
			if(isupper(name[0]))
			{
				printf("# ifndef XtC%s\n", name);
				printf("#  define XtC%s ((char *)&_%sStrings[%i])\n",
					name, app_name, curr_size);
				printf("# endif\n");
				printf("# ifndef XtR%s\n", name);
				printf("#  define XtR%s ((char *)&_%sStrings[%i])\n",
					name, app_name, curr_size);
				printf("# endif\n");

			}
			else
			{
				printf("# ifndef XtN%s\n", name);
				printf("#  define XtN%s ((char *)&_%sStrings[%i])\n",
					name, app_name, curr_size);
				printf("# endif\n");
			}
			printf("#endif\n");
			curr_size += strlen(name)+1;
		}
		printf("\n_XFUNCPROTOEND\n");
		printf("\n/* Don't add anything after this endif! */\n");
		printf("#endif /* __%s_Strings_h__ */\n", app_name);
	}
}

/*****
* Name:			main
* Return Type:	int
* Description:	generates an Xt/Motif-compliant String table
* In:
*	argc:		no of args, must be exactly 4
*	argv:		array of command line options.
* Returns:
*	EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise.
* Note:
*	this small tool takes a list of strings (given via stdin), and generates
*	a string table when called with the ``c'' arg. It generates a
*	corresponding header file when called with the ``h'' arg.
*	Output goes to stdout.
*	The stringtable name is given by the second argument.
*	The type of the stringtable is given by the fourth argument.
*	The generated header file should be installed.
*	The generated c file should be compiled.
*	A generated string table has the name _appname_Strings.
*	A generated message table has the name _appname_Messages.
*****/
int
main(int argc, char **argv)
{
	time_t now;
	char *app_name;

	if(argc < 3 || argc > 4 || (argv[2][0] != 'c' && argv[2][0] != 'h'))
	{
		fprintf(stderr, "%s: generate an Xt or Motif-compliant StringTable.\n",
			MKSTRINGS_VERSION);
		fprintf(stderr, "\nTakes a list of strings as input and "
				"generates a C source\nor C include file as output.\n");
		fprintf(stderr, "\nUsage: mkStrings name [ch] [xm]\n\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "    name: name of application/library for which a\n");
		fprintf(stderr, "          stringtable is to be generated.\n");
		fprintf(stderr, "    c   : generate a C source file\n");
		fprintf(stderr, "    h   : generate a C include file\n");
		fprintf(stderr, "    x   : generate Xt string table\n");
		fprintf(stderr, "    m   : generate Xm string table\n");
		fprintf(stderr, "    w   : generate message table\n\n");
		fprintf(stderr, "Input is read from stdin and output is written to "
			"stdout.\nLines starting with a ; are ignored. Everything else "
			"is considered\nvalid input, including empty lines.\n");
		exit(EXIT_FAILURE);
	}

	app_name = argv[1];

	now = time(NULL);
	printf("/*****\n");
	printf("* Automatically generated file.\n");
	printf("*  ***DO NOT EDIT THIS FILE***\n");
	printf("*****/\n");
	printf("/*****\n");
	printf("* %s, Build Date: %s %s\n", MKSTRINGS_VERSION, __DATE__, __TIME__);
	printf("* File created at: %s", ctime(&now));
	printf("*****/\n");

	if(argc == 4)
	{
		switch(argv[3][0])
		{
			case 'x':
				writeXtStrings(argv[2][0], app_name);
				break;
			case 'w':
				writeWarningTable(argv[2][0], app_name);
				break;
			default:
				writeMotifStrings(argv[2][0], app_name);
		}
	}
	else
		writeMotifStrings(argv[2][0], app_name);

	exit(EXIT_SUCCESS);
}
