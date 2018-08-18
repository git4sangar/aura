//sgn
#ifndef SOAP_SELLER_H
#define SOAP_SELLER_H

#include <iostream>
#include <memory>
#include "SQLiteCpp/SQLiteCpp.h"
#include <tgbot/tgbot.h>

class SoapSeller {
	std::shared_ptr<SQLite::Database> m_hDB;
	static std::string STR_MSG_DEFERRED_RELEASE;
public:
	SoapSeller(std::string strDBFile);
	~SoapSeller() {};

	void onStartCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	void onViewCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	
};

#endif