#include "stdafx.h"
#include "Folder.h"
#include <direct.h>
//#include <CString>
#include <WinBase.h>
#include <shellapi.h>


MyFolder::MyFolder()
{
	
}
MyFolder::MyFolder(char *path, char *name)
{
	this->name = name;
	this->path = path;
}

MyFolder::~MyFolder()
{
	delete[] name;
	delete[] path;
}

bool MyFolder::createFolder(CString *path)
{
	int result = CreateDirectory(path->GetBuffer(), NULL);
	if(result == ERROR_ALREADY_EXISTS||result == ERROR_PATH_NOT_FOUND)return false;
	return true;
}

bool MyFolder::createFolder()
{
	return false; //createFolder(path, name);
}

bool MyFolder::reNameFolder(char *path, char *newName, char *oldName)
{
	char *oldDir = new char(strlen(path) + strlen(oldName) + 2);
	char *newDir = new char(strlen(path) + strlen(newName) + 2);
	strcat_s(oldDir, sizeof(path), path);
	strcat_s(oldDir, sizeof(oldName), oldName);

	strcat_s(newDir,sizeof(path), path);
	strcat_s(newDir, sizeof (newName), newName);
	
	bool result = MoveFile((LPCWSTR)oldDir, (LPCWSTR)newDir);
	
	delete[] oldDir;
	delete[] newDir;

	return result;
}

bool MyFolder::reNameFolder( char *newName)
{
	if (reNameFolder(path, newName, name))
	{
		if (name)delete[] name;
		name = new char(strlen(newName) + 2);
		strcat_s(name, sizeof(newName), newName);
		return true;
	}
	return false;
}

bool MyFolder::deleteFolder(char *path, char *name)
{
	/*
	RemoveDirectoryA(path);
	SHFILEOPSTRUCT f;
	f.wFunc = FO_DELETE;
	f.pFrom = path;
	
	SHFileOperation(&f);
*/
	return false;
}

bool MyFolder::deleteFolder()
{
	return deleteFolder();
}

/*auto size = ((strlen(path) + strlen(name)) * sizeof(char)) + 2;
	char *dir = new char[size];
	if (strlen(path)&& strlen(name))
	{
		strcpy_s(dir, size, path);
		strcat_s(dir, size, name);
	}
	else if (!strlen(path) && strlen(name))
	{
		strcpy_s(dir, size, name);
	}
	else
	{
		return false;
	}
	int result = _mkdir(dir);
	delete[] dir;
	if(result == ENOENT)return false;
	return true;*/