/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 *
 * configdb.h - WindConfig generated configuration header
 *
 * DESCRIPTION
 *
 * This file contains the optional facilities selected by windcfg.
 * Use only WindConfig to modify this file.
 * Note: This file is included at the end of config.h.
*/

#ifndef INCconfigdbh
#define INCconfigdbh

#undef INCLUDE_ANSI_5_0
#undef INCLUDE_ANSI_ALL
#undef INCLUDE_ANSI_ASSERT
#undef INCLUDE_ANSI_CTYPE
#undef INCLUDE_ANSI_LOCALE
#undef INCLUDE_ANSI_STDIO
#undef INCLUDE_ANSI_STDLIB
#undef INCLUDE_ANSI_STRING
#undef INCLUDE_ANSI_TIME
#undef INCLUDE_BOOTP
#undef INCLUDE_CONFIGURATION_5_2
#undef INCLUDE_CONSTANT_RDY_Q

/*sergey: enable cplu for epics*/
#define INCLUDE_CPLUS
#define INCLUDE_CPLUS_DEMANGLER
#define INCLUDE_CPLUS_IOSTREAMS
#define INCLUDE_CPLUS_LANG
#define INCLUDE_CPLUS_STL
#define INCLUDE_CPLUS_STRING
#define INCLUDE_CPLUS_STRING_IO

#undef INCLUDE_CPLUS
#undef INCLUDE_CPLUS_BOOCH
#undef INCLUDE_CPLUS_IOSTREAMS
#undef INCLUDE_CPLUS_MIN
#undef INCLUDE_CPLUS_TOOLS
#undef INCLUDE_CPLUS_VXW

/* sergey: define following line to make epics happy */
#define INCLUDE_SNTPC
#undef  SNTP_PORT
#define SNTP_PORT 123

#undef INCLUDE_DEBUG
#undef INCLUDE_DEMO
#undef INCLUDE_DISK_UTIL
#undef INCLUDE_DOSFS
#undef INCLUDE_ENV_VARS
#undef INCLUDE_EXC_HANDLING
#undef INCLUDE_EXC_TASK
#undef INCLUDE_FLOATING_POINT
#undef INCLUDE_FORMATTED_IO
#undef INCLUDE_FTP_SERVER
#undef INCLUDE_IO_SYSTEM
#undef INCLUDE_LOADER
#undef INCLUDE_LOGGING
#undef INCLUDE_MEM_MGR_FULL
#undef INCLUDE_MIB2_ALL
#undef INCLUDE_MIB2_AT
#undef INCLUDE_MIB2_ICMP
#undef INCLUDE_MIB2_IF
#undef INCLUDE_MIB2_IP
#undef INCLUDE_MIB2_SYSTEM
#undef INCLUDE_MIB2_TCP
#undef INCLUDE_MIB2_UDP
#undef INCLUDE_MIB_CUSTOM
#undef INCLUDE_MIB_VXWORKS
#undef INCLUDE_MSG_Q
#undef INCLUDE_NETWORK
#undef INCLUDE_NET_INIT
#undef INCLUDE_NET_SHOW
#undef INCLUDE_NET_SYM_TBL

#define INCLUDE_NFS /*Sergey: define and add 3 following lines*/
#define NFS_GROUP_ID 100
#define NFS_USER_ID 2001
#define NFS_MAXPATH 255

#undef INCLUDE_NFS_MOUNT_ALL
#undef INCLUDE_NFS_SERVER
#undef INCLUDE_PING
#undef INCLUDE_PIPES
#undef INCLUDE_POSIX_AIO
#undef INCLUDE_POSIX_AIO_SYSDRV
#undef INCLUDE_POSIX_ALL
#undef INCLUDE_POSIX_FTRUNC
#undef INCLUDE_POSIX_MEM
#undef INCLUDE_POSIX_MQ
#undef INCLUDE_POSIX_SCHED
#undef INCLUDE_POSIX_SEM
#undef INCLUDE_POSIX_SIGNALS
#undef INCLUDE_POSIX_TIMERS
#undef INCLUDE_PPP
#undef INCLUDE_PROTECT_TEXT
#undef INCLUDE_PROTECT_VEC_TABLE
#undef INCLUDE_PROXY_CLIENT
#undef INCLUDE_PROXY_DEFAULT_ADDR
#undef INCLUDE_PROXY_SERVER
#undef INCLUDE_RAMDRV
#undef INCLUDE_RAWFS
#undef INCLUDE_RDB
#undef INCLUDE_RLOGIN
#undef INCLUDE_RPC
#undef INCLUDE_RT11FS
#undef INCLUDE_SECURITY
#undef INCLUDE_SELECT
#undef INCLUDE_SEM_BINARY
#undef INCLUDE_SEM_COUNTING
#undef INCLUDE_SEM_MUTEX
#undef INCLUDE_SHELL
#undef INCLUDE_SHOW_ROUTINES
#undef INCLUDE_SIGNALS
#undef INCLUDE_SLIP
#undef INCLUDE_SM_NET
#undef INCLUDE_SM_OBJ
#undef INCLUDE_SM_SEQ_ADDR
#undef INCLUDE_SNMPD
#undef INCLUDE_SNMPD_DEBUG
#undef INCLUDE_SPY
#undef INCLUDE_STANDALONE_SYM_TBL
#define INCLUDE_STARTUP_SCRIPT /*Sergey: define*/
#undef INCLUDE_STAT_SYM_TBL
#undef INCLUDE_STDIO
#undef INCLUDE_STREAMS
#undef INCLUDE_STREAMS_ALL
#undef INCLUDE_STREAMS_AUTOPUSH
#undef INCLUDE_STREAMS_DEBUG
#undef INCLUDE_STREAMS_DLPI
#undef INCLUDE_STREAMS_SOCKET
#undef INCLUDE_STREAMS_STRACE
#undef INCLUDE_STREAMS_STRERR
#undef INCLUDE_STREAMS_TLI
#undef INCLUDE_SYM_TBL
#undef INCLUDE_SYM_TBL_SYNC
#undef INCLUDE_TASK_HOOKS
#undef INCLUDE_TASK_VARS
#undef INCLUDE_TCP_DEBUG
#undef INCLUDE_TELNET
#undef INCLUDE_TFTP_CLIENT
#undef INCLUDE_TFTP_SERVER
#undef INCLUDE_TIMEX
#undef INCLUDE_TTY_DEV
#undef INCLUDE_UNLOADER
#undef INCLUDE_WATCHDOGS
#undef INCLUDE_WDB
#undef INCLUDE_WDB_BANNER
#undef INCLUDE_WDB_BP
#undef INCLUDE_WDB_CTXT
#undef INCLUDE_WDB_DIRECT_CALL
#undef INCLUDE_WDB_EVENTS
#undef INCLUDE_WDB_EXC_NOTIFY
#undef INCLUDE_WDB_EXIT_NOTIFY
#undef INCLUDE_WDB_FUNC_CALL
#undef INCLUDE_WDB_GOPHER
#undef INCLUDE_WDB_MEM
#undef INCLUDE_WDB_REG
#undef INCLUDE_WDB_TTY_TEST
#undef INCLUDE_WDB_VIO
#undef INCLUDE_INSTRUMENTATION
#undef INCLUDE_TIMESTAMP
#undef INCLUDE_ZBUF_SOCK
#define INCLUDE_ANSI_ALL
#define INCLUDE_ANSI_ASSERT
#define INCLUDE_ANSI_CTYPE
#define INCLUDE_ANSI_LOCALE
#define INCLUDE_ANSI_MATH
#define INCLUDE_ANSI_STDIO
#define INCLUDE_ANSI_STDLIB
#define INCLUDE_ANSI_STRING
#define INCLUDE_ANSI_TIME
#define INCLUDE_BSD_SOCKET
#define INCLUDE_CONSTANT_RDY_Q
#define INCLUDE_DISK_UTIL
#define INCLUDE_DOSFS
#define INCLUDE_ELF
#define INCLUDE_ENV_VARS
#define INCLUDE_EXC_HANDLING
#define INCLUDE_EXC_TASK
#define INCLUDE_FEI_IF
#define INCLUDE_FLOATING_POINT
#define INCLUDE_FORMATTED_IO
#define INCLUDE_GCC_FP
#define INCLUDE_HW_FP
#define INCLUDE_ICMP
#define INCLUDE_IGMP

#define INCLUDE_FTP_SERVER
#define INCLUDE_RAMDRV

#define INCLUDE_IO_SYSTEM
#define INCLUDE_LOADER
#define INCLUDE_LOGGING
#define INCLUDE_MEM_MGR_FULL
#define INCLUDE_MMU
#define INCLUDE_MSG_Q
#define INCLUDE_NET_INIT
#define INCLUDE_NET_REM_IO
#define INCLUDE_NET_SHOW
#define INCLUDE_NET_SYM_TBL
#define INCLUDE_NETWORK
#define INCLUDE_PCI
#define INCLUDE_PING
#define INCLUDE_PIPES
#define INCLUDE_PPC_FPU
#define INCLUDE_RLOGIN
#define INCLUDE_RPC
#define INCLUDE_SELECT
#define INCLUDE_SEM_BINARY
#define INCLUDE_SEM_COUNTING
#define INCLUDE_SEM_MUTEX
#define INCLUDE_SERIAL
#define INCLUDE_SHELL
#define INCLUDE_SHOW_ROUTINES
#define INCLUDE_SIGNALS
/* 
#define INCLUDE_SM_SEQ_ADDR
*/
#define INCLUDE_STAT_SYM_TBL
#define INCLUDE_STDIO
#define INCLUDE_SYM_TBL 
#define INCLUDE_SYM_TBL_SHOW 
#define INCLUDE_SYSCLK
#define INCLUDE_TASK_HOOKS
#define INCLUDE_TASK_VARS
#define INCLUDE_TCP
#define INCLUDE_TFTP_CLIENT
#define INCLUDE_TIMESTAMP
#define INCLUDE_TIMEX
#define INCLUDE_TTY_DEV
#define INCLUDE_UDP
#define INCLUDE_UNLOADER
#define INCLUDE_WATCHDOGS
#define INCLUDE_WDB
#define INCLUDE_WDB_BANNER
#define INCLUDE_WDB_BP
#define INCLUDE_WDB_CTXT
#define INCLUDE_WDB_DIRECT_CALL
#define INCLUDE_WDB_EVENTS
#define INCLUDE_WDB_EXC_NOTIFY
#define INCLUDE_WDB_EXIT_NOTIFY
#define INCLUDE_WDB_FUNC_CALL
#define INCLUDE_WDB_GOPHER
#define INCLUDE_WDB_MEM
#define INCLUDE_WDB_REG
#define INCLUDE_WDB_TTY_TEST
#define INCLUDE_WDB_VIO
#define INCLUDE_WINDVIEW

/*Sergey: add following*/
#undef  IP_FLAGS_DFLT
#define IP_FLAGS_DFLT (IP_DO_FORWARDING | IP_DO_REDIRECT | IP_DO_CHECKSUM_SND | IP_DO_CHECKSUM_RCV)
#undef  IP_TTL_DFLT
#define IP_TTL_DFLT 64
#undef  IP_QLEN_DFLT
#define IP_QLEN_DFLT 50
#undef  IP_FRAG_TTL_DFLT
#define IP_FRAG_TTL_DFLT 60
#undef  ICMP_FLAGS_DFLT
#define ICMP_FLAGS_DFLT (ICMP_NO_MASK_REPLY)
#undef  UDP_FLAGS_DFLT
#define UDP_FLAGS_DFLT (UDP_DO_CKSUM_SND | UDP_DO_CKSUM_RCV)
#undef  UDP_SND_SIZE_DFLT
#define UDP_SND_SIZE_DFLT 9216
#undef  UDP_RCV_SIZE_DFLT
#define UDP_RCV_SIZE_DFLT 41600
#undef  TCP_FLAGS_DFLT
#define TCP_FLAGS_DFLT (TCP_DO_RFC1323)
#undef  TCP_SND_SIZE_DFLT
#define TCP_SND_SIZE_DFLT 8192
#undef  TCP_RCV_SIZE_DFLT
#define TCP_RCV_SIZE_DFLT 8192
#undef  TCP_CON_TIMEO_DFLT
#define TCP_CON_TIMEO_DFLT 150
#undef  TCP_REXMT_THLD_DFLT
#define TCP_REXMT_THLD_DFLT 3
#undef  TCP_MSS_DFLT
#define TCP_MSS_DFLT 512
#undef  TCP_RND_TRIP_DFLT
#define TCP_RND_TRIP_DFLT 3
#undef  TCP_IDLE_TIMEO_DFLT
#define TCP_IDLE_TIMEO_DFLT 14400
#undef  TCP_MAX_PROBE_DFLT
#define TCP_MAX_PROBE_DFLT 8
#undef  TCP_RAND_FUNC
#define TCP_RAND_FUNC (FUNCPTR)random
#undef  TCP_MSL_CFG
#define TCP_MSL_CFG 30
#undef  NUM_FILES
#define NUM_FILES 50
#undef  FTP_TRANSIENT_MAX_RETRY_COUNT
#define FTP_TRANSIENT_MAX_RETRY_COUNT 100
#undef  FTP_TRANSIENT_RETRY_INTERVAL
#define FTP_TRANSIENT_RETRY_INTERVAL 0
#undef  FTP_TRANSIENT_FATAL
#define FTP_TRANSIENT_FATAL ftpTransientFatal
#undef  FTP_DEBUG_OPTIONS
#define FTP_DEBUG_OPTIONS 0
#undef  NFS_USER_ID
#define NFS_USER_ID 2001
#undef  NFS_GROUP_ID
#define NFS_GROUP_ID 100
#undef  NFS_MAXPATH
#define NFS_MAXPATH 255
#undef  TELNETD_MAX_CLIENTS
#define TELNETD_MAX_CLIENTS 1
#undef  TELNETD_TASKFLAG
#define TELNETD_TASKFLAG FALSE
#undef  TELNETD_PORT
#define TELNETD_PORT 23
#undef  TELNETD_PARSER_HOOK
#define TELNETD_PARSER_HOOK shellParserControl
#undef  LOGIN_USER_NAME
#define LOGIN_USER_NAME "da"
#undef  LOGIN_PASSWORD
#define LOGIN_PASSWORD "bReb99RRed"
#undef  NUM_NET_MBLKS
#define NUM_NET_MBLKS (2 * NUM_CL_BLKS)
#undef  NUM_CL_BLKS
#define NUM_CL_BLKS (NUM_64 + NUM_128 + NUM_256 + NUM_512 + NUM_1024 + NUM_2048)
#undef  NUM_64
#define NUM_64 500
#undef  NUM_128
#define NUM_128 500
#undef  NUM_256
#define NUM_256 500
#undef  NUM_512
#define NUM_512 500
#undef  NUM_1024
#define NUM_1024 500
#undef  NUM_2048
#define NUM_2048 500
#undef  NUM_SYS_MBLKS
#define NUM_SYS_MBLKS (2 * NUM_SYS_CL_BLKS)
#undef  NUM_SYS_CL_BLKS
#define NUM_SYS_CL_BLKS (NUM_SYS_64  + NUM_SYS_128 + NUM_SYS_256 + NUM_SYS_512)
#undef  NUM_SYS_64
#define NUM_SYS_64 512
#undef  NUM_SYS_128
#define NUM_SYS_128 512
#undef  NUM_SYS_256
#define NUM_SYS_256 512
#undef  NUM_SYS_512
#define NUM_SYS_512 512
#undef  IP_MAX_UNITS
#define IP_MAX_UNITS 1
/*
#undef  ARP_MAX_ENTRIES
#define ARP_MAX_ENTRIES 0
#undef  CLEAR_BSS
#define CLEAR_BSS
#undef  MQ_HASH_SIZE
#define MQ_HASH_SIZE 0
#undef  SHELL_STACK_SIZE
#define SHELL_STACK_SIZE (20000)
#undef  SYM_TBL_HASH_SIZE_LOG2
#define SYM_TBL_HASH_SIZE_LOG2 8
#undef  STAT_TBL_HASH_SIZE_LOG2
#define STAT_TBL_HASH_SIZE_LOG2 6
#undef  WDB_STACK_SIZE
#define WDB_STACK_SIZE 0x2000
#undef  WDB_BP_MAX
#define WDB_BP_MAX 50
#undef  WDB_SPAWN_PRI
#define WDB_SPAWN_PRI 100
#undef  WDB_SPAWN_OPTS
#define WDB_SPAWN_OPTS VX_FP_TASK | VX_ALTIVEC_TASK
#undef  WDB_TASK_PRIORITY
#define WDB_TASK_PRIORITY 3
#undef  WDB_TASK_OPTIONS
#define WDB_TASK_OPTIONS VX_UNBREAKABLE | VX_FP_TASK | VX_ALTIVEC_TASK
#undef  WDB_RESTART_TIME
#define WDB_RESTART_TIME 10
#undef  WDB_MAX_RESTARTS
#define WDB_MAX_RESTARTS 5
#undef  WDB_MTU
#define WDB_MTU 1500
#undef  INCLUDE_CONSTANT_RDY_Q
#define INCLUDE_CONSTANT_RDY_Q
#undef  ROOT_STACK_SIZE
#define ROOT_STACK_SIZE (24000)
#undef  ISR_STACK_SIZE
#define ISR_STACK_SIZE (5000)
#undef  INT_LOCK_LEVEL
#define INT_LOCK_LEVEL 0x0
#undef  ENV_VAR_USE_HOOKS
#define ENV_VAR_USE_HOOKS TRUE
#undef  NUM_DRIVERS
#define NUM_DRIVERS 20
#undef  MAX_LOG_MSGS
#define MAX_LOG_MSGS 50
#undef  MUX_MAX_BINDS
#define MUX_MAX_BINDS 16
#undef  USR_MAX_LINK_HDR
#define USR_MAX_LINK_HDR 16
#undef  SOMAXCONN_CFG
#define SOMAXCONN_CFG 5
#undef  EXC_EXTENDED_VECTORS_ENABLED
#define EXC_EXTENDED_VECTORS_ENABLED TRUE
#undef  USER_APPL_INIT
#define USER_APPL_INIT "sysApplInit()"
#undef  MV64360_AUXCLK_TMR
#define MV64360_AUXCLK_TMR 0
*/

#endif /* INCconfigdbh */
