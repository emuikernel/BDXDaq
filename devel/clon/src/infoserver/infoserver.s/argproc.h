#ifndef SCA_ARGPROC
#define SCA_ARGPROC
/***********************************************************************
 *
 * argproc.h
 *
 * Includes code for processing command line arguments.  The set of
 * arguments is standard for all the scaler related programs so it is
 * useful to define a single function for dealing with them.
 * The individual programs are free to ignore the results of the args
 * as required
 *
 * By: J. Hardie
 ***********************************************************************/

/* standard argument variables */
char *arg_config_file = "./display.config";	/* configuration file name */
char *arg_app_name = "clastest";	/* SmartSockets application name */
char *arg_data_reader = "./reader";	/* Data reader program path */
char arg_direct_readout=0;	/* Flags direct ROC readout */
char arg_mod_events=1;		/* modulo factor for events */

/* standard subroutine for reading and processing arguments.  This 
 * function should be passed ARGV and ARGC from the main program.
 * It scans the arguments and stores the correct values.  Any unrecognized
 * arguments are ignored.
 */
void argproc(int ARGC, char **ARGV)
{
    int i;
    char *value,*arg;
    char flag;

    for (i=0; i< ARGC; i++)
    {
	arg = ARGV[i];
	if (*arg == '-')
	{
	    flag = *(++arg);
	    value = ++arg;
	    if (!*value)
		value = ARGV[i+1];

	    switch (flag)
	    {
	    case 'r':
		arg_data_reader = value;
		break;
	    case 'f':
		arg_config_file = value;
		break;
	    case 'a':
		arg_app_name = value;
		break;
	    case 'd':
		arg_direct_readout = 1;
		break;
	    case 'm':
	        arg_mod_events = atoi(value);
	    default:
		break;
	    }
	}
    }
}
#endif SCA_ARGPROC
