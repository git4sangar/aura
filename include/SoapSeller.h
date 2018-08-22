//sgn
#ifndef SOAP_SELLER_H
#define SOAP_SELLER_H

#include <iostream>
#include <memory>
#include "SQLiteCpp/SQLiteCpp.h"
#include <tgbot/tgbot.h>

class SoapSeller {
	std::shared_ptr<SQLite::Database> m_hDB;
	std::vector<std::string> m_SoapNames;
public:
	SoapSeller(std::string strDBFile);
	~SoapSeller() {};

	void onStartCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	void onViewCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	void onViewSoaps(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	void onViewCart(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	void onViewOrders(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr getMainMenuKBoard();
	

	static std::string STR_MSG_DEFERRED_RELEASE;

	static std::string STR_TABLE_SOAP_COLUMN_NAME;
};

#endif