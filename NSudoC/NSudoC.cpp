//NSudo 3.1
//(C) 2015 NSudo Team. All rights reserved.

#include "stdafx.h"

#include <windows.h>

#include "..\\NSudoAPI\\NSudoAPI.h"

wchar_t szAppPath[260];
wchar_t szShortCutListPath[260];

void ConsolePrintW(LPWSTR Text)
{
	DWORD result;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), Text, wcslen(Text), &result, NULL);
}

void NSudoTitle()
{
	ConsolePrintW(
		L"NSudo 3.1 Debug(Build 950)\n"
		L"\xA9 2015 NSudo Team. All rights reserved.\n\n"
		L"捐赠支付宝账号: wxh32lkk@live.cn\n\n"
		L"感谢cjy__05,mhxkx,NotePad,tangmigoId,wondersnefu,xy137425740,月光光的大力支持（按照英文字母或拼音首字母排序）\n\n");
}

void NSudoCMDHelp()
{
	ConsolePrintW(
		L"格式: NSudoC [ -U ] [ -P ] [ -M ] 命令行或常用任务名\n"
		L"  -U:[ T | S | C | P | D ] 用户\n"
		L"  	T TrustedInstaller\n"
		L"  	S System\n"
		L"  	C 当前用户\n"
		L"  	P 当前进程\n"
		L"  	D 当前进程(降权)\n\n"
		L"  -P:[ E | D ] 特权\n"
		L"  	E 启用全部特权\n"
		L"  	D 禁用所有特权\n"
		L"  	PS: 如果想以默认特权方式的话，请不要包含-P参数\n\n"
		L"  -M:[ S | H | M | L ] 完整性\n"
		L"  	S 系统\n"
		L"  	H 高\n"
		L"  	M 中\n"
		L"  	L 低\n"
		L"  	PS: 如果想以默认完整性方式的话，请不要包含-M参数\n\n"
		L"  -? 显示该内容\n\n"
		L"  例子：以TrustedInstaller权限，启用所有特权，完整性默认运行命令提示符\n"
		L"		NSudoC -U:T -P:E cmd\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	NSudoTitle();

	if (!SetCurrentProcessPrivilege(SE_DEBUG_NAME, true))
	{
		ConsolePrintW(L"特权获取失败，请以管理员运行\n");
		return -1;
	}

	GetModuleFileNameW(NULL, szAppPath, 260);
	wcsrchr(szAppPath, L'\\')[0] = NULL;

	wcscpy_s(szShortCutListPath, 260, szAppPath);
	wcscat_s(szShortCutListPath, 260, L"\\ShortCutList.ini");

	bool bUserArgEnable = true;
	bool bPrivilegeArgEnable = true;
	bool bIntegrityArgEnable = true;
	bool bCMDLineArgEnable = true;

	wchar_t szBuffer[512];

	HANDLE hUserToken = INVALID_HANDLE_VALUE;

	for (int i = 1; i < argc; i++)
	{
		if (_wcsicmp(argv[i], L"-?") == 0)
		{
			NSudoCMDHelp();
			return 0;
		}		
		else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:T") == 0)
		{
			NSudoGetTrustedInstallerToken(&hUserToken);
			bUserArgEnable = false;
		}
		else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:S") == 0)
		{
			NSudoGetSystemToken(&hUserToken);
			bUserArgEnable = false;
		}
		else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:C") == 0)
		{
			NSudoGetCurrentUserToken(&hUserToken);
			bUserArgEnable = false;
		}
		else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:P") == 0)
		{
			NSudoGetCurrentProcessToken(&hUserToken);
			bUserArgEnable = false;
		}
		else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:D") == 0)
		{
			NSudoCreateLUAToken(&hUserToken);
			bUserArgEnable = false;
		}
		else if (bPrivilegeArgEnable && _wcsicmp(argv[i], L"-P:E") == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, true);
			bPrivilegeArgEnable = false;
		}
		else if (bPrivilegeArgEnable && _wcsicmp(argv[i], L"-P:D") == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, false);
			bPrivilegeArgEnable = false;
		}
		else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:S") == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-16384");
			bIntegrityArgEnable = false;
		}
		else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:H") == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-12288");
			bIntegrityArgEnable = false;
		}
		else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:M") == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-8192");
			bIntegrityArgEnable = false;
		}
		else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:L") == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-4096");
			bIntegrityArgEnable = false;
		}
		else if (bCMDLineArgEnable)
		{
			GetSystemDirectoryW(szBuffer, 512);
			wcscat_s(szBuffer, 512, L"\\cmd.exe /c start ");

			wchar_t szPath[260];
			DWORD dwLength = GetPrivateProfileStringW(argv[i], L"CommandLine", L"", szPath, 260, szShortCutListPath);

			if (wcscmp(szPath, L"") != 0)
			{
				wcscat_s(szBuffer, 512, szPath);
			}
			else
			{
				wcscat_s(szBuffer, 512, argv[i]);
			}
			bCMDLineArgEnable = false;
		}

	}

	if (bUserArgEnable || bCMDLineArgEnable)
	{
		ConsolePrintW(L"命令行参数有误，请修改(使用-?参数查看帮助)\n");
		return -1;
	}
	else
	{
		ConsolePrintW(L"正在创建进程，请稍候...\n");
		if (NSudoImpersonateSystemToken())
		{
			if (!NSudoCreateProcess(hUserToken, NULL, szBuffer))
			{
				ConsolePrintW(L"进程创建失败\n");
			}
			else
			{
				ConsolePrintW(L"进程创建成功\n");
			}
			RevertToSelf();
		}
	}

	CloseHandle(hUserToken);

	return 0;
}