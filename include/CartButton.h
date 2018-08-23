//sgn

#ifndef __CART_BUTTON__
#define __CART_BUTTON__

#include <iostream>
#include "AuraButton.h"
#include <DBInterface.h>

class CartButton : public AuraButton {
public:
	CartButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~CartButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif