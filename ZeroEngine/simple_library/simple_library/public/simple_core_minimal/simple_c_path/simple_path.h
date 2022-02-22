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
#include "../simple_c_core/simple_core_minimal.h"

_CRT_BEGIN_C_HEADER
void get_path_directory_inline(char *path_buf);
void get_path_directory(char *buf, const char *path_buf);
void normalization_path(char *path_buf);
void get_path_clean_filename(char *buf, const char *path_buf);
void normalization_directory(char *buf, const char *path_buf);
_CRT_END_C_HEADER
