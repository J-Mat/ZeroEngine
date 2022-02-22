// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../simple_c_core/simple_core_minimal.h"

_CRT_BEGIN_C_HEADER
typedef enum 
{
	LOCAL_TIME,
	GM
}etime_type;

char *get_locale(char *in_buffer, const char *in_locale, const char *in_encoding);

struct tm *get_local_time_tm();

//获取的是本地时间
time_t get_time_t();

void get_local_time_string(char *t_buffer);

struct tm * get_g_m_time_tm();

struct tm * time_t_to_tm(etime_type type,time_t *const t_a);

time_t tm_to_time_t(struct tm *const t_b);

char *tm_to_string(const struct tm *t);

char *time_t_to_string(etime_type type, const time_t *t);

clock_t get_clock();

double get_clock_diff(clock_t start, clock_t end);
_CRT_END_C_HEADER