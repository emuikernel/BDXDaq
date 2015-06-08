/* CoErrorDecodeImpl.cpp -- CoErrorDecode implementation */

/* Copyright (c) 2001 Wind River Systems, Inc. */

/*

modification history
--------------------
01b,13aug01,nel  Make method return HR value as a string instead of fixed
                 string.
01a,09aug01,nel  created

*/

#include "CoErrorDecodeImpl.h"		// Class Definition
#include "comObjLib.h"
#include "comObjLibExt.h"

/*

DESCRIPTION

This file implements the CoErrorDecode CoClass which has one interface
call IDecode. This contains methods for decoding an HRESULT into 
a string representation.

*/

AUTOREGISTER_COCLASS (CoErrorDecodeImpl, PS_DEFAULT, 0);

/* Structure to hold name/HRESULT pairs */
typedef struct _data
    {
    char *	name;
    HRESULT	value;
    } data;

/* Basic HRESULT values that are combined with severity/facility bits */
/* to create a full HRESULT value. */
data codeList [] = {
    {"RPC_S_INVALID_STRING_BINDING",     1700L},
    {"RPC_S_WRONG_KIND_OF_BINDING",      1701L},
    {"RPC_S_INVALID_BINDING",            1702L},
    {"RPC_S_PROTSEQ_NOT_SUPPORTED",      1703L},
    {"RPC_S_INVALID_RPC_PROTSEQ",        1704L},
    {"RPC_S_INVALID_STRING_UUID",        1705L},
    {"RPC_S_INVALID_ENDPOINT_FORMAT",    1706L},
    {"RPC_S_INVALID_NET_ADDR",           1707L},
    {"RPC_S_NO_ENDPOINT_FOUND",          1708L},
    {"RPC_S_INVALID_TIMEOUT",            1709L},
    {"RPC_S_OBJECT_NOT_FOUND",           1710L},
    {"RPC_S_ALREADY_REGISTERED",         1711L},
    {"RPC_S_TYPE_ALREADY_REGISTERED",    1712L},
    {"RPC_S_ALREADY_LISTENING",          1713L},
    {"RPC_S_NO_PROTSEQS_REGISTERED",     1714L},
    {"RPC_S_NOT_LISTENING",              1715L},
    {"RPC_S_UNKNOWN_MGR_TYPE",           1716L},
    {"RPC_S_UNKNOWN_IF",                 1717L},
    {"RPC_S_NO_BINDINGS",                1718L},
    {"RPC_S_NO_PROTSEQS",                1719L},
    {"RPC_S_CANT_CREATE_ENDPOINT",       1720L},
    {"RPC_S_OUT_OF_RESOURCES",           1721L},
    {"RPC_S_SERVER_UNAVAILABLE",         1722L},
    {"RPC_S_SERVER_TOO_BUSY",            1723L},
    {"RPC_S_INVALID_NETWORK_OPTIONS",    1724L},
    {"RPC_S_NO_CALL_ACTIVE",             1725L},
    {"RPC_S_CALL_FAILED",                1726L},
    {"RPC_S_CALL_FAILED_DNE",            1727L},
    {"RPC_S_PROTOCOL_ERROR",             1728L},
    {"RPC_S_UNSUPPORTED_TRANS_SYN",      1730L},
    {"RPC_S_UNSUPPORTED_TYPE",           1732L},
    {"RPC_S_INVALID_TAG",                1733L},
    {"RPC_S_INVALID_BOUND",              1734L},
    {"RPC_S_NO_ENTRY_NAME",              1735L},
    {"RPC_S_INVALID_NAME_SYNTAX",        1736L},
    {"RPC_S_UNSUPPORTED_NAME_SYNTAX",    1737L},
    {"RPC_S_UUID_NO_ADDRESS",            1739L},
    {"RPC_S_DUPLICATE_ENDPOINT",         1740L},
    {"RPC_S_UNKNOWN_AUTHN_TYPE",         1741L},
    {"RPC_S_MAX_CALLS_TOO_SMALL",        1742L},
    {"RPC_S_STRING_TOO_LONG",            1743L},
    {"RPC_S_PROTSEQ_NOT_FOUND",          1744L},
    {"RPC_S_PROCNUM_OUT_OF_RANGE",       1745L},
    {"RPC_S_BINDING_HAS_NO_AUTH",        1746L},
    {"RPC_S_UNKNOWN_AUTHN_SERVICE",      1747L},
    {"RPC_S_UNKNOWN_AUTHN_LEVEL",        1748L},
    {"RPC_S_INVALID_AUTH_IDENTITY",      1749L},
    {"RPC_S_UNKNOWN_AUTHZ_SERVICE",      1750L},
    {"EPT_S_INVALID_ENTRY",              1751L},
    {"EPT_S_CANT_PERFORM_OP",            1752L},
    {"EPT_S_NOT_REGISTERED",             1753L},
    {"RPC_S_NOTHING_TO_EXPORT",          1754L},
    {"RPC_S_INCOMPLETE_NAME",            1755L},
    {"RPC_S_INVALID_VERS_OPTION",        1756L},
    {"RPC_S_NO_MORE_MEMBERS",            1757L},
    {"RPC_S_NOT_ALL_OBJS_UNEXPORTED",    1758L},
    {"RPC_S_INTERFACE_NOT_FOUND",        1759L},
    {"RPC_S_ENTRY_ALREADY_EXISTS",       1760L},
    {"RPC_S_ENTRY_NOT_FOUND",            1761L},
    {"RPC_S_NAME_SERVICE_UNAVAILABLE",   1762L},
    {"RPC_S_INVALID_NAF_ID",             1763L},
    {"RPC_S_CANNOT_SUPPORT",             1764L},
    {"RPC_S_NO_CONTEXT_AVAILABLE",       1765L},
    {"RPC_S_INTERNAL_ERROR",             1766L},
    {"RPC_S_ZERO_DIVIDE",                1767L},
    {"RPC_S_ADDRESS_ERROR",              1768L},
    {"RPC_S_FP_DIV_ZERO",                1769L},
    {"RPC_S_FP_UNDERFLOW",               1770L},
    {"RPC_S_FP_OVERFLOW",                1771L},
    {"RPC_X_NO_MORE_ENTRIES",            1772L},
    {"RPC_X_SS_CHAR_TRANS_OPEN_FAIL",    1773L},
    {"RPC_X_SS_CHAR_TRANS_SHORT_FILE",   1774L},
    {"RPC_X_SS_IN_NULL_CONTEXT",         1775L},
    {"RPC_X_SS_CONTEXT_DAMAGED",         1777L},
    {"RPC_X_SS_HANDLES_MISMATCH",        1778L},
    {"RPC_X_SS_CANNOT_GET_CALL_HANDLE",  1779L},
    {"RPC_X_NULL_REF_POINTER",           1780L},
    {"RPC_X_ENUM_VALUE_OUT_OF_RANGE",    1781L},
    {"RPC_X_BYTE_COUNT_TOO_SMALL",       1782L},
    {"RPC_X_BAD_STUB_DATA",              1783L},
    {"ERROR_INVALID_USER_BUFFER",        1784L},
    {"ERROR_UNRECOGNIZED_MEDIA",         1785L},
    {"ERROR_NO_TRUST_LSA_SECRET",        1786L},
    {"ERROR_NO_TRUST_SAM_ACCOUNT",       1787L},
    {"ERROR_TRUSTED_DOMAIN_FAILURE",     1788L},
    {"ERROR_TRUSTED_RELATIONSHIP_FAILURE", 1789L},
    {"ERROR_TRUST_FAILURE",              1790L},
    {"RPC_S_CALL_IN_PROGRESS",           1791L},
    {"ERROR_NETLOGON_NOT_STARTED",       1792L},
    {"ERROR_ACCOUNT_EXPIRED",            1793L},
    {"ERROR_REDIRECTOR_HAS_OPEN_HANDLES", 1794L},
    {"ERROR_PRINTER_DRIVER_ALREADY_INSTALLED", 1795L},
    {"ERROR_UNKNOWN_PORT",               1796L},
    {"ERROR_UNKNOWN_PRINTER_DRIVER",     1797L},
    {"ERROR_UNKNOWN_PRINTPROCESSOR",     1798L},
    {"ERROR_INVALID_SEPARATOR_FILE",     1799L},
    {"ERROR_INVALID_PRIORITY",           1800L},
    {"ERROR_INVALID_PRINTER_NAME",       1801L},
    {"ERROR_PRINTER_ALREADY_EXISTS",     1802L},
    {"ERROR_INVALID_PRINTER_COMMAND",    1803L},
    {"ERROR_INVALID_DATATYPE",           1804L},
    {"ERROR_INVALID_ENVIRONMENT",        1805L},
    {"RPC_S_NO_MORE_BINDINGS",           1806L},
    {"ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT", 1807L},
    {"ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT", 1808L},
    {"ERROR_NOLOGON_SERVER_TRUST_ACCOUNT", 1809L},
    {"ERROR_DOMAIN_TRUST_INCONSISTENT",  1810L},
    {"ERROR_SERVER_HAS_OPEN_HANDLES",    1811L},
    {"ERROR_RESOURCE_DATA_NOT_FOUND",    1812L},
    {"ERROR_RESOURCE_TYPE_NOT_FOUND",    1813L},
    {"ERROR_RESOURCE_NAME_NOT_FOUND",    1814L},
    {"ERROR_RESOURCE_LANG_NOT_FOUND",    1815L},
    {"ERROR_NOT_ENOUGH_QUOTA",           1816L},
    {"RPC_S_NO_INTERFACES",              1817L},
    {"RPC_S_CALL_CANCELLED",             1818L},
    {"RPC_S_BINDING_INCOMPLETE",         1819L},
    {"RPC_S_COMM_FAILURE",               1820L},
    {"RPC_S_UNSUPPORTED_AUTHN_LEVEL",    1821L},
    {"RPC_S_NO_PRINC_NAME",              1822L},
    {"RPC_S_NOT_RPC_ERROR",              1823L},
    {"RPC_S_UUID_LOCAL_ONLY",            1824L},
    {"RPC_S_SEC_PKG_ERROR",              1825L},
    {"RPC_S_NOT_CANCELLED",              1826L},
    {"RPC_X_INVALID_ES_ACTION",          1827L},
    {"RPC_X_WRONG_ES_VERSION",           1828L},
    {"RPC_X_WRONG_STUB_VERSION",         1829L},
    {"RPC_X_INVALID_PIPE_OBJECT",        1830L},
    {"RPC_X_INVALID_PIPE_OPERATION",     1831L},
    {"RPC_X_WRONG_PIPE_VERSION",         1832L},
    {"RPC_S_GROUP_MEMBER_NOT_FOUND",     1898L},
    {"EPT_S_CANT_CREATE",                1899L},
    {"RPC_S_INVALID_OBJECT",             1900L},
    {"OR_INVALID_OXID",                  1910L},
    {"OR_INVALID_OID",                   1911L},
    {"OR_INVALID_SET",                   1912L},
    {NULL,                               0}
    };

/* Standard COM/DCOM HRESULT values */

data wellKnownList [] = 
    {
    {"S_OK",                             0x00000000L},
    {"S_FALSE",                          0x00000001L},
    {"E_UNEXPECTED",                     0x8000FFFFL},
    {"E_NOTIMPL",                        0x80004001L},
    {"E_OUTOFMEMORY",                    0x8007000EL},
    {"E_INVALIDARG",                     0x80070057L},
    {"E_NOINTERFACE",                    0x80004002L},
    {"E_POINTER",                        0x80004003L},
    {"E_HANDLE",                         0x80070006L},
    {"E_ABORT",                          0x80004004L},
    {"E_FAIL",                           0x80004005L},
    {"E_ACCESSDENIED",                   0x80070005L},
    {"E_PENDING",                        0x8000000AL},
    {"CO_E_INIT_TLS",                    0x80004006L},
    {"CO_E_INIT_SHARED_ALLOCATOR",       0x80004007L},
    {"CO_E_INIT_MEMORY_ALLOCATOR",       0x80004008L},
    {"CO_E_INIT_CLASS_CACHE",            0x80004009L},
    {"CO_E_INIT_RPC_CHANNEL",            0x8000400AL},
    {"CO_E_INIT_TLS_SET_CHANNEL_CONTROL", 0x8000400BL},
    {"CO_E_INIT_TLS_CHANNEL_CONTROL",    0x8000400CL},
    {"CO_E_INIT_UNACCEPTED_USER_ALLOCATOR", 0x8000400DL},
    {"CO_E_INIT_SCM_MUTEX_EXISTS",       0x8000400EL},
    {"CO_E_INIT_SCM_FILE_MAPPING_EXISTS", 0x8000400FL},
    {"CO_E_INIT_SCM_MAP_VIEW_OF_FILE",   0x80004010L},
    {"CO_E_INIT_SCM_EXEC_FAILURE",       0x80004011L},
    {"CO_E_INIT_ONLY_SINGLE_THREADED",   0x80004012L},
    {"CO_E_CANT_REMOTE",                 0x80004013L},
    {"CO_E_BAD_SERVER_NAME",             0x80004014L},
    {"CO_E_WRONG_SERVER_IDENTITY",       0x80004015L},
    {"CO_E_OLE1DDE_DISABLED",            0x80004016L},
    {"CO_E_RUNAS_SYNTAX",                0x80004017L},
    {"CO_E_CREATEPROCESS_FAILURE",       0x80004018L},
    {"CO_E_RUNAS_CREATEPROCESS_FAILURE", 0x80004019L},
    {"CO_E_RUNAS_LOGON_FAILURE",         0x8000401AL},
    {"CO_E_LAUNCH_PERMSSION_DENIED",     0x8000401BL},
    {"CO_E_START_SERVICE_FAILURE",       0x8000401CL},
    {"CO_E_REMOTE_COMMUNICATION_FAILURE", 0x8000401DL},
    {"CO_E_SERVER_START_TIMEOUT",        0x8000401EL},
    {"CO_E_CLSREG_INCONSISTENT",         0x8000401FL},
    {"CO_E_IIDREG_INCONSISTENT",         0x80004020L},
    {"CO_E_NOT_SUPPORTED",               0x80004021L},
    {"CLASS_E_NOAGGREGATION",            0x80040110L},
    {"CLASS_E_CLASSNOTAVAILABLE",        0x80040111L},
    {"REGDB_E_READREGDB",                0x80040150L},
    {"REGDB_E_WRITEREGDB",               0x80040151L},
    {"REGDB_E_KEYMISSING",               0x80040152L},
    {"REGDB_E_INVALIDVALUE",             0x80040153L},
    {"REGDB_E_CLASSNOTREG",              0x80040154L},
    {"REGDB_E_IIDNOTREG",                0x80040155L},
    {"CO_E_NOTINITIALIZED",              0x800401F0L},
    {"CO_E_ALREADYINITIALIZED",          0x800401F1L},
    {"CO_E_CANTDETERMINECLASS",          0x800401F2L},
    {"CO_E_CLASSSTRING",                 0x800401F3L},
    {"CO_E_IIDSTRING",                   0x800401F4L},
    {"CO_E_APPNOTFOUND",                 0x800401F5L},
    {"CO_E_APPSINGLEUSE",                0x800401F6L},
    {"CO_E_ERRORINAPP",                  0x800401F7L},
    {"CO_E_DLLNOTFOUND",                 0x800401F8L},
    {"CO_E_ERRORINDLL",                  0x800401F9L},
    {"CO_E_WRONGOSFORAPP",               0x800401FAL},
    {"CO_E_OBJNOTREG",                   0x800401FBL},
    {"CO_E_OBJISREG",                    0x800401FCL},
    {"CO_E_OBJNOTCONNECTED",             0x800401FDL},
    {"CO_E_APPDIDNTREG",                 0x800401FEL},
    {"CO_E_RELEASED",                    0x800401FFL},
    {"CO_E_CLASS_CREATE_FAILED",         0x80080001L},
    {"CO_E_SCM_ERROR",                   0x80080002L},
    {"CO_E_SCM_RPC_FAILURE",             0x80080003L},
    {"CO_E_BAD_PATH",                    0x80080004L},
    {"CO_E_SERVER_EXEC_FAILURE",         0x80080005L},
    {"CO_E_OBJSRV_RPC_FAILURE",          0x80080006L},
    {"CO_E_SERVER_STOPPING",             0x80080008L},
    {"CO_S_NOTALLINTERFACES",            0x00080012L},
    {"DISP_E_UNKNOWNINTERFACE",          0x80020001L},
    {"DISP_E_MEMBERNOTFOUND",            0x80020003L},
    {"DISP_E_PARAMNOTFOUND",             0x80020004L},
    {"DISP_E_TYPEMISMATCH",              0x80020005L},
    {"DISP_E_UNKNOWNNAME",               0x80020006L},
    {"DISP_E_NONAMEDARGS",               0x80020007L},
    {"DISP_E_BADVARTYPE",                0x80020008L},
    {"DISP_E_EXCEPTION",                 0x80020009L},
    {"DISP_E_OVERFLOW",                  0x8002000AL},
    {"DISP_E_BADINDEX",                  0x8002000BL},
    {"DISP_E_UNKNOWNLCID",               0x8002000CL},
    {"DISP_E_ARRAYISLOCKED",             0x8002000DL},
    {"DISP_E_BADPARAMCOUNT",             0x8002000EL},
    {"DISP_E_PARAMNOTOPTIONAL",          0x8002000FL},
    {"DISP_E_BADCALLEE",                 0x80020010L},
    {"DISP_E_NOTACOLLECTION",            0x80020011L},
    {"TYPE_E_BUFFERTOOSMALL",            0x80028016L},
    {"TYPE_E_INVDATAREAD",               0x80028018L},
    {"TYPE_E_UNSUPFORMAT",               0x80028019L},
    {"TYPE_E_REGISTRYACCESS",            0x8002801CL},
    {"TYPE_E_LIBNOTREGISTERED",          0x8002801DL},
    {"TYPE_E_UNDEFINEDTYPE",             0x80028027L},
    {"TYPE_E_QUALIFIEDNAMEDISALLOWED",   0x80028028L},
    {"TYPE_E_INVALIDSTATE",              0x80028029L},
    {"TYPE_E_WRONGTYPEKIND",             0x8002802AL},
    {"TYPE_E_ELEMENTNOTFOUND",           0x8002802BL},
    {"TYPE_E_AMBIGUOUSNAME",             0x8002802CL},
    {"TYPE_E_NAMECONFLICT",              0x8002802DL},
    {"TYPE_E_UNKNOWNLCID",               0x8002802EL},
    {"TYPE_E_DLLFUNCTIONNOTFOUND",       0x8002802FL},
    {"TYPE_E_BADMODULEKIND",             0x800288BDL},
    {"TYPE_E_SIZETOOBIG",                0x800288C5L},
    {"TYPE_E_DUPLICATEID",               0x800288C6L},
    {"TYPE_E_INVALIDID",                 0x800288CFL},
    {"TYPE_E_TYPEMISMATCH",              0x80028CA0L},
    {"TYPE_E_OUTOFBOUNDS",               0x80028CA1L},
    {"TYPE_E_IOERROR",                   0x80028CA2L},
    {"TYPE_E_CANTCREATETMPFILE",         0x80028CA3L},
    {"TYPE_E_CANTLOADLIBRARY",           0x80029C4AL},
    {"TYPE_E_INCONSISTENTPROPFUNCS",     0x80029C83L},
    {"TYPE_E_CIRCULARTYPE",              0x80029C84L},
    {"RPC_E_CALL_REJECTED",              0x80010001L},
    {"RPC_E_CALL_CANCELED",              0x80010002L},
    {"RPC_E_CANTPOST_INSENDCALL",        0x80010003L},
    {"RPC_E_CANTCALLOUT_INASYNCCALL",    0x80010004L},
    {"RPC_E_CANTCALLOUT_INEXTERNALCALL", 0x80010005L},
    {"RPC_E_CONNECTION_TERMINATED",      0x80010006L},
    {"RPC_E_SERVER_DIED",                0x80010007L},
    {"RPC_E_CLIENT_DIED",                0x80010008L},
    {"RPC_E_INVALID_DATAPACKET",         0x80010009L},
    {"RPC_E_CANTTRANSMIT_CALL",          0x8001000AL},
    {"RPC_E_CLIENT_CANTMARSHAL_DATA",    0x8001000BL},
    {"RPC_E_CLIENT_CANTUNMARSHAL_DATA",  0x8001000CL},
    {"RPC_E_SERVER_CANTMARSHAL_DATA",    0x8001000DL},
    {"RPC_E_SERVER_CANTUNMARSHAL_DATA",  0x8001000EL},
    {"RPC_E_INVALID_DATA",               0x8001000FL},
    {"RPC_E_INVALID_PARAMETER",          0x80010010L},
    {"RPC_E_CANTCALLOUT_AGAIN",          0x80010011L},
    {"RPC_E_SERVER_DIED_DNE",            0x80010012L},
    {"RPC_E_SYS_CALL_FAILED",            0x80010100L},
    {"RPC_E_OUT_OF_RESOURCES",           0x80010101L},
    {"RPC_E_ATTEMPTED_MULTITHREAD",      0x80010102L},
    {"RPC_E_NOT_REGISTERED",             0x80010103L},
    {"RPC_E_FAULT",                      0x80010104L},
    {"RPC_E_SERVERFAULT",                0x80010105L},
    {"RPC_E_CHANGED_MODE",               0x80010106L},
    {"RPC_E_INVALIDMETHOD",              0x80010107L},
    {"RPC_E_DISCONNECTED",               0x80010108L},
    {"RPC_E_RETRY",                      0x80010109L},
    {"RPC_E_SERVERCALL_RETRYLATER",      0x8001010AL},
    {"RPC_E_SERVERCALL_REJECTED",        0x8001010BL},
    {"RPC_E_INVALID_CALLDATA",           0x8001010CL},
    {"RPC_E_CANTCALLOUT_ININPUTSYNCCALL", 0x8001010DL},
    {"RPC_E_WRONG_THREAD",               0x8001010EL},
    {"RPC_E_THREAD_NOT_INIT",            0x8001010FL},
    {"RPC_E_VERSION_MISMATCH",           0x80010110L},
    {"RPC_E_INVALID_HEADER",             0x80010111L},
    {"RPC_E_INVALID_EXTENSION",          0x80010112L},
    {"RPC_E_INVALID_IPID",               0x80010113L},
    {"RPC_E_INVALID_OBJECT",             0x80010114L},
    {"RPC_S_CALLPENDING",                0x80010115L},
    {"RPC_S_WAITONTIMER",                0x80010116L},
    {"RPC_E_CALL_COMPLETE",              0x80010117L},
    {"RPC_E_UNSECURE_CALL",              0x80010118L},
    {"RPC_E_TOO_LATE",                   0x80010119L},
    {"RPC_E_NO_GOOD_SECURITY_PACKAGES",  0x8001011AL},
    {"RPC_E_ACCESS_DENIED",              0x8001011BL},
    {"RPC_E_REMOTE_DISABLED",            0x8001011CL},
    {"RPC_E_INVALID_OBJREF",             0x8001011DL},
    {"RPC_E_UNEXPECTED",                 0x8001FFFFL},
    {NULL,                               0}
    };

/**************************************************************************
*
* CoErrorDecodeImpl :: GetErrorString - converts an HRESULT into a BSTR.
*
* This method takes an HRESULT and converts it to a string representation.
*
* RETURNS: S_OK in all cases.
*/

STDMETHODIMP CoErrorDecodeImpl :: GetErrorString 
    (
    HRESULT		errorNum, 	/* HRESULT to convert */
    BSTR *		pStr		/* String representation */
    ) 
    { 
    /* search in well known list first */
    data *	ptr = wellKnownList;

    while (ptr->name)
        {
        if (ptr->value == errorNum)
            {
            VxComBSTR result (ptr->name);
            *pStr = result.Copy ();
            return S_OK;
            }
        ptr++;
        }

    /* search code list */
    ptr = codeList;

    while (ptr->name)
        {
        if (ptr->value == (errorNum & 0xffff))
            {
            VxComBSTR result (ptr->name);
            *pStr = result.Copy ();
            return S_OK;
            }
        ptr++;
        }

    /* return unknown error */
    VxComBSTR result;
    result.SetHex ((DWORD)errorNum);
    *pStr = result.Copy ();
    return S_OK;
    }
