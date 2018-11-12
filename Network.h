#pragma once
//#include"NetworkClient.h"
#include "NetworkTaskManager.h"

class Server : Client
{
public:
	Server();
	~Server();

	bool startServer();
	void addClient(Client *client);
	bool clouseServer();

private:
	std::mutex lock_client_list;
	addrinfo hints, *result;
	bool initAddrInfoAndBind();
	void startSelect();
	std::vector<Client*> clientConnectList;
	NetworkTaskManager *NTManager;
};

