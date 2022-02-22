// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../public/simple_c_log/simple_c_log.h"
#include "../../public/simple_core_minimal/simple_c_windows/simple_c_windows_setting.h"
#include "../../public/simple_core_minimal/simple_c_core/simple_c_string_algorithm/string_algorithm.h"

char log_path[MAX_PATH] = { 0 }; //存储着我们的路径
char log_filename[MAX_PATH] = { 0 };//具体文件

const char *get_log_filename()
{
	if (log_filename[0] == '\0')
	{
		if (log_path[0] == '\0')
		{
			set_console_w_color(SIMPLE_RED, SIMPLE_BLACK);
			printf("\n You need to add the log path in init_log_system(...) first.  \n");
			//assert(0);
			set_console_w_color(SIMPLE_WHITE, SIMPLE_BLACK);

			return NULL;
		}

		const char *p = log_path;

		char tmp_path[MAX_PATH] = { 0 };
		strcpy(tmp_path, p);
		strcat(tmp_path, "\\");
		_mkdir(tmp_path);

		time_t now_time = time(NULL);

		//char* p_time = ctime(__TIME__);//日期获取失败
		char p_time[256] = { 0 };
		get_local_time_string(p_time);
		if (p_time)
		{
			remove_char_end(p_time, '\n');
			remove_char_end(p_time, ':');
			remove_char_end(p_time, ':');			
			strcat(tmp_path, p_time);
		}
		else
		{
			strcat(tmp_path, "MyLog");
		}
	
		const char file_suffix[] = ".txt";

		strcat(tmp_path, file_suffix);

		FILE *hfile = NULL;
		if ((hfile = fopen(tmp_path, "a+")) != NULL)
		{
			strcpy(log_filename, tmp_path);
			fclose(hfile);
		}
	}

	return log_filename;
}

const char * get_log_path()
{
	if (log_path[0] == '\0')
	{
		set_console_w_color(SIMPLE_RED, SIMPLE_BLACK);
		printf("\n You need to add the log path in init_log_system(...) first.  \n");
		assert(0);
		set_console_w_color(SIMPLE_WHITE, SIMPLE_BLACK);
	}

	return log_path;
}

void init_log_system(const char *path)
{
	strcpy(log_path, path);
}

bool log_wirte(enum e_error error, char *format, ...)
{
	char error_str[64] = { 0 };
	switch (error)
	{
	case SIMPLE_C_SUCCESS:
		strcpy(error_str, "SUCCESS");
		break;
	case SIMPLE_C_LOG:
		strcpy(error_str, "LOG");
		break;
	case SIMPLE_C_WARNING:
		strcpy(error_str, "WARNING");
		break;
	case SIMPLE_C_ERROR:
		strcpy(error_str, "ERROR");
		break;
	}

	const char *p = get_log_filename();

	if (p != NULL)
	{
		FILE* hfile = NULL;
#if _WIN64

#elif _WIN32
		if ((hfile = fopen(p, "a+")) != NULL)
		{
			char buf[8196 * 1024] = { 0 };
			ZeroMemory(buf, sizeof(char) * 8196 * 1024);
			va_list args;
			va_start(args, format);
			_vsnprintf_s(buf, 8196 * 1024 - 1, 8196 * 1024, format, args);
			va_end(args);
			buf[8196 * 1024 - 1] = 0;

			//char* time = ctime(__TIME__);// \n
			char time[256] = { 0 };
			get_local_time_string(time);
			if (time)
			{
				remove_char_end(time, '\n');
			}
	
			char text_buf[8196 * 1024] = { 0 };
			get_printf_s(text_buf, "[%s] [%s] %s \r\n", error_str, time, buf);

			switch (error)
			{
			case SIMPLE_C_SUCCESS:
				set_console_w_color(SIMPLE_PALE_GREEN, SIMPLE_BLACK);
				break;
			case SIMPLE_C_LOG:
				set_console_w_color(SIMPLE_WHITE, SIMPLE_BLACK);
				break;
			case SIMPLE_C_WARNING:
				set_console_w_color(SIMPLE_YELLOW, SIMPLE_BLACK);
				break;
			case SIMPLE_C_ERROR:
				set_console_w_color(SIMPLE_RED, SIMPLE_BLACK);
				break;
			}
			printf(text_buf);
			fprintf(hfile, text_buf);
			set_console_w_color(SIMPLE_WHITE, SIMPLE_BLACK);
			fclose(hfile);
		}
#endif
		return true;
	}

	return false;
}
