/*
   mempart.h - header file for the memory allocation system 
*/

#ifndef __MEM_PART__
#define __MEM_PART__



#ifndef VXWORKS

typedef void (*VOIDFUNCPTR) ();
typedef int (*FUNCPTR) ();

typedef void (*VOIDFUNCPTR1) (unsigned int);
typedef int (*FUNCPTR1) (unsigned int, unsigned int);
typedef int (*FUNCPTR2) (unsigned int);
typedef int (*FUNCPTR3) ();
typedef int (*FUNCPTR4) (unsigned int);

#endif



typedef struct danode			    /* Node of a linked list */
{
  struct danode        *n;	        /* Points at the next node in the list */
  struct danode        *p;	        /* Points at the previous node in the list */
  struct rol_mem_part  *owner;      /* Which partition "owns" this node */
                                                        int bla1;
                                                        int bla2;
                                                        int bla3;
                                                        unsigned int bla_type;
                                                        unsigned int bla_source;

#ifdef VXWORKS
                                                        FUNCPTR bla_reader;
#else
                                                        FUNCPTR1 bla_reader;
#endif

  int                 nevent;      /* event number */

  /* start of CODA fragment: 'rol->dabufpi' points here !! */
  unsigned int        length;	    /* Length of data to follow (bytes) */
  unsigned int        data[1];	    /* Node data */
} DANODE;

typedef struct dalist   /* Header for a linked list */
{
  DANODE *f;                       /* head */
  DANODE *l;                       /* tail */
  int     c;                       /* Number of nodes in list */
                                                        int bla4; 
                                                        int bla5; 
                                                        int bla6; 
} DALIST;

typedef struct rol_mem_part *ROL_MEM_ID;
typedef struct rol_mem_part
{
  DANODE  node;       /* global partition list */
  /* Sergey: that element is used undirectly, so do not remove it ! */
  /*         you will never find statements like 'something->node'
  in the code, but that element actually used in partCreate() function
  in call 'listAdd(&partList,(DANODE *)pPart)' (maybe somewhere else);
  in that function 'pPart' actually is type 'ROL_MEM_PART' so when
  we are casting it to DANODE we assume that DANODE-type element is first
  in pPart structure */

  DALIST  list;           /*  */

                                                        char bla7[40];
                                                        int bla8;
                                                        int bla9;
  int     size;           /* size of a single item */
                                                        int bla10;
  int     total;          /* total items allocated so far */
  int    part[1];        /* pointer to memory pool */

} ROL_MEM_PART;


/******************************************/
/* 2 following macroses are needed by crl */
/******************************************/

/* get event buffer from the pool */
#define listGet(li, no) \
{ \
  (no) = 0; \
  if ((li)->c) /* if at least one event buffer available */ \
  { \
    (li)->c--; /* decrement event buffer counter */ \
    (no) = (li)->f; /* get pointer to the event buffer */ \
    (li)->f = (li)->f->n; /* bump pointer to the next event buffer */ \
  } \
  if (!(li)->c) /* if no event buffers left .. */ \
  { \
    (li)->l = 0; /* .. set 'last' pointer to zero */ \
  } \
}

#define listGet1(li, no) \
{ \
/*if((li)->c > 198) logMsg("listGet1: c=%d\n",(li)->c,2,3,4,5,6);*/ \
  (no) = 0; \
  if ((li)->c) /* if at least one event buffer available */ \
  { \
    (li)->c--; /* decrement event buffer counter */ \
    (no) = (li)->f; /* get pointer to the event buffer */ \
    (li)->f = (li)->f->n; /* bump pointer to the next event buffer */ \
  } \
  if (!(li)->c) /* if no event buffers left .. */ \
  { \
/*logMsg("listGet1: no event buffers left, c=%d f=0x%8x l=0x%8x\n",(li)->c,(li)->f,(li)->l,4,5,6);*/ \
    (li)->l = 0; /* .. set 'last' pointer to zero */ \
  } \
}

/* add event buffer to the pool */ 
#define listAdd(li, no) \
{ \
  if(! (li)->c ) /* if pool is empty .. */ \
  { \
    (li)->f = (li)->l = (no); /* .. set 'first' and 'last' pointers */ \
    (no)->p = 0; /* .. and set 'previous' to zero */ \
  } \
  else /* pool is not empty */ \
  { \
    (no)->p = (li)->l; \
    (li)->l->n = (no); \
    (li)->l = (no); \
  } \
  (no)->n = 0; \
  (li)->c++; /* increment event buffer counter */ \
}


#ifdef	__cplusplus
extern "C" {
#endif

/* libpart.c function prototypes */

void       libPartInit1();
ROL_MEM_ID partCreate1(int ibuf);
void       libPartInit();
int        partIncr(ROL_MEM_ID pPart, int c);
ROL_MEM_ID partCreate(char *name, int size, int c, int incr);
int        partReInitAll();

#ifdef	__cplusplus
}
#endif


#endif






