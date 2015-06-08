/* usrVxdcomInit.c - VxDCOM configlette initialization file */ 

/* Copyright 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,10oct01,nel  Remove debug.
01m,10oct01,nel  SPR#70841. Add SCM stack size parameter.
01l,11jun01,nel  debug.
01k,12oct99,dbs  add INCLUDE_VXIDL_PS folder
01j,17aug99,aim  removed VXDCOM_OBJECT_EXPORTER_REUSE_PORT_NUMBER
01i,17aug99,aim  added ObjectExporter resources
01h,13aug99,aim  reworked globals (again)
01g,13aug99,aim  reworked globals
01f,21jul99,drm  Adding additional parameters to dcomLibInit() call.
01e,28jun99,dbs  add default authn level as arg to dcomLibInit()
01d,24may99,dbs  make configlette take user-configurable params
01c,20may99,dbs  fix names, add support for COM and DCOM separation
01b,29mar99,dbs  removed inclusion of comLib.h as its not C-friendly yet
01a,24mar99,drm  created 
*/

extern int include_vxidl ();

extern int dcomLibInit
    (
    int,			/* BSTR policy */
    int,			/* DCOM Authentication level */
    unsigned int,		/* thread priority */
    unsigned int,		/* Static threads */
    unsigned int,		/* Dynamic threads */
    unsigned int,		/* Stack Size of server thread */
    unsigned int,		/* Stack Size of SCM thread */
    int,			/* Client Priority propogation */
    int 			/* Object Exporter Port Number */
    );

void usrVxdcomInit()
    {
    dcomLibInit (VXDCOM_BSTR_POLICY,
		 VXDCOM_AUTHN_LEVEL,
		 VXDCOM_THREAD_PRIORITY,
		 VXDCOM_STATIC_THREADS,
		 VXDCOM_DYNAMIC_THREADS,
		 VXDCOM_STACK_SIZE,
		 VXDCOM_SCM_STACK_SIZE,
		 VXDCOM_CLIENT_PRIORITY_PROPAGATION,
		 VXDCOM_OBJECT_EXPORTER_PORT_NUMBER);

#ifdef INCLUDE_VXIDL_PS
    include_vxidl ();
#endif
    }
