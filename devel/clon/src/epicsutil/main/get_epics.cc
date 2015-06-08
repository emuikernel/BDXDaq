/* TODO: implement ca_pend_time ?? 
 */

//
//  get_epics
//
//  Reads file of epics channel names, gets data, then writes data to stdout
//  If file doesn't exist uses file as channel name
//
//
//  File format
//  -----------
//    # first non-blank char is a comment line
//    B_xxx.DVI                   // reads channel B_xxx   via "get DVI"
//    MTIRBCK                     // reads channel MTIRBCK via "get"
//
//
//  usage:
//       get_epics [-c ca_pend_time] infile [outfile]
//
//
//  ejw, 9-may-97
// sergey: implement CA-based version, eliminate cdev

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

#include "epicsutil.h"

// max number of elements in CA array
#define MAX_CHANNELS_LENGTH 1000

// max length of epics ca name
#define MAXCANAMELEN 60

// misc
static char *epics_filename    = NULL;
static char *outfile           = NULL;
static int ca_pend_time      = 60;
static int ncallback           = 0;
static char *channel[MAX_ENTRY];
static char channel_get[MAX_ENTRY][10];
static float channel_val[MAX_ENTRY];
static int caget_mode          = 0;


// prototypes
void decode_command_line(int argc, char **argv);
int get_epics_data(void);



//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  // decode command line
  decode_command_line(argc,argv);

  // get HV data
  get_epics_data();

  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------

int
get_epics_data()
{
  char buffer[120];
  int nentry=0;
  char chget[20];
  int result;

  // read channel names from file and create request objects...ignore comments and blank lines
  ifstream file(epics_filename);
  if(file.is_open())
  {
    while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad())
    {
      file.getline(buffer,sizeof(buffer));
      if(strlen(buffer)==0)continue;
      if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
      if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
      if(nentry>=MAX_ENTRY)
      {
	    cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
	    break;
      }
      nentry++;
      channel[nentry-1] = (char *) malloc(MAXCANAMELEN+1);
      sscanf(buffer+strspn(buffer," \t\n\r\b"),"%[^ .].%[^. ]",
	     channel[nentry-1],channel_get[nentry-1]);
      channel_val[nentry-1]=-9999.;
      if(strlen(channel_get[nentry-1])==0) strcpy(channel_get[nentry-1],"VAL");
    }
    file.close();
  }
  else
  {
    caget_mode=1;
    ca_pend_time=1;
    nentry=1;
    channel[0] = (char *) malloc(MAXCANAMELEN+1);
    sscanf(epics_filename+strspn(epics_filename," \t\n\r\b"),"%[^ .].%[^. ]",
	   channel[0],channel_get[0]);
    channel_val[0]=-9999.;
    if(strlen(channel_get[0])==0) strcpy(channel_get[0],"VAL");
  }







  /*** sergey: new CA stuff ***/

  result = getepics(nentry, channel, channel_val);
  /*printf("%d %d >%s< -> %f\n",result,nentry,channel[0],channel_val[0]);*/

  /****************************/







  /*
  // check if all callbacks received
  if(ncallback<nentry) {
    if(caget_mode==0) {
      cerr << "?only received " << ncallback << " callbacks" << " out of " 
	 << nentry << " expected" << endl;
    } else {
      cout << channel_val[0] << endl;
      return;
    }
  }    
  */



  // write results
  if(outfile==NULL)
  {
    cout.setf(ios::left);
    for(int i=0; i<nentry; i++)
    {
      if(caget_mode==0)
      {
	    strstream name;
	    name << channel[i] << "." << channel_get[i] << ends;
	    cout << setw(30) << name.str() << channel_val[i] << endl;
      }
      else
      {
	    cout << channel_val[0] << endl;
      }
    }
  }
  else
  {
    ofstream ofile(outfile);
    ofile.setf(ios::left);
    for(int i=0; i<nentry; i++)
    {
      strstream name;
      name << channel[i] << "." << channel_get[i] << ends;
      ofile << setw(30) << name.str() << channel_val[i] << endl;
    }
  }

  for(int i=0; i<nentry; i++) free(channel[i]);

  return(result);
}



//---------------------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help = "\n  get_epics [-c ca_pend_time] infile [outfile]\n\n";

  if(argc<2)
  {
    cout << help;
    exit(EXIT_SUCCESS);
  }

  while(i<argc)
  {
    if(strncasecmp(argv[i],"-h",2)==0)
    {
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-c",2)==0)
    {
      ca_pend_time=atoi(argv[i+1]);
      i += 2;
    }
    else if (strncasecmp(argv[i],"-",1)!=0)
    {
      epics_filename=strdup(argv[i]);
      if(argc>i+1) outfile=strdup(argv[i+1]);
      break;
    }
  }

  if(epics_filename==NULL)
  {
    cout << help;
    exit(EXIT_SUCCESS);
  }

  return;
}


/*---------------------------------------------------------------------*/

