// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../../simple_core_minimal/simple_c_core/simple_core_minimal.h"
#include "../simple_net_type.h"
#include "../../simple_array/simple_hash_array.h"
#include "../../simple_cpp_core_minimal/simple_cpp_core_minimal.h"
#include "simple_channel.h"
#include "../../../public/simple_cpp_core_minimal/simple_cpp_core_minimal.h"

//class FSimpleChannel;
class FSimpleConnetion
{
public:
	FSimpleConnetion();
	~FSimpleConnetion();
	virtual bool Init();
	virtual void Tick(float InTimeInterval);
	virtual void Close();

	void Analysis();

	virtual BOOL Recv();
	virtual BOOL Send();
	void RecvBuffer(TArray<unsigned char>& InBuffer);
	void SetBuffer(TArray<unsigned char> &InBuffer);

	void StartSendHeartBeat();
	void SendHeartBeat();
	void CheckTimeout();

	void ResetHeartBeat();

	void SetConnetionState(const ESimpleConnetionState &InNewConnetionState);
	void SetConnetionType(const ESimpleConnetionType& InSimpleConnetionType);
	void SetDriveType(const ESimpleDriveType InDriveType);
	FSimpleChannel* GetMainChannel();
	FSimpleChannel* GetChannel(int InID);

	void GetChannelActiveID(std::vector<int>& InIDs);
	std::string GetAddrString();
	std::list<FSimpleChannel> *GetChannels();

	FORCEINLINE SOCKET& GetSocket() { return Socket; }
	FORCEINLINE SOCKADDR_IN& GetConnetionAddr() { return ConnetAddr; }
	FORCEINLINE const ESimpleConnetionState GetConnetionState()  const { return ConnetionState; }
	FORCEINLINE FSimpleIOData &GetIOData() { return IOData; }
	FORCEINLINE const ESimpleDriveType& GetDriveType()  const { return DriveType; }
	FORCEINLINE const ESimpleConnetionType& GetConnetionType()  const { return ConnetionType; }
protected:
	SOCKET Socket;
	SOCKADDR_IN ConnetAddr;
	ESimpleConnetionState ConnetionState;
	ESimpleConnetionType ConnetionType;
	ESimpleDriveType DriveType;
	FSimpleIOData IOData;
	std::list<FSimpleChannel> Channels;

	bool bHeartBeat;
	double HeartTime;

	std::chrono::steady_clock::time_point LastTime;
};