#pragma once
#include <CString>

using namespace std;

CString firstSubStrDelim(CString* sours, CString startDelim, CString endDelim);

enum class TypeData
{
	FOLDER, FILE, UNCNOWN
};

TypeData strToTypeData(CString type);


class Data
{
public:
	Data() {};
	Data(CString name, CString path, TypeData t) :name(name), path(path), type(t) {}
	~Data() {}

	CString getName() { return name; }
	CString getPath() { return path; }
	TypeData getType() { return type; }

	void setName(CString name) { this->name = name; }
	void setPath(CString path) { this->path = path; }
	void setType(TypeData t) { this->type = t; };

	virtual CString getStrData();

	//virtual bool copyData();

private:
	CString name;
	CString path;
	TypeData type;
};


class File : Data
{
public:
	enum class Flag
	{
		COPY,CAT
	};
	File();
	File(CString sourse);
	File(CString name, CString path, int HV, int LV) 
		:Data(name, path, TypeData::FILE), hightVersion(HV), lowVersion(LV) {
		ID = lastId++ + 1;
	}
	~File() {}
	void strToFile(CString sourse);
	void setVersion(int hV, int lV)
	{ this->hightVersion = hV; this->lowVersion = lV; }
	void setPath(CString path) { Data::setPath(path); }
	void setId(int id) { ID = id; }
	bool setNewName(CString newMama);

	CString getStrData();
	CString getName() { return Data::getName(); }
	CString getPath() { return Data::getPath(); }
	int getId() { return ID; }

	bool copyFile(CString *pathSours, CString *pathDestination, Flag f);
	bool deleteFile();
	bool fileExist();
	
	static int lastId;
private:
	int hightVersion;
	int lowVersion;
	int ID;
	//Data modify
};

class Dir : Data
{
public:
	Dir() { countDir++;
		Data::setType(TypeData::FOLDER);
		if (ID > Dir::lastIdDir)
			Dir::lastIdDir = ID;
	}
	Dir(CString sours);
	Dir(CString name, CString path)
		:Data(name, path, TypeData::FOLDER) { countDir++;
	ID = ++Dir::lastIdDir;}
	~Dir() { countDir--; }

	void strToFolder(CString sourse);

	CString getStrData();
	CString getName() { return Data::getName(); }
	CString getPath() { return Data::getPath(); }
	int getId() { return ID; }

	void setPath(CString path) { Data::setPath(path); }

	bool create();
	
	void setId(int id) { ID = id; }
	bool deleteEmptyFolder();

	static int countDir;
	static int lastIdDir;
private:
	int ID;
};