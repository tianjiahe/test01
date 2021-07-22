/* 
   逻辑磁盘路径表示中加入转译字符： "\\\\.\\E:"，实际为：\\.\E, 如： \\计算机名称\E,可访问网络,
   物理磁盘：“\\.\PhysicalDrive0”，打开了磁盘设备，实测不能返回C盘句柄，size 必须是512的倍数,
   https://blog.csdn.net/zuishikonghuan/article/details/46926787
   https://blog.csdn.net/zuishikonghuan/article/details/50380313
*/


#include "pch.h"
#include "framework.h"

#include<Windows.h>
#include "test03.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define PHYSICALDRIVE  "\\\\.\\E:"                  // 访问逻辑盘
#define PHYSICALDRIVE  "\\\\.\\PhysicalDrive1"        // 0基序列访问物理盘，注意：生成的可执行文件要用管理员权限执行


//参数：输出的字符串指针，开始位置，长度
//返回值：读取的大小
DWORD ReadDisk(unsigned char* &out, DWORD start, DWORD size)
{
	OVERLAPPED over = { 0 };
	over.Offset = start;

	HANDLE handle = CreateFile(TEXT(PHYSICALDRIVE), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);  //ok
		
	if (handle == INVALID_HANDLE_VALUE)  return 0;

	unsigned char* buffer = new unsigned char[size + 1];
	DWORD readsize;
	if (ReadFile(handle, buffer, size, &readsize, &over) == 0)
	{
		CloseHandle(handle);
		return 0;
	}
	buffer[size] = 0;
	out = buffer;
	//delete [] buffer;
	//注意这里需要自己释放内存
	CloseHandle(handle);
	return size;
}




//参数：文件名，欲写入的字符串
//返回值：写入的大小
DWORD WriteDisk(unsigned char * inbuf, DWORD start, DWORD size)
{
	OVERLAPPED over = { 0 };
	over.Offset = start;

	HANDLE handle  = CreateFile(TEXT(PHYSICALDRIVE), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		printf("create file error !\n");
		return 0;
	}
	DWORD writeSize = WriteFile(handle, inbuf, size, &writeSize, &over);

	if ( writeSize == 0) {
		
		CloseHandle(handle);
		printf("write file error !\n");
		return 0;
	}
	CloseHandle(handle);
	return writeSize;
}

// 唯一的应用程序对象
CWinApp theApp;
using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
			//theApp.Run();
			unsigned char* a;
			DWORD len = ReadDisk(a, 0, 512);
			if (len) {
				//int i = 0;
				for ( int i = 0; i < 32; i++) {
					for (int j = 0; j < 16; j++) {
						printf("%02X ", a[i*16+j]);
					}
					printf("\n");
				}
			}
			getchar();

			len = WriteDisk( a, 0xc00, 512);
			if(len) 
				printf(" write ok! \n");
			getchar();
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
