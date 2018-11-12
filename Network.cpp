#include "stdafx.h"
#include "Network.h"

Server::Server()
{
	NTManager = new NetworkTaskManager();
}
Server::~Server()
{
	delete NTManager;
}

bool Server::startServer()
{
	if (initSturtupAndSocket() && initAddrInfoAndBind())
	{
		startSelect();
		NTManager->start();
	}
	return false;
}
bool Server::clouseServer()
{
	return Client::clouseClient();
}

bool Server::initAddrInfoAndBind()
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, (PCSTR)DEFAULT_PORT, &hints, &result) != 0) return false;
	
	if (::bind(client, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
	{
		return false;
	}
	freeaddrinfo(result);
	listen(client, SOMAXCONN);
	return true;
}

void Server::addClient(Client *client)
{
	std::lock_guard<std::mutex> lock(lock_client_list);
	clientConnectList.push_back(client);
}

void Server::startSelect()
{
	workSelect = true;
	fd_set openSocket;
	while (workSelect)
	{
		FD_ZERO(&openSocket);
		FD_SET(client, &openSocket);

		NTManager->return_client_list(&clientConnectList);

		for (Client *c : clientConnectList)
		{
			FD_SET(c->getSocket(), &openSocket);
		}

		int result = select(0, &openSocket, NULL, NULL, &timeoutSelect);
		if (result == SOCKET_ERROR) return;
		if (0 <= result)
		{
			if (FD_ISSET(client, &openSocket))
			{

				SOCKET s = accept(client, NULL, NULL);
				if (s != SOCKET_ERROR)
				{
					addClient(new Client(s));
				}
			}

			for (int i = 0; i < clientConnectList.size(); i++)
			{
				if (FD_ISSET(clientConnectList[i]->getSocket(), &openSocket))
				{
					CString buf;
					if (clientConnectList[i]->recvStringClient(buf))
					{
						NTManager->addTask(&buf, clientConnectList[i]);
						clientConnectList.erase(clientConnectList.begin() + i);
					}
					else
					{
						clientConnectList[i]->clouseClient();
						clientConnectList.erase(clientConnectList.begin() + i);
						//error recv buf
					}
				}
			}
		}

	}
}
