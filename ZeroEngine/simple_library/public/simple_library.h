#pragma once
// Copyright (C) RenZhai.2022.All Rights Reserved.
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
//
//关于本套案例的详细操作 :
//文字版本(详细)：
//https://zhuanlan.zhihu.com/p/144558934
//视频版本：
//https://www.bilibili.com/video/BV1x5411s7s3

#ifdef CLOSE_DISABLE_DEBUG_INFOR
#pragma warning(disable:4267)//'argument': conversion from 'xx' to 'xx', possible loss of data
#pragma warning(disable:4311)//type cast': pointer truncation from 'your ptr *' to 'DWORD'
#pragma warning(disable:4244)//conversion from 'xx' to 'xx', possible loss of data
#pragma warning(disable:4309)//truncation of constant value
#pragma warning(disable:4305)//'argument': truncation from 'int' to 'u_short'
#pragma warning(disable:4133)//'function': incompatible types - from 'char [260]' to 'LPWSTR'
#pragma warning(disable:4090)//'initializing': different 'const' qualifiers
#pragma warning(disable:4013)// 'XXX' undefined; assuming extern returning int
#endif // DEBUG

//core minimal
#include "simple_core_minimal/simple_c_guid/simple_guid.h"
#include "simple_core_minimal/simple_c_path/simple_path.h"
#include "simple_core_minimal/simple_c_windows/simple_c_windows_setting.h"
#include "simple_core_minimal/simple_c_windows/simple_c_windows_register.h"
#include "simple_core_minimal/simple_c_helper_file/simple_file_helper.h"
#include "simple_core_minimal/simple_c_time/simple_c_time.h"

//core
#include "simple_core_minimal/simple_c_core/simple_c_string_algorithm/string_algorithm.h"
#include "simple_core_minimal/simple_c_core/simple_c_array/simple_c_array.h"


//application 
#include "simple_c_log/simple_c_log.h"

//Delegate
#include "simple_delegate/simple_delegate.h"

//net
#include "simple_channel/simple_net_drive.h"

//math
#include "simple_math/simple_core_math.h"
