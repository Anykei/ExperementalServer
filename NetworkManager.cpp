#include "stdafx.h"
#include "NetworkManager.h"

NetworkManager *NManager = new NetworkManager();

void selectThreadServer(Server *s);

NetworkManager::NetworkManager()
{
	serverApp = new Server();
}

NetworkManager::~NetworkManager()
{
	delete serverApp;
}
bool NetworkManager::serverStart()
{
	if (!selectServer.joinable())
	{
		selectServer = std::thread(selectThreadServer, serverApp);
		selectServer.detach();
	}
	return true;
}

bool NetworkManager::serverOff()
{
	return serverApp->clouseServer();
	return false;
}

void selectThreadServer(Server *s)
{
	s->startServer();
}

