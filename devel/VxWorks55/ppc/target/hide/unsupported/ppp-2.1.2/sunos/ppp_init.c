/*
  ppp_init.c - autoload support for SunOS 4.1. 

  Derived from example in section 5.7 of Sun manual "Writing Device Drivers",
  and code of similar purpose in a streams logging module written by
  Matthias Urlichs <urlichs@smurf.sub.org>.

  $Id: ppp_init.c,v 1.4 1993/12/15 05:04:14 paulus Exp $
*/

#define NULL 0
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sundev/mbvar.h>
#include <sun/autoconf.h>
#include <sun/vddrv.h>

extern nodev();

extern struct streamtab ppp_asyncinfo;
extern struct streamtab ppp_ifinfo;

static struct vdldrv vd = {
#ifdef sun4m		/* erict@ascent.com	Tuesday March 30, 1993 */
	VDMAGIC_PSEUDO,
	"ppp",
	NULL,
	NULL,0,0,0,
	NULL,
	NULL,NULL,0,1
#else /* !sun4m */
	VDMAGIC_PSEUDO,
	"ppp",
	NULL,
#ifndef sun4c
	NULL,NULL,0,1,
#endif
	NULL,0,0,0
#endif /* sun4m */
};

static struct fmodsw *fmod_advmod0=NULL;
static struct fmodsw *fmod_advmod1=NULL;

int
install_fmod(ste, fmop, name)
    struct streamtab * ste;
    struct fmodsw ** fmop;
    char * name;
{
    int dev,i;

    for(dev=0; dev < fmodcnt; dev++) {
	if(fmodsw[dev].f_str == NULL)
	    break;
    }
    if(dev == fmodcnt) return(ENODEV);

    *fmop = &fmodsw[dev];
    for(i=0;i<=FMNAMESZ;i++) {
	(*fmop)->f_name[i] = name[i];
	if(name[0] == 0) break;
    }
    (*fmop)->f_str = ste;
    return 0;
}


void
deinstall_fmod (fmodp)
    struct fmodsw * fmodp;
{
    fmodp->f_name[0] = '\0';
    fmodp->f_str = NULL;
} 

char *PPPIF = "pppif";
char *PPPASYNC = "pppasync";

xxxinit(fc,vdp,vdi,vds)
unsigned int fc;
struct vddrv *vdp;
addr_t vdi;
struct vdstat *vds;
{
    int er;
    int x;

    switch(fc) {
    case VDLOAD:
	if(fmod_advmod0)
	    return(EBUSY);
	er = install_fmod (&ppp_ifinfo, &fmod_advmod0, PPPIF);
	if(er != 0)
	    return er;
	er = install_fmod (&ppp_asyncinfo, &fmod_advmod1, PPPASYNC);
	if(er != 0) {
	    deinstall_fmod (fmod_advmod0);
	    fmod_advmod0 = 0;
	    return er;
	}

	vdp->vdd_vdtab = (struct vdlinkage *)&vd;
	for(x = 0; x < NPPP; x ++)
	    ppp_attach(x);
	return 0;

    case VDUNLOAD:
	if(ppp_busy())
	    return EIO;

	for(x = 0; x < NPPP; x ++)
	    ppp_unattach(x);

	if(!fmod_advmod0)
	    return ENODEV;

	deinstall_fmod (fmod_advmod0);
	deinstall_fmod (fmod_advmod1);
	return 0;

    case VDSTAT:
	return 0;

    default:
	return EIO;
  }
}
