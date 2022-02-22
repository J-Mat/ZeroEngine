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
#include "../../simple_core_minimal.h"

#define SIMPLE_ARRAY_C_STRUCT(array_c_name,data_type) \
typedef struct array_c_name##_type \
{ \
	int size; \
	data_type *data; \
}array_c_name;

#define SIMPLE_ARRAY_C(array_c_name,data_type) \
typedef struct array_c_name##_type \
{ \
	int size; \
	int max_size; \
	data_type *data; \
}array_c_name; \
void init_##array_c_name(array_c_name *array_c) \
{\
	assert(array_c); \
	array_c->size = 0; \
	array_c->max_size = 1; \
	array_c->data = malloc(array_c->max_size * sizeof(data_type)); \
	memset(array_c->data, 0, 1); \
}\
void destroy_##array_c_name(array_c_name *array_c) \
{ \
	assert(array_c); \
	free(array_c->data); \
} \
void add_##array_c_name(data_type in_data, array_c_name *array_c) \
{ \
	assert(array_c && in_data); \
	if (array_c->size >= array_c->max_size) \
	{ \
		array_c->max_size++; \
		array_c->data = realloc(array_c->data, array_c->max_size * sizeof(data_type));\
	} \
	array_c->data[array_c->size] = in_data; \
	array_c->size++; \
} 