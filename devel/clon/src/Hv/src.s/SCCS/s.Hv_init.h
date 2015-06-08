h52137
s 00000/00000/00000
d R 1.2 02/09/09 11:30:32 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_init.h
e
s 00021/00000/00000
d D 1.1 02/09/09 11:30:31 boiarino 1 0
c date and time created 02/09/09 11:30:31 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef __HVINITH__
#define __HVINITH__

extern void            Hv_HandleFunctionKey(short x,
					    short y,
					    int);

extern void            Hv_InitAppName(char **,
				      char *);

extern void            Hv_MainInit(void);

extern void            Hv_LastStageInitialization(void);

extern void            Hv_SetFKeysActive(Boolean);

extern void            Hv_AddFunctionKeyCallback(int,
						 Hv_FunctionPtr);


#endif
E 1
