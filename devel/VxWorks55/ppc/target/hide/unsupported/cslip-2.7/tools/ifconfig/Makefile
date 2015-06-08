# @(#) $Header: Makefile,v 1.1 92/10/16 17:32:08 leres Exp $ (LBL)

PROG=	ifconfig
.ifdef sunos3
CFLAGS+=-DSUNOS3
.endif
.ifdef sunos4
CFLAGS+=-static -DSUNOS4
.endif
CFLAGS+=-DNOXNS
NOMAN=
BINGRP=	kmem
BINMODE=2555
.ifdef sunos3
BINDIR=	/etc
.endif

.include <bsd.prog.mk>
