// RSimscreen.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include<time.h>
#include <atlconv.h>
#include "pch.h"
#include <iostream>
#include "VirtualDesktops.h"
#include<windows.h>
typedef char    _TCHAR;
SimScreen IScreen[100]; //屏幕结构
int Pat_Right();
int Pat_Left();
void PrintGuid(const GUID &guid)
{
	std::wstring guidStr(40, L'\0');
	::StringFromGUID2(guid, const_cast<LPOLESTR>(guidStr.c_str()), guidStr.length());

	std::wcout << guidStr.c_str();
}

HRESULT EnumVirtualDesktops(IVirtualDesktopManagerInternal *pDesktopManager)
{
	std::wcout << L"<<< EnumDesktops >>>" << std::endl;

	IObjectArray *pObjectArray = nullptr;
	HRESULT hr = pDesktopManager->GetDesktops(&pObjectArray);

	if (SUCCEEDED(hr))
	{
		UINT count;
		hr = pObjectArray->GetCount(&count);

		if (SUCCEEDED(hr))
		{
			std::wcout << L"Count: " << count << std::endl;

			for (UINT i = 0; i < count; i++)
			{
				IVirtualDesktop *pDesktop = nullptr;

				if (FAILED(pObjectArray->GetAt(i, __uuidof(IVirtualDesktop), (void**)&pDesktop)))
					continue;

				GUID id = { 0 };
				if (SUCCEEDED(pDesktop->GetID(&id)))
				{
					std::wcout << L"\t #" << i << L": ";
					PrintGuid(id);
					std::wcout << std::endl;
				}

				pDesktop->Release();
			}
		}

		pObjectArray->Release();
	}

	std::wcout << std::endl;
	return hr;
}

HRESULT GetCurrentVirtualDesktop(IVirtualDesktopManagerInternal *pDesktopManager)
{
	std::wcout << L"<<< GetCurrentVirtualDesktop >>>" << std::endl;

	IVirtualDesktop *pDesktop = nullptr;
	HRESULT hr = pDesktopManager->GetCurrentDesktop(&pDesktop);

	if (SUCCEEDED(hr))
	{
		GUID id = { 0 };

		if (SUCCEEDED(pDesktop->GetID(&id)))
		{
			std::wcout << L"Current desktop id: ";
			PrintGuid(id);
			std::wcout << std::endl;
		}

		pDesktop->Release();
	}

	std::wcout << std::endl;
	return hr;
}

HRESULT EnumAdjacentDesktops(IVirtualDesktopManagerInternal *pDesktopManager)
{
	std::wcout << L"<<< EnumAdjacentDesktops >>>" << std::endl;

	IVirtualDesktop *pDesktop = nullptr;
	HRESULT hr = pDesktopManager->GetCurrentDesktop(&pDesktop);

	if (SUCCEEDED(hr))
	{
		GUID id = { 0 };
		IVirtualDesktop *pAdjacentDesktop = nullptr;
		hr = pDesktopManager->GetAdjacentDesktop(pDesktop, AdjacentDesktop::LeftDirection, &pAdjacentDesktop);

		std::wcout << L"At left direction: ";

		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(pAdjacentDesktop->GetID(&id)))
				PrintGuid(id);

			pAdjacentDesktop->Release();
		}
		else
			std::wcout << L"NULL";
		std::wcout << std::endl;

		id = { 0 };
		pAdjacentDesktop = nullptr;
		hr = pDesktopManager->GetAdjacentDesktop(pDesktop, AdjacentDesktop::RightDirection, &pAdjacentDesktop);

		std::wcout << L"At right direction: ";

		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(pAdjacentDesktop->GetID(&id)))
				PrintGuid(id);

			pAdjacentDesktop->Release();
		}
		else
			std::wcout << L"NULL";
		std::wcout << std::endl;

		pDesktop->Release();
	}

	std::wcout << std::endl;
	return hr;
}

HRESULT ManageVirtualDesktops(IVirtualDesktopManagerInternal *pDesktopManager)
{
	std::wcout << L"<<< ManageVirtualDesktops >>>" << std::endl;
	std::wcout << L"Sleep period: 2000 ms" << std::endl;

	::Sleep(2000);


	IVirtualDesktop *pDesktop = nullptr;
	HRESULT hr = pDesktopManager->GetCurrentDesktop(&pDesktop);

	if (FAILED(hr))
	{
		std::wcout << L"\tFAILED can't get current desktop" << std::endl;
		return hr;
	}

	std::wcout << L"Creating desktop..." << std::endl;

	IVirtualDesktop *pNewDesktop = nullptr;
	hr = pDesktopManager->CreateDesktopW(&pNewDesktop);

	if (SUCCEEDED(hr))
	{
		GUID id;
		hr = pNewDesktop->GetID(&id);

		if (FAILED(hr))
		{
			std::wcout << L"\tFAILED GetID" << std::endl;
			pNewDesktop->Release();
			return hr;
		}

		std::wcout << L"\t";
		PrintGuid(id);
		std::wcout << std::endl;

		std::wcout << L"Switching to desktop..." << std::endl;
		hr = pDesktopManager->SwitchDesktop(pNewDesktop);

		if (FAILED(hr))
		{
			std::wcout << L"\tFAILED SwitchDesktop" << std::endl;
			pNewDesktop->Release();
			return hr;
		}

		::Sleep(2000);

		std::wcout << L"Removing desktop..." << std::endl;

		if (SUCCEEDED(hr))
		{
			hr = pDesktopManager->RemoveDesktop(pNewDesktop, pDesktop);
			pDesktop->Release();

			if (FAILED(hr))
			{
				std::wcout << L"\tFAILED RemoveDesktop" << std::endl;
				pNewDesktop->Release();
				return hr;
			}
		}
	}

	std::wcout << std::endl;
	return hr;
}

/*int _tmain(int argc, _TCHAR* argv[])
int main()
{
	::CoInitialize(NULL);

	IServiceProvider* pServiceProvider = nullptr;
	HRESULT hr = ::CoCreateInstance(
		CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER,
		__uuidof(IServiceProvider), (PVOID*)&pServiceProvider);

	if (SUCCEEDED(hr))
	{
		IVirtualDesktopManagerInternal* pDesktopManagerInternal = nullptr;
		hr = pServiceProvider->QueryService(CLSID_VirtualDesktopAPI_Unknown, &pDesktopManagerInternal);

		if (SUCCEEDED(hr))
		{
			EnumVirtualDesktops(pDesktopManagerInternal);
			GetCurrentVirtualDesktop(pDesktopManagerInternal);
			EnumAdjacentDesktops(pDesktopManagerInternal);
			ManageVirtualDesktops(pDesktopManagerInternal);
			pDesktopManagerInternal->Release();
			pDesktopManagerInternal = nullptr;
		}

		std::wcout << L"<<< Desktop for current window >>>" << std::endl;

		IVirtualDesktopManager *pDesktopManager = nullptr;
		hr = pServiceProvider->QueryService(__uuidof(IVirtualDesktopManager), &pDesktopManager);
		

		if (SUCCEEDED(hr))
		{
			GUID desktopId = { 0 };
			HWND test = FindWindow(NULL, (LPCWSTR)"Visual Studio Code");
			hr = pDesktopManager->GetWindowDesktopId(test, &desktopId);
			if (SUCCEEDED(hr))
			{



				std::wcout << L"\t";
				PrintGuid(desktopId);
				std::wcout << std::endl;
			}
//			if(SUCCEEDED(hr))
			{ 
				HWND test= GetConsoleWindow();
				//hr = pDesktopManager->MoveWindowToDesktop(test, { 0x65F0E669,0xE4D7,0x4FC3,0xA9,0x08,0x3B,0x6D,0x1D,0xD3,0x1E,0x35 }); desktop - 3
				hr = pDesktopManager->MoveWindowToDesktop(test, { 0x45373737,0xD443,0x47E8,0xAD,0x97,0xC0,0x3E,0x2D,0x41,0xBA,0x69 });
				//hr = pDesktopManager->MoveWindowToDesktop(test, desktopId);
				std::cout << hr <<test <<std::endl;
//			}   这里是测试移动屏幕的部分已经失效
			
			pDesktopManager->Release();
			pDesktopManager = nullptr;
			std::wcout << std::endl;
		}
		

		pServiceProvider->Release();
	}
	return 0;
}
*/

GUID Bone(HWND handler)
{
	::CoInitialize(NULL);

	IServiceProvider* pServiceProvider = nullptr;
	HRESULT hr = ::CoCreateInstance(
		CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER,
		__uuidof(IServiceProvider), (PVOID*)&pServiceProvider);
	GUID desktopId = { 0 };
	if (SUCCEEDED(hr))
	{
		IVirtualDesktopManagerInternal* pDesktopManagerInternal = nullptr;
		hr = pServiceProvider->QueryService(CLSID_VirtualDesktopAPI_Unknown, &pDesktopManagerInternal);

		IVirtualDesktopManager *pDesktopManager = nullptr;
		hr = pServiceProvider->QueryService(__uuidof(IVirtualDesktopManager), &pDesktopManager);

		if (SUCCEEDED(hr))
		{
			hr = pDesktopManager->GetWindowDesktopId(handler, &desktopId);
		}

		pServiceProvider->Release();
	}

	return desktopId;
}
int DOG()
{
	int windowsNumber;
	for (int i = 0; i < 30; i++)
	{
		Sleep(100);
		Pat_Left();
	}//将屏幕回滚至最左的屏幕
	for (windowsNumber = 0; windowsNumber <30; windowsNumber++)
	{
		Sleep(300);
		LPWSTR szTitle[100];
		IScreen[windowsNumber].uid= Bone(GetForegroundWindow());
		GetWindowText(GetForegroundWindow(), *szTitle, 1000);
//		IScreen[windowsNumber].name = *szTitle;
		if(windowsNumber !=0)
		if ((IScreen[windowsNumber].uid == IScreen[windowsNumber - 1].uid))
			break;
		IScreen[windowsNumber].order = windowsNumber;
//		PrintGuid(IScreen[windowsNumber].uid);
		Pat_Right();
	}//自学习窗口数量并学习窗口UID
	for (int i = 0; i < 30; i++)
	{
		Sleep(100);
		Pat_Left();
	}//将屏幕回滚至最左的屏幕
	std::cout << "{My dog!Information}:学习结束，共有" << windowsNumber << "个窗口" << std::endl;
	return windowsNumber-1;
}
int Pat_Right()
{
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_LWIN, 0, 0, 0);
	keybd_event(VK_RIGHT, 0, 0, 0);


	keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	return 0;
}
int Pat_Left()
{
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_LWIN, 0, 0, 0);
	keybd_event(VK_LEFT, 0, 0, 0);

	keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	return 0;
}
int Pat_hit()
{
	keybd_event(VK_LBUTTON, 0, 0, 0);
	keybd_event(VK_LBUTTON, 0, 0, 0);
	keybd_event(VK_LBUTTON, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_LBUTTON, 0, KEYEVENTF_KEYUP, 0);
	return 0;
}
void Walking_the_dog(int windowsnumber,int walktime,int Again)
{
	std::cout << "[My dog!Information]:"<<windowsnumber+1<<"窗体轮巡开始" << walktime << "秒轮巡一次" << std::endl;
	if (Again != 0)
	for (int J = 0; J < Again; J++)		
	{
		for (int i = 0; i < windowsnumber; i++)
		{
			Sleep(walktime * 1000);
			Pat_Right();
		}
		for (int i = 0; i < windowsnumber; i++)
		{
			Sleep(walktime * 1000);
			Pat_Left();
		}
	}
	else
	{
		while (true)
		{
			for (int i = 0; i < windowsnumber; i++)
			{
				Sleep(walktime * 1000);
				Pat_Right();
			}
			for (int i = 0; i < windowsnumber; i++)
			{
				Sleep(walktime * 1000);
				Pat_Left();
			}
		}
	}
		
}
int main()
{
	int walktime;
	int Again;
	std::cout << "[My dog!Information]:欢迎使用我的狗（My DOG）桌面级SHELL处理框架（基于Windows SHELL + COM 编制）" << std::endl;
	std::cout << "[My dog!Information]:进入轮询模式" << std::endl;
	std::cout << "[My dog!Information]:请输入轮询的时间，单位为秒"  << std::endl;
	std::cin >> walktime;
	std::cout << "[My dog!Information]:请输入轮询的次数，无限轮询请输入0" << std::endl;
	std::cin >> Again;
	Walking_the_dog(DOG(), walktime,Again);
	return 0;
}