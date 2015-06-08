/* THIS IS A GENERATED FILE. DO NOT EDIT! */
/* Generated from ../O.Common/iocxxxLinux.dbd */

#include <string.h>

#include "epicsStdlib.h"
#include "iocsh.h"
#include "iocshRegisterCommon.h"
#include "registryCommon.h"

extern "C" {

epicsShareExtern rset *pvar_rset_aaiRSET;
epicsShareExtern int (*pvar_func_aaiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aaoRSET;
epicsShareExtern int (*pvar_func_aaoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aiRSET;
epicsShareExtern int (*pvar_func_aiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aoRSET;
epicsShareExtern int (*pvar_func_aoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aSubRSET;
epicsShareExtern int (*pvar_func_aSubRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_biRSET;
epicsShareExtern int (*pvar_func_biRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_boRSET;
epicsShareExtern int (*pvar_func_boRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcRSET;
epicsShareExtern int (*pvar_func_calcRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcoutRSET;
epicsShareExtern int (*pvar_func_calcoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_compressRSET;
epicsShareExtern int (*pvar_func_compressRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_dfanoutRSET;
epicsShareExtern int (*pvar_func_dfanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_eventRSET;
epicsShareExtern int (*pvar_func_eventRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_fanoutRSET;
epicsShareExtern int (*pvar_func_fanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longinRSET;
epicsShareExtern int (*pvar_func_longinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longoutRSET;
epicsShareExtern int (*pvar_func_longoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiRSET;
epicsShareExtern int (*pvar_func_mbbiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiDirectRSET;
epicsShareExtern int (*pvar_func_mbbiDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboRSET;
epicsShareExtern int (*pvar_func_mbboRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboDirectRSET;
epicsShareExtern int (*pvar_func_mbboDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_permissiveRSET;
epicsShareExtern int (*pvar_func_permissiveRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_selRSET;
epicsShareExtern int (*pvar_func_selRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_seqRSET;
epicsShareExtern int (*pvar_func_seqRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stateRSET;
epicsShareExtern int (*pvar_func_stateRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringinRSET;
epicsShareExtern int (*pvar_func_stringinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringoutRSET;
epicsShareExtern int (*pvar_func_stringoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subRSET;
epicsShareExtern int (*pvar_func_subRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subArrayRSET;
epicsShareExtern int (*pvar_func_subArrayRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_waveformRSET;
epicsShareExtern int (*pvar_func_waveformRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_asynRSET;
epicsShareExtern int (*pvar_func_asynRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_scalerRSET;
epicsShareExtern int (*pvar_func_scalerRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_sseqRSET;
epicsShareExtern int (*pvar_func_sseqRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_epidRSET;
epicsShareExtern int (*pvar_func_epidRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_timestampRSET;
epicsShareExtern int (*pvar_func_timestampRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_transformRSET;
epicsShareExtern int (*pvar_func_transformRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_scalcoutRSET;
epicsShareExtern int (*pvar_func_scalcoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_acalcoutRSET;
epicsShareExtern int (*pvar_func_acalcoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_swaitRSET;
epicsShareExtern int (*pvar_func_swaitRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_busyRSET;
epicsShareExtern int (*pvar_func_busyRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_scanparmRSET;
epicsShareExtern int (*pvar_func_scanparmRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_sscanRSET;
epicsShareExtern int (*pvar_func_sscanRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_tableRSET;
epicsShareExtern int (*pvar_func_tableRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mcaRSET;
epicsShareExtern int (*pvar_func_mcaRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_motorRSET;
epicsShareExtern int (*pvar_func_motorRecordSizeOffset)(dbRecordType *pdbRecordType);

static const char * const recordTypeNames[43] = {
    "aai",
    "aao",
    "ai",
    "ao",
    "aSub",
    "bi",
    "bo",
    "calc",
    "calcout",
    "compress",
    "dfanout",
    "event",
    "fanout",
    "longin",
    "longout",
    "mbbi",
    "mbbiDirect",
    "mbbo",
    "mbboDirect",
    "permissive",
    "sel",
    "seq",
    "state",
    "stringin",
    "stringout",
    "sub",
    "subArray",
    "waveform",
    "asyn",
    "scaler",
    "sseq",
    "epid",
    "timestamp",
    "transform",
    "scalcout",
    "acalcout",
    "swait",
    "busy",
    "scanparm",
    "sscan",
    "table",
    "mca",
    "motor"
};

static const recordTypeLocation rtl[43] = {
    {pvar_rset_aaiRSET, pvar_func_aaiRecordSizeOffset},
    {pvar_rset_aaoRSET, pvar_func_aaoRecordSizeOffset},
    {pvar_rset_aiRSET, pvar_func_aiRecordSizeOffset},
    {pvar_rset_aoRSET, pvar_func_aoRecordSizeOffset},
    {pvar_rset_aSubRSET, pvar_func_aSubRecordSizeOffset},
    {pvar_rset_biRSET, pvar_func_biRecordSizeOffset},
    {pvar_rset_boRSET, pvar_func_boRecordSizeOffset},
    {pvar_rset_calcRSET, pvar_func_calcRecordSizeOffset},
    {pvar_rset_calcoutRSET, pvar_func_calcoutRecordSizeOffset},
    {pvar_rset_compressRSET, pvar_func_compressRecordSizeOffset},
    {pvar_rset_dfanoutRSET, pvar_func_dfanoutRecordSizeOffset},
    {pvar_rset_eventRSET, pvar_func_eventRecordSizeOffset},
    {pvar_rset_fanoutRSET, pvar_func_fanoutRecordSizeOffset},
    {pvar_rset_longinRSET, pvar_func_longinRecordSizeOffset},
    {pvar_rset_longoutRSET, pvar_func_longoutRecordSizeOffset},
    {pvar_rset_mbbiRSET, pvar_func_mbbiRecordSizeOffset},
    {pvar_rset_mbbiDirectRSET, pvar_func_mbbiDirectRecordSizeOffset},
    {pvar_rset_mbboRSET, pvar_func_mbboRecordSizeOffset},
    {pvar_rset_mbboDirectRSET, pvar_func_mbboDirectRecordSizeOffset},
    {pvar_rset_permissiveRSET, pvar_func_permissiveRecordSizeOffset},
    {pvar_rset_selRSET, pvar_func_selRecordSizeOffset},
    {pvar_rset_seqRSET, pvar_func_seqRecordSizeOffset},
    {pvar_rset_stateRSET, pvar_func_stateRecordSizeOffset},
    {pvar_rset_stringinRSET, pvar_func_stringinRecordSizeOffset},
    {pvar_rset_stringoutRSET, pvar_func_stringoutRecordSizeOffset},
    {pvar_rset_subRSET, pvar_func_subRecordSizeOffset},
    {pvar_rset_subArrayRSET, pvar_func_subArrayRecordSizeOffset},
    {pvar_rset_waveformRSET, pvar_func_waveformRecordSizeOffset},
    {pvar_rset_asynRSET, pvar_func_asynRecordSizeOffset},
    {pvar_rset_scalerRSET, pvar_func_scalerRecordSizeOffset},
    {pvar_rset_sseqRSET, pvar_func_sseqRecordSizeOffset},
    {pvar_rset_epidRSET, pvar_func_epidRecordSizeOffset},
    {pvar_rset_timestampRSET, pvar_func_timestampRecordSizeOffset},
    {pvar_rset_transformRSET, pvar_func_transformRecordSizeOffset},
    {pvar_rset_scalcoutRSET, pvar_func_scalcoutRecordSizeOffset},
    {pvar_rset_acalcoutRSET, pvar_func_acalcoutRecordSizeOffset},
    {pvar_rset_swaitRSET, pvar_func_swaitRecordSizeOffset},
    {pvar_rset_busyRSET, pvar_func_busyRecordSizeOffset},
    {pvar_rset_scanparmRSET, pvar_func_scanparmRecordSizeOffset},
    {pvar_rset_sscanRSET, pvar_func_sscanRecordSizeOffset},
    {pvar_rset_tableRSET, pvar_func_tableRecordSizeOffset},
    {pvar_rset_mcaRSET, pvar_func_mcaRecordSizeOffset},
    {pvar_rset_motorRSET, pvar_func_motorRecordSizeOffset}
};

epicsShareExtern dset *pvar_dset_devAaiSoft;
epicsShareExtern dset *pvar_dset_devAaoSoft;
epicsShareExtern dset *pvar_dset_devAiSoft;
epicsShareExtern dset *pvar_dset_devAiSoftRaw;
epicsShareExtern dset *pvar_dset_devTimestampAI;
epicsShareExtern dset *pvar_dset_devAiGeneralTime;
epicsShareExtern dset *pvar_dset_asynAiInt32;
epicsShareExtern dset *pvar_dset_asynAiInt32Average;
epicsShareExtern dset *pvar_dset_asynAiFloat64;
epicsShareExtern dset *pvar_dset_asynAiFloat64Average;
epicsShareExtern dset *pvar_dset_devGpib;
epicsShareExtern dset *pvar_dset_devAiTodSeconds;
epicsShareExtern dset *pvar_dset_devAiStrParm;
epicsShareExtern dset *pvar_dset_devAiHeidND261;
epicsShareExtern dset *pvar_dset_devAiMKS;
epicsShareExtern dset *pvar_dset_devAiMPC;
epicsShareExtern dset *pvar_dset_devAiGP307Gpib;
epicsShareExtern dset *pvar_dset_devAiAX301;
epicsShareExtern dset *pvar_dset_devAiTelevac;
epicsShareExtern dset *pvar_dset_devAiTPG261;
epicsShareExtern dset *pvar_dset_devaiStream;
epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devAoSoftRaw;
epicsShareExtern dset *pvar_dset_devAoSoftCallback;
epicsShareExtern dset *pvar_dset_asynAoInt32;
epicsShareExtern dset *pvar_dset_asynAoFloat64;
epicsShareExtern dset *pvar_dset_devAoStrParm;
epicsShareExtern dset *pvar_dset_devAoEurotherm;
epicsShareExtern dset *pvar_dset_devAoMPC;
epicsShareExtern dset *pvar_dset_devAoAX301;
epicsShareExtern dset *pvar_dset_devAoTPG261;
epicsShareExtern dset *pvar_dset_devaoStream;
epicsShareExtern dset *pvar_dset_devBiSoft;
epicsShareExtern dset *pvar_dset_devBiSoftRaw;
epicsShareExtern dset *pvar_dset_asynBiInt32;
epicsShareExtern dset *pvar_dset_asynBiUInt32Digital;
epicsShareExtern dset *pvar_dset_devBiStrParm;
epicsShareExtern dset *pvar_dset_devBiMPC;
epicsShareExtern dset *pvar_dset_devBiGP307Gpib;
epicsShareExtern dset *pvar_dset_devBiTelevac;
epicsShareExtern dset *pvar_dset_devBiTPG261;
epicsShareExtern dset *pvar_dset_devbiStream;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devBoSoftRaw;
epicsShareExtern dset *pvar_dset_devBoSoftCallback;
epicsShareExtern dset *pvar_dset_devBoGeneralTime;
epicsShareExtern dset *pvar_dset_asynBoInt32;
epicsShareExtern dset *pvar_dset_asynBoUInt32Digital;
epicsShareExtern dset *pvar_dset_devBoStrParm;
epicsShareExtern dset *pvar_dset_devBoMPC;
epicsShareExtern dset *pvar_dset_devBoGP307Gpib;
epicsShareExtern dset *pvar_dset_devBoAX301;
epicsShareExtern dset *pvar_dset_devBoTPG261;
epicsShareExtern dset *pvar_dset_devboStream;
epicsShareExtern dset *pvar_dset_devCalcoutSoft;
epicsShareExtern dset *pvar_dset_devCalcoutSoftCallback;
epicsShareExtern dset *pvar_dset_devcalcoutStream;
epicsShareExtern dset *pvar_dset_devEventSoft;
epicsShareExtern dset *pvar_dset_devLiSoft;
epicsShareExtern dset *pvar_dset_devLiGeneralTime;
epicsShareExtern dset *pvar_dset_asynLiInt32;
epicsShareExtern dset *pvar_dset_asynLiUInt32Digital;
epicsShareExtern dset *pvar_dset_devLiStrParm;
epicsShareExtern dset *pvar_dset_devlonginStream;
epicsShareExtern dset *pvar_dset_devLoSoft;
epicsShareExtern dset *pvar_dset_devLoSoftCallback;
epicsShareExtern dset *pvar_dset_asynLoInt32;
epicsShareExtern dset *pvar_dset_asynLoUInt32Digital;
epicsShareExtern dset *pvar_dset_devLoStrParm;
epicsShareExtern dset *pvar_dset_devLoAX301;
epicsShareExtern dset *pvar_dset_devlongoutStream;
epicsShareExtern dset *pvar_dset_devMbbiSoft;
epicsShareExtern dset *pvar_dset_devMbbiSoftRaw;
epicsShareExtern dset *pvar_dset_asynMbbiInt32;
epicsShareExtern dset *pvar_dset_asynMbbiUInt32Digital;
epicsShareExtern dset *pvar_dset_devMbbiTPG261;
epicsShareExtern dset *pvar_dset_devmbbiStream;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoft;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoftRaw;
epicsShareExtern dset *pvar_dset_asynMbbiDirectUInt32Digital;
epicsShareExtern dset *pvar_dset_devmbbiDirectStream;
epicsShareExtern dset *pvar_dset_devMbboSoft;
epicsShareExtern dset *pvar_dset_devMbboSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoftCallback;
epicsShareExtern dset *pvar_dset_asynMbboInt32;
epicsShareExtern dset *pvar_dset_asynMbboUInt32Digital;
epicsShareExtern dset *pvar_dset_devMbboMPC;
epicsShareExtern dset *pvar_dset_devMbboTPG261;
epicsShareExtern dset *pvar_dset_devmbboStream;
epicsShareExtern dset *pvar_dset_devMbboDirectSoft;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftCallback;
epicsShareExtern dset *pvar_dset_asynMbboDirectUInt32Digital;
epicsShareExtern dset *pvar_dset_devmbboDirectStream;
epicsShareExtern dset *pvar_dset_devSiSoft;
epicsShareExtern dset *pvar_dset_devTimestampSI;
epicsShareExtern dset *pvar_dset_devSiGeneralTime;
epicsShareExtern dset *pvar_dset_asynSiOctetCmdResponse;
epicsShareExtern dset *pvar_dset_asynSiOctetWriteRead;
epicsShareExtern dset *pvar_dset_asynSiOctetRead;
epicsShareExtern dset *pvar_dset_devSiTodString;
epicsShareExtern dset *pvar_dset_devSiStrParm;
epicsShareExtern dset *pvar_dset_devSiMPC;
epicsShareExtern dset *pvar_dset_devSiGP307Gpib;
epicsShareExtern dset *pvar_dset_devSiTPG261;
epicsShareExtern dset *pvar_dset_devSiSeq;
epicsShareExtern dset *pvar_dset_devstringinStream;
epicsShareExtern dset *pvar_dset_devSoSoft;
epicsShareExtern dset *pvar_dset_devSoSoftCallback;
epicsShareExtern dset *pvar_dset_devSoStdio;
epicsShareExtern dset *pvar_dset_asynSoOctetWrite;
epicsShareExtern dset *pvar_dset_devSoStrParm;
epicsShareExtern dset *pvar_dset_devSoEurotherm;
epicsShareExtern dset *pvar_dset_devSoMPC;
epicsShareExtern dset *pvar_dset_devstringoutStream;
epicsShareExtern dset *pvar_dset_devSASoft;
epicsShareExtern dset *pvar_dset_devWfSoft;
epicsShareExtern dset *pvar_dset_asynWfOctetCmdResponse;
epicsShareExtern dset *pvar_dset_asynWfOctetWriteRead;
epicsShareExtern dset *pvar_dset_asynWfOctetRead;
epicsShareExtern dset *pvar_dset_asynWfOctetWrite;
epicsShareExtern dset *pvar_dset_asynInt8ArrayWfIn;
epicsShareExtern dset *pvar_dset_asynInt8ArrayWfOut;
epicsShareExtern dset *pvar_dset_asynInt16ArrayWfIn;
epicsShareExtern dset *pvar_dset_asynInt16ArrayWfOut;
epicsShareExtern dset *pvar_dset_asynInt32ArrayWfIn;
epicsShareExtern dset *pvar_dset_asynInt32ArrayWfOut;
epicsShareExtern dset *pvar_dset_asynFloat32ArrayWfIn;
epicsShareExtern dset *pvar_dset_asynFloat32ArrayWfOut;
epicsShareExtern dset *pvar_dset_asynFloat64ArrayWfIn;
epicsShareExtern dset *pvar_dset_asynFloat64ArrayWfOut;
epicsShareExtern dset *pvar_dset_devwaveformStream;
epicsShareExtern dset *pvar_dset_asynRecordDevice;
epicsShareExtern dset *pvar_dset_devScalerAsyn;
epicsShareExtern dset *pvar_dset_devEpidSoft;
epicsShareExtern dset *pvar_dset_devEpidSoftCB;
epicsShareExtern dset *pvar_dset_devEpidFast;
epicsShareExtern dset *pvar_dset_devsCalcoutSoft;
epicsShareExtern dset *pvar_dset_devscalcoutStream;
epicsShareExtern dset *pvar_dset_devaCalcoutSoft;
epicsShareExtern dset *pvar_dset_devSWaitIoEvent;
epicsShareExtern dset *pvar_dset_devBusySoft;
epicsShareExtern dset *pvar_dset_devBusySoftRaw;
epicsShareExtern dset *pvar_dset_asynBusyInt32;
epicsShareExtern dset *pvar_dset_devMCA_soft;
epicsShareExtern dset *pvar_dset_devMcaAsyn;
epicsShareExtern dset *pvar_dset_devMotorAsyn;
epicsShareExtern dset *pvar_dset_devMCB4B;
epicsShareExtern dset *pvar_dset_devEnsemble;
epicsShareExtern dset *pvar_dset_devSoloist;
epicsShareExtern dset *pvar_dset_devMCDC2805;
epicsShareExtern dset *pvar_dset_devIM483SM;
epicsShareExtern dset *pvar_dset_devIM483PL;
epicsShareExtern dset *pvar_dset_devMDrive;
epicsShareExtern dset *pvar_dset_devSC800;
epicsShareExtern dset *pvar_dset_devPM304;
epicsShareExtern dset *pvar_dset_devMicos;
epicsShareExtern dset *pvar_dset_devMVP2001;
epicsShareExtern dset *pvar_dset_devPMNC87xx;
epicsShareExtern dset *pvar_dset_devMM3000;
epicsShareExtern dset *pvar_dset_devMM4000;
epicsShareExtern dset *pvar_dset_devPM500;
epicsShareExtern dset *pvar_dset_devESP300;
epicsShareExtern dset *pvar_dset_devEMC18011;
epicsShareExtern dset *pvar_dset_devPC6K;
epicsShareExtern dset *pvar_dset_devPIJEDS;
epicsShareExtern dset *pvar_dset_devPIC844;
epicsShareExtern dset *pvar_dset_devPIC630;
epicsShareExtern dset *pvar_dset_devPIC848;
epicsShareExtern dset *pvar_dset_devPIC662;
epicsShareExtern dset *pvar_dset_devPIC862;
epicsShareExtern dset *pvar_dset_devPIE710;
epicsShareExtern dset *pvar_dset_devPIE516;
epicsShareExtern dset *pvar_dset_devPIE816;
epicsShareExtern dset *pvar_dset_devSPiiPlus;
epicsShareExtern dset *pvar_dset_devSmartMotor;
epicsShareExtern dset *pvar_dset_devMotorSoft;
epicsShareExtern dset *pvar_dset_devMDT695;
epicsShareExtern dset *pvar_dset_devMotorSim;

static const char * const deviceSupportNames[179] = {
    "devAaiSoft",
    "devAaoSoft",
    "devAiSoft",
    "devAiSoftRaw",
    "devTimestampAI",
    "devAiGeneralTime",
    "asynAiInt32",
    "asynAiInt32Average",
    "asynAiFloat64",
    "asynAiFloat64Average",
    "devGpib",
    "devAiTodSeconds",
    "devAiStrParm",
    "devAiHeidND261",
    "devAiMKS",
    "devAiMPC",
    "devAiGP307Gpib",
    "devAiAX301",
    "devAiTelevac",
    "devAiTPG261",
    "devaiStream",
    "devAoSoft",
    "devAoSoftRaw",
    "devAoSoftCallback",
    "asynAoInt32",
    "asynAoFloat64",
    "devAoStrParm",
    "devAoEurotherm",
    "devAoMPC",
    "devAoAX301",
    "devAoTPG261",
    "devaoStream",
    "devBiSoft",
    "devBiSoftRaw",
    "asynBiInt32",
    "asynBiUInt32Digital",
    "devBiStrParm",
    "devBiMPC",
    "devBiGP307Gpib",
    "devBiTelevac",
    "devBiTPG261",
    "devbiStream",
    "devBoSoft",
    "devBoSoftRaw",
    "devBoSoftCallback",
    "devBoGeneralTime",
    "asynBoInt32",
    "asynBoUInt32Digital",
    "devBoStrParm",
    "devBoMPC",
    "devBoGP307Gpib",
    "devBoAX301",
    "devBoTPG261",
    "devboStream",
    "devCalcoutSoft",
    "devCalcoutSoftCallback",
    "devcalcoutStream",
    "devEventSoft",
    "devLiSoft",
    "devLiGeneralTime",
    "asynLiInt32",
    "asynLiUInt32Digital",
    "devLiStrParm",
    "devlonginStream",
    "devLoSoft",
    "devLoSoftCallback",
    "asynLoInt32",
    "asynLoUInt32Digital",
    "devLoStrParm",
    "devLoAX301",
    "devlongoutStream",
    "devMbbiSoft",
    "devMbbiSoftRaw",
    "asynMbbiInt32",
    "asynMbbiUInt32Digital",
    "devMbbiTPG261",
    "devmbbiStream",
    "devMbbiDirectSoft",
    "devMbbiDirectSoftRaw",
    "asynMbbiDirectUInt32Digital",
    "devmbbiDirectStream",
    "devMbboSoft",
    "devMbboSoftRaw",
    "devMbboSoftCallback",
    "asynMbboInt32",
    "asynMbboUInt32Digital",
    "devMbboMPC",
    "devMbboTPG261",
    "devmbboStream",
    "devMbboDirectSoft",
    "devMbboDirectSoftRaw",
    "devMbboDirectSoftCallback",
    "asynMbboDirectUInt32Digital",
    "devmbboDirectStream",
    "devSiSoft",
    "devTimestampSI",
    "devSiGeneralTime",
    "asynSiOctetCmdResponse",
    "asynSiOctetWriteRead",
    "asynSiOctetRead",
    "devSiTodString",
    "devSiStrParm",
    "devSiMPC",
    "devSiGP307Gpib",
    "devSiTPG261",
    "devSiSeq",
    "devstringinStream",
    "devSoSoft",
    "devSoSoftCallback",
    "devSoStdio",
    "asynSoOctetWrite",
    "devSoStrParm",
    "devSoEurotherm",
    "devSoMPC",
    "devstringoutStream",
    "devSASoft",
    "devWfSoft",
    "asynWfOctetCmdResponse",
    "asynWfOctetWriteRead",
    "asynWfOctetRead",
    "asynWfOctetWrite",
    "asynInt8ArrayWfIn",
    "asynInt8ArrayWfOut",
    "asynInt16ArrayWfIn",
    "asynInt16ArrayWfOut",
    "asynInt32ArrayWfIn",
    "asynInt32ArrayWfOut",
    "asynFloat32ArrayWfIn",
    "asynFloat32ArrayWfOut",
    "asynFloat64ArrayWfIn",
    "asynFloat64ArrayWfOut",
    "devwaveformStream",
    "asynRecordDevice",
    "devScalerAsyn",
    "devEpidSoft",
    "devEpidSoftCB",
    "devEpidFast",
    "devsCalcoutSoft",
    "devscalcoutStream",
    "devaCalcoutSoft",
    "devSWaitIoEvent",
    "devBusySoft",
    "devBusySoftRaw",
    "asynBusyInt32",
    "devMCA_soft",
    "devMcaAsyn",
    "devMotorAsyn",
    "devMCB4B",
    "devEnsemble",
    "devSoloist",
    "devMCDC2805",
    "devIM483SM",
    "devIM483PL",
    "devMDrive",
    "devSC800",
    "devPM304",
    "devMicos",
    "devMVP2001",
    "devPMNC87xx",
    "devMM3000",
    "devMM4000",
    "devPM500",
    "devESP300",
    "devEMC18011",
    "devPC6K",
    "devPIJEDS",
    "devPIC844",
    "devPIC630",
    "devPIC848",
    "devPIC662",
    "devPIC862",
    "devPIE710",
    "devPIE516",
    "devPIE816",
    "devSPiiPlus",
    "devSmartMotor",
    "devMotorSoft",
    "devMDT695",
    "devMotorSim"
};

static const dset * const devsl[179] = {
    pvar_dset_devAaiSoft,
    pvar_dset_devAaoSoft,
    pvar_dset_devAiSoft,
    pvar_dset_devAiSoftRaw,
    pvar_dset_devTimestampAI,
    pvar_dset_devAiGeneralTime,
    pvar_dset_asynAiInt32,
    pvar_dset_asynAiInt32Average,
    pvar_dset_asynAiFloat64,
    pvar_dset_asynAiFloat64Average,
    pvar_dset_devGpib,
    pvar_dset_devAiTodSeconds,
    pvar_dset_devAiStrParm,
    pvar_dset_devAiHeidND261,
    pvar_dset_devAiMKS,
    pvar_dset_devAiMPC,
    pvar_dset_devAiGP307Gpib,
    pvar_dset_devAiAX301,
    pvar_dset_devAiTelevac,
    pvar_dset_devAiTPG261,
    pvar_dset_devaiStream,
    pvar_dset_devAoSoft,
    pvar_dset_devAoSoftRaw,
    pvar_dset_devAoSoftCallback,
    pvar_dset_asynAoInt32,
    pvar_dset_asynAoFloat64,
    pvar_dset_devAoStrParm,
    pvar_dset_devAoEurotherm,
    pvar_dset_devAoMPC,
    pvar_dset_devAoAX301,
    pvar_dset_devAoTPG261,
    pvar_dset_devaoStream,
    pvar_dset_devBiSoft,
    pvar_dset_devBiSoftRaw,
    pvar_dset_asynBiInt32,
    pvar_dset_asynBiUInt32Digital,
    pvar_dset_devBiStrParm,
    pvar_dset_devBiMPC,
    pvar_dset_devBiGP307Gpib,
    pvar_dset_devBiTelevac,
    pvar_dset_devBiTPG261,
    pvar_dset_devbiStream,
    pvar_dset_devBoSoft,
    pvar_dset_devBoSoftRaw,
    pvar_dset_devBoSoftCallback,
    pvar_dset_devBoGeneralTime,
    pvar_dset_asynBoInt32,
    pvar_dset_asynBoUInt32Digital,
    pvar_dset_devBoStrParm,
    pvar_dset_devBoMPC,
    pvar_dset_devBoGP307Gpib,
    pvar_dset_devBoAX301,
    pvar_dset_devBoTPG261,
    pvar_dset_devboStream,
    pvar_dset_devCalcoutSoft,
    pvar_dset_devCalcoutSoftCallback,
    pvar_dset_devcalcoutStream,
    pvar_dset_devEventSoft,
    pvar_dset_devLiSoft,
    pvar_dset_devLiGeneralTime,
    pvar_dset_asynLiInt32,
    pvar_dset_asynLiUInt32Digital,
    pvar_dset_devLiStrParm,
    pvar_dset_devlonginStream,
    pvar_dset_devLoSoft,
    pvar_dset_devLoSoftCallback,
    pvar_dset_asynLoInt32,
    pvar_dset_asynLoUInt32Digital,
    pvar_dset_devLoStrParm,
    pvar_dset_devLoAX301,
    pvar_dset_devlongoutStream,
    pvar_dset_devMbbiSoft,
    pvar_dset_devMbbiSoftRaw,
    pvar_dset_asynMbbiInt32,
    pvar_dset_asynMbbiUInt32Digital,
    pvar_dset_devMbbiTPG261,
    pvar_dset_devmbbiStream,
    pvar_dset_devMbbiDirectSoft,
    pvar_dset_devMbbiDirectSoftRaw,
    pvar_dset_asynMbbiDirectUInt32Digital,
    pvar_dset_devmbbiDirectStream,
    pvar_dset_devMbboSoft,
    pvar_dset_devMbboSoftRaw,
    pvar_dset_devMbboSoftCallback,
    pvar_dset_asynMbboInt32,
    pvar_dset_asynMbboUInt32Digital,
    pvar_dset_devMbboMPC,
    pvar_dset_devMbboTPG261,
    pvar_dset_devmbboStream,
    pvar_dset_devMbboDirectSoft,
    pvar_dset_devMbboDirectSoftRaw,
    pvar_dset_devMbboDirectSoftCallback,
    pvar_dset_asynMbboDirectUInt32Digital,
    pvar_dset_devmbboDirectStream,
    pvar_dset_devSiSoft,
    pvar_dset_devTimestampSI,
    pvar_dset_devSiGeneralTime,
    pvar_dset_asynSiOctetCmdResponse,
    pvar_dset_asynSiOctetWriteRead,
    pvar_dset_asynSiOctetRead,
    pvar_dset_devSiTodString,
    pvar_dset_devSiStrParm,
    pvar_dset_devSiMPC,
    pvar_dset_devSiGP307Gpib,
    pvar_dset_devSiTPG261,
    pvar_dset_devSiSeq,
    pvar_dset_devstringinStream,
    pvar_dset_devSoSoft,
    pvar_dset_devSoSoftCallback,
    pvar_dset_devSoStdio,
    pvar_dset_asynSoOctetWrite,
    pvar_dset_devSoStrParm,
    pvar_dset_devSoEurotherm,
    pvar_dset_devSoMPC,
    pvar_dset_devstringoutStream,
    pvar_dset_devSASoft,
    pvar_dset_devWfSoft,
    pvar_dset_asynWfOctetCmdResponse,
    pvar_dset_asynWfOctetWriteRead,
    pvar_dset_asynWfOctetRead,
    pvar_dset_asynWfOctetWrite,
    pvar_dset_asynInt8ArrayWfIn,
    pvar_dset_asynInt8ArrayWfOut,
    pvar_dset_asynInt16ArrayWfIn,
    pvar_dset_asynInt16ArrayWfOut,
    pvar_dset_asynInt32ArrayWfIn,
    pvar_dset_asynInt32ArrayWfOut,
    pvar_dset_asynFloat32ArrayWfIn,
    pvar_dset_asynFloat32ArrayWfOut,
    pvar_dset_asynFloat64ArrayWfIn,
    pvar_dset_asynFloat64ArrayWfOut,
    pvar_dset_devwaveformStream,
    pvar_dset_asynRecordDevice,
    pvar_dset_devScalerAsyn,
    pvar_dset_devEpidSoft,
    pvar_dset_devEpidSoftCB,
    pvar_dset_devEpidFast,
    pvar_dset_devsCalcoutSoft,
    pvar_dset_devscalcoutStream,
    pvar_dset_devaCalcoutSoft,
    pvar_dset_devSWaitIoEvent,
    pvar_dset_devBusySoft,
    pvar_dset_devBusySoftRaw,
    pvar_dset_asynBusyInt32,
    pvar_dset_devMCA_soft,
    pvar_dset_devMcaAsyn,
    pvar_dset_devMotorAsyn,
    pvar_dset_devMCB4B,
    pvar_dset_devEnsemble,
    pvar_dset_devSoloist,
    pvar_dset_devMCDC2805,
    pvar_dset_devIM483SM,
    pvar_dset_devIM483PL,
    pvar_dset_devMDrive,
    pvar_dset_devSC800,
    pvar_dset_devPM304,
    pvar_dset_devMicos,
    pvar_dset_devMVP2001,
    pvar_dset_devPMNC87xx,
    pvar_dset_devMM3000,
    pvar_dset_devMM4000,
    pvar_dset_devPM500,
    pvar_dset_devESP300,
    pvar_dset_devEMC18011,
    pvar_dset_devPC6K,
    pvar_dset_devPIJEDS,
    pvar_dset_devPIC844,
    pvar_dset_devPIC630,
    pvar_dset_devPIC848,
    pvar_dset_devPIC662,
    pvar_dset_devPIC862,
    pvar_dset_devPIE710,
    pvar_dset_devPIE516,
    pvar_dset_devPIE816,
    pvar_dset_devSPiiPlus,
    pvar_dset_devSmartMotor,
    pvar_dset_devMotorSoft,
    pvar_dset_devMDT695,
    pvar_dset_devMotorSim
};

epicsShareExtern drvet *pvar_drvet_drvAsyn;
epicsShareExtern drvet *pvar_drvet_drvMCB4B;
epicsShareExtern drvet *pvar_drvet_drvEnsemble;
epicsShareExtern drvet *pvar_drvet_drvSoloist;
epicsShareExtern drvet *pvar_drvet_motorEnsemble;
epicsShareExtern drvet *pvar_drvet_motorANC150;
epicsShareExtern drvet *pvar_drvet_drvMCDC2805;
epicsShareExtern drvet *pvar_drvet_drvIM483SM;
epicsShareExtern drvet *pvar_drvet_drvIM483PL;
epicsShareExtern drvet *pvar_drvet_drvMDrive;
epicsShareExtern drvet *pvar_drvet_drvSC800;
epicsShareExtern drvet *pvar_drvet_drvPM304;
epicsShareExtern drvet *pvar_drvet_drvMicos;
epicsShareExtern drvet *pvar_drvet_drvMVP2001;
epicsShareExtern drvet *pvar_drvet_drvPMNC87xx;
epicsShareExtern drvet *pvar_drvet_drvMM3000;
epicsShareExtern drvet *pvar_drvet_drvMM4000;
epicsShareExtern drvet *pvar_drvet_drvPM500;
epicsShareExtern drvet *pvar_drvet_drvESP300;
epicsShareExtern drvet *pvar_drvet_motorXPS;
epicsShareExtern drvet *pvar_drvet_motorMM4000;
epicsShareExtern drvet *pvar_drvet_drvEMC18011;
epicsShareExtern drvet *pvar_drvet_drvPC6K;
epicsShareExtern drvet *pvar_drvet_drvPIJEDS;
epicsShareExtern drvet *pvar_drvet_drvPIC844;
epicsShareExtern drvet *pvar_drvet_drvPIC630;
epicsShareExtern drvet *pvar_drvet_drvPIC848;
epicsShareExtern drvet *pvar_drvet_drvPIC662;
epicsShareExtern drvet *pvar_drvet_drvPIC862;
epicsShareExtern drvet *pvar_drvet_drvPIE710;
epicsShareExtern drvet *pvar_drvet_drvPIE516;
epicsShareExtern drvet *pvar_drvet_drvPIE816;
epicsShareExtern drvet *pvar_drvet_drvSPiiPlus;
epicsShareExtern drvet *pvar_drvet_drvSmartMotor;
epicsShareExtern drvet *pvar_drvet_drvMDT695;
epicsShareExtern drvet *pvar_drvet_motorSim;
epicsShareExtern drvet *pvar_drvet_stream;

static const char *driverSupportNames[37] = {
    "drvAsyn",
    "drvMCB4B",
    "drvEnsemble",
    "drvSoloist",
    "motorEnsemble",
    "motorANC150",
    "drvMCDC2805",
    "drvIM483SM",
    "drvIM483PL",
    "drvMDrive",
    "drvSC800",
    "drvPM304",
    "drvMicos",
    "drvMVP2001",
    "drvPMNC87xx",
    "drvMM3000",
    "drvMM4000",
    "drvPM500",
    "drvESP300",
    "motorXPS",
    "motorMM4000",
    "drvEMC18011",
    "drvPC6K",
    "drvPIJEDS",
    "drvPIC844",
    "drvPIC630",
    "drvPIC848",
    "drvPIC662",
    "drvPIC862",
    "drvPIE710",
    "drvPIE516",
    "drvPIE816",
    "drvSPiiPlus",
    "drvSmartMotor",
    "drvMDT695",
    "motorSim",
    "stream"
};

static struct drvet *drvsl[37] = {
    pvar_drvet_drvAsyn,
    pvar_drvet_drvMCB4B,
    pvar_drvet_drvEnsemble,
    pvar_drvet_drvSoloist,
    pvar_drvet_motorEnsemble,
    pvar_drvet_motorANC150,
    pvar_drvet_drvMCDC2805,
    pvar_drvet_drvIM483SM,
    pvar_drvet_drvIM483PL,
    pvar_drvet_drvMDrive,
    pvar_drvet_drvSC800,
    pvar_drvet_drvPM304,
    pvar_drvet_drvMicos,
    pvar_drvet_drvMVP2001,
    pvar_drvet_drvPMNC87xx,
    pvar_drvet_drvMM3000,
    pvar_drvet_drvMM4000,
    pvar_drvet_drvPM500,
    pvar_drvet_drvESP300,
    pvar_drvet_motorXPS,
    pvar_drvet_motorMM4000,
    pvar_drvet_drvEMC18011,
    pvar_drvet_drvPC6K,
    pvar_drvet_drvPIJEDS,
    pvar_drvet_drvPIC844,
    pvar_drvet_drvPIC630,
    pvar_drvet_drvPIC848,
    pvar_drvet_drvPIC662,
    pvar_drvet_drvPIC862,
    pvar_drvet_drvPIE710,
    pvar_drvet_drvPIE516,
    pvar_drvet_drvPIE816,
    pvar_drvet_drvSPiiPlus,
    pvar_drvet_drvSmartMotor,
    pvar_drvet_drvMDT695,
    pvar_drvet_motorSim,
    pvar_drvet_stream
};

epicsShareExtern void (*pvar_func_asSub)(void);
epicsShareExtern void (*pvar_func_asynRegister)(void);
epicsShareExtern void (*pvar_func_asynInterposeFlushRegister)(void);
epicsShareExtern void (*pvar_func_asynInterposeEosRegister)(void);
epicsShareExtern void (*pvar_func_drvAsynSerialPortRegisterCommands)(void);
epicsShareExtern void (*pvar_func_drvAsynIPPortRegisterCommands)(void);
epicsShareExtern void (*pvar_func_drvAsynIPServerPortRegisterCommands)(void);
epicsShareExtern void (*pvar_func_vxi11RegisterCommands)(void);
epicsShareExtern void (*pvar_func_pvHistoryRegister)(void);
epicsShareExtern void (*pvar_func_drvScalerSoftRegister)(void);
epicsShareExtern void (*pvar_func_femtoRegistrar)(void);
epicsShareExtern void (*pvar_func_Scaler974Register)(void);
epicsShareExtern void (*pvar_func_calcRegister)(void);
epicsShareExtern void (*pvar_func_subAveRegister)(void);
epicsShareExtern void (*pvar_func_interpRegister)(void);
epicsShareExtern void (*pvar_func_arrayTestRegister)(void);
epicsShareExtern void (*pvar_func_saveDataRegistrar)(void);
epicsShareExtern void (*pvar_func_save_restoreRegister)(void);
epicsShareExtern void (*pvar_func_dbrestoreRegister)(void);
epicsShareExtern void (*pvar_func_asInitHooksRegister)(void);
epicsShareExtern void (*pvar_func_kohzuCtlRegistrar)(void);
epicsShareExtern void (*pvar_func_hrCtlRegistrar)(void);
epicsShareExtern void (*pvar_func_xiahscRegistrar)(void);
epicsShareExtern void (*pvar_func_xia_slitRegistrar)(void);
epicsShareExtern void (*pvar_func_orientRegistrar)(void);
epicsShareExtern void (*pvar_func_pf4Registrar)(void);
epicsShareExtern void (*pvar_func_IoRegistrar)(void);
epicsShareExtern void (*pvar_func_ml_monoCtlRegistrar)(void);
epicsShareExtern void (*pvar_func_fastSweepRegister)(void);
epicsShareExtern void (*pvar_func_Keithley2kDMMRegistrar)(void);
epicsShareExtern void (*pvar_func_Keithley65xxEMRegistrar)(void);
epicsShareExtern void (*pvar_func_motorUtilRegister)(void);
epicsShareExtern void (*pvar_func_motorRegister)(void);
epicsShareExtern void (*pvar_func_AcsRegister)(void);
epicsShareExtern void (*pvar_func_AerotechRegister)(void);
epicsShareExtern void (*pvar_func_ANC150Register)(void);
epicsShareExtern void (*pvar_func_MCDC2805Register)(void);
epicsShareExtern void (*pvar_func_IMSmotorRegister)(void);
epicsShareExtern void (*pvar_func_SC800Register)(void);
epicsShareExtern void (*pvar_func_MclennanRegister)(void);
epicsShareExtern void (*pvar_func_NewFocusRegister)(void);
epicsShareExtern void (*pvar_func_NewportRegister)(void);
epicsShareExtern void (*pvar_func_XPSGatheringRegister)(void);
epicsShareExtern void (*pvar_func_XPSRegister)(void);
epicsShareExtern void (*pvar_func_XPSInterposeRegister)(void);
epicsShareExtern void (*pvar_func_drvXPSAsynAuxRegister)(void);
epicsShareExtern void (*pvar_func_MM4005_trajectoryScanRegistrar)(void);
epicsShareExtern void (*pvar_func_XPS_trajectoryScanRegistrar)(void);
epicsShareExtern void (*pvar_func_OrielRegister)(void);
epicsShareExtern void (*pvar_func_ParkerRegister)(void);
epicsShareExtern void (*pvar_func_PIJEDSmotorRegister)(void);
epicsShareExtern void (*pvar_func_PImotorRegister)(void);
epicsShareExtern void (*pvar_func_PIC630Register)(void);
epicsShareExtern void (*pvar_func_PIC848motorRegister)(void);
epicsShareExtern void (*pvar_func_PIC662motorRegister)(void);
epicsShareExtern void (*pvar_func_PIC862motorRegister)(void);
epicsShareExtern void (*pvar_func_PIE710motorRegister)(void);
epicsShareExtern void (*pvar_func_PIE516motorRegister)(void);
epicsShareExtern void (*pvar_func_PIE816motorRegister)(void);
epicsShareExtern void (*pvar_func_ACSTech80Register)(void);
epicsShareExtern void (*pvar_func_SmartMotormotorRegister)(void);
epicsShareExtern void (*pvar_func_ThorLabsRegister)(void);
epicsShareExtern void (*pvar_func_motorSimRegister)(void);
epicsShareExtern void (*pvar_func_motorSimDriverRegister)(void);
epicsShareExtern void (*pvar_func_streamRegistrar)(void);
epicsShareExtern void (*pvar_func_simDetectorRegister)(void);
epicsShareExtern void (*pvar_func_NDStdArraysRegister)(void);
epicsShareExtern void (*pvar_func_NDFileNetCDFRegister)(void);
epicsShareExtern void (*pvar_func_NDFileTIFFRegister)(void);
epicsShareExtern void (*pvar_func_NDFileJPEGRegister)(void);
epicsShareExtern void (*pvar_func_NDFileNexusRegister)(void);
epicsShareExtern void (*pvar_func_NDROIRegister)(void);
epicsShareExtern void (*pvar_func_NDProcessRegister)(void);
epicsShareExtern void (*pvar_func_NDStatsRegister)(void);
epicsShareExtern void (*pvar_func_NDTransformRegister)(void);
epicsShareExtern void (*pvar_func_NDOverlayRegister)(void);
epicsShareExtern void (*pvar_func_NDColorConvertRegister)(void);

epicsShareExtern int *pvar_int_asCaDebug;
epicsShareExtern int *pvar_int_dbRecordsOnceOnly;
epicsShareExtern int *pvar_int_dbBptNotMonotonic;
epicsShareExtern int *pvar_int_sseqRecDebug;
epicsShareExtern int *pvar_int_pvHistoryDebug;
epicsShareExtern int *pvar_int_debugSubAve;
epicsShareExtern int *pvar_int_sCalcPostfixDebug;
epicsShareExtern int *pvar_int_sCalcPerformDebug;
epicsShareExtern int *pvar_int_sCalcoutRecordDebug;
epicsShareExtern int *pvar_int_devsCalcoutSoftDebug;
epicsShareExtern int *pvar_int_aCalcPostfixDebug;
epicsShareExtern int *pvar_int_aCalcPerformDebug;
epicsShareExtern int *pvar_int_aCalcoutRecordDebug;
epicsShareExtern int *pvar_int_devaCalcoutSoftDebug;
epicsShareExtern int *pvar_int_aCalcArraySize;
epicsShareExtern int *pvar_int_transformRecordDebug;
epicsShareExtern int *pvar_int_interpDebug;
epicsShareExtern int *pvar_int_arrayTestDebug;
epicsShareExtern int *pvar_int_recDynLinkDebug;
epicsShareExtern int *pvar_int_recDynLinkQsize;
epicsShareExtern int *pvar_int_debug_saveData;
epicsShareExtern int *pvar_int_debug_saveDataMsg;
epicsShareExtern int *pvar_int_saveData_MessagePolicy;
epicsShareExtern int *pvar_int_sscanRecordDebug;
epicsShareExtern int *pvar_int_sscanRecordViewPos;
epicsShareExtern int *pvar_int_sscanRecordDontCheckLimits;
epicsShareExtern int *pvar_int_sscanRecordLookupTime;
epicsShareExtern int *pvar_int_sscanRecordConnectWaitSeconds;
epicsShareExtern int *pvar_int_save_restoreDebug;
epicsShareExtern int *pvar_int_save_restoreNumSeqFiles;
epicsShareExtern int *pvar_int_save_restoreSeqPeriodInSeconds;
epicsShareExtern int *pvar_int_save_restoreIncompleteSetsOk;
epicsShareExtern int *pvar_int_save_restoreDatedBackupFiles;
epicsShareExtern int *pvar_int_save_restoreRemountThreshold;
epicsShareExtern int *pvar_int_mcaRecordDebug;
epicsShareExtern int *pvar_int_drvMCDC2805debug;
epicsShareExtern int *pvar_int_drvIM483SMdebug;
epicsShareExtern int *pvar_int_drvIM483PLdebug;
epicsShareExtern int *pvar_int_drvMDrivedebug;
epicsShareExtern int *pvar_int_drvSC800debug;
epicsShareExtern int *pvar_int_drvPMNC87xxdebug;
epicsShareExtern int *pvar_int_drvEMC18011debug;
epicsShareExtern int *pvar_int_drvPC6Kdebug;
epicsShareExtern int *pvar_int_drvPIJEDSdebug;
epicsShareExtern int *pvar_int_drvPIC844debug;
epicsShareExtern int *pvar_int_drvPIC630debug;
epicsShareExtern int *pvar_int_drvPIC848debug;
epicsShareExtern int *pvar_int_drvPIC662debug;
epicsShareExtern int *pvar_int_drvPIC862debug;
epicsShareExtern int *pvar_int_drvPIE710debug;
epicsShareExtern int *pvar_int_drvPIE516debug;
epicsShareExtern int *pvar_int_drvPIE816debug;
epicsShareExtern int *pvar_int_drvSmartMotordebug;
epicsShareExtern int *pvar_int_drvMDT695debug;
epicsShareExtern int *pvar_int_streamDebug;
epicsShareExtern int *pvar_int_eraseNDAttributes;
static struct iocshVarDef vardefs[] = {
	{"asCaDebug", iocshArgInt, (void * const)pvar_int_asCaDebug},
	{"dbRecordsOnceOnly", iocshArgInt, (void * const)pvar_int_dbRecordsOnceOnly},
	{"dbBptNotMonotonic", iocshArgInt, (void * const)pvar_int_dbBptNotMonotonic},
	{"sseqRecDebug", iocshArgInt, (void * const)pvar_int_sseqRecDebug},
	{"pvHistoryDebug", iocshArgInt, (void * const)pvar_int_pvHistoryDebug},
	{"debugSubAve", iocshArgInt, (void * const)pvar_int_debugSubAve},
	{"sCalcPostfixDebug", iocshArgInt, (void * const)pvar_int_sCalcPostfixDebug},
	{"sCalcPerformDebug", iocshArgInt, (void * const)pvar_int_sCalcPerformDebug},
	{"sCalcoutRecordDebug", iocshArgInt, (void * const)pvar_int_sCalcoutRecordDebug},
	{"devsCalcoutSoftDebug", iocshArgInt, (void * const)pvar_int_devsCalcoutSoftDebug},
	{"aCalcPostfixDebug", iocshArgInt, (void * const)pvar_int_aCalcPostfixDebug},
	{"aCalcPerformDebug", iocshArgInt, (void * const)pvar_int_aCalcPerformDebug},
	{"aCalcoutRecordDebug", iocshArgInt, (void * const)pvar_int_aCalcoutRecordDebug},
	{"devaCalcoutSoftDebug", iocshArgInt, (void * const)pvar_int_devaCalcoutSoftDebug},
	{"aCalcArraySize", iocshArgInt, (void * const)pvar_int_aCalcArraySize},
	{"transformRecordDebug", iocshArgInt, (void * const)pvar_int_transformRecordDebug},
	{"interpDebug", iocshArgInt, (void * const)pvar_int_interpDebug},
	{"arrayTestDebug", iocshArgInt, (void * const)pvar_int_arrayTestDebug},
	{"recDynLinkDebug", iocshArgInt, (void * const)pvar_int_recDynLinkDebug},
	{"recDynLinkQsize", iocshArgInt, (void * const)pvar_int_recDynLinkQsize},
	{"debug_saveData", iocshArgInt, (void * const)pvar_int_debug_saveData},
	{"debug_saveDataMsg", iocshArgInt, (void * const)pvar_int_debug_saveDataMsg},
	{"saveData_MessagePolicy", iocshArgInt, (void * const)pvar_int_saveData_MessagePolicy},
	{"sscanRecordDebug", iocshArgInt, (void * const)pvar_int_sscanRecordDebug},
	{"sscanRecordViewPos", iocshArgInt, (void * const)pvar_int_sscanRecordViewPos},
	{"sscanRecordDontCheckLimits", iocshArgInt, (void * const)pvar_int_sscanRecordDontCheckLimits},
	{"sscanRecordLookupTime", iocshArgInt, (void * const)pvar_int_sscanRecordLookupTime},
	{"sscanRecordConnectWaitSeconds", iocshArgInt, (void * const)pvar_int_sscanRecordConnectWaitSeconds},
	{"save_restoreDebug", iocshArgInt, (void * const)pvar_int_save_restoreDebug},
	{"save_restoreNumSeqFiles", iocshArgInt, (void * const)pvar_int_save_restoreNumSeqFiles},
	{"save_restoreSeqPeriodInSeconds", iocshArgInt, (void * const)pvar_int_save_restoreSeqPeriodInSeconds},
	{"save_restoreIncompleteSetsOk", iocshArgInt, (void * const)pvar_int_save_restoreIncompleteSetsOk},
	{"save_restoreDatedBackupFiles", iocshArgInt, (void * const)pvar_int_save_restoreDatedBackupFiles},
	{"save_restoreRemountThreshold", iocshArgInt, (void * const)pvar_int_save_restoreRemountThreshold},
	{"mcaRecordDebug", iocshArgInt, (void * const)pvar_int_mcaRecordDebug},
	{"drvMCDC2805debug", iocshArgInt, (void * const)pvar_int_drvMCDC2805debug},
	{"drvIM483SMdebug", iocshArgInt, (void * const)pvar_int_drvIM483SMdebug},
	{"drvIM483PLdebug", iocshArgInt, (void * const)pvar_int_drvIM483PLdebug},
	{"drvMDrivedebug", iocshArgInt, (void * const)pvar_int_drvMDrivedebug},
	{"drvSC800debug", iocshArgInt, (void * const)pvar_int_drvSC800debug},
	{"drvPMNC87xxdebug", iocshArgInt, (void * const)pvar_int_drvPMNC87xxdebug},
	{"drvEMC18011debug", iocshArgInt, (void * const)pvar_int_drvEMC18011debug},
	{"drvPC6Kdebug", iocshArgInt, (void * const)pvar_int_drvPC6Kdebug},
	{"drvPIJEDSdebug", iocshArgInt, (void * const)pvar_int_drvPIJEDSdebug},
	{"drvPIC844debug", iocshArgInt, (void * const)pvar_int_drvPIC844debug},
	{"drvPIC630debug", iocshArgInt, (void * const)pvar_int_drvPIC630debug},
	{"drvPIC848debug", iocshArgInt, (void * const)pvar_int_drvPIC848debug},
	{"drvPIC662debug", iocshArgInt, (void * const)pvar_int_drvPIC662debug},
	{"drvPIC862debug", iocshArgInt, (void * const)pvar_int_drvPIC862debug},
	{"drvPIE710debug", iocshArgInt, (void * const)pvar_int_drvPIE710debug},
	{"drvPIE516debug", iocshArgInt, (void * const)pvar_int_drvPIE516debug},
	{"drvPIE816debug", iocshArgInt, (void * const)pvar_int_drvPIE816debug},
	{"drvSmartMotordebug", iocshArgInt, (void * const)pvar_int_drvSmartMotordebug},
	{"drvMDT695debug", iocshArgInt, (void * const)pvar_int_drvMDT695debug},
	{"streamDebug", iocshArgInt, (void * const)pvar_int_streamDebug},
	{"eraseNDAttributes", iocshArgInt, (void * const)pvar_int_eraseNDAttributes},
	{NULL, iocshArgInt, NULL}
};

int iocxxxLinux_registerRecordDeviceDriver(DBBASE *pbase)
{
    const char *bldTop = "/jlab/clas/devel/R3.14.12.3/synApps/synApps_5_5/support/xxx-5-5";
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

    registerRecordTypes(pbase, 43, recordTypeNames, rtl);
    registerDevices(pbase, 179, deviceSupportNames, devsl);
    registerDrivers(pbase, 37, driverSupportNames, drvsl);
    (*pvar_func_asSub)();
    (*pvar_func_asynRegister)();
    (*pvar_func_asynInterposeFlushRegister)();
    (*pvar_func_asynInterposeEosRegister)();
    (*pvar_func_drvAsynSerialPortRegisterCommands)();
    (*pvar_func_drvAsynIPPortRegisterCommands)();
    (*pvar_func_drvAsynIPServerPortRegisterCommands)();
    (*pvar_func_vxi11RegisterCommands)();
    (*pvar_func_pvHistoryRegister)();
    (*pvar_func_drvScalerSoftRegister)();
    (*pvar_func_femtoRegistrar)();
    (*pvar_func_Scaler974Register)();
    (*pvar_func_calcRegister)();
    (*pvar_func_subAveRegister)();
    (*pvar_func_interpRegister)();
    (*pvar_func_arrayTestRegister)();
    (*pvar_func_saveDataRegistrar)();
    (*pvar_func_save_restoreRegister)();
    (*pvar_func_dbrestoreRegister)();
    (*pvar_func_asInitHooksRegister)();
    (*pvar_func_kohzuCtlRegistrar)();
    (*pvar_func_hrCtlRegistrar)();
    (*pvar_func_xiahscRegistrar)();
    (*pvar_func_xia_slitRegistrar)();
    (*pvar_func_orientRegistrar)();
    (*pvar_func_pf4Registrar)();
    (*pvar_func_IoRegistrar)();
    (*pvar_func_ml_monoCtlRegistrar)();
    (*pvar_func_fastSweepRegister)();
    (*pvar_func_Keithley2kDMMRegistrar)();
    (*pvar_func_Keithley65xxEMRegistrar)();
    (*pvar_func_motorUtilRegister)();
    (*pvar_func_motorRegister)();
    (*pvar_func_AcsRegister)();
    (*pvar_func_AerotechRegister)();
    (*pvar_func_ANC150Register)();
    (*pvar_func_MCDC2805Register)();
    (*pvar_func_IMSmotorRegister)();
    (*pvar_func_SC800Register)();
    (*pvar_func_MclennanRegister)();
    (*pvar_func_NewFocusRegister)();
    (*pvar_func_NewportRegister)();
    (*pvar_func_XPSGatheringRegister)();
    (*pvar_func_XPSRegister)();
    (*pvar_func_XPSInterposeRegister)();
    (*pvar_func_drvXPSAsynAuxRegister)();
    (*pvar_func_MM4005_trajectoryScanRegistrar)();
    (*pvar_func_XPS_trajectoryScanRegistrar)();
    (*pvar_func_OrielRegister)();
    (*pvar_func_ParkerRegister)();
    (*pvar_func_PIJEDSmotorRegister)();
    (*pvar_func_PImotorRegister)();
    (*pvar_func_PIC630Register)();
    (*pvar_func_PIC848motorRegister)();
    (*pvar_func_PIC662motorRegister)();
    (*pvar_func_PIC862motorRegister)();
    (*pvar_func_PIE710motorRegister)();
    (*pvar_func_PIE516motorRegister)();
    (*pvar_func_PIE816motorRegister)();
    (*pvar_func_ACSTech80Register)();
    (*pvar_func_SmartMotormotorRegister)();
    (*pvar_func_ThorLabsRegister)();
    (*pvar_func_motorSimRegister)();
    (*pvar_func_motorSimDriverRegister)();
    (*pvar_func_streamRegistrar)();
    (*pvar_func_simDetectorRegister)();
    (*pvar_func_NDStdArraysRegister)();
    (*pvar_func_NDFileNetCDFRegister)();
    (*pvar_func_NDFileTIFFRegister)();
    (*pvar_func_NDFileJPEGRegister)();
    (*pvar_func_NDFileNexusRegister)();
    (*pvar_func_NDROIRegister)();
    (*pvar_func_NDProcessRegister)();
    (*pvar_func_NDStatsRegister)();
    (*pvar_func_NDTransformRegister)();
    (*pvar_func_NDOverlayRegister)();
    (*pvar_func_NDColorConvertRegister)();
    iocshRegisterVariable(vardefs);
    return 0;
}

/* registerRecordDeviceDriver */
static const iocshArg registerRecordDeviceDriverArg0 =
                                            {"pdbbase",iocshArgPdbbase};
static const iocshArg *registerRecordDeviceDriverArgs[1] =
                                            {&registerRecordDeviceDriverArg0};
static const iocshFuncDef registerRecordDeviceDriverFuncDef =
                {"iocxxxLinux_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    iocxxxLinux_registerRecordDeviceDriver(*iocshPpdbbase);
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
