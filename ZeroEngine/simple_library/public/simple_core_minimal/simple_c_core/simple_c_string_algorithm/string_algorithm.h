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
#include "../simple_core_minimal.h"
 
_CRT_BEGIN_C_HEADER
void remove_string_start(char *str, char const* sub_str);

int find_string(const char *str, char const* sub_str,int start_pos);

void remove_char_start(char *str, char sub_str);

void remove_char_end(char *str, char sub_str);

void remove_all_char_end(char *str, char sub_str);

void replace_char_inline(char *str, const char sub_char_a, const char sub_char_b);

int get_printf(char *buf, char *format, ...);

int get_printf_s(char *out_buf, char *format, ...);

int get_printf_s_s(int buffer_size,char *out_buf, char *format, ...);

char *string_mid(const char *int_buf ,char *out_buf,int start,int count);

int char_to_tchar(const char *str, wchar_t *tc);

int tchar_to_char(const wchar_t *tc, char *str);

//注意 ：str 必须是足够大的空间 不要传一个自动匹配内存的指针
void wremove_string_start(wchar_t *str, wchar_t const* sub_str);

int wfind_string(wchar_t *str, wchar_t const* sub_str);

void wremove_wchar_start(wchar_t *str, wchar_t sub_str);

void wremove_wchar_end(wchar_t *str, wchar_t sub_str);

void wremove_all_wchar_end(wchar_t *str, wchar_t sub_str);

void wreplace_wchar_inline(wchar_t *str, const wchar_t sub_char_a, const wchar_t sub_char_b);

int wget_printf(wchar_t *buf, wchar_t *format, ...);

int wget_printf_s(wchar_t *out_buf, wchar_t *format, ...);

int wget_printf_s_s(int buffer_size, wchar_t *out_buf, wchar_t *format, ...);

wchar_t *wstring_mid(const wchar_t *int_buf, wchar_t *out_buf, int start, int count);

_CRT_END_C_HEADER