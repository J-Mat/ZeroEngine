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
//
//关于本套案例的详细操作 :
//文字版本(详细)：
//https://zhuanlan.zhihu.com/p/144558934
//视频版本：
//https://www.bilibili.com/video/BV1x5411s7s3
#include "../simple_c_core/simple_core_minimal.h"

_CRT_BEGIN_C_HEADER
typedef enum 
{
	SIMPLE_BLACK = 0,				// 黑色		
	SIMPLE_BLUE,					// 蓝色		
	SIMPLE_GREEN,					// 绿色		
	SIMPLE_LIGHT_BLUE,				// 浅绿色	
	SIMPLE_RED,						// 红色		
	SIMPLE_VIOLET,					// 紫色		
	SIMPLE_YELLOW,					// 黄色		
	SIMPLE_WHITE,					// 白色		
	SIMPLE_GREY,					// 灰色
	SIMPLE_PALE_BLUE,				// 淡蓝色
	SIMPLE_PALE_GREEN,				// 淡绿色
	SIMPLE_LIGHT_GREEN,				// 淡浅绿色
	SIMPLE_PALE_RED,				// 淡红色
	SIMPLE_LAVENDER,				// 淡紫色
	SIMPLE_CANARY_YELLOW,			// 淡黄色
	SIMPLE_BRIGHT_WHITE,			// 亮白色
}simple_console_w_color;

//set_console_windows_color
void set_console_w_color(simple_console_w_color font_color, simple_console_w_color background_color);
_CRT_END_C_HEADER