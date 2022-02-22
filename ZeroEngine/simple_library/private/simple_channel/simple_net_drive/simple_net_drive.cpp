// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_channel/simple_net_drive.h"
#include "simple_net_drive_udp.h"
#include "simple_net_drive_tcp.h"

FSimpleNetDrive::FSimpleNetDrive()
	:MainConnetion(nullptr)
{

}

FSimpleNetDrive* FSimpleNetDrive::GetNetDrive(ESimpleSocketType InSokcetType, ESimpleDriveType InDriveType)
{
	FSimpleNetDrive* NetDrive = nullptr;
	switch (InSokcetType)
	{
	case ESimpleSocketType::SOCKETTYPE_UDP:
		NetDrive = new FSimpleUDPNetDrive(InDriveType);
		break;
	case ESimpleSocketType::SOCKETTYPE_TCP:
		NetDrive = new FSimpleTCPNetDrive(InDriveType);
		break;
	}

	return NetDrive;
}

bool FSimpleNetDrive::Init()
{
	return false;
}

void FSimpleNetDrive::Tick(double InTimeInterval)
{

}

FSimpleConnetion* FSimpleNetDrive::GetFreeConnetion()
{
	FSimpleConnetion *FreeConnetion = NULL;
	for (auto &Tmp : Connetions)
	{
		if (Tmp.second->GetConnetionState() == ESimpleConnetionState::FREE)
		{
			FreeConnetion = Tmp.second;
			break;
		}
	}

	return FreeConnetion;
}

void FSimpleNetDrive::SetNonblocking()
{

}
