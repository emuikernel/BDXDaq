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
 *	CAMAC rpc definitions
 *	
 * Author:  Chip Watson, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: caRpc.x,v $
 *   Revision 1.1.1.1  1996/08/21 19:18:46  heyes
 *   Imported sources
 *
 *   Revision 1.1  94/03/16  07:53:07  07:53:07  heyes (Graham Heyes)
 *   Initial revision
 *   
 *   Revision 1.1  94/03/15  11:47:07  11:47:07  heyes (Graham Heyes)
 *   Initial revision
 *   
# Revision 1.1  1992/06/05  20:20:25  watson
# Initial revision
#
 */

struct CA_BCNA {
    int b;
    int c;
    int n;
    int a;
};
struct CA_FXD {
    int f;
    int ext;
    int dat;
};
struct CA_DQ {
    int dat;
    int q;
};
struct CA_XL {
    int ext;
    int l;
};
struct CA_LL {
    int lam;
    int l;
};

program CASRVR {
  version CAVERS {
    int CA_RPCCDREG(CA_BCNA)=1;
    CA_DQ CA_RPCCFSA(CA_FXD)=2;
    int CA_RPCCCCZ(int)=3;
    int CA_RPCCCCC(int)=4;
    int CA_RPCCCCI(CA_XL)=5;
    int CA_RPCCTCI(int)=6;
    int CA_RPCCCCD(CA_XL)=7;
    int CA_RPCCTCD(int)=8;
    int CA_RPCCTGL(int)=9;
    int CA_RPCCDLAM(CA_BCNA)=10;
    int CA_RPCCCLM(CA_LL)=11;
    int CA_RPCCCLC(int)=12;
    int CA_RPCCTLM(int)=13;
    int CA_RPCCCLWT(int)=14;
    int CA_RPCCCINIT(int)=15;
    int CA_RPCCTSTAT(void)=16;
    int CA_RPCCTSTATUS(int)=17;
  } = 1;
} = 0x2c0da009;