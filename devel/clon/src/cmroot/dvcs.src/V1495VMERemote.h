#ifndef V1495VMEREMOTE_H
#define V1495VMEREMOTE_H

#define DVCS_M_BASE					0x08510000
#define DVCS_Q1_BASE				0x08520000
#define DVCS_Q2_BASE				0x08530000
#define DVCS_Q3_BASE				0x08540000
#define DVCS_Q4_BASE				0x08550000

#define TRIGGER_M_BASE				0x08510000

#define BOARD_TYPE_DVCSQ		100
#define BOARD_TYPE_DVCSM		101
#define BOARD_TYPE_TRIGM		110

#define CLOCK_PERIOD_NS			5
#define PAGE_SIZE				264

#define VMESERVER_PORT			6002
#define MAX_MSG_SIZE			5000
#define BLK_MAX_LEN				2049

#define REMOTE_CMD_READ16		0x01
#define REMOTE_CMD_WRITE16		0x02
#define REMOTE_CMD_BLKREAD16	0x03
#define REMOTE_CMD_BLKWRITE16	0x04
#define REMOTE_CMD_WRITEPAGE	0x05

#define CMD_VERIFY_OK			1
#define CMD_VERIFY_BAD			0

#define CMD_RSP(cmd)			(cmd | 0x40)
#define CMD_ERR(cmd)			(cmd & 0x80)

#define REMOTE_MSG_SIZE(type)	(sizeof(RemoteMsgStruct)-MAX_MSG_SIZE+sizeof(type))

#ifdef WIN32
	#define HTONS(v)	(((v)>>8)|((v)<<8))
	#define NTOHS(v)	(((v)>>8)|((v)<<8))
	#define NTOHL(v)	(((v)>>24)|((v)<<24)|(((v)&0xFF00)<<8)|(((v)&0xFF0000)>>8))
	#define HTONL(v)	(((v)>>24)|((v)<<24)|(((v)&0xFF00)<<8)|(((v)&0xFF0000)>>8))
#else
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <inttypes.h>

	#define HTONS(v)	htons(v)
	#define NTOHS(v)	ntohs(v)
	#define NTOHL(v)	ntohl(v)
	#define HTONL(v)	htonl(v)
#endif

typedef struct
{
	unsigned int Length;
	unsigned int BoardType;
	unsigned int MsgType;
	unsigned char Msg[MAX_MSG_SIZE];
} RemoteMsgStruct;

/*****************************************************/
/*********** Some Board Generic Commands *************/
/*****************************************************/
typedef struct
{
	unsigned int Address;
} Cmd_Read16;

typedef struct
{
	unsigned short Value;
} Cmd_Read16_Rsp;

typedef struct
{
	unsigned int Address;
	unsigned short Value;
} Cmd_Write16;

typedef struct
{
	unsigned int Address;
	unsigned short Count;
} Cmd_BlkRead16;

typedef struct
{
	unsigned short Values[BLK_MAX_LEN];
} Cmd_BlkRead16_Rsp;

typedef struct
{
	unsigned int Address;
	unsigned int Count;
	unsigned int Verify;
	unsigned short Values[BLK_MAX_LEN];
} Cmd_BlkWrite16;

typedef struct
{
	unsigned char Verified;
} Cmd_BlkWrite16_Rsp;

typedef struct
{
	unsigned int Address;
	unsigned int Page;
	unsigned char Values[PAGE_SIZE];
} Cmd_WritePage;

typedef struct
{
	unsigned char Verified;
} Cmd_WritePage_Rsp;
/******************************************************
*******************************************************
******************************************************/

#endif
