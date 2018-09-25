//sgn

#ifndef __ORDERS_BUTTON__
#define __ORDERS_BUTTON__

#include <iostream>
#include <AuraButton.h>

#include <DBInterface.h>

class OrdersButton : public AuraButton,
					public std::enable_shared_from_this<OrdersButton> {
	std::string m_StrOrder;
	std::vector<POrder::Ptr> m_Orders;
	std::map<std::string, POrder::Ptr> m_VwOrders, m_CnclOrders;
	std::string getOrderString(DBInterface::Ptr hDB, POrder::Ptr pOrder);
public:
	OrdersButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~OrdersButton() {}

	std::string getMsg() { return m_StrOrder;}
	std::string getParseMode() {return "HTML";}
	TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
};


#endif