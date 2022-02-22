// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "..\..\..\public\simple_channel\simple_core\simple_connetion.h"
#include "..\..\..\public\simple_channel\simple_net_protocols.h"
#include "..\..\..\public\simple_c_log\simple_c_log.h"
//#include "..\..\..\public\simple_channel\simple_core\simple_channel.h"

FSimpleConnetion::FSimpleConnetion()
    :ConnetionState(ESimpleConnetionState::FREE)
	,ConnetionType(ESimpleConnetionType::CONNETION_LISTEN)
	,DriveType(ESimpleDriveType::DRIVETYPE_LISTEN)
	,bHeartBeat(false)
	,HeartTime(0.0)
{

}

FSimpleConnetion::~FSimpleConnetion()
{

}

bool FSimpleConnetion::Init()
{
	//先分配10个通道
	for (int i = 0; i < 10; i++)
	{
		Channels.emplace_back(FSimpleChannel());
		FSimpleChannel& Inst = Channels.back();
		Inst.SetConnetion(this);
	}

    return false;
}

void FSimpleConnetion::Close()
{

}

void FSimpleConnetion::Analysis()
{
	FSimpleBunchHead Head = *(FSimpleBunchHead*)GetIOData().Buffer;
	if (Head.ParamNum == 0)
	{
		memset(GetIOData().Buffer, 0, 1024);
		GetIOData().Len = 0;
	}

	if (FSimpleChannel* Channel = GetMainChannel())
	{
		if (GetDriveType() == ESimpleDriveType::DRIVETYPE_LISTEN)
		{
			switch (Head.Protocols)
			{
				case SP_HeartBeat:
				{
					ResetHeartBeat();

					log_log("[HeartBeat] Recv heartbeat. Client [%s]",GetAddrString().c_str());
					
					break;
				}
				case SP_Close:
				{
					Close();
					break;
				}
				default:
				{
					//讲解反射再延
					break;
				}
			}
		}
		else
		{
			switch (Head.Protocols)
			{
				case 0:
				{
					break;
				}
				default:
				{

					break;
				}
			}
		}
	}
}

void FSimpleConnetion::Tick(float InTimeInterval)
{
	for (auto &Tmp : Channels)
	{
		
	}

	if (ConnetionState == ESimpleConnetionState::JOIN)
	{
		if (DriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
		{
			if (ConnetionType == ESimpleConnetionType::CONNETION_LISTEN)
			{
				//超时连接
				CheckTimeout();
			}
		}
		else if (bHeartBeat) //客户端
		{		
			HeartTime += InTimeInterval;
			if (HeartTime >= 3.f)
			{
				HeartTime = 0.0;

				SendHeartBeat();
			}		
		}
	}
}

BOOL FSimpleConnetion::Recv()
{
	IOData.Type = 0;
	IOData.WsaBuffer.buf = IOData.Buffer;
	IOData.WsaBuffer.len = 1024;
	if (DriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
	{
		DWORD Flag = 0;
		DWORD Len = 0;
		if (WSARecv(Socket,
			&IOData.WsaBuffer,
			1,
			&Len, &Flag, &IOData.Overlapped, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)//GetOverlappedResult
			{
				return FALSE;
			}
		}
	}
	else
	{
		int Flag = 0;
		int Len = 1024;
		int RecvCount = recv(Socket, IOData.WsaBuffer.buf, Len, Flag);
		if (RecvCount == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)//GetOverlappedResult
			{
				return FALSE;
			}
		}
		else
		{
			IOData.WsaBuffer.len = RecvCount;
		}
	}

	return TRUE;
}

void FSimpleConnetion::SetDriveType(const ESimpleDriveType InDriveType)
{
	DriveType = InDriveType;
}

BOOL FSimpleConnetion::Send()
{
	DWORD Flag = 0L;
	IOData.Type = 1;
	IOData.WsaBuffer.buf = IOData.Buffer;
	IOData.WsaBuffer.len = IOData.Len;
	if (send(Socket,IOData.WsaBuffer.buf,IOData.WsaBuffer.len, Flag) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)//GetOverlappedResult
		{
			return FALSE;
		}
	}

	return TRUE;
}

void FSimpleConnetion::RecvBuffer(TArray<unsigned char>& InBuffer)
{
	if (FSimpleBunchHead* Head = (FSimpleBunchHead*)IOData.Buffer)
	{
		if (Head->ParamNum > 0)
		{
			InBuffer.AddUninitialized(IOData.Len);
			void* InData = InBuffer.GetData();
			memcpy(InData, IOData.Buffer, InBuffer.Num());			
		}
	}

	memset(IOData.Buffer, 0, InBuffer.Num());
}

void FSimpleConnetion::SetBuffer(TArray<unsigned char>& InBuffer)
{
	void *InData = InBuffer.GetData();
	IOData.Len = InBuffer.Num();
	memset(IOData.Buffer, 0, 1024);
	memcpy(IOData.Buffer, InData,InBuffer.Num());
}

void FSimpleConnetion::StartSendHeartBeat()
{
	bHeartBeat = true;
}

void FSimpleConnetion::CheckTimeout()
{
	auto CurrentTime = std::chrono::steady_clock::now();
	auto Duration = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - LastTime);
	double TimeInterval = double(Duration.count());
	if (TimeInterval > 60)
	{
		Close();
	}
}

void FSimpleConnetion::ResetHeartBeat()
{
	LastTime = std::chrono::steady_clock::now();
}

void FSimpleConnetion::SendHeartBeat()
{
	if (FSimpleChannel *Channel = GetMainChannel())
	{
		if (1)
		{
			SIMPLE_PROTOCOLS_SEND(SP_HeartBeat);

			log_log("[HeartBeat] Send heartbeat. Server [%s]",GetAddrString().c_str());
		}
	}
}

void FSimpleConnetion::SetConnetionState(const ESimpleConnetionState& InNewConnetionState)
{
    ConnetionState = InNewConnetionState;
}

void FSimpleConnetion::SetConnetionType(const ESimpleConnetionType& InSimpleConnetionType)
{
	ConnetionType = InSimpleConnetionType;
}

FSimpleChannel* FSimpleConnetion::GetMainChannel()
{
	return &(*Channels.begin());
}

FSimpleChannel* FSimpleConnetion::GetChannel(int InID)
{
	for (auto &Tmp : Channels)
	{
		if (Tmp.GetGuid() == InID)
		{
			return &Tmp;
		}
	}

	return nullptr;
}

void FSimpleConnetion::GetChannelActiveID(std::vector<int>& InIDs)
{
	for (auto &Tmp : Channels)
	{
		InIDs.push_back(Tmp.GetGuid());
	}
}

//127.0.0.1:7777
std::string FSimpleConnetion::GetAddrString()
{
	std::string StringAddr = inet_ntoa(ConnetAddr.sin_addr);
	
	char Buff[32] = { 0 };
	_itoa_s(ConnetAddr.sin_port, Buff, 32, 10);

	return StringAddr + ":" + Buff;
}

std::list<FSimpleChannel>* FSimpleConnetion::GetChannels()
{
	return &Channels;
}
