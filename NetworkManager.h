#pragma once
#include "Network.h"

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	bool serverStart();
	bool serverOff();
private:
	std::thread selectServer;
	Server *serverApp;
};

extern NetworkManager *NManager;
