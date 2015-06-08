h31914
s 00001/00001/01150
d D 1.2 07/11/11 11:37:51 boiarino 2 1
c 
e
s 01151/00000/00000
d D 1.1 06/10/19 15:41:11 boiarino 1 0
c date and time created 06/10/19 15:41:11 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMDaq                                                               //
//                                                                      //
//  Daq for Moller Polarimeter                                          //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>
#include <strings.h>

#include <TROOT.h>
#include "TRandom.h"

#include "bos.h"
#include "bosio.h"
#include "bosfun.h"

#include "CMDaq.h"
#include "CMPolar.h"

#include "CMDisplay.h" // to get gCMDisplay
extern CMDisplay *gCMDisplay;

ClassImp(CMDaq)


CMDaq *gCMDaq;

// ET stuff

#include "et.h"
#include "etbosio.h"

et_openconfig  openconfig;
et_statconfig  sconfig;
et_stat_id     my_stat;
et_sys_id      id;
char *et_station_name = "ROOT";
int         et_opened = 0;
int         et_created = 0;
int         et_attached = 0;
int nevents_max, event_size;

static int  et   = 0;
ETSYS *sysptr;
et_event       **pe;
et_event       *pevent;
et_att_id      attach;


// file staff
Int_t file = 0;
BOSIO *desc;
// end of file staff


#define NBCS  500000

Int_t kUPDATE;
Int_t *iw;

//_____________________________________________________________________________
CMDaq::CMDaq(Option_t *fname)
{
// Create one CMDaq object

  gCMDaq = this;
  printf("CMDaq constructor: gCMDaq=0x%08x &gCMDaq=0x%08x\n",gCMDaq,&gCMDaq);

  m_Display = (CMDisplay *) gCMDisplay->Display();
  printf("CMDaq: gCMDisplay=0x%08x, m_Display=0x%08x\n",gCMDisplay,m_Display);

  m_Pad = gCMDisplay->Pad();

  gCMDisplay->SetDaq(this);

  strcpy(filename,fname);
  gCMDisplay->SetTitle(fname);


  nclient = 0;
  downloaded = 0;
  prestarted = 0;
  active = 0;
  ended = 0;

  newdstfile = 0;

}

//_____________________________________________________________________________
CMDaq::~CMDaq()
{
//

  Clear();
}



//_____________________________________________________________________________
void CMDaq::AddClient(CMClient *Client)
{
//

  Int_t i;

  // check if that client are exist already
  for(i=0; i<nclient; i++)
  {
    if(m_Client[i] == (CMClient *)Client)
    {
      printf("CMDaq::AddClient: client is registered already\n");
      return;
    }
  }

  // add the client
  if(nclient < NCLIENT)
  {
    clientActive[nclient] = 0; // no one active in the begining
    m_Client[nclient++] = (CMClient *)Client;
    printf("CMDaq::AddClient: client number %d is registered\n",nclient-1);
  }
  else
  {
    printf("CMDaq::AddClient: there is no empty slots\n");
  }

}



//_____________________________________________________________________________
void CMDaq::RemoveClient(CMClient *Client)
{
//

  Int_t i, j;

  for(i=0; i<nclient; i++)
  {
    if(m_Client[i] == (CMClient *)Client)
    {
      for(j=i; j<i-1; j++)
      {
        clientActive[j] = clientActive[j+1];
        m_Client[j] == m_Client[j+1];
      }
      printf("CMDaq::RemoveClient: client 0x%08x is removed\n",(TObject *)Client);
      return;
    }
  }

  printf("CMDaq::RemoveClient: client 0x%08x is not registered yet\n",(TObject *)Client);

}


//_____________________________________________________________________________
void CMDaq::ChangeClient(Option_t *dtext)
{
// change an clientActive

  if(!downloaded)
  {
    printf("CMDaq::ChangeClientm: you have to download first !!!\n");
    return;
  }

  if(clientActive[0] == 0)
  {
    clientActive[0] = 1;
    clientActive[1] = 0;
    strcpy((char *)dtext,"CLAS");
    m_Client[0]->Update();
  }
  else
  {
    clientActive[0] = 0;
    clientActive[1] = 1;
    strcpy((char *)dtext,"MOLLER");
    m_Client[1]->Update();
  }
  printf("CMDaq::ChangeClient(): set clientActive -> %1d %1d\n",
                 clientActive[0],clientActive[1]);


}


//_____________________________________________________________________________
void CMDaq::Download()
{
// download all clients (active and not active)

  Int_t i, status;

  for(i=0; i<nclient; i++)
  {
    if(m_Client[i])
    {
      m_Client[i]->Download();
    }
    else
    {
      printf("CMDaq::Download: m_Client[%d] is not set\n",i);
    }
  }

  clientActive[0] = 1; // set first client as active

  downloaded = 1;
  ended = 0;
}


//_____________________________________________________________________________
Int_t CMDaq::Prestart()
{
//

  Int_t i, status;

  status = -1;

  if(!downloaded)
  {
    printf("CMDaq::Prestart: you have to download first !!!\n");
    return(status);
  }

/////
// piece from here goto Start()
/////

  prestarted = 1;
  ended = 0;

  Reset();

  return(status);
}


//_____________________________________________________________________________
Int_t CMDaq::Pause()
{
//

  Int_t status;

  active = 0;
  status = 0;
  printf("CMDaq::Pause: stop data accumulating\n");

  return(status);
}


//_____________________________________________________________________________
Int_t CMDaq::Resume()
{
//

  Int_t status;

  if(!prestarted)
  {
    printf("CMDaq::Start: you have to prestart first !!!\n");
    return(status);
  }
  active = 1;
  status = 0;
  printf("CMDaq::Resume: start data accumulating\n");

  return(status);
}


//_____________________________________________________________________________
Int_t CMDaq::Start()
{
// start event loop

  Int_t i, status;

  status = -1;

  if(!prestarted)
  {
    printf("CMDaq::Start: you have to prestart first !!!\n");
    return(status);
  }




/////
// start of piece from Prestart()
/////

  if(!strncmp(filename,"/tmp/et_sys_",12) ) // events from ET system
  {
	/*Sergey: return to 20 after MS test !!!!!!!!!!!!!!!!!!*/
    kUPDATE = 1; // 20;
    file = 0;
    status = InitET();
    if(status != 0)
	{
      //printf("CMDaq: forcedclose\n");
      //if(et_forcedclose(id) != ET_OK)
      //{
      //  printf("CMDaq: cannot close ET system\n");
      //  exit(1);
      //}
      
      et_opened = 0;
      sleep(1);
	}
    etSysalloc(&sysptr); // allocate a system area for the etbosio
  }
  else                        // events from BOSIO file
  {
    kUPDATE = 20; // 20;
    et_opened = 0;
    file = 1;
    // BOSIO array initialization and BOSIO file opening
    iw = new Int_t[NBCS];
    status = bosInit(iw,NBCS);
    printf("bosInit: status = %d\n",status);

    // Open BOS file
    status = bosOpen((char *)filename,"r",&desc);
    printf("opening %s : status = %d desc = %x\n",filename,status,desc);
  }

  // prestart all clients (active and not active)

  for(i=0; i<nclient; i++)
  {
    if(m_Client[i])
    {
      m_Client[i]->Prestart(iw);
    }
    else
    {
      printf("CMDaq::Prestart: m_Client[%d] is not set\n",i);
    }
  }

  //NewEvent(17);

/////
// end of piece from Prestart()
/////








  for(i=0; i<nclient; i++)
  {
    if(m_Client[i] && clientActive[i])
    {
	  //printf("uuuuuu1\n");fflush(stdout);
      m_Client[i]->Go(iw);
 	  //printf("uuuuuu2\n");fflush(stdout);
   }
    else if(clientActive[i])
    {
      printf("CMDaq::Start: m_Client[%d] is not set\n",i);
    }
  }

  active = 1;
  status = 0;

  //printf("CMDaq::Start() done ?\n");fflush(stdout);
  //sleep(3);
  //printf("CMDaq::Start() done !\n");fflush(stdout);
  //sleep(10);

  return(status);
}


//_____________________________________________________________________________
void CMDaq::Stop()
{
//
  Int_t i, status;

  for(i=0; i<nclient; i++)
  {
    if(m_Client[i] && clientActive[i])
    {
      m_Client[i]->End(iw);
    }
    else if(clientActive[i])
    {
      printf("CMDaq::Stop: m_Client[%d] is not set\n",i);
    }
  }

  //NewEvent(20);

  if(!strncmp(filename,"/tmp/et_sys_",12) ) // events from ET system
  {
    etSysfree(sysptr); // release a system area for the etbosio

    if(et_opened)
    {
      if(et_station_detach(id, attach) != ET_OK)
      {
        printf("et_client: error in station detach\n");
        //exit(1);
      }
      else
	  {
        et_attached = 0;
	  }

      //if(et_station_remove(id, my_stat) != ET_OK)
      //{
      //  printf("et_client: error in station remove\n");
      //}
      //else
	  //{
      //  et_created = 0;
	  //}

      //if(et_close(id) != ET_OK)
      //{
      //  printf("CMDaq: cannot close ET system\n");
      //}
      //else
	  //{
      //  printf("CMDaq: ET closed\n");
      //  et_opened = 0;
	  //}
    }
  }
  else                        // events from BOSIO file
  {
    ;
  }

  ended = 1;
  active = 0;
  prestarted = 0;

}



//_____________________________________________________________________________
Int_t CMDaq::InitET()
{
//

  Int_t status, val;


  if(!et_opened) // ET INITIALIZATION
  {
bzero(&openconfig,sizeof(et_openconfig));
    et_open_config_init(&openconfig);

	// force remote access even if host is NOT clon00
    // need it to access 'read-only' ET system
    printf("CMDaq: hostname >%s<\n",getenv("HOST"));
    /*if(strncmp("clon00",getenv("HOST"),6))
    {
      et_open_config_setmode(openconfig, ET_HOST_AS_REMOTE);
      if(strncmp("clon",getenv("HOST"),4))
      {
       printf("CMDaq: use remote access to clasonl1\n");
        et_open_config_sethost(openconfig, "clasonl1");
      }
      else
      {
        printf("CMDaq: use remote access to clon00\n");
        et_open_config_sethost(openconfig, "clon00");
      }
    }
    else*/
    {
      printf("CMDaq: use local access\n");
    }

    et_open_config_setwait(openconfig, ET_OPEN_NOWAIT);
    // does not work if: kill ET, wait several seconds(et_open() will
    // attach to old ET file); remove ET file. After that et_open() is
    // waiting on a old ET file which does not exist on disk but still
    // attached to the program
    //et_open_config_setwait(openconfig, ET_OPEN_WAIT);

printf("2 Initialization of ET system !!!\n");
    if(et_open(&id, filename, openconfig) != ET_OK)
    {
printf("3\n");
      et_open_config_destroy(openconfig);
      printf("CMDaq: cannot open ET system\n");
      et_opened = 0;
      return(-1);
    }
    else
    {
printf("4\n");
      et_opened = 1;
    }
    et_open_config_destroy(openconfig);
  }

  // Now that we have access to an ET system, find out how many
  // events it has and what size they are. Then allocate an array
  // of pointers to use for reading, writing, and modifying these events.

  if(et_system_getnumevents(id, &nevents_max) != ET_OK)
  {
    printf("CMDaq: ET has died\n");
    et_opened = 0;
    return(-10);
  }
  printf("CMDaq: nevents_max=%d\n",nevents_max);

D 2
  if(et_system_geteventsize(id, &event_size) != ET_OK)
E 2
I 2
  if(et_system_geteventsize(id, (unsigned int *)&event_size) != ET_OK)
E 2
  {
    printf("CMDaq: ET has died\n");
  }
  printf("CMDaq: event_size=%d bytes\n",event_size);
  
  if( (pe = (et_event **) calloc(nevents_max, sizeof(et_event *))) == NULL)
  {
    printf("CMDaq: cannot allocate memory\n");
  }

  et_station_config_init(&sconfig);
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI);
  //et_station_config_setrestore(sconfig, ET_STATION_RESTORE_IN);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(sconfig, 1);
  
  // old "all" mode (BLOCKING only ???)
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);



// temporary non-blocking (NewEvent does not work !!!)
  // if ET_STATION_BLOCKING
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);
  // if ET_STATION_NONBLOCKING
  //et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING);


  et_station_config_setcue(sconfig, 100);

  // set debug level
  et_system_setdebug(id, ET_DEBUG_INFO);

  if((status = et_station_create(id, &my_stat, et_station_name, sconfig)) < 0)
  {
    if(status == ET_OK)
    {
      printf("CMDaq: et_station_create(): successful\n");
      et_created = 1;
	}
    else if(status == ET_ERROR_EXISTS)
    {
      // my_stat contains pointer to existing station
      printf("CMDaq: et_station_create(): station exists, will attach\n");
    }
    else if(status == ET_ERROR_TOOMANY)
    {
      printf("CMDaq: et_station_create(): too many stations created, maximum ... allowed\n");
      et_opened = 0;
      return(-2);
    }
    else if(status == ET_ERROR_DEAD)
    {
      printf("CMDaq: et_station_create(): ET system is dead\n");
      et_opened = 0;
      return(-3);
    }
    else if(status == ET_ERROR)
    {
      printf("CMDaq: et_station_create(): \n");
      et_opened = 0;
      return(-4);
    }
    else
    {
      printf("CMDaq: et_station_create(): error in station creation, status=%d\n",status);
      et_opened = 0;
      return(-5);
    }
  }
  printf("CMDaq: station ready\n");

/*
  if(et_attached)
  {
    if(et_station_detach(id, attach) != ET_OK)
    {
      printf("CMDaq: error in station detach before attach\n");
      //exit(1);
    }
    et_attached = 0;
  }
*/



  if(et_station_attach(id, my_stat, &attach) < 0)
  {
    printf("et_client: error in station attach\n");
    return(-4);
  }
  et_attached = 1;

  printf("ET constructor done\n");

  return(0);
}

//_____________________________________________________________________________
void CMDaq::SetFile(const char *filnam)
{
  strcpy(filename,filnam);
  printf("CMDaq::SetFile >%s<\n",filename);
}

//_____________________________________________________________________________
void CMDaq::GetFile(char *filnam)
{
  strcpy(filnam,filename);
  printf("CMDaq::GetFile >%s<\n",filnam);
}

//_____________________________________________________________________________
void CMDaq::DstOpen()
{
//
  // force new DST file closing
  printf("CMDaq: force DST file opening\n");
  etNformat(iw,"+DST","I");
  etNcreate(iw,"+DST",17,1,1);

}
//_____________________________________________________________________________
void CMDaq::DstClose()
{
//
  // force new DST file closing
  printf("CMDaq: force DST file closing\n");
  etNformat(iw,"+DST","I");
  etNcreate(iw,"+DST",20,1,1);

}
//_____________________________________________________________________________
Int_t CMDaq::NewEvent(Int_t type)
{
// Creates new event; id - event type (17 - Prestart, 20 - End)

  Int_t status, ret, ind, i, j;
  Int_t *buf;
  Int_t control_0, nevents, chunk, numread;

  struct timespec timeout;
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 500000000; // 50 ms

  ret = 0;

  printf("NewEvent(%d) reached\n",type);

  if(!strncmp(filename,"/tmp/et_sys_",12))       // events from ET system
  {
    if(et_opened == 0)
	{
printf("1\n");
      if(InitET())
      {
        printf("CMDaq: ET still dead, waiting ... (et_opened=%1d)\n",et_opened);
        sleep(1);
        return(ret);
      }
      et_opened = 1;
      printf("CMDaq: ET came back !\n");
    }

    do
    {
      printf("CMDaq: trying to get old event\n");
      status = et_event_get(id, attach, &pevent, ET_TIMED, &timeout);
                                             //  ET_ASYNC, NULL
                                             //  ET_SLEEP, NULL
                                             //  ET_TIMED, &timeout

      printf("CMDaq: timed out so trying to get new event\n");
      if(status == ET_ERROR_TIMEOUT)
      {
        status = et_event_new(id, attach, &pevent, ET_TIMED, &timeout, event_size);
                                               //  ET_ASYNC, NULL
                                               //  ET_SLEEP, NULL
                                               //  ET_TIMED, &timeout
      }

      if (status == ET_OK)
      {
        printf("CMDaq: event(new) type 0x%08x length %d (bytes)\n",
                                         pevent->control[0], pevent->length);
      }
      else if (status == ET_ERROR_BUSY) // ET_ASYNC mode only
      {
        // busy so try again
        printf("CMDaq: Trying to get event from ET but station is busy, try again\n");
        //status = ET_OK;
      }
      else if (status == ET_ERROR_EMPTY) // ET_ASYNC mode only
      {
        // No events
        printf("CMDaq: No events in ET\n");
        //return(0);
      }
      else if (status == ET_ERROR_TIMEOUT) // ET_TIMED mode only
      {
        // timeout
        printf("CMDaq: got timeout\n");
        //return(0);
      }
      else if (status == ET_ERROR_DEAD)
      {
        printf("CMDaq: ET is dead(0)\n");
        if(et_forcedclose(id) != ET_OK)
        {
          printf("CMDaq: cannot close ET system\n");
          exit(1);
        }
        et_opened = 0;
        sleep(1);
        return(ret);
      }
      else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ))
      {
        printf("CMDaq: socket communication error\n");
        return(-2);
      }
      else
      {
        printf("CMDaq: error %d in et_events_get()\n",status);
        return(-3);
      }


    } while(status < ET_OK);


  }
  else if(file)         // events from BOSIO file
  {
    printf("cannot get new event - not ET !!!\n");
    return(-1);
  }
  else
  {
    printf("no one input stream ... Stop loop.\n");
    return(-1);
  }

  //
  // geting events from BOS banks
  //

  if(et_opened)         // events from ET system
  {
    et_event_getdata(pevent, (void **) &iw);
    //printf("iw1=%08x\n",(int)iw);
    etOpen(iw,(event_size/4-10),sysptr);
  }


  // DST file operations
  if(type == 17)
  {
    newdstfile = 0;
    printf("Open DST file\n");
    DstOpen();
  }
  else if(type == 20)
  {
    printf("Close DST file\n");
    DstClose();
  }


  if(et_opened)
  {
    etClose(iw);
  }


done:

  if(et_opened)
  {
    printf("CMDaq: puting new event back to ET\n");
    status = et_event_put(id, attach, pevent);
    if (status == ET_ERROR_DEAD)
    {
      printf("CMDaq: ET is dead(1)\n");
      if(et_forcedclose(id) != ET_OK)
      {
        printf("CMDaq: cannot close ET system\n");
        exit(1);
      }
      et_opened = 0;
      sleep(1);
    }
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ))
    {
      printf("CMDaq: socket communication error\n");
      exit(1);
    }
    else if (status != ET_OK)
    {
      printf("CMDaq: put error\n");
      exit(1);
    }
    //printf("CMDaq: put events back to ET, status=%d\n",status);

//if(et_station_setblock(id, attach, ET_STATION_BLOCKING) != ET_OK) printf("ERROR 11\n");

  }

  return(ret);
}


//_____________________________________________________________________________
Int_t CMDaq::GetEvent()
{
//

  Int_t status, ret, ind, i, j, jj;
  Int_t *buf;
  Int_t control_0, nevents, len, chunk, numread;

  struct timespec timeout;
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 500000000; // 50 ms

  //printf("CMDaq: GetEvent() reached\n");

  ret = 0;
  if(!active) return(ret);
  //printf("CMDaq: GetEvent: active\n");

  if(!strncmp(filename,"/tmp/et_sys_",12))       // events from ET system
  {
    if(et_opened == 0)
	{
printf("1\n");
      if(InitET())
      {
        printf("CMDaq: ET still dead, waiting ... (et_opened=%1d)\n",et_opened);
        sleep(1);
        return(ret);
      }
      et_opened = 1;
      printf("CMDaq: ET came back !\n");
    }

    do
    {
      //chunk = nevents_max;
      chunk = 100;

      status = et_events_get(id, attach, pe, ET_TIMED, &timeout, chunk, &numread);
                                         //  ET_ASYNC, NULL
                                         //  ET_SLEEP, NULL
                                         //  ET_TIMED, &timeout
      if (status == ET_OK)
      {
        ;
        //printf("numread1=%d\n",numread);
        //for(j=0; j<numread; j++)
        //{
		//  if(pe[j]->control[2]!=0)
        //  {
        //    len = pe[j]->length>>2;  /* Length from ET system is in bytes */
        //    printf("CMDaq: event type %d length %d (bytes)\n",
        //                                 pe[j]->control[0], pe[j]->length);
        //  }
        //}
        //printf("\n");
      }
      else if (status == ET_ERROR_BUSY) // ET_ASYNC mode only
      {
        // busy so try again
        printf("CMDaq: Trying to get event from ET but station is busy, try again\n");
        status = ET_OK;
      }
      else if (status == ET_ERROR_EMPTY) // ET_ASYNC mode only
      {
        // No events
        //printf("CMDaq: No events in ET\n");
        return(0);
      }
      else if (status == ET_ERROR_TIMEOUT) // ET_TIMED mode only
      {
        // timeout - no events
        //printf("CMDaq: got timeout\n");
        return(0);
      }
      else if (status == ET_ERROR_DEAD)
      {
        printf("CMDaq: ET is dead(0)\n");
        if(et_forcedclose(id) != ET_OK)
        {
          printf("CMDaq: cannot close ET system\n");
          exit(1);
        }
        et_opened = 0;
        sleep(1);
        return(ret);
      }
      else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ))
      {
        printf("CMDaq: socket communication error\n");
        return(-2);
      }
      else
      {
        printf("CMDaq: error %d in et_events_get()\n",status);
        return(-3);
      }
    } while(status < ET_OK);


  }
  else if(file)         // events from BOSIO file
  {
    nevents_max = numread = 5;
  }
  else
  {
    printf("no one input stream ... Stop loop.\n");
    return(-1);
  }

  //
  // geting events from BOS banks
  //

  printf("numread=%d\n",numread);fflush(stdout);
  for(jj=0; jj<numread; jj++)
  {

    if(et_opened)         // events from ET system
    {
//boy      if(pe[jj]->control[2] == 0) continue; // my 'select' function
      et_event_getdata(pe[jj], (void **) &iw);
      //printf("iw1=%08x\n",(int)iw);fflush(stdout);
      etOpen(iw,(event_size/4-10),sysptr);
      //printf("iw2=%08x\n",(int)iw);fflush(stdout);
    }
    else if(file)         // events from BOSIO file
    {
      etLdrop(iw,"E");
      if((status=bosRead(desc,iw,"E")) != 0)
      {
        ret=-1;
        printf("CMDaq: bosRead() returns %d\n",status);
        goto done;
      }

      if((ind = etNlink(iw,"HEAD",0)) == 0)
      {
        ret=-2;
        printf("CMDaq: no HEAD bank\n");
        goto done;
      }
      buf = (Int_t *) &iw[ind];
    }

    // process one client event

    for(i=0; i<nclient; i++)
    {
      if(m_Client[i] && clientActive[i])
      {
        //printf("CMDaq: process client %d event ...\n",i);fflush(stdout);
        ret = ret | m_Client[i]->Event(iw);
        //printf("CMDaq: process client %d event - done.\n",i);fflush(stdout);
      }
      else if(clientActive[i])
      {
        printf("CMDaq:GetEvent m_Client[%d] is not set\n",i);
      }
    }

    if(et_opened)         // events from ET system
    {
      etClose(iw);
    }

  }

done:

  //printf("CMDaq: GetEvent - done\n");
  if(et_opened)
  {
    //printf("CMDaq: puting %d events back to ET\n",numread);
    status = et_events_put(id, attach, pe, numread);
    if (status == ET_ERROR_DEAD)
    {
      printf("CMDaq: ET is dead(1)\n");
      if(et_forcedclose(id) != ET_OK)
      {
        printf("CMDaq: cannot close ET system\n");
        exit(1);
      }
      et_opened = 0;
      sleep(1);
    }
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ))
    {
      printf("CMDaq: socket communication error\n");
      exit(1);
    }
    else if (status != ET_OK)
    {
      printf("CMDaq: put error\n");
      exit(1);
    }
    //printf("CMDaq: put events back to ET, status=%d\n",status);
  }

  //printf("CMDaq: GetEvent returns %d\n",ret);
  return(ret);
}
 
//_____________________________________________________________________________
void CMDaq::Clear(Option_t *option)
{
//
  printf("CMDaq::Clear: clean everything !!!\n");

  // release BOSIO array memory
  delete iw;

  Stop();

}


//_____________________________________________________________________________
void CMDaq::Reset()
{
// Reset for all clients (active and not active)

  Int_t i;

  for(i=0; i<nclient; i++)
  {
    if(m_Client[i])
    {
      m_Client[i]->Reset();
    }
    else
    {
      printf("CMDaq::Reset(): m_Client[%d] is not set\n",i);
    }
  }

}

//_____________________________________________________________________________
void CMDaq::Loop()
{
// Display histograms and visual scalers

  Int_t i, status;

  status = CMDaq::GetEvent();

  if(status == 1) // got some events
  {
    nev++;
	//printf("nev=%d kUPDATE=%d\n",nev,kUPDATE);
    if(!(nev%kUPDATE))
    {
      for(i=0; i<nclient; i++)
      {
        if(m_Client[i] && clientActive[i])
        {
          m_Client[i]->Update();
        }
        else if(clientActive[i])
        {
          printf("CMDaq::Loop(): m_Client[%d] is not set\n",i);
        }
      }

      //m_Pad->cd();

      //m_Pad->Modified();
      m_Pad->Update(); // huge delay here !!!!!!!!!!!!!!!!!
    }
  }
  else if(status == -1) // ET is dead
  {
    et_opened = 0;
    gCMDisplay->StopLoop();
  }
  else if(status == -2) // socket communication error
  {
    et_opened = 0;
    gCMDisplay->StopLoop();
  }
  else if(status == -3) // error in et_events_get()
  {
    et_opened = 0;
    gCMDisplay->StopLoop();
  }

}











E 1
