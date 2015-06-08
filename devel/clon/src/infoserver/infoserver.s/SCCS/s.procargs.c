h26060
s 00000/00000/00000
d R 1.2 98/01/22 12:44:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/includes/procargs.c
c Name history : 1 0 includes/procargs.c
e
s 00308/00000/00000
d D 1.1 98/01/22 12:44:49 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
/**************************************************************************
 * procargs.c
 *
 * Utility functions for parsing the command line and setting the appropriate
 * variables.
 * The argument variables are stored in an external structure, the ARGTABLE 
 * which contains any number of rows.
 *************************************************************************/
#include <string.h>
#include <math.h>

#include "procargs.h"

argentry *search_argtable(char *argflag, argentry *argtable);
void print_argtable(argentry *argtable);
void show_usage(char *progname, argentry *argtable);

#ifdef ATDEBUG
#define dpvar(s,v) printf(s,v)
#define dpmsg(s) printf(s)
#else
#define dpvar(s,v)
#define dpmsg(s)
#endif


/*
 * process_arglist
 * scans through the passed argument array, locating any command line
 * switches and comparing them to the values stored in the argument
 * table.  If the argument is found, its value is stored in the
 * correct variable.  If the argument is not found, or if the arg table
 * contains some unknown argument type, the argument is also ignored.
 */
void process_arglist(int argc, char **argv, argentry *argtable)
{
    int i,j;
    char *arg, **cval;
    double *dval,thevalue;
    int *ival;
    argentry *the_arg;

    dpmsg("Entered process_arglist\n");
    for (i=0; i<argc; i++)
    {
	arg = argv[i];

	dpvar("Argument %d ",i); dpvar(" = %s\n",arg);

	switch (*arg)
	{
	case '-':
	    dpmsg("It was type\n");
	    the_arg = search_argtable(++arg,argtable);
	    if (the_arg != NULL) /* found it */
	    {
		switch (the_arg->argtype)
		{
		case TOGGLE:
		    dpmsg(" STRING ");
		    ival = (int *)(the_arg->argvar);
		    *ival = 1;
		    dpmsg(" value 1");
		    break;
		case INTEGER:
		    i++;
		    dpmsg(" INTEGER ");
		    ival = (int *)(the_arg->argvar);
		    *ival = atoi(argv[i]);
		    dpvar("value %d",*ival);
		    break;
		case STRING:
		    i++;
		    dpmsg(" STRING ");
		    cval = (char **)(the_arg->argvar);
		    *cval = argv[i];
		    dpvar("value %s",*cval);
		    break;
		case FILENAME:
		    i++;
		    dpmsg(" FILENAME ");
		    cval = (char **)(the_arg->argvar);
		    *cval = argv[i];
		    dpvar("value %s",*cval);
		    break;
		case DOUBLE:
		    i++;
		    dpmsg(" DOUBLE ");
		    dval = (double *)(the_arg->argvar);
		    dpvar("String: %s: ",argv[i]);
		    sscanf(argv[i],"%lf",&thevalue);
		    dpvar("Trans: %lf: ",thevalue);
		    *dval = thevalue;
		    dpvar("value %lf",*dval);
		    break;
		case USAGE:
		    show_usage(argv[0],argtable);
		    break;
		default:
		    /* unrecognized type - do nothing */
		    dpmsg(" UNKNOWN");
		    break;
		}
	    }
	    else
	    {
		dpmsg(" Unknown Argument");
	    }
	    break;
	default:
	    dpmsg("It was not an argument");
	    break;
	}
	dpmsg("\n");
    }
}


/*
 * search_argtable
 *
 * Scans through the argument table looking for a match for the arg
 * string.  If one is found, returns a pointer to the table entry.  If
 * no match is found, it returns a null.
 *
 * Since the tables are small, use a linear search.  Nothing fancy
 */
argentry *search_argtable(char *argflag, argentry *argtable)
{
    argentry *the_arg;
    int i=0;
    int arglen;

    arglen = strlen(argflag);

    while (argtable[i].argname != NULL)
    {
	if (!strncmp(argflag, argtable[i].argname, arglen))
	    return( &(argtable[i]));
	i++;
    }
    return((argentry *)NULL);
}


/*
 * show_usage
 * Prints a usage message showing the name of the program, the
 * type of each argument and the current value for each argument.
 *
 */
void show_usage(char *progname, argentry *argtable)
{
    int i;
    int *ival;
    char **cval;
    double *dval;
    char *umsg;

    i=0;
    printf("Usage: %s [args...]\n",progname);
    printf("Where [args...] contains one or more of:\n");

    while (argtable[i].argname != NULL)
    {
	printf("-%s ",argtable[i].argname);

	if (argtable[i].usestring) 
	    umsg = argtable[i].usestring;
	else
	    umsg = NULL;

	switch (argtable[i].argtype) 
	{
	case TOGGLE:
	    ival = (int *)(argtable[i].argvar);
	    if (umsg)
		printf("%s",umsg);
	    else
		printf("is an on/off switch");
	    printf(" (Currently %s)",( (*ival) ? "On" : "Off" ));
	    break;
	case INTEGER:
	    ival = (int *)(argtable[i].argvar);
	    if (umsg)
		printf("<int> %s",umsg);
	    else
		printf("<int> is an integer value");
	    printf(" (Currently %d)",*ival);
	    break;
	case STRING:
	    cval = (char **)(argtable[i].argvar);
	    if (umsg)
		printf("<string> %s",umsg);
	    else
		printf("<string> is a character string");
	    if (*cval)
		printf(" (Currently %s)",*cval);
	    else
		printf(" (Currently UNSET)");
	    break;
	case FILENAME:
	    cval = (char **)(argtable[i].argvar);
	    if (umsg)
		printf("<file> %s",umsg);
	    else
		printf("<file> is a valid file name");
	    if (*cval)
		printf(" (Currently %s)",*cval);
	    else
		printf(" (Currently UNSET)");
	    break;
	case DOUBLE:
	    dval = (double *)(argtable[i].argvar);
	    if (umsg)
		printf("<double> %s",umsg);
	    else
		printf("<double> is a double precision number");
	    printf(" (Currently %lf)",*dval);
	    break;
	case USAGE:
	    printf("Prints this help message");
	    break;
	default:
	    printf("UNRECOGNIZED TYPE %d",argtable[i].argtype);
	    break;
	}
	printf("\n");
	i++;
    }
    exit(0);
}



/*
 * print_argtable
 * Prints out the argument table and any values that are currently defined
 */
void print_argtable(argentry *argtable)
{
    int i;
    int *ival;
    char **cval;
    double *dval;

    i=0;
    while (argtable[i].argname != NULL)
    {
	printf("Table[%d] Arg: %s  Type: ",i,argtable[i].argname);
	switch (argtable[i].argtype) 
	{
	case TOGGLE:
	    ival = (int *)(argtable[i].argvar);
	    printf("TOGGLE  Val: %d",*ival);
	    break;
	case INTEGER:
	    ival = (int *)(argtable[i].argvar);
	    printf("INTEGER  Val: %d",*ival);
	    break;
	case STRING:
	    cval = (char **)(argtable[i].argvar);
	    printf("STRING  Val: %s",*cval);
	    break;
	case FILENAME:
	    cval = (char **)(argtable[i].argvar);
	    printf("FILENAME  Val: %s",*cval);
	    break;
	case DOUBLE:
	    dval = (double *)(argtable[i].argvar);
	    printf("DOUBLE  Val: %f",*dval);
	    break;
	default:
	    printf("UNRECOGNIZED TYPE %d",argtable[i].argtype);
	    break;
	}
	printf("\n");
	i++;
    }
}


#ifdef ATDEBUG

char *name = "No Name";
char *file = "No File";
char *config = "No Config";
int intflag = 0;
double dtype = 0.0;
int intvalue = 0;

ARGTABLE
ARGENTRY("name",STRING,&name,"is the application name"),
ARGENTRY("file",FILENAME,&file,"is the configuration file name"),
ARGENTRY("config",STRING,&config,"is a set of configuration options"),
ARGENTRY("flag",TOGGLE,&intflag,"turns on debugging"),
ARGENTRY("doub",DOUBLE,&dtype,"is a calibration constant"),
ARGENTRY("int",INTEGER,&intvalue,"is the maximum number of events"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

void main(int argc, char **argv)
{
    GETARGS(argc,argv);
    print_argtable(argtable);
}

#endif
E 1
