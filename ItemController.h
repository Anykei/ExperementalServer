#pragma once
#include "TreeData.h"
#include <mutex>
#include <Vector>

class ItemList
{
public:
	ItemList();
	~ItemList();

	bool addItem(HTREEITEM hItem, Data *item);

	Data *findItemId(int id, TypeData findType);
	HTREEITEM findPosOnTree(int id, TypeData findType);
	Data *findItemName(CString name);
	Data *getData(HTREEITEM hItem);
	
	vector<HTREEITEM> fileExistList();

	bool removeItem(HTREEITEM rItem);

	mutex lockModifyList;
private:
	HTREEITEM findFolder(int id);
	HTREEITEM findFile(int id);

	CMap<HTREEITEM, HTREEITEM, Data*, Data*> itemList;
};

extern ItemList *IL;