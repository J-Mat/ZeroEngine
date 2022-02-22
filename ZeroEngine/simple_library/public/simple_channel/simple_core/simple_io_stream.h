// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../../simple_cpp_core_minimal/simple_cpp_core_minimal.h"
#include "../../simple_array/simple_hash_array.h"

class FSimpleIOStream
{
public:
	TArray<unsigned char> &Buffer;
	//
	FSimpleIOStream& operator<< (const std::string& InValue);
	FSimpleIOStream& operator<< (const std::wstring& InValue);

	FSimpleIOStream& operator>> (std::string& InValue);

	template<class T>
	FSimpleIOStream& operator<<(const std::vector<T>& InValue)
	{
		*this << InValue.size();
		for (auto &Tmp : InValue)
		{
			Wirte(&Tmp, sizeof(T));
		}
	
		return *this;
	}

	template<class T>
	FSimpleIOStream& operator>>(std::vector<T>& InValue)
	{
		int Size = 0;
		*this >> Size;
		for (int i = 0; i < Size; i++)
		{
			InValue.push_back(T());
			T& Instance = InValue.back();
			*this >> Instance;
		}

		return *this;
	}

	template<class T>
	FSimpleIOStream& operator<<(const T& InValue)
	{
		Wirte(&InValue, sizeof(T));
		return *this;
	}

	template<class T>
	FSimpleIOStream& operator>>(T& InValue)
	{
		Read<T>(InValue);
		return *this;
	}

public:
	FSimpleIOStream(TArray<unsigned char>& InBuffer);

	void Wirte(const void* InData, int InLength);
	template<class T>
	void Read(T& InValue)
	{
		Seek(0);

		InValue = *(T*)Ptr;
		Seek(sizeof(T));
	}

public:
	void Seek(int InPos = 1);

	unsigned char* Begin();
	unsigned char* End();
	unsigned char* Tall();
private:
	unsigned char* Ptr;
};