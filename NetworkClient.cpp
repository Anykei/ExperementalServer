#include "stdafx.h"
#include "NetworkClient.h"
#include <string>
#include <vector>

struct Extension_file_info
{
	int all_size;
	int pacage_size;
	int count_package;
	int file_version;
};

CString ex_file_info_to_csv(const Extension_file_info *efi);
void csv_to_ex_file_info(Extension_file_info &efi, CString *content);
CString cut_firt_to_delim(CString *sours, const char *delim);

int find_buf_size(int size);

Client::Client() {}
Client::~Client() {}

SOCKET Client::getSocket()
{
	return client;

}

bool Client::initSturtupAndSocket()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		WSAGetLastError(); //chek error
		return false;
	}
	if (client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET)
		return false;
	return true;
}

bool Client::initSockAddrAndConnect()
{
	ZeroMemory(&conInfo, sizeof(conInfo));
	conInfo.sin_family = AF_INET;
	conInfo.sin_port = htons(DEFAULT_PORT);
	inet_pton(AF_INET, DEFAULT_IP, &conInfo.sin_addr);

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET)return false;

	if (SOCKET_ERROR == connect(client, (SOCKADDR*)&conInfo, sizeof(conInfo)))
	{
		//log
		return false;
	}
	else {

		//good log
		return true;
	}
}

void Client::startSelect()
{

}

bool Client::startClient()
{
	if (initSturtupAndSocket() && initSockAddrAndConnect())
		startSelect();
	else return false;
	return true;
}

bool Client::clouseClient()
{
	workSelect = false;
	if (shutdown(client, 2))
	{
		closesocket(client);
		WSACleanup();
		return false;
	}
	return true;
}

bool Client::recvStringClient(CString &str)
{
	int size;
	int	iResult = recv(client, (char*)&size, sizeof(int), 0);
	if (iResult <= 0) return false;
	char *buf = new char[sizeof(char) * size];
	iResult = 0;
	while (iResult < size)
	{
		int res = recv(client, buf, size, 0);
		if (res < 0) return false;
		iResult += res;
	}
	str += buf;
	delete[] buf;
	return true;
}

bool Client::sendStringClient(const CString *buf)
{
	CT2CA pszConvertedAnsiString(*buf);
	std::string b(pszConvertedAnsiString);
	int strlenData = b.size() + 1;

	if (INVALID_SOCKET == send(client, (char*)&strlenData, sizeof(int), 0)) return false;
	if (INVALID_SOCKET == send(client, b.data(), strlenData, 0)) return false;
	return true;
}

bool Client::sendData(const Task *task)
{

	
	CString pathFile = task->data->getPath() + task->data->getName();

	CFile file;
	if (!file.Open(pathFile, CFile::modeRead))return false;

	CString answer = L"ready";

	int sizeBuf = find_buf_size(file.GetLength());

	Extension_file_info fInfo;
	fInfo.all_size = file.GetLength();
	fInfo.count_package = fInfo.all_size / sizeBuf;
	fInfo.pacage_size = sizeBuf;
	fInfo.file_version = 0;

	CString exFileInfo = ex_file_info_to_csv(&fInfo);

	if (!sendStringClient(&exFileInfo)) return false;

	do
	{
		answer.Delete(0,answer.GetLength());
		if (!recvStringClient(answer)) return false;
	} while (answer != "ready");

	int posx = 0;
	for (int i = 0; i < fInfo.count_package; i++)
	{
		std::vector<char> buf(fInfo.pacage_size);
		file.Read(buf.data(), fInfo.pacage_size);
		int send_val = 0;
		while (send_val < sizeBuf)
		{
			if (SOCKET_ERROR == (send_val += (send(client, buf.data(), sizeBuf, 0)))) return false;
			posx += sizeBuf;
			do
			{
				answer.Delete(0, answer.GetLength());
				if (!recvStringClient(answer)) return false;
			} while (answer != "ready");
		}
	}
	return true;
}

bool Client::recvData(const Task *task)
{
	Extension_file_info eFileInfo;
	CString exFileInfo;
	CString answer = L"ready";
	if (!sendStringClient(&answer)) return false;
	if (!recvStringClient(exFileInfo)) return false;

	CString data = task->data->getStrData();

	csv_to_ex_file_info(eFileInfo, &exFileInfo);

	if (!sendStringClient(&answer)) return false;

	CString filesPath = task->data->getPath() + task->data->getName();

	CFile file;
	if(file.Open(filesPath,CFile::modeCreate|CFile::modeReadWrite));

	for (int i = 0; i < eFileInfo.count_package; i++)
	{
		std::vector<char> packet(eFileInfo.pacage_size);
		int recv_count = 0;
		while (recv_count < eFileInfo.pacage_size)
		{
			int res = recv(client, &packet[0], packet.size(), 0);
			if (res < 0) return false;
			recv_count += res;
		}
		file.Write(packet.data(), packet.size());
		if (!sendStringClient(&answer))return false;
	}
	return true;
}


CString comToString(COMMAND com)
{
	switch (com)
	{
	case COMMAND::UPLOAD:
		return L"upload";
		break;
	case COMMAND::DOWNLOAD:
		return L"download";
		break;
	case COMMAND::RE_NAME:
		return L"re_name";
		break;
	case COMMAND::DELETE_ITEM:
		return L"delete_item";
		break;
	case COMMAND::CREATE_ITEM:
		return L"create_item";
		break;
	case COMMAND::UNKNOWN:
	default:
		return L"";
		break;
	}
}
COMMAND strToCommand(CString *str)
{
	if (*str == "upload")return COMMAND::UPLOAD;
	if (*str == "download")return COMMAND::DOWNLOAD;
	if (*str == "re_name")return COMMAND::RE_NAME;
	if (*str == "create_item")return COMMAND::CREATE_ITEM;
	if (*str == "delete_item")return COMMAND::DELETE_ITEM;
	else return COMMAND::UNKNOWN;
}

CString ex_file_info_to_csv(const Extension_file_info *efi)
{
	CString buf,allSize,countP,file_v,pSize;
	allSize.Format(L"%d", efi->all_size);
	countP.Format(L"%d", efi->count_package);
	file_v.Format(L"%d", efi->file_version);
	pSize.Format(L"%d", efi->pacage_size);
	buf = allSize + ";" + countP + ";" + file_v + ";" + pSize;
	return  buf;
}
void csv_to_ex_file_info(Extension_file_info &efi, CString *content)
{
	const std::string delim{ ';' };
	efi.all_size = _wtoi(cut_firt_to_delim(content, delim.c_str()));
	efi.count_package = _wtoi(cut_firt_to_delim(content, delim.c_str()));
	efi.file_version = _wtoi(cut_firt_to_delim(content, delim.c_str()));
	efi.pacage_size = _wtoi(cut_firt_to_delim(content, delim.c_str()));
}
CString cut_firt_to_delim(CString *sours, const char *delim)
{
	CString buf;
	int pos = 0;
	pos = sours->Find((LPWSTR)delim);
	buf = sours->Mid(0, pos);
	sours->Delete(0, pos + strlen(delim));
	return buf;
}

int find_buf_size(int size)
{
	if (size < 1) return 0;
	int back = size;
	while (size >= 10240) {
		int i = 2;
		while (size%i != 0) {
			i++;
		}
		back = size;
		size /= i;
	}
	if (size < 10240) return back;
	return size;
}