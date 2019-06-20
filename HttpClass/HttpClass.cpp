// HttpClass.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<iostream>
#include<WinSock2.h>
#include<fstream>
#include<istream>
#include<string>//getline
#include<cstring>
#include<fileapi.h>
#define STR "C:\\Users\\Dell\\Desktop\\HTTPServerProject\\HttpClass\\FileRepositary\\"
SOCKET TCPServerSocket;
struct sockaddr_in TCPServerAdd;
#define MAX 500
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
string SenderBuffer;
int iSend;
struct sockaddr_in TCPClientAdd;
int iTCPClientAdd = sizeof(TCPClientAdd);
HANDLE hThread;

struct socket_info
{

	sockaddr_in TCPClientAdd;
	sockaddr_in TCPServerAddr;
	int sAcceptSocket;
	int port;
};
class Logger
{
public:
	void logg_error(string ip, string msg, int stat);
	void logg(string ip, string filename, int i);
};
void Logger::logg_error(string ip, string msg, int stat)
{
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	ofstream loge("log_error.txt", ios::app);
	switch (stat)
	{
	case 1:
		loge << "IP: " << "No Connection" << " " << "Error:Startup failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 2:
		loge << "IP: " << "No Connection" << " " << "Error:Socket creation failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 3:

		loge << "IP: " << "No Connection" << " " << "Error:Binding failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 4:
		loge << "IP: " << "No Connection" << " " << "Error:Listen failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 5:
		loge << "IP: " << "No Connection" << " " << "Error:Accept failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;

	case 6:
		loge << "IP: " << "No Connection" << " " << "Error:Thread creation failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 7:
		loge << "IP: " << "No Connection" << " " << "Error:Socket closing failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	case 8:
		loge << "IP: " << "No Connection" << " " << "Error:Cleanup failed " << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;
	default:
		loge << "IP: " << ip << " " << msg << " Time: " << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		break;

	}
}
void Logger::logg(string ip, string filename, int i)
{
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	ofstream log("log.txt", ios::app);
	if (i == 0)
	{
		log << "IP:" << ip << " Time:" << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << " Server Status: Fail" << " Requested file: " << filename << endl;
	}
	else
	{
		log << "IP:" << ip << " Time:" << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << " Server Status: Success" << " Requested file: " << filename << endl;
	}
}
class Server
{
public:
	string filepath = STR;
	
	char RecvBuffer[512];
	Logger Log;
	DWORD threadid;
	int iRecv;
	int irecvBuffer = strlen(RecvBuffer) + 1;
	int err;
	int port = 8080;
	SOCKET sAcceptSocket;
	struct socket_info socket_data;

	void acceptConnection();
};
DWORD WINAPI clientThread(LPVOID lpParam)
{
	Logger log;
	string filepath = STR;
	cout << "Thread created successfully";
	char RecvBuffer[512];
	int iRecv;
	int irecvBuffer = strlen(RecvBuffer) + 1;
	//int err;
	struct socket_info* s = (socket_info*)lpParam;
	struct sockaddr_in* addr_in = (struct sockaddr_in*) & s->TCPClientAdd;
	char* ip = inet_ntoa(addr_in->sin_addr);
	cout << s->sAcceptSocket << endl;
	//step8 receiving data from client
	while (1)
	{
		cout << "thread id" << GetCurrentThreadId() << endl;

		iRecv = recv(s->sAcceptSocket, RecvBuffer, irecvBuffer, 0);
		irecvBuffer = strlen(RecvBuffer) + 1;

		if (iRecv <= 0)
		{
			cout << "DATA receiving failed. ERROR -> " << WSAGetLastError() << endl;
			CloseHandle(GetCurrentThread());
			log.logg_error(ip, "Error: Data Receive Failed", 0);
			cout << "thread closed" << endl;
			break;
		}
		cout << "DATA RECEIVED  " << RecvBuffer << endl;

		//getting the filename from the client request
		char filename[20];
		int i, j, c = 0, flag = 0;
		for (i = 0; i < irecvBuffer; i++)
		{
			if (RecvBuffer[i] == '/')
			{
				flag = 1;
				for (j = i + 1; RecvBuffer[j] != ' '; j++)
				{
					filename[c++] = RecvBuffer[j];
				}


			}
			if (flag == 1)
				break;
		}
		filename[c] = '\0';

		if (strlen(filename) == 0)  //if url doesnot contain file then default file is index.html
		{
			strcpy(filename, "index.html");
		}

		filepath.append(filename);
		cout << endl << "-------" << filename << "------" << endl;

		WIN32_FIND_DATAA FindFileData;
		HANDLE hFile = FindFirstFileA(filepath.c_str(), &FindFileData); //finding the file in directory
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SenderBuffer = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/plain\nContent-Length:25 \n\nError 404.File Not Found.";
			log.logg_error(ip, "Error: File Handle Error", 0);
		}

		else
		{
			//reading the content from the file

			ifstream infile(filepath);
			string s;
			string temp;
			while (!infile.eof())
			{
				getline(infile, temp);
				s = s + temp;
			}

			cout << "----------CONTENT OF FILE IS---------" << endl;
			cout << endl << s << endl << endl;


			std::string size = std::to_string(s.length());
			SenderBuffer = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
			SenderBuffer.append(size);
			SenderBuffer.append("\n\n");
			SenderBuffer.append(s);


		}//else

		int iSenderBuffer = strlen(SenderBuffer.c_str()) + 1;


		//step7 send data to client
		iSend = send(s->sAcceptSocket, SenderBuffer.c_str(), iSenderBuffer, 0);
		if (iSend == SOCKET_ERROR)
		{
			log.logg(ip, filename, 0);
			log.logg_error(ip, "Error: Data Sending Failed", 0);
			cout << "sending failed. ERROR -> " << WSAGetLastError() << endl;
			exit(0);
		}

		cout << "Data sending success " << SenderBuffer << endl;
		log.logg(ip, filename, 1);
		FindClose(hFile);
		if (s->sAcceptSocket == INVALID_SOCKET)
		{
			cout << "Socket closded" << endl;
			log.logg_error(ip, "Error: Socket Closed", 0);
			closesocket(s->sAcceptSocket);
			CloseHandle(GetCurrentThread());

		}
	}
}
class Socket
{
public:
	WSADATA winSockData;
	int iWsaStartup;
	int iWsaCleanup;
	int iCloseSocket;

	int iBind;
	int iListen;
	Logger SockLog;
	int port = 8080;
	
	Socket();
	void createSocket();
	void bindSocket();
	void startListen();
	~Socket();
};
Socket::Socket()
{
	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &winSockData);
	if (iWsaStartup != 0)
	{
		cout << "WSaStartup failed " << endl;
		SockLog.logg_error("", "", 1);
		exit(0);
	}
	cout << "WSaStartup Success " << endl;

	//step2 Fillthe structure
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
	TCPServerAdd.sin_port = htons(port);
}
void Socket::createSocket()
{
	TCPServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPServerSocket == INVALID_SOCKET)
	{
		cout << "TCP serversocket creation failed " << WSAGetLastError() << endl;
		SockLog.logg_error("", "", 2);
		exit(0);
	}
	cout << "TCP serversocket creation success " << endl;
}
void Socket::bindSocket()
{
	iBind = bind(TCPServerSocket, (SOCKADDR*)& TCPServerAdd, sizeof(TCPServerAdd));
	if (iBind == SOCKET_ERROR)
	{
		cout << "Binding failed. ERROR -> " << WSAGetLastError() << endl;
		SockLog.logg_error("", "", 3);
		exit(0);
	}
	cout << "Binding success " << endl;
}
void Socket::startListen()
{
	iListen = listen(TCPServerSocket, 2);
	if (iListen == SOCKET_ERROR)
	{
		cout << "listen function failed. ERROR -> " << WSAGetLastError() << endl;
		SockLog.logg_error("", "", 4);
		exit(0);
	}
	cout << "listen function success " << endl;
}
void Server::acceptConnection()
{
	Server server;
	sAcceptSocket = accept(TCPServerSocket, (SOCKADDR*)& TCPClientAdd, &iTCPClientAdd);
	if (sAcceptSocket == INVALID_SOCKET)
	{
		cout << "Accept function failed. ERROR -> " << WSAGetLastError() << endl;
		Log.logg_error("", "", 5);
		exit(0);
	}
	cout << "Accept function success " << endl;
	cout << sAcceptSocket << endl;
	socket_data.port = port;
	socket_data.sAcceptSocket = sAcceptSocket;
	socket_data.TCPClientAdd = TCPClientAdd;
	socket_data.TCPServerAddr = TCPServerAdd;
	hThread = CreateThread(NULL, 0, clientThread, (LPVOID)&socket_data, 0, &threadid);
	if (hThread == NULL)
	{
		Log.logg_error("", "", 6);
	}

}
Socket::~Socket()
{
	iCloseSocket = closesocket(TCPServerSocket);
	if (iCloseSocket == SOCKET_ERROR)
	{
		cout << "closing socket failed. ERROR -> " << WSAGetLastError() << endl;
		SockLog.logg_error("", "", 7);
		exit(0);
	}
	cout << "closing socket  success " << endl;

	//step10 cleanup
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		cout << "clean up failed. ERROR -> " << WSAGetLastError() << endl;
		SockLog.logg_error("", "", 8);
		exit(0);
	}
	cout << "cleanup success " << endl;

}
int main()
{
	Socket sock;
	sock.createSocket();
	Server server;
	sock.bindSocket();
	sock.startListen();
	while (1)
	{
		server.acceptConnection();
	}

	
}
