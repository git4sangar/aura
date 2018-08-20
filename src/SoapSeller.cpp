//sgn

#include <iostream>
#include <memory>
#include <ctime>
#include "SoapSeller.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include <SQLiteCpp/VariadicBind.h>
#include <tgbot/tgbot.h>
#include <vector>

#include "SoapSeller.h"

std::string SoapSeller::STR_MSG_DEFERRED_RELEASE =
	"I am the AuraSoap Bot & am getting ready now..!\nBear with me for \"just a few more days\",\nI'll intimate you...";

SoapSeller::SoapSeller(std::string strDBFile) {
	m_hDB = std::make_shared<SQLite::Database>(strDBFile, SQLite::OPEN_READWRITE);
}

void SoapSeller::onStartCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp) {
	fprintf(fp, "AURA: onStartCommand\n"); fflush(fp);
	SQLite::Statement   query(*m_hDB, "SELECT * FROM User WHERE chat_id = ?");
	query.bind(1, pMessage->chat->id);
	if(!query.executeStep()) {
		fprintf(fp, "AURA: New User %s\n", pMessage->from->firstName.c_str()); fflush(fp);
		char sql[2048];
		//	A new user. So add him / her to database
		SQLite::Transaction transaction(*m_hDB);
		sprintf(sql, "INSERT INTO User (first_name, last_name, chat_id) VALUES (\"%s\", \"%s\", %lld)", 
			pMessage->from->firstName.c_str(), pMessage->from->lastName.c_str(), pMessage->chat->id);
		int nb = m_hDB->exec(sql);
		transaction.commit();
	} else {
		fprintf(fp, "AURA: User %s alreay existing\n", pMessage->from->firstName.c_str()); fflush(fp);
	}
}

void SoapSeller::onViewCommand(std::shared_ptr<TgBot::Bot> pBot, TgBot::Message::Ptr pMessage, FILE *fp) {
	std::vector<TgBot::InlineKeyboardButton::Ptr> row0, row1, row2, row3;
	TgBot::InlineKeyboardButton::Ptr soapButton;

	soapButton = std::make_shared<TgBot::InlineKeyboardButton>();
    soapButton->text = "Sandal";
    soapButton->callbackData = "Sandal";
    row0.push_back(soapButton);

	soapButton = std::make_shared<TgBot::InlineKeyboardButton>();
    soapButton->text = "Lavendar";
    soapButton->callbackData = "Lavendar";
    row1.push_back(soapButton);

	soapButton = std::make_shared<TgBot::InlineKeyboardButton>();
    soapButton->text = "Turmeric";
    soapButton->callbackData = "Turmeric";
    row2.push_back(soapButton);

	soapButton = std::make_shared<TgBot::InlineKeyboardButton>();
    soapButton->text = "Nalangumavu";
    soapButton->callbackData = "Nalangumavu";
    row3.push_back(soapButton);

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    keyboard->inlineKeyboard.push_back(row0);
    keyboard->inlineKeyboard.push_back(row1);
    keyboard->inlineKeyboard.push_back(row2);
    keyboard->inlineKeyboard.push_back(row3);

	SQLite::Statement   query(*m_hDB, "SELECT * FROM User WHERE chat_id = ?");
	char sql[2048];
	SQLite::Transaction transaction(*m_hDB);
	int iTimeNow = std::time(nullptr);
	sprintf(sql, "INSERT INTO View (user_id, time) VALUES (%lld, %d)", pMessage->chat->id, iTimeNow);
	int nb = m_hDB->exec(sql);
	transaction.commit();

	pBot->getApi().sendMessage(pMessage->chat->id, "Tap on the following list to know more about each", true, 0, keyboard);
}