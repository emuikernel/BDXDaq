/*
   header file for the memory allocation system 
*/

#ifndef __DMA_MEM_PART__
#define __DMA_MEM_PART__

typedef struct danode			      /* Node of a linked list. */
{
  struct danode         *n;	              /* Points at the next node in the list. */
  struct danode         *p;	              /* Points at the previous node in the list. */
  struct rol_mem_part   *part;	              /* Which partition "owns" this node. */    
  int                    fd;		      /* File descriptor associated with this node. */
  char                  *current;	      /* Current data mark */
  unsigned long          left;	              /* data left to process (bytes) */
  unsigned long          type;                /* data type */
  unsigned long          source;              /* data source */
  void                   (*reader)();         /* routine to read data if data segment is empty */
  long                   nevent;              /* event number */
  unsigned long          length;	      /* Length of data to follow (bytes). */
  unsigned long          data[1];	      /* Node data. */
} DANODE;

typedef struct alist			      /* Header for a linked list. */
{
  DANODE        *f;		              /* head */
  DANODE        *l;		              /* tail */
  int            c;			      /* Number of nodes in list */
  int            to;
  void          (*add_cmd)(struct alist *li);     /* command to call on list add */
  void          *clientData;               /* data to pass for add_cmd */ 
} DALIST;

typedef struct rol_mem_part *ROL_MEM_ID;

typedef struct rol_mem_part{
    DANODE	 node;		/* global partition list */
    DALIST	 list;		/* free item list */
    char	 name[40];	/* part name */
    void         (*free_cmd)(); /* command to call when fifo is empty */
    void         *clientData;   /* data to pass for free_cmd */ 
    int		 size;		/* size of a single item */
    int		 incr;		/* Flag incr=1 when memory pool is fragmented */
    int		 total;		/* total items allocated so far */

    long         part[1];	/* pointer to memory pool */
} ROL_MEM_PART;


/* List manipulation Macros */

#define listInit(li) {bzero((char *) (li), sizeof(DALIST));}

#define listGet(li,no) {\
  (no) = 0;\
  if ((li)->c){\
    (li)->c--;\
    (no) = (li)->f; \
    (li)->f = (li)->f->n;\
  };\
  if (!(li)->c) {\
    (li)->l = 0;\
  }\
}

#define listWait(li,no) {(no) = 0;(li)->to = 0; \
                         while(((li)->c == 0) && ((li)->to == 0)) { \
                                             }; \
                         if ((li)->to == 0) { \
                                              (li)->c--; \
                                              (no) = (li)->f; \
                                              (li)->f = (li)->f->n; \
                                              if (!(li)->c) \
                                                (li)->l = 0; \
                                              } else { \
                                                (no) = (void *) -1; \
                                              }; \
                        }
 
/* call add_cmd (if it exists) whenever a buffer is added to a list */
#define listAdd(li,no) {if(! (li)->c ){(li)->f = (li)->l = (no);(no)->p = 0;} else \
			  {(no)->p = (li)->l;(li)->l->n = (no);(li)->l = (no);} (no)->n = 0;(li)->c++;\
		          if((li)->add_cmd != NULL) (*((li)->add_cmd)) ((li));  }

#define listSnip(li,no) {if ((no)->p) {(no)->p->n =(no)->n;} else {(li)->f = (no)->n;} \
if ((no)->n) {(no)->n->p =(no)->p;} else {(li)->l = (no)->p;} \
(li)->c--;if ((li)->c==0) (li)->f = (li)->l = (DANODE *)0;(no)->p=(no)->n= (DANODE *)0;}

#define listCount(li) ((li)->c)
#define listFirst(li) ((li)->f)
#define listLast(li) ((li)->l)
#define listNext(no) ((no)->n)

/* Event formating macros */

DANODE *the_event;             /* Buffer node pointer */
unsigned int *dma_dabufp;     /* data pointer */

#define GETEVENT(part, num) {the_event = dmaPGetItem(part); \
		             if(the_event == (DANODE *) 0) { \
			        logMsg("TRIGGER ERROR: no pool buffer available\n",0,0,0,0,0); \
                    dmaPStatsAll; \
                                return; \
		             } \
		             dma_dabufp = (unsigned int *) &the_event->length; \
		             the_event->nevent = num; \
			     } \

#define PUTEVENT(part)      {the_event->length = (((int)(dma_dabufp) - (int)(&the_event->length))>>2) - 1; \
                             dmaPAddItem(part,the_event);}


/* Prototypes */
void dmaPartInit();
ROL_MEM_ID dmaPCreate (char *name, int size, int c, int incr);
ROL_MEM_ID dmaPFindByName (char *name);
void dmaPFree(ROL_MEM_ID pPart);
void dmaPFreeAll();
int dmaPIncr ( ROL_MEM_ID pPart, int c);
void dmaPFreeItem(DANODE *pItem);
int dmaPEmpty(ROL_MEM_ID pPart);
DANODE *dmaPGetItem(ROL_MEM_ID pPart);
void dmaPAddItem(ROL_MEM_ID pPart, DANODE *pItem);
int dmaPReInit (ROL_MEM_ID pPart);
int dmaPReInitAll();
int dmaPStats (ROL_MEM_ID pPart);
int dmaPStatsAll();
int dmaPPrintList(DALIST *alist);


#endif






