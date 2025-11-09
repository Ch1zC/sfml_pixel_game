#pragma once

#include <map>
#include <iostream>

class Item_Database {

	struct itemInfo
	{
		std::string content;
		bool is_solid;
	};

public:
	Item_Database();
	const itemInfo get_itemInfoByID(std::string i) { return itemMap.count(i) ? itemMap[i] : itemInfo{}; }

private:
	std::map<std::string, itemInfo> itemMap;
};