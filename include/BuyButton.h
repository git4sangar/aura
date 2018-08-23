//sgn

#ifndef __BUY_BUTTON__
#define __BUY_BUTTON__

#include <iostream>
#include <AuraButton.h>
#include <DBInterface.h>

class BuyButton : public AuraButton {
	std::string flvrSelected;
	static int m_MaxNos;
	static int m_NosRow;

public:
	BuyButton(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) {}
	virtual ~BuyButton() {}

	std::string getMsg() { return "Choose quantity";}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif