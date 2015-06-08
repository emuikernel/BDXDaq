/*
 * File:  intDisconnect.c
 * Blame: David Abbott -- abbottd@jlab.org
 * Date:  September 1996
 *
 * Description:
 * -----------
 * This function un-installs an interrupt service routine on a Tornado/PPC board
 *
*/

#include <vxWorks.h>
#include <stdlib.h>
#include <stdio.h>
#include <symbol.h>
#include <sysSymTbl.h>
#include <iv.h>
#include <intLib.h>


/*
#include <drv/intrCtl/i82378zb.h>     
*/
typedef struct intHandlerDesc           /* interrupt handler desciption */
    {
    VOIDFUNCPTR                 vec;    /* interrupt vector */
    int                         arg;    /* interrupt handler argument */
    struct intHandlerDesc *     next;   /* next interrupt handler & argument */
    } INT_HANDLER_DESC;


extern sysIntTbl[];
extern sysVmeIntTable[];
int intDisconnect();
LOCAL int handlerDelete();

/******************************************************************************
 *
 * intDisconnect - un-install an ISR
 *
 * This routine un-installs an ISR from the interrupt vector table used by 
 * the local and VMEbus interrupt dispatchers.
 *
 *****************************************************************************/
int
intDisconnect(int vector)
{

  INT_HANDLER_DESC *handler; 
  
  if (vector < 16)
    {
      printf("ERROR: cannot disconnect a local bus ISR with intDisconnect\n");
      return(ERROR);
    }
  if (vector > 255)
    {
      printf("ERROR: interrupt vector out of range\n");
      return(ERROR);
    }
  
  handler = (INT_HANDLER_DESC *)sysIntTbl[(int)vector];

  if (handler != NULL)
    {
      printf("disconnecting vector %d \n", vector);
      
      handlerDelete(handler);
      sysIntTbl[(int)vector] = NULL;
      
    }
  return(OK);
  
}

/******************************************************************************
 *
 * handlerDelete() - recursively deletes ISRs from sysIntTbl vector
 *
 *****************************************************************************/
LOCAL int
handlerDelete(INT_HANDLER_DESC *handler)
{
  if (handler->next != NULL)
    {
      handlerDelete(handler->next);
    }

  free(handler);
  handler = NULL;

  return(OK);
  
}
