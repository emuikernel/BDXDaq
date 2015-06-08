/*
 * cslip loadable driver init code for SunOS 4
 *
 * Copyright (c) 1993 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to the Lawrence
 * Berkeley Laboratory by Dean R. E. Long.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef lint
static char rcsid[] =
    "@(#) $Header: xxxinit.c,v 1.2 93/02/12 16:33:55 leres Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/stream.h>
#include <sys/syslog.h>

#include <sun/openprom.h>
#include <sun/vddrv.h>

#include "sl.h"

static struct vdldrv driver = {
	VDMAGIC_USER,		/* Drv_magic */
	"slip"			/* Drv_name */
};

extern struct streamtab if_slinfo;

int
xxxinit(fcn, vdp, vdi, vds)
	int fcn;
	struct vddrv *vdp;
	caddr_t vdi;
	struct vdstat *vds;	/* only meaningful for VDSTAT */
{
	register struct vdioctl_load *vdl;
	register struct vdioctl_unload *vdu;
	register int unit;
	static int slot = -1;

	switch (fcn) {

	case VDLOAD:
		vdl = (struct vdioctl_load *) vdi;
		if (findmod("slip") >= 0) {
			log(LOG_ERR, "slip stream module already loaded\n");
			return (EADDRINUSE);
		}
		if ((slot = findmod("")) < 0)
			return (ENOSR);
		strncpy(fmodsw[slot].f_name, "slip", FMNAMESZ);
		fmodsw[slot].f_str = &if_slinfo;
		vdp->vdd_vdtab = (struct vdlinkage *) & driver;
		for (unit = 0; unit < NSL; ++unit)
			slattach(unit);
		log(LOG_INFO, "cslip stream module loaded\n");
		break;

	case VDSTAT:
		vdl = (struct vdioctl_load *) vdi;
		strcpy(vds->vds_modname, VERSION);
		vds->vds_modinfo[0] = 0;
		vds->vds_modinfo[1] = 0;
		break;

	case VDUNLOAD:
		vdu = (struct vdioctl_unload *) vdi;
		if (slot < 0)
			return (ENXIO);
		strcpy(fmodsw[slot].f_name, "");
		fmodsw[slot].f_str = 0;
		log(LOG_INFO, "cslip stream module unloaded\n");
		break;
	}
	return (0);
}
