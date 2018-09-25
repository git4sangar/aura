//sgn

#ifndef __VIEW_CART__
#define __VIEW_CART__

#include <iostream>
#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>
#include <AuraButton.h>

class ViewCart :
	public AuraButton,
	public std::enable_shared_from_this<ViewCart>  {
	std::string m_StrCart;
	bool m_IsCartEmpty;
public:
	ViewCart(DBInterface::Ptr hDB) : AuraButton(hDB) {m_IsCartEmpty = true;}
	virtual ~ViewCart() {}

	std::string getMsg() { return m_StrCart;}
	std::string getParseMode() {return "HTML";}
	
	TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}

	static std::string STR_BTN_EMPTY_CART;
	static std::string STR_BTN_PURCHASE;
};


#endif