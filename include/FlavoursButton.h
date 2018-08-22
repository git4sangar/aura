//sgn

#ifndef __FLAVOUR_BUTTON__
#define __FLAVOUR_BUTTON__

#include <iostream>
#include <AuraButton.h>

class FlavoursButton : public AuraButton {

public:
	FlavoursButton(std::shared_ptr<SQLite::Database> hDB) : AuraButton(hDB) {}
	virtual ~FlavoursButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif