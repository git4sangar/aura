//sgn

#ifndef __ORDERS_BUTTON__
#define __ORDERS_BUTTON__

#include <iostream>
#include <AuraButton.h>

#include <DBInterface.h>

class OrdersButton : public AuraButton {
	std::string m_StrOrder;
	std::vector<POrder::Ptr> m_Orders;
public:
	OrdersButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~OrdersButton() {}

	std::string getMsg() { return m_StrOrder;}
	std::string getParseMode() {return "HTML";}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif