#pragma once
//Copyright (C) RenZhai.2019.All Rights Reserved.
//作者 人宅
//该c库的详细源码讲解在AboutCG 《手把手入门硬核c语言》视频工程里面：
//https://www.aboutcg.org/courseDetails/902/introduce
//希望学习其他方面技术 比如做UE4游戏, 可以访问下列网址：
//https://zhuanlan.zhihu.com/p/60117613
//
//bibi可以看到各个人宅系列教程介绍：
//https://space.bilibili.com/29544409
//
//个人博客网站
//http://renzhai.net
//https://www.cnblogs.com/Shirlies/p/5137548.html
//关于本套案例的详细操作 :
//文字版本(详细)：
//https://zhuanlan.zhihu.com/p/144558934
//视频版本：
//https://www.bilibili.com/video/BV1x5411s7s3
//#ifndef WIN32_LEAN_AND_MEAN 
//#define WIN32_LEAN_AND_MEAN 
//#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !1
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif // !1

#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <windows.h>
#include <vcruntime.h>
#include <assert.h>
#include <stdio.h>
#include <corecrt_wstdio.h>
#include <stdbool.h>
#include <corecrt_wstdio.h>
#include <stdarg.h>
#include <combaseapi.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <wchar.h>
#include <process.h>
#include <shellapi.h>