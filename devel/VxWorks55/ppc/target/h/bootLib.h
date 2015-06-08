/* bootLib.h - boot support subroutine library */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,13mar02,jkf  Fixed SPR#74251, increased BOOT_DEV_LEN, BOOT_FILE_LEN, 
                 and BOOT_FIELD_LEN, changed copyright date to 2002.
01l,15may97,spm  fixed SPR 8533: increased BOOT_PARAMS field sizes to prevent 
                 overflow with DHCP client leases
01k,30apr97,spm  moved unit number field to end of BOOT_PARAMS structure
01j,16dec96,spm  removed conditional include from DHCP client code
01i,03dec96,spm  added conditional include to DHCP client code
01h,27nov96,spm  added support for DHCP client and network device unit numbers
01g,22sep92,rrr  added support for c++
01f,04jul92,jcf  cleaned up.
01e,26may92,rrr  the tree shuffle
01d,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01c,05oct90,shl  added ANSI function prototypes.
                 added copyright notice.
01b,10aug90,dnw  added declaration of bootParamsErrorPrint().
01a,18jul90,dnw  written
*/

#ifndef __INCbootLibh
#define __INCbootLibh

#ifdef __cplusplus
extern "C" {
#endif

/* BOOT_PARAMS is a structure containing all the fields of the VxWorks
 * boot line.  The routines in bootLib convert this structure to and
 * from the boot line ascii string.
 */

#define BOOT_DEV_LEN		40	/* max chars in device name */
#define BOOT_HOST_LEN		20	/* max chars in host name */
#define BOOT_ADDR_LEN		30	/* max chars in net addr */
#define BOOT_TARGET_ADDR_LEN    50      /* IP address + mask + lease times */
#define BOOT_ADDR_LEN		30	/* max chars in net addr */
#define BOOT_FILE_LEN		160	/* max chars in file name */
#define BOOT_USR_LEN		20	/* max chars in user name */
#define BOOT_PASSWORD_LEN	20	/* max chars in password */
#define BOOT_OTHER_LEN		80	/* max chars in "other" field */

#define BOOT_FIELD_LEN		160	/* max chars in any boot field */

typedef struct				/* BOOT_PARAMS */
    {
    char bootDev [BOOT_DEV_LEN];	/* boot device code */
    char hostName [BOOT_HOST_LEN];	/* name of host */
    char targetName [BOOT_HOST_LEN];	/* name of target */
    char ead [BOOT_TARGET_ADDR_LEN];	/* ethernet internet addr */
    char bad [BOOT_TARGET_ADDR_LEN];	/* backplane internet addr */
    char had [BOOT_ADDR_LEN];		/* host internet addr */
    char gad [BOOT_ADDR_LEN];		/* gateway internet addr */
    char bootFile [BOOT_FILE_LEN];	/* name of boot file */
    char startupScript [BOOT_FILE_LEN];	/* name of startup script file */
    char usr [BOOT_USR_LEN];		/* user name */
    char passwd [BOOT_PASSWORD_LEN];	/* password */
    char other [BOOT_OTHER_LEN];	/* available for applications */
    int  procNum;			/* processor number */
    int  flags;				/* configuration flags */
    int  unitNum;                       /* network device unit number */
    } BOOT_PARAMS;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	bootBpAnchorExtract (char *string, char ** pAnchorAdrs);
extern STATUS 	bootLeaseExtract (char *string, u_long *pLeaseLen, 
                                  u_long *pLeaseStart);
extern STATUS 	bootNetmaskExtract (char *string, int *pNetmask);
extern STATUS 	bootScanNum (char ** ppString, int *pValue, BOOL hex);
extern STATUS 	bootStructToString (char *paramString, BOOT_PARAMS
		*pBootParams);
extern char *	bootStringToStruct (char *bootString, BOOT_PARAMS *pBootParams);
extern void 	bootParamsErrorPrint (char *bootString, char *pError);
extern void 	bootParamsPrompt (char *string);
extern void 	bootParamsShow (char *paramString);

#else	/* __STDC__ */

extern STATUS 	bootBpAnchorExtract ();
extern STATUS 	bootLeaseExtract ();
extern STATUS 	bootNetmaskExtract ();
extern STATUS 	bootScanNum ();
extern STATUS 	bootStructToString ();
extern char *	bootStringToStruct ();
extern void 	bootParamsErrorPrint ();
extern void 	bootParamsPrompt ();
extern void 	bootParamsShow ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCbootLibh */
