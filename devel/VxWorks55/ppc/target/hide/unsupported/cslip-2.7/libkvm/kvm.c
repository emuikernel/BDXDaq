#ifndef lint
static char rcsid[] =
    "@(#) $Header: kvm.c,v 1.11 92/10/19 14:54:32 leres Exp $ (LBL)";
#endif
/*
 * Copyright (c) 1990, 1991, 1992 Craig Leres
 * Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* BSD emulation of the SunOS 4 kvm_* routines */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/vmmac.h>
#include <sys/file.h>

#include <machine/pte.h>

#include <errno.h>
#include <kvm.h>
#include <nlist.h>
#include <paths.h>
#include <stdio.h>

#ifdef sun
#define KERNBASE KERNELBASE
#endif

static struct nlist nl_sys[] = {
#ifdef sun
/* Sun 3's just have to be different ... */
#define N_SYSSIZE 0
	{ "_Syssize" },
#define N_SYSMAP 1
	{ "_Sysmap" },
#else
#define N_SYSSIZE 0
	{ "_syssize" },
#define N_SYSMAP 1
	{ "_sysmap" },
#endif
	{ "" },
};

extern char *malloc();

extern int errno;
extern long lseek();

kvm_t *
kvm_open(namelist, corefile, swapfile, flag, errstr)
	char *namelist, *corefile, *swapfile;
	int flag;
	char *errstr;
{
	register int i;
	register kvm_t *kd;
	char *memfile;
	int kflag;

	if ((kd = (kvm_t *)malloc(sizeof(kvm_t))) == 0) {
		if (errstr != NULL)
			fprintf(stderr, "%s: malloc failed\n", errstr);
		return (NULL);
	}
	kd->k_vd = -1;
	kd->k_kd = -1;
	kd->k_md = -1;
	kd->k_sd = -1;
	kd->k_sysmap = NULL;
	kd->k_prog = errstr;
	if (namelist == NULL)
		namelist = _PATH_UNIX;
	memfile = NULL;
	if (corefile == NULL) {
		corefile = _PATH_KMEM;
		memfile = _PATH_MEM;
	}
	if (swapfile == NULL)
		swapfile = _PATH_DRUM;
	kflag = (strcmp(namelist, _PATH_UNIX) != 0 ||
	    strcmp(corefile, _PATH_KMEM) != 0);
	kd->k_namelist = namelist;

	if ((kd->k_vd = open(namelist, O_RDONLY)) < 0) {
		i = errno;
		if (kd->k_prog != NULL) {
			fprintf(stderr, "%s: ", kd->k_prog);
			perror(namelist);
		}
		(void)kvm_close(kd);
		errno = i;
		return (NULL);
	}
	if ((kd->k_kd = open(corefile, flag)) < 0) {
		i = errno;
		if (kd->k_prog != NULL) {
			fprintf(stderr, "%s: ", kd->k_prog);
			perror(corefile);
		}
		(void)kvm_close(kd);
		errno = i;
		return (NULL);
	}
	if (kflag) {
		off_t off;
		u_int size;

		if (kvm_nlist(kd, nl_sys) != 0) {
			i = errno;
			(void)kvm_close(kd);
			errno = i;
			return (NULL);
		}
		size = nl_sys[N_SYSSIZE].n_value * sizeof(struct pte);
		kd->k_sysmap = (struct pte *)malloc(size);
		if (kd->k_sysmap == 0) {
			i = errno;
			(void)kvm_close(kd);
			if (kd->k_prog != NULL)
				fprintf(stderr, "%s: malloc failed (2)\n",
				    kd->k_prog);
			errno = i;
			return (NULL);
		}
		off = nl_sys[N_SYSMAP].n_value - KERNBASE;
		(void)lseek(kd->k_kd, off, L_SET);
		(void)read(kd->k_kd, (char *)kd->k_sysmap, (int)size);
	} else {
		if ((kd->k_sd = open(swapfile, O_RDONLY)) < 0) {
			if (kd->k_prog != NULL) {
				fprintf(stderr, "%s: ", kd->k_prog);
				perror(swapfile);
			}
			/* not fatal */
		}
		if (memfile && (kd->k_md = open(memfile, O_RDONLY)) < 0) {
			if (kd->k_prog != NULL) {
				fprintf(stderr, "%s: ", kd->k_prog);
				perror(memfile);
			}
			/* not fatal */
		}
	}
	return (kd);
}

int
kvm_close(kd)
	register kvm_t *kd;
{
	register int stat;

	if (kd == NULL) {
		errno = EBADF;
		return (-1);
	}
	stat = 0;
	if (kd->k_vd >= 0) {
		stat |= close(kd->k_vd);
		kd->k_vd = -1;
	}
	if (kd->k_kd >= 0) {
		stat |= close(kd->k_kd);
		kd->k_kd = -1;
	}
	if (kd->k_md >= 0) {
		stat |= close(kd->k_md);
		kd->k_md = -1;
	}
	if (kd->k_sd >= 0) {
		stat |= close(kd->k_sd);
		kd->k_sd = -1;
	}
	if (kd->k_sysmap) {
		free(kd->k_sysmap);
		kd->k_sysmap = 0;
	}
	free(kd);
	return (stat);
}

int
kvm_read(kd, addr, buf, nbytes)
	register kvm_t *kd;
	register unsigned long addr;
	register char *buf;
	register unsigned nbytes;
{
	register int v;

	if (kd->k_sysmap) {
		/* get kernel pte */
		addr -= KERNBASE;
		addr =
		    ctob(kd->k_sysmap[btop(addr)].pg_pfnum) + (addr & PGOFSET);
	}
	if ((v = lseek(kd->k_kd, addr, 0)) == -1)
		return (v);
	if ((v = read(kd->k_kd, buf, nbytes)) != nbytes)
		return (-1);
	return (v);
}

int
kvm_write(kd, addr, buf, nbytes)
	register kvm_t *kd;
	register unsigned long addr;
	register char *buf;
	register unsigned nbytes;
{
	register int v;

	if (kd->k_sysmap) {
		/* get kernel pte */
		addr -= KERNBASE;
		addr =
		    ctob(kd->k_sysmap[btop(addr)].pg_pfnum) + (addr & PGOFSET);
	}
	if ((v = lseek(kd->k_kd, addr, 0)) == -1)
		return (v);
	if ((v = write(kd->k_kd, buf, nbytes)) != nbytes)
		return (-1);
	return (v);
}

int
kvm_nlist(kd, nl)
	register kvm_t *kd;
	register struct nlist *nl;
{

	return (nlist(kd->k_namelist, nl));
}

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vm.h>

/* Probably doesn't work for crash dumps */
struct user *
kvm_getu(kd, p)
	register kvm_t *kd;
	register struct proc *p;
{
	register struct pte *ptep;
	register int i;
	int size;
	int nbytes, n;
	register struct user *u;
	register u_char *up;
	struct pte pagetbl[UPAGES];
	static union {
		struct user user;
		u_char buf[BLKDEV_IOSIZE];
	} buf;

	u = &buf.user;
	if ((p->p_flag & SLOAD) == 0) {
		/* Process is swapped */
		if (kd->k_sd < 0) {
			errno = EBADF;
			return (NULL);
		}
		(void) lseek(kd->k_sd, (long)dtob(p->p_swaddr), 0);
		n = roundup(sizeof(*u), DEV_BSIZE);
		if (read(kd->k_sd, u, n) == n)
			return (u);
		if (kd->k_prog != NULL) {
			i = errno;
			fprintf(stderr, "%s: kvm_getu swap: %s\n",
			    kd->k_prog, strerror(i));
			errno = i;
		}
		return (NULL);
	}

	/* Process is in core */
	if (kd->k_md < 0) {
		errno = EBADF;
		return (NULL);
	}
	ptep = pagetbl;
	if (kvm_read(kd, p->p_addr, ptep, sizeof(pagetbl),
	    "user pt") != sizeof(pagetbl))
		return (NULL);
	up = buf.buf;
	for (nbytes = sizeof(*u); nbytes > 0; ++ptep, nbytes -= NBPG) {
		n = nbytes;
		if (n > NBPG)
			n = NBPG;
		(void) lseek(kd->k_md, ptep->pg_pfnum * NBPG, 0);
		if (read(kd->k_md, up, n) != n) {
			if (kd->k_prog != NULL) {
				i = errno;
				fprintf(stderr, "%s: kvm_getu user pages: %s\n",
				    kd->k_prog, strerror(i));
				errno = i;
			}
			return (NULL);
		}
		up += n;
	}
	return (u);
}
