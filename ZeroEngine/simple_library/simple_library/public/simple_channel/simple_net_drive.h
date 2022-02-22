// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../simple_core_minimal/simple_c_core/simple_core_minimal.h"
#include "simple_net_type.h"
#include "simple_core/simple_connetion.h"
#include "../simple_cpp_core_minimal/simple_cpp_core_minimal.h"

class FSimpleNetDrive
{
public:
	FSimpleNetDrive();

	static FSimpleNetDrive* GetNetDrive(ESimpleSocketType InSokcetType, ESimpleDriveType InDriveType);

	virtual bool Init();

	virtual void Tick(double InTimeInterval);

protected:
	FSimpleConnetion* GetFreeConnetion();
	virtual void SetNonblocking();
protected:
	FSimpleConnetion* MainConnetion;
	std::map<int, FSimpleConnetion*> Connetions;
};