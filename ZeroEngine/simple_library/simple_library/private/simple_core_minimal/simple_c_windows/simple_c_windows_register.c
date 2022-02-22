// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_core_minimal/simple_c_windows/simple_c_windows_register.h"

void init_feg_info(freg_info *info)
{
	info->size_value = 0;
	memset(info->filename, 0, sizeof(info->filename));
	memset(info->value, 0, sizeof(info->value));
}


bool delete_reg_key(HKEY hkey, const char *filename, const char *subkey_name)
{
	HKEY hresult;
	long value = -1;
	if ((value = RegOpenKeyEx(hkey, filename, 0, KEY_READ, &hresult)) == ERROR_SUCCESS)
	{
		if (value = RegDeleteKey(hresult, subkey_name) != ERROR_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

bool delete_register_info(HKEY hkey, const char *filename)
{
	int error_count = 0;

	HKEY hresult;
	long value = -1;
	DWORD count;
	if ((value = RegOpenKeyEx(hkey, filename, 0, KEY_READ, &hresult)) == ERROR_SUCCESS)
	{
		if ((value = RegQueryInfoKey(hresult, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
		{
			char subkey_name[MAX_PATH] = { 0 };
			for (DWORD i = 0; i < count; i++)
			{
				if ((value = RegEnumKey(hresult, i, subkey_name, MAX_PATH)) == ERROR_SUCCESS)
				{
					if (value = RegDeleteKey(hresult, subkey_name) != ERROR_SUCCESS)
					{
						error_count++;
						printf("\n error 改键  %s 存在子项 无法删除 \n", subkey_name);
					}
				}
				else
				{
					error_count++;
					printf("\n error \n");
				}
			}
		}
		else
		{
			error_count++;
			printf("\n error \n");
		}

		RegCloseKey(hresult);
	}
	else
	{
		error_count++;
		printf("\n error %s 是无效 or 需要管理员权限...\n", filename);
	}

	if (!error_count)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool register_info(const freg_info *info)
{
	HKEY hresult;
	long value = -1;
	if ((value = RegCreateKeyEx(info->hkey, info->filename, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hresult, NULL)) == ERROR_SUCCESS)
	{
		for (int i = 0; i < info->size_value; i++)
		{
			char *value_name = info->value[i].name[0] == '\0' ? NULL : info->value[i].name;
			if ((value = RegSetValueEx(hresult, value_name, 0, info->value[i].type, info->value[i].buf, sizeof(info->value[i].buf))) != ERROR_SUCCESS)
			{
				printf("\n error code RegSetValueEx = %i \n", value);
			}
		}

		RegCloseKey(&hresult);
		return true;
	}
	else
	{
		printf("\n error code RegCreateKeyEx = %i \n", value);
	}

	return false;
}