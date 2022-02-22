// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "..\..\public\simple_channel\simple_net_type.h"

FSimpleIOData::FSimpleIOData()
{
	ZeroMemory(this, sizeof(FSimpleIOData));
}

FSimpleBunchHead::FSimpleBunchHead()
	:Protocols(0)
	, ChannelID(0)
	, ParamNum(0)
{

}