/* @(#) $Header: kvm.h,v 1.2 92/10/19 14:53:45 leres Exp $ (LBL) */

struct _kvm_t {
	int k_kd;	/* kmem fd */
	int k_md;	/* mem fd */
	int k_vd;	/* vmunix fd */
	int k_sd;	/* swap fd */
	struct pte *k_sysmap;
	char *k_prog;
	char *k_namelist;
};

typedef struct _kvm_t kvm_t;

extern kvm_t *kvm_open();
extern int kvm_close(), kvm_read(), kvm_write(), kvm_nlist();
