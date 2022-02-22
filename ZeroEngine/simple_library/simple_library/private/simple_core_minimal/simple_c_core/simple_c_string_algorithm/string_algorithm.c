// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../../public/simple_core_minimal/simple_c_core/simple_c_string_algorithm/string_algorithm.h"

void remove_char_end(char *str, char sub_str)
{
	int len = strlen(str);

	for (int i = len; i >= 0; i--)
	{
		if (str[i] == sub_str)
		{
			strcpy(&str[i], &str[i + 1]);
			break;
		}
	}
}

void remove_all_char_end(char *str, char sub_str)
{
	int len = strlen(str);

	for (int i = len; i >= 0; i--)
	{
		if (str[i] == sub_str)
		{
			strcpy(&str[i], &str[i + 1]);
		}
	}
}

void remove_char_start(char *str, char sub_str)
{
	int len = strlen(str) + 1;

	//从头开始检查 然后删除对象
	for (int i = 0; i < len; i++)
	{
		if (str[i] == sub_str)
		{
			do
			{
				str[i] = str[i + 1];
				i++;
			} while (str[i + 1] != '\0');
			str[i] = '\0';

			break;
		}
	}
}

// "wearwetryy wrwq asdgddawtdgh"
// "as"
int find_string(const char *str, char const* sub_str,int start_pos)
{
	int len = strlen(sub_str);
	int index = -1;
	for (int i = start_pos; str[i] != '\0'; i++)
	{
		if (sub_str[0] == str[i])
		{
			int tmp_index = i;
			int l = 1;//第一个是成功
			while (sub_str[l] == str[i + l] && sub_str[l] != '\0')
			{
				l++;
			}

			if (len == l)
			{
				index = tmp_index;
				break;
			}
		}
	}

	return index;
}

void remove_string_start(char *str, char const* sub_str)
{
	int index = find_string(str, sub_str,0);
	if (index != -1)
	{
		strcpy(&str[index], &str[index + strlen(sub_str)]);
	}
}

void replace_char_inline(char *str, const char sub_char_a, const char sub_char_b)
{
	int index = find_string(str, &sub_char_a, 0);
	if (index != -1)
	{
		str[index] = sub_char_b;
		replace_char_inline(str, sub_char_a, sub_char_b);
	}
}

int get_printf(char *buf, char *format, ...)
{
	va_list param_list;
	va_start(param_list, format);
	char param_char = *format++;

	for (int i = 0; param_char != '\0'; i++)
	{
		if (param_char != '\%' && *(format - 1) != '\%')
		{
			buf[i] = param_char;
		}
		else
		{
			param_char = *format++;
			switch (param_char)
			{
			case 'c':
			case 'C':
			{
				buf[i] = va_arg(param_list, char);
				break;
			}
			case 's':
			case 'S':
			{
				char *p = va_arg(param_list, char*);
				int len = strlen(p);
				for (int j = 0; j < len;)
				{
					buf[i++] = p[j++];
				}
				i--;

				break;
			}
			case 'd':
			case 'D':
			case 'i':
			case 'I':
			{
				int new_int = va_arg(param_list, int);
				char buf_int[8] = { 0 };
				char *p = _itoa(new_int, buf_int, 10);
				int len = strlen(p);
				for (int j = 0; j < len;)
				{
					buf[i++] = p[j++];
				}
				i--;
				break;
			}
			}
		}

		param_char = *format++;
	}

	return strlen(buf) + 1;
}

int get_printf_s(char *out_buf, char *format, ...)
{
	char buf[8196 * 1024] = { 0 };
	memset(buf, 0,sizeof(char) * 8196 * 1024);
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buf, 8196 * 1024 - 1, 8196 * 1024, format, args);
	va_end(args);
	buf[8196 * 1024 - 1] = 0;

	strcat(out_buf, buf);
	return strlen(out_buf) + 1;
}

int get_printf_s_s(int buffer_size, char *out_buf, char *format, ...)
{
	char *buf = (char*)malloc(buffer_size);
	memset(buf, 0, sizeof(char) *buffer_size);
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buf, buffer_size - 1, buffer_size, format, args);
	va_end(args);
	buf[buffer_size - 1] = 0;

	strcat(out_buf, buf);
	free(buf);
	return strlen(out_buf) + 1;
}

char *string_mid(const char *int_buf, char *out_buf, int start, int count)
{
	char *p = &int_buf[start];
	memcpy_s(out_buf,count,p, count);

	return out_buf;
}

int char_to_tchar(const char *str, wchar_t *tc)
{
	return MultiByteToWideChar(CP_UTF7, 0, str, strlen(str), tc, strlen(str));;
}

int tchar_to_char(const wchar_t *tc, char *str)
{
	return WideCharToMultiByte(CP_ACP, 0, tc, -1, str, wcslen(tc), NULL, NULL);
}

void wremove_string_start(wchar_t *str, wchar_t const* sub_str)
{
	int index = wfind_string(str, sub_str);
	if (index != -1)
	{
		wcscpy(&str[index], &str[index + wcslen(sub_str)]);
	}
}

int wfind_string(wchar_t *str, wchar_t const* sub_str)
{
	int len = wcslen(sub_str);
	int index = -1;
	for (int i = 0; str[i] != L'\0'; i++)
	{
		if (sub_str[0] == str[i])
		{
			int tmp_index = i;
			int l = 1;//第一个是成功
			while (sub_str[l] == str[i + l] && sub_str[l] != L'\0')
			{
				l++;
			}

			if (len == l)
			{
				index = tmp_index;
				break;
			}
		}
	}

	return index;
}

void wremove_wchar_start(wchar_t *str, wchar_t sub_str)
{
	int len = wcslen(str) + 1;

	//从头开始检查 然后删除对象
	for (int i = 0; i < len; i++)
	{
		if (str[i] == sub_str)
		{
			do
			{
				str[i] = str[i + 1];
				i++;
			} while (str[i + 1] != L'\0');
			str[i] = L'\0';

			break;
		}
	}
}

void wremove_wchar_end(wchar_t *str, wchar_t sub_str)
{
	int len = wcslen(str);

	for (int i = len; i >= 0; i--)
	{
		if (str[i] == sub_str)
		{
			wcscpy(&str[i], &str[i + 1]);
			break;
		}
	}
}

void wremove_all_wchar_end(wchar_t *str, wchar_t sub_str)
{
	int len = wcslen(str);

	for (int i = len; i >= 0; i--)
	{
		if (str[i] == sub_str)
		{
			wcscpy(&str[i], &str[i + 1]);
		}
	}
}

void wreplace_wchar_inline(wchar_t *str, const wchar_t sub_char_a, const wchar_t sub_char_b)
{
	int index = wfind_string(str, &sub_char_a);
	if (index != -1)
	{
		str[index] = sub_char_b;
		wreplace_wchar_inline(str, sub_char_a, sub_char_b);
	}
}

int wget_printf(wchar_t *buf, wchar_t *format, ...)
{
	va_list param_list;
	va_start(param_list, format);
	wchar_t param_char = *format++;

	for (int i = 0; param_char != L'\0'; i++)
	{
		if (param_char != L'\%' && *(format - 1) != L'\%')
		{
			buf[i] = param_char;
		}
		else
		{
			param_char = *format++;
			switch (param_char)
			{
			case L'c':
			case L'C':
			{
				buf[i] = va_arg(param_list, wchar_t);
				break;
			}
			case L's':
			case L'S':
			{
				wchar_t *p = va_arg(param_list, wchar_t*);
				int len = wcslen(p);
				for (int j = 0; j < len;)
				{
					buf[i++] = p[j++];
				}
				i--;

				break;
			}
			case L'd':
			case L'D':
			case L'i':
			case L'I':
			{
				int new_int = va_arg(param_list, int);
				char buf_int[8] = { 0 };
				char *p = _itoa(new_int, buf_int, 10);

				wchar_t wbuf_int[8] = { 0 };
				char_to_tchar(p,wbuf_int);

				int len = wcslen(wbuf_int);
				for (int j = 0; j < len;)
				{
					buf[i++] = wbuf_int[j++];
				}
				i--;
				break;
			}
			}
		}

		param_char = *format++;
	}

	return wcslen(buf) + 1;
}

int wget_printf_s(wchar_t *out_buf, wchar_t *format, ...)
{
	wchar_t *buf[8196 * 1024] = { 0 };
	wmemset(buf, 0, sizeof(wchar_t) * 8196 * 1024);
	va_list args;
	va_start(args, format);
	_vsnwprintf_s(buf, 8196 * 1024 - 1, 8196 * 1024, format, args);
	va_end(args);
	buf[8196 * 1024 - 1] = 0;

	wcscat(out_buf, buf);
	return wcslen(out_buf) + 1;
}

int wget_printf_s_s(int buffer_size, wchar_t *out_buf, wchar_t *format, ...)
{
	int size = buffer_size / sizeof(wchar_t);
	wchar_t *buf = (wchar_t*)malloc(buffer_size);
	wmemset(buf, 0, size);
	va_list args;
	va_start(args, format);
	_vsnwprintf_s(buf, size - 1, size, format, args);
	va_end(args);
	buf[size - 1] = 0;

	wcscat(out_buf, buf);
	free(buf);
	return wcslen(out_buf) + 1;
}

wchar_t *wstring_mid(const wchar_t *int_buf, wchar_t *out_buf, int start, int count)
{
	wchar_t *p = &int_buf[start];
	wmemcpy_s(out_buf, count, p, count);

	return out_buf;
}