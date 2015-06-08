/*----------------------------------------------------------------------------*
 *  Copyright (c) 2009        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Authors: David Abbott                                                   *
 *             abbottd@jlab.org                  Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-7190             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *             Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     Library for a memory allocation system
 *
 * SVN: $Rev: 393 $
 *
 *----------------------------------------------------------------------------*/
#ifdef VXWORKS
#include <vxWorks.h>
#include <memLib.h>
#include <cacheLib.h>
#include <types.h>
#else
#include <stdlib.h>
#include "jlabgef.h"
#endif

#include <stdio.h>
#include <string.h>
#include "dmaPList.h"

#define DOMUTEX
/* Maximum size that can be allocated for one DMA partition in Linux = 4 MB */
#define LINUX_MAX_PARTSIZE 0x400000

pthread_mutex_t   partMutex = PTHREAD_MUTEX_INITIALIZER;
#ifdef DOMUTEX
#define PARTLOCK     if(pthread_mutex_lock(&partMutex)<0) perror("pthread_mutex_lock");
#define PARTUNLOCK   if(pthread_mutex_unlock(&partMutex)<0) perror("pthread_mutex_unlock");
#else
#define PARTLOCK
#define PARTUNLOCK
#endif


#define maximum(a,b) (a<b ? b : a)

/* global data */

static DMALIST  dmaPList; /* global part list */
/******************************************************************************
 * 
 * dmaHdl_to_PhysAddr - Grab the physical address from the GEF DMA handle
 *
 * RETURNS: the 32bit physical address
 *
 */

GEF_UINT32
dmaHdl_to_PhysAddr
(
 GEF_VME_DMA_HDL inpDmaHdl
 )
{
  /* This returns an UINT32 because we're not using 64bit addresses.. yet */
  return (GEF_UINT32)inpDmaHdl->phys_addr;
}

void
dmaPartInit()
{
  dmalistInit(&dmaPList);
}

/*************************************************************
 * partCreate - create and initialize a memory part 
 */

DMA_MEM_ID 
dmaPCreate (name, size, c, incr)
     char *name;		/* name of new partition */
     int size;			/* size of a single item */
     int c;			/* initial number of items */
     int incr;			/* number of items to add when enlarging */
     
{
  DMA_MEM_ID pPart;
  /*   int c_alloc = 0; */
  GEF_STATUS status;
  GEF_VME_DMA_HDL dma_hdl;
  GEF_MAP_PTR mapPtr;
  
#ifdef VXWORKS
  pPart = (DMA_MEM_ID) cacheDmaMalloc (sizeof(DMA_MEM_PART));
  bzero ((char *) pPart, sizeof(DMA_MEM_PART));
#else
  if(vmeHdl==NULL) 
    {
      printf("dmaPCreate: ERROR: vmeHdl undefined.\n");
      return 0;
    }
  status = gefVmeAllocDmaBuf (vmeHdl,sizeof(DMA_MEM_PART),	
			      &dma_hdl,&mapPtr);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("dmaPCreate: ERROR:  gefVmeAllocDmaBuf returned 0x%x\n",status);
      return 0;
    }
  pPart = (DMA_MEM_ID) mapPtr;
  memset ((char *)pPart,0,sizeof(DMA_MEM_PART));
#endif
  if (pPart != NULL)
    {
      dmalistInit (&(pPart->list));
      pPart->size = size + sizeof(DMANODE);
      pPart->incr = 0; 
      pPart->total = 0;
#ifndef VXWORKS
      pPart->dmaHdl = dma_hdl;
#endif
      strcpy(pPart->name, name);
      if (name && strlen(name) == 0)
	pPart->name[0] = 0;
      dmalistAdd (&dmaPList, (DMANODE *)pPart);      
      if((dmaPIncr (pPart, c)) != c)
	return (0);
    }
  return pPart;
}

/************************************************************** 
 * dmaPFindByName - returns a pointer to a memory part
 *                  with the passed Name.
 */
 
DMA_MEM_ID 
dmaPFindByName (char *name)
{
  DMA_MEM_ID	pPart;

  pPart = (DMA_MEM_ID) dmalistFirst (&dmaPList);
  
  while (pPart != NULL)
    {
      if (pPart->name && strcmp(pPart->name, name) == 0)break;
      pPart = (DMA_MEM_ID) dmalistNext ((DMANODE *)pPart);
    }
  return (pPart);
}


/************************************************************** 
 * dmaPFree - frees all nodes for a given memory part
 *               and removes part from global part list. 
 */

void 
dmaPFree(DMA_MEM_ID pPart)
{
  DMANODE *the_node;
#ifndef VXWORKS
  GEF_STATUS status;
#endif

  if(pPart == NULL) return;
  printf("free list %s\n",pPart->name);

  if (pPart->incr == 1) 
    {
      /* Free all buffers in the partition individually */
      while (pPart->list.c) 
	{
	  dmalistGet(&(pPart->list),the_node);
#ifdef VXWORKS
	  cacheDmaFree(the_node);
#else
#ifdef DEBUG
	  printf("the_node->dmaHdl : 0x%x\n",(unsigned int)the_node->dmaHdl);
#endif
	  status = gefVmeFreeDmaBuf(the_node->dmaHdl);
	  if(status != GEF_STATUS_SUCCESS) 
	    {
	      printf("dmaPFree: gefVmeFreeDmaBuf returned 0x%x\n",status);
	    }
#endif
	  the_node = (DMANODE *)0;
	}
      dmalistSnip (&dmaPList, (DMANODE *)pPart);
    } 
  else 
    {
      /* Just need to Free the one contiguous block of data */
      dmalistSnip (&dmaPList, (DMANODE *)pPart);      
#ifdef VXWORKS
      cacheDmaFree( *((char **) &pPart->part[0]));
#else
      dmalistGet(&(pPart->list),the_node);
      if(the_node) 
	{
#ifdef DEBUG
	  printf("the_node->dmaHdl : 0x%x\n",
		 (unsigned int)the_node->dmaHdl );
#endif
	  status = gefVmeFreeDmaBuf(the_node->dmaHdl );
	  if(status != GEF_STATUS_SUCCESS) 
	    {
	      printf("dmaPFree: gefVmeFreeDmaBuf returned 0x%x (%d)\n",status,status&0xff);
	    }
	}
#endif
    }
#ifdef VXWORKS
  cacheDmaFree(pPart);
#else
#ifdef DEBUG
  printf("pPart->dmaHdl: 0x%x\n",(unsigned int)pPart->dmaHdl);
#endif
  status = gefVmeFreeDmaBuf(pPart->dmaHdl);
  if(status != GEF_STATUS_SUCCESS) 
    {
      printf("dmaPFree: gefVmeFreeDmaBuf returned 0x%x (%d)\n",status,status&0xff);
    }
#endif
  pPart = 0;
}


/************************************************************** 
 * dmaPFreeAll - frees all memory parts in global part list
 *               and frees all nodes for a given list. 
 */

void 
dmaPFreeAll()
{
  DMA_MEM_ID	pPart = (DMA_MEM_ID) 0;

  if (dmalistCount(&dmaPList)) 
    {
      pPart = (DMA_MEM_ID) dmalistFirst (&dmaPList);    
      while (pPart != NULL)
	{
	  dmaPFree(pPart);
	  pPart = (DMA_MEM_ID) dmalistFirst (&dmaPList);
	}
    }
}



/************************************************************** 
 * partIncr - increase part size by c items (min) 
 */

int 
dmaPIncr ( DMA_MEM_ID pPart, int c)
{
  /*      DMA_MEM_ID pPart      	partition to be enlarged */
  /*      int  c		        min # of items to be added */

  register char *node;
  register long *block;
  /*   unsigned bytes; */
  int total_bytes;
  int actual = c;		/* actual # of items added */
  GEF_STATUS status;
  GEF_MAP_PTR mapPtr;
  GEF_VME_DMA_HDL dma_hdl;

  pPart->total += c;

  if ((pPart == NULL)||(c == 0)) return (0);

  total_bytes =  c * pPart->size;

#ifdef VXWORKS
  if (memFindMax() <= total_bytes) 
    {
      printf("dmaPIncr: Creating a fragmented memory partition\n");
      if (memFindMax() < total_bytes/4) 
	{
	  printf("dmaPIncr: Memory Fragmentation to severe to allocate buffers - Reboot CPU");
	  return (-1);
	} 
      else 
	{
	  pPart->incr = 1; /* Create a Fragmented memory Partition */
	  while (actual--)
	    {
	      if ( memFindMax() <= (pPart->size * actual)/4 ) 
		{
		  printf("dmaPIncr: bufs %d max %d  needed %d \n",actual, 
			 memFindMax(),(pPart->size * (actual+1)));
		  printf("dmaPIncr: ERROR: Cannot complete buffer allocation - Reboot CPU");
		  return ((c - actual + 1));
		}
	      block = (long *) cacheDmaMalloc (pPart->size);
	      if (block == NULL)
		return (-1);
	      bzero((char *) block, pPart->size);
	      ((DMANODE *)block)->part = pPart; /* remember where we came from... */
	      dmalistAdd (&pPart->list,(DMANODE *)block);
	    }
	  return (c);
	}
    } 
  else 
    {
      block = (long *) cacheDmaMalloc (total_bytes);
      if (block == NULL)
	return (-1);
    
      pPart->incr = 0;
      bzero((char *) block, c * pPart->size);
      node = (char *) block;
      *((char **) &pPart->part[0]) = node;

      while (actual--)
	{      
	  ((DMANODE *)node)->part = pPart; /* remember where we came from... */
	  dmalistAdd (&pPart->list,(DMANODE *)node);
	  node += pPart->size;
	}
      return (c);
    }
#else
  if(LINUX_MAX_PARTSIZE <= total_bytes)
    {
      printf("%s: Creating a fragmented memory partition.\n",__FUNCTION__);
      if(LINUX_MAX_PARTSIZE < pPart->size)
	{
	  printf("%s: ERROR: Requested partition size (%d) is larger than max allowed (%d)\n",
		 __FUNCTION__,pPart->size,LINUX_MAX_PARTSIZE);
	  return (-1);
	}
      else
	{
	  pPart->incr = 1; /* Create a Fragmented memory Partition */
	  while (actual--)
	    {
	      status = gefVmeAllocDmaBuf (vmeHdl,pPart->size,	
					  &dma_hdl,&mapPtr);
	      if(status != GEF_STATUS_SUCCESS) 
		{
		  printf("dmaPIncr: ERROR.  gefVmeAllocDmaBuf returned 0x%x\n",status);
		  printf("                bytes requested = %d\n",pPart->size);
		  return -1;
		}
	      block = (long *) mapPtr;
#ifdef DEBUG
	      printf("block = 0x%x\t dma_hdl = 0x%x\n",block,dma_hdl);
#endif
	      if (block == NULL) 
		{
		  return (-1);
		}
  
	      memset((char *) block, 0, pPart->size);
	      ((DMANODE *)block)->part = pPart; /* remember where we came from... */
	      ((DMANODE *)block)->dmaHdl = dma_hdl;
	      ((DMANODE *)block)->partBaseAdr = (unsigned long)block;
	      ((DMANODE *)block)->physMemBase = dmaHdl_to_PhysAddr(dma_hdl);
	      dmalistAdd (&pPart->list,(DMANODE *)block);
	    }
	  return (c);
	}
    }
  else /* Single memory block for data */
    {
      status = gefVmeAllocDmaBuf (vmeHdl,total_bytes,	
				  &dma_hdl,&mapPtr);
      if(status != GEF_STATUS_SUCCESS) 
	{
	  printf("dmaPIncr: ERROR.  gefVmeAllocDmaBuf returned 0x%x\n",status);
	  printf("          total_bytes requested = %d\n",total_bytes);
	  return -1;
	}
      block = (long *) mapPtr;
#ifdef DEBUG
      printf("block = 0x%x\t dma_hdl = 0x%x\n",block,dma_hdl);
#endif
      if (block == NULL) 
	{
	  return (-1);
	}
  
      pPart->incr = 0;
      memset((char *) block, 0, c * pPart->size);

      node = (char *) block;
      *((char **) &pPart->part[0]) = node;

      while (actual--)
	{      
	  ((DMANODE *)node)->part = pPart; /* remember where we came from... */
	  ((DMANODE *)node)->dmaHdl = dma_hdl;
	  ((DMANODE *)node)->partBaseAdr = (pPart->part[0]);
	  ((DMANODE *)node)->physMemBase = dmaHdl_to_PhysAddr(dma_hdl);
	  dmalistAdd (&pPart->list,(DMANODE *)node);
	  node += pPart->size;
	}

      return (c);
    }
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
dmaPFreeItem(DMANODE *pItem) 
{
#ifndef VXWORKS
  GEF_STATUS status;
#endif
  unsigned long length=0;

  PARTLOCK;
  /* if the node does not have an owner then delete it - otherwise add it back to
     the owner list - lock out interrupts to be safe */
  if ((pItem)->part == 0) 
    {
#ifdef VXWORKS
      cacheDmaFree(pItem); pItem = 0;
#else
      status = gefVmeFreeDmaBuf(pItem->dmaHdl);
      if(status != GEF_STATUS_SUCCESS) 
	{
	  printf("dmaPFreeItem: gefVmeFreeDmaBuf returned 0x%x\n",status);
	}
      pItem = 0;  
#endif
    } 
  else 
    {
      /* length=(pItem->part->size - sizeof(DMANODE))>>2; */
/*       memset((char *)pItem->data,0,length*sizeof(unsigned int)); */
      pItem->length=0;
      dmalistAdd (&pItem->part->list, pItem);
    }
  
  /* execute any command accociated with freeing the buffer */
  if(pItem->part->free_cmd != NULL)
    (*(pItem->part->free_cmd)) (pItem->part->clientData);

  PARTUNLOCK;
}

int
dmaPEmpty(DMA_MEM_ID pPart) 
{
  return((pPart->list.c == 0));
}

DMANODE *
dmaPGetItem(DMA_MEM_ID pPart) 
{
  DMANODE *theNode;

  PARTLOCK;
  dmalistGet(&(pPart->list),theNode);
  if(!theNode)
    {
      PARTUNLOCK;
      return 0;
    }

  if(theNode->length > theNode->part->size) 
    {
      printf("dmaPGetItem: ERROR.");
      printf("  Event length (%d) is larger than the Event buffer size (%d).  (Event %d)\n",
	     (int)theNode->length,theNode->part->size,
	     (int)theNode->nevent);
    }
  PARTUNLOCK;
  return(theNode);
}

void
dmaPAddItem(DMA_MEM_ID pPart, DMANODE *pItem) 
{

  PARTLOCK;
  dmalistAdd(&(pPart->list),pItem);
  if(pItem->length > pItem->part->size) 
    {
      printf("dmaPAddItem: ERROR.");
      printf("  Event length (%d) is larger than the Event buffer size (%d).  (Event %d)\n",
	     (int)pItem->length,pItem->part->size,
	     (int)pItem->nevent);
    }
  PARTUNLOCK;
}



/************************************************************** 
 * dmaPReInit - Initiialize an existing partition 
 */

int 
dmaPReInit (DMA_MEM_ID pPart)
{
  register char *node;
  register DMANODE *theNode;
  int actual;
  int oldPhysMemBase=0;
  unsigned long oldPartBaseAdr=0;
#ifndef VXWORKS
  GEF_VME_DMA_HDL oldPartDMAHdl,oldNodeDMAHdl;
#endif

  if (pPart == NULL) return -1;

  if (pPart->incr == 1) 
    {   /* Does this partition have a Fragmented buffer list */
      /* Check if partition has buffers that do not belong to it
	 and return them to their rightful owners */
      if ((pPart->total == 0) && (dmalistCount(&pPart->list) > 0)) 
	{
	  while (dmalistCount(&pPart->list) > 0) 
	    {
	      dmalistGet(&pPart->list,theNode);
	      dmaPFreeItem(theNode);
	    }
	}
    
    } 
  else 
    {
      /* Cheat to initialize memory partition assuming buffers in one
	 contiguous memory bloack */
#ifdef VXWORKS
      bzero(*((char **) &pPart->part[0]), pPart->total * pPart->size);
#else
      /* Get the dma handles (if they exist) before they're erased */
      dmalistGet(&pPart->list,theNode);
      if(theNode) 
	{
	  oldNodeDMAHdl = theNode->dmaHdl;
	  oldPhysMemBase = theNode->physMemBase;
	  oldPartBaseAdr = theNode->partBaseAdr;
	} 
      else 
	{
	  oldNodeDMAHdl = 0;
	}
      oldPartDMAHdl = pPart->dmaHdl;
      memset(*((char **) &pPart->part[0]), 0, pPart->total * pPart->size);
#endif
    
      node = *((char **) &pPart->part[0]);
    
      actual = pPart->total;

      pPart->list.f = pPart->list.l = (DMANODE *) (pPart->list.c = 0);
 
      while (actual--)
	{      
	  ((DMANODE *)node)->part = pPart; /* remember where we came from... */
	  ((DMANODE *)node)->dmaHdl = oldNodeDMAHdl;
	  ((DMANODE *)node)->physMemBase = oldPhysMemBase;
	  ((DMANODE *)node)->partBaseAdr = oldPartBaseAdr;
	  dmalistAdd (&pPart->list,(DMANODE *)node);
	  node += pPart->size;
	}
    }
  return 0;
}

int 
dmaPReInitAll()
{
  DMA_MEM_ID	pPart = (DMA_MEM_ID) 0;

  if (dmalistCount(&dmaPList)) 
    {
      pPart = (DMA_MEM_ID) dmalistFirst (&dmaPList);    
      while (pPart != NULL)
	{
	  dmaPReInit(pPart);
	  pPart = (DMA_MEM_ID) dmalistNext ((DMANODE *) pPart);
	}
    }
  return 0;
}

/*************************************************************** 
 * dmaPHdr - Print headings for part statitistics printout 
 */

static void 
dmaPHdr ()
     
{
  printf("Address    total  free   busy   size  incr  (KBytes)  Name\n");
  printf("---------- -----  ----   ----  -----  ----  --------  ----\n");
}


/*************************************************************** 
 * dmaPPrint - Print statitistics for a single part 
 */

static void 
dmaPPrint (pPart)
     DMA_MEM_ID	pPart;    
{
  int freen;

  printf("0x%08x  ",(unsigned int)pPart);

  if (pPart != NULL)
    {
      freen = dmalistCount (&pPart->list); 
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
     DMA_MEM_ID	pPart;
     
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
#ifdef VXWORKS
  int lockKey;
#endif
  DMA_MEM_ID  pPart;
  
  dmaPHdr ();
#ifdef VXWORKS
  lockKey = intLock();
#endif
  pPart = (DMA_MEM_ID) dmalistFirst (&dmaPList);
  while (pPart != NULL)
    {
      dmaPPrint (pPart);
      pPart = (DMA_MEM_ID) dmalistNext ((DMANODE *)pPart);
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
dmaPPrintList(admalist)
     DMALIST *admalist;
{
  DMANODE *theNode;

  printf("dalist->f         %x\n",(unsigned int)admalist->f);
  printf("dalist->l         %x\n",(unsigned int)admalist->l);
  printf("dalist->c         %d\n",(unsigned int)admalist->c);

  theNode = dmalistFirst(admalist);
  while (theNode) 
    {
      printf ("part %x prev %x self %x next %x left %d fd %d\n",
	      (unsigned int)theNode->part,
	      (unsigned int)theNode->p,
	      (unsigned int)theNode,
	      (unsigned int)theNode->n,
	      (int)theNode->left,
	      theNode->fd);
      theNode = dmalistNext(theNode);
    }
  return(0);
}

