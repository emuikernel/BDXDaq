/* THIS IS A GENERATED FILE. DO NOT EDIT! */
/* Generated from ../O.Common/demo.dbd */

#include <string.h>

#include "epicsStdlib.h"
#include "iocsh.h"
#include "iocshRegisterCommon.h"
#include "registryCommon.h"

extern "C" {

epicsShareExtern rset *pvar_rset_aoRSET;
epicsShareExtern int (*pvar_func_aoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_boRSET;
epicsShareExtern int (*pvar_func_boRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringinRSET;
epicsShareExtern int (*pvar_func_stringinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_waveformRSET;
epicsShareExtern int (*pvar_func_waveformRecordSizeOffset)(dbRecordType *pdbRecordType);

static const char * const recordTypeNames[4] = {
    "ao",
    "bo",
    "stringin",
    "waveform"
};

static const recordTypeLocation rtl[4] = {
    {pvar_rset_aoRSET, pvar_func_aoRecordSizeOffset},
    {pvar_rset_boRSET, pvar_func_boRecordSizeOffset},
    {pvar_rset_stringinRSET, pvar_func_stringinRecordSizeOffset},
    {pvar_rset_waveformRSET, pvar_func_waveformRecordSizeOffset}
};

epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devSiSeq;
epicsShareExtern dset *pvar_dset_devWfSoft;

static const char * const deviceSupportNames[4] = {
    "devAoSoft",
    "devBoSoft",
    "devSiSeq",
    "devWfSoft"
};

static const dset * const devsl[4] = {
    pvar_dset_devAoSoft,
    pvar_dset_devBoSoft,
    pvar_dset_devSiSeq,
    pvar_dset_devWfSoft
};

epicsShareExtern void (*pvar_func_demoRegistrar)(void);

int demo_registerRecordDeviceDriver(DBBASE *pbase)
{
    const char *bldTop = "/jlab/clas/devel/R3.14.12.3/synApps/synApps_5_5/support/seq-2-0-12";
    const char *envTop = getenv("TOP");

    if (envTop && strcmp(envTop, bldTop)) {
        printf("Warning: IOC is booting with TOP = \"%s\"\n"
               "          but was built with TOP = \"%s\"\n",
               envTop, bldTop);
    }

    if (!pbase) {
        printf("pdbbase is NULL; you must load a DBD file first.\n");
        return -1;
    }

    registerRecordTypes(pbase, 4, recordTypeNames, rtl);
    registerDevices(pbase, 4, deviceSupportNames, devsl);
    (*pvar_func_demoRegistrar)();
    return 0;
}

/* registerRecordDeviceDriver */
static const iocshArg registerRecordDeviceDriverArg0 =
                                            {"pdbbase",iocshArgPdbbase};
static const iocshArg *registerRecordDeviceDriverArgs[1] =
                                            {&registerRecordDeviceDriverArg0};
static const iocshFuncDef registerRecordDeviceDriverFuncDef =
                {"demo_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    demo_registerRecordDeviceDriver(*iocshPpdbbase);
}

} // extern "C"
/*
 * Register commands on application startup
 */
static int Registration() {
    iocshRegisterCommon();
    iocshRegister(&registerRecordDeviceDriverFuncDef,
        registerRecordDeviceDriverCallFunc);
    return 0;
}

static int done = Registration();
