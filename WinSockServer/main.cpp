using namespace std;
#define _CRT_SECURE_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif // !WIN32_LEAN_AND_MEAN


#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<iostream>

#pragma comment(lib,"WS2_32.lib")

#define DEFAULT_PORT			"27015"

#define DEFAULT_BUFFER_LENGTH	1500
void main()
{
	setlocale(LC_ALL, "");
	//1)������������� WinSock:
	WSAData wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "Eror: WSAstartup failed:" << iResult << endl;
		return;
	}

	//2)������� ����� �������:
	//2)��������� �� ����� �� ����, �� ������� �� ���� ��������� ���� ������
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* result = NULL;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		cout << "Error: getaddrinfo failed:" << iResult << endl;
		return;
	}

	cout << hints.ai_addr << endl;

	//3) ������� �����, ������� ����� ������������ ������:
	SOCKET ListenSocket =
		socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Error:Socket creation failed:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//4) ��������� ����� � ������� ������, ������� �� ����� ������������:
	//strcpy(result->ai_addr->sa_data, "127.0.0.1");
	iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Error: bind failed with code" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//5) ��������� �����:
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Error: Lisyen failed with code" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//6)����������� ����� �� ��������� ���������� �� ��������:
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};
	int recv_buffer_length = DEFAULT_BUFFER_LENGTH;
	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
	do
	{
		iResult = recv(ClientSocket,recvbuffer, recv_buffer_length,0);
		if (iResult > 0)
		{
			cout << "Bytes received:" << iResult << endl;
			cout << "Message:" << recvbuffer << endl;
			CHAR sz_responce[]("Hello I am server");
			INT iSendResult = send(ClientSocket, sz_responce, sizeof (sz_responce), 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "Error: Send failed with code:" << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				closesocket(ListenSocket);
				freeaddrinfo(result);
				WSACleanup();
				return;
			}
			cout << "Bytes sent:" << iSendResult << endl;
		}
		else if (iResult == 0)
		{
			cout << "Connection closing" << endl;
		}
		else
		{
			cout << "Error: recv() failed with code" << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			closesocket(ListenSocket);
			freeaddrinfo(result);
			WSACleanup();
			return;
		}
	} while (iResult > 0);
}