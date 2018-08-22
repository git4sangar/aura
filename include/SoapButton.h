//sgn

#ifndef __SOAP_BUTTON__
#define __SOAP_BUTTON__

#include <iostream>
#include <AuraButton.h>

class SoapButton : public AuraButton {
public:
	SoapButton(std::shared_ptr<SQLite::Database> hDB) : AuraButton(hDB) {}
	virtual ~SoapButton() {}

	std::string getMsg() { return std::string();}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>> listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif