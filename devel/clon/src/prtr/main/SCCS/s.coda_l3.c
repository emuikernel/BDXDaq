h43418
s 00000/00000/00009
d D 1.4 09/10/20 11:49:55 boiarino 4 3
c *** empty log message ***
e
s 00001/00001/00008
d D 1.3 09/08/17 16:47:30 boiarino 3 2
c define DELREP
c 
e
s 00001/00001/00008
d D 1.2 08/03/28 09:35:22 boiarino 2 1
c disable delay reporting
c 
e
s 00009/00000/00000
d D 1.1 07/06/28 23:25:53 boiarino 1 0
c date and time created 07/06/28 23:25:53 by boiarino
e
u
U
f e 0
t
T
I 1
#define L3LIB

/*activate NOT MULTITHREADED part !!!*/
#define L3NOTHREADS

/* activate corrections for helicity delayed-reporting */
D 2
#define DELREP
E 2
I 2
D 3
#undef DELREP
E 3
I 3
#define DELREP
E 3
E 2

#include "sinclude/coda_prtr.c"
E 1
