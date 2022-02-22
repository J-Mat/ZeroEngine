// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "simple_core/simple_channel.h"
#include "simple_core/simple_io_stream.h"
#include "simple_net_type.h"

#define DEFINITION_SIMPLE_BUFFER \
	TArray<unsigned char> Buffer; \
	FSimpleIOStream Stream(Buffer);

template<unsigned int ProtocolsType>
class FSimpleProtocols{};

class FRecursionMessageInfo
{
public:
	template<typename ...ParamTypes>
	static unsigned int GetBuildParams(ParamTypes &...Params)
	{
		return sizeof ...(Params);
	}

	template<typename ...ParamTypes>
	static void BuildSendParams(FSimpleIOStream& InStream, ParamTypes &...Params){}

	template<class T,typename ...ParamTypes>
	static void BuildSendParams(FSimpleIOStream& InStream, T& FirestParam, ParamTypes &...Params)
	{
		InStream << FirestParam;
		BuildSendParams(InStream, Params...);
	}

	template<typename ...ParamTypes>
	static void BuildReceiveParams(FSimpleIOStream& InStream, ParamTypes &...Params) {}

	template<class T, typename ...ParamTypes>
	static void BuildReceiveParams(FSimpleIOStream& InStream, T& FirestParam, ParamTypes &...Params)
	{
		InStream >> FirestParam;
		BuildReceiveParams(InStream, Params...);
	}
};

#define DEFINITION_SIMPLE_PROTOCOLS(ProtocolsName,ProtocolsNumber) \
enum {SP_##ProtocolsName = ProtocolsNumber}; \
template<> class FSimpleProtocols<ProtocolsNumber>\
{\
public:\
	template<typename ...ParamTypes>\
	static void Send(FSimpleChannel* InChannel, ParamTypes &...Params)\
	{\
		DEFINITION_SIMPLE_BUFFER \
		FSimpleBunchHead Head;\
		Head.Protocols = ProtocolsNumber;\
		Head.ParamNum = FRecursionMessageInfo::GetBuildParams(Params...);\
		Head.ChannelID = InChannel->GetGuid();\
		Stream << Head;\
		FRecursionMessageInfo::BuildSendParams(Stream, Params...);\
		InChannel->Send(Buffer);\
	}\
	template<typename ...ParamTypes>\
	static void Receive(FSimpleChannel* InChannel, ParamTypes &...Params)\
	{\
		DEFINITION_SIMPLE_BUFFER \
		if (InChannel->Receive(Buffer))\
		{\
			Stream.Seek(sizeof(FSimpleBunchHead));\
			FRecursionMessageInfo::BuildReceiveParams(Stream, Params...);\
		}\
	}\
};

#define SIMPLE_PROTOCOLS_SEND(InProtocols,...) FSimpleProtocols<InProtocols>::Send(Channel,__VA_ARGS__);
#define SIMPLE_PROTOCOLS_RECEIVE(InProtocols,...)  FSimpleProtocols<InProtocols>::Receive(Channel,__VA_ARGS__);

//案例
//协议的定义
//DEFINITION_SIMPLE_PROTOCOLS(Debug,0)
//发送端
//int a, int b;
//std::stirng data = "Hello World";
//FSimpleChannel *Channel;
//SIMPLE_PROTOCOLS_SEND(Channel, a, b, data);

//接受端
//int a, int b;
//std::stirng data;
//FSimpleChannel *Channel;
//SIMPLE_PROTOCOLS_RECEIVE(Channel, a, b, data);
//if(a == 1){...}