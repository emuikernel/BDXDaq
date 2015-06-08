
/* params.h - contains board name, address and interrupt level/vectors, */
/*            must be replaced by user information during installation */

#ifdef VXWORKSPPC
/* AM=0x29 -> 0xfbffxxxx */
/* AM=0x39 -> 0xfaxxxxxx */
#define CARD_ADDR	0xFBFF8000	/* VMEbus address of 6016 card (A16) */
#define SLAVE_ADDR  0xFA800000  /* VMEbus address of 6016 card buffer (A24) */
#else
/* AM=0x29 -> 0xffffxxxx */
/* AM=0x39 -> 0xf0xxxxxx */
#define CARD_ADDR	0xFFFF8000	/* VMEbus address of 6016 card (A16) */
#define SLAVE_ADDR  0xF0800000  /* VMEbus address of 6016 card buffer (A24) */
#endif

#define DEV_NAME	/*"/tyIP/"*/ "/tty6016/" /* Base name of devices */
#define INTLEVEL1   5           /* error interrupt level */
#define INTVECTOR1  0x44        /* error interrupt vector */
#define INTLEVEL2   6           /* channel interrupt level */
#define INTVECTOR2  0x48        /* channel interrupt vector */
