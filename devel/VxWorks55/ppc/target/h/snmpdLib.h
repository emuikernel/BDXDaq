/* snmpdLib.h - VxWorks SNMP Agent */

/* Copyright 1984-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,02oct98,ann  added missing function prototypes (SPR# 7017)
01d,30apr98,ann  changed snmpVbPrivFree prototype to avoid 
                 conflict with master-subagent.
01c,07jun96,rjc  removed extra } #ifdef'd under _cplusplus
01b,16may96,rjc  rewritten for snmp v1/v2c product
01a,18feb94,elh  written. 
*/

#ifndef __INCsnmpdLibh
#define __INCsnmpdLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vxWorks.h"
#include <snmp/asn1.h>
#include <snmp/snmp.h>
#include <snmp/snmpdefs.h>
#include <netinet/in.h>      /* for htonl, ntohl etc */


#define MAX_OID_LEN   128    /* Max no of components in an OID */

#define SNMP_ERROR      1    /* error messages */
#define SNMP_WARN       2    /* warning messages */
#define SNMP_INFO       3    /* information messages */


#define S_snmpdLib_VIEW_CREATE_FAILURE    (M_snmpdLib | 1)
#define S_snmpdLib_VIEW_INSTALL_FAILURE   (M_snmpdLib | 2)
#define S_snmpdLib_VIEW_MASK_FAILURE      (M_snmpdLib | 3)
#define S_snmpdLib_VIEW_DEINSTALL_FAILURE (M_snmpdLib | 4)
#define S_snmpdLib_VIEW_LOOKUP_FAILURE    (M_snmpdLib | 5)
#define S_snmpdLib_MIB_ADDITION_FAILURE   (M_snmpdLib | 6)
#define S_snmpdLib_NODE_NOT_FOUND         (M_snmpdLib | 7)
#define S_snmpdLib_INVALID_SNMP_VERSION   (M_snmpdLib | 8)
#define S_snmpdLib_TRAP_CREATE_FAILURE    (M_snmpdLib | 9)
#define S_snmpdLib_TRAP_BIND_FAILURE      (M_snmpdLib | 10)
#define S_snmpdLib_TRAP_ENCODE_FAILURE    (M_snmpdLib | 11)
#define S_snmpdLib_INVALID_OID_SYNTAX     (M_snmpdLib | 12)


/* function declarations */
 
#if defined(__STDC__) || defined(__cplusplus)

extern void     snmpdPktProcess (int, char *, void *, void *, void *);
extern void     snmpdLog (int, char *);
extern STATUS   snmpdTreeAdd (char *, MIBNODE_T *);
extern void     snmpdTreeRemove (char *);
extern STATUS   snmpdViewEntrySet (OIDC_T *, int, UINT_16_T, uchar_t *, int, 
                                  int);
extern void     snmpdViewEntryRemove (OIDC_T *, int, UINT_16_T);
extern int      snmpOidToIpHostOrder (int, OIDC_T *, ulong_t *);

extern void     snmpdTrapSend (void *, int, void **, void *, int, char *,
                              OIDC_T *, int, u_long *, int, int, int, FUNCPTR, 
                              void *); 
extern STATUS   snmpNextIndex (int, const OIDC_T *, OIDC_T * , const OIDC_T *, int); 
extern void     snmpdGroupByGetprocAndInstance (SNMP_PKT_T *, VB_T *, int, OIDC_T *);
extern void     snmpdContinue (SNMP_PKT_T *);
extern void     snmpNextError (SNMP_PKT_T *, VB_T *);
extern void     snmpVbPrivFree (VB_T *);
extern int      snmpOidStrToArray (char *, OIDC_T *);
extern void     snmpdInitFinish (VOIDFUNCPTR, FUNCPTR, FUNCPTR, FUNCPTR, FUNCPTR);
extern STATUS   snmpdPktLockGet (SNMP_PKT_T *);

#else   /* __STDC__ */

extern void     snmpdPktProcess ();
extern void     snmpdLog ();
extern STATUS   snmpdTreeAdd ();
extern void     snmpdTreeRemove ();
extern int      snmpdViewEntrySet (); 
extern void     snmpdViewEntryRemove (); 
extern int      snmpOidToIpHostOrder ();
extern void     snmpdTrapSend ();
extern int      snmpNextIndex ();
extern void     groupVarBinds (); 
extern void     snmpNextError ();
extern void     snmpVbPrivFree ();
extern int      snmpOidStrToArray ();
extern void     snmpdInitFinish ();
extern STATUS   snmpdPktLockGet ();
extern void     snmpdGroupByGetprocAndInstance ();
extern void     snmpdContinue ();


#endif  /* __STDC__ */


#ifdef __cplusplus
}
#endif


#endif /* __INCsnmpdLibh */


