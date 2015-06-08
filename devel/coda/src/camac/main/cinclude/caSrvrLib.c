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
 *	Support routines for the CAMAC RPC server
 *	
 * Author:  Chip Watson, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: caSrvrLib.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:18:46  heyes
 *   Imported sources
 *
*	  Revision 1.1  94/03/16  07:53:20  07:53:20  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:47:24  11:47:24  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.1  1992/06/05  20:21:45  watson
 *	  Initial revision
 *
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include "caRpc.h"
#include "ca.h"

void rpcTaskInit();

void
localServerInit()
{
  (void) rpcTaskInit();
  /*(void) ccinit(0);Sergey: must be called by user*/
}

int *
ca_rpccdreg_1(CA_BCNA *arg)
{
  static int ext;
  (void)cdreg(&ext,arg->b,arg->c,arg->n,arg->a);
  return(&ext);
}

CA_DQ *
ca_rpccfsa_1(CA_FXD *arg)
{
  static CA_DQ o;
  (void)cfsa(arg->f,arg->ext,&(arg->dat),&(o.q));
  o.dat=arg->dat;
  return(&o);
}

int *
ca_rpccccz_1(int *ext)
{
  static int o;
  (void)cccz(*ext);
  return(&o);
}

int *
ca_rpccccc_1(int *ext)
{
  static int o;
  (void)cccc(*ext);
  return(&o);
}

int *
ca_rpcccci_1(CA_XL *arg)
{
  static int o;
  (void)ccci(arg->ext,arg->l);
  return(&o);
}

int *
ca_rpcctci_1(int *ext)
{
  static int l;
  (void)ctci(*ext,&l);
  return(&l);
}

int *
ca_rpccccd_1(CA_XL *arg)
{
  static int o;
  (void)cccd(arg->ext,arg->l);
  return(&o);
}

int *
ca_rpcctcd_1(int *ext)
{
  static int l;
  (void)ctcd(*ext,&l);
  return(&l);
}

int *
ca_rpcccgl_1(CA_LL *arg)
{
  static int o;
  (void)ccgl(arg->lam,arg->l);
  return(&o);
}

int *
ca_rpcctgl_1(int *ext)
{
  static int l;
  (void)ctgl(*ext,&l);
  return(&l);
}

int *
ca_rpccdlam_1(CA_BCNA *arg)
{
  static int lam,inta[2];
  (void)cdlam(&lam,arg->b,arg->c,arg->n,arg->a,inta); /* inta is not used */
  return(&lam);
}

int *
ca_rpccclc_1(int *lam)
{
  static int o;
  (void)cclc(*lam);
  return(&o);
}

int *
ca_rpccclm_1(CA_LL *arg)
{
  static int o;
  (void)cclm(arg->lam,arg->l);
  return(&o);
}

int *
ca_rpcctlm_1(int *lam)
{
  static int l;
  (void)ctlm(*lam,&l);
  return(&l);
}

int *
ca_rpcccinit_1(int *b)
{
  static int res;
  (void)ccinit(*b);
  return(&res);
}

int *
ca_rpccclwt_1(int *lam)
{
  static int o;
  (void)cclwt(*lam);
  return(&o);
}

int *
ca_rpcctstat_1()
{
  static int istat;
  (void)ctstat(&istat);
  return(&istat);
}

int *
ca_rpcctstatus_1(int *ext)
{
  static int istat;
  (void)ctstatus(*ext, &istat);
  return(&istat);
}
