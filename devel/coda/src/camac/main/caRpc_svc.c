
#ifdef VXWORKS

#include "cinclude/caSrvrLib.c"


/* caRpc_svc.c - server running in VME */

#include <stdio.h>
#include <rpc/rpc.h>
#include "caRpc.h"

static void casrvr_1();

caSrvr_main()
{
  SVCXPRT *transp;

  (void)localServerInit();
  (void)pmap_unset(CASRVR, CAVERS);

  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if(transp == NULL)
  {
    (void) fprintf(stderr, "cannot create tcp service.\n");
    exit(1);
  }
  if(!svc_register(transp, CASRVR, CAVERS, casrvr_1, IPPROTO_TCP))
  {
    (void) fprintf(stderr, "unable to register (CASRVR, CAVERS, tcp).\n");
    exit(1);
  }
  
  svc_run();
  (void) fprintf(stderr, "svc_run returned\n");

  exit(1);
}

static void
casrvr_1(struct svc_req *rqstp, SVCXPRT *transp)
{
  union
  {
	CA_BCNA ca_rpccdreg_1_arg;
  	CA_FXD ca_rpccfsa_1_arg;
   	int ca_rpccccz_1_arg;
   	int ca_rpccccc_1_arg;
   	CA_XL ca_rpcccci_1_arg;
   	int ca_rpcctci_1_arg;
   	CA_XL ca_rpccccd_1_arg;
   	int ca_rpcctcd_1_arg;
   	int ca_rpcctgl_1_arg;
   	CA_BCNA ca_rpccdlam_1_arg;
   	CA_LL ca_rpccclm_1_arg;
   	int ca_rpccclc_1_arg;
   	int ca_rpcctlm_1_arg;
   	int ca_rpccclwt_1_arg;
   	int ca_rpcccinit_1_arg;
   	int ca_rpcctstat_1_arg; /*Sergey: not sure we need that ???*/
   	int ca_rpcctstatus_1_arg;
  } argument;
  char *result;
  bool_t (*xdr_argument)(), (*xdr_result)();
  char *(*local)();

  switch (rqstp->rq_proc)
  {
	case NULLPROC:
		(void)svc_sendreply(transp, xdr_void, (char *)NULL);
		return;

	case CA_RPCCDREG:
		xdr_argument = xdr_CA_BCNA;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccdreg_1;
		break;

	case CA_RPCCFSA:
		xdr_argument = xdr_CA_FXD;
		xdr_result = xdr_CA_DQ;
		local = (char *(*)()) ca_rpccfsa_1;
		break;

	case CA_RPCCCCZ:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccccz_1;
		break;

	case CA_RPCCCCC:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccccc_1;
		break;

	case CA_RPCCCCI:
		xdr_argument = xdr_CA_XL;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcccci_1;
		break;

	case CA_RPCCTCI:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctci_1;
		break;

	case CA_RPCCCCD:
		xdr_argument = xdr_CA_XL;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccccd_1;
		break;

	case CA_RPCCTCD:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctcd_1;
		break;

	case CA_RPCCTGL:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctgl_1;
		break;

	case CA_RPCCDLAM:
		xdr_argument = xdr_CA_BCNA;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccdlam_1;
		break;

	case CA_RPCCCLM:
		xdr_argument = xdr_CA_LL;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccclm_1;
		break;

	case CA_RPCCCLC:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccclc_1;
		break;

	case CA_RPCCTLM:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctlm_1;
		break;

	case CA_RPCCCLWT:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpccclwt_1;
		break;

	case CA_RPCCCINIT:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcccinit_1;
		break;

	case CA_RPCCTSTAT:
		xdr_argument = xdr_void;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctstat_1;
		break;

	case CA_RPCCTSTATUS:
		xdr_argument = xdr_int;
		xdr_result = xdr_int;
		local = (char *(*)()) ca_rpcctstatus_1;
		break;

	default:
		svcerr_noproc(transp);
		return;
  }

  bzero((char *)&argument, sizeof(argument));
  if(!svc_getargs(transp, xdr_argument, &argument))
  {
    svcerr_decode(transp);
    return;
  }

  result = (*local)(&argument, rqstp);
  if(result != NULL && !svc_sendreply(transp, xdr_result, result))
  {
    svcerr_systemerr(transp);
  }

  if(!svc_freeargs(transp, xdr_argument, &argument))
  {
    (void)fprintf(stderr, "unable to free arguments\n");
    exit(1);
  }
}

#else

#include <stdio.h>
int
main()
{
  printf("VXWORKS only !!!\n");
}

#endif
