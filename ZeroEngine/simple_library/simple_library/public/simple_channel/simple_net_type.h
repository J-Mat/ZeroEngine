// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../simple_core_minimal/simple_c_core/simple_core_minimal.h"

enum class ESimpleSocketType :unsigned char
{
	SOCKETTYPE_UDP,
	SOCKETTYPE_TCP,
}; 

enum class ESimpleDriveType :unsigned char
{
	DRIVETYPE_LISTEN,//������
	DRIVETYPE_CONNECTION,
};

enum class ESimpleConnetionState :unsigned char
{
	FREE,
	VERSION_VERIFICATION,
	LOGIN,
	JOIN,
};

enum class ESimpleConnetionType :unsigned char
{
	CONNETION_MAIN_LISTEN, //��ͨ�� 
	CONNETION_LISTEN,	  //�����Ѿ�Ͷ�ݹ�����
};

struct FSimpleIOData
{
	FSimpleIOData();

	OVERLAPPED Overlapped;
	CHAR Buffer[1024];
	BYTE Type;
	DWORD Len;
	WSABUF WsaBuffer;
};

struct FSimpleBunchHead
{
	FSimpleBunchHead();

	unsigned int Protocols;
	unsigned int ChannelID;
	unsigned int ParamNum;
};