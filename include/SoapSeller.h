//sgn
#ifndef SOAP_SELLER_H
#define SOAP_SELLER_H

#include <iostream>
#include <memory>
#include "SQLiteCpp/SQLiteCpp.h"
#include <tgbot/tgbot.h>

class SoapSeller {
	std::shared_ptr<SQLite::Database> m_hDB;
public:
	SoapSeller(std::string strDBFile);
	~SoapSeller() {};

	void onStartCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage);
	void onViewCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage);
	
};

#endif