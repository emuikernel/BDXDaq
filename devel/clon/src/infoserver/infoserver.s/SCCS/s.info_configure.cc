h28022
s 00000/00000/00025
d D 1.4 97/06/05 11:58:11 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00025
d D 1.3 97/06/05 10:40:36 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00003/00033/00022
d D 1.2 97/06/02 18:37:14 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_configure.cc
c Name history : 2 1 server/info_configure.cc
c Name history : 1 0 s/server/info_configure.cc
e
s 00055/00000/00000
d D 1.1 97/03/25 09:44:59 jhardie 1 0
c New C++ version of info-server program.  Translation
c from C because of complexity of internal structures.
c These are better handled with classes.
e
u
U
f e 0
t
T
I 1
//**********************************************************************
// info_configure.cc
//
// Routines handling the configuration of the info_server program.
// The routines in here read the configuration file, build the objects
// which define the input and output stream and provide the utility
// functions which manipulate the incoming and outgoing messages
//
// Version: %A% %I%
//
// Programmer: J.G. Hardie
//**********************************************************************

#include "info_includes.hh"
D 3
#include "info_proto.hh"
E 3
#include "info_classes.hh"
#include "info_globals.hh"
I 3
#include "info_proto.hh"
E 3

D 3
void make_config_objects();
void process_config_file(char *config);
void check_objects();
E 3

D 3


void config_info_c(char *config)
{
    if (config)
	process_config_file(config);
}

void process_config_file(char *config)
{
    ifstream infile(config);
    if (!infile)
	cout << "Error: could not open file: " << config << endl;
    else
    {
	char ch;
	while (infile.get(ch))
	    cout.put(ch);
    }
}

void check_objects()
{
}

void make_config_objects()
{
}

E 3
char *data_stream_status()
{
    return NULL;
}
I 3


E 3
E 1
