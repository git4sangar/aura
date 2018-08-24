//sgn

#include <iostream>
#include <DBInterface.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>

	std::string DBInterface::DB_TABLE_SOAP_COLUMN_ID		= "soap_id";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_NAME		= "name";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_WEIGHT	= "weight";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_DESC		= "description";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_PIC_ID	= "pic_id";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_STOCK		= "stock";
	std::string DBInterface::DB_TABLE_SOAP_COLUMN_PRICE		= "price";

DBInterface::DBInterface(std::string dbFileName, FILE *fp) {
	m_hDB   = std::make_shared<SQLite::Database>(dbFileName, SQLite::OPEN_READWRITE);
	m_Fp	= fp;
}

DBInterface::~DBInterface() {}

Soap::Ptr DBInterface::getSoapByName(std::string name) {
	return nullptr;
}

std::vector<Soap::Ptr> DBInterface::getFlavours() {
	std::vector<Soap::Ptr> soaps;
	Soap::Ptr soap;

	SQLite::Statement   query(*m_hDB, "SELECT * FROM Soap;");
	while(query.executeStep()) {
		soap = std::make_shared<Soap>();
		soap->m_SoapId		= query.getColumn(DB_TABLE_SOAP_COLUMN_ID.c_str()).getUInt();
		soap->m_Weight		= query.getColumn(DB_TABLE_SOAP_COLUMN_WEIGHT.c_str()).getUInt();
		soap->m_Price		= query.getColumn(DB_TABLE_SOAP_COLUMN_PRICE.c_str()).getUInt();
		soap->m_PicId		= query.getColumn(DB_TABLE_SOAP_COLUMN_PIC_ID.c_str()).getUInt();
		soap->m_Stock		= query.getColumn(DB_TABLE_SOAP_COLUMN_STOCK.c_str()).getUInt();
		soap->m_Name		= query.getColumn(DB_TABLE_SOAP_COLUMN_NAME.c_str()).getString();
		soap->m_Desc		= query.getColumn(DB_TABLE_SOAP_COLUMN_DESC.c_str()).getString();
		soaps.push_back(soap);
	}
	return soaps;
}

/*std::vector<std::string> DBInterface::getFlavours() {
	int		iColNo		= 0;
	bool	isColSet	= false;
	std::vector<std::string> flvrNames;

	SQLite::Statement   query(*m_hDB, "SELECT * FROM Soap;");
	while(query.executeStep()) {
		//	First find the column number for Soap Names
		while(!isColSet && iColNo < query.getColumnCount()) {
			if(DB_TABLE_SOAP_COLUMN_NAME == query.getColumnName(iColNo)) {
				isColSet = true;
				break;
			}
			iColNo++;
		}
		if(query.getColumnCount() > iColNo) {
			const std::string flavour = query.getColumn(iColNo);
			fprintf(m_Fp, "AURA: Queried a flavour \"%s\" from Soap Table\n", flavour.c_str()); fflush(m_Fp);
			flvrNames.push_back(flavour);
		}
	}
	return flvrNames;
}*/


bool DBInterface::addNewUser(int64_t chatId, std::string fname, std::string lname, int64_t mobile, std::string address, std::string email) {
	SQLite::Statement   query(*m_hDB, "SELECT * FROM User WHERE chat_id = ?");
	long long llid = static_cast<long long>(chatId);
	query.bind(1, llid);
	if(!query.executeStep()) {
		fprintf(m_Fp, "AURA: New User %s\n", fname.c_str()); fflush(m_Fp);
		char sql[2048];
		//	A new user. So add him / her to database
		SQLite::Transaction transaction(*m_hDB);
		sprintf(sql, "INSERT INTO User (first_name, last_name, chat_id) VALUES (\"%s\", \"%s\", %ld)", 
			fname.c_str(), lname.c_str(), chatId);
		int nb = m_hDB->exec(sql);
		transaction.commit();
	} else {
		fprintf(m_Fp, "AURA: User %s alreay existing\n", fname.c_str()); fflush(m_Fp);
		return false;
	}
	return true;
}