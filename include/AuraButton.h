//sgn

#ifndef __AURA_BUTTON__
#define __AURA_BUTTON__

#include <iostream>
#include <stdio.h>
#include <memory>

#include <tgbot/tgbot.h>
#include <AuraButton.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <DBInterface.h>
#include <tgbot/tgbot.h>

class AuraButton {
protected:
	DBInterface::Ptr m_hDB;
public:
	AuraButton(std::shared_ptr<DBInterface> hDB) : m_hDB{hDB} {}
	~AuraButton() {}
	DBInterface::Ptr getDBHandle() {return m_hDB;}

	std::vector<TgBot::KeyboardButton::Ptr> getMainMenu() {
		std::vector<TgBot::KeyboardButton::Ptr> row;
		TgBot::KeyboardButton::Ptr kbBtnMMenu;
		kbBtnMMenu 			= std::make_shared<TgBot::KeyboardButton>();
		kbBtnMMenu->text	= "Main Menu";
		row.push_back(kbBtnMMenu);
		return row;
	}

	virtual void init(TgBot::Message::Ptr pMsg, FILE *fp) {}
	virtual std::string getNotifyStr(unsigned int userParam) {return std::string();}
	virtual std::string getNotifyStrForCustomer() {return std::string();}

	virtual std::string getParseMode() {return std::string();}
	virtual TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) = 0;
	virtual TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {return nullptr;}
	virtual unsigned int getChatIdForNotification(TgBot::Message::Ptr pMessage, FILE *fp) {return 0;}
	std::vector<unsigned int> getNotifyUsers() {return m_hDB->getNotifyUsers();}

	virtual void onClick(TgBot::Message::Ptr pMessage, FILE *fp) = 0;
	virtual std::string getMsg() = 0;
	virtual std::shared_ptr<AuraButton> getSharedPtr() = 0;
};


#endif