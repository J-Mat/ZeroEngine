// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../simple_core_minimal/simple_c_core/simple_core_minimal.h"

_CRT_BEGIN_C_HEADER
enum e_error
{
	SIMPLE_C_SUCCESS = 0,
	SIMPLE_C_LOG,
	SIMPLE_C_WARNING,
	SIMPLE_C_ERROR,
};

const char *get_log_filename();
const char *get_log_path();

void init_log_system(const char *path);

//log
bool log_wirte(enum e_error error, char *format, ...);

#define log_system(type,format,...) \
{ \
	char tmp_log_format[] = format; \
	log_wirte(type, tmp_log_format, __VA_ARGS__); \
}

#define log_success(format,...) log_system(SIMPLE_C_SUCCESS,format,__VA_ARGS__)
#define log_log(format,...) log_system(SIMPLE_C_LOG,format,__VA_ARGS__)
#define log_error(format,...) log_system(SIMPLE_C_ERROR,format,__VA_ARGS__)
#define log_warning(format,...) log_system(SIMPLE_C_WARNING,format,__VA_ARGS__)

_CRT_END_C_HEADER