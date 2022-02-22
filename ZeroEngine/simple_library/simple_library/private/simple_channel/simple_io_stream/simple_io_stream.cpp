// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_channel/simple_core/simple_io_stream.h"


FSimpleIOStream::FSimpleIOStream(TArray<unsigned char>& InBuffer)
	:Buffer(InBuffer)
	,Ptr(nullptr)
{

}

void FSimpleIOStream::Wirte(const void* InData, int InLength)
{
	int Pos = Buffer.AddUninitialized(InLength);
	memcpy(&Buffer[Pos], InData, InLength);
}

FSimpleIOStream& FSimpleIOStream::operator>> (std::string& InValue)
{
	InValue = (char *)Ptr;
	Seek(InValue.size() + 1);

	return *this;
}

FSimpleIOStream& FSimpleIOStream::FSimpleIOStream::operator<<(const std::string& InValue)
{
	Wirte(InValue.c_str(), InValue.size());
	Wirte("\0", 1);

	return *this;
}

FSimpleIOStream& FSimpleIOStream::operator<<(const std::wstring& InValue)
{
	return *this;
}

void FSimpleIOStream::Seek(int InPos)
{
	if (!Ptr)
	{
		Ptr = Buffer.GetData();
	}

	Ptr += InPos;
}

unsigned char* FSimpleIOStream::Begin()
{
	Ptr = Buffer.GetData();
	return Ptr;
}

unsigned char* FSimpleIOStream::End()
{
	Begin();
	Ptr += Buffer.Num();
	return Ptr;
}

unsigned char* FSimpleIOStream::Tall()
{
	return Ptr;
}