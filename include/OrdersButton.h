//sgn

#ifndef __ORDERS_BUTTON__
#define __ORDERS_BUTTON__

#include <iostream>
#include <AuraButton.h>

class OrdersButton : public AuraButton {

public:
	OrdersButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~OrdersButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif