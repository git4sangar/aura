//sgn

#ifndef __ON_START_BUTTON__
#define __ON_START_BUTTON__

#include <iostream>
#include "AuraButton.h"

class StartButton : public AuraButton {
public:
	StartButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~StartButton() {}

	std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

	static std::string STR_BTN_VIEW_SOAPS;
	static std::string STR_BTN_VIEW_CART;
	static std::string STR_BTN_VIEW_ORDERS;
	static std::string STR_MSG_DEFF_RELEASE;
};


#endif