
#ifndef VXWORKS

/* UNIX only */

/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 * ------------
 *  RPC interface routines for CAMAC standard routines
 *  Both C and Fortran interfaces are provided. No LAM handling
 *	
 * Author:  Chip Watson, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: caClientLib.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:18:46  heyes
 *   Imported sources
 *
*	  Revision 1.1  94/03/16  07:53:04  07:53:04  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:47:03  11:47:03  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.1  1992/06/05  19:31:01  watson
 *	  Initial revision
 *
 *	fix caopen fortran interface (null terminate string)
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include "caRpc.h"

CLIENT *caHandle;

void caopen(char *server,int *success)
{
  caHandle = clnt_create(server,CASRVR,CAVERS,"tcp");
  if (caHandle == NULL) {
    (void) clnt_pcreateerror(server);
    *success=0;
  }
  else
    *success=1;
}
void caopen_(char *server,int *success,int server_len)
{
  char *s;
  s = (char *) malloc(server_len+1);
  strncpy(s,server,server_len);
  s[server_len]=0;		/* make null terminated string */
  (void) caopen(s,success);
  free(s);
}

void cdreg(int *ext,int b,int c,int n,int a)
{
  CA_BCNA arg;
  int *r;
  arg.b= b;
  arg.c= c;
  arg.n= n;
  arg.a= a;
  r = ca_rpccdreg_1(&arg,caHandle);
  if (r!=NULL) 
    *ext = *r;
  else
    *ext = 0;
}
void cdreg_(int *ext,int *b,int *c,int *n,int *a)
{
  (void) cdreg(ext,*b,*c,*n,*a);
}

void cfsa(int f,int ext,int *data,int *q)
{
  CA_FXD a;
  CA_DQ *r;
  a.f= f;
  a.ext= ext;
  a.dat= *data;
  r = ca_rpccfsa_1(&a,caHandle);
  if (r!=NULL) {
    if (f<8) *data=r->dat;
    *q=r->q;
  }
}
void cfsa_(int *f,int *ext,int *data,int *q)
{
  (void) cfsa(*f,*ext,data,q);
}

void cccz(int ext)
{
  (void)ca_rpccccz_1(&ext,caHandle);
}
void cccz_(int *ext)
{
  (void)ca_rpccccz_1(ext,caHandle);
}

void cccc(int ext)
{
  (void)ca_rpccccc_1(&ext,caHandle);
}
void cccc_(int *ext)
{
  (void)ca_rpccccc_1(ext,caHandle);
}

void ccci(int ext,int l)
{
  CA_XL a;
  a.ext= ext;
  a.l= l;
  (void)ca_rpcccci_1(&a,caHandle);
}
void ccci_(int *ext,int *l)
{
  (void) ccci(*ext,*l);
}

void ctci(int ext,int *l)
{
  int *r;
  r = ca_rpcctci_1(&ext,caHandle);
  *l = r? *r : 0;		/* return 0 on any error */
}
void ctci_(int *ext,int *l)
{
  (void) ctci(*ext,l);
}

void cccd(int ext,int l)
{
  CA_XL a;
  a.ext= ext;
  a.l= l;
  (void)ca_rpccccd_1(&a,caHandle);
}
void cccd_(int *ext,int *l)
{
  (void) cccd(*ext,*l);
}

void ctcd(int ext,int *l)
{
  int *r;
  r = ca_rpcctcd_1(ext,caHandle);
  *l = r? *r : 0;
}
void ctcd_(int *ext,int *l)
{
  (void) ctcd(*ext,l);
}

void ctgl(int ext,int *l)
{
  int *r;
  r = ca_rpcctgl_1(ext,caHandle);
  *l = r? *r : 0;
}
void ctgl_(int *ext,int *l)
{
  (void) ctgl(*ext,l);
}

void cdlam(int *lam,int b,int c,int n,int a,int *inta)
{
  CA_BCNA arg;
  int *r;
  arg.b= b;
  arg.c= c;
  arg.n= n;
  arg.a= a;
  r = ca_rpccdlam_1(&arg,caHandle);
  *lam = r? *r : 0;
}
void cdlam_(int *lam,int *b,int *c,int *n,int *a,int *inta)
{
  (void) cdlam(lam,*b,*c,*n,*a,inta);
}

void cclm(int lam,int l)
{
  CA_LL a;
  a.lam= lam;
  a.l= l;
  (void)ca_rpccclm_1(&a,caHandle);
}
void cclm_(int *lam,int *l)
{
  (void) cclm(*lam,*l);
}

void cclc(int lam)
{
  (void)ca_rpccclc_1(&lam,caHandle);
}
void cclc_(int *lam)
{
  (void)ca_rpccclc_1(lam,caHandle);
}

void ctlm(int lam,int *l)
{
  int *r;
  r = ca_rpcctlm_1(&lam,caHandle);
  *l = r? *r : 0;
}
void ctlm_(int *lam,int *l)
{
  (void) ctlm(*lam,l);
}

void cclwt(int lam)
{
  (void)ca_rpccclwt_1(&lam,caHandle);
}
void cclwt_(int *lam)
{
  (void)ca_rpccclwt_1(lam,caHandle);
}

void ccinit(int b)
{
  (void)ca_rpcccinit_1(&b,caHandle);
}
void ccinit_(int *b)
{
  (void)ca_rpcccinit_1(b,caHandle);
}

/* */

void ctstat(int *istat)
{
  int *r;
  /*printf("111\n");fflush(stdout);*/
  r = ca_rpcctstat_1(NULL,caHandle);
  *istat = r? *r : 0;
}
void ctstat_(int *istat)
{
  /*printf("222\n");fflush(stdout);*/
  (void) ctstat(istat);
}

/* */

void ctstatus(int ext, int *istat)
{
  int *r;
  /*printf("111: ext=0x%08x\n",ext);fflush(stdout);*/
  r = ca_rpcctstatus_1(&ext,caHandle);
  *istat = r? *r : 0;		/* return 0 on any error */
}
void ctstatus_(int *ext, int *istat)
{
  /*printf("222\n");fflush(stdout);*/
  (void) ctstatus(*ext,istat);
}


#else

caClientLib_dummy()
{
  return;
}

#endif
