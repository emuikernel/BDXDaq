h33447
s 00009/00001/00012
d D 1.2 03/04/17 17:20:25 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 03/02/17 16:12:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/tty6016/params.h
e
s 00013/00000/00000
d D 1.1 03/02/17 16:12:58 boiarino 1 0
c date and time created 03/02/17 16:12:58 by boiarino
e
u
U
f e 0
t
T
I 1

/* params.h - contains board name, address and interrupt level/vectors, */
/*            must be replaced by user information during installation */

I 3
#ifdef VXWORKSPPC
E 3
/* AM=0x29 -> 0xfbffxxxx */
/* AM=0x39 -> 0xfaxxxxxx */
D 3
#define DEV_NAME	"/tyIP/" /*"v6016"*/		/* Base name of devices */
E 3
#define CARD_ADDR	0xFBFF8000	/* VMEbus address of 6016 card (A16) */
#define SLAVE_ADDR  0xFA800000  /* VMEbus address of 6016 card buffer (A24) */
I 3
#else
/* AM=0x29 -> 0xffffxxxx */
/* AM=0x39 -> 0xf0xxxxxx */
#define CARD_ADDR	0xFFFF8000	/* VMEbus address of 6016 card (A16) */
#define SLAVE_ADDR  0xF0800000  /* VMEbus address of 6016 card buffer (A24) */
#endif

#define DEV_NAME	/*"/tyIP/"*/ "/tty6016/" /* Base name of devices */
E 3
#define INTLEVEL1   5           /* error interrupt level */
#define INTVECTOR1  0x44        /* error interrupt vector */
#define INTLEVEL2   6           /* channel interrupt level */
#define INTVECTOR2  0x48        /* channel interrupt vector */
E 1
