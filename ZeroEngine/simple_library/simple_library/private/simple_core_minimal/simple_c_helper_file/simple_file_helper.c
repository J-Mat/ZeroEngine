// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_core_minimal/simple_c_helper_file/simple_file_helper.h"
#include "../../../public/simple_core_minimal/simple_c_core/simple_c_array/simple_c_array_string.h"

//用于检测ShellExecute的返回值信息
bool check_ShellExecute_ret(int ret)
{
	if (ret == 0)
	{
		// 内存不足
		assert(0, "open_url_w=>insufficient memory.");
	}
	else if (ret == 2)
	{
		// 文件名错误
		assert(0, "open_url_w=>File name error.");
	}
	else if (ret == 3)
	{
		// 路径名错误
		assert(0, "open_url_w=>Path name error.");
	}
	else if (ret == 11)
	{
		// EXE 文件无效
		assert(0, "open_url_w=>Invalid .exe file.");
	}
	else if (ret == 26)
	{
		// 发生共享错误
		assert(0, "open_url_w=>A sharing error occurred.");
	}
	else if (ret == 27)
	{
		// 文件名不完全或无效
		assert(0, "open_url_w=>incomplete or invalid file name.");
	}
	else if (ret == 28)
	{
		// 超时
		assert(0, "open_url_w=>timeout.");
	}
	else if (ret == 29)
	{
		// DDE 事务失败
		assert(0, "open_url_w=> DDE transaction failed.");
	}
	else if (ret == 30)
	{
		// 正在处理其他 DDE 事务而不能完成该 DDE 事务
		assert(0, "open_url_w=> is processing another DDE transaction and cannot complete the DDE transaction.");
	}
	else if (ret == 31)
	{
		// 没有相关联的应用程序
		assert(0, "open_url_w=>no associated application.");
	}

	return ret <= 32;
}

void init_def_c_paths(def_c_paths *c_paths)
{
	c_paths->index = 0;
	memset(c_paths->paths,0,sizeof(c_paths->paths) - 1);
}

int copy_file(char *Src, char *Dest)
{
	//当前的缓存 缓存1MB大小，如果超过就会出问题 这个会在std C课程里面继续扩展
	char Buf[1024 * 1024] = { 0 };
	int FileSize = 0;
	FILE *FpSrc = NULL;
	FILE *FpDest = NULL;

	if ((FpSrc = fopen(Src, "rb")) != NULL)
	{
		if ((FpDest = fopen(Dest, "wb")) != NULL)
		{
			while ((FileSize = fread(Buf, 1, 512, FpSrc)) > 0)
			{
				fwrite(Buf, FileSize, sizeof(char), FpDest);
				memset(Buf, 0, sizeof(Buf));
			}

			fclose(FpSrc);
			fclose(FpDest);

			return 0;
		}
	}

	return -1;
}
void find_files(char const *in_path, def_c_paths *str, bool b_recursion)
{
	struct _finddata_t finddata;

	long hfile = 0;
	char tmp_path[8196] = { 0 };
	strcpy(tmp_path, in_path);
	strcat(tmp_path, "\\*");
	if ((hfile = _findfirst(tmp_path, &finddata)) != -1)
	{
		do
		{
			if (finddata.attrib & _A_SUBDIR)
			{
				if (b_recursion)
				{
					if (strcmp(finddata.name, ".") == 0 ||
						strcmp(finddata.name, "..") == 0)
					{
						continue;
					}

					char new_path[8196] = { 0 };
					strcpy(new_path, in_path);
					strcat(new_path, "\\");
					strcat(new_path, finddata.name);

					find_files(new_path, str, b_recursion);
				}
			}
			else
			{
				strcpy(str->paths[str->index], in_path);
				strcat(str->paths[str->index], "\\");
				strcat(str->paths[str->index++], finddata.name);
			}

		} while (_findnext(hfile, &finddata) == 0);
		_findclose(hfile);
	}
}

bool create_file(char const *filename)
{
	FILE *f = NULL;
	if ((f = fopen(filename,"w+")) != NULL)
	{
		fclose(f);

		return true;
	}

	return false;
}

bool create_file_directory(char const *in_path)
{
	simple_c_string c_file;
	if (strstr(in_path, "\\"))
	{
		dismantling_string(in_path, "\\", &c_file);
	}
	else if (strstr(in_path, "/"))
	{
		dismantling_string(in_path, "/", &c_file);
	}

	char path[260] = { 0 };
	for (int i = 0;i < c_file.size;i++)
	{
		char *value = get_string(i,&c_file);
		strcat(value, "\\");
		strcat(path, value);
		if (_access(path,0) == -1)
		{
			_mkdir(path);
		}
	}

	destroy_string(&c_file);

	return _access(path, 0) == 0;
}

bool open_url(const char* url)
{
	//宽字符转为窄字符
	wchar_t path[1024] = { 0 };
	char_to_wchar_t(path,1024, url);

	return open_url_w(path);
}

bool get_file_buf(const char *path, char *buf)
{
	FILE *f = NULL;
	if ((f = fopen(path, "r")) != NULL)
	{
		char buf_tmp[2048] = { 0 };
		int file_size = 0;
		while ((file_size = fread(buf_tmp, 1,1024, f)) > 0)
		{
			strcat(buf, buf_tmp);
			memset(buf_tmp, 0, sizeof(buf_tmp));
		}

		fclose(f);

		return buf[0] != '\0';
	}

	return false;
}

bool add_file_buf(const char *path, char *buf)
{
	FILE *f = NULL;
	if ((f = fopen(path, "a+")) != NULL)
	{
		fprintf(f, "%s", buf);
		fclose(f);

		return true;
	}

	return false;
}

bool add_new_file_buf(const char *path, char *buf)
{
	FILE *f = NULL;
	if ((f = fopen(path, "w+")) != NULL)
	{
		fprintf(f, "%s", buf);
		fclose(f);

		return true;
	}

	return false;
}

bool add_new_file_buf_w(const wchar_t* path, char* buf)
{
	FILE* f = NULL;
	if ((f = _wfopen(path, L"w+")) != NULL)
	{
		fprintf(f, "%s", buf);
		fclose(f);

		return true;
	}

	return false;
}

bool get_file_buf_w(const wchar_t* path, char* buf)
{
	FILE* f = NULL;
	if ((f = _wpopen(path, L"r")) != NULL)
	{
		char buf_tmp[2048] = { 0 };
		int file_size = 0;
		while ((file_size = fread(buf_tmp, 1, 1024, f)) > 0)
		{
			strcat(buf, buf_tmp);
			memset(buf_tmp, 0, sizeof(buf_tmp));
		}

		fclose(f);

		return buf[0] != '\0';
	}

	return false;
}

bool save_data_to_disk_w(const wchar_t* path, char* buf, int buf_size)
{
	FILE* f = NULL;
	if ((f = _wfopen(path, L"w+")) != NULL)
	{
		fwrite(buf, buf_size, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

bool load_data_from_disk_w(const wchar_t* path, char* buf)
{
	FILE* f = NULL;
	if ((f = _wfopen(path, L"rb")) != NULL)
	{
		//把文件指针移动到文件尾部;
		fseek(f, 0, SEEK_END);

		int l = 0;
		//来返回当前文件的位置，返回单位为字节 大于0 代表文件有效
		if ((l = ftell(f)) > 0)
		{
			//把文件指针移到文件头部
			rewind(f);
			//把文件拷贝到缓存
			fread(buf, sizeof(unsigned char), l, f);
			//文件后面需要0结尾
			//buf[l] = '\0';
		}
		fclose(f);

		return true;
	}

	return false;
}

bool open_url_w(const wchar_t* url)
{
	return check_ShellExecute_ret(ShellExecute(NULL,
		L"open",
		url,
		NULL,
		NULL,
		SW_SHOWNORMAL));
}

unsigned int get_file_size_by_filename_w(const wchar_t* filename)
{
	unsigned int file_size = 0;

	FILE* f = NULL;
	if ((f = _wfopen(filename, L"r")) != NULL)
	{
		file_size = get_file_size(f);

		fclose(f);
	}

	return file_size;
}

bool load_data_from_disk(const char* path, char* buf)
{
	FILE* f = NULL;
	if ((f = fopen(path, "rb")) != NULL)
	{
		//把文件指针移动到文件尾部;
		fseek(f, 0, SEEK_END);

		int l = 0;
		//来返回当前文件的位置，返回单位为字节 大于0 代表文件有效
		if ((l = ftell(f)) > 0)
		{
			//把文件指针移到文件头部
			rewind(f);
			//把文件拷贝到缓存
			fread(buf, 1, l, f);
			//文件后面需要0结尾
			//buf[l] = '\0';
		}
		fclose(f);

		return true;
	}

	return false;
}

unsigned int get_file_size_by_filename(const char *filename)
{
	unsigned int file_size = 0;

	FILE *f = NULL;
	if ((f = fopen(filename,"r")) != NULL)
	{
		file_size = get_file_size(f);

		fclose(f);
	}

	return file_size;
}

//asdoiajoi ajs aoisjd oaisjd oiasjdoi asodao ijaosijd oaisdja index
unsigned int get_file_size(FILE *file_handle)
{
	unsigned int file_size = 0;

	unsigned int current_read_postion = ftell(file_handle);
	fseek(file_handle, 0, SEEK_END);
	file_size = ftell(file_handle);
	fseek(file_handle, current_read_postion, SEEK_SET);

	return file_size;
}

bool save_data_to_disk(const char* path, char* buf, int buf_size)
{
	FILE* f = NULL;
	if ((f = fopen(path, L"wb")) != NULL)
	{
		fwrite(buf, buf_size, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

size_t wchar_t_to_char(
	_out_pram(char*) dst_char,
	size_t char_size,
	_in_pram(wchar_t const*) _Src)
{
	size_t wchar_t_size = wcslen(_Src);

	size_t wchar_t_to_char_count = 0;
	printf("\nwchar_t to char:[%s];\n", strerror(wcstombs_s(
		&wchar_t_to_char_count, 
		dst_char, char_size, 
		_Src,
		wchar_t_size)));

	return wchar_t_to_char_count;
}

size_t char_to_wchar_t(
	_out_pram(wchar_t*) dst_wchar_t,
	size_t wchar_t_size, 
	_in_pram(char const*) _Src)
{
	size_t char_size = strlen(_Src);
	
	size_t char_to_wchar_t_count = 0;
	printf("\nchar to wchar_t:[%s];\n", strerror(mbstowcs_s(
		&char_to_wchar_t_count, 
		dst_wchar_t,
		wchar_t_size, 
		_Src,
		char_size)));
	
	return char_to_wchar_t_count;
}
