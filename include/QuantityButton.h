//SGN

#ifndef __QUANTITY_BUTTON_H__
#define __QUANTITY_BUTTON_H__

#include <iostream>
#include <memory>

#include <AuraButton.h>
#include <DBInterface.h>

class QuantityButton : public AuraButton {

public:
	typedef std::shared_ptr<QuantityButton> Ptr;
	QuantityButton(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) {}
	virtual ~QuantityButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};



#endif