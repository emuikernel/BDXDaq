#ifndef _NETDB_INCLUDED /* allow multiple inclusions */
#define _NETDB_INCLUDED

struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses from name server */
#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent {
	char		*n_name;	/* official name of net */
	char		**n_aliases;	/* alias list */
	int		n_addrtype;	/* net address type */
	unsigned long	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};

struct rpcent {
  char    *r_name;        /* name of server for this rpc program */
  char    **r_aliases;    /* alias list */
  int     r_number;       /* rpc program number */
};

#ifdef __STDC__
extern struct hostent *gethostbyname(const char *);
extern struct hostent *gethostbyaddr(const char *, int, int);
extern struct hostent *gethostent(void);
extern int sethostent(int);
extern int endhostent(void);
extern struct netent *getnetbyname(const char *);
extern struct netent *getnetbyaddr(int, int);
extern struct netent *getnetent(void);
extern int setnetent(int);
extern int endnetent(void);
extern struct servent *getservbyname(const char *, const char *);
extern struct servent *getservbyport(int, const char *);
extern struct servent *getservent(void);
extern int setservent(int);
extern int endservent(void);
extern struct protoent *getprotobyname(const char *);
extern struct protoent *getprotobynumber(int);
extern struct protoent *getprotoent(void);
extern int setprotoent(int);
extern int endprotoent(void);
extern struct rpcent *getrpcent(void);
extern struct rpcent *getrpcbyname(const char *);
extern struct rpcent *getrpcbynumber(int);
extern int setrpcent(int);
extern int endrpcent(void);
#else 
extern struct hostent *gethostbyname();
extern struct hostent *gethostbyaddr();
extern struct hostent *gethostent();
extern int sethostent();
extern int endhostent();
extern struct netent *getnetbyname();
extern struct netent *getnetbyaddr();
extern struct netent *getnetent();
extern int setnetent();
extern int endnetent();
extern struct servent *getservbyname();
extern struct servent *getservbyport();
extern struct servent *getservent();
extern int setservent();
extern int endservent();
extern struct protoent *getprotobyname();
extern struct protoent *getprotobynumber();
extern struct protoent *getprotoent();
extern int setprotoent();
extern int endprotoent();
extern struct rpcent *getrpcent();
extern struct rpcent *getrpcbyname();
extern struct rpcent *getrpcbynumber();
extern int setrpcent();
extern int endrpcent();
#endif 

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define	HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	TRY_AGAIN	2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA		4 /* Valid name, no data record of requested type */
#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */

#endif /* _NETDB_INCLUDED */
