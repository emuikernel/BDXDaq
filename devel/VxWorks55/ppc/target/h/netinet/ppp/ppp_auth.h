/* ppp_auth.h - PPP Authentication header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,21jun95,dzb  added externs for unbundling of crypt().
01c,16jun95,dzb  renamed to ppp_auth.h.  added header file inclusion.
01b,11may95,dzb  moved in defines from auth.c.  added NONWILD macro.
01a,19jan95,dzb  written.
*/

#ifndef	__INCppp_authh
#define	__INCppp_authh

#ifdef	__cplusplus
extern "C" {
#endif

/* includes */

#include "stdio.h"

/* defines */

/* Bits in scan_authfile return value */

#define NONWILD_SERVER  1
#define NONWILD_CLIENT  2
#define NONWILD		(NONWILD_SERVER | NONWILD_CLIENT)
 
#define ISWILD(word)    (word[0] == '*' && word[1] == 0)

extern FUNCPTR	pppCryptRtn;

/* function declarations */
 
#if defined(__STDC__) || defined(__cplusplus)
  
extern void	link_required (int unit);
extern void	link_terminated (int unit);
extern void	link_down (int unit);
extern void	link_established (int unit);
extern void	auth_peer_fail (int unit, int protocol);
extern void	auth_peer_success (int unit, int protocol);
extern void	auth_withpeer_fail (int unit, int protocol);
extern void	auth_withpeer_success (int unit, int protocol);
extern void	check_auth_options (void);
extern int	check_passwd (int unit, char *auser, int userlen, char *apasswd,
                    int passwdlen, char **msg, int *msglen);
extern int	get_secret (int unit, char *client, char *server, char *secret,
                    int *secret_len, int save_addrs);
extern int	auth_ip_addr (int unit, u_long addr);
extern int	bad_ip_adrs (u_long addr);
extern void	check_access (FILE *f, char *fileName);

extern void	cryptRtnInit (FUNCPTR *cryptRtnHook);

#else	/* __STDC__ */

extern void	link_required ();
extern void	link_terminated ();
extern void	link_down ();
extern void	link_established ();
extern void	auth_peer_fail ();
extern void	auth_peer_success ();
extern void	auth_withpeer_fail ();
extern void	auth_withpeer_success ();
extern void	check_auth_options ();
extern int	check_passwd ();
extern int	get_secret ();
extern int	auth_ip_addr ();
extern int	bad_ip_adrs ();
extern void	check_access ();

extern void	cryptRtnInit ();

#endif	/* __STDC__ */

#ifdef	__cplusplus
}
#endif

#endif	/* __INCppp_authh */
