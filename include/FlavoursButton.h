//sgn

#ifndef __FLAVOUR_BUTTON__
#define __FLAVOUR_BUTTON__

#include <iostream>
#include <AuraButton.h>
#include <DBInterface.h>

class FlavoursButton : public AuraButton, public std::enable_shared_from_this<FlavoursButton> {

public:
	FlavoursButton(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) {}
	virtual ~FlavoursButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
};


#endif