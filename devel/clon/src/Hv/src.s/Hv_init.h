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
