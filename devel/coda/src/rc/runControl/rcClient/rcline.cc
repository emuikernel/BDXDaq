// TEST program to communicate with run control server
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcSvcProtocol.h>
#include <rcMsg.h>
#include <rcClient.h>

static void callback (int status, void* arg, daqNetData* data)
{
  if (status == CODA_SUCCESS)
    printf ("command finished successfully\n");
  else
    printf ("command failed ++++++++++++++++++\n");    
}

static void getValCallback (int status, void* arg, daqNetData* data)
{
  rcClient* obj = (rcClient *)arg;

  if (status == CODA_SUCCESS) {
    printf ("get data %s %s is %s\n",data->name(), data->attribute(),
	    (char *)(*data));
    printf ("Pend IO command returns %d\n", obj->pendIO (2.0));
  }
  else 
    printf ("Get value has bad value\n");
}

static void setValCallback (int status, void* arg, daqNetData* data)
{
  if (status == CODA_SUCCESS) {
    printf ("set data %s %s is %s\n",data->name(), data->attribute(),
	    (char *)(*data));
  }
  else 
    printf ("set value has bad value\n");
}

static void monCallback (int status, void* arg, daqNetData* data)
{
  if (status == CODA_SUCCESS) {
    printf ("data %s %s changed to %s\n",data->name(), data->attribute(),
	    (char *)(*data));
  }
  else 
    printf ("monitor value has bad value\n");
}

static void msgCallback (int status, void* arg, daqNetData* data)
{
  if (status == CODA_SUCCESS)
    printf ("%s", (char *)(*data));
}

static void monOffCallback (int status, void* arg, daqNetData* data)
{
  if (status == CODA_SUCCESS) {
    printf ("data %s %s monitor off \n",data->name(), data->attribute());
  }
  else 
    printf ("monitor off failed\n");
}

main (int argc, char **argv)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (argc != 4){
    fprintf (stderr, "Usage: %s database session msqld\n", argv[0]);
    exit (1);
  }
  char dbase[256];
  char session[256];
  
  strcpy (dbase, argv[1]);
  strcpy (session, argv[2]);

  rcClient client;

  int status = client.connect (argv[1], argv[2], argv[3]);
#else
  if (argc != 4){
    fprintf (stderr, "Usage: %s host name id\n", argv[0]);
    exit (1);
  }
  rcClient client;

  int status = client.connect (argv[1], argv[2], atoi (argv[3]));
#endif
  
  if (status != CODA_SUCCESS) {
    printf ("%s RunControl Server is not running\n", argv[2]);
    exit (1);
  }

  client.monitorOnCallback ("RCS", "runMessage", msgCallback, 0);

  char command[32];
  int count;
  printf ("Enter rcServer Command\n");
  while (1){
    ioctl (fileno (stdin), FIONREAD, &count);
    if (count > 0) {
      scanf ("%s", command);
      if (::strcmp (command, "disconnect") == 0)
	client.disconnect ();
      else if (::strcmp (command, "quit") == 0)
	break;
      else if (::strcmp (command, "load") == 0) {
	char* temp[2];
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	daqData data ("RCS", "command", "unkown");
	// insert database name + session name into data object
	temp[0] = new char[::strlen (dbase) + 1];
	::strcpy (temp[0], dbase);
	temp[1] = new char[::strlen (session) + 1];
	::strcpy (temp[1], session);
	data.assignData (temp, 2);
#else
	printf ("Enter database and session name\n");
	char direc[256];
	scanf ("%s", direc);

	daqData data ("RCS", "command", "unkown");
	// insert database name + session name into data object
	char* temp[2];
	temp[0] = new char[::strlen (direc) + 1];
	::strcpy (temp[0], direc);
	temp[1] = new char[::strlen ("unknown") + 1];
	::strcpy (temp[1], "unknown");
	data.assignData (temp, 2);
#endif
	// free memory
	delete []temp[0]; 
	delete []temp[1];
  	
	status = client.sendCmdCallback (DALOADDBASE, data, callback, 0);
      }
      else if (::strcmp (command, "getruntypes") == 0)
	status = client.getValueCallback (argv[2], "allRunTypes", 
					  getValCallback, (void *)&client);
      else if (::strcmp (command, "configure") == 0) {
	printf ("Enter runtype name\n");
	{
	  char runtype[32];
	  scanf ("%s",runtype);
	  daqData data ("RCS","command",runtype);
	  status = client.sendCmdCallback (DACONFIGURE, data, callback, 0);
	}
      }
      else if (::strcmp (command, "download") == 0) {
	daqData data ("RCS", "command", (int)DADOWNLOAD);
	status = client.sendCmdCallback (DADOWNLOAD, data, callback, 0);
      }	
      else if (::strcmp (command, "abort") == 0) {
	daqData data ("RCS", "command", (int)DAABORT);
	status = client.sendCmdCallback (DAABORT, data, callback, 0);
      }
      else if (::strcmp (command, "reset") == 0) {
	daqData data ("RCS", "command", (int)DATERMINATE);
	status = client.sendCmdCallback (DATERMINATE, data, callback, 0);
      }
      else if (::strcmp (command, "getvalue") == 0) {
	printf ("Enter component + attribute\n");
	{
	  char compname[32];
	  char attr[32];
	  scanf ("%s %s",compname, attr);
	  client.getValueCallback (compname, attr, getValCallback, 
				   (void *)&client);
	}
      }
      else if (::strcmp (command, "setvalue") == 0) {
	printf ("Enter component + attribute + value (string)\n");
	{
	  char compname[32];
	  char attr[32];
	  char val[256];
	  scanf ("%s %s %s",compname, attr, val);
	  daqData data (compname, attr, val);
	  client.setValueCallback (data, setValCallback, 0);
	}
      }
      else if (::strcmp (command, "monitorOn") == 0) {
	printf ("Enter component + attribute\n");
	{
	  char compname[32];
	  char attr[32];
	  scanf ("%s %s",compname, attr);
	  client.monitorOnCallback (compname, attr, monCallback, 0);
	}
      }
      else if (::strcmp (command, "monitorOff") == 0) {
	printf ("Enter component + attribute\n");
	{
	  char compname[32];
	  char attr[32];
	  scanf ("%s %s",compname, attr);
	  client.monitorOffCallback (compname, attr, monCallback, 0,
				     monOffCallback, 0);
	}
      }
      else if (::strcmp (command, "changeState") == 0) {
	printf ("Enter initial and final state\n");
	{
	  int state[2];
	  scanf ("%d %d",&state[0], &state[1]);
	  daqData data ("RCS","command", state, 2);
	  client.sendCmdCallback (DACHANGE_STATE, data, callback, 0);
	}
      }
      else if (::strcmp (command, "test") == 0) {
	daqData data ("RCS", "command", (int)DATEST);	
	status = client.sendCmdCallback (DATEST, data, callback, 0);
      }
      else
	printf ("Illegal command \n");
      printf ("Enter rcServer Command\n");
      fflush (stdout);
    }
    client.pendIO (0.1);
  }
}
