//sgn

#ifndef __DB_NAMES_H__
#define __DB_NAMES_H__

#include <iostream>
#include <vector>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>

class Soap {
public:
	typedef std::shared_ptr<Soap> Ptr;
	int m_Soap_id, m_Weight, m_Price, m_PicId, m_Stock;
	std::string m_Name, m_Desc;
};

class DBInterface {
	std::shared_ptr<SQLite::Database> m_hDB;
	FILE *m_Fp;

public:
	typedef std::shared_ptr<DBInterface> Ptr;

	DBInterface(std::string dbFileName, FILE *fp);
	~DBInterface();

	Soap::Ptr getSoapByName(std::string name);
	std::vector<std::string> getFlavours();
	bool addNewUser(
			int64_t chatId,
			std::string fname,
			std::string lname = "",
			int64_t mobile = 0,
			std::string address = "",
			std::string email = ""
		);

	static std::string DB_TABLE_SOAP_COLUMN_ID;
	static std::string DB_TABLE_SOAP_COLUMN_NAME;
	static std::string DB_TABLE_SOAP_COLUMN_WEIGHT;
	static std::string DB_TABLE_SOAP_COLUMN_DESC;
	static std::string DB_TABLE_SOAP_COLUMN_PIC_ID;
	static std::string DB_TABLE_SOAP_COLUMN_STOCK;
	static std::string DB_TABLE_SOAP_COLUMN_PRICE;
};

#endif
