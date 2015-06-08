/* ledLib.h - header file for ledLib.c */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,06apr98,jmp  added shellPromptStrGet(), shellPromptStrSet(),
		  shellTclModeSet(), shellTclModeGet().
01g,30may95,p_m  added Solaris-2 and HP-UX  support.
01f,18jan95,c_s  added ledIdle prototypes.
01e,22sep92,rrr  added support for c++
01d,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCledLibh
#define __INCledLibh

#ifdef __cplusplus
extern "C" {
#endif

#define	LINE_LEN	400		/* >= MAX_SHELL_LINE!		*/

typedef struct	/* HIST */
    {
    NODE node;
    char line [LINE_LEN+1];
    } HIST;

typedef struct	/* LED */
    {
    int		inFd;
    int		outFd;
    int		histSize;
    LIST	histFreeList;
    LIST	histList;
    HIST *	pHist;
    int		idle;		/* whether we're sitting in read () */

    /*
     * XXX the following are not needed between ledRead's
     * but are used in support routines.
     */

    char *	buffer;		/* hold deletions from curLn */
    int		histNum;	/* current history number */
    BOOL	cmdMode;	/* insert or command mode */
     
    /* the following is needed for select loop support */
    void		(*callback)(void);
    struct timeval      timeout;
    fd_set	      readfds;

    } LED;

typedef LED *LED_ID;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	ledClose (int led_id);
extern int      ledOpen (int inFd, int outFd, int histSize, struct timeval
			 *pTimeout, void (*callback)(void));
extern int 	ledRead (int led_id, char *string, int maxBytes);
extern int 	ledIdle (int led_id);
extern void 	ledControl (int led_id, int inFd, int outFd, int histSize);
extern char *	shellPromptStrGet (void);
extern void	shellPromptStrSet (char * shPrompt);
extern void	shellTclModeSet (int shellTclMode);
extern int	shellTclModeGet (void);

#else	/* __STDC__ */

extern STATUS 	ledClose ();
extern int 	ledOpen ();
extern int 	ledRead ();
extern int 	ledIdle ();
extern void 	ledControl ();
extern char *	shellPromptStrGet ();
extern void	shellPromptStrSet ();
extern void	shellTclModeSet();
extern int	shellTclModeGet();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCledLibh */
