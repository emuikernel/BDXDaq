#ifndef VMERemote_H
#define VMERemote_H

#include "RootHeader.h"
#include "V1495VMERemote.h"
#include "DVCSBoardRegs.h"

class VMERemote		: public TSocket
{
public:
	VMERemote(const char *pHost, int port)	: TSocket(pHost, port, 32768)
	{
		pHostName = pHost;
		if(!IsValid())
		{
			printf("Failed to connected to host: %s\n", pHost);
			gApplication->Terminate();
		}
		else
			printf("Successfully connected to host: %s\n" , pHost);
	}

	Bool_t WriteVME16(unsigned int addr, unsigned short val)
	{
		if(!IsValid())
		{
			printf("WriteVME16 FAILED\n");
			return kFALSE;
		}

		Cmd_Write16 *pCmd_Write16;
		OutgoingMsg.Length = HTONL(REMOTE_MSG_SIZE(Cmd_Write16)-4);
		OutgoingMsg.BoardType = HTONL(BOARD_TYPE_DVCSQ);
		OutgoingMsg.MsgType = HTONL(REMOTE_CMD_WRITE16);
		pCmd_Write16 = (Cmd_Write16 *)OutgoingMsg.Msg;
		pCmd_Write16->Address = HTONL(addr);
		pCmd_Write16->Value = HTONS(val);
		SendRaw(&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_Write16));

		return kTRUE;
	}

	Bool_t ReadVME16(unsigned int addr, unsigned short *val)
	{
		if(!IsValid())
		{
			printf("ReadVME16 FAILED\n");
			return kFALSE;
		}

		Cmd_Read16 *pCmd_Read16;
		OutgoingMsg.Length = HTONL(REMOTE_MSG_SIZE(Cmd_Read16)-4);
		OutgoingMsg.BoardType = HTONL(BOARD_TYPE_DVCSQ);
		OutgoingMsg.MsgType = HTONL(REMOTE_CMD_READ16);
		pCmd_Read16 = (Cmd_Read16 *)OutgoingMsg.Msg;
		pCmd_Read16->Address = HTONL(addr);
		SendRaw(&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_Read16));

		if( (RecvRaw(&IncomingMsg.Length, 4) == 4) &&
			(NTOHL(IncomingMsg.Length) <= sizeof(RemoteMsgStruct)) &&
			(RecvRaw(&IncomingMsg.BoardType, NTOHL(IncomingMsg.Length)) == NTOHL(IncomingMsg.Length)) )
		{
			IncomingMsg.Length = NTOHL(IncomingMsg.Length);
			IncomingMsg.BoardType = NTOHL(IncomingMsg.BoardType);
			IncomingMsg.MsgType = NTOHL(IncomingMsg.MsgType);
			if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_READ16))
			{
				unsigned short *p = (unsigned short *)IncomingMsg.Msg;
				*val = NTOHS(*p);
				return kTRUE;
			}
		}
		Close();
		return kFALSE;
	}

	Bool_t BlkReadVME16(unsigned int addr, unsigned short *vals, int count)
	{
		if(!IsValid())
		{
			printf("BlkReadVME16 FAILED\n");
			return kFALSE;
		}

		Cmd_BlkRead16 *pCmd_BlkRead16;
		OutgoingMsg.Length = HTONL(REMOTE_MSG_SIZE(Cmd_BlkRead16)-4);
		OutgoingMsg.BoardType = HTONL(BOARD_TYPE_DVCSQ);
		OutgoingMsg.MsgType = HTONL(REMOTE_CMD_BLKREAD16);
		pCmd_BlkRead16 = (Cmd_BlkRead16 *)OutgoingMsg.Msg;
		pCmd_BlkRead16->Address = HTONL(addr);
		pCmd_BlkRead16->Count = HTONS(count);
		SendRaw(&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_BlkRead16));

		if( (RecvRaw(&IncomingMsg.Length, 4) == 4) &&
			(NTOHL(IncomingMsg.Length) <= sizeof(RemoteMsgStruct)) &&
			(RecvRaw(&IncomingMsg.BoardType, NTOHL(IncomingMsg.Length)) == NTOHL(IncomingMsg.Length)) )
		{
			IncomingMsg.Length = NTOHL(IncomingMsg.Length);
			IncomingMsg.BoardType = NTOHL(IncomingMsg.BoardType);
			IncomingMsg.MsgType = NTOHL(IncomingMsg.MsgType);
			if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_BLKREAD16))
			{
				unsigned short *p = (unsigned short *)IncomingMsg.Msg;
				for(int i = 0; i < count; i++)
				{
					*vals++ = NTOHS(*p);
					p++;
				}
				return kTRUE;
			}
		}
		Close();
		return kFALSE;
	}

	Bool_t ProgramModule(unsigned int base, const char *filename, TGHProgressBar *pTGHProgressBar = NULL)
	{
		if(!IsValid())
			return kFALSE;
		
		FILE *f = fopen(filename, "rb");
		if(!f)
		{
			printf("Programming Failed - couldn't open file: %s\n", filename);
			return kFALSE;
		}

		Cmd_WritePage *pCmd_WritePage = (Cmd_WritePage *)OutgoingMsg.Msg;
		pCmd_WritePage->Address = HTONL(base);

		OutgoingMsg.Length = HTONL(REMOTE_MSG_SIZE(Cmd_WritePage)-4);
		OutgoingMsg.BoardType = HTONL(BOARD_TYPE_DVCSQ);
		OutgoingMsg.MsgType = HTONL(REMOTE_CMD_WRITEPAGE);

		if(pTGHProgressBar)
			pTGHProgressBar->SetPosition(0);

		int page = 0;
		int nbytes = 0;
		bool quit = false;
		while(!quit)
		{
			int ch = fgetc(f);
			if(ch < 0)
				quit = true;
			else
			{
				pCmd_WritePage->Values[nbytes] = 0;
				for(int i = 0; i < 8; i++)
				{
					if(ch & (1<<i))
						pCmd_WritePage->Values[nbytes] |= 1<<(7-i);
				}
				nbytes++;
			}
			if( (quit && nbytes) || (nbytes == PAGE_SIZE) )
			{
				pCmd_WritePage->Page = HTONL(page+48);
				SendRaw(&OutgoingMsg, REMOTE_MSG_SIZE(Cmd_WritePage));

				if( (RecvRaw(&IncomingMsg.Length, 4) != 4) ||
					(NTOHL(IncomingMsg.Length) > sizeof(RemoteMsgStruct)) ||
					(RecvRaw(&IncomingMsg.BoardType, NTOHL(IncomingMsg.Length)) != NTOHL(IncomingMsg.Length)) )
				{
					printf("Programming Failed - socket error\n");
					Close();
					return kFALSE;
				}
				IncomingMsg.Length = NTOHL(IncomingMsg.Length);
				IncomingMsg.BoardType = NTOHL(IncomingMsg.BoardType);
				IncomingMsg.MsgType = NTOHL(IncomingMsg.MsgType);
				if(IncomingMsg.MsgType == CMD_RSP(REMOTE_CMD_WRITEPAGE))
				{
					Cmd_WritePage_Rsp *pCmd_WritePage_Rsp = (Cmd_WritePage_Rsp *)IncomingMsg.Msg;
					if(!pCmd_WritePage_Rsp->Verified)
					{
						printf("Programming Failed - verify error at page %d\n", page);
						return kFALSE;
					}
				}
				if(pTGHProgressBar)
					pTGHProgressBar->SetPosition((Float_t)page++);
				nbytes = 0;
			}
		}
		fclose(f);
		printf("Progamming Completed and Verified\n");
		return kTRUE;
	}
	const char			*pHostName;
private:
	RemoteMsgStruct		IncomingMsg;
	RemoteMsgStruct		OutgoingMsg;
};

#endif
