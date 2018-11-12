#include "stdafx.h"
#include "ItemController.h"

ItemList *IL = new ItemList();

ItemList::ItemList()
{
}

ItemList::~ItemList()
{
	while (auto d = itemList.PGetFirstAssoc())
	{
		delete d->value;
		itemList.RemoveKey(d->key);
	}
	itemList.RemoveAll();
}

bool ItemList::addItem(HTREEITEM hItem, Data *item)
{
	lock_guard<mutex> lock(lockModifyList);
	if (itemList[hItem])return false;
	else itemList[hItem] = item;
	return true;
}

Data *ItemList::findItemId(int id, TypeData findType)
{
	if (id == 0) return nullptr;
	if (findType == TypeData::FILE)
	{
		return itemList[findFile(id)];
	}
	else if (findType == TypeData::FOLDER)
	{
		return itemList[findFolder(id)];
	}
	else { return nullptr; }
}

HTREEITEM ItemList::findPosOnTree(int id, TypeData findType)
{
	if (id == 0) return nullptr;
	if (findType == TypeData::FILE)
	{
		return findFile(id);
	}
	else if (findType == TypeData::FOLDER)
	{
		return findFolder(id);
	}
	else { return nullptr; }
}

Data *ItemList::findItemName(CString name)
{
	return new Data();
}

Data *ItemList::getData(HTREEITEM hItem)
{
	return itemList[hItem];
}

bool ItemList::removeItem(HTREEITEM rItem)
{
	lock_guard<mutex> lock(lockModifyList);
	auto *item = itemList[rItem];
	if (item)
	{
		if (item->getType() == TypeData::FILE) {
			File *f = (File*)item;
			if (f->deleteFile() || !f->fileExist())
				itemList.RemoveKey(rItem);
			else return false;
		}
		else if (item->getType() == TypeData::FOLDER)
		{
			Dir *d = (Dir*)item;
			if (d->deleteEmptyFolder())
				itemList.RemoveKey(rItem);
			else return false;
		}
		else
		{
			return false;
		}
	}
	delete item;
	return true;	
}

HTREEITEM ItemList::findFolder(int id)
{
	while (auto d = itemList.PGetFirstAssoc())
	{
		Dir *dir = (Dir*)d->value;
		if (d->value->getType() == TypeData::FOLDER && dir->getId() == id)
			return d->key;
	}
	return nullptr;
}

HTREEITEM ItemList::findFile(int id)
{
	while (auto item = itemList.PGetFirstAssoc())
	{
		File *file = (File*)item->value;
		if (item->value->getType() == TypeData::FILE && file->getId() == id)
			return item->key;
	}
	return nullptr;
}

vector<HTREEITEM> ItemList::fileExistList()
{
	vector<HTREEITEM> lostFiles;
	while (auto item = itemList.PGetFirstAssoc())
	{
		File *file = (File*)item->value;
		if (item->value->getType() == TypeData::FILE && !file->fileExist())
			lostFiles.push_back(item->key);
	}
	return lostFiles;
}