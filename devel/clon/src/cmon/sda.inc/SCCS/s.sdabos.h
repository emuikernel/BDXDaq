h33022
s 00000/00000/00000
d R 1.2 01/11/19 19:00:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdabos.h
e
s 00012/00000/00000
d D 1.1 01/11/19 19:00:37 boiarino 1 0
c date and time created 01/11/19 19:00:37 by boiarino
e
u
U
f e 0
t
T
I 1

/* Basic BOS array include file */

#define NBCS 700000

typedef struct boscommon
{
  int ipad[5];
  int iw[NBCS];
} BOScommon;

BOScommon       bcs_;
E 1
