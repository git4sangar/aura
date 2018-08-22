//sgn

#ifndef __AURA_BUTTON__
#define __AURA_BUTTON__

#include <iostream>
#include <stdio.h>
#include <memory>
#include <tgbot/tgbot.h>
#include "AuraButton.h"
#include "SQLiteCpp/SQLiteCpp.h"

class AuraButton {
protected:
	std::shared_ptr<SQLite::Database> m_hDB;
public:
	AuraButton(std::shared_ptr<SQLite::Database> hDB) : m_hDB{hDB} {}
	~AuraButton() {}
	std::shared_ptr<SQLite::Database> getDBHandle() {return m_hDB;}

	virtual std::string getMsg() = 0;
	virtual TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) = 0;
	virtual void onClick(TgBot::Message::Ptr pMessage, FILE *fp) = 0;
};


#endif