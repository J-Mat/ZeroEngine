// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../../../public/simple_channel/simple_net_drive.h"

class FSimpleTCPNetDrive :public FSimpleNetDrive
{
	typedef FSimpleNetDrive Super;
public:
	FSimpleTCPNetDrive(ESimpleDriveType InDriveType);

	virtual bool Init();
	virtual void Tick(double InTimeInterval);
protected:
	virtual void SetNonblocking();
public:
	static HANDLE CompletionPortHandle;
protected:
	ESimpleDriveType DriveType;
	HANDLE hThreadHandle[32];
	WSADATA WsaData;
};