/* simLib.h - miscellaneous simulator routines header */

/*
modification history
--------------------
02i,19dec01,jmp  added simReadSelect().
02h,04apr97,kab  added HPSIM ioctls, added more open() flags. (HPSIM merge,
                 18mar98.)
02g,26mar97,jmb  added macros for converting file modes in HPSIM. (HPSIM merge,
                 18mar98.)
02f,21mar97,cth  added forward declaration for s_ulipIoctl (SPR 8221, 8240)
02e,29jan97,pr   moved conversion code from simLib.c into a macro
02d,09jan97,pr   added macros for converting file modes in simsolaris
02c,14dec93,gae  removed u_printf() declaration (u_Lib.c) conflict for non stdc.
02b,14jul93,gae  fixed banner.
02a,09jan93,gae  renamed from sysULib.h; discarded major portions.
01c,05aug92,gae  added non-ANSI forward declarations.
01b,29jul92,gae  added more prototypes; ANSIfied.
01a,04jun92,gae  written.
*/

/*
This header contains the declarations for miscellaneous simulator routines.
*/

#ifndef	INCsimLibh
#define	INCsimLibh

/* prototypes */

#ifdef __STDC__

extern void s_userGet (char *hostname, char *hostip, char *username, char *cwd);
extern void s_fdint (int fd, int raw);
extern void s_uname (char *sysname, char *nodename, char *release,
		     char *version, char *machine);
extern int simReadSelect (int u_fd, int tv_sec, int tv_usec);

#if CPU==SIMSPARCSOLARIS
extern unsigned int solarisCvtFlags (unsigned int oFlags);
extern int s_ulipIoctl (int fd, int cmd, char *pData, int len);
#endif  /* CPU==SIMSPARCSOLARIS */

#else	/* __STDC__ */

extern void s_userGet ();
extern void s_fdint ();
extern void s_uname ();
extern int simReadSelect ();

#if CPU==SIMSPARCSOLARIS
extern unsigned int solarisCvtFlags ();
extern int s_ulipIoctl ();
#endif  /* CPU==SIMSPARCSOLARIS */

#endif	/* __STDC__ */

/*
 * VxWorks, SunOS and HPUX currently all use the same defines for most
 * of the file modes.  Local versions of the modes are converted using yhe
 * following macros so that they can be used instead of hard coded values.
 */

#if CPU==SIMSPARCSOLARIS

#define L_RDONLY        0x000
#define L_WRONLY        0x001
#define L_RDWR          0x002
#define L_NDELAY        0x004
#define L_APPEND        0x008
#define L_SYNC          0x010
#define L_DSYNC         0x040
#define L_NONBLOCK      0x080
#define L_CREAT         0x100
#define L_TRUNC         0x200
#define L_EXCL          0x400
#define L_NOCTTY        0x800
#define L_RSYNC         0x8000

#define L_VXSTART	0x4000

#define ARCHCVTFLAGS(x) ((x&O_RDONLY ? L_RDONLY : 0) | (x&O_RDWR ? L_RDWR : 0)|\
			(x&O_WRONLY ? L_WRONLY : 0) | (x&O_SYNC ? L_SYNC : 0)|\
			(x&O_NDELAY ? L_NDELAY : 0)| (x&O_CREAT ? L_CREAT : 0)|\
			(x&O_APPEND ? L_APPEND : 0) | (x&O_TRUNC ? L_TRUNC:0)|\
			(x&O_EXCL ? L_EXCL : 0) | (x&O_NOCTTY ? L_NOCTTY : 0)|\
			(x&O_NONBLOCK ? L_NONBLOCK : 0))

#endif /* CPU==SIMSPARCSOLARIS */

#endif	/* INCsimLibh */
