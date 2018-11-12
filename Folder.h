#pragma once
#include <CString>

class MyFolder
{
public:
	MyFolder();
	MyFolder( char *path, char *name);
	~MyFolder();
	static bool createFolder(CString *path);
	bool createFolder();
	static bool reNameFolder(char *path, char *newName, char *oldName);
	bool reNameFolder(char *newName);
	static bool deleteFolder(char *path, char *name);
	bool deleteFolder();
private:
	char *path;
	char *name;
};
