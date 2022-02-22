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
#include "Core/simple_array_c_macro.h"

typedef struct
{
	char data[8196];
}str_node;

SIMPLE_ARRAY_C_STRUCT(simple_c_string,str_node)

_CRT_BEGIN_C_HEADER
 void init_string(simple_c_string *array_c);//初始化 分配内存

 void destroy_string(simple_c_string *array_c);

 void add_string(char const *  in_data,simple_c_string *array_c);

 void printf_string(simple_c_string *array_c);

 char *get_string(int in_index,simple_c_string *array_c);

 void dismantling_string(const char *in_data, const char *str_sub, simple_c_string *array_c);

 char *dismantling_string_by_index(int index, const char *split_str, const char *sub_str, char *buf);

 bool is_exist(char const *  in_data, simple_c_string *array_c);

 _CRT_END_C_HEADER