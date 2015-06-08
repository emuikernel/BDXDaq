h25437
s 00000/00000/00000
d R 1.2 98/01/22 12:44:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/includes/procargs.h
c Name history : 1 0 includes/procargs.h
e
s 00065/00000/00000
d D 1.1 98/01/22 12:44:48 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
#ifndef PROCARGS_H_J
#define PROCARGS_H_J
/***********************************************************************
 * procargs.h
 *
 * Definitions for standard argument processing.
 * Note:  None of these arguments can affect any other arguments on the
 * command line - they are orthogonal.
 *
 * J. Hardie  Dec 1996
 ***********************************************************************/

/* definition of the argument table entry */
typedef struct {
    char *argname;		/* the name of the argument */
    int  argtype;		/* determines the type of cast for the following pointer */
    void *argvar;		/* pointer to the variable containing the arg variable */
    char *usestring;		/* message for usage */
} argentry;


/*
 * Types of arguments.  These are:
 * TOGGLE - a toggle. 
 * INTEGER - specifies an integer value
 * STRING - a character string
 * FILENAME - a character string that is supposed to be a valid file name
 * DOUBLE - floating point value
 * USAGE - print a usage message
 */
#define TOGGLE 0
#define INTEGER 1
#define STRING 2
#define FILENAME 3
#define DOUBLE 4
#define USAGE 5


/*
 * The next two definitions are used to begin and end the definition of the
 * argument table
 */
#define ARGTABLE argentry argtable[] = {
#define ENDTABLE {(char *)"usage", USAGE, (void *)NULL, (char *)NULL}, \
{(char *)NULL, 0, (void *)NULL, (char *)NULL}};

/*
 * The following definition adds an entry to the argument table
 */
#define ARGENTRY(arg,typ,varp,ustr) {(char *)arg, (int)typ, (void *)varp,(char *)ustr}


/*
 * This define simplifies things considerably
 */
void process_arglist(int argc, char **argv, argentry *argtable);
#define GETARGS(ac,av) process_arglist(ac,av,argtable)

#endif PROCARGS_H_J






E 1
