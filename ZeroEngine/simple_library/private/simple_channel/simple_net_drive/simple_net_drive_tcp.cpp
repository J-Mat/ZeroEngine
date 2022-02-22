// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "simple_net_drive_tcp.h"
#include "../../../public/simple_c_log/simple_c_log.h"
#include "../simple_net_connetion/simple_connetion_tcp.h"
#include "../../../public/simple_channel/simple_protocols_definition.h"
#include "../../../public/simple_channel/simple_net_protocols.h"

HANDLE FSimpleTCPNetDrive::CompletionPortHandle = nullptr;

FSimpleTCPNetDrive::FSimpleTCPNetDrive(ESimpleDriveType InDriveType)
	:DriveType(InDriveType)
{
	MainConnetion = new FSimpleTCPConnetion();

	if (InDriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
	{
		MainConnetion->SetConnetionState(ESimpleConnetionState::JOIN);
		MainConnetion->SetConnetionType(ESimpleConnetionType::CONNETION_MAIN_LISTEN);
	}
}

void HandShake(FSimpleConnetion* InLink)
{
	if (!InLink)
	{
		return;
	}

	FSimpleBunchHead Head = *(FSimpleBunchHead*)InLink->GetIOData().Buffer;
	if (Head.ParamNum == 0)
	{
		memset(InLink->GetIOData().Buffer, 0, 1024);
		InLink->GetIOData().Len = 0;
	}

	if (FSimpleChannel* Channel = InLink->GetMainChannel())
	{
		if (InLink->GetDriveType() == ESimpleDriveType::DRIVETYPE_LISTEN)
		{
			switch (Head.Protocols)
			{
				case SP_Hello:
				{
					std::string VersionRemote;
					SIMPLE_PROTOCOLS_RECEIVE(SP_Hello, VersionRemote);

					if (VersionRemote == "1.0.1")
					{
						SIMPLE_PROTOCOLS_SEND(SP_Challenge);

						log_log("Server:[Challenge] %s", InLink->GetAddrString().c_str());
					}

					break;
				}
				case SP_Login:
				{
					std::vector<int> Channels;
					SIMPLE_PROTOCOLS_RECEIVE(SP_Login, Channels)
					if (Channels.size() == 10)
					{
						InLink->SetConnetionState(ESimpleConnetionState::LOGIN);

						auto ChannelLists = InLink->GetChannels();
						
						int i = 0;
						for (auto &Tmp :*ChannelLists)
						{
							Tmp.SetGuid(Channels[i]);
							i++;
						}

						SIMPLE_PROTOCOLS_SEND(SP_Welcom);

						log_log("Server:[Welcom] %s", InLink->GetAddrString().c_str());
					}

					break;
				}
				case SP_Join:
				{
					InLink->SetConnetionState(ESimpleConnetionState::JOIN);
					InLink->ResetHeartBeat();

					log_success("Server:[Join] %s", InLink->GetAddrString().c_str());

					break;
				}
			}
		}
		else//客户端
		{
			switch (Head.Protocols)
			{
				case SP_Challenge:
				{
					std::vector<int> Channels;
					InLink->GetChannelActiveID(Channels);
					SIMPLE_PROTOCOLS_SEND(SP_Login, Channels);
					InLink->SetConnetionState(ESimpleConnetionState::LOGIN);

					log_log("Client:[Login] :%s", InLink->GetAddrString().c_str());
				}
				case SP_Welcom:
				{
					InLink->SetConnetionState(ESimpleConnetionState::JOIN);

					SIMPLE_PROTOCOLS_SEND(SP_Join);

					//设置心跳
					InLink->StartSendHeartBeat();

					log_log("Client:[Join] :%s", InLink->GetAddrString().c_str());
				}
			}
		}	
	}
}

unsigned int __stdcall Run(void* Content)
{
	for (;;)
	{
 		DWORD IOSize = -1;
		LPOVERLAPPED lpOverlapped = NULL;
		FSimpleConnetion* InLink = NULL;

		//win32 和 win64
		bool Ret = GetQueuedCompletionStatus(FSimpleTCPNetDrive::CompletionPortHandle, &IOSize, (PULONG_PTR)&InLink, &lpOverlapped, INFINITE);
		if (InLink == NULL && lpOverlapped == NULL)
		{
			break;
		}

		if (Ret)
		{
			if (IOSize == 0)
			{
				//ListRemove(Client);
				continue;
			}

			FSimpleIOData* pData = CONTAINING_RECORD(lpOverlapped,FSimpleIOData, Overlapped);
			switch (pData->Type)
			{
				case 0://接受 客户端发送的消息
				{
					InLink->GetIOData().Len = IOSize;
					InLink->GetIOData().Buffer[IOSize] = '\0';
					if (InLink->GetConnetionState() == ESimpleConnetionState::JOIN)
					{
						//业务逻辑
						InLink->Analysis();
					}
					else
					{
						//
						HandShake(InLink);
					}

					break;
				}
				case 1://发送
				{	
					//printf(InLink->GetIOData().Buffer);
					//InLink->GetIOData().Len = 0;
					//if (!InLink->Recv())
					//{
					//	InLink->Send();
					//	//ListRemove(Client);
					//}

					//break;
				}
			}
		}
		else
		{
			DWORD Msg = GetLastError();
			if (Msg == WAIT_TIMEOUT)
			{
				continue;
			}
			else if (lpOverlapped != NULL)
			{
				//ListRemove(Client);
			}
			else
			{
				break;
			}
		}
	}

	return 0;
}

bool FSimpleTCPNetDrive::Init()
{
	if (DriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
	{
		//完成端口
		if ((CompletionPortHandle = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE,//打开的文件句柄
			NULL,//
			0,
			0)) == NULL)
		{
			//GetLastError();
			log_error("创建完成端口失败 ~~ \n");
			return false;
		}

		//创建线程数目
		for (int i = 0; i < 2 * 2; i++)
		{
			hThreadHandle[i] = (HANDLE)_beginthreadex(
				NULL,// 安全属性， 为NULL时表示默认安全性
				0,// 线程的堆栈大小， 一般默认为0
				Run, // 所要启动的线程函数
				CompletionPortHandle, // 线程函数的参数， 是一个void*类型， 传递多个参数时用结构体
				0,// 新线程的初始状态，0表示立即执行，CREATE_SUSPENDED表示创建之后挂起
				NULL);   // 用来接收线程ID
		}
	}

	int Ret = 0;
	if ((Ret = WSAStartup(MAKEWORD(2, 1), &WsaData)) != 0)
	{
		log_error("初始化库失败 WSAStartup ~~ \n");
		return false;
	}

	//执行Connetion初始化
	MainConnetion->Init();
	MainConnetion->SetDriveType(DriveType);

	if (DriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
	{
		//创建Socket
		MainConnetion->GetSocket() = INVALID_SOCKET;
		if ((MainConnetion->GetSocket() = WSASocket(
			AF_INET,//
			SOCK_STREAM, //TCP  SOCK_DGRAM
			0, //协议
			NULL,//
			0,
			WSA_FLAG_OVERLAPPED//
		)) == INVALID_SOCKET)
		{
			WSACleanup();
			log_error("创建监听Socket失败 WSAStartup ~~ \n");
			return false;
		}

		//服务端设置地址
		MainConnetion->GetConnetionAddr().sin_family = AF_INET;//IPV4互联网协议族
		MainConnetion->GetConnetionAddr().sin_addr.S_un.S_addr = htonl(INADDR_ANY);//0.0.0.0 所以地址、
		MainConnetion->GetConnetionAddr().sin_port = htons(98592);

		if (bind(MainConnetion->GetSocket(), (SOCKADDR*)&MainConnetion->GetConnetionAddr(), sizeof(MainConnetion->GetConnetionAddr())) == SOCKET_ERROR)
		{
			closesocket(MainConnetion->GetSocket());
			WSACleanup();
			log_error("绑定主连接失败 ~~ \n");
			return false;
		}

		if (listen(MainConnetion->GetSocket(), SOMAXCONN))
		{
			closesocket(MainConnetion->GetSocket());
			WSACleanup();
			log_error("启动监听失败 ~~ \n");
			return false;
		}

		//初始化我们通道
		for (int i = 0 ;i < 2000;i++)
		{
			Connetions.insert(std::make_pair(i, new FSimpleTCPConnetion()));
			Connetions[i]->Init();
			Connetions[i]->SetDriveType(DriveType);
		}
	}
	else
	{
		MainConnetion->GetSocket() = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (MainConnetion->GetSocket() == INVALID_SOCKET)
		{
			WSACleanup();
			log_error("创建监听Socket失败 WSAStartup ~~ \n");
			return false;
		}

		//客户端设置地址
		MainConnetion->GetConnetionAddr().sin_family = AF_INET;//IPV4互联网协议族
		MainConnetion->GetConnetionAddr().sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//0.0.0.0 所以地址、
		MainConnetion->GetConnetionAddr().sin_port = htons(98592);

		if (connect(
			MainConnetion->GetSocket(),
			(SOCKADDR*)&MainConnetion->GetConnetionAddr(),
			sizeof(MainConnetion->GetConnetionAddr())) == SOCKET_ERROR)
		{
			closesocket(MainConnetion->GetSocket());
			WSACleanup();
			log_error("客户端连接失败 ~~ \n");
			return false;
		}

		//向服务器发送验证
		if (FSimpleChannel* Channel = MainConnetion->GetMainChannel())
		{
			std::string v = "1.0.1";
			SIMPLE_PROTOCOLS_SEND(SP_Hello, v);

			log_log("Client send [Hello] to server[addr : %s]~~ \n", MainConnetion->GetAddrString().c_str());
		}
	}

	//设置非阻塞
	SetNonblocking();

	return true;
}

void FSimpleTCPNetDrive::Tick(double InTimeInterval)
{
	Super::Tick(InTimeInterval);

	if (DriveType == ESimpleDriveType::DRIVETYPE_LISTEN)
	{	
		//主通道tick
		MainConnetion->Tick(InTimeInterval);

		//做接受检测
		for (auto &Tmp :Connetions)
		{
			if (Tmp.second->GetConnetionState() != ESimpleConnetionState::FREE)
			{
				Tmp.second->Recv();
			}
			else if (Tmp.second->GetConnetionState() != ESimpleConnetionState::JOIN)
			{
				Tmp.second->Tick(InTimeInterval);
			}
		}

		//iocp的投递
		SOCKET ClientAccept = INVALID_SOCKET;
		SOCKADDR_IN ClientAddr;
		int ClientAddrLen = sizeof(ClientAddr);

		if ((ClientAccept = WSAAccept(//阻塞程序
			MainConnetion->GetSocket(),
			(SOCKADDR*)&ClientAddr,
			&ClientAddrLen,
			NULL,
			0)) == SOCKET_ERROR)
		{
			//log_error("接受客户端投递失败");
			return;
		}

		//首先拿到闲置的
		if (FSimpleConnetion* FreeConnetion = GetFreeConnetion())
		{
			//绑定完成端口
			if (CreateIoCompletionPort(
				(HANDLE)ClientAccept,
				CompletionPortHandle,
				(DWORD)FreeConnetion, 0) == NULL)
			{
				log_error("客户端绑定端口失败");
				return;
			}

			//绑定Socket 和 具体地址
			FreeConnetion->GetSocket() = ClientAccept;
			FreeConnetion->GetConnetionAddr() = ClientAddr;

			if (!FreeConnetion->Recv())
			{
				//ListRemove(InClient);
				log_error("客户端接受失败");
				return;
			}
			else
			{
				FreeConnetion->SetConnetionState(ESimpleConnetionState::VERSION_VERIFICATION);
			}
		}
	}
	else
	{
		MainConnetion->Recv();

		//做解析
		//log_success("客户端接受成功 %s", MainConnetion->GetIOData().Buffer);
		if (MainConnetion->GetConnetionState() != ESimpleConnetionState::JOIN)
		{
			HandShake(MainConnetion);
		}
		else
		{
			MainConnetion->Tick(InTimeInterval);

			//做上层业务的解析
			MainConnetion->Analysis();
		}
	}
}

void FSimpleTCPNetDrive::SetNonblocking()
{
	unsigned long UL = 1;
	int Ret = ioctlsocket(MainConnetion->GetSocket(), FIONBIO, &UL);
	if (Ret == SOCKET_ERROR)
	{
		log_error("Set Non-blocking 失败");
	}
}
