#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include "TreeData.h"

#define DEFAULT_PORT 27000
#define DEFAULT_IP "127.0.0.1"

class Client;

enum class COMMAND
{
	DOWNLOAD, UPLOAD, RE_NAME, DELETE_ITEM, UNKNOWN, CREATE_ITEM
};

CString comToString(COMMAND com);
COMMAND strToCommand(CString *str);

struct Task
{
	COMMAND com;
	Data *data;
};

struct Task_Client
{
	Client *client;
	Task task;
};

class Client
{
public:
	Client();
	Client(SOCKET s) :client(s) {}
	~Client();



	SOCKET getSocket();

	bool startClient();
	bool clouseClient();

	bool recvStringClient(CString &str);
	bool sendStringClient(const CString *buf);
/*
	bool sendData(const Task *task, SOCKET s);
	bool recvData(const Task *task, SOCKET s);
	*/
		bool sendData(const Task *task);
		bool recvData(const Task *task);
	
protected:
	WSAData wsaData;
	bool initSturtupAndSocket();
	int id;//server always id == 0
	/*is possibility be master or slave*/
	SOCKET client{ INVALID_SOCKET };
	//ip & port
	//ip
	//port now used default
	timeval timeoutSelect{ 5,0 };
	bool workSelect;
private:
	bool initSockAddrAndConnect();
	void startSelect();
	sockaddr_in conInfo;
};