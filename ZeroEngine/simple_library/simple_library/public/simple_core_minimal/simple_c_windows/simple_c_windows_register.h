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
#include "../simple_c_core/simple_core_minimal.h"

//运行批处理 以管理员启动
//mshta vbscript:CreateObject("Shell.Application").ShellExecute("%~dp0git_install.exe","1 F:\c\c\20\20.exe F:\c\c\git_install\git_install\icon1.ico test_git1","","runas",1)(window.close)&&exit
//下面是案例
/*
#define REG_SHELL_DIRECTORY					"*\\Shell\\"						//在对象上鼠标右键
#define REG_DIRECTORY						"Directory\\Background\\shell\\"	//在空白的地区点击右键 添加按钮
#define REG_DESKTOPBACKGROUND				"DesktopBackground\\shell\\"		//在空白的地区点击右键 添加按钮

#define REG_PATH_TEST REG_DIRECTORY
int main(int argc, char *argv[])
{
	const char *commit_type = argv[1];
	const char *path_exe = argv[2];
	const char *path_icon = argv[3];
	const char *name = argv[4];

	char buf_reg_key[MAX_PATH] = { 0 };
	char buf_reg_sub_key[MAX_PATH] = { 0 };
	strcpy(buf_reg_key, REG_PATH_TEST);
	strcat(buf_reg_key, name);

	strcpy(buf_reg_sub_key, REG_PATH_TEST);
	strcat(buf_reg_sub_key, name);
	strcat(buf_reg_sub_key, "\\command");

	if (strcmp(commit_type ,"1") == 0) //安装
	{
		//key
		freg_info info;
		init_feg_info(&info);

		strcpy(info.filename, buf_reg_key);
		info.hkey = HKEY_CLASSES_ROOT;
		{
			info.value[info.size_value].type = REG_SZ;
			strcpy(info.value[info.size_value++].buf, name);

			info.value[info.size_value].type = REG_SZ;
			strcpy(info.value[info.size_value].name, "icon");
			strcpy(info.value[info.size_value++].buf, path_icon);
		}

		if (!register_info(&info))
		{
			printf("\n 1 需要管理员权限~~ \n");
			system("pause");

			return 1;
		}

		//sub key
		init_feg_info(&info);
		strcpy(info.filename, buf_reg_sub_key);
		info.hkey = HKEY_CLASSES_ROOT;
		{
			info.value[info.size_value].type = REG_SZ;
			strcpy(info.value[info.size_value++].buf, path_exe);
		}

		if (!register_info(&info))
		{
			printf("\n 2 需要管理员权限~~ \n");
			system("pause");

			return 1;
		}
	}
	else if (strcmp(commit_type, "2") == 0) //卸载
	{
		if (delete_register_info(HKEY_CLASSES_ROOT, buf_reg_key))
		{
			delete_reg_key(HKEY_CLASSES_ROOT, REG_PATH_TEST, name);
		}
	}

	return 0;
}
*/

_CRT_BEGIN_C_HEADER
typedef struct
{
	char name[MAX_PATH];
	DWORD type;
	char buf[1024];
}freg_value;

typedef struct
{
	HKEY hkey;
	char filename[MAX_PATH];

	int size_value;
	freg_value value[1024];
}freg_info;

void init_feg_info(freg_info *info);

//注册 键信息
bool register_info(const freg_info *info);

//子项不运行存在子项 否则删除失败
bool delete_register_info(HKEY hkey, const char *filename);

//子项不运行存在子项 否则删除失败
bool delete_reg_key(HKEY hkey, const char *filename, const char *subkey_name);
_CRT_END_C_HEADER