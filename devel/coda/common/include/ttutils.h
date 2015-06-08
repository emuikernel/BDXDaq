
/* ttutils.h */

int  *utRingAlloc(int length);
void  utRingFree(int *ring);
int   utRingInit(int *ring);
int  *utRingCreate(int length);
int   utRingWriteWord(int *ring, int word);
int   utRingGetNwords(int *ring);
int   utRingReadWord(int *ring);
void  utRingStatusDraw(int *ring);
void  utRingStatusPrint(int *ring);

#ifdef VXWORKS
STATUS utIntInit(VOIDFUNCPTR handler, int arg, int level, int vec);

unsigned long getAddress(int addr, int am);

void utBootPrint();
void utBootPrintTest();
void utBootGetTargetName(char *tn, int tnlen);

#endif
