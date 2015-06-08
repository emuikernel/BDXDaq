/* win_Lib.h - simulator UNIX/VxSim interface header */
    
/* Copyright 1998-2001 Wind River Systems, Inc */

/*
modification history
--------------------
01d,18dec02,jeg  add function wrapper headers and define WIN_LARGE_INTEGER 
		 type for performance counter.
01c,03sep01,hbh  Fixed field types for win_stat structure (SPR 29146 & 65372).
01b,29apr98,cym  added file stat/find structures and exceptions.
01a,31jan98,jmb  written.
*/

/*
DESCRIPTION
This header contains the prototype declarations for all Win32
calls made by VxSim outside of simLib.c
*/

#ifndef	INCwin_Libh
#define	INCwin_Libh

/* This typedef doesn't really belong here */

typedef struct ntsim_int_vec
{  
    int message;
    void (*handler)();
    int arg;
} NTSIM_INT_VEC;

#define INT_WAS_IN_PROGRESS 0x1000000

#define WIN_INFINITE 0xFFFFFFFF

typedef int 	     WIN_BOOL;
typedef unsigned int WIN_DWORD;
typedef WIN_DWORD *  WIN_LPDWORD;
typedef void *       WIN_HANDLE;
typedef void *       WIN_LPVOID;
typedef const void * WIN_LPCVOID;
typedef long long    WIN_LARGE_INTEGER;

typedef struct _WIN_FLOATING_SAVE_AREA
{
    WIN_DWORD ControlWord;
    WIN_DWORD StatusWord;
    WIN_DWORD TagWord;
    WIN_DWORD ErrorOffset;
    WIN_DWORD ErrorSelector;
    WIN_DWORD DataOffset;
    WIN_DWORD DataSelector;
    WIN_DWORD RegisterArea[80];
    WIN_DWORD Cr0NpxState;

} WIN_FLOATING_SAVE_AREA;

typedef struct _WIN_CONTEXT
{
    WIN_DWORD ContextFlags;
    WIN_DWORD Dr0;
    WIN_DWORD Dr1;
    WIN_DWORD Dr3;
    WIN_DWORD Dr6;
    WIN_DWORD Dr7;
  
    WIN_FLOATING_SAVE_AREA FloatSave;

    WIN_DWORD SegGs;
    WIN_DWORD SegFs;
    WIN_DWORD SegEs;
    WIN_DWORD SegDs;

    WIN_DWORD Edi;
    WIN_DWORD Esi;
    WIN_DWORD Ebx;
    WIN_DWORD Edx;
    WIN_DWORD Ecx;
    WIN_DWORD Eax;

    WIN_DWORD Ebp;
    WIN_DWORD Eip;
    WIN_DWORD SegCs;
    WIN_DWORD Eflags;
    WIN_DWORD Esp;
    WIN_DWORD SegSs;
    
} WIN_CONTEXT,*WIN_LPCONTEXT;

struct win_stat {
    short 		st_dev;
    short	 	st_ino;
    unsigned short 	st_mode;
    short 		st_nlink;
    int			st_uid;
    short		st_gid;
    short	 	st_rdev;
    long 		st_size;
    long		st_atime;
    long		st_mtime;
    long		st_ctime;
    };

typedef struct _WIN_FILETIME {
    DWORD LowTime;
    DWORD HighTime;
    } WIN_FILETIME;

typedef struct _WIN_FIND_DATA {
    WIN_DWORD 		attrib;
    WIN_FILETIME	tCreate;
    WIN_FILETIME	tAccess;
    WIN_FILETIME	tWrite;
    WIN_DWORD		szHigh;
    WIN_DWORD		szLow;
    WIN_DWORD		reserved0;
    WIN_DWORD		reserved1;
    char		name[ 260 ];
    char		altName[ 14 ];
    }WIN_FIND_DATA;

/* Only use __stdcall under Win32 compiler */

#ifdef i386
#define WIN_STDCALL  __attribute__ ((stdcall))
#define WIN_WINAPI   WIN_STDCALL
#define WIN_CALLBACK WIN_WINAPI
#else
#define WIN_CALLBACK __stdcall
#endif

typedef WIN_DWORD (WIN_CALLBACK *WIN_LPTHREAD_START_ROUTINE) (WIN_LPVOID);

typedef struct  _WIN_SECURITY_ATTRIBUTES
{
    WIN_DWORD nLength;
    WIN_LPVOID lpSecurityDescriptors;
    WIN_BOOL bInheritHandle;

} *WIN_LPSECURITY_ATTRIBUTES;

#ifdef __STDC__

extern WIN_DWORD win_SetTimer(unsigned int id,unsigned int msecs);
extern WIN_BOOL win_KillTimer(unsigned int id);
extern WIN_BOOL win_FindNextFile(HANDLE hFind, WIN_FIND_DATA *findData);
extern WIN_HANDLE win_FindFirstFile(char *searchStr, WIN_FIND_DATA *findData);
extern WIN_BOOL win_FindClose(HANDLE hFind);
extern WIN_BOOL win_WaitMessage (void);
extern WIN_BOOL win_GetThreadContext (WIN_HANDLE hThread,WIN_LPCONTEXT lpContext);
extern WIN_BOOL win_SetThreadContext (WIN_HANDLE hThread,WIN_LPCONTEXT lpContext);
extern WIN_HANDLE win_CreateThread (WIN_LPSECURITY_ATTRIBUTES security, WIN_DWORD stack,\
    WIN_LPTHREAD_START_ROUTINE start, WIN_LPVOID param, WIN_DWORD flags, WIN_LPDWORD threadId);
extern WIN_DWORD win_ResumeThread (WIN_HANDLE hThread);
extern WIN_DWORD win_SuspendThread (WIN_HANDLE hThread);
extern WIN_HANDLE win_GetCurrentThread (void);
extern WIN_HANDLE win_GetCurrentProcess (void);
extern WIN_DWORD win_SemTake (WIN_HANDLE sem);
extern WIN_BOOL win_SemGive (WIN_HANDLE sem);
extern WIN_BOOL win_FlushInstructionCache(WIN_HANDLE process, WIN_LPCVOID addr, WIN_DWORD bytes);
extern WIN_BOOL wind_QueryPerformanceFrequency (WIN_LARGE_INTEGER * lpFrequency);
extern WIN_BOOL wind_QueryPerformanceCounter (WIN_LARGE_INTEGER * lpPerformanceCount);
extern void *u_malloc (int size);


#else

extern WIN_DWORD win_SetTimer();
extern WIN_BOOL win_KillTimer();
extern WIN_BOOL win_WaitMessage ();
extern WIN_BOOL win_GetThreadContext ();
extern WIN_BOOL win_SetThreadContext ();
extern WIN_HANDLE win_CreateThread ();
extern WIN_DWORD win_ResumeThread ();
extern WIN_DWORD win_SuspendThread ();
extern WIN_HANDLE win_GetCurrentThread ();
extern WIN_HANDLE win_GetCurrentProcess ();
extern WIN_DWORD win_WinSemTake ();
extern WIN_BOOL win_WinSemGive ();
extern WIN_BOOL win_FlushInstructionCache ();
extern WIN_BOOL wind_QueryPerformanceFrequency ();
extern WIN_BOOL wind_QueryPerformanceCounter ();
extern void *u_malloc ();

#endif	/* __STDC__ */

#endif	/* INCwin_Libh */
