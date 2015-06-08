/* wtxerr.h - wtx error header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02i,11sep01,fle  added some new registry error numbers
02h,22jun98,c_c  asynchronous loader implementation.
02g,02apr98,pcn  WTX 2: added new error codes for C API.
02f,20mar98,fle  got rid of the WTX_ERR_REGISTRY_PING_INFO_ERROR error value
02e,17mar98,pcn  WTX 2: added WTX_ERR_SVR_BAD_TARGET_CHECKSUM and
                 WTX_ERR_SVR_DIFFERENT_CHECKSUM.
02d,10mar98,dbt  added WTX_ERR_AGENT_HW_REGS_EXHAUSTED and 
		 WTX_ERR_AGENT_INVALID_HW_BP error numbers.
02c,03mar98,fle  added WTX_ERR_SVR_DOESNT_RESPOND error number
02b,02mar98,pcn  WTX 2: added WTX_ERR_SVR_BKEND_LOG_IN_USE,
                 WTX_ERR_SVR_WTX_LOG_IN_USE.
02a,03feb98,fle  changed WTX_ERR_REGISTRY_SVR_IS_DEAD code value
01z,23jan98,fle  added WTX_ERR_REGISTRY_SVR_IS_DEAD error message
01y,22jan98,c_c  Added WTX_ERR_LOADER_NO_VERIFICATION_ROUTINE error.
01x,24nov97,fle  added WTX_ERR_REGISTRY_DB_FILE_CORRUPTED
                 WTX_ERR_REGISTRY_DIR_NOT_FOUND and
                 WTX_ERR_REGISTRY_FILE_NOT_FOUND error codes
01w,03mar97,wmd  added comment to alert that corresponding changes must also
		 be made in resource/tcl/wtxerrdb.tcl.
01v,28feb97,wmd  added WTX_ERR_SVR_INVALID_LICENSE.
01u,30sep96,elp  put in share/src/wtx (SPR# 6775).
01t,11sep96,pad  added WTX_ERR_LOADER_MAX_MODULES_LOADED (SPR #7133).
01s,15may96,pad  added WTX_ERR_LOADER_SYMBOL_NOT_FROM_SDA_SECTION.
01r,26mar96,pad  added WTX_ERR_LOADER_RELOCATION_OFFSET_TOO_LARGE.
01q,17jan96,p_m  added WTX_ERR_SYMTBL_SYMBOL_NAME_TOO_LONG.
01p,16oct95,pad  added WTX_ERR_SVR_CANT_CONNECT_LICENSE_DAEMON.
01o,31aug95,pad  added WTX_ERR_LOADER_OBJ_MODULE_NOT_APPROPRIATE (SPR #4533).
		 Fixed WTX_ERR_AGENT_MEM_ACCESS_ERROR.
01n,12jul95,pad  added WTX_ERR_LOADER_CANT_ATTACH_OMF_MANAGER.
01m,11jun95,p_m  added WTX_ERR_SVR_TARGET_UNKNOWN and 
		 WTX_ERR_SVR_INVALID_DEVICE.
01l,09jun95,p_m  added WTX_ERR_AGENT_PATTERN_NOT_FOUND.
01k,01jun95,c_s  added AGENT_GOPHER_SYNTAX error.
01j,22may95,jcf  name revision.
01i,19may95,p_m  added WTX_ERR_NO_VIO_CHANNEL
          + tpr  changed WTX_ERR_AGENT_XXX name following new WDB error code.
01h,17may95,s_w  changed exchange and C API errors
01g,11may95,pas  added WTX_ERR_SYMTBL_NO_SUCH_MODULE
01f,04may95,s_w  added errors for C API and exchange errors and WTX_ERR_LAST
01e,29mar95,p_m  added agent errors.
01d,01mar95,pad  added error code for OBJ_XXX stuff.
01c,23feb95,p_m  made all _TGT_SVR_ errors be _SVR_.  leave unused numbers
		 between error classes.
		 changed _WPWRD_ errors to _REGISTRY_.
01b,21feb95,pad  added error codes for DLL and WTX service addition.
01a,15feb95,p_m  written.
*/

#ifndef __INCwtxerrh
#define __INCwtxerrh 1

#define WTX_ERR_NONE                  0

/************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 NOTE:  under UNIX execute the following procedure:

        first check out the ../../resource/tcl/wtxerrdb.tcl file
        go to wpwr/host/src/wtxtcl directory
        then type :
            wtxtcl mkErrDb.tcl ../../include/wtxerr.h > \
                               ../../resource/tcl/wtxerrdb.tcl

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
 
/* wtx status codes */

#define WTXERR_BASE_NUM		0x10000		/* XXX P_M for now */

typedef enum wtx_error_t
    {
    /* Target Server Errors */

    WTX_ERR_SVR_TARGET_NOT_ATTACHED           = (WTXERR_BASE_NUM | 1),
    WTX_ERR_SVR_INVALID_EVENTPOINT            = (WTXERR_BASE_NUM | 2),
    WTX_ERR_SVR_FILE_NOT_FOUND                = (WTXERR_BASE_NUM | 3),
    WTX_ERR_SVR_INVALID_FIND_REQUEST          = (WTXERR_BASE_NUM | 4),
    WTX_ERR_SVR_INVALID_FLAGS                 = (WTXERR_BASE_NUM | 5),
    WTX_ERR_SVR_INVALID_FILE_DESCRIPTOR       = (WTXERR_BASE_NUM | 6),
    WTX_ERR_SVR_EINVAL                        = (WTXERR_BASE_NUM | 7),
    WTX_ERR_SVR_CANT_START_CONSOLE            = (WTXERR_BASE_NUM | 8),
    WTX_ERR_SVR_INVALID_CONSOLE               = (WTXERR_BASE_NUM | 9),
    WTX_ERR_SVR_NO_SCALABLE_AGENT             = (WTXERR_BASE_NUM | 10),
    WTX_ERR_SVR_CANT_ADD_WTX_SERVICE	      = (WTXERR_BASE_NUM | 11),
    WTX_ERR_SVR_NO_VIO_CHANNEL                = (WTXERR_BASE_NUM | 12),
    WTX_ERR_SVR_BKEND_LOG_IN_USE              = (WTXERR_BASE_NUM | 13),
    WTX_ERR_SVR_WTX_LOG_IN_USE                = (WTXERR_BASE_NUM | 14),
    WTX_ERR_SVR_DOESNT_RESPOND                = (WTXERR_BASE_NUM | 15),
    WTX_ERR_SVR_IS_DEAD                       = (WTXERR_BASE_NUM | 16),
    WTX_ERR_SVR_BAD_TARGET_CHECKSUM           = (WTXERR_BASE_NUM | 17),
    WTX_ERR_SVR_CHECKSUM_MISMATCH             = (WTXERR_BASE_NUM | 18),

    /* Common Server Errors */

    WTX_ERR_SVR_NOT_ENOUGH_MEMORY             = (WTXERR_BASE_NUM | 20),
    WTX_ERR_SVR_ACCESS_DENIED                 = (WTXERR_BASE_NUM | 21),
    WTX_ERR_SVR_WIND_BASE_NOT_SET             = (WTXERR_BASE_NUM | 22),
    WTX_ERR_SVR_SERVICE_NOT_AVAILABLE         = (WTXERR_BASE_NUM | 23),
    WTX_ERR_SVR_INVALID_CLIENT_ID             = (WTXERR_BASE_NUM | 24),
    WTX_ERR_SVR_CLIENT_NOT_ATTACHED           = (WTXERR_BASE_NUM | 25),
    WTX_ERR_SVR_TARGET_UNKNOWN                = (WTXERR_BASE_NUM | 26),
    WTX_ERR_SVR_INVALID_DEVICE                = (WTXERR_BASE_NUM | 27),
    WTX_ERR_SVR_CANT_CONNECT_LICENSE_DAEMON   = (WTXERR_BASE_NUM | 28),
    WTX_ERR_SVR_INVALID_LICENSE               = (WTXERR_BASE_NUM | 29),

    /* Registry Errors */

    WTX_ERR_REGISTRY_NAME_NOT_FOUND           = (WTXERR_BASE_NUM | 30),
    WTX_ERR_REGISTRY_BAD_PATTERN              = (WTXERR_BASE_NUM | 31),
    WTX_ERR_REGISTRY_NAME_CLASH               = (WTXERR_BASE_NUM | 32),
    WTX_ERR_REGISTRY_DB_DIR_NOT_FOUND         = (WTXERR_BASE_NUM | 33),
    WTX_ERR_REGISTRY_DB_FILE_ERROR            = (WTXERR_BASE_NUM | 34),
    WTX_ERR_REGISTRY_MEMALLOC                 = (WTXERR_BASE_NUM | 35),

    /* Object Module Management Errors */

    WTX_ERR_LOADER_TEXT_IN_USE                = (WTXERR_BASE_NUM | 40),
    WTX_ERR_LOADER_ILLEGAL_FLAGS_COMBINATION  = (WTXERR_BASE_NUM | 41),
    WTX_ERR_LOADER_NO_RELOCATION_ROUTINE      = (WTXERR_BASE_NUM | 42),
    WTX_ERR_LOADER_CANT_GET_SEGMENT_ADDRESSES = (WTXERR_BASE_NUM | 43),
    WTX_ERR_LOADER_UNKNOWN_RELOCATION_ENTRY   = (WTXERR_BASE_NUM | 44),
    WTX_ERR_LOADER_UNKNOWN_COMMAND_FLAG       = (WTXERR_BASE_NUM | 45),
    WTX_ERR_LOADER_UNKNOWN_OBJ_MODULE_FORMAT  = (WTXERR_BASE_NUM | 46),
    WTX_ERR_LOADER_NULL_SIZE_OBJ_MODULE       = (WTXERR_BASE_NUM | 47),
    WTX_ERR_LOADER_OBJ_MODULE_NOT_FOUND       = (WTXERR_BASE_NUM | 48),
    WTX_ERR_LOADER_BAD_CHECKSUM               = (WTXERR_BASE_NUM | 49),
    WTX_ERR_LOADER_CANT_ATTACH_OMF_MANAGER    = (WTXERR_BASE_NUM | 50),
    WTX_ERR_LOADER_OBJ_MODULE_NOT_APPROPRIATE = (WTXERR_BASE_NUM | 51),
    WTX_ERR_LOADER_RELOCATION_OFFSET_TOO_LARGE= (WTXERR_BASE_NUM | 52),
    WTX_ERR_LOADER_SYMBOL_NOT_FROM_SDA_SECTION= (WTXERR_BASE_NUM | 53),
    WTX_ERR_LOADER_MAX_MODULES_LOADED	      = (WTXERR_BASE_NUM | 54),
    WTX_ERR_LOADER_RELOCATION_ERROR           = (WTXERR_BASE_NUM | 55),
    WTX_ERR_LOADER_NO_VERIFICATION_ROUTINE    = (WTXERR_BASE_NUM | 56),
    WTX_ERR_LOADER_LOAD_CANCELED              = (WTXERR_BASE_NUM | 57),
    WTX_ERR_LOADER_LOAD_IN_PROGRESS           = (WTXERR_BASE_NUM | 58),
    WTX_ERR_LOADER_ALREADY_LOADED             = (WTXERR_BASE_NUM | 59),

    /* Symbol Management Errors */

    WTX_ERR_SYMTBL_INVALID_SYMTBL             = (WTXERR_BASE_NUM | 60),
    WTX_ERR_SYMTBL_SYMBOL_NOT_FOUND           = (WTXERR_BASE_NUM | 61),
    WTX_ERR_SYMTBL_TABLE_NOT_EMPTY            = (WTXERR_BASE_NUM | 62),
    WTX_ERR_SYMTBL_NAME_CLASH                 = (WTXERR_BASE_NUM | 63),
    WTX_ERR_SYMTBL_NO_SUCH_MODULE             = (WTXERR_BASE_NUM | 64),
    WTX_ERR_SYMTBL_SYMBOL_NAME_TOO_LONG       = (WTXERR_BASE_NUM | 65),

    /* Target Memory Management Errors */

    WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY          = (WTXERR_BASE_NUM | 70),
    WTX_ERR_TGTMEM_INVALID_POOL_SIZE          = (WTXERR_BASE_NUM | 71),
    WTX_ERR_TGTMEM_INVALID_BLOCK              = (WTXERR_BASE_NUM | 72),
    WTX_ERR_TGTMEM_PAGE_SIZE_UNAVAILABLE      = (WTXERR_BASE_NUM | 73),
    WTX_ERR_TGTMEM_BLOCK_CONFLICT             = (WTXERR_BASE_NUM | 74),
    WTX_ERR_TGTMEM_CANT_UNMAP_UNKNOWN_BLOCK   = (WTXERR_BASE_NUM | 75),
    WTX_ERR_TGTMEM_UNMAPPED_MEMORY            = (WTXERR_BASE_NUM | 76),
    
    /* generic DLL error codes */

    WTX_ERR_DLL_CANT_OPEN_SHARED_FILE         = (WTXERR_BASE_NUM | 100),
    WTX_ERR_DLL_CANT_BIND_FUNCTION            = (WTXERR_BASE_NUM | 101),
    WTX_ERR_DLL_CANT_CLOSE_SHARED_FILE        = (WTXERR_BASE_NUM | 102),

    /* Internal object error codes */

    WTX_ERR_OBJ_INVALID_OBJECT                = (WTXERR_BASE_NUM | 110),

    /* Agent error codes */

    WTX_ERR_AGENT_UNSUPPORTED_REQUEST         = (WTXERR_BASE_NUM | 200),
    WTX_ERR_AGENT_INVALID_PARAMS              = (WTXERR_BASE_NUM | 201),
    WTX_ERR_AGENT_MEM_ACCESS_ERROR            = (WTXERR_BASE_NUM | 202),
    WTX_ERR_AGENT_AGENT_MODE_ERROR            = (WTXERR_BASE_NUM | 203),
    WTX_ERR_AGENT_RT_ERROR                    = (WTXERR_BASE_NUM | 204),
    WTX_ERR_AGENT_INVALID_CONTEXT             = (WTXERR_BASE_NUM | 205),
    WTX_ERR_AGENT_INVALID_VIO_CHANNEL         = (WTXERR_BASE_NUM | 206),
    WTX_ERR_AGENT_INVALID_EVENT               = (WTXERR_BASE_NUM | 207),
    WTX_ERR_AGENT_INVALID_EVENTPOINT          = (WTXERR_BASE_NUM | 208),
    WTX_ERR_AGENT_GOPHER_FAULT                = (WTXERR_BASE_NUM | 209),
    WTX_ERR_AGENT_GOPHER_TRUNCATED            = (WTXERR_BASE_NUM | 210),
	
    WTX_ERR_AGENT_EVENTPOINT_TABLE_FULL       = (WTXERR_BASE_NUM | 211),
    WTX_ERR_AGENT_NO_AGENT_PROC               = (WTXERR_BASE_NUM | 212),
    WTX_ERR_AGENT_NO_RT_PROC                  = (WTXERR_BASE_NUM | 213),
    WTX_ERR_AGENT_PROC_FAILED                 = (WTXERR_BASE_NUM | 214),
    WTX_ERR_AGENT_PATTERN_NOT_FOUND           = (WTXERR_BASE_NUM | 215),
    WTX_ERR_AGENT_HW_BP_REGS_EXHAUSTED	      = (WTXERR_BASE_NUM | 216),
    WTX_ERR_AGENT_INVALID_HW_BP		      = (WTXERR_BASE_NUM | 217),

    WTX_ERR_AGENT_NO_CONNECTION               = (WTXERR_BASE_NUM | 220),
    WTX_ERR_AGENT_CONNECTION_BUSY             = (WTXERR_BASE_NUM | 221),
    WTX_ERR_AGENT_COMMUNICATION_ERROR         = (WTXERR_BASE_NUM | 222),
    WTX_ERR_AGENT_GOPHER_SYNTAX	              = (WTXERR_BASE_NUM | 223),

    /* WTX C API error codes */

    WTX_ERR_API				      = (WTXERR_BASE_NUM | 300),
    WTX_ERR_API_MEMALLOC		      = (WTXERR_BASE_NUM | 301),
    WTX_ERR_API_HANDLER_NOT_FOUND	      = (WTXERR_BASE_NUM | 302),
    WTX_ERR_API_NOT_CONNECTED		      = (WTXERR_BASE_NUM | 303),
    WTX_ERR_API_INVALID_ARG		      = (WTXERR_BASE_NUM | 304),
    WTX_ERR_API_SERVER_NOT_FOUND	      = (WTXERR_BASE_NUM | 305),
    WTX_ERR_API_ALREADY_CONNECTED	      = (WTXERR_BASE_NUM | 306),
    WTX_ERR_API_AMBIGUOUS_SERVER_NAME	      = (WTXERR_BASE_NUM | 307),
    WTX_ERR_API_REGS_GET_PARTIAL_READ	      = (WTXERR_BASE_NUM | 308),
    WTX_ERR_API_INVALID_HANDLE		      = (WTXERR_BASE_NUM | 309),
    WTX_ERR_API_REGISTRY_UNREACHABLE	      = (WTXERR_BASE_NUM | 310),
    WTX_ERR_API_SERVICE_NOT_REGISTERED	      = (WTXERR_BASE_NUM | 311),
    WTX_ERR_API_SERVICE_ALREADY_REGISTERED    = (WTXERR_BASE_NUM | 312),
    WTX_ERR_API_TOOL_DISCONNECTED	      = (WTXERR_BASE_NUM | 313),
    WTX_ERR_API_REQUEST_FAILED		      = (WTXERR_BASE_NUM | 314),
    WTX_ERR_API_REQUEST_TIMED_OUT	      = (WTXERR_BASE_NUM | 315),	
    WTX_ERR_API_FILE_NOT_FOUND		      = (WTXERR_BASE_NUM | 316),
    WTX_ERR_API_SERVICE_ALREADY_STARTED       = (WTXERR_BASE_NUM | 317),
    WTX_ERR_API_CANT_OPEN_SOCKET              = (WTXERR_BASE_NUM | 318),
    WTX_ERR_API_CANT_GET_HOSTNAME             = (WTXERR_BASE_NUM | 319),
    WTX_ERR_API_FILE_NOT_ACCESSIBLE           = (WTXERR_BASE_NUM | 320),
    WTX_ERR_API_NULL_SIZE_FILE                = (WTXERR_BASE_NUM | 321),
    WTX_ERR_API_CANT_READ_FROM_FILE           = (WTXERR_BASE_NUM | 322),

    /* WTX Exchange Error Codes */

    WTX_ERR_EXCHANGE                          = (WTXERR_BASE_NUM | 400),  
    WTX_ERR_EXCHANGE_INVALID_HANDLE           = (WTXERR_BASE_NUM | 401),  
    WTX_ERR_EXCHANGE_DATA                     = (WTXERR_BASE_NUM | 402),
    WTX_ERR_EXCHANGE_MEMALLOC		      = (WTXERR_BASE_NUM | 403), 
    WTX_ERR_EXCHANGE_NO_SERVER		      = (WTXERR_BASE_NUM | 404),
    WTX_ERR_EXCHANGE_INVALID_ARG	      = (WTXERR_BASE_NUM | 405),
    WTX_ERR_EXCHANGE_MARSHALPTR	              = (WTXERR_BASE_NUM | 406), 
    WTX_ERR_EXCHANGE_TIMEOUT                  = (WTXERR_BASE_NUM | 407),
    WTX_ERR_EXCHANGE_BAD_KEY		      = (WTXERR_BASE_NUM | 408),
    WTX_ERR_EXCHANGE_REQUEST_UNSUPPORTED      = (WTXERR_BASE_NUM | 409),
    WTX_ERR_EXCHANGE_TRANSPORT_UNSUPPORTED    = (WTXERR_BASE_NUM | 410),
    WTX_ERR_EXCHANGE_TRANSPORT_ERROR	      = (WTXERR_BASE_NUM | 411),
    WTX_ERR_EXCHANGE_TRANSPORT_DISCONNECT     = (WTXERR_BASE_NUM | 412),
    WTX_ERR_EXCHANGE_NO_TRANSPORT	      = (WTXERR_BASE_NUM | 413),

    WTX_ERR_LAST			      = (WTXERR_BASE_NUM | 0xFFFF)

    } WTX_ERROR_T;

#endif
