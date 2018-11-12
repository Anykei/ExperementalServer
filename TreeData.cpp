#include "stdafx.h"
#include "TreeData.h"
#include "Folder.h"

CString Data::getStrData()
{
	CString data;
	if (type == TypeData::FILE)
	{
		data = "type=File;";
	}
	else
	{
		data = "type=Folder;";
	}

	data += L"path=" + path + L";";
	data += L"name=" + name + L";";
	return data;
}

File::File() { ID = lastId++ + 1; }

File::File(CString sourse)
{
	strToFile(sourse);
}

void File::strToFile(CString sourse)
{
	CString path, name;
	if (sourse.Find(L"path") >= 0)
	{
		path = firstSubStrDelim(&sourse, L"=", L";");
	}
	else
	{
		path = "";
	}
	if (sourse.Find(L"name") >= 0)
	{
		name = firstSubStrDelim(&sourse, L"=", L";");
	}
	else
	{
		name = L"New file";
	}
	Data::setName(name);
	Data::setPath(path);
	if (sourse.Find(L"hightVersion") >= 0)
	{
		hightVersion = _wtoi(firstSubStrDelim(&sourse, L"=", L";"));
	}
	else
	{
		hightVersion = 0;
	}
	if (sourse.Find(L"lowVersion") >= 0)
	{
		lowVersion = _wtoi(firstSubStrDelim(&sourse, L"=", L";"));
	}
	else
	{
		lowVersion = 0;
	}
	if (sourse.Find(L"ID") >= 0)
	{
		ID = _wtoi(firstSubStrDelim(&sourse, L"=", L";"));
	}
	else
	{
		ID = 0;
	}
	Data::setType(TypeData::FILE);
}

CString File::getStrData()
{
	CString data;
	CString hV, lV, iD;
	hV.Format(L"%d", hightVersion);
	lV.Format(L"%d", lowVersion);
	iD.Format(L"%d", ID);
	data = Data::getStrData();
	data += L"hightVersion=" + lV +  L";";
	data += L"lowVersion=" + lV  + L";";
	data += L"ID=" + iD + L";";
	data += "\n";
	CharToOemW(data, (LPSTR)data.GetBuffer());
	return data;
}

bool File::copyFile(CString *pathSours, CString *pathDestination, Flag f)
{
	if (f != Flag::CAT)
		if (CopyFile(pathSours->GetBuffer(), pathDestination->GetBuffer(), false) != 0)return true;
	else if (CopyFile(pathSours->GetBuffer(), pathDestination->GetBuffer(), true) != 0)return true;
	auto error = GetLastError();
	return false;
}

bool File::deleteFile()
{
	return DeleteFile(getPath() + getName());
}

bool File::fileExist()
{
	WIN32_FIND_DATA wfd;
	bool result = false;
	auto name = getPath() + getName();
	HANDLE hVal = FindFirstFile(name, &wfd);
	if (hVal != INVALID_HANDLE_VALUE)
		result = true;
	FindClose(hVal);
	return result;
}

Dir::Dir(CString sours)
{
	strToFolder(sours);
	Data::setType(TypeData::FOLDER);
}

void Dir::strToFolder(CString sourse)
{
	CString path, name;
	if (sourse.Find(L"path") >= 0)
	{
		path = firstSubStrDelim(&sourse, L"=", L";");
	}
	else
	{
		path = "";
	}
	if (sourse.Find(L"name") >= 0)
	{
		name = firstSubStrDelim(&sourse, L"=", L";");
	}
	else
	{
		name = "New Folder";
	}
	Data::setName(name);
	Data::setPath(path);
	if (sourse.Find(L"ID") >= 0)
	{
		ID = _wtoi(firstSubStrDelim(&sourse, L"=", L";"));
	}
	else
	{
		ID = 0;
	}
}

CString Dir::getStrData()
{
	CString data, id;
	id.Format(L"%d", ID);
	data = Data::getStrData();
	data += L"ID="+ id + L";";
	data += "\n";
	return data;
}

CString firstSubStrDelim(CString* sours, CString startDelim, CString endDelim)
{
	sours->Delete(0, sours->Find(startDelim) + 1);
	int ePos = sours->Find(endDelim);
	auto subStr = sours->Mid(0, ePos);
	sours->Delete(0, ePos);
	return subStr;
}

bool Dir::create()
{
	if (MyFolder::createFolder( &(Data::getPath() + Data::getName()))) return true;
	else Data::setPath(L"");
	return false;
}

bool Dir::deleteEmptyFolder()
{
	auto path = getPath() + getName();
	if (!RemoveDirectory(path)) {
		auto error = GetLastError();
		return false;
	}
	return true;
}

TypeData strToTypeData(CString type)
{
	if (type == L"Folder") return TypeData::FOLDER;
	if (type == L"File") return TypeData::FILE;
	return TypeData::UNCNOWN;
}
