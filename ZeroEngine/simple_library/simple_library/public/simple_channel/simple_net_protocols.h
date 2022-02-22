// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "simple_protocols_definition.h"

DEFINITION_SIMPLE_PROTOCOLS(Hello, 0);		//客户端向服务器发送hello
DEFINITION_SIMPLE_PROTOCOLS(Challenge, 1)	//服务器需要验证客户端
DEFINITION_SIMPLE_PROTOCOLS(Login, 2)		//客户端发送登陆请求
DEFINITION_SIMPLE_PROTOCOLS(Welcom, 3)		//服务端发送欢迎
DEFINITION_SIMPLE_PROTOCOLS(Join, 4)		//成功

DEFINITION_SIMPLE_PROTOCOLS(Debug,5)		//主要用于调试信息
DEFINITION_SIMPLE_PROTOCOLS(Failure,6)		//错误
DEFINITION_SIMPLE_PROTOCOLS(Upgrade, 7)		//版本不一致

DEFINITION_SIMPLE_PROTOCOLS(HeartBeat,8)	//心跳
DEFINITION_SIMPLE_PROTOCOLS(Close,9)		//关闭连接的 协议