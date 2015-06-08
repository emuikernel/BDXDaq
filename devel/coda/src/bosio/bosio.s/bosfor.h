 
/* bosfor.h - backward compatibility */
 
#ifdef VAX
 
#define	bosinit_	bos
#define	boswdrop_	wdrop
#define	boswgarbage_	wgarb
#define	bosldrop_	bdrop
#define	bosncreate_	mbank
#define	bosnlink_	mlink
#define	bosnformat_	mkfmt
#define	bosndrop_	mdrop
#define	bosngarbage_	bgarb
#define	boslctl_	blist
#define	boslname_	mlist
#define	boslprint_	bprnt
#define	bosnprint_	mprnt
#define	boswprint_	wprnt
 
#define	fparm_		fparm
#define	fwbos_		fwbos
#define	frbos_		frbos
#define	fseqr_		fseqr
#define	fseqw_		fseqw
#define	bosopen_	bosopen
#define	bosrewind_	bosrewind
#define	bosclose_	bosclose
#define	bosread_	bosread
#define	boswrite_	boswrite
 
#else
 
#define	bosinit_	    bos_
#define	boswdrop_	    wdrop_
#define	boswgarbage_	wgarb_
#define	bosldrop_	    bdrop_
#define	bosncreate_	    mbank_
#define	bosnlink_	    mlink_
#define	bosnformat_	    mkfmt_
#define	bosngetformat_	mbosfmt_
#define	bosndrop_	    mdrop_
#define	bosngarbage_	bgarb_
#define	boslctl_	    blist_
#define	boslname_	    mlist_
#define	boslprint_	    bprnt_
#define	bosnprint_	    mprnt_
#define	boswprint_	    wprnt_
 
#endif

