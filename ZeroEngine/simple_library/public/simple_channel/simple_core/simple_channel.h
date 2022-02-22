// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../../simple_array/simple_hash_array.h"

class FSimpleConnetion;
class FSimpleChannel
{
public:
	FSimpleChannel();

	unsigned int GetGuid() { return ID; };

	void SetGuid(unsigned int InGuid);

	void Send(TArray<unsigned char> &InBuffer);
	bool Receive(TArray<unsigned char>& InBuffer);

	void SetConnetion(FSimpleConnetion* InConnetion);
protected:
	FSimpleConnetion* Connetion;
	unsigned int ID;
};