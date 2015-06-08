#ifdef Linux

/*****************************************************************************
*   rorc_ddl.c                                                               *
*   DDL related functions for ALICE RORC test                                *
*   softwares of the ALICE-DDL                                               *
*   last update:        22/09/2008                                           *
*   written by: Peter Csato and Ervin Denes                                  *
*                                                                            *  
* history:                                                                   * 
*                                                                            *
* v.5.3.6  02-Nov-2008: use one timeout value in ddlRead/WriteDataBlock()    *
*          22-Sep-2008: separate memory and PCI time-outs.                   *
* v.5.3.1  08-Oct-2007: wait the given timeout to check if the transfer      *
*                          has been finished in ddlWriteDataBlock()          *
* v.5.2.1  11-May-2007: fixing a minor bug in rorcArmDDL()                   *
* v.5.1.7  17-Apr-2007: minor bug in ddlSetSiuLoopBack()                     *
* v.5.1.6  22-Mar-2007: less printout in ddlSendData()                       *
* v.5.1.5  08-Mar-2007: rorcArmDDL() modified                                *
*          22-Jan-2007: wait counters type changed from int to long long     *
*          18-Jan-2007: modifying: ddlPrintStatus()                          *
* v.5.1.4  09-Nov-2006: new routine: ddlPrintStatus()                        *
*          21-Oct-2006: check if month greater than 12 when interprets fw id *
* v.4.4.5  17-Mar-2006: allow RORC revison code 4                            *
*          25-Jan-2006: changing diagnostic prints in ddlSendData()          *
*                                         and in ddlReadDataBlock()          *
* v.4.4.4  23-Nov-2005: checking if data transfer is finished                *
*                                            in ddlWriteDataBlock()          *
* v.4.4.3  02-Sep-2005: checking if DTSTW register is empty                  *
*                                            in ddlWriteDataBlock()          *
*                       checking Rx DMA count in ddlReadDataBlock()          *
*                       checking Tx DMA count in ddlSendData()               *
*                       new version of Set/Clear SiuLoopback                 *
*                       adding PRBS test mode commands                       *
*                       interpreting new PRBS test states                    *
* v.4.3.14 18-Mar-2005: fix time-out bug in ddlReadDataBlock()               *
* v.4.3.12 08-Feb-2005: fix bug in ddlWriteDataBlock()                       * 
* v.4.3.11 20-Jan-2005: do not reset RORC in ddlFindDiuVersion               *
*                       routine in case of INTEG RORC                        *
* v.4.3.5  07-Aug-2004: use usec timeout                                     *
* v.4.2.3  05-Aug 2004: wait 10 ms after SIU reset                           *
*                       in ddlResetSiu (both for RORCs)                      *
*          29-Jun-2004: new event length in FEMU (FEMUSTW)                   *
*          05-Apr-2004: FE time-out bit set in CTSTW                         *
*          26-Mar-2004: ddlSendCommandAndWaitReply modified                  *
*          13-Feb-2004: ddlSendData modified                                 *
* v.4.1    07-Jul-2003: JTAG routines modified                               *
*          06-Jun-2003: JTAG routines added                                  *
*                                                                            *
******************************************************************************/
#define LIB_SOURCE_FILE "rorc_ddl.c"
#define LIB_DESCRIPTION "RORC-DDL test library"

#include <unistd.h>
#include "rorc_ddl.h"
char ddlIdent[]="@(#)""" LIB_SOURCE_FILE """: """ LIB_DESCRIPTION \
     """ """ RORC_LIBRARY_VERSION """ compiled """ __DATE__ """ """ __TIME__;

#ifdef DEBUG
  extern FILE *logFile;
#endif

/* for remote SIU/DIU status */
char* remoteStatus[] = {"Power On Reset", "Offline", "Online", "Waiting for PO",
        "Offline No Signal", "Offline LOS", "No Optical Signal", "undefined"};

/* for ordered sets (old DDL card) */
char* receivedOrderedSet[] = {"SRST", "Not_Op", "Oper", "L_Init",
            "Idle", "Xoff", "Xon", "data or delimiter", "unknown ordered set"};


/***************************************************************/
int ddlFindDiuVersion(rorc_pci_dev_t *dev)
{
  stword_t stw[DDL_MAX_REPLY];
  int  n_reply, ret;
  long long longret;
  long long int TimeOut;

  if (dev->rorc_revision >= INTEG)
  {
    // ide johetne DIU status lekerdezes
    dev->diu_version = EMBEDDED;
    return (RORC_STATUS_OK);
  }

  rorcReset(dev, 0);   // full reset
  dev->diu_version = NO_DIU;
  
  /* read DIU's HW id */
  TimeOut = DDL_RESPONSE_TIME * dev->pci_loop_per_usec;
  ret = ddlSendCommand(dev, DIU, RHWVER, 0, 0, TimeOut);
  if (ret)
    return (ret);

  n_reply = 0;
  while (n_reply < DDL_MAX_REPLY)
  {
    longret = ddlWaitStatus(dev, TimeOut);
    if (longret >= TimeOut)
      break;
    stw[n_reply++] = ddlReadStatus(dev);
  }

  if (n_reply == 0)
  {
    debug("No reply arrived for HW status request\n");
    return (RORC_STATUS_OK);
  }
  debug("HW status word = 0x%08lx.\n", stw[0].stw);

  dev->diu_version = NEW;
  if (n_reply != 2)
  {
    dev->diu_version = OLD;
  }

  return (RORC_STATUS_OK);
}

/***************************************************************/
unsigned long ddlLinkUp_NEW(rorc_pci_dev_t *prorc, int master, int print, 
                    int stop, long long int time)

/* ddlLinkUp tries to reset and initialize (if master) the DDL link.
 * Parameter:  master   if !=0 then tries to initialize the link,
 *                      otherwise only reset the link
 *             print	# of link status prints (-1 means: no limit)
 *             stop	if !=0 returns when active state or print limit
 *                      (if print > 0) is reached
 *             time	# of cycles to wait for command sending and replies
 * Returns:    DIU status word or -1 if no status word can be read
 *             The DIU status word reflects the status of the link after
 *             resetting it. 
 *             Important bits of the status word are
 *             bit 31	must be 0, otherwise one of the error bits is set
 *             if bits 17,16,15 are 1 1 0 the remote SIU or DIU  does not see us 
 *             if bits 14,13,12 are 1 1 0 it means DIU is in power off state,
 *                                  1 0 0 means DIU receives no signal,
 *                            or    0 1 0 which means the link is active.
 */

{

#define PRINTEND \
      if (print > 0) \
      { \
        print--; \
        if (print == 0) \
        { \
          date_log(" Too many messages"); \
          if (stop) return(retlong); \
        } \
      } \
    }

#define PRINT_IF_NEW(a) \
    if (print) \
    { \
      date_log(a); \
      date_log(" remote SIU/DIU in %s state ", remoteStatus[siuStatus]); \
    PRINTEND

  int retval, siuStatus, transid;
  unsigned long retlong, last_stw;
  stword_t stw;
  char pref[13] = "DIU status: ";
  char suff[2] = "\n";

//  date_log ("ddlLinkUp called");
  last_stw = 0xf00; // Not possible value
  transid = 0xf;
  do
  {
    transid = incr15(transid);
    retlong = ddlReadDiu(prorc, transid, time);
    if (retlong == -1)
    {
      if (last_stw != retlong) 
      {
        if (print)
        {
          date_log("-ddlReadDiu returns -1: Error in ddlReadDiu ");
        PRINTEND
        last_stw = retlong;
      }
      continue;
    }

    retlong &= STMASK;
    if (last_stw != retlong)
    {
      if (print)
      {
        date_log("-pRORC%d. DIU status word: 0x%08lx ",
                        prorc->minor, retlong);
      PRINTEND
      last_stw = retlong;
    }
    else
     continue;

    siuStatus = (retlong & REMMASK) >> 15;

    if (retlong & ERROR_BIT)
    {
      if (print)
      {
        date_log(" DIU error at pRORC /dev/prorc %d/%d. Status word: 0x%0lx ",
                        prorc->minor, prorc->ddl_channel, retlong);
        statInfo[0] = '\0';
        ddlInterpretIFSTW(prorc, retlong, pref, suff);
        date_log("%s", statInfo);
      PRINTEND
      continue;
    }

    switch (retlong & DIUSTMASK)
    {
      case DIU_WAIT:
        PRINT_IF_NEW(" DIU port in Waiting for Power Off state"); break;
      case DIU_LOS:
        PRINT_IF_NEW(" DIU port in Offline Loss of Synchr. state"); break;
      case DIU_NOSIG:
        PRINT_IF_NEW(" DIU port in Offline No Signal state"); break;
      case DIU_TSTM:
        PRINT_IF_NEW(" DIU in PRBS Test Mode state"); break;
      case DIU_OFFL:
        PRINT_IF_NEW(" DIU port in Offline state"); break;
      case DIU_POR:
        PRINT_IF_NEW(" DIU port in Power On Reset state"); break;
      case DIU_POFF:
        PRINT_IF_NEW(" DIU port in Power Off state");
        if (master)
        {
          // Send a command to DIU to wake up
          retval=ddlSendCommand(prorc, DIU, WAKEUP, transid, 0, time);
          transid = incr15(transid);

          // Read status FIFO 
          stw = ddlReadStatus(prorc);
          debug("The WAKEUP returned status: %8lx\n", stw.stw);
          continue;
        }
        // not master 
        // if (stop) return (retlong); 
        break;
      case DIU_ONL:
        PRINT_IF_NEW(" DIU port in Online state");
        if (stop) return (retlong);
        break;
    }
  } while (1);

  return (retlong);

}

/***************************************************************/
unsigned long ddlLinkUp_OLD(rorc_pci_dev_t *prorc, int master, int print, 
                           int stop, long long int time)

/* ddlLinkUp tries to reset and initialize (if master) the DDL link.
 * Parameter:  master   if !=0 then tries to initialize the link,
 *                      otherwise only reset the link
 *             print    # of link status prints (-1 means: no limit)
 *             stop     if !=0 returns when active state or print limit
 *                      (if print > 0) is reached
 *             time     # of cycles to wait for command sending and replies
 * Returns:    DIU status word or -1 if no status word can be read
 *             The DIU status word reflects the status of the link after
 *             resetting it. 
 *             Important bits of the status word are
 *             bit 31   must be 0, otherwise one of the error bits is set
 *             bit 29   must be 0, otherwise there is no signal on the line
 *             if bits 17,16,15 are 1 1 0 the other end is in failure state
 *             if bits 14,13,12 are 1 0 0 it means DIU is in off_line state
 *                            or    0 0 0 which means the link is active.
 */

{
  int retval, siuSend, transid;
  unsigned long retlong, last_stw, siuStatus;
  stword_t stw;
  char pref[13] = "DIU status: ";
  char suff[2] = "\n";

  last_stw = 0xf00; // Not possible value
  transid = 0xf;
  do
  {
    transid = incr15(transid);
    retlong = ddlReadDiu(prorc, transid, time);
    if (retlong == -1)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" ddlReadDiu returns -1: Error in ddlReadDiu");
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return (retlong);
            }
          }
        }
        if (stop)
          return (retlong);
        last_stw = retlong;
      }
      continue;
    }

    retlong &= STMASK;
    siuStatus = retlong & REMMASK;
    switch (siuStatus)
    {
      case oSIU_SRST :
        siuSend = 0;
        break;
      case oSIU_FAIL :
        siuSend = 1;
        break;
      case oSIU_OFFL :
        siuSend = 2;
        break;
      case oSIU_LINIT :
        siuSend = 3;
        break;
      case oSIU_ACT :
        siuSend = 4;
        break;
      case oSIU_XOFF :
        siuSend = 5;
        break;
      case oSIU_XON :
        siuSend = 6;
        break;
      case oSIU_ELSE :
        siuSend = 7;
        break;
      default :
        siuSend = 8;
    }

    if (retlong & ERROR_BIT)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU error at pRORC /dev/prorc %d/%d. Status word: 0x%0lx",
                          prorc->minor, prorc->ddl_channel, retlong);
          statInfo[0] = '\0';
          ddlInterpretIFSTW(prorc, retlong, pref, suff);
          date_log("%s", statInfo);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    retlong = mask(retlong, ~DIUERMASK);
    }

    if (retlong & oLOSS_SIGN)
    {
      if (!(last_stw & oLOSS_SIGN))
      {
        if (print)
        {
          date_log(" Loss of signal. Status word: 0x%0lx", retlong);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_NOSYNC)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" Receiver not synchronised. Status word: 0x%0lx", retlong);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_RSTSIU)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU in reset SIU state. Status word: 0x%0lx", retlong);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_FAIL)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU port in fail state, receiving %s",
                   receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }

      // Send reset command to DIU 
      retval = ddlSendCommand(prorc, DIU, LRST, transid ,0, time);
      transid = incr15(transid);
      if (retval == RORC_TIMEOUT)
        if (print)
        {
          date_log("Error: timeout is over for LRST");
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }

      // Read status FIFO 
      stw = ddlReadStatus(prorc);
      debug("The LRST returned status: %8lx\n", stw.stw);
    }
    else if ((retlong & DIUSTMASK) == oDIU_ACCED)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU port in metastable ACCED state, receiving %s",
                   receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_START)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU port in metastable START state, receiving %s",
                   receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_LRES)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" DIU port in metastable LRES state, receiving %s",
                   receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
    }
    else if ((retlong & DIUSTMASK) == oDIU_OFFL)
   {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" pRORC /dev/prorc %d/%d, is Off Line, receiving %s",
                 prorc->minor, prorc->ddl_channel, receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
      }

     if ((retlong & REMMASK) == oSIU_FAIL)
     {
        if (last_stw != retlong)
        {
          if (print)
          {
            date_log(" The remote DIU or SIU is in fail state");
            if (print > 0)
            {
              print--;
              if (print == 0)
              {
                date_log(" Too many messages");
                if (stop) return(retlong);
              }
            }
          }
        }
        last_stw = retlong;
      }
      else if (master)
      {
        last_stw = retlong;
        // Send a command to DIU to initialize 
        retval=ddlSendCommand(prorc, DIU, LINIT, transid, 0, time);
        transid = incr15(transid);

        if (retval == RORC_TIMEOUT)
          if (print)
          {
            date_log("Error: timeout is over for LINIT");
            if (print > 0)
            {
              print--;
              if (print == 0)
              {
                date_log(" Too many messages");
                if (stop) return(retlong);
              }
            }
          }

        // Read status FIFO 
        stw = ddlReadStatus(prorc);
        debug("The LINIT returned status: %8lx\n", stw.stw);
        continue;
      }
      // not master 
      last_stw = retlong;

      // if (stop) return (retlong); 
    }
    else if ((retlong & DIUMASK) == oLINK_ACT)
    {
      if (last_stw != retlong)
      {
        if (print)
        {
          date_log(" pRORC /dev/prorc %d/%d Active, receiving %s",
                prorc->minor, prorc->ddl_channel, receivedOrderedSet[siuSend]);
          if (print > 0)
          {
            print--;
            if (print == 0)
            {
              date_log(" Too many messages");
              if (stop) return(retlong);
            }
          }
        }
        last_stw = retlong;
      }
      if (stop)
        return (retlong);
    }
  } while (1);

  return (retlong);

}

/***************************************************************/
unsigned long ddlLinkUp(rorc_pci_dev_t *prorc, int master, int print, int stop,
                                                        long long int time)

{
  unsigned long retlong;

  if (prorc->diu_version == OLD)
    retlong = ddlLinkUp_OLD(prorc, master, print, stop, time);
  else
    retlong = ddlLinkUp_NEW(prorc, master, print, stop, time);

  return (retlong);
}


/***************************************************************/
unsigned long ddlResetSiu(rorc_pci_dev_t *prorc, int print, int cycle, 
                                                          long long int time)

/* ddlResetSiu tries to reset the SIU.
 *             print	# if != 0 then print link status
 *             cycle	# of status checks
 *             time	# of cycles to wait for command sending and replies
 * Returns:    SIU status word or -1 if no status word can be read
 */

{
  int i, retval, transid;
  long long longret;
  int diu_ok, siu_ok, trial;
  unsigned long retlong;
  stword_t stw;
  char* pref="";
  char* suff="\n";

  retval = ddlSendCommand(prorc, DIU, SRST, 0,  0, time);
  if (retval == RORC_LINK_NOT_ON)
  {
    if (print)
      date_log("SIU reset can not be sent because the link is not on");
    return (-1);
  }
  else if (retval == RORC_TIMEOUT)
  {
    if (print)
      date_log("SIU reset can not be sent out in timeout %lld", time);
    return (-1);
  }
  else
  {
    longret = ddlWaitStatus(prorc, time);
    if (longret >= time)
    {
      if (print)
        date_log("SIU reset: No reply arrived in timeout %lld", time);
      return (-1);
    }
    else
    {
      stw = ddlReadStatus(prorc);
      if (print)
        date_log("SIU reset: reply = 0x%08lx", stw.stw);
    }
  }

  if ((prorc->diu_version != NEW) && (prorc->diu_version != EMBEDDED))
    return (-0);

  trial = cycle;
  transid = 0xf;
  retlong = -1;
  i = 0;

  while (trial)
  {
    usleep(10000);   // sleep 10 msec

    i++;
    if (print)
      date_log("Cycle #%d:", i);

    diu_ok = 0;
    siu_ok = 0;

    transid = incr15(transid);
    retlong = ddlReadDiu(prorc, transid, time);
    if (retlong == -1)
    {
      if (print)
      {
        date_log(" ddlReadDiu returns -1: Error in ddlReadDiu ");
      }
      trial--; 
      continue;
    }

    retlong &= STMASK;
    if (print)
    {
      date_log(" RORC %d/%d. DIU status word: 0x%08lx ",
                      prorc->minor, prorc->ddl_channel, retlong);
    }

    if (retlong & ERROR_BIT)
    {
      if (print)
      {
        date_log(" DIU error at RORC /dev/prorc %d/%d. Status word: 0x%0lx ",
                        prorc->minor, prorc->ddl_channel, retlong);
        statInfo[0] = '\0';
        ddlInterpretIFSTW(prorc, retlong, pref, suff);
        date_log("%s", statInfo);
      }
    }
    else
      diu_ok = 1;

    transid = incr15(transid);
    retlong = ddlReadSiu(prorc, transid, time);
    if (retlong == -1)
    {
      if (print)
      {
        date_log(" ddlReadSiu returns -1: Error in ddlReadSiu ");
      }
      trial--;
      continue;
    }

    retlong &= STMASK;
    if (print)
    {
      date_log(" RORC %d/%d. SIU status word: 0x%08lx ",
                      prorc->minor, prorc->ddl_channel, retlong);
    }

    if (retlong & ERROR_BIT)
    {
      if (print)
      {
        date_log(" SIU error at RORC /dev/prorc %d/%d. Status word: 0x%0lx ",
                        prorc->minor, prorc->ddl_channel, retlong);
        statInfo[0] = '\0';
        ddlInterpretIFSTW(prorc, retlong, pref, suff);
        date_log("%s", statInfo);
      }
    }
    else
      siu_ok = 1;

    if (diu_ok && siu_ok)
      return (retlong);

    trial--;
  }

  if (print)
    date_log(" Too many trials");
  return(retlong);

}

/***************************************************************/
void ddlInterpretStw(stword_t stw)

{
   if (stw.part.dest == RORC)
   switch (stw.part.code)
   {
     case 2 :          printf("pRORC status"); break;
     case 3 :          printf("pRORC Id"); break;
     case 4 : 
     case 5 :          printf("Free FIFO values"); break;
     case 12:          printf("pRORC counters");
                       if (stw.stw & 0xffffff00)
                         printf(" (%ld)", (stw.stw >> 8)); 
                       break;
     default :         printf("Unknown pRORC status word"); break;
   }
  else if ((stw.part.dest == DIU) || (stw.part.dest == SIU)) 
    switch (stw.part.code)
    {
      case CTSTW    :   printf("CTSTW"); break; 
      case ILCMD    :   printf("CTSTW for illegal command"); break;
      case HWSTW    :   printf("HWSTW"); break;
      case PMVAL    :   printf("Power Monitor Value"); break;
      case DTSTW    :   printf("DTSTW"); break;
      case DTSTW_TO :   printf("DTSTW with FE Time-out"); break;
      case IFSTW    :   printf("IFSTW"); break;
      case FWSTW    :   printf("FWSTW"); break;
      case TEVAL    :   printf("PRBS test error counter"); break;
      default       :   printf("Unknown DDL status word"); break;
    }
  else if (stw.part.dest == FEE)
    switch (stw.part.code)
    {
      case CTSTW    :   printf("CTSTW"); break; 
      case CTSTW_TO :   printf("CTSTW with FE Time-out"); break;
      case ILCMD    :   printf("CTSTW for illegal command"); break;
      case FESTW    :   printf("FESTW"); break;
      default       :   printf("Unknown FEE status word"); break;
    }
  else if (stw.part.dest == JTAG)
    switch (stw.part.code)
    {
      case CTSTW    :   printf("CTSTW"); break; 
      case ILCMD    :   printf("CTSTW for illegal command"); break;
      case JTSTW    :   printf("JTSTW");  break;
      case JSTART   :   printf("JSTR") ;  break;
      default       :   printf("Unknown JTAG status word"); break;
    }
  else
    printf("Unknown status word\n");
}

/***************************************************************/
void ddlInterpret_NEW_IFSTW(__u32 ifstw, char *pref, char *suff)
/*
 * Interpret DIU or SIU IFSTW
 */ 

{
  int destination;
  int siuStatus;
  unsigned long status;

  destination = ST_DEST(ifstw);

  status = ifstw & STMASK;
  if (destination == DIU)
  {
    if (mask(status, DIU_LOOP))
      stat_log("%sDIU is set in loop-back mode%s", pref, suff);
    if (mask(status, ERROR_BIT))
    {
      if (strlen(pref) == 0)
        stat_log("%sDIU error bit(s) set:%s", pref, suff);
      if (mask(status, LOSS_SYNC))
        stat_log("%s Loss of synchronization%s", pref, suff);
      if (mask(status, D_TXOF))
        stat_log("%s Transmit data/status overflow%s", pref, suff);
      if (mask(status, D_RES1))
        stat_log("%s Undefined DIU error%s", pref, suff);
      if (mask(status, D_OSINFR))
        stat_log("%s Ordered set in frame%s", pref, suff);
      if (mask(status, D_INVRX))
        stat_log("%s Invalid receive character in frame%s", pref, suff);
      if (mask(status, D_CERR))
        stat_log("%s CRC error%s", pref, suff);
      if (mask(status, D_RES2))
        stat_log("%s Undefined DIU error%s", pref, suff);
      if (mask(status, D_DOUT))
        stat_log("%s Data out of frame%s", pref, suff);
      if (mask(status, D_IFDL))
        stat_log("%s Illegal frame delimiter%s", pref, suff);
      if (mask(status, D_LONG))
        stat_log("%s Too long frame%s", pref, suff);
      if (mask(status, D_RXOF))
        stat_log("%s Received data/status overflow%s", pref, suff);
      if (mask(status, D_FRERR))
        stat_log("%s Error in receive frame%s", pref, suff);
    }

    switch (mask(status, DIUSTMASK))
    {
      case DIU_TSTM:
        stat_log("%sDIU port in PRBS Test Mode state%s", pref, suff); break;
      case DIU_POFF:
        stat_log("%sDIU port in Power Off state%s", pref, suff); break;
      case DIU_LOS:
        stat_log("%sDIU port in Offline Loss of Synchr. state%s", pref, suff); break;
      case DIU_NOSIG:
        stat_log("%sDIU port in Offline No Signal state%s", pref, suff); break;
      case DIU_WAIT:
        stat_log("%sDIU port in Waiting for Power Off state%s", pref, suff); break;
      case DIU_ONL:
        stat_log("%sDIU port in Online state%s", pref, suff); break;
      case DIU_OFFL:
        stat_log("%sDIU port in Offline state%s", pref, suff); break;
      case DIU_POR:
        stat_log("%sDIU port in Power On Reset state%s", pref, suff); break;
    }

    siuStatus = (status & REMMASK) >> 15;
    stat_log("%sremote SIU/DIU port in %s state%s", pref, remoteStatus[siuStatus], suff);
  }
  else  /* SIU */
  {
    if (mask(status, ERROR_BIT))
    {
      if (strlen(pref) == 0)
        stat_log("%sSIU error bit(s) set:%s", pref, suff);
      if (mask(status, S_LONGE))
        stat_log("%s Too long event or read data block%s", pref, suff);
      if (mask(status, S_IFEDS))
        stat_log("%s Illegal FEE data/status%s", pref, suff);
      if (mask(status, S_TXOF))
        stat_log("%s Transmit FIFO overflow%s", pref, suff);
      if (mask(status, S_IWDAT))
        stat_log("%s Illegal write data word%s", pref, suff);
      if (mask(status, S_OSINFR))
        stat_log("%s Ordered set in frame%s", pref, suff);
      if (mask(status, S_INVRX))
        stat_log("%s Invalid character in receive frame%s", pref, suff);
      if (mask(status, S_CERR))
        stat_log("%s CRC error%s", pref, suff);
      if (mask(status, S_DJLERR))
        stat_log("%s DTCC or JTCC error%s", pref, suff);
      if (mask(status, S_DOUT))
        stat_log("%s Data out of receive frame%s", pref, suff);
      if (mask(status, S_IFDL))
        stat_log("%s Illegal frame delimiter%s", pref, suff);
      if (mask(status, S_LONG))
        stat_log("%s Too long receive frame%s", pref, suff);
      if (mask(status, S_RXOF))
        stat_log("%s Receive FIFO overflow%s", pref, suff);
      if (mask(status, S_FRERR))
        stat_log("%s Error in receive frame%s", pref, suff);
      if (mask(status, S_LPERR))
        stat_log("%s Link protocol error%s", pref, suff);
    }
    else
      stat_log("%sSIU error bit not set%s", pref, suff);

    if (mask(status, S_LBMOD))
      stat_log("%sSIU in Loopback Mode%s", pref, suff);
    if (mask(status, S_OPTRAN))
      stat_log("%sOne FEE transaction is open%s", pref, suff);

    if      (mask(status, SIUSTMASK) == SIU_RESERV) {
      stat_log("%sSIU port in undefined state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_POFF) {
      stat_log("%sSIU port in Power Off state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_LOS) {
      stat_log("%sSIU port in Offline Loss of Synchr. state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_NOSIG) {
      stat_log("%sSIU port in Offline No Signal state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_WAIT ) {
      stat_log("%sSIU port in Waiting for Power Off state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_ONL ) {
      stat_log("%sSIU port in Online state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_OFFL) {
      stat_log("%sSIU port in Offline state%s", pref, suff);
    } else if (mask(status, SIUSTMASK) == SIU_POR) {
      stat_log("%sSIU port in Power On Reset state%s", pref, suff);
    }
  }
}

/***************************************************************/
void ddlInterpret_OLD_IFSTW(__u32 ifstw, char* pref, char* suff)
/*
 * Interpret DIU or SIU IFSTW
 */

{
  int siuSend;
  int destination;
  unsigned long status, siuStatus;

  destination = ST_DEST(ifstw);
  status = ifstw & STMASK;
  if (destination == DIU)
  {
    if (mask(status, oDIU_LOOP))
       stat_log("%sDIU is set in loop-back mode%s", pref, suff);
    if (mask(status, ERROR_BIT))
    {
      if (strlen(pref) == 0)
        stat_log("%sDIU error bit(s) set:%s", pref, suff);
      if (mask(status, oLOSS_SIGN))
         stat_log("%s Loss of signal%s", pref, suff);
      else
      {
        if (mask(status, oD_RTOUT))
          stat_log("%s Receiver synchronisation timeout%s", pref, suff);
        if (mask(status, oD_LOSY))
          stat_log("%s Loss of word synchronisation%s", pref, suff);
        if (mask(status, oD_RDERR))
          stat_log("%s Running disparity error%s", pref, suff);
        if (mask(status, oD_INVRX))
          stat_log("%s Invalid receive word%s", pref, suff);
        if (mask(status, oD_CERR))
          stat_log("%s CRC error%s", pref, suff);
        if (mask(status, oD_UNREC))
          stat_log("%s Unrecognised ordered set received%s", pref, suff);
        if (mask(status, oD_DOUT))
          stat_log("%s Data word out of frame%s", pref, suff);
        if (mask(status, oD_IFDL))
          stat_log("%s Illegal frame delimiter%s", pref, suff);
        if (mask(status, oD_LONG))
          stat_log("%s Too long frame%s", pref, suff);
        if (mask(status, oD_RXOV))
          stat_log("%s Received data/status overflow%s", pref, suff);
        if (mask(status, oD_LTOUT))
          stat_log("%s Link initialisation timeout%s", pref, suff);
      }
    }

    if      (mask(status, DIUSTMASK) == oDIU_NOSYNC)
      stat_log("%s DIU port not synchronised%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_RSTSIU)
      stat_log("%s DIU port in reset SIU state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_FAIL)
      stat_log("%s DIU port in fail state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_ACCED)
      stat_log("%s DIU port in metastable ACCED state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_START)
      stat_log("%s DIU port in metastable START state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_LRES)
      stat_log("%s DIU port in metastable LRES state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oDIU_OFFL)
      stat_log("%s DIU port in Off Line state%s", pref, suff);
    else if (mask(status, DIUSTMASK) == oLINK_ACT)
      stat_log("%s DIU port is in Active state%s", pref, suff);

    siuStatus = status & REMMASK;
    switch (siuStatus)
    {
      case oSIU_SRST :
        siuSend = 0;
        break;
      case oSIU_FAIL :
        siuSend = 1;
        break;
      case oSIU_OFFL :
        siuSend = 2;
        break;
      case oSIU_LINIT :
        siuSend = 3;
        break;
      case oSIU_ACT :
        siuSend = 4;
        break;
      case oSIU_XOFF :
        siuSend = 5;
        break;
      case oSIU_XON :
        siuSend = 6;
        break;
      case oSIU_ELSE :
        siuSend = 7;
        break;
      default :
        siuSend = 8;
    }
    stat_log("%sDIU port receives %s%s", pref, receivedOrderedSet[siuSend], suff);
  }
  else  /* SIU */
  {
    if (mask(status, ERROR_BIT))
    {
      if (strlen(pref) == 0)
        stat_log("%sSIU error bit(s) set:%s", pref, suff);
      if (mask(status, oS_LONGE))
        stat_log("%s Too long event or read data block%s", pref, suff);
      if (mask(status, oS_IFEDS))
        stat_log("%s Illegal FEE data/status%s", pref, suff);
      if (mask(status, oS_TXOF))
        stat_log("%s Transmit FIFO overflow%s", pref, suff);
      if (mask(status, oS_IWDAT))
        stat_log("%s Illegal write data word%s", pref, suff);
      if (mask(status, oS_WBLER))
        stat_log("%s Write data block length error%s", pref, suff);
      if (mask(status, oS_RXOV))
        stat_log("%s Receive FIFO overflow%s", pref, suff);
      if (mask(status, oS_LONGD))
        stat_log("%s Too long data frame%s", pref, suff);
      if (mask(status, oS_LONGC))
        stat_log("%s Too long command frame%s", pref, suff);
      if (mask(status, oS_OSIN))
        stat_log("%s Ordered set inside a frame%s", pref, suff);
      if (mask(status, oS_DOUT))
        stat_log("%s Data out of receive frame%s", pref, suff);
      if (mask(status, oS_LPERR))
        stat_log("%s Link protocol error%s", pref, suff);
      if (mask(status, oS_CHERR))
        stat_log("%s Check summ error in receive frame%s", pref, suff);
      if (mask(status, oS_UNREC))
        stat_log("%s Unrecognised ordered set%s", pref, suff);
      if (mask(status, oS_INVRX))
        stat_log("%s Invalid receive word%s", pref, suff);
      if (mask(status, oS_WALER))
        stat_log("%s Word alignment error%s", pref, suff);
      if (mask(status, oS_ISPCH))
        stat_log("%s Illegal special character%s", pref, suff);
      if (mask(status, oS_RDERR))
        stat_log("%s Running disparity error%s", pref, suff);
      if (mask(status, oS_IRXCD))
        stat_log("%s Illegal receive code%s", pref, suff);
      if (mask(status, oS_BUFER))
        stat_log("%s Elastic buffer over/under run%s", pref, suff);
    }
    else
      stat_log("%sSIU error bit not set, SIU is in normal state%s", pref, suff);
  }
}

/***************************************************************/
void ddlInterpretIFSTW(rorc_pci_dev_t *prorc, __u32 ifstw, char* pref, 
                                                           char* suff)
{
if (prorc->diu_version == OLD)
  ddlInterpret_OLD_IFSTW(ifstw, pref, suff);
else
  ddlInterpret_NEW_IFSTW(ifstw, pref, suff);
}


/***************************************************************/
void ddlInterpretHW(rorcHwSerial_t hw, int destination)

/*
 * Interpret DIU or SIU hardware version status
 */

{

  if (destination == DIU)
    printf("DIU HW id: %s\n", hw.data);
  if (destination == SIU)
    printf("SIU HW id: %s\n", hw.data);

  if (strncmp(hw.data, "DDL card", 8) != 0)
    printf("Hardware id not given\n");
  else
    printf("Hardware version: %d.%d, serial: %d\n",
            hw.version, hw.subversion, hw.serial);
}

/***************************************************************/
void ddlInterpretFW(unsigned long fw) 

/*
 * Interpret DIU or SIU firmeware version status  
 */ 

{
  char* monthName[] = {"unknown month", "January", "February", "March",  
                       "April", "May", "June", "July", "August", 
                       "September", "October", "November", "December",
                       "unknown month", "unknown month", "unknown month"};
  int destination;

  destination = ST_DEST(fw);

  if (destination == DIU)
    printf("DIU FW status word: 0x%08lx\n", fw);
  if (destination == SIU)
    printf("SIU FW status word: 0x%08lx\n", fw);

  printf("Firmware version: %d, release date : %s %d 20%02d\n",
          FW_VERS(fw), monthName[FW_MONTH(fw)], FW_DAY(fw), FW_YEAR(fw));
}

/***************************************************************/
void interpretFESTW(stword_t stw)

/*
 * Interpret FEE status word
 */

{
  printf("FEE status = 0x%lx\n", stw.stw);

}

/***************************************************************/
void interpretFEMUSTW(unsigned address, stword_t stw)

/*
 * Interpret FEMU status word
 */
 
{
  double len;
  int len_code;
 
  switch (address)
  {
    case 0:
      printf("PATGEN status = 0x%x, i.e. ", stw.part.param);
      switch (stw.part.param)
      {
        case 0 :
          printf("FEMU Pattern Generator is not set.");
          break;
        case 1 :
          printf("FEMU internal Pattern Generator is disabled, use an external Pattern Generator.");
          break;
        case 2 :
          // printf("FEMU generates alterning chess patterns.") ;
          printf("FEMU generates alterning bit patterns.");
          break;
        case 3 :
          printf("FEMU generates flying '0' (11111110, 11111101, etc.)") ;
          break;
        case 4 :
          printf("FEMU generates flying '1' (00000001, 00000010, etc.)") ;
          break;
        case 5 :
          printf("FEMU generates incremental values, starting from 0.") ;
          break;
        case 6 :
          printf("FEMU generates decremental values, starting from 0.") ;
          break;
        default:
          printf("FEMU Pattern Generator set to invalid value.");
          break;
      }
      break;
    case 1 :
      printf("EVLEN  status = 0x%x, i.e. ", stw.part.param);
      len_code = (stw.part.param & 0x7f);
      if (len_code == 0)
        printf("FEMU event length is not set.");
      else if (len_code > 0x19)
        printf("FEMU event length is set to invalid value.");
      else
      {
        len = 8 << len_code;
        printf("FEMU event length is set to %sword.", border(len, 0));  
        if (stw.part.param & 0x80)
          printf(" Random length!");
      }
      break;
    case 2 :
      printf("TRIG   status = 0x%x, i.e. ", stw.part.param);
      switch (stw.part.param)
      {
        case 0 :
          printf("FEMU trigger mode is not set.");
          break;
        case 1 :
          printf("FEMU trigger mode is set to external pushbutton.");
          break;
        case 2 :
          printf("FEMU trigger mode is set to external trigger signal.");
          break;
        case 3 :
          printf("FEMU trigger is set to 16 word clks between events.") ;
          break;
        case 4 :
          printf("FEMU trigger is set to 128 word clks between events.");
          break;
        case 5 :
          printf("FEMU trigger is set to work periodically every 10 ms.");
          break;
        case 6 :
          printf("FEMU trigger is set to work periodically every 100 ms.");
          break;
        default:
          printf("FEMU trigger mode is set to invalid value.");
          break;
      }
      break;
    case 3 :
      printf("FLCTRL status = 0x%x, i.e. ", stw.part.param);
      switch (stw.part.param)
      {
        case 0 :
          printf("FEMU flow control is not set.");
          break;
        case 1 :
          printf("FEMU flow ctrl is activated after each received word.");
          break;
        case 2 :
          printf("FEMU flow ctrl is activated after each 128 rx word.");
          break;
        case 3 :
          printf("FEMU flow ctrl is activated after each 16K rx word.");
          break;
        default:
          printf("FEMU flow control is set to invalid value.");
          break;
      }
      break;
    case 4 :
      printf("TRDIS  status = 0x%x, i.e. ", stw.part.param);
      switch (stw.part.param)
      {
        case 0 :
          printf("FEMU transfer deactivation is not set.");
          break;
        case 1 :
          printf("FEMU transfer is deactivated after each tx word.");
          break;
        case 2 :
          printf("FEMU tx is deactivated after transmitting 128 word.");
          break;
        case 3 :
          printf("FEMU tx is deactivated after transmitting 16 kword.");
          break;
        default:
          printf("FEMU transfer deactivation is set to invalid value.");
          break;
      }
      break;
    case 5 :
      printf("SEED   status = 0x%x", stw.part.param);
    }
  printf("\n");
}


/***************************************************************/
int ddlSendCommandAndWaitReply(rorc_pci_dev_t *dev,
		               __u32           fee_command,
                               __u32           fee_address,
                               long long int   timeout,
                               stword_t        *stw,
                               int             expected,
			       int             *n_reply)

{
  int ret;
  long long longret;

  *n_reply = 0;
  if (rorcCheckStatus(dev))
  {
    stw[(*n_reply)++] = ddlReadStatus(dev);
    return (RORC_TOO_MANY_REPLY);
  }

  ret = ddlSendCommand(dev, FEE, fee_command, 0, fee_address, timeout);
  if (ret)
    return (ret);

  while (*n_reply < expected)
  {
    longret = ddlWaitStatus(dev, timeout);
    if (longret >= timeout)
      break;
    stw[(*n_reply)++] = ddlReadStatus(dev);
  }

  if (*n_reply == expected)
   return (RORC_STATUS_OK);

  if (*n_reply < expected)
    return (RORC_NOT_ENOUGH_REPLY);
  else
    return (RORC_TOO_MANY_REPLY);
}


/***************************************************************/
int ddlSendCommand(rorc_pci_dev_t *dev,
                   int             dest,
                   __u32           command,
                   int             transid,
                   __u32           param,
                   long long int   time)

/* ddlSendCommand sends one command to the given link.
 * Parameters: dev      pointer to pRorc device. It defines the link
 *                      where the command will be sent
 *             dest     command destination: 0 RORC, 1 DIU, 2 SIU, 4 FEE.
 *                      if -1 then the full command is in the
 *                      command field
 *             command  command code
 *             transid  transaction id
 *             param    command parameter,
 *                      or the full command if dest == -1
 *             time     if > 0 then test if command can be sent and
 *                      wait as many cycles if necessary.
 *
 * Returns:
 *    RORC_STATUS_OK   (0)     if command sent
 *    RORC_TIMEOUT     (-64)   if the command can not be sent in timeout.
 *    RORC_LINK_NOT_ON (-4)    if destination > 1 and the link is not on
 */

{
  __u32 com;
  int destination;
  long long int i;
#ifdef NOMAP
   ret;
#endif

  if(dest == -1)
  {
    com = command;
    destination = com & 0xf;
  }
  else
  {
    destination = dest & 0xf; 
    com = destination + ((command & 0xf)<<4) +
          ((transid & 0xf)<<8) + ((param & 0x7ffff)<<12);
  }

  debug("ddlSendCommand: command to send: 0x%08x\n", com);

  if (destination > DIU)
  {
    if (rorcCheckLink(dev) == RORC_LINK_NOT_ON) 
    {
      debug("ddlSendCommand: command can not be sent as link is not on\n");
      return (RORC_LINK_NOT_ON);
    }
  }

  for (i = 0; i < time; i++)
  {
    if (rorcCheckCommandRegister(dev) == 0)
      break;
  }

  if (time && (i == time))
    return (RORC_TIMEOUT);

  rorcPutCommandRegister (dev, com);

  return (RORC_STATUS_OK);
}
/********************************************************/
int ddlReadDataBlock(rorc_pci_dev_t   *dev,
                     unsigned long    data_phys,
                     unsigned long    return_phys,
                     rorcReadyFifo_t  *rf,
                     __u32            fee_address,
                     long long int    mem_timeout,
                     stword_t         *stw,
		     int              *n_reply,
		     int              *step)
{
  int j, exit_code = 0;
  long long int timeout, ustimeout;
  long long int i;
  int DMA_count, last_DMA_count;

  // supposed the timeouts in us for memory  and PCI loops are the same
  ustimeout = mem_timeout / dev->loop_per_usec;
  timeout = ustimeout * dev->pci_loop_per_usec;

  // push Free FIFO and start data receiver
  rf->length = -1;
  rf->status = -1;
  rorcReset(dev, RORC_RESET_FF);
  if (pRorc(dev))
  {
    pRorcInitCmdProc(dev);
    rorcPushFreeFifo(dev, data_phys, DDL_MAX_WORD, 0);
    pRorcWaitCmdProc(dev);
  }
   else
  {
    dRorcPushRxFreeFifo(dev, data_phys, DDL_MAX_WORD, 0);
  }
  debug("ddlReadDataBlock: push phys= 0x%08lx, return_phys = 0x%08lx\n",
                                       data_phys, return_phys);
  rorcStartDataReceiver(dev, return_phys);

  *step = 1;
  // send STBRD
  exit_code = ddlSendCommandAndWaitReply(dev, STBRD,
                          fee_address, timeout, stw, 1, n_reply);
  if (exit_code)
    return (exit_code);

  *step = 2;
  // wait for data
  last_DMA_count = rorcReadRxDmaCount(dev);
  j = 1;
  while(1)
  {
    i = 0;
    while (!rorcHasData(rf, 0))
    {
      if (++i == mem_timeout)
      {
        exit_code = RORC_TIMEOUT;
        break;
      }
    }
    if (exit_code == RORC_TIMEOUT)
    {
      DMA_count = rorcReadRxDmaCount(dev);
      if (interrupt_arrived  || (DMA_count == last_DMA_count))
      {
        debug("ddlReadDataBlock: RX DMA count (%d) did not change during the %d%s timeout period\n",
                DMA_count, j, th(j));
        return (exit_code);
      }
      debug("ddlReadDataBlock: RX DMA count changed from %d to %d in timeout period of %lld loops\n", 
                last_DMA_count, DMA_count, timeout);
      j++;
      last_DMA_count = DMA_count;
      exit_code = 0;
      continue;
    }
    debug("ddlReadDataBlock: Transfer done during the %d%s timeout period.\n", 
                                 j, th(j));
    break;
  }

  debug("ddlReadDataBlock: Data block arrived. Len = %ld (0x%lx), DTSTW = 0x%08lx\n",
			  rf->length, rf->length, rf->status);
  
  *step = 3;
  // send EOBTR 
  exit_code = ddlSendCommandAndWaitReply(dev, EOBTR,
                          fee_address, timeout, stw, 1, n_reply);
  return (exit_code);
}


/********************************************************/
int ddlWriteDataBlock(rorc_pci_dev_t        *dev,
                      unsigned long          data_phys,
		      unsigned long          data_length,
                      unsigned long          return_phys,
                      volatile unsigned long *return_addr,
                      __u32                  fee_address,
                      long long int          mem_timeout,
                      stword_t               *stw,
		      int                    *n_reply,
		      int                    *step)
{
  int exit_code = 0;
  int send_EOBTR;
  long long ustimeout, wait_time;
  long long int timeout;

  // supposed the timeouts in us for memory  and PCI loops are the same 
  ustimeout = mem_timeout / dev->loop_per_usec;
  timeout = ustimeout * dev->pci_loop_per_usec;

  *step = 1;
  // send STBWR 
  exit_code = ddlSendCommandAndWaitReply(dev, STBWR,
                                fee_address, timeout, stw, 1, n_reply);
  if (exit_code)
    return (exit_code);

  *step = 2;
  // send data
  exit_code = ddlSendData(dev, data_phys, data_length, 
                       return_phys, return_addr, mem_timeout, 1);
  debug("ddlWriteDataBlock: Data block sent. DDL return code: %d\n", exit_code);
  if (exit_code)
    return (exit_code);

  *step = 3;
  // send EOBTR (if data transfer is finished)

  send_EOBTR = !rorcCheckTxNotFinished(dev);
  if (!send_EOBTR)
  {
//    ustimeout = timeout / dev->pci_loop_per_usec;
    wait_time = 0;
    while (wait_time <= ustimeout)
    { 
      usleep(1000);    // sleep 1 ms
      send_EOBTR = !rorcCheckTxNotFinished(dev);
      if (send_EOBTR)
        break;
      wait_time += 1000; 
    }
  }

//  send_EOBTR = 1;

  if (send_EOBTR) 
  {
    if ((dev->diu_version == NEW) || (dev->diu_version == EMBEDDED))
      exit_code = ddlSendCommandAndWaitReply(dev, EOBTR,
                              fee_address, timeout, stw, 1, n_reply);
    else
      exit_code = ddlSendCommandAndWaitReply(dev, EOBTR,
                              fee_address, timeout, stw, 2, n_reply);

  }
  else
  {
    exit_code = RORC_TIMEOUT;
    debug("ddlWriteDataBlock: EOBTR not sent because tx not finished in %lld us. DDL return code: %d\n",
                                                   ustimeout, exit_code);
  }

  return (exit_code);
}


/****************************************************************/
int ddlSendData(rorc_pci_dev_t        *dev, 
                unsigned long           sourceaddr, 
                unsigned long           length,
                unsigned long           return_phys, 
                volatile unsigned long *return_virt,
                long long int           timeout,
                int                     wait_ready)

/****************************************************************************** 
* This function is used to send data block                                    * 
* Parameters: dev         pointer to pRORC device,                            *
*             sourceaddr  the physical address of the data to be sent         *
*             length	  data lenght in words                                *
*             return_phys return physical address                             *
*             return_virt virtual pointer to return address                   *
*             timeout	  if > 0 then wait as many cycles and test if previous*
*                         transfer done.                                      *
*             wait_ready  if !=0 then wait for DMA ending                     *
*                                                                             *
* Returns:   RORC_STATUS OK    if the transfer done                           *
*            RORC_NOT_ABLE     if the previous download was not finished      *
*            RORC_LINK_NOT_ON  link is not on, data can not be sent           *
*            RORC_TIMEOUT      if the given transfer is not done in timeout   *
******************************************************************************/
 
{
  int j, ret;
  long long int i;
  static int first_call = 1;
  int DMA_count, last_DMA_count;

  if (first_call)
    first_call = 0;
  else if (*return_virt == -1)
  {
    if (timeout)
    {
      /* loop for DMA ending */
      i = 0;
      while (*return_virt == -1)
      {
        i++;
        if (i > timeout)
         break;
      }
      if (*return_virt == -1)
        return (RORC_NOT_ABLE);
    }
    else
      return (RORC_NOT_ABLE);
  }

  if (pRorc(dev))
  {
    /* send DTSTW first */
    ret = ddlSendCommand(dev, SIU, DTCC, 0, length, 0);
    if (ret == RORC_LINK_NOT_ON)
      return (ret);
  }

  /* send start DMA command */
  *return_virt = -1;
  if (!pRorc(dev))
    *(return_virt + 1) = -1;
  rorcStartDownload(dev, sourceaddr, length, return_phys);
//  rorcStartDownload(dev, return_phys); // source es length -t a FrFF-ba!

  ret = RORC_STATUS_OK;
  if (wait_ready && timeout)
  {
    /* loop for DMA ending */
    j = 1;
    last_DMA_count = rorcReadTxDmaCount(dev);
    while (1)
    {
      i = 0;
      while (*return_virt == -1)
      {
        i++;
        if (i > timeout)
         break;
      }
      if (*return_virt == -1)
      {
        DMA_count = rorcReadTxDmaCount(dev);
        if (interrupt_arrived || (DMA_count == last_DMA_count))
        {
          printf("ddlSendData: TX DMA count (%d) did not change during the %d%s timeout period\n",
                     DMA_count, j, th(j));
          ret = RORC_TIMEOUT;
          debug("ddlSendData returns with RORC_TIMEOUT\n");
          break;
        }
        debug("ddlSendData: TX DMA count changed from %d to %d in timeout period of %lld loops\n",
                last_DMA_count, DMA_count, timeout);
        j++;
        last_DMA_count = DMA_count;
        continue;
      }
      debug("ddlSendData: Transfer done during the %d%s timeout period.\n",
                      j, th(j));
//      date_log("ddlSendData: Transfer done during the %d%s timeout period.",
//                      j, th(j));
//      printf("ddlSendData: Transfer done during the %d%s timeout period.\n",
//                      j, th(j));
      ret = RORC_STATUS_OK;
      break;
    }
  }

  return (ret);

}


/********************************************************/
int ddlWriteJTAGBlock(rorc_pci_dev_t        *dev,
                      unsigned long          data_phys,
                      unsigned long          data_length,
                      unsigned long          return_phys,
                      volatile unsigned long *return_addr,
                      __u32                  fee_address,
                      long long int          timeout,
                      stword_t               *stw,
                      int                    *n_reply,
                      int                    *step)
{
  int exit_code = 0;
  int expected = 1;
  long long int i, longret;
  int response_length;
  volatile unsigned long *response_ready;

  *step = 1;
  // send JSTART
  *n_reply = 0;
  if (rorcCheckStatus(dev))
  {
    stw[(*n_reply)++] = ddlReadStatus(dev);
    return (RORC_TOO_MANY_REPLY);
  }
  exit_code = ddlSendCommand(dev, JTAG, JSTART, 1, fee_address, timeout);
  if (exit_code)
    return (exit_code);

  while (*n_reply < expected)
  {
    longret = ddlWaitStatus(dev, timeout);
    if (longret >= timeout)
      break;
    stw[(*n_reply)++] = ddlReadStatus(dev);
  }

  if (*n_reply < expected)
    return (RORC_NOT_ENOUGH_REPLY);

  if (*n_reply > expected)
    return (RORC_TOO_MANY_REPLY);

  *step = 2;
  // send JTAG data
  response_length = data_length/4;
  if (data_length != 4 * response_length)
    response_length++;
  response_ready = return_addr + response_length;
  *response_ready = -1;
  for (i = 0; i < response_length; i++)
    *(return_addr + i) = -1;

  exit_code = ddlSendJTAG(dev, data_phys, data_length,
                       return_phys, return_addr, timeout);
  debug("ddlWriteJTAGBlock: JTAG block sent. DDL return code: %d\n", exit_code);
  if (exit_code == RORC_LINK_NOT_ON)  // RORC_TIMEOUT is not fatal
    return (exit_code);

  *step = 3;
  // wait for JTAG response
  if (timeout)
  {
    /* loop for DMA ending */
    i = 0;
    while (*response_ready == -1)
    {
      i++;
      if (i > timeout)
       break;
    }
    if (*response_ready == -1)
      return(RORC_TIMEOUT);
  }

  *step = 4;
  // send EOJTR
  *n_reply = 0;
  if (rorcCheckStatus(dev))
  {
    stw[(*n_reply)++] = ddlReadStatus(dev);
    return (RORC_TOO_MANY_REPLY);
  }
  exit_code = ddlSendCommand(dev, JTAG, EOJTR, 3, fee_address, timeout);

  if (exit_code)
    return (exit_code);

  while (*n_reply < expected)
  {
    longret = ddlWaitStatus(dev, timeout); 
    if (longret >= timeout)
      break;
    stw[(*n_reply)++] = ddlReadStatus(dev);
  }

  if (*n_reply < expected)
    return (RORC_NOT_ENOUGH_REPLY);
  if (*n_reply > expected) 
    return (RORC_TOO_MANY_REPLY);

  return (exit_code);
}


/****************************************************************/
int ddlSendJTAG(rorc_pci_dev_t        *dev,
                unsigned long           sourceaddr,
                unsigned long           length,
                unsigned long           return_phys, 
                volatile unsigned long *return_virt,
                long long int           time)

/******************************************************************************
* This function is used to send JTAG block                                    *
* Parameters: dev         pointer to pRORC device,                            *
*             sourceaddr  the physical address of the JTAG data to be sent    *
*             length      data lenght in words                                *
*             return_phys return physical address                             *
e             return_virt virtual pointer to return address                   *
*             time        if > 0 then wait as many cycles and test if the     *
*                         JTAG response arrived                               *
*                                                                             *
* Returns:   RORC_STATUS OK    if the transfer done                           *
*            RORC_LINK_NOT_ON  link is not on, data can not be sent           *
*            RORC_TIMEOUT      if the response does not arrived in timeout    *
******************************************************************************/

{
  int ret;
  long long int i;

  /* send JTSTW first */
  ret = ddlSendCommand(dev, JTAG, JTSTW, 0, length, 0);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);

  /* send start DMA command */
  *return_virt = -1;
  rorcStartJtag(dev, sourceaddr, length, return_phys);

  if (time)
  {
    /* loop for DMA ending */
    i = 0;
    while (*return_virt == -1)
    {
      i++;
      if (i > time)
       break;
    }
    if (*return_virt == -1)
      return(RORC_TIMEOUT);
  }

  return(RORC_STATUS_OK);

}

/***************************************************************
* read a status word                                           *
***************************************************************/
stword_t ddlReadStatus(rorc_pci_dev_t *prorc)

{
stword_t stw;

  /* call ddlWaitStatus() before this routine
     if status timeout is needed */ 
  if (pRorc(prorc))
    stw.stw = pRorcReadMb (prorc, 1);
  else
    stw.stw = dRorcReadReg (prorc, C_DSR);
  debug("ddlReadStatus: status = %08lx\n", stw.stw);

  return(stw);
}

/***************************************************************
* check status fifo not empty in timeout                       *
***************************************************************/
long long int ddlWaitStatus(rorc_pci_dev_t *prorc, long long int timeout)

/* Checks whether status mail box or register is not empty in timeout       
 *
 * Parameters: prorc    pointer to pRORC device 
 *             timeout  # of check cycles
 *
 * Returns:    # of executed cycles
 *
*/

{
  long long int tries = 0;

  do
  {      
    if(rorcCheckStatus(prorc))
      break;
    tries++;  
  }while(tries <= timeout);

  return(tries);
}

/********************************************************
*    read DIU or SIU hardware version                   *
********************************************************/
__u8 ddlReadHW(rorc_pci_dev_t *prorc, int dest, int address, long long int time)

{
  unsigned long retval;
  long long longret;
  stword_t stw;
  __u8 hw;
  int transid;
  __u32 param, command;
  #ifndef QUIET
    char* destName[] = {"unknown", "DIU", "SIU"};
  #endif

 /* prepare and send DDL command */

  command = RHWVER;
  transid = 0;
  param = address;
  retval = ddlSendCommand(prorc, dest, command, transid, param, time);
  if (retval == RORC_LINK_NOT_ON)
  {
    date_log("Link is not on, can not send DDL command");
    return('\0');
  }
  if (retval == RORC_TIMEOUT)
  {
    date_log("DDL command can not be sent in timeout %lld", time);
    return('\0');
  }

  /* read and check the answer */
  longret = ddlWaitStatus(prorc, time);
  if (longret >= time)
  {
    date_log("No %s HW status arrived in time-out %lld usec",
           destName[dest], (long long int)(time/prorc->pci_loop_per_usec));
    return('\0');
  }

  stw = ddlReadStatus(prorc);
  hw   = (stw.stw >> 20) & 0xff;
  debug("HW status word = 0x%08lx.\n", stw.stw);
  if (ST_CODE(stw.stw) != HWSTW ||
      ST_DEST(stw.stw) != dest ||
      ST_TRID(stw.stw) != transid)
  {
    date_log("NOT %s HWSTW! Expected: 0x.....%x%x%x, received: 0x%08lx",
           destName[dest], transid, HWSTW, dest, stw.stw);
    return ('\0');
  }

  stw = ddlReadCTSTW(prorc, transid, dest, time);
  return(hw);
}

/***************************************************************/
rorcHwSerial_t ddlSerial(rorcHandle_t prorc, int dest, long long int time)
{
  int i;
  rorcHwSerial_t hw;

  hw.data[0] = '\0';
  hw.version = -1;
  hw.subversion = -1;
  hw.serial = -1;
  
  if ( (prorc->diu_version <  NEW) ||
      ((dest == DIU) && (prorc->diu_version != NEW)) )
    return (hw);

  for (i = 0; i < DDL_MAX_HW_ID - 1; i++)
  {
    hw.data[i] = ddlReadHW(prorc, dest, i, time);
    if (hw.data[i] == '\0')
      break;
  }
  hw.data[i] = '\0';

  if (strncmp(hw.data, "DDL card", 8) != 0)
    return(hw);

  hw.version = hw.data[9] - '0';
  hw.subversion = hw.data[11] - '0';
  hw.serial = atoi(&hw.data[44]);

  return(hw);
}

/********************************************************
*    read DIU or SIU firmeware version                  *
********************************************************/
unsigned long ddlReadFW(rorc_pci_dev_t *prorc, int destination, 
                                           long long int time)

{
  unsigned long retval;
  stword_t stw;
  unsigned long fw;
  int transid;
  long long int longi;
  __u32 param, command;
  #ifndef QUIET
    char* destName[] = {"unknown", "DIU", "SIU"};
  #endif

 /* prepare and send DDL command */

  command = RFWVER;
  transid = 0;         //!!!!!!!!
  param = 0;
  retval = ddlSendCommand(prorc, destination, command, transid, param, time);
  if (retval == RORC_LINK_NOT_ON)
  {
    date_log("Link is not on, can not send DDL command");
    retval = -1;
    return(retval);
  }
  if (retval == RORC_TIMEOUT)
  {
    date_log("DDL command can not be sent in timeout %lld", time);
    retval = -1;
    return(retval);
  }

  /* read and check the answer */

  for (longi = 0; longi < time; longi++)
  {
    if (ddlCheckStatus(prorc))
      break;
  }
  if (time && (longi >= time))
  {
    date_log("No %s FW status arrived in time-out %lld usec", 
        destName[destination], (long long int)(time/prorc->pci_loop_per_usec));
    retval = -1;
    return(retval);
  }

  stw = ddlReadStatus(prorc);
  fw   = stw.stw;
  debug("FW status word = 0x%08lx.\n", fw);
  debug("Version: %d, release date (yy/mm/dd): %02d/%02d/%02d\n",
          FW_VERS(fw), FW_YEAR(fw), FW_MONTH(fw), FW_DAY(fw));
  if (ST_CODE(fw) != FWSTW || 
      ST_DEST(fw) != destination || 
      ST_TRID(fw) != transid)
  {
    date_log("NOT %s FWSTW! Expected: 0x.....%x%x%x, received: 0x%08lx",
           destName[destination], transid, FWSTW, destination, fw); 
    retval = -1;
    return (retval);
  }

  stw = ddlReadCTSTW(prorc, transid, destination, time);
  return(fw);
}

/********************************************************
*    read and clear the DIU status                      *
********************************************************/
unsigned long ddlReadDiu(rorc_pci_dev_t *prorc, int transid, 
                                               long long int time)

{
  unsigned long retval;
  stword_t stw;
  int dest;
  long long int longi;
  __u32 param, command;

  /* prepare and send DDL command */

  dest = DIU;
  command =  RandCIFST;
  param = 0;
  retval = ddlSendCommand(prorc, dest, command, transid, param, time);
  if (retval == RORC_TIMEOUT)
  {
    date_log("ddlReadDiu: DIU command can not be sent in timeout %lld", time);
    retval = -1;
    return(retval);
  }

  /* read and check the answer */

  for (longi = 0; longi < time; longi++)
  {
    if (ddlCheckStatus(prorc))
      break;
  }
  if (time && (longi >= time))
  {
    date_log("ddlReadDiu: no status arrived in time-out %lld usec", 
                          (long long int)(time/prorc->pci_loop_per_usec));
    retval = -1;
    return(retval);
  }

  stw = ddlReadStatus (prorc);
  retval = stw.stw;
  if (stw.part.code != IFSTW ||
      stw.part.trid != transid   ||
      stw.part.dest != dest)
  
  {
    date_log("NOT DIU IFSTW! Expected: 0x00000%x%x%x, received: 0x%08lx",
                           transid, IFSTW, dest, stw.stw);
    retval = -1;
  }

  stw = ddlReadCTSTW(prorc, transid, dest, time);

  return(retval);
}

/********************************************************
*    read and clear the SIU status                      *
********************************************************/
unsigned long ddlReadSiu(rorc_pci_dev_t *prorc, int transid, 
                                                long long int time)

{
  unsigned long retval;
  stword_t stw;
  int dest;
  long long int longi;
  __u32 param, command;

  /* prepare and send DDL command */

  dest = SIU;
  command = RandCIFST;
  param = 0;
  retval=ddlSendCommand(prorc, dest, command, transid, param, time);
  if (retval == RORC_LINK_NOT_ON)
  {
    date_log("ddlReadSiu: SIU command can not be send since link is not on");
    retval = -1;
    return(retval);
  }
  if (retval == RORC_TIMEOUT)
  {
    date_log("ddlReadSiu: SIU command can not be sent in timeout %lld", time);
    retval = -1;
    return(retval);
  }

  /* read and check the answer */

  for (longi = 0; longi < time; longi++)
  {
    if (ddlCheckStatus(prorc))
      break;
  }
  if (time && (longi >= time))
  {
    date_log("ddlReadSiu: no status arrived in time-out %lld usec",
                          (long long int)(time/prorc->pci_loop_per_usec));
    retval = -1;
    return(retval);
  }

  stw = ddlReadStatus(prorc);
  if (stw.part.code != IFSTW ||
      stw.part.trid != transid   ||
      stw.part.dest != dest)
  {
    date_log("NOT SIU IFSTW! Expected: 0x00000%x%x%x, received: 0x%08lx",
                             transid, IFSTW, dest, stw.stw);
    retval = -1;
  }
  retval = stw.stw;

  stw = ddlReadStatus (prorc);
  if ((stw.part.code != CTSTW &&
       stw.part.code != ILCMD &&
       stw.part.code != CTSTW_TO) ||
       stw.part.trid != transid   ||
       stw.part.dest != dest)
  {
    date_log( "NOT CTSTW! Expected: 0x00000%x%x%x, received: 0x%08lx",
                          transid, CTSTW, dest, stw.stw);
  }

  return(retval);
}


/********************************************************
*    read and print the DIU or SIU status               *
********************************************************/
char *ddlPrintStatus(rorc_pci_dev_t *prorc, int dest, int prefix)

{
  long long int time, ustime;
  unsigned long status;
  char pref[13], suff[2];

  ustime = DDL_RESPONSE_TIME;
  time = ustime * prorc->pci_loop_per_usec;

  statInfo[0] = '\0';

  /* send DDL command for DIU or SIU status*/
  if (dest == DIU)
    status = ddlReadDiu(prorc, 0, time);
  else if (dest == SIU)
    status = ddlReadSiu(prorc, 0, time);
  else
  {
    stat_log("ddlPrintStatus called with bad argument");
    if (!prefix)
      stat_log("\n");
    return (statInfo);
  }

  pref[0] = '\0';
  suff[0] = '\0';
  if (!prefix)
  { 
    if (dest == DIU)
      strcpy(pref, "DIU status: ");
    else
      strcpy(pref, "SIU status: ");
    suff[0] = '\n';
    suff[1] = '\0';
  }

  if (status != -1)
  {
    stat_log("%s0x%08lx, i.e.%s", pref, status, suff);
    if (prefix)
      strcpy(pref, ",");
    else
      pref[0] = '\0';
    ddlInterpretIFSTW(prorc, status, pref, suff);
  }
  else
    stat_log("%snot given%s", pref, suff);

  if (!prefix)
    stat_log("\n");
 
  return (statInfo);
}


/********************************************************
*    read FEE status word                               *
********************************************************/
stword_t ddlReadFESTW(rorc_pci_dev_t *prorc, int fee_addr, int transid, 
                                         long long int time)

{
  stword_t stw, stw_c;
  int retval;
  long long int longi;

  retval=ddlSendCommand(prorc, FEE, FESTRD, transid, fee_addr, time);
  if(retval == RORC_LINK_NOT_ON || (retval == RORC_TIMEOUT))
  {
    debug("ddlSendCommand error. retval = %d\n", retval);
    stw.part.param = -1;
    return(stw);
  }

  for (longi = 0; longi < time; longi++)
  {
    if (ddlCheckStatus(prorc))
      break;
  }
  if (time && (longi >= time))
  {
    date_log("ddlReadFESTW: no status arrived in time-out %lld usec",
                    (long long int)(time/prorc->pci_loop_per_usec));
    stw.part.param = -1;
    return(stw);
  }

  stw = ddlReadStatus(prorc);
  debug("FEE status word = 0x%08lx\n", stw.stw);
  if (stw.part.code != FESTW || 
      stw.part.dest != FEE   || 
      stw.part.trid != transid)
  {
    date_log("NOT FESTW! Expected: 0x.....%x%x%x, received: 0x%08lx",
                       transid, FESTW, FEE, stw.stw); 
    if (stw.part.code == CTSTW_TO)
    {
      date_log("The 'Front-End Timeout' bit set: no reply arrived from the FE to the SIU\n");
    }
    stw.part.param = -1;
  }

  stw_c = ddlReadCTSTW(prorc, transid, FEE, time);
  return(stw);
}

/********************************************************
*    read CTSTW                                         *
********************************************************/
stword_t ddlReadCTSTW(rorc_pci_dev_t *prorc, int transid, int destination, 
                                        long long int time)

{
  long long int longi;
  stword_t stw;

  for (longi = 0; longi < time; longi++)
  {
    if (ddlCheckStatus(prorc))
      break;
  }
  if (time && (longi >= time))
  {
    date_log("ddlReadCTSTW: no CTSTW arrived in time-out %lld usec", 
                    (long long int)(time/prorc->pci_loop_per_usec));
    stw.part.param = -1;
    return(stw);
  }

  stw = ddlReadStatus(prorc);
  debug("CTSTW = 0x%08lx\n", stw.stw);
  if ((stw.part.code != CTSTW && 
       stw.part.code != ILCMD &&
       stw.part.code != CTSTW_TO) ||
       stw.part.trid != transid   ||
       stw.part.dest != destination)
  {
    date_log("NOT CTSTW! Expected: 0x%x%x%x, received: 0x%x%x%x",
                       transid, CTSTW, destination, 
                       stw.part.trid, stw.part.code, stw.part.dest);
    stw.part.param = -1;
  }
  return(stw);
}

/*******************************************************
* set DIU to loopback mode                             *
*******************************************************/
int ddlDiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, DIU, IFLOOP, 0, 0, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}


/*******************************************************
* set SIU to loopback mode                             *
*******************************************************/
int ddlSetSiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;
  unsigned long retlong;

  /* check SIU fw version */

  ret = ddlSendCommand(dev, SIU, IFLOOP, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;

  *stw = ddlReadStatus(dev);
  if (stw->part.code == ILCMD)
  {
    /* illegal command => old version => send TSTMODE for loopback */
    ret = ddlSendCommand(dev, SIU, TSTMODE, 0, 0, timeout);
    if (ret == RORC_LINK_NOT_ON)
      return (ret);
    if (ret == RORC_TIMEOUT)
      return RORC_STATUS_ERROR;

    longret = ddlWaitStatus(dev, timeout);
    if (longret >= timeout)
      return RORC_NOT_ACCEPTED;
    else
      return RORC_STATUS_OK;
  }
  if (stw->part.code != CTSTW)
    return RORC_STATUS_ERROR;

  /* SIU loopback command accepted => check SIU loopback status */
  retlong = ddlReadSiu(dev, 0, timeout);
  if (retlong == -1)
    return RORC_STATUS_ERROR;

  if (retlong & S_LBMOD)
    return RORC_STATUS_OK; // SIU loopback set 

  /* SIU loopback not set => set it */
  ret = ddlSendCommand(dev, SIU, IFLOOP, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;

}

/*******************************************************
* clear SIU loopback mode                              *
*******************************************************/
int ddlClearSiuLoopBack(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  long long int ret;
  long long longret;
  unsigned long retlong;

  /* check SIU fw version */

  ret = ddlSendCommand(dev, SIU, IFLOOP, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;

  *stw = ddlReadStatus(dev);
  if (stw->part.code == ILCMD)
  {
    /* illegal command => old version => send TSTOP for clear loopback */
    ret = ddlSendCommand(dev, SIU, TSTOP, 0, 0, timeout);
    if (ret == RORC_LINK_NOT_ON)
      return (ret);
    if (ret == RORC_TIMEOUT)
      return RORC_STATUS_ERROR;

    longret = ddlWaitStatus(dev, timeout);
    if (longret >= timeout)
      return RORC_NOT_ACCEPTED;
    else
      return RORC_STATUS_OK;
  }
  if (stw->part.code != CTSTW)
    return RORC_STATUS_ERROR;

  /* SIU loopback command accepted => check SIU loopback status */
  retlong = ddlReadSiu(dev, 0, timeout);
  if (retlong == -1)
    return RORC_STATUS_ERROR;

  if (!(retlong & S_LBMOD))
    return RORC_STATUS_OK; // SIU loopback not set

  /* SIU loopback set => clear it */
  ret = ddlSendCommand(dev, SIU, IFLOOP, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}

/*******************************************************
* set SIU to PRBS test mode                            *
*******************************************************/
int ddlSetSiuTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, SIU, TSTMODE, 0, CLRPRBS, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}

/*******************************************************
* Start PRBS test and clear error counter              *
*******************************************************/
int ddlStartTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  /* start PRBS test mode in DIU */
  ret = ddlSendCommand(dev, DIU, TSTMODE, 0, STRPRBS, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  /* wait a little */
  usleep (1000);

  /* clear pRDS error counter */
  ret = ddlSendCommand(dev, DIU, TSTMODE, 0, CLRPRBS, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}


/********************************************************
*    read PRBS error counter                            *
********************************************************/
int ddlReadPRBS(rorcHandle_t dev, int transid, long long int time, stword_t *stw)
{
  int ret;
  long long int longret;
  stword_t stw_ctstw;

  /* send DDL command */

  ret = ddlSendCommand(dev, DIU, TSTMODE, transid, RDPRBS, time);
  if (ret == RORC_TIMEOUT)
    return(RORC_STATUS_ERROR);

  /* read and check the answer */
  longret = ddlWaitStatus(dev, time);
  if (longret >= time)
    return(RORC_TIMEOUT);

  *stw = ddlReadStatus (dev);
  if (stw->part.code != TEVAL   ||
      stw->part.trid != transid ||
      stw->part.dest != DIU)
    ret = RORC_NOT_ACCEPTED;
  else
    ret = RORC_STATUS_OK;

  stw_ctstw = ddlReadStatus(dev);

  return(ret);
}


/*******************************************************
* stop PRBS test mode                                  *
*******************************************************/
int ddlStopTestMode(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  /* stop PRBS test mode in DIU */
  ret = ddlSendCommand(dev, DIU, TSTMODE, 0, STPPRBS, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  /* wait a little */
  usleep (1000);

  /* suspend DIU laser */
  ret = ddlSendCommand(dev, DIU, SUSPEND, 0, 0, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  /* wait again */
  usleep (1000);

  /* wakeup DIU and SIU */
  ret = ddlSendCommand(dev, DIU, WAKEUP, 0, 0, timeout);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}


/*******************************************************
* send RDYRX to the FEE                                *
*******************************************************/
int rorcStartTrigger(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, FEE, RDYRX, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}

/*******************************************************
* send STBWR to the FEE                                *
*******************************************************/
int ddlStartBlockWrite(rorcHandle_t dev, unsigned long fee_address,  
                       long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, FEE, STBWR, 0, fee_address, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}

/*******************************************************
* send STBRD to the FEE                                *
*******************************************************/
int ddlStartBlockRead(rorcHandle_t dev, unsigned long fee_address,
                      long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, FEE, STBRD, 0, fee_address, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}

/*******************************************************
* send EOBTR to the FEE                                *
*******************************************************/
int rorcStopTrigger(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, FEE, EOBTR, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}


/*******************************************************
* send JSTART                                          *
*******************************************************/
int rorcSendJSTART(rorcHandle_t dev, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;

  ret = ddlSendCommand(dev, JTAG, JSTART, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(dev, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(dev);

  return RORC_STATUS_OK;
}


/******************************************************************************/
int rorcArmDDL(rorc_pci_dev_t *dev, int option)
{
  int ret;
  unsigned long retlong;
  int print = 0; // -1;
  int stop = 1;
  long long int TimeOut;

  TimeOut = DDL_RESPONSE_TIME * dev->pci_loop_per_usec;


  if (dev->diu_version)
  {
    if (option & RORC_RESET_FEE)
    {
      /* not implemented */
      return RORC_CMD_NOT_ALLOWED;
    }
    if (option & RORC_RESET_SIU)
    {
      ret = ddlResetSiu(dev, 0, 3, TimeOut);
      if (ret == -1)
      {
        date_log(" Unsuccessful SIU reset");
        return RORC_NOT_ACCEPTED;
      }
    }
    if (option & RORC_LINK_UP)
    {
      if (dev->diu_version <= NEW)
      {
        retlong = ddlLinkUp(dev, 1, print, stop, TimeOut);
        if (retlong == -1)
        {
          date_log(" Can not read DIU status");
          return RORC_LINK_NOT_ON;
        }
      }
      else
      {
        rorcReset(dev, RORC_RESET_RORC);
        rorcReset(dev, RORC_RESET_DIU);
        rorcReset(dev, RORC_RESET_SIU);
        usleep(100000); 

        if (rorcCheckLink(dev))
          return (RORC_LINK_NOT_ON);
        if (rorcEmptyDataFifos(dev, 100000))
          return (RORC_TIMEOUT);

        rorcReset(dev, RORC_RESET_SIU);
        rorcReset(dev, RORC_RESET_DIU);
        rorcReset(dev, RORC_RESET_RORC); 
        usleep(100000);

        if (rorcCheckLink(dev))
          return (RORC_LINK_NOT_ON);
      }
    }
    if (option & RORC_RESET_DIU)
       rorcReset(dev, RORC_RESET_DIU);
  }
  else
  {
    date_log(" No DIU plugged into the RORC");
    return (RORC_LINK_NOT_ON);
  }
  if (option & RORC_RESET_FF)
      rorcReset(dev, RORC_RESET_FF);
  if (option & RORC_RESET_RORC)
     rorcReset(dev, RORC_RESET_RORC);

  return RORC_STATUS_OK;
}
/****************************************************************************/
int rorcArmFeic(rorc_pci_dev_t *dev, int           pattern,
                                     int           evlen, 
                                     int           trig, 
                                     int           flctrl,
                                     int           trdis,
                                     int           seed,
                                     long long int time,
                                     int          *rounded_len)
{
  stword_t stw, stw_c;
  int i, value, logNum, len_code;
  int transid, retval;
  unsigned long command, fee_addr;

  /* round the event length */

  if ((evlen < 16) || (evlen > 0x10000000))
  {
    *rounded_len = evlen;
    return (RORC_INVALID_PARAM);
  }

  logNum = logi2(evlen);
  len_code = logNum - 3;
  *rounded_len = 1 << logNum;
  if (seed)
    len_code |= 0x80;

  command = FECTRL;
  transid = 0;
  for (i = 0; i < 6; i++)
  {
    /* Send FE command */
    switch (i)
    {
      case 0: value = pattern;   break;
      case 1: value = len_code;  break;
      case 2: value = trig;      break;
      case 3: value = flctrl;    break;
      case 4: value = trdis;     break;
      case 5: value = seed;      break;
      default: value = pattern;  break;
    }
    fee_addr = i * 0x100 + value;
    transid = incr15(transid);
    retval = ddlSendCommand(dev, FEE, command, transid, fee_addr, time);
    if (retval == RORC_LINK_NOT_ON)
      return (retval);
    if (retval == RORC_TIMEOUT)
      return (RORC_TIMEOUT);
    stw_c = ddlReadCTSTW(dev, transid, FEE, time);

    /* Send FE status read */
    fee_addr = i * 0x100;
    transid = incr15(transid);
    stw = ddlReadFESTW(dev, fee_addr, transid, time);
    if (i != 5)                         // !!!!!!!!!!!!!!!!!!!
    if (stw.part.param != value)
      return (RORC_STATUS_ERROR);
  }

  return (RORC_STATUS_OK);

}
/****************************************************************************/

#endif
