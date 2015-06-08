/*
 * Library for the memory allocation system 
 */
#ifdef VXWORKS
#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <memLib.h>
#include <cacheLib.h>
#endif

#include <types.h>
#include "dmaPList.h"

#define maximum(a,b) (a<b ? b : a)

/* global data */

static DALIST  dmaPList; /* global part list */

void
dmaPartInit()
{
  listInit(&dmaPList);
}

/*************************************************************
 * partCreate - create and initialize a memory part 
 */

ROL_MEM_ID 
dmaPCreate (name, size, c, incr)
     char *name;		/* name of new partition */
     int size;			/* size of a single item */
     int c;			/* initial number of items */
     int incr;			/* number of items to add when enlarging */
     
{
  ROL_MEM_ID pPart;
  int c_alloc = 0;
  
  pPart = (ROL_MEM_ID) cacheDmaMalloc (sizeof(ROL_MEM_PART));
  bzero ((char *) pPart, sizeof(ROL_MEM_PART));
  if (pPart != NULL)
    {
      listInit (&(pPart->list));
      pPart->size = size + sizeof(DANODE);
      pPart->incr = 0; 
      pPart->total = 0;
      strcpy(pPart->name, name);
      if (name && strlen(name) == 0)
	pPart->name[0] = 0;
      listAdd (&dmaPList, (DANODE *)pPart);      
      if((dmaPIncr (pPart, c)) != c)
	return (0);
    }
  return pPart;
}

/************************************************************** 
 * dmaPFindByName - returns a pointer to a memory part
 *                  with the passed Name.
 */

ROL_MEM_ID 
dmaPFindByName (char *name)
{
  ROL_MEM_ID	pPart;

  pPart = (ROL_MEM_ID) listFirst (&dmaPList);
  
  while (pPart != NULL)
    {
      if (pPart->name && strcmp(pPart->name, name) == 0)break;
      pPart = (ROL_MEM_ID) listNext ((DANODE *)pPart);
    }
  return (pPart);
}


/************************************************************** 
 * dmaPFree - frees all nodes for a given memory part
 *               and removes part from global part list. 
 */

void 
dmaPFree(ROL_MEM_ID pPart)
{
  DANODE *the_node;

  if(pPart == NULL) return;
  printf("free list %s\n",pPart->name);
  
  if (pPart->incr == 1) {
    /* Free all buffers in the partition individually */
    while (pPart->list.c) {
      listGet(&(pPart->list),the_node);
      cacheDmaFree(the_node);
      the_node = (DANODE *)0;
    }
    listSnip (&dmaPList, (DANODE *)pPart);
  } else {
    /* Just need to Free the one contiguous block of data */
    listSnip (&dmaPList, (DANODE *)pPart);      
    cacheDmaFree( *((char **) &pPart->part[0]));
  }
  cacheDmaFree(pPart);
  pPart = 0;
}


/************************************************************** 
 * dmaPFreeAll - frees all memory parts in global part list
 *               and frees all nodes for a given list. 
 */

void 
dmaPFreeAll()
{
  ROL_MEM_ID	pPart = (ROL_MEM_ID) 0;

  if (listCount(&dmaPList)) {
    pPart = (ROL_MEM_ID) listFirst (&dmaPList);    
    while (pPart != NULL)
      {
	dmaPFree(pPart);
	pPart = (ROL_MEM_ID) listFirst (&dmaPList);
      }
  }
}



/************************************************************** 
 * partIncr - increase part size by c items (min) 
 */

int 
dmaPIncr ( ROL_MEM_ID pPart, int c)
{
/*      ROL_MEM_ID pPart      	partition to be enlarged */
/*      int  c		        min # of items to be added */

  register char *node;
  register long *block;
  unsigned bytes;
  int total_bytes, ii;
  int actual = c;		/* actual # of items added */

  pPart->total += c;

  if ((pPart == NULL)||(c == 0)) return (0);

  total_bytes =  c * pPart->size;

#ifdef VXWORKS
  if (memFindMax() <= total_bytes) {
    printf("dmaPIncr: Cannot malloc a single memory Block for data");
    if (memFindMax() < total_bytes/4) {
      printf("dmaPIncr: Memory Fragmentation to severe to allocate buffers - Reboot CPU");
      return (-1);
    } else {
      pPart->incr = 1; /* Create a Fragmented memory Partition */
      while (actual--)
	{
	  if ( memFindMax() <= (pPart->size * actual)/4 ) {
	    printf("dmaPIncr: bufs %d max %d  needed %d \n",actual, memFindMax(),(pPart->size * (actual+1)));
	    printf("dmaPIncr: ERROR: Cannot complete buffer allocation - Reboot CPU");
	    return ((c - actual + 1));
	  }
	  block = (long *) cacheDmaMalloc (pPart->size);
	  if (block == NULL)
	    return (-1);
	  bzero((char *) block, pPart->size);
	  ((DANODE *)block)->part = pPart; /* remember where we came from... */
	  listAdd (&pPart->list,(DANODE *)block);
	}
      return (c);
    }
  } else {
    block = (long *) cacheDmaMalloc (total_bytes);
    if (block == NULL)
      return (-1);
    
    pPart->incr = 0;
    bzero((char *) block, c * pPart->size);
    node = (char *) block;
    *((char **) &pPart->part[0]) = node;

    while (actual--)
      {      
	((DANODE *)node)->part = pPart; /* remember where we came from... */
	listAdd (&pPart->list,(DANODE *)node);
	node += pPart->size;
      }
    return (c);
  }
#else
  block = (long *) cacheDmaMalloc(total_bytes);
  if (block == NULL) {
    return (-1);
  }

  pPart->incr = 0;
  bzero((char *) block, c * pPart->size);

  node = (char *) block;
  *((char **) &pPart->part[0]) = node;

  while (actual--)
    {      
      ((DANODE *)node)->part = pPart; /* remember where we came from... */
      listAdd (&pPart->list,(DANODE *)node);
      node += pPart->size;
    }
  return (c);
#endif
}



/*****************************************************************
 *
 *  Wrapper routines for DMA Memory Partition list manipulation
 *
 *   dmaPFreeItem:  Free a buffer(node) back to its owner partition
 *   dmaPEmpty   :  Check if a Partition has available nodes
 *   dmaPGetItem :  Get (reserve) the first available  node from a partition
 *   dmaAddItem  :  Add node to a specified partition's list.
 *
 */

void 
dmaPFreeItem(DANODE *pItem) 
{

  /* if the node does not have an owner then delete it - otherwise add it back to
     the owner list - lock out interrupts to be safe */
  if ((pItem)->part == 0) {
    cacheDmaFree(pItem); pItem = 0;
  } else {
    listAdd (&pItem->part->list, pItem);
  }

  /* execute any command accociated with freeing the buffer */
  if(pItem->part->free_cmd != NULL)
    (*(pItem->part->free_cmd)) (pItem->part->clientData);
}

int
dmaPEmpty(ROL_MEM_ID pPart) 
{
  return((pPart->list.c == 0));
}

DANODE *
dmaPGetItem(ROL_MEM_ID pPart) 
{
  DANODE *theNode;

  listGet(&(pPart->list),theNode);

  return(theNode);
}

void
dmaPAddItem(ROL_MEM_ID pPart, DANODE *pItem) 
{

  listAdd(&(pPart->list),pItem);

}



/************************************************************** 
 * dmaPReInit - Initiialize an existing partition 
 */

int 
dmaPReInit (ROL_MEM_ID pPart)
{
  register char *node,*block;
  register DANODE *theNode;
  int actual;

  if (pPart == NULL) return;

  if (pPart->incr == 1) {   /* Does this partition have a Fragmented buffer list */

    /* Check if partition has buffers that do not belong to it
       and return them to their rightful owners */
    if ((pPart->total == 0) && (listCount(&pPart->list) > 0)) {
      while (listCount(&pPart->list) > 0) {
        listGet(&pPart->list,theNode);
	dmaPFreeItem(theNode);
      }
    }
    
  } else {
    /* Cheat to initialize memory partition assuming buffers in one
       contiguous memory bloack */
    bzero(*((char **) &pPart->part[0]), pPart->total * pPart->size);
    
    node = *((char **) &pPart->part[0]);
    
    actual = pPart->total;

    pPart->list.f = pPart->list.l = (DANODE *) (pPart->list.c = 0);
 
    while (actual--)
      {      
	((DANODE *)node)->part = pPart; /* remember where we came from... */
	listAdd (&pPart->list,(DANODE *)node);
	node += pPart->size;
      }
  }
}

int 
dmaPReInitAll()
{
  ROL_MEM_ID	pPart = (ROL_MEM_ID) 0;

  if (listCount(&dmaPList)) {
    pPart = (ROL_MEM_ID) listFirst (&dmaPList);    
    while (pPart != NULL)
      {
	dmaPReInit(pPart);
	pPart = (ROL_MEM_ID) listNext ((DANODE *) pPart);
      }
  }
}

/*************************************************************** 
 * dmaPHdr - Print headings for part statitistics printout 
 */

static void 
dmaPHdr ()
     
{
  printf("Address    total  free   busy   size  incr  (KBytes)  Name\n");
  printf("-------    -----  ----   ----  -----  ----  --------  ----\n");
}


/*************************************************************** 
 * dmaPPrint - Print statitistics for a single part 
 */

static void 
dmaPPrint (pPart)
     ROL_MEM_ID	pPart;    
{
  int freen;

  printf("0x%08x  ",pPart);

  if (pPart != NULL)
    {
      freen = listCount (&pPart->list); 
      printf("%4d  %4d  %4d   %5d  %4d  (%d)       %s\n",
	    pPart->total,
	     freen,
	     pPart->total - freen,
	     pPart->size,
	     pPart->incr,
	     (((pPart->total * pPart->size) + 1023) / 1024),
	     pPart->name
	     );
    }
}


/**************************************************************** 
 * dmaPStats - print statistics on a memory part 
 */

int 
dmaPStats (pPart)
     ROL_MEM_ID	pPart;
     
{
  dmaPHdr ();
  dmaPPrint (pPart);
  return (0);
}


/****************************************************************
 *
 * dmaPStatsAll - print statistics on all parts
 *
 * Print int for all memory parts.
 *
 * int includes the total number of items in the part, the
 * number that are in-use and free, the size of a single item,
 * and the total kilobytes of raw memory allocated.
 */

int 
dmaPStatsAll ()
{
  int lockKey;
  ROL_MEM_ID  pPart;
  
  dmaPHdr ();
#ifdef VXWORKS
  lockKey = intLock();
#endif
  pPart = (ROL_MEM_ID) listFirst (&dmaPList);
  while (pPart != NULL)
    {
      dmaPPrint (pPart);
      pPart = (ROL_MEM_ID) listNext ((DANODE *)pPart);
    }
#ifdef VXWORKS
  intUnlock(lockKey);
#endif
  return (0);
}


/****************************************************************
 * dmaPPrintList - prints statisics for a given list structure
 */
int 
dmaPPrintList(alist)
     DALIST *alist;
{
  DANODE *theNode;

  printf("dalist->f         %x\n",alist->f);
  printf("dalist->l         %x\n",alist->l);
  printf("dalist->c         %d\n",alist->c);

  theNode = listFirst(alist);
  while (theNode) {
    printf ("part %x prev %x self %x next %x left %d fd %d\n",
	    theNode->part,
	    theNode->p,
	    theNode,
	    theNode->n,
	    theNode->left,
	    theNode->fd);
    theNode = listNext(theNode);
  }
  return(0);
}

