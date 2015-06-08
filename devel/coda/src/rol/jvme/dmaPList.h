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
 *     Header for a memory allocation system
 *
 * SVN: $Rev: 393 $
 *
 *----------------------------------------------------------------------------*/

#ifndef __DMAPLIST__
#define __DMAPLIST__
#include <stdarg.h>
#include <pthread.h>
#include "gef/gefcmn_vme.h"
#include "gef/gefcmn_vme_framework.h"

typedef struct dmanode			      /* Node of a linked list. */
{
  struct dmanode         *n;	              /* Points at the next node in the list. */
  struct dmanode         *p;	              /* Points at the previous node in the list. */
  struct dma_mem_part   *part;	              /* Which partition "owns" this node. */    
  int                    fd;		      /* File descriptor associated with this node. */
  GEF_VME_DMA_HDL        dmaHdl;
  char                  *current;	      /* Current data mark */
  unsigned long          left;	              /* data left to process (bytes) */
  unsigned long          type;                /* data type */
  unsigned long          partBaseAdr;         /* Base Address of the partition owning this node */
  unsigned long          physMemBase;         /* Physical Base address of the node */
  void                   (*reader)();         /* routine to read data if data segment is empty */
  long                   nevent;              /* event number */
  unsigned long          length;	      /* Length of data to follow (bytes). */
  unsigned long          data[1];	      /* Node data. */
} DMANODE;

typedef struct admalist			      /* Header for a linked list. */
{
  DMANODE        *f;		              /* head */
  DMANODE        *l;		              /* tail */
  int             c;			      /* Number of nodes in list */
  int            to;
  void          (*add_cmd)(struct admalist *li);     /* command to call on list add */
  void          *clientData;               /* data to pass for add_cmd */ 
} DMALIST;

typedef struct dma_mem_part *DMA_MEM_ID;

typedef struct dma_mem_part
{
  DMANODE	 node;		/* global partition list */
  DMALIST	 list;		/* free item list */
  char	         name[40];	/* part name */
  GEF_VME_DMA_HDL dmaHdl;   
  void         (*free_cmd)();   /* command to call when fifo is empty */
  void          *clientData;    /* data to pass for free_cmd */ 
  int		 size;		/* size of a single item */
  int		 incr;		/* Flag incr=1 when memory pool is fragmented */
  int		 total;		/* total items allocated so far */

  long         part[1];	/* pointer to memory pool */
} DMA_MEM_PART;

/* List manipulation Macros */

#ifdef VXWORKS
#define dmalistInit(li) {bzero((char *) (li), sizeof(DMALIST));}
#else
#define dmalistInit(li) {memset((char *) (li), 0, sizeof(DMALIST));}
#endif

#define dmalistGet(li,no) {			\
    (no) = 0;					\
    if ((li)->c){				\
      (li)->c--;				\
      (no) = (li)->f;				\
      (li)->f = (li)->f->n;			\
    };						\
    if (!(li)->c) {				\
      (li)->l = 0;				\
    }						\
  }

#define dmalistWait(li,no) {					    \
    (no) = 0;(li)->to = 0;					    \
    while(((li)->c == 0) && ((li)->to == 0)) {			    \
    };								    \
    if ((li)->to == 0) {					    \
      (li)->c--;						    \
      (no) = (li)->f;						    \
      (li)->f = (li)->f->n;					    \
      if (!(li)->c)						    \
	(li)->l = 0;						    \
    } else {							    \
      (no) = (void *) -1;					    \
    };								    \
  }
 
/* call add_cmd (if it exists) whenever a buffer is added to a list */
#define dmalistAdd(li,no) { \
    if(! (li)->c ) {							\
      (li)->f = (li)->l = (no);						\
      (no)->p = 0;							\
    } else {								\
      (no)->p = (li)->l;						\
      (li)->l->n = (no);						\
      (li)->l = (no);							\
    }									\
    (no)->n = 0;							\
    (li)->c++;								\
    if((li)->add_cmd != NULL)						\
      (*((li)->add_cmd)) ((li));					\
  }

#define dmalistSnip(li,no) {						\
    if ((no)->p) {							\
      (no)->p->n =(no)->n;						\
    } else {								\
      (li)->f = (no)->n;						\
    }									\
    if ((no)->n) {							\
      (no)->n->p =(no)->p;						\
    } else {								\
      (li)->l = (no)->p;						\
    }									\
    (li)->c--;								\
    if ((li)->c==0)							\
      (li)->f = (li)->l = (DMANODE *)0;					\
    (no)->p=(no)->n= (DMANODE *)0;					\
  }

#define dmalistCount(li) ((li)->c)
#define dmalistFirst(li) ((li)->f)
#define dmalistLast(li) ((li)->l)
#define dmalistNext(no) ((no)->n)

/* Event formating macros */

DMANODE *the_event;             /* Buffer node pointer */
unsigned int *dma_dabufp;     /* data pointer */

#define GETEVENT(part, num) {						\
    the_event = dmaPGetItem(part);					\
    if(the_event == (DMANODE *) 0) {					\
      logMsg("DMA BUFFER ERROR: no pool buffer available for part %s\n", \
	     part->name,0,0,0,0);					\
    } else {								\
      dma_dabufp = (unsigned int *) &(the_event->data[0]);		\
      the_event->nevent = num;						\
    }									\
  }									\
    
#define PUTEVENT(part)      {						\
    the_event->length =							\
      (((int)(dma_dabufp) - (int)(&the_event->length))>>2) - 1;		\
    dmaPAddItem(part,the_event);					\
  }									\


/* Prototypes */
GEF_UINT32 dmaHdl_to_PhysAddr(GEF_VME_DMA_HDL inpDmaHdl);
void dmaPartInit();
DMA_MEM_ID dmaPCreate (char *name, int size, int c, int incr);
DMA_MEM_ID dmaPFindByName (char *name);
void dmaPFree(DMA_MEM_ID pPart);
void dmaPFreeAll();
int dmaPIncr ( DMA_MEM_ID pPart, int c);
void dmaPFreeItem(DMANODE *pItem);
int dmaPEmpty(DMA_MEM_ID pPart);
DMANODE *dmaPGetItem(DMA_MEM_ID pPart);
void dmaPAddItem(DMA_MEM_ID pPart, DMANODE *pItem);
int dmaPReInit (DMA_MEM_ID pPart);
int dmaPReInitAll();
int dmaPStats (DMA_MEM_ID pPart);
int dmaPStatsAll();
int dmaPPrintList(DMALIST *admalist);


#endif






